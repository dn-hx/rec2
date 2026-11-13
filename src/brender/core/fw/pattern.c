#include "pattern.h"

#include <ctype.h>

// FUNCTION: CARMA2_HW 0x00529860
br_boolean C2_HOOK_CDECL BrNamePatternMatch(const char* p, const char* s) {
    char cVar1;
    char cVar2;
    int iVar3;
    int iVar4;
    int iVar5;

    if (p == NULL) {
        return 1;
    }
    if (s == NULL) {
        return 0;
    }
    do {
        cVar1 = *p;
        iVar5 = (int)cVar1;
        switch (iVar5) {
        case 0:
        case 0x2f:
            return *s == '\0';
        default:
            cVar2 = *s;
            if ((cVar2 < 'a') || ('z' < cVar2)) {
                iVar3 = (int)cVar2;
            } else {
                iVar3 = cVar2 + -0x20;
            }
            if ((cVar1 < 'a') || (iVar4 = iVar5 + -0x20, 'z' < cVar1)) {
                iVar4 = iVar5;
            }
            if (iVar3 != iVar4) {
                return 0;
            }
            p = p + 1;
            s = s + 1;
            break;
        case 0x2a:
            iVar5 = BrNamePatternMatch(p + 1, s);
            if (iVar5 != 0) {
                return 1;
            }
            do {
                cVar1 = *s;
                s = s + 1;
                if (cVar1 == '\0') {
                    return 0;
                }
                iVar5 = BrNamePatternMatch(p + 1, s);
            } while (iVar5 == 0);
            return 1;
        case 0x3f:
            if (*s == '\0') {
                return 0;
            }
            p = p + 1;
            s = s + 1;
        }
    } while (1);
}
