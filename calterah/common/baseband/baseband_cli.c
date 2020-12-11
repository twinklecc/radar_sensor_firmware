#include "embARC_toolchain.h"
#include "embARC_assert.h"
#include "embARC.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"
#include "alps_hardware.h"
#include "clkgen.h"
#include "mux.h"
#include "sensor_config.h"
#include "baseband_reg.h"
#include "baseband_hw.h"
#include "baseband.h"
#include "radio_ctrl.h"
#include "calterah_limits.h"
#include "dbg_gpio_reg.h"
#include "baseband_task.h"
#include "baseband_dpc.h"
#include "spi_hal.h"
#include "gpio_hal.h"
#include "dw_gpio.h"
#include "spi_master.h"
#include "cascade.h"
#include "baseband_cas.h"
#include "apb_lvds.h"
#include "radio_reg.h"
#include "radio_ctrl.h"
#include "baseband_cli.h"
#include "timers.h"

static bool sta_dmp_mid_en = false;
static bool sta_dmp_fnl_en = false;
static bool sta_fft1d_en   = false;
static bool scan_stop_flag = false;
static bool stream_on_en   = false;
static bool lvds_en        = false;

bool baseband_stream_on_dmp_mid()
{
        return sta_dmp_mid_en;
}

bool baseband_stream_on_dmp_fnl()
{
        return sta_dmp_fnl_en;
}

bool baseband_stream_on_fft1d()
{
        return sta_fft1d_en;
}

bool baseband_scan_stop_req()
{
        bool tmp = scan_stop_flag;

        if (scan_stop_flag)
                scan_stop_flag = false;

        return tmp;
}

bool baseband_stream_off_req()
{
        bool tmp = stream_on_en;

        if (stream_on_en)
                stream_on_en = false;

        return tmp;
}

#ifdef UNIT_TEST
#define MDELAY(ms)
#define UDELAY(us)
#else
#define MDELAY(ms)  chip_hw_mdelay(ms);
#define UDELAY(us)  chip_hw_udelay(us);
#endif

#define BB_WRITE_REG(bb_hw, RN, val) baseband_write_reg(bb_hw, BB_REG_##RN, val)
#define BB_READ_REG(bb_hw, RN) baseband_read_reg(bb_hw, BB_REG_##RN)

extern SemaphoreHandle_t mutex_frame_count;
extern int32_t frame_count;
extern QueueHandle_t queue_bb_isr;
void baseband_cli_commands();

#define READ_BACK_LEN 64
#define DATA_DUMP_SMOKE_TEST 0 /* 0 -- real-time adc data; 1 -- eable pattern test */
#define ENA_PRINT_ANT_CALIB     0
#define FFTP_SIZE      11
#define FFTP_SIZE_HALF 5

static bool baseband_cli_registered = false;
static BaseType_t scan_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_reg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_regdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_tbldump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bbcfg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_init_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_datdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_dc_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t ant_calib_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t tx_ant_phase_calib_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_datdump_serport_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_dbg_urt_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t radar_param_show(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_fftdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_test_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_bist_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_dac_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_hil_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_dbgdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bb_dbgsam_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#if INTER_FRAME_POWER_SAVE == 1
static BaseType_t bb_interframe_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#endif //INTER_FRAME_POWER_SAVE
static BaseType_t bb_agc_dbg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t fftp_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t Txbf_saturation_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#if (NUM_FRAME_TYPE > 1)
static BaseType_t fi_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#endif // (NUM_FRAME_TYPE > 1)
static BaseType_t bb_sambuf_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

#ifdef CHIP_CASCADE
static BaseType_t bb_sync_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
#endif // CHIP_CASCADE

static BaseType_t bb_rst_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* helpers */
static void print_help(char* buffer, size_t buff_len, const CLI_Command_Definition_t* cmd)
{
        uint32_t tot_count = 0;
        int32_t count = sprintf(buffer, "Wrong input\n\r %s", cmd->pcHelpString);
        EMBARC_ASSERT(count > 0);
        tot_count += count;
        EMBARC_ASSERT(tot_count < buff_len);
}

/* scan command */
static const CLI_Command_Definition_t scan_command = {
        "scan",
        "scan \n\r"
        "\tStart sensor scanning operation. \n\r"
        "\tUsage: scan [start/stop] <nframes = -1> <stream_on/stream_off> <adc/fft1d/fft2d/dbg_sam> <lvds>\n\r",
        scan_command_handler,
        -1
};

static BaseType_t scan_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3, *param4, *param5;
        BaseType_t len1, len2, len3, len4, len5;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
        param5 = FreeRTOS_CLIGetParameter(pcCommandString, 5, &len5);

        sta_dmp_mid_en = false;
        sta_dmp_fnl_en = false;
        sta_fft1d_en   = false;
        lvds_en = false;

        int32_t count = 0;
        uint8_t dump_src = DBG_SRC_DUMP_W_SYNC;
        /* get parameter 1, 2*/
        if (param1 != NULL) {
                if (strncmp(param1, "start", 5) == 0) {
                        baesband_frame_interleave_cnt_clr(); // clear recfg count in frame interleaving

                        scan_stop_flag = false;
                        stream_on_en   = false;

                        if (param2 == NULL)
                                count = -1;
                        else
                                count = strtol(param2, NULL, 0);
                } else if (strncmp(param1, "stop", 4) == 0) {
#ifdef CHIP_CASCADE
                        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                                scan_stop_flag = true;
#else
                        baseband_scan_stop(); /* scan stop */
#endif

                        pcWriteBuffer[0] = '\0';
                        return pdFALSE;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &scan_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &scan_command);
                return pdFALSE;
        }
        /* get parameter 3 */
        if (param3 != NULL) {
                if (strncmp(param3, "stream_on",  sizeof("stream_on") - 1) == 0) {
                        stream_on_en = true;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &scan_command);
                        return pdFALSE;
                }
        }
        /* get parameter 4 */
        if ((param4 != NULL) && (stream_on_en == true)) {
                if (strncmp(param4, "adc",  sizeof("adc") - 1) == 0) {
                        sta_dmp_mid_en = true;
                        sta_fft1d_en   = true;
                        baseband_switch_buf_store(NULL, SYS_BUF_STORE_ADC);
                } else if (strncmp(param4, "fft1d",  sizeof("fft1d") - 1) == 0) {
                        sta_dmp_mid_en = true;
                        baseband_switch_buf_store(NULL, SYS_BUF_STORE_FFT);
                } else if (strncmp(param4, "fft2d",  sizeof("fft2d") - 1) == 0) {
                        sta_dmp_fnl_en = true;
                } else if (strncmp(param4, "dbg_sam",  sizeof("dbg_sam") - 1) == 0) {
                        dump_src = DBG_SRC_SAM; /* config sample debug data to GPIO */
                        bb_clk_switch(); /* debug data has no buffer, bb clock should be switched to dbgbus clock */
                } else {
                        stream_on_en = false;
                        print_help(pcWriteBuffer, xWriteBufferLen, &scan_command);
                        return pdFALSE;
                }
        }
        /* get parameter 5 */
        if(param5 != NULL){
                if(strncmp(param5, "lvds", sizeof("lvds") - 1) == 0){
                        lvds_en = true;
                }
        }

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif

        /* dbgbus/lvds switch */
        if (stream_on_en) {
#ifdef CHIP_CASCADE
                lvds_dump_start(dump_src);
#else
        if(lvds_en == true){
                lvds_dump_start(dump_src);
        }else{
                dbgbus_dump_start(dump_src);
        }
#endif // CHIP_CASCADE
        }

        /* change frame number */
        xSemaphoreTake(mutex_frame_count, portMAX_DELAY);
        frame_count = count;
        xSemaphoreGive(mutex_frame_count);
        pcWriteBuffer[0] = '\0';
        return pdFALSE;
}

/* bb_reg command */
static const CLI_Command_Definition_t bb_reg_command = {
        "bb_reg",
        "bb_reg \n\r"
        "\tWrite or read baseband register. \n\r"
        "\tUsage: bb_reg [addr] <data> \n\r",
        bb_reg_command_handler,
        -1
};

static BaseType_t bb_reg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2;
        BaseType_t len1, len2;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        if (param1 != NULL && param2 != NULL) {
                unsigned int regAddr  = (unsigned int) strtol(param1, NULL, 0);
                unsigned int regValue = strtoul(param2, NULL, 0); /* change 'string' to 'unsigned long' */
                baseband_write_reg(NULL, regAddr, regValue);
                sprintf(pcWriteBuffer, "\r\nset baseband register: address - 0x%02X) value - 0x%02X\r\n",
                        regAddr, regValue);
        } else if (param1 != NULL && param2 == NULL) {
                unsigned int regAddr  = (unsigned int) strtol(param1, NULL, 0);
                unsigned int regValue = baseband_read_reg(NULL, regAddr);
                sprintf(pcWriteBuffer, "\r\nset baseband register: address - 0x%02X  value - 0x%02X\r\n",
                        regAddr, regValue);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_reg_command);
        }

        return pdFALSE;
}

/* bb_regdump command */
static const CLI_Command_Definition_t bb_regdump_command = {
        "bb_regdump",
        "bb_regdump \n\r"
        "\tDump all baseband register. \n\r"
        "\tUsage: bb_regdump\n\r",
        bb_regdump_command_handler,
        0
};

static BaseType_t bb_regdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        baseband_reg_dump(NULL);
        return pdFALSE;
}

/* bb_tbldump command */
static const CLI_Command_Definition_t bb_tbldump_command = {
        "bb_tbldump",
        "bb_tbldump \n\r"
        "\tDump baseband memory/LUT. \n\r"
        "\tUsage: bb_tbldump <table_id> <offset> <length>\n\r",
        bb_tbldump_command_handler,
        -1
};

static BaseType_t bb_tbldump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3;
        BaseType_t len1, len2, len3;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        uint32_t table_id;
        uint32_t offset;
        uint32_t length;
        if (param1 != NULL) {
                table_id = strtol(param1, NULL, 0);
                if (param2 == NULL)
                        offset = 0x0;
                else
                        offset = strtol(param2, NULL, 0);
                if (param3 == NULL)
                        length = 16;
                else
                        length = strtol(param3, NULL, 0);
                baseband_tbl_dump(NULL, table_id, offset, length);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_tbldump_command);
        }
        return pdFALSE;
}

/* bb_init command */
static const CLI_Command_Definition_t bb_init_command = {
        "bb_init",
        "bb_init \n\r"
        "\tBaseband initialization. \n\r"
        "\tUsage: bb_init\n\r",
        bb_init_command_handler,
        0
};

static BaseType_t bb_init_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif
        sensor_config_t *cfg = sensor_config_get_cur_cfg();
        sensor_config_check(cfg);
        baseband_clock_init();
        baseband_cli_commands();
        baseband_init(cfg->bb);
        return pdFALSE;
}

/* bb_datdump command */
static const CLI_Command_Definition_t bb_datdump_command = {
        "bb_datdump",
        "bb_datdump \n\r"
        "\tBaseband data dump \n\r"
        "\tUsage: bb_datdump <adc/fft1d/fft2d> <tone/normal> <nframe=10> <lvds>\n\r",
        bb_datdump_command_handler,
        -1
};

static BaseType_t bb_datdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3, *param4;
        BaseType_t len1, len2, len3, len4;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
        uint32_t dump_position, nframe;
        uint16_t bb_status_en = 0;
        bool fft2d_dump_flag = false;
        bool single_tone = false;
        bool lvds_en = false;

        /* get paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "adc", sizeof("adc") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_ADC;
                } else if (strncmp(param1, "fft1d", sizeof("fft1d") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_FFT;
                } else if (strncmp(param1, "fft2d", sizeof("fft2d") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_FFT;
                        fft2d_dump_flag = true;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_command);
                        return pdFALSE;
                }
        } else {
                dump_position = SYS_BUF_STORE_ADC;
        }

        if (param2 != NULL) {
                if (strncmp(param2, "tone", sizeof("tone") - 1) == 0)
                        single_tone = true; /* no FMCW, only baseband*/
                else if (strncmp(param2, "normal", sizeof("normal") - 1) == 0)
                        single_tone = false;
                else
                        single_tone = false;
        } else {
                single_tone = false;
        }

        if (param3 != NULL) {
                nframe = strtol(param3, NULL, 0);
                if (nframe > 1000) /* max limitation due to no stop mechanism in this command */
                        nframe = 10;
        } else {
                nframe = 10;
        }

        if(param4 != NULL){
                if(strncmp(param4, "lvds", sizeof("lvds") - 1) == 0){
                        lvds_en = true;
                }
        }

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif

        baesband_frame_interleave_cnt_clr(); // clear recfg count in frame interleaving
        baseband_t* bb = baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
        sensor_config_t* cfg = (sensor_config_t*)(bb->cfg);
        fmcw_radio_t* radio = &bb->radio;
        bool tx_en    = !single_tone;
        bool radio_en = !single_tone;


        /* message queue */
        uint32_t event_datdump = 0;

        /* dbgbus/lvds switch */
#ifdef CHIP_CASCADE
        lvds_dump_start(DBG_SRC_DUMP_W_SYNC);
#else
        if (lvds_en == true){
                lvds_dump_start(DBG_SRC_DUMP_W_SYNC);
        }else{
                dbgbus_dump_start(DBG_SRC_DUMP_W_SYNC);
        }
