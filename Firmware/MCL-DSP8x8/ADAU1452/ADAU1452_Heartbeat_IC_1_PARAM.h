/*
 * File:           C:\Users\loral\OneDrive\Documents\EE4893\MCL-DSP8x8\Firmware\MCL-DSP8x8\ADAU1452\ADAU1452_Heartbeat_IC_1_PARAM.h
 *
 * Created:        Saturday, March 21, 2026 10:06:24 PM
 * Description:    ADAU1452_Heartbeat:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2026 Analog Devices, Inc. All rights reserved.
 */
#ifndef __ADAU1452_HEARTBEAT_IC_1_PARAM_H__
#define __ADAU1452_HEARTBEAT_IC_1_PARAM_H__


/* Module SafeLoadModule - SafeLoadModule*/
#define MOD_SAFELOADMODULE_COUNT                       10
#define MOD_SAFELOADMODULE_DEVICE                      "IC1"
#define MOD_SAFELOADMODULE_DATA_SAFELOAD0_ADDR         24576
#define MOD_SAFELOADMODULE_DATA_SAFELOAD1_ADDR         24577
#define MOD_SAFELOADMODULE_DATA_SAFELOAD2_ADDR         24578
#define MOD_SAFELOADMODULE_DATA_SAFELOAD3_ADDR         24579
#define MOD_SAFELOADMODULE_DATA_SAFELOAD4_ADDR         24580
#define MOD_SAFELOADMODULE_ADDRESS_SAFELOAD_ADDR       24581
#define MOD_SAFELOADMODULE_NUM_SAFELOAD_ADDR           24582

/* Module Pulse1 - Pulse with dynamic duty cycle*/
#define MOD_LED_HEARTBEAT_PULSE1_COUNT                 4
#define MOD_LED_HEARTBEAT_PULSE1_DEVICE                "IC1"
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_ISROUNDED_ADDR   20
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_ISROUNDED_VALUE  SIGMASTUDIOTYPE_8_24_CONVERT(0)
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_ISROUNDED_TYPE   SIGMASTUDIOTYPE_8_24
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_FREQ_STEP_ADDR   21
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_FREQ_STEP_VALUE  SIGMASTUDIOTYPE_8_24_CONVERT(4.16666666666667E-05)
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_FREQ_STEP_TYPE   SIGMASTUDIOTYPE_8_24
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_ON_ADDR          22
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_ON_VALUE         SIGMASTUDIOTYPE_8_24_CONVERT(1)
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_ON_TYPE          SIGMASTUDIOTYPE_8_24
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_TH_ADDR          23
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_TH_VALUE         SIGMASTUDIOTYPE_8_24_CONVERT(0.5)
#define MOD_LED_HEARTBEAT_PULSE1_ALG0_TH_TYPE          SIGMASTUDIOTYPE_8_24

#endif
