#include "59-camera.h"
#include "41-utility.h"
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
#include "41-utility.h"

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

// 0x0058f640 is listed as a string in reccmp/carma2_hw-ascii.csv, so a
// GLOBAL annotation here would give the orig entity a name it can never match
// (reccmp keeps the CSV string type); leave it unnamed and let reccmp's string
// analysis name it from its " " contents.
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

// GLOBAL: CARMA2_HW 0x00679324
tActionReplayCameraMode gCameraTypeBeforeCockpit;

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

// FUNCTION: CARMA2_HW 0x0040e900
void C2_HOOK_FASTCALL ToggleCockpit(void) {
    tActionReplayCameraMode old_camera_mode;

    if ((gCar_to_view != &gProgram_state.current_car && !gProgram_state.cockpit_on) || gMap_view == 2) {
        return;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        old_camera_mode = gCameraTypeBeforeCockpit;
        if ((gAction_replay_mode && !gCamera_type_allowed_replay[gCameraTypeBeforeCockpit])
                || (!gAction_replay_mode && !gCamera_type_allowed_gameplay[gCameraTypeBeforeCockpit])
                || gCameraTypeBeforeCockpit >= (int)REC2_ASIZE(gCamera_type_allowed_replay)
                || (gCameraTypeBeforeCockpit == kActionReplayCameraMode_Drone && !OKToViewDrones())
                || (old_camera_mode == kActionReplayCameraMode_Peds && !gPed_count)) {
            SetCameraType(kActionReplayCameraMode_Standard);
        } else {
            SetCameraType(gCameraTypeBeforeCockpit);
        }
    } else {
        gCameraTypeBeforeCockpit = gAction_replay_camera_mode;
        SetCameraType(kActionReplayCameraMode_Internal);
    }
}

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

extern int C2_HOOK_FASTCALL GetPipeCarStateAtTime(tS32 pOffset, tS32* pMode, br_scalar* pTowards, undefined4* pData, tS32* pTime);
extern void C2_HOOK_FASTCALL PangToCamera(br_vector3* pPos, br_matrix34* pMat);
extern void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned);
extern int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tPhysics_object* collision_info);
extern void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
extern int C2_HOOK_FASTCALL ARReplayPlaying(void);
extern int gCamera_incident_flag;
extern int gCamera_incident_mode;
extern int gCamera_incident_direction;
extern tU32 gIncident_car_data[4];
extern br_scalar gIncident_towards;
extern tS32 gIncident_time;
extern tS32 gIncident_camera_stack;

