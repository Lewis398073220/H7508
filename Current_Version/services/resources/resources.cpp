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

#include "stdio.h"
#include "cmsis_os.h"
#include "string.h"

#include "hal_timer.h"
#include "hal_trace.h"

#include "crc32.h"
#include "resources.h"

#ifdef MEDIA_PLAYER_SUPPORT

typedef struct {
	UINT32 MARK;
	UINT16 ID;  // resource id
	UINT16 count;  // count of resource
	UINT32 total_size;  // size include resource header, header list and data
} ResourceHeader_t;


typedef struct {
    UINT16 audio_id; // audio id
    UINT16 type;
    UINT32 size;
    UINT32 offset;
} AudioHeader_t;

typedef struct _AUDIO_List
{
    struct _AUDIO_List *next;
    AudioHeader_t record;
} AUDIO_List;



#define AUDIO_TOOL_VERSION  0x6001
#define MAGIC_NUMBER  0xC0BA
#define AUDIO_TOOL_RES_MARK 0xDDCCBBAA

UINT16 gVersionNO = 0;
UINT32 gReserver1 = 0;
UINT32 gReserver2 = 0;

ResourceHeader_t *gpResourceBase = NULL;   // base address of resource data, will be set by boot monitor

AUDIO_List *gpAudioList = NULL;
AUDIO_List *gpAudioList_CN = NULL;

uint8_t *gpAudioDataBase = NULL;
uint8_t *gpAudioDataBase_CN = NULL;


//uint8_t os_pool_audio_List[sizeof(AUDIO_List)*MAX_RECORD_NUM ]    __attribute__((section(".audioList")));
AUDIO_List os_pool_audio_List[MAX_RECORD_NUM];  //__attribute__((section(".audioList")));
//osPoolDef_t os_pool_def_audio_List = { MAX_RECORD_NUM, sizeof(AUDIO_List), os_pool_audio_List };//osPoolDef(name, no, type)
//osPoolId AUDIO_ListPool_Id;

AUDIO_List os_pool_audio_List_CN[MAX_RECORD_NUM];

UINT8* aud_get_reouce(AUD_ID_ENUM id, UINT32* leng, UINT16* type)
{
	AUDIO_List * list = gpAudioList;

	*leng = 0, *type = 0;

	while(list)
	{
		if (list->record.audio_id == id)
		{
			*leng = list->record.size;
			*type = list->record.type;
			return (UINT8*) (gpAudioDataBase + list->record.offset);
		}
		list = list->next;
	}

	return 0;
}

UINT8* aud_get_reouce_chinese(AUD_ID_ENUM id, UINT32* leng, UINT16* type)
{
	AUDIO_List * list = gpAudioList_CN;

	*leng = 0, *type = 0;

	while(list)
	{
		if (list->record.audio_id == id)
		{
			*leng = list->record.size;
			*type = list->record.type;
			return (UINT8*) (gpAudioDataBase_CN + list->record.offset);
		}
		list = list->next;
	}

	return 0;
}

