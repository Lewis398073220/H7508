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

#include "hal_aud.h"
#include "hal_chipid.h"
#include "hal_trace.h"
#include "apps.h"
#include "app_thread.h"
#include "app_status_ind.h"
#include "bluetooth.h"
#include "nvrecord.h"
#include "besbt.h"
#include "besbt_cfg.h"
#include "me_api.h"
#include "mei_api.h"
#include "a2dp_api.h"
#include "hci_api.h"
#include "l2cap_api.h"
#include "hfp_api.h"
#include "dip_api.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_hfp.h"
#include "app_bt_func.h"
#include "bt_drv_interface.h"
#include "os_api.h"
#include "bt_drv_reg_op.h"
#include "app_a2dp.h"
#include "app_dip.h"
#include "app_ai_manager_api.h"
#include "me_api.h"
#ifdef BTIF_HID_DEVICE
#include "app_bt_hid.h"
#endif

#if defined(IBRT)
#include "app_tws_ibrt.h"
#include "app_ibrt_ui.h"
#include "app_ibrt_if.h"
#endif

#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif

#include "app_ble_mode_switch.h"

#ifdef GFPS_ENABLED
#include "app_gfps.h"
#endif

#ifdef __AI_VOICE__
#include "ai_spp.h"
#include "ai_thread.h"
#include "ai_manager.h"
#endif

#ifdef __INTERCONNECTION__
#include "app_interconnection.h"
#include "app_interconnection_ble.h"
#include "spp_api.h"
#include "app_interconnection_ccmp.h"
#include "app_spp.h"
#include "app_interconnection_spp.h"
#endif

#ifdef __INTERACTION__
#include "app_interaction.h"
#endif

#ifdef BISTO_ENABLED
#include "gsound_custom_bt.h"
#endif

#if defined(__EARPHONE_STAY_BCR_SLAVE__) && defined(__BT_ONE_BRING_TWO__)
#error can not defined at the same time.
#endif

#ifdef GFPS_ENABLED
#include "app_gfps.h"
#endif

#ifdef TILE_DATAPATH
#include "tile_target_ble.h"
#include "rwip_config.h"
#endif
extern "C"
{
#include "ddbif.h"
}

/** add by pang **/
#if defined(__USE_3_5JACK_CTR__)
#include "app_user.h"
#include "app_bt_media_manager.h"
#endif
#include "app_audio.h"
#include "app_bt_stream.h"
#include "app.h"
#include "bt_drv_reg_op.h"

bool lostconncection_to_pairing=0;
bool lacal_bt_off=0;
static bool tx_pwr_for_page_flag=1;
bool power_on_open_reconnect_flag=0;

extern uint8_t app_poweroff_flag;
extern bool factory_reset_flag;

uint8_t remote_dev_name[10]={0};
uint8_t dev_name_user[BT_DEVICE_NUM][100] = {0};
uint8_t cur_device_id=BT_DEVICE_ID_1;

void app_cur_connect_devid_set(uint8_t id, uint8_t connect)
{
	if(connect == true) {
		cur_device_id = id;
	} else{
		memset(dev_name_user[id], 0, sizeof(dev_name_user[id]));
		cur_device_id = (id==BT_DEVICE_ID_1? BT_DEVICE_ID_2 : BT_DEVICE_ID_1);
	}
}

uint8_t app_cur_connect_devid_get(void)
{
	TRACE(2,"***%s: cur_device_id=%d",__func__,cur_device_id);
	return cur_device_id;
}

void app_dev_name_get(uint8_t dev_na[100])
{
	if(strlen((const char *)dev_name_user[app_cur_connect_devid_get()]) != 0)
	{
		memcpy(dev_na, dev_name_user[app_cur_connect_devid_get()], sizeof(dev_name_user[app_cur_connect_devid_get()]));
	}
}

static void reconnect_timeout_set(uint8_t rect);
static void reconnect_timeout_stop(void);
/** end add **/

extern struct BT_DEVICE_T  app_bt_device;
extern "C" bool app_anc_work_status(void);
extern uint8_t avrcp_get_media_status(void);
void avrcp_set_media_status(uint8_t status);

static btif_remote_device_t *connectedMobile = NULL;
static btif_remote_device_t *sppOpenMobile = NULL;

U16 bt_accessory_feature_feature = BTIF_HF_CUSTOM_FEATURE_SUPPORT;

#ifdef __BT_ONE_BRING_TWO__
btif_device_record_t record2_copy;
uint8_t record2_avalible;
#endif

enum bt_profile_reconnect_mode
{
    bt_profile_reconnect_null,
    bt_profile_reconnect_openreconnecting,
    bt_profile_reconnect_reconnecting,
    bt_profile_reconnect_reconnect_pending,
};

enum bt_profile_connect_status{
    bt_profile_connect_status_unknow,
    bt_profile_connect_status_success,
    bt_profile_connect_status_failure,
};

struct app_bt_profile_manager{
    bool has_connected;
    bool isEncrypted;
    enum bt_profile_connect_status hfp_connect;
    enum bt_profile_connect_status hsp_connect;
    enum bt_profile_connect_status a2dp_connect;
    bt_bdaddr_t rmt_addr;
    bt_profile_reconnect_mode reconnect_mode;
    bt_profile_reconnect_mode saved_reconnect_mode;
    a2dp_stream_t *stream;
    //HfChannel *chan;
    hf_chan_handle_t chan;
#if defined (__HSP_ENABLE__)
    HsChannel * hs_chan;
#endif
    uint16_t reconnect_cnt;
    osTimerId connect_timer;
    void (* connect_timer_cb)(void const *);
     
    APP_BT_CONNECTING_STATE_E connectingState;
};
/*====================================================================================================
08 disconnect reconnect time = (RECONNECT_RETRY_INTERVAL_MS+PAGETO)*RECONNECT_RETRY_LIMIT_CNT
                             = (3000ms+5000ms)*15
                             = 120s
------------------------------------------------------------------------------------------------------                             
openning reconnect time      = (RECONNECT_RETRY_INTERVAL_MS+PAGETO)*OPENNING_RECONNECT_RETRY_LIMIT_CNT
                             = (3000ms+5000ms)*2
                             = 16s
======================================================================================================*/
#define APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS (3000)
#define APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT   (15)
#define APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT (50)//15
#define APP_BT_PROFILE_CONNECT_RETRY_MS (10000)

static struct app_bt_profile_manager bt_profile_manager[BT_DEVICE_NUM];

bt_bdaddr_t* get_bt_profile_manager_rmt_addr_by_device_id(BT_DEVICE_ID_T bt_device){
    return &bt_profile_manager[bt_device].rmt_addr;
}

static int8_t app_bt_profile_reconnect_pending_process(void);
void app_bt_connectable_mode_stop_reconnecting(void);

btif_accessible_mode_t g_bt_access_mode = BTIF_BAM_NOT_ACCESSIBLE;

#define APP_BT_PROFILE_BOTH_SCAN_MS (11000)
#define APP_BT_PROFILE_PAGE_SCAN_MS (4000)

osTimerId app_bt_accessmode_timer = NULL;
btif_accessible_mode_t app_bt_accessmode_timer_argument = BTIF_BAM_NOT_ACCESSIBLE;
static int app_bt_accessmode_timehandler(void const *param);
osTimerDef (APP_BT_ACCESSMODE_TIMER, (void (*)(void const *))app_bt_accessmode_timehandler);                      // define timers

#if !defined(IBRT)
static void app_bt_handling_on_abnormal_disconnection(enum BT_DEVICE_ID_T id, 
    bt_bdaddr_t* remDevAddr, uint8_t disc_error);
static bool app_bt_is_no_profiles_ever_connected(uint8_t devId);
#endif

#define A2DP_CONN_CLOSED   10

#ifdef __IAG_BLE_INCLUDE__
#define APP_FAST_BLE_ADV_TIMEOUT_IN_MS 30000
osTimerId app_fast_ble_adv_timeout_timer = NULL;
static int app_fast_ble_adv_timeout_timehandler(void const *param);
osTimerDef(APP_FAST_BLE_ADV_TIMEOUT_TIMER, ( void (*)(void const *) )app_fast_ble_adv_timeout_timehandler);  // define timers

/*---------------------------------------------------------------------------
 *            app_start_fast_connectable_ble_adv
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    start fast connectable BLE adv
 *
 * Parameters:
 *    advInterval - adv interval
 *
 * Return:
 *    void
 */
static void app_start_fast_connectable_ble_adv(uint16_t advInterval);
#endif


btif_accessible_mode_t app_bt_get_current_access_mode(void)
{
    return g_bt_access_mode;
}

#if 1//oen by pang   #if defined(__INTERCONNECTION__)
bool app_bt_is_connected()
{
    uint8_t i=0;
    bool connceted_value=false;
    for(i=0;i<BT_DEVICE_NUM;i++)
    {
        if(bt_profile_manager[i].has_connected)
        {
            connceted_value = true;
            break;
        }
    }

    TRACE(1,"bt_is_connected is %d", connceted_value);
    return connceted_value;
}
#endif

static void app_bt_precheck_before_starting_connecting(uint8_t isBtConnected);

static void app_bt_accessmode_handler(btif_accessible_mode_t accMode)
{
    const btif_access_mode_info_t info = { BTIF_BT_DEFAULT_INQ_SCAN_INTERVAL,
                                    BTIF_BT_DEFAULT_INQ_SCAN_WINDOW,
                                    BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL,
                                    BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW };

    osapi_lock_stack();
    if (accMode == BTIF_BAM_CONNECTABLE_ONLY){
        app_bt_accessmode_timer_argument = BTIF_BAM_GENERAL_ACCESSIBLE;
        osTimerStart(app_bt_accessmode_timer, APP_BT_PROFILE_PAGE_SCAN_MS);
    }else if(accMode == BTIF_BAM_GENERAL_ACCESSIBLE){
        app_bt_accessmode_timer_argument = BTIF_BAM_CONNECTABLE_ONLY;
        osTimerStart(app_bt_accessmode_timer, APP_BT_PROFILE_BOTH_SCAN_MS);
    }
    app_bt_ME_SetAccessibleMode(accMode, &info);
    TRACE(1,"app_bt_accessmode_timehandler accMode=%x",accMode);
    osapi_unlock_stack();
}

static int app_bt_accessmode_timehandler(void const *param)
{
    btif_accessible_mode_t accMode = *(btif_accessible_mode_t*)(param);
    app_bt_start_custom_function_in_bt_thread((uint32_t)accMode,
        0, (uint32_t)app_bt_accessmode_handler);
    return 0;
}


void app_bt_accessmode_set(btif_accessible_mode_t mode)
{
    const btif_access_mode_info_t info = { BTIF_BT_DEFAULT_INQ_SCAN_INTERVAL,
                                    BTIF_BT_DEFAULT_INQ_SCAN_WINDOW,
                                    BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL,
                                    BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW };
#if defined(IBRT)
    return;
#endif
    TRACE(2,"%s %d",__func__, mode);
    osapi_lock_stack();
    g_bt_access_mode = mode;
    if (g_bt_access_mode == BTIF_BAM_GENERAL_ACCESSIBLE){
        app_bt_accessmode_timehandler(&g_bt_access_mode);
    }else{
        osTimerStop(app_bt_accessmode_timer);
        app_bt_ME_SetAccessibleMode(g_bt_access_mode, &info);
        TRACE(1,"app_bt_accessmode_set access_mode=%x",g_bt_access_mode);
    }
    osapi_unlock_stack();
}


#ifdef FPGA
void app_bt_accessmode_set_for_test(btif_accessible_mode_t mode)
{
    const btif_access_mode_info_t info = { BTIF_BT_DEFAULT_INQ_SCAN_INTERVAL,
                                    BTIF_BT_DEFAULT_INQ_SCAN_WINDOW,
                                    BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL,
                                    BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW };

    TRACE(2,"%s %d",__func__, mode);
    app_bt_ME_SetAccessibleMode_Fortest(mode, &info);
}

void app_bt_adv_mode_set_for_test(uint8_t en)
{

    TRACE(2,"%s %d",__func__, en);
    app_bt_ME_Set_Advmode_Fortest(en);
}

void app_start_ble_adv_for_test(void)
{
    TRACE(1,"%s",__func__);

    U8 adv_data[31];
    U8 adv_data_len = 0;
    U8 scan_rsp_data[31];
    U8 scan_rsp_data_len = 0;

    adv_data[adv_data_len++] = 2;
    adv_data[adv_data_len++] = 0x01;
    adv_data[adv_data_len++] = 0x1A;

    adv_data[adv_data_len++] = 17;
    adv_data[adv_data_len++] = 0xFF;

    adv_data[adv_data_len++] = 0x9A;
    adv_data[adv_data_len++] = 0x07;

    adv_data[adv_data_len++] = 0x10;
    adv_data[adv_data_len++] = 0x04;
    adv_data[adv_data_len++] = 0x06;

    adv_data[adv_data_len++] = 0x07;
    adv_data[adv_data_len++] = 0x00;

    adv_data[adv_data_len++] = 0x01;
    adv_data[adv_data_len++] = 0x98;

    adv_data[adv_data_len++] = 1;

    adv_data[adv_data_len++] = 0xFF;
    adv_data[adv_data_len++] = 0xFF;
    adv_data[adv_data_len++] = 0xFF;
    adv_data[adv_data_len++] = 0xFF;
    adv_data[adv_data_len++] = 0xFF;
    adv_data[adv_data_len++] = 0xFF;

    // Get default Device Name (No name if not enough space)
    const char* ble_name_in_nv = BLE_DEFAULT_NAME;
    uint32_t nameLen = strlen(ble_name_in_nv);
    // Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
    int8_t avail_space = 31-adv_data_len;
    if(avail_space - 2 >= (int8_t)nameLen)
    {
        // Check if data can be added to the adv Data
        adv_data[adv_data_len++] = nameLen + 1;
        // Fill Device Name Flag
        adv_data[adv_data_len++] = '\x09';
        // Copy device name
        memcpy(&adv_data[adv_data_len], ble_name_in_nv, nameLen);
        // Update adv Data Length
        adv_data_len += nameLen;
        btif_me_ble_set_adv_data(adv_data_len, adv_data);

        btif_adv_para_struct_t adv_para;
        adv_para.interval_min = 32;
        adv_para.interval_max = 32;
        adv_para.adv_type = 0;
        adv_para.own_addr_type = 0;
        adv_para.peer_addr_type = 0;
        adv_para.adv_chanmap = 0x07;
        adv_para.adv_filter_policy = 0;
        btif_me_ble_set_adv_parameters(&adv_para);
        btif_me_set_ble_bd_address(ble_addr);
        btif_me_ble_set_adv_en(1);
    }
    else
    {
        nameLen = avail_space - 2;
        // Check if data can be added to the adv Data
        adv_data[adv_data_len++] = nameLen + 1;
        // Fill Device Name Flag
        adv_data[adv_data_len++] = '\x08';
        // Copy device name
        memcpy(&adv_data[adv_data_len], ble_name_in_nv, nameLen);
        // Update adv Data Length
        adv_data_len += nameLen;
        btif_me_ble_set_adv_data(adv_data_len, adv_data);

        btif_adv_para_struct_t adv_para;
        adv_para.interval_min = 256;
        adv_para.interval_max = 256;
        adv_para.adv_type = 0;
        adv_para.own_addr_type = 0;
        adv_para.peer_addr_type = 0;
        adv_para.adv_chanmap = 0x07;
        adv_para.adv_filter_policy = 0;
        btif_me_ble_set_adv_parameters(&adv_para);
        btif_me_set_ble_bd_address(ble_addr);

        avail_space = 31;
        nameLen = strlen(ble_name_in_nv);
        if(avail_space - 2 < (int8_t)nameLen)
            nameLen = avail_space - 2;

        scan_rsp_data[scan_rsp_data_len++] = nameLen + 1;
        // Fill Device Name Flag
        scan_rsp_data[scan_rsp_data_len++] = '\x09';
        // Copy device name
        memcpy(&scan_rsp_data[scan_rsp_data_len], ble_name_in_nv, nameLen);
        // Update Scan response Data Length
        scan_rsp_data_len += nameLen;
        btif_me_ble_set_scan_rsp_data(scan_rsp_data_len, scan_rsp_data);
        btif_me_ble_set_adv_en(1);
    }
}

void app_bt_write_controller_memory_for_test(uint32_t addr,uint32_t val,uint8_t type)
{
    TRACE(2,"%s addr=0x%x val=0x%x type=%d",__func__, addr,val,type);
    app_bt_ME_Write_Controller_Memory_Fortest(addr,val,type);
}

void app_bt_read_controller_memory_for_test(uint32_t addr,uint32_t len,uint8_t type)
{
    TRACE(2,"%s addr=0x%x len=%x type=%d",__func__, addr,len,type);
    app_bt_ME_Read_Controller_Memory_Fortest(addr,len,type);
}
#endif

extern "C" uint8_t app_bt_get_act_cons(void)
{
    int activeCons;

    osapi_lock_stack();
    activeCons = btif_me_get_activeCons();
    osapi_unlock_stack();
    TRACE(2,"%s %d",__func__,activeCons);
    return activeCons;
}
enum{
    INITIATE_PAIRING_NONE = 0,
    INITIATE_PAIRING_RUN = 1,
};
static uint8_t initiate_pairing = INITIATE_PAIRING_NONE;
void app_bt_connectable_state_set(uint8_t set)
{
    initiate_pairing = set;
}
bool is_app_bt_pairing_running(void)
{
    return (initiate_pairing == INITIATE_PAIRING_RUN)?(true):(false);
}
#define APP_DISABLE_PAGE_SCAN_AFTER_CONN
#ifdef APP_DISABLE_PAGE_SCAN_AFTER_CONN
osTimerId disable_page_scan_check_timer = NULL;
static void disable_page_scan_check_timer_handler(void const *param);
osTimerDef (DISABLE_PAGE_SCAN_CHECK_TIMER, (void (*)(void const *))disable_page_scan_check_timer_handler);                      // define timers
static void disable_page_scan_check_timer_handler(void const *param)
{
#ifdef __BT_ONE_BRING_TWO__
    if((btif_me_get_activeCons() > 1) && (initiate_pairing == INITIATE_PAIRING_NONE)){
#else
    if((btif_me_get_activeCons() > 0) && (initiate_pairing == INITIATE_PAIRING_NONE)){
#endif
        app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
    }
}

static void disable_page_scan_check_timer_start(void)
{
    if(disable_page_scan_check_timer == NULL){
        disable_page_scan_check_timer = osTimerCreate(osTimer(DISABLE_PAGE_SCAN_CHECK_TIMER), osTimerOnce, NULL);
    }
    osTimerStart(disable_page_scan_check_timer, 4000);
}

#endif
void PairingTransferToConnectable(void)
{
    int activeCons;
    osapi_lock_stack();
    activeCons = btif_me_get_activeCons();
    osapi_unlock_stack();
    TRACE(1,"%s",__func__);

    app_bt_connectable_state_set(INITIATE_PAIRING_NONE);
    if(activeCons == 0){
        TRACE(0,"!!!PairingTransferToConnectable  BAM_CONNECTABLE_ONLY\n");
        app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
    }
}
int app_bt_get_audio_up_id(void)
{
    uint8_t i;
    btif_remote_device_t *remDev = NULL;
    btif_cmgr_handler_t    *cmgrHandler;
    for (i=0; i<BT_DEVICE_NUM; i++)
    {
        remDev = btif_me_enumerate_remote_devices(i);
        if (remDev != NULL) {
            cmgrHandler = btif_cmgr_get_acl_handler(remDev);
            if(btif_cmgr_is_audio_up(cmgrHandler) == true)
                break;
        }
    }

    return i;
}
void hfp_call_state_checker(void);

#if defined(IBRT)
int app_bt_ibrt_profile_checker(const char *str,
                                btif_remote_device_t *remDev, btif_cmgr_handler_t *cmgrHandler,
                                a2dp_stream_t * a2dp_stream, hf_chan_handle_t hf_channel)
{
    if (remDev && cmgrHandler){
        TRACE(6,"checker: %s remDev state:%d mode:%d role:%d sniffInterval:%d/%d",
                str,
                btif_me_get_remote_device_state(remDev),
                btif_me_get_remote_device_mode(remDev),
                btif_me_get_remote_device_role(remDev),
                btif_cmgr_get_cmgrhandler_sniff_Interval(cmgrHandler),
                btif_cmgr_get_cmgrhandler_sniff_info(cmgrHandler)->maxInterval);

        TRACE(2,"checker: %s remDev:%p remote_dev_address:", str,remDev);
        DUMP8("0x%02x ", btif_me_get_remote_device_bdaddr(remDev)->address, BTIF_BD_ADDR_SIZE);
    }else{
        TRACE(3,"checker: %s remDev:%p cmgrHandler:%p", str, remDev, cmgrHandler);
    }

    if (a2dp_stream){
        TRACE(1,"a2dp State:%d", btif_a2dp_get_stream_state(a2dp_stream));
        if (btif_a2dp_get_stream_state(a2dp_stream) > BTIF_AVDTP_STRM_STATE_IDLE){
            TRACE(1,"a2dp cmgrHandler remDev:%p", btif_a2dp_get_stream_devic_cmgrHandler_remdev(a2dp_stream));
        }
    }else{
        TRACE(1,"%s a2dp stream NULL", str);
    }
    if (hf_channel){
        TRACE(4,"hf_channel Connected:%d IsAudioUp:%d/%d remDev:%p", btif_hf_is_acl_connected(hf_channel),
                                                                    app_bt_device.hf_audio_state[0],
                                                                    btif_hf_check_AudioConnect_status(hf_channel),
                                                                    btif_hf_cmgr_get_remote_device(hf_channel));
    }else{
        TRACE(1,"%s hf_channel NULL", str);
    }
    return 0;
}
#endif

int app_bt_state_checker(void)
{
    btif_remote_device_t *remDev = NULL;
    btif_cmgr_handler_t *cmgrHandler;
    osapi_lock_stack();

#if defined(IBRT)
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (app_tws_ibrt_mobile_link_connected()){
        TRACE(1,"checker: IBRT_MASTER activeCons:%d", btif_me_get_activeCons());
        remDev = p_ibrt_ctrl->p_tws_remote_dev;
        if (remDev != NULL) {
            cmgrHandler = btif_cmgr_get_acl_handler(remDev);
            if (cmgrHandler) {
                app_bt_ibrt_profile_checker("tws peers", remDev, cmgrHandler, NULL, NULL);
            }else{
                TRACE(0,"checker: cmgrhandler not handle p_tws_remote_dev!");
            }
        } else {
            TRACE(0,"checker: tws_remote_dev is NULL!");
        }

        remDev = p_ibrt_ctrl->p_mobile_remote_dev;
        if (remDev != NULL) {
            cmgrHandler = btif_cmgr_get_acl_handler(remDev);
            if (cmgrHandler) {
                app_bt_ibrt_profile_checker("master mobile", remDev, cmgrHandler, app_bt_device.a2dp_connected_stream[0],
                                            app_bt_device.hf_conn_flag[0] ? app_bt_device.hf_channel[0] : NULL);
            }else{
                TRACE(0,"checker: cmgrhandler not handle mobile_remote_dev");
            }
        } else {
            TRACE(0,"checker: mobile_remote_dev is NULL!");
        }
    }else if (app_tws_ibrt_slave_ibrt_link_connected()){
        TRACE(1,"checker: IBRT_SLAVE activeCons:%d", btif_me_get_activeCons());
        remDev = p_ibrt_ctrl->p_tws_remote_dev;
        if (remDev != NULL) {
            cmgrHandler = btif_cmgr_get_acl_handler(remDev);
            if (cmgrHandler) {
                app_bt_ibrt_profile_checker("tws peers", remDev, cmgrHandler, NULL, NULL);
            }else{
                TRACE(0,"checker: cmgrhandler not handle p_tws_remote_dev!");
            }
        } else {
            TRACE(0,"checker: tws_remote_dev is NULL!");
        }
        if (app_ibrt_ui_is_profile_exchanged()){
            app_bt_ibrt_profile_checker("ibrt mobile", NULL, NULL, app_bt_device.a2dp_connected_stream[0],
                                        app_bt_device.hf_conn_flag[0] ? app_bt_device.hf_channel[0] : NULL);
        }
    }else{
        TRACE(1,"checker: IBRT_UNKNOW activeCons:%d", btif_me_get_activeCons());
    }
    app_ibrt_if_ctx_checker();
#if defined(ENHANCED_STACK)
    btif_me_cobuf_state_dump();
    btif_me_hcibuff_state_dump();
#endif
    //BT controller state checker
    ASSERT(bt_drv_reg_op_check_bt_controller_state(), "BT controller dead!");
#else

    bt_bdaddr_t *bd_addr = NULL;
	TRACE(2,"\n");
	TRACE(2,"%s: activeCons:%d, current_access_mode:%d",__func__,btif_me_get_activeCons(),app_bt_get_current_access_mode());
    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
	TRACE(8,"checker[%d]: current reconnect mode:%d,reconnect cnt:%d",i,bt_profile_manager[i].reconnect_mode,bt_profile_manager[i].reconnect_cnt);
        remDev = btif_me_enumerate_remote_devices(i);
        if (remDev != NULL) {
            cmgrHandler = btif_cmgr_get_acl_handler(remDev);
            if (cmgrHandler) {
				bd_addr = btif_me_get_remote_device_bdaddr(remDev);			
				TRACE(8,"checker[%d]: cmghdl:%p remDev:%p state:%d mode:%d role:%d sniffInterval:%d/%d",
						i,
						cmgrHandler,
						remDev,
						btif_me_get_remote_device_state(remDev),
						btif_me_get_remote_device_mode(remDev),
						btif_me_get_remote_device_role(remDev),
						btif_cmgr_get_cmgrhandler_sniff_Interval(cmgrHandler),
						btif_cmgr_get_cmgrhandler_sniff_info(cmgrHandler)->maxInterval);
				TRACE(8,"checker[%d]: bdaddr:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
						i,bd_addr->address[5],bd_addr->address[4],bd_addr->address[3],bd_addr->address[2],bd_addr->address[1],bd_addr->address[0]);
				TRACE(8,"checker[%d]: a2dp State:%d, hf connect State:%d ",
						i,app_bt_device.a2dp_connected_stream[i] ?  btif_a2dp_get_stream_state(app_bt_device.a2dp_connected_stream[i]):BTIF_A2DP_STREAM_STATE_CLOSED,
							app_bt_device.hf_conn_flag[i]);
            }
        }
        else {
            TRACE(1,"checker[%d] remDev is NULL!",  i);
        }

#if defined (__HSP_ENABLE__)
        TRACE(2,"hs_channel Connected:%d remDev:%p ",
                        app_bt_device.hs_conn_flag[i],
                        app_bt_device.hs_channel[i].cmgrHandler.remDev);
#endif
    }
	
#ifdef __AI_VOICE__
    TRACE(1,"ai_setup_complete %d", app_ai_is_setup_complete());
#endif
	hfp_call_state_checker();
#if defined(ENHANCED_STACK)
    btif_me_cobuf_state_dump();
    btif_me_hcibuff_state_dump();
#endif
#endif

/** add by pang **/
#ifdef  __IAG_BLE_INCLUDE__
			// start BLE adv
			if(!app_is_arrive_at_max_ble_connections() && app_bt_is_connected()){
				if(!app_ble_is_in_advertising_state()){					 
					app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, true);
				}
			}
			else{
				if(app_ble_is_in_advertising_state())
				app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, false);
			}
