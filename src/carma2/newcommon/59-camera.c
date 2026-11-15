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
#include "53-controls.h"
#include "00-car.h"
#include "49-physics.h"

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

// FUNCTION: CARMA2_HW 0x0040f590
void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime) {
    // GLOBAL: CARMA2_HW 0x00679300
    static float swing_rate;
    // GLOBAL: CARMA2_HW 0x0058f624
    static float swing_frac = 0.f;
    br_matrix34* mat;
    float inv;
    br_matrix34* src;
    tU8 flags;
    float x;
    float y;
    float z;

    mat = &gCamera->t.t.mat;
    src = &pCar->car_master_actor->t.t.mat;
    flags = *(tU8*)&gCamera_key_flags;
    if ((flags & 2) && !(flags & 1) && swing_rate >= 0.f) {
        swing_rate = -0.0002f;
    } else if ((flags & 4) && !(flags & 1) && swing_rate <= 0.f) {
        swing_rate = 0.0002f;
    }
    swing_frac = (float)pTime * swing_rate + swing_frac;
    if (swing_frac > 1.f) {
        swing_rate = 0.f;
        swing_frac = 1.f;
    } else if (swing_frac < 0.f) {
        swing_rate = 0.f;
        swing_frac = 0.f;
    }
    inv = 1.f - swing_frac;
    x = pCar->camera_bumper_position.v[0] * inv + pCar->camera_cockpit_position.v[0] * swing_frac;
    y = pCar->camera_bumper_position.v[1] * inv + pCar->camera_cockpit_position.v[1] * swing_frac;
    z = pCar->camera_bumper_position.v[2] * inv + pCar->camera_cockpit_position.v[2] * swing_frac;
    BrMatrix34Copy(mat, src);
    BrMatrix34PreTranslate(mat, x, y, z);
    if (swing_rate < 0.f) {
        swing_rate = swing_rate - 0.0001;
    }
    if (swing_rate > 0.f) {
        swing_rate = swing_rate - -0.0001;
    }
    CollideCamera2(&pCar->pos, &gCamera->t.t.translate.t, NULL, 0, NULL);
}

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

// SwingCamera

// PointCameraAtCar

// PointCamera

// FUNCTION: CARMA2_HW 0x004122b0
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tPhysics_object* collision_info) {
    int i;
    br_vector3 cam_hither_pos;
    br_vector3 car_coll_pos;
    br_vector3 dir;
    br_scalar ts;
    br_scalar dist;
    br_scalar hither;
    tBounds bnds;
    br_actor* actor;
    br_matrix34 mat;
    br_vector3 tv;
    tFace_ref face_list[6];
    br_scalar face_dots[6];
    int count_min_dots;
    int index_min_dots[3];
    int loop_done;
    int count;
    br_vector3 nor;
    br_vector3 delta;

    hither = ((br_camera*)gCamera->type_data)->hither_z * 3.0f;
    gCamera_has_collided = 0;
    loop_done = 0;
    for (;;) {
        br_vector3 new_cam_pos;
        br_vector3 tv2;
        br_vector3 p1;
        br_vector3 p2;
        br_vector3 nor_mod;
        br_material* material;
        br_scalar cos_coll;
        br_scalar delta_hither;
        br_scalar nor_factor;
        int i;

        BrVector3Sub(&delta, cam_pos, car_pos);
        dist = BrVector3Length(&delta);
        BrVector3Scale(&delta, &delta, 1.25f);
        ActorFindFace(car_pos, &delta, gTrack_actor, &nor, &ts, &material, &actor);
        if (ts >= 1.0) {
            BrVector3Copy(&new_cam_pos, cam_pos);
        } else {
            gCamera_has_collided = 1;
            BrVector3Scale(&tv, &delta, ts);
            BrVector3Add(&new_cam_pos, car_pos, &tv);
        }
        BrVector3Normalise(&dir, &delta);

        BrVector3Scale(&tv2, &dir, -hither);
        BrVector3Add(&cam_hither_pos, &new_cam_pos, &tv2);
        BrVector3Add(&car_coll_pos, car_pos, &delta);

        for (i = 0; i < gNum_active_non_cars; i++) {
            tNon_car_spec* non_car = gActive_non_car_list[i];

            if (non_car->flags & 0x80000) {
                br_scalar factor;
                br_vector3 temp_normal;

                DrMatrix34ApplyLPInverse(&p1, &cam_hither_pos, &non_car->collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal)) {
                    gCamera_has_collided = 1;
                    DrMatrix34ApplyLPInverse(&p1, car_pos, &non_car->collision_info->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                    if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal) && factor < ts) {
                        BrVector3Copy(&nor, &temp_normal);
                        actor = NULL;
                        BrMatrix34ApplyP(&tv, &new_cam_pos, &non_car->collision_info->actor->t.t.mat);
                        BrVector3Copy(&new_cam_pos, &tv);
                    }
                }
            }
        }
        if (ts > 1.f) {
            break;
        }
        cos_coll = BrVector3Dot(&nor, &delta);
        if (cos_coll <= 0.f) {
            cos_coll = -cos_coll;
        } else {
            BrVector3Negate(&nor, &nor);
        }
        if (loop_done) {
            break;
        }
        delta_hither = hither + cos_coll / 1.25f - ts * cos_coll;
        if (delta_hither <= 0.f) {
            break;
        }
        BrVector3Copy(&nor_mod, &nor);
        nor_mod.v[1] += nor.v[1] >= 0.7f ? 3.f : -3.f;
        nor_factor = fabs(nor.v[1]) * delta_hither / BrVector3Length(&nor_mod);
        BrVector3Scale(&tv, &nor_mod, nor_factor);
        BrVector3Accumulate(cam_pos, &tv);

        BrVector3Sub(&delta, cam_pos, car_pos);
        BrVector3Normalise(&delta, &delta);
        BrVector3Scale(&delta, &delta, dist);
        BrVector3Add(cam_pos, car_pos, &delta);
        loop_done = 1;
    }
    if (ts <= 1.f) {
        br_scalar dt;
        br_scalar l;

        gCamera_has_collided = 1;
        if (actor != NULL && actor->identifier != NULL && actor->identifier[0] == '&') {
            br_vector3 tv;

            BrMatrix34ApplyV(&tv, &nor, &actor->t.t.mat);
            BrVector3Copy(&nor, &tv);
        }
        dt = -(hither / BrVector3Dot(&nor, &delta));
        ts -= dt;
        if (ts * dist < 1.f) {
            ts += dt - 0.001f;
            if (ts >= 1.f / dist) {
                ts = 1.f / dist;
            }
        }
        if (ts > .8f) {
            ts = .8f;
        }
        BrVector3Scale(&delta, &delta, ts);
        l = BrVector3LengthSquared(&delta);
        BrVector3Add(cam_pos, car_pos, &delta);
        if ((float)sqrt(l) < gMin_camera_car_distance && !loop_done) {
            br_scalar a;
            br_scalar b;
            br_scalar discr;

            BrVector3Scale(&tv, &nor, -nor.v[1]);
            tv.v[1] += 1.f;
            if (gProgram_state.current_car.car_master_actor->t.t.mat.m[1][1] < 0.f) {
                BrVector3Negate(&tv, &tv);
            }
            a = BrVector3LengthSquared(&tv);
            b = BrVector3Dot(&tv, &delta);
            discr = REC2_SQR(b) - 4.f * a * (l - REC2_SQR(gMin_camera_car_distance));
            if (discr >= 0.f && a != 0.f) {
                br_material* material;
                br_vector3 tv2;
                br_vector3 tv3;
                br_scalar f;
                br_scalar dot;

                f = (sqrt(discr) - b) / (2 * a);
                BrVector3Scale(&tv, &tv, f);
                FindFace(cam_pos, &tv, &nor, &ts, &material);
                if (ts < 1.f) {
                    BrVector3Scale(&tv, &tv, ts);
                }
                BrVector3Copy(&tv2, &delta);
                tv2.v[1] = 0.f;
                BrVector3Normalise(&tv3, &tv2);
                BrVector3Accumulate(&delta, &tv);
                dot = BrVector3Dot(&tv3, &delta);
                if (dot < .03f && !gAction_replay_mode) {
                    br_scalar dot2;

                    BrVector3Normalise(&tv, &tv);
                    dot2 = BrVector3Dot(&tv3, &tv);
                    if (dot2 < -.03f) {
                        a = (.03f - dot) / dot2;
                        BrVector3Scale(&tv, &tv, a);
                        BrVector3Accumulate(&delta, &tv);
                    }
                }
            }
            BrVector3Add(cam_pos, car_pos, &delta);
        }
    }
    bnds.mat = &mat;
    BrMatrix34Identity(&mat);
    BrVector3Set(&tv, hither, hither, hither);
    BrVector3Sub(&bnds.original_bounds.min, cam_pos, &tv);
    BrVector3Add(&bnds.original_bounds.max, cam_pos, &tv);
    count = FindFacesInBox(&bnds, face_list, REC2_ASIZE(face_list), NULL);
    for (i = 0; i < count; i++) {
        tFace_ref* fr = &face_list[i];
        if ((fr->material->flags & BR_MATF_TWO_SIDED) && BrVector3Dot(&fr->normal, &delta) > 0.f) {
            BrVector3Negate(&fr->normal, &fr->normal);
        }
        BrVector3Sub(&tv, cam_pos, &fr->v[0]);
        face_dots[i] = BrVector3Dot(&tv, &fr->normal);
    }

    for (i = 0; i < REC2_ASIZE(index_min_dots); i++) {
        int j;
        float min_dot = 100.f;
        int index_min_dot;

        for (j = 0; j < count; j++) {
            if (face_dots[j] > 0.f && face_dots[j] < min_dot) {
                min_dot = face_dots[j];
                index_min_dot = j;
            }
        }
        if (min_dot == 100.f) {
            break;
        }
        index_min_dots[i] = index_min_dot;
        face_dots[index_min_dot] = -100.f;
    }
    count_min_dots = i;
    if (count_min_dots >= 1) {
        br_scalar d;

        BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[0]].v[0]);
        d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &tv);
        if (d < hither) {
            d = hither - d;
            BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, d);
            BrVector3Accumulate(cam_pos, &tv);
        }
        if (count_min_dots >= 2) {
            int o_i = 1;

            d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[1]].normal);
            if (d > .95f && count_min_dots >= 3) {
                o_i = 2;
                d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[2]].normal);
            }
            if (d <= .95f) {
                br_vector3 tv;

                BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[o_i]].v[0]);
                d = BrVector3Dot(&face_list[index_min_dots[o_i]].normal, &tv);
                if (d < hither) {
                    br_scalar a;

                    a = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[o_i]].normal);
                    BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, a);
                    BrVector3Sub(&face_list[index_min_dots[o_i]].normal, &face_list[index_min_dots[o_i]].normal, &tv);
                    BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, hither - d);
                    BrVector3Accumulate(cam_pos, &tv);
                }
            }
            if (count_min_dots >= 3) {
                br_vector3 tv;
                br_scalar d;

                BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[o_i]].v[0]);
                d = BrVector3Dot(&face_list[index_min_dots[o_i]].normal, &tv);
                if (d < hither && d >= 0.f) {
                    BrVector3Cross(&tv, &face_list[index_min_dots[0]].normal, &face_list[index_min_dots[o_i]].normal);
                    if (BrVector3Dot(&tv, &face_list[index_min_dots[2]].normal) < 0.f) {
                        BrVector3Negate(&tv, &tv);
                    }
                    if (d > .5f) {
                        br_scalar a;

                        a = (hither - d) / (BrVector3Dot(&face_list[index_min_dots[2]].normal, &tv));
                        BrVector3Scale(&tv, &tv, a);
                        BrVector3Accumulate(cam_pos, &tv);
                    }
                }
            }
        }
        if (collision_info != NULL && gAction_replay_camera_mode != kActionReplayCameraMode_Manual) {
            for (;;) {
                br_vector3 p1;
                br_vector3 p2;
                br_vector3 new_cam_pos;
                br_vector3 temp_normal;
                br_vector3 pos_screen;
                int num_collisions;

                BrVector3Sub(&delta, cam_pos, car_pos);
                BrVector3Normalise(&delta, &delta);
                BrVector3Scale(&delta, &delta, hither);
                BrVector3Sub(&pos_screen, cam_pos, &delta);
                DrMatrix34ApplyLPInverse(&p1, cam_pos, &collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &pos_screen, &collision_info->actor->t.t.mat);
                num_collisions = ShapeRayCast(&p1, &p2, collision_info->shape, &new_cam_pos, &ts, &temp_normal);
                if (num_collisions == 0) {
                    break;
                }
                if (num_collisions > 0) {
                    BrMatrix34ApplyP(cam_pos, &new_cam_pos, &collision_info->actor->t.t.mat);
                }
                BrVector3Accumulate(cam_pos, &delta);
                /* original (0x413550): loops back only while the ray starts inside the shape (result < 0) */
                if (num_collisions >= 0) {
                    break;
                }
            }
        }
    }
    return loop_done;
}

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

