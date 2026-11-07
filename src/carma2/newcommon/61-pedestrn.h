#ifndef GUARD_61_PEDESTRN_H
#define GUARD_61_PEDESTRN_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern float gPedestrian_speed_factor;
extern int gPeds_suicidal;
extern int gExploding_pedestrians;
extern tNapalm_bolt gNapalm_bolts[5];
extern int gDancing_peds;
extern int gPed_count;
extern int gPed_overall_movement_disabled;
extern int gPed_retain_root_mode;
extern tPedestrian* gPedestrian_array;
extern int gSelected_ped;
extern tTWTVFS gTwtPeds;
extern br_actor* gLimbs_actor;
extern int gPed_count_limbed_actors;
extern br_actor* gPed_limbed_actors[30];
extern br_vector3 gChanged_points[10];
extern const int gPow2_array[32];
extern int gPed_nearness;
extern int gEthereal_pedestrians;
extern tPedestrian* gPedestrians_in_sight;
extern float gConst_replay_rate_zero;
extern const char* gForm_non_humanoid_names[1];
extern int gCount_race_pedestrian_specs;
extern tRace_ped_spec* gRace_pedestrian_specs;

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

// PedApplyPedDir

extern void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime);
// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

// SetCharacterPhysicsLevelAR

// PedApplyPedPhysics

// PedApplyPedPos

// PedApplyVanishDismembered

// SetCharacterPositionAR

extern void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, int pArg4);
// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

// PedUndoDismember

extern void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);

extern br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed);

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

extern void C2_HOOK_FASTCALL InitNapalmNolts(void);
extern void C2_HOOK_FASTCALL InitPolyPedSystem(void);

// DisposePedSpawnSpec

extern void C2_HOOK_FASTCALL DisposePedStuff(void);

extern FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);
extern void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName);
extern void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pIndex, tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor);

extern int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor);

extern void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pArg3, tU32 pTime);
// StopPedNoise

extern void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pAction);
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

extern void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF);

extern void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality);

extern void C2_HOOK_FASTCALL CBFillInObject(undefined4* pArg1, undefined4 pArg2);

extern void C2_HOOK_FASTCALL DoGibShower(volatile int pId, volatile int pTime, int pSeed, float pForce, int pSize_index, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
// PedApplyGibShower

// PedUndoGibShower

extern void C2_HOOK_FASTCALL DoGiblets(tPedestrian* pPed, tPhysics_object* pObject, tPhysics_object* pOpt_speed_object, float pArg4, br_vector3* pArg5, int pArg6);
extern void C2_HOOK_FASTCALL DoBloodSpurt(uintptr_t pArg1, int pArg2, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

extern void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pTransform, br_vector3* pPosition, br_vector3* pDirection, br_vector3* pRandom_direction);

// PedApplyBloodSpurt

// PedUndoBloodSpurt

extern void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed);
extern void C2_HOOK_FASTCALL ScoreForKilledPedestrian(tPedestrian* pPed, float pHeight);
extern void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info);
// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

extern int C2_HOOK_FASTCALL FindPedestrianNearCar(tCar_spec* pCar, br_scalar pRadius);

extern int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);

extern int C2_HOOK_FASTCALL DoPedAnnihilator(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pArg4, tPedestrian_distances* pDistances);

extern void C2_HOOK_FASTCALL RepulseIt(tPedestrian* pPed, tCar_spec* pCar, float pDist_sq, tU32 pNow, void* pData);

extern int C2_HOOK_FASTCALL DoPedestrianRepulsifictor(tCar_spec* pCar, float pF0, float pF1, float pF2, float pF3);

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

extern void C2_HOOK_FASTCALL MungeNapalm(void);
// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

extern float C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, br_vector3* pArg2);

extern tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime);
extern void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime);
// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

extern int C2_HOOK_FASTCALL ActiveHalted(tPedestrian* pPed, undefined4* pArg2, int pArg3);
extern int C2_HOOK_FASTCALL CBActiveHalted(undefined4* pArg1, undefined4* pArg2);
// RecordPedSlide

// PedProcessForces

extern void C2_HOOK_FASTCALL CBMovedByPhysics(tPed_character_instance* pCharacter, tPhysics_object* pPhysics, undefined4 pArg3);
extern int C2_HOOK_FASTCALL GetPedCount(void);
// GetPedPosition

extern void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

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

// SetThisPedPhysicing

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

extern void C2_HOOK_FASTCALL RenderLimbs(void);
extern void C2_HOOK_FASTCALL MungePedestrians(void);
// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

extern void C2_HOOK_FASTCALL ARResetNapalmBolts(void);

extern void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

extern void C2_HOOK_FASTCALL InitPedsForRace(void);

// RenderElectroBastardRays

// AdjustProxRay

// ResetProxRay

extern tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);
// NextPedCam

extern void C2_HOOK_FASTCALL PrevPedCam(void);

extern void C2_HOOK_FASTCALL FindNearestPed(void);

extern void C2_HOOK_FASTCALL ResetPedNearness(void);
extern void C2_HOOK_FASTCALL PedPreCollisionStuff(void);

// UpdateIfBackwardsAR

// ChangePedScaling

// ChangeHeadScaling

// MakeMaterialBlendy

// UnmakeMaterialBlendy

// TurnOffCollisions

// TurnOnCollisions

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

// MutatePed

extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);

extern void C2_HOOK_FASTCALL EtherealiseMaterial1(void* pMaterial);

extern void C2_HOOK_FASTCALL EtherealiseCharacter1(void* pCharacter);

extern void C2_HOOK_FASTCALL EtherealiseMaterial2(void* pMaterial);

