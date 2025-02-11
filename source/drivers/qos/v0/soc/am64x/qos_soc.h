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
 *  \file qos_soc.h
 *
 *  \brief am64x SOC Quality of Service (QoS) definitions
 *         Auto generated by K3 Resource Partitioning Tool
 */

#ifndef QOS_SOC_H_
#define QOS_SOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define QOS_0	(0 << 0)
#define QOS_1	(1 << 0)
#define QOS_2	(2 << 0)
#define QOS_3	(3 << 0)
#define QOS_4	(4 << 0)
#define QOS_5	(5 << 0)
#define QOS_6	(6 << 0)
#define QOS_7	(7 << 0)

#define ORDERID_0	(0 << 4)
#define ORDERID_1	(1 << 4)
#define ORDERID_2	(2 << 4)
#define ORDERID_3	(3 << 4)
#define ORDERID_4	(4 << 4)
#define ORDERID_5	(5 << 4)
#define ORDERID_6	(6 << 4)
#define ORDERID_7	(7 << 4)
#define ORDERID_8	(8 << 4)
#define ORDERID_9	(9 << 4)
#define ORDERID_10	(10 << 4)
#define ORDERID_11	(11 << 4)
#define ORDERID_12	(12 << 4)
#define ORDERID_13	(13 << 4)
#define ORDERID_14	(14 << 4)
#define ORDERID_15	(15 << 4)

#define ASEL_0	(0 << 8)
#define ASEL_1	(1 << 8)
#define ASEL_2	(2 << 8)
#define ASEL_3	(3 << 8)
#define ASEL_4	(4 << 8)
#define ASEL_5	(5 << 8)
#define ASEL_6	(6 << 8)
#define ASEL_7	(7 << 8)
#define ASEL_8	(8 << 8)
#define ASEL_9	(9 << 8)
#define ASEL_10	(10 << 8)
#define ASEL_11	(11 << 8)
#define ASEL_12	(12 << 8)
#define ASEL_13	(13 << 8)
#define ASEL_14	(14 << 8)
#define ASEL_15	(15 << 8)

#define EPRIORITY_0	(0 << 12)
#define EPRIORITY_1	(1 << 12)
#define EPRIORITY_2	(2 << 12)
#define EPRIORITY_3	(3 << 12)
#define EPRIORITY_4	(4 << 12)
#define EPRIORITY_5	(5 << 12)
#define EPRIORITY_6	(6 << 12)
#define EPRIORITY_7	(7 << 12)

#define VIRTID_0	(0 << 16)
#define VIRTID_1	(1 << 16)
#define VIRTID_2	(2 << 16)
#define VIRTID_3	(3 << 16)
#define VIRTID_4	(4 << 16)
#define VIRTID_5	(5 << 16)
#define VIRTID_6	(6 << 16)
#define VIRTID_7	(7 << 16)
#define VIRTID_8	(8 << 16)
#define VIRTID_9	(9 << 16)
#define VIRTID_10	(10 << 16)
#define VIRTID_11	(11 << 16)
#define VIRTID_12	(12 << 16)
#define VIRTID_13	(13 << 16)
#define VIRTID_14	(14 << 16)
#define VIRTID_15	(15 << 16)

#define ATYPE_0	(0 << 28)
#define ATYPE_1	(1 << 28)
#define ATYPE_2	(2 << 28)
#define ATYPE_3	(3 << 28)

#define PULSAR_LITE_MAIN_0_CPU0_PMST	0x45D20000
#define PULSAR_LITE_MAIN_0_CPU1_PMST	0x45D20400
#define PULSAR_LITE_MAIN_1_CPU0_PMST	0x45D20800
#define PULSAR_LITE_MAIN_1_CPU1_PMST	0x45D20C00
#define BLAZAR_MCU_0_VBUSP_M	0x45D30000
#define DMSS_AM64_MAIN_0_SEC_PROXY_0_DST	0x45D40000
#define DMSS_AM64_MAIN_0_RINGACC_0_DST	0x45D40400
#define SAM64_A53_256KB_WRAP_MAIN_0_A53_DUAL_WRAP_CBA_AXI_R	0x45D80400
#define SAM64_A53_256KB_WRAP_MAIN_0_A53_DUAL_WRAP_CBA_AXI_W	0x45D80800
#define EMMC8SS_16FFC_MAIN_0_EMMCSS_WR	0x45D81000
#define EMMC8SS_16FFC_MAIN_0_EMMCSS_RD	0x45D81400
#define GIC500SS_1_2_MAIN_0_MEM_WR_VBUSM	0x45D82000
#define GIC500SS_1_2_MAIN_0_MEM_RD_VBUSM	0x45D82400
#define PULSAR_LITE_MAIN_0_CPU0_RMST	0x45D82800
#define PULSAR_LITE_MAIN_0_CPU0_WMST	0x45D82C00
#define PULSAR_LITE_MAIN_0_CPU1_RMST	0x45D83C00
#define PULSAR_LITE_MAIN_0_CPU1_WMST	0x45D84000
#define PULSAR_LITE_MAIN_1_CPU0_RMST	0x45D85000
#define PULSAR_LITE_MAIN_1_CPU0_WMST	0x45D85400
#define PULSAR_LITE_MAIN_1_CPU1_RMST	0x45D86400
#define PULSAR_LITE_MAIN_1_CPU1_WMST	0x45D86800
#define ICSS_G_16FF_MAIN_0_PR1_EXT_VBUSM	0x45D87800
#define ICSS_G_16FF_MAIN_1_PR1_EXT_VBUSM	0x45D87C00
#define PCIE_G2X1_64_MAIN_0_PCIE_MST_RD	0x45D88000
#define PCIE_G2X1_64_MAIN_0_PCIE_MST_WR	0x45D88400
#define EMMCSD4SS_MAIN_0_EMMCSDSS_WR	0x45D88800
#define EMMCSD4SS_MAIN_0_EMMCSDSS_RD	0x45D88C00
#define SA2_UL_MAIN_0_CTXCACH_EXT_DMA	0x45D89000
#define USB3P0SS64_16FFC_MAIN_0_MSTR0	0x45D89400
#define USB3P0SS64_16FFC_MAIN_0_MSTW0	0x45D89800
#define DEBUGSS_K3_WRAP_CV0_MAIN_0_VBUSMR	0x45D8A000
#define DEBUGSS_K3_WRAP_CV0_MAIN_0_VBUSMW	0x45D8A400

#ifdef __cplusplus
}
#endif

#endif /* #ifndef QOS_SOC_H_ */