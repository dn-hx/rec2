/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "49-physics.h"

#include "00-car.h"
#include "09-funks.h"
#include "16-graphics1.h"
#include "22-replay.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "52-errors.h"
#include "65-hull.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_math.h"
#include "c2_string.h"
#include "c2_stdlib.h"
#include "13-crush1.h"
#include "27-powerup.h"
#include "13-crush1.h"

#include <stddef.h>

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

// GLOBAL: CARMA2_HW 0x00692dc8
// GLOBAL: CARMA2_SW 0x0068ce20
int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x00692dd8
int gINT_00692dd8;

// GLOBAL: CARMA2_HW 0x00692ddc
int gINT_00692ddc;

// GLOBAL: CARMA2_HW 0x00692de0
tPhysics_joint_force gCollision_contacts[50];

// GLOBAL: CARMA2_HW 0x006940a4
int gINT_006940a4;

// GLOBAL: CARMA2_HW 0x006940c0
// GLOBAL: CARMA2_SW 0x0068e118
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
// GLOBAL: CARMA2_SW 0x0073b2d8
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
// GLOBAL: CARMA2_SW 0x0073b2d4
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074480c
// GLOBAL: CARMA2_SW 0x0073b2cc
int gTotal_count_smash_peds;

// GLOBAL: CARMA2_HW 0x00744810
// GLOBAL: CARMA2_SW 0x0073b2d0
int gImmortal_peds;

// GLOBAL: CARMA2_HW 0x00744804
// GLOBAL: CARMA2_SW 0x0073b2c4
int gBOOL_00744804;

// GLOBAL: CARMA2_HW 0x00744800
// GLOBAL: CARMA2_SW 0x0073b2c0
float gPed_head_scale;

// GLOBAL: CARMA2_HW 0x00744808
// GLOBAL: CARMA2_SW 0x0073b2c8
tPedestrian* gPedestrian_array;

// GLOBAL: CARMA2_HW 0x0074a5e0
// GLOBAL: CARMA2_SW 0x007410a0
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e4
// GLOBAL: CARMA2_SW 0x007410a4
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x0074a5e8
// GLOBAL: CARMA2_SW 0x007410a8
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x0079d858
tFace_ref* gPling_face;

br_scalar gFLOAT_0058a768;

// GLOBAL: CARMA2_HW 0x006940c8
// GLOBAL: CARMA2_SW 0x0068e120
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x00694140
// GLOBAL: CARMA2_SW 0x0068e198
tProximity_ray gProximity_rays[20];

// GLOBAL: CARMA2_HW 0x00694274
// GLOBAL: CARMA2_SW 0x0068e2cc
tPed_giblet_size_spec* gGiblet_sizes;

// GLOBAL: CARMA2_HW 0x00694278
// GLOBAL: CARMA2_SW 0x0068e2d0
br_model* gPed_spurt_model;

// GLOBAL: CARMA2_HW 0x0069427c
// GLOBAL: CARMA2_SW 0x0068e2d4
int gPed_spurty_lumps_count;

// GLOBAL: CARMA2_HW 0x00694268
// GLOBAL: CARMA2_SW 0x0068e2c0
int gPed_times[3];

// GLOBAL: CARMA2_HW 0x00694280
// GLOBAL: CARMA2_SW 0x0068e2d8
br_actor* gPed_limbed_actors[30];

// GLOBAL: CARMA2_HW 0x006942f8
// GLOBAL: CARMA2_SW 0x0068e350
tShrapnel_time_info gPed_spurt_min_max_time;

// GLOBAL: CARMA2_HW 0x006940c4
// GLOBAL: CARMA2_SW 0x0068e11c
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006940bc
// GLOBAL: CARMA2_SW 0x0068e114
void* gPhysics_scratch_ptr;

// GLOBAL: CARMA2_HW 0x006940b0
// GLOBAL: CARMA2_SW 0x0068e108
int gFace_count;

// GLOBAL: CARMA2_HW 0x006940b8
int gINT_006940b8;

double gDOUBLE_0058a798 = 0.01;

// GLOBAL: CARMA2_HW 0x005964c4
int gActorBoxPick_StopGroovidelics;

// GLOBAL: CARMA2_HW 0x0065cf78
// GLOBAL: CARMA2_SW 0x006578b0
tWorld_callbacks gWorld_callbacks = {
    ProcessForcesCallback,
    ProcessJointForcesCallBack,
    NewFaceListCallBack,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065d014
int gINT_0065d014;

// GLOBAL: CARMA2_HW 0x00679434
int gINT_00679434;

// GLOBAL: CARMA2_HW 0x006923d8
tPhysics_object* gResetMan_object;

// count

// STUB: CARMA2_HW 0x00422030
int C2_HOOK_FASTCALL GetPointSample(br_model* pModel, int pCount_vertices, int* pVertex_indices) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00422400
tPhysicsError C2_HOOK_FASTCALL SampledConvexHull(tCollision_shape_polyhedron_data* pPolyhedron, int* pVertex_indices, br_model* pModel, int pCount_vertices) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004225b0
// FUNCTION: CARMA2_SW 0x00422630
void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix) {
    br_vector3 relative;

    BrVector3Sub(&relative, source, (br_vector3*)matrix->m[3]);
    BrMatrix34TApplyV(dest, &relative, matrix);
}

// STUB: CARMA2_HW 0x00422fe0
int C2_HOOK_FASTCALL AddObjectWallForce(float* pWalls, const br_vector3* pPoint, float pDistance, const br_vector3* pNormal, tU32 pUnused, int pCount, tPhysics_object* pObject, float pT, float pRadius, const float* pVector) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004268a0
// FUNCTION: CARMA2_SW 0x00426940
int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue) {
    int i;
    int j;
    tPhysics_joint_force* force;

    (void)pUnused;
    if (pCount < 4) {
        pForces[pCount].field_0x0 = 2.f;
        pCount++;
    }
    for (i = 0, force = pForces; i < pCount; i++, force++) {
        if (pT < force->field_0x0) {
            for (j = MIN(3, pCount); j >= i; j--) {
                pForces[j] = pForces[j - 1];
            }
            force->field_0x10 = pValue;
            force->object_a = pObject_a;
            force->object_b = pObject_b;
            BrVector3Sub(&force->point_a, pPoint_a, &pObject_a->cmpos);
            BrVector3Sub(&force->point_b, pPoint_b, &pObject_b->cmpos);
            BrMatrix34TApplyV(&force->direction_a, pNormal, &pObject_a->transform_matrix);
            BrMatrix34TApplyV(&force->direction_b, pNormal, &pObject_b->transform_matrix);
            force->field_0x0 = pT;
            force->type = pType;
            force->field_0xc = NULL;
            return pCount;
        }
    }
    return pCount;
}

