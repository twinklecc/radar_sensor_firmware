## TCF Processing ##
VALID_TCF = $(wildcard $(TCF))

ifeq ($(VALID_TCF),)
CORE_ARG_FILES =
else
CORE_ARG_FILES_LIST = $(GENE_FILE_LIST)
CORE_ARG_FILES = $(addprefix $(EMBARC_GENERATED_DIR)/, $(CORE_ARG_FILES_LIST))
COMMON_COMPILE_PREREQUISITES += $(CORE_ARG_FILES)
EXTRA_DEFINES += -DEMBARC_TCF_GENERATED
endif


#$(info ...................)
#$(info $(CHIP_DEFINES))
#$(info $(BOARD_DEFINES))
#$(info $(TOOLCHAIN_DEFINES))
#$(info $(EXT_DEV_DEFINES))
#$(info $(APPL_DEFINES))
#$(info ...................)
## All defines ##
ALL_DEFINES += -DPLATFORM_EMBARC $(CHIP_DEFINES) $(BOARD_DEFINES) \
		$(EXT_DEV_DEFINES) $(TOOLCHAIN_DEFINES) $(APPL_DEFINES) $(EXTRA_DEFINES)


ALL_INC_DIRS = $(BOOT_INCDIRS) $(EMBARC_GENERATED_DIR)
#$(info $(BOOT_INCDIRS))

## all includes conversion  ##
ALL_INCLUDES = $(foreach dir,$(ALL_INC_DIRS),-I$(dir))
#$(info $(ALL_INCLUDES))


ALL_ALLOBJS = $(ARC_OBJS) $(CHIP_OBJS) $(BOARD_OBJS) $(DEVICE_OBJS) $(EXT_DEV_OBJS) $(PROJECT_COBJS) $(BOOT_ASMOBJS) $(BOOT_COBJS)
##
# All object sub-directories need to be created
##
ALL_OBJDIRS = $(sort $(dir $(ALL_ALLOBJS)))

##
# Generated directories required to build this applications
##
ALL_GENERATED_DIRS += $(ALL_OBJDIRS) $(EMBARC_GENERATED_DIR)/
ALL_GENERATED_DIRS_TMPFILES = $(sort $(call get_mkdir_tmps, $(ALL_GENERATED_DIRS)))


##
# Collect Dependency Files
##
APPL_DEPS = $(call get_deps, $(APPL_OBJS))
EXTRA_DEPS = $(call get_deps, $(EXTRA_OBJS))
ALL_DEPS = $(call get_deps, $(ALL_ALLOBJS))

# include dependency files of application
ifneq ($(MAKECMDGOALS),clean)
 -include $(APPL_DEPS)
 -include $(EXTRA_DEPS)
endif


##
# Clean Related
##
OUT_DIR_PREFIX_WILDCARD = $(subst \,/,$(OUT_DIR_PREFIX))
OUT_DIRS_WILDCARD = $(sort $(wildcard $(OUT_DIR_PREFIX_WILDCARD)*))
OUT_DIRS = $(subst /,$(PS), $(strip $(OUT_DIRS_WILDCARD)))

ifeq ($(strip $(OUT_DIRS)), )
DIST_DIR_CLEAN =
else
DIST_DIR_CLEAN = $(RMD) $(OUT_DIRS)
endif

TEMP_FILES2CLEAN = $(sort $(wildcard *.o *.out *.bin *.dis *.elf *.a *.hex *.map *.bak *.dump *.d *.img *.dasm *.log))
ifeq ($(strip $(TEMP_FILES2CLEAN)), )
TEMPFILES_CLEAN =
else
TEMPFILES_CLEAN = $(RM) $(TEMP_FILES2CLEAN)
endif

ifneq ($(ELF_2_MULTI_BIN), )
EXTRA_BIN_NAME = $(APPL_FULL_NAME)_extra.bin
else
EXTRA_BIN_NAME =
endif

##
# Overwrite some variables
##
override ALL_DEFINES := $(sort $(ALL_DEFINES))
override ALL_INCLUDES := $(sort $(ALL_INCLUDES))

ALL_CSRC_DIRS += $(ARC_ROOT)
ALL_CSRC_DIRS += $(CHIP_ROOT_DIR)
ALL_CSRC_DIRS += $(BOARD_ROOT_DIR)
ALL_CSRC_DIRS += $(DEVICE_CSRC_DIRS) $(DEVICE_IP_DIR) $(DEVICE_HAL_DIR) $(DEVICE_BAREMETAL_DRV_DIR)
ALL_CSRC_DIRS += $(PROJECT_ROOT_DIR)
ALL_CSRC_DIRS += $(NOR_FLASH_ROOT_DIR)

ALL_ASMSRC_DIRS += $(ARC_ROOT)

