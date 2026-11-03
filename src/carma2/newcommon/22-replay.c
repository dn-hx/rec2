#include "22-replay.h"

#include <stdlib.h>
#include <stdio.h>

#include "globvars.h"
#include "30-opponent.h"
#include "18-graphics2.h"
#include "23-piping.h"
#include "41-utility.h"
#include "59-camera.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "13-crush1.h"
#include "32-spark.h"
#include "56-aiworld.h"
#include "57-drone.h"

// 583638a7f removed these six prototypes from 41-utility.h; declaring them here keeps this TU's
// codegen at the state it had while they were in the header (WorkerPedDismember 0x4ca900).
extern float C2_HOOK_FASTCALL signf(float pValue);
extern void C2_HOOK_FASTCALL CopyMiscString(int pIndex, char* pDest);
extern void C2_HOOK_FASTCALL CopyBrMaterial(br_material* pDestination, const br_material* pSource);
extern void C2_HOOK_FASTCALL GetBlendTableName(int pPercent, char* pDest);
extern int C2_HOOK_FASTCALL DRVector2TestForNan(const br_vector2* pV);
extern int C2_HOOK_FASTCALL DRVector4TestForNan(const float* pV);

// GLOBAL: CARMA2_HW 0x0058f620
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x006a2374
int gKey_down;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// GLOBAL: CARMA2_HW 0x0065fbe0
int gReplay_timeline_x1[2] = { 240, 443 };

// GLOBAL: CARMA2_HW 0x0065fbe8
int gReplay_timeline_x2[2] = { 306, 612 };

// GLOBAL: CARMA2_HW 0x0065fbf0
int gReplay_timeline_y[2] = { 184, 446 };

// FUNCTION: CARMA2_HW 0x004e6280
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
    extern int C2_HOOK_FASTCALL MovieRecording(void);
    extern int C2_HOOK_FASTCALL DoMouseCursor(void);
    extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
    extern void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText);
    extern tGraf_data* gCurrent_graf_data;
    extern int gReal_graf_data_index;
    extern tU32 gLast_replay_frame_time;
    extern br_pixelmap* gReplay_pixelmaps[11];
    extern tU32 gUnk_006a2360;
    extern tU32 gUnk_006a2364;
    extern tU32 gUnk_006a23c8;
    extern tDR_font gFonts[28];
    extern int gReplay_show_controls;
    extern int gShow_status_message;
    extern int gNext_replay_record_state;
    extern int gStatus_message_1;
    extern int gStatus_message_2;
    extern int gRecording_mouse_disabled;
    tU16 colour1;
    tU16 colour2;
    tU16 colour3;
    tU16 colour4;
    tU32 time;
    int elapsed;
    int total;
    int pos;
    int i;
    int x;
    int text_y;
    int showed_camera;

    colour3 = 4;
    colour2 = 2;
    colour4 = 0x52;
    colour1 = 0x51;
    if (gBack_screen->type != 3) {
        colour3 = PaletteEntry16Bit(gRender_palette, 4);
        colour2 = PaletteEntry16Bit(gRender_palette, 2);
        colour4 = PaletteEntry16Bit(gRender_palette, 0x52);
        colour1 = PaletteEntry16Bit(gRender_palette, 0x51);
    }
    time = PDGetTotalTime();
    if (PDKeyDown(0)) {
        return;
    }
    PossibleLock(1);
    DRMaskedStamp((br_int_16)gCurrent_graf_data->field_0x300, (br_int_16)gCurrent_graf_data->field_0x304, gReplay_pixelmaps[1]);
    if (MovieRecording()) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x358), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x35c), gReplay_pixelmaps[10]);
    }
    if (pSpecial_zappy_bastard < 0) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x310 + gCurrent_graf_data->field_0x300), (br_int_16)(gCurrent_graf_data->field_0x314 + gCurrent_graf_data->field_0x304), gReplay_pixelmaps[2]);
    } else if (pSpecial_zappy_bastard > 0) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x340), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x344), gReplay_pixelmaps[8]);
    } else if (ARGetReplayRate() < -1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x318 + gCurrent_graf_data->field_0x300), (br_int_16)(gCurrent_graf_data->field_0x31c + gCurrent_graf_data->field_0x304), gReplay_pixelmaps[3]);
    } else if (ARGetReplayRate() > 1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x338), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x33c), gReplay_pixelmaps[7]);
    } else if (ARGetReplayRate() == 1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x330), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x334), gReplay_pixelmaps[6]);
    } else if (ARGetReplayRate() == -1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x320), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x324), gReplay_pixelmaps[4]);
    } else {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x328), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x32c), gReplay_pixelmaps[5]);
    }
    elapsed = gLast_replay_frame_time - gUnk_006a2360;
    total = gUnk_006a2364 - gUnk_006a2360;
    pos = (int)((float)elapsed / (float)total * (gReplay_timeline_x2[gReal_graf_data_index] - gReplay_timeline_x1[gReal_graf_data_index])) + gReplay_timeline_x1[gReal_graf_data_index];
    if (pos > gReplay_timeline_x1[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index], pos - 1, gReplay_timeline_y[gReal_graf_data_index], colour2);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index] + i, pos - 1, gReplay_timeline_y[gReal_graf_data_index] + i, colour3);
        }
        BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index] + 6, pos - 1, gReplay_timeline_y[gReal_graf_data_index] + 6, colour2);
    }
    if (pos < gReplay_timeline_x2[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index], gReplay_timeline_x2[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index], colour1);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index] + i, gReplay_timeline_x2[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index] + i, colour4);
        }
        BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index] + 6, gReplay_timeline_x2[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index] + 6, colour1);
    }
    BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index], gReplay_timeline_x1[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index] + 6, colour2);
    BrPixelmapLine(gBack_screen, gReplay_timeline_x2[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index], gReplay_timeline_x2[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index] + 6, colour1);
    if (gReplay_show_controls) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x348), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x34c), gReplay_pixelmaps[9]);
    }
    PossibleUnlock(1);
    showed_camera = 0;
    if (time - gUnk_006a23c8 < 2000) {
        showed_camera = 1;
        DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x350, gCurrent_graf_data->field_0x354, &gFonts[1], GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode), gCurrent_graf_data->field_0x350 * 2);
    }
    if (MovieRecording()) {
        if (gShow_status_message) {
            if (gNext_replay_record_state == 4) {
                x = gCurrent_graf_data->field_0x350;
                text_y = gCurrent_graf_data->field_0x354;
                if (!showed_camera) {
                    DRPixelmapText(gBack_screen, x, text_y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
                }
            } else {
                x = gCurrent_graf_data->width / 2;
                text_y = gCurrent_graf_data->height / 2;
                DRPixelmapCentredText(gBack_screen, x, text_y - 0x40, &gFonts[4], GetMiscString(gStatus_message_1));
                if (gStatus_message_2) {
                    DRPixelmapCentredText(gBack_screen, x, text_y - 0x20, &gFonts[6], GetMiscString(gStatus_message_2));
                }
            }
        }
    } else if ((time / 400 & 1) != 0) {
        PossibleLock(1);
        DRMaskedStamp((br_int_16)gCurrent_graf_data->field_0x2f8, (br_int_16)gCurrent_graf_data->field_0x2fc, gReplay_pixelmaps[0]);
        PossibleUnlock(1);
    }
    if (!gRecording_mouse_disabled && !MovieRecording()) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e6260
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {

    RemoveTransientBitmaps(1);
}
// FUNCTION: CARMA2_HW 0x004e6900
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number) {
    extern tU32 gTime_of_last_zappy_headups;
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
    tU32 time;

    time = PDGetTotalTime();
    if (abs(pFrame_number) > 0x2710 && time - gTime_of_last_zappy_headups > 0x32) {
        ActualActionReplayHeadups(pFrame_number);
        gTime_of_last_zappy_headups = time;
        PDScreenBufferSwap(0);
        RemoveTransientBitmaps(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6270
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {

    ActualActionReplayHeadups(0);
}

/* the original passes pFrame_period in ecx to MungePedestrians (rec2 defines it with no parameter) */
#ifdef REC2_MATCHING
#define REPLAY_CALL_MUNGE_PEDESTRIANS(P) ((void (C2_HOOK_FASTCALL *)(int))MungePedestrians)(P)
#else
#define REPLAY_CALL_MUNGE_PEDESTRIANS(P) MungePedestrians()
#endif

// FUNCTION: CARMA2_HW 0x004e6950
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {
    extern tU32 gCamera_period;
    extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);
    extern void C2_HOOK_FASTCALL MungePedestrians(void);

    UpdateFramePeriod(&gCamera_period);
    if (ARGetReplayRate() < 0.0f) {
        REPLAY_CALL_MUNGE_PEDESTRIANS(pFrame_period);
    }
}

extern void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL MungePedestrians(void);
extern void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pArg1);

// FUNCTION: CARMA2_HW 0x004e6980
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) {
    ProcessOilSpills((tU32)pFrame_period);
    if (ARGetReplayRate() >= 0.0f) {
        REPLAY_CALL_MUNGE_PEDESTRIANS(pFrame_period);
    }
    MungePowerupStuff(pFrame_period);
}

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
extern int gEntering_message;
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
extern void C2_HOOK_FASTCALL ComparePedStuff(void);
extern void C2_HOOK_FASTCALL SavePedStuff(void);
extern void C2_HOOK_FASTCALL ToggleMap(void);
extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);
extern tU32 gUnk_006a2360;
extern tU32 gUnk_006a2364;
extern int gUnk_006a2378;
extern tU32 gUnk_006a237c;
extern tU32 gUnk_006a23c8;
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern tNet_mode gNet_mode;
extern int gCamera_type;

// FUNCTION: CARMA2_HW 0x004e6ff0
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {

    if (!gAction_replay_mode) {
        if (!KeyIsDown(58) || gEntering_message) {
            gKey_down = -1;
        } else if (gKey_down == -1) {
            if (gAction_replay_mode) {
                RenderAFrame(1);
                RenderAFrame(1);
            }
            if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
                    (tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess, NULL, NULL)) {
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
            }
        }
    }
}

// GLOBAL: CARMA2_HW 0x0065fbb0
const char* gReplay_pixie_names[11] = {
    "REPLAY.PIX",
    "RBUTTONS.PIX",
    "REWSTART.PIX",
    "REW.PIX",
    "REVPLAY.PIX",
    "PAUSE.PIX",
    "PLAY.PIX",
    "FFWD.PIX",
    "FWDEND.PIX",
    "CAMERA.PIX",
    "RECORD.PIX",
};

// GLOBAL: CARMA2_HW 0x006a2388
br_pixelmap* gReplay_pixelmaps[11];

// (reference only)
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);

// FUNCTION: CARMA2_HW 0x004e7090
void C2_HOOK_FASTCALL InitializeActionReplayPix(void) {
    int i;

    for (i = 0; i < 11; i++) {
        gReplay_pixelmaps[i] = LoadPixelmap(gReplay_pixie_names[i]);
        BRPM_convert(gReplay_pixelmaps[i], gBack_screen->type);
        BrMapAdd(gReplay_pixelmaps[i]);
    }
}

// FreeActionReplayPix