#endif
/** end add **/
    osapi_unlock_stack();

    return 0;
}

uint8_t app_bt_get_devId_from_RemDev(btif_remote_device_t* remDev)
{
    uint8_t connectedDevId = 0;
    for (uint8_t devId = 0;devId < BT_DEVICE_NUM;devId++)
    {
        if (btif_me_enumerate_remote_devices(devId) == remDev)
        {
            TRACE(2, "%s %d", __func__, devId);
            connectedDevId = devId;
            break;
        }
    }

    return connectedDevId;
}

void app_bt_accessible_manager_process(const btif_event_t *Event)
{
#if defined(IBRT)
    //IBRT device's access mode will be controlled by UI
    return;
#else
    btif_event_type_t etype = btif_me_get_callback_event_type(Event);
#ifdef __BT_ONE_BRING_TWO__
    static uint8_t opening_reconnect_cnf_cnt = 0;
    //uint8_t disconnectedDevId = app_bt_get_devId_from_RemDev(btif_me_get_callback_event_rem_dev( Event));

    if (app_bt_profile_connect_openreconnecting(NULL)){
        if (etype == BTIF_BTEVENT_LINK_CONNECT_CNF){
            opening_reconnect_cnf_cnt++;
        }
        if (record2_avalible){
            if (opening_reconnect_cnf_cnt<2){
                //return;
            }
        } 
		/** add by pang **/
		return;
		/** end add **/
    }
    /** add by pang **/
	if(app_bt_profile_connect_reconnecting())
	    return;
	/** end add **/
#endif
/** add by pang **/
	switch (etype){
		case BTIF_BTEVENT_LINK_DISCONNECT:
		    if((0x08==btif_me_get_remote_device_disc_reason_saved(btif_me_get_callback_event_rem_dev( Event)))&&
				(0x08==btif_me_get_remote_device_disc_reason(btif_me_get_callback_event_rem_dev( Event)))&&
				(0==btif_me_get_activeCons())){
				return;
			}
		break;
		default: 
		break;
	}

	if(lacal_bt_off)
		return;
		
#if defined(__USE_3_5JACK_CTR__)
	if(reconncect_null_by_user){
		TRACE(0,"***reconncect_null_by_user=1");
		app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);	
		return;
	}
#endif		
/** end add **/
    switch (etype) {
        case BTIF_BTEVENT_ENCRYPTION_CHANGE:
            TRACE(1,"BTIF_BTEVENT_ENCRYPTION_CHANGE activeCons:%d",btif_me_get_activeCons());
#if defined(__BTIF_EARPHONE__)   && !defined(FPGA)
            app_stop_10_second_timer(APP_PAIR_TIMER_ID);
#endif
#ifdef __BT_ONE_BRING_TWO__
             if(btif_me_get_activeCons() == 0){
#ifdef __EARPHONE_STAY_BOTH_SCAN__
                app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
#else
                app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
#endif
         #if 1
            }else if(btif_me_get_activeCons() == 1){
                app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
            }else if(btif_me_get_activeCons() >= 2){
                app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
            }
		#else //m by pang
			}else if(btif_me_get_activeCons() == 1){
			  if(app_get_multipoint_flag())
                app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
			  else
			  	app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
            }else if(btif_me_get_activeCons() >= 2){
                app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
            }
		#endif
#else
            if(btif_me_get_activeCons() == 0){
#ifdef __EARPHONE_STAY_BOTH_SCAN__
                app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
#else
                app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
#endif
            }else if(btif_me_get_activeCons() >= 1){
                app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
            }
#endif
            break;
        case BTIF_BTEVENT_LINK_DISCONNECT:
            TRACE(1,"DISCONNECT activeCons:%d",btif_me_get_activeCons());
#ifdef __EARPHONE_STAY_BOTH_SCAN__
#ifdef __BT_ONE_BRING_TWO__
            if(btif_me_get_activeCons() == 0){
                app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
            }else if(btif_me_get_activeCons() == 1){
                app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
            }else if(btif_me_get_activeCons() >= 2){
                app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
            }
#else
            app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
#endif
#else			
			#if 0            
            app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
			#else //m by pang
			if((factory_reset_flag&&(btif_me_get_activeCons() == 0)) || BTIF_BAM_GENERAL_ACCESSIBLE == app_bt_get_current_access_mode()){
				app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
			}
			else
				app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
			#endif			
#endif
            break;
#ifdef __BT_ONE_BRING_TWO__
        case BTIF_BTEVENT_SCO_CONNECT_IND:
        case BTIF_BTEVENT_SCO_CONNECT_CNF:
            if(btif_me_get_activeCons() == 1){
                //app_bt_accessmode_set_req(BTIF_BAM_NOT_ACCESSIBLE);
            }
            break;
        case BTIF_BTEVENT_SCO_DISCONNECT:
            if(btif_me_get_activeCons() == 1){
               // app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
            }
            break;
#endif
        default:
            break;
    }
#endif
}

#define APP_BT_SWITCHROLE_LIMIT (1)
//#define __SET_OUR_AS_MASTER__

void app_bt_role_manager_process(const btif_event_t *Event)
{
#if defined(IBRT)
    return;
#else
    static uint8_t switchrole_cnt = 0;
    btif_remote_device_t *remDev = NULL;
    btif_event_type_t etype = btif_me_get_callback_event_type(Event);
    //on phone connecting
    switch (etype) {
        case BTIF_BTEVENT_LINK_CONNECT_IND:
            if(  btif_me_get_callback_event_err_code(Event) == BTIF_BEC_NO_ERROR){
                if (btif_me_get_activeCons() == 1){
                    switch ( btif_me_get_callback_event_rem_dev_role (Event)) {
#if defined(__SET_OUR_AS_MASTER__)
                        case BTIF_BCR_SLAVE:
                        case BTIF_BCR_PSLAVE:
#else
                        case BTIF_BCR_MASTER:
                        case BTIF_BCR_PMASTER:
#endif
                            TRACE(1,"CONNECT_IND try to role %p\n",   btif_me_get_callback_event_rem_dev( Event));
                            //curr connectrot try to role
                            switchrole_cnt = 0;
                            app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_MASTER_SLAVE_SWITCH|BTIF_BLP_SNIFF_MODE);
                            break;
#if defined(__SET_OUR_AS_MASTER__)
                        case BTIF_BCR_MASTER:
                        case BTIF_BCR_PMASTER:
#else
                        case BTIF_BCR_SLAVE:
                        case BTIF_BCR_PSLAVE:
#endif
                        case BTIF_BCR_ANY:
                        case BTIF_BCR_UNKNOWN:
                        default:
                            TRACE(1,"CONNECT_IND disable role %p\n",btif_me_get_callback_event_rem_dev( Event));
                            //disable roleswitch when 1 connect
                            app_bt_Me_SetLinkPolicy  (   btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_SNIFF_MODE);
                            break;
                    }
                    //set next connector to master
                    app_bt_ME_SetConnectionRole(BTIF_BCR_MASTER);
                }else if (btif_me_get_activeCons() > 1){
                    switch (btif_me_get_callback_event_rem_dev_role (Event)) {
                        case BTIF_BCR_MASTER:
                        case BTIF_BCR_PMASTER:
                            TRACE(1,"CONNECT_IND disable role %p\n",btif_me_get_callback_event_rem_dev( Event));
                            //disable roleswitch
                            app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_SNIFF_MODE);
                            break;
                        case BTIF_BCR_SLAVE:
                        case BTIF_BCR_PSLAVE:
                        case BTIF_BCR_ANY:
                        case BTIF_BCR_UNKNOWN:
                        default:
                            //disconnect slave
                            TRACE(1,"CONNECT_IND disconnect slave %p\n",btif_me_get_callback_event_rem_dev( Event));
                            app_bt_MeDisconnectLink(btif_me_get_callback_event_rem_dev( Event));
                            break;
                    }
                    //set next connector to master
                    app_bt_ME_SetConnectionRole(BTIF_BCR_MASTER);
                }
            }
            break;
        case BTIF_BTEVENT_LINK_CONNECT_CNF:
            if (btif_me_get_activeCons() == 1){
                switch (btif_me_get_callback_event_rem_dev_role (Event)) {
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_SLAVE:
                    case BTIF_BCR_PSLAVE:
#else
                    case BTIF_BCR_MASTER:
                    case BTIF_BCR_PMASTER:
#endif
                        TRACE(1,"CONNECT_CNF try to role %p\n",btif_me_get_callback_event_rem_dev( Event));
                        //curr connectrot try to role
                        switchrole_cnt = 0;
                        app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_MASTER_SLAVE_SWITCH|BTIF_BLP_SNIFF_MODE);
                        app_bt_ME_SwitchRole(btif_me_get_callback_event_rem_dev( Event));
                        break;
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_MASTER:
                    case BTIF_BCR_PMASTER:
#else
                    case BTIF_BCR_SLAVE:
                    case BTIF_BCR_PSLAVE:
#endif
                    case BTIF_BCR_ANY:
                    case BTIF_BCR_UNKNOWN:
                    default:
                        TRACE(1,"CONNECT_CNF disable role %p\n",btif_me_get_callback_event_rem_dev( Event));
                        //disable roleswitch
                        app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_SNIFF_MODE);
                        break;
                }
                //set next connector to master
                app_bt_ME_SetConnectionRole(BTIF_BCR_MASTER);
            }else if (btif_me_get_activeCons() > 1){
                switch (btif_me_get_callback_event_rem_dev_role (Event)) {
                    case BTIF_BCR_MASTER:
                    case BTIF_BCR_PMASTER :
                        TRACE(1,"CONNECT_CNF disable role %p\n",btif_me_get_callback_event_rem_dev( Event));
                        //disable roleswitch
                        app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_SNIFF_MODE);
                        break;
                    case BTIF_BCR_SLAVE:
                    case BTIF_BCR_ANY:
                    case BTIF_BCR_UNKNOWN:
                    default:
                        //disconnect slave
                        TRACE(1,"CONNECT_CNF disconnect slave %p\n",btif_me_get_callback_event_rem_dev( Event));
                        app_bt_MeDisconnectLink(btif_me_get_callback_event_rem_dev( Event));
                        break;
                }
                //set next connector to master
                app_bt_ME_SetConnectionRole(BTIF_BCR_MASTER);
            }
            break;
        case BTIF_BTEVENT_LINK_DISCONNECT:
            switchrole_cnt = 0;
            if (btif_me_get_activeCons() == 0){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    if(app_bt_device.a2dp_connected_stream[i])
                        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_connected_stream[i], FALSE);
                    app_bt_HF_SetMasterRole(app_bt_device.hf_channel[i], FALSE);
                }
                app_bt_ME_SetConnectionRole(BTIF_BCR_ANY);
            }else if (btif_me_get_activeCons() == 1){
                //set next connector to master
                app_bt_ME_SetConnectionRole(BTIF_BCR_MASTER);
            }
            break;
        case BTIF_BTEVENT_ROLE_CHANGE:
                switch ( btif_me_get_callback_event_role_change_new_role(Event)) {
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_SLAVE:
#else
                    case BTIF_BCR_MASTER:
#endif
                        if (++switchrole_cnt<=APP_BT_SWITCHROLE_LIMIT){
                            app_bt_ME_SwitchRole(btif_me_get_callback_event_rem_dev( Event));
                        }else{
#if defined(__SET_OUR_AS_MASTER__)
                            TRACE(2,"ROLE TO MASTER FAILED remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#else
                            TRACE(2,"ROLE TO SLAVE FAILED remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#endif
                            switchrole_cnt = 0;
                        }
                        break;
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_MASTER:
                        TRACE(2,"ROLE TO MASTER SUCCESS remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#else
                    case BTIF_BCR_SLAVE:
                        TRACE(2,"ROLE TO SLAVE SUCCESS remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#endif
                        switchrole_cnt = 0;
                        app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event),BTIF_BLP_SNIFF_MODE);
                        break;
                    case BTIF_BCR_ANY:
                        break;
                    case BTIF_BCR_UNKNOWN:
                        break;
                    default:
                        break;
                }

            if (btif_me_get_activeCons() > 1){
                uint8_t slave_cnt = 0;
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    remDev = btif_me_enumerate_remote_devices(i);
                    if ( btif_me_get_current_role(remDev) == BTIF_BCR_SLAVE){
                        slave_cnt++;
                    }
                }
                if (slave_cnt>1){
                    TRACE(1,"ROLE_CHANGE disconnect slave %p\n",btif_me_get_callback_event_rem_dev( Event));
                    app_bt_MeDisconnectLink(btif_me_get_callback_event_rem_dev( Event));
                }
            }
            break;
        default:
           break;
        }
#endif
}

static void app_bt_switch_role_if_needed(btif_remote_device_t *remDev)
{
    int current_role = btif_me_get_remote_device_role(remDev);
#if defined(__SET_OUR_AS_MASTER__)
    if (current_role == BTIF_BCR_SLAVE || current_role == BTIF_BCR_PSLAVE)
#else
    if (current_role == BTIF_BCR_MASTER || current_role == BTIF_BCR_PMASTER)
#endif
    {
        app_bt_Me_SetLinkPolicy(remDev, BTIF_BLP_MASTER_SLAVE_SWITCH|BTIF_BLP_SNIFF_MODE);
        app_bt_ME_SwitchRole(remDev);
    }
}

void app_bt_role_manager_process_dual_slave(const btif_event_t *Event)
{
#if defined(IBRT)
    return;
#else
    static uint8_t switchrole_cnt = 0;
    btif_remote_device_t *remDev = NULL;
    //on phone connecting
    switch ( btif_me_get_callback_event_type(Event)) {
        case BTIF_BTEVENT_LINK_CONNECT_IND:
        case BTIF_BTEVENT_LINK_CONNECT_CNF:
            if(btif_me_get_callback_event_err_code(Event) == BTIF_BEC_NO_ERROR){
                switch (btif_me_get_callback_event_rem_dev_role (Event)) {
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_SLAVE:
                    case BTIF_BCR_PSLAVE:
#else
                    case BTIF_BCR_MASTER:
                    case BTIF_BCR_PMASTER:
#endif
                        TRACE(1,"CONNECT_IND/CNF try to role %p\n",btif_me_get_callback_event_rem_dev( Event));
                        switchrole_cnt = 0;
                        app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_MASTER_SLAVE_SWITCH|BTIF_BLP_SNIFF_MODE);
                        app_bt_ME_SwitchRole(btif_me_get_callback_event_rem_dev( Event));
                        break;
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_MASTER:
                    case BTIF_BCR_PMASTER:
#else
                    case BTIF_BCR_SLAVE:
                    case BTIF_BCR_PSLAVE:
#endif
                    case BTIF_BCR_ANY:
                    case BTIF_BCR_UNKNOWN:
                    default:
                        TRACE(1,"CONNECT_IND disable role %p\n",btif_me_get_callback_event_rem_dev( Event));
                        app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_SNIFF_MODE);
                        break;
                }
                app_bt_ME_SetConnectionRole(BTIF_BCR_SLAVE);
            }
            break;
        case BTIF_BTEVENT_LINK_DISCONNECT:
            switchrole_cnt = 0;
            if (  btif_me_get_activeCons() == 0){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    if(app_bt_device.a2dp_connected_stream[i])
                        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_connected_stream[i], FALSE);
                    app_bt_HF_SetMasterRole(app_bt_device.hf_channel[i], FALSE);
                }
                app_bt_ME_SetConnectionRole(BTIF_BCR_ANY);
            }else if (btif_me_get_activeCons() == 1){
                app_bt_ME_SetConnectionRole(BTIF_BCR_SLAVE);
            }
            break;
        case BTIF_BTEVENT_ROLE_CHANGE:
                switch (btif_me_get_callback_event_role_change_new_role(Event)) {
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_SLAVE:
#else
                    case BTIF_BCR_MASTER:
#endif
                        if (++switchrole_cnt<=APP_BT_SWITCHROLE_LIMIT){
                            TRACE(1,"ROLE_CHANGE try to role again: %d", switchrole_cnt);
                            app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event), BTIF_BLP_MASTER_SLAVE_SWITCH|BTIF_BLP_SNIFF_MODE);
                            app_bt_ME_SwitchRole(btif_me_get_callback_event_rem_dev( Event));
                        }else{
#if defined(__SET_OUR_AS_MASTER__)
                            TRACE(2,"ROLE TO MASTER FAILED remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#else
                            TRACE(2,"ROLE TO SLAVE FAILED remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#endif
                            switchrole_cnt = 0;
                        }
                        break;
#if defined(__SET_OUR_AS_MASTER__)
                    case BTIF_BCR_MASTER:
                        TRACE(2,"ROLE TO MASTER SUCCESS remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#else
                    case BTIF_BCR_SLAVE:
                        TRACE(2,"ROLE TO SLAVE SUCCESS remDev %p cnt:%d\n",btif_me_get_callback_event_rem_dev( Event), switchrole_cnt);
#endif
                        switchrole_cnt = 0;

                        //workaround for power reset opening reconnect sometime unsuccessfully in sniff mode,
                        //only after authentication completes, enable sniff mode.
                        remDev = btif_me_get_callback_event_rem_dev(Event);

                        {
                            uint8_t devId = app_bt_get_devId_from_RemDev(remDev);
                            if (!app_bt_is_link_encrypted(devId))
                            {
//                                btif_me_auth_req(btif_me_get_remote_device_hci_handle(remDev));
                            }
                        }
                        
                        if (btif_me_get_remote_device_auth_state(remDev) == BTIF_BAS_AUTHENTICATED)
                        {
                            app_bt_Me_SetLinkPolicy(remDev,BTIF_BLP_SNIFF_MODE);
                        }
                        else
                        {
                            app_bt_Me_SetLinkPolicy(remDev,BTIF_BLP_DISABLE_ALL);
                        }
                        break;
                    case BTIF_BCR_ANY:
                        break;
                    case BTIF_BCR_UNKNOWN:
                        break;
                    default:
                        break;
                }
            break;
    }
#endif
}

static int app_bt_sniff_manager_init(void)
{
    btif_sniff_info_t sniffInfo;
    btif_remote_device_t *remDev = NULL;

    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
        remDev = btif_me_enumerate_remote_devices(i);
        sniffInfo.maxInterval = BTIF_CMGR_SNIFF_MAX_INTERVAL;
        sniffInfo.minInterval = BTIF_CMGR_SNIFF_MIN_INTERVAL;
        sniffInfo.attempt = BTIF_CMGR_SNIFF_ATTEMPT;
        sniffInfo.timeout =BTIF_CMGR_SNIFF_TIMEOUT;
        app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&sniffInfo, remDev);
        app_bt_HF_EnableSniffMode(app_bt_device.hf_channel[i], FALSE);
#if defined (__HSP_ENABLE__)
        app_bt_HS_EnableSniffMode(&app_bt_device.hs_channel[i], FALSE);
#endif
    }

    return 0;
}

void app_bt_sniff_config(btif_remote_device_t *remDev)
{
    btif_sniff_info_t sniffInfo;
    sniffInfo.maxInterval = BTIF_CMGR_SNIFF_MAX_INTERVAL;
    sniffInfo.minInterval = BTIF_CMGR_SNIFF_MIN_INTERVAL;
    sniffInfo.attempt = BTIF_CMGR_SNIFF_ATTEMPT;
    sniffInfo.timeout = BTIF_CMGR_SNIFF_TIMEOUT;
    app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&sniffInfo, remDev);
#if !defined(IBRT)
    if (btif_me_get_activeCons() > 1){
        btif_remote_device_t* tmpRemDev = NULL;
        btif_cmgr_handler_t    *currbtif_cmgr_handler_t = NULL;
        btif_cmgr_handler_t    *otherbtif_cmgr_handler_t = NULL;
        currbtif_cmgr_handler_t = btif_cmgr_get_conn_ind_handler(remDev);
        for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
            tmpRemDev = btif_me_enumerate_remote_devices(i);
            if (remDev != tmpRemDev && tmpRemDev != NULL){
                otherbtif_cmgr_handler_t = btif_cmgr_get_acl_handler(tmpRemDev);
                if (otherbtif_cmgr_handler_t && currbtif_cmgr_handler_t){
                    if ( btif_cmgr_get_cmgrhandler_sniff_info(otherbtif_cmgr_handler_t)->maxInterval == btif_cmgr_get_cmgrhandler_sniff_info(currbtif_cmgr_handler_t)->maxInterval){
                        sniffInfo.maxInterval = btif_cmgr_get_cmgrhandler_sniff_info(otherbtif_cmgr_handler_t)->maxInterval -20;
                        sniffInfo.minInterval = btif_cmgr_get_cmgrhandler_sniff_info(otherbtif_cmgr_handler_t)->minInterval - 20;
                        sniffInfo.attempt = BTIF_CMGR_SNIFF_ATTEMPT;
                        sniffInfo.timeout = BTIF_CMGR_SNIFF_TIMEOUT;
                        app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&sniffInfo, remDev);
                    }
                }
                break;
            }
            else {
                TRACE(3,"%s:enumerate i:%d remDev is NULL, param remDev:%p, this may cause error!", __func__, i, remDev);
            }
        }
    }
#endif
}

void app_bt_sniff_manager_process(const btif_event_t *Event)
{
    btif_remote_device_t *remDev = NULL;
    btif_cmgr_handler_t    *currbtif_cmgr_handler_t = NULL;
    btif_cmgr_handler_t    *otherbtif_cmgr_handler_t = NULL;

    btif_sniff_info_t sniffInfo;

    if (!besbt_cfg.sniff)
        return;

    switch (btif_me_get_callback_event_type(Event)) {
        case BTIF_BTEVENT_LINK_CONNECT_IND:
            break;
        case BTIF_BTEVENT_LINK_CONNECT_CNF:
            break;
        case BTIF_BTEVENT_LINK_DISCONNECT:
            sniffInfo.maxInterval = BTIF_CMGR_SNIFF_MAX_INTERVAL;
            sniffInfo.minInterval = BTIF_CMGR_SNIFF_MIN_INTERVAL;
            sniffInfo.attempt = BTIF_CMGR_SNIFF_ATTEMPT;
            sniffInfo.timeout = BTIF_CMGR_SNIFF_TIMEOUT;
            app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&sniffInfo,btif_me_get_callback_event_rem_dev( Event));
            break;
        case BTIF_BTEVENT_MODE_CHANGE:

            /*
            if(Event->p.modeChange.curMode == BLM_SNIFF_MODE){
                currbtif_cmgr_handler_t = btif_cmgr_get_acl_handler(btif_me_get_callback_event_rem_dev( Event));
                if (Event->p.modeChange.interval > CMGR_SNIFF_MAX_INTERVAL){
                        if (!opRemDev){
                            opRemDev = currbtif_cmgr_handler_t->remDev;
                        }
                        currbtif_cmgr_handler_t->sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
                        currbtif_cmgr_handler_t->sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
                        currbtif_cmgr_handler_t->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                        currbtif_cmgr_handler_t->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                        app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&currbtif_cmgr_handler_t->sniffInfo,btif_me_get_callback_event_rem_dev( Event));
                        app_bt_ME_StopSniff(currbtif_cmgr_handler_t->remDev);
                }else{
                    if (currbtif_cmgr_handler_t){
                        currbtif_cmgr_handler_t->sniffInfo.maxInterval = Event->p.modeChange.interval;
                        currbtif_cmgr_handler_t->sniffInfo.minInterval = Event->p.modeChange.interval;
                        currbtif_cmgr_handler_t->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                        currbtif_cmgr_handler_t->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                        app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&currbtif_cmgr_handler_t->sniffInfo,btif_me_get_callback_event_rem_dev( Event));
                    }
                    if (btif_me_get_activeCons() > 1){
                        for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                            remDev = btif_me_enumerate_remote_devices(i);
                            if (btif_me_get_callback_event_rem_dev( Event) != remDev){
                                otherbtif_cmgr_handler_t = btif_cmgr_get_acl_handler(remDev);
                                if (otherbtif_cmgr_handler_t){
                                    if (otherbtif_cmgr_handler_t->sniffInfo.maxInterval == currbtif_cmgr_handler_t->sniffInfo.maxInterval){
                                        if (btif_me_get_current_mode(remDev) == BLM_ACTIVE_MODE){
                                            otherbtif_cmgr_handler_t->sniffInfo.maxInterval -= 20;
                                            otherbtif_cmgr_handler_t->sniffInfo.minInterval -= 20;
                                            otherbtif_cmgr_handler_t->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                                            otherbtif_cmgr_handler_t->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                                            app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&otherbtif_cmgr_handler_t->sniffInfo, remDev);
                                            TRACE(1,"reconfig sniff other RemDev:%x\n", remDev);
                                        }else if (btif_me_get_current_mode(remDev) == BLM_SNIFF_MODE){
                                            need_reconfig = true;
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                    if (need_reconfig){
                        opRemDev = remDev;
                        if (currbtif_cmgr_handler_t){
                            currbtif_cmgr_handler_t->sniffInfo.maxInterval -= 20;
                            currbtif_cmgr_handler_t->sniffInfo.minInterval -= 20;
                            currbtif_cmgr_handler_t->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                            currbtif_cmgr_handler_t->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                            app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(&currbtif_cmgr_handler_t->sniffInfo, currbtif_cmgr_handler_t->remDev);
                        }
                        app_bt_ME_StopSniff(currbtif_cmgr_handler_t->remDev);
                        TRACE(1,"reconfig sniff setup op opRemDev:%x\n", opRemDev);
                    }
                }
            }
            if (Event->p.modeChange.curMode == BLM_ACTIVE_MODE){
                if (opRemDev ==btif_me_get_callback_event_rem_dev( Event)){
                    TRACE(1,"reconfig sniff op opRemDev:%x\n", opRemDev);
                    opRemDev = NULL;
                    currbtif_cmgr_handler_t = btif_cmgr_get_acl_handler(btif_me_get_callback_event_rem_dev( Event));
                    if (currbtif_cmgr_handler_t){
                        app_bt_CMGR_SetSniffTimer(currbtif_cmgr_handler_t, NULL, CMGR_SNIFF_TIMER);
                    }
                }
            }
            */
            break;
        case BTIF_BTEVENT_ACL_DATA_ACTIVE:
            btif_cmgr_handler_t    *cmgrHandler;
            /* Start the sniff timer */
            cmgrHandler = btif_cmgr_get_acl_handler(btif_me_get_callback_event_rem_dev( Event));
            if (cmgrHandler)
                app_bt_CMGR_SetSniffTimer(cmgrHandler, NULL, BTIF_CMGR_SNIFF_TIMER);
            break;
        case BTIF_BTEVENT_SCO_CONNECT_IND:
        case BTIF_BTEVENT_SCO_CONNECT_CNF:
            TRACE(1,"BTEVENT_SCO_CONNECT_IND/CNF cur_remDev = %p",btif_me_get_callback_event_rem_dev( Event));
            currbtif_cmgr_handler_t = btif_cmgr_get_conn_ind_handler(btif_me_get_callback_event_rem_dev( Event));
            app_bt_Me_SetLinkPolicy( btif_me_get_callback_event_sco_connect_rem_dev(Event), BTIF_BLP_DISABLE_ALL);
            if (btif_me_get_activeCons() > 1){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    remDev = btif_me_enumerate_remote_devices(i);
                    TRACE(1,"other_remDev = %p",remDev);
                    if (btif_me_get_callback_event_rem_dev( Event) == remDev){
                        continue;
                    }

                    otherbtif_cmgr_handler_t = btif_cmgr_get_conn_ind_handler(remDev);
                    if (otherbtif_cmgr_handler_t){
                        if (btif_cmgr_is_link_up(otherbtif_cmgr_handler_t)){
                            if ( btif_me_get_current_mode(remDev) == BTIF_BLM_ACTIVE_MODE){
                                TRACE(0,"other dev disable sniff");
                                app_bt_Me_SetLinkPolicy(remDev, BTIF_BLP_DISABLE_ALL);
                            }else if (btif_me_get_current_mode(remDev) == BTIF_BLM_SNIFF_MODE){
                                TRACE(0," ohter dev exit & disable sniff");
                                app_bt_ME_StopSniff(remDev);
                                app_bt_Me_SetLinkPolicy(remDev, BTIF_BLP_DISABLE_ALL);
                            }
                        }
                    }

#if defined (HFP_NO_PRERMPT)
                TRACE(2,"cur_audio = %d other_audio = %d",btif_cmgr_is_audio_up(currbtif_cmgr_handler_t),
                    btif_cmgr_is_audio_up(otherbtif_cmgr_handler_t));
                if((btif_cmgr_is_audio_up(otherbtif_cmgr_handler_t) == true) &&
                    (btif_cmgr_is_audio_up(currbtif_cmgr_handler_t) == true)
                    /*(btapp_hfp_get_call_active()!=0)*/){
                    btif_cmgr_remove_audio_link(currbtif_cmgr_handler_t);
                    app_bt_Me_switch_sco(btif_cmgr_get_sco_connect_sco_Hcihandler(otherbtif_cmgr_handler_t));
                }
#endif
                }
            }
            break;
        case BTIF_BTEVENT_SCO_DISCONNECT:
            app_bt_profile_reconnect_pending_process();
            if (a2dp_is_music_ongoing())
            {
                break;
            }
            if (btif_me_get_activeCons() == 1){
                app_bt_Me_SetLinkPolicy( btif_me_get_callback_event_sco_connect_rem_dev(Event), BTIF_BLP_SNIFF_MODE);
            }else{
                uint8_t i;
                for (i=0; i<BT_DEVICE_NUM; i++){
                    remDev = btif_me_enumerate_remote_devices(i);
                    if (btif_me_get_callback_event_rem_dev( Event) == remDev){
                        break;
                    }
                }
/*
                if(i==0)
                    remDev = btif_me_enumerate_remote_devices(1);
                else if(i==1)
                    remDev = btif_me_enumerate_remote_devices(0);
                else
                    ASSERT(0,"error other remotedevice!!!");     */
                otherbtif_cmgr_handler_t = btif_cmgr_get_conn_ind_handler(remDev);
                currbtif_cmgr_handler_t = btif_cmgr_get_conn_ind_handler(btif_me_get_callback_event_rem_dev( Event));

                TRACE(4,"SCO_DISCONNECT:%d/%d %p/%p\n", btif_cmgr_is_audio_up(currbtif_cmgr_handler_t), btif_cmgr_is_audio_up(otherbtif_cmgr_handler_t),
                                                 btif_cmgr_get_cmgrhandler_remdev(currbtif_cmgr_handler_t),btif_me_get_callback_event_rem_dev( Event));
                if (otherbtif_cmgr_handler_t){
                    if (!btif_cmgr_is_audio_up(otherbtif_cmgr_handler_t)){
                        TRACE(0,"enable sniff to all\n");
                        app_bt_Me_SetLinkPolicy(  btif_me_get_callback_event_sco_connect_rem_dev(Event), BTIF_BLP_SNIFF_MODE);
                        app_bt_Me_SetLinkPolicy( btif_cmgr_get_cmgrhandler_remdev(otherbtif_cmgr_handler_t), BTIF_BLP_SNIFF_MODE);
                    }
                }else{
                    app_bt_Me_SetLinkPolicy( btif_me_get_callback_event_sco_connect_rem_dev(Event), BTIF_BLP_SNIFF_MODE);
                }
            }
            break;
        default:
            break;
    }
}

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_global_handle_hook_handler[APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY] = {0};
void app_bt_global_handle_hook(const btif_event_t *Event)
{
    uint8_t i;
    for (i=0; i<APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY; i++){
        if (app_bt_global_handle_hook_handler[i])
            app_bt_global_handle_hook_handler[i](Event);
    }
}