#endif // CHIP_CASCADE

        /* single tone settings */
        if (single_tone == true) {
                fmcw_radio_tx_ch_on(radio, -1, false); /* turn off tx */
                fmcw_radio_single_tone(radio, cfg->fmcw_startfreq, true); /* enable radio single tone of fmcw_startfreq */
                MDELAY(1); /* for FMCW settled */
                BB_WRITE_REG(&bb->bb_hw, SAM_FORCE, SAM_FORCE_ENABLE);
        }

        /* smoke test data pattern settings */
        if (DATA_DUMP_SMOKE_TEST == 1) /* write data pattern to memory */
                baseband_datdump_smoke_test(&bb->bb_hw);

        /* timer start */
        track_start(bb->track);

        while(1) {
                if ((nframe != 0) && (track_is_ready(bb->track))) {
                        track_lock(bb->track);

                        /* align bank of frame type */
                        bb = baseband_frame_interleave_recfg(); /* reconfigure the frame pattern */
                        cfg = (sensor_config_t*)(bb->cfg);
                        baseband_hw_t* bb_hw = &bb->bb_hw;

                        /* baseband dump init */
                        uint32_t old_buf_store = baseband_switch_buf_store(bb_hw, dump_position);
                        uint8_t old_bnk = BB_READ_REG(bb_hw, FDB_SYS_BNK_ACT); /* read back the bank selected in RTl */
                        uint16_t old_status_en = BB_READ_REG(bb_hw, SYS_ENABLE);

                        /* data collection*/
                        if (DATA_DUMP_SMOKE_TEST == 1) { /* data pattern test */
                                bb_status_en = SYS_ENA(DMP_FNL, true);
                                baseband_start_with_params(bb, false, false, bb_status_en, false, BB_IRQ_ENABLE_BB_DONE, false);
                                /* cas_sync_en = false */
                                /* as DMP_FNL on slave will directly run without ADC sync */
                                /* so master no need sync with slave when smoke test */

                        } else if (fft2d_dump_flag == true) { /* fft2d data */
                                bb_status_en =  SYS_ENA(SAM    , true)
                                               |SYS_ENA(FFT_2D , true)
                                               |SYS_ENA(DMP_FNL, true);
                                baseband_start_with_params(bb, radio_en, tx_en, bb_status_en, true, BB_IRQ_ENABLE_ALL, false);

                        } else { /* adc or fft1d data*/
                                bb_status_en =  SYS_ENA(SAM    , true)
                                               |SYS_ENA(DMP_FNL, true);
                                baseband_start_with_params(bb, radio_en, tx_en, bb_status_en, true, BB_IRQ_ENABLE_ALL, false);
                        }

                        // wait done
                        if(xQueueReceive(queue_bb_isr, &event_datdump, portMAX_DELAY) == pdTRUE)
                                ;

                        /* restore baseband status */
                        /* the following 3 lines should be inside the while loop, as bank index will change in multi mode */
                        BB_WRITE_REG(bb_hw, SYS_BNK_ACT, old_bnk);
                        BB_WRITE_REG(bb_hw, SYS_ENABLE, old_status_en);
                        baseband_switch_buf_store(bb_hw, old_buf_store);

                        EMBARC_PRINTF("frame = %d\n", nframe);
                        nframe--;
                } else if( nframe == 0 ) {
                        break;
                } else {
                        taskYIELD();
                }
        }
        /* dump finish */
#ifdef CHIP_CASCADE
        lvds_dump_stop();
#else
        if (lvds_en == true){
                lvds_dump_stop();
        }else{
                dbgbus_dump_stop();
        }
#endif // CHIP_CASCADE

        track_stop(bb->track);

        /* single tone settings restore */
        if (single_tone == true) {
                BB_WRITE_REG(&bb->bb_hw, SAM_FORCE, SAM_FORCE_DISABLE);
                baseband_hw_reset_after_force(&bb->bb_hw);
#if INTER_FRAME_POWER_SAVE == 0 /* when power save enabled, restore will be run when next "baseband_start_with_params" */
                fmcw_radio_tx_restore(radio);
#endif
        }

        return pdFALSE;
}

/* bb_dc command */
static const CLI_Command_Definition_t bb_dc_command = {
        "bb_dc",
        "bb_dc \n\r"
        "\tOutput DC offset.\n\r"
        "\tUsage: bb_dc <voltage/leakage>\n\r",
        bb_dc_command_handler,
        -1
};

static BaseType_t bb_dc_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        bool leakage_flag = false;
        /* get paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "voltage", sizeof("voltage") - 1) == 0) {
                        leakage_flag = false;
                } else if (strncmp(param1, "leakage", sizeof("leakage") - 1) == 0) {
                        leakage_flag = true;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dc_command);
                        return pdFALSE;
                }
        }

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif
        /* get RTL bank selected*/
        baseband_t* bb = baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
        baseband_hw_t *bb_hw = &bb->bb_hw;
        baseband_dc_calib_init(bb_hw, leakage_flag, true);
        return pdFALSE;
}

/* ant_calib command */
static const CLI_Command_Definition_t ant_calib_command = {
        "ant_calib",
        "ant_calib \n\r"
        "\tBaseband initialization. \n\r"
        "\tUsage: ant_calib\n\r",
        ant_calib_command_handler,
        -1
};

static volatile bool frame_ready = true;
static xTimerHandle xTimerFrame = NULL;
void vTimerFrameCallback(xTimerHandle xTimer)
{
        frame_ready = true;
}

static BaseType_t ant_calib_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        baesband_frame_interleave_cnt_clr(); // clear recfg count in frame interleaving

        /* reconfigure the frame pattern for frame-interleaving */
        baseband_t *bb = baseband_frame_interleave_recfg();
        sensor_config_t *cfg = (sensor_config_t*)(bb->cfg);
        baseband_hw_t* bb_hw = &bb->bb_hw;

        const char *param1, *param2, *param3, *param4, *param5, *param6;
        BaseType_t len1, len2, len3, len4, len5, len6;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
        param5 = FreeRTOS_CLIGetParameter(pcCommandString, 5, &len5);
        param6 = FreeRTOS_CLIGetParameter(pcCommandString, 6, &len6);

        int calib_ang = 0;                      // calib angle
        float calib_rang_min = 0.5;             // minimum target distance, unit: meter
        float calib_rang_max = 50.0;            // maximum target distance, unit: meter
        int calib_num = 10, calib_cnt = 0;      // calib times
        uint32_t rng_index = 0;                 // target 2D-FFT range index
        uint16_t frame_period = 50;             // Frame Repetition Period in ms
        bool timer_en = false;
        uint8_t bpm_idx_min = 0;
        uint8_t bpm_idx_max = cfg->nvarray - 1;

        if (cfg->anti_velamb_en) {
                bpm_idx_min = 1;
                bpm_idx_max = cfg->nvarray;
        }

        /* get paremeter */
        if (param1 != NULL) {
                calib_ang = strtol(param1, NULL, 0);
                if (calib_ang > 90 || calib_ang < -90)
                        calib_ang = 0;
        }
        if (param2 != NULL) {
                calib_rang_min = strtol(param2, NULL, 0);
                if (calib_rang_min < 2*bb->sys_params.rng_delta)
                        calib_rang_min = 0.5;
        }
        if (param3 != NULL) {
                calib_rang_max = strtol(param3, NULL, 0);
                if (calib_rang_max < 2*bb->sys_params.rng_delta)
                        calib_rang_max = 50.0;
        }
        if (param4 != NULL) {
                calib_num = strtol(param4, NULL, 0);
                if (calib_num < 1)
                        calib_num = 10;
        }
        if (param5 != NULL) {
                rng_index = strtol(param5, NULL, 0);
                if (rng_index > 200 || calib_num < 5)
                        rng_index = 0;
        }
        if(param6 != NULL){
                timer_en = true;
                frame_period = strtol(param6, NULL,0);
                if(frame_period < 10 || frame_period > 5000)
                        frame_period = 50;
        }
        calib_cnt = calib_num / cfg->nvarray;

        /* Initialize Timer */
        if(timer_en){
                if(xTimerFrame == NULL){
                        /* if timer has not been created before, create timer */
                        xTimerFrame = xTimerCreate("FrameTimer", pdMS_TO_TICKS(frame_period), pdTRUE, (void *)1, vTimerFrameCallback);
                        if(xTimerFrame == NULL){
                                EMBARC_PRINTF("Timer Initialization failed.\r\n");
                        }
                }else{
                        /* if timer has already been created, update the frame repeatition period */
                        xTimerChangePeriod(xTimerFrame, pdMS_TO_TICKS(frame_period), 0);
                }
        }

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif
        /* turn off zero doppler removel */
        bool old_zer_dpl = BB_READ_REG(bb_hw, FFT_ZER_DPL_ENB);
        BB_WRITE_REG(bb_hw, FFT_ZER_DPL_ENB  , 0);

        dmu_adc_reset(); // ADC should reset in cascade

        while(1){
                if((frame_ready) && (calib_cnt > 0)){
                        if(timer_en){
                                frame_ready = false;
                                if( xTimerStart( xTimerFrame, 0 ) != pdPASS ){
                                        EMBARC_PRINTF("Timer not started.\r\n");
                                        break;
                                }
                        }

                        /* start baseband */
                        baseband_start_with_params(bb, true, true,
                                                (   (SYS_ENABLE_SAM_MASK << SYS_ENABLE_SAM_SHIFT)
                                                | (SYS_ENABLE_FFT_2D_MASK << SYS_ENABLE_FFT_2D_SHIFT)
                                                | (SYS_ENABLE_CFR_MASK << SYS_ENABLE_CFR_SHIFT)
                                                | (SYS_ENABLE_BFM_MASK << SYS_ENABLE_BFM_SHIFT)),
                                                true, BB_IRQ_ENABLE_SAM_DONE, false);
                        /* wait done */
                        while (baseband_hw_is_running(bb_hw) == false)
                                ;
                        while (baseband_hw_is_running(bb_hw) == true)
                                ;
                        // Search test target peak in 2D-FFT plane
                        uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                        /* search peak */
                        if ( rng_index==0 ) {
                                float peak_power = 0;
                                complex_t complex_fft;
                                uint32_t fft_mem;
                                int bpm_index;
                                int ch_index;
                                int tmp_rng_ind  = (int)(calib_rang_min/bb->sys_params.rng_delta);
                                int rng_ind_end  = (int)(calib_rang_max/bb->sys_params.rng_delta);
                                for ( ; tmp_rng_ind <= rng_ind_end; tmp_rng_ind++) {
                                        float tmp_power = 0;
                                        for (bpm_index = bpm_idx_min; bpm_index <= bpm_idx_max; bpm_index++) {
                                                for (ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, tmp_rng_ind, 0, bpm_index);
                                                        complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                                                        tmp_power += (complex_fft.r * complex_fft.r + complex_fft.i * complex_fft.i) / 64; // Non-coherent accumulation of 4 channel 2D-FFT value
                                                }
                                        }
                                        if (peak_power < tmp_power) {        // find the peak
                                                peak_power = tmp_power;
                                                rng_index = tmp_rng_ind;     // peak index
                                        }
                                }
                        }
                        /*print the result of last calib to check the memory read is correct*/
                        baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
                        volatile obj_info_t *obj_info = (volatile obj_info_t *)BB_MEM_BASEADDR;
                        if(ENA_PRINT_ANT_CALIB == 1 && calib_cnt == (calib_num / cfg->nvarray) - 1)
                                for (int i = 0; i < 4; i++)
                                        EMBARC_PRINTF("obj_num = 0x%x rng = 0x%x vel = 0x%x  noi = 0x%x sig_0 = 0x%x\n", i, obj_info[i].rng_idx, obj_info[i].vel_idx,  obj_info[i].noi, obj_info[i].doa[0].sig_0);
                        /* output phase */
                        int ch_index;
                        int bpm_index;
                        uint32_t fft_mem;
                        complex_t complex_fft;
                        float angle;
                        float power;
                        baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
#ifdef CHIP_CASCADE
                        if (chip_cascade_status() == CHIP_CASCADE_SLAVE) {
                                cascade_write_buf_req();
                                cascade_write_buf(rng_index); // write rng_index to spi
                                for (int bpm_index = bpm_idx_min; bpm_index <= bpm_idx_max; bpm_index++) {
                                        for (int ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                                fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, 0, bpm_index);
                                                cascade_write_buf(fft_mem); // write FFT data to spi
                                        }
                                }
                                cascade_write_buf_done();
                        }
#endif
                        for (bpm_index = bpm_idx_min; bpm_index <= bpm_idx_max; bpm_index++){
                                for (ch_index = 0; ch_index < MAX_NUM_RX; ch_index++){
                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, 0, bpm_index);
                                        complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                                        angle = (atan2f(complex_fft.i, complex_fft.r) * 180.0 / 3.14);
                                        power = (complex_fft.r * complex_fft.r + complex_fft.i * complex_fft.i) / 50;
                                        EMBARC_PRINTF("%7.2f  %7.2f  ", angle, power);
                                }
                        }
                        EMBARC_PRINTF("  rng_index %d  range %.2f master\n", rng_index, rng_index*bb->sys_params.rng_delta);
                        baseband_switch_mem_access(bb_hw, old);

