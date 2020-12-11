#ifndef CALTERAH_MATH_FUNCS_H
#define CALTERAH_MATH_FUNCS_H

#include "embARC_toolchain.h"

void normalize(float *in, int len, int method);

float get_power(float *in, int len);

uint32_t compute_gcd(uint32_t a, uint32_t b);

#endif
