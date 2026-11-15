/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "00-car.h"

#include "01-network.h"
#include "02-init.h"
#include "04-netgame.h"
#include "10-loading2.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "20-skidmark.h"
#include "22-replay.h"
#include "23-piping.h"
#include "27-powerup.h"
#include "29-raycast.h"
#include "30-opponent.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "49-physics.h"
#include "53-controls.h"
#include "55-volume.h"
#include "57-drone.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"

#include "c2_math.h"
#include "c2_stdio.h"
#include "rec2_macros.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x0079eda0
// GLOBAL: CARMA2_SW 0x00795340
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x0067939c
// GLOBAL: CARMA2_SW 0x00673b0c
int gTesting_car_for_sensible_place;

// GLOBAL: CARMA2_HW 0x00763500
// GLOBAL: CARMA2_SW 0x00759a60
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
// GLOBAL: CARMA2_SW 0x00743720
float gArmour_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761f5c
// GLOBAL: CARMA2_SW 0x007584bc
int gExplosion_sound_id;

// GLOBAL: CARMA2_HW 0x00761f60
// GLOBAL: CARMA2_SW 0x007584c0
float gPower_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761d40
// GLOBAL: CARMA2_SW 0x007582a0
float gOffensive_starting_value[100];

// GLOBAL: CARMA2_HW 0x0075ba20
// GLOBAL: CARMA2_SW 0x00751f80
tFloat_bunch_info gCar_softness;

// GLOBAL: CARMA2_HW 0x0074d600
// GLOBAL: CARMA2_SW 0x00743b60
tFloat_bunch_info gCar_car_damage_multiplier;

// GLOBAL: CARMA2_HW 0x0068b8d0
// GLOBAL: CARMA2_SW 0x00685e30
br_vector3 gZero_v__car;  // FIXME: make const?

// GLOBAL: CARMA2_HW 0x006793a0
// GLOBAL: CARMA2_SW 0x00673b10
int gFreeze_mechanics;

// GLOBAL: CARMA2_HW 0x006793dc
// GLOBAL: CARMA2_SW 0x00673b4c
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x006793d0
// GLOBAL: CARMA2_SW 0x00673b40
int gEliminate_faces;

// GLOBAL: CARMA2_HW 0x00679364
// GLOBAL: CARMA2_SW 0x00673ad4
int gIn_the_sea;

// GLOBAL: CARMA2_HW 0x0065d010
// GLOBAL: CARMA2_SW 0x00657948
int gFace_num__car = 1;

// GLOBAL: CARMA2_HW 0x00744820
// GLOBAL: CARMA2_SW 0x0073b2e0
tFace_ref gFace_list__car[300];

// GLOBAL: CARMA2_HW 0x00679360
// GLOBAL: CARMA2_SW 0x00673ad0
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
// GLOBAL: CARMA2_SW 0x00673b54
tPhysics_object* gUnknown_car_collision_info;

// GLOBAL: CARMA2_HW 0x006793d4
// GLOBAL: CARMA2_SW 0x00673b44
int gDouble_pling_water;

// GLOBAL: CARMA2_HW 0x0079ed88
// GLOBAL: CARMA2_SW 0x00795328
int gDrivable_on_count;

// GLOBAL: CARMA2_HW 0x0079ecc0
// GLOBAL: CARMA2_SW 0x00795260
tPhysics_object* gDrivable_on_list[50];

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

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

// FUNCTION: CARMA2_HW 0x00413d00
// FUNCTION: CARMA2_SW 0x00413d30
void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_engine, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d30
// FUNCTION: CARMA2_SW 0x00413d60
void C2_HOOK_FASTCALL DamageTrans(int pAmount) {
    DamageUnit(&gProgram_state.current_car, eDamage_transmission, pAmount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d90
// FUNCTION: CARMA2_SW 0x00413dc0
void C2_HOOK_FASTCALL DamageLFWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lf_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413dc0
// FUNCTION: CARMA2_SW 0x00413df0
void C2_HOOK_FASTCALL DamageLFBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lf_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413df0
// FUNCTION: CARMA2_SW 0x00413e20
void C2_HOOK_FASTCALL DamageLRBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lr_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e20
// FUNCTION: CARMA2_SW 0x00413e50
void C2_HOOK_FASTCALL DamageLRWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lr_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e50
// FUNCTION: CARMA2_SW 0x00413e80
void C2_HOOK_FASTCALL DamageRFWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rf_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e80
// FUNCTION: CARMA2_SW 0x00413eb0
void C2_HOOK_FASTCALL DamageRFBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rf_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413eb0
// FUNCTION: CARMA2_SW 0x00413ee0
void C2_HOOK_FASTCALL DamageRRBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rr_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413ee0
// FUNCTION: CARMA2_SW 0x00413f10
void C2_HOOK_FASTCALL DamageRRWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rr_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413f10
// FUNCTION: CARMA2_SW 0x00413f40
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

// FUNCTION: CARMA2_HW 0x00413f70
// FUNCTION: CARMA2_SW 0x00413fa0
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
    int index;
    int j;
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
    BrVector3SetFloat(&pCar->collision_info->v, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&pCar->collision_info->omega, 0.0f, 0.0f, 0.0f);
    pCar->curvature = 0.0f;
    pCar->field_0x1260 = 0.0f;
    BrMatrix34Copy(&safe_position, &pCar->car_master_actor->t.t.mat);
    if (safe_position.m[3][0] > 500.0f) {
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &gInitial_position);
    }
    BrMatrix34Copy(&pCar->old_frame_mat, &safe_position);
    BrMatrix34Copy(&pCar->collision_info->transform_matrix, &safe_position);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (j = 0; j < REC2_ASIZE(pCar->oldd); j++) {
        pCar->oldd[j] = pCar->ride_height;
    }
    pCar->gear = 0;
    pCar->revs = 0.0f;
    pCar->traction_control = 1;
    BrVector3Negate(&pCar->direction, (br_vector3*)pCar->car_master_actor->t.t.mat.m[2]);
    for (j = 0; j < REC2_ASIZE(pCar->last_safe_positions); j++) {
        BrMatrix34Copy(&pCar->last_safe_positions[j], &safe_position);
    }
    pCar->collision_info->field_0x261 = 0;
    pCar->collision_info->message_time = 0;
    pCar->dt = -1.0f;
    pCar->collision_info->field_0x49c = 1;
    pCar->time_to_recover = 0;
    pCar->repair_time  = 0;
    pCar->collision_info->water_d  = 10000.0f;

    switch (pCar->driver) {
    case eDriver_oppo:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_oppo) {
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
                    pCar->car_ID = (eVehicle_opponent << 8) + index;
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
                    pCar->car_ID = (eVehicle_net_player << 8) + index;
                }
                index++;
            }
        }
        break;
    case eDriver_local_human:
        pCar->car_ID = 0;
        break;
