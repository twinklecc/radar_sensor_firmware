CONSOLE_ROOT = $(CALTERAH_COMMON_ROOT)/console

CONSOLE_CSRCDIR += $(CONSOLE_ROOT)

# find all the source files in the target directories
CONSOLE_CSRCS = $(call get_csrcs, $(CONSOLE_CSRCDIR))
CONSOLE_ASMSRCS = $(call get_asmsrcs, $(CONSOLE_ASMSRCDIR))

# get object files
CONSOLE_COBJS = $(call get_relobjs, $(CONSOLE_CSRCS))
CONSOLE_ASMOBJS = $(call get_relobjs, $(CONSOLE_ASMSRCS))
CONSOLE_OBJS = $(CONSOLE_COBJS) $(CONSOLE_ASMOBJS)

# get dependency files
CONSOLE_DEPS = $(call get_deps, $(CONSOLE_OBJS))


# genearte library
CONSOLE_LIB = $(OUT_DIR)/lib_console.a

COMMON_COMPILE_PREREQUISITES += $(CONSOLE_ROOT)/console.mk

# library generation rule
$(CONSOLE_LIB): $(CONSOLE_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(CONSOLE_OBJS)

# specific compile rules
# user can add rules to compile this library
# if not rules specified to this library, it will use default compiling rules
.SECONDEXPANSION:
$(CONSOLE_COBJS): $(OUT_DIR)/%.o : %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(CONSOLE_ASMOBJS): $(OUT_DIR)/%.o : %.s $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(ASM_OPT) $< -o $@

CALTERAH_COMMON_CSRC += $(CONSOLE_CSRCS)
CALTERAH_COMMON_ASMSRCS += $(CONSOLE_ASMSRCS)

CALTERAH_COMMON_COBJS += $(CONSOLE_COBJS)
CALTERAH_COMMON_ASMOBJS += $(CONSOLE_ASMOBJS)

CALTERAH_COMMON_LIBS += $(CONSOLE_LIB)
CALTERAH_INC_DIR += $(CONSOLE_CSRCDIR)
