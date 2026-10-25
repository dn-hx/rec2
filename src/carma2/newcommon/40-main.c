#include "40-main.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_types.h"
#include "01-network.h"
#include "08-loading1.h"
#include "41-utility.h"
#include "52-errors.h"
#include "69-sound.h"
#include "02-init.h"
#include "07-structur.h"
#include "10-loading2.h"
#include "24-loadsave.h"
#include "64-movie.h"
#include "60-phil.h"
#include "28-world3.h"
#include "11-world1.h"
#include "18-graphics2.h"

#include <string.h>

// FUNCTION: CARMA2_HW 0x00491f70
C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void) {
    SaveOptions();
    if (gSave_game_out_of_sync) {
        DoSaveGame();
    }
    MovieStopRecordingIfNecessary();
    PHILDisable();
    dr_dprintf("QuitGame() - Point 1");
    if (gProgram_state.racing && gAdditional_actors != NULL && gAdditional_actors->children != NULL) {
        dr_dprintf("QuitGame() - Point 2");
        AutoSaveAdditionalStuff();
        dr_dprintf("QuitGame() - Point 3");
    }
    gProgram_state.racing = 0;
    SaveOptions();
    if (gNet_mode != eNet_mode_none) {
        NetLeaveGame(gCurrent_net_game);
    }
    ShutdownNetIfRequired();
    if (gSound_available) {
        DRS3ShutDown();
    }
    if (gBr_initialized) {
        ClearEntireScreen();
    }
    PDRevertPalette();
    StopMusic();
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
    CloseDiagnostics();
    exit(0);
}

// ServiceTheGame

extern void C2_HOOK_FASTCALL CheckMemory(void);
extern void C2_HOOK_FASTCALL CyclePollKeys(void);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int gSound_available;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00442e90
void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00492180
void C2_HOOK_FASTCALL ServiceGame(void) {
    CheckMemory();
    CyclePollKeys();
    PollKeys();
    rand();
    if (PDServiceSystem(gFrame_period)) {
        SaveOptions();
        if (gSave_game_out_of_sync) {
            DoSaveGame();
        }
        MovieStopRecordingIfNecessary();
        PHILDisable();
        dr_dprintf("QuitGame() - Point 1");
        if (*(int*)((char*)&gProgram_state + 0x28) && gAdditional_actors && gAdditional_actors->children) {
            dr_dprintf("QuitGame() - Point 2");
            AutoSaveAdditionalStuff();
            dr_dprintf("QuitGame() - Point 3");
        }
        *(int*)((char*)&gProgram_state + 0x28) = 0;
        SaveOptions();
        if (gNet_mode) {
            NetLeaveGame(gCurrent_net_game);
        }
        ShutdownNetIfRequired();
        if (gSound_available) {
            DRS3ShutDown();
        }
        if (gBr_initialized) {
            ClearEntireScreen();
        }
        PDRevertPalette();
        StopMusic();
        if (gBr_initialized) {
            DRBrEnd();
        }
        PDShutdownSystem();
        CloseDiagnostics();
        exit(0);
    }
    CheckSystemKeys(0);
    if (gSound_enabled) {
        SoundService();
    }
    NetService(0);
}

// ServiceGameInRace

// FUNCTION: CARMA2_HW 0x004924a0
void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv) {
    tPath_name location;

    PDSetFileVariables();
    PDBuildAppPath(gApplication_path);
    OpenDiagnostics();

    strcat(gApplication_path, "DATA");
    UsePathFileToDetermineIfFullInstallation();
    if (!gCD_fully_installed) {
        if (GetRegisterSourceLocation(location)) {
            if (!PDCheckDriveExists(location)) {
                PDInitialiseSystem();
                PDFatalError("Can't find the Carmageddon CD\n");
            }
        }
    }
    InitialiseDeathRace(pArgc, pArgv);
    DoProgram();
    QuitGame();
}
