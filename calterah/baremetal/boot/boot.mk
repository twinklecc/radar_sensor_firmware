CUR_CORE ?= arcem6

BD_VER ?= 1

# application source dirs
APPL_CSRC_DIR += $(APPLS_ROOT)/$(APPL)
APPL_ASMSRC_DIR += $(APPLS_ROOT)/$(APPL)

ifeq ($(TOOLCHAIN), gnu)
	APPL_LIBS += -lm # math support
endif

# application include dirs
APPL_INC_DIR += $(APPLS_ROOT)/$(APPL)

APPL_DEFINES ?=

# include current project makefile
COMMON_COMPILE_PREREQUISITES += $(APPLS_ROOT)/$(APPL)/$(APPL).mk

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
override CALTERAH_ROOT := $(strip $(subst \,/,$(CALTERAH_ROOT)))


#include $(EMBARC_ROOT)/options/options.mk


###########################################
##
# Special Default Make Goal to all
# so if no make goal specified,
# all will be the make goal,
# not the first target in the makefile
##
.DEFAULT_GOAL = all

##
# Output OBJS Root Directory
##
OUT_DIR_ROOT ?=

##
# Compile Toolchain
# Refer to toolchain.mk
##
TOOLCHAIN ?= gnu

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
OLEVEL ?= O2

##
# STACK SIZE Set
##
STACKSZ ?= 4096

##
# Debugger Select
# Refer to debug.mk
##
JTAG ?= usb

##
# Digilent JTAG Name Specify(Only for Metaware)
# This is especially useful if you have more than one
# Digilent device connected to your host.
# You can open digilent adept tool to see what digilent
# jtag is connected, leave this blank if don't know the name of digilent jtag
# I have see two digilent name: JtagHs1  TE0604-02
# Simple wrapper of -prop=dig_device=name option of Metaware Debugger(mdb)
##
DIG_NAME ?=

##
# Digilent JTAG Choice Select(Only for Metaware)
# Simple wrapper of -prop=dig_device_choice=N option of Metaware Debugger(mdb)
##
DIG_CHOICE ?=

##
# Set Digilent JTAG frequency(in Hz)(Only for Metaware)
# This is especially useful when you want to specify
# the digilent JTAG frequency when your board freq is quite low.
# Simple wrapper of -prop=dig_speed=SSS option of Metaware Debugger(mdb)
##
DIG_SPEED ?=

##
# DEBUG
# 1 for enable
# other for disable
##
DEBUG ?= 1

##
# generate map
# 1 for enable
# other for disable
##
MAP ?= 1

##
# Control Compiler Message Show
# 1: show compile total options
# 0: just show compile file info
##
V ?= 0

##
# Suppress All Messages
##
SILENT ?= 0


GENE_TCF = arc.tcf
GENE_BCR_CONTENTS_TXT = bcr_contents.txt
##
# Argument files' filename generated from TCF
# Don't do any modification here
##
GENE_CCAC_ARG = ccac.arg
GENE_GCC_ARG = gcc.arg
#GENE_NSIM_PROPS = nsim.props
GENE_MDB_ARG = mdb.arg
GENE_CORE_CONFIG_H = core_config.h
GENE_CORE_CONFIG_S = core_config.s
##
# Apex possiblely not generated
##
GENE_APEXEXTENSIONS_H = apexextensions.h
GENE_APEXEXTENSIONS_S = apexextensions.s

## File list which might need to be generated
GENE_FILE_LIST = $(GENE_TCF) $(GENE_CCAC_ARG) $(GENE_GCC_ARG) \
	$(GENE_MDB_ARG) $(GENE_CORE_CONFIG_H) $(GENE_CORE_CONFIG_S)


## Include Scripts and Functions ##
include ./options/scripts.mk
COMMON_COMPILE_PREREQUISITES += ./options/scripts.mk



##
# Output Directory Set
##
OUT_DIR_PREFIX = obj_

## CHIP Infomation
CHIP_INFO = $(strip $(CHIP))$(strip $(CHIP_VER))
## Board Infomation
BOARD_INFO = $(strip $(BOARD))_v$(strip $(BD_VER))
## Build Infomation
BUILD_INFO = $(strip $(TOOLCHAIN))_$(strip $(CUR_CORE))
## Selected Configuration
SELECTED_CONFIG=$(BOARD_INFO)-$(BUILD_INFO)