// STUB: CARMA2_HW 0x004269c0
int C2_HOOK_FASTCALL GetSphereNonSphereForceInfo(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, br_vector3* pVertices1, br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00427870
int C2_HOOK_FASTCALL GetNonSphereNonSphereForceInfo(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00427fd0
int C2_HOOK_FASTCALL FUN_00427fd0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining, const tU8* pFlags, int* pFound, br_vector3* pDirection) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004225f0
// FUNCTION: CARMA2_SW 0x00422670
tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount) {
    br_vector3 normal;
    br_vector3 collision_position;
    br_scalar t;
    tFace_ref* best;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFace_ref, flags, 0x40);

    *pT = 2.f;
    for (i = 0; i < pCount; i++, pFaces++) {
        if ((pFaces->flags & 0x80) == 0) {
            CheckSingleFace(pFaces, pPosition, pDirection, &normal, &t, &collision_position);
            if (t < *pT) {
                *pT = t;
                BrVector3Copy(pNormal, &normal);
                best = pFaces;
                BrVector3Copy(pCollision_position, &collision_position);
            }
        }
    }
    if (*pT < 2.f) {
        return best;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004226d0
// FUNCTION: CARMA2_SW 0x00422750
tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount) {
    br_vector3 normal;
    br_vector3 collision_position;
    br_scalar t;
    tFace_ref* best;
    int i;

    *pT = 2.0f;
    for (i = 0; i < pCount; i++, pFaces++) {
        CheckSingleFace(pFaces, pPosition, pDirection, &normal, &t, &collision_position);
        if (t < *pT) {
            *pT = t;
            BrVector3Copy(pNormal, &normal);
            best = pFaces;
            BrVector3Copy(pCollision_position, &collision_position);
        }
    }
    if (*pT < 2.0f) {
        return best;
    } else {
        return NULL;
    }
}

// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

typedef struct tFace_ref_flag_bits {
    unsigned int other : 7;
    unsigned int eliminated : 1;
} tFace_ref_flag_bits;

// FUNCTION: CARMA2_HW 0x00423320
int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, int* pCtx, br_matrix34* pMatrix) {
    tCollision_shape_common* shape;
    br_matrix34* mat;
    br_bounds3* bnds;
    tFace_ref* f;
    int i;
    int j;
    int count;
    br_vector3 pos;
    br_vector3 edges[3];
    float d;
    float max;
    float min;
    float lo;
    float hi;

    shape = (tCollision_shape_common*)pCtx;
    mat = &pObject->actor->t.t.mat;
    bnds = &shape->field_0x1c;
    i = 0;
    count = 0;
    GetNewBoundingBox(bnds, &shape->bb, pMatrix);
    for (j = 0; j < 3; j++) {
        if (bnds->min.v[j] > shape->bb.min.v[j]) {
            bnds->min.v[j] = shape->bb.min.v[j];
        }
        if (bnds->max.v[j] < shape->bb.max.v[j]) {
            bnds->max.v[j] = shape->bb.max.v[j];
        }
    }
    BrMatrix34ApplyV(&pos, &bnds->min, mat);
    pos.v[0] += mat->m[3][0];
    pos.v[1] += mat->m[3][1];
    pos.v[2] += mat->m[3][2];
    for (j = 0; j < 3; j++) {
        d = bnds->max.v[j] - bnds->min.v[j];
        edges[j].v[0] = mat->m[j][0] * d;
        edges[j].v[1] = mat->m[j][1] * d;
        edges[j].v[2] = mat->m[j][2] * d;
    }
    f = &gFace_list__car[pObject->box_face_start];
    while (i < pObject->box_face_end - pObject->box_face_start) {
        max = (pos.v[0] - f->v[0].v[0]) * f->normal.v[0] + (pos.v[2] - f->v[0].v[2]) * f->normal.v[2] + (pos.v[1] - f->v[0].v[1]) * f->normal.v[1];
        min = max;
        for (j = 0; j < 3; j++) {
            d = edges[j].v[0] * f->normal.v[0] + edges[j].v[1] * f->normal.v[1] + edges[j].v[2] * f->normal.v[2];
            if (d < 0.f) {
                min += d;
            } else {
                max += d;
            }
        }
        if ((max > 0.005f && min > 0.005f) || (max < -0.005f && min < -0.005f) || (f->material != NULL && f->material->identifier[0] == '!')) {
            ((tFace_ref_flag_bits*)&f->flags)->eliminated = 1;
        } else if (f->material->identifier[0] == '>') {
            ((tFace_ref_flag_bits*)&f->flags)->eliminated = 1;
        } else {
            for (j = 0; j < 3; j++) {
                d = mat->m[j][2] * mat->m[3][2] + mat->m[j][0] * mat->m[3][0] + mat->m[j][1] * mat->m[3][1];
                max = mat->m[j][2] * f->v[0].v[2] + mat->m[j][0] * f->v[0].v[0] + mat->m[j][1] * f->v[0].v[1];
                lo = bnds->min.v[j] - 0.005f + d;
                if (max < lo) {
                    if (f->v[1].v[1] * mat->m[j][1] + f->v[1].v[0] * mat->m[j][0] + f->v[1].v[2] * mat->m[j][2] < lo
                        && f->v[2].v[1] * mat->m[j][1] + f->v[2].v[0] * mat->m[j][0] + f->v[2].v[2] * mat->m[j][2] < lo) {
                        ((tFace_ref_flag_bits*)&f->flags)->eliminated = 1;
                    }
                } else {
                    hi = bnds->max.v[j] + d - -0.005f;
                    if (max > hi
                        && f->v[1].v[1] * mat->m[j][1] + f->v[1].v[0] * mat->m[j][0] + f->v[1].v[2] * mat->m[j][2] > hi
                        && f->v[2].v[1] * mat->m[j][1] + f->v[2].v[0] * mat->m[j][0] + f->v[2].v[2] * mat->m[j][2] > hi) {
                        ((tFace_ref_flag_bits*)&f->flags)->eliminated = 1;
                    }
                }
            }
            ((tFace_ref_flag_bits*)&f->flags)->eliminated = 0;
            count++;
        }
        i++;
        f++;
    }
    return count;
}

// GetShapePoint

// GetObjectObjectEdgeIntersections

// STUB: CARMA2_HW 0x004236a0
int C2_HOOK_FASTCALL GetFaceEdgeIntersection(tPhysics_object* pObject, tPhysics_shape* pShape, tSensible_place_contact* pContacts, int pCount, int pMax, br_vector3* pPoints, br_matrix34* pScratch, int* pHas_collision) {
    NOT_IMPLEMENTED();
}

// GetSphereToFacePoints

// CheckSingleEdgeWithSphere

