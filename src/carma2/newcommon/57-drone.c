/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "57-drone.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "23-piping.h"
#include "28-world3.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "56-aiworld.h"
#include "60-phil.h"
#include "c2_string.h"
#include "globvars.h"
#include "c2_stdio.h"
#include "platform.h"
#include "rec2_macros.h"
#include "30-opponent.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x006820d0
// GLOBAL: CARMA2_SW 0x0067c810
int gCount_drones;

// GLOBAL: CARMA2_HW 0x00682178
// GLOBAL: CARMA2_SW 0x0067c8b8
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x0068450c
// GLOBAL: CARMA2_SW 0x0067ec4c
int gCount_drone_forms;

// GLOBAL: CARMA2_HW 0x00684510
// GLOBAL: CARMA2_SW 0x0067ec50
int gCount_drone_path_nodes;

// GLOBAL: CARMA2_HW 0x006844f8
// GLOBAL: CARMA2_SW 0x0067ec38
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x00684504
// GLOBAL: CARMA2_SW 0x0067ec44
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x00684508
// GLOBAL: CARMA2_SW 0x0067ec48
tDrone_path_node* gDrone_path_nodes;

// GLOBAL: CARMA2_HW 0x00684518
// GLOBAL: CARMA2_SW 0x0067ec58
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00705b70
tU32 gINT_00705b70;

// GLOBAL: CARMA2_HW 0x00589dc4
float gDrone_render_zero = 0.0f;

// GLOBAL: CARMA2_HW 0x00681fb0
// GLOBAL: CARMA2_SW 0x0067c6f0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684514
// GLOBAL: CARMA2_SW 0x0067ec54
int gFrame;

// DoNotDprintf

// CrappyLittleVector3DPrintf

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

// FUNCTION: CARMA2_HW 0x0044f640
// FUNCTION: CARMA2_SW 0x0044f3e0
FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name) {
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

// LoadDroneTypeInfo

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone) {
    PHILRemoveObject(&pDrone->collision_info);
}

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone) {
    ReallyRemoveDroneFromPHIL(pDrone);
}

// InitDroneCollisionObject

// FUNCTION: CARMA2_HW 0x00451650
// FUNCTION: CARMA2_SW 0x004513e0
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
// FUNCTION: CARMA2_SW 0x00450390
int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData) {
    tLinkyCallback_context* context;
    int i;

    context = pData;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    pActor->render_style = BR_RSTYLE_DEFAULT;
    for (i = context->model_start; i < context->model_start + context->model_end; i++) {
        if (DRStricmp(gDroneStorage.models[i]->identifier, pActor->model->identifier) == 0) {
            pActor->model = gDroneStorage.models[i];
            return 0;
        }
    }
    PDFatalError("No drone model");
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
    DoNotDprintf("CalculateDrivingInfo called - centre node %d, radius %f, section after corner %p", pDrone->field_0xc, (double)DRONE_F(pDrone, 0x68), (void*)pDrone->field_0x10);
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

// InitDroneSpec

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// StopRenderingThisDrone

// StopProcessingThisDrone

// StartRenderingThisDrone

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

// FUNCTION: CARMA2_HW 0x0044cb80
// FUNCTION: CARMA2_SW 0x0044c930
int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 18.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 13.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 18.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cbd0
// FUNCTION: CARMA2_SW 0x0044c980
int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cc20
// FUNCTION: CARMA2_SW 0x0044c9d0
int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f) {
        return 1;
    } else {
        return 0;
    }
}

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

// FUNCTION: CARMA2_HW 0x0044cfd0
// FUNCTION: CARMA2_SW 0x0044cd80
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) {
    tCompressed_matrix34 compressed_mat;
    br_scalar radius;
    br_scalar speed;

    BrVector3Set(&compressed_mat.m[0],
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[1],
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[2],
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[3],
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][0], -1000.f, 1000.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][1], -1000.f, 1000.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][2], -1000.f, 1000.f));

    radius = MIN(pDrone->h_radius, 100.f);
    speed = pDrone->left_turn ? pDrone->field_0x48 : -pDrone->field_0x48;
    PipeSingleDroneCornerPos(pDrone, DRScalarToU16(speed, -500.f, 500.f), DRScalarToU16(pDrone->field_0x74, 0.f, 1.f), DRScalarToU16(radius, 0.f, 100.f), &compressed_mat);
}

