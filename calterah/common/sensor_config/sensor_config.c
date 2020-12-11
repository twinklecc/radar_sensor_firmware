#include "sensor_config.h"
#include "embARC_assert.h"
#include "baseband.h"
#include <string.h>
#include "sensor_config_cli.h"
#include "flash.h"
#include "calterah_math.h"


#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static uint8_t buff[ANTENNA_INFO_LEN];
static uint8_t current_cfg_idx = 0;

#ifndef CHIP_CASCADE
static sensor_config_t config[MAX_FRAME_TYPE] =
{
#if NUM_VARRAY == 1
        {
        #include "sensor_config_init0.hxx"
                .bb = NULL,
        },
        {
        #include "sensor_config_init1.hxx"
                .bb = NULL,
        },
        {
        #include "sensor_config_init2.hxx"
                .bb = NULL,
        },
        {
        #include "sensor_config_init3.hxx"
                .bb = NULL,
        },
#elif NUM_VARRAY == 2
        {
        #include "sensor_config_init_varray2.hxx"
                .bb = NULL,
        },
        {
        #include "sensor_config_init_varray2_1.hxx"
                .bb = NULL,
        },
#elif NUM_VARRAY == 3
        {
        #include "sensor_config_init_varray3.hxx"
                .bb = NULL,
        },
#else
        {
        #include "sensor_config_init_varray4.hxx"
                .bb = NULL,
        },
#endif
};

#else
static sensor_config_t config[MAX_FRAME_TYPE] =
{
        {
        #include "sensor_config_init0_cas.hxx"
                .bb = NULL,
        },
        {
        #include "sensor_config_init1_cas.hxx"
                .bb = NULL,
        },
};
#endif // CHIP_CASCADE

uint8_t *sensor_config_get_buff()
{
        return buff;
}

void sensor_config_clear_buff()
{
        for(uint32_t i = 0; i < ANTENNA_INFO_LEN; i++){
                buff[i] = 0;
        }
}


static void sensor_config_preattach(sensor_config_t *cfg)
{
        int32_t status;
        uint32_t *head;
        if (cfg->ant_info_from_flash) {
                status = flash_memory_read(cfg->ant_info_flash_addr, buff, ANTENNA_INFO_LEN);
                if (status != E_OK) {
                        EMBARC_PRINTF("/*** Fail to read on-flash antenna position. Fallback to default setting! ***/\n\r");
                        return;
                }
                head = (uint32_t *)buff;
                if (*head != ANTENNA_INFO_MAGICNUM) {
                        EMBARC_PRINTF("/*** Invalid on-flash antenna position. Fallback to default setting! ***/\r\n");
                        return;
                }
                antenna_pos_t *pos = (antenna_pos_t *)(buff + 4);
                for(int a = 0; a < MAX_ANT_ARRAY_SIZE; a++) {
                        uint32_t *tmp = (uint32_t *)&pos[a];
                        if (*tmp == 0xFFFFFFFF) /* prevent get invalid data */
                                break;
                        cfg->ant_pos[a] = pos[a];
                }
                status = flash_memory_read(cfg->ant_info_flash_addr + ANTENNA_INFO_LEN, buff, ANTENNA_INFO_LEN);
                if (status != E_OK) {
                        EMBARC_PRINTF("/*** Fail to read on-flash antenna compensation. Fallback to default setting! ***/\n\r");
                }
                head = (uint32_t *)buff;
                if (*head != ANTENNA_INFO_MAGICNUM) {
                        EMBARC_PRINTF("/*** Invalid on-flash antenna compensation. Fallback to default setting! ***/\r\n");
                        return;
                }
                float *comp = (float *)(buff + 4);
                for(int a = 0; a < MAX_ANT_ARRAY_SIZE; a++) {
                        uint32_t *tmp = (uint32_t *)&comp[a];
                        if (*tmp == 0xFFFFFFFF) /* prevent get invalid data */
                                break;
                        cfg->ant_comps[a] = comp[a];
                }
        }
}

