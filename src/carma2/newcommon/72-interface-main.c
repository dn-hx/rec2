#include "72-interface-main.h"

#include "72-interface.h"

extern tFrontend_spec gFrontend_OPTIONS;
extern tFrontend_spec gFrontend_NETWORK;
extern tFrontend_spec gFrontend_NEWGAME;
extern tFrontend_spec gFrontend_CHANGE_CAR;
int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend);

// GLOBAL: CARMA2_HW 0x005a80f0
tFrontend_spec gFrontend_MAIN = {
    "Main",
    0,
    26,
    MainMenuInfunc,
    MainMenuOutfunc,
    FRONTEND_MainMenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    2,
    2,
    1,
    {
        { 0x9,      OnePlayerSetup,      NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa,      MultiplayerSetup,    &gFrontend_NETWORK,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    testUp,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x403,    testDn,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf,      temp,                &gFrontend_OPTIONS,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NewGameToggleTyping, NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xec,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      StartFudge,          &gFrontend_NEWGAME,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf7,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
    },
    1,
    {
        { 1, 8, 6, 12, 12, 19 },
    }
};


/* FRONTEND_Main (based on carpocalypse2 src/carma2/common/interface.c:2387, FrontEndProcessMenus) lives here, not in 72-interface.c:
   additions to 72-interface.c shift MSVC's codegen of SortScores 0x004743c0 */

#include "02-init.h"
#include "07-structur.h"
#include "18-graphics2.h"
#include "41-utility.h"
#include "42-input.h"
#include "46-win95sys.h"
#include "66-tintedpoly.h"
#include "69-sound.h"
#include "72-interface-netsync.h"
#include "72-interface-networksummary.h"
#include "72-interface-startgame.h"
#include "72-interface-wrecks.h"
#include "19-font.h"
#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "73-istring.h"
#include "72-interface-newgame.h"
#include "72-interface-options.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "c2_string.h"

extern br_pixelmap* gFrontend_images[100];
extern tFrontend_model gFrontend_A_models[13];
extern tFrontend_model gFrontend_B_models[11];
extern tFrontend_model gFrontend_C_models[6];
extern br_actor* gFrontend_backdrop_actors[3];
extern br_actor* gFrontend_billboard_actors[8];
extern int gFrontend_remove_current_backdrop;
extern int gFrontend_leave_current_menu;
extern int gFrontend_interpolate_steps_left;
extern int gFrontend_suppress_mouse;
extern br_pixelmap* gFrontend_backdrop;
extern br_actor* gFrontend_actor;
extern br_actor* gFrontend_menu_camera;
extern br_model* gFrontend_A_model_from;
extern br_model* gFrontend_A_model_to;
extern br_model* gFrontend_B_model_from;
extern br_model* gFrontend_B_model_to;
extern br_model* gFrontend_C_model_from;
extern br_model* gFrontend_C_model_to;
extern int gFrontend_backdrop0_opacity_mode;
extern int gFrontend_backdrop1_opacity_mode;
extern int gFrontend_backdrop2_opacity_mode;
extern br_pixelmap* gFrontend_wrecks_pixelmap;
extern tFrontendMenuType gFrontend_next_menu;
extern tS3_outlet* gEffects_outlet;
extern tU32 gFrontend_time_last_input;
extern br_uint_32 gFrontend_APO_Colour_1;
extern br_uint_32 gFrontend_APO_Colour_2;
extern br_uint_32 gFrontend_APO_Colour_3;
extern void C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h);

// GLOBAL: CARMA2_HW 0x005ed5a0
tFrontend_spec gFrontend_LOAD_GAME = {
    0
};

// GLOBAL: CARMA2_HW 0x005cab48
tFrontend_spec gFrontend_CREDITS = {
    0
};

// GLOBAL: CARMA2_HW 0x0059b0d0
int gFrontend_stuff_not_loaded = 1;