#ifdef CHIP_CASCADE
                        uint32_t slv_fft_buf[MAX_NUM_RX*MAX_NUM_RX];
                        uint32_t rx_buf_offset = 0;
                        if (chip_cascade_status() == CHIP_CASCADE_MASTER) { // read slave FFT data
                                if (E_OK == cascade_read_buf_req(WAIT_TICK_NUM)) { /* wait 30ms */
                                        uint32_t slv_rng_index = cascade_read_buf(rx_buf_offset++); // read rng_index from spi
                                        for (int bpm_index = bpm_idx_min; bpm_index <= bpm_idx_max; bpm_index++) {
                                                for (int ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                                        // now rx_buf_offset == 1
                                                        *(slv_fft_buf + rx_buf_offset-1) = cascade_read_buf(rx_buf_offset); // read fft data from spi, store first then print
                                                        rx_buf_offset++;
                                                }
                                        }
                                        cascade_read_buf_done(); /* release rx buffer */
                                        rx_buf_offset = 0;
                                        for (int bpm_index = bpm_idx_min; bpm_index <= bpm_idx_max; bpm_index++) {
                                                for (int ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                                        uint32_t fft2d_data = *(slv_fft_buf + rx_buf_offset++);
                                                        complex_t complex_fft = cfl_to_complex(fft2d_data, 14, 14, true, 4, false);
                                                        float angle = (atan2f(complex_fft.i, complex_fft.r) * 180.0 / 3.14);
                                                        float power = (complex_fft.r * complex_fft.r + complex_fft.i * complex_fft.i) / 50;
                                                        EMBARC_PRINTF("%7.2f  %7.2f  ", angle, power);
                                                }
                                        }
                                        EMBARC_PRINTF("  rng_index %d  range %.2f slave\n", slv_rng_index, slv_rng_index*bb->sys_params.rng_delta);
                                } else {
                                        EMBARC_PRINTF("!!! merge timeout ~~~\n"); // To be modified for python identification
                                } /* endif E_OK */
                        }
#endif
                        calib_cnt--;
                }else if (calib_cnt == 0){
                        break;
                }else{
                        taskYIELD();
                }
        }
        if(timer_en){
                xTimerStop(xTimerFrame, 0);
                frame_ready = true;
        }

#if(0) // set 1 to print temperature
        float temperature = 0;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        temperature = fmcw_radio_get_temperature(NULL);

        /* display info */
        sprintf(pcWriteBuffer, "\r\n radio temperature: %.2f \r\n", temperature);
#endif
        /* restore zero doppler removel */
        BB_WRITE_REG(bb_hw, FFT_ZER_DPL_ENB, old_zer_dpl);

        return pdFALSE;
}

/* tx_ant_phase_calib command */
static const CLI_Command_Definition_t tx_ant_phase_calib_command = {
        "tx_ant_phase_calib",
        "tx_ant_phase_calib \n\r"
        "\tOutput tx_ant_phase_calib result.\n\r"
        "\tUsage: tx_ant_phase_calib <rx_antenna_idx=0><search_range_start_idx=4> \n\r",
        tx_ant_phase_calib_command_handler,
        -1
};

static BaseType_t tx_ant_phase_calib_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        sensor_config_t *cfg = sensor_config_get_cur_cfg();
        baseband_t *bb = baseband_get_cur_bb();
        baseband_hw_t *bb_hw = &bb->bb_hw;

        //back up tx_groups
        uint32_t tx_groups_bk[MAX_NUM_TX];
        memcpy(&tx_groups_bk[0], &cfg->tx_groups[0], MAX_NUM_TX*sizeof(uint32_t));
        //reset tx_groups to 0
        memset(&cfg->tx_groups[0], 0, MAX_NUM_TX*sizeof(uint32_t));

        const char *param1, *param2;
        BaseType_t len1, len2;
        float calib_phase_result;//toggled phase calibration result
        float calib_rx_angle[2];

        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

        uint8_t  rx_antenna_idx;                   // rx antenna index
        uint32_t phase_scramble_init_state_bak;
        uint32_t phase_scramble_tap_bak;
        uint32_t phase_scramble_init_state_01;
        uint32_t phase_scramble_tap_01;
        uint32_t rng_max_index;
        int32_t search_range_start_idx;

        /* get parameter */
        EMBARC_PRINTF("------ important notice ------\n");
        if (param1 != NULL) {
                rx_antenna_idx = strtol(param1, NULL, 0);
                if (rx_antenna_idx >= MAX_NUM_RX){
                        EMBARC_PRINTF("the rx_antenna_idx does not exist, the default value 0 is used\n");
                        rx_antenna_idx = 0;
                }
        }
        else{//default rx antenna index
                rx_antenna_idx = 0;
        }
        //MAX_VALUE_SEARCH_RANGE_LEN-2,minus 2 in order to endure the error
        float longest_dis = (cfg->fmcw_chirp_rampup*3.0*1e2*cfg->adc_freq*(MAX_VALUE_SEARCH_RANGE_LEN-2)/(2.0*cfg->fmcw_bandwidth*cfg->rng_nfft));
        EMBARC_PRINTF("make sure you put the corner reflector at most %.2f meters away\n",longest_dis);

        if (param2 != NULL) {
                search_range_start_idx = strtol(param2, NULL, 0);
                if ((search_range_start_idx >= MAX_VALUE_SEARCH_RANGE_LEN) || (search_range_start_idx < RNG_START_SEARCH_IDX)){
                        EMBARC_PRINTF("the search_range_start_idx is not appropriate, the default value 4 is used\n");
                        search_range_start_idx = RNG_START_SEARCH_IDX;
                }
        }
        else{
                search_range_start_idx = RNG_START_SEARCH_IDX;
        }

        EMBARC_PRINTF("make sure you put the corner reflector far away enough to elevate the precision\n");
        float shortest_dis = (cfg->fmcw_chirp_rampup*3.0*1e2*cfg->adc_freq*search_range_start_idx/(2.0*cfg->fmcw_bandwidth*cfg->rng_nfft));
        EMBARC_PRINTF("at least %.2f meters away\n",shortest_dis);

        /* turn off zero doppler removel */
        bool old_zer_dpl = BB_READ_REG(bb_hw, FFT_ZER_DPL_ENB);
        BB_WRITE_REG(bb_hw, FFT_ZER_DPL_ENB  , 0);
        //generate "0101..." XOR chain
        phase_scramble_init_state_01 = 0xaaaaaaaa;
        phase_scramble_tap_01 = 0x1;
        //back up the pre XOR chain
        phase_scramble_init_state_bak = BB_READ_REG(bb_hw, FFT_DINT_DAT_PS);
        phase_scramble_tap_bak = BB_READ_REG(bb_hw, FFT_DINT_MSK_PS);
        //rewrite "010101..." XOR CHAIN for test
        BB_WRITE_REG(bb_hw, FFT_DINT_DAT_PS, phase_scramble_init_state_01);
        BB_WRITE_REG(bb_hw, FFT_DINT_MSK_PS, phase_scramble_tap_01);
        BB_WRITE_REG(bb_hw, SAM_DINT_DAT,    phase_scramble_init_state_01);
        BB_WRITE_REG(bb_hw, SAM_DINT_MSK,    phase_scramble_tap_01);
        uint32_t old_dint_ena = BB_READ_REG(bb_hw, FFT_DINT_ENA);
        //setting phase scramble feature
        BB_WRITE_REG(bb_hw, FFT_DINT_ENA     , 4);
        //setting baseband phase compensation
        complex_t ps_cpx_f;
        for (int idx = 0; idx < MAX_NUM_RX; idx++) {
                ps_cpx_f.r = cos(180.0 * M_PI / 180);
                ps_cpx_f.i = sin(180.0 * M_PI / 180);
                baseband_write_reg(bb_hw,
                                   (uint32_t)(((uint32_t *)BB_REG_FFT_DINT_COE_PS_0) + idx),
                                   complex_to_cfx(&ps_cpx_f, 14, 1, true));
        }
        //setting rf phase compensation
        fmcw_radio_t * radio = &bb->radio;


        uint32_t nvarray_bk = cfg->nvarray;
        //reset cfg->nvarray and correlated registers with nvarray
        cfg->nvarray = 1;
        uint32_t size_bpm_bk = BB_READ_REG(bb_hw, SYS_SIZE_BPM);
        BB_WRITE_REG(bb_hw, SYS_SIZE_BPM    , cfg->nvarray - 1);
        uint32_t size_vel_buf = BB_READ_REG(bb_hw, SYS_SIZE_VEL_BUF);
        uint32_t chip_used = cfg->nchirp / cfg->nvarray;
        uint32_t chip_buf = chip_used > cfg->vel_nfft ? cfg->vel_nfft : chip_used;
        BB_WRITE_REG(bb_hw, SYS_SIZE_VEL_BUF, chip_buf - 1);

        //reset vitual array registers
        fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        for (int ch = 0; ch < MAX_NUM_TX; ch++) {
            fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_2 + ch * 3, 0);
        }
        //store old rf register
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        uint8_t old_ps_tap0 = RADIO_READ_BANK_REG(3, FMCW_PS_TAP0);
        uint8_t old_ps_tap1 = RADIO_READ_BANK_REG(3, FMCW_PS_TAP1);
        uint8_t old_ps_tap2 = RADIO_READ_BANK_REG(3, FMCW_PS_TAP2);
        uint8_t old_ps_tap3 = RADIO_READ_BANK_REG(3, FMCW_PS_TAP3);
        uint8_t old_ps_state0 = RADIO_READ_BANK_REG(3, FMCW_PS_STATE0);
        uint8_t old_ps_state1 = RADIO_READ_BANK_REG(3, FMCW_PS_STATE1);
        uint8_t old_ps_state2 = RADIO_READ_BANK_REG(3, FMCW_PS_STATE2);
        uint8_t old_ps_state3 = RADIO_READ_BANK_REG(3, FMCW_PS_STATE3);
        //setting rf register
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP0, REG_L(phase_scramble_tap_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP1, REG_M(phase_scramble_tap_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP2, REG_H(phase_scramble_tap_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP3, REG_INT(phase_scramble_tap_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE0, REG_L(phase_scramble_init_state_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE1, REG_M(phase_scramble_init_state_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE2, REG_H(phase_scramble_init_state_01));
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE3, REG_INT(phase_scramble_init_state_01));

        /* Phase scramble configureation for group 1 */
        fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        uint32_t old_phase_scramble_flag = fmcw_radio_reg_read(radio, RADIO_BK5_FMCW_PS_EN_1);
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_PS_EN_1, 0x3);  /*enable phase scramble*/

        fmcw_radio_switch_bank(radio, old_bank);

        uint32_t old_buf_store = baseband_switch_buf_store(NULL, SYS_BUF_STORE_FFT);

        for (int tx_ant_idx = 0; tx_ant_idx < MAX_NUM_TX; tx_ant_idx ++) {
                calib_rx_angle[0] = 0;
                calib_rx_angle[1] = 0;
                EMBARC_PRINTF("---tx antenna %d phase calibration start\n",tx_ant_idx);
                //configure tx_groups for every antenna
                cfg->tx_groups[tx_ant_idx] = 1;
                //reset previous TX antenna
                if (tx_ant_idx > 0)
                        cfg->tx_groups[tx_ant_idx-1] = 0;


                /* start baseband */
                baseband_start_with_params(bb, true, true,
                                              (SYS_ENABLE_SAM_MASK << SYS_ENABLE_SAM_SHIFT),
                                           true, BB_IRQ_ENABLE_SAM_DONE, false);

                /* wait done */
                while (baseband_hw_is_running(bb_hw) == false)
                        ;
                while (baseband_hw_is_running(bb_hw) == true)
                        ;
                float peak_power = 0;
                complex_t complex_fft;
                uint32_t fft_mem;
                rng_max_index = 0;

                // Search test target peak in 1D-FFT plane with velocity = 0
                uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                float power_tmp;
                //calculate power
                for (uint32_t tmp_rng_ind  = RNG_START_SEARCH_IDX-1; tmp_rng_ind < MAX_VALUE_SEARCH_RANGE_LEN; tmp_rng_ind++) {
                        fft_mem = baseband_hw_get_fft_mem(bb_hw, rx_antenna_idx, tmp_rng_ind, 0, 0);
                        complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                        power_tmp = complex_fft.r * complex_fft.r + complex_fft.i * complex_fft.i;
                        //get max index
                        if (peak_power < power_tmp) {           // find the peak
                                peak_power = power_tmp;
                                rng_max_index = tmp_rng_ind;     // peak index
                        }
                }
                for(uint32_t vel_idx = 0; vel_idx < cfg->vel_nfft; vel_idx++){
                        fft_mem = baseband_hw_get_fft_mem(bb_hw, rx_antenna_idx, rng_max_index, vel_idx, 0);
                        complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                        float theta = atan2f(complex_fft.i, complex_fft.r) * 180.0 / 3.14;
                        calib_rx_angle[vel_idx%2] += (theta/(cfg->vel_nfft/2));
                }
                //1DFFT data has been phase compensated already before used
                calib_phase_result = (180.0 + calib_rx_angle[0] - calib_rx_angle[1]);
                if (calib_phase_result < 0)
                        calib_phase_result += 360;
                else if (calib_phase_result >= 360)
                        calib_phase_result -= 360;
                EMBARC_PRINTF("---tx antenna %d phase calibration result %f\n", tx_ant_idx, calib_phase_result);
                baseband_switch_mem_access(bb_hw, old);
        }
        //restore rf XOR chain
        old_bank = fmcw_radio_switch_bank(radio, 3);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP0, old_ps_tap0);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP1, old_ps_tap1);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP2, old_ps_tap2);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP3, old_ps_tap3);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE0, old_ps_state0);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE1, old_ps_state1);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE2, old_ps_state2);
        RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE3, old_ps_state3);

        fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_PS_EN_1, old_phase_scramble_flag);

        fmcw_radio_switch_bank(radio, old_bank);
        BB_WRITE_REG(bb_hw, FFT_DINT_ENA, old_dint_ena);
        //buffer restore
        baseband_switch_buf_store(NULL, old_buf_store);
        /* restore zero doppler removel */
        BB_WRITE_REG(bb_hw, FFT_ZER_DPL_ENB, old_zer_dpl);
        /* restore XOR chain */
        BB_WRITE_REG(bb_hw, FFT_DINT_DAT_PS, phase_scramble_init_state_bak);
        BB_WRITE_REG(bb_hw, FFT_DINT_MSK_PS, phase_scramble_tap_bak);
        BB_WRITE_REG(bb_hw, SAM_DINT_DAT,    phase_scramble_init_state_bak);
        BB_WRITE_REG(bb_hw, SAM_DINT_MSK,    phase_scramble_tap_bak);
        //restore tx channel PA & LDO
        bool enable;
        for(uint8_t ch = 0; ch < MAX_NUM_TX; ch++) {
                enable = !(cfg->tx_groups[ch] == 0);
                fmcw_radio_tx_ch_on(NULL, ch, enable);
                MDELAY(1); /* for FMCW settled */
        }
        //restore tx_groups
        memcpy(&cfg->tx_groups[0], &tx_groups_bk[0], MAX_NUM_TX*sizeof(uint32_t));
        //restore cfg->nvarray
        cfg->nvarray = nvarray_bk;
        //restore SYS_SIZE_BPM,SYS_SIZE_VEL_BUF
        BB_WRITE_REG(bb_hw, SYS_SIZE_BPM    , size_bpm_bk);
        BB_WRITE_REG(bb_hw, SYS_SIZE_VEL_BUF, size_vel_buf);
        /*restore virtual array settings*/
        if (cfg->nvarray >= 2) {
                fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
                for (int ch = 0; ch < MAX_NUM_TX; ch++) {
                        if (cfg->tx_groups[ch]>0) {
                                // set VAM config
                                uint8_t vam_cfg = ( 0x61 | (cfg->anti_velamb_en << 3) | (( cfg->nvarray - 1 ) << 1 ));
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_2 + ch * 3, vam_cfg);

                                /* set VAM group patten*/
                                uint16_t bit_mux[MAX_NUM_TX] = {0,0,0,0};
                                bit_parse(cfg->tx_groups[ch], bit_mux);
                                uint8_t bit_mux_all = bit_mux[0]<<6 | bit_mux[1]<<4 | bit_mux[2]<< 2 | bit_mux[3];
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_1 + ch * 3, bit_mux_all);
                        } else {
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_2 + ch * 3, 0); // clear 0
                        }
                }
        }

        return pdFALSE;
}

