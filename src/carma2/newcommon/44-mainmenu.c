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

// QuitVerifyStart

extern int gMouse_was_started__mainmenu;
extern int gReplace_background;

extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL TurnOnPanelFlics(void);
extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
extern void C2_HOOK_FASTCALL FadePaletteUp(void);

extern int gMouse_started;
extern void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);
extern void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL EndMouseCursor(void);
extern void C2_HOOK_FASTCALL StartMouseCursor(void);
extern void C2_HOOK_FASTCALL TurnOffPanelFlics(void);
extern void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race);
extern int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

// GLOBAL: CARMA2_HW 0x00659270
tInterface_spec gQuitVerify_spec;

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
