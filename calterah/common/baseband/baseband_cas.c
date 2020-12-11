#include <string.h>
#include "baseband_hw.h"
#include "baseband_cli.h"
#include "baseband_cas.h"
#include "sensor_config.h"
#include "FreeRTOS.h"
#include <stdlib.h>
#include <math.h>

#ifdef CHIP_CASCADE

extern QueueHandle_t queue_spi_cmd;

static int32_t baseband_merge_cfar(baseband_hw_t *bb_hw, uint8_t ant_chl_num, uint16_t mst_obj_num, uint16_t *slv_obj_num, uint16_t *rx_obj_num, uint32_t *mem_mac_offset, uint32_t *merge_num, cfr_info_t *cfr_info, volatile obj_info_t *obj_info)
{
        int32_t result = E_OK;
        uint32_t rx_buf_offset = 0;
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(bb_hw, baseband_t, bb_hw)->cfg;

        if (E_OK == cascade_read_buf_req(WAIT_TICK_NUM)) { /* wait 30ms */
                /* get slave object info */
                uint32_t buf_data = cascade_read_buf(BUF_SHIFT_INFO);     /* read cfar info of slave chip */
                *slv_obj_num = REG_H16(buf_data);
                *rx_obj_num = REG_L16(buf_data); // transmitted object number of slave

                /* merge */
                for (uint16_t i = 0; i < *rx_obj_num; i++) {
                        /* read slave: range, doppler */
                        rx_buf_offset = BUF_SIZE_INFO + i * (ant_chl_num + BUF_SIZE_CAFR);
                        uint32_t slv_rng_vel = cascade_read_buf(rx_buf_offset++);
                        uint32_t slv_amb_noi = cascade_read_buf(rx_buf_offset++);
                        //EMBARC_PRINTF("slv_rng_vel = %u\n", slv_rng_vel);
                        //EMBARC_PRINTF("slv_amb_noi = %u\n", slv_amb_noi);

                        uint32_t slv_rng_idx = (slv_rng_vel >> 18) & 0x3FF; // Extract slave rng_idx
                        uint32_t slv_vel_idx = (slv_rng_vel >> 4) & 0x3FF;  // Extract slave vel_idx

                        /* About cascade master and slave object infos merge type:
                         * determined by cfg->cas_obj_merg_typ:
                         *   0 : merge condition is diff between mst and slv obj's (r,v) is 0
                         *   1 : merge condition is diff between mst and slv obj's (r,v) is 0 or 1
                         *   2 : merge condition is based on slv obj's (r,v)
                         * */

                        if (cfg->cas_obj_merg_typ != 2) { // if not slave based merge
                                /* search master obj_info to pair slave obj_info */
                                for (uint16_t j = 0; j < mst_obj_num; j++) {  //  FIXME: To optimize
                                        baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
                                        uint32_t mst_rng_idx = obj_info[j].rng_idx;
                                        uint32_t mst_vel_idx = obj_info[j].vel_idx;
                                        uint8_t  merge_thres = 0;
                                        if (cfg->cas_obj_merg_typ == 0) {// if and merge
                                                merge_thres = 0;
                                                //EMBARC_PRINTF("MERGE_TYPE == AND_MERGE\n");
                                        } else if (cfg->cas_obj_merg_typ == 1) { // if loose and merge
                                                merge_thres = 1;
                                                //EMBARC_PRINTF("MERGE_TYPE == LOOSE_AND_MERGE\n");
                                        }
                                        if( (abs(mst_rng_idx-slv_rng_idx) <= merge_thres) &&
                                            (abs(mst_vel_idx-slv_vel_idx) <= merge_thres) ) {
                                                // Store the paired master obj_info
                                                cfr_info[*merge_num].rng_acc = obj_info[j].rng_acc;
                                                cfr_info[*merge_num].rng_idx = obj_info[j].rng_idx;
                                                cfr_info[*merge_num].vel_acc = obj_info[j].vel_acc;
                                                cfr_info[*merge_num].vel_idx = obj_info[j].vel_idx;
                                                cfr_info[*merge_num].amb_idx = obj_info[j].amb_idx;
                                                cfr_info[*merge_num].noi     = obj_info[j].noi;

                                                (*merge_num)++;

                                                // Store slave obj FFT peaks in MEM_MAC
                                                baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_MAC);
                                                for (uint8_t chl_index = 0; chl_index < ant_chl_num; chl_index++) {
                                                        uint32_t fft2d_data = cascade_read_buf(rx_buf_offset++);
                                                        //EMBARC_PRINTF("fft2d_data = %u\n", fft2d_data);
                                                        baseband_write_mem_table(bb_hw, *mem_mac_offset, fft2d_data);
                                                        (*mem_mac_offset)++;
                                                }
                                        }
                                }
                        } else { // if slave based merge
                                //EMBARC_PRINTF("MERGE_TYPE == SLAVE_BASE_MERGE\n");
                                // Store the slave obj_info
                                cfr_info[*merge_num].rng_acc = (slv_rng_vel >> 14) & 0xF;  // Extract slave rng_acc
                                cfr_info[*merge_num].rng_idx = slv_rng_idx;
                                cfr_info[*merge_num].vel_acc = slv_rng_vel & 0xF;          // Extract slave vel_acc
                                cfr_info[*merge_num].vel_idx = slv_vel_idx;
                                cfr_info[*merge_num].amb_idx = (slv_amb_noi >> 20) & 0x1F; // Extract slave amb_idx
                                cfr_info[*merge_num].noi     = slv_amb_noi & 0xFFFFF;      // Extract slave noi

                                (*merge_num)++;

                                // Store slave obj FFT peaks in MEM_MAC
                                baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_MAC);
                                for (uint8_t chl_index = 0; chl_index < ant_chl_num; chl_index++) {
                                        uint32_t fft2d_data = cascade_read_buf(rx_buf_offset++);
                                        //EMBARC_PRINTF("fft2d_data = %u\n", fft2d_data);
                                        baseband_write_mem_table(bb_hw, *mem_mac_offset, fft2d_data);
                                        (*mem_mac_offset)++;
                                }
                        }
                }/* end for */
                cascade_read_buf_done(); /* release rx buffer */
        } else {
                result = E_SYS;
                EMBARC_PRINTF("!!! merge timeout ~~~~~~~~~~~~~~~\n");
        }/* endif E_OK */

        return result;

}

