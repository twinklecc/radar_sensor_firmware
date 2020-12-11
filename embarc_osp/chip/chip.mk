##
# \defgroup	MK_CHIP	CHIP Makefile Configurations
# \brief	makefile related to chip configurations
##

#executing platform environment: fpga/chip/...
PLAT_ENV ?= CHIP
override PLAT_ENV := $(strip $(PLAT_ENV))

# chips root declaration
CHIPS_ROOT = $(EMBARC_ROOT)/chip

##
# CHIP
# select the target chip
# scan the sub-dirs of chip to get the supported chips
SUPPORTED_CHIPS = $(basename $(notdir $(wildcard $(CHIPS_ROOT)/*/*.mk)))
CHIP ?= alps

override CHIP := $(strip $(CHIP))

CHIP_CSRCDIR	+= $(CHIPS_ROOT)
CHIP_ASMSRCDIR	+= $(CHIPS_ROOT)
CHIP_INCDIR	+= $(CHIPS_ROOT)

## Set Valid Chip
VALID_CHIP = $(call check_item_exist, $(CHIP), $(SUPPORTED_CHIPS))

## Try Check CHIP is valid
ifeq ($(VALID_CHIP), )
$(info CHIP - $(SUPPORTED_CHIPS) are supported)
$(error CHIP $(CHIP) is not supported, please check it!)
endif

#chip definition
CHIP_ID = $(call uc,CHIP_$(VALID_CHIP))
CHIP_DEFINES += -D$(CHIP_ID) -DPLAT_ENV_$(PLAT_ENV)
#device usage settings
#must be before include
COMMON_COMPILE_PREREQUISITES += $(CHIPS_ROOT)/$(VALID_CHIP)/$(VALID_CHIP).mk
include $(CHIPS_ROOT)/$(VALID_CHIP)/$(VALID_CHIP).mk

LINK_FILE_OPT += -DEXT_RAM_START=$(EXT_RAM_START) -DEXT_RAM_SIZE=$(EXT_RAM_SIZE)
LINK_FILE_OPT += -DEXT_TEXT_XIP_START=$(EXT_TEXT_XIP_START) -DEXT_TEXT_XIP_SIZE=$(EXT_TEXT_XIP_SIZE)

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(CHIP_DEPS)
endif
