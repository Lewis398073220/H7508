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
#include "plat_types.h"
#include "anc_assist.h"

#if defined (VOICE_ASSIST_CUSTOM_LEAK_DETECT)
const static int16_t custom_leak_detect_pcm_data[] = {
   #include "res/ld/tone_peco.h"
};
#else 
const static int16_t custom_leak_detect_pcm_data[] = {
   0,0,0
};
#endif

AncAssistConfig anc_assist_cfg = {
    .bypass = 0,
    .debug_en = 0,
    .howling_debug_en = 0,
    .wind_debug_en = 0,

    .ff_howling_en  = 0,
    .fb_howling_en  = 0,
    .noise_en   = 0,
    .wind_en    = 0,
    .wind_single_mic_en = 0,
	.pilot_en   = 0,
	.pnc_en     = 0,
    .wsd_en     = 0,
    .extern_kws_en     = 0,

    .ff_howling_cfg = {
        .ind0 = 24,         // 62.5Hz per 62.5*32=2k
        .ind1 = 80,        // 62.5*120=7.5k
        .time_thd = 1000,    // ff recover time 500*7.5ms=3.75s
        .power_thd = 1e7f,
    },

    .fb_howling_cfg = {
        .ind0 = 24,         // 62.5Hz per 62.5*32=2k
        .ind1 = 80,        // 62.5*120=7.5k
        .time_thd = 1000,    // ff recover time 500*7.5ms=3.75s
        .power_thd = 1e8f,
    },

    .noise_cfg = {
        .lower_low_thd = 4000,
        .lower_mid_thd = 12000, 
		.quiet_low_thd = 3000,
        .quiet_mid_thd =4000,
        .snr_thd = 100,
        .period = 16,
        .window_size = 5,
        .strong_to_lower = 16,//10s
        .low_to_stronger = 2,//2s
        .band_freq = {100, 400, 1000, 2000},
        .band_weight = {0.7, 0.3, 0},
    },

    .wind_cfg = {
        .scale_size = 16,           // freq range,8/scale=1k
        .to_none_targettime = 500, // time=500*7.5ms=3.75s
        .to_wind_targettime = 300,
        .power_thd = 0.0001, 
		.no_thd = 0.8,
		.small_thd = 0.7,//wind_speed=2m
		.normal_thd = 0.3,//wind_speed=4m
		.strong_thd = 0.1,//wind_speed>4m
		.gain_none = 1.0,
		.gain_small_to_none = 1,
		.gain_small = 0.5,
		.gain_normal_to_small = 0.5,
		.gain_normal = 0,
		.gain_strong_to_normal = 0,
		.gain_strong = 0,
    },

    .pilot_cfg = {
        .dump_en = 0,
	    .delay = 406,
#ifdef VOICE_ASSIST_WD_ENABLED
	    .cal_period = 25,
#else
        .cal_period = 25,
#endif
        .gain_smooth_ms = 300,

        .adaptive_anc_en = 0,
	    .thd_on = {5.6400,  1.5648,  0.6956,  0.4672,  0.4089,  0.2694, 0.1853,0.1373,0.0629,0.0325,0.0043},
	    .thd_off = {5.6400,  1.5648,  0.6956,  0.4672,  0.4089,  0.2694, 0.1853,0.1373,0.0629,0.0325,0.0043},

        .wd_en =0,
        .inear_thd = 0.08,
        .outear_thd = 0.03, 
        .energy_cmp_num = 5, //
        .skip_energy_num = 1,
        .wd_pilot_gain = 1.0,

        .infra_ratio = 0.7,
        .ultrasound_stable_tick = 2,
        .ultrasound_stop_tick = 3,
        .ultrasound_thd = 0.1,

        .custom_leak_detect_en = 0,
        .custom_leak_detect_playback_loop_num = 2,
        .custom_pcm_data = custom_leak_detect_pcm_data,
        .custom_pcm_data_len = sizeof(custom_leak_detect_pcm_data) / sizeof(int16_t),
        .gain_local_pilot_signal = 0.0,
    },

    .pnc_cfg = {
        .pnc_lower_bound = 22,
        .pnc_upper_bound = 25,
	    .out_lower_bound = 34,
	    .out_upper_bound = 38,
        .cal_period = 25,
        .out_thd = 8.0,
    },
	
	.prompt_cfg = {
        .dump_en = 1,
	    .cal_period = 10,
        .curve_num = 10,
        .max_env_energy = 1e9,
        .start_index = 1,
        .end_index = 10,
        .freq_point1 = 275,
        .freq_point2 = 460,
        .freq_point3 = 550,
        .band1_calib = 0.0,
        .band2_calib = 0.0,
        .band3_calib = 0.0,
        .thd1 = {1,0,-2,-4,-6,-8,-10,-12,-14,-16},
        .thd2 = {1.5,-0.5,-2.5,-4.5,-6.5,-8.5,-10.5,-12.5,-14.5,-16.5},
        .thd3 = {1,-1,-3,-5,-7,-9,-11,-13,-15,-17}
    },

};