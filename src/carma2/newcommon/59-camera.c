#include "59-camera.h"
#include "globvrpb.h"
#include "globvrme.h"
#include "01-network.h"
#include "15-displays.h"
#include "globvrkm.h"
#include "globvars.h"

#include "37-brucetrk.h"
#include "globvars.h"

#include "37-brucetrk.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"

#include "00-car.h"
#include "rec2_macros.h"
#include "49-physics.h"
#include "34-finteray.h"

#include "c2_string.h"

extern tNon_car_spec* gActive_non_car_list[99];

int C2_HOOK_FASTCALL ShapeRayCast(const br_vector3* p1, const br_vector3* p2, const tPhysics_shape* pShape, br_vector3* pPos, float* pFactor, br_vector3* pNormal);

// GLOBAL: CARMA2_HW 0x00679278
int gAction_replay_manual_camera_target_type;

// GLOBAL: CARMA2_HW 0x006792b4
// GLOBAL: CARMA2_SW 0x00673a24
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x006792b8
int gCamera_frozen;

// GLOBAL: CARMA2_HW 0x007634ac
float gCamera_cockpit_hither;

// GLOBAL: CARMA2_HW 0x0079efa8
tActionReplayCameraMode gAction_replay_camera_mode;

// GLOBAL: CARMA2_HW 0x006792c8
tSave_camera gSave_camera[2];

// GLOBAL: CARMA2_HW 0x00655f40
br_scalar gCamera_zoom = 0.2f;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

extern int gIt_or_fox;

// FUNCTION: CARMA2_HW 0x0040f4a0
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {
    int i;
    int best;
    tCar_spec* leader;

    leader = gNet_players[0].car;
    best = *(int*)&gNet_players[0].field_0x94;
    if (gCurrent_net_game->type == eNet_game_type_foxy && gIt_or_fox >= 0 && gIt_or_fox < gNumber_of_net_players) {
        return gNet_players[gIt_or_fox].car;
    }
    if (gNumber_of_net_players > 1) {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (*(int*)&gNet_players[i].field_0x94 < best) {
                best = *(int*)&gNet_players[i].field_0x94;
                leader = gNet_players[i].car;
            }
        }
    }
    return leader;
}

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// GLOBAL: CARMA2_HW 0x0058f628
int gView_opponent_index = -1;

// FUNCTION: CARMA2_HW 0x0040e590
void C2_HOOK_FASTCALL ViewOpponent(void) {
    const char* name;

    gView_opponent_index++;
    if (gNet_mode != eNet_mode_none) {
        if (gView_opponent_index >= gNumber_of_net_players) {
            gView_opponent_index = 0;
        }
        if (gNet_mode == eNet_mode_host) {
            gCar_to_view = gNet_players[gView_opponent_index].car;
        } else if (gCar_to_view_id == 0) {
            tNet_message* message;

            gCar_to_view_id = gNet_players[gView_opponent_index].ID;
            message = NetBuildGuaranteedMessage(0x2f, 0);
            *(tPlayer_ID*)((tU8*)message + 0x1c) = gCar_to_view_id;
            NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
            return;
        }
        name = gNet_players[gView_opponent_index].player_name;
    } else {
        if (gView_opponent_index >= gNum_viewable_cars) {
            gView_opponent_index = 0;
        }
        gCar_to_view = gViewable_car_list[gView_opponent_index];
        name = gCar_to_view->driver_name;
    }
    NewTextHeadupSlot(4, 0, 2000, -3, name);
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

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040e700
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    int i;
    int j;
    int one_sided;
    tCarCockpitMaterial* cockpit_material;

    i = 0;
    if (i < pCar->count_window_materials) {
        cockpit_material = pCar->window_materials;
        for (; i < pCar->count_window_materials; i++, cockpit_material++) {
            one_sided = pInternal_cam;
            if (pInternal_cam) {
                for (j = 0; j < cockpit_material->count_maps; j++) {
                    if (cockpit_material->material->colour_map == cockpit_material->maps[j]) {
                        one_sided = 0;
                    }
                }
                if (one_sided) {
                    cockpit_material->material->flags &= ~BR_MATF_TWO_SIDED;
                }
            }
            if (!one_sided) {
                cockpit_material->material->flags |= BR_MATF_TWO_SIDED;
            }
            BrMaterialUpdate(cockpit_material->material, BR_MATU_RENDERING);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern int gAR_camera_type;
extern int gCamera_type;
extern int gPed_count;
extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern int C2_HOOK_FASTCALL OKToViewDrones(void);

extern float gCamera_angle;
extern tRace_over_reason gRace_over_reason;
extern int C2_HOOK_FASTCALL TestForNan(const float* f);
extern void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tU32 C2_HOOK_FASTCALL GetRaceTime(void);
extern tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);
extern br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter);
extern br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void);
extern br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void);
extern void C2_HOOK_FASTCALL ViewNetPlayer(void);
extern void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime);
extern void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime);
extern void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime);

