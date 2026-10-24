#ifndef GUARD_54_RUNTIME_H
#define GUARD_54_RUNTIME_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tS8 gRace_memory[1000];

extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);

// AccumulateRuntimeVariable

// ToggleRuntimeVariable

extern void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void);

#endif // GUARD_54_RUNTIME_H
