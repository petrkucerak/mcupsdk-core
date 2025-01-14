# Release Notes 08.06.00 {#RELEASE_NOTES_08_06_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless explicitly noted otherwise, the SW modules would work in both FreeRTOS and no-RTOS environment. \n
      Unless explicitly noted otherwise, the SW modules would work on any of the R5F's present on the SOC. \n

\attention Klockwork Static Analysis report is not updated for this release

## New in this Release

Feature                                                                                         | Module
------------------------------------------------------------------------------------------------|-----------------------------------
CAN SBL (basic CAN) support                                                                     | SBL
MMCSD SBL support                                                                               | SBL
MCAN transceiver based example                                                                  | MCAN
ECAP Type 3 driver and syconfig                                                                 | ECAP
EPWM TYpe 5 sysconfig enhancement                                                               | EPWM
EQEP example enhancements - Error check                                                         | EQEP
Early Ethernet with PHY in strapped mode example                                                | Ethernet
Receive packet Scatter-Gather feature support                                                   | Ethernet
Support for custom modification receive packet buffer size                                      | Ethernet
Mbed-TLS library support (software cryptography)                                                | Networking
DSCP priority mapping support in CPSW                                                           | Ethernet
ENET driver (enet-lld) APIs added to support IEEE 802.1Qav (TSN Credit Based Shapper) in CPSW   | Ethernet
R5 PMU driver and example added                                                                 | PMU

## Device and Validation Information

\cond SOC_AM263X
SOC   | Supported CPUs  | EVM                                                                          | Host PC
------|-----------------|------------------------------------------------------------------------------|-----------------------------------------
AM263x| R5F             | AM263x ControlCard Revision E1  (referred to as am263x-cc in code). \n       | Windows 10 64b or Ubuntu 18.04 64b
AM263x| R5F             | AM263x LaunchPad Revision E2  (referred to as am263x-lp in code)             | Windows 10 64b or Ubuntu 18.04 64b
\endcond

Refer here for information about using this release with E2 revision of ControlCard
- \subpage RELEASE_NOTES_08_03_00_EVM_REV_E2_SUPPORT_PAGE

## Dependent Tools and Compiler Information

Tools                   | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F            | 12.1.0
SysConfig               | R5F            | 1.14.0 build, build 2667
TI ARM CLANG            | R5F            | 2.1.2.LTS
FreeRTOS Kernel         | R5F            | 10.4.3
LwIP                    | R5F            | STABLE-2_1_2_RELEASE
Mbed-TLS                | R5F            | mbedtls-2.13.1

\attention TI ARM CLANG 2.1.2.LTS is not part of CCS by default, Follow steps at \ref INSTALL_TIARMCLANG to install the compiler

## Key Features

### Experimental Features

\attention Features listed below are early versions and should be considered as "experimental".
\attention Users can evaluate the feature, however the feature is not fully tested at TI side.
\attention TI would not support these feature on public e2e.
\attention Experimental features will be enabled with limited examples and SW modules.

Feature                                                             | Module
--------------------------------------------------------------------|--------------------------
GUI for UART Uniflash Tool                                          | Bootloader

### OS Kernel

OS              | Supported CPUs  | SysConfig Support | Key features tested                                             | Key features not tested / NOT supported
----------------|-----------------|-------------------|-----------------------------------------------------------------|----------------------------------------
FreeRTOS Kernel | R5F             | NA                | Task, Task notification, interrupts, semaphores, mutexs, timers | Task load measurement using FreeRTOS run time statistics APIs. Limited support for ROV features.
FreeRTOS POSIX  | R5F             | NA                | pthread, mqueue, semaphore, clock                               | -
NO RTOS         | R5F             | NA                | See **Driver Porting Layer (DPL)** below                        | -

### Driver Porting Layer (DPL)

