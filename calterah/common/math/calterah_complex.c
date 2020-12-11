#include "calterah_complex.h"

void cmult_cum(complex_t *in1, complex_t *in2, complex_t *dout)
{
        complex_t tmp;
        cmult(in1, in2, &tmp);
        cadd(&tmp, dout, dout);
}

void cmult_conj_cum(complex_t *in1, complex_t *in2, complex_t *dout)
{
        complex_t tmp;
        cmult_conj(in1, in2, &tmp);
        cadd(&tmp, dout, dout);
}
