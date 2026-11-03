#include "57-drone.h"
#include "globvars.h"
#include "52-errors.h"
#include "08-loading1.h"
#include "41-utility.h"
#include "c2_string.h"
#include "platform.h"
#include "23-piping.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <math.h>

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);

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

extern void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...);
extern int gCount_active_drones;
extern tDrone_state_function* gDrone_state_functions[6];
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4];
extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone);

/* inlined copy of CrappyLittleVector3DPrintf (0x451620) at this call site in the original */
static void C2_HOOK_CDECL DroneCrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf_drone("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

// PARTIAL 99.59%: DroneStateFuncReset 0x44cc70 - only diff is the table call operand [eax*4+0x594760] (reccmp never names it, LIMITATION_TABLE_CALL)
// based on REA pseudocode 0x44cc70 and and :1087
// FUNCTION: CARMA2_HW 0x0044cc70
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state) {
    br_model* model;
    br_model* orig_model;

    switch ((int)state) {
    case eDrone_state_START:
        DoNotDprintf_drone("DroneStateFuncReset() START");
        if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
            DoNotDprintf_drone("STOP RENDERING: %d", pDrone->id);
            gCount_rendered_drones -= 1;
            pDrone->actor->render_style = BR_RSTYLE_NONE;
            PipeSingleDroneRender(pDrone, 0);
        }
        if (pDrone->field_0x44) {
            pDrone->field_0x44 = 0;
            gCount_active_drones -= 1;
            if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
                DoNotDprintf_drone("STOP RENDERING: %d", pDrone->id);
                gCount_rendered_drones -= 1;
                pDrone->actor->render_style = BR_RSTYLE_NONE;
                PipeSingleDroneRender(pDrone, 0);
            }
            PHILRemoveObject(&pDrone->collision_info);
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, (tDroneStateFuncState)2);
            }
            DoNotDprintf_drone("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            DroneCrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
        DoNotDprintf_drone("SemiInitDroneSpec() - REINITIALISING DRONE SPEC");
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
        DoNotDprintf_drone("DroneStateFuncReset() RUN");
        if (!gShow_drone_paths) {
            DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 2);
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
        DoNotDprintf_drone("DroneStateFuncReset() STOP");
        break;
    default:
        break;
    }
}

// based on REA pseudocode 0x44d1d0 and and :1293
// FUNCTION: CARMA2_HW 0x0044d1d0
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_START:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() START");
        pDrone->field_0xdc = 0;
        pDrone->field_0xe4 = 0.f;
        pDrone->field_0x48 = 0.f;
        pDrone->field_0x4c = 0.f;
        break;
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() RUN");
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
        DoNotDprintf_drone("DroneStateFuncControlledMovement() DEFAULT");
        break;
    }
}

extern tDrone_state_function* gDrone_state_functions[6];
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...);

// FUNCTION: CARMA2_HW 0x0044eb70
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncPhysicsActive() RUN");
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
        DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
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

// based on REA pseudocode 0x44ec50 and (NewDroneState inlined)
// FUNCTION: CARMA2_HW 0x0044ec50
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncStationaryPassive() RUN");
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
        DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
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

