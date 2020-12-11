#include "embARC.h"
#include "system.h"
#include "ota.h"
#include "flash_header.h"
#include "flash_mmap.h"
#include "instruction.h"
#include "flash.h"
#include "xip_hal.h"
#include "vendor.h"
#include "dw_ssi_reg.h"
#include "dw_ssi.h"
#include "config.h"


#define BOOT_CRC_ENABLE		(1)

#define FLASH_ADDR		(0xb90000)
#define REG_SSI_CTRLR0		(0x0)
#define REG_SSI_CTRLR1		(0x4)
#define REG_SSI_SSIENR 		(0x8)
#define REG_SSI_SER		(0x10)
#define REG_SSI_SR 		(0x28)

#define REG_SSI_DR(x)		(0x60 + ((x) << 0))

#define REG_SSI_SPI_CTRLR0 	(0xF4)

#define BITS_SPI_FRF_MASK	(0x3)
#define BITS_SPI_FRF_SHIFT	(21)
#define BITS_TMOD_MASK		(0x3)
#define BITS_TMOD_SHIFT		(8)

#define	BITS_INST_L_MASK	(0x3)
#define	BITS_INST_L_SHIFT	(8)
#define	BITS_ADDR_L_MASK	(0xF)
#define	BITS_ADDR_L_SHIFT	(2)

#define BITS_RFNE 		(1 << 3)
#define BITS_TFE 		(1 << 2)
#define BITS_TFNF 		(1 << 1)
#define BITS_BUSY 		(1 << 0)

#define CMD_READ		(0x03)


extern uint32_t _e_part2_text;
extern uint32_t _f_part2_text;

void arc_cache_init(void)
{
	_arc_aux_write(AUX_CACHE_LIMIT, 0x800000);

	/* enable IC & DC. */
	_arc_aux_write(0x48, 0x60);
	_arc_aux_write(0x47, 1);
	while(_arc_aux_read(0x48) & 0x100);

	_arc_aux_write(0x11, 0);
	_arc_aux_write(0x10, 0);
	Asm("nop_s");
	Asm("nop_s");
	Asm("nop_s");
}

void chip_raw_mdelay(uint32_t ms)
{
	uint64_t s_tick, e_tick;

	s_tick = _arc_aux_read(AUX_RTC_HIGH);
	s_tick = (s_tick << 32) | _arc_aux_read(AUX_RTC_LOW);
	e_tick = s_tick + ms * (300000);
	if (e_tick > s_tick) {
		do {
			s_tick = _arc_aux_read(AUX_RTC_HIGH);
			s_tick = (s_tick << 32) | _arc_aux_read(AUX_RTC_LOW);
		} while (e_tick > s_tick);
	}
}

static void flash_read_early(uint32_t addr, uint8_t *buf, uint32_t count)
{
	int i;
	uint32_t val;

	uint32_t read_count = 256;
	uint32_t read_time = count / read_count;
	if (count % read_count) {
		read_time++;
	}

	while (count) {
		if (count < read_count) {
			read_count = count;
		}

		raw_writel((REG_SSI_SSIENR + FLASH_ADDR), 0);
		val = raw_readl((REG_SSI_CTRLR0 + FLASH_ADDR));
		val &= ~(BITS_TMOD_MASK << BITS_TMOD_SHIFT);
		val &= ~(BITS_SPI_FRF_MASK << BITS_SPI_FRF_SHIFT);
		val |= (BITS_TMOD_MASK & 3) << BITS_TMOD_SHIFT;
		raw_writel((REG_SSI_CTRLR0 + FLASH_ADDR), val);

		raw_writel((REG_SSI_CTRLR1 + FLASH_ADDR), (read_count-1));
		raw_writel((REG_SSI_SSIENR + FLASH_ADDR), 1);


		while (raw_readl((REG_SSI_SR + FLASH_ADDR)) & BITS_RFNE) {
			raw_readl((REG_SSI_DR(0) + FLASH_ADDR));
		}

		/* wait TXF empty. and then send command and address. */
		while (0 == (raw_readl((REG_SSI_SR + FLASH_ADDR)) & BITS_TFE));
		raw_writel((REG_SSI_DR(0) + FLASH_ADDR), CMD_READ); /* 03h. */
		raw_writel((REG_SSI_DR(0) + FLASH_ADDR), (addr >> 16) & 0xFF);
		raw_writel((REG_SSI_DR(0) + FLASH_ADDR), (addr >> 8) & 0xFF);
		raw_writel((REG_SSI_DR(0) + FLASH_ADDR), (addr & 0xFF));
		raw_writel((REG_SSI_SER + FLASH_ADDR), 1);
		for (i = 0; i < read_count; i++) {
			while (0 == (raw_readl((REG_SSI_SR + FLASH_ADDR)) & BITS_RFNE));
			*buf++ = (uint8_t)(raw_readl((REG_SSI_DR(0) + FLASH_ADDR)));
		}
		do {
			val = raw_readl((REG_SSI_SR + FLASH_ADDR));
		} while ((0 == (val & BITS_TFE)) || (val & BITS_BUSY));
		raw_writel((REG_SSI_SER + FLASH_ADDR), 0);

		count -= read_count;
		addr += read_count;
	}
}

