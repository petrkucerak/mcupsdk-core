/**
 * @file  dpl_interface.c
 *
 * @brief
 *  Example implementation of SDL DPL API.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2021-2023, Texas Instruments, Inc.
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

#include <stdio.h>
#include <sdl/dpl/sdl_dpl.h>
#include <kernel/dpl/HwiP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/AddrTranslateP.h>

#define NUM_HWI_OBJS  256
HwiP_Object gHwiObjects[NUM_HWI_OBJS];
uint32_t gHwiAvail[NUM_HWI_OBJS];

int32_t SDL_EXAMPLE_findAvailObj()
{
    int i = 0;

    for (i = 0; i < NUM_HWI_OBJS; i++)
    {
        if (gHwiAvail[i] == true)
        {
            gHwiAvail[i] = false;
            break;
        }
    }
    if (i == NUM_HWI_OBJS)
    {
        i = -1;
    }
    return i;
}

int32_t SDL_EXAMPLE_findObj(HwiP_Object *obj)
{
    int i = 0;

    for (i = 0; i < NUM_HWI_OBJS; i++)
    {
        if (&gHwiObjects[i] == obj)
        {
            break;
        }
    }
    if (i == NUM_HWI_OBJS)
    {
        i = -1;
    }
    return i;
}

pSDL_DPL_HwipHandle SDL_EXAMPLE_registerInterrupt(SDL_DPL_HwipParams *pParams)
{
    HwiP_Params hwipParams;
    HwiP_Params_init(&hwipParams);
    int32_t objNum;

    hwipParams.args = (void *)pParams->callbackArg;
    hwipParams.intNum = pParams->intNum;

    hwipParams.callback = pParams->callback;

    objNum = SDL_EXAMPLE_findAvailObj();

    if (objNum != -1)
    {
        HwiP_construct(&gHwiObjects[objNum], &hwipParams);
    }

    return &gHwiObjects[objNum];
}

int32_t SDL_EXAMPLE_deregisterInterrupt(pSDL_DPL_HwipHandle handle)
{
    int32_t objNum;
    objNum = SDL_EXAMPLE_findObj(handle);
    if (objNum != -1)
    {
        gHwiAvail[objNum] = true;
    }
    HwiP_destruct(handle);
    return SDL_PASS;
}

int32_t SDL_EXAMPLE_enableInterrupt(uint32_t intNum)
{

	HwiP_enableInt(intNum);
  return SDL_PASS;
}

int32_t SDL_EXAMPLE_disableInterrupt(uint32_t intNum)
{
	HwiP_disableInt(intNum);
  return SDL_PASS;
}

void* SDL_EXAMPLE_addrTranslate(uint64_t addr, uint32_t size)
{
    uint32_t transAddr = (uint32_t)(-1);

    transAddr = (uint32_t)AddrTranslateP_getLocalAddr(addr);

    return (void *)transAddr;
}

SDL_DPL_Interface dpl_interface =
{
    .enableInterrupt = (pSDL_DPL_InterruptFunction) SDL_EXAMPLE_enableInterrupt,
    .disableInterrupt = (pSDL_DPL_InterruptFunction) SDL_EXAMPLE_disableInterrupt,
    .registerInterrupt = (pSDL_DPL_RegisterFunction) SDL_EXAMPLE_registerInterrupt,
    .deregisterInterrupt = (pSDL_DPL_DeregisterFunction) SDL_EXAMPLE_deregisterInterrupt,
    .delay = (pSDL_DPL_DelayFunction) ClockP_sleep,
    .addrTranslate = (pSDL_DPL_AddrTranslateFunction) SDL_EXAMPLE_addrTranslate
};

int32_t SDL_EXAMPLE_dplInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;
    int32_t i;

    for (i = 0; i < NUM_HWI_OBJS; i++)
    {
        gHwiAvail[i] = true;
    }

    ret = SDL_DPL_init(&dpl_interface);

    return ret;
}