// GLOBAL: CARMA2_HW 0x0058f5fc
int gNet_player_to_view_index = -1;

// GLOBAL: CARMA2_HW 0x0058f600
tU8 gCamera_type_allowed_replay[9] = { 1, 1, 1, 1, 1, 0, 0, 1, 1 };

// GLOBAL: CARMA2_HW 0x0058f610
tU8 gCamera_type_allowed_gameplay[9] = { 1, 0, 0, 0, 1, 0, 0, 1, 1 };

// GLOBAL: CARMA2_HW 0x0058f62c
tActionReplayCameraMode old_camera_mode = kActionReplayCameraMode_Invalid;

// GLOBAL: CARMA2_HW 0x0058f640
char gCamera_hud_timer_separator[4] = " ";

// GLOBAL: CARMA2_HW 0x00679298
tU32 time_of_death;

// GLOBAL: CARMA2_HW 0x006792bc
int gOpponent_viewing_mode;

// GLOBAL: CARMA2_HW 0x00679304
br_angle gPanning_camera_angle;

// GLOBAL: CARMA2_HW 0x0067931c
int old_hither;

// GLOBAL: CARMA2_HW 0x00679320
tU32 headup_timer;

// FUNCTION: CARMA2_HW 0x0040e790
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
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040e7f0
void C2_HOOK_FASTCALL ChangeCameraType(void) {
    int mode;

    // based on carpocalypse2 camera.c:48
    if (KeyIsDown(8)) {
        gAction_replay_camera_mode--;
        if (gAction_replay_camera_mode >= 9 || gAction_replay_camera_mode < 0) {
            gAction_replay_camera_mode = 8;
        }
    } else {
        gAction_replay_camera_mode++;
        if (gAction_replay_camera_mode >= 9) {
            gAction_replay_camera_mode = 0;
        }
    }
    mode = gAction_replay_camera_mode;
    if ((gAction_replay_mode && !gCamera_type_allowed_replay[mode])
            || (!gAction_replay_mode && !gCamera_type_allowed_gameplay[mode])
            || mode >= 9
            || (mode == 6 && !OKToViewDrones())
            || (mode == 5 && gPed_count == 0)) {
        ChangeCameraType();
    }
    if (gAction_replay_mode) {
        gAR_camera_type = gAction_replay_camera_mode;
    } else {
        gCamera_type = gAction_replay_camera_mode;
    }
    switch (gAction_replay_camera_mode) {
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
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ToggleCockpit

// PositionCarMountedCamera

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040ea30
// FUNCTION: CARMA2_SW 0x0040ea10
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {
    br_camera* camera;
    tCar_spec* c;
    char message[256];

    camera = gCamera->type_data;
    if (TestForNan(&camera->hither_z) != 0) {
        camera->hither_z = (float)old_hither;
    }
    old_hither = (int)camera->hither_z;
    if (gAction_replay_mode != 0) {
        if (!gCamera_type_allowed_replay[gAction_replay_camera_mode]) {
            goto not_allowed;
        }
    } else {
        if (!gCamera_type_allowed_gameplay[gAction_replay_camera_mode]) {
            goto not_allowed;
        }
    }
    if (gAction_replay_camera_mode >= 9) {
        goto not_allowed;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Drone && !OKToViewDrones()) {
        goto not_allowed;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Peds && gPed_count == 0) {
        goto not_allowed;
    }
    goto positioned;
not_allowed:
    if (!KeyIsDown(8)) {
        gAction_replay_camera_mode++;
        if (gAction_replay_camera_mode >= 9) {
            gAction_replay_camera_mode = 0;
        }
    } else {
        gAction_replay_camera_mode--;
        if (gAction_replay_camera_mode >= 9 || gAction_replay_camera_mode < 0) {
            gAction_replay_camera_mode = 8;
        }
    }
    if (gAction_replay_mode != 0) {
        if (!gCamera_type_allowed_replay[gAction_replay_camera_mode]) {
            goto change;
        }
    } else {
        if (!gCamera_type_allowed_gameplay[gAction_replay_camera_mode]) {
            goto change;
        }
    }
    if (gAction_replay_camera_mode >= 9) {
        goto change;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Drone && !OKToViewDrones()) {
        goto change;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Peds && gPed_count == 0) {
        goto change;
    }
    goto store;
change:
    ChangeCameraType();
store:
    if (gAction_replay_mode != 0) {
        gAR_camera_type = gAction_replay_camera_mode;
    } else {
        gCamera_type = gAction_replay_camera_mode;
    }
    SetCameraType(gAction_replay_camera_mode);
positioned:
    if (gNet_mode != eNet_mode_none
            && (gCurrent_net_game->type == eNet_game_type_4 || gCurrent_net_game->type == eNet_game_type_fight_to_death)) {
        if (gRace_finished == 0) {
            time_of_death = 0;
            gOpponent_viewing_mode = 0;
        } else if (time_of_death == 0) {
            time_of_death = GetRaceTime();
        } else if (time_of_death + 10000 <= GetRaceTime()) {
            if (gOpponent_viewing_mode == 0) {
                gOpponent_viewing_mode = 1;
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if (gNumber_of_net_players <= gNet_player_to_view_index) {
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if (gNet_player_to_view_index < 0 && GetRaceLeader() != gCar_to_view) {
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if ((headup_timer + 1000 < GetRaceTime() || GetRaceTime() < headup_timer)
                    && gRace_over_reason == eRace_not_over_yet) {
                strcpy(message, GetMiscString(0xd9));
                strcat(message, gCamera_hud_timer_separator);
                if (gNet_player_to_view_index < 0) {
                    strcat(message, GetMiscString(0xda));
                } else {
                    strcat(message, gNet_players[gNet_player_to_view_index].player_name);
                }
                headup_timer = GetRaceTime();
                NewTextHeadupSlot(6, 0, 500, -4, message);
            }
        }
    }
    if (gProgram_state.cockpit_on != 0) {
        return;
    }
    if (gOpponent_viewing_mode != 0 && gAction_replay_mode != 0) {
        c = &gProgram_state.current_car;
    } else {
        c = gCar_to_view;
    }
    if (c->car_master_actor->t.t.translate.t.v[0] > 500.f) {
        return;
    }
    if (old_camera_mode != gAction_replay_camera_mode) {
        camera->field_of_view = BrDegreeToAngle(gCamera_angle);
    }
    camera->hither_z = gCamera_hither;
    switch (gAction_replay_camera_mode) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        GeneralisedPositionExternalCamera(c, &c->car_master_actor->t.t.mat, &c->pos,
                c->speed, c->speedo_speed, &c->direction, &c->collision_info->omega, pTime);
        break;
    case kActionReplayCameraMode_Panning:
        CheckDisablePlingMaterials(c);
        camera = gCamera->type_data;
        if (gPanning_camera_angle == 0) {
            gPanning_camera_angle = (br_angle)(BrDegreeToAngle(gCamera_angle) * 0.7);
        }
        camera->field_of_view = gPanning_camera_angle;
        if (old_camera_mode != kActionReplayCameraMode_Panning) {
            SetUpPanningCamera(c);
        }
        PanningExternalCamera(c, pTime);
        EnablePlingMaterials();
        break;
    case kActionReplayCameraMode_ActionTracking:
        CheckDisablePlingMaterials(c);
        camera = gCamera->type_data;
        if (gPanning_camera_angle == 0) {
            gPanning_camera_angle = (br_angle)(BrDegreeToAngle(gCamera_angle) * 0.7);
        }
        camera->field_of_view = gPanning_camera_angle;
        if (IncidentCam(c, pTime) != 0) {
            break;
        }
        if (old_camera_mode != gAction_replay_camera_mode) {
            SetUpPanningCamera(c);
        }
        PanningExternalCamera(c, pTime);
        EnablePlingMaterials();
        break;
    case kActionReplayCameraMode_Manual:
        FrozenCamera(c, pTime);
        break;
    case kActionReplayCameraMode_Peds:
        {
            tPed_character_instance* ped;
            br_matrix34* mat;

            ped = GetTestPed();
            if (ped == NULL) {
                ChangeCameraType();
            } else {
                mat = GetCharacterMatrixPtr(ped);
                GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3],
                        0.f, 0.f, &ped->field_0xc0, &gZero_v__car, pTime);
            }
        }
        break;
    case kActionReplayCameraMode_Drone:
        if (OKToViewDrones()) {
            br_matrix34* mat;
            br_vector3* direction;

            mat = GetCurrentViewDroneMat();
            direction = GetCurrentViewDroneDirection();
            GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3],
                    0.f, 0.f, direction, &gZero_v__car, pTime);
        }
        break;
    case kActionReplayCameraMode_Internal:
        camera->hither_z = gCamera_cockpit_hither;
        PositionCarMountedCamera(c, pTime);
        break;
    default:
        break;
    }
    old_camera_mode = gAction_replay_camera_mode;
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
void C2_HOOK_FASTCALL SaveCameraPosition(int i) {

    if (gSave_camera[i].saved != 1) {
        gSave_camera[i].zoom = gCamera_zoom;
        gSave_camera[i].yaw = gCamera_yaw;
        gSave_camera[i].saved = 1;
    }
}
// FUNCTION: CARMA2_HW 0x00410c20
void C2_HOOK_FASTCALL RestoreCameraPosition(int i) {

    if (gSave_camera[i].saved != 0) {
        gCamera_zoom = gSave_camera[i].zoom;
        gCamera_yaw = gSave_camera[i].yaw;
        gSave_camera[i].saved = 0;
    }
}
// DrVector3RotateY

// GLOBAL: CARMA2_HW 0x006792e0
br_angle gOld_yaw__car;

// GLOBAL: CARMA2_HW 0x006792e4
br_angle gOld_zoom;

// GLOBAL: CARMA2_HW 0x006792e8
br_vector3 gCamera_pos_before_collide;

// GLOBAL: CARMA2_HW 0x006792f4
undefined2 gUNK_006792f4;

// GLOBAL: CARMA2_HW 0x006792f8
int gUNK_006792f8;

extern tCamera_key_flags gCamera_key_flags;
extern int gCamera_mode;
extern int gCamera_has_collided;
extern int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tPhysics_object* collision_info);
extern int C2_HOOK_FASTCALL ARReplayForwards(void);
extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);
static void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: DoCameraControls
void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags* pCamera_controls, tU32 pTime_difference) { NOT_IMPLEMENTED(); }
// STUB: MoveWithWheels
void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing) { NOT_IMPLEMENTED(); }
// GLOBAL: CARMA2_HW 0x0058f63c
static int elapsed_time = -1;

