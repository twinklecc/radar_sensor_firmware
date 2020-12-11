#include "embARC.h"
#include "baseband.h"
#include "baseband_cli.h"
#include "sensor_config.h"

#include "embARC_error.h"

#include "calterah_limits.h"
#include "radio_ctrl.h"
#include "cascade.h"
#include "baseband_cas.h"
#include "baseband_task.h"
#include "apb_lvds.h"
#include "baseband_dpc.h"
#include "dbgbus.h"

#ifdef UNIT_TEST
#define UDELAY(us)
#else
#define UDELAY(us)  chip_hw_udelay(us);
#endif

#define STREAM_ON_EN 1

extern SemaphoreHandle_t mutex_frame_count;
extern int32_t frame_count;

#ifdef CHIP_CASCADE
extern QueueHandle_t queue_cas_sync;
static bool cas_sync_flag = false;
void baseband_cascade_sync_wait(baseband_hw_t *bb_hw);
void baseband_cascade_sync_init();
#define SHAKE_CODE 0xABCD
#endif

static baseband_t baseband[MAX_FRAME_TYPE];
static uint8_t current_frame_type = 0;

static bool     fi_en  = true; /* Frame InterLeaving reconfig enable */
static uint32_t fi_cnt = 0;    /* Frame InterLeaving reconfig count */

static frame_intrlv_t fi        = FI_ROTATE;
static frame_intrlv_t fi_return = FI_ROTATE; /* save the initial strategy */

void baseband_dump_stop(void);

int32_t baseband_init(baseband_t *bb)
{
        int32_t status = E_OK;
        // Duplicate ID to ease access
        bb->radio.frame_type_id = bb->bb_hw.frame_type_id;
        status = fmcw_radio_init(&bb->radio);
        if (status != E_OK)
                return status;
        radar_param_config(&bb->sys_params);
        baseband_hw_init(&bb->bb_hw);
        bb->track = track_get_track_ctrl();
        track_init_sub(bb->track, bb);
        cluster_init(&bb->cluster);
        baseband_data_proc_init();
        return status;
}

void baseband_clock_init()
{
        bb_enable(1); /* bb clock enable */
}

int32_t baseband_dump_init(baseband_t *bb, bool sys_buf_store)
{
        baseband_hw_dump_init(&bb->bb_hw, sys_buf_store);
        return E_OK;
}

void baseband_start(baseband_t *bb)
{
#if INTER_FRAME_POWER_SAVE == 1
        fmcw_radio_adc_fmcwmmd_ldo_on(&bb->radio, true);
        fmcw_radio_lo_on(&bb->radio, true);
        fmcw_radio_rx_on(&bb->radio, true);
        fmcw_radio_tx_restore(&bb->radio);
#endif
        fmcw_radio_sdm_reset(&bb->radio);
        UDELAY(100);

        dmu_adc_reset(); // ADC should reset in cascade

        track_start(bb->track);
        baseband_hw_start(&bb->bb_hw);

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                baseband_cascade_sync_wait(&bb->bb_hw); // wait for slave ready signal
#endif

        if (!fmcw_radio_is_running(&bb->radio))
                fmcw_radio_start(&bb->radio);

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_SLAVE)
                cascade_s2m_sync_bb();                  // slave sents ready signal
#endif

}

void baseband_start_with_params(baseband_t *bb, bool radio_en, bool tx_en, uint16_t sys_enable, bool cas_sync_en, uint8_t sys_irp_en, bool track_en)
{
        if (tx_en == true)
                fmcw_radio_tx_restore(&bb->radio);
#if INTER_FRAME_POWER_SAVE == 1
        /* Recover the radio setting from power save mode */
        fmcw_radio_adc_fmcwmmd_ldo_on(&bb->radio, true);
        fmcw_radio_lo_on(&bb->radio, true);
        fmcw_radio_rx_on(&bb->radio, true);
#endif
        fmcw_radio_sdm_reset(&bb->radio);
        UDELAY(100);

        dmu_adc_reset(); // ADC should reset in cascade

        if (track_en == true)
                track_start(bb->track);
        baseband_hw_start_with_params(&bb->bb_hw, sys_enable, sys_irp_en);

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER && cas_sync_en)
                baseband_cascade_sync_wait(&bb->bb_hw); // wait for slave ready signal
#endif

        if ((!fmcw_radio_is_running(&bb->radio)) && radio_en==true)
                fmcw_radio_start(&bb->radio);

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_SLAVE && cas_sync_en)
                cascade_s2m_sync_bb();                  // slave sents ready signal
#endif

}

