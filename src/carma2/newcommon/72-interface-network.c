#include "72-interface-network.h"

#include "72-interface.h"

#include "c2_string.h"
#include <stdio.h>

extern tFrontend_spec gFrontend_MAIN;
extern tFrontend_spec gFrontend_NETWORK_OPTIONS;
extern tFrontend_spec gFrontend_OPTIONS;

#include "01-network.h"
#include "02-init.h"
#include "10-loading2.h"
#include "24-loadsave.h"
#include "28-world3.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"
#include "41-utility.h"

/* local declarations (no extern added to a header, see PROGRESS: header-count sensitivity) */
extern int gValid_stashed_save_game;
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);
extern tNet_game_options gNet_settings[9];
extern int gNet_grid_starts[10];

/* local declarations for the join-go-ahead body (no extern added to a header) */
extern void C2_HOOK_FASTCALL ClearAlwaysTyping(void);
extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details* pGame_details, char* pPlayer_name, int pCar_index);
extern tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index);
extern void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);
extern void C2_HOOK_FASTCALL AddToJoinList(tNet_game_details* pGame);
extern void C2_HOOK_FASTCALL SetNetAvailability(tNet_game_options* pOptions);
extern int C2_HOOK_FASTCALL PickARandomCar(void);
extern tNet_game_details* gGame_to_join;
extern int gFrontend_net_car_index;
extern char gFrontend_host_join_buffer[256];
extern int gNet_join_host_result;
extern tNet_game_options* gChosen_net_game_options;
extern tNet_mode gFrontend_net_mode;

// GLOBAL: CARMA2_HW 0x00763920
tNet_game_type gFrontend_game_type;

// GLOBAL: CARMA2_HW 0x00764e98
int gRace_index;