// FUNCTION: CARMA2_HW 0x00451620
void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf_drone("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
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

#define DRONE_F(p, off) (*(float*)((char*)(p) + (off)))

// FUNCTION: CARMA2_HW 0x0044dac0
void C2_HOOK_FASTCALL CalculateDrivingInfo(tDrone_spec* pDrone) {
    float max_radius;
    float min_radius;
    tDrone_form* form;
    tDrone_path_node* node_a;
    tDrone_path_node* node_b;
    tDrone_path_node* node_c;
    br_vector3 v1;
    br_vector3 v2;
    br_vector3 n;
    br_vector3 t;
    br_vector3 axis;
    br_vector3 a;
    br_vector3 b;
    br_vector3 d;
    int ang;
    float f_ang;
    float len_a;
    float len_b;
    float dot;
    float cross;
    float s;
    float k;
    float r;
    float mag;
    float len;
    int left;

    form = pDrone->form;
    switch (form->type) {
    case 1:
        min_radius = 0.1f;
        max_radius = 3.0f;
        break;
    default:
        min_radius = 0.02f;
        max_radius = 0.5f;
        break;
    }
    pDrone->field_0xe0 = 0.0f;
    pDrone->field_0x74 = 0.0f;
    if (pDrone->field_0x14 == 0 || (form->flags & 1) == 0) {
        DRONE_F(pDrone, 0x68) = 1000.0f;
        DRONE_F(pDrone, 0x6c) = 0.0f;
    } else {
        node_a = &gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id];
        node_b = &gDrone_path_nodes[pDrone->field_0xc];
        node_c = &gDrone_path_nodes[pDrone->field_0xe];
        BrVector3Sub(&v1, &node_a->position, &node_b->position);
        BrVector3Sub(&v2, &node_c->position, &node_b->position);
        BrVector3Cross(&n, &v2, &v1);
        if (n.v[1] < 0.0f) {
            n.v[0] = -n.v[0];
            n.v[1] = -n.v[1];
            n.v[2] = -n.v[2];
        }
        BrVector3Normalise(&n, &n);
        t.v[0] = 0.0f - n.v[2];
        t.v[1] = 0.0f;
        t.v[2] = n.v[0] - 0.0f;
        BrVector3Normalise(&axis, &t);
        ang = (int)(acos(n.v[1]) * 10430.378350470453);
        f_ang = (float)(ang & 0xffff) * 9.58738e-05f;
        s = f_ang;
        if (s > 3.141592653589793) {
            s = 0.0f;
        }
        if (s > 1.5707963267948966) {
            s = 3.141592653589793 - s;
        }
        if (form->type != 1 && form->type != 3) {
            max_radius = (max_radius - min_radius) * (1.0 - s * 0.6366197723675814) + min_radius;
        }
        BrMatrix34Rotate((br_matrix34*)((char*)pDrone + 0x78), ang, &axis);
        BrMatrix34LPInverse((br_matrix34*)((char*)pDrone + 0xa8), (br_matrix34*)((char*)pDrone + 0x78));
        BrMatrix34ApplyV(&a, &v1, (br_matrix34*)((char*)pDrone + 0x78));
        BrMatrix34ApplyV(&b, &v2, (br_matrix34*)((char*)pDrone + 0x78));
        len_a = a.v[2] * a.v[2] + a.v[0] * a.v[0];
        len_b = b.v[2] * b.v[2] + b.v[0] * b.v[0];
        if (len_a != 0.0f && len_b != 0.0f) {
            dot = b.v[2] * a.v[2] + b.v[0] * a.v[0];
            cross = b.v[0] * a.v[2] + a.v[0] * -b.v[2];
            mag = sqrt(cross * cross + dot * dot);
            if (mag <= 2.3841858e-07f) {
                dot = 1.0f;
                mag = 0.0f;
            } else {
                mag = 1.0 / mag;
                dot = mag * dot;
                mag = mag * cross;
            }
            s = mag;
            if (mag <= 0.0f) {
                s = -mag;
            }
            left = mag > 0.0f;
            if (s >= 0.04f) {
                r = (dot * max_radius + max_radius) / s;
                s = s / (dot - -1.0f);
                r = s * max_radius + sqrt(((s * s * max_radius - r * s * -2.0f) - max_radius) * max_radius) + r;
                DRONE_F(pDrone, 0x6c) = r;
                if (len_a < r * r * 9.0f) {
                    DRONE_F(pDrone, 0x6c) = sqrt(len_a) * 0.33333334f;
                }
                if (len_b < DRONE_F(pDrone, 0x6c) * DRONE_F(pDrone, 0x6c) * 9.0f) {
                    DRONE_F(pDrone, 0x6c) = sqrt(len_b) * 0.33333334f;
                }
                s = s * DRONE_F(pDrone, 0x6c);
                DRONE_F(pDrone, 0x68) = s;
                if (left) {
                    DRONE_F(pDrone, 0x54) = b.v[0] * DRONE_F(pDrone, 0x6c) + s * -b.v[2];
                    DRONE_F(pDrone, 0x58) = b.v[0] * DRONE_F(pDrone, 0x68) + b.v[2] * DRONE_F(pDrone, 0x6c);
                } else {
                    DRONE_F(pDrone, 0x54) = b.v[0] * DRONE_F(pDrone, 0x6c) + b.v[2] * s;
                    DRONE_F(pDrone, 0x58) = b.v[2] * DRONE_F(pDrone, 0x6c) + -DRONE_F(pDrone, 0x68) * b.v[0];
                }
                DRONE_F(pDrone, 0x54) = DRONE_F(pDrone, 0x54) / sqrt(len_b);
                DRONE_F(pDrone, 0x58) = DRONE_F(pDrone, 0x58) / sqrt(len_b);
                k = DRONE_F(pDrone, 0x54);
                d.v[2] = -DRONE_F(pDrone, 0x58);
                d.v[0] = (a.v[0] - DRONE_F(pDrone, 0x54)) - -k;
                d.v[1] = (a.v[2] - DRONE_F(pDrone, 0x58)) - d.v[2];
                pDrone->left_turn = left;
                len = sqrt(d.v[1] * d.v[1] + d.v[0] * d.v[0]);
                r = (1.0f - (float)sin(f_ang) * -7.0f) * sqrt(DRONE_F(pDrone, 0x58) * DRONE_F(pDrone, 0x58) + DRONE_F(pDrone, 0x54) * DRONE_F(pDrone, 0x54));
                DRONE_F(pDrone, 0xe8) = r;
                DRONE_F(pDrone, 0x70) = r;
                if (len <= 2.3841858e-07f) {
                    d.v[1] = 0.0f;
                } else {
                    d.v[1] = (1.0 / len) * d.v[1];
                }
                r = DRONE_F(pDrone, 0x6c);
                DRONE_F(pDrone, 0x5c) = (float)acos(d.v[1]);
                if (r * d.v[1] + d.v[2] < 0.0f) {
                    DRONE_F(pDrone, 0x5c) = (float)(6.283185307179586 - acos(d.v[1]));
                }
                r = DRONE_F(pDrone, 0x6c);
                d.v[0] = (b.v[0] - DRONE_F(pDrone, 0x54)) - -k;
                d.v[1] = (b.v[2] - DRONE_F(pDrone, 0x58)) - d.v[2];
                len = sqrt(d.v[1] * d.v[1] + d.v[0] * d.v[0]);
                if (len <= 2.3841858e-07f) {
                    d.v[1] = 0.0f;
                } else {
                    d.v[1] = (1.0 / len) * d.v[1];
                }
                DRONE_F(pDrone, 0x64) = (float)acos(d.v[1]);
                if (r * d.v[1] + d.v[2] < 0.0f) {
                    DRONE_F(pDrone, 0x64) = (float)(6.283185307179586 - acos(d.v[1]));
                }
                if (left) {
                    if (DRONE_F(pDrone, 0x64) < DRONE_F(pDrone, 0x5c)) {
                        len = DRONE_F(pDrone, 0x64);
                        do {
                            len = len - -6.2831855f;
                        } while (len < DRONE_F(pDrone, 0x5c));
                        DRONE_F(pDrone, 0x64) = len;
                    }
                } else if (DRONE_F(pDrone, 0x5c) < DRONE_F(pDrone, 0x64)) {
                    do {
                        len = DRONE_F(pDrone, 0x5c) - -6.2831855f;
                        DRONE_F(pDrone, 0x5c) = len;
                    } while (len < DRONE_F(pDrone, 0x64));
                }
                pDrone->field_0xdc = 1;
                DRONE_F(pDrone, 0x60) = DRONE_F(pDrone, 0x5c);
                goto done;
            }
        }
        DRONE_F(pDrone, 0x68) = 1000.0f;
        DRONE_F(pDrone, 0x6c) = 0.0f;
    }
done:
    form = pDrone->form;
    if (form->speed >= 0.0f) {
        pDrone->field_0x50 = form->speed;
    } else {
        s = DRONE_F(pDrone, 0x68) * 0.5f;
        pDrone->field_0x50 = s;
        len = s;
        if (s < form->min_speed) {
            len = form->min_speed;
        }
        if (len <= form->max_speed) {
            if (s < form->min_speed) {
                s = form->min_speed;
            }
            pDrone->field_0x50 = s;
        } else {
            pDrone->field_0x50 = form->max_speed;
        }
    }
    DoNotDprintf_drone("CalculateDrivingInfo called - centre node %d, radius %f, section after corner %p", pDrone->field_0xc, (double)DRONE_F(pDrone, 0x68), (void*)pDrone->field_0x10);
}

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

    DoNotDprintf_drone("InitDroneSpec() - INITIALISING DRONE SPEC");
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

