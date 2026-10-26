#include "57-drone.h"
#include "globvars.h"
#include "52-errors.h"
#include "08-loading1.h"
#include "41-utility.h"
#include "c2_string.h"
#include "platform.h"
#include "23-piping.h"

#include <math.h>

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

#include "41-utility.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x00684518
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00684514
int gFrame;

// GLOBAL: CARMA2_HW 0x00684504
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x006844f8
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x00684508
tDrone_path_node* gDrone_path_nodes;

// GLOBAL: CARMA2_HW 0x00682178
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x00681fb0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684500
int gCount_rendered_drones;

// GLOBAL: CARMA2_HW 0x0068452c
int gShow_drone_paths;

// GLOBAL: CARMA2_HW 0x005947c0
br_vector3 gDefault_drone_direction = {{ 0.f, 0.f, -1.f }};

/* separate 0.0f in .rdata used only here (not the shared 0x0058a988 literal) */
// GLOBAL: CARMA2_HW 0x00589dc4
float gDrone_render_zero = 0.0f;

// STUB: CARMA2_HW 0x0044cc70
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044d1d0
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044eb70
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044ec50
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {
    NOT_IMPLEMENTED();
}

// GLOBAL: CARMA2_HW 0x00594738
tDrone_state_function* gDrone_state_functions[6] = {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
};

// FUNCTION: CARMA2_HW 0x0044cfc0
static void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...) {

}

// FUNCTION: CARMA2_HW 0x00451620
void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}
// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0044f640
FILE* C2_HOOK_FASTCALL OpenDroneFile(const char* pDrone_name) {
    tPath_name the_path;
    FILE* f;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    PathCat(the_path, the_path, pDrone_name);
    strcat(the_path, ".TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    return f;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// LoadDroneTypeInfo

// ReallyRemoveDroneFromPHIL

// RemoveDroneFromPHIL

// InitDroneCollisionObject

/* local declarations (no new includes/externs; 60-phil.h is not included here) */
extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);

// FUNCTION: CARMA2_HW 0x00451650
int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone) {

    if (PHILAddObject(&pDrone->collision_info)) {
        return 0;
    }
    PHILSetObjectProperty(&pDrone->collision_info, 0, 1.0);
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 7, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 6, 1);
    return 1;
}

// AddDroneToPHIL

// PreprocessDronePaths

// FUNCTION: CARMA2_HW 0x00450600
int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* data) {
    tLinkyCallback_context* context = data;
    int i;

    if (pActor->type == BR_ACTOR_MODEL) {
        pActor->render_style = BR_RSTYLE_DEFAULT;
        for (i = context->model_start; i < context->model_start + context->model_end; i++) {

            if (DRStricmp(gDroneStorage.models[i]->identifier, pActor->model->identifier) == 0) {
                pActor->model = gDroneStorage.models[i];
                return 0;
            }
        }
        PDFatalError("No drone model");
    }
    return 0;
}
// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

// FUNCTION: CARMA2_HW 0x00450b30
// FUNCTION: CARMA2_SW 0x004508c0
br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName) {

    if (DRStricmp(pActor->identifier, pName) == 0) {
        return pActor;
    } else {
        return NULL;
    }
}

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

// FUNCTION: CARMA2_HW 0x0044ca90
// FUNCTION: CARMA2_SW 0x0044c840
int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 15.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 12.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 15.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cae0
// FUNCTION: CARMA2_SW 0x0044c890
int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 40.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 30.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 40.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cb30
// FUNCTION: CARMA2_SW 0x0044c8e0
int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 50.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 20.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 50.f) {
        return 1;
    } else {
        return 0;
    }
}

// SemiInitDroneSpec

// FUNCTION: CARMA2_HW 0x00451210
void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode) {

    C2_HOOK_BUG_ON(sizeof(*pDrone_spec) != 0x5d8);

    DoNotDprintf("InitDroneSpec() - INITIALISING DRONE SPEC");
    memset(pDrone_spec, 0, sizeof(*pDrone_spec));
    pDrone_spec->field_0xa_pathnode_id = pNode;
    pDrone_spec->field_0x8_pathnode_id = pNode;
    pDrone_spec->field_0xe = -1;
    pDrone_spec->field_0xc = -1;
    BrVector3Set(&pDrone_spec->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone_spec->pos, &gDrone_path_nodes[pNode].position);
    pDrone_spec->field_0xdc = 0;
    pDrone_spec->form = &gDrone_forms[gDrone_path_nodes[pNode].type];
    pDrone_spec->field_0xf4 = -1;
    pDrone_spec->current_state = 0;
    pDrone_spec->field_0x5d4 = 0;
    pDrone_spec->field_0xe4 = 1.0f;
    pDrone_spec->field_0x48 = 1.0f;
}
// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// FUNCTION: CARMA2_HW 0x004516b0
void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {

    if (gDrone_state_functions[pDrone_spec->current_state] != NULL) {
        gDrone_state_functions[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}

// FUNCTION: CARMA2_HW 0x004516d0
void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec) {
    if (pDrone_spec->actor->render_style == BR_RSTYLE_FACES) {
        DoNotDprintf("STOP RENDERING: %d", pDrone_spec->id);
        gCount_rendered_drones -= 1;
        pDrone_spec->actor->render_style = BR_RSTYLE_NONE;
        PipeSingleDroneRender(pDrone_spec, 0);
    }
}

// StopProcessingThisDrone

// FUNCTION: CARMA2_HW 0x00451810
void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone) {

    if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
        return;
    }
    if (gShow_drone_paths && pDrone->actor->render_style == BR_RSTYLE_BOUNDING_FACES) {
        return;
    }
    if (gShow_drone_paths || (pDrone->field_0x44 && gCount_rendered_drones < 10)) {

        DoNotDprintf("START RENDERING: %d", pDrone->id);
        gCount_rendered_drones += 1;
        pDrone->actor->render_style = BR_RSTYLE_FACES;
        PipeSingleDroneRender(pDrone, 1);
    } else {
        DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
        if (pDrone->current_state != 1) {
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
            }
            pDrone->time_last_munge = gTime_stamp_for_this_munging;
            pDrone->prev_states[1] = pDrone->prev_states[0];
            pDrone->prev_states[0] = pDrone->current_state;
            pDrone->current_state = 1;
            if (gDrone_state_functions[1] != NULL) {
                gDrone_state_functions[1](pDrone, eDrone_state_START);
            }
        }
    }
}

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