extern int gTimers_stack[8];
extern int gTimers_max_index;
extern int gTimers_stack_size;
extern tTimer gTimers[19];
extern int gSave_game_out_of_sync;
extern int gBr_initialized;
extern br_actor* gAdditional_actors;
extern tPhysics_object* gMutant_tail_first_collision_info;
extern tSmoke_column gSmoke_column[10];
extern int C2_HOOK_FASTCALL PDGetMicroseconds(void);
extern int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call);
extern void C2_HOOK_FASTCALL CheckMemory(void);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern void C2_HOOK_FASTCALL DoSaveGame(void);
extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);
extern void C2_HOOK_FASTCALL PHILDisable(void);
extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);
extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);
extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);
extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);
extern void C2_HOOK_FASTCALL DRS3ShutDown(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL PDRevertPalette(void);
extern void C2_HOOK_FASTCALL StopMusic(void);
extern br_error C2_HOOK_FASTCALL DRBrEnd(void);
extern void C2_HOOK_FASTCALL PDShutdownSystem(void);
extern void C2_HOOK_FASTCALL CloseDiagnostics(void);
extern void C2_HOOK_FASTCALL NetService(int pIn_race);
extern void C2_HOOK_FASTCALL CheckKevKeys(void);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// (GPL-3.0), follows the disassembly (ServiceTheGame(1) and Timers_Push/Pop inlined)
// FUNCTION: CARMA2_HW 0x004922a0
void C2_HOOK_FASTCALL ServiceGameInRace(void) {
    int prev_type;
    tU32* d;

    gTimers_stack[gTimers_stack_size] = 17;
    prev_type = gTimers_stack_size != 0 ? gTimers_stack[gTimers_stack_size - 1] : gTimers_max_index;
    d = &gTimers[prev_type].durations[gTimers[prev_type].index];
    *d += PDGetMicroseconds() - gTimers[prev_type].start_time;
    gTimers[17].start_time = PDGetMicroseconds();
    gTimers_stack_size += 1;

    CheckMemory();
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
    NetService(1);
    CheckKevKeys();

    gTimers[17].durations[gTimers[17].index] += PDGetMicroseconds() - gTimers[17].start_time;
    if (gTimers_stack_size > 1) {
        prev_type = gTimers_stack[gTimers_stack_size - 1];
    } else {
        prev_type = gTimers_max_index;
    }
    gTimers[prev_type].start_time = PDGetMicroseconds();
    gTimers_stack_size -= 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a23c4
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a23d4
tU32 gLast_synch_time;

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);

// FUNCTION: CARMA2_HW 0x004e7110
void C2_HOOK_FASTCALL SynchronizeActionReplay(void) {

    while (ARGetReplayRate() != 0.f && (double)(tU32)(PDGetTotalTime() - gLast_synch_time) < (double)gFrame_period / fabs(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
}

// (GPL-3.0), follows the disassembly
// FUNCTION: CARMA2_HW 0x004e71c0
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {

    SavePedStuff();
    if (gMap_view == 2) {
        ToggleMap();
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
    gUnk_006a237c = PDGetTotalTime();
    gUnk_006a2360 = ARGetStartTime();
    LoadInterfaceStuff(1);
    StartMouseCursor();
    gKey_down = 0x34;
    gUnk_006a2378 = 0;
    gUnk_006a23c8 = PDGetTotalTime();
    gUnk_006a2364 = GetTotalTime();
    if (gRace_finished == 0) {
        SaveCameraPosition(0);
    }
    gCamera_type = gAction_replay_camera_mode;
    SetCameraType(gAR_camera_type);
    InitReplaySplashes();
}

// FUNCTION: CARMA2_HW 0x004e7270
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

    UnlockInterfaceStuff();
    AssertThisTimeAsCurrentTime();
    if (gRace_finished == 0) {
        RestoreCameraPosition(0);
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
    gAR_camera_type = gAction_replay_camera_mode;
    SetCameraType(gCamera_type);
    ComparePedStuff();
    EndMouseCursor();
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif
}

// FUNCTION: CARMA2_HW 0x004e72d0
void C2_HOOK_FASTCALL AfterActionReplay(void) {

    ForceRebuildActiveCarList();
}
// FUNCTION: CARMA2_HW 0x004e72e0
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) {

    if (gAction_replay_mode) {
        RenderAFrame(1);
        RenderAFrame(1);
    }
    if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
            (tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess, pArg1, pArg2)) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
    }
}

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gReplay_pause_state = pPause;
}

// Replay pipe workers (no PDB symbols). Kept in this TU so that
// the Apply/Undo wrappers in 23-piping.c tail-call them instead of inlining.

// FUNCTION: CARMA2_HW 0x0043bd70
void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {
    br_model* pModel;
    br_vector3 offset;
    br_vector3 scaled_offset;
    tU16* pWeights;
    br_scalar scale;
    int i;
    int vertex_offset;

    pModel = (br_model*)field_0x4;
    ExpandVector3(&offset, (const tCompressed_vector3*)pField_0x8, -1.f, 1.f);
    i = 0;
    if (pModel->nvertices > 0) {
        pWeights = (tU16*)pField_0xe;
        vertex_offset = 0;
        do {
            if (*pWeights != 0) {
                scale = DRU16ToScalar(*pWeights, 0.f, 10.f);
                scaled_offset.v[0] = scale * offset.v[0];
                scaled_offset.v[1] = scale * offset.v[1];
                scaled_offset.v[2] = scale * offset.v[2];
                *(br_scalar*)((tU8*)pModel->vertices + vertex_offset) = scaled_offset.v[0] + *(br_scalar*)((tU8*)pModel->vertices + vertex_offset);
                *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 4) = scaled_offset.v[1] + *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 4);
                *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 8) = scaled_offset.v[2] + *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 8);
            }
            i++;
            pWeights++;
            vertex_offset += sizeof(br_vertex);
        } while (i < pModel->nvertices);
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        ((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
}

/* 0x0043bf80 is ARApplyModelMash (13-crush1.c) */
void C2_HOOK_FASTCALL WorkerModelMash(int field_0x4, tU8* pField_0x8) {

    NOT_IMPLEMENTED();
}

extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);

// based on ApexWorm/rec2 85077822b
// FUNCTION: CARMA2_HW 0x0043c030
void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8) {
    br_model* model;
    tCompressed_vector3* src;
    tU16 i;
    br_vector3 delta;

    model = (br_model*)field_0x4;
    src = (tCompressed_vector3*)pField_0x8;
    for (i = 0; i < model->nvertices; i++) {
        ExpandVector3(&delta, &src[i], -10.0f, 10.0f);
        BrVector3Sub(&model->vertices[i].p, &model->vertices[i].p, &delta);
    }
    if ((model->flags & BR_MODF_CUSTOM) != 0 && model->user != NULL) {
        ((tUser_detail_level_model*)model->user)->field_0x0 = 1;
    } else {
        model->custom = JitModelUpdate;
        model->flags |= BR_MODF_CUSTOM;
    }
}

// based on ApexWorm/rec2 15c72a550
// FUNCTION: CARMA2_HW 0x0043c0e0
void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tU16 field_0x8) {
    br_actor* actor;
    tUser_crush_data* user;
    tUser_crush_data* parent_user;
    br_model* parent_model;
    tCar_crush_flap_data* flap;
    const br_vector3* hinge;
    br_vector3 axis;
    br_scalar nx, ny, nz;

    actor = (br_actor*)field_0x4;
    user = actor->user;
    if (user == NULL || user->crush_data == NULL) {
        return;
    }
    flap = user->crush_data->flap_data;
    if (flap == NULL) {
        return;
    }
    flap->field_0x4 = (tU16)(flap->field_0x4 + field_0x8);
    hinge = &user->models[0]->vertices[flap->hinge0].p;
    BrVector3Sub(&axis, &user->models[0]->vertices[flap->hinge1].p, hinge);
    BrVector3Normalise(&axis, &axis);
    nx = -hinge->v[0];
    ny = -hinge->v[1];
    nz = -hinge->v[2];
    BrMatrix34Translate(&actor->t.t.mat, nx, ny, nz);
    BrMatrix34PostRotate(&actor->t.t.mat, flap->field_0x4, &axis);
    parent_user = (tUser_crush_data*)actor->parent->user;
    parent_model = parent_user->models[0];
    BrMatrix34PostTranslate(&actor->t.t.mat, parent_model->vertices[flap->field_0xc].p.v[0], parent_model->vertices[flap->field_0xc].p.v[1], parent_model->vertices[flap->field_0xc].p.v[2]);
}

// based on ApexWorm/rec2 97b666b11
// RESIDUAL effective 100% (raw ~99): one x87 scheduler slot - the actor load [edi+0xec] for component 2 is emitted before fld here, after fadd in the original; macro/3-statement/pointer-temp forms all fail
// FUNCTION: CARMA2_HW 0x00452880
void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa) {
    tDrone_spec* drone;
    tDrone_path_node_section* section;
    br_vector3 direction;

    drone = &gDrone_specs[field_0x0];
    section = (tDrone_path_node_section*)field_0x4;
    drone->field_0xe4 = DRU16ToScalar(field_0x8, -500.0f, 500.0f);
    drone->field_0xe0 = 0.0f;
    BrVector3Scale(&direction, &section->field_0x10, section->field_0x1c - DRU16ToScalar(field_0xa, 0.0f, 255.0f));
    BrVector3Negate(&direction, &direction);
    BrVector3Add(&drone->actor->t.t.translate.t, &gDrone_path_nodes[section->node1].position, &direction);
    BrVector3Copy(&direction, &section->field_0x10);
    if (drone->form->flags & 0x10) {
        direction.v[1] = 0.0f;
    }
    PointActorAlongThisBloodyVector(drone->actor, &direction);
    DoDroneFunkyGroovyThings(drone);
}

