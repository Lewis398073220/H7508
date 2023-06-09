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
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_chipid.h"
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_2300_internal.h"

extern void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length);

///enable m4 patch func
#define BTDRV_PATCH_EN_REG   0xe0002000

//set m4 patch remap adress
#define BTDRV_PATCH_REMAP_REG    0xe0002004

////instruction patch compare src address
#define BTDRV_PATCH_INS_COMP_ADDR_START   0xe0002008

#define BTDRV_PATCH_INS_REMAP_ADDR_START   0xc0000100

////data patch compare src address
#define BTDRV_PATCH_DATA_COMP_ADDR_START   0xe00020e8

#define BTDRV_PATCH_DATA_REMAP_ADDR_START   0xc00001e0



#define BTDRV_PATCH_ACT   0x1
#define BTDRV_PATCH_INACT   0x0



typedef struct
{
    uint8_t patch_index;   //patch position
    uint8_t patch_state;   //is patch active
    uint16_t patch_length;     ///patch length 0:one instrution replace  other:jump to ram to run more instruction
    uint32_t patch_remap_address;   //patch occured address
    uint32_t patch_remap_value;      //patch replaced instuction
    uint32_t patch_start_address;    ///ram patch address for lenth>0
    uint8_t *patch_data;                  //ram patch date for length >0

} BTDRV_PATCH_STRUCT;

///////////////////ins  patch ..////////////////////////////////////



const uint32_t bes2300_patch0_ins_data[] =
{
    0x68134a04,
    0x3300f423,
    0x46206013,
    0xfa48f628,
    0xbcbdf618,
    0xd02200a4
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data),
    0x0001ed88,
    0xbb3af1e7,
    0xc0006400,
    (uint8_t *)bes2300_patch0_ins_data
};/////test mode



const uint32_t bes2300_patch1_ins_data[] =
{
    0x52434313,
    0x03fff003,
    0xd0012b04,
    0x47702000,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch1_ins_data),
    0x00003044,
    0xb9ecf203,
    0xc0006420,
    (uint8_t *)bes2300_patch1_ins_data
};//inc power



const uint32_t bes2300_patch2_ins_data[] =
{
    0xf0035243,
    0x2b0003ff,
    0x2000d001,
    0x20014770,
    0xbf004770,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch2_ins_data),
    0x00003014,
    0xba14f203,
    0xc0006440,
    (uint8_t *)bes2300_patch2_ins_data
};//dec power


const BTDRV_PATCH_STRUCT bes2300_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    0,
    0x00000494,
    0xbD702000,
    0,
    NULL
};//sleep


const uint32_t bes2300_patch4_ins_data[] =
{
    0x0008f109,
    0xbf0060b0,
    0xba30f626,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_data),
    0x0002c8c8,
    0xbdcaf1d9,
    0xc0006460,
    (uint8_t *)bes2300_patch4_ins_data
};//ld data tx


const BTDRV_PATCH_STRUCT bes2300_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x00019804,
    0xe0092b01,
    0,
    NULL
};//max slot req



const uint32_t bes2300_patch6_ins_data[] =
{
    0x0301f043,
    0xd1012b07,
    0xbc8af624,
    0xbc78f624,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6=
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_data),
    0x0002ad88,
    0xbb72f1db,
    0xc0006470,
    (uint8_t *)bes2300_patch6_ins_data
};//ld data tx 3m



const BTDRV_PATCH_STRUCT bes2300_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    0,
    0x0003520c,
    0xbf00e006,
    0,
    NULL
};///disable for error


const BTDRV_PATCH_STRUCT bes2300_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00035cf4,
    0xbf00e038,
    0,
    NULL
};//bt address ack

const BTDRV_PATCH_STRUCT bes2300_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x000273a0,
    0x789a3100,
    0,
    NULL
};///no sig tx test

static const uint32_t best2300_ins_patch_config[] =
{
    10,
    (uint32_t)&bes2300_ins_patch0,
    (uint32_t)&bes2300_ins_patch1,
    (uint32_t)&bes2300_ins_patch2,
    (uint32_t)&bes2300_ins_patch3,
    (uint32_t)&bes2300_ins_patch4,
    (uint32_t)&bes2300_ins_patch5,
    (uint32_t)&bes2300_ins_patch6,
    (uint32_t)&bes2300_ins_patch7,
    (uint32_t)&bes2300_ins_patch8,
    (uint32_t)&bes2300_ins_patch9,
};


///PATCH RAM POSITION
///PATCH0   0xc0006690--C000669C
///PATCH2   0XC0006920--C0006928
///PATCH3   0XC0006930--C0006944
///PATCH4   0XC0006950--C0006958
///PATCH5   0XC0006980--C00069E4
///PATCH6   0XC00069F0--C0006A0C
///PATCH7   0XC0006A28--C0006A4C
///PATCH9   0xc0006960 --0xc0006978
///PATCH11  0xc0006ca0 --0xc0006cb4
///PATCH12  0xc0006cc0 --0xc0006cd4
///PATCH14  0XC0006800--C0006810
///PATCH15  0XC0006810--C0006820
///PATCH16  0XC0006a60--C0006a74 //(0XC00067f0--C0006800)empty
///PATCH17  0XC00068f0--C0006914
///PATCH19  0xc0006704--c00067ec
///PATCH22  0xc0006848--0xc00068e0
///PATCH25  0xc0006a18--0xc0006a20
///PATCH26  0xc0006670--0xc0006680
///PATCH28  0xc00066b0--0xc00066F8
///PATCH35  0XC0006A54--0xc0006a60
///PATCH42  0XC0006848--0xc0006864
///patch36  0xc0006a80-0xc0006a94
///patch37  0xc0006aa0 --0xc0006ab0
///patch38  0xc0006ac0--0xc0006aec
///patch40  0xc0006af0--0xc0006b00
///patch43  0xc0006be0--0xc0006bf8
///patch44  0xc0006c00--0xc0006c44
///patch46  0xc0006c50--0xc0006c8c
///patch47  0xc0006c98--0xc0006db0

#define BT_FORCE_3M
#define BT_VERSION_50
#define BT_SW_SEQ

//#define BT_DEBUG_PATCH
////
const uint32_t bes2300_patch0_ins_data_2[] =
{
    0x0008f109,
    0xbf0060b0,
    0xba12f627,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0_2 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data_2),
    0x0002dabc,
    0xbde8f1d8,
    0xc0006690,
    (uint8_t *)bes2300_patch0_ins_data_2
};//ld data tx


const BTDRV_PATCH_STRUCT bes2300_ins_patch1_2 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0000e40c,
    0xbf00e0ca,
    0,
    NULL
};///fix sniff re enter

const uint32_t bes2300_patch2_ins_data_2[] =
{
    0x22024b03,
    0x555a463d,
    0x4b022200,
    0xbe2ef622,
    0xc00067a4,
    0xc00008fc
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_2 =
{
    2,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch2_ins_data_2),
    0x00029438,
    0xb9caf1dd,
    0xc00067d0,
    (uint8_t *)bes2300_patch2_ins_data_2
};/*ld_acl_end*/


const uint32_t bes2300_patch3_ins_data_2[] =
{
    0x0080f105,
    0xf60f2101,
    0x8918fe59,
    0xfe8ef600,
    0xb90ef610,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch3_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch3_ins_data_2),
    0x00016b28,
    0xbf02f1ef,
    0xc0006930,
    (uint8_t *)bes2300_patch3_ins_data_2
};//free buff

const uint32_t bes2300_patch4_ins_data_2[] =
{
    0x091ff04f,
    0xbd93f628,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch4_2 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_data_2),
    0x0002f474,
    0xba6cf1d7,
    0xc0006950,
    (uint8_t *)bes2300_patch4_ins_data_2
};//curr prio assert




const uint32_t bes2300_patch5_ins_data_2[] =
{
    0x3002f898,
    0x03c3eb07,
    0x306cf893,
    0x4630b333,
    0xf60b2100,
    0xf640fedd,
    0x21000003,
    0x230e2223,
    0xf918f5fa,
    0x70042400,
    0x80453580,
    0x2002f898,
    0xeb07320d,
    0xf85707c2,
    0x687a1f04,
    0x1006f8c0,
    0x200af8c0,
    0x2002f898,
    0xf8987102,
    0x71422002,
    0xff28f63c,
    0xe8bd2000,
    0xbf0081f0,
    0xe8bd2002,
    0xbf0081f0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_2 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch5_ins_data_2),
    0x00020b00,
    0xbf3ef1e5,
    0xc0006980,
    (uint8_t *)bes2300_patch5_ins_data_2
};//slave feature rsp


const uint32_t bes2300_patch6_ins_data_2[] =
{
    0xf4038873,
    0xf5b34370,
    0xd0055f80,
    0xf1058835,
    0xbf004550,
    0xbe1bf61a,
    0xbe42f61a,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch6_2 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_data_2),
    0x00021638,
    0xb9daf1e5,
    0xc00069f0,
    (uint8_t *)bes2300_patch6_ins_data_2
};///continue packet error


const uint32_t bes2300_patch7_ins_data_2[] =
{
    0xf003783b,
    0x2b000301,
    0x4630d108,
    0x22242117,
    0xfbe4f61b,
    0xbf002000,
    0xbf36f613,
    0xf86cf5fa,
    0xbec6f613,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch7_2 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch7_ins_data_2),
    0x0001a7d4,
    0xb928f1ec,
    0xc0006a28,
    (uint8_t *)bes2300_patch7_ins_data_2
};//sniff req collision


const BTDRV_PATCH_STRUCT bes2300_ins_patch8_2 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00035948,
    0x124cf640,
    0,
    NULL
};//acl evt dur 4 slot

const uint32_t bes2300_patch9_ins_data_2[] =
{
    0xbf00d106,
    0xfdb0f609,
    0xe0012801,
    0xbe30f621,
    0xbee6f621,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch9_2 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch9_ins_data_2),
    0x000285cc,
    0xb9c8f1de,
    0xc0006960,
    (uint8_t *)bes2300_patch9_ins_data_2
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch10_2 =
{
    10,
    BTDRV_PATCH_ACT,
    0,
    0x0004488c,
    0x0212bf00,
    0,
    NULL
};///acl data rx error when disconnect


#ifdef BT_VERSION_50

const BTDRV_PATCH_STRUCT bes2300_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x000227ec,
    0xf88d2309,
    0,
    NULL
};//version



const BTDRV_PATCH_STRUCT bes2300_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x000170a0,
    0x23093004,
    0,
    NULL
};//version

#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};
#endif


#ifdef BLE_POWER_LEVEL_0
const BTDRV_PATCH_STRUCT bes2300_ins_patch13_2 =
{
    13,
    BTDRV_PATCH_ACT,
    0,
    0x0003e4b8,
    0x7f032600,
    0,
    NULL
};//adv power level


const uint32_t bes2300_patch14_ins_data_2[] =
{
    0x32082100, ///00 means used the power level 0
    0x250052a1, //00 means used the power level 0
    0xb812f639,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch14_2 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_data_2),
    0x0003f824,
    0xbfecf1c6,
    0xc0006800,
    (uint8_t *)bes2300_patch14_ins_data_2
};//slave power level

const uint32_t bes2300_patch15_ins_data_2[] =
{
    0xf8282100, ///00 means used the power level 0
    0xf04f1002,
    0xbf000a00,//00 means used the power level 0
    0xbbdcf638,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch15_2 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_data_2),
    0x0003efd0,
    0xbc1ef1c7,
    0xc0006810,
    (uint8_t *)bes2300_patch15_ins_data_2
};//master

