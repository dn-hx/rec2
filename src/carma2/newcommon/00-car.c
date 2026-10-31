#include "00-car.h"

#include "15-displays.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"

#include "rec2_macros.h"

#include "c2_stdio.h"
#include "59-camera.h"
#include "10-loading2.h"
#include "49-physics.h"
#include "30-opponent.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

extern int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);
extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index);

// GLOBAL: CARMA2_HW 0x00763500
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
float gArmour_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761f5c
// GLOBAL: CARMA2_SW 0x007584bc
int gExplosion_sound_id;

// GLOBAL: CARMA2_HW 0x00761f60
float gPower_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761d40
float gOffensive_starting_value[100];

// GLOBAL: CARMA2_HW 0x0075ba20
tFloat_bunch_info gCar_softness;

// GLOBAL: CARMA2_HW 0x0074d600
tFloat_bunch_info gCar_car_damage_multiplier;

// GLOBAL: CARMA2_HW 0x0068b8d0
br_vector3 gZero_v__car;  // FIXME: make const?

// GLOBAL: CARMA2_HW 0x006793dc
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x0065d010
int gFace_num__car = 1;

// FUNCTION: CARMA2_HW 0x00413cc0
// FUNCTION: CARMA2_SW 0x00413cf0
void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount) {
    tDamage_unit* the_damage;

    if (pDamage_amount <= 0) {
        return;
    }
    the_damage = &pCar->damage_units[pUnit_type];
    the_damage->damage_level += pDamage_amount;
    if (the_damage->damage_level >= 100) {
        the_damage->damage_level = 99;
    }
}
// GLOBAL: CARMA2_HW 0x00679364
int gInTheSea;

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

// GLOBAL: CARMA2_HW 0x006793ac
tU32 gQuite_wild_end;

// GLOBAL: CARMA2_HW 0x006793b0
tU32 gQuite_wild_start;

// GLOBAL: CARMA2_HW 0x006793b4
tU32 gOn_me_wheels_start;

// GLOBAL: CARMA2_HW 0x006793b8
tU32 gWoz_upside_down_at_all;

// GLOBAL: CARMA2_HW 0x006793a8
tU32 gWild_start;

// GLOBAL: CARMA2_HW 0x0074c9ec
int gNum_active_cars;

// GLOBAL: CARMA2_HW 0x0074c7c0
tCar_spec* gActive_car_list[139]; /* FIXME: uncertain about array length */

// GLOBAL: CARMA2_HW 0x0079ef2c
int gNum_cars_and_non_cars;

// GLOBAL: CARMA2_HW 0x006940b0
int gFace_count;

// GLOBAL: CARMA2_HW 0x00744820
tFace_ref gFace_list__car[300];

// GLOBAL: CARMA2_HW 0x00679360
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
tPhysics_object* gUnknown_car_collision_info;

// GLOBAL: CARMA2_HW 0x0067939c
int gTesting_car_for_sensible_place;

extern int gCrush_deferred;


// FUNCTION: CARMA2_HW 0x00413f10
intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData) {

    if (pActor->user != NULL) {
        int* index = pData;
        tUser_crush_data* crush_data = pActor->user;
        br_model* model = crush_data->models[*index];

        if (model != NULL) {
            pActor->model = model;
            pActor->type = BR_ACTOR_MODEL;
            return 0;
        }
    }
    pActor->model = NULL;
    pActor->type = BR_ACTOR_NONE;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00413f40
void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &pIndex);
    pCar->field_0xe18 = pIndex;
}

// InitialiseCar2

// InitialiseCar

// FUNCTION: CARMA2_HW 0x00414410
void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race) {
    int i;
    int cat;
    int car_count;
    tCar_spec* car;
    br_bounds bnds;

    gProgram_state.initial_position = pThe_race->initial_position;
    gProgram_state.initial_yaw = pThe_race->initial_yaw;
    BrActorToBounds(&bnds, gProgram_state.track_spec.the_actor);
    gMin_world_y = bnds.min.v[1];
    gNum_active_non_cars = 0;
    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            PossibleService();
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                InitialiseCar2(car, 1);
            }
        }
    }
    gCamera_yaw = 0;
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Manual) {
        gCamera_type = 0;
        gAction_replay_camera_mode = kActionReplayCameraMode_Standard;
    }
    InitialiseExternalCamera();
    if (gUnknown_car_collision_info != NULL && gProgram_state.current_car.collision_info != gUnknown_car_collision_info->parent) {
        AddCollisionInfoChild(gProgram_state.current_car.collision_info, gUnknown_car_collision_info);
    }
}

// SetInitialPosition

// FUNCTION: CARMA2_HW 0x004148d0
void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, number_of_racers, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, opponent_list, 0xce4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpp_spec, ranking, 0x8);
    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);

    for (i = 0; i < pThe_race->number_of_racers; i++) {
        int grid_index = pThe_race->opponent_list[i].ranking;

        if (grid_index >= 0) {
            grid_index = i;
        }
        SetInitialPosition(pThe_race, i, grid_index);
    }
}

// InitialiseNonCar

// NewFaceListCallBack

// FUNCTION: CARMA2_HW 0x00414ca0
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gInTheSea;
}
// RememberSafePosition

// ControlNetCars

// based on carpocalypse2 src/carma2/common/car.c:1126 and :1178
typedef void (C2_HOOK_FASTCALL * tControl_car_fn)(tCar_spec* pCar_spec, float pT);

// GLOBAL: CARMA2_HW 0x0058f660
tControl_car_fn ControlCar[6] = {
    NULL,
    NULL,
    NULL,
    (tControl_car_fn)ControlCar3,
    NULL,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0058f678
int gControl__car = 3;

// GLOBAL: CARMA2_HW 0x006793ec
tU32 gSafe_position_time_count = 0;

extern void C2_HOOK_FASTCALL FlyCar(tCar_spec* c, br_scalar dt);

static void C2_HOOK_FASTCALL RememberSafePosition(tCar_spec* car, tU32 pTime_difference) {
    int i;
    br_vector3 r;

    gSafe_position_time_count += pTime_difference;
    if (gSafe_position_time_count < 5000) {
        return;
    }
    gSafe_position_time_count = 4000;
    if (car->disabled) {
        return;
    }
    if (car->car_crush_spec != NULL && car->car_crush_spec->field_0x4b8) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (car->oldd[i] >= car->susp_height[i >> 1]) {
            return;
        }
    }
    if ((car->collision_info->last_special_volume == NULL || car->collision_info->last_special_volume->gravity_multiplier == 1.0)
        && gCurrent_race.material_modifiers[car->material_index[0]].tyre_road_friction >= 0.1
        && gCurrent_race.material_modifiers[car->material_index[1]].tyre_road_friction >= 0.1
        && gCurrent_race.material_modifiers[car->material_index[2]].tyre_road_friction >= 0.1
        && gCurrent_race.material_modifiers[car->material_index[3]].tyre_road_friction >= 0.1
        && !car->field_0x195c
        && car->car_master_actor->t.t.mat.m[1][1] >= 0.8f) {

        for (i = 0; i < 4; i++) {
            BrVector3Sub(&r, &car->car_master_actor->t.t.translate.t, (br_vector3*)car->last_safe_positions[i].m[3]);

            if (BrVector3LengthSquared(&r) < 8.4015961f) {
                return;
            }
        }
        for (i = 18; i > 0; i--) {
            BrMatrix34Copy(&car->last_safe_positions[i], &car->last_safe_positions[i - 1]);
        }
        BrMatrix34Copy(&car->last_safe_positions[0], &car->car_master_actor->t.t.mat);
        gSafe_position_time_count = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00414cb0
void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference) {
    // GLOBAL: CARMA2_HW 0x0058f6d8
    static int last_key_down = 1;
    tCar_spec* car;
    tU32 time;
    br_vector3 minus_k;
    br_vector3 tmp;
    br_vector3 delta_omega;

    car = &gProgram_state.current_car;

    if (car->keys.change_down) {
        if (!last_key_down) {
            last_key_down = 1;

            if (car->number_of_wheels_on_ground <= 2
                    && BrVector3LengthSquared(&car->collision_info->v) < .04
                    && car->oldd[0] == car->susp_height[0]
                    && car->oldd[1] == car->susp_height[0]
                    && (car->frame_collision_flag || car->collision_info->disable_move_rotate)) {

                tmp.v[0] = -car->collision_info->transform_matrix.m[1][2];
                *(int*)&tmp.v[1] = 0;
                tmp.v[2] = car->collision_info->transform_matrix.m[1][0];
                BrVector3Normalise(&tmp, &tmp);
                BrMatrix34TApplyV(&delta_omega, &tmp, &car->collision_info->transform_matrix);
                BrVector3Accumulate(&car->collision_info->omega, &delta_omega);
                car->collision_info->disable_move_rotate = 0;
            }
        }
    } else {
        last_key_down = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {

            if (i != gThis_net_player_index) {
                ControlCar[gControl__car](gNet_players[i].car, 0.001f * pTime_difference);
            }
        }
    }
    if (gCar_flying) {
        FlyCar(gCar_to_view, 0.001f * pTime_difference);
        return;
    }

    time = GetTotalTime();
    if (car->damage_units[eDamage_steering].damage_level > 40) {
        if (car->end_steering_damage_effect != 0) {
            if (car->end_steering_damage_effect > time || car->damage_units[eDamage_steering].damage_level == 99) {
                car->keys.left = car->false_key_left;
                car->keys.right = car->false_key_right;
            } else {
                car->end_steering_damage_effect = 0;
            }
        } else {
            if (PercentageChance((int)(pTime_difference * (car->damage_units[eDamage_steering].damage_level - 40) * 0.0045)) && fabs(car->collision_info->velocity_car_space.v[2]) > 1. / (10000. * 6.9f)) {
                if (car->keys.left || car->keys.right) {
                    car->false_key_left = !car->keys.left;
                    car->false_key_right = !car->keys.right;
                } else {
                    if (PercentageChance(50)) {
                        car->false_key_left = 1;
                    } else {
                        car->false_key_right = 1;
                    }
                }
                car->end_steering_damage_effect = (tU32)(time + FRandomBetween(0.0f, (float)(25 * (car->damage_units[eDamage_steering].damage_level - 40))));
            }
        }
    }
    if (car->damage_units[eDamage_transmission].damage_level > 40) {
        if (car->end_trans_damage_effect != 0) {
            if (car->end_trans_damage_effect > time || car->damage_units[eDamage_transmission].damage_level == 99) {
                car->gear = 0;
                car->just_changed_gear = 1;
            } else {
                car->end_trans_damage_effect = 0;
            }
        } else {
            if (PercentageChance((int)(pTime_difference * (car->damage_units[eDamage_transmission].damage_level - 40) * 0.006))) {
                car->end_trans_damage_effect = (tU32)(time + FRandomBetween(0.f, (float)(50 * (car->damage_units[eDamage_transmission].damage_level - 40))));
            }
        }
    }
    ControlCar[gControl__car](car, 0.001f * pTime_difference);
    RememberSafePosition(car, pTime_difference);
    if (gCamera_reset) {
        BrVector3SetFloat(&minus_k, 0.0f, 0.0f, -1.0f);
        gCamera_sign = 0;
        BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
    }
}

// CalcEngineForce

// PrepareCars

// CalcGraphicalWheelStuff

extern tU32 gPHIL_last_physics_tick;
extern void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime);

