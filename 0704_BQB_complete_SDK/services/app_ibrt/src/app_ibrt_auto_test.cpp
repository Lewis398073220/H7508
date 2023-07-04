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
#include <string.h>
#include "app_tws_ibrt_trace.h"
#include "factory_section.h"
#include "apps.h"
#include "app_battery.h"
#include "app_anc.h"
#include "app_key.h"
#include "app_ibrt_auto_test.h"
#include "app_ibrt_if.h"
#include "app_ibrt_ui_test.h"
#include "app_ibrt_peripheral_manager.h"
#include "a2dp_decoder.h"
#include "app_ibrt_keyboard.h"
#include "nvrecord_env.h"
#include "nvrecord_ble.h"
#include "app_tws_if.h"
#include "besbt.h"
#include "app_bt.h"
#include "app_ai_if.h"
#include "app_ai_manager_api.h"


#if defined(VOICE_DATAPATH)
// #include "app_voicepath.h"
#endif

#if defined(IBRT)
#include "btapp.h"
extern struct BT_DEVICE_T  app_bt_device;

/*****************************************************************************
 Prototype    : app_ibrt_auto_test_voice_promt_check_handle
 Description  : check if voice promt test cmd has done
 Input        : uint8_t operation_code
              : uint8_t *param
              : uint8_t param_len
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2020/3/3
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_voice_promt_check_handle(uint8_t group_code,
        uint8_t operation_code,
        uint8_t *param,
        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        default:
            break;
    }
}

/*****************************************************************************
Prototype    : app_ibrt_auto_test_a2dp_check_handle
Description  : check if a2dp test cmd has done
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_a2dp_check_handle(uint8_t group_code,
                                        uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);

    switch (operation_code)
    {
        case A2DP_AUTO_TEST_AUDIO_PLAY:
            break;
        case A2DP_AUTO_TEST_AUDIO_PAUSE:
            break;
        case A2DP_AUTO_TEST_AUDIO_FORWARD:
            break;
        case A2DP_AUTO_TEST_AUDIO_BACKWARD:
            break;
        case A2DP_AUTO_TEST_AVRCP_VOL_UP:
            break;
        case A2DP_AUTO_TEST_AVRCP_VOL_DOWN:
            break;
        default:
            break;
    }
}

/*****************************************************************************
Prototype    : app_ibrt_auto_test_hfp_check_handle
Description  : check if hfp test cmd has done
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_hfp_check_handle(uint8_t group_code,
                                        uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);

    switch (operation_code)
    {
        case HFP_AUTO_TEST_SCO_CREATE:
            break;
        case HFP_AUTO_TEST_SCO_DISC:
            break;
        case HFP_AUTO_TEST_CALL_REDIAL:
            break;
        case HFP_AUTO_TEST_CALL_ANSWER:
            break;
        case HFP_AUTO_TEST_CALL_HANGUP:
            break;
        case HFP_AUTO_TEST_VOLUME_UP:
            break;
        case HFP_AUTO_TEST_VOLUME_DOWN:
            break;
        default:
            break;
    }
}
/*****************************************************************************
Prototype    : app_ibrt_auto_test_ui_check_handle
Description  : check if UI test cmd has done
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_ui_check_handle(uint8_t group_code,
                                        uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    app_ibrt_ui_t *p_ibrt_ui = app_ibrt_ui_get_ctx();

    switch (operation_code)
    {
        case UI_AUTO_TEST_OPEN_BOX:
            if (p_ibrt_ui->super_state == IBRT_UI_IDLE &&
                p_ibrt_ui->active_event == IBRT_NONE_EVENT &&
                p_ibrt_ui->ibrt_sm_running == false &&
                p_ibrt_ui->box_state == IBRT_IN_BOX_OPEN)
            {
                AUTO_TEST_TRACE(2, "AUTO_TEST_CMD received:%d:%d:", group_code, operation_code);
            }
            break;
        case UI_AUTO_TEST_CLOSE_BOX:
            if (p_ibrt_ui->super_state == IBRT_UI_IDLE &&
                p_ibrt_ui->active_event == IBRT_NONE_EVENT &&
                p_ibrt_ui->ibrt_sm_running == false &&
                p_ibrt_ui->box_state == IBRT_IN_BOX_CLOSED)
            {
                AUTO_TEST_TRACE(2, "AUTO_TEST_CMD received:%d:%d:", group_code, operation_code);
            }
            break;
        case UI_AUTO_TEST_FETCH_OUT_BOX:
            if (p_ibrt_ui->super_state == IBRT_UI_IDLE &&
                p_ibrt_ui->active_event == IBRT_NONE_EVENT &&
                p_ibrt_ui->ibrt_sm_running == false &&
                p_ibrt_ui->box_state == IBRT_OUT_BOX)
            {
                AUTO_TEST_TRACE(2, "AUTO_TEST_CMD received:%d:%d:", group_code, operation_code);
            }
            break;
        case UI_AUTO_TEST_PUT_IN_BOX:
            if (p_ibrt_ui->super_state == IBRT_UI_IDLE &&
                p_ibrt_ui->active_event == IBRT_NONE_EVENT &&
                p_ibrt_ui->ibrt_sm_running == false &&
                p_ibrt_ui->box_state == IBRT_IN_BOX_OPEN)
            {
                AUTO_TEST_TRACE(2, "AUTO_TEST_CMD received:%d:%d:", group_code, operation_code);
            }
            break;
        case UI_AUTO_TEST_WEAR_UP:
            if (p_ibrt_ui->super_state == IBRT_UI_IDLE &&
                p_ibrt_ui->active_event == IBRT_NONE_EVENT &&
                p_ibrt_ui->ibrt_sm_running == false &&
                p_ibrt_ui->box_state == IBRT_OUT_BOX_WEARED)
            {
                AUTO_TEST_TRACE(2, "AUTO_TEST_CMD received:%d:%d:", group_code, operation_code);
            }
            break;
        case UI_AUTO_TEST_WEAR_DOWN:
            if (p_ibrt_ui->super_state == IBRT_UI_IDLE &&
                p_ibrt_ui->active_event == IBRT_NONE_EVENT &&
                p_ibrt_ui->ibrt_sm_running == false &&
                p_ibrt_ui->box_state == IBRT_OUT_BOX)
            {
                AUTO_TEST_TRACE(2, "AUTO_TEST_CMD received:%d:%d:", group_code, operation_code);
            }
            break;
        case UI_AUTO_TEST_ROLE_SWITCH:
            break;
        case UI_AUTO_TEST_PHONE_CONN_EVENT:
            break;
        case UI_AUTO_TEST_RECONN_EVENT:
            break;
        case UI_AUTO_TEST_CONN_SECOND_MOBILE:
            break;
        case UI_AUTO_TEST_MOBILE_TWS_DISC:
            break;
        case UI_AUTO_TEST_PAIRING_MODE:
            break;
        case UI_AUTO_TEST_FREEMAN_MODE:
            break;
        case UI_AUTO_TEST_SUSPEND_IBRT:
            break;
        case UI_AUTO_TEST_RESUME_IBRT:
            break;
        case UI_AUTO_TEST_SHUT_DOWN:
            break;
        default:
            break;
    }
}
/*****************************************************************************
Prototype    : app_ibrt_auto_test_ai_check_handle
Description  : check if AI test cmd has done
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_ai_check_handle(uint8_t group_code,
                                        uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        default:
            break;
    }
}

/*****************************************************************************
Prototype    : app_ibrt_auto_test_ble_check_handle
Description  : check if ble test cmd has done
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_ble_check_handle(uint8_t group_code,
                                        uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        default:
            break;
    }
}
/*****************************************************************************
Prototype    : app_ibrt_auto_test_flash_check_handle
Description  : check if flash test cmd has done
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_auto_test_flash_check_handle(uint8_t group_code,
                                        uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        default:
            break;
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_automate_test_check_handler
 Description  : ibrt automate test check if cmd has done
 Input        : uint8_t group_code
              : uint8_t operation_code
              : uint8_t *param
              : uint8_t param_len
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2020/3/3
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
extern "C" void app_ibrt_automate_test_check_handler(uint8_t group_code,
        uint8_t operation_code,
        uint8_t *param,
        uint8_t param_len)
{
    switch (group_code)
    {
        case AUTO_TEST_VOICE_PROMPT:
            app_ibrt_auto_test_voice_promt_check_handle(group_code, operation_code, param, param_len);
            break;
        case AUTO_TEST_A2DP:
            app_ibrt_auto_test_a2dp_check_handle(group_code, operation_code, param, param_len);
            break;
        case AUTO_TEST_HFP:
            app_ibrt_auto_test_hfp_check_handle(group_code, operation_code, param, param_len);
            break;
        case AUTO_TEST_UI:
            app_ibrt_auto_test_ui_check_handle(group_code, operation_code, param, param_len);
            break;
        case AUTO_TEST_AI:
            app_ibrt_auto_test_ai_check_handle(group_code, operation_code, param, param_len);
            break;
        case AUTO_TEST_BLE:
            app_ibrt_auto_test_ble_check_handle(group_code, operation_code, param, param_len);
            break;
        case AUTO_TEST_FLASH:
            app_ibrt_auto_test_flash_check_handle(group_code, operation_code, param, param_len);
            break;
        default:
            break;
    }
}

#endif