#$(info $(ALL_CSRC_DIRS))
#
#  the definition of source file directory
#
vpath %.C $(ALL_CSRC_DIRS)
vpath %.c $(ALL_CSRC_DIRS)
vpath %.S $(ALL_ASMSRC_DIRS)
vpath %.s $(ALL_ASMSRC_DIRS)

.PHONY : all build dump dasm bin hex size clean distclean run gui help

all : $(APPL_FULL_NAME).elf

build : clean all bin

dump : $(APPL_FULL_NAME).dump

dasm : $(APPL_FULL_NAME).dasm

bin : $(APPL_FULL_NAME).bin $(EXTRA_BIN_NAME) $(APPL_FULL_NAME).ini

hex : $(APPL_FULL_NAME).hex

help :
	@$(ECHO) 'Build Targets for selected configuration:'
	@$(ECHO) '  all         - Build example'
	@$(ECHO) '  bin         - Build and Generate binary for example'
	@$(ECHO) '  hex         - Build and Generate Intel Hex File for example'
	@$(ECHO) '  build       - Clean first then compile example'
	@$(ECHO) '  dump        - Generate dump information for example'
	@$(ECHO) '  dasm        - Disassemble object file'
	@$(ECHO) '  size        - Display size information of object file'
	@$(ECHO) 'Clean Targets:'
	@$(ECHO) '  clean       - Remove object files of selected configuration'
	@$(ECHO) '  distclean   - Remove object files of all boards'
	@$(ECHO) 'Debug & Run Targets for selected configuration:'
	@$(ECHO) '  run         - Use MDB & JTAG to download and run object elf file'
	@$(ECHO) '  gui         - Use MDB & JTAG to download and debug object elf file'

size : $(APPL_FULL_NAME).elf
	@$(ECHO) "Print Application Program Size"
	$(Q)$(SIZE) $(SIZE_OPT) $<

clean :
	@$(ECHO) "Clean Workspace For Selected Configuration : $(SELECTED_CONFIG)"
	-$(IFEXISTDIR) $(subst /,$(PS),$(OUT_DIR)) $(ENDIFEXISTDIR) $(RMD) $(subst /,$(PS),$(OUT_DIR))
	-$(IFEXISTDIR) .sc.project $(ENDIFEXISTDIR) $(RMD) .sc.project
	-$(TEMPFILES_CLEAN)

distclean :
	@$(ECHO) "Clean All"
	-$(IFEXISTDIR) .sc.project $(ENDIFEXISTDIR) $(RMD) .sc.project
	-$(DIST_DIR_CLEAN)
	-$(TEMPFILES_CLEAN)

run : $(APPL_FULL_NAME).elf
	@$(ECHO) "Download & Run $<"
	$(DBG) $(DBG_HW_FLAGS) $< $(CMD_LINE)

gui : $(APPL_FULL_NAME).elf
	@$(ECHO) "Download & Debug $<"
	$(DBG) $(DBG_HW_FLAGS) $< $(CMD_LINE)


#####RULES FOR GENERATING ARGUMENT FILE USING TCF#####
$(EMBARC_GENERATED_DIR)/$(GENE_TCF): $(VALID_TCF) $(EMBARC_GENERATED_DIR)/.mkdir_done
ifeq ($(UPGRADE_TCF), 1)
## Upgrade TCF file needed.
	@$(ECHO) "Upgrade to latest TCF file $@"
	$(Q)$(TCFGEN) -q -i $< -o $@ -ob $(dir $@)/$(GENE_BCR_CONTENTS_TXT)
else
## No need to upgrade TCF file
	@$(ECHO) "Copy TCF file $< to $@"
#	$(Q)-$(IFEXISTFILE) $(subst /,$(PS),$@) $(ENDIFEXISTFILE) $(RM) $(subst /,$(PS),$@)
#	$(Q)$(CP) $(subst /,$(PS),$<) $(subst /,$(PS),$@)
	$(Q)$(TCFTOOL) -q -d IDE -o $@ $<
endif

$(EMBARC_GENERATED_DIR)/$(GENE_CCAC_ARG): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate Metaware compiler argument file $@"
	$(Q)$(TCFTOOL) -q -x mw_compiler,$@ $<

$(EMBARC_GENERATED_DIR)/$(GENE_GCC_ARG): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate ARC GNU compiler argument file $@"
	$(Q)$(TCFTOOL) -q -x gcc_compiler,$@ $<

$(EMBARC_GENERATED_DIR)/$(GENE_MDB_ARG): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate Metaware Debugger argument file $@"
	$(Q)$(TCFTOOL) -q -x mw_debugger,$@ $<

$(EMBARC_GENERATED_DIR)/$(GENE_CORE_CONFIG_H): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate ARC core config header file $@"
	$(Q)$(TCFTOOL) -q -x C_defines,$@ $<

