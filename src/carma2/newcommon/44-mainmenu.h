#ifndef GUARD_44_MAINMENU_H
#define GUARD_44_MAINMENU_H

#include "c2_hooks.h"
#include "rec2_types.h"

// QuitVerifyStart (defined in 44-mainmenu.c, only used there; a declaration here shifts MSVC codegen of DistanceFromFaceND 0x005162a0 via 41-utility.c)

extern int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);
extern int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

extern void C2_HOOK_FASTCALL DoMainScreen(void);

extern void C2_HOOK_FASTCALL DoOptionsMenu(void);

extern int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type);

#endif // GUARD_44_MAINMENU_H
