#ifndef BASEBAND_ALPS_A_REG_H
#define BASEBAND_ALPS_A_REG_H

/*--- ADDRESS ------------------------*/
#define BB_REG_BASEADDR                 0x900000

#define BB_REG_SYS_BASEADDR             (BB_REG_BASEADDR+0x00000)
#define BB_REG_AGC_BASEADDR             (BB_REG_BASEADDR+0x10000)
#define BB_REG_SAM_BASEADDR             (BB_REG_BASEADDR+0x20000)
#define BB_REG_FFT_BASEADDR             (BB_REG_BASEADDR+0x30000)
#define BB_REG_CFR_BASEADDR             (BB_REG_BASEADDR+0x40000)
#define BB_REG_BFM_BASEADDR             (BB_REG_BASEADDR+0x50000)
#define BB_REG_CMB_BASEADDR             (BB_REG_BASEADDR+0x60000)
#define BB_REG_DMP_BASEADDR             (BB_REG_BASEADDR+0x70000)

#define BB_REG_SYS_START                (BB_REG_SYS_BASEADDR+0x00)
#define BB_REG_SYS_ENABLE               (BB_REG_SYS_BASEADDR+0x04)
#define BB_REG_SYS_STATUS               (BB_REG_SYS_BASEADDR+0x08)
#define BB_REG_SYS_IRQ_ENA              (BB_REG_SYS_BASEADDR+0x0C)
#define BB_REG_SYS_IRQ_CLR              (BB_REG_SYS_BASEADDR+0x10)
#define BB_REG_SYS_NUM_OBJ              (BB_REG_SYS_BASEADDR+0x14)
#define BB_REG_SYS_SIZE_RNG_FFT         (BB_REG_SYS_BASEADDR+0x18)
#define BB_REG_SYS_SIZE_RNG_PRD         (BB_REG_SYS_BASEADDR+0x1C)
#define BB_REG_SYS_SIZE_RNG_SKP         (BB_REG_SYS_BASEADDR+0x20)
#define BB_REG_SYS_SIZE_RNG_BUF         (BB_REG_SYS_BASEADDR+0x24)
#define BB_REG_SYS_SIZE_RNG_ALL         (BB_REG_SYS_BASEADDR+0x28)
#define BB_REG_SYS_SIZE_BPM             (BB_REG_SYS_BASEADDR+0x2C)
#define BB_REG_SYS_SIZE_VEL_FFT         (BB_REG_SYS_BASEADDR+0x30)
#define BB_REG_SYS_SIZE_VEL_BUF         (BB_REG_SYS_BASEADDR+0x34)
#define BB_REG_SYS_SIZE_ANG             (BB_REG_SYS_BASEADDR+0x38)
#define BB_REG_SYS_SIZE_MIM             (BB_REG_SYS_BASEADDR+0x3C)
#define BB_REG_SYS_SIZE_OBJ             (BB_REG_SYS_BASEADDR+0x40)
#define BB_REG_SYS_MEM_ACT              (BB_REG_SYS_BASEADDR+0x44)
#define BB_REG_SYS_BUF_STORE            (BB_REG_SYS_BASEADDR+0x48)
#define BB_REG_SYS_TST_CNT              (BB_REG_SYS_BASEADDR+0x4C)
#define BB_REG_SYS_TST_PRD              (BB_REG_SYS_BASEADDR+0x50)
#define BB_REG_SYS_TST_DAT              (BB_REG_SYS_BASEADDR+0x54)
#define BB_REG_SYS_TST_MSK              (BB_REG_SYS_BASEADDR+0x58)

