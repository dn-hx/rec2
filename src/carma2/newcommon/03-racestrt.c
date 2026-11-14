#include "03-racestrt.h"

#include "rec2_types.h"
#include "globvars.h"
#include "globvrpb.h"
#include "72-interface.h"
#include "41-utility.h"
#include "01-network.h"
#include "platform.h"

// GLOBAL: CARMA2_HW 0x006a0c74
int gOur_starting_position;

// FUNCTION: CARMA2_HW 0x004e2b50
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

// FUNCTION: CARMA2_HW 0x004e2b70
int C2_HOOK_FASTCALL SortOpponents(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        if (gCurrent_race.opponent_list[i].index < 0) {
            return 3;
        }
    }

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);
    qsort(gCurrent_race.opponent_list, gCurrent_race.number_of_racers, sizeof(tOpp_spec), SortGridFunction);
    gOur_starting_position = IRandomBetween(0, gCurrent_race.number_of_racers - 1);


    for (i = gCurrent_race.number_of_racers; i > gOur_starting_position; i--) {
        gCurrent_race.opponent_list[i] = gCurrent_race.opponent_list[i - 1];
    }
    gCurrent_race.opponent_list[gOur_starting_position].index = -1;
    gCurrent_race.opponent_list[gOur_starting_position].ranking = 0;
    gCurrent_race.opponent_list[gOur_starting_position].car_spec = &gProgram_state.current_car;
    gCurrent_race.number_of_racers++;
    return 3;
}
// DoGridPosition

// ReceivedViewCar

// FUNCTION: CARMA2_HW 0x004e2c30
void C2_HOOK_CDECL CheckPlayersAreResponding(void) {
    int i;
    tU32 total_time;

    total_time = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            tNet_game_player_info* player = &gNet_players[i];
            if ((total_time - player->last_heard_from_him) > 20000) {
                player->player_status = ePlayer_status_not_responding;
            }
        }
    }
    if ((gNet_mode == 3) && (gLast_host_query == 0)) {
        tNet_message* msg = NetBuildGuaranteedMessage(0xe, 0);
        NetGuaranteedSendMessageToHost(gCurrent_net_game, msg, (void*)0);
        gLast_host_query = total_time;
    }
}