extern tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);
extern int C2_HOOK_FASTCALL ARReplayForwards(void);
extern void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter);
extern tU32 gOldest_time;
extern tU32 gTrigger_time;
static int C2_HOOK_FASTCALL CheckPipeCarState(tPipe_chunk* pChunk, int pChunk_count, tU32 pTime);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040f790
// FUNCTION: CARMA2_SW 0x0040f770
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime) {
    br_matrix34* m2;
    br_vector3 old_cam_pos;
    br_vector3 cam1;
    br_vector3 camP;
    br_vector3 tv;
    br_vector3 perp;
    br_vector3 vertical;
    br_vector3 scan_pos;
    br_vector3 left;
    br_vector3 right;
    br_vector3 scaled;
    br_vector3 dir;
    br_vector3 nor;
    br_material* material;
    br_scalar d;
    br_scalar ts;
    tU32 time_returned;
    undefined4 scan_data[4];
    tS32 mode2;
    br_scalar towards2;
    tS32 time2;
    int scan_index;
    tCar_spec* car2;
    br_actor* ped_actor;
    br_actor* murderer;
    tU8 col_x;
    tU8 col_z;
    int removed;

    m2 = &gCamera->t.t.mat;
    gPed_actor = NULL;
    scan_index = 0;
    if (gCamera_incident_mode == 3) {
        if (GetPipeCarStateAtTime(-1, &gCamera_incident_mode, &gIncident_towards, gIncident_car_data, &gIncident_time) == 0) {
            gCamera_incident_mode = 3;
        } else if (gIncident_time < 0 && ARGetReplayDirection() >= 0) {
            gCamera_incident_mode = 3;
        } else if (abs(gIncident_time) > 2500) {
            gCamera_incident_mode = 3;
        } else {
            if (GetPipeCarStateAtTime(abs(gIncident_time), &mode2, &towards2, scan_data, &time2) != 0) {
                do {
                    if (++scan_index > 10) {
                        break;
                    }
                    if (abs(time2) > 3500) {
                        break;
                    }
                    if (mode2 < gCamera_incident_mode || (mode2 == gCamera_incident_mode && gIncident_towards <= towards2)) {
                        gCamera_incident_mode = mode2;
                        gIncident_car_data[0] = scan_data[0];
                        gIncident_car_data[1] = scan_data[1];
                        gIncident_car_data[2] = scan_data[2];
                        gIncident_car_data[3] = scan_data[3];
                        gIncident_towards = towards2;
                        gIncident_time = time2;
                    }
                } while (GetPipeCarStateAtTime(abs(time2), &mode2, &towards2, scan_data, &time2) != 0);
            }
            if (abs(gIncident_time) > 2500) {
                gCamera_incident_mode = 3;
                goto lab_fc76;
            }
            if (gCamera_incident_mode != 0 && gCamera_incident_mode == 2) {
                if (gIncident_towards < 0.1f) {
                    gCamera_incident_mode = 3;
                    goto lab_fc76;
                }
                ScanCarsPositions(pCar, &pCar->pos, 100000.f, 0xffffffff, abs(gIncident_time), &scan_pos, &time_returned);
                BrVector3Sub(&tv, &scan_pos, &pCar->pos);
                if (time_returned != 0 && BrVector3LengthSquared(&tv) < 102.91955471539592) {
                    BrVector3Sub(&tv, &scan_pos, (br_vector3*)gIncident_car_data);
                    tv.v[1] = scan_pos.v[1] - -2.0f - ((br_vector3*)gIncident_car_data)->v[1];
                    BrVector3Normalise(&tv, &tv);
                    BrVector3Scale(&tv, &tv, 2.f);
                    BrVector3Set(&vertical, 0.f, 1.f, 0.f);
                    BrVector3Cross(&perp, &vertical, &tv);
                    BrVector3Add(&left, &scan_pos, &tv);
                    BrVector3Add(&left, &left, &perp);
                    left.v[1] = left.v[1] - -2.0f;
                    BrVector3Add(&right, &scan_pos, &tv);
                    BrVector3Sub(&right, &right, &perp);
                    right.v[1] = right.v[1] - -2.0f;
                    CollideCamera2(&scan_pos, &left, NULL, 1, NULL);
                    CollideCamera2(&scan_pos, &right, NULL, 1, NULL);
                    BrVector3Sub(&dir, &left, &scan_pos);
                    BrVector3Sub(&tv, &right, &scan_pos);
                    if (BrVector3LengthSquared(&dir) > BrVector3LengthSquared(&tv)) {
                        BrVector3Copy(&gCamera->t.t.translate.t, &left);
                    } else {
                        BrVector3Copy(&gCamera->t.t.translate.t, &right);
                    }
                } else {
                    gCamera_incident_mode = 3;
                }
            }
            gIncident_time += GetTotalTime();
        }
    } else {
        gCamera_incident_flag = 1;
    }
lab_fc76:
    if (gCamera_incident_mode == 3) {
        return 0;
    }
    if (gCamera_incident_mode != 0) {
        if (gCamera_incident_mode == 1) {
            car2 = (tCar_spec*)gIncident_car_data[0];
            BrVector3Sub(&tv, &car2->pos, &pCar->pos);
            tv.v[1] = 0.f;
            BrVector3Normalise(&tv, &tv);
            BrVector3Scale(&tv, &tv, 2.f);
            BrVector3Add(&gCamera->t.t.translate.t, &car2->pos, &tv);
            gCamera->t.t.translate.t.v[1] = gCamera->t.t.translate.t.v[1] - -1.0f;
            CollideCamera2(&car2->pos, &gCamera->t.t.translate.t, NULL, 1, NULL);
            PangToCamera(&car2->pos, m2);
            BrVector3Sub(&tv, &gCamera->t.t.translate.t, &pCar->pos);
            ts = BrVector3LengthSquared(&tv);
            if ((float)fabs((float)((tS32)GetTotalTime() - gIncident_time)) > 2500.f) {
                gCamera_incident_mode = 3;
            }
            if (ARReplayPlaying() ? (tU32)gIncident_time < GetTotalTime() : GetTotalTime() < (tU32)gIncident_time) {
                if (ts >= 25.f) {
                    gCamera_incident_mode = 3;
                } else {
                    BrVector3Sub(&dir, &gCamera->t.t.translate.t, &pCar->pos);
                    FindFace(&pCar->pos, &dir, &nor, &d, &material);
                    if (d < 1.f) {
                        gCamera_incident_mode = 3;
                    }
                }
            }
        } else if (gCamera_incident_mode == 2) {
            PangToCamera(&pCar->pos, m2);
            BrVector3Sub(&tv, &gCamera->t.t.translate.t, &pCar->pos);
            ts = BrVector3LengthSquared(&tv);
            if ((float)fabs((float)((tS32)GetTotalTime() - gIncident_time)) > 2500.f) {
                gCamera_incident_mode = 3;
            }
            if (ARReplayPlaying() ? (tU32)gIncident_time < GetTotalTime() : GetTotalTime() < (tU32)gIncident_time) {
                if (ts >= 25.f) {
                    gCamera_incident_mode = 3;
                } else {
                    BrVector3Sub(&dir, &gCamera->t.t.translate.t, &pCar->pos);
                    FindFace(&pCar->pos, &dir, &nor, &d, &material);
                    if (d < 1.f) {
                        gCamera_incident_mode = 3;
                    }
                }
            }
        } else {
            gCamera_incident_mode = 3;
        }
    } else {
        BrVector3Copy(&old_cam_pos, &gCamera->t.t.translate.t);
        ped_actor = (br_actor*)gIncident_car_data[0];
        murderer = (br_actor*)gIncident_car_data[1];
        gPed_actor = ped_actor;
        XZToColumnXZ(&col_x, &col_z, murderer->t.t.translate.t.v[0], murderer->t.t.translate.t.v[2], &gProgram_state.track_spec);
        if (gProgram_state.track_spec.columns[col_z][col_x].actor_0x0 == murderer->parent) {
            BrActorRemove(murderer);
            removed = 1;
        } else {
            removed = 0;
        }
        BrVector3Copy(&cam1, &ped_actor->t.t.translate.t);
        gIncident_car_data[1] = (tU32)pCar->car_master_actor;
        if (pCar->car_master_actor != NULL) {
            BrVector3Copy(&camP, &pCar->pos);
        } else if (pCar->car_master_actor->model != NULL) {
            BrVector3Sub(&scaled, &pCar->car_master_actor->model->bounds.max, &pCar->car_master_actor->model->bounds.min);
            BrVector3Scale(&scaled, &scaled, 0.5f);
            BrMatrix34ApplyP(&camP, &scaled, &pCar->car_master_actor->t.t.mat);
        } else {
            BrVector3Copy(&camP, &pCar->car_master_actor->t.t.translate.t);
        }
        if (GetTotalTime() < (tU32)gIncident_time || ARGetReplayDirection() != 1) {
            tv.v[0] = cam1.v[0] - camP.v[0];
            tv.v[1] = 0.f;
            tv.v[2] = cam1.v[2] - camP.v[2];
            BrVector3Normalise(&tv, &tv);
            BrVector3Set(&vertical, 0.f, 0.4f, 0.f);
            BrVector3Cross(&perp, &tv, &vertical);
            if (gCamera_incident_direction != 0) {
                BrVector3Negate(&perp, &perp);
            }
            if (ARReplayPlaying()) {
                BrVector3Accumulate(&perp, &tv);
            }
            BrVector3Add(&gCamera->t.t.translate.t, &cam1, &perp);
            gCamera->t.t.translate.t.v[1] = gCamera->t.t.translate.t.v[1] - -0.25f;
            CollideCamera2(&cam1, &gCamera->t.t.translate.t, NULL, 1, NULL);
        }
        PangToCamera(&cam1, m2);
        BrVector3Sub(&tv, &gCamera->t.t.translate.t, &((br_actor*)gIncident_car_data[1])->t.t.translate.t);
        ts = BrVector3LengthSquared(&tv);
        if (abs((tS32)GetTotalTime() - gIncident_time) > 2500) {
            gCamera_incident_mode = 3;
        }
        if (ARReplayPlaying() ? (tU32)(gIncident_time + 300) < GetTotalTime() : GetTotalTime() < (tU32)(gIncident_time - 600)) {
            if (ts >= 25.f) {
                gCamera_incident_mode = 3;
            } else {
                BrVector3Sub(&dir, &gCamera->t.t.translate.t, &((br_actor*)gIncident_car_data[1])->t.t.translate.t);
                FindFace(&((br_actor*)gIncident_car_data[1])->t.t.translate.t, &dir, &nor, &d, &material);
                if (d < 1.f) {
                    gCamera_incident_mode = 3;
                }
            }
        }
        if (removed) {
            XZToColumnXZ(&col_x, &col_z, murderer->t.t.translate.t.v[0], murderer->t.t.translate.t.v[2], &gProgram_state.track_spec);
            BrActorAdd(gProgram_state.track_spec.columns[col_z][col_x].actor_0x0, (br_actor*)gIncident_car_data[1]);
        }
        tv.v[0] = cam1.v[0] - gCamera->t.t.translate.t.v[0];
        tv.v[2] = cam1.v[2] - gCamera->t.t.translate.t.v[2];
        if (tv.v[0] * tv.v[0] + tv.v[2] * tv.v[2] < 0.0225) {
            BrVector3Copy(&gCamera->t.t.translate.t, &old_cam_pos);
            gPed_actor = NULL;
            return 0;
        }
    }
    if (gCamera_incident_mode != 3) {
        return 1;
    }
    if (gIncident_camera_stack > 1) {
        SetUpPanningCamera(pCar);
        return 0;
    }
    gIncident_camera_stack++;
    if (IncidentCam(pCar, pTime) != 0) {
        gIncident_camera_stack--;
        return 1;
    }
    gIncident_camera_stack--;
    return 0;
}