// GLOBAL: CARMA2_HW 0x0076370c
static int back_screen_base_x;
// GLOBAL: CARMA2_HW 0x00763710
static int back_screen_base_y;
// GLOBAL: CARMA2_HW 0x00763704
static int back_screen_origin_x;
// GLOBAL: CARMA2_HW 0x00763708
static int back_screen_origin_y;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0046d1c0
void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pFrontend_type) {
    /* based on carpocalypse2 src/carma2/common/interface.c:2298 (LoadMenuImages/LoadMenuModels inlined) */
    char s[256];
    char s2[256];
    char s3[256];
    FILE* f;
    FILE* f2;
    int count;
    int i;
    br_model* model;
    br_pixelmap** p;
    tFrontendMenuType type;

    type = pFrontend_type;
    PrintMemoryDump(0, "START OF FRONTEND_Setup");
    LoadInterfaceStuff(gProgram_state.racing);
    if (gFrontend_stuff_not_loaded) {
        IString_Load();
        InitPolyFonts();
        for (i = 1; i < 8; i++) {
            gFrontend_images[i] = LoadPixelmap("64by64.tif");
        }
        PathCat(s, gApplication_path, "INTRFACE");
        PathCat(s, s, "MENUIMAGES.TXT");
        f = DRfopen(s, "rt");
        if (f == NULL) {
            FatalError(kFatalError_Mysterious_SS);
        }
        count = GetAnInt(f);
        if (count > 0) {
            p = &gFrontend_images[8];
            do {
                GetAString(f, s2);
                strcpy(s, s2);
                strcat(s, ".TIF");
                *p = LoadPixelmap(s);
                p++;
            } while (--count != 0);
        }
        PrintMemoryDump(0, "AFTER LoadMenuImages");
        FRONTEND_Setup2D();
        PathCat(s2, gApplication_path, "INTRFACE");
        PathCat(s, s2, "MENUMODELS.TXT");
        f2 = DRfopen(s, "rt");
        if (f2 == NULL) {
            FatalError(kFatalError_Mysterious_SS);
        }
        PathCat(s2, gApplication_path, "INTRFACE");
    count = GetAnInt(f2);
    for (i = 0; i < count; i++) {
        GetAString(f2, s3);
        PathCat(s, s2, s3);
        strcat(s, ".DAT");
        model = BrModelLoad(s);
        gFrontend_A_models[i].model = model;
        model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(model);
        if (i == 0) {
            gFrontend_backdrop_actors[0]->model = BrModelLoad(s);
            gFrontend_backdrop_actors[0]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[0]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[0]->t.t.mat, 300.0f, -250.0f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[0]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[0]->render_style = BR_RSTYLE_FACES;
        }
    }
    count = GetAnInt(f2);
    for (i = 0; i < count; i++) {
        GetAString(f2, s3);
        PathCat(s, s2, s3);
        strcat(s, ".DAT");
        model = BrModelLoad(s);
        gFrontend_B_models[i].model = model;
        model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(model);
        if (i == 0) {
            gFrontend_backdrop_actors[1]->model = BrModelLoad(s);
            gFrontend_backdrop_actors[1]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[1]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[1]->t.t.mat, 300.0f, -250.0f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[1]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[1]->render_style = BR_RSTYLE_FACES;
        }
    }
    count = GetAnInt(f2);
    for (i = 0; i < count; i++) {
        GetAString(f2, s3);
        PathCat(s, s2, s3);
        strcat(s, ".DAT");
        model = BrModelLoad(s);
        gFrontend_C_models[i].model = model;
        model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(model);
        if (i == 0) {
            gFrontend_backdrop_actors[2]->model = BrModelLoad(s);
            gFrontend_backdrop_actors[2]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[2]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[2]->t.t.mat, 300.0f, -250.0f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[2]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[2]->render_style = BR_RSTYLE_FACES;
        }
    }
        PrintMemoryDump(0, "AFTER LoadMenuModels");
        pFrontend_type = type;
        gFrontend_stuff_not_loaded = 0;
    }
    switch (pFrontend_type) {
    case kFrontend_menu_main:
        FRONTEND_CreateMenu(&gFrontend_MAIN);
        gCurrent_frontend_spec = &gFrontend_MAIN;
        break;
    case kFrontend_menu_options:
        FRONTEND_CreateMenu(&gFrontend_OPTIONS);
        gCurrent_frontend_spec = &gFrontend_OPTIONS;
        break;
    case kFrontend_menu_wrecks:
        FRONTEND_CreateMenu(&gFrontend_WRECKS);
        gCurrent_frontend_spec = &gFrontend_WRECKS;
        break;
    case kFrontend_menu_netsync:
        FRONTEND_CreateMenu(&gFrontend_NETSYNC);
        gCurrent_frontend_spec = &gFrontend_NETSYNC;
        break;
    case kFrontend_menu_networksummary:
        FRONTEND_CreateMenu(&gFrontend_NETWORK_SUMMARY);
        gCurrent_frontend_spec = &gFrontend_NETWORK_SUMMARY;
        break;
    case kFrontend_menu_credits:
        FRONTEND_CreateMenu(&gFrontend_CREDITS);
        gCurrent_frontend_spec = &gFrontend_CREDITS;
        break;
    case kFrontend_menu_newgame:
        FRONTEND_CreateMenu(&gFrontend_NEWGAME);
        gCurrent_frontend_spec = &gFrontend_NEWGAME;
        break;
    }
    PrintMemoryDump(0, "AFTER FRONTEND_CreateMenu");
    gCurrent_frontend_spec->previous = NULL;
    gFrontend_A_model_from = gFrontend_A_models[0].model;
    gFrontend_A_model_to = gFrontend_A_models[gCurrent_frontend_spec->model_A_index].model;
    gFrontend_B_model_from = gFrontend_B_models[0].model;
    gFrontend_B_model_to = gFrontend_B_models[gCurrent_frontend_spec->model_B_index].model;
    gFrontend_C_model_from = gFrontend_C_models[0].model;
    gFrontend_C_model_to = gFrontend_C_models[gCurrent_frontend_spec->model_C_index].model;
    gFrontend_backdrop0_opacity_mode = gCurrent_frontend_spec->model_A_index != 0 ? 1 : -2;
    gFrontend_backdrop1_opacity_mode = gCurrent_frontend_spec->model_B_index != 0 ? 1 : -2;
    gFrontend_backdrop2_opacity_mode = gCurrent_frontend_spec->model_C_index != 0 ? 1 : -1;
    gFrontend_interpolate_steps_left = 16;
    PrintMemoryDump(0, "END OF FRONTEND_Setup");
}

extern int gTyping_slot;
extern int gMouse_in_use;
extern void C2_HOOK_FASTCALL ServiceGame(void);
extern tFrontend_spec gFrontend_QUIT;