// based on ApexWorm/rec2 1e59f1e54
// FUNCTION: CARMA2_HW 0x004529d0
void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, tU16 field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa) {
    tDrone_spec* drone;

    drone = &gDrone_specs[field_0x0];
    drone->field_0xe4 = DRU16ToScalar(field_0x4, -500.0f, 500.0f);
    drone->field_0xe0 = DRU16ToScalar(field_0x6, 0.0f, 1.0f);
    drone->field_0xe8 = DRU16ToScalar(field_0x8, 0.0f, 100.0f);
    drone->actor->t.t.mat.m[0][0] = DRU16ToScalar(pField_0xa[0], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[0][1] = DRU16ToScalar(pField_0xa[1], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[0][2] = DRU16ToScalar(pField_0xa[2], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[1][0] = DRU16ToScalar(pField_0xa[3], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[1][1] = DRU16ToScalar(pField_0xa[4], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[1][2] = DRU16ToScalar(pField_0xa[5], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[2][0] = DRU16ToScalar(pField_0xa[6], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[2][1] = DRU16ToScalar(pField_0xa[7], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[2][2] = DRU16ToScalar(pField_0xa[8], -1.0f, 1.0f);
    drone->actor->t.t.mat.m[3][0] = DRU16ToScalar(pField_0xa[9], -1000.0f, 1000.0f);
    drone->actor->t.t.mat.m[3][1] = DRU16ToScalar(pField_0xa[10], -1000.0f, 1000.0f);
    drone->actor->t.t.mat.m[3][2] = DRU16ToScalar(pField_0xa[11], -1000.0f, 1000.0f);
    DoDroneFunkyGroovyThings(drone);
}

// based on ApexWorm/rec2 36e8ff344
// FUNCTION: CARMA2_HW 0x004b7220
void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4) {
    tPhysics_object* object;

    object = (tPhysics_object*)field_0x0;
    BrMatrix34Copy(&object->transform_matrix, pField_0x4);
    BrMatrix34Copy(&object->actor->t.t.mat, pField_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9070
void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale) {
    tCar_spec* car;
    tU8* a;
    tU32 code;
    int i;
    tU8* esi;

    code = *(tU32*)*pChunk;
    if ((code & 0xffffff00) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(code >> 8, code & 0xff);
    }
    a = *(tU8**)((tU8*)car + 0xe10);
    i = 0;
    if (*(tU16*)((tU8*)*pChunk + 4) <= 0) {
        return;
    }
    esi = *(tU8**)(a + 0x18);
    do {
        tU32* colors = *(tU32**)(*(tU8**)(*(tU8**)(esi + 0x4c) + 0x18) + 0x14);
        tU16 idx = *(tU16*)((tU8*)*pChunk + i * 4 + 0xc);
        tS8 bright = (tS8)((tS8)*(tU8*)((tU8*)*pChunk + i * 4 + 0xe) * (tS8)pScale);
        tU32 alpha24 = ((tU32)(tU8)bright) << 24;
        tU32 c = colors[idx];

        colors[idx] = ((c & 0xff000000) + alpha24) & 0xff000000;
        if (*(tU8*)(esi + 0x20) & 0x80) {
            tU8* q = *(tU8**)(esi + 0x4c);
            tU8* w = *(tU8**)(q + 0x18);
            tU32* colors2 = (tU32*)(*(tU8**)(w + 0x14));
            tU16* idx2 = (tU16*)(*(tU8**)(w + 0x18));
            tU8 dl = (tU8)(colors2[idx] >> 24) + (tU8)bright;
            tU8* arr = *(tU8**)(esi + 8);
            tU16 ix = idx2[idx];

            arr[(tU32)ix * 40 + 0x14] = dl;
        }
        i++;
    } while (i < *(tU16*)((tU8*)*pChunk + 4));
}

// FUNCTION: CARMA2_HW 0x004ca4d0
void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir) {
    extern int gBOOL_00744804;
    extern tPedestrian* gPedestrian_array;
    extern br_vector3 gY_unit_vector;
    extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

    if (!gBOOL_00744804) {
        SetCharacterDirection(gPedestrian_array[field_0x0 & 0xffff].character, pDir, &gY_unit_vector);
    }
}

// FUNCTION: CARMA2_HW 0x004ca500
void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38) {
    extern int gBOOL_00744804;
    extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);
    extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);
    extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
    int current_move_id;

    if (!gBOOL_00744804) {
        if (field_0x4 != -1) {
            current_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
            BrMatrix34Copy(&pPed->character->field_0x8c, (br_matrix34*)pField_0x38);
            SetCharacterMove(pPed->character, field_0x4, *(float*)&field_0xc, field_0x14, field_0x8_1, GetTotalTime());
            if (!(pPed->character->field_0x14 & 4)) {
                SetCharacterPosition(pPed->character, pField_0x24, -1);
            }
            if (current_move_id != field_0x4) {
                pPed->character->field_0x1c = (tU16)zero;
                SetCharacterBonePositions(pPed->character, 0, 0);
            }
        }
        if (field_0x34 != -1) {
            pPed->action = (tPed_action)field_0x34;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca5a0
void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38) {
    extern int gBOOL_00744804;
    extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);
    extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);
    extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
    int old_move_id;

    if (gBOOL_00744804 != 0) {
        return;
    }
    if (field_0x6 != -1) {
        old_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
        BrMatrix34Copy(&pPed->character->field_0x8c, (br_matrix34*)pField_0x38);
        SetCharacterMove(pPed->character, field_0x6, *(float*)&field_0x10, zero2, zero, GetTotalTime());
        if ((pPed->character->field_0x14 & 4) == 0) {
            SetCharacterPosition(pPed->character, pField_0x18, -1);
        }
        if (old_move_id != field_0x6) {
            pPed->character->field_0x1c = (tU16)field_0xa;
            SetCharacterBonePositions(pPed->character, 0, 0);
            pPed->character->field_0x24 = GetTotalTime();
        }
    }
    if (field_0x30 != -1) {
        pPed->action = (tPed_action)field_0x30;
    }
}

// adapted from the disassembly
// FUNCTION: CARMA2_HW 0x004ca650
void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4) {
    extern int gBOOL_00744804;
    extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);

    if (!gBOOL_00744804) {
        SetCharacterMove(((tPedestrian*)field_0x0)->character, -1, *(float*)&field_0x4.value, 0, 0, GetTotalTime());
    }
}

// FUNCTION: CARMA2_HW 0x004ca680
void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {
    extern int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);
    extern int C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);
    extern void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (field_0x5 != 0xff) {
        if (field_0x5 != 0) {
            *(tU8*)&ped->flags |= 1;
            MakeCharacterRenderable2(ped->character, (int)(tS8)field_0x9);
        } else {
            *(tU8*)&ped->flags &= 0xfe;
            CharacterNoLongerRenderable(ped->character);
        }
    }
    if (field_0x7 != 0xff) {
        ped->character->field_0x14 = (int)(tS8)field_0x7;
        ped->character->field_0x5 = field_0xb;
        if ((field_0x6 & 4) != 0 && (field_0x7 & 4) == 0) {
            RecalculateOrientationOfRoot(ped->character);
        }
    }
}

// Flag byte of the pedestrian (offset 8) is written through a signed-short view: that is the only
// form the original compiler lowers to the direct `or/and BYTE PTR [esi+8]` (an unsigned byte or
// word view gives a load/or/store pair). Block 2 keeps the original's early return and shared tail.
// FUNCTION: CARMA2_HW 0x004ca6e0
void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {
    extern int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);
    extern int C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);
    extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);
    extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (field_0x4 != 0xff) {
        if (field_0x4 != 0) {
            ((tS16*)ped)[4] |= 1;
            MakeCharacterRenderable2(ped->character, (int)(tS8)field_0x8);
        } else {
            ((tS16*)ped)[4] &= ~1;
            CharacterNoLongerRenderable(ped->character);
        }
    }
    if (field_0x6 == 0xff) {
        return;
    }
    if ((field_0x6 & 4) == 0) {
        if ((field_0x7 & 4) != 0) {
            ped->character->field_0x14 = (int)(tS8)field_0x6;
            ped->character->field_0x5 = field_0xa;
            SetCharacterDirection(ped->character, p0xc, p0x18);
            BrMatrix34Copy(&ped->character->field_0x8c, (br_matrix34*)p0x24);
            SetCharacterBonePositions(ped->character, 0, 0);
            return;
        }
    } else if ((field_0x7 & 4) != 0) {
        goto update_state;
    }
    if ((field_0x6 & 4) != 0) {
        BrMatrix34Copy(&ped->character->field_0x8c, (br_matrix34*)p0x24);
        SetCharacterBonePositions(ped->character, 0, 0);
        ped->character->field_0x14 = (int)(tS8)field_0x6;
        ped->character->field_0x5 = field_0xa;
        return;
    }
update_state:
    ped->character->field_0x14 = (int)(tS8)field_0x6;
    ped->character->field_0x5 = field_0xa;
}

// FUNCTION: CARMA2_HW 0x004ca7d0
void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix) {
    extern tPedestrian* gPedestrian_array;
    extern br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter);
    extern void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);
    extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
    tPedestrian* ped;

    ped = &gPedestrian_array[field_0x0 & 0xffff];
    BrMatrix34Copy(GetCharacterMatrixPtr(ped->character), pMatrix);
    ped->pos.v[0] = pMatrix->m[3][0];
    ped->pos.v[1] = pMatrix->m[3][1];
    ped->pos.v[2] = pMatrix->m[3][2];
    RecalculateOrientationOfRoot(ped->character);
    SetCharacterBonePositions(ped->character, 3, 0);
}

// FUNCTION: CARMA2_HW 0x004ca830
void C2_HOOK_FASTCALL WorkerPedPos(int field_0x0, br_vector3* pVec) {
    extern int gBOOL_00744804;
    extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (gBOOL_00744804 != 0) {
        return;
    }
    if (pVec->v[0] == 0.0f && pVec->v[1] == 0.0f && pVec->v[2] == 0.0f) {
        ped->flags |= 0x80;
        return;
    }
    SetCharacterPosition(ped->character, pVec, -1);
    if (ped->flags & 0x80) {
        ped->flags &= ~0x80;
    }
}

// FUNCTION: CARMA2_HW 0x004ca890
void C2_HOOK_FASTCALL DoVanishDismembered(uintptr_t pOwner, tS32 value) {
    *((tS32*)(*(undefined4**)pOwner) + 4) = value;
}

// FUNCTION: CARMA2_HW 0x004ca8a0
void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa) {
    extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, undefined4 pArg2, int pArg3);
    extern void C2_HOOK_FASTCALL FUN_0040b500(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4);
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (field_0x4 < 0) {
        SetCharacterAllBonesModel(ped->character, -1, field_0xa);
    } else {
        FUN_0040b500(ped->character, field_0x4, field_0xc, field_0xa);
    }
}

// FUNCTION: CARMA2_HW 0x004ca8d0
void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8) {
    extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, undefined4 pArg2, int pArg3);
    extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4);
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (field_0x4 < 0) {
        SetCharacterAllBonesModel(ped->character, field_0x6, field_0x8);
    } else {
        SetCharacterBoneModel(ped->character, field_0x4, field_0x6, field_0x8);
    }
}

// FUNCTION: CARMA2_HW 0x004ca980
void C2_HOOK_FASTCALL WorkerPedDismemberUndo(int field_0x0, int field_0x4, int field_0x6) {
    extern const int gPow2_array[32];
    extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
    tPedestrian* ped;
    tPed_character_instance* character;
    tPed_form_actor_set* actor_set;
    br_actor* actor;

    ped = (tPedestrian*)field_0x0;
    actor_set = &ped->character->personality->form->actor_sets[(tS8)ped->character->field_0x4];
    ped->character->field_0xc &= ~gPow2_array[field_0x4];
    ped->character->field_0x10 &= ~gPow2_array[field_0x4];
    ped->character->field_0x6 = *(tS8*)&field_0x6;
    character = ped->character;
    SetCharacterBonePositions(character, 3, 0);
    actor = actor_set->actors[field_0x4];
    if (actor->parent != NULL) {
        BrActorRemove(actor);
    }
}

// FUNCTION: CARMA2_HW 0x004ca900
void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7) {
    extern const int gPow2_array[32];
    extern tPhysics_object* C2_HOOK_FASTCALL MakeCharacterPhysicsForBone(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, undefined4 pArg4);
    tPedestrian* ped;
    tPed_character_instance* character;
    br_actor* actor;

    ped = (tPedestrian*)field_0x0;
    ped->character->field_0xc |= gPow2_array[field_0x4];
    ped->character->field_0x6 = *(tS8*)&field_0x7;
    MakeCharacterPhysicsForBone(ped->character, field_0x4, 1, 0);
    character = ped->character;
    actor = character->personality->form->actor_sets[(tS8)character->field_0x4].actors[field_0x4];
    if (actor->parent == NULL && (field_0x4 == 0 || (gPow2_array[field_0x4] & character->field_0xc) != 0)) {
        BrActorAdd(gNon_track_actor, actor);
    }
}

