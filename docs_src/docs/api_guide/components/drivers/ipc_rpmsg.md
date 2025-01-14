# IPC RPMessage {#DRIVERS_IPC_RPMESSAGE_PAGE}

[TOC]

\note These APIs are to be used from NORTOS/RTOS. When working with Linux on A53 refer to Linux documentation for Linux side APIs.

## Features Supported

- RP Message + VRING protocol implementation
\cond !SOC_AM62X
- Uses \ref DRIVERS_IPC_NOTIFY_PAGE underneath for interrupts and uses shared memory (VRING) for message buffers
\endcond
\cond SOC_AM62X
- Uses 'IPC Notify' underneath for interrupts and uses shared memory (VRING) for message buffers
\endcond
- Supports message passing between NORTOS, FreeRTOS and Linux based CPUs
- Logical communication channels can be created using unique "end points". This allows multiple tasks on a CPU to talk to multiple tasks
  on another CPU using the same underlying HW mailbox and shared memory.
\cond !SOC_AM62X
- Between NORTOS and RTOS, below can be configured in RP Message to control the shared memory size,
  - Max message size, default is 128 bytes
  - Number of buffers in a VRING, default is 8 messages
  - VRING shared memory buffer address can be configured, can be DDR or internal memory address.
\endcond
- Between Linux and NORTOS/RTOS, the VRING parameters are fixed as below
  - Max message is 512B
  - NUmber of messages in a VRING is 256
  - VRING shared memory address is determined by value in Linux device tree and is placed in DDR.
- APIs to send and receive messages to user specified end points
- Blocking, as well as non-blocking APIs with time out based blocking.
- APIs to announce a created end point, needed when talking to Linux
- API to sync and wait for LInux to be ready, needed when talking to Linux
- APIs to use callback to receive message notifications instead of blocking, see \ref RPMessage_CreateParams for more details.
- APIs to use callback to receive and handle messages in callback itself, see \ref RPMessage_CreateParams for more details.
- Logging to Linux shared memory when IPC with Linux is enabled. Allows to view logs from RTOS/NORTOS on Linux using Linux debugfs.

## SysConfig Features

@VAR_SYSCFG_USAGE_NOTE
\note IPC Examples should be build as System Projects. Single core build will fail since IPC code generation dependends on all core contexts in the application.

\cond !SOC_AM62X
- Enable/Disable IPC RPMessage between different CPUs
- Enable IPC RPMessage between NORTOS/RTOS and Linux. When enabled, SysConfig generates the resource table that is needed to talk with Linux.
- Select RP Message max message size, number of buffer in a VRING
- Enable/Disable SafeIPC. If Enabled, the Shared memory is organized in a way that allows Firewalling. Only Sender and Receiver core will have access to the VRING.
  Firewall configuration is not part of IPC SysCfg module. It has to be done separately. The SysCfg generated code has comments on the Start and End addresses and
  the involved cores. This can be used during the firewall configuration. Data Integrity Check has to be handled in application. CRC can be sent as a part of Payload and decoded in the receiver end.
- Enable/Disable CRC. If Enabled, Data Integrity Check is enabled for IPC RPMsg Data. The calculated CRC is sent as a part of VRING Header and decoded in the
  receiver end. Application has to define a hook function which will be used by driver for CRC calculation. 16 Bit CRC is used by IPC RPMsg.
\endcond
\cond SOC_AM62X
- Enable IPC RPMessage between NORTOS/RTOS and Linux. When enabled, SysConfig generates the resource table that is needed to talk with Linux.
\endcond

## Features NOT Supported
\if SOC_AM64X
- SafeIPC cannot be used to communicate with Linux core.
\else
NA
\endif

## Important Usage Guidelines

\note It is strongly recommended to refer to the IPC examples `examples/drivers/ipc` to understand the linker command file setup for IPC applications.

- When Linux runs along side RTOS/NORTOS, do below to make sure NORTOS/RTOS can talk to Linux.
 - Make sure to call RPMessage_waitForLinuxReady() before starting communication with Linux.
 - Also for any RPMessage point created on NORTOS/RTOS, make sure to announce it to Linux using RPMessage_announce()
 - Make sure to assign the shared memory used for VRING between Linux and this CPU as mentioned in the Linux device tree.
   Also make sure to mark this section as non-cached
 - If the CPU code will run out of DDR, make sure to setup a MPU entry for the code/data section in DDR. This can be
   marked as cached.
 - Again refer to Linux device tree to find out the space in DDR and MSMC where the NORTOS/RTOS applications can execute from.
 \cond !SOC_AM62X
- Make sure to assign the shared memory used for VRINGs between NORTOS/RTOS in a common memory section in each CPUs linker command file
  and make sure to mark this section as non-cached in the R5F MPU.
- Maximum Message size is limited to 1152 Bytes in Syscfg and the maximum number of buffers is limited to 16.
The recommended approach is to keep the number of buffers and message size within this limit.
- If larger messages need to be passed, the data should be kept in a shared memory and a pointer to the same should be passed via IPC.
\endcond

## Example Usage

\cond !SOC_AM62X
Include the below files to access the APIs
\snippet IpcRPMessage_sample.c include

Setup VRING shared memory for IPC RPMessage, this snippet is for reference and is generated by SysConfig when SysConfig is used for IPC.
\snippet IpcRPMessage_sample.c shared mem

Initialize IPC RPMessage, this snippet is for reference and is generated by SysConfig when SysConfig is used for IPC.
\snippet IpcRPMessage_sample.c core0 init
\snippet IpcRPMessage_sample.c core1 init

Define \ref RPMessage_Object objects for receive and ack endpoints,
\snippet IpcRPMessage_sample.c core0 obj
\snippet IpcRPMessage_sample.c core1 obj

Create RPMessage end points for receive and ack endpoints,
\snippet IpcRPMessage_sample.c core0 create
\snippet IpcRPMessage_sample.c core1 create

Send message from Core 0 to Core 1 and wait for reply from Core 1,
\snippet IpcRPMessage_sample.c send

Receive message at Core 1 from Core 0 and send ack to Core 0,
\snippet IpcRPMessage_sample.c recv
\endcond

\cond SOC_AM62X
Include the below files to access the APIs
\snippet IpcRPMessage_linux_sample.c include

Define the following macros
\snippet IpcRPMessage_linux_sample.c defines

Setup resource table to be used by Linux, this snippet is for reference and is generated by SysConfig when SysConfig is used for IPC.
\snippet IpcRPMessage_linux_sample.c resource tab

Initialize IPC RPMessage, this snippet is for reference and is generated by SysConfig when SysConfig is used for IPC.
\snippet IpcRPMessage_linux_sample.c init

Define \ref RPMessage_Object objects for receive endpoints,
\snippet IpcRPMessage_linux_sample.c obj

Create RPMessage end points for receive endpoints,
\snippet IpcRPMessage_linux_sample.c create

Receive message at M4F from A53 Linux and send ack to A53 Linux,
\snippet IpcRPMessage_linux_sample.c recv
\endcond
## API

\ref DRV_IPC_RPMESSAGE_MODULE
