
PMIC_ROOT = $(EMBARC_ROOT)/device/peripheral/pmic

SUPPORTED_PMIC_TYPE = $(basename $(notdir $(wildcard $(PMIC_ROOT)/*/*.c)))

VALID_PMIC_TYPE = $(call check_item_exist, $(PMIC_TYPE), $(SUPPORTED_PMIC_TYPE))
ifeq ($(VALID_PMIC_TYPE), )
$(info PMIC - $(SUPPORTED_PMIC_TYPE) are supported)
$(error PMIC $(PMIC_TYPE) is not supported, please check it!)
endif

DEV_CSRCDIR +=  $(PMIC_ROOT) $(PMIC_ROOT)/$(VALID_PMIC_TYPE)
DEV_INCDIR +=  $(EMBARC_ROOT)/device/peripheral/pmic
DEV_INCDIR +=  $(PMIC_ROOT)/$(VALID_PMIC_TYPE)
