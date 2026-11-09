#ifndef GUARD_72_NETWORK_H
#define GUARD_72_NETWORK_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETWORK;


extern int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL InitNetStorageSpace(void);
extern int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend);
extern int C2_HOOK_CDECL NetworkJoinGoAhead(void);

#endif /* GUARD_72_NETWORK_H */
