# Calterah Top Makefile

# Directories
FIRMWARE_ROOT ?= .
CALTERAH_ROOT ?= $(FIRMWARE_ROOT)/calterah
EMBARC_ROOT ?= $(FIRMWARE_ROOT)/embarc_osp


# Application name
APPL ?= sensor
override APPL := $(strip $(APPL))

# Selected OS
OS_SEL ?= freertos
override OS_SEL := $(strip $(OS_SEL))

# Selected Toolchain
TOOLCHAIN ?= gnu

ifeq ($(OS_SEL),freertos)
	APPLS_ROOT = $(CALTERAH_ROOT)/freertos
else
	APPLS_ROOT = $(CALTERAH_ROOT)/baremetal
endif

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

## Include firmware version compilation file
include $(EMBARC_ROOT)/options/version.mk

FLASH_TYPE ?= s25fls
override FLASH_TYPE := $(strip $(FLASH_TYPE))

## build directory
OUT_DIR_ROOT ?= .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile


include $(APPLS_ROOT)/$(APPL)/$(APPL).mk
