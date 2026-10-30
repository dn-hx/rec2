#ifndef GUARD_44_MAINMENU_H
#define GUARD_44_MAINMENU_H

#include "c2_hooks.h"
#include "rec2_types.h"

// QuitVerifyStart

extern int gMouse_was_started__mainmenu;

extern int gReplace_background;

extern int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);

extern int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

extern void C2_HOOK_FASTCALL DoMainScreen(void);

extern void C2_HOOK_FASTCALL DoOptionsMenu(void);

extern int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type);

#endif // GUARD_44_MAINMENU_H
