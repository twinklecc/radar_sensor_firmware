ARC_ROOT = $(EMBARC_ROOT)/arc

ifneq ($(ELF_2_MULTI_BIN), )
ARC_CSRCS += $(ARC_ROOT)/arc_timer.c \
	      $(ARC_ROOT)/arc_exception.c \
	      $(ARC_ROOT)/startup/arc_cxx_support.c
else
ARC_CSRCS += $(ARC_ROOT)/arc_cache.c \
	      $(ARC_ROOT)/arc_timer.c \
	      $(ARC_ROOT)/arc_exception.c \
	      $(ARC_ROOT)/startup/arc_cxx_support.c
endif

ARC_ASMSRCS += $(ARC_ROOT)/arc_exc_asm.s \
	      $(ARC_ROOT)/startup/arc_startup.s

ARC_COBJS = $(call get_relobjs, $(ARC_CSRCS))
ARC_ASMOBJS = $(call get_relobjs, $(ARC_ASMSRCS))
ARC_OBJS = $(ARC_COBJS) $(ARC_ASMOBJS)

BOOT_INCDIRS += $(EMBARC_ROOT)/inc/arc

CPU_ARC_DEFINES = -DCPU_ARC
CORE_DEFINES += $(CPU_ARC_DEFINES)
