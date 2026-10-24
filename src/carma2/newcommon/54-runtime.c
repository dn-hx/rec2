#include "54-runtime.h"

#include "c2_string.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006ba640
tS8 gRace_memory[1000];

// FUNCTION: CARMA2_HW 0x004e7440
int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress) {

    if (pAddress >= 0 && pAddress < (int)REC2_ASIZE(gRace_memory)) {
        return gRace_memory[pAddress];
    }
#ifdef REC2_FIX_BUGS
    return 0;
#endif
}
// FUNCTION: CARMA2_HW 0x004e7460
int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pDelta) {

    if (pAddress >= 0 && pAddress < (int)REC2_ASIZE(gRace_memory)) {
        gRace_memory[pAddress] += pDelta;
        return gRace_memory[pAddress];
    }
#ifdef REC2_FIX_BUGS
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x004e7480
int C2_HOOK_FASTCALL ToggleRuntimeVariable(int pAddress) {

    if (pAddress >= 0 && pAddress < (int)REC2_ASIZE(gRace_memory)) {
        if (gRace_memory[pAddress]) {
            gRace_memory[pAddress] = 0;
            return 0;
        } else {
            gRace_memory[pAddress] = 1;
            return 1;
        }
    }
#ifdef REC2_FIX_BUGS
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x004e74b0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

