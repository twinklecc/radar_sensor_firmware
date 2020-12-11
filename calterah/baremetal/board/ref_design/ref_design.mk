
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

#BOARD_CSRCS += $(BOARDS_ROOT)/$(BOARD)/board.c
BOARD_CSRCS += $(BOARDS_ROOT)/$(BOARD)/can_baud.c \
	       $(BOARDS_ROOT)/$(BOARD)/flash_header.c

BOARD_COBJS += $(call get_relobjs, $(BOARD_CSRCS))
BOARD_INC_DIR += $(BOARDS_ROOT)/$(BOARD)

BOARD_ROOT_DIR = $(BOARDS_ROOT)/$(BOARD)
