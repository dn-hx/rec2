/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_61_PEDESTRN_H
#define GUARD_61_PEDESTRN_H

#include "c2_hooks.h"
#include "rec2_types.h"

// tPed_anim_seq_2

extern int gPed_count_limbed_actors;
extern br_actor* gLimbs_actor;

extern int gPed_retain_root_mode;

// gPed_recent_ped

// gCount_pedestrian_personality_cloned_materials

// gPed_remaps

// gPed_personalities
extern float gPedestrian_speed_factor;
extern int gSelected_ped;

// gPed_head_scale

// gTotal_count_smash_peds

// gPed_buoyancy_factor
extern int gPeds_suicidal;
extern int gExploding_pedestrians;

// gPedestrians_in_sight
extern tNapalm_bolt gNapalm_bolts[5];

// gAverage_ped_scale

// gPed_recent_points

// gPed_groups
extern int gCount_changed_points;

// gPed_times

// gBurning_ped_models

// gPed_groups_count

// gRace_pedestrian_specs

// gPed_spurt_min_max_time

// gPed_spurt_model

// gPed_spurty_lumps_count
extern br_vector3 gChanged_points[10];
extern int gPed_nearness;
extern tTWTVFS gTwtPeds;

// gPed_cache_006944c0

// gPed_spurt_initial_speed
extern float gMutant_speed;
extern int gDancing_peds;
extern int gDrunk_pedestrians;

// gCount_killed_peds

// gPed_scale
extern int gPeds_already_munged;

// gPipe_halted_ped_status

// gPed_face_caches

// gPed_points

// gPed_score_multiplier

// gBurning_ped_materials

// gPed_movements

// gPed_movements_count

// gCount_race_pedestrian_specs

// gPed_spurt_initial_position
extern int gPed_cache_sizes_1[4];
extern tPed_face_cache_0x34* gPed_face_cache_heads[4];
extern int gPed_cache_notify_flags;

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

extern void C2_HOOK_FASTCALL PedApplyPedDir(tU32 pId, const br_vector3* pDir);

// SetCharacterMoveAR

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

extern void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel);

// PedApplyPedPhysics

// PedApplyPedPos

extern void C2_HOOK_FASTCALL PedApplyVanishDismembered(tPedestrian* pPedestrian, tU32 pValue);

// SetCharacterPositionAR

// SetCharacterBoneModelAR

// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

extern void C2_HOOK_FASTCALL PedUndoDismember(tPedestrian* pPed, int pBone_index, tS8 pValue);

// ReadPedSpecs

extern br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed);

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

// InitNapalmNolts

extern void C2_HOOK_FASTCALL InitPolyPedSystem(void);

// DisposePedSpawnSpec

// DisposeCharacterInstances

extern void C2_HOOK_FASTCALL DisposePedStuff(void);

extern FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);

// BonerReadPersonalityModels

extern void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pBone_index, tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor);

extern int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor);

// SetPedXZDirection

// StopPedNoise

// MakePedNoise

extern void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, tU32 pTime, tPed_action pAction);

extern void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3);

// AcceleratePed

// MungePedHeadAnim

// SetPedHeadAnim

// ClearPedHeadAnim

// ScarePedestrian

// PedAnimCausesMovement

// StopSmoothTurning

// SetKnockedOverMove

// DoPostElectricution

// CBMoveCompleted

extern void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF);

extern void C2_HOOK_FASTCALL CBDisposeForm(tPed_form* pForm);

// CBLoadPersonality

extern void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality);

extern void C2_HOOK_FASTCALL CBFillInObject(tPhysics_object* pObject, undefined4 pType);

// DoGibShower

// PedApplyGibShower

// PedUndoGibShower

// DoGiblets

// DoBloodSpurt

// DoSpurt

// PedApplyBloodSpurt

// PedUndoBloodSpurt

// OneLessPed

// ScoreForKilledPedestrian

// KillPedestrian

// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

// DoToPeds

// DoPedAnnihilator

// RepulseIt

extern int C2_HOOK_FASTCALL DoPedestrianRepulsifictor(tCar_spec* pCar, float pF0, float pF1, float pF2, float pF3);

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

// RescanPedProximity

// RecacheAndSetFace

// PedScanForObjects

// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

extern int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace);

extern int C2_HOOK_FASTCALL CBActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject);

// RecordPedSlide

// PedProcessForces

// CBMovedByPhysics

extern int C2_HOOK_FASTCALL GetPedCount(void);

// GetPedPosition

extern void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

// FinishUpLoadingPeds

extern void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

extern void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed);

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

extern void C2_HOOK_FASTCALL RenderLimbs(void);

