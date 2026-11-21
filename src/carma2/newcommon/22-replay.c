/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include "04-netgame.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "40-main.h"
#include "41-utility.h"
#include "53-controls.h"
#include "59-camera.h"
#include "61-pedestrn.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include "rec2_macros.h"
#include "16-graphics1.h"
#include "15-displays.h"

#include <math.h>
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x006a2360
// GLOBAL: CARMA2_SW 0x0069c3b8
// GLOBAL: CARMA2_CHEAT 0x006a9ed8
tU32 gDAT_006a2360;

// GLOBAL: CARMA2_HW 0x006a2364
// GLOBAL: CARMA2_SW 0x0069c3bc
// GLOBAL: CARMA2_CHEAT 0x006a9edc
tU32 gDAT_006a2364;

// GLOBAL: CARMA2_HW 0x006a2374
// GLOBAL: CARMA2_SW 0x0069c3cc
// GLOBAL: CARMA2_CHEAT 0x006a9eec
int gKey_down;

// GLOBAL: CARMA2_HW 0x006a2378
// GLOBAL: CARMA2_SW 0x0069c3d0
// GLOBAL: CARMA2_CHEAT 0x006a9ef0
float gPending_replay_rate;

// GLOBAL: CARMA2_HW 0x006a237c
// GLOBAL: CARMA2_SW 0x0069c3d4
// GLOBAL: CARMA2_CHEAT 0x006a9ef4
tU32 gUINT_006a237c;

// GLOBAL: CARMA2_HW 0x006a2380
// GLOBAL: CARMA2_SW 0x0069c3d8
// GLOBAL: CARMA2_CHEAT 0x006a9ef8
tU32 gLast_replay_zappy_screen;

// GLOBAL: CARMA2_HW 0x006a2388
// GLOBAL: CARMA2_SW 0x0069c3e0
// GLOBAL: CARMA2_CHEAT 0x006a9f00
br_pixelmap* gReplay_pixies[11] = { NULL };

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x006a23b4
// GLOBAL: CARMA2_SW 0x0069c40c
int gINT_006a23b4 = 0;
#endif

// GLOBAL: CARMA2_HW 0x006a23c4
// GLOBAL: CARMA2_SW 0x0069c41c
// GLOBAL: CARMA2_CHEAT 0x006a9f3c
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a23c8
// GLOBAL: CARMA2_SW 0x0069c420
// GLOBAL: CARMA2_CHEAT 0x006a9f40
tU32 gCam_change_time;

// GLOBAL: CARMA2_HW 0x006a23d4
// GLOBAL: CARMA2_SW 0x0069c42c
// GLOBAL: CARMA2_CHEAT 0x006a9f4c
tU32 gLast_synch_time;

// GLOBAL: CARMA2_HW 0x0074abf0
tU32 gCamera_period;

// GLOBAL: CARMA2_HW 0x006768fc
// GLOBAL: CARMA2_SW 0x0067106c
// GLOBAL: CARMA2_CHEAT 0x0067e1ac
int gPaused;

// GLOBAL: CARMA2_HW 0x0058f2d8
// GLOBAL: CARMA2_SW 0x0058a2d8
// GLOBAL: CARMA2_CHEAT 0x005942d8
const int gPow2_array[32] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

// GLOBAL: CARMA2_HW 0x0058f600
// GLOBAL: CARMA2_SW 0x0058a600
// GLOBAL: CARMA2_CHEAT 0x00594690
tU8 gCamera_type_allowed_replay[9] = {
#ifdef REC2_V101A
    1, 1, 1, 1, 1, 0, 0, 1, 1,
#else
    1, 1, 1, 1, 1, 1, 1, 1, 1,
#endif
};

// GLOBAL: CARMA2_HW 0x0058f610
// GLOBAL: CARMA2_SW 0x0058a610
// GLOBAL: CARMA2_CHEAT 0x005946a0
tU8 gCamera_type_allowed_gameplay[9] = {
#ifdef REC2_101A
    1, 0, 0, 0, 1, 1, 1, 1, 1,
#else
    1, 0, 0, 0, 1, 0, 0, 1, 1,
#endif
};

// GLOBAL: CARMA2_HW 0x0058f620
// GLOBAL: CARMA2_SW 0x0058a620
// GLOBAL: CARMA2_CHEAT 0x005946b0
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x0058f6a4
// GLOBAL: CARMA2_SW 0x0058a6a4
int gCar_car_collisions = 1;

// GLOBAL: CARMA2_HW 0x0058f6b0
// GLOBAL: CARMA2_SW 0x0058a6b0
// GLOBAL: CARMA2_CHEAT 0x00594740
const float gCar_simplification_factor[2][5] = {
    { 50.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.5f, 0.0f }
};

// GLOBAL: CARMA2_HW 0x0058f7f0
// GLOBAL: CARMA2_SW 0x0058a7f0
// GLOBAL: CARMA2_CHEAT 0x00594880
tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

// GLOBAL: CARMA2_HW 0x0065fbb0
const char* gReplay_pixie_names[11] = { 0 };

// GLOBAL: CARMA2_HW 0x0065fbe0
int gReplay_timeline_x1[2];

// GLOBAL: CARMA2_HW 0x0065fbe8
int gReplay_timeline_x2[2];

// GLOBAL: CARMA2_HW 0x0065fbf0
int gReplay_timeline_y[2];

// GLOBAL: CARMA2_HW 0x0065ff08
int gSplit_triangle_vertex_orders[6][3];

// GLOBAL: CARMA2_HW 0x00661678
br_vector3 gVector3_00661678;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e6280
// STUB: CARMA2_SW 0x004e22a0
// STUB: CARMA2_CHEAT 0x004ebf90
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e68e0
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e6260
// FUNCTION: CARMA2_SW 0x004e2280
// FUNCTION: CARMA2_CHEAT 0x004ebf70
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {
    RemoveTransientBitmaps(1);
}

