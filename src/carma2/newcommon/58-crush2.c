/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "58-crush2.h"

#include "00-car.h"
#include "07-structur.h"
#include "09-funks.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "21-mainloop.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "39-oil.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "51-smash.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"
#include "13-crush1.h"
#include "platform.h"
#include "platform.h"
#include "13-crush1.h"

// GLOBAL: CARMA2_HW 0x0074d3b0
// GLOBAL: CARMA2_SW 0x00743910
int gCredits_wasting_car[3];

// GLOBAL: CARMA2_HW 0x0075b930
// GLOBAL: CARMA2_SW 0x00751e90
float gCredits_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x0075b9b0
// GLOBAL: CARMA2_SW 0x00751f10
int gTime_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761a50
// GLOBAL: CARMA2_SW 0x00757fb0
int gCredits_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761ed0
// GLOBAL: CARMA2_SW 0x_00758430
float gSeconds_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x00763520
// GLOBAL: CARMA2_SW 0x00759a80
int gTime_wasting_car[3];

// GLOBAL: CARMA2_HW 0x00763530
// GLOBAL: CARMA2_SW 0x00759a90
int gTime_per_ped_kill[3];

// GLOBAL: CARMA2_HW 0x00590020
// GLOBAL: CARMA2_SW 0x0058b018
float gWobble_spam_y[8] = { 0.0f, -0.15f, 0.4f, 0.15f, -0.4f, 0.25f, 0.0f, -0.25f };

// GLOBAL: CARMA2_HW 0x00590040
// GLOBAL: CARMA2_SW 0x0058b038
float gWobble_spam_z[8] = { 0.4f, -0.25f, 0.0f, 0.25f, 0.0f, 0.15f, -0.4f, -0.15f };

// GLOBAL: CARMA2_HW 0x00590060
// GLOBAL: CARMA2_SW 0x0058b058
float gWheel_circ_to_width = 0.16f;

// GLOBAL: CARMA2_HW 0x00590068
// GLOBAL: CARMA2_SW 0x0058b060
const tU8 gSmoke_steps[12] = { 20, 20, 0, 10, 10, 10, 10, 10, 10, 10, 10, 10 };

void C2_HOOK_FASTCALL CheckLastCar(void) {
    if (gNet_mode == eNet_mode_none && GetCarCount(eVehicle_opponent) != 0 && NumberOfOpponentsLeft() == 0) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
            if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                NewTextHeadupSlot(4, 0, 5000, -4, GetMiscString(eMiscString_every_opponent_wasted));
            }
            RaceCompleted(eRace_over_2);
        }
    }
}

// STUB: CARMA2_HW 0x00441350
int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
// FUNCTION: CARMA2_HW 0x0043f5f0
// FUNCTION: CARMA2_SW 0x0043f6d0
inline void C2_HOOK_FASTCALL KnackerThisCar(tCar_spec* pCar) {

    if (pCar->invulnerable_no_wastage) {
        return;
    }
    if (gNet_mode == eNet_mode_client) {
        return;
    }
    if (gNet_mode == eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        RaceCompleted(eRace_over_6);
    }
    pCar->knackered = 1;
    QueueWastedMassage(pCar->index);
    CheckLastCar();
    QueueOilSpill(pCar);
    if (gNet_mode == eNet_mode_none) {
        KillGroovadelic(pCar->index);
        KillFunkotronic(pCar->index);
    }
}

// FUNCTION: CARMA2_HW 0x0043f6c0
// FUNCTION: CARMA2_SW 0x0043f7a0
void C2_HOOK_FASTCALL ExplodeMeCarUpMeOldMatey(tCar_spec* pCar) {

    Explode(&gExplosion_pix_animation_groups, pCar->car_master_actor,
        &pCar->car_actor->model->bounds, NULL);
    if ((pCar != NULL && pCar->driver ==  eDriver_local_human)) {
        DRS3StartSound(gCar_outlet, gExplosion_sound_id);
    } else {
        DRS3StartSound3D(gCar_outlet, gExplosion_sound_id, &pCar->pos, &gZero_v__car, 1, 255, BR_FIXED_INT(1), BR_FIXED_INT(1));
    }
}