#ifndef REC2_MATCHING
    default:
        break;
#endif
    }
    PossibleService();
    pCar->collision_info->box_face_ref = gFace_num__car - 2;
    pCar->collision_info->box_face_end = 0;
    pCar->collision_info->box_face_start = 0;
    pCar->collision_info->disable_move_rotate = 0;
    pCar->end_steering_damage_effect = 0;
    pCar->end_trans_damage_effect = 0;
    pCar->wheel_dam_offset[0] = 0.0f;
    pCar->wheel_dam_offset[1] = 0.0f;
    pCar->wheel_dam_offset[2] = 0.0f;
    pCar->wheel_dam_offset[3] = 0.0f;
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
    pCar->grip_multiplier = 1.0f;
    pCar->damage_multiplier = 1.0f;
    pCar->field_0x4c8 = 1.0f;
    pCar->field_0x4d4 = 1.0f;
    pCar->bounce_rate = 0.0f;
    pCar->bounce_amount = 0.0f;
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
    SetCarSuspGiveAndHeight(pCar, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    for (j = 0; j < REC2_ASIZE(pCar->powerups); j++) {
        pCar->powerups[j] = 0;
    }
    if (gNet_mode != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < (int)REC2_ASIZE(pCar->power_up_levels); j++) {
            pCar->power_up_levels[j] = gNet_mode != eNet_mode_none ? gInitial_APO[j].initial_network[gCurrent_net_game->type] : gInitial_APO[j].initial[gProgram_state.skill_level];
            pCar->power_up_slots[j] = gNet_mode != eNet_mode_none ? gInitial_APO_potential[j].initial_network[gCurrent_net_game->type] : gInitial_APO_potential[j].initial[gProgram_state.skill_level];
        }
    }
}

// FUNCTION: CARMA2_HW 0x00414400
// FUNCTION: CARMA2_SW 0x00414430
void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar) {
    InitialiseCar2(pCar, 1);
}

// FUNCTION: CARMA2_HW 0x00414410
// FUNCTION: CARMA2_SW 0x00414440
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
        PhysicsAddObjectList(gProgram_state.current_car.collision_info, gUnknown_car_collision_info);
    }
}

