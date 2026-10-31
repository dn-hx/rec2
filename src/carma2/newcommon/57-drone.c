#include "57-drone.h"
#include "globvars.h"
#include "52-errors.h"
#include "08-loading1.h"
#include "41-utility.h"
#include "c2_string.h"
#include "platform.h"
#include "23-piping.h"
#include "globvrpb.h"

#include <math.h>

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

#include "41-utility.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x006820d0
int gCount_drones;

// GLOBAL: CARMA2_HW 0x006820d8
tDrone_crush_data gDrone_crush_data;

// GLOBAL: CARMA2_HW 0x00684518
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00684510
int gCount_drone_path_nodes;

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

// GLOBAL: CARMA2_HW 0x00681fb8
tPath_name gDrone_path_output_name;

// GLOBAL: CARMA2_HW 0x00684534
int gDrone_path_output_ready;

// GLOBAL: CARMA2_HW 0x005947c0
br_vector3 gDefault_drone_direction = {{ 0.f, 0.f, -1.f }};

/* separate 0.0f in .rdata used only here (not the shared 0x0058a988 literal) */
// GLOBAL: CARMA2_HW 0x00589dc4
float gDrone_render_zero = 0.0f;

static void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...);
extern int gCount_active_drones;
extern tDrone_state_function* gDrone_state_functions[6];
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4];
extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone);

/* inlined copy of CrappyLittleVector3DPrintf (0x451620) at this call site in the original */
static void C2_HOOK_CDECL DroneCrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

// based on REA pseudocode 0x44cc70 and carpocalypse2 src/carma2/common/drone.c:1100 and :1087
// FUNCTION: CARMA2_HW 0x0044cc70
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state) {
    br_model* model;
    br_model* orig_model;

    switch ((int)state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncReset() START");
        if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
            DoNotDprintf("STOP RENDERING: %d", pDrone->id);
            gCount_rendered_drones -= 1;
            pDrone->actor->render_style = BR_RSTYLE_NONE;
            PipeSingleDroneRender(pDrone, 0);
        }
        if (pDrone->field_0x44) {
            pDrone->field_0x44 = 0;
            gCount_active_drones -= 1;
            if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
                DoNotDprintf("STOP RENDERING: %d", pDrone->id);
                gCount_rendered_drones -= 1;
                pDrone->actor->render_style = BR_RSTYLE_NONE;
                PipeSingleDroneRender(pDrone, 0);
            }
            PHILRemoveObject(&pDrone->collision_info);
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, (tDroneStateFuncState)2);
            }
            DoNotDprintf("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            DroneCrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
        DoNotDprintf("SemiInitDroneSpec() - REINITIALISING DRONE SPEC");
        pDrone->field_0xa_pathnode_id = pDrone->field_0x8_pathnode_id;
        pDrone->field_0xe = -1;
        pDrone->field_0xc = -1;
        pDrone->field_0x14 = 0;
        pDrone->field_0x10 = 0;
        BrVector3Set(&pDrone->field_0x18, 0.f, 0.f, 0.f);
        BrVector3Copy(&pDrone->pos, &gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position);
        pDrone->field_0xdc = 0;
        pDrone->field_0xe4 = 1.0f;
        pDrone->field_0x48 = 1.0f;
        if (pDrone->field_0xf4 >= 0) {
            model = pDrone->form->models[pDrone->field_0xf4];
            orig_model = gDroneStorage.models[pDrone->form->model_index];
            model->faces = orig_model->faces;
            model->nfaces = orig_model->nfaces;
            memcpy(model->vertices, orig_model->vertices, model->nvertices * sizeof(br_vertex));
            pDrone->form->field_0x80[pDrone->field_0xf4] = 0;
            pDrone->field_0xf4 = -1;
            pDrone->model_actor->model = gDroneStorage.models[pDrone->form->model_index];
        }
        pDrone->field_0x45 = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
        pDrone->field_0x5d4 = 0;
        PipeDroneMatrix(pDrone);
        BrVector3Copy(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[(tS16)pDrone->field_0x8_pathnode_id].position);
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncReset() RUN");
        if (!gShow_drone_paths) {
            DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 2);
            if (pDrone->current_state != 2) {
                if (gDrone_state_functions[pDrone->current_state] != NULL) {
                    gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
                }
                pDrone->time_last_munge = gTime_stamp_for_this_munging;
                pDrone->prev_states[1] = pDrone->prev_states[0];
                pDrone->prev_states[0] = pDrone->current_state;
                pDrone->current_state = 2;
                if (gDrone_state_functions[2] != NULL) {
                    gDrone_state_functions[2](pDrone, eDrone_state_START);
                }
            }
        }
        break;
    case eDrone_state_STOP:
        DoNotDprintf("DroneStateFuncReset() STOP");
        break;
    default:
        break;
    }
}