Module            | Supported CPUs  | SysConfig Support | OS support       | Key features tested                                           | Key features not tested / NOT supported
------------------|-----------------|-------------------|------------------|---------------------------------------------------------------|----------------------------------------
Cache             | R5F             | YES               | FreeRTOS, NORTOS | Cache write back, invalidate, enable/disable                  | -
Clock             | R5F             | YES               | FreeRTOS, NORTOS | Tick timer at user specified resolution, timeouts and delays  | -
CpuId             | R5F             | NA                | FreeRTOS, NORTOS | Verify Core ID and Cluster ID that application is currently running on    | -
CycleCounter      | R5F             | NA                | FreeRTOS, NORTOS | Measure CPU cycles using CPU specific internal counters       | -
Debug             | R5F             | YES               | FreeRTOS, NORTOS | Logging and assert to any combo of: UART, CCS, shared memory  | -
Heap              | R5F             | NA                | FreeRTOS, NORTOS | Create arbitrary heaps in user defined memory segments        | -
Hwi               | R5F             | YES               | FreeRTOS, NORTOS | Interrupt register, enable/disable/restore, Interrupt prioritization                    | -
MPU               | R5F             | YES               | FreeRTOS, NORTOS | Setup MPU and control access to address space                 | -
Semaphore         | R5F             | NA                | FreeRTOS, NORTOS | Binary, Counting Semaphore, recursive mutexs with timeout     | -
Task              | R5F             | NA                | FreeRTOS         | Create, delete tasks                                          | -
Timer             | R5F             | YES               | FreeRTOS, NORTOS | Configure arbitrary timers                                    | -

### Secondary Bootloader (SBL)

Module     | Supported CPUs  | SysConfig Support | OS support       | Key features tested                                                         | Key features not tested / NOT supported
-----------|-----------------|-------------------|------------------|-----------------------------------------------------------------------------|----------------------------------------------------
Bootloader | R5FSS0-0        | YES               | NORTOS           | Boot modes: QSPI, UART. All R5F's. RPRC, multi-core image format            | Force Dual Core Mode, Disable Dual Core Switch and R5SS1 only not tested

### SOC Device Drivers