/* based on carpocalypse2 src/carma2/common/interface.c:2569 */
// FUNCTION: CARMA2_HW 0x0046c0d0
int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend) {
    tFrontend_spec* cur;
    int original_selected_index;
    int the_key;
    int item_under_mouse;
    int mouse_button;
    int x;
    int y;
    int i;
    int next;
    tFrontend_item_spec* item;

    original_selected_index = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (pFrontend->tick != NULL) {
        int ret = pFrontend->tick(pFrontend);
        if (ret != 0) {
            return ret;
        }
    } else {
        PollKeys();
        EdgeTriggerModeOff();
        the_key = PDAnyKeyDown();
        if (the_key != -1 && the_key != 4) {
            gMouse_in_use = 0;
            gFrontend_time_last_input = PDGetTotalTime();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        if (gMouse_in_use) {
            gFrontend_time_last_input = PDGetTotalTime();
            GetMousePosition(&x, &y);
            mouse_button = EitherMouseButtonDown();
            item_under_mouse = GetItemAtMousePos(gCurrent_frontend_spec, x, y);
            if (item_under_mouse != -1) {
                gFrontend_selected_item_index = item_under_mouse;
            } else {
                gFrontend_selected_item_index = 99;
            }
        }

        if (PDKeyDown(72) || PDKeyDown(89)) {
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
            if (gFrontend_selected_item_index < pFrontend->count_items - 1) {
                cur = gCurrent_frontend_spec;
                next = gFrontend_selected_item_index + 1;
                for (; next <= cur->count_items - 1; next++) {
                    if (cur->items[next].enabled > 0 && cur->items[next].visible) {
                        goto found_next;
                    }
                }
                next = 0;
            } else {
                gFrontend_selected_item_index = -1;
                cur = gCurrent_frontend_spec;
                for (i = 0; i <= cur->count_items - 1; i++) {
                    if (cur->items[i].enabled > 0 && cur->items[i].visible) {
                        next = i;
                        goto found_next;
                    }
                }
                next = 0;
            }
        found_next:
            gFrontend_selected_item_index = next;
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= cur->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= cur->scrollers[0].indexLastScrollableItem) {
                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(63)) {
            if (pFrontend->previous == NULL) {
                return 2;
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->previous;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return 0;
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button == 1 && item_under_mouse != -1)) {
            ToggleSelection(pFrontend);
            item = &pFrontend->items[gFrontend_selected_item_index];
            if (item->field_0xc == 2) {
                return item->field_0xc;
            }
            if (item->field_0xc == 1) {
                gFrontend_leave_current_menu = 1;
            }
            if (item->action != NULL) {
                item->action(pFrontend);
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
            return pFrontend->items[original_selected_index].field_0xc;
        }
    }
    ServiceGame();
    return gFrontend_leave_current_menu != 0;
}

extern tConnected_items* gConnected_items;
extern int gFrontend_count_brender_items;
extern double gFrontend_throb_factor;
extern int gCount_connected_items_indices;
extern int gConnected_items_indices[6];

// FUNCTION: CARMA2_HW 0x0046e020
void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) {
    /* based on carpocalypse2 src/carma2/common/interface.c:3509 (Generic_MungeActiveItems, FRONTEND_PingPongFlash inlined) */
    int i;

    if (gFrontend_leave_current_menu || pFrontend->count_items <= 0) {
        return;
    }
    gCount_connected_items_indices = 0;
    if (gFrontend_selected_item_index != -1) {
        tConnected_items* connected_items = gConnected_items;
        int scroll_area_containing_selected_item = 0;
        for (; connected_items != NULL; connected_items = connected_items->next) {
            for (i = 0; i < connected_items->count_ranges; i++) {
                if (gFrontend_selected_item_index >= connected_items->range_starts[i]
                        && gFrontend_selected_item_index < connected_items->range_starts[i] + connected_items->range_length) {
                    scroll_area_containing_selected_item = gFrontend_selected_item_index - connected_items->range_starts[i] + 1;
                    break;
                }
            }
            if (scroll_area_containing_selected_item != 0) {
                break;
            }
        }
        if (scroll_area_containing_selected_item-- != 0) {
            for (i = 0; i < connected_items->count_ranges; i++) {
                gConnected_items_indices[gCount_connected_items_indices] = scroll_area_containing_selected_item + connected_items->range_starts[i];
                pFrontend->items[scroll_area_containing_selected_item + connected_items->range_starts[i]].flags |= 0x1;
                gCount_connected_items_indices += 1;
            }
        } else {
            gCount_connected_items_indices = 1;
            gConnected_items_indices[0] = gFrontend_selected_item_index;
            pFrontend->items[gFrontend_selected_item_index].flags |= 0x1;
        }
    }
    for (i = 0; i < gFrontend_count_brender_items; i++) {
        tFrontend_brender_item* brender_item = &gFrontend_brender_items[i];
        tFrontend_item_spec* item = &pFrontend->items[i];
        const char* text;

        if (!item->visible) {
            brender_item->actor->render_style = BR_RSTYLE_NONE;
            continue;
        }

        brender_item->actor->render_style = BR_RSTYLE_FACES;
        text = item->stringId >= 0x400 ? item->text : IString_Get(item->stringId);
        if (item->flags & 0x1) {
            if (text != NULL) {
                TransparentPolyFontTextInABox(item->unlitFont, text,
                    item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1, 0.6);
            }
            TransparentPolyFontTextInABox(item->highFont, text,
                item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1,
                item->radioButton_selected ? 0.3 - 0.7 * gFrontend_throb_factor : 0.6 * gFrontend_throb_factor);
        } else if (item->enabled >= 0) {
            if (item->radioButton_selected) {
                if (text != NULL) {
                    SolidPolyFontTextInABox(item->highFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_centre, 1);
                }
            } else {
                if (text != NULL) {
                    SolidPolyFontTextInABox(item->unlitFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1);
                }
            }
            brender_item->prims[1].v.x = 0x800000;
            BrMaterialUpdate(brender_item->material, BR_MATU_EXTRA_PRIM);
        } else {
            if (item->map_index && brender_item->prims[1].v.x != 0x400000) {
                brender_item->prims[1].v.x = 0x400000;
                BrMaterialUpdate(brender_item->material, BR_MATU_EXTRA_PRIM);
            }
            if (text != NULL) {
                TransparentPolyFontTextInABox(item->unlitFont, text,
                    item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1, 0.4);
            }
        }
    }
    {
    br_fixed_ls blend_x;
    blend_x = BR_FIXED_INT((int)(255. * (0.3 - 0.7 * gFrontend_throb_factor)));
    gFrontend_brender_items[gFrontend_selected_item_index].prims[1].v.x = blend_x;
    BrMaterialUpdate(gFrontend_brender_items[gFrontend_selected_item_index].material, BR_MATU_ALL);
    if (pFrontend->items[gFrontend_selected_item_index].glowDisabled != 0) {
        for (i = 0; i < gFrontend_count_brender_items; i++) {
            if (pFrontend->items[i].glowDisabled == pFrontend->items[gFrontend_selected_item_index].glowDisabled
                    && gFrontend_brender_items[i].prims[1].v.x != blend_x) {
                gFrontend_brender_items[i].prims[1].v.x = blend_x;
                BrMaterialUpdate(gFrontend_brender_items[i].material, BR_MATU_EXTRA_PRIM);
            }
        }
    }
    }
    {
    int time;
    time = (int)PDGetTotalTime() % 750;
    if (time < 375) {
        gFrontend_throb_factor = (double)time / (750.0 / 2.0);
    } else {
        gFrontend_throb_factor = (double)(750 - time) / (750.0 / 2.0);
    }
    }
    PossibleService();
    for (i = 0; i < gCount_connected_items_indices; i++) {
        pFrontend->items[gConnected_items_indices[i]].flags &= ~0x1;
    }
    gCount_connected_items_indices = 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* inlined into FRONTEND_Main in the original (BuildAPO) */
static void C2_HOOK_FASTCALL FrontendMainBuildAPO(int pCurrent, int pPotential, int pActor_index, int pAPO) {
    br_pixelmap* map;
    int x;
    int y;
    br_colour c;
    int i;

    map = gFrontend_billboard_actors[pActor_index]->material->colour_map;

    if (pAPO == 0) {
        BrPixelmapFill(map, 0);
    }

    for (i = 0; i < 3 * 10; i++) {

        x = 4 * (i % 10);
        y = 0 + 4 * (i / 10);

        if (i >= pPotential) {
            c = gFrontend_APO_Colour_2;
        } else if (i >= pCurrent) {
            c = gFrontend_APO_Colour_3;
        } else {
            c = gFrontend_APO_Colour_1;
        }

        BrPixelmapRectangleFill(map, x, y + 3 * 4 * pAPO, 3, 3, c);
    }
}

// FUNCTION: CARMA2_HW 0x0046d8e0
int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type) {
    int step;
    br_material* apo_material;
    br_model* apo_model;

    if (gSound_enabled) {
        DRS3StopAllOutletSoundsExceptCDA();
    }
    SwitchToHiresMode();
    WaitForNoKeys();
    gFrontend_remove_current_backdrop = 0;
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted3);
    FRONTEND_Setup(pFrontend_type);
    gFrontend_remove_current_backdrop = 1;
    gFrontend_leave_current_menu = 0;
    gFrontend_time_last_input = PDGetTotalTime();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    gCurrent_frontend_spec->unknownLastInt = 0;

    for (;;) {
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;
        if (gFrontend_backdrop != NULL) {
            FRONTEND_Redraw(gBack_screen, 0, 0, gFrontend_backdrop, 0, 0, gFrontend_backdrop->width, gFrontend_backdrop->height);
        }
        if (gCurrent_frontend_spec != &gFrontend_LOAD_GAME
                && gCurrent_frontend_spec != &gFrontend_NETSYNC
                && gCurrent_frontend_spec != &gFrontend_NETWORK_SUMMARY
                && gFrontend_interpolate_steps_left == 0) {
            gCurrent_frontend_spec->unknownLastInt = FRONTEND_GenericMenuHandler(gCurrent_frontend_spec);
        }
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;

        if (gFrontend_interpolate_steps_left > 0) {
            step = 16 - gFrontend_interpolate_steps_left + 1;

            MorphBlob(gFrontend_A_model_from, gFrontend_A_model_to, gFrontend_backdrop_actors[0]->model, step, 16);
            if (gFrontend_backdrop0_opacity_mode == -1) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step) / 16);
            } else if (gFrontend_backdrop0_opacity_mode == 1) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop0_opacity_mode == -2) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[0], BR_MATU_ALL);

            MorphBlob(gFrontend_B_model_from, gFrontend_B_model_to, gFrontend_backdrop_actors[1]->model, step, 16);
            if (gFrontend_backdrop1_opacity_mode == -1) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step) / 16);
            } else if (gFrontend_backdrop1_opacity_mode == 1) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop1_opacity_mode == -2) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[1], BR_MATU_ALL);

            MorphBlob(gFrontend_C_model_from, gFrontend_C_model_to, gFrontend_backdrop_actors[2]->model, step, 16);
            if (gFrontend_backdrop2_opacity_mode == -1) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step) / 16);
            } else if (gFrontend_backdrop2_opacity_mode == 1) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop2_opacity_mode == -2) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[2], BR_MATU_ALL);
        }
        BrZbsSceneRender(gFrontend_actor, gFrontend_camera, gBack_screen, gDepth_buffer);

        if (gCurrent_frontend_spec == &gFrontend_LOAD_GAME
                || gCurrent_frontend_spec == &gFrontend_NETSYNC
                || gCurrent_frontend_spec == &gFrontend_NETWORK_SUMMARY) {
            if (gFrontend_interpolate_steps_left == 0) {
                gCurrent_frontend_spec->unknownLastInt = FRONTEND_GenericMenuHandler(gCurrent_frontend_spec);
            }
        }
        if (gFrontend_interpolate_steps_left == 0) {
            if (gCurrent_frontend_spec == &gFrontend_WRECKS && gFrontend_wrecks_actor != NULL) {
                back_screen_base_x = gBack_screen->base_x;
                back_screen_base_y = gBack_screen->base_y;
                back_screen_origin_x = gBack_screen->origin_x;
                back_screen_origin_y = gBack_screen->origin_y;
                gBack_screen->base_x = 0;
                gBack_screen->base_y = 0;
                gBack_screen->origin_x = 320;
                gBack_screen->origin_y = 120;
                BrZbsSceneRender(gFrontend_wrecks_actor, gFrontend_wrecks_camera, gFrontend_wrecks_pixelmap, gDepth_buffer);
                gBack_screen->base_x = back_screen_base_x;
                gBack_screen->base_y = back_screen_base_y;
                gBack_screen->origin_x = back_screen_origin_x;
                gBack_screen->origin_y = back_screen_origin_y;
            }
            if (gCurrent_frontend_spec == &gFrontend_MAIN && gFrontend_menu_camera != NULL) {
                FrontendMainBuildAPO(gCurrent_APO_levels[0], gCurrent_APO_potential_levels[0], 0, 0);
                FrontendMainBuildAPO(gCurrent_APO_levels[1], gCurrent_APO_potential_levels[1], 0, 1);
                FrontendMainBuildAPO(gCurrent_APO_levels[2], gCurrent_APO_potential_levels[2], 0, 2);

                /* PrepareAPO(0) inlined */
                apo_material = gFrontend_billboard_actors[0]->material;
                apo_model = gFrontend_billboard_actors[0]->model;
                BrMapUpdate(apo_material->colour_map, BR_MAPU_ALL);
                BrMaterialUpdate(apo_material, BR_MATU_ALL);
                BrModelUpdate(apo_model, BR_MODU_VERTICES);
                PrintAPO(95, 348, 0, 0);
                PrintAPO(146, 348, 0, 1);
                PrintAPO(195, 348, 0, 2);
            }
            if (gCurrent_frontend_spec == &gFrontend_CREDITS) {
                ScrollCredits();
            }
            FRONTEND_DrawMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec->unknownLastInt == 0) {
                /* MaybeDoMouseCursor inlined */
                if (!gFrontend_suppress_mouse) {
                    PossibleLock(1);
                    DoMouseCursor();
                    PossibleUnlock(1);
                }
            }
        } else {
            gFrontend_interpolate_steps_left -= 1;
        }
        PDScreenBufferSwap(0);
        if (gCurrent_frontend_spec->unknownLastInt == 1 && gCurrent_frontend_spec != &gFrontend_START_GAME) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            strcpy(gFrontend_START_GAME.backdrop_name, gCurrent_frontend_spec->backdrop_name);
            Morph_Initialise(gCurrent_frontend_spec, &gFrontend_START_GAME);
            gCurrent_frontend_spec = &gFrontend_START_GAME;
            FRONTEND_CreateMenu(&gFrontend_START_GAME);
        }
        if (gCurrent_frontend_spec->unknownLastInt == 3) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            FRONTEND_Setup(gFrontend_next_menu);
            gCurrent_frontend_spec->unknownLastInt = 0;
        }
        if (gCurrent_frontend_spec->unknownLastInt != 0) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            if (gFrontend_backdrop != NULL) {
                BrMapRemove(gFrontend_backdrop);
                BrPixelmapFree(gFrontend_backdrop);
            }
            DisposeInterfaceFonts();
            return gCurrent_frontend_spec->unknownLastInt != 2;
        }
    }
}