/* bb_datdump_serport command */
static const CLI_Command_Definition_t bb_datdump_serport_command = {
        "bb_datdump_serport",
        "bb_datdump_serport \n\r"
        "\tDump n frame data through SPI or UART\n\r"
        "\tUsage: bb_datdump_serport <'adc'/'fft1d'/'fft2d'> <'spi'> <spi_id(0-2)> <frame_num>\n "
        "\tor bb_datdump_serport <'adc'/'fft1d'/'fft2d'> <'uart'> <bpm_index(0-3)> <ch_index(0-3)> <frame_num> \n"
        "\tor bb_datdump_serport <'adc'/'fft1d'/'fft2d'> <'uart'> <bpm_index(0-3)> < '4' > <frame_num> \n"   // Print all channel data
        "\tor bb_datdump_serport <'cfar'/'bfm'> <'uart'> <frame_num> \n\r",
        bb_datdump_serport_command_handler,
        -1
};

typedef enum {
    eBB_DATDUMP_ADC   = 0,    /* Dump ADC Data */
    eBB_DATDUMP_1DFFT = 1,    /* Dump 1DFFT Data */
    eBB_DATDUMP_2DFFT = 2,    /* Dump 2DFFT Data */
    eBB_DATDUMP_CFAR  = 3,    /* Dump CFAR Data */
    eBB_DATDUMP_BFM   = 4,    /* Dump BFM Data */
} eBB_DATDUMP_TYPE;

typedef enum {
    eBB_DATDUMP_SPI  = 0,    /* Dump BB Data through SPI Interface */
    eBB_DATDUMP_UART = 1,    /* Dump BB Data through UART Interface */
} eBB_DATDUMP_IF;

static BaseType_t bb_datdump_serport_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        baseband_t* bb = baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
        sensor_config_t* cfg = (sensor_config_t*)(bb->cfg);
        baseband_hw_t* bb_hw = &bb->bb_hw;

        const char *param1, *param2, *param3, *param4, *param5;
        BaseType_t len1, len2, len3, len4, len5;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
        param5 = FreeRTOS_CLIGetParameter(pcCommandString, 5, &len5);

        unsigned char dump_position = 0;
        int32_t nframe = 0;
        eBB_DATDUMP_TYPE eBBDatdumpType;    // 0: adc  1: fft1d  2: fft2d  3: cfar  4: bfm
        eBB_DATDUMP_IF eBBDatdumpIf;        // 0: SPI  1: UART
        eSPIM_ID SPIM_ID;                   // 0: SPI_M0(master0)   1: SPI_M1(master1)
        char bpm_index_input = 0;           // Input bpm (virtual array) index of adc/fft1d/fft2d data to dump to UART
        signed char ch_index_input = 0;     // Input channel index of adc/fft1d/fft2d data to dump to UART
        char * dat_type_str[3] = {"\nADC", "\nFFT_1D", "\nFFT_2D"};
        uint32_t event = 0;

        /* Get Data Dump Type Parameter - ADC/1DFFT/2DFFT/CFAR/BFM */
        if (param1 != NULL) {
                if (strncmp(param1, "adc", sizeof("adc") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_ADC;
                        eBBDatdumpType = eBB_DATDUMP_ADC;
                } else if (strncmp(param1, "fft1d", sizeof("fft1d") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_FFT;
                        eBBDatdumpType = eBB_DATDUMP_1DFFT;
                } else if (strncmp(param1, "fft2d", sizeof("fft2d") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_FFT;
                        eBBDatdumpType = eBB_DATDUMP_2DFFT;
                } else if (strncmp(param1, "cfar", sizeof("cfar") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_FFT;  // This is a MUST! Or ERROR will happen!
                        eBBDatdumpType = eBB_DATDUMP_CFAR;
                } else if (strncmp(param1, "bfm", sizeof("bfm") - 1) == 0) {
                        dump_position = SYS_BUF_STORE_FFT;  // This is a MUST! Or ERROR will happen!
                        eBBDatdumpType = eBB_DATDUMP_BFM;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                return pdFALSE;
        }

        /* Get Data Dump Interface - SPI/UART */
        if (param2 != NULL) {
                if (strncmp(param2, "spi", sizeof("spi") - 1) == 0) {
                        eBBDatdumpIf = eBB_DATDUMP_SPI;
                        /* Indicate SPI ID Number */
                        if (param3 != NULL) {
                                SPIM_ID = strtol(param3, NULL, 0);
                                if (SPIM_ID > eSPIM_ID_1) {
                                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                                        EMBARC_PRINTF("\n ERR : spi_id > 2 !\n");
                                        return pdFALSE;
                                }
                        } else {
                                print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                                return pdFALSE;
                        }
                        /* Indicate SPI Dump Frame Number */
                        if (param4 != NULL) {
                                nframe = strtol(param4, NULL, 0);
                        }
                } else if (strncmp(param2, "uart", sizeof("uart") - 1) == 0) {
                        eBBDatdumpIf = eBB_DATDUMP_UART;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                return pdFALSE;
        }

        /* spi dump CFAR/BFM data NOT supported !!! */
        if ((eBBDatdumpType >= eBB_DATDUMP_CFAR) && (eBBDatdumpIf == eBB_DATDUMP_SPI)) {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                return pdFALSE;
        }

        /* specify BPM and channel index using uart dump ADC/1D-FFT/2D-FFT data */
        if ((eBBDatdumpType < eBB_DATDUMP_CFAR) && (eBBDatdumpIf == eBB_DATDUMP_UART)) {
                /* Indicate bpm index */
                if (param3 != NULL) {
                        bpm_index_input = strtol(param3, NULL, 0);
                        if (bpm_index_input > cfg->nvarray - 1) {
                                print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                                EMBARC_PRINTF("\n ERR: BPM size is %d, but bpm_index > %d !\n", cfg->nvarray, cfg->nvarray-1);
                                return pdFALSE;
                        }
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                        return pdFALSE;
                }
                /* Indicate channel index */
                if (param4 != NULL) {
                        ch_index_input = strtol(param4, NULL, 0);
                        if (ch_index_input > 3 || ch_index_input < 0) {
                                ch_index_input = -1; // If param4 > 3, print all 4 channel data!
                        }
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_datdump_serport_command);
                        return pdFALSE;
                }
                /* Indicate UART Dump Frame Number */
                if (param5 != NULL) {
                        nframe = strtol(param5, NULL, 0);
                } else {
                        nframe = 1;
                }
        }

        if (eBBDatdumpIf == eBB_DATDUMP_SPI) {
                spi_master_init(SPIM_ID, SPIM_BAUD_RATE_1M);
        }

        /* bb run */
        bool tx_en    = true;
        bool radio_en = true;
        /* baseband dump init */
        uint32_t old_buf_store = baseband_switch_buf_store(bb_hw, dump_position);
        uint8_t old_bnk = BB_READ_REG(bb_hw, FDB_SYS_BNK_ACT); /* read back the bank selected in RTl */
        uint16_t old_status_en = BB_READ_REG(bb_hw, SYS_ENABLE);
        uint16_t bb_status_en = 0;
        uint8_t bb_mem_access_pos;

        if ( eBBDatdumpType == eBB_DATDUMP_2DFFT ) {
                /* Set BB run until 2DFFT finish */
                bb_status_en =    (1 << SYS_ENABLE_SAM_SHIFT    )
                                | (1 << SYS_ENABLE_FFT_2D_SHIFT );
                bb_mem_access_pos = SYS_MEM_ACT_BUF;
        } else if ( (eBBDatdumpType == eBB_DATDUMP_ADC) || (eBBDatdumpType == eBB_DATDUMP_1DFFT) ) {
                /* Set BB run until ADC/1DFFT Sample finish */
                bb_status_en =   (1 << SYS_ENABLE_SAM_SHIFT);
                bb_mem_access_pos = SYS_MEM_ACT_BUF;
        } else if ( (eBBDatdumpType == eBB_DATDUMP_CFAR) || (eBBDatdumpType = eBB_DATDUMP_BFM) ) {
                /* Set BB run until CFAR/BFM finish - Only UART Support */
                bb_status_en =   (SYS_ENABLE_SAM_MASK << SYS_ENABLE_SAM_SHIFT)
                                     | (SYS_ENABLE_FFT_2D_MASK << SYS_ENABLE_FFT_2D_SHIFT)
                                     | (SYS_ENABLE_CFR_MASK << SYS_ENABLE_CFR_SHIFT)
                                     | (SYS_ENABLE_BFM_MASK << SYS_ENABLE_BFM_SHIFT);
                bb_mem_access_pos = SYS_MEM_ACT_RLT;
        }

        /*--------------------------- dump data --------------------------*/
        EMBARC_PRINTF("\n # data_type is %d, dump_type is %d\n", eBBDatdumpType, eBBDatdumpIf);
        uint8_t frame_type = baseband_get_cur_frame_type();
        EMBARC_PRINTF("\n # current frame_type = %d \n", frame_type);

        /* dump adc/fft1d/fft2d data through spi */
        if (((eBBDatdumpType == eBB_DATDUMP_ADC) || (eBBDatdumpType == eBB_DATDUMP_1DFFT) || (eBBDatdumpType == eBB_DATDUMP_2DFFT)) && (eBBDatdumpIf == eBB_DATDUMP_SPI)) {
                int frame_index, ch_index, vel_index, rng_index, bpm_index;
                uint32_t fft_mem;
                uint32_t old;
                bool uart_debug = true;

                for (frame_index = 0; frame_index < nframe; frame_index++) {
                        /* BB Start */
                        baseband_start_with_params(bb, radio_en, tx_en, bb_status_en, true, BB_IRQ_ENABLE_ALL, false);
                        /* wait bb irq */
                        if(xQueueReceive(queue_bb_isr, &event, portMAX_DELAY) == pdTRUE)
                                ;
                        old = baseband_switch_mem_access(bb_hw, bb_mem_access_pos);

                        for (bpm_index = 0; bpm_index <= cfg->nvarray - 1; bpm_index++){
                                for (ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                        if (uart_debug) {
                                                EMBARC_PRINTF(dat_type_str[eBB_DATDUMP_ADC]);
                                                EMBARC_PRINTF("(:, :, %d, %d) = [ ", ch_index+1, bpm_index+1); // print in MATLAB style
                                        }
                                        for (vel_index = 0; vel_index < cfg->vel_nfft; vel_index++ ) {
                                                 for (rng_index = 0; rng_index < cfg->rng_nfft / 2; rng_index++ ) {
                                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, bpm_index);
                                                        /* Start SPI transfer */
                                                        chip_hw_udelay(1); //
                                                        spi_write(SPIM_ID, &fft_mem, 1);
                                                        chip_hw_udelay(1);
                                                        if (uart_debug){ // uart print for spi debug
                                                                EMBARC_PRINTF("%u ", fft_mem); // Print unsigned long to UART
                                                                UDELAY(1);
                                                        }
                                                        if (uart_debug && (rng_index % 20 == 0)){
                                                                EMBARC_PRINTF("...\n"); // switch to next line
                                                                UDELAY(2);
                                                        }
                                                }
                                                if (uart_debug) {
                                                        EMBARC_PRINTF(";...\n"); // end of one row
                                                        UDELAY(2);
                                                }
                                        }
                                        if (uart_debug) {
                                                EMBARC_PRINTF("];...\n"); // end of one matrix
                                        }
                                }
                        }

                        MDELAY(1);
                        baseband_switch_mem_access(bb_hw, old);
                }
        }

        /* dump adc/fft1d/fft2d data through uart */
        if (((eBBDatdumpType == eBB_DATDUMP_ADC) || (eBBDatdumpType == eBB_DATDUMP_1DFFT) || (eBBDatdumpType == eBB_DATDUMP_2DFFT)) && (eBBDatdumpIf == eBB_DATDUMP_UART)) {
                int frame_index, ch_index, vel_index, rng_index;
                uint32_t fft_mem;
                uint32_t old;
                unsigned char dump_channel_num = 1;

                if (ch_index_input < 0) {
                        dump_channel_num = MAX_NUM_RX; // print all 4 channels
                } else {
                        dump_channel_num = 1; // print one channel
                }
                EMBARC_PRINTF("# ch_index_input = %d, dump_channel_num = %d \n", ch_index_input, dump_channel_num);

                for (frame_index = 0; frame_index < nframe; frame_index++) {
                        /* BB Start */
                        baseband_start_with_params(bb, radio_en, tx_en, bb_status_en, true, BB_IRQ_ENABLE_ALL, false);
                        /* wait bb irq */
                        if(xQueueReceive(queue_bb_isr, &event, portMAX_DELAY) == pdTRUE)
                                ;
                        old = baseband_switch_mem_access(bb_hw, bb_mem_access_pos);

                        for (ch_index = 0; ch_index < dump_channel_num; ch_index++) {
                                EMBARC_PRINTF(dat_type_str[eBBDatdumpType]);
                                EMBARC_PRINTF("(:, :, %d, %d) = [ ", ch_index+1, bpm_index_input+1); // print data in MATLAB style
                                UDELAY(2);
                                for (vel_index = 0; vel_index < cfg->vel_nfft; vel_index++ ) {
                                        for (rng_index = 0; rng_index < cfg->rng_nfft / 2; rng_index++ ) {
                                                if (ch_index_input < 0) {
                                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, bpm_index_input); // print all 4 channels
                                                } else {
                                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index_input, rng_index, vel_index, bpm_index_input); // print specified channel
                                                }
                                                UDELAY(2);
                                                EMBARC_PRINTF("%u ", fft_mem); // Print unsigned long to UART
                                                UDELAY(2);
                                                if (rng_index % 20 == 0){
                                                        EMBARC_PRINTF("...\n"); // switch to next line
                                                        UDELAY(2);
                                                }
                                        }
                                        EMBARC_PRINTF(";...\n"); // end of one row
                                        UDELAY(2);
                                }
                                EMBARC_PRINTF("];...\n"); // end of one matrix
                                UDELAY(5);
                        }

                        MDELAY(1);
                        baseband_switch_mem_access(bb_hw, old);
                }
        }

        /* dump cfar/bfm data through uart */
        if ((eBBDatdumpType == eBB_DATDUMP_CFAR) || (eBBDatdumpType == eBB_DATDUMP_BFM)) {
                uint8_t frame_type = baseband_get_cur_frame_type();
                EMBARC_PRINTF("\n frame_type = %d \n", frame_type);
                radar_sys_params_t* sys_params = bb->track->radar_params[frame_type];
                uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
                uint32_t mem_rlt_offset = frame_type * (1<<SYS_SIZE_OBJ_WD_BNK) * RESULT_SIZE;
                volatile obj_info_t *obj_info = (volatile obj_info_t *)(BB_MEM_BASEADDR + mem_rlt_offset);

                int obj_num = baseband_read_reg(bb_hw, BB_REG_CFR_NUMB_OBJ);
                int32_t hw_rng, hw_vel, hw_sig, hw_noi, hw_ang;
                float measure_rng, measure_vel, measure_sig, measure_noi, measure_ang;
                EMBARC_PRINTF("\n--------- Object num is %d -------\n", obj_num);
                EMBARC_PRINTF("------- objects info list: --------\n\n");
                for (int i = 0; i < obj_num; i++) {
                        hw_rng = obj_info[i].rng_idx;
                        hw_vel = obj_info[i].vel_idx;
                        hw_sig = obj_info[i].doa[0].sig_0;
                        hw_ang = obj_info[i].doa[0].ang_idx_0;
                        hw_noi = obj_info[i].noi;
                        EMBARC_PRINTF(" obj_ind = %d \n [Hardware value]:\n 2DFFT_rng_ind = %d, 2DFFT_vel_ind = %d,  DBF_peak_ind = %d, 2DFFT_peak power = %d\n\n",
                                      i, hw_rng, hw_vel, hw_ang, hw_sig);
                        radar_param_fft2rv_nowrap(sys_params, hw_rng, hw_vel, &measure_rng, &measure_vel);
                        measure_sig = fl_to_float(hw_sig, 15, 1, false, 5, false);
                        measure_noi = fl_to_float(hw_noi, 15, 1, false, 5, false); //  In MP firmware, '* sys_params->noise_factor' is removed
                        measure_ang = sys_params->bfm_az_left + hw_ang * sys_params->az_delta_deg ;
                        EMBARC_PRINTF(" [Measured value]:\n  SNR(dB):%3.2f, R(m):%3.2f, V(m/s):%3.2f, A(deg):%3.2f\n\n",
                                10*log10f(measure_sig/measure_noi),
                                measure_rng,
                                measure_vel,
                                measure_ang);
                        uint32_t old1 = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                        uint32_t fft_mem;
                        complex_t complex_fft;
                        for (int bpm_index = 0; bpm_index <= cfg->nvarray - 1; bpm_index++){
                                EMBARC_PRINTF(" [Corresponding 2D-FFT peak values]:\n [bpm_index = %d, rx1-%d]:\n", bpm_index, MAX_NUM_RX);
                                for (int ch_index = 0; ch_index < MAX_NUM_RX; ch_index++){
                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, hw_rng, hw_vel, bpm_index);
                                        complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                                        EMBARC_PRINTF("%7.2f + j*%1.2f", complex_fft.r, complex_fft.i);
                                }
                        }
                        EMBARC_PRINTF("\n\n");
                        baseband_switch_mem_access(bb_hw, old1);
                }
                baseband_switch_mem_access(bb_hw, old);
        }

        BB_WRITE_REG(bb_hw, SYS_BNK_ACT, old_bnk);
        BB_WRITE_REG(bb_hw, SYS_ENABLE, old_status_en);
        baseband_switch_buf_store(bb_hw, old_buf_store);

        return pdFALSE;
}

