
CHIP_RES_CSRCS ?=
CHIP_RES_COBJS ?=

CHIP_RES_INCDIRS ?=

BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/dmu

ifneq ($(USE_UART),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/designware/dw_uart

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/designware/dw_uart)
endif

ifneq ($(USE_GPIO),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/designware/gpio

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/designware/gpio)
endif

ifneq ($(USE_CAN),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/can

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/can_r2p0)
endif

ifneq ($(USE_SSI),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/designware/dw_ssi

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/designware/ssi)
endif

ifneq ($(USE_QSPI),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/designware/dw_qspi

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/designware/dw_qspi)
endif

ifneq ($(USE_XIP),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/xip

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/xip)
endif

ifneq ($(USE_HW_CRC),)
BOOT_INCDIRS += $(CHIP_ROOT_DIR)/drivers/ip/crc

CHIP_RES_CSRCS += $(call get_csrcs, $(CHIP_ROOT_DIR)/drivers/ip/crc)
endif

CHIP_RES_COBJS = $(call get_relobjs, $(CHIP_RES_CSRCS))