int app_bt_global_handle_hook_set(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user, APP_BT_GOLBAL_HANDLE_HOOK_HANDLER handler)
{
    app_bt_global_handle_hook_handler[user] = handler;
    return 0;
}

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_global_handle_hook_get(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user)
{
    return app_bt_global_handle_hook_handler[user];
}

#if !defined(IBRT)
static uint8_t app_bt_get_mobile_index(bt_bdaddr_t *_addr)
{
    uint8_t index=0;
    for (index=0; index<BT_DEVICE_NUM; index++)
    {
        if (app_bt_device.bt_mobile_info[index].used == true)
        {
            if (!memcmp(app_bt_device.bt_mobile_info[index].addr.address, _addr->address, 6))
            {
                break;
            }
        }
    }

    TRACE(2, "%s %d", __func__, index);
    return index;
}

static void app_bt_save_mobile_info(bt_bdaddr_t *_addr)
{
    uint8_t index=BT_DEVICE_NUM;

    if (app_bt_get_mobile_index(_addr) < BT_DEVICE_NUM)
    {
        TRACE(1, "%s already save the info, ignore to save", __func__);
    }
    else
    {
        for (index=0; index<BT_DEVICE_NUM; index++)
        {
            if (app_bt_device.bt_mobile_info[index].used == false)
            {
                app_bt_device.bt_mobile_info[index].used = true;
                memcpy(app_bt_device.bt_mobile_info[index].addr.address, _addr->address, 6);
				TRACE(2,"%s index %d addr:", __func__, index);
				DUMP8("%x ", _addr->address, 6);
                break;
            }
        }
		if(index >= BT_DEVICE_NUM)
		{
			TRACE(2,"%s index %d, bt_mobile_info list is full, ignore to save", __func__,index);
		}
		
    }
	return;

}
static void app_bt_clear_mobile_info(bt_bdaddr_t *_addr)
{
    uint8_t index = app_bt_get_mobile_index(_addr);

    TRACE(2,"%s index %d addr:", __func__, index);
    DUMP8("%x ", _addr->address, 6);

    if (index >= BT_DEVICE_NUM)
    {
        TRACE(2,"%s don't have this addr info, index=%d", __func__, index);
    }
    else
    {
        app_bt_device.bt_mobile_info[index].used = false;
        memset(app_bt_device.bt_mobile_info[index].addr.address, 0, 6);
    }

    return;
}

void app_bt_disconnect_all_mobile_link(void)
{
    btif_remote_device_t *p_remote_dev;

    TRACE(1,"%s", __func__);

    for (uint8_t index=0; index<BT_DEVICE_NUM; index++)
    {
        if (app_bt_device.bt_mobile_info[index].used == true)
        {
            TRACE(1,"app_bt_disconnect index %d addr:", index);
            DUMP8("%x ", app_bt_device.bt_mobile_info[index].addr.address, 6);
            p_remote_dev = btif_me_get_remote_device_by_bdaddr(&app_bt_device.bt_mobile_info[index].addr);
            btif_me_force_disconnect_link_with_reason(NULL, p_remote_dev, BTIF_BEC_USER_TERMINATED, TRUE);
        }
    }
}

#endif

extern void a2dp_update_music_link(void);
/////There is a device connected, so stop PAIR_TIMER and POWEROFF_TIMER of earphone.
btif_handler app_bt_handler;
void app_bt_global_handle(const btif_event_t *Event)
{
    switch (btif_me_get_callback_event_type(Event)) {
        case BTIF_BTEVENT_HCI_INITIALIZED:
            break;
#if defined(IBRT)
        case BTIF_BTEVENT_HCI_COMMAND_SENT:
            return;
#else
        case BTIF_BTEVENT_HCI_COMMAND_SENT:
        case BTIF_BTEVENT_ACL_DATA_NOT_ACTIVE:
            return;
        case BTIF_BTEVENT_ACL_DATA_ACTIVE:
            btif_cmgr_handler_t    *cmgrHandler;
            /* Start the sniff timer */
            cmgrHandler = btif_cmgr_get_acl_handler(btif_me_get_callback_event_rem_dev(Event));
            if (cmgrHandler)
                app_bt_CMGR_SetSniffTimer(cmgrHandler, NULL, BTIF_CMGR_SNIFF_TIMER);
            return;
        case BTIF_BTEVENT_ENCRYPTION_CHANGE:
        {
            uint8_t devId = 
                app_bt_get_devId_from_RemDev(btif_me_get_callback_event_rem_dev(Event));
            if (BTIF_BEC_NO_ERROR == btif_me_get_callback_event_err_code(Event))
            {                
                app_bt_set_encrypted_state(devId, true);
            }
            else
            {
                app_bt_set_encrypted_state(devId, false);
            }
            break;  
        }
#endif
         case BTIF_BTEVENT_AUTHENTICATED:
            TRACE(1,"[BTEVENT] HANDER AUTH error=%x", btif_me_get_callback_event_err_code(Event));
            //after authentication completes, re-enable sniff mode.
            if(btif_me_get_callback_event_err_code(Event) == BTIF_BEC_NO_ERROR)
            {
                //app_bt_Me_SetLinkPolicy(btif_me_get_callback_event_rem_dev( Event),BTIF_BLP_SNIFF_MODE);
            }
            else if (btif_me_get_callback_event_err_code(Event) == BTIF_BEC_AUTHENTICATE_FAILURE)
            {
                //auth failed should clear nv record link key
                bt_bdaddr_t *bd_ddr = btif_me_get_callback_event_rem_dev_bd_addr(Event);
                btif_device_record_t record;
                if (ddbif_find_record(bd_ddr, &record) == BT_STS_SUCCESS)
                {
                    ddbif_delete_record(&record.bdAddr);
                    memset(&record, 0, sizeof(record));
                    record.bdAddr = *bd_ddr;
                    ddbif_add_record(&record);
                }
            }
            break;
    }
    // trace filter
    switch (btif_me_get_callback_event_type(Event)) {
        case BTIF_BTEVENT_HCI_COMMAND_SENT:
        case BTIF_BTEVENT_ACL_DATA_NOT_ACTIVE:
        case BTIF_BTEVENT_ACL_DATA_ACTIVE:
            break;
        default:
            TRACE(1,"[BTEVENT] evt = %d", btif_me_get_callback_event_type(Event));
            break;
    }

    switch (btif_me_get_callback_event_type(Event)) {
        case BTIF_BTEVENT_LINK_CONNECT_IND:
            hfp_reconnecting_timer_stop_callback(Event);//c by pang
        case BTIF_BTEVENT_LINK_CONNECT_CNF:
#ifdef __BT_ONE_BRING_TWO__
            if(bt_drv_get_reconnecting_flag())
            {
                bt_drv_clear_reconnecting_flag();
                if(a2dp_is_music_ongoing())
                    a2dp_update_music_link();
            }
#endif

            if (BTIF_BEC_NO_ERROR == btif_me_get_callback_event_err_code(Event))
            {
                connectedMobile = btif_me_get_callback_event_rem_dev( Event);
                uint8_t connectedDevId = app_bt_get_devId_from_RemDev(connectedMobile);
                app_bt_set_connecting_profiles_state(connectedDevId);
                TRACE(1,"MEC(pendCons) is %d", btif_me_get_pendCons());

                app_bt_stay_active_rem_dev(btif_me_get_callback_event_rem_dev( Event));
#ifdef __BT_ONE_BRING_TWO__
                btif_remote_device_t *remote_dev = btif_me_get_callback_event_rem_dev(Event);
                uint16_t conn_handle = btif_me_get_remote_device_hci_handle(remote_dev);
                btif_me_qos_set_up(conn_handle);
#endif

#if defined(BISTO_ENABLED) && !defined(IBRT)
                //gsound_custom_bt_link_connected_handler(btif_me_get_callback_event_rem_dev_bd_addr( Event)->address);
#endif
#if (defined(__AI_VOICE__) || defined(BISTO_ENABLED))&& !defined(IBRT)
                app_ai_if_mobile_connect_handle(btif_me_get_callback_event_rem_dev_bd_addr(Event));
#endif
#ifndef IBRT
                app_bt_save_mobile_info(btif_me_get_callback_event_rem_dev_bd_addr(Event));
#endif
            }

            TRACE(4,"[BTEVENT] CONNECT_IND/CNF evt:%d errCode:0x%0x newRole:%d activeCons:%d",btif_me_get_callback_event_type(Event),
                btif_me_get_callback_event_err_code(Event),btif_me_get_callback_event_rem_dev_role (Event), btif_me_get_activeCons());
            DUMP8("%02x ", btif_me_get_callback_event_rem_dev_bd_addr(Event), BTIF_BD_ADDR_SIZE);

#if defined(__BTIF_EARPHONE__) && defined(__BTIF_AUTOPOWEROFF__)  && !defined(FPGA)
            if (btif_me_get_activeCons() == 0){
                //app_start_10_second_timer(APP_POWEROFF_TIMER_ID);//c by pang
            }else{
                //app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);//c by pang
            }
#endif
#if defined(__BT_ONE_BRING_TWO__)||defined(IBRT)
            if (btif_me_get_activeCons() > 2){
                TRACE(1,"CONNECT_IND/CNF activeCons:%d so disconnect it", btif_me_get_activeCons());
                app_bt_MeDisconnectLink(btif_me_get_callback_event_rem_dev( Event));
            }
		
#else
            if (btif_me_get_activeCons() > 1){
                TRACE(1,"CONNECT_IND/CNF activeCons:%d so disconnect it", btif_me_get_activeCons());
                app_bt_MeDisconnectLink(btif_me_get_callback_event_rem_dev( Event));
            }
#endif
            break;
        case BTIF_BTEVENT_LINK_DISCONNECT:
        {
        #ifndef IBRT
            bool linkNeverCreated = false;
            if (NULL == connectedMobile)
            {
                linkNeverCreated = true;
            }
        #endif
            
            connectedMobile = btif_me_get_callback_event_rem_dev( Event);
            uint8_t disconnectedDevId = app_bt_get_devId_from_RemDev(connectedMobile);
            connectedMobile = NULL;
            
        #ifndef IBRT
            
            if (linkNeverCreated ||
                app_bt_is_no_profiles_ever_connected(disconnectedDevId))
            {
                app_bt_handling_on_abnormal_disconnection((enum BT_DEVICE_ID_T)disconnectedDevId,
                    btif_me_get_callback_event_rem_dev_bd_addr(Event),
                    btif_me_get_remote_device_disc_reason_saved(btif_me_get_callback_event_rem_dev(Event)));
            }
        #endif
        
            app_bt_clear_connecting_profiles_state(disconnectedDevId);

            app_bt_set_encrypted_state(disconnectedDevId, false);

            btif_remote_device_t *remote_dev = btif_me_get_callback_event_disconnect_rem_dev(Event);
            if(remote_dev)
            {
                uint16_t conhdl = btif_me_get_remote_device_hci_handle(remote_dev);
                bt_drv_acl_tx_silence_clear(conhdl);
                bt_drv_hwspi_select(conhdl-0x80, 0);
            }

            TRACE(5,"[BTEVENT] DISCONNECT evt = %d encryptState:%d reason:0x%02x/0x%02x activeCons:%d",
                                        btif_me_get_callback_event_type(Event),
                                        btif_me_get_remote_sevice_encrypt_state(btif_me_get_callback_event_rem_dev( Event)),
                                        btif_me_get_remote_device_disc_reason_saved(btif_me_get_callback_event_rem_dev( Event)),
                                        btif_me_get_remote_device_disc_reason(btif_me_get_callback_event_rem_dev( Event)),
                                        btif_me_get_activeCons());
            DUMP8("%02x ", btif_me_get_callback_event_rem_dev_bd_addr(Event), BTIF_BD_ADDR_SIZE);
            #ifdef CHIP_BEST2000
                 bt_drv_patch_force_disconnect_ack();
            #endif
             //disconnect from reconnect connection, and the HF don't connect successful once
             //(whitch will release the saved_reconnect_mode ). so we are reconnect fail with remote link key loss.
             // goto pairing.
             //reason 07 maybe from the controller's error .
             //05  auth error
             //16  io cap reject.

#if defined(__BTIF_EARPHONE__) && defined(__BTIF_AUTOPOWEROFF__) && !defined(FPGA)
            if (btif_me_get_activeCons() == 0){
                //app_start_10_second_timer(APP_POWEROFF_TIMER_ID);//close by pang
            }
#endif

#ifndef IBRT
            app_bt_clear_mobile_info(btif_me_get_callback_event_rem_dev_bd_addr(Event));
#endif

#if defined(BISTO_ENABLED) && !defined(IBRT)
            gsound_custom_bt_link_disconnected_handler( btif_me_get_remote_device_bdaddr(btif_me_get_callback_event_rem_dev( Event))->address);
#endif
#if defined(__AI_VOICE__) && !defined(IBRT)
            app_ai_mobile_disconnect_handle(btif_me_get_remote_device_bdaddr(btif_me_get_callback_event_rem_dev( Event)));
#endif
#ifdef  __IAG_BLE_INCLUDE__
            // start BLE adv
            //app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, true);//c by pang for TAH8506 APP
#endif

#ifdef BTIF_DIP_DEVICE
            btif_dip_clear(remote_dev);
#endif

            app_bt_active_mode_reset(disconnectedDevId);

#ifdef GFPS_ENABLED
            app_gfps_handling_on_mobile_link_disconnection();
#endif
            break;
        }
        case BTIF_BTEVENT_ROLE_CHANGE:
            TRACE(3,"[BTEVENT] ROLE_CHANGE eType:0x%x errCode:0x%x newRole:%d activeCons:%d", btif_me_get_callback_event_type(Event),
                btif_me_get_callback_event_err_code(Event), btif_me_get_callback_event_role_change_new_role(Event), btif_me_get_activeCons());
            break;
        case BTIF_BTEVENT_MODE_CHANGE:
            TRACE(4,"[BTEVENT] MODE_CHANGE evt:%d errCode:0x%0x curMode=0x%0x, interval=%d ",btif_me_get_callback_event_type(Event),
                btif_me_get_callback_event_err_code(Event), btif_me_get_callback_event_mode_change_curMode(Event),
                btif_me_get_callback_event_mode_change_interval(Event));
            DUMP8("%02x ", btif_me_get_callback_event_rem_dev_bd_addr(Event), BTIF_BD_ADDR_SIZE);
            break;
        case BTIF_BTEVENT_ACCESSIBLE_CHANGE:
            TRACE(3,"[BTEVENT] ACCESSIBLE_CHANGE evt:%d errCode:0x%0x aMode=0x%0x", btif_me_get_callback_event_type(Event),
                                                                                  btif_me_get_callback_event_err_code(Event),
                                                                                  btif_me_get_callback_event_a_mode(Event));
#if !defined(IBRT)
            if (app_is_access_mode_set_pending())
            {
                app_set_pending_access_mode();
            }
            else
            {
                if (BTIF_BEC_NO_ERROR != btif_me_get_callback_event_err_code(Event))
                {
                    app_retry_setting_access_mode();
                }
            }
#endif
            break;
        case BTIF_BTEVENT_LINK_POLICY_CHANGED:
        {
            BT_SET_LINKPOLICY_REQ_T* pReq = app_bt_pop_pending_set_linkpolicy();
            if (NULL != pReq)
            {
                app_bt_Me_SetLinkPolicy(pReq->remDev, pReq->policy);
            }
            break;
        }
        case BTIF_BTEVENT_DEFAULT_LINK_POLICY_CHANGED:
        {
            TRACE(0,"[BTEVENT] DEFAULT_LINK_POLICY_CHANGED-->BT_STACK_INITIALIZED");
            app_notify_stack_ready(STACK_READY_BT);
            break;
        }
        case BTIF_BTEVENT_NAME_RESULT:
        {
            uint8_t* ptrName;
            uint8_t nameLen;
			uint8_t cur_devid = app_cur_connect_devid_get();//add by cai
            nameLen = btif_me_get_callback_event_remote_dev_name(Event, &ptrName);
            TRACE(1,"[BTEVENT] NAME_RESULT name len %d", nameLen);
            if (nameLen > 0)
            {
                TRACE(2,"***remote dev name: %s, namelen: %d", ptrName, nameLen);
				//memcpy(remote_dev_name, ptrName, nameLen);////by pang  very important:open it will cause reconnect fail 
				//add by cai
				nameLen = nameLen > sizeof(dev_name_user[cur_devid])? sizeof(dev_name_user[cur_devid]) : nameLen;
				memset(dev_name_user[cur_devid], 0, sizeof(dev_name_user[cur_devid]));
				memcpy(dev_name_user[cur_devid], ptrName, nameLen);
				TRACE(2,"***dev_name_user: %s, nameLen: %d", dev_name_user[cur_devid], nameLen);
            }
            //return;
        }
        default:
            break;
    }

#ifdef MULTIPOINT_DUAL_SLAVE
    app_bt_role_manager_process_dual_slave(Event);
#else
    app_bt_role_manager_process(Event);
#endif
    app_bt_accessible_manager_process(Event);
#if !defined(IBRT)
    app_bt_sniff_manager_process(Event);
#endif
    app_bt_global_handle_hook(Event);
#if defined(IBRT)
    app_tws_ibrt_global_callback(Event);
#endif
}

#include "app_bt_media_manager.h"
osTimerId bt_sco_recov_timer = NULL;
static void bt_sco_recov_timer_handler(void const *param);
osTimerDef (BT_SCO_RECOV_TIMER, (void (*)(void const *))bt_sco_recov_timer_handler);                      // define timers
void hfp_reconnect_sco(uint8_t flag);
static void bt_sco_recov_timer_handler(void const *param)
{
    TRACE(1,"%s",__func__);
    hfp_reconnect_sco(0);
}
static void bt_sco_recov_timer_start()
{
    osTimerStop(bt_sco_recov_timer);
    osTimerStart(bt_sco_recov_timer, 2500);
}


enum{
    SCO_DISCONNECT_RECONN_START,
    SCO_DISCONNECT_RECONN_RUN,
    SCO_DISCONNECT_RECONN_NONE,
};

static uint8_t sco_reconnect_status =  SCO_DISCONNECT_RECONN_NONE;

void hfp_reconnect_sco(uint8_t set)
{
    TRACE(3,"%s cur_chl_id=%d reconnect_status =%d",__func__,app_bt_device.curr_hf_channel_id,
        sco_reconnect_status);
    if(set == 1){
        sco_reconnect_status = SCO_DISCONNECT_RECONN_START;
    }
    if(sco_reconnect_status == SCO_DISCONNECT_RECONN_START){
        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_VOICE, app_bt_device.curr_hf_channel_id,MAX_RECORD_NUM);
        app_bt_HF_DisconnectAudioLink(app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id]);
        sco_reconnect_status = SCO_DISCONNECT_RECONN_RUN;
        bt_sco_recov_timer_start();
    }else if(sco_reconnect_status == SCO_DISCONNECT_RECONN_RUN){
        app_bt_HF_CreateAudioLink(app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id]);
        sco_reconnect_status = SCO_DISCONNECT_RECONN_NONE;
    }
}


void app_bt_global_handle_init(void)
{
    btif_event_mask_t mask = BTIF_BEM_NO_EVENTS;
    btif_me_init_handler(&app_bt_handler);
    app_bt_handler.callback = app_bt_global_handle;
    btif_me_register_global_handler(&app_bt_handler);
#if defined(IBRT)
    btif_me_register_accept_handler(&app_bt_handler);
#endif
#ifdef IBRT_SEARCH_UI
    app_bt_global_handle_hook_set(APP_BT_GOLBAL_HANDLE_HOOK_USER_0,app_bt_manager_ibrt_role_process);
#endif

    mask |= BTIF_BEM_ROLE_CHANGE | BTIF_BEM_SCO_CONNECT_CNF | BTIF_BEM_SCO_DISCONNECT | BTIF_BEM_SCO_CONNECT_IND;
    mask |= BTIF_BEM_AUTHENTICATED;
    mask |= BTIF_BEM_LINK_CONNECT_IND;
    mask |= BTIF_BEM_LINK_DISCONNECT;
    mask |= BTIF_BEM_LINK_CONNECT_CNF;
    mask |= BTIF_BEM_ACCESSIBLE_CHANGE;
    mask |= BTIF_BEM_ENCRYPTION_CHANGE;
    mask |= BTIF_BEM_SIMPLE_PAIRING_COMPLETE;
#if (defined(__BT_ONE_BRING_TWO__)||defined(IBRT))
    mask |= BTIF_BEM_MODE_CHANGE;
#endif
    mask |= BTIF_BEM_LINK_POLICY_CHANGED;

    app_bt_ME_SetConnectionRole(BTIF_BCR_ANY);
    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_stream[i]->a2dp_stream, FALSE);
#if defined(A2DP_LHDC_ON)
        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_lhdc_stream[i]->a2dp_stream, FALSE);
#endif
#if defined(A2DP_AAC_ON)
        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_aac_stream[i]->a2dp_stream, FALSE);
#endif
#if defined(A2DP_SCALABLE_ON)
        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_scalable_stream[i]->a2dp_stream, FALSE);
#endif
#if defined(A2DP_LDAC_ON)
        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_ldac_stream[i]->a2dp_stream, FALSE);
#endif

        app_bt_HF_SetMasterRole(app_bt_device.hf_channel[i], FALSE);
#if defined (__HSP_ENABLE__)
        HS_SetMasterRole(&app_bt_device.hs_channel[i], FALSE);
#endif
    }
    btif_me_set_event_mask(&app_bt_handler, mask);
    app_bt_sniff_manager_init();
    app_bt_accessmode_timer = osTimerCreate (osTimer(APP_BT_ACCESSMODE_TIMER), osTimerOnce, &app_bt_accessmode_timer_argument);
    bt_sco_recov_timer = osTimerCreate (osTimer(BT_SCO_RECOV_TIMER), osTimerOnce, NULL);
}

