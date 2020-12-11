
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#endif

#include "calterah_error.h"
#include "sensor_config.h"
#include "baseband.h"
#include "baseband_hw.h"
#include "cmd_reg.h"
#include "radio_reg.h"
#include "radio_ctrl.h"
#include "alps_dmu_reg.h"
#include "baseband_alps_FM_reg.h"
#include "cascade.h"

#define RADIO_WRITE_BANK_REG_OFFSET(bk, addr, offset, val) fmcw_radio_reg_write(radio, RADIO_BK##bk##_##addr + offset, val)
#define RADIO_READ_REG(addr) fmcw_radio_reg_read(radio, RADIO_##addr)
#define RADIO_READ_REG_FIELD(addr, field) fmcw_radio_reg_read_field(radio, RADIO_BK##bk##_##addr, RADIO##_##addr##_##field##_SHIFT,\
                                                                    addr##_##field##_MASK)
#define RADIO_READ_BANK_REG_FIELD(bk, addr, field) fmcw_radio_reg_read_field(radio, RADIO_BK##bk##_##addr, \
                                                                             RADIO_BK##bk##_##addr##_##field##_SHIFT, \
                                                                             RADIO_BK##bk##_##addr##_##field##_MASK)

#define RADIO_READ_BANK_CH_REG_FIELD(bk, ch, addr, field) \
        fmcw_radio_reg_read_field(radio,                                \
                                  (ch == 0) ? RADIO_BK##bk##_CH0_##addr : \
                                  (ch == 1) ? RADIO_BK##bk##_CH1_##addr : \
                                  (ch == 2) ? RADIO_BK##bk##_CH2_##addr : \
                                  RADIO_BK##bk##_CH3_##addr,            \
                                  (ch == 0) ? RADIO_BK##bk##_CH0_##addr##_##field##_SHIFT : \
                                  (ch == 1) ? RADIO_BK##bk##_CH1_##addr##_##field##_SHIFT : \
                                  (ch == 2) ? RADIO_BK##bk##_CH2_##addr##_##field##_SHIFT : \
                                  RADIO_BK##bk##_CH3_##addr##_##field##_SHIFT, \
                                  (ch == 0) ? RADIO_BK##bk##_CH0_##addr##_##field##_MASK : \
                                  (ch == 1) ? RADIO_BK##bk##_CH1_##addr##_##field##_MASK : \
                                  (ch == 2) ? RADIO_BK##bk##_CH2_##addr##_##field##_MASK : \
                                  RADIO_BK##bk##_CH3_##addr##_##field##_MASK)


#define RADIO_WRITE_REG(addr, val) fmcw_radio_reg_write(radio, RADIO_##addr, val)

#define RADIO_MOD_REG(addr, field, val) fmcw_radio_reg_mod(radio, RADIO##_##addr, RADIO##_##addr##_##field##_SHIFT, \
                                                      addr##_##field##_MASK, val)
#define RADIO_MOD_BANK_CH_REG(bk, ch, addr, field, val) \
        fmcw_radio_reg_mod(radio, \
                           (ch == 0) ? RADIO_BK##bk##_CH0_##addr :      \
                           (ch == 1) ? RADIO_BK##bk##_CH1_##addr :      \
                           (ch == 2) ? RADIO_BK##bk##_CH2_##addr :      \
                           RADIO_BK##bk##_CH3_##addr,                   \
                           (ch == 0) ? RADIO_BK##bk##_CH0_##addr##_##field##_SHIFT : \
                           (ch == 1) ? RADIO_BK##bk##_CH1_##addr##_##field##_SHIFT : \
                           (ch == 2) ? RADIO_BK##bk##_CH2_##addr##_##field##_SHIFT : \
                           RADIO_BK##bk##_CH3_##addr##_##field##_SHIFT, \
                           (ch == 0) ? RADIO_BK##bk##_CH0_##addr##_##field##_MASK : \
                           (ch == 1) ? RADIO_BK##bk##_CH1_##addr##_##field##_MASK : \
                           (ch == 2) ? RADIO_BK##bk##_CH2_##addr##_##field##_MASK : \
                           RADIO_BK##bk##_CH3_##addr##_##field##_MASK,  \
                           val)

#define TX_EN_CH(ch)                      (ch == 0) ? RADIO_BK1_CH0_TX_EN0 : \
                                          (ch == 1) ? RADIO_BK1_CH1_TX_EN0 : \
                                          (ch == 2) ? RADIO_BK1_CH2_TX_EN0 : \
                                          RADIO_BK1_CH3_TX_EN0

#define TX_PHASE0_CH(ch)                  (ch == 0) ? RADIO_BK1_CH0_TX_TUNE0 : \
                                          (ch == 1) ? RADIO_BK1_CH1_TX_TUNE0 : \
                                          (ch == 2) ? RADIO_BK1_CH2_TX_TUNE0 : \
                                          RADIO_BK1_CH3_TX_TUNE0
#define TX_PHASE1_CH(ch)                  (ch == 0) ? RADIO_BK1_CH0_TX_TUNE1 : \
                                          (ch == 1) ? RADIO_BK1_CH1_TX_TUNE1 : \
                                          (ch == 2) ? RADIO_BK1_CH2_TX_TUNE1 : \
                                          RADIO_BK1_CH3_TX_TUNE1
/* 01       10                01         10 */
/* in phase opposite phase    in phase opposite phase */
#define VAM_TX_BPM_PATTEN_1               0x66
/* 01       01                10         10 */
/* in phase, in phase, opposite phase, opposite phase */
#define VAM_TX_BPM_PATTEN_2               0x5A
/* 01       10                10         01*/
/* in phase, opposite phase, opposite phase, in phase */
#define VAM_TX_BPM_PATTEN_3               0x69

#define RADIO_WRITE_BANK_FWCW_TX_REG(bk, tx, addr, val) \
        fmcw_radio_reg_write(radio, \
                           (tx == 0) ? RADIO_BK##bk##_FMCW_TX0_##addr :      \
                           (tx == 1) ? RADIO_BK##bk##_FMCW_TX1_##addr :      \
                           (tx == 2) ? RADIO_BK##bk##_FMCW_TX2_##addr :      \
                           RADIO_BK##bk##_FMCW_TX3_##addr,                   \
                           val)

#ifdef UNIT_TEST
#define MDELAY(ms)
#else
#define MDELAY(ms)  chip_hw_mdelay(ms);
#endif

static uint8_t vam_status[4];
static uint8_t txphase_status[8];
bool safety_monitor_mode = false;
uint32_t fmcw_radio_compute_lock_freq(fmcw_radio_t *radio);

uint32_t radio_spi_cmd_mode(uint32_t mode)
{
        volatile uint32_t *dest = (uint32_t *)RADIO_SPI_CMD_SRC_SEL;
        *dest = mode;
        return mode;
}

void radio_spi_cmd_write(char addr, char data)
{
        volatile uint32_t *dest = (uint32_t *)RADIO_SPI_CMD_OUT;
        *dest  = (  (RADIO_SPI_CMD_OUT_WR_EN_MASK << RADIO_SPI_CMD_OUT_WR_EN_SHIFT)
                  + ((addr & RADIO_SPI_CMD_OUT_ADDR_MASK) << RADIO_SPI_CMD_OUT_ADDR_SHIFT)
                  + ((data & RADIO_SPI_CMD_OUT_DATA_MASK) << RADIO_SPI_CMD_OUT_DATA_SHIFT) );
}

uint32_t radio_spi_cmd_read(char addr)
{
        volatile uint32_t *dest = (uint32_t *)RADIO_SPI_CMD_OUT;
        *dest = (addr & RADIO_SPI_CMD_OUT_ADDR_MASK) << RADIO_SPI_CMD_OUT_ADDR_SHIFT;
        dest = (uint32_t *)RADIO_SPI_CMD_IN;
        return *dest;
}

char fmcw_radio_reg_read(fmcw_radio_t *radio, char addr)
{
        uint32_t cmd_mode_pre;
        char cmd_rd_data;
        cmd_mode_pre = radio_spi_cmd_mode(RADIO_SPI_CMD_CPU);
        cmd_rd_data = radio_spi_cmd_read(addr);
        radio_spi_cmd_mode(cmd_mode_pre);
        return cmd_rd_data;
}

char fmcw_radio_reg_read_field(fmcw_radio_t *radio, char addr, char shift, char mask)
{
        uint32_t cmd_mode_pre;
        char cmd_rd_data;
        cmd_mode_pre = radio_spi_cmd_mode(RADIO_SPI_CMD_CPU);
        cmd_rd_data = radio_spi_cmd_read(addr);
        radio_spi_cmd_mode(cmd_mode_pre);
        return ((cmd_rd_data >> shift) & mask);
}

void fmcw_radio_reg_write(fmcw_radio_t *radio, char addr, char data)
{
        uint32_t cmd_mode_pre;
        cmd_mode_pre = radio_spi_cmd_mode(RADIO_SPI_CMD_CPU);
        radio_spi_cmd_write(addr, data);
        radio_spi_cmd_mode(cmd_mode_pre);
}

void fmcw_radio_reg_mod(fmcw_radio_t *radio, char addr, char shift, char mask, char data)
{
        uint32_t cmd_mode_pre;
        char cmd_rd_data;
        cmd_mode_pre = radio_spi_cmd_mode(RADIO_SPI_CMD_CPU);
        cmd_rd_data = radio_spi_cmd_read(addr);
        cmd_rd_data &= ~(mask << shift);
        cmd_rd_data |= (data & mask) << shift;
        radio_spi_cmd_write(addr, cmd_rd_data);
        radio_spi_cmd_mode(cmd_mode_pre);
}

void fmcw_radio_reg_dump(fmcw_radio_t *radio)
{
        uint8_t old_bank;
        char bank_index, addr_index, rd_data;
        old_bank = fmcw_radio_switch_bank(radio, 0);
        for (bank_index = 0; bank_index < BNK_NUM; bank_index++){
                fmcw_radio_switch_bank(radio, bank_index);
                for (addr_index = 1; addr_index < BNK_SIZE; addr_index++) {
                        rd_data = fmcw_radio_reg_read(radio, addr_index);
                        EMBARC_PRINTF("\r\n%1d %3d 0x%2x",
                                      bank_index, addr_index, rd_data);
                }
                                MDELAY(10);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

uint8_t fmcw_radio_switch_bank(fmcw_radio_t *radio, uint8_t bank)
{
        uint8_t old_bank = RADIO_READ_BANK_REG(0, REG_BANK);
        RADIO_WRITE_BANK_REG(0, REG_BANK, bank);
        return old_bank;
}

int32_t fmcw_radio_rx_buffer_on(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 2);
        int ch;
        RADIO_MOD_BANK_REG(2, LVDS_LDO25, LDO25_LVDS_LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(2, LVDS_LDO25, LDO25_LVDS_VSEL, 0x4);
        for (ch = 0; ch < MAX_NUM_RX; ch++)
                RADIO_MOD_BANK_CH_REG(2, ch, BUFFER, EN, 0x1);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_ldo_on(fmcw_radio_t *radio)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        /* CBC enable */
        RADIO_MOD_BANK_REG(0, CBC_EN, CGM_EN, 0x1);
        RADIO_MOD_BANK_REG(0, CBC_EN, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, CBC_EN, BG_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO25_PMU, EN, 0x1);
        /* Output voltage fixed in AlpsMP */
#if HTOL_TEST == 1
        RADIO_MOD_BANK_REG(0, POR,LDO11_SPI_VOUT_SEL, 0x6);
#else
        RADIO_MOD_BANK_REG(0, POR,LDO11_SPI_VOUT_SEL, 0x4);
#endif
        /* LDO for PLL enable */
        RADIO_MOD_BANK_REG(0, REFPLL_LDO0, LDO25_XTAL_LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO1, LDO25_PLL_LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO2, LDO11_VCO_LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO3, LDO11_MMD_LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO4, LDO11_MMD2_LDO_EN, 0x1);

#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO1, LDO25_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO2, LDO11_VCO_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO4, LDO11_CM_EN, 0x1);
        } else {
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO1, LDO25_EN, 0x0);
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO2, LDO11_VCO_EN, 0x0);
                RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x0);
        }
        RADIO_WRITE_REG(BK0_LO_LDO1, 0xc0);
#else
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO1, LDO25_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO2, LDO11_VCO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO4, LDO11_CM_EN, 0x1);
#endif
        /* LDO for LO enable */
        RADIO_MOD_BANK_REG(0, LO_LDO0, LDO11_LOMAINPATH_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_LDO2, LDO11_TXLO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_LDO3, LDO11_RXLO_EN, 0x1);
        /* LDO for RX enable */
        RADIO_MOD_BANK_REG(0, RX_LDO0, LDO11_RFN_EN, 0x1);
        RADIO_MOD_BANK_REG(0, RX_LDO1, LDO11_RFS_EN, 0x1);
        RADIO_MOD_BANK_REG(0, RX_LDO2, LDO25_BBN_EN, 0x1);
        RADIO_MOD_BANK_REG(0, RX_LDO3, LDO25_BBS_EN, 0x1);

        fmcw_radio_switch_bank(radio, 1);
        /* LDO for TX and TX's PA enable */
        if (cfg->tx_groups[0] == 0)        {
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_PA_EN, 0x0);
        }
        else{
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_EN, 0x1);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_PA_EN, 0x1);
        }

        if (cfg->tx_groups[1] == 0){
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_PA_EN, 0x0);

        }
        else{
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_EN, 0x1);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_PA_EN, 0x1);

        }

        if (cfg->tx_groups[2] == 0){
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_PA_EN, 0x0);
        }
        else{
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_EN, 0x1);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_PA_EN, 0x1);
        }

        if (cfg->tx_groups[3] == 0){
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_PA_EN, 0x0);
        }
        else{
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_EN, 0x1);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_PA_EN, 0x1);
        }

        /* Disable LDO for TX and TX's PA in cascade slave chip */
#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_SLAVE) {
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_PA_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_PA_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_PA_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_EN, 0x0);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_PA_EN, 0x0);
        }
#endif
        /* No Need to ON Buffer, only in test */
        /* LDO for ADC enable */
        RADIO_MOD_BANK_REG(1, ADC_LDO0, LDO11_ADC12_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO1, LDO12_ADC12_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO2, LDO25_ADC12_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO3, LDO11_ADC34_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO4, LDO12_ADC34_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO5, LDO25_ADC34_EN, 0x1);

        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_refpll_on(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        /* enable reference PLL's mainly part */
#ifdef CHIP_CASCADE
        /* cfg->cascade_mode must not be used here, since initializaiton is not started yet */
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                RADIO_MOD_BANK_REG(0, REFPLL_EN, CLK_400M_FMCW_EN, 0x1);
        else
                RADIO_MOD_BANK_REG(0, REFPLL_EN, CLK_400M_FMCW_EN, 0x0);

#else
        RADIO_MOD_BANK_REG(0, REFPLL_EN, CLK_400M_FMCW_EN, 0x1);
#endif
        RADIO_MOD_BANK_REG(0, REFPLL_EN, DIV_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_EN, VCO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_EN, CP_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_EN, XOSC_BUF_EN, 0x1);
        RADIO_MOD_BANK_REG(0, REFPLL_EN, XOSC_EN, 0x1); // FIXME, slave may change to be 0, but 1 may also work, comments by wenting
        //enalbe clock to Lock detector,400M ADC, CPU
        RADIO_MOD_BANK_REG(0, PLL_VIO_OUT, REFPLL_REF_AUX_OUT_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_VIO_OUT, REFPLL_REF_OUT_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x0);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_LOCK_DIV_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_LOCK_REF_EN, 0x1);
        //Since PN under 450MHz of ALpsB is very poor, abandon 450MHz, default 400MHz
        #if FMCW_SDM_FREQ == 400
                RADIO_MOD_BANK_REG(0, REFPLL_DIV, L, 0x1);
                RADIO_MOD_BANK_REG(0, REFPLL_LF, DIVH2, 0x2);
        #elif FMCW_SDM_FREQ == 360
                RADIO_MOD_BANK_REG(0, REFPLL_DIV, L, 0x0);
                RADIO_MOD_BANK_REG(0, REFPLL_LF, DIVH2, 0x4);
        #endif

        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_pll_on(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        //enable FMCWPLL's mainly block and on 4G
#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, TSPCDIV_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CMLDIV_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, 4G_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, VCO_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CP_EN, 0x1);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, PFD_DL_DIS, 0x0); // auto dis should open, or FMCW will not work, comments by wenting
                RADIO_MOD_BANK_REG(0, FMCWPLL_VCO, FMCWPLL_PFD_DL_DIS2, 0x1); //dis2
        } else {
                RADIO_WRITE_REG(BK0_FMCWPLL_EN, 0);
        }
#else
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, TSPCDIV_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CMLDIV_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, 4G_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, VCO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CP_EN, 0x1);
        //dis1's setting at up ramp
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, PFD_DL_DIS, 0x0);
        //dis2 = 1
        RADIO_MOD_BANK_REG(0, FMCWPLL_VCO, FMCWPLL_PFD_DL_DIS2, 0x1); //dis2
#endif
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, FMCWPLL_LOCKDIV_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, FMCWPLL_LOCKREF_EN, 0x1);
        //auto dis1
        fmcw_radio_switch_bank(radio, 3);
        RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, SEL, 0x0);
        RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, EN , 0x1);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_lo_on(fmcw_radio_t *radio, bool enable)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* enable LO's mainly block,including RX and TX */
        RADIO_MOD_BANK_REG(0, LO_EN0, LO_RXDR_STG3_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN0, LO_RXDR_STG2_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN0, LO_RXDR_STG1_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN0, DBL2_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN0, DBL1_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN0, LODR_EN, enable);

#ifdef CHIP_CASCADE
        RADIO_MOD_BANK_REG(0, LO_EN0, VCOBUFF2_EN, 0x0);
        RADIO_MOD_BANK_REG(0, LO_EN0, VCOBUFF1_EN, 0x0);
        if (enable == true) {
                if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                        RADIO_WRITE_REG(BK0_LO_EN2, 0xff);
                else
                        RADIO_WRITE_REG(BK0_LO_EN2, 0x7f);

        } else {
                        RADIO_WRITE_REG(BK0_LO_EN2, 0x0);
        }
#else
        RADIO_MOD_BANK_REG(0, LO_EN0, VCOBUFF2_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN0, VCOBUFF1_EN, enable);
