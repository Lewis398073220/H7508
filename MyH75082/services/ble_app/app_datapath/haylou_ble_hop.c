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
#include "string.h"
#include "bluetooth.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "apps.h"
#include "stdbool.h"
#include "rwapp_config.h"
#include "haylou_ble_hop.h"
#include "app_datapath_server.h"
#include "app_ble_cmd_handler.h"
#include "app_ble_custom_cmd.h"

#include <stdio.h>

//#include "../bt_if/inc/avrcp_api.h"
#include "../bt_app/Btapp.h"
#include "../tota/app_spp_tota.h"

#include "../../../apps/userapps/app_user.h"
#include "app_bt_stream.h"
#include "app.h"
#include "app_battery.h"
#include "../../../apps/anc/inc/app_anc.h"
#include "app_bt.h"
#include "haylou_ble_hop.h"
#include "bt_if.h"
#include "besbt.h"



static uint8_t g_valueLen = 0;
static uint8_t g_valuePtr[100] = {0};

static uint8_t protocol_port=0;

static uint8_t opcode_sn=0;
static uint8_t rev_opcode_sn=0;
static uint32_t infor_mask=0;
static uint8_t response_flag=0;

//uint8_t subpackage_data[20]=0;

bool APP_Functions_Call(uint8_t *data, uint32_t size);

void APP_protocol_port(uint8_t port)
{
	protocol_port=port;
}

bool APP_Api_Entry(uint8_t *data, uint32_t size)
{	
   //Check Start head
   if ((data[0] != (uint8_t)0xAA)&&(data[0] != (uint8_t) 0xBB)&&(data[0] != (uint8_t) 0xCC)){        
		return false;
   }

   if ((data[size-3] != (uint8_t)0xDD)&&(data[size-2] != (uint8_t) 0xEE)&&(data[size-1] != (uint8_t) 0xFF)){        
		return false;
   }

   if(size<6)
   	return false;

#if 0
   if (!APP_Functions_Call(data,size)){
		TRACE(0,"Functions_Call Fail!\r\n");
	}

#else
   uint8_t buf_data[size];
   uint32_t buf_end=0;
   uint32_t buf_star=0;
   uint32_t data_p=0;
   uint32_t j=0;
     
   while(data_p<size){
   	    buf_star=0;
		buf_end=0;
	  	for(j=data_p;j<(size-2);j++){
			if((data[j]==0xAA)&&(data[j+1]==0xBB)&&(data[j+2]==0xCC)){
				buf_star=j;
				break;
			}
	  	}
		
		for(j=data_p;j<(size-2);j++){
			if((data[j]==0xDD)&&(data[j+1]==0xEE)&&(data[j+2]==0xFF)){
				buf_end=j+3;
				break;
			}
		}

		if(buf_end>(buf_star+6))
		{
			for(j=0;j<(buf_end-buf_star);j++)
		  	   buf_data[j]=data[buf_star+j];
#if 0	
			uint8_t k=0;
			TRACE(1,"****Receive length %d data: ", buf_end);
			if(buf_end>50){
				while(k<buf_end){ 
				  if((buf_end-k)>=50){
				    DUMP8("0x%02x ", &buf_data[k], 50);
				    k+=50;
				  }
				  else{
					DUMP8("0x%02x ", &buf_data[k], buf_end-k);
					break;
				  }
				}
			}
			else{
				DUMP8("0x%02x ", buf_data, buf_end-buf_star);
			}
#endif
   			if (!APP_Functions_Call(buf_data,(buf_end-buf_star))){
				TRACE(0,"Functions_Call Fail!\r\n");
			}
		}	
		data_p+=buf_end;

		if(buf_end==0)
			return false;
   }
#endif
   TRACE(0,"Test_Haylou_API OK");
   return true;
}
/*
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
*/
uint8_t set_eq_change(uint8_t eq_set)
{
	if(eq_set==0) return (0);
	else if(eq_set==2) return (1);
	else if(eq_set==3) return (2);	
	else if(eq_set==6) return (3);	
	else if(eq_set==7) return (4);
	else if(eq_set==0xF0) return (0XF0);
	else return(0xFF);
}

uint8_t get_eq_change(uint8_t eq_get)
{
	if(eq_get==0) return (0);
	else if(eq_get==1) return (2);
	else if(eq_get==2) return (3);	
	else if(eq_get==3) return (6);	
	else if(eq_get==4) return (7);
	else if(eq_get==0xF0) return (0xF0);
	else return(0xFF);
}

void Haylou_Send_Notify(uint8_t *data, uint32_t size)
{
    if(protocol_port==0)
		app_datapath_server_send_data_via_notification(data,size);
	else
		app_tota_send_data_via_spp(data, size);
}

void Device_Name(uint8_t *buf,uint8_t *len)
{
   	const char * DeviceName=BT_LOCAL_NAME;
	uint8_t name_len=strlen(BT_LOCAL_NAME);
	uint8_t j=0;
	uint8_t name_buf[30]= {0};

    if(name_len>27)
		name_len=27;
	
	memcpy(name_buf, DeviceName, name_len);
	
	buf[0]=name_len+1;
	buf[1]=0x00;
	
	for(j=0;j<name_len;j++){
       	buf[2+j]=name_buf[j];
	}

	len[0]=name_len+2;
}

void Device_Version(uint8_t *buf,uint8_t *len)
{
	uint8_t PCBA_Version=0x02; // 0x02 => v0.2
	uint8_t FW_Version[2]= {0x13,0x00}; //0x10 0x50 => V1.0.5.0  

    buf[0]=0x04;
    buf[1]=0x01;
	buf[2]=PCBA_Version;
	buf[3]=FW_Version[0];
	buf[4]=FW_Version[1];
	len[0]=5;
}

