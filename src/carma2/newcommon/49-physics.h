#ifndef GUARD_49_PHYSICS_H
#define GUARD_49_PHYSICS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tPhysicsError_cbfn* gPhysics_error_cb;
extern void* gPhysics_buffer1_ptr;
extern void* gPhysics_buffer2_ptr;
extern int gImmortal_peds;
extern void* gPhysics_buffer3_ptr;
extern int gPhysics_other_buffer_capacity;
extern void* gPhysics_other_buffer;
extern void* gPhysics_scratch_ptr;
extern tFace_ref* gPling_face;
extern int gActorBoxPick_StopGroovidelics;

// count

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix);

extern int C2_HOOK_FASTCALL AddObjectWallForce(float* pWalls, const br_vector3* pPoint, float pDistance, const br_vector3* pNormal, tU32 pUnused, int pCount, tPhysics_object* pObject, float pT, float pRadius, const float* pVector);
extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);
// RayCastThruFaceList2

extern int C2_HOOK_FASTCALL RayCastInBox(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, tPhysics_object* pObject);
// FaceOffCount2

// FaceOffCount

// FaceOnCount

extern int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, int* pCtx, br_matrix34* pMatrix);
extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

extern int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdgeStart, br_vector3* pEdgeEnd, tPhysics_object* pObject, float* pSphere, float* pWalls, int pCount, br_vector3* pStart, br_vector3* pEnd, tFace_ref* pFace);
extern int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, int pEdx, int pParam1, int pParam2, int pParam3, int pParam4, int pParam5, int pParam6);
extern int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, int pEdx, tU32 pParam1, int pParam2, int pParam3, char* pParam4, tU32 pParam5);
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

extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b);

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

extern void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

extern void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

extern void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject);

// ListLimitOmega

// GroupLimitOmega

// PositionWorldJointedObject

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

extern void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time);

// TellPhysicsJointsHaveChanged

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

extern void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject);
// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

extern void C2_HOOK_FASTCALL GetFacesInBox(tPhysics_object* pCollision, tWorld_callbacks* pWorld_callbacks);

extern void C2_HOOK_FASTCALL PhysicsObjectMoveVelocity(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL PhysicsObjectMoveVelocityList(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL PhysicsObjectSetImpulse(tPhysics_object* pObject, br_vector3* pImpulse);

extern void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject);

// SetListDoingNothingFlag

// SetGroupDoingNothingFlag

// SetHierarchyDoingNothingFlag

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

extern void C2_HOOK_FASTCALL SetCollisionInfoDoNothingChain(tPhysics_object* pCollision_info, tU32 pDisable);

extern void C2_HOOK_FASTCALL SetCollisionInfoDoNothing(tPhysics_object *pCollision_info, tU32 pDisable);

extern void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object *pCollision_info, tU32 pDisable);

extern void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

extern void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);
// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// SetObjectTrialPositionType

// FillInTrialWorkspace

extern int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2);

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// ReduceEffectOfForces

// DoCollisions

extern int C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL DRMatrix33Inverse(tPhysics_object* pObject, int pFlag);

extern int C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object** pObject_list, tPhysics_object* pObject, tPhysics_joint_force* pContacts, tPhysics_joint_force* pJoint_data, int pJoint_count, tWorld_callbacks* pWorld_callbacks);

// InitQuickHinge

// CalcQuickHingeData

// SetUpForce

// SetUpTorqueHingePartners

// SetUpHingePartners

// MakeMatrix

extern void C2_HOOK_FASTCALL SetFieldF0Tree(tPhysics_object* pObject, int pValue);

extern void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject);

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

// NewAddFrictionObjectObject

extern void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* pContact, tWorld_callbacks* pWorld_callbacks);
// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

extern void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime);

extern void C2_HOOK_FASTCALL RotateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject);
// GetFacesNearObject

extern void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects);
// RepositionObjectGroup

// SimpleRotate

extern void C2_HOOK_FASTCALL InterpolateSingleObject(tPhysics_object* pObject, br_scalar pDt);

extern void C2_HOOK_FASTCALL InterpolateObjects(tPhysics_object* pObject, float pDt);

// ApplyGravitySingleObject

// ApplyGravity

// SetGravityDeltaV

// TrialPositionChildren

extern void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot);

extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

extern int C2_HOOK_FASTCALL RQDcmpV(br_scalar* matrix, int size, br_scalar* orthogonal);

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

// GetPhysicsScratchPointer

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

extern void C2_HOOK_FASTCALL FreeThingForm(void* pData);

extern void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape);

extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);