// based on REA pseudocode 0x44d1d0 and carpocalypse2 src/carma2/common/drone.c:1285 and :1293
// FUNCTION: CARMA2_HW 0x0044d1d0
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncControlledMovement() START");
        pDrone->field_0xdc = 0;
        pDrone->field_0xe4 = 0.f;
        pDrone->field_0x48 = 0.f;
        pDrone->field_0x4c = 0.f;
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncControlledMovement() RUN");
        if (pDrone->field_0x44) {
            switch (pDrone->form->type) {
            case kDroneType_plane:
                MoveThisDronePlane(pDrone);
                break;
            default:
                MoveThisDroneCar(pDrone);
                break;
            }
            if (pDrone->field_0xdc == 1) {
                PipeSingleDroneStraightPos(pDrone, pDrone->field_0x10,
                    DRScalarToU16(pDrone->field_0x48, -500.f, 500.f),
                    DRScalarToU16(pDrone->field_0x4c, 0.f, 255.f));
                return;
            }
            if (pDrone->field_0xdc == 2) {
                PipeDroneMatrix(pDrone);
                return;
            }
            PDEnterDebugger("Keith");
            return;
        }
        break;
    default:
        DoNotDprintf("DroneStateFuncControlledMovement() DEFAULT");
        break;
    }
}

extern tDrone_state_function* gDrone_state_functions[6];
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
static void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...);

// FUNCTION: CARMA2_HW 0x0044eb70
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncPhysicsActive() RUN");
        if (pDrone->field_0x44) {
            if (!(pDrone->form->flags & 0x4)) {
                return;
            }
            if (pDrone->field_0x45) {
                return;
            }
        }
        if (!(pDrone->form->flags & 0x2)) {
            return;
        }
        PHILRemoveObject(&pDrone->collision_info);
        pDrone->collision_info.field_0x1a0 = 0x100000;
        PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
        pDrone->field_0x5d4 = 0;
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
        break;
    }
}

// based on REA pseudocode 0x44ec50 and carpocalypse2 src/carma2/common/drone.c:1311 (NewDroneState inlined)
// FUNCTION: CARMA2_HW 0x0044ec50
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncStationaryPassive() RUN");
        if (pDrone->field_0x44) {
            if (!(pDrone->form->flags & 0x4)) {
                return;
            }
            if (pDrone->field_0x45) {
                return;
            }
        }
        if (!(pDrone->form->flags & 0x2)) {
            return;
        }
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
        break;
    }
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

// based on carpocalypse2 src/carma2/common/drone.c:761 (inlined in the original)
static int C2_HOOK_FASTCALL DroneOrientationChanged(tDrone_spec* pDrone, float pDot) {
    br_actor* actor;

    actor = pDrone->actor;
    if (pDrone->collision_info.transform_matrix.m[0][1] * actor->t.t.mat.m[0][1]
            + pDrone->collision_info.transform_matrix.m[0][2] * actor->t.t.mat.m[0][2]
            + pDrone->collision_info.transform_matrix.m[0][0] * actor->t.t.mat.m[0][0] < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)actor->t.t.mat.m[1], (br_vector3*)pDrone->collision_info.transform_matrix.m[1]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)actor->t.t.mat.m[2], (br_vector3*)pDrone->collision_info.transform_matrix.m[2]) < pDot) {
        return 1;
    }
    return 0;
}

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

extern void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone_spec);
extern tPhysics_object* gPHIL_list_collision_infos;
extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pObject1, tPhysics_object* pObject2);

