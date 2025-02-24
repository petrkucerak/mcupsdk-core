/*
 *  Copyright (C) 2018-2023 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include <drivers/bootloader.h>
#include <drivers/bootloader/bootloader_xmodem.h>
#include <drivers/bootloader/bootloader_uniflash/bootloader_uniflash.h>

#undef FLASH_WRITE_SECTOR

/* Please make sure to keep the buffersize >= block/sector size and aligned with block/sector size */
#define BOOTLOADER_UNIFLASH_MAX_FILE_SIZE (0x190000)
/* This has to match the size of MSRAM1 section in linker.cmd */
uint8_t gFileBuf[BOOTLOADER_UNIFLASH_MAX_FILE_SIZE+BOOTLOADER_UNIFLASH_HEADER_SIZE] __attribute__((aligned(128), section(".bss.filebuf")));

#define BOOTLOADER_UNIFLASH_VERIFY_BUF_MAX_SIZE (32*1024)
uint8_t gVerifyBuf[BOOTLOADER_UNIFLASH_VERIFY_BUF_MAX_SIZE] __attribute__((aligned(128), section(".bss")));

/* Flash Write Opcode */
#ifdef FLASH_WRITE_SECTOR
#define FLASH_OPCODE    (BOOTLOADER_UNIFLASH_OPTYPE_FLASH_SECTOR)
#else
#define FLASH_OPCODE    (BOOTLOADER_UNIFLASH_OPTYPE_FLASH)
#endif

volatile uint32_t gFlashStatus;
volatile uint32_t gFlashBuffSize;
volatile Flash_Attrs *gFlashAttrs;
volatile uint32_t gFlashWriteOpcode;

/* call this API to stop the booting process and spin, do that you can connect
 * debugger, load symbols and then make the 'loop' variable as 0 to continue execution
 * with debugger connected.
 */
void loop_forever(void)
{
    volatile uint32_t loop = 1;
    while (loop)
        ;
}


int main(void)
{
    int32_t status = SystemP_SUCCESS;

    System_init();
    Drivers_open();

    status = Board_driversOpen();
    DebugP_assert(status == SystemP_SUCCESS);


    Bootloader_UniflashConfig uniflashConfig;
    Bootloader_UniflashResponseHeader respHeader;

    gFlashAttrs = Flash_getAttrs(CONFIG_FLASH0);
    gFlashBuffSize = BOOTLOADER_UNIFLASH_MAX_FILE_SIZE;
    gFlashWriteOpcode = FLASH_OPCODE;

    if (status == SystemP_SUCCESS)
    {
        uniflashConfig.flashIndex = CONFIG_FLASH0;
        uniflashConfig.buf = gFileBuf;
        uniflashConfig.bufSize = 0; /* Actual fileSize will be parsed from the header */
        uniflashConfig.verifyBuf = gVerifyBuf;
        uniflashConfig.verifyBufSize = BOOTLOADER_UNIFLASH_VERIFY_BUF_MAX_SIZE;

        /* Process the flash commands and return a response */
        Bootloader_uniflashProcessFlashCommands(&uniflashConfig, &respHeader);
        gFlashStatus = respHeader.statusCode;
    }
    /* clear buffer to reset stale data if any */
    memset(gFileBuf, 0, BOOTLOADER_UNIFLASH_MAX_FILE_SIZE);
    Drivers_close();
    System_deinit();

    return 0;
}