/*
--1.Version No&Magic No.<4>
--2.CRC				<4>
--3.Reserver1			<4> total length  = 5+6+7+...
--4.Reserver2			<4>
--5.ResourceHeader_t	<8>
--6.AUDIO_HEAD_LIST X count   <16 x count>
--7.file.. gpAudioDataBase.. offset
--.... another resource
*/
void init_audio_resource(void* gResource)
{
	uint32_t crc;
	uint32_t *pcrc;
	uint32_t data;

	UINT32 res_addr;
	ResourceHeader_t *pRes;

	data =  *((UINT32*)gResource);
	gVersionNO = 0xFFFF & (data>>16);
	gReserver1 =  *((UINT32*)((uint32_t)gResource+2*sizeof(UINT32)));

	pcrc = (uint32_t *)((uint32_t)gResource+sizeof(UINT32)); /*crc*/

	gpResourceBase = (ResourceHeader_t *) ((uint32_t)gResource + 4*sizeof(uint32_t));
/********************************************************/
	crc = crc32(0, (uint8_t *)((uint32_t)gpResourceBase) , gReserver1);
	TRACE(3,"%s, *pcrc: %x ,  crc: %x",__func__, *pcrc, crc);
	if (*pcrc != crc)
	    return;

	res_addr = (uint32_t)gpResourceBase;
	pRes = (ResourceHeader_t *)gpResourceBase;

	while((UINT32)pRes < (UINT32)gpResourceBase +gReserver1 && pRes->MARK == AUDIO_TOOL_RES_MARK)
	{
		if (pRes->ID == RES_ENGLISH_ID)
		{
			AUDIO_List * list;
			memset(os_pool_audio_List, 0, sizeof(AUDIO_List)*MAX_RECORD_NUM);
			memcpy((uint8_t *)os_pool_audio_List, (uint8_t *)((uint32_t)pRes+sizeof(ResourceHeader_t)),sizeof(AUDIO_List)* pRes->count);

			gpAudioList = (AUDIO_List *)os_pool_audio_List;
			list = gpAudioList;

			gpAudioDataBase = (uint8_t *)pRes + sizeof(ResourceHeader_t) + sizeof(AUDIO_List)* pRes->count;
			TRACE(3,"%s,  english count: %d, leng: %d",__func__, pRes->count, pRes->total_size);

			for(int i=1; i<pRes->count; i++)
			{
				list->next = &os_pool_audio_List[i];

				list = list->next;
			}

			TRACE(2,"%s,  english list: %p",__func__, list);

		}
		else if (pRes->ID == RES_CHINESE_ID)
		{

			AUDIO_List * list;
			TRACE(2,"%s, AUDIO_TOOL_RES_MARK: %d",__func__, pRes->count);

			memset(os_pool_audio_List_CN, 0, sizeof(AUDIO_List)*MAX_RECORD_NUM);
			memcpy((uint8_t *)os_pool_audio_List_CN, (uint8_t *)((uint32_t)pRes+sizeof(ResourceHeader_t)),sizeof(AUDIO_List)* pRes->count);

			gpAudioList_CN= (AUDIO_List *)os_pool_audio_List_CN;
			list = gpAudioList_CN;

			gpAudioDataBase_CN = (uint8_t *)pRes +sizeof(ResourceHeader_t) + sizeof(AUDIO_List)* pRes->count;
			TRACE(3,"%s, chinese count: %d, leng: %d",__func__, pRes->count, pRes->total_size);

			for(int i=1; i<pRes->count; i++)
			{
				list->next = &os_pool_audio_List_CN[i];

				list = list->next;
			}
		}

		TRACE(3,"%s, %x,  pRes->total_size: %x",__func__,res_addr, pRes->total_size);

		  res_addr += pRes->total_size;
	        pRes = (ResourceHeader_t*)(res_addr);

	}

	TRACE(2,"%s, pRes add: %p",__func__, pRes);

}

#if 0
UINT8 BIN_FILE[] =
{
0xba,0xc0,0x01,0x60,0x18,0x17,0x81,0x7a,0x90,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
0xaa,0xbb,0xcc,0xdd,0x00,0xff,0x1c,0x00,0xc8,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x02,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x03,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x1b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x05,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x2d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x07,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x08,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x09,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0a,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x5a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0b,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0c,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0d,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x75,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0e,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0f,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x87,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x10,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x11,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x12,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xa2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x13,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xab,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x14,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xb4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x15,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xbd,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x16,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xc6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x17,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x18,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x19,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xe1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x1a,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xea,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x1b,0x00,0x04,0x00,0x09,0x00,0x00,0x00,0xf3,0x00,0x00,0x00,0x34,0x34,0x34,0x35,
0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,
0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,
0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,
0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,
0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,
0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,
0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,
0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,
0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,
0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,
0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,
0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,
0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,
0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,
0x35,0x35,0x35,0x36,0x36,0x36,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x34,
0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0xaa,0xbb,0xcc,0xdd,0x01,0xff,0x1c,0x00,
0xc8,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x1b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x2d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x5a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0b,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x6c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x75,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x87,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xa2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xab,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xb4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xbd,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x16,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xc6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xe1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1a,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xea,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1b,0x00,0x04,0x00,0x09,0x00,0x00,0x00,
0xf3,0x00,0x00,0x00,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,
0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,
0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,
0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,
0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,
0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,
0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,
0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,
0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,
0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,
0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,
0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,
0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,
0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,
0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,
0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x31,0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,

};
void test_resource_load()
{
	UINT32 leng;
	UINT16 type;
	UINT8* data;

	init_audio_resource(BIN_FILE);

	data = aud_get_reouce(AUD_ID_BT_CALL_REFUSE, &leng, &type);

	TRACE(6,"====english_audio, leng = %d, type = %d, data[0] = %x, data[1]= %x, data[2]= %x, data[3]= %x", leng, type, data[0], data[1], data[2], data[3]);

	data = aud_get_reouce_chinese(AUD_ID_BT_CLEAR_SUCCESS, &leng, &type);

	TRACE(6,"====chinese_audio, leng = %d, type = %d, data[0] = %x, data[1]= %x, data[2]= %x, data[3]= %x", leng,type, data[0], data[1], data[2], data[3]);


};
#endif
#endif