/* inlined into its callers in the original (no standalone copy) */
static int C2_HOOK_FASTCALL DroneHasCollided(tDrone_spec* pDrone_spec) {
    tPhysics_object* collision;

    for (collision = gPHIL_list_collision_infos; collision != NULL; collision = collision->next) {
        if (&pDrone_spec->collision_info != collision && TestObjectOverlap(&pDrone_spec->collision_info, collision)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004518e0
int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone) {

    if (gTime_stamp_for_this_munging > pDrone->last_collide_check + 1000) {
        pDrone->last_collide_check = gTime_stamp_for_this_munging;
        InitDroneCollisionObject(pDrone);

        return !DroneHasCollided(pDrone);
    } else {
        return 0;
    }
}

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

extern float gDrone_delta_time;
extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);

// based on REA pseudocode 0x451ca0 and carpocalypse2 src/carma2/common/drone.c:778
// FUNCTION: CARMA2_HW 0x00451ca0
void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex) {
    tDrone_spec* drone;
    float len;

    drone = &gDrone_specs[pIndex];
    BrVector3Copy(&drone->pos, &drone->actor->t.t.translate.t);
    if (gDrone_state_functions[drone->current_state] != NULL) {
        gDrone_state_functions[drone->current_state](drone, eDrone_state_RUN);
    }
    if (drone->field_0x45) {
        DoDroneFunkyGroovyThings(drone);
    }
    BrVector3Sub(&drone->field_0x18, &drone->actor->t.t.translate.t, &drone->pos);
    if ((drone->field_0x44 && DroneOrientationChanged(drone, 0.92f)) || (len = BrVector3Length(&drone->field_0x18)) > 1.0) {
        BrMatrix34Copy(&drone->collision_info.transform_matrix, &drone->actor->t.t.mat);
        DoNotDprintf("REASSERTING OBJECT MATRIX: v length %f, Frame %d, Drone %d, state %d: ",
            len, gFrame, drone->id, drone->current_state);
    }
    drone->field_0x18.v[0] /= gDrone_delta_time;
    drone->field_0x18.v[1] /= gDrone_delta_time;
    drone->field_0x18.v[2] /= gDrone_delta_time;
}

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

// based on carpocalypse2 src/carma2/common/loading.c:3128
// FUNCTION: CARMA2_HW 0x00450b50
tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject) {
    tDrone_spec* pDrone;
    int i;

    pDrone = (tDrone_spec*)pObject->owner;
    if (pDrone->form->crushability == gDrone_render_zero) {
        return NULL;
    }
    if (pDrone->field_0xf4 < 0) {
        for (i = 0; i < pDrone->form->count_models; i++) {
            if (pDrone->form->field_0x80[i] == 0) {
                pDrone->form->field_0x80[i] = 1;
                pDrone->field_0xf4 = i;
                pDrone->model_actor->model = pDrone->form->models[i];
                goto selected_model;
            }
        }
        return NULL;
    }
selected_model:
    gDrone_crush_data.model = pDrone->model_actor->model;
    gDrone_crush_data.crushability = pDrone->form->crushability;
    gDrone_crush_data.bounds = pDrone->form->field_0x54;
    return &gDrone_crush_data;
}

// AllocateAndInitDrones