#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch13_2 =
{
    13,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_2 =
{
    14,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch15_2 =
{
    15,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_2 =
{
    16,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_2 =
{
    17,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_2 =
{
    18,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const uint32_t bes2300_patch19_ins_data_2[] =
{
    0x3026f853,
    0xf8842000,
    0xbf0002fa,
    0xba2af60f,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_2 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_data_2),
    0x00015b64,
    0xbdcef1f0,
    0xc0006704,
    (uint8_t *)bes2300_patch19_ins_data_2
};//bt role switch clear rswerror at lc_epr_cmp


const BTDRV_PATCH_STRUCT bes2300_ins_patch20_2 =
{
    20,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const uint32_t bes2300_patch21_ins_data_2[] =
{
    0x681b4b0f,
    0x4b0fb1bb,
    0x31cbf893,
    0x03c3ebc3,
    0x4a0d005b,
    0xf3c35a9b,
    0x4a0c2340,
    0x2a025c12,
    0x4a0ad103,
    0x20005413,
    0x42934770,
    0x4a07d005,
    0x20005413,
    0x20004770,
    0x20014770,
    0xbf004770,
    0xc00067a0,
    0xc0005c0c,
    0xd021159a,
    0xc00067a4,
    0x00000001,
    0x00020202,
    0x9803bf00,
    0xffd0f7ff,
    0xf0039b05,
    0x28000403,
    0xbf00d102,
    0xb9f3f626,
    0xbaa7f626

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_2 =
{
    21,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch21_ins_data_2),
    0x0002cba0,
    0xbe02f1d9,
    0xc0006750,
    (uint8_t *)bes2300_patch21_ins_data_2
};
/*ld_sw_seqn_filter*/


const BTDRV_PATCH_STRUCT bes2300_ins_patch22_2 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x0002b3f8,
    0xbf00bf00,
    0,
    NULL
};///remove PCM EN in controller

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_2 =
{
    23,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch24_2 =
{
    24,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_2 =
{
    25,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const uint32_t bes2300_patch26_ins_data_2[] =
{
    0x0301f043,
    0xd1012b07,
    0xbc00f625,
    0xbbeef625,
};



const BTDRV_PATCH_STRUCT bes2300_ins_patch26_2=
{
    26,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch26_ins_data_2),
    0x0002be74,
    0xbbfcf1da,
    0xc0006670,
    (uint8_t *)bes2300_patch26_ins_data_2
};//ld data tx 3m


const BTDRV_PATCH_STRUCT bes2300_ins_patch27_2 =
{
    27,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000061a8,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

#ifdef BT_DEBUG_PATCH
const uint32_t bes2300_patch28_ins_data_2[] =
{
    0x2b0b7b23,
    0xbf00d118,
    0x3b017b63,
    0x2b01b2db,
    0xbf00d812,
    0x010ef104,
    0xb1437ba3,
    0x33012200,
    0xb2d2441a,
    0x780b4419,
    0xd1f82b00,
    0x2200e000,
    0xf8ad3202,
    0xbf002006,
    0x72a323ff,
    0x3006f8bd,
    0xbf0072e3,
    0xbe10f63d,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_data_2),
    0x00044310,
    0xb9cef1c2,
    0xc00066b0,
    (uint8_t *)bes2300_patch28_ins_data_2
};//hci_build_dbg_evt for lmp record   ///can close when release sw

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_2 =
{
    29,
    BTDRV_PATCH_ACT,
    0,
    0x00000650,
    0xbdfcf005,
    0,
    NULL
};//enter default handler when assert

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_2 =
{
    30,
    BTDRV_PATCH_ACT,
    0,
    0x000062c4,
    0xfffef3ff,
    0,
    NULL
};//bus fault handler while(1)

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_INACT,
    0,
    0x0002f4d4,
    0xbf002301,
    0,
    NULL
};//force retx to 1


const BTDRV_PATCH_STRUCT bes2300_ins_patch32_2 =
{
    32,
    BTDRV_PATCH_INACT,
    0,
    0x00021de0,
    0xdd212b05,
    0,
    NULL
}; ///lmp record


const BTDRV_PATCH_STRUCT bes2300_ins_patch33_2 =
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0x00021e78,
    0xdd212b05,
    0,
    NULL
};//lmp record

#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch29_2 =
{
    29,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch30_2 =
{
    30,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch32_2 =
{
    32,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch33_2 =
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};




#endif


///can close when there is no patch position
const BTDRV_PATCH_STRUCT bes2300_ins_patch34_2 =
{
    34,
    BTDRV_PATCH_ACT,
    0,
    0x00017708,
    0x700ef247,
    0,
    NULL
};//set afh timeout to 20s

const uint32_t bes2300_patch35_ins_data_2[] =
{
    0x4778f027,
    0x708cf8c4,
    0xbc86f621,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch35_2 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch35_ins_data_2),
    0x00028368,
    0xbb74f1de,
    0xc0006a54,
    (uint8_t *)bes2300_patch35_ins_data_2
};//ld_acl_rx_sync

const uint32_t bes2300_patch36_ins_data_2[] =
{
    0xfbb4f61d,
    0x60184b01,
    0x81f0e8bd,
    0xc0006a90,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch36_2 =
{
    36,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch36_ins_data_2),
    0x0004438c,
    0xbb78f1c2,
    0xc0006a80,
    (uint8_t *)bes2300_patch36_ins_data_2
};//hci timout 1:h4tl_write record clock


const uint32_t bes2300_patch37_ins_data_2[] =
{
    0xfba4f61d,
    0x60184b01,
    0xbf00bd70,
    0xc0006a90,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch37_2 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch37_ins_data_2),
    0x00005e10,
    0xbe46f200,
    0xc0006aa0,
    (uint8_t *)bes2300_patch37_ins_data_2
};//hci timout 2:h4tl_rx_done recored clock

const uint32_t bes2300_patch38_ins_data_2[] =
{
    0x2b007b1b,
    0xbf00d00c,
    0xfb90f61d,
    0x681b4b05,
    0xf0201ac0,
    0xf5b04078,
    0xd9016f80,
    0xbbb6f5f9,
    0xbcdaf5f9,
    0xc0006a90,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_2 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_data_2),
    0x00000244,
    0xbc3cf206,
    0xc0006ac0,
    (uint8_t *)bes2300_patch38_ins_data_2
};//hci timout 3,sleep check


const BTDRV_PATCH_STRUCT bes2300_ins_patch39_2 =
{
    39,
    BTDRV_PATCH_ACT,
    0,
    0x0002b0dc,
    0x491d0300,
    0,
    NULL
};///remove PCM EN in controller


const uint32_t bes2300_patch40_ins_data_2[] =
{
    0x134cf640,
    0xf8b48263,
    0x68a320b8,
    0xbd03f622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch40_2 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch40_ins_data_2),
    0x00029500,
    0xbaf6f1dd,
    0xc0006af0,
    (uint8_t *)bes2300_patch40_ins_data_2
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch41_2 =
{
    41,
    BTDRV_PATCH_ACT,
    0,
    0x00027884,
    0xbf00e012,
    0,
    NULL
};//disable sscan

const uint32_t bes2300_patch42_ins_data_2[] =
{
    0x44137da3,
    0xbf0075a3,
    0xfa06f62f,
    0x4620b920,
    0xf0002101,
    0xb908f971,//bl ld_calculate_timestamp
    0xbb5bf623,
    0xbb68f623,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch42_2 =
{
    42,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch42_ins_data_2),
    0x00029f14,
    0xbc98f1dc,
    0xc0006848,
    (uint8_t *)bes2300_patch42_ins_data_2
};

const uint32_t bes2300_patch43_ins_data_2[] =
{
    0x781b4b05,
    0xd00342bb,
    0x66a3231a,
    0xbf00e002,
    0x66a32354,
    0xbdd0f620,
    0xc0006bfc,
    0x000000ff,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch43_2 =
{
    43,
#ifdef __LARGE_SYNC_WIN__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch43_ins_data_2),
    0x00027794,
    0xba24f1df,
    0xc0006be0,
    (uint8_t *)bes2300_patch43_ins_data_2
};

const uint32_t bes2300_patch44_ins_data_2[] =
{
    0x78124a10,
    0xd003454a,
    0x310d0c89,
    0xbe5af620,
    0xf1a12178,
    0xeba60354,
    0xb29b0353,
    0x4f00f413,
    0x2600d00c,
    0x2371f203,
    0x1c3ab29b,
    0x4778f022,
    0xb2f63601,
    0x4f00f413,
    0xe001d1f4,
    0xbe58f620,
    0xbe57f620,
    0xc0006bfc,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch44_2 =
{
    44,
#ifdef __LARGE_SYNC_WIN__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch44_ins_data_2),
    0x000278c0,
    0xb99ef1df,
    0xc0006c00,
    (uint8_t *)bes2300_patch44_ins_data_2
};

#ifndef __SW_SEQ_FILTER__
const BTDRV_PATCH_STRUCT bes2300_ins_patch45_2 =
{
    45,
    BTDRV_PATCH_ACT,
    0,
    0x0001e3d0,
    0xbf00bf00,
    0,
    NULL,
};//lmp_accept_ext_handle assert
#else
const uint32_t bes2300_patch45_ins_data_2[] =
{
    0xf8934b0b,
    0xebc331cb,
    0x005b03c3,
    0x5a9b4a09,
    0xf3c3b29b,
    0x4a082340,
    0x5c509903,
    0xd00328ff,
    0xd0034298,
    0xbf005453,
    0xbf92f625,
    0xb83af626,
    0xc0005c0c,
    0xd021159a,
    0xc0006c8c,
    0x02020202
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch45_2 =
{
    45,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch45_ins_data_2),
    0x0002cb7c,
    0xb868f1da,
    0xc0006c50,
    (uint8_t *)bes2300_patch45_ins_data_2
};


#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch46_2 =
{
    46,
    BTDRV_PATCH_ACT,
    0,
    0x0002b300,
    0xb2ad0540,
    0,
    NULL
};//sco duration

const uint32_t bes2300_patch47_ins_data_2[] =
{
    0xbf8c4288,//c0006b00 check_timestamp_conflict
    0x1a091a41,
    0x4078f021,
    0xbf8c4290,
    0x20012000,
    0xbf004770,

    0x2300b430,//c0006b18 find_other_acl_link
    0x4d072103,
    0x4290b2da,
    0xf855d004,
    0x2c004023,
    0x4611bf18,
    0x2b033301,
    0x4608d1f4,
    0x4770bc30,

    0xc00008fc,//ld_acl_env

    0x0f28f110,//c0006b40 ld_calculate_event_timestamp
    0x80e8f000,
    0x43f8e92d,
    0x4604460d,
    0xf6042002,
    0x2802fecb,
    0xf894d10b,
    0xf7ff00b2,
    0x2802ffdb,
    0x4b79d809,
    0x9020f853,
    0x0828f109,
    0xf04fe006,
    0x46c10800,
    0xf04fe002,
    0x46c10800,
    0xf8b34b73,
    0xf01331c0,
    0xf0000f18,
    0x4b7080d6,
    0x61c2f893,
    0xfc0ef63f,
    0x2e024607,
    0x80bef000,
    0xf8534b6c,
    0x2e006026,
    0x80bbf000,
    0xe044f896,
    0x1042f896,
    0x6b734608,
    0xf0231a5b,
    0x429f4378,
    0xebc3d909,
    0x4a640c07,
    0x45941a52,
    0x181ad203,
    0x42974613,
    0x2d00d8fb,
    0x808ef040,
    0x4378f023,
    0xd97f290b,
    0x78124a5d,
    0xf89475a2,
    0x2a0120b3,
    0xf894d15a,
    0xf89610b2,
    0x42912046,
    0x4a58d10a,
    0x3a786812,
    0x4a578262,
    0x44136812,
    0x4378f023,
    0xe00960a3,
    0x68124a54,
    0x82623a78,
    0x68124a53,
    0xf0234413,
    0x60a34378,
    0x0f00f1b9,
    0xf1b8d029,
    0xd0260f00,
    0x308bf898,
    0xd1222b01,
    0x8008f8d9,
    0x464068a5,
    0x22064629,
    0xff5af7ff,
    0xf896b148,
    0xeb033042,
    0xeb050343,
    0xf0230343,
    0x60a34378,
    0x4640e070,
    0x22064639,
    0xff4af7ff,
    0xf896b148,
    0xeb033042,
    0xeb050343,
    0xf0230343,
    0x60a34378,
    0xf8d9e060,
    0x4b3a0008,
    0x22066819,
    0xff38f7ff,
    0xd0492800,
    0x2042f896,
    0x441368a3,
    0x4378f023,
    0x200160a3,
    0x83f8e8bd,
    0x124cf640,
    0xf8968262,
    0xf1be2042,
    0xbf280f02,
    0x0e02f04f,
    0x0e01f10e,
    0x10b8f8b4,
    0xf1f2fb91,
    0x2202fb01,
    0x0e4eeb02,
    0x68124a28,
    0x44734496,
    0x4378f023,
    0x200160a3,
    0x83f8e8bd,
    0x12c4f640,
    0x33028262,
    0x4378f023,
    0x200160a3,
    0x83f8e8bd,
    0xd1182d01,
    0x30b5f894,
    0x68a3b9c3,
    0xf023440b,
    0x60a34378,
    0x781b4b13,
    0x200175a3,
    0x83f8e8bd,
    0x47702000,
    0xe8bd2000,
    0x200083f8,
    0x83f8e8bd,
    0xe8bd2001,
    0x200183f8,
    0x83f8e8bd,
    0xe8bd2001,
    0xf63f83f8,
    0x2000fb3b,
    0x83f8e8bd,
    0xe8bd2001,
    0xbf0083f8,

    0xc00008fc,//ld_acl_env
    0xc0005c0c,//ld_env
    0xc00008ec,//ld_sco_env
    0x07fffff5,
    0xa0047adc,//rwip_priority
    0xc0006d78,//same_link_duration
    0xc0006d7c,//same_link_slave_extra
    0xc0006d80,//other_link_duration
    0xc0006d84,//other_link_slave_extra
    0xc0006d88,//master_timestamp
    0xc0006d8c,//master_extra

    0x00000271,//same_link_duration
    0x00000000,//same_link_slave_extra
    0x00000271,//other_link_duration
    0x00000000,//other_link_slave_extra
    0x00000000,//master_timestamp
    0x00000000,//master_extra

    0x460a2100,//ld_acl_sched
    0xfed4f7ff,
    0xf63f4620,
    0xbf00fbdb,
    0xbbe2f622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch47_2 =
{
    47,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch47_ins_data_2),
    0x00029564,
    0xbc14f1dd,
    0xc0006b00,
    (uint8_t *)bes2300_patch47_ins_data_2
};//ld_calculate_acl_timestamp


const uint32_t bes2300_patch48_ins_data_2[] =
{
    0xf3efbf00,
    0xf0138310,
    0x2e000601,
    0xb672d100,
    0xfaeaf600,
    0xd1032800,
    0xd1002e00,
    0xbd70b662,
    0xfbc6f63c,
    0xd1032800,
    0xd1002e00,
    0xbd70b662,
    0xd1012e00,
    0xbb25f5f9,
    0xbb6ef5f9,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch48_2 =
{
    48,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch48_ins_data_2),
    0x0000041c,
    0xbcd0f206,
    0xc0006dc0,
    (uint8_t *)bes2300_patch48_ins_data_2
};


const uint32_t bes2300_patch49_ins_data_2[] =
{

    0xb083b500,
    0x0101f001,
    0x0148f041,
    0x1004f88d,
    0x2005f88d,
    0xf61ba901,
    0xb003f959,
    0xfb04f85d,
    0xbf004d0f,
    0x28ff6828,
    0x0200d016,
    0x407ff400,
    0x0001f040,
    0xfafaf5fa,
    0xd10d2801,
    0x4b076828,
    0x3020f853,
    0xf893b2c0,
    0x4b071040,
    0xf7ff781a,
    0x23ffffd7,
    0xbf00602b,
    0xb986f5ff,
    0xc0005b48,
    0xc0006e84,
    0x000000ff,
    0xc0006e8c,
    0x00000000,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch49_2 =
{
    49,
#ifdef __SEND_PREFERRE_RATE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch49_ins_data_2),
    0x00006174,
    0xbe64f200,
    0xc0006E20,
    (uint8_t *)bes2300_patch49_ins_data_2
};


const uint32_t bes2300_patch50_ins_data_2[] =
{
    0xf5fa4628,
    0x4903fad3,
    0x46286008,
    0xbf002101,
    0xbd4af613,
    0xc0006eb8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch50_2 =
{
    50,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch50_ins_data_2),
    0x0001a944,
    0xbaacf1ec,
    0xc0006ea0,
    (uint8_t *)bes2300_patch50_ins_data_2
};



const uint32_t bes2300_patch51_ins_data_2[] =
{
    0x0f00f1b8,
    0x4628d004,
    0x68094904,
    0xfa3af5fa,
    0x21134630,
    0x3044f894,
    0xbd6bf613,
    0xc0006eb8


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch51_2 =
{
    51,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch51_ins_data_2),
    0x0001a9dc,
    0xba88f1ec,
    0xc0006ef0,
    (uint8_t *)bes2300_patch51_ins_data_2
};


const uint32_t bes2300_patch52_ins_data_2[] =
{
    0xbf00251f,
    0xbe88f61e,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch52_2 =
{
    52,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch52_ins_data_2),
    0x00025c20,
    0xb976f1e1,
    0xc0006f10,
    (uint8_t *)bes2300_patch52_ins_data_2
};

#ifdef __POWER_CONTROL_TYPE_1__
const uint32_t bes2300_patch53_ins_data_2[] =
{
    0xf0035243,
    0x2b00030f,
    0x2000bf14,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch53_2 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch53_ins_data_2),
    0x000033bc,
    0xbdb0f203,
    0xc0006f20,
    (uint8_t *)bes2300_patch53_ins_data_2
};//pwr controll
#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch53_2 =
{
    53,
    BTDRV_PATCH_ACT,
    0,
    0x000033f4,
    0xe0072000,
    0,
    NULL
};
#endif

const uint32_t bes2300_patch54_ins_data_2[] =
{
    0xfbb24a02,
    0xf5f9f3f3,
    0xbf00bbc0,
    0x9af8da00,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch54_2 =
{
    54,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch54_ins_data_2),
    0x000006b4,
    0xbc3cf206,
    0xc0006f30,
    (uint8_t *)bes2300_patch54_ins_data_2
};//bitoff check

const uint32_t bes2300_patch55_ins_data_2[] =
{
    0x3044f885,
    0x22024b02,
    0x541a4630,
    0xba40f613,
    0xc00067a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch55_2 =
{
    55,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch55_ins_data_2),
    0x0001a3d4,
    0xbdb8f1ec,
    0xc0006f48,
    (uint8_t *)bes2300_patch55_ins_data_2
};/*lc_rsw_end_ind*/

static const uint32_t best2300_ins_patch_config_2[] =
{
    56,
    (uint32_t)&bes2300_ins_patch0_2,
    (uint32_t)&bes2300_ins_patch1_2,
    (uint32_t)&bes2300_ins_patch2_2,
    (uint32_t)&bes2300_ins_patch3_2,
    (uint32_t)&bes2300_ins_patch4_2,
    (uint32_t)&bes2300_ins_patch5_2,
    (uint32_t)&bes2300_ins_patch6_2,
    (uint32_t)&bes2300_ins_patch7_2,
    (uint32_t)&bes2300_ins_patch8_2,
    (uint32_t)&bes2300_ins_patch9_2,
    (uint32_t)&bes2300_ins_patch10_2,
    (uint32_t)&bes2300_ins_patch11_2,
    (uint32_t)&bes2300_ins_patch12_2,
    (uint32_t)&bes2300_ins_patch13_2,
    (uint32_t)&bes2300_ins_patch14_2,
    (uint32_t)&bes2300_ins_patch15_2,
    (uint32_t)&bes2300_ins_patch16_2,
    (uint32_t)&bes2300_ins_patch17_2,
    (uint32_t)&bes2300_ins_patch18_2,
    (uint32_t)&bes2300_ins_patch19_2,
    (uint32_t)&bes2300_ins_patch20_2,
    (uint32_t)&bes2300_ins_patch21_2,
    (uint32_t)&bes2300_ins_patch22_2,
    (uint32_t)&bes2300_ins_patch23_2,
    (uint32_t)&bes2300_ins_patch24_2,
    (uint32_t)&bes2300_ins_patch25_2,
    (uint32_t)&bes2300_ins_patch26_2,
    (uint32_t)&bes2300_ins_patch27_2,
    (uint32_t)&bes2300_ins_patch28_2,
    (uint32_t)&bes2300_ins_patch29_2,
    (uint32_t)&bes2300_ins_patch30_2,
    (uint32_t)&bes2300_ins_patch31_2,
    (uint32_t)&bes2300_ins_patch32_2,
    (uint32_t)&bes2300_ins_patch33_2,
    (uint32_t)&bes2300_ins_patch34_2,
    (uint32_t)&bes2300_ins_patch35_2,
    (uint32_t)&bes2300_ins_patch36_2,
    (uint32_t)&bes2300_ins_patch37_2,
    (uint32_t)&bes2300_ins_patch38_2,
    (uint32_t)&bes2300_ins_patch39_2,
    (uint32_t)&bes2300_ins_patch40_2,
    (uint32_t)&bes2300_ins_patch41_2,
    (uint32_t)&bes2300_ins_patch42_2,
    (uint32_t)&bes2300_ins_patch43_2,
    (uint32_t)&bes2300_ins_patch44_2,
    (uint32_t)&bes2300_ins_patch45_2,
    (uint32_t)&bes2300_ins_patch46_2,
    (uint32_t)&bes2300_ins_patch47_2,
    (uint32_t)&bes2300_ins_patch48_2,
    (uint32_t)&bes2300_ins_patch49_2,
    (uint32_t)&bes2300_ins_patch50_2,
    (uint32_t)&bes2300_ins_patch51_2,
    (uint32_t)&bes2300_ins_patch52_2,
    (uint32_t)&bes2300_ins_patch53_2,
    (uint32_t)&bes2300_ins_patch54_2,
    (uint32_t)&bes2300_ins_patch55_2,
};


/*****************************************
2300 t6 patch
patch 0  0xc0006800-0xc0006848
patch 1  0xc0006860-0xc00068cc
patch 2  0xc00068d0-0xc00068ec
patch 3  0xc00068f0-0xc0006910
patch 4  0xc0006914-0xc0006920
patch 5  0xc0006920-0xc000692c
patch 6  0xc000692c-0xc0006938
patch 7  0xc000693c-0xc000694c
patch 8  0xc000694c-0xc000695c
patch 9  0xc0006960-0xc0006978
patch 10 0xc00069b0-0xc00069d0
patch 11
patch 12
patch 13 0xc00069d0-0xc00069fc
patch 14 0xc000430C-0xc0004408
patch 15 0xc0004414-0xc00047e0
patch 16 0xc0006d20-0xc0006d48
patch 17 0xc0006be8-0xc0006c94
patch 18 0xc0006fb0-0xc0006fd4
patch 19 0xc00047e4-0xc00048d0
patch 20
patch 21 0xc0006a58-0xc0006abc
patch 22
patch 23 0xc0006b00-0xc0006b94
patch 24
patch 25 0xc0006ee0-0xc0006ef8
patch 26
patch 27 0xc0006f0c-0xc0006fb0
patch 28 0xc0006ce0-0xc0006d18
patch 29
patch 30
patch 31 0xc0006fd8-0xc0006fe8
patch 32 0xc0006fec-0xc0007000
patch 33 0xc0007000-0xc0007014
patch 34 0xc00042c0-0xc00042dc
patch 35 0xc0007018-0xc000703c
patch 36 0xc000703c-0xc0007068
patch 37 0xc000706c-0xc0007080
patch 38 0xc0007080-0xc00070c8
patch 39 0xc00070c8-0xc0007104
patch 40 0xc0006eb8-0xc0006ecc
patch 41 0xc0006bd8-0xc0006be4
patch 42 0xc0004250-0xc00042b8
patch 43 0xc0006978-0xc000698c
patch 44 0xc000716c-0xc00071a8
patch 45 0xc00071a8-0xc00071c8
patch 46 0xc00071c8-0xc00071e0
patch 47 0xc00071e0-0xc0007200
patch 48 0xc0006a00-0xc0006a54
patch 49 0xc0006990-0xc00069a0
patch 50
patch 51 0xc0004200-0xc0004248
patch 52
patch 53
patch 54
patch 55 0xc0006ad8-0xc0006af4
//blank c0006ac0--c0006ad0
//blank c0006c98--c0006ce0
//blank c0006ed0--c0006ed8
//blank c0006d50--c0006d8c
//blank 0xc0006da4-0xc0006db4
//blank 0xc0006b98-0xc0006bd4
//blank 0xc0006d90-0xc0006da4
*******************************************/
//#define DUAL_SCO_SWITCH

const uint32_t bes2300_patch0_ins_data_3[] =
{
    0xb2db7823,
    0xd1042b01,
    0x70212100,
    0xf5fa2008,
    0xf3eff83f,
    0xf0138310,
    0xd1080701,
    0xf5f8b672,
    0xb110ffc7,
    0x2f00bf30,
    0xb662d106,
    0xf5f8e004,
    0x2800ffbf,
    0xbf00d1f6,
    0xbd3af5ff,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch0_3 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data_3),
    0x00006cf8,
    0xba9af200,
    0xc0007230,
    (uint8_t *)bes2300_patch0_ins_data_3
};


const uint32_t bes2300_patch1_ins_data_3[] =
{
    0x681b4b0f,
    0x4b0fb1bb,
    0x31cbf893,
    0x03c3ebc3,
    0x4a0d005b,
    0xf3c35a9b,
    0x4a0c2340,
    0x2a025c12,
    0x4a0ad103,
    0x20005413,
    0x42934770,
    0x4a07d005,
    0x20005413,
    0x20004770,
    0x20014770,
    0xbf004770,
    0xc00068b0,
    0xc0005c90,
    0xd021159a,
    0xc00068b4,
    0x00000001,
    0x00020202,
    0x9803bf00,
    0xffd0f7ff,
    0xd1012800,
    0xbf97f626,
    0xb81af627
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch1_3 =
{
    1,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch1_ins_data_3),
    0x0002d7f0,
    0xb862f1d9,
    0xc0006860,
    (uint8_t *)bes2300_patch1_ins_data_3
};/*ld_sw_seqn_filter*/

const uint32_t bes2300_patch2_ins_data_3[] =
{
    0xf5fa4628,
    0x4903fd61,
    0x46286008,
    0xbf002101,
    0xbe20f614,
    0xc00068e8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_3=
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch2_ins_data_3),
    0x0001b520,
    0xb9d6f1eb,
    0xc00068d0,
    (uint8_t *)bes2300_patch2_ins_data_3
};//role switch



const uint32_t bes2300_patch3_ins_data_3[] =
{
    0x0f00f1b8,
    0x4628d004,
    0x68094904,
    0xfce0f5fa,
    0x21134630,
    0x3044f894,
    0xbe59f614,
    0xc00068e8


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch3_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch3_ins_data_3),
    0x0001b5b8,
    0xb99af1eb,
    0xc00068f0,
    (uint8_t *)bes2300_patch3_ins_data_3
};//role switch

const uint32_t bes2300_patch4_ins_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbdeef626

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch4_3 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_data_3),
    0x0002d4f4,
    0xba0ef1d9,
    0xc0006914,
    (uint8_t *)bes2300_patch4_ins_data_3
};//ld_acl_rx rssi

const uint32_t bes2300_patch5_ins_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xb818f61f

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_3 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch5_ins_data_3),
    0x00025954,
    0xbfe4f1e0,
    0xc0006920,
    (uint8_t *)bes2300_patch5_ins_data_3
};//ld_inq_rx rssi