// FUNCTION: CARMA2_HW 0x0044cb80
int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    return (float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 18.f
           && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 13.f
           && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 18.f;
}

// FUNCTION: CARMA2_HW 0x0044cbd0
int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    return (float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
           && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
           && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f;
}

// FUNCTION: CARMA2_HW 0x0044cc20
int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    return (float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
           && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
           && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f;

}

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

// PipeDroneMatrix

// PossiblyPipeDroneMovement

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

// FUNCTION: CARMA2_HW 0x0044ecf0
void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on) {

    gTraffic_disabled = !pTraffic_on;
}

// FUNCTION: CARMA2_HW 0x0044ed00
int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void) {
    return !gTraffic_disabled;
}

// GLOBAL: CARMA2_HW 0x0068450c
int gCount_drone_forms;

// GLOBAL: CARMA2_HW 0x00594770
const char* gDrone_type_names[4] = {
    "CAR",
    "PLANE",
    "TRAIN",
    "CHOPPER",
};

// GLOBAL: CARMA2_HW 0x00594780
const char* gGroove_funk_type_names[3] = {
    "spinnyGroove",
    "steeringGroove",
    "spinnyFunk",
};

// FUNCTION: CARMA2_HW 0x0044ed10
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    /* LoadDroneTypeInfo is inlined here; based on carpocalypse2 src/carma2/common/loading.c:900 and :757 */
    int version;
    int drone_version;
    FILE* f;
    FILE* df;
    tPath_name the_path;
    tPath_name drone_path;
    char s[256];
    char s2[256];
    char* str;
    tTWTVFS twt = -1;
    tDrone_form* drone;
    int i;
    const char** p;
    int type_none = -1;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, "DRONE.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    GetALineAndDontArgue(f, s);
    strtok(s, "\t ,/");
    if (strcmp(s, "VERSION") == 0) {
        str = strtok(NULL, "\t ,/");
        if (sscanf(str, "%d", &version) != 1) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
        }
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    memset(gDrone_forms, 0, sizeof(gDrone_forms));
    gCount_drone_forms = 0;
    GetALineAndDontArgue(f, s);
    for (; gCount_drone_forms < 64; gCount_drone_forms++) {
        if (strcmp(s, "END OF DRONES") == 0 || PFfeof(f)) {
            break;
        }
        strcpy(drone_path, gApplication_path);
        PathCat(drone_path, drone_path, "DRONES");
        PathCat(drone_path, drone_path, s);
        twt = OpenPackFileAndSetTiffLoading(drone_path);
        df = OpenDroneFile(s);
        drone = &gDrone_forms[gCount_drone_forms];

        GetALineAndDontArgue(df, s2);
        strtok(s2, "\t ,/");
        if (strcmp(s2, "VERSION") == 0) {
            str = strtok(NULL, "\t ,/");
            if (sscanf(str, "%d", &drone_version) != 1) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
            }
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }
        if (drone_version < 2) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        if (strcmp(s, str) != 0) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }
        strncpy(drone->name, s, 12);

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        i = 0;
        for (p = gDrone_type_names; p < &gDrone_type_names[4]; p++) {
            if (strcmp(*p, str) == 0) {
                break;
            }
            i++;
        }
        if (p >= &gDrone_type_names[4]) {
            i = type_none;
        }
        if (i < 0) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        } else {
            drone->type = i;
        }
        drone->M = GetAScalar(df);
        GetThreeFloats(df, &drone->center.v[0], &drone->center.v[1], &drone->center.v[2]);

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        if (strcmp(str, "smooth") == 0) {
            drone->flags |= 0x1;
        } else if (strcmp(str, "sharp") == 0) {
            drone->flags &= ~0x1;
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        if (strcmp(str, "constant") == 0) {
            drone->speed = GetAScalar(df);
        } else if (strcmp(str, "variable") == 0) {
            drone->speed = -1.f;
            drone->max_acceleration = GetAScalar(df);
            drone->max_speed = GetAScalar(df);
            drone->min_speed = GetAScalar(df);
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }
        drone->crushability = GetAScalar(df);
        if (drone->crushability != 0.f) {
            GetPairOfFloats(df, &drone->crush_limits_front, &drone->crush_limits_back);
            GetPairOfFloats(df, &drone->crush_limits_left, &drone->crush_limits_right);
            GetPairOfFloats(df, &drone->crush_limits_bottom, &drone->crush_limits_top);
        }

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        if (strcmp(str, "respawn") == 0) {
            drone->flags |= 0x2;
        } else if (strcmp(str, "norespawn") == 0) {
            drone->flags &= ~0x2;
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        if (strcmp(str, "vertical") == 0) {
            drone->flags |= 0x10;
        } else if (strcmp(str, "inline") == 0) {
            drone->flags &= ~0x10;
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }

        GetALineAndDontArgue(df, s2);
        str = strtok(s2, "\t ,/");
        if (strcmp(str, "always") == 0) {
            drone->flags |= 0x4;
        } else if (strcmp(str, "distance") == 0) {
            drone->flags &= ~0x4;
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
        }

        drone->flags &= ~0x8;
        if (drone_version >= 3) {
            GetALineAndDontArgue(df, s2);
            str = strtok(s2, "\t ,/");
            if (strcmp(str, "drivable_on") == 0) {
                drone->flags |= 0x8;
            } else if (strcmp(str, "not_drivable_on") == 0) {
                drone->flags &= ~0x8;
            } else {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, s);
            }
        }
        PFfclose(df);
        ClosePackFileAndSetTiffLoading(twt);
        GetALineAndDontArgue(f, s);
    }
    PFfclose(f);
}

