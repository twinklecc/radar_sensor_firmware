#include "can_signal_interface.h"
#include "baseband.h"
#include "semphr.h"
#include "dbg_gpio_reg.h"
#include "baseband_task.h"
#include "sensor_config.h"
#include "embARC.h"
#include "can_hal.h"
#include "can_cli.h"
#include "can_obj.h"
#include "baseband_reg.h"
#include "dev_can.h"
#include "baseband_cli.h"

extern SemaphoreHandle_t mutex_frame_count;
extern int32_t frame_count;
extern int32_t initial_flag;
extern SemaphoreHandle_t mutex_initial_flag;

/* controlling the bk and ak data output */
static bool output_bk_id0 = false;
static bool output_bk_id1 = false;
static bool output_bk_id2 = false;    /* Reserved */

static bool output_ak_id0 = false;
static bool output_ak_id1 = false;
static bool output_ak_id2 = false;    /* Reserved */

static bool scan_sample_on = false;

#define SCAN_START                      1
#define SCAN_NO_STREAM                  0b00            /* No using the stream param */
#define SCAN_STREAM_ON                  0b01
#define SCAN_STREAM_OFF                 0b10
#define SCAN_NO_SAMPLE                  0b000           /* No using the sample type param */
#define SCAN_SAMPLE_ADC                 0b001
#define SCAN_SAMPLE_FFT1D               0b010
#define SCAN_SAMPLE_FFT2D               0b011
#define SCAN_SAMPLE                     0b100
#define SCAN_SAMPLE_BPM                 0b101
#define SYS_ENA(MASK, var) (var << SYS_ENABLE_##MASK##_SHIFT)
#define BB_WRITE_REG(bb_hw, RN, val) baseband_write_reg(bb_hw, BB_REG_##RN, val)

/* The number of arrays required for a CAN frame*/
#define FRAME_ARRAY_NUM                 (4)

static uint32_t track_data[FRAME_ARRAY_NUM] = {0};

static int32_t can_ota_handler(uint8_t *data, uint32_t len);
static int32_t can_ota_comm_handshake(uint8_t *data, uint32_t len);

int32_t can_scan_signal(uint8_t *data, uint32_t len)
{
        int32_t param1 = -1, param2 = -1, param3 = -1, param4 = -1;
        bool dmp_mid_enable = false;
        bool dmp_fnl_enable = false;
        bool fft1d_enable   = false;
        int32_t count = 0;
        bool stream_on_en = false;

        /* getting the output data config of bk and ak */
        output_bk_id0 = (data[3] & 0x1);                /* ID 0x400 */
        output_bk_id1 = ((data[3] >> 1) & 0x1);         /* ID 0x401 */
        output_bk_id2 = ((data[3] >> 2) & 0x1);         /* ID 0x402 */
        output_ak_id0 = ((data[3] >> 3) & 0x1);         /* ID 0x500 */
        output_ak_id1 = ((data[3] >> 4) & 0x1);         /* ID 0x501 */
        output_ak_id2 = ((data[3] >> 5) & 0x1);         /* ID 0x502 */

        param1 = data[0] & 0x1;                         /* Start/Stop:1/0 */
        param2 = (data[1] | (data[2] << 8)) & 0xFFFF;   /* Number of Frame */
        param3 = (data[0] >> 1) & 0x3;                  /* Stream_on/Stream_off:0b01/0b10 */
        param4 = (data[0] >> 3) & 0x7;                  /* Sample_Type: adc/fft1/fft2/cfar/bpm */

        if (param1 == SCAN_START) {
                baseband_frame_type_reset();
        } else {
                /* scan stop */
                bb_clk_restore(); /* restore when stoped by commond */
                xSemaphoreTake(mutex_frame_count, portMAX_DELAY);
                frame_count = 0;
                xSemaphoreGive(mutex_frame_count);
                return E_OK;
        }

        if (param2 == 0)
                count = -1;
        else
                count = param2;

        if (param3 != SCAN_NO_STREAM) {
                if (param3 == SCAN_STREAM_ON){
                        stream_on_en = true;
                } else if (param3 == SCAN_STREAM_OFF){
                        dbgbus_dump_stop();
                        baseband_switch_buf_store(NULL, SYS_BUF_STORE_FFT);
                } else {
                        return E_PAR;
                }
        } else {
                baseband_switch_buf_store(NULL, SYS_BUF_STORE_FFT);
        }

        if ((param4 != SCAN_NO_SAMPLE) && (stream_on_en == true)) {
                scan_sample_on = true;
                if (param4 == SCAN_SAMPLE_ADC) {
                        dbgbus_dump_start(DBG_SRC_DUMP_WO_SYNC);
                        dmp_mid_enable = true;
                        fft1d_enable   = true;
                        baseband_switch_buf_store(NULL, SYS_BUF_STORE_ADC);
                } else if (param4 == SCAN_SAMPLE_FFT1D) {
                        dbgbus_dump_start(DBG_SRC_DUMP_WO_SYNC);
                        dmp_mid_enable = true;
                        baseband_switch_buf_store(NULL, SYS_BUF_STORE_FFT);
                } else if (param4 == SCAN_SAMPLE_FFT2D) {
                        dbgbus_dump_start(DBG_SRC_DUMP_WO_SYNC);
                        dmp_fnl_enable = true;
                } else if (param4 == SCAN_SAMPLE) {
                        dbgbus_dump_start(DBG_SRC_SAM); /* config sample debug data to GPIO */
                        bb_clk_switch(); /* debug data has no buffer, bb clock should be switched to dbgbus clock */
                } else {
                        stream_on_en = false;
                        return E_PAR;
                }
        }

        /* config baseband status enable */
        uint16_t bb_status_cfg =  SYS_ENA(SAM    , true          )
                                 |SYS_ENA(DMP_MID, dmp_mid_enable)
                                 |SYS_ENA(FFT_1D , fft1d_enable  )
                                 |SYS_ENA(FFT_2D , true          )
                                 |SYS_ENA(CFR    , true          )
                                 |SYS_ENA(BFM    , true          )
                                 |SYS_ENA(DMP_FNL, dmp_fnl_enable);
        uint16_t bb_agc_en;
        uint16_t bb_status_en;
        for (int i = 0; i < NUM_FRAME_TYPE; i++) {
                baseband_hw_t *bb_hw = &baseband_get_bb(i)->bb_hw;
                sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(bb_hw, baseband_t, bb_hw)->cfg;
		uint8_t agc_m = cfg->agc_mode == 0 ? 0: 1;
                bb_agc_en            =  (agc_m << SYS_ENABLE_AGC_SHIFT);
                bb_status_en         = bb_status_cfg | bb_agc_en;
                BB_WRITE_REG(bb_hw, SYS_BNK_ACT, i);    /* switch the bank for cpu access*/
                BB_WRITE_REG(bb_hw, SYS_ENABLE, bb_status_en);
        }
        /* change initial flag */
        xSemaphoreTake(mutex_initial_flag, portMAX_DELAY);
        initial_flag = 1;
        xSemaphoreGive(mutex_initial_flag);
        /* change frame number */
        xSemaphoreTake(mutex_frame_count, portMAX_DELAY);
        frame_count = count;
        xSemaphoreGive(mutex_frame_count);

        return E_OK;
}

