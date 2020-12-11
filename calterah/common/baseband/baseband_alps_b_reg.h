#ifndef BASEBAND_ALPS_B_REG_H
#define BASEBAND_ALPS_B_REG_H

/*--- ADDRESS ------------------------*/
#define BB_REG_BASEADDR                 0xC00000

#define BB_REG_SYS_BASEADDR             (BB_REG_BASEADDR+0x000)
#define BB_REG_AGC_BASEADDR             (BB_REG_BASEADDR+0x200)
#define BB_REG_SAM_BASEADDR             (BB_REG_BASEADDR+0x400)
#define BB_REG_FFT_BASEADDR             (BB_REG_BASEADDR+0x600)
#define BB_REG_CFR_BASEADDR             (BB_REG_BASEADDR+0x800)
#define BB_REG_BFM_BASEADDR             (BB_REG_BASEADDR+0xa00)
#define BB_REG_DBG_BASEADDR             (BB_REG_BASEADDR+0xc00)

/*sys register*/
#define BB_REG_SYS_START                (BB_REG_SYS_BASEADDR+0x00)
#define BB_REG_SYS_BNK_MODE             (BB_REG_SYS_BASEADDR+0x04)
#define BB_REG_SYS_BNK_ACT              (BB_REG_SYS_BASEADDR+0x08)
#define BB_REG_SYS_BNK_QUE              (BB_REG_SYS_BASEADDR+0x0C)
#define BB_REG_SYS_BNK_RST              (BB_REG_SYS_BASEADDR+0x10)
#define BB_REG_SYS_MEM_ACT              (BB_REG_SYS_BASEADDR+0x14)
#define BB_REG_SYS_ENABLE               (BB_REG_SYS_BASEADDR+0x18)
#define BB_REG_SYS_SIZE_RNG_PRD         (BB_REG_SYS_BASEADDR+0x1C)
#define BB_REG_SYS_SIZE_RNG_SKP         (BB_REG_SYS_BASEADDR+0x20)
#define BB_REG_SYS_SIZE_RNG_BUF         (BB_REG_SYS_BASEADDR+0x24)
#define BB_REG_SYS_SIZE_RNG_FFT         (BB_REG_SYS_BASEADDR+0x28)
#define BB_REG_SYS_SIZE_BPM             (BB_REG_SYS_BASEADDR+0x2C)
#define BB_REG_SYS_SIZE_VEL_BUF         (BB_REG_SYS_BASEADDR+0x30)
#define BB_REG_SYS_SIZE_VEL_FFT         (BB_REG_SYS_BASEADDR+0x34)
#define BB_REG_SYS_IRQ_ENA              (BB_REG_SYS_BASEADDR+0x38)
#define BB_REG_SYS_IRQ_CLR              (BB_REG_SYS_BASEADDR+0x3C)
#define BB_REG_SYS_ECC_ENA              (BB_REG_SYS_BASEADDR+0x40)
#define BB_REG_SYS_ECC_SB_CLR           (BB_REG_SYS_BASEADDR+0x44)
#define BB_REG_SYS_ECC_DB_CLR           (BB_REG_SYS_BASEADDR+0x48)
#define BB_REG_SYS_STATUS               (BB_REG_SYS_BASEADDR+0x4C)
#define BB_REG_FDB_SYS_BNK_ACT          (BB_REG_SYS_BASEADDR+0x50)
#define BB_REG_FDB_SYS_IRQ_STATUS       (BB_REG_SYS_BASEADDR+0x54)
#define BB_REG_FDB_SYS_ECC_SB_STATUS    (BB_REG_SYS_BASEADDR+0x58)
#define BB_REG_FDB_SYS_ECC_DB_STATUS    (BB_REG_SYS_BASEADDR+0x5C)

/*AGC regesiter*/
#define BB_REG_AGC_SAT_THR_TIA          (BB_REG_AGC_BASEADDR+0x00)
#define BB_REG_AGC_SAT_THR_VGA1         (BB_REG_AGC_BASEADDR+0x04)
#define BB_REG_AGC_SAT_THR_VGA2         (BB_REG_AGC_BASEADDR+0x08)
#define BB_REG_AGC_DAT_MAX_SEL          (BB_REG_AGC_BASEADDR+0x0C)

