/*
 *  Copyright (C) 2021-24 Texas Instruments Incorporated
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

/* This UT demonstrates the McSPI RX and TX operation configured
 * in different configurations and all possible MCSPI instances that can be
 * configured. MCSPI2 instance is muxed with UART, so it is not tested.
 * In case of AM243 LP we, have only 3 instances available.
 *
 * This example sends a known data in the TX mode of length APP_MCSPI_MSGSIZE
 * and then receives the same in RX mode. Internal pad level loopback mode
 * is enabled to receive data.
 *
 * When transfer is completed, TX and RX buffer data are compared.
 * If data is matched, test result is passed otherwise failed.
 */
#include "string.h"
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/TaskP.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include <unity.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <drivers/mcspi/v0/lld/mcspi_lld.h>

#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
#include <drivers/mcspi/v0/lld/dma/mcspi_dma.h>
#endif

/* Task Macros */
#define MCSPI_TASK_PRIORITY   (8U)
#define MCSPI_TASK_STACK_SIZE (4U * 1024U)

/* Number of Word count */
#define APP_MCSPI_MSGSIZE                   (100U)
#define APP_MCSPI_TXONLYMSGSIZE             (5U)
#define APP_MCSPI_TRANSFER_LOOPCOUNT        (5U)
#define APP_MCSPI_PERF_LOOP_ITER_CNT        (1000U)
#define APP_MCSPI_MAX_BITRATE_INDEX         (13U)

#if defined(SOC_AM263X) || defined (SOC_AM263PX) || defined (SOC_AM261X)

#define MCSPI0_BASE_ADDRESS             (CSL_MCSPI0_U_BASE)
#define MCSPI1_BASE_ADDRESS             (CSL_MCSPI1_U_BASE)
#define MCSPI2_BASE_ADDRESS             (CSL_MCSPI2_U_BASE)
#define MCSPI3_BASE_ADDRESS             (CSL_MCSPI3_U_BASE)
#if !(defined (SOC_AM261X))
#define MCSPI4_BASE_ADDRESS             (CSL_MCSPI4_U_BASE)
#endif

#define MCSPI0_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI0_INTR)
#define MCSPI1_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI1_INTR)
#define MCSPI2_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI2_INTR)
#define MCSPI3_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI3_INTR)
#if !(defined (SOC_AM261X))
#define MCSPI4_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI4_INTR)
#endif

#elif defined(SOC_AM64X)

#ifdef R5F_CORE
#define MCSPI0_BASE_ADDRESS             (CSL_MCSPI0_CFG_BASE)
#define MCSPI1_BASE_ADDRESS             (CSL_MCSPI1_CFG_BASE)
#define MCSPI2_BASE_ADDRESS             (CSL_MCSPI2_CFG_BASE)
#define MCSPI3_BASE_ADDRESS             (CSL_MCSPI3_CFG_BASE)
#define MCSPI4_BASE_ADDRESS             (CSL_MCSPI4_CFG_BASE)

#define MCSPI0_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI0_INTR_SPI_0)
#define MCSPI1_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI1_INTR_SPI_0)
#define MCSPI2_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI2_INTR_SPI_0)
#define MCSPI3_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI3_INTR_SPI_0)
#define MCSPI4_INT_NUM                  (CSLR_R5FSS0_CORE0_INTR_MCSPI4_INTR_SPI_0)
#endif

#ifdef A53_CORE
#define MCSPI0_BASE_ADDRESS             (CSL_MCSPI0_CFG_BASE)
#define MCSPI1_BASE_ADDRESS             (CSL_MCSPI1_CFG_BASE)
#define MCSPI2_BASE_ADDRESS             (CSL_MCSPI2_CFG_BASE)
#define MCSPI3_BASE_ADDRESS             (CSL_MCSPI3_CFG_BASE)
#define MCSPI4_BASE_ADDRESS             (CSL_MCSPI4_CFG_BASE)

#define MCSPI0_INT_NUM                  (CSLR_GICSS0_SPI_MCSPI0_INTR_SPI_0)
#define MCSPI1_INT_NUM                  (CSLR_GICSS0_SPI_MCSPI1_INTR_SPI_0)
#define MCSPI2_INT_NUM                  (CSLR_GICSS0_SPI_MCSPI2_INTR_SPI_0)
#define MCSPI3_INT_NUM                  (CSLR_GICSS0_SPI_MCSPI3_INTR_SPI_0)
#define MCSPI4_INT_NUM                  (CSLR_GICSS0_SPI_MCSPI4_INTR_SPI_0)
#endif

#elif defined(SOC_AM65X)

#define MCSPI0_BASE_ADDRESS             (CSL_MCSPI0_CFG_BASE)
#define MCSPI1_BASE_ADDRESS             (CSL_MCSPI1_CFG_BASE)
#define MCSPI2_BASE_ADDRESS             (CSL_MCSPI2_CFG_BASE)
#define MCSPI3_BASE_ADDRESS             (CSL_MCU_MCSPI0_CFG_BASE)
#define MCSPI4_BASE_ADDRESS             (CSL_MCU_MCSPI1_CFG_BASE)

#define MCSPI0_INT_NUM                  (168U)
#define MCSPI1_INT_NUM                  (169U)
#define MCSPI2_INT_NUM                  (170U)
#define MCSPI3_INT_NUM                  (20U)
#define MCSPI4_INT_NUM                  (21U)
/* define the unlock and lock values */
#define CTRLMMR_KICK_LOCK_VAL           (0x00000000U)
#define CTRLMMR_KICK0_UNLOCK_VAL        (0x68EF3490U)
#define CTRLMMR_KICK1_UNLOCK_VAL        (0xD172BC5AU)
#define MCU_MCSPI1_LINK_DISABLE         (0x01)

#else

#define MCSPI0_BASE_ADDRESS             (CSL_MCSPI0_CFG_BASE)
#define MCSPI1_BASE_ADDRESS             (CSL_MCSPI1_CFG_BASE)
#define MCSPI2_BASE_ADDRESS             (CSL_MCSPI2_CFG_BASE)
#define MCSPI3_BASE_ADDRESS             (CSL_MCSPI3_CFG_BASE)
#define MCSPI4_BASE_ADDRESS             (CSL_MCSPI4_CFG_BASE)

#define MCSPI0_INT_NUM                  (204U)
#define MCSPI1_INT_NUM                  (205U)
#define MCSPI2_INT_NUM                  (206U)
#define MCSPI3_INT_NUM                  (63U)
#define MCSPI4_INT_NUM                  (207U)
#endif

static uint32_t   gClkDividerTestListRampUp[] =
{
    0U,   1U,   2U,   3U,   4U,   5U,   6U,    7U,    8U,    9U,    10U,
    99U,  15U,  31U,  63U,  127U,  199U,  255U,  299U, 399U, 499U, 511U, 599U,
    699U, 799U, 899U, 999U, 1023U, 2047U, 3000U, 4095U
};

static uint32_t   gClkDividerTestListRampDown[] =
{
    4095U, 3000U, 2047U, 1023U, 999U, 899U, 799U,
    699U, 599U, 511U, 499U, 399U, 299U, 255U,
    199U, 127U, 99U, 63U, 31U, 15U, 10U, 9U,
    8U, 7U, 6U, 5U, 4U, 3U, 2U, 1U, 0U
};

#define SPI_TEST_NUM_CLK_LIST            (sizeof (gClkDividerTestListRampUp) / \
                                          sizeof (gClkDividerTestListRampUp[0U]))

typedef struct MCSPI_TestParams_s {
    MCSPI_ChConfig     *mcspiChConfigParams;
    MCSPI_OpenParams    mcspiOpenParams;
    uint32_t            transferLength;
    uint32_t            dataSize;
} MCSPI_TestParams;

extern MCSPI_Handle          gMcspiHandle[];
extern MCSPI_Config          gMcspiConfig[];
extern MCSPI_ChConfig       *gConfigMcspiChCfg[];

/* Semaphore to track end of rx_task and tx_task */
SemaphoreP_Object gMcspiTransferTaskDoneSemaphoreObj;
SemaphoreP_Object gMcspiTransferCancelTaskDoneSemaphoreObj;

