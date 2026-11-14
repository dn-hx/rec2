#ifndef GUARD_72_MAIN_H
#define GUARD_72_MAIN_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_MAIN;


extern void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pFrontend_type);
extern void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index);
extern int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend);

#endif /* GUARD_72_MAIN_H */