// MoveCamToIncident

// PanningExternalCamera

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
// DoCameraControls and MoveWithWheels have NO standalone address in CARMA2_HW.EXE: MSVC inlined
// both into GeneralisedPositionExternalCamera (0x00410c60), the only place their code exists
// (gCamera swirl test at 0x410ceb; last_swirl_mode 0x0067930c only at 0x410d40..; move_with_wheels
// 0x0058f638 only at 0x4111c1..). Scoring work item is 0x00410c60 with these bodies in this TU.
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


extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);

// inlined into GeneralisedPositionExternalCamera 0x00410c60; no address of its own
static void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags* pCamera_controls, tU32 pTime_difference) {
    int flag;
    int swirl_mode;
    int up_and_down_mode;
    int going_down;
    // GLOBAL: CARMA2_HW 0x0067930c
    static int last_swirl_mode;

    flag = 0;
    swirl_mode = !GetRuntimeVariable(99) && gRace_finished && !gAction_replay_mode && (gCar_to_view == &gProgram_state.current_car || gCar_to_view->knackered);
    up_and_down_mode = swirl_mode && !gCamera_has_collided;
    going_down = gCamera_zoom > 1.0;
    if (last_swirl_mode != swirl_mode) {
        if (swirl_mode) {
            SaveCameraPosition(0);
        } else {
            RestoreCameraPosition(0);
        }
        last_swirl_mode = swirl_mode;
    }
    if (gMap_view != 2 && !gProgram_state.cockpit_on && (!gAction_replay_mode || gAction_replay_camera_mode <= 0)) {
        if (pCamera_controls->field_0x0_bit2 || (up_and_down_mode && !going_down)) {
            gCamera_zoom += (float)pTime_difference * 0.0005f / (float)(2 * swirl_mode + 1);
            if (gCamera_zoom > 2.f) {
                gCamera_zoom = 2.f;
            }
            if (up_and_down_mode && gCamera_zoom > 1.f) {
                gCamera_zoom = 1.f;
            }
        }
        if (pCamera_controls->field_0x0_bit1 || (up_and_down_mode && going_down)) {
            gCamera_zoom -= (float)pTime_difference * 0.0005f / (float)(2 * swirl_mode + 1);
            if (gCamera_zoom < (gAction_replay_camera_mode == kActionReplayCameraMode_Peds ? 0.001 : 0.1)) {
                gCamera_zoom = (gAction_replay_camera_mode == kActionReplayCameraMode_Peds ? 0.001 : 0.1);
                if (up_and_down_mode) {
                    if (gCamera_zoom < 1.0f) {
                        gCamera_zoom = 1.0f;
                    }
                }
            }
        }
        if (swirl_mode && gProgram_state.current_car.speedo_speed < 0.0014492753623188406) {
            pCamera_controls->field_0x0_bit4 = 0;
            pCamera_controls->field_0x0_bit3 = 1;
        }
        if (gCamera_sign ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!gCamera_reset) {
                gCamera_yaw += BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            flag = 1;
        }
        if (gCamera_sign ? pCamera_controls->field_0x0_bit4 : pCamera_controls->field_0x0_bit3) {
            if (!gCamera_reset) {
                gCamera_yaw -= BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            if (flag) {
                gCamera_reset = 1;
                gCamera_yaw = 0;
            }
        } else {
            if (!flag) {
                gCamera_reset = 0;
            }
        }
    }
}

// inlined into GeneralisedPositionExternalCamera 0x00410c60; no address of its own
static void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing) {
    br_angle yaw;
    br_angle theta;
    // GLOBAL: CARMA2_HW 0x0058f638
    static int move_with_wheels = 1;

    if (pCar != NULL && pCar->speed <= 0.0001f && !gCamera_mode) {
        if (pManual_swing) {
            if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                yaw = gCamera_yaw - BR_ANGLE_DEG(180);
            } else {
                yaw = gCamera_yaw;
            }
            if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                if (move_with_wheels) {
                    theta = BrRadianToAngle(atan2(pCar->wpos[0].v[2] * pCar->curvature, 1.0));
                    gCamera_yaw += (1 - 2 * gCamera_sign) * theta;
                    move_with_wheels = 0;
                }
            } else {
                if (!move_with_wheels) {
                    theta = BrRadianToAngle(atan2(pCar->wpos[0].v[2] * pCar->curvature, 1.0));
                    gCamera_yaw -= (1 - 2 * gCamera_sign) * theta;
                    move_with_wheels = 1;
                }
            }
        }
        if (move_with_wheels) {
            if (!gCar_flying && gAction_replay_camera_mode != kActionReplayCameraMode_Rigid) {
                theta = BrRadianToAngle(atan2(pCar->wpos[0].v[2] * pCar->curvature, 1.0));
                DrVector3RotateY(pDir, theta);
            }
        }
    }
}


