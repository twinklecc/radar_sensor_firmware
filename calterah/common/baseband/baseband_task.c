#include "embARC_toolchain.h"
#include "clkgen.h"
#include "sensor_config.h"
#include "baseband_task.h"
#include "baseband.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "arc_exception.h"
#include "alps_hardware.h"
#include "embARC_debug.h"
#include "baseband_dpc.h"
#include "baseband_cas.h"
#include "cascade.h"

#define BB_WRITE_REG(bb_hw, RN, val) baseband_write_reg(bb_hw, BB_REG_##RN, val)

static uint8_t old_bb_clk;
extern QueueHandle_t queue_bb_isr;
extern QueueHandle_t queue_fix_1p4;
extern SemaphoreHandle_t mutex_frame_count;
int32_t frame_count = 0;
static bool bb_clk_restore_en = false;
/* task variables */
extern SemaphoreHandle_t mutex_initial_flag;
bool initial_flag = true;

void baseband_int_handler(void* ptr)
{
        uint32_t msg = 0;
        BB_WRITE_REG(NULL, SYS_IRQ_CLR, BB_IRQ_CLEAR_ALL);
        xQueueSendFromISR(queue_bb_isr, (void *)&msg, 0);
}

void bb_clk_switch()
{
        bb_clk_restore_en = true;
        old_bb_clk = raw_readl(REG_CLKGEN_DIV_AHB);
        bus_clk_div(BUS_CLK_100M); /* when dumping debug data(no buffer), bb clock should be switched to dbgbus clock(100MHz) */
}

void bb_clk_restore() /* After dumping sample debug data(no buffer), bb clock should be restored to default 200MHz */
{
        if (bb_clk_restore_en == true) {
                bb_clk_restore_en = false;
                raw_writel(REG_CLKGEN_DIV_AHB, old_bb_clk);
        }
}

void frame_count_ctrl(void)
{
        xSemaphoreTake(mutex_frame_count, portMAX_DELAY);
        if (frame_count > 0)
                frame_count--;
        xSemaphoreGive(mutex_frame_count);
}

void initial_flag_set(bool data)
{
        xSemaphoreTake(mutex_initial_flag, portMAX_DELAY);
        initial_flag = data;
        xSemaphoreGive(mutex_initial_flag);
}

void baseband_task(void *params)
{
        uint32_t event = 0;
        int_handler_install(INT_BB_DONE, baseband_int_handler);
        int_enable(INT_BB_DONE);
        dmu_irq_enable(INT_BB_DONE, 1); /* irq mask enable */

        baseband_t* bb = baseband_get_cur_bb();
        baseband_data_proc_t* dpc = baseband_get_dpc();

#ifdef CHIP_CASCADE
        baseband_cascade_handshake();
        baseband_dc_calib_init(NULL, false, false); // dc_calib after handshake
#endif

        while(1) {
                if (frame_count != 0) {
                        if(track_is_ready(bb->track)) {
                                track_lock(bb->track);
                                /* run baseband data proc chain till the end */
                                int index = 0;
                                while (!baseband_data_proc_run(&dpc[index++]))
                                        ;

                                bb = baseband_get_rtl_frame_type();

                                if( initial_flag == true ) {
                                        track_pre_start(bb->track);
                                        initial_flag_set(false);
                                } else {
                                        track_run(bb->track);
                                }
                                /* wait queue for last BB HW run*/
                                if(xQueueReceive(queue_bb_isr, &event, portMAX_DELAY) == pdTRUE) {
#ifdef CHIP_CASCADE
                                        if (chip_cascade_status() == CHIP_CASCADE_SLAVE) {
                                                /* rx "scan stop" to release spi underlying buffer, must before baseband_write_cascade_ctrl*/
                                                baseband_scan_stop_rx(CMD_RX_WAIT);

                                                baseband_write_cascade_ctrl();
                                        }
#endif
                                        frame_count_ctrl();
                                        /* read result from baseband */
                                        track_read(bb->track);
                                        if (frame_count == 0) {
                                                track_output_print(bb->track);/* print the last frame */
                                                baseband_stop(bb);
                                                EMBARC_PRINTF("<EOF>\r\n");
                                        }

                                        baseband_workaroud(&bb->bb_hw);
                                }
                        }
                } else  {
                        if (false == initial_flag)
                                baseband_stop(bb); /* re-call baseband_stop if not asserted in xQueueReceive */
#ifdef CHIP_CASCADE
                        if(chip_cascade_status() == CHIP_CASCADE_SLAVE)
                                baseband_read_cascade_cmd(0); /* waiting for master command "scan start/stop" */
#endif

                        xQueueReceive(queue_fix_1p4, &event, 1); // add one tick to disturb the idle task loop
                        taskYIELD();
                } /* end if */
        } /* end while */
}