// FUNCTION: CARMA2_HW 0x0043f730
// FUNCTION: CARMA2_SW 0x0043f810
int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar) {

    if (gNet_mode == eNet_mode_client
            || pCar->knackered
            || ((pCar->damage_units[eDamage_engine].damage_level < 99
                && (pCar->damage_units[eDamage_transmission].damage_level < 99)
                && pCar->damage_units[eDamage_driver].damage_level < 99)
                && (pCar->damage_units[eDamage_lf_wheel].damage_level < 99
                    || pCar->damage_units[eDamage_rf_wheel].damage_level < 99
                    || pCar->damage_units[eDamage_lr_wheel].damage_level < 99
                    || pCar->damage_units[eDamage_rr_wheel].damage_level < 99))) {
        return 0;
    }
    ExplodeMeCarUpMeOldMatey(pCar);
    KnackerThisCar(pCar);
    if (gNet_mode == eNet_mode_none) {
        if (IRandomBetween(0,1) && gNet_mode == eNet_mode_none) {
            StopCarSmoking(pCar);
            CreateSmokeColumn(pCar, 0, IRandomBetween(0,  11), 20000);
        }
        CreateSmokeColumn(pCar, 0, IRandomBetween(0, 11), 180000);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0043f930
// FUNCTION: CARMA2_SW 0x0043fa10
void C2_HOOK_FASTCALL DamageUnit2(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude) {
    tDamage_unit* the_damage;
    int last_level;

    if (pCar != NULL && pCar->driver >= eDriver_net_human && pDamage_type == eDamage_driver) {
        return;
    }

    if (!pCar->invulnerable_no_damage && pMagnitude >= 5) {
        the_damage = &pCar->damage_units[pDamage_type];
        last_level = the_damage->damage_level;
        the_damage->damage_level += pMagnitude;
        if (the_damage->damage_level >= 99) {
            if (pMagnitude < 10) {
                the_damage->damage_level = last_level;
            } else {
                the_damage->damage_level = 99;
            }
        }
        if (!SetKnackeredFlag(pCar)
                && (pCar == NULL || pCar->driver != eDriver_oppo)
                && gNet_mode == eNet_mode_none
                && ((pCar->damage_units[eDamage_engine].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99)
                    || (pCar->damage_units[eDamage_transmission].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99))) {
            QueueOilSpill(pCar);
        }
    }
}

// RecordLastDamage

inline void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, float pMagnitude, float pNastiness) {

    if (pCar != NULL && pCar->driver >= eDriver_net_human) {
        if (gNet_mode != eNet_mode_none) {
            DamageUnit2(pCar, pDamage_type, (int)(pNastiness * pMagnitude * 15.0f));
        } else if (PercentageChance((int)(pNastiness * pMagnitude * 1500.0f))) {
            DamageUnit2(pCar, pDamage_type, (int)(pNastiness * pMagnitude * 30.0f));
        }
    } else {
        DamageUnit2(pCar, pDamage_type, (int)(pNastiness * pMagnitude * 10.0f * (((1.0 - (double)gCurrent_race.index / (double)gNumber_of_races) * 99.0) / 20.0 + 1.0)));
    }
}

// FUNCTION: CARMA2_HW 0x0043f9f0
// FUNCTION: CARMA2_SW 0x0043fad0
void C2_HOOK_FASTCALL CalcModifiedLocation(tCar_spec* pCar) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_impact_location, 0x6f4);

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].last_level = pCar->damage_units[i].damage_level;
    }
    pCar->damage_magnitude_accumulator = 0.0f;
    pCar->last_impact_location = eImpact_unknown;
    pCar->pre_car_col_mat = pCar->car_master_actor->t.t.mat;
    pCar->pre_car_col_speed = pCar->speed;
    pCar->pre_car_col_knackered = pCar->knackered;
    pCar->pre_car_col_direction = pCar->direction;
    pCar->pre_car_col_velocity = pCar->collision_info->v;
    pCar->pre_car_col_velocity_car_space = pCar->collision_info->velocity_car_space;
}

