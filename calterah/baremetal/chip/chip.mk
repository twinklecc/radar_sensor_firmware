
CHIPS_ROOT = $(EMBARC_ROOT)/chip

##
# CHIP
# select the target chip
# scan the sub-dirs of chip to get the supported chips
SUPPORTED_CHIPS = $(basename $(notdir $(wildcard $(CHIPS_ROOT)/*/*.mk)))
CHIP ?= alps
override CHIP := $(strip $(CHIP))

## Set Valid Chip
VALID_CHIP = $(call check_item_exist, $(CHIP), $(SUPPORTED_CHIPS))

## Try Check CHIP is valid
ifeq ($(VALID_CHIP), )
$(info CHIP - $(SUPPORTED_CHIPS) are supported)
$(error CHIP $(CHIP) is not supported, please check it!)
endif


SUPPORTED_CHIP_VERS = A B MP

## Select Chip Version
CHIP_VER ?= MP
override CHIP_VER := $(strip $(CHIP_VER))

## Set Valid Chip Version
VALID_CHIP_VER = $(call check_item_exist, $(CHIP_VER), $(SUPPORTED_CHIP_VERS))


## Check TCF file existence
ifneq ($(TCF),)
ifeq ($(wildcard $(TCF)),)
$(error Tool Configuration File(TCF) - $(TCF) doesnot exist, please check it!)
else
TCFFILE_IS_VALID = 1
TCFFILE_NAME = $(firstword $(basename $(notdir $(TCF))))
endif
endif


## Compiler Options
CHIP_CORE_DIR = $(CHIPS_ROOT)/$(CHIP)/configs/$(VALID_CHIP_VER)

COMMON_COMPILE_PREREQUISITES += $(CHIPS_ROOT)/$(CHIP)/configs/core_configs.mk
include $(CHIPS_ROOT)/$(CHIP)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(CHIPS_ROOT)/$(CHIP)/configs/core_compiler.mk
include $(CHIPS_ROOT)/$(CHIP)/configs/core_compiler.mk

## Chip Related Settings
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)



CHIP_ROOT_DIR = $(EMBARC_ROOT)/chip/$(CHIP)

BOOT_INCDIRS += $(EMBARC_ROOT)/chip $(CHIP_ROOT_DIR) \
		$(CHIP_ROOT_DIR)/common \
		$(CHIP_ROOT_DIR)/configs/$(CHIP_VER) \
		$(CHIP_ROOT_DIR)/drivers/clkgen \
		$(CHIP_ROOT_DIR)/drivers/mux

#CHIP_COMMON_CSRCS = $(call get_csrcs, $(CHIP_ROOT_DIR)/common)
#CHIP_COMMON_COBJS = $(call get_relobjs, $(CHIP_COMMON_CSRCS))
#CHIP_CSRCS += $(CHIP_COMMON_CSRCS)
#CHIP_COBJS += $(CHIP_COMMON_COBJS)

CHIP_CLKDRV_CSRCS = $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/clkgen)
CHIP_CLKDRV_COBJS = $(call get_relobjs, $(CHIP_CLKDRV_CSRCS))
CHIP_CSRCS += $(CHIP_CLKDRV_CSRCS)
CHIP_COBJS += $(CHIP_CLKDRV_COBJS)

CHIP_MUXDRV_CSRCS = $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/mux)
CHIP_MUXDRV_COBJS = $(call get_relobjs, $(CHIP_MUXDRV_CSRCS))
CHIP_CSRCS += $(CHIP_MUXDRV_CSRCS)
CHIP_COBJS += $(CHIP_MUXDRV_COBJS)

include ./chip/$(CHIP)/$(CHIP).mk
CHIP_CSRCS += $(CHIP_RES_CSRCS)
CHIP_COBJS += $(CHIP_RES_COBJS)

#how to deal with IP configs?
#ifdef CAN_UDS
#CAN config and driver.
#endif

#ifdef CASCADE_COST_DOWN
#QSPI-S config and driver.
#endif

CHIP_OBJS = $(CHIP_COBJS)

CPU_OUT_DIR = $(OUT_DIR)/chip/$(CHIP)
#$(info $(CPU_OUT_DIR))

#chip definition
CHIP_ID = $(call uc,CHIP_$(VALID_CHIP))
CHIP_DEFINES += -D$(CHIP_ID)
CHIP_DEFINES += $(CORE_DEFINES) -DPLAT_ENV_$(PLAT_ENV)
