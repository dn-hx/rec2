#include "58-crush2.h"

#include "07-structur.h"
#include "09-funks.h"
#include "15-displays.h"
#include "21-mainloop.h"
#include "30-opponent.h"
#include "39-oil.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"

extern int gExplosion_sound_id;
extern br_vector3 gZero_v__car;
extern tS3_outlet* gCar_outlet;
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);

// GLOBAL: CARMA2_HW 0x0074d3b0
int gCredits_wasting_car[3];

// GLOBAL: CARMA2_HW 0x0075b930
float gCredits_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x0075b9b0
int gTime_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761a50
int gCredits_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761ed0
float gSeconds_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x00763520
int gTime_wasting_car[3];

// GLOBAL: CARMA2_HW 0x00763530
int gTime_per_ped_kill[3];

// CheckLastCar

// FUNCTION: CARMA2_HW 0x0043f5f0
// FUNCTION: CARMA2_SW 0x0043f6d0
void C2_HOOK_FASTCALL KnackerThisCar(tCar_spec* pCar) {

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
    if (gNet_mode == eNet_mode_none && GetCarCount(eVehicle_opponent) != 0 && NumberOfOpponentsLeft() == 0) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
            if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                NewTextHeadupSlot(4, 0, 5000, -4, GetMiscString(10));
            }
            RaceCompleted(eRace_over_2);
        }
    }
    QueueOilSpill(pCar);
    if (gNet_mode == eNet_mode_none) {
        KillGroovadelic(pCar->index);
        KillFunkotronic(pCar->index);
    }
}

extern void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime);

// (reference only)
// FUNCTION: CARMA2_HW 0x0043f730
int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar) {

    if (gNet_mode == eNet_mode_client || pCar->knackered
        || (pCar->damage_units[0].damage_level < 99 && pCar->damage_units[1].damage_level < 99 && pCar->damage_units[2].damage_level < 99
            && (pCar->damage_units[8].damage_level < 99 || pCar->damage_units[9].damage_level < 99 || pCar->damage_units[10].damage_level < 99 || pCar->damage_units[11].damage_level < 99))) {
        return 0;
    }
    ExplodeMeCarUpMeOldMatey(pCar);
    if (!pCar->invulnerable_no_wastage && gNet_mode != eNet_mode_client) {
        if (gNet_mode == eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
            RaceCompleted(eRace_over_6);
        }
        pCar->knackered = 1;
        QueueWastedMassage(pCar->index);
        if (gNet_mode == eNet_mode_none && GetCarCount(eVehicle_opponent) != 0 && NumberOfOpponentsLeft() == 0) {
            if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                    NewTextHeadupSlot(4, 0, 5000, -4, GetMiscString(10));
                }
                RaceCompleted(eRace_over_2);
            }
        }
        QueueOilSpill(pCar);
        if (gNet_mode == eNet_mode_none) {
            KillGroovadelic(pCar->index);
            KillFunkotronic(pCar->index);
        }
    }
    if (gNet_mode == eNet_mode_none) {
        if (IRandomBetween(0, 1) && gNet_mode == eNet_mode_none) {
            StopCarSmoking(pCar);
            CreateSmokeColumn(pCar, 0, IRandomBetween(0, 11), 20000);
        }
        CreateSmokeColumn(pCar, 0, IRandomBetween(0, 11), 180000);
    }
    return 1;
}

// DamageUnit2

// RecordLastDamage

// DoDamage

// FUNCTION: CARMA2_HW 0x0043f9f0
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

// based on ApexWorm/rec2 1e0d5a8ab (reference only)
// FUNCTION: CARMA2_HW 0x00441350
int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar) {
    int engine;
    int transmission;
    int driver;
    int wheels;
    int health;

    if (pCar->knackered) {
        return 0;
    }

    wheels = pCar->damage_units[eDamage_lr_wheel].damage_level;
    wheels += pCar->damage_units[eDamage_rr_wheel].damage_level;
    wheels += pCar->damage_units[eDamage_lf_wheel].damage_level;
    wheels += pCar->damage_units[eDamage_rf_wheel].damage_level;
    engine = pCar->damage_units[eDamage_engine].damage_level;
    transmission = pCar->damage_units[eDamage_transmission].damage_level;
    driver = pCar->damage_units[eDamage_driver].damage_level;
    wheels /= 4;

    if (engine <= 75 && transmission <= 75 && driver <= 75 && wheels <= 75) {
        health = 100 - (wheels + driver + transmission + engine) / 4;
        if (health < 25) {
            health = 25;
        }
        return health;
    }
    if (engine > transmission && engine > driver && engine > wheels) {
        return 100 - engine;
    }
    if (transmission > driver && transmission > wheels) {
        return 100 - transmission;
    }
    if (driver > wheels) {
        return 100 - driver;
    }
    return 100 - wheels;
}

