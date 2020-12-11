#ifndef BASEBAND_H
#define BASEBAND_H

#include "fmcw_radio.h"
#include "track.h"
#include "cluster.h"
#include "func_safety.h"
#include "radar_sys_params.h"
#include "baseband_hw.h"

typedef struct baseband {
        radar_sys_params_t sys_params;
        fmcw_radio_t       radio;
        baseband_hw_t      bb_hw;
        cluster_t          cluster;
        track_t*           track;
        func_safety_t      fsm;
        void               *cfg;
} baseband_t;

#define FI_FIXED                    \
{                                   \
        .strategy = FIXED,          \
        .sw_num   = 1,              \
        .sel_0    = CFG_0,          \
        .sel_1    = CFG_0,          \
}

#define FI_ROTATE                   \
{                                   \
        .strategy = ROTATE,         \
        .sw_num   = NUM_FRAME_TYPE, \
        .sel_0    = CFG_0,          \
        .sel_1    = CFG_1,          \
}

#define FI_AIR_CONDITIONER          \
{                                   \
        .strategy = AIR_CONDITIONER,\
        .sw_num   = 100,            \
        .sel_0    = CFG_0,          \
        .sel_1    = CFG_1,          \
}

#define FI_VELAMB                   \
{                                   \
        .strategy = VELAMB,         \
        .sw_num   = 3,              \
        .sel_0    = CFG_0,          \
        .sel_1    = CFG_1,          \
}

typedef struct frame_intrlv {        // frame interleaving
        uint8_t            strategy; // FIXMED or ROTATE or AIR_CONDITIONER or VELAMB or customized by yourself
        uint32_t           sw_num;   // loop period or switch number
        uint8_t            sel_0;    // 1st frame type selected
        uint8_t            sel_1;    // 2nd frame type selected
} frame_intrlv_t;

int32_t baseband_init(baseband_t *bb);
void baseband_clock_init();
void baseband_start(baseband_t *bb);
void baseband_stop(baseband_t *bb);
void baseband_scan_stop(void);
int32_t baseband_dump_init(baseband_t *bb, bool sys_buf_store);
void baseband_start_with_params(baseband_t *bb, bool radio_en, bool tx_en, uint16_t sys_enable, bool cas_sync_en, uint8_t sys_irp_en, bool track_en);
baseband_t* baseband_get_bb(uint32_t idx);
baseband_t* baseband_get_cur_bb();
uint8_t baseband_get_cur_idx();
uint8_t baseband_get_cur_frame_type();
void baseband_set_cur_frame_type(uint8_t ft);
baseband_t* baseband_get_rtl_frame_type();
void baseband_frame_type_reset();
baseband_t* baseband_frame_interleave_cfg(uint8_t frame_loop_pattern);
void baesband_frame_interleave_cnt_clr();
baseband_t* baseband_frame_interleave_recfg();
void baesband_frame_interleave_strategy_set(uint8_t strategy, uint32_t sw_num, uint8_t sel_0, uint8_t sel_1);
void baesband_frame_interleave_strategy_return();
bool bit_parse(uint32_t num, uint16_t bit_mux[]);
uint16_t get_ref_tx_antenna(uint32_t patten[]);
bool get_tdm_tx_antenna_in_chirp(uint32_t patten[], uint8_t chip_idx, uint32_t chirp_tx_mux[]);
int8_t bpm_patten_check(uint16_t bit_mux[], uint8_t chip_idx, uint16_t refer_phase, uint8_t pat_num);
int8_t get_bpm_tx_antenna_in_chirp(uint32_t patten[], uint8_t chip_idx, uint32_t chirp_tx_mux[]);
void baseband_hil_input_enable(baseband_t *bb);
void baseband_hil_input_disable(baseband_t *bb);
void baseband_hil_dump_enable(baseband_t *bb);
void baseband_hil_dump_done(baseband_t *bb);

#ifdef CHIP_CASCADE
void baseband_cascade_sync_handler();
void baseband_cascade_sync_wait(baseband_hw_t *bb_hw);
void baseband_cascade_sync_init();
void baseband_cascade_handshake();
#endif

#define CONTAINER_OF(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#ifdef CHIP_ALPS_A
#define DIV_RATIO(val, F_SD) ((val) * 1e3 / 8 / (F_SD) - 16) * (1L<<24)
#define INV_DIV_RATIO(val, F_SD) (1.0 * (val) / (1L<<24) + 16) * 8 * (F_SD) / 1000
#define FREQ_SYNTH_SD_RATE 400 /* in MHz */
#elif CHIP_ALPS_B
#define DIV_RATIO(val, F_SD) ((val) * 1e3 / 8 / (F_SD) - 16) * (1L<<28)
#define INV_DIV_RATIO(val, F_SD) (1.0 * (val) / (1L<<28) + 16) * 8 * (F_SD) / 1000
#define FREQ_SYNTH_SD_RATE FMCW_SDM_FREQ /* in MHz */
#elif CHIP_ALPS_MP
#define DIV_RATIO(val, F_SD) ((val) * 1e3 / 8 / (F_SD) - 16) * (1L<<28)
#define INV_DIV_RATIO(val, F_SD) (1.0 * (val) / (1L<<28) + 16) * 8 * (F_SD) / 1000
#define FREQ_SYNTH_SD_RATE FMCW_SDM_FREQ /* in MHz */
#endif


#endif
