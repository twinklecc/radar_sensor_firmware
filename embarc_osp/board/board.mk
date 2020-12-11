##
# \defgroup	MK_BOARD	Board Makefile Configurations
# \brief	makefile related to board configurations
##

# boards root declaration
BOARDS_ROOT = $(EMBARC_ROOT)/board

SUPPORTED_BOARDS = $(basename $(notdir $(wildcard $(BOARDS_ROOT)/*/*.mk)))
BOARD ?= ref_design
USE_BOARD_MAIN ?= 1
USE_BOARD_LINK ?= 1

override BOARD := $(strip $(BOARD))
override USE_BOARD_MAIN := $(strip $(USE_BOARD_MAIN))
override USE_BOARD_LINK := $(strip $(USE_BOARD_LINK))

## Set Valid Board
VALID_BOARD = $(call check_item_exist, $(BOARD), $(SUPPORTED_BOARDS))

## Try Check BOARD is valid
ifeq ($(VALID_BOARD), )
$(info BOARD - $(SUPPORTED_BOARDS) are supported)
$(error BOARD $(BOARD) is not supported, please check it!)
endif

ifeq ($(USE_BOARD_MAIN), 1)
BOARD_CSRCDIR	+= $(BOARDS_ROOT)
BOARD_ASMSRCDIR	+= $(BOARDS_ROOT)
BOARD_INCDIR	+= $(BOARDS_ROOT)
else
BOARD_CSRCDIR	+= $(BOARDS_ROOT)/$(BOARD)
BOARD_ASMSRCDIR	+= $(BOARDS_ROOT)/$(BOARD)
BOARD_INCDIR	+= $(BOARDS_ROOT)/$(BOARD)
endif

EXTRA_BOARD_DEFINES += $(BOARD_MAIN_DEFINES) $(CHIP_DEFINES)

ifeq ($(USE_BOARD_LINK), 1)
LINKER_DIR = $(BOARDS_ROOT)
else
LINKER_DIR = $(BOARDS_ROOT)/$(BOARD)
endif

include $(BOARDS_ROOT)/$(BOARD)/$(BOARD).mk

ifeq ($(VALID_TOOLCHAIN), mw)
LINKER_SCRIPT_FILE ?= $(LINKER_DIR)/linker_template_mw.ld
else
LINKER_SCRIPT_FILE ?= $(LINKER_DIR)/linker_template_gnu.ld
endif

##
# \brief	add defines for board
##
BOARD_DEFINES += $(EXTRA_BOARD_DEFINES) -DHW_VERSION=$(BD_VER) -DBOARD_$(call uc,$(BOARD))
ifneq ($(CPU_FREQ), )
BOARD_DEFINES += -DBOARD_CPU_FREQ=$(CPU_FREQ)
endif
ifneq ($(DEV_FREQ), )
BOARD_DEFINES += -DBOARD_DEV_FREQ=$(DEV_FREQ)
endif



# extra directories need to be compiled
ifeq ($(USE_BOARD_MAIN), 1)
EXTRA_CSRCDIR += $(BOARDS_ROOT)
else
EXTRA_CSRCDIR += $(BOARDS_ROOT)/$(BOARD)
endif
