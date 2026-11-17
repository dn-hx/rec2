/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "59-camera.h"

#include "01-network.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "42-input.h"
#include "57-drone.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0058f5fc
// GLOBAL: CARMA2_SW 0x0058a5fc
int gNet_player_to_view_index = -1;

// GLOBAL: CARMA2_HW 0x00679278
// GLOBAL: CARMA2_SW 0x006739e8
int gAction_replay_manual_camera_target_type;

// GLOBAL: CARMA2_HW 0x006792b4
// GLOBAL: CARMA2_SW 0x00673a24
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x006792b8
// GLOBAL: CARMA2_SW 0x00673a28
int gCamera_frozen;

// GLOBAL: CARMA2_HW 0x006792bc
// GLOBAL: CARMA2_SW 0x00673a2c
int gOpponent_viewing_mode;

// GLOBAL: CARMA2_HW 0x006792c8
// GLOBAL: CARMA2_SW 0x00673a38
tSave_camera gSave_camera[2];

// GLOBAL: CARMA2_HW 0x007634ac
// GLOBAL: CARMA2_SW 0x00759a0c
float gCamera_cockpit_hither;

// GLOBAL: CARMA2_HW 0x0079efa8
// GLOBAL: CARMA2_SW 0x00795548
tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// FUNCTION: CARMA2_HW 0x0040f4a0
// FUNCTION: CARMA2_SW 0x0040f480
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {
    int i;
    int score;
    tCar_spec* car;

    car = gNet_players[0].car;
    score = gNet_players[0].last_score_index;
    if (gCurrent_net_game->type == eNet_game_type_foxy && gIt_or_fox >= 0 && gIt_or_fox < gNumber_of_net_players) {
        return gNet_players[gIt_or_fox].car;
    } else {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].last_score_index < score) {
                score = gNet_players[i].last_score_index;
                car = gNet_players[i].car;
            }
        }
        return car;
    }
}

// AmIGettingBoredWatchingCameraSpin

// FUNCTION: CARMA2_HW 0x0040e430
// FUNCTION: CARMA2_SW 0x0040e410
void C2_HOOK_FASTCALL ViewNetPlayer(void) {
    tCar_spec* car;
    tNet_game_player_info* net_player;
    tNet_message* message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, last_score_index, 0x94);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_view_car, player, 0x4);

    if (!gOpponent_viewing_mode) {
        return;
    }
    gNet_player_to_view_index++;
    if (gNet_player_to_view_index >= gNumber_of_net_players) {
        gNet_player_to_view_index = -1;
    }
    if (gNet_player_to_view_index >= 0) {
        car = gNet_players[gNet_player_to_view_index].car;
    } else {
        car = GetRaceLeader();
    }
    net_player = NetPlayerFromCar(car);
    if (gNet_mode == eNet_mode_host) {
        gCar_to_view = car;
        gCamera_yaw = 0;
        InitialiseExternalCamera();
        PositionExternalCamera(gCar_to_view, 200);
    } else if (gNet_mode == eNet_mode_client && gCar_to_view_id != net_player->ID) {
        gCar_to_view_id = net_player->ID;
        message = NetBuildGuaranteedMessage(eNet_message_chunk_type_view_car, 0);
        message->contents.view_car.player = gCar_to_view_id;
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
    }
}

// FUNCTION: CARMA2_HW 0x0040e590
// FUNCTION: CARMA2_SW 0x0040e570
void C2_HOOK_FASTCALL ViewOpponent(void) {
    tNet_message* message;

    // GLOBAL: CARMA2_HW 0x0058f628
    // GLOBAL: CARMA2_SW 0x0058a628
    static int n = -1;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_view_car, player, 0x4);

    n++;
    if (gNet_mode != eNet_mode_none) {
        if (n >= gNumber_of_net_players) {
            n = 0;
        }
        if (gNet_mode == eNet_mode_host) {
            gCar_to_view = gNet_players[n].car;
        } else if (gCar_to_view_id == 0) {
            gCar_to_view_id = gNet_players[n].ID;
            message = NetBuildGuaranteedMessage(eNet_message_chunk_type_view_car, 0);
            message->contents.view_car.player = gCar_to_view_id;
            NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
            return;
        }
        NewTextHeadupSlot(4, 0, 2000, -3, gNet_players[n].player_name);
    } else {
        if (n >= gNum_viewable_cars) {
            n = 0;
        }
        gCar_to_view = gViewable_car_list[n];
        NewTextHeadupSlot(4, 0, 2000, -3, gViewable_car_list[n]->driver_name);
    }
    gCamera_yaw = 0;
    InitialiseExternalCamera();
    PositionExternalCamera(gCar_to_view, 200);
}