/* explicit initializers keep gGames_to_join and the next global adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x00688718
tJoinable_game gGames_to_join[6] = { 0 };

// GLOBAL: CARMA2_HW 0x00688748
int gINT_00688748 = 0;

// GLOBAL: CARMA2_HW 0x00688760
int gLast_graph_sel;


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// ScrollUp and RefreshNetRacesScroller are inlined in the original.
// FUNCTION: CARMA2_HW 0x00466c00
int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend) {
    int i;
    int race_idx;

    gFrontend_net_current_roll = PDGetTotalTime();
    if ((int)(gFrontend_net_current_roll - gFrontend_net_last_roll) < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    /* ScrollUp(pFrontend, 1), inlined in the original; the volatile read keeps MSVC
       from reusing the comparison's copy of indexFirstScrollableItem (it re-loads it) */
    if (pFrontend->scrollers[1].indexTopItem > *(volatile int*)&pFrontend->scrollers[1].indexFirstScrollableItem) {
        pFrontend->scrollers[1].indexTopItem -= 1;
    }
    /* RefreshNetRacesScroller(pFrontend), inlined in the original */
    race_idx = pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem;
    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; race_idx++, i++) {
        strcpy(pFrontend->items[i].text, gRace_list[race_idx].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// ScrollDn and RefreshNetRacesScroller are inlined in the original.
// FUNCTION: CARMA2_HW 0x00466da0
int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend) {
    int i;
    int race_idx;

    gFrontend_net_current_roll = PDGetTotalTime();
    if ((int)(gFrontend_net_current_roll - gFrontend_net_last_roll) < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    /* ScrollDn(pFrontend, 1), inlined in the original; the volatile read keeps MSVC
       from reusing the comparison's copy of indexFirstScrollableItem (it re-loads it) */
    if (pFrontend->scrollers[1].indexTopItem + pFrontend->scrollers[1].nbDisplayedAtOnce < *(volatile int*)&pFrontend->scrollers[1].indexFirstScrollableItem + pFrontend->scrollers[1].count) {
        pFrontend->scrollers[1].indexTopItem += 1;
    }
    /* RefreshNetRacesScroller(pFrontend), inlined in the original */
    race_idx = pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem;
    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; race_idx++, i++) {
        strcpy(pFrontend->items[i].text, gRace_list[race_idx].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
    return 0;
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00467ef0
int C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x00468260
int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend) {
    int i;
    int selected;

    NetEndJoinList();
    selected = gLast_graph_sel;
    for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
        if (i != selected && gGames_to_join[i].game != NULL) {
            NetDisposeGameDetails(gGames_to_join[i].game);
            gGames_to_join[i].game = NULL;
        }
    }
    return 0;
}

// ScrollDn of the join list and NetworkUpdateSelectedGameInfo are inlined in the original:
// the game-info text is built with sprintf into a local buffer and copied to items[30].
static void NetJoinDisposeGame(int netgame_idx) {
    NetDisposeGameDetails(gGames_to_join[netgame_idx].game);
    gGames_to_join[netgame_idx].game = NULL;
}

static void NetJoinUpdateSelectedGameInfo(tFrontend_spec* pFrontend) {
    int netgame_idx;

    netgame_idx = 0;
    if (gFrontend_selected_item_index >= pFrontend->scrollers[2].indexFirstScrollableItem
            && gFrontend_selected_item_index <= pFrontend->scrollers[2].indexLastScrollableItem) {
        netgame_idx = gFrontend_selected_item_index - pFrontend->scrollers[2].indexFirstScrollableItem;
    }
    if (gGames_to_join[netgame_idx].game != NULL
            && gGames_to_join[netgame_idx].game->type >= 0
            && gGames_to_join[netgame_idx].game->type <= 7) {
        size_t len;
        const char* status_str;
        const char* open_str;
        char s[256];

        if (PDGetTotalTime() - gGames_to_join[netgame_idx].time >= 15000) {
            NetJoinDisposeGame(netgame_idx);
            return;
        }
        len = 0;
        len += sprintf(s + len, "Game: %s @R", gGames_to_join[netgame_idx].game->pd_net_info.addr + 14);
        len += sprintf(s + len, "Type: %s @R", GetMiscString(eMiscString_network_type_start + gGames_to_join[netgame_idx].game->type));
        len += sprintf(s + len, "Players: %d @R", gGames_to_join[netgame_idx].game->num_players);
        status_str = GetMiscString(eMiscString_netgame_stage_start + gGames_to_join[netgame_idx].game->status.stage);
        open_str = GetMiscString(eMiscString_closed + gGames_to_join[netgame_idx].game->options.open_game);
        sprintf(s + len, "Status: %s, %s ", status_str, open_str);
        sprintf(pFrontend->items[30].text, "%s", s);
        MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    }
}

// FUNCTION: CARMA2_HW 0x004682a0
int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend) {
    extern tNet_mode gFrontend_net_mode;

    (void)pFrontend;
    if (gFrontend_net_mode == eNet_mode_thinking_about_it) {
        gFrontend_net_mode = eNet_mode_host;
    } else {
        gFrontend_net_mode = eNet_mode_thinking_about_it;
    }
    WaitFor(6000);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004682c0
int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend) {
    int sel;

    sel = gFrontend_selected_item_index - pFrontend->scrollers[2].indexFirstScrollableItem;
    if (sel < 0 || gGames_to_join[sel].game == NULL || (!gGames_to_join[sel].game->options.open_game && !gGames_to_join[sel].game->no_races_yet) || gGames_to_join[sel].game->num_players >= kMax_netplayers) {
        gLast_graph_sel = -1;
    } else {
        gLast_graph_sel = sel;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    NetJoinUpdateSelectedGameInfo(pFrontend);
    return 0;
}

// GLOBAL: CARMA2_HW 0x00763714
tNet_game_details* gGame_to_join;

// GLOBAL: CARMA2_HW 0x007638e0
int gFrontend_net_car_index;

// GLOBAL: CARMA2_HW 0x00763600
char gFrontend_host_join_buffer[256];

// GLOBAL: CARMA2_HW 0x007638a8
const char* gNet_has_joined_text;

// FUNCTION: CARMA2_HW 0x00468460
int C2_HOOK_CDECL NetworkJoinGoAhead(void) {
    int i;
    int selected;
    tNet_game_details* game;

    ClearAlwaysTyping();
    if (gLast_graph_sel >= 0) {
        NetEndJoinList();
        selected = gLast_graph_sel;
        for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
            if (i != selected && gGames_to_join[i].game != NULL) {
                NetDisposeGameDetails(gGames_to_join[i].game);
                gGames_to_join[i].game = NULL;
            }
        }
        SaveOptions();
        gGame_to_join = gGames_to_join[gLast_graph_sel].game;
        if (gFrontend_net_mode == eNet_mode_thinking_about_it) {
            if (gGame_to_join != NULL) {
                SolidPolyFontText("Joining a game", 200, 100, kPolyfont_hand_green_15pt_lit, eJust_left, 1);
                gFrontend_net_car_index = -1;
                do {
                    gNet_join_host_result = 0;
                    for (i = 0; i < kMax_netplayers; i++) {
                        gCurrent_race.opponent_list[i].car_spec = NULL;
                    }
                    gNet_storage_space_initialized = 1;
                    InitialiseStorageSpace(0, &gNet_cars_storage_space, 2000, 50, 2000, 2000, 50);
                    gNet_join_host_result = NetJoinGame(gGame_to_join, gProgram_state.player_name, gFrontend_net_car_index);
                    if (gNet_join_host_result == 0) {
                        LoadRaces(gRace_list, &gNumber_of_races, gCurrent_net_game->type);
                        game = gGame_to_join;
                        gProgram_state.skill_level = 1;
                        InitGame(game->start_race);
                        ReenableNetService();
                        gNet_has_joined_text = GetMiscString(eMiscString_has_joined_the_game);
                        strcpy(gFrontend_host_join_buffer, gProgram_state.player_name);
                        strcat(gFrontend_host_join_buffer, " ");
                        strcat(gFrontend_host_join_buffer, gNet_has_joined_text);
                        NetSendHeadupToAllPlayers(gFrontend_host_join_buffer);
                        gNet_join_host_result = 1;
                    } else {
                        if (gNet_storage_space_initialized != 0) {
                            DisposeStorageSpace(&gNet_cars_storage_space);
                        }
                        gNet_storage_space_initialized = 0;
                    }
                } while (gNet_join_host_result == -4);
                if (gNet_join_host_result > 0) {
                    goto joined;
                }
                NetDisposeGameDetails(gGame_to_join);
            }
        } else {
            gFrontend_net_car_index = -1;
            gRace_index = gProgram_state.current_race_index;
            gChosen_net_game_options = &gFrontend_net_options;
            SetNetAvailability(&gFrontend_net_options);
            gFrontend_net_car_index = PickARandomCar();
            if (gFrontend_net_car_index >= 0) {
                gCar_details[gFrontend_net_car_index].ownership = eCar_owner_someone;
            }
            InitNetStorageSpace();
            if (NetHostGame(gFrontend_game_type, &gFrontend_net_options, gRace_index, gProgram_state.player_name, gFrontend_net_car_index) != 0) {
                game = gCurrent_net_game;
                gProgram_state.skill_level = 1;
                InitGame(game->start_race);
                ReenableNetService();
            joined:
                return 1;
            }
            if (gNet_storage_space_initialized != 0) {
                DisposeStorageSpace(&gNet_cars_storage_space);
            }
            gNet_storage_space_initialized = 0;
            ReenableNetService();
            NetLeaveGame(gCurrent_net_game);
        }
        for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
            gGames_to_join[i].game = NULL;
        }
        gLast_graph_sel = -1;
        NetStartProducingJoinList(AddToJoinList);
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00468750
void C2_HOOK_FASTCALL InitNetStorageSpace(void) {
    int i;

    for (i = 0; i < kMax_netplayers; i++) {
        gCurrent_race.opponent_list[i].car_spec = NULL;
    }
    gNet_storage_space_initialized = 1;
    InitialiseStorageSpace(0, &gNet_cars_storage_space, 2000, 50, 2000, 2000, 50);
}

// SetOptions / ReadNetGameChoices / UpdateNetTrackScroller / UpdateNetGameTypeScroller
// are all inlined in the original (only plain callees are visible in the disassembly).
// FUNCTION: CARMA2_HW 0x00468790
int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend) {
    tFrontend_scroller_spec* scroller;
    const tRace_list_spec* race_list_item;
    int i;
    int text_idx;
    tNet_game_type game_type;

    game_type = pFrontend->scrollers[0].indexTopItem - pFrontend->scrollers[0].indexFirstScrollableItem;
    switch (gFrontend_selected_item_index) {
    case 3:
        break;
    case 4:
        game_type += 1;
        break;
    case 5:
        game_type += 2;
        break;
    case 6:
        game_type += 3;
        break;
    }
    if (game_type >= 2) {
        game_type += 1;
    }
    gFrontend_game_type = game_type;
    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    gNet_last_game_type = gFrontend_game_type;
    SaveOptions();
    /* SetOptions(&gFrontend_game_type, &gFrontend_net_options), inlined (SetGameTarget is empty) */
    gFrontend_net_options.show_players_on_map = gNet_settings[0].show_players_on_map;
    gFrontend_net_options.enable_text_messages = gNet_settings[0].enable_text_messages;
    gFrontend_net_options.show_powerups_on_map = gNet_settings[0].show_powerups_on_map;
    gFrontend_net_options.powerup_respawn = gNet_settings[0].powerup_respawn;
    gFrontend_net_options.open_game = gNet_settings[gFrontend_game_type + 1].open_game;
    gFrontend_net_options.starting_credits = gNet_settings[0].starting_credits;
    gFrontend_net_options.grid_start = gNet_settings[gFrontend_game_type + 1].grid_start || gNet_grid_starts[gFrontend_game_type + 1];
    gFrontend_net_options.waste_to_transfer = gNet_settings[gFrontend_game_type + 1].waste_to_transfer;
    gFrontend_net_options.random_car_choice = gNet_settings[gFrontend_game_type + 1].random_car_choice;
    gFrontend_net_options.race_sequence_type = gNet_settings[0].race_sequence_type;
    gFrontend_net_options.car_choice = gNet_settings[0].car_choice;
    gFrontend_net_options.starting_target = gNet_settings[gFrontend_game_type + 1].starting_target;
    /* the PickNetRace call of ReadNetGameChoices (the final inlined part) */
    gRace_index = PickNetRace(-1, gFrontend_net_options.race_sequence_type);
    LoadRaces(gRace_list, &gNumber_of_races, gFrontend_game_type);
    sprintf(pFrontend->items[30].text, "%s", GetMiscString(eMiscString_network_type_description_start + gFrontend_game_type));
    MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    scroller = &pFrontend->scrollers[1];
    scroller->indexTopItem = scroller->indexFirstScrollableItem;
    scroller->count = gNumber_of_races;
    /* UpdateNetTrackScroller(pFrontend), inlined */
    race_list_item = &gRace_list[scroller->indexTopItem - scroller->indexFirstScrollableItem];
    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; race_list_item++, i++) {
        strcpy(pFrontend->items[i].text, race_list_item->name);
    }
    /* UpdateNetGameTypeScroller(pFrontend), inlined */
    scroller = &pFrontend->scrollers[0];
    text_idx = scroller->indexTopItem - scroller->indexFirstScrollableItem;
    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; text_idx++, i++) {
        int text_num;

        text_num = text_idx >= 2 ? text_idx + 1 : text_idx;
        strcpy(pFrontend->items[i].text, GetMiscString(eMiscString_network_type_start + text_num));
    }
    FuckWithWidths(pFrontend);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// STUB: CARMA2_HW 0x00468a50
