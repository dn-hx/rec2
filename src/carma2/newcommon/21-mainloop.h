#ifndef GUARD_21_MAINLOOP_H
#define GUARD_21_MAINLOOP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */
extern int gAuto_increase_credits_dt[3];
extern tU32 gNet_auto_increase_credits_dt[8];

extern tU32 gOld_camera_time;
extern tU32 gActual_last_tick_count;
extern tU32 gAverage_frame_period;
extern tU32 gLast_tick_count;

// CalculateFrameRate

// LoseOldestWastedMassage

extern void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex);

extern void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

// MungeHeadups

extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);

// CheckTimer

// MungeRaceFinished

extern void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period);
// CheckActiveCarList

extern tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

extern tRace_result C2_HOOK_FASTCALL DoRace(void);


extern void C2_HOOK_FASTCALL ResetCarScreens(void);
extern void C2_HOOK_FASTCALL MungeJoystickHeadups(void);
extern void C2_HOOK_FASTCALL LastChanceForPedEffects(void);
extern void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, void (C2_HOOK_FASTCALL * pBetween_sessions)(int), tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);
extern void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL MouseClickInActionReplay(int pCursor_x, int pCursor_y, int pArg3);
extern void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost);
extern void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost);
extern void C2_HOOK_FASTCALL PollActionReplayControls(tU32* pFrame_period, tU32* pAverage_frame_period);
extern void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);
extern void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pBuffer);
extern void C2_HOOK_FASTCALL CreateKeyConfigurationHeadups(tHeadup_text_buffer* pBuffer);
extern void C2_HOOK_FASTCALL PrecacheData(volatile tU8* pBase, tU32 pSize);

#endif // GUARD_21_MAINLOOP_H
