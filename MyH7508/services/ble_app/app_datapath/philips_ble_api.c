/***************************************************************************
 *
 * Copyright TPV.
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
#include "string.h"
#include "bluetooth.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "apps.h"
#include "stdbool.h"
#include "rwapp_config.h"
#include "philips_ble_api.h"
#include "app_datapath_server.h"
#include "app_ble_cmd_handler.h"
#include "app_ble_custom_cmd.h"
#include <stdio.h>

//Philips BLE
#include "../bt_if_enhanced/inc/avrcp_api.h"
#include "../bt_app/Btapp.h"
#include "app_spp_tota.h"

/** add by pang **/
#include "../../../apps/userapps/app_user.h"
#include "app_bt_stream.h"
#include "app.h"
#include "app_battery.h"
#include "../../../apps/anc/inc/app_anc.h"
#include "app_bt.h"
#include "hal_codec.h"//add by cai
#include "app_hfp.h"//add by cai

static uint8_t protocol_port=0;

void Philips_Api_protocol_port(uint8_t port)
{
	protocol_port=port;
}
/** end add **/


bool Philips_Headphone_Api_Entry(uint8_t *data, uint32_t size)
{	
   //Check Start head
   if (data[0] != (uint8_t) 0xff){
       TRACE(1,"Philips : fail! data[0]= %x\r\n ", data[0]);
	   return false;
   }

   //Check API Version
   if (data[1] !=  (uint8_t)0x01){
       TRACE(1,"Philips : fail! data[1]= %x\r\n ", data[1]);
	   return false;
   }

   //Check package Length
   if (data[2] != (uint8_t) size){
       TRACE(1,"Philips : fail! data[2]= %x\r\n ", data[2]);
	   return false;
   }
 
   //Check Vendor ID
   if (data[3] !=  (uint8_t)0x04){
   		TRACE(1,"Philips : fail! data[3]= %x\r\n ", data[3]);
		return false;
   } else{
	   if (data[4] !=  (uint8_t)0x71){
	        TRACE(1,"Philips : fail! data[4]= %x\r\n ", data[4]);
		    return false;
	   }
   }   

   //Check Command ID
   if (!CheckCommandID(data) ){
		TRACE(0,"Philips : Check Command ID Fail!\r\n");
		return false;
   }

   //Package Checksun
   if (!Command_CheckSum(data,(uint8_t) size) ){
		TRACE(0,"Philips : Checksun Fail!\r\n");
		return false;
   } 

   //Philips functions call
   if (!Philips_Functions_Call(data,(uint8_t) size) ){
		TRACE(0,"Philips : Philips_Functions_Call Fail!\r\n");
		return false;
   }

   TRACE(0,"Test_Philips_API OK");
   return true;
}

