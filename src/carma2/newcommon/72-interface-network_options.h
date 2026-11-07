#ifndef GUARD_72_NETWORK_OPTIONS_H
#define GUARD_72_NETWORK_OPTIONS_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETWORK_OPTIONS;


extern int C2_HOOK_FASTCALL NetOptions_Infunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetOptions_TargetRoller(tFrontend_spec* pFrontend);

#endif /* GUARD_72_NETWORK_OPTIONS_H */