// GLOBAL: CARMA2_HW 0x00686f08
int gCount_connected_items_indices;
// GLOBAL: CARMA2_HW 0x00687018
int gConnected_items_indices[6];

/* Main menu spec callbacks (runtime milestone: gFrontend_MAIN needs its create/tick procs to load the menu and backdrop).
   Based on carpocalypse2 src/carma2/common/interface-main.c and interface.c, corrected against the original disassembly. */

extern int gFrontend_scrollbars_updated;
extern int gFrontend_car_image_outdated;
extern int gTyping;
extern tFrontend_brender_item gFrontend_brender_items[100];
extern tFrontend_spec gFrontend_CHANGE_CAR;
extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);
extern tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern tRace_group_spec* gRace_groups;
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);

// GLOBAL: CARMA2_HW 0x005d6410
tFrontend_spec gFrontend_CHANGE_CAR = {
    0
};

// FUNCTION: CARMA2_HW 0x00518d60
tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp) {

    switch (pm->type) {
    case BR_PMT_RGB_555:
        return (((((red & 0xf8) << 5) | (grn & 0xf8)) << 2)) | ((blu >> 3) & 0x1f);
    case BR_PMT_RGB_565:
        return ((((red & 0xf8) << 5) | (grn & 0xfc)) << 3) | ((blu >> 3) & 0x1f);
    case BR_PMT_RGBA_8888:
        return (((((alp << 8) | red) << 8) | grn) << 8) | blu;
    case BR_PMT_RGBA_4444:
        return ((((alp & 0xfffffff0u) << 4) | (red & 0xfffffff0u)) << 4) | (blu >> 4) | (grn & 0xfffffff0u);
    case BR_PMT_ARGB_1555:
        return (((((((alp & 0xffffff80u) << 1) | (red & 0xf8)) << 5) | (grn & 0xf8)) << 2)) | ((blu >> 3) & 0x1f);
    default:
        return (((red << 8) | grn) << 8) | blu;
    }
}