void app_bt_send_request(uint32_t message_id, uint32_t param0, uint32_t param1, uint32_t ptr)
{
    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODUAL_BT;
    msg.msg_body.message_id = message_id;
    msg.msg_body.message_Param0 = param0;
    msg.msg_body.message_Param1 = param1;
    msg.msg_body.message_ptr = ptr;
    app_mailbox_put(&msg);
}

extern void app_start_10_second_timer(uint8_t timer_id);

static int app_bt_handle_process(APP_MESSAGE_BODY *msg_body)
{
    btif_accessible_mode_t old_access_mode;

    switch (msg_body->message_id) {
        case APP_BT_REQ_ACCESS_MODE_SET:
            old_access_mode = g_bt_access_mode;
            app_bt_accessmode_set(msg_body->message_Param0);
            if (msg_body->message_Param0 == BTIF_BAM_GENERAL_ACCESSIBLE &&
                old_access_mode != BTIF_BAM_GENERAL_ACCESSIBLE){
#if 0                	                       
#ifndef FPGA
                app_status_indication_set(APP_STATUS_INDICATION_BOTHSCAN);
#ifdef MEDIA_PLAYER_SUPPORT
                app_voice_report(APP_STATUS_INDICATION_BOTHSCAN, 0);
#endif
                app_start_10_second_timer(APP_PAIR_TIMER_ID);
#endif
            	}
#else // m by pang
				if(app_poweroff_flag)
					return 0;

				TRACE(5,"%s lostconncection_to_pairing=%d", __func__,lostconncection_to_pairing);
			 	if((lostconncection_to_pairing==0) && (APP_STATUS_INDICATION_BOTHSCAN != app_status_indication_get())){					
					lostconncection_to_pairing=1;								   
#ifndef FPGA
					app_status_indication_set(APP_STATUS_INDICATION_BOTHSCAN);
#ifdef MEDIA_PLAYER_SUPPORT
					app_voice_report(APP_STATUS_INDICATION_BOTHSCAN, 0);
#endif
					app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
#endif
				}
#endif				
            }else{
#ifndef FPGA
               //app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);//close by pang
#endif
            }
            break;
        default:
            break;
    }

    return 0;
}

void *app_bt_profile_active_store_ptr_get(uint8_t *bdAddr)
{
#if defined(IBRT)
    static btdevice_profile device_profile = {true, false, true,0};
#else
    static btdevice_profile device_profile = {false, false, false,0};
#endif
    btdevice_profile *ptr;

#ifndef FPGA
    nvrec_btdevicerecord *record = NULL;
    if (!nv_record_btdevicerecord_find((bt_bdaddr_t *)bdAddr,&record)){
        ptr = &(record->device_plf);
        DUMP8("0x%02x ", bdAddr, BTIF_BD_ADDR_SIZE);
        TRACE(5,"%s hfp_act:%d hsp_act:%d a2dp_act:0x%x codec_type=%x", __func__, ptr->hfp_act, ptr->hsp_act, ptr->a2dp_act,ptr->a2dp_codectype);
    }else
#endif
    {
        ptr = &device_profile;
        TRACE(1,"%s default", __func__);
    }
    return (void *)ptr;
}

static void app_bt_profile_reconnect_timehandler(void const *param);

osTimerDef (BT_PROFILE_CONNECT_TIMER0, app_bt_profile_reconnect_timehandler);                      // define timers
#ifdef __BT_ONE_BRING_TWO__
osTimerDef (BT_PROFILE_CONNECT_TIMER1, app_bt_profile_reconnect_timehandler);
#endif

#ifdef __AUTO_CONNECT_OTHER_PROFILE__
static void app_bt_profile_connect_hf_retry_handler(void)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
    if (MEC(pendCons) > 0)
    {
        TRACE(1,"Former link is not down yet, reset the timer %s.", __FUNCTION__);
        osTimerStart(bt_profile_manager_p->connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
    }
    else
    {
        app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
        if (bt_profile_manager_p->hfp_connect != bt_profile_connect_status_success)
        {
            app_bt_HF_CreateServiceLink(bt_profile_manager_p->chan, &bt_profile_manager_p->rmt_addr);
        }
    }
}

static void app_bt_profile_connect_hf_retry_timehandler(void const *param)
{
    app_bt_start_custom_function_in_bt_thread(0, 0,
        (uint32_t)app_bt_profile_connect_hf_retry_handler);
}

#if defined (__HSP_ENABLE__)
static void app_bt_profile_connect_hs_retry_timehandler(void const *param)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
    if (MEC(pendCons) > 0)
    {
        if (bt_profile_manager_p->reconnect_cnt < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT)
        {
            bt_profile_manager_p->reconnect_cnt++;
        }
        TRACE(1,"Former link is not down yet, reset the timer %s.", __FUNCTION__);
        osTimerStart(bt_profile_manager_p->connect_timer,
            BTIF_BT_DEFAULT_PAGE_TIMEOUT_IN_MS+APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
    }
    else
    {
        if (bt_profile_manager_p->hsp_connect != bt_profile_connect_status_success)
        {
            app_bt_HS_CreateServiceLink(bt_profile_manager_p->hs_chan, &bt_profile_manager_p->rmt_addr);
        }
    }
}
#endif

static bool app_bt_profile_manager_connect_a2dp_filter_connected_a2dp_stream(BT_BD_ADDR bd_addr)
{
    uint8_t i =0;
    BtRemoteDevice *StrmRemDev;
    A2dpStream * connected_stream;

    for(i =0;i<BT_DEVICE_NUM;i++){
        if((app_bt_device.a2dp_stream[i].stream.state == AVDTP_STRM_STATE_STREAMING ||
            app_bt_device.a2dp_stream[i].stream.state == AVDTP_STRM_STATE_OPEN)){
            connected_stream = &app_bt_device.a2dp_stream[i];
            StrmRemDev = A2DP_GetRemoteDevice(connected_stream);
            if(memcmp(StrmRemDev->bdAddr.addr,bd_addr.addr,BD_ADDR_SIZE) == 0){
                return true;
            }
        }
    }
    return false;
}

static void app_bt_profile_connect_a2dp_retry_handler(void)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
	TRACE(1,"%s reconnect_cnt = %d",__func__,bt_profile_manager_p->reconnect_cnt);

    if (MEC(pendCons) > 0)
    {
        if (bt_profile_manager_p->reconnect_cnt < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT)
        {
            bt_profile_manager_p->reconnect_cnt++;
        }
        TRACE(1,"Former link is not down yet, reset the timer %s.", __FUNCTION__);
        osTimerStart(bt_profile_manager_p->connect_timer,
            BTIF_BT_DEFAULT_PAGE_TIMEOUT_IN_MS+APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
    }
    else
    {
        if(app_bt_profile_manager_connect_a2dp_filter_connected_a2dp_stream(bt_profile_manager_p->rmt_addr) == true){
            TRACE(0,"has been connected , no need to init connect again");
            return ;
        }
        app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
        if (bt_profile_manager_p->a2dp_connect != bt_profile_connect_status_success)
        {
            app_bt_A2DP_OpenStream(bt_profile_manager_p->stream, &bt_profile_manager_p->rmt_addr);
        }
    }
}

static void app_bt_profile_connect_a2dp_retry_timehandler(void const *param)
{
    app_bt_start_custom_function_in_bt_thread(0, 0,
        (uint32_t)app_bt_profile_connect_a2dp_retry_handler);
}
#endif

void app_bt_reset_reconnect_timer(bt_bdaddr_t *pBdAddr)
{
    uint8_t devId = 0;
    for (uint8_t i = 0; i < BT_DEVICE_NUM; i++)
    {
        if (pBdAddr == &(bt_profile_manager[i].rmt_addr))
        {
            devId = i;
            break;
        }
    }

    TRACE(1,"Resart the reconnecting timer of dev %d", devId);
    osTimerStart(bt_profile_manager[devId].connect_timer,
        BTIF_BT_DEFAULT_PAGE_TIMEOUT_IN_MS+APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
}

static void app_bt_update_connectable_mode_after_connection_management(void);//add by pang
static void app_bt_profile_reconnect_handler(void const *param)
{
#if !defined(IBRT)
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
	TRACE(1,"%s reconnect_cnt = %d",__FUNCTION__,bt_profile_manager_p->reconnect_cnt);

    if ( btif_me_get_pendCons() > 0)
    {
        if (bt_profile_manager_p->reconnect_cnt < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT)
        {
            //bt_profile_manager_p->reconnect_cnt++;//c by pang
        }
        TRACE(1,"Former link is not down yet, reset the timer %s.", __FUNCTION__);
        osTimerStart(bt_profile_manager_p->connect_timer,
            BTIF_BT_DEFAULT_PAGE_TIMEOUT_IN_MS+APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
    }
    else
    {
        btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager_p->rmt_addr.address);
#ifdef __BT_ONE_BRING_TWO__
        if(a2dp_is_music_ongoing()&&(bt_profile_manager_p->has_connected == false))
        {
            bt_drv_set_reconnecting_flag();
            a2dp_update_music_link();
        }
#endif

        if (bt_profile_manager_p->connect_timer_cb){
            bt_profile_manager_p->connect_timer_cb(param);
            bt_profile_manager_p->connect_timer_cb = NULL;
        }else{
            if((btdevice_plf_p->a2dp_act)
                &&(bt_profile_manager_p->a2dp_connect != bt_profile_connect_status_success)){
                TRACE(0,"try connect a2dp");
                app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
                app_bt_A2DP_OpenStream(bt_profile_manager_p->stream, &bt_profile_manager_p->rmt_addr);
            }
#if defined (__HSP_ENABLE__)
            else if(btdevice_plf_p->hsp_act)
			 &&(bt_profile_manager_p->hsp_connect != bt_profile_connect_status_success)){
                TRACE(0,"try connect hs");
                app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
                app_bt_HS_CreateServiceLink(bt_profile_manager_p->hs_chan, &bt_profile_manager_p->rmt_addr);
            }
#endif
            else if ((btdevice_plf_p->hfp_act)
                &&(bt_profile_manager_p->hfp_connect != bt_profile_connect_status_success)){
                TRACE(0,"try connect hf");
                app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
                app_bt_HF_CreateServiceLink(bt_profile_manager_p->chan, (bt_bdaddr_t *)&bt_profile_manager_p->rmt_addr);
            }
        }
    }
#else
    TRACE(0,"ibrt_ui_log:app_bt_profile_reconnect_timehandler called");
#endif
}

static void app_bt_profile_reconnect_timehandler(void const *param)
{
    app_bt_start_custom_function_in_bt_thread((uint32_t)param, 0,
        (uint32_t)app_bt_profile_reconnect_handler);
}

bool app_bt_is_in_connecting_profiles_state(void)
{
    for (uint8_t devId = 0;devId < BT_DEVICE_NUM;devId++)
    {
        if (APP_BT_IN_CONNECTING_PROFILES_STATE == bt_profile_manager[devId].connectingState)
        {
            return true;
        }
    }

    return false;
}

void app_bt_clear_connecting_profiles_state(uint8_t devId)
{
    TRACE(1,"Dev %d exists connecting profiles state", devId);

    bt_profile_manager[devId].connectingState = APP_BT_IDLE_STATE;
    if (!app_bt_is_in_connecting_profiles_state())
    {
#ifdef __IAG_BLE_INCLUDE__
        app_start_fast_connectable_ble_adv(BLE_FAST_ADVERTISING_INTERVAL);
#endif
    }
}

#if !defined(IBRT)
bool app_bt_is_no_profiles_ever_connected(uint8_t devId)
{
    return (APP_BT_IN_CONNECTING_PROFILES_STATE == 
        bt_profile_manager[devId].connectingState);
}
#endif

void app_bt_set_connecting_profiles_state(uint8_t devId)
{
    TRACE(1,"Dev %d enters connecting profiles state", devId);

    bt_profile_manager[devId].connectingState = APP_BT_IN_CONNECTING_PROFILES_STATE;
}

void app_bt_set_encrypted_state(uint8_t devId, bool isEncrypted)
{
    bt_profile_manager[devId].isEncrypted = isEncrypted;
}

bool app_bt_is_link_encrypted(uint8_t devId)
{
    return bt_profile_manager[devId].isEncrypted;
}

void app_bt_profile_connect_manager_open(void)
{
    uint8_t i=0;
    for (i=0;i<BT_DEVICE_NUM;i++){
        bt_profile_manager[i].has_connected = false;
        bt_profile_manager[i].isEncrypted = false;
        bt_profile_manager[i].hfp_connect = bt_profile_connect_status_unknow;
        bt_profile_manager[i].hsp_connect = bt_profile_connect_status_unknow;
        bt_profile_manager[i].a2dp_connect = bt_profile_connect_status_unknow;
        memset(bt_profile_manager[i].rmt_addr.address, 0, BTIF_BD_ADDR_SIZE);
        bt_profile_manager[i].reconnect_mode = bt_profile_reconnect_null;
        bt_profile_manager[i].saved_reconnect_mode = bt_profile_reconnect_null;
        bt_profile_manager[i].stream = NULL;
        bt_profile_manager[i].chan = NULL;
#if defined (__HSP_ENABLE__)
        bt_profile_manager[i].hs_chan = NULL;
#endif
        bt_profile_manager[i].reconnect_cnt = 0;
        bt_profile_manager[i].connect_timer_cb = NULL;
        bt_profile_manager[i].connectingState = APP_BT_IDLE_STATE;
    }

    bt_profile_manager[BT_DEVICE_ID_1].connect_timer = osTimerCreate (osTimer(BT_PROFILE_CONNECT_TIMER0), osTimerOnce, &bt_profile_manager[BT_DEVICE_ID_1]);
#ifdef __BT_ONE_BRING_TWO__
    bt_profile_manager[BT_DEVICE_ID_2].connect_timer = osTimerCreate (osTimer(BT_PROFILE_CONNECT_TIMER1), osTimerOnce, &bt_profile_manager[BT_DEVICE_ID_2]);
#endif
}

BOOL app_bt_profile_connect_openreconnecting(void *ptr)
{
    bool nRet = false;
    uint8_t i;

    /*
     * If launched from peer device,stop reconnecting and accept connection
     */
    if ((ptr != NULL) && (btif_me_get_remote_device_initiator((btif_remote_device_t *)ptr) == FALSE))
    {
        //Peer device launch reconnet,then we give up reconnect procedure
        TRACE(0,"give up reconnecting");
        app_bt_connectable_mode_stop_reconnecting();
        return false;
    }

    for (i=0;i<BT_DEVICE_NUM;i++){
        nRet |= bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting ? true : false;
        if(nRet){
            TRACE(2,"io cap rj [%d]: %d", i, bt_profile_manager[i].reconnect_mode);
        }
    }

    return nRet;
}

/** add by pang **/
BOOL app_bt_profile_connect_reconnecting(void)
{
   uint8_t devId;

    for (devId=0;devId<BT_DEVICE_NUM;devId++){
        if(bt_profile_manager[devId].reconnect_mode == bt_profile_reconnect_reconnecting)
        	return true;
    }

    return false;
}
/** end add **/

bool app_bt_is_in_reconnecting(void)
{
    uint8_t devId;
    for (devId = 0;devId < BT_DEVICE_NUM;devId++)
    {
        if (bt_profile_reconnect_null != bt_profile_manager[devId].reconnect_mode)
        {
            return true;
        }
    }

    return false;
}

void app_bt_profile_connect_manager_opening_reconnect(void)
{
    int ret;
    btif_device_record_t record1;
    btif_device_record_t record2;
	btif_device_record_t last_active_recored;
    btdevice_profile *btdevice_plf_p;

/** add by pang **/
	bool last_active_device=0;
	uint8_t MAX_BT_DEVICE_COUNT=0;
/** end add **/	
	
    btdevice_profile *btdevice_plf_p2;
    int find_invalid_record_cnt;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
    if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
    {
        return ;
    }
#endif
    osapi_lock_stack();

#ifndef __BT_ONE_BRING_TWO__
    if(btif_me_get_activeCons() != 0){
        osapi_unlock_stack();
        TRACE(0,"bt link disconnect not complete,ignore this time reconnect");
        return;
    }
#endif
	MAX_BT_DEVICE_COUNT=9;//add by pang
    do{
		/** add by pang **/
		MAX_BT_DEVICE_COUNT--;
		TRACE(1,"***MAX_BT_DEVICE_COUNT=%d\n",MAX_BT_DEVICE_COUNT);
		if(MAX_BT_DEVICE_COUNT==0){
			ret=0;
			break;
		}
		/** end add **/
        find_invalid_record_cnt = 0;
        ret = nv_record_enum_latest_two_paired_dev(&record1,&record2);
		last_active_recored = record1;
        if(ret == 1){
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(record1.bdAddr.address);
			
            if (!(btdevice_plf_p->hfp_act)&&!(btdevice_plf_p->a2dp_act)){
                nv_record_ddbrec_delete((bt_bdaddr_t *)&record1.bdAddr);
                find_invalid_record_cnt++;
            }
			nv_record_btdevicerecord_set_last_active(btdevice_plf_p);
        }else if(ret == 2){
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(record1.bdAddr.address);
            if (!(btdevice_plf_p->hfp_act)&&!(btdevice_plf_p->a2dp_act)){
                nv_record_ddbrec_delete((bt_bdaddr_t *)&record1.bdAddr);
                find_invalid_record_cnt++;
            }else{
				TRACE(0,"record1 device_plf 0x%x,p_last_active 0x%x",(uint32_t)btdevice_plf_p,btdevice_plf_p->p_last_active);
        	}
            btdevice_plf_p2 = (btdevice_profile *)app_bt_profile_active_store_ptr_get(record2.bdAddr.address);
            if (!(btdevice_plf_p2->hfp_act)&&!(btdevice_plf_p2->a2dp_act)){
                nv_record_ddbrec_delete((bt_bdaddr_t *)&record2.bdAddr);
                find_invalid_record_cnt++;
            }else {
				TRACE(0,"record2 device_plf 0x%x,p_last_active 0x%x",(uint32_t)btdevice_plf_p2,btdevice_plf_p2->p_last_active);
				if( btdevice_plf_p2->p_last_active > btdevice_plf_p->p_last_active){	
					last_active_recored = record2;
					//last_active_device=1;//add by pang
					TRACE(0,"set last_active_recored to record2");
				}
            }
			nv_record_btdevicerecord_set_last_active(btdevice_plf_p);
			nv_record_btdevicerecord_set_last_active(btdevice_plf_p2);
        }
    }while(find_invalid_record_cnt);

	/** add by pang **/
	if(last_active_device){
		record2=record1;
		record1=last_active_recored;
	}
	/** end add **/
		
    TRACE(0,"!!!app_bt_opening_reconnect:\n");
    DUMP8("%02x ", &record1.bdAddr, 6);
    DUMP8("%02x ", &record2.bdAddr, 6);
    TRACE(0,"!!!last record addr:\n");
    DUMP8("%02x ", &last_active_recored.bdAddr, 6);
    //record1 = last_active_recored;//close by pang for reconnect the last two devices
	
    if(ret > 0){
        TRACE(0,"!!!start reconnect first device\n");

        if(  btif_me_get_pendCons() == 0){
            bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_openreconnecting;
            bt_profile_manager[BT_DEVICE_ID_1].reconnect_cnt = 0;
            memcpy(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.address, record1.bdAddr.address, BTIF_BD_ADDR_SIZE);
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.address);

#if defined(A2DP_LHDC_ON)
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
                bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream;
            else
#endif
#if defined(A2DP_AAC_ON)
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC)
                bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream;
            else
#endif
#if defined(A2DP_LDAC_ON)  //workaround for mate10 no a2dp issue when link back
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP){
                //bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream;
                //btdevice_plf_p->a2dp_codectype = BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC;
                bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream;
                // btdevice_plf_p->a2dp_codectype = BTIF_AVDTP_CODEC_TYPE_NON_A2DP;
            }
            else
#endif

#if defined(A2DP_SCALABLE_ON)
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
                bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream;
            else
#endif
            {
                bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream;
            }

            bt_profile_manager[BT_DEVICE_ID_1].chan = app_bt_device.hf_channel[BT_DEVICE_ID_1];
#if defined (__HSP_ENABLE__)
            bt_profile_manager[BT_DEVICE_ID_1].hs_chan = &app_bt_device.hs_channel[BT_DEVICE_ID_1];
#endif
            btif_a2dp_reset_stream_state(bt_profile_manager[BT_DEVICE_ID_1].stream);

			if(btdevice_plf_p->a2dp_act) {
                TRACE(0,"try connect a2dp");
                app_bt_precheck_before_starting_connecting(bt_profile_manager[BT_DEVICE_ID_1].has_connected);
                app_bt_A2DP_OpenStream(bt_profile_manager[BT_DEVICE_ID_1].stream, &bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
            }
            else if (btdevice_plf_p->hfp_act){
				TRACE(0,"try connect hf");
				app_bt_precheck_before_starting_connecting(bt_profile_manager[BT_DEVICE_ID_1].has_connected);
				app_bt_HF_CreateServiceLink(bt_profile_manager[BT_DEVICE_ID_1].chan, (bt_bdaddr_t *)&bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
			}
           
#if defined (__HSP_ENABLE__)
            else if (btdevice_plf_p->hsp_act){
                TRACE(0,"try connect hs");
                app_bt_precheck_before_starting_connecting(bt_profile_manager[BT_DEVICE_ID_1].has_connected);
                app_bt_HS_CreateServiceLink(bt_profile_manager[BT_DEVICE_ID_1].hs_chan, &bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
            }
#endif

        }
#ifdef __BT_ONE_BRING_TWO__   //open by cai
        if(ret > 1){
            TRACE(0,"!!!need reconnect second device\n");
            bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode = bt_profile_reconnect_openreconnecting;
            bt_profile_manager[BT_DEVICE_ID_2].reconnect_cnt = 0;
            memcpy(bt_profile_manager[BT_DEVICE_ID_2].rmt_addr.address, record2.bdAddr.address, BTIF_BD_ADDR_SIZE);
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager[BT_DEVICE_ID_2].rmt_addr.address);

#if defined(A2DP_LHDC_ON)
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
                bt_profile_manager[BT_DEVICE_ID_2].stream = app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_2]->a2dp_stream;
            else
#endif
#if defined(A2DP_AAC_ON)
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC)
                bt_profile_manager[BT_DEVICE_ID_2].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2]->a2dp_stream;
            else
#endif
#if defined(A2DP_SCALABLE_ON)
            if(btdevice_plf_p->a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
                bt_profile_manager[BT_DEVICE_ID_2].stream = app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2]->a2dp_stream;
            else
#endif
            {
                bt_profile_manager[BT_DEVICE_ID_2].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_2]->a2dp_stream;
            }
            btif_a2dp_reset_stream_state(bt_profile_manager[BT_DEVICE_ID_2].stream);
            bt_profile_manager[BT_DEVICE_ID_2].chan = app_bt_device.hf_channel[BT_DEVICE_ID_2];
#if defined (__HSP_ENABLE__)
            bt_profile_manager[BT_DEVICE_ID_2].hs_chan = &app_bt_device.hs_channel[BT_DEVICE_ID_2];
#endif
        }
#endif
/** add by pang **/
		if(0==power_on_open_reconnect_flag){
			reconnect_timeout_set(0);
		}		
		app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
/** end add **/
    }

    else
    {
        TRACE(0,"!!!go to pairing\n");
#ifdef __EARPHONE_STAY_BOTH_SCAN__
        app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
#else
        //app_bt_accessmode_set_req(BTIF_BAM_CONNECTABLE_ONLY);
		app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);//m by pang
#endif
    }
	power_on_open_reconnect_flag=1;//add by pang
    osapi_unlock_stack();
}


void app_bt_resume_sniff_mode(uint8_t deviceId)
{
    if (bt_profile_connect_status_success == bt_profile_manager[deviceId].a2dp_connect||
        bt_profile_connect_status_success == bt_profile_manager[deviceId].hfp_connect||
        bt_profile_connect_status_success == bt_profile_manager[deviceId].hsp_connect)
    {
        app_bt_allow_sniff(deviceId);
        btif_remote_device_t* currentRemDev = app_bt_get_remoteDev(deviceId);
        app_bt_sniff_config(currentRemDev);
    }
}
#if !defined(IBRT)
static int8_t app_bt_profile_reconnect_pending(enum BT_DEVICE_ID_T id)
{
    TRACE(2,"%s call_active %d", __func__, btapp_hfp_is_dev_call_active(id));
    if(btapp_hfp_is_dev_call_active(id) == true){
        bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnect_pending;
        return 0;
    }
    return -1;
}
#endif
static int8_t app_bt_profile_reconnect_pending_process(void)
{
    uint8_t i =BT_DEVICE_NUM;

    btif_remote_device_t *remDev = NULL;
    btif_cmgr_handler_t    *cmgrHandler;


    for (i=0; i<BT_DEVICE_NUM; i++){
        remDev = btif_me_enumerate_remote_devices(i);
        if (remDev != NULL) {
            cmgrHandler = btif_cmgr_get_acl_handler(remDev);
            if(btif_cmgr_is_audio_up(cmgrHandler) == 1)
                return -1;
        }
    }
    for(i = 0;i < BT_DEVICE_NUM;i++){
        if(bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_reconnect_pending)
            break;
    }

    if(i == BT_DEVICE_NUM)
        return -1;

    bt_profile_manager[i].reconnect_mode = bt_profile_reconnect_reconnecting;
#ifdef __IAG_BLE_INCLUDE__
    app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
#endif
    TRACE(1, "%s", __func__);
    osTimerStart(bt_profile_manager[i].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
    return 0;
}

uint8_t app_bt_get_num_of_connected_dev(void)
{
    uint8_t num_of_connected_dev = 0;
    uint8_t deviceId;

    for (deviceId = 0; deviceId < BT_DEVICE_NUM; deviceId++)
    {
        if (bt_profile_manager[deviceId].has_connected)
        {
            num_of_connected_dev++;
        }
    }

    return num_of_connected_dev;
}

static uint8_t recorded_latest_connected_service_device_id = BT_DEVICE_ID_1;
void app_bt_record_latest_connected_service_device_id(uint8_t device_id)
{
    recorded_latest_connected_service_device_id = device_id;
}

uint8_t app_bt_get_recorded_latest_connected_service_device_id(void)
{
    return recorded_latest_connected_service_device_id;
}


static void app_bt_precheck_before_starting_connecting(uint8_t isBtConnected)
{
#ifdef __IAG_BLE_INCLUDE__
    if (!isBtConnected)
    {
        app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, false);
    }
#endif
}

static void app_bt_restore_reconnecting_idle_mode(uint8_t deviceId)
{
    TRACE(2,"%s id %d",__func__, deviceId);
    bt_profile_manager[deviceId].reconnect_mode = bt_profile_reconnect_null;
#ifdef __IAG_BLE_INCLUDE__
    app_start_fast_connectable_ble_adv(BLE_FAST_ADVERTISING_INTERVAL);
#endif
}