int C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00468d70
int C2_HOOK_FASTCALL ChooseNetCar(tNet_game_details* pNet_game, tNet_game_options* pOptions, int* pCar_index, int pIm_the_host_so_fuck_off) {
    int cars[120];
    int i;
    int array_size;

    C2_HOOK_BUG_ON(REC2_ASIZE(cars) != 120);

    if (!pIm_the_host_so_fuck_off) {
        return 1;
    }
    gChosen_net_game_options = pOptions;
    for (i = 0; i < gNumber_of_racers; i++) {
        switch (gOpponents[i].network_availability) {
        case eNet_avail_never:
            gCar_details[i].ownership = eCar_owner_not_allowed;
            break;
        case eNet_avail_eagle:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_eagle) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        case eNet_avail_all:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_novelty) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        default:
            break;
        }
    }
    array_size = 0;
    for (i = 0; i < gNumber_of_racers; i++) {
        if (gCar_details[i].ownership == eCar_owner_none) {
            cars[array_size] = i;
            array_size++;
            if (gOpponents[i].network_availability == eNet_avail_all) {
                cars[array_size] = i;
                array_size++;
            }
        }
    }
    i = cars[IRandomBetween(0, array_size - 1)];
    *pCar_index = i;
    if (i >= 0) {
        gCar_details[i].ownership = eCar_owner_someone;
    }
    return 1;
}

