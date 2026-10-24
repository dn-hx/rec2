#include "06-apo.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "27-powerup.h"
#include "41-utility.h"
#include "globvars.h"
#include "globvrpb.h"
#include "27-powerup.h"
#include "08-loading1.h"
#include "c2_string.h"
#include "c2_stdio.h"

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

// BuyPSPowerup

// BuyArmour

// BuyPower

// BuyOffense

