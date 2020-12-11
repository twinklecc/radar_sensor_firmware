#include "track_common.h"
#ifdef TRACK_TEST_MODE
#include "stdio.h"
#else
#include "track.h"
#include "embARC_debug.h"
#endif
#include "ekf_track.h"
#include "radar_sys_params.h"
#include "string.h"


#ifdef TRACK_TEST_MODE
#define FUNC_DECOR
#else
#define FUNC_DECOR static
#endif

/*--- VARIABLE -----------------------*/
FUNC_DECOR track_float_t track_A[STATE_ELEM_NUM * STATE_ELEM_NUM]     = {0};
FUNC_DECOR track_float_t track_Q[STATE_ELEM_NUM * STATE_ELEM_NUM]     = {0};
FUNC_DECOR track_float_t track_R[MEASURE_ELEM_NUM * MEASURE_ELEM_NUM] = {0};
FUNC_DECOR track_float_t track_meas_var_inv[MEASURE_ELEM_NUM] = {0};

#if TRK_CONF_3D
FUNC_DECOR track_float_t track_init_P[STATE_ELEM_NUM * STATE_ELEM_NUM] = \
        {10,  0,  0,  0,  0,  0,  0,  0,  0,   \
          0, 10,  0,  0,  0,  0,  0,  0,  0,   \
          0,  0, 10,  0,  0,  0,  0,  0,  0,   \
          0,  0,  0, 10,  0,  0,  0,  0,  0,   \
          0,  0,  0,  0, 10,  0,  0,  0,  0,   \
          0,  0,  0,  0,  0, 10,  0,  0,  0,   \
          0,  0,  0,  0,  0,  0, 10,  0,  0,   \
          0,  0,  0,  0,  0,  0,  0, 10,  0,   \
          0,  0,  0,  0,  0,  0,  0,  0, 10};
#else
FUNC_DECOR track_float_t track_init_P[STATE_ELEM_NUM * STATE_ELEM_NUM] = \
        {10,  0,  0,  0,  0,  0,     \
          0, 10,  0,  0,  0,  0,     \
          0,  0, 10,  0,  0,  0,     \
          0,  0,  0, 10,  0,  0,     \
          0,  0,  0,  0, 10,  0,     \
          0,  0,  0,  0,  0, 10};
#endif

/* interfaces to track */
/* Please use same name for each algorithm */
FUNC_DECOR track_trk_pkg_t trk_internal;
FUNC_DECOR void func_track_init(void *data, void *ctx);
FUNC_DECOR void func_track_run(void *data, void *ctx);
FUNC_DECOR void func_track_stop(void *data, void *ctx);
FUNC_DECOR void func_track_post(void *data, void *ctx);
FUNC_DECOR void func_set_frame_int(void *data, track_float_t delta);
FUNC_DECOR void func_track_header_update(void *data, void *ctx);
FUNC_DECOR void func_track_obj_info_update(void *data, void *ctx, uint32_t i);
FUNC_DECOR bool func_has_run(void *data);

/* helpers */
/* pair candidate and tracker */
static void ekf_run_pair(track_trk_pkg_t* trk_pkg, radar_sys_params_t* sys_params);
static void ekf_run_update(track_trk_pkg_t* trk_pkg, radar_sys_params_t* sys_params);
static void ekf_run_calcul_A(track_trk_pkg_t* trk_pkg);
static void ekf_run_calcul_Q(track_trk_pkg_t* trk_pkg);
static void ekf_run_filter(track_trk_pkg_t* trk_pkg, radar_sys_params_t* sys_params);
void track_run_filter_core(track_trk_pkg_t* trk_pkg, track_float_t f_time, track_trk_t *trk, track_cdi_t *cdi);

#ifdef TRACK_TEST_MODE
void setup_track(track_obj_output_t *obj_out, track_header_output_t *hdr_out, track_cdi_pkg_t *raw_input)
{
        trk_internal.raw_input  = raw_input;
        trk_internal.output_obj = obj_out;
        trk_internal.output_hdr = hdr_out;
}
#else
void install_ekf_track(track_t *track)
{
        track_trk_pkg_t* trk_pkg = &trk_internal;

        trk_pkg->raw_input  = &track->cdi_pkg;
        trk_pkg->output_obj = &track->output_obj;
        trk_pkg->output_hdr = &track->output_hdr;
        track->trk_data     = trk_pkg;
        track->trk_init     = func_track_init;
        track->trk_pre      = NULL;
        track->trk_run      = func_track_run;
        track->trk_post     = func_track_post;
        track->trk_stop     = func_track_stop;
        track->trk_set_frame_int   = func_set_frame_int;
        track->trk_update_obj_info = func_track_obj_info_update;
        track->trk_update_header   = func_track_header_update;
        track->trk_has_run         = func_has_run;
}
#endif

FUNC_DECOR void func_track_run(void *data, void *ctx)
{
        radar_sys_params_t* sys_params = (radar_sys_params_t *)ctx;
        track_trk_pkg_t*    trk_pkg    = (track_trk_pkg_t*)data;

        trk_pkg->output_trk_number = 0;
        /* calculate A */
        if (TRACK_ALWAYS_CALCUL_A)
                ekf_run_calcul_A(trk_pkg);
        else if (trk_pkg->has_run == false)
                ekf_run_calcul_A(trk_pkg);

        /* calculate Q */
        if (TRACK_ALWAYS_CALCUL_Q)
                ekf_run_calcul_Q(trk_pkg);
        else if (trk_pkg->has_run == false)
                ekf_run_calcul_Q(trk_pkg);

        if(trk_pkg->has_run == false)
                trk_pkg->has_run = true;

        /* pair candidate and tracker */
        ekf_run_pair(trk_pkg, sys_params);
        /* update tracker type */
        ekf_run_update(trk_pkg, sys_params);
        /* filter calculation */
        ekf_run_filter(trk_pkg, sys_params);
}

FUNC_DECOR void func_track_init(void *data, void *ctx)
{
        radar_sys_params_t* sys_params = (radar_sys_params_t *)ctx;
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;

        trk_pkg->track_type_thres_pre = sys_params->trk_capt_delay * sys_params->trk_fps;
        trk_pkg->track_type_thres_val = sys_params->trk_drop_delay * sys_params->trk_fps;

        /* FIXME: init of R, only quantization error from baseband but no error from noisy environment*/
        track_R[0 * MEASURE_ELEM_NUM + 0] = sys_params->rng_delta    * sys_params->rng_delta * RNG_NOISE_STD * RNG_NOISE_STD;
        track_R[1 * MEASURE_ELEM_NUM + 1] = sys_params->vel_delta    * sys_params->vel_delta * VEL_NOISE_STD * VEL_NOISE_STD;
        track_R[2 * MEASURE_ELEM_NUM + 2] = sys_params->az_delta_deg * sys_params->az_delta_deg * ANG_NOISE_STD * ANG_NOISE_STD;
#if TRK_CONF_3D
        track_R[3 * MEASURE_ELEM_NUM + 3] = sys_params->ev_delta_deg * sys_params->ev_delta_deg * ANG_NOISE_STD * ANG_NOISE_STD;
#endif

        track_meas_var_inv[0] = (track_float_t)1.0 / track_R[0 * MEASURE_ELEM_NUM + 0];
        track_meas_var_inv[1] = (track_float_t)1.0 / track_R[1 * MEASURE_ELEM_NUM + 1];
        track_meas_var_inv[2] = (track_float_t)1.0 / track_R[2 * MEASURE_ELEM_NUM + 2];
#if TRK_CONF_3D
        track_meas_var_inv[3] = (track_float_t)1.0 / track_R[3 * MEASURE_ELEM_NUM + 3];
#endif

        /* clear trackers */
        for (int i = 0; i < TRACK_NUM_TRK; i++)
                trk_pkg->trk[i].type = TRACK_TYP_NUL;
        trk_pkg->has_run = false;
        trk_pkg->f_numb = 0;
}

FUNC_DECOR void func_track_stop(void *data, void *ctx)
{
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;
        trk_pkg->has_run = false;
        trk_pkg->f_numb = 0;
}

FUNC_DECOR void func_track_post(void *data, void *ctx)
{
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;
        trk_pkg->f_numb++;
}

