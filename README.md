# Radar Sensor Firmware

Firmware of Calterah SoC radar sensor. It currently support chips are listed below

   * AlpsA


## Build System

Build system is organized with GNU Make. It supports both GNU and MetaWare toolchains:

   * GNU toolchain please download release version from [here](https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases)
   * MetaWare is provided by Synopsys Inc.

We **do not** provide IDE support, here is the reason:

   1. IDE is difficult to maintain large projects especially when building process is highly
      configurable. Our firmware supports multiple chips with multiple versions, which may used on
      mulitlple boards with various applications, like BSD, FCW, ACC and so on.
   2. IDE usually hide details such as building options. For example, when you mitigate from one
      version of IDE to another, there is no garantee you will get same compiling
      results. Therefore, maintaining a project file for particular IDE is not meaningfule.
   3. Key value of an IDE has two folds: fancy editor and debugger.

      * There are tons of editors with fancy GUI and fancy cross-ref functions
	  * Once the image is built, you can choose your favorite debugger. With GNU toolchain, it is
      easy to setup a debug project with *eclipse*; with MetaWare, GUI based debug session can be
      invoked through *make* command.

### How to build

Build code is simple:

   1. Go to source code root
   2. Type ``make APPL=sensor``

After it finishes, you will see a new directory create at your root directory, E.g. ``obj_alpsA_fpga_v1``, where

   1. alps: chip name
   2. A:    chip version
   3. fpga: board name
   4. v1:   board revision

Inside the directory, you will find image in *.elf together with all temporarily generated files. To
get more help, please run ``make APPL=sensor help``

To invoke the debugger

    make APPL=sensor TOOLCHAIN=gnu OPENOCD_SCRIPT_ROOT=path/inside/your/gnu/toolchain gui  # GDB console based
    make APPL=sensor TOOLCHAIN=mw  gui  # MW GUI based

### How build system is organized

TODO

## Source Code Overview

Under source code root, there three directories:

   1. ``embarc_osp``: source code pull from
      [embarc_osp](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp), where
      synopsys hosts ARC/IOT related source code. We did some modification to fit into our
      development structure, which mainly on makefiles. For future development, please keep its
      source code modifications minimal.
   2. ``calterah``: Calterah's own source code resides in this directory. That means you will work most here
   3. ``doc``: Documentation, which is currently empty

### embarc_osp

Our building system generally reuse ``embarc_osp``'s. Changes are made in following aspects:

   1. Separate configure related to ``chip`` from ``board``. This allows us to support multiple
      chip/revision in a systematic way.
   2. Some drivers related to on-chip peripheral-related IPs are included under each chip. Currently
      UART driver is brought up.

### calterah

Inside, it has four sub-directories:

   1. ``baremetal``
   2. ``freertos``
   3. ``common``
   4. ``include``

Our plan is to support both non-OS/OS based applications. Therefore, ``baremetal`` and ``freertos``
holds OS and APP specific codes, while ``common`` and ``include`` are code shared by them. ``README.md`` are provided for each directory to have more details. Please follow it carefully.


### doc

Documentation directory is currently a place-holder. We do not have much to say yet!