const uint32_t bes2300_patch6_ins_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbf56f63a

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6_3 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_data_3),
    0x000417dc,
    0xb8a6f1c5,
    0xc000692c,
    (uint8_t *)bes2300_patch6_ins_data_3
};//lld_pdu_rx_handler rssi


const BTDRV_PATCH_STRUCT bes2300_ins_patch7_3 =
{
    7,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002c22c,
    0xbf00e010,
    0,
    NULL
};


const uint32_t bes2300_patch8_ins_data_3[] =
{
    0xf8934b04,
    0xb11b3037,
    0x30acf8bb,
    0xbe92f626,
    0xbea1f626,
    0xc0004168,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch8_3 =
{
    8,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch8_ins_data_3),
    0x0002d578,
    0xb966f1d9,
    0xc0006848,
    (uint8_t *)bes2300_patch8_ins_data_3
};////ld_acl_rx not accumulate rssi when hw_agc flag is false

const uint32_t bes2300_patch9_ins_data_3[] =
{
    0x22024b03,
    0x555a463d,
    0x4b022200,
    0xbb62f623,
    0xc00068b4,
    0xc000095c
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch9_3 =
{
    9,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch9_ins_data_3),
    0x0002a030,
    0xbc96f1dc,
    0xc0006960,
    (uint8_t *)bes2300_patch9_ins_data_3
};/*ld_acl_end*/

const uint32_t bes2300_patch10_ins_data_3[] =
{
    0xf8957812,
    0xf89500bc,
    0x287810bb,
    0x2978d001,
    0x4613d102,
    0xba32f627,
    0xbf004413,
    0xba2ef627
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch10_3 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch10_ins_data_3),
    0x0002de28,
    0xbdc2f1d8,
    0xc00069b0,
    (uint8_t *)bes2300_patch10_ins_data_3
};//ld_acl_frm_isr


const BTDRV_PATCH_STRUCT bes2300_ins_patch11_3 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x00003444,
    0xe00f382d,
    0,
    NULL
};///hw rssi read


const BTDRV_PATCH_STRUCT bes2300_ins_patch12_3 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x0001b77c,
    0xbf00e001,
    0,
    NULL
};//start enc always accept

const uint32_t bes2300_patch13_ins_data_3[] =
{
    0xf5fa4620,
    0x285dfce1,
    0xf240d10e,
    0x46215005,
    0xf85af5fa,
    0xf8534b05,
    0x46203026,
    0x135ef893,
    0xfc66f5fa,
    0xbc73f616,
    0xbc71f616,
    0xc0005bcc,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_3 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch13_ins_data_3),
    0x0001d2cc,
    0xbb80f1e9,
    0xc00069d0,
    (uint8_t *)bes2300_patch13_ins_data_3
};

#ifdef DUAL_SCO_SWITCH
const uint32_t bes2300_patch14_ins_data_3[] =
{
    0xf8534b39,
    0xf8d22020,
    0xb12330fc,
    0x2b06789b,
    0x2b08d063,
    0xf892d063,
    0xb11330c6,
    0xb2c01e58,
    0x4b324770,
    0x2b01681b,
    0xe92dd15b,
    0x4b3047f0,
    0x087f781f,
    0x3cfff117,
    0xf04fbf18,
    0x4d2d0c01,
    0xf1a52401,
    0xf8550e08,
    0x2a002d04,
    0xf892d03e,
    0x42833046,
    0xf892d13a,
    0xbbbb304a,
    0x6027f85e,
    0x8034f8d6,
    0x302cf85e,
    0x45986b5b,
    0xebc3bf8c,
    0xebc80808,
    0xf8960803,
    0x6b539042,
    0xd9094299,
    0xa042f892,
    0xe0004453,
    0xeb034633,
    0x4299060a,
    0x6353d8fa,
    0xf0231a5b,
    0xf8924378,
    0xfbb36042,
    0xfb06f2f6,
    0xb98b3312,
    0xf3f9fbb8,
    0x8313fb09,
    0x2b063b03,
    0xb2e0d80a,
    0x4a0f0164,
    0xf42358a3,
    0xf443037f,
    0x50a33380,
    0x87f0e8bd,
    0xf1b43c01,
    0xd1b83fff,
    0xe8bd2002,
    0x200087f0,
    0x20004770,
    0x20024770,
    0xbf004770,
    0xc000095c,//ld_acl_env
    0xc0006d44,//inactive_sco_en
    0xc0006009,//bt_voice_active
    0xc0000954,//ld_sco_env+0x08
    0xd0220150,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_3 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_data_3),
    0x000282fc,
    0xb806f1dc,
    0xc000430C,
    (uint8_t *)bes2300_patch14_ins_data_3
};//ld_acl_sco_rsvd_check
#else
const uint32_t bes2300_patch14_ins_data_3[] =
{
    0xf8b34b10,
    0xf01331c0,
    0xd0190f18,
    0xf8934b0d,
    0xf61e71c2,
    0x2f02f899,
    0x4b0bd012,
    0x2027f853,
    0x6b53b172,
    0xd2064298,
    0xf8921a1b,
    0xeb022042,
    0x42930242,
    0x4620d804,
    0x460a2100,
    0xfd48f623,
    0xbed2f623,

    0xc0005c90,//ld_env
    0xc000094c,//ld_sco_env


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_3 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_data_3),
    0x0002acec,
    0xb90ef1dc,
    0xc0006f0c,
    (uint8_t *)bes2300_patch14_ins_data_3
};///ld acl sched
#endif

const uint32_t bes2300_patch15_ins_data_3[] =
{
    0x4ff0e92d,
    0x9100b089,
    0x4bc09201,
    0xb91b781b,
    0xb0092000,
    0x8ff0e8bd,
    0x4bbc4605,
    0x9004f993,
    0x8005f993,
    0xa006f993,
    0xb007f993,
    0x920568da,
    0x9303691b,
    0x681b4bb6,
    0x20029306,
    0xffe0f607,
    0xd0642802,
    0x93042300,
    0x4bb29302,
    0x31c0f8b3,
    0x0f18f013,
    0x821ff000,
    0xf8934bae,
    0xf62041c2,
    0x4606fe7b,
    0xf0002c02,
    0x00a4820c,
    0x4440f104,
    0x794cf8d4,
    0xf0002f00,
    0xf8978206,
    0x93073044,
    0x2042f897,
    0x6b7c4613,
    0xf0241aa4,
    0x42a04478,
    0x441cd904,
    0x4478f024,
    0xd8fa42a6,
    0x2b009b00,
    0x81b4f040,
    0xf2402a0b,
    0x4b9b81a8,
    0x75ab781b,
    0x30b3f895,
    0xd0432b01,
    0xf8b34b96,
    0xf3c331c0,
    0x2b0303c1,
    0x8169f040,
    0x20b2f895,
    0x3046f897,
    0xf000429a,
    0x9b03814d,
    0x691cf5c3,
    0x0904f109,
    0x9012f8a5,
    0x3042f897,
    0x20b8f8b5,
    0xf2f3fb92,
    0x3303fb02,
    0x2a019a07,
    0x2201bf28,
    0xeb0a3201,
    0x44530a42,
    0xf0234423,
    0x60ab4378,
    0xf895e161,
    0x230120b2,
    0xf202fa03,
    0xf8b34b7e,
    0xea2331c0,
    0xf3c30302,
    0x009b0341,
    0x4340f103,
    0x395cf8d3,
    0xb1139302,
    0x93043328,
    0x9b02e787,
    0xe7849304,
    0xf8b34b74,
    0xf3c331c0,
    0x2b0303c1,
    0xf895d069,
    0xf89720b2,
    0x429a3046,
    0x80a5f000,
    0xfd10f5fe,
    0xf5c39b03,
    0xf109691c,
    0xf8900904,
    0xeba93025,
    0xf8a50903,
    0xf1089012,
    0x44230302,
    0x4378f023,
    0x9b0260ab,
    0x9b04b133,
    0xf893b123,
    0x2b01308b,
    0x809af000,
    0xfb6ef642,
    0x680368ac,
    0xf24042a3,
    0xf64280cf,
    0x6803fb67,
    0x1a9b68aa,
    0x4378f023,
    0xd8062b06,
    0x3042f897,
    0x441368aa,
    0x4378f023,
    0x210060ab,
    0xf63c68a8,
    0x2806f9c1,
    0x8101f240,
    0x3042f897,
    0x441368aa,
    0x4378f023,
    0x9a0260ab,
    0xf0002a00,
    0x9a0480f6,
    0xf0002a00,
    0x9a0480f2,
    0x208bf892,
    0xf0402a01,
    0x9a0280ec,
    0x429a6892,
    0x1ad1bf8c,
    0xf0211a99,
    0x29064178,
    0x809ff200,
    0x2042f897,
    0xfb012106,
    0xf0233302,
    0x60ab4378,
    0xf895e0d7,
    0xf89720b2,
    0x429a3046,
    0x9b06d016,
    0xd0242b01,
    0xfca4f5fe,
    0xf5c39b03,
    0xf109691c,
    0xf8900904,
    0xeba93025,
    0xf8a50903,
    0xf1089012,
    0x44230302,
    0x4378f023,
    0xe79260ab,
    0xfc90f5fe,
    0xf5c39b05,
    0x3304631c,
    0x2025f890,
    0x826b1a9b,
    0x0304f109,
    0xf0234423,
    0x60ab4378,
    0xf5fee781,
    0x9b03fc7f,
    0x691cf5c3,
    0x0904f109,
    0x3025f890,
    0x0903eba9,
    0x9012f8a5,
    0x0304f108,
    0xf0234423,
    0x60ab4378,
    0xf5fee76d,
    0x9b05fc6b,
    0x631cf5c3,
    0xf8903304,
    0x1a9b2025,
    0xf109826b,
    0x44230304,
    0x4378f023,
    0xe75c60ab,
    0x689b9b02,
    0x428b68a9,
    0x1a5abf8c,
    0xf0221aca,
    0x2a064278,
    0xf897d81e,
    0xeb033042,
    0xeb010343,
    0xf0230343,
    0x60ab4378,
    0x68a82100,
    0xf92af63c,
    0xd96a2806,
    0x3042f897,
    0x441368aa,
    0x4378f023,
    0xe77060ab,
    0xc0006054,
    0xc0006c94,
    0xc0005c90,
    0xc0000200,
    0xbf3442b3,
    0x1b9b1af3,
    0x4378f023,
    0xf63f2b05,
    0xf897af33,
    0x22063042,
    0x1303fb02,
    0x4378f023,
    0xe7d760ab,
    0xfa98f642,
    0x1ae36803,
    0x4378f023,
    0x42b2e730,
    0x1ab2bf34,
    0xf0221b92,
    0x2a054278,
    0xf897d839,
    0x21062042,
    0x3302fb01,
    0x4378f023,
    0xe03060ab,
    0xf5c39b05,
    0x3306636a,
    0xf897826b,
    0xf8b53042,
    0xfb9220b8,
    0xfb02f2f3,
    0xf10a3303,
    0x44530a04,
    0xf0234423,
    0x60ab4378,
    0xf5fee01b,
    0xf890fbf1,
    0xf5c33025,
    0x3304631c,
    0xf897826b,
    0xf8b53042,
    0xfb9220b8,
    0xfb02f2f3,
    0x9a073303,
    0xbf282a02,
    0x32012202,
    0x0a42eb0a,
    0x44234453,
    0x4378f023,
    0x68ab60ab,
    0xf0221b9a,
    0xf1b14178,
    0xd9056f80,
    0xf0331af3,
    0xd1054378,
    0xe6122001,
    0x4278f022,
    0xdd4a2a90,
    0x200160ae,
    0xf240e60b,
    0x826b43e2,
    0xf0243402,
    0x60ac4478,
    0xe6022001,
    0x2b019b00,
    0xf640d11c,
    0x826b13c4,
    0x3042f897,
    0x441c3304,
    0x4478f024,
    0x1ba360ac,
    0x4278f023,
    0x6f80f1b2,
    0x1b34d905,
    0x4378f034,
    0x2001d105,
    0xf023e5e9,
    0x2b904378,
    0x60aedd23,
    0xe5e22001,
    0x2b029b00,
    0x9b07d11f,
    0x8aa93302,
    0x7fc8f5b1,
    0x2100bf94,
    0xebc12101,
    0x9b010143,
    0xf3f2fb93,
    0x2202fb03,
    0xeb014493,
    0x4423030b,
    0x4378f023,
    0x200160ab,
    0x2000e5c7,
    0x2000e5c5,
    0x2001e5c3,
    0x2001e5c1,
    0x2001e5bf,
    0xf620e5bd,
    0x2000fc5f,
    0xbf00e5b9,

};



const BTDRV_PATCH_STRUCT bes2300_ins_patch15_3 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_data_3),
    0x0002a9dc,
    0xbc8ef1d9,
    0xc00042fc,
    (uint8_t *)bes2300_patch15_ins_data_3
};


#ifdef DUAL_SCO_SWITCH
const uint32_t bes2300_patch16_ins_data_3[] =
{
    0xf8b34b06,
    0xf3c331c0,
    0x2b0303c1,
    0x2201d102,
    0x601a4b03,
    0xe8bdb011,
    0xbf008ff0,
    0xc0005c90,
    0xc0006d44, //inactive_sco_en
    0x00000000
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_3 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch16_ins_data_3),
    0x00031000,
    0xbe8ef1d5,
    0xc0006d20,
    (uint8_t *)bes2300_patch16_ins_data_3
};///ld sco start
#else
const uint32_t bes2300_patch16_ins_data_3[] =
{
    0x8016f894,
    0x0f1ff1b8,
    0x231fd902,
    0x469875a3,
    0xbbd6f622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_3 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch16_ins_data_3),
    0x000294dc,
    0xbc20f1dd,
    0xc0006d20,
    (uint8_t *)bes2300_patch16_ins_data_3
};//ld_acl_clk_isr protect curr_prio