#define BB_REG_AGC_CODE_LNA_0           (BB_REG_AGC_BASEADDR+0x10)
#define BB_REG_AGC_CODE_LNA_1           (BB_REG_AGC_BASEADDR+0x14)

#define BB_REG_AGC_CODE_TIA_0           (BB_REG_AGC_BASEADDR+0x18)
#define BB_REG_AGC_CODE_TIA_1           (BB_REG_AGC_BASEADDR+0x1C)
#define BB_REG_AGC_CODE_TIA_2           (BB_REG_AGC_BASEADDR+0x20)
#define BB_REG_AGC_CODE_TIA_3           (BB_REG_AGC_BASEADDR+0x24)

#define BB_REG_AGC_GAIN_MIN             (BB_REG_AGC_BASEADDR+0x28)
#define BB_REG_AGC_CDGN_INIT            (BB_REG_AGC_BASEADDR+0x2C)
#define BB_REG_AGC_CDGN_C0_0            (BB_REG_AGC_BASEADDR+0x30)
#define BB_REG_AGC_CDGN_C0_1            (BB_REG_AGC_BASEADDR+0x34)
#define BB_REG_AGC_CDGN_C0_2            (BB_REG_AGC_BASEADDR+0x38)
#define BB_REG_AGC_CDGN_C1_0            (BB_REG_AGC_BASEADDR+0x3C)
#define BB_REG_AGC_CDGN_C1_1            (BB_REG_AGC_BASEADDR+0x40)
#define BB_REG_AGC_CDGN_C1_2            (BB_REG_AGC_BASEADDR+0x44)
#define BB_REG_AGC_CDGN_C1_3            (BB_REG_AGC_BASEADDR+0x48)
#define BB_REG_AGC_CDGN_C1_4            (BB_REG_AGC_BASEADDR+0x4C)
#define BB_REG_AGC_CDGN_C1_5            (BB_REG_AGC_BASEADDR+0x50)
#define BB_REG_AGC_CDGN_C1_6            (BB_REG_AGC_BASEADDR+0x54)
#define BB_REG_AGC_CDGN_C1_7            (BB_REG_AGC_BASEADDR+0x58)
#define BB_REG_AGC_CDGN_C1_8            (BB_REG_AGC_BASEADDR+0x5C)

