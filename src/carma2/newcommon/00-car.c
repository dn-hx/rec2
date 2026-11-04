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
extern int C2_HOOK_FASTCALL SetJoystickFFBForce(int pMagnitude);
extern int C2_HOOK_FASTCALL SetJoystickFFBSteeringForce(int pMagnitude);
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

// GLOBAL: CARMA2_HW 0x006793d4
int gDouble_pling_water;

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

// GLOBAL: CARMA2_HW 0x005895f0
double gZero = 1.0;

// GLOBAL: CARMA2_HW 0x005896a8
float gF1000 = 1000.0f;

// GLOBAL: CARMA2_HW 0x00589694
float gFneg1000 = -1000.0f;

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

extern tWorld_callbacks gWorld_callbacks;
extern void C2_HOOK_FASTCALL APTCPreCollision(void);
extern void C2_HOOK_FASTCALL APTCPostCollision(void);

// GLOBAL: CARMA2_HW 0x0058f6e0
tPhysics_callbacks gCar_physics_callbacks = {
    &gWorld_callbacks,
    APTCPreCollision,
    APTCPostCollision,
    APTCChangedObjects,
    APTCActiveHalted,
    (int (C2_HOOK_FASTCALL *)(tPhysics_object*))APTCPassiveActivated,
    NULL,
};

// GLOBAL: CARMA2_HW 0x006793a0
int gFreeze_mechanics;

// GLOBAL: CARMA2_HW 0x006793f4
tU32 last_frame_start;

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

// and :1178
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

extern void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00415ef0
void C2_HOOK_FASTCALL AddDrag(tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDt) {
    br_scalar drag;
    br_scalar k;
    br_vector3 v;

    drag = pDt * 0.0005f;
    if (pObject->last_special_volume != NULL) {
        k = pObject->last_special_volume->viscosity_multiplier;
        if (pCar->underwater_ability != 0) {
            k *= 0.6;
        }
        drag *= k;
        drag *= pObject->water_depth_factor;
    }

    k = BrVector3Length(&pObject->v) * drag * 6.9 / pObject->M;
    v.v[0] = pObject->v.v[0] * k;
    v.v[1] = pObject->v.v[1] * k;
    v.v[2] = pObject->v.v[2] * k;
    DRVector3Diminish(&pObject->v, &v);

    k = BrVector3Length(&pObject->omega) * drag;
    v.v[0] = pObject->omega.v[0] * k;
    v.v[1] = pObject->omega.v[1] * k;
    v.v[2] = pObject->omega.v[2] * k;
    ApplyWaterOmegaBrake(pObject, &v);
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

#define PO_U8(o, off) (*(tU8*)((char*)(o) + (off)))
#define PO_U16(o, off) (*(tU16*)((char*)(o) + (off)))
#define PO_U32(o, off) (*(tU32*)((char*)(o) + (off)))
#define PO_F32(o, off) (*(float*)((char*)(o) + (off)))

extern int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern tWorld_callbacks gWorld_callbacks;
extern void C2_HOOK_FASTCALL UpdateCrushTimers(void);
extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object* pCollision_info, tU32 pDisable);
extern tPhysics_object* gPHIL_list_collision_infos;
extern tU32 gPalette_fade_time;
extern tU32 gPHIL_last_physics_tick;
extern tNon_car_spec* gActive_non_car_list[99];
extern tU32 C2_HOOK_FASTCALL GetRaceTime(void);
extern void C2_HOOK_FASTCALL GetFacesInBox(tPhysics_object* pCollision, tWorld_callbacks* pWorld_callbacks);
extern void C2_HOOK_FASTCALL MinePreCollisionStuff(void);
extern void C2_HOOK_FASTCALL PedPreCollisionStuff(void);
extern void C2_HOOK_FASTCALL DronePreCollisionStuff(void);
extern tPhysics_object* gDrivable_on_list[50];
extern tU32 gDrivable_on_count;
extern void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt);
extern void C2_HOOK_FASTCALL MoveNonCar(tNon_car_spec* pNon_car, br_scalar pDt);
extern void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);
typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
    br_vector3 field_0xc;
    br_vector3 field_0x18;
    br_vector3 field_0x24;
    br_bounds3 bounds;
    float field_0x48;
} tUpdate_crush_semi_detach_item;
typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
    br_bounds3 bounds;
} tUpdate_crush_detach_item;
typedef struct {
    tCar_spec* car;
    tPhysics_object* object;
    tU32 time;
} tUpdate_crush_full_detach_item;
typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
} tUpdate_crush_reattach_item;
extern tUpdate_crush_semi_detach_item gNet_crush_semi_detach_bit_list[8];
extern int gCount_net_crush_semi_detach_bit_list;
extern tUpdate_crush_detach_item gNet_crush_detach_list[8];
extern int gCount_net_crush_detach_list;
extern tUpdate_crush_full_detach_item gNet_crush_full_detach_bit_list[8];
extern int gCount_net_crush_full_detach_bit_list;
extern tUpdate_crush_reattach_item gNet_crush_reattach_bit_list[8];
extern int gCount_net_crush_reattach_bit_list;
extern tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, tU32 pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);
extern void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);
extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pCollision_info, int pParam);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pCollision);
extern void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2);
extern int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter);
extern int C2_HOOK_FASTCALL BonerPassiveCollision(tPed_character_instance* pCharacter, undefined4 pArg2);
extern void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2);
extern int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther);
extern void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2);