// STUB: CARMA2_HW 0x00423c00
int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, int pEdx, int pParam1, int pParam2, int pParam3, int pParam4, int pParam5, int pParam6) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00423ce0
int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdgeStart, br_vector3* pEdgeEnd, tPhysics_object* pObject, float* pSphere, float* pWalls, int pCount, br_vector3* pStart, br_vector3* pEnd, tFace_ref* pFace) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00423ed0
int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, int pEdx, tU32 pParam1, int pParam2, int pParam3, char* pParam4, tU32 pParam5) {
    NOT_IMPLEMENTED();
}

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// STUB: CARMA2_HW 0x00424270
int C2_HOOK_FASTCALL CheckSingleEdgeWithObjectEdges(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00424990
int C2_HOOK_FASTCALL CheckTwoEdges(br_vector3* pPoint, br_vector3* pDiff, const br_vector3* pVertices, br_vector3* pDelta, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, void* pUnused, tPhysics_object* pObject1, tPhysics_object* pObject2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00425630
int C2_HOOK_FASTCALL FUN_00425630(br_vector3* pCross1, br_vector3* pCross2, br_vector3* pPoint, br_vector3* pDiff, const br_vector3* pVertices, br_vector3* pDelta, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, void* pUnused, tPhysics_object* pObject1, tPhysics_object* pObject2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00424140
int C2_HOOK_FASTCALL GetEdgeToEdgePoints(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7) {
    NOT_IMPLEMENTED();
}

// SetUpWorkspace

// STUB: CARMA2_HW 0x004228b0
int C2_HOOK_FASTCALL GetObjectWallForceInfo(tPhysics_object* pObject, tSensible_place_contact* pContacts, int pArg) {
    NOT_IMPLEMENTED();
}

// GetObjectCornerToObjectFacePoints

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00425ba0
// STUB: CARMA2_SW 0x00425c40
int C2_HOOK_FASTCALL BoundsOverlapTest(br_bounds3* pBounds_1, br_bounds3* pBounds_2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00425ca0
int C2_HOOK_FASTCALL GetObjectObjectForceInfo(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_joint_force* pContacts, int pCount) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

// FUNCTION: CARMA2_HW 0x00428480
// FUNCTION: CARMA2_SW 0x00428510
void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        BrVector3Set(pPoint,
            (pIndex & 0x2) ? pShape->common.bb.min.v[0] : pShape->common.bb.max.v[0],
            (pIndex & 0x1) ? pShape->common.bb.min.v[1] : pShape->common.bb.max.v[1],
            (pIndex & 0x4) ? pShape->common.bb.min.v[2] : pShape->common.bb.max.v[2]);
        break;
    case kCollisionShapeType_Wireframe:
        BrVector3Copy(pPoint, &pShape->wireframe.wireframe.points[pIndex]);
        return;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        BrVector3Copy(pPoint, &pShape->polyhedron.polyhedron.points[pIndex]);
        return;
    default:
        return;
    }
}

// STUB: CARMA2_HW 0x00428530
int C2_HOOK_FASTCALL FUN_00428530(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices, int pCount, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining, tU8* pFlags) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004287a0
int C2_HOOK_FASTCALL FUN_004287a0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining, const tU8* pFlags1, const tU8* pFlags2) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00428cf0
// FUNCTION: CARMA2_SW 0x00428d80
int C2_HOOK_FASTCALL AddOldForces(tPhysics_joint_force* pForces, int pCount, const tPhysics_joint_force* pOld_forces, int pOld_count) {
    int i;

    for (i = 0; i < pOld_count && pCount < 40; i++) {
        if (pOld_forces[i].type != 9) {
            pForces[pCount] = pOld_forces[i];
            pCount++;
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x00428d40
// FUNCTION: CARMA2_SW 0x00428dd0
int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;

    shape1 = pCollision_1->shape;
    shape2 = pCollision_2->shape;
    if (FlagsAreGo(pCollision_1, pCollision_2)
        && BoundsOverlapTest(&pCollision_1->field_0xf4, &pCollision_2->field_0xf4)) {
        while (shape1 != NULL) {
            while (shape2 != NULL) {
                if (TestShapeEdgeIntersection(pCollision_1, pCollision_2, shape1, shape2)
                    || TestShapeEdgeIntersection(pCollision_2, pCollision_1, shape2, shape1)) {
                    return 1;
                }
                shape2 = shape2->common.next;
            }
            shape1 = shape1->common.next;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00428dc0
// FUNCTION: CARMA2_SW 0x00428e50
int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, tPhysics_shape* pCollision_shape_1, tPhysics_shape* pCollision_shape_2) {
    br_vector3 points[64];
    br_vector3 transformed;
    br_vector3 point;
    br_bounds bounds;
    br_scalar t;
    tPolyhedron_edge_indexes* edges;
    tPolyhedron_edge_indexes* edge;
    int count_edges;
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gDefault_box_collision_wireframe_edges) != 12);

    switch (pCollision_shape_1->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        for (i = 0; i < pCollision_shape_1->polyhedron.polyhedron.count_points; i++) {
            BrMatrix34ApplyP(&transformed, &pCollision_shape_1->polyhedron.polyhedron.points[i], &pCollision_info_1->transform_matrix);
            DrMatrix34ApplyLPInverse(&points[i], &transformed, &pCollision_info_2->transform_matrix);
        }
        edges = pCollision_shape_1->polyhedron.polyhedron.edges;
        count_edges = pCollision_shape_1->polyhedron.polyhedron.count_edges;
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        for (i = 0; i < 8; i++) {
            BrVector3Set(&points[i],
                (i & 0x2) ? pCollision_shape_1->common.bb.min.v[0] : pCollision_shape_1->common.bb.max.v[0],
                (i & 0x1) ? pCollision_shape_1->common.bb.min.v[1] : pCollision_shape_1->common.bb.max.v[1],
                (i & 0x4) ? pCollision_shape_1->common.bb.min.v[2] : pCollision_shape_1->common.bb.max.v[2]);
            BrMatrix34ApplyP(&transformed, &points[i], &pCollision_info_1->transform_matrix);
            DrMatrix34ApplyLPInverse(&points[i], &transformed, &pCollision_info_2->transform_matrix);
        }
        edges = gDefault_box_collision_wireframe_edges;
        count_edges = REC2_ASIZE(gDefault_box_collision_wireframe_edges);
        break;
    }
    switch (pCollision_shape_2->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        for (i = 0, edge = edges; i < count_edges; i++, edge++) {
            if (LinePolyColl(&points[edge->index1], &points[edge->index2], &pCollision_shape_2->polyhedron.polyhedron, &point, 0.005f, &t)) {
                return 1;
            }
        }
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        BrVector3Set(&bounds.min,
            pCollision_shape_2->common.bb.min.v[0] - 0.005f,
            pCollision_shape_2->common.bb.min.v[1] - 0.005f,
            pCollision_shape_2->common.bb.min.v[2] - 0.005f);
        BrVector3Set(&bounds.max,
            pCollision_shape_2->common.bb.max.v[0] + 0.005f,
            pCollision_shape_2->common.bb.max.v[1] + 0.005f,
            pCollision_shape_2->common.bb.max.v[2] + 0.005f);
        for (i = 0, edge = edges; i < count_edges; i++, edge++) {
            if (LineBoxColl(&points[edge->index1], &points[edge->index2], &bounds, &point, &t)) {
                return 1;
            }
        }
        break;
    }
    return 0;
}

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

// STUB: CARMA2_HW 0x00429070
// STUB: CARMA2_SW 0x00429100
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0045e200
void C2_HOOK_FASTCALL FUN_0045e200(float* pOffset, tCollision_shape_polyhedron_data* pPolyhedron, float* pParam1, float* pParam2, float* pParam3, float* pParam4, float* pParam5) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0045e4e0
void C2_HOOK_FASTCALL FUN_0045e4e0(float* pOffset, tCollision_shape_common* pShape, float* pParam1, float* pParam2, float* pParam3, float* pParam4, float* pParam5) {
    NOT_IMPLEMENTED();
}

// HierarchyCalcEnergy

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

// FUNCTION: CARMA2_HW 0x004b8c20
void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_a, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_b, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_a, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_b, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_a, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_b, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, type, 0x5c);

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    pForce->field_0xc = NULL;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, pDirection, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 10;
}

// STUB: CARMA2_HW 0x004b8cf0
void C2_HOOK_FASTCALL RedoQuickHingeVelocities(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004b8e10
void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, &pForce->direction_a, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 11;
}

// STUB: CARMA2_HW 0x004b8ee0
void C2_HOOK_FASTCALL TranslateAndRotateJointedObject(tPhysics_object* pObject, float pDelta_time) {
    NOT_IMPLEMENTED();
}

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

// FUNCTION: CARMA2_HW 0x004c1840
// FUNCTION: CARMA2_SW 0x004bdf70
void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject) {
    br_scalar ts = BrVector3LengthSquared(&pObject->omega) * REC2_SQR(0.04f);
    if (ts > (br_scalar)REC2_SQR(1.57)) {
        ts = (br_scalar)sqrt(ts) * (2.0f / 3.14f);
        pObject->omega.v[0] = pObject->omega.v[0] / (2.f * ts);
        pObject->omega.v[1] = pObject->omega.v[1] / (2.f * ts);
        pObject->omega.v[2] = pObject->omega.v[2] / (2.f * ts);
    }
}

