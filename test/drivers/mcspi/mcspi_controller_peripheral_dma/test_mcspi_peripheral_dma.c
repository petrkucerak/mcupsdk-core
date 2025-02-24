/*
 *  Copyright (C) 2021-23 Texas Instruments Incorporated
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

/*
 *  The application demonstrates MCSPI peripheral DMA operation by receiving
 *  a known data from the controller and then sending the same
 *  and finally compare the results.Ipc sync is used for syncronization
 *  between the cores.
 *  Please connect pins as described below on AM243X LP.
 *  Master SPI Pins                                 Peripheral SPI Pins
 *  -----------------                            ---------------------
 *  SPI0_CS1[Pin J6.6]     <----------------->     SPI3_CS0[Pin J2.6]
 *  SPI0_D0 [Pin J6.12]    <----------------->     SPI3_D1[Pin J2.14]
 *  SPI0_D1 [Pin J6.14]    <----------------->     SPI3_D0[Pin J2.12]
 *  SPI0_CLK [Pin J5.13]   <----------------->     SPI3_CLK[Pin J1.13]
 * 
 *  Please connect pins as described below on AM263x LP.
 *  MCU_SPI0_CS0(Pin 18)   ------------->   MCU_SPI1_CS0(Pin 58)
 *  MCU_SPI0_CLK(Pin 7)    ------------->   MCU_SPI1_CLK(Pin 47)
 *  MCU_SPI0_D0(Pin 55)    ------------->   MCU_SPI1_D1(Pin 14)
 *  MCU_SPI0_D1(Pin 54)    ------------->   MCU_SPI1_D0(Pin 15)
 *
 *  Please connect pins as described below on AM261x LP.
 *  MCU_SPI0_CS0(Pin 19) (B13)  ------------>  MCU_SPI2_CS1(Pin 59) A18  
 *  MCU_SPI0_CLK(Pin 7) (A13)   ------------>  MCU_SPI2_CLK(Pin 47) D17 
 *  MCU_SPI0_D0(Pin 15) (B12)   ------------>  MCU_SPI2_D1(Pin 54)  B18
 *  MCU_SPI0_D1(Pin 14) (C12)   ------------>  MCU_SPI2_D0(Pin 55)  A16
 * 
 *  Please connect pins as described below on AM261x SOM on HSEC Board.
 *  MCU_SPI0_CS0 (C11) -> HSEC_SPI1_CS0(J20-16)  ------------>  MCU_SPI3_CS0 (D7) -> HSEC_SPI1_CS0(J20-12)
 *  MCU_SPI0_CLK (A11) -> HSEC_SPI1_CLK(J20-15)  ------------>  MCU_SPI3_CLK (C8) -> HSEC_SPI1_CLK(J20-11)
 *  MCU_SPI0_D0  (C10) -> HSEC_SPI1_CS0(J20-14)  ------------>  MCU_SPI3_D0 (C7) -> HSEC_SPI1_CS0(J20-10)
 *  MCU_SPI0_D1  (B11) -> HSEC_SPI1_CS0(J20-13)  ------------>  MCU_SPI3_D1 (B7) -> HSEC_SPI1_CS0(J20-9)
 * 
 */

#include "string.h"
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/TaskP.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include <unity.h>

#define APP_MCSPI_MSGSIZE       (128U)
#define TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, transaction) \
    do { \
        if((SystemP_SUCCESS != (transferOK)) || \
                ((MCSPI_TRANSFER_COMPLETED != transaction.status) && \
                (MCSPI_TRANSFER_STARTED != transaction.status))) \
        { \
            DebugP_assert(FALSE); /* MCSPI TX/RX failed!! */ \
        } \
    } while(0) \

typedef struct MCSPI_PeripheralTestParams_s {
    MCSPI_ChConfig      *mcspiChConfigParams;
    MCSPI_OpenParams    mcspiOpenParams;
    uint32_t            testcaseId;
    uint32_t            dataSize;
} MCSPI_PeripheralTestParams;

uint8_t  gMcspiTxBuffer[APP_MCSPI_MSGSIZE] __attribute__((aligned(CacheP_CACHELINE_ALIGNMENT)));
uint8_t  gMcspiRxBuffer[APP_MCSPI_MSGSIZE] __attribute__((aligned(CacheP_CACHELINE_ALIGNMENT)));
uint32_t gMcspiTxBuffer32[APP_MCSPI_MSGSIZE] __attribute__((aligned(CacheP_CACHELINE_ALIGNMENT)));
uint32_t gMcspiRxBuffer32[APP_MCSPI_MSGSIZE] __attribute__((aligned(CacheP_CACHELINE_ALIGNMENT)));

