#ifndef GUARD_22_REPLAY_H
#define GUARD_22_REPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include "23-piping.h"

extern int gAR_camera_type;

extern void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard);

extern void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);
// DoZappyActionReplayHeadups

extern void C2_HOOK_FASTCALL DoActionReplayHeadups(void);

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// SynchronizeActionReplay

// TurnOnActionReplay

// TurnOffActionReplay

extern void C2_HOOK_FASTCALL AfterActionReplay(void);
// ToggleReplay

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

extern void C2_HOOK_FASTCALL WorkerPedMove(int field_0x0, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(int field_0x0, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);

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


#endif // GUARD_22_REPLAY_H