#endif

        RADIO_MOD_BANK_REG(0, LO_EN1, LO_VTUNE_VAR_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG3_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG2_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG1_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG3_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG2_EN, enable);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG1_EN, enable);

        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_tx_on(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        int ch;
        //4 channels
        for (ch = 0; ch < MAX_NUM_TX; ch++) {
                RADIO_MOD_BANK_CH_REG(1, ch, TX_EN0, PADR_BIAS_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, TX_EN0, PA_BIAS_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, TX_EN0, QDAC_BIAS_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, TX_EN0, IDAC_BIAS_EN, 0x1);
        }
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_rx_on(fmcw_radio_t *radio, bool enable)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        if (enable == true)
                fmcw_radio_hp_auto_ch_off(radio,-1);
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        uint8_t ch;
        uint8_t enable_2bit = 0x3 & (enable | (enable << 1));
        //4 channels
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_RF_EN, RXLOBUFF_BIAS_EN, enable);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_RF_EN, LNA2_BIAS_EN, enable);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_RF_EN, LNA1_BIAS_EN, enable);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_RF_EN, TIA_BIAS_EN, enable);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_RF_EN, TIA_S1_EN, enable_2bit);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_RF_EN, TIA_VCTRL_EN, enable);

                RADIO_MOD_BANK_CH_REG(0, ch, RX_BB_EN, BIAS_EN, enable);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_BB_EN, VGA1_EN, enable);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_BB_EN, VGA2_EN, enable);

                /* RX gain setting to cfg value */
                if (enable == true && safety_monitor_mode == false) {
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, cfg->rf_tia_gain);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, cfg->rf_vga2_gain);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, cfg->rf_vga1_gain);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE2, HP2_SEL, cfg->rf_hpf2);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE2, HP1_SEL, cfg->rf_hpf1);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_BIAS4, VGA2_VCMSEL, 0x4);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_BIAS4, VGA1_VCMSEL, 0x4);
                }
        }
#ifndef CHIP_CASCADE  // Not ready for cascade but HPF can still work
        fmcw_radio_hp_auto_ch_on(radio,-1); // To shorten HPF set up time
#endif
        fmcw_radio_switch_bank(radio, old_bank);
#ifndef CHIP_CASCADE
        fmcw_radio_hp_auto_ch_on(radio,-1); /* re-open hp after inter_frame_power_save */
#endif
        return E_OK;
}

int32_t fmcw_radio_adc_on(fmcw_radio_t *radio)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, ADC_MUX_OUT_SEL, CLK_EN, 0x1);
        int ch;
        //4 channels ADC, vcmbuf should always kept low
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, RST, 0x0);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, BUFFER_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, BUFFER_VCMBUF_EN, 0x0);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, ANALS_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, OP1_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, OP2_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, OP3_EN, 0x1);

                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, CMP_VCALREF_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, BIAS_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, IDAC1_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, IDAC3_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, ESL_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, REFPBUF_EN, 0x1);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, REFNBUF_EN, 0x1);
        }
        fmcw_radio_switch_bank(radio, 2);

        /*400MHz and 800MHz ADC switch*/
        //400MHz  Bank0 Reg0x26 set as 0x4F,
        //Bank1 Reg 0x34, 0x43, 0x52 and 0x61 set as 0x7F;

        //800MHz  Bank0 Reg0x26 set as 0x3F,
        //Bank1 Reg0x34, 0x43, 0x52 and 0x61 set as 0xFF ;
        //Test Plan 901

        //800MHz support from AlpsB
        if (cfg->adc_freq == 20){      //400MHz, signal bandwidth 20MHz
                //400MHz
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x0);

                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_REG(1, CH0_ADC_EN0, BW20M_EN, 0x0);
                RADIO_MOD_BANK_REG(1, CH1_ADC_EN0, BW20M_EN, 0x0);
                RADIO_MOD_BANK_REG(1, CH2_ADC_EN0, BW20M_EN, 0x0);
                RADIO_MOD_BANK_REG(1, CH3_ADC_EN0, BW20M_EN, 0x0);

                fmcw_radio_switch_bank(radio, 2);
                RADIO_MOD_BANK_REG(2, CH1_ADC_FILTER, BDW_SEL, 0x0);
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, CLK_SEL, 0x0);
                //default 0x4
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, DAT_SFT_SEL, 0x4);
        }
        else if (cfg->adc_freq == 25){ //400MHz, signal bandwidth 25MHz
                //400MHz
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x0);

                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_REG(1, CH0_ADC_EN0, BW20M_EN, 0x0);
                RADIO_MOD_BANK_REG(1, CH1_ADC_EN0, BW20M_EN, 0x0);
                RADIO_MOD_BANK_REG(1, CH2_ADC_EN0, BW20M_EN, 0x0);
                RADIO_MOD_BANK_REG(1, CH3_ADC_EN0, BW20M_EN, 0x0);

                fmcw_radio_switch_bank(radio, 2);
                RADIO_MOD_BANK_REG(2, CH1_ADC_FILTER, BDW_SEL, 0x1);
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, CLK_SEL, 0x0);
                //default 0x3 for 16bits output
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, DAT_SFT_SEL, 0x3);
        }
        else if (cfg->adc_freq == 40){ //800MHz, signal bandwidth 40MHz

                //800Mhz
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);

                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_REG(1, CH0_ADC_EN0, BW20M_EN, 0x1);
                RADIO_MOD_BANK_REG(1, CH1_ADC_EN0, BW20M_EN, 0x1);
                RADIO_MOD_BANK_REG(1, CH2_ADC_EN0, BW20M_EN, 0x1);
                RADIO_MOD_BANK_REG(1, CH3_ADC_EN0, BW20M_EN, 0x1);

                fmcw_radio_switch_bank(radio, 2);
                RADIO_MOD_BANK_REG(2, CH1_ADC_FILTER, BDW_SEL, 0x0);
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, CLK_SEL, 0x1);
                //default 0x4
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, DAT_SFT_SEL, 0x4);
        }
        else if (cfg->adc_freq == 50){ //800MHz, signal bandwidth 50MHz

                //800Mhz
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);

                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_REG(1, CH0_ADC_EN0, BW20M_EN, 0x1);
                RADIO_MOD_BANK_REG(1, CH1_ADC_EN0, BW20M_EN, 0x1);
                RADIO_MOD_BANK_REG(1, CH2_ADC_EN0, BW20M_EN, 0x1);
                RADIO_MOD_BANK_REG(1, CH3_ADC_EN0, BW20M_EN, 0x1);

                fmcw_radio_switch_bank(radio, 2);
                RADIO_MOD_BANK_REG(2, CH1_ADC_FILTER, BDW_SEL, 0x1);
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, CLK_SEL, 0x1);
                //default 0x4
                RADIO_MOD_BANK_REG(2, ADC_FILTER0, DAT_SFT_SEL, 0x3);
        }

        RADIO_MOD_BANK_REG(2, ADC_FILTER0, CMOS_OUT_EN, 0x1);
        RADIO_MOD_BANK_REG(2, ADC_FILTER0, RSTN, 0x1);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_do_refpll_cal(fmcw_radio_t *radio)
{
        uint8_t old_bank;
        old_bank = fmcw_radio_switch_bank(radio, 0);
        //for refernce PLL, release reset is enough
        //Settling time is already been set by default
        RADIO_MOD_BANK_REG(0, AUTO_LOCK0, REFPLL_RSTN, 0x0);
        RADIO_MOD_BANK_REG(0, AUTO_LOCK0, REFPLL_RSTN, 0x1);
        MDELAY(15);

#ifdef CHIP_CASCADE
        /* cfg->cascade_mode must not be used here, since initializaiton is not started yet */
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                fmcw_radio_switch_bank(radio, 2);
                RADIO_MOD_BANK_REG(2, DIV_CLK, FLTR_CLK_OUT_ENABLE, 0x1);
        }
#endif
        fmcw_radio_switch_bank(radio, old_bank);
        if (fmcw_radio_is_refpll_locked(radio))
                return E_OK;
        else
                return E_REFPLL_UNLOCK;
        return E_OK;
}

int32_t fmcw_radio_do_pll_cal(fmcw_radio_t *radio, uint32_t lock_freq)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 5);// PLL bank is not considered
        //config frequency
        RADIO_WRITE_BANK_REG(5,FMCW_START_FREQ_1_0, REG_L(lock_freq));
        RADIO_WRITE_BANK_REG(5,FMCW_START_FREQ_1_1, REG_M(lock_freq));
        RADIO_WRITE_BANK_REG(5,FMCW_START_FREQ_1_2, REG_H(lock_freq));
        RADIO_WRITE_BANK_REG(5,FMCW_START_FREQ_1_3, REG_INT(lock_freq));
        //reset to 0 to clean unexpected config
        fmcw_radio_switch_bank(radio, 3);
        RADIO_WRITE_BANK_REG(3, FMCW_SYNC, 0x0);
         RADIO_WRITE_BANK_REG(3, FMCW_START, 0x0);
        //mode to 3'b000 is hold mode, single point frequency
        RADIO_WRITE_BANK_REG(3, FMCW_MODE_SEL, 0x0);

        /* During calibration sync signal is disabled */
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, EN, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, ADDER_RSTN, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x1);
        //config FMCWPLL's settling time to the largest
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, FMCWPLL_SLTSIZE0, 0xFF);
        RADIO_WRITE_BANK_REG(2, FMCWPLL_SLTSIZE1, 0xFF);
        //On 3MHz bandwidth for fast settling
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO1, LDO25_VOUT_SEL, 0x7);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LF, 3MBW_EN, 0x1);
        //start FMCWPLL's auto lock logic
        RADIO_MOD_BANK_REG(0, AUTO_LOCK0, FMCWPLL_RSTN, 0x0);
        RADIO_MOD_BANK_REG(0, AUTO_LOCK0, FMCWPLL_RSTN, 0x1);
        MDELAY(3);
        fmcw_radio_switch_bank(radio, 3);
        /* Turn off START */
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);
        //shut down 3MHz bandwidth
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LF, 3MBW_EN, 0x0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO1, LDO25_VOUT_SEL, 0x4);
        //added auto 3MHz bandwidth
        fmcw_radio_switch_bank(radio, 3);
        //set auto 3MHz bandwidth to enable at down and idle state
        RADIO_MOD_BANK_REG(3, AUTO_3MBW, SEL, 0x1);
        //enable auto 3MHz bandwidth
        RADIO_MOD_BANK_REG(3, AUTO_3MBW, EN , 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

bool fmcw_radio_is_refpll_locked(fmcw_radio_t *radio)
{
        uint8_t old_bank;
        old_bank = fmcw_radio_switch_bank(radio, 0);
        bool status = RADIO_READ_BANK_REG_FIELD(0,AUTO_LOCK1,REFPLL_LCKED);
        fmcw_radio_switch_bank(radio, old_bank);
        return status;
}

bool fmcw_radio_is_pll_locked(fmcw_radio_t *radio)
{
        uint8_t old_bank;
        old_bank = fmcw_radio_switch_bank(radio, 0);
#ifdef CHIP_ALPS_A
        bool status = RADIO_READ_BANK_REG_FIELD(0,AUTO_LOCK1,REFPLL_LCKED) &
                      RADIO_READ_BANK_REG_FIELD(0,AUTO_LOCK1,FMCWPLL_LCKED);
#elif CHIP_ALPS_MP
        //fixed analog's routing bug, analog's layout have effected auto lock's logic, confirmed with wenting
        //Not a very solid solution, expect to be solved by analog in the next T.O.
        bool status = RADIO_READ_BANK_REG_FIELD(0,AUTO_LOCK1,FMCWPLL_LCKED) ? RADIO_READ_BANK_REG_FIELD(0,AUTO_LOCK1,REFPLL_LCKED) :
                    ( RADIO_READ_BANK_REG(0, LOCK_DETECTOR2_1) == 1 &&
                      RADIO_READ_BANK_REG(0, LOCK_DETECTOR2_2) == 0 &&
                      RADIO_READ_BANK_REG(0, LOCK_DETECTOR2_3) == 0  );
#endif
        fmcw_radio_switch_bank(radio, old_bank);
        return status;
}

//enable clock to CPU
int32_t fmcw_radio_pll_clock_en(void)
{
        fmcw_radio_t *radio = NULL;
        uint8_t old_bank;
        old_bank = fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, CBC_EN, CGM_EN, 0x01);
        RADIO_MOD_BANK_REG(0, CBC_EN, LDO_EN, 0x01);
        RADIO_MOD_BANK_REG(0, CBC_EN, BG_EN, 0x01);

        RADIO_MOD_BANK_REG(0, LDO25_PMU, EN, 0x01);

        RADIO_MOD_BANK_REG(0, REFPLL_LDO1, LDO25_PLL_LDO_EN, 0x01);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO2, LDO11_VCO_LDO_EN, 0x01);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO3, LDO11_MMD_LDO_EN, 0x01);
        RADIO_MOD_BANK_REG(0, REFPLL_LDO4, LDO11_MMD2_LDO_EN, 0x01);

        RADIO_MOD_BANK_REG(0, REFPLL_LF, DIVCPU_EN, 0x01);
        RADIO_MOD_BANK_REG(0, REFPLL_LF, DIVDIG_EN, 0x01);

        fmcw_radio_switch_bank(radio, old_bank);
        fmcw_radio_refpll_on(radio);
        MDELAY(1);
        return fmcw_radio_do_refpll_cal(radio);
}