// FUNCTION: CARMA2_HW 0x0040e680
// FUNCTION: CARMA2_SW 0x0040e660
void C2_HOOK_FASTCALL ReallyViewOpponent(void) {
    int i;

    if (gNet_mode == eNet_mode_client) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == gCar_to_view_id) {
                gCar_to_view_id = 0;
                gCamera_yaw = 0;
                gCar_to_view = gNet_players[i].car;
                InitialiseExternalCamera();
                PositionExternalCamera(gCar_to_view, 200);
                return;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0040f510
// FUNCTION: CARMA2_SW 0x0040f4f0
void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar) {
    br_matrix34* mat;
    br_scalar height;
    int i;

    height = 0.f;
    if (pCar->collision_info->water_d != 10000.f) {
        mat = &pCar->car_master_actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            if (mat->m[i][1] > 0.f) {
                height += pCar->collision_info->bb2.max.v[i] * mat->m[i][1];
            } else {
                height += pCar->collision_info->bb2.min.v[i] * mat->m[i][1];
            }
        }
        if ((mat->m[3][1] / WORLD_SCALE) + height < pCar->collision_info->water_d) {
            DisablePlingMaterials();
        }
    } else {
        DisablePlingMaterials();
    }
}

// SetPanningFieldOfView

int C2_HOOK_FASTCALL IsCameraTypeAllowed(int pCamera_mode) {
    if (gAction_replay_mode && !gCamera_type_allowed_replay[pCamera_mode]) {
        return 0;
    }
    if (!gAction_replay_mode && !gCamera_type_allowed_gameplay[pCamera_mode]) {
        return 0;
    }
    if (pCamera_mode >= 9) {
        return 0;
    }
    if (pCamera_mode == 6 && !OKToViewDrones()) {
        return 0;
    }
    if (pCamera_mode == 5 && gPed_count == 0) {
        return 0;
    }
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040e700
// FUNCTION: CARMA2_SW 0x0040e6e0
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    int i;

    for (i = 0; i < pCar->count_window_materials; i++) {
        int internal_map_missing = pInternal_cam;

        if (pInternal_cam) {
            int j;

            for (j = 0; j < pCar->window_materials[i].count_maps; j++) {
                if (pCar->window_materials[i].material->colour_map == pCar->window_materials[i].maps[j]) {
                    internal_map_missing = 0;
                }
            }
            if (internal_map_missing) {
                pCar->window_materials[i].material->flags &= ~BR_MATF_TWO_SIDED;
            }
        }
        if (!internal_map_missing) {
            pCar->window_materials[i].material->flags |= BR_MATF_TWO_SIDED;
        }
        BrMaterialUpdate(pCar->window_materials[i].material, BR_MATU_RENDERING);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040e790
// FUNCTION: CARMA2_SW 0x0040e770
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

    gAction_replay_camera_mode = pCamPos;
    switch (pCamPos) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        gAction_replay_manual_camera_target_type = 0;
        break;
    default:
        break;
    }
    MungeCarMaterials(&gProgram_state.current_car, gAction_replay_camera_mode == kActionReplayCameraMode_Internal);
}