## Objects Output Directory
BOARD_OUT_DIR = $(OUT_DIR_PREFIX)$(BOARD_INFO)
CHIP_OUT_DIR = $(OUT_DIR_PREFIX)$(CHIP_INFO)_$(BOARD_INFO)_$(APPL)
OUT_DIR = $(CHIP_OUT_DIR)/$(BUILD_INFO)

##
# Application Path and Name Setting
##
APPL_NAME = $(strip $(APPL)_$(BUILD_INFO))
APPL_FULL_NAME = $(strip $(OUT_DIR)/$(APPL_NAME))
#APPL_OUT_DIR = $(OUT_DIR)/application

##
# Generated directory - contains generated files
# Such as metaware, arc gnu, nsim argument files, generated link file
##
EMBARC_GENERATED_DIR = $(OUT_DIR)/embARC_generated

include ./boot/config.mk


####################################
# source files and object files
BOOT_CSRCS =
BOOT_ASMSRCS =
BOOT_ALLSRCS =
BOOT_INCDIRS = $(EMBARC_ROOT)/inc ./include

BOOT_COBJS =
BOOT_ASMOBJS =
BOOT_ALLOBJS =

#BOOT_DEFINES =
BOOT_DEPS =

PROJECT_ROOT_DIR = boot
PROJECT_CSRCS = boot/main.c
ifneq ($(USE_SYSTEM_TICK),)
PROJECT_CSRCS += boot/tick.c
endif
ifneq ($(UART_OTA),)
PROJECT_CSRCS += boot/uart_ota.c
endif
ifneq ($(CAN_OTA),)
PROJECT_CSRCS += boot/can_ota.c
endif
PROJECT_CSRCS += boot/flash_boot.c

ifneq ($(ELF_2_MULTI_BIN), )
PROJECT_CSRCS += boot/tiny_main.c
endif
PROJECT_CSRCS += boot/crc32.c
PROJECT_COBJS = $(call get_relobjs, $(PROJECT_CSRCS))
PROJECT_OUT_DIR = $(OUT_DIR)/boot

BOOT_INCDIRS += boot

ifneq ($(USE_SYSTEM_TICK),)
APPL_DEFINES += -DSYSTEM_TICK
endif
ifneq ($(UART_LOG_EN),)
APPL_DEFINES += -DSYSTEM_UART_LOG_EN
endif
ifneq ($(UART_OTA),)
APPL_DEFINES += -DSYSTEM_UART_OTA -DUART_OTA_ID=$(UART_OTA_ID)
else
ifneq ($(UART_LOG_EN),)
APPL_DEFINES += -DUART_OTA_ID=$(UART_OTA_ID)
endif
endif
ifneq ($(CAN_OTA),)
APPL_DEFINES += -DSYSTEM_CAN_OTA
endif
APPL_DEFINES += -DFMCW_SDM_FREQ=$(FMCW_SDM_FREQ)

##
# Application Link file definition
##
ifeq ($(TOOLCHAIN), gnu)
ifneq ($(ELF_2_MULTI_BIN), )
APPL_LINK_FILE = ./boot/linker_gnu_extra.ldf
APPL_DEFINES += -DBOOT_SPLIT
else
APPL_LINK_FILE = ./boot/linker_gnu.ldf
endif
endif
ifeq ($(TOOLCHAIN), mw)
ifneq ($(ELF_2_MULTI_BIN), )
APPL_LINK_FILE = ./boot/linker_mw_extra.ldf
APPL_DEFINES += -DBOOT_SPLIT
else
APPL_LINK_FILE = ./boot/linker_mw.ldf
endif
endif

# include toolchain settings
include ./options/toolchain.mk
COMMON_COMPILE_PREREQUISITES += ./options/toolchain.mk



include ./chip/chip.mk
COMMON_COMPILE_PREREQUISITES += ./chip/chip.mk

include ./board/board.mk
COMMON_COMPILE_PREREQUISITES += ./board/board.mk

include ./arc/arc.mk
COMMON_COMPILE_PREREQUISITES += ./arc/arc.mk

include ./device/device.mk
COMMON_COMPILE_PREREQUISITES += ./device/device.mk

include ./fmcw_radio/fmcw_radio.mk
COMMON_COMPILE_PREREQUISITES += ./fmcw_radio/fmcw_radio.mk

# include debug settings
include ./options/debug.mk
COMMON_COMPILE_PREREQUISITES += ./options/debug.mk

include ./options/rules.mk
COMMON_COMPILE_PREREQUISITES += ./options/rules.mk
