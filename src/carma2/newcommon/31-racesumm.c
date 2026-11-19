/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "31-racesumm.h"

#include "30-opponent.h"

#include "globvars.h"
#include "35-intrface.h"
#include "globvrpb.h"
#include "01-network.h"
#include "platform.h"
#include "72-interface.h"
#include "18-graphics2.h"
#include "69-sound.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

static int C2_HOOK_CDECL SortScores_2(const void* pArg1, const void* pArg2) {

    return gNet_players[*(int*)pArg2].score - gNet_players[*(int*)pArg1].score;
}

// GLOBAL: CARMA2_HW 0x007051a0
int gPlayer_lookup_racesumm[14];

// GLOBAL: CARMA2_HW 0x0074a694
int gReceived_game_scores;

// FUNCTION: CARMA2_HW 0x004e32b0
void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed) {
    int start_time;
    int i;
    int count;

    ResetInterfaceTimeout__intrface();
    if (gNet_mode != eNet_mode_none) {
        NetPlayerStatusChanged(ePlayer_status_summary);
        start_time = PDGetTotalTime();
        while (!gReceived_game_scores && (int)PDGetTotalTime() - start_time < 20000) {
            NetService(0);
        }
        if (!gReceived_game_scores) {
            return;
        }
        for (i = 0; i < gNumber_of_net_players; i++) {
            gPlayer_lookup_racesumm[i] = i;
        }
        qsort(gPlayer_lookup_racesumm, gNumber_of_net_players, sizeof(int), SortScores_2);
        FRONTEND_Main(kFrontend_menu_networksummary);
        NetPlayerStatusChanged(ePlayer_status_loading);
        FadePaletteDown();
        return;
    }

    if (pGame_completed) {
        return;
    }

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            StartMusicTrack(0x270e);
            FRONTEND_Main(kFrontend_menu_wrecks);
            return;
        }
    }
}
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

// FUNCTION: CARMA2_HW 0x004e3410
// FUNCTION: CARMA2_SW 0x004df7a0
void C2_HOOK_FASTCALL RecordAPOAndCredits(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, power_up_levels, 0x18a8);

    gMoney_pre_race = gProgram_state.credits;
    gAPO_pre_race[0] = gProgram_state.current_car.power_up_levels[0];
    gAPO_pre_race[1] = gProgram_state.current_car.power_up_levels[1];
    gAPO_pre_race[2] = gProgram_state.current_car.power_up_levels[2];
}

// PickRaceWhenGameCompleted

// MungeRaceProgression

