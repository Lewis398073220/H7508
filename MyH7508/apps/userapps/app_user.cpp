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
#include "app_anc.h"
#include "iir_process.h"


#if defined(__EVRCORD_USER_DEFINE__)
static uint8_t eq_set_index = 0;
static uint8_t fota_flag = 0;
static enum APP_ANC_MODE_STATUS anc_set_index = ANC_HIGH;
static uint8_t monitor_level = 20;
static uint8_t focus_on = 0;
static enum APP_ANC_MODE_STATUS anc_table_value = ANC_HIGH;
#endif

#if defined(__EVRCORD_USER_DEFINE__)
IIR_CFG_T eq_custom_para={
    .gain0 = -6,
    .gain1 = -6,
    .num = 6,
    .param = {
        {IIR_TYPE_PEAK, .0,   100, 0.7},
        {IIR_TYPE_PEAK, .0,   400, 0.7},
        {IIR_TYPE_PEAK, .0,  1000, 0.7},
        {IIR_TYPE_PEAK, .0,  2500, 0.7},
        {IIR_TYPE_PEAK, .0,  6300, 0.7},
		{IIR_TYPE_PEAK, .0, 12000, 0.7},
	}
};

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

uint8_t app_nvrecord_anc_table_get(void)
{
	return(anc_table_value);
}

void app_nvrecord_anc_set(enum APP_ANC_MODE_STATUS nc)
{	
	anc_set_index=nc;
	
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->anc_mode = nc;
	if(nc > NC_OFF && nc < MONITOR_ON){ //m by cai
		nvrecord_env->anc_table_value = nc;
		anc_table_value=nc;
	}
	nv_record_env_set(nvrecord_env);

#if FPGA==0
    nv_record_flash_flush();
#endif
}

uint8_t app_get_monitor_level(void)
{
	return (monitor_level);
}

void app_nvrecord_monitor_level_set(uint8_t level)
{
   	monitor_level=level;
   
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->monitor_level = monitor_level;
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
}

uint8_t app_get_focus(void)
{
	return (focus_on);
}

void app_focus_set_no_save(uint8_t focus)//add by cai
{
	focus_on=focus;
}

void app_nvrecord_focus_set(uint8_t focus)
{
   	focus_on=focus;
   
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->focus_on = focus_on;
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
}

uint8_t app_eq_index_get(void)
{   
	return (eq_set_index);
}

void app_eq_index_set_nosave(uint8_t eq_index)
{   
	eq_set_index = eq_index;
}

void app_nvrecord_eq_set(uint8_t eq_index)
{
    eq_set_index=eq_index;
		
	struct nvrecord_env_t *nvrecord_env;
			
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->eq_mode=eq_set_index;
	nv_record_env_set(nvrecord_env);

#if FPGA==0
    nv_record_flash_flush();
#endif
}

#endif