static void flash_command_send(dev_cmd_t *command)
{
	//int ret = 0;

	int j, i, cnt = 0;
	uint32_t val;
	uint8_t address_length = 0;
	uint8_t ins_length = 2;

	uint32_t command_buf[8];

	i = 0;
	command_buf[i++] = command->cmd;
	if ((command->addr >> 24)) {
		address_length = 6;
		ins_length = 2;
		command_buf[i++] = (command->addr >> 16) & 0xFFU;
		command_buf[i++] = (command->addr >> 8) & 0xFFU;
		command_buf[i++] = command->addr & 0xFFU;
	}
	
	for (j = 0; j < 4; j++) {
		if ((command[cnt].value[j] >> 8)) {
			command_buf[i++] = command->value[j] & 0xFFU;
			ins_length = 2;
		} else {
			break;
		}
	}

	raw_writel((REG_SSI_SSIENR + FLASH_ADDR), 0);
	val = raw_readl((REG_SSI_CTRLR0 + FLASH_ADDR));
	val &= ~(BITS_TMOD_MASK << BITS_TMOD_SHIFT);
	val |= (BITS_TMOD_MASK & 1) << BITS_TMOD_SHIFT;
	raw_writel((REG_SSI_CTRLR0 + FLASH_ADDR), val);

	val = raw_readl((REG_SSI_SPI_CTRLR0 + FLASH_ADDR));
	val &= ~(BITS_ADDR_L_MASK << BITS_ADDR_L_SHIFT);
	val |= (BITS_ADDR_L_MASK & address_length) << BITS_ADDR_L_SHIFT;

	val &= ~(BITS_INST_L_MASK << BITS_INST_L_SHIFT);
	val |= (BITS_INST_L_MASK & ins_length) << BITS_INST_L_SHIFT;
	raw_writel((REG_SSI_SPI_CTRLR0 + FLASH_ADDR), val);
	raw_writel((REG_SSI_SSIENR + FLASH_ADDR), 1);

	cnt = 0;
	while (0 == (raw_readl((REG_SSI_SR + FLASH_ADDR)) & BITS_TFE));
	while (i--) {
		raw_writel((REG_SSI_DR(0) + FLASH_ADDR), command_buf[cnt++]);
	}
	raw_writel((REG_SSI_SER + FLASH_ADDR), 1);
	do {
		val = raw_readl((REG_SSI_SR + FLASH_ADDR));
	} while ((0 == (val & BITS_TFE)) || (val & BITS_BUSY));
	raw_writel((REG_SSI_SER + FLASH_ADDR), 0);
	chip_raw_mdelay(1);
}

static void flash_quad_entry_early(void)
{
	dev_cmd_t command;
	if (FLASH_DEV_CMD0_INS) {
		command.cmd = FLASH_DEV_CMD0_INS;
		command.addr = FLASH_DEV_CMD0_ADDR;
		command.value[0] = FLASH_DEV_CMD0_VAL0;
		command.value[1] = FLASH_DEV_CMD0_VAL1;
		command.value[2] = FLASH_DEV_CMD0_VAL2;
		command.value[3] = FLASH_DEV_CMD0_VAL3;
		flash_command_send(&command);
	}
	if (FLASH_DEV_CMD1_INS) {
		command.cmd = FLASH_DEV_CMD1_INS;
		command.addr = FLASH_DEV_CMD1_ADDR;
		command.value[0] = FLASH_DEV_CMD1_VAL0;
		command.value[1] = FLASH_DEV_CMD1_VAL1;
		command.value[2] = FLASH_DEV_CMD1_VAL2;
		command.value[3] = FLASH_DEV_CMD1_VAL3;
		flash_command_send(&command);
	}
}