Peripheral   | Supported CPUs | SysConfig Support | DMA Supported                         | Key features tested                                                                                                                                             | Key features not tested / NOT supported
-------------|----------------|-------------------|---------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------
ADC          | R5F            | YES               | Yes. Example:  adc_soc_continuous_dma | Single software triggered conversion, Multiple ADC trigger using PWM, Result read using DMA, EPWM trip through PPB limit, PPB limits, PPB offsets, burst mode oversampling, differential mode, Offset, EPWM triggered conversion                | -
Bootloader   | R5F            | YES               | Yes. DMA enabled for SBL QSPI         | Boot modes: QSPI, UART. All R5F's                                                                                                                               | -
CMPSS        | R5F            | YES               | NA                                    | Asynchronous PWM trip                                                                                                                                           | -
CPSW         | R5F            | YES               | No                                    | MAC loopback, PHY loopback, LWIP: Getting IP, Ping, Iperf, Layer 2 MAC, Layer 2 PTP Timestamping and Ethernet CPSW Switch support                               | -
DAC          | R5F            | YES               | Yes. Example: dac_sine_dma            | Constant voltage, Square wave generation, Sine wave generation with and without DMA, Ramp wave generation, Random Voltage generation                            | -
ECAP         | R5F            | YES               | No                                    | ECAP APWM mode, PWM capture                                                                                                                                     | -
EDMA         | R5F            | YES               | NA                                    | DMA transfer using interrupt and polling mode, QDMA Transfer, Channel Chaining, PaRAM Linking                                                                   | -
EPWM         | R5F            | YES               | Yes. Example: epwm_dma                | PWM outputs A and B in up-down count mode, Trip zone, Update PWM using EDMA, Valley switching, High resolution time period adjustment, type5 feature            | -
EQEP         | R5F            | YES               | NA                                    | Speed and Position measurement.                                                                                                                                 | Frequency Measurement not tested
FSI          | R5F            | YES               | Yes. Example: fsi_loopback_dma        | RX, TX, polling, interrupt mode, Dma, single lane loopback.                                                                                                     | - FSI Spi Mode
GPIO         | R5F            | YES               | NA                                    | Output, Input and Interrupt functionality                                                                                                                       | -
I2C          | R5F            | YES               | No                                    | Controller mode, basic read/write                                                                                                                                   | -
IPC Notify   | R5F            | YES               | NA                                    | Mailbox functionality, IPC between RTOS/NORTOS CPUs                                                                                                             | M4F core
IPC Rpmsg    | R5F            | YES               | NA                                    | RPMessage protocol based IPC                                                                                                                                    | M4F core
MCAN         | R5F            | YES               | No                                    | RX, TX, interrupt and polling mode, Corrupt Message Transmission Prevention, Error Passive state, Bus Off State, Bus Monitoring Mode                            | -
MCSPI        | R5F            | YES               | Yes. Example: mcspi_loopback_dma      | Controller/Peripheral mode, basic read/write, polling, interrupt and DMA mode                                                                                            | -
MDIO         | R5F            | YES               | NA                                    | Register read/write, link status and link interrupt enable API                                                                                                  | -
MPU Firewall | R5F            | YES               | NA                                    | Only compiled (Works only on HS-SE  device)                                                                                                                     | -
MMCSD        | R5F            | YES               | NA                                    | MMCSD 4bit, Raw read/write                                                                                                  | - file IO, eMMC
PINMUX       | R5F            | YES               | NA                                    | Tested with multiple peripheral pinmuxes
PMU          | R5F            | NO                | NA                                    | Tested various PMU events                                                                                   | Counter overflow detection is not enabled                                                                                                                        | -
PRUICSS      | R5F            | YES               | NA                                    | Tested with Ethercat FW HAL                                                                                                                                     | -
QSPI         | R5F            | YES               | Yes. Example: qspi_flash_dma_transfer | Read direct, Write indirect, Read/Write commands, DMA for read                                                                                                  | -
RTI          | R5F            | YES               | No                                    | Counter read, timebase selction, comparator setup for Interrupt, DMA requests                                                                                   | Capture feature, fast enabling/disabling of events not tested
SDFM         | R5F            | YES               | No                                    | Filter data read from CPU, Filter data read with PWM sync                                                                                                       | -
SOC          | R5F            | YES               | NA                                    | Lock/unlock MMRs, clock enable, set Hz, Xbar configuration, SW Warm Reset, Address Translation                                                                  | -
SPINLOCK     | R5F            | NA                | NA                                    | Lock, unlock HW spinlocks                                                                                                                                       | -
UART         | R5F            | YES               | Yes. Example: uart_echo_dma           | Basic read/write at baud rate 115200, polling, interrupt mode                                                                                                   | HW flow control not tested, DMA mode not supported
WATCHDOG     | R5F            | YES               | NA                                    | Reset mode, Interrupt mode                                                                                       | -

### Board Device Drivers

Peripheral | Supported CPUs | SysConfig Support | Key features tested                                         | Key features not tested
-----------|----------------|-------------------|-------------------------------------------------------------|------------------------
EEPROM     | R5F            | YES               | Only compiled                                               | -
ETHPHY     | R5F            | YES               | Tested with ethercat_slave_beckhoff_ssc_demo example        | -
FLASH      | R5F            | YES               | QSPI Flash                                                  | -
LED        | R5F            | YES               | GPIO                                                        | -

### CMSIS

Module                      | Supported CPUs | SysConfig Support | OS Support        | Key features tested                                                                         | Key features not tested
----------------------------|----------------|-------------------|-------------------|---------------------------------------------------------------------------------------------|------------------------
-                           | -              | -                 | -                 | -                                                                                           |  -

### Industrial Communications Toolkit

