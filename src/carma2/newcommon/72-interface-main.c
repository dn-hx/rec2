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
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "c2_string.h"

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
// STUB: CARMA2_HW 0x0046d1c0
void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pFrontend_type) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0046c0d0
int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
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