static void baseband_write_cfar(baseband_hw_t *bb_hw, uint32_t *merge_num, cfr_info_t *cfr_info, volatile obj_info_t *obj_info)
{
        /* write merge results to mem */
        baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
        for (uint16_t i = 0; i < *merge_num; i++) {

                obj_info[i].rng_acc = cfr_info[i].rng_acc;
                obj_info[i].rng_idx = cfr_info[i].rng_idx;
                obj_info[i].vel_acc = cfr_info[i].vel_acc;
                obj_info[i].vel_idx = cfr_info[i].vel_idx;

                // amb and noi should reordered due to bug 764
                uint32_t * p_tmp = (uint32_t *)(&(obj_info[i]));
                *(p_tmp + NOI_AMB_OFFSET) = (cfr_info[i].noi << NOI_AMB_REODER) | cfr_info[i].amb_idx;

        }

        BB_WRITE_REG(bb_hw, DOA_NUMB_OBJ, *merge_num);
}

static void baseband_write_cascade_info(baseband_hw_t *bb_hw, uint16_t slv_obj_num, uint16_t tx_obj_num)
{
        uint32_t slv_info = SHIFT_L16(slv_obj_num) | tx_obj_num;
        cascade_write_buf(slv_info);     /* write cfar info of slave chip */
}

static void baseband_write_cascade_data(baseband_hw_t *bb_hw, uint16_t obj_index, uint8_t nvarray)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(bb_hw, baseband_t, bb_hw)->cfg;

        uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);

        /* get slave object info */
        uint32_t mem_rlt_offset = 0;

        /* get cfar result address */
        if ((BB_READ_REG(bb_hw, CFR_SIZE_OBJ)) < SYS_SIZE_OBJ_BNK) { /* mem_rlt will be splited to 4 banks when cfar size less than 256 */
                uint8_t bnk_idx = baseband_get_cur_frame_type();
                mem_rlt_offset = bnk_idx * (1 << SYS_SIZE_OBJ_WD_BNK) * RESULT_SIZE;
        }
        volatile obj_info_t *obj_info = (volatile obj_info_t *)(BB_MEM_BASEADDR + mem_rlt_offset);

        /* range and doppler index */
        uint16_t hw_rng_idx = obj_info[obj_index].rng_idx;
        uint16_t hw_vel_idx = obj_info[obj_index].vel_idx;