Module                                | Supported CPUs | SysConfig Support | OS Support  | Key features tested                                                                                      | Key features not tested
--------------------------------------|----------------|-------------------|-------------|----------------------------------------------------------------------------------------------------------|------------------------
EtherCAT SubDevice FWHAL                  | R5F            | NO                | FreeRTOS    | Tested with ethercat_slave_beckhoff_ssc_demo example                                                     | Reset isolation

### Motor Control

Module                      | Supported CPUs | SysConfig Support | OS Support        | Key features tested                             | Key features not tested
----------------------------|----------------|-------------------|-------------------|-------------------------------------------------|------------------------
-                           | -              | -                 | -                 | -                                               |  -

### Networking

Module                      | Supported CPUs | SysConfig Support | OS Support  | Key features tested                                                                    | Key features not tested
----------------------------|----------------|-------------------|-------------|----------------------------------------------------------------------------------------|------------------------
LwIP                        | R5F            | YES               | FreeRTOS    | TCP/UDP IP networking stack with and without checksum offload enabled, TCP/UDP IP networking stack with server and client functionality, basic Socket APIs, netconn APIs and raw APIs, DHCP, ping, TCP iperf, scatter-gather, DSCP priority mapping                         | Other LwIP features
Ethernet driver (ENET)      | R5F            | YES               | FreeRTOS    | Ethernet as port using CPSW, MAC loopback and PHY loopback, Layer 2 MAC, Packet Timestamping, CPSW Switch, CPSW EST, interrupt pacing, Policer and Classifier, MDIO Manual Mode, CBS (IEEE 802.1Qav), Strapped PHY (Early Ethernet) | RMII mode
ICSS-EMAC                   | R5F            | YES               | FreeRTOS    | Only compiled                                                                          | Not tested
Mbed-TLS                    | R5F            | NO                | FreeRTOS    | Tested software cryptography after porting, used mbedTLS with LwIP to implement HTTPS server  | Hardware offloaded cryptography

### Demos

Module                      | Supported CPUs | SysConfig Support | OS Support        | Key features tested                             | Key features not tested
----------------------------|----------------|-------------------|-------------------|-------------------------------------------------|------------------------
-                           | -              | -                 | -                 | -                                               |  -

### Safety Diagnostic Library

Module            | Supported CPUs  | SysConfig Support | OS support       | Key features tested                                                                            | Key features not tested / NOT supported
------------------|-----------------|-------------------|------------------|------------------------------------------------------------------------------------------------|----------------------------------------
MCRC              | R5F             | NA                |  NORTOS | Full CPU, Auto CPU Mode and Semi CPU Auto Mode                                                          | -
DCC               | R5F             | NA                |  NORTOS | Single Shot and Continuous modes                                    | -
PBIST             | R5F             | NA                |  NORTOS | Memories supported by MSS PBIST controller.          | -
ESM               | R5F             | NA                |  NORTOS | Tested in combination with RTI, DCC                                        | -
RTI               | R5F             | NA                |  NORTOS | WINDOWSIZE_100_PERCENT, WINDOWSIZE_50_PERCENT ,Latency/Propagation timing error(early)(50% window),Latency/Propagation timing error(late)(50% window)                                     | -
ECC               | R5F             | NA                |  NORTOS | ECC of MSS_L2, R5F TCM, MCAN, VIM, ICSSM      | R5F Cache, HSM, CPSW
Bus Safety        | R5F             | NA                |  NORTOS | AHB, AXI, TPTC                           | -
CCM               | R5F             | NA                |  NORTOS | CCM Self Test Mode,Error Forcing Mode and Self Test Error Forcing Mode.                      | -
R5F STC(LBIST)    | R5F             | NA                |  NORTOS | STC of R5F.                                                 |-
PARITY            | R5F             | NA                |  NORTOS | TCM and DMA memories                                                |-
## Fixed Issues

<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> Applicable Releases
    <th> Applicable Devices
    <th> Resolution/Comments