// STUB: CARMA2_HW 0x004c18d0
void C2_HOOK_FASTCALL DoJointFriction(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c1be0
void C2_HOOK_FASTCALL RotateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pDelta_time) {
    NOT_IMPLEMENTED();
}

// ListLimitOmega

// GroupLimitOmega

// FUNCTION: CARMA2_HW 0x004b8fe0
// FUNCTION: CARMA2_SW 0x004b5700
void C2_HOOK_FASTCALL PositionWorldJointedObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 axis;
    br_vector3 tv;
    br_scalar ts;

    if (pObject->physics_joint2->type == eJoint_translation) {
        BrMatrix34ApplyV(&axis, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
        ts = BrVector3Dot(&pObject->actor->t.t.translate.t, &axis);
        BrMatrix34ApplyP(&tv, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        BrVector3Sub(&tv, &pObject->field_0x17c, &tv);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
        ts -= BrVector3Dot(&pObject->actor->t.t.translate.t, &axis);
        BrVector3Scale(&tv, &axis, ts);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
        ts = BrVector3Dot(&pObject->v, &axis);
        BrVector3Scale(&pObject->v, &axis, ts);
        TranslateObject(pObject, pDelta_time);
    } else {
        BrMatrix34ApplyP(&tv, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        BrVector3Sub(&tv, &pObject->field_0x17c, &tv);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
    }
}

// STUB: CARMA2_HW 0x004b9230
void C2_HOOK_FASTCALL TranslateAndRotatePartialObject(tPhysics_object* pObject, tPhysics_object** ppChanged_object, float pDelta_time) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b9650
int C2_HOOK_FASTCALL CountJointForces(tPhysics_object* pObject, tPhysics_object* pParent_context) {
    NOT_IMPLEMENTED();
}

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

// STUB: CARMA2_HW 0x004b7510
void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b77b0
void C2_HOOK_FASTCALL MakeJointVelocitiesVector(br_scalar* pVelocity, tPhysics_joint_force** pRecords, int pCount) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b7960
void C2_HOOK_FASTCALL ApplySingleForce(tPhysics_joint_force* pForce, br_scalar pImpulse) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b8350
int C2_HOOK_FASTCALL GetJointForces(tPhysics_object* pObject, tPhysics_joint_force* pForce) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b9750
// STUB: CARMA2_SW 0x004b5e70
void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pCollision_info) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b99e0
void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b9ce0
void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds3* pBox) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b9e40
void C2_HOOK_FASTCALL SetCollisionInfoDoNothingChain(tPhysics_object* pCollision_info, tU32 pDisable) {
    NOT_IMPLEMENTED();
}

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

// FUNCTION: CARMA2_HW 0x004c2a10
// FUNCTION: CARMA2_SW 0x004bf140
void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject) {
    tPhysics_object* child;
    tPhysics_object* root;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x230, 0x230);

    root = pObject;
    child = pObject->child;
    if (pObject->flags & 0x400) {
        root = pObject->field_0x230;
    }
    for (; child != NULL; child = child->next) {
        if (!(pObject->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pObject->actor->t.t.mat);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->actor->t.t.mat.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->field_0x1e8, &root->actor->t.t.mat);
        }
        WriteNewPositions(child);
    }
}

// FUNCTION: CARMA2_HW 0x004b9eb0
// FUNCTION: CARMA2_SW 0x004b65d0
void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    while (pObject != NULL) {
        pObject->disable_move_rotate = pFlag;
        pObject->field_0x1df = 0;
        if (pObject->child != NULL) {
            SetListDoingNothingFlag(pObject->child, pFlag);
        }
        pObject = pObject->next;
    }
}

// SetGroupDoingNothingFlag

// FUNCTION: CARMA2_HW 0x004b9ef0
// FUNCTION: CARMA2_SW 0x004b6610
void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    pObject->disable_move_rotate = pFlag;
    pObject->field_0x1df = 0;
    if (pObject->child != NULL) {
        SetListDoingNothingFlag(pObject->child, pFlag);
    }
}

// CheckDoingNothingFlag

// FUNCTION: CARMA2_HW 0x004bae80
// FUNCTION: CARMA2_SW 0x004b75a0
void C2_HOOK_FASTCALL SetGroup(tPhysics_object* pObject, int pGroup) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0x1dc = (tU8)pGroup;
    for (; child != NULL; child = child->next) {
        if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
            SetGroup(child, pGroup);
        }
    }
}