#endif

const uint32_t bes2300_patch17_ins_data_3[] =
{
    0xf6254651,
    0x4680f9d7,
    0xf6052002,
    0x2802fb83,
    0xf894d13b,
    0xf04f3046,
    0xfa090901,
    0x4b1ef903,
    0x31c0f8b3,
    0x0909ea23,
    0x0941f3c9,
    0x681b4b1b,
    0x4648b133,
    0xfd12f629,
    0xbf082805,
    0x0801f04f,
    0x2096f8b5,
    0xf8534b17,
    0xf8b33029,
    0xb2103096,
    0x4288b219,
    0x1a43dd01,
    0xb21be002,
    0x1a9bb212,
    0x02e1f1a3,
    0xd9032a0e,
    0x1381f2a3,
    0xd8092b0e,
    0x4b0d2201,
    0xf894601a,
    0xf44f9047,
    0xbf003800,
    0xba75f629,
    0x4b082200,
    0xf894601a,
    0xea4f9047,
    0xbf004808,
    0xba63f629,

    0xc0005c90,//ld_env
    0xc0006c88,//music_ongoing_flag
    0x00000000,
    0xc000095c,//ld_acl_env
    0xc0006c94,//acl_rearrange_flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_3 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_data_3),
    0x00030138,
    0xbd56f1d6,
    0xc0006be8,
    (uint8_t *)bes2300_patch17_ins_data_3
};//ld_sco_evt_start_cbk


const uint32_t bes2300_patch18_ins_data_3[] =
{
    0x781b4b06,
    0x0f53ebb7,
    0x2201d006,
    0x701a4b04,
    0xe8bdb003,
    0xbf008ff0,
    0xb8f5f625,
    0xc0006009, //bt_voice_active
    0xc000096a,//inactive_sco_return
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_3 =
{
    18,
    #ifdef DUAL_SCO_SWITCH
    BTDRV_PATCH_ACT,
    #else
    BTDRV_PATCH_INACT,
    #endif
    sizeof(bes2300_patch18_ins_data_3),
    0x0002c0c4,
    0xbf74f1da,
    0xc0006fb0,
    (uint8_t *)bes2300_patch18_ins_data_3
};//ld_sco_sched 1


#ifdef DUAL_SCO_SWITCH
const uint32_t bes2300_patch19_ins_data_3[] =
{
    0x4b332201,
    0x2200601a,
    0x601a4b32,
    0xf8b34b32,
    0xf01331c0,
    0xd0570318,
    0x2b0310db,
    0x4b2ed103,
    0x01c2f883,
    0x2b02e00c,
    0x2201d104,
    0xf8834b2a,
    0xe00521c2,
    0xd1032b01,
    0x4b272200,
    0x21c2f883,
    0xf8934b25,
    0x4b2521c2,
    0x4022f853,
    0xf6072002,
    0xb1d8fd63,
    0xf8934b20,
    0x230151c2,
    0x4a1b40ab,
    0x429a7812,
    0x2101d012,
    0x70114a1a,
    0x70134a17,
    0x3042f894,
    0xd9092b02,
    0xfb0ef5fe,
    0x78820229,
    0x500bf240,
    0x0101f041,
    0xf8b6f5fc,
    0xf6072001,
    0x2802fd43,
    0xf894d128,
    0x2801004b,
    0x4c27d13e,
    0x4b236822,
    0x51b9f893,
    0x11b8f893,
    0x2105ea41,
    0x51c2f893,
    0x2105ea41,
    0x0201f002,
    0x6022430a,
    0x21c2f893,
    0xbb11f627,
    0xbb1cf627,

    0xc0006009,
    0xc00048d0,
    0xc0006d44,
    0xc0005c90,
    0xc000094c,
    0xc000096a,
    0xd02201b0,

    0xbb29f627,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_3 =
{
    19,   
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_data_3),
    0x0002be30,
    0xbcd8f1d8,
    0xc00047e4,
    (uint8_t *)bes2300_patch19_ins_data_3
};//ld_sco_dynamic_switch
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch19_3 =
{
    19,
    BTDRV_PATCH_ACT,
    0,
    0x0002e478,
    0xbf00e001,
    0,
    0
};//ld_acl_flow_on
//fix ld_acl_tx assert
#endif

const uint32_t bes2300_patch20_ins_data_3[] =
{
    0x681b4b07,
    0x4b08b143,
    0xebc3681b,
    0xeb031243,
    0xeb030382,
    0x82630383,
    0x3090f8b4,
    0xbf00f63b,

    0xc0006ddc,//music_playing_flag
    0x00000000,
    0xc0006de4,//ble_slot_set
    0x00000002,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_3 =
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch20_ins_data_3),
    0x00042bd4,
    0xb8f0f1c4,
    0xc0006db8,
    (uint8_t *)bes2300_patch20_ins_data_3
};//lld_evt_restart set duration = 2 slot


const uint32_t bes2300_patch21_ins_data_3[] =
{
    0xf0234413,
    0x4a034378,
    0xfb026812,
    0x63633300,
    0xba7cf62a,

    0xc0006a78,
    0x0000000f,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_3 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch21_ins_data_3),
    0x00030f64,
    0xbd7cf1d5,
    0xc0006a60,
    (uint8_t *)bes2300_patch21_ins_data_3
};//ld_sco_start


const BTDRV_PATCH_STRUCT bes2300_ins_patch22_3 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x000190a8,
    0xbf00e003,
    0,
    NULL
};////iocap rsp


const uint32_t bes2300_patch23_ins_data_3[] =
{
    0xf8534b20,
    0x01474020,
    0x58bb4a1f,
    0x4380f443,
    0x4b1e50bb,
    0x31c0f8b3,
    0x03c1f3c3,
    0xd1022b03,
    0x4b1b2201,
    0x4b1b601a,
    0xb1c3781b,
    0x4b192200,
    0xe006701a,
    0x2042f894,
    0x44136b63,
    0x4378f023,
    0x6b656363,
    0xfa0df640,
    0xd3f34285,
    0x2042f894,
    0xeb036b63,
    0x63630342,
    0xf6254630,
    0xf894fa8d,
    0x2b01304b,
    0x4a09d10c,
    0x11b9f892,
    0x31b8f892,
    0x2301ea43,
    0x2306ea43,
    0x0301f043,
    0xbbaaf625,
    0xbbb3f625,

    0xc000094c,
    0xd0220140,
    0xc0005c90,
    0xc0006d44,
    0xc000096a,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_3 =
{
    23,
    #ifdef DUAL_SCO_SWITCH
    BTDRV_PATCH_ACT,
    #else
    BTDRV_PATCH_INACT,
    #endif
    sizeof(bes2300_patch23_ins_data_3),
    0x0002c288,
    0xbc3af1da,
    0xc0006b00,
    (uint8_t *)bes2300_patch23_ins_data_3
};//ld_sco_switch_to_handler all

const BTDRV_PATCH_STRUCT bes2300_ins_patch24_3 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x0001dd08,
    0xbf00e058,
    0,
    NULL
};//lmp accepted assert


const uint32_t bes2300_patch25_ins_data_3[] =
{
    0x4b042200,
    0x0000701a,
    0x8310f3ef,
    0x0401f013,
    0xbdf4f62a,
    0xc0006d44,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_3 =
{
    25,
    #ifdef DUAL_SCO_SWITCH
    BTDRV_PATCH_ACT,
    #else
    BTDRV_PATCH_INACT,
    #endif
    sizeof(bes2300_patch25_ins_data_3),
    0x00031ad4,
    0xba04f1d5,
    0xc0006ee0,
    (uint8_t *)bes2300_patch25_ins_data_3
};//ld_sco_stop_link set inactive_sco_en=0

const BTDRV_PATCH_STRUCT bes2300_ins_patch26_3 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00019d80,
    0xbf00e003,
    0,
    NULL
};//power rsp processs

const uint32_t bes2300_patch27_ins_data_3[] =
{
    0x4c1cb538,
    0x5020f854,
    0xf8b44c1b,
    0xf3c331c0,
    0x2b0303c1,
    0xf895d126,
    0x2b0130b3,
    0xf894d124,
    0x428331c2,
    0x4615d022,
    0x2001460c,
    0xf9e0f605,
    0xd11d2801,
    0xf8934b10,
    0x4b1021c2,
    0x2022f853,
    0x42a36893,
    0xf892d207,
    0x44132042,
    0xbf88429c,
    0x70fff64f,
    0x1b18d801,
    0x42a8e7ff,
    0x2000bf8c,
    0xbd382001,
    0xbd382000,
    0xbd382000,
    0xbd382000,
    0xbd382000,
    0xc000095c,
    0xc0005c90,
    0xc000094c,
    0x30c7f895,
    0xd10b2b00,
    0xf85ef61e,
    0x46204601,
    0xf7ff2205,
    0x2800ffb5,
    0xbf00d102,
    0xbfc1f626,
    0xbfabf626,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch27_3 =
{
    27,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch27_ins_data_3),
    0x0002df00,
    0xb844f1d9,
    0xc0006f0c,
    (uint8_t *)bes2300_patch27_ins_data_3
};//acl frm isr
//MASTER+SLAVE can be not used

const uint32_t bes2300_patch28_ins_data_3[] =
{
    0xf6052002,
    0x2802fb0b,
    0xf89bd10d,
    0xf5fc40b2,
    0x7c03f8c5,
    0xd00642a3,
    0xf9e6f60c,
    0x2300b918,
    0x30bbf88b,
    0xf89be006,
    0x3b0130bb,
    0xf88bb2db,
    0xbf0030bb,
    0xbd03f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch28_3 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_data_3),
    0x0002d718,
    0xbae2f1d9,
    0xc0006ce0,
    (uint8_t *)bes2300_patch28_ins_data_3
};//ld_acl_rx rx_traffic--

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_3 =
{
    29,
    BTDRV_PATCH_ACT,
    0,
    0x0002eaa8,
    0xbf00e053,
    0,
    NULL
};///ld_acl_lmp_flush return

const uint32_t bes2300_patch30_ins_data_3[] =
{
    0x20b4f895,
    0xd1132a02,
    0x4b0bb983,
    0x0024f853,
    0xfb4cf640,
    0x30b5f895,
    0xd1032b04,
    0x21164620,
    0xfc7ef622,
    0x21164620,
    0xf942f623,
    0xb97ef627,
    0x20bef895,
    0xb966f627,

    0xc000095c,//ld_acl_env
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_3 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_data_3),
    0x0002df98,
    0xbe7ef1d8,
    0xc0006c98,
    (uint8_t *)bes2300_patch30_ins_data_3
};//ld_acl_sket_isr


const uint32_t bes2300_patch31_ins_data_3[] =
{
    0xfbb24a02,
    0xf5f9f3f3,
    0xbf00bb3a,
    0x9af8da00,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_3 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch31_ins_data_3),
    0x00000650,
    0xbcc2f206,
    0xc0006fd8,
    (uint8_t *)bes2300_patch31_ins_data_3
};//lpo timer

const uint32_t bes2300_patch32_ins_data_3[] =
{
    0x781b4b02,
    0xf8807583,
    0x477030ad,
    0xc0006ffc,
    0x0000000f,//priority
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch32_3 =
{
    32,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch32_ins_data_3),
    0x00043eb8,
    0xb898f1c3,
    0xc0006fec,
    (uint8_t *)bes2300_patch32_ins_data_3
};//change rwip_priority[RWIP_PRIO_LE_CON_IDLE_IDX].value

const uint32_t bes2300_patch33_ins_data_3[] =
{
    0x781b4b02,
    0xf8807583,
    0x477030ad,
    0xc0007010,
    0x0000000f,//priority
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch33_3 =
{
    33,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch33_ins_data_3),
    0x00043ec8,
    0xb89af1c3,
    0xc0007000,
    (uint8_t *)bes2300_patch33_ins_data_3
};//change rwip_priority[RWIP_PRIO_LE_CON_ACT_IDX].value

const uint32_t bes2300_patch34_ins_data_3[] =
{
    0x736c60ab,
    0x681b4b05,
    0x0f53ebb4,
    0xf1b9d104,
    0xbf840f01,
    0x601f4b02,
    0xba0cf61e,

    0xc0006009,
    0xc00042d4,//sco_timestamp_push
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch34_3 =
{
    34,
    #ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
    #else
    BTDRV_PATCH_INACT,
    #endif
    sizeof(bes2300_patch34_ins_data_3),
    0x000253e0,
    0xbde6f1e1,
    0xc0006fb0,
    (uint8_t *)bes2300_patch34_ins_data_3
};//ld_fm_prog_push

const uint32_t bes2300_patch35_ins_data_3[] =
{
    0x892b4806,
    0x123ff240,
    0xd001421a,
    0xe0002201,
    0x60022200,
    0x3086f897,
    0xbc14f63a,
    0xc0007038,//no_packet flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch35_3 =
{
    35,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch35_ins_data_3),
    0x00041858,
    0xbbdef1c5,
    0xc0007018,
    (uint8_t *)bes2300_patch35_ins_data_3
};//lld_pdu_rx_handler check ble rx

const uint32_t bes2300_patch36_ins_data_3[] =
{
    0x4628d805,
    0xf8b52102,
    0xf62320a8,
    0x4b06fcc9,
    0xb12b681b,
    0x23130000,
    0x220075ab,
    0x601a4b02,
    0x00004628,
    0xbc8cf63c,
    0xc0007038,//no_packet flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch36_3 =
{
    36,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch36_ins_data_3),
    0x0004396c,
    0xbb66f1c3,
    0xc000703c,
    (uint8_t *)bes2300_patch36_ins_data_3
};//lld_evt_end


const uint32_t bes2300_patch37_ins_data_3[] =
{
    /*706c*/ 0xd1052b01,
    /*7070*/ 0x2b0178f3,
    /*7074*/ 0xbf00d902,
    /*7078*/ 0xbc28f617, // jump a0207078 -> a001e8cc
    /*707c*/ 0xbc30f617, // jump a020707c -> a001e8e0
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch37_3 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch37_ins_data_3),
    0x0001e8c8,
    0xbbd0f1e8, // jump a001e8c8 -> a020706c
    0xc000706c,
    (uint8_t *)bes2300_patch37_ins_data_3
};//LMP_MSG_HANDLER(feats_res_ext)

#ifdef DUAL_SCO_SWITCH
const uint32_t bes2300_patch38_ins_data_3[] =
{
    0x781b4b0b,
    0x0f53ebb4,
    0x4b0ed00e,
    0x31c0f8b3,
    0x0318f003,
    0xd0072b00,
    0x4b062201,
    0x4b07601a,
    0x0000601c,
    0xbb4cf623,
    0x22000164,
    0xbb46f623,
    0xc0006009,
    0xc00070b8,//acl_end
    0x00000000,
    0xc00070c0,//background_sco_link
    0x000000ff,
    0xc0005c90,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_3 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_data_3),
    0x0002a738,
    0xbca2f1dc,
    0xc0007080,
    (uint8_t *)bes2300_patch38_ins_data_3
};//ld_sco_end:
//two sco,background sco end, set link and acl_end

const uint32_t bes2300_patch39_ins_data_3[] =
{
    0x681b4b0a,
    0xd00c2b00,
    0x681b4b09,
    0xd0082bff,
    0x2200015b,
    0x505a4907,
    0x601a4b04,
    0x4b0422ff,
    0x4620601a,
    0x00004639,
    0xbea0f626,
    0xc00070b8,
    0xc00070c0,
    0xd0220140,
    0xc000094c,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch39_3 =
{
    39,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch39_ins_data_3),
    0x0002de30,
    0xb94af1d9,
    0xc00070c8,
    (uint8_t *)bes2300_patch39_ins_data_3
};//ld_acl_frm_isr
//if acl_end and link, bt_e_scochancntl_pack(background_sco_link, 0, 0, 0, 0, 0);
#else

const uint32_t bes2300_patch38_ins_data_3[] =
{
    0x4604b510,
    0x309df890,
    0xd00d2b01,
    0xd0122b02,
    0x30aef894,
    0x0308f023,
    0x30aef884,
    0xf8842300,
    0xf8a4309d,
    0xbd10308c,
    0xf8a02200,
    0x21032096,
    0x47984b03,
    0x2200e7ec,
    0x4b012104,
    0xe7e74798,
    0xa0041bdd,
    0xf890b508,
    0x3b0330ab,
    0x2b01b2db,
    0xbd08d900,
    0x30aef890,
    0x0f08f013,
    0xf8b0d0f9,
    0xf8b02090,
    0x429a308c,
    0xf7ffd1f3,
    0xe7f0ffc9,
    0x4620b50f,
    0xffe6f7ff,
    0x400fe8bd,
    0xf904f63a,
    0xbea6f63a,
    0x00004770,//c0007104
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_3 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_data_3),
    0x00041e4c,
    0xb950f1c5,//trap_conv.exe a0041e4c a02070f0
    0xc0007080,
    (uint8_t *)bes2300_patch38_ins_data_3
};//lld_evt_schedule param update instant and map update instant