#include "xip_early.c"
static void flash_xip_boot(void)
{
#if BOOT_CRC_ENABLE
	uint8_t *image_ptr = NULL;
	uint32_t *crc32_ptr = NULL;
	uint32_t crc32 = 0;
	uint32_t single_crc_size = 0;
#endif
	uint32_t payload_size = 0;

	image_header_t *header_ptr = (image_header_t *)(FLASH_MMAP_FIRMWARE_BASE + FLASH_FIRMWARE_BASE);

	next_image_entry firmware_entry;

	flash_quad_entry_early();
	/* configure XIP controller: */
	flash_xip_init_early();

	/* firmware memory mapping address */
	image_ptr = (uint8_t *)(FLASH_MMAP_FIRMWARE_BASE + header_ptr->payload_addr);

	firmware_entry = (next_image_entry)(image_ptr + header_ptr->exec_offset);

#if BOOT_CRC_ENABLE
	crc32_ptr = (uint32_t *)(image_ptr + header_ptr->payload_size);

	payload_size = header_ptr->payload_size;
	single_crc_size = header_ptr->crc_part_size;
	while (payload_size) {
		if (payload_size < single_crc_size) {
			single_crc_size = payload_size;
		}

		crc32 = update_crc(0, image_ptr, single_crc_size);
		//crc32_update(0, image_ptr, single_crc_size);
		//crc32 = crc_output();
		if (crc32 != *crc32_ptr) {
			break;
		}
		crc32_ptr++;
		image_ptr += single_crc_size;
		payload_size -= single_crc_size;
	}
#endif
	if (0 == payload_size) {
		_arc_aux_write(0x4B, 1);
		while (_arc_aux_read(0x48) & 0x100);
		icache_invalidate();
		/* jump to the next image. */
		firmware_entry();
	}
}

#define REG_FLASH_AES_DIN_OFFSET(x, y)	(((0x12 + ((x) << 2) + (y)) << 2))
#define REG_FLASH_AES_DOUT_OFFSET(x, y) (((0x26 + ((x) << 2) + (y)) << 2))
int aes_decrypt(uint32_t *data_in, uint32_t *data_out, uint32_t len)
{
    int ret = 0;

    unsigned int i, j;

    if (len % 64) {
        ret = -1;
    } else {
        while (len) {
            for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                raw_writel(REG_FLASH_AES_DIN_OFFSET(i, j) + 0xd00100, *data_in++);
            }
            }

            raw_writel(0xd00188, 1);
            raw_writel(0xd0018c, 1);
            //raw_writel(REG_FLASH_AES_DIN_VAL_OFFSET +XIP_ADDR, 1);

            while (0 == (raw_readl(0xd00194) & 0x1));
            for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                *data_out++ = raw_readl(REG_FLASH_AES_DOUT_OFFSET(i, j) + 0xd00100);
            }
            }
            len -= 64;
        }
    }

    return ret;
}


static void part2_read(void)
{
	uint32_t crc_len;
	uint32_t part2_base;

	flash_header_t header;
	flash_read_early(0, (uint8_t *)&header, 32);

	uint32_t read_count = header.pload_size;
	if (header.pload_size % 0x4) {
		read_count += 4 - (header.pload_size % 0x4);
	}

	crc_len = header.pload_size / 0x1000;
	if (header.pload_size % 0x1000) {
		crc_len += 1;
	}
	crc_len <<= 2;
	read_count += crc_len;
	if (raw_readl(0xb00004) & 1) {
		if (read_count % 64) {
			read_count += 64 - read_count % 64;
		}
	}

	part2_base = header.pload_addr + read_count;
    read_count = (uint32_t)(&_e_part2_text) - (uint32_t)(&_f_part2_text);

	if (raw_readl(0xb00004) & 1) {
		if (read_count % 0x4) {
			read_count += 4 - (read_count % 0x4);
		}
		crc_len = read_count / 0x1000;
		if (read_count % 0x1000) {
			crc_len += 1;
		}
		crc_len <<= 2;
		read_count += crc_len;
		if (read_count % 64) {
			read_count += 64 - read_count % 64;
		}
	}
	flash_read_early(part2_base, (uint8_t *)&_f_part2_text, read_count);

	if (raw_readl(0xb00004) & 1) {
		aes_decrypt((uint32_t *)&_f_part2_text, (uint32_t *)&_f_part2_text, read_count);
	}

	_arc_aux_write(0x4B, 1);
	while (_arc_aux_read(0x48) & 0x100);
}



void board_main(void)
{
	uint32_t boot_mode = reboot_cause();


	gen_crc_table();

	if (0 == boot_mode) {
		flash_xip_boot();
	} else {
		
		part2_read();

		system_ota_entry();
	}
}