static const CLI_Command_Definition_t bb_dbg_urt_command = {
        "bb_dbg_urt",
        "bb_dbg_urt \n\r"
        "\tUsage 1: dump cfar rv or dbf \n\r"
        "\tstep1: bb_dbg_urt <'cfar'/'dbf'> : turn on debug mode \n\r"
        "\tstep2: scan start <1> : bb run one frame \n\r"
        "\tstep3: bb_dbg_urt <'cfar'/'dbf'> <'dump'> : dump debug data \n\r"
        "\tstep4: bb_dbg_urt <'off'> : turn off debug mode \n\r"
        "\tUsage 2: dump MEM_BUF \n\r"
        "\tstep1: bb_dbg_urt <'adc'/'fft1d'> : select data store in MEM_BUF, default fft2d \n\r"
        "\tstep2: scan start <1> : bb run one frame \n\r"
        "\tstep3: bb_dbg_urt <'buf'> <ch_index:0-4>: dump 1 or 4 RXs' data in MEM_BUF\n\r"
        "\tstep4: bb_dbg_urt <'off'> : turn off debug mode \n\r",
                bb_dbg_urt_command_handler,
        -1
};

static BaseType_t bb_dbg_urt_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        baseband_t* bb = baseband_get_rtl_frame_type(); // read back the bank selected in RTL
        sensor_config_t* cfg = (sensor_config_t*)(bb->cfg);
        baseband_hw_t* bb_hw = &bb->bb_hw;
        uint8_t rtl_frame_type = baseband_read_reg(NULL, BB_REG_FDB_SYS_BNK_ACT);
        EMBARC_PRINTF("\n # rtl_frame_type = %d \n", rtl_frame_type);

        const char *param1, *param2;
        BaseType_t len1, len2;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

        unsigned char dat_type = 0;    // 0: non-coherent combined 2D_FFT,  1: DBF spectrum,  2: MEM_BUF data (ADC or FFT1D or FFT2D)
        signed char ch_index_input = 0;
        bool dump_en = false;
        char * dat_type_str[2] = {"\n comb_2dfft", "\n dbf_spectrum"};
        unsigned char dbg_trgt = DBG_BUF_TAR_NONE;
        /* get paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "adc", sizeof("adc") - 1) == 0) {
                        uint16_t sys_enable  =  SYS_ENA(AGC   , cfg->agc_mode == 0 ? 0: 1)
                                               |SYS_ENA(SAM   , true         );
                        bb_dpc_sysen_set(0, sys_enable);
                        BB_WRITE_REG(NULL, SAM_SINKER, SYS_BUF_STORE_ADC);
                } else if (strncmp(param1, "fft1d", sizeof("fft1d") - 1) == 0) {
                        uint16_t sys_enable  =  SYS_ENA(AGC   , cfg->agc_mode == 0 ? 0: 1)
                                               |SYS_ENA(SAM   , true         );
                        bb_dpc_sysen_set(0, sys_enable);
                        BB_WRITE_REG(NULL, SAM_SINKER, SYS_BUF_STORE_FFT);
                } else if (strncmp(param1, "cfar", sizeof("cfar") - 1) == 0) {
                        dat_type = 0;
                        dbg_trgt = DBG_BUF_TAR_CFR;
                } else if (strncmp(param1, "dbf", sizeof("dbf") - 1) == 0) {
                        dat_type = 1;
                        dbg_trgt = DBG_BUF_TAR_BFM;
                } else if (strncmp(param1, "off", sizeof("off") - 1) == 0) {
                        dbg_trgt = DBG_BUF_TAR_NONE;
                        baseband_data_proc_init();
                        BB_WRITE_REG(NULL, SAM_SINKER      , SAM_SINKER_FFT        );  // Reset register SAM_SINKER to default value as in baseband_sam_init().
                } else if (strncmp(param1, "buf", sizeof("buf") - 1) == 0) {
                        dat_type = 2;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbg_urt_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbg_urt_command);
                return pdFALSE;
        }
        if (param2 != NULL) {
                if (strncmp(param2, "dump", sizeof("dump") - 1) == 0) {
                        dump_en = true;
                } else if (dat_type == 2) {  //dump MEM_BUF ADC/FFT1D/FFT2D data
                        ch_index_input = strtol(param2, NULL, 0); // select one RX channel to dump
                        if (ch_index_input > 3 || ch_index_input < 0) {
                                ch_index_input = -1; // If param2 > 3, print all 4 channel data!
                        }
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbg_urt_command);
                        return pdFALSE;
                }
        } else {
                baseband_write_reg(bb_hw, BB_REG_DBG_BUF_TAR, dbg_trgt&0x3F); /* turn on/off debug mode */
        }

        if (dump_en == true){

#if (0) // set 1 to print cfar target information
        uint8_t frame_type = baseband_get_cur_frame_type();
        EMBARC_PRINTF("\n current frame_type = %d \n", frame_type);
        radar_sys_params_t* sys_params = bb->track->radar_params[frame_type];
        uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
        uint32_t mem_rlt_offset = frame_type * (1<<SYS_SIZE_OBJ_WD_BNK) * RESULT_SIZE;
        volatile obj_info_t *obj_info = (volatile obj_info_t *)(BB_MEM_BASEADDR + mem_rlt_offset);
        int obj_num = BB_READ_REG(bb_hw, CFR_NUMB_OBJ); ;
#ifdef CHIP_CASCADE
        obj_num = BB_READ_REG(bb_hw, DOA_NUMB_OBJ);
#endif
        int32_t hw_rng, hw_vel, hw_sig, hw_noi, hw_ang;
        float measure_rng, measure_vel, measure_sig, measure_noi, measure_ang;
        EMBARC_PRINTF("\n------  Object NUM is %d -----\n", obj_num);
        EMBARC_PRINTF(" ------ objects info list: ------- \n\n");
        for (int i = 0; i < obj_num; i++) {
                hw_rng = obj_info[i].rng_idx;
                hw_vel = obj_info[i].vel_idx;
                hw_sig = obj_info[i].doa[0].sig_0;
                hw_ang = obj_info[i].doa[0].ang_idx_0;
                hw_noi = obj_info[i].noi;

                EMBARC_PRINTF("\n obj_ind = %d \n [Hardware value]:\n 2DFFT_rng_ind = %d, 2DFFT_vel_ind = %d,  DBF_peak_ind = %d, 2DFFT_peak power = %d\n\n",
                              i, hw_rng, hw_vel, hw_ang, hw_sig);
                radar_param_fft2rv_nowrap(sys_params, hw_rng, hw_vel, &measure_rng, &measure_vel);
                measure_sig = fl_to_float(hw_sig, 15, 1, false, 5, false);
                measure_noi = fl_to_float(hw_noi, 15, 1, false, 5, false);
                measure_ang = sys_params->bfm_az_left + hw_ang * sys_params->az_delta_deg ;
                EMBARC_PRINTF("\n [Measured value]:\n  SNR(dB):%3.2f, R(m):%3.2f, V(m/s):%3.2f, A(deg):%3.2f\n\n",
                        10*log10f(measure_sig/measure_noi),
                        measure_rng,
                        measure_vel,
                        measure_ang);
                // Read fft data from MEM_BUF
                uint32_t old1 = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                uint32_t fft_mem;
                complex_t complex_fft;
                for (uint8_t bpm_index = 0; bpm_index <= cfg->nvarray - 1; bpm_index++){
                        EMBARC_PRINTF("\n [Corresponding 2D-FFT peak values]:\n [bpm_index = %d, rx1-%d]:\n", bpm_index, DOA_MAX_NUM_RX);
                        for (uint8_t ch_index = 0; ch_index < MAX_NUM_RX; ch_index++){
                                fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, hw_rng, hw_vel, bpm_index);
                                complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                                //EMBARC_PRINTF("fft_mem = %u ", fft_mem);
                                EMBARC_PRINTF("%7.2f + j*%1.2f", complex_fft.r, complex_fft.i);
                        }
#ifdef CHIP_CASCADE
                        // switch to MEM_MAC to get slave FFT peak data
                        uint32_t old2 = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_MAC);
                        uint32_t addr_shift = MAX_NUM_RX * ( i * cfg->nvarray + bpm_index );
                        for (uint8_t ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                fft_mem = baseband_read_mem_table(bb_hw, addr_shift + ch_index);
                                complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                                //EMBARC_PRINTF("fft_mem = %u ", fft_mem);
                                EMBARC_PRINTF("%7.2f + j*%1.2f", complex_fft.r, complex_fft.i);
                        }
                        baseband_switch_mem_access(bb_hw, old2);
#endif
                }
                EMBARC_PRINTF("\n\n");

                baseband_switch_mem_access(bb_hw, old1);
        }
        baseband_switch_mem_access(bb_hw, old);