void fmcw_radio_frame_interleave_reg_write(fmcw_radio_t *radio, uint32_t fil_que, uint8_t fil_prd)
{
        /* config the frame loop registers */
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        RADIO_WRITE_BANK_REG(3, FMCW_FIL0, REG_L(fil_que));
        RADIO_WRITE_BANK_REG(3, FMCW_FIL1, REG_M(fil_que));
        RADIO_WRITE_BANK_REG(3, FMCW_FIL2, REG_H(fil_que));
        RADIO_WRITE_BANK_REG(3, FMCW_FIL3, REG_INT(fil_que));
        RADIO_WRITE_BANK_REG(3, FMCW_FIL_PRD,  fil_prd);
        if (NUM_FRAME_TYPE == 1)
                RADIO_WRITE_BANK_REG(3, FMCW_FIL_EN, 0x0);
        else
                RADIO_WRITE_BANK_REG(3, FMCW_FIL_EN, 0x1);
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_frame_interleave_pattern(fmcw_radio_t *radio, uint8_t frame_loop_pattern)
{
        /* set frame interleave pattern */
        uint32_t  fil_que   = frame_loop_pattern;
        uint32_t  fil_prd   = 0;

        fmcw_radio_frame_interleave_reg_write(radio, fil_que, fil_prd);
}

void fmcw_radio_frame_type_reset(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        RADIO_WRITE_BANK_REG(3, FMCW_FIL_EN, 0x0); // reset asserted
        RADIO_WRITE_BANK_REG(3, FMCW_FIL_EN, 0x1); // reset deasserted
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_generate_fmcw(fmcw_radio_t *radio)
{
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
#ifndef CHIP_CASCADE
        fmcw_radio_hp_auto_ch_on(radio,-1);
#endif
        /* stop fmcw before programming */
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x0);
        /* tx phase */
        if ((radio->frame_type_id) == 0) {
                uint32_t lock_freq = fmcw_radio_compute_lock_freq(radio);
                fmcw_radio_do_pll_cal(radio, lock_freq); /* only run in frame_type 0, do not run in other cases*/
                /*Configure all Txs Phase Shifter */
                uint32_t ch, reg_val;
                for (ch = 0; ch < MAX_NUM_TX; ch++) /* config one time*/
                {
                    reg_val = phase_val_2_reg_val(cfg->tx_phase_value[ch]);
                    fmcw_radio_set_tx_phase(radio, ch, reg_val);
                }
        }
        /* chirp parameters */
        fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_1_0,   REG_L(radio->start_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_1_1,   REG_M(radio->start_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_1_2,   REG_H(radio->start_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_1_3,   REG_INT(radio->start_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_1_0,    REG_L(radio->stop_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_1_1,    REG_M(radio->stop_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_1_2,    REG_H(radio->stop_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_1_3,    REG_INT(radio->stop_freq));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_1_0, REG_L(radio->step_up));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_1_1, REG_M(radio->step_up));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_1_2, REG_H(radio->step_up));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_1_3, REG_INT(radio->step_up));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_1_0, REG_L(radio->step_down));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_1_1, REG_M(radio->step_down));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_1_2, REG_H(radio->step_down));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_1_3, REG_INT(radio->step_down));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_1_0,         REG_L(radio->cnt_wait));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_1_1,         REG_M(radio->cnt_wait));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_1_2,         REG_H(radio->cnt_wait));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_1_3,         REG_INT(radio->cnt_wait));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CHIRP_SIZE_1_0,   REG_L(radio->nchirp));
        fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CHIRP_SIZE_1_1,   REG_M(radio->nchirp));
        fmcw_radio_switch_bank(radio, 3);

        /* MIMO */

        /*turn off VAM mode,ps mode,agc mode bofore turning on to protection switch modes without power off the chip*/
        fmcw_radio_special_mods_off(radio); /* only clear one time */

        /* virtual array settings */
        if (cfg->nvarray >= 2) {
                fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
                for (int ch = 0; ch < MAX_NUM_TX; ch++) {
                        if (cfg->tx_groups[ch]>0) {
                                // set VAM config
                                uint8_t vam_cfg = 0;
                                /*Bug fix for anti velamb glitch*/
                                if (cfg->anti_velamb_en)
                                        vam_cfg = ( 0x41 | (cfg->anti_velamb_en << 3) | (( cfg->nvarray - 1 ) << 1 ));
                                else
                                        vam_cfg = ( 0x61 | (cfg->anti_velamb_en << 3) | (( cfg->nvarray - 1 ) << 1 ));
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_2 + ch * 3, vam_cfg);

                                /* set VAM group patten*/
                                uint16_t bit_mux[MAX_NUM_TX] = {0,0,0,0};
                                bit_parse(cfg->tx_groups[ch], bit_mux);
                                uint8_t bit_mux_all = 0;
                                /*Bug fix for anti velamb glitch*/
                                if (cfg->anti_velamb_en == false)
                                        bit_mux_all = bit_mux[0]<<6 | bit_mux[1]<<4 | bit_mux[2]<< 2 | bit_mux[3];
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_1 + ch * 3, bit_mux_all);
                        } else {
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_TX0_CTRL_1_2 + ch * 3, 0); // clear 0
                        }
                }
        }

        /* phase scramble config */
        if (cfg->phase_scramble_on) {
                fmcw_radio_switch_bank(radio, 3);
                RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP0, REG_L(cfg->phase_scramble_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP1, REG_M(cfg->phase_scramble_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP2, REG_H(cfg->phase_scramble_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_TAP3, REG_INT(cfg->phase_scramble_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE0, REG_L(cfg->phase_scramble_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE1, REG_M(cfg->phase_scramble_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE2, REG_H(cfg->phase_scramble_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_PS_STATE3, REG_INT(cfg->phase_scramble_init_state));

                if(cfg->phase_scramble_on & 0x2)
                        RADIO_WRITE_BANK_REG(3, PS_RM, 0);
                /* Phase scramble configureation for group 1 */
                fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_PS_EN_1, 0x1);  /*enable phase scramble*/
         }

        /* frequency hopping config */
        if (cfg->freq_hopping_on) {
                /*config XOR chain*/
                fmcw_radio_switch_bank(radio, 3);
                RADIO_WRITE_BANK_REG(3, FMCW_HP_TAP0, REG_L(cfg->freq_hopping_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_TAP1, REG_M(cfg->freq_hopping_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_TAP2, REG_H(cfg->freq_hopping_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_TAP3, REG_INT(cfg->freq_hopping_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_STATE0, REG_L(cfg->freq_hopping_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_STATE1, REG_M(cfg->freq_hopping_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_STATE2, REG_H(cfg->freq_hopping_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_HP_STATE3, REG_INT(cfg->freq_hopping_init_state));

                if(cfg->freq_hopping_on & 0x2)
                        RADIO_WRITE_BANK_REG(3, HP_RM, 0);

                /*config freq parameters*/
                fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_HP_1_0, REG_L(radio->hp_start_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_HP_1_1, REG_M(radio->hp_start_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_HP_1_2, REG_H(radio->hp_start_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_START_FREQ_HP_1_3, REG_INT(radio->hp_start_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_HP_1_0, REG_L(radio->hp_stop_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_HP_1_1, REG_M(radio->hp_stop_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_HP_1_2, REG_H(radio->hp_stop_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STOP_FREQ_HP_1_3, REG_INT(radio->hp_stop_freq));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_HP_1_0, REG_L(radio->step_up));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_HP_1_1, REG_M(radio->step_up));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_HP_1_2, REG_H(radio->step_up));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_UP_FREQ_HP_1_3, REG_INT(radio->step_up));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_HP_1_0, REG_L(radio->step_down));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_HP_1_1, REG_M(radio->step_down));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_HP_1_2, REG_H(radio->step_down));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_STEP_DN_FREQ_HP_1_3, REG_INT(radio->step_down));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_HP_1_0, REG_L(radio->cnt_wait));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_HP_1_1, REG_M(radio->cnt_wait));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_HP_1_2, REG_H(radio->cnt_wait));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_IDLE_HP_1_3, REG_INT(radio->cnt_wait));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_HP_EN_1, 0x1);
        }

        /* anti velocity deambiguity config */
        if (cfg->anti_velamb_en == true) {
                fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
                fmcw_radio_reg_write(radio,RADIO_BK5_FMCW_CS_AVA_DLY_1_0, REG_L(radio->anti_velamb_cycle));
                fmcw_radio_reg_write(radio,RADIO_BK5_FMCW_CS_AVA_DLY_1_1, REG_M(radio->anti_velamb_cycle));
                fmcw_radio_reg_write(radio,RADIO_BK5_FMCW_CS_AVA_DLY_1_2, REG_H(radio->anti_velamb_cycle));
                fmcw_radio_reg_write(radio,RADIO_BK5_FMCW_CS_AVA_DLY_1_3, REG_INT(radio->anti_velamb_cycle));

                /* configure extra chirp */
                for (int ch = 0; ch < MAX_NUM_TX; ch++) {
                        if (cfg->tx_groups[ch] & 0xF) {
                                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CS_AVA_EN_1, ( ch << 2 ) | 0x1);
                                break;
                        }
                }
                fmcw_radio_cmd_cfg(radio);
        } else {
                fmcw_radio_reg_write(radio,RADIO_BK5_FMCW_CS_AVA_EN_1, 0x0);
        }

        /* chirp shifting config */
        if (cfg->chirp_shifting_on) {
                fmcw_radio_switch_bank(radio, 3);
                RADIO_WRITE_BANK_REG(3, FMCW_CS_TAP0, REG_L(cfg->chirp_shifting_init_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_TAP1, REG_M(cfg->chirp_shifting_init_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_TAP2, REG_H(cfg->chirp_shifting_init_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_TAP3, REG_INT(cfg->chirp_shifting_init_tap));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_STATE0, REG_L(cfg->chirp_shifting_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_STATE1, REG_M(cfg->chirp_shifting_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_STATE2, REG_H(cfg->chirp_shifting_init_state));
                RADIO_WRITE_BANK_REG(3, FMCW_CS_STATE3, REG_INT(cfg->chirp_shifting_init_state));

                if(cfg->chirp_shifting_on & 0x2)
                        RADIO_WRITE_BANK_REG(3, CS_RM, 0);

                fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CS_DLY_1_0, REG_L(radio->chirp_shifting_cyle));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CS_DLY_1_1, REG_M(radio->chirp_shifting_cyle));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CS_DLY_1_2, REG_H(radio->chirp_shifting_cyle));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CS_DLY_1_3, REG_INT(radio->chirp_shifting_cyle));
                fmcw_radio_reg_write(radio, RADIO_BK5_FMCW_CS_EN_1, 0x1);
        }

        fmcw_radio_switch_bank(radio, 3);
        //MODE_SEL
        //0x00 hold
        //0x01 realtime
        //0x02 predefined
        //0x05 bypass_SDM
        RADIO_WRITE_BANK_REG(3, FMCW_MODE_SEL, 0x2);
        /* Only need to reset once at beginning */
        RADIO_MOD_BANK_REG(3, FMCW_START, ADDER_RSTN, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, ADDER_RSTN, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, EN, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, SEL, 0x3);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC_DLY, SYNC_DLY_EN, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, EN, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x0);
        /* TODO: auto tx off function */
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_start_fmcw(fmcw_radio_t *radio)
{
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;

        /* set txlo buffer by tx groups status */
        fmcw_radio_txlobuf_on(radio);

        if (cfg->nvarray >= 2){
                int ch;
                /* switch SPI source */
                radio_spi_cmd_mode(RADIO_SPI_CMD_CPU);
                if (cfg->bpm_mode==true) {
                        for (ch = 0; ch < MAX_NUM_TX; ch++) {
                                if (cfg->tx_groups[ch] > 0)
                                        fmcw_radio_set_tx_status(radio, ch, 0xf);
                        }
                } else {
                        for (ch = 0; ch < MAX_NUM_TX; ch++)
                                //fmcw_radio_set_tx_status(radio, ch, 0x0);
                                fmcw_radio_set_tx_status(radio, ch, 0xF);
                }
        }
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        /* Reset before start */
        RADIO_WRITE_BANK_REG(3, FMCW_MODE_SEL, 0x2);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, ADDER_RSTN, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);

        /* DMU triggers FMCW_START */
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SEL, 1);
        fmcw_radio_switch_bank(radio, old_bank);
        /* Enable CMD */
        raw_writel(RADIO_SPI_CMD_SRC_SEL, RADIO_SPI_CMD_FMCW  );
        raw_writel(REG_DMU_FMCW_START + REL_REGBASE_DMU, 1);

        fmcw_radio_switch_bank(radio, old_bank);

}

void fmcw_radio_stop_fmcw(fmcw_radio_t *radio)
{
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        if (cfg->nvarray >= 2 || cfg->phase_scramble_on == true ){
                radio_spi_cmd_mode(RADIO_SPI_CMD_CPU);
        }
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        RADIO_MOD_BANK_REG(3,FMCW_START,START_SPI,0x0);
        fmcw_radio_switch_bank(radio, old_bank);
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : RX_TIA_133omhs:0xF / RX_TIA_250ohms:0x1 / RX_TIA_500ohms:0x2 / RX_TIA_1000ohms:0x4 / RX_TIA_2000ohms:0x8
 */
void fmcw_radio_set_tia_gain(fmcw_radio_t *radio,
                             int32_t channel_index, int32_t gain)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch;
        if (channel_index != -1)
                RADIO_MOD_BANK_CH_REG(0, channel_index, RX_TUNE0, TIA_RFB_SEL, gain);
        else {
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, gain);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_tia_gain(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        char rx_gain = RADIO_READ_BANK_CH_REG_FIELD(0, channel_index, RX_TUNE0, TIA_RFB_SEL);
        fmcw_radio_switch_bank(radio, old_bank);
        return rx_gain;
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : RX_VGA1_6dB:0x01 / RX_VGA1_9dB:0x02 / RX_VGA1_12dB:0x03 /
 *                 RX_VGA1_15dB:0x04 / RX_VGA1_18dB:0x05 / RX_VGA1_21dB:0x06
 */
void fmcw_radio_set_vga1_gain(fmcw_radio_t *radio, int32_t channel_index, int32_t gain)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch;
        if (channel_index != -1)
                RADIO_MOD_BANK_CH_REG(0, channel_index, RX_TUNE1, VGA1_GAINSEL, gain);
        else {
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, gain);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_vga1_gain(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        char rx_gain = RADIO_READ_BANK_CH_REG_FIELD(0, channel_index, RX_TUNE1, VGA1_GAINSEL);
        fmcw_radio_switch_bank(radio, old_bank);
        return rx_gain;
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : RX_VGA1_5dB:0x01 / RX_VGA1_8dB:0x02 / RX_VGA1_11dB:0x03 /
 *                 RX_VGA1_14dB:0x04 / RX_VGA1_16dB:0x05 / RX_VGA1_19dB:0x06
 */
void fmcw_radio_set_vga2_gain(fmcw_radio_t *radio, int32_t channel_index, char gain)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch;
        if (channel_index != -1)
                RADIO_MOD_BANK_CH_REG(0, channel_index, RX_TUNE1, VGA2_GAINSEL, gain);
        else {
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, gain);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_vga2_gain(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        char rx_gain = RADIO_READ_BANK_CH_REG_FIELD(0, channel_index, RX_TUNE1, VGA2_GAINSEL);
        fmcw_radio_switch_bank(radio, old_bank);
        return rx_gain;
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : TX_ON:0xF / TX_OFF:0x0
 */
void fmcw_radio_set_tx_status(fmcw_radio_t *radio, int32_t channel_index, char status)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        int ch;
        if (channel_index != -1)
                RADIO_WRITE_BANK_CH_REG(1, channel_index, TX_EN0, status);
        else {
                for(ch = 0; ch < MAX_NUM_TX; ch++)
                        RADIO_WRITE_BANK_CH_REG(1, ch, TX_EN0, status);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_tx_status(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank;
        char tx_status;
        old_bank = fmcw_radio_switch_bank(radio, 1);
        tx_status = RADIO_READ_BANK_CH_REG(1, channel_index, TX_EN0);
        fmcw_radio_switch_bank(radio, old_bank);
        return tx_status;
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : TX_POWER_DEFAULT:0xAA / TX_POWER_MAX:0xFF / TX_POWER_MAX_SUB5:0x88 / TX_POWER_MAX_SUB10:0x0
 */
//TO-DO: added power_index_0 and power_index_1
void fmcw_radio_set_tx_power(fmcw_radio_t *radio,
                             int32_t channel_index, char power_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        int ch;
        if (channel_index != -1){
                RADIO_WRITE_BANK_CH_REG(1, channel_index, TX_BIAS0, power_index);
                RADIO_WRITE_BANK_CH_REG(1, channel_index, TX_BIAS1, power_index);
        }
        else {
                for(ch = 0; ch < MAX_NUM_TX; ch++){
                        RADIO_WRITE_BANK_CH_REG(1, ch, TX_BIAS0, power_index);
                        RADIO_WRITE_BANK_CH_REG(1, ch, TX_BIAS1, power_index);
                }
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

//TO-DO : added for TX_BIAS1 ???
int32_t fmcw_radio_get_tx_power(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        char tx_power = RADIO_READ_BANK_CH_REG(1, channel_index, TX_BIAS0);
        fmcw_radio_switch_bank(radio, old_bank);
        return tx_power;
}

/*
 * channel_index : CH0 / CH0 / CH1 / CH2 / CH_ALL
 *      #define TX_PHASE_0       0x0f0fu
 *      #define TX_PHASE_45      0x000fu
 *      #define TX_PHASE_90      0x1f0fu
 *      #define TX_PHASE_135     0x1f00u
 *      #define TX_PHASE_180     0x1f1fu
 *      #define TX_PHASE_225     0x001fu
 *      #define TX_PHASE_270     0x0f1fu
 *      #define TX_PHASE_315     0x0f00u
 */
uint32_t phase_val_2_reg_val(uint32_t phase_val)
{
        uint32_t reg_val = 0x0;
        switch (phase_val) {
        case 0:
                reg_val = TX_PHASE_0;
                break;
        case 45:
                reg_val = TX_PHASE_45;
                break;
        case 90:
                reg_val = TX_PHASE_90;
                break;
        case 135:
                reg_val = TX_PHASE_135;
                break;
        case 180:
                reg_val = TX_PHASE_180;
                break;
        case 225:
                reg_val = TX_PHASE_225;
                break;
        case 270:
                reg_val = TX_PHASE_270;
                break;
        case 315:
                reg_val = TX_PHASE_315;
                break;
        default:
                reg_val = 0x0;
                break;
        }
        return reg_val;
}
uint32_t reg_val_2_phase_val(uint32_t reg_val)
{
        uint32_t phase_val = 0xfff;
        switch (reg_val) {
        case TX_PHASE_0:
                phase_val = 0;
                break;
        case TX_PHASE_45:
                phase_val = 45;
                break;
        case TX_PHASE_90:
                phase_val = 90;
                break;
        case TX_PHASE_135:
                phase_val = 135;
                break;
        case TX_PHASE_180:
                phase_val = 180;
                break;
        case TX_PHASE_225:
                phase_val = 225;
                break;
        case TX_PHASE_270:
                phase_val = 270;
                break;
        case TX_PHASE_315:
                phase_val = 315;
                break;
        default:
                phase_val = 0xfff;
                break;
        }
        return phase_val;
}

void fmcw_radio_set_tx_phase(fmcw_radio_t *radio, int32_t channel_index, int32_t phase_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        char tx_phase_i = (char) (phase_index >> 8);
        char tx_phase_q = (char) phase_index;
        int ch;
        if (channel_index != -1) {
                RADIO_WRITE_BANK_CH_REG(1, channel_index, TX_TUNE0, tx_phase_i);
                RADIO_WRITE_BANK_CH_REG(1, channel_index, TX_TUNE1, tx_phase_q);
        } else {
                for(ch = 0; ch < MAX_NUM_TX; ch++) {
                        RADIO_WRITE_BANK_CH_REG(1, ch, TX_TUNE0, tx_phase_i);
                        RADIO_WRITE_BANK_CH_REG(1, ch, TX_TUNE1, tx_phase_q);
                }
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_tx_phase(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        int32_t tx_phase_index = RADIO_READ_BANK_CH_REG(1, channel_index, TX_TUNE0) << 8 |
                RADIO_READ_BANK_CH_REG(1, channel_index, TX_TUNE1);
        fmcw_radio_switch_bank(radio, old_bank);
        return tx_phase_index;
}

//No need to judge if read is greater than 128 or not
float fmcw_radio_get_temperature(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        /* Enable 800M ADC CLK */
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
        /* Enable safety monitor LDO */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);
        /* Enable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        /* AUXADC2 on and reset */
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BUF_BYPASS, 0x0);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BUF_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BIAS_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_OP1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_OP2_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_REFGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_VCMGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_RST, 0x1);

        /* load trimming value from the corresponding eFuse address */
        fmcw_radio_auxadc_trim(radio);
        MDELAY(2);

        /* select AUXADC2 InputMUX */
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, TS_BG_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, TS_VPTAT_CAL, 0x4);
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, DTSDM_MUXIN_SEL, 0x0);

        /* AUXADC2 de-reset */
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_RST, 0x0);
        /* AUXADC2 Filter de-reset */
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, DC_FILTER2_RST_EN, 0x01);
        MDELAY(2);

        /* read back AUXADC2 Filter Output Digital Bits */
        fmcw_radio_switch_bank(radio, 1);
        uint8_t doutL, doutM, doutH;
        float dout, radio_temp;
        doutL =  RADIO_READ_BANK_REG(1, DTSDM2_DAT0);
        doutM =  RADIO_READ_BANK_REG(1, DTSDM2_DAT1);
        doutH =  RADIO_READ_BANK_REG(1, DTSDM2_DAT2);
        dout = doutL + ( doutM << 8 ) + ( doutH << 16 );

        /* return voltage measurement, formula refer to 125C */
        radio_temp = ((float)(dout) - 108058.0) / 116.5 + 125;

        /* Disable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
        return radio_temp;
}

/* gain_compensation is used under high ambient temperatures */
#if HTOL_TEST == 1
void fmcw_radio_gain_compensation(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch;
        RADIO_MOD_BANK_REG(0, LO_LDO0, LDO11_LOMAINPATH_VOUT_SEL, 0x6);
        RADIO_MOD_BANK_REG(0, LO_LDO1, LDO11_LOMUXIO_VOUT_SEL, 0x7);
        RADIO_MOD_BANK_REG(0, LO_LDO2, LDO11_TXLO_VOUT_SEL, 0x6);
        RADIO_MOD_BANK_REG(0, LO_LDO3, LDO11_RXLO_VOUT_SEL, 0x7);
        RADIO_MOD_BANK_REG(0, RX_LDO0, LDO11_RFN_VOUT_SEL, 0x5);
        RADIO_MOD_BANK_REG(0, RX_LDO1, LDO11_RFS_VOUT_SEL, 0x5);
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, TX_LDO0, LDO11_TX0_VOUT_SEL, 0x5);
        RADIO_MOD_BANK_REG(1, TX_LDO1, LDO11_TX1_VOUT_SEL, 0x5);
        RADIO_MOD_BANK_REG(1, TX_LDO2, LDO11_TX2_VOUT_SEL, 0x5);
        RADIO_MOD_BANK_REG(1, TX_LDO3, LDO11_TX3_VOUT_SEL, 0x5);
        for(ch = 0; ch < MAX_NUM_RX; ch++)
                RADIO_MOD_BANK_CH_REG(1, ch, PA_LDO, LDO11_TX_PA_VOUT_SEL, 0xd);
        fmcw_radio_switch_bank(radio, old_bank);
}
#endif /* gain_compensation */

void fmcw_radio_power_on(fmcw_radio_t *radio)
{
        fmcw_radio_ldo_on(radio);
        fmcw_radio_pll_on(radio);
        fmcw_radio_lo_on(radio, true);
        /*set TX power as default:0xAA */
        fmcw_radio_set_tx_power(radio, -1, TX_POWER_DEFAULT);
        fmcw_radio_tx_on(radio);
        fmcw_radio_rx_on(radio, true);
        fmcw_radio_rc_calibration(radio);
        fmcw_radio_adc_cmp_calibration(radio);
        fmcw_radio_adc_on(radio);
        fmcw_radio_auxadc_trim(radio);
}

void fmcw_radio_power_off(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        /* FMCWPLL disable */
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, TSPCDIV_EN, 0x0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CMLDIV_EN, 0x0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, VCO_EN, 0x0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CP_EN, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
}

bool fmcw_radio_is_running(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        uint8_t mode = RADIO_READ_BANK_REG(3, FMCW_MODE_SEL);
        uint8_t start = RADIO_READ_BANK_REG_FIELD(3, FMCW_START, START_SPI);
        fmcw_radio_switch_bank(radio, 5  + radio->frame_type_id);
        uint8_t chirp_size_1_0 = RADIO_READ_BANK_REG(5, FMCW_CHIRP_SIZE_1_0);
        uint8_t chirp_size_1_1 = RADIO_READ_BANK_REG(5, FMCW_CHIRP_SIZE_1_1);
        fmcw_radio_switch_bank(radio, old_bank);
        bool ret = false;
        if (mode == 0x2)
                ret = (start == 1 && !chirp_size_1_0 && !chirp_size_1_1 ) ? true : false;
        else if (mode == 0x3)
                ret = false;
        return ret;
}

int32_t fmcw_radio_vctrl_on(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        //enable PLL's local switch
        RADIO_MOD_BANK_REG(0, FMCWPLL_LF, VTR_TEST_SW_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LF, VTR_TEST_EN, 0x1);
        //enable TP_ANA1 and mux VCTRL to output
        RADIO_MOD_BANK_REG(0, TPANA1, EN, 0x1);
        RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_EN, 0x1);
        RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_SEL, 0x38);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

int32_t fmcw_radio_vctrl_off(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        //disable PLL's local switch
        RADIO_MOD_BANK_REG(0, FMCWPLL_LF, VTR_TEST_SW_EN, 0x0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LF, VTR_TEST_EN, 0x0);
        //disable TP_ANA1
        RADIO_MOD_BANK_REG(0, TPANA1, EN, 0x0);
        RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_EN, 0x0);
        RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_SEL, 0x00);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 */
void fmcw_radio_if_output_on(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 2);
        int ch;
        RADIO_MOD_BANK_REG(2, LVDS_LDO25, LDO25_LVDS_LDO_EN, 0x1);
        if (channel_index != -1) {
                RADIO_MOD_BANK_CH_REG(2, channel_index, BUFFER, EN, 0x1);
        } else {
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(2, ch, BUFFER, EN, 0x1);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

/* enable buffer will output IF to output pad, only used for debug */
void fmcw_radio_if_output_off(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 2);
        int ch;
        if (channel_index != -1) {
                RADIO_MOD_BANK_CH_REG(2, channel_index, BUFFER, EN, 0x0);
        } else {
                RADIO_MOD_BANK_REG(2, LVDS_LDO25, LDO25_LVDS_LDO_EN, 0x0);
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(2, ch, BUFFER, EN, 0x0);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

//control TX's LDO and PA's LDO
void fmcw_radio_tx_ch_on(fmcw_radio_t *radio, int32_t channel_index, bool enable)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        uint8_t enable_4bit, enable_8bit;
        if (enable == true) {
                enable_4bit = 0x0f;
                enable_8bit = 0xff;
        } else {
                enable_4bit = 0x00;
                enable_8bit = 0x00;
        }
        if (channel_index == -1) {
                RADIO_WRITE_BANK_REG(1, TX_LDO_EN, enable_8bit);
                for(uint8_t ch = 0; ch < MAX_NUM_TX; ch++)
                        RADIO_WRITE_BANK_CH_REG(1, ch, TX_EN0, enable_4bit);
                //Wait to add after VAM is done
        } else if (channel_index == 0) {
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_EN, enable);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX0_PA_EN, enable);
                RADIO_WRITE_BANK_REG(1, CH0_TX_EN0, enable_4bit);
        } else if (channel_index == 1) {
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_EN, enable);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX1_PA_EN, enable);
                RADIO_WRITE_BANK_REG(1, CH1_TX_EN0, enable_4bit);
        } else if (channel_index == 2) {
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_EN, enable);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX2_PA_EN, enable);
                RADIO_WRITE_BANK_REG(1, CH2_TX_EN0, enable_4bit);
        } else if (channel_index == 3) {
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_EN, enable);
                RADIO_MOD_BANK_REG(1, TX_LDO_EN, LDO11_TX3_PA_EN, enable);
                RADIO_WRITE_BANK_REG(1, CH3_TX_EN0, enable_4bit);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

