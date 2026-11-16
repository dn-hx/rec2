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
#include "00-car.h"
#include "13-crush1.h"
#include "17-world2.h"
#include "23-piping.h"
#include "32-spark.h"
#include "51-smash.h"
#include "69-sound.h"
#include "platform.h"


/* Moved out of 00-car.c: keeping this body in that TU perturbed the x87 scheduling of
   DoJointSnapping 0x431310 (97.69). Must stay immediately after DoPratcamHit here: at
   the end of this file the same body scores 85.42% (tail emits fld [edx]; fadd st(1); fstp). */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* tCrush_displacement_context_local is declared in 58-crush2.h so the
   prototypes of AddTotalDeflection/RepairObject can name it. */
// AddTotalDeflection - body (GPL-3.0). It was kept
// here as a PARTIAL at 85.42% (the remaining diff then was the x87 tail idiom); the final tree, with
// the block-scope `delta` temp in the axis accumulation and the tail statement
// `*total_displacement = displacement + *total_displacement;`, verifies at 100% (re-checked after the
// DamageSystems work). The original is at 0x0043b2e0.
// PARTIAL 85.42%: AddTotalDeflection 0x0043b2e0 [x87 tail: fld [edx]; fadd st(1); fstp vs original fadd [edx]]
// FUNCTION: CARMA2_HW 0x0043b2e0
intptr_t C2_HOOK_CDECL AddTotalDeflection(br_actor* pActor, tCrush_displacement_context_local* pContext) {
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
// FUNCTION: CARMA2_HW 0x0043b400
int C2_HOOK_FASTCALL RepairObject(tPhysics_object* pObject, tCrush_displacement_context_local* pContext) {
    tCrush_displacement_context_local* context;
    tModel_mash_context_local model_context;

    context = pContext;
    model_context.car = context->car;
    model_context.step = context->step;
    model_context.displacement = 0.0f;
    model_context.total_displacement = context->total_displacement;

    ARStartPipingSession(0x18);
    DRActorEnumRecurse(pObject->actor, (br_actor_enum_cbfn*)RepairModels_2, &model_context);
    AREndPipingSession();
    DRActorEnumRecurse(pObject->actor, (br_actor_enum_cbfn*)CheckFlapAngles, NULL);

    context->displacement += model_context.displacement;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0043b470
intptr_t C2_HOOK_CDECL RepairModels_2(br_actor* pActor, tModel_mash_context_local* pContext) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    int model_index;
    br_model** model_cursor;

    user = pActor->user;
    if (user == NULL || user->crush_data == NULL) {
        return 0;
    }
    crush_data = user->crush_data;
    if (RepairModel(user->models[0], pContext) != 0) {
        RepairSmash(pContext->car, user->crush_data);
        crush_data->field_0x2c = 0;
        if (crush_data->flap_data != NULL && crush_data->flap_data->kev_o_flap == 0) {
            crush_data->flap_data->field_0x15 = 0;
            crush_data->flap_data->field_0x14 = 1;
            crush_data->flap_data->field_0x18 = 0.f;
        }
        if (crush_data->detach_data != NULL) {
            crush_data->detach_data->field_0x0 = 0.f;
        }
        return 0;
    }
    model_index = 1;
    if (model_index < pContext->car->count_detail_levels) {
        model_cursor = &user->models[1];
        do {
            RepairModel(*model_cursor, pContext);
            model_index++;
            model_cursor++;
        } while (model_index < pContext->car->count_detail_levels);
    }
    return 0;
}

// GLOBAL: CARMA2_HW 0x00590068
unsigned char gSmoke_part_table[12] = {
    0x14, 0x14, 0x00, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a
};


/* Moved here from 00-car.c (2026-11-02): the same two blocks at the end of 00-car.c cost
   DoJointSnapping 0x00431310 its 100 (97.69, two fld operands swapped) as soon as
   RepairObject had any stack frame at all; no placement inside that TU avoided it. Like
   AddTotalDeflection above, they live in this TU instead. Bodies are unchanged.
   The context typedef is declared in 58-crush2.h (identical definition to the copy in 00-car.c). */

/* Relaxes every vertex of one detail-level model back towards its uncrushed position by at most
   `step` per pass, accumulating the applied movement into the context and piping the per-vertex
   displacement as a model mash. Returns 1 when nothing moved, 0 otherwise.
   PARTIAL 54.65% (6 attempts): frame (0x3030), stack slots, both step branches, the mash zero-store
   block, the total_displacement accumulation and the final displacement store now match exactly
   (condition is `delta > gCrushZero` with the zero store as the else, which is what the original's
   block order and `jne` polarity prove). Residual: MSVC 11 homes pModel in ebx, the vertex pointer
   in edi and -vertex_data in ebp where the original uses esi/ebx/edi (callee-saved rotation) plus
   the x87 shape of one registered delta (fld/fadd st(1) vs fadd [mem]). Best copy:
   state/partials/FUN_0043b510.c */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0043b510
int C2_HOOK_FASTCALL RepairModel(br_model* pModel, tModel_mash_context_local* pContext) {
    br_vector3 mash[1024];
    tUser_detail_level_model* detail_model;
    tModel_detail_vertex_data* vertex_data;
    br_vector3 original_position;
    br_vector3 vertex_position;
    br_vertex* vertex;
    int i;
    int j;
    float displacement;

    if (pModel == NULL) {
        return 0;
    }
    detail_model = pModel->user;
    if (detail_model == NULL) {
        return 0;
    }
    vertex_data = detail_model->field_0x4;
    if (vertex_data == NULL) {
        return 0;
    }
    if (pModel->nvertices > REC2_ASIZE(mash)) {
        PDFatalError("Dog gone it, there's a lot of vertices on that model.");
    }
    displacement = gCrushZero;
    vertex = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++) {
        ExpandVector3(&original_position, &vertex_data[i].p, -10.f, 10.f);
        vertex_position = vertex->p;
        for (j = 0; j < 3; j++) {
            float delta;

            delta = original_position.v[j] - vertex_position.v[j];
            delta = fabs(delta);
            if (delta > gCrushZero) {
                *pContext->total_displacement = *pContext->total_displacement + delta;
                if (original_position.v[j] < vertex_position.v[j]) {
                    vertex->p.v[j] = vertex->p.v[j] - pContext->step;
                    if (vertex->p.v[j] < original_position.v[j]) {
                        vertex->p.v[j] = original_position.v[j];
                    }
                    delta = vertex->p.v[j] - vertex_position.v[j];
                    displacement = displacement - delta;
                    mash[i].v[j] = delta;
                } else {
                    vertex->p.v[j] = pContext->step + vertex->p.v[j];
                    if (vertex->p.v[j] > original_position.v[j]) {
                        vertex->p.v[j] = original_position.v[j];
                    }
                    delta = vertex->p.v[j] - vertex_position.v[j];
                    displacement = displacement + delta;
                    mash[i].v[j] = delta;
                }
                vertex_data[i].flags &= 0xfb;
            } else {
                mash[i].v[j] = 0.f;
            }
        }
        vertex++;
    }
    if (displacement == gCrushZero) {
        return 1;
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        ((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
    AddModelMashToPipingSession(pModel, mash);
    {
        float previous_displacement;

        previous_displacement = pContext->displacement;
        pContext->displacement = previous_displacement + displacement;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
extern tS3_outlet* gCar_outlet;


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

// DamageUnit2

// FUNCTION: CARMA2_HW 0x0043f9f0
void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar) {
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
// GLOBAL: CARMA2_HW 0x00590020
float gWobble_spam_y[8] = { 0.f, -0.15f, 0.4f, 0.15f, -0.4f, 0.25f, 0.f, -0.25f };

// GLOBAL: CARMA2_HW 0x00590040
float gWobble_spam_z[8] = { 0.4f, -0.25f, 0.f, 0.25f, 0.f, 0.15f, -0.4f, -0.15f };

// GLOBAL: CARMA2_HW 0x00590060
float gWheel_circ_to_width = 0.16f;

// FUNCTION: CARMA2_HW 0x00440350
void C2_HOOK_FASTCALL DoWheelDamage(void) {
    extern tCar_spec* gActive_car_list[139];
    extern br_scalar gFloat_00589b64;
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
                BrMatrix34PreRotateY(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(y_amount < gFloat_00589b64 ? y_amount + 360.f : y_amount));
                BrMatrix34PreRotateZ(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(z_amount < gFloat_00589b64 ? z_amount + 360.f : z_amount));
                if (j < 2 && car->wheel_actors[4 + j] != NULL) {
                    BrMatrix34PreRotateY(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(y_amount < gFloat_00589b64 ? y_amount + 360.f : y_amount));
                    BrMatrix34PreRotateZ(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(z_amount < gFloat_00589b64 ? z_amount + 360.f : z_amount));
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

                    temp_vector.v[1] = 0.f;
                    temp_vector.v[2] = 0.f;
                    temp_vector.v[0] = wheel_circum * gWheel_circ_to_width;
                    BrMatrix34ApplyV(&wonky_vector, &temp_vector, &car->wheel_actors[j]->t.t.mat);
                    car->wheel_dam_offset[j] = fabs(wonky_vector.v[1]);
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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



#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#define DCE_I(p, o) (*(int*)((char*)(p) + (o)))
#define DCE_U(p, o) (*(tU32*)((char*)(p) + (o)))
#define DCE_F(p, o) (*(float*)((char*)(p) + (o)))
#define DCE_P(p, o) (*(tCar_spec**)((char*)(p) + (o)))

#define DCE_V(p, o) (*(char**)((char*)(p) + (o)))
#define ABS(a) (((a) < 0) ? -(a) : (a))

extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);

// GLOBAL: CARMA2_HW 0x0075bba0
int gCrash_earnings_index;

// GLOBAL: CARMA2_HW 0x0067c3b4
int gLast_credit_award_time;

static __inline int DCE_Tier20(tCar_spec* p) {
    int m;
    m = DCE_I(p, 0x6f4);
    if (m == 2 || m == 3 || m == 0 || m == 1) {
        if (DCE_F(p, 0x13e0) < 0.25) {
            return 4;
        }
        if (DCE_F(p, 0x13e0) <= 0.75) {
            return m;
        }
        return 5;
    }
    return m;
}

static __inline int DCE_Tier13(tCar_spec* p) {
    float c0;
    float c4;
    float c8;
    c0 = DCE_F(p, 0x13c0);
    c4 = DCE_F(p, 0x13c4);
    c8 = DCE_F(p, 0x13c8);
    if (ABS(c0) > ABS(c4) && ABS(c0) > ABS(c8)) {
        return (c0 < 0.0f) ? 2 : 3;
    }
    if (ABS(c4) > ABS(c0) && ABS(c4) > ABS(c8)) {
        return (c4 < 0.0f) ? 1 : 0;
    }
    return (c8 < 0.0f) ? 4 : 5;
}

// PARTIAL unmeasured: DoCrashEarnings 0x00440660 [clean-room port; counted budget 0, no reccmp % yet; proxy raw 34.31]
// STUB: CARMA2_SW 0x00440740
// FUNCTION: CARMA2_HW 0x00440660
int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {
    tCar_spec* local_5c;
    tCar_spec* local_58;
    tCar_spec* local_54;
    tCar_spec* partner;
    int local_4c;
    int local_34;
    int local_3c;
    int flag3c;
    int bVar5;
    int bVar8;
    int bVar9;
    int time;
    int uVar12;
    int uVar18;
    int iVar20;
    int iVar13;
    int cnt;
    int cond1;
    int cond2;
    int sgn;
    int credits;
    int c2;
    int tt;
    int mn;
    int k;
    int diff;
    int d;
    int dsum;
    int dmgv;
    int* pa;
    int* pb;
    int* po;
    char* mat;
    double sq;
    float fdmg;
    float dot;
    float cc0;
    float cc4;
    float cc8;
    br_vector3 vec1;
    br_vector3 vec2;
    tNet_message* msg;
    tNet_game_player_info* np;

    local_5c = 0;
    local_58 = 0;
    local_54 = pCar2;
    local_4c = 1;
    bVar8 = 0;
    bVar9 = 0;
    flag3c = 0;
    bVar5 = 0;
    local_3c = 0;

    time = (int)PDGetTotalTime();
    uVar12 = (int)((pCar1 == 0 || DCE_I(pCar1, 0xc) <= 5) ? 0.0f : DCE_F(pCar1, 0x133c));
    if (pCar2 != 0 && DCE_I(pCar2, 0xc) > 5) {
        if (DCE_F(pCar2, 0x133c) != 0.0f) {
            bVar5 = 1;
        }
    }

    if (pCar1 == 0 || DCE_I(pCar1, 0xc) <= 5) {
        if (pCar2 == 0 || DCE_I(pCar2, 0xc) <= 5) {
            return 0;
        }
        if (pCar1 != 0) {
            iVar13 = DCE_I(pCar1, 0xc);
            if (iVar13 != 0 && (iVar13 == 4 || iVar13 == 2 || iVar13 == 3)) {
                partner = DCE_P(pCar1, 0xc8);
                if (partner != 0 && DCE_I(partner, 0xc) > 5 && partner != pCar2) {
                    DCE_I(pCar2, 0x13e4) = time;
                    DCE_P(pCar2, 0x13ec) = DCE_P(pCar1, 0xc8);
                }
            }
        }
        local_54 = 0;
        pCar1 = pCar2;
    }

    if (local_54 != 0 && DCE_I(local_54, 0xc) <= 5) {
        iVar13 = DCE_I(local_54, 0xc);
        if (iVar13 != 0 && (iVar13 == 4 || iVar13 == 2 || iVar13 == 3)) {
            partner = DCE_P(local_54, 0xc8);
            if (partner != 0 && DCE_I(pCar1, 0xc) > 5 && DCE_I(partner, 0xc) > 5 && partner != pCar1) {
                DCE_I(pCar1, 0x13e4) = time;
                DCE_P(pCar1, 0x13ec) = DCE_P(local_54, 0xc8);
            }
        }
        local_54 = 0;
    }

    if (DCE_I(pCar1, 0x1d8) != 0) {
        goto L_441239;
    }
    if (local_54 != 0) {
        if (DCE_I(local_54, 0x1d8) != 0) {
            goto L_441239;
        }
        if (DCE_F(local_54, 0x133c) <= 5e-05f && DCE_F(pCar1, 0x133c) <= 5e-05f) {
            goto L_441239;
        }
    }

    iVar20 = DCE_Tier20(pCar1);
    iVar13 = DCE_Tier13(pCar1);

    uVar18 = uVar12;
    if (local_54 != 0) {
        local_3c = DCE_Tier20(local_54);
        uVar18 = (local_3c == DCE_Tier13(local_54));
    }

    if (pCar1 == 0 || DCE_I(pCar1, 0xc) < 7) {
        if (local_54 == 0) {
            goto L_440b7f;
        }
        if (DCE_I(local_54, 0xc) >= 7 && uVar18 != 0 && DCE_F(local_54, 0x13c8) != 0.0f) {
            sgn = (DCE_F(local_54, 0x13c8) > 0.0f);
            if (((DCE_I(local_54, 0x135c) > 0) ^ sgn) != 0
                && ((DCE_I(local_54, 0x12d0) & 0x40000) != 0 || DCE_I(local_54, 0x12dc) > 0x8000)) {
                DCE_P(pCar1, 0x13ec) = local_54;
                DCE_I(pCar1, 0x13e4) = time;
            }
        }
        goto L_440b7b;
    }
    if (local_54 == 0) {
        goto L_440b7f;
    }
    if (iVar20 == iVar13 && DCE_F(pCar1, 0x13c8) != 0.0f) {
        sgn = (DCE_F(pCar1, 0x13c8) > 0.0f);
        if (((DCE_I(pCar1, 0x135c) > 0) ^ sgn) != 0
            && ((DCE_I(pCar1, 0x12d0) & 0x40000) != 0 || DCE_I(pCar1, 0x12dc) > 0x8000)) {
            DCE_P(local_54, 0x13ec) = pCar1;
            DCE_I(local_54, 0x13e4) = time;
        }
    }

L_440b7b:
    if (local_54 != 0) {
        goto L_440bb7;
    }

L_440b7f:
    if (time - DCE_I(pCar1, 0x13e4) > 4999) {
        return 1;
    }
    local_5c = DCE_P(pCar1, 0x13ec);
    local_4c = 1;
    bVar9 = 1;
    local_58 = pCar1;
    goto L_440d1a;

L_440bb7:
    cond1 = 0;
    if (iVar20 == iVar13 && DCE_F(pCar1, 0x13a4) * 5.0f > DCE_F(local_54, 0x13a4)
        && DCE_F(pCar1, 0x13a4) > 0.0005f) {
        if (DCE_I(pCar1, 0xc) < 7) {
            cond1 = 1;
        } else if (DCE_F(pCar1, 0x13c8) != 0.0f) {
            sgn = (DCE_F(pCar1, 0x13c8) > 0.0f);
            if (((DCE_I(pCar1, 0x135c) > 0) ^ sgn) != 0
                && ((DCE_I(pCar1, 0x12d0) & 0x40000) != 0 || DCE_I(pCar1, 0x12dc) > 0x8000)) {
                cond1 = 1;
            }
        }
    }

    cond2 = 0;
    if (uVar18 != 0 && DCE_F(local_54, 0x13a4) * 5.0f > DCE_F(pCar1, 0x13a4)
        && DCE_F(local_54, 0x13a4) > 0.0005f) {
        if (DCE_I(local_54, 0xc) < 7) {
            cond2 = 1;
        } else if (DCE_F(local_54, 0x13c8) != 0.0f) {
            sgn = (DCE_F(local_54, 0x13c8) > 0.0f);
            if (((DCE_I(local_54, 0x135c) > 0) ^ sgn) != 0
                && ((DCE_I(local_54, 0x12d0) & 0x40000) != 0 || DCE_I(local_54, 0x12dc) > 0x8000)) {
                cond2 = 1;
            }
        }
    }

    if (gNet_mode == 0 || cond1 == 0) {
        if (cond2 != 0 && DCE_I(local_54, 0xc) == 8) {
            cond1 = 0;
        }
    } else {
        if (cond2 == 0) {
            goto L_441274;
        }
        bVar8 = 1;
        goto L_440d1a;
    }

L_441274:
    if (cond1 != 0) {
        local_5c = pCar1;
        local_58 = local_54;
    } else if (cond2 != 0) {
        local_5c = local_54;
        local_58 = pCar1;
    } else {
        goto L_440d1a;
    }
    cc0 = DCE_F(local_54, 0x13a8) * DCE_F(pCar1, 0x13a8);
    cc4 = DCE_F(local_54, 0x13ac) * DCE_F(pCar1, 0x13ac);
    cc8 = DCE_F(local_54, 0x13b0) * DCE_F(pCar1, 0x13b0);
    dot = (cc0 + cc4) + cc8;
    local_4c = 1;
    if (iVar20 == 4 && local_3c == 4 && DCE_F(pCar1, 0x13a4) > 0.001f
        && DCE_F(local_54, 0x13a4) > 0.001f && dot < -0.7f) {
        flag3c = 1;
        local_4c = 2;
    }
    goto L_440d1a;

L_440d1a:
    if (bVar8 != 0) {
        goto L_440db7;
    }
    if (local_58 == 0 || local_5c == 0 || DCE_I(local_5c, 0xc) < 7) {
        if (local_54 != 0 && DCE_P(local_54, 0x13f0) == pCar1 && time - DCE_I(local_54, 0x13e8) < 750) {
            bVar9 = 1;
            local_5c = pCar1;
            local_58 = local_54;
        } else if (local_54 != 0 && DCE_P(pCar1, 0x13f0) == local_54 && time - DCE_I(pCar1, 0x13e8) < 750) {
            bVar9 = 1;
            local_5c = local_54;
            local_58 = pCar1;
        } else if (local_54 == 0 && time - DCE_I(pCar1, 0x13e8) < 750) {
            bVar9 = 1;
            local_5c = DCE_P(pCar1, 0x13f0);
            local_58 = pCar1;
        }
    }

L_440db7:
    if (local_5c != 0 && local_58 != 0) {
        RecordOpponentTwattageOccurrence(local_5c, local_58);
        diff = (int)((char*)local_5c - (char*)local_58);
        pa = (int*)((char*)local_58 + 0x4e0);
        pb = (int*)((char*)local_5c + 0x4e4);
        dsum = 0;
        for (k = 0; k < 12; k++) {
            po = (int*)((char*)pa + diff);
            if (pa[1] < pa[0]) {
                d = pa[0] - pa[1];
                tt = (int)((double)pa[1] - (double)d * -2.0);
                pa[0] = tt;
                if (tt > 0x62) {
                    pa[0] = 99;
                }
                dsum += pa[0] - pa[1];
            }
            if (*pb < *po) {
                d = *po - *pb;
                tt = (int)((double)*pb - (double)d * -0.1);
                *po = tt;
                if (tt < 0) {
                    *po = 0;
                }
            }
            pa += 11;
            pb += 11;
        }
    }

    cnt = (bVar8 != 0) + 1;
    for (local_34 = 0; local_34 < cnt; local_34++) {
        if (bVar8 != 0) {
            if (local_34 == 0) {
                local_5c = local_54;
                local_58 = pCar1;
            } else {
                local_58 = local_54;
                local_5c = pCar1;
            }
        }
        if (local_5c != 0 && (DCE_I(local_5c, 0xc) == 8 || gNet_mode != 0) && local_58 != 0) {
            SetKnackeredFlag(local_58);
            if (DCE_I(local_58, 0x1d4) != 0 && DCE_I(local_58, 0x1d8) == 0) {
                DCE_I(local_58, 0x1d8) = 1;
                if (gNet_mode == 0) {
                    PratcamEvent(0x20);
                    DoFancyHeadup(0x14);
                    sq = sqr(0.7 / DCE_F(local_58, 0x18d0));
                    credits = (int)((sq * (double)gCredits_wasting_car[gCrash_earnings_index] - -50.0) * 0.01);
                    EarnCredits(credits * 100);
                    AwardTime(gTime_wasting_car[gCrash_earnings_index]);
                } else {
                    msg = NetBuildGuaranteedMessage(0x19, 0);
                    np = NetPlayerFromCar(local_58);
                    DCE_I(msg, 0x1c) = DCE_I(np, 0x24);
                    if (NetPlayerFromCar(local_5c) == 0) {
                        DCE_I(msg, 0x20) = -2;
                    } else {
                        np = NetPlayerFromCar(local_5c);
                        DCE_I(msg, 0x20) = DCE_I(np, 0x24);
                    }
                    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, msg, 0);
                }
            }
            DCE_I(local_58, 0x13e4) = time;
            DCE_P(local_58, 0x13ec) = local_5c;
            if (bVar9 == 0) {
                DCE_I(local_58, 0x13e8) = time;
                DCE_P(local_58, 0x13f0) = local_5c;
            }
            mat = DCE_V(local_58, 0x8);
            if (ABS(*(float*)(mat + 0x74)) > 4.0f || ABS(*(float*)(mat + 0x78)) > 6.0f
                || ABS(*(float*)(mat + 0x7c)) > 4.0f) {
                local_4c = local_4c * 2;
            }
            if (DCE_I(pCar1, 0x12e8) == 0) {
                BrMatrix34ApplyP(&vec1, (br_vector3*)(DCE_V(pCar1, 0x8) + 0x14), (br_matrix34*)(DCE_V(pCar1, 0x8) + 0x8c));
                vec1.v[1] = vec1.v[1] - -0.15f;
            }
            if (local_54 != 0 && DCE_I(local_54, 0x12e8) == 0) {
                BrMatrix34ApplyP(&vec2, (br_vector3*)(DCE_V(local_54, 0x8) + 0x14), (br_matrix34*)(DCE_V(local_54, 0x8) + 0x8c));
                vec2.v[1] = vec2.v[1] - -0.15f;
            }
            dmgv = (int)((0.7 / DCE_F(local_58, 0x18d0)) * (double)dsum);
            if (DCE_P(local_58, 0x1cc) == 0 && gNet_mode == 0) {
                mn = (local_4c > 8) ? 8 : local_4c;
                fdmg = (float)dmgv;
                c2 = (int)((gCredits_per_unit_car_damage[gCrash_earnings_index] * fdmg * (float)mn - -50.0f) * 0.01f) * 100;
                if (c2 != 0 || DCE_I(local_58, 0x1d4) != 0) {
                    if (DCE_I(local_58, 0x1d4) == 0) {
                        credits = c2;
                        if (credits >= 2000) {
                            credits = 2000;
                        }
                        EarnCredits(credits);
                        gLast_credit_award_time = time;
                        tt = (int)((gSeconds_per_unit_car_damage[gCrash_earnings_index] * fdmg - -2.5) * 0.2) * 5;
                        if (tt >= 90) {
                            tt = 90;
                        }
                        AwardTime(tt);
                        if (local_54 != 0) {
                            if (flag3c != 0) {
                                DoFancyHeadup(0x13);
                            } else if (local_4c > 2) {
                                DoFancyHeadup(3);
                            } else if (local_4c > 1) {
                                DoFancyHeadup(2);
                            }
                        }
                    }
                    for (k = 0; k < 12; k++) {
                        DCE_I(local_58, 0x4e4 + 0x2c * k) = DCE_I(local_58, 0x4e0 + 0x2c * k);
                    }
                }
            }
        }
    }

    DCE_I(pCar1, 0x133c) = 0;
    if (local_54 != 0) {
        DCE_I(local_54, 0x133c) = 0;
    }

L_441239:
    if (uVar12 != 0) {
        return 1;
    }
    if (local_54 == 0) {
        return 0;
    }
    if (bVar5 == 0) {
        return 0;
    }
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// PARTIAL 95.35%: PercentageCarHealth 0x00441350 [regalloc: the 4-term int sum is scheduled D,C,A,B instead of A,B,C,D; body identical]
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