uint8_t gMcspiTransferTaskStack[MCSPI_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gMcspiTransferTaskObject;

uint8_t gMcspiTransferCancelTaskStack[MCSPI_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gMcspiTransferCancelTaskObject;

uint32_t     gMcspiTxBuffer[APP_MCSPI_MSGSIZE];
uint32_t     gMcspiRxBuffer[APP_MCSPI_MSGSIZE];
uint32_t     gMcspiTxBuffer1[APP_MCSPI_MSGSIZE];
uint32_t     gMcspiRxBuffer1[APP_MCSPI_MSGSIZE];
uint32_t     gMcspiTxBuffer2[APP_MCSPI_MSGSIZE];
uint32_t     gMcspiRxBuffer2[APP_MCSPI_MSGSIZE];
uint16_t     gMcspiPerfTxBuffer[APP_MCSPI_TXONLYMSGSIZE];
uint32_t     gChEnableRegVal, gChDisableRegVal;
uint32_t     gCsAssertRegVal, gCsDeAssertRegVal;

#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
uint32_t     gMcspiTxBufferDma[APP_MCSPI_MSGSIZE] __attribute__((aligned(CacheP_CACHELINE_ALIGNMENT)));
uint32_t     gMcspiRxBufferDma[APP_MCSPI_MSGSIZE] __attribute__((aligned(CacheP_CACHELINE_ALIGNMENT)));
#endif

typedef struct mcspiUtPref_ {
uint64_t polled;
uint64_t interrupt;
uint64_t dma;
} mcspiUtPref;

mcspiUtPref gUtPerf[3];

/* Semaphore to indicate Tx/Rx completion used in callback api's */
static SemaphoreP_Object gMcspiTransferDoneSem;

static void test_mcspi_set_params(MCSPI_TestParams *testParams, uint32_t testCaseId);
void test_mcspi_loopback(void *args);
void test_mcspi_loopback_cs_disable(void *args);
void test_mcspi_loopback_simultaneous(void *args);
void test_mcspi_loopback_back2back(void *args);
void test_mcspi_loopback_multimaster(void *args);
void test_mcspi_performance_16bit(void *args);
void test_mcspi_loopback_timeout(void *args);
void test_mcspi_transfer_cancel(void *args);
void test_mcspi_transfer_cancel_transfer(void *args);
void test_mcspi_transfer_cancel_cancel(void *args);
void test_mcspi_loopback_performance(void *args);
void test_mcspi_callback(MCSPI_Handle handle, MCSPI_Transaction *trans);
static void mcspi_low_latency_transfer_16bit(uint32_t baseAddr,
                                            uint32_t chNum,
                                            uint16_t *txBuff,
                                            uint32_t length,
                                            uint32_t bufWidthShift);
void test_mcspi_loopback_bitrate(void *args);
void test_mcspi_loopback_turbo_mode(void *args);
void test_mcspi_loopback_multi_word_access(void *args);

#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
void test_mcspi_loopback_dma(void *args);
void test_mcspi_loopback_multimaster_dma(void *args);
#endif
#if defined(SOC_AM65X)
void test_mcspi_mcu_mcspi1_detach(void);
extern void MCSPI_intr_router_configInit(void);
extern void MCSPI_intr_router_configDeinit(void);
#endif

#define TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, transaction) \
    do { \
        if((SystemP_SUCCESS != (transferOK)) || \
                ((MCSPI_TRANSFER_COMPLETED != transaction.status) && \
                (MCSPI_TRANSFER_STARTED != transaction.status))) \
        { \
            DebugP_assert(FALSE); /* MCSPI TX/RX failed!! */ \
        } \
    } while(0) \

void test_main(void *args)
{
    MCSPI_TestParams  testParams;
    uint32_t          clkList;
    MCSPI_ChConfig   *chConfigParams;
    MCSPI_Config     *config;
    MCSPI_Attrs      *attrParams;

    testParams.mcspiChConfigParams = gConfigMcspiChCfg[MCSPI_CHANNEL_0];

    Drivers_open();
    Board_driversOpen();
#if defined(SOC_AM65X)
    MCSPI_intr_router_configInit();
    test_mcspi_mcu_mcspi1_detach();
#endif

    UNITY_BEGIN();

    test_mcspi_set_params(&testParams, 335);
    RUN_TEST(test_mcspi_loopback,  335, (void*)&testParams);
    test_mcspi_set_params(&testParams, 336);
    RUN_TEST(test_mcspi_loopback, 336, (void*)&testParams);
/* AM263X does not support MCU_SPI instance */
#if !(defined(SOC_AM263X) || defined (SOC_AM263PX) || defined (SOC_AM261X))
/* AM243 LP we, have only 2 instances available */
#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
    test_mcspi_set_params(&testParams, 970);
    RUN_TEST(test_mcspi_loopback,  970, (void*)&testParams);
    test_mcspi_set_params(&testParams, 971);
    RUN_TEST(test_mcspi_loopback,  971, (void*)&testParams);
#endif
#endif
#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
#if !defined(SOC_AM261X)
    test_mcspi_set_params(&testParams, 972);
    RUN_TEST(test_mcspi_loopback,  972, (void*)&testParams);
#endif
#endif
#if !defined(SOC_AM64X) && !defined(SOC_AM243X) && !defined(SOC_AM263X) && !defined (SOC_AM263PX) || !defined (SOC_AM261X)
    test_mcspi_set_params(&testParams, 973);
    RUN_TEST(test_mcspi_loopback,  973, (void*)&testParams);
#endif
    test_mcspi_set_params(&testParams, 974);
    RUN_TEST(test_mcspi_loopback,  974, (void*)&testParams);
#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
#if !defined(SOC_AM261X)
    test_mcspi_set_params(&testParams, 975);
    RUN_TEST(test_mcspi_loopback,  975, (void*)&testParams);
#endif
#endif
    test_mcspi_set_params(&testParams, 980);
    RUN_TEST(test_mcspi_loopback_performance,  980, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7351);
    RUN_TEST(test_mcspi_loopback_performance,  7351, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7352);
    RUN_TEST(test_mcspi_loopback_performance,  7352, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7353);
    RUN_TEST(test_mcspi_loopback_performance,  7353, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7354);
    RUN_TEST(test_mcspi_loopback_performance,  7354, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7355);
    RUN_TEST(test_mcspi_loopback_performance,  7355, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11485);
    RUN_TEST(test_mcspi_loopback_bitrate,  11485, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11416);
    RUN_TEST(test_mcspi_loopback,  11416, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11417);
    RUN_TEST(test_mcspi_loopback,  11417, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11418);
    RUN_TEST(test_mcspi_loopback,  11418, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11599);
    RUN_TEST(test_mcspi_loopback,  11599, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11600);
    RUN_TEST(test_mcspi_loopback,  11600, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11722);
    RUN_TEST(test_mcspi_loopback_cs_disable,  11722, (void*)&testParams);

#if !defined(SOC_AM65X)
    /* AM65x completes the first MCSPI_transfer() before the next/second MCSPI_transfer() initiates */
    test_mcspi_set_params(&testParams, 985);
    RUN_TEST(test_mcspi_loopback_back2back,  985, (void*)&testParams);
#endif
    test_mcspi_set_params(&testParams, 991);
    RUN_TEST(test_mcspi_loopback,  991, (void*)&testParams);
    test_mcspi_set_params(&testParams, 992);
    /* Change clock divider as per test list */
    chConfigParams = testParams.mcspiChConfigParams;
    config = &gMcspiConfig[CONFIG_MCSPI0];
    attrParams = (MCSPI_Attrs *)config->attrs;
    for (clkList = 0U; clkList < SPI_TEST_NUM_CLK_LIST; clkList++)
    {
        chConfigParams->bitRate = (attrParams->inputClkFreq / (gClkDividerTestListRampUp[clkList] + 1));
        RUN_TEST(test_mcspi_loopback,  992, (void*)&testParams);
    }
    test_mcspi_set_params(&testParams, 993);
    config = &gMcspiConfig[CONFIG_MCSPI0];
    attrParams = (MCSPI_Attrs *)config->attrs;
    for (clkList = 0U; clkList < SPI_TEST_NUM_CLK_LIST; clkList++)
    {
        gConfigMcspiChCfg[MCSPI_CHANNEL_0]->bitRate = (attrParams->inputClkFreq / (gClkDividerTestListRampDown[clkList] + 1));
        RUN_TEST(test_mcspi_loopback,  993, (void*)&testParams);
    }
    test_mcspi_set_params(&testParams, 994);
    RUN_TEST(test_mcspi_loopback,  994, (void*)&testParams);
    test_mcspi_set_params(&testParams, 995);
    RUN_TEST(test_mcspi_loopback,  995, (void*)&testParams);
    test_mcspi_set_params(&testParams, 996);
    RUN_TEST(test_mcspi_loopback,  996, (void*)&testParams);
    test_mcspi_set_params(&testParams, 997);
    RUN_TEST(test_mcspi_loopback,  997, (void*)&testParams);
    test_mcspi_set_params(&testParams, 998);
    RUN_TEST(test_mcspi_loopback,  998, (void*)&testParams);
    test_mcspi_set_params(&testParams, 999);
    RUN_TEST(test_mcspi_loopback,  999, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1000);
    RUN_TEST(test_mcspi_loopback,  1000, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1001);
    RUN_TEST(test_mcspi_loopback,  1001, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1002);
    RUN_TEST(test_mcspi_loopback,  1002, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1003);
    RUN_TEST(test_mcspi_loopback,  1003, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1004);
    RUN_TEST(test_mcspi_loopback,  1004, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1005);
    RUN_TEST(test_mcspi_loopback,  1005, (void*)&testParams);
    testParams.mcspiChConfigParams = gConfigMcspiChCfg[1];
    test_mcspi_set_params(&testParams, 1006);
    RUN_TEST(test_mcspi_loopback_multimaster,  1006, (void*)&testParams);
    testParams.mcspiChConfigParams = gConfigMcspiChCfg[0];
#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
    test_mcspi_set_params(&testParams, 1007);
    /* Change clock divider as per test list */
    config = &gMcspiConfig[CONFIG_MCSPI0];
    attrParams = (MCSPI_Attrs *)config->attrs;
    for (clkList = 0U; clkList < SPI_TEST_NUM_CLK_LIST; clkList++)
    {
        gConfigMcspiChCfg[MCSPI_CHANNEL_0]->bitRate = (attrParams->inputClkFreq / (gClkDividerTestListRampUp[clkList] + 1));
        RUN_TEST(test_mcspi_loopback,  1007, (void*)&testParams);
    }
#endif
    test_mcspi_set_params(&testParams, 1008);
    config = &gMcspiConfig[CONFIG_MCSPI0];
    attrParams = (MCSPI_Attrs *)config->attrs;
    for (clkList = 0U; clkList < SPI_TEST_NUM_CLK_LIST; clkList++)
    {
        gConfigMcspiChCfg[MCSPI_CHANNEL_0]->bitRate = (attrParams->inputClkFreq / (gClkDividerTestListRampDown[clkList] + 1));
        RUN_TEST(test_mcspi_loopback,  1008, (void*)&testParams);
    }
    test_mcspi_set_params(&testParams, 1011);
    RUN_TEST(test_mcspi_transfer_cancel, 1011, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1012);
    RUN_TEST(test_mcspi_transfer_cancel, 1012, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1014);
    RUN_TEST(test_mcspi_loopback_timeout, 1014, (void*)&testParams);
    test_mcspi_set_params(&testParams, 1565);
    RUN_TEST(test_mcspi_performance_16bit, 1565, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11537);
    RUN_TEST(test_mcspi_loopback_turbo_mode,  11537, (void*)&testParams);

#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
    test_mcspi_set_params(&testParams, 2394);
    RUN_TEST(test_mcspi_loopback_dma,  2394, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7356);
    RUN_TEST(test_mcspi_loopback_dma,  7356, (void*)&testParams);
    test_mcspi_set_params(&testParams, 7357);
    RUN_TEST(test_mcspi_loopback_dma,  7357, (void*)&testParams);
    test_mcspi_set_params(&testParams, 2397);
    RUN_TEST(test_mcspi_loopback_multimaster_dma,  2397, (void*)&testParams);
#endif
    test_mcspi_set_params(&testParams, 1009);
    RUN_TEST(test_mcspi_loopback_simultaneous, 1009, (void*)&testParams);
    test_mcspi_set_params(&testParams, 11538);
    testParams.transferLength = 128U;
    RUN_TEST(test_mcspi_loopback_multi_word_access,  11538, (void*)&testParams);

    /* Print Performance Numbers. */
    DebugP_log("\nMCSPI Performance Numbers Print Start\r\n\n");
    DebugP_log("Number of Words | Word Width (Bits)     | Polled mode Throughput / Transfer time  | Interrupt mode (Mbps) Throughput / Transfer time | Dma mode (Mbps) Throughput / Transfer time\r\n");
    DebugP_log("----------------|-----------------------|-------------------------------|-------------------------------|-------------------------------\r\n");
    for (uint32_t i =0; i<3; i++)
    {
        uint32_t dataWidth = 8*(1<<i);
        uint32_t dataLength = 400/(1<<i);
        DebugP_log(" %u\t\t| %02u\t\t\t| %5.2f Mbps / %5.2f us \t| %5.2f Mbps / %5.2f us \t| %5.2f Mbps / %5.2f us\r\n", dataLength, dataWidth,
                            (float)(dataLength * dataWidth)/((float)gUtPerf[i].polled / APP_MCSPI_PERF_LOOP_ITER_CNT),
                            ((float)gUtPerf[i].polled / APP_MCSPI_PERF_LOOP_ITER_CNT),
                            (float)(dataLength * dataWidth)/((float)gUtPerf[i].interrupt / APP_MCSPI_PERF_LOOP_ITER_CNT),
                            ((float)gUtPerf[i].interrupt / APP_MCSPI_PERF_LOOP_ITER_CNT),
                            (float)(dataLength * dataWidth)/((float)gUtPerf[i].dma / APP_MCSPI_PERF_LOOP_ITER_CNT),
                            ((float)gUtPerf[i].dma / APP_MCSPI_PERF_LOOP_ITER_CNT));
    }
    DebugP_log("\nMCSPI Performance Numbers Print End\r\n");

    UNITY_END();

    Board_driversClose();
    Drivers_close();

    return;
}

void test_mcspi_loopback(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;
    transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
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

    MCSPI_close(mcspiHandle);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_cs_disable(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer1[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer1[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = FALSE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;
    transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer1;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer1;
    spiTransaction.args      = NULL;
    transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    /* Compare data */
    uint8_t *tempTxPtr1, *tempRxPtr1;
    tempTxPtr1 = (uint8_t *) &gMcspiTxBuffer1[0U];
    tempRxPtr1 = (uint8_t *) &gMcspiRxBuffer1[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr1++ != *tempRxPtr1++)
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    MCSPI_close(mcspiHandle);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_performance(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i,j, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    uint64_t            startTimeInUSec, elapsedTimeInUsecs, totalTimeInUsecs = 0U;
    uint32_t            perf_offset;
    MCSPI_Config       *config = &gMcspiConfig[CONFIG_MCSPI0];
    MCSPI_Attrs        *attrParams = (MCSPI_Attrs *)config->attrs;

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    for(j = 0U; j < APP_MCSPI_PERF_LOOP_ITER_CNT; j++)
    {
        /* Initiate transfer */
        spiTransaction.channel  = testParams->mcspiChConfigParams->chNum;
        spiTransaction.count    = testParams->transferLength;
        spiTransaction.dataSize  = dataWidth;
        spiTransaction.csDisable = TRUE;
        spiTransaction.txBuf    = (void *)gMcspiTxBuffer;
        spiTransaction.rxBuf    = (void *)gMcspiRxBuffer;
        spiTransaction.args     = NULL;
        startTimeInUSec = ClockP_getTimeUsec();
        transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
        TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);
        if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
        {
            /* Wait for transfer completion */
            SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
        }
        elapsedTimeInUsecs = ClockP_getTimeUsec() - startTimeInUSec;
        totalTimeInUsecs += elapsedTimeInUsecs;

        /* Compare data */
        uint8_t *tempTxPtr, *tempRxPtr;
        tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
        for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
        {
            if(*tempTxPtr++ != *tempRxPtr++)
            {
                status = SystemP_FAILURE;   /* Data mismatch */
                DebugP_log("Data Mismatch at offset %d\r\n", i);
                break;
            }
        }
    }

    /* Store Performance value in global var. Performance numbers are printed at the end of UT. */
    if (dataWidth == 8) {
        perf_offset = 0;
    } else if (dataWidth == 16) {
        perf_offset = 1;
    } else if (dataWidth == 32) {
        perf_offset = 2;
    }
    if (attrParams->operMode == MCSPI_OPER_MODE_POLLED)
    {
        gUtPerf[perf_offset].polled = totalTimeInUsecs;
    }
    if (attrParams->operMode == MCSPI_OPER_MODE_INTERRUPT)
    {
        gUtPerf[perf_offset].interrupt = totalTimeInUsecs;
    }

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_back2back(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    DebugP_assert(transferOK != SystemP_SUCCESS);
    DebugP_assert(spiTransaction.status == MCSPI_TRANSFER_CANCELLED);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }
    /* Compare data */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_SUCCESS;   /* Data Match */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_multimaster(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK, chCnt;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    uint32_t            mcspiChDataSize[4] = { 8, 16, 32, 4};
    MCSPI_Config       *config;
    MCSPI_Attrs        *attrParams;

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    config = &gMcspiConfig[CONFIG_MCSPI1];
    attrParams = (MCSPI_Attrs *)config->attrs;

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI1]);

    attrParams->operMode = MCSPI_OPER_MODE_INTERRUPT;
    for(chCnt = 0U; chCnt < CONFIG_MCSPI1_NUM_CH; chCnt++)
    {
        if ((chCnt % 2) == 0U)
        {
            gConfigMcspi1ChCfg[chCnt].inputSelect = MCSPI_IS_D1;
            gConfigMcspi1ChCfg[chCnt].dpe0        = MCSPI_DPE_DISABLE;
            gConfigMcspi1ChCfg[chCnt].dpe1        = MCSPI_DPE_ENABLE;
            gConfigMcspi1ChCfg[chCnt].csPolarity  = MCSPI_CS_POL_HIGH;
        }
        else
        {
            gConfigMcspi1ChCfg[chCnt].inputSelect = MCSPI_IS_D0;
            gConfigMcspi1ChCfg[chCnt].dpe0        = MCSPI_DPE_ENABLE;
            gConfigMcspi1ChCfg[chCnt].dpe1        = MCSPI_DPE_DISABLE;
            gConfigMcspi1ChCfg[chCnt].csPolarity  = MCSPI_CS_POL_LOW;
        }
    }
    mcspiHandle = MCSPI_open(CONFIG_MCSPI1, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    for(chCnt = 0U; chCnt < CONFIG_MCSPI1_NUM_CH; chCnt++)
    {
        if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
        {
            status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
            DebugP_assert(SystemP_SUCCESS == status);
        }

        dataWidth = mcspiChDataSize[chCnt];
        if (dataWidth < 9U)
        {
            /* Init TX buffer with known data and memset RX buffer */
            tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
            tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
        }
        else if (dataWidth < 17U)
        {
            /* Init TX buffer with known data and memset RX buffer */
            tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
            tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
        }
        else
        {
            /* Init TX buffer with known data and memset RX buffer */
            tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
            tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
        }
        fifoBitMask = 0x0U;
        for (dataWidthIdx = 0U;
             dataWidthIdx < dataWidth; dataWidthIdx++)
        {
            fifoBitMask |= (1U << dataWidthIdx);
        }

        /* Memfill buffers */
        for (i = 0U; i < testParams->transferLength; i++)
        {
            tempTxData = 0xDEADBABE;
            tempTxData &= (fifoBitMask);
            if (dataWidth < 9U)
            {
                *tempTxPtr8++ = (uint8_t) (tempTxData);
                *tempRxPtr8++ = 0U;
            }
            else if (dataWidth < 17U)
            {
                *tempTxPtr16++ = (uint16_t) (tempTxData);
                *tempRxPtr16++ = 0U;
            }
            else
            {
                *tempTxPtr32++ = (uint32_t) (tempTxData);
                *tempRxPtr32++ = 0U;
            }
        }

        /* Initiate transfer */
        spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
        spiTransaction.dataSize  = dataWidth;
        spiTransaction.csDisable = TRUE;
        spiTransaction.count     = testParams->transferLength;
        spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
        spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
        spiTransaction.args      = NULL;
        transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
        TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

        if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
        {
            /* Wait for transfer completion */
            SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
        }

        /* Compare data */
        uint8_t *tempTxPtr, *tempRxPtr;
        tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
        for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
        {
            if(*tempTxPtr++ != *tempRxPtr++)
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

    }

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI1]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
void test_mcspi_loopback_multimaster_dma(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK, chCnt;
    MCSPI_Transaction   spiTransaction;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_Handle        mcspiHandle;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    /* Memset Buffers */
    memset(&gMcspiTxBufferDma[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBufferDma[0U]));
    memset(&gMcspiRxBufferDma[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBufferDma[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI3]);

    for(chCnt = 0U; chCnt < CONFIG_MCSPI3_NUM_CH; chCnt++)
    {
        if ((chCnt % 2) == 0U)
        {
            gConfigMcspi3ChCfg[chCnt].inputSelect = MCSPI_IS_D1;
            gConfigMcspi3ChCfg[chCnt].dpe0        = MCSPI_DPE_DISABLE;
            gConfigMcspi3ChCfg[chCnt].dpe1        = MCSPI_DPE_ENABLE;
        }
        else
        {
            gConfigMcspi3ChCfg[chCnt].inputSelect = MCSPI_IS_D0;
            gConfigMcspi3ChCfg[chCnt].dpe0        = MCSPI_DPE_ENABLE;
            gConfigMcspi3ChCfg[chCnt].dpe1        = MCSPI_DPE_DISABLE;
        }
    }
    mcspiHandle = MCSPI_open(CONFIG_MCSPI3, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    for(chCnt = 0U; chCnt < CONFIG_MCSPI3_NUM_CH; chCnt++)
    {
        if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
        {
            status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
            DebugP_assert(SystemP_SUCCESS == status);
        }

        dataWidth = 8U;
        if (dataWidth < 9U)
        {
            /* Init TX buffer with known data and memset RX buffer */
            tempTxPtr8 = (uint8_t *) &gMcspiTxBufferDma[0U];
            tempRxPtr8 = (uint8_t *) &gMcspiRxBufferDma[0U];
        }
        else if (dataWidth < 17U)
        {
            /* Init TX buffer with known data and memset RX buffer */
            tempTxPtr16 = (uint16_t *) &gMcspiTxBufferDma[0U];
            tempRxPtr16 = (uint16_t *) &gMcspiRxBufferDma[0U];
        }
        else
        {
            /* Init TX buffer with known data and memset RX buffer */
            tempTxPtr32 = (uint32_t *) &gMcspiTxBufferDma[0U];
            tempRxPtr32 = (uint32_t *) &gMcspiRxBufferDma[0U];
        }
        fifoBitMask = 0x0U;
        for (dataWidthIdx = 0U;
             dataWidthIdx < dataWidth; dataWidthIdx++)
        {
            fifoBitMask |= (1U << dataWidthIdx);
        }

        /* Memfill buffers */
        for (i = 0U; i < testParams->transferLength; i++)
        {
            tempTxData = 0xDEADBABE;
            tempTxData &= (fifoBitMask);
            if (dataWidth < 9U)
            {
                *tempTxPtr8++ = (uint8_t) (tempTxData);
                *tempRxPtr8++ = 0U;
            }
            else if (dataWidth < 17U)
            {
                *tempTxPtr16++ = (uint16_t) (tempTxData);
                *tempRxPtr16++ = 0U;
            }
            else
            {
                *tempTxPtr32++ = (uint32_t) (tempTxData);
                *tempRxPtr32++ = 0U;
            }
        }

        /* Writeback buffer */
        CacheP_wb(&gMcspiTxBufferDma[0U], sizeof(gMcspiTxBufferDma), CacheP_TYPE_ALLD);
        CacheP_wb(&gMcspiRxBufferDma[0U], sizeof(gMcspiRxBufferDma), CacheP_TYPE_ALLD);

        /* Initiate transfer */
        spiTransaction.channel   = gConfigMcspi3ChCfg[chCnt].chNum;
        spiTransaction.dataSize  = dataWidth;
        spiTransaction.csDisable = TRUE;
        spiTransaction.count     = APP_MCSPI_MSGSIZE / (dataWidth / 8);
        spiTransaction.txBuf     = (void *)gMcspiTxBufferDma;
        spiTransaction.rxBuf     = (void *)gMcspiRxBufferDma;
        spiTransaction.args      = NULL;
        transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
        TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

        if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
        {
            /* Wait for transfer completion */
            SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
        }

        /* Invalidate cache */
        CacheP_inv(&gMcspiRxBufferDma[0U], sizeof(gMcspiRxBufferDma), CacheP_TYPE_ALLD);
        /* Compare data */
        uint8_t *tempTxPtr, *tempRxPtr;
        tempTxPtr = (uint8_t *) &gMcspiTxBufferDma[0U];
        tempRxPtr = (uint8_t *) &gMcspiRxBufferDma[0U];
        for(i = 0U; i < (APP_MCSPI_MSGSIZE); i++)
        {
            if(*tempTxPtr++ != *tempRxPtr++)
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

    }

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI3]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_dma(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, j, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    MCSPI_Config       *config;
    MCSPI_Attrs        *attrParams;
    MCSPI_Handle        mcspiHandle;
    uint64_t            startTimeInUSec, elapsedTimeInUsecs, totalTimeInUsecs = 0U;
    uint32_t            perf_offset;

    /* Memset Buffers */
    memset(&gMcspiTxBufferDma[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBufferDma[0U]));
    memset(&gMcspiRxBufferDma[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBufferDma[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI3]);

    config = &gMcspiConfig[CONFIG_MCSPI3];
    attrParams = (MCSPI_Attrs *)config->attrs;
    attrParams->operMode                    = MCSPI_OPER_MODE_DMA;
    mcspiOpenParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
    mcspiOpenParams->transferCallbackFxn    = test_mcspi_callback;
    mcspiOpenParams->mcspiDmaIndex          = 0;
    mcspiHandle = MCSPI_open(CONFIG_MCSPI3, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBufferDma[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBufferDma[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBufferDma[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBufferDma[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBufferDma[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBufferDma[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    /* Writeback buffer */
    CacheP_wb(&gMcspiTxBufferDma[0U], sizeof(gMcspiTxBufferDma), CacheP_TYPE_ALLD);
    CacheP_wb(&gMcspiRxBufferDma[0U], sizeof(gMcspiRxBufferDma), CacheP_TYPE_ALLD);
    for(j = 0U; j < APP_MCSPI_PERF_LOOP_ITER_CNT; j++)
    {
        /* Initiate transfer */
        spiTransaction.channel  = gConfigMcspi3ChCfg[0U].chNum;
        spiTransaction.count    = testParams->transferLength;
        spiTransaction.dataSize  = dataWidth;
        spiTransaction.csDisable = TRUE;
        spiTransaction.txBuf    = (void *)gMcspiTxBufferDma;
        spiTransaction.rxBuf    = (void *)gMcspiRxBufferDma;
        spiTransaction.args     = NULL;
        startTimeInUSec = ClockP_getTimeUsec();
        transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI3], &spiTransaction);
        TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

        if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
        {
            /* Wait for transfer completion */
            SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
        }
        elapsedTimeInUsecs = ClockP_getTimeUsec() - startTimeInUSec;
        totalTimeInUsecs += elapsedTimeInUsecs;

        /* Invalidate cache */
        CacheP_inv(&gMcspiRxBufferDma[0U], sizeof(gMcspiRxBufferDma), CacheP_TYPE_ALLD);

        /* Compare data */
        uint8_t *tempTxPtr, *tempRxPtr;
        tempTxPtr = (uint8_t *) &gMcspiTxBufferDma[0U];
        tempRxPtr = (uint8_t *) &gMcspiRxBufferDma[0U];
        for(i = 0U; i < (APP_MCSPI_MSGSIZE); i++)
        {
            if(*tempTxPtr++ != *tempRxPtr++)
            {
                status = SystemP_FAILURE;   /* Data mismatch */
                DebugP_log("Data Mismatch at offset %d\r\n", i);
                break;
            }
        }
    }

    /* Store Performance value in global var. Performance numbers are printed at the end of UT. */
    if (dataWidth == 8) {
        perf_offset = 0;
    } else if (dataWidth == 16) {
        perf_offset = 1;
    } else{
        perf_offset = 2;
    }
    gUtPerf[perf_offset].dma = totalTimeInUsecs;

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI3]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

#endif

void test_mcspi_loopback_timeout(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    MCSPI_ChConfig     *chConfig        = testParams->mcspiChConfigParams;

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel  = chConfig->chNum;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.count    = testParams->transferLength;
    spiTransaction.txBuf    = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf    = (void *)gMcspiRxBuffer;
    spiTransaction.args     = NULL;
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    DebugP_assert(spiTransaction.status == MCSPI_TRANSFER_TIMEOUT);
    DebugP_assert(transferOK == SystemP_FAILURE);

    /* Compare data and should mismatch as timeout occurred */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_SUCCESS;
            break;
        }
    }

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_performance_16bit(void *args)
{
    uint32_t            i, j;
    uint32_t            dataLength, dataWidth, bitRate, bufWidthShift;
    uint32_t            baseAddr, chNum, dataSize;
    uint32_t            chCtrlRegVal, chConfRegVal;
    uint64_t            startTimeInUSec, elapsedTimeInUsecs;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    /* update data size to 16 and bitrate to 12MHZ */
    dataSize = 16U;
    gConfigMcspi0ChCfg[0U].bitRate  = 12500000U;
    gConfigMcspi0ChCfg[0U].trMode   = MCSPI_TR_MODE_TX_ONLY;

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    DebugP_log("[MCSPI] Performance 16bit Example Started...\r\n\n");

    /* Memfill buffers */
    for(i = 0U; i < APP_MCSPI_TXONLYMSGSIZE; i++)
    {
        gMcspiPerfTxBuffer[i] = i;
    }

    /* Initialize SPI Channel Number */
    chNum  = gConfigMcspi0ChCfg[0U].chNum;

    /* Get SPI Channel Info */
    baseAddr = MCSPI_getBaseAddr(gMcspiHandle[CONFIG_MCSPI0]);
    DebugP_assert(baseAddr != 0U); /* MCSPI baseAddr Invalid!! */

    /* Set dataWidth */
    MCSPI_setDataWidth(baseAddr, chNum, dataSize);
    /* Enable the transmitter FIFO of McSPI peripheral. */
    MCSPI_enableTxFIFO(baseAddr, chNum, MCSPI_TX_FIFO_ENABLE);

    /* Disable the receiver FIFO of McSPI peripheral for Tx only mode. */
    MCSPI_enableRxFIFO(baseAddr, chNum, MCSPI_RX_FIFO_DISABLE);

    /*
     * Channel Control and config registers are updated after Open/Reconfigure.
     * Channel enable/disable and CS assert/deassert require updation of bits in
     * control and config registers. Also these registers will not be updated
     * during data transfer. So reg read modify write operations can be updated
     * to write only operations.
     * Store ch enable/disable reg val and cs assert/deassert reg vals.
     */
    chCtrlRegVal     = MCSPI_readChCtrlReg(baseAddr, chNum);
    gChEnableRegVal  = chCtrlRegVal | CSL_MCSPI_CH0CTRL_EN_MASK;
    gChDisableRegVal = chCtrlRegVal & (~CSL_MCSPI_CH0CTRL_EN_MASK);

    chConfRegVal      = MCSPI_readChConf(baseAddr, chNum);
    gCsAssertRegVal   = chConfRegVal | CSL_MCSPI_CH0CONF_FORCE_MASK;
    gCsDeAssertRegVal = chConfRegVal & (~CSL_MCSPI_CH0CONF_FORCE_MASK);

    /*  Calculate buffer width shift value.
     *  When dataWidth <= 8,           bufWidth = uint8_t  (1 byte - 0 shift)
     *  When dataWidth > 8  && <= 16,  bufWidth = uint16_t (2 bytes - 1 shift)
     *  When dataWidth > 16 && <= 32,  bufWidth = uint32_t (4 bytes - 2 shift)
     */
    dataWidth  = dataSize;
    bufWidthShift = MCSPI_getBufWidthShift(dataWidth);
    dataLength = APP_MCSPI_TXONLYMSGSIZE;
    bitRate    = gConfigMcspi0ChCfg[0U].bitRate;

    /* Initiate transfer */
    startTimeInUSec = ClockP_getTimeUsec();
    for(j = 0U; j < APP_MCSPI_TRANSFER_LOOPCOUNT; j++)
    {
        mcspi_low_latency_transfer_16bit(baseAddr, chNum,
                                   &gMcspiPerfTxBuffer[0], dataLength, bufWidthShift);
    }
    elapsedTimeInUsecs = ClockP_getTimeUsec() - startTimeInUSec;

    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("McSPI Clock %d Hz\r\n", bitRate);
    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("Data Width \tData Length \tTransfer Time (micro sec)\r\n");
    DebugP_log("%u\t\t%u\t\t%5.2f\r\n", dataWidth, dataLength,
                        (float)elapsedTimeInUsecs / APP_MCSPI_TRANSFER_LOOPCOUNT);
    DebugP_log("----------------------------------------------------------\r\n\n");

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    return;
}

void test_mcspi_loopback_simultaneous(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction, spiTransaction1;
    MCSPI_Handle        mcspiHandle, mcspiHandle1;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    uint32_t            dataWidth1, bufWidthShift, bufWidthShift1;
    MCSPI_TestParams   testParams1;
    MCSPI_OpenParams   *mcspiOpenParams     = &(testParams->mcspiOpenParams);
    MCSPI_ChConfig     *mcspiChConfigParams = testParams->mcspiChConfigParams;
    MCSPI_OpenParams   *mcspiOpenParams1    = &(testParams1.mcspiOpenParams);
    MCSPI_Config       *config1     = &gMcspiConfig[CONFIG_MCSPI1];
    MCSPI_Attrs        *attrParams1 = (MCSPI_Attrs *)config1->attrs;
    uint8_t            *tempTxPtr, *tempRxPtr;

    testParams->mcspiChConfigParams = gConfigMcspiChCfg[0];
    testParams1.mcspiChConfigParams = gConfigMcspiChCfg[1];
    mcspiOpenParams->transferMode   = MCSPI_TRANSFER_MODE_BLOCKING;

#if (CONFIG_MCSPI_NUM_INSTANCES > 2U)
    MCSPI_Transaction  spiTransaction2;
    MCSPI_Handle       mcspiHandle2;
    MCSPI_Config      *config2     = &gMcspiConfig[CONFIG_MCSPI2];
    MCSPI_Attrs       *attrParams2 = (MCSPI_Attrs *)config2->attrs;
    MCSPI_TestParams   testParams2;
    MCSPI_OpenParams   *mcspiOpenParams2    = &(testParams2.mcspiOpenParams);
#if !(defined (SOC_AM261X))
    attrParams2->baseAddr           = MCSPI4_BASE_ADDRESS;
    attrParams2->intrNum            = MCSPI4_INT_NUM;
#endif
    attrParams2->operMode           = MCSPI_OPER_MODE_POLLED;
    testParams2.mcspiChConfigParams = gConfigMcspiChCfg[2];
    test_mcspi_set_params(&testParams2, 1009);
    mcspiOpenParams2->transferMode = MCSPI_TRANSFER_MODE_BLOCKING;
#endif
    /* Instance 1 Init params */
    test_mcspi_set_params(&testParams1, 1009);

#if (CONFIG_MCSPI_NUM_INSTANCES > 2U)
    attrParams1->baseAddr           = MCSPI1_BASE_ADDRESS;
    attrParams1->intrNum            = MCSPI1_INT_NUM;
#else /* LP Case */
    attrParams1->baseAddr           = MCSPI3_BASE_ADDRESS;
    attrParams1->intrNum            = MCSPI3_INT_NUM;
#endif
    attrParams1->operMode                                = MCSPI_OPER_MODE_INTERRUPT;
    attrParams1->intrPriority                            = 4U;
    testParams1.mcspiChConfigParams->bitRate            = 12500000;
    testParams1.mcspiChConfigParams->csPolarity         = MCSPI_CS_POL_HIGH;
    testParams1.dataSize                                = 16;

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));
    /* Memset Buffers */
    memset(&gMcspiTxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer1[0U]));
    memset(&gMcspiRxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer1[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);
    MCSPI_close(gMcspiHandle[CONFIG_MCSPI1]);

#if (CONFIG_MCSPI_NUM_INSTANCES > 2U)
/* Memset Buffers */
    memset(&gMcspiTxBuffer2[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer2[0U]));
    memset(&gMcspiRxBuffer2[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer2[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI2]);
#endif
    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);
    mcspiHandle1 = MCSPI_open(CONFIG_MCSPI1, mcspiOpenParams1);
    TEST_ASSERT_NOT_NULL(mcspiHandle1);
#if (CONFIG_MCSPI_NUM_INSTANCES > 2U)
    mcspiHandle2 = MCSPI_open(CONFIG_MCSPI2, mcspiOpenParams2);
    TEST_ASSERT_NOT_NULL(mcspiHandle2);
#endif
    if(mcspiOpenParams1->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    bufWidthShift = MCSPI_getBufWidthShift(dataWidth);
    if (dataWidth <= 32U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }

    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U; dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        *tempTxPtr32++ = (uint32_t) (tempTxData);
        *tempRxPtr32++ = 0U;
    }

    dataWidth1 = testParams1.dataSize;
    bufWidthShift1 = MCSPI_getBufWidthShift(dataWidth1);
    if (dataWidth1 <= 16U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer1[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer1[0U];
    }

    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U; dataWidthIdx < dataWidth1; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < (APP_MCSPI_MSGSIZE * (sizeof(gMcspiTxBuffer1[0U]) / (1 << bufWidthShift1))); i++)
    {
        tempTxData = 0xBEDEEFAD;
        tempTxData &= (fifoBitMask);
        *tempTxPtr16++ = (uint16_t) (tempTxData);
        *tempRxPtr16++ = 0U;
    }

    /* Initiate transfer */
    spiTransaction1.channel  = testParams1.mcspiChConfigParams->chNum;
    spiTransaction1.dataSize  = testParams1.dataSize;
    spiTransaction1.csDisable = TRUE;
    spiTransaction1.count    = (APP_MCSPI_MSGSIZE * (sizeof(gMcspiTxBuffer1[0U]) / (1 << bufWidthShift1)));
    spiTransaction1.txBuf    = (void *)gMcspiTxBuffer1;
    spiTransaction1.rxBuf    = (void *)gMcspiRxBuffer1;
    spiTransaction1.args     = NULL;
    transferOK = MCSPI_transfer(mcspiHandle1, &spiTransaction1);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction1);

    /* Initiate transfer */
    spiTransaction.channel   = mcspiChConfigParams->chNum;
    spiTransaction.count     = (APP_MCSPI_MSGSIZE * (sizeof(gMcspiTxBuffer[0U]) / (1 << bufWidthShift)));
    spiTransaction.dataSize  = testParams->dataSize;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf    = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf    = (void *)gMcspiRxBuffer;
    spiTransaction.args     = NULL;
    transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

#if (CONFIG_MCSPI_NUM_INSTANCES > 2U)
    /* Initiate transfer */
    spiTransaction2.channel  = testParams2.mcspiChConfigParams->chNum;
    spiTransaction2.dataSize  = testParams2.dataSize;
    spiTransaction2.csDisable = TRUE;
    spiTransaction2.count    = (APP_MCSPI_MSGSIZE * (sizeof(gMcspiTxBuffer2[0U]) / (1 << bufWidthShift)));
    spiTransaction2.txBuf    = (void *)gMcspiTxBuffer2;
    spiTransaction2.rxBuf    = (void *)gMcspiRxBuffer2;
    spiTransaction2.args     = NULL;
    transferOK = MCSPI_transfer(mcspiHandle2, &spiTransaction2);
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction2);

    /* Compare data */
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer2[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer2[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }
#endif

    if(mcspiOpenParams1->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
         if(*tempTxPtr++ != *tempRxPtr++)
         {
             status = SystemP_FAILURE;   /* Data mismatch */
             DebugP_log("Data Mismatch at offset %d\r\n", i);
             break;
         }
    }

    /* Compare data */
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer1[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer1[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
       if(*tempTxPtr++ != *tempRxPtr++)
       {
           status = SystemP_FAILURE;   /* Data mismatch */
           DebugP_log("Data Mismatch at offset %d\r\n", i);
           break;
       }
    }

    if(mcspiOpenParams1->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));
    /* Memset Buffers */
    memset(&gMcspiTxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer1[0U]));
    memset(&gMcspiRxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer1[0U]));

#if (CONFIG_MCSPI_NUM_INSTANCES > 2U)
    /* Memset Buffers */
    memset(&gMcspiTxBuffer2[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer2[0U]));
    memset(&gMcspiRxBuffer2[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer2[0U]));
    MCSPI_close(gMcspiHandle[CONFIG_MCSPI2]);
#endif

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);
    MCSPI_close(gMcspiHandle[CONFIG_MCSPI1]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_transfer_cancel(void *args)
{
    int32_t status = SystemP_SUCCESS;
    MCSPI_Handle        mcspiHandle;
    TaskP_Params transferTaskParms, transferCancelTaskParms;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    status = SemaphoreP_constructCounting(&gMcspiTransferTaskDoneSemaphoreObj, 0, 2);
    DebugP_assert(status == SystemP_SUCCESS);
    status = SemaphoreP_constructCounting(&gMcspiTransferCancelTaskDoneSemaphoreObj, 0, 2);
    DebugP_assert(status == SystemP_SUCCESS);

    TaskP_Params_init(&transferTaskParms);
    transferTaskParms.name = "MCSPI Transfer Task";
    transferTaskParms.stackSize = MCSPI_TASK_STACK_SIZE;
    transferTaskParms.stack = gMcspiTransferTaskStack;
    transferTaskParms.priority = MCSPI_TASK_PRIORITY;
    transferTaskParms.args = testParams;
    transferTaskParms.taskMain = test_mcspi_transfer_cancel_transfer;
    status = TaskP_construct(&gMcspiTransferTaskObject, &transferTaskParms);
    DebugP_assert(status == SystemP_SUCCESS);

    ClockP_usleep(1000);

    TaskP_Params_init(&transferCancelTaskParms);
    transferCancelTaskParms.name = "MCSPI Transfer Cancel Task";
    transferCancelTaskParms.stackSize = MCSPI_TASK_STACK_SIZE;
    transferCancelTaskParms.stack = gMcspiTransferCancelTaskStack;
    transferCancelTaskParms.priority = MCSPI_TASK_PRIORITY;
    transferCancelTaskParms.args = testParams;
    transferCancelTaskParms.taskMain = test_mcspi_transfer_cancel_cancel;
    status = TaskP_construct(&gMcspiTransferCancelTaskObject, &transferCancelTaskParms);
    DebugP_assert(status == SystemP_SUCCESS);

    SemaphoreP_pend(&gMcspiTransferTaskDoneSemaphoreObj, SystemP_WAIT_FOREVER);
    SemaphoreP_pend(&gMcspiTransferCancelTaskDoneSemaphoreObj, SystemP_WAIT_FOREVER);

    TaskP_destruct(&gMcspiTransferTaskObject);
    TaskP_destruct(&gMcspiTransferCancelTaskObject);
    ClockP_usleep(1000);
    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
}

void test_mcspi_transfer_cancel_transfer(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    MCSPI_ChConfig     *chConfig   = gConfigMcspiChCfg[MCSPI_CHANNEL_0];

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }

    /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xABCDBADC;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel  = chConfig->chNum;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.count    = testParams->transferLength;
    spiTransaction.txBuf    = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf    = (void *)gMcspiRxBuffer;
    spiTransaction.args     = NULL;
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_BLOCKING)
    {
        DebugP_assert(spiTransaction.status == MCSPI_TRANSFER_CANCELLED);
    }
    DebugP_assert(transferOK == SystemP_SUCCESS);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
        DebugP_assert(spiTransaction.status == MCSPI_TRANSFER_CANCELLED);
    }

    /* Compare data and should be mismatch as it is cancelled in another task */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_SUCCESS;   /* Data mismatch */
            break;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel   = chConfig->chNum;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;
    transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_BLOCKING)
    {
        DebugP_assert(spiTransaction.status == MCSPI_TRANSFER_COMPLETED);
    }
    DebugP_assert(transferOK == SystemP_SUCCESS);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
        DebugP_assert(spiTransaction.status == MCSPI_TRANSFER_COMPLETED);
    }

    /* Compare data and should be mismatch as it is cancelled in another task */
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_assert(FALSE);
            break;
        }
    }

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        SemaphoreP_destruct(&gMcspiTransferDoneSem);
    }

    SemaphoreP_post(&gMcspiTransferTaskDoneSemaphoreObj);
    while(1)
    {
        /* Yield to the main task which deletes this task. */
        TaskP_yield();
    };

    return;
}

void test_mcspi_transfer_cancel_cancel(void *args)
{
    int32_t transferOK;

    transferOK = MCSPI_transferCancel(gMcspiHandle[CONFIG_MCSPI0]);
    DebugP_assert(transferOK == SystemP_SUCCESS);

    SemaphoreP_post(&gMcspiTransferCancelTaskDoneSemaphoreObj);
    while(1)
    {
        /* Yield to the main task which deletes this task. */
        TaskP_yield();
    };
}

void test_mcspi_callback(MCSPI_Handle handle, MCSPI_Transaction *trans)
{
    DebugP_assertNoLog(MCSPI_TRANSFER_COMPLETED == trans->status);
    SemaphoreP_post(&gMcspiTransferDoneSem);

    return;
}

void test_mcspi_callback_cancel(MCSPI_Handle handle, MCSPI_Transaction *trans)
{
    SemaphoreP_post(&gMcspiTransferDoneSem);
    return;
}

void setUp(void)
{
}

void tearDown(void)
{
}

static void mcspi_low_latency_transfer_16bit(uint32_t baseAddr,
                                            uint32_t chNum,
                                            uint16_t *txBuff,
                                            uint32_t length,
                                            uint32_t bufWidthShift)
{
    /* Effective FIFO depth in bytes(64/32/16) depending on datawidth */
    uint32_t effTxFifoDepth = MCSPI_FIFO_LENGTH >> bufWidthShift;
    uint32_t i, numWordsWritten = 0U, transferLength = length;

    /* Enable the McSPI channel for communication.*/
    /* Updated for write only operation. */
    MCSPI_writeChCtrlReg(baseAddr, chNum, gChEnableRegVal);

    /* SPIEN line is forced to low state.*/
    /* Updated for write only operation. */
    MCSPI_writeChConfReg(baseAddr, chNum, gCsAssertRegVal);

    while (transferLength != 0)
    {
        /* Write Effective TX FIFO depth */
        if (transferLength >= effTxFifoDepth)
        {
            transferLength = effTxFifoDepth;
        }
        while (0 == (MCSPI_readChStatusReg(baseAddr, chNum) &
                        CSL_MCSPI_CH0STAT_TXFFE_MASK))
        {
            /* Wait for Tx FIFO to be empty before writing the data. */
        }
        /* Write the data in Tx FIFO. */
        for (i = 0; i < transferLength; i++)
        {
            MCSPI_writeTxDataReg(baseAddr, (uint16_t) (*txBuff++), chNum);
        }
        numWordsWritten  += transferLength;
        transferLength    = length - numWordsWritten;
    }

    while (0 == (MCSPI_readChStatusReg(baseAddr, chNum) &
                    CSL_MCSPI_CH0STAT_TXFFE_MASK))
    {
        /* Wait fot Tx FIFO to be empty for the last set of data. */
    }
    while (0 == (MCSPI_readChStatusReg(baseAddr, chNum) &
                    CSL_MCSPI_CH0STAT_EOT_MASK))
    {
        /* Tx FIFO Empty is triggered when last word from FIFO is written to
           internal shift register. SO wait for the end of transfer of last word.
           The EOT gets set after every word when the transfer from shift
           register is complete and is reset when the transmission starts.
           So FIFO empty check is required to make sure the data in FIFO is
           sent out then wait for EOT for the last word. */
    }

    /* Force SPIEN line to the inactive state.*/
    /* Updated for write only operation. */
    MCSPI_writeChConfReg(baseAddr, chNum, gCsDeAssertRegVal);

    /* Disable the McSPI channel.*/
    /* Updated for write only operation. */
    MCSPI_writeChCtrlReg(baseAddr, chNum, gChDisableRegVal);
}

void test_mcspi_loopback_bitrate(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK, chCnt;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    uint32_t            mcspiChDataSize[4] = { 8, 16, 32, 4};
    MCSPI_Config       *config;
    MCSPI_Attrs        *attrParams;

    uint32_t bitRate[APP_MCSPI_MAX_BITRATE_INDEX] =
    {
        50000000, 25000000, 12500000, 6250000, 3125000, 1562500,
        781250, 390625, 195000, 97700, 48800, 24400, 12200
    };

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));

    config = &gMcspiConfig[CONFIG_MCSPI1];
    attrParams = (MCSPI_Attrs *)config->attrs;

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI1]);

    attrParams->operMode = MCSPI_OPER_MODE_INTERRUPT;

    for(chCnt = 0U; chCnt < CONFIG_MCSPI1_NUM_CH; chCnt++)
    {
        gConfigMcspi1ChCfg[chCnt].inputSelect = MCSPI_IS_D1;
        gConfigMcspi1ChCfg[chCnt].dpe0        = MCSPI_DPE_DISABLE;
        gConfigMcspi1ChCfg[chCnt].dpe1        = MCSPI_DPE_ENABLE;
        gConfigMcspi1ChCfg[chCnt].csPolarity  = MCSPI_CS_POL_HIGH;

        for(uint8_t bitrateIndex = 0; bitrateIndex < APP_MCSPI_MAX_BITRATE_INDEX; bitrateIndex++)
        {
            gConfigMcspi1ChCfg[chCnt].bitRate     = bitRate[bitrateIndex];
            mcspiHandle = MCSPI_open(CONFIG_MCSPI1, mcspiOpenParams);
            TEST_ASSERT_NOT_NULL(mcspiHandle);
            if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
            {
                status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
                DebugP_assert(SystemP_SUCCESS == status);
            }
            dataWidth = mcspiChDataSize[chCnt];
            if (dataWidth < 9U)
            {
                /* Init TX buffer with known data and memset RX buffer */
                tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
                tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
            }
            else if (dataWidth < 17U)
            {
                /* Init TX buffer with known data and memset RX buffer */
                tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
                tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
            }
            else
            {
                /* Init TX buffer with known data and memset RX buffer */
                tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
                tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
            }
            fifoBitMask = 0x0U;
            for (dataWidthIdx = 0U;
                dataWidthIdx < dataWidth; dataWidthIdx++)
            {
                fifoBitMask |= (1U << dataWidthIdx);
            }

            /* Memfill buffers */
            for (i = 0U; i < testParams->transferLength; i++)
            {
                tempTxData = 0xDEADBABE;
                tempTxData &= (fifoBitMask);
                if (dataWidth < 9U)
                {
                    *tempTxPtr8++ = (uint8_t) (tempTxData);
                    *tempRxPtr8++ = 0U;
                }
                else if (dataWidth < 17U)
                {
                    *tempTxPtr16++ = (uint16_t) (tempTxData);
                    *tempRxPtr16++ = 0U;
                }
                else
                {
                    *tempTxPtr32++ = (uint32_t) (tempTxData);
                    *tempRxPtr32++ = 0U;
                }
            }
            /* Initiate transfer */
            spiTransaction.channel   = gConfigMcspi1ChCfg[chCnt].chNum;
            spiTransaction.dataSize  = dataWidth;
            spiTransaction.csDisable = TRUE;
            spiTransaction.count     = testParams->transferLength;
            spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
            spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
            spiTransaction.args      = NULL;
            transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
            TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

            if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
            {
                /* Wait for transfer completion */
                SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
            }

            /* Compare data */
            uint8_t *tempTxPtr, *tempRxPtr;
            tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
            tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
            for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
            {
                if(*tempTxPtr++ != *tempRxPtr++)
                {
                    status += SystemP_FAILURE;   /* Data mismatch */
                    DebugP_log("Data Mismatch at offset %d\r\n", i);
                    break;
                }
            }

            if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
            {
                SemaphoreP_destruct(&gMcspiTransferDoneSem);
            }

            MCSPI_close(gMcspiHandle[CONFIG_MCSPI1]);
        }
    }

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_turbo_mode(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    uint64_t            startTimeInUSec, elapsedTimeInUsecs, elapsedTimeInUsecsTurboMode;

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));
    memset(&gMcspiTxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer1[0U]));
    memset(&gMcspiRxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer1[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);
    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }
     /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }
    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;

    startTimeInUSec = ClockP_getTimeUsec();
    transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    elapsedTimeInUsecs = ClockP_getTimeUsec() - startTimeInUSec;
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("\nWITH TURBO MODE DISABLED \r\n");
    DebugP_log("McSPI Clock %d Hz\r\n", gConfigMcspi0ChCfg[0U].bitRate);
    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("Data Width \tData Length \tTransfer Time (micro sec)\r\n");
    DebugP_log("%u\t\t%u\t\t%5.2f\r\n", spiTransaction.dataSize, APP_MCSPI_MSGSIZE,
                        (float)elapsedTimeInUsecs);
    DebugP_log("----------------------------------------------------------\r\n\n");

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;

    startTimeInUSec = ClockP_getTimeUsec();
    transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    elapsedTimeInUsecsTurboMode = ClockP_getTimeUsec() - startTimeInUSec;
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("\nWITH TURBO MODE ENABLED \r\n");
    DebugP_log("McSPI Clock %d Hz\r\n", gConfigMcspi0ChCfg[0U].bitRate);
    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("Data Width \tData Length \tTransfer Time (micro sec)\r\n");
    DebugP_log("%u\t\t%u\t\t%5.2f\r\n", spiTransaction.dataSize, APP_MCSPI_MSGSIZE,
                        (float)elapsedTimeInUsecsTurboMode);
    DebugP_log("----------------------------------------------------------\r\n\n");

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    uint8_t *tempTxPtr1, *tempRxPtr1;
    tempTxPtr1 = (uint8_t *) &gMcspiTxBuffer1[0U];
    tempRxPtr1 = (uint8_t *) &gMcspiRxBuffer1[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr1++ != *tempRxPtr1++)
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
    /* Once turbo mode is enabled its transfer time is less. */
    if((elapsedTimeInUsecsTurboMode < elapsedTimeInUsecs) & (status != SystemP_FAILURE))
    {
        status = SystemP_SUCCESS;
    }

    MCSPI_close(mcspiHandle);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

