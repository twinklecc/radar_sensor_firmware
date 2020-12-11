#include "baseband_dpc.h"
#include "sensor_config.h"
#include "baseband_hw.h"
#include "baseband_cas.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "vel_deamb_MF.h"
#include "baseband_cli.h"
#include "cascade.h"

extern QueueHandle_t queue_bb_isr;

static baseband_data_proc_t bb_dpc[DPC_SIZE];
static bool dpc_hil_en = false; /* flag used to re-init data process chain after hil */

void baseband_data_proc_default();
void baseband_start_dpc(baseband_data_proc_t * dp, baseband_t *bb);
void baseband_data_proc_cascade();

void bb_dpc_sysen_set(uint8_t bb_dpc_ind, uint16_t bb_sysen)
{
        if (bb_dpc_ind < DPC_SIZE)
        {
                bb_dpc[bb_dpc_ind].sys_enable = bb_sysen;
        } else {
                EMBARC_PRINTF("Max dpc size is %d\n", DPC_SIZE);
        }
}

/* request the status of data process chain */
bool baseband_data_proc_req()
{
        bool tmp = dpc_hil_en;

        if (dpc_hil_en)
                dpc_hil_en = false;

        return tmp;
}

void baseband_data_proc_init()
{
#ifndef CHIP_CASCADE

#if VEL_DEAMB_MF_EN
        bb_dpc_config(bb_dpc);
        baesband_frame_interleave_strategy_set(VELAMB, 3, CFG_0, CFG_1);
        baesband_frame_interleave_cnt_clr();
#else
        baseband_data_proc_default();
#endif // VEL_DEAMB_MF_EN

#else
        baseband_data_proc_cascade();
#endif // CHIP_CASCADE

}

bool baseband_data_proc_run(baseband_data_proc_t * dp)
{
        uint32_t event = 0;
        baseband_t* bb;
        if (dp->fi_recfg)
                bb = baseband_frame_interleave_recfg();
        else
                bb = baseband_get_rtl_frame_type(); // align with hardware frame type
        if (dp->pre)
                dp->pre(bb);

        baseband_start_dpc(dp, bb); // hw start

        if (dp->post)
                dp->post(bb);
        if (!dp->end) {
                if(xQueueReceive(queue_bb_isr, &event, portMAX_DELAY) == pdTRUE) {
                        if (dp->post_irq)
                                dp->post_irq(bb);
                }
        }
        return dp->end;
}

baseband_data_proc_t* baseband_get_dpc()
{
        return bb_dpc;
}

void baseband_data_proc_default()
{
        bb_dpc[0].pre = NULL;
        bb_dpc[0].post = NULL;
        bb_dpc[0].post_irq = NULL;
        bb_dpc[0].fi_recfg = true;
        bb_dpc[0].stream_on = true;
        bb_dpc[0].radio_en = true;
        bb_dpc[0].tx_en = true;
        sensor_config_t* cfg = sensor_config_get_config(0);
        bb_dpc[0].sys_enable =
                 SYS_ENA(AGC   , cfg->agc_mode == 0 ? 0: 1)
                |SYS_ENA(SAM   , true         )
                |SYS_ENA(FFT_2D, true         )
                |SYS_ENA(CFR   , true         )
                |SYS_ENA(BFM   , true         );
        bb_dpc[0].cas_sync_en = true;
        bb_dpc[0].sys_irq_en = BB_IRQ_ENABLE_ALL;
        bb_dpc[0].track_en = true;
        bb_dpc[0].end = true;
}

void baseband_start_dpc(baseband_data_proc_t * dp, baseband_t *bb)
{
        uint16_t sys_enable_new = 0;

        if (dp->stream_on)
                sys_enable_new =  (dp->sys_enable)
                                 |SYS_ENA(DMP_MID, baseband_stream_on_dmp_mid())
                                 |SYS_ENA(FFT_1D , baseband_stream_on_fft1d()  )
                                 |SYS_ENA(DMP_FNL, baseband_stream_on_dmp_fnl());
        else
                sys_enable_new =  dp->sys_enable;

        baseband_start_with_params(bb, dp->radio_en, dp->tx_en, sys_enable_new,
                                   dp->cas_sync_en, dp->sys_irq_en, dp->track_en);

}

