/*
 *   Copyright (c) Texas Instruments Incorporated 2024
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
 *
 */

/**
 *  \file     sdl_soc_tog.c
 *
 *  \brief    This file contains the soc-specific implementation of the API's present in the
 *            device abstraction layer file of TOG.
 */

#include <stdint.h>
#include <stdbool.h>
#include <sdl/include/sdl_types.h>
#include <sdl/include/hw_types.h>
#include <sdl/dpl/sdl_dpl.h>
#include <sdl/stog/v0/soc/sdl_tog_soc.h>
#include <sdl/include/am263px/sdlr_intr_r5fss0_core0.h>

uint8_t SDL_TOG_interruptDone = 0u;
static pSDL_DPL_HwipHandle TOG_intrHandle;

int32_t SDL_TOG_getBaseaddr(SDL_TOG_Inst instance,
                            uint32_t *baseAddr)
{
    int32_t status = SDL_PASS;
    uint32_t size = 0;

    if (baseAddr == NULL)
    {
        status = SDL_EBADARGS;
    }
    else
    {
        *baseAddr = SDL_TOG_baseAddress[instance];
    }

    if (status == SDL_PASS)
    {
        *baseAddr = (uint32_t)SDL_DPL_addrTranslate((uint64_t)*baseAddr, size);
    }

    return (status);
}

int32_t SDL_TOG_registerInterrupt(void)
{
    int32_t status = SDL_PASS;
    SDL_DPL_HwipParams intrParams;

    status = SDL_DPL_disableInterrupt((int32_t)SDL_R5FSS0_CORE0_INTR_FSS_VBUSM_TIMEOUT);

    if (status == SDL_PASS)
    {
        intrParams.intNum = (int32_t)SDL_R5FSS0_CORE0_INTR_FSS_VBUSM_TIMEOUT;
        intrParams.callback = (pSDL_DPL_InterruptCallbackFunction)SDL_TOG_irqHandler;
        intrParams.callbackArg = (uintptr_t)SDL_TOG_INSTANCE;

        /* Register call back function for TOG Interrupt */
        status = SDL_DPL_registerInterrupt(&intrParams,
                                           &TOG_intrHandle);
    }

    if (status == SDL_PASS)
    {
        status = SDL_DPL_enableInterrupt((int32_t)SDL_R5FSS0_CORE0_INTR_FSS_VBUSM_TIMEOUT);
    }

    return (status);
}

void SDL_TOG_irqHandler(void)
{
    SDL_TOG_interruptDone = 1U;
    return;
}


