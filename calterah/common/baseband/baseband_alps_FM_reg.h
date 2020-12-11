#ifndef BASEBAND_ALPS_FM_REG_H
#define BASEBAND_ALPS_FM_REG_H


/*--- ADDRESS ------------------------*/
#define BB_REG_BASEADDR                 0xC00000

#define BB_REG_SYS_BASEADDR             (BB_REG_BASEADDR+0x000)
#define BB_REG_AGC_BASEADDR             (BB_REG_BASEADDR+0x200)
#define BB_REG_SAM_BASEADDR             (BB_REG_BASEADDR+0x400)
#define BB_REG_FFT_BASEADDR             (BB_REG_BASEADDR+0x600)
#define BB_REG_CFR_BASEADDR             (BB_REG_BASEADDR+0x800)
#define BB_REG_DOA_BASEADDR             (BB_REG_BASEADDR+0xa00)
#define BB_REG_DBG_BASEADDR             (BB_REG_BASEADDR+0xc00)
#define BB_REG_DML_BASEADDR             (BB_REG_BASEADDR+0xe00)

/*sys register*/
#define BB_REG_SYS_START                (BB_REG_SYS_BASEADDR+0x0)
#define BB_REG_SYS_BNK_MODE             (BB_REG_SYS_BASEADDR+0x4)
#define BB_REG_SYS_BNK_ACT              (BB_REG_SYS_BASEADDR+0x8)
#define BB_REG_SYS_BNK_QUE              (BB_REG_SYS_BASEADDR+0xC)
#define BB_REG_SYS_BNK_RST              (BB_REG_SYS_BASEADDR+0x10)
#define BB_REG_SYS_MEM_ACT              (BB_REG_SYS_BASEADDR+0x14)
#define BB_REG_SYS_ENABLE               (BB_REG_SYS_BASEADDR+0x18)
#define BB_REG_SYS_TYPE_FMCW            (BB_REG_SYS_BASEADDR+0x1C)
#define BB_REG_SYS_SIZE_RNG_PRD         (BB_REG_SYS_BASEADDR+0x20)
#define BB_REG_SYS_SIZE_FLT             (BB_REG_SYS_BASEADDR+0x24)
#define BB_REG_SYS_SIZE_RNG_SKP         (BB_REG_SYS_BASEADDR+0x28)
#define BB_REG_SYS_SIZE_RNG_BUF         (BB_REG_SYS_BASEADDR+0x2C)
#define BB_REG_SYS_SIZE_RNG_FFT         (BB_REG_SYS_BASEADDR+0x30)
#define BB_REG_SYS_SIZE_BPM             (BB_REG_SYS_BASEADDR+0x34)
#define BB_REG_SYS_SIZE_VEL_BUF         (BB_REG_SYS_BASEADDR+0x38)
#define BB_REG_SYS_SIZE_VEL_FFT         (BB_REG_SYS_BASEADDR+0x3C)
#define BB_REG_SYS_FRMT_ADC             (BB_REG_SYS_BASEADDR+0x40)
#define BB_REG_SYS_IRQ_ENA              (BB_REG_SYS_BASEADDR+0x44)
#define BB_REG_SYS_IRQ_CLR              (BB_REG_SYS_BASEADDR+0x48)
#define BB_REG_SYS_ECC_ENA              (BB_REG_SYS_BASEADDR+0x4C)
#define BB_REG_SYS_ECC_SB_CLR           (BB_REG_SYS_BASEADDR+0x50)
#define BB_REG_SYS_ECC_DB_CLR           (BB_REG_SYS_BASEADDR+0x54)
#define BB_REG_SYS_TYP_ARB              (BB_REG_SYS_BASEADDR+0x58)
#define BB_REG_SYS_STATUS               (BB_REG_SYS_BASEADDR+0x5C)
#define BB_REG_FDB_SYS_BNK_ACT          (BB_REG_SYS_BASEADDR+0x60)
#define BB_REG_SYS_IRQ_STATUS           (BB_REG_SYS_BASEADDR+0x64)
#define BB_REG_SYS_ECC_SB_STATUS        (BB_REG_SYS_BASEADDR+0x68)
#define BB_REG_SYS_ECC_DB_STATUS        (BB_REG_SYS_BASEADDR+0x6C)

