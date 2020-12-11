# Application name
APPL ?= sensor

# root dir of calterah
CALTERAH_ROOT ?= ../../

# Selected OS
OS_SEL ?= freertos

USE_BOARD_MAIN ?= 0

# root dir of embARC
EMBARC_ROOT ?= ../../../embarc_osp


CUR_CORE ?= arcem6

MID_SEL ?= common

BD_VER ?= 1

CHIP_CASCADE ?=

# application source dirs
APPL_CSRC_DIR += $(APPLS_ROOT)/$(APPL)
APPL_ASMSRC_DIR += $(APPLS_ROOT)/$(APPL)

ifeq ($(TOOLCHAIN), gnu)
	APPL_LIBS += -lm # math support
else

endif

SYSTEM_BOOT_STAGE ?= 2

FLASH_TYPE ?= s25fls

EXT_DEV_LIST ?= nor_flash

# To enable XIP function, "LOAD_XIP_TEXT_EN" in options/option.mk also need to be set to 1
FLASH_XIP ?= 0

UART_OTA ?= 1
SYSTEM_WATCHDOG ?=
SPIS_SERVER_ON ?=

ifeq ($(FLASH_XIP), 1)
	FLASH_STATIC_PARAM ?= 1
else
	FLASH_STATIC_PARAM ?=
endif

#if PLL clock hasn't been opened before firmware, please set to a 1.
#it will indicate whether open PLL clock. and if PLL clock has been
#opened before, please set to a 0.
PLL_CLOCK_OPEN ?= 0

# application include dirs
APPL_INC_DIR += $(APPLS_ROOT)/$(APPL)

# include current project makefile
COMMON_COMPILE_PREREQUISITES += $(APPLS_ROOT)/$(APPL)/$(APPL).mk

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
override CALTERAH_ROOT := $(strip $(subst \,/,$(CALTERAH_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
EXTRA_DEFINES += -DACC_BB_BOOTUP=$(ACC_BB_BOOTUP)