// (inlined in the original)
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
        DoNotDprintf_drone("STOP RENDERING: %d", pDrone_spec->id);
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

        DoNotDprintf_drone("START RENDERING: %d", pDrone->id);
        gCount_rendered_drones += 1;
        pDrone->actor->render_style = BR_RSTYLE_FACES;
        PipeSingleDroneRender(pDrone, 1);
    } else {
        DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
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

// based on REA pseudocode 0x451ca0 and // FUNCTION: CARMA2_HW 0x00451ca0
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
        DoNotDprintf_drone("REASSERTING OBJECT MATRIX: v length %f, Frame %d, Drone %d, state %d: ",
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
const char* gGroove_funk_type_names[4] = {
    "spinnyGroove",
    "steeringGroove",
    "spinnyFunk",
    NULL,
};

// FUNCTION: CARMA2_HW 0x0044ed10
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    /* LoadDroneTypeInfo is inlined here; and :757 */
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

    DoNotDprintf_drone("Start of LoadInDronePaths()...");
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
    DoNotDprintf_drone("End of LoadInDronePaths(), totals:");
    DoNotDprintf_drone("Nodes: %d", gCount_drone_path_nodes);
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

    DoNotDprintf_drone("MyDroneHathCollideth(): PHIL called a bloody callback. Rejoice!");
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
    DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 3);
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
    DoNotDprintf_drone("MyDroneHathHalteth(): PHIL called a bloody callback. Rejoice!");
    pDrone->collision_info.field_0x1a0 = 0x100000;
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    pDrone->field_0x5d4 = 0;
    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 4);
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
    DoNotDprintf_drone("MoveMyDroneBaby(): Drone %d, State %d", pDrone->id, pDrone->current_state);
}

// DroneLookWhereYoureGoingYouBlindCunt

// FUNCTION: CARMA2_HW 0x004525c0
void C2_HOOK_FASTCALL DroneNoOp(void) {
}

