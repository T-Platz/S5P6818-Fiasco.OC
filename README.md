# S5P6818-Fiasco.OC
A collection of drivers and config files for running the Fiasco.OC microkernel on the S5P6818 SoC. This repository is only an extension to the [Fiasco.OC repository](https://github.com/kernkonzept/fiasco).

The ```s5p``` directory contains several drivers, which are breifly explained followingly. The whole folder is to be copied to ```/src/kern/arm/bsp``` in the aforementioned Fiasco.OC repository.
- ```config-arm-s5p6818.cpp```: Defines the ```TARGET_NAME``` symbol.
- ```kernel_uart-arm-s5p6818```: Specifies the initialization and startup of the UART component.
- ```mem_layout-arm-s5p6818.cpp```: Contains memory mapping definitions for the platform.
- ```pic-arm-s5p6818.cpp```: Configures the position independent interrupt controller (PIC).
- ```platform_control-arm-s5p6818.cpp```: Initializes the CPUs in multicore mode.
- ```reset-arm-s5p6818.cpp```: Implements the ```platform_reset``` function.
- ```timer-arm-s5p6818.cpp```: Initializes the system timer. The ARM generic timer could not be used, since it is not supported on the platform, as explained by Aaron Heise in [this article](https://medium.com/hi-z-labs/the-tale-of-the-s5p6818-architected-timer-4f252b44d7c5).
- ```uart-arm-s5p6818.cpp```: Provides functionality required by the Fiasco.OC kernel to communicate with peripherals via UART.

An additional UART driver can be found in this repository's ```uart``` directory. The contents of this folder need to be copied to ```/src/lib/uart``` in the Fiasco.OC repository.

Instructions on how to build the Fiasco.OC microkernel can be found in the Fiasco.OC repository and its wiki.
