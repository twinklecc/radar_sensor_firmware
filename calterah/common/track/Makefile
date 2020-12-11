# Makefile for generate .so
TRACK_TEST_ROOT = .
CALTERAH_COMMON_ROOT = $(TRACK_TEST_ROOT)/..

TRACK_TEST_INCDIRS += $(CALTERAH_COMMON_ROOT)/baseband

DEFINES += -DTRACK_TEST_MODE

CFLAGS += $(addprefix -I, $(TRACK_TEST_INCDIRS))

CFLAGS += $(DEFINES) -g

ekf_track.so: ekf_track.c ekf_track.h track_common.h
	gcc $(CFLAGS) -shared -Wl,-soname,ekf_track -lm -o ekf_track.so -fPIC ekf_track.c -std=gnu99


.PHONY : clean
clean :
	@rm ekf_track.so
