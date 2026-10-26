#include "07-structur.h"

#include "13-crush1.h"
#include "15-displays.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <string.h>

/* gRace_over_reason (0x006b781c) is defined in 68-timer.c, adjacent to gTimers as in the original layout */

// GLOBAL: CARMA2_HW 0x00655e48
int gHud_tinted1 = -1;

// GLOBAL: CARMA2_HW 0x00655e4c
int gHud_tinted2 = -1;

// GLOBAL: CARMA2_HW 0x00655e50
int gHud_tinted3 = -1;

// GLOBAL: CARMA2_HW 0x00655e58
float gForce_feedback_upper_limit = 2.7f;

// GLOBAL: CARMA2_HW 0x00655e5c
float gForce_feedback_lower_limit = 1.5f;

// GLOBAL: CARMA2_HW 0x0074a738
int gInitialised_grid;

// GLOBAL: CARMA2_HW 0x0074c6d0
int gInitialised_grid_unk_0074c6d0;

// GLOBAL: CARMA2_HW 0x0079ec44
tSpecial_volume* gLast_camera_special_volume;

// GLOBAL: CARMA2_HW 0x0079ec54
int gAuto_repair;

// GLOBAL: CARMA2_HW 0x0079eaa8
int gCredits_won_headup;

// GLOBAL: CARMA2_HW 0x0079eaf4
int gPed_kill_count_headup;

// GLOBAL: CARMA2_HW 0x0079eaf8
int gCar_kill_count_headup;

// GLOBAL: CARMA2_HW 0x0079eaec
int gTimer_headup;

// GLOBAL: CARMA2_HW 0x0079eb40
int gTime_awarded_headup;

// GLOBAL: CARMA2_HW 0x0079eaf0
int gLaps_headup;

// GLOBAL: CARMA2_HW 0x00762140
int gCredits_checkpoint[3];

// GLOBAL: CARMA2_HW 0x00660cdc
int gLast_wrong_checkpoint = -1;

// GLOBAL: CARMA2_HW 0x00660ce0
int gMirror_on__structur = 1;

// GLOBAL: CARMA2_HW 0x00660ce8
int gLoad_last_save_game = 1;

// GLOBAL: CARMA2_HW 0x006aaa34
tU32 gLast_checkpoint_time;

// GLOBAL: CARMA2_HW 0x006aaa38
int gHud_tinted1_visible;

// GLOBAL: CARMA2_HW 0x006aaa3c
int gHud_tinted2_visible;

// FUNCTION: CARMA2_HW 0x00503050
int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int result;
    int count;
    int i;
    tCar_spec* car_spec;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || (gCurrent_race.race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}

// STUB: CARMA2_HW 0x005030b0
// STUB: CARMA2_SW 0x004fea00
void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00503270
void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound) {
    (void)pCheckpoint_index;

    PratcamEvent(33);
    DoFancyHeadup(21);
    if (pDo_sound) {
        DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x005032a0
// STUB: CARMA2_SW 0x004febf0
void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x005033b0
// FUNCTION: CARMA2_SW 0x004fed00
void C2_HOOK_FASTCALL IncrementLap(void) {
    int i;

    for (i = gCheckpoint; i <= gCheckpoint_count; i++) {
        IncrementCheckpoint();
    }
}

// RayHitFace

// FUNCTION: CARMA2_HW 0x005033e0
void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index) {

    if (gRace_finished) {
        return;
    }
    if (!(gCurrent_race.race_spec->race_type == kRaceType_Carma1 ||
        gCurrent_race.race_spec->race_type == kRaceType_Checkpoints)) {
        return;
    }
    if ((gLast_wrong_checkpoint == pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 20000)
            || (gLast_wrong_checkpoint != pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 2000)) {

        if (gNet_mode == eNet_mode_none) {
            int next_checkpoint = pCheckpoint_index + 2;
            if (next_checkpoint > gCurrent_race.check_point_count) {
                if (gLap == 1) {
                    next_checkpoint = -1;
                } else {
                    next_checkpoint = 1;
                }
            }
            if (next_checkpoint == gCheckpoint) {
                return;
            }
        }
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_wrong_checkpoint));
        DRS3StartSound(gPedestrians_outlet, eSoundId_WrongCheckpoint);
        gLast_checkpoint_time = GetTotalTime();
        gLast_wrong_checkpoint = pCheckpoint_index;
    }
}
// CheckCheckpoints