void test_mcspi_loopback_multi_word_access(void *args)
{
    int32_t             status = SystemP_SUCCESS;
    uint32_t            i, dataWidth, fifoBitMask, tempTxData, dataWidthIdx;
    int32_t             transferOK, NumOfIteration = 100U;
    MCSPI_Transaction   spiTransaction;
    MCSPI_Handle        mcspiHandle;
    MCSPI_TestParams   *testParams = (MCSPI_TestParams *)args;
    uint8_t            *tempRxPtr8 = NULL, *tempTxPtr8 = NULL;
    uint16_t           *tempRxPtr16 = NULL, *tempTxPtr16 = NULL;
    uint32_t           *tempRxPtr32 = NULL, *tempTxPtr32 = NULL;
    MCSPI_OpenParams   *mcspiOpenParams = &(testParams->mcspiOpenParams);
    uint64_t            startTimeInUSec, elapsedTimeInUsecs, elapsedTimeInUsecsMultiAccess;
    MCSPI_Config        *config = &gMcspiConfig[CONFIG_MCSPI0];;
    MCSPI_Attrs   *attrParams = (MCSPI_Attrs *)config->attrs;

    /* Memset Buffers */
    memset(&gMcspiTxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer[0U]));
    memset(&gMcspiRxBuffer[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer[0U]));
    memset(&gMcspiTxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiTxBuffer1[0U]));
    memset(&gMcspiRxBuffer1[0U], 0, APP_MCSPI_MSGSIZE * sizeof(gMcspiRxBuffer1[0U]));

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);
    attrParams->multiWordAccess = FALSE;
    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        status = SemaphoreP_constructBinary(&gMcspiTransferDoneSem, 0);
        DebugP_assert(SystemP_SUCCESS == status);
    }

    dataWidth = testParams->dataSize;
    if (dataWidth < 9U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr8 = (uint8_t *) &gMcspiTxBuffer[0U];
        tempRxPtr8 = (uint8_t *) &gMcspiRxBuffer[0U];
    }
    else if (dataWidth < 17U)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr16 = (uint16_t *) &gMcspiTxBuffer[0U];
        tempRxPtr16 = (uint16_t *) &gMcspiRxBuffer[0U];
    }
    else
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr32 = (uint32_t *) &gMcspiTxBuffer[0U];
        tempRxPtr32 = (uint32_t *) &gMcspiRxBuffer[0U];
    }
    fifoBitMask = 0x0U;
    for (dataWidthIdx = 0U;
         dataWidthIdx < dataWidth; dataWidthIdx++)
    {
        fifoBitMask |= (1U << dataWidthIdx);
    }
     /* Memfill buffers */
    for (i = 0U; i < testParams->transferLength; i++)
    {
        tempTxData = 0xDEADBABE;
        tempTxData &= (fifoBitMask);
        if (dataWidth < 9U)
        {
            *tempTxPtr8++ = (uint8_t) (tempTxData);
            *tempRxPtr8++ = 0U;
        }
        else if (dataWidth < 17U)
        {
            *tempTxPtr16++ = (uint16_t) (tempTxData);
            *tempRxPtr16++ = 0U;
        }
        else
        {
            *tempTxPtr32++ = (uint32_t) (tempTxData);
            *tempRxPtr32++ = 0U;
        }
    }
    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;

    startTimeInUSec = ClockP_getTimeUsec();
    for(uint8_t j = 0U; j < NumOfIteration; j++)
    {
        transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    }
    elapsedTimeInUsecs = ClockP_getTimeUsec() - startTimeInUSec;
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("\nWITH MULTI WORD ACCESS MODE DISABLED \r\n");
    DebugP_log("McSPI Clock %d Hz\r\n", gConfigMcspi0ChCfg[0U].bitRate);
    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("Data Width \tData Length \tTransfer Time (micro sec)\r\n");
    DebugP_log("%u\t\t%u\t\t%5.2f\r\n", spiTransaction.dataSize, spiTransaction.count,
                        (float)elapsedTimeInUsecs / NumOfIteration);

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    uint8_t *tempTxPtr, *tempRxPtr;
    tempTxPtr = (uint8_t *) &gMcspiTxBuffer[0U];
    tempRxPtr = (uint8_t *) &gMcspiRxBuffer[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr++ != *tempRxPtr++)
        {
            status = SystemP_FAILURE;   /* Data mismatch */
            DebugP_log("Data Mismatch at offset %d\r\n", i);
            break;
        }
    }

    MCSPI_close(gMcspiHandle[CONFIG_MCSPI0]);
    attrParams->multiWordAccess = TRUE;
    mcspiHandle = MCSPI_open(CONFIG_MCSPI0, mcspiOpenParams);
    TEST_ASSERT_NOT_NULL(mcspiHandle);

    /* Initiate transfer */
    spiTransaction.channel   = testParams->mcspiChConfigParams->chNum;
    spiTransaction.count     = testParams->transferLength;
    spiTransaction.dataSize  = dataWidth;
    spiTransaction.csDisable = TRUE;
    spiTransaction.txBuf     = (void *)gMcspiTxBuffer;
    spiTransaction.rxBuf     = (void *)gMcspiRxBuffer;
    spiTransaction.args      = NULL;

    startTimeInUSec = ClockP_getTimeUsec();
    for(uint8_t j = 0U; j < NumOfIteration; j++)
    {
        transferOK = MCSPI_transfer(mcspiHandle, &spiTransaction);
    }
    elapsedTimeInUsecsMultiAccess = ClockP_getTimeUsec() - startTimeInUSec;
    TEST_APP_MCSPI_ASSERT_ON_FAILURE(transferOK, spiTransaction);

    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("\nWITH MULTI WORD ACCESS ENABLED \r\n");
    DebugP_log("McSPI Clock %d Hz\r\n", gConfigMcspi0ChCfg[0U].bitRate);
    DebugP_log("----------------------------------------------------------\r\n");
    DebugP_log("Data Width \tData Length \tTransfer Time (micro sec)\r\n");
    DebugP_log("%u\t\t%u\t\t%5.2f\r\n", spiTransaction.dataSize, spiTransaction.count,
                        (float)elapsedTimeInUsecsMultiAccess / NumOfIteration);
    DebugP_log("----------------------------------------------------------\r\n\n");

    if(mcspiOpenParams->transferMode == MCSPI_TRANSFER_MODE_CALLBACK)
    {
        /* Wait for transfer completion */
        SemaphoreP_pend(&gMcspiTransferDoneSem, SystemP_WAIT_FOREVER);
    }

    /* Compare data */
    uint8_t *tempTxPtr1, *tempRxPtr1;
    tempTxPtr1 = (uint8_t *) &gMcspiTxBuffer1[0U];
    tempRxPtr1 = (uint8_t *) &gMcspiRxBuffer1[0U];
    for(i = 0U; i < (APP_MCSPI_MSGSIZE * 4); i++)
    {
        if(*tempTxPtr1++ != *tempRxPtr1++)
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
    /* Multi Word Access transer will take less time. */
    if((elapsedTimeInUsecsMultiAccess < elapsedTimeInUsecs) & (status != SystemP_FAILURE))
    {
        status = SystemP_SUCCESS;
    }

    MCSPI_close(mcspiHandle);

    TEST_ASSERT_EQUAL_INT32(SystemP_SUCCESS, status);
    return;
}

