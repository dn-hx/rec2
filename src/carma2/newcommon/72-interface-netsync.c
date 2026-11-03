#include "72-interface-netsync.h"

#include <string.h>

#include "72-interface.h"

extern tFrontend_spec gFrontend_MAIN;
extern int C2_HOOK_FASTCALL NetSync_Start(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tNet_mode gNet_mode;
extern int gNumber_of_net_players;
extern int gThis_net_player_index;
extern tNet_game_player_info gNet_players[12];
extern int gNo_races_yet;
extern tNet_game_details* gCurrent_net_game;
extern tU32 gLast_host_query;
extern tU32 gNet_synch_start;
extern int gFrontend_scrollbars_updated;
extern tFrontend_slider* gCurrent_frontend_scrollbars;
extern tConnected_items* gConnected_items;
extern tStruct_00686508* gPTR_00686508;
extern tNet_mode gFrontend_net_mode;
extern void C2_HOOK_FASTCALL KillSplashScreen(void);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL GetMousePosition(int* pX, int* pY);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern void C2_HOOK_FASTCALL ServiceGame(void);
extern void C2_HOOK_FASTCALL SignalToStartRace(void);
extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);
extern void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY);
extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern tFrontend_spec gFrontend_QUIT;
extern int gTyping_slot;
extern int gMouse_in_use;
extern tU32 gFrontend_time_last_input;
extern int gFrontend_leave_current_menu;
extern int gStart_race_sent;
extern tFrontend_spec* gCurrent_frontend_spec;
extern int gFrontend_selected_item_index;
extern tProgram_state gProgram_state;

