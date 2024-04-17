/******************************************************************************
 * Copyright (c) 2024 Texas Instruments Incorporated - http://www.ti.com
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
 *****************************************************************************/

/**
 * @file   pmic_irq_tps6594x_priv.h
 *
 * @brief  The macro definitions, structures and function prototypes for
 *         configuring PMIC IRQ.
 */

#ifndef PMIC_IRQ_TPS65386X_PRIV_H_
#define PMIC_IRQ_TPS65386X_PRIV_H_

/* ========================================================================= */
/*                             Include Files                                 */
/* ========================================================================= */
#include "pmic_irq_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/*                             Macros & Typedefs                             */
/* ========================================================================= */

/*==========================================================================*/
/*                         Structures and Enums                             */
/*==========================================================================*/

/*==========================================================================*/
/*                         Function Declarations                            */
/*==========================================================================*/

void pmic_get_tps65386x_intrCfg(Pmic_IntrCfg_t ** pIntrCfg);

void pmic_get_tps65386x_intrGpioCfg(Pmic_GpioIntrTypeCfg_t ** pIntGpioCfg);

int32_t Pmic_tps65386x_irqGetL2Error(Pmic_CoreHandle_t * pPmicCoreHandle,
    uint16_t l1RegAddr, Pmic_IrqStatus_t * pErrStat);

void Pmic_tps65386x_reInitInterruptConfig(void);

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif /* PMIC_IRQ_TPS6594X_PRIV_H_ */