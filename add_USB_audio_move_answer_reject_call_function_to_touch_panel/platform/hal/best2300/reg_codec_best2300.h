/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __REG_CODEC_H__
#define __REG_CODEC_H__

#include "plat_types.h"

struct CODEC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
    __IO uint32_t REG_078;
    __IO uint32_t REG_07C;
    __IO uint32_t REG_080;
    __IO uint32_t REG_084;
    __IO uint32_t REG_088;
    __IO uint32_t REG_08C;
    __IO uint32_t REG_090;
    __IO uint32_t REG_094;
    __IO uint32_t REG_098;
    __IO uint32_t REG_09C;
    __IO uint32_t REG_0A0;
    __IO uint32_t REG_0A4;
    __IO uint32_t REG_0A8;
    __IO uint32_t REG_0AC;
    __IO uint32_t REG_0B0;
    __IO uint32_t REG_0B4;
    __IO uint32_t REG_0B8;
    __IO uint32_t REG_0BC;
    __IO uint32_t REG_0C0;
    __IO uint32_t REG_0C4;
    __IO uint32_t REG_0C8;
    __IO uint32_t REG_0CC;
    __IO uint32_t REG_0D0;
    __IO uint32_t REG_0D4;
    __IO uint32_t REG_0D8;
    __IO uint32_t REG_0DC;
    __IO uint32_t REG_0E0;
    __IO uint32_t REG_0E4;
    __IO uint32_t REG_0E8;
    __IO uint32_t REG_0EC;
    __IO uint32_t REG_0F0;
    __IO uint32_t REG_0F4;
    __IO uint32_t REG_0F8;
    __IO uint32_t REG_0FC;
    __IO uint32_t REG_100;
    __IO uint32_t REG_104;
    __IO uint32_t REG_108;
    __IO uint32_t REG_10C;
    __IO uint32_t REG_110;
    __IO uint32_t REG_114;
    __IO uint32_t REG_118;
    __IO uint32_t REG_11C;
    __IO uint32_t REG_120;
    __IO uint32_t REG_124;
    __IO uint32_t REG_128;
    __IO uint32_t REG_12C;
    __IO uint32_t REG_130;
    __IO uint32_t REG_134;
    __IO uint32_t REG_138;
    __IO uint32_t REG_13C;
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
    __IO uint32_t REG_148;
    __IO uint32_t REG_14C;
    __IO uint32_t REG_150;
    __IO uint32_t REG_154;
    __IO uint32_t REG_158;
    __IO uint32_t REG_15C;
    __IO uint32_t REG_160;
    __IO uint32_t REG_164;
    __IO uint32_t REG_168;
    __IO uint32_t REG_16C;
    __IO uint32_t REG_170;
    __IO uint32_t REG_174;
    __IO uint32_t REG_178;
    __IO uint32_t REG_17C;
    __IO uint32_t REG_180;
    __IO uint32_t REG_184;
    __IO uint32_t REG_188;
    __IO uint32_t REG_18C;
    __IO uint32_t REG_190;
    __IO uint32_t REG_194;
    __IO uint32_t REG_198;
    __IO uint32_t REG_19C;
    __IO uint32_t REG_1A0;
    __IO uint32_t REG_1A4;
    __IO uint32_t REG_1A8;
    __IO uint32_t REG_1AC;
    __IO uint32_t REG_1B0;
    __IO uint32_t REG_1B4;
    __IO uint32_t REG_1B8;
    __IO uint32_t REG_1BC;
    __IO uint32_t REG_1C0;
    __IO uint32_t REG_1C4;
    __IO uint32_t REG_1C8;
    __IO uint32_t REG_1CC;
    __IO uint32_t REG_1D0;
    __IO uint32_t REG_1D4;
    __IO uint32_t REG_1D8;
    __IO uint32_t REG_1DC;
    __IO uint32_t REG_1E0;
    __IO uint32_t REG_1E4;
    __IO uint32_t REG_1E8;
    __IO uint32_t REG_1EC;
    __IO uint32_t REG_1F0;
    __IO uint32_t REG_1F4;
    __IO uint32_t REG_1F8;
    __IO uint32_t REG_1FC;
    __IO uint32_t REG_200;
    __IO uint32_t REG_204;
    __IO uint32_t REG_208;
    __IO uint32_t REG_20C;
    __IO uint32_t REG_210;
    __IO uint32_t REG_214;
    __IO uint32_t REG_218;
    __IO uint32_t REG_21C;
    __IO uint32_t REG_220;
    __IO uint32_t REG_224;
};

// reg_00
#define CODEC_CODEC_IF_EN                        (1 << 0)
#define CODEC_ADC_ENABLE                         (1 << 1)
#define CODEC_ADC_ENABLE_CH0                     (1 << 2)
#define CODEC_ADC_ENABLE_CH1                     (1 << 3)
#define CODEC_ADC_ENABLE_CH2                     (1 << 4)
#define CODEC_ADC_ENABLE_CH3                     (1 << 5)
#define CODEC_ADC_ENABLE_CH4                     (1 << 6)
#define CODEC_DAC_ENABLE                         (1 << 7)
#define CODEC_DMACTRL_RX                         (1 << 8)
#define CODEC_DMACTRL_TX                         (1 << 9)

// reg_04
#define CODEC_RX_FIFO_FLUSH_CH0                  (1 << 0)
#define CODEC_RX_FIFO_FLUSH_CH1                  (1 << 1)
#define CODEC_RX_FIFO_FLUSH_CH2                  (1 << 2)
#define CODEC_RX_FIFO_FLUSH_CH3                  (1 << 3)
#define CODEC_RX_FIFO_FLUSH_CH4                  (1 << 4)
#define CODEC_TX_FIFO_FLUSH                      (1 << 5)
#define CODEC_DSD_RX_FIFO_FLUSH                  (1 << 6)
#define CODEC_DSD_TX_FIFO_FLUSH                  (1 << 7)
#define CODEC_MC_FIFO_FLUSH                      (1 << 8)
#define CODEC_IIR_RX_FIFO_FLUSH                  (1 << 9)
#define CODEC_IIR_TX_FIFO_FLUSH                  (1 << 10)

// reg_08
#define CODEC_CODEC_RX_THRESHOLD(n)              (((n) & 0xF) << 0)
#define CODEC_CODEC_RX_THRESHOLD_MASK            (0xF << 0)
#define CODEC_CODEC_RX_THRESHOLD_SHIFT           (0)
#define CODEC_CODEC_TX_THRESHOLD(n)              (((n) & 0xF) << 4)
#define CODEC_CODEC_TX_THRESHOLD_MASK            (0xF << 4)
#define CODEC_CODEC_TX_THRESHOLD_SHIFT           (4)
#define CODEC_DSD_RX_THRESHOLD(n)                (((n) & 0xF) << 8)
#define CODEC_DSD_RX_THRESHOLD_MASK              (0xF << 8)
#define CODEC_DSD_RX_THRESHOLD_SHIFT             (8)
#define CODEC_DSD_TX_THRESHOLD(n)                (((n) & 0x7) << 12)
#define CODEC_DSD_TX_THRESHOLD_MASK              (0x7 << 12)
#define CODEC_DSD_TX_THRESHOLD_SHIFT             (12)
#define CODEC_MC_THRESHOLD(n)                    (((n) & 0xF) << 15)
#define CODEC_MC_THRESHOLD_MASK                  (0xF << 15)
#define CODEC_MC_THRESHOLD_SHIFT                 (15)
#define CODEC_IIR_RX_THRESHOLD(n)                (((n) & 0x3F) << 19)
#define CODEC_IIR_RX_THRESHOLD_MASK              (0x3F << 19)
#define CODEC_IIR_RX_THRESHOLD_SHIFT             (19)
#define CODEC_IIR_TX_THRESHOLD(n)                (((n) & 0x3F) << 25)
#define CODEC_IIR_TX_THRESHOLD_MASK              (0x3F << 25)
#define CODEC_IIR_TX_THRESHOLD_SHIFT             (25)

// reg_0c
#define CODEC_CODEC_RX_OVERFLOW(n)               (((n) & 0x1F) << 0)
#define CODEC_CODEC_RX_OVERFLOW_MASK             (0x1F << 0)
#define CODEC_CODEC_RX_OVERFLOW_SHIFT            (0)
#define CODEC_CODEC_RX_UNDERFLOW(n)              (((n) & 0x1F) << 5)
#define CODEC_CODEC_RX_UNDERFLOW_MASK            (0x1F << 5)
#define CODEC_CODEC_RX_UNDERFLOW_SHIFT           (5)
#define CODEC_CODEC_TX_OVERFLOW                  (1 << 10)
#define CODEC_CODEC_TX_UNDERFLOW                 (1 << 11)
#define CODEC_DSD_RX_OVERFLOW                    (1 << 12)
#define CODEC_DSD_RX_UNDERFLOW                   (1 << 13)
#define CODEC_DSD_TX_OVERFLOW                    (1 << 14)
#define CODEC_DSD_TX_UNDERFLOW                   (1 << 15)
#define CODEC_MC_OVERFLOW                        (1 << 16)
#define CODEC_MC_UNDERFLOW                       (1 << 17)
#define CODEC_IIR_RX_OVERFLOW                    (1 << 18)
#define CODEC_IIR_RX_UNDERFLOW                   (1 << 19)
#define CODEC_IIR_TX_OVERFLOW                    (1 << 20)
#define CODEC_IIR_TX_UNDERFLOW                   (1 << 21)
#define CODEC_EVENT_TRIGGER                      (1 << 22)
#define CODEC_FB_CHECK_ERROR_TRIG_CH0            (1 << 23)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1            (1 << 24)
#define CODEC_VAD_FIND                           (1 << 25)
#define CODEC_VAD_NOT_FIND                       (1 << 26)
#define CODEC_BT_TRIGGER                         (1 << 27)
#define CODEC_ADC_MAX_OVERFLOW                   (1 << 28)