extern void C2_HOOK_FASTCALL DoGibShower(int pId, int pTime, int pSeed, int pForce, int pSize_index, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

// FUNCTION: CARMA2_HW 0x004ccae0
void C2_HOOK_FASTCALL WorkerGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c) {

    DoGibShower(field_0x0, field_0xc, field_0x8, field_0x10, field_0x4, pField_0x14, pField_0x20, pField_0x2c);
}

extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);
extern tSmash_vertex gSmash_glass_fragments[200];

// FUNCTION: CARMA2_HW 0x004ccc40
void C2_HOOK_FASTCALL WorkerGibShowerUndo(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c) {
    int i;
    tSmash_vertex* fragment;
    float delta_time;

    DoGibShower(prev_0x0, prev_0xc, prev_0x8, prev_0x10, prev_0x4, prev_0x14, prev_0x20, prev_0x2c);
    delta_time = (float)(field_0xc - prev_0xc);
    for (i = 0, fragment = gSmash_glass_fragments; i < 200; i++, fragment++) {
        if (fragment->end_time != 0 && fragment->field_0x18 == prev_0x0) {
            fragment->field_0x34 = 0;
            DoFragMovement(fragment, GetTotalTime(), delta_time);
            fragment->field_0x34 = 1;
        }
    }
}

// based on ApexWorm/rec2 612dc29dc (reference only)
// FUNCTION: CARMA2_HW 0x004d6180
void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8) {
    extern tProximity_ray gProximity_rays[20];
    extern tPedestrian* gPedestrian_array;
    tCar_spec* car;

    if ((field_0x6 >> 8) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(field_0x6 >> 8, field_0x6 & 0xff);
    }
    gProximity_rays[field_0x0].car = car;
    gProximity_rays[field_0x0].ped = &gPedestrian_array[field_0x4];
    gProximity_rays[field_0x0].start_time = field_0x8;
}

// FUNCTION: CARMA2_HW 0x004d6db0
void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144) {
}

// FUNCTION: CARMA2_HW 0x004da960
void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc) {
    extern tRepulse_link gRepulse_links[6];

    gRepulse_links[field_0x0].pos_origin = (br_vector3*)field_0x4;
    gRepulse_links[field_0x0].pos_victim = (br_vector3*)field_0x8;
    gRepulse_links[field_0x0].time = (tU32)field_0xc;
}

// based on ApexWorm/rec2 5b75cde93
// FUNCTION: CARMA2_HW 0x004de000
void C2_HOOK_FASTCALL WorkerShitMine(int field_0x0, br_vector3* pPos) {
    br_actor* actor;

    actor = (br_actor*)field_0x0;
    if (actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, actor);
    }
    actor->render_style = BR_RSTYLE_FACES;
}

// based on ApexWorm/rec2 fdf25dcef
// FUNCTION: CARMA2_HW 0x004de020
void C2_HOOK_FASTCALL WorkerEndShitMine(int field_0x0, br_vector3* pPos) {
    br_actor* actor;

    actor = (br_actor*)field_0x0;
    if (actor->parent != NULL) {
        BrActorRemove(actor);
    }
    actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004df200
void C2_HOOK_FASTCALL WorkerPowerupGot(int field_0x0, int field_0x4) {
    tPowerup* powerup;
    tCar_spec* car;

    powerup = (tPowerup*)field_0x0;
    car = (tCar_spec*)field_0x4;
    if (powerup->got_proc != NULL) {
        powerup->got_proc(powerup, car);
    }
}

// FUNCTION: CARMA2_HW 0x004df210
void C2_HOOK_FASTCALL WorkerPowerupLose(int field_0x0, int field_0x4) {
    tPowerup* powerup;
    tCar_spec* car;

    powerup = (tPowerup*)field_0x0;
    car = (tCar_spec*)field_0x4;
    if (powerup->lose_proc != NULL) {
        powerup->lose_proc(powerup, car);
    }
}

// based on ApexWorm/rec2 b70251db0
// FUNCTION: CARMA2_HW 0x004e0710
void C2_HOOK_FASTCALL WorkerSpecial(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_FACES;
    }
}

void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004e07d0
void C2_HOOK_FASTCALL WorkerPowerupRespawn(int field_0x0, int field_0x4) {
    ARDoPowerupRespawn((br_actor*)(uintptr_t)field_0x0);
}

void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e07e0
void C2_HOOK_FASTCALL WorkerPowerupRespawnUndo(int field_0x0, int field_0x4) {
    ARUndoPowerupRespawn((br_actor*)(uintptr_t)field_0x0);
}