// FUNCTION: CARMA2_HW 0x004e6900
// FUNCTION: CARMA2_SW 0x004e27f0
// FUNCTION: CARMA2_CHEAT 0x004ec5f0
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pSpecial_zappy_bastard) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (abs(pSpecial_zappy_bastard) > 10000 && the_time - gLast_replay_zappy_screen > 50) {
        ActualActionReplayHeadups(pSpecial_zappy_bastard);
        gLast_replay_zappy_screen = the_time;
        PDScreenBufferSwap(0);
        RemoveTransientBitmaps(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6270
// FUNCTION: CARMA2_SW 0x004e2290
// FUNCTION: CARMA2_CHEAT 0x004ebf80
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {
    ActualActionReplayHeadups(0);
}

// STUB: CARMA2_HW 0x004e6950
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e6980
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e6ff0
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e7090
void C2_HOOK_FASTCALL InitializeActionReplay(void) {
    NOT_IMPLEMENTED();
}

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FUNCTION: CARMA2_HW 0x004e70d0
// FUNCTION: CARMA2_SW 0x004e2f10
void C2_HOOK_FASTCALL FreeActionReplayPix(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gReplay_pixies) != 11);

    for (i = 0; i < (int)REC2_ASIZE(gReplay_pixies); i++) {
        if (gReplay_pixies[i] != NULL) {
            BrMapRemove(gReplay_pixies[i]);
            BrPixelmapFree(gReplay_pixies[i]);
            gReplay_pixies[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e7110
// FUNCTION: CARMA2_SW 0x004e2f50
// FUNCTION: CARMA2_CHEAT 0x004ecdb0
void C2_HOOK_FASTCALL SynchronizeActionReplay(void) {

    while (ARGetReplayRate() != 0.f && PDGetTotalTime() - gLast_synch_time < gFrame_period / fabs(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e71c0
// FUNCTION: CARMA2_SW 0x004e3000
// FUNCTION: CARMA2_CHEAT 0x004ece60
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {

    SavePedStuff();
    if (gMap_view == 2) {
        ToggleMap();
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
    gUINT_006a237c = PDGetTotalTime();
    gDAT_006a2360 = ARGetStartTime();
    LoadInterfaceStuff(1);
    StartMouseCursor();
    gKey_down = 52;
    gPending_replay_rate = 0.0f;
    gCam_change_time = PDGetTotalTime();
    gDAT_006a2364 = GetTotalTime();
    if (!gRace_finished) {
        SaveCameraPosition(0);
    }
    gCamera_type = gAction_replay_camera_mode;
    SetCameraType(gAR_camera_type);
    InitReplaySplashes();
}

// FUNCTION: CARMA2_HW 0x004e7270
// FUNCTION: CARMA2_SW 0x004e30b0
// FUNCTION: CARMA2_CHEAT 0x004ecf10
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {
    UnlockInterfaceStuff();
    AssertThisTimeAsCurrentTime();
    if (gRace_finished == 0) {
        RestoreCameraPosition(0);
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
    gAR_camera_type = gAction_replay_camera_mode;
    SetCameraType(gCamera_type);
    ComparePedStuff();
    EndMouseCursor();
}

// FUNCTION: CARMA2_HW 0x004e72d0
// FUNCTION: CARMA2_SW 0x004e3110
// FUNCTION: CARMA2_CHEAT 0x004ecf70
void C2_HOOK_FASTCALL AfterActionReplay(void) {
    ForceRebuildActiveCarList();
}

// STUB: CARMA2_HW 0x004e72e0
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e7350
void C2_HOOK_FASTCALL DisplayMovieStatusPanel(int pReplay_only) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ec370
void C2_HOOK_FASTCALL ShatterObject(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tMovement_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform) {
    NOT_IMPLEMENTED();
}

// ToggleReplay

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
// FUNCTION: CARMA2_SW 0x004022e0
// FUNCTION: CARMA2_CHEAT 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
// FUNCTION: CARMA2_SW 0x004022f0
// FUNCTION: CARMA2_CHEAT 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gPaused = pPause;
}

// STUB: CARMA2_HW 0x0043bd70
void C2_HOOK_FASTCALL ARApplyCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043be50
void C2_HOOK_FASTCALL ARUndoCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043c240
void C2_HOOK_FASTCALL ARApplySplitWeld(void* roadMachine, br_vector3* pVector) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043c3d0
void C2_HOOK_FASTCALL ARUndoSplitWeld(void* roadMachine) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca500
void C2_HOOK_FASTCALL PedApplyPedMove(tPedestrian* pPed, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca5a0
void C2_HOOK_FASTCALL PedUndoPedMove(tPedestrian* pPed, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca680
void C2_HOOK_FASTCALL PedApplyStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca6e0
void C2_HOOK_FASTCALL PedUndoStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca7d0
void C2_HOOK_FASTCALL PedApplyPedPhysics(int field_0x0, br_matrix34* pMatrix) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca8a0
void C2_HOOK_FASTCALL PedApplyPedModelChangeP(int field_0x0, int field_0x4, int field_0xc, int field_0xa) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca8d0
void C2_HOOK_FASTCALL PedApplyPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ca900
void C2_HOOK_FASTCALL PedApplyDismember(int field_0x0, int field_0x4, int field_0x7) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ccae0
void C2_HOOK_FASTCALL PedApplyGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ccc40
void C2_HOOK_FASTCALL PedUndoGibShower(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c) {
    NOT_IMPLEMENTED();
}