static void test_mcspi_set_params(MCSPI_TestParams *testParams, uint32_t tcId)
{
    uint32_t bufWidthShift;
    MCSPI_Config     *config = &gMcspiConfig[CONFIG_MCSPI0];
    MCSPI_Attrs      *attrParams = (MCSPI_Attrs *)config->attrs;
    MCSPI_OpenParams *openParams = &(testParams->mcspiOpenParams);
    MCSPI_ChConfig    *chConfig = testParams->mcspiChConfigParams;

    /* Default Attribute Parameters */
    attrParams->baseAddr           = MCSPI0_BASE_ADDRESS;
    attrParams->inputClkFreq       = 50000000U;
    attrParams->intrNum            = MCSPI0_INT_NUM;
    attrParams->operMode           = MCSPI_OPER_MODE_INTERRUPT;
    attrParams->intrPriority       = 4U;
    attrParams->chMode             = MCSPI_CH_MODE_SINGLE;
    attrParams->pinMode            = MCSPI_PINMODE_4PIN;
    attrParams->initDelay          = MCSPI_INITDLY_0;

    /* Default Open Parameters */
    openParams->transferMode           = MCSPI_TRANSFER_MODE_BLOCKING;
    openParams->transferTimeout        = SystemP_WAIT_FOREVER;
    openParams->transferCallbackFxn    = NULL;
    openParams->msMode                 = MCSPI_MS_MODE_CONTROLLER;
    openParams->mcspiDmaIndex          = -1;

    testParams->dataSize           = 32;
    /* Default Channel Config Parameters */
    chConfig->chNum              = MCSPI_CHANNEL_0;
    chConfig->frameFormat        = MCSPI_FF_POL0_PHA0;
    chConfig->bitRate            = 50000000;
    chConfig->csPolarity         = MCSPI_CS_POL_LOW;
    chConfig->trMode             = MCSPI_TR_MODE_TX_RX;
    chConfig->inputSelect        = MCSPI_IS_D0;
    chConfig->dpe0               = MCSPI_DPE_ENABLE;
    chConfig->dpe1               = MCSPI_DPE_DISABLE;
    chConfig->slvCsSelect        = MCSPI_SLV_CS_SELECT_0;
    chConfig->startBitEnable     = FALSE;
    chConfig->startBitPolarity   = MCSPI_SB_POL_LOW;
    chConfig->csIdleTime         = MCSPI_TCS0_0_CLK;
    chConfig->defaultTxData      = 0x0U;
    chConfig->txFifoTrigLvl      = 16U;
    chConfig->rxFifoTrigLvl      = 16U;
    switch (tcId)
    {
        case 335:
            attrParams->operMode         = MCSPI_OPER_MODE_POLLED;
            break;
        case 336:
            openParams->transferMode = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            break;
/* AM263X does not support MCU_SPI instance */
#if !(defined(SOC_AM263X) || defined (SOC_AM263PX) || defined (SOC_AM261X))
#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
        case 970:
#if defined(SOC_AM65X)
            attrParams->baseAddr           = CSL_MCU_MCSPI0_CFG_BASE;
            attrParams->intrNum            = MCSPI3_INT_NUM;
#else
            attrParams->baseAddr           = CSL_MCU_MCSPI0_CFG_BASE;
            attrParams->intrNum            = 208U;
#endif
            break;
        case 971:
#if defined(SOC_AM65X)
            attrParams->baseAddr           = CSL_MCU_MCSPI1_CFG_BASE;
            attrParams->intrNum            = MCSPI4_INT_NUM;
#else
            attrParams->baseAddr           = CSL_MCU_MCSPI1_CFG_BASE;
            attrParams->intrNum            = 209U;
#endif
            testParams->dataSize           = 8;
            break;
#endif
#endif
        case 972:
#if !(defined (SOC_AM261X))
            attrParams->baseAddr           = MCSPI4_BASE_ADDRESS;
#endif
            attrParams->operMode           = MCSPI_OPER_MODE_POLLED;
            break;
        case 973:
#if defined (SOC_AM263PX)
            /* Due to some hardware constraint D0 cannot be used for SPI instance 2. */
            attrParams->baseAddr         = MCSPI2_BASE_ADDRESS;
            chConfig->inputSelect        = MCSPI_IS_D1;
            chConfig->dpe0               = MCSPI_DPE_DISABLE;
            chConfig->dpe1               = MCSPI_DPE_ENABLE;
#else
            attrParams->baseAddr           = MCSPI2_BASE_ADDRESS;
#endif
            attrParams->intrNum            = MCSPI2_INT_NUM;
            testParams->dataSize           = 16;
            break;
        case 974:
            attrParams->baseAddr           = MCSPI0_BASE_ADDRESS;
            attrParams->intrNum            = MCSPI0_INT_NUM;
            break;
        case 975:
#if !defined(SOC_AM261X)
            attrParams->baseAddr           = MCSPI4_BASE_ADDRESS;
            attrParams->intrNum            = MCSPI4_INT_NUM;
#endif
            break;
        case 980:
            testParams->dataSize           = 8;
            break;
        case 7351:
            testParams->dataSize           = 16;
            break;
        case 7352:
            testParams->dataSize           = 32;
            break;
        case 7353:
            testParams->dataSize           = 8;
            attrParams->operMode = MCSPI_OPER_MODE_POLLED;
            break;
        case 7354:
            testParams->dataSize           = 16;
            attrParams->operMode = MCSPI_OPER_MODE_POLLED;
            break;
        case 7355:
            testParams->dataSize           = 32;
            attrParams->operMode = MCSPI_OPER_MODE_POLLED;
            break;
        case 985:
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            chConfig->bitRate                  = 16661;
            break;
        case 991:
        case 992:
        case 993:
            attrParams->operMode = MCSPI_OPER_MODE_POLLED;
            break;
        case 995:
            testParams->dataSize = 16;
            break;
        case 996:
            testParams->dataSize = 8;
            chConfig->csIdleTime = MCSPI_TCS0_1_CLK;
            break;
        case 997:
            attrParams->pinMode = MCSPI_PINMODE_3PIN;
            break;
        case 998:
            attrParams->initDelay = MCSPI_INITDLY_8;
            break;
        case 999:
            chConfig->csPolarity = MCSPI_CS_POL_HIGH;
            break;
        case 1000:
            chConfig->startBitEnable = TRUE;
            break;
        case 1001:
            chConfig->csIdleTime = MCSPI_TCS0_3_CLK;
            break;
        case 1002:
            chConfig->inputSelect = MCSPI_IS_D1;
            chConfig->dpe0        = MCSPI_DPE_DISABLE;
            chConfig->dpe1        = MCSPI_DPE_ENABLE;
            break;
        case 1003:
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            break;
        case 1005:
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            break;
        case 1006:
            attrParams->operMode               = MCSPI_OPER_MODE_INTERRUPT;
            break;
        case 1007:
            attrParams->baseAddr               = MCSPI1_BASE_ADDRESS;
            attrParams->intrNum                = MCSPI1_INT_NUM;
            break;
        case 1009:
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            break;
        case 1011:
            testParams->dataSize               = 8;
            chConfig->bitRate                  = 1000000;
            break;
        case 1012:
            testParams->dataSize               = 16;
            chConfig->bitRate                  = 1000000;
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback_cancel;
            break;
        case 1014:
            openParams->transferTimeout        = 1U;
            chConfig->bitRate                  = 50000;
            break;
        case 1565:
            attrParams->operMode               = MCSPI_OPER_MODE_POLLED;
            break;
        case 11416:
            chConfig->frameFormat        = MCSPI_FF_POL0_PHA1;
            chConfig->txFifoTrigLvl      = 16U;
            chConfig->rxFifoTrigLvl      = 16U;
            break;
        case 11417:
            chConfig->frameFormat        = MCSPI_FF_POL1_PHA0;
            chConfig->txFifoTrigLvl      = 8U;
            chConfig->rxFifoTrigLvl      = 8U;
#if defined(SOC_AM65X)
            attrParams->baseAddr           = CSL_MCU_MCSPI0_CFG_BASE;
            attrParams->intrNum            = MCSPI3_INT_NUM;
#endif
            break;
        case 11418:
            chConfig->frameFormat        = MCSPI_FF_POL1_PHA1;
            chConfig->txFifoTrigLvl      = 32U;
            chConfig->rxFifoTrigLvl      = 32U;
            break;
        case 11599:
            chConfig->frameFormat        = MCSPI_FF_POL1_PHA1;
            chConfig->txFifoTrigLvl      = 26U;
            chConfig->rxFifoTrigLvl      = 26U;
            attrParams->operMode         = MCSPI_OPER_MODE_POLLED;
            break;
        case 11600:
            chConfig->frameFormat        = MCSPI_FF_POL1_PHA1;
            chConfig->txFifoTrigLvl      = 26U;
            chConfig->rxFifoTrigLvl      = 26U;
            attrParams->operMode         = MCSPI_OPER_MODE_POLLED;
            break;
        case 11429:
        /* To verify this test trigger level should be less than or equal to word length. */
            testParams->dataSize         = 8;
            chConfig->txFifoTrigLvl      = 1U;
            chConfig->rxFifoTrigLvl      = 1U;
            break;
        case 11430:
        /* To verify this test trigger level should be less than or equal to word length. */
            testParams->dataSize         = 8;
            chConfig->txFifoTrigLvl      = 16U;
            chConfig->rxFifoTrigLvl      = 1U;
            break;
        case 11431:
        /* To verify this test trigger level should be less than or equal to word length. */
            testParams->dataSize         = 8;
            chConfig->txFifoTrigLvl      = 1U;
            chConfig->rxFifoTrigLvl      = 16U;
            break;
        case 11537:
            chConfig->turboEnable        = TRUE;
            break;
        /* Multi Word Acess */
        case 11538:
            testParams->dataSize         = 8;
            chConfig->txFifoTrigLvl      = 16U;
            chConfig->rxFifoTrigLvl      = 16U;
            break;
#if (CONFIG_MCSPI_NUM_INSTANCES > 2)
        case 2394:
            attrParams->baseAddr               = MCSPI3_BASE_ADDRESS;
            testParams->dataSize               = 8;
            attrParams->operMode               = MCSPI_OPER_MODE_DMA;
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            openParams->mcspiDmaIndex          = 0;
            break;
        case 7356:
            attrParams->baseAddr               = MCSPI3_BASE_ADDRESS;
            testParams->dataSize               = 16;
            attrParams->operMode               = MCSPI_OPER_MODE_DMA;
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            openParams->mcspiDmaIndex          = 0;
            break;
        case 7357:
            attrParams->baseAddr               = MCSPI3_BASE_ADDRESS;
            testParams->dataSize               = 32;
            attrParams->operMode               = MCSPI_OPER_MODE_DMA;
            openParams->transferMode           = MCSPI_TRANSFER_MODE_CALLBACK;
            openParams->transferCallbackFxn    = test_mcspi_callback;
            openParams->mcspiDmaIndex          = 0;
            break;
        case 2397:
            attrParams->baseAddr               = MCSPI3_BASE_ADDRESS;
            testParams->dataSize               = 16;
            attrParams->operMode               = MCSPI_OPER_MODE_DMA;
            openParams->mcspiDmaIndex          = 0;
            break;
#endif
    }

    bufWidthShift = MCSPI_getBufWidthShift(testParams->dataSize);
    /* If Count is less than FIFO trigger level */
    if ((tcId == 971U) || (tcId == 991U))
    {
        testParams->transferLength = (2U * (sizeof(gMcspiTxBuffer[0U]) / (1 << bufWidthShift)));
    }
    else
    {
        testParams->transferLength = (APP_MCSPI_MSGSIZE * (sizeof(gMcspiTxBuffer[0U]) / (1 << bufWidthShift)));
    }

    return;
}

