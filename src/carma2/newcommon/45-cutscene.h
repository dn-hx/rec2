#ifndef GUARD_45_CUTSCENE_H
#define GUARD_45_CUTSCENE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern void C2_HOOK_FASTCALL DoSCILogo(void);

extern void C2_HOOK_STDCALL DoStainlessLogo(void);

extern void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name);

extern void C2_HOOK_FASTCALL DoOpeningAnimation(void);

extern void C2_HOOK_FASTCALL DoNewGameAnimation(void);

extern void C2_HOOK_FASTCALL DoGoToTrackAnimation(void);

extern void C2_HOOK_FASTCALL DoEndRaceAnimation(void);

extern void C2_HOOK_FASTCALL DoGameOverAnimation(void);

extern void C2_HOOK_FASTCALL DoGameCompletedAnimation(void);

extern void C2_HOOK_FASTCALL StartLoadingScreen(void);


extern void C2_HOOK_FASTCALL SetFlicSound(int pSound_ID, tU32 pSound_time);
extern void C2_HOOK_FASTCALL DustRotate(void);
extern void C2_HOOK_FASTCALL DoFeatureUnavailableInDemo(void);

#endif // GUARD_45_CUTSCENE_H
