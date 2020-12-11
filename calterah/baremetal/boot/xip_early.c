void flash_xip_init_early(void)
{
	xip_enable(1);

	raw_writel(0xd00000, CLK_MODE0_SSI_CTRL0(QUAD_FRAME_FORMAT, DW_SSI_DATA_LEN_32, DW_SSI_RECEIVE_ONLY));
	raw_writel(0xd00004, 0x7ff);
	raw_writel(0xd00014, 0xa);
	raw_writel(0xd000f0, FLASH_DEV_SAMPLE_DELAY);
	raw_writel(0xd000f4, SSI_SPI_CTRLR0(FLASH_DEV_DUMMY_CYCLE + FLASH_DEV_MODE_CYCLE,\
				DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_24, DW_SSI_1_X_X));

	raw_writel(0xd00010, 1);

	raw_writel(0xd00018, 1);
	raw_writel(0xd0001C, 0);
	raw_writel(0xd0002C, 0x7f);

    /* XIP Read Command Register - XRDCR */
	raw_writel(0xd00108, FLASH_COMMAND(CMD_Q_READ, DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_24, DW_SSI_DATA_LEN_32));
    /* XIP Instruction Section Offset Register - XISOR */
	raw_writel(0xd0011c, CONFIG_XIP_INS_OFFSET);
    /* XIP AHB Bus Endian Control Register - XABECR */
	raw_writel(0xd001d8, 0x03);
    /* XIP AES Data Bus Endian Control Register - XADBECR */
	raw_writel(0xd001dc, 0x00);
    /* AES Mode Register - AMR */
	raw_writel(0xd00124, 0x07);

    /* XIP Instruction Buffer Control Register - XIBCR */
	raw_writel(0xd001e0, CONFIG_XIP_INS_BUF_LEN);
    /* XIP Data Section Offset Register - XDSOR */
	raw_writel(0xd00120, CONFIG_XIP_DATA_OFFSET);
    /* XIP Read Buffer Control Register - XRBCR */
	raw_writel(0xd001e4, CONFIG_XIP_RD_BUF_LEN);

	raw_writel(0xd00008, 1);
    /* XIP Enable Register - XER */
	raw_writel(0xd00100, 1);
	//chip_hw_udelay(1000);
	while (0 == raw_readl(0xd00100));
}
