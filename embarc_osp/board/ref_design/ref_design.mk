REF_DESIGN_ROOT = $(BOARDS_ROOT)/ref_design

SUPPORTED_BOARD_VERS = 1

## Select Chip Version
BD_VER ?= 1

override BD_VER := $(strip $(BD_VER))

## Set Valid Board
VALID_BD_VER = $(call check_item_exist, $(BD_VER), $(SUPPORTED_BOARD_VERS))

## Try Check CHIP is valid
ifeq ($(VALID_BD_VER), )
$(info BOARD_VER - $(SUPPORTED_BOARD_VERS) are supported)
$(error BOARD_VER $(BD_VER) is not supported, please check it!)
endif

# link file
ifeq ($(VALID_TOOLCHAIN), mw)
LINKER_SCRIPT_FILE ?= $(REF_DESIGN_ROOT)/linker_template_mw.ld
else
LINKER_SCRIPT_FILE ?= $(REF_DESIGN_ROOT)/linker_template_gnu.ld
endif

#alps.cfg is for RDP board
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/alps.cfg
#snps_em_sk_v2.2.cfg is for validation board using GNU
#OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk_v2.2.cfg

# UART setting
USE_UART0 ?= 1
CHIP_CONSOLE_UART_BAUD ?= 3000000
#UART0_IO_POS ?= 0
USE_UART1 ?= 0
#UART1_IO_POS ?= 1

#SPI setting
USE_SPI0 ?= 0
USE_SPI1 ?= 0
USE_SPI2 ?= 0

#qspi setting
USE_QSPI ?= 1

#CAN setting
USE_CAN0 ?= 0
USE_CAN1 ?= 0
USE_CAN_FD ?= 0

# CAN TX work mode
#    0: using polling mode
#    1: using interrupt mode
CAN_TX_INT_MODE ?= 0

ifeq ($(IO_STREAM_SKIP),)
#IO STREAM setting
USE_IO_STREAM ?= 0
USE_IO_STREAM_PRINTF ?= 0
else
USE_IO_STREAM ?= 0
USE_IO_STREAM_PRINTF ?= 0
endif

USE_SW_TRACE ?= 1
SW_TRACE_BASE ?= 0xa00000
SW_TRACE_LEN ?= 0x8000

#AHB DMA
USE_AHB_DMA ?= 1
BOARD_DEFINES += -DUSE_DW_AHB_DMA=$(USE_AHB_DMA)

BOARD_DEFINES += -DUSE_DW_UART_0=$(USE_UART0) -DUSE_DW_UART_1=$(USE_UART1) -DCHIP_CONSOLE_UART_BAUD=$(CHIP_CONSOLE_UART_BAUD)

BOARD_DEFINES += -DUART0_IO_POS=$(UART0_IO_POS) -DUART1_IO_POS=$(UART1_IO_POS)

BOARD_DEFINES += -DUSE_DW_SPI_0=$(USE_SPI0) -DUSE_DW_SPI_1=$(USE_SPI1) -DUSE_DW_SPI_2=$(USE_SPI2)

BOARD_DEFINES += -DUSE_DW_QSPI=$(USE_QSPI) -DFLASH_XIP_EN=$(FLASH_XIP)

BOARD_DEFINES += -DUSE_CAN_0=$(USE_CAN0) -DUSE_CAN_1=$(USE_CAN1) -DUSE_CAN_FD=$(USE_CAN_FD)

BOARD_DEFINES += -DUSE_IO_STREAM=$(USE_IO_STREAM) -DIO_STREAM_PRINTF=$(USE_IO_STREAM_PRINTF)

BOARD_DEFINES += -DCAN_TX_INT_MODE=$(CAN_TX_INT_MODE)

#BOARD_DEFINES += -DUSE_XIP=$(USE_XIP)
ifneq ($(CHIP_CASCADE),)
BOARD_DEFINES += -DCHIP_CASCADE
endif

BOARD_DEFINES += -DPLL_CLOCK_OPEN_EN=$(PLL_CLOCK_OPEN)

BOARD_DEFINES += -DCONSOLE_PRINTF

BOARD_DEFINES += -DSYSTEM_$(SYSTEM_BOOT_STAGE)_STAGES_BOOT
BOARD_DEFINES += -D__FIRMWARE__

ifneq ($(USE_SW_TRACE),)
BOARD_DEFINES += -DUSE_SW_TRACE -DSW_TRACE_BASE=$(SW_TRACE_BASE) -DSW_TRACE_LEN=$(SW_TRACE_LEN)
endif

ifneq ($(UART_OTA),)
BOARD_DEFINES += -DSYSTEM_UART_OTA
endif

ifneq ($(SYSTEM_WATCHDOG),)
BOARD_DEFINES += -DSYSTEM_WATCHDOG
endif

ifneq ($(SPIS_SERVER_ON),)
BOARD_DEFINES += -DSPIS_SERVER
endif
ifneq ($(FLASH_STATIC_PARAM), )
BOARD_DEFINES += -DSTATIC_EXT_FLASH_PARAM
endif
