#ifndef __USER_APPS_H__
#define __USER_APPS_H__

#include "../../../apps/anc/inc/app_anc.h"

#if defined(__EVRCORD_USER_DEFINE__)
#include "nvrecord_env.h"
#include "../../../services/multimedia/audio/process/filters/include/iir_process.h"

extern IIR_CFG_T eq_custom_para;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__EVRCORD_USER_DEFINE__)
uint8_t app_get_fota_flag(void);
void app_nvrecord_fotaflag_set(uint8_t on);
uint8_t app_nvrecord_anc_table_get(void);
void app_nvrecord_anc_set(enum APP_ANC_MODE_STATUS nc);
uint8_t app_get_monitor_level(void);
void app_nvrecord_monitor_level_set(uint8_t level);
uint8_t app_get_focus(void);
void app_focus_set_no_save(uint8_t focus);
void app_nvrecord_focus_set(uint8_t focus);
uint8_t app_eq_index_get(void);
void app_eq_index_set_nosave(uint8_t eq_index);
void app_nvrecord_eq_set(uint8_t eq_index);


#endif

#ifdef __cplusplus
	}//extern "C" {
#endif

#endif
