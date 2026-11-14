#ifndef SOFTREND_DRIVER_H
#define SOFTREND_DRIVER_H

#include <brender/brender.h>

#include "c2_hooks.h"

extern br_device* C2_HOOK_CDECL BrDrv1SoftRendBegin(const char* arguments);

#endif /* SOFTREND_DRIVER_H */
