#include "72-interface-networksummary.h"

#include "72-interface.h"

extern tFrontend_spec gFrontend_MAIN;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00473f40
int C2_HOOK_FASTCALL NetSummary_Infunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00627398
tFrontend_spec gFrontend_NETWORK_SUMMARY = {
    "NetworkSummary",
    30000,
    80,
    NetSummary_Infunc,
    Generic_Outfunc,
    NetSummary_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    7,
    0,
    {
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xfc, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xfb, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xb9, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xbb, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xb6, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
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
        { 0xb8, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xb7, temp, NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
};