// MungePedestrians

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

// ARResetNapalmBolts

extern void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

// InitPedsForRace

// RenderElectroBastardRays

extern void C2_HOOK_FASTCALL AdjustProxRay(int pRay_index, tU16 pCar_ID, tU16 pPed_index, tU32 pTime);

// ResetProxRay

extern tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);

extern void C2_HOOK_FASTCALL NextPedCam(void);

extern void C2_HOOK_FASTCALL PrevPedCam(void);

// FindNearestPed

extern void C2_HOOK_FASTCALL ResetPedNearness(void);

extern void C2_HOOK_FASTCALL GetPedestrianDebugInfo(char* pBuffer, int pPed_index);

// PedPreCollisionStuff

// UpdateIfBackwardsAR

extern int C2_HOOK_FASTCALL ChangePedScaling(br_vector3* pScale);

// ChangeHeadScaling

// MakeMaterialBlendy

extern void C2_HOOK_FASTCALL UnmakeMaterialBlendy(br_material* pMaterial);

// TurnOffCollisions

// TurnOnCollisions

extern void C2_HOOK_FASTCALL BlendifyMaterial25(br_material* pMaterial);

extern void C2_HOOK_FASTCALL ModelUpdatePrimitiveMaterials(br_model* pModel);

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

extern void C2_HOOK_FASTCALL DisableCollisionsForEthereal(tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL MutatePed(tPed_character_instance* pCharacter, void* pUnused, tU8* pAngle_x, tU8* pAngle_y, tU8* pAngle_z);

extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);

extern void C2_HOOK_FASTCALL SavePedStuff(void);

extern void C2_HOOK_FASTCALL ComparePedStuff(void);

extern void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4);

// CalmDownAllPeds

extern void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetZombiePedFolderNames(void);

extern void C2_HOOK_FASTCALL SetBloodPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetAlienPedFolderNames(void);

extern void C2_HOOK_FASTCALL AddChangedPoint(br_vector3* pPoint);

extern int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);

extern int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void);

extern int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);

extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

extern void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

extern void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

extern void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on);

extern void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

// ClearOutMorphs

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

// ScaleModelXYZ

// DRScaleModel

// DisposeMove

// DisposePedMoves

// DisposeAllLoadedMoves

// ReadRemap

extern void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap);

// DisposePedForms

// DisposePedForm

// DisposeAllRemaps

// DisposeAllLoadedRemaps

// FindOrOpenRemap

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

// SetUpCharacterForm

// DisposeCharacterForm

// DisposeAllLoadedForms

extern void C2_HOOK_FASTCALL DisposeCharacterInstance(tPed_character_instance* pCharacter);

// DisposeAllPedPersonalities

// DisposePedPersonality

// FindOrOpenForm

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

// FindOrOpenPersonality

// BuildCharacterInstance

extern br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter);

extern br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index);

extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(const tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, int pArg2, int pArg3);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter);

// MakeCharacterPhysicsSetup

// MakeCharacterPhysics

// CharacterNoLongerRenderable

// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4);

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance* pCharacter, int pArg2, int pArg3);

// GetRootObject

// CharacterNoLongerCollideworthy

extern void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, const br_vector3* pV);

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// MakeCharacterPhysicworthy

// RecalculateOrientationOfRoot

extern int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2);

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

// StopCharacterMorphing

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);

// DRMatrix34RotateCos

// DropPointOntoPlane

extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pChar, br_vector3* pPosition, int pMode);

// DisposeClonedMaterials

// DisposeAllBonerData

extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set);

// SetCharacterBoneModelP

extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pModel_index, int pModel_set);

extern br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index);

extern int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter);

// BonerPassiveCollision

// BonerActiveHalted

// MakeCharacterBonePhysicworthy

// BonerPedMovedByPhysics

// DismemberCharacter

extern float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum);

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

// MungeAllCharacterMaterials

// MungeAllCharacterObjects

// MungeAllCharacterModels

extern void C2_HOOK_FASTCALL DisableOverallMovement(void);

extern void C2_HOOK_FASTCALL EnableOverallMovement(void);

extern void C2_HOOK_FASTCALL EnableRetainRootMode(void);

extern void C2_HOOK_FASTCALL DisableRetainRootMode(void);

// gPed_forms

// ScaleModel

// gPed_moves

// gPed_vtable

// gFloat_00589358

// gPedestrian_character_cloned_materials

// gBurning_ped_map_names

// gPed_last_score_time

// gPed_noise_table

// gEthereal_pedestrians

// gPed_valium_left

// gCalm_ped_anim_sequence

#endif // GUARD_61_PEDESTRN_H