#if defined(SOC_AM65X)
void test_mcspi_mcu_mcspi1_detach(void){
    uint32_t            baseAddr;
    volatile uint32_t  *kickAddr;

    baseAddr = (uint32_t) AddrTranslateP_getLocalAddr(CSL_MCU_CTRL_MMR0_CFG0_BASE);

   /* Lock 0 */
    kickAddr = (volatile uint32_t *) (baseAddr + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK0);
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK0_UNLOCK_VAL);   /* KICK 0 */
    kickAddr++;
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK1_UNLOCK_VAL);   /* KICK 1 */

    /* Lock 1 */
    kickAddr = (volatile uint32_t *) (baseAddr + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0);
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK0_UNLOCK_VAL);   /* KICK 0 */
    kickAddr++;
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK1_UNLOCK_VAL);   /* KICK 1 */

    /* Disable direct conncetion of main domain MCSPI3 and MCU_MCSPI1 instances */
    CSL_REG32_WR((baseAddr + CSL_MCU_CTRL_MMR_CFG0_MCU_SPI1_CTRL), MCU_MCSPI1_LINK_DISABLE);

    /* Lock 0 */
    kickAddr = (volatile uint32_t *) (baseAddr + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK0);
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK_LOCK_VAL);      /* KICK 0 */
    kickAddr++;
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK_LOCK_VAL);      /* KICK 1 */

    /* Lock 1 */
    kickAddr = (volatile uint32_t *) (baseAddr + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0);
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK_LOCK_VAL);      /* KICK 0 */
    kickAddr++;
    CSL_REG32_WR(kickAddr, CTRLMMR_KICK_LOCK_VAL);      /* KICK 1 */
}
#endif