/*AGC regesiter*/
#define BB_REG_AGC_SAT_THR_TIA          (BB_REG_AGC_BASEADDR+0x00)
#define BB_REG_AGC_SAT_THR_VGA1         (BB_REG_AGC_BASEADDR+0x04)
#define BB_REG_AGC_SAT_THR_VGA2         (BB_REG_AGC_BASEADDR+0x08)
#define BB_REG_AGC_SAT_CNT_CLR_FRA      (BB_REG_AGC_BASEADDR+0x0C)
#define BB_REG_AGC_DAT_MAX_SEL          (BB_REG_AGC_BASEADDR+0x10)
#define BB_REG_AGC_CODE_LNA_0           (BB_REG_AGC_BASEADDR+0x14)
#define BB_REG_AGC_CODE_LNA_1           (BB_REG_AGC_BASEADDR+0x18)
#define BB_REG_AGC_CODE_TIA_0           (BB_REG_AGC_BASEADDR+0x1C)
#define BB_REG_AGC_CODE_TIA_1           (BB_REG_AGC_BASEADDR+0x20)
#define BB_REG_AGC_CODE_TIA_2           (BB_REG_AGC_BASEADDR+0x24)
#define BB_REG_AGC_CODE_TIA_3           (BB_REG_AGC_BASEADDR+0x28)
#define BB_REG_AGC_GAIN_MIN             (BB_REG_AGC_BASEADDR+0x2C)
#define BB_REG_AGC_CDGN_INIT            (BB_REG_AGC_BASEADDR+0x30)
#define BB_REG_AGC_CDGN_C0_0            (BB_REG_AGC_BASEADDR+0x34)
#define BB_REG_AGC_CDGN_C0_1            (BB_REG_AGC_BASEADDR+0x38)
#define BB_REG_AGC_CDGN_C0_2            (BB_REG_AGC_BASEADDR+0x3C)
#define BB_REG_AGC_CDGN_C1_0            (BB_REG_AGC_BASEADDR+0x40)
#define BB_REG_AGC_CDGN_C1_1            (BB_REG_AGC_BASEADDR+0x44)
#define BB_REG_AGC_CDGN_C1_2            (BB_REG_AGC_BASEADDR+0x48)
#define BB_REG_AGC_CDGN_C1_3            (BB_REG_AGC_BASEADDR+0x4C)
#define BB_REG_AGC_CDGN_C1_4            (BB_REG_AGC_BASEADDR+0x50)
#define BB_REG_AGC_CDGN_C1_5            (BB_REG_AGC_BASEADDR+0x54)
#define BB_REG_AGC_CDGN_C1_6            (BB_REG_AGC_BASEADDR+0x58)
#define BB_REG_AGC_CDGN_C1_7            (BB_REG_AGC_BASEADDR+0x5C)
#define BB_REG_AGC_CDGN_C1_8            (BB_REG_AGC_BASEADDR+0x60)
#define BB_REG_AGC_CHCK_ENA             (BB_REG_AGC_BASEADDR+0x64)
#define BB_REG_AGC_ALIGN_EN             (BB_REG_AGC_BASEADDR+0x68)
#define BB_REG_AGC_CMPN_EN              (BB_REG_AGC_BASEADDR+0x6C)
#define BB_REG_AGC_CMPN_ALIGN_EN        (BB_REG_AGC_BASEADDR+0x70)
#define BB_REG_AGC_CMPN_LVL             (BB_REG_AGC_BASEADDR+0x74)
#define BB_REG_AGC_DB_TARGET            (BB_REG_AGC_BASEADDR+0x78)
#define BB_REG_AGC_IRQ_ENA              (BB_REG_AGC_BASEADDR+0x7C)
#define BB_REG_AGC_IRQ_CLR              (BB_REG_AGC_BASEADDR+0x80)
#define BB_REG_AGC_COD_C0               (BB_REG_AGC_BASEADDR+0x84)
#define BB_REG_AGC_COD_C1               (BB_REG_AGC_BASEADDR+0x88)
#define BB_REG_AGC_COD_C2               (BB_REG_AGC_BASEADDR+0x8C)
#define BB_REG_AGC_COD_C3               (BB_REG_AGC_BASEADDR+0x90)
#define BB_REG_AGC_SAT_CNT_TIA__C0      (BB_REG_AGC_BASEADDR+0x94)
#define BB_REG_AGC_SAT_CNT_TIA__C1      (BB_REG_AGC_BASEADDR+0x98)
#define BB_REG_AGC_SAT_CNT_TIA__C2      (BB_REG_AGC_BASEADDR+0x9C)
#define BB_REG_AGC_SAT_CNT_TIA__C3      (BB_REG_AGC_BASEADDR+0xA0)
#define BB_REG_AGC_SAT_CNT_VGA1_C0      (BB_REG_AGC_BASEADDR+0xA4)
#define BB_REG_AGC_SAT_CNT_VGA1_C1      (BB_REG_AGC_BASEADDR+0xA8)
#define BB_REG_AGC_SAT_CNT_VGA1_C2      (BB_REG_AGC_BASEADDR+0xAC)
#define BB_REG_AGC_SAT_CNT_VGA1_C3      (BB_REG_AGC_BASEADDR+0xB0)
#define BB_REG_AGC_SAT_CNT_VGA2_C0      (BB_REG_AGC_BASEADDR+0xB4)
#define BB_REG_AGC_SAT_CNT_VGA2_C1      (BB_REG_AGC_BASEADDR+0xB8)
#define BB_REG_AGC_SAT_CNT_VGA2_C2      (BB_REG_AGC_BASEADDR+0xBC)
#define BB_REG_AGC_SAT_CNT_VGA2_C3      (BB_REG_AGC_BASEADDR+0xC0)
#define BB_REG_AGC_DAT_MAX_1ST_C0       (BB_REG_AGC_BASEADDR+0xC4)
#define BB_REG_AGC_DAT_MAX_1ST_C1       (BB_REG_AGC_BASEADDR+0xC8)
#define BB_REG_AGC_DAT_MAX_1ST_C2       (BB_REG_AGC_BASEADDR+0xCC)
#define BB_REG_AGC_DAT_MAX_1ST_C3       (BB_REG_AGC_BASEADDR+0xD0)
#define BB_REG_AGC_DAT_MAX_2ND_C0       (BB_REG_AGC_BASEADDR+0xD4)
#define BB_REG_AGC_DAT_MAX_2ND_C1       (BB_REG_AGC_BASEADDR+0xD8)
#define BB_REG_AGC_DAT_MAX_2ND_C2       (BB_REG_AGC_BASEADDR+0xDC)
#define BB_REG_AGC_DAT_MAX_2ND_C3       (BB_REG_AGC_BASEADDR+0xE0)
#define BB_REG_AGC_DAT_MAX_3RD_C0       (BB_REG_AGC_BASEADDR+0xE4)
#define BB_REG_AGC_DAT_MAX_3RD_C1       (BB_REG_AGC_BASEADDR+0xE8)
#define BB_REG_AGC_DAT_MAX_3RD_C2       (BB_REG_AGC_BASEADDR+0xEC)
#define BB_REG_AGC_DAT_MAX_3RD_C3       (BB_REG_AGC_BASEADDR+0xF0)
#define BB_REG_AGC_IRQ_STATUS           (BB_REG_AGC_BASEADDR+0xF4)


/*SAM register*/
#define BB_REG_SAM_SINKER               (BB_REG_SAM_BASEADDR+0x00)
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
#define BB_REG_SAM_DINT_ENA             (BB_REG_SAM_BASEADDR+0x4C)
#define BB_REG_SAM_DINT_MOD             (BB_REG_SAM_BASEADDR+0x50)
#define BB_REG_SAM_DINT_SET             (BB_REG_SAM_BASEADDR+0x54)
#define BB_REG_SAM_DINT_DAT             (BB_REG_SAM_BASEADDR+0x58)
#define BB_REG_SAM_DINT_MSK             (BB_REG_SAM_BASEADDR+0x5C)
#define BB_REG_SAM_DAMB_PRD             (BB_REG_SAM_BASEADDR+0x60)
#define BB_REG_SAM_FORCE                (BB_REG_SAM_BASEADDR+0x64)
#define BB_REG_SAM_DBG_SRC              (BB_REG_SAM_BASEADDR+0x68)
#define BB_REG_SAM_SIZE_DBG_BGN         (BB_REG_SAM_BASEADDR+0x6C)
#define BB_REG_SAM_SIZE_DBG_END         (BB_REG_SAM_BASEADDR+0x70)
#define BB_REG_SAM_SPK_RM_EN            (BB_REG_SAM_BASEADDR+0x74)
#define BB_REG_SAM_SPK_CFM_SIZE         (BB_REG_SAM_BASEADDR+0x78)
#define BB_REG_SAM_SPK_SET_ZERO         (BB_REG_SAM_BASEADDR+0x7C)
#define BB_REG_SAM_SPK_OVER_NUM         (BB_REG_SAM_BASEADDR+0x80)
#define BB_REG_SAM_SPK_THRES_DBL        (BB_REG_SAM_BASEADDR+0x84)
#define BB_REG_SAM_SPK_MIN_MAX_SEL      (BB_REG_SAM_BASEADDR+0x88)
#define BB_REG_FDB_SAM_DINT_DAT         (BB_REG_SAM_BASEADDR+0x8C)