void track_pkg_can_print(track_t *track)
{
        /* variable */
        uint16_t i = 0;

        void* sys_params = track->radar_params[baseband_get_cur_frame_type()];

        if (track->trk_update_header)
                track->trk_update_header(track->trk_data, sys_params);

        /* Head Data - 3 WORD Length : From track_data[0] to track_data[3] */
        /* track_data[0] - ID 0x300 Byte0 -> Byte3 */
        /* track_data[1] - ID 0x300 Byte4 -> Byte7 */
        /* track_data[2] - ID 0x301 Byte0 -> Byte3 */
        /* track_data[3] - ID 0x301 Byte4 -> Byte7 */
        track_data[0] = (((uint32_t)((track->output_hdr.frame_int * 100) + ROUNDF) & LOW10BITMASk)
                        | ((track->output_hdr.frame_id & LOW16BITMASk) << 16));
        track_data[1] = (((track->output_hdr.frame_id >> 16) & LOW16BITMASk)
                        | ((track->cdi_pkg.cdi_number & LOW10BITMASk) << 16));
        track_data[2] = ((track->output_hdr.track_output_number & LOW10BITMASk)
                        | ((track->cdi_pkg.raw_number & LOW10BITMASk) << 16));
        track_data[3] = 0x0;                              /* Reserved */

#if (USE_CAN_FD == 1)
        /* If use CAN FD, data length can be 16 Bytes at one time */
        /* Send a frame data of Head to CAN bus */
        can_send_data(CAN_0_ID, HEAD_FRAME_ID0, &track_data[0], eDATA_LEN_16);
#else
        /* If use CAN, data length can only be 8 Bytes at one time */
        /* Send a frame data of Head to CAN bus */
        can_send_data(CAN_0_ID, HEAD_FRAME_ID0, &track_data[0], eDATA_LEN_8);
        can_send_data(CAN_0_ID, HEAD_FRAME_ID1, &track_data[2], eDATA_LEN_8);
#endif

        /* BK Data */
        for (i = 0; i < track->cdi_pkg.cdi_number; i++) {
                float tmpS = track->cdi_pkg.cdi[i].raw_z.sig;
                float tmpN = track->cdi_pkg.cdi[i].raw_z.noi;
                float SNR = 10*log10f(tmpS/tmpN);

                track_data[0] = ((i & LOW10BITMASk)
                                | (((uint32_t)((track->cdi_pkg.cdi[i].raw_z.rng * 100) + ROUNDF)) & LOW16BITMASk) << 16);
                track_data[1] = (((((uint32_t)((track->cdi_pkg.cdi[i].raw_z.rng * 100) + ROUNDF)) >> 16) & LOW16BITMASk)
                                | ((transform_data(track->cdi_pkg.cdi[i].raw_z.vel) & LOW15BITMASk) << 16));
                track_data[2] = ((transform_data(SNR) & LOW15BITMASk)
                                | ((transform_data(track->cdi_pkg.cdi[i].raw_z.ang) & LOW15BITMASk) << 16));
#if (TRK_CONF_3D == 0)
                track_data[3] = 0x4000;
#else
                track_data[3] = transform_data(track->cdi_pkg.cdi[i].raw_z.ang_elv) & LOW15BITMASk;
#endif

#if (USE_CAN_FD == 1)
                /* If use CAN FD, data length can be 16 Bytes at one time */
                /* Send a frame data of BK to CAN bus */
                if (output_bk_id0) {
                        can_send_data(CAN_0_ID, BK_FRAME_ID0, &track_data[0], eDATA_LEN_16);
                }
#else
                /* If use CAN, data length can only be 8 Bytes at one time */
                /* Send a frame data of BK to CAN bus */
                if (output_bk_id0) {
                        can_send_data(CAN_0_ID, BK_FRAME_ID0, &track_data[0], eDATA_LEN_8);
                }
                if (output_bk_id1) {
                        can_send_data(CAN_0_ID, BK_FRAME_ID1, &track_data[2], eDATA_LEN_8);
                }
#endif
        }

        /* AK Data */
        for (i = 0; i < TRACK_NUM_TRK; i++) {
                if (track->trk_update_obj_info)
                        track->trk_update_obj_info(track->trk_data, sys_params, i);
                if (track->output_obj.output) {
                        track_data[0] = ((i & LOW10BITMASk)
                                        | ((track->output_obj.track_level & LOW2BITMASk) << 10)
                                        | ((((uint32_t)((track->output_obj.rng  * 100) + ROUNDF)) & LOW16BITMASk) << 16)); 
                        track_data[1] = (((((uint32_t)((track->output_obj.rng  * 100) + ROUNDF)) >> 16)& LOW16BITMASk)
                                        | ((transform_data(track->output_obj.vel) & LOW15BITMASk) << 16));
                        track_data[2] = ((transform_data(track->output_obj.SNR) & LOW15BITMASk)
                                                  | (transform_data(track->output_obj.ang) & LOW15BITMASk) << 16);
#if (TRK_CONF_3D == 0)
                        track_data[3] = 0x4000;
#else
                        track_data[3] = transform_data(track->output_obj.ang_elv) & LOW15BITMASk;
#endif

#if (USE_CAN_FD == 1)
                       /* If use CAN FD, data length can be 16 Bytes at one time */
                       /* Send a frame data of AK to CAN bus */
                       if (output_ak_id0) {
                                can_send_data(CAN_0_ID, AK_FRAME_ID0, &track_data[0], eDATA_LEN_16);
                       }
#else
                       /* If use CAN, data length can only be 8 Bytes at one time */
                       /* Send a frame data of AK to CAN bus */
                       if (output_ak_id0) {
                                can_send_data(CAN_0_ID, AK_FRAME_ID0, &track_data[0], eDATA_LEN_8);
                       }
                       if (output_ak_id1) {
                                can_send_data(CAN_0_ID, AK_FRAME_ID1, &track_data[2], eDATA_LEN_8);
                       }
#endif
                }
        }
}