#define BB_REG_AGC_FORCE                (BB_REG_AGC_BASEADDR+0x60)
#define BB_REG_AGC_COD_C0               (BB_REG_AGC_BASEADDR+0x64)
#define BB_REG_AGC_COD_C1               (BB_REG_AGC_BASEADDR+0x68)
#define BB_REG_AGC_COD_C2               (BB_REG_AGC_BASEADDR+0x6C)
#define BB_REG_AGC_COD_C3               (BB_REG_AGC_BASEADDR+0x70)
#define BB_REG_AGC_SAT_CNT_TIA_C0       (BB_REG_AGC_BASEADDR+0x74)
#define BB_REG_AGC_SAT_CNT_TIA_C1       (BB_REG_AGC_BASEADDR+0x78)
#define BB_REG_AGC_SAT_CNT_TIA_C2       (BB_REG_AGC_BASEADDR+0x7C)
#define BB_REG_AGC_SAT_CNT_TIA_C3       (BB_REG_AGC_BASEADDR+0x80)
#define BB_REG_AGC_SAT_CNT_VGA1_C0      (BB_REG_AGC_BASEADDR+0x84)
#define BB_REG_AGC_SAT_CNT_VGA1_C1      (BB_REG_AGC_BASEADDR+0x88)
#define BB_REG_AGC_SAT_CNT_VGA1_C2      (BB_REG_AGC_BASEADDR+0x8C)
#define BB_REG_AGC_SAT_CNT_VGA1_C3      (BB_REG_AGC_BASEADDR+0x90)
#define BB_REG_AGC_SAT_CNT_VGA2_C0      (BB_REG_AGC_BASEADDR+0x94)
#define BB_REG_AGC_SAT_CNT_VGA2_C1      (BB_REG_AGC_BASEADDR+0x98)
#define BB_REG_AGC_SAT_CNT_VGA2_C2      (BB_REG_AGC_BASEADDR+0x9C)
#define BB_REG_AGC_SAT_CNT_VGA2_C3      (BB_REG_AGC_BASEADDR+0xA0)
#define BB_REG_AGC_DAT_MAX_1ST_C0       (BB_REG_AGC_BASEADDR+0xA4)
#define BB_REG_AGC_DAT_MAX_1ST_C1       (BB_REG_AGC_BASEADDR+0xA8)
#define BB_REG_AGC_DAT_MAX_1ST_C2       (BB_REG_AGC_BASEADDR+0xAC)
#define BB_REG_AGC_DAT_MAX_1ST_C3       (BB_REG_AGC_BASEADDR+0xB0)
#define BB_REG_AGC_DAT_MAX_2ND_C0       (BB_REG_AGC_BASEADDR+0xB4)
#define BB_REG_AGC_DAT_MAX_2ND_C1       (BB_REG_AGC_BASEADDR+0xB8)
#define BB_REG_AGC_DAT_MAX_2ND_C2       (BB_REG_AGC_BASEADDR+0xBC)
#define BB_REG_AGC_DAT_MAX_2ND_C3       (BB_REG_AGC_BASEADDR+0xC0)
#define BB_REG_AGC_DAT_MAX_3RD_C0       (BB_REG_AGC_BASEADDR+0xC4)
#define BB_REG_AGC_DAT_MAX_3RD_C1       (BB_REG_AGC_BASEADDR+0xC8)
#define BB_REG_AGC_DAT_MAX_3RD_C2       (BB_REG_AGC_BASEADDR+0xCC)
#define BB_REG_AGC_DAT_MAX_3RD_C3       (BB_REG_AGC_BASEADDR+0xD0)


/*SAM register*/
#define BB_REG_SAM_SINKER               (BB_REG_SAM_BASEADDR+0x00)
#define BB_REG_SAM_OFFSET               (BB_REG_SAM_BASEADDR+0x04)
#define BB_REG_SAM_FILT_CNT             (BB_REG_SAM_BASEADDR+0x08)
#define BB_REG_SAM_F_0_S1               (BB_REG_SAM_BASEADDR+0x0C)
#define BB_REG_SAM_F_0_B1               (BB_REG_SAM_BASEADDR+0x10)
#define BB_REG_SAM_F_0_A1               (BB_REG_SAM_BASEADDR+0x14)
#define BB_REG_SAM_F_0_A2               (BB_REG_SAM_BASEADDR+0x18)
#define BB_REG_SAM_F_1_S1               (BB_REG_SAM_BASEADDR+0x1C)
#define BB_REG_SAM_F_1_B1               (BB_REG_SAM_BASEADDR+0x20)
#define BB_REG_SAM_F_1_A1               (BB_REG_SAM_BASEADDR+0x24)
#define BB_REG_SAM_F_1_A2               (BB_REG_SAM_BASEADDR+0x28)
#define BB_REG_SAM_F_2_S1               (BB_REG_SAM_BASEADDR+0x2C)
#define BB_REG_SAM_F_2_B1               (BB_REG_SAM_BASEADDR+0x30)
#define BB_REG_SAM_F_2_A1               (BB_REG_SAM_BASEADDR+0x34)
#define BB_REG_SAM_F_2_A2               (BB_REG_SAM_BASEADDR+0x38)
#define BB_REG_SAM_F_3_S1               (BB_REG_SAM_BASEADDR+0x3C)
#define BB_REG_SAM_F_3_B1               (BB_REG_SAM_BASEADDR+0x40)
#define BB_REG_SAM_F_3_A1               (BB_REG_SAM_BASEADDR+0x44)
#define BB_REG_SAM_F_3_A2               (BB_REG_SAM_BASEADDR+0x48)
#define BB_REG_SAM_FNL_SHF              (BB_REG_SAM_BASEADDR+0x4C)
#define BB_REG_SAM_FNL_SCL              (BB_REG_SAM_BASEADDR+0x50)
#define BB_REG_SAM_DE_INT_ENA           (BB_REG_SAM_BASEADDR+0x54)
#define BB_REG_SAM_DE_INT_DAT           (BB_REG_SAM_BASEADDR+0x58)
#define BB_REG_SAM_DE_INT_MSK           (BB_REG_SAM_BASEADDR+0x5C)
#define BB_REG_SAM_FORCE                (BB_REG_SAM_BASEADDR+0x60)
/* for debug data in SAM module*/
#define BB_REG_SAM_DBG_SRC              (BB_REG_SAM_BASEADDR+0x64)
/* before down sampling or after down sampling */
#define BB_REG_SAM_DBG_SRC_BF 0
#define BB_REG_SAM_DBG_SRC_AF 1
#define BB_REG_SAM_SIZE_DBG_BGN         (BB_REG_SAM_BASEADDR+0x68)
#define BB_REG_SAM_SIZE_DBG_END         (BB_REG_SAM_BASEADDR+0x6C)
/*selection bit for data format of ADC data*/
#define BB_REG_SAM_FRMT_ADC             (BB_REG_SAM_BASEADDR+0x70)
/*4 valid data in every 4 samples or 4 valid data in every 5 samples*/
#define BB_REG_SAM_FRMT_ADC_4IN4 0
#define BB_REG_SAM_FRMT_ADC_4IN5 1

