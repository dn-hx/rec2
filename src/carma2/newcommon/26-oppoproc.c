/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "26-oppoproc.h"

#include "08-loading1.h"

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// GetStraight

// FUNCTION: CARMA2_HW 0x004af8d0
// FUNCTION: CARMA2_SW 0x004aba40
int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec) {

    if (gTime_stamp_for_this_munging >
        (float)(750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1))
            - pOpponent_spec->car_spec->collision_info->M * -30.f
            + (float)(pOpponent_spec->follow_path_data.struggle_time + 2750)) {
        return 1;
    } else {
        return 0;
    }
}

// TimeToBeBrakingWhenStruggling

// FUNCTION: CARMA2_HW 0x004b13c0
// FUNCTION: CARMA2_SW 0x004ad530
void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {

}

// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// SetMaxSpeedFromSOCs

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

// STUB: CARMA2_HW 0x004af960
// STUB: CARMA2_SW 0x004abad0
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
    NOT_IMPLEMENTED();
}

// FollowCheatyPath


// FUNCTION: CARMA2_HW 0x004b2430
int C2_HOOK_FASTCALL FUN_004b2430(int* pIndex) {
    int prev;
    int cur;
    int n;
    int m;
    int idx;

    DRS3StartSound(gEffects_outlet, 3000);
    idx = *pIndex;
    n = idx - 2;
    prev = gFlic_cycle_specs[n].current - 1;
    if (prev < 0) {
        prev = gFlic_cycle_specs[n].count - 1;
    }
    m = n;
    cur = ((int*)&gFlic_cycle_specs[0])[m * 8 + 2];
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(287, gFlic_cycle_specs[m].x[cur], gFlic_cycle_specs[m].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(288, gFlic_cycle_specs[m].x[prev], gFlic_cycle_specs[m].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    gFlic_cycle_specs[m].current = prev;
    return 1;
}