// PossiblyPipeDroneMovement

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

// FUNCTION: CARMA2_HW 0x0044ecf0
// FUNCTION: CARMA2_SW 0x0044ea90
void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on) {

    gTraffic_disabled = !pTraffic_on;
}

// FUNCTION: CARMA2_HW 0x0044ed00
// FUNCTION: CARMA2_SW 0x0044eaa0
int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void) {
    return !gTraffic_disabled;
}

// STUB: CARMA2_HW 0x0044ed10
// STUB: CARMA2_SW 0x0044eab0
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00450b50
tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
// InitDrones

// FUNCTION: CARMA2_HW 0x0044fc10
// FUNCTION: CARMA2_SW 0x0044f9b0
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;

    if (gCount_drones == 0) {
        return;
    }

    for (i = 0; i < gCount_drones; i++) {

        if (gDrone_specs[i].actor->parent != NULL) {
            BrActorRemove(gDrone_specs[i].actor);
        }
        BrActorFree(gDrone_specs[i].actor);
        RemoveDroneFromPHIL(&gDrone_specs[i]);
        if (gDrone_specs[i].field_0x46) {
            FreeShape(gDrone_specs[i].collision_info.shape);
            gDrone_specs[i].field_0x46 = 0;
        }

        if (gDrone_specs[i].funk_grooves != NULL) {
            BrMemFree(gDrone_specs[i].funk_grooves);
            gDrone_specs[i].funk_grooves = NULL;
        }
    }

    for (i = 0; i < gCount_drone_forms; i++) {

        if (gDrone_forms[i].field_0x80 != NULL) {
            int j;

            BrMemFree(gDrone_forms[i].field_0x80);
            gDrone_forms[i].field_0x80 = NULL;
            for (j = 0; j < gDrone_forms[i].count_models; j++) {
                BrModelRemove(gDrone_forms[i].models[j]);
                gDrone_forms[i].models[j]->nfaces = 0;
                gDrone_forms[i].models[j]->faces = NULL;
                BrModelFree(gDrone_forms[i].models[j]);
            }
            if (gDrone_forms[i].models != NULL) {
                BrMemFree(gDrone_forms[i].models);
                gDrone_forms[i].models = NULL;
            }
        }
    }
    ClearOutStorageSpace(&gDroneStorage);
    BrMemFree(gDrone_specs);
    gDrone_specs = NULL;
    gCount_drones = 0;
    BrMemFree(gDrone_path_nodes);
    gDrone_path_nodes = NULL;
    gCount_drone_path_nodes = 0;
}

// STUB: CARMA2_HW 0x0044fda0
// STUB: CARMA2_SW 0x0044fb40
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {
    NOT_IMPLEMENTED();
}

// GetDroneCrushData

// AllocateAndInitDrones

// STUB: CARMA2_HW 0x00450bf0
// STUB: CARMA2_SW 0x00450980
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004512d0
// FUNCTION: CARMA2_SW 0x00451060
void C2_HOOK_FASTCALL DronePreCollisionStuff(void) {
    gINT_00681fb0 = 0;
    gFrame++;
}

// DoDronePerGameFrameStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00451e70
// STUB: CARMA2_SW 0x00451c00
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x00452230
int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00452360
int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00452410
void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004525c0
// FUNCTION: CARMA2_SW 0x00452350
void C2_HOOK_FASTCALL DroneLookWhereYoureGoingYouBlindCunt(void) {
}

// FUNCTION: CARMA2_HW 0x004525d0
// FUNCTION: CARMA2_SW 0x00452360
void C2_HOOK_FASTCALL SelectNextDrone(void) {
    char s[64];

    gCurrent_selected_drone++;
    if (gCurrent_selected_drone >= gCount_drones) {
        gCurrent_selected_drone = 0;
    }
    sprintf(s, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, s);
}