// FUNCTION: CARMA2_HW 0x004525d0
void C2_HOOK_FASTCALL SelectNextDrone(void) {
    char text[64];
    extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);

    gCurrent_selected_drone++;
    if (gCurrent_selected_drone >= gCount_drones) {
        gCurrent_selected_drone = 0;
    }
    sprintf(text, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, text);
}

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
            DoNotDprintf_drone("START RENDERING: %d", pDrone->id);
            gCount_rendered_drones += 1;
            pDrone->actor->render_style = BR_RSTYLE_FACES;
            PipeSingleDroneRender(pDrone, 1);
        } else {
            DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
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
        DoNotDprintf_drone("B DRONERENDER");
    } else {
        DoNotDprintf_drone("F DRONERENDER");
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

// PARTIAL 88.46%: PipeDroneMatrix 0x44cfd0 - radius clamp copied via mov instead of fld/fstp, arg eval order/regs differ
// FUNCTION: CARMA2_HW 0x0044cfd0
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) {
    tCompressed_matrix34 matrix;
    float l_radius;
    float l_48;
    extern void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pField_0x70, tCompressed_matrix34* pMatrix);

    matrix.m[0].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[0][0], -1.f, 1.f);
    matrix.m[0].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[0][1], -1.f, 1.f);
    matrix.m[0].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[0][2], -1.f, 1.f);
    matrix.m[1].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[1][0], -1.f, 1.f);
    matrix.m[1].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[1][1], -1.f, 1.f);
    matrix.m[1].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[1][2], -1.f, 1.f);
    matrix.m[2].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[2][0], -1.f, 1.f);
    matrix.m[2].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[2][1], -1.f, 1.f);
    matrix.m[2].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[2][2], -1.f, 1.f);
    matrix.m[3].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[3][0], -1000.f, 1000.f);
    matrix.m[3].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[3][1], -1000.f, 1000.f);
    matrix.m[3].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[3][2], -1000.f, 1000.f);
    l_radius = (pDrone->h_radius < 100.f) ? pDrone->h_radius : 100.f;
    if (pDrone->left_turn) {
        l_48 = pDrone->field_0x48;
    } else {
        l_48 = -pDrone->field_0x48;
    }
    PipeSingleDroneCornerPos(pDrone,
        DRScalarToU16(l_48, -500.f, 500.f),
        DRScalarToU16(pDrone->field_0x74, 0.f, 1.f),
        DRScalarToU16(l_radius, 0.f, 100.f),
        &matrix);
}

extern int C2_HOOK_FASTCALL AdvanceDroneSection(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);
extern void C2_HOOK_FASTCALL CalculateDrivingInfo(tDrone_spec* pDrone);

#define DRONE_SEC(p) ((tDrone_path_node_section*)(p)->field_0x10)
#define DRONE_SEC2(p) ((tDrone_path_node_section*)(p)->field_0x14)

