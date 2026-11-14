#ifndef GUARD_13_CRUSH1_H
#define GUARD_13_CRUSH1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

typedef struct {
    undefined field_0x0[0x2c];
    br_vector3 normal;
    undefined field_0x38[0x28];
} tSensible_place_contact;

// Inferred layout of the bend data stored at tCar_crush_spec::field_0x154
// (the record is not modelled in rec2_types.h yet).
typedef struct tBend_data_local {
    br_scalar field_0x0;
    br_scalar field_0x4;
    undefined field_0x8[4];
    br_scalar field_0xc;
    br_scalar field_0x10;
    undefined field_0x14[4];
    br_vector3 field_0x18;
    br_vector3 field_0x24;
    br_vector3 field_0x30;
    br_angle field_0x3c;
    br_angle field_0x3e;
    br_angle field_0x40;
    undefined field_0x42[2];
    br_vector3 field_0x44;
    br_vector3 field_0x50;
    br_vector3 field_0x5c;
    br_vector3 field_0x68;
    int field_0x74;
    tCar_crush_vertex_data* field_0x78;
} tBend_data_local;

typedef struct tBend_point_context_local {
    tCar_spec* car;
    br_matrix34 matrix;
    tBend_data_local* bend;
} tBend_point_context_local;

extern undefined4 gCrush_array_0067a190[10];
extern undefined4 gDAT_00679440;
extern tCrush_info_buffer gDetached_bit_crush_info_buffer;
extern tPhysics_object* gDetached_bit_collision_infos;
extern tDriver gDetached_bit_driver;
extern tPhysics_joint* gTrack_crush_joints[32];
extern tCrush_info_buffer gSplit_car_crush_info_buffer;
extern tPhysics_object* gSplit_car_collision_infos;
extern tDriver gSplit_car_driver;
extern tPhysics_shape* gGonad_sphere_collision_shape;
extern br_scalar gCrushZero;
extern int gCount_crush_detach_list;
extern tCrush_detach_list_item gCrush_detach_list[16];
extern tU32 gINT_0067be84;

// Constants used by SetUpSemiDetachedBitJoint (SetUpSemiDetachedBitJoint / similar)
extern br_scalar gFLT_EPSILON_00589a5c;        // 0x589a5c = 2.3841858e-07
extern double gDouble_00589a60;                // 0x589a60 = 1.0 (double)
/* CrushBendFlapRend scales, used by 13-crush1.c only. Declared here adds 3 symbols
 * to every including TU, which shifts MSVC 5 register allocation in unrelated
 * functions (PedApplyDismember 0x4ca900, 22-replay.c); that TU opts out. */
#ifndef REC2_NO_CRUSH_SCALE_GLOBALS
extern double gDouble_00589b18;                // 0x589b18 = 99.0 (double)
extern double gDouble_00589b20;                // 0x589b20 = -0.05 (double)
extern br_scalar gFloat_00589b28;              // 0x589b28 = 5.0f
#endif
extern br_scalar gFLOAT_1_0_00589a54;          // 0x589a54 = 1.0f
extern br_scalar gFLOAT_NEG1_0_00589b2c;       // 0x589b2c = -1.0f
extern br_scalar gFLOAT_0_01_00589a58;         // 0x589a58 = 0.01f
extern br_scalar gFLOAT_0_015_00589b30;        // 0x589b30 = 0.015f

extern void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

// ClearCrushLists

extern void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL InitCrushSystems(void);

// ResetCrushSystems

// ReadCrushData

extern intptr_t C2_HOOK_CDECL CreateDefaultCrushDataCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec);

// ReadMasterCrushData

// LoadCrushDataFile

extern intptr_t C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor* pActor, tCar_crush_buffer* pBuffer);

extern intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData);

// InitModelMasterCrushData

#ifndef REC2_NO_CRUSH_DECLS
extern void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush);
#endif

extern int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

extern int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

extern tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);
extern intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* pCallback, void* pContext);

// SoftnessOfNearestPoint

extern void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec);
// CopyShapePolyhedron

// InitShapeStuff

extern void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec);
// CheckHingePointOrder

extern tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel);
// DistanceToBounds

// FindNearestParentVertex

extern void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds);
extern void C2_HOOK_FASTCALL SetUpDetachedBitObject(tPhysics_object* pObject, br_actor* pActor, br_model* pModel, tCar_crush_detach_data* pDetach_data, tCar_crush_spec* pCar_crush, const br_bounds3* pBounds, tPhysics_object* pParent_collision_info);

extern intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data);
// InitPhysCrushData

