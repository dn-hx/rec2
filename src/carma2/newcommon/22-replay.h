#ifndef GUARD_22_REPLAY_H
#define GUARD_22_REPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include "23-piping.h"

extern int gAR_camera_type;

extern void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard);

extern void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);
extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);
extern void C2_HOOK_FASTCALL DoActionReplayHeadups(void);

extern void C2_HOOK_FASTCALL PreProcess(int pFrame_period);
extern void C2_HOOK_FASTCALL PostProcess(int pFrame_period);
// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

extern void C2_HOOK_FASTCALL CheckReplayTurnOn(void);
extern void C2_HOOK_FASTCALL InitializeActionReplayPix(void);
// FreeActionReplayPix

extern void C2_HOOK_FASTCALL SynchronizeActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
extern void C2_HOOK_FASTCALL AfterActionReplay(void);
extern void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);
// DisplayMovieStatusPanel

extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);

extern void C2_HOOK_FASTCALL ARSetPause(int pPause);

extern void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale);

extern void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10);

extern void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL WorkerModelMash(int field_0x4, tU8* pField_0x8);

extern void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8);

extern void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tU16 field_0x8);

extern void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa);

extern void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, tU16 field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa);

extern void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4);

extern void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir);

extern void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4);

extern void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);

extern void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);

extern void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL WorkerPedPos(int field_0x0, br_vector3* pVec);

extern void C2_HOOK_FASTCALL DoVanishDismembered(uintptr_t pOwner, tS32 value);

extern void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa);

extern void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8);

extern void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7);

extern void C2_HOOK_FASTCALL WorkerPedDismemberUndo(int field_0x0, int field_0x4, int field_0x6);

extern void C2_HOOK_FASTCALL WorkerGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c);

extern void C2_HOOK_FASTCALL WorkerGibShowerUndo(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c);

extern void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8);

extern void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144);

extern void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc);

extern void C2_HOOK_FASTCALL WorkerShitMine(int field_0x0, br_vector3* pPos);

extern void C2_HOOK_FASTCALL WorkerEndShitMine(int field_0x0, br_vector3* pPos);

extern void C2_HOOK_FASTCALL WorkerPowerupGot(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL WorkerPowerupLose(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL WorkerSpecial(void);

extern void C2_HOOK_FASTCALL WorkerPowerupRespawn(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL WorkerPowerupRespawnUndo(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 field_0x4, tU32 field_0x84, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0);

extern void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38);

extern void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4);

extern void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14);

extern void C2_HOOK_FASTCALL WorkerSmokeColumn(int column_flags, br_vector3* pPos, int vertex, int pFlags_hi);

extern void C2_HOOK_FASTCALL WorkerFlame(int index, int flag, float f1, float f2, float f3, float f4);

extern void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18);

extern void C2_HOOK_FASTCALL WorkerSpecialUndo(void);

extern void C2_HOOK_CDECL RelinkActorKeepWorldTransform(br_actor* pParent, br_actor* pActor);

extern void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* roadMachine, br_vector3* pVector);

extern void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine);


extern void C2_HOOK_FASTCALL ServiceGameInRace(void);
extern void C2_HOOK_FASTCALL ComparePedStuff(void);
extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);
extern br_uint_8 C2_HOOK_CDECL ActorToActorMatrix34Local(br_matrix34* m, br_actor* a, br_actor* b);
extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);
extern tPhysics_object* C2_HOOK_FASTCALL MakeCharacterPhysicsForBone(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, undefined4 pArg4);
extern void C2_HOOK_FASTCALL FUN_004f8b50(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials);
extern void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform);
extern void C2_HOOK_FASTCALL FUN_004c8940(uintptr_t pOwner);
extern void C2_HOOK_FASTCALL FUN_0040b500(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4);
extern void C2_HOOK_FASTCALL DrawStatusMessage(int pReplay_only);

#endif // GUARD_22_REPLAY_H
