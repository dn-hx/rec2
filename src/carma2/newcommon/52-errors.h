#ifndef GUARD_52_ERRORS_H
#define GUARD_52_ERRORS_H

#include "brender/br_types.h"
#include "c2_hooks.h"
#include "rec2_types.h"

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);

extern void C2_HOOK_CDECL NonFatalError(int pStr_index, ...);

extern void C2_HOOK_FASTCALL CloseDiagnostics(void);

extern void C2_HOOK_FASTCALL OpenDiagnostics(void);

extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);


extern void C2_HOOK_FASTCALL DumpVector3Diagnostic(const char* pName, const br_vector3* pVec);

#endif // GUARD_52_ERRORS_H