// FUNCTION: CARMA2_HW 0x0043faa0
// FUNCTION: CARMA2_SW 0x0043fb80
void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity) {
    br_scalar magnitude_squared;
    br_scalar abs_x;
    br_scalar abs_z;
    int offset;

    magnitude_squared = BrVector3LengthSquared(pVelocity);
    offset = 0;
    if (magnitude_squared > 0.2f) {
        offset = 8;
    } else if (magnitude_squared > 0.015f) {
        offset = 4;
    } else if (magnitude_squared < 0.001f) {
        return;
    }

    abs_x = (br_scalar)fabs(pVelocity->v[0]);
    abs_z = (br_scalar)fabs(pVelocity->v[2]);
    if (abs_x > abs_z) {
        if (pVelocity->v[0] < 0.f) {
            PratcamEvent(offset + 16);
        } else {
            PratcamEvent(offset + 15);
        }
    } else {
        if (pVelocity->v[2] < 0.f) {
            PratcamEvent(offset + 13);
        } else {
            PratcamEvent(offset + 14);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043fb70
// FUNCTION: CARMA2_SW 0x0043fc50
void C2_HOOK_FASTCALL DamageSystems(tCar_spec* pCar, br_vector3* pImpact_point, br_vector3* pEnergy_vector, int pWas_hitting_a_car) {
    int i;
    int j;
    int result;
    br_bounds3 crushed_car_bounds;
    float proportion_x;
    float proportion_y;
    float proportion_z;
    float energy_magnitude;
    float pure_energy_magnitude;
    br_scalar x;
    br_scalar y;
    br_scalar z;
    br_scalar x1;
    br_scalar x2;
    br_scalar y1;
    br_scalar y2;
    br_scalar z1;
    br_scalar z2;
    tImpact_location impact_location;
    tDamage_program* the_program;
    tDamage_clause* the_clause;
    tDamage_condition* the_condition;
    tDamage_effect* the_effect;

    pure_energy_magnitude = BrVector3Length(pEnergy_vector);
    if (pure_energy_magnitude == 0.0f && !pWas_hitting_a_car) {
        return;
    }

    energy_magnitude = (float)(pCar->softness_factor * pure_energy_magnitude / 0.7);
    crushed_car_bounds = pCar->collision_info->bb1;

    x1 = pImpact_point->v[0] - crushed_car_bounds.min.v[0];
    x2 = crushed_car_bounds.max.v[0] - pImpact_point->v[0];
    if (x1 < x2) {
        x = x1;
    } else {
        x = x2;
    }
    y1 = pImpact_point->v[1] - crushed_car_bounds.min.v[1];
    y2 = crushed_car_bounds.max.v[1] - pImpact_point->v[1];
    if (y1 < y2) {
        y = y1;
    } else {
        y = y2;
    }
    z1 = pImpact_point->v[2] - crushed_car_bounds.min.v[2];
    z2 = crushed_car_bounds.max.v[2] - pImpact_point->v[2];
    if (z1 < z2) {
        z = z1;
    } else {
        z = z2;
    }
    if (z <= x && z <= y) {
        impact_location = z1 < z2 ? eImpact_front : eImpact_back;
        proportion_x = x1 / (crushed_car_bounds.max.v[0] - crushed_car_bounds.min.v[0]);
        proportion_y = y1 / (crushed_car_bounds.max.v[1] - crushed_car_bounds.min.v[1]);
    } else if (x <= y && x <= z) {
        impact_location = x1 < x2 ? eImpact_left : eImpact_right;
        proportion_y = y1 / (crushed_car_bounds.max.v[1] - crushed_car_bounds.min.v[1]);
        proportion_z = z1 / (crushed_car_bounds.max.v[2] - crushed_car_bounds.min.v[2]);
    } else {
        impact_location = y1 < y2 ? eImpact_bottom : eImpact_top;
        proportion_x = x1 / (crushed_car_bounds.max.v[0] - crushed_car_bounds.min.v[0]);
        proportion_z = z1 / (crushed_car_bounds.max.v[2] - crushed_car_bounds.min.v[2]);
    }
    if (pWas_hitting_a_car && pCar->last_impact_location == eImpact_unknown) {
        pCar->last_impact_location = impact_location;
        pCar->last_col_prop_x = proportion_x;
        pCar->last_col_prop_y = proportion_y;
        pCar->last_col_prop_z = proportion_z;
    }

    if (energy_magnitude == 0.0f || pCar->invulnerable_no_damage) {
        return;
    }
    if (!pWas_hitting_a_car && impact_location == eImpact_bottom) {
        energy_magnitude = energy_magnitude / 2.0f;
    }

    the_program = &pCar->damage_programs[impact_location];
    for (i = 0, the_clause = the_program->clauses; i < the_program->clause_count; i++, the_clause++) {
        result = 1;
        for (j = 0, the_condition = the_clause->conditions; j < the_clause->condition_count && result; j++, the_condition++) {
            switch (the_condition->axis_comp) {
            case eAxis_x:
                if (the_condition->condition_operator == eCondition_less_than) {
                    result &= proportion_x < the_condition->comparitor;
                } else {
                    result &= proportion_x > the_condition->comparitor;
                }
                break;
            case eAxis_y:
                if (the_condition->condition_operator == eCondition_less_than) {
                    result &= proportion_y < the_condition->comparitor;
                } else {
                    result &= proportion_y > the_condition->comparitor;
                }
                break;
            case eAxis_z:
                if (the_condition->condition_operator == eCondition_less_than) {
                    result &= proportion_z < the_condition->comparitor;
                } else {
                    result &= proportion_z > the_condition->comparitor;
                }
                break;
            }
        }
        if (result) {
            for (j = 0, the_effect = the_clause->effects; j < the_clause->effect_count; j++, the_effect++) {
                DoDamage(pCar, the_effect->type, energy_magnitude, the_effect->weakness_factor);
            }
        }
    }
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        switch (impact_location) {
        case eImpact_left:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 50.0f, energy_magnitude * 100.0f),
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(4.0f / energy_magnitude, 7.0f / energy_magnitude));
            break;
        case eImpact_right:
            NewScreenWobble(
                -FRandomBetween(energy_magnitude * 50.0f, energy_magnitude * 100.0f),
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(4.0f / energy_magnitude, 7.0f / energy_magnitude));
            break;
        case eImpact_bottom:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(energy_magnitude * 30.0f, energy_magnitude * 60.0f),
                FRandomBetween(1.0f / energy_magnitude, 5.0f / energy_magnitude));
            break;
        case eImpact_top:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(energy_magnitude * 30.0f, energy_magnitude * 60.0f),
                FRandomBetween(1.0f / energy_magnitude, 5.0f / energy_magnitude));
            break;
        case eImpact_front:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(energy_magnitude * 50.0f, energy_magnitude * 150.0f),
                FRandomBetween(7.0f / energy_magnitude, 25.0f / energy_magnitude));
            break;
        case eImpact_back:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(-energy_magnitude * 50.0f, energy_magnitude * 150.0f),
                FRandomBetween(7.0f / energy_magnitude, 25.0f / energy_magnitude));
            break;
        default:
            break;
        }
    }
}