// CalculateReferencePoints

extern intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data);
// InitModelCrushData

extern float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB);
extern void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data);
// InitModelVertexData

extern intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data);
// InitVertexData

// CheckWheelPositions

extern void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL FreeCrushData(tCar_crush_buffer_entry* pCrush_data);

extern void C2_HOOK_FASTCALL FreeGroovidelic(tModel_detail_vertex_data* pVertex_data);

// BuckleTheBugger

extern int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside);
extern float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);
extern float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);

// PointSimplexDistSq

extern float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

// LimitImpactScale

// GetImpactScale

extern int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern void C2_HOOK_FASTCALL LimitImpactScale(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, tCar_crush_limits* pLimits, int pFound);

extern float C2_HOOK_FASTCALL GetImpactScale(br_vector3* pVertexPos, int pCount, br_vector3* pVectors, br_scalar pMaxDistSq);

extern int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector);

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

// ClipPointToLimits

extern void C2_HOOK_FASTCALL CrushCollisionShapes(tCar_spec* pCar, void* pContext);

// SimpleCrushCarModelsCB

// SimpleCrushCar

extern void C2_HOOK_FASTCALL CrushCar(tCar_spec* pCar, br_actor* pActor, tCar_crush* pCrush);

// SendCrush

extern void C2_HOOK_FASTCALL ClipPointToLimits(br_vector3* pVertexPos, tU16 pFlags, tCar_crush_limits* pLimits);

extern void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc);

extern void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc);
// RemoveDuplicatePoints

extern void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds);
// CrushDrone

extern void C2_HOOK_FASTCALL CrushDroneObject(tPhysics_object* pObject, int pCount, float* pPoints, br_vector3* pScale);
// DoDroneCrushing

extern void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints);

extern void C2_HOOK_FASTCALL QueueDroneCrush(tPhysics_object* pObject, int pChain_count, br_vector3* pChain, br_vector3* pForce);
// DoCrushing

#ifndef REC2_NO_CRUSH_DECLS
extern void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue);
#endif

extern void C2_HOOK_FASTCALL QueueCarCrush(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick);

extern void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);

// SetFlappage

// GetUniqueID

// InitializeCollisionInfo

// RemoveBit

// RemoveNonCarChildren

// RemoveNonCar

// GetNonCar

// GetNonCarObject

// SetRelationsDoingNothingFlags

// SetUpDetachedBitObject

// SetUpSemiDetachedBitJoint

// FUNCTION: CARMA2_HW 0x00437480
extern void C2_HOOK_FASTCALL SetUpSemiDetachedBitJoint(tPhysics_joint* pJoint, tCar_crush_detach_data* pDetach_data, int pVertex_index1, int pVertex_index2, tPhysics_object** pPhysics_objects, const br_vector3* pOptional_pos);

// BitIsInBentPartOfCar

extern void C2_HOOK_FASTCALL SetUpFlapJoint(tPhysics_joint* pJoint, tCar_crush_flap_data* pFlap_data, br_model* pModel, tPhysics_object* pObject, br_vector3* pPoint1, br_vector3* pPoint2, br_vector3* pPoint3);
extern void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);

// CheckFlapAngles

extern void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel);

extern void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject);
// CentreModelOnPoint

extern tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor);

extern void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4);
// SendDetachBit

extern void C2_HOOK_FASTCALL SendAllSemiDetachedBits(undefined4* pRecipient);

// AddNonCar

extern tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);
extern void C2_HOOK_FASTCALL DoDetaching(void);

extern intptr_t C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern int C2_HOOK_FASTCALL CheckSingleEdgeWithObjectEdges(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data);
// FlapBits

// StartFlapping

extern void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);
// SendReAttachBit

extern void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL DoFlapping(void);
// ToggleFlapsCB2

extern int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data);

// SendToggleFlaps

extern void C2_HOOK_FASTCALL ToggleFlaps(void);
extern int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL DoFullyDetaching(void);
// CalculateCollisionObjectStuffForSplitHalves

// SetUpCollisionInfoStuffForSplitHalf

// SetFace

// AddSplitVertexIndex

// CreateSplitVertex

// GetSplitVertex

// SplitModel

// CreateSplitPoints

// SplitCollisionShape

// SplitCarCollisionShapes

extern int C2_HOOK_FASTCALL QuantizeWithinBounds(br_scalar* pValue, br_scalar pQuant, br_scalar pMin, br_scalar pMax);
// SaveVitalBitsOfObject

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);

// SwapCarShapes

// IsAWheelOrPivotActor

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