// FUNCTION: CARMA2_HW 0x005039b0
// FUNCTION: CARMA2_SW 0x004ff300
void C2_HOOK_FASTCALL TotalRepair(void) {

    TotallyRepairCar();
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_instant_repair));
}

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// FUNCTION: CARMA2_HW 0x00503b00
// FUNCTION: CARMA2_SW 0x004ff450
int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < gNumber_of_racers; i++) {
            if ((pNastiness == 0 || gOpponents[i].strength_rating == pNastiness)
                && gProgram_state.current_car_index != i
                && !gOpponents[i].picked
                && gOpponents[i].strength_rating <= 5
                && gOpponents[i].strength_rating >= 1) {
                temp_array[count++] = i;
            }
        }
        if (count != 0) {
            break;
        }
        if (pNastiness < 5) {
            pNastiness++;
        } else {
            pNastiness = 0;
        }
    }
    i = temp_array[IRandomBetween(0, count - 1)];
    gOpponents[i].picked = 1;
    return i;
}

// SelectOpponents

// FUNCTION: CARMA2_HW 0x00503ba0
int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence) {
    int i;
    int new_index;
    int races_count;
    int most_seldom_seen;
    int races_to_pick_from[50];

    if (pNet_race_sequence == eNet_sequence_sequential) {
        pCurrent_race++;
        if (pCurrent_race >= gNumber_of_races) {
            pCurrent_race = 0;
        }
        return pCurrent_race;
    } else {
        most_seldom_seen = 10000;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents < most_seldom_seen) {
                most_seldom_seen = gRace_list[i].count_opponents;
            }
        }
        races_count = 0;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents == most_seldom_seen && i != pCurrent_race) {
                races_to_pick_from[races_count] = i;
                races_count++;
            }
        }
        new_index = races_to_pick_from[IRandomBetween(0, races_count - 1)];
        gRace_list[new_index].count_opponents++;
        return new_index;
    }
}

// SwapNetCarsLoad

// SwapNetCarsDispose

extern void C2_HOOK_FASTCALL DisposeGameIfNecessary(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL DoSCILogo(void);
extern void C2_HOOK_FASTCALL DoOpeningAnimation(void);
extern void C2_HOOK_STDCALL DoStainlessLogo(void);
extern void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);
extern void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);
extern void C2_HOOK_FASTCALL InitGame(int pStart_race);
extern void C2_HOOK_FASTCALL MaybeRestoreSavedGame(void);
extern void C2_HOOK_FASTCALL DoLoadMostRecentGame(void);
extern void C2_HOOK_FASTCALL DoMainScreen(void);
extern int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);
extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, const char* pTitle);
extern void C2_HOOK_FASTCALL StartLoadingScreen(void);
extern void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info);
extern void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race);
extern void C2_HOOK_FASTCALL DisposeRaceInfo(tRace_info* pRace_info);
extern void C2_HOOK_FASTCALL DisableNetService(void);
extern void C2_HOOK_FASTCALL ReenableNetService(void);
extern void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);
extern int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type);
extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);
extern int C2_HOOK_FASTCALL SwitchToLoresMode(void);
extern int C2_HOOK_FASTCALL SortOpponents(void);
extern void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race);
extern void C2_HOOK_FASTCALL DisposeRace(void);
extern void C2_HOOK_FASTCALL DisposeTrack(void);
extern void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info);
extern void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race);
extern void C2_HOOK_FASTCALL InitSoundSources(void);
extern void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type);
extern void C2_HOOK_FASTCALL EnableAutoFogification(void);
extern void C2_HOOK_FASTCALL DisableAutoFogification(void);
extern void C2_HOOK_FASTCALL RecordAPOAndCredits(void);
extern tRace_result C2_HOOK_FASTCALL DoRace(void);
extern void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed);
extern void C2_HOOK_FASTCALL DoEndRaceAnimation(void);
extern void C2_HOOK_FASTCALL DoSaveGame(void);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL TotallyRepairCar(void);
extern void C2_HOOK_FASTCALL DisposeOpponentsCars(tRace_info* pRace_info);
extern void C2_HOOK_FASTCALL DisposeCar(tCar_spec* pCar_spec, int pOwner);
extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);
extern void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space);
extern tCar_spec* gCar_to_view;
extern int gSave_game_out_of_sync;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on carpocalypse2 structur.c:362
// FUNCTION: CARMA2_HW 0x005039e0
void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pThe_race) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        pThe_race->number_of_racers = pThe_race->race_spec->count_explicit_opponents;
        for (i = 0; i < gNumber_of_racers; i++) {
            gOpponents[i].picked = 0;
        }
        for (i = 0; i < pThe_race->race_spec->count_explicit_opponents; i++) {
            if (pThe_race->race_spec->explicit_opponents[i] >= 0) {
                gOpponents[pThe_race->race_spec->explicit_opponents[i]].picked = 1;
                pThe_race->opponent_list[i].index = pThe_race->race_spec->explicit_opponents[i];
                pThe_race->opponent_list[i].ranking = -(i + 1);
            }
        }
        for (i = 0; i < pThe_race->race_spec->count_explicit_opponents; i++) {
            if (pThe_race->race_spec->explicit_opponents[i] < 0) {
                pThe_race->opponent_list[i].index = ChooseOpponent(-pThe_race->race_spec->explicit_opponents[i]);
                pThe_race->opponent_list[i].ranking = IRandomBetween(0, 99);
            }
        }
    }
}

extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern char* C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern void C2_HOOK_FASTCALL InitFogificateMaterials(void);
extern void C2_HOOK_FASTCALL InitSmashTargets(void);
extern void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void);
extern int C2_HOOK_FASTCALL PHILInit(void);
extern void C2_HOOK_FASTCALL SetUpTestObjects(void);
extern void C2_HOOK_FASTCALL InitTargetLock(void);
extern void C2_HOOK_FASTCALL ReinitSmashing(void);
extern void C2_HOOK_FASTCALL InitRepairAnimations(void);
extern void C2_HOOK_FASTCALL ResetExplosions(void);
extern void C2_HOOK_FASTCALL ClearWobbles(void);
extern void C2_HOOK_FASTCALL ClearHeadups(void);
extern void C2_HOOK_FASTCALL ResetOilSpills(void);
extern void C2_HOOK_FASTCALL HideSkids(void);
extern void C2_HOOK_FASTCALL ResetPedNearness(void);
extern void C2_HOOK_FASTCALL InitPratcam(void);
extern void C2_HOOK_FASTCALL SetIntegerMapRenders(void);
extern int C2_HOOK_FASTCALL GetSkyTextureOn(void);
extern void C2_HOOK_FASTCALL ToggleSkyQuietly(void);
extern int C2_HOOK_FASTCALL GetDepthCueingOn(void);
extern void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void);
extern void C2_HOOK_FASTCALL SaveShadeTables(void);
extern void C2_HOOK_FASTCALL ResetSparks(void);
extern void C2_HOOK_FASTCALL InitPedsForRace(void);
extern void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette);
extern void C2_HOOK_FASTCALL LoadFont(int pFont_ID);
extern int C2_HOOK_FASTCALL TranslationMode(void);
extern void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);
extern void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);
extern void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);
extern void C2_HOOK_FASTCALL InitNetGameplayStuff(void);
extern void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
extern tDR_font gFonts[28];
extern br_pixelmap* gIcons_pix;
extern br_pixelmap* gLit_up_stat;
extern tPath_name gApplication_path;
extern tSlot_info gMax_APO[3];
extern tSlot_info gInitial_APO_potential[3];
extern int gInitial_powerup_slots[3];
extern int gRace_head_ups[22];
extern int gNumber_of_races;
extern int gSwap_depth_effect_type;
extern br_pixelmap* gSwap_sky_texture;
extern int gInstant_handbrake;
extern br_pixelmap* gRender_palette;
extern int gCredits_won_headup;
extern int gPed_kill_count_headup;
extern int gCar_kill_count_headup;
extern int gTimer_headup;
extern int gTime_awarded_headup;
extern int gLaps_headup;
extern int gNet_cash_headup;
extern int gNet_ped_headup;
extern int gCountdown;
extern int gLap;
extern int gTotal_laps;
extern int gCheckpoint;
extern int gCheckpoint_count;
extern int gFreeze_timer;
extern int gFree_repairs;
extern int gShow_opponents;
extern int gOn_drugs;
extern tU32 gRace_finished;
extern float gOpponent_speed_factor;
extern float gCop_speed_factor;
extern float gGravity_multiplier;
extern float gDefault_gravity;
extern float gPinball_factor;
extern int gShow_peds_on_map;
extern tU32 gMap_time;
extern tU32 gTimer;
extern br_scalar gYon_multiplier;
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern br_pixelmap* gBack_screen;

