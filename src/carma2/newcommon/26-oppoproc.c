/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "26-oppoproc.h"

#include "08-loading1.h"
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pSection);

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

// FUNCTION: CARMA2_HW 0x004b1ab0
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar) {
    int count;
    int i;
    int section;
    tCorner* corner;

    count = 0;
    section = pNext_section;
    for (i = 0, corner = pCorners; i < pCount_corners; i++, corner++) {
        while (section != -1) {
            const br_vector3* start;
            const br_vector3* finish;
            float dx;
            float dz;

            if (count >= 10) {
                goto next_corner;
            }
            if (section == (int)corner->section) {
                break;
            }
            start = GetOpponentsSectionStartNodePoint(pOpponent_spec, corner->section);
            finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            dx = finish->v[0] - start->v[0];
            dz = finish->v[2] - start->v[2];
            if (sqrt(dx * dx + dz * dz) <= corner->field_0x0c) {
                break;
            }
            pSocs[count].field_0x0 |= 1;
            pSocs[count].field_0x4 = section;
            count += 1;
            section = GetOpponentsNextSection(pOpponent_spec, section);
        }
        if (count >= 10) {
            goto next_corner;
        }
        pSocs[count].field_0x0 &= ~1;
        pSocs[count].field_0x4 = i;
        corner->field_0x18 = (int)sqrt(corner->field_0x08 * 6.9001222f * 7.5f * 1.96f);
        while (section != (int)corner->section) {
            if ((int)corner->field_0x18 > GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1)) {
                corner->field_0x18 = GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1);
            }
            section = GetOpponentsNextSection(pOpponent_spec, section);
        }
        while (section != -1) {
            const br_vector3* start;
            const br_vector3* finish;
            float dx;
            float dz;

            start = GetOpponentsSectionStartNodePoint(pOpponent_spec, corner->section);
            finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            dx = finish->v[0] - start->v[0];
            dz = finish->v[2] - start->v[2];
            if (sqrt(dx * dx + dz * dz) >= corner->field_0x0c) {
                break;
            }
            if ((int)corner->field_0x18 > GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1)) {
                corner->field_0x18 = GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1);
            }
            section = GetOpponentsNextSection(pOpponent_spec, section);
        }
        count += 1;
next_corner:;
    }
    return count;
}

// StuffDataFromCorner

// SetCurvature

// STUB: CARMA2_HW 0x004af960
// STUB: CARMA2_SW 0x004abad0
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
    NOT_IMPLEMENTED();
}

// FollowCheatyPath