extern MCSPI_Handle     gMcspiHandle[];
extern MCSPI_Config     gMcspiConfig[];
extern MCSPI_ChConfig  *gConfigMcspiChCfg[];

/* Semaphore to indicate Tx/Rx completion used in callback api's */
static SemaphoreP_Object gMcspiTransferDoneSem;

static void mcspi_peripheral_main(void *args);
static void test_mcspi_callback(MCSPI_Handle handle, MCSPI_Transaction *trans);
static int32_t mcspi_peripheral_transfer(uint32_t size);
static void test_mcspi_set_peripheral_params(MCSPI_PeripheralTestParams *testParams, uint32_t tcId);
static void test_mcspi_peripheral_transfer(void *args);

void test_mcspi_peripheral_dma_main(void *args)
{
    MCSPI_PeripheralTestParams  testParams;

    testParams.mcspiChConfigParams = gConfigMcspiChCfg[MCSPI_CHANNEL_0];
    Drivers_open();

    UNITY_BEGIN();

    test_mcspi_set_peripheral_params(&testParams, 2405);
    RUN_TEST(mcspi_peripheral_main,  2405, (void*)&testParams);
    test_mcspi_set_peripheral_params(&testParams, 2411);
    RUN_TEST(test_mcspi_peripheral_transfer,  2411, (void*)&testParams);

    UNITY_END();

    /* We dont close drivers to let the UART driver remain open and flush any pending messages to console
     * and also closing mcspi driver in controller */

    /* Drivers_close(); */

    return;
}

void mcspi_peripheral_main(void *args)
{
    int32_t             status = SystemP_SUCCESS, statusAll = SystemP_SUCCESS;
    uint32_t            size;

    DebugP_log("[MCSPI Peripheral] example started ...\r\n");

    size = APP_MCSPI_MSGSIZE/4;
    status = mcspi_peripheral_transfer(size);
    if (status != SystemP_SUCCESS)
    {
        statusAll = status;
        DebugP_log("[MCSPI Peripheral] test failed for size: %d!!\r\n", size);
    }

    size = APP_MCSPI_MSGSIZE/2;
    status = mcspi_peripheral_transfer(size);
    if (status != SystemP_SUCCESS)
    {
        statusAll = status;
        DebugP_log("[MCSPI Peripheral] test failed for size: %d!!\r\n", size);
    }

    size = APP_MCSPI_MSGSIZE;
    status = mcspi_peripheral_transfer(size);
    if (status != SystemP_SUCCESS)
    {
        statusAll = status;
        DebugP_log("[MCSPI Peripheral] test failed for size: %d!!\r\n", size);
    }

    /* wait for mcspi peripheral to be ready */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, statusAll);

}