// STUB: CARMA2_HW 0x004baec0
int C2_HOOK_FASTCALL DoInterGroupCollisions(tPhysics_object** pObject_list, tPhysics_object* pObject, tPhysics_joint_force* pContacts, tPhysics_joint_force* pJoint_data, int pJoint_count, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bc190
void C2_HOOK_FASTCALL FUN_004bc190(tPhysics_joint_force** pRecords, br_scalar* pImpulses, int pCount, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bc8e0
void C2_HOOK_FASTCALL FUN_004bc8e0(tPhysics_object* pObject, float pScale) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bca10
int C2_HOOK_FASTCALL GetJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bd4f0
void C2_HOOK_FASTCALL GetUniversalRotationAngle(br_matrix34* pMatrix1, br_matrix34* pMatrix2, br_vector3* pChild1, br_vector3* pParent1, br_vector3* pChild2, br_vector3* pParent2, br_vector3* pOut1, br_vector3* pOut2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bd6d0
int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bd720
int C2_HOOK_FASTCALL ClampObjectSpin(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bd7f0
int C2_HOOK_FASTCALL ClampObjectSpinTree(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bd830
tPhysics_object** C2_HOOK_FASTCALL GetReducedList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pObjects) {
    NOT_IMPLEMENTED();
}

// GroupTogether

// GetPreCalculatedJointForces

// FUNCTION: CARMA2_HW 0x004b9f40
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->disable_move_rotate = 0;
            pObject->box_face_ref = gFace_num__car - 2;
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}

// STUB: CARMA2_HW 0x004ba190
void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original) {
    NOT_IMPLEMENTED();
}

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// FUNCTION: CARMA2_HW 0x004bd9b0
// FUNCTION: CARMA2_SW 0x004ba0d0
void C2_HOOK_FASTCALL SetObjectTrialPositionType(tPhysics_object* pObject, int pType) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0xf0 = pType;
    for (; child != NULL; child = child->next) {
        SetObjectTrialPositionType(child, pType);
    }
}

// STUB: CARMA2_HW 0x004bd9e0
int C2_HOOK_FASTCALL ReducedFindObjectCollisions(tPhysics_object* pObject, tPhysics_object** pOverlapping, tPhysics_joint_force* pContacts, int pCount, tWorld_callbacks* pWorld_callbacks, tPhysics_object* pList) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bddd0
void C2_HOOK_FASTCALL GroupTogether(tPhysics_object* pObject1, tPhysics_object* pObject2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004be0e0
void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004be350
void C2_HOOK_FASTCALL DRMatrix33Inverse(float* pDestination, const float* pSource) {
    NOT_IMPLEMENTED();
}

// FillInTrialWorkspace

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004ba4f0
// FUNCTION: CARMA2_SW 0x004b6c10
int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    int blocked;

    blocked = (pCollision_1->field_0x1a0 & pCollision_2->field_0x1a4)
        || (pCollision_2->field_0x1a0 & pCollision_1->field_0x1a4)
        || (pCollision_1->flags & 4)
        || (pCollision_2->flags & 4);
    return !blocked;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// FUNCTION: CARMA2_HW 0x004ba550
// FUNCTION: CARMA2_SW 0x004b6c70
int C2_HOOK_FASTCALL ReduceEffectOfForces(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount, const int* pIndices, int pIndex_count, br_scalar pThreshold) {
    int i;
    br_scalar total;

    total = 0.0f;
    for (i = 0; i < pIndex_count; i++) {
        total += pImpulses[pIndices[i]];
    }
    if (total <= pThreshold) {
        return 0;
    }
    pThreshold /= total;
    gINT_00692dc8 = 1;
    for (i = 0; i < pCount; i++) {
        pImpulses[i] = -pImpulses[i];
    }
    for (i = 0; i < pIndex_count; i++) {
        pImpulses[pIndices[i]] *= -pThreshold;
        if (pForces[pIndices[i]].type & 0x100) {
            BrVector3Scale(&pForces[pIndices[i]].field_0x44, &pForces[pIndices[i]].field_0x44,pThreshold);
            if (pForces[pIndices[i]].object_b != NULL) {
                BrVector3Scale(&pForces[pIndices[i]].field_0x44, &pForces[pIndices[i]].field_0x44, pThreshold);
            }
        }
    }
    for (i = 0; i < pCount; i++) {
        if (pImpulses[i] < 0.0f) {
            pImpulses[i] = 0.0f;
        }
        pForces[i].type = 9;
    }
    return 1;
    // }
    // return 0;
}

// DoCollisions

// AddDoubleTorqueToMatrix

// DRMatrix33Inverse

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

// FUNCTION: CARMA2_HW 0x004be4b0
// FUNCTION: CARMA2_SW 0x004babd0
void C2_HOOK_FASTCALL SetUpForce(br_vector3* pAngular, br_vector3* pLinear, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject) {
    br_scalar inverse_mass;
    int i;

    if (!(pObject->flags & 0x800)) {
        BrVector3Cross(pAngular, pPoint, pDirection);
        for (i = 0; i < 3; i++) {
            pAngular->v[i] /= pObject->I.v[i];
        }
    } else {
        BrVector3Set(pAngular, 0.0f, 0.0f, 0.f);
    }
    inverse_mass = (br_scalar)(1.0 / pObject->M);
    BrVector3Scale(pLinear, pDirection, inverse_mass);
}

// STUB: CARMA2_HW 0x004be550
int C2_HOOK_FASTCALL SetUpTorqueHingePartners(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, const br_vector3* pDirection, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004becf0
int C2_HOOK_FASTCALL SetUpHingePartners(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004bf490
void C2_HOOK_FASTCALL MakeMatrix(br_scalar* pMatrix, int pStride, tPhysics_joint_force** pRecords, int pCount) {
    NOT_IMPLEMENTED();
}

// MakeMatrix

// FUNCTION: CARMA2_HW 0x004c03f0
void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, rotate_omega, 0x1c0);

    while (pObject != NULL) {
        BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
        if (pObject->child != NULL) {
            RestoreListOmega(pObject->child);
        }
        pObject = pObject->next;
    }
}

// STUB: CARMA2_HW 0x004c0430
int C2_HOOK_FASTCALL MakeVelocitiesVector2(br_scalar* pRhs, tPhysics_joint_force** pRecords, int pCount, int pCollision_count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c0ac0
void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* pContact, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

// NewAddFrictionObjectObject

// NewAddFriction

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

// FUNCTION: CARMA2_HW 0x004c1f10
void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis;
    br_vector3 momentum;
    br_vector3 rotated;
    br_scalar length;
    br_scalar angle;

    length = BrVector3Length(&pObject->omega);
    angle = length * pTime;
    if (angle < 0.0001) {
        return;
    }
    axis.v[0] = pObject->omega.v[0] / length;
    axis.v[1] = pObject->omega.v[1] / length;
    axis.v[2] = pObject->omega.v[2] / length;
    momentum.v[0] = pObject->I.v[0] * pObject->omega.v[0];
    momentum.v[1] = pObject->I.v[1] * pObject->omega.v[1];
    momentum.v[2] = pObject->I.v[2] * pObject->omega.v[2];
    BrMatrix34Rotate(&rotation, BrRadianToAngle(angle), &axis);
    BrMatrix34TApplyV(&rotated, &momentum, &rotation);
    BrMatrix34PreTranslate(&rotation, -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
    BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
    BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
    pObject->omega.v[0] = rotated.v[0] / pObject->I.v[0];
    pObject->omega.v[1] = rotated.v[1] / pObject->I.v[1];
    pObject->omega.v[2] = rotated.v[2] / pObject->I.v[2];
}

// RotateObject

void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 tv;

    BrVector3Scale(&tv, &pObject->v, pDelta_time);
    BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
}

// STUB: CARMA2_HW 0x004c2060
// STUB: CARMA2_SW 0x004be790
void C2_HOOK_FASTCALL GetFacesNearObject(tPhysics_object *pObject, tWorld_callbacks *pCallbacks) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c25a0
// FUNCTION: CARMA2_SW 0x004becd0
void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject) {
    tPhysics_object* child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xbc, 0xbc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xf0, 0xf0);

    if (pObject->field_0xf0 == 1) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, &pObject->field_0xbc);
    } else {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        ResetObjectAndChildren(child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2600
// FUNCTION: CARMA2_SW 0x004bed30
void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects) {

    while (pObjects != NULL) {
        ResetObjectAndChildren(pObjects);
        pObjects = pObjects->next;
    }
}

// RepositionObjectGroup

// STUB: CARMA2_HW 0x004c2670
void C2_HOOK_FASTCALL SimpleRotate(tPhysics_object* pObject, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c27c0
void C2_HOOK_FASTCALL FUN_004c27c0(tPhysics_object* pObject, float pDt) {
    NOT_IMPLEMENTED();
}

// InterpolateSingleObject

// InterpolateObjects

// FUNCTION: CARMA2_HW 0x004c2910
// FUNCTION: CARMA2_SW 0x004bf040
void C2_HOOK_FASTCALL ApplyGravitySingleObject(tPhysics_object* pObject) {

    if (!pObject->disable_move_rotate) {
        BrVector3Accumulate(&pObject->v, &pObject->field_0x54);
    }
    if (pObject->child != NULL) {
        ApplyGravity(pObject->child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2970
// FUNCTION: CARMA2_SW 0x004bf0a0
void C2_HOOK_FASTCALL ApplyGravity(tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        ApplyGravitySingleObject(pObject);
    }
}

// FUNCTION: CARMA2_HW 0x004c29d0
// FUNCTION: CARMA2_SW 0x004bf100
void C2_HOOK_FASTCALL SetGravityDeltaV(tPhysics_object* pObject, br_vector3* pImpulse) {
    tPhysics_object* pChild;

    pObject->field_0x54 = *pImpulse;
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        SetGravityDeltaV(pChild, pImpulse);
    }
}

// TrialPositionChildren

// FUNCTION: CARMA2_HW 0x004c2b20
// FUNCTION: CARMA2_SW 0x004bf250
void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot) {
    tPhysics_object* child;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);

    child = pParent->child;
    if (!(pParent->flags & 0x400)) {
        pRoot = pParent;
    }
    for (; child != NULL; child = child->next) {
        if (!(pParent->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->transform_matrix);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->transform_matrix.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        }
        InternalPositionChildren(child, pRoot);
    }
}

// FUNCTION: CARMA2_HW 0x004c2b10
// FUNCTION: CARMA2_SW 0x004bf240
void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info) {

    InternalPositionChildren(pCollision_info, pCollision_info);
}

