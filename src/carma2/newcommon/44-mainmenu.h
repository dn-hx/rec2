#ifndef GUARD_44_MAINMENU_H
#define GUARD_44_MAINMENU_H

#include "c2_hooks.h"
#include "rec2_types.h"

// QuitVerifyStart

// QuitVerifyDone

extern int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

extern void C2_HOOK_FASTCALL DoMainScreen(void);

// DoOptionsMenu

extern int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type);

#endif // GUARD_44_MAINMENU_H