// FUNCTION: CARMA2_HW 0x0040e7f0
// FUNCTION: CARMA2_SW 0x0040e7d0
void C2_HOOK_FASTCALL ChangeCameraType(void) {
    tActionReplayCameraMode mode;

    if (KeyIsDown(8)) {
        mode = gAction_replay_camera_mode;
        gAction_replay_camera_mode = mode - 1;
        if (gAction_replay_camera_mode >= 9 || gAction_replay_camera_mode < 0) {
            gAction_replay_camera_mode = 8;
        }
    } else {
        mode = gAction_replay_camera_mode;
        gAction_replay_camera_mode = mode + 1;
        if (gAction_replay_camera_mode >= 9) {
            gAction_replay_camera_mode = 0;
        }
    }
    if (!IsCameraTypeAllowed(gAction_replay_camera_mode)) {
        ChangeCameraType();
    }
    if (gAction_replay_mode) {
        gAR_camera_type = gAction_replay_camera_mode;
    } else {
        gCamera_type = gAction_replay_camera_mode;
    }
    SetCameraType(gAction_replay_camera_mode);
}

// FUNCTION: CARMA2_HW 0x0040e900
// FUNCTION: CARMA2_SW 0x0040e8e0
void C2_HOOK_FASTCALL ToggleCockpit(void) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_type_allowed_replay) != 9);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_type_allowed_gameplay) != 9);

    if ((gCar_to_view != &gProgram_state.current_car && !gProgram_state.cockpit_on) || gMap_view == 2) {
        return;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        tActionReplayCameraMode old_camera_mode = gAction_replay_camera_mode_00679324;

        if ((gAction_replay_mode && !gCamera_type_allowed_replay[gAction_replay_camera_mode_00679324])
                || (!gAction_replay_mode && !gCamera_type_allowed_gameplay[gAction_replay_camera_mode_00679324])
                || gAction_replay_camera_mode_00679324 >= (int)REC2_ASIZE(gCamera_type_allowed_replay)
                || (gAction_replay_camera_mode_00679324 == kActionReplayCameraMode_Drone && !OKToViewDrones())
                || (old_camera_mode == kActionReplayCameraMode_Peds && !gPed_count)) {
            SetCameraType(kActionReplayCameraMode_Standard);
        } else {
            SetCameraType(gAction_replay_camera_mode_00679324);
        }
    } else {
        gAction_replay_camera_mode_00679324 = gAction_replay_camera_mode;
        SetCameraType(kActionReplayCameraMode_Internal);
    }
}

// PositionCarMountedCamera

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040ea30
// STUB: CARMA2_SW 0x0040ea10
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040f760
// FUNCTION: CARMA2_SW 0x0040f740
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime) {

    if (gAction_replay_mode
            && (gAction_replay_camera_mode == kActionReplayCameraMode_ActionTracking || gAction_replay_camera_mode == kActionReplayCameraMode_Panning)
            && gPed_actor != NULL
            && !gProgram_state.cockpit_on) {
        IncidentCam(pCar, pTime);
    }
}

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040f790
// STUB: CARMA2_SW 0x0040f770
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

// FUNCTION: CARMA2_HW 0x00410be0
// FUNCTION: CARMA2_SW 0x00410bc0
void C2_HOOK_FASTCALL SaveCameraPosition(int pIndex) {

    if (gSave_camera[pIndex].saved == 1) {
        return;
    }
    gSave_camera[pIndex].zoom = gCamera_zoom;
    gSave_camera[pIndex].yaw = gCamera_yaw;
    gSave_camera[pIndex].saved = 1;
}

// FUNCTION: CARMA2_HW 0x00410c20
// FUNCTION: CARMA2_SW 0x00410c00
void C2_HOOK_FASTCALL RestoreCameraPosition(int pIndex) {

    if (gSave_camera[pIndex].saved == 0) {
        return;
    }
    gCamera_zoom = gSave_camera[pIndex].zoom;
    gCamera_yaw = gSave_camera[pIndex].yaw;
    gSave_camera[pIndex].saved = 0;
}

