#include "math.h"
#include "calterah_complex.h"
#include "calterah_math_funcs.h"
#include "calterah_data_conversion.h"
#include "fft_window.h"
#include "string.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

static float aux1[MAX_FFT_WIN];
static float aux2[MAX_FFT_WIN];

static float gen_chebwin_internal(int N, float at, float *out)
{
        int order = N - 1;
        double R = pow(10.0, fabs(at)/20.0);
        double x0 = cosh(1.0/order * acosh(R));
        double x;
        int k;
        int odd = N % 2;
        /* To improve precision, Freq domain coeffs are computed in double */
        for(k = 0; k < N; k++) {
                x = x0 * cos(M_PI * k / N);
                if (x > 1)
                        out[k] = cosh(order * acosh(x));
                else if (x < -1)
                        out[k] = (1 - 2 * (order % 2)) * cosh(order * acosh(-x));
                else
                        out[k] = cos(order * acos(x));
        }
        int i, j;
        for(i = 0; i < N; i++) {
                complex_t tmp1;
                tmp1.r = 0;
                tmp1.i = 0;
                for(j = 0; j < N; j++) {
                        complex_t tw, tmp2;
                        float theta = -2 * M_PI * i * j / N;
                        tw.r = cosf(theta);
                        tw.i = sinf(theta);
                        tmp2.r = out[j];
                        tmp2.i = 0;
                        if (!odd) {
                                complex_t tmp3;
                                complex_t tw2;
                                float theta2 = M_PI * j / N;
                                tw2.r = cosf(theta2);
                                tw2.i = sinf(theta2);
                                cmult(&tw2, &tmp2, &tmp3);
                                tmp2 = tmp3;
                        }
                        cmult_cum(&tw, &tmp2, &tmp1);
                }
                aux2[i] = tmp1.r;
        }
        int n = odd ? (N+1)/2 : N/2;
        for (k = 0; k < n; k++) {
                if (odd) {
                        out[n-1-k] = aux2[k];
                        out[n-1+k] = aux2[k];
                } else {
                        out[n-k-1] = aux2[k+1];
                        out[n+k] = aux2[k+1];
                }
        }
        normalize(out, N, 1);
        return get_power(out, N);
}

static float gen_hanning_internal(int N, float *out)
{
        uint32_t i;
        for(i = 0; i < N; i++)
                out[i] = 0.5 - 0.5*cos(2*M_PI*i/(N-1));
        return get_power(out, N);
}
static float gen_hamming_internal(int N, float *out)
{
        uint32_t i;
        for(i = 0; i < N; i++)
                out[i] = 0.54 - 0.46*cos(2*M_PI*i/(N-1));
        return get_power(out, N);
}

static float gen_square_internal(uint32_t len, float *aux1)
{
        uint32_t i;
        for(i = 0; i < len; i++)
                aux1[i] = 1.0;
        return get_power(aux1, len);
}

float gen_window(const char *wintype, int len, double param1, double param2, double param3)
{
        float retval = 0;
        if (strcmp(wintype, "cheb") == 0)
                retval = gen_chebwin_internal(len, param1, aux1);
        else if (strcmp(wintype, "hanning") == 0)
                retval = gen_hanning_internal(len, aux1);
        else if (strcmp(wintype, "hamming") == 0)
                retval = gen_hamming_internal(len, aux1);
        else
                retval = gen_square_internal(len, aux1);
        
        return retval;
}

uint32_t get_win_coeff(uint32_t idx)
{
        return float_to_fx(aux1[idx], WIN_COEFF_BITW, WIN_COEFF_INT, false);
}

float get_win_coeff_float(uint32_t idx)
{
        return aux1[idx];
}

float* get_win_buff()
{
        return aux2;
}
