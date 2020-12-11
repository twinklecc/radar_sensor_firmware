#ifndef FFT_WINDOW_H
#define FFT_WINDOW_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

#include "calterah_limits.h"

#define WIN_COEFF_BITW 14
#define WIN_COEFF_INT  0


float gen_window(const char *wintype, int len, double param1, double param2, double param3);

uint32_t get_win_coeff(uint32_t idx);

float get_win_coeff_float(uint32_t idx);

float* get_win_buff();

#endif
