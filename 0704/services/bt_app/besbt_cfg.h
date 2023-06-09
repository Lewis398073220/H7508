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
/***
 * besbt_cfg.h
 */

#ifndef BESBT_CFG_H
#define BESBT_CFG_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct besbt_cfg_t{
    bool sniff;
    bool one_bring_two;
    bool support_enre_mode;
    bool bt_hid_enable;
    bool avdtp_cp_enable;
    bool source_enable;
    bool avdtp_tg_enable;
    bool ldac_interop_let_phone_connect;
    bool close_avdtp_mi5_interop_workaound;
    bool lhdc_v3;
};
extern struct besbt_cfg_t besbt_cfg;

#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