// FUNCTION: CARMA2_HW 0x00414510
// FUNCTION: CARMA2_SW 0x00414540
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, tU32 pGrid_index) {
    int place_on_grid;
    int i;
    int start_i;
    int j;
    br_actor* car_actor;
    br_angle initial_yaw;
    br_scalar nearest_y_above;
    br_scalar nearest_y_below;
    int below_face_index;
    int above_face_index;
    br_model* below_model;
    br_model* above_model;
    tCar_spec* car;
    br_vector3 grid_offset;
    br_vector3 dist;
    br_vector3 real_pos;
    br_matrix34 initial_yaw_matrix;
    tCompressed_matrix3 compressed;
    int inactive;

    car = pThe_race->opponent_list[pCar_index].car_spec;
    car_actor = car->car_master_actor;
    BrMatrix34Identity(&car_actor->t.t.mat);
    place_on_grid = 1;
    if (gNet_mode != eNet_mode_none && !gCurrent_net_game->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.0f) {
                        BrVector3Sub(&real_pos, &real_pos, &gInitial_position);
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.0f) {
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
    if (gNet_mode == eNet_mode_none && (int)pGrid_index < 0) {
        BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[-((int)pGrid_index + 1)].pos);
        initial_yaw = BrDegreeToAngle(pThe_race->net_starts[-((int)pGrid_index + 1)].yaw);
        place_on_grid = 0;
    }
    if (place_on_grid) {
        initial_yaw = BrDegreeToAngle(pThe_race->initial_yaw);
        BrMatrix34RotateY(&initial_yaw_matrix, initial_yaw);
        grid_offset.v[0] = -(br_scalar)((int)pGrid_index % 2);
        grid_offset.v[1] = 0.0f;
        grid_offset.v[2] = (br_scalar)((int)pGrid_index % 2) * 0.4f - (br_scalar)((int)pGrid_index / 2) * -2.0f;
        BrMatrix34ApplyV(&car_actor->t.t.translate.t, &grid_offset, &initial_yaw_matrix);
        BrVector3Accumulate(&car_actor->t.t.translate.t, &pThe_race->initial_position);
    }
    if (gTrack_actor != NULL) {
        FindBestY(&car_actor->t.t.translate.t, gTrack_actor, 10.0f, &nearest_y_above, &nearest_y_below, &above_model, &below_model, &above_face_index, &below_face_index);
        if (nearest_y_above != 30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_above;
        } else if (nearest_y_below != -30000.0f) {
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
    if (gNet_mode != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.0f) {
        DisableCar(car);
    }
}

// FUNCTION: CARMA2_HW 0x004148d0
// FUNCTION: CARMA2_SW 0x00414900
void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race) {
    int i;

    for (i = 0; i < pThe_race->number_of_racers; i++) {
        int grid_index = pThe_race->opponent_list[i].ranking;

        if (grid_index >= 0) {
            grid_index = i;
        }
        SetInitialPosition(pThe_race, i, grid_index);
    }
}

// InitialiseNonCar

// FUNCTION: CARMA2_HW 0x00414910
// FUNCTION: CARMA2_SW 0x00414940
void C2_HOOK_FASTCALL NewFaceListCallBack(tPhysics_object* pObject, tFace_ref* pFace) {
    br_bounds3 current_bounds;
    br_scalar old_d;
    float prop7;

    old_d = pObject->water_d;
    if (pObject->owner == NULL) {
        return;
    }
    prop7 = PHILGetObjectProperty(pObject, 7);
    if (pObject->flags_0x238 != 1 && prop7 == 0.0f) {
        return;
    }
    if (pObject != NULL
            && pObject->owner != NULL
            && pObject->flags_0x238 == 1
            && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human
            && pObject->water_d != 10000.0f
            && gDouble_pling_water
            && BrVector3Dot(&pObject->water_normal, &pObject->field_0xf4.max) - pObject->water_d <= 0.0f) {
        gIn_the_sea = 1;
        FreezeCamera();
    }
    if (pFace != NULL && (float)fabs(pFace->normal.v[1]) > 0.9f) {
        BrVector3Copy(&pObject->water_normal, &pFace->normal);
        if (pObject->water_normal.v[1] < 0.0f) {
            BrVector3Negate(&pObject->water_normal, &pObject->water_normal);
        }
        pObject->water_d = BrVector3Dot(&pFace->v[0], &pObject->water_normal);
        if (pObject != NULL
            && pObject->owner != NULL
            && pObject->flags_0x238 == 1
            && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human) {
            if (pFace->material->identifier[1] == '!') {
                if (BrVector3Dot(&pObject->field_0xf4.min, &pObject->water_normal) - pObject->water_d < 0.0f) {
                    GetNewBoundingBox(&current_bounds, &pObject->bb1, &pObject->actor->t.t.mat);
                    if (BrVector3Dot(&current_bounds.min, &pObject->water_normal) - pObject->water_d < 0.0f) {
                        gIn_the_sea = 1;
                        FreezeCamera();
                    }
                }
                gDouble_pling_water = 1;
            } else {
                gDouble_pling_water = 0;
            }
        }
    } else {
        pObject->water_d = 10000.f;
        if (pObject != NULL
                && pObject->owner != NULL
                && pObject->flags_0x238 == 1
                && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human) {
            gIn_the_sea = gIn_the_sea == 1 ? 2 : 0;
        }
    }
    if (fabs(old_d - pObject->water_d) > 1e-05) {
        if (PHILGetObjectProperty(pObject, 6) != 0.f) {
            ARStartPipingSession(ePipe_extended_splash);
            AddExtendedSplashToPipingSession(pObject, pObject);
            AREndPipingSession();
        }
        if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
            tCar_spec* owner = (tCar_spec*)pObject->owner;
            if (owner != NULL && owner->driver > 5) {
                ARStartPipingSession(ePipe_chunk_splash);
                AddSplashToPipingSession(pObject);
                AREndPipingSession();
                return;
            }
            ARStartPipingSession(ePipe_extended_splash);
            AddExtendedSplashToPipingSession(pObject, pObject->actor);
            AREndPipingSession();
            return;
        }
        if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
            ARStartPipingSession(ePipe_extended_splash);
            AddExtendedSplashToPipingSession(pObject, pObject->owner);
            AREndPipingSession();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00414ca0
// FUNCTION: CARMA2_SW 0x00414cd0
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gIn_the_sea;
}

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

// FUNCTION: CARMA2_HW 0x004157e0
// FUNCTION: CARMA2_SW 0x00415810
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = (br_scalar)BrRadianToDegree(atan((pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (pCar->field_0x1260 + pCar->curvature)));
    pCar->lr_sus_position = (br_scalar)((pCar->wpos[0].v[1] - pCar->oldd[0]) / WORLD_SCALE_D);
    pCar->rr_sus_position = (br_scalar)((pCar->wpos[1].v[1] - pCar->oldd[1]) / WORLD_SCALE_D);
    pCar->lf_sus_position = (br_scalar)((pCar->wpos[2].v[1] - pCar->oldd[2]) / WORLD_SCALE_D);
    pCar->rf_sus_position = (br_scalar)((pCar->wpos[3].v[1] - pCar->oldd[3]) / WORLD_SCALE_D);
    PipeSingleGraphicalWheelStuff(pCar);
}

// FinishCars

// GetNonCars

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00415c40
// STUB: CARMA2_SW 0x00415c70
void C2_HOOK_FASTCALL GetNetPos(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL MungeCarsMass(tCar_spec* pCar, br_scalar pFactor) {

    pCar->collision_info->M *= pFactor;
    BrVector3Scale(&pCar->collision_info->I, &pCar->collision_info->I, pFactor);
}

// FUNCTION: CARMA2_HW 0x00415ef0
// FUNCTION: CARMA2_SW 0x00415f20
void C2_HOOK_FASTCALL AddDrag(const tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDelta_time) {
    br_scalar drag;
    br_scalar k;
    br_vector3 v;

    drag = pDelta_time * 0.0005f;
    if (pObject->last_special_volume != NULL) {
        k = pObject->last_special_volume->viscosity_multiplier;
        if (pCar->underwater_ability) {
            k = (br_scalar)(k * 0.6);
        }
        drag *= k;
        drag *= pObject->water_depth_factor;
    }

    k = (br_scalar)(BrVector3Length(&pObject->v) * drag * WORLD_SCALE_D / pObject->M);
    BrVector3Scale(&v, &pObject->v, k);
    DRVector3Diminish(&pObject->v, &v);

    k = BrVector3Length(&pObject->omega) * drag;
    BrVector3Scale(&v, &pObject->omega, k);
    ApplyDiminishingTorque(pObject, &v);
}

// FUNCTION: CARMA2_HW 0x00416030
// FUNCTION: CARMA2_SW 0x00416060
void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        AddDrag(pCar, pObject, 0.04f);
        if (pObject->child != NULL) {
            DragChildren(pCar, pObject->child);
        }
    }
}

inline void C2_HOOK_FASTCALL DoCarStuff(tCar_spec* pCar) {

    BrMatrix34Copy(&pCar->car_master_actor->t.t.mat, &pCar->collision_info->transform_matrix);
    if (pCar->collision_info->field_0x261 == 16
            && pCar->collision_info->message_time == gPHIL_last_physics_tick) {
        if (pCar == NULL || pCar->driver != eDriver_local_human) {
            pCar->curvature = (int)pCar->collision_info->field_0x278 * pCar->maxcurve / 32767.0f;
        }
        if (gNet_mode == eNet_mode_host) {
            pCar->collision_info->field_0xf0 = 1;
            GetNetPos(pCar);
        } else {
            GetNetPos(pCar);
        }
    }
    if (!pCar->disabled) {
        if (!(pCar->collision_info->disable_move_rotate
                && gPalette_fade_time != 0
                && pCar != NULL
                && pCar->driver == eDriver_local_human)) {
            if (pCar->collision_info->box_face_ref != gFace_num__car
                    && (pCar->collision_info->box_face_ref != gFace_num__car - 1
                        || pCar->collision_info->box_face_start <= gFace_count)) {
                GetFacesNearObject(pCar->collision_info, &gWorld_callbacks);
            }
            if (pCar->dt != 0.0f) {
                MoveAndCollideCar(pCar, 0.04f);
            }
            if (pCar->collision_info->child != NULL) {
                ApplyGravity(pCar->collision_info->child);
                DragChildren(pCar, pCar->collision_info->child);
                PositionChildren(pCar->collision_info);
            }
        }
    }
}