/*FFT register*/
#define BB_REG_FFT_SHFT_RNG             (BB_REG_FFT_BASEADDR+0x00)
#define BB_REG_FFT_SHFT_VEL             (BB_REG_FFT_BASEADDR+0x04)
#define BB_REG_FFT_DBPM_ENA             (BB_REG_FFT_BASEADDR+0x08)
#define BB_REG_FFT_DBPM_DIR             (BB_REG_FFT_BASEADDR+0x0C)
#define BB_REG_FFT_DAMB_ENA             (BB_REG_FFT_BASEADDR+0x10)
#define BB_REG_FFT_DINT_ENA             (BB_REG_FFT_BASEADDR+0x14)
#define BB_REG_FFT_DINT_MOD             (BB_REG_FFT_BASEADDR+0x18)
#define BB_REG_FFT_DINT_SET             (BB_REG_FFT_BASEADDR+0x1C)
#define BB_REG_FFT_DINT_DAT_FH          (BB_REG_FFT_BASEADDR+0x20)
#define BB_REG_FFT_DINT_DAT_CS          (BB_REG_FFT_BASEADDR+0x24)
#define BB_REG_FFT_DINT_DAT_PS          (BB_REG_FFT_BASEADDR+0x28)
#define BB_REG_FFT_DINT_MSK_FH          (BB_REG_FFT_BASEADDR+0x2C)
#define BB_REG_FFT_DINT_MSK_CS          (BB_REG_FFT_BASEADDR+0x30)
#define BB_REG_FFT_DINT_MSK_PS          (BB_REG_FFT_BASEADDR+0x34)
#define BB_REG_FFT_DINT_COE_FH          (BB_REG_FFT_BASEADDR+0x38)
#define BB_REG_FFT_DINT_COE_CS          (BB_REG_FFT_BASEADDR+0x3C)
#define BB_REG_FFT_DINT_COE_FC          (BB_REG_FFT_BASEADDR+0x40)
#define BB_REG_FFT_DINT_COE_PS_0        (BB_REG_FFT_BASEADDR+0x44)
#define BB_REG_FFT_DINT_COE_PS_1        (BB_REG_FFT_BASEADDR+0x48)
#define BB_REG_FFT_DINT_COE_PS_2        (BB_REG_FFT_BASEADDR+0x4C)
#define BB_REG_FFT_DINT_COE_PS_3        (BB_REG_FFT_BASEADDR+0x50)
#define BB_REG_FFT_NO_WIN               (BB_REG_FFT_BASEADDR+0x54)
#define BB_REG_FFT_NVE_MODE             (BB_REG_FFT_BASEADDR+0x58)
#define BB_REG_FFT_NVE_SCL_0            (BB_REG_FFT_BASEADDR+0x5C)
#define BB_REG_FFT_NVE_SCL_1            (BB_REG_FFT_BASEADDR+0x60)
#define BB_REG_FFT_NVE_SFT              (BB_REG_FFT_BASEADDR+0x64)
#define BB_REG_FFT_NVE_CH_MSK           (BB_REG_FFT_BASEADDR+0x68)
#define BB_REG_FFT_NVE_DFT_VALUE        (BB_REG_FFT_BASEADDR+0x6C)
#define BB_REG_FFT_ZER_DPL_ENB          (BB_REG_FFT_BASEADDR+0x70)
#define BB_REG_FDB_FFT_DINT_DAT_FH      (BB_REG_FFT_BASEADDR+0x74)
#define BB_REG_FDB_FFT_DINT_DAT_CS      (BB_REG_FFT_BASEADDR+0x78)
#define BB_REG_FDB_FFT_DINT_DAT_PS      (BB_REG_FFT_BASEADDR+0x7C)

