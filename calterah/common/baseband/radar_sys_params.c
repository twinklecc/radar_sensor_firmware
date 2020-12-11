#include "baseband.h"
#include "sensor_config.h"
#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#include "stdio.h"
#else
#include "embARC_error.h"
#endif
#include "math.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define DEG2RAD 0.017453292519943295f

void radar_param_update(radar_sys_params_t *sys_params)
{
        sensor_config_t* cfg = (sensor_config_t*) CONTAINER_OF(sys_params, baseband_t, sys_params)->cfg;
        float r, v;

        bool anti_velamb_en = cfg ->anti_velamb_en;
        float TD = sys_params->chirp_period * cfg->nvarray;

        if (anti_velamb_en)
                TD = sys_params->chirp_period * (cfg->nvarray + 1) + cfg->anti_velamb_delay;

        /* compute coeff first */
        sys_params->r_k_coeff = sys_params->Fs / cfg->dec_factor / sys_params->rng_nfft; // dividing Fs by dec_factor to correct range calculation
        sys_params->r_p_coeff = 1.0 / (TD * sys_params->vel_nfft);

        sys_params->r_coeff = LIGHTSPEED / 1e+6 / (2 * sys_params->bandwidth) * sys_params->chirp_rampup;
        sys_params->v_coeff = LIGHTSPEED / 1e+3 / (2 * sys_params->vel_nfft * TD * sys_params->carrier_freq);

        radar_param_fft2rv(sys_params, 1, 0, &r, &v);
        sys_params->rng_delta = r;
        radar_param_fft2rv(sys_params, 0, 1, &r, &v);
        sys_params->vel_delta = v;
        sys_params->az_delta_deg = (sys_params->bfm_az_right -
                                    sys_params->bfm_az_left) / sys_params->doa_npoint[0];
        sys_params->ev_delta_deg = (sys_params->bfm_ev_up -
                                    sys_params->bfm_ev_down) / sys_params->doa_npoint[1];
        sys_params->az_delta = sys_params->az_delta_deg * DEG2RAD;
        sys_params->ev_delta = sys_params->ev_delta_deg * DEG2RAD;
        sys_params->vel_wrap_max = cfg->track_vel_pos_ind_portion * sys_params->vel_nfft;
}

uint32_t radar_param_check(const radar_sys_params_t *sys_params)
{
        return E_OK;
}

static uint32_t index_wrap(const int32_t N, int32_t idx)
{
        while (idx >= N/2)
                idx -= N;
        while (idx < -N/2)
                idx += N;
        return idx;
}

static uint32_t index_mirror(const int32_t N, int32_t idx)
{
        while (idx < 0)
                idx += N;
        while (idx >= N)
                idx -= N;
        if (idx > N/2)
                idx = N - idx;
        return idx;
}

void radar_param_fft2rv(const radar_sys_params_t *sys_params,
                        const int32_t k,
                        const int32_t p,
                        float *r,
                        float *v)
{
        int32_t idx_p = index_wrap(sys_params->vel_nfft, p);
        int32_t idx_k = index_wrap(sys_params->rng_nfft, k);
        radar_param_fft2rv_nowrap(sys_params, idx_k, idx_p, r, v);
}

void radar_param_fft2rv_nowrap(const radar_sys_params_t *sys_params,
                               const float k,
                               const float p,
                               float *r,
                               float *v)
{
        *r = sys_params->r_coeff * (sys_params->r_k_coeff * k - sys_params->r_p_coeff * p);
        *v = sys_params->v_coeff * p;
}

void radar_param_rv2fft(const radar_sys_params_t *sys_params,
                        const float r,
                        const float v,
                        int32_t *k,
                        int32_t *p)
{
        sensor_config_t* cfg = (sensor_config_t*) CONTAINER_OF(sys_params, baseband_t, sys_params)->cfg;
        float idx = v * 2 * sys_params->vel_nfft;
        idx *= sys_params->chirp_period;
        /* 1e+3 is to balance unit */
        idx *= sys_params->carrier_freq * 1e+3;
        idx /= LIGHTSPEED;
        int32_t tmpp = round(idx);
        *p = index_wrap(sys_params->vel_nfft, tmpp);
        /* 1e+6 is to balance unit */
        float tmp = r * sys_params->bandwidth * 2 * 1e+6;
        tmp /= sys_params->chirp_rampup * LIGHTSPEED;
        tmp -= idx / (sys_params->chirp_period * sys_params->vel_nfft);
        tmp *= sys_params->rng_nfft / sys_params->Fs * cfg->dec_factor; // dividing Fs by dec_factor to correct range calculation
        *k = index_mirror(sys_params->rng_nfft, round(tmp));
}

