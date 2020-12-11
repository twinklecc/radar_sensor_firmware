
# boards root declaration
BOARDS_ROOT = $(EMBARC_ROOT)/board

SUPPORTED_BOARDS = $(basename $(notdir $(wildcard $(BOARDS_ROOT)/*/*.mk)))
BOARD ?= ref_design

override BOARD := $(strip $(BOARD))

## Set Valid Board
VALID_BOARD = $(call check_item_exist, $(BOARD), $(SUPPORTED_BOARDS))

## Try Check BOARD is valid
ifeq ($(VALID_BOARD), )
$(info BOARD - $(SUPPORTED_BOARDS) are supported)
$(error BOARD $(BOARD) is not supported, please check it!)
endif

BOARD_DEFINES += -DHW_VERSION=$(BD_VER) -DBOARD_$(call uc,$(BOARD))
ifneq ($(COMMON_STACK_INIT),)
BOARD_DEFINES += -DCOMMON_STACK_INIT
endif
BOARD_DEFINES += -DSYSTEM_${SYSTEM_BOOT_STAGE}_STAGES_BOOT

BOARD_CSRCS ?=
BOARD_COBJS ?=
BOARD_INC_DIR ?=
include ./board/$(BOARD)/$(BOARD).mk

OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/$(CHIP).cfg
#OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk_v2.2.cfg
$(info $(OPENOCD_CFG_FILE))

BOOT_INCDIRS += $(BOARD_INC_DIR)

BSP_OUT_DIR = $(OUT_DIR)/board/$(BOARD)

BOARD_OBJS = $(BOARD_COBJS)