FUNC_DECOR void func_set_frame_int(void *data, track_float_t delta)
{
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;
        trk_pkg->frame_int = delta;
}

FUNC_DECOR void func_track_header_update(void *data, void *ctx)
{
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;
        trk_pkg->output_hdr->frame_int = trk_pkg->frame_int;
        trk_pkg->output_hdr->frame_id  = trk_pkg->f_numb;
        trk_pkg->output_hdr->track_output_number = trk_pkg->output_trk_number;
}

FUNC_DECOR void func_track_obj_info_update(void *data, void *ctx, uint32_t i)
{
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;
        uint32_t tag_s;

        if (i < TRACK_NUM_TRK && trk_pkg->trk[i].type != TRACK_TYP_NUL) {
                if(trk_pkg->trk[i].type == TRACK_TYP_PRE)
                        tag_s = 0;
                else
                        tag_s = (trk_pkg->trk[i].miss_time == 0)? 1 : 2;
                trk_pkg->output_obj->track_level = tag_s;
                track_float_t tmpS = trk_pkg->trk[i].flt_z.sig;
                track_float_t tmpN = trk_pkg->trk[i].flt_z.noi;
                trk_pkg->output_obj->SNR = 10*TRACK_LOG10(tmpS/tmpN);
                trk_pkg->output_obj->rng = trk_pkg->trk[i].flt_z.rng;
                trk_pkg->output_obj->vel = trk_pkg->trk[i].flt_z.vel;
                trk_pkg->output_obj->ang = trk_pkg->trk[i].flt_z.ang;
#if TRK_CONF_3D
                trk_pkg->output_obj->ang_elv = trk_pkg->trk[i].flt_z.ang_elv;
#endif
                trk_pkg->output_obj->output = true;
        } else
                trk_pkg->output_obj->output = false;
}

FUNC_DECOR bool func_has_run(void *data)
{
        track_trk_pkg_t* trk_pkg = (track_trk_pkg_t*)data;
        return trk_pkg->has_run;
}