// GetDirection

// FUNCTION: CARMA2_HW 0x00440210
void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].smoke_last_level = pCar->damage_units[i].damage_level;
    }
}

// FUNCTION: CARMA2_HW 0x00440230
// FUNCTION: CARMA2_SW 0x00440310
void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar) {
    int i;
    int colour;
    int old_colour;
    int step;
    int smoking;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmoke_steps) != 12);

    smoking = 0;
    if (pCar != NULL && pCar->driver > eDriver_5) {
        for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
            step = gSmoke_steps[i];
            if (step != 0) {
                colour = (99 - pCar->damage_units[i].damage_level) / step;
                if (colour <= 2) {
                    smoking = 1;
                }
                if (pCar->damage_units[i].damage_level == pCar->damage_units[i].smoke_last_level) {
                    continue;
                }
                step = gSmoke_steps[i];
                if (step != 0 && pCar->damage_units[i].damage_level > pCar->damage_units[i].smoke_last_level) {
                    old_colour = (99 - pCar->damage_units[i].smoke_last_level) / step;
                    if (step > 0 && colour != old_colour && colour <= 2) {
                        ConditionalSmokeColumn(pCar, i, colour);
                    }
                }
            }
        }
        if (!smoking && IsCarSmoking(pCar)) {
            StopCarSmoking(pCar);
        }
        SetSmokeLastDamageLevel(pCar);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

#define DCE_F(p, o) (*(float*)((char*)(p) + (o)))

#define DCE_I(p, o) (*(int*)((char*)(p) + (o)))

#define DCE_P(p, o) (*(tCar_spec**)((char*)(p) + (o)))

#define DCE_U(p, o) (*(tU32*)((char*)(p) + (o)))