void baseband_stop(baseband_t *bb)
{
        initial_flag_set(true);
        fmcw_radio_stop(&bb->radio);
        baseband_hw_stop(&bb->bb_hw);
        track_stop(bb->track);

        baseband_dump_stop(); /* stream data dump stop */
        bb_clk_restore();     /* restore bb clock after dumping sample debug data */
        if (true == baseband_data_proc_req()) {
                baseband_data_proc_init(); /* re-init data process chain */
                dmu_hil_input_mux(HIL_AHB); /* trun off dbgbus input */
                lvds_dump_done();           /* done signal to  fpga board */
        }
}

baseband_t* baseband_get_bb(uint32_t idx)
{
        if (idx >= NUM_FRAME_TYPE)
                return NULL;
        else
                return &baseband[idx];
}

baseband_t* baseband_get_cur_bb()
{
        sensor_config_t *cfg = sensor_config_get_cur_cfg();
        return cfg->bb;
}

uint8_t baseband_get_cur_idx()
{
        return sensor_config_get_cur_cfg_idx();
}

uint8_t baseband_get_cur_frame_type()
{
        return current_frame_type;
}

void baseband_set_cur_frame_type(uint8_t ft)
{
        current_frame_type = ft;
}

baseband_t* baseband_get_rtl_frame_type()
{
        current_frame_type = baseband_read_reg(NULL, BB_REG_FDB_SYS_BNK_ACT); /* read back the bank selected in RTL */
        baseband_t* bb = baseband_get_bb(current_frame_type);
        baseband_write_reg(&bb->bb_hw, BB_REG_SYS_BNK_ACT, current_frame_type);     /* match the bank accessed by CPU */
        sensor_config_set_cur_cfg_idx(current_frame_type);
        return bb;
}

void baseband_frame_type_reset()
{
        baseband_write_reg(NULL, BB_REG_SYS_BNK_RST,  1);
        fmcw_radio_frame_type_reset(NULL);
}

/* configure the loop pattern in frame interleaving */
baseband_t* baseband_frame_interleave_cfg(uint8_t frame_loop_pattern)
{
        baseband_frame_interleave_pattern(NULL, frame_loop_pattern);
        fmcw_radio_frame_interleave_pattern(NULL, frame_loop_pattern);
        return baseband_get_rtl_frame_type(); /* read back the bank selected in RTL */
}

/* set new strategy in frame interleaving */
void baesband_frame_interleave_strategy_set(uint8_t strategy, uint32_t sw_num, uint8_t sel_0, uint8_t sel_1)
{
                fi.strategy = strategy;       // FIXMED or ROTATE or AIR_CONDITIONER or VELAMB or customized by yourself
                fi.sw_num   = sw_num;         // loop period or switch number
                fi.sel_0    = sel_0;          // 1st frame type used
                fi.sel_1    = sel_1;          // 2nd frame type used
}

/* return the default strategy in frame interleaving */
void baesband_frame_interleave_strategy_return()
{
        fi = fi_return;
}

/* clear the fi_cnt in frame interleaving */
void baesband_frame_interleave_cnt_clr()
{
        fi_en  = true;
        fi_cnt = 0;
}

/* reconfigure the loop pattern in frame interleaving, just a reference code here*/
baseband_t* baseband_frame_interleave_recfg()
{
        baseband_t* bb = baseband_get_rtl_frame_type();

        switch (fi.strategy) {

        case FIXED : // fix one frame type
                if (fi_en && fi_cnt == 0) {
                        bb = baseband_frame_interleave_cfg(fi.sel_0);
                        fi_en = false;
                }
                break;

        case ROTATE : // alwasys loop all available frame types
                bb = baseband_frame_interleave_cfg(fi_cnt % fi.sw_num);
                break;

        case AIR_CONDITIONER : // use fi.sel_0 first, then fi.sel_1
                if (fi_en) {
                        if (fi_cnt == 0) {
                                bb = baseband_frame_interleave_cfg(fi.sel_0);
                        } else if (fi_cnt == (fi.sw_num - 1)) {
                                bb = baseband_frame_interleave_cfg(fi.sel_1);
                                fi_en = false;
                        }
                }
                break;

        case VELAMB : // always loop
                if (fi_cnt % fi.sw_num  == 2)
                        bb = baseband_frame_interleave_cfg(fi.sel_1);
                else
                        bb = baseband_frame_interleave_cfg(fi_cnt % fi.sw_num);
                break;

        // you can customize a new pattern here

        default:
                break;

        }

        if (fi_en) {
                if (fi_cnt < (fi.sw_num - 1))
                        fi_cnt++;
                else
                        fi_cnt = 0;
        }

        return bb;
}