// FUNCTION: CARMA2_HW 0x00415890
void C2_HOOK_FASTCALL APTCPreCollision(void) {
    tPhysics_object* obj;
    tCar_spec* car;
    tNon_car_spec* non_car;
    tPhysics_object* node;
    void** wp;
    int i;
    float x;
    float f;

    gDrivable_on_count = 0;
    for (obj = gPHIL_list_collision_infos; obj != NULL && gDrivable_on_count < 50; obj = obj->next) {
        if (obj->drivable_on) {
            gDrivable_on_list[gDrivable_on_count] = obj;
            gDrivable_on_count += 1;
        }
    }

    gCar_to_view_original_v.v[0] = gCar_to_view->collision_info->v.v[0];
    gCar_to_view_original_v.v[1] = gCar_to_view->collision_info->v.v[1];
    gCar_to_view_original_v.v[2] = gCar_to_view->collision_info->v.v[2];

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            if (*wp != NULL && ((tCar_spec*)*wp)->driver > 5) {
                car = (tCar_spec*)*wp;
                BrMatrix34Copy(&car->car_master_actor->t.t.mat, &car->collision_info->transform_matrix);
                if (PO_U8(car->collision_info, 0x261) == 0x10
                        && car->collision_info->message_time == gPHIL_last_physics_tick) {
                    if (car == NULL || car->driver != eDriver_local_human) {
                        car->curvature = (float)(tS16)PO_U16(car->collision_info, 0x278) * car->maxcurve * 0.000030518509f;
                    }
                    if (gNet_mode == eNet_mode_host) {
                        car->collision_info->field_0xf0 = 1;
                    }
                    SetCollisionFlagsAndStuff(car);
                }
                if (car->disabled == 0) {
                    if (!(car->collision_info->disable_move_rotate != 0
                            && gPalette_fade_time != 0
                            && car != NULL
                            && car->driver == eDriver_local_human)) {
                        if (car->collision_info->box_face_ref != gFace_num__car
                                && (car->collision_info->box_face_ref != gFace_num__car - 1
                                    || car->collision_info->box_face_start <= gFace_count)) {
                            GetFacesInBox(car->collision_info, &gWorld_callbacks);
                        }
                        if (car->dt != 0.f) {
                            MoveAndCollideCar(car, 0.04f);
                        }
                        if (car->collision_info->child != NULL) {
                            PhysicsObjectMoveVelocityList(car->collision_info->child);
                            for (node = car->collision_info->child; node != NULL; node = node->next) {
                                AddDrag(car, node, 0.04f);
                                if (node->child != NULL) {
                                    DragChildren(car, node->child);
                                }
                            }
                            PositionChildren(car->collision_info);
                        }
                    }
                }
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    i = 0;
    if (i < gNum_active_non_cars) {
        wp = (void**)gActive_non_car_list;
        do {
            non_car = (tNon_car_spec*)*wp;
            if (PO_U8(non_car->collision_info, 0x261) != 0
                    && non_car->collision_info->message_time == gPHIL_last_physics_tick) {
                SetCollisionFlagsAndStuff((tCar_spec*)non_car);
            }
            if (non_car->flags & 0x10000) {
                if (PO_U32(non_car, 0xf8) != 0 && PO_U32(non_car, 0xf8) < GetRaceTime()) {
                    PO_U32(non_car, 0xf8) = 0;
                    non_car->flags = (non_car->flags & 0xffff0100) | 0x100;
                    non_car->collision_info->disable_move_rotate = 0;
                }
            }
            if (non_car->collision_info->disable_move_rotate == 0
                    || (non_car->flags & 0xff) != 0
                    || (non_car->flags & 0xff00) != 0) {
                non_car->collision_info->disable_move_rotate = 0;
                if (PO_F32(non_car, 0xc4) != 0.f) {
                    MoveAndCollideNonCar(non_car, 0.04f);
                }
                if (non_car->collision_info->child != NULL) {
                    PhysicsObjectMoveVelocityList(non_car->collision_info->child);
                    PositionChildren(non_car->collision_info);
                }
            }
            i++;
            wp++;
        } while (i < gNum_active_non_cars);
    }

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            car = (tCar_spec*)*wp;
            if (car->field_0x4c8 != gZero) {
                x = car->field_0x4c8;
                f = car->collision_info->M; f *= x; car->collision_info->M = f;
                f = car->collision_info->I.v[0]; f *= x; car->collision_info->I.v[0] = f;
                f = car->collision_info->I.v[1]; f *= x; car->collision_info->I.v[1] = f;
                f = car->collision_info->I.v[2]; f *= x; car->collision_info->I.v[2] = f;
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    if (gCar_flying) {
        tCar_spec* c = gCar_to_view;

        c->collision_info->transform_matrix.m[3][1] -= gFneg1000;
        BrMatrix34Copy(&c->car_master_actor->t.t.mat, &c->collision_info->transform_matrix);
        PositionChildren(c->collision_info);
        SetCollisionInfoChildsDoNothing(c->collision_info, 1);
    }
    MinePreCollisionStuff();
    PedPreCollisionStuff();
    DronePreCollisionStuff();
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
typedef struct {
    tCar_spec* car;
    float step;
    float* total_displacement;
    float displacement;
} tCrush_displacement_context_local;
extern br_scalar gCrushZero;
extern int gCount_toggled_doors;
extern int gCount_crush_detach_list;
extern br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount);
extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec);
extern intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec);
extern int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL PipeSingleDSModel(int pKind, void* pThing);
extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);
extern int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);
extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);
extern void C2_HOOK_FASTCALL FUN_0043b250(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_0043b2a0(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL FUN_0043b400(tPhysics_object* pObject, tCrush_displacement_context_local* pContext);
extern intptr_t C2_HOOK_CDECL FUN_0043b2e0(br_actor* pActor, tCrush_displacement_context_local* pContext);
extern void C2_HOOK_FASTCALL SmoothCarCrushShapePoints(tCar_spec* pCar, float pStep);
extern br_scalar gCrushReattachDistanceSquared;
extern float gFLOAT_00589b48;

// based on the REA pseudocode of the original at 0x0043ace0 (verify with state/dis.sh)
// FUNCTION: CARMA2_HW 0x0043ace0
float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, tU32 pDelta, float* pOut) {
    tCar_crush_spec* crush;
    tCrush_displacement_context_local ctx;
    tCar_bit_spec* bit;
    tCar_crush_detach_data* detach;
    tUser_crush_data* user;
    br_model* model;
    br_vertex* vertices;
    float repaired;
    tU32 delta;
    int i;
    int j;
    int found;

    delta = pDelta;
    pCar->repair_time += delta;
    if (pCar->use_shell_model != 0
            || pCar == NULL
            || pCar->driver < eDriver_net_human
            || (crush = pCar->car_crush_spec) == NULL
            || crush->field_0x4b8 != 0
            || gAusterity_mode != 0) {
        return 0.f;
    }
    if (crush->field_0x144 == 0) {
        if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
            for (i = 0; i < crush->field_0x2b0; i++) {
                bit = &crush->field_0x2b4[i];
                if (bit->field_0xc != NULL
                        && bit->field_0x4 != NULL
                        && (user = bit->field_0x4->user) != NULL
                        && (model = user->models[0]) != NULL) {
                    detach = (tCar_crush_detach_data*)bit->field_0xc;
                    vertices = model->vertices;
                    if (Vector3DistanceSquared(&vertices[detach->min_vertex].p, (br_vector3*)&detach->field_0xc) < gCrushReattachDistanceSquared
                            && Vector3DistanceSquared(&vertices[detach->max_vertex].p, (br_vector3*)(detach->field_0xc + 12)) < gCrushReattachDistanceSquared) {
                        if (gNet_mode != eNet_mode_none) {
                            found = 0;
                            for (j = 0; j < gCount_net_crush_reattach_bit_list; j++) {
                                if (gNet_crush_reattach_bit_list[j].actor == bit->field_0x0) {
                                    found = 1;
                                    break;
                                }
                            }
                            if (!found) {
                                FUN_0043b250(pCar, bit->field_0x8);
                            }
                        } else {
                            ReAttachBit(pCar, bit->field_0x4, bit->field_0x0, NULL);
                        }
                    }
                }
            }
            for (i = 0; i < crush->field_0x270; i++) {
                bit = &crush->field_0x274[i];
                if (bit->field_0xc != NULL
                        && bit->field_0x4 != NULL
                        && (user = bit->field_0x4->user) != NULL
                        && (model = user->models[0]) != NULL) {
                    detach = (tCar_crush_detach_data*)bit->field_0xc;
                    vertices = model->vertices;
                    if (Vector3DistanceSquared(&vertices[detach->min_vertex].p, (br_vector3*)&detach->field_0xc) < gCrushReattachDistanceSquared
                            && Vector3DistanceSquared(&vertices[detach->max_vertex].p, (br_vector3*)(detach->field_0xc + 12)) < gCrushReattachDistanceSquared) {
                        if (gNet_mode != eNet_mode_none) {
                            FUN_0043b250(pCar, bit->field_0x8);
                        } else {
                            ReAttachBit(pCar, bit->field_0x4, bit->field_0x0, bit->field_0x8);
                        }
                    }
                }
            }
            MungeDetachLists(crush);
        }
        *pOut = 0.f;
        ctx.car = pCar;
        ctx.step = (float)delta * gFLOAT_00589b48;
        ctx.total_displacement = pOut;
        ctx.displacement = 0.f;
        PhysicsObjectRecurse(pCar->collision_info, (tEnumCollision_cbfn*)FUN_0043b400, &ctx);
        SmoothCarCrushShapePoints(pCar, ctx.step);
        for (i = 0; i < crush->field_0x2b0; i++) {
            DRActorEnumRecurse(crush->field_0x2b4[i].field_0x0, (br_actor_enum_cbfn*)FUN_0043b2e0, &ctx);
        }
        repaired = ctx.displacement;
        if (ctx.displacement == gCrushZero) {
            RepairAllSmashes(pCar);
            TotallyRepairCarCollisionShapes(pCar);
            if (gNet_mode == eNet_mode_none) {
                for (i = 0; i < crush->field_0x2b0; i++) {
                    ReAttachBit(pCar, crush->field_0x2b4[i].field_0x4, crush->field_0x2b4[i].field_0x0, NULL);
                }
                i = 0;
                crush->field_0x2b0 = 0;
                for (i = 0; i < crush->field_0x270; i++) {
                    ReAttachBit(pCar, crush->field_0x274[i].field_0x4, crush->field_0x274[i].field_0x0, crush->field_0x274[i].field_0x8);
                }
                crush->field_0x270 = 0;
            } else if (gNet_mode == eNet_mode_host) {
                FUN_0043b2a0(pCar);
            }
            DRActorEnumRecurse(pCar->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar);
            PipeSingleDSModel(0, pCar);
            gCount_toggled_doors = 0;
            gCount_crush_detach_list = 0;
            if (*pOut == gCrushZero
                    && (gNet_mode == eNet_mode_none
                        || (crush->field_0x270 < 1 && crush->field_0x2b0 < 1))) {
                PhysicsObjectRecurse(pCar->collision_info, BattenDownTheObjects, pCar);
                if (pCar->driver == eDriver_local_human) {
                    pCar->repair_time += 100000;
                }
                pCar->use_shell_model = 1;
            } else {
                repaired = 1.1920929e-07f;
            }
        }
    } else {
        repaired = UnBendCar(pCar, delta, pOut);
    }
    if (SwapShapesIfPossible(pCar) != 0 && pCar->driver == eDriver_local_human) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(6));
    }
    return repaired;
}
// PLACEMENT NOTE (2026-11-02, revised for the FUN_0045c910 body): this TU is
// placement-sensitive, MSVC 5.0 flips x87 operand order in unrelated functions when the
// block layout changes. Working arrangement (all four verified 100 together):
//   - WeldCarByNetwork/WeldCarPartial stay here at their address-ordered slot; moving
//     them to the end of the TU swaps fld/fadd in CalcGraphicalWheelStuff 0x4157e0 and
//     in the weld bodies themselves (100 -> EFFECTIVE).
//   - the joystick force-feedback constants and the SetJoystickFFBCollisionForce stub
//     stay at the very end of the file (after FullyDetachSnappedBits 0x431310); moving
//     them before 0x431310 swaps two fld operands in that function (100 -> 97.69).
//   - ActuallySmashCarPart 0x4ed2b0 also stays at the very end of the TU (its own slot
//     before FlushSmashQueue, i.e. before CalcGraphicalWheelStuff, floors that function
//     at 99.99: fld [ecx+0x1254]/fadd [ecx+0x1260] instead of the original order; the
//     source operand order cannot fix it, only this placement does).
// Re-measure 0x4157e0, 0x43dce0, 0x43df10, 0x431310 and 0x4ba550 after any edit here.
// FUNCTION: CARMA2_HW 0x0043dce0
void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar) {
    extern br_scalar gFLOAT_00589a2c;
    extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7);
    tCar_crush_spec* spec;
    tPhysics_object* info;
    br_scalar temp;
    br_scalar m1;
    tU8 f;
    int flag;
    tU16 id_a;
    tU16 id_b;
    tU16 id_c;
    br_vector3 v;
    int c2;
    int c1;

    spec = pCar->car_crush_spec;
    if (spec->field_0x144 != 0) {
        info = pCar->collision_info;
        f = *(tU8*)((tU8*)info + 0x481);
        c1 = (((f & 1) != 0 && spec->field_0x174 == spec->bend_z_min)
                || ((f & 1) == 0 && spec->field_0x174 == spec->bend_z_max)) & 1;
        c2 = ((f & 2) != 0 && *(tU16*)((tU8*)spec + 0x190) == *(tU16*)((tU8*)info + 0x482))
            || ((f & 4) != 0 && *(tU16*)((tU8*)spec + 0x192) == *(tU16*)((tU8*)info + 0x482))
            || ((f & 8) != 0 && *(tU16*)((tU8*)spec + 0x194) == *(tU16*)((tU8*)info + 0x482));
        if (c1 & c2) {
            return;
        }
        UnBendCar(pCar, 2000, &temp);
    }
    info = pCar->collision_info;
    id_a = 0;
    v.v[0] = 0.f;
    v.v[1] = 0.f;
    v.v[2] = 0.f;
    id_c = 0;
    id_b = 0;
    if ((*(tU8*)((tU8*)info + 0x481) & 1) != 0) {
        v.v[2] = spec->bend_z_min;
        temp = info->bb1.max.v[2];
        flag = 1;
    } else {
        v.v[2] = spec->bend_z_max;
        temp = info->bb1.min.v[2];
        flag = 0;
    }
    switch (*(tU8*)((tU8*)info + 0x481) >> 1) {
    case 1:
        id_a = *(tU16*)((tU8*)info + 0x482);
        if ((id_a < 0x7ff8 && flag) || (id_a > 0x7ff8 && !flag)) {
            v.v[1] = info->bb1.min.v[1];
        } else {
            v.v[1] = info->bb1.max.v[1];
        }
        break;
    case 2:
        id_b = *(tU16*)((tU8*)info + 0x482);
        if ((id_b > 0x7ff8 && flag) || (id_b < 0x7ff8 && !flag)) {
            v.v[0] = info->bb1.min.v[0];
        } else {
            v.v[0] = info->bb1.max.v[0];
        }
        break;
    case 4:
        id_c = *(tU16*)((tU8*)info + 0x482);
        m1 = info->bb1.max.v[0];
        v.v[0] = (m1 + info->bb1.min.v[0]) * gFLOAT_00589a2c;
        m1 = info->bb1.min.v[1];
        v.v[1] = (m1 + info->bb1.max.v[1]) * gFLOAT_00589a2c;
        break;
    }
    BendCar(pCar, id_a, id_b, id_c, &v, temp, 0);
}
// based on REA pseudocode and // FUNCTION: CARMA2_HW 0x0043df10
void C2_HOOK_FASTCALL WeldCarPartial(tCar_spec* pCar) {
    extern void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag);
    tCar_crush_spec* spec;
    const tU16* pData;
    br_vector4 plane;

    spec = pCar->car_crush_spec;
    if (spec == NULL) {
        return;
    }
    pData = (const tU16*)((tU8*)pCar->collision_info + 0x484);
    ExpandVector3((br_vector3*)&plane, (const tCompressed_vector3*)pData, -1.f, 1.f);
    plane.v[3] = DRU16ToScalar(pData[3], -2.5f, 2.5f);
    if (spec->field_0x4b8 != 0) {
        if (spec->field_0x548.v[0] == plane.v[0] && spec->field_0x548.v[1] == plane.v[1] && spec->field_0x548.v[2] == plane.v[2] && spec->field_0x554 == plane.v[3]) {
            return;
        }
        WeldCar(pCar);
    }
    SplitCar(pCar, &plane, 0);
}
extern void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL WeldCarPartial(tCar_spec* pCar);
// based on REA pseudocode of the original
// FUNCTION: CARMA2_HW 0x0043db00
void C2_HOOK_FASTCALL UpdateCrushVertices(tCar_spec* pCar, tU8* pData) {
    extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec);
    extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);
    extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
    tPhysics_shape* shape;
    tCar_crush_shape_info* shape_info;
    int i;
    int j;
    int count;

    if (pCar->car_crush_spec->field_0x4b8 != 0) {
        count = *pData++;
        shape = pCar->collision_info->shape;
        while (shape != NULL && count > 0) {
            if (shape->common.type == kCollisionShapeType_Polyhedron
                    || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                shape->polyhedron.polyhedron.count_points = *pData++;
                for (i = 0; i < shape->polyhedron.polyhedron.count_points; i++) {
                    ExpandVector3(&shape->polyhedron.polyhedron.points[i], (const tCompressed_vector3*)pData, -2.5f, 2.5f);
                    pData += 6;
                }
                count--;
            }
            shape = shape->common.next;
        }
    } else {
        pData++;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            shape_info = &pCar->car_crush_spec->field_0x4[i];
            for (j = 0; j < shape_info->count_points; j++) {
                ExpandVector3(&shape_info->field_0x18[j].field_0x18, (const tCompressed_vector3*)pData, -2.5f, 2.5f);
                pData += 6;
                shape_info->field_0x18[j].field_0xc.v[0] = shape_info->field_0x18[j].field_0x18.v[0];
                shape_info->field_0x18[j].field_0xc.v[1] = shape_info->field_0x18[j].field_0x18.v[1];
                shape_info->field_0x18[j].field_0xc.v[2] = shape_info->field_0x18[j].field_0x18.v[2];
                shape_info->field_0x0->polyhedron.points[j].v[0] = shape_info->field_0x18[j].field_0xc.v[0];
                shape_info->field_0x0->polyhedron.points[j].v[1] = shape_info->field_0x18[j].field_0xc.v[1];
                shape_info->field_0x0->polyhedron.points[j].v[2] = shape_info->field_0x18[j].field_0xc.v[2];
            }
            FillInShape((tPhysics_shape*)shape_info->field_0x0);
        }
    }
    UpdateCollisionObject(pCar->collision_info);
    pCar->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar);
    pCar->car_crush_spec->field_0x26 = PO_U16(pCar->collision_info, 0x290);
    pCar->car_crush_spec->field_0x24 = pCar->car_crush_spec->field_0x26;
    pCar->car_crush_spec->expand_bounding_box = 0;
}
// based on REA pseudocode of the original; list layout from the disassembly
// FUNCTION: CARMA2_HW 0x0043ceb0
void C2_HOOK_FASTCALL UpdateCrushPainList(void) {
    tCar_crush_spec* car_crush;
    tCar_spec* car;
    br_actor* bit;
    int count;
    int keep;
    int found;
    int i;
    int j;

    count = 0;
    for (i = 0; i < gCount_net_crush_semi_detach_bit_list; i++) {
        keep = 1;
        if (gNet_crush_semi_detach_bit_list[i].time <= gPHIL_last_physics_tick) {
            SemiDetachBit(gNet_crush_semi_detach_bit_list[i].car, gNet_crush_semi_detach_bit_list[i].actor,
                *(tU32*)&gNet_crush_semi_detach_bit_list[i].field_0x48, &keep,
                &gNet_crush_semi_detach_bit_list[i].field_0xc,
                &gNet_crush_semi_detach_bit_list[i].bounds,
                &gNet_crush_semi_detach_bit_list[i].field_0x18,
                &gNet_crush_semi_detach_bit_list[i].field_0x24);
        }
        if (keep != 0) {
            if (count != i) {
                gNet_crush_semi_detach_bit_list[count] = gNet_crush_semi_detach_bit_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_semi_detach_bit_list = count;

    count = 0;
    for (i = 0; i < gCount_net_crush_detach_list; i++) {
        if (gNet_crush_detach_list[i].time <= gPHIL_last_physics_tick) {
            DetachBit(gNet_crush_detach_list[i].car, gNet_crush_detach_list[i].actor,
                &gNet_crush_detach_list[i].bounds);
        } else {
            if (count != i) {
                gNet_crush_detach_list[count] = gNet_crush_detach_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_detach_list = count;

    count = 0;
    for (i = 0; i < gCount_net_crush_full_detach_bit_list; i++) {
        if (gNet_crush_full_detach_bit_list[i].time <= gPHIL_last_physics_tick) {
            FullyDetachBit(gNet_crush_full_detach_bit_list[i].car, gNet_crush_full_detach_bit_list[i].object);
            MungeDetachLists(gNet_crush_full_detach_bit_list[i].car->car_crush_spec);
        } else {
            if (count != i) {
                gNet_crush_full_detach_bit_list[count] = gNet_crush_full_detach_bit_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_full_detach_bit_list = count;

    count = 0;
    for (i = 0; i < gCount_net_crush_reattach_bit_list; i++) {
        if (gNet_crush_reattach_bit_list[i].time <= gPHIL_last_physics_tick) {
            car = gNet_crush_reattach_bit_list[i].car;
            bit = gNet_crush_reattach_bit_list[i].actor;
            car_crush = car->car_crush_spec;
            if (bit == NULL) {
                for (j = 0; j < car_crush->field_0x2b0; j++) {
                    ReAttachBit(car, car_crush->field_0x2b4[j].field_0x4, car_crush->field_0x2b4[j].field_0x0, NULL);
                }
                car_crush->field_0x2b0 = 0;
                for (j = 0; j < car_crush->field_0x270; j++) {
                    ReAttachBit(car, car_crush->field_0x274[j].field_0x4, car_crush->field_0x274[j].field_0x0, car_crush->field_0x274[j].field_0x8);
                }
                car_crush->field_0x270 = 0;
            } else {
                found = 0;
                for (j = 0; j < car_crush->field_0x2b0; j++) {
                    if (car_crush->field_0x2b4[j].field_0x0 == bit) {
                        ReAttachBit(car, car_crush->field_0x2b4[j].field_0x4, bit, NULL);
                        MungeDetachLists(car_crush);
                        found = 1;
                    }
                }
                if (!found) {
                    for (j = 0; j < car_crush->field_0x270; j++) {
                        if (car_crush->field_0x274[j].field_0x0 == bit) {
                            ReAttachBit(car, car_crush->field_0x274[j].field_0x4, bit, car_crush->field_0x274[j].field_0x8);
                            MungeDetachLists(car_crush);
                            break;
                        }
                    }
                }
            }
        } else {
            if (count != i) {
                gNet_crush_reattach_bit_list[count] = gNet_crush_reattach_bit_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_reattach_bit_list = count;
}
// FUNCTION: CARMA2_HW 0x00417030
void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {

    if (pNon_car->collision_info->water_d != 10000.f) {
        TestAutoSpecialVolume(pNon_car->collision_info);
    }
    MungeSpecialVolume(pNon_car->collision_info);
    if (((tCar_spec*)pNon_car)->dt >= 0.f) {
        pDt = ((tCar_spec*)pNon_car)->dt;
    }
    MoveNonCar(pNon_car, pDt);
    BrMatrix34ApplyP(&((tCar_spec*)pNon_car)->pos, &((tCar_spec*)pNon_car)->centre_of_mass_world_scale, &pNon_car->actor->t.t.mat);
    ((tCar_spec*)pNon_car)->pos.v[0] = ((tCar_spec*)pNon_car)->pos.v[0] * 0.14492753f;
    ((tCar_spec*)pNon_car)->pos.v[1] = ((tCar_spec*)pNon_car)->pos.v[1] * 0.14492753f;
    ((tCar_spec*)pNon_car)->pos.v[2] = ((tCar_spec*)pNon_car)->pos.v[2] * 0.14492753f;
}

// FUNCTION: CARMA2_HW 0x00415c40
void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tCar_spec* pCar) {
    float temp;
    int i;

    if (gNet_mode == eNet_mode_host
            && PO_U32(pCar->collision_info, 0x49c) > PO_U32(pCar->collision_info, 0x274)) {
        PO_U8(pCar->collision_info, 0x261) = 0;
        pCar->dt = -1.f;
        return;
    }

    switch (pCar->collision_info->field_0xf0) {
    case 0:
        SelectCrushNetwork((undefined4*)&PO_U8(pCar->collision_info, 0x294), pCar->collision_info, 2);
        break;
    case 1:
        SelectCrushNetwork((undefined4*)&PO_U8(pCar->collision_info, 0x294), pCar->collision_info, 1);
        break;
    case 2:
        SelectCrushNetwork((undefined4*)&PO_U8(pCar->collision_info, 0x294), pCar->collision_info, 1);
        break;
    }

    if (pCar != NULL && pCar->driver > 5) {
        for (i = 0; i < 4; i++) {
            pCar->oldd[i] = (float)PO_U8(pCar->collision_info, 0x26c + i)
                    * pCar->susp_height[i >> 1] * (1.f / 255.f);
        }

        if (pCar->driver == eDriver_net_human) {
            pCar->revs = (float)PO_U16(pCar->collision_info, 0x27a);
        }

        if (pCar->driver >= eDriver_net_human
                && PO_U32(pCar->collision_info, 0x27c) > pCar->repair_time) {
            if (PO_U32(pCar->collision_info, 0x27c) - pCar->repair_time >= 100000) {
                TotallyRepairACar(pCar);
                pCar->repair_time = PO_U32(pCar->collision_info, 0x27c);
            } else {
                ComputeCarImpact(pCar, PO_U32(pCar->collision_info, 0x27c) - pCar->repair_time, &temp);
            }
            for (i = 0; i < 12; i++) {
                pCar->damage_units[i].damage_level = PO_U8(pCar->collision_info, 0x280 + i);
            }
            SetSmokeLastDamageLevel(pCar);
            StopCarSmoking(pCar);
        } else {
            for (i = 0; i < 12; i++) {
                pCar->damage_units[i].damage_level = PO_U8(pCar->collision_info, 0x280 + i);
            }
            SortOutSmoke(pCar);
        }

        if (pCar->collision_info->disable_move_rotate != 0) {
            if (BrVector3LengthSquared(&pCar->collision_info->v) != 0.f) {
                pCar->collision_info->disable_move_rotate = 0;
            }
        }

        if (pCar->driver >= eDriver_net_human
                && (PO_U8(pCar->collision_info, 0x292) & 1) != 0
                && (PO_U16(pCar->collision_info, 0x290) != pCar->car_crush_spec->field_0x24
                        || pCar->driver == eDriver_local_human)) {
            if ((PO_U8(pCar->collision_info, 0x292) & 2) != 0) {
                WeldCarByNetwork(pCar);
            } else if (pCar->car_crush_spec->field_0x144 != 0) {
                CompletelyUnBendCar(pCar);
            }

            if ((PO_U8(pCar->collision_info, 0x292) & 4) != 0) {
                WeldCarPartial(pCar);
            } else if (pCar->car_crush_spec->field_0x4b8 != 0) {
                WeldCar(pCar);
            }

            UpdateCrushVertices(pCar, &PO_U8(pCar->collision_info, 0x2fc));
        }

        if (pCar->driver != eDriver_local_human) {
            for (i = 0; i < 4; i++) {
                *(tU32*)&pCar->wheel_dam_offset[i] = PO_U32(pCar->collision_info, 0x48c + i * 4);
            }
        }

        GetFacesInBox(pCar->collision_info, &gWorld_callbacks);
    }

    PO_U8(pCar->collision_info, 0x261) = 0;
    if (gNet_mode == eNet_mode_client) {
        PO_U32(pCar->collision_info, 0x49c) = PO_U32(pCar->collision_info, 0x274);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);

// FUNCTION: CARMA2_HW 0x004161a0
void C2_HOOK_FASTCALL APTCPostCollisionTree(tPhysics_object* pObject) {
    tPhysics_object* child;
    tCar_spec* car;
    int collided;

    for (; pObject != NULL; pObject = pObject->next) {
        if (gNet_mode == eNet_mode_host) {
            collided = (pObject->collision_flag & 0x2) != 0 || (pObject->flags & 0x1000) != 0;
            for (child = pObject->child; child != NULL; child = child->next) {
                collided |= child->collision_flag & 0x2;
            }
            if (collided) {
                pObject->field_0x49c = gPHIL_last_physics_tick + 40;
                if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
                    car = pObject->owner;
                    if (car->driver > 5 && car->car_crush_spec != NULL) {
                        SetAllShapeStatusFlags(car->car_crush_spec->network_stuff);
                    }
                }
            }
        }
        pObject->disable_move_rotate &= ~0x2;
        APTCPostCollisionTree(pObject->child);
    }
}

// FUNCTION: CARMA2_HW 0x00416070
void C2_HOOK_FASTCALL APTCPostCollision(void) {
    void** wp;
    tCar_spec* car;
    int i;

    if (gCar_flying) {
        tCar_spec* c = gCar_to_view;

        c->collision_info->transform_matrix.m[3][1] -= gF1000;
        BrMatrix34Copy(&c->car_master_actor->t.t.mat, &c->collision_info->transform_matrix);
        PositionChildren(c->collision_info);
        SetCollisionInfoChildsDoNothing(c->collision_info, 0);
    }

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            float x;
            float fac;
            float f;

            car = (tCar_spec*)*wp;
            if (car->field_0x4c8 != gZero) {
                x = car->field_0x4c8;
                fac = (float)(gZero / x);
                f = car->collision_info->M; f *= fac; car->collision_info->M = f;
                f = car->collision_info->I.v[0]; f *= fac; car->collision_info->I.v[0] = f;
                f = car->collision_info->I.v[1]; f *= fac; car->collision_info->I.v[1] = f;
                f = car->collision_info->I.v[2]; f *= fac; car->collision_info->I.v[2] = f;
            }
            if (((tCar_spec*)*wp)->number_of_wheels_on_ground != 0) {
                SetCollisionInfoChildsDoNothing(((tCar_spec*)*wp)->collision_info, 0);
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    i = 0;
    if (i < gNum_cars_and_non_cars) {
        wp = (void**)gActive_car_list;
        do {
            car = (tCar_spec*)*wp;
            car->frame_collision_flag |= (int)(signed char)car->collision_info->collision_flag;
            i++;
            wp++;
        } while (i < gNum_cars_and_non_cars);
    }

    UpdateCrushTimers();
    if (gNet_mode) {
        UpdateCrushPainList();
    }
    APTCPostCollisionTree(gPHIL_list_collision_infos);
}

// FUNCTION: CARMA2_HW 0x00414910
void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2) {
    tFace_ref* pFace;
    br_bounds3 current_bounds;
    br_scalar old_d;
    float prop7;

    pFace = (tFace_ref*)arg2;
    old_d = pCollision->water_d;
    if (pCollision->owner == NULL) {
        return;
    }
    prop7 = PHILGetObjectProperty(pCollision, 7);
    if (pCollision->flags_0x238 != 1 && prop7 == 0.f) {
        return;
    }
    if (pCollision != NULL
        && pCollision->owner != NULL
        && pCollision->flags_0x238 == 1
        && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human
        && pCollision->water_d != 10000.f
        && gDouble_pling_water
        && BrVector3Dot(&pCollision->water_normal, &pCollision->field_0xf4.max) - pCollision->water_d <= 0.f) {
        gInTheSea = 1;
        FreezeCamera();
    }
    if (pFace != NULL && fabs(pFace->normal.v[1]) > 0.9f) {
        BrVector3Copy(&pCollision->water_normal, &pFace->normal);
        if (pCollision->water_normal.v[1] < 0.f) {
            BrVector3Negate(&pCollision->water_normal, &pCollision->water_normal);
        }
        pCollision->water_d = (((pFace->v[0].v[2]) * (pCollision->water_normal.v[2])) + ((pFace->v[0].v[1]) * (pCollision->water_normal.v[1]))) + ((pFace->v[0].v[0]) * (pCollision->water_normal.v[0]));
        if (pCollision != NULL
            && pCollision->owner != NULL
            && pCollision->flags_0x238 == 1
            && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human) {
            if (pFace->material->identifier[1] == '!') {
                if (BrVector3Dot(&pCollision->field_0xf4.min, &pCollision->water_normal) - pCollision->water_d < 0.f) {
                    GetNewBoundingBox(&current_bounds, &pCollision->bb1, &pCollision->actor->t.t.mat);
                    if ((((pCollision->water_normal.v[0]) * (current_bounds.min.v[0])) + ((pCollision->water_normal.v[1]) * (current_bounds.min.v[1]))) + ((pCollision->water_normal.v[2]) * (current_bounds.min.v[2])) - pCollision->water_d < 0.f) {
                        gInTheSea = 1;
                        FreezeCamera();
                    }
                }
                gDouble_pling_water = 1;
            } else {
                gDouble_pling_water = 0;
            }
        }
    } else {
        pCollision->water_d = 10000.f;
        if (pCollision != NULL
            && pCollision->owner != NULL
            && pCollision->flags_0x238 == 1
            && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human) {
            gInTheSea = gInTheSea == 1 ? 2 : 0;
        }
    }
    if (fabs(old_d - pCollision->water_d) > 1e-05) {
        if (PHILGetObjectProperty(pCollision, 6) != 0.f) {
            ARStartPipingSession((tPipe_chunk_type)59);
            AddExtendedSplashToPipingSession(pCollision, pCollision);
            AREndPipingSession();
        }
        if (pCollision != NULL && pCollision->owner != NULL && pCollision->flags_0x238 == 1) {
            tCar_spec* owner = (tCar_spec*)pCollision->owner;
            if (owner != NULL && owner->driver > 5) {
                ARStartPipingSession((tPipe_chunk_type)20);
                AddSplashToPipingSession(pCollision);
                AREndPipingSession();
                return;
            }
            ARStartPipingSession((tPipe_chunk_type)59);
            AddExtendedSplashToPipingSession(pCollision, pCollision->actor);
            AREndPipingSession();
            return;
        }
        if (pCollision != NULL && pCollision->owner != NULL && pCollision->flags_0x238 >= 2 && pCollision->flags_0x238 <= 4) {
            ARStartPipingSession((tPipe_chunk_type)59);
            AddExtendedSplashToPipingSession(pCollision, pCollision->owner);
            AREndPipingSession();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00416270
int C2_HOOK_FASTCALL APTCActiveHalted(tPhysics_object* pObject) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return BonerActiveHalted((tPed_character_instance*)pObject->owner);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        return MyDroneHathHalteth(pObject);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004162b0
int C2_HOOK_FASTCALL APTCPassiveActivated(tPhysics_object* pObject, undefined4 pArg2) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return BonerPassiveCollision((tPed_character_instance*)pObject->owner, pArg2);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        return MyDroneHathCollideth(pObject, (tPhysics_object*)pArg2);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00416300
void C2_HOOK_FASTCALL APTCChangedObjects(tPhysics_object* pObject, undefined4 pArg2) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 2) {
        BonerPedMovedByPhysics((tPed_character_instance*)pObject->owner, pArg2);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        MoveMyDroneBaby(pObject, pArg2);
    }
}

extern int gOver_shoot;
extern int gStop_opponents_moving;
extern tNon_car_spec* gActive_non_car_list[99];
extern tCar_spec* gLast_car_to_skid[2];
extern int gSkid_tag[2];
extern void C2_HOOK_FASTCALL CalcModifiedLocation(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag);
extern void C2_HOOK_FASTCALL PHILDoPhysics(tPhysics_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period);
extern int C2_HOOK_FASTCALL TimeToSendData(void);
extern void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime);
extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);
extern int C2_HOOK_FASTCALL RayCastInBox(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, tPhysics_object* pObject);
extern tSpecial_volume* gDefault_water_spec_vol_real;
extern void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time);
extern void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time);

/* SendCarData 0x497900, SendMines 0x4dcfb0 and their stubs are implemented at the very
   end of this file: as with FullyDetachSnappedBits, their TU position decides the x87
   operand order of other functions (0x431310 was 97.69% while they sat here). */

// PARTIAL 90.16%: list copy emits memcpy rep movsb tail instead of rep movsd; minor register allocation
// FUNCTION: CARMA2_HW 0x00416340
void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 pLast_tick_time, tU32 pFrame_period) {
    int i;

    if (gFreeze_mechanics) {
        return;
    }
    if (gNet_mode == eNet_mode_client) {
        ForceRebuildActiveCarList();
    }
    gNum_cars_and_non_cars = gNum_active_non_cars + gNum_active_cars;
    if (gNum_active_cars < gNum_cars_and_non_cars) {
        memcpy(&gActive_car_list[gNum_active_cars], gActive_non_car_list, (gNum_cars_and_non_cars - gNum_active_cars) * sizeof(void*));
    }
    last_frame_start = pLast_tick_time;
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        car->frame_collision_flag = gOver_shoot && car->collision_info->collision_flag;
        if (car != NULL && car->driver > 5) {
            CalcModifiedLocation(car);
            if (car->driver == eDriver_oppo && gStop_opponents_moving) {
                car->acc_force = 0.f;
                car->brake_force = 0.f;
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
            }
            if (!car->wheel_slip) {
                if (gLast_car_to_skid[0] == car) {
                    DRS3StopSound(gSkid_tag[0]);
                }
                if (gLast_car_to_skid[1] == car) {
                    DRS3StopSound(gSkid_tag[1]);
                }
            }
            if (car->driver == eDriver_net_human && car->collision_info->message_time < pLast_tick_time - 1000) {
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
                car->keys.horn = 0;
            }
            SetSmokeLastDamageLevel(car);
        }
    }
    PHILDoPhysics(&gCar_physics_callbacks, pLast_tick_time, pFrame_period);
    if (TimeToSendData()) {
        SendCarData(gPHIL_last_physics_tick);
        SendMines(gPHIL_last_physics_tick);
    }
    FinishCars(pLast_tick_time + pFrame_period, pFrame_period);
    CheckForDeAttachmentOfNonCars(pFrame_period);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 74.90%: first-loop x87 scheduling and ebx/ebp roles differ (original: ebx=&transform_matrix, ebp=&bb2.min); frame 0x3c, slots, compares and calls match
// FUNCTION: CARMA2_HW 0x004ff5d0
void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject) {
    br_scalar t1;
    br_scalar t2;
    tSpecial_volume* def_vol;
    br_vector3 edge;
    br_vector3 low;
    br_vector3 high;
    br_vector3 dir;
    br_scalar d;
    br_scalar d_neg;
    br_scalar d_pos;
    int k;

    d = 0.0f;
    for (k = 0; k < 3; k++) {
        d += BR_MAC3(pObject->transform_matrix.m[k][1], pObject->water_normal.v[1],
                         pObject->transform_matrix.m[k][0], pObject->water_normal.v[0],
                         pObject->transform_matrix.m[k][2], pObject->water_normal.v[2]) * pObject->bb2.min.v[k];
    }
    d += BR_MAC3(pObject->transform_matrix.m[3][0], pObject->water_normal.v[0],
                 pObject->transform_matrix.m[3][1], pObject->water_normal.v[1],
                 pObject->transform_matrix.m[3][2], pObject->water_normal.v[2]);
    d_pos = d;
    d_neg = d;
    for (k = 0; k < 3; k++) {
        t1 = BR_MAC3(pObject->transform_matrix.m[k][1], pObject->water_normal.v[1],
                     pObject->transform_matrix.m[k][2], pObject->water_normal.v[2],
                     pObject->transform_matrix.m[k][0], pObject->water_normal.v[0]) * (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]);
        if (t1 < 0.0f) {
            d_neg += t1;
        } else {
            d_pos += t1;
        }
    }
    if (d_neg >= pObject->water_d) {
        pObject->auto_special_volume = NULL;
        pObject->water_depth_factor = 1.0f;
        return;
    }
    if (d_pos - 0.001 > pObject->water_d) {
        pObject->water_depth_factor = (pObject->water_d - d_neg) / (d_pos - d_neg);
    } else {
        pObject->water_depth_factor = 1.0f;
    }
    if (pObject->auto_special_volume != NULL) {
        return;
    }
    def_vol = gDefault_water_spec_vol_real;
    if (def_vol != NULL) {
        BrMatrix34ApplyP(&low, &pObject->bb2.min, &pObject->transform_matrix);
        high.v[0] = low.v[0];
        high.v[1] = low.v[1];
        high.v[2] = low.v[2];
        for (k = 0; k < 3; k++) {
            edge.v[0] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][0];
            edge.v[2] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][2];
            edge.v[1] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][1];
            if (BrVector3Dot(&pObject->water_normal, &edge) > 0.0f) {
                high.v[0] += edge.v[0];
                high.v[1] += edge.v[1];
                high.v[2] += edge.v[2];
            } else {
                low.v[0] += edge.v[0];
                low.v[1] += edge.v[1];
                low.v[2] += edge.v[2];
            }
        }
        dir.v[0] = low.v[0] - high.v[0];
        dir.v[1] = low.v[1] - high.v[1];
        dir.v[2] = low.v[2] - high.v[2];
        DisablePlingMaterials();
        RayCastInBox(&high, &dir, &edge, &t1, pObject);
        EnablePlingMaterials();
        RayCastInBox(&high, &dir, &edge, &t2, pObject);
        if (t1 < t2) {
            pObject->auto_special_volume = def_vol;
            return;
        }
    }
    pObject->water_depth_factor = 1.0f;
    pObject->auto_special_volume = NULL;
}

#define CAR_GEAR(c) (*(int*)&(c)->gear)

// (GPL-3.0), adapted to the 0x415300 disassembly
// PARTIAL 92.40%: frame 8 vs 0xc (original spills ft to [esp+0x18]); within-block x87/int scheduling in the factor, brake and revs blocks (fld/fmul order, hoisted loads) - non-source classes
// FUNCTION: CARMA2_HW 0x00415300
void C2_HOOK_FASTCALL CalcEngineForce(tCar_spec* pCar, br_scalar pDt) {
    br_scalar speed;
    br_scalar factor;
    tS32 joy;

    speed = -pCar->collision_info->velocity_car_space.v[2] * 6.9;
    pCar->acc_force = 0.f;
    if (pCar->revs == 0.f) {
        CAR_GEAR(pCar) = 0;
    }
    if ((!pCar->keys.backwards && CAR_GEAR(pCar) < 0) ||
        (CAR_GEAR(pCar) == 0 && speed < -0.5 && !pCar->keys.backwards)) {
        pCar->keys.backwards = !pCar->keys.backwards;
        pCar->keys.acc ^= pCar->keys.dec;
        pCar->keys.dec ^= pCar->keys.acc;
        pCar->keys.acc ^= pCar->keys.dec;
        joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = joy;
    }
    if ((pCar->keys.backwards && CAR_GEAR(pCar) > 0) ||
        (CAR_GEAR(pCar) == 0 && speed > 0.5 && pCar->keys.backwards)) {
        pCar->keys.backwards = !pCar->keys.backwards;
        pCar->keys.acc ^= pCar->keys.dec;
        pCar->keys.dec ^= pCar->keys.acc;
        pCar->keys.acc ^= pCar->keys.dec;
        joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = joy;
    }
    if (CAR_GEAR(pCar) == 0 && !pCar->keys.acc && pCar->joystick.acc <= 0 &&
        (pCar->keys.dec || pCar->joystick.dec > 0) && !pCar->keys.backwards &&
        (float)fabs(speed) < 1.f) {
        pCar->keys.backwards = 1;
        pCar->keys.acc = pCar->keys.dec;
        pCar->keys.dec = 0;
        joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = joy;
    }

    pCar->torque = pCar->revs * pCar->revs * -1e-08f - 0.2;
    if (pCar->keys.acc || pCar->joystick.acc >= 0) {
        if (fabs(pCar->curvature) > pCar->maxcurve * 0.5f && CAR_GEAR(pCar) < 2 &&
            CAR_GEAR(pCar) != 0 && pCar->traction_control && pCar->joystick.acc < 0) {
            factor = 0.7f;
        } else if (pCar->joystick.acc >= 0) {
            factor = pCar->joystick.acc * (1.0 / 54613.0);
        } else {
            factor = 1.2f;
        }
        factor = factor * pCar->field_0x4d4;
        factor = factor * gPower_starting_value[pCar->power_up_levels[1]];
        if (pCar->damage_units[0].damage_level > 10) {
            factor = factor * (1.f - (pCar->damage_units[0].damage_level - 10) * 0.01f);
        }
        pCar->torque = pCar->torque + factor;
    } else {
        pCar->traction_control = 1;
    }

    if (pCar->keys.dec || (pCar->keys.acc && CAR_GEAR(pCar) == 0) ||
        pCar->joystick.dec > 0 || (pCar->joystick.acc > 0 && CAR_GEAR(pCar) == 0)) {
        if (pCar->joystick.dec > 0) {
            br_scalar bi = pCar->brake_increase;
            br_scalar q = (pCar->joystick.dec) / 65536;
            pCar->brake_force = pCar->initial_brake + bi * q;
        }
        if (pCar->brake_force == 0.f) {
            pCar->brake_force = pCar->initial_brake;
        } else {
            br_scalar bi = pCar->brake_increase;
            pCar->brake_force = pCar->brake_force + bi * pDt;
            if (pCar->brake_force > pCar->initial_brake + pCar->brake_increase) {
                pCar->brake_force = pCar->initial_brake + pCar->brake_increase;
            }
        }
    } else {
        pCar->brake_force = 0.f;
    }

    if (CAR_GEAR(pCar) == 0) {
        return;
    }
    pCar->acc_force = pCar->force_torque_ratio * pCar->torque / (float)CAR_GEAR(pCar);
    if (pCar->brake_force != 0.f) {
        pCar->revs = pCar->target_revs;
    } else if (pCar->revs - 1.f > pCar->target_revs || pCar->revs + 1.f < pCar->target_revs) {
        br_scalar lt = gZero / (pCar->speed_revs_ratio * pCar->collision_info->M) / (float)CAR_GEAR(pCar);
        pCar->acc_force = pCar->acc_force +
            (pCar->torque * pDt * 5000.0 + (pCar->revs - pCar->target_revs)) /
            ((lt + (float)CAR_GEAR(pCar) * (gZero / (pCar->force_torque_ratio * 0.0002))) * pDt);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern void C2_HOOK_FASTCALL CalcForce(tCar_spec* pCar, br_scalar pDt);
extern void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num);

// PARTIAL 96.45%: gravity block shared vs per-branch 1/6.9 fmul (single-statement form scores 96.31), branch displacements, and eax/ecx swap in the target_revs dot product (SIB/regalloc, both operand orders tried) - non-source
// FUNCTION: CARMA2_HW 0x004168c0
void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt) {
    br_scalar wheel_spin_force;
    br_scalar ts;
    int new_gear;
    br_vector3 v;
    br_scalar scale;
    br_scalar k;
    int i;

    if (pCar->dt >= 0.f) {
        pDt = pCar->dt;
    }
    if (pDt == 0.f) {
        return;
    }
    if (gCar_flying && pCar == gCar_to_view) {
        return;
    }

    pCar->new_skidding = 0;
    if (pCar->collision_info->water_d != 10000.f) {
        TestAutoSpecialVolume(pCar->collision_info);
    }
    MungeSpecialVolume(pCar->collision_info);
    pCar->car_master_actor->t.t.mat.m[3][0] = pCar->car_master_actor->t.t.mat.m[3][0] * 6.9f;
    pCar->car_master_actor->t.t.mat.m[3][1] = pCar->car_master_actor->t.t.mat.m[3][1] * 6.9f;
    pCar->car_master_actor->t.t.mat.m[3][2] = pCar->car_master_actor->t.t.mat.m[3][2] * 6.9f;

    if (pCar->field_0x18c8 != 0) {
        if (pCar->collision_info->disable_move_rotate == 0) {
            k = pDt * 10.f;
            if (pCar->collision_info->last_special_volume != NULL) {
                k *= pCar->collision_info->last_special_volume->gravity_multiplier;
                pCar->collision_info->v.v[1] = pCar->collision_info->v.v[1] - k * (1.0 / 6.9);
            } else {
                pCar->collision_info->v.v[1] = pCar->collision_info->v.v[1] - k * (1.0 / 6.9);
            }
        }
        if (pCar->field_0x18c8 == 2) {
            BrMatrix34TApplyV(&v, (br_vector3*)&gCamera->t.t.mat, &pCar->collision_info->actor->t.t.mat);
            scale = BrVector3Length(&v);
            if (scale > 2.3841858e-07f) {
                scale = gZero / scale;
                v.v[0] = v.v[0] * scale;
                v.v[1] = v.v[1] * scale;
                v.v[2] = v.v[2] * scale;
            } else {
                v.v[0] = 1.f;
                v.v[1] = 0.f;
                v.v[2] = 0.f;
            }
            if (pCar->keys.dec) {
                pCar->collision_info->omega.v[0] += v.v[0];
                pCar->collision_info->omega.v[1] += v.v[1];
                pCar->collision_info->omega.v[2] += v.v[2];
            }
            if (pCar->keys.acc) {
                pCar->collision_info->omega.v[0] -= v.v[0];
                pCar->collision_info->omega.v[1] -= v.v[1];
                pCar->collision_info->omega.v[2] -= v.v[2];
            }
            scale = BrVector3Length(&pCar->collision_info->velocity_car_space);
            if (scale > 2.3841858e-07f) {
                scale = gZero / scale;
                ts = pCar->collision_info->velocity_car_space.v[0];
                v.v[0] = ts * scale;
                ts = pCar->collision_info->velocity_car_space.v[1];
                v.v[1] = ts * scale;
                ts = pCar->collision_info->velocity_car_space.v[2];
                v.v[2] = ts * scale;
            } else {
                v.v[0] = 1.f;
                v.v[1] = 0.f;
                v.v[2] = 0.f;
            }
            if (pCar->keys.right) {
                pCar->collision_info->omega.v[0] += v.v[0];
                pCar->collision_info->omega.v[1] += v.v[1];
                pCar->collision_info->omega.v[2] += v.v[2];
            }
            if (pCar->keys.left) {
                pCar->collision_info->omega.v[0] -= v.v[0];
                pCar->collision_info->omega.v[1] -= v.v[1];
                pCar->collision_info->omega.v[2] -= v.v[2];
            }
            if (pCar->keys.brake) {
                pCar->collision_info->omega.v[0] = 0.f;
                pCar->collision_info->omega.v[1] = 0.f;
                pCar->collision_info->omega.v[2] = 0.f;
            }
        }
    } else if (pCar == NULL || pCar->driver < eDriver_net_human) {
        CalcForce(pCar, pDt);
    } else {
        if (gCar_flying && pCar->driver == eDriver_local_human) {
            pCar->acc_force = 0.f;
        } else {
            CalcEngineForce(pCar, pDt);
        }
        CalcForce(pCar, pDt);

        ts = -BrVector3Dot(&pCar->collision_info->v, (br_vector3*)&pCar->car_master_actor->t.t.mat.m[2]) * 6.9;
        pCar->target_revs = (CAR_GEAR(pCar) == 0) ? 0.f : (float)(ts / pCar->speed_revs_ratio / (double)CAR_GEAR(pCar));
        if (pCar->target_revs < 0.f) {
            pCar->target_revs = 0.f;
            CAR_GEAR(pCar) = 0;
        }
        if (!pCar->number_of_wheels_on_ground || ((pCar->wheel_slip & 2) + 1) != 0 || !CAR_GEAR(pCar)) {
            if (!pCar->number_of_wheels_on_ground) {
                wheel_spin_force = pCar->torque * pCar->force_torque_ratio;
            } else {
                wheel_spin_force = pCar->torque * pCar->force_torque_ratio
                    - CAR_GEAR(pCar) * pCar->acc_force;
            }
            if (CAR_GEAR(pCar) == 0) {
                if (pCar->revs > gF1000 && !pCar->keys.brake
                        && (pCar->keys.acc || pCar->joystick.acc > 0)
                        && gCountdown == 0) {
                    new_gear = pCar->keys.backwards ? -1 : 1;
                    CAR_GEAR(pCar) = new_gear;
                    pCar->target_revs = ts / pCar->speed_revs_ratio / (double)new_gear;
                }
                wheel_spin_force = pCar->torque * pCar->force_torque_ratio;
            } else if (CAR_GEAR(pCar) < 2 && (pCar->keys.dec || pCar->joystick.dec > 0)
                    && (float)fabs(ts) < 1.f && pCar->revs > gF1000) {
                CAR_GEAR(pCar) = -CAR_GEAR(pCar);
            }
            pCar->revs = pCar->revs
                - wheel_spin_force / pCar->force_torque_ratio * pDt * -5000.;
            if (pCar->traction_control && wheel_spin_force > 0.f) {
                if (pCar->revs > pCar->target_revs && CAR_GEAR(pCar) != 0
                        && pCar->target_revs > gF1000) {
                    pCar->revs = pCar->target_revs;
                }
            }
            if (pCar->revs <= 0.f) {
                pCar->revs = 0.f;
            }
        }
        while ((pCar->wheel_slip & 2) == 0 && pCar->target_revs > 6000.f
                && CAR_GEAR(pCar) < pCar->max_gear && CAR_GEAR(pCar) > 0
                && !pCar->just_changed_gear) {
            pCar->target_revs = pCar->target_revs * CAR_GEAR(pCar) / (CAR_GEAR(pCar) + 1);
            CAR_GEAR(pCar)++;
        }
        while (CAR_GEAR(pCar) > 1 && pCar->target_revs < 3000.f
                && !pCar->just_changed_gear) {
            pCar->target_revs = pCar->target_revs * CAR_GEAR(pCar) / (CAR_GEAR(pCar) - 1);
            CAR_GEAR(pCar)--;
        }
        if (pCar->revs < 200.f && pCar->target_revs < 200.f && CAR_GEAR(pCar) <= 1
                && !pCar->keys.acc && pCar->joystick.acc <= 0
                && !pCar->just_changed_gear) {
            CAR_GEAR(pCar) = 0;
        }
        if (pCar->just_changed_gear && pCar->revs < 6000.f && pCar->revs > 200.f
                && (CAR_GEAR(pCar) < 2 || pCar->revs >= 3000.f)) {
            pCar->just_changed_gear = 0;
        }
        if (pCar->revs >= 6000.f && (pCar->keys.acc || pCar->joystick.acc > 0)) {
            pCar->just_changed_gear = 0;
        }
    }

    pCar->car_master_actor->t.t.mat.m[3][0] = pCar->car_master_actor->t.t.mat.m[3][0] * (1.0f / 6.9f);
    pCar->car_master_actor->t.t.mat.m[3][1] = pCar->car_master_actor->t.t.mat.m[3][1] * (1.0f / 6.9f);
    pCar->car_master_actor->t.t.mat.m[3][2] = pCar->car_master_actor->t.t.mat.m[3][2] * (1.0f / 6.9f);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (i = 0; i < 4; i++) {
        SkidMark(pCar, i);
    }
}

// MoveAndCollideNonCar

// Body structure and names adapted from ApexWorm/rec2 branch match-control-car2 (GPL-3.0);
// the control flow and constants re-derived from the CARMA2_HW disassembly (keys bits 0x10000 left,
// 0x20000 right, 0x40000 acc, 0x80000 dec; 7.0 acc, +-0.125 turn step, 25.0 time steps, 0.05/(5+speed) sqrt term).
// FUNCTION: CARMA2_HW 0x00417180
void C2_HOOK_FAKE_THISCALL ControlCar2(tCar_spec* pCar, void* pUnused, br_scalar pDt) {
    br_scalar speed;

    (void)pUnused;
    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - pDt * -0.125);
        } else {
            speed = BrVector3Length(&pCar->collision_info->v);
            pCar->turn_speed = (float)(pCar->turn_speed - (pDt * 25.0f) * (0.05f / (speed + 5.0f)) * -0.5f);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - pDt * 0.125);
        } else {
            speed = BrVector3Length(&pCar->collision_info->v);
            pCar->turn_speed = (float)(pCar->turn_speed - (pDt * 25.0f) * (0.05f / (speed + 5.0f)) * 0.5f);
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

// Body structure and names adapted from ApexWorm/rec2 branch match-control-car5 (GPL-3.0);
// the control flow and constants re-derived from the CARMA2_HW disassembly (keys bits 0x10000 left,
// 0x20000 right, 0x40000 acc, 0x80000 dec, 0x800000 holdw; 7.0 acc, +-0.0625 turn step, 25.0 time steps).
// FUNCTION: CARMA2_HW 0x00417a20
void C2_HOOK_FAKE_THISCALL ControlCar5(tCar_spec* pCar, void* pUnused, br_scalar pDt) {
    br_scalar speed;

    (void)pUnused;
    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - pDt * -0.0625);
        } else {
            speed = BrVector3Length(&pCar->collision_info->v);
            pCar->turn_speed = (float)(pCar->turn_speed - (pDt * 25.0f) * (0.05f / (speed + 5.0f)) * -0.5f * 0.5);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - pDt * 0.0625);
        } else {
            speed = BrVector3Length(&pCar->collision_info->v);
            pCar->turn_speed = (float)(pCar->turn_speed - (pDt * 25.0f) * (0.05f / (speed + 5.0f)) * 0.5f * 0.5);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
        if (pCar->curvature < 0.0f && !pCar->keys.holdw) {
            speed = BrVector3Length(&pCar->collision_info->v);
            pCar->curvature = (float)(pCar->curvature - (pDt * 25.0f) * (0.05f / (speed + 5.0f)) * -0.5f * 4.0);
            if (pCar->curvature > 0.0f) {
                pCar->curvature = 0.0f;
            }
        } else if (pCar->curvature > 0.0f && !pCar->keys.holdw) {
            speed = BrVector3Length(&pCar->collision_info->v);
            pCar->curvature = (float)(pCar->curvature - (pDt * 25.0f) * (0.05f / (speed + 5.0f)) * 0.5f * 4.0);
            if (pCar->curvature < 0.0f) {
                pCar->curvature = 0.0f;
            }
        }
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
    pCar->keys.left = 1;
}

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

