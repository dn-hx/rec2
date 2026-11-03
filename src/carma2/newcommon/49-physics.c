#include "49-physics.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "52-errors.h"
#include "60-phil.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "c2_string.h"
#include "c2_stdlib.h"

#include <stddef.h>

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

// GLOBAL: CARMA2_HW 0x006940c0
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x00744810
// GLOBAL: CARMA2_SW 0x0073b2d0
int gImmortal_peds;

// GLOBAL: CARMA2_HW 0x0074a5e0
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x006940c8
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x006940c4
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006940bc
void* gPhysics_scratch_ptr;

// GLOBAL: CARMA2_HW 0x0079d858
tFace_ref* gPling_face;

// GLOBAL: CARMA2_HW 0x005964c4
int gActorBoxPick_StopGroovidelics = 1;

// count

// FUNCTION: CARMA2_HW 0x004225b0
void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix) {
    br_vector3 relative;

    BrVector3Sub(&relative, source, (br_vector3*)matrix->m[3]);
    BrMatrix34TApplyV(dest, &relative, matrix);
}

#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif
// FUNCTION: CARMA2_HW 0x00422fe0
int C2_HOOK_FASTCALL AddObjectWallForce(float* pWalls, const br_vector3* pPoint, float pDistance, const br_vector3* pNormal, tU32 pUnused, int pCount, tPhysics_object* pObject, float pT, float pRadius, const float* pVector) {
    float* piVar1;
    float fAccum;
    float fVar2;
    float fVar3;
    float fVar4;
    tPhysics_object* pObj;
    int iVar6;
    int iVar8;
    int iVar9;
    float* pfVar7;
    float* pfVar10;
    float* pfVar11;
    float* pfVar12;
    float local_18[6];

    pObj = pObject;
    if ((*(int*)((char*)pObj + 0x228) == 0) &&
        (piVar1 = *(float**)((char*)pObj + 0x18c), piVar1 != NULL) && (*(int*)piVar1 != 0)) {
        iVar8 = 0;
        local_18[3] = pObj->bb1.max.v[0] - (float)piVar1[2];
        local_18[4] = pObj->bb1.max.v[1] - (float)piVar1[3];
        local_18[5] = pObj->bb1.max.v[2] - (float)piVar1[4];
        local_18[0] = pObj->bb1.min.v[0] - (float)piVar1[2];
        local_18[1] = pObj->bb1.min.v[1] - (float)piVar1[3];
        local_18[2] = pObj->bb1.min.v[2] - (float)piVar1[4];
        fAccum = 0.0;
        do {
            fVar2 = ABS(*(float*)((char*)local_18 + iVar8 + 0xc));
            if (ABS(*(float*)((char*)local_18 + iVar8)) <= fVar2) {
                fVar2 = ABS(*(float*)((char*)local_18 + iVar8));
            }
            if (fAccum < fVar2) {
                fAccum = fVar2;
            }
            iVar8 = iVar8 + 4;
        } while (iVar8 < 0xc);
        fAccum = fAccum - -0.01;
        if (((ABS(pPoint->v[0] - (float)piVar1[2]) < fAccum) &&
            (fVar2 = ABS(pPoint->v[1] - (float)piVar1[3]), fVar2 < fAccum)) &&
           ((fVar2 + fVar2 < pObj->bb1.max.v[1] - pObj->bb1.min.v[1]) &&
            (ABS(pPoint->v[2] - (float)piVar1[4]) < fAccum))) {
            return pCount;
        }
    }
    iVar8 = 0;
    if (0 < pCount) {
        pfVar7 = pWalls + 6;
        do {
            fVar4 = pfVar7[-1] - (pPoint->v[0] - pObj->cmpos.v[0]);
            fVar2 = *pfVar7 - (pPoint->v[1] - pObj->cmpos.v[1]);
            fVar3 = pfVar7[1] - (pPoint->v[2] - pObj->cmpos.v[2]);
            fVar2 = fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4;
            if (fVar2 < 0.0045) {
                fVar3 = pfVar7[5] * pNormal->v[0] + pNormal->v[1] * pfVar7[6] + pfVar7[7] * pNormal->v[2];
                if ((0.95 < fVar3) && (fVar2 < 0.0005)) {
                    return pCount;
                }
                if (fVar3 < -0.95) {
                    return pCount;
                }
            }
            iVar8 = iVar8 + 1;
            pfVar7 = pfVar7 + 0x18;
        } while (iVar8 < pCount);
    }
    iVar8 = pCount;
    if (pCount < 4) {
        iVar8 = pCount + 1;
        pWalls[pCount * 0x18] = 2.0;
    }
    iVar9 = 0;
    pfVar7 = pWalls;
    if (0 < iVar8) {
        while (*pfVar7 <= pDistance) {
            iVar9 = iVar9 + 1;
            pfVar7 = pfVar7 + 0x18;
            if (iVar8 <= iVar9) {
                return iVar8;
            }
        }
        iVar6 = 3;
        if (iVar8 < 4) {
            iVar6 = iVar8;
        }
        if (iVar9 <= iVar6) {
            pCount = (iVar6 - iVar9) + 1;
            pfVar11 = pWalls + iVar6 * 0x18;
            do {
                pfVar10 = pfVar11 + -0x18;
                pfVar12 = pfVar11;
                for (iVar9 = 0x18; iVar9 != 0; iVar9 = iVar9 + -1) {
                    *pfVar12 = *pfVar10;
                    pfVar10 = pfVar10 + 1;
                    pfVar12 = pfVar12 + 1;
                }
                pCount = pCount + -1;
                pfVar11 = pfVar11 + -0x18;
            } while (pCount != 0);
        }
        *pfVar7 = pDistance;
        *(tPhysics_object**)(pfVar7 + 1) = pObj;
        pfVar7[2] = 0.0;
        pfVar7[3] = pRadius;
        pfVar7[0xb] = pNormal->v[0];
        pfVar7[0xc] = pNormal->v[1];
        pfVar7[0xd] = pNormal->v[2];
        pfVar7[5] = pPoint->v[0] - pObj->cmpos.v[0];
        pfVar7[6] = pPoint->v[1] - pObj->cmpos.v[1];
        pfVar7[7] = pPoint->v[2] - pObj->cmpos.v[2];
        if (pVector != NULL) {
            pfVar7[8] = pVector[0];
            pfVar7[9] = pVector[1];
            pfVar7[10] = pVector[2];
        }
        pfVar7[0x17] = pT;
    }
    return iVar8;
}

// based on ApexWorm/rec2 1b37e235f (reference only)
// FUNCTION: CARMA2_HW 0x004268a0
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

// (and ApexWorm/rec2 44a06345a)
// FUNCTION: CARMA2_HW 0x004225f0
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

// RayCastThruFaceList2

/* based on the original disassembly (and ApexWorm c0f29cfd4 as reference) */
// FUNCTION: CARMA2_HW 0x0041e480
int C2_HOOK_FASTCALL RayCastInBox(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, tPhysics_object* pObject) {
    extern int gUNK_006793d0;
    br_vector3 normal;
    br_vector3 coll_pos;
    br_scalar t;
    int best;
    int i;
    tFace_ref* pFace;

    *pT = 2.f;
    for (i = pObject->box_face_start, pFace = &gFace_list__car[i]; i < pObject->box_face_end; i++, pFace++) {
        if (gUNK_006793d0 != 0 && (pFace->flags & 0x80) != 0) {
            continue;
        }
        CheckSingleFace(pFace, pRay_pos, pRay_dir, &normal, &t, &coll_pos);
        if (t < *pT) {
            *pT = t;
            best = i;
            BrVector3Copy(pHit_pos, &normal);
        }
    }
    if (*pT < 2.f) {
        int c;

        c = gFace_list__car[best].material->identifier[0] - '/';
        if (c >= 0 && c < 11) {
            return c;
        }
    }
    return 0;
}

// FaceOffCount2

// FaceOffCount

// FaceOnCount

// FUNCTION: CARMA2_HW 0x00423320
int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, int* pCtx, br_matrix34* pMatrix) {
    float* pfVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    int iVar7;
    float fVar8;
    float* pfVar9;
    tU32 uVar10;
    int* piVar13;
    int iVar14;
    int iVar15;
    int local_3c;
    float local_24[9];
    float local_30[3];
    float* pfVar11;
    float* pfVar12;

    iVar7 = *(int*)pObject;
    pfVar1 = (float*)((char*)pCtx + 0x1c);
    iVar14 = 0;
    local_3c = 0;
    GetNewBoundingBox((br_bounds*)pfVar1, (br_bounds*)(pCtx + 1), pMatrix);
    pfVar9 = (float*)((char*)pCtx + 0x10);
    iVar15 = 3;
    pfVar11 = pfVar1;
    do {
        if (pfVar9[-3] < *pfVar11) {
            *pfVar11 = pfVar9[-3];
        }
        if (pfVar11[3] < *pfVar9) {
            pfVar11[3] = *pfVar9;
        }
        pfVar11 = pfVar11 + 1;
        pfVar9 = pfVar9 + 1;
        iVar15 = iVar15 + -1;
    } while (iVar15 != 0);
    BrMatrix34ApplyV((br_vector3*)local_30, (br_vector3*)pfVar1, (br_matrix34*)(iVar7 + 0x2c));
    fVar2 = *(float*)(iVar7 + 0x50);
    pfVar11 = local_24 + 1;
    pfVar9 = (float*)(iVar7 + 0x34);
    fVar3 = *(float*)(iVar7 + 0x54);
    iVar15 = 3;
    fVar4 = *(float*)(iVar7 + 0x58);
    pfVar12 = pfVar1;
    do {
        fVar8 = pfVar12[3] - *pfVar12;
        fVar5 = pfVar9[-1];
        fVar6 = *pfVar9;
        pfVar11[-1] = pfVar9[-2] * fVar8;
        *pfVar11 = fVar5 * fVar8;
        pfVar12 = pfVar12 + 1;
        pfVar11 = pfVar11 + 3;
        *(float*)(((int)local_24 - (iVar7 + 0x2c)) + (int)pfVar9) = fVar6 * fVar8;
        pfVar9 = pfVar9 + 3;
        iVar15 = iVar15 + -1;
    } while (iVar15 != 0);
    uVar10 = (tU32)*(tU16*)((char*)pObject + 0x174);
    piVar13 = (int*)((char*)gFace_list__car + uVar10 * 0x50);
    if (*(tU16*)((char*)pObject + 0x176) != uVar10 &&
        -1 < (int)(*(tU16*)((char*)pObject + 0x176) - uVar10)) {
        do {
            pfVar9 = local_24 + 2;
            iVar15 = 3;
            fVar5 = ((local_30[1] + fVar3) - (float)piVar13[2]) * (float)piVar13[0xe] +
                    (float)piVar13[0xf] * ((local_30[2] + fVar4) - (float)piVar13[3]) +
                    (float)piVar13[0xd] * ((local_30[0] + fVar2) - (float)piVar13[1]);
            fVar6 = fVar5;
            do {
                fVar8 = *pfVar9 * (float)piVar13[0xf] +
                        (float)piVar13[0xe] * pfVar9[-1] + (float)piVar13[0xd] * pfVar9[-2];
                if (0.0 <= fVar8) {
                    fVar5 = fVar8 + fVar5;
                }
                else {
                    fVar6 = fVar6 + fVar8;
                }
                pfVar9 = pfVar9 + 3;
                iVar15 = iVar15 + -1;
            } while (iVar15 != 0);
            if ((((0.005 < fVar5) && (0.005 < fVar6)) || ((fVar5 < -0.005 && (fVar6 < -0.005)))) ||
               ((iVar15 = *piVar13, iVar15 != 0 && (**(char**)(iVar15 + 4) == '!')))) {
                piVar13[0x10] = piVar13[0x10] | 0x80;
            }
            else if (**(char**)(iVar15 + 4) == '>') {
                piVar13[0x10] = piVar13[0x10] | 0x80;
            }
            else {
                pfVar9 = (float*)(iVar7 + 0x34);
                iVar15 = 3;
                pfVar11 = pfVar1;
                do {
                    fVar5 = *(float*)(iVar7 + 0x54) * pfVar9[-1] +
                            *(float*)(iVar7 + 0x50) * pfVar9[-2] + *(float*)(iVar7 + 0x58) * *pfVar9;
                    fVar8 = (float)piVar13[2] * pfVar9[-1] +
                            (float)piVar13[1] * pfVar9[-2] + *pfVar9 * (float)piVar13[3];
                    fVar6 = (*pfVar11 - 0.005) + fVar5;
                    if (fVar6 <= fVar8) {
                        fVar5 = (pfVar11[3] + fVar5) - -0.005;
                        if (((fVar5 < fVar8) &&
                            (fVar5 < (float)piVar13[6] * *pfVar9 +
                                     pfVar9[-2] * (float)piVar13[4] + pfVar9[-1] * (float)piVar13[5])) &&
                           (fVar5 < (float)piVar13[9] * *pfVar9 +
                                    pfVar9[-2] * (float)piVar13[7] + pfVar9[-1] * (float)piVar13[8]))
                            goto LAB_00423642;
                    }
                    else if (((float)piVar13[6] * *pfVar9 +
                              pfVar9[-2] * (float)piVar13[4] + pfVar9[-1] * (float)piVar13[5] < fVar6) &&
                             ((float)piVar13[9] * *pfVar9 +
                              pfVar9[-2] * (float)piVar13[7] + pfVar9[-1] * (float)piVar13[8] < fVar6)) {
LAB_00423642:
                        piVar13[0x10] = piVar13[0x10] | 0x80;
                    }
                    pfVar11 = pfVar11 + 1;
                    pfVar9 = pfVar9 + 3;
                    iVar15 = iVar15 - 1;
                } while (iVar15 != 0);
                local_3c = local_3c + 1;
                piVar13[0x10] = piVar13[0x10] & 0xffffff7f;
            }
            iVar14 = iVar14 + 1;
            piVar13 = piVar13 + 0x14;
        } while (iVar14 < (int)((tU32)*(tU16*)((char*)pObject + 0x176) - (tU32)*(tU16*)((char*)pObject + 0x174)));
    }
    return local_3c;
}

// based on ApexWorm/rec2 968549a84 (reference only)
// FUNCTION: CARMA2_HW 0x00428480
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

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

void C2_HOOK_FASTCALL SphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT, tU32 pFlags_a, tU8 pFlags_b);

// FUNCTION: CARMA2_HW 0x00423ce0
int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdgeStart, br_vector3* pEdgeEnd, tPhysics_object* pObject, float* pSphere, float* pWalls, int pCount, br_vector3* pStart, br_vector3* pEnd, tFace_ref* pFace) {
    float local_48[3];
    float local_3c[3];
    float local_30[3];
    float local_24[5];
    float local_10;
    float local_c[3];
    float fVar1;
    float fVar2;
    float fVar3;
    float* pfVar4;
    int uVar5;
    tU32 four;

    pfVar4 = pSphere;
    SphereEdgeCollide(pEdgeStart, pEdgeEnd, pStart, pEnd, (tCollision_shape_sphere_data*)pSphere, (br_vector3*)local_c, (br_vector3*)local_24, (br_scalar*)&pSphere, 0, 0);
    if (*(float*)&pSphere <= 1.0f) {
        BrMatrix34TApplyV((br_vector3*)local_48, (br_vector3*)local_24, (br_matrix34*)((char*)pObject + 0x8c));
        BrMatrix34TApplyV((br_vector3*)local_3c, (br_vector3*)local_24, (br_matrix34*)(*(char**)pObject + 0x2c));
        fVar1 = 1.0f - *(float*)&pSphere;
        local_3c[0] = *(float*)&pSphere * local_3c[0];
        local_3c[1] = *(float*)&pSphere * local_3c[1];
        local_3c[2] = *(float*)&pSphere * local_3c[2];
        fVar3 = local_48[0] * fVar1 + local_3c[0];
        fVar2 = local_48[1] * fVar1 + local_3c[1];
        fVar1 = local_48[2] * fVar1 + local_3c[2];
        local_10 = (float)sqrt(fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3);
        if (local_10 <= 2.3841858e-07f) {
            fVar3 = 1.0;
            fVar2 = 0.0;
            local_10 = 0.0;
        } else {
            local_10 = 1.0 / local_10;
            fVar3 = local_10 * fVar3;
            fVar2 = local_10 * fVar2;
            local_10 = local_10 * fVar1;
        }
        local_30[0] = pfVar4[1] + *pfVar4 * fVar3;
        local_30[1] = pfVar4[2] + *pfVar4 * fVar2;
        local_30[2] = pfVar4[3] + *pfVar4 * local_10;
        local_48[0] = -local_48[0];
        local_48[1] = -local_48[1];
        local_48[2] = -local_48[2];
        four = 4;
        uVar5 = AddObjectWallForce(pWalls, (br_vector3*)local_30, *(float*)&pSphere, (br_vector3*)local_48, 0, pCount, pObject, *(float*)&four, *(float*)&pFace, NULL);
        return uVar5;
    }
    return pCount;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00423c00
int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, int pEdx, int pParam1, int pParam2, int pParam3, int pParam4, int pParam5, int pParam6) {
    int local_4;
    char* puVar1;

    local_4 = (int)*(tU16*)((char*)pObject + 0x174);
    puVar1 = (char*)gFace_list__car + local_4 * 0x50;
    if (local_4 < (int)*(tU16*)((char*)pObject + 0x176)) {
        do {
            if ((*(tU32*)(puVar1 + 0x40) & 0x80) == 0) {
                if ((*(tU32*)(puVar1 + 0x40) & 1) == 0) {
                    pParam2 = CheckSingleEdgeWithSphere((br_vector3*)(puVar1 + 0x4), (br_vector3*)(puVar1 + 0x10), pObject, (float*)pEdx, (float*)pParam1, pParam2, (br_vector3*)pParam4, (br_vector3*)pParam5, (tFace_ref*)puVar1);
                }
                if ((*(tU32*)(puVar1 + 0x40) & 2) == 0) {
                    pParam2 = CheckSingleEdgeWithSphere((br_vector3*)(puVar1 + 0x10), (br_vector3*)(puVar1 + 0x1c), pObject, (float*)pEdx, (float*)pParam1, pParam2, (br_vector3*)pParam4, (br_vector3*)pParam5, (tFace_ref*)puVar1);
                }
                if ((*(tU32*)(puVar1 + 0x40) & 4) == 0) {
                    pParam2 = CheckSingleEdgeWithSphere((br_vector3*)(puVar1 + 0x1c), (br_vector3*)(puVar1 + 0x4), pObject, (float*)pEdx, (float*)pParam1, pParam2, (br_vector3*)pParam4, (br_vector3*)pParam5, (tFace_ref*)puVar1);
                }
            }
            local_4 = local_4 + 1;
            puVar1 = puVar1 + 0x50;
        } while ((int)local_4 < (int)(tU32)*(tU16*)((char*)pObject + 0x176));
    }
    return pParam2;
}

// FUNCTION: CARMA2_HW 0x00423ed0
int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, int pEdx, tU32 pParam1, int pParam2, int pParam3, char* pParam4, tU32 pParam5) {
    int local_54;
    float local_4c;
    float local_48[3];
    float local_3c[3];
    float local_30[3];
    float local_24[3];
    float local_18[3];
    float local_c[3];
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float* pfVar5;
    int* piVar;
    int iVar4;
    int start;
    int count;

    if (3 < pParam3) {
        local_54 = 0;
        if (*(short*)(pParam4 + 0x700) != 0) {
            pfVar5 = (float*)(pParam4 + 0x304);
            piVar = (int*)(pParam4 + 0x600);
            do {
                if (*piVar == 0) {
                    local_48[0] = pfVar5[-1] - pfVar5[-0xc1];
                    local_48[1] = *pfVar5 - pfVar5[-0xc0];
                    local_48[2] = pfVar5[1] - pfVar5[-0xbf];
                    fVar4 = (float)sqrt(local_48[2] * local_48[2] + local_48[1] * local_48[1] + local_48[0] * local_48[0]);
                    if (fVar4 <= 2.3841858e-07f) {
                        local_3c[0] = 1.0f;
                        local_3c[1] = 0.0f;
                        fVar4 = 0.0f;
                    } else {
                        fVar4 = 1.0f / fVar4;
                        local_3c[0] = fVar4 * local_48[0];
                        local_3c[1] = fVar4 * local_48[1];
                        fVar4 = fVar4 * local_48[2];
                    }
                    local_3c[2] = fVar4;
                    local_3c[0] = local_3c[0] * 0.0010434783f;
                    local_3c[1] = local_3c[1] * 0.0010434783f;
                    local_3c[2] = local_3c[2] * 0.0010434783f;
                    local_48[0] = local_3c[0] + local_48[0];
                    local_48[1] = local_3c[1] + local_48[1];
                    local_48[2] = local_3c[2] + local_48[2];
                    start = (int)*(tU16*)((char*)pObject + 0x174);
                    count = (int)*(tU16*)((char*)pObject + 0x176) - start;
                    iVar4 = (int)RayCastThruFaceList((br_vector3*)local_48, (br_vector3*)local_3c, (br_vector3*)local_c, (br_scalar*)&local_4c, (br_vector3*)local_30, (tFace_ref*)((char*)gFace_list__car + start * 0x50), (int)count);
                    if (iVar4 != 0) {
                        BrMatrix34TApplyV((br_vector3*)local_3c, (br_vector3*)local_30, (br_matrix34*)((char*)pObject + 0x8c));
                        fVar1 = local_30[0] * local_48[0];
                        fVar3 = local_30[1] * local_48[1];
                        fVar2 = local_30[2] * local_48[2];
                        local_24[0] = pfVar5[-0xc1] - *(float*)((char*)pObject + 0xb0);
                        local_24[1] = pfVar5[-0xc0] - *(float*)((char*)pObject + 0xb4);
                        local_24[2] = pfVar5[-0xbf] - *(float*)((char*)pObject + 0xb8);
                        BrMatrix34TApplyV((br_vector3*)local_18, (br_vector3*)local_24, (br_matrix34*)((char*)pObject + 0x8c));
                        pParam2 = AddObjectWallForce((float*)pParam4, (br_vector3*)local_18, fVar2 + fVar3 + fVar1, (br_vector3*)local_3c, 0, pParam2, pObject, 1.0f, local_4c, NULL);
                    }
                }
                piVar = piVar + 1;
                local_54 = local_54 + 1;
                pfVar5 = pfVar5 + 3;
            } while (local_54 < (int)(tU32)*(short*)(pParam4 + 0x700));
        }
        if (3 < pParam2) {
            pParam2 = 4;
        }
        return pParam2;
    }
    return 0;
}

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// CheckSingleEdgeWithObjectEdges

// GetEdgeToEdgePoints

// SetUpWorkspace

// GetObjectWallForceInfo

// GetObjectCornerToObjectFacePoints

// BoundsOverlapTest

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

// AddOldForces

// GLOBAL: CARMA2_HW 0x0058f7f0
tPolyhedron_edge_indexes gBox_edge_indexes[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

// FUNCTION: CARMA2_HW 0x00428dc0
int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b) {
    br_scalar t;
    br_vector3 diff;
    br_vector3 tmp;
    br_bounds3 bounds;
    br_vector3 point;
    br_vector3 points[64];
    tPolyhedron_edge_indexes* edges;
    int count;
    int i;
    int j;
    br_matrix34* mat_a;
    br_matrix34* mat_b;

    switch (pShape_a->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        for (i = 0; i < pShape_a->polyhedron.polyhedron.count_points; i++) {
            mat_a = &pObject_a->transform_matrix;
            mat_b = &pObject_b->transform_matrix;
            BrMatrix34ApplyP(&tmp, &pShape_a->polyhedron.polyhedron.points[i], mat_a);
            diff.v[0] = tmp.v[0] - mat_b->m[3][0];
            diff.v[1] = tmp.v[1] - mat_b->m[3][1];
            diff.v[2] = tmp.v[2] - mat_b->m[3][2];
            BrMatrix34TApplyV(&points[i], &diff, mat_b);
        }
        edges = pShape_a->polyhedron.polyhedron.edges;
        count = pShape_a->polyhedron.polyhedron.count_edges;
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        for (i = 0, mat_a = &pObject_a->transform_matrix, mat_b = &pObject_b->transform_matrix; i < 8; i++) {
            points[i].v[0] = (i & 2) ? pShape_a->common.bb.min.v[0] : pShape_a->common.bb.max.v[0];
            points[i].v[1] = (i & 1) ? pShape_a->common.bb.min.v[1] : pShape_a->common.bb.max.v[1];
            points[i].v[2] = (i & 4) ? pShape_a->common.bb.min.v[2] : pShape_a->common.bb.max.v[2];
            BrMatrix34ApplyP(&tmp, &points[i], mat_a);
            diff.v[0] = tmp.v[0] - mat_b->m[3][0];
            diff.v[1] = tmp.v[1] - mat_b->m[3][1];
            diff.v[2] = tmp.v[2] - mat_b->m[3][2];
            BrMatrix34TApplyV(&points[i], &diff, mat_b);
        }
        edges = gBox_edge_indexes;
        count = 12;
        break;
    }

    switch (pShape_b->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        for (j = 0; j < count; j++, edges++) {
            if (LinePolyColl(&points[edges->index1], &points[edges->index2], &pShape_b->polyhedron.polyhedron, &point, (br_scalar)0.005f, &t)) {
                return 1;
            }
        }
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        bounds.min.v[0] = pShape_b->common.bb.min.v[0] - 0.005f;
        bounds.min.v[1] = pShape_b->common.bb.min.v[1] - 0.005f;
        bounds.min.v[2] = pShape_b->common.bb.min.v[2] - 0.005f;
        bounds.max.v[0] = pShape_b->common.bb.max.v[0] - -0.005f;
        bounds.max.v[1] = pShape_b->common.bb.max.v[1] - -0.005f;
        bounds.max.v[2] = pShape_b->common.bb.max.v[2] - -0.005f;
        for (j = 0; j < count; j++, edges++) {
            if (LineBoxColl(&points[edges->index1], &points[edges->index2], &bounds, &point, &t)) {
                return 1;
            }
        }
        break;
    default:
        return 0;
    }
    return 0;
}

// TestObjectOverlap

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

// TestForObjectInSensiblePlace

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
void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject) {
    br_scalar t;

    t = BrVector3LengthSquared(&pObject->omega) * 0.0016f;
    if (t > 2.4649f) {
        t = (float)sqrt(t) * 0.6369f;
        pObject->omega.v[0] = pObject->omega.v[0] / (2.f * t);
        pObject->omega.v[1] = pObject->omega.v[1] / (2.f * t);
        pObject->omega.v[2] = pObject->omega.v[2] / (2.f * t);
    }
}

// ListLimitOmega

// GroupLimitOmega

// PositionWorldJointedObject

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

// MoveJointedObject

// TellPhysicsJointsHaveChanged

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

extern void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time);

extern void C2_HOOK_FASTCALL MergeBoundsIntoParent(tPhysics_object* pObject, br_bounds3* pBox);

extern br_scalar gFLOAT_0058a768;

// PARTIAL 76.27%: regalloc (orig ebx=0 for NULL compares, edi=parent, ebp=&transform_matrix) and x87 order of the 4-way *= 50.f
// FUNCTION: CARMA2_HW 0x004b99e0
void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject) {
    tPhysics_object* child;
    tPhysics_object* parent;
    int i;
    int j;

    if (pObject->M == gFLOAT_0058a768) {
        pObject->M = 0.01f;
        BrVector3SetFloat(&pObject->I, 0.01f, 0.01f, 0.01f);
    }
    {
        int f = pObject->flags;
        f &= ~0x1200;
        pObject->flags = f;
    }
    pObject->field_0x1de = 0;
    pObject->collision_flag = 0;
    pObject->field_0x1dc = 0;
    *(int*)&pObject->field_0x1e0[4] = 0;
    pObject->field_0x21c = 1.f;
    pObject->field183_0x1d8 = (undefined4)pObject;
    pObject->field_0x218 = 0;
    pObject->field_0x1e0[0] = pObject->disable_move_rotate;
    BrMatrix34TApplyV(&pObject->velocity_car_space, &pObject->v, &pObject->transform_matrix);
    parent = pObject->parent;
    ((float*)pObject->field_0x1b4)[0] = pObject->v.v[0];
    ((float*)pObject->field_0x1b4)[1] = pObject->v.v[1];
    ((float*)pObject->field_0x1b4)[2] = pObject->v.v[2];
    ((float*)pObject->field_0x1cc)[0] = pObject->omega.v[0];
    ((float*)pObject->field_0x1cc)[1] = pObject->omega.v[1];
    ((float*)pObject->field_0x1cc)[2] = pObject->omega.v[2];
    if (parent != NULL) {
        pObject->disable_move_rotate = parent->disable_move_rotate;
    }
    if (pObject->disable_move_rotate == 0 && pObject->field_0x1df != 0) {
        pObject->field_0x21c *= 50.f;
        pObject->I.v[0] *= 50.f;
        pObject->I.v[1] *= 50.f;
        pObject->I.v[2] *= 50.f;
    }
    if (pObject->disable_move_rotate & (pObject->field_0xf0 != 2)) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, &pObject->transform_matrix);
        for (child = pObject->child; child != NULL; child = child->next) {
            InternalPrepareObject(child);
        }
        if (pObject->parent == NULL) {
            MergeBoundsIntoParent(pObject, NULL);
        }
        return;
    }
    if (pObject->field_0xf0 == 2) {
        br_vector3 d;

        pObject->field_0x230 = parent;
        d.v[0] = pObject->actor->t.t.mat.m[3][0] - pObject->transform_matrix.m[3][0];
        d.v[1] = pObject->actor->t.t.mat.m[3][1] - pObject->transform_matrix.m[3][1];
        d.v[2] = pObject->actor->t.t.mat.m[3][2] - pObject->transform_matrix.m[3][2];
        if (d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2] > 1.0) {
            PhysicsWarning("Stupid trial position");
            BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
        }
        for (j = 3; j >= 0; j--) {
            for (i = 0; i < 3; i++) {
                if (pObject->transform_matrix.m[j][i] != pObject->actor->t.t.mat.m[j][i]) {
                    pObject->disable_move_rotate = 0;
                    goto differ_done;
                }
            }
        }