/* CFAR register*/
#define BB_REG_CFR_SIZE_OBJ             (BB_REG_CFR_BASEADDR+0x00) /*upper bound for object number*/
#define BB_REG_CFR_BACK_RNG             (BB_REG_CFR_BASEADDR+0x04)
#define BB_REG_CFR_TYPE_CMB             (BB_REG_CFR_BASEADDR+0x08)
#define BB_REG_CFR_MIMO_NUM             (BB_REG_CFR_BASEADDR+0x0C)
#define BB_REG_CFR_MIMO_ADR             (BB_REG_CFR_BASEADDR+0x10)
#define BB_REG_CFR_PRT_VEL_00           (BB_REG_CFR_BASEADDR+0x14)
#define BB_REG_CFR_PRT_VEL_01           (BB_REG_CFR_BASEADDR+0x18)
#define BB_REG_CFR_PRT_VEL_10           (BB_REG_CFR_BASEADDR+0x1C)
#define BB_REG_CFR_PRT_VEL_11           (BB_REG_CFR_BASEADDR+0x20)
#define BB_REG_CFR_PRT_VEL_20           (BB_REG_CFR_BASEADDR+0x24)
#define BB_REG_CFR_PRT_VEL_21           (BB_REG_CFR_BASEADDR+0x28)
#define BB_REG_CFR_PRT_VEL_30           (BB_REG_CFR_BASEADDR+0x2C)
#define BB_REG_CFR_PRT_VEL_31           (BB_REG_CFR_BASEADDR+0x30)
#define BB_REG_CFR_PRT_RNG_00           (BB_REG_CFR_BASEADDR+0x34)
#define BB_REG_CFR_PRT_RNG_01           (BB_REG_CFR_BASEADDR+0x38)
#define BB_REG_CFR_PRT_RNG_02           (BB_REG_CFR_BASEADDR+0x3C)
#define BB_REG_CFR_TYP_AL               (BB_REG_CFR_BASEADDR+0x40)
#define BB_REG_CFR_TYP_DS               (BB_REG_CFR_BASEADDR+0x44)
#define BB_REG_CFR_PRM_0_0              (BB_REG_CFR_BASEADDR+0x48)
#define BB_REG_CFR_PRM_0_1              (BB_REG_CFR_BASEADDR+0x4C)
#define BB_REG_CFR_PRM_1_0              (BB_REG_CFR_BASEADDR+0x50)
#define BB_REG_CFR_PRM_1_1              (BB_REG_CFR_BASEADDR+0x54)
#define BB_REG_CFR_PRM_1_2              (BB_REG_CFR_BASEADDR+0x58)
#define BB_REG_CFR_PRM_2_0              (BB_REG_CFR_BASEADDR+0x5C)
#define BB_REG_CFR_PRM_2_1              (BB_REG_CFR_BASEADDR+0x60)
#define BB_REG_CFR_PRM_2_2              (BB_REG_CFR_BASEADDR+0x64)
#define BB_REG_CFR_PRM_3_0              (BB_REG_CFR_BASEADDR+0x68)
#define BB_REG_CFR_PRM_3_1              (BB_REG_CFR_BASEADDR+0x6C)
#define BB_REG_CFR_PRM_3_2              (BB_REG_CFR_BASEADDR+0x70)
#define BB_REG_CFR_PRM_3_3              (BB_REG_CFR_BASEADDR+0x74)
#define BB_REG_CFR_PRM_4_0              (BB_REG_CFR_BASEADDR+0x78)
#define BB_REG_CFR_PRM_4_1              (BB_REG_CFR_BASEADDR+0x7C)
#define BB_REG_CFR_PRM_5_0              (BB_REG_CFR_BASEADDR+0x80)
#define BB_REG_CFR_PRM_5_1              (BB_REG_CFR_BASEADDR+0x84)
#define BB_REG_CFR_PRM_6_0              (BB_REG_CFR_BASEADDR+0x88)
#define BB_REG_CFR_PRM_6_1              (BB_REG_CFR_BASEADDR+0x8C)
#define BB_REG_CFR_PRM_6_2              (BB_REG_CFR_BASEADDR+0x90)
#define BB_REG_CFR_PRM_6_3              (BB_REG_CFR_BASEADDR+0x94)
#define BB_REG_CFR_PRM_7_0              (BB_REG_CFR_BASEADDR+0x98)
#define BB_REG_CFR_MSK_DS_00            (BB_REG_CFR_BASEADDR+0x9C)
#define BB_REG_CFR_MSK_DS_01            (BB_REG_CFR_BASEADDR+0xA0)
#define BB_REG_CFR_MSK_DS_02            (BB_REG_CFR_BASEADDR+0xA4)
#define BB_REG_CFR_MSK_DS_03            (BB_REG_CFR_BASEADDR+0xA8)
#define BB_REG_CFR_MSK_DS_10            (BB_REG_CFR_BASEADDR+0xAC)
#define BB_REG_CFR_MSK_DS_11            (BB_REG_CFR_BASEADDR+0xB0)
#define BB_REG_CFR_MSK_DS_12            (BB_REG_CFR_BASEADDR+0xB4)
#define BB_REG_CFR_MSK_DS_13            (BB_REG_CFR_BASEADDR+0xB8)
#define BB_REG_CFR_MSK_DS_20            (BB_REG_CFR_BASEADDR+0xBC)
#define BB_REG_CFR_MSK_DS_21            (BB_REG_CFR_BASEADDR+0xC0)
#define BB_REG_CFR_MSK_DS_22            (BB_REG_CFR_BASEADDR+0xC4)
#define BB_REG_CFR_MSK_DS_23            (BB_REG_CFR_BASEADDR+0xC8)
#define BB_REG_CFR_MSK_DS_30            (BB_REG_CFR_BASEADDR+0xCC)
#define BB_REG_CFR_MSK_DS_31            (BB_REG_CFR_BASEADDR+0xD0)
#define BB_REG_CFR_MSK_DS_32            (BB_REG_CFR_BASEADDR+0xD4)
#define BB_REG_CFR_MSK_DS_33            (BB_REG_CFR_BASEADDR+0xD8)
#define BB_REG_CFR_MSK_DS_40            (BB_REG_CFR_BASEADDR+0xDC)
#define BB_REG_CFR_MSK_DS_41            (BB_REG_CFR_BASEADDR+0xE0)
#define BB_REG_CFR_MSK_DS_42            (BB_REG_CFR_BASEADDR+0xE4)
#define BB_REG_CFR_MSK_DS_43            (BB_REG_CFR_BASEADDR+0xE8)
#define BB_REG_CFR_MSK_DS_50            (BB_REG_CFR_BASEADDR+0xEC)
#define BB_REG_CFR_MSK_DS_51            (BB_REG_CFR_BASEADDR+0xF0)
#define BB_REG_CFR_MSK_DS_52            (BB_REG_CFR_BASEADDR+0xF4)
#define BB_REG_CFR_MSK_DS_53            (BB_REG_CFR_BASEADDR+0xF8)
#define BB_REG_CFR_MSK_DS_60            (BB_REG_CFR_BASEADDR+0xFC)
#define BB_REG_CFR_MSK_DS_61            (BB_REG_CFR_BASEADDR+0x100)
#define BB_REG_CFR_MSK_DS_62            (BB_REG_CFR_BASEADDR+0x104)
#define BB_REG_CFR_MSK_DS_63            (BB_REG_CFR_BASEADDR+0x108)
#define BB_REG_CFR_MSK_DS_70            (BB_REG_CFR_BASEADDR+0x10C)
#define BB_REG_CFR_MSK_DS_71            (BB_REG_CFR_BASEADDR+0x110)
#define BB_REG_CFR_MSK_DS_72            (BB_REG_CFR_BASEADDR+0x114)
#define BB_REG_CFR_MSK_DS_73            (BB_REG_CFR_BASEADDR+0x118)
#define BB_REG_CFR_MSK_PK_00            (BB_REG_CFR_BASEADDR+0x11C)
#define BB_REG_CFR_MSK_PK_01            (BB_REG_CFR_BASEADDR+0x120)
#define BB_REG_CFR_MSK_PK_02            (BB_REG_CFR_BASEADDR+0x124)
#define BB_REG_CFR_MSK_PK_03            (BB_REG_CFR_BASEADDR+0x128)
#define BB_REG_CFR_MSK_PK_04            (BB_REG_CFR_BASEADDR+0x12C)
#define BB_REG_CFR_MSK_PK_05            (BB_REG_CFR_BASEADDR+0x130)
#define BB_REG_CFR_MSK_PK_06            (BB_REG_CFR_BASEADDR+0x134)
#define BB_REG_CFR_MSK_PK_07            (BB_REG_CFR_BASEADDR+0x138)
#define BB_REG_CFR_PK_ENB               (BB_REG_CFR_BASEADDR+0x13C)
#define BB_REG_CFR_PK_THR_0             (BB_REG_CFR_BASEADDR+0x140)
#define BB_REG_CFR_PK_THR_1             (BB_REG_CFR_BASEADDR+0x144)
#define BB_REG_CFR_PK_THR_2             (BB_REG_CFR_BASEADDR+0x148)
#define BB_REG_CFR_PK_THR_3             (BB_REG_CFR_BASEADDR+0x14C)
#define BB_REG_CFR_PK_THR_4             (BB_REG_CFR_BASEADDR+0x150)
#define BB_REG_CFR_PK_THR_5             (BB_REG_CFR_BASEADDR+0x154)
#define BB_REG_CFR_PK_THR_6             (BB_REG_CFR_BASEADDR+0x158)
#define BB_REG_CFR_PK_THR_7             (BB_REG_CFR_BASEADDR+0x15C)
#define BB_REG_CFR_CS_ENA               (BB_REG_CFR_BASEADDR+0x160)
#define BB_REG_CFR_CS_SKP_VEL           (BB_REG_CFR_BASEADDR+0x164)
#define BB_REG_CFR_CS_SKP_RNG           (BB_REG_CFR_BASEADDR+0x168)
#define BB_REG_CFR_CS_SIZ_VEL           (BB_REG_CFR_BASEADDR+0x16C)
#define BB_REG_CFR_CS_SIZ_RNG           (BB_REG_CFR_BASEADDR+0x170)
#define BB_REG_CFR_TYP_NOI              (BB_REG_CFR_BASEADDR+0x174)
#define BB_REG_CFR_NUMB_OBJ             (BB_REG_CFR_BASEADDR+0x178)


