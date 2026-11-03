#ifndef GUARD_47_WIN95NET_H
#define GUARD_47_WIN95NET_H

#include "c2_hooks.h"
#include "rec2_types.h"

// StartLookup

// CreateNBPAddress

extern int C2_HOOK_FASTCALL PDNetHostGame(tNet_game_details* pDetails, char* pHost_name, void** pHost_address);

extern tU32 C2_HOOK_FASTCALL PDNetExtractGameID(tNet_game_details* pDetails);

// SetForRecieve

extern void C2_HOOK_FASTCALL PDNetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address);

#endif // GUARD_47_WIN95NET_H