</tr>
<tr>
    <td> MCUSDK-9460
    <td> LIN SDK function is setting a improper register bit for selecting the checksum type in the register
    <td> LIN
    <td> 8.4.0 onwards
    <td> AM263x
    <td> None
</tr>
<tr>
    <td> MCUSDK-9313
    <td> EPWM : SYSCFG : Mixed Event selection is missing from the DC and ET Sub Modules
    <td> EPWM
    <td> 8.4.0 onwards
    <td> AM263x
    <td> None
</tr>
<tr>
    <td> MCUSDK-9044
    <td> Strapping mode in phy is not giving correct link speed
    <td> Ethernet
    <td> 8.5.0 onwards
    <td> AM263x
    <td> Phy configuration for strapped and forced mode was not correctly handled
</tr>
<tr>
    <td> MCUSDK-8994
    <td> EQEP frequency measurement example fails
    <td> EQEP
    <td> 8.5.0 onwards
    <td> AM263x
    <td> Incorrect value used for pass/fail criteria and typecasting bug in EQEP frequency measurement example
</tr>
<tr>
    <td> MCUSDK-8974
    <td> LIN External Example is only sending the LIN ID and no packet data
    <td> LIN
    <td> 8.4.0 onwards
    <td> AM263x
    <td> Incorrect lin configuration
</tr>
<tr>
    <td> MCUSDK-8383
    <td> Load from JSON feature fails in SysConfig in Windows PC
    <td> Flash
    <td> 8.4.0 onwards
    <td> AM263x
    <td> Updated the sysconfig to use OS agnostic copy function
</tr>
<tr>
    <td> MCUSDK-7320
    <td> Errata ADC: DMA Read of Stale Result
    <td> ADC
    <td> 8.4.0 onwards
    <td> AM263x
    <td> refer Errata i2355
</tr>
<tr>
    <td> MCUSDK-2557
    <td> AM263x_lp:EQEP examples are not working
    <td> EQEP
    <td> 8.2.0 onwards
    <td> AM263x
    <td> Incorrect value used for pass/fail criteria and typecasting bug in EQEP frequency measurement example
</tr>
<tr>
    <td> MCUSDK-9304
    <td> LWIP Ethernet CPSW Socket: Putting Udp application buffer in cached region of memory causes stale data to be sent out in Udp packets
    <td> ENET
    <td> 8.4.0 onwards
    <td> AM263x
    <td> Fixed the udp examples and added udp client socket example
</tr>
<tr>
    <td> MCUSDK-9185
    <td> Enet Lwip Ethernet CPSW example: Correct MAC address not available from EEPROM on custom board and Pg1.0 lp causes example crash
    <td> ENET
    <td> 8.4.0 onwards
    <td> AM263x
    <td> Fixed
</tr>
<tr>
    <td> PROC_SDL-5739
    <td> MCRC Auto mode was not implemented correctly.
    <td> SDL
    <td> 8.5.0 onwards
	<td> AM263x
    <td> Fixed the source and example.
</tr>
<tr>
    <td> MCUSDK-9578
    <td> ICSS-EMAC : IOCTL for statistics always returns errors
    <td> ICSS-EMAC
    <td> 7.3.0 onwards
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-9640
    <td> ICSS-EMAC : isNRT flag in ICSS_EMAC_pollPkt is not cleared appropriately
    <td> ICSS-EMAC
    <td> 8.2.0 onwards
    <td> AM64x, AM243x
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-8403
    <td> 1000000(1MHz) baud rate not working on UART
    <td> UART
    <td> 8.4.0
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-8983
    <td> EtherCAT : EDIO pins for AL event is not supported in firmware.
    <td> EtherCAT
    <td> 7.3.0 onwards
    <td> AM263x
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-9051
    <td> CAN : Sampling Point Calculation For Default Nominal and Data baud rate configuration not present in SysCfg
    <td> CAN
    <td> 8.5.0
    <td> AM263x
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-9523
    <td> ENET : PhyState polling frequency is incorrectly set
    <td> ENET
    <td> 8.5.0
    <td> AM263x
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-6909
    <td> EPWM: Emulation mode doesn't work
    <td> EPWM
    <td> 8.4.0 onwards
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-7915
    <td> SDFM: EPWM filter sync example does not configure and check the PWM synchronization
    <td> SDFM
    <td> 8.3.0 onwards
    <td> None