/* HIL */
void baseband_data_proc_hil(uint16_t bb_ena_0, uint16_t bb_ena_1, uint16_t bb_ena_2)
{
        dpc_hil_en = true; /* flag used to re-init data process chain after hil */

        bool dpc_end_0 = false;
        bool dpc_end_1 = false;
        bool dpc_end_2 = false;
        if (bb_ena_1 == 0 && bb_ena_2 == 0)
                dpc_end_0 = true;

        if (bb_ena_1 != 0 && bb_ena_2 == 0)
                dpc_end_1 = true;

        if (bb_ena_2 != 0)
                dpc_end_2 = true;

        /* 1st run */
        bb_dpc[0].pre         = (bb_ena_1 !=0) ? baseband_hil_dump_done : NULL; /* done for FPGA to switch the direction of dbgbus */
        bb_dpc[0].post        = baseband_hil_input_enable;
        bb_dpc[0].post_irq    = baseband_hil_input_disable; /* only run when bb_ena_1 != 0*/
        bb_dpc[0].fi_recfg    = true;
        bb_dpc[0].stream_on   = false;
        bb_dpc[0].radio_en    = false;
        bb_dpc[0].tx_en       = false;
        bb_dpc[0].sys_enable  = bb_ena_0;
        bb_dpc[0].cas_sync_en = false;
        bb_dpc[0].sys_irq_en  = BB_IRQ_ENABLE_BB_DONE;
        bb_dpc[0].track_en    = dpc_end_0; /* enable track at the end of hil */
        bb_dpc[0].end         = dpc_end_0;

        /* 2nd run */
        bb_dpc[1].pre         = baseband_hil_dump_enable;
        bb_dpc[1].post        = NULL;
        bb_dpc[1].post_irq    = NULL;
        bb_dpc[1].fi_recfg    = false;
        bb_dpc[1].stream_on   = false;
        bb_dpc[1].radio_en    = false;
        bb_dpc[1].tx_en       = false;
        bb_dpc[1].sys_enable  = bb_ena_1;
        bb_dpc[1].cas_sync_en = false;
        bb_dpc[1].sys_irq_en  = BB_IRQ_ENABLE_BB_DONE;
        bb_dpc[1].track_en    = dpc_end_1; /* enable track at the end of hil */
        bb_dpc[1].end         = dpc_end_1;

        /* 3rd run */
        bb_dpc[2].pre         = NULL;
        bb_dpc[2].post        = NULL;
        bb_dpc[2].post_irq    = NULL;
        bb_dpc[2].fi_recfg    = false;
        bb_dpc[2].stream_on   = false;
        bb_dpc[2].radio_en    = false;
        bb_dpc[2].tx_en       = false;
        bb_dpc[2].sys_enable  = bb_ena_2;
        bb_dpc[2].cas_sync_en = false;
        bb_dpc[2].sys_irq_en  = BB_IRQ_ENABLE_BB_DONE;
        bb_dpc[2].track_en    = dpc_end_2; /* enable track at the end of hil */
        bb_dpc[2].end         = dpc_end_2;
}

#ifdef CHIP_CASCADE
void baseband_data_proc_cascade()
{
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                // master process 0, SAM-FFT-CFAR
                bb_dpc[0].post_irq    = baseband_merge_cascade;
                bb_dpc[0].fi_recfg    = true; // flag for frame type reconfig
                bb_dpc[0].stream_on   = true;
                bb_dpc[0].radio_en    = true;
                bb_dpc[0].tx_en       = true;
                sensor_config_t* cfg  = sensor_config_get_config(0);
                bb_dpc[0].sys_enable  =  SYS_ENA(AGC   , cfg->agc_mode == 0 ? 0: 1)
                                        |SYS_ENA(SAM   , true         )
                                        |SYS_ENA(FFT_2D, true         )
                                        |SYS_ENA(CFR   , true         );
                bb_dpc[0].cas_sync_en = true;
                bb_dpc[0].sys_irq_en  = BB_IRQ_ENABLE_ALL;
                bb_dpc[0].track_en    = true;
                bb_dpc[0].end         = false;

                // master process 1, DOA only
                bb_dpc[1].fi_recfg    = false; // flag for frame type reconfig
                bb_dpc[1].stream_on   = false;
                bb_dpc[1].radio_en    = false;
                bb_dpc[1].tx_en       = false;
                bb_dpc[1].sys_enable  = SYS_ENA(BFM   , true);
                bb_dpc[1].cas_sync_en = false;
                bb_dpc[1].sys_irq_en  = BB_IRQ_ENABLE_ALL;
                bb_dpc[1].track_en    = false;
                bb_dpc[1].end         = true;

        } else {
                // slave process 0, SAM-FFT-CFAR
                bb_dpc[0].fi_recfg    = true;  // flag for frame type reconfig
                bb_dpc[0].stream_on   = true;
                bb_dpc[0].radio_en    = false; // no need fmcw in slave
                bb_dpc[0].tx_en       = false;
                sensor_config_t* cfg  = sensor_config_get_config(0);
                bb_dpc[0].sys_enable  =  SYS_ENA(AGC   , cfg->agc_mode == 0 ? 0: 1)
                                        |SYS_ENA(SAM   , true         )
                                        |SYS_ENA(FFT_2D, true         )
                                        |SYS_ENA(CFR   , true         );
                bb_dpc[0].cas_sync_en = true;
                bb_dpc[0].sys_irq_en  = BB_IRQ_ENABLE_ALL;
                bb_dpc[0].track_en    = true;
                bb_dpc[0].end         = true;
        }
}
#endif