// PARTIAL 67.47%: MoveThisDronePlane 0x0044d2a0 - x87 scheduling/reg alloc differences (corner angle compare, type if-chain layout, rem kept in memory)
// FUNCTION: CARMA2_HW 0x0044d2a0
void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) {
    float dt;
    float target;
    double target_d;
    float consumed;
    float sum;
    float rem;
    float decel;
    float accel;
    float time_to;
    float ang;
    float t;
    float l_time;
    int done;
    br_vector3 vec;
    br_vector3 va;
    br_vector3 vb;

    DoNotDprintf_drone("MoveThisDronePlane()");
    dt = gDrone_delta_time;
    if (dt <= gDrone_render_zero) {
        return;
    }
    do {
        switch (pDrone->field_0xdc) {
        case 0:
            DoNotDprintf_drone("eDDS_straight_start");
            if (!AdvanceDroneSection(pDrone)) {
                DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 4);
                if (pDrone->current_state == 4) {
                    return;
                }
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
                return;
            }
            CalculateDrivingInfo(pDrone);
            PointActorAlongThisBloodyVector(pDrone->actor, &DRONE_SEC(pDrone)->field_0x04);
            pDrone->field_0xdc = 1;
            break;
        case 1:
            sum = dt * pDrone->field_0x48 + pDrone->field_0x4c;
            if (DRONE_SEC(pDrone)->type < 4) {
                target = pDrone->form->max_speed;
                target_d = target;
                DoNotDprintf_drone("eDDS_straight_driving: TAKEOFF/INFLIGHT %f %f", target_d, (double)pDrone->field_0x48);
            } else if (DRONE_SEC(pDrone)->type <= 5) {
                target = pDrone->form->max_speed * 0.66f;
                target_d = target;
                DoNotDprintf_drone("eDDS_straight_driving: APPROACH %f %f", target_d, (double)pDrone->field_0x48);
            } else if (DRONE_SEC(pDrone)->type == 6) {
                target = pDrone->form->min_speed;
                target_d = target;
                DoNotDprintf_drone("eDDS_straight_driving: TAXI/LANDING %f %f", target_d, (double)pDrone->field_0x48);
            } else {
                target = pDrone->form->max_speed;
                target_d = target;
                DoNotDprintf_drone("eDDS_straight_driving: TAKEOFF/INFLIGHT %f %f", target_d, (double)pDrone->field_0x48);
            }
            rem = DRONE_SEC(pDrone)->field_0x1c - DRONE_F(pDrone, 0x6c);
            if (pDrone->field_0x4c < rem) {
                if (sum > rem) {
                    if (pDrone->field_0x48 <= target) {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 2b %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = dt;
                    } else {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 2a %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = (pDrone->field_0x48 - target) / pDrone->form->max_acceleration;
                        pDrone->field_0x48 = target;
                    }
                } else {
                    decel = rem - (pDrone->field_0x48 * pDrone->field_0x48 - target * target) * 0.5f;
                    DoNotDprintf_drone("eDDS_straight_driving: decel_envelope %f %f %f", (double)decel, target_d, (double)pDrone->field_0x48);
                    if (DRONE_SEC(pDrone)->type != 2 && DRONE_SEC(pDrone)->type != 3 && pDrone->field_0x48 > target && pDrone->field_0x4c >= decel) {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 4 %f %f", target_d, (double)pDrone->field_0x48);
                        accel = -((pDrone->field_0x48 * pDrone->field_0x48 - target * target) / (((DRONE_SEC(pDrone)->field_0x1c - DRONE_F(pDrone, 0x6c)) - pDrone->field_0x4c) * 2.f));
                        time_to = (pDrone->field_0x48 - target) / -accel;
                        DoNotDprintf_drone("eDDS_straight_driving: a %f, time_to_hit_max_corner_speed %f %f %f", (double)accel, (double)time_to, target_d, (double)pDrone->field_0x48);
                        if (time_to <= dt) {
                            DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 4a %f %f", target_d, (double)pDrone->field_0x48);
                            pDrone->field_0x48 = target;
                            consumed = time_to;
                        } else {
                            DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 4b %f %f", target_d, (double)pDrone->field_0x48);
                            pDrone->field_0x48 = accel * dt + pDrone->field_0x48;
                            consumed = dt;
                        }
                    } else if (DRONE_SEC(pDrone)->type != 2 && DRONE_SEC(pDrone)->type != 3 && pDrone->field_0x48 > target && sum > decel) {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 5 %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = dt;
                    } else if (DRONE_SEC(pDrone)->type != 2 && DRONE_SEC(pDrone)->type != 3 && pDrone->field_0x48 > target && pDrone->field_0x48 * dt + dt * dt * pDrone->form->max_acceleration * 0.5f + pDrone->field_0x4c > decel) {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 6 %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = dt;
                    } else {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 7 %f %f", target_d, (double)pDrone->field_0x48);
                        if (pDrone->field_0x48 > target) {
                            pDrone->field_0x48 = pDrone->field_0x48 - pDrone->form->max_acceleration * dt;
                            if (pDrone->field_0x48 < pDrone->form->min_speed) {
                                pDrone->field_0x48 = pDrone->form->min_speed;
                            }
                        } else {
                            pDrone->field_0x48 = pDrone->form->max_acceleration * dt + pDrone->field_0x48;
                            if (pDrone->field_0x48 > pDrone->form->max_speed) {
                                pDrone->field_0x48 = pDrone->form->max_speed;
                            }
                        }
                        consumed = gDrone_delta_time;
                    }
                }
            } else {
                pDrone->field_0xdc = 2;
                consumed = gDrone_render_zero;
            }
            if (pDrone->field_0xdc == 2) {
                pDrone->field_0x4c = DRONE_SEC(pDrone)->field_0x1c - DRONE_F(pDrone, 0x6c);
                pDrone->actor->t.t.translate.t.v[0] = DRONE_SEC(pDrone)->field_0x10.v[0] * pDrone->field_0x4c;
                pDrone->actor->t.t.translate.t.v[1] = DRONE_SEC(pDrone)->field_0x10.v[1] * pDrone->field_0x4c;
                pDrone->actor->t.t.translate.t.v[2] = DRONE_SEC(pDrone)->field_0x10.v[2] * pDrone->field_0x4c;
            } else {
                pDrone->field_0x4c = consumed * pDrone->field_0x48 + pDrone->field_0x4c;
                pDrone->actor->t.t.translate.t.v[0] = DRONE_SEC(pDrone)->field_0x10.v[0] * pDrone->field_0x4c;
                pDrone->actor->t.t.translate.t.v[1] = DRONE_SEC(pDrone)->field_0x10.v[1] * pDrone->field_0x4c;
                pDrone->actor->t.t.translate.t.v[2] = DRONE_SEC(pDrone)->field_0x10.v[2] * pDrone->field_0x4c;
            }
            pDrone->actor->t.t.translate.t.v[0] += gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position.v[0];
            pDrone->actor->t.t.translate.t.v[1] += gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position.v[1];
            pDrone->actor->t.t.translate.t.v[2] += gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position.v[2];
            pDrone->field_0xe4 = pDrone->field_0x48;
            dt = dt - consumed;
            break;
        case 2:
            if (DRONE_F(pDrone, 0x68) > gDrone_render_zero) {
                done = 0;
                ang = dt * pDrone->field_0x48 / DRONE_F(pDrone, 0x68);
                if (pDrone->left_turn) {
                    if (ang + DRONE_F(pDrone, 0x60) >= DRONE_F(pDrone, 0x64)) {
                        done = 1;
                        l_time = (DRONE_F(pDrone, 0x64) - DRONE_F(pDrone, 0x60)) * DRONE_F(pDrone, 0x68) / pDrone->field_0x48;
                        DRONE_F(pDrone, 0x60) = DRONE_F(pDrone, 0x64);
                    } else {
                        l_time = gDrone_delta_time;
                        DRONE_F(pDrone, 0x60) = ang + DRONE_F(pDrone, 0x60);
                    }
                } else {
                    if (DRONE_F(pDrone, 0x60) - ang <= DRONE_F(pDrone, 0x64)) {
                        done = 1;
                        l_time = (DRONE_F(pDrone, 0x60) - DRONE_F(pDrone, 0x64)) * DRONE_F(pDrone, 0x68) / pDrone->field_0x48;
                        DRONE_F(pDrone, 0x60) = DRONE_F(pDrone, 0x64);
                    } else {
                        l_time = gDrone_delta_time;
                        DRONE_F(pDrone, 0x60) = DRONE_F(pDrone, 0x60) - ang;
                    }
                }
                vec.v[1] = 0.f;
                vec.v[0] = cos(DRONE_F(pDrone, 0x60)) * DRONE_F(pDrone, 0x68) + DRONE_F(pDrone, 0x54);
                vec.v[2] = sin(DRONE_F(pDrone, 0x60)) * DRONE_F(pDrone, 0x68) + DRONE_F(pDrone, 0x58);
                BrMatrix34ApplyV(&pDrone->actor->t.t.translate.t, &vec, (br_matrix34*)((char*)pDrone + 0xa8));
                pDrone->actor->t.t.translate.t.v[0] += gDrone_path_nodes[pDrone->field_0xc].position.v[0];
                pDrone->actor->t.t.translate.t.v[1] += gDrone_path_nodes[pDrone->field_0xc].position.v[1];
                pDrone->actor->t.t.translate.t.v[2] += gDrone_path_nodes[pDrone->field_0xc].position.v[2];
                t = (DRONE_F(pDrone, 0x60) - DRONE_F(pDrone, 0x5c)) / (DRONE_F(pDrone, 0x64) - DRONE_F(pDrone, 0x5c));
                pDrone->field_0xe0 = t;
                pDrone->field_0x74 = t;
                BrVector3Scale(&va, &DRONE_SEC2(pDrone)->field_0x10, t);
                BrVector3Scale(&vb, &DRONE_SEC(pDrone)->field_0x10, 1.f - t);
                BrVector3Accumulate(&va, &vb);
                PointActorAlongThisBloodyVector(pDrone->actor, &va);
                dt = dt - l_time;
                if (done) {
                    pDrone->field_0x4c = DRONE_F(pDrone, 0x6c);
                    pDrone->field_0xdc = 0;
                    pDrone->field_0xe4 = pDrone->field_0x48;
                    continue;
                }
            } else {
                pDrone->field_0x4c = 0.f;
                pDrone->field_0xdc = 0;
            }
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        default:
            break;
        }
    } while (dt > gDrone_render_zero);
}

