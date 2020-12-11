/*
   This is a common header file shared between firmware and PC-simulation.
   Therefore, definition used by both parties should be include here.
*/

#ifndef TRACK_COMMON_H
#define TRACK_COMMON_H

#include "math.h"

#ifndef TRACK_TEST_MODE
        #ifdef UNIT_TEST
                #include "calterah_unit_test.h"
        #else
                #include "embARC_toolchain.h"
        #endif
#else
        /* #include "mex.h" */
        /* #define inline __inline */
        /* typedef unsigned int   uint32_t; */
        /* typedef          int   int32_t ; */
        /* typedef unsigned short uint16_t; */
        /* typedef          short int16_t ; */
        /* typedef unsigned char  uint8_t ; */
        /* typedef          char  int8_t  ; */
        #include "stdint.h"
        #include "stdbool.h"
#endif

/*--- DEFINES ------------------------*/
#ifndef M_PI
#define M_PI                     3.1415926536
#endif

#ifndef RAD2ANG
#define RAD2ANG                  57.2957795131
#endif

#ifndef ANG2RAD
#define ANG2RAD                  0.017453292519943
#endif

#define TRACK_USE_DOUBLE         0
#ifdef MAX_OUTPUT_OBJS
#define TRACK_NUM_CDI            MAX_OUTPUT_OBJS     /* candidate */
#define TRACK_NUM_TRK            MAX_OUTPUT_OBJS     /* tracker   */
#else
#define TRACK_NUM_CDI            64     /* candidate */
#define TRACK_NUM_TRK            64     /* tracker   */
#endif //MAX_OUTPUT_OBJS 

#if TRACK_USE_DOUBLE
        #define TRACK_FABS       fabs
        #define TRACK_POW        pow
        #define TRACK_SIN        sin
        #define TRACK_COS        cos
        #define TRACK_SQRT       sqrt
        #define TRACK_ATAN       atan
        #define TRACK_LOG10      log10
#else
        #define TRACK_FABS       fabsf
        #define TRACK_POW        powf
        #define TRACK_SIN        sinf
        #define TRACK_COS        cosf
        #define TRACK_SQRT       sqrtf
        #define TRACK_ATAN       atanf
        #define TRACK_LOG10      log10f
#endif

/*--- TYPEDEF ------------------------*/
/* float type used in track */
#if TRACK_USE_DOUBLE
        typedef double track_float_t;
#else
        typedef float  track_float_t;
#endif

/* measurement */
typedef struct {
        track_float_t rng;       /* range    */
        track_float_t vel;       /* velocity */
        track_float_t ang;       /* angle    */
#if TRK_CONF_3D
        track_float_t ang_elv;   /* angle of elevation */
        track_float_t sig_elv;   /* power of elevation*/
#endif
        track_float_t sig;       /* power    */
        track_float_t noi;       /* noise    */
} track_measu_t;

/* candidate */
typedef struct {
        uint32_t      index;
        track_measu_t raw_z;
} track_cdi_t;

/* candidate package */
typedef struct {
        uint32_t    raw_number;            /* measurement number before pre-filter */
        uint32_t    cdi_number;            /* measurement number after  pre-filter */
        track_cdi_t cdi[TRACK_NUM_CDI];    /* measurement */
} track_cdi_pkg_t;

typedef struct {
        bool          output;
        track_float_t SNR;
        track_float_t rng;
        track_float_t vel;
        track_float_t ang;
#if TRK_CONF_3D
        track_float_t ang_elv;
	    track_float_t SNR_elv;
#endif
        uint32_t      track_level;
} track_obj_output_t;

typedef struct {
        track_float_t frame_int;
        uint32_t      frame_id;
        uint32_t      track_output_number;
} track_header_output_t;

#endif
