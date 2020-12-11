#include "calterah_steering_vector.h"
#include "math.h"
#include "calterah_limits.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

#define DEG2RAD 0.017453292519943295f

static void de_mod(complex_t *vec, const uint8_t size)
{
        complex_t tmp[MAX_NUM_RX];
        int i = 0;
        int idx = size / MAX_NUM_RX;
        if (idx == 2) {
                for(i = 0; i < MAX_NUM_RX; i++) {
                        tmp[0] = vec[i];
                        tmp[1] = vec[i + MAX_NUM_RX];
                        cadd(&tmp[0], &tmp[1], &vec[i]);
                        csub(&tmp[0], &tmp[1], &vec[i + MAX_NUM_RX]);
                        crmult(&vec[i], .5f,  &vec[i]);
                        crmult(&vec[i + MAX_NUM_RX], .5f,  &vec[i + MAX_NUM_RX]);
                }
        } else if (idx == 4) {
                for(i = 0; i < MAX_NUM_RX; i++) {
                        tmp[0] = vec[i];
                        tmp[1] = vec[i + MAX_NUM_RX];
                        tmp[2] = vec[i + 2 * MAX_NUM_RX];
                        tmp[3] = vec[i + 3 * MAX_NUM_RX];
                        vec[i].r = tmp[0].r + tmp[1].r + tmp[2].r + tmp[3].r;
                        vec[i].i = tmp[0].i + tmp[1].i + tmp[2].i + tmp[3].i;
                        vec[i + MAX_NUM_RX].r = tmp[0].r - tmp[1].r + tmp[2].r - tmp[3].r;
                        vec[i + MAX_NUM_RX].i = tmp[0].i - tmp[1].i + tmp[2].i - tmp[3].i;
                        vec[i + 2 * MAX_NUM_RX].r = tmp[0].r + tmp[1].r - tmp[2].r - tmp[3].r;
                        vec[i + 2 * MAX_NUM_RX].i = tmp[0].i + tmp[1].i - tmp[2].i - tmp[3].i;
                        vec[i + 3 * MAX_NUM_RX].r = tmp[0].r - tmp[1].r - tmp[2].r + tmp[3].r;
                        vec[i + 3 * MAX_NUM_RX].i = tmp[0].i - tmp[1].i - tmp[2].i + tmp[3].i;
                        crmult(&vec[i], .25f,  &vec[i]);
                        crmult(&vec[i + MAX_NUM_RX], .25f,  &vec[i + MAX_NUM_RX]);
                        crmult(&vec[i + 2 * MAX_NUM_RX], .25f,  &vec[i + 2 * MAX_NUM_RX]);
                        crmult(&vec[i + 3 * MAX_NUM_RX], .25f,  &vec[i + 3 * MAX_NUM_RX]);
                }
        }
}

void gen_steering_vec(complex_t *vec,
                      const float *win,
                      const antenna_pos_t *ant_pos, /* in wavelength */
                      const float *ant_comps,       /* in deg */
                      const uint8_t size,
                      const float pm1,              /* theta in rad*/
                      const float pm2,              /* phi in rad */
                      const char space,
                      const bool bpm)
{
        float v_x, v_y;
        switch (space)
        {
        case 'u' :
                v_x = pm1;
                v_y = pm2;
                break;
        default :
                v_x = sinf(pm1) * cosf(pm2);
                v_y = sinf(pm2);
                break;
        }
        int ch = 0;
        for(ch = 0; ch < size; ch++) {
                float tw = ant_pos[ch].x * v_x + ant_pos[ch].y * v_y;
                /* FIXME: need to doublecheck the sign */
                complex_t tmp = expj(2 * M_PI * tw + ant_comps[ch] * DEG2RAD);
                crmult(&tmp, win[ch], &vec[ch]);
        }
        if (bpm)
                de_mod(vec, size);
}

void gen_steering_vec2(complex_t *vec,
                      const float *win,
                      const antenna_pos_t *ant_pos, /* in wavelength */
                      const float *ant_comps,       /* in deg */
                      const uint8_t size,           /*the size of steering vector*/
                      const float pm1,              /* theta in rad*/
                      const float pm2,              /* phi in rad */
                      const char space,
                      const bool bpm,
                      const bool phase_comp,        /*flag for if need do phase_compensation, added for combined mode to generate sv of elevated direction*/
                      const uint8_t ant_idx[])   /*physical rx ant idx, virtualized by TDM va=4 tx_group=[1 16 256 4096]*/
{
        float v_x, v_y;
        switch (space)
        {
        case 'u' :
                v_x = pm1;
                v_y = pm2;
                break;
        default :
                v_x = sinf(pm1) * cosf(pm2);
                v_y = sinf(pm2);
                break;
        }
        int ch = 0;
        int comb_ant_num = 0;
        for(ch = 0; ch < size; ch++) {
                        float tw = ant_pos[ant_idx[ch]].x * v_x + ant_pos[ant_idx[ch]].y * v_y;
                        /* FIXME: need to doublecheck the sign */
                        int idx = bpm ? ant_idx[ch]%DOA_MAX_NUM_RX : ant_idx[ch];
                        float phase = phase_comp ? ant_comps[idx] : 0;
                        complex_t tmp = expj(-2 * M_PI * tw - phase * DEG2RAD); // use conjugate steering vector
                        crmult(&tmp, win[comb_ant_num], &vec[comb_ant_num]);
                        comb_ant_num = comb_ant_num + 1;
        }
}

void arrange_doa_win(const antenna_pos_t *ant_pos,
                     const uint8_t *ant_idx,
                     const uint8_t sv_size,
                     float *win,
                     uint8_t  doa_dir) {

        float distance[MAX_ANT_ARRAY_SIZE];
        int sorted_ind[MAX_ANT_ARRAY_SIZE];
        float win_sort[MAX_ANT_ARRAY_SIZE];

        for (uint8_t k = 0; k < sv_size; k++) {
                antenna_pos_t ant_pos_tmp = ant_pos[ant_idx[k]];
                // choose one coordinate of ant_pos for sorting
                if (doa_dir == 0)
                        distance[k] = ant_pos_tmp.x;
                else if (doa_dir == 1)
                        distance[k] = ant_pos_tmp.y;
                else
                        distance[k] = ant_pos_tmp.x;
        }

        bubble_sort(distance, sv_size, sorted_ind);

        for (uint8_t k = 0; k < sv_size; k++) {
                win_sort[sorted_ind[k]] = win[k];
        }

        for (uint8_t k = 0; k < sv_size; k++) {
                win[k] = win_sort[k];
        }
}