</tr>
<tr>
    <td> MCUSDK-8348
    <td> EnetDma_initPktInfo does not initialized chkSumInfo member
    <td> Enet
    <td> 8.4.0 onwards
    <td> Fixed
</tr>
<tr>
    <td> MCUSDK-8989
    <td> WDT Reset example takes more than expiration time to reset.
    <td> WDT
    <td> 8.5.0
    <td> None
</tr>
<tr>
    <td> MCUSDK-8072
    <td> EnetBoard_setupPorts does not provide config option to enable internal delay for RGMII
    <td> Enet
    <td> 8.4.0 onwards
    <td> None
</tr>
<tr>
    <td> MCUSDK-9480
    <td> EPWM : HR Updown example crashing on AM263x-LP
    <td> EPWM
    <td> 8.5.0
    <td> None
</tr>
<tr>
    <td> MCUSDK-9595
    <td> MCPSI Lookback DMA example locks up when the Bit Rate is set to < 12.5Mhz
    <td> McSPI
    <td> 8.4.0 onwards
    <td> None
</tr>
<tr>
    <td> MCUSDK-9651
    <td> Enet - Host port statistics driver and CSL overlay is not correct
    <td> Enet
    <td> 8.5.0
    <td> None
</tr>
</table>

## Known Issues
<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> Reported in release
    <th> Workaround
</tr>
<tr>
    <td> MCUSDK-2294
    <td> GPIO Pin Direction
    <td> GPIO. GPIO Pin Direction not getting automatically configured.
    <td> 8.0.0
    <td> Use GPIO_setDirMode to set pin direction for GPIO pin.
</tr>
<tr>
    <td> MCUSDK-4234
    <td> FSI RX Generic Trigger Test is not working
    <td> FSI
    <td> 8.3.0
    <td> -
</tr>
<tr>
    <td> SITARAAPPS-2040
    <td> Dual Core configuration issue with CSP 1.1.3 (Sitara MCU Device Support) on AM263x
    <td> CSP Gel Scripts
    <td> 8.2.1
    <td> Edit gel file as mentioned in  \ref PREREQUISITES while running  multi core applications.
</tr>
<tr>
    <td> MCUSDK-7030
    <td> Interrupt nesting is not functional as expected when you have 2 or more interrupts with different priorities
    <td> MCAN
    <td> 8.4.0
    <td> Keep the interrupt priority same in system
</tr>
<tr>
    <td> MCUSDK-7319
    <td> CONTROLSS-SDFM: Two Back-to-Back Writes to SDCPARMx Register Bit Fields CEVT1SEL, CEVT2SEL, and HZEN Within Three SD-Modulator Clock Cycles can Corrupt SDFM State Machine, Resulting in Spurious Comparator Events
    <td> SDFM
    <td> 8.4.0
    <td> Avoid back-to-back writes within three SD-modulator clock cycles or have the SDCPARMx register bit fields configured in one register write.
</tr>
<tr>
    <td> MCUSDK-7811
    <td> CPSW: Ethernet Packet corruption occurs if CPDMA fetches a packet which spans across memory banks
    <td> Ethernet CPSW
    <td> 8.3.0 onwards
    <td> Ensure from application side single ethernet packet does not span across memory banks.
</tr>
<tr>
    <td> MCUSDK-8073
    <td> UART1 not working as expected while configuring two uarts i.e UART0 and UART1 for two different cores
    <td> UART
    <td> 8.4.0 onwards
    <td> UART1 configuration from other core should be done after UART0 is configured and initialized