// FUNCTION: CARMA2_HW 0x0043faa0
// FUNCTION: CARMA2_SW 0x0043fb80
void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity) {
    extern br_scalar gFloat_00589b64;
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
        if (pVelocity->v[0] < gFloat_00589b64) {
            PratcamEvent(offset + 16);
        } else {
            PratcamEvent(offset + 15);
        }
    } else {
        if (pVelocity->v[2] < gFloat_00589b64) {
            PratcamEvent(offset + 13);
        } else {
            PratcamEvent(offset + 14);
        }
    }
}

/* Moved out of 00-car.c: keeping this body in that TU perturbed the x87 scheduling of
   FullyDetachSnappedBits 0x431310 (97.69). Must stay immediately after DoPratcamHit here: at
   the end of this file the same body scores 85.42% (tail emits fld [edx]; fadd st(1); fstp). */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
typedef struct {
    tCar_spec* car;
    float step;
    float* total_displacement;
    float displacement;
} tCrush_displacement_context_local;
// FUN_0043b2e0 - body (GPL-3.0). It was kept
// here as a PARTIAL at 85.42% (the remaining diff then was the x87 tail idiom); the final tree, with
// the block-scope `delta` temp in the axis accumulation and the tail statement
// `*total_displacement = displacement + *total_displacement;`, verifies at 100% (re-checked after the
// FUN_0043fb70 work). The original is at 0x0043b2e0.
// FUNCTION: CARMA2_HW 0x0043b2e0
intptr_t C2_HOOK_CDECL FUN_0043b2e0(br_actor* pActor, tCrush_displacement_context_local* pContext) {
    extern br_scalar gCrushZero;
    tUser_crush_data* user;
    br_vector3 original_position;
    float displacement;
    int model_index;
    int vertex_index;
    int axis;
    br_model** model_cursor;

    user = pActor->user;
    if (user == NULL || user->crush_data == NULL || user->crush_data->field_0x2c == 0) {
        return 0;
    }
    model_index = 0;
    if (model_index < pContext->car->count_detail_levels) {
        model_cursor = user->models;
        do {
            br_model* model;
            float* total_displacement;

            total_displacement = pContext->total_displacement;
            model = *model_cursor;
            if (model != NULL && model->user != NULL
                    && ((tUser_detail_level_model*)model->user)->field_0x4 != NULL) {
                tModel_detail_vertex_data* original_vertices;

                original_vertices = ((tUser_detail_level_model*)model->user)->field_0x4;
                displacement = gCrushZero;
                for (vertex_index = 0; vertex_index < model->nvertices; vertex_index++) {
                    ExpandVector3(&original_position, &original_vertices[vertex_index].p, -10.f, 10.f);
                    for (axis = 0; axis < 3; axis++) {
                        float delta;
                        delta = original_position.v[axis] - model->vertices[vertex_index].p.v[axis];
                        delta = fabs(delta);
                        displacement = displacement + delta;
                    }
                }
                *total_displacement = displacement + *total_displacement;
            }
            model_index++;
            model_cursor++;
        } while (model_index < pContext->car->count_detail_levels);
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DamageSystems

// GetDirection

// FUNCTION: CARMA2_HW 0x00440210
void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].smoke_last_level = pCar->damage_units[i].damage_level;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern unsigned char gSmoke_part_table[12];
extern void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pLevel);
extern int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);

// FUNCTION: CARMA2_HW 0x00440230
void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar) {
    int iVar8;
    unsigned int uVar6;
    int* piVar7;
    int* piVar5;
    int iVar4;
    int bVar3;
    int iVar2;
    int iVar1;

    iVar8 = 0;
    bVar3 = 0;
    if (pCar != NULL && pCar->driver > 5) {
        piVar5 = &pCar->damage_units[0].smoke_last_level;
        piVar7 = piVar5;
        do {
            uVar6 = gSmoke_part_table[iVar8];
            if (uVar6 != 0) {
                iVar1 = piVar7[-2];
                iVar4 = (99 - iVar1) / (int)uVar6;
                if (iVar4 < 3) {
                    bVar3 = 1;
                }
                iVar2 = *piVar7;
                if (iVar1 != iVar2 && uVar6 != 0 && iVar2 < iVar1 && uVar6 != 0 && iVar4 != (99 - iVar2) / (int)uVar6 && iVar4 < 3) {
                    ConditionalSmokeColumn(pCar, iVar8, iVar4);
                }
            }
            iVar8 = iVar8 + 1;
            piVar7 = piVar7 + 0xb;
        } while (iVar8 < 0xc);
        if (!bVar3 && (iVar8 = IsCarSmoking(pCar), iVar8 != 0)) {
            StopCarSmoking(pCar);
        }
        iVar8 = 0xc;
        do {
            *piVar5 = piVar5[-2];
            piVar5 = piVar5 + 0xb;
            iVar8 = iVar8 - 1;
        } while (iVar8 != 0);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar);
#define DCE_I(p, o) (*(int*)((char*)(p) + (o)))
#define DCE_U(p, o) (*(tU32*)((char*)(p) + (o)))
#define DCE_F(p, o) (*(float*)((char*)(p) + (o)))
#define DCE_P(p, o) (*(tCar_spec**)((char*)(p) + (o)))

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

#undef DCE_I
#undef DCE_U
#undef DCE_F
#undef DCE_P

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00440640
// FUNCTION: CARMA2_SW 0x00440720
void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

    if (DoCrashEarnings(pCar1, pCar2)) {
        SortOutSmoke(pCar1);
        SortOutSmoke(pCar2);
    }
}

