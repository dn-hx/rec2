/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_22_REPLAY_H
#define GUARD_22_REPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tU32 gDAT_006a2360;
extern tU32 gDAT_006a2364;
extern int gKey_down;
extern float gPending_replay_rate;
extern tU32 gUINT_006a237c;
extern tU32 gLast_replay_zappy_screen;
extern br_pixelmap* gReplay_pixies[11];
extern int gSingle_frame_mode;
extern tU32 gCam_change_time;
extern tU32 gLast_synch_time;

extern int gPaused;

extern const int gPow2_array[32];

extern tU8 gCamera_type_allowed_replay[9];
extern tU8 gCamera_type_allowed_gameplay[9];

extern int gAR_camera_type;

extern int gCar_car_collisions;

extern const float gCar_simplification_factor[2][5];
extern tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12];

extern void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard);

extern void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);

extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pSpecial_zappy_bastard);

extern void C2_HOOK_FASTCALL DoActionReplayHeadups(void);

extern void C2_HOOK_FASTCALL PreProcess(int pFrame_period);

extern void C2_HOOK_FASTCALL PostProcess(int pFrame_period);

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

extern void C2_HOOK_FASTCALL CheckReplayTurnOn(void);

// InitializeActionReplayPix

extern void C2_HOOK_FASTCALL FreeActionReplayPix(void);

extern void C2_HOOK_FASTCALL SynchronizeActionReplay(void);

extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);

extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);

extern void C2_HOOK_FASTCALL AfterActionReplay(void);

// ToggleReplay

// DisplayMovieStatusPanel

extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);

extern void C2_HOOK_FASTCALL ARSetPause(int pPause);

extern void C2_HOOK_FASTCALL ARApplyCrush(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL PedApplyStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);

extern void C2_HOOK_FASTCALL PedApplyPedPhysics(int field_0x0, br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL PedApplyDismember(int field_0x0, int field_0x4, int field_0x7);

extern void C2_HOOK_FASTCALL PedApplyPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8);

extern void C2_HOOK_FASTCALL PedApplyGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c);

extern void C2_HOOK_FASTCALL PedUndoStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);

extern void C2_HOOK_FASTCALL PedApplyPedModelChangeP(int field_0x0, int field_0x4, int field_0xc, int field_0xa);

#endif // GUARD_22_REPLAY_H
