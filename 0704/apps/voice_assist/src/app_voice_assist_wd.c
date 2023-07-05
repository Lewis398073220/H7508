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
#if defined(VOICE_ASSIST_OUT_EAR_DETECTION)
#include "hal_trace.h"
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_voice_assist_wd.h"
#include "cmsis.h"
#include "arm_math.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "app_media_player.h"

static int32_t _voice_assist_wd_callback(void *buf, uint32_t len, void *other);

#define WD_HISTORY_LEN (360)
static uint8_t history_status[WD_HISTORY_LEN] = {0};
static int32_t current_ear_status[2] = {0};
static int32_t is_running = 0;
static HWTIMER_ID timer_wd_id = NULL;
static int32_t hw_timer_is_working = 0;

#define WD_TIMER_PERIOD (10000)

static void app_voice_assist_wd_monitor_callback(void *param){
    if(is_running == 1){
        // do nothing, skip
    } else {
        app_voice_assist_wd_open();
    }
    hwtimer_start(timer_wd_id, MS_TO_HWTICKS(WD_TIMER_PERIOD));    
    return ;
}

int32_t app_voice_assist_wd_monitor_init(void){
    // init hw timer
    timer_wd_id = hwtimer_alloc(app_voice_assist_wd_monitor_callback, NULL);
    return 0;
}

static void wd_reset_history(void){
    for(int i = 0; i<WD_HISTORY_LEN;i++){
        history_status[i] = 1;
    }
}

int32_t app_voice_assist_wd_monitor_start(void){
    // start timer
    if(hw_timer_is_working == 0){
        hw_timer_is_working = 1;
        hwtimer_start(timer_wd_id, MS_TO_HWTICKS(WD_TIMER_PERIOD));
        wd_reset_history();
    } else {
        TRACE(0,"%s hw timer is working, skip start",__func__);
    }
    
    return 0;
}

int32_t app_voice_assist_wd_monitor_stop(void){
    if(is_running == 1){
        app_voice_assist_wd_close(0);
    }
    if(hw_timer_is_working == 1){
        // stop hw timer
        hw_timer_is_working = 0;
        hwtimer_stop(timer_wd_id);
    } else {
        TRACE(0,"%s hw timer is stop, skip stop",__func__);
    }
    return 0;
}

int32_t app_voice_assist_wd_init(void)
{
    app_anc_assist_register(ANC_ASSIST_USER_WD, _voice_assist_wd_callback);

    return 0;
}

int32_t app_voice_assist_wd_open(void)
{
    TRACE(0,"!!! %s",__func__);
    is_running = 1;
    current_ear_status[0] = -1;
    current_ear_status[1] = -1;
    app_anc_assist_open(ANC_ASSIST_USER_WD);
    // media_PlayAudio_locally(AUD_ID_WEAR_DETECT,0);
    media_PlayAudio(AUD_ID_WEAR_DETECT,0);
    return 0;
}

static int32_t wd_long_term_decision(uint8_t * data, int32_t data_len){
    // 90% out ear for 1 hour data
    float cnt = 0;
    for(int i = 0; i < data_len; i++){
        cnt += data[i];
    }
    TRACE(0,"%s wd_process long term = %d",__func__,(int)(cnt));
    if(cnt < data_len * 0.1){
        return 1;
    } else {
        return 0;
    }
}

static int32_t wd_middle_term_decision(uint8_t * data, int32_t data_len){
    // 95% out ear for 15 min data
    float cnt = 0;
    for(int i = 0; i < data_len; i++){
        cnt += data[i];
    }
    TRACE(0,"%s wd_process middle term = %d",__func__,(int)(cnt));
    if(cnt < data_len * 0.05){
        return 1;
    } else {
        return 0;
    }
}

static int32_t wd_short_term_decision(uint8_t * data, int32_t data_len){
    // 100% out ear for 4 min data
    int32_t cnt = 0;
    for(int i = 0; i < data_len; i++){
        cnt += data[i];
    }
    TRACE(0,"%s wd_process short term = %d",__func__,(int)(cnt));
    if(cnt == 0){
        return 1;
    } else {
        return 0;
    }
}