/* helpers */
static void ekf_run_pair(track_trk_pkg_t* trk_pkg, radar_sys_params_t* sys_params)
{
        /* variable */
        uint16_t      i, j;
        track_float_t cur_cst;
        track_float_t rng_cst;
        track_float_t vel_cst;
        track_float_t ang_cst;
#if TRK_CONF_3D
        track_float_t ang_elv_cst;
#endif
        int16_t       cdi_idx_tmp_1;
        int16_t       cdi_idx_tmp_2;

        /* clear idx */
        for (i = 0; i < TRACK_NUM_CDI; i++) {
                trk_pkg->raw_input->cdi[i].index = TRACK_IDX_NUL;
        }

        for (i = 0; i < TRACK_NUM_TRK; i++) {
                trk_pkg->trk[i].idx_1 = TRACK_IDX_NUL;
                trk_pkg->trk[i].idx_2 = TRACK_IDX_NUL;
                trk_pkg->trk[i].cst_1 = (track_float_t)TRACK_CST_MAX;
                trk_pkg->trk[i].cst_2 = (track_float_t)TRACK_CST_MAX;
        }

        /* searh all trackers */
        for (i = 0; i < TRACK_NUM_TRK; i++) {
                /* it should be candidate or in track type */
                if (trk_pkg->trk[i].type != TRACK_TYP_NUL) {
                        /* search all candidates */
                        for (j = 0; j < trk_pkg->raw_input->cdi_number; j++) {
#ifdef TRACK_ADAPTIVE_CST
                                track_float_t tmp = trk_pkg->trk[i].pre_z.rng - trk_pkg->raw_input->cdi[j].raw_z.rng;
                                rng_cst = tmp * tmp * trk_pkg->trk[i].meas_var_inv[0];
                                tmp = trk_pkg->trk[i].pre_z.vel - trk_pkg->raw_input->cdi[j].raw_z.vel;
                                vel_cst = tmp * tmp * trk_pkg->trk[i].meas_var_inv[1];
                                tmp = trk_pkg->trk[i].pre_z.ang - trk_pkg->raw_input->cdi[j].raw_z.ang;
                                ang_cst = tmp * tmp * trk_pkg->trk[i].meas_var_inv[2];
#if TRK_CONF_3D
                                tmp = trk_pkg->trk[i].pre_z.ang_elv - trk_pkg->raw_input->cdi[j].raw_z.ang_elv;
                                ang_elv_cst = tmp * tmp * trk_pkg->trk[i].meas_var_inv[3];
#endif /* TRK_CONF_3D */
#else
                                /* TODO: use index or true data */
                                rng_cst = TRACK_FABS(trk_pkg->trk[i].pre_z.rng - trk_pkg->raw_input->cdi[j].raw_z.rng) / sys_params->rng_delta;
                                /* TODO: use index or true data */
                                vel_cst = TRACK_FABS(trk_pkg->trk[i].pre_z.vel - trk_pkg->raw_input->cdi[j].raw_z.vel) / sys_params->vel_delta;
                                /* TODO: use index or true data */
                                ang_cst = TRACK_FABS(trk_pkg->trk[i].pre_z.ang - trk_pkg->raw_input->cdi[j].raw_z.ang) / sys_params->az_delta_deg;
#if TRK_CONF_3D
                                ang_elv_cst = TRACK_FABS(trk_pkg->trk[i].pre_z.ang_elv - trk_pkg->raw_input->cdi[j].raw_z.ang_elv) / sys_params->ev_delta_deg;
#endif /* TRK_CONF_3D */
#endif /* TRACK_ADAPTIVE_CST */
                                /* test if within threshold */
                                if ((rng_cst < (track_float_t)TRACK_CST_THR_RNG) &&
                                    (vel_cst < (track_float_t)TRACK_CST_THR_VEL) &&
                                    (ang_cst < (track_float_t)TRACK_CST_THR_ANG)
#if TRK_CONF_3D
                                     && (ang_elv_cst < (track_float_t)TRACK_CST_THR_ANG_ELV)
#endif
                                   ) {
#ifdef TRACK_ADAPTIVE_CST
                                        cur_cst = rng_cst + vel_cst + ang_cst;
#if TRK_CONF_3D
                                        cur_cst += ang_elv_cst;
#endif
#else
                                        cur_cst = rng_cst * rng_cst
                                                + vel_cst * vel_cst
                                                + ang_cst * ang_cst;
#if TRK_CONF_3D
                                        cur_cst += ang_elv_cst * ang_elv_cst;
#endif
#endif /* TRACK_ADAPTIVE_CST */
                                        /* when cost is smaller than cst_1 */
                                        if (cur_cst < trk_pkg->trk[i].cst_1) {
                                                /* update cst */
                                                trk_pkg->trk[i].cst_2 = trk_pkg->trk[i].cst_1;
                                                trk_pkg->trk[i].cst_1 = cur_cst;
                                                /* update idx */
                                                trk_pkg->trk[i].idx_2 = trk_pkg->trk[i].idx_1;
                                                trk_pkg->trk[i].idx_1 = j;
                                        }
                                        /* when cost is smaller than cst_2 */
                                        else if (cur_cst < trk_pkg->trk[i].cst_2) {
                                                /* update new cst */
                                                trk_pkg->trk[i].cst_2 = cur_cst;
                                                /* update new idx */
                                                trk_pkg->trk[i].idx_2 = j;
                                        }
                                }
                        }
                        /* update candidate index if paired */
                        if (trk_pkg->trk[i].idx_1 != TRACK_IDX_NUL) {
                                /* get candidate index */
                                cdi_idx_tmp_1 = trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_1].index;
                                /* if it hasn't been paired */
                                if (cdi_idx_tmp_1 == TRACK_IDX_NUL) {
                                        /* set candidate index to current tracker */
                                        trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_1].index = i;
                                }
                                /* if it has been paired */
                                else {
                                        /* if current pair has a smaller cost */
                                        if (trk_pkg->trk[i].cst_1 < trk_pkg->trk[cdi_idx_tmp_1].cst_1 ) {
                                                /* build new pair */
                                                trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_1].index = i;
                                                /* if original tracker has no second pair */
                                                if (trk_pkg->trk[cdi_idx_tmp_1].idx_2 == TRACK_IDX_NUL) {
                                                        /* set it to null */
                                                        trk_pkg->trk[cdi_idx_tmp_1].idx_1 = TRACK_IDX_NUL;
                                                }
                                                /* if original tracker has a second pair */
                                                else {
                                                        /* get the candidate index */
                                                        cdi_idx_tmp_2 = trk_pkg->raw_input->cdi[trk_pkg->trk[cdi_idx_tmp_1].idx_2].index;
                                                        /* if it hasn't been paired */
                                                        if (cdi_idx_tmp_2 == TRACK_IDX_NUL) {
                                                                /* build a new pair */
                                                                trk_pkg->raw_input->cdi[trk_pkg->trk[cdi_idx_tmp_1].idx_2].index = cdi_idx_tmp_1;
                                                                trk_pkg->trk[cdi_idx_tmp_1].idx_1 = trk_pkg->trk[cdi_idx_tmp_1].idx_2;
                                                                trk_pkg->trk[cdi_idx_tmp_1].idx_2 = TRACK_IDX_NUL;
                                                                trk_pkg->trk[cdi_idx_tmp_1].cst_1 = trk_pkg->trk[cdi_idx_tmp_1].cst_2;
                                                        }
                                                        /* if it has been paired */
                                                        else {
                                                                /* set it to nul */
                                                                trk_pkg->trk[cdi_idx_tmp_1].idx_1 = TRACK_IDX_NUL;
                                                        }
                                                }
                                        }
                                        /* if current pair has a larger cost */
                                        else {
                                                /* if no second pair exist */
                                                if (trk_pkg->trk[i].idx_2 == TRACK_IDX_NUL) {
                                                        /* set it to null */
                                                        trk_pkg->trk[i].idx_1 = TRACK_IDX_NUL;
                                                }
                                                /* if second pair exist */
                                                else {
                                                        /* get candidate index */
                                                        cdi_idx_tmp_2 = trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_2].index;
                                                        /* if it hasn't been paired */
                                                        if (cdi_idx_tmp_2 == TRACK_IDX_NUL) {
                                                                /* set candidate index to current tracker */
                                                                trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_2].index = i;
                                                                trk_pkg->trk[i].idx_1 = trk_pkg->trk[i].idx_2;
                                                                trk_pkg->trk[i].cst_1 = trk_pkg->trk[i].cst_2;
                                                                trk_pkg->trk[i].idx_2 = TRACK_IDX_NUL;
                                                        }
                                                        /* if it has been paired */
                                                        else {
                                                                /* set it to null */
                                                                trk_pkg->trk[i].idx_1 = TRACK_IDX_NUL;
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

static void ekf_run_update(track_trk_pkg_t* trk_pkg, radar_sys_params_t* sys_params)
{
        /* variable */
        uint16_t i, j;

        /* update trackers with pre-trk type */
        for (i = 0; i < TRACK_NUM_TRK; i++) {
                /* pick trackers with pre-trk type */
                if (trk_pkg->trk[i].type == TRACK_TYP_PRE) {
                        if (trk_pkg->trk[i].idx_1 != TRACK_IDX_NUL) { /* if paired */
                                /* update hit time */
                                trk_pkg->trk[i].hit_time++;
                                /* if reach the boundary, promote it */
                                if (trk_pkg->trk[i].hit_time == trk_pkg->track_type_thres_pre) {
                                        /* promote to valid track type */
                                        trk_pkg->trk[i].type = TRACK_TYP_VAL;
                                        trk_pkg->trk[i].miss_time = 0;
                                        trk_pkg->trk[i].hit_time--;    /* TODO: is this needed? */
                                }
                        } else {  /* if not paired */
                                /* discard non-paired pre trackers */
                                trk_pkg->trk[i].type = TRACK_TYP_NUL;
                        }
                }
        }

        /* update trackers with nul-type */
        for (j = 0, i = 0; j < trk_pkg->raw_input->cdi_number; j++) {
                /* if paired */
                if (trk_pkg->raw_input->cdi[j].index == TRACK_IDX_NUL) {
                        /* find blank tracker */
                        while (trk_pkg->trk[i].type != TRACK_TYP_NUL) {
                                i++;
                                if (i >= TRACK_NUM_TRK)
                                        break;
                        }
                        /* if i is valid */
                        if (i < TRACK_NUM_TRK) {
                                /* promote candidate */
                                trk_pkg->trk[i].idx_1    = j;
                                trk_pkg->trk[i].type     = TRACK_TYP_PRE;
                                trk_pkg->trk[i].hit_time = 0;
                                trk_pkg->raw_input->cdi[j].index    = i;
                        } else
                                break;
                }
        }

        /* update tracker with val-trk type */
        for (i = 0; i < TRACK_NUM_TRK; i++) {
                /* pick trackers with val-trk type */
                if (trk_pkg->trk[i].type == TRACK_TYP_VAL) {
                        /* if not paired */
                        if (trk_pkg->trk[i].idx_1 == TRACK_IDX_NUL) {
                                /* update miss time */
                                trk_pkg->trk[i].miss_time++;
                                /* faster discard */
                                /* if (TRACK_FABS(trk_pkg->trk[i].x.vel_x) >= TRACK_DIS_THR_VEL) {    /\* TODO: use index or true data *\/ */
                                /*         trk_pkg->trk[i].miss_time += 2 ; */
                                /* } */
                                /* hard discard for too close or out-of-FOV objects */
                                if (trk_pkg->trk[i].flt_z.ang > sys_params->trk_fov_az_right ||
                                    trk_pkg->trk[i].flt_z.ang < sys_params->trk_fov_az_left ||
                                    trk_pkg->trk[i].flt_z.rng <= sys_params->trk_nf_thres)
                                        trk_pkg->trk[i].type = TRACK_TYP_NUL;
#if TRK_CONF_3D
                                if (trk_pkg->trk[i].flt_z.ang_elv > 60 || trk_pkg->trk[i].flt_z.ang_elv < -60)
                                        /* FIXME: we have to define something like "trk_fov_ev_left" and "trk_fov_ev_right" in sys_params */
                                        trk_pkg->trk[i].type = TRACK_TYP_NUL;
#endif
                                /* normal discard */
                                if (trk_pkg->trk[i].miss_time >= trk_pkg->trk[i].hit_time)
                                        trk_pkg->trk[i].type = TRACK_TYP_NUL;
                        }
                        /* if paired */
                        else {
                                /* reset miss_time */
                                trk_pkg->trk[i].miss_time = 0;
                                /* update hit time */
                                if (trk_pkg->trk[i].hit_time < trk_pkg->track_type_thres_val) {
                                        trk_pkg->trk[i].hit_time++;
                                }
                        }
                }
        } /* loop of i*/
}

/* filter calculation */
static void ekf_run_filter(track_trk_pkg_t* trk_pkg, radar_sys_params_t* sys_params)
{
        /* variable */
        uint16_t      i;
        track_float_t angle;
#if TRK_CONF_3D
        track_float_t angle_elevate;
#endif
        track_float_t rng_proj_xy;
        track_float_t vel_proj_xy;

        /* for-loop in trk */
        for (i = 0; i < TRACK_NUM_TRK; i++) {
                /* process according to type */
                switch (trk_pkg->trk[i].type) {
                        /* null type */
                        case TRACK_TYP_NUL :
                                break;
                        /* pre type */
                        case TRACK_TYP_PRE :
                                trk_pkg->output_trk_number++;
                                if (trk_pkg->trk[i].hit_time == 0) {
                                        /* filtered */
                                        trk_pkg->trk[i].flt_z = trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_1].raw_z;
                                        /* predicted */
                                        trk_pkg->trk[i].pre_z = trk_pkg->trk[i].flt_z;
                                        trk_pkg->trk[i].pre_z.rng = trk_pkg->trk[i].flt_z.rng + trk_pkg->trk[i].flt_z.vel * trk_pkg->frame_int;
                                        /* sigma */
                                        trk_pkg->trk[i].sigma_x = (track_float_t)1.0;
                                        trk_pkg->trk[i].sigma_y = (track_float_t)1.0;
#if TRK_CONF_3D
                                        trk_pkg->trk[i].sigma_z = (track_float_t)1.0;
#endif
                                        /* int_P */
                                        memcpy(trk_pkg->trk[i].P, track_init_P, STATE_ELEM_NUM * STATE_ELEM_NUM * sizeof(track_float_t));
                                        /* meas_var_int */
                                        memcpy(trk_pkg->trk[i].meas_var_inv, track_meas_var_inv, MEASURE_ELEM_NUM * sizeof(track_float_t));
                                        /* state */
#if TRK_CONF_3D
                                        angle         = trk_pkg->trk[i].flt_z.ang     * ANG2RAD;
                                        angle_elevate = trk_pkg->trk[i].flt_z.ang_elv * ANG2RAD;
                                        rng_proj_xy   = trk_pkg->trk[i].flt_z.rng * TRACK_COS(angle_elevate);
                                        vel_proj_xy   = trk_pkg->trk[i].flt_z.vel * TRACK_COS(angle_elevate);
#else
                                        angle         = trk_pkg->trk[i].pre_z.ang * ANG2RAD;
                                        rng_proj_xy   = trk_pkg->trk[i].pre_z.rng;
                                        vel_proj_xy   = trk_pkg->trk[i].pre_z.vel;
#endif
                                        trk_pkg->trk[i].x.rng_x = rng_proj_xy * TRACK_SIN(angle);
                                        trk_pkg->trk[i].x.rng_y = rng_proj_xy * TRACK_COS(angle);
                                        trk_pkg->trk[i].x.vel_x = vel_proj_xy * TRACK_SIN(angle);
                                        trk_pkg->trk[i].x.vel_y = vel_proj_xy * TRACK_COS(angle);
                                        trk_pkg->trk[i].x.acc_x = (track_float_t)0.0;
                                        trk_pkg->trk[i].x.acc_y = (track_float_t)0.0;
#if TRK_CONF_3D
                                        trk_pkg->trk[i].x.rng_z = trk_pkg->trk[i].flt_z.rng * TRACK_SIN(angle_elevate);
                                        trk_pkg->trk[i].x.vel_z = trk_pkg->trk[i].flt_z.vel * TRACK_SIN(angle_elevate);
                                        trk_pkg->trk[i].x.acc_z = (track_float_t)0.0;
#endif
                                } else {
                                        /* kalman_filter */
                                        track_run_filter_core(trk_pkg, trk_pkg->frame_int, &trk_pkg->trk[i], &trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_1]);
                                }
                                break;
                        /* val type */
                        case TRACK_TYP_VAL :
                                trk_pkg->output_trk_number++;
                                /* if paried */
                                if (trk_pkg->trk[i].idx_1 != TRACK_IDX_NUL) {
                                        /* filter with measurement */
                                        track_run_filter_core(trk_pkg, trk_pkg->frame_int, &trk_pkg->trk[i], &trk_pkg->raw_input->cdi[trk_pkg->trk[i].idx_1]);
                                } else {
                                        /* filter without measurement */
                                        track_run_filter_core(trk_pkg, trk_pkg->frame_int, &trk_pkg->trk[i], NULL);
                                }
                                break;
                }
        }
}

static void ekf_run_calcul_A(track_trk_pkg_t* trk_pkg)
{
        /* variable (to avoid numerical problem, they are declared as double */
        double a1;
        double a2;
        double a3;

        /* get ax */
        a1 = 1.0 / (TRACK_EKF_ACC * TRACK_EKF_ACC) * ((-1.0 + TRACK_EKF_ACC * (double)trk_pkg->frame_int) + exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int));
        a2 = 1.0 / TRACK_EKF_ACC * (1.0 - exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int));
        a3 = exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int);

        /*****************************************************************************************************************************************
        in case of 3d :
        [ 1, 0, 0, T, 0, 0, (exp(-T*alpha) + T*alpha - 1)/alpha^2,                                     0,                                     0]
        [ 0, 1, 0, 0, T, 0,                                     0, (exp(-T*alpha) + T*alpha - 1)/alpha^2,                                     0]
        [ 0, 0, 1, 0, 0, T,                                     0,                                     0, (exp(-T*alpha) + T*alpha - 1)/alpha^2]
        [ 0, 0, 0, 1, 0, 0,            -(exp(-T*alpha) - 1)/alpha,                                     0,                                     0]
        [ 0, 0, 0, 0, 1, 0,                                     0,            -(exp(-T*alpha) - 1)/alpha,                                     0]
        [ 0, 0, 0, 0, 0, 1,                                     0,                                     0,            -(exp(-T*alpha) - 1)/alpha]
        [ 0, 0, 0, 0, 0, 0,                         exp(-T*alpha),                                     0,                                     0]
        [ 0, 0, 0, 0, 0, 0,                                     0,                         exp(-T*alpha),                                     0]
        [ 0, 0, 0, 0, 0, 0,                                     0,                                     0,                         exp(-T*alpha)]

        in case of 2d :
        [ 1, 0, T, 0, (exp(-T*alpha) + T*alpha - 1)/alpha^2,                                     0]
        [ 0, 1, 0, T,                                     0, (exp(-T*alpha) + T*alpha - 1)/alpha^2]
        [ 0, 0, 1, 0,            -(exp(-T*alpha) - 1)/alpha,                                     0]
        [ 0, 0, 0, 1,                                     0,            -(exp(-T*alpha) - 1)/alpha]
        [ 0, 0, 0, 0,                         exp(-T*alpha),                                     0]
        [ 0, 0, 0, 0,                                     0,                         exp(-T*alpha)]
        *******************************************************************************************************************************************/