bool CheckCommandID(uint8_t *data)
{
	uint16_t command_id = ((uint16_t)data[5] << 8) | ((uint16_t)data[6]);
	   
	switch(command_id)
	{
		case GET_CONNECT_PHONE_MAC:
			//TRACE(0,"Philips : GET_CONNECT_PHONE_MAC!\r\n");
			return true;
		case GET_DEVICE_FEATURE_CONFIG:		
			//TRACE(0,"Philips : GET_DEVICE_FEATURE_CONFIG!\r\n");
			return true;
		case GET_API_VERSION:
			//TRACE(0,"Philips : GET_API_VERSION!\r\n");
			return true;	
		case GET_SOUND_QUALITY:
			//TRACE(0,"Philips : GET_SOUND_QUALITY!\r\n");
			return true;	
		case GET_RIGHT_EAR_MAC_ADDRESS:
			//TRACE(0,"Philips : GET_RIGHT_EAR_MAC_ADDRESS!\r\n");
			return true;
		case GET_LEFT_EAR_MAC_ADDRESS:
			//TRACE(0,"Philips : GET_LEFT_EAR_MAC_ADDRESS!\r\n");
			return true;		
		case NOTIFICATION_SOUND_QUALITY_CHANGE:
			//TRACE(0,"Philips : NOTIFICATION_SOUND_QUALITY_CHANGE!\r\n");
			return true;		
		case GET_BLE_MAC_ADDRESS:
			//TRACE(0,"Philips : GET_BLE_MAC_ADDRESS!\r\n");
			return true;			
		case GET_SALES_REGION:
			//TRACE(0,"Philips : GET_SALES_REGION!\r\n");
			return true;					
		case GET_CHIPSET_VENDOR:		
			//TRACE(0,"Philips : GET_CHIPSET_VENDOR!\r\n");
			return true;
		case GET_CHIPSET_SOLUTION:		
			//TRACE(0,"Philips : GET_CHIPSET_SOLUTION!\r\n");
			return true;
		case GET_FW_VERSION:		
			//TRACE(0,"Philips : GET_FW_VERSION!\r\n");
			return true;	
		case GET_DEVICE_TYPE:		
			//TRACE(0,"Philips : GET_DEVICE_TYPE!\r\n");
			return true;	
		case GET_FOTA_FINISH_FLAG:		
			//TRACE(0,"Philips : GET_FOTA_FINISH_FLAG!\r\n");
			return true;
		case SET_FOTA_FINISH_FLAG:		
			//TRACE(0,"Philips : SET_FOTA_FINISH_FLAG!\r\n");
			return true;		
		case GET_PCBA_VERSION:		
			//TRACE(0,"Philips : GET_PCBA_VERSION!\r\n");
			return true;		
			
		case GET_ANC_MODE_SUPPORT_LIST:		
			//TRACE(0,"Philips : GET_ANC_MODE_SUPPORT_LIST!\r\n");
			return true;
		case GET_ANC_MODE_STATUS:		
			//TRACE(0,"Philips : GET_ANC_MODE_STATUS!\r\n");
			return true;
		case SET_ANC_MODE_VALUE:		
			//TRACE(0,"Philips : SET_ANC_MODE_VALUE!\r\n");
			return true;	
		case GET_AWARENESS_VALUE:		
			//TRACE(0,"Philips : GET_AWARENESS_VALUE!\r\n");
			return true;
		case SET_AWARENESS_VALUE:		
			//TRACE(0,"Philips : SET_AWARENESS_VALUE!\r\n");
			return true;
		case GET_ENHANCE_VOICE_STATUS:		
			//TRACE(0,"Philips : GET_ENHANCE_VOICE_STATUS!\r\n");
			return true;
		case SET_ENHANCE_VOICE_STATUS:		
			//TRACE(0,"Philips : GET_ANC_TABLE_VALUE!\r\n");
			return true;
		case GET_ANC_TABLE_VALUE:		
			//TRACE(0,"Philips : SET_ENHANCE_VOICE_STATUS!\r\n");
			return true;
/*
		case GET_AMBIENT_SOUND_ITEM_STATUS:		
			//TRACE(0,"Philips : GET_AMBIENT_SOUND_ITEM_STATUS!\r\n");
			return true;	
		case SET_AMBIENT_SOUND_ITEM_VALUE:		
			//TRACE(0,"Philips : SET_AMBIENT_SOUND_ITEM_VALUE!\r\n");
			return true;	
		case GET_AMBIENT_RANGE_SUPPORT:		
			//TRACE(0,"Philips : GET_AMBIENT_RANGE_SUPPORT!\r\n");
			return true;	
		case GET_AMBIENT_RANGE_TOTAL:		
			//TRACE(0,"Philips : GET_AMBIENT_RANGE_TOTAL!\r\n");
			return true;	
		case GET_AMBIENT_FOCUS_ON_VOICE_SUPPORT:		
			//TRACE(0,"Philips : GET_AMBIENT_FOCUS_ON_VOICE_SUPPORT!\r\n");
			return true;	
		case GET_ADAPTIVE_STAND_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_STAND_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_WALKING_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_WALKING_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_RUNNING_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_RUNNING_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_TRAFFIC_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_TRAFFIC_D4_VALUE!\r\n");
			return true;	
		case GET_AMBIENT_D4_VALUE:		
			//TRACE(0,"Philips : GET_AMBIENT_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_STAND_FOCUS_ON_VOICE_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_STAND_FOCUS_ON_VOICE_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_WALKING_FOCUS_ON_VOICE_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_WALKING_FOCUS_ON_VOICE_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_RUNNING_FOCUS_ON_VOICE_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_RUNNING_FOCUS_ON_VOICE_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_TRAFFIC_FOCUS_ON_VOICE_D4_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_TRAFFIC_FOCUS_ON_VOICE_D4_VALUE!\r\n");
			return true;	
		case GET_AMBIENT_FOCUS_ON_VOICE_D4_VALUE:		
			//TRACE(0,"Philips : GET_AMBIENT_FOCUS_ON_VOICE_D4_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_STAND_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_STAND_CURRENT_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_STAND_CURRENT_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_STAND_CURRENT_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_STAND_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_STAND_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_STAND_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_STAND_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_WALKING_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_WALKING_CURRENT_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_WALKING_CURRENT_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_WALKING_CURRENT_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_WALKING_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_WALKING_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_WALKING_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_WALKING_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_RUNNING_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_RUNNING_CURRENT_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_RUNNING_CURRENT_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_RUNNING_CURRENT_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_RUNNING_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_RUNNING_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_RUNNING_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_RUNNING_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case GET_ADAPTIVE_TRAFFIC_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_TRAFFIC_CURRENT_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_TRAFFIC_CURRENT_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_TRAFFIC_CURRENT_VALUE!\r\n");
			return true;			
		case GET_ADAPTIVE_TRAFFIC_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : GET_ADAPTIVE_TRAFFIC_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case SET_ADAPTIVE_TRAFFIC_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : SET_ADAPTIVE_TRAFFIC_FOCUS_ON_VOICE_VALUE!\r\n");
			return true;	
		case GET_AMBIENT_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_AMBIENT_CURRENT_VALUE!\r\n");
			return true;	
		case SET_AMBIENT_CURRENT_VALUE:		
			//TRACE(0,"Philips : SET_AMBIENT_CURRENT_VALUE!\r\n");
			return true;	
		case GET_AMBIENT_FOCUS_ON_VOICE_STATUS:		
			//TRACE(0,"Philips : GET_AMBIENT_FOCUS_ON_VOICE_STATUS!\r\n");
			return true;	
		case SET_AMBIENT_FOCUS_ON_VOICE_VALUE:		
			//TRACE(0,"Philips : GET_AMBIENT_FOCUS_ON_VOICE_STATUS!\r\n");
			return true;
*/
/*
		case GET_HW_NOSIE_CANCELLING_STATUS:		
			//TRACE(0,"Philips : GET_HW_NOSIE_CANCELLING_STATUS!\r\n");
			return true;
		case SET_HW_ANC_VALUE:		
			//TRACE(0,"Philips : SET_HW_ANC_VALUE!\r\n");
			return true;
		case GET_HW_AMBIENT_VALUE:		
			//TRACE(0,"Philips : GET_HW_AMBIENT_VALUE!\r\n");
			return true;			
		case SET_HW_AMBIENT_VALUE:		
			//TRACE(0,"Philips : SET_HW_AMBIENT_VALUE!\r\n");
			return true;	
		case GET_HW_FOCUS_ON_VOICE_STATUS:		
			//TRACE(0,"Philips : GET_HW_FOCUS_ON_VOICE_STATUS!\r\n");
			return true;	
		case SET_HW_FOCUS_ON_VOICE_STATUS:		
			//TRACE(0,"Philips : SET_HW_FOCUS_ON_VOICE_STATUS!\r\n");
			return true;
*/
		case NOTIFICATIION_NOSIE_CANCELLING_CHANGE:		
			//TRACE(0,"Philips : NOTIFICATIION_NOSIE_CANCELLING_CHANGE!\r\n");
			return true;					
		case GET_EQUALIZER_SUPPORT_LIST:		
			//TRACE(0,"Philips : GET_EQUALIZER_SUPPORT_LIST!\r\n");
			return true;	
		case GET_EQUALIZER_STATUS:		
			//TRACE(0,"Philips : GET_EQUALIZER_STATUS!\r\n");
			return true;	
		case SET_EQUALIZER_STATUS_VALUE:		
			//TRACE(0,"Philips : SET_EQUALIZER_STATUS_VALUE!\r\n");
			return true;	
		case GET_EQUALIZER_STATUS_D4_VALUE:
			//TRACE(0,"Philips : GET_EQUALIZER_STATUS_D4_VALUE!\r\n");
			return true;		
		case GET_LAST_EQ_STATUS:
			//TRACE(0,"Philips : GET_LAST_EQ_STATUS!\r\n");
			return true;
		case NOTIFICATIION_EQ_CHANGE:
			//TRACE(0,"Philips : NOTIFICATIION_EQ_CHANGE!\r\n");
			return true;			
		case GET_CUSTOMIZATION_EQ_SUPPORT_LIST:		
			//TRACE(0,"Philips : GET_CUSTOMIZATION_EQ_SUPPORT_LIST!\r\n");
			return true;	
		case GET_CUSTOMIZATION_EQ_BAND_VALUE:		
			//TRACE(0,"Philips : GET_CUSTOMIZATION_EQ_BAND_STATUS!\r\n");
			return true;	
		case SET_CUSTOMIZATION_EQ_BAND_VALUE:		
			//TRACE(0,"Philips : SET_CUSTOMIZATION_EQ_BAND_VALUE!\r\n");
			return true;	
		case GET_CUSTOMIZATION_EQ_BAND_RANGE_VALUE:		
			//TRACE(0,"Philips : GET_CUSTOMIZATION_EQ_BAND_RANGE_VALUE!\r\n");
			return true;	
		case GET_CUSTOMIZATION_EQ_WITH_LIB:		
			//TRACE(0,"Philips : GET_CUSTOMIZATION_EQ_WITH_LIB!\r\n");
			return true;	
		case GET_NOWPLAYING_PLAYBACK_STATUS:		
			//TRACE(0,"Philips : GET_NOWPLAYING_PLAYBACK_STATUS!\r\n");
			return true;		
		case SET_NOWPLAYING_PLAYBACK_STATUS_VALUE:		
			//TRACE(0,"Philips : SET_NOWPLAYING_PLAYBACK_STATUS_VALUE!\r\n");
			return true;	
		case SET_NOWPLAYING_PLAYBACK_NEXT:		
			//TRACE(0,"Philips : SET_NOWPLAYING_PLAYBACK_NEXT!\r\n");
			return true;	
		case SET_NOWPLAYING_PLAYBACK_PREVIOUS:		
			//TRACE(0,"Philips : SET_NOWPLAYING_PLAYBACK_PREVIOUS!\r\n");
			return true;	
		case NOTIFICATION_MEDIA_CHANGE:		
			//TRACE(0,"Philips : NOTIFICATION_MEDIA_CHANGE!\r\n");
			return true;				
		case GET_MEDIA_TITLE:		
			//TRACE(0,"Philips : GET_MEDIA_TITLE!\r\n");
			return true;	
		case GET_MEDIA_ARTIST:		
			//TRACE(0,"Philips : GET_MEDIA_ARTIST!\r\n");
			return true;	
		case GET_MEDIA_ALBUM:		
			//TRACE(0,"Philips : GET_MEDIA_ALBUM!\r\n");
			return true;			
		case GET_MEDIA_FUNTION_SUPPORT:		
			//TRACE(0,"Philips : GET_MEDIA_FUNTION_SUPPORT!\r\n");
			return true;		
/*						
		case GET_HEART_RATE_STATUS:		
			//TRACE(0,"Philips : GET_HEART_RATE_STATUS!\r\n");
			return true;	
		case SET_HEART_RATE_STATUS_VALUE:		
			//TRACE(0,"Philips : SET_HEART_RATE_STATUS_VALUE!\r\n");
			return true;	
		case GET_HEART_RATE_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_HEART_RATE_CURRENT_VALUE!\r\n");
			return true;		
		case GET_HEART_RATE_AUTO_NODIFY_STATUS:		
			//TRACE(0,"Philips : GET_HEART_RATE_AUTO_NODIFY_STATUS!\r\n");
			return true;	
		case SET_HEART_RATE_AUTO_NODIFY_VALUE:		
			//TRACE(0,"Philips : SET_HEART_RATE_AUTO_NODIFY_VALUE!\r\n");
			return true;		
*/	
/*
		case GET_EAR_DETECTION_STATUS:		
			//TRACE(0,"Philips : GET_EAR_DETECTION_STATUS!\r\n");
			return true;	
		case SET_EAR_DETECTION_STATUS_VALUE:		
			//TRACE(0,"Philips : SET_EAR_DETECTION_STATUS_VALUE!\r\n");
			return true;	
		case GET_EAR_DETECTION_CURRENT_VALUE:		
			//TRACE(0,"Philips : GET_EAR_DETECTION_CURRENT_VALUE!\r\n");
			return true;		
		case NOTIFICATION_EAR_DETECTION_CHANGE:		
			//TRACE(0,"Philips : NOTIFICATION_EAR_DETECTION_CHANGE!\r\n");
			return true;
*/
/*
		case P_SENSOR_CALIBRATION:		
			//TRACE(0,"Philips : P_SENSOR_CALIBRATION!\r\n");
			return true;	
		case P_SENSOR_RESET:		
			//TRACE(0,"Philips : P_SENSOR_RESET!\r\n");
			return true;	
		case SET_PAUSES_MUSIC_WHEN_TAKEOFF:		
			//TRACE(0,"Philips : SET_PAUSES_MUSIC_WHEN_TAKEOFF!\r\n");
			return true;		
		case GET_PAUSES_MUSIC_WHEN_TAKEOFF:		
			//TRACE(0,"Philips : GET_PAUSES_MUSIC_WHEN_TAKEOFF!\r\n");
			return true;
*/
		case GET_BATTERY_CHARGE_STATUS:		
			//TRACE(0,"Philips : GET_BATTERY_CHARGE_STATUS!\r\n");
			return true;	
		case GET_RIGHT_EAR_BATTERY_LEVEL_VALUE:		
			//TRACE(0,"Philips : GET_RIGHT_EAR_BATTERY_LEVEL_VALUE!\r\n");
			return true;	
		case GET_LEFT_EAR_BATTERY_LEVEL_VALUE:		
			//TRACE(0,"Philips : GET_LEFT_EAR_BATTERY_LEVEL_VALUE!\r\n");
			return true;	
		case GET_CHARGE_BOX_BATTERY_LEVEL_VALUE:		
			//TRACE(0,"Philips : GET_CHARGE_BOX_BATTERY_LEVEL_VALUE!\r\n");
			return true;	
		case NOTIFICATION_BATTERY_LEVEL_CHANGE:		
			//TRACE(0,"Philips : NOTIFICATION_BATTERY_LEVEL_CHANGE!\r\n");
			return true;	
		case GET_BATTERY_NOTIFY_STATUS:		
			//TRACE(0,"Philips : GET_BATTERY_NOTIFY_STATUS!\r\n");
			return true;				
		case SET_BATTERY_NOTIFY_STATUS:		
			//TRACE(0,"Philips : SET_BATTERY_NOTIFY_STATUS!\r\n");
			return true;	
		case NOTIFICATION_BATTERY_CHARGE_CHANGE:		
			//TRACE(0,"Philips : NOTIFICATION_BATTERY_CHARGE_CHANGE!\r\n");
			return true;				
		case SET_SLEEP_MODE_TIMER:		
			//TRACE(0,"Philips : SET_SLEEP_MODE_TIMER!\r\n");
			return true;				
		case GET_SLEEP_MODE_TIMER:		
			//TRACE(0,"Philips : GET_SLEEP_MODE_TIMER!\r\n");
			return true;		
		case GET_SMART_CHARGEBOX_SUPPORT:		
			//TRACE(0,"Philips : GET_SMART_CHARGEBOX_SUPPORT!\r\n");
			return true;	
		case GET_AUTO_POWER_OFF_SUPPORT:		
			//TRACE(0,"Philips : GET_AUTO_POWER_OFF_SUPPORT!\r\n");
			return true;	
		case GET_AUTO_POWER_OFF_TIMER:		
			//TRACE(0,"Philips : GET_AUTO_POWER_OFF_TIMER!\r\n");
			return true;
		case SET_AUTO_POWER_OFF_TIMER:		
			//TRACE(0,"Philips : SET_AUTO_POWER_OFF_TIMER!\r\n");
			return true;					
		/*		
		case GET_MULTI_LANGUAGE_VOICE_PROMPT_SUPPORT_LIST:		
			//TRACE(0,"Philips : GET_MULTI_LANGUAGE_VOICE_PROMPT_SUPPORT_LIST!\r\n");
			return true;		
		case GET_MULTI_LANGUAGE_VOICE_PROMPT_STATUS:		
			//TRACE(0,"Philips : GET_MULTI_LANGUAGE_VOICE_PROMPT_STATUS!\r\n");
			return true;	
		case SET_MULTI_LANGUAGE_VOICE_PROMPT_STATUS_VALUE:		
			//TRACE(0,"Philips : SET_MULTI_LANGUAGE_VOICE_PROMPT_STATUS_VALUE!\r\n");
			return true;
		case GET_MULTI_LANGUAGE_VOICE_PROMPT_D4_VALUE:		
			//TRACE(0,"Philips : GET_MULTI_LANGUAGE_VOICE_PROMPT_D4_VALUE!\r\n");
			return true;

		case GET_VOICE_ASSISTANT_SUPPORT_LIST:		
			//TRACE(0,"Philips : GET_VOICE_ASSISTANT_SUPPORT_LIST!\r\n");
			return true;
		case GET_VOICE_WAKEUP_SUPPORT_LIST:		
			//TRACE(0,"Philips : GET_VOICE_WAKEUP_SUPPORT_LIST!\r\n");
			return true;
		case GET_VOICE_ASSISTANT_STATUS:		
			//TRACE(0,"Philips : GET_VOICE_ASSISTANT_STATUS!\r\n");
			return true;
		case SET_VOICE_ASSISTANT_STATUS:		
			//TRACE(0,"Philips : SET_VOICE_ASSISTANT_STATUS!\r\n");
			return true;
		case GET_PUSH_AND_TALK_STATUS:		
			//TRACE(0,"Philips : GET_PUSH_AND_TALK_STATUS!\r\n");
			return true;
		case SET_PUSH_AND_TALK_STATUS:		
			//TRACE(0,"Philips : SET_PUSH_AND_TALK_STATUS!\r\n");
			return true;
		case GET_VOICE_WAKEUP_STATUS:		
			//TRACE(0,"Philips : GET_VOICE_WAKEUP_STATUS!\r\n");
			return true;
		case SET_VOICE_WAKEUP_STATUS:		
			//TRACE(0,"Philips : SET_VOICE_WAKEUP_STATUS!\r\n");
			return true;	
*/				
		case GET_SPECIAL_FUNCTION1_SUPPORT_LIST:
			//TRACE(0,"Philips : GET_SPECIAL_FUNCTION1_SUPPORT_LIST!\r\n");
		      return true;	
		case GET_TOUCH_STATUS:		
			//TRACE(0,"Philips : GET_TOUCH_STATUS!\r\n");
			return true;				
		case SET_TOUCH_STATUS:		
			//TRACE(0,"Philips : SET_TOUCH_STATUS!\r\n");
			return true;
/*
		case GET_RUNNING_LIGHT_STATUS:		
			//TRACE(0,"Philips : GET_RUNNING_LIGHT_STATUS!\r\n");
			return true;				
		case SET_RUNNING_LIGHT_STATUS:		
			//TRACE(0,"Philips : SET_RUNNING_LIGHT_STATUS!\r\n");
			return true;
*/
		case GET_SIDE_TONE_CONTROL_STATUS:		
			//TRACE(0,"Philips : GET_SIDE_TONE_CONTROL_STATUS!\r\n");
			return true;				
		case SET_SIDE_TONE_CONTROL_STATUS:		
			//TRACE(0,"Philips : SET_SIDE_TONE_CONTROL_STATUS!\r\n");
			return true;

		case GET_LOW_LATENCY_STATUS:		
			//TRACE(0,"Philips : GET_LOW_LATENCY_STATUS!\r\n");
			return true;				
		case SET_LOW_LATENCY_STATUS:		
			//TRACE(0,"Philips : SET_LOW_LATENCY_STATUS!\r\n");
			return true;
/*
		case GET_VIBRATION_STATUS:		
			//TRACE(0,"Philips : GET_VIBRATION_STATUS!\r\n");
			return true;				
		case SET_VIBRATION_STATUS:		
			//TRACE(0,"Philips : SET_VIBRATION_STATUS!\r\n");
			return true;
*/		
		case GET_MULTIPOINT_STATUS:		
			//TRACE(0,"Philips : GET_MULTIPOINT_STATUS!\r\n");
			return true;				
		case SET_MULTIPOINT_STATUS:		
			//TRACE(0,"Philips : SET_MULTIPOINT_STATUS!\r\n");
			return true;

		case GET_DEVICE_COLOUR_STATUS:		
			//TRACE(0,"Philips : GET_DEVICE_COLOUR_STATUS!\r\n");
			return true;
		//add by cai
		case SET_DEVICE_COLOUR_STATUS:		
			//TRACE(0,"Philips : SET_DEVICE_COLOUR_STATUS!\r\n");
			return true;

		case GET_KEY_DEFINE_SUPPORT_LIST:
			//TRACE(0,"Philips : GET_KEY_DEFINE_SUPPORT_LIST!\r\n");
			return true;

		case GET_UX_ANC_TOGGLE_STATUS:
			//TRACE(0,"Philips : GET_UX_ANC_TOGGLE_STATUS!\r\n");
			return true;

		case SET_UX_ANC_TOGGLE_STATUS:
			//TRACE(0,"Philips : SET_UX_ANC_TOGGLE_STATUS!\r\n");
			return true;

		case GET_NOWPLAYING2_SUPPORT_LIST:
			//TRACE(0,"Philips : GET_NOWPLAYING2_SUPPORT_LIST!\r\n");
			return true;

		case GET_NOWPLAYING2_PLAYBACK_STATUS:
			//TRACE(0,"Philips : GET_NOWPLAYING2_PLAYBACK_STATUS!\r\n");
			return true;

		case SET_NOWPLAYING2_PLAYBACK_STATUS:
			//TRACE(0,"Philips : SET_NOWPLAYING2_PLAYBACK_STATUS!\r\n");
			return true;

		case SET_NOWPLAYING2_PLAYBACK_NEXT:
			//TRACE(0,"Philips : SET_NOWPLAYING2_PLAYBACK_NEXT!\r\n");
			return true;

		case SET_NOWPLAYING2_PLAYBACK_PREVIOUS:
			//TRACE(0,"Philips : SET_NOWPLAYING2_PLAYBACK_PREVIOUS!\r\n");
			return true;

		case GET_NOWPLAYING2_MEDIA_TITLE:
			//TRACE(0,"Philips : GET_NOWPLAYING2_MEDIA_TITLE!\r\n");
			return true;

		case GET_NOWPLAYING2_MEDIA_ARTIST:
			//TRACE(0,"Philips : GET_NOWPLAYING2_MEDIA_ARTIST!\r\n");
			return true;

		case GET_NOWPLAYING2_MEDIA_ALBUM:
			//TRACE(0,"Philips : GET_NOWPLAYING2_MEDIA_ALBUM!\r\n");
			return true;
		//end add
		
		case GET_SPECIAL_FUNCTION2_SUPPORT_LIST:
		case GET_TALK_MIC_LED_STATUS:					
		case SET_TALK_MIC_LED_STATUS:					
			return true;

		default:
			TRACE(0,"Philips : Command error!\r\n");
      break;
	}
	
	return false;
}

