/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-main.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x005a80f0
// GLOBAL: CARMA2_SW 0x005a3020
tFrontend_spec gFrontend_MAIN = {
#if 0 // FIXME: enable this
    "Main",
    0,
    26,
    MainMenuInfunc,
    MainMenuOutfunc,
    FRONTEND_MainMenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    2,
    2,
    1,
    {
        { 0x9,      OnePlayerSetup,      NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa,      MultiplayerSetup,    &gFrontend_NETWORK,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    testUp,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x403,    testDn,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf,      temp,                &gFrontend_OPTIONS,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NewGameToggleTyping, NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xec,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      StartFudge,          &gFrontend_NEWGAME,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf7,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
    },
    1,
    {
        { 1, 8, 6, 12, 12, 12, 19 },
    }
#else
    0
#endif
};

// GLOBAL: CARMA2_HW 0x005d6410
tFrontend_spec gFrontend_CHANGE_CAR;

// GLOBAL: CARMA2_HW 0x005ed5a0
tFrontend_spec gFrontend_LOAD_GAME;

// GLOBAL: CARMA2_HW 0x00686f08
int gCount_connected_items_indices;

// GLOBAL: CARMA2_HW 0x00687018
int gConnected_items_indices[6];

// GLOBAL: CARMA2_HW 0x00763704
int back_screen_origin_x;

// GLOBAL: CARMA2_HW 0x00763708
int back_screen_origin_y;

// GLOBAL: CARMA2_HW 0x0076370c
int back_screen_base_x;

// GLOBAL: CARMA2_HW 0x00763710
int back_screen_base_y;



// STUB: CARMA2_HW 0x00467280
int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00469a40
int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00469ea0
int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0046a0e0
int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}
