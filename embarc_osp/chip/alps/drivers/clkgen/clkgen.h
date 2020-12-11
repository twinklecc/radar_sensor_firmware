#ifndef ALPS_CLK_GEN_H
#define ALPS_CLK_GEN_H

#include "arc_builtin.h"
#include "alps_clock.h"

int32_t clock_init();
int32_t clock_select_source(clock_source_t clk_src, clock_source_t sel);
int32_t clock_divider(clock_source_t clk_src, uint32_t div);
int32_t clock_enable(clock_source_t clk_src, uint32_t enable);
int32_t clock_frequency(clock_source_t clk_src);
void system_clock_init(void);

void bus_clk_div(uint32_t div);
void apb_clk_div(uint32_t div);
void apb_ref_clk_div(uint32_t div);
void switch_gpio_out_clk(bool ctrl);

#endif