#ifdef __BT_ONE_BRING_TWO__
static void app_bt_update_connectable_mode_after_connection_management(void)
{
    uint8_t deviceId;
    bool isEnterConnetableOnlyState = true;
	TRACE(1,"%s",__func__);
    for (deviceId = 0; deviceId < BT_DEVICE_NUM; deviceId++)
    {
        // assure none of the device is in reconnecting mode
        if (bt_profile_manager[deviceId].reconnect_mode != bt_profile_reconnect_null)
        {
            isEnterConnetableOnlyState = false;
            break;
        }
    }
#if 0
    if (isEnterConnetableOnlyState)
    {
        for (deviceId = 0; deviceId < BT_DEVICE_NUM; deviceId++)
        {
            if (!bt_profile_manager[deviceId].has_connected)
            {
                app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                return;
            }
        }
    }
#else //m by pang				
#if defined(__USE_3_5JACK_CTR__)
		if(reconncect_null_by_user){
			bt_profile_manager[0].reconnect_mode = bt_profile_reconnect_null;
			bt_profile_manager[1].reconnect_mode = bt_profile_reconnect_null;
			bt_profile_manager[0].reconnect_cnt = 0;
			bt_profile_manager[1].reconnect_cnt = 0;
			isEnterConnetableOnlyState = true;
			app_bt_accessmode_set(BTIF_BAM_NOT_ACCESSIBLE); 
			app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
		}
#endif

	if (isEnterConnetableOnlyState)
	{	
#if defined(__USE_3_5JACK_CTR__)
		if(app_poweroff_flag||lacal_bt_off||factory_reset_flag||reconncect_null_by_user)
	   		return;
#else
		if(app_poweroff_flag||lacal_bt_off||factory_reset_flag)
	   		return;
#endif
 
		if((bt_profile_manager[0].has_connected)&&(bt_profile_manager[1].has_connected)){
			app_bt_accessmode_set(BTIF_BAM_NOT_ACCESSIBLE);
			app_status_indication_set(APP_STATUS_INDICATION_CONNECTED);
	   	}
		else if((bt_profile_manager[0].has_connected)||(bt_profile_manager[1].has_connected)){
			app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
			app_status_indication_set(APP_STATUS_INDICATION_CONNECTED);
        }
		else{		 	
			#ifndef __EARPHONE_STAY_BOTH_SCAN__
			if(BTIF_BAM_GENERAL_ACCESSIBLE!= app_bt_get_current_access_mode()){	
				app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
				app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
			}
			#else
			if(BTIF_BAM_GENERAL_ACCESSIBLE!= app_bt_get_current_access_mode()){	
				app_bt_accessmode_set(BTIF_BAM_GENERAL_ACCESSIBLE);
				if(APP_STATUS_INDICATION_BOTHSCAN != app_status_indication_get()){
				app_voice_report(APP_STATUS_INDICATION_BOTHSCAN, 0);	
				}
				app_status_indication_set(APP_STATUS_INDICATION_BOTHSCAN);
				//app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
			}
			#endif
		}
		
#ifdef  __IAG_BLE_INCLUDE__
		// start BLE adv
		if(!app_is_arrive_at_max_ble_connections() && app_bt_is_connected()){
			if(!app_ble_is_in_advertising_state())
			app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, true);
		}
		else{
			if(app_ble_is_in_advertising_state())
			app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, false);
		}
#endif		
	}
#endif
}
#endif

static void app_bt_connectable_mode_stop_reconnecting_handler(void)
{
    uint8_t deviceId;
    btif_remote_device_t*remDev;
    btif_cmgr_handler_t * cmgrHandler;
    for (deviceId = 0; deviceId < BT_DEVICE_NUM; deviceId++)
    {
        if (bt_profile_manager[deviceId].reconnect_mode != bt_profile_reconnect_null){
            TRACE(3, "%s id %d mode %d", __func__, deviceId, bt_profile_manager[deviceId].reconnect_mode);
            bt_profile_manager[deviceId].hfp_connect = bt_profile_connect_status_failure;
            bt_profile_manager[deviceId].reconnect_mode = bt_profile_reconnect_null;
            bt_profile_manager[deviceId].saved_reconnect_mode = bt_profile_reconnect_null;
            bt_profile_manager[deviceId].reconnect_cnt = 0;
            if(bt_profile_manager[deviceId].connect_timer !=NULL)
                osTimerStop(bt_profile_manager[deviceId].connect_timer);
            remDev = btif_me_enumerate_remote_devices(deviceId);
            if (remDev != NULL) {
                cmgrHandler = btif_cmgr_get_acl_handler(remDev);
                btif_me_cancel_create_link( btif_cmgr_get_cmgrhandler_remdev_bthandle(cmgrHandler),remDev);
            }
        }
    }
}

void app_bt_connectable_mode_stop_reconnecting(void)
{
    app_bt_start_custom_function_in_bt_thread(0, 0,
        (uint32_t)app_bt_connectable_mode_stop_reconnecting_handler);
}

#if defined (__HSP_ENABLE__)
void app_bt_profile_connect_manager_hs(enum BT_DEVICE_ID_T id, HsChannel *Chan, HsCallbackParms *Info)
{
    btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)Info->p.remDev->bdAddr.address);

    osTimerStop(bt_profile_manager[id].connect_timer);
    bt_profile_manager[id].connect_timer_cb = NULL;
    bool profile_reconnect_enable = false;

    if (Chan&&Info){
        switch(Info->event)
        {
            case HF_EVENT_SERVICE_CONNECTED:
                TRACE(1,"%s HS_EVENT_SERVICE_CONNECTED",__func__);
                nv_record_btdevicerecord_set_hsp_profile_active_state(btdevice_plf_p, true);
#ifndef FPGA
                nv_record_touch_cause_flush();
#endif
                bt_profile_manager[id].hsp_connect = bt_profile_connect_status_success;
                bt_profile_manager[id].reconnect_cnt = 0;
                bt_profile_manager[id].hs_chan = &app_bt_device.hs_channel[id];
                memcpy(bt_profile_manager[id].rmt_addr.address, Info->p.remDev->bdAddr.address, BTIF_BD_ADDR_SIZE);
                if (false == bt_profile_manager[id].has_connected)
                {
                    app_bt_resume_sniff_mode(id);
                }

                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success){
                        TRACE(0,"!!!continue connect a2dp\n");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                        app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_address);
                    }
                }
#ifdef __AUTO_CONNECT_OTHER_PROFILE__
                else{
                    if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success){
                        bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_a2dp_retry_timehandler;
                        app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                        osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                    }
                }
#endif
                break;
            case HF_EVENT_SERVICE_DISCONNECTED:
                TRACE(2,"%s HS_EVENT_SERVICE_DISCONNECTED discReason:%d",__func__, Info->p.remDev->discReason);
                bt_profile_manager[id].hsp_connect = bt_profile_connect_status_failure;
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    if (++bt_profile_manager[id].reconnect_cnt < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT){
                        app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                        profile_reconnect_enable = true;
                        bt_profile_manager[id].hfp_connect = bt_profile_connect_status_unknow;
                    }
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (++bt_profile_manager[id].reconnect_cnt < APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT){
                        app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                        profile_reconnect_enable = true
                    }else{
                        app_bt_restore_reconnecting_idle_mode(id);
                        //bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
                    }
                    TRACE(2,"%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
#if !defined(IBRT)
                }else if(Info->p.remDev->discReason == 0x8){
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnecting;
                    app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                    TRACE(1,"%s try to reconnect",__func__);
                    if(app_bt_profile_reconnect_pending(id) != 0)
                    {
                        profile_reconnect_enable = true;
                    }
#endif
                }else{
                    bt_profile_manager[id].hsp_connect = bt_profile_connect_status_unknow;
                }

                if (profile_reconnect_enable){
                #ifdef __IAG_BLE_INCLUDE__
                    app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
                #endif
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                }
                break;
            default:
                break;
        }
    }

    if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
        bool reconnect_hsp_proc_final = true;
        bool reconnect_a2dp_proc_final = true;
        if (bt_profile_manager[id].hsp_connect == bt_profile_connect_status_failure){
            reconnect_hsp_proc_final = false;
        }
        if (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure){
            reconnect_a2dp_proc_final = false;
        }
        if (reconnect_hsp_proc_final && reconnect_a2dp_proc_final){
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            app_bt_restore_reconnecting_idle_mode(id);
            // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }
    }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
        bool opening_hsp_proc_final = false;
        bool opening_a2dp_proc_final = false;

        if (btdevice_plf_p->hsp_act && bt_profile_manager[id].hsp_connect == bt_profile_connect_status_unknow){
            opening_hsp_proc_final = false;
        }else{
            opening_hsp_proc_final = true;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_unknow){
            opening_a2dp_proc_final = false;
        }else{
            opening_a2dp_proc_final = true;
        }

        if ((opening_hsp_proc_final && opening_a2dp_proc_final) ||
            (bt_profile_manager[id].hsp_connect == bt_profile_connect_status_failure)){
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            app_bt_restore_reconnecting_idle_mode(id);
            // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }

        if (btdevice_plf_p->hsp_act && bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success){
            if (btdevice_plf_p->a2dp_act && !opening_a2dp_proc_final){
                TRACE(0,"!!!continue connect a2dp\n");
                app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
            }
        }

        if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null){
            for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                if (bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    TRACE(0,"!!!hs->start reconnect second device\n");
                    if ((btdevice_plf_p->hfp_act)&&(!bt_profile_manager[i].hfp_connect)){
                        TRACE(0,"try connect hf");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_HF_CreateServiceLink(bt_profile_manager[i].chan, &bt_profile_manager[i].rmt_addr);
                    }
                    else if ((btdevice_plf_p->hsp_act)&&(!bt_profile_manager[i].hsp_connect)){
                        TRACE(0,"try connect hs");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_HS_CreateServiceLink(bt_profile_manager[i].hs_chan, &bt_profile_manager[i].rmt_addr);

                    } else if((btdevice_plf_p->a2dp_act)&&(!bt_profile_manager[i].a2dp_connect)) {
                        TRACE(0,"try connect a2dp");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_A2DP_OpenStream(bt_profile_manager[i].stream, &bt_profile_manager[i].rmt_addr);
                    }
                    break;
                }
            }
        }
    }

#ifdef  __IAG_BLE_INCLUDE__
    if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success &&
        bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success&&
        bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success){
        app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, true);
    }
#endif

    if (!bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success ||
         bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success||
         bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = true;
        nv_record_btdevicerecord_set_last_active(btdevice_plf_p);
        TRACE(0,"BT connected!!!");

#ifndef IBRT
        btif_me_get_remote_device_name(&(ctx->remote_dev_bdaddr), app_bt_global_handle);
#endif
#if defined(MEDIA_PLAYER_SUPPORT)&& !defined(IBRT)
        app_voice_report(APP_STATUS_INDICATION_CONNECTED, id);
#endif

        app_bt_Me_SetLinkPolicy(btif_me_enumerate_remote_devices(id), 
            BTIF_BLP_SNIFF_MODE);

#ifdef __INTERCONNECTION__
        app_interconnection_start_disappear_adv(INTERCONNECTION_BLE_ADVERTISING_INTERVAL,
                                                APP_INTERCONNECTION_DISAPPEAR_ADV_IN_MS);

        if (btif_me_get_activeCons() <= 2)
        {
            app_interconnection_spp_open(btif_me_enumerate_remote_devices(id));
        }
#endif
#ifdef __INTERACTION__
        // app_interaction_spp_open();
#endif
    }

    if (bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = false;
        TRACE(0,"BT disconnected!!!");

    #ifdef GFPS_ENABLED
        if (app_gfps_is_last_response_pending())
        {
            app_gfps_enter_connectable_mode_req_handler(app_gfps_get_last_response());
        }
    #endif

#if defined(MEDIA_PLAYER_SUPPORT)&& !defined(IBRT)
        app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, id);
#endif
#ifdef __INTERCONNECTION__
        app_interconnection_disconnected_callback();
#endif

        app_set_disconnecting_all_bt_connections(false);
   }

#ifdef __BT_ONE_BRING_TWO__
    app_bt_update_connectable_mode_after_connection_management();
#endif
}
#endif
void hfp_reconnecting_timer_stop_callback(const btif_event_t *event)
{
    uint8_t i =0;
    uint8_t id =BT_DEVICE_NUM;
    bt_bdaddr_t *remote = NULL;
    bt_bdaddr_t *hfp_remote = NULL;
    remote = btif_me_get_callback_event_rem_dev_bd_addr(event);

    if(remote != NULL){
        for(i = 0; i<BT_DEVICE_NUM;i++){
            hfp_remote= &bt_profile_manager[i].rmt_addr;
            if(!strcmp((char*)hfp_remote,(char*)remote)){
                id=i;
                TRACE(2,"%s: find bt device num = %d",__func__,id);
                break;
            }
        }
    }
    if(i<BT_DEVICE_NUM){
        TRACE(3,"%s: hfp_connect=%d,reconnect_mode=%d,reconnect_cnt=%d",__func__,bt_profile_manager[id].hfp_connect,bt_profile_manager[id].reconnect_mode,bt_profile_manager[id].reconnect_cnt);
        if((bt_profile_manager[id].reconnect_mode != bt_profile_reconnect_null)&& bt_profile_manager[id].reconnect_cnt != 0){
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
            bt_profile_manager[id].saved_reconnect_mode = bt_profile_reconnect_null;
            bt_profile_manager[id].reconnect_cnt = 0;
            if(bt_profile_manager[id].connect_timer !=NULL)
                osTimerStop(bt_profile_manager[id].connect_timer);

            TRACE(1,"%s: stop success",__func__);
        }
    }
    else{
        TRACE(1,"%s: not find bt device",__func__);
    }
}

void app_audio_switch_flash_flush_req(void);

//void app_bt_profile_connect_manager_hf(enum BT_DEVICE_ID_T id, HfChannel *Chan, HfCallbackParms *Info)
void app_bt_profile_connect_manager_hf(enum BT_DEVICE_ID_T id, hf_chan_handle_t Chan, struct hfp_context *ctx)
{
    //btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)Info->p.remDev->bdAddr.address);
    btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)ctx->remote_dev_bdaddr.address);
    bool profile_reconnect_enable = false;

    osTimerStop(bt_profile_manager[id].connect_timer);
    bt_profile_manager[id].connect_timer_cb = NULL;
    //if (Chan&&Info){
    if (Chan){
        switch(ctx->event)
        {
            case BTIF_HF_EVENT_SERVICE_CONNECTED:
                TRACE(1,"%s HF_EVENT_SERVICE_CONNECTED",__func__);
                nv_record_btdevicerecord_set_hfp_profile_active_state(btdevice_plf_p, true);
#ifndef FPGA
                nv_record_touch_cause_flush();
#endif
				lostconncection_to_pairing=0;//add by pang
                bt_profile_manager[id].hfp_connect = bt_profile_connect_status_success;
                bt_profile_manager[id].saved_reconnect_mode =bt_profile_reconnect_null;
                bt_profile_manager[id].reconnect_cnt = 0;
                bt_profile_manager[id].chan = app_bt_device.hf_channel[id];
                memcpy(bt_profile_manager[id].rmt_addr.address, ctx->remote_dev_bdaddr.address, BTIF_BD_ADDR_SIZE);
                if (false == bt_profile_manager[id].has_connected)
                {
                    app_bt_resume_sniff_mode(id);
                }

            #ifdef BTIF_DIP_DEVICE
                btif_dip_get_remote_info(app_bt_get_remoteDev(id), id);
            #endif

#ifdef BT_PBAP_SUPPORT
                app_bt_reconnect_pbap_profile(&ctx->remote_dev_bdaddr);
#endif

                TRACE(3,"%s id %d reconnect_mode %d",__func__, id, bt_profile_manager[id].reconnect_mode);
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }
#if defined(IBRT)
                else if (app_bt_ibrt_reconnect_mobile_profile_flag_get()){
                    app_bt_ibrt_reconnect_mobile_profile_flag_clear();
#else
                else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
#endif
			/** add by pang **/
				reconnect_timeout_stop();
				reconnect_timeout_set(1);
			/** end add **/

                    TRACE(2,"app_bt: a2dp_act in NV =%d,a2dp_connect=%d", btdevice_plf_p->a2dp_act, bt_profile_manager[id].a2dp_connect);
                    if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success){
                        TRACE(0,"!!!continue connect a2dp\n");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                        app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
                    }
                }
#ifdef __AUTO_CONNECT_OTHER_PROFILE__
                else{
                    TRACE(2,"app_bt: a2dp_act in NV =%d,a2dp_connect=%d", btdevice_plf_p->a2dp_act, bt_profile_manager[id].a2dp_connect);
                    //befor auto connect a2dp profile, check whether a2dp is supported
                    if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success) {
                        bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_a2dp_retry_timehandler;
                        app_bt_accessmode_set(BAM_CONNECTABLE_ONLY);
                        osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                    }
                }
#endif
                app_bt_switch_role_if_needed(app_bt_get_remoteDev(id));
                break;
            case BTIF_HF_EVENT_SERVICE_DISCONNECTED:
                //TRACE(3,"%s HF_EVENT_SERVICE_DISCONNECTED discReason:%d/%d",__func__, Info->p.remDev->discReason, Info->p.remDev->discReason_saved);
                TRACE(3,"%s HF_EVENT_SERVICE_DISCONNECTED id:%d discReason:0x%x/0x%x %d/%d",__func__,id, ctx->disc_reason, ctx->disc_reason_saved, bt_profile_manager[id].reconnect_mode,bt_profile_manager[id].reconnect_cnt);//m by pang
                bt_profile_manager[id].hfp_connect = bt_profile_connect_status_failure;
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    if (bt_profile_manager[id].reconnect_cnt++ < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT){
                        app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                        profile_reconnect_enable = true;
                        bt_profile_manager[id].hfp_connect = bt_profile_connect_status_unknow;
                    }
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (bt_profile_manager[id].reconnect_cnt++ < APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT){
                        app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                        profile_reconnect_enable = true;
					 #ifdef MEDIA_PLAYER_SUPPORT	   
					   //if(!(bt_profile_manager[id].reconnect_cnt % 2))			
					   		//app_voice_report(APP_STATUS_INDICATION_LOST_OF_RANGE, id);//add by pang								
				     #endif 
						/** add by pang **/
				   		reconnect_timeout_stop();
				   		reconnect_timeout_set(1);
					   /** end add **/
                    }else{
                        reconnect_timeout_stop();//add by pang
                        app_bt_restore_reconnecting_idle_mode(id);
                        // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
                    }
                    TRACE(2,"%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
                    /*
                }else if ((Info->p.remDev->discReason == 0x8)||
                          (Info->p.remDev->discReason_saved == 0x8)){
                          */
                }
#if !defined(IBRT)
#if defined(ENHANCED_STACK)
                else if (((ctx->disc_reason == 0x8)||
                          (ctx->disc_reason_saved == 0x8) ||
                          (ctx->disc_reason == 0x4)||
                          (ctx->disc_reason_saved == 0x4)||
                          (ctx->disc_reason == 0x22)||
                          (ctx->disc_reason_saved == 0x22))&&(factory_reset_flag==0))//m by pang        &&(factory_reset_flag==0)
#else
                    else if ((ctx->disc_reason == 0x8)||
                          (ctx->disc_reason_saved == 0x8) ||
                          (ctx->disc_reason == 0x0)||
                          (ctx->disc_reason_saved == 0x0)||
                          (ctx->disc_reason == 0x22)||
                          (ctx->disc_reason_saved == 0x22))
#endif
                 {
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnecting;
                    app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                    TRACE(2,"%s try to reconnect reason =%d",__func__,ctx->disc_reason);
                    if(app_bt_profile_reconnect_pending(id) != 0)
                    {
                        profile_reconnect_enable = true;
                    }
					/** add by pang **/
					if(false==app_bt_is_connected()){
						app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);	
					}
				   	reconnect_timeout_stop();
				   	reconnect_timeout_set(1);
					app_start_10_second_timer(APP_POWEROFF_TIMER_ID);//add by cai
					/** end add **/
                }
#endif
                else{
                    bt_profile_manager[id].hfp_connect = bt_profile_connect_status_unknow;
                }

                if (profile_reconnect_enable){
                #ifdef  __IAG_BLE_INCLUDE__
                    app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
                #endif
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                }
                break;
            default:
                break;
        }
    }
    TRACE(2,"%s reconnect_mode %d",__func__, bt_profile_manager[id].reconnect_mode);
    DUMP8("%02x ", &bt_profile_manager[id].rmt_addr.address, 6);
    btdevice_profile *btdevice_plf_p1 = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)&bt_profile_manager[id].rmt_addr.address);

    if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
        bool reconnect_hfp_proc_final = false;
        bool reconnect_a2dp_proc_final = false;

        TRACE(2,"hfp_connect %d a2dp_connect %d", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].a2dp_connect);
        if (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success){
            reconnect_hfp_proc_final = false;
        }else{
            reconnect_hfp_proc_final = true;
        }
        if (bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success){
            TRACE(2,"hfp_act %d a2dp_act %d", btdevice_plf_p1->hfp_act, btdevice_plf_p1->a2dp_act);
            if(btdevice_plf_p1->hfp_act && btdevice_plf_p1->a2dp_act){
                reconnect_a2dp_proc_final = false;
            }
            else{
                reconnect_a2dp_proc_final = true;
            }
        }
        else
        {
            reconnect_a2dp_proc_final = true;
        }
        if (reconnect_hfp_proc_final && reconnect_a2dp_proc_final){
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            app_bt_restore_reconnecting_idle_mode(id);
            // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }
    }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
        bool opening_hfp_proc_final = false;
        bool opening_a2dp_proc_final = false;

        TRACE(2,"hfp_connect %d a2dp_connect %d", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].a2dp_connect);
        TRACE(2,"hfp_act %d a2dp_act %d", btdevice_plf_p1->hfp_act, btdevice_plf_p1->a2dp_act);
        if (btdevice_plf_p1->hfp_act && bt_profile_manager[id].hfp_connect == bt_profile_connect_status_unknow){
            opening_hfp_proc_final = false;
        }else{
            opening_hfp_proc_final = true;
        }

        if (btdevice_plf_p1->a2dp_act && bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_unknow){
            opening_a2dp_proc_final = false;
        }else{
            opening_a2dp_proc_final = true;
        }

        if(opening_hfp_proc_final && opening_a2dp_proc_final)
        {
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].saved_reconnect_mode =  bt_profile_reconnect_openreconnecting;
			app_bt_restore_reconnecting_idle_mode(id); 
        }
        else if(bt_profile_manager[id].hfp_connect == bt_profile_connect_status_failure)	
        {
            TRACE(3,"reconnect_mode888:%d",bt_profile_manager[id].reconnect_mode);
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].saved_reconnect_mode =  bt_profile_reconnect_openreconnecting;
			if ((bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting)
                    &&(bt_profile_manager[id].reconnect_cnt >= APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT))
            {                  
                app_bt_restore_reconnecting_idle_mode(id);
            }
        }

        if (btdevice_plf_p1->hfp_act && bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success){
            if (btdevice_plf_p1->a2dp_act && !opening_a2dp_proc_final){
                TRACE(0,"!!!continue connect a2dp\n");
                app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
            }
        }

        if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null){
            for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                btdevice_profile *btdevice_plf_p_temp = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)bt_profile_manager[i].rmt_addr.address);
				TRACE(2,"id %d reconnect_mode %d", i, bt_profile_manager[i].reconnect_mode);
                if (bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    TRACE(0,"!!!hf->start reconnect second device\n");
                    if ((btdevice_plf_p_temp->hfp_act)&&(!bt_profile_manager[i].hfp_connect)){
                        TRACE(0,"try connect hf");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_HF_CreateServiceLink(bt_profile_manager[i].chan, (bt_bdaddr_t *)&bt_profile_manager[i].rmt_addr);
                    }
#if defined (__HSP_ENABLE__)
                    else if((btdevice_plf_p_temp->hsp_act)&&(!bt_profile_manager[i].hsp_connect)) {
                        TRACE(0,"try connect hs");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_HS_CreateServiceLink(bt_profile_manager[i].hs_chan, &bt_profile_manager[i].rmt_addr);
                    }
#endif
                    else if((btdevice_plf_p_temp->a2dp_act)&&(!bt_profile_manager[i].a2dp_connect)) {
                        TRACE(0,"try connect a2dp");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_A2DP_OpenStream(bt_profile_manager[i].stream, &bt_profile_manager[i].rmt_addr);
                    }
                    break;
                }
            }
        }
    }
#ifdef __INTERCONNECTION__
    if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success &&
        bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)
    {
        app_interconnection_start_disappear_adv(INTERCONNECTION_BLE_ADVERTISING_INTERVAL,
                                                APP_INTERCONNECTION_DISAPPEAR_ADV_IN_MS);

        if (btif_me_get_activeCons() <= 2)
        {
            app_interconnection_spp_open(btif_me_enumerate_remote_devices(id));
        }
    }
#endif

#ifdef  __IAG_BLE_INCLUDE__
    TRACE(3, "%s hfp %d a2dp %d", __func__, bt_profile_manager[id].hfp_connect, bt_profile_manager[id].a2dp_connect);
    if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success &&
#ifdef __HSP_ENABLE__
        bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success&&
#endif
        bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success){
        app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, true);
    }
#endif

    if (!bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success ||
#ifdef __HSP_ENABLE__
         bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success||
#endif
         bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = true;

        nv_record_btdevicerecord_set_last_active(btdevice_plf_p);
		
		TRACE(1,"BT connected!!!: %d",id);//m by cai
		app_cur_connect_devid_set(id, true);//add by cai
		
		if((bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null))//回连的时候不保存配对记录 
		{																		//按键切换回连成功保存配对记录 ZCL @ 2020/07/24		
			TRACE(0,"not reconnect record!!!");
			nv_record_flash_flush();
		}

#ifndef IBRT
        btif_me_get_remote_device_name(&(ctx->remote_dev_bdaddr), app_bt_global_handle);
#endif
#if defined(MEDIA_PLAYER_SUPPORT)&& !defined(IBRT)
        app_voice_report(APP_STATUS_INDICATION_CONNECTED, id);
#endif

/** add by pang **/
		lacal_bt_off=0;
		factory_reset_flag=0;
		app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
		app_start_10_second_timer(APP_AUTO_POWEROFF_TIMER_ID);
		//app_status_indication_set(APP_STATUS_INDICATION_CONNECTED);
		app_status_indication_set_next(APP_STATUS_INDICATION_CONNECTING,APP_STATUS_INDICATION_CONNECTED);

		//if ((bt_profile_manager[0].reconnect_mode == bt_profile_reconnect_null)&&(bt_profile_manager[1].reconnect_mode == bt_profile_reconnect_null))
		{
			if(tx_pwr_for_page_flag){
				bt_drv_tx_pwr_for_init();
				tx_pwr_for_page_flag=0;
			}
		}
/** end add **/

        app_bt_Me_SetLinkPolicy(btif_me_enumerate_remote_devices(id), 
            BTIF_BLP_SNIFF_MODE);
    

#if 0 // #ifdef __INTERCONNECTION__
        app_interconnection_start_disappear_adv(BLE_ADVERTISING_INTERVAL, APP_INTERCONNECTION_DISAPPEAR_ADV_IN_MS);
        app_interconnection_spp_open();
#endif

#ifdef __INTERACTION__
    //    app_interaction_spp_open();
#endif
    }

    if (bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success &&
#ifdef __HSP_ENABLE__
         bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success &&
#endif
         bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = false;
        TRACE(1,"BT disconnected!!!: %d",id);//m by cai
		app_cur_connect_devid_set(id, false);//add by cai

#ifdef GFPS_ENABLED
        if (app_gfps_is_last_response_pending())
        {
            app_gfps_enter_connectable_mode_req_handler(app_gfps_get_last_response());
        }
#endif

#if defined(MEDIA_PLAYER_SUPPORT)&& !defined(IBRT)
		if((factory_reset_flag==0) && (app_poweroff_flag==0))
			app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, id);