// PARTIAL 85.87%: only diff is the four `-1` stores: original keeps -1 in ebx (`or ebx,0xffffffff` in
// the prologue, `mov [esi+0xN],bx`) while this body gets 16-bit immediates; no C form tried reproduced the hoist.
// based on REA disassembly 0x44e410 and (GPL-3.0)
// FUNCTION: CARMA2_HW 0x0044e410
int C2_HOOK_FASTCALL AdvanceDroneSection(tDrone_spec* pDrone) {
    tDrone_path_node_section* section;
    int pick;

    if (pDrone->field_0xc < 0) {
        if (gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].count_sections != 0) {
            pick = IRandomBetween(0, MAX(0, gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].count_sections - 1));
            pDrone->field_0x10 = (undefined4)&gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].sections[pick];
        } else {
            pDrone->field_0x14 = 0;
            pDrone->field_0x10 = 0;
            pDrone->field_0xc = -1;
            pDrone->field_0xa_pathnode_id = -1;
        }
    } else {
        pDrone->field_0xa_pathnode_id = pDrone->field_0xc;
        pDrone->field_0x10 = pDrone->field_0x14;
    }

    section = DRONE_SEC(pDrone);
    if (section != NULL) {
        pDrone->field_0xc = section->node1;
    } else {
        pDrone->field_0xc = -1;
    }
    if (pDrone->field_0xc < 0) {
        return 0;
    }

    if (gDrone_path_nodes[pDrone->field_0xc].count_sections != 0) {
        pick = IRandomBetween(0, MAX(0, gDrone_path_nodes[pDrone->field_0xc].count_sections - 1));
        section = &gDrone_path_nodes[pDrone->field_0xc].sections[pick];
        pDrone->field_0x14 = (undefined4)section;
        pDrone->field_0xe = section->node1;
        return 1;
    }
    pDrone->field_0x14 = 0;
    pDrone->field_0xe = -1;
    return 1;
}