// FUNCTION: CARMA2_HW 0x00416500
void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime) {
    int i;
    br_vector3 tv;
    br_scalar dt;
    br_vector3 minus_k;

    if (gCar_flying) {
        BrMatrix34Copy(&gCar_to_view->collision_info->actor->t.t.mat, &gCar_to_view->collision_info->transform_matrix);
    }
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;
        float original_speed;

        car = gActive_car_list[i];
        BrMatrix34ApplyP(&car->pos, &car->collision_info->cmpos, &car->car_master_actor->t.t.mat);
        original_speed = car->speed;
        car->speed = BR_LENGTH2(car->collision_info->v.v[0], car->collision_info->v.v[2]) / 1000.f;
        BrVector3Negate(&minus_k, (br_vector3*)&car->car_master_actor->t.t.mat.m[2]);
        if (original_speed < 0.0001f && car->speed > 0.0001f) {
            car->speed = .0001f;
        }
        if (car->speed > .0001f) {
            BrVector3Normalise(&car->direction, &car->collision_info->v);
        } else {
            if (BrVector3Dot(&car->direction, &minus_k) < 0.f) {
                BrVector3SetFloat(&minus_k, 0.f, 0.f, 1.f);
            } else {
                BrVector3SetFloat(&minus_k, 0.f, 0.f, -1.f);
            }
            BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
        }
        if (car != NULL && car->driver > 5) {
            int wheel;

            car->speedo_speed = BrVector3Dot(&minus_k, &car->collision_info->v) / 1000.f;
            CalcGraphicalWheelStuff(car);

            for (wheel = 0; wheel < 4; wheel++) {
                if (car->oldd[wheel] < car->susp_height[wheel >> 1]
                        && gCurrent_race.material_modifiers[car->material_index[wheel]].smoke_type >= 2
                        && !car->collision_info->disable_move_rotate) {
                    GenerateContinuousSmoke(car, wheel, pTime);
                }
            }
        }
    }
    if (gPHIL_last_physics_tick > pLast_frame_time && gCar_to_view->speed > .0001f) {
        dt = (gPHIL_last_physics_tick - pLast_frame_time) / 40.f;
        BrVector3Sub(&tv, &gCar_to_view_original_v, &gCar_to_view->collision_info->v);
        BrVector3Scale(&tv, &tv, dt);
        BrVector3Accumulate(&tv, &gCar_to_view->collision_info->v);
        BrVector3Normalise(&gCar_to_view->direction, &tv);
    }
}

// GetNonCars

// GetNetPos

// MungeCarsMass

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00415ef0
void C2_HOOK_FASTCALL AddDrag(tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDt) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00416030
void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pChild) {

    while (pChild != NULL) {
        AddDrag(pCar, pChild, 0.04f);
        if (pChild->child != NULL) {
            DragChildren(pCar, pChild->child);
        }
        pChild = pChild->next;
    }
}

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// APTCPreCollision

// SetCollisionFlagsAndStuff

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

// ControlCar2

// FUNCTION: CARMA2_HW 0x004173b0
// FUNCTION: CARMA2_SW 0x004173e0
void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* pCar, void* pUnused, br_scalar pDt) {
    br_scalar time_steps;

    (void)pUnused;
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f || pCar->collision_info->omega.v[1] < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f || pCar->collision_info->omega.v[1] > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// ControlCar4

// ControlCar5

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

// FUNCTION: CARMA2_HW 0x00418230
// FUNCTION: CARMA2_SW 0x00418260
void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, flags, 0x100);

    if (gNet_mode != eNet_mode_client) {
        pNon_car->flags &= ~0xff;
        pNon_car->flags |= 0x1;
    }
}

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

extern void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt);
extern int C2_HOOK_FASTCALL ShapeRayCast(const br_vector3* p1, const br_vector3* p2, const tPhysics_shape* pShape, br_vector3* pPos, float* pFactor, br_vector3* pNormal);
extern void C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFl, br_scalar* pFr, br_scalar* pRl, br_scalar* pRr);
extern int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag);
extern int gKey_mapping[77];
extern int gUNK_006793d0;
extern void C2_HOOK_FASTCALL SkidNoise(tCar_spec* pCar, int pSide, br_scalar pV, int pMaterial_index);
extern tCar_spec* gLast_car_to_skid[2];
extern int gSkid_tag[2];

// GLOBAL: CARMA2_HW 0x0074cf98
int gUNK_0074cf98;

// GLOBAL: CARMA2_HW 0x0079ecc0
tPhysics_object* gDrivable_on_list[50];

// GLOBAL: CARMA2_HW 0x0079ed88
tU32 gDrivable_on_count;

#define gFriction_materials gCurrent_race.material_modifiers

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004f1140
float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8) { NOT_IMPLEMENTED(); return 0.f; }
// STUB: CARMA2_HW 0x004ecfb0
void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) { NOT_IMPLEMENTED(); }
// FUNCTION: CARMA2_HW 0x0041bd40
void C2_HOOK_FASTCALL ConditionallyNoteSkid(tCar_spec* pCar, tFace_ref** faces, int pIndex) {
    br_vector3 skid_pos;
    br_scalar d;

    if (faces[pIndex] == NULL) {
        return;
    }
    if (faces[pIndex]->material != NULL
            && faces[pIndex]->material->colour_map != NULL
            && faces[pIndex]->material->colour_map->type == BR_PMT_RGBA_4444) {
        return;
    }
    if (faces[pIndex]->material != NULL
            && faces[pIndex]->material->identifier != NULL
            && strlen(faces[pIndex]->material->identifier) == 0xb
            && faces[pIndex]->material->identifier[5] == '|') {
        return;
    }
    if ((pCar->new_skidding & (1 << pIndex)) != 0) {
        return;
    }
    pCar->new_skidding |= 1 << pIndex;
    if ((pIndex & 1) != 0) {
        skid_pos.v[0] = pCar->wpos[pIndex].v[0] - 0.1725f;
    } else {
        skid_pos.v[0] = pCar->wpos[pIndex].v[0] - -0.1725f;
    }
    skid_pos.v[0] = skid_pos.v[0] * 0.14492753f;
    skid_pos.v[1] = (pCar->wpos[pIndex].v[1] - pCar->oldd[pIndex]) * 0.14492753f;
    skid_pos.v[2] = pCar->wpos[pIndex].v[2] * 0.14492753f;
    BrMatrix34ApplyP(&pCar->special_start[pIndex], &skid_pos, &pCar->collision_info->transform_matrix);
    d = BrVector3Dot(&faces[pIndex]->v[0], &faces[pIndex]->normal)
        - BrVector3Dot(&pCar->special_start[pIndex], &faces[pIndex]->normal);
    pCar->special_start[pIndex].v[0] = faces[pIndex]->normal.v[0] * d + pCar->special_start[pIndex].v[0];
    pCar->special_start[pIndex].v[1] = faces[pIndex]->normal.v[1] * d + pCar->special_start[pIndex].v[1];
    pCar->special_start[pIndex].v[2] = faces[pIndex]->normal.v[2] * d + pCar->special_start[pIndex].v[2];
}
// STUB: SetJoystickFFBForce
int C2_HOOK_FASTCALL SetJoystickFFBForce(int pMagnitude) { NOT_IMPLEMENTED(); return 0; }
// STUB: SetJoystickFFBSteeringForce
int C2_HOOK_FASTCALL SetJoystickFFBSteeringForce(int pMagnitude) { NOT_IMPLEMENTED(); return 0; }
// STUB: PhysicsObjectApplyTorque
void C2_HOOK_FASTCALL PhysicsObjectApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) { NOT_IMPLEMENTED(); }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static br_scalar stop_timer;
static br_scalar slide_dist;