$(EMBARC_GENERATED_DIR)/$(GENE_CORE_CONFIG_S): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate ARC core config assembler file $@"
	$(Q)$(TCFTOOL) -q -x assembler_defines,$@ $<

$(EMBARC_GENERATED_DIR)/$(GENE_APEXEXTENSIONS_H): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate ARC APEX externsion header file $@"
	-$(Q)$(TCFTOOL) -q -x apex_header,$@ $<

$(EMBARC_GENERATED_DIR)/$(GENE_APEXEXTENSIONS_S): $(EMBARC_GENERATED_DIR)/$(GENE_TCF)
	@$(ECHO) "Generate ARC APEX externsion assembler file $@"
	-$(Q)$(TCFTOOL) -q -x apex_assembly,$@ $<


LINK_OPT += -nostdlib
#####RULES FOR GENERATING ELF FILE#####
.SECONDEXPANSION:
$(APPL_FULL_NAME).elf : $(ALL_GENERATED_DIRS_TMPFILES) $(CORE_ARG_FILES) $(APPL_LINK_FILE) $(ARC_OBJS) $(CHIP_OBJS) $(BOARD_OBJS) $(DEVICE_OBJS) $(DEVICE_BAREMETAL_DRV_OBJS) $(EXT_DEV_OBJS) $(PROJECT_COBJS) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_LINK)
	$(Q)$(LD) $(LINK_OPT) $(ARC_OBJS) $(CHIP_OBJS) $(BOARD_OBJS) $(DEVICE_OBJS) $(DEVICE_BAREMETAL_DRV_OBJS) $(EXT_DEV_OBJS) $(PROJECT_COBJS) -o $@

.SECONDEXPANSION:
$(PROJECT_COBJS) :$(PROJECT_OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(NOR_FLASH_COBJS) :$(NOR_FLASH_OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(DEVICE_COBJS) :$(DEVICE_IP_OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(DEVICE_BAREMETAL_DRV_COBJS) :$(DEVICE_BAREMETAL_DRV_OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(BOARD_COBJS) :$(BSP_OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(CHIP_COBJS) :$(CPU_OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(ARC_COBJS) :$(OUT_DIR)/arc/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(ARC_ASMOBJS) :$(OUT_DIR)/arc/%.o: %.s $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(ASM_OPT) $< -o $@


#####RULES FOR CREATING REQUIRED DIRECTORIES#####
%/.mkdir_done:
	$(TRACE_CREATE_DIR)
	$(TRY_MK_OBJDIR)
	@$(ECHO) $(@D) > $@

#####RULES FOR GENERATING DUMP/DASM/BIN/HEX FILE#####
$(APPL_FULL_NAME).dump : $(APPL_FULL_NAME).elf
	@$(ECHO) "Dumping $<"
	$(Q)$(DMP) $(DMP_OPT) $<  > $@

$(APPL_FULL_NAME).dasm : $(APPL_FULL_NAME).elf
	@$(ECHO) "Disassembling $<"
	$(Q)$(DMP) $(DASM_OPT) $<  > $@

$(APPL_FULL_NAME).bin : $(APPL_FULL_NAME).elf
	@$(ECHO) "Generating Binary $@"
	$(Q)$(OBJCOPY) $(ELF2BIN_OPT) $< $@

$(EXTRA_BIN_NAME) : $(APPL_FULL_NAME).elf
	@$(ECHO) "Generating extra Binary $@"
	$(Q)$(OBJCOPY) $(ELF2EXTRABIN_OPT)=$@ $<

$(APPL_FULL_NAME).ini : $(APPL_FULL_NAME).elf
	@$(ECHO) "Generating image info $@"
	@$(ECHO) $(TOOLCHAIN) > $@
	$(Q)$(OBJDUMP) $(BIN_LOAD_OPT) $< | $(GREP) $(SEC_INIT) >> $@
	$(Q)$(OBJDUMP) $(BIN_LOAD_OPT) $< | $(GREP) $(SEC_DATA) >> $@
	$(Q)$(OBJDUMP) $(BIN_LOAD_OPT) $< | $(GREP) $(SEC_HEADER) >> $@
	$(Q)$(OBJDUMP) $(BIN_EXEC_OPT) $< | $(GREP) $(EXEC_ENTRY) >> $@

$(APPL_FULL_NAME).hex : $(APPL_FULL_NAME).elf
	@$(ECHO) "Generating Intel Hex File $@"
	$(Q)$(ELF2HEX) $(ELF2HEX_INOPT) $< $(ELF2HEX_OUTOPT) $@

#####RULES FOR WORKAROUNDS#####
<built-in> :
	@$(ECHO) "Rules to fix built-in missing caused by metaware compiler 2014.12"