/* 
   decision table
    0  |  1  |  -1
0   OFF   ON     OFF 
1   ON    ON     ON
-1  OFF   ON     ON
*/
int32_t app_voice_assist_wd_close(int32_t is_normal_close)
{
    if(is_running == 1){
        TRACE(0,"%s ...",__func__);
        is_running = 0;
        if(is_normal_close){
            TRACE(0,"%s normal, start summary ...",__func__);
            for(int i = 0; i<WD_HISTORY_LEN-1;i++){
                history_status[i] = history_status[i+1];
            }
            if(current_ear_status[0] == current_ear_status[1]){
                if(current_ear_status[0] == -1){
                    current_ear_status[0] = 1;
                    current_ear_status[1] = 1;
                }
                TRACE(0,"%s left = right = %d",__func__,current_ear_status[0]);
                history_status[WD_HISTORY_LEN-1] = current_ear_status[0];
            } else {
                TRACE(0,"%s left != right %d %d",__func__,current_ear_status[0],current_ear_status[1]);
                if(current_ear_status[0] == -1){
                    history_status[WD_HISTORY_LEN-1] = current_ear_status[1];
                } else if(current_ear_status[1] == -1){
                    history_status[WD_HISTORY_LEN-1] = current_ear_status[0];
                } else {
                    history_status[WD_HISTORY_LEN-1] = 1;
                }
                
                // do nothing
            }
            TRACE(0,"%s make decision",__func__);
            int flag_long = wd_long_term_decision(history_status,WD_HISTORY_LEN) ;
            int flag_middle = wd_middle_term_decision(&history_status[WD_HISTORY_LEN-WD_HISTORY_LEN/2],WD_HISTORY_LEN/2);
            int flag_short = wd_short_term_decision(&history_status[WD_HISTORY_LEN-WD_HISTORY_LEN/4],WD_HISTORY_LEN/4);
            if(flag_long && flag_middle && flag_short){
                // stop anc callback
                TRACE(0,"wd_process , anc callback");
            } else {
                // do nothing
            }
        } else {
            TRACE(0,"%s interrupted, skip summary ...",__func__);
        }

        app_anc_assist_close(ANC_ASSIST_USER_WD);
    } else {
        TRACE(0,"%s avoid double close",__func__);
    }
   

    return 0;
}

extern bool g_gain_switch_flag;
static int32_t _voice_assist_wd_callback(void *buf, uint32_t len, void *other)
{
    // TRACE(0, "[%s] len = %d", __func__, len);

    uint32_t *res = (uint32_t *)buf;

    anc_assist_algo_status_t wd_changed = res[0];
    wd_status_t wd_status_L = res[1];
    wd_status_t wd_status_R = res[2];

    if (wd_changed == ANC_ASSIST_ALGO_STATUS_CHANGED && is_running) {
        if (wd_status_L == WD_STATUS_IN_EAR) {
            TRACE(0, "[%s] FB %d change to WD_STATUS_IN_EAR", __func__, 0);
            current_ear_status[0] = 1;
        } 
        if (wd_status_L == WD_STATUS_OUT_EAR) {
            TRACE(0, "[%s] FB %d change to WD_STATUS_OUT_EAR", __func__, 0);
            current_ear_status[0] = 0;
        }
        if (wd_status_L == WD_STATUS_IDLE) {
            TRACE(0, "[%s] FB %d change to WD_STATUS_IDLE", __func__, 0);
        }
        if (wd_status_R == WD_STATUS_IN_EAR) {
            TRACE(0, "[%s] FB %d change to WD_STATUS_IN_EAR", __func__, 1);
            current_ear_status[1] = 1;
        } 
        if (wd_status_R == WD_STATUS_OUT_EAR) {
            TRACE(0, "[%s] FB %d change to WD_STATUS_OUT_EAR", __func__, 1);
            current_ear_status[1] = 0;
        }
        if (wd_status_R == WD_STATUS_IDLE) {
            TRACE(0, "[%s] FB %d change to WD_STATUS_IDLE", __func__, 1);
        }
        app_voice_assist_wd_close(1);


    } 

    return 0;
}
#endif