#define BB_REG_AGC_PERIOD               (BB_REG_AGC_BASEADDR+0x00)
#define BB_REG_AGC_CNT_TIA_C0           (BB_REG_AGC_BASEADDR+0x04)
#define BB_REG_AGC_CNT_TIA_C1           (BB_REG_AGC_BASEADDR+0x08)
#define BB_REG_AGC_CNT_TIA_C2           (BB_REG_AGC_BASEADDR+0x0C)
#define BB_REG_AGC_CNT_TIA_C3           (BB_REG_AGC_BASEADDR+0x10)
#define BB_REG_AGC_CNT_VGA1_C0          (BB_REG_AGC_BASEADDR+0x14)
#define BB_REG_AGC_CNT_VGA1_C1          (BB_REG_AGC_BASEADDR+0x18)
#define BB_REG_AGC_CNT_VGA1_C2          (BB_REG_AGC_BASEADDR+0x1C)
#define BB_REG_AGC_CNT_VGA1_C3          (BB_REG_AGC_BASEADDR+0x20)
#define BB_REG_AGC_CNT_VGA2_C0          (BB_REG_AGC_BASEADDR+0x24)
#define BB_REG_AGC_CNT_VGA2_C1          (BB_REG_AGC_BASEADDR+0x28)
#define BB_REG_AGC_CNT_VGA2_C2          (BB_REG_AGC_BASEADDR+0x2C)
#define BB_REG_AGC_CNT_VGA2_C3          (BB_REG_AGC_BASEADDR+0x30)
#define BB_REG_AGC_MAX_1ST_C0           (BB_REG_AGC_BASEADDR+0x34)
#define BB_REG_AGC_MAX_1ST_C1           (BB_REG_AGC_BASEADDR+0x38)
#define BB_REG_AGC_MAX_1ST_C2           (BB_REG_AGC_BASEADDR+0x3C)
#define BB_REG_AGC_MAX_1ST_C3           (BB_REG_AGC_BASEADDR+0x40)
#define BB_REG_AGC_MAX_2ND_C0           (BB_REG_AGC_BASEADDR+0x44)
#define BB_REG_AGC_MAX_2ND_C1           (BB_REG_AGC_BASEADDR+0x48)
#define BB_REG_AGC_MAX_2ND_C2           (BB_REG_AGC_BASEADDR+0x4C)
#define BB_REG_AGC_MAX_2ND_C3           (BB_REG_AGC_BASEADDR+0x50)
#define BB_REG_AGC_MAX_3RD_C0           (BB_REG_AGC_BASEADDR+0x54)
#define BB_REG_AGC_MAX_3RD_C1           (BB_REG_AGC_BASEADDR+0x58)
#define BB_REG_AGC_MAX_3RD_C2           (BB_REG_AGC_BASEADDR+0x5C)
#define BB_REG_AGC_MAX_3RD_C3           (BB_REG_AGC_BASEADDR+0x60)

#define BB_REG_SAM_FILT_CNT             (BB_REG_SAM_BASEADDR+0x00)
#define BB_REG_SAM_F_0_S1               (BB_REG_SAM_BASEADDR+0x04)
#define BB_REG_SAM_F_0_B1               (BB_REG_SAM_BASEADDR+0x08)
#define BB_REG_SAM_F_0_A1               (BB_REG_SAM_BASEADDR+0x0C)
#define BB_REG_SAM_F_0_A2               (BB_REG_SAM_BASEADDR+0x10)
#define BB_REG_SAM_F_1_S1               (BB_REG_SAM_BASEADDR+0x14)
#define BB_REG_SAM_F_1_B1               (BB_REG_SAM_BASEADDR+0x18)
#define BB_REG_SAM_F_1_A1               (BB_REG_SAM_BASEADDR+0x1C)
#define BB_REG_SAM_F_1_A2               (BB_REG_SAM_BASEADDR+0x20)
#define BB_REG_SAM_F_2_S1               (BB_REG_SAM_BASEADDR+0x24)
#define BB_REG_SAM_F_2_B1               (BB_REG_SAM_BASEADDR+0x28)
#define BB_REG_SAM_F_2_A1               (BB_REG_SAM_BASEADDR+0x2C)
#define BB_REG_SAM_F_2_A2               (BB_REG_SAM_BASEADDR+0x30)
#define BB_REG_SAM_F_3_S1               (BB_REG_SAM_BASEADDR+0x34)
#define BB_REG_SAM_F_3_B1               (BB_REG_SAM_BASEADDR+0x38)
#define BB_REG_SAM_F_3_A1               (BB_REG_SAM_BASEADDR+0x3C)
#define BB_REG_SAM_F_3_A2               (BB_REG_SAM_BASEADDR+0x40)
#define BB_REG_SAM_FNL_SHF              (BB_REG_SAM_BASEADDR+0x44)
#define BB_REG_SAM_FNL_SCL              (BB_REG_SAM_BASEADDR+0x48)
#define BB_REG_SAM_DE_INT_ENA           (BB_REG_SAM_BASEADDR+0x4C)
#define BB_REG_SAM_DE_INT_DAT           (BB_REG_SAM_BASEADDR+0x50)
#define BB_REG_SAM_DE_INT_MSK           (BB_REG_SAM_BASEADDR+0x54)
#define BB_REG_SAM_FORCE                (BB_REG_SAM_BASEADDR+0x58)
#define BB_REG_SAM_SIZE                 (BB_REG_SAM_BASEADDR+0x5C)

