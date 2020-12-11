## Firmware version selection(debug/release, default:debug) ##
FW ?= debug

REL_FILE_NAME ?= firmware_release_version
FILE_FULL_NAME = $(FIRMWARE_ROOT)/firmware_release_version.txt
FILE_NAME = $(basename $(notdir $(wildcard $(FILE_FULL_NAME))))

VALID_REL_FILE = $(call check_item_exist_tmp, $(REL_FILE_NAME), $(FILE_NAME))

ifeq ($(VALID_REL_FILE), )
$(error firmware_release_version.txt doesn't exist, please check it!)
endif

VALID_GIT ?= $(wildcard .git)

ifeq ($(VALID_GIT), .git)
ifeq ($(FW), release)
VALID_TAG ?= $(shell git log -1 --decorate=full | awk -F '/'  '{if(NR == 1)print $$5}')
ifeq ($(VALID_TAG), )
$(error  Please creat a tag for this release firstly!!!)
endif

$(shell sed -i '6,10d' $(FILE_FULL_NAME); \
git log -1 --decorate=full | sed -n '1,3p' | sed '2d' | sed '/commit/s/H.*tags\///g' \
| sed '/commit/s/,.*D//g' | sed '/commit/s/commit/Release Commit ID :/g' | sed '/Date/s/Date:/Release Date :/g'\
| sed '2a \\\n================================================================' >> $(FILE_FULL_NAME); \
sed -i 's/Release Commit ID/\nRelease Commit ID/g' $(FILE_FULL_NAME))

TAG_VERSION = $(shell awk -F '[()]'  '{if(NR == 7)print $$2}' $(FILE_FULL_NAME))
MAJOR_VERSION_ID ?= $(shell awk -F '[V.()]'  '{if(NR == 7)print $$3}' $(FILE_FULL_NAME))
MINOR_VERSION_ID ?= $(shell awk -F '[V.()]'  '{if(NR == 7)print $$4}' $(FILE_FULL_NAME))
STAGE_VERSION_ID ?= $(shell awk -F '[V.()]'  '{if(NR == 7)print $$5}' $(FILE_FULL_NAME))
SYS_NAME_STR_TMP ?= $(shell awk -F '[:]'  '{if(NR == 5)print $$2}' $(FILE_FULL_NAME))
SYS_NAME_STR ?= \"$(strip $(SYS_NAME_STR_TMP))\"

ifneq ($(CHIP_CASCADE),)
# CA is for CASCADE
CHIP_STR = CA
SYS_NAME_STR_TMP := $(SYS_NAME_STR_TMP)_$(strip $(CHIP_STR))
SYS_NAME_STR := \"$(strip $(SYS_NAME_STR_TMP))\"
endif

$(shell sed -i '1c $(TAG_VERSION)' $(FILE_FULL_NAME); \
sed -i '2c ---major version id: $(MAJOR_VERSION_ID)' $(FILE_FULL_NAME); \
sed -i '3c ---minor version id: $(MINOR_VERSION_ID)' $(FILE_FULL_NAME); \
sed -i '4c ---stage version id: $(STAGE_VERSION_ID)' $(FILE_FULL_NAME); \
sed -i '5c ---system information: $(SYS_NAME_STR_TMP)' $(FILE_FULL_NAME))
else
MAJOR_VERSION_ID ?= 0
MINOR_VERSION_ID ?= 0
STAGE_VERSION_ID ?= 0
ifneq ($(CHIP_CASCADE),)
SYS_NAME_STR_TMP ?= Calterah_Radar_System_CA
else
SYS_NAME_STR_TMP ?= Calterah_Radar_System
endif
SYS_NAME_STR ?= \"$(SYS_NAME_STR_TMP)\"
endif
BUILD_COMMIT_ID_TMP ?= $(shell git rev-parse --short HEAD)
BUILD_COMMIT_ID ?= \"$(strip $(BUILD_COMMIT_ID_TMP))\"
else
MAJOR_VERSION_ID ?= $(shell cat $(FILE_FULL_NAME) | head -n 1 | cut -d 'V' -f 2 | cut -c 1 )
MINOR_VERSION_ID ?= $(shell cat $(FILE_FULL_NAME) | head -n 1 | cut -d 'V' -f 2 | cut -c 3 )
STAGE_VERSION_ID ?= $(shell cat $(FILE_FULL_NAME) | head -n 1 | cut -d 'V' -f 2 | cut -c 5 )
SYS_NAME_STR_TMP ?= $(shell cat $(FILE_FULL_NAME) | head -n 5 | tail -n 1 | cut -d ':' -f 2)
SYS_NAME_STR ?= \"$(strip $(SYS_NAME_STR_TMP))\"
BUILD_COMMIT_ID_TMP ?= $(shell cat $(FILE_FULL_NAME) | head -n 7  | tail -n 1 |\
						cut -d ':' -f 2 | cut -c 1-7)
BUILD_COMMIT_ID ?= \"$(strip $(BUILD_COMMIT_ID_TMP))\"
endif

$(info  Firmware version: $(MAJOR_VERSION_ID).$(MINOR_VERSION_ID).$(STAGE_VERSION_ID))
$(info  System information: $(SYS_NAME_STR_TMP))
$(info  Build Commit Id: $(BUILD_COMMIT_ID_TMP))

FW_VER_DEFINES += -DFW_VER=$(FW_VER) -DMAJOR_VERSION_ID=$(MAJOR_VERSION_ID) -DMINOR_VERSION_ID=$(MINOR_VERSION_ID)
FW_VER_DEFINES += -DSTAGE_VERSION_ID=$(STAGE_VERSION_ID) -DSYS_NAME_STR=$(SYS_NAME_STR) -DBUILD_COMMIT_ID=$(BUILD_COMMIT_ID)
