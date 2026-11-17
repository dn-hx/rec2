/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_26_OPPOPROC_H
#define GUARD_26_OPPOPROC_H

#include "rec2_types.h"

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// GetStraight

extern int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec);

// TimeToBeBrakingWhenStruggling

extern void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

extern void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* socs, int count_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorner, tOpponent_spec* pOpponent_spec);

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

extern tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle);

// FollowCheatyPath

extern int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);

typedef struct {
    int count;
    int y;
    int current;
    int x[5];
} tFlic_cycle_spec;

extern tFlic_cycle_spec gFlic_cycle_specs[2];

#endif // GUARD_26_OPPOPROC_H