// IsOnNonCarHalf

extern void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag);

// QuantizePlane

// WithinBounds

extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane);

// FindForceEdge

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

// PointIsOutsideShape

// PointIsOutsideLimits

// ClipPointToPolyhedron

// QuantizeVector3

// MoveCarToSensiblePlace

extern int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec* pCar_spec);
// DoShapeSwapping

extern void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context_local* pContext);
// EdgeCrossesZPlane

// BendCollisionShape

// BendCollisionShapes

// BendModel

// BendModels

// BrAngleToSignedDegree

// BendWheels

// BendActor

// BendOtherThings

// ReAttachOrFullyDetachDifficultBits

extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7);
// QuantizeAngle

// BendCarOneForce

extern void C2_HOOK_FASTCALL DoBending(void);
extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

extern void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel);

// RandomlyXZSplitCar

// RandomlyBendCar

extern void C2_HOOK_FASTCALL TotallySpamTheCar(tCar_spec* pCar_spec, float pDamage);

// DRActorEnumRecurseUltraPlus

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

extern void C2_HOOK_FASTCALL CrushBendFlapRend(void);
// SphericizeModel

// SphericizePhysics

extern void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar);

extern intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape);

// CompletelyUnBendCollisionShapes

extern void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec);
// CompletelyUnBendOtherThings

extern intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData);

// DoCompletelyUnBentThings

extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec *pCar_spec);
// TotallyRepairModel

extern intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);

// SendReAttachAllBits

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL TotallyRepairCar(void);

// AddTotalDeflection

// AddTotalDeflection

// RepairModel

// RepairModels_1

// UnBendModel

// UnBendModels

extern void C2_HOOK_FASTCALL UnBendActor(br_actor* pActor, tBend_data_local* pBend, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL UnBendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context_local* pContext);
// UnBendCollisionShapes

extern void C2_HOOK_FASTCALL UnBendWheels(tCar_spec* pCar_spec, const tBend_point_context_local* pContext);
// UnBendOtherThings

extern br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle);

// CalculateAmountRepairedFraction

extern br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount);

extern void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec);

// ReAttachBits

// RepairObject

// RepairCarCollisionShapes

// RepairCar2

// RepairCar

// RestoreVitalBitsOfObject

extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

extern void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc);

// ARUndoModelMash

// ARApplyFlap

// ARApplySplitWeld

// ARUndoSplitWeld

extern void C2_HOOK_FASTCALL ReceivedCrush(tU8* pContents);
// FindBitWithID

// AttachedBitFromID

extern void C2_HOOK_FASTCALL ReceivedSemiDetachBit(tU8* pContents);

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

extern void C2_HOOK_FASTCALL ReceivedFullyDetachBit(tU8* pContents);

// DetachedBitFromID

// SemiDetachedBitFromID

extern void C2_HOOK_FASTCALL ReceivedReAttachBit(tU8* pContents);

// ReceivedReAttachAllBits

// DoQueuedNetDetaching

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

// ReceivedToggleFlaps

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

// GetNetShapeDataLengthFromShape

// GetNetShapeDataLengthFromData

#ifndef REC2_NO_CRUSH_DECLS
extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);
#endif

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

extern void C2_HOOK_FASTCALL CompressPlane(tU16* pDest, const br_vector4* pSrc);
extern void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16* pSrc);
// CopySplitInMessage

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);