// FUNCTION: CARMA2_HW 0x00410c60
void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference) {
    br_vector3 old_camera_pos;
    br_matrix34* m1;
    int swoop;

    m1 = &gCamera->t.t.mat;
    swoop = gCountdown && gCamera_height > pPos->v[1] - -0.001;
    BrVector3Copy(&old_camera_pos, &gCamera->t.t.translate.t);
    if (!gProgram_state.cockpit_on) {
        int manual_swing;
        int flag;
        float d;
        float l;
        float height_inc;
        float time;

        DoCameraControls(&gCamera_key_flags, pTime_difference);
        manual_swing = gCamera_key_flags.field_0x0_bit4 || gCamera_key_flags.field_0x0_bit3 || swoop;
        flag = gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2;
        if (swoop) {
            manual_swing = 1;
            gCamera_yaw = 0;
        }
        if (fabs(pSpeedo_speed) > 0.0006 && gCamera_mode > 0) {
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
        d = sqrt(gCamera_zoom) - (-4.f / WORLD_SCALE);
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

            a.v[0] = pPos->v[0] - gCamera_pos_before_collide.v[0];
            a.v[2] = pPos->v[2] - gCamera_pos_before_collide.v[2];
            a.v[1] = 0.0f;
            BrVector3Copy(&old_camera_pos, &gCamera_pos_before_collide);
            if (manual_swing) {
                DrVector3RotateY(&a, (gCamera_sign == 0 ? 1 : -1) * (gCamera_yaw - gOld_yaw__car));
                gCamera_yaw = gOld_yaw__car;
            }
            BrVector3Normalise(&vn, &a);
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || manual_swing
                    || BrVector3Dot((br_vector3*)pMat->m[2], &vn) < BrVector3Dot((br_vector3*)pMat->m[2], &gView_direction)) {
                BrVector3Copy(&vn, &gView_direction);
            } else {
                BrVector3Copy(&gView_direction, &vn);
            }
            BrVector3Scale(&vn, &vn, -d);
            BrVector3Accumulate(&a, &vn);
            dist = BrVector3Length(&a);
            l = (float)pTime_difference * 0.001f * (dist - -1.0f) / dist;
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard
                    && l < 1.0f && BrVector3Dot(&a, &vn) > 0.0f) {
                BrVector3Scale(&a, &a, l - 1.f);
                BrVector3Accumulate(&vn, &a);
            }
            BrVector3Add(&gCamera->t.t.translate.t, pPos, &vn);
        }

        height_inc = gCamera_zoom * gCamera_zoom - -0.3f;
        time = (float)pTime_difference * 0.001f;
        if (!gCamera_frozen && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
            if (pTime_difference >= 5000) {
                gCamera_height = pPos->v[1];
            } else if (swoop) {
                if (time > 0.2f) {
                    time = 0.2f;
                }
                gCamera_height -= time * 5.0f;
                if (gCamera_height < pPos->v[1]) {
                    gCamera_height = pPos->v[1];
                }
            } else {
                gCamera_height = (5.0f * time * pPos->v[1] + gCamera_height) / (1.0f - time * -5.0f);
            }
        }
        if (pDirection->v[1] * d > 0.0f) {
            l = pPos->v[1] - pDirection->v[1] * d - height_inc * 0.5f;
            if (gCamera_height < l) {
                gCamera_height = l;
            }
        }

        gCamera->t.t.translate.t.v[1] = height_inc + gCamera_height;
        BrVector3Copy(&gCamera_pos_before_collide, &gCamera->t.t.translate.t);
        CollideCameraWithOtherCars(pPos, &gCamera->t.t.translate.t);
        CollideCamera2(pPos, &gCamera->t.t.translate.t, &old_camera_pos,
            manual_swing || flag, pCar != NULL ? pCar->collision_info : NULL);
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

