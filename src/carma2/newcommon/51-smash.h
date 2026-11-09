#ifndef GUARD_51_SMASH_H
#define GUARD_51_SMASH_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tExplosion gExplosions[50];
extern br_vector3 gZero_vector__smash;
extern tDecal gDecals[50];
extern int gCount_queued_smashes;
extern tSmashable_environment_name* gSmashable_noncars;
extern const char* gExplosion_rotate_names[2];
extern int gShrapnel_ghost_actor_count;
extern const char* gInitial_smashable_position_type_names[3];
extern const char* gInitial_position_sphere_where[2];
extern tSmash_vertex gSmash_glass_fragments[200];
extern tRepair_animation gRepair_animations[25];
extern int gCount_host_smashes;
extern tNet_stored_smash* gNet_host_smashes;
extern tQueued_net_smash gQueued_net_smashes[50];
extern int gSize_powerup_queue;
extern tSmash_explosion gSmash_explosions[20];

extern void C2_HOOK_FASTCALL InitExplosions(void);

extern void C2_HOOK_FASTCALL ResetExplosions(void);

// RemoveAllExplosionettes

extern void C2_HOOK_FASTCALL MungeExplosions(void);
extern void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion);

// DoExplosionette

extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec);

// CalcRandomPointBetween

extern void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9);

extern void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

// ApplyTimeSpec

#ifndef REC2_NO_SMASH_DECLS
extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority);
#endif

// SetShrapnelFragment

// ShatterObject

// MungeInternalCarGlass

// ActuallySmashPart

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// SetInitPosition

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

#ifndef REC2_NO_SMASH_DECLS
extern void C2_HOOK_FASTCALL DoSpillage(tSlick_spec* pSlick, br_actor* pActor);
#endif

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

extern void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);
// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

extern void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor* pActor);

// DoConnotationsMultiLevel

extern void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tPhysics_object* pObject, tSmashable_item_spec* pSpec, undefined4 pArg3, tConnotations* pConnotations, tU32 pSeed, undefined4 pArg6, const br_vector3* pPoint, const br_vector3* pSound_position, br_actor* pActor, tU32 pTime, undefined4 pArg11, br_matrix34* pTransform, br_bounds3* pBounds, br_vector3* pPosition, float pDistance, undefined4 pArg16, int pFlag);
extern void C2_HOOK_FASTCALL InitSmashQueue(void);

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// ReadShrapnelMovementSpec

// ReadShrapnelInitialPosition

// ReadShrapnelTimeInfo

// NoteChildren

// ReadSmokeFireChance

// LoadActorFromRace

extern void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count);
typedef struct tMin_max_time {
  tU32 min_time;
  tU32 max_time;
} tMin_max_time;

extern void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, tMin_max_time* pTimes);

extern void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick);
extern void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed);

extern void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos);

extern void C2_HOOK_FASTCALL ReadExplosion(FILE *pF, tExplosion_animation* pAnimation);
// FreeExplosion

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

// ReadSmashLevels

// ReadSmashableData

extern void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

// KillFragment

// AddRepairAnimationToQueue

extern void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data);
extern intptr_t C2_HOOK_CDECL AddGhostActorToBuffer(br_actor* pActor, void* data);

extern void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data);

// InitGlassFragments

// InitDecals

// RemoveAllDecals

extern void C2_HOOK_FASTCALL ReadPowerupSmash(FILE* pF);

extern void C2_HOOK_FASTCALL InitSmashing(void);

// VerySimpleRotate

extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);
// MungeAnimationRepairs

extern void C2_HOOK_FASTCALL MungeGlassFragments(void);
// KillAllGlassFragments

extern void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

extern void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data);

// DoBackwardShrapnel

extern void C2_HOOK_FASTCALL InitRepairAnimations(void);

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

extern void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void);
// CloneMemory

// CloneEnvSmash

extern void C2_HOOK_FASTCALL FreeConnotations(tConnotations* pConnotations, int pKeep_actors);

extern void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash);
extern void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void);
// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

// ExtrapolateGeometryCoordinates

// ExtractDeTiledFaces

// AddDecal

// InsertQuad

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);
extern float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);

extern void C2_HOOK_FASTCALL NonCarSnapSmashCallback(tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce);

extern int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material* pMaterial);
extern intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal);

extern void C2_HOOK_FASTCALL ReinitSmashing(void);

// LinkSmashies

extern intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser);

extern intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec);

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

extern void C2_HOOK_FASTCALL LoadWastedExplosion(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

extern void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

extern void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, int pCount);

// SmashRestoreSomeFaces