const BTDRV_PATCH_STRUCT bes2300_ins_patch39_3 =
{
    39,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

#endif

const uint32_t bes2300_patch40_ins_data_3[] =
{
    0xf0037803,
    0xf8860301,
    0x46083045,
    0xfbf6f5fa,
    0xd1132836,
    0xf8534b0b,
    0x46282027,
    0xfbeef5fa,
    0x035ef882,
    0x5005f240,
    0xf5f94629,
    0x4628ff67,
    0xf5fa2101,
    0x2201fb77,
    0x601a4b03,
    0xbf004628,
    0xb8b0f614,

    0xc0005bcc,//lc_env
    0xc0006db0,//sniff_esco_conflict_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch40_3 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch40_ins_data_3),
    0x0001ad34,
    0xbf32f1eb,
    0xc0006b9c,
    (uint8_t *)bes2300_patch40_ins_data_3
};//lmp_esco_link_req_handler

const uint32_t bes2300_patch41_ins_data_3[] =
{
    0x681b4b0c,
    0x0a2ab17b,
    0xf8534b0c,
    0x46283022,
    0x135ef893,
    0xfa98f5fa,
    0xf60c4628,
    0x2200fedf,
    0x601a4b04,
    0x4628bd38,
    0xf5fa2101,
    0xbd38fa8d,
    0xbf00bf00,

    0xc0006db0,//sniff_esco_conflict_flag
    0x00000000,
    0xc0005bcc,//lc_env

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch41_3 =
{
    41,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch41_ins_data_3),
    0x0000f7c0,
    0xbadaf1f7,
    0xc0006d78,
    (uint8_t *)bes2300_patch41_ins_data_3
};//lc_sco_free_nego


const uint32_t bes2300_patch42_ins_data_3[] =
{
    0x4b1768a2,
    0x429a681b,
    0x1ad3bf8c,
    0xf8941a9b,
    0x1d912042,
    0xd801428b,
    0xd21d4293,
    0x3046f894,
    0x4a110159,
    0x2a00588a,
    0x8f61da08,
    0xf44f0049,
    0xfb026207,
    0x480d1303,
    0xe0094418,
    0x4a0b8f21,
    0xf44f440a,
    0xfb006007,
    0x8f602303,
    0x0040eb03,
    0x8f622100,
    0xfcd1f643,
    0x6b6368a2,
    0xbe72f62b,

    0xc00042d4,//sco_timestamp_push
    0x00000000,
    0xd0220140,
    0xd021449c,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch42_3 =
{
    42,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch42_ins_data_3),
    0x0002ffb0,
    0xb95ef1d4,
    0xc0004270,
    (uint8_t *)bes2300_patch42_ins_data_3
};//ld_sco_evt_start_cbk

const uint32_t bes2300_patch43_ins_data_3[] =
{
    0x3044f885,
    0x22024b02,
    0x541a4630,
    0xbb06f614,
    0xc00068b4
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch43_3 =
{
    43,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch43_ins_data_3),
    0x0001af90,
    0xbcf2f1eb,
    0xc0006978,
    (uint8_t *)bes2300_patch43_ins_data_3
};/*lc_rsw_end_ind*/

const uint32_t bes2300_patch44_ins_data_3[] =
{
    0xf0135a9b,
    0xd00f0f80,
    0xd10d2c00,
    0x30abf890,
    0xb2db3b03,
    0xd8052b01,
    0x4b062201,
    0x7d83601a,
    0x75834413,
    0xbc02f63c,
    0x4b022200,
    0x0000601a,
    0xbb53f63c,
    0xc00071a4,//ble_unstarted_flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch44_3 =
{
    44,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch44_ins_data_3),
    0x00043838,
    0xbc98f1c3,
    0xc000716c,
    (uint8_t *)bes2300_patch44_ins_data_3
};//lld_evt_end

const uint32_t bes2300_patch45_ins_data_3[] =
{
    0xd1092d00,
    0x68124a05,
    0xd1052a00,
    0x49032200,
    0x0000600a,
    0xbd39f63b,
    0xbdbaf63b,
    0xc00071a4,//ble_unstarted_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch45_3 =
{
    45,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch45_ins_data_3),
    0x00042c2c,
    0xbabcf1c4,
    0xc00071a8,
    (uint8_t *)bes2300_patch45_ins_data_3
};//lld_evt_restart

const uint32_t bes2300_patch46_ins_data_3[] =
{
    0x4b032101,
    0xf8946019,
    0x0000309d,
    0xbb9ef63b,
    0xc00071dc,//ble_param_update
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch46_3 =
{
    46,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch46_ins_data_3),
    0x00042910,
    0xbc5af1c4,
    0xc00071c8,
    (uint8_t *)bes2300_patch46_ins_data_3
};//lld_evt_restart

const uint32_t bes2300_patch47_ins_data_3[] =
{
    0x4b06460e,
    0x2b00681b,
    0x2200d004,
    0x601a4b03,
    0x75832313,
    0x00002501,
    0xbdc6f63b,
    0xc00071dc,//ble_param_update
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch47_3 =
{
    47,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch47_ins_data_3),
    0x00042d6c,
    0xba38f1c4,
    0xc00071e0,
    (uint8_t *)bes2300_patch47_ins_data_3
};//lld_evt_elt_insert

const uint32_t bes2300_patch48_ins_data_3[] =
{
    0x681b4b1e,
    0xf5feb173,
    0x7bc3fb47,
    0xf5feb153,
    0x7c03fb43,
    0xd10542b3,
    0xf5fe8a67,
    0x6943fb3d,
    0x8263443b,
    0xfb38f5fe,
    0x3035f890,
    0x2002b14b,
    0xfd72f607,
    0xd1042802,
    0x681a4b12,
    0x44138a63,
    0xf5fe8263,
    0x7bc3fb29,
    0x2002b1ab,
    0xfd64f607,
    0xd1102802,
    0xfb20f5fe,
    0x42b37c03,
    0x8a67d00b,
    0xfb1af5fe,
    0x443b6943,
    0xfba24a08,
    0x68a22303,
    0x2353eb02,
    0xbf0060a3,
    0xba45f626,

    0xc0004864,//reconnecting_flag
    0x00000000,
    0xc000486c,//dual_slave_slot_add
    0x000009c4,
    0xd1b71759,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch48_3 =
{
    48,
#if defined(__BT_ONE_BRING_TWO__)&&defined(MULTIPOINT_DUAL_SLAVE)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch48_ins_data_3),
    0x0002aca0,
    0xbda0f1d9,
    0xc00047e4,
    (uint8_t *)bes2300_patch48_ins_data_3
};//ld_acl_sched

const uint32_t bes2300_patch49_ins_data_3[] =
{
    0x02004638,
    0x0001f040,
    0xf83cf60d,
    0xbaa4f61c
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch49_3 =
{
    49,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch49_ins_data_3),
    0x00022ebc,
    0xbd68f1e3,
    0xc0006990,
    (uint8_t *)bes2300_patch49_ins_data_3
};//lc_send_lmp

const BTDRV_PATCH_STRUCT bes2300_ins_patch50_3 =
{
    50,
    BTDRV_PATCH_ACT,
    0,
    0x0002e430,
    0xe0022000,
    0,
    NULL
};///ld_acl_stop

const uint32_t bes2300_patch51_ins_data_3[] =
{
    0x68124a0c,
    0xd1132a01,
    0x68104a0b,
    0x60103001,
    0xd10d280c,//12
    0xf890480a,
    0x014001c2,
    0x58424909,
    0x4280f422,
    0x20005042,
    0x60104a03,
    0x60104a01,
    0xb948f625,
    0xc00048d0,//switch_sco_flag
    0xc000423c,//switch_delay_counter,
    0x00000000,
    0xc0005c90,//ld_env
    0xd0220140,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch51_3 =
{
    51,
    #ifdef DUAL_SCO_SWITCH
    BTDRV_PATCH_ACT,
    #else
    BTDRV_PATCH_INACT,
    #endif
    sizeof(bes2300_patch51_ins_data_3),
    0x000294a4,
    0xbeacf1da,
    0xc0004200,
    (uint8_t *)bes2300_patch51_ins_data_3
};//ld_acl_clk_isr, set switch to link
//(bt_e_scochancntl_e_scochanen_setf 1)

const uint32_t bes2300_patch52_ins_data_3[] =
{
    0xf8934b07,
    0xf0133084,
    0xd0070f06,
    0xf6324620,
    0x4601fb5f,
    0xbf002400,
    0xbceaf631,
    0xbca9f631,

    0xc0006344,//llm_le_env
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch52_3 =
{
    52,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch52_ins_data_3),
    0x000386bc,
    0xbb48f1ce,
    0xc0006d50,
    (uint8_t *)bes2300_patch52_ins_data_3
};//hci_le_set_adv_data_cmd_handler


const uint32_t bes2300_patch53_ins_data_3[] =
{
    0xd8092b01,
    0xfb03236e,
    0x4a04f308,
    0xf4135a9b,
    0xd1016f00,
    0xbb85f627,
    0xbc0df627,
    0xd02111a8
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch53_3 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch53_ins_data_3),
    0x0002e92c,
    0xbc70f1d8,
    0xc0007210,
    (uint8_t *)bes2300_patch53_ins_data_3
};//tx null


const BTDRV_PATCH_STRUCT bes2300_ins_patch54_3 =
{
    54,
    BTDRV_PATCH_ACT,
    0,
    0x00028344,
    0xbf00e00c,
    0,
    NULL
};//ld_acl_sco_rsvd_check


const uint32_t bes2300_patch55_ins_data_3[] =
{
    0x2040f896,
    0xd1112a01,
    0x0109f107,
    0xf5fc4608,
    0x2813fac5,
    0x221ed802,
    0xbd16f613,
    0x2b017a3b,
    0xf613d801,
    0x221ebcce,
    0xbd0ef613,
    0xbd0af613,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch55_3 =
{
    55,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch55_ins_data_3),
    0x0001a470,
    0xbb26f1ec,
    0xc0006ac0,
    (uint8_t *)bes2300_patch55_ins_data_3
};


/////2300 t6 patch
static const uint32_t best2300_ins_patch_config_3[] =
{
    56,
    (uint32_t)&bes2300_ins_patch0_3,
    (uint32_t)&bes2300_ins_patch1_3,
    (uint32_t)&bes2300_ins_patch2_3,
    (uint32_t)&bes2300_ins_patch3_3,
    (uint32_t)&bes2300_ins_patch4_3,
    (uint32_t)&bes2300_ins_patch5_3,
    (uint32_t)&bes2300_ins_patch6_3,
    (uint32_t)&bes2300_ins_patch7_3,
    (uint32_t)&bes2300_ins_patch8_3,
    (uint32_t)&bes2300_ins_patch9_3,
    (uint32_t)&bes2300_ins_patch10_3,
    (uint32_t)&bes2300_ins_patch11_3,
    (uint32_t)&bes2300_ins_patch12_3,
    (uint32_t)&bes2300_ins_patch13_3,
    (uint32_t)&bes2300_ins_patch14_3,
    (uint32_t)&bes2300_ins_patch15_3,
    (uint32_t)&bes2300_ins_patch16_3,
    (uint32_t)&bes2300_ins_patch17_3,
    (uint32_t)&bes2300_ins_patch18_3,
    (uint32_t)&bes2300_ins_patch19_3,
    (uint32_t)&bes2300_ins_patch20_3,
    (uint32_t)&bes2300_ins_patch21_3,
    (uint32_t)&bes2300_ins_patch22_3,
    (uint32_t)&bes2300_ins_patch23_3,
    (uint32_t)&bes2300_ins_patch24_3,
    (uint32_t)&bes2300_ins_patch25_3,
    (uint32_t)&bes2300_ins_patch26_3,
    (uint32_t)&bes2300_ins_patch27_3,
    (uint32_t)&bes2300_ins_patch28_3,
    (uint32_t)&bes2300_ins_patch29_3,
    (uint32_t)&bes2300_ins_patch30_3,
    (uint32_t)&bes2300_ins_patch31_3,
    (uint32_t)&bes2300_ins_patch32_3,
    (uint32_t)&bes2300_ins_patch33_3,
    (uint32_t)&bes2300_ins_patch34_3,
    (uint32_t)&bes2300_ins_patch35_3,
    (uint32_t)&bes2300_ins_patch36_3,
    (uint32_t)&bes2300_ins_patch37_3,
    (uint32_t)&bes2300_ins_patch38_3,
    (uint32_t)&bes2300_ins_patch39_3,
    (uint32_t)&bes2300_ins_patch40_3,
    (uint32_t)&bes2300_ins_patch41_3,
    (uint32_t)&bes2300_ins_patch42_3,
    (uint32_t)&bes2300_ins_patch43_3,
    (uint32_t)&bes2300_ins_patch44_3,
    (uint32_t)&bes2300_ins_patch45_3,
    (uint32_t)&bes2300_ins_patch46_3,
    (uint32_t)&bes2300_ins_patch47_3,
    (uint32_t)&bes2300_ins_patch48_3,
    (uint32_t)&bes2300_ins_patch49_3,
    (uint32_t)&bes2300_ins_patch50_3,
    (uint32_t)&bes2300_ins_patch51_3,
    (uint32_t)&bes2300_ins_patch52_3,
    (uint32_t)&bes2300_ins_patch53_3,
    (uint32_t)&bes2300_ins_patch54_3,
    (uint32_t)&bes2300_ins_patch55_3,

};

void btdrv_ins_patch_write(BTDRV_PATCH_STRUCT *ins_patch_p)
{
    uint32_t remap_addr;
    // uint8_t i=0;
    remap_addr =   ins_patch_p->patch_remap_address | 1;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*4),
                       (uint8_t *)&ins_patch_p->patch_remap_value,4);
    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
#if 0
        for( ; i<(ins_patch_p->patch_length)/128; i++)
        {
            btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,
                               (ins_patch_p->patch_data+i*128),128);
        }

        btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,ins_patch_p->patch_data+i*128,
                           ins_patch_p->patch_length%128);
#endif
        btdrv_memory_copy((uint32_t *)ins_patch_p->patch_start_address,(uint32_t *)ins_patch_p->patch_data,ins_patch_p->patch_length);
    }

    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*4),
                       (uint8_t *)&remap_addr,4);

}

void btdrv_ins_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0; i<best2300_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        //HAL_CHIP_METAL_ID_2
        for(uint8_t i=0; i<best2300_ins_patch_config_2[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_2[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        for(uint8_t i=0; i<best2300_ins_patch_config_3[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_3[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_3[i+1]);
        }
    }
}

///////////////////data  patch ..////////////////////////////////////
#if 0
0106 0109 010a 0114 0114 010a 0212 0105     ................
010a 0119 0119 0105 0105 0108 0214 0114     ................
0108 0114 010a 010a 0105 0105 0114 010a     ................
0b0f 0105 010a 0000
#endif

static const uint32_t bes2300_dpatch0_data[] =
{
    0x01090106,
    0x0114010a,
    0x010a0114,
    0x01050212,
    0x0119010a,
    0x01050119,
    0x01080105,
    0x01140214,
    0x01140108,
    0x010a010a,
    0x01050105,
    0x0b0f0114,
    0x01050b0f,
    0x0000010a,
};

static const BTDRV_PATCH_STRUCT bes2300_data_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_dpatch0_data),
    0x00043318,
    0xc0000058,
    0xc0000058,
    (uint8_t *)&bes2300_dpatch0_data
};

static const uint32_t best2300_data_patch_config[] =
{
    1,
    (uint32_t)&bes2300_data_patch0,

};






void btdrv_data_patch_write(const BTDRV_PATCH_STRUCT *d_patch_p)
{

    uint32_t remap_addr;
    uint8_t i=0;

    remap_addr = d_patch_p->patch_remap_address |1;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_DATA_COMP_ADDR_START + d_patch_p->patch_index*4),
                       (uint8_t *)&remap_addr,4);
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_DATA_REMAP_ADDR_START + d_patch_p->patch_index*4),
                       (uint8_t *)&d_patch_p->patch_remap_value,4);

    if(d_patch_p->patch_length != 0)  //have ram patch data
    {
        for( ; i<(d_patch_p->patch_length-1)/128; i++)
        {
            btdrv_write_memory(_32_Bit,d_patch_p->patch_start_address+i*128,
                               (d_patch_p->patch_data+i*128),128);

        }

        btdrv_write_memory(_32_Bit,d_patch_p->patch_start_address+i*128,d_patch_p->patch_data+i*128,
                           d_patch_p->patch_length%128);
    }

}


void btdrv_ins_patch_disable(uint8_t index)
{
    uint32_t addr=0;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_COMP_ADDR_START + index*4),
                       (uint8_t *)&addr,4);

}


void btdrv_data_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *data_patch_p;
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0; i<best2300_data_patch_config[0]; i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)best2300_data_patch_config[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)best2300_data_patch_config[i+1]);
        }
    }
}


//////////////////////////////patch enable////////////////////////


void btdrv_patch_en(uint8_t en)
{
    uint32_t value[2];

    //set patch enable
    value[0] = 0x2f02 | en;
    //set patch remap address  to 0xc0000100
    value[1] = 0x20000100;
    btdrv_write_memory(_32_Bit,BTDRV_PATCH_EN_REG,(uint8_t *)&value,8);
}


const BTDRV_PATCH_STRUCT bes2300_ins_patch0_testmode_2 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x0002816c,
    0x789a3100,
    0,
    NULL
};///no sig tx test


const BTDRV_PATCH_STRUCT bes2300_ins_patch1_testmode_2 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0000b7ec,
    0xbf00e007,
    0,
    NULL
};///lm init

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_testmode_2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x0003faa4,
    0xbf002202,
    0,
    NULL
};//ble test power

const BTDRV_PATCH_STRUCT bes2300_ins_patch3_testmode_2 =
{
    3,
    BTDRV_PATCH_ACT,
    0,
    0x00023d64,
    0xbf00bd08,
    0,
    NULL
};//ld_channel_assess


const BTDRV_PATCH_STRUCT bes2300_ins_patch4_testmode_2 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0003E280,
#ifdef BLE_POWER_LEVEL_0
    0x23008011,