void radar_param_config(radar_sys_params_t *sys_params)
{
        fmcw_radio_t* radio = &(CONTAINER_OF(sys_params, baseband_t, sys_params)->radio);
        sensor_config_t* cfg = (sensor_config_t*) CONTAINER_OF(sys_params, baseband_t, sys_params)->cfg;
        float start_freq = INV_DIV_RATIO(radio->start_freq, FREQ_SYNTH_SD_RATE);
        float stop_freq = INV_DIV_RATIO(radio->stop_freq, FREQ_SYNTH_SD_RATE);
        sys_params->carrier_freq = start_freq;
        float bandwidth = (stop_freq - start_freq) * 1e3;
        sys_params->bandwidth = bandwidth ;
#ifdef CHIP_ALPS_A
        float up = 1.0 * (1L << 21) / FREQ_SYNTH_SD_RATE / FREQ_SYNTH_SD_RATE / radio->step_up * bandwidth;
        float down = 1.0 * (1L << 21) / FREQ_SYNTH_SD_RATE / FREQ_SYNTH_SD_RATE / radio->step_down * bandwidth;
        float idle = (1 << radio->cnt_wait) * 1.0 / FREQ_SYNTH_SD_RATE;
#endif
#if (defined(CHIP_ALPS_B) || defined(CHIP_ALPS_MP))
        float up = 1.0 * (1L << 25) / FREQ_SYNTH_SD_RATE / FREQ_SYNTH_SD_RATE / radio->step_up * bandwidth;
        float down = 0;
        float idle =  radio->cnt_wait * 1.0 / FREQ_SYNTH_SD_RATE;
#endif
        sys_params->chirp_rampup = up;
        sys_params->chirp_period = up + down + idle;
        sys_params->Fs = cfg->adc_freq;
        sys_params->rng_nfft = cfg->rng_nfft;
        sys_params->vel_nfft = cfg->vel_nfft;
        sys_params->doa_npoint[0] = cfg->doa_npoint[0];
        sys_params->doa_npoint[1] = cfg->doa_npoint[1];
        sys_params->bfm_az_left = cfg->bfm_az_left;
        sys_params->bfm_az_right = cfg->bfm_az_right;
        sys_params->bfm_ev_up = cfg->bfm_ev_up;
        sys_params->bfm_ev_down = cfg->bfm_ev_down;

        float asin_l = sin(sys_params->bfm_az_left * DEG2RAD);
        float asin_r = sin(sys_params->bfm_az_right * DEG2RAD);
        float asin_step = (asin_r - asin_l) / cfg->doa_npoint[0];

        float asin_d = sin(sys_params->bfm_ev_down * DEG2RAD);
        float asin_u = sin(sys_params->bfm_ev_up * DEG2RAD);
        float asin_step_ev = (asin_u - asin_d) / cfg->doa_npoint[1];

        sys_params->dml_sin_az_left = asin_l;
        sys_params->dml_sin_az_step = asin_step;
        sys_params->dml_sin_ev_down = asin_d;
        sys_params->dml_sin_ev_step = asin_step_ev;

        sys_params->trk_fps = cfg->track_fps;
        sys_params->trk_fov_az_left = cfg->track_fov_az_left;
        sys_params->trk_fov_az_right = cfg->track_fov_az_right;
        sys_params->trk_fov_ev_down = cfg->track_fov_ev_down;
        sys_params->trk_fov_ev_up = cfg->track_fov_ev_up;
        sys_params->trk_nf_thres = cfg->track_near_field_thres;
        sys_params->trk_capt_delay = cfg->track_capture_delay;
        sys_params->trk_drop_delay = cfg->track_drop_delay;
        sys_params->trk_fps = cfg->track_fps;
#ifndef UNIT_TEST
        radar_param_update(sys_params);
#endif
}
