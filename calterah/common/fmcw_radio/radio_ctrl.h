#ifndef RADIO_CTRL_H
#define RADIO_CTRL_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

#include "fmcw_radio.h"

typedef enum {
        CH_ALL = -1,
        CH0    = 0,
        CH1    = 1,
        CH2    = 2,
        CH3    = 3,
} CHANNEL_INDEX_t;

typedef enum{
        TX_POWER_INDEX0 = 0,
        TX_POWER_INDEX1 = 1,
        TX_POWER_INDEX2 = 2,
        TX_POWER_INDEX3 = 3,
} TX_POWER_t;

#define TX_POWER_DEFAULT       0xAA
#define TX_POWER_MAX           0xFF
#define TX_POWER_MAX_SUB5      0x88
#define TX_POWER_MAX_SUB10     0x00

// Higher byte for IDAC Sign and IDAC Amplitude, lower byte for QDAC Sign and QDAC Amplitude
#if defined(CHIP_ALPS_MP)
#define TX_PHASE_0       0x0f0fu
#define TX_PHASE_45      0x000fu
#define TX_PHASE_90      0x1f0fu
#define TX_PHASE_135     0x1f00u
#define TX_PHASE_180     0x1f1fu
#define TX_PHASE_225     0x001fu
#define TX_PHASE_270     0x0f1fu
#define TX_PHASE_315     0x0f00u
#else //CHIP_ALPS_A, CHIP_ALPS_B
#define TX_PHASE_0       0x1f10u
#define TX_PHASE_22_5    0x1d16u
#define TX_PHASE_45      0x1b1au
#define TX_PHASE_67_5    0x171cu
#define TX_PHASE_90      0x011eu
#define TX_PHASE_112_5   0x071cu
#define TX_PHASE_135     0x0b1au
#define TX_PHASE_157_5   0x0d16u
#define TX_PHASE_180     0x0f00u
#define TX_PHASE_202_5   0x0d06u
#define TX_PHASE_225     0x0b0au
#define TX_PHASE_247_5   0x070cu
#define TX_PHASE_270     0x110eu
#define TX_PHASE_292_5   0x170cu
#define TX_PHASE_315     0x1b0au
#define TX_PHASE_337_5   0x1d06u
#endif

#define AUXADC1_MainBG_VPTAT     0x00
#define AUXADC1_TestMUXN         0x01
#define AUXADC1_TestMUXP         0x10
#define AUXADC1_TPANA1           0x11

#define AUXADC2_TS_VPTAT     0x00
#define AUXADC2_TS_VBG       0x01
#define AUXADC2_TestMUXN     0x10
#define AUXADC2_TPANA2       0x11

#define RADIO_WRITE_BANK_REG(bk, addr, val) fmcw_radio_reg_write(NULL, RADIO_BK##bk##_##addr, val)
#define RADIO_WRITE_BANK_CH_REG(bk, ch, addr, val) fmcw_radio_reg_write(NULL, \
                                                                        (ch == 0) ? RADIO_BK##bk##_CH0_##addr : \
                                                                        (ch == 1) ? RADIO_BK##bk##_CH1_##addr : \
                                                                        (ch == 2) ? RADIO_BK##bk##_CH2_##addr : \
                                                                        RADIO_BK##bk##_CH3_##addr, \
                                                                        val)