int32_t sensor_config_attach(sensor_config_t *cfg, void *bb, uint8_t frame_type)
{
        int32_t status = E_OK;
        cfg->bb = bb;
        sensor_config_check(cfg);
        sensor_config_preattach(cfg);
        ((baseband_t *)bb)->cfg = (void *)cfg;
        ((baseband_t *)bb)->bb_hw.frame_type_id = frame_type;

        status = baseband_init((baseband_t*)bb);
        if (status != E_OK)
                EMBARC_PRINTF("/*** sensor_config_attach fails with error code %d ***/", status);
        return status;
}

void sensor_config_check(sensor_config_t *cfg)
{
        /*EMBARC_ASSERT(cfg->fmcw_startfreq >= 74 && cfg->fmcw_startfreq <= 81);
          float stop_freq = cfg->fmcw_startfreq + cfg->fmcw_bandwidth * 1e-3;
          EMBARC_ASSERT(stop_freq >= 74 && stop_freq <= 81);
        */
        sensor_config_tx_group_check(cfg);
        EMBARC_ASSERT(cfg->fmcw_chirp_rampup > 0);
        EMBARC_ASSERT(cfg->fmcw_chirp_down > 0);
        EMBARC_ASSERT(cfg->fmcw_chirp_period >= cfg->fmcw_chirp_rampup + cfg->fmcw_chirp_down);
        EMBARC_ASSERT(cfg->adc_sample_start > 0 && cfg->adc_sample_end > cfg->adc_sample_start);
        EMBARC_ASSERT(cfg->adc_sample_end > 0);
        EMBARC_ASSERT(cfg->nchirp <= 4096);
#ifdef CHIP_ALPS_MP
        EMBARC_ASSERT((cfg->doa_npoint[0] <= 360 && cfg->doa_method == 0) || (cfg->doa_method == 2 && cfg->doa_npoint[0] <= 760));
        EMBARC_ASSERT(cfg->doa_samp_space == 'u' || cfg->doa_samp_space == 't');
        EMBARC_ASSERT((cfg->doa_max_obj_per_bin[0] <= 4 && cfg->doa_mode != 2) || (cfg->doa_max_obj_per_bin[0] != 12 && cfg->doa_mode == 2));
#endif
        EMBARC_ASSERT(cfg->bfm_az_left >= -90);
        EMBARC_ASSERT(cfg->bfm_az_right <= 90);
        EMBARC_ASSERT(cfg->bfm_az_right >= cfg->bfm_az_left);
        EMBARC_ASSERT(cfg->dec_factor <= 16 && cfg->dec_factor > 0);
        EMBARC_ASSERT(cfg->nvarray > 0);

        EMBARC_ASSERT(cfg->fmcw_bandwidth > 0 && cfg->fmcw_bandwidth <= 5000);
        EMBARC_ASSERT(cfg->vel_nfft <= 1024);
        EMBARC_ASSERT(cfg->rng_nfft <= 2048);
        EMBARC_ASSERT(cfg->vel_nfft * cfg->rng_nfft * cfg->nvarray <= 512 * 512);
        EMBARC_ASSERT(cfg->adc_freq == 20 || cfg->adc_freq == 25 || cfg->adc_freq == 40 || cfg->adc_freq == 50 );

        if (cfg->anti_velamb_en) {
                EMBARC_ASSERT(cfg->nvarray != 4);
                EMBARC_ASSERT(cfg->bpm_mode != true);
                EMBARC_ASSERT(cfg->vel_nfft * cfg->rng_nfft * (cfg->nvarray + 1) <= 512 * 512);

                float            Tr = cfg->fmcw_chirp_period;
                float         delay = cfg->anti_velamb_delay;
                uint32_t    nvarray = cfg->nvarray;
                int32_t       q_min = cfg->anti_velamb_qmin;

                uint32_t Td = (uint32_t)round((nvarray + 1) * Tr + delay);
                uint32_t q_num = MIN(32, Td / compute_gcd((uint32_t)round(Tr + delay), Td));
                int32_t q_max = q_min + q_num - 1;

                EMBARC_ASSERT(q_min >= -16 && q_min <= 15 && q_max >= -16 && q_max <=15);
        }

        /* ::TODO XL check limitation in MP */
        /*
        for (int ch = 0; ch < MAX_NUM_TX; ch++)
        {
            EMBARC_ASSERT(cfg->tx_phase_value[ch] ==   0 || cfg->tx_phase_value[ch] ==  22 || cfg->tx_phase_value[ch] == 45  || cfg->tx_phase_value[ch] ==  67 ||
                          cfg->tx_phase_value[ch] ==  90 || cfg->tx_phase_value[ch] == 112 || cfg->tx_phase_value[ch] == 135 || cfg->tx_phase_value[ch] == 157 ||
                          cfg->tx_phase_value[ch] == 180 || cfg->tx_phase_value[ch] == 202 || cfg->tx_phase_value[ch] == 225 || cfg->tx_phase_value[ch] == 247 ||
                          cfg->tx_phase_value[ch] == 270 || cfg->tx_phase_value[ch] == 292 || cfg->tx_phase_value[ch] == 315 || cfg->tx_phase_value[ch] == 337 );
        }
        */
        /* FIXME add more checks */
}

