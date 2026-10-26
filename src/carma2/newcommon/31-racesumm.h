#ifndef GUARD_31_RACESUMM_H
#define GUARD_31_RACESUMM_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gMoney_pre_race;
extern int gAPO_pre_race[3];
extern int gPlayer_lookup_racesumm[14];
extern int gReceived_game_scores;

// SortScores

// SortGameScores

// DoNetRaceSummary

extern void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed);

extern int C2_HOOK_FASTCALL AnyBuyableCars(void);

extern void C2_HOOK_FASTCALL RecordAPOAndCredits(void);
// PickRaceWhenGameCompleted

// MungeRaceProgression

#endif // GUARD_31_RACESUMM_H