/*FFT register*/
#define BB_REG_FFT_SHFT_RNG             (BB_REG_FFT_BASEADDR+0x00)
#define BB_REG_FFT_SHFT_VEL             (BB_REG_FFT_BASEADDR+0x04)
#define BB_REG_FFT_DAMB_ENA             (BB_REG_FFT_BASEADDR+0x08)
#define BB_REG_FFT_DRCT_CPN             (BB_REG_FFT_BASEADDR+0x0C)
#define BB_REG_FFT_NO_WIN               (BB_REG_FFT_BASEADDR+0x10)

/* CFAR register*/ 
/*upper bound for object number*/
#define BB_REG_CFR_SIZE_OBJ             (BB_REG_CFR_BASEADDR+0x00)
#define BB_REG_CFR_BACK_RNG             (BB_REG_CFR_BASEADDR+0x04)
#define BB_REG_CFR_TYPE_CMB             (BB_REG_CFR_BASEADDR+0x08)

#define CFR_TYPE_CMB_SISO 0
#define CFR_TYPE_CMB_MIMO 1
#define CFR_TYPE_CMB_DBPM 2
#define CFR_TYPE_CMB_MUSIC 3
#define BB_REG_CFR_MIMO_NUM             (BB_REG_CFR_BASEADDR+0x0C)
/*offset address of MIMO coefficients*/
#define BB_REG_CFR_MIMO_ADR             (BB_REG_CFR_BASEADDR+0x10)
#define BB_REG_CFR_TYPE_DEC             (BB_REG_CFR_BASEADDR+0x14)

#define CFR_TYPE_DEC_CA 0
#define CFR_TYPE_DEC_OS 1
#define CFR_TYPE_DEC_CA_AND_OS 2
#define CFR_TYPE_DEC_CA_OR_OS 3

