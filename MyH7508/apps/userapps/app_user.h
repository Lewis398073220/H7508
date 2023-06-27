#ifndef __USER_APPS_H__
#define __USER_APPS_H__

#include "../../../apps/anc/inc/app_anc.h"

#if defined(__EVRCORD_USER_DEFINE__)
#include "nvrecord_env.h"
#include "../../../services/multimedia/audio/process/filters/include/iir_process.h"

#define SLEEP_TIME_3MIN  24
#define SLEEP_TIME_5MIN  39//39
#define SLEEP_TIME_10MIN 77
#define SLEEP_TIME_PERM  255
#define DEFAULT_SLEEP_TIME SLEEP_TIME_5MIN

#define AUTO_PWOFF_TIME_30MIN  212
#define AUTO_PWOFF_TIME_1HOUR  431
#define AUTO_PWOFF_TIME_2HOUR  862
#define AUTO_PWOFF_TIME_4HOUR  1724
#define AUTO_PWOFF_TIME_6HOUR  2586
#define AUTO_PWOFF_TIME_PERM   4095
#define DEFAULT_AUTO_PWOFF_TIME AUTO_PWOFF_TIME_PERM

extern IIR_CFG_T eq_custom_para;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__EVRCORD_USER_DEFINE__)
uint8_t app_get_fota_flag(void);
void app_nvrecord_fotaflag_set(uint8_t on);
enum APP_ANC_MODE_STATUS app_nvrecord_anc_status_get(void);
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
void app_eq_custom_para_get(uint8_t customization_eq_value[6]);
void app_nvrecord_eq_param_set(uint8_t customization_eq_value[6]);
uint8_t get_sleep_time(void);
void app_nvrecord_sleep_time_set(uint8_t sltime);
uint8_t app_get_sleep_time(void);
uint8_t app_get_auto_poweroff(void);
uint16_t get_auto_pwoff_time(void);
void app_auto_poweroff_set(uint16_t pftime);
uint8_t app_get_touchlock(void);
void app_nvrecord_touchlock_set(uint8_t on);
uint8_t app_get_sidetone(void);
void app_nvrecord_sidetone_set(uint8_t on);
uint8_t app_get_low_latency_status(void);
void app_low_latency_set(uint8_t on);
uint8_t app_get_new_multipoint_flag(void);
uint8_t app_get_multipoint_flag(void);
void app_nvrecord_multipoint_set(uint8_t on);
enum ANC_TOGGLE_MODE app_nvrecord_anc_toggle_mode_get(void);
void app_nvrecord_anc_toggle_mode_set(enum ANC_TOGGLE_MODE nc_toggle);
void app_nvrecord_language_set(uint8_t lang);
void app_nvrecord_para_get(void);
#endif

#if defined(CUSTOM_BIN_CONFIG)
void app_get_custom_bin_config(void);
uint8_t get_custom_bin_config(uint8_t config_num);
void set_custom_bin_config(uint8_t config_num,uint8_t binconfig_value);//add by cai
#endif

#ifdef __cplusplus
	}//extern "C" {
#endif

#endif