#if TRK_CONF_3D
        /* line 0 */
        track_A[0 * STATE_ELEM_NUM + 0] = (track_float_t)1.0;
        track_A[3 * STATE_ELEM_NUM + 0] = (track_float_t)trk_pkg->frame_int;
        track_A[6 * STATE_ELEM_NUM + 0] = (track_float_t)a1;

        /* line 1 */
        track_A[1 * STATE_ELEM_NUM + 1] = (track_float_t)1.0;
        track_A[4 * STATE_ELEM_NUM + 1] = (track_float_t)trk_pkg->frame_int;
        track_A[7 * STATE_ELEM_NUM + 1] = (track_float_t)a1;

        /* line 2 */
        track_A[2 * STATE_ELEM_NUM + 2] = (track_float_t)1.0;
        track_A[5 * STATE_ELEM_NUM + 2] = (track_float_t)trk_pkg->frame_int;
        track_A[8 * STATE_ELEM_NUM + 2] = (track_float_t)a1;

        /* line 3 */
        track_A[3 * STATE_ELEM_NUM + 3] = (track_float_t)1.0;
        track_A[6 * STATE_ELEM_NUM + 3] = (track_float_t)a2;

        /* line 4 */
        track_A[4 * STATE_ELEM_NUM + 4] = (track_float_t)1.0;
        track_A[7 * STATE_ELEM_NUM + 4] = (track_float_t)a2;

        /* line 5 */
        track_A[5 * STATE_ELEM_NUM + 5] = (track_float_t)1.0;
        track_A[8 * STATE_ELEM_NUM + 5] = (track_float_t)a2;

        /* line 6 */
        track_A[6 * STATE_ELEM_NUM + 6] = (track_float_t)a3;

        /* line 7 */
        track_A[7 * STATE_ELEM_NUM + 7] = (track_float_t)a3;

        /* line 8 */
        track_A[8 * STATE_ELEM_NUM + 8] = (track_float_t)a3;

#else
        /* line 0 */
        track_A[0 * STATE_ELEM_NUM + 0] = (track_float_t)1.0;
        track_A[2 * STATE_ELEM_NUM + 0] = (track_float_t)trk_pkg->frame_int;
        track_A[4 * STATE_ELEM_NUM + 0] = (track_float_t)a1;

        /* line 1 */
        track_A[1 * STATE_ELEM_NUM + 1] = (track_float_t)1.0;
        track_A[3 * STATE_ELEM_NUM + 1] = (track_float_t)trk_pkg->frame_int;
        track_A[5 * STATE_ELEM_NUM + 1] = (track_float_t)a1;

        /* line 2 */
        track_A[2 * STATE_ELEM_NUM + 2] = (track_float_t)1.0;
        track_A[4 * STATE_ELEM_NUM + 2] = (track_float_t)a2;

        /* line 3 */
        track_A[3 * STATE_ELEM_NUM + 3] = (track_float_t)1.0;
        track_A[5 * STATE_ELEM_NUM + 3] = (track_float_t)a2;

        /* line 4 */
        track_A[4 * STATE_ELEM_NUM + 4] = (track_float_t)a3;

        /* line 5 */
        track_A[5 * STATE_ELEM_NUM + 5] = (track_float_t)a3;

