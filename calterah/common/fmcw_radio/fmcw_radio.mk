ifeq ($(CHIP_VER), A)
include $(CALTERAH_ROOT)/common/fmcw_radio/alps_a/alps_a.mk
CALTERAH_INC_DIR += $(CALTERAH_ROOT)/common/fmcw_radio/alps_a
endif

ifeq ($(CHIP_VER),B)
include $(CALTERAH_ROOT)/common/fmcw_radio/alps_b/alps_b.mk
CALTERAH_INC_DIR += $(CALTERAH_ROOT)/common/fmcw_radio/alps_b
endif

ifeq ($(CHIP_VER),MP)
include $(CALTERAH_ROOT)/common/fmcw_radio/alps_mp/alps_mp.mk
CALTERAH_INC_DIR += $(CALTERAH_ROOT)/common/fmcw_radio/alps_mp
endif
