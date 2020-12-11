#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_error.h"
#include "calterah_error.h"
#endif

#include "baseband.h"
#include "sensor_config.h"
#include "math.h"
#include "fmcw_radio_cli.h"
#include "radio_ctrl.h"
#include "calterah_math.h"

void fmcw_radio_compute_reg_value(fmcw_radio_t *radio)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint32_t gcd = compute_gcd(cfg->adc_freq, FREQ_SYNTH_SD_RATE);
        uint32_t sd_M = FREQ_SYNTH_SD_RATE / gcd;
        uint32_t total_cycle = round(cfg->fmcw_chirp_period * gcd) * sd_M;
        radio->nchirp = cfg->nchirp;
        if (cfg->anti_velamb_en)
                radio->nchirp += cfg->nchirp / cfg->nvarray;
        radio->start_freq = DIV_RATIO(cfg->fmcw_startfreq, FREQ_SYNTH_SD_RATE);
        uint32_t stop_freq = DIV_RATIO(cfg->fmcw_startfreq + cfg->fmcw_bandwidth * 1e-3, FREQ_SYNTH_SD_RATE);
        uint32_t step_up = (1L<<28) * cfg->fmcw_bandwidth / (FREQ_SYNTH_SD_RATE * cfg->fmcw_chirp_rampup * FREQ_SYNTH_SD_RATE * 8);
        uint32_t bandwidth = stop_freq - radio->start_freq;
        uint32_t up_cycle = ceil(1.0 * bandwidth / step_up);
        uint32_t step_down = (1L<<28) * cfg->fmcw_bandwidth / (FREQ_SYNTH_SD_RATE * cfg->fmcw_chirp_down * FREQ_SYNTH_SD_RATE * 8);
        uint32_t down_cycle = ceil(1.0 * bandwidth / step_down);
        uint32_t wait_cycle = total_cycle - up_cycle - down_cycle;
        uint32_t anti_velamb_cycle = round(cfg->anti_velamb_delay * gcd) * sd_M; //cycle alignment for anti-velamb
        uint32_t chirp_shifting_cyle = round(cfg->chirp_shifting_delay * gcd) * sd_M; //cycle alignment for chirp shifting

        /*Added for frequency hopping*/
        radio->hp_start_freq = DIV_RATIO(cfg->fmcw_startfreq + cfg->freq_hopping_deltaf * 1e-3, FREQ_SYNTH_SD_RATE);
        radio->hp_stop_freq = radio->hp_start_freq + bandwidth;
        radio->hp_mid_freq = radio->hp_start_freq + bandwidth / 2;

        bandwidth = step_up * up_cycle;
        radio->stop_freq = radio->start_freq + bandwidth;
        radio->mid_freq = radio->start_freq + bandwidth / 2;
        radio->step_up = step_up;
        radio->step_down = step_down;
        radio->up_cycle = up_cycle;
        radio->down_cycle = down_cycle;
        radio->wait_cycle = wait_cycle;
        radio->total_cycle = total_cycle;
        radio->cnt_wait = radio->wait_cycle + radio->down_cycle;
        radio->anti_velamb_cycle = anti_velamb_cycle;
        radio->chirp_shifting_cyle = chirp_shifting_cyle;

/* move doppler spur */
#if DOPPLER_MOVE == 1
        uint32_t *doppler_move_opt = NULL;
        doppler_move_opt = fmcw_doppler_move(radio);
        if (doppler_move_opt[0]) {
                radio->step_down = doppler_move_opt[1];
                radio->down_cycle = doppler_move_opt[2];
                radio->wait_cycle = doppler_move_opt[3];
        } else {
                EMBARC_PRINTF("no optimized doppler position found");
        }
#endif
}

int32_t fmcw_radio_init(fmcw_radio_t *radio)
{
        fmcw_radio_power_on(radio);
#if HTOL_TEST == 1
        fmcw_radio_gain_compensation(radio);
#endif
        fmcw_radio_compute_reg_value(radio);
        fmcw_radio_generate_fmcw(radio);
#ifndef UNIT_TEST
        fmcw_radio_cli_commands();
#endif
        return E_OK;
}

int32_t fmcw_radio_start(fmcw_radio_t *radio)
{
        fmcw_radio_start_fmcw(radio);
        return E_OK;
}

int32_t fmcw_radio_stop(fmcw_radio_t *radio)
{
        fmcw_radio_stop_fmcw(radio);
        return E_OK;
}