// FUNCTION: CARMA2_HW 0x0046aa20
void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index) {
    tPath_name pack_path;
    char pack_filename[64];
    char pm_name[20];
    char* pos_dot;
    tTWTVFS twt;
    int i;

    pos_dot = strrchr(gOpponents[pCar_index].car_file_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");
    strcpy(pack_filename, gOpponents[pCar_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);
    for (i = 0; i < 6; i++) {
        sprintf(pm_name, "%.*s%c.TIF", (int)(pos_dot - gOpponents[pCar_index].car_file_name), gOpponents[pCar_index].car_file_name, 'A' + i);
        BrMapRemove(gFrontend_images[i + 1]);
        BrPixelmapFree(gFrontend_images[i + 1]);
        gFrontend_images[i + 1] = GetThisFuckingPixelmapPleaseMrTwatter(pack_path, pm_name);
        if (gFrontend_images[i + 1] == NULL) {
            gFrontend_images[i + 1] = LoadPixelmap("64by64.tif");
        }
        BrPixelmapCopy(gFrontend_brender_items[pBrender_index + i].field_0xc, gFrontend_images[i + 1]);
        BrPixelmapCopy(gFrontend_brender_items[pBrender_index + i].field_0x10, gFrontend_brender_items[pBrender_index + i].field_0xc);
        BrMapUpdate(gFrontend_brender_items[pBrender_index + i].field_0xc, BR_MAPU_ALL);
        BrMaterialUpdate(gFrontend_brender_items[pBrender_index + i].material, BR_MATU_COLOURMAP);
    }
    ClosePackFileAndSetTiffLoading(twt);
}

// FUNCTION: CARMA2_HW 0x00469a40
int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend) {
    int group;
    int race_index;
    int i;
    char group_name[12];
    br_camera* camera;
    tFrontend_item_spec* item;

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
    gFrontend_time_last_input = PDGetTotalTime();
    group = (gCurrent_race_group - gRace_groups) % 10;
    sprintf(group_name, "%s %d", IString_Get(78), group + 1);
    strcpy(pFrontend->items[2].text, group_name);
    race_index = 4 * group;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++) {
        item = &pFrontend->items[i];
        strcpy(item->text, gRace_list[race_index].name);
        item->radioButton_selected = race_index == gProgram_state.current_race_index;
        if (gRace_list[race_index].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
        race_index++;
    }
    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }
    div(gProgram_state.current_race_index, 4);
    strcpy(pFrontend->items[22].text, gRace_list[gProgram_state.current_race_index].description);
    MungeMetaCharactersChar(pFrontend->items[22].text, 'R', '\r');
    MungeMetaCharactersNum(pFrontend->items[22].text, 'O', gRace_list[gProgram_state.current_race_index].count_explicit_opponents);
    MungeMetaCharactersNum(pFrontend->items[22].text, 'L', gRace_list[gProgram_state.current_race_index].count_laps);
    strcpy(pFrontend->items[18].text, gProgram_state.player_name);
    strcpy(pFrontend->items[20].text, gOpponents[gProgram_state.current_car_index].car_name);
    strcpy(pFrontend->items[23].text, MungeCommas(gProgram_state.credits));
    gFrontend_car_image_outdated = 1;
    pFrontend->isPreviousSomeOtherMenu = 1;
    gMouse_in_use = 0;
    gFrontend_menu_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_menu_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BR_ANGLE_DEG(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;
    gFrontend_billboard_actors[0] = CreateAPOactor();
    gFrontend_APO_Colour_1 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0xff, 0x80, 0x00, 0xff);
    gFrontend_APO_Colour_2 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0x80, 0xff);
    gFrontend_APO_Colour_3 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0xff, 0xff);
    FuckWithWidths(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00469ea0
int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend) {
    char group_name[12];
    int race_index;
    int i;
    char* str;

    race_index = 0;
    for (i = 0; i < gNumber_of_races; i++) {
        if (DRStricmp(pFrontend->items[gFrontend_selected_item_index].text, gRace_list[i].name) == 0) {
            race_index = i;
            break;
        }
    }
    gProgram_state.current_race_index = race_index;
    pFrontend->items[22].highFont = 11;
    pFrontend->items[22].unlitFont = 11;
    if (!gMouse_in_use) {
        str = pFrontend->items[22].text;
        strcpy(str, gRace_list[race_index].description);
        MungeMetaCharactersChar(str, 'R', '\r');
        MungeMetaCharactersNum(str, 'O', gRace_list[race_index].count_explicit_opponents);
        MungeMetaCharactersNum(str, 'L', gRace_list[race_index].count_laps);
    }
    gCurrent_race_group = gRace_list[gProgram_state.current_race_index].group;
    sprintf(group_name, "%s %d", IString_Get(78), (gCurrent_race_group - gRace_groups) % 10 + 1);
    strcpy(pFrontend->items[2].text, group_name);
    FuckWithWidths(pFrontend);
    return 0;
}