// SwingCamera

// PointCameraAtCar

// PointCamera

// (reference)
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
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// GLOBAL: CARMA2_HW 0x0067927c
br_vector3 gAction_replay_manual_camera_target_position;

// GLOBAL: CARMA2_HW 0x00679288
tCar_spec* gAction_replay_manual_camera_target_car;

// GLOBAL: CARMA2_HW 0x0067928c
tPedestrian* gAction_replay_manual_camera_target_pedestrian;

// GLOBAL: CARMA2_HW 0x00679290
br_actor* gAction_replay_manual_camera_target_character_actor;

// GLOBAL: CARMA2_HW 0x00679310
br_scalar gManual_camera_yaw;

// GLOBAL: CARMA2_HW 0x00679314
br_scalar gManual_camera_dolly;

// GLOBAL: CARMA2_HW 0x00679318
br_scalar gManual_camera_vertical;

// FUNCTION: CARMA2_HW 0x0040ef90
void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime) {
    br_vector3 old_pos;
    br_vector3 v;
    float* target;
    float move;
    float min_dist;
    float dist;
    float z;

    if (gCamera_key_flags.field_0x0_bit3 && gCamera_key_flags.field_0x0_bit4) {
        InitialiseExternalCamera();
        GeneralisedPositionExternalCamera(pCar, &pCar->car_master_actor->t.t.mat, &pCar->pos,
            pCar->speed, pCar->speedo_speed, &pCar->direction, &pCar->collision_info->omega, pTime);
        gAction_replay_manual_camera_target_type = 0;
    } else if (gCamera_key_flags.field_0x0_bit3) {
        gManual_camera_yaw = gManual_camera_yaw - 0.00025f;
    } else if (gCamera_key_flags.field_0x0_bit4) {
        gManual_camera_yaw = gManual_camera_yaw - -0.00025f;
    } else {
        gManual_camera_yaw = 0.f;
    }
    if (!gCamera_key_flags.field_0x0_bit0) {
        gManual_camera_vertical = 0.f;
        if (gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2) {
            if (gCamera_key_flags.field_0x0_bit1) {
                gManual_camera_dolly = gManual_camera_dolly - -0.00025f;
            }
            if (gCamera_key_flags.field_0x0_bit2) {
                gManual_camera_dolly = gManual_camera_dolly - 0.00025f;
            }
        } else {
            gManual_camera_dolly = 0.f;
        }
    } else {
        gManual_camera_dolly = 0.f;
        if (gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2) {
            if (gCamera_key_flags.field_0x0_bit1) {
                gManual_camera_vertical = gManual_camera_vertical - -0.00025f;
                if (gCamera->t.t.translate.t.v[1] > pCar->collision_info->transform_matrix.m[3][1] - -10.f) {
                    gManual_camera_vertical = 0.f;
                }
            }
            if (gCamera_key_flags.field_0x0_bit2) {
                gManual_camera_vertical = gManual_camera_vertical - 0.00025f;
            }
        } else {
            gManual_camera_vertical = 0.f;
        }
    }
    BrVector3Copy(&old_pos, &gCamera->t.t.translate.t);
    min_dist = 0.2f;
    gCamera->t.t.translate.t.v[1] = gManual_camera_vertical * (float)pTime + gCamera->t.t.translate.t.v[1];
    switch (gAction_replay_manual_camera_target_type) {
    case 0:
        min_dist = pCar->collision_info->bb1.max.v[2] - pCar->collision_info->bb1.min.v[2];
        target = (float*)&pCar->pos;
        break;
    case 1:
        target = (float*)&gAction_replay_manual_camera_target_position;
        break;
    case 2:
        target = (float*)&gAction_replay_manual_camera_target_position;
        BrMatrix34ApplyP((br_vector3*)target, &gAction_replay_manual_camera_target_car->collision_info->cmpos, &gAction_replay_manual_camera_target_car->car_master_actor->t.t.mat);
        break;
    case 3:
        target = (float*)&gAction_replay_manual_camera_target_position;
        if (gAction_replay_manual_camera_target_pedestrian->flags & 1) {
            tPed_personality* pers;

            pers = gAction_replay_manual_camera_target_pedestrian->character->personality;
            v.v[0] = pers->bb.min.v[0] + pers->bb.max.v[0];
            pers = gAction_replay_manual_camera_target_pedestrian->character->personality;
            v.v[1] = pers->bb.min.v[1] + pers->bb.max.v[1];
            pers = gAction_replay_manual_camera_target_pedestrian->character->personality;
            v.v[2] = pers->bb.min.v[2] + pers->bb.max.v[2];
            v.v[0] = v.v[0] * 0.5;
            v.v[1] = v.v[1] * 0.5;
            v.v[2] = v.v[2] * 0.5;
            BrMatrix34ApplyP((br_vector3*)target, &v, &gAction_replay_manual_camera_target_character_actor->t.t.mat);
        }
        break;
    }
    v.v[0] = gCamera->t.t.translate.t.v[0] - target[0];
    v.v[1] = gCamera->t.t.translate.t.v[1] - target[1];
    v.v[2] = gCamera->t.t.translate.t.v[2] - target[2];
    dist = (float)sqrt(v.v[2] * v.v[2] + v.v[0] * v.v[0]);
    if (gManual_camera_dolly != 0.f) {
        v.v[0] = gCamera->t.t.mat.m[2][0];
        v.v[1] = 0.f;
        v.v[2] = gCamera->t.t.mat.m[2][2];
        move = gManual_camera_dolly * (float)pTime;
        z = (float)sqrt(v.v[2] * v.v[2] + v.v[0] * v.v[0]);
        if (z <= 2.3841858e-07f) {
            v.v[0] = 1.f;
            v.v[1] = 0.f;
            v.v[2] = 0.f;
        } else {
            z = 1.0 / z;
            v.v[0] = z * v.v[0];
            v.v[1] = z * v.v[1];
            v.v[2] = z * v.v[2];
        }
        if (dist - move < min_dist) {
            move = dist - min_dist;
        }
        move = -move;
        v.v[0] = v.v[0] * move;
        v.v[1] = move * 0.f;
        v.v[2] = v.v[2] * move;
        gCamera->t.t.translate.t.v[0] = v.v[0] + gCamera->t.t.translate.t.v[0];
        gCamera->t.t.translate.t.v[1] = v.v[1] + gCamera->t.t.translate.t.v[1];
        gCamera->t.t.translate.t.v[2] = v.v[2] + gCamera->t.t.translate.t.v[2];
    }
    if (gManual_camera_yaw != 0.f) {
        gCamera->t.t.translate.t.v[0] = gCamera->t.t.translate.t.v[0] - target[0];
        gCamera->t.t.translate.t.v[1] = gCamera->t.t.translate.t.v[1] - target[1];
        gCamera->t.t.translate.t.v[2] = gCamera->t.t.translate.t.v[2] - target[2];
        BrMatrix34PostRotateY(&gCamera->t.t.mat, BrRadianToAngle(gManual_camera_yaw * (float)pTime / dist));
        gCamera->t.t.translate.t.v[0] = target[0] + gCamera->t.t.translate.t.v[0];
        gCamera->t.t.translate.t.v[1] = target[1] + gCamera->t.t.translate.t.v[1];
        gCamera->t.t.translate.t.v[2] = target[2] + gCamera->t.t.translate.t.v[2];
    }
    CollideCameraWithOtherCars((br_vector3*)target, &gCamera->t.t.translate.t);
    CollideCamera2(&old_pos, &gCamera->t.t.translate.t, NULL, 1, pCar->collision_info);
    if (gCamera_has_collided) {
        gManual_camera_yaw = 0.f;
        gManual_camera_dolly = 0.f;
        gManual_camera_vertical = 0.f;
        gCamera->t.t.translate.t.v[0] = old_pos.v[0];
        gCamera->t.t.translate.t.v[1] = old_pos.v[1];
        gCamera->t.t.translate.t.v[2] = old_pos.v[2];
    }
    PointCameraAtCar((br_vector3*)target, &gCamera->t.t.mat, 1.f);
}
// GLOBAL: CARMA2_HW 0x00589418
const br_scalar gFLOAT_00589418 = 0.0f;