extern int C2_HOOK_CDECL ToggleFlapsCB2(br_actor* pActor, void* data);
extern void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data);
extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec);
extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3);
extern int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);
extern void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);
extern void C2_HOOK_FASTCALL FUN_0043c7a0(br_vector3* pVertex, int pCount, float* pVectors, br_vector3* pDirection, br_scalar pForce, tU32 pFlags, const tCar_crush_limits* pLimits, int pFound);
extern intptr_t C2_HOOK_FASTCALL CrushModels(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern intptr_t C2_HOOK_FASTCALL SimpleCrushCarModelsCB(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern void C2_HOOK_FASTCALL PipeSingleCrush(br_model* pModel, br_vector3* pSrc, const float* pScalars);
extern void C2_HOOK_FASTCALL AddCrushToPipingSession(br_model* pModel, br_vector3* pSrc, const float* pScalars);
extern void C2_HOOK_FASTCALL ARCompressCrush(undefined4* pArg1, void* pArg2, br_model* pModel, const br_vector3* pSrc, const float* pScalars);
extern intptr_t C2_HOOK_FASTCALL UnBendModels(br_actor* pActor, br_vector3* pDelta, void* pData);
extern void C2_HOOK_FASTCALL UnBendModel(br_model* pModel, br_vector3* pTranslation, void* pContext);
extern void C2_HOOK_FASTCALL PipeSingleRelink(uintptr_t pId, br_actor* pActor);
extern intptr_t C2_HOOK_CDECL CheckFlapAngles(br_actor* pActor, int pValue);
extern void C2_HOOK_FASTCALL AddCarDamageToPipingSession(int pOwner, void* pData, tU32 pArg3);
extern int C2_HOOK_FASTCALL GetObjectWallForceInfo(tPhysics_object* pObject, tSensible_place_contact* pContacts, int pArg);
extern int C2_HOOK_FASTCALL TestPairOverlap(tPhysics_object* pObject, tPhysics_object* pOther);
extern intptr_t C2_HOOK_FASTCALL BendModels(br_actor* pActor, br_vector3* pDelta, void* pContext);
extern void C2_HOOK_FASTCALL BendActor(tCar_spec* pCar, br_actor* pActor, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg6, float pArg7, tBend_data_local* pBend);
extern void C2_HOOK_FASTCALL BendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context_local* pContext);
extern void C2_HOOK_FASTCALL BendWheels(tCar_spec* pCar_spec, const tBend_point_context_local* pContext);
extern void C2_HOOK_FASTCALL BendModel(br_model* pModel, const br_vector3* pOffset, void* pContext, br_actor* pActor);
extern void C2_HOOK_FASTCALL SmashEverything(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL LoseDodgyPowerups(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL ChristmasLights(tCar_spec* pCar_spec, int pMin_damage, int pMax_damage);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tU8 pType, int pSize_decider);
extern void C2_HOOK_FASTCALL QueueSmashIfNecessary2(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, void* pArg4, br_actor* pActor, int pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, int pArg9);
extern tSmashable_level* C2_HOOK_FASTCALL QueueSmashIfNecessary3(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, undefined4 pArg9, undefined4 pArg10);
extern void C2_HOOK_FASTCALL SplitModel(br_model* pSource_model, br_model** pModels, br_model** pModel_out, br_scalar* pPlane, int pFlag);
extern tCrush_info* C2_HOOK_FASTCALL GetNonCar(tCrush_info_buffer* pInfo_buffer, br_actor* pActor, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL CreateSplitPoints(br_vector3* pPositive_intersection, br_vector3* pNegative_intersection, const br_vector3* pPositive_point, const br_vector3* pNegative_point, float pPositive_distance, float pNegative_distance);
extern void C2_HOOK_FASTCALL SplitCollisionShape(tCollision_shape_polyhedron* pSource, tCollision_shape_polyhedron* pPositive_output, tPhysics_shape* pNegative_output, br_scalar* pPlane);
extern int C2_HOOK_FASTCALL GetFaceEdgeIntersection(tPhysics_object* pObject, tPhysics_shape* pShape, tSensible_place_contact* pContacts, int pCount, int pMax, br_vector3* pPoints, br_matrix34* pScratch, int* pHas_collision);
extern int C2_HOOK_FASTCALL GetEdgeToEdgePoints(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7);
extern void C2_HOOK_FASTCALL GetSplitVertex(tU16 pVertex_a, tU16 pVertex_b, tU16* pOutput_a, tU16* pOutput_b, float* pDistances);
extern void C2_HOOK_FASTCALL LazyMoveModel(br_model* pModel, const br_vector3* pOffset);
extern void C2_HOOK_FASTCALL CrushModel(br_model* pModel, int pCount, br_vector3* pPoints, br_vector3* pDirection, float pForce, const tCar_crush_limits* pLimits, int pAxis, int pFound, float* pMax_bend, float* pMax_distance, tU32* pMax_vertex);
extern int C2_HOOK_CDECL FindBitWithID(br_actor* pActor, void* pArg);
extern void C2_HOOK_FASTCALL ReceivedDetachBit(void* pMsg);
extern void C2_HOOK_FASTCALL SetFlappage(br_actor* pActor, tCar_spec* pCar, int pFlag, float pDelay);
extern void C2_HOOK_FASTCALL FUN_00439500(void);
extern void C2_HOOK_FASTCALL BuckleTheBugger(tCar_spec* pCar, br_actor* pActor, br_vector3* pDirection, float pDistance, tU32 pMax_vertex, tU32 pField_0x50);
extern void C2_HOOK_FASTCALL FUN_004c7fa0(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates);

#endif // GUARD_13_CRUSH1_H