extern void C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

// GLOBAL: CARMA2_HW 0x006a4688
br_vector3 gVector3_006a4688;
tQueued_smash gQueued_smashes[20];
extern int gCount_queued_smashes;
extern int gSize_powerup_queue;
extern tPowerup_queue_item gPowerup_queue[50];
extern br_matrix34 gShrapnel_shower_matrix;
extern int gINT_006a3334;
extern int gCount_shrapnel_shower_calls;
extern void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex);
extern void C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* actor);
extern void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tPhysics_object* pObject, void* pArg2, int pArg3, void* pArg4, void* pArg5, int pArg6, void* pArg7, void* pArg8, br_actor* pActor, int pArg10, int pArg11, void* pArg12, int pArg13, int pArg14, float pArg15, void* pArg16, int pArg17);

// FUNCTION: CARMA2_HW 0x004ecfb0
void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) {
    int i;
    int j;
    tQueued_smash* q;
    tPhysics_object* object;
    br_matrix34 local_30;
    br_model* model;
    unsigned char* local_70;
    unsigned char* puVar5;
    tU32 uVar8;
    int uVar2;
    int uVar7;
    int local_50;
    int iVar4;
    int iVar9;
    int iVar10;
    int iVar12;
    int iVar17;

    local_50 = 0;
    if (0 < gCount_queued_smashes) {
    q = gQueued_smashes;
    do {
        if (q->actor->model != NULL) {
            if (q->field_0x40 == 0) {
                gINT_006a3334 = 0;
            } else {
                gShrapnel_shower_matrix = q->field_0x4c;
                gVector3_006a4688 = q->field_0x7c;
                gINT_006a3334 = 1;
            }
            uVar2 = *(int*)((char*)q + 0x3c);
            iVar12 = *(int*)((char*)q + 0x38);
            object = (tPhysics_object*)q->field_0x10;
            uVar8 = GetTotalTime();
            iVar9 = *(int*)((char*)object + 0x4c);
            iVar10 = *(int*)((char*)object + 0x48) - 1;
            if (iVar9 < iVar10) {
                if (*(int*)((char*)object + 0x48) <= iVar12) {
                    iVar12 = iVar10;
                }
                BrActorToActorMatrix34(&local_30, q->actor, gUniverse_actor);
                ActuallySmashCarPart(q->car, (tCar_crush_smashable_part*)object, iVar12);
                uVar7 = gCount_shrapnel_shower_calls;
                iVar4 = *(int*)((char*)object + 0x50);
                iVar10 = iVar9 + 1;
                if (iVar10 <= iVar12) {
                    local_70 = (unsigned char*)(iVar4 + iVar10 * 0x2c4 + 700);
                    iVar10 = (iVar12 - iVar10) + 1;
                    do {
                        if (((*local_70 & 1) == 0) || (iVar12 - iVar9 == 1)) {
                            SmashItIntoVerySmallPiecesIndeed(object, (void*)(iVar4 + 0x14 + iVar12 * 0x2c4), (int)object ^ (int)uVar8, (char*)q + 0x14, (char*)q + 0x2c, (int)uVar8, (char*)q + 0x2c, (char*)q + 0x2c, q->actor, (int)uVar8, uVar7, &local_30, 0, 0,
                                    (float)sqrt(*(float*)((char*)q + 0x20) * *(float*)((char*)q + 0x20) +
                                                *(float*)((char*)q + 0x24) * *(float*)((char*)q + 0x24) +
                                                *(float*)((char*)q + 0x28) * *(float*)((char*)q + 0x28)),
                                    (char*)q + 0x20, uVar2);
                        }
                        local_70 = local_70 + 0x2c4;
                        iVar10 = iVar10 - 1;
                    } while (iVar10 != 0);
                }
                if ((*(int*)((char*)object + 0x44) != 0) &&
                    (puVar5 = *(unsigned char**)(*(int*)((char*)object + 0x44) + 4)) != NULL) {
                    iVar17 = *(int*)(*(int*)((char*)object + 0x50) + 0x2c0 + *(int*)((char*)object + 0x4c) * 0x2c4);
                    if (iVar17 == 0) {
                        *puVar5 = 0x58;
                    } else if (iVar17 == 1) {
                        *puVar5 = 0x3e;
                    } else if (iVar17 == 2) {
                        *puVar5 = 0x3f;
                    }
                }
            }
        }
        local_50 += 1;
        q += 1;
    } while (local_50 < gCount_queued_smashes);
    }
    for (i = 0; i < gCount_queued_smashes; i++) {
        q = &gQueued_smashes[i];
        if (q->field_0x44 != 0) {
            model = q->actor->model;
            DRModelUpdateAndKevificateMaterials(model, BR_MODU_ALL);
            for (j = i + 1; j < gCount_queued_smashes; j++) {
                if (model == gQueued_smashes[j].actor->model) {
                    q->field_0x44 = 0;
                }
            }
        }
    }
    if (pFlush_powerups == 0) {
        for (i = 0; i < gSize_powerup_queue; i++) {
            GotPowerup(gPowerup_queue[i].car, gPowerup_queue[i].powerup_index);
            MayQueuePowerupRespawn(gPowerup_queue[i].powerup_index, gPowerup_queue[i].actor);
        }
        gSize_powerup_queue = 0;
    }
    gCount_queued_smashes = 0;
}
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
// (GPL-3.0); the original at 0x004c2c30
// FUNCTION: CARMA2_HW 0x004c2c30
void C2_HOOK_FASTCALL PhysicsObjectApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) {
    br_vector3 t;

    t.v[1] = pTorque->v[1] * 0.0210039914f;
    t.v[2] = pTorque->v[2] * 0.0210039914f;
    t.v[0] = pTorque->v[0] * 0.0210039914f;
    pObject->omega.v[1] += t.v[1] / pObject->I.v[1];
    pObject->omega.v[2] += t.v[2] / pObject->I.v[2];
    pObject->omega.v[0] += t.v[0] / pObject->I.v[0];
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static br_scalar stop_timer;
static br_scalar slide_dist;

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
            && *(int*)((tNon_car_spec*)objs[0]->owner)->field_0xf8 == 0
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

typedef struct tCollision_contact {
    float impulse;
    tPhysics_object* object1;
    tPhysics_object* object2;
    void* field_0xc;
    float field_0x10;
    br_vector3 r1;
    br_vector3 r2;
    br_vector3 normal;
    br_vector3 field_0x38;
    br_vector3 impulse1;
    br_vector3 impulse2;
    int type;
} tCollision_contact;

// These five callees of ProcessForcesCallback are not decompiled yet. They are
// deliberately inert (instead of NOT_IMPLEMENTED) because they are reached on every
// physics tick with contacts, and aborting there would kill the race path.
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* FullyDetachSnappedBits 0x00431310 is implemented at the end of this file: its TU
   position decides the x87 evaluation order of the dot product, and only the file-end
   position reproduces the original. */
/* Force feedback: turns the collision point into a direction and hands the impulse to
   the joystick layer.
   PARTIAL 65.31%: the original keeps the three position components in x87 registers
   across the length and the normalise (fld st(2) copies), while MSVC 5.0 spills v[1]
   to the stack frame for every source shape tried; the register dance that follows
   differs with it. Semantics are identical. */
// FUNCTION: CARMA2_HW 0x0045c910
void C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, float pImpulse) {
    extern float gForce_feedback_upper_limit;
    extern float gForce_feedback_lower_limit;
    extern const double gJoystick_collision_force_scale;
    extern const float gJoystick_collision_force_min_length;
    extern const double gJoystick_collision_force_one;
    extern const float gJoystick_collision_force_one_f;
    extern const float gJoystick_collision_force_zero_f;
    extern const double gJoystick_collision_force_radians_to_degrees;
    extern int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(int pMagnitude, int pDirection);
    float force_length;
    float inverse_force_length;
    float impulse;
    br_vector3 normalized_position;
    register int angle_degrees;
    int magnitude;
    int flip;
    float force_x;
    float force_y;
    float force_z;

    (void)pCar;
    (void)pForce;
    impulse = pImpulse;
    if (pImpulse > gForce_feedback_upper_limit) {
        impulse = gForce_feedback_upper_limit;
    }
    if (impulse < gForce_feedback_lower_limit) {
        impulse = gForce_feedback_lower_limit;
    }
    magnitude = (int)(impulse * (gJoystick_collision_force_scale / gForce_feedback_upper_limit));
    force_x = pPoint->v[0];
    force_y = pPoint->v[1];
    force_z = pPoint->v[2];
    force_length = sqrt(force_x * force_x + force_y * force_y + force_z * force_z);
    if (force_length > gJoystick_collision_force_min_length) {
        inverse_force_length = gJoystick_collision_force_one / force_length;
        normalized_position.v[0] = force_x * inverse_force_length;
        normalized_position.v[1] = force_y * inverse_force_length;
        normalized_position.v[2] = force_z * inverse_force_length;
    } else {
        normalized_position.v[0] = gJoystick_collision_force_one_f;
        normalized_position.v[1] = gJoystick_collision_force_zero_f;
        normalized_position.v[2] = gJoystick_collision_force_zero_f;
    }
    flip = (normalized_position.v[0] + gJoystick_collision_force_zero_f) <= gJoystick_collision_force_zero_f;
    angle_degrees = (int)(acos(gJoystick_collision_force_zero_f - normalized_position.v[2] * gJoystick_collision_force_one_f) * gJoystick_collision_force_radians_to_degrees);
    if (angle_degrees < 45) {
        angle_degrees = 90 - angle_degrees;
    }
    if (angle_degrees > 135) {
        angle_degrees = 270 - angle_degrees;
    }
    if (flip) {
        angle_degrees = 360 - angle_degrees;
    }
    SetJoystickFFBCollisionForce(magnitude, angle_degrees * 100);
}
// STUB: CARMA2_HW 0x004d2320
void C2_HOOK_FASTCALL FUN_004d2320(tPhysics_object* pObject1, tPhysics_object* pObject2, tFace_ref* pFace, float pImpulse, br_vector3* pPoint) {
    (void)pObject1; (void)pObject2; (void)pFace; (void)pImpulse; (void)pPoint;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


// FUNCTION: CARMA2_HW 0x0041c1b0
int C2_HOOK_FASTCALL ProcessForcesCallback(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    int i;
    int k;
    int num_impacts;
    int* smash_index;
    union {
        int smash_indices[20];
        unsigned char flags[80];
    } smash_buffer;
    br_scalar max_impulse;
    tFace_ref* face;
    tPhysics_object* object1;
    tPhysics_object* object2;
    tPhysics_object* object;
    br_actor* actor;
    br_vector3 scaled_normal;
    br_vector3 contact_offset;
    br_vector3 pair_point;
    br_vector3 pair_world_velocity;
    br_scalar pair_impulse;
    br_scalar pair_result;
    br_vector3 point;
    br_vector3 world_normal;
    br_scalar result;
    br_vector3 point1;
    br_vector3 point2;
    br_vector3 force1;
    br_vector3 force2;
    br_vector3 chain1[4];
    br_vector3 chain2[4];
    br_vector3 world_force;
    br_vector3 world_point;
    br_vector3 zero_velocity;
    br_vector3 recoil;
    float impulse;
    int j;
    br_scalar sparkiness;
    int chain_count;
    int effects;
    int material;
    int volume;
    br_vector3 velocity;
    br_vector3 world_velocity;
    // GLOBAL: CARMA2_HW 0x00679400
    static tS3_sound_tag scrape_tag;
    // GLOBAL: CARMA2_HW 0x00679404
    static tS3_volume last_scrape_vol;
    extern tS3_outlet* gCar_outlet;
    extern int gScrape_sound_id[3];
    extern tSmashable_item_spec* gSmashable_track_indexable_triggers[100];
    extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);
    extern float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);
    extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);
    extern int C2_HOOK_FASTCALL ScaleCollisionImpulses(tCollision_contact* pContacts, float* pImpulses, int pCount, int* pSmash_indices, int pNum_impacts, float pMax_impulse);
    extern int C2_HOOK_FASTCALL TrySplitCarOnVerticalForce(tCollision_contact* pContacts, float* pImpulses, int pCount);
    extern int C2_HOOK_FASTCALL FullyDetachSnappedBits(tCollision_contact* pContacts, float* pImpulses, int pCount);
    extern void C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, float pImpulse);
    extern void C2_HOOK_FASTCALL FUN_004d2320(tPhysics_object* pObject1, tPhysics_object* pObject2, tFace_ref* pFace, float pImpulse, br_vector3* pPoint);
    extern void C2_HOOK_FASTCALL CreateSparkShower(br_vector3* pPosition, br_vector3* pVelocity, br_vector3* pForce, tNon_car_spec* pOwner1, tNon_car_spec* pOwner2);
    extern void C2_HOOK_FASTCALL CreateSparks(const br_vector3* pPosition, br_vector3* pVelocity, const br_vector3* pForce, br_scalar pStrength, tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);
    extern void C2_HOOK_FASTCALL CollideShitMine(tPhysics_object* pObject1, tPhysics_object* pObject2, float pCollision_speed);
    extern void C2_HOOK_FASTCALL CrushAndDamageCar(tCar_spec* pCar1, br_vector3* pPoint, br_vector3* pForce, tCar_spec* pCar2, int pChain_count, br_vector3* pChain, br_actor* pActor);
    extern void C2_HOOK_FASTCALL QueueDroneCrush(tPhysics_object* pObject, int pChain_count, br_vector3* pChain, br_vector3* pForce);
    extern int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);
    extern int C2_HOOK_FASTCALL DRS3ChangeVolume(int pChannel_id, int pVolume);
    extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
    extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

    num_impacts = 0;
    max_impulse = -1.f;
    smash_index = smash_buffer.smash_indices;
    for (i = 0; i < pCount; i++) {
        if (pImpulses[i] <= 0.02f) {
            continue;
        }
        if (pContacts[i].object2 != NULL) {
            goto pair_path;
        }
        face = (tFace_ref*)pContacts[i].field_0xc;
        if (face == NULL) {
            goto pair_path;
        }
        if (face->material != NULL
                && face->material->identifier != NULL
                && strlen(face->material->identifier) == 0xb
                && face->material->identifier[5] == '|') {
            goto track_path;
        }
        if (face->a == NULL
                || face->a->identifier == NULL) {
            goto pair_path;
        }
        if (strlen(face->a->identifier) == 0xb
                && face->a->identifier[5] == '|') {
            goto track_path;
        }
        if (((unsigned char*)face->a->identifier)[1] == 0xa3) {
            goto track_path;
        }
        goto pair_path;

track_path:
        BrVector3Scale(&scaled_normal, &pContacts[i].normal, pImpulses[i]);
        BrVector3Add(&contact_offset, &pContacts[i].object1->cmpos, &pContacts[i].r1);
        if (pContacts[i].type != 0) {
            BrMatrix34ApplyP(&point, &contact_offset, &pContacts[i].object1->transform_matrix);
            object = pContacts[i].object1;
            goto smash_path;
        }
        BrVector3Copy(&point, &pContacts[i].r2);
        {
            br_scalar d;

            d = DistanceFromFaceND(&point, &face->normal, face->d);
            if (fabs(d) > 0.5) {
                goto nearest_vertex;
            }
            {
                br_vector3 projection;
                int dominant;
                int axis_a;
                int axis_b;
                br_scalar e1s, e1l, e2s, e2l, ps, pl;
                br_scalar bary_a, bary_b;
                br_vector3 abs_normal;
                br_vector3 normal;

                normal = face->normal;
                abs_normal.v[0] = (float)fabs(normal.v[0]);
                abs_normal.v[1] = (float)fabs(normal.v[1]);
                abs_normal.v[2] = (float)fabs(normal.v[2]);
                BrVector3Scale(&projection, &normal, -d);
                BrVector3Add(&projection, &projection, &point);
                dominant = 0;
                if (abs_normal.v[0] < abs_normal.v[1]) {
                    dominant = 1;
                }
                if (abs_normal.v[dominant] < abs_normal.v[2]) {
                    dominant = 2;
                }
                if (dominant == 0) {
                    axis_b = 2;
                    axis_a = 1;
                } else if (dominant == 1) {
                    axis_b = 2;
                    axis_a = 0;
                } else {
                    axis_b = 1;
                    axis_a = 0;
                }
                e1s = face->v[1].v[axis_a] - face->v[0].v[axis_a];
                e1l = face->v[1].v[axis_b] - face->v[0].v[axis_b];
                e2s = face->v[2].v[axis_a] - face->v[0].v[axis_a];
                e2l = face->v[2].v[axis_b] - face->v[0].v[axis_b];
                ps = projection.v[axis_a] - face->v[0].v[axis_a];
                pl = projection.v[axis_b] - face->v[0].v[axis_b];
                if ((float)fabs(e1l) <= 2.384185791015625e-07) {
                    bary_b = pl / e2l;
                    bary_a = (ps - bary_b * e2s) / e1s;
                } else {
                    br_scalar det;

                    det = e2s * e1l - e1s * e2l;
                    if (det == 0.0f) {
                        goto nearest_vertex;
                    }
                    bary_b = (pl * e1s - ps * e1l) / det;
                    bary_a = (pl - bary_b * e2l) / e1l;
                }
                if (bary_a >= 0.0001 && bary_b >= 0.0001 && bary_a + bary_b <= 0.999) {
                    object = pContacts[i].object1;
                    goto smash_path;
                }
            }
nearest_vertex:
            {
                br_vector3 delta;
                br_scalar dist[3];
                int closest;
                int other1;
                int other2;

                BrVector3Sub(&delta, &point, &face->v[0]);
                dist[0] = BrVector3LengthSquared(&delta);
                BrVector3Sub(&delta, &point, &face->v[1]);
                dist[1] = BrVector3LengthSquared(&delta);
                BrVector3Sub(&delta, &point, &face->v[2]);
                dist[2] = BrVector3LengthSquared(&delta);
                if (dist[0] < dist[1] && dist[0] < dist[2]) {
                    closest = 0;
                    other1 = 1;
                    other2 = 2;
                } else if (dist[1] < dist[2]) {
                    closest = 1;
                    other1 = 0;
                    other2 = 2;
                } else {
                    closest = 2;
                    other1 = 1;
                    other2 = 0;
                }
                point.v[0] = face->v[closest].v[0] * 0.98 - (face->v[other1].v[0] + face->v[other2].v[0]) * -0.01;
                point.v[1] = face->v[closest].v[1] * 0.98 - (face->v[other1].v[1] + face->v[other2].v[1]) * -0.01;
                point.v[2] = face->v[closest].v[2] * 0.98 - (face->v[other1].v[2] + face->v[other2].v[2]) * -0.01;
            }
        }
        object = pContacts[i].object1;
        goto smash_path;

pair_path:
        object2 = pContacts[i].object2;
        if (object2 == NULL) {
            continue;
        }
        object1 = pContacts[i].object1;
        object = NULL;
        actor = object1->actor;
        if (actor != NULL
                && actor->identifier != NULL
                && actor->identifier[0] == '&'
                && gSmashable_track_indexable_triggers[((actor->identifier[1] - '0') * 10 + (actor->identifier[2] - '0'))] != NULL) {
            BrVector3Scale(&scaled_normal, &pContacts[i].normal, pImpulses[i]);
            BrVector3Add(&contact_offset, &object1->cmpos, &pContacts[i].r1);
            object = object1;
        } else {
            actor = object2->actor;
            if (actor != NULL
                    && actor->identifier != NULL
                    && actor->identifier[0] == '&'
                    && gSmashable_track_indexable_triggers[((actor->identifier[1] - '0') * 10 + (actor->identifier[2] - '0'))] != NULL) {
                BrVector3Scale(&scaled_normal, &pContacts[i].field_0x38, pImpulses[i]);
                BrVector3Add(&contact_offset, &object2->cmpos, &pContacts[i].r2);
                object = object2;
            }
        }
        if (object == NULL) {
            continue;
        }
        pair_impulse = pImpulses[i];
        velocity.v[0] = object->omega.v[1] * contact_offset.v[2] - object->omega.v[2] * contact_offset.v[1];
        velocity.v[1] = object->omega.v[2] * contact_offset.v[0] - object->omega.v[0] * contact_offset.v[2];
        velocity.v[2] = object->omega.v[0] * contact_offset.v[1] - object->omega.v[1] * contact_offset.v[0];
        BrVector3Add(&velocity, &velocity, &object->velocity_car_space);
        BrMatrix34ApplyV(&pair_world_velocity, &velocity, &object->transform_matrix);
        BrMatrix34ApplyV(&world_normal, &scaled_normal, &object->transform_matrix);
        BrMatrix34ApplyP(&pair_point, &contact_offset, &object->transform_matrix);
        world_normal.v[0] *= 6.9;
        world_normal.v[1] *= 6.9;
        world_normal.v[2] *= 6.9;
        pair_result = BashObject(object, object->actor, pair_impulse, &pair_point, &pair_world_velocity, &world_normal, 0, 0);
        if (pair_result != -1.f && max_impulse == -1.f) {
            max_impulse = pair_result;
        }
        if (max_impulse != -1.f && num_impacts < 20) {
            *smash_index++ = i;
            num_impacts++;
        }
        continue;

smash_path:
        velocity.v[0] = object->omega.v[1] * contact_offset.v[2] - object->omega.v[2] * contact_offset.v[1];
        velocity.v[1] = object->omega.v[2] * contact_offset.v[0] - object->omega.v[0] * contact_offset.v[2];
        velocity.v[2] = object->omega.v[0] * contact_offset.v[1] - object->omega.v[1] * contact_offset.v[0];
        BrVector3Add(&velocity, &velocity, &object->velocity_car_space);
        BrMatrix34ApplyV(&world_velocity, &velocity, &object->transform_matrix);
        BrMatrix34ApplyV(&world_normal, &scaled_normal, &object->transform_matrix);
        result = SmashEnvironment(object, (undefined4*)face, pImpulses[i], &point, &world_velocity, &world_normal, 1, 0);
        if (result != -1.f && max_impulse == -1.f) {
            max_impulse = result;
        }
        if (max_impulse != -1.f && num_impacts < 20) {
            *smash_index++ = i;
            num_impacts++;
        }
        continue;
    }
    FlushSmashQueue(1);
    if (num_impacts != 0) {
        ScaleCollisionImpulses(pContacts, pImpulses, pCount, smash_buffer.smash_indices, num_impacts, max_impulse);
        return 0;
    }
    if (TrySplitCarOnVerticalForce((void*)pContacts, pImpulses, pCount) != 0) {
        return 0;
    }
    if (FullyDetachSnappedBits(pContacts, pImpulses, pCount) != 0) {
        return 0;
    }
    for (k = 0; k < pCount; k++) {
        smash_buffer.flags[k] = 0;
    }
    for (k = 0; k < pCount; k++) {
        if (smash_buffer.flags[k] != 0) {
            continue;
        }
        effects = 0;
        BrVector3Scale(&point1, &pContacts[k].r1, pImpulses[k]);
        BrVector3Scale(&force1, &pContacts[k].normal, pImpulses[k]);
        object1 = pContacts[k].object1;
        object2 = pContacts[k].object2;
        BrVector3Add(&chain1[0], &object1->cmpos, &pContacts[k].r1);
        if (object2 != NULL) {
            BrVector3Scale(&point2, &pContacts[k].r2, pImpulses[k]);
            BrVector3Scale(&force2, &pContacts[k].field_0x38, -pImpulses[k]);
            BrVector3Add(&chain2[0], &object2->cmpos, &pContacts[k].r2);
        }
        smash_buffer.flags[k] = 1;
        impulse = pImpulses[k];
        chain_count = 1;
        j = k + 1;
        if (j >= pCount) {
            goto merge_done;
        }
        while (chain_count != 4) {
            if (smash_buffer.flags[j] != 0) {
                goto next_j;
            }
            if (pContacts[j].object1 == object1 && pContacts[j].object2 == object2) {
                if (BrVector3Dot(&pContacts[j].normal, &pContacts[k].normal) > 0.707107f) {
                    if (pImpulses[j] != 0.f) {
                        point1.v[0] += pContacts[j].r1.v[0] * pImpulses[j];
                        point1.v[1] += pContacts[j].r1.v[1] * pImpulses[j];
                        point1.v[2] += pContacts[j].r1.v[2] * pImpulses[j];
                        force1.v[0] += pContacts[j].normal.v[0] * pImpulses[j];
                        force1.v[1] += pContacts[j].normal.v[1] * pImpulses[j];
                        force1.v[2] += pContacts[j].normal.v[2] * pImpulses[j];
                        impulse += pImpulses[j];
                    }
                    BrVector3Add(&chain1[chain_count], &pContacts[j].object1->cmpos, &pContacts[j].r1);
                    if (pContacts[j].object2 != NULL) {
                        if (pImpulses[j] != 0.f) {
                            point2.v[0] += pContacts[j].r2.v[0] * pImpulses[j];
                            point2.v[1] += pContacts[j].r2.v[1] * pImpulses[j];
                            point2.v[2] += pContacts[j].r2.v[2] * pImpulses[j];
                            force2.v[0] += pContacts[j].field_0x38.v[0] * -pImpulses[j];
                            force2.v[1] += pContacts[j].field_0x38.v[1] * -pImpulses[j];
                            force2.v[2] += pContacts[j].field_0x38.v[2] * -pImpulses[j];
                        }
                        BrVector3Add(&chain2[chain_count], &pContacts[j].object2->cmpos, &pContacts[j].r2);
                    }
                    chain_count++;
                    smash_buffer.flags[j] = 1;
                }
            } else if (pContacts[j].object2 == object1 && pContacts[j].object1 == object2) {
                if (BrVector3Dot(&pContacts[j].field_0x38, &pContacts[k].normal) < -0.707107f) {
                    if (pImpulses[j] != 0.f) {
                        point1.v[0] += pContacts[j].r2.v[0] * pImpulses[j];
                        point1.v[1] += pContacts[j].r2.v[1] * pImpulses[j];
                        point1.v[2] += pContacts[j].r2.v[2] * pImpulses[j];
                        force1.v[0] += pContacts[j].field_0x38.v[0] * -pImpulses[j];
                        force1.v[1] += pContacts[j].field_0x38.v[1] * -pImpulses[j];
                        force1.v[2] += pContacts[j].field_0x38.v[2] * -pImpulses[j];
                        point2.v[0] += pContacts[j].r1.v[0] * pImpulses[j];
                        point2.v[1] += pContacts[j].r1.v[1] * pImpulses[j];
                        point2.v[2] += pContacts[j].r1.v[2] * pImpulses[j];
                        force2.v[0] += pContacts[j].normal.v[0] * pImpulses[j];
                        force2.v[1] += pContacts[j].normal.v[1] * pImpulses[j];
                        force2.v[2] += pContacts[j].normal.v[2] * pImpulses[j];
                        impulse += pImpulses[j];
                    }
                    chain1[chain_count].v[0] = pContacts[j].object2->cmpos.v[0] + pContacts[j].r2.v[0];
                    chain1[chain_count].v[1] = pContacts[j].object2->cmpos.v[1] + pContacts[j].r2.v[1];
                    chain1[chain_count].v[2] = pContacts[j].object2->cmpos.v[2] + pContacts[j].r2.v[2];
                    chain2[chain_count].v[0] = pContacts[j].object1->cmpos.v[0] + pContacts[j].r1.v[0];
                    chain2[chain_count].v[1] = pContacts[j].object1->cmpos.v[1] + pContacts[j].r1.v[1];
                    chain2[chain_count].v[2] = pContacts[j].object1->cmpos.v[2] + pContacts[j].r1.v[2];
                    chain_count++;
                    smash_buffer.flags[j] = 1;
                }
            }
next_j:
            j++;
            if (j >= pCount) {
                break;
            }
        }
merge_done:
        point1.v[0] = point1.v[0] / impulse;
        point1.v[1] = point1.v[1] / impulse;
        point1.v[2] = point1.v[2] / impulse;
        BrVector3Add(&point1, &point1, &object1->cmpos);
        if (object2 != NULL) {
            point2.v[0] = point2.v[0] / impulse;
            point2.v[1] = point2.v[1] / impulse;
            point2.v[2] = point2.v[2] / impulse;
            BrVector3Add(&point2, &object2->cmpos, &point2);
        }
        if (object1 != NULL && object1->owner != NULL && object1->flags_0x238 == 1
                && ((tCar_spec*)object1->owner)->driver == eDriver_local_human) {
            if (impulse > 0.02f) {
                FUN_0045c910((tCar_spec*)object1->owner, &point1, &force1, impulse);
            }
        } else if (object2 != NULL && object2->owner != NULL && object2->flags_0x238 == 1
                && ((tCar_spec*)object2->owner)->driver == eDriver_local_human) {
            if (impulse > 0.02f) {
                FUN_0045c910((tCar_spec*)object2->owner, &point2, &force2, impulse);
            }
        }
        if (object1 != NULL && object1->owner != NULL && object1->flags_0x238 == 1
                && impulse > 0.02f) {
            if (object2 != NULL && object2->owner != NULL && object2->flags_0x238 == 1) {
                CrushAndDamageCar((tCar_spec*)object1->owner, &point1, &force1, (tCar_spec*)object2->owner, chain_count, chain1, object1->actor);
                CrushAndDamageCar((tCar_spec*)object2->owner, &point2, &force2, (tCar_spec*)object1->owner, chain_count, chain2, object2->actor);
                CrashEarnings((tCar_spec*)object1->owner, (tCar_spec*)object2->owner);
                if (BrVector3LengthSquared(&object1->v) > 0.0009 || BrVector3LengthSquared(&object2->v) > 0.0009) {
                    BrVector3Scale(&world_velocity, &object2->v, 2.f);
                    BrVector3Scale(&world_velocity, &world_velocity, 0.5f);
                    BrMatrix34ApplyV(&world_force, &force1, &object1->transform_matrix);
                    BrMatrix34ApplyP(&world_point, &point1, &object1->transform_matrix);
                    world_force.v[0] *= 6.9;
                    world_force.v[1] *= 6.9;
                    world_force.v[2] *= 6.9;
                    effects = 1;
                    CreateSparkShower(&world_point, &world_velocity, &world_force, (tNon_car_spec*)object1->owner, (tNon_car_spec*)object2->owner);
                    CrashNoise(&world_force, &world_point, 0);
                }
            } else {
                CrushAndDamageCar((tCar_spec*)object1->owner, &point1, &force1, NULL, chain_count, chain1, object1->actor);
                if (pContacts[k].object2 == NULL && pContacts[k].field_0xc != NULL) {
                    if (object1->v.v[1] < -2.89855f) {
                        force1.v[0] *= 2.f;
                        force1.v[1] *= 2.f;
                        force1.v[2] *= 2.f;
                    }
                    CrashEarnings((tCar_spec*)object1->owner, NULL);
                    if (BrVector3LengthSquared(&object1->v) > 0.0009) {
                        BrVector3Cross(&velocity, &object1->omega, &point1);
                        BrVector3Accumulate(&velocity, &object1->velocity_car_space);
                        BrMatrix34ApplyV(&world_velocity, &velocity, &object1->transform_matrix);
                        BrMatrix34ApplyV(&world_force, &force1, &object1->transform_matrix);
                        BrMatrix34ApplyP(&world_point, &point1, &object1->transform_matrix);
                        world_force.v[0] *= 6.9;
                        world_force.v[1] *= 6.9;
                        world_force.v[2] *= 6.9;
                        material = (signed char)((tFace_ref*)pContacts[k].field_0xc)->material->identifier[0] - 0x2f;
                        if (material < 0 || material > 9) {
                            material = 0;
                        }
                        effects = 1;
                        sparkiness = gFriction_materials[material].sparkiness;
                        if ((object1->owner != NULL && ((tCar_spec*)object1->owner)->driver > 5)
                                || (((tNon_car_spec*)object1->owner)->flags & 0x200000) != 0) {
                            CreateSparks(&world_point, &world_velocity, &world_force, sparkiness, (tCar_spec*)object1->owner);
                        }
                        CrashNoise(&world_force, &world_point, material);
                        volume = (int)((int)(BrVector3Length(&world_velocity) * 7.0f) * 6.9);
                        if (gFriction_materials[material].scrape_noise_index != -1) {
                            if (!(scrape_tag != 0 && DRS3SoundStillPlaying(scrape_tag)) && volume > 30) {
                                last_scrape_vol = volume;
                                BrVector3Set(&zero_velocity, 0.f, 0.f, 0.f);
                                scrape_tag = DRS3StartSound3D(gCar_outlet,
                                    gScrape_sound_id[IRandomBetween(0, 2)],
                                    &world_point, &zero_velocity, 1, volume,
                                    IRandomBetween(0xc000, 0x14000), 0x10000);
                            } else if (volume > last_scrape_vol) {
                                DRS3ChangeVolume(scrape_tag, volume);
                                last_scrape_vol = volume;
                            }
                        }
                    }
                }
            }
        }
        if (impulse > 0.0005f) {
            if (pContacts[k].object1 != NULL && pContacts[k].object1->owner != NULL && pContacts[k].object1->flags_0x238 >= 2
                    && pContacts[k].object1->flags_0x238 <= 4) {
                if (pContacts[k].object2 == NULL || pContacts[k].object2->owner == NULL || pContacts[k].object2->flags_0x238 < 2
                        || pContacts[k].object2->flags_0x238 > 4) {
                    BrMatrix34ApplyP(&world_point, &point1, &pContacts[k].object1->transform_matrix);
                    FUN_004d2320(pContacts[k].object1, pContacts[k].object2, (tFace_ref*)pContacts[k].field_0xc, pImpulses[k], &world_point);
                }
            }
            if (pContacts[k].object2 != NULL && pContacts[k].object2->owner != NULL && pContacts[k].object2->flags_0x238 >= 2
                    && pContacts[k].object2->flags_0x238 <= 4) {
                if (pContacts[k].object1 == NULL || pContacts[k].object1->owner == NULL || pContacts[k].object1->flags_0x238 < 2
                        || pContacts[k].object1->flags_0x238 > 4) {
                    BrMatrix34ApplyP(&world_point, &point2, &pContacts[k].object2->transform_matrix);
                    FUN_004d2320(pContacts[k].object2, pContacts[k].object1, (tFace_ref*)pContacts[k].field_0xc, pImpulses[k], &world_point);
                }
            }
        }
        if (impulse > 0.06f) {
            if (object1 != NULL && object1->flags_0x238 == 0x10) {
                QueueDroneCrush(object1, chain_count, chain1, &force1);
                BrMatrix34ApplyV(&world_force, &force1, &object1->transform_matrix);
                BrMatrix34ApplyP(&world_point, &point1, &object1->transform_matrix);
                world_force.v[0] *= 6.9;
                world_force.v[1] *= 6.9;
                world_force.v[2] *= 6.9;
                effects = 1;
                CrashNoise(&world_force, &world_point, 0);
            } else if (object2 != NULL && object2->flags_0x238 == 0x10) {
                QueueDroneCrush(object2, chain_count, chain2, &force2);
                BrMatrix34ApplyV(&world_force, &force2, &object2->transform_matrix);
                BrMatrix34ApplyP(&world_point, &point2, &object2->transform_matrix);
                effects = 1;
                world_force.v[2] *= 6.9;
                world_force.v[1] *= 6.9;
                world_force.v[0] *= 6.9;
                CrashNoise(&world_force, &world_point, 0);
            }
        }
        if (impulse > 0.001f) {
            if ((object1 != NULL && object1->flags_0x238 == 0x40)
                    || (object2 != NULL && object2->flags_0x238 == 0x40)) {
                CollideShitMine(object1, object2, impulse);
            }
        }
        if (gPinball_factor != 0.f && pContacts[k].object2 == NULL && impulse > 0.0002f) {
            if (!effects) {
                BrMatrix34ApplyV(&world_force, &force1, &object1->transform_matrix);
                world_force.v[0] *= 6.9;
                world_force.v[1] *= 6.9;
                world_force.v[2] *= 6.9;
            }
            recoil.v[0] = world_force.v[0] * gPinball_factor / object1->M * 0.2f;
            recoil.v[1] = world_force.v[1] * gPinball_factor / object1->M * 0.2f;
            recoil.v[2] = world_force.v[2] * gPinball_factor / object1->M * 0.2f;
            {
                br_scalar recoil_dot = BrVector3LengthSquared(&recoil);

                if (recoil_dot > 500.f) {
                    BrVector3Scale(&recoil, &recoil, 500.f / recoil_dot);
                }
                object1->v.v[0] += recoil.v[0];
                object1->v.v[1] += recoil.v[1];
                object1->v.v[2] += recoil.v[2];
                if (recoil_dot > 1.f) {
                    if (object1 != NULL && object1->owner != NULL && object1->flags_0x238 == 1
                            && ((tCar_spec*)object1->owner)->driver == eDriver_local_human) {
                        DRS3StartSound(gCar_outlet, 0x2333);
                    } else {
                        DRS3StartSound3D(gCar_outlet, 0x2333, &object1->actor->t.t.translate.t, &gZero_v__car, 1, 255, 0x10000, 0x10000);
                    }
                }
            }
        }
    }
    return 0;
}


