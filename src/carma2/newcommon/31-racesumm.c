#include "31-racesumm.h"

#include "globvars.h"
#include "globvrpb.h"
#include "18-graphics2.h"
#include "44-mainmenu.h"
#include "30-opponent.h"
#include "01-network.h"
#include "35-intrface.h"
#include "platform.h"
#include "69-sound.h"

// GLOBAL: CARMA2_HW 0x007051f8
int gMoney_pre_race;

// GLOBAL: CARMA2_HW 0x007051e0
int gAPO_pre_race[3];

// GLOBAL: CARMA2_HW 0x007051a0
int gPlayer_lookup_racesumm[14];

// GLOBAL: CARMA2_HW 0x0074a694
int gReceived_game_scores;

// SortScores

// SortGameScores

// DoNetRaceSummary

// FUNCTION: CARMA2_HW 0x004e33a0
static int C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {

    return gNet_players[*(int*)pArg2].score - gNet_players[*(int*)pArg1].score;
}


// AnyBuyableCars

// RecordAPOAndCredits

// PickRaceWhenGameCompleted

// MungeRaceProgression