// FUNCTION: CARMA2_HW 0x00452620
// FUNCTION: CARMA2_SW 0x004523b0
void C2_HOOK_FASTCALL SelectPreviousDrone(void) {
    char s[64];

    if (gCount_drones != 0) {
        gCurrent_selected_drone--;
        if (gCurrent_selected_drone < 0) {
            gCurrent_selected_drone = gCount_drones - 1;
        }
    }
    sprintf(s, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, s);
}

// FUNCTION: CARMA2_HW 0x00452680
// FUNCTION: CARMA2_SW 0x00452410
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452690
// FUNCTION: CARMA2_SW 0x00452420
br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {
    return &gDrone_specs[gCurrent_selected_drone].actor->t.t.mat;
}

// GetCurrentViewDroneDirection

// ApplyDroneRender2

// FUNCTION: CARMA2_HW 0x00452880
// FUNCTION: CARMA2_SW 0x00452610
void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int pDrone_id, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance) {
    br_vector3 direction;
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_id];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.0f, 500.0f);
    drone->field_0xe0 = 0.0f;
    BrVector3Scale(&direction, &pSection->field_0x10, pSection->field_0x1c - DRU16ToScalar(pDistance, 0.0f, 255.0f));
    BrVector3Negate(&direction, &direction);
    BrVector3Add(&drone->actor->t.t.translate.t, &gDrone_path_nodes[pSection->node1].position, &direction);
    BrVector3Copy(&direction, &pSection->field_0x10);
    if (drone->form->flags & 0x10) {
        direction.v[1] = 0.0f;
    }
    PointActorAlongThisBloodyVector(drone->actor, &direction);
    DoDroneFunkyGroovyThings(drone);
}

// FUNCTION: CARMA2_HW 0x004529d0
// FUNCTION: CARMA2_SW 0x00452760
void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int pDrone_id, tU16 pSpeed, tU16 pField_0x74, tU16 pRadius, const tCompressed_matrix34* pMatrix) {
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_id];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.0f, 500.0f);
    drone->field_0xe0 = DRU16ToScalar(pField_0x74, 0.0f, 1.0f);
    drone->field_0xe8 = DRU16ToScalar(pRadius, 0.0f, 100.0f);
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[0],
        DRU16ToScalar(pMatrix->m[0].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[0].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[0].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[1],
        DRU16ToScalar(pMatrix->m[1].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[1].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[1].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[2],
        DRU16ToScalar(pMatrix->m[2].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[2].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[2].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[3],
        DRU16ToScalar(pMatrix->m[3].v[0], -1000.0f, 1000.0f),
        DRU16ToScalar(pMatrix->m[3].v[1], -1000.0f, 1000.0f),
        DRU16ToScalar(pMatrix->m[3].v[2], -1000.0f, 1000.0f));
    DoDroneFunkyGroovyThings(drone);
}

// FUNCTION: CARMA2_HW 0x00452f40
// FUNCTION: CARMA2_SW 0x00452cd0
void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pCount_faces, int pCount_vertices) {
    int i;
    br_vertex* vertices;
    br_face* faces;

    vertices = BrResAllocate(pModel, pCount_vertices * sizeof(br_vertex), BR_MEMORY_VERTICES);
    BrBlockFill(vertices, 0, pCount_vertices * sizeof(br_vertex) / sizeof(br_uint_32));
    if (pModel->nvertices != 0) {
        for (i = 0; i < (pModel->nvertices < pCount_vertices ? pModel->nvertices : pCount_vertices); i++) {
            vertices[i] = pModel->vertices[i];
        }
        BrResRemove(pModel->vertices);
        BrResFree(pModel->vertices);
    }
    pModel->nvertices = pCount_vertices;
    pModel->vertices = vertices;
    faces = BrResAllocate(pModel, pCount_faces * sizeof(br_face), BR_MEMORY_FACES);
    BrBlockFill(faces, 0, pCount_faces * sizeof(br_face) / sizeof(br_uint_32));
    if (pModel->nfaces != 0) {
        for (i = 0; i < (pModel->nfaces < pCount_faces ? pModel->nfaces : pCount_faces); i++) {
            faces[i] = pModel->faces[i];
        }
        BrResRemove(pModel->faces);
        BrResFree(pModel->faces);
    }
    pModel->nfaces = pCount_faces;
    pModel->faces = faces;
}

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

