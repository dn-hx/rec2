#ifndef GUARD_03_RACESTRT_H
#define GUARD_03_RACESTRT_H

#include "c2_hooks.h"

extern int gOur_starting_position;

extern int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);

extern int C2_HOOK_FASTCALL SortOpponents(void);
// DoGridPosition

// ReceivedViewCar

#endif // GUARD_03_RACESTRT_H