#define BB_REG_FFT_SHFT_RNG             (BB_REG_FFT_BASEADDR+0x00)
#define BB_REG_FFT_SHFT_VEL             (BB_REG_FFT_BASEADDR+0x04)

#define BB_REG_CFR_TYPE                 (BB_REG_CFR_BASEADDR+0x00)
#define BB_REG_CFR_BACK_RNG             (BB_REG_CFR_BASEADDR+0x04)
#define BB_REG_CFR_CA_DATA_SCL          (BB_REG_CFR_BASEADDR+0x08)
#define BB_REG_CFR_CA_MASK_0            (BB_REG_CFR_BASEADDR+0x0C)
#define BB_REG_CFR_CA_MASK_1            (BB_REG_CFR_BASEADDR+0x10)
#define BB_REG_CFR_CA_MASK_2            (BB_REG_CFR_BASEADDR+0x14)
#define BB_REG_CFR_CA_MASK_3            (BB_REG_CFR_BASEADDR+0x18)
#define BB_REG_CFR_CA_MASK_4            (BB_REG_CFR_BASEADDR+0x1C)
#define BB_REG_CFR_CA_MASK_5            (BB_REG_CFR_BASEADDR+0x20)
#define BB_REG_CFR_CA_MASK_6            (BB_REG_CFR_BASEADDR+0x24)
#define BB_REG_CFR_OS_DATA_SCL          (BB_REG_CFR_BASEADDR+0x28)
#define BB_REG_CFR_OS_DATA_THR          (BB_REG_CFR_BASEADDR+0x2C)
#define BB_REG_CFR_OS_MASK_0            (BB_REG_CFR_BASEADDR+0x30)
#define BB_REG_CFR_OS_MASK_1            (BB_REG_CFR_BASEADDR+0x34)
#define BB_REG_CFR_OS_MASK_2            (BB_REG_CFR_BASEADDR+0x38)
#define BB_REG_CFR_OS_MASK_3            (BB_REG_CFR_BASEADDR+0x3C)
#define BB_REG_CFR_OS_MASK_4            (BB_REG_CFR_BASEADDR+0x40)
#define BB_REG_CFR_OS_MASK_5            (BB_REG_CFR_BASEADDR+0x44)
#define BB_REG_CFR_OS_MASK_6            (BB_REG_CFR_BASEADDR+0x48)
#define BB_REG_CFR_PK_DATA_THR          (BB_REG_CFR_BASEADDR+0x4C)
#define BB_REG_CFR_PK_MASK_0            (BB_REG_CFR_BASEADDR+0x50)
#define BB_REG_CFR_PK_MASK_1            (BB_REG_CFR_BASEADDR+0x54)
#define BB_REG_CFR_PK_MASK_2            (BB_REG_CFR_BASEADDR+0x58)
#define BB_REG_CFR_PK_MASK_3            (BB_REG_CFR_BASEADDR+0x5C)
#define BB_REG_CFR_PK_MASK_4            (BB_REG_CFR_BASEADDR+0x60)
#define BB_REG_CFR_PK_MASK_5            (BB_REG_CFR_BASEADDR+0x64)
#define BB_REG_CFR_PK_MASK_6            (BB_REG_CFR_BASEADDR+0x68)