static void fmcw_radio_tx_restore_proc(fmcw_radio_t *radio)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        bool enable;
        for(uint8_t ch = 0; ch < MAX_NUM_TX; ch++) {
                enable = !(cfg->tx_groups[ch] == 0);
                fmcw_radio_tx_ch_on(NULL, ch, enable);
        }
}

void fmcw_radio_tx_restore(fmcw_radio_t *radio)
{
#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                fmcw_radio_tx_restore_proc(radio);
#else
                fmcw_radio_tx_restore_proc(radio);
#endif
}

void fmcw_radio_loop_test_en(fmcw_radio_t *radio, bool enable)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 9);
        int val = (enable == true)?1:0;
        RADIO_WRITE_BANK_REG(9, LP_TST_EN, val);
        /* reset SDM */
        fmcw_radio_switch_bank(radio, 3);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);
        /* In loop test, rstn_fmcw_gen must be 0 */
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, !val);
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_dac_reg_cfg_outer(fmcw_radio_t *radio) /* outer circle */
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 9);
        int ch;
        /* turn on dac */
        RADIO_WRITE_BANK_REG(9, LP_TST_EN1, 0x7F);
        RADIO_WRITE_BANK_REG(9, LP_TST_EN2, 0xBF);
        RADIO_WRITE_BANK_REG(9, LP_TST_EN3, 0x00);
        RADIO_WRITE_BANK_REG(9, DAC_LP_TST, 0x11);
        for(ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_WRITE_BANK_CH_REG(9, ch, RXBB, 0x01);
        }
        fmcw_radio_switch_bank(radio, 2);
        RADIO_MOD_BANK_REG(2, BIST_EN1, BIST_EN_SPARE, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_dac_reg_restore(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 2);
        uint8_t ch; /* channel index*/
        RADIO_WRITE_BANK_REG(2, BIST_LDO, 0x40);
        RADIO_WRITE_BANK_REG(2, BIST_EN0, 0x00);
        RADIO_WRITE_BANK_REG(2, BIST_EN1, 0x00);
        RADIO_WRITE_BANK_REG(2, DAC_EN, 0x10);
        fmcw_radio_switch_bank(radio, 0);
        for(ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_WRITE_BANK_CH_REG(0, ch, RX_TEST, 0x1);
                RADIO_WRITE_BANK_CH_REG(0, ch, RX_BIAS0, 0x8);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_dac_reg_cfg_inner(fmcw_radio_t *radio, uint8_t inject_num, uint8_t out_num) /* inner circle */
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 9);
        uint8_t ch; /* channel index*/
        /* turn on dac */
        RADIO_WRITE_BANK_REG(9, LP_TST_EN1, 0x70);
        RADIO_WRITE_BANK_REG(9, LP_TST_EN2, 0xC0);
        RADIO_WRITE_BANK_REG(9, DAC_LP_TST, 0x11);

        for(ch = 0; ch < MAX_NUM_RX; ch++) {
                /* inject choice */
                switch (inject_num) {
                case 0: /* TIA injected*/
                        RADIO_WRITE_BANK_REG(9, LP_TST_EN3, 0xFF);
                        break;
                case 1: /* HPF1 injected*/
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, TSTMUX_SEL_LP_TST, 0x8);
                        break;
                case 2: /* VGA1 injected*/
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, TSTMUX_SEL_LP_TST, 0x4);
                        break;
                case 3: /* HPF2 injected*/
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, TSTMUX_SEL_LP_TST, 0x2);
                        break;
                case 4: /* VGA2 injected*/
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, TSTMUX_SEL_LP_TST, 0x1);
                        break;
                default: /* HPF1 injected*/
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, TSTMUX_SEL_LP_TST, 0x8);
                        break;
                }

                /* out choice */
                switch (out_num) {
                case 0: /* TIA out */
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, OUTMUX_SEL_LP_TST, 0x8);
                        break;
                case 1: /* HPF1 out */
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, OUTMUX_SEL_LP_TST, 0x4);
                        break;
                case 2: /* VGA1 out */
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, OUTMUX_SEL_LP_TST, 0x2);
                        break;
                case 3: /* VGA2 out */
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, OUTMUX_SEL_LP_TST, 0x1);
                        break;
                default: /* VGA2 out */
                        RADIO_MOD_BANK_CH_REG(9, ch, RXBB, OUTMUX_SEL_LP_TST, 0x1);
                        break;
                }
        }

        fmcw_radio_switch_bank(radio, 2);
        RADIO_MOD_BANK_REG(2, BIST_EN1, BIST_EN_SPARE, 0x1);
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_dc_reg_cfg(fmcw_radio_t *radio, int32_t channel_index, int16_t dc_offset, bool dc_calib_print_ena)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 2);
        int16_t dc_offset_reverse;
        uint8_t dc_high8 = 0;
        uint8_t dc_low8  = 0;
        uint8_t old_dc_high8 = 0;
        uint8_t old_dc_low8  = 0;
        //readback bandwidth sel from chip
        uint8_t bdw = RADIO_READ_BANK_REG_FIELD(2,CH1_ADC_FILTER,BDW_SEL);
        /* change to 2 values */
        /* write FMCW registers(15 bits), but the MSB 3 bits are all sign bits*/
        /* dc_low8 using 8 bits, dc_high8 using 7 bits */
        if ( bdw == 0 ){
                dc_offset_reverse = (-dc_offset) / 4; /* adc data has left-shift 2 bits in RTL, so / 4*/
        }
        else{
                dc_offset_reverse = (-dc_offset) / 8; /* adc data has left-shift 1 bits in RTL, so / 2*/
        }

        dc_high8 = REG_H7(dc_offset_reverse); /* mask the high 7 bits*/
        dc_low8  = REG_L(dc_offset_reverse);  /* mask the low  8 bits*/

        if (0 == radio->frame_type_id) { // Store dc_offset value to default common register
                /* read */
                old_dc_low8  = RADIO_READ_BANK_CH_REG(2, channel_index, FILTER_DC_CANCEL_1);
                old_dc_high8 = RADIO_READ_BANK_CH_REG(2, channel_index, FILTER_DC_CANCEL_2);
                /* write */
                RADIO_WRITE_BANK_CH_REG(2, channel_index, FILTER_DC_CANCEL_1, dc_low8);
                RADIO_WRITE_BANK_CH_REG(2, channel_index, FILTER_DC_CANCEL_2, dc_high8);
        }
        // Store dc_offset value to banked registers which located in radio bank 5, 6, 7, 8
        uint8_t old_bank1 = fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        /* read */
        // The different banked registers share the same address values, so choose bank5 address values
        old_dc_low8  = RADIO_READ_BANK_CH_REG(5, channel_index, FILTER_DC_CANCEL_1_1);
        old_dc_high8 = RADIO_READ_BANK_CH_REG(5, channel_index, FILTER_DC_CANCEL_1_2);
        /* write */
        RADIO_WRITE_BANK_CH_REG(5, channel_index, FILTER_DC_CANCEL_1_1, dc_low8);
        RADIO_WRITE_BANK_CH_REG(5, channel_index, FILTER_DC_CANCEL_1_2, dc_high8);
        fmcw_radio_switch_bank(radio, old_bank1);
        /* check */
        if(dc_calib_print_ena == 1){
                EMBARC_PRINTF("frame type %d: channel %d before modification, old_dc_high8 = 0x%x, old_dc_low8 = 0x%x\n", radio->frame_type_id, channel_index, old_dc_high8, old_dc_low8);
                EMBARC_PRINTF("frame type %d: channel %d after modification,  new_dc_high8 = 0x%x, new_dc_low8 = 0x%x\n", radio->frame_type_id, channel_index, dc_high8, dc_low8);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_adc_cmp_calibration(fmcw_radio_t *radio)
{
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* Enable 400M ADC CLK and ADC Local CLK*/
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x0);
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, ADC_MUX_OUT_SEL, CLK_EN, 0x1);

        /* LDO ON */
        RADIO_MOD_BANK_REG(1, ADC_LDO0, LDO11_ADC12_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO1, LDO12_ADC12_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO2, LDO25_ADC12_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO3, LDO11_ADC34_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO4, LDO12_ADC34_EN, 0x1);
        RADIO_MOD_BANK_REG(1, ADC_LDO5, LDO25_ADC34_EN, 0x1);
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, RX_LDO0, LDO11_RFN_EN, 0x1);
        RADIO_MOD_BANK_REG(0, RX_LDO1, LDO11_RFS_EN, 0x1);
        RADIO_MOD_BANK_REG(0, RX_LDO2, LDO25_BBN_EN, 0x1);
        RADIO_MOD_BANK_REG(0, RX_LDO3, LDO25_BBS_EN, 0x1);

        /* RXBB outmux de-selsection */
        RADIO_MOD_BANK_REG(0,CH0_RX_TEST,OUTMUX_SEL,0x0);
        RADIO_MOD_BANK_REG(0,CH1_RX_TEST,OUTMUX_SEL,0x0);
        RADIO_MOD_BANK_REG(0,CH2_RX_TEST,OUTMUX_SEL,0x0);
        RADIO_MOD_BANK_REG(0,CH3_RX_TEST,OUTMUX_SEL,0x0);

        /* Enable ADC and Reset */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_WRITE_BANK_REG(1, CH0_ADC_EN0, 0x7F);
        RADIO_WRITE_BANK_REG(1, CH0_ADC_EN1, 0xCE);
        RADIO_WRITE_BANK_REG(1, CH1_ADC_EN0, 0x7F);
        RADIO_WRITE_BANK_REG(1, CH1_ADC_EN1, 0xCE);
        RADIO_WRITE_BANK_REG(1, CH2_ADC_EN0, 0x7F);
        RADIO_WRITE_BANK_REG(1, CH2_ADC_EN1, 0xCE);
        RADIO_WRITE_BANK_REG(1, CH3_ADC_EN0, 0x7F);
        RADIO_WRITE_BANK_REG(1, CH3_ADC_EN1, 0xCE);

        /* ADC CMP Calibration Pre-Setting */
        RADIO_WRITE_BANK_REG(1, CH0_ADC_TUNE10, 0x29);
        RADIO_WRITE_BANK_REG(1, CH0_ADC_TUNE11, 0x60);
        RADIO_WRITE_BANK_REG(1, CH1_ADC_TUNE10, 0x29);
        RADIO_WRITE_BANK_REG(1, CH1_ADC_TUNE11, 0x60);
        RADIO_WRITE_BANK_REG(1, CH2_ADC_TUNE10, 0x29);
        RADIO_WRITE_BANK_REG(1, CH2_ADC_TUNE11, 0x60);
        RADIO_WRITE_BANK_REG(1, CH3_ADC_TUNE10, 0x29);
        RADIO_WRITE_BANK_REG(1, CH3_ADC_TUNE11, 0x60);
        /* Wait for 1ms */
        MDELAY(1);

        /* ADC CMP Calibration and waiting for allready=1
           due to corner&temperature, need at most 1ms) */
        RADIO_MOD_BANK_REG(1, CH0_ADC_TUNE11, CMP_OSCALIB_START, 0x1);
        RADIO_MOD_BANK_REG(1, CH1_ADC_TUNE11, CMP_OSCALIB_START, 0x1);
        RADIO_MOD_BANK_REG(1, CH2_ADC_TUNE11, CMP_OSCALIB_START, 0x1);
        RADIO_MOD_BANK_REG(1, CH3_ADC_TUNE11, CMP_OSCALIB_START, 0x1);
        MDELAY(1);

        /* ADC CMP Calibration Post-Setting */
        RADIO_MOD_BANK_REG(1, CH0_ADC_TUNE10, SATDET_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH0_ADC_TUNE11, CMP_OSCALIB_SHORT2VCM_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH1_ADC_TUNE10, SATDET_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH1_ADC_TUNE11, CMP_OSCALIB_SHORT2VCM_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH2_ADC_TUNE10, SATDET_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH2_ADC_TUNE11, CMP_OSCALIB_SHORT2VCM_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH3_ADC_TUNE10, SATDET_EN, 0x0);
        RADIO_MOD_BANK_REG(1, CH3_ADC_TUNE11, CMP_OSCALIB_SHORT2VCM_EN, 0x0);

        /* Enable DAC */
        RADIO_MOD_BANK_REG(1, CH0_ADC_EN1, IDAC1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH0_ADC_EN1, IDAC3_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH1_ADC_EN1, IDAC1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH1_ADC_EN1, IDAC3_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH2_ADC_EN1, IDAC1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH2_ADC_EN1, IDAC3_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH3_ADC_EN1, IDAC1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, CH3_ADC_EN1, IDAC3_EN, 0x1);

        /* RXBB outmux selsection */
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0,CH0_RX_TEST,OUTMUX_SEL,0x1);
        RADIO_MOD_BANK_REG(0,CH1_RX_TEST,OUTMUX_SEL,0x1);
        RADIO_MOD_BANK_REG(0,CH2_RX_TEST,OUTMUX_SEL,0x1);
        RADIO_MOD_BANK_REG(0,CH3_RX_TEST,OUTMUX_SEL,0x1);

        /* VCM Buf disable */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, CH0_ADC_EN0,BUFFER_VCMBUF_EN,0x0);
        RADIO_MOD_BANK_REG(1, CH1_ADC_EN0,BUFFER_VCMBUF_EN,0x0);
        RADIO_MOD_BANK_REG(1, CH2_ADC_EN0,BUFFER_VCMBUF_EN,0x0);
        RADIO_MOD_BANK_REG(1, CH3_ADC_EN0,BUFFER_VCMBUF_EN,0x0);

        /* 800MHz, signal bandwidth 40MHz & 50MHz */
        if ((cfg->adc_freq == 40) || (cfg->adc_freq == 50)){
                /* 800M CLK enable */
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
                /* 800M ADC enable and reset */
                fmcw_radio_switch_bank(radio, 1);
                RADIO_WRITE_BANK_REG(1, CH0_ADC_EN0, 0xEF);
                RADIO_WRITE_BANK_REG(1, CH1_ADC_EN0, 0xEF);
                RADIO_WRITE_BANK_REG(1, CH2_ADC_EN0, 0xEF);
                RADIO_WRITE_BANK_REG(1, CH3_ADC_EN0, 0xEF);
        }

        /* ADC de-Reset */
        RADIO_MOD_BANK_REG(1, CH0_ADC_EN0, RST, 0x0);
        RADIO_MOD_BANK_REG(1, CH1_ADC_EN0, RST, 0x0);
        RADIO_MOD_BANK_REG(1, CH2_ADC_EN0, RST, 0x0);
        RADIO_MOD_BANK_REG(1, CH3_ADC_EN0, RST, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_rc_calibration(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);

        //RC Calibration
        fmcw_radio_switch_bank(radio, 2);
        RADIO_MOD_BANK_REG(2, RCCAL, CLK_5M_EN, 0x1);
        RADIO_MOD_BANK_REG(2, RCCAL, VREFSEL, 0x4);
        RADIO_MOD_BANK_REG(2, RCCAL, PD, 0x1);

        /* Below Delay of 1ms is necessary for calibrated RC circuit to be settled. */
        MDELAY(1);

        RADIO_MOD_BANK_REG(2, RCCAL, START, 0x1);
        //Wait for 1ms
        MDELAY(1);

        /* The clock of RC calibration has to be turned off after calibration,
           otherwise it will cause unwanted spurs in BB FFT2D data. */
        RADIO_MOD_BANK_REG(2, RCCAL, CLK_5M_EN, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* single point frequency */
int32_t fmcw_radio_single_tone(fmcw_radio_t *radio, double freq, bool enable)
{
        uint32_t freq_reg;
        uint32_t freq_sel;

        freq_reg = DIV_RATIO(freq, FREQ_SYNTH_SD_RATE);
        fmcw_radio_do_pll_cal(radio, freq_reg);
        fmcw_radio_hp_auto_ch_off(radio,-1);
        if (fmcw_radio_is_pll_locked(radio) == false)
                return E_PLL_UNLOCK;

        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SEL, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x0);
        RADIO_WRITE_BANK_REG(3, FMCW_MODE_SEL, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, EN, 0x0);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, SEL, 0x3);
        RADIO_MOD_BANK_REG(3, FMCW_SYNC, EN, 0x1);
        fmcw_radio_switch_bank(radio, 5); // single tone bank is not considered

        if (enable == true) // enable single_tone
                freq_sel = freq_reg;
        else // disable single_tone, restore init config
                freq_sel = radio->start_freq;

        RADIO_WRITE_BANK_REG(5, FMCW_START_FREQ_1_0, REG_L(freq_sel));
        RADIO_WRITE_BANK_REG(5, FMCW_START_FREQ_1_1, REG_M(freq_sel));
        RADIO_WRITE_BANK_REG(5, FMCW_START_FREQ_1_2, REG_H(freq_sel));
        RADIO_WRITE_BANK_REG(5, FMCW_START_FREQ_1_3, REG_INT(freq_sel));
        fmcw_radio_switch_bank(radio, 9);
        RADIO_WRITE_BANK_REG(9, LP_TST_FREQ_0, REG_L(freq_sel));
        RADIO_WRITE_BANK_REG(9, LP_TST_FREQ_1, REG_M(freq_sel));
        RADIO_WRITE_BANK_REG(9, LP_TST_FREQ_2, REG_H(freq_sel));
        RADIO_WRITE_BANK_REG(9, LP_TST_FREQ_3, REG_INT(freq_sel));
        fmcw_radio_switch_bank(radio, 3);
        RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);
        RADIO_MOD_BANK_REG(3, FMCW_START, START_SPI, 0x1);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}