// reg_10
#define CODEC_CODEC_RX_OVERFLOW_MSK(n)           (((n) & 0x1F) << 0)
#define CODEC_CODEC_RX_OVERFLOW_MSK_MASK         (0x1F << 0)
#define CODEC_CODEC_RX_OVERFLOW_MSK_SHIFT        (0)
#define CODEC_CODEC_RX_UNDERFLOW_MSK(n)          (((n) & 0x1F) << 5)
#define CODEC_CODEC_RX_UNDERFLOW_MSK_MASK        (0x1F << 5)
#define CODEC_CODEC_RX_UNDERFLOW_MSK_SHIFT       (5)
#define CODEC_CODEC_TX_OVERFLOW_MSK              (1 << 10)
#define CODEC_CODEC_TX_UNDERFLOW_MSK             (1 << 11)
#define CODEC_DSD_RX_OVERFLOW_MSK                (1 << 12)
#define CODEC_DSD_RX_UNDERFLOW_MSK               (1 << 13)
#define CODEC_DSD_TX_OVERFLOW_MSK                (1 << 14)
#define CODEC_DSD_TX_UNDERFLOW_MSK               (1 << 15)
#define CODEC_MC_OVERFLOW_MSK                    (1 << 16)
#define CODEC_MC_UNDERFLOW_MSK                   (1 << 17)
#define CODEC_IIR_RX_OVERFLOW_MSK                (1 << 18)
#define CODEC_IIR_RX_UNDERFLOW_MSK               (1 << 19)
#define CODEC_IIR_TX_OVERFLOW_MSK                (1 << 20)
#define CODEC_IIR_TX_UNDERFLOW_MSK               (1 << 21)
#define CODEC_EVENT_TRIGGER_MSK                  (1 << 22)
#define CODEC_FB_CHECK_ERROR_TRIG_CH0_MSK        (1 << 23)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1_MSK        (1 << 24)
#define CODEC_VAD_FIND_MSK                       (1 << 25)
#define CODEC_VAD_NOT_FIND_MSK                   (1 << 26)
#define CODEC_BT_TRIGGER_MSK                     (1 << 27)
#define CODEC_ADC_MAX_OVERFLOW_MSK               (1 << 28)

// reg_14
#define CODEC_FIFO_COUNT_CH0(n)                  (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_CH0_MASK                (0xF << 0)
#define CODEC_FIFO_COUNT_CH0_SHIFT               (0)
#define CODEC_FIFO_COUNT_CH1(n)                  (((n) & 0xF) << 4)
#define CODEC_FIFO_COUNT_CH1_MASK                (0xF << 4)
#define CODEC_FIFO_COUNT_CH1_SHIFT               (4)
#define CODEC_FIFO_COUNT_CH2(n)                  (((n) & 0xF) << 8)
#define CODEC_FIFO_COUNT_CH2_MASK                (0xF << 8)
#define CODEC_FIFO_COUNT_CH2_SHIFT               (8)
#define CODEC_FIFO_COUNT_CH3(n)                  (((n) & 0xF) << 12)
#define CODEC_FIFO_COUNT_CH3_MASK                (0xF << 12)
#define CODEC_FIFO_COUNT_CH3_SHIFT               (12)
#define CODEC_FIFO_COUNT_CH4(n)                  (((n) & 0xF) << 16)
#define CODEC_FIFO_COUNT_CH4_MASK                (0xF << 16)
#define CODEC_FIFO_COUNT_CH4_SHIFT               (16)
#define CODEC_FIFO_COUNT_RX_DSD(n)               (((n) & 0xF) << 20)
#define CODEC_FIFO_COUNT_RX_DSD_MASK             (0xF << 20)
#define CODEC_FIFO_COUNT_RX_DSD_SHIFT            (20)
#define CODEC_FIFO_COUNT_RX_IIR(n)               (((n) & 0x3F) << 24)
#define CODEC_FIFO_COUNT_RX_IIR_MASK             (0x3F << 24)
#define CODEC_FIFO_COUNT_RX_IIR_SHIFT            (24)

// reg_18
#define CODEC_FIFO_COUNT_TX(n)                   (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_TX_MASK                 (0xF << 0)
#define CODEC_FIFO_COUNT_TX_SHIFT                (0)
#define CODEC_STATE_RX_CH(n)                     (((n) & 0x3F) << 4)
#define CODEC_STATE_RX_CH_MASK                   (0x3F << 4)
#define CODEC_STATE_RX_CH_SHIFT                  (4)
#define CODEC_FIFO_COUNT_TX_DSD(n)               (((n) & 0x7) << 10)
#define CODEC_FIFO_COUNT_TX_DSD_MASK             (0x7 << 10)
#define CODEC_FIFO_COUNT_TX_DSD_SHIFT            (10)
#define CODEC_MC_FIFO_COUNT(n)                   (((n) & 0xF) << 13)
#define CODEC_MC_FIFO_COUNT_MASK                 (0xF << 13)
#define CODEC_MC_FIFO_COUNT_SHIFT                (13)
#define CODEC_FIFO_COUNT_TX_IIR(n)               (((n) & 0x3F) << 17)
#define CODEC_FIFO_COUNT_TX_IIR_MASK             (0x3F << 17)
#define CODEC_FIFO_COUNT_TX_IIR_SHIFT            (17)

// reg_1c
#define CODEC_RX_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                 (0)

// reg_20
#define CODEC_RX_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                 (0)

// reg_24
#define CODEC_RX_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                 (0)

// reg_28
#define CODEC_RX_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                 (0)

// reg_2c
#define CODEC_RX_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                 (0)

// reg_30
#define CODEC_RX_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                 (0)

// reg_34
#define CODEC_RX_FIFO_DATA_DSD(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_DSD_MASK              (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_DSD_SHIFT             (0)

// reg_38
#define CODEC_MC_FIFO_DATA(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_MC_FIFO_DATA_MASK                  (0xFFFFFFFF << 0)
#define CODEC_MC_FIFO_DATA_SHIFT                 (0)

// reg_3c
#define CODEC_RX_FIFO_DATA_IIR(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_IIR_MASK              (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_IIR_SHIFT             (0)

// reg_40
#define CODEC_MODE_16BIT_ADC_CH0                 (1 << 0)
#define CODEC_MODE_16BIT_ADC_CH1                 (1 << 1)
#define CODEC_MODE_16BIT_ADC_CH2                 (1 << 2)
#define CODEC_MODE_16BIT_ADC_CH3                 (1 << 3)
#define CODEC_MODE_16BIT_ADC_CH4                 (1 << 4)
#define CODEC_MODE_24BIT_ADC                     (1 << 5)
#define CODEC_MODE_32BIT_ADC                     (1 << 6)

// reg_44
#define CODEC_DUAL_CHANNEL_DAC                   (1 << 0)
#define CODEC_DAC_EXCHANGE_L_R                   (1 << 1)
#define CODEC_MODE_16BIT_DAC                     (1 << 2)
#define CODEC_MODE_32BIT_DAC                     (1 << 3)

// reg_48
#define CODEC_DSD_IF_EN                          (1 << 0)
#define CODEC_DSD_ENABLE                         (1 << 1)
#define CODEC_DSD_DUAL_CHANNEL                   (1 << 2)
#define CODEC_DSD_MSB_FIRST                      (1 << 3)
#define CODEC_MODE_24BIT_DSD                     (1 << 4)
#define CODEC_MODE_32BIT_DSD                     (1 << 5)
#define CODEC_DMA_CTRL_RX_DSD                    (1 << 6)
#define CODEC_DMA_CTRL_TX_DSD                    (1 << 7)
#define CODEC_MODE_16BIT_DSD                     (1 << 8)
#define CODEC_DSD_IN_16BIT                       (1 << 9)

// reg_4c
#define CODEC_MC_ENABLE                          (1 << 0)
#define CODEC_DUAL_CHANNEL_MC                    (1 << 1)
#define CODEC_MODE_16BIT_MC                      (1 << 2)
#define CODEC_DMA_CTRL_MC                        (1 << 3)
#define CODEC_MC_DELAY(n)                        (((n) & 0xFF) << 4)
#define CODEC_MC_DELAY_MASK                      (0xFF << 4)
#define CODEC_MC_DELAY_SHIFT                     (4)
#define CODEC_MC_RATE_SEL                        (1 << 12)
#define CODEC_MODE_32BIT_MC                      (1 << 13)

// reg_50
#define CODEC_CODEC_COUNT_KEEP(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_COUNT_KEEP_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_COUNT_KEEP_SHIFT             (0)