extern void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds);

// FillInShape

// UpdateCollisionObject

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

extern void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage);

extern void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn);

// RepositionAndOrientateChildren

extern void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild);

// PhysicsAddObjectList

// PhysicsRemoveObject

extern void C2_HOOK_FASTCALL AddCollisionInfoChild(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize);

extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);

extern float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace);

extern void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage);

extern void C2_HOOK_FASTCALL ResetMan2(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetUpTestObjects(void);

// ResetObjectList

// ResetMan

extern void C2_HOOK_FASTCALL InitPhysics(void);

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID


extern void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation);
extern br_scalar C2_HOOK_FASTCALL PhysicsObjectWeightedSquareSum(tPhysics_object* pObject);
extern br_scalar C2_HOOK_FASTCALL FUN_004b7290(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL FUN_004b9650(tPhysics_object* pObject, tPhysics_object* pParent_context);
extern void* C2_HOOK_FASTCALL FUN_004c5c70(int pSize);
extern int C2_HOOK_FASTCALL FUN_004b8350(tPhysics_object* pObject, tPhysics_joint_force* pForce);
extern void C2_HOOK_FASTCALL FUN_004bf490(br_scalar* pMatrix, int pStride, tPhysics_joint_force** pRecords, int pCount);
extern void C2_HOOK_FASTCALL FUN_004b77b0(br_scalar* pVelocity, tPhysics_joint_force** pRecords, int pCount);
extern void C2_HOOK_FASTCALL FUN_004b7960(tPhysics_joint_force* pForce, br_scalar pImpulse);
extern void C2_HOOK_FASTCALL FUN_004b8ee0(tPhysics_object* pObject, float pDelta_time);
extern void C2_HOOK_FASTCALL FUN_004b8cf0(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004be4b0(br_scalar* pOut1, br_scalar* pOut2, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject);
extern tPhysics_object** C2_HOOK_FASTCALL AppendOverlappingObjects(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pObjects);
extern int C2_HOOK_FASTCALL FUN_004bd9e0(tPhysics_object* pObject, tPhysics_object** pOverlapping, tPhysics_joint_force* pContacts, int pCount, tWorld_callbacks* pWorld_callbacks, tPhysics_object* pList);
extern int C2_HOOK_FASTCALL FUN_00425ca0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_joint_force* pContacts, int pCount);
extern void C2_HOOK_FASTCALL FUN_004bddd0(tPhysics_object* pObject1, tPhysics_object* pObject2);
extern void C2_HOOK_FASTCALL FUN_004be0e0(tPhysics_object* pObject, tPhysics_object* pList);
extern void C2_HOOK_FASTCALL MergeBoundsIntoParent(tPhysics_object* pObject, br_bounds3* pBox);
extern int C2_HOOK_FASTCALL CollectJointLimitsIntoBuffer(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count);
extern int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, tPhysics_joint_force* pContacts, int pCount, int pMax_count);
extern int C2_HOOK_FASTCALL CopyPhysicsJointData(tPhysics_joint_force* pDestination, int pDestination_index, const tPhysics_joint_force* pSource, int pCount);
extern int C2_HOOK_FASTCALL ClampObjectSpinTree(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004bd4f0(br_matrix34* pMatrix1, br_matrix34* pMatrix2, br_vector3* pChild1, br_vector3* pParent1, br_vector3* pChild2, br_vector3* pParent2, br_vector3* pOut1, br_vector3* pOut2);
extern tCollision_shape_polyhedron_data* C2_HOOK_FASTCALL FUN_00421fd0(int pCount_points);
extern int C2_HOOK_FASTCALL FUN_00422030(br_model* pModel, int pCount_vertices, int* pVertex_indices);
extern tPhysicsError C2_HOOK_FASTCALL FUN_00422400(tCollision_shape_polyhedron_data* pPolyhedron, int* pVertex_indices, br_model* pModel, int pCount_vertices);
extern int C2_HOOK_FASTCALL FUN_00422500(tCollision_shape_polyhedron_data* pPolyhedron, br_model* pModel, int pMin_point_count, int pMax_point_count, int* pVertex_indices);
extern void C2_HOOK_FASTCALL FUN_004b73a0(tPhysics_object* pObject, const br_matrix34* pParent_matrix, const br_matrix34* pRoot_matrix);
extern void C2_HOOK_FASTCALL FUN_004b7310(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004be350(float* pDestination, const float* pSource);
extern void C2_HOOK_FASTCALL FUN_004b8fe0(tPhysics_object* pObject, float pDelta_time);

#endif // GUARD_49_PHYSICS_H