inline void C2_HOOK_FASTCALL DoNonCarStuff(tNon_car_spec* pNon_car) {

    if (pNon_car->collision_info->field_0x261 != 0
            && pNon_car->collision_info->message_time == gPHIL_last_physics_tick) {
        GetNetPos((tCar_spec*)pNon_car);
    }
    if (pNon_car->flags & 0x10000) {
        if (pNon_car->field_0xf8 != 0 && pNon_car->field_0xf8 < GetRaceTime()) {
            pNon_car->field_0xf8 = 0;
            pNon_car->flags = (pNon_car->flags & 0xffff0100) | 0x100;
            pNon_car->collision_info->disable_move_rotate = 0;
        }
    }
    if (!pNon_car->collision_info->disable_move_rotate
            || (pNon_car->flags & 0xff) != 0
            || (pNon_car->flags & 0xff00) != 0) {
        pNon_car->collision_info->disable_move_rotate = 0;
        if (pNon_car->field_0xc4 != 0.0f) {
            MoveAndCollideNonCar(pNon_car, 0.04f);
        }
        if (pNon_car->collision_info->child != NULL) {
            ApplyGravity(pNon_car->collision_info->child);
            PositionChildren(pNon_car->collision_info);
        }
    }
}

void C2_HOOK_FASTCALL RemoveFlyingCar(tCar_spec* pCar) {

    pCar->collision_info->transform_matrix.m[3][1] += 1000.0f;
    BrMatrix34Copy(&pCar->car_master_actor->t.t.mat, &pCar->collision_info->transform_matrix);
    PositionChildren(pCar->collision_info);
    SetHierarchyDoingNothingFlag(pCar->collision_info, 0x1);
}

// RestoreFlyingCar