static int C2_HOOK_FASTCALL FindNextActiveItemFrontend(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index + 1; i <= pFrontend->count_items - 1; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        if (item->enabled > 0 && item->visible) {
            return i;
        }
    }
    return 0;
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00473610
int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
    int i;
    tU32 time;
    tNet_message* message;

    /* DefaultInfunc inlined */
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    gCurrent_frontend_scrollbars = NULL;
    gConnected_items = NULL;
    gPTR_00686508 = NULL;
    time = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && time - gNet_players[i].last_heard_from_him > 60000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
    if (gNet_mode == eNet_mode_client && gLast_host_query == 0) {
        message = NetBuildGuaranteedMessage(0xe, 0);
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
        gLast_host_query = time;
    }
    if (gFrontend_net_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            if (gCurrent_net_game->status.stage == eNet_game_stage0) {
                gCurrent_net_game->status.stage = 1;
            }
            SetUpNetCarPositions();
            gNet_synch_start = PDGetTotalTime();
        } else {
            if (gCurrent_net_game->status.stage == eNet_game_stage0) {
                gCurrent_net_game->status.stage = 1;
            }
            SetUpNetCarPositions();
            gNet_synch_start = PDGetTotalTime();
        }
    }
    for (i = 0; i < 12; i++) {
        pFrontend->items[2 + i].x = 160;
    }
    if (gCurrent_net_game->options.open_game) {
        sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_open));
    } else {
        sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_closed));
    }
    if (gNet_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[26].visible = 1;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[26].stringId = 14;
            pFrontend->items[27].stringId = 21;
            FuckWithWidths(pFrontend);
            return 0;
        }
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].visible = 0;
        pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[27].visible = 1;
        pFrontend->items[27].stringId = 21;
        FuckWithWidths(pFrontend);
        return 0;
    }
    pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
    pFrontend->items[26].visible = 0;
    pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[27].visible = 1;
    pFrontend->items[27].stringId = 179;
    FuckWithWidths(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00473870
int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend) {
    int key;
    int mouse_down;
    int mouse_item;
    int original;

    if (gProgram_state.prog_status == eProg_idling) {
        return 1;
    }
    original = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    PollKeys();
    EdgeTriggerModeOff();
    key = PDAnyKeyDown();
    if (key != -1 && key != 4) {
        gMouse_in_use = 0;
        gFrontend_time_last_input = PDGetTotalTime();
    }
    EdgeTriggerModeOn();
    mouse_down = 0;
    if (gMouse_in_use) {
        int mouse_x;
        int mouse_y;

        gFrontend_time_last_input = PDGetTotalTime();
        GetMousePosition(&mouse_x, &mouse_y);
        mouse_down = EitherMouseButtonDown();
        mouse_item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
        gFrontend_selected_item_index = mouse_item;
        if (mouse_item == -1) {
            gFrontend_selected_item_index = 99;
        }
    }
    if (PDKeyDown(72) || PDKeyDown(89)) {
        tFrontend_spec* cur;
        int next;

        next = gFrontend_selected_item_index - 1;
        cur = gCurrent_frontend_spec;
        for (; next >= 0; next--) {
            if (cur->items[next].enabled > 0 && cur->items[next].visible) {
                goto found_prev;
            }
        }
        next = FindPrevActiveItem(cur, cur->count_items);
found_prev:
        gFrontend_selected_item_index = next;
        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
    }
    if (PDKeyDown(73) || PDKeyDown(83)) {
        tFrontend_spec* cur;

        if (gFrontend_selected_item_index < pFrontend->count_items - 1) {
            cur = gCurrent_frontend_spec;
            gFrontend_selected_item_index = FindNextActiveItemFrontend(cur, gFrontend_selected_item_index);
        } else {
            gFrontend_selected_item_index = -1;
            cur = gCurrent_frontend_spec;
            gFrontend_selected_item_index = FindNextActiveItemFrontend(cur, gFrontend_selected_item_index);
        }
        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
    }
    if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
        int result;
        int i;

        result = pFrontend->items[original].field_0xc;
        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
            return 2;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
        }
        return result;
    }

    ServiceGame();
    /* MungePlayers inlined */
    {
        int count_ready;
        int i;
        tU32 time;

        count_ready = 0;
        /* CheckPlayersAreResponding inlined */
        time = PDGetTotalTime();
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (i != gThis_net_player_index && time - gNet_players[i].last_heard_from_him > 60000) {
                gNet_players[i].player_status = ePlayer_status_not_responding;
            }
        }
        if (gNet_mode == eNet_mode_client && gLast_host_query == 0) {
            tNet_message* message;

            message = NetBuildGuaranteedMessage(0xe, 0);
            NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
            gLast_host_query = time;
        }
        NetPlayerStatusChanged(ePlayer_status_ready);
        for (i = 0; i < gNumber_of_net_players; i++) {
            pFrontend->items[2 + i].visible = 1;
            pFrontend->items[14 + i].visible = 1;
            pFrontend->items[2 + i].enabled = kFrontendItemEnabled_default;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
            if (gNet_players[i].host) {
                sprintf(pFrontend->items[2 + i].text, "%s (%s)", gNet_players[i].player_name, IString_Get(154));
            } else {
                strcpy(pFrontend->items[2 + i].text, gNet_players[i].player_name);
            }
            if (gNet_players[i].player_status == ePlayer_status_ready) {
                count_ready += 1;
            }
        }
        for (i = gNumber_of_net_players; i < 12; i++) {
            pFrontend->items[2 + i].visible = 0;
            pFrontend->items[14 + i].visible = 0;
            pFrontend->items[2 + i].enabled = kFrontendItemEnabled_default;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
        }
        if (gNet_mode == eNet_mode_host
                && count_ready == gNumber_of_net_players
                && gNumber_of_net_players > 1
                && (!gNo_races_yet || gNumber_of_net_players == 12)) {
            SignalToStartRace();
            gStart_race_sent = 1;
        }
    }
    /* NetSync_Draw inlined */
    {
        char s[128];
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {
            DrawThisCarIconNow(gNet_players[i].car_index, 115, 99 + 26 * i);
            SolidPolyFontText(GetMiscString(eMiscString_player_status_start + gNet_players[i].player_status),
                520, 96 + 26 * i, pFrontend->items[2 + i].unlitFont, eJust_right, 1);
        }
        if (gNet_mode != eNet_mode_host) {
            sprintf(s, "...%s...", IString_Get(250));
            SolidPolyFontText(s, 320, 96 + 26 * gNumber_of_net_players, pFrontend->items[2].unlitFont, eJust_centre, 1);
        }
    }
    FuckWithWidths(pFrontend);
    if (gFrontend_leave_current_menu || gStart_race_sent || gProgram_state.prog_status == eProg_game_starting) {
        return 1;
    } else {
        return 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0061bad0
tFrontend_spec gFrontend_NETSYNC = {
    "NetSync",
    0,
    28,
    NetSync_Infunc,
    0,
    NetSync_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    6,
    0,
    {
        { 0xab, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe, NetSync_Start, NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15, NetSync_Abort, NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
};

// GLOBAL: CARMA2_HW 0x0059c820
tNet_mode gFrontend_net_mode = eNet_mode_thinking_about_it;