static int32_t can_ota_handler(uint8_t *data, uint32_t len)
{
        int32_t result = E_OK;

        result = can_ota_comm_handshake(data, len);
        
        if (E_OK == result) {
                reboot_cause_set(ECU_REBOOT_CAN_OTA);

                chip_hw_mdelay(1);

                chip_reset();
        }
        return  result;
}

static int32_t can_ota_comm_handshake(uint8_t *data, uint32_t len)
{
        int32_t result = E_OK;
        uint32_t ota_comm_data[2] = {0};

        transfer_word_stream(data, ota_comm_data, len);

        if ((CAN_OTA_COM_MAGIC_NUM == ota_comm_data[0]) && \
            (CAN_OTA_COM_HS_CODE == ota_comm_data[1])) {
                /* If the received "Magic number" and "HS code" are equal to the setting value,
                    return the received value to Host */
                can_send_data(CAN_0_ID, CAN_OTA_ID, ota_comm_data, len);
        } else {
                result = E_NODATA;
        }

        return result;
}

int32_t can_cli_commands(void)
{
        int32_t result = E_OK;

        /* register scan command */
        result = can_cli_register(SCAN_FRAME_ID, can_scan_signal);
        /* register CAN OTA start command */
        result = can_cli_register(CAN_OTA_ID, can_ota_handler);

        return result;
}

