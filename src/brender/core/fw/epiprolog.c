#include "epiprolog.h"
/*
 * Entry/exit tracking hooks for compiler
 */

// FUNCTION: CARMA2_HW 0x0052d050
void C2_HOOK_CDECL Brender_EPI_PRO(void) {
}

void C2_HOOK_CDECL _PRO(void) {
    Brender_EPI_PRO();
}
