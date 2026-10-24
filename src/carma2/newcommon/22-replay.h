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

extern void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL WorkerModelMash(int field_0x4, tU8* pField_0x8);

extern void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8);

extern void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tU16 field_0x8);

extern void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa);

extern void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, tU16 field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa);

extern void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4);


#endif // GUARD_22_REPLAY_H