// GLOBAL: CARMA2_HW 0x0058f680
int gMetal_crunch_sound_id[5] = { 5000, 5001, 5002, 5003, 5004 };

// GLOBAL: CARMA2_HW 0x0058f698
int gScrape_sound_id[3] = { 5010, 5011, 5012 };

// FUNCTION: CARMA2_HW 0x0041ddc0
void C2_HOOK_FASTCALL CrashNoise(br_vector3* pForce, br_vector3* pPosition, int pMaterial) {
    // GLOBAL: CARMA2_HW 0x00679410
    static tS3_sound_tag crunch_tag;
    // GLOBAL: CARMA2_HW 0x0067935c
    static tS3_volume last_crunch_vol;
    tS3_volume vol;
    br_vector3 velocity;

    extern tS3_outlet* gCar_outlet;
    extern int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);
    extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

    vol = 60.0 * BrVector3Length(pForce);
    if (gCurrent_race.material_modifiers[pMaterial].crash_noise_index == -1) {
    } else {
        if (vol > 255) {
            vol = 255;
        }
        if (crunch_tag == 0 || !DRS3SoundStillPlaying(crunch_tag)) {
            if (vol > 30) {
                last_crunch_vol = vol;
                (void)last_crunch_vol;
                velocity.v[0] = 0.f;
                velocity.v[1] = 0.f;
                velocity.v[2] = 0.f;
                crunch_tag = DRS3StartSound3D(gCar_outlet,
                    gMetal_crunch_sound_id[IRandomBetween(0, 4)],
                    pPosition, &velocity, 1, vol, IRandomBetween(49152, 81920), 0x10000);
            }
        }
    }
}

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
extern float gFloat_sine_table[91];
extern float gFloat_sine_table_360[1];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x005115b0
br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees) {

    if (pAngle_in_degrees < 0) {
        pAngle_in_degrees = 360 - -pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    }
    if (pAngle_in_degrees > 270) {
        return -*(gFloat_sine_table_360 - pAngle_in_degrees);
    }
    if (pAngle_in_degrees > 180) {
        return -gFloat_sine_table[pAngle_in_degrees - 180];
    }
    if (pAngle_in_degrees > 90) {
        return *(gFloat_sine_table + 180 - pAngle_in_degrees);
    }
    return gFloat_sine_table[pAngle_in_degrees];
}