// FUNCTION: CARMA2_HW 0x004eb8f0
void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 pOwner, tU32 pShrapnel, tU32 pSeed, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, tU32 pActor, tU32 pTime, tU32 pArg9, br_vector3* pTransform, br_vector3* pFragment_owner, br_vector3* pPosition, tU32 pDistance, br_vector3* pArg14, tU32 pArg15) {
    extern int gINT_006a3334;
    extern br_matrix34 gShrapnel_shower_matrix;
    extern br_vector3 gVector3_006a4688;
    extern br_vector3 gVector3_00661678;
    extern br_vector3 gZero_vector__smash;
    extern float gYon_squared;
    extern int gINT_006a8294;
    extern int gCount_shrapnel_shower_calls;
    extern int gMax_count_non_cars;
    extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority);
    extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8);
    extern void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
    extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);
    extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
    extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
    extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
    extern void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform);
    extern void C2_HOOK_FASTCALL FUN_004f8b50(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials);
    extern void C2_HOOK_FASTCALL FUN_004c8940(uintptr_t pOwner);
    extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, br_scalar pMax_distance);
    extern void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);
    float fVar1;
    undefined4 uVar2;
    int iVar3;
    int iVar4;
    tU32 uVar5;
    int iVar6;
    int iVar7;
    int iVar8;
    undefined4 uVar9;
    undefined4 uVar10;
    undefined4 uVar11;
    int* pShrapnel_spec;
    tU8* local_c8;
    int local_c4;
    int local_c0;
    int local_a4;
    tU32 local_70;
    br_matrix34 basis;
    br_matrix34 scale_matrix;
    br_matrix34 emission_transform;
    br_vector3 initial_velocity;
    br_vector3 initial_omega;
    float width;
    float height;

    pShrapnel_spec = (int*)pShrapnel;
    if (gAction_replay_mode != 0) {
        BrMatrix34Copy(&basis, (br_matrix34*)pTransform);
    } else if (((*pShrapnel_spec == 2) || (*pShrapnel_spec == 3)) && (pShrapnel_spec[10] == 2) && (gINT_006a3334 != 0)) {
        basis.m[0][0] = gShrapnel_shower_matrix.m[1][0] - gShrapnel_shower_matrix.m[0][0];
        basis.m[0][1] = gShrapnel_shower_matrix.m[1][1] - gShrapnel_shower_matrix.m[0][1];
        fVar1 = gShrapnel_shower_matrix.m[1][2] - gShrapnel_shower_matrix.m[0][2];
        basis.m[0][2] = (float)sqrt(fVar1 * fVar1 + basis.m[0][1] * basis.m[0][1] + basis.m[0][0] * basis.m[0][0]);
        if (basis.m[0][2] <= 2.3841858e-07f) {
            basis.m[0][0] = 1.0f;
            basis.m[0][1] = 0.0f;
            basis.m[0][2] = 0.0f;
        } else {
            basis.m[0][2] = (float)(1.0 / basis.m[0][2]);
            basis.m[0][0] = basis.m[0][2] * basis.m[0][0];
            basis.m[0][1] = basis.m[0][2] * basis.m[0][1];
            basis.m[0][2] = basis.m[0][2] * fVar1;
        }
        basis.m[1][0] = gShrapnel_shower_matrix.m[2][0] - gShrapnel_shower_matrix.m[0][0];
        basis.m[1][1] = gShrapnel_shower_matrix.m[2][1] - gShrapnel_shower_matrix.m[0][1];
        fVar1 = gShrapnel_shower_matrix.m[2][2] - gShrapnel_shower_matrix.m[0][2];
        basis.m[1][2] = (float)sqrt(fVar1 * fVar1 + basis.m[1][1] * basis.m[1][1] + basis.m[1][0] * basis.m[1][0]);
        if (basis.m[1][2] <= 2.3841858e-07f) {
            basis.m[1][0] = 1.0f;
            basis.m[1][1] = 0.0f;
            basis.m[1][2] = 0.0f;
        } else {
            basis.m[1][2] = (float)(1.0 / basis.m[1][2]);
            basis.m[1][0] = basis.m[1][2] * basis.m[1][0];
            basis.m[1][1] = basis.m[1][2] * basis.m[1][1];
            basis.m[1][2] = basis.m[1][2] * fVar1;
        }
        basis.m[2][2] = (float)sqrt(gVector3_006a4688.v[2] * gVector3_006a4688.v[2]
                + gVector3_006a4688.v[1] * gVector3_006a4688.v[1]
                + gVector3_006a4688.v[0] * gVector3_006a4688.v[0]);
        if (basis.m[2][2] <= 2.3841858e-07f) {
            basis.m[2][0] = 1.0f;
            basis.m[2][1] = 0.0f;
            basis.m[2][2] = 0.0f;
        } else {
            basis.m[2][2] = (float)(1.0 / basis.m[2][2]);
            basis.m[2][0] = basis.m[2][2] * gVector3_006a4688.v[0];
            basis.m[2][1] = basis.m[2][2] * gVector3_006a4688.v[1];
            basis.m[2][2] = basis.m[2][2] * gVector3_006a4688.v[2];
        }
        basis.m[3][0] = gShrapnel_shower_matrix.m[1][0];
        basis.m[3][1] = gShrapnel_shower_matrix.m[1][1];
        basis.m[3][2] = gShrapnel_shower_matrix.m[1][2];
    }
    switch (*pShrapnel_spec) {
    case 0:
        if (gAction_replay_mode != 0 || PointOutOfSight(pArg6, gYon_squared) == 0) {
            if (pPosition == NULL) {
                pPosition = &gVector3_00661678;
            }
            FUN_004f8b50(pArg6, &gZero_vector__smash, pPosition, (float)sqrt(*(float*)&pDistance) * 10.0f, pShrapnel_spec[0xe], (br_material**)(pShrapnel_spec + 0xf));
        }
        break;
    case 1:
        if (gAction_replay_mode != 0 || PointOutOfSight(pArg6, gYon_squared) == 0) {
            if (pOwner == 0) {
                uVar9 = 0;
                uVar10 = 0;
                uVar11 = 0;
                uVar2 = 0;
            } else {
                uVar9 = *(undefined4*)(pOwner + 0x70);
                uVar11 = *(undefined4*)(pOwner + 0x6c);
                uVar10 = *(undefined4*)(pOwner + 0x78);
                uVar2 = *(undefined4*)(pOwner + 0x74);
            }
            CreateSmashedGlassFragments(pSeed, pArg4, (br_actor*)pActor, pTime, (int)uVar2, (const tU16*)uVar10, (int)uVar11, (const tU16*)uVar9, pShrapnel_spec[0xe], pShrapnel_spec[0x10], *(float*)(pShrapnel_spec + 0xf), (tSmashable_initial_speed_spec*)(pShrapnel_spec + 1), (const int*)(pShrapnel_spec + 8), pArg9, (br_matrix34*)pTransform);
        }
        break;
    case 2:
        if (gAction_replay_mode != 0 || PointOutOfSight(pArg6, gYon_squared) == 0) {
            local_c8 = gINT_006a3334 != 0 ? (tU8*)&basis : NULL;
            uVar5 = rand();
            srand(pSeed);
            iVar3 = pShrapnel_spec[0xe];
            if (iVar3 >= 0) {
                iVar4 = IRandomBetween(iVar3, pShrapnel_spec[0xf]);
            } else {
                iVar4 = pShrapnel_spec[0x10];
            }
            if (local_c8 != NULL && *(float*)&pShrapnel_spec[0x11] != 0.0f && *(float*)&pShrapnel_spec[0x12] != 0.0f) {
                width = (float)sqrt((gShrapnel_shower_matrix.m[1][0] - gShrapnel_shower_matrix.m[0][0]) * (gShrapnel_shower_matrix.m[1][0] - gShrapnel_shower_matrix.m[0][0])
                        + (gShrapnel_shower_matrix.m[1][1] - gShrapnel_shower_matrix.m[0][1]) * (gShrapnel_shower_matrix.m[1][1] - gShrapnel_shower_matrix.m[0][1])
                        + (gShrapnel_shower_matrix.m[1][2] - gShrapnel_shower_matrix.m[0][2]) * (gShrapnel_shower_matrix.m[1][2] - gShrapnel_shower_matrix.m[0][2]));
                height = (float)sqrt((gShrapnel_shower_matrix.m[2][0] - gShrapnel_shower_matrix.m[0][0]) * (gShrapnel_shower_matrix.m[2][0] - gShrapnel_shower_matrix.m[0][0])
                        + (gShrapnel_shower_matrix.m[2][1] - gShrapnel_shower_matrix.m[0][1]) * (gShrapnel_shower_matrix.m[2][1] - gShrapnel_shower_matrix.m[0][1])
                        + (gShrapnel_shower_matrix.m[2][2] - gShrapnel_shower_matrix.m[0][2]) * (gShrapnel_shower_matrix.m[2][2] - gShrapnel_shower_matrix.m[0][2]));
                if (width < 0.001 || height < 0.001) {
                    break;
                }
                BrMatrix34Scale(&scale_matrix, width / *(float*)&pShrapnel_spec[0x11], height / *(float*)&pShrapnel_spec[0x12], 1.0f);
                BrMatrix34Mul(&emission_transform, &scale_matrix, &basis);
                local_c8 = (tU8*)&emission_transform;
            }
            iVar6 = 0;
            if (iVar4 > 0) {
                do {
                    if (iVar3 >= 0) {
                        iVar7 = IRandomBetween(0, pShrapnel_spec[0x10] - 1);
                        local_c4 = *(int*)((tU8*)pShrapnel_spec[0x13] + iVar7 * 4);
                    } else {
                        local_c4 = *(int*)((tU8*)pShrapnel_spec[0x13] + iVar6 * 4);
                    }
                    iVar7 = (int)FindShrapnelFragment(((br_actor*)local_c4)->model, pArg9, 4);
                    if (iVar7 == 0) {
                        break;
                    }
                    fVar1 = PositionFragment(((tSmash_vertex*)iVar7)->actor, (tSmashable_initial_position_spec*)(pShrapnel_spec + 10), local_c4, (int)pArg5, (int)pArg6, (br_vector3*)pActor, (br_vector3*)pFragment_owner, (int)local_c8);
                    ApplyInitialMovement((undefined4*)(pShrapnel_spec + 1), pArg4, &((tSmash_vertex*)iVar7)->v, &((tSmash_vertex*)iVar7)->omega, fVar1, &gZero_vector__smash, pArg14, &((tSmash_vertex*)iVar7)->actor->t.t.translate.t);
                    ((tSmash_vertex*)iVar7)->field_0xc = pTime;
                    ((tSmash_vertex*)iVar7)->time_last_move = pTime;
                    ((tSmash_vertex*)iVar7)->end_time = pTime + pShrapnel_spec[9];
                    iVar8 = IRandomBetween(pShrapnel_spec[8], pShrapnel_spec[9]);
                    iVar6++;
                    ((tSmash_vertex*)iVar7)->field_0x10 = iVar8 + pTime;
                } while (iVar6 < iVar4);
            }
            srand(uVar5);
        }
        break;
    case 3:
        local_a4 = gINT_006a3334 != 0 ? (int)&basis : 0;
        local_c0 = 0;
        if (gNet_mode != 3 || gINT_006a8294 != 0) {
            iVar3 = pShrapnel_spec[0xe];
            if (iVar3 >= 0) {
                iVar4 = IRandomBetween(iVar3, pShrapnel_spec[0xf]);
            } else {
                iVar4 = pShrapnel_spec[0x10];
            }
            local_c4 = 0;
            if (iVar4 > 0) {
                do {
                    uVar5 = rand();
                    local_70 = uVar5;
                    if (gMax_count_non_cars <= gProgram_state.track_spec.count_non_cars) {
                        break;
                    }
                    if (gNet_mode == 2) {
                        local_c0 = (int)NetBuildGuaranteedMessage(0x22, 0);
                        *(tU32*)(local_c0 + 0x1c) = uVar5;
                        *(tU16*)(local_c0 + 0x20) = (tU16)gProgram_state.track_spec.count_non_cars;
                    }
                    if (iVar3 >= 0) {
                        iVar6 = IRandomBetween(0, pShrapnel_spec[0x10] - 1);
                        iVar6 = *(int*)((tU8*)pShrapnel_spec[0x11] + iVar6 * 4);
                    } else {
                        iVar6 = *(int*)((tU8*)pShrapnel_spec[0x11] + local_c4 * 4);
                    }
                    if (gNet_mode == 2) {
                        *(tU16*)(local_c0 + 0x22) = *(tU16*)(*(int*)(iVar6 + 0x14) + 1);
                    }
                    iVar6 = (int)CloneActor((br_actor*)iVar6);
                    sprintf((char*)(*(int*)(iVar6 + 0x14) + 4), "%04d.ACT", gProgram_state.track_spec.count_non_cars);
                    gProgram_state.track_spec.non_car_list[gProgram_state.track_spec.count_non_cars] = (br_actor*)iVar6;
                    gProgram_state.track_spec.count_non_cars++;
                    BrActorAdd(gDont_render_actor, (br_actor*)iVar6);
                    fVar1 = PositionFragment((br_actor*)iVar6, (tSmashable_initial_position_spec*)(pShrapnel_spec + 10), iVar6, (int)pArg5, (int)pArg6, (br_vector3*)pActor, (br_vector3*)pFragment_owner, local_a4);
                    if (*(int*)(iVar6 + 0x5c) != 0 || (iVar7 = (int)DoPullActorFromWorld((br_actor*)iVar6)) == 0) {
                        BrActorRemove((br_actor*)iVar6);
                        BrActorFree((br_actor*)iVar6);
                        gProgram_state.track_spec.count_non_cars--;
                        break;
                    }
                    uVar5 = rand();
                    ApplyInitialMovement((undefined4*)(pShrapnel_spec + 1), pArg4, &initial_velocity, &initial_omega, fVar1, &gZero_vector__smash, pArg14, &((br_actor*)iVar6)->t.t.translate.t);
                    *(float*)(*(int*)(iVar7 + 8) + 0x68) = initial_velocity.v[0] * 0.14492753;
                    *(float*)(*(int*)(iVar7 + 8) + 0x6c) = initial_velocity.v[1] * 0.14492753;
                    *(float*)(*(int*)(iVar7 + 8) + 0x70) = initial_velocity.v[2] * 0.14492753;
                    *(float*)(*(int*)(iVar7 + 8) + 0x74) = initial_omega.v[0] * 6.2831855;
                    *(float*)(*(int*)(iVar7 + 8) + 0x78) = initial_omega.v[1] * 6.2831855;
                    *(float*)(*(int*)(iVar7 + 8) + 0x7c) = initial_omega.v[2] * 6.2831855;
                    *(tU8*)(*(int*)(iVar7 + 8) + 0xec) = 0;
                    srand(local_70);
                    iVar8 = PercentageChance(pShrapnel_spec[0x12]);
                    if (iVar8 != 0 && pShrapnel_spec[0x13] > 0) {
                        iVar8 = 0;
                        do {
                            uVar9 = IRandomBetween(0, *(tU16*)(*(int*)(*(int*)(*(int*)(iVar6 + 0x18) + 0x4c) + 0x18) + 0x1e));
                            CreateSmokeColumn2(2, (br_actor*)iVar6, (tCar_spec*)iVar7, NULL, IRandomBetween(pShrapnel_spec[0x14], pShrapnel_spec[0x15]), (int)uVar9, 10000000);
                            iVar8++;
                        } while (iVar8 < pShrapnel_spec[0x13]);
                    }
                    if (gNet_mode == 2) {
                        BrMatrix34Copy(&basis, &gShrapnel_shower_matrix);
                        *(undefined4*)(local_c0 + 0x24) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x68);
                        *(undefined4*)(local_c0 + 0x28) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x6c);
                        *(undefined4*)(local_c0 + 0x2c) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x70);
                        *(undefined4*)(local_c0 + 0x30) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x74);
                        *(undefined4*)(local_c0 + 0x34) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x78);
                        *(undefined4*)(local_c0 + 0x38) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x7c);
                        *(int*)(local_c0 + 0x6c) = pShrapnel_spec[0x12];
                        *(int*)(local_c0 + 0x70) = pShrapnel_spec[0x13];
                        *(int*)(local_c0 + 0x74) = pShrapnel_spec[0x14];
                        *(int*)(local_c0 + 0x78) = pShrapnel_spec[0x15];
                    }
                    FUN_004c8940(gProgram_state.track_spec.count_non_cars - 1);
                    if (gNet_mode == 2) {
                        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, (tNet_message*)local_c0, NULL);
                    }
                    srand(uVar5);
                    local_c4++;
                } while (local_c4 < iVar4);
            }
        }
        break;
    }
    if (*pShrapnel_spec == 2) {
        pTransform = (br_vector3*)&basis;
    } else if (*pShrapnel_spec != 1) {
        return;
    }
    PipeSingleShrapnelShower(pArg9, pTime, pSeed, pOwner, pArg4, pPosition, *(float*)&pDistance, pArg5, pArg6, pShrapnel, (br_bounds3*)pFragment_owner, pActor, (br_matrix34*)pTransform, pArg14, &gShrapnel_shower_matrix);
    gCount_shrapnel_shower_calls++;
}

extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);
extern tSmash_vertex gSmash_glass_fragments[200];

// FUNCTION: CARMA2_HW 0x004f0380
void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38) {
    int i;
    tSmash_vertex* fragment;
    float delta_time;

    WorkerShrapnelShower(prev_0x4, prev_0x84, prev_0x44, prev_0x8, prev_0x20, prev_0x2c, prev_0x50, prev_0x48, prev_0x0, prev_0x54, prev_0x88, prev_0x14, prev_0xa0, prev_0x38, 0);
    delta_time = (float)(chunk_0x48 - prev_0x48);
    for (i = 0, fragment = gSmash_glass_fragments; i < 200; i++, fragment++) {
        if (fragment->end_time != 0 && fragment->field_0x18 == prev_0x0) {
            fragment->field_0x34 = 0;
            DoFragMovement(fragment, GetTotalTime(), delta_time);
            fragment->field_0x34 = 1;
        }
    }
}

