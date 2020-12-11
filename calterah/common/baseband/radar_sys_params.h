#ifndef RADAR_SYS_PARAMS_H
#define RADAR_SYS_PARAMS_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#elif TRACK_TEST_MODE
#include "stdint.h"
#include "stdbool.h"
#else
#include "embARC_toolchain.h"
#endif

#define LIGHTSPEED 299792458.0

typedef struct radar_sys_params {
        float carrier_freq;      /* in GHz   */
        float bandwidth;         /* in MHz   */
        float chirp_rampup;      /* in us    */
        float chirp_period;      /* in us    */
        float Fs;                /* in MHz   */
        uint32_t rng_nfft;       /* unitless */
        uint32_t vel_nfft;       /* unitless */
        uint32_t doa_npoint[2];     /* unitless */
        float bfm_az_left;       /* in deg   */
        float bfm_az_right;      /* in deg   */
        float bfm_ev_up;         /* in deg   */
        float bfm_ev_down;       /* in deg   */

        float dml_sin_az_left;   /* in rad */
        float dml_sin_az_step;   /* in rad */
        float dml_sin_ev_down;   /* in rad */
        float dml_sin_ev_step;   /* in rad */

        float trk_fov_az_left;   /* in deg   */
        float trk_fov_az_right;  /* in deg   */
        float trk_fov_ev_up;   /* in deg   */
        float trk_fov_ev_down;  /* in deg   */
        float trk_nf_thres;      /* in m     */
        float trk_drop_delay;    /* in sec   */
        float trk_capt_delay;    /* in sec   */
        uint32_t trk_fps;        /* frm/sec  */
        float noise_factor;      /* noise_factor */
        /* derived values for quick access */
        float rng_delta;       /* in m   */
        float vel_delta;       /* in m/s */
        float az_delta;        /* in rad */
        float ev_delta;        /* in rad */
        float az_delta_deg;    /* in deg */
        float ev_delta_deg;    /* in deg */
        float vel_wrap_max;
        /* derived high velocity compensating values for quick access */
        float vel_comp;
        float rng_comp;
        /* derived values to save computation time */
        float r_k_coeff;
        float r_p_coeff;
        float r_coeff;
        float v_coeff;
} radar_sys_params_t;

void radar_param_fft2rv(const radar_sys_params_t *sys_params, const int32_t k, const int32_t p, float *r, float *v);

void radar_param_fft2rv_nowrap(const radar_sys_params_t *sys_params, const float k, const float p, float *r, float *v);

void radar_param_rv2fft(const radar_sys_params_t *sys_params, const float r, const float v, int32_t *k, int32_t *p);

void radar_param_update(radar_sys_params_t *sys_params);

uint32_t radar_param_check(const radar_sys_params_t *sys_params);

void radar_param_config(radar_sys_params_t *sys_params);

#endif
