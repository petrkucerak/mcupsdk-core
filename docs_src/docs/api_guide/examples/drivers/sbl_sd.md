# SBL SD {#EXAMPLES_DRIVERS_SBL_SD}

[TOC]

# Introduction

This bootloader does SOC initializations and attempts to boot a multicore appimage file named "app" present in the first FAT partition found in the connected SD card. The file can be copied to the SD card by connecting it to the host PC using a card reader. Make sure that it is named "app" without any file extension. If the card is new, make sure that it is formatted with FAT32/16.

If a multicore appimage file is found at the location, the SBL reads the file into a buffer and parses it. Each core is then initialized, application image is loaded, entry points are set and the core is released from reset. For more on bootflow/bootloaders, please refer \ref BOOTFLOW_GUIDE

\cond SOC_AM263X || SOC_AM263PX

This bootloader runs in three steps:
- Format the SD Card with FAT32/16 (this is necessary if the card is new) and paste the *.appimage / *.mcelf image of desired application in the first FAT partition found in the Connected SD card. For connecting the SD Card to the host PC use a card reader. Make sure that the pasted file is named "app" without any file extension.
- Flashing the SBL SD at offset 0x0 (Setup the EVM in UART Boot Mode, \ref BASIC_STEPS_TO_FLASH_FILES). For flashing the SBL use the `default_sbl_sd` configuration file present in tools/boot/sbl_prebuilt.
\cond SOC_AM263X 
- Switch to \ref BOOTMODE_QSPI and Connect to UART in 5 seconds to see logs from UART
\endcond
\cond SOC_AM263PX || SOC_AM261X
- Switch to \ref BOOTMODE_OSPI and Connect to UART in 5 seconds to see logs from UART
\endcond


\imageStyle{am263x_sbl_sd_flow.png,width:9%}
\image html am263x_sbl_sd_flow.png SBL SD Flow Overview
\endcond

\cond SOC_AM263X || SOC_AM263PX || SOC_AM261X

\note RPRC image booting using SBL would be deprecated from SDK 11.00 release onwards. MCELF would be the default boot image format supported by SBL going forward.

# SBL SD MULTICORE ELF {#EXAMPLES_DRIVERS_SBL_SD_MCELF}

To parse and load an **mcelf** file via SD bootloader, use the project **examples/drivers/boot/sbl_sd_multicore_elf**

When an mcelf image is received, the SBL parses it, loads each segment to its respective core. Then the core is released from reset.

The steps to run the example is same irrespective of the image format.

\endcond

# Supported Combinations {#EXAMPLES_DRIVERS_SBL_SD_COMBOS}

\cond SOC_AM64X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 nortos
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER, @VAR_SK_BOARD_NAME_LOWER
 Example folder | examples/drivers/boot/sbl_sd

\endcond

\cond SOC_AM243X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 nortos
 Toolchain      | ti-arm-clang
 Boards         | @VAR_BOARD_NAME_LOWER
 Example folder | examples/drivers/boot/sbl_sd

\endcond

\cond SOC_AM263X || SOC_AM263PX

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 nortos
 Toolchain      | ti-arm-clang
 Boards         | @VAR_BOARD_NAME_LOWER, @VAR_LP_BOARD_NAME_LOWER
 Example folder | examples/drivers/boot/sbl_sd

\endcond

\cond SOC_AM65X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 nortos
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER,
 Example folder | examples/drivers/boot/sbl_sd

\endcond

# Steps to Run the Example

Since this is a bootloader, the example will be run every time you boot an application using this example. It is run from an SD card unlike other examples which are usually loaded with CCS. Nevertheless, you can build this example like you do for the others using makefile or build it via CCS by importing as a project.

- **When using CCS projects to build**, import the CCS project for the required combination
  and build it using the CCS project menu (see \ref CCS_PROJECTS_PAGE).
- **When using makefiles to build**, note the required combination and build using
  make command (see \ref MAKEFILE_BUILD_PAGE)
