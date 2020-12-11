/*
    Header file for Extended KF. This is where you put algorithm specific definitions.
    The only interface between this model and tracking is "install_ekf_track", which will
    hook up handlers, data with tracking module properly.
 */

#ifndef EKF_TRACK_H
#define EKF_TRACK_H

#define TRACK_ALWAYS_CALCUL_A    1
#define TRACK_ALWAYS_CALCUL_Q    1

#define TRACK_EKF_ACC            0.05

/* #define TRACK_ADAPTIVE_CST */

#if TRK_CONF_3D
#define MEASURE_ELEM_NUM  4   /* r v a a_elv */
#define STATE_ELEM_NUM    9   /* rx ry rz vx vy vz ax ay az */
#define DIM_NUM           3
#else
#define MEASURE_ELEM_NUM  3   /* r v a */
#define STATE_ELEM_NUM    6   /* rx ry vx vy ax ay */
#define DIM_NUM           2

#endif

#ifdef TRACK_ADAPTIVE_CST
#define TRACK_CST_THR_RNG        9.0f  /* Unitless */
#define TRACK_CST_THR_VEL        9.0f  /* Unitless */
#define TRACK_CST_THR_ANG        9.0f  /* Unitless */
#if TRK_CONF_3D
#define TRACK_CST_THR_ANG_ELV    9.0f  /* Unitless */
#endif
#else
#define TRACK_CST_THR_RNG        10.0 /* Unit in BIN*/
#define TRACK_CST_THR_VEL        20.0 /* Unit in BIN*/
#define TRACK_CST_THR_ANG        15.0 /* Unit in BIN*/
#if TRK_CONF_3D
#define TRACK_CST_THR_ANG_ELV    15.0 /* Unit in BIN*/
#endif
#endif

#define RNG_NOISE_STD            1    /* Unit in BIN*/
#define VEL_NOISE_STD            1    /* Unit in BIN*/
#define ANG_NOISE_STD            12   /* Unit in BIN*/
#if TRK_CONF_3D
#define ANG_ELV_NOISE_STD        12   /* Unit in BIN*/
#endif

#define TRACK_DIS_THR_VEL        10.0  /* Unit in BIN */

#define TRACK_IDX_NUL            -1

#define TRACK_CST_MAX_2D        (TRACK_CST_THR_RNG * TRACK_CST_THR_RNG \
                               + TRACK_CST_THR_VEL * TRACK_CST_THR_VEL \
                               + TRACK_CST_THR_ANG * TRACK_CST_THR_ANG)
#if TRK_CONF_3D                                
#define TRACK_CST_MAX (TRACK_CST_MAX_2D + TRACK_CST_THR_ANG_ELV * TRACK_CST_THR_ANG_ELV)
#else
#define TRACK_CST_MAX TRACK_CST_MAX_2D
#endif

/* state */
typedef struct {
        track_float_t rng_x;    /* range along x axes        */
        track_float_t rng_y;    /* range along y axes        */
#if TRK_CONF_3D
        track_float_t rng_z;    /* range along z axes        */
#endif
        track_float_t vel_x;    /* velocity along x axes     */
        track_float_t vel_y;    /* velocity along y axes     */
#if TRK_CONF_3D
        track_float_t vel_z;    /* velocity along z axes     */
#endif
        track_float_t acc_x;    /* acceleration along x axes */
        track_float_t acc_y;    /* acceleration along y axes */
#if TRK_CONF_3D
        track_float_t acc_z;    /* acceleration along z axes */
#endif
} track_state_t;

/* tracker type */
typedef enum {
        TRACK_TYP_NUL = 0,
        TRACK_TYP_PRE ,
        TRACK_TYP_VAL ,
} track_trk_type_t;

/* tracker */
typedef struct {
        track_trk_type_t type     ;
        int8_t           hit_time ;
        int8_t           miss_time;
        int16_t          idx_1    ;
        int16_t          idx_2    ;
        track_float_t    cst_1    ;
        track_float_t    cst_2    ;
        track_float_t    sigma_x  ;
        track_float_t    sigma_y  ;
#if TRK_CONF_3D 
        track_float_t    sigma_z  ;
#endif
        track_float_t    P[STATE_ELEM_NUM * STATE_ELEM_NUM];    /* could be reduced to 21 for 2D; 45 for 3D */
        track_state_t    x        ;
        track_measu_t    flt_z    ;
        track_measu_t    pre_z    ;
        track_float_t    meas_var_inv[MEASURE_ELEM_NUM];
} track_trk_t;

/* tracker package */
typedef struct {
        track_trk_t trk[TRACK_NUM_TRK];
        uint32_t    output_trk_number;
        bool          has_run;
        uint32_t      f_numb;
        track_float_t frame_int;

        uint16_t      track_type_thres_pre;
        uint16_t      track_type_thres_val;

        track_cdi_pkg_t       *raw_input;
        track_obj_output_t    *output_obj;
        track_header_output_t *output_hdr;
} track_trk_pkg_t;

#ifndef TRACK_TEST_MODE
void install_ekf_track(track_t *trk);
#endif

#endif