// based on carpocalypse2 src/carma2/common/car.c (GPL-3.0)
// FUNCTION: CARMA2_HW 0x00418850
void C2_HOOK_FASTCALL CalcForce(tCar_spec* pCar, br_scalar pDt) {
    int i;
    int j;
    int normnum;
    int vol_mod;
    int count;
    int best_index;
    int ffb_force;
    br_scalar force[4];
    br_scalar d[4];
    tFace_ref* collected[4];
    br_scalar dd[4];
    br_scalar rt[4];
    br_scalar val[4];
    br_scalar best;
    br_scalar wheelratio;
    br_scalar k;
    br_scalar ts;
    br_scalar ts2;
    br_scalar ts3;
    br_scalar ts4;
    br_scalar maxfl;
    br_scalar maxfr;
    br_scalar friction_number;
    br_scalar deltaomega;
    br_scalar v98;
    br_scalar v99;
    br_scalar v87;
    br_scalar v106;
    br_scalar v108;
    br_scalar v109;
    br_scalar v116;
    br_scalar v125;
    br_scalar v128;
    br_scalar v129;
    br_scalar v134;
    br_scalar v135;
    br_scalar pV;
    br_scalar fl_oil_factor;
    br_scalar fr_oil_factor;
    br_scalar rl_oil_factor;
    br_scalar rr_oil_factor;
    int delta;
    br_vector3 b;
    br_vector3 f;
    br_vector3 B;
    br_vector3 road_normal;
    br_vector3 v;
    br_vector3 tv;
    br_vector3 tmp;
    br_vector3 ray_dir;
    br_vector3 normal;
    br_vector3 nor2;
    br_vector3 vplane;
    br_vector3 rightplane;
    br_vector3 a;
    br_vector3 v103;
    br_vector3 v136;
    br_vector3 v123;
    br_vector3 norm[4];
    br_vector3 ray_pos[4];
    br_vector3 contact[4];
    br_vector3 wheel_pos[4];
    br_bounds bounds;
    tFace_ref* faces[4];
    tPhysics_object* objs[4];
    tPhysics_object* pObj;
    tPhysics_object** pList;
    tFace_ref* pFace;
    br_matrix34* mat;
    tSpecial_volume* vol;
    tMaterial_modifiers* mat_list;
    br_scalar* pd;

    B.v[0] = 0.f;
    B.v[1] = 0.f;
    B.v[2] = 0.f;
    pCar->curvature = pCar->curvature + pCar->field_0x1260;
    pCar->field_0x195c = 0;
    vol = pCar->collision_info->last_special_volume;
    wheelratio = (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2])
        / (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]);
    faces[0] = NULL;
    faces[1] = NULL;
    faces[2] = NULL;
    faces[3] = NULL;
    normnum = 0;
    objs[0] = NULL;
    objs[1] = NULL;
    objs[2] = NULL;
    objs[3] = NULL;
    vol_mod = 0;
    f.v[0] = 0.f;
    f.v[1] = 0.f;
    f.v[2] = 0.f;
    mat = &pCar->car_master_actor->t.t.mat;
    b.v[0] = -mat->m[1][0];
    b.v[1] = -mat->m[1][1];
    b.v[2] = -mat->m[1][2];
    v.v[0] = pCar->collision_info->v.v[0] * 6.9;
    v.v[1] = pCar->collision_info->v.v[1] * 6.9;
    v.v[2] = pCar->collision_info->v.v[2] * 6.9;
    road_normal.v[0] = 0.f;
    road_normal.v[1] = 0.f;
    road_normal.v[2] = 0.f;
    for (i = 0; i < 4; i++) {
        BrMatrix34ApplyP(&wheel_pos[i], &pCar->wpos[i], mat);
    }
    k = (pCar->susp_height[0] < pCar->susp_height[1] ? pCar->susp_height[0] : pCar->susp_height[1]) - (-0.5);
    d[0] = 2.f;
    d[1] = 2.f;
    BrVector3Scale(&tmp, &b, k);
    d[2] = 2.f;
    d[3] = 2.f;
    for (i = 0; i < 4; i++) {
        BrVector3InvScale(&ray_pos[i], &wheel_pos[i], WORLD_SCALE);
    }
    BrVector3InvScale(&ray_dir, &tmp, WORLD_SCALE);
    for (i = pCar->collision_info->box_face_start, pFace = &gFace_list__car[i];
            i < pCar->collision_info->box_face_end;
            i++, pFace++) {
        if (gUNK_006793d0 != 0 && (pFace->flags & 0x80) != 0) {
            continue;
        }
        MultiRayCheckSingleFace(4, pFace, ray_pos, &ray_dir, &normal, rt);
        for (j = 0; j < 4; j++) {
            if (rt[j] < d[j]) {
                faces[j] = pFace;
                d[j] = rt[j];
                BrVector3Copy(&pCar->nor[j], &normal);
            }
        }
    }
    bounds.max = ray_pos[0];
    bounds.min = ray_pos[0];
    for (i = 1; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            if (ray_pos[i].v[j] < bounds.min.v[j]) {
                bounds.min.v[j] = ray_pos[i].v[j];
            }
            if (bounds.max.v[j] < ray_pos[i].v[j]) {
                bounds.max.v[j] = ray_pos[i].v[j];
            }
        }
    }
    for (j = 0; j < 3; j++) {
        if (ray_dir.v[j] < 0.f) {
            bounds.min.v[j] = bounds.min.v[j] + ray_dir.v[j];
        } else {
            bounds.max.v[j] = bounds.max.v[j] + ray_dir.v[j];
        }
    }
    for (i = 0, pList = gDrivable_on_list; i < (int)gDrivable_on_count; i++, pList++) {
        pObj = *pList;
        if (!BoundsOverlapTest__finteray(&bounds, &pObj->field_0xf4)) {
            continue;
        }
        BrMatrix34TApplyV(&ray_dir, &ray_dir, &pObj->transform_matrix);
        if (pObj->shape == NULL) {
            continue;
        }
        for (;;) {
            BrMatrix34ApplyP(&pObj->cmpos, &pObj->cmpos, &pObj->transform_matrix);
            for (j = 0; j < 4; j++) {
                DrMatrix34ApplyLPInverse(&tmp, &ray_pos[j], &pObj->transform_matrix);
                ShapeRayCast(&tmp, &ray_dir, (const tPhysics_shape*)pObj->shape, &pObj->cmpos, &ts, &normal);
                if (ts < d[j]) {
                    normal.v[0] = -normal.v[0];
                    normal.v[1] = -normal.v[1];
                    normal.v[2] = -normal.v[2];
                    BrMatrix34ApplyV(&normal, &normal, &pObj->transform_matrix);
                    d[j] = ts;
                    faces[j] = NULL;
                    objs[j] = pObj;
                }
            }
            if (pObj->next == NULL) {
                break;
            }
            pObj = (tPhysics_object*)pObj->next;
        }
    }
    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->material_modifier_index != 0) {
        vol_mod = pCar->collision_info->last_special_volume->material_modifier_index;
    }
    mat_list = gFriction_materials;
    for (i = 0, j = 0; j < 0x30; i++, j += 0xc) {
        pd = &d[i];
        *pd = *pd * k;
        ts = *pd;
        if (faces[i] != NULL
                && faces[i]->material != NULL
                && faces[i]->material->identifier != NULL) {
            pCar->material_index[i] = vol_mod != 0
                ? vol_mod
                : (signed char)faces[i]->material->identifier[0] - '/';
            if (pCar->material_index[i] < 0 || pCar->material_index[i] > 0xb) {
                pCar->material_index[i] = 0;
            }
            if (strlen(faces[i]->material->identifier) == 0xb
                    && faces[i]->material->identifier[5] == '|') {
                pCar->oil_remaining[i] = 0.f;
            }
        } else {
            pCar->material_index[i] = 0;
        }
        BrMatrix34TApplyV(&norm[i], &pCar->nor[i], mat);
        if (mat_list[pCar->material_index[i]].bumpiness != 0.f) {
            br_scalar tv0 = pCar->nor[i].v[0] * *pd;
            br_scalar tv2 = pCar->nor[i].v[2] * *pd;
            {
                int x = abs((int)(512.f * (tv0 + wheel_pos[i].v[0])));
                int y = abs((int)(512.f * (wheel_pos[i].v[2] + tv2)));
                x = x % 2048;
                y = y % 2048;
                if (x > 1024) {
                    x = 2048 - x;
                }
                if (y > 1024) {
                    y = 2048 - y;
                }
                if (x + y > 1024) {
                    delta = 2048 - x - y;
                } else {
                    delta = x + y;
                }
                delta = delta - 400;
                if (delta < 0) {
                    delta = 0;
                }
            }
            d[i] = delta * mat_list[pCar->material_index[i]].bumpiness / 42400.f
                * norm[i].v[1] + *pd;
        }
        if (*pd < -0.5f
                || pCar->wheel_dam_offset[i ^ 2] * 6.9 + pCar->susp_height[i / 2] < *pd) {
            force[i] = 0.f;
            d[i] = pCar->susp_height[i / 2];
        } else {
            BrVector3Accumulate(&road_normal, &norm[i]);
            normnum++;
            d[i] = d[i] - pCar->wheel_dam_offset[i ^ 2] * 6.9;
            force[i] = (pCar->susp_height[i / 2] - d[i]) * pCar->sk[i / 2];
            force[i] = force[i] - (d[i] - pCar->oldd[i]) / pDt * pCar->sb[i / 2];
            if (pCar->susp_height[i / 2] == pCar->oldd[i]
                && pCar->nor[i].v[0] * v.v[0] + pCar->nor[i].v[1] * v.v[1] + pCar->nor[i].v[2] * v.v[2] > -0.01f
                && pCar->collision_info->M * 5.f < force[i]) {
                d[i] = pCar->susp_height[i / 2];
                force[i] = pCar->collision_info->M * 5.f;
            }
            if (force[i] < 0.f) {
                force[i] = 0.f;
            }
            B.v[1] = force[i] + B.v[1];
            f.v[0] = f.v[0] - (pCar->wpos[i].v[2] - pCar->centre_of_mass_world_scale.v[2]) * force[i];
            f.v[2] = (pCar->wpos[i].v[0] - pCar->centre_of_mass_world_scale.v[0]) * force[i] + f.v[2];
        }
        dd[i] = pCar->oldd[i] - d[i];
        pCar->oldd[i] = d[i];
        if (objs[i] == NULL || force[i] == 0.f) {
            continue;
        }
        pCar->field_0x195c = (unsigned int)objs[i];
        if (objs[i]->disable_move_rotate != 0
                && objs[i]->owner != NULL
                && objs[i]->flags_0x238 == 1
                && ((tNon_car_spec*)objs[i]->owner)->driver <= 5
                && (((tNon_car_spec*)objs[i]->owner)->flags & 0x10000) != 0) {
            continue;
        }
        objs[i]->disable_move_rotate = 0;
        ts = pCar->collision_info->M * pDt / (force[i] / 6.9);
        tmp.v[0] = ts * b.v[0];
        tmp.v[1] = ts * b.v[1];
        tmp.v[2] = ts * b.v[2];
        objs[i]->v.v[0] = tmp.v[0] + objs[i]->v.v[0];
        objs[i]->v.v[1] = tmp.v[1] + objs[i]->v.v[1];
        objs[i]->v.v[2] = tmp.v[2] + objs[i]->v.v[2];
        if (gUNK_006793d0 == 2) {
            pCar->collision_info->field_0x49c = (int)&gZero_v__car;
        }
    }
    if (objs[0] != NULL
            && objs[0] == objs[1]
            && objs[0] == objs[2]
            && objs[0] == objs[3]
            && objs[0]->owner != NULL
            && objs[0]->flags_0x238 == 1
            && ((tNon_car_spec*)objs[0]->owner)->driver <= 5
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0x10000) != 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0x40000) != 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0xff) == 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0xff00) == 0
            && ((tNon_car_spec*)objs[0]->owner)->field_0xf8 == 0
            && gUNK_006793d0 != 3) {
        ((tNon_car_spec*)objs[0]->owner)->flags = (((tNon_car_spec*)objs[0]->owner)->flags & 1) | 1;
    }
    if (pCar->collision_info->disable_move_rotate != 0 && normnum == 0) {
        goto steer_centre;
    }
    if (pCar->collision_info->disable_move_rotate != 0) {
        SetCollisionInfoDoNothingChain(pCar->collision_info, 0);
    }
    if (pCar == NULL || pCar->driver <= 5 || pCar->wall_climber_mode == 0
            || (road_normal.v[0] == 0.f && road_normal.v[1] == 0.f && road_normal.v[2] == 0.f)) {
        friction_number = pCar->collision_info->M * 10.f;
        if (vol != NULL) {
            friction_number = (1.f - vol->gravity_multiplier) * pCar->collision_info->water_depth_factor;
            if (pCar->underwater_ability != 0) {
                friction_number = friction_number * 0.6f;
            }
            friction_number = (1.f - friction_number) * pCar->collision_info->M;
        } else {
            friction_number = pCar->collision_info->M;
        }
        friction_number = friction_number * gGravity_multiplier * 10.f;
        B.v[0] = B.v[0] - mat->m[0][1] * friction_number;
        B.v[1] = B.v[1] - mat->m[1][1] * friction_number;
        B.v[2] = B.v[2] - mat->m[2][1] * friction_number;
    } else {
        BrVector3Normalise(&tmp, &road_normal);
        BrVector3Scale(&tmp, &tmp, -(pCar->collision_info->M * 10.0f));
        BrVector3Accumulate(&B, &tmp);
    }