</tr>
<tr>
    <td> MCUSDK-8391
    <td> PRU Pin Mux configuration missing in syscfg am263x
    <td> PRU
    <td> 8.4.0
    <td> -
</tr>
<tr>
    <td> MCUSDK-8825
    <td> MCAN bit timing parameters not correct in Sysconfig generated code
    <td> MCAN
    <td> 8.4.0
    <td> -
</tr>
<tr>
    <td> MCUSDK-9082
    <td> MbedTLS - RSA exploit by kernel-privileged cache side-channel attackers
    <td> Mbed-TLS
    <td> 8.6.0
    <td> -
</tr>
<tr>
    <td> PROC_SDL-5159
    <td> SEC ECC Bus Safety for MSS_AXI_RD not supported.
    <td> SDL
    <td> 8.5.0 onwards
    <td> None.
</tr>
<tr>
    <td> PROC_SDL-5616
    <td> For ECC Bus Safety, SEC and DED are not supported for CPSW.
    <td> SDL
    <td> 8.6.0 onwards
    <td> None.
</tr>
<tr>
    <td> PROC_SDL-5617
    <td> ECC Bus safety for SEC and DED not supported for MSS_L2.
    <td> SDL
    <td> 8.6.0 onwards
    <td> None.
</tr>
<tr>
    <td> PROC_SDL-4749
    <td> AXI DED Bus Safety fail.
    <td> SDL
    <td> 8.5.0 onwards
    <td> None.
</tr>
<tr>
    <td> MCUSDK-9800
    <td> ENET: Connection reset while running HTTPS server due to insufficient packet buffers
    <td> ENET
    <td> 8.6.0 onwards
    <td> <a href="https://mbed-tls.readthedocs.io/en/latest/tech-updates/security-advisories/mbedtls-security-advisory-2021-07-1/">mbedTLS-advisory</a> <br> MCUSDK-9082
    <td> MbedTLS - RSA exploit by kernel-privileged cache side-channel attackers
    <td> Mbed-TLS
    <td> 8.6.0
    <td> -
</tr>
<tr>
    <td> MCUSDK-9813
    <td> WDT: Time to reset or generate interrupt is incorrect when run on CCS
    <td> WDT
    <td> 8.6.0 onwards
    <td> -
</tr>
<tr>
    <td> MCUSDK-9662
    <td> QSPI LLD EDMA Transfer fails for size (Unaligned) > MAX EDMA CNT
    <td> QSPI
    <td> 8.4.0 onwards
    <td> Initiate trasfer with aligned data
</tr>
<tr>
    <td> MCUSDK-9813
    <td> WDT takes incorrect time to reset or generate interrupt when run on CCS
    <td> WDT
    <td> 8.6.0
    <td> None. Works when run with sbl
</tr>
</table>

## Limitations

<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> Reported in release
    <th> Workaround
</tr>
<tr>
    <td> MCUSDK-9471
    <td> Ethernet CPSW CPDMA stuck with SOF overrun when TCP/DUP checksum offload is enabled.
    <td> Ethernet CPSW
    <td> 08.05.00 onwards
    <td> Disable TCPUDP checksum offload in receive (THOST) direction.
</tr>
</table>

## Upgrade and Compatibility Information
### Compiler Options

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
</table>

### SOC Device Drivers

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
<tr>
    <td> ECAP
    <td> HRCAP APIs and Macros
- Removed 16 functions:
    - HRCAP_enableHighResolution
    - HRCAP_disableHighResolution
    - HRCAP_enableHighResolutionClock
    - HRCAP_disbleHighResolutionClock
    - HRCAP_startCalibration
    - HRCAP_setCalibrationMode
    - HRCAP_enableCalibrationInterrupt
    - HRCAP_disableCalibrationInterrupt
    - HRCAP_getCalibrationFlags
    - HRCAP_clearCalibrationFlags
    - HRCAP_isCalibrationBusy
    - HRCAP_forceCalibrationFlags
    - HRCAP_setCalibrationPeriod
    - HRCAP_getCalibrationClockPeriod
    - HRCAP_getScaleFactor
    - HRCAP_convertEventTimeStampNanoseconds
