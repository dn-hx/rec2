#include "59-camera.h"
#include "globvrkm.h"
#include "globvars.h"

#include "37-brucetrk.h"
#include "globvars.h"

#include "37-brucetrk.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"

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

// GetRaceLeader

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// ViewOpponent

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

// GLOBAL: CARMA2_HW 0x0058f600
tU8 gCamera_type_allowed_replay[9] = { 1, 1, 1, 1, 1, 0, 0, 1, 1 };

// GLOBAL: CARMA2_HW 0x0058f610
tU8 gCamera_type_allowed_gameplay[9] = { 1, 0, 0, 0, 1, 0, 0, 1, 1 };

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

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

// PointCameraAtCar

// PointCamera

// CollideCamera2

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
