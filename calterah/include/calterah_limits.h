#ifndef CALTERAH_LIMITS_H
#define CALTERAH_LIMITS_H

#ifdef CHIP_ALPS_A
#define MAX_FRAME_TYPE 1
#else
#define MAX_FRAME_TYPE 4
#endif

/* baseband limitation */
#define MAX_FFT_WIN         2048
#define MAX_FFT_RNG         2048

#define MAX_NUM_RX          4
#define MAX_NUM_TX          4

#define MAX_CFAR_DIRS       16
#define MAX_ANT_ARRAY_SIZE  32     /*MAX_ANT_ARRAY_SIZE cascade chirp*/
#define MAX_ANT_ARRAY_SIZE_SC  16  /*MAX_ANT_ARRAY_SIZE single chirp*/

#define CFAR_WIN_MAX_RNG 7
#define CFAR_WIN_MAX_VEL 9
#define CFAR_MIMO_COMB_COE (MAX_CFAR_DIRS * MAX_ANT_ARRAY_SIZE * MAX_FRAME_TYPE)
#define CFAR_MAX_GRP_NUM 8
#define CFAR_MAX_RECWIN_MSK_LEN_PERGRP 11
#define CFAR_MAX_RECWIN_MSK_LEN (CFAR_MAX_GRP_NUM * CFAR_MAX_RECWIN_MSK_LEN_PERGRP)

#define DEBPM_COE (MAX_ANT_ARRAY_SIZE_SC * MAX_FRAME_TYPE)
#define BB_ADDRESS_UNIT     4
#define MAX_BFM_GROUP_NUM 3
#define AGC_CODE_ENTRY_NUM 13

#ifdef CHIP_CASCADE
#define DOA_MAX_NUM_RX  8
#else
#define DOA_MAX_NUM_RX  4
#endif


#endif