// FUNCTION: CARMA2_HW 0x004c2c30
// FUNCTION: CARMA2_SW 0x004bf360
void C2_HOOK_FASTCALL ApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) {
    br_vector3 torque;
    br_vector3 inertia;

    BrVector3Scale(&torque, pTorque, 1.0f / REC2_SQR(WORLD_SCALE));
    BrVector3Copy(&inertia, &pObject->I);
    pObject->omega.v[0] += torque.v[0] / inertia.v[0];
    pObject->omega.v[1] += torque.v[1] / inertia.v[1];
    pObject->omega.v[2] += torque.v[2] / inertia.v[2];
}

// FUNCTION: CARMA2_HW 0x004c2c90
// FUNCTION: CARMA2_SW 0x004bf3c0
void C2_HOOK_FASTCALL ApplyDiminishingTorque(tPhysics_object* pObject, const br_vector3* pV) {
    br_vector3 original_omega;
    br_vector3 tv;
    br_vector3 inertia;

    original_omega = pObject->omega;
    BrVector3Scale(&tv, pV, (br_scalar)(1.0 / REC2_SQR(WORLD_SCALE_D)));
    BrVector3Copy(&inertia, &pObject->I);
    pObject->omega.v[0] -= tv.v[0] / inertia.v[0];
    pObject->omega.v[1] -= tv.v[1] / inertia.v[1];
    pObject->omega.v[2] -= tv.v[2] / inertia.v[2];
    if (original_omega.v[0] * pObject->omega.v[0] <= 0.0f) {
        pObject->omega.v[0] = 0.0f;
    }
    if (original_omega.v[1] * pObject->omega.v[1] <= 0.0f) {
        pObject->omega.v[1] = 0.0f;
    }
    if (original_omega.v[2] * pObject->omega.v[2] <= 0.0f) {
        pObject->omega.v[2] = 0.0f;
    }
}

typedef struct tPhysics_joint_force_group {
    int count;
    tPhysics_joint_force* records;
    undefined4 field_0x8[3];
    br_scalar* orthogonal;
    br_scalar* matrix;
} tPhysics_joint_force_group;

// STUB: CARMA2_HW 0x004c3b60
int C2_HOOK_FASTCALL RQMatrixSolveNoNegatives(br_scalar* pMatrix, br_scalar* pRhs, br_scalar* pX, int pCount, int pStride, tU8* pFlags, int* pOut, int pJoint_count, int pCollision_count, tPhysics_joint_force_group** pGroups, int pGroup_count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c4850
void C2_HOOK_FASTCALL RQExpand(br_scalar* pL, br_scalar* pMatrix, int pIndex, int pCount, br_scalar* pInv, tU8* pOrder) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c4f40
void C2_HOOK_FASTCALL RQSingleExpand(br_scalar* pL, br_scalar* pMatrix, int pIndex, br_scalar* pInv, tU8* pOrder) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c5220
void C2_HOOK_FASTCALL MakeMatrixFromIndices(br_scalar* pL, br_scalar* pInv, br_scalar* pMatrix, br_scalar* pB, br_scalar* pRhs, tU8* pOrder, int pCount, tPhysics_joint_force_group** pGroups, int pGroup_count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c53b0
void C2_HOOK_FASTCALL RQMatrixSolveWithNegatives(br_scalar* pMatrix, br_scalar* pOrthogonal, br_scalar* pVelocity, br_scalar* pImpulses, int pCount) {
    NOT_IMPLEMENTED();
}

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

// FUNCTION: CARMA2_HW 0x004c5830
// FUNCTION: CARMA2_SW 0x004c1f60
int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pDim, br_scalar* pQ) {
    int i;
    int j;
    int k;
    int singular;
    br_scalar scale;
    br_scalar sum;
    br_scalar norm;
    br_scalar divisor;
    br_scalar matrix_sum;
    br_scalar orthogonal_sum;
    br_scalar tau;
    br_scalar tau2;
    br_scalar* row;

    /* Using householder transformations, apply RQ decomposition on pMatrix:
     * - pMatrix is in-place converted into a lower-triangular matrix
     * - pQ is a orthonormal matrix */

    singular = 0;

    /* pQ starts as a identity matrix */
    for (i = 0; i < pDim; i++) {
        for (j = 0; j < pDim; j++) {
            pQ[i * pDim + j] = 0.0f;
        }
        pQ[i * pDim + i] = 1.0f;
    }

    for (i = 0; i < pDim; i++) {
        // Rotate i-th upper-diagonal row onto base vector

        row = &pMatrix[i * pDim];
        scale = 0.0f;
        for (j = i; j < pDim; j++) {
            scale = (br_scalar)MAX(scale, fabs(row[j]));
        }
        if (scale == 0.0) {
            singular = 1;
            continue;
        }
        // Only normalize the upper-vector that must be rotated onto the base vector
        // This is done for numerical stability reasons.
        for (j = i; j < pDim; j++) {
            row[j] /= scale;
        }
        // Calculate norm of the upper-diagonal row
        sum = 0.0f;
        for (j = i; j < pDim; j++) {
            sum += row[j] * row[j];
        }
        // Apply Householder reflection on pQ and the row+matrix
        norm = (br_scalar)(row[i] > 0.0 ? fabs(sqrt(sum)) : -fabs(sqrt(sum)));
        divisor = norm * (norm + row[i]);
        row[i] += norm;
        for (j = 0; j <= i; j++) {
            sum = 0.0f;
            for (k = i; k < pDim; k++) {
                sum += row[k] * pQ[k * pDim + j];
            }
            tau = sum / divisor;
            for (k = i; k < pDim; k++) {
                pQ[k * pDim + j] -= tau * row[k];
            }
        }
        for (j = i + 1; j < pDim; j++) {
            matrix_sum = 0.0f;
            for (k = i; k < pDim; k++) {
                matrix_sum += pMatrix[j * pDim + k] * row[k];
            }
            tau2 = matrix_sum / divisor;
            orthogonal_sum = 0.0f;
            for (k = i; k < pDim; k++) {
                orthogonal_sum += pQ[k * pDim + j] * pMatrix[i * pDim + k];
            }
            tau = orthogonal_sum / divisor;
            for (k = i; k < pDim; k++) {
                pMatrix[j * pDim + k] -= tau2 * row[k];
                pQ[k * pDim + j] -= tau * row[k];
            }
        }
        row[i] = -(norm * scale);
        // Zero upper-dialog values so what remains is lower-diagonal
        for (j = i + 1; j < pDim; j++) {
            row[j] = 0.0f;
        }
    }
    return singular;
}

// MakeIdentity

// SparseRQDcmpV

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

// RQMatrixSolveWithNegatives

// FUNCTION: CARMA2_HW 0x004c5b50
// FUNCTION: CARMA2_SW 0x004c2280
int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim) {
    return RQDcmpV(pR, pDim, pQ);
}