// reg_54
#define CODEC_DAC_ENABLE_SEL(n)                  (((n) & 0x3) << 0)
#define CODEC_DAC_ENABLE_SEL_MASK                (0x3 << 0)
#define CODEC_DAC_ENABLE_SEL_SHIFT               (0)
#define CODEC_ADC_ENABLE_SEL(n)                  (((n) & 0x3) << 2)
#define CODEC_ADC_ENABLE_SEL_MASK                (0x3 << 2)
#define CODEC_ADC_ENABLE_SEL_SHIFT               (2)
#define CODEC_CODEC_DAC_ENABLE_SEL(n)            (((n) & 0x3) << 4)
#define CODEC_CODEC_DAC_ENABLE_SEL_MASK          (0x3 << 4)
#define CODEC_CODEC_DAC_ENABLE_SEL_SHIFT         (4)
#define CODEC_CODEC_ADC_ENABLE_SEL(n)            (((n) & 0x3) << 6)
#define CODEC_CODEC_ADC_ENABLE_SEL_MASK          (0x3 << 6)
#define CODEC_CODEC_ADC_ENABLE_SEL_SHIFT         (6)
#define CODEC_GPIO_TRIGGER_DB_ENABLE             (1 << 8)
#define CODEC_STAMP_CLR_USED                     (1 << 9)
#define CODEC_EVENT_SEL                          (1 << 10)
#define CODEC_EVENT_FOR_CAPTURE                  (1 << 11)
#define CODEC_TEST_PORT_SEL(n)                   (((n) & 0x7) << 12)
#define CODEC_TEST_PORT_SEL_MASK                 (0x7 << 12)
#define CODEC_TEST_PORT_SEL_SHIFT                (12)
#define CODEC_PLL_OSC_TRIGGER_SEL(n)             (((n) & 0x3) << 15)
#define CODEC_PLL_OSC_TRIGGER_SEL_MASK           (0x3 << 15)
#define CODEC_PLL_OSC_TRIGGER_SEL_SHIFT          (15)
#define CODEC_FAULT_MUTE_DAC_ENABLE              (1 << 17)

// reg_60
#define CODEC_EN_CLK_ADC_ANA(n)                  (((n) & 0x1F) << 0)
#define CODEC_EN_CLK_ADC_ANA_MASK                (0x1F << 0)
#define CODEC_EN_CLK_ADC_ANA_SHIFT               (0)
#define CODEC_EN_CLK_ADC(n)                      (((n) & 0x3F) << 5)
#define CODEC_EN_CLK_ADC_MASK                    (0x3F << 5)
#define CODEC_EN_CLK_ADC_SHIFT                   (5)
#define CODEC_EN_CLK_DAC                         (1 << 11)
#define CODEC_POL_ADC_ANA(n)                     (((n) & 0x1F) << 12)
#define CODEC_POL_ADC_ANA_MASK                   (0x1F << 12)
#define CODEC_POL_ADC_ANA_SHIFT                  (12)
#define CODEC_POL_DAC_OUT                        (1 << 17)
#define CODEC_CFG_CLK_OUT(n)                     (((n) & 0x7) << 18)
#define CODEC_CFG_CLK_OUT_MASK                   (0x7 << 18)
#define CODEC_CFG_CLK_OUT_SHIFT                  (18)

// reg_64
#define CODEC_SOFT_RSTN_ADC_ANA(n)               (((n) & 0x1F) << 0)
#define CODEC_SOFT_RSTN_ADC_ANA_MASK             (0x1F << 0)
#define CODEC_SOFT_RSTN_ADC_ANA_SHIFT            (0)
#define CODEC_SOFT_RSTN_ADC(n)                   (((n) & 0x3F) << 5)
#define CODEC_SOFT_RSTN_ADC_MASK                 (0x3F << 5)
#define CODEC_SOFT_RSTN_ADC_SHIFT                (5)
#define CODEC_SOFT_RSTN_DAC                      (1 << 11)
#define CODEC_SOFT_RSTN_RS                       (1 << 12)
#define CODEC_SOFT_RSTN_IIR                      (1 << 13)
#define CODEC_SOFT_RSTN_32K                      (1 << 14)

// reg_68
#define CODEC_RET1N_RF                           (1 << 0)
#define CODEC_RET2N_RF                           (1 << 1)
#define CODEC_PGEN_RF                            (1 << 2)
#define CODEC_EMA_RF(n)                          (((n) & 0x7) << 3)
#define CODEC_EMA_RF_MASK                        (0x7 << 3)
#define CODEC_EMA_RF_SHIFT                       (3)
#define CODEC_EMAW_RF(n)                         (((n) & 0x3) << 6)
#define CODEC_EMAW_RF_MASK                       (0x3 << 6)
#define CODEC_EMAW_RF_SHIFT                      (6)
#define CODEC_EMAS_RF                            (1 << 8)
#define CODEC_WABL_RF                            (1 << 9)
#define CODEC_WABLM_RF(n)                        (((n) & 0x3) << 10)
#define CODEC_WABLM_RF_MASK                      (0x3 << 10)
#define CODEC_WABLM_RF_SHIFT                     (10)
#define CODEC_RET1N_SRAM                         (1 << 12)
#define CODEC_RET2N_SRAM                         (1 << 13)
#define CODEC_PGEN_SRAM                          (1 << 14)
#define CODEC_EMA_SRAM(n)                        (((n) & 0x7) << 15)
#define CODEC_EMA_SRAM_MASK                      (0x7 << 15)
#define CODEC_EMA_SRAM_SHIFT                     (15)
#define CODEC_EMAW_SRAM(n)                       (((n) & 0x3) << 18)
#define CODEC_EMAW_SRAM_MASK                     (0x3 << 18)
#define CODEC_EMAW_SRAM_SHIFT                    (18)
#define CODEC_EMAS_SRAM                          (1 << 20)
#define CODEC_WABL_SRAM                          (1 << 21)
#define CODEC_WABLM_SRAM(n)                      (((n) & 0x3) << 22)
#define CODEC_WABLM_SRAM_MASK                    (0x3 << 22)
#define CODEC_WABLM_SRAM_SHIFT                   (22)
#define CODEC_EMA_ROM(n)                         (((n) & 0x7) << 24)
#define CODEC_EMA_ROM_MASK                       (0x7 << 24)
#define CODEC_EMA_ROM_SHIFT                      (24)
#define CODEC_KEN_ROM                            (1 << 27)
#define CODEC_PGEN_ROM_0                         (1 << 28)
#define CODEC_PGEN_ROM_1                         (1 << 29)

// reg_78
#define CODEC_CODEC_SIDE_TONE_CH_SEL            (1 << 0)
#define CODEC_CODEC_ADC_IIR_CH0_SEL(n)          (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IIR_CH0_SEL_MASK        (0x7 << 1)
#define CODEC_CODEC_ADC_IIR_CH0_SEL_SHIFT       (1)
#define CODEC_CODEC_ADC_IIR_CH1_SEL(n)          (((n) & 0x7) << 4)
#define CODEC_CODEC_ADC_IIR_CH1_SEL_MASK        (0x7 << 4)
#define CODEC_CODEC_ADC_IIR_CH1_SEL_SHIFT       (4)
#define CODEC_CODEC_DAC_HBF4_DELAY_SEL          (1 << 7)
#define CODEC_TRIG_TIME(n)                      (((n) & 0x3FFFFF) << 8)
#define CODEC_TRIG_TIME_MASK                    (0x3FFFFF << 8)
#define CODEC_TRIG_TIME_SHIFT                   (8)
#define CODEC_TRIG_TIME_ENABLE                  (1 << 30)
#define CODEC_GET_CNT_TRIG                      (1 << 31)

// reg_7c
#define CODEC_RESERVED_REG1(n)                   (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RESERVED_REG1_MASK                 (0xFFFFFFFF << 0)
#define CODEC_RESERVED_REG1_SHIFT                (0)

// reg_80
#define CODEC_CODEC_ADC_EN                       (1 << 0)
#define CODEC_CODEC_ADC_EN_CH0                   (1 << 1)
#define CODEC_CODEC_ADC_EN_CH1                   (1 << 2)
#define CODEC_CODEC_ADC_EN_CH2                   (1 << 3)
#define CODEC_CODEC_ADC_EN_CH3                   (1 << 4)
#define CODEC_CODEC_ADC_EN_CH4                   (1 << 5)
#define CODEC_CODEC_SIDE_TONE_GAIN(n)            (((n) & 0x1F) << 6)
#define CODEC_CODEC_SIDE_TONE_GAIN_MASK          (0x1F << 6)
#define CODEC_CODEC_SIDE_TONE_GAIN_SHIFT         (6)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL            (1 << 11)
#define CODEC_CODEC_ADC_LOOP                     (1 << 12)
#define CODEC_CODEC_LOOP_SEL_L(n)                (((n) & 0x7) << 13)
#define CODEC_CODEC_LOOP_SEL_L_MASK              (0x7 << 13)
#define CODEC_CODEC_LOOP_SEL_L_SHIFT             (13)
#define CODEC_CODEC_LOOP_SEL_R(n)                (((n) & 0x7) << 16)
#define CODEC_CODEC_LOOP_SEL_R_MASK              (0x7 << 16)
#define CODEC_CODEC_LOOP_SEL_R_SHIFT             (16)
#define CODEC_CODEC_TEST_PORT_SEL(n)             (((n) & 0x1F) << 19)
#define CODEC_CODEC_TEST_PORT_SEL_MASK           (0x1F << 19)
#define CODEC_CODEC_TEST_PORT_SEL_SHIFT          (19)

// reg_84
#define CODEC_CODEC_ADC_SIGNED_CH0               (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH0(n)            (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH0_MASK          (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH0_SHIFT         (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0(n)          (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0_MASK        (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0_SHIFT       (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH0          (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH0          (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH0          (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH0             (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH0(n)              (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH0_MASK            (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH0_SHIFT           (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0(n)          (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0_MASK        (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0_SHIFT       (30)

// reg_88
#define CODEC_CODEC_ADC_SIGNED_CH1               (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH1(n)            (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH1_MASK          (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH1_SHIFT         (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1(n)          (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1_MASK        (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1_SHIFT       (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH1          (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH1          (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH1          (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH1             (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH1(n)              (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH1_MASK            (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH1_SHIFT           (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1(n)          (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1_MASK        (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1_SHIFT       (30)

// reg_8c
#define CODEC_CODEC_ADC_SIGNED_CH2               (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH2(n)            (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH2_MASK          (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH2_SHIFT         (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2(n)          (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2_MASK        (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2_SHIFT       (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH2          (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH2          (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH2          (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH2             (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH2(n)              (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH2_MASK            (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH2_SHIFT           (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2(n)          (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2_MASK        (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2_SHIFT       (30)