// STUB: CARMA2_HW 0x00504bf0
void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0042a190
void C2_HOOK_FASTCALL ResetCrushSystems(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0041fc60
void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004da630
void C2_HOOK_FASTCALL ResetPowerups(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0049b720
void C2_HOOK_FASTCALL InitPlayers(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* Modify2DCopyPixelmaps and SetupFFBValues are inlined into InitRace by the original compiler */
static void InitRace_Modify2DCopyPixelmaps(void) {
    int i;
    int pixel_type;

    pixel_type = gBack_screen->type;
    for (i = 0; i < (int)REC2_ASIZE(gFonts); i++) {
        BRPM_convert(gFonts[i].images, pixel_type);
    }
    BRPM_convert(gIcons_pix, pixel_type);
    BRPM_convert(gLit_up_stat, pixel_type);
}

static void InitRace_SetupFFBValues(void) {
    tPath_name path;
    FILE* f;
    char s[256];

    PathCat(path, gApplication_path, "FFB.TXT");
    f = DRfopen(path, "rt");
    GetALineAndDontArgue(f, s);
    sscanf(strtok(s, "\t ,/"), "%f", &gForce_feedback_upper_limit);
    GetALineAndDontArgue(f, s);
    sscanf(strtok(s, "\t ,/"), "%f", &gForce_feedback_lower_limit);
    PFfclose(f);
}

/* based on carpocalypse2 src/carma2/common/init.c:518 (GPL-3.0) */
// FUNCTION: CARMA2_HW 0x00481830
void C2_HOOK_FASTCALL InitRace(void) {
    int i;

    gMap_view = 1;
    InitFogificateMaterials();
    InitSmashTargets();
    ClearAllRuntimeVariables();
    PHILInit();
    SetUpTestObjects();
    InitTargetLock();
    ReinitSmashing();
    InitRepairAnimations();
    ResetExplosions();
    ClearWobbles();
    ClearHeadups();
    ResetOilSpills();
    HideSkids();
    ResetPedNearness();
    PossibleService();
    InitPedsForRace();
    BuildColourTable(gRender_palette);
    PossibleService();

    gInitialised_grid_unk_0074c6d0 = 0;
    gINT_0074b780 = 0;
    gLast_camera_special_volume = NULL;
    if (!TranslationMode()) {
        LoadFont(1);
        LoadFont(2);
    }
    LoadFont(3);
    LoadFont(4);
    LoadFont(5);
    PossibleService();
    LoadFont(6);
    LoadFont(7);
    LoadFont(8);
    LoadFont(23);
    PossibleService();
    ResetRecoveryVouchers();
    gMap_time = 0;
    gProgram_state.cockpit_image_index = 0;
    PossibleService();
    gProgram_state.which_view = eView_forward;
    gProgram_state.new_view = eView_undefined;
    gProgram_state.pratcam_move_start = 0;
    gAction_replay_mode = 0;
    InitPratcam();
    gProgram_state.peds_killed = 0;
    gProgram_state.revs = 2000;
    gProgram_state.current_car.speed = 0.0f;
    gProgram_state.current_car.steering_angle = 0.0f;
    gProgram_state.current_car.lf_sus_position = 0.0f;
    gProgram_state.current_car.rf_sus_position = 0.0f;
    gProgram_state.current_car.lr_sus_position = 0.0f;
    gProgram_state.current_car.rr_sus_position = 0.0f;
    PossibleService();
    gAuto_repair = 0;
    SetIntegerMapRenders();
    AdjustRenderScreenSize();
    PrintMemoryDump(0, "DIRECTLY BEFORE LOADING IN TRACK");
    LoadTrack(gProgram_state.track_file_name, &gProgram_state.track_spec, &gCurrent_race);
    if (gYon_multiplier != 1.0f) {
        AdjustRenderScreenSize();
    }
    PrintMemoryDump(0, "DIRECTLY AFTER LOADING IN TRACK");
    ResetCrushSystems();
    LoadAIWorldForRace(&gCurrent_race);

    for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
        gRace_head_ups[i] = NewTextHeadupSlot(16 + i, 0, 0, -1, "");
    }
    if (gNet_mode != eNet_mode_none) {
        gNet_cash_headup = NewTextHeadupSlot(13, 0, 0, -6, "");
        gNet_ped_headup = NewTextHeadupSlot(14, 0, 0, -6, "");

        for (i = 0; i < (int)REC2_ASIZE(gInitial_powerup_slots); i++) {
            gInitial_powerup_slots[i] = gNet_mode != eNet_mode_none ? gMax_APO[i].initial_network[gCurrent_net_game->type] : gMax_APO[i].initial[gProgram_state.skill_level];
        }
    } else {
        gCredits_won_headup = NewTextHeadupSlot(1, 0, 0, -6, "");
        gPed_kill_count_headup = NewTextHeadupSlot(2, 0, 0, -6, "");
        gCar_kill_count_headup = NewTextHeadupSlot(12, 0, 0, -6, "");
        gTimer_headup = NewTextHeadupSlot(7, 0, 0, -5, "");
        gTime_awarded_headup = NewTextHeadupSlot(11, 0, 0, -2, "");
        gLaps_headup = NewTextHeadupSlot(8, 0, 0, -6, "");

        for (i = 0; i < (int)REC2_ASIZE(gInitial_powerup_slots); i++) {
#define INITRACE_MAX(i) (gNet_mode != eNet_mode_none ? gMax_APO[i].initial_network[gCurrent_net_game->type] : gMax_APO[i].initial[gProgram_state.skill_level])
#define INITRACE_CUR(i) (gNet_mode != eNet_mode_none ? gInitial_APO_potential[i].initial_network[gCurrent_net_game->type] : gInitial_APO_potential[i].initial[gProgram_state.skill_level])
            gInitial_powerup_slots[i] = INITRACE_CUR(i) + 1 + (INITRACE_MAX(i) - INITRACE_CUR(i) - 1) * gCurrent_race.index / (gNumber_of_races - 1);
        }
    }

    InstantDepthChange(
        gProgram_state.default_depth_effect.type,
        gProgram_state.default_depth_effect.sky_texture,
        gProgram_state.default_depth_effect.start,
        gProgram_state.default_depth_effect.end,
        gProgram_state.default_depth_effect.colour.red,
        gProgram_state.default_depth_effect.colour.green,
        gProgram_state.default_depth_effect.colour.blue,
        1);
    gSwap_sky_texture = NULL;
    if (!GetSkyTextureOn()) {
        ToggleSkyQuietly();
    }
    gSwap_depth_effect_type = eDepth_effect_none;
    if (!GetDepthCueingOn()) {
        ToggleDepthCueingQuietly();
    }
    PrintMemoryDump(0, "AFTER LOADING IN AI STUFF");
    SaveShadeTables();
    gCountdown = 7;
    gLap = 1;
    gTimer = 1000 * gCurrent_race.initial_timer[gProgram_state.skill_level];
    gTotal_laps = gCurrent_race.total_laps;

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_5) {
        gTotal_laps = gCurrent_net_game->options.starting_target;
    }
    gCheckpoint = 1;
    gCheckpoint_count = gCurrent_race.check_point_count;
    gFreeze_timer = 0;
    gFree_repairs = 0;
    if (gNet_mode == eNet_mode_none) {
        gShow_opponents = 1;
    } else {
        gShow_opponents = gCurrent_net_game->options.show_players_on_map;
    }
    gOn_drugs = 0;
    gRace_finished = 0;
    gOpponent_speed_factor = 1.f;
    gCop_speed_factor = 1.f;
    gGravity_multiplier = gDefault_gravity;
    gPinball_factor = 0.f;
    gInstant_handbrake = 0;
    gShow_peds_on_map = 0;

    PossibleService();
    SetCarSuspGiveAndHeight(&gProgram_state.current_car, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    ResetPowerups();
    PossibleService();
    ResetSparks();
    PossibleService();
    PossibleService();
    InitPlayers();
    if (gNet_mode != eNet_mode_none) {
        gProgram_state.credits = gCurrent_net_game->options.starting_credits;
        InitNetGameplayStuff();
    }
    gInitialised_grid = 0;
    SetCameraType(gAction_replay_camera_mode);
    InitRace_Modify2DCopyPixelmaps();

    InitRace_SetupFFBValues();
    PossibleLock(1);
    PossibleUnlock(1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004c6580
void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e3440
int C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* DoGame (inlined into DoProgram by the original compiler) */
static void DoGame(void) {
    int i;
    tRace_result race_result;
    int game_completed;

    PrintMemoryDump(0, "START OF DoGame()");
    gAbandon_game = 0;
    gDisallow_abandon_race = 0;
    gCar_to_view = &gProgram_state.current_car;
    StartLoadingScreen();
    gProgram_state.prog_status = eProg_game_ongoing;
    if (gNet_mode_of_last_game != gNet_mode) {
        gProgram_state.prog_status = eProg_idling;
        return;
    }
    PrintMemoryDump(0, "BEFORE START RACE SCREEN");
    LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
    SelectOpponents(&gCurrent_race);
    if (gNet_mode != eNet_mode_none) {
        FillInRaceInfo(&gCurrent_race);
        DisposeRaceInfo(&gCurrent_race);
    } else {
        SelectOpponents(&gCurrent_race);
        LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
        FillInRaceInfo(&gCurrent_race);
    }
    PrintMemoryDump(0, "AFTER START RACE SCREEN");
    if (gNet_mode != eNet_mode_none && (gCurrent_net_game->options.car_choice == 2 || gCurrent_net_game->options.car_choice == 1) && !gNo_races_yet) {
        DisableNetService();
        AboutToLoadFirstCar();
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].next_car_index >= 0) {
                gNet_players[i].car_index = gNet_players[i].next_car_index;
            }
            gNet_players[i].next_car_index = -1;
            LoadCar(gOpponents[gNet_players[i].car_index].car_file_name, (i == gThis_net_player_index) + 7, gNet_players[i].car, gNet_players[i].car_index, gNet_players[i].player_name, &gNet_cars_storage_space);
        }
        ReenableNetService();
    }
    PrintMemoryDump(0, "AFTER LOADING OPPONENTS IN");
    InitRace();
    if (gNet_mode_of_last_game != gNet_mode) {
        gProgram_state.prog_status = eProg_idling;
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        FRONTEND_Main(3);
        dr_dprintf("Returned from NetSync screen");
        SwitchToLoresMode();
    } else {
        SortOpponents();
        SetInitialPositions(&gCurrent_race);
    }
    if (gProgram_state.prog_status == eProg_idling) {
        dr_dprintf("Calling DisposeRace()");
        DisposeRace();
        dr_dprintf("Calling DisposeTrack()");
        DisposeTrack();
        dr_dprintf("Returning from DoGame()");
        return;
    }
    InitialiseAIWorldForRace(&gCurrent_race);
    InitialiseCarsEtc(&gCurrent_race);
    InitSoundSources();
    InitLastDamageArrayEtc();
    FogificateMaterials(1);
    EnableAutoFogification();
    RecordAPOAndCredits();
    race_result = DoRace();
    DisableAutoFogification();
    DisposeRace();
    PrintMemoryDump(0, "AFTER DISPOSING RACE");
    if (gNet_mode != eNet_mode_none) {
        gProgram_state.current_race_index = gPending_race;
        gCurrent_net_game->start_race = gPending_race;
        gPending_race = -1;
        if (race_result != 0 && race_result != 1) {
            DoEndRaceSummary(race_result, 0);
        }
    } else {
        game_completed = DoPostRace(race_result);
        if (race_result == 3) {
            gSave_game_out_of_sync = 1;
            DoEndRaceAnimation();
            DoEndRaceSummary(race_result, game_completed);
            DoSaveGame();
        }
    }
    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            TotallyRepairACar(gNet_players[i].car);
        }
    } else {
        TotallyRepairCar();
    }
    if (gNet_mode == eNet_mode_none) {
        DisposeOpponentsCars(&gCurrent_race);
    } else if ((gCurrent_net_game->options.car_choice == 2 || gCurrent_net_game->options.car_choice == 1) && !gNo_races_yet) {
        DisableNetService();
        for (i = 0; i < gNumber_of_net_players; i++) {
            DisposeCar(gNet_players[i].car, gNet_players[i].car_index);
            if (gNet_players[i].car_index >= 0) {
                gCar_details[gNet_players[i].car_index].ownership = 0;
            }
        }
        ClearOutStorageSpace(&gOur_car_storage_space);
        ClearOutStorageSpace(&gNet_cars_storage_space);
        ReenableNetService();
    }
    PrintMemoryDump(0, "AFTER DISPOSING CARS");
    DisposeTrack();
    PrintMemoryDump(0, "AFTER DISPOSING TRACK");
    if (gProgram_state.prog_status == eProg_game_ongoing) {
        gProgram_state.prog_status = (gNet_mode != eNet_mode_none) ? eProg_game_starting : eProg_idling;
    }
}