#define BB_REG_CFR_CA_MASK_0              (BB_REG_CFR_BASEADDR+0x18)
#define BB_REG_CFR_CA_MASK_1              (BB_REG_CFR_BASEADDR+0x1C)
#define BB_REG_CFR_CA_MASK_2              (BB_REG_CFR_BASEADDR+0x20)
#define BB_REG_CFR_CA_MASK_3              (BB_REG_CFR_BASEADDR+0x24)
#define BB_REG_CFR_CA_MASK_4              (BB_REG_CFR_BASEADDR+0x28)
#define BB_REG_CFR_CA_MASK_5              (BB_REG_CFR_BASEADDR+0x2C)
#define BB_REG_CFR_CA_MASK_6              (BB_REG_CFR_BASEADDR+0x30)
#define BB_REG_CFR_CA_DATA_SCL            (BB_REG_CFR_BASEADDR+0x34)
#define BB_REG_CFR_OS_MASK_0              (BB_REG_CFR_BASEADDR+0x38)
#define BB_REG_CFR_OS_MASK_1              (BB_REG_CFR_BASEADDR+0x3C)
#define BB_REG_CFR_OS_MASK_2              (BB_REG_CFR_BASEADDR+0x40)
#define BB_REG_CFR_OS_MASK_3              (BB_REG_CFR_BASEADDR+0x44)
#define BB_REG_CFR_OS_MASK_4              (BB_REG_CFR_BASEADDR+0x48)
#define BB_REG_CFR_OS_MASK_5              (BB_REG_CFR_BASEADDR+0x4C)
#define BB_REG_CFR_OS_MASK_6              (BB_REG_CFR_BASEADDR+0x50)
#define BB_REG_CFR_OS_DATA_SCL            (BB_REG_CFR_BASEADDR+0x54)
#define BB_REG_CFR_OS_DATA_THR            (BB_REG_CFR_BASEADDR+0x58)
#define BB_REG_CFR_PK_MASK_0              (BB_REG_CFR_BASEADDR+0x5C)
#define BB_REG_CFR_PK_MASK_1              (BB_REG_CFR_BASEADDR+0x60)
#define BB_REG_CFR_PK_MASK_2              (BB_REG_CFR_BASEADDR+0x64)
#define BB_REG_CFR_PK_MASK_3              (BB_REG_CFR_BASEADDR+0x68)
#define BB_REG_CFR_PK_MASK_4              (BB_REG_CFR_BASEADDR+0x6C)
#define BB_REG_CFR_PK_MASK_5              (BB_REG_CFR_BASEADDR+0x70)
#define BB_REG_CFR_PK_MASK_6              (BB_REG_CFR_BASEADDR+0x74)
#define BB_REG_CFR_PK_DATA_THR            (BB_REG_CFR_BASEADDR+0x78)
/*detected obj num*/
#define BB_REG_CFR_NUMB_OBJ               (BB_REG_CFR_BASEADDR+0x7C)

/*DOA regisetr*/
/* normal mode or cascade mode*/
#define BB_REG_BFM_MODE                   (BB_REG_BFM_BASEADDR+0x00)
#define BB_REG_BFM_MODE_NORMAL 0
#define BB_REG_BFM_MODE_CASCADE 1
/*merged object number*/
#define BB_REG_BFM_NUMB_OBJ_CAS           (BB_REG_BFM_BASEADDR+0x04)
/* BFM method*/
#define BB_REG_BFM_TYPE                   (BB_REG_BFM_BASEADDR+0x08)
#define BB_REG_BFM_TYPE_NORMAL 0
#define BB_REG_BFM_TYPE_MUSIC 1
/*music related*/
#define BB_REG_BFM_M_TYP_SMO              (BB_REG_BFM_BASEADDR+0x0C)
#define BB_REG_BFM_M_TYP_SMO_FORW 0
#define BB_REG_BFM_M_TYP_SMO_FRBK 1
#define BB_REG_BFM_M_NEI_ENA              (BB_REG_BFM_BASEADDR+0x10)
/*size of sub-array is (x + 1)*/
#define BB_REG_BFM_M_SIZ_SUB              (BB_REG_BFM_BASEADDR+0x14)