static int C2_HOOK_FASTCALL MainMenuRaceIndex(const char* pName, int pDefault) {
    int i;

    for (i = 0; i < gNumber_of_races; i++) {
        if (DRStricmp(pName, gRace_list[i].name) == 0) {
            return i;
        }
    }
    return pDefault;
}

static int C2_HOOK_FASTCALL MainMenuFindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index + 1; i <= pFrontend->count_items - 1; i++) {
        if (pFrontend->items[i].enabled > 0 && pFrontend->items[i].visible) {
            return i;
        }
    }
    return 0;
}

static void C2_HOOK_FASTCALL MainMenuSwitchTo(tFrontend_spec* pFrontend, tFrontend_spec* pNext) {
    int i;

    for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
        gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
    }
    FRONTEND_DestroyMenu(pFrontend);
    gCurrent_frontend_spec = pNext;
    FRONTEND_CreateMenu(gCurrent_frontend_spec);
    if (gCurrent_frontend_spec != pFrontend->previous) {
        gCurrent_frontend_spec->previous = pFrontend;
    }
    Morph_Initialise(pFrontend, gCurrent_frontend_spec);
}

static void C2_HOOK_FASTCALL MainMenuRestoreScrollers(void) {
    int i;

    for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
        gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
    }
    UpdateScrollPositions(gCurrent_frontend_spec);
}