// FUNCTION: CARMA2_HW 0x00503c50
void C2_HOOK_FASTCALL DoProgram(void) {

    /* InitialiseProgramState (inlined) */
    gProgram_state.mirror_on = gMirror_on__structur;
    gProgram_state.prog_status = eProg_intro;
    gProgram_state.last_slot = 0;
    gProgram_state.view_type = 0;
    gProgram_state.frank_or_anniness = eFrankie;
    gProgram_state.racing = 0;
    gProgram_state.cut_scene = 0;
    gProgram_state.saving = 0;
    gProgram_state.loading = 0;
    gProgram_state.dont_save_or_load = 0;
    gProgram_state.dont_load = 0;
    gProgram_state.field_0x2c = 0;
    if (gAusterity_mode) {
        gPratcam_on = 0;
        gProgram_state.prat_cam_on = 0;
    } else {
        gProgram_state.prat_cam_on = gPratcam_on;
    }
    gProgram_state.cockpit_on = gCockpit_on;
    gProgram_state.car_name[0] = '\0';
    SetSoundVolumes(0);

    do {
        switch (gProgram_state.prog_status) {
        case eProg_intro:
            DisposeGameIfNecessary();
            ClearEntireScreen();
            DoSCILogo();
            DoOpeningAnimation();
            DoStainlessLogo();
            gProgram_state.prog_status = eProg_opening;
            break;
        case eProg_opening:
            DisposeGameIfNecessary();
            gProgram_state.prog_status = eProg_idling;
            DRS3StopOutletSound(gEffects_outlet);
            break;
        case eProg_idling:
            DisposeGameIfNecessary();
            if (gNo_current_game) {
                LoadRaces(gRace_list, &gNumber_of_races, -1);
                InitGame(gDev_initial_race);
                MaybeRestoreSavedGame();
            }
            if (gAuto_load && gLoad_last_save_game) {
                DoLoadMostRecentGame();
                gLoad_last_save_game = 0;
            }
            DoMainScreen();
            break;
        case eProg_demo:
            ClearEntireScreen();
            DoSCILogo();
            DoOpeningAnimation();
            DoStainlessLogo();
            gProgram_state.prog_status = eProg_idling;
            DRS3StopOutletSound(gEffects_outlet);
            break;
        case eProg_game_starting:
            DoGame();
            break;
        default:
            break;
        }
    } while (gProgram_state.prog_status != eProg_quit);
}

