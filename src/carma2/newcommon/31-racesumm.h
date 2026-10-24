#ifndef GUARD_31_RACESUMM_H
#define GUARD_31_RACESUMM_H

#include "c2_hooks.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

extern void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed);

extern int C2_HOOK_FASTCALL AnyBuyableCars(void);

extern void C2_HOOK_FASTCALL RecordAPOAndCredits(void);
// PickRaceWhenGameCompleted

// MungeRaceProgression

#endif // GUARD_31_RACESUMM_H