void Device_Battery_Level(uint8_t *buf,uint8_t *len)
{
	uint8_t Battery_Level_Value=app_battery_current_level();

    buf[0]=0x02;
    buf[1]=0x02;
	buf[2]=Battery_Level_Value*10;
	len[0]=3;
}

void Device_Comm_Protocol_Version(uint8_t *buf,uint8_t *len)
{
	uint8_t protocol_version=0x10; //0x10 -> v1.0

    buf[0]=0x02;
    buf[1]=0x03;
	buf[2]=protocol_version;
	len[0]=3;
}

void Device_VID_PID(uint8_t *buf,uint8_t *len)
{
	uint8_t vid[2]={0x00,0x00};
	uint8_t pid[2]={0x00,0x00};

    buf[0]=0x05;
    buf[1]=0x04;
	buf[2]=vid[0];
	buf[3]=vid[1];
	buf[4]=pid[0];
	buf[5]=pid[1];
	len[0]=6;  
}

void Device_Serial_Number(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x05;
    buf[1]=0x05;
	
	buf[2]=0x53;
	buf[3]=0x33;
	buf[4]=0x35;
	buf[5]=0x30;
	
	buf[6]=((get_custom_bin_config(1)<<4)|(get_custom_bin_config(0)&0x0F));

	buf[7]=0x5A;
	buf[8]=0x52;
	
	len[0]=9;   
}

void Device_Area(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x06;
	buf[2]=get_custom_bin_config(1);//device area
	len[0]=3;
}

void Device_EDR_Status(uint8_t *buf,uint8_t *len)
{
	uint8_t connect_status=app_device_EDR_connect_status();

    buf[0]=0x02;
    buf[1]=0x07;
	buf[2]=connect_status;
	len[0]=3;
}

void Device_Mandatory_Upgrade(uint8_t *buf,uint8_t *len)
{
	uint8_t upgrade_flag=0;

    buf[0]=0x02;
    buf[1]=0x08;
	buf[2]=upgrade_flag;
	len[0]=3; 
}

void Device_Uboot_Version(uint8_t *buf,uint8_t *len)
{
	uint8_t uboot_version[2]={0x00,0x00};

    buf[0]=0x03;
    buf[1]=0x09;
	buf[2]=uboot_version[0];
	buf[3]=uboot_version[1];
	len[0]=4;  
}

void Device_Mult_Battery_Level(uint8_t *buf,uint8_t *len)
{
	uint8_t Battery_Level_Value=app_battery_current_level();

    buf[0]=0x04;
    buf[1]=0x0a;
	buf[2]=Battery_Level_Value*10;
	buf[3]=buf[2];
	buf[4]=0;
	len[0]=5;
}

void Device_Codec_type(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x0b;
	buf[2]=0x00;//codec type
	len[0]=3;
}

void Device_Power_Save(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x0c;
	buf[2]=0x00;//power save not support
	len[0]=3;
}

void Device_Func_Key(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x0d;
	buf[2]=0x00;//function key not support
	len[0]=3;
}

void Device_Hot_Word(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x0e;
	buf[2]=0x00;//hot word not support
	len[0]=3;
}

void Device_Charset_Type(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x0f;
	buf[2]=0x00;//utf-8
	len[0]=3;
}

void Device_Color(uint8_t *buf,uint8_t *len)
{
    buf[0]=0x02;
    buf[1]=0x10;
	buf[2]=get_custom_bin_config(0);
	len[0]=3;
}