#else
    0x23028011,
#endif
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_testmode_2 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x0003E284,
    0x021CbF00,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6_testmode_2 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x00018450,
    0x78b3bF00,
    0,
    NULL
};


const uint32_t bes2300_patch7_ins_test_data_2[] =
{
    0x30fcf8d4,
    0xd0092b00,
    0x680b4905,
    0x3f00f413,
    0x680bd004,
    0x3300f423,
    0xbf00600b,
    0xbaeff627,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch7_testmode_2 =
{
    7,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch7_ins_test_data_2),
    0x0002df58,
    0xbd12f1d8,
    0xc0006980,
    (uint8_t *)bes2300_patch7_ins_test_data_2
};



const uint32_t bes2300_patch8_ins_test_data_2[] =
{
    0x30fcf8d4,
    0xd00b2b00,
    0xf003789b,
    0x2b0503fd,
    0x4b04d106,
    0xf443681b,
    0x4a023300,
    0xbf006013,
    0xbc50f625,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch8_testmode_2 =
{
    8,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch8_ins_test_data_2),
    0x0002c140,
    0xbc36f1da,
    0xc00069b0,
    (uint8_t *)bes2300_patch8_ins_test_data_2
};




const uint32_t bes2300_patch9_ins_test_data_2[] =
{
    0x5bf24a03,
    0x080ff002,
    0xbccbf622,
    0xd0211160,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch9_testmode_2 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch9_ins_test_data_2),
    0x000290A0,
    0xbb30f1dd,
    0xc0006704,
    (uint8_t *)bes2300_patch9_ins_test_data_2
};


const uint32_t bes2300_patch10_ins_test_data_2[] =
{
    0xf0025bf2,
    0xbf00020f,
    0xbce8f622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch10_testmode_2 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch10_ins_test_data_2),
    0x000290ec,
    0xbb12f1dd,
    0xc0006714,
    (uint8_t *)bes2300_patch10_ins_test_data_2
};



const uint32_t bes2300_patch11_ins_test_data_2[] =
{
    0xf0035bf3,
    0xbf00080f,
    0xbcfaf622,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch11_testmode_2 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch11_ins_test_data_2),
    0x0002911c,
    0xbb00f1dd,
    0xc0006720,
    (uint8_t *)bes2300_patch11_ins_test_data_2
};



const uint32_t bes2300_patch12_ins_test_data_2[] =
{
    0xf0035bf3,
    0xbf00010f,
    0xbd1af622,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_testmode_2 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch12_ins_test_data_2),
    0x0002916c,
    0xbae0f1dd,
    0xc0006730,
    (uint8_t *)bes2300_patch12_ins_test_data_2
};

#ifdef __POWER_CONTROL_TYPE_1__
const uint32_t bes2300_patch13_ins_test_data_2[] =
{
    0xf0035243,
    0x2b00030f,
    0x2000bf14,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch13_testmode_2 =
{
    13,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch13_ins_test_data_2),
    0x000033bc,
    0xbdb0f203,
    0xc0006f20,
    (uint8_t *)bes2300_patch13_ins_test_data_2
};//pwr controll
#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_testmode_2 =
{
    13,
    BTDRV_PATCH_INACT,
    0,
    0x000033f4,
    0xe0072000,
    0,
    NULL
};
#endif

////for power control test mode
const BTDRV_PATCH_STRUCT bes2300_ins_patch14_testmode_2 =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x00028e8c,
    0xe0cfbf00,
    0,
    NULL
};

const uint32_t bes2300_patch15_ins_test_data_2[] =
{
    0xb2db1e63,
    0xbf002b01,
    0x30fcf8d6,
    0xf002789a,
    0x2a0502fd,
    0x4a2ad14a,
    0x11cbf892,
    0x4a2a0109,
    0xf3c25a8a,
    0x48292240,
    0x49216002,
    0x3201680a,
    0x4a20600a,
    0x8000f8c2,
    0xf3c29a05,
    0x491e02c9,
    0x491f600a,
    0x11cbf891,
    0x01c1ebc1,
    0xf830481e,
    0xf3c11011,
    0xbf002100,
    0xb119bf00,
    0x68014816,
    0x6001481f,
    0x600c4915,
    0x68004819,
    0x2100b150,
    0x60014817,
    0x68014817,
    0x6001480f,
    0x68014816,
    0x6001480e,
    0xf1008c18,
    0xf5004050,
    0x4b0c1004,
    0x31cbf893,
    0x03c3ebc3,
    0xf831490a,
    0xb2891013,
    0x4150f101,
    0x1104f501,
    0xfc6af640,
    0xba71f626,
    0xc0006864,///test_mode_acl_rx_flag
    0xc0006868,///loopback_type
    0xc000686c,///loopback_length
    0xc0006870,///loopback_llid
    0xc0005c0c,
    0xd02115a0,
    0xd021159a,
    0xc0006874,///rxseq_flag
    0xc0006878,///unack_seqerr_flag
    0xc000687c,///rxlength_flag
    0xc0006880,///rxllid_flag
    0xc0006884,///rxarqn_flag
    0xc0006888,///ok_rx_length_flag
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch15_testmode_2 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_test_data_2),
    0x0002cd04,
    0xbd3cf1d9,
    0xc0006780,
    (uint8_t *)bes2300_patch15_ins_test_data_2
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch16_testmode_2 =
{
    16,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};
////end for power control test mode


const uint32_t bes2300_patch17_ins_test_data_2[] =
{
    0x20014b04,
    0x4b026018,
    0xbf00689b,
    0xbc0ef610,
    0xc0004100,
    0xc00068d8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_testmode_2 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_test_data_2),
    0x000170E8,
    0xbbeaf1ef,
    0xc00068C0,
    (uint8_t *)bes2300_patch17_ins_test_data_2
};


const uint32_t bes2300_patch18_ins_test_data_2[] =
{
    0x20024b04,
    0x4b026018,
    0xbf00689b,
    0xbdbaf611,
    0xc0004100,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_testmode_2 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch18_ins_test_data_2),
    0x00018460,
    0xba3ef1ee,
    0xc00068e0,
    (uint8_t *)bes2300_patch18_ins_test_data_2
};


const uint32_t bes2300_patch19_ins_test_data_2[] =
{
    0x20014b04,
    0x4b026018,
    0xbf0068db,
    0xbc1af610,
    0xc0004100,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_testmode_2 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_test_data_2),
    0x00017140,
    0xbbdef1ef,
    0xc0006900,
    (uint8_t *)bes2300_patch19_ins_test_data_2
};


const uint32_t bes2300_patch20_ins_test_data_2[] =
{
    0x20024b04,
    0x4b026018,
    0x462068db,
    0xbda4f611,
    0xc0004100,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_testmode_2 =
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch20_ins_test_data_2),
    0x00018474,
    0xba54f1ee,
    0xc0006920,
    (uint8_t *)bes2300_patch20_ins_test_data_2
};


const uint32_t bes2300_patch21_ins_test_data_2[] =
{
    0xd91b429a,
    0x5a42b410,
    0xb2dc3301,
    0x427ff402,
    0x52424322,
    0x68104a0c,
    0xd00a2801,
    0xf8924a0b,
    0x42830030,
    0x2000bf14,
    0x4a072001,
    0x60112100,
    0x2000e001,
    0xf85d6010,
    0x47704b04,
    0x20004b02,
    0x20016018,
    0xbf004770,
    0xc00068d8,
    0xc0004100,


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_testmode_2 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch21_ins_test_data_2),
    0x000033e0,
    0xbaaef203,
    0xc0006940,
    (uint8_t *)bes2300_patch21_ins_test_data_2
};



const uint32_t bes2300_patch22_ins_test_data_2[] =
{
    0xf0135a43,
    0xd016030f,
    0x3b015a42,
    0xf402b2db,
    0x4313427f,
    0xf0135243,
    0x4a08030f,
    0x28016810,
    0x2b00d006,
    0x2000bf14,
    0x21002001,
    0xe0016011,
    0x60102000,
    0x20014770,
    0xbf004770,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_testmode_2 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch22_ins_test_data_2),
    0x000033a8,
    0xbafaf203,
    0xc00069a0,
    (uint8_t *)bes2300_patch22_ins_test_data_2
};


#if 0
const uint32_t bes2300_patch23_ins_test_data_2[] =
{
    0xf8934b14,
    0xebc331cb,
    0x005b03c3,
    0x4812461f,
    0x4a124910,
    0xf2414d12,
    0x5a3b5496,
    0x030ef3c3,
    0xf891523b,
    0x330131cb,
    0xd5034013,
    0xf0633b01,
    0x33010303,
    0x31cbf881,
    0xebc3b2db,
    0x005b03c3,
    0x682e461f,
    0xf3c64423,
    0x42b3060e,
    0xbf00d1e5,
    0xba3df626,
    0xc0005c0c,
    0xd0211596,
    0x80000003,
    0xd022002c,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_testmode_2 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_test_data_2),
    0x0002cdf4,
    0xbdfcf1d9,
    0xc00069f0,
    (uint8_t *)bes2300_patch23_ins_test_data_2
};
#else

const uint32_t bes2300_patch23_ins_test_data_2[] =
{
    0x60b39b00,
    0xfb00206e,
    0x4a0bf004,
    0xf0436813,
    0x60130302,
    0x4b092202,
    0xf5a3601a,
    0x3bb6436e,
    0x3003f830,
    0x030cf3c3,
    0x5300f443,
    0xf8204a04,
    0xbf003002,
    0xb840f625,
    0xD022000C,
    0xd0220018,
    0xd0211162,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_testmode_2 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_test_data_2),
    0x0002baa4,
    0xbfa4f1da,
    0xc00069f0,
    (uint8_t *)bes2300_patch23_ins_test_data_2
};
#endif
const BTDRV_PATCH_STRUCT bes2300_ins_patch24_testmode_2 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x0003772c,
    0xf7c92100,
    0,
    NULL,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch25_testmode_2 =
{
    25,
    BTDRV_PATCH_ACT,
    0,
    0x00024148,
    0xbf00e002,
    0,
    NULL,
};

#if 0
const BTDRV_PATCH_STRUCT bes2300_ins_patch26_testmode_2 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00024024,
    0xbf00bf00,
    0,
    NULL,
};
#else
const uint32_t bes2300_patch26_ins_test_data_2[] =
{
    0x3a063006,
    0xfa57f640,
    0xb9def61d,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch26_testmode_2 =
{
    26,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch26_ins_test_data_2),
    0x00024024,
    0xbe1cf1e2,
    0xc0006c60,
    (uint8_t *)bes2300_patch26_ins_test_data_2
};
#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch27_testmode_2 =
{
    27,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000061a8,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

const uint32_t bes2300_patch28_ins_test_data_2[] =
{
    0x0f03f1b8,
    0xf1b8d002,
    0xd1010f08,
    0xe0192401,
    0xf8934b0d,
    0xebc331cb,
    0x4a0c03c3,
    0x3013f832,
    0x2300f3c3,
    0x60134a0f,
    0x4b09b953,
    0x4b0c681a,
    0x4b08601a,
    0x4b08681a,
    0x2201601a,
    0x601a4b07,
    0xb842f626,
    0xb945f626,
    0xc0005c0c,
    0xd021159a,
    0xc0006870,///loopback_llid
    0xc000686c,///loopback_length
    0xc000687c,///rxlength_flag
    0xc0006878,///unack_seqerr_flag
    0xc0006880,///rxllid_flag
    0xc0006884,////rxarqn_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch28_testmode_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_test_data_2),
    0x0002ccf4,
    0xbea4f1d9,
    0xc0006a40,
    (uint8_t *)bes2300_patch28_ins_test_data_2
};///ld_acl_rx:skip rxseq_err


const uint32_t bes2300_patch29_ins_test_data_2[] =
{
    0xb2d23a05,
    0xd9012a03,
    0xb8a2f000,
    0xd0022a00,
    0xf0402a02,
    0xbf00809f,
    0x681b4b4f,
    0xf0002b00,
    0xf8d58099,
    0x881a30fc,
    0xf00179d9,
    0xb90a010f,
    0x881a4b4a,
    0x236e9804,
    0xf000fb03,
    0x5ac34b4b,
    0x6f00f413,
    0x2904d02b,
    0x2a36d103,
    0x2236bf28,
    0x2908e03e,
    0x2a53d103,
    0x2253bf28,
    0x290ae038,
    0xf240d105,
    0x429a136f,
    0x461abf28,
    0x290be030,
    0xf5b2d105,
    0xbf287f0a,
    0x720af44f,
    0x290ee028,
    0xf240d105,
    0x429a23a7,
    0x461abf28,
    0x290fe020,
    0xf240d11e,
    0x429a33fd,
    0x461abf28,
    0x2902e018,
    0x2a12d103,
    0x2212bf28,
    0x2904e012,
    0x2a1bd103,
    0x221bbf28,
    0x290be00c,
    0x2ab7d103,
    0x22b7bf28,
    0x290fe006,
    0xf240d104,
    0x429a1353,
    0x461abf28,
    0x601a4b22,
    0x0088eb08,
    0x3010f837,
    0xf423b29b,
    0x4c217300,
    0xea438824,
    0xb29b2344,
    0x3010f827,
    0x3010f837,
    0xf023b29b,
    0xea430378,
    0xf82701c1,
    0x4b171010,
    0xf043681b,
    0xea430304,
    0xb29b03c2,
    0x3010f829,
    0x30fcf8d5,
    0x4a128c1b,
    0x3010f822,
    0x3010f83a,
    0x030ef3c3,
    0x3010f82a,
    0x30c2f895,
    0xf383fab3,
    0xf885095b,
    0xf89530c2,
    0x330130c3,
    0x30c3f885,
    0x68134a04,
    0x60133b01,
    0xb9f9f625,
    0xb855f625,
    0xb9faf625,
    0xc0006864,///test_mode_acl_rx_flag
    0xc000686c,///loopback_length
    0xc0006870,///loopback_llid
    0xd02115d4,
    0xc0006874,///rxseq_flag
    0xd0211152,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_testmode_2 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch29_ins_test_data_2),
    0x0002bca0,
    0xbf04f1da,
    0xc0006aac,
    (uint8_t *)bes2300_patch29_ins_test_data_2
};///test mode: ld_acl_tx_prog


#if 0
const uint32_t bes2300_patch30_ins_test_data_2[] =
{
    0xf014d002,
    0xd0010f24,
    0xbf62f625,
    0xb854f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_testmode_2 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_test_data_2),
    0x0002cb0c,
    0xb898f1da,
    0xc0006c40,
    (uint8_t *)bes2300_patch30_ins_test_data_2
};///ld_acl_rx:skip rx crcerr
#else
const uint32_t bes2300_patch30_ins_test_data_2[] =
{
    0xf014d006,
    0xd0050f24,
    0x0f03f1b8,
    0xbf00d002,
    0xbf5ef625,
    0xb850f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_testmode_2 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_test_data_2),
    0x0002cb0c,
    0xb898f1da,
    0xc0006c40,
    (uint8_t *)bes2300_patch30_ins_test_data_2
};///ld_acl_rx:skip rx crcerr

#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_testmode_2 =
{
    31,
    BTDRV_PATCH_ACT,
    0,
    0x00000100,
    0x20004b15,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset

const uint32_t bes2300_patch32_ins_test_data_2[] =
{
    0x236e9803,
    0xf000fb03,
    0x5ac34b0b,
    0x6f00f413,
    0xf1b8d108,
    0xd1050f0f,
    0x68134a08,
    0x0301f023,
    0xe0056013,
    0x68134a05,
    0x0301f043,
    0xbf006013,
    0x0408f004,
    0xbf36f625,
    0xd02114c2,
    0xd03503a0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch32_testmode_2 =
{
    32,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch32_ins_test_data_2),
    0x0002cb10,
    0xb8aef1da,
    0xc0006c70,
    (uint8_t*)bes2300_patch32_ins_test_data_2
};///test mode: about DH5 set d03503a0 bit0=0 in ld_acl_rx

static const uint32_t ins_patch_2300_config_testmode[] =
{
    33,
    (uint32_t)&bes2300_ins_patch0_testmode_2,
    (uint32_t)&bes2300_ins_patch1_testmode_2,
    (uint32_t)&bes2300_ins_patch2_testmode_2,
    (uint32_t)&bes2300_ins_patch3_testmode_2,
    (uint32_t)&bes2300_ins_patch4_testmode_2,
    (uint32_t)&bes2300_ins_patch5_testmode_2,
    (uint32_t)&bes2300_ins_patch6_testmode_2,
    (uint32_t)&bes2300_ins_patch7_testmode_2,
    (uint32_t)&bes2300_ins_patch8_testmode_2,
    (uint32_t)&bes2300_ins_patch9_testmode_2,
    (uint32_t)&bes2300_ins_patch10_testmode_2,
    (uint32_t)&bes2300_ins_patch11_testmode_2,
    (uint32_t)&bes2300_ins_patch12_testmode_2,
    (uint32_t)&bes2300_ins_patch13_testmode_2,
    (uint32_t)&bes2300_ins_patch14_testmode_2,
    (uint32_t)&bes2300_ins_patch15_testmode_2,
    (uint32_t)&bes2300_ins_patch16_testmode_2,
    (uint32_t)&bes2300_ins_patch17_testmode_2,
    (uint32_t)&bes2300_ins_patch18_testmode_2,
    (uint32_t)&bes2300_ins_patch19_testmode_2,
    (uint32_t)&bes2300_ins_patch20_testmode_2,
    (uint32_t)&bes2300_ins_patch21_testmode_2,
    (uint32_t)&bes2300_ins_patch22_testmode_2,
    (uint32_t)&bes2300_ins_patch23_testmode_2,
    (uint32_t)&bes2300_ins_patch24_testmode_2,
    (uint32_t)&bes2300_ins_patch25_testmode_2,
    (uint32_t)&bes2300_ins_patch26_testmode_2,
    (uint32_t)&bes2300_ins_patch27_testmode_2,
    (uint32_t)&bes2300_ins_patch28_testmode_2,
    (uint32_t)&bes2300_ins_patch29_testmode_2,
    (uint32_t)&bes2300_ins_patch30_testmode_2,
    (uint32_t)&bes2300_ins_patch31_testmode_2,
    (uint32_t)&bes2300_ins_patch32_testmode_2,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0_testmode_3 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00024bcc,
    0xbf00bd10,
    0,
    NULL
};//ld_channel_assess


const BTDRV_PATCH_STRUCT bes2300_ins_patch1_testmode_3 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0003ed7c,
#ifdef BLE_POWER_LEVEL_0
    0xbf002300,
#else
    0xbf002302,
#endif
    0,
    NULL
};//ble power level



