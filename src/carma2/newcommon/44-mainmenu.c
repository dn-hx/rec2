#include "44-mainmenu.h"
#include "rec2_types.h"
#include "globvars.h"
#include "69-sound.h"

#include "69-sound.h"
#include "72-interface.h"
#include "globvars.h"

#include "01-network.h"
#include "69-sound.h"
#include "72-interface.h"
#include "globvars.h"
#include "globvrpb.h"
#include "07-structur.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "35-intrface.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "platform.h"

// FUNCTION: CARMA2_HW 0x004943f0
void C2_HOOK_FASTCALL QuitVerifyStart(void) {

    FadePaletteDown();
}

extern int gMouse_was_started__mainmenu;
extern int gReplace_background;


extern int gMouse_started;

// GLOBAL: CARMA2_HW 0x00659198
tFlicette gFlicker_on_flics__mainmenu[2] = {
    { 43, { 181, 362 }, { 124, 298 } },
    { 43, { 84, 168 }, { 124, 298 } },
};

// GLOBAL: CARMA2_HW 0x006591c0
tFlicette gFlicker_off_flics__mainmenu[2] = {
    { 42, { 181, 362 }, { 124, 298 } },
    { 42, { 84, 168 }, { 124, 298 } },
};

// GLOBAL: CARMA2_HW 0x006591e8
tFlicette gPushed_flics__mainmenu[2] = {
    { 135, { 181, 362 }, { 124, 298 } },
    { 45, { 84, 168 }, { 124, 298 } },
};

// GLOBAL: CARMA2_HW 0x00659210
tMouse_area gMouse_areas__mainmenu[2] = {
    { { 181, 362 }, { 124, 298 }, { 244, 488 }, { 144, 346 }, 0, 0, 0, NULL },
    { { 84, 168 }, { 124, 298 }, { 147, 294 }, { 144, 346 }, 1, 0, 0, NULL },
};

// GLOBAL: CARMA2_HW 0x00659270
tInterface_spec gQuitVerify_spec = {
    0,                          /* initial_imode */
    0,                          /* first_opening_flic (set by DoVerifyQuit) */
    0,                          /* second_opening_flic */
    0,                          /* end_flic_go_ahead */
    0,                          /* end_flic_escaped */
    0,                          /* end_flic_otherwise */
    0,                          /* flic_bunch_to_load */
    { -1, 0 },                  /* move_left_new_mode */
    { -1, 0 },                  /* move_left_delta */
    { 0, 0 },                   /* move_left_min */
    { 1, 0 },                   /* move_left_max */
    { NULL, NULL },             /* move_left_proc */
    { -1, 0 },                  /* move_right_new_mode */
    { 1, 0 },                   /* move_right_delta */
    { 0, 0 },                   /* move_right_min */
    { 1, 0 },                   /* move_right_max */
    { NULL, NULL },             /* move_right_proc */
    { -1, 0 },                  /* move_up_new_mode */
    { 0, 0 },                   /* move_up_delta */
    { 0, 0 },                   /* move_up_min */
    { 0, 0 },                   /* move_up_max */
    { NULL, NULL },             /* move_up_proc */
    { -1, 0 },                  /* move_down_new_mode */
    { 0, 0 },                   /* move_down_delta */
    { 0, 0 },                   /* move_down_min */
    { 0, 0 },                   /* move_down_max */
    { NULL, NULL },             /* move_down_proc */
    { 1, 1 },                   /* go_ahead_allowed */
    { NULL, NULL },             /* go_ahead_proc */
    { 1, 1 },                   /* escape_allowed */
    { NULL, NULL },             /* escape_proc */
    NULL,                       /* exit_proc */
    NULL,                       /* draw_proc */
    0,                          /* time_out */
    QuitVerifyStart,            /* start_proc1 */
    NULL,                       /* start_proc2 */
    QuitVerifyDone,             /* done_proc */
    0,                          /* font_needed */
    { 0, 0 },                   /* typeable */
    NULL,                       /* get_original_string */
    1,                          /* escape_code */
    1,                          /* dont_save_or_load */
    2,                          /* number_of_button_flics */
    gFlicker_on_flics__mainmenu,  /* flicker_on_flics */
    gFlicker_off_flics__mainmenu, /* flicker_off_flics */
    gPushed_flics__mainmenu,      /* pushed_flics */
    2,                          /* number_of_mouse_areas */
    gMouse_areas__mainmenu,     /* mouse_areas */
    0,                          /* number_of_recopy_areas */
    NULL,                       /* recopy_areas */
};