/*enable bit for de-BPM*/
#define BB_REG_BFM_DBPM_ENA               (BB_REG_BFM_BASEADDR+0x18)
/*offset address of de-BPM coefficients*/
#define BB_REG_BFM_DBPM_ADR               (BB_REG_BFM_BASEADDR+0x1C)
/*number of directions to be searched for each BFM input*/
#define BB_REG_BFM_NUMB_GRP               (BB_REG_BFM_BASEADDR+0x20)
/*obj num share in a bin*/
#define BB_REG_BFM_NUMB_SCH               (BB_REG_BFM_BASEADDR+0x24)
/*BFM and 2d BFM related*/
/* group 0 related*/
/*offset address of BFM/MUSIC coefficients of 0th direction*/
#define BB_REG_BFM_GRP_0_ADDR_COE         (BB_REG_BFM_BASEADDR+0x28)
/*offset address of BFM spectrum of 0th direction */
#define BB_REG_BFM_GRP_0_ADDR_SHP         (BB_REG_BFM_BASEADDR+0x2C)
#define BB_REG_BFM_GRP_0_SIZE_ANG         (BB_REG_BFM_BASEADDR+0x30)
/*number of antennas to be combined in 0th direction*/
#define BB_REG_BFM_GRP_0_SIZE_CMB         (BB_REG_BFM_BASEADDR+0x34) 
/* rx idx stored for bfm,IDX_x store most 4 rx idx*/
#define BB_REG_BFM_GRP_0_DATA_IDX_0       (BB_REG_BFM_BASEADDR+0x38)
#define BB_REG_BFM_GRP_0_DATA_IDX_1       (BB_REG_BFM_BASEADDR+0x3C)
#define BB_REG_BFM_GRP_0_DATA_IDX_2       (BB_REG_BFM_BASEADDR+0x40)
#define BB_REG_BFM_GRP_0_DATA_IDX_3       (BB_REG_BFM_BASEADDR+0x44)
#define BB_REG_BFM_GRP_0_DATA_IDX_4       (BB_REG_BFM_BASEADDR+0x48)
#define BB_REG_BFM_GRP_0_DATA_IDX_5       (BB_REG_BFM_BASEADDR+0x4C)
#define BB_REG_BFM_GRP_0_DATA_IDX_6       (BB_REG_BFM_BASEADDR+0x50)
#define BB_REG_BFM_GRP_0_DATA_IDX_7       (BB_REG_BFM_BASEADDR+0x54)
#define BB_REG_BFM_GRP_0_SCLR_SIG         (BB_REG_BFM_BASEADDR+0x58)
#define BB_REG_BFM_GRP_0_SCLR_NOI         (BB_REG_BFM_BASEADDR+0x5C)

#define BB_REG_BFM_GRP_1_ADDR_COE         (BB_REG_BFM_BASEADDR+0x60)
#define BB_REG_BFM_GRP_1_ADDR_SHP         (BB_REG_BFM_BASEADDR+0x64)
#define BB_REG_BFM_GRP_1_SIZE_ANG         (BB_REG_BFM_BASEADDR+0x68)
#define BB_REG_BFM_GRP_1_SIZE_CMB         (BB_REG_BFM_BASEADDR+0x6C)
#define BB_REG_BFM_GRP_1_DATA_IDX_0        (BB_REG_BFM_BASEADDR+0x70)
#define BB_REG_BFM_GRP_1_DATA_IDX_1        (BB_REG_BFM_BASEADDR+0x74)
#define BB_REG_BFM_GRP_1_DATA_IDX_2        (BB_REG_BFM_BASEADDR+0x78)
#define BB_REG_BFM_GRP_1_DATA_IDX_3        (BB_REG_BFM_BASEADDR+0x7C)
#define BB_REG_BFM_GRP_1_DATA_IDX_4       (BB_REG_BFM_BASEADDR+0x80)
#define BB_REG_BFM_GRP_1_DATA_IDX_5        (BB_REG_BFM_BASEADDR+0x84)
#define BB_REG_BFM_GRP_1_DATA_IDX_6        (BB_REG_BFM_BASEADDR+0x88)
#define BB_REG_BFM_GRP_1_DATA_IDX_7        (BB_REG_BFM_BASEADDR+0x8C)
#define BB_REG_BFM_GRP_1_SCLR_SIG          (BB_REG_BFM_BASEADDR+0x90)
#define BB_REG_BFM_GRP_1_SCLR_NOI          (BB_REG_BFM_BASEADDR+0x94)

