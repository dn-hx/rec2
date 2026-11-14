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
extern void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

extern void C2_HOOK_FASTCALL CheckReplayTurnOn(void);
extern void C2_HOOK_FASTCALL InitializeActionReplay(void);
// FreeActionReplayPix

extern void C2_HOOK_FASTCALL SynchronizeActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
extern void C2_HOOK_FASTCALL AfterActionReplay(void);
extern void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);

extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);

extern void C2_HOOK_FASTCALL SetReplayPaused(int pPause);

extern void C2_HOOK_FASTCALL ARApplyCrush(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL AdjustSpark(int field_0x0, void* pField_0x4, void* pField_0x10);

extern void C2_HOOK_FASTCALL ARUndoCrush(int field_0x4, void* pField_0x8, void* pField_0xe);


extern void C2_HOOK_FASTCALL ARUndoModelMash(int field_0x4, tU8* pField_0x8);

extern void C2_HOOK_FASTCALL ARApplyFlap(int field_0x4, tU16 field_0x8);

extern void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa);

extern void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int field_0x0, tU16 field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa);

extern void C2_HOOK_FASTCALL PHILApplyPHILObject(int field_0x0, br_matrix34* pField_0x4);

extern void C2_HOOK_FASTCALL PedApplyPedDir(int field_0x0, br_vector3* pDir);

extern void C2_HOOK_FASTCALL PedApplyPedMove(tPedestrian* pPed, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL PedUndoPedMove(tPedestrian* pPed, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL PedApplyPedFPChange(int field_0x0, tPipe_stack_word field_0x4);

extern void C2_HOOK_FASTCALL PedApplyStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);

extern void C2_HOOK_FASTCALL PedUndoStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);

extern void C2_HOOK_FASTCALL PedApplyPedPhysics(int field_0x0, br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL PedApplyPedPos(int field_0x0, br_vector3* pVec);

extern void C2_HOOK_FASTCALL PedApplyVanishDismembered(uintptr_t pOwner, tS32 value);

extern void C2_HOOK_FASTCALL PedApplyPedModelChangeP(int field_0x0, int field_0x4, int field_0xc, int field_0xa);

extern void C2_HOOK_FASTCALL PedApplyPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8);

extern void C2_HOOK_FASTCALL PedApplyDismember(int field_0x0, int field_0x4, int field_0x7);

extern void C2_HOOK_FASTCALL PedUndoDismember(int field_0x0, int field_0x4, int field_0x6);

extern void C2_HOOK_FASTCALL PedApplyGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c);

extern void C2_HOOK_FASTCALL PedUndoGibShower(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c);

extern void C2_HOOK_FASTCALL AdjustProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8);

extern void C2_HOOK_FASTCALL AdjustPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144);

extern void C2_HOOK_FASTCALL AdjustRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc);

extern void C2_HOOK_FASTCALL ARDoShitMine(int field_0x0, br_vector3* pPos);

extern void C2_HOOK_FASTCALL ARUndoShitMine(int field_0x0, br_vector3* pPos);

extern void C2_HOOK_FASTCALL ARPowerupGot(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL ARPowerupLose(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL DoInitialTail(void);

extern void C2_HOOK_FASTCALL ARDoPowerupRespawn(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL ARUndoPowerupRespawn(int field_0x0, int field_0x4);

extern void C2_HOOK_FASTCALL DoShrapnel(tU32 field_0x4, tU32 field_0x84, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0);

extern void C2_HOOK_FASTCALL DoBackwardShrapnel(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38);


extern void C2_HOOK_FASTCALL AdjustShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14);



extern void C2_HOOK_FASTCALL AdjustSplashReplay(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18);

extern void C2_HOOK_FASTCALL UndoInitialTail(void);

extern void C2_HOOK_CDECL ActorRelink(br_actor* pParent, br_actor* pActor);

extern void C2_HOOK_FASTCALL ARApplySplitWeld(void* roadMachine, br_vector3* pVector);

extern void C2_HOOK_FASTCALL ARUndoSplitWeld(void* roadMachine);


extern void C2_HOOK_FASTCALL ServiceGameInRace(void);
extern void C2_HOOK_FASTCALL ComparePedStuff(void);
extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);
extern br_uint_8 C2_HOOK_CDECL ActorToActorMatrix34(br_matrix34* m, br_actor* a, br_actor* b);
extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL ShatterObject(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform);
extern void C2_HOOK_FASTCALL PipeSingleNonCarCreation(uintptr_t pOwner);
extern void C2_HOOK_FASTCALL DisplayMovieStatusPanel(int pReplay_only);

#endif // GUARD_22_REPLAY_H
