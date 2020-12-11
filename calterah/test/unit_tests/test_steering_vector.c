#include "CuTest.h"
#include "calterah_steering_vector.h"
#include "fft_window.h"
#include "stdio.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

void test_gen_steering_vector(CuTest *tc)
{
        complex_t vec[16];
        float win[16];
        antenna_pos_t ant_pos[16] = {{0, 0}, {.5, 0}, {1, 0}, {1.5, 0},
                                     {2, 0}, {2.5, 0}, {3.0, 0}, {3.5, 0},
                                     {4, 0}, {4.5, 0}, {5, 0}, {5.5, 0},
                                     {6, 0}, {6.5, 0}, {7.0, 0}, {7.5, 0}};
        float ant_comp[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0};
        int i;
        gen_window("square", 4, 80, 0, 0);
        for(i = 0; i < 4; i++)
                win[i] = get_win_coeff_float(i);
        gen_steering_vec(vec, win, ant_pos, ant_comp, 4, 90.0/180 * M_PI, 0, 't', false);
        CuAssertDblEquals(tc, 1,  vec[0].r, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[0].i, 1e-5);
        CuAssertDblEquals(tc, -1, vec[1].r, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[1].i, 1e-5);
        CuAssertDblEquals(tc, 1,  vec[2].r, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[2].i, 1e-5);
        CuAssertDblEquals(tc, -1, vec[3].r, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[3].i, 1e-5);
        gen_steering_vec(vec, win, ant_pos, ant_comp, 4, 30.0/180 * M_PI, 0, 't', false);
        CuAssertDblEquals(tc, 1,  vec[0].r, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[0].i, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[1].r, 1e-5);
        CuAssertDblEquals(tc, 1,  vec[1].i, 1e-5);
        CuAssertDblEquals(tc, -1, vec[2].r, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[2].i, 1e-5);
        CuAssertDblEquals(tc, 0,  vec[3].r, 1e-5);
        CuAssertDblEquals(tc, -1, vec[3].i, 1e-5);
}

CuSuite* steering_vector_get_suite() {
        CuSuite* suite = CuSuiteNew();
        SUITE_ADD_TEST(suite, test_gen_steering_vector);
        return suite;
}