const char *aud_id_str[] =
{
    "[POWER_ON]",
    "[POWER_OFF]",
    "[LANGUAGE_SWITCH]",

    "[NUM_0]",
    "[NUM_1]",
    "[NUM_2]",
    "[NUM_3]",
    "[NUM_4]",
    "[NUM_5]",
    "[NUM_6]",
    "[NUM_7]",
    "[NUM_8]",
    "[NUM_9]",

    "[BT_PAIR_ENABLE]",
    "[BT_PAIRING]",
    "[BT_PAIRING_SUC]",
    "[BT_PAIRING_FAIL]",
    "[BT_CALL_REFUSE]",
    "[BT_CALL_OVER]",
    "[BT_CALL_ANSWER]",
    "[BT_CALL_HUNG_UP]",
    "[BT_CALL_INCOMING_CALL]",
    "[BT_CALL_INCOMING_NUMBER]",
    "[BT_CHARGE_PLEASE]",
    "[BT_CHARGE_FINISH]",
    "[BT_CLEAR_SUCCESS]",
    "[BT_CLEAR_FAIL]",
    "[BT_CONNECTED]",
    "[BT_DIS_CONNECT]",
    "[BT_WARNING]",
    "[BT_ALEXA_START]",
    "[BT_ALEXA_STOP]",
    "[BT_GSOUND_MIC_OPEN]",
    "[BT_GSOUND_MIC_CLOSE]",
    "[BT_GSOUND_NC]",
    "[BT_MUTE]",
    "[RING_WARNING]",
#ifdef __INTERACTION__
    "[BT_FINDME]",
#endif
/** add by pang **/
	"[BEEP_21]",
	"[BEEP_22]",
	"[BEEP_24]",
	"[BEEP_24S]",
	"[BUTTON_49]",
	"[BUTTON_50]",
	"[ANC_OFF]",
	"[ANC_ON]",
	"[AWARENESS_ON]",
	"[POWEROFF_LOWBATTERY]",
	"[DEMO_MODE]",
	"[RING_DIGITAL]",
	"[TOUCHPAD_DISABLED]",
	"[BT_OFF]",
	"[BLUETOOTH_ONE_CONNECTED]",
	"[BLUETOOTH_ONE_DISCONNECTED]",
	"[BLUETOOTH_TWO_CONNECTED]",
	"[BLUETOOTH_TWO_DISCONNECTED]",
	"[VOL_MINMAX]",
/** end add **/

//add by cai
	"[A2DP]",
	"[CALLING_MUTE]",
	"[CALLING_UNMUTE]",
//end add
};

const char *aud_id2str(UINT16 aud_id)
{
    const char *str = NULL;

    if (aud_id >= 0 && aud_id < MAX_RECORD_NUM)
    {
        str = aud_id_str[aud_id];
    }
    else
    {
        str = "[UNKNOWN]";
    }

    return str;
}

