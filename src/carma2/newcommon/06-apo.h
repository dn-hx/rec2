#ifndef GUARD_06_APO_H
#define GUARD_06_APO_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern int gInitial_powerup_slots[3];
extern int gINT_0068b8e4;
extern int gINT_0068b8e8;
extern int gIt_or_fox;

extern void C2_HOOK_FASTCALL MaxOutAPO(void);
extern void C2_HOOK_FASTCALL ImprovePSPowerup(tCar_spec* pCar, int pIndex);

extern void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex);

extern void C2_HOOK_FASTCALL BuyArmour(void);

extern void C2_HOOK_FASTCALL BuyPower(void);

extern void C2_HOOK_FASTCALL BuyOffense(void);

#endif // GUARD_06_APO_H