// reg_90
#define CODEC_CODEC_ADC_SIGNED_CH3               (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH3(n)            (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH3_MASK          (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH3_SHIFT         (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3(n)          (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3_MASK        (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3_SHIFT       (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH3          (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH3          (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH3          (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH3             (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH3(n)              (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH3_MASK            (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH3_SHIFT           (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH3(n)          (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH3_MASK        (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH3_SHIFT       (30)

// reg_94
#define CODEC_CODEC_ADC_SIGNED_CH4               (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH4(n)            (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH4_MASK          (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH4_SHIFT         (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH4(n)          (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH4_MASK        (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH4_SHIFT       (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH4          (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH4          (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH4          (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH4             (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH4(n)              (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH4_MASK            (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH4_SHIFT           (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH4(n)          (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH4_MASK        (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH4_SHIFT       (30)

// reg_98
#define CODEC_CODEC_DAC_EN                       (1 << 0)
#define CODEC_CODEC_DAC_EN_CH0                   (1 << 1)
#define CODEC_CODEC_DAC_EN_CH1                   (1 << 2)
#define CODEC_CODEC_DAC_DITHER_GAIN(n)           (((n) & 0x1F) << 3)
#define CODEC_CODEC_DAC_DITHER_GAIN_MASK         (0x1F << 3)
#define CODEC_CODEC_DAC_DITHER_GAIN_SHIFT        (3)
#define CODEC_CODEC_DAC_SDM_GAIN(n)              (((n) & 0x3) << 8)
#define CODEC_CODEC_DAC_SDM_GAIN_MASK            (0x3 << 8)
#define CODEC_CODEC_DAC_SDM_GAIN_SHIFT           (8)
#define CODEC_CODEC_DITHER_BYPASS                (1 << 10)
#define CODEC_CODEC_DAC_HBF3_BYPASS              (1 << 11)
#define CODEC_CODEC_DAC_HBF2_BYPASS              (1 << 12)
#define CODEC_CODEC_DAC_HBF1_BYPASS              (1 << 13)
#define CODEC_CODEC_DAC_UP_SEL(n)                (((n) & 0x7) << 14)
#define CODEC_CODEC_DAC_UP_SEL_MASK              (0x7 << 14)
#define CODEC_CODEC_DAC_UP_SEL_SHIFT             (14)
#define CODEC_CODEC_DAC_TONE_TEST                (1 << 17)
#define CODEC_CODEC_DAC_SIN1K_STEP(n)            (((n) & 0xF) << 18)
#define CODEC_CODEC_DAC_SIN1K_STEP_MASK          (0xF << 18)
#define CODEC_CODEC_DAC_SIN1K_STEP_SHIFT         (18)
#define CODEC_CODEC_DAC_OSR_SEL(n)               (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_OSR_SEL_MASK             (0x3 << 22)
#define CODEC_CODEC_DAC_OSR_SEL_SHIFT            (22)
#define CODEC_CODEC_DAC_LR_SWAP                  (1 << 24)
#define CODEC_CODEC_DAC_SDM_H4_6M_CH0            (1 << 25)
#define CODEC_CODEC_DAC_SDM_H4_6M_CH1            (1 << 26)
#define CODEC_CODEC_DAC_L_FIR_UPSAMPLE           (1 << 27)
#define CODEC_CODEC_DAC_R_FIR_UPSAMPLE           (1 << 28)
#define CODEC_CODEC_DAC_SDM_CLOSE                (1 << 29)
#define CODEC_CODEC_DAC_USE_HBF4                 (1 << 30)
#define CODEC_CODEC_DAC_USE_HBF5                 (1 << 31)

// reg_9c
#define CODEC_CODEC_DAC_GAIN_CH0(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_CH0_MASK            (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_GAIN_SEL_CH0             (1 << 20)
#define CODEC_CODEC_DAC_GAIN_UPDATE              (1 << 21)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH0          (1 << 22)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH1          (1 << 23)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH2          (1 << 24)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH3          (1 << 25)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH4          (1 << 26)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL(n)      (((n) & 0x3) << 27)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_MASK    (0x3 << 27)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SHIFT   (27)

// reg_a0
#define CODEC_CODEC_DAC_GAIN_CH1(n)              (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_CH1_MASK            (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_CH1_SHIFT           (0)
#define CODEC_CODEC_DAC_GAIN_SEL_CH1             (1 << 20)
#define CODEC_CODEC_DAC_OUT_SWAP                 (1 << 21)
#define CODEC_CODEC_DAC_H4_DELAY_CH0(n)          (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_H4_DELAY_CH0_MASK        (0x3 << 22)
#define CODEC_CODEC_DAC_H4_DELAY_CH0_SHIFT       (22)
#define CODEC_CODEC_DAC_L4_DELAY_CH0(n)          (((n) & 0x3) << 24)
#define CODEC_CODEC_DAC_L4_DELAY_CH0_MASK        (0x3 << 24)
#define CODEC_CODEC_DAC_L4_DELAY_CH0_SHIFT       (24)
#define CODEC_CODEC_DAC_H4_DELAY_CH1(n)          (((n) & 0x3) << 26)
#define CODEC_CODEC_DAC_H4_DELAY_CH1_MASK        (0x3 << 26)
#define CODEC_CODEC_DAC_H4_DELAY_CH1_SHIFT       (26)
#define CODEC_CODEC_DAC_L4_DELAY_CH1(n)          (((n) & 0x3) << 28)
#define CODEC_CODEC_DAC_L4_DELAY_CH1_MASK        (0x3 << 28)
#define CODEC_CODEC_DAC_L4_DELAY_CH1_SHIFT       (28)

// reg_a4
#define CODEC_CODEC_PDM_ENABLE                   (1 << 0)
#define CODEC_CODEC_PDM_DATA_INV                 (1 << 1)
#define CODEC_CODEC_PDM_ADC_SEL_CH0              (1 << 2)
#define CODEC_CODEC_PDM_ADC_SEL_CH1              (1 << 3)
#define CODEC_CODEC_PDM_ADC_SEL_CH2              (1 << 4)
#define CODEC_CODEC_PDM_ADC_SEL_CH3              (1 << 5)
#define CODEC_CODEC_PDM_ADC_SEL_CH4              (1 << 6)

// reg_a8
#define CODEC_CODEC_PDM_MUX_CH0(n)               (((n) & 0x7) << 0)
#define CODEC_CODEC_PDM_MUX_CH0_MASK             (0x7 << 0)
#define CODEC_CODEC_PDM_MUX_CH0_SHIFT            (0)
#define CODEC_CODEC_PDM_MUX_CH1(n)               (((n) & 0x7) << 3)
#define CODEC_CODEC_PDM_MUX_CH1_MASK             (0x7 << 3)
#define CODEC_CODEC_PDM_MUX_CH1_SHIFT            (3)
#define CODEC_CODEC_PDM_MUX_CH2(n)               (((n) & 0x7) << 6)
#define CODEC_CODEC_PDM_MUX_CH2_MASK             (0x7 << 6)
#define CODEC_CODEC_PDM_MUX_CH2_SHIFT            (6)
#define CODEC_CODEC_PDM_MUX_CH3(n)               (((n) & 0x7) << 9)
#define CODEC_CODEC_PDM_MUX_CH3_MASK             (0x7 << 9)
#define CODEC_CODEC_PDM_MUX_CH3_SHIFT            (9)
#define CODEC_CODEC_PDM_MUX_CH4(n)               (((n) & 0x7) << 12)
#define CODEC_CODEC_PDM_MUX_CH4_MASK             (0x7 << 12)
#define CODEC_CODEC_PDM_MUX_CH4_SHIFT            (12)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0(n)         (((n) & 0x3) << 15)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0_MASK       (0x3 << 15)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0_SHIFT      (15)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1(n)         (((n) & 0x3) << 17)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1_MASK       (0x3 << 17)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1_SHIFT      (17)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2(n)         (((n) & 0x3) << 19)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2_MASK       (0x3 << 19)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2_SHIFT      (19)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3(n)         (((n) & 0x3) << 21)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3_MASK       (0x3 << 21)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3_SHIFT      (21)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4(n)         (((n) & 0x3) << 23)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4_MASK       (0x3 << 23)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4_SHIFT      (23)

// reg_b0
#define CODEC_CODEC_CLASSG_EN                    (1 << 0)
#define CODEC_CODEC_CLASSG_QUICK_DOWN            (1 << 1)
#define CODEC_CODEC_CLASSG_STEP_3_4N             (1 << 2)
#define CODEC_CODEC_CLASSG_LR                    (1 << 3)
#define CODEC_CODEC_CLASSG_WINDOW(n)             (((n) & 0xFFF) << 4)
#define CODEC_CODEC_CLASSG_WINDOW_MASK           (0xFFF << 4)
#define CODEC_CODEC_CLASSG_WINDOW_SHIFT          (4)

// reg_b4
#define CODEC_CODEC_CLASSG_THD0(n)               (((n) & 0xFF) << 0)
#define CODEC_CODEC_CLASSG_THD0_MASK             (0xFF << 0)
#define CODEC_CODEC_CLASSG_THD0_SHIFT            (0)
#define CODEC_CODEC_CLASSG_THD1(n)               (((n) & 0xFF) << 8)
#define CODEC_CODEC_CLASSG_THD1_MASK             (0xFF << 8)
#define CODEC_CODEC_CLASSG_THD1_SHIFT            (8)
#define CODEC_CODEC_CLASSG_THD2(n)               (((n) & 0xFF) << 16)
#define CODEC_CODEC_CLASSG_THD2_MASK             (0xFF << 16)
#define CODEC_CODEC_CLASSG_THD2_SHIFT            (16)

