#include "72-interface-netsync.h"

#include "72-interface.h"

extern tFrontend_spec gFrontend_MAIN;
extern int C2_HOOK_FASTCALL NetSync_Start(tFrontend_spec* pFrontend);


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00473610
int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00473870
int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0061bad0
tFrontend_spec gFrontend_NETSYNC = {
    "NetSync",
    0,
    28,
    NetSync_Infunc,
    0,
    NetSync_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    6,
    0,
    {
        { 0xab, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe, NetSync_Start, NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15, NetSync_Abort, NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
};