void Get_Device_Infor_Response(void)
{
	uint8_t i=0;
	uint8_t len[1]={0};
	uint8_t buf[30];
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x02,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
	
	g_valueLen=9;	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }
   
	if(infor_mask & HOP_ATTR_TYPE_NAME){
		Device_Name(buf, len);
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_VERSION){
		Device_Version(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		//TRACE(3,"%s length %d %d", __func__,head[6], len[0]);
		g_valuePtr[6]+=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_BATTERY){
		Device_Battery_Level(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_COMM_PROTOCOL_VER){
		Device_Comm_Protocol_Version(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_VID_AND_PID){
		Device_VID_PID(buf, len); 		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_DEVICE_SERIAL_NUMBER){
		Device_Serial_Number(buf, len); 		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_DEVICE_COUNTRY){
		Device_Area(buf, len); 		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_EDR_CONNECTION_STATUS){
		Device_EDR_Status(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_MANDATORY_UPGRADE_FLAG){
		Device_Mandatory_Upgrade(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_UBOOT_VERSION){
		Device_Uboot_Version(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ARRT_TYPE_MULT_BATTERY){
		Device_Mult_Battery_Level(buf, len);
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_CODEC_TYPE){
		Device_Codec_type(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_POWER_SAVE){
		Device_Power_Save(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_FUNC_KEY){
		Device_Func_Key(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_HOTWORD){
		Device_Hot_Word(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_CHARSET_TYPE){
		Device_Charset_Type(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & HOP_ATTR_TYPE_COLOR){
		Device_Color(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Get_Device_Infor(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
	infor_mask=((uint32_t) data[8]<<24) |((uint32_t) data[9]<<16) | ((uint32_t) data[10]<<8) | ((uint32_t) data[11]);
    TRACE(1,"Get_Device_Infor infor_mask %d",infor_mask);
	if(response_flag){
		Get_Device_Infor_Response();
	}
}

//HOP_OPCODE_DEV_REBOOT
void Get_Device_Reboot_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x03,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 

	g_valueLen=9;	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }
	
	g_valueLen+=3;	
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Device_Reboot(uint8_t *data, uint8_t size)
{    
	//uint16_t para_len=0;
	uint8_t reboot_type=0;
	
	response_flag=data[3] & 0x40;
	//para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];
	reboot_type=data[8];
	

	if(response_flag) {
		Get_Device_Reboot_Response();	
	} 

	if(reboot_type){
     	//shutdown
      	app_shutdown();
	}
	else{
    	//reboot
    	app_reset();
	}
}

//HOP_OPCODE_NOTIFY_PHONE_INFO
void Get_Notify_Phone_Infor_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x04,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 

	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Notify_Phone_Infor(uint8_t *data, uint8_t size)
{    
	//uint16_t para_len=0;
	uint8_t phone_type=0;
	
	response_flag=data[3] & 0x40;
	//para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];
	phone_type=data[8];

	if(response_flag) {
		Get_Notify_Phone_Infor_Response();	
	}

	if(phone_type==0x02){	
		//iphone need to reconnect the ihone
		if(app_device_EDR_connect_status()<2)
			app_bt_profile_connect_manager_opening_reconnect();
	}
}

//HOP_OPCODE_A2F_DISCONNECT_EDR
void Get_A2F_Disconnect_EDR_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x06,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 

	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }
	
	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_A2F_Disconnect_EDR(uint8_t *data, uint8_t size)
{    
	//uint16_t para_len=0;
	
	response_flag=data[3] & 0x40;
	//para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];

	if(response_flag) {
		Get_A2F_Disconnect_EDR_Response();	
	} 

	//need to disconnect
	app_disconnect_all_bt_connections();
}

//HOP_OPCODE_F2A_EDR_STATUS
void Set_F2A_EDR_Status(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0xC0,0x07,0x00,0x02,0x00,0x00};	
    head[7] = opcode_sn++; 

	head[8]=app_device_EDR_connect_status();// BT connect status

	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Set_F2A_EDR_Status_Response(uint8_t *data, uint8_t size)
{    
	//uint16_t para_len=0;
	
	response_flag=data[3] & 0x40;
	//para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];
}

//HOP_OPCODE_SET_DEVICE_INFO
void Get_Setting_Device_Infor_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x08,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
	
	g_valueLen=9;	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

    g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Setting_Device_Infor(uint8_t *data, uint8_t size)
{    
	uint16_t para_len=0;
	uint8_t attr_len=0;
	uint8_t set_attr_mask=0;
	uint8_t set_attr_value=0;
	uint8_t eq_set;
	
	response_flag=data[3] & 0x40;
	para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];
			
	if(response_flag) {
		Get_Setting_Device_Infor_Response();		
	}

    para_len-=1;
	uint8_t j=0;
    while(para_len){
		attr_len=data[8+j];
		set_attr_mask=data[9+j];
		set_attr_value=data[10+j];
 		switch(set_attr_mask){
			case INFO_DEVICE_SHUTDOWN_TIME:
				app_auto_poweroff_set(set_attr_value);
			break;

			case INFO_DEVICE_2DEVICES_CONNECT:

			break;

			case INFO_DEVICE_EQ_MODE:
				eq_set=set_eq_change(set_attr_value);
				if(0xFF!=eq_set){
					app_nvrecord_eq_set(eq_set);
					change_eq_from_ble_api(eq_set);
				}
			break;

			case INFO_SPORTS_MODE_SWITCH_IND:

			break;

			case INFO_DEVICE_ANC_MODE:
				if((set_attr_value>=0)&&(set_attr_value<3)){
					app_nvrecord_anc_set(set_attr_value);
					set_anc_mode(set_attr_value,1);
				}
			break;

			case INFO_DEVICE_GAME_MODE:
				if((set_attr_value>=0)&&(set_attr_value<2)){
					api_gaming_mode(set_attr_value);
				}
			break;

			case INFO_DEVICE_AUTO_PLAY:

			break;

			default:
			break;
 		}
		para_len-=attr_len+1;
		j+=attr_len+1;
    }
}

//HOP_OPCODE_GET_DEVICE_RUN_INFO
void Device_EDR_Mac(uint8_t *buf,uint8_t *len)
{
	uint8_t i=0;
	uint8_t MacAddr[6] ={0};
	memcpy(MacAddr, bt_addr, 6);

	buf[0]=0x07;
	buf[1]=0x00;
    for (i = 0 ; i < 6 ; i++){
	 	buf[2 + i ] = MacAddr[i];
    } 
	len[0]=8;
}

void Device_BLE_Mac(uint8_t *buf,uint8_t *len)
{
	uint8_t i=0;
	uint8_t MacAddr[6] ={0};
	memcpy(MacAddr, ble_addr, 6);

	buf[0]=0x07;
	buf[1]=0x01;
    for (i = 0 ; i < 6 ; i++){
	 	buf[2 + i ] = MacAddr[i];
    } 
	len[0]=8;
}

void Device_Communication_Max_MTU(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x02;
    buf[2]=0x00;//max MTU
	len[0]=3;
}

void Device_Class_BT_Status(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x03;
    buf[2]=app_bt_is_connected();//Class Bluetooth Status
	len[0]=3;
}

void Device_Power_Mode(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x04;
    buf[2]=0x00;//Power mode
	len[0]=3;
}

void Device_Shutdown_Time(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x05;
    buf[2]=app_get_auto_poweroff();//shut down time
	len[0]=3;
}

void Device_TWS_Status(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x06;
    buf[2]=0x00;
	len[0]=3;
}

void Device_Virtual_Addr(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x07;
    buf[2]=0x00;
	buf[3]=0x00;
	buf[4]=0x00;
	buf[5]=0x00;
	len[0]=6;
}

void Device_Dongle_Status(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x08;
    buf[2]=0x00;
	len[0]=3;
}

void Device_ANC_Status(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x09;
    buf[2]=app_get_anc_mode();//anc mode
	len[0]=3;
}

void Device_Auto_Play(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x0A;
    buf[2]=0X01;
	len[0]=3;
}

void Device_Game_Mode(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x0B;
    buf[2]=get_app_gaming_mode();//game mode
	len[0]=3;
}

void Device_EQ_Mode(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x0C;
    buf[2]=get_eq_change(app_eq_index_get());//EQ mode
	len[0]=3;
}

void Device_Sport_ind(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x0D;
    buf[2]=0x00;
	len[0]=3;
}

void Device_1TO2(uint8_t *buf,uint8_t *len)
{
	buf[0]=0x02;
	buf[1]=0x0E;
    buf[2]=0x01;//support 1 TO 2
	len[0]=3;
}

void Get_Device_Run_Infor_Response(void)
{
	uint8_t i=0;
	uint8_t len[1];
	uint8_t buf[10];
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x09,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 

    g_valueLen=9;	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }
   
	if(infor_mask & ATTR_EDR_MAC){
		Device_EDR_Mac(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_BLE_MAC){
		Device_BLE_Mac(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_COMMUNICATION_MAX_MTU){
		Device_Communication_Max_MTU(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_CLASSIC_BLUETOOTH_STATUS){
		Device_Class_BT_Status(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_GET_POWER_MODE){
		Device_Power_Mode(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_DEVICE_SHUTDOWN_TIME){
		Device_Shutdown_Time(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_TWS_STATUS){
		Device_TWS_Status(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_DEVICE_VIRTUAL_ADDRESS){
		Device_Virtual_Addr(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_TYPE_DONGLE_STATUS){
		Device_Dongle_Status(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_TYPE_GET_ANC_STATUS){
		Device_ANC_Status(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_DEVICE_AUTO_PLAY){
		Device_Auto_Play(buf, len);			
		for (i =0;i < len[0]; i++){
	   		g_valuePtr[g_valueLen+i] =buf[i];
    	}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}
	
	if(infor_mask & ATTR_DEVICE_GAME_MODE){
		Device_Game_Mode(buf, len);			
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_DEVICE_EQ_MODE){
		Device_EQ_Mode(buf, len); 		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_SPORTS_MODE_SWITCH_IND){
		Device_Sport_ind(buf, len); 		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	if(infor_mask & ATTR_DEVICE_2DEVICES_CONNECT){
		Device_1TO2(buf, len);		
		for (i =0;i < len[0]; i++){
			g_valuePtr[g_valueLen+i] =buf[i];
		}
		g_valuePtr[6] +=len[0];
		g_valueLen+=len[0];
	}

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Device_Run_Infor(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
	infor_mask=((uint32_t) data[8]<<24) | ((uint32_t) data[9]<<16) | ((uint32_t) data[10]<<8) | ((uint32_t) data[11]);
    
	if(response_flag) {
		Get_Device_Run_Infor_Response();	
	}
}

//HOP_OPCODE_SET_DEVICE_INFO
void Get_Comm_Type_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x0A,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }
	
	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Get_Comm_Type(uint8_t *data, uint8_t size)
{    
	uint8_t comm_type=0;

	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
    comm_type=data[8];
		
	if(response_flag) {
		Get_Comm_Type_Response();	
	}

	APP_protocol_port(comm_type);
}

//HOP_OPCODE_WAKEUP_CLASSIC_BLUETOOTH
void Get_Wakeup_Class_BT_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x0B,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Wakeup_Class_BT(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
		
	if(response_flag) {
		Get_Wakeup_Class_BT_Response();		
	}
}

//HOP_OPCODE_NOTIFY_F2A_BT_OP
void Set_F2A_BT_OP(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0xC0,0x0D,0x00,0x02,0x00,0x00};	
    head[7] = opcode_sn++;

	head[8]=0x01;
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Set_F2A_BT_OP_Response(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
}

//HOP_OPCODE_REPORT_DEVICE_STATUS
void Set_Report_Device_Status(void)
{    
	uint8_t i=0;
	uint8_t data[12];
	uint8_t head[8] = {0xAA,0xBB,0xCC,0xC0,0x0E,0x00,0x0d,0x00};	
    head[7] = opcode_sn++; 
   
	g_valueLen=8+12;	
	data[0]=0x02;
	data[1]=0x05;
	data[2]=app_battery_current_level()*10;
	
	data[3]=0x02;
	data[4]=0x08;
	data[5]=app_get_anc_mode();//ANC MODE

	data[6]=0x02;
	data[7]=0x09;
	data[8]=get_app_gaming_mode();//game MODE

	data[9]=0x02;
	data[10]=0x0a;
	data[11]=get_eq_change(app_eq_index_get());//game MODE
	
    for (i =0;i < 8; i++){
	   g_valuePtr[i] =head[i];
    }

	for (i =0;i < 12; i++){
	   g_valuePtr[8+i] =data[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Set_Report_Battery_Status(void)
{    
	uint8_t i=0;
	uint8_t head[11] = {0xAA,0xBB,0xCC,0xC0,0x0E,0x00,0x04,0x00,0x02,0x05,0x00};	
    head[7] = opcode_sn++; 
   
	g_valueLen=11;	
	head[10]=app_battery_current_level()*10;
	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Set_Report_Game_Status(void)
{    
	uint8_t i=0;
	uint8_t head[11] = {0xAA,0xBB,0xCC,0xC0,0x0E,0x00,0x04,0x00,0x02,0x09,0x00};	
    head[7] = opcode_sn++; 
   
	g_valueLen=11;	
	head[10]=get_app_gaming_mode();//game MODE
	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Set_Report_Anc_Status(void)
{    
	uint8_t i=0;
	uint8_t head[11] = {0xAA,0xBB,0xCC,0xC0,0x0E,0x00,0x04,0x00,0x02,0x08,0x00};	
    head[7] = opcode_sn++; 
   
	g_valueLen=11;	
	head[10]=app_get_anc_mode();//ANC MODE
	
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}


void Set_Report_Device_Status_Response(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];		
}


//HOP_OPCODE_NOTIFY_UNBOUND
void Get_Notify_Unbound_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x0F,0x00,0x02,0x00,0X00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Get_Notify_Unbound(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
		
	if(response_flag) {
		Get_Notify_Unbound_Response();	
	}

	//disconnect bt and anter pairing mode
	app_enter_pairing();
}

//HOP_OPCODE_NOTIFY_A2F_STATUS
void Get_A2F_Status_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x10,0x00,0x02,0x00,0X00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Get_A2F_Status(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
		
	if(response_flag) {
		Get_A2F_Status_Response();	
	}
}

//HOP_OPCODE_NOTIFY_A2F_RESET
void Get_A2F_Reset_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x11,0x00,0x02,0x00,0X00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

void Get_A2F_Reset(uint8_t *data, uint8_t size)
{    
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
		
	if(response_flag) {
		Get_A2F_Reset_Response();	
	}
	app_nvrecord_para_default();
}

//HOP_OPCODE_SET_DEVICE_CONFIG
void Set_Device_Config_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0xF2,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

static uint8_t fine_device_flag=0;
uint8_t api_find_device(void)
{   
    if(fine_device_flag){
		return (fine_device_flag--);
    }
	else{
		return (0);
	}
}
void Set_Device_Config(uint8_t *data, uint8_t size)
{   
   	uint16_t para_len=0;
   	uint8_t tlv_len=0;
   	uint16_t tlv_type=0;
   	uint8_t tlv_value[4]={0};
	uint8_t temp;
	uint8_t i=0;

	response_flag=data[3] & 0x40;
	para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];
		
	if(response_flag) {
		Set_Device_Config_Response();	
	}

	para_len-=1;
	uint8_t tlv_star=8;
    while(para_len){
		tlv_len=data[tlv_star];
		tlv_type=((uint16_t) data[tlv_star+1]<<8) | ((uint16_t) data[tlv_star+2]);
 		switch(tlv_type){
			case CONFIG_AUDIO_MODE:
			break;

			case CONFIG_FUN_KEY:
			break;

			case CONFIG_AUTO_ANSWER:
			break;

			case CONFIG_MULTI_CONNECT:
			break;

			case CONFIG_CONJOIN_DETECT:
			break;

			case CONFIG_HEADPHONE_FIT:
			break;

			case CONFIG_EQ_MODE:
				tlv_value[0]=data[tlv_star+3];
 				temp=set_eq_change(tlv_value[0]);
				if(0xFF!=temp){
					app_nvrecord_eq_set(temp);
					change_eq_from_ble_api(temp);
				}
			break;
			
			case CONFIG_DEVICE_NAME:
				bt_name_len=tlv_len-2;
				if(bt_name_len>0){
					if(bt_name_len>26)
						bt_name_len=26;

					for(i=0;i<bt_name_len;i++)
						bt_name[i]=data[tlv_star+3+i]; 

					bt_name[i]='\0';
					app_nvrecord_set_bt_name();
				}
			break;

			case CONFIG_FIND_DEVICE:
			tlv_value[0]=data[tlv_star+3];
			tlv_value[1]=data[tlv_star+4];
			if(tlv_value[0]){
				fine_device_flag=19;
				app_api_start();
			}
			else{
				fine_device_flag=0;
				app_api_stop();
			}
			break;

			case CONFIG_ANC_MODE:
				//tlv_value[0]=data[tlv_star+3];
			break;

			case CONFIG_ANC_DEPTH:
				tlv_value[0]=data[tlv_star+3];
				temp=tlv_value[0];
				if((temp>0)&&(temp<3)){
					app_nvrecord_anc_set(temp);
					set_anc_mode(temp,1);
				}
			break;

			case CONFIG_ANTI_LOST:
			break;

			case CONFIG_SERIES_DETECT:
			break;

			case CONFIG_APP_NAME:
				
			break;

			default:
			break;
 		}
		para_len-=tlv_len+1;
		tlv_star+=tlv_len+1;
    }	
}

//HOP_OPCODE_GET_DEVICE_CONFIG
uint8_t tlv_config_count=0;
uint16_t tlv_config[14]={0};
void Get_Device_Config_Response(void)
{
	uint8_t i=0;
	uint8_t data[85];
	const char * DeviceName=BT_LOCAL_NAME;
	uint8_t name_len=strlen(BT_LOCAL_NAME);
	uint8_t name_buf[30];
	uint8_t len=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0xF3,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

    uint16_t tlv_config_value=0;
	while(tlv_config_count){
		tlv_config_count--;
		tlv_config_value=tlv_config[tlv_config_count];
		switch(tlv_config_value){
			case CONFIG_AUDIO_MODE:
				data[len++]=0x03;
				data[len++]=0x00;
			    data[len++]=0x01;
				data[len++]=0x00;
				g_valuePtr[6]+=4;
			break;

			case CONFIG_FUN_KEY:
				data[len++]=0x05;
				data[len++]=0x00;
			    data[len++]=0x02;
				data[len++]=0x00;
				data[len++]=0xFF;
				data[len++]=0xFF;
				g_valuePtr[6]+=6;
			break;

			case CONFIG_AUTO_ANSWER:
				data[len++]=0x03;
				data[len++]=0x00;
			    data[len++]=0x03;
				data[len++]=0x00;
				g_valuePtr[6]+=4;
			break;

			case CONFIG_MULTI_CONNECT:
				data[len++]=0x03;
				data[len++]=0x00;
			    data[len++]=0x04;
				data[len++]=0x01;
				g_valuePtr[6]+=4;
			break;

			case CONFIG_CONJOIN_DETECT:
				data[len++]=0x03;
				data[len++]=0x00;
			    data[len++]=0x05;
				data[len++]=0x00;
				g_valuePtr[6]+=4;
			break;

			case CONFIG_HEADPHONE_FIT:
				data[len++]=0x04;
				data[len++]=0x00;
			    data[len++]=0x06;
				data[len++]=0x00;
				data[len++]=0x00;
				g_valuePtr[6]+=5;
			break;

			case CONFIG_EQ_MODE:
				data[len++]=0x03;
				data[len++]=0x00;
			    data[len++]=0x07;
				data[len++]=get_eq_change(app_eq_index_get());
				g_valuePtr[6]+=4;	
			break;
			
			case CONFIG_DEVICE_NAME:
				memcpy(name_buf, DeviceName, name_len);
				data[len++]=name_len+2;
				data[len++]=0x00;
			    data[len++]=0x08;
				for(uint8_t j=0;j<name_len;j++)
				  data[len++]=name_buf[j];
				g_valuePtr[6]+=name_len+3;	
			break;

			case CONFIG_FIND_DEVICE:
				data[len++]=0x04;
				data[len++]=0x00;
			    data[len++]=0x09;
				data[len++]=0x00;
				data[len++]=0x00;
				g_valuePtr[6]+=5;	
			break;

			case CONFIG_ANC_MODE:
				data[len++]=0x04;
				data[len++]=0x00;
			    data[len++]=0x0a;
				data[len++]=0x07;
				data[len++]=0x07;
				g_valuePtr[6]+=5;	
			break;

			case CONFIG_ANC_DEPTH:
				data[len++]=0x04;
				data[len++]=0x00;
			    data[len++]=0x0b;
				data[len++]=app_get_anc_mode();//anc mode;
				data[len++]=0x00;
				g_valuePtr[6]+=5;
			break;

			case CONFIG_ANTI_LOST:
				data[len++]=0x03;
				data[len++]=0x00;
			    data[len++]=0x0c;
				data[len++]=0x00;
				g_valuePtr[6]+=4;	
			break;

			case CONFIG_SERIES_DETECT:
				data[len++]=0x04;
				data[len++]=0x00;
			    data[len++]=0x0d;
				data[len++]=0x00;
				data[len++]=0x00;
				g_valuePtr[6]+=5;	
			break;

			case CONFIG_APP_NAME:

			break;

			default:
			break;
 		}	
	}

    if(len){	
		for (i =0;i <len; i++){
	   		g_valuePtr[g_valueLen+i] =data[i];
    	}
		g_valueLen+=len;
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Device_Config(uint8_t *data, uint8_t size)
{   
   uint16_t para_len=0;
   
	response_flag=data[3] & 0x40;
	para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	rev_opcode_sn=data[7];

	para_len-=1;
	uint8_t tvl_star=8;
	tlv_config_count=0;
    while(para_len){
		tlv_config[tlv_config_count]=((uint16_t) data[tvl_star]<<8) | ((uint16_t) data[tvl_star+1]);
		tlv_config_count++;
		tvl_star+=2;
	    para_len-=2;
    }
		
	if(response_flag) {
		Get_Device_Config_Response();	
	}
}

//HOP_OPCODE_GET_DEVICE_CONFIG
/*
void Set_Device_Config(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0xF4,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < 8; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Set_Device_Config_Response(uint8_t *data, uint8_t size)
{   
	
}
*/
void Set_Custom_EQ_Config_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0xD1,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Set_Custom_EQ_Config(uint8_t *data, uint8_t size)
{ 
/*
	uint16_t customization_eq_value[10];
	uint16_t para_len;

	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
		
	if(response_flag) {
		Set_Custom_EQ_Config_Response();	
	}

	para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	
    if(para_len<61)
		return;
		
	customization_eq_value[0]=((uint16_t) data[8]<<8) | ((uint16_t) data[9]);
	customization_eq_value[1]=((uint16_t) data[14]<<8) | ((uint16_t) data[15]);
	customization_eq_value[2]=((uint16_t) data[20]<<8) | ((uint16_t) data[21]);
	customization_eq_value[3]=((uint16_t) data[26]<<8) | ((uint16_t) data[27]);
	customization_eq_value[4]=((uint16_t) data[32]<<8) | ((uint16_t) data[33]);
	customization_eq_value[5]=((uint16_t) data[38]<<8) | ((uint16_t) data[39]);
	customization_eq_value[6]=((uint16_t) data[44]<<8) | ((uint16_t) data[45]);
	customization_eq_value[7]=((uint16_t) data[50]<<8) | ((uint16_t) data[51]);
	customization_eq_value[8]=((uint16_t) data[56]<<8) | ((uint16_t) data[57]);
	customization_eq_value[9]=((uint16_t) data[62]<<8) | ((uint16_t) data[63]);

	app_nvrecord_eq_param_set(customization_eq_value);
	app_nvrecord_eq_set(0xF0);
	change_eq_from_ble_api(0xF0);
*/
}
/*
void Get_Custom_EQ_Config_Response(void)
{
	uint8_t i=0;
	uint8_t customization_eq_value[6];
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0xD2,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	app_eq_custom_para_get(customization_eq_value);

	for (i =0;i <18; i++){	
		g_valuePtr[g_valueLen+i] =0x00;
	}
	
	g_valuePtr[11] =customization_eq_value[0];
	g_valuePtr[14] =customization_eq_value[1];
	g_valuePtr[17] =customization_eq_value[2];
	g_valuePtr[20] =customization_eq_value[3];
	g_valuePtr[23] =customization_eq_value[4];
	g_valuePtr[26] =customization_eq_value[5];
		
	g_valuePtr[6] +=18;
	g_valueLen+=18;
	
	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}
void Get_Custom_EQ_Config(uint8_t *data, uint8_t size)
{   
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];

	if(response_flag) {
		Get_Custom_EQ_Config_Response();	
	}
}
*/

void Set_Custom_EQ_file_Response(void)
{
	uint8_t i=0;
	uint8_t head[9] = {0xAA,0xBB,0xCC,0x00,0x12,0x00,0x02,0x00,0x00};	
    head[8] = rev_opcode_sn; 
   
	g_valueLen=9;
    for (i =0;i < g_valueLen; i++){
	   g_valuePtr[i] =head[i];
    }

	g_valueLen+=3;
	g_valuePtr[g_valueLen-3]=0xDD;
	g_valuePtr[g_valueLen-2]=0xEE;
	g_valuePtr[g_valueLen-1]=0xFF;
	
    Haylou_Send_Notify(g_valuePtr, (uint32_t)g_valueLen);
}

static float char_to_int(uint8_t *file_char,uint8_t char_len);
static void set_eq_para(float eq_para, uint8_t idex,uint8_t eq_type);
void set_Custom_EQ_file(uint8_t *data, uint32_t size)
{
	uint16_t para_len;
	uint16_t file_len;
	uint8_t  file_type;
	uint16_t file_star=0;
	uint16_t i=0;
	uint8_t j=0;
	uint8_t temp[10];

	float float_data=0;
	uint8_t eq_para_idex=0;
	bool eq_type=0;
	
	response_flag=data[3] & 0x40;
	rev_opcode_sn=data[7];
			
	if(response_flag) {
		Set_Custom_EQ_file_Response();	
	}

	para_len=((uint16_t) data[5]<<8) | ((uint16_t) data[6]);
	file_len=((uint16_t) data[8]<<8) | ((uint16_t) data[9]);
	file_type=data[10];
	
	if(file_type==0x01){
		if(size!=(file_len+14))
			return;
		
		if(para_len!=(file_len+4))
			return;

		file_star=11;
		j=0;
		while(file_star<(file_len+11)){
			for(i=file_star;i<(file_len+11);i++){
				if((data[i]!=0x2C)&&(data[i]!=0x0A)){
					temp[j++]=data[i];

				    if(j>9) return;

				    if(i==(file_len+10)){
						file_star=i+1;
						eq_para_idex++;
						float_data=char_to_int(temp,j);	
						set_eq_para(float_data, eq_para_idex,eq_type);
						//TRACE(2,"float_data=%d, index=%d",(int32_t)(float_data*10),eq_para_idex);
						break;
				    }
				}
				else{
					file_star=i+1;
					eq_para_idex++;
				    float_data=char_to_int(temp,j);
					set_eq_para(float_data, eq_para_idex,eq_type);
					//TRACE(2,"float_data=%d, index=%d",(int32_t)(float_data*10),eq_para_idex);
					j=0;
					if(data[i]==0x0A){
						eq_type=1;
						eq_para_idex=0;
					}
					break;
				}
			}
		}

		eq_set_index=0xf0;

		change_eq_from_ble_api(0Xf0);
		app_nvrecord_eq_set(0Xf0);
		app_nvrecord_eq_param_set();
		app_nvrecord_eq_param2_set();	
	}
}

static float char_to_int(uint8_t *file_char,uint8_t char_len)
{
	uint8_t index=0;
	bool sign_flag=0;
	uint8_t point=0;
	uint16_t intpart=0;
	uint16_t fracpart=0;
	float get_float=0;

	while(index<char_len){
		switch(file_char[index]){
			case 0x2d:
				sign_flag=1;
		    break;
			case 0x2e:
				point=1;
			break;
			case 0x30:
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x34:
			case 0x35:
			case 0x36:
			case 0x37:
			case 0x38:
			case 0x39:
				if(point){
					if(fracpart!=0) fracpart*=10;
					fracpart+=(file_char[index]&0x0F);

					point++;
				}
				else{
					if(intpart!=0) intpart*=10;
					intpart+=file_char[index]&0x0F;
				}
			break;
			default:
			break;
		}	
		index++;
	}

	get_float=(float)fracpart;
	while(point>1)
	{
		get_float/=10;
		point--;
	}

    get_float+=(float)intpart;
	
	if(sign_flag){
		get_float*=-1;
	}

    return get_float;
}

static void set_eq_para(float eq_para, uint8_t idex,uint8_t eq_type)
{
	IIR_CFG_T *custom_eq_set;
	uint8_t para_num=0;
	uint8_t para_t=0;

	if(eq_type==0)
		custom_eq_set=&eq_custom_para_ancon;
	else
		custom_eq_set=&eq_custom_para_ancoff;

	if(idex==1)		
		custom_eq_set->gain0=eq_para;
	else if(idex==2)	
		custom_eq_set->gain1=eq_para;
	else if(idex==3)
		custom_eq_set->num=(int)eq_para;
	else{ 
		para_num=idex/4-1;
		para_t=idex%4;

		if(para_t==0)
			custom_eq_set->param[para_num].type=eq_para;
		if(para_t==1)
			custom_eq_set->param[para_num].gain=eq_para;
		if(para_t==2)	
			custom_eq_set->param[para_num].fc=eq_para;
		if(para_t==3)		
			custom_eq_set->param[para_num].Q=eq_para;
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool APP_Functions_Call(uint8_t *data, uint32_t size)
{
	uint8_t opcode = (uint8_t)data[4];
	   
	switch(opcode)
	{
		case HOP_OPCODE_DATA_TRANSMIT:
			TRACE(0,"HOP_OPCODE_DATA_TRANSMIT\r\n");
			return true;
		
		case HOP_OPCODE_GET_DEVICE_INFO:
			TRACE(0,"HOP_OPCODE_GET_DEVICE_INFO\r\n");
			Get_Device_Infor(data, size);
			return true;
		
		case HOP_OPCODE_DEV_REBOOT:
			TRACE(0,"HOP_OPCODE_DEV_REBOOT\r\n");
			Get_Device_Reboot(data, size);
			return true;	
				
		case HOP_OPCODE_NOTIFY_PHONE_INFO:
			TRACE(0,"HOP_OPCODE_NOTIFY_PHONE_INFO\r\n");
			Get_Notify_Phone_Infor(data, size);
			return true;		
			
		case HOP_OPCODE_A2F_DISCONNECT_EDR:
			TRACE(0,"HOP_OPCODE_A2F_DISCONNECT_EDR\r\n");
			Get_A2F_Disconnect_EDR(data, size);
			return true;
			
		case HOP_OPCODE_F2A_EDR_STATUS:
			TRACE(0,"HOP_OPCODE_F2A_EDR_STATUS\r\n");
			Set_F2A_EDR_Status_Response(data, size);
			return true;	
				
		case HOP_OPCODE_SET_DEVICE_INFO:
			TRACE(0,"HOP_OPCODE_SET_DEVICE_INFO\r\n");
			Get_Setting_Device_Infor(data, size);
			return true;		
			
		case HOP_OPCODE_GET_DEVICE_RUN_INFO:
			TRACE(0,"HOP_OPCODE_GET_DEVICE_RUN_INFO\r\n");
			Get_Device_Run_Infor(data, size);
			return true;			
			
		case HOP_OPCODE_A2F_COMMUNICATION_TYPE:
			TRACE(0,"HOP_OPCODE_A2F_COMMUNICATION_TYPE\r\n");
			Get_Comm_Type(data, size);
			return true;		
										
		case HOP_OPCODE_WAKEUP_CLASSIC_BLUETOOTH:
			TRACE(0,"HOP_OPCODE_WAKEUP_CLASSIC_BLUETOOTH\r\n");
			Get_Wakeup_Class_BT(data, size);
			return true;
			
		case HOP_OPCODE_NOTIFY_PHONE_VIRTUAL_ADDR:
			TRACE(0,"HOP_OPCODE_NOTIFY_PHONE_VIRTUAL_ADDR\r\n");
			return true;
			
		case HOP_OPCODE_NOTIFY_F2A_BT_OP:
			TRACE(0,"HOP_OPCODE_NOTIFY_F2A_BT_OP\r\n");
			Set_F2A_BT_OP_Response(data, size);
			return true;	
			
		case HOP_OPCODE_REPORT_DEVICE_STATUS:
			TRACE(0,"HOP_OPCODE_REPORT_DEVICE_STATUS\r\n");
			Set_Report_Device_Status_Response(data, size);
			return true;			
			
		case HOP_OPCODE_NOTIFY_UNBOUND:
			TRACE(0,"HOP_OPCODE_NOTIFY_UNBOUND\r\n");
			Get_Notify_Unbound(data, size);
			return true;
			
		case HOP_OPCODE_NOTIFY_A2F_STATUS:
			TRACE(0,"HOP_OPCODE_NOTIFY_A2F_STATUS\r\n");
			Get_A2F_Status(data, size);
			return true;

		case HOP_OPCODE_NOTIFY_A2F_RESET:
			TRACE(0,"HOP_OPCODE_NOTIFY_A2F_STATUS\r\n");
			Get_A2F_Reset(data, size);
			return true;
			
		case HOP_OPCODE_SET_DEVICE_CONFIG:
			TRACE(0,"HOP_OPCODE_SET_DEVICE_CONFIG\r\n");
			Set_Device_Config(data, size);
			return true;	
			
		case HOP_OPCODE_GET_DEVICE_CONFIG:
			TRACE(0,"HOP_OPCODE_GET_DEVICE_CONFIG\r\n");
			Get_Device_Config(data, size);
			return true;
		case HOP_OPCODE_NOTIFY_DEVICE_CONFIG:
			TRACE(0,"HOP_OPCODE_NOTIFY_DEVICE_CONFIG\r\n");

			return true;
		case HOP_OPCODE_SET_DEVICE_EQMODE_CONFIG:
			TRACE(0,"HOP_OPCODE_SET_DEVICE_EQMODE_CONFIG\r\n");
			//Set_Custom_EQ_Config(data, size);
			return true;
			
		case HOP_OPCODE_GET_DEVICE_EQMODE_CONFIG:
			TRACE(0,"HOP_OPCODE_GET_DEVICE_EQMODE_CONFIG\r\n");
			//Get_Custom_EQ_Config(data, size);
			return true;

		case HOP_OPCODE_SEND_DEVICE_FILE:
			TRACE(0,"HOP_OPCODE_SEND_DEVICE_FILE\r\n");
			set_Custom_EQ_file(data, size);
			return true;

		default:
			TRACE(0,"HAYLOU Command error!\r\n");
			break;
	}
	
	return false;
}