#endif
}

/* calculate Q */
static void ekf_run_calcul_Q(track_trk_pkg_t* trk_pkg)
{
        /* variable */
        double q11;
        double q12;
        double q13;
        double q22;
        double q23;
        double q33;

        /* get qxx */
        q11 = 1.0 / pow(TRACK_EKF_ACC, 4.0) * (
                + 1.0
                - exp(-2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                + 2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int
                + 2.0 * pow(TRACK_EKF_ACC, 3.0) * pow((double)trk_pkg->frame_int, 3.0)/3.0
                - 2.0 * TRACK_EKF_ACC * TRACK_EKF_ACC * (double)trk_pkg->frame_int * (double)trk_pkg->frame_int
                - 4.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int * exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                );
        q12 = 1.0 / pow(TRACK_EKF_ACC, 3.0) * (
                + exp(-2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                + 1.0
                - 2.0 * exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                + 2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int * exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                - 2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int
                + TRACK_EKF_ACC * TRACK_EKF_ACC * (double)trk_pkg->frame_int * (double)trk_pkg->frame_int
                );
        q13 = 1.0 / (TRACK_EKF_ACC * TRACK_EKF_ACC) * (
                1.0
                - exp(-2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                - 2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int * exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                );
        q22 = 1.0 / (TRACK_EKF_ACC * TRACK_EKF_ACC) * (
                + 4.0 * exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                - 3.0
                - exp(-2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                + 2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int
                );
        q23 = 1.0 / TRACK_EKF_ACC * (
                + exp(-2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                + 1.0
                - 2.0 * exp(-TRACK_EKF_ACC * (double)trk_pkg->frame_int)
                );
        q33 = 1.0 - exp(-2.0 * TRACK_EKF_ACC * (double)trk_pkg->frame_int);

#if TRK_CONF_3D
        /* line 0 rx */
        track_Q[0 * STATE_ELEM_NUM + 0] = (track_float_t)q11;
        track_Q[3 * STATE_ELEM_NUM + 0] = (track_float_t)q12;
        track_Q[6 * STATE_ELEM_NUM + 0] = (track_float_t)q13;

        /* line 1 ry */
        track_Q[1 * STATE_ELEM_NUM + 1] = (track_float_t)q11;
        track_Q[4 * STATE_ELEM_NUM + 1] = (track_float_t)q12;
        track_Q[7 * STATE_ELEM_NUM + 1] = (track_float_t)q13;

        /* line 2 rz */
        track_Q[2 * STATE_ELEM_NUM + 2] = (track_float_t)q11;
        track_Q[5 * STATE_ELEM_NUM + 2] = (track_float_t)q12;
        track_Q[8 * STATE_ELEM_NUM + 2] = (track_float_t)q13;

        /* line 3 vx */
        track_Q[0 * STATE_ELEM_NUM + 3] = (track_float_t)q12;
        track_Q[3 * STATE_ELEM_NUM + 3] = (track_float_t)q22;
        track_Q[6 * STATE_ELEM_NUM + 3] = (track_float_t)q23;

        /* line 4 vy */
        track_Q[1 * STATE_ELEM_NUM + 4] = (track_float_t)q12;
        track_Q[4 * STATE_ELEM_NUM + 4] = (track_float_t)q22;
        track_Q[7 * STATE_ELEM_NUM + 4] = (track_float_t)q23;

        /* line 5 vz */
        track_Q[2 * STATE_ELEM_NUM + 5] = (track_float_t)q12;
        track_Q[5 * STATE_ELEM_NUM + 5] = (track_float_t)q22;
        track_Q[8 * STATE_ELEM_NUM + 5] = (track_float_t)q23;

        /* line 6 ax */
        track_Q[0 * STATE_ELEM_NUM + 6] = (track_float_t)q13;
        track_Q[3 * STATE_ELEM_NUM + 6] = (track_float_t)q23;
        track_Q[6 * STATE_ELEM_NUM + 6] = (track_float_t)q33;

        /* line 7 ay */
        track_Q[1 * STATE_ELEM_NUM + 7] = (track_float_t)q13;
        track_Q[4 * STATE_ELEM_NUM + 7] = (track_float_t)q23;
        track_Q[7 * STATE_ELEM_NUM + 7] = (track_float_t)q33;

        /* line 8 az */
        track_Q[2 * STATE_ELEM_NUM + 8] = (track_float_t)q13;
        track_Q[5 * STATE_ELEM_NUM + 8] = (track_float_t)q23;
        track_Q[8 * STATE_ELEM_NUM + 8] = (track_float_t)q33;
#else
        /* line 0 rx */
        track_Q[0 * STATE_ELEM_NUM + 0] = (track_float_t)q11;
        track_Q[2 * STATE_ELEM_NUM + 0] = (track_float_t)q12;
        track_Q[4 * STATE_ELEM_NUM + 0] = (track_float_t)q13;

        /* line 1 */
        track_Q[1 * STATE_ELEM_NUM + 1] = (track_float_t)q11;
        track_Q[3 * STATE_ELEM_NUM + 1] = (track_float_t)q12;
        track_Q[5 * STATE_ELEM_NUM + 1] = (track_float_t)q13;

        /* line 2 */
        track_Q[0 * STATE_ELEM_NUM + 2] = (track_float_t)q12;
        track_Q[2 * STATE_ELEM_NUM + 2] = (track_float_t)q22;
        track_Q[4 * STATE_ELEM_NUM + 2] = (track_float_t)q23;

        /* line 3 */
        track_Q[1 * STATE_ELEM_NUM + 3] = (track_float_t)q12;
        track_Q[3 * STATE_ELEM_NUM + 3] = (track_float_t)q22;
        track_Q[5 * STATE_ELEM_NUM + 3] = (track_float_t)q23;

        /* line 4 */
        track_Q[0 * STATE_ELEM_NUM + 4] = (track_float_t)q13;
        track_Q[2 * STATE_ELEM_NUM + 4] = (track_float_t)q23;
        track_Q[4 * STATE_ELEM_NUM + 4] = (track_float_t)q33;

        /* line 5 */
        track_Q[1 * STATE_ELEM_NUM + 5] = (track_float_t)q13;
        track_Q[3 * STATE_ELEM_NUM + 5] = (track_float_t)q23;
        track_Q[5 * STATE_ELEM_NUM + 5] = (track_float_t)q33;
#endif
}

static inline void track_inline_x2z(track_measu_t *z, track_state_t *x)
{
#if TRK_CONF_3D
        /* r = (rx^2+ry^2+rz^2)^0.5 */
        z->rng = TRACK_SQRT(x->rng_x * x->rng_x + x->rng_y * x->rng_y + x->rng_z  * x->rng_z);
        /* v = vx*rx/r + vy*ry/r + vz*rz/r */
        z->vel = (x->vel_x * x->rng_x + x->vel_y * x->rng_y + x->vel_z * x->rng_z) / z->rng;
        /* a = atan(rx/ry) */
        z->ang = TRACK_ATAN(x->rng_x / x->rng_y) * (track_float_t)RAD2ANG;
        /* a_elv = atan(z / sqrt(rx^2+ry^2)) */
        z->ang_elv = TRACK_ATAN(x->rng_z / TRACK_SQRT(x->rng_x * x->rng_x + x->rng_y * x->rng_y)) * (track_float_t)RAD2ANG;
#else
        /* r = (rx^2+ry^2)^0.5 */
        z->rng = TRACK_SQRT(x->rng_x * x->rng_x + x->rng_y * x->rng_y);
        /* v = vx*rx/r + vy*ry/r */
        z->vel = x->vel_x * x->rng_x / z->rng + x->vel_y * x->rng_y / z->rng;
        /* a = atan(rx/ry) */
        z->ang = TRACK_ATAN(x->rng_x / x->rng_y) * (track_float_t)RAD2ANG;
#endif
}

/* Z = A*B, Z: i*j, A:i*k, B:k*j */
static inline void track_inline_MA_mul_MB(track_float_t *Z, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        Z[idx_i + i * idx_j] = (track_float_t)0.0;
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] += A[idx_i + i * idx_k] * B[idx_k + k * idx_j];
                        }
                }
        }
}

/* Z = A*B', Z: i*j, A:i*k, B':k*j */
static inline void track_inline_MA_mul_MB_T(track_float_t *Z, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        Z[idx_i + i * idx_j] = (track_float_t)0.0;
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] += A[idx_i + i * idx_k] * B[idx_j + j * idx_k];
                        }
                }
        }
}