// InitDrones

// DisposeDronesRaceStuff

// LoadPerRaceDroneStuff

// GetDroneCrushData

// AllocateAndInitDrones

// LoadInDronePaths

// STUB: CARMA2_HW 0x00450bf0
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004512d0
void C2_HOOK_FASTCALL DronePreCollisionStuff(void) {

    gFrame += 1;
    gINT_00681fb0 = 0;
}

// DoDronePerGameFrameStuff

// MyDroneHathCollideth

// MyDroneHathHalteth

// FUNCTION: CARMA2_HW 0x00452410
void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2) {
    tDrone_spec* pDrone;

    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf("MoveMyDroneBaby(): Drone %d, State %d", pDrone->id, pDrone->current_state);
}

// DroneLookWhereYoureGoingYouBlindCunt

// FUNCTION: CARMA2_HW 0x00452680
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452690
br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {

    return &gDrone_specs[gCurrent_selected_drone].actor->t.t.mat;
}

// FUNCTION: CARMA2_HW 0x004526c0
br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void) {
    tDrone_spec* pDrone;

    pDrone = &gDrone_specs[gCurrent_selected_drone];
    if (pDrone->actor->render_style != BR_RSTYLE_FACES &&
        !(gShow_drone_paths && pDrone->actor->render_style == BR_RSTYLE_BOUNDING_FACES)) {
        if (gShow_drone_paths || (pDrone->field_0x44 && gCount_rendered_drones < 10)) {
            DoNotDprintf("START RENDERING: %d", pDrone->id);
            gCount_rendered_drones += 1;
            pDrone->actor->render_style = BR_RSTYLE_FACES;
            PipeSingleDroneRender(pDrone, 1);
        } else {
            DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
            if (pDrone->current_state != 1) {
                if (gDrone_state_functions[pDrone->current_state] != NULL) {
                    gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
                }
                pDrone->time_last_munge = gTime_stamp_for_this_munging;
                pDrone->prev_states[1] = pDrone->prev_states[0];
                pDrone->prev_states[0] = pDrone->current_state;
                pDrone->current_state = 1;
                if (gDrone_state_functions[1] != NULL) {
                    gDrone_state_functions[1](pDrone, eDrone_state_START);
                }
            }
        }
    }
    if (BrVector3LengthSquared(&gDrone_specs[gCurrent_selected_drone].field_0x18) > gDrone_render_zero) {
        return &gDrone_specs[gCurrent_selected_drone].field_0x18;
    } else {
        return &gDefault_drone_direction;
    }
}

// FUNCTION: CARMA2_HW 0x00452810
void C2_HOOK_FASTCALL ApplyDroneRender2(int pIndex, int pRender) {

    if (gAction_replay_mode && ARGetReplayRate() < gDrone_render_zero) {
        pRender = (pRender == 0);
        DoNotDprintf("B DRONERENDER");
    } else {
        DoNotDprintf("F DRONERENDER");
    }
    gDrone_specs[pIndex].actor->render_style = (pRender == 0) ? BR_RSTYLE_NONE : BR_RSTYLE_FACES;
}

// ApplyDroneStraightPos2

// ApplyDroneCornerPos2

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

