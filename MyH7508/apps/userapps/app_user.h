#ifndef __USER_APPS_H__
#define __USER_APPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__EVRCORD_USER_DEFINE__)
uint8_t app_get_fota_flag(void);
void app_nvrecord_fotaflag_set(uint8_t on);
#endif

#ifdef __cplusplus
	}//extern "C" {
#endif

#endif
