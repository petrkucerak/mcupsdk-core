/*
 *   Copyright (c) Texas Instruments Incorporated 2022
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
 *  \file     sdl_mcrc_soc.c
 *
 *  \brief    This file contains the soc-specific implementation of the API's present in the
 *            device abstraction layer file of MCRC.
 */
 
#include <stdint.h>
#include <stdbool.h>
#include <sdl/include/sdl_types.h>
#include <sdl/include/hw_types.h>
#include <sdl/dpl/sdl_dpl.h>
#include <sdl/mcrc/v0/sdl_ip_mcrc.h>
#include <sdl/mcrc/v0/sdl_mcrc_hw.h>

/**
 *  Design: PROC_SDL-2101 
 */
int32_t SDL_MCRC_getBaseaddr(SDL_MCRC_InstType instance,
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
        if (instance == DSS_MCRC)
        {
            *baseAddr = (uint32_t)SDL_DSS_MCRC_U_BASE;
            size = 0x520UL;
        }
		else if (instance == MSS_MCRC)
		{
			*baseAddr = (uint32_t)SDL_MSS_MCRC_U_BASE;
            size = 0x520UL;
		}
        else
        {
            status = SDL_EBADARGS;
        }
    }

    if (status == SDL_PASS)
    {
        *baseAddr = (uint32_t)SDL_DPL_addrTranslate(*baseAddr, size);
    }

    return (status);
}