steer_centre:
    if (pCar != NULL && pCar->driver >= 7) {
        if (pCar->curvature > pCar->maxcurve) {
            pCar->curvature = pCar->maxcurve;
        }
        if (pCar->curvature < -pCar->maxcurve) {
            pCar->curvature = -pCar->maxcurve;
        }
        if (!(pCar->keys.left || pCar->keys.right || pCar->keys.holdw)
            && (gUNK_0074cf98 != 0
                || (pCar->joystick.left <= 0 && pCar->joystick.right <= 0))
            && (pCar->oldd[2] < pCar->susp_height[1] || pCar->oldd[3] < pCar->susp_height[1])) {
            ts = -((pCar->collision_info->omega.v[2] * road_normal.v[2]
                        + pCar->collision_info->omega.v[1] * road_normal.v[1]
                        + pCar->collision_info->omega.v[0] * road_normal.v[0])
                * (pDt / (pCar->wpos[0].v[2] - pCar->wpos[2].v[2])));
            ts2 = -(pCar->curvature * pDt);
            if (fabs(ts) < fabs(ts2) || ts * ts2 < 0.f) {
                ts = ts2;
            }
            if (gKey_mapping[47] < 143 && gKey_mapping[48] < 143) {
                pCar->curvature += ts;
                if (pCar->curvature * ts > 0.f) {
                    pCar->curvature = 0.f;
                }
            }
            ffb_force = (int)(ts * 240000.f);
        } else {
            ffb_force = 0;
        }
        if (pCar->driver == eDriver_local_human) {
            SetJoystickFFBSteeringForce(ffb_force - (int)((dd[2] - dd[3]) * -30000.f));
        }
    }
    if (normnum != 0) {
        count = 0;
        best_index = -1;
        best = 0.f;
        for (i = 0; i < 4; i++) {
            pFace = faces[i];
            if (pFace == NULL || pFace->material == NULL
                    || pFace->material->identifier == NULL) {
                continue;
            }
            if (strlen(pFace->material->identifier) != 0xb
                    || pFace->material->identifier[5] != '|') {
                continue;
            }
            for (j = 0; j < count; j++) {
                if (collected[j] == pFace) {
                    break;
                }
            }
            if (j != count) {
                continue;
            }
            contact[count].v[0] = -pCar->oldd[i] * mat->m[1][0];
            contact[count].v[1] = -pCar->oldd[i] * mat->m[1][1];
            contact[count].v[2] = -pCar->oldd[i] * mat->m[1][2];
            val[count] = force[i] * 0.04f;
            contact[count].v[0] += wheel_pos[i].v[0];
            contact[count].v[1] += wheel_pos[i].v[1];
            contact[count].v[2] += wheel_pos[i].v[2];
            if (val[count] > best) {
                best = val[count];
                best_index = count;
            }
            collected[count] = pFace;
            count++;
        }
        if (best_index >= 0) {
            BrVector3InvScale(&contact[best_index], &contact[best_index], WORLD_SCALE);
            tv.v[0] = -val[best_index] * mat->m[1][0];
            tv.v[1] = -val[best_index] * mat->m[1][1];
            tv.v[2] = -val[best_index] * mat->m[1][2];
            SmashEnvironment(pCar->collision_info,
                (undefined4*)collected[best_index],
                val[best_index], &contact[best_index],
                &pCar->collision_info->v, &tv, 0, 0);
        }
        FlushSmashQueue(1);
        BrVector3NormaliseQuick(&road_normal, &road_normal);
        friction_number = road_normal.v[1] * mat->m[1][1]
            + road_normal.v[2] * mat->m[2][1]
            + road_normal.v[0] * mat->m[0][1];
        if (pCar->driver > 5 && pCar->wall_climber_mode != 0) {
            friction_number = 1.f;
        }
        friction_number = mat_list[pCar->material_index[0]].down_force * friction_number;
        if (friction_number > 0.f) {
            friction_number = fabs(pCar->collision_info->velocity_car_space.v[2])
                * pCar->collision_info->M * 10.0 * friction_number / pCar->downforce_to_weight;
            if (pCar->collision_info->M * 10.0 < friction_number) {
                friction_number = pCar->collision_info->M * 10.0;
            }
            if (pCar->number_of_wheels_on_ground == 4
                && pCar->oldd[2] == pCar->susp_height[1]
                && pCar->oldd[3] == pCar->susp_height[1]) {
                br_vector3 pos;
                br_vector3 delta;
                br_material* material;

                pos.v[0] = (mat->m[3][0] + mat->m[2][0] * pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                pos.v[1] = (mat->m[3][1] + mat->m[2][1] * pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                pos.v[2] = (mat->m[3][2] + mat->m[2][2] * pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                delta.v[0] = b.v[0] * (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                delta.v[1] = b.v[1] * (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                delta.v[2] = b.v[2] * (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                ActorFindFace(&pos, &delta, gTrack_actor, &nor2, &ts, &material, NULL);
                if (ts > 1.f) {
                    pCar->field_0x12a8 = 1;
                }
            }
            if (pCar->field_0x12a8 != 0
                && (pCar->oldd[2] < pCar->susp_height[1] || pCar->oldd[3] < pCar->susp_height[1])) {
                pCar->field_0x12a8 = 0;
            }
            if (pCar->field_0x12a8 != 0) {
                friction_number = friction_number
                    * ((pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2])
                        / (pCar->wpos[2].v[2] - pCar->wpos[0].v[2]));
                f.v[0] = f.v[0]
                    + (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) * friction_number;
            }
            B.v[1] = B.v[1] - friction_number;
        }
        ts2 = BrVector3Dot(&pCar->collision_info->velocity_car_space, &road_normal);
        vplane.v[0] = pCar->collision_info->velocity_car_space.v[0] - ts2 * road_normal.v[0];
        vplane.v[1] = pCar->collision_info->velocity_car_space.v[1] - ts2 * road_normal.v[1];
        vplane.v[2] = pCar->collision_info->velocity_car_space.v[2] - ts2 * road_normal.v[2];
        if (vplane.v[2] < 0.f) {
            ts = 1.f;
        } else {
            ts = -1.f;
        }
        ts2 = BrVector3Length(&vplane);
        deltaomega = ts2 * pCar->curvature * ts;
        deltaomega = deltaomega - BrVector3Dot(&pCar->collision_info->omega, &road_normal);
        BrVector3Set(&v103, road_normal.v[1], -road_normal.v[0], 0.f);
        BrVector3Normalise(&v103, &v103);
        friction_number = pCar->collision_info->I.v[1] / pDt * deltaomega;
        ts = friction_number / (pCar->wpos[2].v[2] - pCar->wpos[0].v[2]);
        v108 = ts;
        v109 = -ts;
        BrVector3Set(&rightplane, 0.f, road_normal.v[2], -road_normal.v[1]);
        BrVector3Normalise(&rightplane, &rightplane);
        v99 = pCar->acc_force;
        friction_number = BrVector3Dot(&rightplane, &vplane);
        v108 = BrVector3Dot(&v103, &vplane);
        ts2 = fabs(v108);
        friction_number = (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) * friction_number * fabs(pCar->curvature);
        if (pCar->curvature <= 0.f) {
            friction_number = v108 - friction_number;
        } else {
            friction_number = v108 + friction_number;
        }
        friction_number = -(pCar->collision_info->M / pDt * friction_number);
        friction_number = friction_number - BrVector3Dot(&B, &v103);
        friction_number = friction_number / (1.0 - wheelratio);
        v108 = friction_number + v108;
        v109 = -wheelratio * friction_number + v109;
        friction_number = (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * v108;
        v98 = friction_number * pCar->curvature;
        friction_number = BrVector3Dot(&pCar->collision_info->velocity_car_space, &rightplane) * pCar->collision_info->M / pDt;
        v129 = BrVector3Dot(&rightplane, &B) + friction_number;
        v128 = pCar->mu.v[0] * pCar->brake_force / (pCar->mu.v[1] / pCar->traction_multiplier + pCar->mu.v[0]);
        v125 = pCar->brake_force - v128;
        ts = (pCar->damage_units[7].damage_level + pCar->damage_units[6].damage_level) / 2;
        if (ts > 20.f) {
            v128 = (1.f - (ts - 20.f) / 80.f) * (1.f - (ts - 20.f) / 80.f) * v128;
        }
        ts = (pCar->damage_units[5].damage_level + pCar->damage_units[4].damage_level) / 2;
        if (ts > 20.f) {
            v125 = (1.f - (ts - 20.f) / 80.f) * (1.f - (ts - 20.f) / 80.f) * v125;
        }
        ts2 = (force[1] + force[0]) * pCar->rolling_resistance + v128;
        v106 = (force[2] + force[3]) * pCar->steerable_rolling_resistance + v125;
        v128 = pCar->wpos[0].v[2] - pCar->wpos[2].v[2];
        v128 = sqrt(v128 * v128 * pCar->curvature * pCar->curvature + 1.0);
        v106 = v106 / v128;
        v134 = v106 + ts2;
        if (fabs(v129) < fabs(v134)) {
            ts2 = v129 / v134 * ts2;
            v106 = v129 / v134 * v106;
        }
        if ((v106 + ts2) * v129 < 0.f) {
            ts2 = -ts2;
            v106 = -v106;
        }
        v129 = v129 - (ts2 + v106);
        v99 = v99 - ts2;
        if (pCar->keys.brake
                && pCar->damage_units[7].damage_level < 60
                && pCar->damage_units[6].damage_level < 60) {
            v99 = v99 - v129;
            pCar->gear = 0;
        }
        v99 = v99 / pCar->traction_multiplier;
        v135 = sqrt(v99 * v99 + v109 * v109) / 2.0;
        GetOilFrictionFactors(pCar, &fl_oil_factor, &fr_oil_factor, &rl_oil_factor, &rr_oil_factor);
        for (i = 0; i < 4; i++) {
            if (pCar->oil_remaining[i] != 0.f) {
                ConditionallyNoteSkid(pCar, faces, i);
            }
        }
        if (pCar->driver <= 5) {
            v116 = 1.f;
        } else {
            v116 = pCar->grip_multiplier;
        }
        BrVector3Sub(&a, &pCar->wpos[0], &pCar->centre_of_mass_world_scale);
        BrVector3Cross(&a, &pCar->collision_info->omega, &a);
        BrVector3Accumulate(&a, &pCar->collision_info->velocity_car_space);
        if (pCar != NULL && pCar->driver >= eDriver_net_human
            && (((pCar->keys.left || pCar->joystick.left > 0x8000) && pCar->curvature > 0.f && deltaomega > 0.1 && a.v[0] > 0.f)
                || ((pCar->keys.right || pCar->joystick.right > 0x8000) && pCar->curvature < 0.f && deltaomega < 0.1 && a.v[0] < 0.f))
            && ts > 0.f) {
            friction_number = pCar->mu.v[0];
        } else {
            friction_number = pCar->mu.v[2];
            ts2 = fabs(a.v[0]) / 10.f;
            if (ts2 > 1.f) {
                ts2 = 1.f;
            }
            friction_number = (pCar->mu.v[2] - pCar->mu.v[0]) * ts2 + friction_number;
        }
        maxfl = sqrt(force[0]) * friction_number * (rl_oil_factor * v116) * mat_list[pCar->material_index[0]].tyre_road_friction;
        maxfr = sqrt(force[1]) * friction_number * (rr_oil_factor * v116) * mat_list[pCar->material_index[1]].tyre_road_friction;
        pCar->max_force_rear = maxfr + maxfl;
        if (rl_oil_factor == 1.f && rr_oil_factor == 1.f && pCar->traction_control
                && v135 * 2.f > pCar->max_force_rear && pCar->acc_force > 0.f
                && (pCar == NULL || pCar->driver < eDriver_net_human
                    || (pCar->target_revs > 1000.f && pCar->gear > 0))) {
            ts2 = v99;
            if (v99 * v99 <= v135 * v135 * 4.f) {
                v87 = sqrt(v135 * v135 * 4.f - v99 * v99);
            } else {
                v87 = 0.f;
            }
            if (pCar->max_force_rear <= v87) {
                pCar->torque = -(pCar->revs * pCar->revs / 100000000.f) - 0.1f;
            } else {
                float v177 = sqrt(pCar->max_force_rear * pCar->max_force_rear - v87 * v87);
                ts3 = ts2 < 0.f ? -1.f : 1.f;
                ts4 = (ts2 - ts3 * v177) * 0.99f;
                if (fabs(ts2) > fabs(ts4)) {
                    ts2 = ts4;
                }
            }
            v99 = v99 - ts2;
            v135 = sqrt(v99 * v99 + v109 * v109) / 2.f;
        } else if (pCar != NULL && pCar->driver >= eDriver_net_human && pCar->gear > 0
                && pCar->revs > pCar->target_revs && !pCar->traction_control) {
            if (!pCar->keys.change_down) {
                pCar->traction_control = 1;
            }
            friction_number = 1.f - (pCar->revs - pCar->target_revs) / (float)(400 * pCar->gear);
            if (friction_number < 0.4f) {
                friction_number = 0.4f;
            }
            maxfl = friction_number * maxfl;
            maxfr = friction_number * maxfr;
        }
        if (fabs(v109) > maxfr + maxfl && maxfr + maxfl > 0.1f) {
            v106 = (maxfr + maxfl) / fabs(v109) * pDt;
            v109 = v106 * v109;
            v99 = pCar->traction_multiplier * v106 * v99;
        }
        v98 = v98 - v106;
        v108 = (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * pCar->curvature * v106 + v108;
        if (v135 > 0.0001f) {
            v109 = v109 / (v135 * 2.f);
            v99 = v99 / (v135 * 2.0);
        }
        v99 = pCar->traction_multiplier * v99;
        force[0] = v135;
        force[1] = v135;
        pCar->wheel_slip = 0;
        switch ((force[0] > maxfl) + 2 * (force[1] > maxfr)) {
        case 0:
            slide_dist = 0.f;
            break;
        case 1:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = v135 - force[0] + force[1];
            if (force[1] <= maxfr) {
                slide_dist = 0.f;
            } else {
                if (maxfr > 0.1f) {
                    pV = (force[1] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 1);
                    }
                    SkidNoise(pCar, 1, pV, pCar->material_index[1]);
                }
                force[1] = pCar->friction_slipping_reduction * maxfr;
                pCar->wheel_slip |= 2;
            }
            break;
        case 2:
            force[1] = pCar->friction_slipping_reduction * maxfr;
            force[0] = v135 - force[1] + force[0];
            if (force[0] <= maxfl) {
                slide_dist = 0.f;
            } else {
                if (maxfl > 0.1f) {
                    pV = (force[0] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 0);
                    }
                    SkidNoise(pCar, 0, pV, pCar->material_index[0]);
                }
                force[0] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 2;
            }
            break;
        case 3:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = pCar->friction_slipping_reduction * maxfr;
            pCar->wheel_slip |= 2;
            pV = (v135 * 2.0 - maxfl - maxfr) / (maxfr + maxfl);
            if (&gProgram_state.current_car == pCar) {
                ts2 = 20.f;
            } else {
                ts2 = 60.f;
            }
            if (ts2 <= pV) {
                if (maxfl > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 0);
                }
                if (maxfr > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 1);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1f) {
                    SkidNoise(pCar, 0, pV, pCar->material_index[0]);
                }
            } else {
                if (maxfr > 0.1f) {
                    SkidNoise(pCar, 1, pV, pCar->material_index[1]);
                }
            }
            break;
        }
        v135 = sqrt(v108 * v108 + v98 * v98) / 2.0;
        if (v135 > 0.0001f) {
            v108 = v108 / (v135 * 2.0);
            v98 = v98 / (v135 * 2.0);
        }
        maxfl = sqrt(force[2]) * pCar->mu.v[1] * (fl_oil_factor * v116) * mat_list[pCar->material_index[2]].tyre_road_friction;
        maxfr = sqrt(force[3]) * pCar->mu.v[1] * (fr_oil_factor * v116) * mat_list[pCar->material_index[3]].tyre_road_friction;
        if (pCar != NULL && pCar->driver == eDriver_local_human) {
            SetJoystickFFBForce((int)(225.0f * (maxfl * force[2] + maxfr * force[3]) / (1.0f + 750.0f * pCar->speed)));
        }
        pCar->max_force_front = maxfr + maxfl;
        force[2] = v135;
        force[3] = v135;
        switch ((v135 > maxfl) + 2 * (v135 > maxfr)) {
        case 1:
            force[2] = pCar->friction_slipping_reduction * maxfl;
            force[3] = v135 - force[2] + force[3];
            if (force[3] > maxfr) {
                if (maxfr > 0.1f) {
                    pV = (force[3] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 3);
                    }
                    SkidNoise(pCar, 3, pV, pCar->material_index[3]);
                }
                force[3] = pCar->friction_slipping_reduction * maxfr;
                pCar->wheel_slip |= 1;
            }
            break;
        case 2:
            force[3] = pCar->friction_slipping_reduction * maxfr;
            force[2] = v135 - force[3] + force[2];
            if (force[2] > maxfl) {
                if (maxfl > 0.1f) {
                    pV = (force[2] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 2);
                    }
                    SkidNoise(pCar, 2, pV, pCar->material_index[2]);
                }
                force[2] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 1;
            }
            break;
        case 3:
            force[2] = pCar->friction_slipping_reduction * maxfl;
            force[3] = pCar->friction_slipping_reduction * maxfr;
            pCar->wheel_slip |= 1;
            pV = (v135 * 2.0 - maxfl - maxfr) / (maxfr + maxfl);
            if (&gProgram_state.current_car == pCar) {
                ts2 = 20.f;
            } else {
                ts2 = 60.f;
            }
            if (ts2 <= pV) {
                if (maxfl > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 2);
                }
                if (maxfr > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 3);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1f) {
                    SkidNoise(pCar, 2, pV, pCar->material_index[2]);
                }
            } else {
                if (maxfr > 0.1f) {
                    SkidNoise(pCar, 3, pV, pCar->material_index[3]);
                }
            }
            break;
        }
        BrVector3Scale(&v136, &rightplane, v99);
        BrVector3Scale(&a, &v103, v109);
        BrVector3Accumulate(&v136, &a);
        BrVector3Scale(&v123, &rightplane, v98);
        BrVector3Scale(&a, &v103, v108);
        BrVector3Accumulate(&v123, &a);
        for (i = 0; i < 4; i++) {
            rightplane = pCar->wpos[i];
            rightplane.v[1] = rightplane.v[1] - pCar->oldd[i];
            BrVector3Sub(&rightplane, &rightplane, &pCar->centre_of_mass_world_scale);
            if (i < 2) {
                BrVector3Scale(&b, &v136, force[i]);
            } else {
                BrVector3Scale(&b, &v123, force[i]);
            }
            BrVector3Accumulate(&B, &b);
            BrVector3Cross(&a, &rightplane, &b);
            BrVector3Accumulate(&f, &a);
        }
    } else {
        pCar->max_force_front = 0.f;
        pCar->max_force_rear = 0.f;
        if (gLast_car_to_skid[0] == pCar) {
            DRS3StopSound(gSkid_tag[0]);
        }
        if (gLast_car_to_skid[1] == pCar) {
            DRS3StopSound(gSkid_tag[1]);
        }
    }
    pCar->number_of_wheels_on_ground = normnum;
    BrMatrix34ApplyV(&b, &B, mat);
    BrVector3Scale(&rightplane, &f, pDt);
    PhysicsObjectApplyTorque(pCar->collision_info, &rightplane);
    BrVector3Scale(&rightplane, &b, pDt / (pCar->collision_info->M * WORLD_SCALE));
    BrVector3Accumulate(&pCar->collision_info->v, &rightplane);
    if (pCar->speed < 0.0001f
        && ((!pCar->keys.acc && pCar->joystick.acc <= 0) || !pCar->gear)
        && !pCar->keys.dec
        && pCar->joystick.dec <= 0
        && pCar->bounce_rate == 0.f
        && BrVector3Length(&pCar->collision_info->omega) < 0.05f) {
        if (vol != NULL) {
            ts2 = pCar->driver > 5 && pCar->underwater_ability != 0
                ? 1.0 - (1.0 - vol->gravity_multiplier) * 0.6
                : vol->gravity_multiplier;
            friction_number = BrVector3Length(&b) / ts2 / gGravity_multiplier;
        } else {
            friction_number = BrVector3Length(&b);
        }
        if (pCar->collision_info->M > friction_number || (pCar->keys.brake && normnum >= 3)) {
            if (stop_timer == 100.f) {
                stop_timer = 0.f;
            }
            if (stop_timer > 0.5f) {
                BrVector3SetFloat(&pCar->collision_info->v, 0.f, 0.f, 0.f);
                BrVector3SetFloat(&pCar->collision_info->omega, 0.f, 0.f, 0.f);
                stop_timer = 0.5f;
            }
        }
    }
    stop_timer = pDt + stop_timer;
    if (stop_timer > 1.f) {
        stop_timer = 100.f;
    }
    AddDrag(pCar, (tPhysics_object*)pCar->collision_info, pDt);
    if (pCar->driver >= eDriver_net_human) {
        pCar->acc_force = -(v136.v[2] * force[0]) - v136.v[2] * force[1];
    }
    pCar->curvature = pCar->curvature - pCar->field_0x1260;
}