/*DOA regisetr*/
#define BB_REG_DOA_DAMB_FRDTRA          (BB_REG_DOA_BASEADDR+0x00)
#define BB_REG_DOA_DAMB_IDX_BGN         (BB_REG_DOA_BASEADDR+0x04)
#define BB_REG_DOA_DAMB_IDX_LEN         (BB_REG_DOA_BASEADDR+0x08)
#define BB_REG_DOA_DAMB_TYP             (BB_REG_DOA_BASEADDR+0x0C)
#define BB_REG_DOA_DAMB_DEFQ            (BB_REG_DOA_BASEADDR+0x10)
#define BB_REG_DOA_DAMB_FDTR            (BB_REG_DOA_BASEADDR+0x14)
#define BB_REG_DOA_DAMB_VELCOM          (BB_REG_DOA_BASEADDR+0x18)
#define BB_REG_DOA_DBPM_ENA             (BB_REG_DOA_BASEADDR+0x1C)
#define BB_REG_DOA_DBPM_ADR             (BB_REG_DOA_BASEADDR+0x20)
#define BB_REG_DOA_MODE_RUN             (BB_REG_DOA_BASEADDR+0x24)
#define BB_REG_DOA_NUMB_OBJ             (BB_REG_DOA_BASEADDR+0x28)
#define BB_REG_DOA_MODE_GRP             (BB_REG_DOA_BASEADDR+0x2C)
#define BB_REG_DOA_NUMB_GRP             (BB_REG_DOA_BASEADDR+0x30)
#define BB_REG_DOA_GRP0_TYP_SCH         (BB_REG_DOA_BASEADDR+0x34)
#define BB_REG_DOA_GRP0_MOD_SCH         (BB_REG_DOA_BASEADDR+0x38)
#define BB_REG_DOA_GRP0_NUM_SCH         (BB_REG_DOA_BASEADDR+0x3C)
#define BB_REG_DOA_GRP0_ADR_COE         (BB_REG_DOA_BASEADDR+0x40)
#define BB_REG_DOA_GRP0_SIZ_ONE_ANG     (BB_REG_DOA_BASEADDR+0x44)
#define BB_REG_DOA_GRP0_SIZ_RNG_PKS_CRS (BB_REG_DOA_BASEADDR+0x48)
#define BB_REG_DOA_GRP0_SIZ_STP_PKS_CRS (BB_REG_DOA_BASEADDR+0x4C)
#define BB_REG_DOA_GRP0_SIZ_RNG_PKS_RFD (BB_REG_DOA_BASEADDR+0x50)
#define BB_REG_DOA_GRP0_SIZ_CMB         (BB_REG_DOA_BASEADDR+0x54)
#define BB_REG_DOA_GRP0_DAT_IDX_0       (BB_REG_DOA_BASEADDR+0x58)
#define BB_REG_DOA_GRP0_DAT_IDX_1       (BB_REG_DOA_BASEADDR+0x5C)
#define BB_REG_DOA_GRP0_DAT_IDX_2       (BB_REG_DOA_BASEADDR+0x60)
#define BB_REG_DOA_GRP0_DAT_IDX_3       (BB_REG_DOA_BASEADDR+0x64)
#define BB_REG_DOA_GRP0_DAT_IDX_4       (BB_REG_DOA_BASEADDR+0x68)
#define BB_REG_DOA_GRP0_DAT_IDX_5       (BB_REG_DOA_BASEADDR+0x6C)
#define BB_REG_DOA_GRP0_DAT_IDX_6       (BB_REG_DOA_BASEADDR+0x70)
#define BB_REG_DOA_GRP0_DAT_IDX_7       (BB_REG_DOA_BASEADDR+0x74)
#define BB_REG_DOA_GRP0_SIZ_WIN         (BB_REG_DOA_BASEADDR+0x78)
#define BB_REG_DOA_GRP0_THR_SNR_0       (BB_REG_DOA_BASEADDR+0x7C)
#define BB_REG_DOA_GRP0_THR_SNR_1       (BB_REG_DOA_BASEADDR+0x80)
#define BB_REG_DOA_GRP0_SCL_POW         (BB_REG_DOA_BASEADDR+0x84)
#define BB_REG_DOA_GRP0_SCL_NOI_0       (BB_REG_DOA_BASEADDR+0x88)
#define BB_REG_DOA_GRP0_SCL_NOI_1       (BB_REG_DOA_BASEADDR+0x8C)
#define BB_REG_DOA_GRP0_SCL_NOI_2       (BB_REG_DOA_BASEADDR+0x90)
#define BB_REG_DOA_GRP0_TST_POW         (BB_REG_DOA_BASEADDR+0x94)
#define BB_REG_DOA_GRP0_SIZ_RNG_AML     (BB_REG_DOA_BASEADDR+0x98)
#define BB_REG_DOA_GRP0_SIZ_STP_AML_CRS (BB_REG_DOA_BASEADDR+0x9C)
#define BB_REG_DOA_GRP0_SIZ_STP_AML_RFD (BB_REG_DOA_BASEADDR+0xA0)
#define BB_REG_DOA_GRP0_SCL_REM_0       (BB_REG_DOA_BASEADDR+0xA4)
#define BB_REG_DOA_GRP0_SCL_REM_1       (BB_REG_DOA_BASEADDR+0xA8)
#define BB_REG_DOA_GRP0_SCL_REM_2       (BB_REG_DOA_BASEADDR+0xAC)
#define BB_REG_DOA_GRP0_ENA_NEI         (BB_REG_DOA_BASEADDR+0xB0)
#define BB_REG_DOA_GRP0_SIZ_SUB         (BB_REG_DOA_BASEADDR+0xB4)
#define BB_REG_DOA_GRP0_TYP_SMO         (BB_REG_DOA_BASEADDR+0xB8)
#define BB_REG_DOA_GRP1_TYP_SCH         (BB_REG_DOA_BASEADDR+0xBC)
#define BB_REG_DOA_GRP1_MOD_SCH         (BB_REG_DOA_BASEADDR+0xC0)
#define BB_REG_DOA_GRP1_NUM_SCH         (BB_REG_DOA_BASEADDR+0xC4)
#define BB_REG_DOA_GRP1_ADR_COE         (BB_REG_DOA_BASEADDR+0xC8)
#define BB_REG_DOA_GRP1_SIZ_ONE_ANG     (BB_REG_DOA_BASEADDR+0xCC)
#define BB_REG_DOA_GRP1_SIZ_RNG_PKS_CRS (BB_REG_DOA_BASEADDR+0xD0)
#define BB_REG_DOA_GRP1_SIZ_STP_PKS_CRS (BB_REG_DOA_BASEADDR+0xD4)
#define BB_REG_DOA_GRP1_SIZ_RNG_PKS_RFD (BB_REG_DOA_BASEADDR+0xD8)
#define BB_REG_DOA_GRP1_SIZ_CMB         (BB_REG_DOA_BASEADDR+0xDC)
#define BB_REG_DOA_GRP1_DAT_IDX_0       (BB_REG_DOA_BASEADDR+0xE0)
#define BB_REG_DOA_GRP1_DAT_IDX_1       (BB_REG_DOA_BASEADDR+0xE4)
#define BB_REG_DOA_GRP1_DAT_IDX_2       (BB_REG_DOA_BASEADDR+0xE8)
#define BB_REG_DOA_GRP1_DAT_IDX_3       (BB_REG_DOA_BASEADDR+0xEC)
#define BB_REG_DOA_GRP1_DAT_IDX_4       (BB_REG_DOA_BASEADDR+0xF0)
#define BB_REG_DOA_GRP1_DAT_IDX_5       (BB_REG_DOA_BASEADDR+0xF4)
#define BB_REG_DOA_GRP1_DAT_IDX_6       (BB_REG_DOA_BASEADDR+0xF8)
#define BB_REG_DOA_GRP1_DAT_IDX_7       (BB_REG_DOA_BASEADDR+0xFC)
#define BB_REG_DOA_GRP1_SIZ_WIN         (BB_REG_DOA_BASEADDR+0x100)
#define BB_REG_DOA_GRP1_THR_SNR_0       (BB_REG_DOA_BASEADDR+0x104)
#define BB_REG_DOA_GRP1_THR_SNR_1       (BB_REG_DOA_BASEADDR+0x108)
#define BB_REG_DOA_GRP1_SCL_POW         (BB_REG_DOA_BASEADDR+0x10C)
#define BB_REG_DOA_GRP1_SCL_NOI_0       (BB_REG_DOA_BASEADDR+0x110)
#define BB_REG_DOA_GRP1_SCL_NOI_1       (BB_REG_DOA_BASEADDR+0x114)
#define BB_REG_DOA_GRP1_SCL_NOI_2       (BB_REG_DOA_BASEADDR+0x118)
#define BB_REG_DOA_GRP1_TST_POW         (BB_REG_DOA_BASEADDR+0x11C)
#define BB_REG_DOA_GRP1_SIZ_RNG_AML     (BB_REG_DOA_BASEADDR+0x120)
#define BB_REG_DOA_GRP1_SIZ_STP_AML_CRS (BB_REG_DOA_BASEADDR+0x124)
#define BB_REG_DOA_GRP1_SIZ_STP_AML_RFD (BB_REG_DOA_BASEADDR+0x128)
#define BB_REG_DOA_GRP1_SCL_REM_0       (BB_REG_DOA_BASEADDR+0x12C)
#define BB_REG_DOA_GRP1_SCL_REM_1       (BB_REG_DOA_BASEADDR+0x130)
#define BB_REG_DOA_GRP1_SCL_REM_2       (BB_REG_DOA_BASEADDR+0x134)
#define BB_REG_DOA_GRP1_ENA_NEI         (BB_REG_DOA_BASEADDR+0x138)
#define BB_REG_DOA_GRP1_SIZ_SUB         (BB_REG_DOA_BASEADDR+0x13C)
#define BB_REG_DOA_GRP1_TYP_SMO         (BB_REG_DOA_BASEADDR+0x140)
#define BB_REG_DOA_GRP2_TYP_SCH         (BB_REG_DOA_BASEADDR+0x144)
#define BB_REG_DOA_GRP2_MOD_SCH         (BB_REG_DOA_BASEADDR+0x148)
#define BB_REG_DOA_GRP2_NUM_SCH         (BB_REG_DOA_BASEADDR+0x14C)
#define BB_REG_DOA_GRP2_ADR_COE         (BB_REG_DOA_BASEADDR+0x150)
#define BB_REG_DOA_GRP2_SIZ_ONE_ANG     (BB_REG_DOA_BASEADDR+0x154)
#define BB_REG_DOA_GRP2_SIZ_RNG_PKS_CRS (BB_REG_DOA_BASEADDR+0x158)
#define BB_REG_DOA_GRP2_SIZ_STP_PKS_CRS (BB_REG_DOA_BASEADDR+0x15C)
#define BB_REG_DOA_GRP2_SIZ_RNG_PKS_RFD (BB_REG_DOA_BASEADDR+0x160)
#define BB_REG_DOA_GRP2_SIZ_CMB         (BB_REG_DOA_BASEADDR+0x164)
#define BB_REG_DOA_GRP2_DAT_IDX_0       (BB_REG_DOA_BASEADDR+0x168)
#define BB_REG_DOA_GRP2_DAT_IDX_1       (BB_REG_DOA_BASEADDR+0x16C)
#define BB_REG_DOA_GRP2_DAT_IDX_2       (BB_REG_DOA_BASEADDR+0x170)
#define BB_REG_DOA_GRP2_DAT_IDX_3       (BB_REG_DOA_BASEADDR+0x174)
#define BB_REG_DOA_GRP2_DAT_IDX_4       (BB_REG_DOA_BASEADDR+0x178)
#define BB_REG_DOA_GRP2_DAT_IDX_5       (BB_REG_DOA_BASEADDR+0x17C)
#define BB_REG_DOA_GRP2_DAT_IDX_6       (BB_REG_DOA_BASEADDR+0x180)
#define BB_REG_DOA_GRP2_DAT_IDX_7       (BB_REG_DOA_BASEADDR+0x184)
#define BB_REG_DOA_GRP2_SIZ_WIN         (BB_REG_DOA_BASEADDR+0x188)
#define BB_REG_DOA_GRP2_THR_SNR_0       (BB_REG_DOA_BASEADDR+0x18C)
#define BB_REG_DOA_GRP2_THR_SNR_1       (BB_REG_DOA_BASEADDR+0x190)
#define BB_REG_DOA_GRP2_SCL_POW         (BB_REG_DOA_BASEADDR+0x194)
#define BB_REG_DOA_GRP2_SCL_NOI_0       (BB_REG_DOA_BASEADDR+0x198)
#define BB_REG_DOA_GRP2_SCL_NOI_1       (BB_REG_DOA_BASEADDR+0x19C)
#define BB_REG_DOA_GRP2_SCL_NOI_2       (BB_REG_DOA_BASEADDR+0x1A0)
#define BB_REG_DOA_GRP2_TST_POW         (BB_REG_DOA_BASEADDR+0x1A4)
#define BB_REG_DOA_GRP2_SIZ_RNG_AML     (BB_REG_DOA_BASEADDR+0x1A8)
#define BB_REG_DOA_GRP2_SIZ_STP_AML_CRS (BB_REG_DOA_BASEADDR+0x1AC)
#define BB_REG_DOA_GRP2_SIZ_STP_AML_RFD (BB_REG_DOA_BASEADDR+0x1B0)
#define BB_REG_DOA_GRP2_SCL_REM_0       (BB_REG_DOA_BASEADDR+0x1B4)
#define BB_REG_DOA_GRP2_SCL_REM_1       (BB_REG_DOA_BASEADDR+0x1B8)
#define BB_REG_DOA_GRP2_SCL_REM_2       (BB_REG_DOA_BASEADDR+0x1BC)
#define BB_REG_DOA_GRP2_ENA_NEI         (BB_REG_DOA_BASEADDR+0x1C0)
#define BB_REG_DOA_GRP2_SIZ_SUB         (BB_REG_DOA_BASEADDR+0x1C4)
#define BB_REG_DOA_GRP2_TYP_SMO         (BB_REG_DOA_BASEADDR+0x1C8)
#define BB_REG_DOA_C2D1_ADR_COE         (BB_REG_DOA_BASEADDR+0x1CC)
#define BB_REG_DOA_C2D1_SIZ_CMB         (BB_REG_DOA_BASEADDR+0x1D0)
#define BB_REG_DOA_C2D2_ADR_COE         (BB_REG_DOA_BASEADDR+0x1D4)
#define BB_REG_DOA_C2D2_SIZ_CMB         (BB_REG_DOA_BASEADDR+0x1D8)
#define BB_REG_DOA_C2D3_ADR_COE         (BB_REG_DOA_BASEADDR+0x1DC)
#define BB_REG_DOA_C2D3_SIZ_CMB         (BB_REG_DOA_BASEADDR+0x1E0)