// (reference only)
// FUNCTION: CARMA2_HW 0x004ff410
void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject) {
    extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);
    extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
    extern int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);
    extern tS3_outlet* gCar_outlet;
    tSpecial_volume* new_vol;
    tCar_spec* sv;

    new_vol = FindSpecialVolume(&pObject->pos, pObject->last_special_volume, 0);

    sv = NULL;
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        sv = (tCar_spec*)pObject->owner;
    } else {
        sv = NULL;
    }

    if (pObject->auto_special_volume != NULL) {
        if (new_vol == NULL || new_vol->gravity_multiplier == 1.0f) {
            if (pObject->water_d == 10000.0f && pObject->water_depth_factor != 1.0f) {
                pObject->auto_special_volume = NULL;
            } else {
                new_vol = pObject->auto_special_volume;
            }
        }
    }

    if (new_vol != pObject->last_special_volume && sv != NULL && sv->driver == eDriver_local_human) {
        if (pObject->last_special_volume != NULL && pObject->last_special_volume->exit_noise >= 0
                && (new_vol == NULL || new_vol->exit_noise != pObject->last_special_volume->exit_noise)) {
            DRS3StartSound(gCar_outlet, pObject->last_special_volume->exit_noise);
        }
        if (new_vol != NULL && new_vol->entry_noise >= 0
                && (pObject->last_special_volume == NULL || pObject->last_special_volume->entry_noise != new_vol->entry_noise)) {
            DRS3StartSound(gCar_outlet, new_vol->entry_noise);
        }
    }

    pObject->last_special_volume = new_vol;

    if (sv != NULL && new_vol != NULL) {
        br_scalar gravity_multiplier = new_vol->gravity_multiplier;
        if (gravity_multiplier < 1.0f) {
            if (IsCarSmoking(sv) != 0) {
                StopCarSmoking(sv);
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006793a4
tU32 gLast_cunning_stunt;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// (SetAmbientPratCam inlined)
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
// GLOBAL: CARMA2_HW 0x0058f6b0
const float gCar_simplification_factor[2][5] = {
    { 20.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.50f, 0.0f }
};

extern int gCar_simplification_level;
extern int gGraf_spec_index;

// FUNCTION: CARMA2_HW 0x0041f110
void C2_HOOK_FASTCALL DoLODCarModels(void) {
    int i;
    int j;
    int variant;
    tCar_spec* car;
    float tx, ty, tz;
    float level;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];

        if (car != NULL && car->driver > 5 && car->car_master_actor->render_style != BR_RSTYLE_NONE) {
            tx = car->car_master_actor->t.t.translate.t.v[0] - gCamera_to_world.m[3][0];
            ty = car->car_master_actor->t.t.translate.t.v[1] - gCamera_to_world.m[3][1];
            tz = car->car_master_actor->t.t.translate.t.v[2] - gCamera_to_world.m[3][2];
            if (gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level] >= 0.001f) {
                level = (tx * tx + ty * ty + tz * tz) / gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level];
            } else {
                level = BR_SCALAR_MAX;
            }

            for (j = car->count_detail_levels - 1; j > 0; j--) {
                if (car->detail_levels[j] < level) {
                    break;
                }
            }
            if (j > 0) {
                variant = j;
                DRActorEnumRecurse(car->car_model_actor, SwitchCarModel, &variant);
                car->field_0xe18 = variant;
            } else if (!gAction_replay_mode && car->use_shell_model && car->shell_model != NULL) {
                DRActorEnumRecurse(car->car_model_actor, TurnOffNonGroovers, NULL);
                car->car_model_actor->type = BR_ACTOR_MODEL;
                car->car_model_actor->model = car->shell_model;
            }
        }
    }
}

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

extern int gFlyCarMode;
extern br_scalar gFlyCarAngularVelocity;
extern int gFlyCarKeyFlags;
extern int gFlyCarKeyTimer0;
extern int gFlyCarKeyTimer1;
extern int gFlyCarKeyTimer2;
extern int gFlyCarKeyTimer3;
extern void C2_HOOK_FASTCALL UpdateChildTransforms(tPhysics_object* pObject, br_matrix34* pParent_matrix);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0041f4f0
void C2_HOOK_FASTCALL FlyCar(tCar_spec* c, br_scalar dt) {
    br_matrix34* mat;
    br_vector3 a;
    br_vector3 b;
    br_vector3 norm;
    br_vector3 norm2;
    br_material* material;
    br_scalar dist;
    br_scalar dist2;
    br_scalar temp0;
    br_scalar temp1;
    br_scalar temp2;
    br_scalar temp3;
    int shift_down;
    int ctrl_only;
    int mode_saved;
    int i;

    mat = &c->collision_info->actor->t.t.mat;
    shift_down = PDKeyDown(0);
    if (shift_down == 0 && PDKeyDown(1) != 0) {
        ctrl_only = 1;
    } else {
        ctrl_only = 0;
    }

    mode_saved = gFlyCarMode;
    if ((gFlyCarKeyFlags & 0x40000) == 0 && gFlyCarKeyTimer2 < 1) {
        if ((gFlyCarKeyFlags & 0x80000) == 0 && gFlyCarKeyTimer3 < 1) {
            if ((gFlyCarKeyFlags & 0x200000) != 0 && shift_down != 0) {
                gFlyCarAngularVelocity = gFlyCarAngularVelocity - dt * 3.0f * 0.1449275362318841;
                gFlyCarMode = 1;
            } else if ((gFlyCarKeyFlags & 0x400000) != 0 && shift_down != 0) {
                gFlyCarAngularVelocity = gFlyCarAngularVelocity - dt * -3.0f * 0.1449275362318841;
                gFlyCarMode = 1;
            } else if ((gFlyCarKeyFlags & 0x10000) != 0 && ctrl_only != 0) {
                gFlyCarAngularVelocity = gFlyCarAngularVelocity - dt * -3.0f * 0.1449275362318841;
                gFlyCarMode = 0;
            } else if ((gFlyCarKeyFlags & 0x20000) != 0 && ctrl_only != 0) {
                gFlyCarAngularVelocity = gFlyCarAngularVelocity - dt * 3.0f * 0.1449275362318841;
                gFlyCarMode = 0;
            } else if (dt * 3.0f <= gFlyCarAngularVelocity || gFlyCarAngularVelocity <= dt * -3.0f) {
                gFlyCarAngularVelocity = gFlyCarAngularVelocity - (gFlyCarAngularVelocity / fabs(gFlyCarAngularVelocity)) * dt * 3.0f;
            } else {
                gFlyCarAngularVelocity = 0.0f;
            }
        } else {
            gFlyCarAngularVelocity = gFlyCarAngularVelocity - dt * 10.0f * 0.1449275362318841;
            gFlyCarMode = 2;
        }
    } else {
        gFlyCarAngularVelocity = gFlyCarAngularVelocity - dt * -10.0f * 0.1449275362318841;
        gFlyCarMode = 2;
    }
    if (gFlyCarMode != mode_saved) {
        gFlyCarAngularVelocity = 0.0f;
    }
    if ((gFlyCarKeyFlags & 0x100000) != 0) {
        gFlyCarAngularVelocity = 0.0f;
    }
    if (shift_down != 0) {
        i = 2;
        c->collision_info->omega.v[1] = 0.0f;
    } else {
        i = 1;
        c->collision_info->omega.v[2] = 0.0f;
    }
    if (((gFlyCarKeyFlags & 0x10000) != 0 || 0 < gFlyCarKeyTimer0) && ctrl_only == 0) {
        c->collision_info->omega.v[i] = dt + c->collision_info->omega.v[i];
    } else if (((gFlyCarKeyFlags & 0x20000) != 0 || 0 < gFlyCarKeyTimer1) && ctrl_only == 0) {
        c->collision_info->omega.v[i] = c->collision_info->omega.v[i] - dt;
    } else {
        c->collision_info->omega.v[i] = 0.0f;
    }
    if ((gFlyCarKeyFlags & 0x200000) != 0 && shift_down == 0) {
        c->collision_info->omega.v[0] = dt + c->collision_info->omega.v[0];
    } else if ((gFlyCarKeyFlags & 0x400000) != 0 && shift_down == 0) {
        c->collision_info->omega.v[0] = c->collision_info->omega.v[0] - dt;
    } else {
        c->collision_info->omega.v[0] = 0.0f;
    }
    if (c->collision_info->omega.v[0] != 0.0f) {
        BrMatrix34PreRotateX(mat, BrRadianToAngle(c->collision_info->omega.v[0] * dt));
    }
    if (c->collision_info->omega.v[1] != 0.0f) {
        norm.v[0] = mat->m[3][0];
        norm.v[1] = mat->m[3][1];
        norm.v[2] = mat->m[3][2];
        mat->m[3][0] = mat->m[3][0] - norm.v[0];
        mat->m[3][1] = mat->m[3][1] - norm.v[1];
        mat->m[3][2] = mat->m[3][2] - norm.v[2];
        BrMatrix34PostRotateY(mat, BrRadianToAngle(c->collision_info->omega.v[1] * dt));
        mat->m[3][0] = norm.v[0] + mat->m[3][0];
        mat->m[3][1] = norm.v[1] + mat->m[3][1];
        mat->m[3][2] = norm.v[2] + mat->m[3][2];
    }
    if (c->collision_info->omega.v[2] != 0.0f) {
        BrMatrix34PreRotateZ(mat, BrRadianToAngle(c->collision_info->omega.v[2] * dt));
    }
    c->collision_info->v.v[0] = -gFlyCarAngularVelocity * mat->m[gFlyCarMode][0];
    c->collision_info->v.v[1] = -gFlyCarAngularVelocity * mat->m[gFlyCarMode][1];
    c->collision_info->v.v[2] = -gFlyCarAngularVelocity * mat->m[gFlyCarMode][2];
    temp0 = c->collision_info->v.v[2] * dt;
    temp1 = c->collision_info->v.v[0];
    temp2 = c->collision_info->v.v[1] * dt + mat->m[3][1];
    temp3 = temp0 + mat->m[3][2];
    mat->m[3][1] = temp2;
    mat->m[3][0] = temp1 * dt + mat->m[3][0];
    mat->m[3][2] = temp3;
    if ((gFlyCarKeyFlags & 0x800000) != 0) {
        if (ctrl_only != 0) {
            BrMatrix34PreRotateX(mat, BrRadianToAngle(asin(mat->m[1][1])));
            BrMatrix34PreRotateZ(mat, (br_angle)(long)(asin(mat->m[0][1]) * -10430.378350470453));
        } else {
            b.v[0] = mat->m[3][0];
            b.v[1] = mat->m[3][1];
            b.v[2] = mat->m[3][2];
            mat->m[3][0] = mat->m[3][0] - b.v[0];
            mat->m[3][1] = mat->m[3][1] - b.v[1];
            mat->m[3][2] = mat->m[3][2] - b.v[2];
            BrMatrix34RotateY(mat, 0x3ffc - BrRadianToAngle(atan2(mat->m[2][2], mat->m[2][0])));
            mat->m[3][0] = b.v[0];
            mat->m[3][1] = b.v[1];
            mat->m[3][2] = b.v[2];
            BrVector3SetFloat(&b, 0.0f, -10.0f, 0.0f);
            a.v[0] = mat->m[3][0];
            a.v[1] = mat->m[3][1];
            a.v[2] = mat->m[3][2];
            ActorFindFace(&a, &b, gTrack_actor, &norm, &dist, &material, NULL);
            a.v[1] = a.v[1] - -10.0f;
            ActorFindFace(&a, &b, gTrack_actor, &norm2, &dist2, &material, NULL);
            if (dist2 <= 1.0f) {
                BrVector3SetFloat(&b, 0.0f, -5.01f, 0.0f);
                a.v[1] = a.v[1] - 10.0f;
                i = 0;
                do {
                    a.v[1] = a.v[1] - -0.5f;
                    ActorFindFace(&a, &b, gTrack_actor, &norm2, &dist2, &material, NULL);
                    if (dist2 <= 1.0f) {
                        break;
                    }
                    i++;
                } while (i < 20);
                dist2 = dist2 * -0.05f - (float)(i + 1) * -0.05f;
            }
            if (dist2 < dist) {
                norm.v[0] = norm2.v[0];
                dist = -dist2;
                norm.v[1] = norm2.v[1];
                norm.v[2] = norm2.v[2];
            }
            if (fabs(dist) <= 1.0f) {
                mat->m[3][1] = mat->m[3][1] - dist * 10.0f;
                BrMatrix34PreRotateX(mat, BrRadianToAngle(asin(mat->m[2][0] * norm.v[0] + (mat->m[2][2] * norm.v[2] + mat->m[2][1] * norm.v[1]))));
                BrMatrix34PreRotateZ(mat, (br_angle)(long)(asin(mat->m[0][0] * norm.v[0] + (mat->m[0][2] * norm.v[2] + mat->m[0][1] * norm.v[1])) * -10430.378350470453));
            }
        }
    }
    c->direction.v[0] = -mat->m[2][0];
    c->direction.v[1] = -mat->m[2][1];
    c->direction.v[2] = -mat->m[2][2];
    BrMatrix34ApplyP(&c->pos, &c->collision_info->cmpos, mat);
    UpdateChildTransforms(c->collision_info, NULL);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

extern tS8 gNon_car_spec_indices[100];
extern tNon_car_spec* gActive_non_car_list[99];
extern br_actor* gAdditional_actors;
extern void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor);
extern int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);
extern int C2_HOOK_FASTCALL FUN_00420310(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL FUN_00420360(tPhysics_object* pObject);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0041ff20
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor) {
    tNon_car_spec* non_car;
    int code;

    non_car = NULL;
    if (actor->type_data != NULL) {
        return NULL;
    }
    code = actor->identifier[1] * 10 + actor->identifier[2] - 0x210;
    if (gNon_car_spec_indices[code] != 0) {
        non_car = &gProgram_state.non_cars[gNon_car_spec_indices[code] + 0x27];
    }
    if (non_car == NULL) {
        goto failed;
    }
    if (non_car->driver == eDriver_4) {
        tNon_car_spec* slot;
        int i;

        i = 0;
        slot = gProgram_state.non_cars;
        do {
            if (slot->driver == eDriver_non_car_unused_slot) {
                break;
            }
            i++;
            slot++;
        } while (i < 40);
        if (i == 40) {
            non_car = NULL;
        } else {
            tPhysics_object* saved_collision_info;

            saved_collision_info = slot->collision_info;
            *saved_collision_info = *gProgram_state.non_cars[gNon_car_spec_indices[code] + 0x27].collision_info;
            *slot = gProgram_state.non_cars[gNon_car_spec_indices[code] + 0x27];
            slot->collision_info = saved_collision_info;
            non_car = slot;
        }
    }
    if (non_car == NULL) {
        goto failed;
    }
    actor->type_data = non_car;
    non_car->driver = eDriver_4;
    non_car->collision_info->disable_move_rotate = 1;
    if (actor->parent == gAdditional_actors) {
        non_car->car_ID = -1;
    } else {
        non_car->car_ID = actor->identifier[7] + 10 * (actor->identifier[6] + 10 * actor->identifier[5]) - 0x14d0;
    }
    BrActorRemove(actor);
    BrActorAdd(gNon_track_actor, actor);
    non_car->actor = actor;
    gActive_non_car_list[gNum_active_non_cars] = non_car;
    gNum_active_non_cars += 1;
    gActive_car_list[gNum_cars_and_non_cars] = (tCar_spec*)non_car;
    gNum_cars_and_non_cars += 1;
    if (gTesting_car_for_sensible_place) {
        GetNewBoundingBox(&non_car->collision_info->field_0xf4, &non_car->collision_info->bb2, &actor->t.t.mat);
        non_car->collision_info->field_0x10c = non_car->collision_info->field_0xf4;
    }
    BrMatrix34Copy(&non_car->collision_info->transform_matrix, &actor->t.t.mat);
    non_car->collision_info->box_face_ref = gFace_num__car - 2;
    non_car->collision_info->disable_move_rotate = 1;
    non_car->collision_info->actor = non_car->actor;
    *(int*)((tU8*)non_car + 0x4) = 0;
    BrVector3SetFloat(&non_car->collision_info->v, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&non_car->collision_info->omega, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&non_car->collision_info->rotate_omega, 0.0f, 0.0f, 0.0f);
    *(int*)((tU8*)non_car + 0x68) = 0;
    *(int*)((tU8*)non_car + 0xc8) = 0;
    if (actor->identifier[3] == '!' || non_car->snap_off_cosine >= 1.0f) {
        actor->identifier[3] = '!';
        non_car->collision_info->field_0xed = 1;
        non_car->collision_info->M = non_car->free_mass;
        *(float*)((tU8*)non_car + 0x14) = 0.0f;
        if (non_car->snap_off_cosine < 1.0f) {
            non_car->collision_info->physics_joint2 = NULL;
        }
        *(int*)((tU8*)non_car->collision_info + 0x194) = 0;
        *(int*)((tU8*)non_car->collision_info + 0x198) = 0;
        if (non_car->collision_info->physics_joint2 != NULL) {
            BrMatrix34ApplyP(&non_car->collision_info->field_0x17c, (br_vector3*)((tU8*)non_car->collision_info->physics_joint2 + 8), &non_car->collision_info->transform_matrix);
        }
    } else {
        non_car->collision_info->M = non_car->attached_mass;
        *(float*)((tU8*)non_car + 0x14) = non_car->min_torque_squared;
        non_car->collision_info->field_0xed = 0;
        non_car->collision_info->physics_joint2 = non_car->field_0xf0;
        *(int*)((tU8*)non_car->collision_info + 0x194) = (int)FUN_00420360;
        *(int*)((tU8*)non_car->collision_info + 0x198) = (int)FUN_00420310;
        BrMatrix34ApplyP(&non_car->collision_info->field_0x17c, (br_vector3*)((tU8*)non_car->collision_info->physics_joint2 + 8), &non_car->collision_info->transform_matrix);
    }
    non_car->collision_info->I.v[0] = non_car->I_over_M.v[0] * non_car->collision_info->M;
    non_car->collision_info->I.v[1] = non_car->I_over_M.v[1] * non_car->collision_info->M;
    non_car->collision_info->I.v[2] = non_car->I_over_M.v[2] * non_car->collision_info->M;
    non_car->collision_info->water_d = 10000.0f;
    non_car->collision_info->field_0x261 = 0;
    non_car->collision_info->message_time = 0;
    *(float*)((tU8*)non_car + 0xc4) = -1.0f;
    non_car->collision_info->field_0x49c = 1;
    *(int*)non_car->field_0xf8 = 0;
    non_car->flags &= 0xffef0000;
    BrVector3SetFloat(&non_car->collision_info->field_0x54, 0.0f, -0.05797101557f, 0.0f);
    ResetCarSpecialVolume(non_car->collision_info);
    BrMatrix34Copy(&non_car->collision_info->transform_matrix, &actor->t.t.mat);
    if (non_car->car_ID != -1) {
        PipeSingleNonCar(non_car->car_ID, non_car->actor);
    }
    PHILAddActiveObjectImmediately(non_car->collision_info, NULL, NULL, NULL);
    return non_car;
failed:
    actor->identifier[1] = 'x';
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
// and :492
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
extern void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar);

// FUNCTION: CARMA2_HW 0x004157e0
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = (float)(atan((pCar->wpos[0].v[2] - pCar->wpos[2].v[2])
            * (pCar->field_0x1260 + pCar->curvature)) * (180.0 / PI));
    pCar->lr_sus_position = (pCar->wpos[0].v[1] - pCar->oldd[0]) * 0.14492753623188406;
    pCar->rr_sus_position = (pCar->wpos[1].v[1] - pCar->oldd[1]) * 0.14492753623188406;
    pCar->lf_sus_position = (pCar->wpos[2].v[1] - pCar->oldd[2]) * 0.14492753623188406;
    pCar->rf_sus_position = (pCar->wpos[3].v[1] - pCar->oldd[3]) * 0.14492753623188406;
    PipeSingleGraphicalWheelStuff(pCar);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int gDust_rotate;
extern int gNum_dust_tables;
extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

