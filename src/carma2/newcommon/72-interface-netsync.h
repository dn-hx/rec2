#ifndef GUARD_72_NETSYNC_H
#define GUARD_72_NETSYNC_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETSYNC;


extern int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend);

#endif /* GUARD_72_NETSYNC_H */