/*Debug data dump register*/
#define BB_REG_DBG_BUF_TAR              (BB_REG_DBG_BASEADDR+0x00)
#define BB_REG_DBG_MAP_RLT              (BB_REG_DBG_BASEADDR+0x04)
#define BB_REG_DBG_RFRSH_START          (BB_REG_DBG_BASEADDR+0x08)
#define BB_REG_DBG_RFRSH_CLR            (BB_REG_DBG_BASEADDR+0x0C)
#define BB_REG_DBG_RFRSH_STATUS         (BB_REG_DBG_BASEADDR+0x10)


/*DML register*/
#define BB_REG_DML_GRP0_SV_STP          (BB_REG_DML_BASEADDR+0x00)
#define BB_REG_DML_GRP0_SV_START        (BB_REG_DML_BASEADDR+0x04)
#define BB_REG_DML_GRP0_SV_END          (BB_REG_DML_BASEADDR+0x08)
#define BB_REG_DML_GRP0_DC_COE_0        (BB_REG_DML_BASEADDR+0x0C)
#define BB_REG_DML_GRP0_DC_COE_1        (BB_REG_DML_BASEADDR+0x10)
#define BB_REG_DML_GRP0_DC_COE_2        (BB_REG_DML_BASEADDR+0x14)
#define BB_REG_DML_GRP0_DC_COE_3        (BB_REG_DML_BASEADDR+0x18)
#define BB_REG_DML_GRP0_DC_COE_4        (BB_REG_DML_BASEADDR+0x1C)
#define BB_REG_DML_GRP0_EXTRA_EN        (BB_REG_DML_BASEADDR+0x20)
#define BB_REG_DML_GRP0_DC_COE_2_EN     (BB_REG_DML_BASEADDR+0x24)
#define BB_REG_DML_GRP1_SV_STP          (BB_REG_DML_BASEADDR+0x28)
#define BB_REG_DML_GRP1_SV_START        (BB_REG_DML_BASEADDR+0x2C)
#define BB_REG_DML_GRP1_SV_END          (BB_REG_DML_BASEADDR+0x30)
#define BB_REG_DML_GRP1_DC_COE_0        (BB_REG_DML_BASEADDR+0x34)
#define BB_REG_DML_GRP1_DC_COE_1        (BB_REG_DML_BASEADDR+0x38)
#define BB_REG_DML_GRP1_DC_COE_2        (BB_REG_DML_BASEADDR+0x3C)
#define BB_REG_DML_GRP1_DC_COE_3        (BB_REG_DML_BASEADDR+0x40)
#define BB_REG_DML_GRP1_DC_COE_4        (BB_REG_DML_BASEADDR+0x44)
#define BB_REG_DML_GRP1_EXTRA_EN        (BB_REG_DML_BASEADDR+0x48)
#define BB_REG_DML_GRP1_DC_COE_2_EN     (BB_REG_DML_BASEADDR+0x4C)
#define BB_REG_DML_MEM_BASE_ADR         (BB_REG_DML_BASEADDR+0x50)


