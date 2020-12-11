#include "CuTest.h"
#include "fft_window.h"

void test_fft_window(CuTest *tc)
{
        int len = 1000;
        gen_window("cheb", len, 80, 0, 0);
        uint32_t v = get_win_coeff(len/2);
        CuAssertIntEquals(tc, 0x3FFF, v);
}

CuSuite* fft_window_get_suite() {
        CuSuite* suite = CuSuiteNew();
        SUITE_ADD_TEST(suite, test_fft_window);
        return suite;
}