// DrVector3RotateY

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// FUNCTION: CARMA2_HW 0x00411980
void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference, tCar_spec* pCar) {
    br_angle yaw;
    br_angle yaw2;
    br_angle omega;
    br_angle v8;
    br_angle v9;
    br_scalar sign;
    br_scalar dot;
    br_scalar v16;
    br_scalar v17;
    br_scalar v18;
    br_scalar abs_v18;
    br_scalar cos_dtheta;
    br_scalar c;
    br_scalar s;
    br_scalar ts;
    int manual_swing;

    yaw = gCamera_yaw;
    manual_swing = gOld_yaw__car != gCamera_yaw;
    if (elapsed_time > 500) {
        elapsed_time = -1;
    }
    if (elapsed_time >= 0) {
        elapsed_time += pTime_difference;
    }
    sign = -BrVector3Dot((br_vector3*)pM1->m[2], pVn);
    dot = BrVector3Dot(&old_vn, pVn);
    if ((sign < 0.f) == gCamera_sign) {
        elapsed_time = -1;
    } else if (dot <= 0.f || elapsed_time >= 0) {
        if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || gCamera_sign != 0) {
            if (elapsed_time < 0) {
                elapsed_time = 0;
            }
            if (elapsed_time >= 500 || sign > 0.f) {
                elapsed_time = 500;
                if ((sign > 0.f ? 0.0001 : 0.0006) < fabs(pSpeedo_speed) && gCar_flying == 0) {
                    gCamera_sign = !gCamera_sign;
                    gUNK_006792f4 = BrDegreeToAngle(200.0 * (gUNK_006792f8 ? 0.06 : 0.04));
                    yaw2 = gCamera_yaw;
                    if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                        yaw2 = gCamera_yaw - BR_ANGLE_DEG(180);
                    }
                    if ((br_angle)(gCamera_yaw + BR_ANGLE_DEG(90)) <= BR_ANGLE_DEG(180)) {
                        if (yaw2 > BR_ANGLE_DEG(45) && yaw2 < BR_ANGLE_DEG(135)) {
                            gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                            yaw = gCamera_yaw;
                        }
                    } else {
                        gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                        yaw = gCamera_yaw;
                    }
                    goto camera_positioned;
                }
            }
        }
        BrVector3Negate(pVn, pVn);
        yaw = gCamera_yaw;
    } else {
        gCamera_sign = !gCamera_sign;
        yaw2 = gCamera_yaw;
        if (gCamera_yaw > BR_ANGLE_DEG(180)) {
            yaw2 = gCamera_yaw - BR_ANGLE_DEG(180);
        }
        if (yaw2 > BR_ANGLE_DEG(45) && yaw2 < BR_ANGLE_DEG(135)) {
            gCamera_yaw = -gCamera_yaw;
            yaw = gCamera_yaw;
        }
    }
