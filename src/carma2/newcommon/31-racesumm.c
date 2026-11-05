/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "31-racesumm.h"
#include "01-network.h"
#include "18-graphics2.h"

#include "30-opponent.h"
#include "44-mainmenu.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary


// OriginalResetInterfaceTimeout

// gReceived_game_scores

// gPlayer_lookup_racesumm

// gMoney_pre_race

// gAPO_pre_race
// FUNCTION: CARMA2_HW 0x004e33d0
// FUNCTION: CARMA2_SW 0x004df760
int C2_HOOK_FASTCALL AnyBuyableCars(void) {
    int count;
    int i;

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            return 1;
        }
    }
    return 0;
}

// RecordAPOAndCredits

// PickRaceWhenGameCompleted

// MungeRaceProgression

