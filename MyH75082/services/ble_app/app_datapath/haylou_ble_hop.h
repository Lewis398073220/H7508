/***************************************************************************
 *
 * Copyright CMT
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
#ifndef __HAYLOU_BLE_HOP_H__
#define __HAYLOU_BLE_HOP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HAYLOU_OPCODE
{
	HOP_OPCODE_DATA_TRANSMIT = 0x01,			
	HOP_OPCODE_GET_DEVICE_INFO,				
	HOP_OPCODE_DEV_REBOOT,				                   
	HOP_OPCODE_NOTIFY_PHONE_INFO,						
	HOP_OPCODE_SET_PRO_MTU,	//05			
	HOP_OPCODE_A2F_DISCONNECT_EDR,			
	HOP_OPCODE_F2A_EDR_STATUS,
	HOP_OPCODE_SET_DEVICE_INFO,
	HOP_OPCODE_GET_DEVICE_RUN_INFO,
	HOP_OPCODE_A2F_COMMUNICATION_TYPE,//0A	
	HOP_OPCODE_WAKEUP_CLASSIC_BLUETOOTH,
	HOP_OPCODE_NOTIFY_PHONE_VIRTUAL_ADDR,
	HOP_OPCODE_NOTIFY_F2A_BT_OP,
	HOP_OPCODE_REPORT_DEVICE_STATUS,
	HOP_OPCODE_NOTIFY_UNBOUND,//0F
	HOP_OPCODE_NOTIFY_A2F_STATUS=0X10,
	HOP_OPCODE_NOTIFY_A2F_RESET,
	HOP_OPCODE_SEND_DEVICE_FILE,
	HOP_OPCODE_SET_DEVICE_CONFIG=0XF2,
	HOP_OPCODE_GET_DEVICE_CONFIG,
	HOP_OPCODE_NOTIFY_DEVICE_CONFIG,
	HOP_OPCODE_SET_DEVICE_EQMODE_CONFIG=0xD1,
	HOP_OPCODE_GET_DEVICE_EQMODE_CONFIG=0xD2,
}HAYLOU_OPCODE;

typedef enum HAYLOU_COMMAND_STATUS
{
	HOP_SUCCESS = 0x00,	             
	HOP_FAIL,		
	HOP_UNKOWN_CMD,					
	HOP_BUSY,						
	HOP_NO_RESPONSE,                           
	HOP_CRC_ERROR,                           	
	HOP_ALL_DATA_CRC_ERROR,
	HOP_PARAM_ERROR,
	HOP_RESPONSE_DATA_OVER_LIMIT,
	HOP_NOT_SUPPORT,
	HOP_PARTIAL_OPERATION_FAILED,
	HOP_UNREACHABLE,
}HAYLOU_COMMAND_STATUS;


//HOP_OPCODE_GET_DEVICE_INFO ATTR
#define HOP_ATTR_TYPE_NAME 						0x00000001
#define HOP_ATTR_TYPE_VERSION 			  		0x00000002
#define HOP_ATTR_TYPE_BATTERY 	               	0x00000004
#define HOP_ATTR_COMM_PROTOCOL_VER 			  	0x00000008
#define HOP_ATTR_TYPE_VID_AND_PID 	  			0x00000010
#define HOP_ATTR_DEVICE_SERIAL_NUMBER 		  	0x00000020
#define HOP_ATTR_DEVICE_COUNTRY 		     	0x00000040
#define HOP_ATTR_TYPE_EDR_CONNECTION_STATUS 	0x00000080

#define HOP_ATTR_TYPE_MANDATORY_UPGRADE_FLAG	0x00000100
#define HOP_ATTR_TYPE_UBOOT_VERSION 	      	0x00000200
#define HOP_ARRT_TYPE_MULT_BATTERY 	            0x00000400
#define HOP_ATTR_TYPE_CODEC_TYPE 			    0x00000800
#define HOP_ATTR_TYPE_POWER_SAVE 	        	0x00001000
#define HOP_ATTR_TYPE_FUNC_KEY 		        	0x00002000
#define HOP_ATTR_TYPE_HOTWORD 		  			0x00004000
#define HOP_ATTR_TYPE_CHARSET_TYPE              0x00008000

#define HOP_ATTR_TYPE_COLOR				       	0x00010000

//HOP_OPCODE_SET_DEVICE_INFO
#define INFO_DEVICE_SHUTDOWN_TIME 				0x00
#define INFO_DEVICE_2DEVICES_CONNECT 			0x01
#define INFO_DEVICE_EQ_MODE 	            	0x02
#define INFO_SPORTS_MODE_SWITCH_IND 			0x03
#define INFO_DEVICE_ANC_MODE 	  				0x04
#define INFO_DEVICE_GAME_MODE 		  			0x05
#define INFO_DEVICE_AUTO_PLAY 		     		0x06


//HOP_OPCODE_GET_DEVICE_RUN_INFO
#define ATTR_EDR_MAC 							0x00000001
#define ATTR_BLE_MAC 			  				0x00000002
#define ATTR_COMMUNICATION_MAX_MTU 	            0x00000004
#define ATTR_CLASSIC_BLUETOOTH_STATUS 			0x00000008
#define ATTR_GET_POWER_MODE 	  				0x00000010
#define ATTR_DEVICE_SHUTDOWN_TIME 		  		0x00000020
#define ATTR_TWS_STATUS 		     			0x00000040
#define ATTR_DEVICE_VIRTUAL_ADDRESS 			0x00000080

#define ATTR_TYPE_DONGLE_STATUS					0x00000100
#define ATTR_TYPE_GET_ANC_STATUS 	      		0x00000200
#define ATTR_DEVICE_AUTO_PLAY 	            	0x00000400
#define ATTR_DEVICE_GAME_MODE 			    	0x00000800
#define ATTR_DEVICE_EQ_MODE 	        		0x00001000
#define ATTR_SPORTS_MODE_SWITCH_IND 		    0x00002000
#define ATTR_DEVICE_2DEVICES_CONNECT 		  	0x00004000


#define CONFIG_AUDIO_MODE 						0x0001
#define CONFIG_FUN_KEY 							0x0002
#define CONFIG_AUTO_ANSWER 						0x0003
#define CONFIG_MULTI_CONNECT 					0x0004
#define CONFIG_CONJOIN_DETECT 					0x0005
#define CONFIG_HEADPHONE_FIT 					0x0006
#define CONFIG_EQ_MODE 							0x0007
#define CONFIG_DEVICE_NAME 						0x0008
#define CONFIG_FIND_DEVICE 						0x0009
#define CONFIG_ANC_MODE 						0x000A
#define CONFIG_ANC_DEPTH 						0x000B
#define CONFIG_ANTI_LOST 						0x000C
#define CONFIG_SERIES_DETECT 					0x000D
#define CONFIG_APP_NAME 						0x000E

void APP_protocol_port(uint8_t port);
bool APP_Api_Entry(uint8_t *data, uint8_t size);
void Set_Report_Device_Status(void);
void Set_Report_Battery_Status(void);
void Set_Report_Game_Status(void);
void Set_Report_Anc_Status(void);
uint8_t api_find_device(void);
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PHILIPS_BLE_API_H__