#include "06-apo.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "27-powerup.h"
#include "41-utility.h"
#include "globvars.h"
#include "globvrpb.h"
#include "27-powerup.h"
#include "08-loading1.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00762130
int gInitial_powerup_slots[3];

// GLOBAL: CARMA2_HW 0x0068b8e4
int gINT_0068b8e4;

// GLOBAL: CARMA2_HW 0x0068b8e8
int gINT_0068b8e8;

// GLOBAL: CARMA2_HW 0x0074a734
int gIt_or_fox;

// FUNCTION: CARMA2_HW 0x00502e00
void C2_HOOK_FASTCALL MaxOutAPO(void) {
    int i;

    for (i = 0; i < 3; i++) {
        gProgram_state.current_car.power_up_levels[i] = gInitial_powerup_slots[i];
        gProgram_state.current_car.power_up_slots[i] = gInitial_powerup_slots[i];
    }
}
// FUNCTION: CARMA2_HW 0x00502e20
void C2_HOOK_FASTCALL ImprovePSPowerup(tCar_spec* pCar, int pIndex) {

    pCar->power_up_levels[pIndex]++;
    NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_increase_apo_start + pIndex));
}

// FUNCTION: CARMA2_HW 0x00502e70
// FUNCTION: CARMA2_SW 0x004fe7c0
void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex) {
    char message[256];
    char cost[256];

    if (gRace_finished || gProgram_state.current_car.knackered || gWait_for_it) {
        return;
    }
    if (gProgram_state.current_car.power_up_levels[pIndex] < gProgram_state.current_car.power_up_slots[pIndex]) {
        if (gProgram_state.credits < (gNet_mode != eNet_mode_none ? gCost_APO.initial_network[gCurrent_net_game->type] : gCost_APO.initial[gProgram_state.skill_level])) {
            strcpy(message, GetMiscString(eMiscString_cannot_afford_it_credits_S));
            sprintf(cost, "%d", gNet_mode != eNet_mode_none ? gCost_APO.initial_network[gCurrent_net_game->type] : gCost_APO.initial[gProgram_state.skill_level]);
            SubsStringJob(message, cost);
            NewTextHeadupSlot(4, 0, 3000, -4, message);
        } else {
            SpendCredits(gNet_mode != eNet_mode_none ? gCost_APO.initial_network[gCurrent_net_game->type] : gCost_APO.initial[gProgram_state.skill_level]);
            gProgram_state.current_car.power_up_levels[pIndex]++;
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_increase_apo_start + pIndex));
        }
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_you_re_already_at_max_you_need_more_slots));
    }
}

// FUNCTION: CARMA2_HW 0x00502e50
void C2_HOOK_FASTCALL BuyArmour(void) {

    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        BuyPSPowerup(0);
    }
}


