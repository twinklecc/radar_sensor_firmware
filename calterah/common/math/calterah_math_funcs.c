#include "calterah_math_funcs.h"
#include "math.h"

void normalize(float *in, int len, int method)
{
        /* method : 0 - no normalization */
        /*        : 1 - normalized to max */
        /*        : 2 - normalized to sum */
        /*        : 3 - normalized to power */
        float sum = 0;
        float sum_sq = 0;
        float max = in[0];
        int i;
        for(i = 0; i < len; i++) {
                sum += in[i];
                sum_sq += in[i] * in[i];
                if (max < in[i])
                        max = in[i];
        }
        float normer;
        switch (method) {
        case 0:
                normer = 1.0f;
                break;
        case 1:
                normer = max;
                break;
        case 2:
                normer = sum;
                break;
        case 3:
                normer = sqrt(sum_sq / len);
                break;
        default:
                normer = 1.0f;
        }
        for(i = 0; i < len; i++)
                in[i] /= normer;
}

float get_power(float *in, int len)
{
        float sum_sq = 0;
        int i;
        for(i = 0; i < len; i++)
                sum_sq += in[i] * in[i];
        return sum_sq / len;
}

/* recursive implementation. It is not heavily used so it is OK */
uint32_t compute_gcd(uint32_t a, uint32_t b)
{
        if(a < b) {
                return compute_gcd(b, a);
        }
        if(a % b == 0) {
                return b;
        }
        return compute_gcd(b, a % b);
}

