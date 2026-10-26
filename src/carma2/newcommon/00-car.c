#include "00-car.h"

#include "15-displays.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"

#include "c2_stdio.h"
#include "59-camera.h"
#include "10-loading2.h"
#include "49-physics.h"
#include "30-opponent.h"

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

// ControlOurCar

// CalcEngineForce

// PrepareCars

// CalcGraphicalWheelStuff

// FinishCars

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

// CalcForce

// DoRevs

// ScrapeNoise

// SkidNoise

// StopSkid

// CrashNoise

// CrushAndDamageCar

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

// MungeCarGraphics

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

// FlyCar

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

// CheckForDeAttachmentOfNonCars

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
// STUB: CARMA2_HW 0x00414510
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index) {
    NOT_IMPLEMENTED();
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00413f70
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
