#ifndef CALTERAH_COMMON_VEL_DEAMB_VEL_DEAMB_MF_H
#define CALTERAH_COMMON_VEL_DEAMB_VEL_DEAMB_MF_H

#include "embARC_toolchain.h"
#include "clkgen.h"
#include "sensor_config.h"
#include "baseband_task.h"
#include "baseband.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "arc_exception.h"
#include "alps_hardware.h"
#include "embARC_debug.h"
#include <stdlib.h>
#include "baseband_alps_FM_reg.h"
#include "baseband_hw.h"
#include <math.h>
#include "track.h"
#include "baseband_dpc.h"
#include "calterah_complex.h"
#include "calterah_math_funcs.h"

#define BB_WRITE_REG(bb_hw, RN, val) baseband_write_reg(bb_hw, BB_REG_##RN, val)
#define BB_READ_REG(bb_hw, RN) baseband_read_reg(bb_hw, BB_REG_##RN)

#define CUSTOM_VEL_DEAMB_MF false // To be compatible with customer implemented multi-frame velocity de-ambiguity

/* Enable velocity de-ambiguity using multiple frame(MF) method,
 * currently only 2 frame types supported.
 * To enable velocity de-ambiguity using multiple frame, set VEL_DEAMB_MF_EN to true.
 */
#define VEL_DEAMB_MF_EN (false && (NUM_FRAME_TYPE==2))
// Enable only to read information of successfully paired targets
#define TRACK_READ_PAIR_ONLY  true

#define  PRINT_EN  false // enable print debug message

#if PRINT_EN
#define OPTION_PRINT EMBARC_PRINTF
#else
#define OPTION_PRINT NO_PRINTF
#endif
void NO_PRINTF(const char *format,...);

void bb_dpc_pre(baseband_t * bb);
void bb_dpc_post_irq(baseband_t * bb);
void bb_dpc_config(baseband_data_proc_t * bb_dpc);

static void get_fft_vec(baseband_hw_t * bb_hw, uint32_t * fft_vec,
                int rng_index, int vel_index);
static void FFT_mem_buf2rlt(baseband_hw_t *bb_hw);
float corr_coe_cal(uint32_t * fft_mem1, uint32_t * fft_mem2, uint8_t len);

#define MAX_WRAP_NUM  3
void search_vel_wrap_num(uint32_t v_ind1, uint32_t v_nfft1, uint32_t Tc1,
                uint32_t v_ind2, uint32_t v_nfft2, uint32_t Tc2, signed char * wrp_num1,
                signed char * wrp_num2);

void frame_type_params_update(baseband_t *bb);
/* Thresholds of various object information types for pairing */
#define RNG_DIFF_THRES 1.5  // unit: m
#define SNR_DIFF_THRES 8 // unit: dB
#define VEL_DIFF_THRES 0.7  // unit: m/s FIXME: refer to vel_delta 25-27us = 0.27-0.3m/s
#define CORR_COE_THRES 0.6  // no unit
void obj_pair(baseband_t *bb);

/*------------ Structure pointer chain based pairing declaration (begin) -------------*/
#define MAX_PAIR_CANDI      3  // Constraint of chain length

/* The main object information is obtained in type 1 frame.
 * The candidate object information is obtained in type 0 frame.
 */
struct pair_candi_obj_info {
        unsigned char candi_obj_index;
        signed char candi_vel_wrap_num;
        unsigned char weighted_diff_exp;
        struct pair_candi_obj_info * next;
};

typedef struct pair_candi_obj_info pair_candi_obj_info_t;

typedef struct {
        unsigned char candi_obj_num;
        bool match_flag;
        pair_candi_obj_info_t * head;
        pair_candi_obj_info_t * tail;
} pair_main_obj_t;

struct pair_main_obj_info {
        unsigned char main_obj_index;
        signed char main_vel_wrap_num;
        unsigned char weighted_diff_exp;
        struct pair_main_obj_info * next;
};

typedef struct pair_main_obj_info pair_main_obj_info_t;

typedef struct {
        unsigned char main_obj_num;
        bool match_flag;
        pair_main_obj_info_t * head;
        pair_main_obj_info_t * tail;
} pair_candi_obj_t;

void insert_main_obj_info_node(pair_candi_obj_t *frame_type_0_obj, uint8_t candi_obj_ind, pair_main_obj_info_t * main_obj);
void insert_candi_obj_info_node(pair_main_obj_t *frame_type_1_obj, uint8_t main_obj_ind, pair_candi_obj_info_t * candi_obj);

bool recur_pair_candi_obj(pair_main_obj_t *frame_type_1_obj,
                pair_candi_obj_t *frame_type_0_obj, uint8_t candi_obj_ind);
bool recur_pair_main_obj(pair_main_obj_t *frame_type_1_obj,
                pair_candi_obj_t *frame_type_0_obj, uint8_t main_obj_ind);

bool Is_main_obj_match(unsigned char obj_ind);
bool Is_candi_obj_match(unsigned char obj_ind);
signed char main_obj_wrap_num(unsigned char obj_ind);

void print_candi_obj_info(uint8_t candi_obj_ind);
void print_main_obj_info(uint8_t main_obj_ind);

/*------------ Structure pointer chain based pairing declaration (end) -------------*/

#endif