#define RADIO_MOD_BANK_FWCW_TX_REG(bk, tx, addr, field, val) \
        fmcw_radio_reg_mod(NULL, \
                           (tx == 0) ? RADIO_BK##bk##_FMCW_TX0_##addr :      \
                           (tx == 1) ? RADIO_BK##bk##_FMCW_TX1_##addr :      \
                           (tx == 2) ? RADIO_BK##bk##_FMCW_TX2_##addr :      \
                           RADIO_BK##bk##_FMCW_TX3_##addr,                   \
                           (tx == 0) ? RADIO_BK##bk##_FMCW_TX0_##addr##_##field##_SHIFT :      \
                           (tx == 1) ? RADIO_BK##bk##_FMCW_TX1_##addr##_##field##_SHIFT :      \
                           (tx == 2) ? RADIO_BK##bk##_FMCW_TX2_##addr##_##field##_SHIFT :      \
                           RADIO_BK##bk##_FMCW_TX3_##addr##_##field##_SHIFT,  \
                           (tx == 0) ? RADIO_BK##bk##_FMCW_TX0_##addr##_##field##_MASK :      \
                           (tx == 1) ? RADIO_BK##bk##_FMCW_TX1_##addr##_##field##_MASK :      \
                           (tx == 2) ? RADIO_BK##bk##_FMCW_TX2_##addr##_##field##_MASK :      \
                           RADIO_BK##bk##_FMCW_TX3_##addr##_##field##_MASK,  \
                           val)

#define RADIO_READ_BANK_REG(bk, addr) fmcw_radio_reg_read(NULL, RADIO_BK##bk##_##addr)
#define RADIO_READ_BANK_CH_REG(bk, ch, addr) fmcw_radio_reg_read(NULL, (ch == 0) ? RADIO_BK##bk##_CH0##_##addr : \
                                                                 (ch == 1) ? RADIO_BK##bk##_CH1##_##addr : \
                                                                 (ch == 2) ? RADIO_BK##bk##_CH2##_##addr : \
                                                                 RADIO_BK##bk##_CH3##_##addr)

#define RADIO_MOD_BANK_REG(bk, addr, field, val) fmcw_radio_reg_mod(NULL, RADIO_BK##bk##_##addr, \
                                                               RADIO_BK##bk##_##addr##_##field##_SHIFT, \
                                                               RADIO_BK##bk##_##addr##_##field##_MASK, val)

uint32_t phase_val_2_reg_val(uint32_t phase_val);
uint32_t reg_val_2_phase_val(uint32_t reg_val);

uint32_t radio_spi_cmd_mode(uint32_t mode);
void radio_spi_cmd_write(char addr, char data);
uint32_t radio_spi_cmd_read(char addr);

char fmcw_radio_reg_read(fmcw_radio_t *radio, char addr);
char fmcw_radio_reg_read_field(fmcw_radio_t *radio, char addr, char shift, char mask);
void fmcw_radio_reg_write(fmcw_radio_t *radio, char addr, char data);
void fmcw_radio_reg_mod(fmcw_radio_t *radio, char addr, char shift, char mask, char data);
void fmcw_radio_reg_dump(fmcw_radio_t *radio);

void fmcw_radio_power_on(fmcw_radio_t *radio);
void fmcw_radio_power_off(fmcw_radio_t *radio);
uint8_t fmcw_radio_switch_bank(fmcw_radio_t *radio, uint8_t bank);
int32_t fmcw_radio_rx_buffer_on(fmcw_radio_t *radio);
int32_t fmcw_radio_ldo_on(fmcw_radio_t *radio);
int32_t fmcw_radio_refpll_on(fmcw_radio_t *radio);
int32_t fmcw_radio_pll_on(fmcw_radio_t *radio);
int32_t fmcw_radio_lo_on(fmcw_radio_t *radio, bool enable);
int32_t fmcw_radio_tx_on(fmcw_radio_t *radio);
int32_t fmcw_radio_rx_on(fmcw_radio_t *radio, bool enable);
int32_t fmcw_radio_adc_on(fmcw_radio_t *radio);
int32_t fmcw_radio_do_refpll_cal(fmcw_radio_t *radio);
int32_t fmcw_radio_do_pll_cal(fmcw_radio_t *radio, uint32_t lock_freq);
bool fmcw_radio_is_refpll_locked(fmcw_radio_t *radio);
bool fmcw_radio_is_pll_locked(fmcw_radio_t *radio);
void fmcw_radio_frame_interleave_pattern(fmcw_radio_t *radio, uint8_t frame_loop_pattern);
void fmcw_radio_frame_type_reset(fmcw_radio_t *radio);
void fmcw_radio_generate_fmcw(fmcw_radio_t *radio);
void fmcw_radio_start_fmcw(fmcw_radio_t *radio);
void fmcw_radio_stop_fmcw(fmcw_radio_t *radio);
int32_t fmcw_radio_single_tone(fmcw_radio_t *radio, double freq, bool enable);

