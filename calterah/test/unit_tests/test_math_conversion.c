#include "CuTest.h"
#include "calterah_data_conversion.h"

void test_fx_to_float(CuTest *tc)
{
        uint32_t input = 0x399;
        float actual = fx_to_float(input, 10, 1, false);
        float expected = 1.798828;
        float df = 1e-6;
        CuAssertDblEquals(tc, expected, actual, df);
        actual = fx_to_float(0x199, 10, 1, true);
        expected = 0.798828;
        CuAssertDblEquals(tc, expected, actual, df);
        actual = fx_to_float(0x19a, 10, -1, true);
        expected = 0.200195;
        CuAssertDblEquals(tc, expected, actual, df);
}

void test_fl_to_float(CuTest *tc)
{
        uint32_t input = 0x19990;
        float actual = fl_to_float(input, 14, 1, true, 4, false);
        float expected = 0.799927;
        float df = 1e-6;
        CuAssertDblEquals(tc, expected, actual, df);
        actual = fl_to_float(0x1a368, 14, -1, true, 4, false);
        expected = 0.000799894;
        df = 1e-9;
        CuAssertDblEquals(tc, expected, actual, df);
}

void test_cfx_to_complex(CuTest *tc)
{
        uint32_t input = 0xc0100;
        float df = 1e-6;
        complex_t actual = cfx_to_complex(input, 10, 1, false);
        complex_t expected;
        expected.r = 1.5;
        expected.i = 0.5;
        CuAssertDblEquals(tc, expected.r, actual.r, df);
        CuAssertDblEquals(tc, expected.i, actual.i, df);
        input = 0x668cd;
        actual = cfx_to_complex(input, 10, -1, true);
        df = 1e-9;
        expected.r = 0.2001953125;
        expected.i = 0.10009765625;
        CuAssertDblEquals(tc, expected.r, actual.r, df);
        CuAssertDblEquals(tc, expected.i, actual.i, df);
}

void test_cfl_to_complex(CuTest *tc)
{
        uint32_t input = 0x2000c007;
        float df = 1e-6;
        complex_t actual = cfl_to_complex(input, 14, 2, true, 3, true);
        complex_t expected;
        expected.r = 0.5;
        expected.i = 0.75;
        CuAssertDblEquals(tc, expected.r, actual.r, df);
        CuAssertDblEquals(tc, expected.i, actual.i, df);
        input = 0x10006000;
        actual = cfl_to_complex(input, 14, 2, true, 3, false);
        expected.r = 0.5;
        expected.i = 0.75;
        CuAssertDblEquals(tc, expected.r, actual.r, df);
        CuAssertDblEquals(tc, expected.i, actual.i, df);
}

void test_float_to_fx(CuTest *tc)
{
        float input = -1.0f;
        uint32_t d;
        d = float_to_fx(input, 14, 1, true);
        CuAssertIntEquals(tc, 0x2000, d);
}

void test_complex_to_cfx(CuTest *tc)
{
        complex_t input = {-1.000000000000000, -0.000000087422777};
        uint32_t d;
        d = complex_to_cfx(&input, 14, 1, true);
        CuAssertIntEquals(tc, 0x8000000, d);
}


CuSuite* math_conversion_get_suite() {
        CuSuite* suite = CuSuiteNew();
        SUITE_ADD_TEST(suite, test_fx_to_float);
        SUITE_ADD_TEST(suite, test_fl_to_float);
        SUITE_ADD_TEST(suite, test_cfx_to_complex);
        SUITE_ADD_TEST(suite, test_cfl_to_complex);
        SUITE_ADD_TEST(suite, test_float_to_fx);
        SUITE_ADD_TEST(suite, test_complex_to_cfx);
        return suite;
}
