
CALTERAH_INC_DIR += $(CALTERAH_ROOT)/include
CALTERAH_INC_DIR += $(dir $(wildcard $(CALTERAH_ROOT)/common/*/))
CALTERAH_CSRC_DIR += $(dir $(wildcard $(CALTERAH_ROOT)/common/*/))
CALTERAH_CXXSRC_DIR += $(dir $(wildcard $(CALTERAH_ROOT)/common/*/))

ifneq ($(OS_SEL),)
include $(CALTERAH_ROOT)/common/common.mk
include $(CALTERAH_ROOT)/freertos/common/common.mk
endif