#define BB_REG_BFM_NUMB_SCH             (BB_REG_BFM_BASEADDR+0x00)
#define BB_REG_BFM_SCL_SIG              (BB_REG_BFM_BASEADDR+0x04)
#define BB_REG_BFM_SCL_NOI              (BB_REG_BFM_BASEADDR+0x08)

#define BB_REG_CMB_TYPE                 (BB_REG_CMB_BASEADDR+0x00)

#define BB_REG_DMP_MODE                 (BB_REG_DMP_BASEADDR+0x00)
#define BB_REG_DMP_SIZE                 (BB_REG_DMP_BASEADDR+0x04)
#define BB_REG_DMP_ENAB                 (BB_REG_DMP_BASEADDR+0x08)

#define BB_MEM_BASEADDR                 0xA00000
#define BB_MEM_BASEADDR_CH_OFFSET       0x040000


/*--- VALUE, MASK OR SHIFT -----------*/
#define BB_REG_SYS_MEM_ACT_NONE          0
#define BB_REG_SYS_MEM_ACT_WIN           1
#define BB_REG_SYS_MEM_ACT_BUF           2
#define BB_REG_SYS_MEM_ACT_COE           3
#define BB_REG_SYS_MEM_ACT_MAC           4
#define BB_REG_SYS_MEM_ACT_RLT           5
#define BB_REG_SYS_MEM_ACT_SHP           6

#define BB_REG_SYS_ENABLE_TST_SHIFT     7
#define BB_REG_SYS_ENABLE_TST_MASK      0x1
#define BB_REG_SYS_ENABLE_HIL_SHIFT     6
#define BB_REG_SYS_ENABLE_HIL_MASK      0x1
#define BB_REG_SYS_ENABLE_AGC_SHIFT     5
#define BB_REG_SYS_ENABLE_AGC_MASK      0x1
#define BB_REG_SYS_ENABLE_SAM_SHIFT     4
#define BB_REG_SYS_ENABLE_SAM_MASK      0x1
#define BB_REG_SYS_ENABLE_FFT_2D_SHIFT  3
#define BB_REG_SYS_ENABLE_FFT_2D_MASK   0x1
#define BB_REG_SYS_ENABLE_CFR_SHIFT     2
#define BB_REG_SYS_ENABLE_CFR_MASK      0x1
#define BB_REG_SYS_ENABLE_BFM_SHIFT     1
#define BB_REG_SYS_ENABLE_BFM_MASK      0x1
#define BB_REG_SYS_ENABLE_DMP_SHIFT     0
#define BB_REG_SYS_ENABLE_DMP_MASK      0x1

#define BB_REG_CFR_TYPE_CA              0
#define BB_REG_CFR_TYPE_OS              1
#define BB_REG_CFR_TYPE_CA_OR_OS        2
#define BB_REG_CFR_TYPE_CA_AND_OS       3

#define BB_REG_DMP_MODE_SELF_CTL        0
#define BB_REG_DMP_MODE_HOST_CTL        1

#define BB_REG_SYS_BUF_STORE_FFT        0
#define BB_REG_SYS_BUF_STORE_ADC        1

#define BB_REG_CMB_TYPE_MAGN            0
#define BB_REG_CMB_TYPE_MIMO            1


/*--- TYPEDEF -------------------------*/
typedef struct {
        uint32_t vel      : 16;    /* 0x00 */
        uint32_t rng      : 16;
        uint32_t ang_0    : 16;    /* 0x04 */
        uint32_t is_obj_1 :  1;
        uint32_t is_obj_2 :  1;
        uint32_t is_obj_3 : 14;
        uint32_t ang_1    :  9;    /* 0x08 */
        uint32_t ang_2    :  9;
        uint32_t ang_3    : 14;
        uint32_t noi          ;    /* 0x0C */
        uint32_t sig_0        ;    /* 0x10 */
        uint32_t sig_1        ;    /* 0x14 */
        uint32_t sig_2        ;    /* 0x18 */
        uint32_t sig_3        ;    /* 0x1C */
} obj_info_t;

#endif
