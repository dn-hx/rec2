#ifndef GUARD_72_MAIN_H
#define GUARD_72_MAIN_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_MAIN;


extern void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pFrontend_type);
extern int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend);
extern tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp);
extern void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index);
extern int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index);

#endif /* GUARD_72_MAIN_H */
