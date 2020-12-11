## Current Supported Core Configurations
##
# ====================ALPS 2.3===================================
#  arcem7d     (em7d with Secure MPU, DSP, without XY)	-configuration 0 (switch 1 up,   switch 2 up)
#  arcem9d     (em9d with XY, DSP, FPUsp)		-configuration 1 (switch 1 down, switch 2 up)
#  arcem11d    (em11d with XY, DSP, FPUdp)		-configuration 2 (switch 1 up, switch 2 down)
##

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= arcem7d

## Force uDMA version to be v2
# For alps 2.3 the udma controller hardware version is v2
CORE_DEFINES += -DCORE_DMAC_INTERNAL_VERSION=2
ONCHIP_IP_LIST ?= designware/iic designware/spi designware/uart designware/qspi designware/ahb_dmac ip_hal can gpio xip

EXT_RAM_START ?= 0x700000
EXT_RAM_SIZE ?= 0x100000