#define DRONE_SF(p, off) (*(float*)((char*)(p) + (off)))
#define DRONE_SP(p, off) (*(char**)((char*)(p) + (off)))

// based on REA pseudocode 0x44e540 and // PARTIAL 59.92%: frame 0x40 vs 0x3c; corner compare uses fcompp not fcom; 0x60=0x64 copy via fld
// FUNCTION: CARMA2_HW 0x0044e540
void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) {
    float dt;
    float step;
    float remaining;
    float new_dist;
    float brake;
    float decel;
    float t;
    int finished;
    tDrone_form* form;
    char* sect;
    char* secremaining;
    br_vector3 v;
    br_vector3 v2;
    br_vector3 dir;

    DoNotDprintf_drone("MoveThisDroneCar()");
    dt = gDrone_delta_time;
    if (dt <= gDrone_render_zero) {
        return;
    }
    do {
        switch (pDrone->field_0xdc) {
        case 0:
            DoNotDprintf_drone("eDDS_straight_start");
            if (!AdvanceDroneSection(pDrone)) {
                DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 4);
                if (pDrone->current_state == 4) {
                    return;
                }
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
                return;
            }
            CalculateDrivingInfo(pDrone);
            sect = DRONE_SP(pDrone, 0x10);
            dir.v[0] = DRONE_SF(sect, 0x4);
            dir.v[1] = DRONE_SF(sect, 0x8);
            dir.v[2] = DRONE_SF(sect, 0xc);
            if (pDrone->form->flags & 0x10) {
                dir.v[1] = 0.0f;
            }
            PointActorAlongThisBloodyVector(pDrone->actor, &dir);
            pDrone->field_0xdc = 1;
            break;
        case 1:
            form = pDrone->form;
            if (DRONE_SF(form, 0x18) >= gDrone_render_zero) {
                pDrone->field_0x48 = DRONE_SF(form, 0x18);
                pDrone->field_0x50 = DRONE_SF(form, 0x18);
            }
            sect = DRONE_SP(pDrone, 0x10);
            remaining = DRONE_SF(sect, 0x1c) - DRONE_F(pDrone, 0x6c);
            new_dist = dt * pDrone->field_0x48 + pDrone->field_0x4c;
            if (pDrone->field_0x4c < remaining) {
                step = dt;
                if (new_dist <= remaining) {
                    if (DRONE_SF(form, 0x18) < gDrone_render_zero) {
                        decel = pDrone->field_0x48 * pDrone->field_0x48 - pDrone->field_0x50 * pDrone->field_0x50;
                        brake = remaining - decel * 0.5f;
                        if (pDrone->field_0x48 <= pDrone->field_0x50 || pDrone->field_0x4c < brake) {
                            if ((pDrone->field_0x48 <= pDrone->field_0x50 || new_dist <= brake)
                                && (pDrone->field_0x48 <= pDrone->field_0x50
                                    || dt * pDrone->field_0x48 + DRONE_SF(form, 0x24) * dt * dt * 0.5f + pDrone->field_0x4c <= brake)) {
                                pDrone->field_0x48 = DRONE_SF(form, 0x24) * dt + pDrone->field_0x48;
                                step = gDrone_delta_time;
                                if (DRONE_SF(form, 0x20) < pDrone->field_0x48) {
                                    pDrone->field_0x48 = DRONE_SF(form, 0x20);
                                }
                            }
                        } else {
                            t = -(decel / ((remaining - pDrone->field_0x4c) + (remaining - pDrone->field_0x4c)));
                            step = (pDrone->field_0x48 - pDrone->field_0x50) / -t;
                            if (dt < step) {
                                pDrone->field_0x48 = t * dt + pDrone->field_0x48;
                                step = dt;
                            } else {
                                pDrone->field_0x48 = pDrone->field_0x50;
                            }
                        }
                    }
                } else if (pDrone->field_0x50 < pDrone->field_0x48) {
                    t = pDrone->field_0x48;
                    pDrone->field_0x48 = pDrone->field_0x50;
                    step = t - pDrone->field_0x50;
                }
            } else {
                pDrone->field_0xdc = 2;
                step = gDrone_render_zero;
                if (pDrone->field_0x50 < pDrone->field_0x48) {
                    pDrone->field_0x48 = pDrone->field_0x50;
                }
            }
            if (pDrone->field_0xdc == 2) {
                pDrone->field_0x4c = DRONE_SF(sect, 0x1c) - DRONE_F(pDrone, 0x6c);
            } else {
                pDrone->field_0x4c = step * pDrone->field_0x48 + pDrone->field_0x4c;
            }
            pDrone->actor->t.t.translate.t.v[0] = DRONE_SF(sect, 0x10) * pDrone->field_0x4c;
            pDrone->actor->t.t.translate.t.v[1] = DRONE_SF(DRONE_SP(pDrone, 0x10), 0x14) * pDrone->field_0x4c;
            pDrone->actor->t.t.translate.t.v[2] = DRONE_SF(DRONE_SP(pDrone, 0x10), 0x18) * pDrone->field_0x4c;
            pDrone->actor->t.t.translate.t.v[0] = gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position.v[0] + pDrone->actor->t.t.translate.t.v[0];
            pDrone->actor->t.t.translate.t.v[1] = gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position.v[1] + pDrone->actor->t.t.translate.t.v[1];
            pDrone->actor->t.t.translate.t.v[2] = gDrone_path_nodes[(tS16)pDrone->field_0xa_pathnode_id].position.v[2] + pDrone->actor->t.t.translate.t.v[2];
            dt = dt - step;
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        case 2:
            if (DRONE_F(pDrone, 0x68) > gDrone_render_zero) {
                finished = 0;
                t = dt * pDrone->field_0x48 / DRONE_F(pDrone, 0x68);
                if (pDrone->left_turn) {
                    t = t + DRONE_F(pDrone, 0x60);
                    if (t >= DRONE_F(pDrone, 0x64)) {
                        finished = 1;
                        step = (DRONE_F(pDrone, 0x64) - DRONE_F(pDrone, 0x60)) * DRONE_F(pDrone, 0x68) / pDrone->field_0x48;
                        DRONE_F(pDrone, 0x60) = DRONE_F(pDrone, 0x64);
                    } else {
                        step = gDrone_delta_time;
                        DRONE_F(pDrone, 0x60) = t;
                    }
                } else {
                    t = DRONE_F(pDrone, 0x60) - t;
                    if (t <= DRONE_F(pDrone, 0x64)) {
                        finished = 1;
                        step = (DRONE_F(pDrone, 0x60) - DRONE_F(pDrone, 0x64)) * DRONE_F(pDrone, 0x68) / pDrone->field_0x48;
                        DRONE_F(pDrone, 0x60) = DRONE_F(pDrone, 0x64);
                    } else {
                        step = gDrone_delta_time;
                        DRONE_F(pDrone, 0x60) = t;
                    }
                }
                v.v[1] = 0.0f;
                v.v[0] = cos(DRONE_F(pDrone, 0x60)) * DRONE_F(pDrone, 0x68) + DRONE_F(pDrone, 0x54);
                v.v[2] = sin(DRONE_F(pDrone, 0x60)) * DRONE_F(pDrone, 0x68) + DRONE_F(pDrone, 0x58);
                BrMatrix34ApplyV(&pDrone->actor->t.t.translate.t, &v, (br_matrix34*)((char*)pDrone + 0xa8));
                pDrone->actor->t.t.translate.t.v[0] = gDrone_path_nodes[pDrone->field_0xc].position.v[0] + pDrone->actor->t.t.translate.t.v[0];
                pDrone->actor->t.t.translate.t.v[1] = gDrone_path_nodes[pDrone->field_0xc].position.v[1] + pDrone->actor->t.t.translate.t.v[1];
                pDrone->actor->t.t.translate.t.v[2] = gDrone_path_nodes[pDrone->field_0xc].position.v[2] + pDrone->actor->t.t.translate.t.v[2];
                remaining = (DRONE_F(pDrone, 0x60) - DRONE_F(pDrone, 0x5c)) / (DRONE_F(pDrone, 0x64) - DRONE_F(pDrone, 0x5c));
                pDrone->field_0xe0 = remaining;
                pDrone->field_0x74 = remaining;
                sect = DRONE_SP(pDrone, 0x10);
                secremaining = DRONE_SP(pDrone, 0x14);
                dir.v[0] = DRONE_SF(secremaining, 0x10) * remaining;
                dir.v[1] = DRONE_SF(secremaining, 0x14) * remaining;
                dir.v[2] = DRONE_SF(secremaining, 0x18) * remaining;
                v2.v[0] = DRONE_SF(sect, 0x10) * (1.0f - remaining);
                v2.v[1] = DRONE_SF(sect, 0x14) * (1.0f - remaining);
                v2.v[2] = DRONE_SF(sect, 0x18) * (1.0f - remaining);
                dir.v[0] = dir.v[0] + v2.v[0];
                dir.v[1] = dir.v[1] + v2.v[1];
                dir.v[2] = dir.v[2] + v2.v[2];
                if (pDrone->form->flags & 0x10) {
                    dir.v[1] = 0.0f;
                }
                PointActorAlongThisBloodyVector(pDrone->actor, &dir);
                dt = dt - step;
                if (finished) {
                    pDrone->field_0x4c = DRONE_F(pDrone, 0x6c);
                    pDrone->field_0xdc = 0;
                }
            } else {
                pDrone->field_0x4c = 0;
                pDrone->field_0xdc = 0;
            }
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        default:
            break;
        }
    } while (dt > gDrone_render_zero);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
