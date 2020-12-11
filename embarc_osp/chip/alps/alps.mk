##
# \defgroup 	MK_CHIP_ALPS	EM Starter Kit Chip Related Makefile Configurations
# \ingroup	MK_CHIP
# \brief	makefile related to alps chip configurations
##

##
# \brief	current chip directory definition
##
CHIP_ALPS_DIR = $(CHIPS_ROOT)/alps
##
##
SUPPORTED_CHIP_VERS = B MP

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

## If CUR_CORE is not in SUPPORTED_BD_VERS list, then force BD_VER and VALID_BD_VER to be 23

ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CHIP_VER),)
override CHIP_VER := B
override VALID_CHIP_VER := B
endif
endif

## Try to include different chip version makefiles
ifneq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CHIP_VER),)
$(info CHIP $(CHIP) Version - $(SUPPORTED_CHIP_VERS) are supported)
$(error $(CHIP) Version $(CHIP_VER) is not supported, please check it!)
endif
endif


## Compiler Options
CHIP_CORE_DIR = $(CHIP_ALPS_DIR)/configs/$(VALID_CHIP_VER)
COMMON_COMPILE_PREREQUISITES += $(CHIP_ALPS_DIR)/configs/core_configs.mk
include $(CHIP_ALPS_DIR)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(CHIP_ALPS_DIR)/configs/core_compiler.mk
include $(CHIP_ALPS_DIR)/configs/core_compiler.mk

## Chip Related Settings
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
#BOARD_ALPS_DEFINES += -DBOARD_SPI_FREQ=800000




##
# \brief	alps chip related source and header
##
# onchip ip object rules
ifdef ONCHIP_IP_LIST
	CHIP_ALPS_DEV_CSRCDIR += $(foreach ONCHIP_IP_OBJ, $(ONCHIP_IP_LIST), $(addprefix $(CHIP_ALPS_DIR)/drivers/ip/, $(ONCHIP_IP_OBJ)))
endif

include $(EMBARC_ROOT)/device/device.mk

##
# \brief	alps device driver related
##
CHIP_ALPS_DEV_CSRCDIR		+= $(DEV_CSRCDIR)
CHIP_ALPS_DEV_ASMSRCDIR	+= $(DEV_ASMSRCDIR)
CHIP_ALPS_DEV_INCDIR		+= $(DEV_INCDIR)

CHIP_ALPS_CSRCDIR += $(CHIP_ALPS_DEV_CSRCDIR) $(CHIP_CORE_DIR) \
			$(CHIP_ALPS_DIR)/common \
			$(CHIP_ALPS_DIR)/drivers/clkgen \
			$(CHIP_ALPS_DIR)/drivers/mux \
			$(CHIP_ALPS_DIR)/drivers/lvds \
			$(CHIP_ALPS_DIR)/drivers/dmu

CHIP_ALPS_ASMSRCDIR	+= $(CHIP_ALPS_DEV_ASMSRCDIR) $(CHIP_CORE_DIR)
CHIP_ALPS_INCDIR	+= $(CHIP_ALPS_DEV_INCDIR) $(CHIP_CORE_DIR) \
				$(CHIP_ALPS_DIR)/common \
				$(CHIP_ALPS_DIR)/drivers/clkgen \
				$(CHIP_ALPS_DIR)/drivers/mux \
				$(CHIP_ALPS_DIR)/drivers/lvds \
				$(CHIP_ALPS_DIR)/drivers/dmu \
				$(CHIP_ALPS_DIR)/drivers/ip/can_r2p0

# find all the source files in the target directories
CHIP_ALPS_CSRCS = $(call get_csrcs, $(CHIP_ALPS_CSRCDIR))
CHIP_ALPS_ASMSRCS = $(call get_asmsrcs, $(CHIP_ALPS_ASMSRCDIR))

# get object files
CHIP_ALPS_COBJS = $(call get_relobjs, $(CHIP_ALPS_CSRCS))
CHIP_ALPS_ASMOBJS = $(call get_relobjs, $(CHIP_ALPS_ASMSRCS))
CHIP_ALPS_OBJS = $(CHIP_ALPS_COBJS) $(CHIP_ALPS_ASMOBJS)

# get dependency files
CHIP_ALPS_DEPS = $(call get_deps, $(CHIP_ALPS_OBJS))

# extra macros to be defined
CHIP_ALPS_DEFINES += $(CORE_DEFINES) $(DEV_DEFINES) -DCHIP_ALPS_$(VALID_CHIP_VER)

# genearte library
CHIP_LIB_ALPS = $(OUT_DIR)/libchip_alps.a

# library generation rule
$(CHIP_LIB_ALPS): $(CHIP_ALPS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(CHIP_ALPS_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(CHIP_ALPS_INCDIR)
BOARD_CSRCDIR += $(CHIP_ALPS_CSRCDIR)
BOARD_ASMSRCDIR += $(CHIP_ALPS_ASMSRCDIR)

BOARD_CSRCS += $(CHIP_ALPS_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(CHIP_ALPS_ASMSRCS)
BOARD_ALLSRCS += $(CHIP_ALPS_CSRCS) $(CHIP_ALPS_ASMSRCS)

BOARD_COBJS += $(CHIP_ALPS_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(CHIP_ALPS_ASMOBJS)
BOARD_ALLOBJS += $(CHIP_ALPS_OBJS)

BOARD_DEFINES += $(CHIP_ALPS_DEFINES)
BOARD_DEPS += $(CHIP_ALPS_DEPS)
BOARD_LIB = $(CHIP_LIB_ALPS)
