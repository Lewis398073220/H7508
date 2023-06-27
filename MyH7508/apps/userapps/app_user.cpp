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
#ifdef MEDIA_PLAYER_SUPPORT
#include "app_media_player.h"
#endif
#include "iir_process.h"


#if defined(__EVRCORD_USER_DEFINE__)
static uint8_t sleep_time = DEFAULT_SLEEP_TIME;
static uint16_t auto_poweroff_time = DEFAULT_AUTO_PWOFF_TIME;//add by cai
static uint8_t vibrate_mode = 1;
static uint8_t eq_set_index = 0;
static enum APP_ANC_MODE_STATUS anc_set_index = ANC_HIGH;
static uint8_t monitor_level = 20;
static uint8_t focus_on = 0;
static enum ANC_TOGGLE_MODE anc_toggle_mode = AncOn_AncOff_Awareness;//add by cai
static uint8_t sensor_enable = 1;
static uint8_t touch_lock = 0;
static uint8_t sidetone = 0;
static uint8_t low_latency_on = 0;//add by cai
static enum APP_ANC_MODE_STATUS anc_table_value = ANC_HIGH;
static uint8_t fota_flag = 0;
static uint8_t multipoint = 1;
static uint8_t talkmic_led = 1;
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

enum APP_ANC_MODE_STATUS app_nvrecord_anc_status_get(void)
{
	return (anc_set_index);
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

void app_eq_custom_para_get(uint8_t customization_eq_value[6])
{
	int8_t temp;	
		
	for(uint8_t i = 0; i < 6; i++){
		if(eq_custom_para.param[i].gain<0){
			temp=(int8_t)(2*(eq_custom_para.param[i].gain));
			temp*=-1;
			customization_eq_value[i]=(uint8_t)temp;
			customization_eq_value[i]|=0x80;
		}
		else{
			customization_eq_value[i] = (uint8_t)(2*(eq_custom_para.param[i].gain));
		}
		//TRACE(2,"***get customization_eq_value[%d]=%x",i,customization_eq_value[i]);
	}
}

void app_nvrecord_eq_param_set(uint8_t customization_eq_value[6])
{
	struct nvrecord_env_t *nvrecord_env;
	int8_t temp;
	uint8_t i=0;

	for(i=0;i<6;i++){
		if(customization_eq_value[i]>0x80){
			temp=(int8_t)(0x0f & customization_eq_value[i]);
			temp*=-1;
			eq_custom_para.param[i].gain=(float)temp;
			eq_custom_para.param[i].gain=(eq_custom_para.param[i].gain)/2;
			if(eq_custom_para.param[i].gain<-5)
				eq_custom_para.param[i].gain=-5;
			/*
			eq_custom_para_ancoff.param[i].gain=(float)temp;
			eq_custom_para_ancoff.param[i].gain=(eq_custom_para_ancoff.param[i].gain)/2;
			if(eq_custom_para_ancoff.param[i].gain<-5)
				eq_custom_para_ancoff.param[i].gain=-5;
			*/
		}
		else{
			temp=(int8_t)customization_eq_value[i];
			eq_custom_para.param[i].gain=(float)temp;
			eq_custom_para.param[i].gain=(eq_custom_para.param[i].gain)/2;
			if(eq_custom_para.param[i].gain>5)
				eq_custom_para.param[i].gain=5;
			/*
			eq_custom_para_ancoff.param[i].gain=(float)temp;
			eq_custom_para_ancoff.param[i].gain=(eq_custom_para_ancoff.param[i].gain)/2;
			if(eq_custom_para_ancoff.param[i].gain>5)
				eq_custom_para_ancoff.param[i].gain=5;
			*/
		}
		//TRACE(1,"***set customization_eq_value [%d]=%x",i,customization_eq_value[i]);
	}
	
#if 0// defined(AUDIO_LINEIN)
	for(i=0;i<6;i++){
		if(customization_eq_value[i]>0x80){
			temp=(int8_t)(0x0f & customization_eq_value[i]);
			temp*=-1;
			eq_custom_para_linein.param[i].gain=(float)temp;
			eq_custom_para_linein.param[i].gain=(eq_custom_para_linein.param[i].gain)/2;
			if(eq_custom_para_linein.param[i].gain<-5)
				eq_custom_para_linein.param[i].gain=-5;
		}
		else{
			temp=(int8_t)customization_eq_value[i];
			eq_custom_para_linein.param[i].gain=(float)temp;
			eq_custom_para_linein.param[i].gain=(eq_custom_para_linein.param[i].gain)/2;
			if(eq_custom_para_linein.param[i].gain>5)
				eq_custom_para_linein.param[i].gain=5;
		}
		//TRACE(1,"***set customization_eq_value [%d]=%x",i,customization_eq_value[i]);
	}
#endif

	nv_record_env_get(&nvrecord_env);
	for(i=0;i<6;i++){
		nvrecord_env->iir_gain[i] = customization_eq_value[i];
	}	
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
}

uint8_t get_sleep_time(void)
{
	return (sleep_time);
}

void app_nvrecord_sleep_time_set(uint8_t sltime)
{
	if(sltime == 0x00)
		sleep_time = SLEEP_TIME_PERM;
	else if(sltime == 0x03)
		sleep_time = SLEEP_TIME_10MIN;
	else if(sltime == 0x02)
		sleep_time = SLEEP_TIME_5MIN;
	else
		sleep_time = SLEEP_TIME_3MIN;

	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->sleep_time = sleep_time;
	nv_record_env_set(nvrecord_env);

#if FPGA==0
	nv_record_flash_flush();
#endif
}

uint8_t app_get_sleep_time(void)
{
	if(sleep_time == SLEEP_TIME_PERM) 
		return (0x00);
	else if(sleep_time == SLEEP_TIME_10MIN)
		return (0x03); 
	else if(sleep_time == SLEEP_TIME_5MIN)
		return (0x02);
    else	
		return (0x01);
}

uint8_t app_get_auto_poweroff(void)
{
	if(auto_poweroff_time == AUTO_PWOFF_TIME_PERM)
		return (0x00);
	else if(auto_poweroff_time == AUTO_PWOFF_TIME_30MIN)
		return (0x01); 
	else if(auto_poweroff_time == AUTO_PWOFF_TIME_1HOUR)
		return (0x02);
	else if(auto_poweroff_time == AUTO_PWOFF_TIME_2HOUR)
		return (0x03);
	else if(auto_poweroff_time == AUTO_PWOFF_TIME_4HOUR)
		return (0x04);
	else	
		return (0x05);
}	

uint16_t get_auto_pwoff_time(void)
{
	return (auto_poweroff_time);
}

void app_auto_poweroff_set(uint16_t pftime)
{
   if(pftime == 0x00)
		auto_poweroff_time = AUTO_PWOFF_TIME_PERM;
   else if(pftime == 0x01)
   		auto_poweroff_time = AUTO_PWOFF_TIME_30MIN;
   else if(pftime == 0x02)
		auto_poweroff_time = AUTO_PWOFF_TIME_1HOUR;
   else if(pftime == 0x03)
		auto_poweroff_time = AUTO_PWOFF_TIME_2HOUR;
   else if(pftime == 0x04)
		auto_poweroff_time = AUTO_PWOFF_TIME_4HOUR;
   else
   		auto_poweroff_time = AUTO_PWOFF_TIME_6HOUR;
}

uint8_t app_get_touchlock(void)
{
	return (touch_lock);
}

void app_nvrecord_touchlock_set(uint8_t on)
{
   	touch_lock=on;
}

uint8_t app_get_sidetone(void)
{
	return (sidetone);
}

void app_nvrecord_sidetone_set(uint8_t on)
{
   	sidetone=on;
   
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->sidetone = sidetone;
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
}

uint8_t app_get_low_latency_status(void)
{
	return (low_latency_on);
}

void app_low_latency_set(uint8_t on)
{
	low_latency_on = on;
}

static uint8_t new_multipoint=1;
uint8_t app_get_new_multipoint_flag(void)
{
	return (new_multipoint);
}	

uint8_t app_get_multipoint_flag(void)
{
    if(new_multipoint)
		return 1;
	else
		return (multipoint);
}

void app_nvrecord_multipoint_set(uint8_t on)
{
   	new_multipoint=on;
   
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->multipoint = on;
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
    if(new_multipoint){
		app_multipoint_api_set_on();
    }
}

enum ANC_TOGGLE_MODE app_nvrecord_anc_toggle_mode_get(void)
{
	return(anc_toggle_mode);
}

void app_nvrecord_anc_toggle_mode_set(enum ANC_TOGGLE_MODE nc_toggle)
{	
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	
	if(nc_toggle>=0x00 && nc_toggle<0x04){ 
		nvrecord_env->anc_toggle_mode = nc_toggle;
		anc_toggle_mode = nc_toggle;
	}else return;
	nv_record_env_set(nvrecord_env);

#if FPGA==0
    nv_record_flash_flush();
#endif
}

void app_nvrecord_language_set(uint8_t lang)
{
	struct nvrecord_env_t *nvrecord_env;
	
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->media_language.language = lang;
	app_play_audio_set_lang(lang);
	nv_record_env_set(nvrecord_env);
	
#if FPGA==0
    nv_record_flash_flush();
#endif
}

void app_nvrecord_para_get(void)
{
	struct nvrecord_env_t *nvrecord_env;
	uint8_t i=0;
	int8_t temp;
	uint8_t igain=0;
	
	nv_record_env_get(&nvrecord_env);

	sleep_time = nvrecord_env->sleep_time;
	vibrate_mode = nvrecord_env->vibrate_mode;
	eq_set_index = nvrecord_env->eq_mode;
	anc_set_index = (enum APP_ANC_MODE_STATUS)nvrecord_env->anc_mode;
	monitor_level = nvrecord_env->monitor_level;
	focus_on = nvrecord_env->focus_on;
	sensor_enable = nvrecord_env->sensor_enable;
	touch_lock = nvrecord_env->touch_lock;
	sidetone = nvrecord_env->sidetone;
	anc_table_value = (enum APP_ANC_MODE_STATUS)nvrecord_env->anc_table_value;
	fota_flag = nvrecord_env->fota_flag;
	multipoint = nvrecord_env->multipoint;
	new_multipoint = multipoint;
	talkmic_led = nvrecord_env->talkmic_led;
	auto_poweroff_time = DEFAULT_AUTO_PWOFF_TIME;//add by cai
	anc_toggle_mode = (enum ANC_TOGGLE_MODE)nvrecord_env->anc_toggle_mode;//add by cai
	low_latency_on = 0;//add by cai

	for(i = 0; i < 6; i++){
		igain=nvrecord_env->iir_gain[i];
		if(igain > 0x80){
			temp = (int8_t)(0x0f & igain);
			temp *= -1;
			eq_custom_para.param[i].gain = (float)temp;		
			eq_custom_para.param[i].gain = (eq_custom_para.param[i].gain)/2;
			if(eq_custom_para.param[i].gain < -5)
				eq_custom_para.param[i].gain = -5;		
		}
		else{
			temp = (int8_t)igain;
			eq_custom_para.param[i].gain = (float)temp;
			eq_custom_para.param[i].gain = (eq_custom_para.param[i].gain)/2;
			if(eq_custom_para.param[i].gain > 5)
				eq_custom_para.param[i].gain = 5;
		}
		TRACE(3,"***%s: customization_eq_gain[%d]=%x",__func__,i,nvrecord_env->iir_gain[i]);		
	}
#if defined(AUDIO_LINEIN)
	for(i=0;i<6;i++){
		igain=nvrecord_env->iir_gain[i];
		if(igain>0x80){
			temp=(int8_t)(0x0f & igain);
			temp*=-1;
			eq_custom_para_linein.param[i].gain=(float)temp/100;		
			eq_custom_para_linein.param[i].gain=(eq_custom_para_linein.param[i].gain)/2;
			if(eq_custom_para_linein.param[i].gain<-5)
				eq_custom_para_linein.param[i].gain=-5;
		}
		else{
			temp=(int8_t)igain;
			eq_custom_para_linein.param[i].gain=(float)temp/100;		
			eq_custom_para_linein.param[i].gain=(eq_custom_para_linein.param[i].gain)/2;
			if(eq_custom_para_linein.param[i].gain>5)
				eq_custom_para_linein.param[i].gain=5;
		}
			TRACE(1,"***get customization_linein_eq_gain [%d]=%x",i,nvrecord_env->iir_gain[i]);		
	}
#endif

#if defined(CUSTOM_BIN_CONFIG)
	app_get_custom_bin_config();//for debug
#endif

	TRACE(6,"sleep_time=%d, eq_set_index=%d, monitor_level=%d, focus_on=%d, multipoint=%d, auto_poweroff_time=%d",sleep_time,eq_set_index,monitor_level,focus_on,multipoint,auto_poweroff_time);
}

#endif

#if defined(CUSTOM_BIN_CONFIG)
extern uint32_t __custom_bin_start[];
uint8_t binconfig[1]={0x00};

void app_get_custom_bin_config(void)
{		
	memcpy(binconfig,(const void *)0x383e9000,1);//color, add by cai

	TRACE(2,"***%s binconfig[0]=%d",__func__, binconfig[0]);
}

uint8_t get_custom_bin_config(uint8_t config_num)
{	
    if(config_num==0)
		return (binconfig[0]);
	else
		return 0xff;//invalid value
}

void set_custom_bin_config(uint8_t config_num,uint8_t binconfig_value)
{	
    if(config_num==0)
		binconfig[0]=binconfig_value;
	//else
		//return (binconfig[1]);
}
#endif