extern void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was);
extern int C2_HOOK_FASTCALL SpendCredits(int pAmount);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);

// FUNCTION: CARMA2_HW 0x00504170
void C2_HOOK_FASTCALL JumpTheStart(void) {
    char s[256];

    if (gNet_mode == eNet_mode_none || gProgram_state.credits >= gJump_start_fine[gProgram_state.skill_level]) {
        WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(gCountdown);
        gCountdown = 0;
        DRS3StopOutletSound(gPedestrians_outlet);
        DRS3StartSound(gPedestrians_outlet, eSoundId_HeyYou);
        SpendCredits(gJump_start_fine[gProgram_state.skill_level]);
        sprintf(s, "%s %d %s",
            GetMiscString(eMiscString_bad_boy),
            gJump_start_fine[gProgram_state.skill_level],
            GetMiscString(eMiscString_credit_fine));
        NewTextHeadupSlot(4, 0, 1000, -4, s);
    }
}

extern int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex);
extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);
extern void C2_HOOK_FASTCALL PlayFlicsFromDisk(void);
extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

// FUNCTION: CARMA2_HW 0x00504230
void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void) {

    gInterface_within_race_mode = 1;
    gHud_tinted1_visible = !!TintedPolyIsOn(gHud_tinted1);
    gHud_tinted2_visible = !!TintedPolyIsOn(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    PlayFlicsFromDisk();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
}

extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);
extern int C2_HOOK_FASTCALL SwitchToLoresMode(void);

// FUNCTION: CARMA2_HW 0x005042a0
void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void) {

    gInterface_within_race_mode = 0;
    if (gHud_tinted1_visible) {
        TurnTintedPolyOn(gHud_tinted1);
    }
    if (gHud_tinted2_visible) {
        TurnTintedPolyOn(gHud_tinted2);
    }
    SwitchToLoresMode();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
}