// STUB: CARMA2_HW 0x00468e80
int C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// RaceIndex and FillInRaceDescription are inlined in the original (the inlined loop calls
// DRStricmp, and the description copy calls the two MungeMetaCharacters helpers).
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
static int NetHostRaceIndex(const char* pName) {
    int i;
    int index;

    for (i = 0; i < gNumber_of_races; i++) {
        if (DRStricmp(pName, gRace_list[i].name) == 0) {
            index = i;
            break;
        }
    }
    return index;
}

// FUNCTION: CARMA2_HW 0x004691a0
int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend) {
    int race_index;

    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    race_index = NetHostRaceIndex(pFrontend->items[gFrontend_selected_item_index].text);
    gProgram_state.current_race_index = race_index;
    strcpy(pFrontend->items[30].text, gRace_list[race_index].description);
    MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    MungeMetaCharactersNum(pFrontend->items[30].text, 'O', gRace_list[race_index].count_explicit_opponents);
    MungeMetaCharactersNum(pFrontend->items[30].text, 'L', gRace_list[race_index].count_laps);
    FuckWithWidths(pFrontend);
    return 0;
}


// GLOBAL: CARMA2_HW 0x005b39b8
tFrontend_spec gFrontend_NETWORK = {
    "Network",
    0,
    31,
    NetworkJoinMenuInfunc,
    NetworkJoinMenuOutfunc,
    FRONTEND_NetworkJoinMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    3,
    3,
    2,
    {
        { 0x98, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa3, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403, NetGameTypeUp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetSetRaceType, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetSetRaceType, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetSetRaceType, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetSetRaceType, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, NetGameTypeDn, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe4, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403, NetRaceUp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetHostChooseThisRace, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetHostChooseThisRace, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetHostChooseThisRace, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetHostChooseThisRace, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, NetRaceDn, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe5, temp, &gFrontend_NETWORK_OPTIONS, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 8, NetCancel, &gFrontend_MAIN, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9a, NetworkStartHost, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x97, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetJoinChooseThisGame, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetJoinChooseThisGame, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetJoinChooseThisGame, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, NetJoinChooseThisGame, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, NetGameToggleTyping, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf, temp, &gFrontend_OPTIONS, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x99, NetworkStartJoin, NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
    }
};

// FUNCTION: CARMA2_HW 0x00469280
int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend) {
    int i;
    int selected;

    NetEndJoinList();
    selected = gLast_graph_sel;
    for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
        if (i != selected && gGames_to_join[i].game != NULL) {
            NetDisposeGameDetails(gGames_to_join[i].game);
            gGames_to_join[i].game = NULL;
        }
    }
    ShutdownNetIfRequired();
    gNet_mode = eNet_mode_none;
    LoadRaces(gRace_list, &gNumber_of_races, -1);
    InitGame(gDev_initial_race);
    /* MaybeRestoreSavedGame / RestoreSinglePlayerState, inlined in the original */
    if (gValid_stashed_save_game) {
        LoadRaces(gRace_list, &gNumber_of_races, -1);
        DoLoadGame2(&gStashed_save_game);
        gValid_stashed_save_game = 0;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00469320
int C2_HOOK_FASTCALL FRONTEND_NetworkJoinMenuHandler(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0049e210
tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index) {

    (void)pNet_type;
    (void)pOptions;
    (void)pRace_index;
    (void)pHost_name;
    (void)pCar_index;
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
/* NetJoinGame / NetHostGame live only in the dead src/carma2/common tree (not built);
   inert stubs so the caller links, guarded so they are not inlined away. */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049e970
int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details* pGame_details, char* pPlayer_name, int pCar_index) {

    (void)pGame_details;
    (void)pPlayer_name;
    (void)pCar_index;
    return -1;
}