differ_done:
        for (child = pObject->child; child != NULL; child = child->next) {
            InternalPrepareObject(child);
        }
        MergeBoundsIntoParent(pObject, NULL);
        return;
    }
    if (pObject->field_0xf0 != 1) {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    } else {
        BrMatrix34Copy((br_matrix34*)pObject->field_0xbc, &pObject->actor->t.t.mat);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        InternalPrepareObject(child);
    }
    if (pObject->parent != NULL) {
        return;
    }
    MoveJointedObject(pObject, 0.04f);
    MergeBoundsIntoParent(pObject, NULL);
}

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

// FUNCTION: CARMA2_HW 0x004c2060
void C2_HOOK_FASTCALL GetFacesInBox(tPhysics_object* pCollision, tWorld_callbacks* pWorld_callbacks) {
    extern br_scalar gFLOAT_0058a768;
    tBounds bnds;
    br_bounds xformed_bounds;
    br_bounds tmp_bounds;
    br_vector3 vel;
    br_matrix34 mat1;
    br_matrix34 mat2;
    br_matrix34 mat3;
    br_matrix34 mat4;
    int i;
    int flags;

    if (pCollision->flags & 0x40) {
        pCollision->box_face_start = pCollision->box_face_end;
        return;
    }
    if ((pCollision->flags & 0x2) && pCollision->parent != NULL) {
        pCollision->box_face_start = pCollision->parent->box_face_start;
        pCollision->box_face_end = pCollision->parent->box_face_end;
        pCollision->box_face_ref = pCollision->parent->box_face_ref;
        return;
    }
    if (pCollision->flags & 0x1) {
        if (pCollision->box_face_ref == gFace_num__car
                || (pCollision->box_face_ref == gFace_num__car - 1
                    && pCollision->box_face_start > gFace_count)) {
            if (pCollision->field_0x10c.min.v[0] > pCollision->field_0x124.min.v[0]
                    && pCollision->field_0x10c.min.v[1] > pCollision->field_0x124.min.v[1]
                    && pCollision->field_0x10c.min.v[2] > pCollision->field_0x124.min.v[2]
                    && pCollision->field_0x10c.max.v[0] < pCollision->field_0x124.max.v[0]
                    && pCollision->field_0x10c.max.v[1] < pCollision->field_0x124.max.v[1]
                    && pCollision->field_0x10c.max.v[2] < pCollision->field_0x124.max.v[2]) {
                return;
            }
        }
        BrVector3Scale(&vel, &pCollision->v, 0.12f);
        for (i = 0; i < 3; i++) {
            bnds.original_bounds.min.v[i] = pCollision->field_0x10c.min.v[i] - 0.15;
            bnds.original_bounds.max.v[i] = pCollision->field_0x10c.max.v[i] + 0.15;
            if (vel.v[i] < gFLOAT_0058a768) {
                bnds.original_bounds.min.v[i] += vel.v[i];
            } else {
                bnds.original_bounds.max.v[i] += vel.v[i];
            }
            pCollision->field_0x124.min.v[i] = bnds.original_bounds.min.v[i] + 0.005;
            pCollision->field_0x124.max.v[i] = bnds.original_bounds.max.v[i] - 0.005;
        }
        bnds.mat = &mat3;
        BrMatrix34Identity(&mat3);
    } else {
        br_matrix34* actor_mat = &pCollision->actor->t.t.mat;
        br_matrix34* trans_mat = &pCollision->transform_matrix;

        if (pCollision->box_face_ref == gFace_num__car
                || (pCollision->box_face_ref == gFace_num__car - 1
                    && pCollision->box_face_start > gFace_count)) {
            BrMatrix34Mul(&mat1, actor_mat, &pCollision->field_0x144);
            GetNewBoundingBox(&xformed_bounds, &pCollision->bb2, &mat1);
            if (xformed_bounds.max.v[0] < pCollision->field_0x124.max.v[0]
                    && xformed_bounds.max.v[1] < pCollision->field_0x124.max.v[1]
                    && xformed_bounds.max.v[2] < pCollision->field_0x124.max.v[2]
                    && xformed_bounds.min.v[0] > pCollision->field_0x124.min.v[0]
                    && xformed_bounds.min.v[1] > pCollision->field_0x124.min.v[1]
                    && xformed_bounds.min.v[2] > pCollision->field_0x124.min.v[2]) {
                return;
            }
        }
        BrMatrix34LPInverse(&mat3, actor_mat);
        BrMatrix34Mul(&mat2, trans_mat, &mat3);
        GetNewBoundingBox(&bnds.original_bounds, &pCollision->bb2, &mat2);
        for (i = 0; i < 3; i++) {
            bnds.original_bounds.min.v[i] = bnds.original_bounds.min.v[i] < pCollision->bb2.min.v[i]
                ? bnds.original_bounds.min.v[i] : pCollision->bb2.min.v[i];
            bnds.original_bounds.max.v[i] = bnds.original_bounds.max.v[i] > pCollision->bb2.max.v[i]
                ? bnds.original_bounds.max.v[i] : pCollision->bb2.max.v[i];
            bnds.original_bounds.min.v[i] -= 0.0025f;
            bnds.original_bounds.max.v[i] += 0.0025f;
        }
        BrMatrix34Mul(&mat1, &mat2, &mat2);
        BrMatrix34Mul(&mat4, &mat1, &mat2);
        BrMatrix34LPInverse(&mat1, &mat4);
        GetNewBoundingBox(&tmp_bounds, &pCollision->bb2, &mat1);
        for (i = 0; i < 3; i++) {
            bnds.original_bounds.min.v[i] = bnds.original_bounds.min.v[i] < tmp_bounds.min.v[i]
                ? bnds.original_bounds.min.v[i] : tmp_bounds.min.v[i];
            bnds.original_bounds.max.v[i] = bnds.original_bounds.max.v[i] > tmp_bounds.max.v[i]
                ? bnds.original_bounds.max.v[i] : tmp_bounds.max.v[i];
            bnds.original_bounds.min.v[i] -= 0.02f;
            bnds.original_bounds.max.v[i] += 0.02f;
        }
        pCollision->field_0x124 = bnds.original_bounds;
        BrMatrix34Copy(&pCollision->field_0x144, &mat3);
        bnds.mat = actor_mat;
    }
    pCollision->box_face_start = gFace_count;
    gPling_face = NULL;
    flags = pCollision->flags;
    gActorBoxPick_StopGroovidelics = (tU8)(~flags) >> 7;
    if (pWorld_callbacks != NULL && pWorld_callbacks->find_faces_in_box != NULL) {
        gFace_count += pWorld_callbacks->find_faces_in_box(&bnds, &gFace_list__car[gFace_count], 300 - gFace_count, pWorld_callbacks);
        if (gFace_count >= 300) {
            pCollision->box_face_start = 0;
            gFace_count = pWorld_callbacks->find_faces_in_box(&bnds, gFace_list__car, 300, pWorld_callbacks);
            gFace_num__car++;
        }
    }
    pCollision->box_face_end = gFace_count;
    pCollision->box_face_ref = gFace_num__car;
    gActorBoxPick_StopGroovidelics = 1;
    if (pWorld_callbacks != NULL && pWorld_callbacks->new_face_list != NULL) {
        pWorld_callbacks->new_face_list(pCollision, (undefined4*)gPling_face);
    }
    if (pCollision->flags & 0x80) {
        pCollision->box_face_end = pCollision->box_face_start;
    }
}

// FUNCTION: CARMA2_HW 0x004c2910
void C2_HOOK_FASTCALL PhysicsObjectMoveVelocity(tPhysics_object* pObject) {
    tPhysics_object* pChild;

    if (!pObject->disable_move_rotate) {
        pObject->v.v[0] += pObject->field_0x54.v[0];
        pObject->v.v[1] += pObject->field_0x54.v[1];
        pObject->v.v[2] += pObject->field_0x54.v[2];
    }
    if (pObject->child != NULL) {
        for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
            PhysicsObjectMoveVelocity(pChild);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c2970
void C2_HOOK_FASTCALL PhysicsObjectMoveVelocityList(tPhysics_object* pObject) {

    while (pObject != NULL) {
        if (!pObject->disable_move_rotate) {
            pObject->v.v[0] += pObject->field_0x54.v[0];
            pObject->v.v[1] += pObject->field_0x54.v[1];
            pObject->v.v[2] += pObject->field_0x54.v[2];
        }
        if (pObject->child != NULL) {
            PhysicsObjectMoveVelocityList(pObject->child);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004c29d0
void C2_HOOK_FASTCALL PhysicsObjectSetImpulse(tPhysics_object* pObject, br_vector3* pImpulse) {
    tPhysics_object* pChild;

    pObject->field_0x54 = *pImpulse;
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        PhysicsObjectSetImpulse(pChild, pImpulse);
    }
}

// FUNCTION: CARMA2_HW 0x004c2a10
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

// SetListDoingNothingFlag

// SetGroupDoingNothingFlag

// SetHierarchyDoingNothingFlag

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b9e40
void C2_HOOK_FASTCALL SetCollisionInfoDoNothingChain(tPhysics_object* pCollision_info, tU32 pDisable) {
    tPhysics_object* cur;
    tPhysics_object* sibling;

    pCollision_info->disable_move_rotate = pDisable;
    cur = pCollision_info;
    do {
        cur->disable_move_rotate = pDisable;
        cur->field_0x1df = 0;
        if (cur->child != NULL) {
            sibling = cur->child;
            while (sibling != NULL) {
                sibling->disable_move_rotate = pDisable;
                sibling->field_0x1df = 0;
                if (sibling->child != NULL) {
                    SetCollisionInfoDoNothing(sibling->child, pDisable);
                }
                sibling = sibling->next;
            }
        }
        cur = (tPhysics_object*)cur->field183_0x1d8;
    } while (cur != NULL && cur != pCollision_info);
}

// FUNCTION: CARMA2_HW 0x004b9eb0
void C2_HOOK_FASTCALL SetCollisionInfoDoNothing(tPhysics_object *pCollision_info, tU32 pDisable) {

    while (pCollision_info != NULL) {
        pCollision_info->disable_move_rotate = pDisable;
        pCollision_info->field_0x1df = 0;
        if (pCollision_info->child != NULL) {
            SetCollisionInfoDoNothing(pCollision_info->child, pDisable);
        }
        pCollision_info = pCollision_info->next;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004b9ef0
void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object *pCollision_info, tU32 pDisable) {
    tPhysics_object *sibling;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1df, 0x1df);

    pCollision_info->disable_move_rotate = pDisable;
    pCollision_info->field_0x1df = 0;
    if (pCollision_info->child != NULL) {
        sibling = pCollision_info->child;
        while (sibling != NULL) {
            sibling->disable_move_rotate = pDisable;
            sibling->field_0x1df = 0;
            if (sibling->child != NULL) {
                SetCollisionInfoDoNothing(sibling->child, pDisable);
            }
            sibling = sibling->next;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b9f40
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->box_face_ref = gFace_num__car - 2;
            pObject->disable_move_rotate = 0;
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

extern void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time);

extern double gDOUBLE_0058a798;

// FUNCTION: CARMA2_HW 0x004ba190
void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original) {
    tPhysics_object* list_cur;
    tPhysics_object* obj;
    tPhysics_object* child;

    for (list_cur = pList; list_cur != NULL; list_cur = list_cur->next) {
        if (list_cur->disable_move_rotate != 0) {
            if (pObject->field_0xf4.min.v[0] - list_cur->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                    && pObject->field_0xf4.min.v[1] - list_cur->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                    && pObject->field_0xf4.min.v[2] - list_cur->field_0xf4.max.v[2] <= gDOUBLE_0058a798
                    && list_cur->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                    && list_cur->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                    && list_cur->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] <= gDOUBLE_0058a798) {
                obj = list_cur;
                while (obj->parent != NULL) {
                    if (obj->physics_joint1 == NULL || obj->physics_joint1->type == eJoint_none) {
                        break;
                    }
                    obj = obj->parent;
                }
                obj->box_face_ref = gFace_num__car - 2;
                obj->disable_move_rotate = 0;
                if (obj->child != NULL || obj->physics_joint1 != NULL || obj->physics_joint2 != NULL) {
                    if (obj->field_0x218 == 0) {
                        MoveJointedObject(obj, 0.f);
                    }
                    child = obj->child;
                    while (child != NULL) {
                        if (child->disable_move_rotate) {
                            child->box_face_ref = gFace_num__car - 2;
                            child->disable_move_rotate = 0;
                        }
                        if (child->child != NULL) {
                            MakeObjectListDoSomething(child->child);
                        }
                        child = child->next;
                    }
                }
                CheckForObjectHierachyTouchingObjectList(obj, pList_original, pList_original);
            }
            child = pObject->child;
            if (child != NULL && list_cur->disable_move_rotate != 0) {
                child = pObject->child;
                while (child != NULL) {
                    if (child->field_0xf4.min.v[0] - list_cur->field_0xf4.max.v[0] > gDOUBLE_0058a798
                            || child->field_0xf4.min.v[1] - list_cur->field_0xf4.max.v[1] > gDOUBLE_0058a798
                            || child->field_0xf4.min.v[2] - list_cur->field_0xf4.max.v[2] > gDOUBLE_0058a798
                            || list_cur->field_0xf4.min.v[0] - child->field_0xf4.max.v[0] > gDOUBLE_0058a798
                            || list_cur->field_0xf4.min.v[1] - child->field_0xf4.max.v[1] > gDOUBLE_0058a798
                            || list_cur->field_0xf4.min.v[2] - child->field_0xf4.max.v[2] > gDOUBLE_0058a798) {
                        child = child->next;
                        if (child == NULL) {
                            break;
                        }
                        continue;
                    }
                    obj = list_cur;
                    while (obj->parent != NULL) {
                        if (obj->physics_joint1 == NULL || obj->physics_joint1->type == eJoint_none) {
                            break;
                        }
                        obj = obj->parent;
                    }
                    obj->box_face_ref = gFace_num__car - 2;
                    child = obj->child;
                    obj->disable_move_rotate = 0;
                    if (child != NULL || obj->physics_joint1 != NULL || obj->physics_joint2 != NULL) {
                        if (obj->field_0x218 == 0) {
                            MoveJointedObject(obj, 0.f);
                        }
                        child = obj->child;
                        while (child != NULL) {
                            if (child->disable_move_rotate) {
                                child->box_face_ref = gFace_num__car - 2;
                                child->disable_move_rotate = 0;
                            }
                            if (child->child != NULL) {
                                MakeObjectListDoSomething(child->child);
                            }
                            child = child->next;
                        }
                    }
                    CheckForObjectHierachyTouchingObjectList(obj, pList_original, pList_original);
                    break;
                }
            }
            if (list_cur->child != NULL) {
                CheckForObjectHierachyTouchingObjectList(pObject, list_cur->child, pList_original);
            }
        }
    }
}

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// SetObjectTrialPositionType

// FillInTrialWorkspace

// FUNCTION: CARMA2_HW 0x004ba4f0
int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2) {
    int not_go;

    if ((pObj2->field_0x1a4 & pObj1->field_0x1a0)
        || (pObj1->field_0x1a4 & pObj2->field_0x1a0)
        || (pObj1->flags & 4)
        || (pObj2->flags & 4)) {
        not_go = 1;
    } else {
        not_go = 0;
    }
    return not_go == 0;
}

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// ReduceEffectOfForces

// DoCollisions

// FUNCTION: CARMA2_HW 0x004baa00
int C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(tPhysics_object* pObject) {
    tPhysics_object* obj;
    int acc;
    int child_acc;
    float X;
    float Y;
    float Z;
    float eAng;
    float eLin;
    float len;
    float nx;
    float ny;
    float nz;
    float d1;
    float d2;
    float bx;
    float by;
    float bz;
    float cx;
    float cy;
    float cz;
    float vx;
    float vy;
    float vz;
    int i;
    int new1df;

    obj = pObject;
    acc = 1;
    if (obj != NULL) {
    for (; obj != NULL; obj = obj->next) {
        if (obj->field_0x21c != 1.0f) {
            obj->I.v[0] /= obj->field_0x21c;
            obj->I.v[1] /= obj->field_0x21c;
            obj->I.v[2] /= obj->field_0x21c;
        }
        if (obj->field_0xf0 != 0) {
            obj->field_0xf0 = 0;
            obj->disable_move_rotate = (tU8)(((tU32)obj->flags >> 5) & 1);
        }
        {
            tU32 f = obj->flags;
            f = (f & ~0x100) | ((f >> 1) & 0x100);
            obj->flags = f;
        }
        new1df = 0;
        if (obj->disable_move_rotate == 0) {
            if (obj->field_0x1df != 0 && obj->field183_0x1d8 == (int)obj) {
                obj->field_0x1df = 0;
            }
            X = ((br_vector3*)obj->field_0x1cc)->v[0] + obj->omega.v[0];
            Y = ((br_vector3*)obj->field_0x1cc)->v[1] + obj->omega.v[1];
            Z = ((br_vector3*)obj->field_0x1cc)->v[2] + obj->omega.v[2];
            eAng = (obj->I.v[0] * X * X + obj->I.v[1] * Y * Y + obj->I.v[2] * Z * Z) / obj->M;
            if (obj->collision_flag != 0 || obj->field_0x1df == 0) {
                eLin = obj->v.v[0] * obj->v.v[0] + obj->v.v[1] * obj->v.v[1] + obj->v.v[2] * obj->v.v[2];
            } else {
                eLin = gFLOAT_0058a768;
            }
            if (eLin < 0.001 && eAng < 0.00009) {
                len = (float)sqrt(obj->field_0x54.v[0] * obj->field_0x54.v[0] + obj->field_0x54.v[1] * obj->field_0x54.v[1] + obj->field_0x54.v[2] * obj->field_0x54.v[2]);
                nx = obj->field_0x54.v[0] / len;
                ny = obj->field_0x54.v[1] / len;
                nz = obj->field_0x54.v[2] / len;
                d1 = -(obj->v.v[0] * nx + obj->v.v[1] * ny + obj->v.v[2] * nz);
                d2 = -(((br_vector3*)obj->field_0x1b4)->v[0] * nx + ((br_vector3*)obj->field_0x1b4)->v[1] * ny + ((br_vector3*)obj->field_0x1b4)->v[2] * nz);
                if (obj->field_0x1df == 0) {
                    if (d1 < 0.012f && (d1 - d2) > 0.9f * len) {
                        bx = obj->omega.v[1] * obj->cmpos.v[2] - obj->omega.v[2] * obj->cmpos.v[1] + obj->velocity_car_space.v[0];
                        by = obj->omega.v[2] * obj->cmpos.v[0] - obj->omega.v[0] * obj->cmpos.v[2] + obj->velocity_car_space.v[1];
                        bz = obj->omega.v[0] * obj->cmpos.v[1] - obj->omega.v[1] * obj->cmpos.v[0] + obj->velocity_car_space.v[2];
                        for (i = 0; i < 8; i++) {
                            cx = (i & 2) ? obj->bb1.min.v[0] : obj->bb1.max.v[0];
                            cy = (i & 1) ? obj->bb1.min.v[1] : obj->bb1.max.v[1];
                            cz = (i & 4) ? obj->bb1.min.v[2] : obj->bb1.max.v[2];
                            vx = obj->omega.v[1] * cz - obj->omega.v[2] * cy + bx;
                            vy = obj->omega.v[2] * cx - obj->omega.v[0] * cz + by;
                            vz = obj->omega.v[0] * cy - obj->omega.v[1] * cx + bz;
                            if (vx * vx + vy * vy + vz * vz > 0.0004f) {
                                goto copy;
                            }
                        }
                    } else {
                        goto copy;
                    }
                }
                if (obj->physics_joint2 == NULL || obj->collision_flag != 0 || obj->transform_matrix.m[1][1] > 0.999) {
                    new1df = 1;
                    BrVector3SetFloat(&obj->v, 0.0f, 0.0f, 0.0f);
                    BrVector3SetFloat(&obj->omega, 0.0f, 0.0f, 0.0f);
                    BrVector3SetFloat(&obj->rotate_omega, 0.0f, 0.0f, 0.0f);
                }
            }
        }
copy:
        obj->field_0x1df = (tU8)new1df;
        BrMatrix34Copy(&obj->transform_matrix, &obj->actor->t.t.mat);
        BrMatrix34ApplyP(&obj->pos, &obj->cmpos, &obj->transform_matrix);
        if (obj->child != NULL) {
            child_acc = AddDoubleTorqueToMatrix(obj->child);
            if (obj->field_0x1df != 0) {
                obj->field_0x1df = (tU8)((child_acc << 1) + obj->field_0x1df);
            }
        } else {
            obj->field_0x1df = (tU8)(obj->field_0x1df * 3);
        }
        acc &= ((tU32)obj->field_0x1df >> 1) & 1;
    }
    }
    return acc;
}

// FUNCTION: CARMA2_HW 0x004bae80
void C2_HOOK_FASTCALL DRMatrix33Inverse(tPhysics_object* pObject, int pFlag) {
    tPhysics_object* sibling;

    pObject->field_0x1dc = (tU8)pFlag;
    for (sibling = pObject->child; sibling != NULL; sibling = sibling->next) {
        if (sibling->physics_joint1 != NULL && sibling->physics_joint1->type != 0) {
            DRMatrix33Inverse(sibling, pFlag);
        }
    }
}

// InitQuickHinge

// CalcQuickHingeData

// SetUpForce

// SetUpTorqueHingePartners

// SetUpHingePartners

// MakeMatrix

// FUNCTION: CARMA2_HW 0x004bd9b0
void C2_HOOK_FASTCALL SetFieldF0Tree(tPhysics_object* pObject, int pValue) {
    tPhysics_object* child;

    pObject->field_0xf0 = pValue;
    for (child = pObject->child; child != NULL; child = child->next) {
        SetFieldF0Tree(child, pValue);
    }
}

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

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

// NewAddFrictionObjectObject

// PARTIAL 28.92%: NewAddFriction 0x004c0ac0 - body is complete and follows the REA pseudocode
// statement by statement (both the two-object and the single-object branch, incl. the friction
// callback and the omega-cancelling early out); the remaining diff is x87 scheduling and stack-slot
// layout: the original keeps ~0x94 of frame (component-wise float locals) and schedules velocity_b
// before velocity_a, so no instruction past the prologue lines up. Known trap for the next attempt:
// CapOmega (0x4c1840) is defined above this function in this file, so MSVC inlines it three times
// here, while the original (defined after) always calls it - wrapping CapOmega's definition in
// #pragma auto_inline(off)/(on) removes the inlining but scored lower with reccmp (27.21%) as long
// as the rest of the body is not aligned, so it was reverted.
// based on the REA pseudocode for 0x004c0ac0 and ApexWorm/rec2 095fbd102 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004c0ac0
void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* pContact, tWorld_callbacks* pWorld_callbacks) {
    tPhysics_object* pObject_a;
    tPhysics_object* pObject_b;
    br_vector3 velocity_a;
    br_vector3 velocity_b;
    br_vector3 world;
    br_vector3 normal;
    br_vector3 direction_a;
    br_vector3 direction_b;
    br_vector3 angular_a;
    br_vector3 angular_b;
    br_vector3 cross;
    br_vector3 lever;
    br_scalar coefficient;
    br_scalar maximum;
    br_scalar speed;
    br_scalar inverse_speed;
    br_scalar denominator;
    br_scalar impulse;
    br_scalar scale;
    br_scalar dot;
    br_scalar base;
    br_scalar tangent_x;
    br_scalar tangent_y;
    br_scalar tangent_z;

    pObject_a = pContact->object_a;
    pObject_b = pContact->object_b;

    if (pObject_b != NULL && pContact->type != 14) {
        coefficient = pContact->field_0x0;
        pObject_a->collision_flag |= 2;
        pObject_b->collision_flag |= 2;

        velocity_a.v[0] = pContact->point_a.v[2] * pObject_a->omega.v[1] - pContact->point_a.v[1] * pObject_a->omega.v[2] + pObject_a->velocity_car_space.v[0];
        velocity_a.v[1] = pObject_a->omega.v[2] * pContact->point_a.v[0] - pObject_a->omega.v[0] * pContact->point_a.v[2] + pObject_a->velocity_car_space.v[1];
        velocity_a.v[2] = pObject_a->omega.v[0] * pContact->point_a.v[1] - pObject_a->omega.v[1] * pContact->point_a.v[0] + pObject_a->velocity_car_space.v[2];

        velocity_b.v[0] = pObject_b->omega.v[1] * pContact->point_b.v[2] - pObject_b->omega.v[2] * pContact->point_b.v[1] + pObject_b->velocity_car_space.v[0];
        velocity_b.v[1] = pContact->point_b.v[0] * pObject_b->omega.v[2] - pObject_b->omega.v[0] * pContact->point_b.v[2] + pObject_b->velocity_car_space.v[1];
        velocity_b.v[2] = pObject_b->omega.v[0] * pContact->point_b.v[1] - pObject_b->omega.v[1] * pContact->point_b.v[0] + pObject_b->velocity_car_space.v[2];

        BrMatrix34ApplyV(&world, &velocity_b, &pObject_b->actor->t.t.mat);
        BrMatrix34TApplyV(&velocity_b, &world, &pObject_a->actor->t.t.mat);
        velocity_a.v[0] -= velocity_b.v[0];
        velocity_a.v[1] -= velocity_b.v[1];
        velocity_a.v[2] -= velocity_b.v[2];

        BrMatrix34ApplyV(&world, &pContact->direction_a, &pObject_a->transform_matrix);
        BrMatrix34TApplyV(&normal, &world, &pObject_a->actor->t.t.mat);
        dot = normal.v[2] * velocity_a.v[2] + normal.v[1] * velocity_a.v[1] + normal.v[0] * velocity_a.v[0];
        world.v[0] = normal.v[0] * dot;
        world.v[1] = normal.v[1] * dot;
        world.v[2] = normal.v[2] * dot;
        tangent_x = velocity_a.v[0] - world.v[0];
        tangent_y = velocity_a.v[1] - world.v[1];
        tangent_z = velocity_a.v[2] - world.v[2];
        speed = (br_scalar)sqrt(tangent_z * tangent_z + tangent_y * tangent_y + tangent_x * tangent_x);
        base = coefficient * 0.35f;
        maximum = pObject_a->object_friction * base + base;
        maximum = pObject_b->object_friction * maximum + maximum;

        if (speed >= 0.001f) {
            inverse_speed = 1.0f / -speed;
            direction_a.v[0] = inverse_speed * tangent_x;
            direction_a.v[1] = inverse_speed * tangent_y;
            direction_a.v[2] = inverse_speed * tangent_z;
            BrMatrix34ApplyV(&world, &direction_a, &pObject_a->actor->t.t.mat);
            BrMatrix34TApplyV(&direction_b, &world, &pObject_b->actor->t.t.mat);
            if (!(pObject_a->flags & 0x800)) {
                angular_a.v[0] = (pContact->point_a.v[1] * direction_a.v[2] - pContact->point_a.v[2] * direction_a.v[1]) / pObject_a->I.v[0];
                angular_a.v[1] = (pContact->point_a.v[2] * direction_a.v[0] - pContact->point_a.v[0] * direction_a.v[2]) / pObject_a->I.v[1];
                angular_a.v[2] = (pContact->point_a.v[0] * direction_a.v[1] - pContact->point_a.v[1] * direction_a.v[0]) / pObject_a->I.v[2];
            } else {
                angular_a.v[0] = 0.0f;
                angular_a.v[1] = 0.0f;
                angular_a.v[2] = 0.0f;
            }
            if (!(pObject_b->flags & 0x800)) {
                angular_b.v[0] = (pContact->point_b.v[1] * direction_b.v[2] - pContact->point_b.v[2] * direction_b.v[1]) / pObject_b->I.v[0];
                angular_b.v[1] = (pContact->point_b.v[2] * direction_b.v[0] - pContact->point_b.v[0] * direction_b.v[2]) / pObject_b->I.v[1];
                angular_b.v[2] = (pContact->point_b.v[0] * direction_b.v[1] - pContact->point_b.v[1] * direction_b.v[0]) / pObject_b->I.v[2];
            } else {
                angular_b.v[0] = 0.0f;
                angular_b.v[1] = 0.0f;
                angular_b.v[2] = 0.0f;
            }
            world.v[0] = pContact->point_b.v[2] * angular_b.v[1] - pContact->point_b.v[1] * angular_b.v[2];
            world.v[1] = pContact->point_b.v[0] * angular_b.v[2] - pContact->point_b.v[2] * angular_b.v[0];
            world.v[2] = pContact->point_b.v[1] * angular_b.v[0] - pContact->point_b.v[0] * angular_b.v[1];
            base = (pContact->point_a.v[0] * angular_a.v[2] - pContact->point_a.v[2] * angular_a.v[0]) * direction_a.v[1]
                + (pContact->point_a.v[2] * angular_a.v[1] - pContact->point_a.v[1] * angular_a.v[2]) * direction_a.v[0]
                + (pContact->point_a.v[1] * angular_a.v[0] - pContact->point_a.v[0] * angular_a.v[1]) * direction_a.v[2]
                + 1.0f / pObject_a->M + 1.0f / pObject_b->M;
            denominator = direction_b.v[0] * world.v[0] + direction_b.v[1] * world.v[1] + direction_b.v[2] * world.v[2] + base;
            if (0.0001f < fabs(denominator)) {
                impulse = speed / denominator;
                if (maximum < impulse) {
                    impulse = maximum;
                }
                scale = impulse / pObject_a->M;
                direction_a.v[0] = direction_a.v[0] * scale;
                direction_a.v[1] = direction_a.v[1] * scale;
                direction_a.v[2] = direction_a.v[2] * scale;
                scale = -(impulse / pObject_b->M);
                direction_b.v[0] = direction_b.v[0] * scale;
                direction_b.v[1] = direction_b.v[1] * scale;
                direction_b.v[2] = direction_b.v[2] * scale;
                pContact->field_0x44.v[0] = pObject_a->M * direction_a.v[0];
                pContact->field_0x44.v[1] = pObject_a->M * direction_a.v[1];
                pContact->field_0x44.v[2] = pObject_a->M * direction_a.v[2];
                pContact->field_0x50.v[0] = pObject_b->M * direction_b.v[0];
                pContact->field_0x50.v[1] = pObject_b->M * direction_b.v[1];
                pContact->field_0x50.v[2] = pObject_b->M * direction_b.v[2];
                pContact->type |= 0x100;
                pObject_a->omega.v[0] += impulse * angular_a.v[0];
                pObject_a->omega.v[1] += impulse * angular_a.v[1];
                pObject_a->omega.v[2] += impulse * angular_a.v[2];
                pObject_b->omega.v[0] += -impulse * angular_b.v[0];
                pObject_b->omega.v[1] += -impulse * angular_b.v[1];
                pObject_b->omega.v[2] += -impulse * angular_b.v[2];
                BrMatrix34ApplyV(&world, &direction_a, &pObject_a->actor->t.t.mat);
                pObject_a->v.v[0] += world.v[0];
                pObject_a->v.v[1] += world.v[1];
                pObject_a->v.v[2] += world.v[2];
                pObject_a->velocity_car_space.v[0] += direction_a.v[0];
                pObject_a->velocity_car_space.v[1] += direction_a.v[1];
                pObject_a->velocity_car_space.v[2] += direction_a.v[2];
                BrMatrix34ApplyV(&world, &direction_b, &pObject_b->actor->t.t.mat);
                pObject_b->v.v[0] += world.v[0];
                pObject_b->v.v[1] += world.v[1];
                pObject_b->v.v[2] += world.v[2];
                pObject_b->velocity_car_space.v[0] += direction_b.v[0];
                pObject_b->velocity_car_space.v[1] += direction_b.v[1];
                pObject_b->velocity_car_space.v[2] += direction_b.v[2];
            }
        }
        CapOmega(pObject_a);
        CapOmega(pObject_b);
        return;
    }

    coefficient = pContact->field_0x0;
    pObject_a->collision_flag |= 1;
    if (pContact->type == 1) {
        lever.v[0] = pContact->point_a.v[0] * -0.005f + pContact->point_a.v[0];
        lever.v[1] = pContact->point_a.v[1] * -0.005f + pContact->point_a.v[1];
        lever.v[2] = pContact->point_a.v[2] * -0.005f + pContact->point_a.v[2];
        velocity_a.v[0] = pObject_a->omega.v[1] * lever.v[2] - pObject_a->omega.v[2] * lever.v[1];
        velocity_a.v[1] = pObject_a->omega.v[2] * lever.v[0] - pObject_a->omega.v[0] * lever.v[2];
        velocity_a.v[2] = pObject_a->omega.v[0] * lever.v[1] - pObject_a->omega.v[1] * lever.v[0];
    } else {
        velocity_a.v[0] = pContact->point_a.v[2] * pObject_a->omega.v[1] - pObject_a->omega.v[2] * pContact->point_a.v[1];
        velocity_a.v[1] = pContact->point_a.v[0] * pObject_a->omega.v[2] - pObject_a->omega.v[0] * pContact->point_a.v[2];
        velocity_a.v[2] = pContact->point_a.v[1] * pObject_a->omega.v[0] - pObject_a->omega.v[1] * pContact->point_a.v[0];
    }
    velocity_a.v[0] += pObject_a->velocity_car_space.v[0];
    velocity_a.v[1] += pObject_a->velocity_car_space.v[1];
    velocity_a.v[2] += pObject_a->velocity_car_space.v[2];
    BrMatrix34ApplyV(&world, &pContact->direction_a, &pObject_a->transform_matrix);
    BrMatrix34TApplyV(&normal, &world, &pObject_a->actor->t.t.mat);
    dot = normal.v[2] * velocity_a.v[2] + normal.v[1] * velocity_a.v[1] + normal.v[0] * velocity_a.v[0];
    world.v[0] = dot * normal.v[0];
    world.v[1] = dot * normal.v[1];
    world.v[2] = dot * normal.v[2];
    velocity_a.v[0] -= world.v[0];
    velocity_a.v[1] -= world.v[1];
    velocity_a.v[2] -= world.v[2];
    if (pWorld_callbacks != NULL && pWorld_callbacks->get_friction_from_face != NULL && pContact->field_0xc != NULL) {
        coefficient = pWorld_callbacks->get_friction_from_face(pContact->field_0xc) * coefficient;
    }
    speed = (br_scalar)sqrt(velocity_a.v[2] * velocity_a.v[2] + velocity_a.v[1] * velocity_a.v[1] + velocity_a.v[0] * velocity_a.v[0]);
    base = coefficient * 0.35f;
    maximum = pObject_a->world_friction * base + base;
    if (speed >= 0.001f) {
        inverse_speed = 1.0f / -speed;
        direction_a.v[0] = inverse_speed * velocity_a.v[0];
        direction_a.v[1] = inverse_speed * velocity_a.v[1];
        direction_a.v[2] = inverse_speed * velocity_a.v[2];
        if (!(pObject_a->flags & 0x800)) {
            angular_a.v[0] = (pContact->point_a.v[1] * direction_a.v[2] - pContact->point_a.v[2] * direction_a.v[1]) / pObject_a->I.v[0];
            angular_a.v[1] = (pContact->point_a.v[2] * direction_a.v[0] - pContact->point_a.v[0] * direction_a.v[2]) / pObject_a->I.v[1];
            angular_a.v[2] = (pContact->point_a.v[0] * direction_a.v[1] - pContact->point_a.v[1] * direction_a.v[0]) / pObject_a->I.v[2];
        } else {
            angular_a.v[0] = 0.0f;
            angular_a.v[1] = 0.0f;
            angular_a.v[2] = 0.0f;
        }
        dot = pObject_a->omega.v[0] * pContact->direction_a.v[0] + pContact->direction_a.v[2] * pObject_a->omega.v[2] + pObject_a->omega.v[1] * pContact->direction_a.v[1];
        if (fabs(dot) < 0.1f
                && pObject_a->omega.v[0] * angular_a.v[0] + pObject_a->omega.v[2] * angular_a.v[2] + pObject_a->omega.v[1] * angular_a.v[1] < 0.0f
                && pObject_a->v.v[2] * pObject_a->v.v[2] + pObject_a->v.v[1] * pObject_a->v.v[1] + pObject_a->v.v[0] * pObject_a->v.v[0] < 0.001f) {
            world.v[0] = pContact->direction_a.v[0] * dot;
            world.v[1] = pContact->direction_a.v[1] * dot;
            world.v[2] = pContact->direction_a.v[2] * dot;
            pObject_a->omega.v[0] -= world.v[0];
            pObject_a->omega.v[1] -= world.v[1];
            pObject_a->omega.v[2] -= world.v[2];
            BrVector3SetFloat(&angular_a, 0.0f, 0.0f, 0.0f);
            direction_a.v[0] = pObject_a->velocity_car_space.v[0];
            direction_a.v[1] = pObject_a->velocity_car_space.v[1];
            direction_a.v[2] = pObject_a->velocity_car_space.v[2];
            speed = (br_scalar)sqrt(direction_a.v[2] * direction_a.v[2] + direction_a.v[1] * direction_a.v[1] + direction_a.v[0] * direction_a.v[0]);
            if (speed < 0.001f) {
                return;
            }
            inverse_speed = -speed;
            direction_a.v[0] = direction_a.v[0] / inverse_speed;
            direction_a.v[1] = direction_a.v[1] / inverse_speed;
            direction_a.v[2] = direction_a.v[2] / inverse_speed;
        }
        cross.v[0] = pContact->point_a.v[2] * angular_a.v[1] - pContact->point_a.v[1] * angular_a.v[2];
        cross.v[1] = pContact->point_a.v[0] * angular_a.v[2] - pContact->point_a.v[2] * angular_a.v[0];
        cross.v[2] = pContact->point_a.v[1] * angular_a.v[0] - pContact->point_a.v[0] * angular_a.v[1];
        denominator = direction_a.v[2] * cross.v[2] + direction_a.v[1] * cross.v[1] + direction_a.v[0] * cross.v[0] + 1.0f / pObject_a->M;
        if (0.0001f < fabs(denominator)) {
            impulse = speed / denominator;
            if (maximum < impulse) {
                impulse = maximum;
            }
            angular_a.v[0] = impulse * angular_a.v[0];
            angular_a.v[1] = impulse * angular_a.v[1];
            angular_a.v[2] = impulse * angular_a.v[2];
            scale = impulse / pObject_a->M;
            direction_a.v[0] = direction_a.v[0] * scale;
            direction_a.v[1] = direction_a.v[1] * scale;
            direction_a.v[2] = direction_a.v[2] * scale;
            pContact->field_0x44.v[0] = pObject_a->M * direction_a.v[0];
            pContact->field_0x44.v[1] = pObject_a->M * direction_a.v[1];
            pContact->field_0x44.v[2] = pObject_a->M * direction_a.v[2];
            pContact->type |= 0x100;
            pObject_a->omega.v[0] += angular_a.v[0];
            pObject_a->omega.v[1] += angular_a.v[1];
            pObject_a->omega.v[2] += angular_a.v[2];
            BrMatrix34ApplyV(&world, &direction_a, &pObject_a->actor->t.t.mat);
            pObject_a->v.v[0] += world.v[0];
            pObject_a->v.v[1] += world.v[1];
            pObject_a->v.v[2] += world.v[2];
            pObject_a->velocity_car_space.v[0] += direction_a.v[0];
            pObject_a->velocity_car_space.v[1] += direction_a.v[1];
            pObject_a->velocity_car_space.v[2] += direction_a.v[2];
            CapOmega(pObject_a);
        }
    }
}

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

// FUNCTION: CARMA2_HW 0x004c1f10
void C2_HOOK_FAKE_THISCALL RotateObjectFirstOrder(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis;
    br_vector3 momentum;
    br_vector3 rotated;
    br_scalar length;
    br_scalar angle;
    br_scalar m1;

    length = BrVector3Length(&pObject->omega);
    angle = length * pTime;
    if (angle < 0.0001) {
        return;
    }
    axis.v[0] = pObject->omega.v[0] / length;
    axis.v[1] = pObject->omega.v[1] / length;
    axis.v[2] = pObject->omega.v[2] / length;
    momentum.v[0] = pObject->I.v[0] * pObject->omega.v[0];
    m1 = pObject->omega.v[1];
    momentum.v[1] = m1 * pObject->I.v[1];
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

extern tNet_mode gNet_mode;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

// FUNCTION: CARMA2_HW 0x0049c000
void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation) {
    br_quat quaternion;
    br_scalar x;
    br_scalar y;
    br_scalar z;
    br_scalar w;

    x = (br_scalar)pOrientation[0] * 1.5259255e-05f;
    y = (br_scalar)pOrientation[1] * 1.5259255e-05f;
    z = (br_scalar)pOrientation[2] * 1.5259255e-05f;
    w = (br_scalar)pOrientation[3] * 1.5259255e-05f;
    quaternion.x = (x + x) - 1.0f;
    quaternion.y = (y + y) - 1.0f;
    quaternion.z = (z + z) - 1.0f;
    quaternion.w = (w + w) - 1.0f;
    BrQuatNormalise(&quaternion, &quaternion);
    BrQuatToMatrix34(pMatrix, &quaternion);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// and ApexWorm 6afd11ad1
// FUNCTION: CARMA2_HW 0x004c1be0
void C2_HOOK_FAKE_THISCALL RotateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pDelta_time) {
    br_matrix34 rotation;
    br_vector3 axis;
    br_vector3 momentum;
    br_vector3 rotated;
    tFixed_quat packed_rotation;
    br_vector3 translation;
    br_scalar rad_rate;
    br_scalar rad;
    br_scalar rad_squared;
    int steps;
    int i;

    (void)pUnused;
    pObject->rotate_omega.v[0] = pObject->omega.v[0];
    rad_squared = pDelta_time * pDelta_time * BrVector3LengthSquared(&pObject->omega);
    pObject->rotate_omega.v[1] = pObject->omega.v[1];
    pObject->rotate_omega.v[2] = pObject->omega.v[2];
    if (rad_squared < 0.0000001) {
        return;
    }
    if (rad_squared > 0.008f) {
        steps = (int)sqrt(rad_squared * 31.25f) + 1;
        pDelta_time /= steps;
        for (i = 0; i < steps && i < 400; i++) {
            rad_rate = BrVector3Length(&pObject->omega);
            axis.v[0] = pObject->omega.v[0] / rad_rate;
            axis.v[1] = pObject->omega.v[1] / rad_rate;
            axis.v[2] = pObject->omega.v[2] / rad_rate;
            momentum.v[0] = pObject->omega.v[0] * pObject->I.v[0];
            momentum.v[1] = pObject->omega.v[1] * pObject->I.v[1];
            momentum.v[2] = pObject->omega.v[2] * pObject->I.v[2];
            rad = rad_rate * pDelta_time;
            BrMatrix34Rotate(&rotation, (br_angle)(BrRadianToAngle(rad) >> 1), &axis);
            BrMatrix34TApplyV(&rotated, &momentum, &rotation);
            axis.v[0] = rotated.v[0] / pObject->I.v[0];
            axis.v[1] = rotated.v[1] / pObject->I.v[1];
            axis.v[2] = rotated.v[2] / pObject->I.v[2];
            rad_rate = BrVector3Length(&axis);
            axis.v[0] = axis.v[0] / rad_rate;
            axis.v[1] = axis.v[1] / rad_rate;
            axis.v[2] = axis.v[2] / rad_rate;
            rad = rad_rate * pDelta_time;
            BrMatrix34Rotate(&rotation, BrRadianToAngle(rad), &axis);
            BrMatrix34PreTranslate(&rotation, -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
            BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
            BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
            BrMatrix34TApplyV(&rotated, &momentum, &rotation);
            pObject->omega.v[0] = rotated.v[0] / pObject->I.v[0];
            pObject->omega.v[1] = rotated.v[1] / pObject->I.v[1];
            pObject->omega.v[2] = rotated.v[2] / pObject->I.v[2];
        }
    } else {
        RotateObjectFirstOrder(pObject, pDelta_time, pDelta_time);
    }
    if (gNet_mode != 0) {
        GetOrientationFromMatrix(&packed_rotation, &pObject->actor->t.t.mat);
        translation.v[0] = pObject->actor->t.t.mat.m[3][0];
        translation.v[1] = pObject->actor->t.t.mat.m[3][1];
        translation.v[2] = pObject->actor->t.t.mat.m[3][2];
        GetMatrixFromOrientation(&pObject->actor->t.t.mat, packed_rotation.v);
        pObject->actor->t.t.mat.m[3][0] = translation.v[0];
        pObject->actor->t.t.mat.m[3][1] = translation.v[1];
        pObject->actor->t.t.mat.m[3][2] = translation.v[2];
    }
}

// (reference)
// FUNCTION: CARMA2_HW 0x004c18d0
void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject) {
    tPhysics_joint* joint;
    tPhysics_object* child;
    br_vector3 axis;
    br_vector3 tmp[3];
    br_scalar imp[3];
    br_scalar friction;
    br_scalar friction_sq;
    br_scalar sum_sq;
    br_scalar num;
    br_scalar denom;
    br_scalar scale;
    int i;

    joint = pObject->physics_joint1;
    if (joint != NULL && joint->friction != 0.0f) {
        if (pObject->field_0x230 != NULL) {
            br_vector3* src = (br_vector3*)&pObject->transform_matrix.m[0][0];
            float* impPtr = imp;
            br_vector3* tmpPtr = tmp;
            float* iPtr = &pObject->I.v[0];
            float* oPtr = &pObject->omega.v[0];
            int k = 3;
            friction = joint->friction * 0.04f;
            sum_sq = 0.0f;
            friction_sq = friction * friction;
            do {
                float t;
                float* tp = &tmpPtr->v[0];
                float* ap = &axis.v[0];
                float* ip = &pObject->field_0x230->I.v[0];
                int n = 3;
                BrMatrix34TApplyV(&axis, src, &pObject->field_0x230->transform_matrix);
                num = pObject->field_0x230->omega.v[0] * axis.v[0] + pObject->field_0x230->omega.v[2] * axis.v[2] + pObject->field_0x230->omega.v[1] * axis.v[1] - *oPtr;
                do {
                    *tp = *ap / *ip;
                    tp++;
                    ap++;
                    ip++;
                    n--;
                } while (n != 0);
                denom = 1.0 / *iPtr + (tmpPtr->v[0] * axis.v[0] + tmpPtr->v[2] * axis.v[2] + tmpPtr->v[1] * axis.v[1]);
                t = num / denom;
                sum_sq += t * t;
                *impPtr = t;
                src++;
                impPtr++;
                tmpPtr++;
                iPtr++;
                oPtr++;
                k--;
            } while (k != 0);
            if (friction_sq < sum_sq) {
                scale = sqrt(friction_sq / sum_sq);
                imp[0] *= scale;
                imp[1] *= scale;
                imp[2] *= scale;
            }
            for (i = 0; i < 3; i++) {
                pObject->omega.v[i] += imp[i] / pObject->I.v[i];
                pObject->field_0x230->omega.v[0] -= imp[i] * tmp[i].v[0];
                pObject->field_0x230->omega.v[1] -= imp[i] * tmp[i].v[1];
                pObject->field_0x230->omega.v[2] -= imp[i] * tmp[i].v[2];
            }
        }
    }

    joint = pObject->physics_joint2;
    if (joint != NULL && joint->friction != 0.0f) {
        float* b = &pObject->I.v[0];
        friction = joint->friction * 0.04f;
        friction_sq = friction * friction;
        sum_sq = 0.0f;
        for (i = 0; i < 3; i++) {
            float u = b[0] * b[27];
            sum_sq += u * u;
            imp[i] = u;
            b++;
        }
        if (sum_sq > friction_sq) {
            scale = sqrt(friction_sq / sum_sq);
            imp[0] *= scale;
            imp[1] *= scale;
            imp[2] *= scale;
        }
        for (i = 0; i < 3; i++) {
            pObject->omega.v[i] -= imp[i] / pObject->I.v[i];
        }
    }

    for (child = pObject->child; child != NULL; child = child->next) {
        if (child->disable_move_rotate == 0) {
            TranslateObject(child);
        }
    }
}


// GetFacesNearObject

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on ApexWorm/rec2 a2bbacecc (reference only)
// FUNCTION: CARMA2_HW 0x004c25a0
void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject) {
    tPhysics_object* child;

    if (pObject->field_0xf0 == 1) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, (const br_matrix34*)&pObject->field_0xbc);
    } else {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        ResetObjectAndChildren(child);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c2600
void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects) {
    tPhysics_object* object;
    tPhysics_object* child;

    for (object = pObjects; object != NULL; object = object->next) {
        if (object->field_0xf0 == 1) {
            BrMatrix34Copy(&object->actor->t.t.mat, (const br_matrix34*)&object->field_0xbc);
        } else {
            BrMatrix34LPNormalise(&object->actor->t.t.mat, &object->transform_matrix);
        }
        for (child = object->child; child != NULL; child = child->next) {
            ResetObjectAndChildren(child);
        }
    }
}

// RepositionObjectGroup

// SimpleRotate

// (reference only)
// FUNCTION: CARMA2_HW 0x004c2670
void C2_HOOK_FAKE_THISCALL InterpolateSingleObject(tPhysics_object* pObject, br_scalar pDt) {
    br_vector3 temp;
    br_vector3 axis;
    br_scalar length;
    br_scalar angle;

    if (pObject->disable_move_rotate) {
        return;
    }
    length = BrVector3Length(&pObject->rotate_omega);
    angle = length * pDt;
    axis.v[0] = pObject->rotate_omega.v[0] / length;
    axis.v[1] = pObject->rotate_omega.v[1] / length;
    axis.v[2] = pObject->rotate_omega.v[2] / length;
    if (fabs(angle) < 0.0001) {
        return;
    }
    BrMatrix34ApplyV(&temp, &pObject->cmpos, &pObject->actor->t.t.mat);
    pObject->actor->t.t.mat.m[3][0] += temp.v[0];
    pObject->actor->t.t.mat.m[3][1] += temp.v[1];
    pObject->actor->t.t.mat.m[3][2] += temp.v[2];
    BrMatrix34PreRotate(&pObject->actor->t.t.mat, BrRadianToAngle(angle), &axis);
    BrMatrix34ApplyV(&temp, &pObject->cmpos, &pObject->actor->t.t.mat);
    pObject->actor->t.t.mat.m[3][0] -= temp.v[0];
    pObject->actor->t.t.mat.m[3][1] -= temp.v[1];
    pObject->actor->t.t.mat.m[3][2] -= temp.v[2];
}

// (reference only)
// FUNCTION: CARMA2_HW 0x004c27c0
void C2_HOOK_FAKE_THISCALL InterpolateObjects(tPhysics_object* pObject, float pDt) {
    br_vector3 omega_world;

    pObject->rotate_omega.v[0] = pObject->omega.v[0];
    pObject->rotate_omega.v[1] = pObject->omega.v[1];
    pObject->rotate_omega.v[2] = pObject->omega.v[2];
    BrMatrix34ApplyV(&omega_world, &pObject->omega, &pObject->actor->t.t.mat);
    InterpolateSingleObject(pObject, pDt);
    BrMatrix34TApplyV(&pObject->omega, &omega_world, &pObject->actor->t.t.mat);
}

// ApplyGravitySingleObject

// ApplyGravity

// SetGravityDeltaV

// TrialPositionChildren

// FUNCTION: CARMA2_HW 0x004c2b20
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
void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info) {

    InternalPositionChildren(pCollision_info, pCollision_info);
}

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on ApexWorm/rec2 af10f5a01
// FUNCTION: CARMA2_HW 0x004c5830
// FUNCTION: CARMA2_SW 0x004c1f60
int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pSize, br_scalar* pOrthogonal) {
    int i;
    int j;
    int k;
    int singular;
    br_scalar scale;
    br_scalar sum;
    br_scalar sigma;
    br_scalar divisor;
    br_scalar matrix_sum;
    br_scalar orthogonal_sum;
    br_scalar tau;
    br_scalar tau2;
    br_scalar* row;

    singular = 0;
    for (i = 0; i < pSize; i++) {
        for (j = 0; j < pSize; j++) {
            pOrthogonal[i * pSize + j] = 0.f;
        }
        pOrthogonal[i * pSize + i] = 1.f;
    }
    for (i = 0; i < pSize; i++) {
        row = &pMatrix[i * pSize];
        scale = 0.f;
        for (j = i; j < pSize; j++) {
            scale = MAX(scale, fabs(row[j]));
        }
        if (scale == 0.0) {
            singular = 1;
        } else {
            for (j = i; j < pSize; j++) {
                row[j] /= scale;
            }
            sum = 0.f;
            for (j = i; j < pSize; j++) {
                sum += row[j] * row[j];
            }
            sigma = row[i] > 0.0 ? fabs(sqrt(sum)) : -fabs(sqrt(sum));
            divisor = sigma * (sigma + row[i]);
            row[i] += sigma;
            for (j = 0; j <= i; j++) {
                sum = 0.f;
                for (k = i; k < pSize; k++) {
                    sum += row[k] * pOrthogonal[k * pSize + j];
                }
                tau = sum / divisor;
                for (k = i; k < pSize; k++) {
                    pOrthogonal[k * pSize + j] -= tau * row[k];
                }
            }
            for (j = i + 1; j < pSize; j++) {
                matrix_sum = 0.f;
                for (k = i; k < pSize; k++) {
                    matrix_sum += pMatrix[j * pSize + k] * row[k];
                }
                tau2 = matrix_sum / divisor;
                orthogonal_sum = 0.f;
                for (k = i; k < pSize; k++) {
                    orthogonal_sum += pOrthogonal[k * pSize + j] * pMatrix[i * pSize + k];
                }
                tau = orthogonal_sum / divisor;
                for (k = i; k < pSize; k++) {
                    pMatrix[j * pSize + k] -= tau2 * row[k];
                    pOrthogonal[k * pSize + j] -= tau * row[k];
                }
            }
            row[i] = -(sigma * scale);
            for (j = i + 1; j < pSize; j++) {
                row[j] = 0.f;
            }
        }
    }
    return singular;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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
     * - all matrices (A, Q, R) are of size N x N (with N = pDim)
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

// GetPhysicsScratchPointer

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
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    result = BrMemAllocate(sizeof(tCollision_shape_sphere), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5d70
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

void C2_HOOK_FASTCALL FreeThingForm(void* pData) {

    BrMemFree(pData);
}

// FUNCTION: CARMA2_HW 0x004c5e80
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// FUNCTION: CARMA2_HW 0x004c5eb0
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

// FillInShape

// UpdateCollisionObject

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c61c0
void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {

    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }
    exit(pError);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c61e0
void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage) {
    (void)pMessage;
}

// FUNCTION: CARMA2_HW 0x004c61f0
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn) {

    gPhysics_error_cb = pError_cbfn;
}

// RepositionAndOrientateChildren

// FUNCTION: CARMA2_HW 0x004c63b0
void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
}

// PhysicsAddObjectList

// PhysicsRemoveObject

// FUNCTION: CARMA2_HW 0x004c63d0
void C2_HOOK_FASTCALL AddCollisionInfoChild(tPhysics_object* pParent, tPhysics_object* pChild) {
    tPhysics_object* current;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->parent = pParent;
    current = pChild;
    while (current->next != NULL) {
        current = current->next;
        current->parent = pParent;
    }
    current->next = pParent->child;
    pParent->child = pChild;
}

// FUNCTION: CARMA2_HW 0x004c6410
void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject) {
    tPhysics_object* parent;
    tPhysics_object* walk;

    parent = pObject->parent;
    walk = parent->child;
    if (walk == pObject) {
        parent->child = pObject->next;
    } else {
        while (walk->next != pObject) {
            walk = walk->next;
        }
        walk->next = pObject->next;
    }
    pObject->parent = NULL;
    pObject->next = NULL;
}

// FUNCTION: CARMA2_HW 0x004c6470
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
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {

    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        /* the original inlines PhysicsError here (no call) */
        if (gPhysics_error_cb != NULL) {
            gPhysics_error_cb(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
        }
        exit(ePhysicsError_InsufficientSizedBuffer);
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

// FUNCTION: CARMA2_HW 0x004b5aa0
void C2_HOOK_FASTCALL SetUpTestObjects(void) {

}

// FUNCTION: CARMA2_HW 0x004b5c00
void C2_HOOK_FASTCALL ResetMan2(tPhysics_object* pObject) {

    while (pObject != NULL) {
        BrMatrix34Translate(&pObject->transform_matrix, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->omega, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->rotate_omega, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->velocity_car_space, 0.f, 0.f, 0.f);
        pObject->disable_move_rotate = 1;
        if (pObject->child != NULL) {
            ResetMan2(pObject->child);
        }
        pObject = pObject->next;
    }
}

// ResetObjectList

// ResetMan

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004b5ca0
void C2_HOOK_FASTCALL InitPhysics(void) {

    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

extern int C2_HOOK_FASTCALL FUN_004b9650(tPhysics_object* pObject, tPhysics_object* pObject2);
extern void* C2_HOOK_FASTCALL FUN_004c5c70(int pSize);
extern int C2_HOOK_FASTCALL FUN_004b8350(tPhysics_object* pObject, tPhysics_joint_force* pForce);
extern void C2_HOOK_FASTCALL FUN_004b8cf0(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL FUN_004be550(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, const br_vector3* pDirection, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004be4b0(br_scalar* pOut1, br_scalar* pOut2, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL FUN_004becf0(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004bf490(br_scalar* pMatrix, int pCount, tPhysics_joint_force** pRecords, int pCount2);
extern void C2_HOOK_FASTCALL FUN_004b77b0(br_scalar* pVelocity, tPhysics_joint_force** pRecords, int pCount);
extern void C2_HOOK_FASTCALL FUN_004c53b0(br_scalar* pMatrix, br_scalar* pOrthogonal, br_scalar* pVelocity, br_scalar* pImpulses, int pCount);
extern br_scalar C2_HOOK_FASTCALL FUN_004b7290(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004b7960(tPhysics_joint_force* pForce, br_scalar pImpulse);
extern void C2_HOOK_FASTCALL FUN_004b8ee0(tPhysics_object* pObject, float pDelta_time);
extern void C2_HOOK_FASTCALL FUN_004b8fe0(tPhysics_object* pObject, float pDelta_time);
extern void C2_HOOK_FASTCALL FUN_004b9230(tPhysics_object* pObject, tPhysics_object** ppChanged_object, float pDelta_time);
extern void C2_HOOK_FASTCALL FUN_004be350(float* pDestination, const float* pSource);

typedef struct tPhysics_joint_force_group {
    int count;
    tPhysics_joint_force* records;
    undefined4 field_0x8[3];
    br_scalar* orthogonal;
    br_scalar* matrix;
} tPhysics_joint_force_group;

// GLOBAL: CARMA2_HW 0x0058a768
br_scalar gFLOAT_0058a768 = 0.0f;

// PARTIAL 49.06%: PhysicsObjectWeightedSquareSum 0x004b7250 - body is carpa's 1.000 source token for token; residual is the x87 schedule of the 3-iteration loop (VC5 here squares omega first and materialises the omega copy through esi/mov instead of the original's fld+fst, so the frame is 8 vs 4 and ~15 of 26 instructions differ); reordering the terms or adding a br_scalar temp for omega is canonicalised or scores lower (45.28)
// FUNCTION: CARMA2_HW 0x004b7250
br_scalar C2_HOOK_FASTCALL PhysicsObjectWeightedSquareSum(tPhysics_object* pObject) {
    br_scalar sum;
    int i;

    sum = gFLOAT_0058a768;
    for (i = 0; i < 3; i++) {
        sum += pObject->M * (pObject->v.v[i] * pObject->v.v[i]) + pObject->I.v[i] * (pObject->omega.v[i] * pObject->omega.v[i]);
    }
    return sum;
}

// PARTIAL 55.81%: FUN_004b7290 0x004b7290 - body is the inlined PhysicsObjectWeightedSquareSum as in the original; remaining diff is the x87 schedule of the inner loop (ours squares omega before v and spills omega through a GP register instead of fst) and a 0xc vs 8 frame
// FUNCTION: CARMA2_HW 0x004b7290
br_scalar C2_HOOK_FASTCALL FUN_004b7290(tPhysics_object* pObject) {
    br_scalar sum;
    br_scalar object_sum;

    sum = 0.0f;
    while (pObject != NULL) {
        object_sum = PhysicsObjectWeightedSquareSum(pObject);
        sum += object_sum;
        sum += FUN_004b7290(pObject->child);
        pObject = pObject->next;
    }
    return sum;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b7510
void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time) {
    br_scalar energy;
    br_scalar total;
    tPhysics_joint_force* records[40];
    br_scalar velocity[100];
    br_scalar impulses[100];
    tPhysics_joint_force_group* joint_forces;
    tPhysics_joint_force* record;
    tPhysics_object* obj;
    int count;
    int actual_count;
    int i;

    count = FUN_004b9650(pObject, NULL);
    if (count != 0) {
        joint_forces = (tPhysics_joint_force_group*)FUN_004c5c70((count + 12) * count * 8 + 28);
        IncreasePhysicsScratchPointer((count + 12) * count * 8 + 28);
        joint_forces->count = count;
        joint_forces->records = (tPhysics_joint_force*)((tU8*)joint_forces + 28);
        joint_forces->orthogonal = (br_scalar*)(joint_forces->records + count);
        joint_forces->matrix = joint_forces->orthogonal + count * count;
        actual_count = FUN_004b8350(pObject, joint_forces->records);
        pObject->field_0x218 = (undefined4)joint_forces;
        record = joint_forces->records;
        for (i = 0; i < actual_count; i++) {
            records[i] = record++;
        }
        FUN_004bf490(joint_forces->matrix, actual_count, records, actual_count);
        FUN_004b77b0(velocity, records, actual_count);
        FUN_004c53b0(joint_forces->matrix, joint_forces->orthogonal, velocity, impulses, actual_count);
        if (pDelta_time == gFLOAT_0058a768) {
            return;
        }
        total = gFLOAT_0058a768;
        for (obj = pObject; obj != NULL; obj = obj->next) {
            energy = gFLOAT_0058a768;
            for (i = 0; i < 3; i++) {
                energy += obj->v.v[i] * obj->v.v[i] * obj->M + obj->omega.v[i] * obj->omega.v[i] * obj->I.v[i];
            }
            total = FUN_004b7290(obj->child) + energy + total;
        }
        for (i = 0; i < actual_count; i++) {
            FUN_004b7960(records[i], impulses[i]);
        }
        total = gFLOAT_0058a768;
        for (obj = pObject; obj != NULL; obj = obj->next) {
            energy = gFLOAT_0058a768;
            for (i = 0; i < 3; i++) {
                energy += obj->v.v[i] * obj->v.v[i] * obj->M + obj->omega.v[i] * obj->omega.v[i] * obj->I.v[i];
            }
            total = FUN_004b7290(obj->child) + energy + total;
        }
        if (pDelta_time > gFLOAT_0058a768) {
            FUN_004b8ee0(pObject, pDelta_time);
        }
        return;
    }
    if (pObject->child != NULL) {
        FUN_004b8ee0(pObject, pDelta_time);
        return;
    }
    pObject->actor->t.t.mat.m[3][0] = pObject->v.v[0] * pDelta_time + pObject->actor->t.t.mat.m[3][0];
    pObject->actor->t.t.mat.m[3][1] = pObject->v.v[1] * pDelta_time + pObject->actor->t.t.mat.m[3][1];
    pObject->actor->t.t.mat.m[3][2] = pObject->v.v[2] * pDelta_time + pObject->actor->t.t.mat.m[3][2];
    RotateObject(pObject, *(undefined4*)&pDelta_time, pDelta_time);
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006793d0
int gUNK_006793d0;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b9650
int C2_HOOK_FASTCALL FUN_004b9650(tPhysics_object* pObject, tPhysics_object* pParent_context) {
    tPhysics_joint* joint;
    tPhysics_object* child;
    tPhysics_object* child_parent_context;
    volatile int pObject_value;
    int result;

    pObject_value = (int)pObject;
    result = 0;
    joint = pObject->physics_joint1;
    pObject->field_0x230 = pParent_context;
    *(int*)((tU8*)pObject + 0x1e4) = 0;
    if (joint != NULL && (pObject->flags & 0x400) == 0) {
        switch (joint->type) {
        case eJoint_none:
            break;
        case eJoint_universal:
            result = 4;
            break;
        case eJoint_ball_n_socket:
            result = 3;
            break;
        case eJoint_quick_hinge:
            if (*(int*)((tU8*)pParent_context + 0x1e4) == 0) {
                result = 2;
                *(int*)((tU8*)pObject + 0x1e4) = -1;
                *(int*)((tU8*)pParent_context + 0x1e4) = -1;
                break;
            }
            joint->type = eJoint_hinge;
            /* fall through */
        case eJoint_hinge:
            result = 5;
            break;
        default:
            PhysicsError((tPhysicsError)8, "");
            result = pObject_value;
            break;
        }
    }
    if (pObject->physics_joint2 != NULL) {
        switch (pObject->physics_joint2->type) {
        case eJoint_universal:
            result += 4;
            break;
        case eJoint_ball_n_socket:
            result += 3;
            break;
        case eJoint_translation:
            result += 2;
            break;
        default:
            break;
        }
    }
    child_parent_context = pParent_context;
    if ((pObject->flags & 0x400) == 0) {
        child_parent_context = pObject;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        result += FUN_004b9650(child, child_parent_context);
    }
    return result;
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c5c70
void* C2_HOOK_FASTCALL FUN_004c5c70(int pSize) {

    if (pSize + (tU32)gPhysics_scratch_ptr > (tU32)gPhysics_other_buffer + (tU32)gPhysics_other_buffer_capacity) {
        /* the original inlines PhysicsError here (no call) */
        if (gPhysics_error_cb != NULL) {
            gPhysics_error_cb(ePhysicsError_OutOfMemory, NULL);
        }
        exit(ePhysicsError_OutOfMemory);
    }
    return gPhysics_scratch_ptr;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b8350
int C2_HOOK_FASTCALL FUN_004b8350(tPhysics_object* pObject, tPhysics_joint_force* pForce) {
    tPhysics_object* other;
    tPhysics_object* child;
    float* axis_component;
    float local_54[9];
    br_matrix34 inverse;
    tU8* workspace;
    int count;
    int local_58;
    int child_count;
    int i;

    count = 0;
    if (pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none && (pObject->flags & 0x400) == 0) {
        other = pObject->field_0x230;
        if (pObject->physics_joint1->type == eJoint_quick_hinge) {
            CreateJointForce(pObject, other, pForce, &pObject->physics_joint1->hinge_axis2);
            CreateJointForce(pObject, other, pForce + 1, &pObject->physics_joint1->hinge_axis3);
            pForce += 2;
            count = 2;
        } else {
            axis_component = local_54;
            local_58 = 3;
            do {
                local_54[0] = 0.f;
                local_54[1] = 0.f;
                local_54[2] = 0.f;
                *axis_component = 1.f;
                CreateJointForce(pObject, other, pForce, (br_vector3*)local_54);
                pForce++;
                axis_component++;
                local_58--;
            } while (local_58 != 0);
            count = 3;
        }
        switch (pObject->physics_joint1->type) {
        case eJoint_hinge:
            pForce->field_0x0 = 0.f;
            pForce->object_a = pObject;
            pForce->object_b = other;
            pForce->point_a.v[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            pForce->point_a.v[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            pForce->point_a.v[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            pForce->point_b.v[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            pForce->point_b.v[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            pForce->point_b.v[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            pForce->direction_a.v[0] = pObject->physics_joint1->hinge_axis2.v[0];
            pForce->direction_a.v[1] = pObject->physics_joint1->hinge_axis2.v[1];
            pForce->direction_a.v[2] = pObject->physics_joint1->hinge_axis2.v[2];
            BrMatrix34ApplyV((br_vector3*)local_54, &pForce->direction_a, &pObject->transform_matrix);
            BrMatrix34TApplyV(&pForce->direction_b, (br_vector3*)local_54, &other->transform_matrix);
            pForce->type = 0xb;
            pForce++;

            pForce->field_0x0 = 0.f;
            pForce->object_a = pObject;
            pForce->object_b = other;
            pForce->point_a.v[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            pForce->point_a.v[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            pForce->point_a.v[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            pForce->point_b.v[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            pForce->point_b.v[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            pForce->point_b.v[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            pForce->direction_a.v[0] = pObject->physics_joint1->hinge_axis3.v[0];
            pForce->direction_a.v[1] = pObject->physics_joint1->hinge_axis3.v[1];
            pForce->direction_a.v[2] = pObject->physics_joint1->hinge_axis3.v[2];
            BrMatrix34ApplyV((br_vector3*)local_54, &pForce->direction_a, &pObject->transform_matrix);
            BrMatrix34TApplyV(&pForce->direction_b, (br_vector3*)local_54, &other->transform_matrix);
            pForce->type = 0xb;
            pForce++;
            count += 2;
            break;

        case eJoint_universal:
            pForce->field_0x0 = 0.f;
            pForce->object_a = pObject;
            pForce->object_b = other;
            pForce->point_a.v[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            pForce->point_a.v[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            pForce->point_a.v[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            pForce->point_b.v[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            pForce->point_b.v[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            pForce->point_b.v[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            pForce->direction_a.v[0] = pObject->physics_joint1->hinge_axis2.v[0];
            pForce->direction_a.v[1] = pObject->physics_joint1->hinge_axis2.v[1];
            pForce->direction_a.v[2] = pObject->physics_joint1->hinge_axis2.v[2];
            BrMatrix34ApplyV((br_vector3*)local_54, &pForce->direction_a, &pObject->transform_matrix);
            BrMatrix34TApplyV(&pForce->direction_b, (br_vector3*)local_54, &other->transform_matrix);
            pForce->type = 0xb;
            pForce++;
            count++;
            break;

        case eJoint_ball_n_socket:
            break;

        case eJoint_quick_hinge:
            workspace = (tU8*)FUN_004c5c70(0xd0);
            IncreasePhysicsScratchPointer(0xd0);
            *(int*)((tU8*)pObject + 0x1e4) = (int)workspace;
            *(int*)((tU8*)other + 0x1e4) = (int)(workspace + 0x68);
            *(int*)workspace = 0;
            *(tPhysics_object**)(workspace + 4) = other;
            *(int*)(workspace + 0x68) = 0;
            *(tPhysics_object**)(workspace + 0x6c) = pObject;
            BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
            BrMatrix34Mul((br_matrix34*)(workspace + 0x38), &other->transform_matrix, &inverse);
            BrMatrix34LPInverse((br_matrix34*)(workspace + 0xa0), (br_matrix34*)(workspace + 0x38));
            *(int*)workspace = 1;
            *(int*)(workspace + 0x68) = 1;
            FUN_004b8cf0(pObject);

            local_54[0] = pObject->physics_joint1->parent_bone_axis.v[1] * other->omega.v[1]
                + other->omega.v[0] * pObject->physics_joint1->parent_bone_axis.v[0]
                + pObject->physics_joint1->parent_bone_axis.v[2] * other->omega.v[2];
            local_54[3] = other->omega.v[0] - pObject->physics_joint1->parent_bone_axis.v[0] * local_54[0];
            local_54[4] = other->omega.v[1] - pObject->physics_joint1->parent_bone_axis.v[1] * local_54[0];
            local_54[5] = other->omega.v[2] - pObject->physics_joint1->parent_bone_axis.v[2] * local_54[0];
            BrMatrix34ApplyV((br_vector3*)local_54, (br_vector3*)(local_54 + 3), &other->transform_matrix);
            BrMatrix34TApplyV((br_vector3*)(local_54 + 6), (br_vector3*)local_54, &pObject->transform_matrix);

            local_54[0] = pObject->physics_joint1->hinge_axis.v[1] * pObject->omega.v[1]
                + pObject->physics_joint1->hinge_axis.v[2] * pObject->omega.v[2]
                + pObject->physics_joint1->hinge_axis.v[0] * pObject->omega.v[0];
            pObject->omega.v[0] = pObject->physics_joint1->hinge_axis.v[0] * local_54[0];
            pObject->omega.v[1] = pObject->physics_joint1->hinge_axis.v[1] * local_54[0];
            pObject->omega.v[0] = local_54[6] + pObject->omega.v[0];
            pObject->omega.v[1] = local_54[7] + pObject->omega.v[1];
            pObject->omega.v[2] = pObject->physics_joint1->hinge_axis.v[2] * local_54[0];
            pObject->omega.v[2] = local_54[8] + pObject->omega.v[2];

            local_54[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            local_54[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            local_54[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            local_54[3] = other->omega.v[1] * local_54[2] - other->omega.v[2] * local_54[1];
            local_54[4] = other->omega.v[2] * local_54[0] - other->omega.v[0] * local_54[2];
            local_54[5] = other->omega.v[0] * local_54[1] - other->omega.v[1] * local_54[0];
            BrMatrix34ApplyV((br_vector3*)local_54, (br_vector3*)(local_54 + 3), &other->transform_matrix);
            local_54[0] = other->v.v[0] + local_54[0];
            local_54[1] = other->v.v[1] + local_54[1];
            local_54[2] = other->v.v[2] + local_54[2];
            BrMatrix34TApplyV((br_vector3*)(local_54 + 6), (br_vector3*)local_54, &pObject->transform_matrix);

            local_54[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            local_54[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            local_54[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            local_54[3] = pObject->omega.v[1] * local_54[2] - pObject->omega.v[2] * local_54[1];
            local_54[4] = pObject->omega.v[2] * local_54[0] - pObject->omega.v[0] * local_54[2];
            local_54[5] = pObject->omega.v[0] * local_54[1] - pObject->omega.v[1] * local_54[0];
            local_54[6] = local_54[6] - local_54[3];
            local_54[7] = local_54[7] - local_54[4];
            local_54[8] = local_54[8] - local_54[5];
            local_54[2] = (pObject->physics_joint1->hinge_axis.v[1] * local_54[7]
                + pObject->physics_joint1->hinge_axis.v[0] * local_54[6]
                + pObject->physics_joint1->hinge_axis.v[2] * local_54[8])
                - (pObject->physics_joint1->hinge_axis.v[1] * pObject->velocity_car_space.v[1]
                + pObject->physics_joint1->hinge_axis.v[0] * pObject->velocity_car_space.v[0]
                + pObject->physics_joint1->hinge_axis.v[2] * pObject->velocity_car_space.v[2]);
            local_54[0] = pObject->physics_joint1->hinge_axis.v[0] * local_54[2];
            local_54[1] = pObject->physics_joint1->hinge_axis.v[1] * local_54[2];
            local_54[2] = pObject->physics_joint1->hinge_axis.v[2] * local_54[2];
            pObject->velocity_car_space.v[0] = local_54[0] + pObject->velocity_car_space.v[0];
            pObject->velocity_car_space.v[1] = local_54[1] + pObject->velocity_car_space.v[1];
            pObject->velocity_car_space.v[2] = local_54[2] + pObject->velocity_car_space.v[2];
            BrMatrix34ApplyV(&pObject->v, &pObject->velocity_car_space, &pObject->transform_matrix);
            break;

        default:
            PhysicsError(ePhysicsError_UnknownHingeType, NULL);
            break;
        }
    }

    if (pObject->physics_joint2 != NULL) {
        if (pObject->physics_joint2->type == eJoint_translation) {
            pForce->field_0x0 = 0.f;
            pForce->object_a = pObject;
            pForce->object_b = NULL;
            pForce->field_0xc = NULL;
            pForce->direction_a.v[0] = pObject->physics_joint2->hinge_axis2.v[0];
            pForce->direction_a.v[1] = pObject->physics_joint2->hinge_axis2.v[1];
            pForce->direction_a.v[2] = pObject->physics_joint2->hinge_axis2.v[2];
            pForce->type = 10;
            pForce++;
            count++;

            pForce->field_0x0 = 0.f;
            pForce->object_a = pObject;
            pForce->object_b = NULL;
            pForce->field_0xc = NULL;
            pForce->direction_a.v[0] = pObject->physics_joint2->hinge_axis3.v[0];
            pForce->direction_a.v[1] = pObject->physics_joint2->hinge_axis3.v[1];
            pForce->direction_a.v[2] = pObject->physics_joint2->hinge_axis3.v[2];
            pForce->type = 10;
            pForce++;
            count++;
        } else {
            count += 3;
            for (i = 0; i < 3; i++) {
                pForce->field_0x0 = 0.f;
                pForce->object_a = pObject;
                pForce->object_b = NULL;
                pForce->field_0xc = NULL;
                pForce->point_a.v[0] = pObject->physics_joint2->field_0x08.v[0] - pObject->cmpos.v[0];
                pForce->point_a.v[1] = pObject->physics_joint2->field_0x08.v[1] - pObject->cmpos.v[1];
                pForce->point_a.v[2] = pObject->physics_joint2->field_0x08.v[2] - pObject->cmpos.v[2];
                pForce->direction_a.v[0] = 0.f;
                pForce->direction_a.v[1] = 0.f;
                pForce->direction_a.v[2] = 0.f;
                pForce->direction_a.v[i] = 1.f;
                pForce->type = 10;
                pForce++;
            }
            switch (pObject->physics_joint2->type) {
            case eJoint_hinge:
            case eJoint_ball_n_socket:
                break;

            case eJoint_universal:
                pForce->field_0x0 = 0.f;
                pForce->object_a = pObject;
                pForce->object_b = NULL;
                pForce->field_0xc = NULL;
                pForce->point_a.v[0] = pObject->physics_joint2->field_0x08.v[0] - pObject->cmpos.v[0];
                pForce->point_a.v[1] = pObject->physics_joint2->field_0x08.v[1] - pObject->cmpos.v[1];
                pForce->point_a.v[2] = pObject->physics_joint2->field_0x08.v[2] - pObject->cmpos.v[2];
                pForce->direction_a.v[0] = pObject->physics_joint2->hinge_axis2.v[0];
                pForce->direction_a.v[1] = pObject->physics_joint2->hinge_axis2.v[1];
                pForce->direction_a.v[2] = pObject->physics_joint2->hinge_axis2.v[2];
                pForce->type = 0xb;
                pForce++;
                count++;
                break;

            default:
                PhysicsError(ePhysicsError_UnknownHingeType, NULL);
                break;
            }
        }
    }

    for (child = pObject->child; child != NULL; child = child->next) {
        child_count = FUN_004b8350(child, pForce);
        count += child_count;
        pForce += child_count;
    }
    return count;
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004bf490
void C2_HOOK_FASTCALL FUN_004bf490(br_scalar* pMatrix, int pStride, tPhysics_joint_force** pRecords, int pCount) {
  /* Keep each four-byte stack slot addressable for contiguous vector outputs. */
  union { int i[60]; float f[60]; } local_storage;
#define local_e0 local_storage.i[4]
#define local_dc local_storage.f[5]
#define local_d8 local_storage.f[6]
#define local_d4 local_storage.f[7]
#define local_d0 local_storage.f[8]
#define local_cc local_storage.f[9]
#define local_c8 local_storage.f[10]
#define local_c4 local_storage.f[11]
#define local_c0 local_storage.f[12]
#define local_bc local_storage.f[13]
#define local_b8 local_storage.f[14]
#define local_b4 local_storage.f[15]
#define local_b0 local_storage.f[16]
#define local_ac local_storage.f[17]
#define local_a8 local_storage.f[18]
#define local_a4 local_storage.f[19]
#define local_a0 local_storage.f[20]
#define local_9c local_storage.f[21]
#define local_98 local_storage.f[22]
#define local_94 local_storage.f[23]
#define local_90 local_storage.f[24]
#define local_8c local_storage.f[25]
#define local_88 local_storage.f[26]
#define local_84 local_storage.f[27]
#define local_80 local_storage.f[28]
#define local_7c local_storage.i[29]
#define local_78 local_storage.i[30]
#define local_74 local_storage.i[31]
#define local_70 local_storage.i[32]
#define local_6c local_storage.i[33]
#define local_68 local_storage.i[34]
#define local_64 local_storage.i[35]
#define local_60 local_storage.f[36]
#define local_5c local_storage.f[37]
#define local_58 local_storage.f[38]
#define local_54 local_storage.f[39]
#define local_50 local_storage.f[40]
#define local_4c local_storage.f[41]
#define local_48 local_storage.f[42]
#define local_44 local_storage.f[43]
#define local_40 local_storage.f[44]
#define local_3c local_storage.f[45]
#define local_38 local_storage.f[46]
#define local_34 local_storage.f[47]
#define local_30 local_storage.f[48]
#define local_2c local_storage.f[49]
#define local_28 local_storage.f[50]
#define local_24 local_storage.f[51]
#define local_20 local_storage.f[52]
#define local_1c local_storage.f[53]
#define local_18 local_storage.f[54]
#define local_14 local_storage.f[55]
#define local_10 local_storage.f[56]
#define local_c local_storage.f[57]
#define local_8 local_storage.f[58]
#define local_4 local_storage.f[59]
  int iVar1;
  float fVar2;
  int iVar3;
  float *pfVar4;
  int *piVar5;
  float *pfVar6;
  int iVar7;
  int iVar8;
  int iVar9;

  local_78 = pStride;
  local_64 = (int)pMatrix;
  local_74 = 0;
  if (0 < pCount) {
    do {
      iVar7 = local_74;
      iVar1 = ((int*)pRecords)[local_74];
      iVar9 = *(int *)(iVar1 + 8);
      iVar8 = *(int *)(iVar1 + 4);
      if ((iVar9 != 0) && (*(int *)(iVar1 + 0x5c) == 0xe)) {
        iVar9 = 0;
      }
      local_7c = iVar8;
      if (*(int *)(iVar1 + 0x5c) == 0xb) {
        iVar9 = *(int *)(iVar1 + 4);
        iVar8 = *(int *)(iVar1 + 8);
        iVar3 = (int)FUN_004be550(&local_c4, &local_48, &local_94, &local_30, (const br_vector3*)(iVar1 + 0x2c), (tPhysics_object*)iVar9);
        if (iVar8 != 0) {
          local_6c = (int)FUN_004be550(&local_a0, &local_18, &local_b8, &local_3c, (const br_vector3*)(iVar1 + 0x38), (tPhysics_object*)iVar8);
        }
        if (-1 < iVar7) {
          pfVar6 = (float *)(local_64 + iVar7 * local_78 * 4);
          pfVar4 = (float *)(local_64 + iVar7 * 4);
          local_7c = iVar7 + 1;
          piVar5 = (int*)pRecords;
          do {
            iVar1 = *piVar5;
            if (*(int *)(iVar1 + 0x5c) == 0xb) {
              if (*(int *)(iVar1 + 4) == iVar9) {
                *pfVar4 = *(float *)(iVar1 + 0x2c) * local_c4 +
                          *(float *)(iVar1 + 0x30) * local_c0 + *(float *)(iVar1 + 0x34) * local_bc;
              }
              else if (*(int *)(iVar1 + 8) == iVar9) {
                *pfVar4 = -(*(float *)(iVar1 + 0x40) * local_bc +
                           *(float *)(iVar1 + 0x38) * local_c4 + *(float *)(iVar1 + 0x3c) * local_c0
                           );
              }
              else {
                *pfVar4 = 0.0;
              }
              if (iVar3 != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == iVar3) {
                  fVar2 = *(float *)(iVar1 + 0x2c) * local_94 +
                          *(float *)(iVar1 + 0x30) * local_90 + *(float *)(iVar1 + 0x34) * local_8c
                          + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != iVar3) goto LAB_004bf646;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x40) * local_8c +
                                    *(float *)(iVar1 + 0x38) * local_94 +
                                    *(float *)(iVar1 + 0x3c) * local_90);
                }
                *pfVar4 = fVar2;
              }
LAB_004bf646:
              if (iVar8 != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == iVar8) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x2c) * local_a0 +
                                    *(float *)(iVar1 + 0x30) * local_9c +
                                    *(float *)(iVar1 + 0x34) * local_98);
LAB_004bf6a6:
                  *pfVar4 = fVar2;
                }
                else if (*(int *)(iVar1 + 8) == iVar8) {
                  fVar2 = *(float *)(iVar1 + 0x40) * local_98 +
                          *(float *)(iVar1 + 0x38) * local_a0 + *(float *)(iVar1 + 0x3c) * local_9c
                          + *pfVar4;
                  goto LAB_004bf6a6;
                }
                if (local_6c != 0) {
                  iVar1 = *piVar5;
                  if (*(int *)(iVar1 + 4) == local_6c) {
                    *pfVar4 = *pfVar4 - (*(float *)(iVar1 + 0x2c) * local_b8 +
                                        *(float *)(iVar1 + 0x30) * local_b4 +
                                        *(float *)(iVar1 + 0x34) * local_b0);
                  }
                  else if (*(int *)(iVar1 + 8) == local_6c) {
                    *pfVar4 = *(float *)(iVar1 + 0x40) * local_b0 +
                              *(float *)(iVar1 + 0x38) * local_b8 +
                              *(float *)(iVar1 + 0x3c) * local_b4 + *pfVar4;
                  }
                }
              }
            }
            else {
              if (*(int *)(iVar1 + 4) == iVar9) {
                *pfVar4 = *(float *)(iVar1 + 0x2c) *
                          ((*(float *)(iVar1 + 0x1c) * local_c0 -
                           *(float *)(iVar1 + 0x18) * local_bc) + local_48) +
                          *(float *)(iVar1 + 0x34) *
                          ((*(float *)(iVar1 + 0x18) * local_c4 -
                           *(float *)(iVar1 + 0x14) * local_c0) + local_40) +
                          *(float *)(iVar1 + 0x30) *
                          ((*(float *)(iVar1 + 0x14) * local_bc -
                           *(float *)(iVar1 + 0x1c) * local_c4) + local_44);
              }
              else if (*(int *)(iVar1 + 8) == iVar9) {
                *pfVar4 = -(*(float *)(iVar1 + 0x40) *
                            ((*(float *)(iVar1 + 0x24) * local_c4 -
                             *(float *)(iVar1 + 0x20) * local_c0) + local_40) +
                           *(float *)(iVar1 + 0x3c) *
                           ((*(float *)(iVar1 + 0x20) * local_bc -
                            *(float *)(iVar1 + 0x28) * local_c4) + local_44) +
                           *(float *)(iVar1 + 0x38) *
                           ((*(float *)(iVar1 + 0x28) * local_c0 -
                            *(float *)(iVar1 + 0x24) * local_bc) + local_48));
              }
              else {
                *pfVar4 = 0.0;
              }
              if (iVar3 != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == iVar3) {
                  fVar2 = *(float *)(iVar1 + 0x2c) *
                          ((*(float *)(iVar1 + 0x1c) * local_90 -
                           *(float *)(iVar1 + 0x18) * local_8c) + local_30) +
                          *(float *)(iVar1 + 0x34) *
                          ((*(float *)(iVar1 + 0x18) * local_94 -
                           *(float *)(iVar1 + 0x14) * local_90) + local_28) +
                          *(float *)(iVar1 + 0x30) *
                          ((*(float *)(iVar1 + 0x14) * local_8c -
                           *(float *)(iVar1 + 0x1c) * local_94) + local_2c) + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != iVar3) goto LAB_004bf981;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x40) *
                                     ((*(float *)(iVar1 + 0x24) * local_94 -
                                      *(float *)(iVar1 + 0x20) * local_90) + local_28) +
                                    *(float *)(iVar1 + 0x3c) *
                                    ((*(float *)(iVar1 + 0x20) * local_8c -
                                     *(float *)(iVar1 + 0x28) * local_94) + local_2c) +
                                    *(float *)(iVar1 + 0x38) *
                                    ((*(float *)(iVar1 + 0x28) * local_90 -
                                     *(float *)(iVar1 + 0x24) * local_8c) + local_30));
                }
                *pfVar4 = fVar2;
              }
LAB_004bf981:
              if (iVar8 == 0) goto LAB_004bfbea;
              iVar1 = *piVar5;
              if (*(int *)(iVar1 + 4) == iVar8) {
                fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x2c) *
                                   ((*(float *)(iVar1 + 0x1c) * local_9c -
                                    *(float *)(iVar1 + 0x18) * local_98) + local_18) +
                                  *(float *)(iVar1 + 0x34) *
                                  ((*(float *)(iVar1 + 0x18) * local_a0 -
                                   *(float *)(iVar1 + 0x14) * local_9c) + local_10) +
                                  *(float *)(iVar1 + 0x30) *
                                  ((*(float *)(iVar1 + 0x14) * local_98 -
                                   *(float *)(iVar1 + 0x1c) * local_a0) + local_14));
LAB_004bfaae:
                *pfVar4 = fVar2;
              }
              else if (*(int *)(iVar1 + 8) == iVar8) {
                fVar2 = *(float *)(iVar1 + 0x40) *
                        ((*(float *)(iVar1 + 0x24) * local_a0 - *(float *)(iVar1 + 0x20) * local_9c)
                        + local_10) +
                        *(float *)(iVar1 + 0x3c) *
                        ((*(float *)(iVar1 + 0x20) * local_98 - *(float *)(iVar1 + 0x28) * local_a0)
                        + local_14) +
                        *(float *)(iVar1 + 0x38) *
                        ((*(float *)(iVar1 + 0x28) * local_9c - *(float *)(iVar1 + 0x24) * local_98)
                        + local_18) + *pfVar4;
                goto LAB_004bfaae;
              }
              if (local_6c != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == local_6c) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x2c) *
                                     ((*(float *)(iVar1 + 0x1c) * local_b4 -
                                      *(float *)(iVar1 + 0x18) * local_b0) + local_3c) +
                                    *(float *)(iVar1 + 0x34) *
                                    ((*(float *)(iVar1 + 0x18) * local_b8 -
                                     *(float *)(iVar1 + 0x14) * local_b4) + local_34) +
                                    *(float *)(iVar1 + 0x30) *
                                    ((*(float *)(iVar1 + 0x14) * local_b0 -
                                     *(float *)(iVar1 + 0x1c) * local_b8) + local_38));
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_6c) goto LAB_004bfbea;
                  fVar2 = *(float *)(iVar1 + 0x40) *
                          ((*(float *)(iVar1 + 0x24) * local_b8 -
                           *(float *)(iVar1 + 0x20) * local_b4) + local_34) +
                          *(float *)(iVar1 + 0x3c) *
                          ((*(float *)(iVar1 + 0x20) * local_b0 -
                           *(float *)(iVar1 + 0x28) * local_b8) + local_38) +
                          *(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_b4 -
                           *(float *)(iVar1 + 0x24) * local_b0) + local_3c) + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
            }
LAB_004bfbea:
            piVar5 = piVar5 + 1;
            *pfVar6 = *pfVar4;
            pfVar4 = pfVar4 + local_78;
            pfVar6 = pfVar6 + 1;
            local_7c = local_7c + -1;
            iVar7 = local_74;
          } while (local_7c != 0);
        }
      }
      else {
        FUN_004be4b0(&local_d0, &local_60, (const br_vector3*)(iVar1 + 0x14), (const br_vector3*)(iVar1 + 0x2c), (tPhysics_object*)iVar8);
        if (*(int *)(iVar8 + 0x1e4) == 0) {
          local_e0 = 0;
        }
        else {
          local_e0 = (int)FUN_004becf0(&local_d0, &local_60, &local_ac, &local_24, (tPhysics_object*)iVar8);
        }
        iVar8 = 0;
        local_68 = 0;
        if ((iVar9 != 0) &&
           (FUN_004be4b0(&local_dc, &local_54, (const br_vector3*)(iVar1 + 0x20), (const br_vector3*)(iVar1 + 0x38), (tPhysics_object*)iVar9), *(int *)(iVar9 + 0x1e4) != 0)) {
          iVar8 = (int)FUN_004becf0(&local_dc, &local_54, &local_88, &local_c, (tPhysics_object*)iVar9);
          local_68 = iVar8;
        }
        if (-1 < iVar7) {
          pfVar6 = (float *)(local_64 + local_74 * local_78 * 4);
          pfVar4 = (float *)(local_64 + local_74 * 4);
          local_70 = local_74 + 1;
          piVar5 = (int*)pRecords;
          do {
            *pfVar4 = 0.0;
            iVar1 = *piVar5;
            if (*(int *)(iVar1 + 0x5c) == 0xb) {
              if (*(int *)(iVar1 + 4) == local_7c) {
                fVar2 = *(float *)(iVar1 + 0x34) * local_c8 +
                        *(float *)(iVar1 + 0x30) * local_cc + *(float *)(iVar1 + 0x2c) * local_d0;
LAB_004bfd67:
                *pfVar4 = fVar2;
              }
              else if (*(int *)(iVar1 + 8) == local_7c) {
                fVar2 = -(*(float *)(iVar1 + 0x3c) * local_cc +
                         *(float *)(iVar1 + 0x38) * local_d0 + *(float *)(iVar1 + 0x40) * local_c8);
                goto LAB_004bfd67;
              }
              if (iVar9 != 0) {
                if (*(int *)(iVar1 + 4) == iVar9) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x34) * local_d4 +
                                    *(float *)(iVar1 + 0x30) * local_d8 +
                                    *(float *)(iVar1 + 0x2c) * local_dc);
                }
                else {
                  if (*(int *)(iVar1 + 8) != iVar9) goto LAB_004bfdc5;
                  fVar2 = *(float *)(iVar1 + 0x3c) * local_d8 +
                          *(float *)(iVar1 + 0x38) * local_dc + *(float *)(iVar1 + 0x40) * local_d4
                          + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
LAB_004bfdc5:
              if (local_e0 != 0) {
                if (*(int *)(iVar1 + 4) == local_e0) {
                  fVar2 = *(float *)(iVar1 + 0x34) * local_a4 +
                          *(float *)(iVar1 + 0x30) * local_a8 + *(float *)(iVar1 + 0x2c) * local_ac
                          + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_e0) goto LAB_004bfe25;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x3c) * local_a8 +
                                    *(float *)(iVar1 + 0x38) * local_ac +
                                    *(float *)(iVar1 + 0x40) * local_a4);
                }
                *pfVar4 = fVar2;
              }
LAB_004bfe25:
              if (iVar8 != 0) {
                if (*(int *)(iVar1 + 4) == iVar8) {
                  *pfVar4 = *pfVar4 - (*(float *)(iVar1 + 0x34) * local_80 +
                                      *(float *)(iVar1 + 0x30) * local_84 +
                                      *(float *)(iVar1 + 0x2c) * local_88);
                }
                else if (*(int *)(iVar1 + 8) == iVar8) {
                  *pfVar4 = *(float *)(iVar1 + 0x3c) * local_84 +
                            *(float *)(iVar1 + 0x38) * local_88 +
                            *(float *)(iVar1 + 0x40) * local_80 + *pfVar4;
                }
              }
            }
            else {
              if (*(int *)(iVar1 + 4) == local_7c) {
                fVar2 = *(float *)(iVar1 + 0x30) *
                        ((*(float *)(iVar1 + 0x14) * local_c8 - *(float *)(iVar1 + 0x1c) * local_d0)
                        + local_5c) +
                        *(float *)(iVar1 + 0x34) *
                        ((*(float *)(iVar1 + 0x18) * local_d0 - *(float *)(iVar1 + 0x14) * local_cc)
                        + local_58) +
                        *(float *)(iVar1 + 0x2c) *
                        ((*(float *)(iVar1 + 0x1c) * local_cc - *(float *)(iVar1 + 0x18) * local_c8)
                        + local_60);
LAB_004bffd0:
                *pfVar4 = fVar2;
              }
              else if ((*(int *)(iVar1 + 8) == local_7c) && (*(int *)(iVar1 + 0x5c) != 0xe)) {
                fVar2 = -(*(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_cc -
                           *(float *)(iVar1 + 0x24) * local_c8) + local_60) +
                         *(float *)(iVar1 + 0x3c) *
                         ((*(float *)(iVar1 + 0x20) * local_c8 - *(float *)(iVar1 + 0x28) * local_d0
                          ) + local_5c) +
                         *(float *)(iVar1 + 0x40) *
                         ((*(float *)(iVar1 + 0x24) * local_d0 - *(float *)(iVar1 + 0x20) * local_cc
                          ) + local_58));
                goto LAB_004bffd0;
              }
              if (iVar9 != 0) {
                if (*(int *)(iVar1 + 4) == iVar9) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x30) *
                                     ((*(float *)(iVar1 + 0x14) * local_d4 -
                                      *(float *)(iVar1 + 0x1c) * local_dc) + local_50) +
                                    *(float *)(iVar1 + 0x34) *
                                    ((*(float *)(iVar1 + 0x18) * local_dc -
                                     *(float *)(iVar1 + 0x14) * local_d8) + local_4c) +
                                    *(float *)(iVar1 + 0x2c) *
                                    ((*(float *)(iVar1 + 0x1c) * local_d8 -
                                     *(float *)(iVar1 + 0x18) * local_d4) + local_54));
                }
                else {
                  if ((*(int *)(iVar1 + 8) != iVar9) || (*(int *)(iVar1 + 0x5c) == 0xe))
                  goto LAB_004c0107;
                  fVar2 = *(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_d8 -
                           *(float *)(iVar1 + 0x24) * local_d4) + local_54) +
                          *(float *)(iVar1 + 0x3c) *
                          ((*(float *)(iVar1 + 0x20) * local_d4 -
                           *(float *)(iVar1 + 0x28) * local_dc) + local_50) +
                          *(float *)(iVar1 + 0x40) *
                          ((*(float *)(iVar1 + 0x24) * local_dc -
                           *(float *)(iVar1 + 0x20) * local_d8) + local_4c) + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
LAB_004c0107:
              if (local_e0 != 0) {
                if (*(int *)(iVar1 + 4) == local_e0) {
                  fVar2 = *(float *)(iVar1 + 0x30) *
                          ((*(float *)(iVar1 + 0x14) * local_a4 -
                           *(float *)(iVar1 + 0x1c) * local_ac) + local_20) +
                          *(float *)(iVar1 + 0x34) *
                          ((*(float *)(iVar1 + 0x18) * local_ac -
                           *(float *)(iVar1 + 0x14) * local_a8) + local_1c) +
                          *(float *)(iVar1 + 0x2c) *
                          ((*(float *)(iVar1 + 0x1c) * local_a8 -
                           *(float *)(iVar1 + 0x18) * local_a4) + local_24) + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_e0) goto LAB_004c0234;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x38) *
                                     ((*(float *)(iVar1 + 0x28) * local_a8 -
                                      *(float *)(iVar1 + 0x24) * local_a4) + local_24) +
                                    *(float *)(iVar1 + 0x3c) *
                                    ((*(float *)(iVar1 + 0x20) * local_a4 -
                                     *(float *)(iVar1 + 0x28) * local_ac) + local_20) +
                                    *(float *)(iVar1 + 0x40) *
                                    ((*(float *)(iVar1 + 0x24) * local_ac -
                                     *(float *)(iVar1 + 0x20) * local_a8) + local_1c));
                }
                *pfVar4 = fVar2;
              }
LAB_004c0234:
              iVar8 = local_68;
              if (local_68 != 0) {
                if (*(int *)(iVar1 + 4) == local_68) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x30) *
                                     ((*(float *)(iVar1 + 0x14) * local_80 -
                                      *(float *)(iVar1 + 0x1c) * local_88) + local_8) +
                                    *(float *)(iVar1 + 0x34) *
                                    ((*(float *)(iVar1 + 0x18) * local_88 -
                                     *(float *)(iVar1 + 0x14) * local_84) + local_4) +
                                    *(float *)(iVar1 + 0x2c) *
                                    ((*(float *)(iVar1 + 0x1c) * local_84 -
                                     *(float *)(iVar1 + 0x18) * local_80) + local_c));
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_68) goto LAB_004c0388;
                  fVar2 = *(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_84 -
                           *(float *)(iVar1 + 0x24) * local_80) + local_c) +
                          *(float *)(iVar1 + 0x3c) *
                          ((*(float *)(iVar1 + 0x20) * local_80 -
                           *(float *)(iVar1 + 0x28) * local_88) + local_8) +
                          *(float *)(iVar1 + 0x40) *
                          ((*(float *)(iVar1 + 0x24) * local_88 -
                           *(float *)(iVar1 + 0x20) * local_84) + local_4) + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
            }
LAB_004c0388:
            *pfVar6 = *pfVar4;
            piVar5 = piVar5 + 1;
            pfVar6 = pfVar6 + 1;
            pfVar4 = pfVar4 + local_78;
            local_70 = local_70 + -1;
            iVar7 = local_74;
          } while (local_70 != 0);
        }
      }
      local_74 = iVar7 + 1;
    } while (local_74 < pCount);
  }
  return;
}
#undef local_e0
#undef local_dc
#undef local_d8
#undef local_d4
#undef local_d0
#undef local_cc
#undef local_c8
#undef local_c4
#undef local_c0
#undef local_bc
#undef local_b8
#undef local_b4
#undef local_b0
#undef local_ac
#undef local_a8
#undef local_a4
#undef local_a0
#undef local_9c
#undef local_98
#undef local_94
#undef local_90
#undef local_8c
#undef local_88
#undef local_84
#undef local_80
#undef local_7c
#undef local_78
#undef local_74
#undef local_70
#undef local_6c
#undef local_68
#undef local_64
#undef local_60
#undef local_5c
#undef local_58
#undef local_54
#undef local_50
#undef local_4c
#undef local_48
#undef local_44
#undef local_40
#undef local_3c
#undef local_38
#undef local_34
#undef local_30
#undef local_2c
#undef local_28
#undef local_24
#undef local_20
#undef local_1c
#undef local_18
#undef local_14
#undef local_10
#undef local_c
#undef local_8
#undef local_4
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b77b0
void C2_HOOK_FASTCALL FUN_004b77b0(br_scalar* pVelocity, tPhysics_joint_force** pRecords, int pCount) {
    tPhysics_joint_force* record;
    tPhysics_object* object1;
    tPhysics_object* object2;
    float angular_delta[3];
    int offset;

    if (pCount <= 0) {
        return;
    }
    offset = (int)pVelocity - (int)pRecords;
    do {
        record = *pRecords;
        object1 = record->object_a;
        if (record->type == eJoint_limit_11) {
            *(br_scalar*)((tU8*)pRecords + offset) = -(record->direction_a.v[1] * object1->omega.v[1]
                + record->direction_a.v[2] * object1->omega.v[2]
                + record->direction_a.v[0] * object1->omega.v[0]);
            record = *pRecords;
            object2 = record->object_b;
            if (object2 != NULL) {
                *(br_scalar*)((tU8*)pRecords + offset) = record->direction_b.v[1] * object2->omega.v[1]
                    + record->direction_b.v[0] * object2->omega.v[0]
                    + record->direction_b.v[2] * object2->omega.v[2]
                    + *(br_scalar*)((tU8*)pRecords + offset);
            }
        } else {
            angular_delta[0] = record->point_a.v[2] * object1->omega.v[1] - record->point_a.v[1] * object1->omega.v[2] + object1->velocity_car_space.v[0];
            angular_delta[1] = record->point_a.v[0] * object1->omega.v[2] - record->point_a.v[2] * object1->omega.v[0] + object1->velocity_car_space.v[1];
            angular_delta[2] = record->point_a.v[1] * object1->omega.v[0] - record->point_a.v[0] * object1->omega.v[1] + object1->velocity_car_space.v[2];
            *(br_scalar*)((tU8*)pRecords + offset) = -(record->direction_a.v[1] * angular_delta[1]
                + record->direction_a.v[2] * angular_delta[2]
                + record->direction_a.v[0] * angular_delta[0]);
            record = *pRecords;
            object2 = record->object_b;
            if (object2 != NULL) {
                angular_delta[0] = record->point_b.v[2] * object2->omega.v[1] - record->point_b.v[1] * object2->omega.v[2] + object2->velocity_car_space.v[0];
                angular_delta[1] = record->point_b.v[0] * object2->omega.v[2] - record->point_b.v[2] * object2->omega.v[0] + object2->velocity_car_space.v[1];
                angular_delta[2] = record->point_b.v[1] * object2->omega.v[0] - record->point_b.v[0] * object2->omega.v[1] + object2->velocity_car_space.v[2];
                *(br_scalar*)((tU8*)pRecords + offset) = record->direction_b.v[0] * angular_delta[0]
                    + record->direction_b.v[1] * angular_delta[1]
                    + record->direction_b.v[2] * angular_delta[2]
                    + *(br_scalar*)((tU8*)pRecords + offset);
            }
        }
        pRecords++;
    } while (--pCount != 0);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004c53b0
void C2_HOOK_FASTCALL FUN_004c53b0(br_scalar* pMatrix, br_scalar* pOrthogonal, br_scalar* pVelocity, br_scalar* pImpulses, int pCount) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b7960
void C2_HOOK_FASTCALL FUN_004b7960(tPhysics_joint_force* pForce, br_scalar pImpulse) {
    /* The seven vectors occupy the whole 0x54-byte frame in this exact order
       (linear_delta at the lowest address, linear_accel at the highest). */
    struct {
        br_vector3 linear_delta;
        br_vector3 angular_delta;
        br_vector3 linear_delta_other;
        br_vector3 angular_delta_other;
        br_vector3 world_delta;
        br_vector3 impulse;
        br_vector3 linear_accel;
    } delta;
    tPhysics_object* pObject_a;
    tPhysics_object* pObject_b;
    tPhysics_object* pPartner;
    br_scalar inverse_dt;

    if (pForce->type == 9 || pImpulse == 0.f) {
        return;
    }
    pObject_a = pForce->object_a;
    pObject_b = pForce->object_b;
    if (pForce->type == 0xb) {
        pPartner = (tPhysics_object*)FUN_004be550(delta.angular_delta.v, delta.linear_delta.v, delta.angular_delta_other.v, delta.linear_delta_other.v, &pForce->direction_a, pObject_a);
        delta.angular_delta.v[0] *= pImpulse;
        delta.angular_delta.v[1] *= pImpulse;
        delta.angular_delta.v[2] *= pImpulse;
        pObject_a->omega.v[0] += delta.angular_delta.v[0];
        pObject_a->omega.v[1] += delta.angular_delta.v[1];
        pObject_a->omega.v[2] += delta.angular_delta.v[2];
        if (pPartner != NULL) {
            delta.linear_delta.v[0] *= pImpulse;
            delta.linear_delta.v[1] *= pImpulse;
            delta.linear_delta.v[2] *= pImpulse;
            pObject_a->velocity_car_space.v[0] += delta.linear_delta.v[0];
            pObject_a->velocity_car_space.v[1] += delta.linear_delta.v[1];
            pObject_a->velocity_car_space.v[2] += delta.linear_delta.v[2];
            BrMatrix34ApplyV(&pObject_a->v, &pObject_a->velocity_car_space, &pObject_a->transform_matrix);
            delta.linear_delta_other.v[0] *= pImpulse;
            delta.linear_delta_other.v[1] *= pImpulse;
            delta.linear_delta_other.v[2] *= pImpulse;
            pPartner->velocity_car_space.v[0] += delta.linear_delta_other.v[0];
            pPartner->velocity_car_space.v[1] += delta.linear_delta_other.v[1];
            pPartner->velocity_car_space.v[2] += delta.linear_delta_other.v[2];
            BrMatrix34ApplyV(&pPartner->v, &pPartner->velocity_car_space, &pPartner->transform_matrix);
            delta.angular_delta_other.v[0] *= pImpulse;
            delta.angular_delta_other.v[1] *= pImpulse;
            delta.angular_delta_other.v[2] *= pImpulse;
            pPartner->omega.v[0] += delta.angular_delta_other.v[0];
            pPartner->omega.v[1] += delta.angular_delta_other.v[1];
            pPartner->omega.v[2] += delta.angular_delta_other.v[2];
        }
        if (pObject_b != NULL) {
            pPartner = (tPhysics_object*)FUN_004be550(delta.angular_delta.v, delta.linear_delta.v, delta.angular_delta_other.v, delta.linear_delta_other.v, &pForce->direction_b, pObject_b);
            pImpulse = -pImpulse;
            delta.angular_delta.v[0] *= pImpulse;
            delta.angular_delta.v[1] *= pImpulse;
            delta.angular_delta.v[2] *= pImpulse;
            pObject_b->omega.v[0] += delta.angular_delta.v[0];
            pObject_b->omega.v[1] += delta.angular_delta.v[1];
            pObject_b->omega.v[2] += delta.angular_delta.v[2];
            if (pPartner != NULL) {
                delta.linear_delta.v[0] *= pImpulse;
                delta.linear_delta.v[1] *= pImpulse;
                delta.linear_delta.v[2] *= pImpulse;
                pObject_b->velocity_car_space.v[0] += delta.linear_delta.v[0];
                pObject_b->velocity_car_space.v[1] += delta.linear_delta.v[1];
                pObject_b->velocity_car_space.v[2] += delta.linear_delta.v[2];
                BrMatrix34ApplyV(&pObject_b->v, &pObject_b->velocity_car_space, &pObject_b->transform_matrix);
                delta.linear_delta_other.v[0] *= pImpulse;
                delta.linear_delta_other.v[1] *= pImpulse;
                delta.linear_delta_other.v[2] *= pImpulse;
                pPartner->velocity_car_space.v[0] += delta.linear_delta_other.v[0];
                pPartner->velocity_car_space.v[1] += delta.linear_delta_other.v[1];
                pPartner->velocity_car_space.v[2] += delta.linear_delta_other.v[2];
                BrMatrix34ApplyV(&pPartner->v, &pPartner->velocity_car_space, &pPartner->transform_matrix);
                delta.angular_delta_other.v[0] *= pImpulse;
                delta.angular_delta_other.v[1] *= pImpulse;
                delta.angular_delta_other.v[2] *= pImpulse;
                pPartner->omega.v[0] += delta.angular_delta_other.v[0];
                pPartner->omega.v[1] += delta.angular_delta_other.v[1];
                pPartner->omega.v[2] += delta.angular_delta_other.v[2];
            }
        }
        return;
    }
    if (*(int*)((tU8*)pObject_a + 0x1e4) != 0) {
        FUN_004be4b0(delta.angular_delta.v, delta.linear_delta.v, &pForce->point_a, &pForce->direction_a, pObject_a);
        pPartner = (tPhysics_object*)FUN_004becf0(delta.angular_delta.v, delta.linear_delta.v, delta.angular_delta_other.v, delta.linear_delta_other.v, pObject_a);
        delta.angular_delta.v[0] *= pImpulse;
        delta.angular_delta.v[1] *= pImpulse;
        delta.angular_delta.v[2] *= pImpulse;
        pObject_a->omega.v[0] += delta.angular_delta.v[0];
        pObject_a->omega.v[1] += delta.angular_delta.v[1];
        pObject_a->omega.v[2] += delta.angular_delta.v[2];
        delta.angular_delta_other.v[0] *= pImpulse;
        delta.angular_delta_other.v[1] *= pImpulse;
        delta.angular_delta_other.v[2] *= pImpulse;
        pPartner->omega.v[0] += delta.angular_delta_other.v[0];
        pPartner->omega.v[1] += delta.angular_delta_other.v[1];
        pPartner->omega.v[2] += delta.angular_delta_other.v[2];
        delta.linear_delta.v[0] *= pImpulse;
        delta.linear_delta.v[1] *= pImpulse;
        delta.linear_delta.v[2] *= pImpulse;
        pObject_a->velocity_car_space.v[0] += delta.linear_delta.v[0];
        pObject_a->velocity_car_space.v[1] += delta.linear_delta.v[1];
        pObject_a->velocity_car_space.v[2] += delta.linear_delta.v[2];
        BrMatrix34ApplyV(&delta.world_delta, &delta.linear_delta, &pObject_a->transform_matrix);
        pObject_a->v.v[0] += delta.world_delta.v[0];
        pObject_a->v.v[1] += delta.world_delta.v[1];
        pObject_a->v.v[2] += delta.world_delta.v[2];
        delta.linear_delta_other.v[0] *= pImpulse;
        delta.linear_delta_other.v[1] *= pImpulse;
        delta.linear_delta_other.v[2] *= pImpulse;
        pPartner->velocity_car_space.v[0] += delta.linear_delta_other.v[0];
        pPartner->velocity_car_space.v[1] += delta.linear_delta_other.v[1];
        pPartner->velocity_car_space.v[2] += delta.linear_delta_other.v[2];
        BrMatrix34ApplyV(&delta.world_delta, &delta.linear_delta_other, &pPartner->transform_matrix);
        pPartner->v.v[0] += delta.world_delta.v[0];
        pPartner->v.v[1] += delta.world_delta.v[1];
        pPartner->v.v[2] += delta.world_delta.v[2];
    } else {
        delta.impulse.v[0] = pForce->direction_a.v[0] * pImpulse;
        delta.linear_accel.v[0] = delta.impulse.v[0] / pObject_a->M;
        delta.impulse.v[1] = pForce->direction_a.v[1] * pImpulse;
        delta.linear_accel.v[1] = delta.impulse.v[1] / pObject_a->M;
        delta.impulse.v[2] = pForce->direction_a.v[2] * pImpulse;
        delta.linear_accel.v[2] = delta.impulse.v[2] / pObject_a->M;
        pObject_a->velocity_car_space.v[0] += delta.linear_accel.v[0];
        pObject_a->velocity_car_space.v[1] += delta.linear_accel.v[1];
        pObject_a->velocity_car_space.v[2] += delta.linear_accel.v[2];
        BrMatrix34ApplyV(&delta.world_delta, &delta.linear_accel, &pObject_a->transform_matrix);
        pObject_a->v.v[0] += delta.world_delta.v[0];
        pObject_a->v.v[1] += delta.world_delta.v[1];
        pObject_a->v.v[2] += delta.world_delta.v[2];
        if ((pObject_a->flags & 0x800) == 0) {
            delta.angular_delta.v[0] = delta.impulse.v[2] * pForce->point_a.v[1] - delta.impulse.v[1] * pForce->point_a.v[2];
            delta.angular_delta.v[1] = delta.impulse.v[0] * pForce->point_a.v[2] - delta.impulse.v[2] * pForce->point_a.v[0];
            delta.angular_delta.v[2] = delta.impulse.v[1] * pForce->point_a.v[0] - delta.impulse.v[0] * pForce->point_a.v[1];
            pObject_a->omega.v[0] += delta.angular_delta.v[0] / pObject_a->I.v[0];
            pObject_a->omega.v[1] += delta.angular_delta.v[1] / pObject_a->I.v[1];
            pObject_a->omega.v[2] += delta.angular_delta.v[2] / pObject_a->I.v[2];
        }
    }
    if (pObject_b != NULL && pForce->type != 0xe) {
        if (*(int*)((tU8*)pObject_b + 0x1e4) != 0) {
            FUN_004be4b0(delta.angular_delta.v, delta.linear_delta.v, &pForce->point_b, &pForce->direction_b, pObject_b);
            pPartner = (tPhysics_object*)FUN_004becf0(delta.angular_delta.v, delta.linear_delta.v, delta.angular_delta_other.v, delta.linear_delta_other.v, pObject_b);
            pImpulse = -pImpulse;
            delta.angular_delta.v[0] *= pImpulse;
            delta.angular_delta.v[1] *= pImpulse;
            delta.angular_delta.v[2] *= pImpulse;
            pObject_b->omega.v[0] += delta.angular_delta.v[0];
            pObject_b->omega.v[1] += delta.angular_delta.v[1];
            pObject_b->omega.v[2] += delta.angular_delta.v[2];
            delta.angular_delta_other.v[0] *= pImpulse;
            delta.angular_delta_other.v[1] *= pImpulse;
            delta.angular_delta_other.v[2] *= pImpulse;
            pPartner->omega.v[0] += delta.angular_delta_other.v[0];
            pPartner->omega.v[1] += delta.angular_delta_other.v[1];
            pPartner->omega.v[2] += delta.angular_delta_other.v[2];
            delta.linear_delta.v[0] *= pImpulse;
            delta.linear_delta.v[1] *= pImpulse;
            delta.linear_delta.v[2] *= pImpulse;
            pObject_b->velocity_car_space.v[0] += delta.linear_delta.v[0];
            pObject_b->velocity_car_space.v[1] += delta.linear_delta.v[1];
            pObject_b->velocity_car_space.v[2] += delta.linear_delta.v[2];
            BrMatrix34ApplyV(&delta.world_delta, &delta.linear_delta, &pObject_b->transform_matrix);
            pObject_b->v.v[0] += delta.world_delta.v[0];
            pObject_b->v.v[1] += delta.world_delta.v[1];
            pObject_b->v.v[2] += delta.world_delta.v[2];
            delta.linear_delta_other.v[0] *= pImpulse;
            delta.linear_delta_other.v[1] *= pImpulse;
            delta.linear_delta_other.v[2] *= pImpulse;
            pPartner->velocity_car_space.v[0] += delta.linear_delta_other.v[0];
            pPartner->velocity_car_space.v[1] += delta.linear_delta_other.v[1];
            pPartner->velocity_car_space.v[2] += delta.linear_delta_other.v[2];
            BrMatrix34ApplyV(&delta.world_delta, &delta.linear_delta_other, &pPartner->transform_matrix);
            pPartner->v.v[0] += delta.world_delta.v[0];
            pPartner->v.v[1] += delta.world_delta.v[1];
            pPartner->v.v[2] += delta.world_delta.v[2];
        } else {
            inverse_dt = -pImpulse;
            delta.impulse.v[0] = pForce->direction_b.v[0] * inverse_dt;
            delta.linear_accel.v[0] = delta.impulse.v[0] / pObject_b->M;
            delta.impulse.v[1] = pForce->direction_b.v[1] * inverse_dt;
            delta.linear_accel.v[1] = delta.impulse.v[1] / pObject_b->M;
            delta.impulse.v[2] = pForce->direction_b.v[2] * inverse_dt;
            delta.linear_accel.v[2] = delta.impulse.v[2] / pObject_b->M;
            pObject_b->velocity_car_space.v[0] += delta.linear_accel.v[0];
            pObject_b->velocity_car_space.v[1] += delta.linear_accel.v[1];
            pObject_b->velocity_car_space.v[2] += delta.linear_accel.v[2];
            BrMatrix34ApplyV(&delta.world_delta, &delta.linear_accel, &pObject_b->transform_matrix);
            pObject_b->v.v[0] += delta.world_delta.v[0];
            pObject_b->v.v[1] += delta.world_delta.v[1];
            pObject_b->v.v[2] += delta.world_delta.v[2];
            if ((pObject_b->flags & 0x800) == 0) {
                delta.angular_delta.v[0] = delta.impulse.v[2] * pForce->point_b.v[1] - delta.impulse.v[1] * pForce->point_b.v[2];
                delta.angular_delta.v[1] = delta.impulse.v[0] * pForce->point_b.v[2] - delta.impulse.v[2] * pForce->point_b.v[0];
                delta.angular_delta.v[2] = delta.impulse.v[1] * pForce->point_b.v[0] - delta.impulse.v[0] * pForce->point_b.v[1];
                pObject_b->omega.v[0] += delta.angular_delta.v[0] / pObject_b->I.v[0];
                pObject_b->omega.v[1] += delta.angular_delta.v[1] / pObject_b->I.v[1];
                pObject_b->omega.v[2] += delta.angular_delta.v[2] / pObject_b->I.v[2];
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 98.67%: FUN_004b8ee0 0x004b8ee0 - only the stack slot of changed_object differs (orig esp+8, ours esp+0xc: local declaration order); callees FUN_004b8fe0/FUN_004b9230 are still stubs
// FUNCTION: CARMA2_HW 0x004b8ee0
void C2_HOOK_FASTCALL FUN_004b8ee0(tPhysics_object* pObject, float pDelta_time) {
    tPhysics_object* changed_object;
    tPhysics_object* child;
    tPhysics_object* matrix_parent;
    br_vector3 child_joint_pos;
    br_vector3 parent_joint_pos;

    changed_object = NULL;
    FUN_004b9230(pObject, &changed_object, pDelta_time);
    if (changed_object != NULL && changed_object->field_0x230 != NULL) {
        child = changed_object;
        FUN_004b8fe0(child, pDelta_time);
        matrix_parent = child->field_0x230;
        do {
            BrMatrix34ApplyP(&child_joint_pos, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
            BrMatrix34ApplyP(&parent_joint_pos, &child->physics_joint1->field_0x14, &matrix_parent->actor->t.t.mat);
            child_joint_pos.v[0] -= parent_joint_pos.v[0];
            child_joint_pos.v[1] -= parent_joint_pos.v[1];
            child_joint_pos.v[2] -= parent_joint_pos.v[2];
            child->parent->actor->t.t.mat.m[3][0] += child_joint_pos.v[0];
            child->parent->actor->t.t.mat.m[3][1] += child_joint_pos.v[1];
            child->parent->actor->t.t.mat.m[3][2] += child_joint_pos.v[2];
            child = child->field_0x230;
            matrix_parent = child->field_0x230;
        } while (matrix_parent != NULL);
        WriteNewPositions(pObject);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b8cf0
void C2_HOOK_FASTCALL FUN_004b8cf0(tPhysics_object* pObject) {
    int uVar1;
    int uVar2;
    int i;
    int* piVar4;
    char* puVar3;
    int local_174[6];
    float local_15c[15];
    tPhysics_joint_force rows[3];
    tPhysics_object* pObject2;
    tPhysics_object* pOther;

    pObject2 = pObject->field_0x230;
    pOther = (tPhysics_object*)*(int*)(*(int*)((tU8*)pObject + 0x1e4) + 4);
    if (pOther != pObject2) {
        pObject2 = pObject;
        pObject = pOther;
    }
    CreateJointForce(pObject, pObject2, &rows[0], (br_vector3*)((char*)pObject->physics_joint1 + 0x38));
    CreateJointTorque(pObject, pObject2, &rows[1], (br_vector3*)((char*)pObject->physics_joint1 + 0x20));
    CreateJointTorque(pObject, pObject2, &rows[2], (br_vector3*)((char*)pObject->physics_joint1 + 0x2c));
    piVar4 = local_174;
    puVar3 = (char*)rows;
    i = 3;
    do {
        *piVar4 = (int)puVar3;
        puVar3 += 0x60;
        piVar4++;
        i--;
    } while (i != 0);
    uVar1 = *(int*)&pObject2->field_0x1e0[4];
    uVar2 = *(int*)&pObject->field_0x1e0[4];
    *(int*)&pObject->field_0x1e0[4] = 0;
    *(int*)&pObject2->field_0x1e0[4] = 0;
    FUN_004bf490(&local_15c[3], 3, (tPhysics_joint_force**)local_174, 3);
    FUN_004be350((float*)(uVar2 + 8), &local_15c[3]);
    FUN_004b77b0((br_scalar*)&local_174[3], (tPhysics_joint_force**)local_174, 3);
    BrMatrix34ApplyV((br_vector3*)&local_15c[0], (br_vector3*)&local_174[3], (br_matrix34*)(uVar2 + 8));
    i = 0;
    do {
        FUN_004b7960((tPhysics_joint_force*)*(int*)((char*)local_174 + i), *(float*)((char*)local_15c + i));
        i += 4;
    } while (i < 0xc);
    *(int*)&pObject->field_0x1e0[4] = uVar2;
    *(int*)&pObject2->field_0x1e0[4] = uVar1;
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004be550
int C2_HOOK_FASTCALL FUN_004be550(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, const br_vector3* pDirection, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/*
 * Torque and velocity at a contact: pOut1 receives (lever x force) / I,
 * pOut2 receives force / mass. Objects flagged 0x800 get a zero torque.
 */
// FUNCTION: CARMA2_HW 0x004be4b0
void C2_HOOK_FASTCALL FUN_004be4b0(br_scalar* pOut1, br_scalar* pOut2, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject) {
    int i;
    double inv_mass;

    if ((pObject->flags & 0x800) == 0) {
        pOut1[0] = (br_scalar)((double)pPoint->v[1] * pDirection->v[2] - (double)pPoint->v[2] * pDirection->v[1]);
        pOut1[1] = (br_scalar)((double)pPoint->v[2] * pDirection->v[0] - (double)pDirection->v[2] * pPoint->v[0]);
        pOut1[2] = (br_scalar)((double)pDirection->v[1] * pPoint->v[0] - (double)pPoint->v[1] * pDirection->v[0]);
        for (i = 0; i < 3; i++) {
            pOut1[i] = pOut1[i] / pObject->I.v[i];
        }
    } else {
        pOut1[0] = 0.0f;
        pOut1[1] = 0.0f;
        pOut1[2] = 0.0f;
    }
    inv_mass = 1.0 / pObject->M;
    pOut2[0] = pDirection->v[0] * inv_mass;
    pOut2[1] = pDirection->v[1] * inv_mass;
    pOut2[2] = pDirection->v[2] * inv_mass;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004becf0
int C2_HOOK_FASTCALL FUN_004becf0(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0065d014
int gINT_0065d014 = 1;

// GLOBAL: CARMA2_HW 0x00692dd8
int gINT_00692dd8;

// GLOBAL: CARMA2_HW 0x00692ddc
int gINT_00692ddc;

// GLOBAL: CARMA2_HW 0x00692de0
tPhysics_joint_force gCollision_contacts[50];

// GLOBAL: CARMA2_HW 0x006940a4
int gINT_006940a4;

extern int gINT_00692dc8;

typedef int C2_HOOK_FASTCALL tQuickHinge_moved_cbfn(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);
extern float gFLOAT_00692dd0;
extern float gFLOAT_00692dd4;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004bd830
tPhysics_object** C2_HOOK_FASTCALL AppendOverlappingObjects(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pObjects) {
    tPhysics_object** pLink;
    tPhysics_object** pChild_link;
    tPhysics_object* pCurrent;
    tPhysics_object* child_head;
    float x;

    pLink = pObjects;
    if (pList->parent != NULL) {
        for (pCurrent = pList; pCurrent != NULL; pCurrent = pCurrent->next) {
            if ((pCurrent == pObject && (pObject->flags & 8) == 0)
                    || BoundsOverlapTest__finteray(&pObject->field_0x10c, &pCurrent->field_0x10c)) {
                *pLink = pCurrent;
                pLink = (tPhysics_object**)&pCurrent->field_0x234;
            }
        }
    } else {
        x = pObject->field_0x10c.min.v[0] - gFLOAT_00692dd0;
        for (pCurrent = pObject->prev; pCurrent != NULL && pCurrent->transform_matrix.m[3][0] >= x; pCurrent = pCurrent->prev) {
            if (BoundsOverlapTest__finteray(&pObject->field_0x10c, &pCurrent->field_0x10c)) {
                *pLink = pCurrent;
                pLink = (tPhysics_object**)&pCurrent->field_0x234;
            }
        }
        x = pObject->field_0x10c.max.v[0] - gFLOAT_00692dd4;
        for (pCurrent = pObject->next; pCurrent != NULL && pCurrent->transform_matrix.m[3][0] <= x; pCurrent = pCurrent->next) {
            if (BoundsOverlapTest__finteray(&pObject->field_0x10c, &pCurrent->field_0x10c)) {
                *pLink = pCurrent;
                pLink = (tPhysics_object**)&pCurrent->field_0x234;
            }
        }
        if ((pObject->flags & 8) == 0) {
            *pLink = pObject;
            pLink = (tPhysics_object**)&pObject->field_0x234;
        }
    }
    *pLink = NULL;

    child_head = NULL;
    pCurrent = *pObjects;
    pChild_link = &child_head;
    while (pCurrent != NULL) {
        if (pCurrent->child != NULL) {
            pChild_link = AppendOverlappingObjects(pObject, pCurrent->child, pChild_link);
        }
        pCurrent = (tPhysics_object*)pCurrent->field_0x234;
    }
    if (child_head != NULL) {
        *pChild_link = NULL;
        *pLink = child_head;
        return pChild_link;
    }
    return pLink;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tPhysics_object** gReduced_object_list;
extern int gINT_006940ac;
// 0x4228b0 (stubbed in 13-crush1.c), 0x425ca0, 0x4bddd0 and 0x4be0e0 are still unimplemented.
extern int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount);
extern int C2_HOOK_FASTCALL FUN_00425ca0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_joint_force* pContacts, int pCount);
extern void C2_HOOK_FASTCALL FUN_004bddd0(tPhysics_object* pObject, tPhysics_object* pPartner);
extern void C2_HOOK_FASTCALL FUN_004be0e0(tPhysics_object* pObject, tPhysics_object* pList);

typedef void (C2_HOOK_FASTCALL *tActivatePassive_cbfn)(tPhysics_object* pObject, tPhysics_object* pActivator);

// FUNCTION: CARMA2_HW 0x004bd9e0
int C2_HOOK_FASTCALL FUN_004bd9e0(tPhysics_object* pObject, tPhysics_object** pOverlapping, tPhysics_joint_force* pContacts, int pCount, tWorld_callbacks* pWorld_callbacks, tPhysics_object* pList) {
    tPhysics_object* object;
    tPhysics_object* partner;
    tPhysics_object* child;
    tPhysics_object* moved;
    tPhysics_object** slot;
    tPhysics_object** moved_tail;
    int added;

    gReduced_object_list = pOverlapping;
    gINT_006940ac = (((float*)pObject->field_0x1b4)[0] * ((float*)pObject->field_0x1b4)[0]
        + ((float*)pObject->field_0x1b4)[1] * ((float*)pObject->field_0x1b4)[1]
        + ((float*)pObject->field_0x1b4)[2] * ((float*)pObject->field_0x1b4)[2]) == 0.f;
    GetFacesInBox(pObject, pWorld_callbacks);
    gReduced_object_list = NULL;
    gINT_006940ac = 0;
    added = FUN_004228b0(pObject, pContacts, pCount);
    if (added != 0) {
        if (pObject->field_0xf0 == 2) {
            object = pObject;
            while (object->parent != NULL && object->physics_joint1 != NULL
                    && object->physics_joint1->type != eJoint_none) {
                object = object->parent;
            }
            object->field_0xf0 = 0;
            for (child = object->child; child != NULL; child = child->next) {
                SetFieldF0Tree(child, 0);
            }
            if (pWorld_callbacks != NULL && pWorld_callbacks->activate_passive != NULL) {
                ((tActivatePassive_cbfn)pWorld_callbacks->activate_passive)(object, NULL);
            }
            if ((object->child != NULL || object->physics_joint2 != NULL) && object->field_0x218 == 0) {
                MoveJointedObject(object, 0.f);
            }
        }
        pCount += added;
    }
    for (partner = *pOverlapping; partner != NULL; partner = (tPhysics_object*)partner->field_0x234) {
        if (partner == pObject) {
            continue;
        }
        if (pObject->field_0x1dc != 0 && partner->field_0x1dc == pObject->field_0x1dc && (tU32)partner <= (tU32)pObject) {
            continue;
        }
        if (pObject->parent == partner || partner->parent == pObject) {
            continue;
        }
        if ((pObject->field_0x1a0 & partner->field_0x1a4) != 0
                || (pObject->field_0x1a4 & partner->field_0x1a0) != 0
                || (pObject->flags & 4) != 0
                || (partner->flags & 4) != 0) {
            continue;
        }
        added = FUN_00425ca0(pObject, partner, pContacts, pCount);
        if (added == 0) {
            continue;
        }
        if (pObject->field_0xf0 == 2) {
            object = pObject;
            while (object->parent != NULL && object->physics_joint1 != NULL
                    && object->physics_joint1->type != eJoint_none) {
                object = object->parent;
            }
            SetFieldF0Tree(object, 0);
            if (pWorld_callbacks != NULL && pWorld_callbacks->activate_passive != NULL) {
                ((tActivatePassive_cbfn)pWorld_callbacks->activate_passive)(object, partner);
            }
            if ((object->child != NULL || object->physics_joint2 != NULL) && object->field_0x218 == 0) {
                MoveJointedObject(object, 0.f);
            }
        }
        if (partner->field_0xf0 == 2) {
            object = partner;
            while (object->parent != NULL && object->physics_joint1 != NULL
                    && object->physics_joint1->type != eJoint_none) {
                object = object->parent;
            }
            SetFieldF0Tree(object, 0);
            if (pWorld_callbacks != NULL && pWorld_callbacks->activate_passive != NULL) {
                ((tActivatePassive_cbfn)pWorld_callbacks->activate_passive)(object, pObject);
            }
            if ((object->child != NULL || object->physics_joint2 != NULL) && object->field_0x218 == 0) {
                MoveJointedObject(object, 0.f);
            }
        }
        if (partner->disable_move_rotate != 0) {
            FUN_004be0e0(partner, pList);
        }
        FUN_004bddd0(pObject, partner);
        pCount += added;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        if (child->physics_joint1 == NULL || child->physics_joint1->type == eJoint_none) {
            continue;
        }
        moved = NULL;
        moved_tail = &moved;
        slot = pOverlapping;
        for (partner = *slot; partner != NULL; partner = *slot) {
            if (partner == child || BoundsOverlapTest__finteray(&child->field_0x10c, &partner->field_0x10c)) {
                *slot = (tPhysics_object*)partner->field_0x234;
                *moved_tail = partner;
                moved_tail = (tPhysics_object**)&partner->field_0x234;
            } else {
                slot = (tPhysics_object**)&partner->field_0x234;
            }
        }
        *moved_tail = NULL;
        *slot = moved;
        pCount = FUN_004bd9e0(child, slot, pContacts, pCount, pWorld_callbacks, pList);
    }
    return pCount;
}

extern int C2_HOOK_FASTCALL FUN_004269c0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining);
extern int C2_HOOK_FASTCALL FUN_00427870(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining);
extern void C2_HOOK_FASTCALL FUN_0045d5e0(tCollision_shape_sphere_data* pSphere1, tCollision_shape_sphere_data* pSphere2, br_vector3* pStart, br_vector3* pEnd, br_vector3* pNormal, float* pFactor);

/* based on the REA pseudocode for 0x00425ca0 and the original disassembly */
// FUNCTION: CARMA2_HW 0x00425ca0
int C2_HOOK_FASTCALL FUN_00425ca0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_joint_force* pContacts, int pCount) {
    br_vector3 vertices1[128][2];
    br_vector3 vertices2[128][2];
    tU16 point_counts1[66];
    tU16 point_counts2[18];
    br_matrix34 object2_to_object1;
    br_matrix34 actor2_to_actor1;
    br_vector3 point;
    br_vector3 translated_point;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    br_vector3* vertices1_for_shape;
    br_vector3* vertices2_for_shape;
    tU16* point_count1_for_shape;
    tU16* point_count2_for_shape;
    int vertex_count1;
    int vertex_count2;
    int point_count;
    int i;
    int contact_count;
    int result;
    float collision_factor;
    float remaining;
    tPhysics_joint_force* first_contact;
    float* record;
    float normal_dot;

    shape2 = pObject2->shape;
    shape1 = pObject1->shape;
    contact_count = 0;
    if (pCount > 0x24) {
        return 0;
    }

    {
        br_matrix34 inverse_object1;
        br_matrix34 inverse_actor1;

        BrMatrix34LPInverse(&inverse_object1, &pObject1->transform_matrix);
        BrMatrix34Mul(&object2_to_object1, &pObject2->transform_matrix, &inverse_object1);
        BrMatrix34LPInverse(&inverse_actor1, &pObject1->actor->t.t.mat);
        BrMatrix34Mul(&actor2_to_actor1, &pObject2->actor->t.t.mat, &inverse_actor1);
    }
    /* Object 2 points are expressed in object 1 space; object 1 points use
     * the inverse relative transform to enter object 2 space. */
    point_count2_for_shape = point_counts2;
    vertices2_for_shape = &vertices2[0][0];
    for (; shape2 != NULL; shape2 = shape2->common.next) {
        switch (shape2->common.type) {
        case kCollisionShapeType_Box:
            point_count = 8;
            for (i = 0; i < 8; i++) {
                point.v[0] = (i & 2) ? shape2->box.common.bb.min.v[0] : shape2->box.common.bb.max.v[0];
                point.v[1] = (i & 1) ? shape2->box.common.bb.min.v[1] : shape2->box.common.bb.max.v[1];
                point.v[2] = (i & 4) ? shape2->box.common.bb.min.v[2] : shape2->box.common.bb.max.v[2];
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2], &point, &object2_to_object1);
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2 + 1], &point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            point_count = shape2->polyhedron.polyhedron.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape2->polyhedron.polyhedron.points[i];
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2], &point, &object2_to_object1);
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2 + 1], &point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Wireframe:
            point_count = shape2->wireframe.wireframe.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape2->wireframe.wireframe.points[i];
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2], &point, &object2_to_object1);
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2 + 1], &point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Sphere:
            point_count = 1;
            point = shape2->sphere.sphere.center;
            BrMatrix34ApplyP(&vertices2_for_shape[0], &point, &object2_to_object1);
            BrMatrix34ApplyP(&vertices2_for_shape[1], &point, &actor2_to_actor1);
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, "");
            point_count = 0;
            break;
        }
        *point_count2_for_shape = (tU16)point_count;
        point_count2_for_shape++;
        vertices2_for_shape += point_count * 2;
    }

    point_count1_for_shape = point_counts1;
    vertices1_for_shape = &vertices1[0][0];
    for (; shape1 != NULL; shape1 = shape1->common.next) {
        switch (shape1->common.type) {
        case kCollisionShapeType_Box:
            point_count = 8;
            for (i = 0; i < 8; i++) {
                point.v[0] = (i & 2) ? shape1->box.common.bb.min.v[0] : shape1->box.common.bb.max.v[0];
                point.v[1] = (i & 1) ? shape1->box.common.bb.min.v[1] : shape1->box.common.bb.max.v[1];
                point.v[2] = (i & 4) ? shape1->box.common.bb.min.v[2] : shape1->box.common.bb.max.v[2];
                translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
                translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
                translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2], &translated_point, &object2_to_object1);
                translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
                translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
                translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2 + 1], &translated_point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            point_count = shape1->polyhedron.polyhedron.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape1->polyhedron.polyhedron.points[i];
                translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
                translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
                translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2], &translated_point, &object2_to_object1);
                translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
                translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
                translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2 + 1], &translated_point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Wireframe:
            point_count = shape1->wireframe.wireframe.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape1->wireframe.wireframe.points[i];
                translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
                translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
                translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2], &translated_point, &object2_to_object1);
                translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
                translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
                translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2 + 1], &translated_point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Sphere:
            point_count = 1;
            point = shape1->sphere.sphere.center;
            translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
            translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
            translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
            BrMatrix34TApplyV(&vertices1_for_shape[0], &translated_point, &object2_to_object1);
            translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
            translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
            translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
            BrMatrix34TApplyV(&vertices1_for_shape[1], &translated_point, &actor2_to_actor1);
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, "");
            point_count = 0;
            break;
        }
        *point_count1_for_shape = (tU16)point_count;
        point_count1_for_shape++;
        vertices1_for_shape += point_count * 2;
    }

    contact_count = 0;
    point_count1_for_shape = point_counts1;
    vertices1_for_shape = &vertices1[0][0];
    for (shape1 = pObject1->shape; shape1 != NULL; shape1 = shape1->common.next) {
        vertex_count1 = *point_count1_for_shape;
        point_count2_for_shape = point_counts2;
        vertices2_for_shape = &vertices2[0][0];
        for (shape2 = pObject2->shape; shape2 != NULL; shape2 = shape2->common.next) {
            vertex_count2 = *point_count2_for_shape;
            if (shape1->common.type == kCollisionShapeType_Sphere) {
                if (shape2->common.type == kCollisionShapeType_Sphere) {
                    br_vector3 normal;
                    br_vector3 contact1;
                    br_vector3 contact2;
                    br_vector3 normal_start;
                    br_vector3 normal_end;
                    br_vector3 world_normal;
                    br_vector3 local_normal_start;
                    br_vector3 local_normal_end;
                    br_vector3 local_normal;

                    collision_factor = 100.0f;
                    FUN_0045d5e0(&shape1->sphere.sphere, &shape2->sphere.sphere, &vertices2_for_shape[0], &vertices2_for_shape[1], &normal, &collision_factor);
                    if (collision_factor <= 1.0f) {
                        BrVector3Scale(&contact1, &normal, -shape1->sphere.sphere.radius);
                        BrVector3Accumulate(&contact1, &shape1->sphere.sphere.center);
                        BrMatrix34ApplyV(&normal_start, &normal, &pObject1->transform_matrix);
                        BrMatrix34ApplyV(&normal_end, &normal, &pObject1->actor->t.t.mat);
                        remaining = 1.0f - collision_factor;
                        world_normal.v[0] = normal_start.v[0] * remaining + normal_end.v[0] * collision_factor;
                        world_normal.v[1] = normal_start.v[1] * remaining + normal_end.v[1] * collision_factor;
                        world_normal.v[2] = normal_start.v[2] * remaining + normal_end.v[2] * collision_factor;
                        BrMatrix34TApplyV(&local_normal_start, &world_normal, &pObject2->transform_matrix);
                        BrMatrix34TApplyV(&local_normal_end, &world_normal, &pObject2->actor->t.t.mat);
                        local_normal.v[0] = local_normal_start.v[0] * remaining + local_normal_end.v[0] * collision_factor;
                        local_normal.v[1] = local_normal_start.v[1] * remaining + local_normal_end.v[1] * collision_factor;
                        local_normal.v[2] = local_normal_start.v[2] * remaining + local_normal_end.v[2] * collision_factor;
                        BrVector3Scale(&contact2, &local_normal, shape2->sphere.sphere.radius);
                        BrVector3Accumulate(&contact2, &shape2->sphere.sphere.center);
                        contact_count = AddObjectObjectForce(&pContacts[pCount], &contact1, collision_factor, &contact2, &world_normal, contact_count, pObject1, pObject2, 7, NULL, 0.0f);
                    }
                } else {
                    contact_count = FUN_004269c0(pObject2, pObject1, shape2, shape1, vertices2_for_shape, vertices1_for_shape, vertex_count2, vertex_count1, &pContacts[pCount], contact_count, 0x28 - pCount);
                }
            } else if (shape2->common.type == kCollisionShapeType_Sphere) {
                contact_count = FUN_004269c0(pObject1, pObject2, shape1, shape2, vertices1_for_shape, vertices2_for_shape, vertex_count1, vertex_count2, &pContacts[pCount], contact_count, 0x28 - pCount);
            } else {
                contact_count = FUN_00427870(pObject1, pObject2, shape1, shape2, vertices1_for_shape, vertices2_for_shape, vertex_count1, vertex_count2, &pContacts[pCount], contact_count, 0x28 - pCount);
            }
            vertices2_for_shape += vertex_count2 * 2;
            point_count2_for_shape++;
        }
        vertices1_for_shape += vertex_count1 * 2;
        point_count1_for_shape++;
    }

    if (contact_count != 0) {
        pObject1->omega = pObject1->rotate_omega;
        pObject2->omega = pObject2->rotate_omega;
        result = 1;
        if (contact_count > 1) {
            first_contact = &pContacts[pCount];
            record = (float*)&first_contact[1].direction_a.v[2];
            do {
                if (first_contact[1].object_a == first_contact[0].object_a) {
                    normal_dot = record[0] * first_contact[0].direction_a.v[2]
                            + record[-1] * first_contact[0].direction_a.v[1]
                            + record[-2] * first_contact[0].direction_a.v[0];
                } else {
                    normal_dot = -(record[0] * first_contact[0].direction_b.v[2]
                            + record[-1] * first_contact[0].direction_b.v[1]
                            + record[-2] * first_contact[0].direction_b.v[0]);
                }
                if (normal_dot < 0.7071067811865476) {
                    return result;
                }
                result++;
                record += 0x18;
            } while (result < contact_count);
        }
        return result;
    }
    return 0;
}

// GLOBAL: CARMA2_HW 0x0058a798
double gDOUBLE_0058a798 = 0.01;

extern int gINT_006940a0;

// PARTIAL 31.18%: FUN_004bddd0 0x004bddd0 - logic mirrors the original; mark values are read straight
// from pObject->field_0x1dc (no byte locals). Residual is MSVC register/stack assignment: frame 0xc vs
// 0x8 (group link pointer spills to [esp+0x18]) and pObject2 lives in ebp instead of the second slot.
// FUNCTION: CARMA2_HW 0x004bddd0
void C2_HOOK_FASTCALL FUN_004bddd0(tPhysics_object* pObject1, tPhysics_object* pObject2) {
    tPhysics_object* parent;
    tPhysics_object* child;
    tPhysics_object* group;
    int next_mark;

    parent = pObject1->parent;
    while (parent != NULL && pObject1->physics_joint1 != NULL && pObject1->physics_joint1->type != eJoint_none) {
        pObject1 = parent;
        parent = parent->parent;
    }
    parent = pObject2->parent;
    while (parent != NULL && pObject2->physics_joint1 != NULL && pObject2->physics_joint1->type != eJoint_none) {
        pObject2 = parent;
        parent = parent->parent;
    }
    if (pObject1 == pObject2) {
        return;
    }
    if (pObject2 == NULL) {
        if (pObject1->field_0x1dc != 0) {
            return;
        }
        next_mark = gINT_006940a0;
        pObject1->field_0x1dc = (tU8)next_mark;
        for (child = pObject1->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
                DRMatrix33Inverse(child, next_mark);
            }
        }
        pObject1->field183_0x1d8 = (undefined4)pObject1;
        gINT_006940a0++;
        return;
    }
    if (pObject1->field_0x1dc == 0) {
        if (pObject2->field_0x1dc == 0) {
            next_mark = gINT_006940a0;
            pObject1->field_0x1dc = (tU8)next_mark;
            for (child = pObject1->child; child != NULL; child = child->next) {
                if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
                    DRMatrix33Inverse(child, next_mark);
                }
            }
            next_mark = gINT_006940a0;
            pObject2->field_0x1dc = (tU8)next_mark;
            for (child = pObject2->child; child != NULL; child = child->next) {
                if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
                    DRMatrix33Inverse(child, next_mark);
                }
            }
            pObject1->field183_0x1d8 = (undefined4)pObject2;
            pObject2->field183_0x1d8 = (undefined4)pObject1;
            gINT_006940a0++;
            return;
        }
        pObject1->field_0x1dc = pObject2->field_0x1dc;
        for (child = pObject1->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
                DRMatrix33Inverse(child, (char)pObject2->field_0x1dc);
            }
        }
        pObject1->field183_0x1d8 = pObject2->field183_0x1d8;
        pObject2->field183_0x1d8 = (undefined4)pObject1;
        return;
    }
    if (pObject2->field_0x1dc == 0) {
        pObject2->field_0x1dc = pObject1->field_0x1dc;
        for (child = pObject2->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
                DRMatrix33Inverse(child, (char)pObject1->field_0x1dc);
            }
        }
        pObject2->field183_0x1d8 = pObject1->field183_0x1d8;
        pObject1->field183_0x1d8 = (undefined4)pObject2;
        return;
    }
    if (pObject1->field_0x1dc == pObject2->field_0x1dc) {
        return;
    }
    group = pObject2;
    while ((tPhysics_object*)group->field183_0x1d8 != pObject2) {
        group->field_0x1dc = pObject1->field_0x1dc;
        for (child = group->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
                DRMatrix33Inverse(child, (char)pObject1->field_0x1dc);
            }
        }
        group = (tPhysics_object*)group->field183_0x1d8;
    }
    group->field_0x1dc = pObject1->field_0x1dc;
    for (child = group->child; child != NULL; child = child->next) {
        if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
            DRMatrix33Inverse(child, (char)pObject1->field_0x1dc);
        }
    }
    group->field183_0x1d8 = pObject1->field183_0x1d8;
    pObject1->field183_0x1d8 = (undefined4)pObject2;
}