// GLOBAL: CARMA2_HW 0x00679338
static br_vector3 old_vn;

// FUNCTION: CARMA2_HW 0x00411980
void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference, tCar_spec* pCar) {
    br_angle yaw;
    br_scalar cos_dtheta;
    br_scalar sign;
    int manual_swing;
    br_scalar v16;
    br_scalar v17;
    br_scalar v18;
    br_scalar abs_v18;
    br_angle v8;
    br_angle v9;
    br_angle omega;
    br_scalar ts;

    manual_swing = gOld_yaw__car != gCamera_yaw;
    if (elapsed_time > 500) {
        elapsed_time = -1;
    }
    if (elapsed_time >= 0) {
        elapsed_time += pTime_difference;
    }
    sign = -BrVector3Dot((br_vector3*)pM1->m[2], pVn);
    if ((sign < 0.f) == gCamera_sign) {
        elapsed_time = -1;
    } else if (BrVector3Dot(pVn, &old_vn) <= 0.f || elapsed_time >= 0) {
        if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || gCamera_sign) {
            if (elapsed_time < 0) {
                elapsed_time = 0;
            }
            if (elapsed_time < 500 && sign <= 0.f) {
                BrVector3Negate(pVn, pVn);
            } else {
                elapsed_time = 500;
                if ((sign > 0.f ? 0.0001 : 0.0006) < fabs(pSpeedo_speed) && !gCar_flying) {
                    gCamera_sign = !gCamera_sign;
                    gUNK_006792f4 = BrDegreeToAngle(200.0 * (gUNK_006792f8 ? .06 : .04));
                    if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                        yaw = gCamera_yaw - BR_ANGLE_DEG(180);
                    } else {
                        yaw = gCamera_yaw;
                    }
                    if (gCamera_yaw + BR_ANGLE_DEG(90) > BR_ANGLE_DEG(180)) {
                        gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                    } else if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                        gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                    }
                    goto camera_positioned;
                }
                BrVector3Negate(pVn, pVn);
            }
        } else {
            BrVector3Negate(pVn, pVn);
        }
    } else {
        gCamera_sign = !gCamera_sign;
        if (gCamera_yaw > BR_ANGLE_DEG(180)) {
            yaw = gCamera_yaw - BR_ANGLE_DEG(180);
        } else {
            yaw = gCamera_yaw;
        }
        if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
            gCamera_yaw = -gCamera_yaw;
        }
    }