/* Z = Z+A*B, Z: i*j, A:i*k, B:k*j */
static inline void track_inline_Z_add_MA_mul_MB(track_float_t *Z, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] += A[idx_i + i * idx_k] * B[idx_k + k * idx_j];
                        }
                }
        }
}

/* Z = Z+A*B', Z: i*j, A:i*k, B':k*j */
static inline void track_inline_Z_add_MA_mul_MB_T(track_float_t *Z, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] += A[idx_i + i * idx_k] * B[idx_j + j * idx_k];
                        }
                }
        }
}

/* Z = C+A*B, Z: i*j, A:i*k, B:k*j */
static inline void track_inline_C_add_MA_mul_MB(track_float_t *Z, track_float_t *C, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        Z[idx_i + i * idx_j] = C[idx_i + i * idx_j];
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] += A[idx_i + i * idx_k] * B[idx_k + k * idx_j];
                        }
                }
        }
}

/* Z = C+A*B', Z: i*j, A:i*k, B':k*j */
static inline void track_inline_C_add_MA_mul_MB_T(track_float_t *Z, track_float_t *C, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        Z[idx_i + i * idx_j] = C[idx_i + i * idx_j];
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] += A[idx_i + i * idx_k] * B[idx_j + j * idx_k];
                        }
                }
        }
}

/* Z = Z-A*B, Z: i*j, A:i*k, B:k*j */
static inline void track_inline_Z_sub_MA_mul_MB(track_float_t *Z, track_float_t *A, track_float_t *B, int i, int j, int k)
{
        uint8_t idx_i;
        uint8_t idx_j;
        uint8_t idx_k;
        for (idx_i = 0; idx_i < i; idx_i++) {
                for (idx_j = 0; idx_j < j; idx_j++) {
                        for (idx_k = 0; idx_k < k; idx_k++) {
                                Z[idx_i + i * idx_j] -= A[idx_i + i * idx_k] * B[idx_k + k * idx_j];
                        }
                }
        }
}

#if TRK_CONF_3D
/* Z = A/B, Z: 9*4, A:9*4, B:4*4 */

static void m4x4_inv(track_float_t *M)
{
        track_float_t a00, a01, a02, a03;
        track_float_t a10, a11, a12, a13;
        track_float_t a20, a21, a22, a23;
        track_float_t a30, a31, a32, a33;

        track_float_t *MINV = M;

        a00 = M[0 * 4 + 0];
        a01 = M[1 * 4 + 0];
        a02 = M[2 * 4 + 0];
        a03 = M[3 * 4 + 0];

        a10 = M[0 * 4 + 1];
        a11 = M[1 * 4 + 1];
        a12 = M[2 * 4 + 1];
        a13 = M[3 * 4 + 1];

        a20 = M[0 * 4 + 2];
        a21 = M[1 * 4 + 2];
        a22 = M[2 * 4 + 2];
        a23 = M[3 * 4 + 2];

        a30 = M[0 * 4 + 3];
        a31 = M[1 * 4 + 3];
        a32 = M[2 * 4 + 3];
        a33 = M[3 * 4 + 3];

        track_float_t det = a00*a11*a22*a33 - a00*a11*a23*a32 - a00*a12*a21*a33 + a00*a12*a23*a31 + a00*a13*a21*a32 - a00*a13*a22*a31 - a01*a10*a22*a33 + a01*a10*a23*a32 + a01*a12*a20*a33 - a01*a12*a23*a30 - a01*a13*a20*a32 + a01*a13*a22*a30 + a02*a10*a21*a33 - a02*a10*a23*a31 - a02*a11*a20*a33 + a02*a11*a23*a30 + a02*a13*a20*a31 - a02*a13*a21*a30 - a03*a10*a21*a32 + a03*a10*a22*a31 + a03*a11*a20*a32 - a03*a11*a22*a30 - a03*a12*a20*a31 + a03*a12*a21*a30;

        if (TRACK_FABS(det) < 0.0000001)
                det = 0.0000001;

        MINV[0 * 4 + 0] =  (a11*a22*a33 - a11*a23*a32 - a12*a21*a33 + a12*a23*a31 + a13*a21*a32 - a13*a22*a31) / (det);
        MINV[1 * 4 + 0] = -(a01*a22*a33 - a01*a23*a32 - a02*a21*a33 + a02*a23*a31 + a03*a21*a32 - a03*a22*a31) / (det);
        MINV[2 * 4 + 0] =  (a01*a12*a33 - a01*a13*a32 - a02*a11*a33 + a02*a13*a31 + a03*a11*a32 - a03*a12*a31) / (det);
        MINV[3 * 4 + 0] = -(a01*a12*a23 - a01*a13*a22 - a02*a11*a23 + a02*a13*a21 + a03*a11*a22 - a03*a12*a21) / (det);

        MINV[0 * 4 + 1] = -(a10*a22*a33 - a10*a23*a32 - a12*a20*a33 + a12*a23*a30 + a13*a20*a32 - a13*a22*a30) / (det);
        MINV[1 * 4 + 1] =  (a00*a22*a33 - a00*a23*a32 - a02*a20*a33 + a02*a23*a30 + a03*a20*a32 - a03*a22*a30) / (det);
        MINV[2 * 4 + 1] = -(a00*a12*a33 - a00*a13*a32 - a02*a10*a33 + a02*a13*a30 + a03*a10*a32 - a03*a12*a30) / (det);
        MINV[3 * 4 + 1] =  (a00*a12*a23 - a00*a13*a22 - a02*a10*a23 + a02*a13*a20 + a03*a10*a22 - a03*a12*a20) / (det);

        MINV[0 * 4 + 2] =  (a10*a21*a33 - a10*a23*a31 - a11*a20*a33 + a11*a23*a30 + a13*a20*a31 - a13*a21*a30) / (det);
        MINV[1 * 4 + 2] = -(a00*a21*a33 - a00*a23*a31 - a01*a20*a33 + a01*a23*a30 + a03*a20*a31 - a03*a21*a30) / (det);
        MINV[2 * 4 + 2] =  (a00*a11*a33 - a00*a13*a31 - a01*a10*a33 + a01*a13*a30 + a03*a10*a31 - a03*a11*a30) / (det);
        MINV[3 * 4 + 2] = -(a00*a11*a23 - a00*a13*a21 - a01*a10*a23 + a01*a13*a20 + a03*a10*a21 - a03*a11*a20) / (det);

        MINV[0 * 4 + 3] = -(a10*a21*a32 - a10*a22*a31 - a11*a20*a32 + a11*a22*a30 + a12*a20*a31 - a12*a21*a30) / (det);
        MINV[1 * 4 + 3] =  (a00*a21*a32 - a00*a22*a31 - a01*a20*a32 + a01*a22*a30 + a02*a20*a31 - a02*a21*a30) / (det);
        MINV[2 * 4 + 3] = -(a00*a11*a32 - a00*a12*a31 - a01*a10*a32 + a01*a12*a30 + a02*a10*a31 - a02*a11*a30) / (det);
        MINV[3 * 4 + 3] =  (a00*a11*a22 - a00*a12*a21 - a01*a10*a22 + a01*a12*a20 + a02*a10*a21 - a02*a11*a20) / (det);
}

void track_inline_MA_9x4_div_MB_4x4(track_float_t *Z, track_float_t *A, track_float_t *B)
{
        m4x4_inv(B);
        track_inline_MA_mul_MB(Z, A, B, 9, 4, 4);
}
#endif

