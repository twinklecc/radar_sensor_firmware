
VENDOR_ROOT = $(EMBARC_ROOT)/device/peripheral/nor_flash/vendor

SUPPORTED_FLASH_TYPE = $(basename $(notdir $(wildcard $(VENDOR_ROOT)/*/*.c)))

VALID_FLASH_TYPE = $(call check_item_exist, $(FLASH_TYPE), $(SUPPORTED_FLASH_TYPE))
ifeq ($(VALID_FLASH_TYPE), )
$(info FLASH - $(SUPPORTED_FLASH_TYPE) are supported)
$(error FLASH $(FLASH_TYPE) is not supported, please check it!)
endif
#EXTRA_CSRCS += $(VENDOR_ROOT)/$(VALID_FLASH_TYPE).c

DEV_CSRCDIR +=  $(EMBARC_ROOT)/device/peripheral/nor_flash $(VENDOR_ROOT)/$(VALID_FLASH_TYPE)
DEV_INCDIR +=  $(EMBARC_ROOT)/device/peripheral/nor_flash
DEV_INCDIR +=  $(EMBARC_ROOT)/device/peripheral/nor_flash/vendor
DEV_INCDIR +=  $(EMBARC_ROOT)/device/peripheral/nor_flash/vendor/$(VALID_FLASH_TYPE)