#endif


        // Read debug data from MEM_BUF
        uint32_t old1 = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
        uint32_t fft_mem;
        uint32_t addr_offset = 0x100000/4; // Start read debug data from 1MB offset in MEM_BUF, read uint is word, so offset = 1MB / 4 bytes
        uint32_t data_num = 0; // number of debug data to print
        uint32_t col_num = 0; // column number of printed matrix
        if (dat_type == 0) {  // dump non-coherent combined 2D-FFT data
                data_num = cfg->rng_nfft / 2 * cfg->vel_nfft; // number of combined 2D-FFT points, only half range dimension of 2D-FFT data are stored
                col_num = cfg->vel_nfft;
        } else if (dat_type == 1) {  // dump DBF_spectrum data
                int obj_num = BB_READ_REG(bb_hw, CFR_NUMB_OBJ); // number of detected objects
#ifdef CHIP_CASCADE
                obj_num = BB_READ_REG(bb_hw, DOA_NUMB_OBJ);
#endif
                data_num = obj_num*cfg->doa_npoint[0];
                col_num = cfg->doa_npoint[0];
        }
        EMBARC_PRINTF("# data_size = %d \n", data_num);
        EMBARC_PRINTF(dat_type_str[dat_type]);
        EMBARC_PRINTF(" = [...\n"); // print in MATLAB style
        for (uint32_t i = 0; i < data_num; i ++){
                fft_mem = baseband_read_mem_table(bb_hw, i + addr_offset); // not i*4
                EMBARC_PRINTF("%d ", fft_mem);
                UDELAY(1);
                if ((i > 0) && ((i+1) % 32 == 0)){
                        EMBARC_PRINTF("...\n"); // switch to next line
                        UDELAY(2);
                }
                if ((i > 0) && ((i+1) % (col_num) == 0)){ // index i start from 0, so i+1 here
                        EMBARC_PRINTF(";...\n"); // print next row
                        UDELAY(2);
                }
        }
        EMBARC_PRINTF("];...\n");

        baseband_switch_mem_access(bb_hw, old1); // print debug data over

        }

        if (dat_type == 2){
                int bpm_index, ch_index, vel_index, rng_index;
                uint32_t fft_mem;
                uint32_t old;
                unsigned char dump_channel_num = 1;

                if (ch_index_input < 0) {
                        dump_channel_num = MAX_NUM_RX; // print all 4 channels
                } else {
                        dump_channel_num = 1; // print one channel
                }
                EMBARC_PRINTF("# ch_index_input = %d, dump_channel_num = %d \n", ch_index_input, dump_channel_num);

                old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                for (bpm_index = 0; bpm_index <= cfg->nvarray - 1; bpm_index++) {
                        for (ch_index = 0; ch_index < dump_channel_num; ch_index++) {
                                EMBARC_PRINTF("MEM_BUF(:, :, %d, %d) = [ ", ch_index+1, bpm_index +1); // print data in MATLAB style
                                UDELAY(2);
                                for (vel_index = 0; vel_index < cfg->vel_nfft; vel_index++ ) {
                                        for (rng_index = 0; rng_index < cfg->rng_nfft / 2; rng_index++ ) {
                                                if (ch_index_input < 0) {
                                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, bpm_index); // print all 4 channels
                                                } else {
                                                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index_input, rng_index, vel_index, bpm_index); // print specified channel
                                                }
                                                UDELAY(2);
                                                EMBARC_PRINTF("%u ", fft_mem); // Print unsigned long to UART
                                                UDELAY(2);
                                                if (rng_index % 20 == 0){
                                                        EMBARC_PRINTF("...\n"); // switch to next line
                                                        UDELAY(2);
                                                }
                                        }
                                        EMBARC_PRINTF(";...\n"); // end of one row
                                        UDELAY(2);
                                }
                                EMBARC_PRINTF("];...\n"); // end of one matrix
                                UDELAY(5);
                        }

                        MDELAY(1);
                        baseband_switch_mem_access(bb_hw, old);
                }
        }

        return pdFALSE;
}

/* radar_params command */
static const CLI_Command_Definition_t radar_param_show_command = {
        "radar_params",
        "radar_params \n\r"
        "\tShow radar system parameters \n\r"
        "\tUsage: radar_param\n\r",
        radar_param_show,
        0
};

static BaseType_t radar_param_show(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        static int bb_idx = 0;
        baseband_t *bb = baseband_get_bb(bb_idx);
        static uint32_t count = 0;
        radar_sys_params_t *sys = &bb->sys_params;
        switch(count) {
        case 0 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "carrier_freq = %.3fGHz\n\r", sys->carrier_freq);
                count++;
                return pdTRUE;
        case 1 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "bandwidth = %.3fMHz\n\r", sys->bandwidth);
                count++;
                return pdTRUE;
        case 2 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "chirp_rampup = %.3fus\n\r", sys->chirp_rampup);
                count++;
                return pdTRUE;
        case 3 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "chirp_period = %.3fus\n\r", sys->chirp_period);
                count++;
                return pdTRUE;
        case 4 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "Fs = %.3fMHz\n\r", sys->Fs);
                count++;
                return pdTRUE;
        case 5 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "rng_nfft = %d\n\r", sys->rng_nfft);
                count++;
                return pdTRUE;
        case 6 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "vel_nfft = %d\n\r", sys->vel_nfft);
                count++;
                return pdTRUE;
        case 7 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "doa_npoint = (%d,%d)\n\r", sys->doa_npoint[0],sys->doa_npoint[1]);
                count++;
                return pdTRUE;
        case 8 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "bfm_az_left = %.3fdeg\n\r", sys->bfm_az_left);
                count++;
                return pdTRUE;
        case 9 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "bfm_az_right = %.3fdeg\n\r", sys->bfm_az_right);
                count++;
                return pdTRUE;
        case 10 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "bfm_ev_up = %.3fdeg\n\r", sys->bfm_ev_up);
                count++;
                return pdTRUE;
        case 11 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "bfm_ev_down = %.3fdeg\n\r", sys->bfm_ev_down);
                count++;
                return pdTRUE;
        case 12 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "rng_delta = %.3fm\n\r", sys->rng_delta);
                count++;
                return pdTRUE;
        case 13 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "vel_delta = %.3fm/s\n\r", sys->vel_delta);
                count++;
                return pdTRUE;
        case 14 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "az_delta = %.3fdeg (%.3frad)\n\r", sys->az_delta_deg, sys->az_delta);
                count++;
                return pdTRUE;
        case 15 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "ev_delta = %.3fdeg (%.3frad)\n\r", sys->ev_delta_deg, sys->ev_delta);
                count++;
                return pdTRUE;
        case 16 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_fov_az_left = %.3fdeg\n\r", sys->trk_fov_az_left);
                count++;
                return pdTRUE;
        case 17 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_fov_az_right = %.3fdeg\n\r", sys->trk_fov_az_right);
                count++;
                return pdTRUE;
        case 18 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_nf_thres = %.3fm\n\r", sys->trk_nf_thres);
                count++;
                return pdTRUE;
        case 19 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_drop_delay = %.3fsec\n\r", sys->trk_drop_delay);
                count++;
                return pdTRUE;
        case 20 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_capt_delay = %.3fsec\n\r", sys->trk_capt_delay);
                count++;
                return pdTRUE;
        case 21 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_fps = %dfrm/sec \n\r", sys->trk_fps);
                count++;
                return pdTRUE;
        case 22 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_fov_ev_down = %.3fdeg\n\r", sys->trk_fov_ev_down);
                count++;
                return pdTRUE;
        case 23 :
                snprintf(pcWriteBuffer, cmdMAX_OUTPUT_SIZE-1, "trk_fov_ev_up = %.3fdeg\n\r", sys->trk_fov_ev_up);
                count++;
                return pdTRUE;
        default:
                count = 0;
                snprintf(pcWriteBuffer, xWriteBufferLen, "-----------SYS-PARAM-EOF %d----------\r\n", bb_idx);
                if (bb_idx == NUM_FRAME_TYPE-1) {
                        bb_idx = 0;
                        return pdFALSE;
                } else {
                        bb_idx++;
                        return pdTRUE;
                }
        }
        return pdFALSE;
}

/* bb_fftdump command */
static const CLI_Command_Definition_t bb_fftdump_command = {
        "bb_fftdump",
        "bb_fftdump \n\r"
        "\tDump FFT memory data \n\r"
        "\tUsage: bb_fftdump [ant_index] [bpm_index] [chirp_index] [sample_offset] [sample_len]\n\r",
        bb_fftdump_command_handler,
        -1
};

static BaseType_t bb_fftdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3, *param4, *param5;
        BaseType_t len1, len2, len3, len4, len5;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
        param5 = FreeRTOS_CLIGetParameter(pcCommandString, 5, &len5);
        sensor_config_t *cfg = sensor_config_get_cur_cfg();
        baseband_t *bb = baseband_get_cur_bb();
        baseband_hw_t *bb_hw = &bb->bb_hw;

        if ( param2==NULL ) {

                int ch_index, vel_index, rng_index;
                uint32_t fft_mem;
                complex_t complex_fft;
                float fft_power_db;
                uint32_t frame_index, frame_num;
                uint32_t old;
                if ( param1==NULL )
                        frame_num = 1;
                else
                        frame_num = ch_index = strtol(param1, NULL, 0);

                for (frame_index = 1; frame_index <= frame_num; frame_index++) {
                        EMBARC_PRINTF("\n==========\n");
                        baseband_start(bb);
                        /* wait done */
                        while (baseband_hw_is_running(bb_hw) == false)
                                ;
                        while (baseband_hw_is_running(bb_hw) == true)
                                ;
                        old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                        for (ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                                EMBARC_PRINTF("\nA %d\n", ch_index);
                                for (vel_index = 0; vel_index < cfg->vel_nfft / 2; vel_index++ ) {
                                        EMBARC_PRINTF("V %d\n", vel_index);
                                        for (rng_index = 0; rng_index < cfg->rng_nfft / 2; rng_index++ ) {
                                                fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, 0);
                                                complex_fft = cfl_to_complex(fft_mem, 14, 14, true, 4, false);
                                                fft_power_db = log10f(complex_fft.r * complex_fft.r + complex_fft.i * complex_fft.i);
                                                EMBARC_PRINTF("%2.2f\n", fft_power_db);
                                                if (rng_index % 10 == 0)
                                                        MDELAY(1);
                                        }
                                }
                        }
                        MDELAY(1000);
                        baseband_switch_mem_access(bb_hw, old);
                }

        } else {
                int ch_index = strtol(param1, NULL, 0);
                int bpm_index = strtol(param2, NULL, 0);
                int vel_index = strtol(param3, NULL, 0);
                int rng_offset = strtol(param4, NULL, 0);
                int rng_len = strtol(param5, NULL, 0);
                int rng_index;
                uint32_t fft_mem;

                uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                for (rng_index = rng_offset; rng_index <= rng_len; rng_index++) {
                        fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, bpm_index);
                        EMBARC_PRINTF( "  ant_index %d  bpm_index %d  chirp_index %d  fft_mem 0x%08x\n", ch_index, bpm_index, vel_index, fft_mem);
                }
                baseband_switch_mem_access(bb_hw, old);

        }

        return pdFALSE;
}