// FUNCTION: CARMA2_HW 0x004c5b60
void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim) {
    br_scalar intermediate[100];
    br_scalar sum;
    int i;
    int j;

    /* Solve Ax = b, with A represented by its RQ decomposition, where:
     * - R is a lower-diagonal matrix
     * - Q is an orthonormal matrix
     * with:
     * - all matrices (A, Q, R) are square of size N x N (with N = pDim)
     * - vectors x, t are of size N x 1
     * A, Q, R matrices are of size [pDim x pDim]
     */

    C2_HOOK_ASSERT(pDim > 0 && pDim <= (int)REC2_ASIZE(intermediate));

    /* Use back substitution to solve Ry = b, with R a lower-diagonal matrix */
    intermediate[0] = pRhs[0] / pR[0 * pDim + 0];             /*  T[0] = B[0] / R[0, 0] */
    for (i = 1; i < pDim; i++) {
        sum = 0.0f;
        for (j = 0; j < i; j++) {
            sum += intermediate[j] * pR[i * pDim + j];        /*  sum += T[j] * R[i, j] */
        }
        intermediate[i] = (pRhs[i] - sum) / pR[i * pDim + i]; /*  T[i] = (B[i] - sum) / R[i, i] */
    }
    /* Solve Qx = T, by multiplying both sides with Q transposed */
    for (i = 0; i < pDim; i++) {
        pX[i] = pQ[0 * pDim + i] * intermediate[0];           /*  X[i]  = Q[0, i] * T[0] */
        for (j = 1; j < pDim; j++) {
            pX[i] += intermediate[j] * pQ[j * pDim + i];      /*  X[i] += Q[j, i] * T[j] */
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5c70
void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    return gPhysics_scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x004c5cb0
void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    gPhysics_scratch_ptr = (tU8*)gPhysics_scratch_ptr + pSize;
}

// FUNCTION: CARMA2_HW 0x004c5d00
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {

    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

// FUNCTION: CARMA2_HW 0x004c5d10
// FUNCTION: CARMA2_SW 0x004c2440
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    result = BrMemAllocate(sizeof(tCollision_shape_sphere), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}

// STUB: CARMA2_HW 0x004c5d30
tCollision_shape_polyhedron* C2_HOOK_FASTCALL FUN_004c5d30(int pCount_points, int pType) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c5d70
// FUNCTION: CARMA2_SW 0x004c24a0
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;
    int count_edges;

    count_edges = (pCount_points - 2) * 3;
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + (pCount_points - 2) * 2 * sizeof(br_vector4), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_polyhedron);
    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += count_edges * sizeof(tPolyhedron_edge_indexes);
    result->polyhedron.planes = (br_vector4*)raw_memory;
    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5dc0
tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3)
        + pCount_lines * sizeof(tPolyhedron_edge_indexes), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_wireframe);
    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5e00
tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType) {
    tCollision_shape_box* result;

    result = BrMemAllocate(sizeof(tCollision_shape_box), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}

// GetJointSize

// FUNCTION: CARMA2_HW 0x004c5e20
// FUNCTION: CARMA2_SW 0x004c2550
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {
    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), (br_uint_8)pType);
}

// FUNCTION: CARMA2_HW 0x004c5e40
tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, (br_uint_8)pType);
    memcpy(clone, pJoint, size);
    return clone;
}

// FUNCTION: CARMA2_HW 0x004c5e70
// FUNCTION: CARMA2_SW 0x004c25a0
void C2_HOOK_FASTCALL FreeShape(void* pData) {
    BrMemFree(pData);
}

// FUNCTION: CARMA2_HW 0x004c5e80
// FUNCTION: CARMA2_SW 0x004c25b0
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
// FUNCTION: CARMA2_SW 0x004c25d0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// FUNCTION: CARMA2_HW 0x004c5eb0
// FUNCTION: CARMA2_SW 0x004c25e0
void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds) {
    int i;

    BrVector3Copy(&pBounds->min, &pVertices[0]);
    BrVector3Copy(&pBounds->max, &pVertices[0]);
    for (i = 1; i < pCount_vertices; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (pVertices[i].v[j] < pBounds->min.v[j]) {
                pBounds->min.v[j] = pVertices[i].v[j];
            } else if (pVertices[i].v[j] > pBounds->max.v[j]) {
                pBounds->max.v[j] = pVertices[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5f20
// FUNCTION: CARMA2_SW 0x004c2650
void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape) {
    int i;
    tPhysicsError error;

    C2_HOOK_BUG_ON(REC2_ASIZE(pShape->common.bb.min.v) != 3);

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        break;
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            PhysicsError(ePhysicsError_WireFrameHasNoPoints, NULL);
        } else {
            GetBoundsFromPointList(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            PhysicsError(ePhysicsError_PolyhedronHasNoPoints, NULL);
        } else {
            error = ConvexHull3D(&pShape->polyhedron.polyhedron);
            if (error != ePhysicsError_Ok) {
                if (error == ePhysicsError_WireFrameHasNoPoints) {
                    pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
                } else {
                    PDFatalError("This polyhedron is too wierd for me");
                }
            }
            GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            PhysicsError(4, NULL);
        } else {
            ConvexHull2D(&pShape->polyhedron.polyhedron);
            GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Sphere:
        BrVector3Copy(&pShape->common.bb.min, &pShape->sphere.sphere.center);
        BrVector3Copy(&pShape->common.bb.max, &pShape->sphere.sphere.center);
        for (i = 0; i < REC2_ASIZE(pShape->common.bb.min.v); i++) {
            pShape->common.bb.min.v[i] -= pShape->sphere.sphere.radius;
            pShape->common.bb.max.v[i] += pShape->sphere.sphere.radius;
        }
        pShape->sphere.sphere.radius_squared = REC2_SQR(pShape->sphere.sphere.radius);
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, NULL);
        break;
    }
}

// STUB: CARMA2_HW 0x004c60a0
// STUB: CARMA2_SW 0x004c27d0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c61c0
// FUNCTION: CARMA2_SW 0x004c28f0
void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {

    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }
    exit(pError);
}

// FUNCTION: CARMA2_HW 0x004c61e0
// FUNCTION: CARMA2_SW 0x004c2910
void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage) {
    (void)pMessage;
}

