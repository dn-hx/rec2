#include "72-interface-network_options.h"

#include "72-interface.h"

extern tFrontend_spec gFrontend_MAIN;

/* NetOptions_Cancel and its data live here, not in 72-interface.c: additions to 72-interface.c shift MSVC's codegen of SortScores 0x004743c0 */

// GLOBAL: CARMA2_HW 0x006864a0
tNet_game_options gFrontend_backup_net_options;

/* inlined into NetOptions_Cancel in the original (no standalone copy) */
static void C2_HOOK_FASTCALL RestoreNetworkOptions(void) {

    gFrontend_net_options = gFrontend_backup_net_options;
}

extern int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern int gNet_last_game_type;
extern tNet_game_options gNet_settings[9];

// FUNCTION: CARMA2_HW 0x00473470
int C2_HOOK_FASTCALL NetOptions_Ok(tFrontend_spec* pFrontend) {

    gFrontend_net_options.show_players_on_map = !(WhichItemIsSelectedIn(pFrontend, 1) - 2);
    gFrontend_net_options.powerup_respawn = !(WhichItemIsSelectedIn(pFrontend, 2) - 5);
    gFrontend_net_options.open_game = !(WhichItemIsSelectedIn(pFrontend, 3) - 8);
    gFrontend_net_options.grid_start = !(WhichItemIsSelectedIn(pFrontend, 4) - 11);
    gFrontend_net_options.random_car_choice = !(WhichItemIsSelectedIn(pFrontend, 6) - 17);
    gFrontend_net_options.race_sequence_type = WhichItemIsSelectedIn(pFrontend, 5) - 14;
    gFrontend_net_options.car_choice = WhichItemIsSelectedIn(pFrontend, 7) - 20;

    gNet_settings[0] = gFrontend_net_options;
    gNet_settings[gNet_last_game_type + 1] = gFrontend_net_options;
    SaveOptions();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00473550
int C2_HOOK_FASTCALL NetOptions_Cancel(tFrontend_spec* pFrontend) {

    RestoreNetworkOptions();
    return 0;
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00472ed0
int C2_HOOK_FASTCALL NetOptions_Infunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004732b0
int C2_HOOK_FASTCALL NetOptions_TargetRoller(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00610208
tFrontend_spec gFrontend_NETWORK_OPTIONS = {
    "NetworkOptions",
    0,
    33,
    NetOptions_Infunc,
    NetOptions_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    5,
    0,
    {
        { 0x9c, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xcd, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xcf, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xa7, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x9e, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9f, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe6, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd0, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xc6, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc7, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd1, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe7, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xd3, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd4, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd5, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc8, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe8, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, NetOptions_CreditsRoller, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, NetOptions_CreditsRoller, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, NetOptions_TargetRoller, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, NetOptions_TargetRoller, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 7, NetOptions_Ok, (void*)1, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 8, NetOptions_Cancel, (void*)1, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
};