void fmcw_radio_set_tia_gain(fmcw_radio_t *radio, int32_t channel_index, int32_t gain);
int32_t fmcw_radio_get_tia_gain(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_set_vga1_gain(fmcw_radio_t *radio, int32_t channel_index, int32_t gain);
int32_t fmcw_radio_get_vga1_gain(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_set_vga2_gain(fmcw_radio_t *radio, int32_t channel_index, char gain);
int32_t fmcw_radio_get_vga2_gain(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_set_tx_status(fmcw_radio_t *radio, int32_t channel_index, char status);
int32_t fmcw_radio_get_tx_status(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_set_tx_power(fmcw_radio_t *radio, int32_t channel_index, char power_index);
int32_t fmcw_radio_get_tx_power(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_set_tx_phase(fmcw_radio_t *radio, int32_t channel_index, int32_t phase_index);
int32_t fmcw_radio_get_tx_phase(fmcw_radio_t *radio, int32_t channel_index);
float fmcw_radio_get_temperature(fmcw_radio_t *radio);
void fmcw_radio_gain_compensation(fmcw_radio_t *radio);
int32_t fmcw_radio_vctrl_on(fmcw_radio_t *radio);
int32_t fmcw_radio_vctrl_off(fmcw_radio_t *radio);
void fmcw_radio_if_output_on(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_if_output_off(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_tx_ch_on(fmcw_radio_t *radio, int32_t channel_index, bool enable);
void fmcw_radio_rc_calibration(fmcw_radio_t *radio);
void fmcw_radio_adc_cmp_calibration(fmcw_radio_t *radio);
void fmcw_radio_set_hpf1(fmcw_radio_t *radio, int32_t channel_index, int32_t filter_index);
int32_t fmcw_radio_get_hpf1(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_set_hpf2(fmcw_radio_t *radio, int32_t channel_index, int32_t filter_index);
int32_t fmcw_radio_get_hpf2(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_dc_reg_cfg(fmcw_radio_t *radio, int32_t channel_index, int16_t dc_offset, bool dc_calib_print_ena);
void fmcw_radio_dac_reg_cfg_outer(fmcw_radio_t *radio);
void fmcw_radio_dac_reg_cfg_inner(fmcw_radio_t *radio, uint8_t inject_num, uint8_t out_num);
void fmcw_radio_dac_reg_restore(fmcw_radio_t *radio);
void fmcw_radio_tx_restore(fmcw_radio_t *radio);
void fmcw_radio_agc_enable(fmcw_radio_t *radio, bool enable);
void fmcw_radio_agc_setup(fmcw_radio_t *radio);
void fmcw_radio_special_mods_off(fmcw_radio_t *radio);
int32_t fmcw_radio_pll_clock_en(void);
void fmcw_radio_hp_auto_ch_on(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_hp_auto_ch_off(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_tx_auto_ch_on(fmcw_radio_t *radio, int32_t channel_index, int32_t auto_sel);
void fmcw_radio_tx_auto_ch_off(fmcw_radio_t *radio, int32_t channel_index);
void fmcw_radio_sdm_reset(fmcw_radio_t *radio);
void fmcw_radio_vam_status_save(fmcw_radio_t *radio);
void fmcw_radio_vam_disable(fmcw_radio_t *radio);
void fmcw_radio_vam_status_restore(fmcw_radio_t *radio);
void fmcw_radio_cmd_cfg(fmcw_radio_t *radio);
void fmcw_radio_frame_interleave_config(fmcw_radio_t *radio, uint32_t fil_que, uint8_t fil_prd);
void fmcw_radio_auxadc_trim(fmcw_radio_t *radio);
uint32_t fmcw_radio_rfbist_trim(fmcw_radio_t *radio);
int32_t fmcw_radio_lvds_on(fmcw_radio_t *radio, bool enable);
void fmcw_radio_clk_out_for_cascade(void);
float fmcw_radio_auxadc1_voltage(fmcw_radio_t *radio, int32_t muxin_sel);
float fmcw_radio_auxadc2_voltage(fmcw_radio_t *radio, int32_t muxin_sel);
void fmcw_radio_txphase_status(fmcw_radio_t *radio, bool save);
uint8_t* fmcw_radio_sm_ldo_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_avdd33_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_dvdd11_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_bg_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_cpu_clk_lock_det_IRQ(fmcw_radio_t *radio);
uint8_t* fmcw_radio_sm_rfpower_detector_fault_injection_IRQ(fmcw_radio_t *radio, double freq, double power_th, int32_t channel_index);
uint8_t* fmcw_radio_sm_rfpower_detector_IRQ(fmcw_radio_t *radio, double freq, int32_t channel_index);
uint8_t fmcw_radio_sm_if_loopback_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_rf_loopback_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_chirp_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection);
uint8_t fmcw_radio_sm_over_temp_detector_IRQ(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_pdt_reg(fmcw_radio_t *radio, int8_t pdt_type, int32_t channel_index);
double fmcw_radio_auxadc2_dout(fmcw_radio_t *radio);
double fmcw_get_pa_dout(fmcw_radio_t *radio, double cal_on, double cal_off, int32_t channel_index, double freq, double power_th);
void fmcw_radio_sm_fault_injection(fmcw_radio_t *radio);
int32_t fmcw_radio_adc_fmcwmmd_ldo_on(fmcw_radio_t *radio, bool enable);
void fmcw_radio_loop_test_en(fmcw_radio_t *radio, bool enable);
void fmcw_radio_txlobuf_on(fmcw_radio_t *radio);
void fmcw_radio_sm_ldo_monitor_setting(fmcw_radio_t *radio, bool fault_injection);
uint8_t* fmcw_radio_sm_ldo_monitor_ldo_on(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_sm_ldo_monitor_ldo_off(fmcw_radio_t *radio, bool fault_injection, uint8_t* old_ldo);
void fmcw_radio_sm_avdd33_monitor_setting(fmcw_radio_t *radio);
void fmcw_radio_sm_avdd33_monitor_threshold(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_sm_dvdd11_monitor_setting(fmcw_radio_t *radio);
void fmcw_radio_sm_dvdd11_monitor_threshold(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_sm_bg_monitor_setting(fmcw_radio_t *radio);
void fmcw_radio_sm_bg_monitor_threshold(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_sm_cpu_clk_lock_det_fault_injection(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_sm_rfpower_detector_setting(fmcw_radio_t *radio, double freq, int32_t channel_index);
void fmcw_radio_sm_rfpower_detector_fault_injection_threshold(fmcw_radio_t *radio);
void fmcw_radio_sm_rfpower_detector_threshold(fmcw_radio_t *radio, double freq, double power_th, int32_t channel_index);
void fmcw_radio_sm_chirp_monitor_setting(fmcw_radio_t *radio);
void fmcw_radio_sm_chirp_monitor_fault_injection(fmcw_radio_t *radio, bool fault_injection);
void fmcw_radio_sm_over_temp_detector_setting(fmcw_radio_t *radio);
void fmcw_radio_sm_over_temp_detector_threshold(fmcw_radio_t *radio, bool fault_injection);
float fmcw_radio_rf_comp_code(fmcw_radio_t *radio);
uint32_t* fmcw_doppler_move(fmcw_radio_t *radio);

#define CMD_CYCLE_MARGIN 20

#define REG_L(data)                       (data >>  0) & 0xff
#define REG_M(data)                       (data >>  8) & 0xff
#define REG_H(data)                       (data >> 16) & 0xff
#define REG_INT(data)                     (data >> 24) & 0xff
#define REG_H7(data)                      (data >>  8) & 0x7f

#endif