bool Command_CheckSum(uint8_t *data, uint8_t size)
{
	uint8_t i = 0;
	uint8_t checksum = data[0];
  
	for ( i=1; i < (size - 1); i++){
		checksum ^= data[i];
	}

	if (data[size -1 ] == checksum){
		return true;
	}
	
	return false;
}

uint8_t Do_CheckSum(uint8_t *data, uint8_t size)
{
	uint8_t i = 0;
	uint8_t checksum = data[0];
	  
	for ( i=1; i < (size - 1); i++){
		checksum ^= data[i];
	}
	
	return checksum;
}

void Philips_Send_Notify(uint8_t *data, uint32_t size)
{
    //app_control_app_server_send_data_via_notification(data,size);
    if(protocol_port==1)
		app_datapath_server_send_data_via_notification(data,size);
	else if(protocol_port==2)
		app_tota_send_data_via_spp(data, size);
}

void Get_Connect_Phone_Mac(void)
{
    uint8_t phoneAddr[6] ={0};
    app_get_curr_remDev_Mac(phoneAddr);
	
    uint8_t valueLen = 14, i =0;
    uint8_t data[14] = {0xff,0x01,0x00,0x04,0x71,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	//Data length
	data[2] = 0x0e;
	//Phone Mac address 6 byte
	for (i = 0 ; i < 6 ; i++){
	  data[7 + i ] = phoneAddr[i];
	} 	
	//Do checksum
	data[valueLen - 1]=Do_CheckSum(data,valueLen);	   

	Philips_Send_Notify(data, (uint32_t)valueLen);
}

void Get_Device_Feature_Config(void)
{
	uint8_t device_feature[3] = {0};
	device_feature[0] = FOTA_SUPPORT | ANC_MODE_SUPPORT | EQUALIZER_SUPPORT | CUSTOMIZATION_EQ_SUPPORT; //| NOWPLAYING_SUPPORT; //| EAR_DETECTION_SUPPORT;//  | HEART_RATE_SUPPORT;
    device_feature[1] = BATTERY_STATUS_SUPPORT  | SPECIAL_FUNCTION1_SUPPORT; //|SPECIAL_FUNCTION2_SUPPORT;//| MULTI_LANGAUAGE_VOICE_PROMPT_SUPPORT
    device_feature[2] = NOWPLAYING2_0_SUPPORT | KEY_DEFINE_CHANGE_SUPPORT;
    
    uint8_t valueLen = 11;
    uint8_t data[11] = {0xff,0x01,0x00,0x04,0x71,0x80,0x01,0x00,0x00,0x00,0x00};
	
	//Data length
	data[2] = 0x0b;
	//Device features list 2 byte
	data[7] = device_feature[0];
	data[8] = device_feature[1];
	data[9] = device_feature[2];	
	//Do checksum
	data[valueLen - 1]=Do_CheckSum(data,valueLen);

    Philips_Send_Notify(data, (uint32_t)valueLen);
}

unsigned char Version[] = "API V5.0";
void Get_Api_Version()
{
	uint8_t valueLen = 0, i =0;
	uint8_t head[7] = {0xff,0x01,0x00,0x04,0x71,0x80,0x02};
	uint8_t charsize = sizeof(Version);

	//TRACE(1,"charsize %d : ", charsize);
	valueLen = 8 + charsize;

	uint8_t version[valueLen];	 

	for (i = 0 ; i < 7 ; i++){
		version[i] = head[i];
	}

	//Data length
	version[2] = valueLen;

	for (i = 7 ; i < (7 + charsize) ; i++){
		version[i] = (uint8_t) Version[i - 7];
	}    
	//Do checksum
	version[valueLen - 1]=Do_CheckSum(version,valueLen); 

	Philips_Send_Notify(version, (uint32_t)valueLen);
}

void Get_Sound_Quality(void)
{
	uint8_t sound_quality[]= {0x00}; //0 is SBC ,1 is AAC
	uint8_t valueLen = 0;
	uint8_t head[9] = {0xff,0x01,0x00,0x04,0x71,0x80,0x03,0x00,0x00};

	if(0x02==bt_sbc_player_get_codec_type())
		sound_quality[0]=0x01;
	else
		sound_quality[0]=0x00;

	TRACE(2,"***%s: sound_quality=%d",__func__,sound_quality[0]);

	valueLen = 9;

	//Data length
	head[2] = 0x09;
	//sound quality Item status 1 byte  
	head[7] = sound_quality[0];
	//Do checksum
	head[valueLen - 1]=Do_CheckSum(head,valueLen);   
	
    Philips_Send_Notify(head, (uint32_t)valueLen);	
}

void Get_Right_Ear_MAC_Address(void)
{
	uint8_t MacAddr[6] ={0};
	uint8_t valueLen = 0, i = 0;
	uint8_t data[14] = {0xff,0x01,0x00,0x04,0x71,0x80,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	memcpy(MacAddr, bt_addr, 6);
	
    valueLen = 14;
    
	//Data length
	data[2] = 0x0e;
	//Mac address 6 byte
	for (i = 0 ; i < 6 ; i++){
		data[7 + i ] = MacAddr[i];  //Need fill it.
	} 	

	//Do checksum
	data[valueLen - 1]=Do_CheckSum(data,valueLen);   
	 
    Philips_Send_Notify(data, (uint32_t)valueLen);	
}

void Get_Left_Ear_MAC_Address(void)
{
	uint8_t MacAddr[6] ={0};
	uint8_t valueLen = 0, i = 0;
	uint8_t data[14] = {0xff,0x01,0x00,0x04,0x71,0x80,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	memcpy(MacAddr, bt_addr, 6);
	
    valueLen = 14;
    
	//Data length
	data[2] = 0x0e;
	//Mac address 6 byte
	for (i = 0 ; i < 6 ; i++){
		data[7 + i ] = MacAddr[i];  //Need fill it.
	} 	

	//Do checksum
	data[valueLen - 1]=Do_CheckSum(data,valueLen);   
	 
    Philips_Send_Notify(data, (uint32_t)valueLen);	
}

void Notification_Sound_Quality_Change(void)
{
	TRACE(0,"********%s",__func__);

    uint8_t valueLen = 9;
    uint8_t head[9] = {0xff,0x01,0x00,0x04,0x71,0x80,0x06,0x00,0x00};
	
	//Data length
	head[2] = 0x09;
	//Notification_EQ_Change 1 byte  
	head[7] = 0x01; 
	//Do checksum
	head[valueLen - 1]=Do_CheckSum(head,valueLen);
   
    Philips_Send_Notify(head, (uint32_t)valueLen);    
}

void Get_BLE_MAC_Address(void)
{
	uint8_t MacAddr[6] ={3};
	uint8_t valueLen = 14, i =0;
	uint8_t data[14] = {0xff,0x01,0x00,0x04,0x71,0x80,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	memcpy(MacAddr, ble_addr, 6);


	//Data length
	data[2] = 0x0e;
	//Mac address 6 byte
	for (i = 0 ; i < 6 ; i++){
		data[7 + i ] = MacAddr[i];  //Need fill it.
	} 	

	//Do checksum
	data[valueLen - 1]=Do_CheckSum(data,valueLen);
	
	Philips_Send_Notify(data, (uint32_t)valueLen);
}

void Get_Sales_Region(void)
{
	uint8_t sales_region[]= {0x01}; //00 is Global region , /93 is China
	uint8_t valueLen = 9;
	uint8_t head[9] = {0xff,0x01,0x00,0x04,0x71,0x80,0x08,0x00,0x00};

	//Data length
	head[2] = 0x09;
	//sound quality Item status 1 byte  
	head[7] = sales_region[0];
	//Do checksum
	head[valueLen - 1]=Do_CheckSum(head,valueLen);

	Philips_Send_Notify(head, (uint32_t)valueLen);	
}

unsigned char Vendor[] = "BES";
void Get_Chipset_Vendor(void)
{
	uint8_t i =0;
	uint8_t head[7] = {0xff,0x01,0x00,0x04,0x71,0x80,0x10};
	uint8_t charsize = sizeof(Vendor);
	uint8_t valueLen = 0;

	//TRACE(1,"charsize %d : ", charsize);

	valueLen = 8 + charsize;

	uint8_t vendor[valueLen];	 

	for (i = 0 ; i < 7 ; i++){
		vendor[i] = head[i];
	}

	//Data length
	vendor[2] = valueLen;

	for (i = 7 ; i < (7 + charsize) ; i++){
		vendor[i] = (uint8_t) Vendor[i - 7];
	}    
	//Do checksum
	vendor[valueLen - 1]=Do_CheckSum(vendor,valueLen);

	Philips_Send_Notify(vendor, (uint32_t)valueLen);
}

unsigned char Solution[] = "BES2500HP";
void Get_Chipset_Solution(void)
{
	uint8_t i =0;
	uint8_t head[7] = {0xff,0x01,0x00,0x04,0x71,0x80,0x11};
	uint8_t charsize = sizeof(Solution);
	uint8_t valueLen = 0;

	//TRACE(1,"charsize %d : ", charsize);

	valueLen = 8 + charsize;

	uint8_t solution[valueLen];	 

	for (i = 0 ; i < 7 ; i++){
		solution[i] = head[i];
	}

	//Data length
	solution[2] = valueLen;

	for (i = 7 ; i < (7 + charsize) ; i++){
		solution[i] = (uint8_t) Solution[i - 7];
	}    
	//Do checksum
	solution[valueLen - 1]=Do_CheckSum(solution,valueLen);
	
	Philips_Send_Notify(solution, (uint32_t)valueLen);
}

unsigned char Chipset_Version[] = "V0.2.0.0";
void Get_Chipset_Version(void)
{	
	uint8_t i =0;
	uint8_t head[7] = {0xff,0x01,0x00,0x04,0x71,0x80,0x12};
	uint8_t valueLen = 0;
	uint8_t charsize = sizeof(Chipset_Version);

	//TRACE(1,"charsize %d : ", charsize);
	
	valueLen = 8 + charsize;

	uint8_t version[valueLen];	 

	for (i = 0 ; i < 7 ; i++){
		version[i] = head[i];
	}

	//Data length
	version[2] = valueLen;

	for (i = 7 ; i < (7 + charsize) ; i++){
		version[i] = (uint8_t) Chipset_Version[i - 7];
	}    

	//Do checksum
	version[valueLen - 1]=Do_CheckSum(version,valueLen);
	
	Philips_Send_Notify(version, (uint32_t)valueLen);
}

void Get_Device_Type(void)
{
	uint8_t device_type[]= {0x00}; //0 is Headset 1 is TWS
	uint8_t valueLen = 9;
	uint8_t head[9] = {0xff,0x01,0x00,0x04,0x71,0x80,0x13,0x00,0x00};
	
	//Data length
	head[2] = 0x09;
	//device type Item status 1 byte  
	head[7] = device_type[0];
	//Do checksum
	head[valueLen - 1]=Do_CheckSum(head,valueLen);

	Philips_Send_Notify(head, (uint32_t)valueLen);
}

static uint8_t g_FOTA_Flage[]= {0x00};
void Get_FOTA_Finish_Flag(void)
{
	uint8_t valueLen = 9;
	uint8_t head[9] = {0xff,0x01,0x00,0x04,0x71,0x80,0x14,0x00,0x00};
	
	//Data length
	head[2] = 0x09;
	//device type Item status 1 byte 
	g_FOTA_Flage[0]=app_get_fota_flag();
	head[7] = g_FOTA_Flage[0];
	//Do checksum
	head[valueLen - 1]=Do_CheckSum(head,valueLen);
	
	Philips_Send_Notify(head, (uint32_t)valueLen);
}

void  Set_FOTA_Finish_Flag(uint8_t *valuePtr)
{
	g_FOTA_Flage[0] =  valuePtr[0] & 0x01;
	app_nvrecord_fotaflag_set(g_FOTA_Flage[0]); 
}

unsigned char PCBA_Version[] = "V1.0";
void Get_PCBA_Version(void)
{
	uint8_t valueLen = 0;
	uint8_t i =0;
	uint8_t head[7] = {0xff,0x01,0x00,0x04,0x71,0x80,0x16};
	uint8_t charsize = sizeof(PCBA_Version);

	//TRACE(1,"charsize %d : ", charsize);

	valueLen = 8 + charsize;

	uint8_t version[valueLen];	 

	for (i = 0 ; i < 7 ; i++){
		version[i] = head[i];
	}

	//Data length
	version[2] = valueLen;

	for (i = 7 ; i < (7 + charsize) ; i++){
		version[i] = (uint8_t) PCBA_Version[i - 7];
	}    

	//Do checksum
	version[valueLen - 1]=Do_CheckSum(version,valueLen);

	Philips_Send_Notify(version, (uint32_t)valueLen);
}

bool Philips_Functions_Call(uint8_t *data, uint8_t size)
{
	uint16_t command_id = ((uint16_t)data[5] << 8) | ((uint16_t)data[6]);
	TRACE(3,"***%s: %d %d",__func__,command_id,size);

	switch(command_id)
	{
		case GET_CONNECT_PHONE_MAC:
			//TRACE(0,"Philips : Philips_Functions_Call GET_CONNECT_PHONE_MAC!\r\n");
			Get_Connect_Phone_Mac();
		return true;

		case GET_DEVICE_FEATURE_CONFIG:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_DEVICE_FEATURE_CONFIG!\r\n");
			Get_Device_Feature_Config();
		return true;
		
		case GET_API_VERSION:
			//TRACE(0,"Philips : Philips_Functions_Call GET_API_VERSION!\r\n");
			Get_Api_Version();
		return true;	
				
		case GET_SOUND_QUALITY:
			//TRACE(0,"Philips : Philips_Functions_Call GET_SOUND_QUALITY!\r\n");
			Get_Sound_Quality();
		return true;		
			
		case GET_RIGHT_EAR_MAC_ADDRESS:
			//TRACE(0,"Philips : Philips_Functions_Call GET_RIGHT_EAR_MAC_ADDRESS!\r\n");
			Get_Right_Ear_MAC_Address();
		return true;
			
		case GET_LEFT_EAR_MAC_ADDRESS:
			//TRACE(0,"Philips : Philips_Functions_Call GET_LEFT_EAR_MAC_ADDRESS!\r\n");
			Get_Left_Ear_MAC_Address();
		return true;	
				
		case NOTIFICATION_SOUND_QUALITY_CHANGE:
			//TRACE(0,"Philips : Philips_Functions_Call NOTIFICATION_SOUND_QUALITY_CHANGE!\r\n");
		return true;		
			
		case GET_BLE_MAC_ADDRESS:
			//TRACE(0,"Philips : Philips_Functions_Call GET_BLE_MAC_ADDRESS!\r\n");
			Get_BLE_MAC_Address();
		return true;			
			
		case GET_SALES_REGION:
			//TRACE(0,"Philips : Philips_Functions_Call GET_SALES_REGION!\r\n");
			Get_Sales_Region();
		return true;		
										
		case GET_CHIPSET_VENDOR:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_CHIPSET_VENDOR!\r\n");
			Get_Chipset_Vendor();
		return true;
			
		case GET_CHIPSET_SOLUTION:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_CHIPSET_SOLUTION!\r\n");
			Get_Chipset_Solution();
		return true;
			
		case GET_FW_VERSION:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_FW_VERSION!\r\n");
			Get_Chipset_Version();
		return true;	
			
		case GET_DEVICE_TYPE:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_DEVICE_TYPE!\r\n");
			Get_Device_Type();			
		return true;			
			
		case GET_FOTA_FINISH_FLAG:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_FOTA_FINISH_FLAG!\r\n");
			Get_FOTA_Finish_Flag();
		return true;
		
		case SET_FOTA_FINISH_FLAG:		
			//TRACE(0,"Philips : Philips_Functions_Call SET_FOTA_FINISH_FLAG!\r\n");
			if (size != 9){
				return false;
			}			
			uint8_t fota_flag[1] = {0};
			fota_flag[0] = data[7];		
			Set_FOTA_Finish_Flag(fota_flag);			
		return true;
			
		case GET_PCBA_VERSION:		
			//TRACE(0,"Philips : Philips_Functions_Call GET_PCBA_VERSION!\r\n");
			Get_PCBA_Version();
		return true;	
			
		default:
			TRACE(0,"Philips : Philips_Functions_Call Command error!\r\n");
		break;
	}
	
	return false;
}