// FUNCTION: CARMA2_HW 0x0046a0e0
int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend) {
    int i;
    int race_index;
    int input;
    int key;
    int mouse_button;
    int mouse_x;
    int mouse_y;
    int item_at_mouse_pos;
    int result;
    char buf[2];

    if (gProgram_state.game_completed) {
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].visible = 1;
        pFrontend->items[8].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[8].visible = 1;
    } else {
        pFrontend->items[3].enabled = 0;
        pFrontend->items[3].visible = 0;
        pFrontend->items[8].enabled = 0;
        pFrontend->items[8].visible = 0;
    }
    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }
    TransparentPolyFontTextInABox(kPolyfont_hand_green_10pt_lit, "v1.02a",
        gBack_screen->width - PolyFontTextWidth(kPolyfont_hand_green_10pt_lit, "v1.02a") - 4,
        gBack_screen->height - PolyFontHeight(kPolyfont_hand_green_10pt_lit) - 4,
        gBack_screen->width,
        gBack_screen->height,
        eJust_left, 1, 150.);
    if (gFrontend_selected_item_index == 0) {
        pFrontend->default_item = 21;
        gFrontend_selected_item_index = 21;
    }
    if (gFrontend_car_image_outdated) {
        MenuSetCarImage(gProgram_state.current_car_index, 10);
        gFrontend_car_image_outdated = 0;
    }
    race_index = 0;
    for (i = gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem; i <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem; i++) {
        race_index = MainMenuRaceIndex(pFrontend->items[i].text, race_index);
        if (gRace_list[race_index].count_opponents != 0) {
            sprintf(buf, "%c", '\x1f');
            SolidPolyFontText(buf, pFrontend->items[i].x - 10, pFrontend->items[i].y, kPolyfont_hand_green_15pt_lit, eJust_centre, 1);
        }
    }
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            ToggleTyping(pFrontend);
            if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {
                strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
                SaveOptions();
            }
        }
        FuckWithWidths(pFrontend);
    } else {
        PollKeys();
        EdgeTriggerModeOff();
        key = PDAnyKeyDown();
        if (key != -1 && key != 4) {
            gMouse_in_use = 0;
            gFrontend_time_last_input = PDGetTotalTime();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_at_mouse_pos = -1;
        if (gMouse_in_use) {
            gFrontend_selected_item_index = 0;
            gFrontend_time_last_input = PDGetTotalTime();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse_pos = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = item_at_mouse_pos;
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 99;
            }
            if (gFrontend_selected_item_index > 3 && gFrontend_selected_item_index < 8
                    && pFrontend->items[gFrontend_selected_item_index].enabled > 0) {
                race_index = MainMenuRaceIndex(pFrontend->items[gFrontend_selected_item_index].text, 0);
                strcpy(pFrontend->items[22].text, gRace_list[race_index].description);
                MungeMetaCharactersChar(pFrontend->items[22].text, 'R', '\r');
                MungeMetaCharactersNum(pFrontend->items[22].text, 'O', gRace_list[race_index].count_explicit_opponents);
                MungeMetaCharactersNum(pFrontend->items[22].text, 'L', gRace_list[race_index].count_laps);
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
            if (gFrontend_selected_item_index == 15) {
                gFrontend_selected_item_index = 10;
            }
            gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
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
            if (gFrontend_selected_item_index < 21) {
                if (gFrontend_selected_item_index == 10) {
                    gFrontend_selected_item_index = 15;
                }
                gFrontend_selected_item_index = MainMenuFindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
            } else {
                gFrontend_selected_item_index = -1;
                gFrontend_selected_item_index = MainMenuFindNextActiveItem(gCurrent_frontend_spec, -1);
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
        if (PDKeyDown(63)) {
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            MainMenuSwitchTo(pFrontend, &gFrontend_OPTIONS);
            MainMenuRestoreScrollers();
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            if (gProgram_state.racing) {
                if (gNet_mode != eNet_mode_none && gCurrent_frontend_spec->default_item == 5) {
                    gFrontend_selected_item_index = 3;
                }
            } else if (gCurrent_frontend_spec->default_item < 3 || gCurrent_frontend_spec->default_item > 30) {
                gFrontend_selected_item_index = 3;
            }
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse_pos != -1)) {
            ToggleSelection(pFrontend);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
                gFrontend_selected_item_index = 21;
            }
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
                return 2;
            }
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
                gFrontend_leave_current_menu = 1;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                if (mouse_button == 2 && gFrontend_selected_item_index >= 4 && gFrontend_selected_item_index < 8) {
                    if (gProgram_state.field_0x2c) {
                        return 1;
                    }
                    gFrontend_next_menu = kFrontend_menu_newgame;
                    return 3;
                }
            } else {
                result = pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (result == 5) {
                return 1;
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo == NULL) {
                return result;
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            MainMenuSwitchTo(pFrontend, pFrontend->items[gFrontend_selected_item_index].menuInfo);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            if (gCurrent_frontend_spec == &gFrontend_OPTIONS) {
                if (gProgram_state.racing) {
                    if (gNet_mode != eNet_mode_none && gCurrent_frontend_spec->default_item == 5) {
                        gFrontend_selected_item_index = 3;
                    }
                } else if (gCurrent_frontend_spec->default_item < 3) {
                    gFrontend_selected_item_index = 3;
                }
            }
            MainMenuRestoreScrollers();
            return result;
        }
    }
    ServiceGame();
    CheckForCheatingGits();
    return gFrontend_leave_current_menu != 0;
}