/*
        uint32_t hw_rng_vel = SHIFT_L16(hw_rng_idx) | hw_vel_idx;
        cascade_write_buf(hw_rng_vel);
*/
        uint32_t * obj_info_word = (uint32_t *)(&(obj_info[obj_index]));
        uint32_t slv_rng_vel = *obj_info_word;
        uint32_t slv_amb_noi = *(obj_info_word + 1);
        //EMBARC_PRINTF("slv_rng_vel = %u\n", slv_rng_vel);
        //EMBARC_PRINTF("slv_amb_noi = %u\n", slv_amb_noi);
        cascade_write_buf(slv_rng_vel);       // Transfer rng_idx, rng_acc, vel_idx, vel_acc
        cascade_write_buf(slv_amb_noi);   // Transfer amb_idx(not ready after cfar), Noi(useful in DoA)

        /* fft2d */
        baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
        uint8_t ch_index;
        uint8_t bpm_index = 0;

        uint8_t bpm_offset = cfg->anti_velamb_en ? 1 : 0;

        for (bpm_index = bpm_offset; bpm_index < (nvarray + bpm_offset); bpm_index++) {
                for (ch_index = 0; ch_index < ANT_NUM; ch_index++) {
                        uint32_t fft_mem = baseband_hw_get_fft_mem(bb_hw, ch_index, hw_rng_idx, hw_vel_idx, bpm_index);
                        cascade_write_buf(fft_mem);
                        //EMBARC_PRINTF("fft_mem = %u\n", fft_mem);
                }
        }
        baseband_switch_mem_access(bb_hw, old);
}

void baseband_write_cascade_ctrl(void)
{
        baseband_hw_t *bb_hw = &baseband_get_cur_bb()->bb_hw;
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(bb_hw, baseband_t, bb_hw)->cfg;

        uint8_t slv_nvarray = cfg->nvarray; /* fft2d data number of each object */
        uint16_t obj_num = BB_READ_REG(bb_hw, CFR_NUMB_OBJ);
        if ( obj_num > TRACK_NUM_CDI )
                obj_num = TRACK_NUM_CDI;

        uint16_t slv_obj_num = obj_num; // total object number of slave
        uint16_t tx_obj_num = 0;  // transmitted object number of slave
        uint16_t obj_idx_offset = 0; // object index offset of cfar result

        do {
                // tx object number control
                obj_idx_offset = obj_idx_offset + tx_obj_num;

                if (obj_num > MAX_TX_OBJ_NUM)
                        tx_obj_num = MAX_TX_OBJ_NUM;
                else
                        tx_obj_num = obj_num;

                /* About slave chip data transfer scheme:
                 *
                 * 'tx_obj_num = MAX_TX_OBJ_NUM;' is the maximum number of slave object info blocks allowed to transfer
                 * in one SPI TX process, which depends on SPI buffer size. So if 'slv_obj_num', the total slave object number,
                 * is greater than MAX_TX_OBJ_NUM, slave info blocks will be split and transfered in several SPI TX processes.
                 * Variable 'obj_num' is to record the number of remaining slave info blocks to transfer.
                 *
                 * An internal protocol guarantees that if transfered data block is not read by master chip, slave chip
                 * would not start the next SPI TX process.
                 * Since CFAR output object number is not too large in practical test, this case is not triggered thus not verified.
                 * */
                obj_num = obj_num - tx_obj_num; // record the number of remaining slave obj info blocks to transfer

                /*------ Start one SPI TX process by slave chip ------*/
                // tx asserted
                cascade_write_buf_req();
                // In each SPI TX process, the first word consists the total number of slave objs and
                // the number of slave objs to transfer in this SPI TX process :
                baseband_write_cascade_info(bb_hw, slv_obj_num, tx_obj_num);

                for (uint16_t i = 0; i < tx_obj_num; i++) {
                        uint16_t obj_idx = i + obj_idx_offset; // object index of cfar result
                        baseband_write_cascade_data(bb_hw, obj_idx, slv_nvarray);  /* write k, p, noise */
                }

                // tx done
                cascade_write_buf_done();
                /*------ End of one SPI TX process by slave chip------*/

        } while (obj_num != 0); // tx loop

}