// FUNCTION: CARMA2_HW 0x004c61f0
// FUNCTION: CARMA2_SW 0x004c2920
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn) {

    gPhysics_error_cb = pError_cbfn;
}

// FUNCTION: CARMA2_HW 0x004c62e0
// FUNCTION: CARMA2_SW 0x004c2a10
void C2_HOOK_FASTCALL RepositionAndOrientateChildren(tPhysics_object* pObject, br_matrix34* pParent_matrix) {
    br_matrix34 inverse;
    br_matrix34 parent_matrix;
    br_matrix34 transform;
    tPhysics_object* child;

    child = pObject->child;
    if (pParent_matrix != NULL) {
        BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->transform_matrix, pParent_matrix);
    } else {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&transform, &inverse, &pObject->actor->t.t.mat);
        BrMatrix34LPNormalise(&parent_matrix, &transform);
        pParent_matrix = &parent_matrix;
    }
    if (child != NULL) {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&parent_matrix, &inverse, &pObject->actor->t.t.mat);
        for (; child != NULL; child = child->next) {
            RepositionAndOrientateChildren(child, pParent_matrix);
        }
    }
    BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c63b0
// FUNCTION: CARMA2_SW 0x004c2ae0
void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
}

// FUNCTION: CARMA2_HW 0x004c63d0
// FUNCTION: CARMA2_SW 0x004c2b00
void C2_HOOK_FASTCALL PhysicsAddObjectList(tPhysics_object* pParent, tPhysics_object* pChild) {
    tPhysics_object* last;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->parent = pParent;
    last = pChild;
    while (last->next != NULL) {
        last = last->next;
        last->parent = pParent;
    }
    last->next = pParent->child;
    pParent->child = pChild;
}

// FUNCTION: CARMA2_HW 0x004c6410
// FUNCTION: CARMA2_SW 0x004c2b40
void C2_HOOK_FASTCALL PhysicsRemoveObject(tPhysics_object* pObject) {
    tPhysics_object* parent;
    tPhysics_object* sibling;

    parent = pObject->parent;
    sibling = parent->child;
    if (sibling == pObject) {
        parent->child = pObject->next;
    } else {
        while (sibling->next != pObject) {
            sibling = sibling->next;
        }
        sibling->next = pObject->next;
    }
    pObject->parent = NULL;
    pObject->next = NULL;
}

// FUNCTION: CARMA2_HW 0x004c6470
// FUNCTION: CARMA2_SW 0x004c2ba0
int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    int result;
    tPhysics_object* child;

    result = pCallback(pObject, pContext);
    if (result != 0) {
        return result;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c64b0
int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    tPhysics_object* child;

    for (child = pObject->child; child != NULL; child = child->next) {
        int result;

        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c6510
// FUNCTION: CARMA2_SW 0x004c2c40
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {

    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
    }
    gPhysics_other_buffer_capacity = pSize;
    gPhysics_other_buffer = pBuffer;
}

// FUNCTION: CARMA2_HW 0x004b57d0
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks) {
    tU8 cx_min;
    tU8 cz_min;
    tU8 cx_max;
    tU8 cz_max;
    int x;
    int z;
    int emitted;
    tTrack_spec* track_spec;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_z, 0x1);

    emitted = 0;
    track_spec = &gProgram_state.track_spec;
    FillInBounds(pBounds);
    XZToColumnXZ(&cx_min, &cz_min, pBounds->real_bounds.min.v[0], pBounds->real_bounds.min.v[2], track_spec);
    XZToColumnXZ(&cx_max, &cz_max, pBounds->real_bounds.max.v[0], pBounds->real_bounds.max.v[2], track_spec);
    if (cx_min > 0) {
        cx_min--;
    }
    if (cz_min > 0) {
        cz_min--;
    }
    if (cx_max + 1 < track_spec->ncolumns_x) {
        cx_max++;
    }
    if (cz_max + 1 < track_spec->ncolumns_z) {
        cz_max++;
    }
    for (x = cx_min; x <= cx_max; x++) {
        for (z = cz_min; z <= cz_max; z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                emitted = pMax_face - ActorBoxPick(pBounds, track_spec->columns[z][x].actor_0x0, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        emitted = pMax_face - ActorBoxPick(pBounds, gAdditional_actors, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
    }
    return emitted;
}

// FUNCTION: CARMA2_HW 0x004b5970
float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace) {
    int material_index;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCurrent_race.material_modifiers) != 11);

    material_index = pFace->material->identifier[0] - '/';
    if (material_index < 0 || material_index >= (int)REC2_ASIZE(gCurrent_race.material_modifiers)) {
        material_index = 0;
    }
    return gCurrent_race.material_modifiers[material_index].car_wall_friction;
}

// FUNCTION: CARMA2_HW 0x004b5990
// FUNCTION: CARMA2_SW 0x004b20b0
void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage) {
    char s[256];

    (void)pMessage;
    switch (pError) {
    case ePhysicsError_UnknownShapeType:
        strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        strcpy(s, "Unknown hinge type");
        /* The original overwrites this message with the code. */
#ifndef REC2_MATCHING
        break;
#endif
        /* fall through */
    default:
        sprintf(s, "%d ", pError);
        break;
    }
    FatalError(1000 + pError, s);
}

// ResetMan2

// FUNCTION: CARMA2_HW 0x004b5aa0
void C2_HOOK_FASTCALL SetUpTestObjects(void) {

}

// STUB: CARMA2_HW 0x004b5ab0
void C2_HOOK_FASTCALL ResetMan(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b5c00
void C2_HOOK_FASTCALL ResetObjectList_1(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// ResetObjectList

// ResetMan

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004b5ca0
// FUNCTION: CARMA2_SW 0x004b23c0
void C2_HOOK_FASTCALL InitPhysics(void) {

    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
}

// STUB: CARMA2_HW 0x004b7250
br_scalar C2_HOOK_FASTCALL PhysicsObjectWeightedSquareSum(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b7290
br_scalar C2_HOOK_FASTCALL HierarchyCalcEnergy(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b7310
void C2_HOOK_FASTCALL FUN_004b7310(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b73a0
void C2_HOOK_FASTCALL FUN_004b73a0(tPhysics_object* pObject, const br_matrix34* pParent_matrix, const br_matrix34* pRoot_matrix) {
    NOT_IMPLEMENTED();
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID
