/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_49_PHYSICS_H
#define GUARD_49_PHYSICS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gINT_00692dc8;

extern tPhysicsError_cbfn* gPhysics_error_cb;
extern void* gPhysics_buffer1_ptr;
extern void* gPhysics_buffer2_ptr;
extern int gImmortal_peds;
extern int gTotal_count_smash_peds;
extern int gBOOL_00744804;
extern float gPed_head_scale;
extern tPedestrian* gPedestrian_array;
extern void* gPhysics_buffer3_ptr;
extern int gPHIL_mechanics_time_sync;
extern int gPHIL_munging_objects;
extern int gPhysics_other_buffer_capacity;
extern tProximity_ray gProximity_rays[20];
extern tPed_giblet_size_spec* gGiblet_sizes;
extern br_model* gPed_spurt_model;
extern int gPed_spurty_lumps_count;
extern int gPed_times[3];
extern br_actor* gPed_limbed_actors[30];
extern tShrapnel_time_info gPed_spurt_min_max_time;
extern void* gPhysics_other_buffer;
extern void* gPhysics_scratch_ptr;
extern int gFace_count;
extern tWorld_callbacks gWorld_callbacks;
extern double gDOUBLE_0058a798;

// count

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix);

extern int C2_HOOK_FASTCALL AddObjectWallForce(float* pWalls, const br_vector3* pPoint, float pDistance, const br_vector3* pNormal, tU32 pUnused, int pCount, tPhysics_object* pObject, float pT, float pRadius, const float* pVector);

extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

extern int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, int* pCtx, br_matrix34* pMatrix);

// GetShapePoint

// GetObjectObjectEdgeIntersections

extern int C2_HOOK_FASTCALL GetFaceEdgeIntersection(tPhysics_object* pObject, tPhysics_shape* pShape, tSensible_place_contact* pContacts, int pCount, int pMax, br_vector3* pPoints, br_matrix34* pScratch, int* pHas_collision);

// GetSphereToFacePoints

// CheckSingleEdgeWithSphere

extern int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, int pEdx, int pParam1, int pParam2, int pParam3, int pParam4, int pParam5, int pParam6);

extern int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, int pEdx, tU32 pParam1, int pParam2, int pParam3, char* pParam4, tU32 pParam5);

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

extern int C2_HOOK_FASTCALL CheckSingleEdgeWithObjectEdges(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data);

extern int C2_HOOK_FASTCALL GetEdgeToEdgePoints(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7);

// SetUpWorkspace

extern int C2_HOOK_FASTCALL GetObjectWallForceInfo(tPhysics_object* pObject, tSensible_place_contact* pContacts, int pArg);

// GetObjectCornerToObjectFacePoints

extern int C2_HOOK_FASTCALL BoundsOverlapTest(br_bounds3* pBounds_1, br_bounds3* pBounds_2);

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);

extern int C2_HOOK_FASTCALL AddOldForces(tPhysics_joint_force* pForces, int pCount, const tPhysics_joint_force* pOld_forces, int pOld_count);

extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, tPhysics_shape* pCollision_shape_1, tPhysics_shape* pCollision_shape_2);

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

extern int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);

// HierarchyCalcEnergy

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

extern void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

extern void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

extern int C2_HOOK_FASTCALL GetJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count);

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

extern void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject);

// ListLimitOmega

// GroupLimitOmega

extern void C2_HOOK_FASTCALL PositionWorldJointedObject(tPhysics_object* pObject, br_scalar pDelta_time);

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

extern void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time);

extern void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pCollision_info);

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

extern void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, int pFlag);

// SetGroupDoingNothingFlag

extern void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, int pFlag);

// CheckDoingNothingFlag

extern void C2_HOOK_FASTCALL SetGroup(tPhysics_object* pObject, int pGroup);

// GroupTogether

// GetPreCalculatedJointForces

extern void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

extern void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

extern void C2_HOOK_FASTCALL SetObjectTrialPositionType(tPhysics_object* pObject, int pType);

// FillInTrialWorkspace

extern int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

extern int C2_HOOK_FASTCALL ReduceEffectOfForces(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount, const int* pIndices, int pIndex_count, br_scalar pThreshold);

// DoCollisions

// AddDoubleTorqueToMatrix

// DRMatrix33Inverse

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