void C2_HOOK_FASTCALL GetDrivableOnList(tPhysics_object *pObjects) {
    tPhysics_object* object;

    gDrivable_on_count = 0;
    for (object = pObjects; object != NULL && gDrivable_on_count < REC2_ASIZE(gDrivable_on_list); object = object->next) {
        if (object->drivable_on) {
            gDrivable_on_list[gDrivable_on_count++] = object;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00415890
// FUNCTION: CARMA2_SW 0x004158c0
void C2_HOOK_FASTCALL APTCPreCollision(void) {
    int i;

    GetDrivableOnList(gPHIL_list_collision_infos);

    BrVector3Copy(&gCar_to_view_original_v, &gCar_to_view->collision_info->v);

    for (i = 0; i < gNum_active_cars; i++) {
        if (gActive_car_list[i] != NULL && gActive_car_list[i]->driver > eDriver_5) {
            DoCarStuff(gActive_car_list[i]);
        }
    }
    for (i = 0; i < gNum_active_non_cars; i++) {
        DoNonCarStuff(gActive_non_car_list[i]);
    }
    for (i = 0; i < gNum_active_cars; i++) {
        if (gActive_car_list[i]->field_0x4c8 != 1.0f) {
            MungeCarsMass(gActive_car_list[i], gActive_car_list[i]->field_0x4c8);
        }
    }
    if (gCar_flying) {
        RemoveFlyingCar(gCar_to_view);
    }
    MinePreCollisionStuff();
    PedPreCollisionStuff();
    DronePreCollisionStuff();
}

// FUNCTION: CARMA2_HW 0x004161a0
// FUNCTION: CARMA2_SW 0x004161d0
void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tPhysics_object* pObject) {
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
        SetCollisionFlagsAndStuff(pObject->child);
    }
}

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004168c0
// STUB: CARMA2_SW 0x004168f0
void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00417030
// FUNCTION: CARMA2_SW 0x00417060
void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {

    if (pNon_car->collision_info->water_d != 10000.0f) {
        TestAutoSpecialVolume(pNon_car->collision_info);
    }
    MungeSpecialVolume(pNon_car->collision_info);
    if (pNon_car->field_0xc4 >= 0.0f) {
        pDt = pNon_car->field_0xc4;
    }
    NonCarCalcForce(pNon_car, pDt);
    BrMatrix34ApplyP(&pNon_car->pos, &pNon_car->centre_of_mass_world_scale, &pNon_car->car_master_actor->t.t.mat);
    BrVector3InvScale(&pNon_car->pos, &pNon_car->pos, WORLD_SCALE);
}

// FUNCTION: CARMA2_HW 0x00417180
// FUNCTION: CARMA2_SW 0x004171b0
void C2_HOOK_FASTCALL ControlCar2(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

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
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed += time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed -= time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
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
void C2_HOOK_FASTCALL ControlCar3(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

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

// FUNCTION: CARMA2_HW 0x00417a20
// FUNCTION: CARMA2_SW 0x00417a50
void C2_HOOK_FASTCALL ControlCar5(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

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
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
        if (pCar->curvature < 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature + (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature > 0.0f) {
                pCar->curvature = 0.0f;
            }
        } else if (pCar->curvature > 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature - (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00418250
// STUB: CARMA2_SW 0x00418280
void C2_HOOK_FASTCALL NonCarCalcForce(tNon_car_spec* pNon_car, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

// FUNCTION: CARMA2_HW 0x00418850
void C2_HOOK_FASTCALL CalcForce(tCar_spec* pCar, br_scalar pDt) {
    int i;
    int j;
    int normnum;
    int vol_mod;
    int count;
    int best_index;
    int ffb_force;
    br_scalar ts2;
    br_scalar d[4];
    tFace_ref* collected[4];
    br_scalar dd[4];
    br_scalar friction_number;
    br_scalar val[4];
    br_vector3 norm[4];
    br_scalar best;
    br_scalar wheelratio;
    br_scalar fr_oil_factor;
    br_scalar ts;
    br_scalar force[4];
    int ts3;
    br_scalar ts4;
    br_scalar maxfl;
    br_scalar v135;
    br_vector3 v;
    br_scalar v98;
    br_vector3 wheel_pos[4];
    br_scalar v87;
    br_scalar v106;
    br_scalar clamp_ratio;
    br_scalar v108;
    br_vector3 v103;
    br_scalar v109;
    br_scalar v128;
    br_scalar v129;
    br_scalar v134;
    br_scalar vdot;
    br_scalar maxfr;
    br_scalar pV;
    br_scalar fl_oil_factor;
    br_scalar k;
    br_scalar rl_oil_factor;
    br_matrix34* mat;
    int delta;
    br_vector3 b;
    br_vector3 f;
    br_vector3 B;
    br_scalar deltaomega;
    br_vector3 tmp;
    br_vector3 ray_dir;
    br_vector3 normal;
    br_vector3 rightplane;
    br_scalar rr_oil_factor;
    br_vector3 a;
    br_vector3 v136;
    br_vector3 v123;
    br_vector3 ray_pos[4];
    br_vector3 contact[4];
    br_scalar v99;
    br_bounds bounds;
    br_vector3 tv;
    tFace_ref* faces[4];
    tPhysics_object* objs[4];
    tPhysics_shape* shape;
    br_vector3 end;
    br_vector3 hit;
    tPhysics_object* pObj;
    tPhysics_object** pList;
    tFace_ref* pFace;
    tSpecial_volume* vol;
    br_scalar* pd;

#define road_normal (*(br_vector3*)(pCar->field_0x1348))
    B.v[0] = 0.f;
    B.v[1] = 0.f;
    B.v[2] = 0.f;
    pCar->curvature = pCar->curvature + pCar->field_0x1260;
    pCar->field_0x195c = 0;
    wheelratio = (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2])
        / (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]);
    faces[0] = NULL;
    faces[1] = NULL;
    faces[2] = NULL;
    faces[3] = NULL;
    normnum = 0;
    vol = pCar->collision_info->last_special_volume;
    mat = &pCar->car_master_actor->t.t.mat;
    objs[0] = NULL;
    objs[1] = NULL;
    objs[2] = NULL;
    objs[3] = NULL;
    vol_mod = 0;
    f.v[0] = 0.f;
    f.v[1] = 0.f;
    f.v[2] = 0.f;
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
    {
    br_scalar rt[4];
    for (i = pCar->collision_info->box_face_start, pFace = &gFace_list__car[i];
            i < pCar->collision_info->box_face_end;
            i++, pFace++) {
        if (gEliminate_faces != 0 && (pFace->flags & 0x80) != 0) {
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
    }
    bounds.max = ray_pos[0];
    bounds.min = ray_pos[0];
    for (i = 1; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            bounds.min.v[j] = bounds.min.v[j] < ray_pos[i].v[j] ? bounds.min.v[j] : ray_pos[i].v[j];
            bounds.max.v[j] = bounds.max.v[j] > ray_pos[i].v[j] ? bounds.max.v[j] : ray_pos[i].v[j];
        }
    }
    for (j = 0; j < 3; j++) {
        if (ray_dir.v[j] > 0.f) {
            bounds.max.v[j] = bounds.max.v[j] + ray_dir.v[j];
        } else {
            bounds.min.v[j] = bounds.min.v[j] + ray_dir.v[j];
        }
    }
    for (i = 0, pList = gDrivable_on_list; i < gDrivable_on_count; i++, pList++) {
        pObj = *pList;
        if (!BoundsOverlapTest(&bounds, &pObj->field_0xf4)) {
            continue;
        }
        shape = pObj->shape;
        BrMatrix34TApplyV(&tv, &ray_dir, &pObj->transform_matrix);
        if (shape == NULL) {
            continue;
        }
        do {
            for (j = 0; j < 4; j++) {
                DrMatrix34ApplyLPInverse(&tmp, &ray_pos[j], &pObj->transform_matrix);
                BrVector3Add(&end, &tmp, &tv);
                ShapeRayCast(&tmp, &end, shape, &hit, &ts, &normal);
                if (ts < d[j]) {
                    normal.v[0] = -normal.v[0];
                    normal.v[1] = -normal.v[1];
                    normal.v[2] = -normal.v[2];
                    BrMatrix34ApplyV(&pCar->nor[j], &normal, &pObj->transform_matrix);
                    d[j] = ts;
                    faces[j] = NULL;
                    objs[j] = pObj;
                }
            }
            shape = shape->common.next;
        } while (shape != NULL);
    }
    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->material_modifier_index != 0) {
        vol_mod = pCar->collision_info->last_special_volume->material_modifier_index;
    }
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
        if (gFriction_materials[pCar->material_index[i]].bumpiness != 0.f) {
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
            d[i] = delta * gFriction_materials[pCar->material_index[i]].bumpiness / 42400.f
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
        ts = force[i] * 0.14492753623188406 * pDt;
        tmp.v[0] = ts / objs[i]->M * b.v[0];
        tmp.v[1] = ts / objs[i]->M * b.v[1];
        tmp.v[2] = ts / objs[i]->M * b.v[2];
        objs[i]->v.v[0] = tmp.v[0] + objs[i]->v.v[0];
        objs[i]->v.v[1] = tmp.v[1] + objs[i]->v.v[1];
        objs[i]->v.v[2] = tmp.v[2] + objs[i]->v.v[2];
        if (gNet_mode == eNet_mode_host) {
            pCar->collision_info->field_0x49c = gPHIL_last_physics_tick + 40;
        }
        if ((objs[i]->flags & 0x800) == 0) {
            br_vector3 rt;
            BrMatrix34ApplyP(&rt, &objs[i]->cmpos, &objs[i]->transform_matrix);
            tv.v[0] = wheel_pos[i].v[0] * 0.14492753623188406;
            tv.v[1] = wheel_pos[i].v[1] * 0.14492753623188406;
            tv.v[2] = wheel_pos[i].v[2] * 0.14492753623188406;
            rt.v[0] = tv.v[0] - rt.v[0];
            rt.v[1] = tv.v[1] - rt.v[1];
            rt.v[2] = tv.v[2] - rt.v[2];
            tmp.v[0] = (rt.v[1] * b.v[2] - rt.v[2] * b.v[1]) * ts;
            tmp.v[1] = (rt.v[2] * b.v[0] - rt.v[0] * b.v[2]) * ts;
            tmp.v[2] = (rt.v[0] * b.v[1] - rt.v[1] * b.v[0]) * ts;
            BrMatrix34TApplyV(&rt, &tmp, &objs[i]->transform_matrix);
            for (j = 0; j < 3; j++) {
                objs[i]->omega.v[j] = rt.v[j] / objs[i]->I.v[j] + objs[i]->omega.v[j];
            }
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
            && gEliminate_faces != 3) {
        ((tNon_car_spec*)objs[0]->owner)->flags = (((tNon_car_spec*)objs[0]->owner)->flags & ~0xfe) | 1;
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
                friction_number = friction_number * 0.6;
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
        br_scalar nx;
        br_scalar ny;
        br_scalar nz;
        br_scalar len;

        len = BR_LENGTH3(road_normal.v[0], road_normal.v[1], road_normal.v[2]);
        if (len > BR_SCALAR_EPSILON * 2) {
            len = 1.0 / len;
            nx = road_normal.v[0] * len;
            ny = road_normal.v[1] * len;
            nz = road_normal.v[2] * len;
        } else {
            nx = 1.f;
            ny = 0.f;
            nz = 0.f;
        }
        k = -(pCar->collision_info->M * 10.0f);
        tmp.v[0] = nx * k;
        tmp.v[1] = ny * k;
        tmp.v[2] = nz * k;
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
            k = -(pCar->curvature * pDt);
            if (fabs(ts) < fabs(k) || ts * k < 0.f) {
                ts = k;
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
            contact[count].v[0] = -pCar->oldd[i] * pCar->car_master_actor->t.t.mat.m[1][0];
            contact[count].v[1] = -pCar->oldd[i] * pCar->car_master_actor->t.t.mat.m[1][1];
            contact[count].v[2] = -pCar->oldd[i] * pCar->car_master_actor->t.t.mat.m[1][2];
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
            tv.v[0] = -val[best_index] * pCar->car_master_actor->t.t.mat.m[1][0];
            tv.v[1] = -val[best_index] * pCar->car_master_actor->t.t.mat.m[1][1];
            tv.v[2] = -val[best_index] * pCar->car_master_actor->t.t.mat.m[1][2];
            SmashEnvironment(pCar->collision_info,
                (undefined4*)collected[best_index],
                val[best_index], &contact[best_index],
                &pCar->collision_info->v, &tv, 0, 0);
        }
        FlushSmashQueue(1);
        k = 1.0 / BR_LENGTH3(road_normal.v[0], road_normal.v[1], road_normal.v[2]);
        BrVector3Scale(&road_normal, &road_normal, k);
        BrVector3Scale(&ray_dir, &pCar->collision_info->velocity_car_space, WORLD_SCALE);
        friction_number = road_normal.v[0] * mat->m[0][1]
            + road_normal.v[2] * mat->m[2][1]
            + road_normal.v[1] * mat->m[1][1];
        if (pCar != NULL && pCar->driver > 5 && pCar->wall_climber_mode != 0) {
            friction_number = 1.f;
        }
        friction_number = gFriction_materials[pCar->material_index[0]].down_force * friction_number;
        if (friction_number > 0.f) {
            friction_number = (br_scalar)fabs(ray_dir.v[2])
                * pCar->collision_info->M * 10.f * friction_number / pCar->downforce_to_weight;
            if (friction_number > pCar->collision_info->M * 10.f) {
                friction_number = pCar->collision_info->M * 10.f;
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
                ActorFindFace(&pos, &delta, gTrack_actor, &normal, &ts, &material, NULL);
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
        k = BrVector3Dot(&road_normal, &ray_dir);
        v.v[0] = ray_dir.v[0] - k * road_normal.v[0];
        v.v[1] = ray_dir.v[1] - k * road_normal.v[1];
        v.v[2] = ray_dir.v[2] - k * road_normal.v[2];
        if (v.v[2] < 0.f) {
            ts = 1.f;
        } else {
            ts = -1.f;
        }
        ts2 = BrVector3Length(&v) * pCar->curvature;
        deltaomega = ts * ts2;
        deltaomega = deltaomega - BrVector3Dot(&pCar->collision_info->omega, &road_normal);
        BrVector3Set(&v103, road_normal.v[1], -road_normal.v[0], 0.f);
        BrVector3Normalise(&v103, &v103);
        friction_number = pCar->collision_info->I.v[1] * 47.61f / pDt * deltaomega;
        v108 = friction_number / (pCar->wpos[2].v[2] - pCar->wpos[0].v[2]);
        v109 = -v108;
        BrVector3Set(&rightplane, 0.f, road_normal.v[2], -road_normal.v[1]);
        BrVector3Normalise(&rightplane, &rightplane);
        v99 = pCar->acc_force;
        friction_number = BrVector3Dot(&rightplane, &v);
        vdot = BrVector3Dot(&v103, &v);
        ts2 = fabs(vdot);
        friction_number = (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) * friction_number * fabs(pCar->curvature);
        if (pCar->curvature <= 0.f) {
            friction_number = vdot - friction_number;
        } else {
            friction_number = vdot + friction_number;
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
        k = pCar->brake_force - v128;
        ts3 = (pCar->damage_units[7].damage_level + pCar->damage_units[6].damage_level) / 2;
        if (ts3 > 20) {
            v128 = (1.f - (ts3 - 20) / 80.f) * (1.f - (ts3 - 20) / 80.f) * v128;
        }
        ts3 = (pCar->damage_units[5].damage_level + pCar->damage_units[4].damage_level) / 2;
        if (ts3 > 20) {
            k = (1.f - (ts3 - 20) / 80.f) * (1.f - (ts3 - 20) / 80.f) * k;
        }
        ts2 = (force[1] + force[0]) * pCar->rolling_resistance + v128;
        v106 = (force[2] + force[3]) * pCar->steerable_rolling_resistance + k;
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
        v135 = (float)sqrt(v99 * v99 + v109 * v109) * 0.5f;
        GetOilFrictionFactors(pCar, &fl_oil_factor, &fr_oil_factor, &rl_oil_factor, &rr_oil_factor);
        for (i = 0; i < 4; i++) {
            if (pCar->oil_remaining[i] != 0.f) {
                NewSkidding(pCar, faces, i);
            }
        }
        if (pCar->driver <= 5) {
            k = 1.f;
        } else {
            k = pCar->grip_multiplier;
        }
        BrVector3Sub(&a, &pCar->wpos[0], &pCar->centre_of_mass_world_scale);
        BrVector3Cross(&a, &pCar->collision_info->omega, &a);
        BrVector3Accumulate(&a, &ray_dir);
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
        maxfl = sqrt(force[0]) * friction_number * (rl_oil_factor * k) * gFriction_materials[pCar->material_index[0]].tyre_road_friction;
        maxfr = sqrt(force[1]) * friction_number * (rr_oil_factor * k) * gFriction_materials[pCar->material_index[1]].tyre_road_friction;
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
                pCar->torque = -(pCar->revs * pCar->revs / 100000000.f) - 0.1;
            } else {
                float v177 = sqrt(pCar->max_force_rear * pCar->max_force_rear - v87 * v87);
                ts3 = ts2 < 0.f ? -1 : 1;
                ts4 = (ts2 - ts3 * v177) * 0.99f;
                if (fabs(ts2) > fabs(ts4)) {
                    ts2 = ts4;
                }
            }
            v99 = v99 - ts2;
            v135 = (float)sqrt(v99 * v99 + v109 * v109) * 0.5f;
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
        if (fabs(v109) > maxfr + maxfl && maxfr + maxfl > 0.1) {
            clamp_ratio = (maxfr + maxfl) / fabs(v109) * pDt;
            v109 = v109 * clamp_ratio;
            v99 = v99 * clamp_ratio * pCar->traction_multiplier;
            v109 = v109 / clamp_ratio;
            v99 = v99 / (clamp_ratio * pCar->traction_multiplier);
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
                if (maxfl > 0.1) {
                    NewSkidding(pCar, faces, 0);
                }
                if (maxfr > 0.1) {
                    NewSkidding(pCar, faces, 1);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1) {
                    SkidNoise(pCar, 0, pV, pCar->material_index[0]);
                }
            } else {
                if (maxfr > 0.1) {
                    SkidNoise(pCar, 1, pV, pCar->material_index[1]);
                }
            }
            break;
        case 1:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = v135 - force[0] + force[1];
            if (force[1] <= maxfr) {
                slide_dist = 0.f;
            } else {
                if (maxfr > 0.1) {
                    pV = (force[1] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        NewSkidding(pCar, faces, 1);
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
                if (maxfl > 0.1) {
                    pV = (force[0] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        NewSkidding(pCar, faces, 0);
                    }
                    SkidNoise(pCar, 0, pV, pCar->material_index[0]);
                }
                force[0] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 2;
            }
            break;
        case 0:
            slide_dist = 0.f;
            break;
        }
        v135 = (float)sqrt(v108 * v108 + v98 * v98) * 0.5f;
        if (v135 > 0.0001f) {
            v108 = v108 / (v135 * 2.0);
            v98 = v98 / (v135 * 2.0);
        }
        maxfl = sqrt(force[2]) * pCar->mu.v[1] * (fl_oil_factor * k) * gFriction_materials[pCar->material_index[2]].tyre_road_friction;
        maxfr = sqrt(force[3]) * pCar->mu.v[1] * (fr_oil_factor * k) * gFriction_materials[pCar->material_index[3]].tyre_road_friction;
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
                if (maxfr > 0.1) {
                    pV = (force[3] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        NewSkidding(pCar, faces, 3);
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
                if (maxfl > 0.1) {
                    pV = (force[2] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        NewSkidding(pCar, faces, 2);
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
                if (maxfl > 0.1) {
                    NewSkidding(pCar, faces, 2);
                }
                if (maxfr > 0.1) {
                    NewSkidding(pCar, faces, 3);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1) {
                    SkidNoise(pCar, 2, pV, pCar->material_index[2]);
                }
            } else {
                if (maxfr > 0.1) {
                    SkidNoise(pCar, 3, pV, pCar->material_index[3]);
                }
            }
            break;
        }
        v136.v[0] = v109 * v103.v[0] + v99 * rightplane.v[0];
        v136.v[1] = v109 * v103.v[1] + v99 * rightplane.v[1];
        v136.v[2] = v109 * v103.v[2] + v99 * rightplane.v[2];
        v123.v[0] = v108 * v103.v[0] + v98 * rightplane.v[0];
        v123.v[1] = v108 * v103.v[1] + v98 * rightplane.v[1];
        v123.v[2] = v108 * v103.v[2] + v98 * rightplane.v[2];
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
    ApplyTorque(pCar->collision_info, &rightplane);
    BrVector3Scale(&rightplane, &b, pDt / (pCar->collision_info->M * WORLD_SCALE));
    BrVector3Accumulate(&pCar->collision_info->v, &rightplane);
    if (pCar->speed < 0.0001f
        && ((!pCar->keys.acc && pCar->joystick.acc <= 0) || !pCar->gear)
        && !pCar->keys.dec
        && pCar->joystick.dec <= 0
        && pCar->bounce_rate == 0.f
        && BrVector3Length(&pCar->collision_info->omega) < 0.05) {
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

// SkidNoise

// StopSkid

// CrashNoise

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// STUB: CARMA2_HW 0x0041c1b0
// STUB: CARMA2_SW 0x0041c1e0
int C2_HOOK_FASTCALL ProcessForcesCallback(void* pArg1, float *pArg2, int pArg3) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0041e310
// FUNCTION: CARMA2_SW 0x0041e340
int C2_HOOK_FASTCALL ProcessJointForcesCallBack(void* pArg1, void* pArg2, void* pArg3) {

    return 0;
}

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

// FUNCTION: CARMA2_HW 0x0041e340
// FUNCTION: CARMA2_SW 0x0041e370
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

// findfloor

// FUNCTION: CARMA2_HW 0x0041e480
// FUNCTION: CARMA2_SW 0x0041e4b0
int C2_HOOK_FASTCALL FindFloorInBoxBU(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject) {
    br_vector3 nor2;
    br_vector3 collision_position;
    int i;
    int j;
    br_scalar dist;
    tFace_ref* face_ref;

    *pDistance = 2.f;
    i = pObject->box_face_start;
    face_ref = &gFace_list__car[i];
    for (; i < pObject->box_face_end; i++, face_ref++) {
        if (gEliminate_faces && (face_ref->flags & 0x80) != 0) {
            continue;
        }
        CheckSingleFace(face_ref, pPosition, pDirection, &nor2, &dist, &collision_position);
        if (dist < *pDistance) {
            *pDistance = dist;
            j = i;
            BrVector3Copy(pNormal, &nor2);
        }
    }
    if (*pDistance < 2.f) {
        i = gFace_list__car[j].material->identifier[0] - ('0' - 1);
        if (i >= 0 && i < 11) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041e580
// FUNCTION: CARMA2_SW 0x0041e5b0
void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {
    gQuite_wild_end = 0;
    gQuite_wild_start = 0;
    gOn_me_wheels_start = 0;
    gWoz_upside_down_at_all = 0;
    gWild_start = 0;
}

// SetAmbientPratCam

void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar) {
    pCar->field_0x18cc = 0;
    if (pCar->keys.brake || (pCar->brake_force != 0.f && (float)fabs(pCar->collision_info->velocity_car_space.v[2]) > 1.0 / (1200.0 * WORLD_SCALE_D))) {
        pCar->field_0x18cc |= 0x4;
    }
    if (pCar->gear < 0 || (!(pCar != NULL && pCar->driver == eDriver_local_human) && pCar->collision_info->velocity_car_space.v[2] > 0.f)) {
        pCar->field_0x18cc |= 0x8;
    }
}

// FUNCTION: CARMA2_HW 0x0041e5a0
// FUNCTION: CARMA2_SW 0x0041e5d0
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {
    int i;
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear, 0x135c);

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car->car_master_actor->render_style != BR_RSTYLE_NONE && !gAction_replay_mode) {
            SetTextureBits(car);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041e660
// STUB: CARMA2_SW 0x0041e690
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041f110
// FUNCTION: CARMA2_SW 0x0041f140
void C2_HOOK_FASTCALL DoLODCarModels(void) {
    int i;
    int j;
    tCar_spec* car;
    float level;
    float factor;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_simplification_factor) != 2);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_simplification_factor[0]) != 5);

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1 && car->car_master_actor->render_style != BR_RSTYLE_NONE) {
            level = Vector3DistanceSquared(&car->car_master_actor->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]);
            factor = gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level];
            if (!(factor >= 0.001f)) {
                level = BR_SCALAR_MAX;
            } else {
                level /= factor;
            }
#ifdef REC2_SW
            if (gNet_mode != eNet_mode_none && (gNet_players[gIt_or_fox].car == car || NetPlayerFromCar(car)->field_0x80 != 0)) {
                level = 0.0f;
            }
#endif
            for (j = car->count_detail_levels - 1; j > 0; j--) {
                if (level > car->detail_levels[j]) {
                    break;
                }
            }
            if (j > 0) {
                int variant;

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

// FUNCTION: CARMA2_HW 0x0041f280
// FUNCTION: CARMA2_SW 0x0041f300
intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* crush_data = pActor->user;

    (void)pData;
    if (crush_data != NULL && crush_data->groove == NULL) {
        pActor->type = BR_ACTOR_NONE;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041f2a0
// FUNCTION: CARMA2_SW 0x0041f320
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

// FUNCTION: CARMA2_HW 0x0041f300
// FUNCTION: CARMA2_SW 0x0041f380
void C2_HOOK_FASTCALL ResetCarScreens(void) {
    int cat;
    int car_count;
    int i;
    tCar_spec* the_car;

    for (cat = eVehicle_self; cat <= eVehicle_rozzer; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            the_car->collision_info->last_special_volume = NULL;
        }
    }
    MungeCarGraphics(gFrame_period);
}

// FUNCTION: CARMA2_HW 0x0041f360
// FUNCTION: CARMA2_SW 0x0041f3e0
void C2_HOOK_FASTCALL ToggleCarToCarCollisions(void) {

    gCar_car_collisions = !gCar_car_collisions;
    if (gCar_car_collisions) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Car Car Collisions");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Ghost Cars");
    }
}

// FUNCTION: CARMA2_HW 0x0041f3b0
// FUNCTION: CARMA2_SW 0x0041f430
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

// FUNCTION: CARMA2_HW 0x0041f450
// FUNCTION: CARMA2_SW 0x0041f4d0
void C2_HOOK_FASTCALL FreezeMechanics(void) {

    gFreeze_mechanics = !gFreeze_mechanics;
    if (gFreeze_mechanics) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Mechanics Frozen");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Thawed Mechanics");
    }
}

// FlyCar

// GetCarOverallBoundsMinY

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041fc60
// STUB: CARMA2_SW 0x0041fce0
void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041fe50
// FUNCTION: CARMA2_SW 0x0041fed0
int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3) {
    int r;

    if (!gProgram_state.racing) {
        return 1;
    }
    gTesting_car_for_sensible_place = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info,
        gPHIL_list_collision_infos,
        pVec3,
        &gWorld_callbacks);
    gTesting_car_for_sensible_place = 0;
    return r;
}

// FUNCTION: CARMA2_HW 0x0041ff00
// FUNCTION: CARMA2_SW 0x0041ff80
tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* pActor) {
    if (!gPHIL_doing_physics && !gTesting_car_for_sensible_place) {
        return NULL;
    } else {
        return DoPullActorFromWorld(pActor);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041ff20
// STUB: CARMA2_SW 0x0041ffa0
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* pActor) {
    NOT_IMPLEMENTED();
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// FUNCTION: CARMA2_HW 0x004207e0
// FUNCTION: CARMA2_SW 0x00420860
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
// FUNCTION: CARMA2_SW 0x00420900
int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref != gFace_num__car) {
        if (pCar->collision_info->box_face_ref != gFace_num__car - 1 || pCar->collision_info->box_face_start <= gFace_count) {
            return 0;
        }
    }
    *pFace_refs = &gFace_list__car[pCar->collision_info->box_face_start];
    return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
}

// FUNCTION: CARMA2_HW 0x004208f0
// FUNCTION: CARMA2_SW 0x00420970
void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x00420900
// FUNCTION: CARMA2_SW 0x00420980
void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_NONE;
}