// FUNCTION: CARMA2_HW 0x004fa5b0
void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
    br_vector3 pos;
    br_vector3 v;
    br_vector3 vcs;
    br_vector3 tv;
    br_scalar decay_factor;
    br_scalar ts;
    br_scalar alpha;
    br_scalar beta;
    int colour;

    if ((tU32)pCar->dust_time[pWheel] > pTime) {
        pCar->dust_time[pWheel] -= pTime;
        return;
    }
    pCar->dust_time[pWheel] += IRandomBetween(200, 400) - pTime;
    if (pCar->dust_time[pWheel] < 0) {
        pCar->dust_time[pWheel] = 0;
    }
    tv.v[0] = pCar->wpos[pWheel].v[2] * pCar->collision_info->omega.v[1] - pCar->collision_info->omega.v[2] * pCar->wpos[pWheel].v[1];
    tv.v[1] = pCar->wpos[pWheel].v[0] * pCar->collision_info->omega.v[2] - pCar->collision_info->omega.v[0] * pCar->wpos[pWheel].v[2];
    tv.v[2] = pCar->wpos[pWheel].v[0] * pCar->collision_info->omega.v[1] - pCar->collision_info->omega.v[0] * pCar->wpos[pWheel].v[1];
    BrVector3Scale(&vcs, &pCar->collision_info->velocity_car_space, WORLD_SCALE);
    BrVector3Accumulate(&vcs, &tv);
    ts = BrVector3LengthSquared(&vcs);
    if (ts < 25.0f) {
        return;
    }
    decay_factor = (float)sqrt(ts) * 0.04f;
    if (decay_factor > 1.0f) {
        decay_factor = 1.0f;
    }
    BrVector3InvScale(&tv, &pCar->wpos[pWheel], WORLD_SCALE);
    tv.v[1] -= pCar->oldd[pWheel] * 0.14492753623188406;
    alpha = -1000.0f;
    if (vcs.v[2] < 0.0f) {
        alpha = (pCar->collision_info->bb2.max.v[2] - tv.v[2]) / vcs.v[2];
    } else if (vcs.v[2] > 0.0f) {
        alpha = (pCar->collision_info->bb2.min.v[2] - tv.v[2]) / vcs.v[2];
    }
    beta = -1000.0f;
    if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.max.v[0] - tv.v[0]) / vcs.v[0];
    } else if (vcs.v[0] > 0.0f) {
        beta = (pCar->collision_info->bb2.min.v[0] - tv.v[0]) / vcs.v[0];
    }
    ts = MAX(alpha, beta);
    BrVector3Scale(&pos, &vcs, ts);
    BrVector3Accumulate(&tv, &pos);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyV(&v, &vcs, &pCar->car_master_actor->t.t.mat);
    colour = gDust_rotate + gCurrent_race.material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= gNum_dust_tables) {
        colour -= gNum_dust_tables;
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, decay_factor + decay_factor, colour + 8);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c6ff0
void C2_HOOK_FASTCALL PipeSingleNonCarCreation(uintptr_t pId, br_actor* pActor) {
    extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

    ARAddVariedDataToSession(ePipe_chunk_non_car, pId, 2, 4, 0, pActor, 0x30, 4, &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004207a0
int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data) {

    if (!pCollision_info->disable_move_rotate && pCollision_info->actor != NULL && pCollision_info->actor->type_data != NULL) {
        PipeSingleNonCarCreation((tU16)((tNon_car_spec*)pCollision_info->actor->type_data)->car_ID, pCollision_info->actor);
    }
    ((tNon_car_spec*)pCollision_info->owner)->field_0xfc = pCollision_info->disable_move_rotate;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fee70
void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject) {
    extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pCollision_info, int pParam);
    extern void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2);
    extern int gReplay_splash_flags;
    extern tReplay_splash gReplay_splashes[10];
    extern const br_scalar gFLOAT_0058b008;
    void* object;
    int i;

    if (pObject->water_d == 10000.f) {
        return;
    }
    pObject->water_d = 10000.f;
    if (PHILGetObjectProperty(pObject, 6) != gFLOAT_0058b008) {
        object = pObject;
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        object = pObject->actor;
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        object = pObject->owner;
    } else {
        object = NULL;
    }
    if (object == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gReplay_splashes); i++) {
        if ((gReplay_splash_flags & (1 << i)) && object == gReplay_splashes[i].object.actor) {
            ARStartPipingSession((tPipe_chunk_type)0x3b);
            AddExtendedSplashToPipingSession(pObject, object);
            AREndPipingSession();
            return;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 57.86%: FUN_0043fb70 (DamageCarInDirection) - logic from the REA pseudocode of 0x0043fb70 and
// (GPL-3.0). Remaining diff is x87 scheduling and register
// allocation only: the original keeps the three pForce components in x87 (fld/fst copies) for the
// magnitude, ours copies them through integer registers (mov esi) and reloads, which also moves the
// esi/edi pushes; the impact_location and condition-loop registers differ likewise.
// FUNCTION: CARMA2_HW 0x0043fb70
void C2_HOOK_FASTCALL FUN_0043fb70(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, int pCars_touch) {
    extern void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude);
    extern void C2_HOOK_STDCALL NewScreenWobble(double pAmplitude_x, double pAmplitude_y, double pPeriod);
    extern br_scalar gFloat_00589b64;
    extern double gDouble_00589b68;
    extern br_scalar gFloat_00589b70;
    extern br_scalar gFloat_00589b74;
    extern br_scalar gFloat_00589b78;
    extern br_scalar gFloat_00589b7c;
    extern double gDouble_00589b80;
    extern double gDouble_00589b88;
    extern double gDouble_00589b90;
    extern br_scalar gFloat_00589b98;
    extern br_scalar gFloat_00589b9c;
    extern br_scalar gFloat_00589ba0;
    extern br_scalar gFloat_00589ba4;
    extern br_scalar gFloat_00589ba8;
    extern br_scalar gFloat_00589bac;
    extern br_scalar gFloat_00589bb0;
    extern br_scalar gFloat_00589bb4;
    extern br_scalar gFloat_00589bb8;
    extern br_scalar gFloat_00589bbc;
    extern br_scalar gFloat_00589bc0;
    extern br_scalar gFloat_00589bc4;
    br_bounds3 bounds;
    float impact_strength;
    float x_from_min;
    float x_from_max;
    float y_from_min;
    float y_from_max;
    float z_from_min;
    float z_from_max;
    float nearest_x;
    float nearest_y;
    float nearest_z;
    float normalized_x;
    float normalized_y;
    float normalized_z;
    int impact_location;
    int clause_index;

    impact_strength = (float)sqrt(pForce->v[0] * pForce->v[0]
        + pForce->v[2] * pForce->v[2]
        + pForce->v[1] * pForce->v[1]);
    if (impact_strength == gFloat_00589b64 && pCars_touch == 0) {
        return;
    }

    bounds = pCar->collision_info->bb1;
    x_from_min = pPoint->v[0] - bounds.min.v[0];
    x_from_max = bounds.max.v[0] - pPoint->v[0];
    impact_strength = (float)(pCar->softness_factor * impact_strength * gDouble_00589b68);
    nearest_x = (x_from_min >= x_from_max) ? x_from_max : x_from_min;

    y_from_min = pPoint->v[1] - bounds.min.v[1];
    y_from_max = bounds.max.v[1] - pPoint->v[1];
    nearest_y = (y_from_min >= y_from_max) ? y_from_max : y_from_min;

    z_from_min = pPoint->v[2] - bounds.min.v[2];
    z_from_max = bounds.max.v[2] - pPoint->v[2];
    nearest_z = (z_from_min < z_from_max) ? z_from_min : z_from_max;

    if (nearest_x < nearest_z || nearest_y < nearest_z) {
        if (nearest_y < nearest_x || nearest_z < nearest_x) {
            impact_location = y_from_min < y_from_max ? eImpact_bottom : eImpact_top;
            normalized_x = x_from_min / (bounds.max.v[0] - bounds.min.v[0]);
            normalized_y = y_from_min;
        } else {
            impact_location = x_from_max <= x_from_min ? eImpact_right : eImpact_left;
            normalized_x = nearest_y;
            normalized_y = y_from_min / (bounds.max.v[1] - bounds.min.v[1]);
        }
        normalized_z = z_from_min / (bounds.max.v[2] - bounds.min.v[2]);
    } else {
        impact_location = z_from_max <= z_from_min ? eImpact_back : eImpact_front;
        normalized_x = x_from_min / (bounds.max.v[0] - bounds.min.v[0]);
        normalized_y = y_from_min / (bounds.max.v[1] - bounds.min.v[1]);
        normalized_z = x_from_min;
    }
    if (pCars_touch != 0 && pCar->last_impact_location == eImpact_unknown) {
        *(float*)((char*)pCar + 0x13d8) = normalized_x;
        *(float*)((char*)pCar + 0x13dc) = normalized_y;
        *(float*)((char*)pCar + 0x13e0) = normalized_z;
        pCar->last_impact_location = (tImpact_location)impact_location;
    }
    if (impact_strength == gFloat_00589b64 || pCar->invulnerable_no_damage != 0) {
        return;
    }
    if (pCars_touch == 0 && impact_location == eImpact_bottom) {
        impact_strength *= gFloat_00589b70;
    }

    for (clause_index = 0; clause_index < pCar->damage_programs[impact_location].clause_count; clause_index++) {
        tDamage_clause* clause;
        int condition_index;
        int conditions_pass;

        clause = &pCar->damage_programs[impact_location].clauses[clause_index];
        conditions_pass = 1;
        for (condition_index = 0; condition_index < clause->condition_count && conditions_pass; condition_index++) {
            tDamage_condition* condition;
            float position_component;

            condition = &clause->conditions[condition_index];
            switch (condition->axis_comp) {
            case eAxis_x:
                position_component = normalized_x;
                break;
            case eAxis_y:
                position_component = normalized_y;
                break;
            case eAxis_z:
                position_component = normalized_z;
                break;
            default:
                continue;
            }
            if (condition->condition_operator == eCondition_less_than) {
                conditions_pass = position_component < condition->comparitor;
            } else {
                conditions_pass = position_component > condition->comparitor;
            }
        }
        if (conditions_pass) {
            int effect_index;

            for (effect_index = 0; effect_index < clause->effect_count; effect_index++) {
                tDamage_effect* effect;
                float damage_amount;

                effect = &clause->effects[effect_index];
                if (pCar == NULL || pCar->driver < eDriver_net_human) {
                    damage_amount = effect->weakness_factor * impact_strength;
                    damage_amount *= gFloat_00589b98;
                    damage_amount *= (float)(gDouble_00589b80
                        - ((gDouble_00589b80 - (double)gCurrent_race.index / gNumber_of_races)
                            * gDouble_00589b88 * gDouble_00589b90));
                    DoDamage(pCar, effect->type, (int)damage_amount);
                } else if (gNet_mode != eNet_mode_none) {
                    damage_amount = effect->weakness_factor * impact_strength;
                    damage_amount *= gFloat_00589b74;
                    DoDamage(pCar, effect->type, (int)damage_amount);
                } else {
                    damage_amount = effect->weakness_factor * impact_strength;
                    if (PercentageChance((int)(damage_amount * gFloat_00589b78))) {
                        DoDamage(pCar, effect->type, (int)(damage_amount * gFloat_00589b7c));
                    }
                }
            }
        }
    }

    if (pCar == NULL || pCar->driver != eDriver_local_human) {
        return;
    }
    switch (impact_location) {
    case eImpact_top:
    case eImpact_bottom:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(impact_strength * gFloat_00589b7c, impact_strength * gFloat_00589bb8),
            FRandomBetween(gFloat_00589bb4 / impact_strength, gFloat_00589ba8 / impact_strength));
        return;
    case eImpact_left:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589bb0, impact_strength * gFloat_00589bac),
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(gFloat_00589ba0 / impact_strength, gFloat_00589b9c / impact_strength));
        return;
    case eImpact_right:
        NewScreenWobble(
            -FRandomBetween(impact_strength * gFloat_00589bb0, impact_strength * gFloat_00589bac),
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(gFloat_00589ba0 / impact_strength, gFloat_00589b9c / impact_strength));
        return;
    case eImpact_front:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(impact_strength * gFloat_00589bb0, impact_strength * gFloat_00589bc0),
            FRandomBetween(gFloat_00589b9c / impact_strength, gFloat_00589bbc / impact_strength));
        return;
    case eImpact_back:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(impact_strength * gFloat_00589bc4, impact_strength * gFloat_00589bc0),
            FRandomBetween(gFloat_00589b9c / impact_strength, gFloat_00589bbc / impact_strength));
        return;
    }
}
extern tCrush_list_item gCrush_lists[8];
extern br_scalar gMin_crush_force;
extern void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue);

// PARTIAL 49.06%: FUN_0042d400 - body complete (crush force accumulation); remaining diff:
// force components spilled through integer registers instead of fld, x87 scheduling.
// FUNCTION: CARMA2_HW 0x0042d400
void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick) {
    int point_count;
    int unique_count;
    int point_i;
    int list_i;
    int crush_i;
    int i;
    int is_unique;
    float force_length;
    float force_magnitude;
    float scale;
    br_vector3 unique_points[4];
    br_vector3 average;
    br_vector3 scaled_force;

    if (pCar->car_crush_spec == NULL || *(int*)((tU8*)pCar + 0x1b4) != 0 || pCar->car_crush_spec->field_0x4b8 != 0) {
        return;
    }
    force_length = (float)sqrt((double)(pForce->v[0] * pForce->v[0] + pForce->v[2] * pForce->v[2] + pForce->v[1] * pForce->v[1]));
    force_magnitude = pCar->softness_factor * force_length;
    if (!(force_magnitude > gMin_crush_force)) {
        return;
    }
    if (force_magnitude > 10.0f) {
        force_magnitude = 10.0f;
    }
    scale = (float)(1.0 / force_length);
    scaled_force.v[0] = pForce->v[0] * scale;
    scaled_force.v[1] = pForce->v[1] * scale;
    scaled_force.v[2] = pForce->v[2] * scale;
    unique_count = 1;
    unique_points[0] = pChain[0];
    average = pChain[0];
    point_count = pChain_count;
    if (point_count > 4) {
        point_count = 4;
    }
    for (point_i = 1; point_i < point_count; point_i++) {
        is_unique = 1;
        for (i = 0; i < unique_count; i++) {
            if (fabs((double)pChain[point_i].v[0] - (double)unique_points[i].v[0]) < 0.005
                    && fabs((double)pChain[point_i].v[1] - (double)unique_points[i].v[1]) < 0.005
                    && fabs((double)pChain[point_i].v[2] - (double)unique_points[i].v[2]) < 0.005) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_points[unique_count] = pChain[point_i];
            average.v[0] += pChain[point_i].v[0];
            average.v[1] += pChain[point_i].v[1];
            average.v[2] += pChain[point_i].v[2];
            unique_count++;
        }
    }
    average.v[0] /= (float)unique_count;
    average.v[1] /= (float)unique_count;
    average.v[2] /= (float)unique_count;
    for (list_i = 0; list_i < 8; list_i++) {
        if (gCrush_lists[list_i].car_spec == pCar || gCrush_lists[list_i].car_spec == NULL) {
            break;
        }
    }
    if (list_i >= 8) {
        return;
    }
    if (gCrush_lists[list_i].car_spec == NULL) {
        gCrush_lists[list_i].car_spec = pCar;
        gCrush_lists[list_i].count = 1;
        crush_i = 0;
    } else if (gCrush_lists[list_i].count < 4) {
        crush_i = gCrush_lists[list_i].count;
        gCrush_lists[list_i].count++;
    } else {
        float weakest_force;

        weakest_force = gCrush_lists[list_i].items[0].field_0x40;
        crush_i = 0;
        for (i = 1; i < gCrush_lists[list_i].count; i++) {
            if (gCrush_lists[list_i].items[i].field_0x40 < weakest_force) {
                weakest_force = gCrush_lists[list_i].items[i].field_0x40;
                crush_i = i;
            }
        }
        if (!(weakest_force < force_magnitude)) {
            return;
        }
    }
    SetCrush(&gCrush_lists[list_i].items[crush_i], unique_count, unique_points, &average, &scaled_force, force_magnitude, pTick);
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// (GPL-3.0); the original at 0x004c2c90
// FUNCTION: CARMA2_HW 0x004c2c90
void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV) {
    br_scalar b0;
    br_scalar b1;
    br_scalar b2;
    br_vector3 om;

    b0 = (pV->v[0] * 0.0210039914f) / pObject->I.v[0];
    om = pObject->omega;
    b1 = (pV->v[1] * 0.0210039914f) / pObject->I.v[1];
    b2 = (pV->v[2] * 0.0210039914f) / pObject->I.v[2];
    pObject->omega.v[0] -= b0;
    pObject->omega.v[1] -= b1;
    pObject->omega.v[2] -= b2;
    if (om.v[0] * pObject->omega.v[0] <= 0.0f) {
        pObject->omega.v[0] = 0.0f;
    }
    if (om.v[1] * pObject->omega.v[1] <= 0.0f) {
        pObject->omega.v[1] = 0.0f;
    }
    if (om.v[2] * pObject->omega.v[2] <= 0.0f) {
        pObject->omega.v[2] = 0.0f;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0058f704
int gFlyCarMode = 2;

// GLOBAL: CARMA2_HW 0x00679414
br_scalar gFlyCarAngularVelocity;

// SHADOWS gProgram_state (naming breaks other TUs): 0x0075cefc
int gFlyCarKeyFlags;

// SHADOWS gProgram_state (naming breaks other TUs): 0x0075cf00
int gFlyCarKeyTimer0;

// SHADOWS gProgram_state (naming breaks other TUs): 0x0075cf04
int gFlyCarKeyTimer1;

// SHADOWS gProgram_state (naming breaks other TUs): 0x0075cf08
int gFlyCarKeyTimer2;

// SHADOWS gProgram_state (naming breaks other TUs): 0x0075cf0c
int gFlyCarKeyTimer3;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c62e0
void C2_HOOK_FASTCALL UpdateChildTransforms(tPhysics_object* pObject, br_matrix34* pParent_matrix) {
    br_matrix34 inverse;
    br_matrix34 tmp_matrix;
    br_matrix34 tmp_matrix2;
    tPhysics_object* child;

    child = pObject->child;
    if (pParent_matrix != NULL) {
        BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->transform_matrix, pParent_matrix);
    } else {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&tmp_matrix2, &inverse, &pObject->actor->t.t.mat);
        BrMatrix34LPNormalise(&tmp_matrix, &tmp_matrix2);
        pParent_matrix = &tmp_matrix;
    }
    if (child != NULL) {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&tmp_matrix, &inverse, &pObject->actor->t.t.mat);
        do {
            UpdateChildTransforms(child, pParent_matrix);
            child = child->next;
        } while (child != NULL);
    }
    BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00420310
