#ifndef ALPS_PAD_MUX_H
#define ALPS_PAD_MUX_H

void io_mux_init();
void io_mux_dbgbus_dump();
void io_mux_lvds_dump();
void io_mux_dbgbus_mode_stop();
void io_mux_free_run();
void io_mux_free_run_disable();

int8_t io_get_dmumode(void);
void io_sel_dmumode(uint8_t m);

#ifdef CHIP_CASCADE
void io_mux_adc_sync(void);
void io_mux_adc_sync_disable(void);
void io_mux_casade_irq(void);
void io_mux_casade_irq_disable(void);
#endif

#endif