sensor_config_t *sensor_config_get_config(uint32_t idx)
{
        if (idx >= NUM_FRAME_TYPE)
                return NULL;
        else
                return &config[idx];
}

sensor_config_t *sensor_config_get_cur_cfg()
{
        return sensor_config_get_config(current_cfg_idx);
}

uint8_t sensor_config_get_cur_cfg_idx()
{
        return current_cfg_idx;
}

void sensor_config_set_cur_cfg_idx(uint32_t idx)
{
        if (idx < NUM_FRAME_TYPE)
                current_cfg_idx = idx;
}

void sensor_config_tx_group_check(sensor_config_t *cfg) {
        int32_t tx_grp[MAX_NUM_TX] = {0, 0, 0, 0}; /*recording the last chip index to transmit signal for power on Tx*/
        int32_t max_idx[MAX_NUM_TX] = {-1, -1, -1, -1}; /*recording the last chip index to transmit signal for all Tx*/
        int num = 0;       /*recording the number of power on Tx*/
        int32_t chip_idx = -1;   /*recording the last chip index to transmit signal, related to n_va*/
        uint32_t chirp_tx_mux[MAX_NUM_TX] = {0, 0, 0, 0}; /*chirp_tx_mux[0]:which tx transmit in chirp 0*/
        int8_t bpm_checker = -1;

        for (int i=0; i<MAX_NUM_TX; i++) {
                uint16_t bit_mux[MAX_NUM_TX] = {0,0,0,0};
                EMBARC_ASSERT(bit_parse(cfg->tx_groups[i], bit_mux));
                for (int c = 0; c < MAX_NUM_TX; c++) {
                        if (bit_mux[c] != 0)
                            max_idx[i] = c;
                }
                if ( max_idx[i] >= 0) {
                    tx_grp[num] = max_idx[i];
                    num = num + 1;
                }
        }
        for (int i=0; i<num; i++) {
                if (tx_grp[i] > chip_idx)
                    chip_idx = tx_grp[i];
        }
        /* computing nvarray */
        if (chip_idx >= 1) {
                if (cfg->bpm_mode == false) {
                        EMBARC_ASSERT(get_tdm_tx_antenna_in_chirp(cfg->tx_groups, chip_idx, chirp_tx_mux));
                } else {
                        EMBARC_ASSERT((chip_idx == 1)||(chip_idx == 3));
                        bpm_checker = get_bpm_tx_antenna_in_chirp(cfg->tx_groups, chip_idx, chirp_tx_mux);
                        EMBARC_ASSERT(bpm_checker != -1 );
                }
                cfg->nvarray = chip_idx + 1;
        } else if(chip_idx < 0) {
                EMBARC_PRINTF("Notice TX are all off\n");
                cfg->nvarray = 1;
        } else {
                EMBARC_ASSERT(get_tdm_tx_antenna_in_chirp(cfg->tx_groups, chip_idx, chirp_tx_mux));
                cfg->nvarray = 1;
        }
}