/*--- VALUE, MASK OR SHIFT -----------*/
#define SYS_BNK_MODE_SINGLE 0
#define SYS_BNK_MODE_RESERVED 1
#define SYS_BNK_MODE_ROTATE 2

/* 10 memory */
#define SYS_MEM_ACT_DML 9  /* DML d,k list */
#define SYS_MEM_ACT_ANC 8  /* DE-AMBILITY Parameters */
#define SYS_MEM_ACT_RLT 7
#define SYS_MEM_ACT_MAC 6
#define SYS_MEM_ACT_COE 5
#define SYS_MEM_ACT_BUF 4
#define SYS_MEM_ACT_NVE 3
#define SYS_MEM_ACT_WIN 2
#define SYS_MEM_ACT_SAM 1
#define SYS_MEM_ACT_COD 0

/* 9 enable */
#define SYS_ENABLE_AGC_MASK      0x1
#define SYS_ENABLE_HIL_MASK      0x1
#define SYS_ENABLE_SAM_MASK      0x1
#define SYS_ENABLE_DMP_MID_MASK  0x1
#define SYS_ENABLE_FFT_1D_MASK   0x1
#define SYS_ENABLE_FFT_2D_MASK   0x1
#define SYS_ENABLE_CFR_MASK      0x1
#define SYS_ENABLE_BFM_MASK      0x1
#define SYS_ENABLE_DMP_FNL_MASK  0x1

#define SYS_ENABLE_AGC_SHIFT     0
#define SYS_ENABLE_HIL_SHIFT     1
#define SYS_ENABLE_SAM_SHIFT     2
#define SYS_ENABLE_DMP_MID_SHIFT 3
#define SYS_ENABLE_FFT_1D_SHIFT  4
#define SYS_ENABLE_FFT_2D_SHIFT  5
#define SYS_ENABLE_CFR_SHIFT     6
#define SYS_ENABLE_BFM_SHIFT     7
#define SYS_ENABLE_DMP_FNL_SHIFT 8

#define SAM_DBG_SRC_BF 0 /* before down sampling */
#define SAM_DBG_SRC_AF 1 /* after down sampling */

#define SAM_SINKER_BUF 0
#define SAM_SINKER_FFT 1
#define SAM_FORCE_ENABLE  1
#define SAM_FORCE_DISABLE 0

