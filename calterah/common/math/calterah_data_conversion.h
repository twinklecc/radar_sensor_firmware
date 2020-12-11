#ifndef CALTERAH_DATA_CONVERSION_H
#define CALTERAH_DATA_CONVERSION_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

#include "calterah_complex.h"

float fx_to_float(uint32_t val, uint32_t W, int32_t I, bool sign);
float fl_to_float(uint32_t val, uint32_t W_m, int32_t I, bool sign_m, uint32_t W_e, bool sign_e);
complex_t cfx_to_complex(uint32_t val, uint32_t W, int32_t I, bool sign);
complex_t cfl_to_complex(uint32_t val, uint32_t W_m, int32_t I, bool sign_m, uint32_t W_e, bool sign_e);
uint32_t float_to_fx(float val, uint32_t W, int32_t I, bool sign);
uint32_t float_to_fl(float val, uint32_t W_m, int32_t I, bool sign_m, uint32_t W_e, bool sign_e);
uint32_t complex_to_cfx(complex_t *val, uint32_t W, int32_t I, bool sign);
uint32_t complex_to_cfl(complex_t *val, uint32_t W_m, int32_t I, bool sign_m, uint32_t W_e, bool sign_e);
uint64_t complex_to_cfl_dwords(complex_t *val, uint32_t W_m, int32_t I_m, bool sign_m, uint32_t W_e, bool sign_e);

#endif