// based on ApexWorm/rec2 832cae840
// FUNCTION: CARMA2_HW 0x004f5fa0
void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4) {
    br_model* pModel;
    br_material* pMaterial;
    const br_uint_16* source;
    br_face* face;
    int i;

    pModel = (br_model*)field_0x0;
    source = pField_0xc;
    pMaterial = (br_material*)field_0x4;
    face = pModel->faces + pModel->nfaces;
    for (i = 0; i < field_0x8; i++) {
        face->material = pMaterial;
        face->vertices[0] = source[0];
        face->vertices[1] = source[1];
        face->vertices[2] = source[2];
        face->smoothing = source[3];
        face->flags = 0;
        face++;
        source += 4;
    }
    pModel->nfaces += field_0x8;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f8630
void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10) {
    br_vector3* pPos;
    br_vector3* pLength;
    int i;
    br_actor* actor;
    br_vector3 delta;

    pPos = (br_vector3*)pField_0x4;
    pLength = (br_vector3*)pField_0x10;
    i = field_0x0 & 0xff;
    gSpark_flags |= 1 << field_0x0;
    if (gSparks[i].car != NULL) {
        actor = gSparks[i].car->car_master_actor;
        delta.v[0] = pPos->v[0] - actor->t.t.translate.t.v[0];
        delta.v[1] = pPos->v[1] - actor->t.t.translate.t.v[1];
        delta.v[2] = pPos->v[2] - actor->t.t.translate.t.v[2];
        BrMatrix34TApplyV(&gSparks[i].pos, &delta, &actor->t.t.mat);
    } else {
        gSparks[i].pos.v[0] = pPos->v[0];
        gSparks[i].pos.v[1] = pPos->v[1];
        gSparks[i].pos.v[2] = pPos->v[2];
    }
    gSparks[i].length.v[0] = pLength->v[0];
    gSparks[i].length.v[1] = pLength->v[1];
    gSparks[i].length.v[2] = pLength->v[2];
    gSparks[i].colour = field_0x0 >> 8;
}

extern int gShrapnel_flags;
extern tShrapnel gShrapnel[30];

// FUNCTION: CARMA2_HW 0x004f86f0
void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14) {
    int i;

    i = field_0x0 & 0x7fff;
    if ((gShrapnel_flags & (1 << i)) == 0) {
        BrActorAdd(gNon_track_actor, gShrapnel[i].actor);
    }
    gShrapnel_flags |= 1 << i;
    gShrapnel[i].actor->t.t.translate.t.v[0] = ((br_vector3*)pField_0x4)->v[0];
    gShrapnel[i].actor->t.t.translate.t.v[1] = ((br_vector3*)pField_0x4)->v[1];
    gShrapnel[i].actor->t.t.translate.t.v[2] = ((br_vector3*)pField_0x4)->v[2];
    if ((field_0x0 & 0x8000) != 0) {
        gShrapnel[i].age = field_0x10;
        gShrapnel[i].actor->material = (br_material*)field_0x14;
    }
}

// based on ApexWorm/rec2 715455cdd
void C2_HOOK_FASTCALL WorkerSmokeColumn(int pIndex, br_vector3* pCore, int pVertex, int pColour) {
    tU32 bit;

    bit = 1 << pIndex;
    gColumn_flags ^= bit;
    gSmoke_column[pIndex].core = *(const tSmoke_column_core*)pCore;
    gSmoke_column[pIndex].vertex_index = pVertex;
    gSmoke_column[pIndex].colour = pColour;
    gSmoke_column[pIndex].frame_count[0] = 100;
    gSmoke_column[pIndex].frame_count[1] = 100;
    gSmoke_column[pIndex].frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[pIndex].flame_actor);
        } else if (gSmoke_column[pIndex].flame_actor->depth != 0) {
            BrActorRemove(gSmoke_column[pIndex].flame_actor);
        }
    }
}

// based on ApexWorm/rec2 64e1bed48
void C2_HOOK_FASTCALL WorkerFlame(int index, int flag, float f1, float f2, float f3, float f4) {
    int i;
    tSmoke_column* col;

    i = index & 0xf;
    col = &gSmoke_column[(index & ~0xf) >> 4];
    col->frame_count[i] = flag;
    col->scale_x[i] = f1;
    col->scale_y[i] = f2;
    col->offset_x[i] = f3;
    col->offset_z[i] = f4;
}

// PARTIAL 65.69%: reg roles (free_slot/i/ptr) and found-path offset hoist differ
// FUNCTION: CARMA2_HW 0x004fef40
void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18) {
    int i;
    int free_slot;
    tReplay_splash* p;
    int* src;

    free_slot = -1;
    i = 0;
    p = gReplay_splashes;
    do {
        if (!(gReplay_splash_flags & (1 << i))) {
            if (free_slot < 0) {
                free_slot = i;
            }
        } else if (p->object.actor == (br_actor*)field_0x0) {
            goto found;
        }
        p++;
        i++;
    } while (p < &gReplay_splashes[10]);
    if (free_slot < 0) {
        free_slot = IRandomBetween(0, 10);
    }
    gReplay_splash_flags |= 1 << free_slot;
    gReplay_splashes[free_slot].object.actor = (br_actor*)field_0x0;
    gReplay_splashes[free_slot].type = field_0x4;
    i = free_slot;
found:
    *(int*)&gReplay_splashes[i].water_density = field_0x8;
    src = (int*)pField_0xc;
    *(int*)&gReplay_splashes[i].normal.v[0] = src[0];
    *(int*)&gReplay_splashes[i].normal.v[1] = src[1];
    *(int*)&gReplay_splashes[i].normal.v[2] = src[2];
    src = (int*)pField_0x18;
    *(int*)&gReplay_splashes[i].bb.min.v[0] = src[0];
    *(int*)&gReplay_splashes[i].bb.min.v[1] = src[1];
    *(int*)&gReplay_splashes[i].bb.min.v[2] = src[2];
    *(int*)&gReplay_splashes[i].bb.max.v[0] = src[3];
    *(int*)&gReplay_splashes[i].bb.max.v[1] = src[4];
    *(int*)&gReplay_splashes[i].bb.max.v[2] = src[5];
}

// based on ApexWorm/rec2 8a2f3d992
// FUNCTION: CARMA2_HW 0x004e0730
void C2_HOOK_FASTCALL WorkerSpecialUndo(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_NONE;
    }
}

extern void C2_HOOK_CDECL BrMatrix34ToTransform(br_transform* xform, const br_matrix34* mat);
extern br_uint_8 C2_HOOK_CDECL ActorToActorMatrix34Local(br_matrix34* m, br_actor* a, br_actor* b);

// FUNCTION: CARMA2_HW 0x0040ca40
void C2_HOOK_CDECL RelinkActorKeepWorldTransform(br_actor* pParent, br_actor* pActor) {
    br_matrix34 mat;

    if (pActor->parent != pParent) {
        ActorToActorMatrix34Local(&mat, pActor, pParent);
        BrMatrix34ToTransform(&pActor->t, &mat);
        BrActorAdd(pParent, BrActorRemove(pActor));
    }
}

extern void C2_HOOK_FASTCALL FUN_0042f5d0(br_model* pModel, br_model** pModels, br_model** pModel_out, br_scalar* pPlane, int pFlag);

