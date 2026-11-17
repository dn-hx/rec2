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

// FUNCTION: CARMA2_HW 0x004b18b0
void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* socs, int count_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorner, tOpponent_spec* pOpponent_spec) {
    const br_vector3* point;
    float dist = 0.f;
    float last;
    float threshold;
    double max_speed;
    float prev_z;
    float prev_x;
    float v2;
    float v0;
    float dz;
    float dx;
    int i;
    int bVar8 = 0;

    threshold = (pSpeed * pSpeed * 0.0625f - (float)gFrame_period_for_this_munging * pSpeed * -0.001f) * 0.14492753f;
    prev_z = pPos2d->v[0];
    prev_x = pPos2d->v[1];
    for (i = 0; i < count_socs; i++) {
        if (dist >= threshold) {
            return;
        }
        if (bVar8) {
            dist = dist - last;
        }
        if ((*(int*)&socs[i] & 1) == 0) {
            bVar8 = 1;
            point = GetOpponentsSectionStartNodePoint(pOpponent_spec, pCorner[socs[i].field_0x4].section);
            max_speed = (double)(int)pCorner[socs[i].field_0x4].field_0x18;
            last = pCorner[socs[i].field_0x4].field_0x0c;
            dist = dist - last;
        } else {
            bVar8 = 0;
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)socs[i].field_0x4);
            max_speed = (double)GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)socs[i].field_0x4, 1);
        }
        v2 = point->v[2];
        v0 = point->v[0];
        dz = prev_z - v2;
        dx = prev_x - v0;
        dist = sqrt(dx * dx + dz * dz) + dist;
        if (dist < 0.f) {
            dist = 0.f;
        }
        if (dist * 6.9f < (pSpeed * pSpeed - max_speed * max_speed) * 0.0625f - (float)gFrame_period_for_this_munging * pSpeed * -0.001f) {
            if (max_speed < *pDesired_speed) {
                *pDesired_speed = max_speed;
            }
        }
        prev_z = v2;
        prev_x = v0;
    }
}

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

// STUB: CARMA2_HW 0x004af960
// STUB: CARMA2_SW 0x004abad0
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
    NOT_IMPLEMENTED();
}

// FollowCheatyPath