/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : HPF1_Hz   : 0x1
 *                 HPF1_Hz   : 0x2
 *                 HPF1_Hz   : 0x3
 *                 HPF1_Hz   : 0x4
 *                 HPF1_Hz   : 0x5
 *                 HPF1_Hz   : 0x6
 *                 HPF1_Hz   : 0x7
 */
void fmcw_radio_set_hpf1(fmcw_radio_t *radio, int32_t channel_index, int32_t filter_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch;
        if (channel_index != -1)
                RADIO_MOD_BANK_CH_REG(0, channel_index, RX_TUNE2, HP1_SEL, filter_index);
        else {
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE2, HP1_SEL, filter_index);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_hpf1(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        char filter_index = RADIO_READ_BANK_CH_REG_FIELD(0, channel_index, RX_TUNE2, HP1_SEL);
        fmcw_radio_switch_bank(radio, old_bank);
        return filter_index;
}

/*
 * channel_index : 0 / 1 / 2 / 3 / -1
 * data          : HPF2_Hz   : 0x0
 *                 HPF2_Hz   : 0x1
 *                 HPF2_Hz   : 0x2
 *                 HPF2_Hz   : 0x3
 *                 HPF2_Hz   : 0x4
 *                 HPF2_Hz   : 0x5
 *                 HPF2_Hz   : 0x6
 */
void fmcw_radio_set_hpf2(fmcw_radio_t *radio, int32_t channel_index, int32_t filter_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch;
        if (channel_index != -1)
                RADIO_MOD_BANK_CH_REG(0, channel_index, RX_TUNE2, HP2_SEL, filter_index);
        else {
                for(ch = 0; ch < MAX_NUM_RX; ch++)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE2, HP2_SEL, filter_index);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

int32_t fmcw_radio_get_hpf2(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        char filter_index = RADIO_READ_BANK_CH_REG_FIELD(0, channel_index, RX_TUNE2, HP2_SEL);
        fmcw_radio_switch_bank(radio, old_bank);
        return filter_index;
}

/* enable fmcw auto on hp1 and hp2 */
void fmcw_radio_hp_auto_ch_on(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        if (channel_index == -1) {
                /* 0xF--on, ox0--off*/
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH0, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH1, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH2, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH3, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH0, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH1, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH2, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH3, EN, 0x1);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH0, 0x67);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH1, 0x67);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH2, 0x67);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH3, 0x67);
        } else if (channel_index == 0) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH0, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH0, EN, 0x1);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH0, 0x67);
        } else if (channel_index == 1) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH1, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH1, EN, 0x1);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH1, 0x67);
        } else if (channel_index == 2) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH2, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH2, EN, 0x1);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH2, 0x67);
        } else if (channel_index == 3) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH3, EN, 0x1);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH3, EN, 0x1);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH3, 0x67);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

/* disable fmcw auto off hp1 and hp2 */
void fmcw_radio_hp_auto_ch_off(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        if (channel_index == -1) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH0, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH1, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH2, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH3, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH0, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH1, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH2, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH3, EN, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH0, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH1, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH2, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH3, 0x0);
        } else if (channel_index == 0) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH0, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH0, EN, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH0, 0x0);
        } else if (channel_index == 1) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH1, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH1, EN, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH1, 0x0);
        } else if (channel_index == 2) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH2, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH2, EN, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH2, 0x0);
        } else if (channel_index == 3) {
                RADIO_MOD_BANK_REG(3, AUTO_HPF1_CH3, EN, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_HPF2_CH3, EN, 0x0);
                RADIO_WRITE_BANK_REG(3, AUTO_RXBB_CH3, 0x0);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

/*
 * enable fmcw auto on tx
 * auto tx mode selection:
 * 0000 = idle start, down and idle state
 * 0001 = idle start and down state
 * 0010 = idle start and idle state
 * 0011 = idle start state
 * 0100 = down and idle state
 * 0101 = down state
 * 0110 = idle state
 * 0111 = off
 * 1111 = no mode selected, keep old mode
 * enable: 0x1--on, 0x0--off
 */

void fmcw_radio_tx_auto_ch_on(fmcw_radio_t *radio, int32_t channel_index, int32_t mode_sel)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        int ch;
        if (channel_index == -1){
                for (ch = 0; ch < 4; ch++){
                        RADIO_MOD_BANK_CH_REG(3, ch, AUTO_TX, EN, 0x1);
                        /* certain mode selected */
                        if (mode_sel != 0xf)
                                RADIO_MOD_BANK_CH_REG(3, ch, AUTO_TX, SEL, mode_sel);
                }
        }
        else{
                RADIO_MOD_BANK_CH_REG(3, channel_index, AUTO_TX, EN, 0x1);
                if (mode_sel != 0xf)
                        RADIO_MOD_BANK_CH_REG(3, channel_index, AUTO_TX, SEL, mode_sel);
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

/*
 * disable fmcw auto off tx
 * enable: 0x1--on, ox0--off
 */
void fmcw_radio_tx_auto_ch_off(fmcw_radio_t *radio, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        int ch;
        if (channel_index == -1)
                for (ch = 0; ch < 4; ch++)
                        RADIO_MOD_BANK_CH_REG(3, ch, AUTO_TX, EN, 0x0);
        else
                RADIO_MOD_BANK_CH_REG(3, channel_index, AUTO_TX, EN, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
}

/* SDM reset */
void fmcw_radio_sdm_reset(fmcw_radio_t *radio)
{
        fmcw_radio_adc_on(radio);
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        uint8_t ch; /* channel index*/
        for(ch = 0; ch < MAX_NUM_RX; ch++)
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, RST, 1);
        /* after 4 channel reset asserted, then deasserted*/
        for(ch = 0; ch < MAX_NUM_RX; ch++)
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, RST, 0);
        fmcw_radio_switch_bank(radio, old_bank);
}

/* enable/disable fmcw agc mode */
void fmcw_radio_agc_enable(fmcw_radio_t *radio, bool enable)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        int val = (enable == true)?1:0;
        RADIO_MOD_BANK_REG(5,FMCW_AGC_EN_1,AGC_EN_1,val);   //agc enable
        fmcw_radio_switch_bank(radio, 0);
        for (int ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_VGA, val);     //RF BB VGA2 enable
                RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_EN, val);      //RF BB VGA1 enable
                RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, SAT_EN, val);      //TIA SAT enable
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_agc_setup(fmcw_radio_t *radio)
{
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        //  code start
        for (int ch = 0; ch < MAX_NUM_RX; ch++) {
                //TIA_SAT_VREF config
                if (cfg->agc_tia_thres >= 0.5)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, TIA_SAT_VREF_SEL,0x3);  //TIA vREf 1v
                else if(cfg->agc_tia_thres > 0.45)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, TIA_SAT_VREF_SEL,0x2);  //TIA vREf 0.95v
                else if(cfg->agc_tia_thres > 0.43)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, TIA_SAT_VREF_SEL,0x1);  //TIA vREf 0.9v
                else
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, TIA_SAT_VREF_SEL,0x0);  //TIA vREf 0.85v
                //VGA_SAT_VREF config
                if (cfg->agc_vga1_thres >= 1.0)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_VTHSEL,0x3);  //VGA vREf 1.7v
                else if (cfg->agc_vga1_thres >= 0.95)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_VTHSEL,0x2);  //VGA vREf 1.65v
                else if (cfg->agc_vga1_thres >= 0.8)
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_VTHSEL,0x1);  //VGA vREf 1.55v
                else
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_VTHSEL,0x0);  //VGA vREf 1.5v
        }
        //  code end
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_vam_status_save(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        vam_status[0] = RADIO_READ_BANK_REG(5, FMCW_TX0_CTRL_1_2);
        vam_status[1] = RADIO_READ_BANK_REG(5, FMCW_TX1_CTRL_1_2);
        vam_status[2] = RADIO_READ_BANK_REG(5, FMCW_TX2_CTRL_1_2);
        vam_status[3] = RADIO_READ_BANK_REG(5, FMCW_TX3_CTRL_1_2);
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_txphase_status(fmcw_radio_t *radio, bool save)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        int ch;
        if (save) {
                for (ch=0; ch<MAX_NUM_TX; ch++) {
                       txphase_status[2*ch]     = RADIO_READ_BANK_CH_REG(1, ch, TX_TUNE0);
                       txphase_status[2*ch + 1] = RADIO_READ_BANK_CH_REG(1, ch, TX_TUNE1);
                }
        } else {
                for (ch=0; ch<MAX_NUM_TX; ch++) {
                       RADIO_WRITE_BANK_CH_REG(1, ch, TX_TUNE0, txphase_status[2*ch]);
                       RADIO_WRITE_BANK_CH_REG(1, ch, TX_TUNE1, txphase_status[2*ch+1]);
                }
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_vam_disable(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        for (uint8_t ch = 0; ch < MAX_NUM_RX; ch++)
                RADIO_MOD_BANK_FWCW_TX_REG(5, ch, CTRL_1_2, VAM_EN, 0x0);  /* disable varray mode to write spi registers */
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_vam_status_restore(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        RADIO_WRITE_BANK_REG(5, FMCW_TX0_CTRL_1_2, vam_status[0]);
        RADIO_WRITE_BANK_REG(5, FMCW_TX1_CTRL_1_2, vam_status[1]);
        RADIO_WRITE_BANK_REG(5, FMCW_TX2_CTRL_1_2, vam_status[2]);
        RADIO_WRITE_BANK_REG(5, FMCW_TX3_CTRL_1_2, vam_status[3]);
        fmcw_radio_switch_bank(radio, old_bank);
}

/* For protection of switching between some special modes without power off the chirp */
void fmcw_radio_special_mods_off(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        //turn off VAM mode
        for (int ch = 0; ch < MAX_NUM_TX; ch++) {
                RADIO_WRITE_BANK_FWCW_TX_REG(5, ch, CTRL_1_1, 0x0);
                RADIO_MOD_BANK_FWCW_TX_REG(5, ch, CTRL_1_2, VAM_EN, 0x0);
                RADIO_MOD_BANK_FWCW_TX_REG(5, ch, CTRL_1_2, VAM_P,  0x0);
        }
        //turn off ps mode
        fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        RADIO_MOD_BANK_REG(5, FMCW_PS_EN_1, PS_EN_1, 0x0);
        //turn off AGC mode
        RADIO_MOD_BANK_REG(5, FMCW_AGC_EN_1, AGC_EN_1,0x0);

        fmcw_radio_switch_bank(radio, 0);
        for (int ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_VGA, 0x0);     //RF BB VGA2 enable
                RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, PKD_EN, 0x0);      //RF BB VGA1 enable
                RADIO_MOD_BANK_CH_REG(0, ch, RX_PDT, SAT_EN, 0x0);      //TIA SAT enable
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

uint32_t fmcw_radio_compute_lock_freq(fmcw_radio_t *radio)
{
        uint32_t min_freq = 0;
        uint32_t max_freq = 0;
        uint32_t start_freq;
        uint32_t stop_freq;
        uint32_t max_bandwidth;
        for (uint8_t i = 0; i < NUM_FRAME_TYPE; i++) {
                sensor_config_t *cfg = sensor_config_get_config(i);
                start_freq = DIV_RATIO(cfg->fmcw_startfreq, FREQ_SYNTH_SD_RATE);
                stop_freq  = DIV_RATIO(cfg->fmcw_startfreq + cfg->fmcw_bandwidth * 1e-3, FREQ_SYNTH_SD_RATE);
                if(i == 0) {
                        min_freq = start_freq;
                        max_freq = stop_freq;
                } else {
                        if (min_freq > start_freq)
                                min_freq = start_freq;
                        if (max_freq < stop_freq)
                                max_freq = stop_freq;
                }
        }
        max_bandwidth = max_freq - min_freq;
        uint32_t lock_freq = min_freq + max_bandwidth / 2;
        return lock_freq;
}

/*Bug fix for anti velamb glitch*/
void fmcw_radio_cmd_cfg(fmcw_radio_t *radio)
{
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;

        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        //switch to bank 5 without return value
        fmcw_radio_switch_bank(radio, 5 + radio->frame_type_id);
        //CMD period
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_PRD_1, cfg->nvarray + 1);

        /* CMD1 config */
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_X_1_0, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_X_1_1, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_X_1_2, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_X_1_3, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_NUM_1_0    , 0x0 );

        /* CMD2 config */
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Y_1_0, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Y_1_1, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Y_1_2, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Y_1_3, 0x0 );
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_NUM_1_1    , 0x0);

        /* CMD3 config */
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Z_1_0, (((radio->up_cycle - CMD_CYCLE_MARGIN) / 2) >>  0) & 0xff);
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Z_1_1, (((radio->up_cycle - CMD_CYCLE_MARGIN) / 2) >>  8) & 0xff);
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Z_1_2, (((radio->up_cycle - CMD_CYCLE_MARGIN) / 2) >> 16) & 0xff);
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_TIMER_Z_1_3, (((radio->up_cycle - CMD_CYCLE_MARGIN) / 2) >> 24) & 0xff);
        RADIO_WRITE_BANK_REG(5, FMCW_CMD_NUM_1_2, 0x2);

        //switch to bank 4 without return value
        fmcw_radio_switch_bank(radio, 4);

        //CMD Group1, switch to bank 1, for the extra chirp
        RADIO_WRITE_BANK_REG(4, CPU_ADDR1, 0x00);
        RADIO_WRITE_BANK_REG(4, CPU_DATA1, 0x01);
        RADIO_WRITE_BANK_REG(4, CPU_ADDR2, 0x00);
        RADIO_WRITE_BANK_REG(4, CPU_DATA2, 0x01);
        //CMD Group2 and CMD group 3
        for (int prd = 0; prd < (cfg->nvarray); prd++){
                EMBARC_PRINTF("prd = %d\n", prd);
                for (int ch = 0; ch < MAX_NUM_TX; ch++) {
                        if ( ( (cfg->tx_groups[ch]) >> ( 0 + prd * 4  )) & 0xF) {
                                fmcw_radio_reg_write(radio,RADIO_BK4_CPU_ADDR3 + prd * 4 , RADIO_BK1_CH0_TX_EN0 + ch * 9);
                        }
                        if (prd == (cfg->nvarray - 1)){
                                if (cfg->tx_groups[ch] & 0xF) {
                                        fmcw_radio_reg_write(radio,RADIO_BK4_CPU_DATA4 + prd * 4 - 1, RADIO_BK1_CH0_TX_EN0 + ch * 9);
                                }
                        }
                        else if ( ( (cfg->tx_groups[ch]) >> ( 4 + prd * 4  )) & 0xF) {
                                fmcw_radio_reg_write(radio,RADIO_BK4_CPU_ADDR4 + prd * 4 , RADIO_BK1_CH0_TX_EN0 + ch * 9);
                        }
                }
                fmcw_radio_reg_write(radio,RADIO_BK4_CPU_DATA3 + prd * 4 , 0x03);
                fmcw_radio_reg_write(radio,RADIO_BK4_CPU_DATA4 + prd * 4 , 0x0f);
        }
        //code end
        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_auxadc_trim(fmcw_radio_t *radio)
{
        uint16_t trim_data = baseband_read_reg(NULL,OTP_TRIM_AUX_ADC);
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        if( trim_data != 0 ){
                RADIO_MOD_BANK_REG(1, DTSMD1_SEL, DTSDM_REFSEL, REG_M(trim_data));
                RADIO_MOD_BANK_REG(1, DTSMD2_SEL, DTSDM_REFSEL, REG_L(trim_data));
        }
        fmcw_radio_switch_bank(radio, old_bank);
}

uint32_t fmcw_radio_rfbist_trim(fmcw_radio_t *radio)
{
        uint32_t trim_data = baseband_read_reg(NULL,OTP_TRIM_RF_BIST);
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        fmcw_radio_switch_bank(radio, old_bank);
        return trim_data;
}

int32_t fmcw_radio_lvds_on(fmcw_radio_t *radio, bool enable)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 2);
        int ch;
        RADIO_MOD_BANK_REG(2, LVDS_LDO25, LDO25_LVDS_LDO_EN, enable);
        for (ch = 0; ch < MAX_NUM_RX; ch++){
                RADIO_MOD_BANK_CH_REG(2, ch, LVDS, EN, enable);
                RADIO_MOD_BANK_CH_REG(2, ch, LVDS, PRE_EN, enable);
        }
        RADIO_MOD_BANK_REG(2, LVDS_CLK, EN, enable);
        RADIO_MOD_BANK_REG(2, LVDS_CLK, PRE_EN, enable);
        RADIO_MOD_BANK_REG(2, LVDS_FRAME, EN, enable);
        RADIO_MOD_BANK_REG(2, LVDS_FRAME, PRE_EN, enable);
        RADIO_MOD_BANK_REG(2, ADC_FILTER0, RSTN, enable);
        RADIO_MOD_BANK_REG(2, ADC_FILTER0, LVDS_OUT_EN, enable);
        RADIO_MOD_BANK_REG(2, ADC_FILTER0, CMOS_OUT_EN, enable);
        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

/*
 * inputmux selection : MainBG VPTAT(default) / TestMUXN / TestMUXP / TPANA1
 *      #define AUXADC1_MainBG_VPTAT     0
 *      #define AUXADC1_TestMUXN         1
 *      #define AUXADC1_TestMUXP         2
 *      #define AUXADC1_TPANA1           3
 */
