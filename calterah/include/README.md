# Calterah Common Header File

Header files here are used across all levels of calterah's firmware. Module design requires definitions be localized. So please think through if it is necessary to put defines here.

**Note:** All defines/declares should be ``independent`` of freeRTOS, toolChains, drivers, chips, and boards.

## ``calterah_defines.h``

Common used definitions including structs and macros.


## ``calterah_error.h``

It will be a complement to ``embARC_error.h`` and it only includes calterah specific errors, most of which are HW related. To work with embARC's own error defines, please use negative number and make sure error number less than -128.
