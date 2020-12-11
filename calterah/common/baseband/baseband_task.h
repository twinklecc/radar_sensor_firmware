#ifndef BASEBAND_TASK_H
#define BASEBAND_TASK_H

#define BB_ISR_QUEUE_LENGTH 8
#define BB_1P4_QUEUE_LENGTH 1

void baseband_task(void *parameters);
void bb_clk_switch();
void bb_clk_restore();
void initial_flag_set(bool data);

#endif