camera_positioned:
    BrVector3Copy(&old_vn, pVn);
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    if (!gCar_flying) {
        DrVector3RotateY(pVn, yaw);
    }
    v16 = pVn->v[0] * gView_direction.v[0] + pVn->v[2] * gView_direction.v[2];
    v17 = pVn->v[0] * gView_direction.v[2] - pVn->v[2] * gView_direction.v[0];
    if (v16 < 0.5 && gCamera_yaw == 0) {
        gUNK_006792f8 = 1;
    }
    v18 = pOmega->v[0] * pM1->m[0][1] + pOmega->v[1] * pM1->m[1][1] + pOmega->v[2] * pM1->m[2][1];
    abs_v18 = fabs(v18);
    v8 = BrRadianToAngle((float)pTime_difference * (abs_v18 - -0.08726646f) / 1000.f);
    v9 = BrRadianToAngle(sqrt(fabs(v17)));
    if (!(gUNK_006792f4 == 0 && v16 > 0.f && v9 < v8) && !gCar_flying && !manual_swing) {
        if (gUNK_006792f4 == 0) {
            gUNK_006792f4 = BrDegreeToAngle((gUNK_006792f8 ? .06 : .04) * 50.0);
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
                pVn->v[0] = sinf(ts) * gView_direction.v[2] + cosf(ts) * gView_direction.v[0];
                pVn->v[2] = cosf(ts) * gView_direction.v[2] - sinf(ts) * gView_direction.v[0];
            } else {
                pVn->v[0] = cosf(ts) * gView_direction.v[0] - sinf(ts) * gView_direction.v[2];
                pVn->v[2] = sinf(ts) * gView_direction.v[0] + cosf(ts) * gView_direction.v[2];
            }
            gUNK_006792f4 += BrDegreeToAngle(pTime_difference * (gUNK_006792f8 ? .06 : .04));
            if (gUNK_006792f4 > BrDegreeToAngle(gUNK_006792f8 ? 40. : 10.)) {
                gUNK_006792f4 = BrDegreeToAngle(gUNK_006792f8 ? 40. : 10.);
            }
            return;
        }
    }
    gUNK_006792f4 = 0;
    gCamera_mode = 0;
    gUNK_006792f8 = 0;
}
// FUNCTION: CARMA2_HW 0x00411fc0
void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor) {
    br_vector3 vn;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar dist;
    br_scalar frac;
    br_angle theta;
    br_vector3* pos;
    br_camera* camera_ptr;
    int swoop;

    camera_ptr = gCamera->type_data;
    theta = (br_angle)(pFov_factor * 0.2f * camera_ptr->field_of_view);
    swoop = gCountdown && gCamera_height > pPos->v[1] - -0.01f;
    if (swoop) {
        BrVector3Sub(&tv, &gAverage_grid_position, pPos);
        frac = (gCamera_height - pPos->v[1]) * 0.1f;
        BrVector3Scale(&tv, &tv, frac);
        BrVector3Add(&tv, pPos, &tv);
        pos = &tv;
        theta = (br_angle)((1.0f - frac) * theta);
    } else {
        pos = pPos;
    }
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[3]);
    vn.v[1] = 0.f;
    BrVector3Normalise(&vn, &vn);
    pMat->m[0][0] = -vn.v[2];
    pMat->m[0][1] = 0.0f;
    pMat->m[1][0] = 0.0f;
    pMat->m[0][2] = vn.v[0];
    pMat->m[2][0] = -vn.v[0];
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][2] = -vn.v[2];
    pMat->m[2][1] = 0.0f;
    BrVector3Sub(&tv2, pos, (br_vector3*)pMat->m[3]);
    dist = BrVector3Dot(&tv2, &vn);
    BrMatrix34PreRotateX(pMat, theta - BR_ATAN2(pMat->m[3][1] - pos->v[1], dist));
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on carpocalypse2 src/carma2/common/car.c (GPL-3.0)
// FUNCTION: CARMA2_HW 0x00410c60
void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference) {
    br_vector3 old_camera_pos;
    br_matrix34* m1;
    int swoop;

    m1 = &gCamera->t.t.mat;
    swoop = gCountdown && gCamera_height > pPos->v[1] + 0.001f;
    BrVector3Copy(&old_camera_pos, &gCamera->t.t.translate.t);
    if (!gProgram_state.cockpit_on) {
        int manual_swing;
        float d;
        float l;
        float height_inc;
        float time;

        DoCameraControls(&gCamera_key_flags, pTime_difference);
        manual_swing = gCamera_key_flags.field_0x0_bit4 || gCamera_key_flags.field_0x0_bit3 || swoop;
        if (swoop) {
            gCamera_yaw = 0;
        }
        if (fabs(pSpeedo_speed) > 0.0006f && gCamera_mode > 0) {
            gCamera_mode = -1;
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Standard) {
                gCamera_sign = 0;
            } else if (BrVector3Dot((br_vector3*)pMat->m[2], pDirection) <= 0.f) {
                gCamera_sign = 0;
            } else {
                gCamera_sign = 1;
            }
        }
        if (pCar != NULL && pCar->frame_collision_flag && gCamera_mode != -2
                && (!gAction_replay_mode || ARReplayForwards())) {
            gCamera_mode = 1;
        }
        if (gCar_flying || gCamera_reset
                || gCamera_mode == -2 || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
            gCamera_mode = 0;
        }
        d = sqrt(gCamera_zoom) + 4.f / WORLD_SCALE;
        if (!gCamera_mode || gCamera_mode == -1) {
            br_vector3 vn;
            br_vector3 a;

            if (gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                BrVector3Negate(&vn, (br_vector3 *) pMat->m[2]);
            } else {
                BrVector3Copy(&vn, pDirection);
            }
            MoveWithWheels(pCar, &vn, manual_swing);
            vn.v[1] = 0.0f;
            BrVector3Normalise(&vn, &vn);
            vn.v[1] = 0.0f;
            if (gCar_flying || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                gCamera_sign = 0;
            }
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, pTime_difference, pCar);
            } else {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, 0, pCar);
            }
            BrVector3Scale(&a, &vn, d);
            BrVector3Sub(&gCamera->t.t.translate.t, pPos, &a);
            BrVector3Copy(&gView_direction, &vn);
        } else {
            gUNK_006792f4 = 0;
            gUNK_006792f8 = 0;
        }
        if (gCamera_mode == 1) {
            br_vector3 vn;
            br_vector3 a;
            br_scalar dist;
            br_scalar l;

            BrVector3Sub(&a, pPos, &old_camera_pos);
            BrVector3Copy(&old_camera_pos, &gCamera_pos_before_collide);
            a.v[1] = 0.0f;
            if (manual_swing) {
                DrVector3RotateY(&a, (gCamera_sign == 0 ? 1 : -1) * (gCamera_yaw - gOld_yaw__car));
                gCamera_yaw = gOld_yaw__car;
            }
            BrVector3Normalise(&vn, &a);
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || manual_swing
                    || BrVector3Dot((br_vector3*)pMat->m[2], &vn) < BrVector3Dot((br_vector3*)pMat->m[2], &gView_direction)) {
                BrVector3Copy(&gView_direction, &vn);
            }
            BrVector3Scale(&vn, &vn, -d);
            BrVector3Accumulate(&a, &vn);
            dist = BrVector3Length(&a);
            l = (float)pTime_difference / 1000.0f * (dist + 1.0f) / dist;
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard
                    && l < 1.0f && BrVector3Dot(&a, &vn) > 0.0f) {
                BrVector3Scale(&a, &a, l - 1.f);
                BrVector3Accumulate(&vn, &a);
            }
            BrVector3Add(&gCamera->t.t.translate.t, pPos, &vn);
        }

        height_inc = gCamera_zoom * gCamera_zoom + 0.3f;
        time = pTime_difference / 1000.f;
        if (!gCamera_frozen && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
            if (pTime_difference >= 5000) {
                gCamera_height = pPos->v[1];
            } else if (swoop) {
                if (time > 0.2f) {
                    time = 0.2f;
                }
                gCamera_height -= 5.0f * time;
                if (gCamera_height < pPos->v[1]) {
                    gCamera_height = pPos->v[1];
                }
            } else {
                gCamera_height += 5.0f * time * pPos->v[1];
                gCamera_height /= 5.0f * time + 1.0f;
            }
        }
        l = pDirection->v[1] * d;
        if (l > 0) {
            br_scalar new_height = pPos->v[1] - l - height_inc / 2.0f;
            if (new_height > gCamera_height) {
                gCamera_height = new_height;
            }
        }

        gCamera->t.t.translate.t.v[1] = height_inc + gCamera_height;
        BrVector3Copy(&gCamera_pos_before_collide, &gCamera->t.t.translate.t);
        CollideCameraWithOtherCars(pPos, &gCamera->t.t.translate.t);
        CollideCamera2(pPos, &gCamera->t.t.translate.t, &old_camera_pos,
            manual_swing || gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2, pCar != NULL ? pCar->collision_info : NULL);
        if (gCamera_has_collided && swoop) {
            gCamera_height = pPos->v[1];
        }
        PointCameraAtCar(pPos, m1, 1.f);
    }
    gOld_yaw__car = gCamera_yaw;
    gOld_zoom = (int)gCamera_zoom;
}

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

