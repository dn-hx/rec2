/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "26-oppoproc.h"

#include "08-loading1.h"
#include "30-opponent.h"

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// FUNCTION: CARMA2_HW 0x004b13d0
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec) {
    const br_vector3* point;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float width;
    int section;
    int straight;

    section = pSection;
    point = GetOpponentsSectionStartNodePoint(pOpponent_spec, section);
    pStart->v[0] = point->v[2];
    point = GetOpponentsSectionStartNodePoint(pOpponent_spec, section);
    pStart->v[1] = point->v[0];
    point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
    pFinish->v[0] = point->v[2];
    point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
    pFinish->v[1] = point->v[0];
    *pWidth = GetOpponentsSectionWidth(pOpponent_spec, section);
    section = GetOpponentsNextSection(pOpponent_spec, section);
    if (section != -1) {
        for (;;) {
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            fVar2 = point->v[2];
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            fVar1 = point->v[0];
            fVar3 = pFinish->v[0] - pStart->v[0];
            fVar6 = pFinish->v[1] - pStart->v[1];
            fVar4 = (fVar2 - pFinish->v[0]) + fVar3;
            fVar5 = (fVar1 - pFinish->v[1]) + fVar6;
            if ((fVar5 * fVar6 + fVar4 * fVar3 < 0.0) || (0.1 <= fabs((fVar5 * fVar3 + -fVar6 * fVar4) / sqrt(fVar5 * fVar5 + fVar4 * fVar4)))) {
                straight = 0;
            } else {
                straight = 1;
            }
            if (!straight) {
                break;
            }
            pFinish->v[0] = fVar2;
            pFinish->v[1] = fVar1;
            width = GetOpponentsSectionWidth(pOpponent_spec, section);
            if (width < *pWidth) {
                *pWidth = width;
            }
            section = GetOpponentsNextSection(pOpponent_spec, section);
            if (section == -1) {
                return -1;
            }
        }
    }
    return section;
}

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

