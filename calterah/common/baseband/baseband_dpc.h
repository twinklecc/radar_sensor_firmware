#ifndef BASEBAND_DPC_H
#define BASEBAND_DPC_H

#include "embARC_toolchain.h"
#include "baseband.h"

#define DPC_SIZE 5

typedef void (*dpc_callback)(baseband_t *bb);

typedef struct {
        dpc_callback pre;
        dpc_callback post;
        dpc_callback post_irq;

        bool fi_recfg;  // flag for frame interleaving reconfig
        bool stream_on; // flag for data dump support of stream on

        // Below are parameter for baseband_hw_start_with_params
        bool radio_en;
        bool tx_en;
        uint16_t sys_enable;
        bool cas_sync_en;
        uint8_t sys_irq_en;
        bool track_en;
        // flag for last data proc
        bool end;
} baseband_data_proc_t;

bool baseband_data_proc_run(baseband_data_proc_t * dp);
void baseband_data_proc_init();
baseband_data_proc_t * baseband_get_dpc();
void baseband_data_proc_hil(uint16_t bb_ena_0, uint16_t bb_ena_1, uint16_t bb_ena_2);
bool baseband_data_proc_req();
void bb_dpc_sysen_set(uint8_t bb_dpc_ind, uint16_t bb_sysen);

#endif
