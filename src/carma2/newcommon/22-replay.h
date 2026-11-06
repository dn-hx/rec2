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

// REPLAY_CALL_MUNGE_PEDESTRIANS

extern int gPaused;

// gTime_of_last_zappy_headups

extern const int gPow2_array[32];

extern tU8 gCamera_type_allowed_replay[9];
extern tU8 gCamera_type_allowed_gameplay[9];

extern int gAR_camera_type;

extern int gCar_car_collisions;

// gReplay_pixie_names

extern const float gCar_simplification_factor[2][5];
extern tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12];

// ActualActionReplayHeadups

extern void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);

// DoZappyActionReplayHeadups

// DoActionReplayHeadups

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// SynchronizeActionReplay

// TurnOnActionReplay

// TurnOffActionReplay

extern void C2_HOOK_FASTCALL AfterActionReplay(void);

// ToggleReplay

// DisplayMovieStatusPanel

extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);

extern void C2_HOOK_FASTCALL ARSetPause(int pPause);

// MakeCharacterPhysicsForBone

extern void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 field_0x4, tU32 field_0x84, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0);

extern br_pixelmap* gReplay_pixelmaps[11];

// gCamera_period

// WorkerPedModelChangeUndo

// WorkerPedDismember

extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);

#endif // GUARD_22_REPLAY_H