/* bb_test command for temporarily debugging, please do not check in debug code*/
static const CLI_Command_Definition_t bb_test_command = {
        "bb_test",
        "bb_test \n\r"
        "\tTest for temp debug.\n\r"
        "\tUsage: bb_test\n\r",
        bb_test_command_handler,
        -1
};

static BaseType_t bb_test_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        /* bb_test for temp debug, NO check in */
        return pdFALSE;
}

/* bb_bist command */
static const CLI_Command_Definition_t bb_bist_command = {
        "bb_bist",
        "bb_bist \n\r"
        "\tTest for bb logic bist.\n\r"
        "\tUsage: bb_bist\n\r",
        bb_bist_command_handler,
        -1
};

static BaseType_t bb_bist_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif

        sensor_config_t *cfg = sensor_config_get_cur_cfg();
        bool bist_result = baseband_bist_ctrl(cfg->bb, true);

        if (bist_result == true)
                EMBARC_PRINTF("BB_LBIST SUCCESS !!\n\r");
        else
                EMBARC_PRINTF("BB_LBIST ERROR !!\n\r");

        return pdFALSE;
}

/* bb_dac command */
static const CLI_Command_Definition_t bb_dac_command = {
        "bb_dac",
        "bb_dac \n\r"
        "\tTest for bb dac playback.\n\r"
        "\tUsage: bb_dac <outer/inner> <tia/hpf1/vga1/hpf2/vga2> <tia/hpf1/vga1/vga2> <adc>\n\r",
        bb_dac_command_handler,
        -1
};

static BaseType_t bb_dac_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3, *param4;
        BaseType_t len1, len2, len3, len4;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
        bool inner_circle = false;
        bool adc_dbg_en = false;
        uint8_t inject_num, out_num;
        baseband_t *bb = baseband_get_bb(0);
        baseband_hw_t *bb_hw = &bb->bb_hw;
        /* get paremeter 1 */
        if (param1 != NULL) {
                if (strncmp(param1, "inner", sizeof("inner") - 1) == 0) {
                        inner_circle = true;
                } else if (strncmp(param1, "outer", sizeof("outer") - 1) == 0) {
                        inner_circle = false;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dac_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_dac_command);
                return pdFALSE;
        }
        /* get paremeter 2 */
        if (param2 != NULL) {
                if (strncmp(param2, "tia", sizeof("tia") - 1) == 0) {
                        inject_num = 0;
                } else if (strncmp(param2, "hpf1", sizeof("hpf1") - 1) == 0) {
                        inject_num = 1;
                } else if (strncmp(param2, "vga1", sizeof("vga1") - 1) == 0) {
                        inject_num = 2;
                } else if (strncmp(param2, "hpf2", sizeof("hpf2") - 1) == 0) {
                        inject_num = 3;
                } else if (strncmp(param2, "vga2", sizeof("vga2") - 1) == 0) {
                        inject_num = 4;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dac_command);
                        return pdFALSE;
                }
        } else {
                inject_num = 1;
        }
        /* get paremeter 3 */
        if (param3 != NULL) {
                if (strncmp(param3, "tia", sizeof("tia") - 1) == 0) {
                        out_num = 0;
                } else if (strncmp(param3, "hpf1", sizeof("hpf1") - 1) == 0) {
                        out_num = 1;
                } else if (strncmp(param3, "vga1", sizeof("vga1") - 1) == 0) {
                        out_num = 2;
                } else if (strncmp(param3, "vga2", sizeof("vga2") - 1) == 0) {
                        out_num = 3;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dac_command);
                        return pdFALSE;
                }
        } else {
                out_num = 3;
        }

        /* get paremeter 4 */
        if (param4 != NULL) {
                if (strncmp(param4, "adc", sizeof("adc") - 1) == 0) {
                        adc_dbg_en = true;
                } else {
                        adc_dbg_en = false;
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dac_command);
                        return pdFALSE;
                }
        } else {
                adc_dbg_en = false;
        }

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif

        /* call DAC function */
        float peak_power[4];
        baseband_dac_playback(bb_hw, inner_circle, inject_num, out_num, adc_dbg_en, peak_power);
        return pdFALSE;
}

/* bb_hil command */
/* TODO, bb_hil should be tested with FPGA and GUI, when they are ready */
static const CLI_Command_Definition_t bb_hil_command = {
        "bb_hil",
        "bb_hil \n\r"
        "\tTest for bb HIL mode (GPIO or AHB input).\n\r"
        "\tUsage: bb_hil <start/stop> <gpio/ahb> <fft1d/fft2d/null> <frame_num>\n\r",
        bb_hil_command_handler,
        -1
};

static BaseType_t bb_hil_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3, *param4;
        BaseType_t len1, len2, len3, len4;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);

        baseband_t *bb = baseband_get_cur_bb();
        baseband_hw_t *bb_hw = &bb->bb_hw;
        bool hil_start = false;
        bool hil_mux = false;
        uint8_t dmp_mux = 0;

        /* get paremeter 1 */
        if (param1 != NULL) {
                if (strncmp(param1, "start", sizeof("start") - 1) == 0) {
                        hil_start = true;
                } else if (strncmp(param1, "stop", sizeof("stop") - 1) == 0) {
                        baseband_t* bb = baseband_get_rtl_frame_type();
                        baseband_stop(bb);
                        return pdFALSE;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_hil_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_hil_command);
                return pdFALSE;
        }

        /* get paremeter 2 */
        if (param2 != NULL) {
                if (strncmp(param2, "ahb", sizeof("ahb") - 1) == 0) {
                        hil_mux = false;
                } else if (strncmp(param2, "gpio", sizeof("gpio") - 1) == 0) {
                        hil_mux = true;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_hil_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_hil_command);
                return pdFALSE;
        }

        /* get paremeter 3 */
        /* hil from ahb is not necessary to dump data */
        if (param3 != NULL) {
                if (strncmp(param3, "fft1d", sizeof("fft1d") - 1) == 0) {
                        dmp_mux = 1;
                } else if (strncmp(param3, "fft2d", sizeof("fft2d") - 1) == 0) {
                        dmp_mux = 2;
                } else if (strncmp(param3, "null", sizeof("null") - 1) == 0) {
                        dmp_mux = 0;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_hil_command);
                        return pdFALSE;
                }
        } else {
                dmp_mux = 0; // no dump
        }

        int32_t frame_num = 1;
        /* get paremeter 4 */
        if (param4 != NULL) {
                frame_num = strtol(param4, NULL, 0);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_hil_command);
                return pdFALSE;
        }

        /* HIL on */
        if (true == hil_start) {
                stream_on_en = true;
                baseband_hil_on(bb_hw, hil_mux, dmp_mux, frame_num);
        }
        return pdFALSE;
}

/* bb_dbgdump, dumping cfar and doa debug data to extra 1MB memory in MEM_BUF */
static const CLI_Command_Definition_t bb_dbgdump_command = {
        "bb_dbgdump",
        "bb_dbgdump \n\r"
        "\tTest for bb debug-data dump.\n\r"
        "\tUsage: bb_dbgdump <cfar/dbf>\n\r",
        bb_dbgdump_command_handler,
        -1
};

static BaseType_t bb_dbgdump_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        uint8_t dbg_mux = DBG_BUF_TAR_NONE;
        /* get 1st paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "cfar", sizeof("cfar") - 1) == 0) {
                        dbg_mux = DBG_BUF_TAR_CFR;   /* config cfar debug data to extra 1M in mem_buf */
                } else if (strncmp(param1, "dbf", sizeof("dbf") - 1) == 0) {
                        dbg_mux = DBG_BUF_TAR_BFM;   /* config doa debug data to extra 1M in mem_buf */
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbgdump_command);
                        return pdFALSE;
                }
                EMBARC_PRINTF("Enable %s debug data to extra 1MB\n", param1);
        } else {
                return pdFALSE;
        }

        baseband_t* bb = baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
        baseband_dbg_start(&bb->bb_hw, dbg_mux); /* start bb and dumping debug data to memory */

        /* If needed, post-processing function can be called here, as debug data is stored to extra 1M */

        return pdFALSE;
}

/* bb_dbgsam, config before/after filter, start point and end point in a chirp */
static const CLI_Command_Definition_t bb_dbgsam_command = {
        "bb_dbgsam",
        "bb_dbgsam \n\r"
        "\tTest for bb debug-sample-data settings.\n\r"
        "\tUsage: bb_dbgsam <bf/af> <begin_point = 0> <dbg_end = 511>\n\r",
        bb_dbgsam_command_handler,
        -1
};

static BaseType_t bb_dbgsam_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        BaseType_t len2;
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        BaseType_t len3;
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
        bool     dbg_src = SAM_DBG_SRC_BF;
        uint16_t dbg_bgn = 0;
        uint16_t dbg_end = 511;
        /* get 1st paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "bf", sizeof("bf") - 1) == 0) {
                        dbg_src = SAM_DBG_SRC_BF;
                } else if (strncmp(param1, "af", sizeof("af") - 1) == 0) {
                        dbg_src = SAM_DBG_SRC_AF;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbgsam_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbgsam_command);
                return pdFALSE;
        }

        /* get 2nd paremeter */
        if (param2 != NULL) {
                dbg_bgn = strtol(param2, NULL, 0);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbgsam_command);
                return pdFALSE;
        }

        /* get 3rd paremeter */
        if (param3 != NULL) {
                dbg_end = strtol(param3, NULL, 0);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_dbgsam_command);
                return pdFALSE;
        }

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_write_cascade_cmd(pcCommandString); /* spi tx CommandString to slave*/
#endif

        baseband_t* bb = baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
        baseband_hw_t *bb_hw = &bb->bb_hw;

        uint8_t old_bnk = BB_READ_REG(bb_hw, SYS_BNK_ACT);
        for (int i = 0; i < NUM_FRAME_TYPE; i++) {
                BB_WRITE_REG(bb_hw, SYS_BNK_ACT     , i);       /* bank selected */
                BB_WRITE_REG(bb_hw, SAM_DBG_SRC     , dbg_src); /* before or after down sampling */
                BB_WRITE_REG(bb_hw, SAM_SIZE_DBG_BGN, dbg_bgn);
                BB_WRITE_REG(bb_hw, SAM_SIZE_DBG_END, dbg_end);
        }
        BB_WRITE_REG(bb_hw, SYS_BNK_ACT, old_bnk);       /* bank restore */

        return pdFALSE;
}

#if INTER_FRAME_POWER_SAVE == 1
/* bb_interframe command */
static const CLI_Command_Definition_t bb_interframe_command = {
        "bb_interframe",
        "bb_interframe \n\r"
        "\tinterframe tx block or single tone.\n\r"
        "\tUsage: bb_interframe <block/single>\n\r",
        bb_interframe_command_handler,
        -1
};

static BaseType_t bb_interframe_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        bool tx_block = true;
        /* get paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "block", sizeof("block") - 1) == 0) {
                        tx_block = true;
                } else if (strncmp(param1, "single", sizeof("single") - 1) == 0) {
                        tx_block = false;
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_interframe_command);
                        return pdFALSE;
                }
        } else {
                tx_block = true;
        }

        baseband_t *bb = baseband_get_rtl_frame_type(); // align with hardware frame type

        /* block or single tone */
        if (tx_block == true) {
                baseband_interframe_power_save_init(&bb->bb_hw);
                EMBARC_PRINTF("----------- interframe tx blocked -----------\r\n");
        } else {
                int_disable(INT_BB_SAM);
                fmcw_radio_adc_fmcwmmd_ldo_on(&bb->radio, true);
                fmcw_radio_lo_on(&bb->radio, true);
                fmcw_radio_rx_on(&bb->radio, true);
                fmcw_radio_tx_restore(&bb->radio);
                EMBARC_PRINTF("----------- interframe tx single tone -----------\r\n");
        }
        return pdFALSE;
}
#endif //INTER_FRAME_POWER_SAVE

/* bb_agc_dbg_command */
static const CLI_Command_Definition_t bb_agc_dbg_command = {
        "bb_agc_dbg",
        "bb_agc_dbg \n\r"
        "\tDump baseband monitoring-related agc register. \n\r"
        "\tUsage: bb_agc_dbg <cod/sat/max/state>\n\r",
        bb_agc_dbg_command_handler,
        -1
};

static BaseType_t bb_agc_dbg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        int item = 0;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        /* get parameter 1, 2*/
        if (param1 != NULL) {
                if (strncmp(param1, "cod", 3) == 0)
                        item = 0;
                else if (strncmp(param1, "sat", 3) == 0)
                        item = 1;
                else if (strncmp(param1, "max", 3) == 0)
                        item = 2;
                else if (strncmp(param1, "state", 5) == 0)
                        item = 3;
                else
                        item = -1;
        } else {
                item = -1;
        }
        if (item == -1) {
                print_help(pcWriteBuffer, xWriteBufferLen, &bb_agc_dbg_command);
                return pdFALSE;
        } else {
                baseband_agc_dbg_reg_dump(NULL,item);
        }
        return pdFALSE;
}

/* fft power print(default 11*11) commond */
static const CLI_Command_Definition_t fftp_command = {
        "fftp",
        "fftp \n\r"
        "\tfft power print \n\r"
        "\tUsage: fftp [rng_center] [vel_center] [frame_num]\n\r",
        fftp_command_handler,
        -1
};

