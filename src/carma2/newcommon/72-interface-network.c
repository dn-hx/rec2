/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-network.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x005b39b8
// GLOBAL: CARMA2_SW 0x005ae8e8
tFrontend_spec gFrontend_NETWORK = {
#if 0 // FIXME: enable this
    "Network",
    0,
    31,
    NetworkJoinMenuInfunc,
    NetworkJoinMenuOutfunc,
    FRONTEND_NetworkJoinMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    3,
    3,
    2,
    {
        { 0x98,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa3,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    NetGameTypeUp,          NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetGameTypeDn,          NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe4,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    NetRaceUp,              NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetRaceDn,              NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe5,     temp,                   &gFrontend_NETWORK_OPTIONS,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x8,      NetCancel,              &gFrontend_MAIN,               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9a,     NetworkStartHost,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x97,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NetGameToggleTyping,    NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf,      temp,                   &gFrontend_OPTIONS,            0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x99,     NetworkStartJoin,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
    }
#else
    0
#endif
};

// GLOBAL: CARMA2_HW 0x0068703c
int gINT_0068703c;

// GLOBAL: CARMA2_HW 0x00763600
char gFrontend_host_join_buffer[256];

// GLOBAL: CARMA2_HW 0x00763714
tNet_game_details* gGame_to_join;

// GLOBAL: CARMA2_HW 0x007638a8
const char* gNet_has_joined_text = 0;

// GLOBAL: CARMA2_HW 0x007638e0
int gFrontend_net_car_index;

// GLOBAL: CARMA2_HW 0x00764e98
int gRace_index;


// STUB: CARMA2_HW 0x00466c00
int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00466da0
int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00467ef0
int C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004682a0
int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004682c0
int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00468460
int C2_HOOK_CDECL NetworkJoinGoAhead(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00468790
int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00468a50
int C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00468e80
int C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004691a0
int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00469320
int C2_HOOK_FASTCALL FRONTEND_NetworkJoinMenuHandler(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0049e210
tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0049e970
int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details* pGame_details, char* pPlayer_name, int pCar_index) {
    NOT_IMPLEMENTED();
}