int C2_HOOK_FASTCALL FUN_00420310(tPhysics_object* pObject) {
    tNon_car_spec* non_car;

    non_car = (tNon_car_spec*)pObject->owner;
    pObject->field_0xed = 1;
    *(int*)((tU8*)pObject + 0x194) = 0;
    *(int*)((tU8*)pObject + 0x198) = 0;
    if (non_car->snap_off_cosine < 1.0f) {
        pObject->physics_joint2 = NULL;
    }
    pObject->field_0x218 = 0;
    pObject->v.v[0] = 0.0f;
    pObject->v.v[1] = 0.0f;
    pObject->v.v[2] = 0.0f;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00420360
int C2_HOOK_FASTCALL FUN_00420360(tPhysics_object* pObject) {
    tNon_car_spec* non_car;

    non_car = (tNon_car_spec*)pObject->owner;
    if (*(br_scalar*)((tU8*)pObject->actor + 0x3c) < non_car->snap_off_cosine) {
        pObject->field_0xed = 1;
        *(int*)((tU8*)pObject + 0x194) = 0;
        *(int*)((tU8*)pObject + 0x198) = 0;
        if (non_car->snap_off_cosine < 1.0f) {
            pObject->physics_joint2 = NULL;
        }
        pObject->field_0x218 = 0;
        pObject->v.v[0] = 0.0f;
        pObject->v.v[1] = 0.0f;
        pObject->v.v[2] = 0.0f;
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_FASTCALL NonCarSnapSmashCallback(tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce);
extern br_vector3 gVector3_00661678;
extern void C2_HOOK_FASTCALL ApplyNonCarTumble(tPhysics_object* pObject, br_vector3* pV, br_scalar pTumbleFactor, br_scalar pTumbleThreshold);

// (GPL-3.0) and the original at 0x00418250
// FUNCTION: CARMA2_HW 0x00418250
void C2_HOOK_FASTCALL MoveNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {
    tSpecial_volume* last_special_volume;
    tJoint_translation_params* translation_parameters;
    br_vector3 drag_vector;
    br_vector3 projected_axis;
    br_scalar drag;
    br_scalar k;
    br_scalar speed;
    br_scalar along;
    br_scalar acceleration;
    br_scalar scale;
    br_scalar gravity;

    last_special_volume = pNon_car->collision_info->last_special_volume;
    pNon_car->flags |= 0x100000;
    if (gCar_flying && gCar_to_view == (tCar_spec*)pNon_car) {
        return;
    }
    if (pNon_car->actor->identifier[3] != '!') {
        pNon_car->actor->identifier[3] = '&';
        if (pNon_car->collision_info->physics_joint2 == NULL) {
            pNon_car->collision_info->actor->identifier[3] = '!';
            pNon_car->collision_info->omega.v[0] = 0.f;
            pNon_car->collision_info->omega.v[1] = 0.f;
            pNon_car->collision_info->omega.v[2] = 0.f;
            pNon_car->collision_info->v.v[0] = 0.f;
            pNon_car->collision_info->v.v[1] = 0.f;
            pNon_car->collision_info->v.v[2] = 0.f;
            pNon_car->collision_info->M = pNon_car->free_mass;
            pNon_car->collision_info->field_0xed = 1;
            pNon_car->collision_info->I.v[0] = pNon_car->I_over_M.v[0] * pNon_car->collision_info->M;
            pNon_car->collision_info->I.v[1] = pNon_car->I_over_M.v[1] * pNon_car->collision_info->M;
            pNon_car->collision_info->I.v[2] = pNon_car->I_over_M.v[2] * pNon_car->collision_info->M;
            if (pNon_car->collision_info->field183_0x1d8 != 0
                    && ((tPhysics_object*)pNon_car->collision_info->field183_0x1d8)->owner != NULL
                    && ((tPhysics_object*)pNon_car->collision_info->field183_0x1d8)->flags_0x238 == 1
                    && ((tPhysics_object*)pNon_car->collision_info->field183_0x1d8)->owner != (void*)pNon_car) {
                NonCarSnapSmashCallback(pNon_car->collision_info, &((tCar_spec*)pNon_car)->pos, &pNon_car->collision_info->v, &gVector3_00661678);
            }
        } else {
            BrVector3SetFloat(&pNon_car->collision_info->v, 0.f, 0.f, 0.f);
            BrVector3SetFloat(&pNon_car->collision_info->omega, 0.f, 0.f, 0.f);
            BrVector3SetFloat(&pNon_car->collision_info->rotate_omega, 0.f, 0.f, 0.f);
            pNon_car->collision_info->disable_move_rotate = 1;
        }
    }
    if (pNon_car->actor->identifier[3] != '!') {
        return;
    }
    if (last_special_volume != NULL) {
        gravity = pDt * 10.f;
        gravity *= last_special_volume->gravity_multiplier;
        gravity *= 0.14492753623188406;
    } else {
        gravity = pDt * 1.4492753623188406;
    }
    pNon_car->collision_info->field_0x54.v[0] = 0.f;
    pNon_car->collision_info->field_0x54.v[1] = -gravity;
    pNon_car->collision_info->field_0x54.v[2] = 0.f;
    pNon_car->collision_info->v.v[1] -= gravity;
    speed = BrVector3Length(&pNon_car->collision_info->v) * 6.9;
    if (last_special_volume != NULL) {
        speed *= last_special_volume->viscosity_multiplier;
    }
    drag = pDt * 0.0005f;
    speed *= drag;
    k = speed / pNon_car->collision_info->M;
    drag_vector.v[0] = pNon_car->collision_info->v.v[0] * k;
    drag_vector.v[1] = pNon_car->collision_info->v.v[1] * k;
    drag_vector.v[2] = pNon_car->collision_info->v.v[2] * k;
    DRVector3Diminish(&pNon_car->collision_info->v, &drag_vector);
    speed = BrVector3Length(&pNon_car->collision_info->omega);
    if (last_special_volume != NULL) {
        speed *= last_special_volume->viscosity_multiplier;
    }
    speed *= drag;
    k = speed;
    drag_vector.v[0] = pNon_car->collision_info->omega.v[0] * k;
    drag_vector.v[1] = pNon_car->collision_info->omega.v[1] * k;
    drag_vector.v[2] = pNon_car->collision_info->omega.v[2] * k;
    ApplyWaterOmegaBrake(pNon_car->collision_info, &drag_vector);
    if (pNon_car->tumble_factor != 0.f) {
        ApplyNonCarTumble(pNon_car->collision_info, &pNon_car->collision_info->v, pNon_car->tumble_factor, pNon_car->tumble_threshold);
    }
    if ((pNon_car->flags & 0x10000) == 0) {
        return;
    }
    BrMatrix34ApplyV(&projected_axis, &pNon_car->field_0xf0->hinge_axis, &pNon_car->collision_info->transform_matrix);
    translation_parameters = pNon_car->translation_parameters;
    along = BrVector3Dot(&pNon_car->collision_info->velocity_car_space, &pNon_car->field_0xf0->hinge_axis);
    scale = translation_parameters->forward_resistance;
    acceleration = 0.f;
    speed = along - (BrVector3Dot((br_vector3*)pNon_car->collision_info->field_0x1b4, &projected_axis) + translation_parameters->forward_resistance);
    if ((pNon_car->flags & 0x20000) != 0
            && (pNon_car->collision_info->collision_flag & 2) != 0
            && (pNon_car->flags & 0xff) == 0
            && (pNon_car->flags & 0xff00) == 0
            && *(int*)pNon_car->field_0xf8 == 0
            && gNet_mode != eNet_mode_client) {
        pNon_car->flags = (pNon_car->flags & 1) | 1;
    }
    if ((pNon_car->flags & 0xff) != 0) {
        goto forward;
    }
    if (pNon_car->collision_info->disable_move_rotate == 0 && *(int*)pNon_car->field_0xf8 != 0) {
        goto forward;
    }
    if ((pNon_car->flags & 0xff00) != 0) {
        goto reverse;
    }
    if (pNon_car->collision_info->disable_move_rotate != 0 || *(int*)pNon_car->field_0xf8 != 0) {
        goto apply;
    }
reverse:
    acceleration = -translation_parameters->reverse_acceleration * 0.057971014492753624;
    scale = translation_parameters->reverse_resistance;
    if (speed <= 0.05) {
        goto apply;
    }
    if (along <= -0.005) {
        goto apply;
    }
    if ((pNon_car->flags >> 8) < pNon_car->number_of_pushes && (pNon_car->collision_info->collision_flag & 2) != 0) {
        pNon_car->flags = (pNon_car->flags & ~0xff00) | 0x100;
        goto apply;
    }
    pNon_car->flags = pNon_car->flags & ~0xff00;
    pNon_car->collision_info->disable_move_rotate = 1;
    BrVector3SetFloat(&pNon_car->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pNon_car->collision_info->omega, 0.f, 0.f, 0.f);
    acceleration = 0.f;
    goto apply;
forward:
    acceleration = translation_parameters->forward_acceleration * 0.057971014492753624;
    if (speed >= -0.05) {
        goto apply;
    }
    if (along >= 0.005) {
        goto apply;
    }
    if (*(int*)pNon_car->field_0xf8 == 0 || (pNon_car->flags & 0xff) == 1) {
        *(int*)pNon_car->field_0xf8 = GetRaceTime() + translation_parameters->pause_at_top;
    }
    if ((pNon_car->flags & 0xff) < pNon_car->number_of_pushes && (pNon_car->collision_info->collision_flag & 2) != 0) {
        if ((pNon_car->flags & 0xff) == 0) {
            goto apply;
        }
        pNon_car->flags = (pNon_car->flags & ~0xff) | ((pNon_car->flags + 1) & 0xff);
        goto apply;
    }
    pNon_car->flags = pNon_car->flags & ~0xff;
    pNon_car->collision_info->disable_move_rotate = 1;
    BrVector3SetFloat(&pNon_car->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pNon_car->collision_info->omega, 0.f, 0.f, 0.f);
    acceleration = 0.f;
apply:
    pNon_car->collision_info->v.v[0] = pNon_car->field_0xf0->hinge_axis.v[0] * acceleration + pNon_car->collision_info->v.v[0];
    pNon_car->collision_info->v.v[1] = pNon_car->field_0xf0->hinge_axis.v[1] * acceleration + pNon_car->collision_info->v.v[1];
    pNon_car->collision_info->v.v[2] = pNon_car->field_0xf0->hinge_axis.v[2] * acceleration + pNon_car->collision_info->v.v[2];
    pNon_car->collision_info->v.v[0] = pNon_car->collision_info->v.v[0] * scale;
    pNon_car->collision_info->v.v[1] = pNon_car->collision_info->v.v[1] * scale;
    pNon_car->collision_info->v.v[2] = pNon_car->collision_info->v.v[2] * scale;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00589a68
br_scalar gCrushReattachDistanceSquared = 0.000210039914236404f;

// GLOBAL: CARMA2_HW 0x00589b48
float gFLOAT_00589b48 = 4.999999873689376e-05f;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// (GPL-3.0) and the original at 0x0043b740
// FUNCTION: CARMA2_HW 0x0043b740
void C2_HOOK_FASTCALL SmoothCarCrushShapePoints(tCar_spec* pCar, float pStep) {
    tCar_crush_spec* crush;
    tCar_crush_shape_info* shape_info;
    tCar_crush_reordered_shape_info* point;
    br_vector3* pCurrent;
    float* pTarget;
    float value;
    int shape_index;
    int point_index;
    int axis;

    crush = pCar->car_crush_spec;
    for (shape_index = 0; shape_index < crush->count_shapes; shape_index++) {
        shape_info = &crush->field_0x4[shape_index];
        for (point_index = 0; point_index < shape_info->count_points; point_index++) {
            point = &shape_info->field_0x18[point_index];
            pCurrent = &point->field_0x18;
            pTarget = point->field_0x0.v;
            for (axis = 0; axis < 3; axis++) {
                if (pCurrent->v[axis] > pTarget[axis]) {
                    value = pCurrent->v[axis] - pStep;
                    pCurrent->v[axis] = value;
                    if (value < pTarget[axis]) {
                        pCurrent->v[axis] = pTarget[axis];
                    }
                } else if (pCurrent->v[axis] < pTarget[axis]) {
                    value = pCurrent->v[axis];
                    pCurrent->v[axis] = value + pStep;
                    if (pTarget[axis] < value + pStep) {
                        pCurrent->v[axis] = pTarget[axis];
                    }
                }
            }
            if (DRVector3TestForNan(pCurrent)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar->car_crush_spec->expand_bounding_box = 3;
}


/* Networking: broadcasts a "re-attach this bit" message for pObject's uid on pCar.
   */
// FUNCTION: CARMA2_HW 0x0043b250
void C2_HOOK_FASTCALL FUN_0043b250(tCar_spec* pCar, tPhysics_object* pObject) {
    extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
    extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);
    extern tU32 gPHIL_last_physics_tick;
    tNet_message* message;

    message = NetBuildGuaranteedMessage(0x26, 0);
    *(tPlayer_ID*)((tU8*)message + 0x1c) = NetPlayerFromCar(pCar)->ID;
    *(tU8*)((tU8*)message + 0x20) = pObject->uid;
    *(tU32*)((tU8*)message + 0x24) = gPHIL_last_physics_tick + 120;
    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
}

/* Networking: broadcasts a "re-attach all bits" message for pCar.
   */
// FUNCTION: CARMA2_HW 0x0043b2a0
void C2_HOOK_FASTCALL FUN_0043b2a0(tCar_spec* pCar) {
    extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
    extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);
    extern tU32 gPHIL_last_physics_tick;
    tNet_message* message;

    message = NetBuildGuaranteedMessage(0x2b, 0);
    *(tPlayer_ID*)((tU8*)message + 0x1c) = NetPlayerFromCar(pCar)->ID;
    *(tU32*)((tU8*)message + 0x20) = gPHIL_last_physics_tick + 120;
    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
}

// PARTIAL 57.81%: ApplyNonCarTumble - logic complete (replaces the inert stub); remaining diff:
// MSVC 11 shapes the len2/cross sums with integer moves instead of the original fxch chain, and
// 1.0/scale is an unnamed constant here (original uses gZero), x87 scheduling.
// FUNCTION: CARMA2_HW 0x00417fb0
void C2_HOOK_FASTCALL ApplyNonCarTumble(tPhysics_object* pObject, br_vector3* pV, br_scalar pTumbleFactor, br_scalar pTumbleThreshold) {
    extern void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
    br_vector3 cross;
    br_vector3 out;
    br_vector3 size;
    br_scalar len2;
    br_scalar speed;
    br_scalar over;
    br_scalar scale;
    br_scalar k;
    br_scalar nx;
    br_scalar ny;
    br_scalar nz;
    br_scalar new_len2;
    int i;

    if (pObject->collision_flag == 0 && pObject->disable_move_rotate == 0) {
        return;
    }
    len2 = pV->v[0] * pV->v[0] + pV->v[1] * pV->v[1] + pV->v[2] * pV->v[2];
    if (len2 < pTumbleThreshold * pTumbleThreshold) {
        return;
    }
    DRVector3SafeCross(&cross, &gFace_list__car[pObject->box_face_start].normal, pV);
    scale = (float)sqrt(cross.v[0] * cross.v[0] + cross.v[1] * cross.v[1] + cross.v[2] * cross.v[2]);
    if (scale > BR_SCALAR_EPSILON * 2) {
        scale = (float)(1.0 / scale);
        cross.v[0] = cross.v[0] * scale;
        cross.v[1] = cross.v[1] * scale;
        cross.v[2] = cross.v[2] * scale;
    } else {
        cross.v[0] = 1.f;
        cross.v[1] = 0.f;
        cross.v[2] = 0.f;
    }
    BrMatrix34TApplyV(&out, &cross, &pObject->actor->t.t.mat);
    if (out.v[0] * pObject->omega.v[0] + out.v[2] * pObject->omega.v[2] + out.v[1] * pObject->omega.v[1] > 1.5f) {
        return;
    }
    speed = (float)sqrt(len2);
    size.v[0] = pObject->bb1.max.v[0] - pObject->bb1.min.v[0];
    size.v[1] = pObject->bb1.max.v[1] - pObject->bb1.min.v[1];
    size.v[2] = pObject->bb1.max.v[2] - pObject->bb1.min.v[2];
    over = speed - pTumbleThreshold;
    for (i = 0; i < 3; i++) {
        out.v[i] = out.v[i] * (0.5f * size.v[i]) / pObject->I.v[i];
    }
    k = 0.1f * pTumbleFactor * over * pObject->M;
    nx = out.v[0] * k + pObject->omega.v[0];
    ny = out.v[1] * k + pObject->omega.v[1];
    nz = out.v[2] * k + pObject->omega.v[2];
    new_len2 = nx * nx + ny * ny + nz * nz;
    pObject->omega.v[0] = nx;
    pObject->omega.v[1] = ny;
    pObject->omega.v[2] = nz;
    if (new_len2 > 400.f) {
        scale = (float)sqrt(400.f / new_len2);
        pObject->omega.v[0] = nx * scale;
        pObject->omega.v[1] = ny * scale;
        pObject->omega.v[2] = nz * scale;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00430e30
int C2_HOOK_FASTCALL FUN_00430e30(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    (void)pContacts; (void)pImpulses; (void)pCount;
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ba550
int C2_HOOK_FASTCALL ScaleCollisionImpulses(tCollision_contact* pContacts, float* pImpulses, int pCount, int* pSmash_indices, int pNum_impacts, float pMax_impulse) {
    extern int gINT_00692dc8;
    extern br_scalar gFLOAT_0058a768;
    int i;
    float total;
    float scale;
    float f;

    total = gFLOAT_0058a768;
    for (i = 0; i < pNum_impacts; i++) {
        total += pImpulses[pSmash_indices[i]];
    }
    if (total <= pMax_impulse) {
        return 0;
    }
    scale = pMax_impulse / total;
    gINT_00692dc8 = 1;
    for (i = 0; i < pCount; i++) {
        pImpulses[i] = -pImpulses[i];
    }
    for (i = 0; i < pNum_impacts; i++) {
        f = pImpulses[pSmash_indices[i]];
        pImpulses[pSmash_indices[i]] = f * (0.f - scale);
        if (pContacts[pSmash_indices[i]].type & 0x100) {
            f = pContacts[pSmash_indices[i]].impulse1.v[0];
            pContacts[pSmash_indices[i]].impulse1.v[0] = f * scale;
            f = pContacts[pSmash_indices[i]].impulse1.v[1];
            pContacts[pSmash_indices[i]].impulse1.v[1] = f * scale;
            f = pContacts[pSmash_indices[i]].impulse1.v[2];
            pContacts[pSmash_indices[i]].impulse1.v[2] = f * scale;
            if (pContacts[pSmash_indices[i]].object2 != NULL) {
                f = pContacts[pSmash_indices[i]].impulse1.v[0];
                pContacts[pSmash_indices[i]].impulse1.v[0] = f * scale;
                f = pContacts[pSmash_indices[i]].impulse1.v[1];
                pContacts[pSmash_indices[i]].impulse1.v[1] = f * scale;
                f = pContacts[pSmash_indices[i]].impulse1.v[2];
                pContacts[pSmash_indices[i]].impulse1.v[2] = f * scale;
            }
        }
    }
    for (i = 0; i < pCount; i++) {
        if (pImpulses[i] < gFLOAT_0058a768) {
            pImpulses[i] = 0.f;
        }
        pContacts[i].type = 9;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00431310
int C2_HOOK_FASTCALL FullyDetachSnappedBits(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    extern br_scalar gTorque_to_snap_per_tonne;
    extern void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
    tPhysics_object* object1;
    tPhysics_object* parent;
    tCar_spec* car;
    tUser_crush_data* user;
    float snap_torque;
    int index;
    int i;

    for (i = 0; i < pCount; i++) {
        if (pContacts[i].type == 0xb
                && (object1 = pContacts[i].object1) != NULL
                && object1->owner != NULL
                && object1->flags_0x238 == 1
                && (parent = object1->parent) != NULL
                && parent->owner != NULL
                && parent->flags_0x238 == 1
                && (car = (tCar_spec*)parent->owner) != NULL
                && car->driver > 5
                && car->invulnerable_no_crushage == 0
                && (user = (tUser_crush_data*)object1->actor->user) != NULL
                && user->crush_data != NULL
                && user->crush_data->flap_data != NULL
                && user->crush_data->flap_data->kev_o_flap != 0) {
            snap_torque = object1->M * gTorque_to_snap_per_tonne;
            if (pImpulses[i] > snap_torque) {
                if (BrVector3Dot(&pContacts[i].normal, &object1->physics_joint1->hinge_axis) < gCrushZero) {
                    if (gNet_mode == eNet_mode_none) {
                        index = i;
                        ScaleCollisionImpulses(pContacts, pImpulses, pCount, &index, 1, snap_torque);
                        FullyDetachBit(car, object1);
                        MungeDetachLists(car->car_crush_spec);
                        return 1;
                    } else if (gNet_mode == eNet_mode_host) {
                        SendFullyDetachBit(car, object1);
                    }
                }
            }
        }
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0042dfd0
int C2_HOOK_FASTCALL TrySplitCarOnVerticalForce(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    int flags[40];
    int split_indices[40];
    br_vector4 split_plane;
    int count;
    int i;
    int j;
    int split_count;
    int can_split;
    float total_force;
    float split_y;
    float threshold;
    tPhysics_object* object;
    tCar_spec* car;
    extern int C2_HOOK_FASTCALL FUN_00430e30(tCollision_contact* pContacts, float* pImpulses, int pCount);
    extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane);
    extern void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag);
    extern float gMin_force_to_split_Y_per_tonne;

    if (FUN_00430e30(pContacts, pImpulses, pCount) != 0) {
        return 1;
    }
    count = pCount;
    if (count > 40) {
        count = 40;
    }
    for (i = 0; i < count; i++) {
        flags[i] = 0;
    }
    for (i = 0; i < count; i++) {
        if (pImpulses[i] >= 0.01f
                && flags[i] == 0
                && (object = pContacts[i].object1) != NULL
                && pContacts[i].object2 == NULL
                && object->owner != NULL
                && object->flags_0x238 == 1
                && (car = (tCar_spec*)object->owner) != NULL
                && car->driver > 5
                && car->car_crush_spec != NULL
                && car->car_crush_spec->field_0x4b8 == 0
                && car->invulnerable_no_crushage == 0) {
            can_split = 1;
            split_y = car->car_crush_spec->split_y_pos - car->collision_info->cmpos.v[1];
            if (pContacts[i].r1.v[1] < split_y) {
                can_split = 0;
            } else {
                split_indices[0] = i;
                split_count = 1;
                total_force = pImpulses[i];
            }
            flags[i] = 1;
            j = i + 1;
            while (j < count) {
                if (pImpulses[j] >= 0.01f
                        && flags[j] == 0
                        && pContacts[j].object2 == NULL
                        && (tCar_spec*)pContacts[j].object1->owner == car) {
                    if (can_split) {
                        if (pContacts[j].r1.v[1] < split_y) {
                            can_split = 0;
                        } else {
                            split_indices[split_count] = j;
                            split_count++;
                            total_force += pImpulses[j];
                        }
                    }
                    flags[j] = 1;
                }
                j++;
            }
            if (can_split) {
                threshold = (car->collision_info->M * gMin_force_to_split_Y_per_tonne)
                    / (car->softness_factor * car->car_crush_spec->snappability_factor);
                if (total_force > threshold
                        && SplitCheck(car, 1, NULL, car->car_crush_spec->split_y_pos, &split_plane) != 0) {
                    ScaleCollisionImpulses(pContacts, pImpulses, count, split_indices, split_count, threshold);
                    SplitCar(car, &split_plane, 1);
                    return 1;
                }
            }
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tRepair_animation gRepair_animations[25];
extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);
extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);
extern void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex);
extern const float gFLOAT_0058af20;

// PARTIAL 83.07%: ActuallySmashCarPart - body complete and runtime-safe; remaining diff: pixelmap
// index computation (original keeps 177*level+i in a scaled load), funk_material reload register,
// -1.0f literal is an unnamed data offset here, case-block ordering.
// FUNCTION: CARMA2_HW 0x004ed2b0
void C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    br_pixelmap* texture;
    br_pixelmap** pixelmaps;
    int i;
    unsigned int flags;
    float fVar1;
    tU32 uVar5;

    texture = NULL;
    if (pCar_spec != NULL) {
        for (i = 0; i < (int)REC2_ASIZE(gRepair_animations); i++) {
            if (gRepair_animations[i].field_0x0 == (void*)pSmashable) {
                gRepair_animations[i].field_0x0 = NULL;
                break;
            }
        }
    }
    i = pSmashable->levels[pLevel].count_pixelmaps;
    if (i != 0) {
        i = IRandomBetween(0, i - 1);
        pixelmaps = (br_pixelmap**)pSmashable->levels;
        texture = pixelmaps[pLevel * 0xb1 + i];
    }
    PipeSingleSmashTextureChange(pCar_spec, pSmashable->funk_material, texture);
    pSmashable->funk_material->user = texture;
    if (pCar_spec == NULL || pCar_spec->field_0x1960 == NULL) {
        pSmashable->funk_material->colour_map = texture;
    }
    pSmashable->field_0x4c = pLevel;
    flags = pSmashable->levels[pLevel].flags;
    if (pCar_spec == NULL || (flags & 0x1c) != 0) {
        if ((flags & 0x4) != 0 && PercentageChance(0x32)) {
            pSmashable->funk_material->map_transform.m[0][0] *= -1.0f;
        }
        if ((flags & 0x8) != 0 && PercentageChance(0x32)) {
            pSmashable->funk_material->map_transform.m[1][1] *= -1.0f;
        }
        if ((flags & 0x10) != 0) {
            switch (IRandomBetween(0, 3)) {
            case 0:
                pSmashable->funk_material->map_transform.m[0][0] *= -1.0f;
                fVar1 = pSmashable->funk_material->map_transform.m[1][1] * -1.0f;
                break;
            case 1:
                uVar5 = *(tU32*)&pSmashable->funk_material->map_transform.m[0][0];
                *(tU32*)&pSmashable->funk_material->map_transform.m[0][0] = *(tU32*)&pSmashable->funk_material->map_transform.m[0][1];
                *(tU32*)&pSmashable->funk_material->map_transform.m[0][1] = uVar5;
                fVar1 = pSmashable->funk_material->map_transform.m[1][0];
                pSmashable->funk_material->map_transform.m[1][0] = -pSmashable->funk_material->map_transform.m[0][1];
                fVar1 = -fVar1;
                break;
            case 2:
                fVar1 = pSmashable->funk_material->map_transform.m[0][0];
                pSmashable->funk_material->map_transform.m[0][0] = -pSmashable->funk_material->map_transform.m[0][1];
                pSmashable->funk_material->map_transform.m[0][1] = -fVar1;
                fVar1 = pSmashable->funk_material->map_transform.m[1][0];
                *(tU32*)&pSmashable->funk_material->map_transform.m[1][0] = *(tU32*)&pSmashable->funk_material->map_transform.m[0][1];
                break;
            default:
                goto no_transform;
            }
            pSmashable->funk_material->map_transform.m[1][1] = fVar1;
        }
no_transform:
        ;
    } else {
        BrMatrix23Identity(&pSmashable->funk_material->map_transform);
    }
    BrMaterialUpdate(pSmashable->funk_material, BR_MATU_ALL);
    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
    if (pSmashable->funk >= 0) {
        DisableFunkotronic(pSmashable->funk);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00589f80
const double gJoystick_collision_force_scale = 10000.0;

// GLOBAL: CARMA2_HW 0x00589f88
const float gJoystick_collision_force_min_length = 0.0000002384185791015625f;

// GLOBAL: CARMA2_HW 0x00589f90
const double gJoystick_collision_force_one = 1.0;

// GLOBAL: CARMA2_HW 0x00589f98
const float gJoystick_collision_force_one_f = 1.0f;

// GLOBAL: CARMA2_HW 0x00589f9c
const float gJoystick_collision_force_zero_f = 0.0f;

// GLOBAL: CARMA2_HW 0x00589fa0
const double gJoystick_collision_force_radians_to_degrees = 57.29577951308;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* DirectInput 5 type subset, declared locally because the DirectX SDK headers are visible
   only to the platform target (same layout as the subset in joystick.c). */
typedef struct {
    tU32 dwSize;
    tU32 dwFlags;
    tU32 dwDuration;
    tU32 dwSamplePeriod;
    tU32 dwGain;
    tU32 dwTriggerButton;
    tU32 dwTriggerRepeatInterval;
    tU32 cAxes;
    tU32* rgdwAxes;
    tS32* rglDirection;
    void* lpEnvelope;
    tU32 cbTypeSpecificParams;
    void* lpvTypeSpecificParams;
} tDIEFFECT;

typedef struct {
    tDIEFFECT di_effect;
    tU8 type_specific_params[24];
    tU8 field_0x4c[20];
    tS32 field_0x60;
    tU32 field_0x64;
    char name[128];
} tJoystickForceEffect;

typedef struct tDirectInputEffect tDirectInputEffect;

typedef struct {
    void* field_0x0[6];
    int (C2_HOOK_STDCALL* SetParameters)(tDirectInputEffect* pEffect, const tDIEFFECT* pParams, tU32 pFlags);
} tDirectInputEffectVtbl;

struct tDirectInputEffect {
    tDirectInputEffectVtbl* lpVtbl;
};

extern tJoystickForceEffect gJoystick_effects[30];
extern tDirectInputEffect* gDirectInputEffects[30];
extern int gForceFeedbackAvailable;
extern int gCollision_force_joystick_effect_index;
extern void C2_HOOK_FASTCALL StartDinputEffect(int pIndex);

// FUNCTION: CARMA2_HW 0x00458720
int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(int pMagnitude, int pDirection) {
    /* This SDK spells DIEP_TYPESPECIFICPARAMS and DIEP_DIRECTION 0x100 and 0x40. */
    if (!gForceFeedbackAvailable) {
        return 0;
    }
    if (pMagnitude > 10000) {
        pMagnitude = 10000;
    }
    *(tS32*)&gJoystick_effects[gCollision_force_joystick_effect_index].type_specific_params = pMagnitude;
    gJoystick_effects[gCollision_force_joystick_effect_index].field_0x60 = pDirection;
    if (gDirectInputEffects[gCollision_force_joystick_effect_index] != NULL) {
        gDirectInputEffects[gCollision_force_joystick_effect_index]->lpVtbl->SetParameters(gDirectInputEffects[gCollision_force_joystick_effect_index], &gJoystick_effects[gCollision_force_joystick_effect_index].di_effect, 0x100);
        gDirectInputEffects[gCollision_force_joystick_effect_index]->lpVtbl->SetParameters(gDirectInputEffects[gCollision_force_joystick_effect_index], &gJoystick_effects[gCollision_force_joystick_effect_index].di_effect, 0x40);
    }
    StartDinputEffect(gCollision_force_joystick_effect_index);
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider);
extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(tU8 pType, int pSize_decider);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tU8 pType, int pSize_decider, tPlayer_ID pPlayer_id);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL CompressPlane(tU16* pDest, const br_vector4* pSrc);
extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);
extern void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id);
extern int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id);
extern void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pPlayer_id);
extern void C2_HOOK_FASTCALL SendLocalCarData(tU32 pNext_frame_time);
extern void C2_HOOK_FASTCALL CompressCarDataChunk(tNet_message_chunk* pMessage, int pPlayer_index);
extern int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar);
extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);
extern tShit_mine gShit_mines[20];

typedef struct {
    tU8 size;
    tU8 type;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU32 next_frame_time;
    tU8 suspension_compression[4];
    tU32 controls;
    tU32 hierarchy_flags;
    tU16 curvature;
    tU16 revs;
    tS32 repair_time;
    tU8 damage_levels[12];
    tU8 power_up_levels[3];
    undefined field_0x2f;
    tU16 crush_state;
    tU8 flags;
    undefined field_0x33;
    tU8 object_hierarchy[0x208];
} tNet_message_chunk_car_data;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00497900
void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time) {
    tNet_message_chunk_car_data* message;
    tU8 message_buffer[0x23c];
    tNet_message_chunk* chunk;
    tCar_spec* pCar;
    tCar_spec* pOther_car;
    tNon_car_spec* pNon_car;
    tU8 hierarchy_size;
    tU8 message_size;
    tU8 flags;
    tU8 base_size;
    tU8 extra_size;
    tU8 deform_flags;
    tU16 deform_value;
    int i;
    int j;
    int k;
    int check_idx;
    int crush_size;
    tPlayer_ID target_id;
    tPlayer_ID proxy_id;
    float dx;
    float dy;
    float dz;
    tU8* pData;

    message = (tNet_message_chunk_car_data*)message_buffer;
    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (gNet_mode == eNet_mode_host) {
        chunk = NetStartBroadcastContents(0x12, 0);
        if (chunk != NULL) {
            *(tU32*)((tU8*)chunk + 4) = gRace_start;
            NetBroadcastContents(chunk);
        }
    } else {
        chunk = NetAllocateMessageChunk(0x12, 0);
        *(tU32*)((tU8*)chunk + 4) = gRace_start;
    }
    if (gNet_mode == eNet_mode_host) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            pCar = gNet_players[i].car;
            if (pCar->disabled != 0) {
                continue;
            }
            flags = 0;
            hierarchy_size = (tU8)GetHierarchyNetworkSize(pCar->collision_info);
            message_size = hierarchy_size + 0x34;
            if (pCar->damage_units[8].damage_level > 0x1e || pCar->damage_units[9].damage_level > 0x1e
                    || pCar->damage_units[10].damage_level > 0x1e || pCar->damage_units[11].damage_level > 0x1e) {
                flags = 8;
                message_size += 0x10;
            }
            message->size = message_size;
            message->type = 0x10;
            message->player_id = gNet_players[i].ID;
            message->next_frame_time = pNext_frame_time;
            message->curvature = (tU16)(pCar->curvature / pCar->maxcurve * 32766.0f);
            message->controls = *(tU32*)&pCar->keys;
            message->controls = (message->controls & 0xffffff00) | ((pCar->joystick.acc >> 9) & 0xff);
            message->controls = (message->controls & 0xffff00ff) | ((pCar->joystick.dec >> 1) & 0xff00);
            message->revs = (tU16)pCar->revs;
            for (j = 0; j < 4; j++) {
                message->suspension_compression[j] = (tU8)(pCar->oldd[j] / pCar->susp_height[j >> 1] * 255.0f);
            }
            for (j = 0; j < 12; j++) {
                message->damage_levels[j] = (tU8)pCar->damage_units[j].damage_level;
            }
            message->power_up_levels[0] = (tU8)pCar->power_up_levels[0];
            message->power_up_levels[1] = (tU8)pCar->power_up_levels[1];
            message->power_up_levels[2] = (tU8)pCar->power_up_levels[2];
            message->repair_time = pCar->repair_time;
            message->hierarchy_flags = pCar->collision_info->field_0x49c;
            message->crush_state = pCar->car_crush_spec->field_0x24;
            message->flags = flags;
            if (GetHierarchyNetworkStuff(pCar->collision_info, message->object_hierarchy, 0x68) != hierarchy_size) {
                FatalError(0x72, "in car object data");
            }
            pData = message->object_hierarchy + hierarchy_size;
            if ((flags & 8) != 0) {
                for (j = 0; j < 4; j++) {
                    *(float*)pData = pCar->wheel_dam_offset[j];
                    pData += sizeof(float);
                }
            }
            base_size = message->size;
            for (j = 1; j < gNumber_of_net_players; j++) {
                if (i == j && pCar->collision_info->field_0x49c == *(undefined4*)((char*)pCar->collision_info + 0x274)) {
                    continue;
                }
                target_id = gNet_players[j].ID;
                proxy_id = *(tPlayer_ID*)&gNet_players[j].field_0xd0;
                if (proxy_id == 0) {
                    check_idx = j;
                } else {
                    check_idx = 0;
                    if (gNumber_of_net_players - 1 > 0) {
                        for (k = 0; k < gNumber_of_net_players - 1; k++) {
                            if (gNet_players[k].ID == proxy_id) {
                                break;
                            }
                        }
                        check_idx = k;
                    }
                }
                if (i != j) {
                    pOther_car = gNet_players[check_idx].car;
                    dx = pCar->car_master_actor->t.t.translate.t.v[0] - pOther_car->car_master_actor->t.t.translate.t.v[0];
                    dy = pCar->car_master_actor->t.t.translate.t.v[1] - pOther_car->car_master_actor->t.t.translate.t.v[1];
                    dz = pCar->car_master_actor->t.t.translate.t.v[2] - pOther_car->car_master_actor->t.t.translate.t.v[2];
                    if (sqrt(dx * dx + dy * dy + dz * dz) >= 40.0f) {
                        chunk = NetGetToPlayerContents(0x2d, 0, target_id);
                        CompressCarDataChunk(chunk, i);
                        continue;
                    }
                }
                message->flags &= 0xf8;
                message->size = base_size;
                if (pCar->car_crush_spec->version_le_100 != 0) {
                    if (pCar->car_crush_spec->field_0x28 != 0) {
                        pCar->car_crush_spec->field_0x24 = (tU16)(pCar->car_crush_spec->field_0x24 + 2 - ((pCar->car_crush_spec->field_0x24 & 1) != 0));
                        SetAllShapeStatusFlags(pCar->car_crush_spec->network_stuff);
                        ClearShapeStatusFlag(pCar->car_crush_spec->network_stuff, gNet_players[i].ID);
                        pCar->car_crush_spec->field_0x28 = 0;
                    }
                    if (CarShapeNeedsSendingToPlayer(pCar->car_crush_spec->network_stuff, gNet_players[check_idx].ID) != 0) {
                        crush_size = FUN_0043d900(pCar);
                        extra_size = 1;
                        message_size = (tU8)crush_size;
                        if (pCar->car_crush_spec->field_0x144 != 0) {
                            extra_size = 3;
                            message_size += 3;
                        }
                        if (pCar->car_crush_spec->field_0x4b8 != 0) {
                            extra_size |= 4;
                            message_size += 8;
                        }
                        message->flags |= extra_size;
                        pData = (tU8*)message + message->size;
                        message->size += message_size;
                        if ((extra_size & 1) != 0) {
                            EncodeNetShapeData(pData, pCar);
                            pData += crush_size;
                        }
                        if ((extra_size & 2) != 0) {
                            deform_flags = 0;
                            if (pCar->car_crush_spec->field_0x174 == pCar->car_crush_spec->bend_z_min) {
                                deform_flags |= 1;
                            }
                            if (*(tU16*)((char*)pCar->car_crush_spec + 0x190) != 0) {
                                deform_flags |= 2;
                                deform_value = *(tU16*)((char*)pCar->car_crush_spec + 0x190);
                            } else if (*(tU16*)((char*)pCar->car_crush_spec + 0x192) != 0) {
                                deform_flags |= 4;
                                deform_value = *(tU16*)((char*)pCar->car_crush_spec + 0x192);
                            } else {
                                deform_flags |= 8;
                                deform_value = *(tU16*)((char*)pCar->car_crush_spec + 0x194);
                            }
                            *pData = deform_flags;
                            pData++;
                            *(tU16*)pData = deform_value;
                            pData += 2;
                        }
                        if ((extra_size & 4) != 0) {
                            CompressPlane((tU16*)pData, (const br_vector4*)&pCar->car_crush_spec->field_0x548);
                        }
                    }
                }
                message->size = (tU8)(message->size + (message->size & 1));
                chunk = NetGetToPlayerContents(0x10, message->size, target_id);
                memcpy(chunk, message, message->size);
            }
            if (pCar->time_to_recover != 0 && pCar->time_to_recover - 500 < pNext_frame_time) {
                chunk = NetStartBroadcastContents(0x17, 0);
                if (chunk != NULL) {
                    *(tU32*)((tU8*)chunk + 4) = gNet_players[i].ID;
                    *(tU32*)((tU8*)chunk + 8) = pCar->time_to_recover;
                    NetBroadcastContents(chunk);
                }
            }
        }
        for (i = 0; i < gNum_active_non_cars; i++) {
            pNon_car = gActive_non_car_list[i];
            if ((pNon_car->flags & 0x100000) != 0) {
                if (pNon_car->driver == 2) {
                    SendDetachedNonCarData(pNon_car, pNext_frame_time, NULL);
                } else if (pNon_car->driver == 3) {
                    SendSplitHalfNonCarData(pNon_car, pNext_frame_time, NULL);
                } else {
                    SendNonCar(pNon_car, pNext_frame_time);
                }
            }
        }
    } else if (gNet_mode == eNet_mode_client) {
        SendLocalCarData(pNext_frame_time);
    }
}

// FUNCTION: CARMA2_HW 0x004dcfb0
void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time) {
    int j;
    int k;
    int hierarchy_size;
    int chunk_size;
    tShit_mine* mine;
    tU8 tmp_buf[0x23c];
    tNet_message_chunk_mine_explode* tmp;
    tNet_message_chunk* chunk;
    tPlayer_ID target_id;
    tPlayer_ID proxy_id;
    int check_idx;
    float dx;
    float dy;
    float dz;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    if (gNumber_of_net_players == 1) {
        return;
    }
    tmp = (tNet_message_chunk_mine_explode*)tmp_buf;
    for (mine = gShit_mines; mine < &gShit_mines[REC2_ASIZE(gShit_mines)]; mine++) {
        if (!(mine->flags & 1)) {
            continue;
        }
        tmp->mine_index = (tS8)(mine - gShit_mines);
        tmp->field_0x3 = mine->field_0x35;
        tmp->field_0x4 = mine->field_0x34;
        tmp->field_0x5 = (tU8)(NetPlayerFromCar(mine->field_0x30) - gNet_players);
        tmp->field_0x8 = (tS32)pNext_frame_time;
        GetHierarchyNetworkStuff(mine->collision_info, tmp->object_hierarchy, 0x48);
        hierarchy_size = GetHierarchyNetworkSize(mine->collision_info);
        chunk_size = (int)((float)hierarchy_size - (-12.0f));
        tmp->header.size = (tU8)chunk_size;
        tmp->header.type = 0x31;
        for (j = 1; j < gNumber_of_net_players; j++) {
            target_id = gNet_players[j].ID;
            proxy_id = *(tPlayer_ID*)&gNet_players[j].field_0xd0;
            if (proxy_id == 0) {
                check_idx = j;
            } else {
                check_idx = 0;
                if (gNumber_of_net_players - 1 > 0) {
                    for (k = 0; k < gNumber_of_net_players - 1; k++) {
                        if (gNet_players[k].ID == proxy_id) {
                            break;
                        }
                    }
                    check_idx = k;
                }
            }
            dx = mine->collision_info->transform_matrix.m[3][0] - gNet_players[check_idx].car->collision_info->transform_matrix.m[3][0];
            dy = mine->collision_info->transform_matrix.m[3][1] - gNet_players[check_idx].car->collision_info->transform_matrix.m[3][1];
            dz = mine->collision_info->transform_matrix.m[3][2] - gNet_players[check_idx].car->collision_info->transform_matrix.m[3][2];
            if (dx * dx + dy * dy + dz * dz >= 400.0f) {
                continue;
            }
            chunk = NetGetToPlayerContents(0x31, chunk_size, target_id);
            if (chunk == NULL) {
                continue;
            }
            memcpy(chunk, tmp, chunk_size);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00498460
void C2_HOOK_FASTCALL CompressCarDataChunk(tNet_message_chunk* pMessage, int pPlayer_index) {
    tU8* chunk;
    tCar_spec* car;
    br_actor* actor;

    chunk = (tU8*)pMessage;
    car = gNet_players[pPlayer_index].car;
    CompressVector3((tCompressed_vector3*)(chunk + 8), &car->car_master_actor->t.t.translate.t, -500.f, 500.f);
    actor = car->car_master_actor;
    *(tS16*)(chunk + 2) = (tS16)BrRadianToAngle(atan2(actor->t.t.mat.m[2][0], actor->t.t.mat.m[2][2]));
    *(int*)(chunk + 4) = gNet_players[pPlayer_index].ID;
    *(int*)(chunk + 0x10) = *(int*)((tU8*)car + 0x18a4);
}
// FUNCTION: CARMA2_HW 0x0043d900
int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar) {
    tCar_crush_spec* crush;
    int* node;
    int i;
    int total;
    int result;

    crush = pCar->car_crush_spec;
    if (crush->field_0x4b8 != 0) {
        result = 1;
        node = *(int**)(*(int*)((tU8*)pCar + 8) + 0x20);
        if (node != NULL) {
            do {
                if (node[0] == 1 || node[0] == 3) {
                    result = result + 1 + node[0xe] * 6;
                }
                node = (int*)node[0xd];
            } while (node != NULL);
        }
        return result;
    }
    total = 0;
    for (i = 0; i < crush->count_shapes; i++) {
        total += crush->field_0x4[i].count_points;
    }
    return total * 6 + 1;
}
// PARTIAL 84.45%: car_crush_spec CSE'd into one register vs the original's reload after each store; field_0x24 formula sub/add vs neg/sbb; controls chain reloads [esi+0x10]; store scheduling
// FUNCTION: CARMA2_HW 0x00497fc0
void C2_HOOK_FASTCALL SendLocalCarData(tU32 pNext_frame_time) {
    tNet_message_chunk_car_data* message;
    tNet_message_chunk* chunk;
    tU8* pData;
    tU8 flags;
    tU8 hierarchy_size;
    tU8 deformation_flags;
    tU16 deformation_value;
    int message_size;
    int crush_size;
    int i;

    if (gProgram_state.current_car.disabled != 0) {
        return;
    }
    flags = 0;
    hierarchy_size = (tU8)GetHierarchyNetworkSize(gProgram_state.current_car.collision_info);
    message_size = hierarchy_size + 0x34;
    if (gProgram_state.current_car.damage_units[8].damage_level > 0x1e
            || gProgram_state.current_car.damage_units[9].damage_level > 0x1e
            || gProgram_state.current_car.damage_units[10].damage_level > 0x1e
            || gProgram_state.current_car.damage_units[11].damage_level > 0x1e) {
        flags = 8;
        message_size += 0x10;
    }
    if (gProgram_state.current_car.car_crush_spec->version_le_100 != 0) {
        if (gProgram_state.current_car.car_crush_spec->field_0x28 != 0) {
            gProgram_state.current_car.car_crush_spec->field_0x24 = (tU16)(gProgram_state.current_car.car_crush_spec->field_0x24 + (2 - ((gProgram_state.current_car.car_crush_spec->field_0x24 & 1) != 0)));
            gProgram_state.current_car.car_crush_spec->field_0x28 = 0;
        }
        if (gProgram_state.current_car.car_crush_spec->field_0x24 != gProgram_state.current_car.car_crush_spec->field_0x26) {
            crush_size = FUN_0043d900(&gProgram_state.current_car);
            message_size += crush_size;
            flags |= 1;
            if (gProgram_state.current_car.car_crush_spec->field_0x144 != 0) {
                message_size += 3;
                flags |= 2;
            }
            if (gProgram_state.current_car.car_crush_spec->field_0x4b8 != 0) {
                message_size += 8;
                flags |= 4;
            }
        }
    }
    message = (tNet_message_chunk_car_data*)NetAllocateMessageChunk(0x10, message_size);
    message->next_frame_time = pNext_frame_time;
    message->player_id = gNet_players[gThis_net_player_index].ID;
    message->curvature = (tU16)(gProgram_state.current_car.curvature / gProgram_state.current_car.maxcurve * 32767.0f);
    message->controls = *(tU32*)&gProgram_state.current_car.keys;
    message->controls = (message->controls & 0xffffff00) | ((gProgram_state.current_car.joystick.acc >> 9) & 0xff);
    message->controls = (message->controls & 0xffff00ff) | ((gProgram_state.current_car.joystick.dec >> 1) & 0xff00);
    message->revs = (tU16)gProgram_state.current_car.revs;
    message->hierarchy_flags = gProgram_state.current_car.collision_info->field_0x49c;
    for (i = 0; i < 4; i++) {
        message->suspension_compression[i] = (tU8)(gProgram_state.current_car.oldd[i] / gProgram_state.current_car.susp_height[i >> 1] * 255.0f);
    }
    for (i = 0; i < 12; i++) {
        message->damage_levels[i] = (tU8)gProgram_state.current_car.damage_units[i].damage_level;
    }
    message->power_up_levels[0] = (tU8)gProgram_state.current_car.power_up_levels[0];
    message->power_up_levels[1] = (tU8)gProgram_state.current_car.power_up_levels[1];
    message->power_up_levels[2] = (tU8)gProgram_state.current_car.power_up_levels[2];
    message->repair_time = gProgram_state.current_car.repair_time;
    message->crush_state = gProgram_state.current_car.car_crush_spec->field_0x24;
    message->flags = flags;
    pData = message->object_hierarchy;
    if (GetHierarchyNetworkStuff(gProgram_state.current_car.collision_info, message->object_hierarchy, 0x68) != hierarchy_size) {
        FatalError(0x72, "in car object data");
    }
    pData += hierarchy_size;
    if ((flags & 8) != 0) {
        for (i = 0; i < 4; i++) {
            *(float*)pData = gProgram_state.current_car.wheel_dam_offset[i];
            pData += sizeof(float);
        }
    }
    if ((flags & 1) != 0) {
        EncodeNetShapeData(pData, &gProgram_state.current_car);
        pData += crush_size;
    }
    if ((flags & 2) != 0) {
        deformation_flags = 0;
        if (gProgram_state.current_car.car_crush_spec->field_0x174 == gProgram_state.current_car.car_crush_spec->bend_z_min) {
            deformation_flags |= 1;
        }
        if (*(tU16*)((char*)gProgram_state.current_car.car_crush_spec + 0x190) != 0) {
            deformation_flags |= 2;
            deformation_value = *(tU16*)((char*)gProgram_state.current_car.car_crush_spec + 0x190);
        } else if (*(tU16*)((char*)gProgram_state.current_car.car_crush_spec + 0x192) != 0) {
            deformation_flags |= 4;
            deformation_value = *(tU16*)((char*)gProgram_state.current_car.car_crush_spec + 0x192);
        } else {
            deformation_flags |= 8;
            deformation_value = *(tU16*)((char*)gProgram_state.current_car.car_crush_spec + 0x194);
        }
        *pData = deformation_flags;
        pData++;
        *(tU16*)pData = deformation_value;
        pData += 2;
    }
    if ((flags & 4) != 0) {
        CompressPlane((tU16*)pData, (const br_vector4*)&gProgram_state.current_car.car_crush_spec->field_0x548);
    }
    if (gProgram_state.current_car.time_to_recover != 0 && gProgram_state.current_car.time_to_recover - 500 < pNext_frame_time) {
        chunk = NetAllocateMessageChunk(0x17, 0);
        *(tU32*)((tU8*)chunk + 4) = gNet_players[gThis_net_player_index].ID;
        *(tU32*)((tU8*)chunk + 8) = gProgram_state.current_car.time_to_recover;
    }
    chunk = NetAllocateMessageChunk(0x2a, gNumber_of_net_players * 8 + 4);
    *(tU32*)((tU8*)chunk + 4) = gNet_players[gThis_net_player_index].ID;
    *(tU8*)((tU8*)chunk + 8) = (tU8)(gNumber_of_net_players - 1);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            *(tU32*)((tU8*)chunk + 0xc + i * 8) = gNet_players[i].ID;
            *(tU16*)((tU8*)chunk + 0x10 + i * 8) = gNet_players[i].car->car_crush_spec->field_0x24;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// Crush/damage tuning constants used by FUN_0043fb70 (DamageCarInDirection). Values and address
// order taken from the original data segment
// GLOBAL: CARMA2_HW 0x00589b64
br_scalar gFloat_00589b64 = 0.0f;
// GLOBAL: CARMA2_HW 0x00589b68
double gDouble_00589b68 = 1.4285714285714286;
// GLOBAL: CARMA2_HW 0x00589b70
br_scalar gFloat_00589b70 = 0.5f;
// GLOBAL: CARMA2_HW 0x00589b74
br_scalar gFloat_00589b74 = 15.0f;
// GLOBAL: CARMA2_HW 0x00589b78
br_scalar gFloat_00589b78 = 1500.0f;
// GLOBAL: CARMA2_HW 0x00589b7c
br_scalar gFloat_00589b7c = 30.0f;
// GLOBAL: CARMA2_HW 0x00589b80
double gDouble_00589b80 = 1.0;
// GLOBAL: CARMA2_HW 0x00589b88
double gDouble_00589b88 = 99.0;
// GLOBAL: CARMA2_HW 0x00589b90
double gDouble_00589b90 = -0.05;
// GLOBAL: CARMA2_HW 0x00589b98
br_scalar gFloat_00589b98 = 10.0f;
// GLOBAL: CARMA2_HW 0x00589b9c
br_scalar gFloat_00589b9c = 7.0f;
// GLOBAL: CARMA2_HW 0x00589ba0
br_scalar gFloat_00589ba0 = 4.0f;
// GLOBAL: CARMA2_HW 0x00589ba4
br_scalar gFloat_00589ba4 = 20.0f;
// GLOBAL: CARMA2_HW 0x00589ba8
br_scalar gFloat_00589ba8 = 5.0f;
// GLOBAL: CARMA2_HW 0x00589bac
br_scalar gFloat_00589bac = 100.0f;
// GLOBAL: CARMA2_HW 0x00589bb0
br_scalar gFloat_00589bb0 = 50.0f;
// GLOBAL: CARMA2_HW 0x00589bb4
br_scalar gFloat_00589bb4 = 1.0f;
// GLOBAL: CARMA2_HW 0x00589bb8
br_scalar gFloat_00589bb8 = 60.0f;
// GLOBAL: CARMA2_HW 0x00589bbc
br_scalar gFloat_00589bbc = 25.0f;
// GLOBAL: CARMA2_HW 0x00589bc0
br_scalar gFloat_00589bc0 = 150.0f;
// GLOBAL: CARMA2_HW 0x00589bc4
br_scalar gFloat_00589bc4 = -50.0f;

// FUNCTION: CARMA2_HW 0x004170c0
void C2_HOOK_FASTCALL CycleControlCar(void) {
    gControl__car++;
    if (ControlCar[gControl__car] == NULL) {
        gControl__car = 0;
    }
    switch (gControl__car) {
    case 0:
        NewTextHeadupSlot(4, 0, 500, -1, "Original Controls");
        return;
    case 1:
        NewTextHeadupSlot(4, 0, 500, -1, "Accelerated steering");
        return;
    case 2:
        NewTextHeadupSlot(4, 0, 500, -1, "0.75 Accelerated");
        return;
    case 3:
        NewTextHeadupSlot(4, 0, 500, -1, "0.5 Accelerated");
        return;
    default:
        NewTextHeadupSlot(4, 0, 500, -1, "New controls");
        return;
    }
}

// FUNCTION: CARMA2_HW 0x0041f4a0
void C2_HOOK_FASTCALL PutOpponentsInNeutral(void) {
    gStop_opponents_moving = !gStop_opponents_moving;
    if (gStop_opponents_moving) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Opponents in neutral");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Back in gear");
    }
}

// FUNCTION: CARMA2_HW 0x0043d960
int C2_HOOK_FASTCALL GetCompressedCrushVertexDataSize(tU8* pData) {
    tU8 count_or_points;
    int count;
    int size;

    count_or_points = *pData++;
    if (count_or_points <= 4) {
        count = count_or_points;
        size = 1;
        if (count > 0) {
            do {
                count_or_points = *pData;
                size = size + 1 + count_or_points * 6;
                pData = pData + count_or_points * 6 + 1;
                count--;
            } while (count != 0);
        }
        return size;
    }
    return count_or_points * 6 + 1;
}

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU32 field_0x8;
} tNet_message_chunk_crush_reattach_bit;

// FUNCTION: CARMA2_HW 0x0043ce60
void C2_HOOK_FASTCALL ReceivedCrushReattachBit(tNet_message_chunk_crush_reattach_bit* pContents) {
    extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);
    tCar_spec* car;
    int item_offset;
    int count;
    tU32 bit_index;

    if (gCount_net_crush_reattach_bit_list != 8) {
        car = NetCarFromPlayerID(pContents->player_id);
        if (car != NULL) {
            count = gCount_net_crush_reattach_bit_list;
            item_offset = count * 3;
            item_offset <<= 2;
            count++;
            *(tCar_spec**)((tU8*)&gNet_crush_reattach_bit_list[0].car + item_offset) = car;
            *(tU32*)((tU8*)&gNet_crush_reattach_bit_list[0].actor + item_offset) = 0;
            bit_index = pContents->field_0x8;
            gCount_net_crush_reattach_bit_list = count;
            *(tU32*)((tU8*)&gNet_crush_reattach_bit_list[0].time + item_offset) = bit_index;
        }
    }
}
