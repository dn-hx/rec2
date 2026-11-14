#ifndef GUARD_72_NETWORK_H
#define GUARD_72_NETWORK_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETWORK;


extern int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL InitNetStorageSpace(void);
extern int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend);
extern int C2_HOOK_CDECL NetworkJoinGoAhead(void);
extern int C2_HOOK_FASTCALL ChooseNetCar(tNet_game_details* pNet_game, tNet_game_options* pOptions, int* pCar_index, int pIm_the_host_so_fuck_off);

#endif /* GUARD_72_NETWORK_H */