// DoRevs

// ScrapeNoise

// GLOBAL: CARMA2_HW 0x006793c0
int gSkid_tag[2];

// GLOBAL: CARMA2_HW 0x006793c8
tCar_spec* gLast_car_to_skid[2];

// GLOBAL: CARMA2_HW 0x00679408
int gLast_skid_vol[2];

// based on carpocalypse2 src/carma2/common/car.c:5960 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x0041bed0
void C2_HOOK_FASTCALL SkidNoise(tCar_spec* pCar, int pSide, br_scalar pV, int pMaterial_index) {
    br_vector3 tv;
    br_vector3 wvw;
    br_vector3 world_pos;
    br_vector3 pos;
    br_vector3 wv;
    br_scalar ts;
    int i;

    extern tS3_outlet* gCar_outlet;
    extern int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);
    extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
    extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

    i = IRandomBetween(0, 1);
    if (pV < 5.0f) {
        return;
    }
    if (gCurrent_race.material_modifiers[pMaterial_index].tyre_noise_index == -1) {
        return;
    }
    if (IRandomBetween(0, 4) != 0) {
        return;
    }

    gLast_skid_vol[i] = pV * 10.0f;
    pos.v[0] = pCar->wpos[pSide].v[0];
    pos.v[1] = pCar->wpos[pSide].v[1] - pCar->oldd[pSide];
    pos.v[2] = pCar->wpos[pSide].v[2];
    BrMatrix34ApplyP(&world_pos, &pos, &pCar->car_master_actor->t.t.mat);
    BrVector3InvScale(&world_pos, &world_pos, 6.9f);
    if (!DRS3SoundStillPlaying(gSkid_tag[i])
            || (pCar != NULL && pCar->driver == eDriver_local_human && gLast_car_to_skid[i] != pCar)) {
        gSkid_tag[i] = DRS3StartSound3D(gCar_outlet,
            IRandomBetween(0, 4) + 9000,
            &world_pos,
            &pCar->vel,
            1,
            gLast_skid_vol[i],
            IRandomBetween(49152, 81920),
            0x10000);
        gLast_car_to_skid[i] = pCar;
    }
    if (gCurrent_race.material_modifiers[pMaterial_index].smoke_type == 1) {
        wv.v[0] = pCar->collision_info->omega.v[1] * pos.v[2] - pCar->collision_info->omega.v[2] * pos.v[1];
        wv.v[1] = pCar->collision_info->omega.v[2] * pos.v[0] - pCar->collision_info->omega.v[0] * pos.v[2];
        wv.v[2] = pCar->collision_info->omega.v[0] * pos.v[1] - pCar->collision_info->omega.v[1] * pos.v[0];
        tv.v[0] = pCar->collision_info->velocity_car_space.v[0] * 6.9f;
        tv.v[1] = pCar->collision_info->velocity_car_space.v[1] * 6.9f;
        tv.v[2] = pCar->collision_info->velocity_car_space.v[2] * 6.9f;
        wv.v[0] += tv.v[0];
        wv.v[1] += tv.v[1];
        wv.v[2] += tv.v[2];
        ts = -(wv.v[0] * ((br_vector3*)(pCar->field_0x1348))->v[0] + wv.v[1] * ((br_vector3*)(pCar->field_0x1348))->v[1] + wv.v[2] * ((br_vector3*)(pCar->field_0x1348))->v[2]);
        wvw.v[0] = ((br_vector3*)(pCar->field_0x1348))->v[0] * ts;
        wvw.v[1] = ((br_vector3*)(pCar->field_0x1348))->v[1] * ts;
        wvw.v[2] = ((br_vector3*)(pCar->field_0x1348))->v[2] * ts;
        wv.v[0] += wvw.v[0];
        wv.v[1] += wvw.v[1];
        wv.v[2] += wvw.v[2];
        BrMatrix34ApplyV(&wvw, &wv, &pCar->car_master_actor->t.t.mat);
        CreatePuffOfSmoke(&world_pos, &wvw, pV / 25.0f, 1.0f, 4);
    }
}

// StopSkid

// CrashNoise

// FUNCTION: CARMA2_HW 0x0041deb0
void C2_HOOK_FASTCALL CrushAndDamageCar(tCar_spec* pCar1, br_vector3* pPoint, br_vector3* pForce, tCar_spec* pCar2, int pChain_count, br_vector3* pChain, br_actor* pActor) {
    extern int gIt_or_fox;
    extern void C2_HOOK_FASTCALL FUN_0043fb70(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, int pCars_touch);
    extern void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick);
    extern void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity);
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);
    extern void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce);
    extern void C2_HOOK_FASTCALL PipeSingleLocalCarIncident(br_vector3* pPosition, float pForce);
    br_vector3 vForce;
    br_vector3 vOther;
    br_vector3 vWorld;
    br_matrix34 mat;
    float power;
    int driver2;

    if (pCar2 != NULL) {
        if (pCar2->field_0xc8 == NULL || pCar2->driver > 5) {
            pCar2->field_0xc8 = (undefined*)pCar1;
        }
        if (pCar1->field_0xc8 != NULL) {
            if (pCar1 == NULL) {
                return;
            }
            if (pCar1->driver > 5) {
                pCar1->field_0xc8 = (undefined*)pCar2;
            }
        } else {
            pCar1->field_0xc8 = (undefined*)pCar2;
        }
    }
    if (pCar1 == NULL) {
        return;
    }
    if (pCar1->driver <= 5) {
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        power = gCar_softness.initial_network[gCurrent_net_game->type];
    } else {
        power = gCar_softness.initial[gProgram_state.skill_level];
    }
    if (pCar1->driver == eDriver_oppo) {
        vOther.v[0] = pCar1->pos.v[0] - gCamera_to_world.m[3][0];
        vOther.v[1] = pCar1->pos.v[1] - gCamera_to_world.m[3][1];
        vOther.v[2] = pCar1->pos.v[2] - gCamera_to_world.m[3][2];
        if (200.f < vOther.v[0] * vOther.v[0] + vOther.v[1] * vOther.v[1] + vOther.v[2] * vOther.v[2]) {
            return;
        }
    }
    if (pCar2 != NULL) {
        if (pCar2->driver > 5) {
            TwoCarsHitEachOther(pCar1, pCar2);
        }
        if (pCar1->driver >= eDriver_net_human) {
            power = gArmour_starting_value[pCar1->power_up_levels[0]] * 1.2 * power;
        }
        driver2 = pCar2->driver;
        if (driver2 >= eDriver_net_human) {
            power = power * (gNet_mode != eNet_mode_none
                    ? gCar_car_damage_multiplier.initial_network[gCurrent_net_game->type]
                    : gCar_car_damage_multiplier.initial[gProgram_state.skill_level])
                * gOffensive_starting_value[pCar2->power_up_levels[2]]
                * pCar2->damage_multiplier * 0.9;
        }
        if (pCar1->driver == eDriver_oppo && driver2 == eDriver_oppo) {
            power = power * 0.2f;
        }
        if (driver2 > 5) {
            power = power / ((pCar2->softness_factor - -0.7) * 1.4285715);
        } else {
            pCar2 = NULL;
        }
    } else {
        if (pCar1->driver == eDriver_oppo) {
            power = pCar1->softness_factor * power;
            if ((int)(PDGetTotalTime() - pCar1->time_last_hit) > 5000) {
                power = power * 0.2f;
            }
        }
    }
    power = power * 0.2f;
    vForce.v[0] = pForce->v[0] * power;
    vForce.v[1] = pForce->v[1] * power;
    vForce.v[2] = pForce->v[2] * power;
    power = vForce.v[0] * vForce.v[0] + vForce.v[1] * vForce.v[1] + vForce.v[2] * vForce.v[2];
    if (pCar1->driver <= 5 || pCar1->invulnerable_no_damage == 0) {
        pCar1->damage_magnitude_accumulator = power + pCar1->damage_magnitude_accumulator;
    }
    if (pCar1->driver >= eDriver_net_human) {
        if (pCar1->field_0x4c8 != 1.0) {
            vForce.v[0] = vForce.v[0] / pCar1->field_0x4c8;
            vForce.v[1] = vForce.v[1] / pCar1->field_0x4c8;
            vForce.v[2] = vForce.v[2] / pCar1->field_0x4c8;
        }
        vOther.v[0] = vForce.v[0] * 0.5f;
        vOther.v[1] = vForce.v[1] * 0.5f;
        vOther.v[2] = vForce.v[2] * 0.5f;
        if (pCar1->driver == eDriver_local_human) {
            DoPratcamHit(&vForce);
        }
    } else {
        vOther.v[0] = vForce.v[0] * 1.5f;
        vOther.v[1] = vForce.v[1] * 1.5f;
        vOther.v[2] = vForce.v[2] * 1.5f;
    }
    if (gNet_mode == eNet_mode_host && gCurrent_net_game->type == eNet_game_type_foxy && pCar2 != NULL
            && pCar1->driver >= eDriver_net_human && pCar2->driver >= eDriver_net_human) {
        if (pCar1 == gNet_players[gIt_or_fox].car && pCar2->knackered == 0) {
            CarInContactWithFox(NetPlayerFromCar(pCar2));
        } else if (pCar2 == gNet_players[gIt_or_fox].car && pCar1->knackered == 0) {
            CarInContactWithFox(NetPlayerFromCar(pCar1));
        }
    }
    if (gNet_mode != eNet_mode_client || pCar2 == NULL) {
        FUN_0043fb70(pCar1, pPoint, &vForce, pCar2 != NULL);
    }
    FUN_0042d400(pCar1, pChain_count, pChain, &vOther, gPHIL_last_physics_tick);
    if (pCar2 != NULL && pCar2->driver == eDriver_local_human && power > 0.003) {
        PipeSingleCarIncident(pCar1, pPoint, power);
    }
    if (pCar2 == NULL && pCar1->driver == eDriver_local_human && power > 0.003) {
        BrMatrix34Copy(&mat, &pCar1->car_master_actor->t.t.mat);
        mat.m[3][0] = mat.m[3][0] * 0.14492753f;
        mat.m[3][1] = mat.m[3][1] * 0.14492753f;
        mat.m[3][2] = mat.m[3][2] * 0.14492753f;
        BrMatrix34ApplyP(&vWorld, pPoint, &mat);
        PipeSingleLocalCarIncident(&vWorld, power);
    }
}

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// ProcessForcesCallBack