float fmcw_radio_auxadc1_voltage(fmcw_radio_t *radio, int32_t muxin_sel)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        /* Enable 800M ADC CLK */
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
        /* Enable safety monitor LDO */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);
        /* Enable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        /* AUXADC1 on and reset */
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_BUF_BYPASS, 0x0);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_BUF_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_BIAS_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_OP1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_OP2_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_REFGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_VCMGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_RST, 0x1);

        /* load trimming value from the corresponding eFuse address */
        fmcw_radio_auxadc_trim(radio);
        MDELAY(2);

        /* select AUXADC1 InputMUX */
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, DTSDM_MUXIN_SEL, muxin_sel);
        switch (muxin_sel) {
        case 0:
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, TEST_CBC2, VPTAT_TEST_EN, 0x1);
                break;
        case 1:
        case 2:
                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_MUXN_EN, 0x1);
                RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_MUXP_EN, 0x1);
                break;
        case 3:
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, TPANA1, EN, 0x1);
                break;
        default:
                break;
        }

        /* AUXADC1 de-reset */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_RST, 0x0);
        /* AUXADC1 Filter de-reset */
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, DC_FILTER1_RST_EN, 0x01);
        MDELAY(2);
        /* read back AUXADC1 Filter Output Digital Bits */
        fmcw_radio_switch_bank(radio, 1);
        uint32_t doutL, doutM, doutH;
        float dout;
        doutL = RADIO_READ_BANK_REG(1, DTSDM1_DAT0);
        doutM = RADIO_READ_BANK_REG(1, DTSDM1_DAT1);
        doutH = RADIO_READ_BANK_REG(1, DTSDM1_DAT2);
        dout = doutL + (doutM << 8) + (doutH << 16);
        /* return voltage measurement, formula refer to 85C */
        float auxadc1_voltage;
        auxadc1_voltage = (dout / (1<<17) -1) * 3.3 + 1.67;

        /* Disable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
        return auxadc1_voltage;
}

void fmcw_radio_clk_out_for_cascade(void)
{
#ifdef CHIP_CASCADE
        uint8_t old_bank = fmcw_radio_switch_bank(NULL, 0);

        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                RADIO_MOD_BANK_REG(0, REFPLL_EN, CLK_400M_FMCW_EN, 0x1);
                MDELAY(200);
                fmcw_radio_switch_bank(NULL, 2);
                RADIO_MOD_BANK_REG(2, DIV_CLK, FLTR_CLK_OUT_ENABLE, 0x1);
        } else {
                RADIO_MOD_BANK_REG(0, REFPLL_EN, CLK_400M_FMCW_EN, 0x0);
        }

        fmcw_radio_switch_bank(NULL, old_bank);
#endif
}

/*
 * inputmux selection : TS VPTAT(default) / TS VBG / TestMUXN / TPANA2
 *      #define AUXADC2_TS_VPTAT     0
 *      #define AUXADC2_TS_VBG       1
 *      #define AUXADC2_TestMUXN     2
 *      #define AUXADC2_TPANA2       3
 */
float fmcw_radio_auxadc2_voltage(fmcw_radio_t *radio, int32_t muxin_sel)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        /* Enable 800M ADC CLK */
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
        /* Enable safety monitor LDO */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);
        /* Enable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        /* AUXADC2 on and reset */
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BUF_BYPASS, 0x0);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BUF_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BIAS_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_OP1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_OP2_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_REFGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_VCMGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_RST, 0x1);

        /* load trimming value from the corresponding eFuse address */
        fmcw_radio_auxadc_trim(radio);
        MDELAY(2);

        /* select AUXADC2 InputMUX */
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, TS_BG_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, TS_VPTAT_CAL, 0x4);
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, DTSDM_MUXIN_SEL, muxin_sel);
        switch (muxin_sel) {
        case 0:
                break;
        case 1:
                break;
        case 2:
                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_MUXN_EN, 0x1);
                RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_MUXP_EN, 0x1);
                break;
        case 3:
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, TPANA2, TPANA2_EN, 0x1);
                break;
        default:
                break;
        }

        /* AUXADC2 de-reset */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_RST, 0x0);
        /* AUXADC2 Filter de-reset */
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, DC_FILTER2_RST_EN, 0x01);
        MDELAY(2);
        /* read back AUXADC2 Filter Output Digital Bits */
        fmcw_radio_switch_bank(radio, 1);
        uint32_t doutL, doutM, doutH;
        float dout;
        doutL = RADIO_READ_BANK_REG(1, DTSDM2_DAT0);
        doutM = RADIO_READ_BANK_REG(1, DTSDM2_DAT1);
        doutH = RADIO_READ_BANK_REG(1, DTSDM2_DAT2);
        dout = doutL + ( doutM << 8 ) + ( doutH << 16 );
        /* return voltage measurement, formula refer to 85C */
        float auxadc2_voltage;
        auxadc2_voltage = (dout / (1<<17) -1) * 3.3 + 1.67;

        /* Disable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
        return auxadc2_voltage;
}

/* This function is used for getting IRQ value of Safety Monitor1: LDO Monitor
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
uint8_t* fmcw_radio_sm_ldo_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        uint8_t *old_ldo;
        static uint8_t IRQ_value[6];

        /* get ldo monitor setting */
        old_ldo = fmcw_radio_sm_ldo_monitor_ldo_on(radio, fault_injection);
        fmcw_radio_sm_ldo_monitor_setting(radio,fault_injection);

        /* cannot reduce this delay due to cnt size and ldo numbers
         * LDO Monitor will not get the right status without enough delay
         * already studied the delay, cannot reduce
         */
        if(fault_injection){
                MDELAY(4);
        }
        else{
                MDELAY(100);
        }

        /* Read IRQ */
        IRQ_value[0] = RADIO_READ_BANK_REG(10, LDO_MONITOR_STATUS_0);
        IRQ_value[1] = RADIO_READ_BANK_REG(10, LDO_MONITOR_STATUS_1);
        IRQ_value[2] = RADIO_READ_BANK_REG(10, LDO_MONITOR_STATUS_2);
        IRQ_value[3] = RADIO_READ_BANK_REG(10, LDO_MONITOR_STATUS_3);
        IRQ_value[4] = RADIO_READ_BANK_REG(10, LDO_MONITOR_STATUS_4);
        IRQ_value[5] = RADIO_READ_BANK_REG_FIELD(10,ITF_IRQ_1,ITF_IRQ_SUPPLY_LDO);

        /* restore ldo setting and clear IRQ status for monitoring */
        if (fault_injection) {
                fmcw_radio_sm_ldo_monitor_ldo_off(radio, fault_injection, old_ldo);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_CLEAR, 0x1);
                UDELAY(100);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_TEST_START, 0x1);
                UDELAY(300);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_TEST_START, 0x0);
        }

        /* Disable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for getting IRQ value of Safety Monitor2: AVDD33 Monitor
 * 111 AVDD33 > +10%
 * 101 +7.5% < AVDD33 < +10%
 * 000 -7.5% < AVDD33 < +7.5%
 * 110 -10% < AVDD33 < -7.5%
 * 100 AVDD33 < -10%
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
uint8_t fmcw_radio_sm_avdd33_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        uint8_t IRQ_value_L,IRQ_value_H,IRQ_value;
        /* get avdd33 monitor threahold */
        fmcw_radio_sm_avdd33_monitor_threshold(radio, fault_injection);
        /* get avdd33 monitor setting */
        fmcw_radio_sm_avdd33_monitor_setting(radio);

        /* Read IRQ */
        IRQ_value_H = RADIO_READ_BANK_REG_FIELD(10,ITF_IRQ_1,ITF_IRQ_SUPPLY_CBC33);
        fmcw_radio_switch_bank(radio, 0);
        IRQ_value_L = RADIO_READ_BANK_REG(0,MS)& 0x03;
        IRQ_value = (IRQ_value_H << 2) + IRQ_value_L;

        /* restore avdd33 threahold and clear IRQ status for monitoring */
        if (fault_injection) {
                fmcw_radio_sm_avdd33_monitor_threshold(radio, false);
                fmcw_radio_switch_bank(radio, 10);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_CLEAR, 0x1);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_TEST_START, 0x1);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_TEST_START, 0x0);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for getting IRQ value of Safety Monitor3: DVDD11 Monitor
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
uint8_t fmcw_radio_sm_dvdd11_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        uint8_t IRQ_value;

        /* get dvdd11 monitor threahold */
        fmcw_radio_sm_dvdd11_monitor_threshold(radio, fault_injection);
        /* get dvdd11 monitor setting */
        fmcw_radio_sm_dvdd11_monitor_setting(radio);

        /* Read IRQ */
        IRQ_value = RADIO_READ_BANK_REG_FIELD(10,ITF_IRQ_1,ITF_IRQ_SUPPLY_DVDD11);

        /* restore dvdd11 threahold and clear IRQ status for monitoring */
        if (fault_injection) {
                fmcw_radio_sm_dvdd11_monitor_threshold(radio, false);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_CLEAR, 0x1);
                UDELAY(100);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_TEST_START, 0x1);
                UDELAY(300);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_TEST_START, 0x0);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for getting IRQ value of Safety Monitor4: Bandgap Voltage Monitor
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
uint8_t fmcw_radio_sm_bg_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        uint8_t IRQ_value;

        /* get bg monitor threahold */
        fmcw_radio_sm_bg_monitor_threshold(radio, fault_injection);
        /* get bg monitor setting */
        fmcw_radio_sm_bg_monitor_setting(radio);

        /* Read IRQ */
        IRQ_value = RADIO_READ_BANK_REG_FIELD(10,ITF_IRQ_1,ITF_IRQ_VBG);

        /* restore dvdd11 threahold and clear IRQ status for monitoring */
        if (fault_injection) {
                fmcw_radio_sm_bg_monitor_threshold(radio, false);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_CLEAR, 0x1);
                UDELAY(100);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_TEST_START, 0x1);
                UDELAY(300);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_TEST_START, 0x0);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for getting IRQ value of Safety Monitor5: CPU Clock Lock Detector
 * no fault injection required
 */
uint8_t fmcw_radio_sm_cpu_clk_lock_det_IRQ(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        uint8_t IRQ_value;

        /* Read IRQ */
        IRQ_value = !(RADIO_READ_BANK_REG_FIELD(0,AUTO_LOCK1,REFPLL_LCKED));

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/*
 * This function is used for getting IRQ value of Safety Monitor fault injection: RF Power Detector
 * freq            : lock freq
 * power_th        : rf power lower threahold (min 5dBm)
 * channel_index   : 0/ 1/ 2/ 3/ -1 (all channels)
 * fault injection : true
 */
uint8_t* fmcw_radio_sm_rfpower_detector_fault_injection_IRQ(fmcw_radio_t *radio, double freq, double power_th, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        static uint8_t IRQ_value[5];
        /* set rfpower detector fault injection threshold */
        fmcw_radio_sm_rfpower_detector_fault_injection_threshold(radio);
        /* get rfpower_detector setting*/
        fmcw_radio_sm_rfpower_detector_setting(radio, freq, channel_index);

        /* Read IRQ */
        IRQ_value[0] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_1, TX0);
        IRQ_value[1] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_0, TX1);
        IRQ_value[2] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_0, TX2);
        IRQ_value[3] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_0, TX3);
        IRQ_value[4] = RADIO_READ_BANK_REG_FIELD(10, ITF_IRQ_2, ITF_IRQ_RFPOWER);

        /* set rfpower detector threshold for monitoring*/
        fmcw_radio_sm_rfpower_detector_threshold(radio, freq, power_th, channel_index);

        /* clear IRQ status for monitoring */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_CLEAR, 0x1);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_CLEAR, 0x0);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_TEST_START, 0x1);
        MDELAY(1);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_TEST_START, 0x0);

        /* Disable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);

        /* Generate FMCW */
        fmcw_radio_generate_fmcw(radio);
        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/*
 * This function is used for getting IRQ value of Safety Monitor: RF Power Detector
 * freq            : lock freq
 * power_th        : rf power lower threahold (min 5dBm)
 * channel_index   : 0/ 1/ 2/ 3/ -1 (all channels)
 * fault injection : false
 */
uint8_t* fmcw_radio_sm_rfpower_detector_IRQ(fmcw_radio_t *radio, double freq, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        static uint8_t IRQ_value[5];

        /* get rfpower detector setting */
        fmcw_radio_sm_rfpower_detector_setting(radio, freq, channel_index);
        /* Read IRQ */
        IRQ_value[0] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_1, TX0);
        IRQ_value[1] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_0, TX1);
        IRQ_value[2] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_0, TX2);
        IRQ_value[3] = RADIO_READ_BANK_REG_FIELD(10, RFPOWER_MONITOR_STATUS_0, TX3);
        IRQ_value[4] = RADIO_READ_BANK_REG_FIELD(10, ITF_IRQ_2, ITF_IRQ_RFPOWER);

        /* Disable 5M AUXADC CLK */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);

        /* Generate FMCW */
        fmcw_radio_generate_fmcw(radio);
        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for Safety Monitor: IF Loopback */
uint8_t fmcw_radio_sm_if_loopback_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        uint8_t ch, IRQ_value = 0;
        float vga_gain_tia250[4], vga_gain_tia500[4];
        safety_monitor_mode = true;
        baseband_t *bb = baseband_get_bb(0);
        baseband_hw_t *bb_hw = &bb->bb_hw;

        /* enable 400M ADC CLK */
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x0);

        /* set rx gain 250 1 1 */
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, 0x1);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, 0x1);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, 0x1);
        }

        /* Fault Injection */
        if (fault_injection) {
                EMBARC_PRINTF("reached sm11 if loopback fault injection\n");
                /* set rx gain*/
                for (ch = 0; ch < MAX_NUM_RX; ch++) {
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, 0xf);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, 0x1);
                        RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, 0x1);
                }
        }
        MDELAY(2);
        baseband_dac_playback(bb_hw, true, 0, 3, false, vga_gain_tia250);
        /* set rx gain 500 3 3*/
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, 0x2);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, 0x3);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, 0x3);
        }
        MDELAY(2);
        baseband_dac_playback(bb_hw, true, 0, 3, false, vga_gain_tia500);
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                if ((vga_gain_tia500[ch] - vga_gain_tia250[ch] >= 16) && ((vga_gain_tia500[ch] - vga_gain_tia250[ch]) <= 20)) {
                        continue;
                } else {
                        IRQ_value = 1;
                        break;
                }
        }

        safety_monitor_mode = false;

        /* restore rx gain setting */
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, cfg->rf_tia_gain);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, cfg->rf_vga2_gain);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, cfg->rf_vga1_gain);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for Safety Monitor: RF Loopback */
uint8_t fmcw_radio_sm_rf_loopback_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        uint8_t ch, IRQ_value=0;
        float rf_gain[4];
        safety_monitor_mode = true;
        baseband_t *bb = baseband_get_bb(0);
        baseband_hw_t *bb_hw = &bb->bb_hw;

        /* enter shadow bank */
        fmcw_radio_loop_test_en(radio,true);
        /* lock frequency 80G */
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x1);
        fmcw_radio_single_tone(radio, 80, true);
        /* leave shadow bank */
        fmcw_radio_loop_test_en(radio,false);
        MDELAY(1);
        /* set rx gain */
        for (ch = 0; ch < 4; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, 0x1);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, 0x1);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, 0x1);
        }
        /* Fault Injection */
        if (fault_injection) {
                EMBARC_PRINTF("reached sm12 rf loopback fault injection\n");
                RADIO_MOD_BANK_REG(0, LO_LDO3, LDO11_RXLO_EN, 0x0);
        }
        /* get rx bist value */
        baseband_dac_playback(bb_hw, false, 0, 3, false, rf_gain);

        /* Normal Operation */
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                if (rf_gain[ch] >= 40) {
                        continue;
                } else {
                        IRQ_value = 1;
                        break;
                }
        }

        /* Fault Injection Release*/
        if (fault_injection)
                RADIO_MOD_BANK_REG(0, LO_LDO3, LDO11_RXLO_EN, 0x1);

        safety_monitor_mode = false;

        /* restore rx gain setting */
        for (ch = 0; ch < MAX_NUM_RX; ch++) {
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE0, TIA_RFB_SEL, cfg->rf_tia_gain);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA2_GAINSEL, cfg->rf_vga2_gain);
                RADIO_MOD_BANK_CH_REG(0, ch, RX_TUNE1, VGA1_GAINSEL, cfg->rf_vga1_gain);
        }

        /* Generate FMCW */
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x1);
        fmcw_radio_generate_fmcw(radio);
        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for getting IRQ value of Safety Monitor13: Chirp Monitor */
uint8_t fmcw_radio_sm_chirp_monitor_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        uint8_t IRQ_value;

        fmcw_radio_sm_chirp_monitor_setting(radio);

        /* Generate FMCW */
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x1);
        fmcw_radio_generate_fmcw(radio);

        /* Fault Injection */
        fmcw_radio_sm_chirp_monitor_fault_injection(radio, fault_injection);

        fmcw_radio_start_fmcw(radio);
        MDELAY(1);

        /* Read IRQ */
        fmcw_radio_switch_bank(radio, 10);
        IRQ_value = RADIO_READ_BANK_REG(10,ITF_IRQ_4);

        /* fault injection restore and clear IRQ status for monitoring */
        if (fault_injection) {
                fmcw_radio_sm_chirp_monitor_fault_injection(radio, false);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_CLEAR, 0x1);
                UDELAY(100);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_TEST_START, 0x1);
                UDELAY(300);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_TEST_START, 0x0);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

/* This function is used for getting IRQ value of Safety Monitor14: Over Temperature Detector */
uint8_t fmcw_radio_sm_over_temp_detector_IRQ(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        uint8_t IRQ_value;

        /* get over_temp_detector threahold */
        fmcw_radio_sm_over_temp_detector_threshold(radio, fault_injection);
        /* get over_temp_detector setting */
        fmcw_radio_sm_over_temp_detector_setting(radio);
        /* Read IRQ */
        IRQ_value = RADIO_READ_BANK_REG(10,ITF_IRQ_5);

        /* restore over_temp_detector threahold and clear IRQ status for monitoring */
        if (fault_injection) {
                fmcw_radio_sm_over_temp_detector_threshold(radio, false);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_CLEAR, 0x1);
                UDELAY(100);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_TEST_START, 0x1);
                UDELAY(300);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_TEST_START, 0x0);
        }

        /* Disable 5M ADC Clock */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x0);
        fmcw_radio_switch_bank(radio, old_bank);
        return IRQ_value;
}

 /*
  * This function is used for register setting of power detector
  * channel_index : 0 / 1 / 2 / 3 /-1 (all channels)
  * pdt_type      : calon 0 / caloff 1 / paon 2 / paoff 3
  * if all channels on, set register of TX0
  */
void fmcw_radio_pdt_reg(fmcw_radio_t *radio, int8_t pdt_type, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);

        /* general setting */
        RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDT_EN,0x1);
        RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDT_CALGEN_RSEL,0x2);
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, TPANA1, EN, 0x1);
        RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_EN, 0x1);

        /* select TPANA mux of channel */
        switch (channel_index) {
        case -1:
        case 0:
                RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_SEL, 0x28);
                break;
        case 1:
                RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_SEL, 0x29);
                break;
        case 2:
                RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_SEL, 0x2A);
                break;
        case 3:
                RADIO_MOD_BANK_REG(0, TPANA1, TEST_MUX_1_SEL, 0x2B);
                break;
        default:
                break;
        }

        /* select pdt_type */
        fmcw_radio_switch_bank(radio, 1);
        switch (pdt_type) {
        case 0:
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDT_CALGEN_EN,0x1);
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDTOUT_SEL,0x4);
                break;
        case 1:
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDT_CALGEN_EN,0x0);
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDTOUT_SEL,0x4);
                break;
        case 2:
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDT_CALGEN_EN,0x0);
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDTOUT_SEL,0x1);
                break;
        case 3:
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDT_CALGEN_EN,0x0);
                RADIO_MOD_BANK_CH_REG(1, channel_index, TX_TUNE2, PDTOUT_SEL,0x1);
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, VCO_EN, 0x0);
                break;
        default:
                break;
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

 /* This function is used for dout code calculation of auxadc2 */
