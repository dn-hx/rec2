/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_13_CRUSH1_H
#define GUARD_13_CRUSH1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

// tFind_crush_actor_arg

// tBend_data_local

// tUnbend_model_context_local

// tNet_message_chunk_semi_detach_bit

// tCrush_net_reattach_list_item_local

extern undefined4 gCrush_array_0067a190[10];

// gNet_crush_detach_list
extern undefined4 gDAT_00679440;
extern tCrush_info_buffer gDetached_bit_crush_info_buffer;
extern tPhysics_object* gDetached_bit_collision_infos;
extern tDriver gDetached_bit_driver;
extern tPhysics_joint* gTrack_crush_joints[32];
extern tCrush_info_buffer gSplit_car_crush_info_buffer;
extern tPhysics_object* gSplit_car_collision_infos;
extern tDriver gSplit_car_driver;
extern tPhysics_shape* gGonad_sphere_collision_shape;

extern void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

// ClearCrushLists

// RemoveCarFromCrushLists

extern void C2_HOOK_FASTCALL InitCrushSystems(void);

// ResetCrushSystems

// ReadCrushData

extern intptr_t C2_HOOK_CDECL CreateDefaultCrushDataCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL CreateDefaultCrushData(br_actor* pActor, tCar_spec* pCar_spec);

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

extern void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor* pActor, tCar_crush_buffer* pBuffer);

extern intptr_t C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData);

extern intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData);

// InitModelMasterCrushData

// InitNetworkShapesStuff

extern int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

extern int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

extern tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);

// SoftnessOfNearestPointCB

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* pCallback, void* pContext);

// SoftnessOfNearestPoint

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

// SetUpSemiDetachJointStuff

// InitPhysCrushDataCB

// InitPhysCrushData

// CalculateReferencePoints

extern intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* pActor, void* pData);

// InitModelCrushData

// PointEdgeDistSq

// SetFlapCheckVertices

// InitModelVertexData

// InitVertexDataCB

// InitVertexData

// CheckWheelPositions

// InitPhysModCrushData

extern void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data);

extern void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data);

// BuckleTheBugger

// OutsideEdgeLine

// PointTriangleDistSq

// PointRectDistSq

// PointSimplexDistSq

extern float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

// LimitImpactScale

extern float C2_HOOK_FASTCALL GetImpactScale(const br_vector3* pP, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector);

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

extern void C2_HOOK_FASTCALL ClipPointToLimits(br_vector3* pPoint, tU16 pLimit_bits, const tCar_crush_limits* pLimits);

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

extern void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc);

// ExpandCrush

// RemoveDuplicatePoints

extern void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds);

// CrushDrone

// CrushDroneObject

// DoDroneCrushing

extern void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints);

// QueueDroneCrush

// DoCrushing

extern void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue);

// QueueCarCrush

// SetBitForDetachment

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

// BitIsInBentPartOfCar

// SetUpFlapJoint

extern void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);

// CheckFlapAngles

extern void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel);

// ReAttachBit

// CentreModelOnPoint

// SemiDetachBit

extern int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP3, br_bounds3* pBounds, br_actor* pActor);

extern void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, tNet_game_player_info* pPlayer);

// SendDetachBit

extern void C2_HOOK_FASTCALL SendAllSemiDetachedBits(tNet_game_player_info* pPlayer);

// AddNonCar

// DetachBit

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

// MungeDetachLists

extern void C2_HOOK_FASTCALL SendReAttachBit(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);

// DoFlapping

// ToggleFlapsCB2

extern int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data);

// ToggleDoorsActorCallback

// SendToggleFlaps

extern void C2_HOOK_FASTCALL ToggleFlaps(void);

extern int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject);

// FullyDetachBit

// SendFullyDetachBit

// DoFullyDetaching

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

// QuantizeWithinBounds

// SaveVitalBitsOfObject

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);

// SwapCarShapes

// IsAWheelOrPivotActor

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

// IsOnNonCarHalf

// SplitCar

// QuantizePlane

// WithinBounds

// SplitCheck

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

// SwapShapesIfPossible

// DoShapeSwapping

// BendPoint

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

// DoBending

extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

extern void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel);

// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg);

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

extern void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar);

extern intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape);

// FUN_0043a5b0

// UnBendActorCB

// CompletelyUnBendCollisionShapes

// CompletelyUnBendWheels

// CompletelyUnBendOtherThings

extern intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData);

// DoCompletelyUnBentThings

extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec);

// TotallyRepairModel

extern intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL SendReAttachAllBits(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL TotallyRepairCar(void);

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

// UnBendCollisionShape

// UnBendCollisionShapes

// UnBendWheels

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

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

extern void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, tCompressed_vector3* pSrc);

// FindActorByCrushEntryID

// ARApplyFlap

// ARApplySplitWeld

// ARUndoSplitWeld

// ReceivedCrush

// FindBitWithID

// AttachedBitFromID

// ReceivedSemiDetachBit

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

// ReceivedFullyDetachBit

// DetachedBitFromID

// SemiDetachedBitFromID

// ReceivedReAttachBit

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

extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

// CompressPlane

// ExpandPlane

// CopySplitInMessage

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);

// NetAllocateMessageChunk

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

extern void C2_HOOK_FASTCALL FUN_00435960(br_model* pModel, const br_vector3* pOffset, tCar_spec** ppCar, br_actor* pActor);

extern br_scalar gCrushZero;

// gWorld_callbacks

extern void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar);

extern tSmashable_level* C2_HOOK_FASTCALL FUN_004ee290(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, undefined4 pArg9, undefined4 pArg10);

extern intptr_t C2_HOOK_CDECL FUN_004f5a70(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

extern br_scalar gFLOAT_00589a2c;

extern int gCollision_face_mask__raycast;

extern int C2_HOOK_FASTCALL FUN_00424270(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data);

extern tToggled_door gToggled_doors[16];

extern void C2_HOOK_FASTCALL FUN_0043bca0(undefined4* pArg1, void* pArg2, br_model* pModel, const br_vector3* pSrc, const float* pScalars);

// gNet_crush_reattach_bit_list

// gCount_crush_detach_list

// TestForCarInSensiblePlace

// gNet_crush_semi_detach_bit_list

// gNet_crush_full_detach_bit_list

// gCrush_detach_list

// gCar_damage_crush_list

// gINT_0067be84

#endif // GUARD_13_CRUSH1_H