// FUNCTION: CARMA2_HW 0x0040f510
void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar) {
    br_matrix34* mat;
    br_scalar height;
    int i;

    height = gFLOAT_00589418;
    if (pCar->collision_info->water_d != 10000.f) {
        mat = &pCar->car_master_actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            if (mat->m[i][1] > gFLOAT_00589418) {
                height += pCar->collision_info->bb2.max.v[i] * mat->m[i][1];
            } else {
                height += mat->m[i][1] * pCar->collision_info->bb2.min.v[i];
            }
        }
        {
            br_scalar d = mat->m[3][1] / WORLD_SCALE;

            if (d + height < pCar->collision_info->water_d) {
                DisablePlingMaterials();
            }
        }
    } else {
        DisablePlingMaterials();
    }
}
// PARTIAL 76.49%: x87 schedule of lerp differs
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
extern int C2_HOOK_FASTCALL ARReplayPlaying(void);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);

// GLOBAL: CARMA2_HW 0x006792c0
tU32 gSwitch_time;

// GLOBAL: CARMA2_HW 0x0058f5e8
int gCamera_incident_flag = 1;

// PARTIAL 64%: frame 0x24 vs 0x20, x87 schedule differs
// FUNCTION: CARMA2_HW 0x004104b0
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime) {
    br_vector3 dir;
    br_vector3 normal;
    br_scalar t;
    br_scalar d;
    br_material* material;
    br_vector3 delta;

    BrVector3Sub(&delta, (br_vector3*)&gCamera->t.t.mat.m[3][0], &pCar->pos);
    d = BrVector3LengthSquared(&delta);
    if (d <= 102.91955471539592) {
        if (gSwitch_time != 0) {
            int passed;

            if (ARReplayPlaying()) {
                passed = GetTotalTime() < gSwitch_time;
            } else {
                passed = gSwitch_time < GetTotalTime();
            }
            if (passed) {
                goto check_camera;
            }
        }
        gCamera_incident_flag = 1;
        goto point_camera;
    }
check_camera:
    if (gCamera_incident_flag == 0 && d <= 205.83910943079184) {
        goto point_camera;
    }
    if (d <= 25.f) {
        BrVector3Sub(&dir, (br_vector3*)&gCamera->t.t.mat.m[3][0], &pCar->pos);
        FindFace(&pCar->pos, &dir, &normal, &t, &material);
        if (t > 1.f) {
            goto point_camera;
        }
    }
    SetUpPanningCamera(pCar);
point_camera:
    PointCameraAtCar(&pCar->pos, &gCamera->t.t.mat, 1.f);
}
extern void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned);
extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
static __inline int C2_HOOK_FASTCALL CheckForWall(br_vector3* start, br_vector3* end) {
    br_vector3 dir;
    br_material* material;
    br_vector3 normal;
    br_scalar d;

    BrVector3Sub(&dir, end, start);
    FindFace(start, &dir, &normal, &d, &material);
    return d <= 1.f;
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 89.04%: logic complete (frame 0x7c, identical slot map, calls and args); residual is x87 scheduling only:
// perp.z is kept in st (fst) where the original pops and reloads (fstp/fld), and the tv/tv2 length-squared block
// schedules the last subtraction after SRandomPosNeg with different spill temps (+3 bytes).
// FUNCTION: CARMA2_HW 0x004105f0
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* pCar) {
    br_vector3 pos;
    br_vector3 perp;
    br_vector3 dir;
    br_vector3 tv;
    br_vector3 tv2;
    br_vector3 left;
    br_vector3 right;
    br_vector3 car_centre;
    br_scalar ts;
    tU32 time;
    tU32 time_step;
    tU32 t;
    tU32 t2;
    br_matrix34* m1;

    m1 = &pCar->car_master_actor->t.t.mat;
    gCamera_incident_flag = 0;
    ScanCarsPositions(pCar, &pCar->pos, 411.67822265625f, 0xffffffff, 5000, &pos, &time);
    BrVector3Sub(&dir, &pos, &pCar->pos);
    if (GetTotalTime() < time) {
        time_step = time - GetTotalTime();
    } else {
        time_step = GetTotalTime() - time;
    }
    time_step *= SRandomBetween(0.8f, 1.5f);
    if (BrVector3LengthSquared(&dir) < 0.01 || time == 0) {
        BrVector3Negate(&dir, (br_vector3*)&m1->m[2][0]);
        BrVector3Copy(&car_centre, &pCar->pos);
        time_step = 0;
    } else {
        ScanCarsPositions(pCar, &pCar->pos, 102.9195556640625f, 0xffffffff, time_step / 2, &car_centre, &t);
        if (t == 0) {
            BrVector3Copy(&car_centre, &pCar->pos);
        }
    }
    BrVector3SetFloat(&tv, 0.f, 1.f, 0.f);
    BrVector3Cross(&perp, &tv, &dir);
    ts = BrVector3Length(&perp);
    if (ts < 0.1f) {
        return;
    }
    ts = 2.f / ts;
    ts *= SRandomBetween(0.3333333432674408f, 1.f);
    BrVector3Scale(&perp, &perp, ts);
    ts = SRandomBetween(0.3333333432674408f, 1.f) * 2.f;
    BrVector3Set(&tv, 0.f, ts, 0.f);
    BrVector3Add(&tv, &car_centre, &tv);
    BrVector3Add(&left, &tv, &perp);
    BrVector3Sub(&right, &tv, &perp);
    CollideCamera2(&car_centre, &left, NULL, 1, NULL);
    CollideCamera2(&car_centre, &right, NULL, 1, NULL);
    BrVector3Sub(&tv, &left, &car_centre);
    BrVector3Sub(&tv2, &right, &car_centre);
    if (BrVector3LengthSquared(&tv) + SRandomPosNeg(0.01f) > BrVector3LengthSquared(&tv2)) {
        BrVector3Copy(&gCamera->t.t.translate.t, &left);
    } else {
        BrVector3Copy(&gCamera->t.t.translate.t, &right);
    }
    if (time != 0) {
        if (CheckForWall(&pCar->pos, &gCamera->t.t.translate.t)) {
            ScanCarsPositions(pCar, &pCar->pos, 10000.f, 0xffffffff, 1000, &tv, &t2);
            CollideCamera2(&tv, &gCamera->t.t.translate.t, NULL, 1, NULL);
        }
    }
    if (time != 0) {
        if (CheckForWall(&pos, &gCamera->t.t.translate.t)) {
            time_step = time_step / 16;
            BrVector3Copy(&tv, &car_centre);
            do {
                ScanCarsPositions(pCar, &tv, 10000.f, abs((tS32)t - (tS32)GetTotalTime()), time_step, &tv2, &t2);
                t += ((ARGetReplayDirection() != 0) ? 1 : -1) * time_step;
                BrVector3Copy(&tv, &tv2);
            } while (!CheckForWall(&tv, &gCamera->t.t.translate.t) && t < GetTotalTime() + 5000);
            gSwitch_time = t;
            return;
        }
    }
    if (time == 0) {
        time = 5000;
    }
    gSwitch_time = time;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006792a0
tU32 gIncident_car_data[4];

// GLOBAL: CARMA2_HW 0x006792b0
br_scalar gIncident_towards;

// GLOBAL: CARMA2_HW 0x0058f630
int gCamera_incident_mode = 3;

// GLOBAL: CARMA2_HW 0x0058f634
int gCamera_incident_direction = 1;

// GLOBAL: CARMA2_HW 0x0067932c
tS32 gIncident_time;

// GLOBAL: CARMA2_HW 0x00679330
tS32 gIncident_camera_stack;

// GLOBAL: CARMA2_HW 0x006940fc
tU8* gPipe_car_state_chunk;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c6c90
int C2_HOOK_FASTCALL GetPipeCarStateAtTime(tS32 pOffset, tS32* pMode, br_scalar* pTowards, undefined4* pData, tS32* pTime) {
    tU8* temp_ptr;

    temp_ptr = ARGetPipePlayPtr();
    gTrigger_time = 0;
    if (ARReplayForwards()) {
        gOldest_time = GetTotalTime() + pOffset;
    } else {
        gOldest_time = GetTotalTime() - pOffset;
    }
    ARScanBuffer(&temp_ptr, ePipe_chunk_ped_incident, GetTotalTime(), CheckPipeCarState, NULL);
    if (gTrigger_time != 0) {
        *pTime = gTrigger_time - GetTotalTime();
        *pMode = *(tS32*)gPipe_car_state_chunk;
        *pTowards = *(br_scalar*)(gPipe_car_state_chunk + 4);
        switch (*pMode) {
        case 0:
            {
                tPedestrian* ped = *(tPedestrian**)(gPipe_car_state_chunk + 8);
                br_actor* actor = GetCharacterActorPtr(ped->character);

                pData[0] = (undefined4)(uintptr_t)actor;
                if (actor == NULL) {
                    gTrigger_time = 0;
                }
                pData[1] = *(undefined4*)(gPipe_car_state_chunk + 0xc);
            }
            break;
        case 1:
            {
                tU32 car_id = *(tU16*)(gPipe_car_state_chunk + 8);
                tCar_spec* car;

                if ((car_id & 0xffffff00) == 0) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(car_id >> 8, car_id & 0xff);
                }
                pData[0] = (undefined4)(uintptr_t)car;
                pData[1] = *(undefined4*)(gPipe_car_state_chunk + 0xc);
                pData[2] = *(undefined4*)(gPipe_car_state_chunk + 0x10);
                pData[3] = *(undefined4*)(gPipe_car_state_chunk + 0x14);
            }
            break;
        case 2:
            pData[0] = *(undefined4*)(gPipe_car_state_chunk + 8);
            pData[1] = *(undefined4*)(gPipe_car_state_chunk + 0xc);
            pData[2] = *(undefined4*)(gPipe_car_state_chunk + 0x10);
            break;
        default:
            break;
        }
    }
    return gTrigger_time;
}