static BaseType_t fftp_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2, *param3;
        BaseType_t len1, len2, len3;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);

        uint32_t nframe;
        int rng_center;
        int vel_center;

        if (param1 != NULL) {
                rng_center = strtol(param1, NULL, 0);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &fftp_command);
                return pdFALSE;
        }

        if (param2 != NULL) {
                vel_center = strtol(param2, NULL, 0);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &fftp_command);
                return pdFALSE;
        }

        if (param3 != NULL)
                nframe = strtol(param3, NULL, 0);
        else
                nframe = 20;


        baseband_t* bb = baseband_get_bb(0);
        baseband_hw_t *bb_hw = &bb->bb_hw;
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(bb_hw, baseband_t, bb_hw)->cfg;

        uint32_t raw_data;
        complex_t complex_fft;
        float* fft_pwr;

        /* check the boundary */
        int rng_bgn = rng_center - FFTP_SIZE_HALF;
        int rng_end = rng_center + FFTP_SIZE_HALF;
        int vel_bgn = vel_center - FFTP_SIZE_HALF;
        int vel_end = vel_center + FFTP_SIZE_HALF;

        if ( rng_center < FFTP_SIZE_HALF)
                rng_bgn = 0;
        if ( rng_end > ((cfg->rng_nfft)/2 - 1))
                rng_end = (cfg->rng_nfft)/2 - 1;

        if ( vel_bgn < FFTP_SIZE_HALF)
                vel_bgn = 0;
        if ( vel_end > ((cfg->vel_nfft) - 1))
                vel_end = (cfg->vel_nfft) - 1;


        /* Malloc */
        uint32_t fft_pwr_size = FFTP_SIZE * FFTP_SIZE * sizeof(float);
        if (fft_pwr_size > xPortGetFreeHeapSize()) {
                EMBARC_PRINTF("fft_pwr Malloc Failed, free heap size is %d bytes\r\n", xPortGetFreeHeapSize());
                return pdFALSE;
        }

        fft_pwr = (float*)pvPortMalloc(fft_pwr_size);
        if (fft_pwr == NULL) {
                EMBARC_PRINTF(" fft_pwr Malloc Error\r\n");
                return pdFALSE;
        }
        memset(fft_pwr, 0, fft_pwr_size); /* init 0 */


        /* save baseband status */
        uint8_t old_mem_act = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
        uint8_t old_buf_store = baseband_switch_buf_store(bb_hw, SYS_BUF_STORE_FFT);
        uint16_t old_status_en = BB_READ_REG(bb_hw, SYS_ENABLE);
        uint16_t bb_status_en =  SYS_ENA(SAM    , true)
                                |SYS_ENA(FFT_2D , true);


        /* fft data processing */
        for (uint32_t frame_cnt = 0; frame_cnt < nframe; frame_cnt ++) {

                /* start baseband */
                baseband_start_with_params(bb, true, true, bb_status_en, true, BB_IRQ_ENABLE_SAM_DONE, false);

                /* wait done */
                while (baseband_hw_is_running(bb_hw) == false)
                        ;
                while (baseband_hw_is_running(bb_hw) == true)
                        ;

                /* read and accumulate fft2d power */
                old_mem_act = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                for (int j = rng_bgn ; j <= rng_end; j++) {
                        for (int i = vel_bgn; i <= vel_end; i++) {
                                for (int ant_idx = 0; ant_idx < MAX_NUM_RX; ant_idx++) {
                                        raw_data = baseband_hw_get_fft_mem(bb_hw, ant_idx, j, i, 0);
                                        complex_fft = cfl_to_complex(raw_data, 14, 14, true, 4, false);

                                        int buf_adr = (j - rng_bgn) * FFTP_SIZE + (i - vel_bgn);
                                        fft_pwr[buf_adr] += (complex_fft.r * complex_fft.r + complex_fft.i * complex_fft.i);
                                }
                        }

                }
        }


        /* fft print */
        EMBARC_PRINTF("rng\\dpl");
        for (int i = vel_bgn ; i <= vel_end; i++)
                    EMBARC_PRINTF("%7d ", i);

        for (int j = rng_bgn ; j <= rng_end; j++) {
                for (int i = vel_bgn ; i <= vel_end; i++) {
                        if (i == vel_bgn)
                                EMBARC_PRINTF("\r\n%4d:   ", j);

                        int buf_adr = (j - rng_bgn) * FFTP_SIZE + (i - vel_bgn);
                        if (i == vel_end)
                                EMBARC_PRINTF("%4.2f ", fft_pwr[buf_adr]);
                        else
                                EMBARC_PRINTF("%4.2f ", fft_pwr[buf_adr]);
                }
        }

        /* restore baseband status */
        baseband_switch_mem_access(bb_hw, old_mem_act);
        BB_WRITE_REG(bb_hw, SYS_ENABLE, old_status_en);
        baseband_switch_buf_store(bb_hw, old_buf_store);
        vPortFree(fft_pwr);

        return pdFALSE;
}

#if (NUM_FRAME_TYPE > 1)
/* frame interleaving reconfig */
static const CLI_Command_Definition_t fi_command = {
        "fi",
        "fi \n\r"
        "\tframe interleaving set FIXED <0/1/2/3> or returned to default strategy \n\r"
        "\tUsage: fi <set/rtn> <frame_type_index =0> \n\r",
        fi_command_handler,
        -1
};

static BaseType_t fi_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2;
        BaseType_t len1, len2;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        uint8_t no_effect = 1; // no effect in FIXED strategy

        /* get paremeter */
        if (param1 != NULL) {
                if (strncmp(param1, "set", sizeof("set") - 1) == 0) {
                        if (param2 != NULL) {
                                uint8_t frame_type_idx = strtol(param2, NULL, 0);

                                if (frame_type_idx < NUM_FRAME_TYPE) {
                                        baesband_frame_interleave_strategy_set(FIXED, no_effect, frame_type_idx, no_effect);
                                        EMBARC_PRINTF("\tFIXED %d\n\r", frame_type_idx);
                                } else {
                                        EMBARC_PRINTF("\tindex should < %d\n\r", NUM_FRAME_TYPE);
                                        return pdFALSE;
                                }

                        } else {
                                baesband_frame_interleave_strategy_set(FIXED, no_effect, 0, no_effect);
                                EMBARC_PRINTF("\tFIXED 0\n\r");
                        }

                } else if (strncmp(param1, "rtn", sizeof("rtn") - 1) == 0) {
                        baesband_frame_interleave_strategy_return();
                        EMBARC_PRINTF("\treturn to default strategy\n\r");
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &fi_command);
                        return pdFALSE;
                }
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &fi_command);
                return pdFALSE;
        }

        return pdFALSE;
}
#endif // (NUM_FRAME_TYPE > 1)

/* bb_sambuf command, print sample buffer data */
static const CLI_Command_Definition_t bb_sambuf_command = {
        "bb_sambuf",
        "bb_sambuf \n\r"
        "\tsample buffer data print \n\r"
        "\tUsage: bb_sambuf \n\r",
        bb_sambuf_command_handler,
        -1
};

static BaseType_t bb_sambuf_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

        baseband_hw_t *bb_hw = &baseband_get_cur_bb()->bb_hw;
        uint32_t size_rng_buf = (BB_READ_REG(bb_hw, SYS_SIZE_RNG_BUF) + 1)/2; // adc data is 16 bits, 2 adc data in one memory entry(32 bits)

        // read memory
        uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_SAM);

        for (int i = 0; i < 2; i++) {                    // velocity index, only 2 chirps stored
                for (int k = 0; k < MAX_NUM_RX; k++)  {  // antenna index
                        for (int j = 0; j < size_rng_buf; j++) {  // range index
                                uint32_t dat = baseband_hw_get_sam_buf(bb_hw, i, j, k);

                                if (j % 4 == 3)          // 4 data printed in 1 line
                                        EMBARC_PRINTF("0x%08x\n\r", dat);
                                else
                                        EMBARC_PRINTF("0x%08x, ", dat);
                                MDELAY(1); /* add delay to improve the validity of the serial data*/
                        }
                }
        }

        baseband_switch_mem_access(bb_hw, old);
        return pdFALSE;
}

#ifdef CHIP_CASCADE
/* bb_sync command, enable sync irq for cascade */
static const CLI_Command_Definition_t bb_sync_command = {
        "bb_sync",
        "bb_sync \n\r"
        "\tsync irq for cascade, master rx irq and slave tx irq\n\r"
        "\tUsage: bb_sync <tx/rx>\n\r",
        bb_sync_command_handler,
        -1
};

static BaseType_t bb_sync_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

        /* get paremeter */
        if (param1 != NULL) {
                if ((strncmp(param1, "rx", sizeof("rx") - 1) == 0) && (chip_cascade_status() == CHIP_CASCADE_MASTER)) {

                        baseband_cascade_sync_init();
                        baseband_cascade_sync_wait(NULL); // rx one irq of sync
                        EMBARC_PRINTF("mst, wait and rx one irq \r\n");

                } else if ((strncmp(param1, "tx", sizeof("tx") - 1) == 0) && (chip_cascade_status() == CHIP_CASCADE_SLAVE)) {

                        cascade_s2m_sync_bb(); // tx one irq of sync
                        EMBARC_PRINTF("slv, tx one irq \r\n");

                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &bb_sync_command);
                }

        } else if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                baseband_cascade_sync_init();
                EMBARC_PRINTF("irq of sync enabled \r\n");
        }

        return pdFALSE;
}
#endif // CHIP_CASCADE

/*bb saturation counts for different tx groups do beamforming */
static const CLI_Command_Definition_t Txbf_saturation_command = {
        "Txbf_saturation",
        "Txbf_saturation \n\r"
        "\tOutput saturation counts of CHs under different Tx phase.\n\r"
        "\tUsage: Txbf_saturation <bitmux of tx group (at most 4bits):%x>\n\r",
        Txbf_saturation_command_handler,
        -1
};

static BaseType_t Txbf_saturation_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        if (param1 != NULL) {
                unsigned int tx_g  = (unsigned int) strtol(param1, NULL, 0);
                /* get RTL bank selected*/
                baseband_t* bb = baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
                baseband_hw_t *bb_hw = &bb->bb_hw;
                Txbf_bb_satur_monitor(bb_hw, tx_g);
        } else {
                print_help(pcWriteBuffer, xWriteBufferLen, &Txbf_saturation_command);
        }
        return pdFALSE;
}

/* bb_rst command */
static const CLI_Command_Definition_t bb_rst_command = {
        "bb_rst",
        "bb_rst \n\r"
        "\treset of bb core.\n\r"
        "\tUsage: bb_rst\n\r",
        bb_rst_command_handler,
        -1
};

static BaseType_t bb_rst_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        bb_core_reset(1);           // reset bb_core
        bb_core_reset(0);           // deassert reset
        EMBARC_PRINTF("bb_core_reset done!\n\r");
        return pdFALSE;
}

void baseband_cli_commands(){
        if (baseband_cli_registered)
                return;
        FreeRTOS_CLIRegisterCommand(&scan_command);
        FreeRTOS_CLIRegisterCommand(&bb_reg_command);
        FreeRTOS_CLIRegisterCommand(&bb_regdump_command);
        FreeRTOS_CLIRegisterCommand(&bb_tbldump_command);
        FreeRTOS_CLIRegisterCommand(&bb_init_command);
        FreeRTOS_CLIRegisterCommand(&bb_datdump_command);
        FreeRTOS_CLIRegisterCommand(&bb_dc_command);
        FreeRTOS_CLIRegisterCommand(&ant_calib_command);
        FreeRTOS_CLIRegisterCommand(&tx_ant_phase_calib_command);
        FreeRTOS_CLIRegisterCommand(&bb_datdump_serport_command);
        FreeRTOS_CLIRegisterCommand(&bb_dbg_urt_command);
        FreeRTOS_CLIRegisterCommand(&radar_param_show_command);
        FreeRTOS_CLIRegisterCommand(&bb_fftdump_command);
        FreeRTOS_CLIRegisterCommand(&bb_test_command);
        FreeRTOS_CLIRegisterCommand(&bb_bist_command);
        FreeRTOS_CLIRegisterCommand(&bb_dac_command);
        FreeRTOS_CLIRegisterCommand(&bb_hil_command);
#if INTER_FRAME_POWER_SAVE == 1
        FreeRTOS_CLIRegisterCommand(&bb_interframe_command);
#endif //INTER_FRAME_POWER_SAVE
        FreeRTOS_CLIRegisterCommand(&bb_dbgdump_command);
        FreeRTOS_CLIRegisterCommand(&bb_dbgsam_command);
        FreeRTOS_CLIRegisterCommand(&bb_agc_dbg_command);
        FreeRTOS_CLIRegisterCommand(&fftp_command); /* fft power print commond */
#if (NUM_FRAME_TYPE > 1)
        FreeRTOS_CLIRegisterCommand(&fi_command);  /* frame interleaving reconfig */
#endif // (NUM_FRAME_TYPE > 1)
        FreeRTOS_CLIRegisterCommand(&bb_sambuf_command);  /* frame interleaving reconfig */

#ifdef CHIP_CASCADE
        FreeRTOS_CLIRegisterCommand(&bb_sync_command);  /* enable sync irq for cascade */
#endif // CHIP_CASCADE
        FreeRTOS_CLIRegisterCommand(&Txbf_saturation_command);
        FreeRTOS_CLIRegisterCommand(&bb_rst_command);
        baseband_cli_registered = true;
}