#define BB_REG_BFM_GRP_2_ADDR_COE          (BB_REG_BFM_BASEADDR+0x98)
#define BB_REG_BFM_GRP_2_ADDR_SHP             (BB_REG_BFM_BASEADDR+0x9C)
#define BB_REG_BFM_GRP_2_SIZE_ANG             (BB_REG_BFM_BASEADDR+0xA0)
#define BB_REG_BFM_GRP_2_SIZE_CMB             (BB_REG_BFM_BASEADDR+0xA4)
#define BB_REG_BFM_GRP_2_DATA_IDX_0           (BB_REG_BFM_BASEADDR+0xA8)
#define BB_REG_BFM_GRP_2_DATA_IDX_1           (BB_REG_BFM_BASEADDR+0xAC)
#define BB_REG_BFM_GRP_2_DATA_IDX_2           (BB_REG_BFM_BASEADDR+0xB0)
#define BB_REG_BFM_GRP_2_DATA_IDX_3           (BB_REG_BFM_BASEADDR+0xB4)
#define BB_REG_BFM_GRP_2_DATA_IDX_4           (BB_REG_BFM_BASEADDR+0xB8)
#define BB_REG_BFM_GRP_2_DATA_IDX_5           (BB_REG_BFM_BASEADDR+0xBC)
#define BB_REG_BFM_GRP_2_DATA_IDX_6           (BB_REG_BFM_BASEADDR+0xC0)
#define BB_REG_BFM_GRP_2_DATA_IDX_7           (BB_REG_BFM_BASEADDR+0xC4)
#define BB_REG_BFM_GRP_2_SCLR_SIG             (BB_REG_BFM_BASEADDR+0xC8)
#define BB_REG_BFM_GRP_2_SCLR_NOI             (BB_REG_BFM_BASEADDR+0xCC)
/*dump data register*/
#define BB_REG_DBG_TARGET                     (BB_REG_DBG_BASEADDR+0x00)

#define DBG_TARGET_ADC (1<<0)
#define DBG_TARGET_FFT_1D (1<<1)
#define DBG_TARGET_FFT_2D (1<<2)
#define DBG_TARGET_CFR (1<<3)
#define DBG_TARGET_BFM (1<<4)

#define BB_REG_DBG_MAP_BUF                    (BB_REG_DBG_BASEADDR+0x04)

#define DBG_MAP_BUF_VBAR 0
#define DBG_MAP_BUF_MBUF 1

#define BB_REG_DBG_MAP_RLT                    (BB_REG_DBG_BASEADDR+0x08)
#define DBG_MAP_RLT_WD 1
#define DBG_MAP_RLT_BOPS 0
#define DBG_MAP_RLT_BPSO 1

#define BB_REG_DBG_DAC_ENA                    (BB_REG_DBG_BASEADDR+0x0c)
#define BB_REG_DBG_DAC_MUL                    (BB_REG_DBG_BASEADDR+0x10)
#define BB_REG_DBG_DAC_DIV                    (BB_REG_DBG_BASEADDR+0x14)

/*--- VALUE, MASK OR SHIFT -----------*/
#define BB_REG_SYS_BNK_MODE_SINGLE 0
#define BB_REG_SYS_BNK_MODE_MULTI 1
#define BB_REG_SYS_BNK_MODE_ROTATE 2

#define BB_REG_SYS_MEM_ACT_COD 0   
#define BB_REG_SYS_MEM_ACT_WIN 1
#define BB_REG_SYS_MEM_ACT_BUF 2
#define BB_REG_SYS_MEM_ACT_COE 3
#define BB_REG_SYS_MEM_ACT_MAC 4
#define BB_REG_SYS_MEM_ACT_RLT 5
#define BB_REG_SYS_MEM_ACT_SHP 6

/* 9 enable */
#define BB_REG_SYS_ENABLE_AGC_MASK      0x1
#define BB_REG_SYS_ENABLE_HIL_MASK      0x1
#define BB_REG_SYS_ENABLE_SAM_MASK      0x1
#define BB_REG_SYS_ENABLE_DMP_MID_MASK  0x1
#define BB_REG_SYS_ENABLE_FFT_1D_MASK   0x1
#define BB_REG_SYS_ENABLE_FFT_2D_MASK   0x1
#define BB_REG_SYS_ENABLE_CFR_MASK      0x1
#define BB_REG_SYS_ENABLE_BFM_MASK      0x1
#define BB_REG_SYS_ENABLE_DMP_FNL_MASK  0x1

