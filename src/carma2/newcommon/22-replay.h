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

// AfterActionReplay

// ToggleReplay

// DisplayMovieStatusPanel

extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);

// ARSetPause

#endif // GUARD_22_REPLAY_H
