#include "CuTest.h"
#include "baseband.h"
#include "sensor_config.h"
#include "fmcw_radio.h"
#include "stdio.h"

static sensor_config_t cfg;
static baseband_t bb;

static void init_fmcw_radio()
{
        bb.cfg = &cfg;
        cfg.bb = &bb;
}

static void init_config_case0()
{
        cfg.adc_freq = 20;
        cfg.fmcw_startfreq = 76;
        cfg.fmcw_bandwidth = 1000;
        cfg.fmcw_chirp_rampup = 5.12;
        cfg.fmcw_chirp_down = 1.024;
        cfg.fmcw_chirp_period = 5.12 + 1.024 + 2.56;
}

static void init_config_case1()
{
        cfg.adc_freq = 20;
        cfg.fmcw_startfreq = 76.5;
        cfg.fmcw_bandwidth = 1000;
        cfg.fmcw_chirp_rampup = 5.12;
        cfg.fmcw_chirp_down = 1.024;
        cfg.fmcw_chirp_period = 5.12 + 1.024 + 2.56;
}

static void init_config_case2()
{
        cfg.adc_freq = 20;
        cfg.fmcw_startfreq = 76.1;
        cfg.fmcw_bandwidth = 50;
        cfg.fmcw_chirp_rampup = 11;
        cfg.fmcw_chirp_down = 28.2;
        cfg.fmcw_chirp_period = 11 + 28.2 + .8;
}

void test_fmcw_radio_compute_reg_value(CuTest *tc)
{
        init_fmcw_radio();
        init_config_case0();
        fmcw_radio_compute_reg_value(&bb.radio);
        CuAssertIntEquals(tc, 0x7c00000, bb.radio.start_freq);
        CuAssertIntEquals(tc, 0x8100000, bb.radio.stop_freq);
        CuAssertIntEquals(tc, 0x7e80000, bb.radio.mid_freq);
        CuAssertIntEquals(tc, 0xa00, bb.radio.step_up);
        CuAssertIntEquals(tc, 0x3233, bb.radio.step_down);
        CuAssertIntEquals(tc, 10, bb.radio.cnt_wait);
        CuAssertIntEquals(tc, 2048, bb.radio.up_cycle);
        CuAssertIntEquals(tc, 1024, bb.radio.wait_cycle);
        CuAssertIntEquals(tc, 3480, bb.radio.total_cycle);
        init_config_case1();
        fmcw_radio_compute_reg_value(&bb.radio);
        CuAssertIntEquals(tc, 0x7e80000, bb.radio.start_freq);
        CuAssertIntEquals(tc, 0x8380000, bb.radio.stop_freq);
        CuAssertIntEquals(tc, 0x8100000, bb.radio.mid_freq);
        CuAssertIntEquals(tc, 0xa00, bb.radio.step_up);
        CuAssertIntEquals(tc, 0x3233, bb.radio.step_down);
        CuAssertIntEquals(tc, 10, bb.radio.cnt_wait);
        CuAssertIntEquals(tc, 2048, bb.radio.up_cycle);
        CuAssertIntEquals(tc, 1024, bb.radio.wait_cycle);
        CuAssertIntEquals(tc, 3480, bb.radio.total_cycle);
        init_config_case2();
        fmcw_radio_compute_reg_value(&bb.radio);
        CuAssertIntEquals(tc, 0x7c7fff8, bb.radio.start_freq);
        CuAssertIntEquals(tc, 0x7cc8fc1, bb.radio.stop_freq);
        CuAssertIntEquals(tc, 0x7ca47dc, bb.radio.mid_freq);
        CuAssertIntEquals(tc, 0x3B, bb.radio.step_up);
        CuAssertIntEquals(tc, 0x1C, bb.radio.step_down);
        CuAssertIntEquals(tc, 8, bb.radio.cnt_wait);
        CuAssertIntEquals(tc, 5067, bb.radio.up_cycle);
        CuAssertIntEquals(tc, 256, bb.radio.wait_cycle);
        CuAssertIntEquals(tc, 16000, bb.radio.total_cycle);
}

CuSuite* fmcw_radio_get_suite()
{
        CuSuite* suite = CuSuiteNew();
        SUITE_ADD_TEST(suite, test_fmcw_radio_compute_reg_value);
        return suite;
}