// FUNCTION: CARMA2_HW 0x00450bf0
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
    int i;
    int j;
    int version;
    int drone_i;
    char* line;
    tDrone_path_node* node;
    br_scalar float_buffer[4];
    char s[256];
    char s2[256];

    DoNotDprintf("Start of LoadInDronePaths()...");
    while (1) {
        line = GetALineAndDontArgue(pF, s);
        if (line == NULL) {
            break;
        }
        if (strcmp("START OF DRONE PATHS", s) == 0) {
            break;
        }
    }
    if (line == NULL) {
        PDFatalError("Corrupt race text file - No drone path info");
    }
    gCount_drones = 0;
    gCount_drone_path_nodes = 0;
    if (gNet_mode == eNet_mode_none && !gTraffic_disabled) {
        version = GetAnInt(pF);
        if (version > 0) {
            if (version != 2) {
                PDFatalError("Drones paths are not version 2");
            }
            gCount_drone_path_nodes = GetAnInt(pF);
            if (gCount_drone_path_nodes != 0) {
                gDrone_path_nodes = BrMemAllocate(gCount_drone_path_nodes * sizeof(tDrone_path_node), kMem_drone_paths);
            }
            for (i = 0; i < gCount_drone_path_nodes; i++) {
                node = &gDrone_path_nodes[i];
                GetNScalars(pF, 3, float_buffer);
                node->position.v[0] = float_buffer[0];
                node->position.v[1] = float_buffer[1];
                node->position.v[2] = float_buffer[2];
                s[0] = '\0';
                GetAString(pF, s);
                Uppercaseificate(s, s);
                node->type = -1;
                if (strlen(s) == 0) {
                    sprintf(s, "Corrupt race text file - drone type corrupt in drone node %d", i);
                    PDFatalError(s);
                } else {
                    if (strcmp(s, "NONE") != 0) {
                        for (j = 0; j < gCount_drone_forms; j++) {
                            if (strcmp(s, gDrone_forms[j].name) == 0) {
                                node->type = j;
                            }
                        }
                        if (node->type < 0) {
                            sprintf(s2, "ERROR - '%s' no such drone type in drone node %d", s, i);
                            PDFatalError(s2);
                        }
                    }
                }
                GetNScalars(pF, 1, float_buffer);
                node->field_0x12e = (int)float_buffer[0];
                node->count_sections = GetAnInt(pF);
                if (node->count_sections > 8) {
                    sprintf(s, "ERROR - drone node %d has too many sections", i);
                    PDFatalError(s);
                }
                for (j = 0; j < node->count_sections; j++) {
                    GetNScalars(pF, 4, float_buffer);
                    node->sections[j].node1 = (int)float_buffer[0];
                    node->sections[j].node2 = (int)float_buffer[1];
                    node->sections[j].field_0x21 = (int)float_buffer[2];
                    node->sections[j].type = (int)float_buffer[3];
                    if (node->sections[j].type > 7) {
                        PDEnterDebugger("Weird type for drone section");
                    }
                }
                if (node->type >= 0) {
                    if (gCount_drones < 200) {
                        gCount_drones = gCount_drones + 1;
                    } else {
                        sprintf(s, "Too many drones in race (limit %d)", 200);
                        PDFatalError(s);
                    }
                }
            }
        }
    }
    while (1) {
        if (GetALineAndDontArgue(pF, s) == NULL) {
            break;
        }
        if (strcmp("END OF DRONE PATHS", s) == 0) {
            break;
        }
    }
    PreprocessDronePaths();
    drone_i = 0;
    gDrone_specs = BrMemAllocate(gCount_drones * sizeof(tDrone_spec), kMem_drone_specs);
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        node = &gDrone_path_nodes[i];
        if (node->type >= 0) {
            InitDroneSpec(&gDrone_specs[drone_i], i);
            gDrone_specs[drone_i].id = drone_i;
            drone_i += 1;
        }
    }
    DoNotDprintf("End of LoadInDronePaths(), totals:");
    DoNotDprintf("Nodes: %d", gCount_drone_path_nodes);
}

// FUNCTION: CARMA2_HW 0x004512d0
void C2_HOOK_FASTCALL DronePreCollisionStuff(void) {

    gFrame += 1;
    gINT_00681fb0 = 0;
}

// DoDronePerGameFrameStuff

// GLOBAL: CARMA2_HW 0x00705b70
tU32 gINT_00705b70;

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);
extern int gMutant_tail_state;

// FUNCTION: CARMA2_HW 0x00452230
int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther) {
    tDrone_spec* pDrone;

    DoNotDprintf("MyDroneHathCollideth(): PHIL called a bloody callback. Rejoice!");
    pDrone = (tDrone_spec*)pObject->owner;
    pObject->disable_move_rotate = 0;
    pObject->field_0xed = 1;
    pObject->field_0x1a0 = 0x200000;
    pObject->v.v[0] = pDrone->field_0x18.v[0];
    pObject->v.v[1] = pDrone->field_0x18.v[1];
    pObject->v.v[2] = pDrone->field_0x18.v[2];
    PHILMakeObjectActive(pObject, NULL, NULL, 1);
    PHILSetObjectProperty(pObject, 3, 0);
    if ((pOther != NULL && pOther->owner != NULL && pOther->flags_0x238 == 1)
            || (pOther->flags_0x238 == 0x20
                && (gMutant_tail_state == 2 || GetTotalTime() - gINT_00705b70 < 0x1d4c))) {
        ((tDrone_spec*)pObject->owner)->field_0x5d4 = (undefined4)pOther->owner;
    } else if (pOther != NULL && pOther->flags_0x238 == 0x40) {
        ((tDrone_spec*)pObject->owner)->field_0x5d4 = ((tDrone_spec*)pOther->owner)->time_last_munge;
    }
    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 3);
    if (pDrone->current_state != 3) {
        if (gDrone_state_functions[pDrone->current_state] != NULL) {
            gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
        }
        pDrone->time_last_munge = gTime_stamp_for_this_munging;
        pDrone->prev_states[1] = pDrone->prev_states[0];
        pDrone->prev_states[0] = pDrone->current_state;
        pDrone->current_state = 3;
        if (gDrone_state_functions[3] != NULL) {
            gDrone_state_functions[3](pDrone, eDrone_state_START);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452360
int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject) {
    tDrone_spec* pDrone;

    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf("MyDroneHathHalteth(): PHIL called a bloody callback. Rejoice!");
    pDrone->collision_info.field_0x1a0 = 0x100000;
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    pDrone->field_0x5d4 = 0;
    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 4);
    if (pDrone->current_state != 4) {
        if (gDrone_state_functions[pDrone->current_state] != NULL) {
            gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
        }
        pDrone->time_last_munge = gTime_stamp_for_this_munging;
        pDrone->prev_states[1] = pDrone->prev_states[0];
        pDrone->prev_states[0] = pDrone->current_state;
        pDrone->current_state = 4;
        if (gDrone_state_functions[4] != NULL) {
            gDrone_state_functions[4](pDrone, eDrone_state_START);
        }
    }
    return 1;
}

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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat="
#endif