double fmcw_radio_auxadc2_dout(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);

        /* read back AUXADC2 Filter Output Digital Bits */
        uint32_t doutL, doutM, doutH;
        double dout;
        /* 1st bring up, cannot readback dout, delay is necessary */
        MDELAY(1);
        doutL = RADIO_READ_BANK_REG(1, DTSDM2_DAT0);
        doutM = RADIO_READ_BANK_REG(1, DTSDM2_DAT1);
        doutH = RADIO_READ_BANK_REG(1, DTSDM2_DAT2);
        dout = doutL + ( doutM << 8 ) + ( doutH << 16 );

        fmcw_radio_switch_bank(radio, old_bank);
        return dout;
}

 /* This function is used to calculate pa dout of pdt */
double fmcw_get_pa_dout(fmcw_radio_t *radio, double cal_on, double cal_off, int32_t channel_index, double freq, double power_th)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);

        /* calculate according to pdt formula */
        double Dconst, Dconst1, Dconst2, Dconst3, paon_dout, paoff_dout, pa_dout, paldo_default;

        if (freq <= 78)
                Dconst1 = pow(10,(power_th-7)/12);
        else
                Dconst1 = pow(10,(power_th-6.5)/12);
        Dconst = (cal_on - cal_off) * Dconst1;

        /* cal paon-paoff @ pa ldo = 0 */
        fmcw_radio_switch_bank(radio, 1);
        /* record paldo default value */
        paldo_default = RADIO_READ_BANK_CH_REG_FIELD(1, channel_index, PA_LDO, LDO11_TX_PA_VOUT_SEL);
        RADIO_MOD_BANK_CH_REG(1, channel_index, PA_LDO, LDO11_TX_PA_VOUT_SEL, 0);
        /* set pdt pa on reg */
        fmcw_radio_pdt_reg(radio, 2, channel_index);
        paon_dout = fmcw_radio_auxadc2_dout(radio);
        /* set pdt pa off reg */
        fmcw_radio_pdt_reg(radio, 3, channel_index);
        paoff_dout = fmcw_radio_auxadc2_dout(radio);
        /* after pa off, we should lock frequency again */
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, VCO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
        fmcw_radio_single_tone(radio, freq, true);
        pa_dout = paon_dout;
        Dconst2 = paon_dout - paoff_dout;

        /* find the nearest paon - paoff = Dconst */
        for (int pa_ldo = 1; pa_ldo <16; pa_ldo++){
                fmcw_radio_switch_bank(radio, 1);
                RADIO_MOD_BANK_CH_REG(1, channel_index, PA_LDO, LDO11_TX_PA_VOUT_SEL, pa_ldo);
                /* set pdt pa on reg */
                fmcw_radio_pdt_reg(radio, 2, channel_index);
                paon_dout = fmcw_radio_auxadc2_dout(radio);
                /* set pdt pa off reg */
                fmcw_radio_pdt_reg(radio, 3, channel_index);
                paoff_dout = fmcw_radio_auxadc2_dout(radio);

                /* after pa off, we should lock frequency again */
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, VCO_EN, 0x1);
                RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
                fmcw_radio_single_tone(radio, freq, true);
                Dconst3 = paon_dout - paoff_dout;

                if (abs(Dconst2-Dconst) > abs(Dconst3-Dconst)) {
                        Dconst2 = Dconst3;
                        pa_dout = paon_dout;
                } else {
                        fmcw_radio_switch_bank(radio, 1);
                        RADIO_MOD_BANK_CH_REG(1, channel_index, PA_LDO, LDO11_TX_PA_VOUT_SEL, paldo_default);
                        fmcw_radio_switch_bank(radio, old_bank);
                        return pa_dout;
                }

                if (pa_ldo == 15) {
                        fmcw_radio_switch_bank(radio, 1);
                        RADIO_MOD_BANK_CH_REG(1, channel_index, PA_LDO, LDO11_TX_PA_VOUT_SEL, paldo_default);
                        fmcw_radio_switch_bank(radio, old_bank);
                        return paon_dout;
                }
        }
        fmcw_radio_switch_bank(radio, old_bank);
        return 0;
}

/* This function is used for fault injection of Safety Monitor Power ON Self Test
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
void fmcw_radio_sm_fault_injection(fmcw_radio_t *radio){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        uint8_t *IRQ_value_ldo_monitor;
        uint8_t *IRQ_value_rfpower_detector;
        uint8_t IRQ_value;

        /* sm_rfpower_detector fault injection */
        IRQ_value_rfpower_detector = fmcw_radio_sm_rfpower_detector_fault_injection_IRQ(radio,77,5,1);
        /* Read IRQ */
        EMBARC_PRINTF("sm RF Power Detector fault injection\n");
        for(int i = 0;i<5;i++)
                EMBARC_PRINTF("IRQ_Value%d, =%d\n",i,IRQ_value_rfpower_detector[i]);

        /* sm_rfpower_detector */
        IRQ_value_rfpower_detector = fmcw_radio_sm_rfpower_detector_IRQ(radio,77,1);
        /* Read IRQ */
        EMBARC_PRINTF("sm RF Power Detector\n");
        for(int i = 0;i<5;i++)
                EMBARC_PRINTF("IRQ_Value%d, =%d\n",i,IRQ_value_rfpower_detector[i]);

        /* sm_if_loopback fault injection */
        IRQ_value = fmcw_radio_sm_if_loopback_IRQ(radio,true);
        EMBARC_PRINTF("sm IF Loopback IRQ = %d\n",IRQ_value);

        /* sm_rf_loopback fault injection */
        IRQ_value = fmcw_radio_sm_rf_loopback_IRQ(radio,true);
        EMBARC_PRINTF("sm RF Loopback IRQ = %d\n",IRQ_value);

        /* sm_ldo_monitor fault injection */
        IRQ_value_ldo_monitor = fmcw_radio_sm_ldo_monitor_IRQ(radio,true);
        /* Read IRQ */
        EMBARC_PRINTF("sm LDO Monitor\n");
        for(int i = 0;i<6;i++)
                EMBARC_PRINTF("IRQ_Value%d, =%d\n",i,IRQ_value_ldo_monitor[i]);

        /* sm_avdd33_monitor fault injection */
        IRQ_value = fmcw_radio_sm_avdd33_monitor_IRQ(radio,true);
        EMBARC_PRINTF("sm AVDD33 Monitor IRQ = %d\n",IRQ_value);

        /* sm_dvdd11_monitor fault injection */
        IRQ_value = fmcw_radio_sm_dvdd11_monitor_IRQ(radio,true);
        EMBARC_PRINTF("sm DVDD11 Monitor IRQ = %d\n",IRQ_value);

        /* sm_bg_monitor fault injection */
        IRQ_value = fmcw_radio_sm_bg_monitor_IRQ(radio,true);
        EMBARC_PRINTF("sm BG Monitor IRQ = %d\n",IRQ_value);

        /* sm_cpu_clk_lock_det fault injection */
        IRQ_value = fmcw_radio_sm_cpu_clk_lock_det_IRQ(radio);
        EMBARC_PRINTF("sm CPU Clock Lock Detector IRQ = %d\n",IRQ_value);

        /* sm_chirp_monitor fault injection */
        IRQ_value = fmcw_radio_sm_chirp_monitor_IRQ(radio,true);
        EMBARC_PRINTF("sm Chirp Monitor IRQ = %d\n",IRQ_value);

        /* sm_over_temp_detector fault injection */
        IRQ_value = fmcw_radio_sm_over_temp_detector_IRQ(radio,true);
        EMBARC_PRINTF("sm Over Temp Detector IRQ = %d\n",IRQ_value);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* radio 2.5V and 1.3V ldo under power save mode */
int32_t fmcw_radio_adc_fmcwmmd_ldo_on(fmcw_radio_t *radio, bool enable)
{
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        int ch, down_idle_time, delay_time;
        int pll_settling_time = 400;

        /* set FMCWPLL reg in power save mode */
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, enable);

        /* leave power save */
        if(enable){
                /* auto dis = 0 to disable dis */
                fmcw_radio_switch_bank(radio, 3);
                RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, SEL, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, EN , 0x0);
                /* dis = 0 when leaving power save mode */
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, PFD_DL_DIS, 0x0);
                /* auto dis = 1 when leaving power save mode */
                fmcw_radio_switch_bank(radio, 3);
                RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, SEL, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, EN , 0x1);
                down_idle_time = ceil(cfg->fmcw_chirp_period - cfg->fmcw_chirp_rampup);
                if (down_idle_time < pll_settling_time)
                        delay_time = pll_settling_time;
                else
                        delay_time = pll_settling_time + down_idle_time;
                UDELAY(delay_time);
        }

        /* enter power save */
        else{
                /* auto dis = 0 when entering power save mode */
                fmcw_radio_switch_bank(radio, 3);
                RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, SEL, 0x0);
                RADIO_MOD_BANK_REG(3, AUTO_DL_DIS, EN , 0x0);
                RADIO_MOD_BANK_REG(3, FMCW_START, ADDER_RSTN, 0x0);
                RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x0);
                /* dis = 1 when entering power save mode */
                fmcw_radio_switch_bank(radio, 0);
                RADIO_MOD_BANK_REG(0, FMCWPLL_EN, PFD_DL_DIS, 0x1);
        }

        /* set ADC reg in power save mode */
        fmcw_radio_switch_bank(radio, 1);
        for (ch = 0; ch < MAX_NUM_RX; ch++){
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, RST, 0x0);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, BUFFER_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, BUFFER_VCMBUF_EN, 0x0);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, ANALS_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, OP1_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, OP2_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN0, OP3_EN, enable);

                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, CMP_VCALREF_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, BIAS_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, IDAC1_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, IDAC3_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, ESL_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, REFPBUF_EN, enable);
                RADIO_MOD_BANK_CH_REG(1, ch, ADC_EN1, REFNBUF_EN, enable);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return E_OK;
}

/* heck tx groups configuration to disable txlo buf for power saving purpose */
void fmcw_radio_txlobuf_on(fmcw_radio_t *radio){
        sensor_config_t *cfg = (sensor_config_t *)CONTAINER_OF(radio, baseband_t, radio)->cfg;
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* enable txlo buffer */
        RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG3_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG2_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG1_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG3_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG2_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG1_EN, 0x1);

        /* check if txlo buffer could be off */
        if ((cfg->tx_groups[0] == 0) && (cfg->tx_groups[1] == 0)){
                RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG3_EN, 0x0);
                RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG2_EN, 0x0);
                RADIO_MOD_BANK_REG(0, LO_EN1, LO1_TXDR_STG1_EN, 0x0);
        }
        if ((cfg->tx_groups[2] == 0) && (cfg->tx_groups[3] == 0)){
                RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG3_EN, 0x0);
                RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG2_EN, 0x0);
                RADIO_MOD_BANK_REG(0, LO_EN1, LO2_TXDR_STG1_EN, 0x0);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is a basic setting of Safety Monitor1: LDO Monitor */
void fmcw_radio_sm_ldo_monitor_setting(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* Enable 800M ADC CLK */
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
        /* Enable safety monitor LDO */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);

        /* Enable 5M AUXADC CLK and DTSDM MUXIN SEL */
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, DTSDM_MUXIN_SEL, 0x3);
        /* AUXADC1 on and reset */
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_BUF_BYPASS, 0x0);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_BUF_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_BIAS_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_OP1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_OP2_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_REFGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_VCMGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_RST, 0x1);

        /* load trimming value from the corresponding eFuse address */
        fmcw_radio_auxadc_trim(radio);
        MDELAY(2);

        /* AUXADC1 de-reset */
        RADIO_MOD_BANK_REG(1, DTSDM1_EN, DTSDM_RST, 0x0);
        /* AUXADC1 Filter de-reset */
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, DC_FILTER1_RST_EN, 0x01);
        MDELAY(2);

        /* Set threshold of 1.1V LDO Monitor */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH11150_LOW_0,  0x11);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH11150_LOW_1,  0x91);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH11150_LOW_2,  0x1);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1175_LOW_0,   0xDD);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1175_LOW_1,   0x9D);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1175_LOW_2,   0x1);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1175_HIGH_0,  0x77);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1175_HIGH_1,  0xB7);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1175_HIGH_2,  0x1);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH11150_HIGH_0, 0x44);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH11150_HIGH_1, 0xC4);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH11150_HIGH_2, 0x1);

        /* power on self test */
        if (fault_injection){
                EMBARC_PRINTF("reached sm ldo monitor fault injection\n");
                /* enable LDO Monitor of REFPLL_VCO */
                fmcw_radio_switch_bank(radio, 10);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_EN_3, 0x01);
        }

        /* monitoring */
        else{
                /* Set threshold of 1V LDO Monitor */
                fmcw_radio_switch_bank(radio, 10);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH10150_LOW_0,  0xE0);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH10150_LOW_1,  0x83);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH10150_LOW_2,  0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1075_LOW_0,   0xE0);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1075_LOW_1,   0x83);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1075_LOW_2,   0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1075_HIGH_0,  0x6C);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1075_HIGH_1,  0xB2);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1075_HIGH_2,  0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH10150_HIGH_0, 0x6C);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH10150_HIGH_1, 0xB2);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH10150_HIGH_2, 0x1);

                /* Set threshold of 1.2V LDO Monitor */
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH12150_LOW_0,  0x41);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH12150_LOW_1,  0x9E);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH12150_LOW_2,  0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1275_LOW_0,   0x37);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1275_LOW_1,   0xAC);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1275_LOW_2,   0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1275_HIGH_0,  0x25);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1275_HIGH_1,  0xC8);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1275_HIGH_2,  0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH12150_HIGH_0, 0x1B);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH12150_HIGH_1, 0xD6);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH12150_HIGH_2, 0x1);

                /* Set threshold of 1.3V LDO Monitor */
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH13150_LOW_0,  0x71);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH13150_LOW_1,  0xAB);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH13150_LOW_2,  0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1375_LOW_0,   0x91);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1375_LOW_1,   0xBA);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1375_LOW_2,   0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1375_HIGH_0,  0xD3);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1375_HIGH_1,  0xD8);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH1375_HIGH_2,  0x1);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH13150_HIGH_0, 0xF3);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH13150_HIGH_1, 0xE7);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH13150_HIGH_2, 0x1);

                /* Set threshold of 2.5V LDO Monitor */
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH25150_LOW_0,  0xB2);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH25150_LOW_1,  0x49);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH25150_LOW_2,  0x2);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH2575_LOW_0,   0x7C);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH2575_LOW_1,   0x70);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH2575_LOW_2,   0x2);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH2575_HIGH_0,  0x45);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH2575_HIGH_1,  0x97);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH2575_HIGH_2,  0x2);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH25150_HIGH_0, 0x0F);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH25150_HIGH_1, 0xBE);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_VTH25150_HIGH_2, 0x2);

                /* Enable LDO Monitor */
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_EN_0, 0xFE);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_EN_1, 0x7F);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_EN_2, 0xFF);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_EN_3, 0xFF);
                RADIO_WRITE_BANK_REG(10, LDO_MONITOR_EN_4, 0x7F);
        }

        /* Set threshold of LDO Monitor timing */
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_TIME_CNT_1, 0x3F);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_CTU_SIZE, 0x40);
        RADIO_WRITE_BANK_REG(10, LDO_MONITOR_RSTN, 0x1);

        /* Clear IRQ of LDO Monitor */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_CLEAR, 0x1);
        UDELAY(100);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_CLEAR, 0x0);

        /* Start LDO Monitor */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_TEST_START, 0x1);
        UDELAY(300);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_LDO_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is ldo on setting of Safety Monitor1: LDO Monitor */
uint8_t* fmcw_radio_sm_ldo_monitor_ldo_on(fmcw_radio_t *radio, bool fault_injection){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        static uint8_t old_ldo[5];

        /* enable LDO for lomuxio, tx, bist and LVDS, in normal operation they are disabled */
        old_ldo[0] = RADIO_READ_BANK_REG(0,LO_LDO1);
        RADIO_MOD_BANK_REG(0, LO_LDO1, LDO11_LOMUXIO_EN, 0x1);
        fmcw_radio_switch_bank(radio, 1);
        old_ldo[1] = RADIO_READ_BANK_REG(1,TX_LDO_EN);
        RADIO_WRITE_BANK_REG(1,TX_LDO_EN,0xff);
        fmcw_radio_switch_bank(radio, 2);
        old_ldo[2] = RADIO_READ_BANK_REG(2,BIST_LDO);
        RADIO_MOD_BANK_REG(2, BIST_LDO, LDO11_BIST_EN, 0x1);
        old_ldo[3] = RADIO_READ_BANK_REG(2,LVDS_LDO25);
        RADIO_MOD_BANK_REG(2, LVDS_LDO25, LDO25_LVDS_LDO_EN, 0x1);

        /* power on self test */
        if (fault_injection){
                /* set vco voltage selection to highest vaule */
                fmcw_radio_switch_bank(radio, 0);
                old_ldo[4] = RADIO_READ_BANK_REG(0,REFPLL_LDO2);
                RADIO_MOD_BANK_REG(0, REFPLL_LDO2, LDO11_VCO_VSEL, 0x7);
        }

        fmcw_radio_switch_bank(radio, old_bank);
        return old_ldo;
}

/* This function is ldo off setting of Safety Monitor1: LDO Monitor */
void fmcw_radio_sm_ldo_monitor_ldo_off(fmcw_radio_t *radio, bool fault_injection, uint8_t* old_ldo){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* disable LDO for lomuxio, tx, bist and LVDS for normal operation */
        RADIO_WRITE_BANK_REG(0, LO_LDO1, old_ldo[0]);
        fmcw_radio_switch_bank(radio, 1);
        RADIO_WRITE_BANK_REG(1,TX_LDO_EN,old_ldo[1]);
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, BIST_LDO, old_ldo[2]);
        RADIO_WRITE_BANK_REG(2, LVDS_LDO25, old_ldo[3]);

        /* power on self test */
        if (fault_injection){
                /* set vco voltage selection to highest vaule */
                fmcw_radio_switch_bank(radio, 0);
                RADIO_WRITE_BANK_REG(0, REFPLL_LDO2, old_ldo[4]);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is a basic setting of Safety Monitor2: AVDD33 Monitor */
void fmcw_radio_sm_avdd33_monitor_setting(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* Enable safety monitor LDO */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);

        /* Enable AVDD33 Monitor */
        RADIO_MOD_BANK_REG(0, TEST_CBC2, CBC33_MONITOR_RDAC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, TEST_CBC2, CBC33_MONITOR_EN, 0x1);

        /* Clear IRQ of LDO Monitor */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_CLEAR, 0x1);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_CLEAR, 0x0);

        /* Start LDO Monitor */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_TEST_START, 0x1);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_CBC33_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is threashold setting for Safety Monitor2: AVDD33 Monitor
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
void fmcw_radio_sm_avdd33_monitor_threshold(fmcw_radio_t *radio, bool fault_injection){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* power on self test */
        if (fault_injection){
                EMBARC_PRINTF("reached sm avdd33 monitor fault injection\n");
                /* Set threshold of AVDD33 LDO Monitor as 0 */
                RADIO_WRITE_BANK_REG(0, CBC33_MONITOR0, 0x00);
                RADIO_WRITE_BANK_REG(0, CBC33_MONITOR1, 0x00);
        }

        else{
                /* Set threshold of AVDD33 Monitor */
                RADIO_WRITE_BANK_REG(0, CBC33_MONITOR0, 0x43);
                RADIO_WRITE_BANK_REG(0, CBC33_MONITOR1, 0xBA);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is a basic setting of Safety Monitor3: DVDD11 Monitor */
void fmcw_radio_sm_dvdd11_monitor_setting(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* Enable safety monitor DVDD11 */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);

        /* Enable DVDD11 Monitor */
        RADIO_MOD_BANK_REG(0, TEST_CBC2, DVDD11_MONITOR_EN, 0x1);

        /* Clear IRQ of DVDD11 Monitor */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_CLEAR, 0x1);
        UDELAY(100);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_CLEAR, 0x0);

        /* Start DVDD11 Monitor */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_TEST_START, 0x1);
        UDELAY(300);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_SUPPLY_DVDD11_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is threashold setting for Safety Monitor3: DVDD11 Monitor
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
void fmcw_radio_sm_dvdd11_monitor_threshold(fmcw_radio_t *radio, bool fault_injection){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* power on self test */
        if (fault_injection){
                EMBARC_PRINTF("reached sm dvdd11 monitor fault injection\n");
                RADIO_MOD_BANK_REG(0, DVDD11_MONITOR, DVDD11_MONITOR_VHSEL, 0x0);
                RADIO_MOD_BANK_REG(0, DVDD11_MONITOR, DVDD11_MONITOR_VLSEL, 0x0);
        }

        else{
                /* Set threshold of DVDD11 Monitor */
                RADIO_MOD_BANK_REG(0, DVDD11_MONITOR, DVDD11_MONITOR_VHSEL, 0x1);
                RADIO_MOD_BANK_REG(0, DVDD11_MONITOR, DVDD11_MONITOR_VLSEL, 0x1);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is a basic setting of Safety Monitor4: Bandgap Voltage Monitor */
void fmcw_radio_sm_bg_monitor_setting(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* Enable safety monitor BG */
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);

        /* Enable BG Monitor */
        RADIO_MOD_BANK_REG(0, TEST_CBC2, VBG_MONITOR_EN, 0x1);

        /* Clear IRQ of BG Monitor */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_CLEAR, 0x1);
        UDELAY(100);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_CLEAR, 0x0);

        /* Start BG Monitor */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_TEST_START, 0x1);
        UDELAY(300);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_1, ITF_VBG_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is threashold setting for Safety Monitor4: BG Monitor
 * fault_injection = true: enter power on self test
 * fault_injection = false: leave power on self test/normal monitoring
 */