// reg_b8
#define CODEC_CODEC_DSD_ENABLE_L                 (1 << 0)
#define CODEC_CODEC_DSD_ENABLE_R                 (1 << 1)
#define CODEC_CODEC_DSD_DATA_INV                 (1 << 2)
#define CODEC_CODEC_DSD_SAMPLE_RATE(n)           (((n) & 0x3) << 3)
#define CODEC_CODEC_DSD_SAMPLE_RATE_MASK         (0x3 << 3)
#define CODEC_CODEC_DSD_SAMPLE_RATE_SHIFT        (3)

// reg_bc
#define CODEC_CODEC_ADC_MC_EN_CH0                (1 << 0)
#define CODEC_CODEC_ADC_MC_EN_CH1                (1 << 1)
#define CODEC_CODEC_FEEDBACK_MC_EN_CH0           (1 << 2)
#define CODEC_CODEC_FEEDBACK_MC_EN_CH1           (1 << 3)

// reg_c0
#define CODEC_CODEC_DRE_ENABLE_CH0               (1 << 0)
#define CODEC_CODEC_DRE_STEP_MODE_CH0(n)         (((n) & 0x3) << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH0_MASK       (0x3 << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH0_SHIFT      (1)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(n)     (((n) & 0xF) << 3)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_MASK   (0xF << 3)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_SHIFT  (3)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(n)      (((n) & 0xF) << 7)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_MASK    (0xF << 7)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_SHIFT   (7)
#define CODEC_CODEC_DRE_DELAY_CH0(n)             (((n) & 0x3F) << 11)
#define CODEC_CODEC_DRE_DELAY_CH0_MASK           (0x3F << 11)
#define CODEC_CODEC_DRE_DELAY_CH0_SHIFT          (11)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0   (1 << 17)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0(n)       (((n) & 0x7) << 18)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0_MASK     (0x7 << 18)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0_SHIFT    (18)

// reg_c4
#define CODEC_CODEC_DRE_AMP_HIGH_CH0(n)          (((n) & 0x7FF) << 0)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0_MASK        (0x7FF << 0)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0_SHIFT       (0)
#define CODEC_CODEC_DRE_WINDOW_CH0(n)            (((n) & 0x1FFFFF) << 11)
#define CODEC_CODEC_DRE_WINDOW_CH0_MASK          (0x1FFFFF << 11)
#define CODEC_CODEC_DRE_WINDOW_CH0_SHIFT         (11)

// reg_c8
#define CODEC_CODEC_DRE_ENABLE_CH1               (1 << 0)
#define CODEC_CODEC_DRE_STEP_MODE_CH1(n)         (((n) & 0x3) << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH1_MASK       (0x3 << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH1_SHIFT      (1)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH1(n)     (((n) & 0xF) << 3)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH1_MASK   (0xF << 3)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH1_SHIFT  (3)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(n)      (((n) & 0xF) << 7)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH1_MASK    (0xF << 7)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH1_SHIFT   (7)
#define CODEC_CODEC_DRE_DELAY_CH1(n)             (((n) & 0x3F) << 11)
#define CODEC_CODEC_DRE_DELAY_CH1_MASK           (0x3F << 11)
#define CODEC_CODEC_DRE_DELAY_CH1_SHIFT          (11)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH1   (1 << 17)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH1(n)       (((n) & 0x7) << 18)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH1_MASK     (0x7 << 18)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH1_SHIFT    (18)

// reg_cc
#define CODEC_CODEC_DRE_AMP_HIGH_CH1(n)          (((n) & 0x7FF) << 0)
#define CODEC_CODEC_DRE_AMP_HIGH_CH1_MASK        (0x7FF << 0)
#define CODEC_CODEC_DRE_AMP_HIGH_CH1_SHIFT       (0)
#define CODEC_CODEC_DRE_WINDOW_CH1(n)            (((n) & 0x1FFFFF) << 11)
#define CODEC_CODEC_DRE_WINDOW_CH1_MASK          (0x1FFFFF << 11)
#define CODEC_CODEC_DRE_WINDOW_CH1_SHIFT         (11)

// reg_d0
#define CODEC_CODEC_ANC_ENABLE_CH0               (1 << 0)
#define CODEC_CODEC_ANC_ENABLE_CH1               (1 << 1)
#define CODEC_CODEC_DUAL_ANC_CH0                 (1 << 2)
#define CODEC_CODEC_DUAL_ANC_CH1                 (1 << 3)
#define CODEC_CODEC_ANC_MUTE_CH0                 (1 << 4)
#define CODEC_CODEC_ANC_MUTE_CH1                 (1 << 5)
#define CODEC_CODEC_FF_CH0_FIR_EN                (1 << 6)
#define CODEC_CODEC_FF_CH1_FIR_EN                (1 << 7)
#define CODEC_CODEC_FB_CH0_FIR_EN                (1 << 8)
#define CODEC_CODEC_FB_CH1_FIR_EN                (1 << 9)
#define CODEC_CODEC_ANC_RATE_SEL                 (1 << 10)
#define CODEC_CODEC_ANC_FF_SR_SEL(n)             (((n) & 0x3) << 11)
#define CODEC_CODEC_ANC_FF_SR_SEL_MASK           (0x3 << 11)
#define CODEC_CODEC_ANC_FF_SR_SEL_SHIFT          (11)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL(n)       (((n) & 0x7) << 13)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL_MASK     (0x7 << 13)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL_SHIFT    (13)
#define CODEC_CODEC_ANC_FB_SR_SEL(n)             (((n) & 0x3) << 16)
#define CODEC_CODEC_ANC_FB_SR_SEL_MASK           (0x3 << 16)
#define CODEC_CODEC_ANC_FB_SR_SEL_SHIFT          (16)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL(n)       (((n) & 0x7) << 18)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL_MASK     (0x7 << 18)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL_SHIFT    (18)
#define CODEC_CODEC_FEEDBACK_CH0                 (1 << 21)
#define CODEC_CODEC_FEEDBACK_CH1                 (1 << 22)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH2            (1 << 23)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH2           (1 << 24)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH3            (1 << 25)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH3           (1 << 26)

