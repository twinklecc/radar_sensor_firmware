UNIT_TEST_ROOT = .
CALTERAH_ROOT = $(UNIT_TEST_ROOT)/../..
EMBARC_ROOT = $(CALTERAH_ROOT)/../embarc_osp

UNIT_TEST_CSRCDIRS += $(UNIT_TEST_ROOT)/cutest
UNIT_TEST_INCDIRS += $(UNIT_TEST_ROOT)/cutest $(UNIT_TEST_ROOT) $(CALTERAH_ROOT)/include
UNIT_TEST_CSRCS += $(wildcard $(UNIT_TEST_ROOT)/*.c)

UNIT_TEST_CSRCDIRS += $(CALTERAH_ROOT)/common/math

ifeq ($(CHIP_VER),A)
UNIT_TEST_INCDIRS += $(CALTERAH_ROOT)/common/math $(CALTERAH_ROOT)/common/baseband \
			$(CALTERAH_ROOT)/common/fmcw_radio/alps_a $(CALTERAH_ROOT)/common/fsm \
			$(CALTERAH_ROOT)/common/track $(CALTERAH_ROOT)/common/cluster \
			$(CALTERAH_ROOT)/common/sensor_config \
			$(CALTERAH_ROOT)/common/fmcw_radio
endif

ifeq ($(CHIP_VER),B)
UNIT_TEST_INCDIRS += $(CALTERAH_ROOT)/common/math $(CALTERAH_ROOT)/common/baseband \
			$(CALTERAH_ROOT)/common/fmcw_radio/alps_b $(CALTERAH_ROOT)/common/fsm \
			$(CALTERAH_ROOT)/common/track $(CALTERAH_ROOT)/common/cluster \
			$(CALTERAH_ROOT)/common/sensor_config \
			$(CALTERAH_ROOT)/common/fmcw_radio
endif


UNIT_TEST_CSRCS += $(foreach dir, $(UNIT_TEST_CSRCDIRS), $(wildcard $(dir)/*.c))

ifeq ($(CHIP_VER),A)
UNIT_TEST_CSRCS += $(CALTERAH_ROOT)/common/fmcw_radio/alps_a/fmcw_radio.c $(CALTERAH_ROOT)/common/baseband/radar_sys_params.c \
			$(CALTERAH_ROOT)/common/fmcw_radio/alps_a/radio_ctrl.c
endif

ifeq ($(CHIP_VER),B)
UNIT_TEST_CSRCS += $(CALTERAH_ROOT)/common/fmcw_radio/alps_b/fmcw_radio.c $(CALTERAH_ROOT)/common/baseband/radar_sys_params.c \
			$(CALTERAH_ROOT)/common/fmcw_radio/alps_b/radio_ctrl.c
endif

CC = gcc
CFLAGS = -O2
CHIP ?= ALPS_A
UNIT_TEST_DEF = -DUNIT_TEST -DCHIP_$(CHIP)

UNIT_TEST_OBJS = $(UNIT_TEST_CSRCS:%.c=%.o)

CFLAGS += $(addprefix -I, $(UNIT_TEST_INCDIRS))

AllTest : $(UNIT_TEST_CSRCS)
	$(CC) $(CFLAGS) $(UNIT_TEST_DEF) $(UNIT_TEST_CSRCS) -lm -o alltest


.PHONY : clean
clean :
	@rm ./.depend
	@rm alltest
	@rm *.o
	@rm *.d
