# Calterah Top Makefile

# Directories
CALTERAH_ROOT ?= ../
EMBARC_ROOT ?= ../../embarc_osp


# Application name
APPL ?= boot
override APPL := $(strip $(APPL))

# Selected Toolchain
TOOLCHAIN ?= gnu


APPLS_ROOT = ./

ifeq ($(TOOLCHAIN),gnu)
	COMPILE_OPT += -Wall -Wno-unused-function -Wno-format
endif

SUPPORTED_APPLS = $(basename $(notdir $(wildcard $(APPLS_ROOT)/*/*.mk)))

## Set Valid APPL
check_item_exist_tmp = $(strip $(if $(filter 1, $(words $(1))),$(filter $(1), $(sort $(2))),))
VALID_APPL = $(call check_item_exist_tmp, $(APPL), $(SUPPORTED_APPLS))

## Test if APPL is valid
ifeq ($(VALID_APPL), )
$(info APPL - $(SUPPORTED_APPLS) are supported)
$(error APPL $(APPL) is not supported, please check it!)
endif

## build directory
OUT_DIR_ROOT ?= .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

include $(APPLS_ROOT)/$(APPL)/$(APPL).mk
