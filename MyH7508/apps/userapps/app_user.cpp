#include "stdio.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "string.h"
#include "hal_trace.h"
#include "apps.h"
#include "app_bt.h"
#include "app_thread.h"
#include "tgt_hardware.h"
#include "app_user.h"
#include "philips_ble_api.h"
#include "app_bt_stream.h"
#include "bt_sco_chain.h"
#include "hal_codec.h"
#include "nvrecord_env.h"

#if defined(__EVRCORD_USER_DEFINE__)
static uint8_t fota_flag = 0;
#endif

#if defined(__EVRCORD_USER_DEFINE__)
uint8_t app_get_fota_flag(void)
{
	return (fota_flag);
}

void app_nvrecord_fotaflag_set(uint8_t on)
{
   	fota_flag=on;
   
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->fota_flag = fota_flag;
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
}

#endif