void baseband_merge_cascade(baseband_t *bb)
{
        baseband_hw_t *bb_hw = &bb->bb_hw;
        sensor_config_t* cfg = (sensor_config_t*)(bb->cfg);

        /* get antenna channel number */
        uint8_t ant_chl_num = (cfg->nvarray) * ANT_NUM; /* fft2d data number of each object */

        /* get cfar result address */
        uint32_t mem_rlt_offset = 0;
        if ((BB_READ_REG(bb_hw, CFR_SIZE_OBJ)) < SYS_SIZE_OBJ_BNK) { /* mem_rlt will be splited to 4 banks when cfar size less than 256 */
                uint8_t bnk_idx = baseband_get_cur_frame_type();
                mem_rlt_offset = bnk_idx * (1 << SYS_SIZE_OBJ_WD_BNK) * RESULT_SIZE;
        }
        volatile obj_info_t *obj_info = (volatile obj_info_t *)(BB_MEM_BASEADDR + mem_rlt_offset);

        /* get cfar result number */
        uint16_t mst_obj_num = BB_READ_REG(bb_hw, CFR_NUMB_OBJ);

        if ( mst_obj_num > TRACK_NUM_CDI )
                mst_obj_num = TRACK_NUM_CDI;

        // merge
        cfr_info_t cfr_info[TRACK_NUM_CDI]; // store the cfar merge result
        uint16_t slv_obj_num = 0;
        uint16_t rx_obj_num = 0;  // received object number of slave
        uint16_t total_rx_obj_num = 0; // totally received object number of slave
        uint32_t merge_num = 0;
        uint32_t mem_mac_offset = 0; // store fft2d data

        do {
                if (E_OK == baseband_merge_cfar(bb_hw, ant_chl_num, mst_obj_num,
                                                &slv_obj_num, &rx_obj_num, &mem_mac_offset,
                                                &merge_num, cfr_info, obj_info))
                        total_rx_obj_num += rx_obj_num;
                else
                        break;
        } while (total_rx_obj_num != slv_obj_num);

        /* write merge results to mem */
        baseband_write_cfar(bb_hw, &merge_num, cfr_info, obj_info);
}

/* <scan stop> needs special handling */
/* <scan stop> does not use neither baseband_write_cascade_cmd nor baseband_read_cascade_cmd */
/* <scan stop> should be faster tx/rx to ensure both master and slave will stop at the same frame */
void baseband_scan_stop_tx(uint16_t cascade_cmd, uint16_t param)
{
        cascade_write_buf_req();
        cascade_write_buf(SHIFT_L16(param) | cascade_cmd);  /* write spi, when "scan start/stop" */
        cascade_write_buf_done();
}

void baseband_scan_stop_rx(TickType_t xTicksToWait)
{
        if (E_OK == cascade_read_buf_req(xTicksToWait)) {   /* wait buf */
                uint16_t rx_cmd   = REG_L16(cascade_read_buf(0));        /* read buf to get "scan stop" */

                if (rx_cmd == CMD_SCAN_STOP)
                        baseband_scan_stop(); /* scan stop for slave */

                cascade_read_buf_done(); /* release spi underlying buffer */
        }
}

void baseband_write_cascade_cmd(const char *pcCommandString)
{
        cascade_write_buf_req();

        cascade_write_buf(CMD_HDR);              /* write cmd header */
        cascade_write_buf_str(pcCommandString);  /* write cmd string */

        cascade_write_buf_done();
}

void baseband_read_cascade_cmd(TickType_t xTicksToWait)
{
        if (E_OK == cascade_read_buf_req(xTicksToWait)) {   /* wait buf */

                if (CMD_HDR == cascade_read_buf(0)) {
                        cascade_read_buf_str();

                        uint32_t msg = 1; // push 1 to queue
                        xQueueSendFromISR(queue_spi_cmd, (void *)&msg, 0);
                }

                cascade_read_buf_done(); /* release spi underlying buffer */
        }
}

uint32_t cascade_spi_cmd_wait(void)
{
        uint32_t event = 0;
        xQueueReceive(queue_spi_cmd, &event, 0); // take 1 from queue

        return event;
}

#endif // CHIP_CASCADE
