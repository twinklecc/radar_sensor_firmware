#include "calterah_data_conversion.h"
#include "math.h"
#include "stdio.h"

#ifdef UNIT_TEST
#define EMBARC_ASSERT(t)
#else
#include "embARC_assert.h"
#endif

float fx_to_float(uint32_t val, uint32_t W, int32_t I, bool sign)
{
        EMBARC_ASSERT(W<32);
        int32_t pow_i = I-W;
        float tpow = pow(2.0, 1.0 * pow_i);
        uint32_t d_max = 1<<(sign ? W-1 : W);
        float retval = 0;
        if (sign) {
                int32_t d = val < d_max ? val : val - 2 * d_max;
                retval = d * tpow;
        } else {
                retval = val * tpow;
        }
        return retval;
}

float fl_to_float(uint32_t val, uint32_t W_m, int32_t I, bool sign_m, uint32_t W_e, bool sign_e)
{
        uint32_t d_e = (~((-1L) << W_e)) & val;
        uint32_t d_e_max = 1 << (sign_e ? W_e-1 : W_e);
        if (sign_e)
                d_e = d_e < d_e_max ? d_e : d_e - 2 * d_e_max;
        uint32_t d_m = val >> W_e;
        int32_t pow = sign_e ?  I + d_e : I - d_e;
        return fx_to_float(d_m, W_m, pow, sign_m);
}

complex_t cfx_to_complex(uint32_t val, uint32_t W, int32_t I, bool sign)
{
        EMBARC_ASSERT(W<16);
        uint32_t dy = (~((-1L) << W)) & val;
        uint32_t dx = val >> W;
        complex_t ret;
        ret.r = fx_to_float(dx, W, I, sign);
        ret.i = fx_to_float(dy, W, I, sign);
        return ret;
}

complex_t cfl_to_complex(uint32_t val, uint32_t W_m, int32_t I, bool sign_m, uint32_t W_e, bool sign_e)
{
        uint32_t d_e = (~((-1L) << W_e)) & val;
        uint32_t d_e_max = 1 << (sign_e ? W_e-1 : W_e);
        if (sign_e)
                d_e = d_e < d_e_max ? d_e : d_e - 2 * d_e_max;
        uint32_t d_m = val >> W_e;
        int32_t pow = sign_e ? I + d_e : I - d_e;
        return cfx_to_complex(d_m, W_m, pow, sign_m);
}

uint32_t float_to_fx_trunc(double val, uint32_t W, int32_t I, bool sign, bool truc)
{
        EMBARC_ASSERT(W < 32);
        uint32_t retval = 0;
        int32_t pow = W-I;
        uint32_t mask = -1L << (sign ? W-1 : W);
        int32_t d_min = sign ? ((int32_t) mask) : 0;
        uint32_t d_max = ~mask;
        double tmp = pow > 0 ? ((double)val) * (1 << pow) : ((double)val) / (1 << (-pow));

        if (truc)
                tmp = floor(tmp);
        else
                tmp = round(tmp);

        if (d_min > tmp)
                retval = d_min;
        else if (d_max < tmp)
                retval = d_max;
        else
                /* It is very trick: we need to convert it to int32_t instead of uint32_t!*/
                retval = (int32_t) tmp;
        return retval & (~(-1L << W));
}

uint32_t float_to_fl_com(float val, uint32_t W_m, int32_t I_m, bool sign_m, uint32_t W_e, bool sign_e, bool epos)
{
        uint32_t retval  = 0;
        uint32_t retzero = 0;

        int exp_min = sign_e ? -(1 << (W_e - 1))    : epos ? 0 : -(1 << W_e) + 1;
        int exp_max = sign_e ? (1 << (W_e - 1)) - 1 : epos ? (1 << W_e) - 1 : 0;

        int exp = exp_min;
        if (sign_e == false)
                exp = -exp;   /* it is correct no matter epos is true or false */

        retzero |= exp & ((1 << W_e) - 1);

        if ((val == 0) || (val < 0 && sign_m == false))
                return retzero;

        exp = 0;
        double tmp_val = (double)val;

        if (tmp_val > 0) {
                int integer_len = sign_m ? I_m - 1 : I_m;
                int tmp = -integer_len + 1;
                double min = integer_len > 0 ? (double)(1 << (integer_len - 1)) : (double)1.0 / (1 << tmp);
                double max = min * 2;

                while ((tmp_val < min) && (exp > exp_min)) {
                        tmp_val = tmp_val * 2;
                        exp--;
                }

                while ((tmp_val >= max) && (exp < exp_max)) {
                        tmp_val = tmp_val / 2;
                        exp++;
                }
        } else {
                int pow = I_m - 1;
                double min;

                if (pow < 0) {
                        int rev = -pow;
                        min = (double)(-1.0) / (1 << rev);
                } else {
                        min = (double)(-1.0) * (1 << pow);
                }

                double max = min / 2;

                while ((tmp_val < min) && (exp < exp_max)) {
                        tmp_val = tmp_val / 2;
                        exp++;
                }

                while ((tmp_val >= max) && (exp > exp_min)) {
                        tmp_val = tmp_val * 2;
                        exp--;
                }
        }

        uint32_t mantissa = float_to_fx_trunc(tmp_val, W_m, I_m, sign_m, true);

        if (sign_e == false && epos == false)
                exp = -exp;

        retval |= exp & ((1 << W_e) - 1);
        retval |= mantissa << W_e;

        return retval;
}

