/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "03-racestrt.h"

#include "rec2_types.h"
#include "globvrpb.h"

// GLOBAL: CARMA2_HW 0x006a0d38
// GLOBAL: CARMA2_SW 0x0069ad90
tU32 gLast_host_query;

// FUNCTION: CARMA2_HW 0x004e2b50
// FUNCTION: CARMA2_SW 0x004deed0
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

// SortOpponents

// DoGridPosition

// FUNCTION: CARMA2_HW 0x00497860
void C2_HOOK_FASTCALL ReceivedViewCar(int* pA, int* pB) {
    int i;
    tNet_game_player_info* p;

    if (gNet_mode != eNet_mode_none) {
        p = gNet_players;
        for (i = 0; i < gNumber_of_net_players; i++, p++) {
            if (p->ID == (tPlayer_ID)pB[2]) {
                *(int*)&gNet_players[i].field_0xd0 = pA[1];
                if (gNet_players[i].ID == (tPlayer_ID)pA[1]) {
                    *(int*)&gNet_players[i].car->collision_info->field_0x49c += 0x28;
                }
                return;
            }
        }
    }
}