const uint32_t bes2300_patch2_ins_test_data_3[] =
{
    0x30fcf8d4,
    0xd0092b00,
    0x680b4905,
    0x3f00f413,
    0x680bd004,
    0x3300f423,
    0xbf00600b,
    0xb8c0f628,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_testmode_3 =
{
    2,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch2_ins_test_data_3),
    0x0002e998,
    0xbf32f1d7,
    0xc0006800,
    (uint8_t *)bes2300_patch2_ins_test_data_3
};



const uint32_t bes2300_patch3_ins_test_data_3[] =
{
    0x30fcf8d4,
    0xd00b2b00,
    0xf003789b,
    0x2b0503fd,
    0x4b04d106,
    0xf443681b,
    0x4a023300,
    0xbf006013,
    0xbbcbf626,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch3_testmode_3 =
{
    3,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch3_ins_test_data_3),
    0x0002cf08,
    0xbc92f1d9,
    0xc0006830,
    (uint8_t *)bes2300_patch3_ins_test_data_3
};


const uint32_t bes2300_patch4_ins_test_data_3[] =
{
    0xf0025bf2,
    0xbf00080f,
    0xba2bf623,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch4_testmode_3 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_test_data_3),
    0x00029cbc,
    0xbdd0f1dc,
    0xc0006860,
    (uint8_t *)bes2300_patch4_ins_test_data_3
};


const uint32_t bes2300_patch5_ins_test_data_3[] =
{
    0xf0025bf2,
    0xbf00020f,
    0xba49f623,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_testmode_3 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch5_ins_test_data_3),
    0x00029d08,
    0xbdb2f1dc,
    0xc0006870,
    (uint8_t *)bes2300_patch5_ins_test_data_3
};



const uint32_t bes2300_patch6_ins_test_data_3[] =
{
    0xf0035bf3,
    0xbf00080f,
    0xba58f623,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6_testmode_3 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_test_data_3),
    0x00029d38,
    0xbda2f1dc,
    0xc0006880,
    (uint8_t *)bes2300_patch6_ins_test_data_3
};



const uint32_t bes2300_patch7_ins_test_data_3[] =
{
    0xf0035bf3,
    0xbf00010f,
    0xba78f623,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch7_testmode_3 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch7_ins_test_data_3),
    0x00029d88,
    0xbd82f1dc,
    0xc0006890,
    (uint8_t *)bes2300_patch7_ins_test_data_3
};


#ifdef __POWER_CONTROL_TYPE_1__

const uint32_t bes2300_patch8_ins_test_data_3[] =
{
    0xf0135243,
    0x2b00030f,
    0x2000bf14,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch8_testmode_3 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch8_ins_test_data_3),
    0x00003498,
    0xba50f203,
    0xc000693c,
    (uint8_t *)bes2300_patch8_ins_test_data_3
};//pwr controll
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch8_testmode_3 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x000034d0,
    0xe0072200,
    0,
    NULL
};

#endif


/////for test mode power control
const BTDRV_PATCH_STRUCT bes2300_ins_patch9_testmode_3 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x00029aa8,
    0xe0cfbf00,
    0,
    NULL
};


const uint32_t bes2300_patch10_ins_test_data_3[] =
{
    0xb2db1e6b,
    0xbf002b01,
    0x30fcf8db,
    0xf002789a,
    0x2a0502fd,
    0x4a2ad14a,
    0x11cbf892,
    0x4a2a0109,
    0xf3c25a8a,
    0x48292240,
    0x49216002,
    0x3201680a,
    0x4a20600a,
    0x8000f8c2,
    0xf3c29a05,
    0x491e02c9,
    0x491f600a,
    0x11cbf891,
    0x01c1ebc1,
    0xf830481e,
    0xf3c11011,
    0xbf002100,
    0xb119bf00,
    0x68014816,
    0x6001481f,
    0x600d4915,
    0x68004819,
    0x2100b150,
    0x60014817,
    0x68014817,
    0x6001480f,
    0x68014816,
    0x6001480e,
    0xf1008c18,
    0xf5004050,
    0x4b0c1004,
    0x31cbf893,
    0x03c3ebc3,
    0xf831490a,
    0xb2891013,
    0x4150f101,
    0x1104f501,
    0xf8f4f641,
    0xbf4ff626,
    0xc0006aa4,///test_mode_acl_rx_flag
    0xc0006aa8,///loopback_type
    0xc0006aac,///loopback_length
    0xc0006ab0,///loopback_llid
    0xc0005c90,
    0xd02115a0,
    0xd021159a,
    0xc0006ab4,///rxseq_flag
    0xc0006ab8,///unack_seqerr_flag
    0xc0006abc,///rxlength_flag
    0xc0006ac0,///rxllid_flag
    0xc0006ac4,///rxarqn_flag
    0xc0006ac8,///ok_rx_length_flag
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch10_testmode_3 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch10_ins_test_data_3),
    0x0002d904,
    0xb85cf1d9,
    0xc00069c0,
    (uint8_t *)bes2300_patch10_ins_test_data_3
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch11_testmode_3 =
{
    11,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};
/////end for test mode power control



const uint32_t bes2300_patch12_ins_test_data_3[] =
{
    0x20014b04,
    0x4b026018,
    0xbf00689b,
    0xb958f611,
    0xc0004168,
    0xc00068b8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_testmode_3 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch12_ins_test_data_3),
    0x00017b5c,
    0xbea0f1ee,
    0xc00068a0,
    (uint8_t *)bes2300_patch12_ins_test_data_3
};


const uint32_t bes2300_patch13_ins_test_data_3[] =
{
    0x20024b04,
    0x4b026018,
    0xbf00689b,
    0xbaecf612,
    0xc0004168,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_testmode_3 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch13_ins_test_data_3),
    0x00018ea4,
    0xbd0cf1ed,
    0xc00068c0,
    (uint8_t *)bes2300_patch13_ins_test_data_3
};


const uint32_t bes2300_patch14_ins_test_data_3[] =
{
    0x20014b04,
    0x4b026018,
    0xbf0068db,
    0xb964f611,
    0xc0004168,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_testmode_3 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_test_data_3),
    0x00017bb4,
    0xbe94f1ee,
    0xc00068e0,
    (uint8_t *)bes2300_patch14_ins_test_data_3
};


const uint32_t bes2300_patch15_ins_test_data_3[] =
{
    0x20024b04,
    0x4b026018,
    0x462068db,
    0xbad6f612,
    0xc0004168,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch15_testmode_3 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_test_data_3),
    0x00018eb8,
    0xbd22f1ed,
    0xc0006900,
    (uint8_t *)bes2300_patch15_ins_test_data_3
};


const uint32_t bes2300_patch16_ins_test_data_3[] =
{
    0xd91b429a,
    0x5a42b410,
    0xb2dc3301,
    0x427ff402,
    0x52424322,
    0x68104a0c,
    0xd00a2801,
    0xf8924a0b,
    0x42830030,
    0x2000bf14,
    0x4a072001,
    0x60112100,
    0x2000e001,
    0xf85d6010,
    0x47704b04,
    0x20004b02,
    0x20016018,
    0xbf004770,
    0xc00068b8,
    0xc0004168,


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_testmode_3 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch16_ins_test_data_3),
    0x000034bc,
    0xba40f203,
    0xc0006940,
    (uint8_t *)bes2300_patch16_ins_test_data_3
};



const uint32_t bes2300_patch17_ins_test_data_3[] =
{
    0xf0135a43,
    0xd016030f,
    0x3b015a42,
    0xf402b2db,
    0x4313427f,
    0xf0135243,
    0x4a08030f,
    0x28016810,
    0x2b00d006,
    0x2000bf14,
    0x21002001,
    0xe0016011,
    0x60102000,
    0x20014770,
    0xbf004770,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_testmode_3 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_test_data_3),
    0x00003484,
    0xbb44f203,
    0xc0006b10,
    (uint8_t *)bes2300_patch17_ins_test_data_3
};


const uint32_t bes2300_patch18_ins_test_data_3[] =
{
    0x60b39b01,
    0xfb00206e,
    0x4a0bf004,
    0xf0436813,
    0x60130302,
    0x4b092202,
    0xf5a3601a,
    0x3bb6436e,
    0x3003f830,
    0x030cf3c3,
    0x5300f443,
    0xf8204a04,
    0xbf003002,
    0xbeaaf625,
    0xD022000C,
    0xd0220018,
    0xd0211162,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_testmode_3 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch18_ins_test_data_3),
    0x0002c8e8,
    0xb93af1da,
    0xc0006b60,
    (uint8_t *)bes2300_patch18_ins_test_data_3
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_testmode_3 =
{
    19,
    BTDRV_PATCH_ACT,
    0,
    0x00038180,
    0xf7c92100,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_testmode_3 =
{
    20,
    BTDRV_PATCH_ACT,
    0,
    0x00003444,
    0xe00f382d,
    0,
    NULL
};///hw rssi read

const uint32_t bes2300_patch21_ins_test_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbca0f626
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_testmode_3 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch21_ins_test_data_3),
    0x0002d4f4,
    0xbb5cf1d9,
    0xc0006bb0,
    (uint8_t *)bes2300_patch21_ins_test_data_3
};//ld_acl_rx

const uint32_t bes2300_patch22_ins_test_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbecaf61e
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_testmode_3 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch22_ins_test_data_3),
    0x00025954,
    0xb932f1e1,
    0xc0006bbc,
    (uint8_t *)bes2300_patch22_ins_test_data_3
};//ld_inq_rx

const uint32_t bes2300_patch23_ins_test_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbe08f63a
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_testmode_3 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_test_data_3),
    0x000417dc,
    0xb9f4f1c5,
    0xc0006bc8,
    (uint8_t *)bes2300_patch23_ins_test_data_3
};//lld_pdu_rx_handler


const BTDRV_PATCH_STRUCT bes2300_ins_patch24_testmode_3 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x00024fb0,
    0xbf00e002,
    0,
    NULL,
};

const uint32_t bes2300_patch25_ins_test_data_3[] =
{
    0x3a063006,
    0xf847f641,
    0xb958f61e,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_testmode_3 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch25_ins_test_data_3),
    0x00024e8c,
    0xbea2f1e1,
    0xc0006bd4,
    (uint8_t *)bes2300_patch25_ins_test_data_3
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch26_testmode_3 =
{
    26,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x00006d14,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

const uint32_t bes2300_patch27_ins_test_data_3[] =
{
    0x0f03f1b8,
    0xf1b8d002,
    0xd1070f08,
    0x30bbf89b,
    0xbf003301,
    0x2401bf00,
    0xe019bf00,
    0xf8934b0d,
    0xebc331cb,
    0x4a0c03c3,
    0x3013f832,
    0x2300f3c3,
    0x60134a0f,
    0x4b09b953,
    0x4b0c681a,
    0x4b08601a,
    0x4b08681a,
    0x2201601a,
    0x601a4b07,
    0xbdacf626,
    0xbe6df626,
    0xc0005c90,
    0xd021159a,
    0xc0006ab0,///loopback_llid
    0xc0006aac,///loopback_length
    0xc0006abc,///rxlength_flag
    0xc0006ab8,///unack_seqerr_flag
    0xc0006ac0,///rxllid_flag
    0xc0006ac4,////rxarqn_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch27_testmode_3 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch27_ins_test_data_3),
    0x0002d780,
    0xba2ef1d9,
    0xc0006be0,
    (uint8_t *)bes2300_patch27_ins_test_data_3
};///ld_acl_rx:skip rxseq_err


const uint32_t bes2300_patch28_ins_test_data_3[] =
{
    0xbf00789a,
    0xb2d23a05,
    0xd9012a03,
    0xb8a2f000,
    0xd0022a00,
    0xf0402a02,
    0xbf00809f,
    0x681b4b4f,
    0xf0002b00,
    0xf8d58099,
    0x881a30fc,
    0xf00179d9,
    0xb90a010f,
    0x881a4b4a,
    0x236e9804,
    0xf000fb03,
    0x5ac34b4b,
    0x6f00f413,
    0x2904d02b,
    0x2a36d103,
    0x2236bf28,
    0x2908e03e,
    0x2a53d103,
    0x2253bf28,
    0x290ae038,
    0xf240d105,
    0x429a136f,
    0x461abf28,
    0x290be030,
    0xf5b2d105,
    0xbf287f0a,
    0x720af44f,
    0x290ee028,
    0xf240d105,
    0x429a23a7,
    0x461abf28,
    0x290fe020,
    0xf240d11e,
    0x429a33fd,
    0x461abf28,
    0x2902e018,
    0x2a12d103,
    0x2212bf28,
    0x2904e012,
    0x2a1bd103,
    0x221bbf28,
    0x290be00c,
    0x2ab7d103,
    0x22b7bf28,
    0x290fe006,
    0xf240d104,
    0x429a1353,
    0x461abf28,
    0x601a4b22,
    0x0484eb04,
    0x3014f837,
    0xf423b29b,
    0x48217300,
    0xea438800,
    0xb29b2340,
    0x3014f827,
    0x3014f837,
    0xf023b29b,
    0xea430378,
    0xf82701c1,
    0x4b171014,
    0xf043681b,
    0xea430304,
    0xb29b03c2,
    0x3014f82a,
    0x30fcf8d5,
    0x4a128c1b,
    0x3014f822,
    0x3014f836,
    0x030ef3c3,
    0x3014f826,
    0x30c2f895,
    0xf383fab3,
    0xf885095b,
    0xf89530c2,
    0x330130c3,
    0x30c3f885,
    0x68134a04,
    0x60133b01,
    0xb81af626,
    0xbeaaf625,
    0xb81bf626,
    0xc0006aa4,///test_mode_acl_rx_flag
    0xc0006aac,///loopback_length
    0xc0006ab0,///loopback_llid
    0xd02115d4,
    0xc0006ab4,///rxseq_flag
    0xd0211152,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch28_testmode_3 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_test_data_3),
    0x0002cb0c,
    0xb8aef1da,
    0xc0006c6c,
    (uint8_t *)bes2300_patch28_ins_test_data_3
};///test mode: ld_acl_tx_prog


#if 0
const uint32_t bes2300_patch29_ins_test_data_3[] =
{
    0xf014d002,
    0xd0010f24,
    0xbcc2f626,
    0xbd76f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_testmode_3 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch29_ins_test_data_3),
    0x0002d78c,
    0xbb38f1d9,
    0xc0006e00,
    (uint8_t*)bes2300_patch29_ins_test_data_3
};///test mode: skip crc err in ld_acl_rx
#else

const uint32_t bes2300_patch29_ins_test_data_3[] =
{
    0xf014d006,
    0xd0050f24,
    0x0f03f1b8,
    0xbf00d002,
    0xbc9ef626,
    0xbd52f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_testmode_3 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch29_ins_test_data_3),
    0x0002d78c,
    0xbb58f1d9,
    0xc0006e40,
    (uint8_t*)bes2300_patch29_ins_test_data_3
};///test mode: skip crc err in ld_acl_rx


#endif

const uint32_t bes2300_patch30_ins_test_data_3[] =
{
    0xfbb24a02,
    0xf5f9f3f3,
    0xbf00bb3a,
    0x9af8da00,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_testmode_3 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_test_data_3),
    0x00000650,
    0xbcc2f206,
    0xc0006fd8,
    (uint8_t *)bes2300_patch30_ins_test_data_3
};//lpo timer

const uint32_t bes2300_patch31_ins_test_data_3[] =
{
    0x02004638,
    0x0001f040,
    0xfcf8f60c,
    0xbf60f61b,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_testmode_3 =
{
    31,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch31_ins_test_data_3),
    0x00022ebc,
    0xb8acf1e4,
    0xc0007018,
    (uint8_t *)bes2300_patch31_ins_test_data_3
};//lc_send_lmp

const BTDRV_PATCH_STRUCT bes2300_ins_patch32_testmode_3 =
{
    32,
    BTDRV_PATCH_INACT,
    0,
    0x0002e430,
    0xe0022000,
    0,
    NULL
};///ld_acl_stop


const BTDRV_PATCH_STRUCT bes2300_ins_patch33_testmode_3 =
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0x0002d9f4,
    0xe053bf00,
    0,
    NULL
};//disable power control



const BTDRV_PATCH_STRUCT bes2300_ins_patch34_testmode_3 =
{
    34,
    BTDRV_PATCH_ACT,
    0,
    0x00006bcc,
    0xbf00e016,
    0,
    NULL
};///disable assert warning