static int32_t mcspi_peripheral_transfer(uint32_t size)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;

    DebugP_log("[MCSPI Peripheral] transfer test with size:%d ...\r\n", size);

    /* Memfill buffers */
    for(i = 0U; i < size; i++)
    {
        gMcspiTxBuffer[i] = i + 1U;
        gMcspiRxBuffer[i] = 0U;
    }

    /* indicate mcspi controller that peripheral is ready */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);

    /* Writeback buffer */
    CacheP_wb(&gMcspiTxBuffer[0U], sizeof(gMcspiTxBuffer), CacheP_TYPE_ALLD);
    CacheP_wb(&gMcspiRxBuffer[0U], sizeof(gMcspiRxBuffer), CacheP_TYPE_ALLD);

    /* Initiate transfer */
    spiTransaction.channel  = gConfigMcspiChCfg[0]->chNum;
    spiTransaction.dataSize  = 8;
    spiTransaction.csDisable = TRUE;
    spiTransaction.count    = size / (spiTransaction.dataSize/8);
    spiTransaction.txBuf    = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf    = (void *)gMcspiRxBuffer;
    spiTransaction.args     = NULL;
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    /* Invalidate cache */
    CacheP_inv(&gMcspiRxBuffer[0U], sizeof(gMcspiRxBuffer), CacheP_TYPE_ALLD);
    /* Compare data */
    for(i = 0U; i < size; i++)
    {
        if(gMcspiTxBuffer[i] != gMcspiRxBuffer[i])
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("[MCSPI Peripheral] Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    return status;
}

static void test_mcspi_callback(MCSPI_Handle handle, MCSPI_Transaction *trans)
{
    DebugP_assertNoLog(MCSPI_TRANSFER_COMPLETED == trans->status);
    SemaphoreP_post(&gMcspiTransferDoneSem);

    return;
}

static void test_mcspi_peripheral_transfer(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_PeripheralTestParams   *testParams = (MCSPI_PeripheralTestParams *)args;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    /* Memset Buffers */
    memset(&gMcspiTxBuffer32[0U], 0, APP_MCSPI_MSGSIZE);
    memset(&gMcspiRxBuffer32[0U], 0, APP_MCSPI_MSGSIZE);

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiOpenParams->mcspiDmaIndex = 0;
    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    /* Init TX buffer with known data and memset RX buffer */
    tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer32[0U];
    tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer32[0U];

    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers, TX data should be same as what done in Master application */
    for (i = 0U; i < APP_MCSPI_MSGSIZE; i++)
    {
        tempTxData = 0xDEADBABE + i;
        tempTxData &= (fifoBitMask);
        *tempTxPtr32++ = (uint32_t) (tempTxData);
        *tempRxPtr32++ = 0U;
    }

    /* wait for mcspi peripheral to be ready */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);

    /* Writeback buffer */
    CacheP_wb(&gMcspiTxBuffer32[0U], sizeof(gMcspiTxBuffer32), CacheP_TYPE_ALLD);
    CacheP_wb(&gMcspiRxBuffer32[0U], sizeof(gMcspiRxBuffer32), CacheP_TYPE_ALLD);

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.dataSize  = testParams->dataSize;
    spiTransaction.csDisable = TRUE;
    spiTransaction.count     = APP_MCSPI_MSGSIZE;
    spiTransaction.txBuf     = (void *)NULL;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer32;
    spiTransaction.args      = NULL;
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Invalidate cache */
    CacheP_inv(&gMcspiRxBuffer32[0U], sizeof(gMcspiRxBuffer32), CacheP_TYPE_ALLD);
    /* Compare data */
    /* This parameter value should be same as controller */
    for(i = 0U; i < (APP_MCSPI_MSGSIZE); i++)
    {
        if (gMcspiTxBuffer32[i] != gMcspiRxBuffer32[i])
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    /* wait for mcspi controller to be ready */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);

    return;
}

void setUp(void)
{
}

void tearDown(void)
{
}

static void test_mcspi_set_peripheral_params(MCSPI_PeripheralTestParams *testParams, uint32_t tcId)
{
    MCSPI_Config     *config = &gMcspiConfig[CONFIG_MCSPI0];
    MCSPI_Attrs      *attrParams = (MCSPI_Attrs *)config->attrs;
    MCSPI_OpenParams *openParams = &(testParams->mcspiOpenParams);
    MCSPI_ChConfig   *chConfigParams = testParams->mcspiChConfigParams;

    /* Default Attribute Parameters */
    attrParams->inputClkFreq       = 50000000U;
    attrParams->operMode           = MCSPI_OPER_MODE_DMA;
    attrParams->intrPriority       = 4U;
    attrParams->chMode             = MCSPI_CH_MODE_SINGLE;
    attrParams->pinMode            = MCSPI_PINMODE_4PIN;
    attrParams->initDelay          = MCSPI_INITDLY_0;

    /* Default Open Parameters */
    openParams->transferMode           = MCSPI_TRANSFER_MODE_BLOCKING;
    openParams->transferTimeout        = SystemP_WAIT_FOREVER;
    openParams->transferCallbackFxn    = NULL;
    openParams->msMode                 = MCSPI_MS_MODE_PERIPHERAL;

    /* Default Channel Config Parameters */
    chConfigParams->chNum              = MCSPI_CHANNEL_0;
    chConfigParams->frameFormat        = MCSPI_FF_POL0_PHA0;
    chConfigParams->bitRate            = 1000000;
    chConfigParams->csPolarity         = MCSPI_CS_POL_LOW;
    testParams->dataSize               = 8;
    chConfigParams->trMode             = MCSPI_TR_MODE_TX_RX;
    chConfigParams->inputSelect        = MCSPI_IS_D1;
    chConfigParams->dpe0               = MCSPI_DPE_DISABLE;
    chConfigParams->dpe1               = MCSPI_DPE_DISABLE;
    chConfigParams->slvCsSelect        = MCSPI_SLV_CS_SELECT_0;
    chConfigParams->startBitEnable     = FALSE;
    chConfigParams->startBitPolarity   = MCSPI_SB_POL_LOW;
    chConfigParams->csIdleTime         = MCSPI_TCS0_0_CLK;
    chConfigParams->defaultTxData      = 0x0U;
    switch (tcId)
    {
        case 2411:
        testParams->dataSize           = 32;
        chConfigParams->trMode         = MCSPI_TR_MODE_RX_ONLY;
        break;
    }

    return;
}