// ProcessJointForcesCallBack

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

// FUNCTION: CARMA2_HW 0x0041e310
int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0041e340
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

// findfloor

// FindFloorInBoxBU

// FUNCTION: CARMA2_HW 0x0041e580
void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {

    gQuite_wild_start = 0;
    gQuite_wild_end = 0;
    gOn_me_wheels_start = 0;
    gWoz_upside_down_at_all = 0;
    gWild_start = 0;
}
// SetAmbientPratCam

// SetTextureBits

// FUNCTION: CARMA2_HW 0x0041e5a0
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {
    int i;

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car->car_master_actor->render_style != BR_RSTYLE_NONE && !gAction_replay_mode) {
            car->field_0x18cc = 0;
            if (car->keys.brake || (car->brake_force != 0.f && fabs(car->collision_info->velocity_car_space.v[2]) > 7.2463765e-05)) {
                car->field_0x18cc |= 0x4;
            }
            if (*(int*)&car->gear < 0 || (!(car != NULL && car->driver == eDriver_local_human) && car->collision_info->velocity_car_space.v[2] > 0.f)) {
                car->field_0x18cc |= 0x8;
            }
        }
    }
}

extern void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, br_scalar pMax_distance);
extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);
extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
extern int C2_HOOK_FASTCALL GetOilSpillCount(void);
extern void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize);
extern void C2_HOOK_FASTCALL ControlBoundFunkGroove(int pSlot_number, float pValue);
extern float C2_HOOK_FAKE_THISCALL ControlBoundFunkGroovePlus(int pSlot_number, undefined4 pArg2, float pValue);
extern float gYon_squared;
extern int gIt_or_fox;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x005115b0
br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees) {
    NOT_IMPLEMENTED();
    return 0.f;
}

// STUB: CARMA2_HW 0x004ff410
void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006793a4
tU32 gLast_cunning_stunt;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/car.c:1813 (SetAmbientPratCam inlined)
// FUNCTION: CARMA2_HW 0x0041e660
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {
    tU32 the_time;
    int car;
    int cat;
    tCar_spec* pCar;

    if (gNet_mode != eNet_mode_none
            && gCurrent_net_game->type == eNet_game_type_foxy
            && gThis_net_player_index == gIt_or_fox) {
        gProgram_state.current_car.power_up_levels[1] = 0;
    }

    /* inlined SetAmbientPratCam(&gProgram_state.current_car) */
    pCar = &gProgram_state.current_car;
    {
        // GLOBAL: CARMA2_HW 0x00679370
        static tU32 last_time_on_ground;
        br_scalar vcs_x;
        br_scalar vcs_y;
        br_scalar vcs_z;
        br_scalar abs_vcs_x;
        br_scalar abs_vcs_y;
        br_scalar abs_vcs_z;
        br_scalar abs_omega_x;
        br_scalar abs_omega_y;
        br_scalar abs_omega_z;
        tU32 pratcam_time;

        if (!gRace_finished) {
            pratcam_time = GetTotalTime();
            if (pCar->number_of_wheels_on_ground != 0) {
                last_time_on_ground = pratcam_time;
            }
            vcs_x = pCar->collision_info->velocity_car_space.v[0] / 1000.f;
            vcs_y = pCar->collision_info->velocity_car_space.v[1] / 1000.f;
            vcs_z = pCar->collision_info->velocity_car_space.v[2] / 1000.f;
            abs_vcs_x = fabs(vcs_x);
            abs_vcs_y = fabs(vcs_y);
            abs_vcs_z = fabs(vcs_z);
            abs_omega_x = fabs(pCar->collision_info->omega.v[0]);
            abs_omega_y = fabs(pCar->collision_info->omega.v[1]);
            abs_omega_z = fabs(pCar->collision_info->omega.v[2]);

            if (abs_omega_x > 4.5f || abs_omega_z > 4.5f) {
                ChangeAmbientPratcam(9);
            } else if (abs_omega_y > 4.5f) {
                ChangeAmbientPratcam(12);
            } else if (abs_omega_x > 3.f || abs_omega_z > 3.f) {
                ChangeAmbientPratcam(8);
            } else if (abs_omega_y > 3.f) {
                ChangeAmbientPratcam(11);
            } else if (pCar->car_master_actor->t.t.mat.m[1][1] < 0.1f) {
                ChangeAmbientPratcam(44);
            } else if (abs_vcs_y > abs_vcs_z && abs_vcs_y > abs_vcs_x && vcs_y < -.004f) {
                ChangeAmbientPratcam(6);
            } else if (pratcam_time - last_time_on_ground > 500) {
                ChangeAmbientPratcam(5);
            } else if (abs_vcs_x > abs_vcs_z && vcs_x > .001f) {
                ChangeAmbientPratcam(26);
            } else if (abs_vcs_x > abs_vcs_z && vcs_x < -.001f) {
                ChangeAmbientPratcam(25);
            } else if (abs_omega_x > 1.5f || abs_omega_z > 1.5f) {
                ChangeAmbientPratcam(7);
            } else if (abs_omega_y > 1.5f) {
                ChangeAmbientPratcam(10);
            } else if (abs_vcs_z > .01f) {
                ChangeAmbientPratcam(3);
            } else if (abs_vcs_z > .004f) {
                ChangeAmbientPratcam(2);
            } else if (abs_vcs_z > .0015f) {
                ChangeAmbientPratcam(1);
            } else {
                ChangeAmbientPratcam(0);
            }
        }
    }

    the_time = PDGetTotalTime();
    for (cat = eVehicle_self; cat <= eVehicle_net_player; cat++) {
        int car_count;

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car = 0; car < car_count; car++) {
            tCar_spec* the_car;

            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, car);
            }
            if (!(the_car != NULL && the_car->driver == eDriver_local_human) && PointOutOfSight(&the_car->pos, gYon_squared)) {
                the_car->car_master_actor->render_style = BR_RSTYLE_NONE;
            } else {
                the_car->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
            }
        }
    }
    if (!(gCar_to_view != NULL && gCar_to_view->driver == eDriver_oppo)) {
        gCar_to_view->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    }

    for (car = 0; car < gNum_active_cars; car++) {
        tCar_spec* the_car;

        the_car = gActive_car_list[car];
        if (the_car->car_master_actor->render_style != BR_RSTYLE_NONE && the_car != NULL && the_car->driver >= eDriver_oppo) {
            br_scalar car_x;
            br_scalar car_z;
            int oily_count;
            int i;

            the_car->shadow_intersection_flags = 0;
            car_x = the_car->car_master_actor->t.t.translate.t.v[0];
            car_z = the_car->car_master_actor->t.t.translate.t.v[2];
            oily_count = GetOilSpillCount();

            for (i = 0; i < oily_count; i++) {
                br_actor* oily_actor;
                br_scalar oily_size;

                GetOilSpillDetails(i, &oily_actor, &oily_size);
                if (oily_actor != NULL) {
                    br_scalar car_radius;

                    car_radius = the_car->collision_info->shape->common.bb.max.v[2] / WORLD_SCALE * 1.5f;
                    if (oily_actor->t.t.translate.t.v[0] - oily_size < car_x + car_radius
                            && oily_actor->t.t.translate.t.v[0] + oily_size > car_x - car_radius
                            && oily_actor->t.t.translate.t.v[2] - oily_size < car_z + car_radius
                            && oily_actor->t.t.translate.t.v[2] + oily_size > car_z - car_radius) {
                        the_car->shadow_intersection_flags |= 1 << i;
                    }
                }
            }
            if (the_car->driver < eDriver_net_human && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
                if (gCountdown) {
                    float sine_angle;
                    float raw_revs;
                    float rev_reducer;

                    sine_angle = FRandomBetween(.4f, 1.6f) * ((float)GetTotalTime() / ((float)gCountdown * 100.f));
                    sine_angle = frac(sine_angle) * 360.0f;
                    sine_angle = FastScalarSin((int)sine_angle);
                    raw_revs = the_car->red_line * fabs(sine_angle);
                    rev_reducer = (11.f - (float)(gCountdown)) / 10.f;
                    the_car->revs = rev_reducer * raw_revs;
                } else {
                    the_car->revs = (the_car->speedo_speed / 0.003f - (float)(int)(the_car->speedo_speed / 0.003))
                                    * (float)(the_car->red_line - 800)
                                    + 800.f;
                }
            }
            for (i = 0; i < the_car->number_of_steerable_wheels; i++) {
                ControlBoundFunkGroove(the_car->steering_ref[i], the_car->steering_angle);
            }
            for (i = 0; i < REC2_ASIZE(the_car->rf_sus_ref); i++) {
                ControlBoundFunkGroove(the_car->rf_sus_ref[i], the_car->rf_sus_position);
                if ((i & 1) != 0) {
                    ControlBoundFunkGroove(the_car->lf_sus_ref[i], -the_car->lf_sus_position);
                } else {
                    ControlBoundFunkGroove(the_car->lf_sus_ref[i], the_car->lf_sus_position);
                }
            }
            for (i = 0; i < REC2_ASIZE(the_car->rr_sus_ref); i++) {
                ControlBoundFunkGroove(the_car->rr_sus_ref[i], the_car->rr_sus_position);
                if ((i & 1) != 0) {
                    ControlBoundFunkGroove(the_car->lr_sus_ref[i], -the_car->lr_sus_position);
                } else {
                    ControlBoundFunkGroove(the_car->lr_sus_ref[i], the_car->lr_sus_position);
                }
            }
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                float wheel_speed;

                wheel_speed = -(the_car->speedo_speed / the_car->non_driven_wheels_circum * (float)gFrame_period);
                if (gAction_replay_mode && ARGetReplayDirection() < 0) {
                    wheel_speed = -wheel_speed;
                }
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_1, 0, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_2, 0, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_3, 0, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_4, 0, wheel_speed);
                if (the_car->driver >= eDriver_net_human) {
                    if (the_car->gear != 0) {
                        wheel_speed = -(the_car->revs
                                        * the_car->speed_revs_ratio
                                        * (float)the_car->gear
                                        * (float)gFrame_period
                                        / (1000.f * WORLD_SCALE))
                                        / the_car->driven_wheels_circum;
                    } else if (the_car->keys.brake) {
                        wheel_speed = 0.0;
                    } else {
                        wheel_speed = -(the_car->speedo_speed / the_car->driven_wheels_circum * (float)gFrame_period);
                    }
                    if (gAction_replay_mode && ARGetReplayDirection() < 0) {
                        wheel_speed = -wheel_speed;
                    }
                }
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_1, 0, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_2, 0, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_3, 0, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_4, 0, wheel_speed);
            }
            if (gAction_replay_mode) {
                MungeSpecialVolume(the_car->collision_info);
            } else if (the_car->driver == eDriver_local_human && the_car->collision_info->M < 5.f) {
                br_scalar abs_omega_x;
                br_scalar abs_omega_y;
                br_scalar abs_omega_z;
                int spinning_wildly;

                abs_omega_x = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[0] + 3.30f) / 2.f * fabs(the_car->collision_info->omega.v[0]);
                abs_omega_y = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[1] + 3.57f) / 2.f * fabs(the_car->collision_info->omega.v[1]);
                abs_omega_z = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[2] + 0.44f) / 2.f * fabs(the_car->collision_info->omega.v[2]);
                spinning_wildly = abs_omega_x > 26.4f || abs_omega_y > 49.98f || abs_omega_z > 3.52f;
                if (spinning_wildly && the_time - gLast_cunning_stunt > 10000) {
                    if (gWild_start == 0
                            || (the_car->collision_info->last_special_volume != NULL && the_car->collision_info->last_special_volume->gravity_multiplier != 1.f)) {
                        gWild_start = the_time;
                    } else if (the_time - gWild_start >= 500) {
                        DoFancyHeadup(18);
                        EarnCredits(gCunning_stunt_bonus[gProgram_state.skill_level]);
                        gLast_cunning_stunt = the_time;
                        gOn_me_wheels_start = 0;
                        gQuite_wild_end = 0;
                        gQuite_wild_start = 0;
                        gWoz_upside_down_at_all = 0;
                    }
                } else {
                    int spinning_mildly;

                    gWild_start = 0;
                    spinning_mildly = abs_omega_x > 1.65f || abs_omega_z > .22f;
                    if (the_car->number_of_wheels_on_ground < 4) {
                        gOn_me_wheels_start = 0;
                        if (the_car->number_of_wheels_on_ground == 0 && spinning_mildly) {
                            if (gQuite_wild_start == 0) {
                                gQuite_wild_start = the_time;
                            }
                            if (the_car->car_master_actor->t.t.mat.m[1][1] < -.8f) {
                                gWoz_upside_down_at_all = the_time;
                            }
                        } else {
                            gQuite_wild_end = the_time;
                        }
                    } else {
                        if (gQuite_wild_end == 0) {
                            gQuite_wild_end = the_time;
                        }
                        if (gQuite_wild_start == 0
                                || the_time - gLast_cunning_stunt <= 10000) {
                            gWild_start = 0;
                        } else if (gQuite_wild_end - gQuite_wild_start >= 2000
                                && gQuite_wild_start <= gWoz_upside_down_at_all
                                && gWoz_upside_down_at_all <= gQuite_wild_end) {

                            if (gOn_me_wheels_start != 0) {
                                if (the_time - gOn_me_wheels_start > 500
                                        && !(the_car->collision_info->last_special_volume != NULL && the_car->collision_info->last_special_volume->gravity_multiplier != 1.f)) {
                                    DoFancyHeadup(18);
                                    EarnCredits(gCunning_stunt_bonus[gProgram_state.skill_level]);
                                    gLast_cunning_stunt = PDGetTotalTime();
                                    gQuite_wild_start = 0;
                                    gQuite_wild_end = 0;
                                    gOn_me_wheels_start = 0;
                                    gWoz_upside_down_at_all = 0;
                                } else {
                                    gWild_start = 0;
                                }
                            } else if (the_time - gQuite_wild_end < 300) {
                                gOn_me_wheels_start = the_time;
                                gWild_start = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041f280
intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data *crush_data = pActor->user;

    if (crush_data != NULL && crush_data->groove == NULL) {
        pActor->type = BR_ACTOR_NONE;
    }
    return 0;
}
// DoLODCarModels

// FUNCTION: CARMA2_HW 0x0041f2a0
void C2_HOOK_FASTCALL DoComplexCarModels(void) {
    int i;
    tCar_spec* car;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > 5) {
            SwitchCarModels(car, 0);
        }
    }
}
// ResetCarScreens

// FUNCTION: CARMA2_HW 0x0041f3b0
void C2_HOOK_FASTCALL SwapCar(void) {
}

// FUNCTION: CARMA2_HW 0x0041f3c0
// FUNCTION: CARMA2_SW 0x0041f440
void C2_HOOK_FASTCALL AdjustDownForce(void) {
    char s[100];
    tCar_spec* c;

    c = gCar_to_view;
    c->downforce_to_weight += 50.f;
    if (c->downforce_to_weight > 2000.f) {
        c->downforce_to_weight = 50.f;
    }
    if (c->downforce_to_weight > 300.f) {
        c->downforce_to_weight = 2000.f;
    }
    sprintf(s, "DownForceSpeed %f", c->downforce_to_weight);
    NewTextHeadupSlot(4, 0, 1500, -4, s);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0041f4f0
void C2_HOOK_FASTCALL FlyCar(tCar_spec* c, br_scalar dt) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0041ff20
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor) {
    NOT_IMPLEMENTED();
    return NULL;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041ff00
tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor) {

    if (!gCrush_deferred && !gTesting_car_for_sensible_place) {
        return NULL;
    }
    return DoPullActorFromWorld(actor);
}

// PipeNonCarObject

// PipeNonCars

extern tNon_car_spec* gActive_non_car_list[99];
extern br_actor* gAdditional_actors;
extern int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data);
extern void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