// FUNCTION: CARMA2_HW 0x004be0e0
void C2_HOOK_FASTCALL FUN_004be0e0(tPhysics_object* pObject, tPhysics_object* pList) {
    tPhysics_object* child;
    tPhysics_object* item;

    if (pObject->disable_move_rotate == 0) {
        return;
    }
    while (pObject->parent != NULL && pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none) {
        pObject = pObject->parent;
    }
    pObject->box_face_ref = gFace_num__car - 2;
    pObject->disable_move_rotate = 0;
    if (pObject->child != NULL || pObject->physics_joint1 != NULL || pObject->physics_joint2 != NULL) {
        if (pObject->field_0x218 == 0) {
            MoveJointedObject(pObject, 0.f);
        }
        for (child = pObject->child; child != NULL; child = child->next) {
            if (child->disable_move_rotate != 0) {
                child->box_face_ref = gFace_num__car - 2;
                child->disable_move_rotate = 0;
            }
            if (child->child != NULL) {
                MakeObjectListDoSomething(child->child);
            }
        }
    }
    for (item = pList; item != NULL; item = item->next) {
        if (item->disable_move_rotate == 0) {
            continue;
        }
        if (pObject->field_0xf4.min.v[0] - item->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                && pObject->field_0xf4.min.v[1] - item->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                && pObject->field_0xf4.min.v[2] - item->field_0xf4.max.v[2] <= gDOUBLE_0058a798
                && item->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                && item->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                && item->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] <= gDOUBLE_0058a798) {
            FUN_004be0e0(item, pList);
        }
        if (pObject->child != NULL && item->disable_move_rotate != 0) {
            child = pObject->child;
            if (child != NULL) {
                do {
                    if (child->field_0xf4.min.v[0] - item->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                        && child->field_0xf4.min.v[1] - item->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                        && child->field_0xf4.min.v[2] - item->field_0xf4.max.v[2] <= gDOUBLE_0058a798
                        && item->field_0xf4.min.v[0] - child->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                        && item->field_0xf4.min.v[1] - child->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                        && item->field_0xf4.min.v[2] - child->field_0xf4.max.v[2] <= gDOUBLE_0058a798) {
                        FUN_004be0e0(item, pList);
                        break;
                    }
                    child = child->next;
                } while (child != NULL);
            }
        }
        if (item->child != NULL) {
            CheckForObjectHierachyTouchingObjectList(pObject, item->child, pList);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern float gFLOAT_00692dd0;
extern float gFLOAT_00692dd4;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b9ce0
void C2_HOOK_FASTCALL MergeBoundsIntoParent(tPhysics_object* pObject, br_bounds3* pBox) {
    br_bounds3 local_box;
    float* pSrc;
    tPhysics_object* child;
    int i;

    GetNewBoundingBox(&pObject->field_0xf4, &pObject->bb1, &pObject->transform_matrix);
    GetNewBoundingBox(&local_box, &pObject->bb1, &pObject->actor->t.t.mat);
    for (i = 0; i < 3; i++) {
        if (pObject->field_0xf4.min.v[i] > local_box.min.v[i]) {
            pObject->field_0xf4.min.v[i] = local_box.min.v[i];
        }
        if (pObject->field_0xf4.max.v[i] < local_box.max.v[i]) {
            pObject->field_0xf4.max.v[i] = local_box.max.v[i];
        }
        pObject->field_0xf4.min.v[i] = pObject->field_0xf4.min.v[i] - 0.0025;
        pObject->field_0xf4.max.v[i] = pObject->field_0xf4.max.v[i] - -0.0025;
    }
    pObject->field_0x10c = pObject->field_0xf4;
    for (child = pObject->child; child != NULL; child = child->next) {
        MergeBoundsIntoParent(child, &pObject->field_0x10c);
    }
    if (pBox != NULL) {
        pSrc = &pObject->field_0x10c.min.v[0];
        for (i = 0; i < 3; i++) {
            if (pSrc[i] < pBox->min.v[i]) {
                pBox->min.v[i] = pSrc[i];
            }
            if (pSrc[i + 3] > pBox->max.v[i]) {
                pBox->max.v[i] = pSrc[i + 3];
            }
        }
    } else {
        if (gFLOAT_00692dd0 <= pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0]) {
            gFLOAT_00692dd0 = pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0];
        }
        if (gFLOAT_00692dd4 >= pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0]) {
            gFLOAT_00692dd4 = pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0];
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// 0x00676920 is the original's empty string. It must stay a bare "" literal here: an annotated
// `// GLOBAL:` definition makes reccmp render the operand as DATA, while the original's CSV row marks
// 0x676920 as a STRING, so a named global can never pair with it (measured on the v3cp clean run: it
// dropped EarnCredits, ReadUntilCharacter, InitShadow, ShapeRayCast, LoadMenuSettings and others).
// The previous gEmpty_string_00676920 definition was removed for that reason.

extern void C2_HOOK_FASTCALL FUN_004bd4f0(br_matrix34* pMatrix1, br_matrix34* pMatrix2, br_vector3* pChild1, br_vector3* pParent1, br_vector3* pChild2, br_vector3* pParent2, br_vector3* pOut1, br_vector3* pOut2);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004bca10
int C2_HOOK_FASTCALL CollectJointLimitsIntoBuffer(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count) {
    tPhysics_object* partner;
    int i;
    br_vector3 cross;
    br_vector3 vec;
    br_scalar dot;
    br_scalar dot2;
    tPhysics_joint_force* rec;

    if (pObject->physics_joint1 != NULL) {
        partner = pObject->field_0x230;
        if (partner == NULL) {
            PhysicsError((tPhysicsError)11, "");
        }
        i = 0;
        if (pObject->physics_joint1->count_limits > 0) {
            rec = &pContacts[pCount];
            do {
                if (pCount >= pMax_count) {
                    break;
                }
                switch (pObject->physics_joint1->limits[i].type) {
                case eJoint_limit_plane: {
                    br_vector3 vec2;
                    br_vector3 vec3;

                    BrMatrix34ApplyV(&vec, &pObject->physics_joint1->limits[i].child, &pObject->actor->t.t.mat);
                    BrMatrix34TApplyV(&vec2, &vec, &partner->actor->t.t.mat);
                    dot = pObject->physics_joint1->limits[i].parent.v[0] * vec2.v[0]
                        + pObject->physics_joint1->limits[i].parent.v[1] * vec2.v[1]
                        + pObject->physics_joint1->limits[i].parent.v[2] * vec2.v[2];
                    if (dot < 0.001) {
                        BrMatrix34ApplyV(&vec, &pObject->physics_joint1->limits[i].child, &pObject->transform_matrix);
                        BrMatrix34TApplyV(&vec3, &vec, &partner->transform_matrix);
                        dot2 = pObject->physics_joint1->limits[i].parent.v[0] * vec3.v[0]
                            + pObject->physics_joint1->limits[i].parent.v[1] * vec3.v[1]
                            + pObject->physics_joint1->limits[i].parent.v[2] * vec3.v[2];
                        if (0.0f <= dot2 && (0.001 <= dot2 || pObject->field_0xf0 != 0)) {
                            rec->field_0x0 = dot - dot2;
                            rec->type = 11;
                            rec->object_a = pObject;
                            rec->object_b = partner;
                            cross.v[0] = pObject->physics_joint1->limits[i].parent.v[2] * vec3.v[1]
                                - pObject->physics_joint1->limits[i].parent.v[1] * vec3.v[2];
                            cross.v[1] = pObject->physics_joint1->limits[i].parent.v[0] * vec3.v[2]
                                - pObject->physics_joint1->limits[i].parent.v[2] * vec3.v[0];
                            cross.v[2] = pObject->physics_joint1->limits[i].parent.v[1] * vec3.v[0]
                                - pObject->physics_joint1->limits[i].parent.v[0] * vec3.v[1];
                            dot2 = (br_scalar)sqrt(cross.v[0] * cross.v[0] + cross.v[1] * cross.v[1] + cross.v[2] * cross.v[2]);
                            if (dot2 <= 1e-06) {
                                BrMatrix34ApplyV(&cross, &pObject->omega, &pObject->transform_matrix);
                                BrMatrix34TApplyV(&rec->direction_b, &cross, &partner->transform_matrix);
                                rec->direction_b.v[0] = rec->direction_b.v[0] - partner->omega.v[0];
                                rec->direction_b.v[1] = rec->direction_b.v[1] - partner->omega.v[1];
                                rec->direction_b.v[2] = rec->direction_b.v[2] - partner->omega.v[2];
                                dot2 = (br_scalar)sqrt(rec->direction_b.v[0] * rec->direction_b.v[0]
                                    + rec->direction_b.v[1] * rec->direction_b.v[1]
                                    + rec->direction_b.v[2] * rec->direction_b.v[2]);
                                if (dot2 <= 2.3841858e-07f) {
                                    rec->direction_b.v[0] = 1.0f;
                                    rec->direction_b.v[1] = 0.0f;
                                    rec->direction_b.v[2] = 0.0f;
                                } else {
                                    rec->direction_b.v[0] = rec->direction_b.v[0] * (1.0 / dot2);
                                    rec->direction_b.v[1] = rec->direction_b.v[1] * (1.0 / dot2);
                                    rec->direction_b.v[2] = rec->direction_b.v[2] * (1.0 / dot2);
                                }
                            } else {
                                rec->direction_b.v[0] = cross.v[0] / dot2;
                                rec->direction_b.v[1] = cross.v[1] / dot2;
                                rec->direction_b.v[2] = cross.v[2] / dot2;
                            }
                            BrMatrix34ApplyV(&cross, &rec->direction_b, &partner->transform_matrix);
                            BrMatrix34TApplyV(&rec->direction_a, &cross, &pObject->transform_matrix);
                            rec->point_a.v[0] = pObject->physics_joint1->field_0x08.v[0];
                            rec->point_a.v[1] = pObject->physics_joint1->field_0x08.v[1];
                            rec->point_a.v[2] = pObject->physics_joint1->field_0x08.v[2];
                            rec->point_b.v[0] = pObject->physics_joint1->field_0x14.v[0];
                            rec->point_b.v[1] = pObject->physics_joint1->field_0x14.v[1];
                            rec->point_b.v[2] = pObject->physics_joint1->field_0x14.v[2];
                            pCount++;
                            rec++;
                        }
                    }
                    break;
                }
                case eJoint_limit_universal: {
                    br_vector3 out1;
                    br_vector3 out2;

                    FUN_004bd4f0(&pObject->actor->t.t.mat, &partner->actor->t.t.mat,
                                 &pObject->physics_joint1->limits[i].child, &pObject->physics_joint1->limits[i].parent,
                                 &pObject->physics_joint1->limits[i + 1].child, &pObject->physics_joint1->limits[i + 1].parent,
                                 &out1, &cross);
                    dot = pObject->physics_joint1->limits[i + 1].parent.v[0] * out1.v[0]
                        + pObject->physics_joint1->limits[i + 1].parent.v[2] * out1.v[2]
                        + pObject->physics_joint1->limits[i + 1].parent.v[1] * out1.v[1];
                    if (dot < pObject->physics_joint1->limits[i].value - -0.001) {
                        FUN_004bd4f0(&pObject->transform_matrix, &partner->transform_matrix,
                                     &pObject->physics_joint1->limits[i].child, &pObject->physics_joint1->limits[i].parent,
                                     &pObject->physics_joint1->limits[i + 1].child, &pObject->physics_joint1->limits[i + 1].parent,
                                     &out2, &rec->direction_b);
                        dot2 = pObject->physics_joint1->limits[i + 1].parent.v[2] * out2.v[2]
                            + pObject->physics_joint1->limits[i + 1].parent.v[1] * out2.v[1]
                            + pObject->physics_joint1->limits[i + 1].parent.v[0] * out2.v[0];
                        if (pObject->physics_joint1->limits[i].value < dot2) {
                            cross.v[0] = out1.v[1] * out2.v[2] - out1.v[2] * out2.v[1];
                            cross.v[1] = out1.v[2] * out2.v[0] - out1.v[0] * out2.v[2];
                            cross.v[2] = out1.v[0] * out2.v[1] - out1.v[1] * out2.v[0];
                            if (rec->direction_b.v[0] * cross.v[0] + rec->direction_b.v[2] * cross.v[2] + rec->direction_b.v[1] * cross.v[1] < 0.0f) {
                                rec->direction_b.v[0] = -rec->direction_b.v[0];
                                rec->direction_b.v[1] = -rec->direction_b.v[1];
                                rec->direction_b.v[2] = -rec->direction_b.v[2];
                            }
                            BrMatrix34ApplyV(&cross, &rec->direction_b, &partner->transform_matrix);
                            BrMatrix34TApplyV(&rec->direction_a, &cross, &pObject->transform_matrix);
                            rec->field_0x0 = dot - dot2;
                            rec->type = 11;
                            rec->object_a = pObject;
                            rec->object_b = partner;
                            pCount++;
                            rec++;
                        }
                    }
                    i++;
                    break;
                }
                default:
                    PhysicsError((tPhysicsError)8, "");
                    break;
                }
                i++;
            } while (i < pObject->physics_joint1->count_limits);
        }
    }
    if (pObject->physics_joint2 == NULL || pObject->physics_joint2->count_limits < 1) {
        return pCount;
    }
    i = 0;
    do {
        switch (pObject->physics_joint2->limits[i].type) {
        case eJoint_limit_plane:
            BrMatrix34ApplyV(&vec, &pObject->physics_joint2->limits[i].child, &pObject->actor->t.t.mat);
            dot = pObject->physics_joint2->limits[i].parent.v[0] * vec.v[0]
                + pObject->physics_joint2->limits[i].parent.v[1] * vec.v[1]
                + pObject->physics_joint2->limits[i].parent.v[2] * vec.v[2];
            if (dot < 0.001) {
                if (pCount == pMax_count) {
                    return pCount;
                }
                pContacts[pCount].field_0x0 = dot;
                pContacts[pCount].type = 11;
                pContacts[pCount].object_a = pObject;
                pContacts[pCount].object_b = NULL;
                cross.v[0] = pObject->physics_joint2->limits[i].parent.v[2] * vec.v[1]
                    - pObject->physics_joint2->limits[i].parent.v[1] * vec.v[2];
                cross.v[1] = pObject->physics_joint2->limits[i].parent.v[0] * vec.v[2]
                    - pObject->physics_joint2->limits[i].parent.v[2] * vec.v[0];
                cross.v[2] = pObject->physics_joint2->limits[i].parent.v[1] * vec.v[0]
                    - pObject->physics_joint2->limits[i].parent.v[0] * vec.v[1];
                dot2 = (br_scalar)sqrt(cross.v[0] * cross.v[0] + cross.v[1] * cross.v[1] + cross.v[2] * cross.v[2]);
                if (1e-06 < dot2) {
                    pContacts[pCount].direction_b.v[0] = cross.v[0] / dot2;
                    pContacts[pCount].direction_b.v[1] = cross.v[1] / dot2;
                    pContacts[pCount].direction_b.v[2] = cross.v[2] / dot2;
                }
            }
            break;
        case eJoint_limit_10: {
            int n;
            int m;
            if (pObject->field_0x1e0[0] != 0) {
                gINT_00692dd8 = 1;
                for (n = 0; n < pCount; n++) {
                    if (pContacts[n].object_a == pObject) {
                        if (pContacts[n].object_b == NULL) {
                            if (n < pCount) {
                                m = pCount - n;
                                rec = &pContacts[n];
                                do {
                                    *rec = rec[1];
                                    rec++;
                                } while (--m != 0);
                            }
                            pCount--;
                            n--;
                        } else {
                            rec = &pContacts[n];
                            cross.v[0] = -rec->direction_a.v[0];
                            cross.v[1] = -rec->direction_a.v[1];
                            cross.v[2] = -rec->direction_a.v[2];
                            rec->direction_a.v[0] = -rec->direction_b.v[0];
                            rec->direction_a.v[1] = -rec->direction_b.v[1];
                            rec->direction_a.v[2] = -rec->direction_b.v[2];
                            rec->direction_b.v[0] = cross.v[0];
                            rec->direction_b.v[1] = cross.v[1];
                            rec->direction_b.v[2] = cross.v[2];
                            cross = rec->point_a;
                            rec->point_a = rec->point_b;
                            rec->point_b = cross;
                            rec->object_a = rec->object_b;
                            rec->object_b = pObject;
                            rec->type = 14;
                        }
                    } else if (pContacts[n].object_b == pObject) {
                        pContacts[n].type = 14;
                    }
                }
            }
            break;
        }
        case eJoint_limit_11: {
            br_scalar t0;
            br_scalar len;
            br_vector3 vecB;
            BrMatrix34ApplyV(&vecB, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
            dot = pObject->field_0x17c.v[0] * vecB.v[0]
                + pObject->field_0x17c.v[2] * vecB.v[2]
                + pObject->field_0x17c.v[1] * vecB.v[1];
            t0 = pObject->physics_joint2->limits[i].value + dot;
            dot2 = pObject->actor->t.t.mat.m[3][0] * vecB.v[0]
                + pObject->actor->t.t.mat.m[3][1] * vecB.v[1]
                + pObject->actor->t.t.mat.m[3][2] * vecB.v[2];
            len = pObject->transform_matrix.m[3][2] * vecB.v[2]
                + pObject->transform_matrix.m[3][1] * vecB.v[1]
                + pObject->transform_matrix.m[3][0] * vecB.v[0];
            if (dot2 <= t0 || t0 - -0.001 <= len) {
                if (dot2 < dot - 0.001 && dot - 0.002 < len) {
                    if (pCount == pMax_count) {
                        return pCount;
                    }
                    pContacts[pCount].field_0x0 = (dot - len) / (dot2 - len);
                    pContacts[pCount].field_0xc = NULL;
                    pContacts[pCount].type = 2;
                    pContacts[pCount].object_a = pObject;
                    pContacts[pCount].object_b = NULL;
                    pContacts[pCount].direction_a = pObject->physics_joint2->hinge_axis;
                    BrVector3SetFloat(&pContacts[pCount].point_a, 0.0f, 0.0f, 0.0f);
                    pCount++;
                }
            } else {
                if (pCount == pMax_count) {
                    return pCount;
                }
                pContacts[pCount].field_0x0 = (t0 - len) / (dot2 - len);
                pContacts[pCount].field_0xc = NULL;
                pContacts[pCount].type = 2;
                pContacts[pCount].object_a = pObject;
                pContacts[pCount].object_b = NULL;
                pContacts[pCount].direction_a.v[0] = -pObject->physics_joint2->hinge_axis.v[0];
                pContacts[pCount].direction_a.v[1] = -pObject->physics_joint2->hinge_axis.v[1];
                pContacts[pCount].direction_a.v[2] = -pObject->physics_joint2->hinge_axis.v[2];
                BrVector3SetFloat(&pContacts[pCount].point_a, 0.0f, 0.0f, 0.0f);
                pCount++;
            }
            break;
        }
        default:
            PhysicsError((tPhysicsError)8, "");
            break;
        }
        i++;
    } while (i < pObject->physics_joint2->count_limits);
    return pCount;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004bd6d0
int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count) {
    if (pObject != NULL) {
        do {
            pCount = CollectJointLimitsIntoBuffer(pObject, pContacts, pCount, pMax_count);
            if (pObject->child != NULL) {
                pCount = CollectJointLimitsOfTree(pObject->child, pContacts, pCount, pMax_count);
            }
            pObject = pObject->next;
        } while (pObject != NULL);
    }
    return pCount;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00428cf0
int C2_HOOK_FASTCALL CopyPhysicsJointData(tPhysics_joint_force* pDestination, int pDestination_index, const tPhysics_joint_force* pSource, int pCount) {
    int i;

    for (i = 0; i < pCount && pDestination_index < 40; i++) {
        if (pSource[i].type != 9) {
            memcpy(&pDestination[pDestination_index], &pSource[i], sizeof(*pSource));
            pDestination_index++;
        }
    }
    return pDestination_index;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004c0430
int C2_HOOK_FASTCALL FUN_004c0430(br_scalar* pRhs, tPhysics_joint_force** pRecords, int pCount, int pCollision_count) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004c3b60
int C2_HOOK_FASTCALL FUN_004c3b60(br_scalar* pMatrix, br_scalar* pRhs, br_scalar* pX, int pCount, int pStride, tU8* pFlags, int* pOut, int pJoint_count, int pCollision_count, tPhysics_joint_force_group** pGroups, int pGroup_count) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004bc190
void C2_HOOK_FASTCALL FUN_004bc190(tPhysics_joint_force** pRecords, br_scalar* pImpulses, int pCount, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004bd720
int C2_HOOK_FASTCALL ClampObjectSpin(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004bd7f0
int C2_HOOK_FASTCALL ClampObjectSpinTree(tPhysics_object* pObject) {
    int count;

    count = 0;
    for (; pObject != NULL; pObject = pObject->next) {
        count += ClampObjectSpin(pObject);
        if (pObject->child != NULL) {
            count += ClampObjectSpinTree(pObject->child);
        }
    }
    return count;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004baec0
int C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object** pObject_list, tPhysics_object* pObject, tPhysics_joint_force* pContacts, tPhysics_joint_force* pJoint_data, int pJoint_count, tWorld_callbacks* pWorld_callbacks) {
    tPhysics_joint_force* records[100];
    tU8 solver_flags[100];
    tPhysics_joint_force* group_records[40];
    br_scalar x[100];
    br_scalar rhs[100];
    tPhysics_joint_force_group* groups[100];
    br_scalar velocity[100];
    br_scalar impulses[100];
    tPhysics_object* obj;
    tPhysics_object* next;
    tPhysics_object* head;
    tPhysics_object* child;
    tPhysics_object* overlapping;
    tPhysics_object* old;
    tPhysics_object* obj_a;
    tPhysics_object* obj_b;
    tPhysics_joint_force* c;
    tPhysics_joint_force* rec;
    tPhysics_joint_force** records_c;
    tPhysics_joint_force_group* group;
    tPhysics_joint* joint;
    br_scalar* x_c;
    br_scalar* rhs_c;
    br_vector3 w;
    br_vector3 p;
    br_vector3 q;
    br_vector3 r;
    br_scalar len;
    br_scalar limit;
    br_scalar max;
    br_scalar s;
    int count;
    int prev_count;
    int n;
    int m;
    int total;
    int num_records;
    int num_groups;
    int i;
    int j;
    int flag;
    int found;
    int spin;
    int sub_spin;
    int cb_flags;
    int solver_out;

    count = 0;
    obj = pObject;
    do {
        next = (tPhysics_object*)obj->field183_0x1d8;
        prev_count = count;
        head = *pObject_list;
        AppendOverlappingObjects(obj, head, &overlapping);
        count = FUN_004bd9e0(obj, &overlapping, pContacts, count, pWorld_callbacks, head);
        if (gNet_mode == eNet_mode_host && obj->field_0xf0 == 1 && count > prev_count) {
            p.v[0] = obj->transform_matrix.m[3][0] - ((br_matrix34*)&obj->field_0xbc)->m[3][0];
            p.v[1] = obj->transform_matrix.m[3][1] - ((br_matrix34*)&obj->field_0xbc)->m[3][1];
            p.v[2] = obj->transform_matrix.m[3][2] - ((br_matrix34*)&obj->field_0xbc)->m[3][2];
            len = BrVector3Length(&obj->v);
            if (len - -0.5 < BrVector3Length(&p) * 25.f) {
                obj->flags |= 0x1000;
                obj->field_0xf0 = 0;
                cb_flags = 0;
                obj = pObject;
                do {
                    if (!obj->disable_move_rotate) {
                        if (obj->field_0xf0 == 1) {
                            BrMatrix34Copy(&obj->actor->t.t.mat, (const br_matrix34*)&obj->field_0xbc);
                        } else {
                            BrMatrix34LPNormalise(&obj->actor->t.t.mat, &obj->transform_matrix);
                        }
                        for (child = obj->child; child != NULL; child = child->next) {
                            ResetObjectAndChildren(child);
                        }
                        FUN_004b8ee0(obj, 0.04f);
                        MergeBoundsIntoParent(obj, NULL);
                        if (*(tQuickHinge_moved_cbfn**)&obj->field_0x191[3] != NULL) {
                            cb_flags |= (*(tQuickHinge_moved_cbfn**)&obj->field_0x191[3])(obj);
                        }
                    }
                    obj = (tPhysics_object*)obj->field183_0x1d8;
                } while (obj != pObject);
                return -1;
            }
        }
        obj = next;
    } while (obj != pObject);

    gINT_00692dd8 = 0;
    m = count;
    i = 0;
    obj = pObject;
    do {
        i++;
        if (i > 100) {
            PhysicsError((tPhysicsError)10, "Corrupt group");
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    obj = pObject;
    do {
        if (!(obj->flags & 0x10)) {
            prev_count = m;
            m = CollectJointLimitsIntoBuffer(obj, pContacts, m, 40);
            if (obj->child != NULL) {
                for (child = obj->child; child != NULL; child = child->next) {
                    m = CollectJointLimitsIntoBuffer(child, pContacts, m, 40);
                    if (child->child != NULL) {
                        m = CollectJointLimitsOfTree(child->child, pContacts, m, 40);
                    }
                }
            }
            if (prev_count != m && obj->field_0xf0 == 2) {
                old = obj;
                while (old->parent != NULL && old->physics_joint1 != NULL && old->physics_joint1->type != 0) {
                    old = old->parent;
                }
                old->field_0xf0 = 0;
                for (child = old->child; child != NULL; child = child->next) {
                    SetFieldF0Tree(child, 0);
                }
                if (pWorld_callbacks != NULL && pWorld_callbacks->activate_passive != NULL) {
                    ((void (C2_HOOK_FASTCALL*)(tPhysics_object*, int))pWorld_callbacks->activate_passive)(old, 0);
                }
                if ((old->child != NULL || old->physics_joint2 != NULL) && old->field_0x218 == 0) {
                    MoveJointedObject(old, 0.f);
                }
            }
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    if (m == 0) {
        return 0;
    }
    for (i = 0; i < m; i++) {
        c = &pContacts[i];
        old = c->object_a;
        while (c->object_a->flags & 0x400) {
            c->object_a = c->object_a->parent;
        }
        if (old != c->object_a) {
            BrVector3Accumulate(&c->point_a, &old->cmpos);
            BrMatrix34ApplyP(&w, &c->point_a, &old->transform_matrix);
            DrMatrix34ApplyLPInverse(&c->point_a, &w, &c->object_a->transform_matrix);
            BrVector3Sub(&c->point_a, &c->point_a, &c->object_a->cmpos);
            BrMatrix34ApplyV(&w, &c->direction_a, &old->transform_matrix);
            BrMatrix34TApplyV(&c->direction_a, &w, &c->object_a->transform_matrix);
        }
        old = c->object_b;
        if (old != NULL) {
            while (c->object_b->flags & 0x400) {
                c->object_b = c->object_b->parent;
            }
            if (old != c->object_a) {
                BrVector3Accumulate(&c->point_b, &old->cmpos);
                BrMatrix34ApplyP(&w, &c->point_b, &old->transform_matrix);
                DrMatrix34ApplyLPInverse(&c->point_b, &w, &c->object_b->transform_matrix);
                BrVector3Sub(&c->point_b, &c->point_b, &c->object_b->cmpos);
                BrMatrix34ApplyV(&w, &c->direction_b, &old->transform_matrix);
                BrMatrix34TApplyV(&c->direction_b, &w, &c->object_b->transform_matrix);
            }
        }
    }
    n = m;
    m = CopyPhysicsJointData(pContacts, m, pJoint_data, pJoint_count);
    num_records = 0;
    num_groups = 0;
    obj = pObject;
    do {
        if (obj->field_0x218 != 0) {
            groups[num_groups] = (tPhysics_joint_force_group*)obj->field_0x218;
            num_groups++;
            rec = ((tPhysics_joint_force_group*)obj->field_0x218)->records;
            for (j = 0; j < ((tPhysics_joint_force_group*)obj->field_0x218)->count; j++) {
                records[num_records] = rec;
                rec++;
                num_records++;
            }
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    for (i = 0; i < m; i++) {
        records[num_records + i] = &pContacts[i];
    }
    if (num_records + m > 100) {
        m = 100 - num_records;
    }
    if (n > m) {
        n = m;
    }
    total = num_records + m;
    x_c = &x[num_records];
    FUN_004bf490(gPhysics_buffer3_ptr, 100, records, total);
    obj = pObject;
    do {
        obj->omega = obj->rotate_omega;
        for (child = obj->child; child != NULL; child = child->next) {
            child->omega = child->rotate_omega;
            if (child->child != NULL) {
                RestoreListOmega(child->child);
            }
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    records_c = &records[num_records];
    rhs_c = &rhs[num_records];
    flag = 0;
    if (gINT_0065d014 != 0) {
        flag = FUN_004c0430(rhs_c, records_c, m, n);
    } else {
        for (i = n; i < m; i++) {
            rhs_c[i] = 0.f;
        }
        for (i = 0; i < n; i++) {
            c = records_c[i];
            obj_a = c->object_a;
            if (c->type == 11) {
                rhs_c[i] = -BrVector3Dot(&obj_a->omega, &c->direction_a);
                obj_b = c->object_b;
                if (obj_b != NULL) {
                    rhs_c[i] += BrVector3Dot(&obj_b->omega, &c->direction_b);
                }
                if (rhs_c[i] < 0.001) {
                    rhs_c[i] = 0.001f;
                } else {
                    flag = 1;
                }
            } else {
                BrVector3Cross(&w, &obj_a->omega, &c->point_a);
                BrVector3Accumulate(&w, &obj_a->velocity_car_space);
                rhs_c[i] = -BrVector3Dot(&c->direction_a, &w);
                obj_b = c->object_b;
                if (obj_b != NULL) {
                    BrVector3Cross(&w, &obj_b->omega, &c->point_b);
                    BrVector3Accumulate(&w, &obj_b->velocity_car_space);
                    rhs_c[i] += BrVector3Dot(&c->direction_b, &w);
                }
                if (rhs_c[i] < 0.f) {
                    rhs_c[i] = 0.f;
                } else if (rhs_c[i] > 0.01) {
                    flag = 1;
                }
                rhs_c[i] = 0.01 - rhs_c[i] * -1.001;
            }
        }
        if (flag == 0) {
            for (i = 0; i < n; i++) {
                c = records_c[i];
                if (c->type == 11) {
                    rhs_c[i] = (0.001 - c->field_0x0) * 25.0;
                } else if (c->object_b != NULL) {
                    BrVector3Add(&p, &c->object_a->cmpos, &c->point_a);
                    BrMatrix34ApplyP(&w, &p, &c->object_a->transform_matrix);
                    DrMatrix34ApplyLPInverse(&q, &w, &c->object_b->transform_matrix);
                    BrMatrix34ApplyP(&w, &p, &c->object_a->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&r, &w, &c->object_b->actor->t.t.mat);
                    BrVector3Sub(&q, &r, &q);
                    if (c->type == 5) {
                        BrMatrix34ApplyV(&w, &c->direction_b, &c->object_b->transform_matrix);
                        BrMatrix34TApplyV(&r, &w, &c->object_b->actor->t.t.mat);
                        rhs_c[i] = -(BrVector3Dot(&r, &q) * 25.f);
                    } else {
                        rhs_c[i] = -(BrVector3Dot(&c->direction_b, &q) * 25.f);
                    }
                } else {
                    BrVector3Add(&p, &c->object_a->cmpos, &c->point_a);
                    BrMatrix34ApplyP(&w, &p, &c->object_a->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&q, &w, &c->object_a->transform_matrix);
                    BrVector3Sub(&q, &q, &p);
                    rhs_c[i] = 0.01 - BrVector3Dot(&c->direction_a, &q) * 25.f;
                }
                if (rhs_c[i] < 0.01) {
                    rhs_c[i] = 0.01f;
                } else {
                    flag = 1;
                }
            }
            if (flag == 0) {
                for (i = 0; i < n; i++) {
                    if (records_c[i]->type != 11) {
                        rhs_c[i] = 0.05f;
                    }
                }
            }
            flag = 1;
        } else {
            flag = 0;
        }
    }
    FUN_004b77b0(rhs, records, num_records);
    if (FUN_004c3b60(gPhysics_buffer3_ptr, rhs, x, total, 100, solver_flags, &solver_out, num_records, n, groups, num_groups) != 0) {
        gINT_006940a4++;
    }
    if (gINT_00692dd8 != 0) {
        found = 0;
        obj = pObject;
        do {
            joint = obj->physics_joint2;
            if (joint != NULL && obj->field_0x1e0[0] != 0) {
                for (i = 0; i < joint->count_limits; i++) {
                    if (joint->limits[i].type == eJoint_limit_10) {
                        break;
                    }
                }
                if (i != joint->count_limits) {
                    limit = joint->limits[i].value;
                    BrVector3SetFloat(&q, 0.f, 0.f, 0.f);
                    for (j = 0; j < m; j++) {
                        if (pContacts[j].object_a == obj) {
                            BrVector3Sub(&w, &pContacts[j].point_a, &obj->physics_joint2->field_0x08);
                            BrVector3Cross(&r, &w, &pContacts[j].direction_a);
                            BrVector3Scale(&r, &r, x_c[j]);
                            BrVector3Accumulate(&q, &r);
                        } else if (pContacts[j].object_b == obj) {
                            BrVector3Sub(&w, &pContacts[j].point_b, &obj->physics_joint2->field_0x08);
                            BrVector3Cross(&r, &w, &pContacts[j].direction_b);
                            s = -x_c[j];
                            BrVector3Scale(&r, &r, s);
                            BrVector3Accumulate(&q, &r);
                        }
                    }
                    if (BrVector3LengthSquared(&q) > limit) {
                        found = 1;
                        obj->field_0x1e0[0] = 0;
                        for (j = 0; j < m; j++) {
                            if (pContacts[j].object_a == obj || pContacts[j].object_b == obj) {
                                pContacts[j].type = 5;
                            }
                        }
                    } else {
                        obj->disable_move_rotate = 1;
                    }
                }
            }
            obj = (tPhysics_object*)obj->field183_0x1d8;
        } while (obj != pObject);
        if (found) {
            FUN_004bf490(gPhysics_buffer3_ptr, 100, records, total);
            FUN_004c3b60(gPhysics_buffer3_ptr, rhs, x, total, 100, solver_flags, &solver_out, num_records, n, groups, num_groups);
        }
    }
    max = 0.f;
    for (i = 0; i < n; i++) {
        if (max < x_c[i]) {
            max = x_c[i];
        }
    }
    (void)max;
    for (i = 0; i < num_records; i++) {
        if (x[i] > 100.f) {
            i++;
        }
    }
    for (i = 0; i < m; i++) {
        if (x_c[i] == 0.f) {
            pContacts[i].type = 9;
        }
    }
    gINT_00692dc8 = 0;
    if (pWorld_callbacks->process_forces != NULL) {
        pWorld_callbacks->process_forces(pContacts, x_c, m);
    }
    if (gINT_00692dc8 == 0) {
        if (pWorld_callbacks->process_joint_forces != NULL) {
            ((void (C2_HOOK_FASTCALL*)(tPhysics_joint_force**, br_scalar*, int))pWorld_callbacks->process_joint_forces)(records, x, num_records);
        }
        if (gINT_00692dc8 == 0) {
            for (i = 0; i < num_records; i++) {
                FUN_004b7960(records[i], x[i]);
            }
        }
    }
    for (i = 0; i < m; i++) {
        s = x_c[i];
        if (s != 0.f && pContacts[i].type != 9) {
            obj_b = pContacts[i].object_b;
            obj_a = pContacts[i].object_a;
            FUN_004b7960(&pContacts[i], s);
            if (pContacts[i].type != 11) {
                if (obj_b != NULL) {
                    if ((obj_a->collision_flag & 2) && (obj_b->collision_flag & 2)) {
                        continue;
                    }
                } else if (obj_a->collision_flag & 1) {
                    continue;
                }
                if (gINT_00692ddc < 50) {
                    c = &gCollision_contacts[gINT_00692ddc];
                    *c = pContacts[i];
                    gINT_00692ddc++;
                    c->field_0x0 = x_c[i];
                }
            }
        }
    }
    if (flag != 0 && gINT_00692dc8 == 0) {
        FUN_004bc190(records, x, num_records, pWorld_callbacks);
    }
    i = 0;
    obj = pObject;
    do {
        i++;
        if (i > 100) {
            PhysicsError((tPhysicsError)10, "Corrupt group");
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    spin = 0;
    obj = pObject;
    do {
        spin += ClampObjectSpin(obj);
        if (obj->child != NULL) {
            sub_spin = 0;
            for (child = obj->child; child != NULL; child = child->next) {
                sub_spin += ClampObjectSpin(child);
                if (child->child != NULL) {
                    sub_spin += ClampObjectSpinTree(child->child);
                }
            }
            spin += sub_spin;
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    if (spin != 0) {
        obj = pObject;
        do {
            group = (tPhysics_joint_force_group*)obj->field_0x218;
            if (group != NULL) {
                for (i = 0; i < group->count; i++) {
                    group_records[i] = &group->records[i];
                }
                FUN_004b77b0(velocity, group_records, group->count);
                RqBackSub(group->matrix, group->orthogonal, velocity, impulses, group->count);
                for (i = 0; i < group->count; i++) {
                    FUN_004b7960(group_records[i], impulses[i]);
                }
            }
            obj = (tPhysics_object*)obj->field183_0x1d8;
        } while (obj != pObject);
    }
    cb_flags = 0;
    obj = pObject;
    do {
        if (!obj->disable_move_rotate) {
            if (obj->field_0xf0 == 1) {
                BrMatrix34Copy(&obj->actor->t.t.mat, (const br_matrix34*)&obj->field_0xbc);
            } else {
                BrMatrix34LPNormalise(&obj->actor->t.t.mat, &obj->transform_matrix);
            }
            for (child = obj->child; child != NULL; child = child->next) {
                ResetObjectAndChildren(child);
            }
            FUN_004b8ee0(obj, 0.04f);
            MergeBoundsIntoParent(obj, NULL);
            if (*(tQuickHinge_moved_cbfn**)&obj->field_0x191[3] != NULL) {
                cb_flags |= (*(tQuickHinge_moved_cbfn**)&obj->field_0x191[3])(obj);
            }
        }
        obj = (tPhysics_object*)obj->field183_0x1d8;
    } while (obj != pObject);
    if ((cb_flags | spin) != 0) {
        m = -m;
    }
    return m;
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004269c0
int C2_HOOK_FASTCALL FUN_004269c0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00427870
int C2_HOOK_FASTCALL FUN_00427870(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0045d5e0
void C2_HOOK_FASTCALL FUN_0045d5e0(tCollision_shape_sphere_data* pSphere1, tCollision_shape_sphere_data* pSphere2, br_vector3* pStart, br_vector3* pEnd, br_vector3* pNormal, float* pFactor) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/*
 * Universal-joint axis builder. pOut2 = normalise(pMatrix2^T * (pMatrix1 * pChild1) + pParent1),
 * falling back to (1,0,0) for a degenerate vector, then pOut1 = pMatrix2^T * (pMatrix1 * pChild2)
 * reflected against pOut2 and against pParent1. pParent2 is unused.
 */
// FUNCTION: CARMA2_HW 0x004bd4f0
void C2_HOOK_FASTCALL FUN_004bd4f0(br_matrix34* pMatrix1, br_matrix34* pMatrix2, br_vector3* pChild1, br_vector3* pParent1, br_vector3* pChild2, br_vector3* pParent2, br_vector3* pOut1, br_vector3* pOut2) {
    br_vector3 vec[2];
    float d;
    float t1;
    float t2;

    BrMatrix34ApplyV(&vec[0], pChild1, pMatrix1);
    BrMatrix34TApplyV(&vec[1], &vec[0], pMatrix2);
    pOut2->v[0] = vec[1].v[0] + pParent1->v[0];
    pOut2->v[1] = vec[1].v[1] + pParent1->v[1];
    vec[1].v[2] = vec[1].v[2] + pParent1->v[2];
    pOut2->v[2] = vec[1].v[2];
    d = sqrt(pOut2->v[0] * pOut2->v[0] + vec[1].v[2] * vec[1].v[2] + pOut2->v[1] * pOut2->v[1]);
    if (d <= 2.3841858e-07f) {
        pOut2->v[0] = 1.0f;
        pOut2->v[1] = 0.0f;
        pOut2->v[2] = 0.0f;
    } else {
        d = 1.0 / d;
        pOut2->v[0] = pOut2->v[0] * d;
        pOut2->v[1] = pOut2->v[1] * d;
        pOut2->v[2] = pOut2->v[2] * d;
    }
    BrMatrix34ApplyV(&vec[0], pChild2, pMatrix1);
    BrMatrix34TApplyV(pOut1, &vec[0], pMatrix2);
    d = pOut1->v[1] * pOut2->v[1] + pOut2->v[2] * pOut1->v[2] + pOut1->v[0] * pOut2->v[0];
    d = d + d;
    t1 = pOut1->v[1] - pOut2->v[1] * d;
    t2 = pOut1->v[2] - pOut2->v[2] * d;
    pOut1->v[0] = pOut1->v[0] - d * pOut2->v[0];
    pOut1->v[1] = t1;
    pOut1->v[2] = t2;
    d = pParent1->v[2] * pOut1->v[2] + pParent1->v[1] * pOut1->v[1] + pOut1->v[0] * pParent1->v[0];
    d = d + d;
    pOut1->v[0] = pOut1->v[0] - d * pParent1->v[0];
    pOut1->v[1] = pOut1->v[1] - pParent1->v[1] * d;
    pOut1->v[2] = pOut1->v[2] - pParent1->v[2] * d;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00421fd0
tCollision_shape_polyhedron_data* C2_HOOK_FASTCALL FUN_00421fd0(int pCount_points) {
    tCollision_shape_polyhedron_data* result;
    int count_minus_two;
    int count_plus_two;
    int count_edges;

    count_minus_two = pCount_points - 2;
    count_plus_two = pCount_points + 2;
    count_edges = count_minus_two * 3;
    result = BrMemAllocate(2 * (count_edges + count_minus_two * 16 + count_plus_two * 6), 0x80);
    if (result == NULL) {
        return NULL;
    }
    result->points = (br_vector3*)((tU8*)result + sizeof(*result));
    result->count_points = 0;
    result->count_edges = 0;
    result->edges = (tPolyhedron_edge_indexes*)(result->points + pCount_points);
    result->count_planes = 0;
    result->planes = (br_vector4*)(result->edges + count_edges);
    return result;
}

extern tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron);

// PARTIAL 79.65%: FUN_00422030 0x00422030 - helper pulled in by FUN_00422500; the original starts both distance sums from the float 0.0f global at 0x00589954 (fld ds:0x589954). A gFLOAT_00589954 global was measured twice: neutral for this function (79.65 either way) but it moved data layout and cost FUN_004be350 0.81% (32.26->31.45), so it was reverted. Residual is register/x87 allocation (distances in ecx vs edi) and the inner distance loop's expression ordering (original loads all six components then does three fsubp; ours interleaves fsub with the loads).
// FUNCTION: CARMA2_HW 0x00422030
int C2_HOOK_FASTCALL FUN_00422030(br_model* pModel, int pCount_vertices, int* pVertex_indices) {
    float* distances;
    int removed[512];
    int count_vertices;
    int distance_offset;
    int remove_count;
    int i;

    count_vertices = pModel->nvertices;
    if (count_vertices > 512) {
        count_vertices = 512;
    }
    if (count_vertices <= pCount_vertices) {
        return 3;
    }
    distances = BrMemAllocate(count_vertices * count_vertices / 2 * sizeof(float), 0x80);
    if (distances == NULL) {
        return 4;
    }

    distance_offset = 0;
    i = 0;
    if (count_vertices != 0) {
        int vertex_offset;

        vertex_offset = 0;
        do {
            if (vertex_offset > 0) {
                float* row;
                int j;
                int inner_offset;

                row = distances + distance_offset;
                distance_offset += i;
                j = i;
                inner_offset = 0;
                do {
                    row[0] =
                        (*(float*)((char*)pModel->vertices + vertex_offset + 8) - *(float*)((char*)pModel->vertices + inner_offset + 8)) * (*(float*)((char*)pModel->vertices + vertex_offset + 8) - *(float*)((char*)pModel->vertices + inner_offset + 8))
                        + (*(float*)((char*)pModel->vertices + vertex_offset + 4) - *(float*)((char*)pModel->vertices + inner_offset + 4)) * (*(float*)((char*)pModel->vertices + vertex_offset + 4) - *(float*)((char*)pModel->vertices + inner_offset + 4))
                        + (*(float*)((char*)pModel->vertices + vertex_offset) - *(float*)((char*)pModel->vertices + inner_offset)) * (*(float*)((char*)pModel->vertices + vertex_offset) - *(float*)((char*)pModel->vertices + inner_offset));
                    row++;
                    inner_offset += 0x28;
                    j--;
                } while (j != 0);
            }
            i++;
            vertex_offset += 0x28;
        } while (i < count_vertices);
    }

    memset(removed, 0, count_vertices * sizeof(removed[0]));
    remove_count = count_vertices - pCount_vertices;
    do {
        int index1;
        int index2;
        float min_distance;
        float sum1;
        float sum2;

        min_distance = 3.4028235e+38f;
        for (i = 0; i < count_vertices; i++) {
            if (removed[i] == 0) {
                int j;

                for (j = 0; j < i; j++) {
                    if (removed[j] == 0) {
                        int distance_index;
                        float distance;

                        if (j < i) {
                            distance_index = ((i - 1) * i >> 1) + j;
                        } else {
                            distance_index = ((j - 1) * j >> 1) + i;
                        }
                        distance = distances[distance_index];
                        if (distance < min_distance) {
                            index1 = i;
                            index2 = j;
                            min_distance = distance;
                        }
                    }
                }
            }
        }

        sum1 = 0.0f;
        sum2 = 0.0f;
        for (i = 0; i < count_vertices; i++) {
            if (removed[i] == 0 && i != index1 && i != index2) {
                int distance_index;

                if (i < index1) {
                    distance_index = ((index1 - 1) * index1 >> 1) + i;
                } else {
                    distance_index = ((i - 1) * i >> 1) + index1;
                }
                sum1 = (float)sqrt(distances[distance_index]) + sum1;

                if (i < index2) {
                    distance_index = ((index2 - 1) * index2 >> 1) + i;
                } else {
                    distance_index = ((i - 1) * i >> 1) + index2;
                }
                sum2 = (float)sqrt(distances[distance_index]) + sum2;
            }
        }
        if (sum1 >= sum2) {
            removed[index2] = 1;
        } else {
            removed[index1] = 1;
        }
        remove_count--;
    } while (remove_count != 0);

    for (i = 0; i < count_vertices; i++) {
        if (removed[i] == 0) {
            *pVertex_indices = i;
            pVertex_indices++;
        }
    }
    BrMemFree(distances);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00422400
tPhysicsError C2_HOOK_FASTCALL FUN_00422400(tCollision_shape_polyhedron_data* pPolyhedron, int* pVertex_indices, br_model* pModel, int pCount_vertices) {
    int i;
    int result;

    if (pModel->nvertices > pCount_vertices) {
        result = FUN_00422030(pModel, pCount_vertices, pVertex_indices);
        if (result != ePhysicsError_Ok) {
            return result;
        }
        pPolyhedron->count_points = pCount_vertices;
        if (pCount_vertices > 0) {
            for (i = 0; i < pCount_vertices; i++) {
                pPolyhedron->points[i].v[0] = pModel->vertices[pVertex_indices[i]].p.v[0];
                pPolyhedron->points[i].v[1] = pModel->vertices[pVertex_indices[i]].p.v[1];
                pPolyhedron->points[i].v[2] = pModel->vertices[pVertex_indices[i]].p.v[2];
            }
            return ConvexHull3D(pPolyhedron);
        }
    } else {
        pPolyhedron->count_points = pModel->nvertices;
        for (i = 0; i < pModel->nvertices; i++) {
            pVertex_indices[i] = i;
            pPolyhedron->points[i].v[0] = pModel->vertices[i].p.v[0];
            pPolyhedron->points[i].v[1] = pModel->vertices[i].p.v[1];
            pPolyhedron->points[i].v[2] = pModel->vertices[i].p.v[2];
        }
    }
    return ConvexHull3D(pPolyhedron);
}

// FUNCTION: CARMA2_HW 0x00422500
int C2_HOOK_FASTCALL FUN_00422500(tCollision_shape_polyhedron_data* pPolyhedron, br_model* pModel, int pMin_point_count, int pMax_point_count, int* pVertex_indices) {
    int result;
    int count_points;
    int point_count;

    if (pModel == NULL) {
        return 5;
    }
    if (pMax_point_count < 3) {
        return 6;
    }
    if (pMax_point_count > (int)pModel->nvertices) {
        pMax_point_count = pModel->nvertices;
    }
    if (pMin_point_count < 3) {
        pMin_point_count = 3;
    } else if (pMin_point_count > pMax_point_count) {
        pMin_point_count = pMax_point_count;
    }
    point_count = pMax_point_count;
    result = FUN_00422400(pPolyhedron, pVertex_indices, pModel, point_count);
    count_points = pPolyhedron->count_points;
    while (count_points != 0
            && ((result == 0 || result == 2)
                && (point_count = point_count * pMin_point_count / count_points, count_points != pMin_point_count))
            && point_count < pMax_point_count) {
        result = FUN_00422400(pPolyhedron, pVertex_indices, pModel, point_count);
        count_points = pPolyhedron->count_points;
    }
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b73a0
void C2_HOOK_FASTCALL FUN_004b73a0(tPhysics_object* pObject, const br_matrix34* pParent_matrix, const br_matrix34* pRoot_matrix) {
    br_matrix34 inverse;
    br_matrix34 normalised;
    br_vector3 transformed;
    tPhysics_joint* joint;
    int i;

    for (; pObject != NULL; pObject = pObject->next) {
        if ((pObject->flags & 0x400) != 0) {
            BrMatrix34LPInverse(&inverse, &pObject->parent->transform_matrix);
            BrMatrix34LPNormalise(&normalised, &inverse);
            BrMatrix34Mul(&inverse, &pObject->transform_matrix, &normalised);
            BrMatrix34Mul(&pObject->field_0x1e8, &inverse, &pObject->parent->field_0x1e8);
        }
        joint = pObject->physics_joint1;
        if (joint != NULL) {
            if ((pObject->flags & 0x400) != 0) {
                FUN_004b73a0(pObject->child, pParent_matrix, pRoot_matrix);
            } else {
                BrMatrix34ApplyP(&transformed, &joint->field_0x14, pParent_matrix);
                DrMatrix34ApplyLPInverse(&joint->field_0x14, &transformed, pRoot_matrix);
                if (joint->type == 1 || joint->type == 4) {
                    BrMatrix34ApplyV(&transformed, &joint->parent_bone_axis, pParent_matrix);
                    BrMatrix34TApplyV(&joint->parent_bone_axis, &transformed, pRoot_matrix);
                }
            }
            for (i = 0; i < joint->count_limits; i++) {
                BrMatrix34ApplyV(&transformed, &joint->limits[i].parent, pParent_matrix);
                BrMatrix34TApplyV(&joint->limits[i].parent, &transformed, pRoot_matrix);
            }
        }
    }
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b7310
void C2_HOOK_FASTCALL FUN_004b7310(tPhysics_object* pObject) {
    tPhysics_object* parent;
    br_matrix34 inverse;
    br_matrix34 normalised;

    parent = pObject->parent;
    pObject->flags |= 0x400;
    while (parent != NULL && (parent->flags & 0x400) != 0) {
        parent = parent->parent;
    }
    BrMatrix34LPInverse(&inverse, &parent->transform_matrix);
    BrMatrix34LPNormalise(&normalised, &inverse);
    BrMatrix34Mul(&pObject->field_0x1e8, &pObject->transform_matrix, &normalised);
    FUN_004b73a0(pObject->child, &pObject->transform_matrix, &parent->transform_matrix);
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 32.00%: FUN_004be350 0x004be350 - helper pulled in by FUN_004b8cf0; the original divides by the double 1.0 global at 0x0058a780 (fdivr QWORD PTR ds:0x58a780). A gDOUBLE_0058a780 global was measured (32.26 with it, 32.26 without — this score drifts 0.2-0.3 between runs) but it moved data layout and cost FUN_004be4b0 1.53% (83.97->82.44), so it was reverted. Residual is the x87 schedule (our body multiplies components by the reciprocal in a different order).
// FUNCTION: CARMA2_HW 0x004be350
void C2_HOOK_FASTCALL FUN_004be350(float* pDestination, const float* pSource) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;

    pDestination[0] = -pSource[5] * pSource[7] + pSource[8] * pSource[4];
    pDestination[1] = -(-pSource[2] * pSource[7] + pSource[1] * pSource[8]);
    fVar1 = -pSource[2] * pSource[4] + pSource[1] * pSource[5];
    pDestination[2] = fVar1;
    fVar5 = 1.0 / (pSource[1] * pDestination[1] + pSource[2] * fVar1 + pSource[0] * pDestination[0]);
    pDestination[0] = pDestination[0] * fVar5;
    pDestination[3] = -((-pSource[5] * pSource[6] + pSource[8] * pSource[3]) * fVar5);
    fVar1 = pSource[4];
    fVar2 = pSource[3];
    fVar3 = pSource[7];
    fVar4 = pSource[6];
    pDestination[1] = pDestination[1] * fVar5;
    pDestination[6] = (-fVar1 * fVar4 + fVar3 * fVar2) * fVar5;
    pDestination[4] = (-pSource[2] * pSource[6] + pSource[8] * pSource[0]) * fVar5;
    fVar1 = pSource[1];
    fVar2 = pSource[7];
    fVar3 = pSource[6];
    fVar4 = pSource[0];
    pDestination[2] = pDestination[2] * fVar5;
    pDestination[7] = -((-fVar1 * fVar3 + fVar2 * fVar4) * fVar5);
    pDestination[5] = -((-pSource[2] * pSource[3] + pSource[5] * pSource[0]) * fVar5);
    pDestination[8] = (-pSource[1] * pSource[3] + pSource[4] * pSource[0]) * fVar5;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 63.74%: FUN_004b8fe0 0x004b8fe0 - x87 schedule differs (original keeps dt in FPU regs, frame 0x28 vs ours 0x1c: extra temp vector)
// FUNCTION: CARMA2_HW 0x004b8fe0
void C2_HOOK_FASTCALL FUN_004b8fe0(tPhysics_object* pObject, float pDelta_time) {
    br_vector3 joint_axis;
    br_vector3 joint_position;
    br_vector3 scaled;
    float axis_offset;
    float axis_speed;

    if (pObject->physics_joint2->type == eJoint_translation) {
        BrMatrix34ApplyV(&joint_axis, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
        axis_offset = pObject->actor->t.t.mat.m[3][0] * joint_axis.v[0]
                    + pObject->actor->t.t.mat.m[3][1] * joint_axis.v[1]
                    + pObject->actor->t.t.mat.m[3][2] * joint_axis.v[2];
        BrMatrix34ApplyP(&joint_position, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        joint_position.v[0] = pObject->field_0x17c.v[0] - joint_position.v[0];
        joint_position.v[1] = pObject->field_0x17c.v[1] - joint_position.v[1];
        joint_position.v[2] = pObject->field_0x17c.v[2] - joint_position.v[2];
        pObject->actor->t.t.mat.m[3][0] += joint_position.v[0];
        pObject->actor->t.t.mat.m[3][1] += joint_position.v[1];
        pObject->actor->t.t.mat.m[3][2] += joint_position.v[2];
        axis_offset -= pObject->actor->t.t.mat.m[3][0] * joint_axis.v[0]
                     + pObject->actor->t.t.mat.m[3][1] * joint_axis.v[1]
                     + pObject->actor->t.t.mat.m[3][2] * joint_axis.v[2];
        joint_position.v[0] = axis_offset * joint_axis.v[0];
        joint_position.v[1] = axis_offset * joint_axis.v[1];
        joint_position.v[2] = axis_offset * joint_axis.v[2];
        pObject->actor->t.t.mat.m[3][0] += joint_position.v[0];
        pObject->actor->t.t.mat.m[3][1] += joint_position.v[1];
        pObject->actor->t.t.mat.m[3][2] += joint_position.v[2];
        axis_speed = pObject->v.v[0] * joint_axis.v[0]
                   + pObject->v.v[1] * joint_axis.v[1]
                   + pObject->v.v[2] * joint_axis.v[2];
        scaled.v[0] = axis_speed * joint_axis.v[0];
        scaled.v[1] = axis_speed * joint_axis.v[1];
        scaled.v[2] = axis_speed * joint_axis.v[2];
        pObject->v.v[0] = scaled.v[0];
        pObject->v.v[1] = scaled.v[1];
        pObject->v.v[2] = scaled.v[2];
        pObject->actor->t.t.mat.m[3][0] += pDelta_time * scaled.v[0];
        pObject->actor->t.t.mat.m[3][1] += pDelta_time * scaled.v[1];
        pObject->actor->t.t.mat.m[3][2] += pDelta_time * scaled.v[2];
    } else {
        BrMatrix34ApplyP(&joint_position, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        joint_position.v[0] = pObject->field_0x17c.v[0] - joint_position.v[0];
        joint_position.v[1] = pObject->field_0x17c.v[1] - joint_position.v[1];
        joint_position.v[2] = pObject->field_0x17c.v[2] - joint_position.v[2];
        pObject->actor->t.t.mat.m[3][0] += joint_position.v[0];
        pObject->actor->t.t.mat.m[3][1] += joint_position.v[1];
        pObject->actor->t.t.mat.m[3][2] += joint_position.v[2];
    }
}

// STUB: CARMA2_HW 0x004b9230
void C2_HOOK_FASTCALL FUN_004b9230(tPhysics_object* pObject, tPhysics_object** ppChanged_object, float pDelta_time) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