// PointCameraAtCar

// PointCamera

// based on carpocalypse2 src/carma2/common/camera.c (reference)
extern int gCamera_has_collided;
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
            }
        }
    }
    return loop_done;
}


// FUNCTION: CARMA2_HW 0x00413570
int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos) {

    return 0;
}

extern int gCamera_mode;
extern float gUNK_006792fc;

static void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t) {
    br_scalar c;
    br_scalar s;
    br_scalar ts;

    c = cosf(BrAngleToRadian(t));
    s = sinf(BrAngleToRadian(t));
    ts = v->v[0] * c + v->v[2] * s;
    v->v[2] = v->v[2] * c - v->v[0] * s;
    v->v[0] = ts;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00413580
// FUNCTION: CARMA2_SW 0x004135b0
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    br_scalar ts;
    tCar_spec* c;
    br_angle yaw;

    c = gCar_to_view;
    if (!gProgram_state.racing) {
        c = &gProgram_state.current_car;
    }
    gCamera_height = c->pos.v[1];
    BrVector3Set(&gView_direction, c->direction.v[0], 0.0f, c->direction.v[2]);
    gUNK_006792fc = 1.0f;
    BrVector3Normalise(&gView_direction, &gView_direction);
    ts = -BrVector3Dot(&gView_direction, (br_vector3*)c->car_master_actor->t.t.mat.m[2]);
    gCamera_sign = ts < 0;
    gCamera_mode = 0;
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    DrVector3RotateY(&gView_direction, yaw);
    gMin_camera_car_distance = 0.6f;
    gCamera_frozen = 0;
    gCamera_mode = -2;
    if (gCountdown && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.grid_start) && gCountdown > 4) {
        gCamera_height -= -10.0f;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00413770
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


// GLOBAL: CARMA2_HW 0x00679294
int gCamera_mode;

// GLOBAL: CARMA2_HW 0x006792fc
float gUNK_006792fc;

// GLOBAL: CARMA2_HW 0x006793e0
int gCamera_has_collided;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: ShapeRayCast
int C2_HOOK_FASTCALL ShapeRayCast(const br_vector3* p1, const br_vector3* p2, const tPhysics_shape* pShape, br_vector3* pPos, float* pFactor, br_vector3* pNormal) { NOT_IMPLEMENTED(); return 0; }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0040ef90
void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime) { NOT_IMPLEMENTED(); }
// STUB: CARMA2_HW 0x0040f510
void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar) { NOT_IMPLEMENTED(); }
// STUB: CARMA2_HW 0x0040f590
void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime) { NOT_IMPLEMENTED(); }
// STUB: CARMA2_HW 0x004104b0
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime) { NOT_IMPLEMENTED(); }
// STUB: CARMA2_HW 0x004105f0
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* pCar) { NOT_IMPLEMENTED(); }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
