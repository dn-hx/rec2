#include "72-interface-main.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x005a80f0
tFrontend_spec gFrontend_MAIN = {
#if 0 // FIXME: enable this
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
        { 1, 8, 6, 12, 12, 12, 19 },
    }
#else
    0
#endif
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
extern int C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h);

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

// STUB: CARMA2_HW 0x0046e020
void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
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
