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
extern void C2_HOOK_FASTCALL CalculateDrivingInfo(tDrone_spec* pDrone);
extern tDrone_state_function* gDrone_state_functions[6];

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

#define DRONE_F(p, off) (*(float*)((char*)(p) + (off)))

#define DRONE_SEC(p) ((tDrone_path_node_section*)(p)->field_0x10)

#define DRONE_SEC2(p) ((tDrone_path_node_section*)(p)->field_0x14)

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

    DoNotDprintf("MoveThisDronePlane()");
    dt = gDrone_delta_time;
    if (dt <= gDrone_render_zero) {
        return;
    }
    do {
        switch (pDrone->field_0xdc) {
        case 0:
            DoNotDprintf("eDDS_straight_start");
            if (!AdvanceDroneSection(pDrone)) {
                DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 4);
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
                DoNotDprintf("eDDS_straight_driving: TAKEOFF/INFLIGHT %f %f", target_d, (double)pDrone->field_0x48);
            } else if (DRONE_SEC(pDrone)->type <= 5) {
                target = pDrone->form->max_speed * 0.66f;
                target_d = target;
                DoNotDprintf("eDDS_straight_driving: APPROACH %f %f", target_d, (double)pDrone->field_0x48);
            } else if (DRONE_SEC(pDrone)->type == 6) {
                target = pDrone->form->min_speed;
                target_d = target;
                DoNotDprintf("eDDS_straight_driving: TAXI/LANDING %f %f", target_d, (double)pDrone->field_0x48);
            } else {
                target = pDrone->form->max_speed;
                target_d = target;
                DoNotDprintf("eDDS_straight_driving: TAKEOFF/INFLIGHT %f %f", target_d, (double)pDrone->field_0x48);
            }
            rem = DRONE_SEC(pDrone)->field_0x1c - DRONE_F(pDrone, 0x6c);
            if (pDrone->field_0x4c < rem) {
                if (sum > rem) {
                    if (pDrone->field_0x48 <= target) {
                        DoNotDprintf("eDDS_straight_driving: CLAUSE 2b %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = dt;
                    } else {
                        DoNotDprintf("eDDS_straight_driving: CLAUSE 2a %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = (pDrone->field_0x48 - target) / pDrone->form->max_acceleration;
                        pDrone->field_0x48 = target;
                    }
                } else {
                    decel = rem - (pDrone->field_0x48 * pDrone->field_0x48 - target * target) * 0.5f;
                    DoNotDprintf("eDDS_straight_driving: decel_envelope %f %f %f", (double)decel, target_d, (double)pDrone->field_0x48);
                    if (DRONE_SEC(pDrone)->type != 2 && DRONE_SEC(pDrone)->type != 3 && pDrone->field_0x48 > target && pDrone->field_0x4c >= decel) {
                        DoNotDprintf("eDDS_straight_driving: CLAUSE 4 %f %f", target_d, (double)pDrone->field_0x48);
                        accel = -((pDrone->field_0x48 * pDrone->field_0x48 - target * target) / (((DRONE_SEC(pDrone)->field_0x1c - DRONE_F(pDrone, 0x6c)) - pDrone->field_0x4c) * 2.f));
                        time_to = (pDrone->field_0x48 - target) / -accel;
                        DoNotDprintf("eDDS_straight_driving: a %f, time_to_hit_max_corner_speed %f %f %f", (double)accel, (double)time_to, target_d, (double)pDrone->field_0x48);
                        if (time_to <= dt) {
                            DoNotDprintf("eDDS_straight_driving: CLAUSE 4a %f %f", target_d, (double)pDrone->field_0x48);
                            pDrone->field_0x48 = target;
                            consumed = time_to;
                        } else {
                            DoNotDprintf("eDDS_straight_driving: CLAUSE 4b %f %f", target_d, (double)pDrone->field_0x48);
                            pDrone->field_0x48 = accel * dt + pDrone->field_0x48;
                            consumed = dt;
                        }
                    } else if (DRONE_SEC(pDrone)->type != 2 && DRONE_SEC(pDrone)->type != 3 && pDrone->field_0x48 > target && sum > decel) {
                        DoNotDprintf("eDDS_straight_driving: CLAUSE 5 %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = dt;
                    } else if (DRONE_SEC(pDrone)->type != 2 && DRONE_SEC(pDrone)->type != 3 && pDrone->field_0x48 > target && pDrone->field_0x48 * dt + dt * dt * pDrone->form->max_acceleration * 0.5f + pDrone->field_0x4c > decel) {
                        DoNotDprintf("eDDS_straight_driving: CLAUSE 6 %f %f", target_d, (double)pDrone->field_0x48);
                        consumed = dt;
                    } else {
                        DoNotDprintf("eDDS_straight_driving: CLAUSE 7 %f %f", target_d, (double)pDrone->field_0x48);
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