// FUNCTION: CARMA2_HW 0x00440660
// STUB: CARMA2_SW 0x00440740
int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {
    tCar_spec* local_5c;
    tCar_spec* local_58;
    tCar_spec* local_54;
    int local_4c;
    int local_34;
    float local_3c;
    int cls3c;
    int iVar11;
    tU32 uVar12;
    int iVar13;
    int iVar15;
    int iVar20;
    int bVar5;
    int bVar6;
    int bVar7;
    int bVar8;
    int bVar9;
    int bVar10;
    int uVar18;
    int* piVar17;
    int* piVar19;
    int iVar_k;
    int loop_n;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar14;
    br_vector3 local_18;
    br_vector3 local_c;
    tCar_spec* tmp_car;

    local_5c = NULL;
    local_58 = NULL;
    bVar10 = 0;
    local_4c = 1;
    bVar8 = 0;
    iVar11 = (int)PDGetTotalTime();
    bVar9 = 0;
    uVar12 = (tU32)iVar11;
    bVar5 = 0;
    if (pCar2 != NULL) {
        if (DCE_I(pCar2, 0xc) < 6) {
            fVar14 = 0.0f;
        } else {
            fVar14 = DCE_F(pCar2, 0x133c);
        }
        if (fVar14 != 0.0f) {
            bVar5 = 1;
        }
    }
    local_54 = NULL;
    if (pCar1 != NULL) {
        local_54 = pCar2;
    }
    if (pCar1 == NULL || DCE_I(pCar1, 0xc) < 6) {
        if (pCar2 == NULL) {
            return 0;
        }
        if (DCE_I(pCar2, 0xc) < 6) {
            return 0;
        }
        if (pCar1 != NULL) {
            iVar13 = DCE_I(pCar1, 0xc);
            if (iVar13 != 0 && (iVar13 == 4 || iVar13 == 2 || iVar13 == 3)) {
                tmp_car = DCE_P(pCar1, 0xc8);
                if (tmp_car != NULL && DCE_I(tmp_car, 0xc) > 5 && tmp_car != pCar2) {
                    DCE_I(pCar2, 0x13e4) = iVar11;
                    DCE_P(pCar2, 0x13ec) = DCE_P(pCar1, 0xc8);
                }
            }
        }
        local_54 = NULL;
        pCar1 = pCar2;
    }
    if (local_54 != NULL && DCE_I(local_54, 0xc) < 6) {
        iVar13 = DCE_I(local_54, 0xc);
        if (iVar13 != 0 && (iVar13 == 4 || iVar13 == 2 || iVar13 == 3)) {
            tmp_car = DCE_P(local_54, 0xc8);
            if (tmp_car != NULL && pCar1 != NULL && DCE_I(pCar1, 0xc) > 5 && tmp_car != pCar1) {
                DCE_I(pCar1, 0x13e4) = iVar11;
                DCE_P(pCar1, 0x13ec) = DCE_P(local_54, 0xc8);
            }
        }
        local_54 = NULL;
    }
    if (DCE_I(pCar1, 0x1d8) != 0
        || (local_54 != NULL
            && (DCE_I(local_54, 0x1d8) != 0
                || (DCE_F(local_54, 0x133c) <= 5e-05f && DCE_F(pCar1, 0x133c) <= 5e-05f)))) {
        goto LAB_00441239;
    }
    iVar13 = DCE_I(pCar1, 0x6f4);
    if (iVar13 == 2 || iVar13 == 3 || iVar13 == 0 || iVar13 == 1) {
        if (0.25f <= DCE_F(pCar1, 0x13e0)) {
            iVar20 = 5;
            if (DCE_F(pCar1, 0x13e0) <= 0.75f) {
                iVar20 = iVar13;
            }
        } else {
            iVar20 = 4;
        }
    } else {
        iVar20 = iVar13;
    }
    fVar14 = (float)fabs(DCE_F(pCar1, 0x13c0));
    local_3c = (float)fabs(DCE_F(pCar1, 0x13c4));
    if (fVar14 <= local_3c || fVar14 <= (float)fabs(DCE_F(pCar1, 0x13c8))) {
        if (local_3c <= fVar14 || local_3c <= (float)fabs(DCE_F(pCar1, 0x13c8))) {
            iVar13 = 4;
            if (0.0f <= DCE_F(pCar1, 0x13c8)) {
                iVar13 = 5;
            }
        } else if (0.0f <= DCE_F(pCar1, 0x13c4)) {
            iVar13 = 0;
        } else {
            iVar13 = 1;
        }
    } else if (0.0f <= DCE_F(pCar1, 0x13c0)) {
        iVar13 = 3;
    } else {
        iVar13 = 2;
    }
    uVar18 = (int)uVar12;
    if (local_54 != NULL) {
        cls3c = DCE_I(local_54, 0x6f4);
        if (cls3c == 2 || cls3c == 3 || cls3c == 0 || cls3c == 1) {
            if (0.25f <= DCE_F(local_54, 0x13e0)) {
                if (DCE_F(local_54, 0x13e0) > 0.75f) {
                    cls3c = 5;
                }
            } else {
                cls3c = 4;
            }
        }
        fVar1 = (float)fabs(DCE_F(local_54, 0x13c0));
        fVar14 = (float)fabs(DCE_F(local_54, 0x13c4));
        if (fVar1 <= fVar14 || fVar1 <= (float)fabs(DCE_F(local_54, 0x13c8))) {
            if (fVar14 <= fVar1 || fVar14 <= (float)fabs(DCE_F(local_54, 0x13c8))) {
                iVar_k = 4;
                if (0.0f <= DCE_F(local_54, 0x13c8)) {
                    iVar_k = 5;
                }
                uVar18 = (cls3c == iVar_k);
            } else if (0.0f <= DCE_F(local_54, 0x13c4)) {
                uVar18 = (cls3c == 0);
            } else {
                uVar18 = (cls3c == 1);
            }
        } else if (0.0f <= DCE_F(local_54, 0x13c0)) {
            uVar18 = (cls3c == 3);
        } else {
            uVar18 = (cls3c == 2);
        }
        local_3c = (float)cls3c;
    }
    if (pCar1 == NULL || DCE_I(pCar1, 0xc) < 7) {
        if (local_54 != NULL) {
            if (DCE_I(local_54, 0xc) > 6 && uVar18 != 0 && DCE_F(local_54, 0x13c8) != 0.0f
                && (((0 < DCE_I(local_54, 0x135c)) != (0.0f < DCE_F(local_54, 0x13c8)))
                && ((DCE_U(local_54, 0x12d0) & 0x40000) != 0 || DCE_I(local_54, 0x12dc) > 0x8000))) {
                DCE_P(pCar1, 0x13ec) = local_54;
                DCE_I(pCar1, 0x13e4) = iVar11;
            }
            goto LAB_00440b7b;
        }
LAB_00440b7f:
        if (iVar11 - DCE_I(pCar1, 0x13e4) > 4999) {
            return 1;
        }
        local_5c = DCE_P(pCar1, 0x13ec);
        local_4c = 1;
        bVar9 = 1;
        local_58 = pCar1;
    } else {
        if (local_54 == NULL) {
            goto LAB_00440b7f;
        }
        if (iVar20 == iVar13 && DCE_F(pCar1, 0x13c8) != 0.0f
            && (((0 < DCE_I(pCar1, 0x135c)) != (0.0f < DCE_F(pCar1, 0x13c8)))
            && ((DCE_U(pCar1, 0x12d0) & 0x40000) != 0 || DCE_I(pCar1, 0x12dc) > 0x8000))) {
            DCE_P(local_54, 0x13ec) = pCar1;
            DCE_I(local_54, 0x13e4) = iVar11;
        }
LAB_00440b7b:
        if (local_54 == NULL) {
            goto LAB_00440b7f;
        }
        if (iVar20 != iVar13
            || DCE_F(pCar1, 0x13a4) * 5.0f <= DCE_F(local_54, 0x13a4)
            || DCE_F(pCar1, 0x13a4) <= 0.0005f
            || (pCar1 != NULL && DCE_I(pCar1, 0xc) > 6
                && (DCE_F(pCar1, 0x13c8) == 0.0f
                    || ((0 < DCE_I(pCar1, 0x135c)) == (0.0f < DCE_F(pCar1, 0x13c8)))
                    || ((DCE_U(pCar1, 0x12d0) & 0x40000) == 0 && DCE_I(pCar1, 0x12dc) < 0x8001)))) {
            bVar6 = 0;
        } else {
            bVar6 = 1;
        }
        if (uVar18 == 0
            || DCE_F(local_54, 0x13a4) * 5.0f <= DCE_F(pCar1, 0x13a4)
            || DCE_F(local_54, 0x13a4) <= 0.0005f
            || (DCE_I(local_54, 0xc) > 6
                && (DCE_F(local_54, 0x13c8) == 0.0f
                    || ((0 < DCE_I(local_54, 0x135c)) == (0.0f < DCE_F(local_54, 0x13c8)))
                    || ((DCE_U(local_54, 0x12d0) & 0x40000) == 0 && DCE_I(local_54, 0x12dc) < 0x8001)))) {
            bVar7 = 0;
        } else {
            bVar7 = 1;
        }
        if (gNet_mode == 0 || !bVar6) {
            if (bVar7 && DCE_I(local_54, 0xc) == 8) {
                bVar6 = 0;
            }
LAB_00441274:
            if (bVar6) {
                fVar14 = DCE_F(local_54, 0x13a8);
                fVar1 = DCE_F(pCar1, 0x13a8);
                fVar2 = DCE_F(local_54, 0x13ac);
                fVar3 = DCE_F(pCar1, 0x13ac);
                fVar4 = DCE_F(local_54, 0x13b0);
                local_58 = local_54;
                local_5c = pCar1;
            } else {
                if (!bVar7) {
                    goto LAB_00440d1a;
                }
                fVar14 = DCE_F(local_54, 0x13a8);
                fVar1 = DCE_F(pCar1, 0x13a8);
                fVar2 = DCE_F(local_54, 0x13ac);
                fVar3 = DCE_F(pCar1, 0x13ac);
                fVar4 = DCE_F(local_54, 0x13b0);
                local_5c = local_54;
                local_58 = pCar1;
            }
            if (iVar20 != 4 || cls3c != 4 || DCE_F(pCar1, 0x13a4) <= 0.001f
                || DCE_F(local_54, 0x13a4) <= 0.001f
                || -0.7f <= fVar4 * DCE_F(pCar1, 0x13b0) + fVar3 * fVar2 + fVar1 * fVar14) {
                local_4c = 1;
            } else {
                bVar10 = 1;
                local_4c = 2;
            }
        } else {
            if (!bVar7) {
                goto LAB_00441274;
            }
            bVar8 = 1;
        }
    }
LAB_00440d1a:
    if (!bVar8 && (local_58 == NULL || local_5c == NULL || DCE_I(local_5c, 0xc) < 7)) {
        if (local_54 == NULL || DCE_P(local_54, 0x13f0) != pCar1 || iVar11 - DCE_I(local_54, 0x13e8) > 0x2ed) {
            if (local_54 == NULL) {
                if (iVar11 - DCE_I(pCar1, 0x13e8) > 0x2ed) {
                    goto LAB_00440db7;
                }
                iVar13 = (int)DCE_P(pCar1, 0x13f0);
            } else if (DCE_P(pCar1, 0x13f0) != local_54 || iVar11 - DCE_I(pCar1, 0x13e8) > 0x2ed) {
                if (local_54 != NULL) {
                    goto LAB_00440db7;
                }
                if (iVar11 - DCE_I(pCar1, 0x13e8) > 0x2ed) {
                    goto LAB_00440db7;
                }
                iVar13 = (int)DCE_P(pCar1, 0x13f0);
            } else {
                iVar13 = (int)local_54;
            }
            bVar9 = 1;
            local_5c = (tCar_spec*)iVar13;
            local_58 = pCar1;
        } else {
            bVar9 = 1;
            local_58 = local_54;
            local_5c = pCar1;
        }
    }
LAB_00440db7:
    if (local_5c != NULL && local_58 != NULL) {
        RecordOpponentTwattageOccurrence(local_58, local_5c);
        piVar17 = (int*)((char*)local_5c + 0x4e4);
        piVar19 = (int*)((char*)local_58 + 0x4e0);
        iVar20 = (int)((char*)local_5c - (char*)local_58);
        loop_n = 0xc;
        do {
            if (piVar19[1] < piVar19[0]) {
                iVar15 = (int)piVar19[1];
                piVar19[0] = iVar15;
                if (iVar15 > 0x62) {
                    piVar19[0] = 99;
                }
            }
            if (piVar17[0] < *(int*)((char*)piVar19 + iVar20)) {
                iVar15 = (int)piVar17[0];
                *(int*)((char*)piVar19 + iVar20) = iVar15;
                if (iVar15 < 0) {
                    *(int*)((char*)piVar19 + iVar20) = 0;
                }
            }
            piVar19 = piVar19 + 0xb;
            piVar17 = piVar17 + 0xb;
            loop_n = loop_n - 1;
        } while (loop_n != 0);
    }
    local_34 = 0;
    do {
        if (bVar8) {
            if (local_34 == 0) {
                local_5c = local_54;
                local_58 = pCar1;
            } else {
                local_58 = local_54;
                local_5c = pCar1;
            }
        }
        if (local_5c != NULL && (DCE_I(local_5c, 0xc) == 8 || gNet_mode != 0) && local_58 != NULL) {
            SetKnackeredFlag(local_5c);
            if (DCE_I(local_58, 0x1d4) != 0 && DCE_I(local_58, 0x1d8) == 0) {
                DCE_I(local_58, 0x1d8) = 1;
                if (gNet_mode == 0) {
                    PratcamEvent(0);
                    DoFancyHeadup(0);
                    EarnCredits(0);
                    AwardTime(0);
                }
            }
            DCE_I(local_58, 0x13e4) = iVar11;
            DCE_P(local_58, 0x13ec) = local_5c;
            if (!bVar9) {
                DCE_I(local_58, 0x13e8) = iVar11;
                DCE_P(local_58, 0x13f0) = local_5c;
            }
            iVar13 = DCE_I(local_58, 0x8);
            if (4.0f < (float)fabs(DCE_F(iVar13, 0x74)) || 6.0f < (float)fabs(DCE_F(iVar13, 0x78))
                || 4.0f < (float)fabs(DCE_F(iVar13, 0x7c))) {
                local_4c = local_4c * 2;
            }
            if (DCE_I(pCar1, 0x12e8) == 0) {
                BrMatrix34ApplyP(&local_18, (br_vector3*)((char*)DCE_P(pCar1, 0x8) + 0x14),
                                 (br_matrix34*)((char*)DCE_P(pCar1, 0x8) + 0x8c));
                local_18.v[1] = local_18.v[1] + 0.15f;
            }
            if (local_54 != NULL && DCE_I(local_54, 0x12e8) == 0) {
                BrMatrix34ApplyP(&local_c, (br_vector3*)((char*)DCE_P(local_54, 0x8) + 0x14),
                                 (br_matrix34*)((char*)DCE_P(local_54, 0x8) + 0x8c));
                local_c.v[1] = local_c.v[1] + 0.15f;
            }
            iVar13 = (int)uVar12;
            if (DCE_I(local_58, 0x1cc) == 0 && gNet_mode == 0 && (iVar13 * 100 != 0 || DCE_I(local_58, 0x1d4) != 0)) {
                if (DCE_I(local_58, 0x1d4) == 0) {
                    EarnCredits(0);
                    AwardTime(0);
                    if (local_54 != NULL && (bVar10 || local_4c > 2 || local_4c > 1)) {
                        DoFancyHeadup(0);
                    }
                }
                piVar19 = (int*)((char*)local_58 + 0x4e4);
                loop_n = 0xc;
                do {
                    piVar19[0] = piVar19[-1];
                    piVar19 = piVar19 + 0xb;
                    loop_n = loop_n - 1;
                } while (loop_n != 0);
            }
        }
        local_34 = local_34 + 1;
    } while (local_34 < bVar8 + 1);
    DCE_I(pCar1, 0x133c) = 0;
    if (local_54 != NULL) {
        DCE_I(local_54, 0x133c) = 0;
    }
LAB_00441239:
    if (uVar12 == 0 && (local_54 == NULL || !bVar5)) {
        return 0;
    }
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00440350
// FUNCTION: CARMA2_SW 0x00440430
void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period) {
    int i;
    br_vector3 temp_vector;
    br_vector3 wonky_vector;

    if (gAction_replay_mode && ARReplayIsReallyPaused()) {
        return;
    }

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if ((car != NULL && car->driver > eDriver_5) && !(car->car_crush_spec != NULL && car->car_crush_spec->field_0x144)) {
            int j;

            for (j = 0; j < (int)REC2_ASIZE(car->wheel_dam_offset); j++) {
                int damage;
                br_scalar y_amount;
                br_scalar z_amount;
                br_scalar wheel_circum;

                if (car->wheel_actors[j] == NULL) {
                    continue;
                }
                damage = car->damage_units[8 + j].damage_level;
                if (damage <= 30 || gRace_finished) {
                    car->wheel_dam_offset[j] = 0.f;
                    continue;
                }
                if (PointOutOfSight(&car->pos, 32.f)) {
                    break;
                }
                y_amount = (damage - 30) * gWobble_spam_y[damage & 7];
                z_amount = (damage - 30) * gWobble_spam_z[damage & 7];
                BrMatrix34PreRotateY(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(y_amount < 0.f ? y_amount + 360.f : y_amount));
                BrMatrix34PreRotateZ(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(z_amount < 0.f ? z_amount + 360.f : z_amount));
                if (j < 2 && car->wheel_actors[4 + j] != NULL) {
                    BrMatrix34PreRotateY(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(y_amount < 0.f ? y_amount + 360.f : y_amount));
                    BrMatrix34PreRotateZ(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(z_amount < 0.f ? z_amount + 360.f : z_amount));
                }
                switch (j) {
                case 0:
                    if (car->driven_wheels_spin_ref_1 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                case 1:
                    if (car->driven_wheels_spin_ref_2 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                case 2:
                    if (car->driven_wheels_spin_ref_3 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                case 3:
                    if (car->driven_wheels_spin_ref_4 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                }
                if (gNet_mode == eNet_mode_none || car->driver == eDriver_local_human) {
                    BrVector3Set(&temp_vector, wheel_circum * gWheel_circ_to_width, 0.f, 0.f);
                    BrMatrix34ApplyV(&wonky_vector, &temp_vector, &car->wheel_actors[j]->t.t.mat);
                    car->wheel_dam_offset[j] = (float)fabs(wonky_vector.v[1]);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00440640
// FUNCTION: CARMA2_SW 0x00440720
void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

    if (DoCrashEarnings(pCar1, pCar2)) {
        SortOutSmoke(pCar1);
        SortOutSmoke(pCar2);
    }
}

// PercentageCarHealth

// FUNCTION: CARMA2_HW 0x00441420
// FUNCTION: CARMA2_SW 0x00441500
void C2_HOOK_FASTCALL ChristmasLights(tCar_spec* pCar, int pMin_damage, int pMax_damage) {
    int restarts = 0;
    tDamage_type damage_type = eDamage_start;

    for (;;) {
        DamageUnit2(pCar, damage_type, IRandomBetween(pMin_damage, pMax_damage));
        if (damage_type == eDamage_engine && pCar->damage_units[damage_type].damage_level < 80) {
            restarts += 1;
            if (restarts < 30) {
                continue;
            }
        }
        damage_type++;
        if (damage_type >= eDamage_count) {
            break;

        }
    }
    pCar->damage_magnitude_accumulator += 0.5f;
}
