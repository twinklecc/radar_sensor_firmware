#include "CuTest.h"
#include "baseband.h"
#include "radar_sys_params.h"
#include "sensor_config.h"
#include "stdio.h"

static sensor_config_t cfg;
static baseband_t bb;

static void init_radar_sys_params()
{
        bb.cfg = &cfg;
        cfg.bb = &bb;
}

static void init_config_case0()
{
        bb.radio.start_freq = 0x7c00000;
        bb.radio.stop_freq = 0x8100000;
        bb.radio.mid_freq = 0x7e80000;
        bb.radio.step_up = 0xa00;
        bb.radio.step_down = 0x3233;
        bb.radio.cnt_wait = 10;
}

static void init_config_case1()
{
        bb.radio.start_freq = 0x7e80000;
        bb.radio.stop_freq = 0x8380000;
        bb.radio.mid_freq = 0x8100000;
        bb.radio.step_up = 0xa00;
        bb.radio.step_down = 0x3233;
        bb.radio.cnt_wait = 10;
}

static void init_config_case2()
{
        bb.radio.start_freq = 0x7c7fff8;
        bb.radio.stop_freq = 0x7cc8fc1;
        bb.radio.mid_freq = 0x8100000;
        bb.radio.step_up = 0x3B;
        bb.radio.step_down = 0x1C;
        bb.radio.cnt_wait = 8;
}


void test_radar_param_config(CuTest *tc)
{
        float df = 1e-6;
        init_radar_sys_params();
        init_config_case0();
        radar_param_config(&bb.sys_params);
        CuAssertDblEquals(tc, 76, bb.sys_params.carrier_freq, df);
        CuAssertDblEquals(tc, 1000, bb.sys_params.bandwidth, df);
        CuAssertDblEquals(tc, 5.12, bb.sys_params.chirp_rampup, df);
        CuAssertDblEquals(tc, 8.699935913085938, bb.sys_params.chirp_period, df);
        init_config_case1();
        radar_param_config(&bb.sys_params);
        CuAssertDblEquals(tc, 76.5, bb.sys_params.carrier_freq, df);
        CuAssertDblEquals(tc, 1000, bb.sys_params.bandwidth, df);
        CuAssertDblEquals(tc, 5.12, bb.sys_params.chirp_rampup, df);
        CuAssertDblEquals(tc, 8.699935913085938, bb.sys_params.chirp_period, df);
        init_config_case2();
        radar_param_config(&bb.sys_params);
        CuAssertDblEquals(tc, 76.1, bb.sys_params.carrier_freq, 1e-5);
        CuAssertDblEquals(tc, 57.022094, bb.sys_params.bandwidth, 1e-5);
        CuAssertDblEquals(tc, 12.667796, bb.sys_params.chirp_rampup, df);
        CuAssertDblEquals(tc, 40.000652, bb.sys_params.chirp_period, df);
}

static void init_params_case0()
{
        bb.sys_params.carrier_freq = 76;
        bb.sys_params.bandwidth = 1000;
        bb.sys_params.chirp_rampup = 50;
        bb.sys_params.chirp_period = 60;
        bb.sys_params.Fs = 20;
        bb.sys_params.rng_nfft = 512;
        bb.sys_params.vel_nfft = 256;
        bb.sys_params.bfm_npoint = 360;
        bb.sys_params.bfm_az_left = -60;
        bb.sys_params.bfm_az_left = 60;
        bb.sys_params.bfm_ev_up = 60;
        bb.sys_params.bfm_ev_down = -60;
        radar_param_update(&bb.sys_params);
}

void test_radar_param_fft2rv(CuTest *tc)
{
        float r, v;
        float dl = 1e-6;
        init_radar_sys_params();
        init_params_case0();
        radar_param_fft2rv(&bb.sys_params, 10, 20, &r, &v);
        CuAssertDblEquals(tc, 2.9179017, r, dl);
        CuAssertDblEquals(tc, 2.5681233, v, dl);
        radar_param_fft2rv(&bb.sys_params, 0, 1, &r, &v);
        CuAssertDblEquals(tc, -0.0004879, r, dl);
        CuAssertDblEquals(tc, 0.128406167, v, dl);
        radar_param_fft2rv(&bb.sys_params, 1, 0, &r, &v);
        CuAssertDblEquals(tc, 0.292766, r, dl);
        CuAssertDblEquals(tc, 0, v, dl);
        radar_param_fft2rv(&bb.sys_params, bb.sys_params.rng_nfft/2, bb.sys_params.vel_nfft/2, &r, &v);
        CuAssertDblEquals(tc, -74.88565826, r, dl);
        CuAssertDblEquals(tc, -16.435989, v, dl);
}

void test_radar_param_rv2fft(CuTest *tc)
{
        int32_t k, p;
        init_radar_sys_params();
        init_params_case0();
        radar_param_rv2fft(&bb.sys_params, 10, 20, &k, &p);
        CuAssertIntEquals(tc, 34, k);
        CuAssertIntEquals(tc, -100, p);
        radar_param_rv2fft(&bb.sys_params, 100, 2, &k, &p);
        CuAssertIntEquals(tc, 170, k);
        CuAssertIntEquals(tc, 16, p);
        radar_param_rv2fft(&bb.sys_params, 0, 1, &k, &p);
        CuAssertIntEquals(tc, 0, k);
        CuAssertIntEquals(tc, 8, p);
        radar_param_rv2fft(&bb.sys_params, 1, 0, &k, &p);
        CuAssertIntEquals(tc, 3, k);
        CuAssertIntEquals(tc, 0, p);
}


CuSuite* radar_sys_params_get_suite()
{
        CuSuite* suite = CuSuiteNew();
        SUITE_ADD_TEST(suite, test_radar_param_config);
        SUITE_ADD_TEST(suite, test_radar_param_fft2rv);
        SUITE_ADD_TEST(suite, test_radar_param_rv2fft);
        return suite;
}