// FUNCTION: CARMA2_HW 0x004c6e00
static int C2_HOOK_FASTCALL CheckPipeCarState(tPipe_chunk* pChunk, int pChunk_count, tU32 pTime) {

    if (ARReplayForwards()) {
        if (pTime <= gOldest_time) {
            return 0;
        }
    } else {
        if (pTime >= gOldest_time) {
            return 0;
        }
    }
    gTrigger_time = pTime;
    gPipe_car_state_chunk = (tU8*)pChunk;
    return 1;
}
// FUNCTION: CARMA2_HW 0x004121b0
void C2_HOOK_FASTCALL PangToCamera(br_vector3* pPos, br_matrix34* pMat) {
    br_camera* camera;
    br_scalar dx;
    br_scalar dz;
    br_scalar d;
    br_scalar nx;
    br_scalar nz;
    br_angle pitch;

    camera = gCamera->type_data;
    dx = pPos->v[0] - pMat->m[3][0];
    dz = pPos->v[2] - pMat->m[3][2];
    d = sqrt(dx * dx + dz * dz);
    if (d > BR_SCALAR_EPSILON * 2) {
        d = 1.0 / d;
        nx = dx * d;
        nz = dz * d;
    } else {
        nx = 1.0f;
        nz = 0.0f;
    }
    pMat->m[0][0] = -nz;
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = nx;
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -nx;
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -nz;
    pitch = BR_ATAN2(pMat->m[3][1] - pPos->v[1],
        BR_LENGTH2(pMat->m[3][2] - pPos->v[2], pMat->m[3][0] - pPos->v[0]));
    BrMatrix34PreRotateX(pMat, (br_angle)(camera->field_of_view / 5) - pitch);
}


#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
