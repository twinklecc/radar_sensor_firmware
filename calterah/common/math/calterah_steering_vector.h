#ifndef CALTERAH_STEERING_VECTOR_H
#define CALTERAH_STEERING_VECTOR_H
#include "calterah_math.h"

typedef struct antenna_pos {
        float x;
        float y;
} antenna_pos_t;

void gen_steering_vec(complex_t *vec,
                      const float *win,
                      const antenna_pos_t *ant_pos,
                      const float *ant_comps,
                      const uint8_t size,
                      const float pm1,  /* theta */
                      const float pm2,  /* phi */
                      const char space,
                      const bool bpm);

void gen_steering_vec2(complex_t *vec,
                      const float *win,
                      const antenna_pos_t *ant_pos,
                      const float *ant_comps,
                      const uint8_t size,
                      const float pm1,  /* theta */
                      const float pm2,  /* phi */
                      const char space,
                      const bool bpm,
                      const bool phase_comp,
                      const uint8_t ant_idx[]);

void arrange_doa_win(const antenna_pos_t *ant_pos,
                     const uint8_t *ant_idx,
                     const uint8_t sv_size,
                     float *win,
                     uint8_t  doa_dir);

#endif
