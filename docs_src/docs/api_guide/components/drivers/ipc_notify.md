# IPC Notify {#DRIVERS_IPC_NOTIFY_PAGE}

[TOC]

This module define's APIs for low latency IPC between different core's on a CPU.
These low latency IPC APIs are constrained in features but offer extermely fast
transfer of message values between two cores.

Underlying implementation will use HW mechanisms to interrupt the
receiving cores, it will also use HW FIFOs (when available)
or shared memory based SW FIFOs in fast internal RAM's
to transport the message values.

\cond SOC_AM64X || SOC_AM243X
@VAR_SOC_NAME uses HW mailbox based HW FIFOs to transport the message and interrupt the receiving core.
\endcond

## Features Supported

- Low latency message send and receive between any to any CPUs running no-RTOS or RTOS
- Low latency is achieved by
  - Accessing the HW in very few steps (due to this most error checking is left to the user)
  - Combining the message and client ID into a single 32b value that is send via HW/SW FIFO's
    (due to this there are constraints on max client ID and max message value)
  - Handling the message received within ISR itself and calling user callback within few steps of receiving the interrupt.
- Client ID field allows to send messages to different SW clients on the receving side
  - Typically each SW client will be indepedant SW entity, so client ID allows to do basic
    demultiplexing of messages and thus keep the SW entities indepedant of each other even when IPC is involved.
- Ability to register different user handlers for different client ID's
- Callback based mechnism to recieve messages
- Ability to block on message send for ever OR return with error, if the underlying IPC HW/SW FIFO is full.

## SysConfig Features

@VAR_SYSCFG_USAGE_NOTE
\note IPC Examples should be build as System Projects. Single core build will fail since IPC code generation dependends on all core contexts in the application.

- Enable/Disable IPC Notify between different CPUs
- Enable/Disable SafeIPC. If Enabled, the Shared memory is organized in a way that allows Firewalling. Only Sender and Receiver core will have access to the Queue.
  Firewall configuration is not part of IPC SysCfg module. It has to be done separately. The SysCfg generated code has comments on the Start and End addresses and
  the involved cores. This can be used during the firewall configuration. Data Integrity Check has to be handled in application. CRC can be sent as a part of Payload and decoded in the receiver end.
- Enable/Disable CRC. If Enabled, Data Integrity Check is enabled for IPC Notify Data. The calculated CRC is sent as a part of Payload (ClientID + CRC + Data) and decoded in
  the receiver end. Application has to define a hook function which will be used by driver for CRC calculation. 8 Bit CRC is used by IPC Notify.

## Important Usage Guidelines

- To balance low latency performance vs flexiblility to end user, below contraints are introduced in the API
  - The maximum number of clients supported is constrained to \ref IPC_NOTIFY_CLIENT_ID_MAX.
  - The maximum message value that is exchanged is contrained to \ref IPC_NOTIFY_MSG_VALUE_MAX
- \ref IPC_NOTIFY_MSG_VALUE_MAX is < 32b, and hence one cannot pass pointers as messages.
  - However, passing pointers as messages is not a recommended SW design and instead
    one should pass offset from some known base address as values instead.
  - Offsets can easily fit within \ref IPC_NOTIFY_MSG_VALUE_MAX limit.
- Internally the implementation will combine client ID and message value as one 32b integer.

\cond SOC_AM64X
\note In case of applications involving **Linux OS**, Client ID 15 is reserved for remoteproc messages, this client ID should not be utilized by other users.
For more details refer to example at **examples/drivers/ipc/ipc_rpmsg_echo_linux/**
\endcond

## Example Usage

Include the below files to access the APIs
\snippet IpcNotify_sample.c include

Initialize IPC Notify
\snippet IpcNotify_sample.c init

Register handler to receive messages
\snippet IpcNotify_sample.c register

Send message
\snippet IpcNotify_sample.c send

Receive message and handle it in a task
\snippet IpcNotify_sample.c recv

## API

\ref DRV_IPC_NOTIFY_MODULE