camera_positioned:
    BrVector3Copy(&old_vn, pVn);
    if (gCamera_sign != 0) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    if (gCar_flying == 0) {
        c = cosf(BrAngleToRadian(yaw));
        s = sinf(BrAngleToRadian(yaw));
        ts = pVn->v[0] * c + pVn->v[2] * s;
        pVn->v[2] = pVn->v[2] * c - pVn->v[0] * s;
        pVn->v[0] = ts;
    }
    v16 = pVn->v[0] * gView_direction.v[0] + pVn->v[2] * gView_direction.v[2];
    v17 = pVn->v[0] * gView_direction.v[2] - pVn->v[2] * gView_direction.v[0];
    if (v16 < 0.5 && gCamera_yaw == 0) {
        gUNK_006792f8 = 1;
    }
    v18 = pOmega->v[0] * pM1->m[0][1] + pOmega->v[1] * pM1->m[1][1] + pOmega->v[2] * pM1->m[2][1];
    abs_v18 = fabs(v18);
    v8 = BrRadianToAngle(pTime_difference * (abs_v18 - -0.08726646f) * 0.001f);
    if (gUNK_006792f4 == 0 && v16 > 0.f) {
        v9 = BrRadianToAngle(sqrt(fabs(v17)));
        if (v9 < v8) {
            goto camera_reset;
        }
    }
    if (gCar_flying == 0 && manual_swing != 0) {
        if (gUNK_006792f4 == 0) {
            gUNK_006792f4 = BrDegreeToAngle(50.0 * (gUNK_006792f8 ? 0.06 : 0.04));
        }
        omega = pTime_difference * gUNK_006792f4 / 100;
        if (omega < v8) {
            omega = v8;
        }
        cos_dtheta = BR_COS(omega);
        if (cos_dtheta > v16) {
            if (v16 < -0.7 && abs_v18 > 0.8 && v18 * v17 > 0.f) {
                omega = -omega;
            }
            ts = BrAngleToRadian(omega);
            if (v17 > 0.f) {
                pVn->v[0] = cosf(ts) * gView_direction.v[0] + sinf(ts) * gView_direction.v[2];
                pVn->v[2] = cosf(ts) * gView_direction.v[2] - sinf(ts) * gView_direction.v[0];
            } else {
                pVn->v[0] = cosf(ts) * gView_direction.v[0] - sinf(ts) * gView_direction.v[2];
                pVn->v[2] = cosf(ts) * gView_direction.v[2] + sinf(ts) * gView_direction.v[0];
            }
            gUNK_006792f4 += BrDegreeToAngle(pTime_difference * (gUNK_006792f8 ? 0.06 : 0.04));
            if (gUNK_006792f4 > BrDegreeToAngle(gUNK_006792f8 ? 40.0 : 10.0)) {
                gUNK_006792f4 = BrDegreeToAngle(gUNK_006792f8 ? 40.0 : 10.0);
            }
            return;
        }
    }
camera_reset:
    gUNK_006792f4 = 0;
    gCamera_mode = 0;
    gUNK_006792f8 = 0;
}

// PointCameraAtCar

// FUNCTION: CARMA2_HW 0x004121b0
// FUNCTION: CARMA2_SW 0x00412190
void C2_HOOK_FASTCALL PointCamera(const br_vector3* pPos, br_matrix34* pMat) {
    br_vector3 vn;
    br_scalar dist;
    br_angle theta;
    br_camera* camera_ptr;

    camera_ptr = gCamera->type_data;
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[3]);
    vn.v[1] = 0.0f;
    BrVector3Normalise(&vn, &vn);
    pMat->m[0][0] = -vn.v[2];
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = vn.v[0];
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -vn.v[0];
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -vn.v[2];
    dist = BR_LENGTH2(pMat->m[3][0] - pPos->v[0], pMat->m[3][2] - pPos->v[2]);
    theta = BR_ATAN2(pMat->m[3][1] - pPos->v[1], dist);
    BrMatrix34PreRotateX(pMat, camera_ptr->field_of_view / 5 - theta);
}

// CollideCamera2

// FUNCTION: CARMA2_HW 0x00413570
// FUNCTION: CARMA2_SW 0x004135a0
int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos) {
    (void)pPos;
    (void)pCamera_pos;
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00413580
// STUB: CARMA2_SW 0x004135b0
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00413770
// FUNCTION: CARMA2_SW 0x004137a0
void C2_HOOK_FASTCALL FreezeCamera(void) {

    gCamera_frozen = 1;
}

// FUNCTION: CARMA2_HW 0x00413780
// FUNCTION: CARMA2_SW 0x004137b0
void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race) {
    int i;
    br_scalar total_cars;
    tCar_spec* car;

    total_cars = 0.0f;
    BrVector3SetFloat(&gAverage_grid_position, 0.0f, 0.0f, 0.0f);
    if (pThe_race->number_of_racers <= 2) {
        BrVector3Copy(&gAverage_grid_position, &gProgram_state.current_car.pos);
    } else {
        for (i = 0; i < pThe_race->number_of_racers; i++) {
            car = pThe_race->opponent_list[i].car_spec;
            BrVector3Accumulate(&gAverage_grid_position, &car->pos);
            total_cars += 1.0f;
        }
        gAverage_grid_position.v[0] /= total_cars;
        gAverage_grid_position.v[1] /= total_cars;
        gAverage_grid_position.v[2] /= total_cars;
    }
}

// FindACar

// FindAPed

// MouseClickInActionReplay