#define CFR_TYPE_CMB_SISO 0
#define CFR_TYPE_CMB_MIMO 1
#define CFR_TYPE_CMB_DBPM 2
#define CFR_TYPE_CMB_MUSIC 3
#define CFR_TYPE_DEC_CA 0
#define CFR_TYPE_DEC_OS 1
#define CFR_TYPE_DEC_CA_AND_OS 2
#define CFR_TYPE_DEC_CA_OR_OS 3

#define DOA_MODE_NORMAL 0
#define DOA_MODE_CASCADE 1
#define DOA_TYPE_DBF 0
#define DOA_TYPE_RESERVED 1
#define DOA_TYPE_DML 2

#define DBG_MAP_RLT_BOPS 0
#define DBG_MAP_RLT_BPSO 1
#define DBG_BUF_TAR_NONE 0
#define DBG_BUF_TAR_ADC (1<<0)
#define DBG_BUF_TAR_FFT_1D (1<<1)
#define DBG_BUF_TAR_FFT_2D (1<<2)
#define DBG_BUF_TAR_CFR (1<<3)
#define DBG_BUF_TAR_BFM (1<<4)
#define DBG_BUF_TAR_CPU (1<<5)

#define ECC_MODE_OFF  0
#define ECC_MODE_SB   1
#define ECC_MODE_DB   2 /* DB err should occur in cfr_buf & top_mac */

#define ADC_MODE_SAM   0
#define ADC_MODE_HIL   1 /* HIL is only supported under "DIRECT" FFT_MODE */


#define BB_MEM_BASEADDR                 0xE00000
#define BB_MEM_BASEADDR_CH_OFFSET       0x040000 /* TODO: check the usage in bb_dc_command in baseband_cli.c*/

#define MEM_SIZE_COD_WD 6
#define MEM_DATA_COE_WD 28

#define SYS_SIZE_ANT 4
#define SYS_DATA_ADC_WD 13
#define SYS_DATA_WIN_WD 16
#define SYS_DATA_FFT_WD 26
#define SYS_DATA_COM_WD 32
#define SYS_DATA_SIG_WD 20
#define AGC_DATA_COD_WD 9
#define AGC_DATA_GAI_WD 8
#define SAM_DATA_COE_WD 8
#define SAM_DATA_SCL_WD 10
#define FFT_DATA_AMB_WD 4
#define FFT_DATA_COE_WD 14
#define CFR_SIZE_MIM_WD 4

#define ANT_NUM 4
#define SYS_SIZE_RNG_WD 11  /* defined in RTL */
#define SYS_SIZE_VEL_WD 10  /* defined in RTL */
/* name sync with ALPS_A */
#define SYS_BUF_STORE_ADC SAM_SINKER_BUF
#define SYS_BUF_STORE_FFT SAM_SINKER_FFT

#define BB_IRQ_ENABLE_ALL                        0x7
#define BB_IRQ_CLEAR_ALL                         0x7
#define BB_IRQ_ENABLE_SAM_DONE                   0x4
#define BB_IRQ_CLEAR_SAM_DONE                    0x4
#define BB_IRQ_ENABLE_BB_DONE                    0x1
#define BB_ECC_ERR_CLEAR_ALL                     0x7ff

#define SYS_SIZE_OBJ_WD                          10    /* maximal object number is 1024 for all bank */
#define SYS_SIZE_OBJ_WD_BNK                      8     /* maximal object number is 256 for each bank */
#define SYS_SIZE_OBJ_BNK                         256   /* maximal object number is 256 for each bank */
#define RESULT_SIZE                              128   /* 128 bytes */

#define DMP_FFT_1D             1 /* fft1d data dump */
#define DMP_FFT_2D             2 /* fft2d data dump */

/* frame interleaving loop pattern */
#define FIXED                0   /* fixed */
#define ROTATE               1   /* loop */
#define AIR_CONDITIONER      2   /* special scene of air conditioner */
#define VELAMB               3   /* special scene of velocity ambiguity*/

#define CFG_0                0   /* frame interleaving config 0 */
#define CFG_1                1   /* frame interleaving config 1 */
#define CFG_2                2   /* frame interleaving config 2 */
#define CFG_3                3   /* frame interleaving config 3 */

/*--- TYPEDEF ( to match newest RTL )-------------------------*/
typedef struct {
        uint32_t ang_acc_0   : 4  ; /* 0x00 */
        uint32_t ang_idx_0   : 9  ;
        uint32_t ang_vld_0   : 1  ;
        uint32_t ang_dummy_0 : 18 ;
        uint32_t sig_0       : 20 ; /* 0x04 */
        uint32_t sig_dummy_0 : 12 ;

        uint32_t ang_acc_1   : 4  ; /* 0x08 */
        uint32_t ang_idx_1   : 9  ;
        uint32_t ang_vld_1   : 1  ;
        uint32_t ang_dummy_1 : 18 ;
        uint32_t sig_1       : 20 ; /* 0x0c */
        uint32_t sig_dummy_1 : 12 ;

        uint32_t ang_acc_2   : 4  ; /* 0x10 */
        uint32_t ang_idx_2   : 9  ;
        uint32_t ang_vld_2   : 1  ;
        uint32_t ang_dummy_2 : 18 ;
        uint32_t sig_2       : 20 ; /* 0x14 */
        uint32_t sig_dummy_2 : 12 ;

        uint32_t ang_acc_3   : 4  ; /* 0x18 */
        uint32_t ang_idx_3   : 9  ;
        uint32_t ang_vld_3   : 1  ;
        uint32_t ang_dummy_3 : 18 ;
        uint32_t sig_3       : 20 ; /* 0x1c */
        uint32_t sig_dummy_3 : 12 ;
} doa_info_t;


typedef struct {
        uint32_t vel_acc      : 4  ; /* 0x00 */
        uint32_t vel_idx      : 10 ;
        uint32_t rng_acc      : 4  ;
        uint32_t rng_idx      : 10 ;
        uint32_t dummy_0      : 4  ;

        uint32_t noi          : 20 ; /* 0x04 */
        uint32_t amb_idx      : 5  ;
        uint32_t dummy_1      : 7  ;

        doa_info_t doa[3]          ;
        uint32_t dummy[6]          ;
} obj_info_t; /* 32 words(4 bytes) in all */

typedef struct {
        uint32_t vel_acc      : 4  ; /* 0x00 */
        uint32_t vel_idx      : 10 ;
        uint32_t rng_acc      : 4  ;
        uint32_t rng_idx      : 10 ;
        uint32_t dummy_0      : 4  ;

        uint32_t noi          : 20 ; /* 0x04 */
        uint32_t amb_idx      : 5  ;
        uint32_t dummy_1      : 7  ;

} cfr_info_t;

#define NOI_AMB_REODER          5
#define NOI_AMB_OFFSET          1

#endif /* BASEBAND_ALPS_FM_REG_H_ */