- Removed a supported input macro from 5 functions and 1 macro:
    - ECAP_ISR_SOURCE_HR_ERROR from ECAP_forceInterrupt, ECAP_clearInterrupt, ECAP_getInterruptSource, ECAP_disableInterrupt, ECAP_enableInterrupt. And from ECAP_ISR_SOURCE_ALL
- Removed 2 enums and 3 macros:
    - HRCAP_CalibrationClockSource,
    - HRCAP_ContinuousCalibrationMode.
    - HRCAP_GLOBAL_CALIBRATION_INTERRUPT,
    - HRCAP_CALIBRATION_DONE and
    - HRCAP_CALIBRATION_PERIOD_OVERFLOW
- CSLR change:
    - removed register offsets and related CSL for HRCAP in ECAP.
    <td> removed
    <td> HRCAP is not supported in AM263x.
</tr>

</table>

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
<tr>
    <td> FSI
    <td> Macro FSI_RX_MASTER_CORE_RESET, FSI_TX_MASTER_CORE_RESET
    <td> API/MACRO/STRUCTURE name are updated while keeping the case sensitivity from **MASTER** to **MAIN**
    <td> Updated to use the inclusive naming
</tr>
<tr>
    <td> I2C
    <td> Structure I2C_Transaction member slaveAddress, masterMode
    <td> API/MACRO/STRUCTURE name are updated while keeping the case sensitivity from **master** to **controller** and **slave** to **target**, for example..\n
    slaveAddress->targetAddress
    <td> Updated to use the inclusive naming
</tr>
<tr>
    <td> LIN
    <td> Enum LIN_LINMode member LIN_MODE_LIN_SLAVE, LIN_MODE_LIN_MASTER\n
    Enum LIN_MessageFilter member LIN_MSG_FILTER_IDSLAVE\n
    Function LIN_setIDSlaveTask
    <td> API/MACRO/STRUCTURE name are updated while keeping the case sensitivity from **master** to **commander** and **slave** to **responder**, for example..\n
    LIN_setIDSlaveTask->LIN_setIDResponderTask
    <td> Updated to use the inclusive naming
</tr>
<tr>
    <td> McSPI
    <td> MACRO MCSPI_MS_MODE_MASTER, MCSPI_MS_MODE_SLAVE
    <td> Replaced csumOffloadEn parameter with txCsumOffloadEn and rxCsumOffloadEn.
    <td> This enables support to control TXP/DUP checksum offload along Rx and Tx seperatly.
</tr>
<tr>
    <td> SDFM
    <td> MACRO SDFM_MASTER_INTERRUPT_FLAG\n
    Function SDFM_enableMasterInterrupt\n
    Function SDFM_disableMasterInterrupt\n
    Function SDFM_enableMasterFilter\n
    Function SDFM_disableMasterFilter
    <td> API/MACRO/STRUCTURE name are updated while keeping the case sensitivity from **master** to **main**, for example..\n
    SDFM_enableMasterFilter->SDFM_enableMainFilter
    <td> Updated to use the inclusive naming
</table>

### Networking

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
<tr>
    <td> Ethernet CPSW
    <td> Structure \ref CpswHostPort_Cfg in \ref Cpsw_Cfg \n
    Function Enet_open
    <td> Replaced csumOffloadEn parameter with txCsumOffloadEn and rxCsumOffloadEn
    <td> This enables support to control TXP/DUP checksum offload along Rx and Tx separately
</tr>
<tr>
    <td> Ethernet CPSW
    <td> Structure \ref EnetCpdma_Cfg in \ref Cpsw_Cfg \n
    Function Enet_open
    <td> Removed the parameter isCacheable.
    <td> Support to place descriptor in cached memory is removed. CPDMA descriptiors has be placed un-cached memory section always.
</tr>
</table>