// GLOBAL: CARMA2_HW 0x00679418
tU32 gCheck_for_deattachment_total_time;

// based on carpocalypse2 src/carma2/common/car.c:3975 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004203c0
void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime) {
    int new_count;

    new_count = 0;
    if (gNum_active_non_cars != 0) {
        int i;

        ARStartPipingSession(ePipe_chunk_non_car);
        for (i = 0; i < gNum_active_non_cars; i++) {
            tNon_car_spec* non_car = gActive_non_car_list[i];

            if (non_car->car_ID != -1) {
                PhysicsObjectRecurse(non_car->collision_info, PipeNonCarObject, NULL);
            }
        }
        AREndPipingSession();
        gCheck_for_deattachment_total_time += pTime;
        if (gCheck_for_deattachment_total_time >= 1000) {
            gCheck_for_deattachment_total_time = 0;
            for (i = 0; i < gNum_active_non_cars; i++) {
                tNon_car_spec* non_car = gActive_non_car_list[i];

                if (non_car->actor->t.t.translate.t.v[1] < gMin_world_y - 10.f) {
                    non_car->collision_info->disable_move_rotate = 1;
                }
                if (TestForNan(&non_car->actor->t.t.translate.t.v[1])) {
                    non_car->collision_info->omega.v[0] = 0.f;
                    non_car->collision_info->omega.v[1] = 0.f;
                    non_car->collision_info->omega.v[2] = 0.f;
                    BrMatrix34Identity(&non_car->actor->t.t.mat);
                    non_car->actor->t.t.translate.t.v[0] = 2000.f;
                    non_car->actor->t.t.translate.t.v[1] = 0.f;
                    non_car->actor->t.t.translate.t.v[2] = 0.f;
                    non_car->collision_info->disable_move_rotate = 1;
                }
                gActive_non_car_list[new_count] = non_car;
                if (non_car->collision_info->disable_move_rotate && non_car->driver == eDriver_4) {
                    int j;
                    int drop;
                    br_actor* non_car_actor;

                    drop = 1;
                    non_car_actor = non_car->actor;
                    for (j = 0; j < gNum_cars_and_non_cars; j++) {
                        tCar_spec* car = gActive_car_list[j];

                        if (car != (tCar_spec*)non_car && !car->collision_info->disable_move_rotate) {
                            br_matrix34 mat;
                            br_bounds bb;

                            BrMatrix34Mul(&mat, &non_car_actor->t.t.mat, &car->collision_info->field_0x144);
                            GetNewBoundingBox(&bb, &non_car_actor->model->bounds, &mat);
                            if (!(bb.min.v[0] > car->collision_info->field_0x124.max.v[0]
                                  || bb.min.v[1] > car->collision_info->field_0x124.max.v[1]
                                  || bb.min.v[2] > car->collision_info->field_0x124.max.v[2]
                                  || car->collision_info->field_0x124.min.v[0] > bb.max.v[0]
                                  || car->collision_info->field_0x124.min.v[1] > bb.max.v[1]
                                  || car->collision_info->field_0x124.min.v[2] > bb.max.v[2])) {
                                drop = 0;
                                break;
                            }
                        }
                    }
                    if (drop) {
                        if (non_car->flags & 0x10000) {
                            br_vector3* stored_pos = (br_vector3*)non_car->collision_info->transform_matrix.m[3];
                            br_vector3 diff;

                            diff.v[0] = non_car->collision_info->field_0x17c.v[0] - stored_pos->v[0];
                            diff.v[1] = non_car->collision_info->field_0x17c.v[1] - stored_pos->v[1];
                            diff.v[2] = non_car->collision_info->field_0x17c.v[2] - stored_pos->v[2];
                            if (diff.v[0] * diff.v[0] + diff.v[1] * diff.v[1] + diff.v[2] * diff.v[2] > 0.005f * 0.005f) {
                                drop = 0;
                            } else {
                                stored_pos->v[0] = non_car->collision_info->field_0x17c.v[0];
                                stored_pos->v[1] = non_car->collision_info->field_0x17c.v[1];
                                stored_pos->v[2] = non_car->collision_info->field_0x17c.v[2];
                            }
                        }
                        if ((tCar_spec*)non_car != gCar_to_view) {
                            BrActorRemove(non_car_actor);
                            ClearSplashes(non_car->collision_info);
                            new_count -= 1;
                            non_car->driver = eDriver_non_car_unused_slot;
                            if (non_car->car_ID != -1) {
                                tU8 col_x;
                                tU8 col_z;
                                br_actor* parent;

                                XZToColumnXZ(&col_x, &col_z, non_car_actor->t.t.mat.m[3][0], non_car_actor->t.t.mat.m[3][2], &gProgram_state.track_spec);
                                parent = gTrack_actor;
                                if (gProgram_state.track_spec.columns[col_z][col_x].actor_0x0 != NULL) {
                                    parent = gProgram_state.track_spec.columns[col_z][col_x].actor_0x0;
                                }
                                BrActorAdd(parent, non_car_actor);
                            } else if (gAdditional_actors != NULL) {
                                BrActorAdd(gAdditional_actors, non_car_actor);
                            }
                            non_car_actor->type_data = NULL;
                            PHILRemoveObject(non_car->collision_info);
                        }
                    }
                }
                new_count += 1;
            }
            gNum_active_non_cars = new_count;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004207e0
void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix) {
    tU8 column_x;
    tU8 column_z;
    br_actor* parent;

    BrMatrix34Copy(&pActor->t.t.mat, pMatrix);
    if (pActor->parent == gNon_track_actor) {
        return;
    }
    XZToColumnXZ(&column_x, &column_z, pActor->t.t.mat.m[3][0], pActor->t.t.mat.m[3][2], &gProgram_state.track_spec);
    parent = gProgram_state.track_spec.columns[column_z][column_x].actor_0x0;
    if (parent != pActor->parent && parent != NULL) {
        BrActorRemove(pActor);
        BrActorAdd(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, pActor);
    }
}

// FUNCTION: CARMA2_HW 0x00420880
int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref == gFace_num__car
        || (pCar->collision_info->box_face_ref == gFace_num__car - 1 && pCar->collision_info->box_face_start > gFace_count)) {
        *pFace_refs = &gFace_list__car[pCar->collision_info->box_face_start];
        return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
    }
    return 0;
}
// FUNCTION: CARMA2_HW 0x004208f0
void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x00420900
void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_NONE;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00414510
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index) {
    int start_i;
    int place_on_grid;
    br_scalar nearest_y_below;
    tCar_spec* car;
    br_actor* car_actor;
    br_scalar nearest_y_above;
    int inactive;
    br_vector3 dist;
    br_vector3 grid_offset;
    br_model* above_model;
    br_model* below_model;
    int below_face_index;
    int above_face_index;
    tCompressed_matrix3 compressed;
    br_matrix34 initial_yaw_matrix;
    int i;
    int j;
    br_angle initial_yaw;
    br_vector3 real_pos;

    car_actor = pThe_race->opponent_list[pCar_index].car_spec->car_master_actor;
    car = pThe_race->opponent_list[pCar_index].car_spec;
    BrMatrix34Identity(&car_actor->t.t.mat);
    place_on_grid = 1;
    if (gNet_mode != eNet_mode_none && !gCurrent_net_game->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.f) {
                        BrVector3Sub(&real_pos, &real_pos, &gInitial_position);
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.f) {
                        break;
                    }
                }
            }
            if (j == gNumber_of_net_players) {
                BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[i].pos);
                initial_yaw = BrDegreeToAngle(pThe_race->net_starts[i].yaw);
                place_on_grid = 0;
                break;
            }
            i++;
            if (i == pThe_race->count_network_start_points) {
                i = 0;
            }
        } while (i != start_i);
    }
    if (gNet_mode == eNet_mode_none && pGrid_index < 0) {
        BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[-pGrid_index - 1].pos);
        initial_yaw = BrDegreeToAngle(pThe_race->net_starts[-pGrid_index - 1].yaw);
        place_on_grid = 0;
    }
    if (place_on_grid) {
        initial_yaw = BrDegreeToAngle(pThe_race->initial_yaw);
        BrMatrix34RotateY(&initial_yaw_matrix, initial_yaw);
        grid_offset.v[0] = -(br_scalar)(pGrid_index % 2);
        grid_offset.v[1] = 0.0f;
        grid_offset.v[2] = 0.4f * (br_scalar)(pGrid_index % 2) - (br_scalar)(pGrid_index / 2) * -2.0f;
        BrMatrix34ApplyV(&car_actor->t.t.translate.t, &grid_offset, &initial_yaw_matrix);
        BrVector3Accumulate(&car_actor->t.t.translate.t, &pThe_race->initial_position);
    }
    if (gTrack_actor != NULL) {
        FindBestY(&car_actor->t.t.translate.t, gTrack_actor, 10.0f, &nearest_y_above, &nearest_y_below, &above_model, &below_model, &above_face_index, &below_face_index);
        if (nearest_y_above != 30000.f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_above;
        } else if (nearest_y_below != -30000.f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_below;
        } else {
            car_actor->t.t.translate.t.v[1] = 0.0f;
        }
    }
    BrMatrix34PreRotateY(&car_actor->t.t.mat, initial_yaw);
    if (gNet_mode != eNet_mode_none) {
        CompressMatrix34(&compressed, &inactive, &car_actor->t.t.mat);
        ExpandMatrix34(&car_actor->t.t.mat, &compressed, inactive);
        BrMatrix34Copy(&gNet_players[pThe_race->opponent_list[pCar_index].net_player_index].initial_position, &car->car_master_actor->t.t.mat);
    }
    if (gNet_mode != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.f) {
        DisableCar(car);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00414400
void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar) {

    InitialiseCar2(pCar, 1);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern tSlot_info gInitial_APO[3];
extern tSlot_info gInitial_APO_potential[3];
extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);
extern void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00413f70
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
    int index;
    int j;
    int level;
    br_matrix34 safe_position;
    tNet_game_player_info* net_player;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x261, 0x261);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, message_time, 0x268);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x49c, 0x49c);

    PossibleService();
    if (pCar->disabled && pClear_disabled_flag) {
        if (gNet_mode == eNet_mode_none) {
            EnableCar(pCar);
        } else {
            net_player = NetPlayerFromCar(pCar);
            if (net_player->player_status == ePlayer_status_ready || net_player->player_status == ePlayer_status_racing) {
                EnableCar(pCar);
            }
        }
    }
    InitCarSkidStuff(pCar);
    pCar->car_model_actor->render_style = BR_RSTYLE_DEFAULT;
    SwitchCarModels(pCar, 0);
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    pCar->field_0xc8 = NULL;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        ResetRecoveryVouchers();
    }
    BrVector3SetFloat(&pCar->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pCar->collision_info->omega, 0.f, 0.f, 0.f);
    pCar->curvature = 0.f;
    pCar->field_0x1260 = 0.f;
    BrMatrix34Copy(&safe_position, &pCar->car_master_actor->t.t.mat);
    if (safe_position.m[3][0] > 500.0f) {
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &gInitial_position);
    }
    BrMatrix34Copy(&pCar->old_frame_mat, &safe_position);
    BrMatrix34Copy(&pCar->collision_info->transform_matrix, &safe_position);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (j = 0; j < 4; j++) {
        pCar->oldd[j] = pCar->ride_height;
    }
    pCar->gear = 0.f;
    pCar->revs = 0.f;
    pCar->traction_control = 1;
    BrVector3Negate(&pCar->direction, (br_vector3*)pCar->car_master_actor->t.t.mat.m[2]);
    for (j = 0; j < 20; j++) {
        BrMatrix34Copy(&pCar->last_safe_positions[j], &safe_position);
    }
    pCar->collision_info->field_0x261 = 0;
    pCar->collision_info->message_time = 0;
    pCar->dt = -1.f;
    pCar->collision_info->field_0x49c = 1;
    pCar->time_to_recover = 0;
    pCar->repair_time  = 0;
    pCar->collision_info->water_d  = 10000.f;

    switch (pCar->driver) {
    case eDriver_oppo:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_oppo) {
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
                    pCar->car_ID = 0x200 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_net_human:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_net_human) {
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
                    pCar->car_ID = 0x100 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_local_human:
        pCar->car_ID = 0;
        break;
    default:
        break;
    }
    PossibleService();
    pCar->collision_info->box_face_ref = gFace_num__car - 2;
    pCar->collision_info->box_face_end = 0;
    pCar->collision_info->box_face_start = 0;
    pCar->collision_info->disable_move_rotate = 0;
    pCar->end_steering_damage_effect = 0;
    pCar->end_trans_damage_effect = 0;
    pCar->wheel_dam_offset[0] = 0.f;
    pCar->wheel_dam_offset[1] = 0.f;
    pCar->wheel_dam_offset[2] = 0.f;
    pCar->wheel_dam_offset[3] = 0.f;
    pCar->shadow_intersection_flags = 0;
    pCar->underwater_ability = 0;

    if (gNet_mode == eNet_mode_none) {
        net_player = NULL;
    } else {
        net_player = NetPlayerFromCar(pCar);
    }
    if (net_player != NULL && net_player->field_0x80) {
        pCar->invulnerable_no_crushage = 1;
        pCar->invulnerable_no_damage = 1;
        pCar->invulnerable_no_wastage = 1;
    } else {
        pCar->invulnerable_no_crushage = 0;
        pCar->invulnerable_no_damage = 0;
        pCar->invulnerable_no_wastage = 0;
    }
    pCar->wall_climber_mode = 0;
    pCar->grip_multiplier = 1.f;
    pCar->damage_multiplier = 1.f;
    pCar->field_0x4c8 = 1.f;
    pCar->field_0x4d4 = 1.f;
    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
    pCar->knackered = 0;
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    RemoveFromCloakingList(pCar);
    TurnOffCloaking(NULL, pCar);
    if (pCar == NULL || pCar->driver != eDriver_local_human) {
        pCar->joystick.left = -1;
        pCar->joystick.right = -1;
    }
    TotallyRepairACar(pCar);
    SetCarSuspGiveAndHeight(pCar, 1.f, 1.f, 1.f, 0.f, 0.f);
    for (j = 0; j < 100; j++) {
        pCar->powerups[j] = 0;
    }
    if (gNet_mode != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < 3; j++) {
            if (gNet_mode != eNet_mode_none) {
                level = gInitial_APO[j].initial_network[gCurrent_net_game->type];
            } else {
                level = gInitial_APO[j].initial[gProgram_state.skill_level];
            }
            pCar->power_up_levels[j] = level;
            if (gNet_mode != eNet_mode_none) {
                level = gInitial_APO_potential[j].initial_network[gCurrent_net_game->type];
            } else {
                level = gInitial_APO_potential[j].initial[gProgram_state.skill_level];
            }
            pCar->power_up_slots[j] = level;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on carpocalypse2 src/carma2/common/raycast.c:400 and :492
// FUNCTION: CARMA2_HW 0x004e3bc0
void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index) {
    br_matrix34 camera_tfm;
    br_camera* camera_data;
    br_actor* parent;
    br_angle view_over_2;
    br_scalar aspect;
    br_scalar angle;
    br_scalar cos_angle;
    br_scalar sin_angle;
    br_scalar scale;
    extern br_actor* gY_picking_camera;
    extern br_scalar gLowest_y_above;
    extern br_scalar gHighest_y_below;
    extern br_scalar gCurrent_y;
    extern br_matrix34 gPick_model_to_view__raycast;
    extern br_model* gAbove_model;
    extern br_model* gBelow_model;
    extern int gAbove_face_index;
    extern int gBelow_face_index;
    extern br_material* gBlack_material;
    extern int C2_HOOK_FASTCALL ActorPick2D(br_actor* pActor, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback, void* pArg);
    extern int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg);

    gLowest_y_above = 30000.0f;
    gHighest_y_below = -30000.0f;
    gCurrent_y = pPosition->v[1] - -1.192093e-5f;
    gY_picking_camera->t.t.translate.t = *pPosition;
    gY_picking_camera->t.t.mat.m[3][1] += pStarting_height;
    camera_data = (br_camera*)gY_picking_camera->type_data;
    if (gY_picking_camera == pWorld) {
        BrMatrix34Identity(&camera_tfm);
    } else {
        BrTransformToMatrix34(&camera_tfm, &gY_picking_camera->t);
        for (parent = gY_picking_camera->parent; parent != NULL && parent != pWorld; parent = parent->parent) {
            if (parent->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&camera_tfm, &parent->t);
            }
        }
    }
    BrMatrix34Inverse(&gPick_model_to_view__raycast, &camera_tfm);
    view_over_2 = camera_data->field_of_view / 2;
    aspect = camera_data->aspect;
    angle = BrAngleToRadian(view_over_2);
    cos_angle = cosf(angle);
    sin_angle = sinf(angle);
    scale = cos_angle / sin_angle;
    BrMatrix34PostScale(&gPick_model_to_view__raycast, scale / aspect, scale, 1.0f);
    ActorPick2D(pWorld, NULL, gBlack_material, FindHighestCallBack__raycast, NULL);
    *pNearest_y_above = gLowest_y_above;
    *pNearest_y_below = gHighest_y_below;
    *pNearest_above_model = gAbove_model;
    *pNearest_below_model = gBelow_model;
    *pNearest_above_face_index = gAbove_face_index;
    *pNearest_below_face_index = gBelow_face_index;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004157e0
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004fa5b0
void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004207a0
int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x004fee70
void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0043fb70
void C2_HOOK_FASTCALL FUN_0043fb70(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, int pCars_touch) {
    NOT_IMPLEMENTED();
}
// STUB: CARMA2_HW 0x0042d400
void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static int notfinite(const float* f) {
    if (f != NULL) {
        return ((~*(tU32*)f) & 0x7f800000) == 0;
    } else {
        return 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on carpocalypse2 src/carma2/common/finteray.c:630 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x00425ba0
int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2) {
    int i;

    if (b1->min.v[2] > b2->max.v[2]
           || b2->min.v[2] > b1->max.v[2]
           || b1->min.v[1] > b2->max.v[1]
           || b2->min.v[1] > b1->max.v[1]
           || b1->min.v[0] > b2->max.v[0]
           || b2->min.v[0] > b1->max.v[0]) {

        return 0;
    }

    for (i = 0; i < 3; i++) {
        if (notfinite(&b1->min.v[i]) || notfinite(&b1->max.v[i])
                || notfinite(&b2->min.v[i]) || notfinite(&b2->max.v[i])) {
            return 0;
        }
    }
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
