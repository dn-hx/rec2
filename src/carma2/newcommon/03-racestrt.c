#include "03-racestrt.h"

#include "rec2_types.h"
#include "globvars.h"
#include "41-utility.h"

// GLOBAL: CARMA2_HW 0x006a0c74
int gOur_starting_position;

// FUNCTION: CARMA2_HW 0x004e2b50
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

// SortOpponents

// DoGridPosition

// ReceivedViewCar