/*parse each element of cfg->tx_groups to work state in a chirp way*/
bool bit_parse(uint32_t num, uint16_t bit_mux[])
{
        bool valid_mux = true;
        for(int i=0; i<MAX_NUM_TX;i++){
                bit_mux[i] = (num >> (4*i)) & 0x3;   //output tx work status in chirp i, bit_mux=0 tx off; bit_mux=1 tx on in phase; bit_mux=2  tx on opposite phase
                if (bit_mux[i] == 3) {
                        valid_mux = false;
                        break;
                }
        }
        return valid_mux;
}

/*get the phase of the first chirp, when more than 1 tx is on in the first chirp, return the phase of antenna with small index
mainly used to judge the transmittion order of BPM*/
uint16_t get_ref_tx_antenna(uint32_t patten[])
{
        uint16_t refer_phase = 0;
        uint16_t bit_mux[MAX_NUM_TX] = {0, 0, 0, 0};
        for(int i = 0; i < MAX_NUM_TX; i++) {
                bit_parse(patten[i],bit_mux);
                if (bit_mux[0]>0) {
                        refer_phase = bit_mux[0];
                        break;
                }
        }
        return refer_phase;
}

/*to get the tx mux in every chirp by parse all patten*/
bool get_tdm_tx_antenna_in_chirp(uint32_t patten[], uint8_t chip_idx, uint32_t chirp_tx_mux[])
{
        uint16_t bit_mux[MAX_NUM_TX] = {0, 0, 0, 0};
        int32_t tx_groups_tmp[MAX_NUM_TX][MAX_NUM_TX] = {0};
        uint32_t ret = 0;
        for(int i = 0; i < MAX_NUM_TX; i++) {          //Tx loop
                bit_parse(patten[i],bit_mux);
                for(int c = 0; c < MAX_NUM_TX; c++) {  //chirp loop
                        if (bit_mux[c] > 0)
                                tx_groups_tmp[i][c] = 1;  //output in which chirps Txi transmit
                }
        }
        for(int c = 0; c <= chip_idx; c++) {  //chirp loop
                ret = 0;
                for(int i = 0; i < MAX_NUM_TX; i++) {          //Tx loop
                        if(tx_groups_tmp[i][c] > 0) {
                                ret |= (1 << i);
                        }
                }
                if (ret == 0) {
                        return false;     //mean all tx off in chirp c
                }
                chirp_tx_mux[c] = ret;    //ret is the mux presentation of txs which are on in chirp c
        }
        return true;
}

/*check bit_mux of a tx if satisfy the standard bpm pattern
for bpm va = 2, pattern 0 is x x , pattern 1 is x -x
for bpm va = 4, pattern 0 is x x x x, pattern 1 is x -x x -x, pattern 2 is x x -x -x, pattern 3 is x -x -x x*/
int8_t bpm_patten_check(uint16_t bit_mux[], uint8_t chip_idx, uint16_t refer_phase, uint8_t pat_num)
{
        int loop_num = chip_idx/2 + 1;     //for patten 1 2 3 check
        int8_t p = 0;
        for (int m = 0; m <= pat_num; m++ ) {
                p = 0;      //indicate which pattern bit_mux shows. p = -1 :mean invalid pattern
                switch (m) {
                case 0:
                        for(int j=0;j<=chip_idx;j++){
                               if (bit_mux[j] != refer_phase) {
                                       p = -1;
                                       break;
                               }
                        }
                        if (p != -1)
                                p = 0;         // patten 0 gotten     1 1 1 1
                        break;
                case 1:
                        for (int j = 0; j < loop_num; j++) {
                                if( (bit_mux[2*j] != refer_phase) || ((bit_mux[2*j] + bit_mux[2*j + 1]) != 3) ){
                                        p = -1;
                                        break;
                                }
                        }
                        if (p != -1)
                                p = 1;         // patten 1 gotten  1 -1 1 -1
                        break;
                case 2:
                        for (int j = 0; j < loop_num; j++) {
                                uint16_t phase = (j == 0)?refer_phase:(3-refer_phase);
                                if( (bit_mux[2*j] != phase) || (bit_mux[2*j] != bit_mux[2*j + 1]) ){
                                        p = -1;
                                        break;
                                }
                        }
                        if (p != -1)
                                p = 2;         // patten 2 gotten  1 1 -1 -1
                        break;
                case 3:
                       for (int j = 0; j < loop_num; j++) {
                               uint16_t phase = (j == 0)?refer_phase:(3-refer_phase);
                                if( (bit_mux[2*j] != phase) || ( (bit_mux[2*j] + bit_mux[2*j + 1]) != 3) ){
                                        p = -1;
                                        break;
                                }
                       }
                        if (p != -1)
                                p = 3;         // patten 3 gotten  1 -1 -1 1
                        break;
                default :
                        break;
                }
                if (p != -1)
                        break;
        }
        return p;
}