const uint32_t bes2300_patch35_ins_test_data_3[] =
{
    0x880b4904,
    0xf423b29b,
    0x021263e0,
    0x800b4313,
    0xbbc5f639,
    0xd02101d0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch35_testmode_3 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch35_ins_test_data_3),
    0x000405b4,
    0xbc34f1c6,
    0xc0006e20,
    (uint8_t *)bes2300_patch35_ins_test_data_3
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch36_testmode_3 =
{
    36,
    BTDRV_PATCH_ACT,
    0,
    0x000406f4,
    0x0106f248,
    0,
    NULL,
};//ble win cntl

const BTDRV_PATCH_STRUCT bes2300_ins_patch37_testmode_3 =
{
    37,
    BTDRV_PATCH_ACT,
    0,
    0x00000100,
    0x20004b15,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset


const uint32_t bes2300_patch38_ins_test_data_3[] =
{
    0x4681b083,
    0x4b022200,
    0x2020f843,
    0xbf94f621,
    0xc0007044,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_testmode_3 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_test_data_3),
    0x00028f64,
    0xb864f1de,
    0xc0007030,
    (uint8_t *)bes2300_patch38_ins_test_data_3
};//swagc patch


const uint32_t bes2300_patch39_ins_test_data_3[] =
{
    0x6020f853,
    0xf8524a0d,
    0x33013020,
    0x3020f842,
    0x68124a0b,
    0xd90f4293,
    0x4a082300,
    0x3020f842,
    0xb082b402,
    0x92004a08,
    0xf06f6a14,
    0x46290059,
    0x47a02201,
    0xbc02b002,
    0xbeb2f621,
    0xc0007044,
    0xc0007094,
    0x00000010,
    0xc0004168,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch39_testmode_3 =
{
    39,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch39_ins_test_data_3),
    0x00028dec,
    0xb930f1de,
    0xc0007050,
    (uint8_t *)bes2300_patch39_ins_test_data_3
};

const uint32_t bes2300_patch40_ins_test_data_3[] =
{
    0x2255f24c,
    0x601a4b03,
    0x68134a01,
    0xb9a2f632,
    0xd0220200,
    0xd02101d0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch40_testmode_3 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch40_ins_test_data_3),
    0x000391b0,
    0xbe56f1cd,
    0xc0006e60,
    (uint8_t *)bes2300_patch40_ins_test_data_3
};///ble rx test mode: set rxgain idx = 0

const uint32_t bes2300_patch41_ins_test_data_3[] =
{
    0xf8802300,
    0xf8803020,
    0xf8943021,
    0x2b0130b3,
    0x2300d102,
    0xbfd2f62a,
    0xbfdef62a,
};/* 6e80-6e98 */

const BTDRV_PATCH_STRUCT bes2300_ins_patch41_testmode_3 =
{
    41,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch41_ins_test_data_3),
    0x00031e38,
    0xb822f1d5,
    0xc0006e80,
    (uint8_t *)bes2300_patch41_ins_test_data_3
};//nosig test pattern problem

const uint32_t bes2300_patch42_ins_test_data_3[] =
{
    0x236e9803,
    0xf000fb03,
    0x5ac34b0b,
    0x6f00f413,
    0xf1b8d108,
    0xd1050f0f,
    0x68134a08,
    0x0301f023,
    0xe0056013,
    0x68134a05,
    0x0301f043,
    0xbf006013,
    0x30fcf8db,
    0xbccaf626,
    0xd0211152,
    0xd03503a0,
};/* 6ea0- 6edc*/
const BTDRV_PATCH_STRUCT bes2300_ins_patch42_testmode_3 =
{
    42,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch42_ins_test_data_3),
    0x0002d868,
    0xbb1af1d9,
    0xc0006ea0,
    (uint8_t*)bes2300_patch42_ins_test_data_3
};///test mode: about DH5 set d03503a0 bit0=0 in ld_acl_rx

static const uint32_t ins_patch_2300_config_testmode_3[] =
{
    43,
    (uint32_t)&bes2300_ins_patch0_testmode_3,
    (uint32_t)&bes2300_ins_patch1_testmode_3,
    (uint32_t)&bes2300_ins_patch2_testmode_3,
    (uint32_t)&bes2300_ins_patch3_testmode_3,
    (uint32_t)&bes2300_ins_patch4_testmode_3,
    (uint32_t)&bes2300_ins_patch5_testmode_3,
    (uint32_t)&bes2300_ins_patch6_testmode_3,
    (uint32_t)&bes2300_ins_patch7_testmode_3,
    (uint32_t)&bes2300_ins_patch8_testmode_3,
    (uint32_t)&bes2300_ins_patch9_testmode_3,
    (uint32_t)&bes2300_ins_patch10_testmode_3,
    (uint32_t)&bes2300_ins_patch11_testmode_3,
    (uint32_t)&bes2300_ins_patch12_testmode_3,
    (uint32_t)&bes2300_ins_patch13_testmode_3,
    (uint32_t)&bes2300_ins_patch14_testmode_3,
    (uint32_t)&bes2300_ins_patch15_testmode_3,
    (uint32_t)&bes2300_ins_patch16_testmode_3,
    (uint32_t)&bes2300_ins_patch17_testmode_3,
    (uint32_t)&bes2300_ins_patch18_testmode_3,
    (uint32_t)&bes2300_ins_patch19_testmode_3,
    (uint32_t)&bes2300_ins_patch20_testmode_3,
    (uint32_t)&bes2300_ins_patch21_testmode_3,
    (uint32_t)&bes2300_ins_patch22_testmode_3,
    (uint32_t)&bes2300_ins_patch23_testmode_3,
    (uint32_t)&bes2300_ins_patch24_testmode_3,
    (uint32_t)&bes2300_ins_patch25_testmode_3,
    (uint32_t)&bes2300_ins_patch26_testmode_3,
    (uint32_t)&bes2300_ins_patch27_testmode_3,
    (uint32_t)&bes2300_ins_patch28_testmode_3,
    (uint32_t)&bes2300_ins_patch29_testmode_3,
    (uint32_t)&bes2300_ins_patch30_testmode_3,
    (uint32_t)&bes2300_ins_patch31_testmode_3,
    (uint32_t)&bes2300_ins_patch32_testmode_3,
    (uint32_t)&bes2300_ins_patch33_testmode_3,
    (uint32_t)&bes2300_ins_patch34_testmode_3,
    (uint32_t)&bes2300_ins_patch35_testmode_3,
    (uint32_t)&bes2300_ins_patch36_testmode_3,
    (uint32_t)&bes2300_ins_patch37_testmode_3,
    (uint32_t)&bes2300_ins_patch38_testmode_3,
    (uint32_t)&bes2300_ins_patch39_testmode_3,
    (uint32_t)&bes2300_ins_patch40_testmode_3,
    (uint32_t)&bes2300_ins_patch41_testmode_3,
    (uint32_t)&bes2300_ins_patch42_testmode_3,
};


void btdrv_ins_patch_test_init(void)
{

    const BTDRV_PATCH_STRUCT *ins_patch_p;

    btdrv_patch_en(0);

    for(uint8_t i=0; i<56; i++)
    {
        btdrv_ins_patch_disable(i);
    }

    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0; i<ins_patch_2300_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode[i+1]);
        }
#ifdef BLE_POWER_LEVEL_0
        *(uint32_t *)0xd02101d0 = 0xf855;
#endif


    }
    else if(hal_get_chip_metal_id()>=HAL_CHIP_METAL_ID_5)
    {
        for(uint8_t i=0; i<ins_patch_2300_config_testmode_3[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode_3[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode_3[i+1]);
        }
    }
    btdrv_patch_en(1);
}

void btdrv_dynamic_patch_moble_disconnect_reason_hacker(uint16_t hciHandle)
{
    uint32_t lc_ptr=0;
    uint32_t disconnect_reason_address = 0;
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        lc_ptr = *(uint32_t *)(0xc0005b48+(hciHandle-0x80)*4);
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        lc_ptr = *(uint32_t *)(0xc0005bcc+(hciHandle-0x80)*4);
    }
    //BT_DRV_TRACE(2,"lc_ptr %x, disconnect_reason_addr %x",lc_ptr,lc_ptr+66);
    if(lc_ptr == 0)
    {
        return;
    }
    else
    {
        disconnect_reason_address = (uint32_t)(lc_ptr+66);
        BT_DRV_TRACE(3,"%s:hdl=%x reason=%d",__func__,hciHandle,*(uint8_t *)(disconnect_reason_address));
        *(uint8_t *)(disconnect_reason_address) = 0x0;
        BT_DRV_TRACE(1,"After op,reason=%d",*(uint8_t *)(disconnect_reason_address));
        return;
    }
}

void btdrv_preferre_rate_send(uint16_t conhdl,uint8_t rate)
{
#ifdef __SEND_PREFERRE_RATE__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        ASSERT(*(uint32_t *)0xc0006e80 ==0xc0006e84,"ERROR PATCH FOR PREFERRE RATE!");
        ASSERT(conhdl>=0x80,"ERROR CONNECT HANDLE");
        ASSERT(conhdl<=0x83,"ERROR CONNECT HANDLE");
        *(uint32_t *)0xc0006e8c = rate;
        *(uint32_t *)0xc0006e84 = conhdl-0x80;
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        ASSERT(*(uint32_t *)0xc00068c0 ==0xc00068c4,"ERROR PATCH FOR PREFERRE RATE!");
        ASSERT(conhdl>=0x80,"ERROR CONNECT HANDLE");
        ASSERT(conhdl<=0x83,"ERROR CONNECT HANDLE");
        *(uint32_t *)0xc00068cc = rate;
        *(uint32_t *)0xc00068c4 = conhdl-0x80;
    }

#endif
}

void btdrv_preferre_rate_clear(void)
{
#ifdef __SEND_PREFERRE_RATE__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        ASSERT(*(uint32_t *)0xc0006e80 ==0xc0006e84,"ERROR PATCH FOR PREFERRE RATE!");
        *(uint32_t *)0xc0006e84 = 0xff;
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        ASSERT(*(uint32_t *)0xc00068c0 ==0xc00068c4,"ERROR PATCH FOR PREFERRE RATE!");
        *(uint32_t *)0xc00068c0 = 0xff;
    }
#endif
}
void btdrv_dynamic_patch_sco_status_clear(void)
{
    return;
}


void btdrv_seq_bak_mode(uint8_t mode,uint8_t linkid)
{
#ifdef __SW_SEQ_FILTER__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        uint32_t seq_bak_ptr = 0xc0006c8c;
        BT_DRV_TRACE(2,"btdrv_seq_bak_mode mode=%x,linkid = %x",mode,linkid);
        uint32_t val = *(uint32_t *)seq_bak_ptr;
        val &= ~(0xff<<(linkid*8));
        if(mode ==1)//en
        {
            val |=(0x2<<linkid*8);
        }
        else if(mode ==0)
        {
            val =0xffffffff;
        }
        *(uint32_t *)seq_bak_ptr = val;

        BT_DRV_TRACE(1,"val=%x",val);
    }
#endif
}



void bt_drv_patch_cvsd_check_check(void)
{
    if(*(uint32_t *)0xc000430c !=0xf8534b39  || *(uint32_t *)0xc0004404 !=0xd0220150)
    {
        ASSERT(0,"patch is rewrite on the address 0xc000430c");

    }
}


////////////////////////////////bt 5.0 /////////////////////////////////

const BTDRV_PATCH_STRUCT bes2300_50_ins_patch0 =
{
    0,
    BTDRV_PATCH_INACT,
    0,
    0x00056138,
    0xe0042300,
    0,
    NULL
};//disable reslove to



const uint32_t bes2300_50_patch1_ins_data[] =
{
    0x0f02f013,
    0xf008bf12,
    0x79330303,
    0x0303ea08,
    0x300bf889,
    0xf01378b3,
    0xbf120f01,
    0x0303f008,
    0xea0878f3,
    0xf8890303,
    0xbf00300a,
    0xbf6ff659,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch1 =
{
    1,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_50_patch1_ins_data),
    0x0005f6e8,
    0xb88af1a6,
    0xc0005800,
    (uint8_t *)bes2300_50_patch1_ins_data
};


const uint32_t bes2300_50_patch2_ins_data[] =
{
    0xf01378cb,
    0xd1080f04,
    0xf013790b,
    0xd1040f04,
    0xf003788b,
    0xf6590202,
    0x2211bf16,
    0xbf99f659,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch2 =
{
    2,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_50_patch2_ins_data),
    0x0005f680,
    0xb8def1a6,
    0xc0005840,
    (uint8_t *)bes2300_50_patch2_ins_data
};///SET PHY CMD



const uint32_t bes2300_50_patch3_ins_data[] =
{
    0x4b034902,
    0x4b036019,
    0xb8c5f64b,
    0x00800190,
    0xc0004af4,
    0xc00002d4,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch3_ins_data),
    0x00050a70,
    0xbf36f1b4,
    0xc00058e0,
    (uint8_t *)bes2300_50_patch3_ins_data
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x000676c4,
    0x0301f043,
    0,
    NULL
};



static const uint32_t best2300_50_ins_patch_config[] =
{
    5,
    (uint32_t)&bes2300_50_ins_patch0,
    (uint32_t)&bes2300_50_ins_patch1,
    (uint32_t)&bes2300_50_ins_patch2,
    (uint32_t)&bes2300_50_ins_patch3,
    (uint32_t)&bes2300_50_ins_patch4,

};


const uint32_t bes2300_50_patch0_ins_data_2[] =
{
    0x4b034902,
    0x4b036019,
    0xbe25f64b,
    0x00800190,
    0xc0004bc0,
    0xc0000390,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch0_2 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch0_ins_data_2),
    0x00050f30,
    0xb9d6f1b4,
    0xc00052e0,
    (uint8_t *)bes2300_50_patch0_ins_data_2
};//sleep patch


const uint32_t bes2300_50_patch1_ins_data_2[] =
{
    0xf8934b1e,
    0x2b0030dc,
    0xb470d132,
    0x4b1cb082,
    0x4d1c8818,
    0x4e1c882b,
    0x2303f3c3,
    0x69f2b2c0,
    0x1000ea43,
    0x882c4790,
    0x9600b2a4,
    0x21006a36,
    0x2301460a,
    0x882b47b0,
    0x2403f3c4,
    0x2303f3c3,
    0xd013429c,
    0x68194b11,
    0x4b112200,
    0x3bd0601a,
    0xf422681a,
    0x601a4280,
    0xf042681a,
    0x601a6280,
    0x4b0c2201,
    0x2302601a,
    0x302af881,
    0xbc70b002,
    0xbf00bf00,
    0xff26f65a,
    0xbc61f662,
    0xc00050d0,
    0xd061074c,
    0xd0610160,
    0xc00048b8,
    0xc00008c4,
    0xd06200d0,
    0xc00053a4,
    0x00000000,






};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch1_2 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch1_ins_data_2),
    0x00067c44,
    0xbb62f19d,
    0xc000530c,
    (uint8_t *)bes2300_50_patch1_ins_data_2
};

const uint32_t bes2300_50_patch2_ins_data_2[] =
{

    0xf6684620,
    0x4b09fd2d,
    0x2b01681b,
    0xf894d10c,
    0x2b02302a,
    0xf662d108,
    0x4805fbc5,
    0xfc58f662,
    0x4b022200,
    0xbd38601a,
    0xbbd1f662,
    0xc00053a4,
    0xc00053e4,
    0x00000000,
    0x00000000,


};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch2_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch2_ins_data_2),
    0x00067b78,
    0xbc1af19d,
    0xc00053b0,
    (uint8_t *)bes2300_50_patch2_ins_data_2
};



const uint32_t bes2300_50_patch3_ins_data_2[] =
{

    0x4b046013,
    0x601a6822,
    0x711a7922,
    0xbf002000,
    0xbc7cf662,
    0xc00053e4,


};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch3_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch3_ins_data_2),
    0x00067d08,
    0xbb7af19d,
    0xc0005400,
    (uint8_t *)bes2300_50_patch3_ins_data_2
};


static const uint32_t best2300_50_ins_patch_config_2[] =
{
    4,
    (uint32_t)&bes2300_50_ins_patch0_2,
    (uint32_t)&bes2300_50_ins_patch1_2,
    (uint32_t)&bes2300_50_ins_patch2_2,
    (uint32_t)&bes2300_50_ins_patch3_2,
};

void btdrv_ins_patch_init_50(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0; i<best2300_50_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() < HAL_CHIP_METAL_ID_5)
    {
        for(uint8_t i=0; i<best2300_50_ins_patch_config_2[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config_2[i+1]);
        }
    }
}

const uint32_t dpatch0_data_2300_50[] =
{
    0x29B00033,
    0x0020B000,
    0xB00015B0,
    0x05B0000B,
    0x00F9B000,
    0x7F7F7FB0,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x0000007F

};

const BTDRV_PATCH_STRUCT data_patch0_2300_50 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(dpatch0_data_2300_50),
    0x00051930,
    0xc0005870,
    0xc0005870,
    (uint8_t *)&dpatch0_data_2300_50
};



const uint32_t dpatch1_data_2300_50[] =
{
    0xC1C1BCBC,
    0xDcDcd3d3,
    0xE7E7e2e2,
    0x7f7fEeEe,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x00007F7F,

};

const BTDRV_PATCH_STRUCT data_patch1_2300_50 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(dpatch1_data_2300_50),
    0x00051aa8,
    0xc00058b0,
    0xc00058b0,
    (uint8_t *)&dpatch1_data_2300_50
};


uint32_t data_patch_config_2300_50[] =
{
    2,
    (uint32_t)&data_patch0_2300_50,
    (uint32_t)&data_patch1_2300_50,

};





void btdrv_data_patch_init_50(void)
{
    const BTDRV_PATCH_STRUCT *data_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {

        for(uint8_t i=0; i<data_patch_config_2300_50[0]; i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)data_patch_config_2300_50[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)data_patch_config_2300_50[i+1]);
        }
    }

}