#endif
/** add by pang **/
		if((false==bt_profile_manager[0].has_connected)&&(false==bt_profile_manager[1].has_connected)&&
			(false==lacal_bt_off)&&(false==factory_reset_flag)&&(app_poweroff_flag==0)){
			app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
			app_start_10_second_timer(APP_POWEROFF_TIMER_ID);//m by cai
			app_stop_10_second_timer(APP_AUTO_POWEROFF_TIMER_ID);
		}

		//if ((bt_profile_manager[0].reconnect_mode != bt_profile_reconnect_null)||(bt_profile_manager[1].reconnect_mode != bt_profile_reconnect_null))
		{
			if(tx_pwr_for_page_flag==0){
				bt_drv_tx_pwr_for_page();
				tx_pwr_for_page_flag=1;
			}
		}
/** end add **/

#ifdef __INTERCONNECTION__
        app_interconnection_disconnected_callback();
#endif

        app_set_disconnecting_all_bt_connections(false);
   }

#ifdef __BT_ONE_BRING_TWO__
    app_bt_update_connectable_mode_after_connection_management();
#endif
}

#if !defined(IBRT)
static void app_bt_handling_on_abnormal_disconnection(enum BT_DEVICE_ID_T id, bt_bdaddr_t* remDevAddr, uint8_t disc_error)
{
    hf_chan_handle_t hfChannel = bt_profile_manager[id].chan;
    struct hfp_context hfContext;
    hfContext.event = BTIF_HF_EVENT_SERVICE_DISCONNECTED;
    hfContext.remote_dev_bdaddr = *remDevAddr;
    hfContext.disc_reason = disc_error;
    hfContext.disc_reason_saved = disc_error;

    app_bt_profile_connect_manager_hf(id, hfChannel, &hfContext);
}
#endif

void app_bt_profile_connect_manager_a2dp(enum BT_DEVICE_ID_T id, a2dp_stream_t *Stream, const   a2dp_callback_parms_t *info)
{
    btdevice_profile *btdevice_plf_p = NULL;
    btif_remote_device_t *remDev = NULL;
    btif_a2dp_callback_parms_t* Info = (btif_a2dp_callback_parms_t*)info;
    osTimerStop(bt_profile_manager[id].connect_timer);
    bt_profile_manager[id].connect_timer_cb = NULL;
    bool profile_reconnect_enable = false;

#if 0	
    remDev = btif_a2dp_get_stream_conn_remDev(Stream);	
    if (remDev){
        btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(btif_me_get_remote_device_bdaddr(remDev)->address);
    }else{
        btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(NULL);
    }
#else //modify by pang
	remDev = btif_a2dp_get_stream_conn_remDev(Stream);	
	if (remDev){
		btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(btif_me_get_remote_device_bdaddr(remDev)->address);
	}else{
		btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager[id].rmt_addr.address);
	}    
#endif 
 
#ifdef __BT_ONE_BRING_TWO__
    if(remDev){
        if((bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting) && 
            memcmp(bt_profile_manager[id].rmt_addr.address, btif_me_get_remote_device_bdaddr(remDev)->address, 6) != 0){
            uint8_t other_device_id = (id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
            TRACE(1, "continue reconnect device[%d]:", other_device_id);
            DUMP8("%02x ", bt_profile_manager[id].rmt_addr.address, 6);
            memcpy(bt_profile_manager[other_device_id].rmt_addr.address, bt_profile_manager[id].rmt_addr.address, 6);
            bt_profile_manager[other_device_id].reconnect_cnt = bt_profile_manager[id].reconnect_cnt;
            app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
            osTimerStart(bt_profile_manager[other_device_id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
        }
    }
#endif

    if (Stream&&Info){

        switch(Info->event)
        {
            case BTIF_A2DP_EVENT_STREAM_OPEN:
                TRACE(4,"%s A2DP_EVENT_STREAM_OPEN,codec type=%x a2dp:%d mode:%d",
                                                                       __func__, Info->p.configReq->codec.codecType,
                                                                       bt_profile_manager[id].a2dp_connect,
                                                                       bt_profile_manager[id].reconnect_mode);

                nv_record_btdevicerecord_set_a2dp_profile_active_state(btdevice_plf_p, true);
                nv_record_btdevicerecord_set_a2dp_profile_codec(btdevice_plf_p, Info->p.configReq->codec.codecType);
#ifndef FPGA
                nv_record_touch_cause_flush();
#endif
                if(bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)
                {
                    TRACE(0,"!!!a2dp has opened   force return ");
                    return;
                }
				
				/** add by pang **/ 
				lostconncection_to_pairing=0;
				/** end add **/
			
                bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_success;
                bt_profile_manager[id].reconnect_cnt = 0;
                bt_profile_manager[id].stream = app_bt_device.a2dp_connected_stream[id];
                memcpy(bt_profile_manager[id].rmt_addr.address,  btif_me_get_remote_device_bdaddr(btif_a2dp_get_stream_conn_remDev(Stream))->address, BTIF_BD_ADDR_SIZE);
                app_bt_record_latest_connected_service_device_id(id);
                if (false == bt_profile_manager[id].has_connected)
                {
                    app_bt_resume_sniff_mode(id);
                }

            #ifdef BTIF_DIP_DEVICE
                btif_dip_get_remote_info(remDev, id);
            #endif

            TRACE(3,"%s id %d reconnect_mode %d",__func__, id, bt_profile_manager[id].reconnect_mode);
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }

#if defined(IBRT)
                else if (app_bt_ibrt_reconnect_mobile_profile_flag_get()){
                    app_bt_ibrt_reconnect_mobile_profile_flag_clear();
#else
                else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
#endif
				/** add by pang **/
					reconnect_timeout_stop();
					reconnect_timeout_set(1);
				/** end add **/

                    TRACE(2,"hfp_act %d,a2dp_connect %d", btdevice_plf_p->hfp_act, bt_profile_manager[id].hfp_connect);
                    if (btdevice_plf_p->hfp_act&& bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success){
                        TRACE(0,"!!!continue connect hfp\n");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                        app_bt_HF_CreateServiceLink(bt_profile_manager[id].chan, (bt_bdaddr_t *)&bt_profile_manager[id].rmt_addr);
                    }
#if defined (__HSP_ENABLE__)
                    else if(btdevice_plf_p->hsp_act&& bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success){
                        TRACE(0,"!!!continue connect hsp\n");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                        app_bt_HS_CreateServiceLink(bt_profile_manager[id].hs_chan, &bt_profile_manager[id].rmt_addr);
                    }
#endif
                }
#ifdef __AUTO_CONNECT_OTHER_PROFILE__
                else{
                    TRACE(2,"hfp_act %d,a2dp_connect %d", btdevice_plf_p->hfp_act, bt_profile_manager[id].hfp_connect);
                    if(btdevice_plf_p->hfp_act && bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success)
                    {
                        bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_hf_retry_timehandler;
                        app_bt_accessmode_set(BAM_CONNECTABLE_ONLY);
                        osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                    }
#if defined (__HSP_ENABLE__)
                    else if(btdevice_plf_p->hsp_act && bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success)
                    {
                        bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_hs_retry_timehandler;
                        app_bt_accessmode_set(BAM_CONNECTABLE_ONLY);
                        osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                    }
#endif
                }
#endif
#ifdef APP_DISABLE_PAGE_SCAN_AFTER_CONN
                disable_page_scan_check_timer_start();
#endif
                app_bt_switch_role_if_needed(remDev);
                break;
			case BTIF_A2DP_EVENT_STREAM_STARTED:
            case BTIF_A2DP_EVENT_STREAM_STARTED_MOCK:
				nv_record_btdevicerecord_set_last_active(btdevice_plf_p);
				return;
            case BTIF_A2DP_EVENT_STREAM_CLOSED:

                TRACE(2,"%s A2DP_EVENT_STREAM_CLOSED discReason1:%d",__func__, Info->discReason);

				if(Info->subevt != A2DP_CONN_CLOSED)
				{
					TRACE(0,"do not need set access mode");
					return ;
				}

                if(Stream!=NULL)
                {
                    if(btif_a2dp_get_remote_device(Stream)!=NULL)
                        TRACE(2,"%s A2DP_EVENT_STREAM_CLOSED discReason2:%d",__func__,btif_me_get_remote_device_disc_reason_saved(btif_a2dp_get_remote_device(Stream)));
                }

                bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_failure;

                TRACE(3,"id %d reconnect_mode %d recon_cnt %d", id, bt_profile_manager[id].reconnect_mode, bt_profile_manager[id].reconnect_cnt);
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                   if (bt_profile_manager[id].reconnect_cnt++ < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT){
                       app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                       profile_reconnect_enable = true;
                       bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_unknow;
                   }
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                   if (bt_profile_manager[id].reconnect_cnt++ < APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT){
                       app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                       profile_reconnect_enable = true;
				   #ifdef MEDIA_PLAYER_SUPPORT	   
					   //if(!(bt_profile_manager[id].reconnect_cnt % 2))			
					   		//app_voice_report(APP_STATUS_INDICATION_LOST_OF_RANGE, id);//add by pang								
				   #endif 
				   	   /** add by pang **/
				   	   reconnect_timeout_stop();
				   	   reconnect_timeout_set(1);
					   /** end add **/
                   }else{
                   		reconnect_timeout_stop();//add by pang
                       app_bt_restore_reconnecting_idle_mode(id);
                       // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
                   }
                   TRACE(2,"%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
                }
#if !defined(IBRT)
                #if defined(ENHANCED_STACK)
                   else if(((Info->discReason == 0x08)||
                           (Info->discReason == 0x04)||
                           (Info->discReason == 0x22)) &&
                #else
                    else if(((Info->discReason == 0x8)||
                        (Info->discReason_saved == 0x8)||
                        (Info->discReason_saved == 0x0)||
                        (Info->discReason_saved == 0x22)) &&
                #endif
                          (btdevice_plf_p->a2dp_act)&&
                          (!btdevice_plf_p->hfp_act) &&
                          (!btdevice_plf_p->hsp_act)&&(factory_reset_flag==0)){ //m by pang        &&(factory_reset_flag==0)
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnecting;
                    TRACE(2,"%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
                    app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
                    if(app_bt_profile_reconnect_pending(id) != 0)
                    {
                        profile_reconnect_enable = true;
                    }
					/** add by pang **/
					if(false==app_bt_is_connected()){
						app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);	
					}
				   	reconnect_timeout_stop();
				   	reconnect_timeout_set(1);
				   	app_start_10_second_timer(APP_POWEROFF_TIMER_ID);//add by cai
					/** end add **/
                }	   
#endif
                else{
                    bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_unknow;
               }

               if (profile_reconnect_enable){
               #ifdef __IAG_BLE_INCLUDE__
                   app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
               #endif
                   osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
               }
               break;
            default:
                break;
        }
    }

    if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
        bool reconnect_hfp_proc_final = true;
        bool reconnect_a2dp_proc_final = true;
        TRACE(2,"hfp_connect %d a2dp_connect %d", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].a2dp_connect);
        if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_failure){
            reconnect_hfp_proc_final = false;
        }
#if defined (__HSP_ENABLE__)
        if(btdevice_plf_p->hsp_act !=0){    //has HSP
            reconnect_hfp_proc_final = true;
            if (bt_profile_manager[id].hsp_connect == bt_profile_connect_status_failure){
                reconnect_hfp_proc_final = false;
            }
        }
#endif
        if (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure){
            reconnect_a2dp_proc_final = false;
        }
        if (reconnect_hfp_proc_final && reconnect_a2dp_proc_final){
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            app_bt_restore_reconnecting_idle_mode(id);
            // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }
    }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
        bool opening_hfp_proc_final = false;
        bool opening_a2dp_proc_final = false;

        TRACE(2,"hfp_connect %d a2dp_connect %d", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].a2dp_connect);
        TRACE(2,"hfp_act %d a2dp_act %d", btdevice_plf_p->hfp_act, btdevice_plf_p->a2dp_act);
        if (btdevice_plf_p->hfp_act && bt_profile_manager[id].hfp_connect == bt_profile_connect_status_unknow){
            opening_hfp_proc_final = false;
        }else{
            opening_hfp_proc_final = true;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_unknow){
            opening_a2dp_proc_final = false;
        }else{
            opening_a2dp_proc_final = true;
        }

        if ((opening_hfp_proc_final && opening_a2dp_proc_final) ||
            (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure)){
            TRACE(3,"!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            app_bt_restore_reconnecting_idle_mode(id);
            // bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect== bt_profile_connect_status_success){
            if (btdevice_plf_p->hfp_act && !opening_hfp_proc_final){
                TRACE(0,"!!!continue connect hf\n");
                app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                app_bt_HF_CreateServiceLink(bt_profile_manager[id].chan, (bt_bdaddr_t *)&bt_profile_manager[id].rmt_addr);
            }
#if defined (__HSP_ENABLE)
            else if(btdevice_plf_p->hsp_act && !opening_hfp_hsp_proc_final){
                TRACE(0,"!!!continue connect hs\n");
                app_bt_precheck_before_starting_connecting(bt_profile_manager[id].has_connected);
                app_bt_HS_CreateServiceLink(bt_profile_manager[id].hs_chan, &bt_profile_manager[id].rmt_addr);
            }
#endif
        }

        if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null){
            for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                btdevice_profile *btdevice_plf_p_temp = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)bt_profile_manager[i].rmt_addr.address);
                TRACE(2,"id %d reconnect_mode %d", i, bt_profile_manager[i].reconnect_mode);
                if (bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    TRACE(1,"!!!a2dp->start reconnect device %d\n", i);
                    if((btdevice_plf_p_temp->a2dp_act)&&(!bt_profile_manager[i].a2dp_connect)) {
                        TRACE(0,"try connect a2dp");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_A2DP_OpenStream(bt_profile_manager[i].stream, &bt_profile_manager[i].rmt_addr);
                    }
#if defined (__HSP_ENABLE__)
                    else if((btdevice_plf_p_temp->hsp_act)&&(!bt_profile_manager[i].hsp_connect)) {
                        TRACE(0,"try connect hs");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_HS_CreateServiceLink(bt_profile_manager[i].hs_chan, &bt_profile_manager[i].rmt_addr);
                    }
#endif
                    else if ((btdevice_plf_p_temp->hfp_act)&&(!bt_profile_manager[i].hfp_connect)){
                        TRACE(0,"try connect hf");
                        app_bt_precheck_before_starting_connecting(bt_profile_manager[i].has_connected);
                        app_bt_HF_CreateServiceLink(bt_profile_manager[i].chan, (bt_bdaddr_t *)&bt_profile_manager[i].rmt_addr);
                    }
                    break;
                }
            }
        }
    }

#ifdef __INTERCONNECTION__
    if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success &&
        bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)
    {
        app_interconnection_start_disappear_adv(INTERCONNECTION_BLE_ADVERTISING_INTERVAL,
                                                APP_INTERCONNECTION_DISAPPEAR_ADV_IN_MS);

        if (btif_me_get_activeCons() <= 2)
        {
            app_interconnection_spp_open(remDev);
        }
    }
#endif

#ifdef  __IAG_BLE_INCLUDE__
    TRACE(3, "%s hfp %d a2dp %d", __func__, bt_profile_manager[id].hfp_connect, bt_profile_manager[id].a2dp_connect);
    if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success &&
#ifdef __HSP_ENABLE__
        bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success &&
#endif
        bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success){
        app_ble_force_switch_adv(BLE_SWITCH_USER_BT_CONNECT, true);
    }
#endif

    if (!bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success ||
#ifdef __HSP_ENABLE__
        bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success||
#endif
        bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = true;

        nv_record_btdevicerecord_set_last_active(btdevice_plf_p);
        TRACE(1,"BT connected!!!: %d",id);//m by cai
		app_cur_connect_devid_set(id, true);//add by cai
		
		if((bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null))//回连的时候不保存配对记录 
		{																	     //按键切换回连成功保存配对记录 ZCL @ 2020/07/24		
			TRACE(0,"not reconnect record!!!");
			nv_record_flash_flush();
		}

#ifndef IBRT
        btif_me_get_remote_device_name(&(bt_profile_manager[id].rmt_addr), app_bt_global_handle);
#endif
#if defined(MEDIA_PLAYER_SUPPORT)&& !defined(IBRT)
        app_voice_report(APP_STATUS_INDICATION_CONNECTED, id);
#endif

/** add by pang **/
		lacal_bt_off=0;
		factory_reset_flag=0;
		app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
		app_start_10_second_timer(APP_AUTO_POWEROFF_TIMER_ID);
		//app_status_indication_set(APP_STATUS_INDICATION_CONNECTED);
		app_status_indication_set_next(APP_STATUS_INDICATION_CONNECTING,APP_STATUS_INDICATION_CONNECTED);

		//if ((bt_profile_manager[0].reconnect_mode == bt_profile_reconnect_null)&&(bt_profile_manager[1].reconnect_mode == bt_profile_reconnect_null))
		{
			if(tx_pwr_for_page_flag){
				bt_drv_tx_pwr_for_init();
				tx_pwr_for_page_flag=0;
			}
		}
/** end add **/

        app_bt_Me_SetLinkPolicy(btif_me_enumerate_remote_devices(id), 
            BTIF_BLP_SNIFF_MODE);

#if 0 // #ifdef __INTERCONNECTION__
        app_interconnection_start_disappear_adv(BLE_ADVERTISING_INTERVAL, APP_INTERCONNECTION_DISAPPEAR_ADV_IN_MS);
        app_interconnection_spp_open();
#endif

#ifdef __INTERACTION__
    //    app_interaction_spp_open();
#endif
    }

    if (bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success &&
#ifdef __HSP_ENABLE__
         bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success &&
#endif
         bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = false;
		TRACE(1,"BT disconnected!!!: %d",id);
		app_cur_connect_devid_set(id, false);//add by cai

#ifdef GFPS_ENABLED
        if (app_gfps_is_last_response_pending())
        {
            app_gfps_enter_connectable_mode_req_handler(app_gfps_get_last_response());
        }
#endif

#if defined(MEDIA_PLAYER_SUPPORT)&& !defined(IBRT)
		if((factory_reset_flag==0) && (app_poweroff_flag==0))
			app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, id);
#endif
/** add by pang **/
		if((false==bt_profile_manager[0].has_connected)&&(false==bt_profile_manager[1].has_connected)&&
			(false==lacal_bt_off)&&(false==factory_reset_flag)&&(app_poweroff_flag==0)){
			app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
			app_start_10_second_timer(APP_POWEROFF_TIMER_ID);//m by cai
			app_stop_10_second_timer(APP_AUTO_POWEROFF_TIMER_ID);
        }

		//if ((bt_profile_manager[0].reconnect_mode != bt_profile_reconnect_null)||(bt_profile_manager[1].reconnect_mode != bt_profile_reconnect_null))
		{
			if(tx_pwr_for_page_flag==0){
				bt_drv_tx_pwr_for_page();
				tx_pwr_for_page_flag=1;
			}
		}
/** end add **/

#ifdef __INTERCONNECTION__
        app_interconnection_disconnected_callback();
#endif

        app_set_disconnecting_all_bt_connections(false);
    }

#ifdef __BT_ONE_BRING_TWO__
    app_bt_update_connectable_mode_after_connection_management();
#endif
}


static bool isDisconnectAllBtConnections = false;

bool app_is_disconnecting_all_bt_connections(void)
{
    return isDisconnectAllBtConnections;
}

void app_set_disconnecting_all_bt_connections(bool isEnable)
{
    isDisconnectAllBtConnections = isEnable;
}

bt_status_t LinkDisconnectDirectly(bool PowerOffFlag)
{
    app_set_disconnecting_all_bt_connections(true);
    //TRACE(1,"osapi_lock_is_exist:%d",osapi_lock_is_exist());
    if(osapi_lock_is_exist())
        osapi_lock_stack();
#ifdef __IAG_BLE_INCLUDE__
    TRACE(1,"ble_connected_state:%d", app_ble_is_any_connection_exist());
#endif

#if defined(IBRT)
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if(true==PowerOffFlag)
        p_ibrt_ctrl->ibrt_in_poweroff= true;

    if (p_ibrt_ctrl->init_done) {
        if(IBRT_MASTER==p_ibrt_ctrl->current_role)
        {
            if(app_tws_ibrt_mobile_link_connected())
            {
                //should check return status
                app_tws_ibrt_disconnect_connection(btif_me_get_remote_device_by_handle(p_ibrt_ctrl->mobile_conhandle));
            }
        }
        if (app_tws_ibrt_tws_link_connected())
        {
            app_tws_ibrt_disconnect_connection(btif_me_get_remote_device_by_handle(p_ibrt_ctrl->tws_conhandle));
        }
    }
#else
    app_bt_disconnect_all_mobile_link();
#endif

    if(osapi_lock_is_exist())
        osapi_unlock_stack();
    osDelay(500);

#ifdef __IAG_BLE_INCLUDE__
    if(app_ble_is_any_connection_exist())
    {
#ifdef GFPS_ENABLED
        if (!app_gfps_is_last_response_pending())
#endif
        {
            app_ble_disconnect_all();
        }
        if(osapi_lock_is_exist())
            osapi_unlock_stack();
        osDelay(500);
    }
#endif

    return BT_STS_SUCCESS;

#if 0
    TRACE(1,"activeCons:%d", btif_me_get_activeCons());

    uint8_t Tmp_activeCons = btif_me_get_activeCons();

    if(Tmp_activeCons)
    {
        //TRACE(3,"%s id1 hf:%d a2dp:%d",__func__, app_bt_device.hf_channel[BT_DEVICE_ID_1].state, btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream));
        TRACE(3,"%s id1 hf:%d a2dp:%d",__func__,  btif_get_hf_chan_state(app_bt_device.hf_channel[BT_DEVICE_ID_1]), btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream));
#ifdef BTIF_HID_DEVICE
            app_bt_hid_exit_shutter_mode();
#endif
        if (btif_get_hf_chan_state(app_bt_device.hf_channel[BT_DEVICE_ID_1]) == BTIF_HF_STATE_OPEN){
            app_bt_HF_DisconnectServiceLink(app_bt_device.hf_channel[BT_DEVICE_ID_1]);
        }
#if defined (__HSP_ENABLE__)
        if(app_bt_device.hs_channel[BT_DEVICE_ID_1].state == HS_STATE_OPEN){
            app_bt_HS_DisconnectServiceLink(&app_bt_device.hs_channel[BT_DEVICE_ID_1]);
        }
#endif   //btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[device_id]->a2dp_stream)
        if(btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream);
        }
#if defined(A2DP_LHDC_ON)
        if(btif_a2dp_get_stream_state(app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream);
        }
#endif
#if defined(A2DP_LDAC_ON)
       if(btif_a2dp_get_stream_state(app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream);
       }
#endif

#if defined(A2DP_AAC_ON)
        if(btif_a2dp_get_stream_state( app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state( app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream);
        }
#endif
#if defined(A2DP_SCALABLE_ON)
        if(btif_a2dp_get_stream_state( app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream);
        }
#endif
        if( btif_avrcp_get_remote_device(app_bt_device.avrcp_channel[BT_DEVICE_ID_1]->avrcp_channel_handle))
        {
            btif_avrcp_disconnect(app_bt_device.avrcp_channel[BT_DEVICE_ID_1]->avrcp_channel_handle);
        }
#ifdef BT_PBAP_SUPPORT
        if (app_bt_pbap_is_connected(curr_device->pbap_channel))
        {
            app_bt_disconnect_pbap_profile(curr_device->pbap_channel);
        }
#endif
#ifdef __BT_ONE_BRING_TWO__
            TRACE(3,"%s id2 hf:%d a2dp:%d",__func__, btif_get_hf_chan_state(app_bt_device.hf_channel[BT_DEVICE_ID_2]), btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_2]->a2dp_stream));
            //if(app_bt_device.hf_channel[BT_DEVICE_ID_2].state == HF_STATE_OPEN){
            if(btif_get_hf_chan_state(app_bt_device.hf_channel[BT_DEVICE_ID_2]) == BTIF_HF_STATE_OPEN){
                app_bt_HF_DisconnectServiceLink(app_bt_device.hf_channel[BT_DEVICE_ID_2]);
            }

#if defined (__HSP_ENABLE__)
            if(app_bt_device.hs_channel[BT_DEVICE_ID_2].state == HS_STATE_OPEN){
                app_bt_HS_DisconnectServiceLink(&app_bt_device.hs_channel[BT_DEVICE_ID_2]);
            }
#endif // __HSP_ENABLE__

        if( btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_2]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_stream[BT_DEVICE_ID_2]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_stream[BT_DEVICE_ID_2]->a2dp_stream);
        }
#if defined(A2DP_LHDC_ON)
        if(btif_a2dp_get_stream_state(app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_2]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_2]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_2]->a2dp_stream);
        }
#endif // A2DP_LHDC_ON
#if defined(A2DP_AAC_ON)
        if(btif_a2dp_get_stream_state(app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_STREAMING ||
            btif_a2dp_get_stream_state(app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2]->a2dp_stream)== BTIF_AVDTP_STRM_STATE_OPEN){
            app_bt_A2DP_CloseStream(app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2]->a2dp_stream);
        }
#endif // A2DP_AAC_ON
#if defined(A2DP_SCALABLE_ON)
            if(btif_a2dp_get_stream_state(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2]->a2dp_stream)== BTIF_AVDTP_STRM_STATE_STREAMING ||
                    btif_a2dp_get_stream_state(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2]->a2dp_stream) == BTIF_AVDTP_STRM_STATE_OPEN){
                app_bt_A2DP_CloseStream(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2]->a2dp_stream);
            }
#endif // A2DP_SCALABLE_ON
        if( btif_avrcp_get_remote_device(app_bt_device.avrcp_channel[BT_DEVICE_ID_2]->avrcp_channel_handle))    {
            btif_avrcp_disconnect(app_bt_device.avrcp_channel[BT_DEVICE_ID_2]->avrcp_channel_handle);
        }
#endif //__BT_ONE_BRING_TWO__

#ifdef BISTO_ENABLED
        gsound_custom_bt_disconnect_all_channel();