extern void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL ChangeSmashTexture(br_material* pMaterial, tCar_spec* pCar_spec, br_pixelmap* pOld_texture, br_pixelmap* pNew_texture);

extern void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash);

extern void C2_HOOK_FASTCALL UpdateCrushTimers(void);

extern void C2_HOOK_FASTCALL ReceivedSmashEnvironment(tQueued_net_smash* pSmash);

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers

extern void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race);


extern void C2_HOOK_FASTCALL FreeExplosionAnimation(tExplosion_animation* pAnimation);
extern void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pLevels, int* pCount, int pIs_texture_change, tBrender_storage* pStorage);
extern void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);
extern void C2_HOOK_FASTCALL ActuallyRepairSmashThunk(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pUnused, int pLevel);
extern void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);
extern void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
extern float C2_HOOK_FASTCALL FUN_004f18d0(tPhysics_object* pObject, br_material* pMaterial, br_actor* pActor, tSmashable_item_spec* pSpec, tSmashable_item_spec* pSpec2, tFace_ref* pFace, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg11, int pArg12, int* pFlag, int pNetwork);
extern float C2_HOOK_FASTCALL ApplySmashTriggerEffect(tSmashable_item_spec* pSpec, tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int* pFlag, int pNetwork);
extern void C2_HOOK_FASTCALL ApplySmashTriggerNumber(tPhysics_object* pObject, tFace_ref* pFace, br_scalar pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg7);
extern void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);
extern void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);
extern void C2_HOOK_FASTCALL QueueNonCarCuboidActivation(br_actor* pActor, tNon_car_cuboid_activations* pActivations, void* pContext);
extern void C2_HOOK_FASTCALL FUN_004edbe0(tSmashable_item_spec* pSpec, void* pObject, tShrapnel_side_effects* pSide_effects, undefined4 pArg16, float pDistance);
extern void C2_HOOK_FASTCALL FUN_004f1030(tPhysics_object* pObject, int pMode, br_actor* pActor);
extern int C2_HOOK_FASTCALL FUN_004f0d50(const float* pPoint, const float* pNormal, float pPlane_distance, const float* pVertex0, const float* pVertex1, const float* pVertex2);
extern intptr_t C2_HOOK_CDECL FUN_004edb20(br_actor* pActor, void* pContext);
extern void C2_HOOK_FASTCALL FUN_004d9d00(tPhysics_object* pObject, int pMode);
extern int C2_HOOK_FASTCALL FUN_004f4c70(tFace_ref* pFace, br_vector3* pPoint, tFace_ref* pOut, int pFlag);
extern void C2_HOOK_FASTCALL FUN_004f3740(const int* pIndices, const float* pTransform, const float* pPoint, float* pResult);
extern void C2_HOOK_FASTCALL FUN_004f36c0(const void* pMap, br_vector2* pUV, br_vector3* pOut);
extern br_actor* C2_HOOK_FASTCALL FUN_004f3820(tFace_ref* pFace, br_vertex* pQuad, br_material* pMaterial, undefined4 pPoly_array, undefined4 pVertex_array);
extern int C2_HOOK_FASTCALL FUN_004f39a0(const void* pMap, br_actor* pActor, br_vertex* pQuad, tU8* pEdge_at_bounds, tU8** pCorner, br_material* pMaterial, undefined4 pPoly_array, undefined4 pVertex_array);
extern void C2_HOOK_FASTCALL MungeSmashMaterialName(br_material* pMaterial, char** pIdentifier, char pIndex);
extern void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel);
extern void C2_HOOK_FASTCALL FUN_004d35e0(tPedestrian* pPed);
extern void C2_HOOK_FASTCALL FUN_004c8bf0(tPedestrian* pPed, tU8 pBone, tS8 pArg3, tS8 pArg4);
extern int C2_HOOK_FASTCALL FUN_004cd9e0(tPedestrian* pPed, int pBoneIndex);
extern int C2_HOOK_FASTCALL FUN_004cd640(tPedestrian* pPed, int pSpeed, int pMove1, int pMove2, tPhysics_object* pCollision_info);

/* Payload of the net message chunk that reproduces a non-car shrapnel smash on
   the other players' machines (chunk type 0x22). Used by 51-smash.c only. */
typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tU32 seed;
    tU16 field_0x8;
    tS8 field_0xa;
    tS8 field_0xb;
    br_vector3 v;
    br_vector3 omega;
    br_matrix34 mat;
    int probability_fire;
    int count_fire_columns;
    int min_smokiness;
    int max_smokiness;
} tNet_message_chunk_non_car_shrapnel;

extern void C2_HOOK_FASTCALL FUN_004f6320(tNet_message_chunk_non_car_shrapnel* pSpec);

#endif // GUARD_51_SMASH_H
