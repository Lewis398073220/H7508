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
#ifndef __APP_IBRT_UI_TEST_H__
#define __APP_IBRT_UI_TEST_H__
#include <stdint.h>

#if defined(IBRT)
#define  IBRT_OPEN_BOX_TEST_EVENT       (1)
#define  IBRT_FETCH_OUT_TEST_EVENT      (2)
#define  IBRT_PUT_IN_TEST_EVENT         (3)
#define  IBRT_CLOSE_BOX_TEST_EVENT      (4)

#define  IBRT_WEAR_UP_TEST_EVENT        (5)
#define  IBRT_WEAR_DOWN_TEST_EVENT      (6)
#define  IBRT_RECONNECT_TEST_EVENT      (7)
#define  IBRT_PHONE_CONNECT_TEST_EVENT  (8)

//automate test group code
#define AUTO_TEST_CONNECT       0
#define AUTO_TEST_A2DP          1
#define AUTO_TEST_HFP           2
#define AUTO_TEST_BLE           3

#define AUTO_TEST_ROLE_SWITCH   20
#define AUTO_TEST_UI            21
#define AUTO_TEST_VOICE_PROMPT  22

#define AUTO_TEST_AI            30
#define AUTO_TEST_FLASH         31
#define AUTO_TEST_OTA           32

//voice promt automate test operation code

// A2DP automate test operation code, configure the respective
// .ini file used for auto test script
#define A2DP_AUTO_TEST_AUDIO_PLAY       0
#define A2DP_AUTO_TEST_AUDIO_PAUSE      1
#define A2DP_AUTO_TEST_AUDIO_FORWARD    2
#define A2DP_AUTO_TEST_AUDIO_BACKWARD   3
#define A2DP_AUTO_TEST_AVRCP_VOL_UP     4
#define A2DP_AUTO_TEST_AVRCP_VOL_DOWN   5

// HFP automate test operation code, configure the respective
// .ini file used for auto test script
#define HFP_AUTO_TEST_SCO_CREATE        0
#define HFP_AUTO_TEST_SCO_DISC          1
#define HFP_AUTO_TEST_CALL_REDIAL       2
#define HFP_AUTO_TEST_CALL_ANSWER       3
#define HFP_AUTO_TEST_CALL_HANGUP       4
#define HFP_AUTO_TEST_VOLUME_UP         5
#define HFP_AUTO_TEST_VOLUME_DOWN       6

// UI automate test operation code , configure the respective
// .ini file used for auto test script
#define UI_AUTO_TEST_OPEN_BOX           0
#define UI_AUTO_TEST_CLOSE_BOX          1
#define UI_AUTO_TEST_FETCH_OUT_BOX      2
#define UI_AUTO_TEST_PUT_IN_BOX         3
#define UI_AUTO_TEST_WEAR_UP            4
#define UI_AUTO_TEST_WEAR_DOWN          5
#define UI_AUTO_TEST_ROLE_SWITCH        6
#define UI_AUTO_TEST_PHONE_CONN_EVENT   7
#define UI_AUTO_TEST_RECONN_EVENT       8
#define UI_AUTO_TEST_CONN_SECOND_MOBILE 9
#define UI_AUTO_TEST_MOBILE_TWS_DISC    10
#define UI_AUTO_TEST_PAIRING_MODE       11
#define UI_AUTO_TEST_FREEMAN_MODE       12
#define UI_AUTO_TEST_SUSPEND_IBRT       13
#define UI_AUTO_TEST_RESUME_IBRT        14
#define UI_AUTO_TEST_SHUT_DOWN          15

// AI automate test operation code, configure the respective
// .ini file used for auto test script

// BLE automate test operation code, configure the respective
// .ini file used for auto test script

// flash automate test operation code, configure the respective
// .ini file used for auto test script

typedef void (*app_uart_test_function_handle)(void);

typedef struct
{
    const char* string;
    app_uart_test_function_handle function;
} app_uart_handle_t;

app_uart_test_function_handle app_ibrt_ui_find_uart_handle(unsigned char* buf);

void app_ibrt_ui_test_key_init(void);

void app_ibrt_ui_test_init(void);

int app_ibrt_ui_test_config_load(void *config);

void app_ibrt_ui_sync_anc_status(uint8_t *buf, uint32_t len);

void app_ibrt_ui_test_voice_assistant_key(APP_KEY_STATUS *status, void *param);

void app_ibrt_search_ui_gpio_key_handle(APP_KEY_STATUS *status, void *param);
#endif
#endif
