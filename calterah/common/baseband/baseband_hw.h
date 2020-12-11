#ifndef BASEBAND_HW_H
#define BASEBAND_HW_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#endif
#include "calterah_error.h"
#include "baseband_reg.h"
#include "./../../../embarc_osp/device/peripheral/nor_flash/flash.h"

/* CTRL CMD defines */
#define BB_CTRL_CMD_ENABLE 0x1

#define MAX_CFAR_OBJS MAX_OUTPUT_OBJS

#define BB_READ_REG(bb_hw, RN) baseband_read_reg(bb_hw, BB_REG_##RN)
#define BB_READ_REG_FEILD(bb_hw, RN, FD) baseband_read_regfield(bb_hw, BB_REG_##RN, BB_REG_##RN##_##FD##_SHIFT, BB_REG_##RN##_##FD##_MASK)
#define BB_WRITE_REG(bb_hw, RN, val) baseband_write_reg(bb_hw, BB_REG_##RN, val)
#define BB_MOD_REG(bb_hw, RN, FD, val) baseband_mod_reg(bb_hw, BB_REG_##RN, val, BB_REG_##RN##_##FD##_SHIFT, BB_REG_##RN##_##FD##_MASK)

#define REG_H16(data)                     (data >> 16) & 0xffff
#define REG_L16(data)                     (data >>  0) & 0xffff
#define REG_L8(data)                      (data >>  0) & 0xff

#ifdef UNIT_TEST
#define MDELAY(ms)
#define UDELAY(us)
#else
#define MDELAY(ms)  chip_hw_mdelay(ms);
#define UDELAY(us)  chip_hw_udelay(us);
#endif

#define SYS_ENA(MASK, var) (var << SYS_ENABLE_##MASK##_SHIFT)

#define HIL_AHB                0 // hil input from ahb bus
#define HIL_GPIO               1 // hil input from gpio(debug bus)
#define HIL_SIN_PERIOD         32  /* sine period used as hil input */

typedef struct baseband_hw {
        uint8_t frame_type_id;
} baseband_hw_t;

uint32_t baseband_read_reg(baseband_hw_t *bb_hw, const uint32_t addr);
uint32_t baseband_read_regfield(baseband_hw_t *bb_hw, const uint32_t addr,
                                const uint32_t shift, const uint32_t mask);
void baseband_write_reg(baseband_hw_t *bb_hw, const uint32_t addr, const uint32_t val);
void baseband_mod_reg(baseband_hw_t *bb_hw, const uint32_t addr, const uint32_t val,
                      const uint32_t shift, const uint32_t mask);
void baseband_reg_dump(baseband_hw_t *bb_hw);
void baseband_tbl_dump(baseband_hw_t *bb_hw, uint32_t tbl_id, uint32_t offset, uint32_t length);
void baseband_write_mem_table(baseband_hw_t *bb_hw, uint32_t offset, uint32_t value);
uint32_t  baseband_read_mem_table(baseband_hw_t *bb_hw, uint32_t offset);
uint32_t baseband_switch_mem_access(baseband_hw_t *bb_hw, uint32_t table_id);
uint32_t baseband_switch_buf_store(baseband_hw_t *bb_hw, uint32_t buf_id);

int32_t baseband_hw_init(baseband_hw_t *bb_hw);
void baseband_hw_start(baseband_hw_t *bb_hw);
void baseband_hw_start_with_params(baseband_hw_t *bb_hw, uint16_t sys_enable, uint8_t sys_irp_en);
void baseband_hw_stop(baseband_hw_t *bb_hw);
int32_t baseband_hw_dump_init(baseband_hw_t *bb_hw, bool sys_buf_store);
bool baseband_hw_is_running(baseband_hw_t *bb_hw);
uint32_t baseband_hw_get_fft_mem(baseband_hw_t *bb_hw, int ant_index, int rng_index, int vel_index, int bpm_index);
void baseband_interference_mode_set(baseband_hw_t *bb_hw);
void baseband_dc_calib_init(baseband_hw_t *bb_hw, bool leakage_ena, bool dc_calib_print_ena);
bool baseband_bist_ctrl(baseband_hw_t *bb_hw, bool print_ena);
void baseband_dac_playback(baseband_hw_t *bb_hw, bool inner_circle, uint8_t inject_num, uint8_t out_num, bool adc_dbg_en, float* peak_power);
void baseband_hil_on(baseband_hw_t *bb_hw, bool input_mux, uint8_t dmp_mux, int32_t frame_num);
void baseband_agc_dbg_reg_dump(baseband_hw_t *bb_hw, int item);
void baseband_agc_dbg_reg_store(baseband_hw_t *bb_hw, uint32_t *p_dbg_reg);
void baseband_datdump_smoke_test(baseband_hw_t *bb_hw);
void baseband_dbg_start(baseband_hw_t *bb_hw, uint8_t dbg_mux);
void baseband_frame_interleave_pattern(baseband_hw_t *bb_hw, uint8_t frame_loop_pattern);
uint32_t baseband_hw_get_sam_buf(baseband_hw_t *bb_hw, int vel_idx, int rng_idx, int ant_idx);
void baseband_interframe_power_save_init(baseband_hw_t *bb_hw);
void Txbf_bb_satur_monitor(baseband_hw_t *bb_hw, unsigned int tx_mux);
void baseband_workaroud(baseband_hw_t *bb_hw);
void baseband_hw_reset_after_force(baseband_hw_t *bb_hw);

#endif