// FUNCTION: CARMA2_HW 0x00494400
int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {
    (void)pCurrent_mode;
    (void)pGo_ahead;
    (void)pEscaped;
    (void)pTimed_out;

    FadePaletteDown();
    TurnOnPanelFlics();
    if (gMouse_was_started__mainmenu != 0) {
        RemoveTransientBitmaps(1);
    }
    if (gReplace_background != 0) {
        if (pCurrent_choice == 0) {
            ClearEntireScreen();
        }
        PDScreenBufferSwap(0);
        FadePaletteUp();
    }
    return pCurrent_choice == 0;
}

// FUNCTION: CARMA2_HW 0x00494450
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
    int from_race;
    int result;

    from_race = 0;
    if (gAusterity_mode != 0) {
        return 1;
    }
    if (gProgram_state.racing != 0) {
        FadePaletteDown();
        ClearEntireScreen();
        GoingToInterfaceFromRace();
        from_race = 1;
        gProgram_state.racing = 0;
    }
    switch (IRandomBetween(1, 3)) {
    case 1:
        gQuitVerify_spec.first_opening_flic = 0x82;
        break;
    case 2:
        gQuitVerify_spec.first_opening_flic = 0x83;
        break;
    case 3:
        gQuitVerify_spec.first_opening_flic = 0x84;
        break;
    }
    gMouse_was_started__mainmenu = gMouse_started;
    if (gMouse_started != 0) {
        RemoveTransientBitmaps(1);
        EndMouseCursor();
    }
    TurnOffPanelFlics();
    gReplace_background = pReplace_background;
    DRS3StopOutletSound(gEffects_outlet);
    LoadInterfaceStuff(from_race);
    result = DoInterfaceScreen(&gQuitVerify_spec, 0, 0);
    if (gMouse_was_started__mainmenu != 0) {
        LoadInterfaceStuff(from_race);
        StartMouseCursor();
    }
    if (from_race) {
        GoingBackToRaceFromInterface();
        gProgram_state.racing = 1;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00494540
// FUNCTION: CARMA2_SW 0x00491840
void C2_HOOK_FASTCALL DoMainScreen(void) {

    StartMusicTrack(9998);
    switch (FRONTEND_Main(kFrontend_menu_main)) {
    case 0:
        gProgram_state.prog_status = eProg_quit;
        break;
    case 1:
        gProgram_state.prog_status = eProg_game_starting;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00494570
// FUNCTION: CARMA2_SW 0x00491870
void C2_HOOK_FASTCALL DoOptionsMenu(void) {
    tPlayer_status player_status;

    if (gNet_mode != eNet_mode_none) {
        player_status = gNet_players[gThis_net_player_index].player_status;
        NetPlayerStatusChanged(ePlayer_status_main_menu);
    }
    if (FRONTEND_Main(kFrontend_menu_options) == 0) {
        gProgram_state.prog_status = eProg_quit;
    }
    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status == ePlayer_status_main_menu) {
        NetPlayerStatusChanged(player_status);
    }
}

// GLOBAL: CARMA2_HW 0x0068c850
int gMouse_was_started__mainmenu = 0;

// GLOBAL: CARMA2_HW 0x0068c854
int gReplace_background = 0;