int8_t get_bpm_tx_antenna_in_chirp(uint32_t patten[], uint8_t chip_idx, uint32_t chirp_tx_mux[])
{
        uint16_t bit_mux[MAX_NUM_TX] = {0, 0, 0, 0};
        int8_t checker = 0;     //checker = -1: pattern not found
        uint16_t refer_phase = 0;
        int i = 0;
        int32_t tx_groups[MAX_NUM_TX] = {0, 0, 0, 0};
        uint32_t ret = 0;
        refer_phase = get_ref_tx_antenna(patten);
        if (refer_phase == 0) {       //mean all tx off in chirp 0
                checker = -1;
                return checker;
        }
        // get patten 0 first
        for(i = 0; i < MAX_NUM_TX; i++) {
                bit_parse(patten[i],bit_mux);
                checker = bpm_patten_check(bit_mux, chip_idx, refer_phase, 0);
                if (checker==0) {
                        tx_groups[i] = 1;
                        break;
                } else {
                        continue;
                }
        }
        if (checker == -1)
                return  checker;
        // get other pattern
        for(int j = 0; j < MAX_NUM_TX; j++) {
                if (j == i-1)
                        continue;
                bit_parse(patten[j],bit_mux);
                uint8_t patten_num = (chip_idx == 1)?1:3;      //chip_idx == 1 mean varray = 2
                checker = bpm_patten_check(bit_mux, chip_idx, refer_phase, patten_num);
                if (checker!=-1) {
                        tx_groups[j] = checker + 1;
                }
        }
        /*generate the tx mux which are on in the same chirp*/
        for (int c=0; c<= chip_idx; c++) {
                ret = 0;
                for(int i = 0; i < MAX_NUM_TX; i++) {
                        if (tx_groups[i] == (c+1))
                               ret |= (1<<i);
                }
                if (ret == 0) {
                        checker = -1;
                        return checker;
                }
                chirp_tx_mux[c] = ret;
        }
        checker = 4;     //success over
        return checker;
}

void baseband_dump_stop(void)
{
        bool param = baseband_stream_off_req();
        if (param == STREAM_ON_EN) { /* data dump finish */
#ifdef CHIP_CASCADE
                lvds_dump_stop();
#else
                dbgbus_dump_stop();
#endif // CHIP_CASCADE
                baseband_switch_buf_store(NULL, SYS_BUF_STORE_FFT);
        }
}

void baseband_hil_input_enable(baseband_t *bb)
{
        dbgbus_hil_ready(); /* ready signal of hil sent to FPGA data collection board */
        dmu_hil_input_mux(HIL_GPIO);
}

void baseband_hil_input_disable(baseband_t *bb)
{
        dmu_hil_input_mux(HIL_AHB); /* trun off dbgbus input */
}

void baseband_hil_dump_enable(baseband_t *bb)
{
        fmcw_radio_lvds_on(NULL, true);
        lvds_dump_reset();  /* reset signal to fpga board */
}

void baseband_hil_dump_done(baseband_t *bb)
{
        lvds_dump_done();  /* done signal to  fpga board */
}

void baseband_scan_stop(void)
{
        xSemaphoreTake(mutex_frame_count, portMAX_DELAY);
        frame_count = 0;
        xSemaphoreGive(mutex_frame_count);
}

#ifdef CHIP_CASCADE
void baseband_cascade_handshake()
{
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                EMBARC_PRINTF("wait for handshake ...\r\n");

                cascade_read_buf_req(portMAX_DELAY); /* wait buf */
                if (cascade_read_buf(0) == SHAKE_CODE)
                        EMBARC_PRINTF("handshake success!\r\n");

                cascade_read_buf_done();

                // cascade sync init after handshake
                baseband_cascade_sync_init();

        } else {
                EMBARC_PRINTF("tx handshake ...\r\n");

                cascade_write_buf_req();
                cascade_write_buf(SHAKE_CODE);
                cascade_write_buf_done();
        }

}

void baseband_cascade_sync_handler()
{
        uint32_t msg = 0;
        if (cas_sync_flag)
                xQueueSendFromISR(queue_cas_sync, (void *)&msg, 0);

}

void baseband_cascade_sync_wait(baseband_hw_t *bb_hw)
{
        uint32_t event = 0;

        if(xQueueReceive(queue_cas_sync, &event, portMAX_DELAY) == pdTRUE) {
                if (true == baseband_scan_stop_req()) {
                        uint16_t param = baseband_stream_off_req();
                        baseband_scan_stop_tx(CMD_SCAN_STOP, param); /* tx "scan stop" commond to slave*/
                        baseband_scan_stop(); /* scan stop for master itself */
                }
        }

}

void baseband_cascade_sync_init()
{
        cas_sync_flag = true;
}
#endif