#define BB_REG_SYS_ENABLE_AGC_SHIFT     0
#define BB_REG_SYS_ENABLE_HIL_SHIFT     1
#define BB_REG_SYS_ENABLE_SAM_SHIFT     2
#define BB_REG_SYS_ENABLE_DMP_MID_SHIFT 3
#define BB_REG_SYS_ENABLE_FFT_1D_SHIFT  4
#define BB_REG_SYS_ENABLE_FFT_2D_SHIFT  5
#define BB_REG_SYS_ENABLE_CFR_SHIFT     6
#define BB_REG_SYS_ENABLE_BFM_SHIFT     7
#define BB_REG_SYS_ENABLE_DMP_FNL_SHIFT 8

#define BB_REG_ECC_MODE_OFF  0
#define BB_REG_ECC_MODE_SB   1
#define BB_REG_ECC_MODE_DB   2 /* DB err should occur in cfr_buf & top_mac */

#define BB_REG_ADC_MODE_SAM   0
#define BB_REG_ADC_MODE_HIL   1 /* HIL is only supported under "DIRECT" FFT_MODE */


#define BB_MEM_BASEADDR                 0x800000
#define BB_MEM_BASEADDR_CH_OFFSET       0x040000 /* TODO: check the usage in bb_dc_command in baseband_cli.c*/

#define BB_REG_MEM_SIZE_COD_WD 6
#define BB_REG_MEM_DATA_COE_WD 28

#define BB_REG_SYS_SIZE_ANT 4
#define BB_REG_SYS_DATA_ADC_WD 13
#define BB_REG_SYS_DATA_WIN_WD 16
#define BB_REG_SYS_DATA_FFT_WD 26
#define BB_REG_SYS_DATA_COM_WD 32
#define BB_REG_SYS_DATA_SIG_WD 20
#define BB_REG_AGC_DATA_COD_WD 9
#define BB_REG_AGC_DATA_GAI_WD 8
#define BB_REG_SAM_DATA_COE_WD 8
#define BB_REG_SAM_DATA_SCL_WD 10
#define BB_REG_FFT_DATA_AMB_WD 4
#define BB_REG_FFT_DATA_COE_WD 14
#define BB_REG_CFR_SIZE_MIM_WD 4

#define SAM_SINKER_BUF 0
#define SAM_SINKER_FFT 1
#define SAM_FORCE_ENABLE  1
#define SAM_FORCE_DISABLE 0
#define ANT_NUM 4
#define SYS_SIZE_RNG_WD 11 /* defined in RTL */
#define SYS_SIZE_VEL_WD 9  /* defined in RTL */
/* name sync with ALPS_A */
#define SYS_BUF_STORE_ADC SAM_SINKER_BUF
#define SYS_BUF_STORE_FFT SAM_SINKER_FFT

#define BB_IRQ_ENABLE_ALL                        0x7
#define BB_IRQ_CLEAR_ALL                         0x7
#define BB_IRQ_ENABLE_SAM_DONE                   0x4
#define BB_IRQ_CLEAR_SAM_DONE                    0x4
#define BB_ECC_ERR_CLEAR_ALL                     0x7ff

#define SYS_SIZE_OBJ_WD                          8    /* maximal object number is 256 */
#define RESULT_SIZE                              128   /* 128 bytes */
/*--- TYPEDEF -------------------------*/
typedef struct {
     uint32_t ang_0    : 16;    /* 0x00 */
     uint32_t ang_1    : 9;    
	 uint32_t is_obj_1 : 7;    
	 uint32_t ang_2    : 9;    /* 0x04 */
	 uint32_t is_obj_2 :  7;
	 uint32_t ang_3    : 9;
	 uint32_t is_obj_3 :  7;
	 uint32_t sig_0        ;    /* 0x08 */
	 uint32_t sig_1        ;    /* 0x0C */
     uint32_t sig_2        ;    /* 0x10 */
     uint32_t sig_3        ;    /* 0x14 */	 
} doa_info_t;


typedef struct {
        uint32_t vel      : 16;    /* 0x00 */
        uint32_t rng      : 16;
        uint32_t noi      : 32;    /* 0x04 */
        doa_info_t doa[3];
		uint32_t dummy[12];
} obj_info_t;

#endif
