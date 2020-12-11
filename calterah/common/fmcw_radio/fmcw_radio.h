#ifndef FMCW_RADIO_H
#define FMCW_RADIO_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

typedef struct fmcw_radio {
        uint32_t start_freq;
        uint32_t stop_freq;
        uint32_t mid_freq;
        uint32_t step_up;
        uint32_t step_down;
        uint32_t cnt_wait;
        uint32_t total_cycle;
        uint32_t sd_freq;
        int32_t nchirp;
        uint8_t frame_type_id;
        uint32_t anti_velamb_cycle;
        uint32_t chirp_shifting_cyle;
        /* following is for debug purpose */
        uint32_t up_cycle;
        uint32_t down_cycle;
        uint32_t wait_cycle;

        uint32_t hp_start_freq;
        uint32_t hp_stop_freq;
        uint32_t hp_mid_freq;
        uint32_t hp_step_down;
} fmcw_radio_t;

int32_t fmcw_radio_init(fmcw_radio_t *radio);
int32_t fmcw_radio_start(fmcw_radio_t *radio);
bool fmcw_radio_is_running(fmcw_radio_t *radio);
int32_t fmcw_radio_stop(fmcw_radio_t *radio);
void fmcw_radio_compute_reg_value(fmcw_radio_t *radio);

#endif
