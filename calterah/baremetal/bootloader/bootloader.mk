# Application name
APPL ?= bootloader

# root dir of calterah
CALTERAH_ROOT ?= ../../

USE_BOARD_MAIN ?= 0

# root dir of embARC
EMBARC_ROOT ?= ../../../embarc_osp

BAREMETAL_PROJECT ?= 1

CUR_CORE ?= arcem6

MID_SEL ?= common

BD_VER ?= 1

EXT_DEV_LIST ?= flash
FLASH_XIP ?= 0

IO_STREAM_SKIP ?= 1

PLL_CLOCK_OPEN ?= 0

# application source dirs
APPL_CSRC_DIR += $(APPLS_ROOT)/$(APPL)
APPL_ASMSRC_DIR += $(APPLS_ROOT)/$(APPL)

ifeq ($(TOOLCHAIN), gnu)
	APPL_LIBS += -lm # math support
else
endif

# application include dirs
APPL_INC_DIR += $(APPLS_ROOT)/$(APPL)

APPL_DEFINES += -DSKIP_BOARD_MAIN

# include current project makefile
COMMON_COMPILE_PREREQUISITES += $(APPLS_ROOT)/$(APPL)/$(APPL).mk

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
override CALTERAH_ROOT := $(strip $(subst \,/,$(CALTERAH_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