- Refer to the page \ref BASIC_STEPS_TO_FLASH_FILES to flash this bootloader along with the application to boot.

# See Also

\ref DRIVERS_BOOTLOADER_PAGE

# Sample Output

\cond SOC_AM263X || SOC_AM263PX
Logs from IPC RPMSG ECHO example:
\code
Starting SD Bootloader ...
[BOOTLOADER_PROFILE] Boot Media       : SD Card
[BOOTLOADER_PROFILE] Boot Image Size  : 183 KB
[BOOTLOADER_PROFILE] Cores present    :
r5f1-1
r5f1-0
r5f0-1
r5f0-0
[BOOTLOADER PROFILE] System_init                      :        368us
[BOOTLOADER PROFILE] Drivers_open                     :     280357us
[BOOTLOADER PROFILE] LoadHsmRtFw                      :         44us
[BOOTLOADER PROFILE] Board_driversOpen                :        405us
[BOOTLOADER PROFILE] File read from SD card           :     240375us
[BOOTLOADER PROFILE] CPU load                         :      16916us
[BOOTLOADER_PROFILE] SBL Total Time Taken             :     538468us

Image loading done, switching to application ...
[r5f0-1]     0.001077s : [IPC RPMSG ECHO] Remote Core waiting for messages from main core ... !!!
[r5f1-0]     0.001073s : [IPC RPMSG ECHO] Remote Core waiting for messages from main core ... !!!
[r5f1-1]     0.001068s : [IPC RPMSG ECHO] Remote Core waiting for messages from main core ... !!!
[IPC RPMSG ECHO] Message exchange started by main core !!!
[IPC RPMSG ECHO] All echoed messages received by main core from 3 remote cores !!!
[IPC RPMSG ECHO] Messages sent to each core = 100000
[IPC RPMSG ECHO] Number of remote cores = 3
[IPC RPMSG ECHO] Total execution time = 6349363 usecs
[IPC RPMSG ECHO] One way message latency = 10582 nsec
All tests have passed!!
\endcode
\endcond

\cond SOC_AM243X || SOC_AM64X
\code
[BOOTLOADER PROFILE] SYSFW Load                       :      17632us
[BOOTLOADER PROFILE] System_init                      :      12246us
[BOOTLOADER PROFILE] Drivers_open                     :      82499us
[BOOTLOADER PROFILE] Board_driversOpen                :          0us
[BOOTLOADER PROFILE] File read from SD card           :       6843us
[BOOTLOADER PROFILE] CPU load                         :       9501us
[BOOTLOADER_PROFILE] SBL Total Time Taken             :     129399us

Image loading done, switching to application ...
\endcode
\endcond

\cond SOC_AM65X
\code
Starting MMCSD Bootloader ...

DMSC Firmware Version 22.1.1--v2022.01 (Terrific Llam
DMSC Firmware revision 0x16
DMSC ABI revision 3.1

INFO: Bootloader_loadSelfCpu:232: CPU r5f0-0 is initialized to 400000000 Hz !!!
INFO: Bootloader_loadSelfCpu:232: CPU r5f0-1 is initialized to 400000000 Hz !!!
[BOOTLOADER_PROFILE] Boot Media       : SD Card
[BOOTLOADER_PROFILE] Boot Image Size  : 69 KB
[BOOTLOADER_PROFILE] Cores present    :
[BOOTLOADER PROFILE] System_init                      :        594us
[BOOTLOADER PROFILE] Drivers_open                     :      17398us
[BOOTLOADER PROFILE] Board_driversOpen                :          1us
[BOOTLOADER PROFILE] Sciclient Get Version            :     131273us
[BOOTLOADER PROFILE] File read from SD card           :      18256us
[BOOTLOADER PROFILE] CPU load                         :      31279us
[BOOTLOADER PROFILE] SBL End                          :          1us
[BOOTLOADER_PROFILE] SBL Total Time Taken             :     198805us

Image loading done, switching to application ...
INFO: Bootloader_runSelfCpu:242: All done, reseting self ...
\endcode
\endcond