/* Z = A/B, Z: 6*3, A:6*3, B:3*3 */
void track_inline_MA_6x3_div_MB_3x3(track_float_t *Z, track_float_t *A, track_float_t *B)
{
        uint8_t       idx_i;
        uint8_t       idx_j;
        uint8_t       idx_k;
        uint8_t       idx_l;
        track_float_t dat_a;
        track_float_t dat_b;
        track_float_t dat_c;

        idx_i = 0;
        idx_j = 1;
        idx_k = 2;

        /* 1st reorder */
        dat_a = TRACK_FABS(B[0]);
        dat_b = TRACK_FABS(B[1]);
        dat_c = TRACK_FABS(B[2]);

        if (dat_b > dat_a) {
                dat_a = dat_b;
                idx_i = 1;
                idx_j = 0;
        }
        if (dat_c > dat_a) {
                idx_i = 2;
                idx_j = 1;
                idx_k = 0;
        }

        B[    idx_j] /= B[idx_i];
        B[    idx_k] /= B[idx_i];
        B[3 + idx_j] -= B[idx_j] * B[3 + idx_i];
        B[3 + idx_k] -= B[idx_k] * B[3 + idx_i];
        B[6 + idx_j] -= B[idx_j] * B[6 + idx_i];
        B[6 + idx_k] -= B[idx_k] * B[6 + idx_i];

        /* 2nd reorder */
        dat_a = TRACK_FABS(B[3 + idx_j]);
        dat_b = TRACK_FABS(B[3 + idx_k]);

        if (dat_b > dat_a) {
                idx_l = idx_j;
                idx_j = idx_k;
                idx_k = idx_l;
        }

        B[3 + idx_k] /= B[3 + idx_j];
        B[6 + idx_k] -= B[3 + idx_k] * B[6+idx_j];

        /* calculation */
        for (idx_l=0; idx_l<6; idx_l++) {
                Z[idx_l + 6 * idx_i]  = A[     idx_l] / B[            idx_i];
                Z[idx_l + 6 * idx_j]  = A[6  + idx_l] - Z[idx_l + 6 * idx_i]*B[3 + idx_i];
                Z[idx_l + 6 * idx_k]  = A[12 + idx_l] - Z[idx_l + 6 * idx_i]*B[6 + idx_i];
                Z[idx_l + 6 * idx_j] /= B[3  + idx_j];
                Z[idx_l + 6 * idx_k] -= B[6  + idx_j] * Z[idx_l + 6 * idx_j];
                Z[idx_l + 6 * idx_k] /= B[6  + idx_k];
                Z[idx_l + 6 * idx_j] -= B[3  + idx_k] * Z[idx_l + 6 * idx_k];
                Z[idx_l + 6 * idx_i] -= B[     idx_k] * Z[idx_l + 6 * idx_k];
                Z[idx_l + 6 * idx_i] -= B[     idx_j] * Z[idx_l + 6 * idx_j];
        }
}