// FUNCTION: CARMA2_HW 0x00467280
int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend) {
    int orig_selected_item;
    int input;
    int key;
    int mouse_button;
    int mouse_x;
    int mouse_y;
    int item_at_mouse_pos;
    int i;

    orig_selected_item = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            ToggleTyping(pFrontend);
            if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {
                strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
                SaveOptions();
            }
            SaveOptions();
        }
        ServiceGame();
        return 0;
    }
    PollKeys();
    EdgeTriggerModeOff();
    key = PDAnyKeyDown();
    if (key != -1 && key != 4) {
        gMouse_in_use = 0;
        gFrontend_time_last_input = PDGetTotalTime();
    }
    EdgeTriggerModeOn();
    mouse_button = 0;
    item_at_mouse_pos = -1;
    if (gMouse_in_use) {
        gFrontend_time_last_input = PDGetTotalTime();
        GetMousePosition(&mouse_x, &mouse_y);
        mouse_button = EitherMouseButtonDown();
        item_at_mouse_pos = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
        if (item_at_mouse_pos != -1) {
            gFrontend_selected_item_index = item_at_mouse_pos;
        } else {
            gFrontend_selected_item_index = 99;
        }
    }
    if (PDKeyDown(72) || PDKeyDown(89)) {
        gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
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
        if (gFrontend_selected_item_index < pFrontend->count_items - 1) {
            gFrontend_selected_item_index = MainMenuFindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
        } else {
            gFrontend_selected_item_index = -1;
            gFrontend_selected_item_index = MainMenuFindNextActiveItem(gCurrent_frontend_spec, -1);
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
    if (PDKeyDown(63)) {
        gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
        FRONTEND_DestroyMenu(pFrontend);
        gCurrent_frontend_spec = &gFrontend_MAIN;
        FRONTEND_CreateMenu(&gFrontend_MAIN);
        Morph_Initialise(pFrontend, gCurrent_frontend_spec);
        gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
        MainMenuRestoreScrollers();
        DRS3StartSound(gEffects_outlet, 0xbbd);
        return 0;
    }
    if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse_pos != -1)) {
        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
            return 2;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            if (mouse_button == 2) {
                return 1;
            }
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            MainMenuRestoreScrollers();
        }
        DRS3StartSound(gEffects_outlet, 0xbbc);
        return pFrontend->items[orig_selected_item].field_0xc;
    }
    ServiceGame();
    return 0;
}