// PARTIAL 36.22%: WorkerSplitWeldApply 0x0043c240 - logic complete (inlined MakeModelMaterialsDoubleSided loop); residual: original copies the 4 plane floats via fld/fstp (x87 hoisted), MSVC emits integer movs; frame/regalloc follow.
// FUNCTION: CARMA2_HW 0x0043c240
void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* roadMachine, br_vector3* pVector) {
    tCar_spec* car;
    tCar_crush_spec* crush;
    tUser_crush_data* user;
    br_model* model;
    br_model* swap_model;
    br_vector4 split_plane;
    int i;
    int g;

    car = roadMachine;
    split_plane.v[0] = pVector->v[0];
    split_plane.v[1] = pVector->v[1];
    split_plane.v[2] = pVector->v[2];
    crush = car->car_crush_spec;
    user = car->car_model_actor->user;
    split_plane.v[3] = ((float*)pVector)[3] - (car->car_model_actor->t.t.translate.t.v[1] * split_plane.v[1] + car->car_model_actor->t.t.translate.t.v[0] * split_plane.v[0] + car->car_model_actor->t.t.translate.t.v[2] * split_plane.v[2]);
    FUN_0042f5d0(user->models[0], (br_model**)crush->field_0x4c4, &crush->model_actor->model, split_plane.v, 0);
    i = 1;
    if (1 < car->count_detail_levels) {
        do {
            if (user->models[i] != NULL) {
                FUN_0042f5d0(user->models[i], (br_model**)crush->field_0x4c4 + i, &model, split_plane.v, 1);
            }
            i++;
        } while (i < car->count_detail_levels);
    }
    i = 0;
    if (0 < car->count_detail_levels) {
        do {
            swap_model = user->models[i];
            user->models[i] = ((br_model**)crush->field_0x4c4)[i];
            ((br_model**)crush->field_0x4c4)[i] = swap_model;
            i++;
        } while (i < car->count_detail_levels);
    }
    car->car_model_actor->model = user->models[0];
    DRActorEnumRecurse(car->car_model_actor, MakeCarModelsMaterialsDoubleSided, car);
    model = crush->model_actor->model;
    if (model != NULL) {
        g = 0;
        if (model->prepared->ngroups != 0) {
            do {
                br_material* material = model->faces[*model->prepared->groups[g].face_user].material;

                if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
                    material->flags |= BR_MATF_TWO_SIDED;
                    BrMaterialUpdate(material, BR_MATU_RENDERING);
                }
                g++;
            } while (g < model->prepared->ngroups);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043c3d0
void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine) {
    tCar_spec* car;
    tCar_crush_spec* spec;
    tUser_crush_data* user;
    int i;

    car = roadMachine;
    spec = car->car_crush_spec;
    user = (tUser_crush_data*)car->car_model_actor->user;
    for (i = 0; i < car->count_detail_levels; i++) {
        if (user->models[i] != NULL) {
            BrModelFree(BrModelRemove(user->models[i]));
            user->models[i] = (br_model*)*(undefined4*)&spec->field_0x4c4[i * 4];
        }
    }
    car->car_model_actor->model = user->models[0];
    BrModelFree(BrModelRemove(spec->model_actor->model));
    DRActorEnumRecurse(car->car_model_actor, MakeCarModelsMaterialsSingleSided, car);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// Original is a bare ret (no body to implement).
// based on ApexWorm/rec2 b674ae1d3 (reference only)
// FUNCTION: CARMA2_HW 0x004d6da0
void C2_HOOK_FASTCALL ComparePedStuff(void) {
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00403020
tU32 C2_HOOK_FASTCALL ARGetStartTime(void) {
    extern void C2_HOOK_CDECL FatalError(int pStr_index, ...);
    tU8* session;
    tU32 i;
    tU32 length;
    tU32 total;

    session = gPipe_buffer_oldest;
    for (;;) {
        total = 0;
        for (i = 0; i < ((tPipe_session*)session)->count; i++) {
            tU32 chunk_type;
            tU8* the_chunk;

            chunk_type = (tU32)((tPipe_session*)session)->type;
            the_chunk = session + 8 + total;
            if (gPipe_callbacks[chunk_type].calc_length != NULL) {
                length = gPipe_callbacks[chunk_type].calc_length((tPipe_chunk*)the_chunk);
                length = length + gPipe_callbacks[chunk_type].length + sizeof(void*);
            } else {
                length = gPipe_callbacks[chunk_type].length + sizeof(void*);
            }
            total += length;
        }
        total += 10;
        if ((total & 1) != 0) {
            FatalError(0x62);
        }
        session = session + total;
        if ((uintptr_t)session >= (uintptr_t)gPipe_buffer_working_end && session != gPipe_record_ptr) {
            session = gPipe_buffer_start;
        }
        if (session == gPipe_record_ptr) {
            return 0;
        }
        if (((tPipe_session*)session)->type == 0) {
            return *(tU32*)(session + 12);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a2360
tU32 gUnk_006a2360;

// GLOBAL: CARMA2_HW 0x006a2364
tU32 gUnk_006a2364;

// GLOBAL: CARMA2_HW 0x006a2378
int gUnk_006a2378;

// GLOBAL: CARMA2_HW 0x006a237c
tU32 gUnk_006a237c;

// GLOBAL: CARMA2_HW 0x006a23c8
tU32 gUnk_006a23c8;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// Game-local copy of BrActorToActorMatrix34, with its own transform-combine table.
// FUNCTION: CARMA2_HW 0x0040c660
br_uint_8 C2_HOOK_CDECL ActorToActorMatrix34Local(br_matrix34* m, br_actor* a, br_actor* b) {
    br_uint_8 transform_type_combine[7][7] = {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1, 2, 2 },
        { 0, 1, 1, 1, 1, 3, 3 },
        { 0, 1, 1, 1, 1, 4, 4 },
        { 0, 1, 2, 3, 4, 5, 5 },
        { 0, 1, 2, 3, 4, 5, 6 },
    };
    br_matrix34 mat_a;
    br_matrix34 mat_b;
    br_matrix34 mat_c;
    br_uint_8 a_type;
    br_uint_8 b_type;

    if (a == b) {
        BrMatrix34Identity(m);
        return BR_TRANSFORM_IDENTITY;
    }
    if (a->parent == b) {
        BrTransformToMatrix34(m, &a->t);
        return (br_uint_8)a->t.type;
    }
    if (b->parent == a) {
        BrTransformToMatrix34(&mat_b, &b->t);
        if (b->t.type != BR_TRANSFORM_MATRIX34) {
            BrMatrix34LPInverse(m, &mat_b);
        } else {
            BrMatrix34Inverse(m, &mat_b);
        }
        return (br_uint_8)b->t.type;
    }
    a_type = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mat_a);
    b_type = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mat_b);
    while (a != NULL && b != NULL && a != b) {
        if (a->depth > b->depth) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_a, &a->t);
                a_type = transform_type_combine[a_type][a->t.type];
            }
            a = a->parent;
        } else if (b->depth > a->depth) {
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_b, &b->t);
                b_type = transform_type_combine[b_type][b->t.type];
            }
            b = b->parent;
        } else {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_a, &a->t);
                a_type = transform_type_combine[a_type][a->t.type];
            }
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_b, &b->t);
                b_type = transform_type_combine[b_type][b->t.type];
            }
            b = b->parent;
            a = a->parent;
        }
    }
    if (b_type == BR_TRANSFORM_IDENTITY) {
        BrMatrix34Copy(m, &mat_a);
        return a_type;
    }
    if (a_type == BR_TRANSFORM_IDENTITY) {
        if (b_type != BR_TRANSFORM_MATRIX34) {
            BrMatrix34LPInverse(m, &mat_b);
        } else {
            BrMatrix34Inverse(m, &mat_b);
        }
        return b_type;
    }
    if (b_type != BR_TRANSFORM_MATRIX34) {
        BrMatrix34LPInverse(&mat_c, &mat_b);
    } else {
        BrMatrix34Inverse(&mat_c, &mat_b);
    }
    BrMatrix34Mul(m, &mat_a, &mat_c);
    return transform_type_combine[a_type][b_type];
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern float gDrone_render_zero;
// PARTIAL 82.75%: clamp compares use fcomp mem instead of original fld/fld/fcompp (test ah,0x41)
// FUNCTION: CARMA2_HW 0x00451e70
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone) {
    float delta_time;
    float flt_unk0;
    float flt_unk1;
    int left_turn;
    int i;

    delta_time = gAction_replay_mode ? gFrame_period * 0.001 : gDrone_delta_time;
    flt_unk0 = gAction_replay_mode ? pDrone->field_0xe0 : pDrone->field_0x74;
    flt_unk1 = gAction_replay_mode ? pDrone->field_0xe4 : pDrone->field_0x48;
    if (gAction_replay_mode) {
        if (flt_unk1 < gDrone_render_zero) {
            flt_unk1 = -flt_unk1;
            left_turn = 0;
        } else {
            left_turn = 1;
        }
    } else {
        left_turn = pDrone->left_turn;
    }
    if (pDrone->funk_grooves == NULL) {
        return;
    }
    if (!gAction_replay_mode && (pDrone->current_state == 3 || pDrone->current_state == 4)) {
        return;
    }
    for (i = 0; i < pDrone->funk_grooves->count; i++) {
        tFunk_groove* groove = &pDrone->funk_grooves->items[i];

        switch ((tU8)groove->type) {
        case 0:
            {
                float angle;

                if (groove->spinny.speed_control) {
                    angle = delta_time * flt_unk1 / groove->spinny.omega;
                } else {
                    angle = gDrone_render_zero;
                }
                if (!groove->spinny.reverse) {
                    angle = -angle;
                }
                switch (groove->spinny.axis) {
                case 0:
                    BrMatrix34PreRotateX(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                case 1:
                    BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                case 2:
                    BrMatrix34PreRotateZ(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                }
            }
            break;
        case 1:
            {
                float f;
                float angle;

                if (gAction_replay_mode) {
                    f = (10.f - (10.f < (pDrone->field_0xe8 >= gDrone_render_zero ? pDrone->field_0xe8 : gDrone_render_zero) ? 10.f : (pDrone->field_0xe8 >= gDrone_render_zero ? pDrone->field_0xe8 : gDrone_render_zero))) * 0.014285714f;
                } else if (pDrone->field_0xdc == 2) {
                    f = (10.f - (10.f < (pDrone->h_radius >= gDrone_render_zero ? pDrone->h_radius : gDrone_render_zero) ? 10.f : (pDrone->h_radius >= gDrone_render_zero ? pDrone->h_radius : gDrone_render_zero))) * 0.014285714f;
                } else {
                    f = gDrone_render_zero;
                }
                if (flt_unk0 < 0.5) {
                    angle = flt_unk0;
                } else {
                    angle = 1.f - flt_unk0;
                }
                angle = angle * f;
                angle = angle + angle;
                if (groove->steering.reverse) {
                    angle = -angle;
                }
                if (left_turn) {
                    angle = -angle;
                }
                groove->actor->t.t.mat.m[0][0] = 1.f;
                groove->actor->t.t.mat.m[0][1] = 0.f;
                groove->actor->t.t.mat.m[0][2] = 0.f;
                groove->actor->t.t.mat.m[1][0] = 0.f;
                groove->actor->t.t.mat.m[1][1] = 1.f;
                groove->actor->t.t.mat.m[1][2] = 0.f;
                groove->actor->t.t.mat.m[2][0] = 0.f;
                groove->actor->t.t.mat.m[2][1] = 0.f;
                groove->actor->t.t.mat.m[2][2] = 1.f;
                BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                if (pDrone->field_0xdc == 2) {
                    DoNotDprintf("STEERING GROOVE: rot %f, h_radius %f, reverse %d, left_turn %d", angle, pDrone->h_radius, groove->steering.reverse, pDrone->left_turn);
                }
            }
            break;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// RESIDUAL 97.44% (reccmp: effective 100): one symmetric SIB encoding differs at 0x4085c5 ([eax+esi] vs [esi+eax]); operand swap in the source flips the return-path load too, so it is the allocator's choice
// FUNCTION: CARMA2_HW 0x00408580
tPhysics_object* C2_HOOK_FASTCALL MakeCharacterPhysicsForBone(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, undefined4 pArg4) {
    extern int C2_HOOK_FASTCALL MakeCharacterPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, br_actor* pActor, void* pFrame, br_vector3* pBonePos, int pKind, undefined4 pArg3, int pFlag, int pBoneIndex, br_vector3* pBoneVec, br_vector3* pBonePtr);
    tPed_form* form;
    int iVar1;
    int iVar2;

    form = pCharacter->personality->form;
    iVar1 = (int)form->boned_physicing + (tS8)pCharacter->field_0x6 * 8;
    pBone_index *= 4;
    iVar2 = *(int*)((int)form->actor_sets + 4 + (tS8)pCharacter->field_0x4 * 8);
    iVar2 = *(int*)(iVar2 + pBone_index);
    iVar2 = MakeCharacterPhysics(pCharacter, (tPhysics_object*)*(int*)(*(int*)(iVar1 + 4) + pBone_index), (br_actor*)iVar2, (void*)(*(int*)(iVar2 + 0x18) + 0x34), NULL, 3, pArg3, pArg4, 0, NULL, NULL);
    if (iVar2 != 0) {
        return (tPhysics_object*)*(int*)(*(int*)(iVar1 + 4) + pBone_index);
    }
    return NULL;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a2380
tU32 gTime_of_last_zappy_headups;

// GLOBAL: CARMA2_HW 0x0074abf0
tU32 gCamera_period;

// GLOBAL: CARMA2_HW 0x00661678
br_vector3 gVector3_00661678 = { { 0.0f, 1.0f, 0.0f } };

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004f8b50
void C2_HOOK_FASTCALL FUN_004f8b50(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials) {
    extern void C2_HOOK_FASTCALL CreateShrapnelShower(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials);
    CreateShrapnelShower(pPosition, pVelocity, pDirection, pStrength, NULL, NULL, pMaterialCount, pMaterials);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004ec370
void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform) {
    extern int gSplit_triangle_vertex_orders[6][3];
    extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority);
    extern void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
    float vertex_data[2000];
    tU16 face_data[4000];
    int saved_seed;
    int face_count;
    int vertex_count;
    int face_index;
    int subdivision;
    int i;
    int j;
    br_vector3 centre;
    br_vector3 fragment_centre;
    br_vector3 transformed;
    float extents;
    float speed_scale;
    float edge01;
    float edge02;
    float edge12;
    float fraction;
    float* vtx[3];
    float* new_vertex;
    float* next_vertex;
    float* midpoint0;
    float* midpoint1;
    tU16* face_in;
    tU16* face_out;
    const int* order;
    tSmash_vertex* fragment;

    if (pFace_count != 0 && pSource_faces != NULL && pVertex_count != 0 && pSource_vertices != NULL) {
    saved_seed = rand();
    srand(pSeed);
    for (i = 0; i < 2000; i++) {
        vertex_data[i] = 0.0f;
    }
    for (i = 0; i < pVertex_count; i++) {
        *(br_vertex*)&vertex_data[i * 10] = pSource_actor->model->vertices[pSource_vertices[i]];
    }
    vertex_count = pVertex_count;
    for (i = 0; i < 4000; i++) {
        face_data[i] = 0;
    }
    for (i = 0; i < pFace_count; i++) {
        face_data[i * 20 + 0] = pSource_faces[i * 3 + 0];
        face_data[i * 20 + 1] = pSource_faces[i * 3 + 1];
        face_data[i * 20 + 2] = pSource_faces[i * 3 + 2];
        *(tU32*)&face_data[i * 20 + 4] = pOwner;
        *(tU8*)&face_data[i * 20 + 8] = 0;
    }
    face_count = pFace_count;
    face_in = face_data;
    for (face_index = 0; face_index < face_count; face_index++) {
        if (198 < face_count || 198 < vertex_count) {
            break;
        }
        face_out = &face_data[face_count * 20];
        new_vertex = &vertex_data[vertex_count * 10];
        for (subdivision = 0; subdivision < 4; subdivision++) {
            if (198 < face_count || 198 < vertex_count) {
                break;
            }
            for (j = 0; j < 3; j++) {
                vtx[j] = &vertex_data[face_in[j] * 10];
            }
            edge01 = (vtx[0][0] - vtx[1][0]) * (vtx[0][0] - vtx[1][0]) + (vtx[0][2] - vtx[1][2]) * (vtx[0][2] - vtx[1][2]) + (vtx[0][1] - vtx[1][1]) * (vtx[0][1] - vtx[1][1]);
            edge02 = (vtx[0][0] - vtx[2][0]) * (vtx[0][0] - vtx[2][0]) + (vtx[0][2] - vtx[2][2]) * (vtx[0][2] - vtx[2][2]) + (vtx[0][1] - vtx[2][1]) * (vtx[0][1] - vtx[2][1]);
            edge12 = (vtx[1][0] - vtx[2][0]) * (vtx[1][0] - vtx[2][0]) + (vtx[1][2] - vtx[2][2]) * (vtx[1][2] - vtx[2][2]) + (vtx[1][1] - vtx[2][1]) * (vtx[1][1] - vtx[2][1]);
            if (edge01 < pLongest_edge_squared && edge02 < pLongest_edge_squared && edge12 < pLongest_edge_squared && PercentageChance(0) == 0) {
                break;
            }
            if (edge01 < edge02 && edge01 < edge12) {
                order = gSplit_triangle_vertex_orders[4 + IRandomBetween(0, 1)];
            } else if (edge12 < edge02 && edge12 < edge01) {
                order = gSplit_triangle_vertex_orders[0 + IRandomBetween(0, 1)];
            } else {
                order = gSplit_triangle_vertex_orders[2 + IRandomBetween(0, 1)];
            }
            midpoint0 = new_vertex;
            midpoint1 = new_vertex + 10;
            next_vertex = new_vertex + 20;
            vertex_count += 2;
            fraction = FRandomBetween(0.2f, 0.8f);
            for (j = 0; j < 3; j++) {
                midpoint0[j] = (vtx[order[1]][j] - vtx[order[0]][j]) * fraction + vtx[order[0]][j];
            }
            for (j = 0; j < 2; j++) {
                midpoint0[j + 3] = (vtx[order[1]][j + 3] - vtx[order[0]][j + 3]) * fraction + vtx[order[0]][j + 3];
            }
            fraction = FRandomBetween(0.2f, 0.8f);
            for (j = 0; j < 3; j++) {
                midpoint1[j] = (vtx[order[2]][j] - vtx[order[0]][j]) * fraction + vtx[order[0]][j];
            }
            for (j = 0; j < 2; j++) {
                midpoint1[j + 3] = (vtx[order[2]][j + 3] - vtx[order[0]][j + 3]) * fraction + vtx[order[0]][j + 3];
            }
            face_out[0] = face_in[order[1]];
            face_out[1] = (tU16)(vertex_count - 2);
            face_out[2] = (tU16)(vertex_count - 1);
            *(tU32*)&face_out[4] = pOwner;
            *(tU8*)&face_out[8] = 0;
            face_out[20] = face_in[order[1]];
            face_out[21] = face_in[order[2]];
            face_out[22] = (tU16)(vertex_count - 1);
            *(tU32*)&face_out[24] = pOwner;
            *(tU8*)&face_out[28] = 0;
            face_in[order[1]] = (tU16)(vertex_count - 2);
            face_in[order[2]] = (tU16)(vertex_count - 1);
            face_out += 40;
            face_count += 2;
            new_vertex = next_vertex;
        }
        face_in += 20;
    }
    centre.v[0] = 0.0f;
    centre.v[1] = 0.0f;
    centre.v[2] = 0.0f;
    for (i = 0; i < vertex_count; i++) {
        BrMatrix34ApplyP(&transformed, (br_vector3*)&vertex_data[i * 10], pSource_transform);
        vertex_data[i * 10 + 0] = transformed.v[0];
        vertex_data[i * 10 + 1] = transformed.v[1];
        vertex_data[i * 10 + 2] = transformed.v[2];
        if ((pFlags & 1) != 0) {
            centre.v[0] += transformed.v[0];
            centre.v[1] += transformed.v[1];
            centre.v[2] += transformed.v[2];
        }
    }
    centre.v[0] /= (float)vertex_count;
    centre.v[1] /= (float)vertex_count;
    centre.v[2] /= (float)vertex_count;
    for (face_index = 0; face_index < face_count; face_index++) {
        tU16 face0;
        tU16 face1;
        tU16 face2;
        tU16 index;
        fragment = FindShrapnelFragment(NULL, pFragment_owner, 3);
        if (fragment == NULL) {
            break;
        }
        face0 = face_data[face_index * 20 + 0];
        face1 = face_data[face_index * 20 + 1];
        face2 = face_data[face_index * 20 + 2];
        for (i = 0; i < 3; i++) {
            if (i == 0) {
                index = face0;
            } else if (i == 1) {
                index = face1;
            } else {
                index = face2;
            }
            for (j = 0; j < 3; j++) {
                fragment->actor->model->vertices[i].p.v[j] = vertex_data[index * 10 + j];
            }
            fragment->actor->model->vertices[i].map.v[0] = vertex_data[index * 10 + 3];
            fragment->actor->model->vertices[i].map.v[1] = vertex_data[index * 10 + 4];
        }
        fragment->actor->model->faces[0].material = (br_material*)pOwner;
        fragment_centre.v[0] = (fragment->actor->model->vertices[0].p.v[0] + fragment->actor->model->vertices[1].p.v[0] + fragment->actor->model->vertices[2].p.v[0]) / (float)fragment->actor->model->nvertices;
        fragment_centre.v[1] = (fragment->actor->model->vertices[0].p.v[1] + fragment->actor->model->vertices[1].p.v[1] + fragment->actor->model->vertices[2].p.v[1]) / (float)fragment->actor->model->nvertices;
        fragment_centre.v[2] = (fragment->actor->model->vertices[0].p.v[2] + fragment->actor->model->vertices[1].p.v[2] + fragment->actor->model->vertices[2].p.v[2]) / (float)fragment->actor->model->nvertices;
        for (i = 0; i < fragment->actor->model->nvertices; i++) {
            fragment->actor->model->vertices[i].p.v[0] -= fragment_centre.v[0];
            fragment->actor->model->vertices[i].p.v[1] -= fragment_centre.v[1];
            fragment->actor->model->vertices[i].p.v[2] -= fragment_centre.v[2];
        }
        BrMatrix34Identity(&fragment->actor->t.t.mat);
        for (i = 0; i < 3; i++) {
            if ((pFlags & 1) != 0) {
                fragment->actor->t.t.translate.t.v[i] = FRandomPosNeg(0.04f) + centre.v[i];
            } else {
                fragment->actor->t.t.translate.t.v[i] = FRandomPosNeg(0.015f) + fragment_centre.v[i];
            }
        }
        BrModelUpdate(fragment->actor->model, 0x15);
        extents = 0.0f;
        for (i = 0; i < 3; i++) {
            extents += fragment->actor->model->bounds.max.v[i] - fragment->actor->model->bounds.min.v[i];
        }
        if (extents < 0.1) {
            speed_scale = (float)((0.2 - extents) * 5.0);
        } else {
            speed_scale = 0.5f;
        }
        ApplyInitialMovement((undefined4*)pInitial_speed, pDirection, &fragment->v, &fragment->omega, speed_scale, (br_vector3*)&fragment->actor->model->prepared->groups[0].faces[0].eqn, NULL, NULL);
        fragment->field_0xc = pTime;
        fragment->time_last_move = pTime;
        fragment->end_time = pTime + pLifetime_range[1];
        fragment->field_0x10 = pTime + IRandomBetween(pLifetime_range[0], pLifetime_range[1]);
    }
    srand(saved_seed);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c8940
void C2_HOOK_FASTCALL FUN_004c8940(uintptr_t pOwner) {
    extern void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);
    extern br_vector3 gZero_vector__smash;
    ARDoSingleSession(32, pOwner, &gZero_vector__smash, 4);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0065ff08
int gSplit_triangle_vertex_orders[6][3] = {
    { 0, 1, 2 },
    { 0, 2, 1 },
    { 1, 0, 2 },
    { 1, 2, 0 },
    { 2, 0, 1 },
    { 2, 1, 0 },
};

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040b500
void C2_HOOK_FASTCALL FUN_0040b500(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4) {
    if (pCharacter->field_0x4 >= 0) {
        pCharacter->field_0xa = (tS8)pArg4;
        pCharacter->personality->form->actor_sets[(tS8)pCharacter->field_0x4].actors[pBone_index]->model = (br_model*)pArg3;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004e7350
void C2_HOOK_FASTCALL DrawStatusMessage(int pReplay_only) {
    extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
    extern void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText);
    extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
    extern tGraf_data* gCurrent_graf_data;
    extern tDR_font gFonts[28];
    extern int gShow_status_message;
    extern int gNext_replay_record_state;
    extern int gStatus_message_1;
    extern int gStatus_message_2;
    int x;
    int y;

    if (gShow_status_message != 0) {
        if (gNext_replay_record_state == 4) {
            x = gCurrent_graf_data->field_0x350;
            y = gCurrent_graf_data->field_0x354;
            if (pReplay_only == 0) {
                DRPixelmapText(gBack_screen, x, y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
            }
        } else {
            x = gCurrent_graf_data->width / 2;
            y = gCurrent_graf_data->height / 2;
            DRPixelmapCentredText(gBack_screen, x, y - 0x40, &gFonts[4], GetMiscString(gStatus_message_1));
            if (gStatus_message_2 != 0) {
                DRPixelmapCentredText(gBack_screen, x, y - 0x20, &gFonts[6], GetMiscString(gStatus_message_2));
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0043be50
void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe) {
    br_model* pModel;
    br_vector3 offset;
    br_vector3 scaled_offset;
    tU16* pWeights;
    br_scalar scale;
    int i;
    int vertex_offset;

    pModel = (br_model*)field_0x4;
    ExpandVector3(&offset, (const tCompressed_vector3*)pField_0x8, -1.f, 1.f);
    i = 0;
    if (pModel->nvertices > 0) {
        pWeights = (tU16*)pField_0xe;
        vertex_offset = 0;
        do {
            if (*pWeights != 0) {
                scale = DRU16ToScalar(*pWeights, 0.f, 10.f);
                BrVector3Scale(&scaled_offset, &offset, -scale);
                BrVector3Accumulate((br_vector3*)((tU8*)pModel->vertices + vertex_offset), &scaled_offset);
            }
            i++;
            pWeights++;
            vertex_offset += sizeof(br_vertex);
        } while (i < pModel->nvertices);
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        ((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
}