// reg_d4
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0(n)      (((n) & 0xFFF) << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0_MASK    (0xFFF << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0_SHIFT   (0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH1(n)      (((n) & 0xFFF) << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH1_MASK    (0xFFF << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH1_SHIFT   (12)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0   (1 << 24)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1   (1 << 25)

// reg_d8
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0(n)      (((n) & 0xFFF) << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0_MASK    (0xFFF << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0_SHIFT   (0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH1(n)      (((n) & 0xFFF) << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH1_MASK    (0xFFF << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH1_SHIFT   (12)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0   (1 << 24)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1   (1 << 25)

// reg_dc
#define CODEC_CODEC_IIR_ENABLE                   (1 << 0)
#define CODEC_CODEC_IIR_CH0_BYPASS               (1 << 1)
#define CODEC_CODEC_IIR_CH1_BYPASS               (1 << 2)
#define CODEC_CODEC_IIR_CH2_BYPASS               (1 << 3)
#define CODEC_CODEC_IIR_CH3_BYPASS               (1 << 4)
#define CODEC_CODEC_IIR_COUNT_CH0(n)             (((n) & 0xF) << 5)
#define CODEC_CODEC_IIR_COUNT_CH0_MASK           (0xF << 5)
#define CODEC_CODEC_IIR_COUNT_CH0_SHIFT          (5)
#define CODEC_CODEC_IIR_COUNT_CH1(n)             (((n) & 0xF) << 9)
#define CODEC_CODEC_IIR_COUNT_CH1_MASK           (0xF << 9)
#define CODEC_CODEC_IIR_COUNT_CH1_SHIFT          (9)
#define CODEC_CODEC_IIR_COUNT_CH2(n)             (((n) & 0xF) << 13)
#define CODEC_CODEC_IIR_COUNT_CH2_MASK           (0xF << 13)
#define CODEC_CODEC_IIR_COUNT_CH2_SHIFT          (13)
#define CODEC_CODEC_IIR_COUNT_CH3(n)             (((n) & 0xF) << 17)
#define CODEC_CODEC_IIR_COUNT_CH3_MASK           (0xF << 17)
#define CODEC_CODEC_IIR_COUNT_CH3_SHIFT          (17)
#define CODEC_CODEC_DAC_L_IIR_ENABLE             (1 << 21)
#define CODEC_CODEC_DAC_R_IIR_ENABLE             (1 << 22)
#define CODEC_CODEC_ADC_CH0_IIR_ENABLE           (1 << 23)
#define CODEC_CODEC_ADC_CH1_IIR_ENABLE           (1 << 24)
#define CODEC_CODEC_IIR_COEF_SWAP                (1 << 25)
#define CODEC_CODEC_IIR_COEF_SWAP_STATUS         (1 << 26)

// reg_e0
#define CODEC_CODEC_DAC_DC_CH0(n)                (((n) & 0x7FFFF) << 0)
#define CODEC_CODEC_DAC_DC_CH0_MASK              (0x7FFFF << 0)
#define CODEC_CODEC_DAC_DC_CH0_SHIFT             (0)
#define CODEC_CODEC_DAC_DC_UPDATE_CH0            (1 << 19)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(n) (((n) & 0xFF) << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0_MASK (0xFF << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0_SHIFT (20)
#define CODEC_CODEC_DAC_DC_UPDATE_PASS0_CH0      (1 << 28)
#define CODEC_CODEC_DAC_DC_UPDATE_STATUS_CH0     (1 << 29)

// reg_e4
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE          (1 << 0)
#define CODEC_CODEC_RESAMPLE_DAC_L_ENABLE        (1 << 1)
#define CODEC_CODEC_RESAMPLE_DAC_R_ENABLE        (1 << 2)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE          (1 << 3)
#define CODEC_CODEC_RESAMPLE_ADC_DUAL_CH         (1 << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL(n)      (((n) & 0x7) << 5)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_MASK    (0x7 << 5)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_SHIFT   (5)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL(n)      (((n) & 0x7) << 8)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_MASK    (0x7 << 8)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_SHIFT   (8)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE    (1 << 11)
#define CODEC_CODEC_RESAMPLE_DAC_TRIGGER_SEL(n)  (((n) & 0x3) << 12)
#define CODEC_CODEC_RESAMPLE_DAC_TRIGGER_SEL_MASK (0x3 << 12)
#define CODEC_CODEC_RESAMPLE_DAC_TRIGGER_SEL_SHIFT (12)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE    (1 << 14)
#define CODEC_CODEC_RESAMPLE_ADC_TRIGGER_SEL(n)  (((n) & 0x3) << 15)
#define CODEC_CODEC_RESAMPLE_ADC_TRIGGER_SEL_MASK (0x3 << 15)
#define CODEC_CODEC_RESAMPLE_ADC_TRIGGER_SEL_SHIFT (15)

// reg_e8
#define CODEC_CODEC_DAC_DC_CH1(n)                (((n) & 0x7FFFF) << 0)
#define CODEC_CODEC_DAC_DC_CH1_MASK              (0x7FFFF << 0)
#define CODEC_CODEC_DAC_DC_CH1_SHIFT             (0)
#define CODEC_CODEC_DAC_DC_UPDATE_CH1            (1 << 19)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1(n) (((n) & 0xFF) << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1_MASK (0xFF << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1_SHIFT (20)
#define CODEC_CODEC_DAC_DC_UPDATE_PASS0_CH1      (1 << 28)
#define CODEC_CODEC_DAC_DC_UPDATE_STATUS_CH1     (1 << 29)

// reg_ec
#define CODEC_CODEC_RAMP_STEP_CH0(n)             (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_CH0_MASK           (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_CH0_SHIFT          (0)
#define CODEC_CODEC_RAMP_DIRECT_CH0              (1 << 12)
#define CODEC_CODEC_RAMP_EN_CH0                  (1 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH0(n)         (((n) & 0x7) << 14)
#define CODEC_CODEC_RAMP_INTERVAL_CH0_MASK       (0x7 << 14)
#define CODEC_CODEC_RAMP_INTERVAL_CH0_SHIFT      (14)

// reg_f0
#define CODEC_CODEC_RAMP_STEP_CH1(n)             (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_CH1_MASK           (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_CH1_SHIFT          (0)
#define CODEC_CODEC_RAMP_DIRECT_CH1              (1 << 12)
#define CODEC_CODEC_RAMP_EN_CH1                  (1 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH1(n)         (((n) & 0x7) << 14)
#define CODEC_CODEC_RAMP_INTERVAL_CH1_MASK       (0x7 << 14)
#define CODEC_CODEC_RAMP_INTERVAL_CH1_SHIFT      (14)

// reg_f4
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC(n)    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_MASK  (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SHIFT (0)

// reg_f8
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC(n)    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC_MASK  (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC_SHIFT (0)

// reg_100
#define CODEC_FIR_STREAM_ENABLE_CH0              (1 << 0)
#define CODEC_FIR_STREAM_ENABLE_CH1              (1 << 1)
#define CODEC_FIR_STREAM_ENABLE_CH2              (1 << 2)
#define CODEC_FIR_STREAM_ENABLE_CH3              (1 << 3)
#define CODEC_FIR_ENABLE_CH0                     (1 << 4)
#define CODEC_FIR_ENABLE_CH1                     (1 << 5)
#define CODEC_FIR_ENABLE_CH2                     (1 << 6)
#define CODEC_FIR_ENABLE_CH3                     (1 << 7)
#define CODEC_DMA_CTRL_RX_FIR                    (1 << 8)
#define CODEC_DMA_CTRL_TX_FIR                    (1 << 9)
#define CODEC_FIR_UPSAMPLE_CH0                   (1 << 10)
#define CODEC_FIR_UPSAMPLE_CH1                   (1 << 11)
#define CODEC_FIR_UPSAMPLE_CH2                   (1 << 12)
#define CODEC_FIR_UPSAMPLE_CH3                   (1 << 13)
#define CODEC_MODE_32BIT_FIR                     (1 << 14)
#define CODEC_FIR_RESERVED_REG0                  (1 << 15)
#define CODEC_MODE_16BIT_FIR_TX_CH0              (1 << 16)
#define CODEC_MODE_16BIT_FIR_RX_CH0              (1 << 17)
#define CODEC_MODE_16BIT_FIR_TX_CH1              (1 << 18)
#define CODEC_MODE_16BIT_FIR_RX_CH1              (1 << 19)
#define CODEC_MODE_16BIT_FIR_TX_CH2              (1 << 20)
#define CODEC_MODE_16BIT_FIR_RX_CH2              (1 << 21)
#define CODEC_MODE_16BIT_FIR_TX_CH3              (1 << 22)
#define CODEC_MODE_16BIT_FIR_RX_CH3              (1 << 23)

// reg_104
#define CODEC_FIR_ACCESS_OFFSET_CH0(n)           (((n) & 0x7) << 0)
#define CODEC_FIR_ACCESS_OFFSET_CH0_MASK         (0x7 << 0)
#define CODEC_FIR_ACCESS_OFFSET_CH0_SHIFT        (0)
#define CODEC_FIR_ACCESS_OFFSET_CH1(n)           (((n) & 0x7) << 3)
#define CODEC_FIR_ACCESS_OFFSET_CH1_MASK         (0x7 << 3)
#define CODEC_FIR_ACCESS_OFFSET_CH1_SHIFT        (3)
#define CODEC_FIR_ACCESS_OFFSET_CH2(n)           (((n) & 0x7) << 6)
#define CODEC_FIR_ACCESS_OFFSET_CH2_MASK         (0x7 << 6)
#define CODEC_FIR_ACCESS_OFFSET_CH2_SHIFT        (6)
#define CODEC_FIR_ACCESS_OFFSET_CH3(n)           (((n) & 0x7) << 9)
#define CODEC_FIR_ACCESS_OFFSET_CH3_MASK         (0x7 << 9)
#define CODEC_FIR_ACCESS_OFFSET_CH3_SHIFT        (9)

// reg_108
#define CODEC_STREAM0_FIR1_CH0                   (1 << 0)
#define CODEC_FIR_MODE_CH0(n)                    (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH0_MASK                  (0x3 << 1)
#define CODEC_FIR_MODE_CH0_SHIFT                 (1)
#define CODEC_FIR_ORDER_CH0(n)                   (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH0_MASK                 (0x3FF << 3)
#define CODEC_FIR_ORDER_CH0_SHIFT                (3)
#define CODEC_FIR_SAMPLE_START_CH0(n)            (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH0_MASK          (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH0_SHIFT         (13)
#define CODEC_FIR_SAMPLE_NUM_CH0(n)              (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH0_MASK            (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH0_SHIFT           (22)
#define CODEC_FIR_DO_REMAP_CH0                   (1 << 31)

// reg_10c
#define CODEC_FIR_RESULT_BASE_ADDR_CH0(n)        (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH0_MASK      (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH0_SHIFT     (0)
#define CODEC_FIR_SLIDE_OFFSET_CH0(n)            (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH0_MASK          (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH0_SHIFT         (9)
#define CODEC_FIR_BURST_LENGTH_CH0(n)            (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH0_MASK          (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH0_SHIFT         (15)
#define CODEC_FIR_GAIN_SEL_CH0(n)                (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH0_MASK              (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH0_SHIFT             (21)
#define CODEC_FIR_LOOP_NUM_CH0(n)                (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH0_MASK              (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH0_SHIFT             (25)

// reg_110
#define CODEC_STREAM0_FIR1_CH1                   (1 << 0)
#define CODEC_FIR_MODE_CH1(n)                    (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH1_MASK                  (0x3 << 1)
#define CODEC_FIR_MODE_CH1_SHIFT                 (1)
#define CODEC_FIR_ORDER_CH1(n)                   (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH1_MASK                 (0x3FF << 3)
#define CODEC_FIR_ORDER_CH1_SHIFT                (3)
#define CODEC_FIR_SAMPLE_START_CH1(n)            (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH1_MASK          (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH1_SHIFT         (13)
#define CODEC_FIR_SAMPLE_NUM_CH1(n)              (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH1_MASK            (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH1_SHIFT           (22)
#define CODEC_FIR_DO_REMAP_CH1                   (1 << 31)

// reg_114
#define CODEC_FIR_RESULT_BASE_ADDR_CH1(n)        (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH1_MASK      (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH1_SHIFT     (0)
#define CODEC_FIR_SLIDE_OFFSET_CH1(n)            (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH1_MASK          (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH1_SHIFT         (9)
#define CODEC_FIR_BURST_LENGTH_CH1(n)            (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH1_MASK          (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH1_SHIFT         (15)
#define CODEC_FIR_GAIN_SEL_CH1(n)                (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH1_MASK              (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH1_SHIFT             (21)
#define CODEC_FIR_LOOP_NUM_CH1(n)                (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH1_MASK              (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH1_SHIFT             (25)

// reg_118
#define CODEC_STREAM0_FIR1_CH2                   (1 << 0)
#define CODEC_FIR_MODE_CH2(n)                    (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH2_MASK                  (0x3 << 1)
#define CODEC_FIR_MODE_CH2_SHIFT                 (1)
#define CODEC_FIR_ORDER_CH2(n)                   (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH2_MASK                 (0x3FF << 3)
#define CODEC_FIR_ORDER_CH2_SHIFT                (3)
#define CODEC_FIR_SAMPLE_START_CH2(n)            (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH2_MASK          (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH2_SHIFT         (13)
#define CODEC_FIR_SAMPLE_NUM_CH2(n)              (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH2_MASK            (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH2_SHIFT           (22)
#define CODEC_FIR_DO_REMAP_CH2                   (1 << 31)

// reg_11c
#define CODEC_FIR_RESULT_BASE_ADDR_CH2(n)        (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH2_MASK      (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH2_SHIFT     (0)
#define CODEC_FIR_SLIDE_OFFSET_CH2(n)            (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH2_MASK          (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH2_SHIFT         (9)
#define CODEC_FIR_BURST_LENGTH_CH2(n)            (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH2_MASK          (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH2_SHIFT         (15)
#define CODEC_FIR_GAIN_SEL_CH2(n)                (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH2_MASK              (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH2_SHIFT             (21)
#define CODEC_FIR_LOOP_NUM_CH2(n)                (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH2_MASK              (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH2_SHIFT             (25)

// reg_120
#define CODEC_STREAM0_FIR1_CH3                   (1 << 0)
#define CODEC_FIR_MODE_CH3(n)                    (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH3_MASK                  (0x3 << 1)
#define CODEC_FIR_MODE_CH3_SHIFT                 (1)
#define CODEC_FIR_ORDER_CH3(n)                   (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH3_MASK                 (0x3FF << 3)
#define CODEC_FIR_ORDER_CH3_SHIFT                (3)
#define CODEC_FIR_SAMPLE_START_CH3(n)            (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH3_MASK          (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH3_SHIFT         (13)
#define CODEC_FIR_SAMPLE_NUM_CH3(n)              (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH3_MASK            (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH3_SHIFT           (22)
#define CODEC_FIR_DO_REMAP_CH3                   (1 << 31)

// reg_124
#define CODEC_FIR_RESULT_BASE_ADDR_CH3(n)        (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH3_MASK      (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH3_SHIFT     (0)
#define CODEC_FIR_SLIDE_OFFSET_CH3(n)            (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH3_MASK          (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH3_SHIFT         (9)
#define CODEC_FIR_BURST_LENGTH_CH3(n)            (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH3_MASK          (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH3_SHIFT         (15)
#define CODEC_FIR_GAIN_SEL_CH3(n)                (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH3_MASK              (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH3_SHIFT             (21)
#define CODEC_FIR_LOOP_NUM_CH3(n)                (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH3_MASK              (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH3_SHIFT             (25)

// reg_128
#define CODEC_AHB_IIR_ENABLE                     (1 << 0)
#define CODEC_DO_SAMPLE_INI                      (1 << 1)
#define CODEC_USE_SAMPLE_HIGH_HALF               (1 << 2)
#define CODEC_AHB_IIR_IF_EN                      (1 << 3)
#define CODEC_DMA_CTRL_RX_IIR                    (1 << 4)
#define CODEC_DMA_CTRL_TX_IIR                    (1 << 5)
#define CODEC_MODE_32BIT_IIR                     (1 << 6)
#define CODEC_MODE_16BIT_IIR                     (1 << 7)
#define CODEC_AHB_IIR_MAX_CNT(n)                 (((n) & 0xF) << 8)
#define CODEC_AHB_IIR_MAX_CNT_MASK               (0xF << 8)
#define CODEC_AHB_IIR_MAX_CNT_SHIFT              (8)
#define CODEC_IIR_MULTI_CYCLE_MODE(n)            (((n) & 0x3) << 12)
#define CODEC_IIR_MULTI_CYCLE_MODE_MASK          (0x3 << 12)
#define CODEC_IIR_MULTI_CYCLE_MODE_SHIFT         (12)

// reg_130
#define CODEC_CODEC_FB_CHECK_ENABLE_CH0          (1 << 0)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0(n) (((n) & 0x3) << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0_MASK (0x3 << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0_SHIFT (1)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0         (1 << 3)
#define CODEC_CODEC_FB_CHECK_KEEP_SEL_CH0        (1 << 4)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0(n)   (((n) & 0xFFF) << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0_MASK (0xFFF << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0_SHIFT (5)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0(n)  (((n) & 0x3FF) << 17)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0_MASK (0x3FF << 17)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0_SHIFT (17)
#define CODEC_CODEC_FB_CHECK_KEEP_CH0            (1 << 27)

// reg_134
#define CODEC_CODEC_FB_CHECK_ENABLE_CH1          (1 << 0)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH1(n) (((n) & 0x3) << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH1_MASK (0x3 << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH1_SHIFT (1)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH1         (1 << 3)
#define CODEC_CODEC_FB_CHECK_KEEP_SEL_CH1        (1 << 4)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH1(n)   (((n) & 0xFFF) << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH1_MASK (0xFFF << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH1_SHIFT (5)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH1(n)  (((n) & 0x3FF) << 17)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH1_MASK (0x3FF << 17)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH1_SHIFT (17)
#define CODEC_CODEC_FB_CHECK_KEEP_CH1            (1 << 27)

// reg_138
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0(n)    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0_MASK  (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0_SHIFT (0)

// reg_13c
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH1(n)    (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH1_MASK  (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH1_SHIFT (0)

// reg_140
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0(n) (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0_MASK (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0_SHIFT (0)

// reg_144
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH1(n) (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH1_MASK (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH1_SHIFT (0)

// reg_148
#define CODEC_VAD_EN                             (1 << 0)
#define CODEC_VAD_DS_BYPASS                      (1 << 1)
#define CODEC_VAD_DC_CANCEL_BYPASS               (1 << 2)
#define CODEC_VAD_PRE_BYPASS                     (1 << 3)
#define CODEC_VAD_DIG_MODE                       (1 << 4)
#define CODEC_VAD_FINISH                         (1 << 5)

// reg_14c
#define CODEC_VAD_U_DC(n)                        (((n) & 0xF) << 0)
#define CODEC_VAD_U_DC_MASK                      (0xF << 0)
#define CODEC_VAD_U_DC_SHIFT                     (0)
#define CODEC_VAD_U_PRE(n)                       (((n) & 0x7) << 4)
#define CODEC_VAD_U_PRE_MASK                     (0x7 << 4)
#define CODEC_VAD_U_PRE_SHIFT                    (4)
#define CODEC_VAD_FRAME_LEN(n)                   (((n) & 0xFF) << 7)
#define CODEC_VAD_FRAME_LEN_MASK                 (0xFF << 7)
#define CODEC_VAD_FRAME_LEN_SHIFT                (7)
#define CODEC_VAD_MVAD(n)                        (((n) & 0xF) << 15)
#define CODEC_VAD_MVAD_MASK                      (0xF << 15)
#define CODEC_VAD_MVAD_SHIFT                     (15)
#define CODEC_VAD_PRE_GAIN(n)                    (((n) & 0x3F) << 19)
#define CODEC_VAD_PRE_GAIN_MASK                  (0x3F << 19)
#define CODEC_VAD_PRE_GAIN_SHIFT                 (19)
#define CODEC_VAD_STH(n)                         (((n) & 0x3F) << 25)
#define CODEC_VAD_STH_MASK                       (0x3F << 25)
#define CODEC_VAD_STH_SHIFT                      (25)

// reg_150
#define CODEC_VAD_FRAME_TH1(n)                   (((n) & 0xFF) << 0)
#define CODEC_VAD_FRAME_TH1_MASK                 (0xFF << 0)
#define CODEC_VAD_FRAME_TH1_SHIFT                (0)
#define CODEC_VAD_FRAME_TH2(n)                   (((n) & 0x3FF) << 8)
#define CODEC_VAD_FRAME_TH2_MASK                 (0x3FF << 8)
#define CODEC_VAD_FRAME_TH2_SHIFT                (8)
#define CODEC_VAD_FRAME_TH3(n)                   (((n) & 0x3FFF) << 18)
#define CODEC_VAD_FRAME_TH3_MASK                 (0x3FFF << 18)
#define CODEC_VAD_FRAME_TH3_SHIFT                (18)

// reg_154
#define CODEC_VAD_RANGE1(n)                      (((n) & 0x1F) << 0)
#define CODEC_VAD_RANGE1_MASK                    (0x1F << 0)
#define CODEC_VAD_RANGE1_SHIFT                   (0)
#define CODEC_VAD_RANGE2(n)                      (((n) & 0x7F) << 5)
#define CODEC_VAD_RANGE2_MASK                    (0x7F << 5)
#define CODEC_VAD_RANGE2_SHIFT                   (5)
#define CODEC_VAD_RANGE3(n)                      (((n) & 0x1FF) << 12)
#define CODEC_VAD_RANGE3_MASK                    (0x1FF << 12)
#define CODEC_VAD_RANGE3_SHIFT                   (12)
#define CODEC_VAD_RANGE4(n)                      (((n) & 0x3FF) << 21)
#define CODEC_VAD_RANGE4_MASK                    (0x3FF << 21)
#define CODEC_VAD_RANGE4_SHIFT                   (21)

// reg_158
#define CODEC_VAD_PSD_TH1(n)                     (((n) & 0x7FFFFFF) << 0)
#define CODEC_VAD_PSD_TH1_MASK                   (0x7FFFFFF << 0)
#define CODEC_VAD_PSD_TH1_SHIFT                  (0)

// reg_15c
#define CODEC_VAD_PSD_TH2(n)                     (((n) & 0x7FFFFFF) << 0)
#define CODEC_VAD_PSD_TH2_MASK                   (0x7FFFFFF << 0)
#define CODEC_VAD_PSD_TH2_SHIFT                  (0)

// reg_160
#define CODEC_VAD_MEM_ADDR_CNT(n)                (((n) & 0x7FF) << 0)
#define CODEC_VAD_MEM_ADDR_CNT_MASK              (0x7FF << 0)
#define CODEC_VAD_MEM_ADDR_CNT_SHIFT             (0)
#define CODEC_VAD_MEM_DATA_CNT(n)                (((n) & 0x7FF) << 11)
#define CODEC_VAD_MEM_DATA_CNT_MASK              (0x7FF << 11)
#define CODEC_VAD_MEM_DATA_CNT_SHIFT             (11)

// reg_164
#define CODEC_SMIN_SYC(n)                        (((n) & 0x7FFFFFF) << 0)
#define CODEC_SMIN_SYC_MASK                      (0x7FFFFFF << 0)
#define CODEC_SMIN_SYC_SHIFT                     (0)

// reg_168
#define CODEC_PSD_SYC(n)                         (((n) & 0x7FFFFFF) << 0)
#define CODEC_PSD_SYC_MASK                       (0x7FFFFFF << 0)
#define CODEC_PSD_SYC_SHIFT                      (0)

// reg_170
#define CODEC_FIR_CH0_STATE(n)                   (((n) & 0xFF) << 0)
#define CODEC_FIR_CH0_STATE_MASK                 (0xFF << 0)
#define CODEC_FIR_CH0_STATE_SHIFT                (0)
#define CODEC_FIR_CH1_STATE(n)                   (((n) & 0xFF) << 8)
#define CODEC_FIR_CH1_STATE_MASK                 (0xFF << 8)
#define CODEC_FIR_CH1_STATE_SHIFT                (8)
#define CODEC_FIR_CH2_STATE(n)                   (((n) & 0xFF) << 16)
#define CODEC_FIR_CH2_STATE_MASK                 (0xFF << 16)
#define CODEC_FIR_CH2_STATE_SHIFT                (16)
#define CODEC_FIR_CH3_STATE(n)                   (((n) & 0xFF) << 24)
#define CODEC_FIR_CH3_STATE_MASK                 (0xFF << 24)
#define CODEC_FIR_CH3_STATE_SHIFT                (24)

// reg_174
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC(n)      (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC_MASK    (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC_SHIFT   (0)

// reg_178
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC(n)      (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC_MASK    (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC_SHIFT   (0)

// reg_17c
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC(n)      (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC_MASK    (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC_SHIFT   (0)

// reg_180
#define CODEC_CODEC_ADC_DC_DOUT_CH3_SYNC(n)      (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH3_SYNC_MASK    (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH3_SYNC_SHIFT   (0)

// reg_184
#define CODEC_CODEC_ADC_DC_DOUT_CH4_SYNC(n)      (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH4_SYNC_MASK    (0xFFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH4_SYNC_SHIFT   (0)

// reg_188
#define CODEC_CODEC_ADC_DC_DIN_CH0(n)            (((n) & 0x7FFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH0_MASK          (0x7FFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH0_SHIFT         (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH0            (1 << 15)

// reg_18c
#define CODEC_CODEC_ADC_DC_DIN_CH1(n)            (((n) & 0x7FFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH1_MASK          (0x7FFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH1_SHIFT         (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH1            (1 << 15)

// reg_190
#define CODEC_CODEC_ADC_DC_DIN_CH2(n)            (((n) & 0x7FFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH2_MASK          (0x7FFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH2_SHIFT         (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH2            (1 << 15)

// reg_194
#define CODEC_CODEC_ADC_DC_DIN_CH3(n)            (((n) & 0x7FFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH3_MASK          (0x7FFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH3_SHIFT         (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH3            (1 << 15)

// reg_198
#define CODEC_CODEC_ADC_DC_DIN_CH4(n)            (((n) & 0x3FFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH4_MASK          (0x3FFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH4_SHIFT         (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH4            (1 << 18)

// reg_1a0
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0(n)        (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_MASK      (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_SHIFT     (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1(n)        (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_MASK      (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_SHIFT     (14)

// reg_1a4
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2(n)        (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_MASK      (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_SHIFT     (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3(n)        (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_MASK      (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_SHIFT     (14)

// reg_1a8
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4(n)        (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_MASK      (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_SHIFT     (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5(n)        (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_MASK      (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_SHIFT     (14)

// reg_1ac
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6(n)        (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_MASK      (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_SHIFT     (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7(n)        (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_MASK      (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_SHIFT     (14)

// reg_1b0
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8(n)        (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_MASK      (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_SHIFT     (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9(n)        (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_MASK      (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_SHIFT     (14)

// reg_1b4
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10(n)       (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_MASK     (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_SHIFT    (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11(n)       (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_MASK     (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_SHIFT    (14)

// reg_1b8
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12(n)       (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_MASK     (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_SHIFT    (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13(n)       (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_MASK     (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_SHIFT    (14)

// reg_1bc
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14(n)       (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_MASK     (0x3FFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_SHIFT    (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15(n)       (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_MASK     (0x3FFF << 14)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_SHIFT    (14)

// reg_1c0
#define CODEC_CODEC_ADC_DRE_ENABLE_CH0           (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0(n)     (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0_MASK   (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0_SHIFT  (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0(n) (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0_MASK (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0_SHIFT (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0(n)  (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0_MASK (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0_SHIFT (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0(n)     (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0_MASK   (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0_SHIFT  (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0(n)     (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0_MASK   (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0_SHIFT  (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH0 (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0(n)       (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0_MASK     (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0_SHIFT    (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH0 (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH0        (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0(n) (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0_MASK (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0_SHIFT (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH0      (1 << 26)

// reg_1c4
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0(n)      (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0_MASK    (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0_SHIFT   (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0(n)        (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0_MASK      (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0_SHIFT     (11)

// reg_1c8
#define CODEC_CODEC_ADC_DRE_ENABLE_CH1           (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1(n)     (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1_MASK   (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1_SHIFT  (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1(n) (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1_MASK (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1_SHIFT (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1(n)  (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1_MASK (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1_SHIFT (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1(n)     (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1_MASK   (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1_SHIFT  (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1(n)     (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1_MASK   (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1_SHIFT  (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH1 (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1(n)       (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1_MASK     (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1_SHIFT    (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH1 (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH1        (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1(n) (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1_MASK (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1_SHIFT (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH1      (1 << 26)

// reg_1cc
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1(n)      (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1_MASK    (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1_SHIFT   (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1(n)        (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1_MASK      (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1_SHIFT     (11)

// reg_1d0
#define CODEC_CODEC_ADC_DRE_ENABLE_CH2           (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2(n)     (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2_MASK   (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2_SHIFT  (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2(n) (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2_MASK (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2_SHIFT (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2(n)  (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2_MASK (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2_SHIFT (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2(n)     (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2_MASK   (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2_SHIFT  (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2(n)     (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2_MASK   (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2_SHIFT  (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH2 (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2(n)       (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2_MASK     (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2_SHIFT    (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH2 (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH2        (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2(n) (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2_MASK (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2_SHIFT (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH2      (1 << 26)

// reg_1d4
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2(n)      (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2_MASK    (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2_SHIFT   (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2(n)        (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2_MASK      (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2_SHIFT     (11)

// reg_1d8
#define CODEC_CODEC_ADC_DRE_ENABLE_CH3           (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH3(n)     (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH3_MASK   (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH3_SHIFT  (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH3(n) (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH3_MASK (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH3_SHIFT (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3(n)  (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3_MASK (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3_SHIFT (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH3(n)     (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH3_MASK   (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH3_SHIFT  (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH3(n)     (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH3_MASK   (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH3_SHIFT  (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH3 (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH3(n)       (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH3_MASK     (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH3_SHIFT    (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH3 (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH3        (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH3(n) (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH3_MASK (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH3_SHIFT (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH3      (1 << 26)

// reg_1dc
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH3(n)      (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH3_MASK    (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH3_SHIFT   (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH3(n)        (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH3_MASK      (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH3_SHIFT     (11)

// reg_1e0
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0_SHIFT (14)

// reg_1e4
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0_SHIFT (14)

// reg_1e8
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0_SHIFT (14)

// reg_1ec
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0_SHIFT (14)

// reg_1f0
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0_SHIFT (14)

// reg_1f4
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0(n)   (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0_MASK (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0(n)   (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0_MASK (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0_SHIFT (14)

// reg_1f8
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0(n)   (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0_MASK (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0(n)   (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0_MASK (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0_SHIFT (14)

// reg_1fc
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0(n)   (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0_MASK (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0(n)   (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0_MASK (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0_SHIFT (14)

// reg_200
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1_SHIFT (14)

// reg_204
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1_SHIFT (14)

// reg_208
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1_SHIFT (14)

// reg_20c
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1_SHIFT (14)

// reg_210
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1(n)    (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1_MASK  (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1(n)    (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1_MASK  (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1_SHIFT (14)

// reg_214
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1(n)   (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1_MASK (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1(n)   (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1_MASK (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1_SHIFT (14)

// reg_218
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1(n)   (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1_MASK (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1(n)   (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1_MASK (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1_SHIFT (14)

// reg_21c
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1(n)   (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1_MASK (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1_SHIFT (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1(n)   (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1_MASK (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1_SHIFT (14)

// reg_220
#define CODEC_VAD_DELAY1(n)                      (((n) & 0x3FF) << 0)
#define CODEC_VAD_DELAY1_MASK                    (0x3FF << 0)
#define CODEC_VAD_DELAY1_SHIFT                   (0)

// reg_224
#define CODEC_VAD_DELAY2(n)                      (((n) & 0xFFFFFF) << 0)
#define CODEC_VAD_DELAY2_MASK                    (0xFFFFFF << 0)
#define CODEC_VAD_DELAY2_SHIFT                   (0)

#endif
