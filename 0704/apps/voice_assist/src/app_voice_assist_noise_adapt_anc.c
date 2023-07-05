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
#include "hal_trace.h"
#include "app_anc_assist.h"
#include "anc_assist.h"
#include "anc_process.h"
#include "app_voice_assist_noise_adapt_anc.h"
extern AncAssistConfig anc_assist_cfg;
static int32_t _voice_assist_noise_adapt_anc_callback(void *buf, uint32_t len, void *other);

int32_t app_voice_assist_noise_adapt_anc_init(void)
{
    app_anc_assist_register(ANC_ASSIST_USER_NOISE_ADAPT_ANC, _voice_assist_noise_adapt_anc_callback);
    return 0;
}

static int32_t noise_status_cache[2] = {0};
int32_t app_voice_assist_noise_adapt_anc_open(void)
{
    TRACE(0, "[%s] noise adapt anc start stream", __func__);
    app_anc_assist_open(ANC_ASSIST_USER_NOISE_ADAPT_ANC);
    noise_status_cache[0] = NOISE_STATUS_MIDDLE_ANC;
    noise_status_cache[1] = NOISE_STATUS_MIDDLE_ANC;
    return 0;
}



int32_t app_voice_assist_noise_adapt_anc_close(void)
{
    TRACE(0, "[%s] noise adapt anc close stream", __func__);
    app_anc_assist_close(ANC_ASSIST_USER_NOISE_ADAPT_ANC);
    return 0;
}

static int32_t _voice_assist_noise_adapt_anc_callback(void * buf, uint32_t len, void *other)
{
    uint32_t *res = (uint32_t *)buf;
    AncAssistRes * assist_res = (AncAssistRes *)other;
    if(res[0]){
        noise_status_cache[0] = res[1];
    }
    if(res[2]){
        noise_status_cache[1] = res[3];
    }

    TRACE(0, "noise status L = %d, R = %d",noise_status_cache[0], noise_status_cache[1]);
    
    int32_t stereo_mode;
    if(noise_status_cache[0]<= noise_status_cache[1]){
        stereo_mode = noise_status_cache[0];
    } else {
        stereo_mode = noise_status_cache[1];
    }
    assist_res->fb_gain_changed[0] = 1;
    assist_res->ff_gain_changed[0] = 1;
    assist_res->fb_gain_changed[1] = 1;
    assist_res->ff_gain_changed[1] = 1;
    if(stereo_mode == NOISE_STATUS_LOWER_ANC){
        assist_res->ff_gain[0] = 1.0;
        assist_res->ff_gain[1] = 1.0;
        assist_res->fb_gain[0] = 0.0;
        assist_res->fb_gain[1] = 0.0;
    } else if(stereo_mode == NOISE_STATUS_MIDDLE_ANC){
        assist_res->ff_gain[0] = 1.0;
        assist_res->ff_gain[1] = 1.0;
        assist_res->fb_gain[0] = 0.3;
        assist_res->fb_gain[1] = 0.3;
    } else if(stereo_mode == NOISE_STATUS_QUIET_ANC){
        assist_res->ff_gain[0] = 0.0;
        assist_res->ff_gain[1] = 0.0;
        assist_res->fb_gain[0] = 0.0;
        assist_res->fb_gain[1] = 0.0;
    } else {
        TRACE(0,"adapt noise not support this noise");
    }



    return 0;
}