extern void C2_HOOK_FASTCALL MakePedsNotEthereal(void);

extern void C2_HOOK_FASTCALL DeetherealiseMaterial1(void* pMaterial);

extern void C2_HOOK_FASTCALL DeetherealiseCharacter1(void* pCharacter);

extern void C2_HOOK_FASTCALL SavePedStuff(void);

// ComparePedStuff

// AdjustPedDiagnostics

extern int C2_HOOK_FASTCALL CalmDownAllPeds(void);

extern void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetZombiePedFolderNames(void);

extern void C2_HOOK_FASTCALL SetBloodPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetAlienPedFolderNames(void);

extern void C2_HOOK_FASTCALL AddChangedPoint(const br_vector3* pPoint);

extern int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);

extern int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void);

extern int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);

extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

extern void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

extern void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

extern void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on);

extern void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

extern void C2_HOOK_FASTCALL ClearOutMorphs(void);

extern void C2_HOOK_FASTCALL InitBoner(tPedForms_vtable* pTable);
// DRVector3SafeCross

// DRVector3SafeCrossXZ

extern void C2_HOOK_FASTCALL ScaleModel(br_model* pModel, int pArg2, float pScale);
extern void C2_HOOK_FASTCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);
// DRScaleModel

extern void C2_HOOK_FASTCALL DisposeMove(tPed_move* pMove);

// DisposeAllLoadedMoves

// ReadRemap

extern void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap);

// DisposeAllLoadedRemaps

extern tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName);
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

extern tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName);

extern void C2_HOOK_FASTCALL MakeRotationMatrix(br_matrix34* pMat, tU8 pAngle_x, tU32 pAngle_y, tU32 pAngle_z);
extern void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9);
// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

extern tPed_personality* C2_HOOK_FASTCALL FindOrOpenPersonality(const char* pName);

// BuildCharacterInstance

extern br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance *pCharacter);
extern br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index);
extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter);
extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
extern int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter);

extern int C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);
// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4);
extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance *pCharacter, int pArg2, undefined4 pArg3);

extern tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance *pPed);
extern int C2_HOOK_FASTCALL CharacterNoLongerCollideworthy(tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, br_vector3* pV);

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

extern int C2_HOOK_FASTCALL MakeCharacterPhysicworthy(tPed_character_instance* pCharacter);
extern void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);

extern int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2);

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

extern void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter);

extern int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2);
// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);

// DRMatrix34RotateCos

extern void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest);
extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);
extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pChar, br_vector3* pPosition, int pMode);
// DisposeClonedMaterials

// DisposeAllBonerData

extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set);
// SetCharacterBoneModelP

extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, undefined4 pArg2, int pArg3);

extern br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index);
extern int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter);
extern int C2_HOOK_FASTCALL BonerPassiveCollision(tPed_character_instance* pCharacter, undefined4 pArg2);

extern int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2);

// DismemberCharacter

extern float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum);

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

extern void C2_HOOK_FASTCALL MungeAllCharacterMaterials(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FASTCALL DisableOverallMovement(void);
extern void C2_HOOK_FASTCALL EnableOverallMovement(void);
extern void C2_HOOK_FASTCALL EnableRetainRootMode(void);

extern void C2_HOOK_FASTCALL DisableRetainRootMode(void);

extern void C2_HOOK_FASTCALL DisposeCharacterInstances(void);


extern void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pF);
extern void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13);
extern void C2_HOOK_FASTCALL FUN_004eb6a0(br_matrix34* pMatrix);
extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8);
extern void C2_HOOK_FASTCALL DisposePedPersonality(tPed_personality* pPersonality);
extern void C2_HOOK_FASTCALL DisposePedForm(tPed_form* pForm);
extern int C2_HOOK_FASTCALL MakeCharacterPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, br_actor* pActor, void* pFrame, br_vector3* pBonePos, int pKind, undefined4 pArg3, int pFlag, int pBoneIndex, br_vector3* pBoneVec, br_vector3* pBonePtr);
extern void C2_HOOK_FASTCALL MakeCharacterPhysicsSetup(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr);
extern void C2_HOOK_FASTCALL CachePedestrianFaces(tPedestrian* pPed, br_vector3* pHit_pos, tU32 pCache_key);
extern void C2_HOOK_FASTCALL DisposePedMoves(void);
extern void C2_HOOK_FASTCALL DisposeAllRemaps(void);
extern void C2_HOOK_FASTCALL DisposePedForms(void);
extern void C2_HOOK_FASTCALL DisposeAllPedPersonalities(void);
extern void C2_HOOK_FASTCALL FUN_00402dd0(void);
extern int C2_HOOK_FASTCALL FUN_004093e0(tPed_character_instance* pCharacter);
extern void C2_HOOK_FASTCALL PedNoOp(void);
extern int C2_HOOK_FASTCALL GetCharacterBoneIndexByName(tPed_character_instance* pCharacter, const char* pBone_name);
extern int C2_HOOK_FASTCALL MakeCharacterBonePhysicworthy(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pOmega);
extern void C2_HOOK_FASTCALL SetPhysicsObjectOmegaRecursive(tPhysics_object* pObject, br_vector3* pOmega);
extern void C2_HOOK_FASTCALL PedProcessContact(tPedestrian* pPed, tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL FUN_004d3600(void);
extern void C2_HOOK_FASTCALL MungeCharacterAnimationBody(tPed_character_instance* pCharacter, tU32 pTime, undefined4 pArg3);
extern void C2_HOOK_FASTCALL FUN_004d6c10(tPed_character_instance* pCharacter, undefined4 pArg2, char* pX, char* pY, char* pZ);

#endif // GUARD_61_PEDESTRN_H