/* filter calculation core */
void track_run_filter_core(track_trk_pkg_t* trk_pkg, track_float_t f_time, track_trk_t *trk, track_cdi_t *cdi)
{
        /* variable */
        static uint8_t       idx_i;
        static uint8_t       idx_j;

        static track_float_t old_vel_x;
        static track_float_t old_vel_y;
#if TRK_CONF_3D
        static track_float_t old_vel_z;
#endif
        static track_float_t tra_x[STATE_ELEM_NUM];
        static track_float_t tra_P[STATE_ELEM_NUM * STATE_ELEM_NUM];
        static track_float_t tra_H[STATE_ELEM_NUM * MEASURE_ELEM_NUM] = {0};
        static track_float_t tra_Hx[MEASURE_ELEM_NUM];
        static track_float_t tra_Kg[STATE_ELEM_NUM * MEASURE_ELEM_NUM];
        static track_float_t mat_tmp_6x6[STATE_ELEM_NUM * STATE_ELEM_NUM];
        static track_float_t mat_tmp_3x6[STATE_ELEM_NUM * MEASURE_ELEM_NUM];

        /* update power and noise */
        if (cdi) {
                trk->flt_z.sig = cdi->raw_z.sig;
                trk->flt_z.noi = cdi->raw_z.noi;
        }

        /* preseve vel_x vel_y */
        old_vel_x = trk->x.vel_x;
        old_vel_y = trk->x.vel_y;
#if TRK_CONF_3D
        old_vel_z = trk->x.vel_z;
#endif

        /* tra_x = A*old_x */
        track_inline_MA_mul_MB(tra_x, track_A, &trk->x.rng_x, STATE_ELEM_NUM, 1, STATE_ELEM_NUM);

        /* A*old_P */
        track_inline_MA_mul_MB(mat_tmp_6x6, track_A, trk->P, STATE_ELEM_NUM, STATE_ELEM_NUM, STATE_ELEM_NUM);

        /* set tra_P to Q.*old_s */
        for (idx_i = 0; idx_i < STATE_ELEM_NUM; idx_i++) {
                for (idx_j = 0; idx_j < STATE_ELEM_NUM; idx_j++) {
                        if (idx_i % DIM_NUM == idx_j % DIM_NUM) {
                                if (idx_i % DIM_NUM == 0)
                                        tra_P[idx_i + STATE_ELEM_NUM * idx_j] = track_Q[idx_i + STATE_ELEM_NUM * idx_j] * trk->sigma_x;
                                else if (idx_i % DIM_NUM == 1)
                                        tra_P[idx_i + STATE_ELEM_NUM * idx_j] = track_Q[idx_i + STATE_ELEM_NUM * idx_j] * trk->sigma_y;
#if TRK_CONF_3D
                                else if (idx_i % DIM_NUM == 2)
                                        tra_P[idx_i + STATE_ELEM_NUM * idx_j] = track_Q[idx_i + STATE_ELEM_NUM * idx_j] * trk->sigma_z;
#endif
                        } else {
                                tra_P[idx_i + STATE_ELEM_NUM * idx_j] = (track_float_t)0.0;
                        }
                }
        }

        /* tra_P = A*old_P*A' + Q.*old_s */
        track_inline_Z_add_MA_mul_MB_T(tra_P, mat_tmp_6x6, track_A, STATE_ELEM_NUM, STATE_ELEM_NUM, STATE_ELEM_NUM);

        /* tra_Hx */
        track_inline_x2z((track_measu_t *)tra_Hx, (track_state_t *)tra_x);

        /* tra_H */
        mat_tmp_6x6[0] = (track_float_t)1.0 / tra_Hx[0];  /* 1/r     */
        mat_tmp_6x6[1] = mat_tmp_6x6[0] / tra_Hx[0];      /* 1/(r*r) */
        mat_tmp_6x6[2] = tra_x[0] * mat_tmp_6x6[1];       /* x/(r*r) */
        mat_tmp_6x6[3] = tra_x[1] * mat_tmp_6x6[1];       /* y/(r*r) */
#if TRK_CONF_3D
        mat_tmp_6x6[4] = tra_x[2] * mat_tmp_6x6[1];       /* z/(r*r)   */
        mat_tmp_6x6[5] = mat_tmp_6x6[1] / tra_Hx[0];      /* 1/(r*r*r) */
        mat_tmp_6x6[6] = (track_float_t)1.0 / (tra_x[0] * tra_x[0] + tra_x[1] * tra_x[1]); /* 1 / (rx*rx + ry*ry) */
        mat_tmp_6x6[7] = TRACK_SQRT(tra_x[0] * tra_x[0] + tra_x[1] * tra_x[1]);            /* sqrt(rx*rx + ry*ry) */
#endif

#if TRK_CONF_3D
        /**************************************************************/
        /*
        tra_x[0] : rx       tra_x[3] : vx
        tra_x[1] : ry       tra_x[4] : vy
        tra_x[2] : rz       tra_x[5] : vz
        */
        /**************************************************************/

        /* dr && d(vr)/d(vx, vy, vz) */
        tra_H[0] = tra_H[13] = tra_x[0] * mat_tmp_6x6[0];  /* rx/r */
        tra_H[4] = tra_H[17] = tra_x[1] * mat_tmp_6x6[0];  /* ry/r */
        tra_H[8] = tra_H[21] = tra_x[2] * mat_tmp_6x6[0];  /* rz/r */

        /* d(vr)/d(rx) */
        tra_H[1] = tra_x[1] * (tra_x[3] * tra_x[1] - tra_x[4] * tra_x[0]) + tra_x[2] * (tra_x[3] * tra_x[2] - tra_x[5] * tra_x[0]);
        tra_H[1] = tra_H[1] * mat_tmp_6x6[5];

        /* d(vr)/d(ry) */
        tra_H[5] = tra_x[0] * (tra_x[4] * tra_x[0] - tra_x[3] * tra_x[1]) + tra_x[2] * (tra_x[4] * tra_x[2] - tra_x[5] * tra_x[1]);
        tra_H[5] = tra_H[5] * mat_tmp_6x6[5];

        /* d(vr)/d(rz) */
        tra_H[9] = tra_x[0] * (tra_x[5] * tra_x[0] - tra_x[3] * tra_x[2]) + tra_x[1] * (tra_x[5] * tra_x[1] - tra_x[4] * tra_x[2]);
        tra_H[9] = tra_H[9] * mat_tmp_6x6[5];

        /* d(ang) */
        tra_H[2] = tra_x[1] * mat_tmp_6x6[6];  /* y/(x^2+y^2)  */
        tra_H[6] = -tra_x[0] * mat_tmp_6x6[6]; /* -x/(x^2+y^2) */

        /* d(ang_elv) */
        tra_H[3]  = -mat_tmp_6x6[2] * tra_x[2] / mat_tmp_6x6[7]; /* -x/(r^2)*z/(sqrt(x^2+y^2)) */
        tra_H[7]  = -mat_tmp_6x6[3] * tra_x[2] / mat_tmp_6x6[7]; /* -y/(r^2)*z/(sqrt(x^2+y^2)) */
        tra_H[11] = mat_tmp_6x6[7] * mat_tmp_6x6[1];             /* sqrt(x^2+y^2)/(r^2) */
#else
        tra_H[7]  = tra_H[0] = tra_x[0] * mat_tmp_6x6[0];
        tra_H[1]  = tra_x[2] * mat_tmp_6x6[0] - tra_Hx[1] * mat_tmp_6x6[2];
        tra_H[2]  = mat_tmp_6x6[3] * (track_float_t)RAD2ANG;
        tra_H[10] = tra_H[3] = tra_x[1] * mat_tmp_6x6[0];
        tra_H[4]  = tra_x[3] * mat_tmp_6x6[0] - tra_Hx[1] * mat_tmp_6x6[3];
        tra_H[5]  = -mat_tmp_6x6[2] * (track_float_t)RAD2ANG;
#endif

        /* tra_H*tra_P */
        track_inline_MA_mul_MB(mat_tmp_6x6, tra_H, tra_P, MEASURE_ELEM_NUM, STATE_ELEM_NUM, STATE_ELEM_NUM);

        /* tra_H*tra_P*tra_H'+R */
        track_inline_C_add_MA_mul_MB_T(mat_tmp_3x6, track_R, mat_tmp_6x6, tra_H, MEASURE_ELEM_NUM, MEASURE_ELEM_NUM, STATE_ELEM_NUM);

        trk->meas_var_inv[0] = (track_float_t)1.0 / mat_tmp_3x6[0];
        trk->meas_var_inv[1] = (track_float_t)1.0 / mat_tmp_3x6[1 + MEASURE_ELEM_NUM];
        trk->meas_var_inv[2] = (track_float_t)1.0 / mat_tmp_3x6[2 + 2 * MEASURE_ELEM_NUM];
#if TRK_CONF_3D
        trk->meas_var_inv[3] = (track_float_t)1.0 / mat_tmp_3x6[3 + 3 * MEASURE_ELEM_NUM];
#endif

        if (!cdi) { /* miss */
                /* new_x & new_P */
                memcpy(&trk->x.rng_x, tra_x, STATE_ELEM_NUM * sizeof(track_float_t));
                memcpy(trk->P, tra_P, STATE_ELEM_NUM * STATE_ELEM_NUM * sizeof(track_float_t));
        } else { /* hit */

                /* tra_P*tra_H' */
                track_inline_MA_mul_MB_T(mat_tmp_6x6, tra_P, tra_H, STATE_ELEM_NUM, MEASURE_ELEM_NUM, STATE_ELEM_NUM);

                /* (tra_P*tra_H')/(tra_H*tra_P*tra_H'+R) */
#if TRK_CONF_3D
                track_inline_MA_9x4_div_MB_4x4(tra_Kg, mat_tmp_6x6, mat_tmp_3x6);
#else
                track_inline_MA_6x3_div_MB_3x3(tra_Kg, mat_tmp_6x6, mat_tmp_3x6);
#endif

                /* mea_z-tra_Hx */
                for (idx_i = 0; idx_i < MEASURE_ELEM_NUM; idx_i++) {
                         mat_tmp_6x6[idx_i] = (&cdi->raw_z.rng)[idx_i] - tra_Hx[idx_i];
                }

                /* set new_x to tra_x */
                memcpy(&trk->x.rng_x, tra_x, STATE_ELEM_NUM * sizeof(track_float_t));

                /* new_x = tra_x + tra_Kg*(mea_z-tra_Hx); */
                track_inline_Z_add_MA_mul_MB(&trk->x.rng_x, tra_Kg, mat_tmp_6x6, STATE_ELEM_NUM, 1, MEASURE_ELEM_NUM);

                /* set mat_tmp_6x6 to eye(6) */
                memset(mat_tmp_6x6, 0, STATE_ELEM_NUM * STATE_ELEM_NUM * sizeof(track_float_t));
                for (idx_i = 0; idx_i < STATE_ELEM_NUM; idx_i++)
                        mat_tmp_6x6[idx_i + STATE_ELEM_NUM * idx_i] = (track_float_t)1.0;

                /* (eye(6)-tra_Kg*tra_H) */
                track_inline_Z_sub_MA_mul_MB(mat_tmp_6x6, tra_Kg, tra_H, STATE_ELEM_NUM, STATE_ELEM_NUM, MEASURE_ELEM_NUM);

                /* new_P = (eye(6)-tra_Kg*tra_H) * tra_P */
                track_inline_MA_mul_MB(trk->P, mat_tmp_6x6, tra_P, STATE_ELEM_NUM, STATE_ELEM_NUM, STATE_ELEM_NUM);
        } /* end of if not hit */

        /* new_z */
        track_inline_x2z(&trk->flt_z, &trk->x);


#if TRK_CONF_3D
        if (cdi) {
                mat_tmp_3x6[0] = (trk->x.vel_x - old_vel_x) / f_time;
                trk->sigma_x = (trk->sigma_x + mat_tmp_3x6[0] * mat_tmp_3x6[0]) / (track_float_t)2.0;

                mat_tmp_3x6[0] = (trk->x.vel_y - old_vel_y) / f_time;
                trk->sigma_y = (trk->sigma_y + mat_tmp_3x6[0] * mat_tmp_3x6[0]) / (track_float_t)2.0;

                mat_tmp_3x6[0] = (trk->x.vel_z - old_vel_z) / f_time;
                trk->sigma_z = (trk->sigma_z + mat_tmp_3x6[0] * mat_tmp_3x6[0]) / (track_float_t)2.0;
        }
#else
        if(!cdi){
                trk->sigma_x = trk->x.acc_x;
                trk->sigma_y = trk->x.acc_y;
        } else {
                /* ax_mean = mean(vx_new-vx_old)/T, ax_new) */
                trk->sigma_x = ( (trk->x.vel_x-old_vel_x) / f_time + trk->x.acc_x) / (track_float_t)2.0;

                /* ay_mean = mean(vy_new-vy_old)/T, ay_new) */
                trk->sigma_y = ( (trk->x.vel_y-old_vel_y) / f_time + trk->x.acc_y) / (track_float_t)2.0;
        }

        /* sigma = a^2 */
        trk->sigma_x *= trk->sigma_x;
        trk->sigma_y *= trk->sigma_y;
#endif

        /* A*new_x */
        track_inline_MA_mul_MB(mat_tmp_6x6, track_A, &trk->x.rng_x, STATE_ELEM_NUM, 1, STATE_ELEM_NUM);

        /* pre_z */
        track_inline_x2z(&trk->pre_z, (track_state_t *)mat_tmp_6x6);
}
