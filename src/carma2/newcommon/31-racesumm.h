/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_31_RACESUMM_H
#define GUARD_31_RACESUMM_H

#include "c2_hooks.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

extern int C2_HOOK_FASTCALL AnyBuyableCars(void);

extern void C2_HOOK_FASTCALL RecordAPOAndCredits(void);

// PickRaceWhenGameCompleted

// MungeRaceProgression

// OriginalResetInterfaceTimeout

// gReceived_game_scores

// gPlayer_lookup_racesumm

extern int gMoney_pre_race;

extern int gAPO_pre_race[3];

#endif // GUARD_31_RACESUMM_H