#endif
    }

#ifdef __IAG_BLE_INCLUDE__
    if(app_ble_is_any_connection_exist())
    {
#ifdef GFPS_ENABLED
        if (!app_gfps_is_last_response_pending())
#endif
        app_ble_disconnect_all();
    }
#endif

    if(osapi_lock_is_exist())
        osapi_unlock_stack();

    osDelay(500);

    if(Tmp_activeCons)
    {
        btif_remote_device_t* remDev = app_bt_get_remoteDev(BT_DEVICE_ID_1);
        if (NULL != remDev)
        {
            app_bt_MeDisconnectLink(remDev);
        }

#ifdef __BT_ONE_BRING_TWO__
        remDev = app_bt_get_remoteDev(BT_DEVICE_ID_2);
        if (NULL != remDev)
        {
            osDelay(200);
            app_bt_MeDisconnectLink(remDev);
        }
#endif
    }
    return BT_STS_SUCCESS;
#endif
}

void app_disconnect_all_bt_connections(void)
{
    LinkDisconnectDirectly(false);
}

bool is_need_to_reject_io_cap_requeset(void *bdaddr)
{
	bool nRet = false;
	bt_bdaddr_t *pbd_addr = (bt_bdaddr_t *)bdaddr;

	if(bdaddr != NULL){
		nRet = app_bt_profile_connect_openreconnecting(btif_me_get_remote_device_by_bdaddr(pbd_addr));
	}
    return nRet;
}

void app_bt_init(void)
{
    app_bt_mail_init();
    app_set_threadhandle(APP_MODUAL_BT, app_bt_handle_process);
#if defined(ENHANCED_STACK)
#if !defined(IBRT)
    btif_me_register_io_capbility_callback(is_need_to_reject_io_cap_requeset);
#endif
#else
	btif_me_sec_set_io_cap_rsp_reject_ext(app_bt_profile_connect_openreconnecting);
#endif

    app_bt_active_mode_manager_init();
}

extern "C" bool app_bt_has_connectivitys(void)
{
    int activeCons;
    osapi_lock_stack();
    activeCons = btif_me_get_activeCons();
    osapi_unlock_stack();

    if(activeCons > 0)
        return true;

    return false;
#if 0
    if(app_bt_device.hf_channel[BT_DEVICE_ID_1].cmgrHandler.remDev)
        return true;
    if(app_bt_device.a2dp_stream[BT_DEVICE_ID_1].device->cmgrHandler.remDev)
        return true;
#ifdef __BT_ONE_BRING_TWO__
    if(app_bt_device.hf_channel[BT_DEVICE_ID_2].cmgrHandler.remDev)
        return true;
    if(app_bt_device.a2dp_stream[BT_DEVICE_ID_2].device->cmgrHandler.remDev)
        return true;
#endif
    return false;
#endif
}


#ifdef __TWS_CHARGER_BOX__

extern "C" {
    bt_status_t ME_Ble_Clear_Whitelist(void);
    bt_status_t ME_Ble_Set_Private_Address(BT_BD_ADDR *addr);
    bt_status_t ME_Ble_Add_Dev_To_Whitelist(U8 addr_type,BT_BD_ADDR *addr);
    bt_status_t ME_Ble_SetAdv_data(U8 len, U8 *data);
    bt_status_t ME_Ble_SetScanRsp_data(U8 len, U8 *data);
    bt_status_t ME_Ble_SetAdv_parameters(adv_para_struct *para);
    bt_status_t ME_Ble_SetAdv_en(U8 en);
    bt_status_t ME_Ble_Setscan_parameter(scan_para_struct *para);
    bt_status_t ME_Ble_Setscan_en(U8 scan_en,  U8 filter_duplicate);
}


int8_t power_level=0;
#define TWS_BOX_OPEN 1
#define TWS_BOX_CLOSE 0
void app_tws_box_set_slave_adv_data(uint8_t power_level,uint8_t box_status)
{
    uint8_t adv_data[] = {
        0x02,0xfe, 0x00,
        0x02, 0xfd, 0x00  // manufacturer data
    };

    adv_data[2] = power_level;

    adv_data[5] = box_status;
    ME_Ble_SetAdv_data(sizeof(adv_data), adv_data);

}


void app_tws_box_set_slave_adv_para(void)
{
    uint8_t  peer_addr[BTIF_BD_ADDR_SIZE] = {0};
    adv_para_struct para;


    para.interval_min =             0x0040; // 20ms
    para.interval_max =             0x0040; // 20ms
    para.adv_type =                 0x03;
    para.own_addr_type =            0x01;
    para.peer_addr_type =           0x01;
    para.adv_chanmap =              0x07;
    para.adv_filter_policy =        0x00;
    memcpy(para.bd_addr.addr, peer_addr, BTIF_BD_ADDR_SIZE);

    ME_Ble_SetAdv_parameters(&para);

}


extern uint8_t bt_addr[6];
void app_tws_start_chargerbox_adv(void)
{
    app_tws_box_set_slave_adv_data(power_level,TWS_BOX_OPEN);
    ME_Ble_Set_Private_Address((BT_BD_ADDR *)bt_addr);
    app_tws_box_set_slave_adv_para();
    ME_Ble_SetAdv_en(1);

}



#endif

bool app_is_hfp_service_connected(void)
{
    return (bt_profile_manager[BT_DEVICE_ID_1].hfp_connect == bt_profile_connect_status_success);
}


btif_remote_device_t* app_bt_get_remoteDev(uint8_t deviceId)
{
    btif_remote_device_t* currentRemDev = NULL;

    if(btif_a2dp_get_stream_state( app_bt_device.a2dp_stream[deviceId]->a2dp_stream)
        == BTIF_AVDTP_STRM_STATE_STREAMING ||
        btif_a2dp_get_stream_state( app_bt_device.a2dp_stream[deviceId]->a2dp_stream)
        == BTIF_AVDTP_STRM_STATE_OPEN)
    {
        currentRemDev = btif_a2dp_get_stream_conn_remDev(app_bt_device.a2dp_stream[deviceId]->a2dp_stream);
    }
    else if (btif_get_hf_chan_state(app_bt_device.hf_channel[deviceId]) == BTIF_HF_STATE_OPEN)
    {
        currentRemDev = (btif_remote_device_t *)btif_hf_cmgr_get_remote_device(app_bt_device.hf_channel[deviceId]);
    }

    TRACE(2,"%s get current Remdev %p", __FUNCTION__, currentRemDev);

    return currentRemDev;
}

void app_bt_stay_active_rem_dev(btif_remote_device_t* pRemDev)
{
    if (pRemDev)
    {
        btif_cmgr_handler_t    *cmgrHandler;
        /* Clear the sniff timer */
        cmgrHandler = btif_cmgr_get_acl_handler(pRemDev);
        btif_cmgr_clear_sniff_timer(cmgrHandler);
        btif_cmgr_disable_sniff_timer(cmgrHandler);
        app_bt_Me_SetLinkPolicy(pRemDev, BTIF_BLP_MASTER_SLAVE_SWITCH);
    }
}

void app_bt_stay_active(uint8_t deviceId)
{
    btif_remote_device_t* currentRemDev = app_bt_get_remoteDev(deviceId);
    app_bt_stay_active_rem_dev(currentRemDev);
}

void app_bt_allow_sniff_rem_dev(btif_remote_device_t* pRemDev)
{
    if (pRemDev && (BTIF_BDS_CONNECTED ==  btif_me_get_remote_device_state(pRemDev)))
    {
        btif_cmgr_handler_t    *cmgrHandler;
        /* Enable the sniff timer */
        cmgrHandler = btif_cmgr_get_acl_handler(pRemDev);

        /* Start the sniff timer */
        btif_sniff_info_t sniffInfo;
        sniffInfo.minInterval = BTIF_CMGR_SNIFF_MIN_INTERVAL;
        sniffInfo.maxInterval = BTIF_CMGR_SNIFF_MAX_INTERVAL;
        sniffInfo.attempt = BTIF_CMGR_SNIFF_ATTEMPT;
        sniffInfo.timeout = BTIF_CMGR_SNIFF_TIMEOUT;
        if (cmgrHandler){
            btif_cmgr_set_sniff_timer(cmgrHandler, &sniffInfo, BTIF_CMGR_SNIFF_TIMER);
        }
        app_bt_Me_SetLinkPolicy(pRemDev, BTIF_BLP_MASTER_SLAVE_SWITCH | BTIF_BLP_SNIFF_MODE);
    }
}

extern "C" uint8_t is_sco_mode (void);
void app_bt_allow_sniff(uint8_t deviceId)
{
    if (a2dp_is_music_ongoing() || is_sco_mode() || btapp_hfp_is_call_active())
    {
        return;
    }
    btif_remote_device_t* currentRemDev = app_bt_get_remoteDev(deviceId);
    app_bt_allow_sniff_rem_dev(currentRemDev);
}

void app_bt_stop_sniff(uint8_t deviceId)
{
    btif_remote_device_t* currentRemDev = app_bt_get_remoteDev(deviceId);

    if (currentRemDev && (btif_me_get_remote_device_state(currentRemDev) == BTIF_BDS_CONNECTED)){
        if (btif_me_get_current_mode(currentRemDev) == BTIF_BLM_SNIFF_MODE){
            TRACE(1,"!!! stop sniff currmode:%d\n", btif_me_get_current_mode(currentRemDev));
            app_bt_ME_StopSniff(currentRemDev);
        }
    }
}

bool app_bt_is_device_connected(uint8_t deviceId)
{
    if (deviceId < BT_DEVICE_NUM) {
        return bt_profile_manager[deviceId].has_connected;
    } else {
        // Indicate no connection is user passes invalid deviceId
        return false;
    }
}

#if defined(__BT_SELECT_PROF_DEVICE_ID__)
int8_t app_bt_a2dp_is_same_stream(a2dp_stream_t *src_Stream, a2dp_stream_t *dst_Stream)
{
    return btif_a2dp_is_register_codec_same(src_Stream, dst_Stream);
}
void app_bt_a2dp_find_same_unused_stream(a2dp_stream_t *in_Stream, a2dp_stream_t **out_Stream, uint32_t device_id)
{
    *out_Stream = NULL;
    if (app_bt_a2dp_is_same_stream(app_bt_device.a2dp_stream[device_id]->a2dp_stream, in_Stream))
        *out_Stream = app_bt_device.a2dp_stream[device_id]->a2dp_stream;
#if defined(A2DP_LHDC_ON)
    else if (app_bt_a2dp_is_same_stream(app_bt_device.a2dp_lhdc_stream[device_id]->a2dp_stream, in_Stream))
        *out_Stream = app_bt_device.a2dp_lhdc_stream[device_id]->a2dp_stream;
#endif
#if defined(A2DP_LDAC_ON)
    else if (app_bt_a2dp_is_same_stream(app_bt_device.a2dp_ldac_stream[device_id]->a2dp_stream, in_Stream))
        *out_Stream = app_bt_device.a2dp_ldac_stream[device_id]->a2dp_stream;
#endif
#if defined(A2DP_AAC_ON)
    else if (app_bt_a2dp_is_same_stream(app_bt_device.a2dp_aac_stream[device_id]->a2dp_stream, in_Stream))
        *out_Stream = app_bt_device.a2dp_aac_stream[device_id]->a2dp_stream;
#endif
#if defined(A2DP_SCALABLE_ON)
    else if (app_bt_a2dp_is_same_stream(app_bt_device.a2dp_scalable_stream[device_id]->a2dp_stream, in_Stream))
        *out_Stream = app_bt_device.a2dp_scalable_stream[device_id]->a2dp_stream;
#endif
}
int8_t app_bt_a2dp_is_stream_on_device_id(a2dp_stream_t *in_Stream, uint32_t device_id)
{
    if (app_bt_device.a2dp_stream[device_id]->a2dp_stream == in_Stream)
        return 1;
#if defined(A2DP_LHDC_ON)
    else if (app_bt_device.a2dp_lhdc_stream[device_id]->a2dp_stream == in_Stream)
        return 1;
#endif
#if defined(A2DP_LDAC_ON)
    else if (app_bt_device.a2dp_ldac_stream[device_id]->a2dp_stream == in_Stream)
        return 1;
#endif
#if defined(A2DP_AAC_ON)
    else if (app_bt_device.a2dp_aac_stream[device_id]->a2dp_stream == in_Stream)
        return 1;
#endif
#if defined(A2DP_SCALABLE_ON)
    else if (app_bt_device.a2dp_scalable_stream[device_id]->a2dp_stream == in_Stream)
        return 1;
#endif
    return 0;
}
int8_t app_bt_hfp_is_chan_on_device_id(hf_chan_handle_t chan, uint32_t device_id)
{
    if (app_bt_device.hf_channel[device_id] == chan)
        return 1;
    return 0;
}
int8_t app_bt_is_any_profile_connected(uint32_t device_id)
{
    // TODO avrcp?spp?hid?bisto?ama?dma?rfcomm?
    if ((bt_profile_manager[device_id].hfp_connect == bt_profile_connect_status_success)
        || (bt_profile_manager[device_id].hsp_connect == bt_profile_connect_status_success)
             || (bt_profile_manager[device_id].a2dp_connect == bt_profile_connect_status_success)) {
        return 1;
    }

    return 0;
}
int8_t app_bt_is_a2dp_connected(uint32_t device_id)
{
    if (bt_profile_manager[device_id].a2dp_connect == bt_profile_connect_status_success)  {
        return 1;
    }

    return 0;
}
btif_remote_device_t *app_bt_get_connected_profile_remdev(uint32_t device_id)
{
    if (bt_profile_manager[device_id].a2dp_connect == bt_profile_connect_status_success) {
        return (btif_remote_device_t *)btif_a2dp_get_remote_device(app_bt_device.a2dp_connected_stream[device_id]);
    }
    else if (bt_profile_manager[device_id].hfp_connect == bt_profile_connect_status_success){
         return (btif_remote_device_t *)btif_hf_cmgr_get_remote_device(app_bt_device.hf_channel[device_id]);
    }
#if defined (__HSP_ENABLE__)
    else if (bt_profile_manager[device_id].hsp_connect == bt_profile_connect_status_success){
        // TODO hsp support
        //return (btif_remote_device_t *)btif_hs_cmgr_get_remote_device(app_bt_device.hs_channel[i]);
    }
#endif

    return NULL;
}
#endif

bool app_bt_get_device_bdaddr(uint8_t deviceId, uint8_t* btAddr)
{
    bool ret = false;

    if (app_bt_is_device_connected(deviceId))
    {
        btif_remote_device_t* currentRemDev = app_bt_get_remoteDev(deviceId);

        if (currentRemDev)
        {
            memcpy(btAddr,  btif_me_get_remote_device_bdaddr(currentRemDev)->address, BTIF_BD_ADDR_SIZE);
            ret = true;
        }
    }

    return ret;
}

void fast_pair_enter_pairing_mode_handler(void)
{
#if defined(IBRT)
    app_ibrt_ui_judge_scan_type(IBRT_FASTPAIR_TRIGGER, MOBILE_LINK, 0);
#else
    app_bt_accessmode_set(BTIF_BAM_GENERAL_ACCESSIBLE);
#endif

#ifdef __INTERCONNECTION__
    clear_discoverable_adv_timeout_flag();
    app_interceonnection_start_discoverable_adv(INTERCONNECTION_BLE_FAST_ADVERTISING_INTERVAL,
                                                APP_INTERCONNECTION_FAST_ADV_TIMEOUT_IN_MS);
#endif
}

bool app_bt_is_hfp_audio_on(void)
{
	bool hfp_audio_is_on = false;
	for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
		if(BTIF_HF_AUDIO_CON == app_bt_device.hf_audio_state[i]){
			hfp_audio_is_on = true;
			break;
		}
	}
	return hfp_audio_is_on;
}

btif_remote_device_t* app_bt_get_connected_mobile_device_ptr(void)
{
    return connectedMobile;
}
void app_bt_set_spp_device_ptr(btif_remote_device_t* device)
{
    TRACE(2,"%s set sppOpenMobile is %p", __func__,device);
    sppOpenMobile = device;
    return;
}

btif_remote_device_t* app_bt_get_spp_device_ptr(void)
{
    TRACE(2,"%s sppOpenMobile %p", __func__,sppOpenMobile);
    ASSERT((sppOpenMobile != NULL), "sppOpenMobile is NULL!!!!!!!!!!!!");
    return sppOpenMobile;
}

#ifdef BT_USB_AUDIO_DUAL_MODE
#include "a2dp_api.h"
extern "C" a2dp_stream_t* app_bt_get_steam(enum BT_DEVICE_ID_T id)
{
    a2dp_stream_t* stream;

    stream = (a2dp_stream_t*)bt_profile_manager[id].stream;
    return stream;
}

extern "C" int app_bt_get_bt_addr(enum BT_DEVICE_ID_T id,bt_bdaddr_t *bdaddr)
{
    memcpy(bdaddr,&bt_profile_manager[id].rmt_addr,sizeof(bt_bdaddr_t));
    return 0;
}

extern "C" bool app_bt_a2dp_service_is_connected(void)
{
    return (bt_profile_manager[BT_DEVICE_ID_1].a2dp_connect == bt_profile_connect_status_success);
}
#endif


struct app_bt_search_t
{
    bool search_start;
    bool inquiry_pending;
    bool device_searched;
    bt_bdaddr_t address;
};

static bool app_bt_search_device_match(const bt_bdaddr_t* addr, const char* name)
{
    TRACE(7,"app_bt_search_callback found device %02x:%02x:%02x:%02x:%02x:%02x '%s'\n",
            addr->address[0], addr->address[1], addr->address[2], addr->address[3],
            addr->address[4], addr->address[5], name);

#if defined(HFP_MOBILE_AG_ROLE)
    bt_bdaddr_t test_device1 = {{0xd2, 0x53, 0x86, 0x42, 0x71, 0x31}};
    bt_bdaddr_t test_device2 = {{0xd3, 0x53, 0x86, 0x42, 0x71, 0x31}};
    return (memcmp(addr, test_device1.address, sizeof(bt_bdaddr_t)) == 0 ||
            memcmp(addr, test_device2.address, sizeof(bt_bdaddr_t)) == 0);
#else
    return false;
#endif
}

static struct app_bt_search_t g_bt_search;
static void app_bt_search_callback(const btif_event_t* event)
{
    TRACE(2,"%s event %d\n", __func__, btif_me_get_callback_event_type(event));

    switch(btif_me_get_callback_event_type(event))
    {
        case BTIF_BTEVENT_INQUIRY_RESULT:
            {
                bt_bdaddr_t *addr = btif_me_get_callback_event_inq_result_bd_addr(event);
                uint8_t mode = btif_me_get_callback_event_inq_result_inq_mode(event);
                const int NAME_MAX_LEN = 255;
                char device_name[NAME_MAX_LEN+1] = {0};
                int device_name_len = 0;
                uint8_t *eir = NULL;

                if ((mode == BTIF_INQ_MODE_EXTENDED) &&
                    (eir = btif_me_get_callback_event_inq_result_ext_inq_resp(event)))
                {
                    device_name_len = btif_me_get_ext_inq_data(eir, 0x09, (uint8_t *)device_name, NAME_MAX_LEN);
                }

                if (app_bt_search_device_match(addr, device_name_len > 0 ? device_name : ""))
                {
                    g_bt_search.address = *addr;
                    g_bt_search.device_searched = true;
                    btif_me_cancel_inquiry();
                }
            }
            break;
        case BTIF_BTEVENT_INQUIRY_COMPLETE:
        case BTIF_BTEVENT_INQUIRY_CANCELED:
            btif_me_unregister_globa_handler((btif_handler *)btif_me_get_bt_handler());
            g_bt_search.search_start = false;
            g_bt_search.inquiry_pending = false;
            if (g_bt_search.device_searched)
            {
#if defined(HFP_MOBILE_AG_ROLE)
                bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_null;
                bt_profile_manager[BT_DEVICE_ID_1].rmt_addr = g_bt_search.address;
                bt_profile_manager[BT_DEVICE_ID_1].chan = app_bt_device.hf_channel[BT_DEVICE_ID_1];
                app_bt_precheck_before_starting_connecting(bt_profile_manager[BT_DEVICE_ID_1].has_connected);
                app_bt_HF_CreateServiceLink(bt_profile_manager[BT_DEVICE_ID_1].chan, &bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
#endif
            }
            else
            {
                TRACE(1,"%s no device matched\n", __func__);
            #if 0
                /* continue to search ??? */
                app_bt_start_search();
            #endif
            }
            break;
        default:
            break;
    }
}

void app_bt_start_search(void)
{
    uint8_t max_search_time = 10; /* 12.8s */

    if (g_bt_search.search_start)
    {
        TRACE(1,"%s already started\n", __func__);
        return;
    }

    btif_me_set_handler(btif_me_get_bt_handler(), app_bt_search_callback);

    btif_me_set_event_mask(btif_me_get_bt_handler(),
            BTIF_BEM_INQUIRY_RESULT | BTIF_BEM_INQUIRY_COMPLETE | BTIF_BEM_INQUIRY_CANCELED |
            BTIF_BEM_LINK_CONNECT_IND | BTIF_BEM_LINK_CONNECT_CNF | BTIF_BEM_LINK_DISCONNECT |
            BTIF_BEM_ROLE_CHANGE | BTIF_BEM_MODE_CHANGE);

    btif_me_register_global_handler(btif_me_get_bt_handler());

    g_bt_search.search_start = true;
    g_bt_search.device_searched = false;
    g_bt_search.inquiry_pending = false;

    if (BT_STS_PENDING != btif_me_inquiry(BTIF_BT_IAC_GIAC, max_search_time, 0))
    {
        TRACE(1,"%s start inquiry failed\n", __func__);
        g_bt_search.inquiry_pending = true;
    }
}

uint8_t app_bt_avrcp_get_notify_trans_id(void)
{
    return btif_a2dp_get_avrcpadvancedpdu_trans_id(app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1]);
}

void app_bt_avrcp_set_notify_trans_id(uint8_t trans_id)
{
    TRACE(3,"%s %d %p\n", __func__, trans_id, app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1]);
    btif_a2dp_set_avrcpadvancedpdu_trans_id(app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1], trans_id);
}

uint8_t app_bt_avrcp_get_ctl_trans_id(void)
{
    return btif_avrcp_get_ctl_trans_id(app_bt_device.avrcp_channel[BT_DEVICE_ID_1]);
}

void app_bt_avrcp_set_ctl_trans_id(uint8_t trans_id)
{
    TRACE(3,"%s %d %p\n", __func__, trans_id, app_bt_device.avrcp_channel[BT_DEVICE_ID_1]);
    btif_avrcp_set_ctl_trans_id(app_bt_device.avrcp_channel[BT_DEVICE_ID_1], trans_id);
}

#if defined(IBRT)
#if defined(ENHANCED_STACK)
uint32_t app_bt_save_spp_app_ctx(uint32_t app_id,btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len)
{
    bt_bdaddr_t *remote = NULL;
    uint32_t offset = 0;
    struct spp_device *device = (struct spp_device *)btif_spp_get_device(app_id);
    ASSERT(device, "%s NULL spp device app_id=0x%x",__func__, app_id);

    // save app_id
    buf[offset++] = app_id & 0xFF;
    buf[offset++] = (app_id >> 8) & 0xFF;
    buf[offset++] = (app_id >> 16) & 0xFF;
    buf[offset++] = (app_id >> 24) & 0xFF;

    // save port type
    buf[offset++] = device->portType;
    
    //save remote address
    remote = btif_me_get_remote_device_bdaddr(rem_dev);
    memcpy(buf+offset, remote, sizeof(bt_bdaddr_t));
    offset += sizeof(bt_bdaddr_t);

    //TRACE(7,"%s:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
    //    __func__, remote->addr[5], remote->addr[4], remote->addr[3],
    //               remote->addr[2], remote->addr[1], remote->addr[0]);

    //spp device
    buf[offset++] = device->spp_connected_flag;

    return offset;
}

uint32_t app_bt_restore_spp_app_ctx(uint8_t *buf, uint32_t buf_len, uint32_t *app_id)
{
    bt_bdaddr_t remote;
    uint32_t offset = 0;
    struct spp_device *device = NULL;
    uint8_t i = 0;
    uint8_t port_type = 0;
    
    *app_id = 0x00; //clean before restore
    // restore app_id
    for (i=0; i<4; i++)
    {
        *app_id |= (buf[offset+i]<<(8*i));
    }
    offset += 4;

    port_type = buf[offset++];

    // restore remote address
    memcpy(&remote, buf+offset, sizeof(remote));
    offset += sizeof(remote);

    device = (struct spp_device *)btif_spp_get_device(*app_id);

    if (device == NULL)
    {
        /*
         * SPP client device may not be created in bt host initialized stage,so IBRT SLAVE will restore it
         */
        if (port_type == BTIF_SPP_CLIENT_PORT)
        {
            switch (*app_id)
            {
#ifdef __INTERCONNECTION__
                case BTIF_APP_SPP_CLIENT_CCMP_ID:
                    app_ccmp_client_open((uint8_t *)SppServiceSearchReq, app_interconnection_get_length(), 0, 1);
                    device = (struct spp_device *)btif_spp_get_device(BTIF_APP_SPP_CLIENT_CCMP_ID);
                    device->spp_callback = ccmp_callback;
                    //device->_channel = chnl; //restore in btif_spp_profile_restore_ctx
                    device->sppUsedFlag = 1;
                    break;
                    
                case BTIF_APP_SPP_CLIENT_RED_ID:
                    btif_remote_device_t *remote_device = NULL;
                    app_spp_client_open((uint8_t*)SppServiceSearchReq, app_interconnection_get_length(),1);
                    device = (struct spp_device *)btif_spp_get_device(BTIF_APP_SPP_CLIENT_RED_ID);
                    device->spp_callback = spp_client_callback;
                    //device->_channel = chnl; //restore in btif_spp_profile_restore_ctx
                    device->sppUsedFlag = 1;
                    remote_device = btif_me_get_remote_device_by_bdaddr(&remote);
                    app_bt_set_spp_device_ptr(remote_device);
                    break;
#endif
           
                default:
                    ASSERT(device, "%s NULL spp client device app_id=0x%x", __func__, *app_id);
                    break;
            }
        }
        else
        {
            ASSERT(device, "%s NULL spp server device app_id=0x%x", __func__, *app_id);
        }
    }
    
    //restore spp device
    device->portType = port_type;
    device->spp_connected_flag = buf[offset++];

    return offset;

}

uint32_t app_bt_save_hfp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len)
{
    BTIF_CTX_INIT(buf);

    BTIF_CTX_STR_BUF(btif_me_get_remote_device_bdaddr(rem_dev), BTIF_BD_ADDR_SIZE);

    BTIF_CTX_STR_VAL8(app_bt_device.hfchan_call[BT_DEVICE_ID_1]);
    BTIF_CTX_STR_VAL8(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1]);
    BTIF_CTX_STR_VAL8(app_bt_device.hf_callheld[BT_DEVICE_ID_1]);

    BTIF_CTX_SAVE_UPDATE_DATA_LEN();
    return BTIF_CTX_GET_TOTAL_LEN();
}

