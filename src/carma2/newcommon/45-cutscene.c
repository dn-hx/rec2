#include "45-cutscene.h"

#include "18-graphics2.h"
#include "42-input.h"
#include "globvars.h"
#include "41-utility.h"
#include "69-sound.h"

// FUNCTION: CARMA2_HW 0x0043e750
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

// PlaySmackerFile

// DoOpeningAnimation

// FUNCTION: CARMA2_HW 0x0043f370
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// DoGameCompletedAnimation

// StartLoadingScreen

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