// ChristmasLights


// (reference only)
// FUNCTION: CARMA2_HW 0x0043f6c0
void C2_HOOK_FASTCALL ExplodeMeCarUpMeOldMatey(tCar_spec* pCar) {

    Explode(&gExplosion_pix_animation_groups, pCar->car_master_actor, &pCar->car_actor->model->bounds, NULL);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound(gCar_outlet, gExplosion_sound_id);
    } else {
        DRS3StartSound3D(gCar_outlet, gExplosion_sound_id, &pCar->pos, &gZero_v__car, 1, 255, 0x10000, 0x10000);
    }
}

// GLOBAL: CARMA2_HW 0x00590020
float gWobble_spam_y[8] = { 0.f, -0.15f, 0.4f, 0.15f, -0.4f, 0.25f, 0.f, -0.25f };

// GLOBAL: CARMA2_HW 0x00590040
float gWobble_spam_z[8] = { 0.4f, -0.25f, 0.f, 0.25f, 0.f, 0.15f, -0.4f, -0.15f };

// GLOBAL: CARMA2_HW 0x00590060
float gWheel_circ_to_width = 0.16f;

// FUNCTION: CARMA2_HW 0x00440350
void C2_HOOK_FASTCALL DoWheelDamage(void) {
    extern int gNum_active_cars;
    extern tCar_spec* gActive_car_list[139];
    extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);
    extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, br_scalar pMax_distance);
    int i;

    if (gAction_replay_mode && ARReplayIsReallyPaused()) {
        return;
    }

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if ((car != NULL && car->driver > 5) && !(car->car_crush_spec != NULL && car->car_crush_spec->field_0x144)) {
            int j;

            for (j = 0; j < 4; j++) {
                int damage;
                br_scalar wheel_circum;
                br_scalar z_amount;
                br_scalar y_amount;

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
                BrMatrix34PreRotateY(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(y_amount < 0 ? y_amount + 360.f : y_amount));
                BrMatrix34PreRotateZ(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(z_amount < 0 ? z_amount + 360.f : z_amount));
                if (j < 2 && car->wheel_actors[4 + j] != NULL) {
                    BrMatrix34PreRotateY(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(y_amount < 0 ? y_amount + 360.f : y_amount));
                    BrMatrix34PreRotateZ(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(z_amount < 0 ? z_amount + 360.f : z_amount));
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
                    br_vector3 temp_vector;
                    br_vector3 wonky_vector;

                    BrVector3Set(&temp_vector, wheel_circum * gWheel_circ_to_width, 0.f, 0.f);
                    BrMatrix34ApplyV(&wonky_vector, &temp_vector, &car->wheel_actors[j]->t.t.mat);
                    car->wheel_dam_offset[j] = fabs(wonky_vector.v[1]);
                }
            }
        }
    }
}

// GLOBAL: CARMA2_HW 0x00590068
unsigned char gSmoke_part_table[12] = {
    0x14, 0x14, 0x00, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a
};


/* Moved here from 00-car.c (2026-11-02): the same two blocks at the end of 00-car.c cost
   FullyDetachSnappedBits 0x00431310 its 100 (97.69, two fld operands swapped) as soon as
   FUN_0043b400 had any stack frame at all; no placement inside that TU avoided it. Like
   FUN_0043b2e0 above, they live in this TU instead. Bodies are unchanged.
   The typedef below is a file-local copy of the one in 00-car.c (identical definition). */
typedef struct {
    tCar_spec* car;
    float step;
    float displacement;
    float* total_displacement;
} tModel_mash_context_local;

extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern intptr_t C2_HOOK_CDECL FUN_00434320(br_actor* pActor, int pArg2);

/* The original walks the model hierarchy with FUN_0043b470 (0x0043b470); that callback is not
   implemented in rec2 yet, so an inert one keeps this runtime-safe. */
// STUB: CARMA2_HW 0x0043b470
intptr_t C2_HOOK_CDECL FUN_0043b470(br_actor* pActor, tModel_mash_context_local* pContext) {
    (void)pActor; (void)pContext;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0043b400
int C2_HOOK_FASTCALL FUN_0043b400(tPhysics_object* pObject, tCrush_displacement_context_local* pContext) {
    tCrush_displacement_context_local* context;
    tModel_mash_context_local model_context;

    context = pContext;
    model_context.car = context->car;
    model_context.step = context->step;
    model_context.displacement = 0.0f;
    model_context.total_displacement = context->total_displacement;

    ARStartPipingSession(0x18);
    DRActorEnumRecurse(pObject->actor, (br_actor_enum_cbfn*)FUN_0043b470, &model_context);
    AREndPipingSession();
    DRActorEnumRecurse(pObject->actor, (br_actor_enum_cbfn*)FUN_00434320, NULL);

    context->displacement += model_context.displacement;
    return 0;
}