extern void C2_HOOK_FASTCALL SetUpForce(br_vector3* pAngular, br_vector3* pLinear, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL SetUpTorqueHingePartners(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, const br_vector3* pDirection, tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL SetUpHingePartners(br_scalar* pOut1, br_scalar* pOut2, br_scalar* pOut3, br_scalar* pOut4, tPhysics_object* pObject);

// MakeMatrix

extern void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject);

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

extern void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, br_scalar pTime);

// RotateObject

extern void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL GetFacesNearObject(tPhysics_object *pObject, tWorld_callbacks *pCallbacks);

extern void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects);

// RepositionObjectGroup

extern void C2_HOOK_FASTCALL SimpleRotate(tPhysics_object* pObject, br_scalar pDt);

// InterpolateSingleObject

// InterpolateObjects

extern void C2_HOOK_FASTCALL ApplyGravitySingleObject(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ApplyGravity(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetGravityDeltaV(tPhysics_object* pObject, br_vector3* pImpulse);

// TrialPositionChildren

extern void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot);

extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL ApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque);

extern void C2_HOOK_FASTCALL ApplyDiminishingTorque(tPhysics_object* pObject, const br_vector3 *pTorque);

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

extern int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pDim, br_scalar* pQ);

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

extern int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim);

extern void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim);

extern void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void);

extern tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType);

extern tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType);

extern tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType);

extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType);

// GetJointSize

extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);

extern tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);

extern void C2_HOOK_FASTCALL FreeShape(void* pData);

extern void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape);

extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);

extern void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);

extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

extern void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage);

extern void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn);

extern void C2_HOOK_FASTCALL RepositionAndOrientateChildren(tPhysics_object* pObject, br_matrix34* pParent_matrix);

extern void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PhysicsAddObjectList(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PhysicsRemoveObject(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize);

extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);

extern float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace);

extern void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage);

// ResetMan2

extern void C2_HOOK_FASTCALL SetUpTestObjects(void);

// ResetObjectList

// ResetMan

extern void C2_HOOK_FASTCALL InitPhysics(void);

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

extern int gActorBoxPick_StopGroovidelics;

extern void C2_HOOK_FASTCALL FUN_0045e4e0(float* pOffset, tCollision_shape_common* pShape, float* pParam1, float* pParam2, float* pParam3, float* pParam4, float* pParam5);

extern void C2_HOOK_FASTCALL FUN_0045e200(float* pOffset, tCollision_shape_polyhedron_data* pPolyhedron, float* pParam1, float* pParam2, float* pParam3, float* pParam4, float* pParam5);

extern void C2_HOOK_FASTCALL MakeMatrix(br_scalar* pMatrix, int pStride, tPhysics_joint_force** pRecords, int pCount);

extern void C2_HOOK_FASTCALL ApplySingleForce(tPhysics_joint_force* pForce, br_scalar pImpulse);

extern void C2_HOOK_FASTCALL ResetObjectList_1(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL GetNonSphereNonSphereForceInfo(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining);

extern int C2_HOOK_FASTCALL GetPointSample(br_model* pModel, int pCount_vertices, int* pVertex_indices);



extern void C2_HOOK_FASTCALL RotateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds3* pBox);

extern tPhysics_object** C2_HOOK_FASTCALL GetReducedList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pObjects);

extern int C2_HOOK_FASTCALL ReducedFindObjectCollisions(tPhysics_object* pObject, tPhysics_object** pOverlapping, tPhysics_joint_force* pContacts, int pCount, tWorld_callbacks* pWorld_callbacks, tPhysics_object* pList);

extern int C2_HOOK_FASTCALL GetJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL MakeVelocitiesVector2(br_scalar* pRhs, tPhysics_joint_force** pRecords, int pCount, int pCollision_count);

extern void C2_HOOK_FASTCALL FUN_004bc190(tPhysics_joint_force** pRecords, br_scalar* pImpulses, int pCount, tWorld_callbacks* pWorld_callbacks);

extern int C2_HOOK_FASTCALL ClampObjectSpin(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL ClampObjectSpinTree(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL FUN_00427fd0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining, const tU8* pFlags, int* pFound, br_vector3* pDirection);

extern int C2_HOOK_FASTCALL FUN_004287a0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, tPhysics_joint_force* pRecords, int pRecord_count, int pRemaining, const tU8* pFlags1, const tU8* pFlags2);

extern void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList);

#endif // GUARD_49_PHYSICS_H