uint32_t app_bt_restore_hfp_app_ctx(uint8_t *buf, uint32_t buf_len)
{
    bt_bdaddr_t remote;
    uint8_t call, callsetup, callheld;
    BTIF_CTX_INIT(buf);

    BTIF_CTX_LDR_BUF(&remote, BTIF_BD_ADDR_SIZE);

    BTIF_CTX_LDR_VAL8(call);
    BTIF_CTX_LDR_VAL8(callsetup);
    BTIF_CTX_LDR_VAL8(callheld);

    app_bt_device.hfchan_call[BT_DEVICE_ID_1] = call;
    app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] = callsetup;
    app_bt_device.hf_callheld[BT_DEVICE_ID_1] = callheld;

    TRACE(4,"%s call %d callsetup %d callheld %d", __func__, call, callsetup, callheld);

    return BTIF_CTX_GET_TOTAL_LEN();
}
uint32_t app_bt_save_a2dp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len)
{
    uint32_t offset = 0;
    unsigned char stream_enc = 0;

    // TODO
    // more codecs, BT_DEVICE_ID_2
    if (bt_profile_manager[BT_DEVICE_ID_1].stream == app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream) {
        stream_enc = 0;
    }
#if defined(A2DP_AAC_ON)
    else if (bt_profile_manager[BT_DEVICE_ID_1].stream == app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream) {
        stream_enc = 1;
    }
#endif
#if defined(A2DP_LHDC_ON)
    else if (bt_profile_manager[BT_DEVICE_ID_1].stream == app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream) {
        stream_enc = 2;
    }
#endif

    buf[offset++] = stream_enc;
    memcpy(buf+offset,btif_me_get_remote_device_bdaddr(rem_dev),BTIF_BD_ADDR_SIZE);
    offset += BTIF_BD_ADDR_SIZE;

    buf[offset++] = app_bt_device.a2dp_state[BT_DEVICE_ID_1];
    buf[offset++] = app_bt_device.a2dp_play_pause_flag;
    buf[offset++] = avrcp_get_media_status();

    //codec type
    buf[offset++] = app_bt_device.codec_type[BT_DEVICE_ID_1];
    buf[offset++] = app_bt_device.sample_rate[BT_DEVICE_ID_1];
    buf[offset++] = app_bt_device.sample_bit[BT_DEVICE_ID_1];

#if defined(__A2DP_AVDTP_CP__)
    buf[offset++] = app_bt_device.avdtp_cp[BT_DEVICE_ID_1];
#endif

    return offset;
}

uint32_t app_bt_restore_a2dp_app_ctx(uint8_t *buf, uint32_t buf_len)
{
    uint32_t offset = 0;
    bt_bdaddr_t remote;
    unsigned char stream_enc = 0;

    stream_enc = buf[offset++];

    memcpy(&remote,buf+offset,BTIF_BD_ADDR_SIZE);
    offset += BTIF_BD_ADDR_SIZE;


    app_bt_device.a2dp_state[BT_DEVICE_ID_1]  = buf[offset++];
    app_bt_device.a2dp_play_pause_flag = buf[offset++];
    avrcp_set_media_status(buf[offset++]);

    //codec type
    app_bt_device.codec_type[BT_DEVICE_ID_1] = buf[offset++];
    app_bt_device.sample_rate[BT_DEVICE_ID_1] = buf[offset++];
    app_bt_device.sample_bit[BT_DEVICE_ID_1] = buf[offset++];

#if defined(__A2DP_AVDTP_CP__)
    buf[offset++] = app_bt_device.avdtp_cp[BT_DEVICE_ID_1];
#endif

     // TODO
    // more codecs, BT_DEVICE_ID_2
    if (stream_enc == 0) {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
#if defined(A2DP_AAC_ON)
    else if (stream_enc == 1) {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
#endif
#if defined(A2DP_LHDC_ON)
    else if (stream_enc == 2) {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
#endif

    memcpy(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.address, &remote, BTIF_BD_ADDR_SIZE);
    bt_profile_manager[BT_DEVICE_ID_1].a2dp_connect = bt_profile_connect_status_success;
    bt_profile_manager[BT_DEVICE_ID_1].hfp_connect = bt_profile_connect_status_success;
    bt_profile_manager[BT_DEVICE_ID_1].has_connected = true;

    return offset;
}

uint32_t app_bt_save_avrcp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len)
{
    uint32_t offset = 0;

    buf[offset++] = app_bt_device.avrcp_state[BT_DEVICE_ID_1];
    buf[offset++] = app_bt_device.volume_report[BT_DEVICE_ID_1];

    if (app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1])
    {
        buf[offset++] = true;
        buf[offset++] = app_bt_avrcp_get_notify_trans_id();
    }
    else
    {
        buf[offset++] = false;
        buf[offset++] = 0;
    }

    return offset;
}

uint32_t app_bt_restore_avrcp_app_ctx(uint8_t *buf, uint32_t buf_len)
{
    uint32_t offset = 0;
    uint8_t notify_rsp_exist = 0;
    uint8_t trans_id = 0;

    app_bt_device.avrcp_state[BT_DEVICE_ID_1] = buf[offset++];
    app_bt_device.volume_report[BT_DEVICE_ID_1] = buf[offset++];
    notify_rsp_exist = buf[offset++];
    trans_id = buf[offset++];

    TRACE(4,"app_bt_restore_avrcp_app_ctx state %d report %d notify %d %d\n",
        app_bt_device.avrcp_state[BT_DEVICE_ID_1], app_bt_device.volume_report[BT_DEVICE_ID_1], notify_rsp_exist, trans_id);

    if (notify_rsp_exist && app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1] == NULL)
    {
        btif_app_a2dp_avrcpadvancedpdu_mempool_calloc(&app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1]);
    }

    app_bt_avrcp_set_notify_trans_id(trans_id);

    return offset;
}

#ifdef BT_MAP_SUPPORT
uint32_t app_bt_save_map_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len)
{
    //struct bdaddr_t *remote = NULL;
    uint32_t offset = 0;

    memcpy((void *)buf, (void *)app_bt_device.map_session_handle, sizeof(app_bt_device.map_session_handle));
    offset += sizeof(app_bt_device.map_session_handle);

    return offset;
}

uint32_t app_bt_restore_map_app_ctx(uint8_t *buf, uint32_t buf_len)
{
    uint32_t offset = 0;

    memcpy((void *)app_bt_device.map_session_handle, (void *)buf, sizeof(btif_map_session_handle_t));
    offset += sizeof(btif_map_session_handle_t);

    return offset;
}
#endif

#endif /* ENHANCED_STACK */
a2dp_stream_t * app_bt_get_mobile_a2dp_stream(uint32_t deviceId)
{
    return bt_profile_manager[deviceId].stream;
}

void app_bt_update_bt_profile_manager(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    memcpy(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.address, p_ibrt_ctrl->mobile_addr.address, BTIF_BD_ADDR_SIZE);

#if defined(A2DP_AAC_ON)
    if(p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream;

    }
    else
#endif
#if defined(A2DP_SCALABLE_ON)
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
    else
#endif
#if defined(A2DP_LHDC_ON)
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
    else
#endif
#if defined(A2DP_LDAC_ON)
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
    else
#endif
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_SBC){
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }else{
        ASSERT(0, "%s err codec_type:%d ", __func__, p_ibrt_ctrl->a2dp_codec.codec_type);
    }

    bt_profile_manager[BT_DEVICE_ID_1].a2dp_connect = bt_profile_connect_status_success;
    bt_profile_manager[BT_DEVICE_ID_1].hfp_connect = bt_profile_connect_status_success;
    bt_profile_manager[BT_DEVICE_ID_1].has_connected = true;

    TRACE(3,"%s codec_type:%x if_a2dp_stream:%p", __func__, p_ibrt_ctrl->a2dp_codec.codec_type, bt_profile_manager[BT_DEVICE_ID_1].stream);
    DUMP8("%02x ", bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.address, BTIF_BD_ADDR_SIZE);
}

void app_bt_update_bt_profile_manager_codec_type(uint8_t  codec_type)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    p_ibrt_ctrl->a2dp_codec.codec_type = codec_type;

#if defined(A2DP_AAC_ON)
    if(p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream;

    }
    else
#endif
#if defined(A2DP_SCALABLE_ON)
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
    else
#endif
#if defined(A2DP_LHDC_ON)
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
    else
#endif
#if defined(A2DP_LDAC_ON)
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_NON_A2DP)
    {
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }
    else
#endif
    if (p_ibrt_ctrl->a2dp_codec.codec_type == BTIF_AVDTP_CODEC_TYPE_SBC){
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream;
    }else{
        ASSERT(0, "%s err codec_type:%d ", __func__, p_ibrt_ctrl->a2dp_codec.codec_type);
    }

    bt_profile_manager[BT_DEVICE_ID_1].a2dp_connect = bt_profile_connect_status_success;
    bt_profile_manager[BT_DEVICE_ID_1].hfp_connect = bt_profile_connect_status_success;
    bt_profile_manager[BT_DEVICE_ID_1].has_connected = true;

    TRACE(3,"%s codec_type:%x if_a2dp_stream:%p", __func__, p_ibrt_ctrl->a2dp_codec.codec_type, bt_profile_manager[BT_DEVICE_ID_1].stream);
}

static bool ibrt_reconnect_mobile_profile_flag = false;
void app_bt_ibrt_reconnect_mobile_profile_flag_set(void)
{
    ibrt_reconnect_mobile_profile_flag = true;
}

void app_bt_ibrt_reconnect_mobile_profile_flag_clear(void)
{
    ibrt_reconnect_mobile_profile_flag = false;
}

bool app_bt_ibrt_reconnect_mobile_profile_flag_get(void)
{
    return ibrt_reconnect_mobile_profile_flag;
}

void app_bt_ibrt_reconnect_mobile_profile(bt_bdaddr_t mobile_addr)
{
    nvrec_btdevicerecord *mobile_record = NULL;

    bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_null;
    bt_profile_manager[BT_DEVICE_ID_1].rmt_addr = mobile_addr;
    bt_profile_manager[BT_DEVICE_ID_1].chan = app_bt_device.hf_channel[BT_DEVICE_ID_1];

    if (!nv_record_btdevicerecord_find(&mobile_addr, &mobile_record)){
#if defined(A2DP_AAC_ON)
        if(mobile_record->device_plf.a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC){
            bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]->a2dp_stream;
        }else
#endif
#if defined(A2DP_SCALABLE_ON)
        if(mobile_record->device_plf.a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP){
            bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]->a2dp_stream;
        }else
#endif
#if defined(A2DP_LHDC_ON)
        if(mobile_record->device_plf.a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP){
            bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_lhdc_stream[BT_DEVICE_ID_1]->a2dp_stream;
        }else
#endif
#if defined(A2DP_LDAC_ON)
        if(mobile_record->device_plf.a2dp_codectype == BTIF_AVDTP_CODEC_TYPE_NON_A2DP){
            bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_ldac_stream[BT_DEVICE_ID_1]->a2dp_stream;
        }else
#endif
        {
            bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream;
        }
    }else{
        bt_profile_manager[BT_DEVICE_ID_1].stream = app_bt_device.a2dp_stream[BT_DEVICE_ID_1]->a2dp_stream;//default using SBC
    }

    TRACE(0,"ibrt_ui_log:start reconnect mobile, addr below:");
    DUMP8("0x%02x ",&(mobile_addr.address[0]),BTIF_BD_ADDR_SIZE);
    app_bt_ibrt_reconnect_mobile_profile_flag_set();
    app_bt_precheck_before_starting_connecting(bt_profile_manager[BT_DEVICE_ID_1].has_connected);

    {
        app_bt_A2DP_OpenStream(bt_profile_manager[BT_DEVICE_ID_1].stream, &(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr));
        //app_bt_HF_CreateServiceLink(bt_profile_manager[BT_DEVICE_ID_1].chan, &(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr));
    }
    //osTimerStart(bt_profile_manager[BT_DEVICE_ID_1].connect_timer, APP_IBRT_RECONNECT_TIMEOUT_MS);
}
#endif

#ifdef __IAG_BLE_INCLUDE__
static void app_start_fast_connectable_ble_adv(uint16_t advInterval)
{
    bool ret = FALSE;

    if (NULL == app_fast_ble_adv_timeout_timer)
    {
        app_fast_ble_adv_timeout_timer =
            osTimerCreate(osTimer(APP_FAST_BLE_ADV_TIMEOUT_TIMER),
                          osTimerOnce,
                          NULL);
    }

    osTimerStart(app_fast_ble_adv_timeout_timer, APP_FAST_BLE_ADV_TIMEOUT_IN_MS);

#ifdef IBRT
    ret = app_ibrt_ui_get_snoop_via_ble_enable();
#endif

    if (FALSE == ret)
    {
        app_ble_start_connectable_adv(advInterval);
    }
}

static int app_fast_ble_adv_timeout_timehandler(void const *param)
{
    bool ret = FALSE;

#ifdef IBRT
    ret = app_ibrt_ui_get_snoop_via_ble_enable();
#endif

    if (FALSE == ret)
    {
        app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
    }

    return 0;
}

void app_stop_fast_connectable_ble_adv_timer(void)
{
    if (NULL != app_fast_ble_adv_timeout_timer)
    {
        osTimerStop(app_fast_ble_adv_timeout_timer);
    }
}
#endif


static uint32_t bt_link_active_mode_bits[MAX_ACTIVE_MODE_MANAGED_LINKS];

void app_bt_active_mode_manager_init(void)
{
    memset(bt_link_active_mode_bits, 0, sizeof(bt_link_active_mode_bits));
}

void app_bt_active_mode_reset(uint32_t linkIndex)
{
    bt_link_active_mode_bits[linkIndex] = 0;
}

void app_bt_active_mode_set(BT_LINK_ACTIVE_MODE_KEEPER_USER_E user, uint32_t linkIndex)
{
    bool isAlreadyInActiveMode = false;
    if (linkIndex < MAX_ACTIVE_MODE_MANAGED_LINKS)
    {
        uint32_t lock = int_lock_global();
        if (bt_link_active_mode_bits[linkIndex] > 0)
        {
            isAlreadyInActiveMode = true;
        }
        else
        {
            isAlreadyInActiveMode = false;
        }
        bt_link_active_mode_bits[linkIndex] |= (1 << user);
        int_unlock_global(lock);

        if (!isAlreadyInActiveMode)
        {
            app_bt_stop_sniff(linkIndex);
            app_bt_stay_active(linkIndex);
        }

    }
    else if (MAX_ACTIVE_MODE_MANAGED_LINKS == linkIndex)
    {
        for (uint8_t devId = 0;devId < BT_DEVICE_NUM;devId++)
        {
            uint32_t lock = int_lock_global();
            if (bt_link_active_mode_bits[devId] > 0)
            {
                isAlreadyInActiveMode = true;
            }
            else
            {
                isAlreadyInActiveMode = false;
            }
            bt_link_active_mode_bits[devId] |= (1 << user);
            int_unlock_global(lock);

            if (!isAlreadyInActiveMode)
            {
                app_bt_stop_sniff(devId);
                app_bt_stay_active(devId);
            }
        }
    }

    TRACE(2,"set active mode for user %d, link %d, now state:", user, linkIndex);
    DUMP32("%08x ", bt_link_active_mode_bits, MAX_ACTIVE_MODE_MANAGED_LINKS);
}

void app_bt_active_mode_clear(BT_LINK_ACTIVE_MODE_KEEPER_USER_E user, uint32_t linkIndex)
{
    bool isAlreadyAllowSniff = false;
    if (linkIndex < MAX_ACTIVE_MODE_MANAGED_LINKS)
    {
        uint32_t lock = int_lock_global();

        if (0 == bt_link_active_mode_bits[linkIndex])
        {
            isAlreadyAllowSniff = true;
        }
        else
        {
            isAlreadyAllowSniff = false;
        }

        bt_link_active_mode_bits[linkIndex] &= (~(1 << user));

        int_unlock_global(lock);

        if (!isAlreadyAllowSniff)
        {
            app_bt_allow_sniff(linkIndex);
        }
    }
    else if (MAX_ACTIVE_MODE_MANAGED_LINKS == linkIndex)
    {
        for (uint8_t devId = 0;devId < BT_DEVICE_NUM;devId++)
        {
            uint32_t lock = int_lock_global();
            if (0 == bt_link_active_mode_bits[devId])
            {
                isAlreadyAllowSniff = true;
            }
            else
            {
                isAlreadyAllowSniff = false;
            }
            bt_link_active_mode_bits[devId] &= (~(1 << user));
            int_unlock_global(lock);

            if (!isAlreadyAllowSniff)
            {
                app_bt_allow_sniff(devId);
            }
        }
    }

    TRACE(2,"clear active mode for user %d, link %d, now state:", user, linkIndex);
    DUMP32("%08x ", bt_link_active_mode_bits, MAX_ACTIVE_MODE_MANAGED_LINKS);
}

int8_t app_bt_get_rssi(void)
{
    int8_t rssi=127;
    uint8_t i;
    btif_remote_device_t *remDev = NULL;
	rx_agc_t tws_agc = {0};

    for (i=0; i<BT_DEVICE_NUM; i++){
        remDev = btif_me_enumerate_remote_devices(i);
        if (remDev)
        {
            if(btif_me_get_remote_device_hci_handle(remDev))
            {
                 rssi = bt_drv_read_rssi_in_dbm(btif_me_get_remote_device_hci_handle(remDev),&tws_agc);
                 rssi = bt_drv_rssi_correction(rssi);
                 TRACE(1," headset to mobile RSSI:%d dBm",rssi);
            }
        }
    }
    return rssi;
}

#ifdef TILE_DATAPATH 
int8_t app_tile_get_ble_rssi(void)
{
    int8_t rssi=127;
    uint8_t i;
    btif_remote_device_t *remDev = NULL;
	rx_agc_t tws_agc = {0};

    for (i=0; i<BT_DEVICE_NUM; i++){
        remDev = btif_me_enumerate_remote_devices(i);
        if (remDev)
        {
            if(app_tile_ble_get_connection_index() != BLE_INVALID_CONNECTION_INDEX)
            {
                 rssi = bt_drv_read_ble_rssi_in_dbm(app_tile_ble_get_connection_index(),&tws_agc);
                 rssi = bt_drv_rssi_correction(rssi);
                 TRACE(1," headset to mobile RSSI:%d dBm",rssi);
            }
        }
    }
    return rssi;
}
#endif

/** add by pang **/
void app_bt_Establish_SCL(void)
{
	uint8_t dev_idx = 0;

	struct app_bt_profile_manager *bt_profile_manager_p = NULL;

	if((!app_bt_is_connected())&&(BTIF_BAM_GENERAL_ACCESSIBLE == app_bt_get_current_access_mode()))
	{
		for(dev_idx = 0; dev_idx < BT_DEVICE_NUM; dev_idx ++)
		{
			bt_profile_manager_p = &bt_profile_manager[dev_idx];

			btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager_p->rmt_addr.address);
				
			if (btdevice_plf_p->hfp_act){
				TRACE(0,"try connect hf");
				app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
				app_bt_HF_CreateServiceLink(bt_profile_manager_p->chan, (bt_bdaddr_t *)&bt_profile_manager_p->rmt_addr);
			}
#if defined (__HSP_ENABLE__)
			else if(btdevice_plf_p->hsp_act){
				TRACE(0,"try connect hs");
				app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
				app_bt_HS_CreateServiceLink(bt_profile_manager_p->hs_chan, &bt_profile_manager_p->rmt_addr);
			}
#endif
			else if(btdevice_plf_p->a2dp_act){
				TRACE(0,"try connect a2dp");
				app_bt_precheck_before_starting_connecting(bt_profile_manager_p->has_connected);
				app_bt_A2DP_OpenStream(bt_profile_manager_p->stream, &bt_profile_manager_p->rmt_addr);
			}
		}

		lostconncection_to_pairing=1;
	}	
}

void app_bt_reconnect_idle_mode(void)
{
	bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_null;
	bt_profile_manager[BT_DEVICE_ID_1].reconnect_cnt = 0;
#ifdef __BT_ONE_BRING_TWO__	
	bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode = bt_profile_reconnect_null;
	bt_profile_manager[BT_DEVICE_ID_2].reconnect_cnt = 0;
#endif
}

void app_bt_off(void)
{	
	if(app_bt_is_connected()){
		lacal_bt_off=1;	
		app_audio_sendrequest(APP_BT_STREAM_INVALID, (uint8_t)APP_BT_SETTING_CLOSEALL, 0);
		osDelay(500);
		app_disconnect_all_bt_connections();
		app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
		app_status_indication_set(APP_STATUS_INDICATION_DISCONNECTED);
		//app_start_10_second_timer(APP_BTOFF_POWEROFF_TIMER_ID);
	}
	else{
		lacal_bt_off=0;	
		app_bt_profile_connect_manager_opening_reconnect();
	    //app_stop_10_second_timer(APP_BTOFF_POWEROFF_TIMER_ID);
	}
}

bool app_lacal_host_bt_off(void){
	return (lacal_bt_off);
}

osTimerId reconnect_timeout_timer = NULL;
static void reconnect_timeout_handler(void const *param);
osTimerDef(RECONNECT_TIMEOUT_TIMER, reconnect_timeout_handler);// define timers
uint8_t reconnect_type=0;
uint8_t reconnect_detect_num=0;
#define OPENRECONNECT_TIMEOUT_IN_MS	(60000)//(13000)//10s   m by cai
#define RECONNECT_TIMEOUT_IN_MS	(300000)//10000*15=120s

static void reconnect_timeout_set(uint8_t rect)
{
    if (reconnect_timeout_timer == NULL)
       reconnect_timeout_timer= osTimerCreate(osTimer(RECONNECT_TIMEOUT_TIMER), osTimerOnce, NULL);

    reconnect_type=rect;
#if 1	
	if(rect)
    	osTimerStart(reconnect_timeout_timer,RECONNECT_TIMEOUT_IN_MS);
	else
		osTimerStart(reconnect_timeout_timer,OPENRECONNECT_TIMEOUT_IN_MS);
#else
	if(!rect)
		osTimerStart(reconnect_timeout_timer,OPENRECONNECT_TIMEOUT_IN_MS);
#endif
}

static void reconnect_timeout_stop(void)
{
    if (reconnect_timeout_timer == NULL)
       return;
	
	reconnect_detect_num=0;
	osTimerStop(reconnect_timeout_timer);
}

static void reconnect_timeout_handler(void const *param)
{
#ifdef __BT_ONE_BRING_TWO__
    static bool oenreconnect_flag=0; 
	if(reconnect_type){
		reconnect_detect_num++;
		if(reconnect_detect_num>1){
		   #if 0
			if(bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode == bt_profile_reconnect_reconnecting){
				bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_null;
			}
			if(bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode == bt_profile_reconnect_reconnecting){
				bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode = bt_profile_reconnect_null;
			}
			app_bt_update_connectable_mode_after_connection_management();
	
			#else
			/*
			if((bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode == bt_profile_reconnect_reconnecting)||
				(bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode == bt_profile_reconnect_reconnecting)){
				bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_null;
				bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode = bt_profile_reconnect_null;
				TRACE(1,"%s",__func__);
				app_bt_update_connectable_mode_after_connection_management();
			}
			*/
			TRACE(1,"%s,%d %d %d %d",__func__,bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode,bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode,bt_profile_manager[0].reconnect_cnt,bt_profile_manager[1].reconnect_cnt);

			if(bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode == bt_profile_reconnect_reconnecting){
				if (bt_profile_manager[0].reconnect_cnt < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT){
					app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
			 		#ifdef __IAG_BLE_INCLUDE__
                	app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
             		#endif
					osTimerStart(bt_profile_manager[0].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
				}
			}
			else if(bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode == bt_profile_reconnect_reconnecting){
				if (bt_profile_manager[1].reconnect_cnt < APP_BT_PROFILE_OPENNING_RECONNECT_RETRY_LIMIT_CNT){
					app_bt_accessmode_set(BTIF_BAM_CONNECTABLE_ONLY);
			 		#ifdef __IAG_BLE_INCLUDE__
                	app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
             		#endif
					osTimerStart(bt_profile_manager[1].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
				}
			}
            #endif
		}
		else{
			reconnect_timeout_set(1);
		}
	}
	else{
		if(bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode == bt_profile_reconnect_openreconnecting){
			bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_null;
			oenreconnect_flag=1;
		}
		if(bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode == bt_profile_reconnect_openreconnecting){
			bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode = bt_profile_reconnect_null;
			oenreconnect_flag=1;
		}

    	if(oenreconnect_flag){
			app_bt_update_connectable_mode_after_connection_management();			
    	}	
	}
	//osTimerDelete(reconnect_timeout_timer);
#endif
}

void app_get_curr_remDev_Mac(unsigned char* mobile_addr)
{
    uint8_t num_of_connected_dev=0;
	num_of_connected_dev=app_bt_get_num_of_connected_dev();
#ifdef __BT_ONE_BRING_TWO__	
	app_audio_manager_a2dp_is_active(BT_DEVICE_ID_1);
	app_audio_manager_a2dp_is_active(BT_DEVICE_ID_2);
    if(num_of_connected_dev==0) 
		mobile_addr=0;
	else if(num_of_connected_dev==1)
		memcpy(mobile_addr,bt_profile_manager[0].rmt_addr.address, 6);
	else{
	    if(app_audio_manager_a2dp_is_active(BT_DEVICE_ID_1))
			memcpy(mobile_addr,bt_profile_manager[0].rmt_addr.address, 6);	
		else
			memcpy(mobile_addr,bt_profile_manager[1].rmt_addr.address, 6);
	}
#else
	if(num_of_connected_dev==0) 
		mobile_addr=0;
	else
		memcpy(mobile_addr,bt_profile_manager[0].rmt_addr.address, 6);
#endif
}

void app_multipoint_api_set_on(void)
{
	app_bt_update_connectable_mode_after_connection_management();
}


uint8_t app_device_EDR_connect_status(void)
{
    uint8_t conncect_status=0;

	if(0==nv_record_get_paired_dev_count())
		conncect_status=2;
	else{
		if(app_bt_is_connected())
			conncect_status=1;
		else
			conncect_status=0;
	}

	return conncect_status;
}

void app_enter_pairing(void)
{	
	LinkDisconnectDirectly(false);
	osDelay(800);
	
	app_bt_reconnect_idle_mode();
	app_bt_accessmode_set_req(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
}

void app_get_remote_dev_name(uint8_t* ptrName,uint16_t nameLen)
{
    nameLen = btif_me_get_callback_event_remote_dev_name((const btif_event_t *)BTIF_BTEVENT_NAME_RESULT, &ptrName);
    TRACE(1,"app_get_remote_dev_name name len %d", nameLen);
    if (nameLen > 0)
    {
       TRACE(1,"remote dev name: %s", ptrName);
    }
}
/** end add **/