void fmcw_radio_sm_bg_monitor_threshold(fmcw_radio_t *radio, bool fault_injection){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* power on self test */
        if (fault_injection){
                EMBARC_PRINTF("reached sm bg monitor fault injection\n");
                RADIO_MOD_BANK_REG(0, VBG_MONITOR, VBG_MONITOR_VHSEL, 0x0);
                RADIO_MOD_BANK_REG(0, VBG_MONITOR, VBG_MONITOR_VLSEL, 0x0);
        }

        else{
                /* Set threshold of BG Monitor */
                RADIO_MOD_BANK_REG(0, VBG_MONITOR, VBG_MONITOR_VHSEL, 0x1);
                RADIO_MOD_BANK_REG(0, VBG_MONITOR, VBG_MONITOR_VLSEL, 0x1);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is threashold setting for Safety Monitor4: BG Monitor
 * fault_injection = true: enter power on self test, firmware fleet and restore, IRQ = 0
 * fault_injection = false: normal monitoring, if not safe, firmware fleet
 */
void fmcw_radio_sm_cpu_clk_lock_det_fault_injection(fmcw_radio_t *radio, bool fault_injection){
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* power on self test */
        if (fault_injection){
                EMBARC_PRINTF("reached sm cpu clock lock detector fault injection\n");
                RADIO_MOD_BANK_REG(0, REFPLL_LDO1, LDO25_PLL_LDO_EN, 0x0);
                UDELAY(50);
                /* Fault Injection Restore */
                RADIO_MOD_BANK_REG(0, REFPLL_LDO1, LDO25_PLL_LDO_EN, 0x1);
                UDELAY(50);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/*
 * This function is a basic setting of Safety Monitor: RF Power Detector
 * freq            : lock freq
 * power_th        : rf power lower threahold (min 5dBm)
 * channel_index   : 0/ 1/ 2/ 3/ -1(all channels)
 */
void fmcw_radio_sm_rfpower_detector_setting(fmcw_radio_t *radio, double freq, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* enter shadow bank */
        fmcw_radio_loop_test_en(radio,true);
        /* lock frequency */
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO3, LDO11_MMD_EN, 0x1);
        fmcw_radio_single_tone(radio, freq, true);
        /* leave shadow bank */
        fmcw_radio_loop_test_en(radio,false);

        /* disable over temperature detector, rf pdt will lead to wrong IRQ of temp det */
        fmcw_radio_switch_bank(radio, 10);
        int temp_det_status = RADIO_READ_BANK_REG_FIELD(10, ITF_IRQ_CTRL_5, TEMP_IRQ_RSTN);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, TEMP_IRQ_RSTN, 0x0);

        /* enable auxadc2 */
        fmcw_radio_switch_bank(radio, 0);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_400M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_CLK_800M_ADC_EN, 0x1);
        RADIO_MOD_BANK_REG(0, PLL_CLK_OUT, REFPLL_SDMADC_CLKSEL, 0x1);
        RADIO_MOD_BANK_REG(0, LDO25_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, LDO11_SM, LDO_EN, 0x1);
        RADIO_MOD_BANK_REG(0, TPANA2, TPANA2_EN, 0x1);

        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BUF_BYPASS, 0x0);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BUF_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_BIAS_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_OP1_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_OP2_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_REFGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_VCMGEN_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_RST, 0x1);

        /* load trimming value from the corresponding eFuse address */
        fmcw_radio_auxadc_trim(radio);
        MDELAY(2);

        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, TS_BG_EN, 0x1);
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, TS_VPTAT_CAL, 0x4);
        RADIO_MOD_BANK_REG(1, DTSMD2_MUXIN_SEL, DTSDM_MUXIN_SEL, 0x3);

        RADIO_MOD_BANK_REG(1, DTSDM2_EN, DTSDM_RST, 0x0);
        fmcw_radio_switch_bank(radio, 2);
        RADIO_WRITE_BANK_REG(2, DC_FILTER2_RST_EN, 0x01);
        MDELAY(2);

        /* restore over temperature detector */
        fmcw_radio_switch_bank(radio, 10);
        if(temp_det_status) {
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, TEMP_IRQ_RSTN, 0x1);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_CLEAR, 0x1);
                UDELAY(100);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_CLEAR, 0x0);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_TEST_START, 0x1);
                UDELAY(300);
                RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_TEST_START, 0x0);
        }

        /* set threshold of RF power detector timing */
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TIME_CNT_1, 0x3F);
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_CTU_SIZE, 0x02);

        /* enable RF power detector */
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_RSTN, 0x1);
        /* set voltage high threashold */
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_HIGH_0, 0x00);
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_HIGH_1, 0x7C);
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_HIGH_2, 0x03);

        /* enable TX monitor according to channel_index */
        fmcw_radio_switch_bank(radio, 10);
        switch (channel_index) {
        case -1:
                RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_EN_1, 0x1);
                RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_EN_0, 0xE0);
                break;
        case 0:
                RADIO_MOD_BANK_REG(10, RFPOWER_MONITOR_EN_1, TX0_EN, 0x1);
                break;
        case 1:
                RADIO_MOD_BANK_REG(10, RFPOWER_MONITOR_EN_0, TX1_EN, 0x1);
                break;
        case 2:
                RADIO_MOD_BANK_REG(10, RFPOWER_MONITOR_EN_0, TX2_EN, 0x1);
                break;
        case 3:
                RADIO_MOD_BANK_REG(10, RFPOWER_MONITOR_EN_0, TX3_EN, 0x1);
                break;
        default:
                break;
        }

        /* clear IRQ of rf power detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_CLEAR, 0x1);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_CLEAR, 0x0);
        /* start rf power detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_TEST_START, 0x1);
        MDELAY(1);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_2, ITF_RFPOWER_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is threashold setting for Fault Injection of Safety Monitor: RF Power Detector */
void fmcw_radio_sm_rfpower_detector_fault_injection_threshold(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);

        /* set threashold for power on self test */
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_LOW_2, 0x3);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is threashold setting for Safety Monitor: RF Power Detector */
void fmcw_radio_sm_rfpower_detector_threshold(fmcw_radio_t *radio, double freq, double power_th, int32_t channel_index)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);
        uint32_t pa_on, doutL, doutM, doutH;
        double cal_on, cal_off;

        /* set threashold for monitoring */
        /* enable 5M ADC clock */
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        /* cal on */
        fmcw_radio_pdt_reg(radio, 0, channel_index);
        cal_on = fmcw_radio_auxadc2_dout(radio);
        /* cal off */
        fmcw_radio_pdt_reg(radio, 1, channel_index);
        cal_off = fmcw_radio_auxadc2_dout(radio);
        /* calculate pa on code */
        pa_on = floor(fmcw_get_pa_dout(radio, cal_on, cal_off, channel_index, freq, power_th));
        doutH = pa_on >> 16;
        doutM = (pa_on >> 8) - (doutH << 8);
        doutL = pa_on - (doutM << 8) - (doutH << 16) ;
        /* set voltage low threashold */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_LOW_0, doutL);
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_LOW_1, doutM);
        RADIO_WRITE_BANK_REG(10, RFPOWER_MONITOR_TXVTH_LOW_2, doutH);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is a basic setting of Safety Monitor13: Chirp Monitor */
void fmcw_radio_sm_chirp_monitor_setting(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);

        /* enable Chirp Monitor */
        RADIO_MOD_BANK_REG(0, FMCWPLL_EN, CM_EN, 0x1);
        RADIO_MOD_BANK_REG(0, FMCWPLL_LDO4, LDO11_CM_EN, 0x1);

        /* enable Chirp Monitor IRQ */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_MOD_BANK_REG(10, FM_CTRL_0, FM_TEST_EN, 0x1);
        RADIO_MOD_BANK_REG(10, FM_CTRL_0, FM_RSTN, 0x1);

        /* Reset Release for IRQ of Chirp Detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, FM_IRQ_RSTN, 0x0);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, FM_IRQ_RSTN, 0x1);

        /* Clear IRQ of Chirp Detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_CLEAR, 0x1);
        UDELAY(100);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_CLEAR, 0x0);

        /* Start IRQ of Chirp Detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_TEST_START, 0x1);
        UDELAY(300);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_4, ITF_FM_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

void fmcw_radio_sm_chirp_monitor_fault_injection(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 3);
        /* power on self test */
        if (fault_injection){
                RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x0);
                EMBARC_PRINTF("reached sm chirp monitor fault injection\n");
        }

        /* fault injection restore */
        else{
                RADIO_MOD_BANK_REG(3, FMCW_START, RSTN_SDM_MASH, 0x1);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is a basic setting of Safety Monitor14: Over Temperature Detector */
void fmcw_radio_sm_over_temp_detector_setting(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 1);

        /* get temperature */
        fmcw_radio_get_temperature(radio);
        /* enable 5M AUXADC CLK */
        RADIO_MOD_BANK_REG(1, DTSMD1_MUXIN_SEL, ADC_CLK_5M_EN, 0x1);
        /* Enable Over Temperature Detector */
        fmcw_radio_switch_bank(radio, 10);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, TEMP_IRQ_RSTN, 0x1);
        /* Clear IRQ of Over Temp Detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_CLEAR, 0x1);
        UDELAY(100);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_CLEAR, 0x0);
        /* Trigger rising edge of Over Temp Detector */
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_TEST_START, 0x1);
        UDELAY(300);
        RADIO_MOD_BANK_REG(10, ITF_IRQ_CTRL_5, ITF_TEMP_TEST_START, 0x0);

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function is the threashold setting of Safety Monitor: Over Temperature Detector */
void fmcw_radio_sm_over_temp_detector_threshold(fmcw_radio_t *radio, bool fault_injection)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 10);
        /* power on self test */
        if (fault_injection){
                /* set 125C AUXADC2 Ref Code as 0 to Threadshold */
                RADIO_WRITE_BANK_REG(10, TEMP_MONITOR_VTH_HIGH_2, 0x00);
                RADIO_WRITE_BANK_REG(10, TEMP_MONITOR_VTH_HIGH_1, 0x00);
                RADIO_WRITE_BANK_REG(10, TEMP_MONITOR_VTH_HIGH_0, 0x00);
                EMBARC_PRINTF("reached sm over temperature detector fault injection\n");
        }

        else{
                /* send 125C AUXADC2 Ref Code 0x01A61A to Threadshold */
                RADIO_WRITE_BANK_REG(10, TEMP_MONITOR_VTH_HIGH_2, 0x01);
                RADIO_WRITE_BANK_REG(10, TEMP_MONITOR_VTH_HIGH_1, 0xA6);
                RADIO_WRITE_BANK_REG(10, TEMP_MONITOR_VTH_HIGH_0, 0x1A);
        }

        fmcw_radio_switch_bank(radio, old_bank);
}

/* This function provide RF compensation code vs Junction Temp>55C, 10C/step */
float fmcw_radio_rf_comp_code(fmcw_radio_t *radio)
{
        uint8_t old_bank = fmcw_radio_switch_bank(radio, 0);
        int ch, array_pos;
        float temp;
        /* get desired temperature and compensation code arrays */
        int temp_sel[9]    = {50, 60, 70, 80, 90, 100, 110, 120, 130};
        int lomain_code[9] = {4, 4, 4, 6, 6, 6, 6, 6, 6};
        int txlo_code[9]   = {4, 4, 5, 6, 6, 6, 6, 6, 6};
        int rxlo_code[9]   = {4, 5, 6, 7, 7, 7, 7, 7, 7};
        int rx_code[9]     = {4, 4, 4, 4, 5, 5, 5, 5, 5};
        int tx_code[9]     = {4, 4, 4, 4, 4, 5, 5, 5, 5};
        int pa_code[9]     = {0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xc, 0xd, 0xd};
        /* get temperature */
        temp = fmcw_radio_get_temperature(radio);
        /* round temperture to 10C/step */
        int junc_temp = round(temp / 10.0) * 10;

        if (junc_temp <= 50) {
                /* if junction temperature < 50C, keep default code */
                array_pos = 0;
        } else if (junc_temp >= 130) {
                /* if junction temperature >= 130C, set maximum code */
                array_pos = 8;
        } else {
                /* if 60C < junction temperature < 130C, set code according to array */
                for (array_pos = 1; array_pos < 8; array_pos++) {
                        if (temp_sel[array_pos] == junc_temp)
                                break;
                }
        }

        /* apply compensation code */
        RADIO_MOD_BANK_REG(0, LO_LDO0, LDO11_LOMAINPATH_VOUT_SEL, lomain_code[array_pos]);
        RADIO_MOD_BANK_REG(0, LO_LDO2, LDO11_TXLO_VOUT_SEL, txlo_code[array_pos]);
        RADIO_MOD_BANK_REG(0, LO_LDO3, LDO11_RXLO_VOUT_SEL, rxlo_code[array_pos]);
        RADIO_MOD_BANK_REG(0, RX_LDO0, LDO11_RFN_VOUT_SEL, rx_code[array_pos]);
        RADIO_MOD_BANK_REG(0, RX_LDO1, LDO11_RFS_VOUT_SEL, rx_code[array_pos]);
        fmcw_radio_switch_bank(radio, 1);
        RADIO_MOD_BANK_REG(1, TX_LDO0, LDO11_TX0_VOUT_SEL, tx_code[array_pos]);
        RADIO_MOD_BANK_REG(1, TX_LDO1, LDO11_TX1_VOUT_SEL, tx_code[array_pos]);
        RADIO_MOD_BANK_REG(1, TX_LDO2, LDO11_TX2_VOUT_SEL, tx_code[array_pos]);
        RADIO_MOD_BANK_REG(1, TX_LDO3, LDO11_TX3_VOUT_SEL, tx_code[array_pos]);
        for(ch = 0; ch < MAX_NUM_RX; ch++)
                RADIO_MOD_BANK_CH_REG(1, ch, PA_LDO, LDO11_TX_PA_VOUT_SEL, pa_code[array_pos]);

        fmcw_radio_switch_bank(radio, old_bank);
        return temp;
}

uint32_t* fmcw_doppler_move(fmcw_radio_t *radio)
{
        static uint32_t doppler_move_opt[4];
        uint32_t step_down_opt,down_cycle_opt,wait_cycle_opt,T_dn_test;
        uint32_t bits_frac = 28;
        uint32_t bits_frac_shift = 1 << bits_frac;
        uint32_t T_dn_opt = 0;
        uint32_t T_up = radio->up_cycle;
        uint32_t T_dn = radio->down_cycle;
        uint32_t T_idle = radio->cnt_wait;
        uint32_t T_dn_begin = T_dn;
        double FL_dec_final = (1.0 * radio->start_freq) / bits_frac_shift;
        double FH_dec_final = (1.0 * radio->stop_freq) / bits_frac_shift;
        double Fstep_up_dec_final = (1.0 * radio->step_up) / bits_frac_shift;
        double Fstep_dn_dec_final = (1.0 * radio->step_down) / bits_frac_shift;
        double cal_acc1, acc1, acc1_err;
        double err_min = 1;
        doppler_move_opt[0] = 0;

        /* get doppler spur acc1 value */
        cal_acc1 = (T_up) * FL_dec_final + (T_up-1) * (T_up) / 2 * Fstep_up_dec_final +
                   (T_dn) * FH_dec_final - (T_dn-1) * (T_dn) / 2 * Fstep_dn_dec_final +
                   FL_dec_final * (T_idle-T_dn);
        acc1 = cal_acc1 - floor(cal_acc1);

        for (T_dn_test = T_dn_begin; T_dn_test < T_idle; T_dn_test++) {
                Fstep_dn_dec_final = floor((FH_dec_final - FL_dec_final) / T_dn_test * bits_frac_shift) / bits_frac_shift;
                T_dn = ceil((FH_dec_final-FL_dec_final) / Fstep_dn_dec_final);

                /* Estimate doppler position */
                cal_acc1 = (T_up) * FL_dec_final + (T_up-1) * (T_up) / 2 * Fstep_up_dec_final +
                           (T_dn) * FH_dec_final - (T_dn-1) * (T_dn) / 2 * Fstep_dn_dec_final +
                           FL_dec_final * (T_idle-T_dn);
                acc1 = cal_acc1 - floor(cal_acc1);

                if(acc1<0.5) {
                        acc1_err = fabs(acc1-0.25);
                } else {
                        acc1_err = fabs(acc1-0.75);
                }

                if(acc1_err < err_min) {
                        err_min = acc1_err;
                        T_dn_opt = T_dn_test;
                        doppler_move_opt[0] = 1;
                }
        }

        Fstep_dn_dec_final = floor((FH_dec_final - FL_dec_final) / T_dn_opt* bits_frac_shift) / bits_frac_shift;
        step_down_opt =  floor(Fstep_dn_dec_final * bits_frac_shift);
        T_dn = ceil((FH_dec_final-FL_dec_final) / Fstep_dn_dec_final);
        down_cycle_opt = T_dn;
        wait_cycle_opt = radio->cnt_wait - down_cycle_opt;

        doppler_move_opt[1] = step_down_opt;
        doppler_move_opt[2] = down_cycle_opt;
        doppler_move_opt[3] = wait_cycle_opt;

        return doppler_move_opt;
}
