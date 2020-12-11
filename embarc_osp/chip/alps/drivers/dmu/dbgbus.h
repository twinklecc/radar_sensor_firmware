#ifndef _DBGBUS_H_
#define _DBGBUS_H_

void dbgbus_input_config(void);
void dbgbus_dump_enable(uint8_t dbg_src);
void dbgbus_hil_ready(void);
void dbgbus_dump_reset(void);
void dbgbus_dump_done(void);
void dbgbus_dump_disable(void);
void dbgbus_free_run_enable(void);
void dbgbus_dump_start(uint8_t dbg_src);
void dbgbus_dump_stop(void);
void gpio_free_run_sync(void);
void lvds_dump_reset(void);
void lvds_dump_done(void);

#endif