extern int C2_HOOK_CDECL fprintf(FILE* pFile, const char* pFormat, ...);

// FUNCTION: CARMA2_HW 0x00453050
void C2_HOOK_FASTCALL WriteOutDronePaths(void) {
    int node_index;
    int section_index;
    int path_index;
    char filename[16];
    char path[256];
    FILE* file;

    if (!gDrone_path_output_ready) {
        path_index = 0;
        sprintf(filename, "DPATH%0.3d.TXT", path_index);
        PathCat(path, gApplication_path, filename);
        file = DRfopen(path, "r+");
        while (file != NULL) {
            PFfclose(file);
            path_index++;
            sprintf(filename, "DPATH%0.3d.TXT", path_index);
            PathCat(path, gApplication_path, filename);
            file = DRfopen(path, "r+");
        }
        gDrone_path_output_ready = 1;
        strcpy(gDrone_path_output_name, path);
    }
    file = DRfopen(gDrone_path_output_name, "wt");
    if (file != NULL) {
        fprintf(file, "%s\n", "START OF DRONE PATHS");
        fprintf(file, "\n%-3d   // version\n", 2);
        fprintf(file, "%-3d   // n_nodes\n", gCount_drone_path_nodes);
        for (node_index = 0; node_index < gCount_drone_path_nodes; node_index++) {
            tDrone_path_node* node;

            node = &gDrone_path_nodes[node_index];
            fprintf(file, "\t\n// %d:\n", node_index);
            fprintf(file, "\t%.3f, %.3f, %.3f\n",
                (double)node->position.v[0],
                (double)node->position.v[1],
                (double)node->position.v[2]);
            fprintf(file, "\t%s\n", node->type >= 0 ? gDrone_forms[node->type].name : "NONE");
            fprintf(file, "\t%d\n", node->field_0x12e);
            fprintf(file, "\t%d\n", node->count_sections);
            for (section_index = 0; section_index < node->count_sections; section_index++) {
                tDrone_path_node_section* section;

                section = &node->sections[section_index];
                fprintf(file, "\t\t%d, %d, %d, %d\n",
                    section->node1,
                    (tU16)section->node2,
                    section->type,
                    section->field_0x21);
            }
        }
        fprintf(file, "\n%s\n", "END OF DRONE PATHS");
        PFfclose(file);
    } else {
        PDFatalError("Unable to open drone path output file");
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00451070
void C2_HOOK_FASTCALL PreprocessDronePaths(void) {
    int i;
    int j;

    for (i = 0; i < gCount_drone_path_nodes; i++) {
        for (j = 0; j < gDrone_path_nodes[i].count_sections; j++) {
            BrVector3Sub(&gDrone_path_nodes[i].sections[j].field_0x04, &gDrone_path_nodes[gDrone_path_nodes[i].sections[j].node1].position, &gDrone_path_nodes[i].position);
            gDrone_path_nodes[i].sections[j].field_0x1c = BrVector3Length(&gDrone_path_nodes[i].sections[j].field_0x04);
            {
                br_scalar len;
                double scale;

                len = BrVector3Length(&gDrone_path_nodes[i].sections[j].field_0x04);
                if (len > BR_SCALAR_EPSILON * 2) {
                    scale = 1.0 / len;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[0] = gDrone_path_nodes[i].sections[j].field_0x04.v[0] * scale;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[1] = gDrone_path_nodes[i].sections[j].field_0x04.v[1] * scale;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[2] = gDrone_path_nodes[i].sections[j].field_0x04.v[2] * scale;
                } else {
                    gDrone_path_nodes[i].sections[j].field_0x10.v[0] = 1.0f;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[1] = 0.0f;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[2] = 0.0f;
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00594760
tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4] = {
    DroneCarWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
    DroneTrainWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
};

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0044cfd0
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044d2a0
void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044e540
void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