uint64_t complex_to_cfl_dwords_com(complex_t *val,
                                   uint32_t W_m, int32_t I_m, bool sign_m,
                                   uint32_t W_e, bool sign_e, bool epos)
{
        double real = val->r;
        double imag = val->i;

        uint64_t retval  = 0;
        uint64_t retzero = 0;

        int exp_min = sign_e ? -(1 << (W_e - 1))    : epos ? 0 : -(1 << W_e) + 1;
        int exp_max = sign_e ? (1 << (W_e - 1)) - 1 : epos ? (1 << W_e) - 1 : 0;

        int exp = exp_min;
        if (sign_e == false)
                exp = -exp;   /* it is correct no matter epos is true or false */

        retzero |= exp & ((1 << W_e) - 1);

        real = (real < 0 && sign_m == false) ? 0 : real;
        imag = (imag < 0 && sign_m == false) ? 0 : imag;

        if (real == 0 && imag == 0) {
                return retzero;
        }

        double max_abs = fabs(real) >= fabs(imag) ? (fabs(real) == fabs(imag) ? (real > imag ? real : imag) : real) : imag;

        exp = 0;

        if (max_abs > 0) {
                int integer_len = sign_m ? I_m - 1 : I_m;
                int tmp = -integer_len + 1;
                double min = integer_len > 0 ? (double)(1 << (integer_len - 1)) : (double)1.0 / (1 << tmp);
                double max = min * 2;

                while ((max_abs < min) && (exp > exp_min)) {
                        max_abs = max_abs * 2;
                        real = real * 2;
                        imag = imag * 2;
                        exp--;
                }

                while ((max_abs >= max) && (exp < exp_max)) {
                        max_abs = max_abs / 2;
                        real = real / 2;
                        imag = imag / 2;
                        exp++;
                }
        } else {
                int pow = I_m - 1;
                double min;

                if (pow < 0) {
                        int rev = -pow;
                        min = (double)(-1.0) / (1 << rev);
                } else {
                        min = (double)(-1.0) * (1 << pow);
                }

                double max = min / 2;

                while ((max_abs < min) && (exp < exp_max)) {
                        max_abs = max_abs / 2;
                        real = real / 2;
                        imag = imag / 2;
                        exp++;
                }

                while ((max_abs >= max) && (exp > exp_min)) {
                        max_abs = max_abs * 2;
                        real = real * 2;
                        imag = imag * 2;
                        exp--;
                }
        }

        uint32_t mantissa_r = float_to_fx_trunc(real, W_m, I_m, sign_m, true);
        uint32_t mantissa_i = float_to_fx_trunc(imag, W_m, I_m, sign_m, true);

        if (sign_e == false && epos == false)
                exp = -exp;

        retval |= exp & ((1 << W_e) - 1);
        retval |= (uint64_t)mantissa_i << W_e;
        retval |= (uint64_t)mantissa_r << (W_e + W_m);

        return retval;
}

uint32_t complex_to_cfx(complex_t *val, uint32_t W, int32_t I, bool sign)
{
        EMBARC_ASSERT(W<16);
        uint32_t r, i;
        r = float_to_fx(val->r, W, I, sign);
        i = float_to_fx(val->i, W, I, sign);
        uint32_t ret = (r << W) | i;
        return ret;
}

uint32_t complex_to_cfl(complex_t *val, uint32_t W_m, int32_t I_m, bool sign_m, uint32_t W_e, bool sign_e)
{
        EMBARC_ASSERT(2 * W_m + W_e <= 32);
        return (uint32_t)complex_to_cfl_dwords_com(val, W_m, I_m, sign_m, W_e, sign_e, false);
}

uint64_t complex_to_cfl_dwords(complex_t *val, uint32_t W_m, int32_t I_m, bool sign_m, uint32_t W_e, bool sign_e)
{
        /* when W_m*2 + W_e > 32, this function should be called instead of complex_to_cfl */
        EMBARC_ASSERT(2 * W_m + W_e <= 64);
        return complex_to_cfl_dwords_com(val, W_m, I_m, sign_m, W_e, sign_e, false);
}

uint32_t float_to_fx(float val, uint32_t W, int32_t I, bool sign)
{
        return float_to_fx_trunc(val, W, I, sign, false);
}

uint32_t float_to_fl(float val, uint32_t W_m, int32_t I_m, bool sign_m, uint32_t W_e, bool sign_e) {
        return float_to_fl_com(val, W_m, I_m, sign_m, W_e, sign_e, false);
}

