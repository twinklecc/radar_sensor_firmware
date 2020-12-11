/*
  The header file defines interfaces between tracking module and the result of firmware.
  Please DONOT put definitions that is specific to particular tracking algorithm here!!
 */

#ifndef CALTERAH_TRACK_H
#define CALTERAH_TRACK_H

#include "track_common.h"
#include "radar_sys_params.h"
#include "calterah_limits.h"

/*--- DEFINE -------------------------*/
#define LOW2BITMASk            0x0003                /* bit 0 - bit 1 */
#define LOW10BITMASk           0x03FF                /* bit 0 - bit 9 */
#define LOW15BITMASk           0x7FFF                /* bit 0 - bit 14 */
#define LOW16BITMASk           0xFFFF                /* bit 0 - bit 15 */
#define LOW32BITMASk           0xFFFFFFFF            /* bit 0 - bit 31 */
#define ROUNDF                 (0.5)

/* The size of a full frame format is [5 * (AK + BK + Head)] words */
/* so the max array num is [5 * (2 * MAX_OBJ_NUM + 1)] */
#define MAX_OBJ_NUM            1024
#define MAX_ARRAY_NUM          (5 * MAX_OBJ_NUM + 2)

typedef void (*track_if_t)(void *data, void *ctx);
typedef void (*track_frame_interval_t)(void *data, track_float_t delta);
typedef void (*track_update_obj_t)(void *data, void *ctx, uint32_t i);
typedef bool (*track_has_run_t)(void *data);

/* tracker top */
typedef struct {
        track_float_t            f_last_time;           /* time stamp of last frame */
        track_float_t            f_now;                 /* time stamp of current frame */
        track_cdi_pkg_t          cdi_pkg;               /* raw data read from HW with conversion */
        track_obj_output_t       output_obj;            /* algorithm will update it for object data output */
        track_header_output_t    output_hdr;            /* algorithm will update it for header data output */
        void                     *trk_data;             /* tracking algorithm specific data */
        track_if_t               trk_init;              /* algorithm init handler */
        track_if_t               trk_pre;               /* algorithm pre-run handler */
        track_if_t               trk_run;               /* algorithm run handler */
        track_if_t               trk_post;              /* algorithm post-run handler */
        track_if_t               trk_stop;              /* algorithm stop handler */
        track_has_run_t          trk_has_run;           /* whether it is first time to run algorithm */
        track_frame_interval_t   trk_set_frame_int;     /* set frame interval for algorithm to run */
        track_update_obj_t       trk_update_obj_info;   /* handler to update "output_obj" */
        track_if_t               trk_update_header;     /* handler to update "output_hdr" */
        radar_sys_params_t*      radar_params[MAX_FRAME_TYPE];
} track_t;

typedef enum {
        HEAD = 1,
        BK,
        AK
} data_block_t;


/*--- DECLARAION ---------------------*/
void track_init (track_t *track);
void track_init_sub(track_t *track, void *ctx);
void track_pre_start(track_t *track);
bool track_is_ready(track_t *track);
void track_lock(track_t *track);
void track_start(track_t *track);
void track_stop (track_t *track);
void track_read (track_t *track);
void track_run  (track_t *track);
void track_output_print(track_t *track);
void track_pkg_uart_hex_print(track_t *track);
void track_pkg_uart_string_print(track_t *track);
track_t* track_get_track_ctrl();
void trac_init_track_ctrl(track_t *ctrl, void *ctx);
static void init_track_data(uint16_t number);
uint32_t transform_data(float data);
static void transfer_data(track_t *track, data_block_t data, uint16_t index, uint32_t end);

#if 0
/* TODO, following 3 functions(switch timer period) can be removed, just reserved here for future possible needs */
/* They are used for bug fixing in ALPS_B, but they're unuseful for now in ALPS_MP */
void track_timer_period_save();
void track_timer_period_restore();
void track_timer_period_change(uint8_t new_period);
#endif

#endif
