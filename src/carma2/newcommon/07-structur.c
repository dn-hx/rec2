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

#include <ctype.h>
#include <math.h>
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x005030b0
// STUB: CARMA2_SW 0x004fea00
void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// based on ApexWorm/rec2 b2b9f23b6
// FUNCTION: CARMA2_HW 0x005032a0
// FUNCTION: CARMA2_SW 0x004febf0
void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    int done_voice;

    done_voice = 0;
    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    gLast_checkpoint_time = GetTotalTime();
    if (gCheckpoint >= gCheckpoint_count) {
        gCheckpoint = 1;
        if (gLap++ >= gTotal_laps) {
            gLap = gTotal_laps;
            gCheckpoint = gCheckpoint_count;
            RaceCompleted(eRace_over_0);
        } else if (gLap == gTotal_laps) {
            PratcamEvent(33);
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_final_lap));
            DRS3StartSound(gPedestrians_outlet, eSoundId_FinalLap);
            done_voice = 1;
        }
    } else {
        gCheckpoint += 1;
    }
    if (!gRace_finished) {
        PratcamEvent(33);
        DoFancyHeadup(21);
        if (!done_voice) {
            DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
        }
        if (gCredits_checkpoint[gProgram_state.skill_level] != 0) {
            EarnCredits(gCredits_checkpoint[gProgram_state.skill_level]);
        }
    }
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
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);

static int C2_HOOK_FASTCALL RayHitFace(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2, br_vector3* pNormal, br_vector3* pStart, br_vector3* pDir) {
    (void)pV0; (void)pV1; (void)pV2; (void)pNormal; (void)pStart; (void)pDir;
    return 0;
}

static void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    (void)pPlayer; (void)pMess; (void)pParam_1; (void)pParam_2; (void)pParam_3; (void)pParam_4;
}

static void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {
    (void)pWinner_index;
}

/* based on carpocalypse2 src/carma2/common/structur.c:573 */
// FUNCTION: CARMA2_HW 0x005034b0
void C2_HOOK_FASTCALL CheckCheckpoints(void) {
    tCar_spec* car;
    br_vector3 orig;
    br_vector3 dir;
    int i;
    int j;
    int cat;
    int car_count;
    int car_index;
    tNet_game_player_info* net_player;

    if (gNet_mode == eNet_mode_client) {
        return;
    }
    if (gNet_mode == eNet_mode_host && gCurrent_net_game->type != eNet_game_type_5 && gCurrent_net_game->type != eNet_game_type_checkpoint && gCurrent_net_game->type != eNet_game_type_4) {
        return;
    }
    for (cat = eVehicle_self; cat <= gNet_mode; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car_index = 0; car_index < car_count; car_index++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, car_index);
            }
            BrVector3Copy(&orig, (br_vector3*)car->old_frame_mat.m[3]);
            BrVector3Sub(&dir, &car->car_master_actor->t.t.translate.t, &orig);
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                for (j = 0; j < gCurrent_race.checkpoints[i].count_quads; j++) {
                    if (
                            RayHitFace(&gCurrent_race.checkpoints[i].quads[j].points[0],
                                       &gCurrent_race.checkpoints[i].quads[j].points[1],
                                       &gCurrent_race.checkpoints[i].quads[j].points[2],
                                       &gCurrent_race.checkpoints[i].normals[j],
                                       &orig, &dir)
                            || RayHitFace(&gCurrent_race.checkpoints[i].quads[j].points[0],
                                          &gCurrent_race.checkpoints[i].quads[j].points[2],
                                          &gCurrent_race.checkpoints[i].quads[j].points[3],
                                          &gCurrent_race.checkpoints[i].normals[j],
                                          &orig,
                                          &dir)) {
                        if (gNet_mode == eNet_mode_none) {
                            if (i + 1 == gCheckpoint) {
                                IncrementCheckpoint();
                            } else {
                                WrongCheckpoint(i);
                            }
                        } else {
                            net_player = NetPlayerFromCar(car);
                            if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
                                if (net_player->score2 & (1 << i)) {
                                    net_player->score2 &= ~(1 << i);
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else if (gCurrent_net_game->type == eNet_game_type_5) {
                                if ((net_player->score2 % gCurrent_race.check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    gLast_checkpoint_time = GetTotalTime();
                                    if (net_player->score2 >= gCurrent_net_game->options.starting_target * gCurrent_race.check_point_count) {
                                        DeclareWinner(net_player - gNet_players);
                                    }
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else {
                                if ((net_player->score2 % gCurrent_race.check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    break;
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            }
                        }
                        break;
                    }
                }
            }
            BrMatrix34Copy(&car->old_frame_mat, &car->car_master_actor->t.t.mat);
        }
    }
}

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
extern tCrush_list_item gCrush_lists[8];
extern int gCount_queued_drone_crushes;
extern int gINT_0067bdfc;
extern int gCount_toggled_doors;
extern tU8 gCrush_data_entry_counter;
extern int gCount_net_crush_detach_list;
extern int gCount_net_crush_semi_detach_bit_list;
extern int gCount_net_crush_full_detach_bit_list;
extern int gCount_net_crush_reattach_bit_list;
extern int gCount_car_damage_crush_list;

/* LoadTrack (0x00504bf0) is defined at the end of this file (kept out of 28-world3.c so the storage helpers there are called, not inlined, as in the original) */
extern void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info);

// FUNCTION: CARMA2_HW 0x0042a190
void C2_HOOK_FASTCALL ResetCrushSystems(void) {
    int i;
    tCrush_list_item* crush_list;

    for (i = 0; i < (int)REC2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i]->type = eJoint_none;
    }
    gDetached_bit_crush_info_buffer.field_0x8 = 0;
    gSplit_car_crush_info_buffer.field_0x8 = 0;
    crush_list = gCrush_lists;
    while (crush_list->car_spec != NULL) {
        crush_list->car_spec = NULL;
        crush_list++;
    }
    gCount_queued_drone_crushes = 0;
    gCount_crush_detach_list = 0;
    gINT_0067bdfc = 0;
    gCount_toggled_doors = 0;
    gCrush_data_entry_counter = 0;
    gCount_net_crush_detach_list = 0;
    gCount_net_crush_semi_detach_bit_list = 0;
    gCount_net_crush_full_detach_bit_list = 0;
    gCount_net_crush_reattach_bit_list = 0;
    gCount_car_damage_crush_list = 0;
}

// FUNCTION: CARMA2_HW 0x0041fc60
void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
    tPhysics_object* collision_info = pCar->collision_info;
    float ratio;
    float rear_divisor;
    float front_give;
    float rear_give;
    float damping;
    float root_stiffness;
    float front_divisor;
    float rear_height;
    float front_height;
    float rear_min;
    float front_min;
    float min_value;

    ratio = (float)fabs((pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) / (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2]));
    rear_divisor = ratio - -1.0f;
    rear_give = pCar->steerable_suspension_give * pRear_give_factor * WORLD_SCALE;
    pCar->sk[0] = (collision_info->M / rear_divisor) * 5.0f / rear_give;
    root_stiffness = (float)sqrt(5.0);
    pCar->sb[0] = (float)((collision_info->M / rear_divisor) * root_stiffness / sqrt(rear_give));
    front_give = pCar->susp_give * pFront_give_factor * WORLD_SCALE;
    damping = pCar->damping * pDamping_factor;
    front_divisor = 1.0 / ratio;
    pCar->sk[1] = (float)((collision_info->M / (front_divisor - -1.0f)) * 5.0f / front_give);
    pCar->sb[1] = (float)((collision_info->M / (front_divisor - -1.0f)) * root_stiffness / sqrt(front_give));
    pCar->sb[0] *= damping;
    pCar->sb[1] *= damping;
    rear_height = rear_give + pExtra_rear_height + pCar->ride_height;
    front_height = front_give + pExtra_front_height + pCar->ride_height;
    pCar->susp_height[0] = rear_height;
    pCar->susp_height[1] = front_height;
    rear_min = (pCar->wpos[0].v[1] - rear_height) * (1.0f / WORLD_SCALE);
    front_min = (pCar->wpos[2].v[1] - front_height) * (1.0f / WORLD_SCALE);
    min_value = MIN(rear_min, front_min);
    min_value = MIN(min_value, collision_info->bb1.min.v[1]);
    collision_info->bb2.min.v[1] = min_value;
}

/* powerup/inventory globals defined in 27-powerup.c */
extern tU8* gRace_powerup_respawn_bools;
extern tPowerup_respawn_spec* gPowerup_respawn_specs;
extern tPowerup* gPowerup_array;
extern int gNumber_of_powerups;
extern tRespawn_powerup gRespawn_powerups[100];
extern tHeadup_icon gPickedup_powerups[20];
extern int gInventory[20];
extern int gInventory_selected;
extern int gINT_006a0a6c;
extern int gINT_006a0a5c;
extern int gInventory_cycling;
extern tU32 gInventory_timeout;
extern int gCount_cloaked_cars;
extern tRepulse_link gRepulse_links[6];
extern br_actor* gNon_track_actor;
extern int gNumber_of_icons;
extern undefined4 gDAT_006a0958;
extern tU32 gNext_goody_time;

// FUNCTION: CARMA2_HW 0x004da630
void C2_HOOK_FASTCALL ResetPowerups(void) {
    int i;
    char* pp;
    tPowerup* the_powerup;
    tRepulse_link* link;

    gRace_powerup_respawn_bools = gPowerup_respawn_specs[gNet_mode == eNet_mode_none ? 0 : 1].bools;
    i = 0;
    if (0 < gNumber_of_powerups) {
        pp = (char*)gPowerup_array + 0x28;
        do {
            the_powerup = (tPowerup*)(pp - 0x28);
            the_powerup->got_time = 0;
            the_powerup->current_value = -1;
            i++;
            pp += sizeof(tPowerup);
        } while (i < gNumber_of_powerups);
    }
    for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
        gRespawn_powerups[i].actor = NULL;
    }
    for (i = 0; i < (int)REC2_ASIZE(gPickedup_powerups); i++) {
        if (gPickedup_powerups[i].icon_actor != NULL) {
            gPickedup_powerups[i].icon_actor->render_style = BR_RSTYLE_NONE;
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
    gNumber_of_icons = 0;
    gDAT_006a0958 = 0;
    gInventory_cycling = 0;
    gInventory_timeout = 0;
    gCount_cloaked_cars = 0;
    i = 0;
    do {
        link = &gRepulse_links[i];
        link->time = 0;
        if (link->actor->parent == gNon_track_actor) {
            BrActorRemove(link->actor);
        }
        i++;
    } while ((int)&gRepulse_links[i].actor < (int)&gINT_006a0a5c);
    gNext_goody_time = 0;
}

extern int gIt_or_fox;
extern tU32 gTime_for_punishment;
extern int gNot_shown_race_type_headup;
extern tU32 gLast_it_change;
extern tNet_game_player_info* gLast_lepper;
extern int gWinner_declared;

// FUNCTION: CARMA2_HW 0x0049b720
void C2_HOOK_FASTCALL InitPlayers(void) {
    tNet_game_player_info* player;
    int i;

    i = 0;
    if (0 < gNumber_of_net_players) {
        player = &gNet_players[0];
        do {
            PossibleService();
            switch (gCurrent_net_game->type) {
            case eNet_game_type_fight_to_death:
                player->score2 = 100;
                break;
            case eNet_game_type_checkpoint:
                player->score2 = 0xffff;
                break;
            case eNet_game_type_1:
            case eNet_game_type_2:
            case eNet_game_type_4:
            case eNet_game_type_5:
            case eNet_game_type_6:
            case eNet_game_type_foxy:
                player->score2 = 0;
                break;
            }
            player->credits = gCurrent_net_game->options.starting_credits;
            player->wasted = 0;
            player->reposition_time = 0;
            *(int*)player->field_0xd0 = 0;
            player->field_0x80 = 0;
            i++;
            player++;
        } while (i < gNumber_of_net_players);
    }
    gIt_or_fox = -1;
    if (gNet_mode == eNet_mode_host) {
        gLast_it_change = 0;
        gLast_lepper = NULL;
    }
    gTime_for_punishment = 0;
    gWinner_declared = 0;
    gNot_shown_race_type_headup = 1;
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


/* LoadTrack support: globals defined in 28-world3.c, callees without a shared prototype */
extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);
extern br_model* C2_HOOK_FASTCALL LoadModel(const char* pName);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path);
extern void C2_HOOK_FASTCALL ClosePackFile(tTWTVFS twt);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);
extern void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void);
extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);
extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);
extern void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM);
extern br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName);
extern br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern float C2_HOOK_FASTCALL GetAFloat(FILE* pF);
extern void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3);
extern void C2_HOOK_FASTCALL GetThreeScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3);
extern void C2_HOOK_FASTCALL GetFourScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4);
extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int gTrack_version;
extern tMaterial_exception* gMaterial_exceptions;
extern char gCurrent_load_directory[256];
extern int gRendering_accessories;
extern int gCount_smashable_noncars;
extern tSmashable_environment_name* gSmashable_noncars;
extern tSpecial_volume* gDefault_water_spec_vol_real;
extern br_pixelmap* gBack_screen;
extern tU8* gTrack_flic_buffer;
extern tU32 gTrack_flic_buffer_size;
extern tFlic_descriptor gTrack_flic_descriptor;
extern void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath);
extern void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage* pStorage, const char* pPath);
extern void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF);
extern void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);
extern void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF);
extern int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);
extern int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);
extern void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap);
extern void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF);
extern void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* pF);
extern void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);
extern void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void);
extern void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial);
extern void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* pRace_lighting_path);
extern intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag);
extern void C2_HOOK_FASTCALL ChangeDepthEffect(void);
extern void C2_HOOK_FASTCALL MungeForwardSky(void);
extern void C2_HOOK_FASTCALL MungeRearviewSky(void);
extern char gCurrent_load_name[256];
extern char gAdditional_model_path[256];
extern char gAdditional_actor_path[256];
extern br_model* gAdditional_models[1000];
extern int gNumber_of_additional_models;
extern br_actor* gLast_actor;
extern br_actor* gAdditional_actors;
extern tGraf_spec gGraf_specs[2];
extern int gGraf_spec_index;
extern br_angle gSky_image_width;
extern br_angle gSky_image_height;
extern br_angle gSky_image_underground;
extern int gTrack_depth_colour_red;
extern int gTrack_depth_colour_green;
extern int gTrack_depth_colour_blue;
extern tS8 gNon_car_spec_indices[100];
extern const char* gDepth_effect_names[3];
extern void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath);
extern void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec);
extern void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);
extern void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);
extern void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);
extern void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);
extern void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec);

/* GetFaceFlag: inlined by the original into LoadTrack (MungeTrackModel part) */
static int GetFaceFlag(int pIndex) {

    switch (pIndex) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 4;
    default:
        return 0;
    }
}

static void AddExceptionToList(tMaterial_exception** pList, tMaterial_exception* pItem) {

    pItem->next = *pList;
    *pList = pItem;
}

static void LoadExceptionsFile(const char* pPath) {
    FILE* f;
    int version;
    char delimiters[4] = "\t ,";
    char* str;
    char s[256];
    tMaterial_exception* matexc;

    f = DRfopen(pPath, "rt");
    if (f != NULL) {
        GetALineAndDontArgue(f, s);
        str = strtok(s, delimiters);
        if (DRStricmp(str, "VERSION") != 0) {
            FatalError(kFatalError_FileMustStartWith_SS, pPath, "VERSION");
        }
        str = strtok(NULL, delimiters);
        if (sscanf(str, "%d", &version) == 0 || version != 1) {
            FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pPath);
        }
        GetALineAndDontArgue(f, s);
        str = strtok(s, delimiters);
        while (DRStricmp(str, "end") != 0) {
            matexc = BrMemAllocate(sizeof(tMaterial_exception), kMem_exception);
            matexc->texture_name = BrMemAllocate(strlen(str) + 1, kMem_misc_string);
            strcpy(matexc->texture_name, str);
            matexc->flags = 0;
            for (str = strtok(NULL, delimiters); str != NULL && isalnum(str[0]); str = strtok(NULL, delimiters)) {
                if (DRStricmp(str, "nobilinear") == 0) {
                    matexc->flags |= eMaterial_exception_nobilinear;
                } else if (DRStricmp(str, "wrap") == 0) {
                    matexc->flags |= 0x8; /* the original ORs 8 for "wrap" */
                } else {
                    FatalError(kFatalError_Mysterious_SS, str, pPath);
                }
            }
            AddExceptionToList(&gMaterial_exceptions, matexc);
            GetALineAndDontArgue(f, s);
            str = strtok(s, delimiters);
        }
        PFfclose(f);
    }
}

static void LoadExceptionsFileForTrack(const char* pTrack_name) {
    tPath_name path;

    sprintf(path, "%s%s%s%s", pTrack_name, gDir_separator, gRenderer_fixup_basename, gRenderer_fixup_extension);
    LoadExceptionsFile(path);
}

static void FreeExceptions(void) {
    tMaterial_exception* matexc;
    tMaterial_exception* next_matexc;

    for (matexc = gMaterial_exceptions; matexc != NULL; matexc = next_matexc) {
        next_matexc = matexc->next;
        BrMemFree(matexc->texture_name);
        BrMemFree(matexc);
    }
    gMaterial_exceptions = NULL;
}

static int ModelIsATree(br_model* pModel, char* pName_replacement) {
    int i;

    for (i = 0; i < gTree_surgery_pass1_count; i++) {
        if (strstr(pModel->identifier, gTree_surgery_pass1[i].name) == pModel->identifier) {
            break;
        }
    }
    if (i == gTree_surgery_pass1_count) {
        return 0;
    }
    for (i = 0; i < gTree_surgery_pass2_count; i++) {
        if (strcmp(pModel->identifier, gTree_surgery_pass2[i].original) == 0) {
            strcpy(pName_replacement, gTree_surgery_pass2[i].replacement);
            return 1;
        }
    }
    return 0;
}

static void PerformTreeSurgery(tBrender_storage* pStorage, br_model* pModel, char* pName_replacement) {
    br_model* replacement;

    replacement = LoadModel(pName_replacement);
    if (replacement != NULL) {
        BrResFree(pModel->faces);
        BrResFree(pModel->vertices);
        pModel->faces = BrResAllocate(pModel, BrResSize(replacement->faces), BrResClass(replacement->faces));
        memcpy(pModel->faces, replacement->faces, BrResSize(replacement->faces));
        pModel->nfaces = replacement->nfaces;
        pModel->vertices = BrResAllocate(pModel, BrResSize(replacement->vertices), BrResClass(replacement->vertices));
        memcpy(pModel->vertices, replacement->vertices, BrResSize(replacement->vertices));
        pModel->nvertices = replacement->nvertices;
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

static void PossibleTreeSurgery(br_model* pModel) {
    char name_replacement[32];

    if (pModel != NULL && pModel->identifier != NULL) {
        if (ModelIsATree(pModel, name_replacement)) {
            PerformTreeSurgery(&gTrack_storage_space, pModel, name_replacement);
        }
    }
}

static void MungeTrackModel(br_model* pModel) {
    int* vertex_index_buffer;
    int (*face_index_buffer)[3];
    br_vertex* vertices;
    int i;
    int j;
    int l;
    int m;
    int s1a;
    int s1b;
    int s1c;
    int s2a;
    int s2b;
    int s2c;
    float d;
    br_vector3 tmp;
    br_vector3 d21;
    br_vector3 o31a;
    br_vector3 o31b;
    br_vector3 c2;

    vertex_index_buffer = BrMemAllocate((pModel->nvertices + 1) * sizeof(int), BR_MEMORY_APPLICATION);
    vertices = pModel->vertices - 1;
    face_index_buffer = BrMemAllocate(pModel->nfaces * 3 * sizeof(int), BR_MEMORY_APPLICATION);
    for (i = 1; i <= pModel->nvertices; i++) {
        if (vertex_index_buffer[i] == 0) {
            vertex_index_buffer[i] = i;
            for (j = i + 1; j <= pModel->nvertices; j++) {
                BrVector3Sub(&tmp, &vertices[i].p, &vertices[j].p);
                if (BrVector3LengthSquared(&tmp) < 1e-8f) {
                    vertex_index_buffer[j] = i;
                }
            }
        }
    }
    for (i = 0; i < pModel->nfaces; i++) {
        for (j = 0; j < 3; j++) {
            face_index_buffer[i][j] = vertex_index_buffer[1 + pModel->faces[i].vertices[j]];
        }
    }
    for (i = 0; i < pModel->nfaces - 1; i++) {
        for (j = i + 1; j < pModel->nfaces; j++) {
            for (l = 0; l < 3; l++) {
                s1a = face_index_buffer[i][l];
                s1b = face_index_buffer[i][(l + 1) % 3];
                for (m = 0; m < 3; m++) {
                    s2a = face_index_buffer[j][m];
                    s2b = face_index_buffer[j][(m + 1) % 3];
                    if ((s1a == s2a && s1b == s2b) || (s1a == s2b && s1b == s2a)) {
                        pModel->faces[j].flags |= GetFaceFlag(m);
                        BrVector3Sub(&d21, &pModel->vertices[s2b - 1].p, &pModel->vertices[s2a - 1].p);
                        s1c = face_index_buffer[i][(l + 2) % 3];
                        s2c = face_index_buffer[j][(m + 2) % 3];
                        BrVector3Sub(&o31b, &pModel->vertices[s2c - 1].p, &pModel->vertices[s2a - 1].p);
                        BrVector3Sub(&o31a, &pModel->vertices[s1c - 1].p, &pModel->vertices[s2c - 1].p);
                        BrVector3Cross(&c2, &o31b, &d21);
                        d = BrVector3Dot(&o31a, &c2);
                        if (d < .0001 && (((pModel->faces[i].material == NULL || !(pModel->faces[i].material->flags & BR_MATF_TWO_SIDED))
                                && (pModel->faces[j].material == NULL || !(pModel->faces[j].material->flags & BR_MATF_TWO_SIDED)))
                                || d >= -.0001)) {
                            pModel->faces[i].flags |= GetFaceFlag(l);
                        }
                    }
                }
            }
        }
    }
    BrMemFree(face_index_buffer);
    BrMemFree(vertex_index_buffer);
    pModel->flags |= BR_MODF_UPDATEABLE;
    BrModelUpdate(pModel, BR_MODU_FACES);
}

static void CheckNonCarModelBounds(void) {
    int i;
    int j;
    br_actor* actor;
    tNon_car_spec* non_car;
    br_bounds3* bb;
    br_bounds* bounds;

    for (i = 0; i < gProgram_state.track_spec.count_non_cars; i++) {
        actor = gProgram_state.track_spec.non_car_list[i];
        if (actor != NULL) {
            non_car = &gProgram_state.non_cars[gNon_car_spec_indices[10 * (actor->identifier[1] - '0') + (actor->identifier[2] - '0')] + 40 - 1];
            if (non_car != NULL && non_car->collision_info->actor == NULL) {
                non_car->collision_info->actor = actor;
                bb = &non_car->collision_info->bb2;
                bounds = &actor->model->bounds;
                for (j = 0; j < 3; j++) {
                    bounds->min.v[j] = MIN(bounds->min.v[j], bb->min.v[j]);
                    bounds->max.v[j] = MAX(bounds->max.v[j], bb->max.v[j]);
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* LoadTrack: based on carpocalypse2 src/carma2/common/world3.c:233 (GPL-3.0), rewritten from the disassembly.
 * The original inlines LoadExceptionsFileForTrack/LoadExceptionsFile/AddExceptionToList, GetThreeFloats/GetAFloat,
 * MungeTrackModel, PossibleTreeSurgery (ModelIsATree + PerformTreeSurgery), CheckNonCarModelBounds and FreeExceptions. */
// FUNCTION: CARMA2_HW 0x00504bf0
void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {
    int i;
    int j;
    int k;
    int killed_sky;
    int count_material_modifiers;
    int count_noncar_objects;
    int rstyle;
    int len;
    char s[256];
    char race_desc_path[256];
    char local_name[256];
    char actor_path[256];
    char local_directory[256];
    char local_race_path[256];
    char race_lighting_path[256];
    char* str;
    FILE* f;
    FILE* g;
    tTWTVFS twt;
    tTWTVFS twt2;
    br_pixelmap* sky;
    tU16 sky_pixels_high;
    float temp_float;
    br_material* material;
    br_vector3 tmp1;
    br_vector3 tmp2;
    tMaterial_modifiers* modifier;
    tNon_car_spec* non_car;

    killed_sky = 0;
    PrintMemoryDump(0, "AT THE START OF LOAD TRACK");
    strcpy(gCurrent_load_directory, "RACES");
    strcpy(gCurrent_load_name, pFile_name);
    gCurrent_load_name[strlen(gCurrent_load_name) - 4] = '\0';
    strcpy(local_directory, gCurrent_load_directory);
    strcpy(local_name, gCurrent_load_name);
    PathCat(gRace_path, gApplication_path, local_directory);
    strcpy(local_race_path, gRace_path);

    LoadExceptionsFileForTrack(local_race_path);

    PathCat(gRace_path, gRace_path, local_name);
    twt = OpenPackFileAndSetTiffLoading(gRace_path);
    PathCat(race_lighting_path, gRace_path, "LIGHTING.TXT");
    PathCat(race_desc_path, gRace_path, pFile_name);
    f = DRfopen(race_desc_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    GetALineAndDontArgue(f, s);
    strtok(s, "\t ,/");
    if (strcmp(s, "VERSION") == 0) {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &gTrack_version);
        if (gTrack_version == 8) {
            gTrack_version = 0;
        }
    } else {
        gTrack_version = 0;
    }
    if (gTrack_version > 0) {
        ReadGlobalLightingInfo(f);
    }

    /* Position of centre of start of grid */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pRace_info->initial_position.v[0] = temp_float;
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pRace_info->initial_position.v[1] = temp_float;
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pRace_info->initial_position.v[2] = temp_float;
    PossibleService();

    /* Direction that grid faces in */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pRace_info->initial_yaw = temp_float;
    PossibleService();

    /* # checkpoints */
    pRace_info->check_point_count = GetAnInt(f);
    for (i = 0; i < pRace_info->check_point_count; i++) {
        PossibleService();
        GetThreeInts(f, &pRace_info->checkpoints[i].timer_increments[0], &pRace_info->checkpoints[i].timer_increments[1], &pRace_info->checkpoints[i].timer_increments[2]);
        pRace_info->checkpoints[i].pos.v[0] = 0.f;
        pRace_info->checkpoints[i].pos.v[1] = 0.f;
        pRace_info->checkpoints[i].pos.v[2] = 0.f;
        pRace_info->checkpoints[i].count_quads = GetAnInt(f);
        for (j = 0; j < pRace_info->checkpoints[i].count_quads; j++) {
            for (k = 0; k < 4; k++) {
                GetThreeScalars(f, &pRace_info->checkpoints[i].quads[j].points[k].v[0], &pRace_info->checkpoints[i].quads[j].points[k].v[1], &pRace_info->checkpoints[i].quads[j].points[k].v[2]);
                pRace_info->checkpoints[i].pos.v[0] += pRace_info->checkpoints[i].quads[j].points[k].v[0];
                pRace_info->checkpoints[i].pos.v[1] += pRace_info->checkpoints[i].quads[j].points[k].v[1];
                pRace_info->checkpoints[i].pos.v[2] += pRace_info->checkpoints[i].quads[j].points[k].v[2];
            }
            BrVector3Sub(&tmp1, &pRace_info->checkpoints[i].quads[j].points[2], &pRace_info->checkpoints[i].quads[j].points[0]);
            BrVector3Sub(&tmp2, &pRace_info->checkpoints[i].quads[j].points[1], &pRace_info->checkpoints[i].quads[j].points[0]);
            BrVector3Cross(&pRace_info->checkpoints[i].normals[j], &tmp2, &tmp1);
        }
        pRace_info->checkpoints[i].pos.v[0] = pRace_info->checkpoints[i].pos.v[0] / (4 * pRace_info->checkpoints[i].count_quads);
        pRace_info->checkpoints[i].pos.v[1] = pRace_info->checkpoints[i].pos.v[1] / (4 * pRace_info->checkpoints[i].count_quads);
        pRace_info->checkpoints[i].pos.v[2] = pRace_info->checkpoints[i].pos.v[2] / (4 * pRace_info->checkpoints[i].count_quads);
    }

    PossibleService();
    strcpy(s, pFile_name);
    s[4] = '\0';
    PathCat(race_desc_path, gApplication_path, "RACES");
    PathCat(race_desc_path, race_desc_path, s);
    twt2 = OpenPackFile(race_desc_path);
    LoadAllImagesInDirectory(&gTrack_storage_space, race_desc_path);
    ClosePackFile(twt2);
    PossibleService();
    LoadAllImagesInDirectory(&gTrack_storage_space, gRace_path);
    PossibleService();
    LoadAllShadeTablesInDirectory(&gTrack_storage_space, gRace_path);
    PossibleService();
    LoadTrackMaterials(&gTrack_storage_space, gRace_path);
    PossibleService();
    LoadTrackModels(&gTrack_storage_space, gRace_path);

    for (i = 0; i < gTrack_storage_space.models_count; i++) {
        PossibleService();
        MungeTrackModel(gTrack_storage_space.models[i]);
    }
    PrintMemoryDump(0, "JUST LOADED IN TEXTURES/MATS/MODELS FOR TRACK");

    PathCat(actor_path, gRace_path, local_name);
    strcat(actor_path, ".ACT");
    pTrack_spec->the_actor = BrActorLoad(actor_path);
    PrintMemoryDump(0, "AFTER LOADING TRACK ACTORS");
    PossibleService();
    ReadSmashableEnvironment(f, gRace_path);
    PrintMemoryDump(0, "AFTER LOADING SMASHABLE ENVIRONMENT");
    ReadPedSpecs(f);
    PossibleService();
    PackFileRevertTiffLoading();
    ExtractColumns(pTrack_spec);
    PackFileRerevertTiffLoading();
    FinishUpLoadingPeds();
    PrintMemoryDump(0, "JUST EXTRACTED COLUMNS AND LOADED IN PEDS");

    gTrack_actor = pTrack_spec->the_actor;
    if (!gRendering_accessories && gNet_mode == eNet_mode_none) {
        rstyle = BR_RSTYLE_NONE;
        PossibleService();
        DRActorEnumRecurse(gTrack_actor, SetAccessoryRenderingCB, &rstyle);
    }
    BrActorAdd(gUniverse_actor, pTrack_spec->the_actor);

    /* Additional actor */
    GetALineAndDontArgue(f, s);
    str = strtok(strtok(s, "\t ,/"), ".");
    strcat(str, ".DAT");
    PathCat(gAdditional_model_path, gApplication_path, "MODELS");
    PathCat(gAdditional_model_path, gAdditional_model_path, str);
    PossibleService();
    gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, REC2_ASIZE(gAdditional_models));
    for (i = 0; i < gNumber_of_additional_models; i++) {
        gAdditional_models[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
    PossibleService();
    str = strtok(strtok(s, "\t ,/"), ".");
    strcat(str, ".ACT");
    PathCat(gAdditional_actor_path, gApplication_path, "ACTORS");
    PathCat(gAdditional_actor_path, gAdditional_actor_path, str);
    gAdditional_actors = BrActorLoad(gAdditional_actor_path);
    if (gAdditional_actors == NULL) {
        gAdditional_actors = BrActorAllocate(BR_ACTOR_NONE, NULL);
    }
    BrActorAdd(gUniverse_actor, gAdditional_actors);
    gLast_actor = NULL;

    /* Name of sky texture pixelmap (or "none") */
    GetAString(f, s);
    if (!gAusterity_mode && strcmp(&s[strlen(s) - 4], ".FLI") == 0) {
        gTrack_flic_buffer = NULL;
        if (!LoadFlicData(s, &gTrack_flic_buffer, &gTrack_flic_buffer_size)) {
            FatalError(kFatalError_CantFindFile_S, s);
        }
        StartFlic(s, -1, &gTrack_flic_descriptor, gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
        sky = DRPixelmapAllocate(gScreen->type, gTrack_flic_descriptor.width, gTrack_flic_descriptor.height,
            BrMemAllocate(((gTrack_flic_descriptor.width + 3) & ~3) * gTrack_flic_descriptor.height, kMem_video_pixels), 0);
        BrMapAdd(sky);
        AssertFlicPixelmap(&gTrack_flic_descriptor, sky);
    } else {
        gTrack_flic_buffer = NULL;
        sky = BrMapFind(s);
    }
    if (gAusterity_mode && sky != NULL) {
        for (i = 0; i < gTrack_storage_space.pixelmaps_count; i++) {
            if (gTrack_storage_space.pixelmaps[i] == sky) {
                BrMapRemove(gTrack_storage_space.pixelmaps[i]);
                BrPixelmapFree(gTrack_storage_space.pixelmaps[i]);
                gTrack_storage_space.pixelmaps[i] = gTrack_storage_space.pixelmaps[gTrack_storage_space.pixelmaps_count - 1];
                gTrack_storage_space.pixelmaps_count--;
                break;
            }
        }
        sky = NULL;
        killed_sky = 1;
    }
    gProgram_state.default_depth_effect.sky_texture = sky;
    if (sky != NULL) {
        sky_pixels_high = sky->height;
    } else {
        sky_pixels_high = 100;
    }
    PossibleService();

    /* Horizontal repetitions of sky texture */
    gSky_image_width = BrDegreeToAngle(360.f / GetAnInt(f));
    /* Vertical size of sky texture (degrees) */
    gSky_image_height = BrDegreeToAngle(GetAFloat(f));
    /* Position of horizon (pixels below top) */
    gSky_image_underground = (sky_pixels_high - GetAnInt(f)) * gSky_image_height / sky_pixels_high;
    MungeForwardSky();
    MungeRearviewSky();
    PossibleService();

    /* Depth cue mode ("none", "dark" or "fog") */
    gProgram_state.default_depth_effect.type = GetALineAndInterpretCommand(f, gDepth_effect_names, REC2_ASIZE(gDepth_effect_names));
    /* Degree of fog/darkness */
    GetPairOfInts(f, &gProgram_state.default_depth_effect.start, &gProgram_state.default_depth_effect.end);
    /* Depth cue colour (red, green, blue ) */
    GetThreeInts(f, &gProgram_state.default_depth_effect.colour.red, &gProgram_state.default_depth_effect.colour.green, &gProgram_state.default_depth_effect.colour.blue);
    gTrack_depth_colour_red = gProgram_state.default_depth_effect.colour.red;
    gTrack_depth_colour_green = gProgram_state.default_depth_effect.colour.green;
    gTrack_depth_colour_blue = gProgram_state.default_depth_effect.colour.blue;
    ChangeDepthEffect();
    if (killed_sky && gProgram_state.default_depth_effect.type != eDepth_effect_fog) {
        gProgram_state.default_depth_effect.type = eDepth_effect_fog;
        gProgram_state.default_depth_effect.start = 7;
        gProgram_state.default_depth_effect.end = 0;
        gProgram_state.default_depth_effect.colour.red = 0xf8;
        gProgram_state.default_depth_effect.colour.green = 0xf8;
        gProgram_state.default_depth_effect.colour.blue = 0xf8;
    }
    PossibleService();
    ReadSpecialVolumes(f);
    ReadSoundGenerators(pTrack_spec, f);

    /* REFLECTIVE WINDSCREEN SPECIFICATIONS */
    GetAString(f, s);
    gProgram_state.standard_screen = BrMapFind(s);
    GetAString(f, s);
    gProgram_state.standard_screen_dark = BrMapFind(s);
    GetAString(f, s);
    gProgram_state.standard_screen_fog = BrMapFind(s);
    gProgram_state.special_screens_count = GetAnInt(f);
    if (gProgram_state.special_screens_count != 0) {
        gProgram_state.special_screens = BrMemAllocate(sizeof(tSpecial_screen) * gProgram_state.special_screens_count, kMem_special_screen);
        for (i = 0; i < gProgram_state.special_screens_count; i++) {
            GetFourScalars(f,
                &gProgram_state.special_screens[i].min_x,
                &gProgram_state.special_screens[i].min_z,
                &gProgram_state.special_screens[i].max_x,
                &gProgram_state.special_screens[i].max_z);
            GetAString(f, s);
            gProgram_state.special_screens[i].material = BrMaterialFind(s);
        }
    }
    PossibleService();

    /* MAP DETAILS */
    GetAString(f, s);
    pRace_info->map_image = BrMapFind(s);
    if (pRace_info->map_image == NULL) {
        strtok(s, ".");
        pRace_info->map_image = BrMapFind(s);
        if (pRace_info->map_image == NULL) {
            Uppercaseificate(s, s);
            pRace_info->map_image = BrMapFind(s);
        }
    }
    BRPM_convert(pRace_info->map_image, gBack_screen->type);
    PrintMemoryDump(0, "JUST LOADING SKY/SPEC VOLS/SCREENS/MAP");
    for (i = 0; i < 4; i++) {
        GetThreeScalars(f, &pRace_info->map_transformation.m[i][0], &pRace_info->map_transformation.m[i][1], &pRace_info->map_transformation.m[i][2]);
    }
    if (gGraf_specs[gGraf_spec_index].total_width != 640 || gGraf_specs[gGraf_spec_index].total_height != 480) {
        pRace_info->map_transformation.m[3][0] -= 320.f;
        pRace_info->map_transformation.m[3][1] -= 240.f;
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BR_ANGLE_DEG(90));
        BrMatrix34PostScale(&pRace_info->map_transformation,
            gGraf_specs[gGraf_spec_index].total_width / 640.f,
            0.f,
            gGraf_specs[gGraf_spec_index].total_height / 480.f);
        BrMatrix34PostRotateX(&pRace_info->map_transformation, -BR_ANGLE_DEG(90));
        pRace_info->map_transformation.m[3][0] += gGraf_specs[gGraf_spec_index].total_width / 2;
        pRace_info->map_transformation.m[3][1] += gGraf_specs[gGraf_spec_index].total_height / 2;
    }

    /* START OF FUNK */
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, -2, 1080, NULL);
    /* START OF GROOVE */
    GetALineAndDontArgue(f, s);
    AddGroovidelics(f, -2, gUniverse_actor, 1080, 0);
    PossibleService();
    PrintMemoryDump(0, "JUST LOADING IN FUNKS AND GROOVES");

    for (i = 0; i < gTrack_storage_space.models_count; i++) {
        PossibleService();
        PossibleTreeSurgery(gTrack_storage_space.models[i]);
        if (gTrack_storage_space.models[i] != NULL && (gTrack_storage_space.models[i]->flags & (BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE))) {
            if (gTrack_storage_space.models[i]->identifier != NULL && gTrack_storage_space.models[i]->identifier[0] != '-') {
                gTrack_storage_space.models[i]->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
            }
            for (j = 0; j < V11MODEL(gTrack_storage_space.models[i])->ngroups; j++) {
                material = V11MODEL(gTrack_storage_space.models[i])->groups[j].face_colours.materials[0] = gTrack_storage_space.models[i]->faces[*V11MODEL(gTrack_storage_space.models[i])->groups[j].face_user].material;
                if (material != NULL) {
                    SmoothificateWorldMaterial(material);
                    if (material->index_shade == NULL) {
                        material->index_shade = BrTableFind("DRRENDER.TAB");
                        BrMaterialUpdate(material, BR_MATU_ALL);
                    }
                }
            }
            if (gTrack_storage_space.models[i]->flags & BR_MODF_UPDATEABLE) {
                BrModelUpdate(gTrack_storage_space.models[i], BR_MODU_ALL);
            } else {
                DodgyModelUpdate(gTrack_storage_space.models[i]);
            }
        }
    }
    PossibleService();
    PrintMemoryDump(0, "JUST ABOUT TO LOAD IN AI WORLD");
    LoadAIWorldTrackInfo(f);
    PrintMemoryDump(0, "JUST LOADED IN AI WORLD");
    MungeSmashMaterialNames();
    PackFileRevertTiffLoading();

    /* number of material modifiers */
    count_material_modifiers = GetAnInt(f);
    for (i = 0; i < count_material_modifiers; i++) {
        PossibleService();
        modifier = &pRace_info->material_modifiers[i];
        modifier->car_wall_friction = GetAFloat(f);
        modifier->tyre_road_friction = GetAFloat(f);
        modifier->down_force = GetAFloat(f);
        modifier->bumpiness = GetAFloat(f);
        modifier->tyre_noise_index = GetAnInt(f);
        modifier->crash_noise_index = GetAnInt(f);
        modifier->scrape_noise_index = GetAnInt(f);
        modifier->sparkiness = GetAFloat(f);
        modifier->smoke_type = GetAnInt(f);
        GetAString(f, s);
        str = strtok(s, ".");
        if (strcmp(s, "none") == 0 || strcmp(s, "NONE") == 0 || strcmp(s, "0") == 0 || strcmp(s, "1") == 0) {
            modifier->skid_mark_material = NULL;
        } else {
            len = strlen(str);
            strcat(str, ".PIX");
            LoadSinglePixelmap(&gTrack_storage_space, str);
            str[len] = '\0';
            strcat(str, ".MAT");
            modifier->skid_mark_material = LoadSingleMaterial(&gTrack_storage_space, str);
        }
    }
    PackFileRerevertTiffLoading();
    for (i = count_material_modifiers; i < 10; i++) {
        modifier = &pRace_info->material_modifiers[i];
        modifier->car_wall_friction = 1.f;
        modifier->tyre_road_friction = 1.f;
        modifier->down_force = 1.f;
        modifier->bumpiness = 0.f;
        modifier->tyre_noise_index = 0;
        modifier->crash_noise_index = 0;
        modifier->scrape_noise_index = 0;
        modifier->sparkiness = 1.f;
        modifier->smoke_type = 1;
        modifier->skid_mark_material = NULL;
    }
    pRace_info->material_modifiers[10].car_wall_friction = 1.f;
    pRace_info->material_modifiers[10].tyre_road_friction = 1.f;
    pRace_info->material_modifiers[10].down_force = 0.f;
    pRace_info->material_modifiers[10].bumpiness = 0.f;
    pRace_info->material_modifiers[10].tyre_noise_index = -1;
    pRace_info->material_modifiers[10].crash_noise_index = 0;
    pRace_info->material_modifiers[10].scrape_noise_index = 0;
    pRace_info->material_modifiers[10].sparkiness = 0.f;
    pRace_info->material_modifiers[10].smoke_type = 1;
    pRace_info->material_modifiers[10].skid_mark_material = NULL;
    gDefault_water_spec_vol_real->material_modifier_index = 10;

    /* Non CarObjects */
    count_noncar_objects = GetAnInt(f);
    if (count_noncar_objects > 40) {
        PDFatalError("Too many non-car types");
    }
    gProgram_state.num_non_car_spaces = count_noncar_objects + gCount_smashable_noncars + 40;
    non_car = BrMemCalloc(gProgram_state.num_non_car_spaces, sizeof(tNon_car_spec), kMem_non_car_spec);
    if (non_car == NULL && count_noncar_objects != 0) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    gProgram_state.non_cars = non_car;
    memset(gNon_car_spec_indices, 0, sizeof(gNon_car_spec_indices));
    for (i = 0; i < 40; i++, non_car++) {
        non_car->driver = eDriver_non_car_unused_slot;
        non_car->collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
        non_car->collision_info->owner = non_car;
        non_car->collision_info->flags_0x238 = 1;
        non_car->collision_info->field_0x1a0 = 0xffff;
        non_car->collision_info->field_0x1a4 = 0;
    }
    for (i = 0; i < count_noncar_objects + gCount_smashable_noncars; i++, non_car++) {
        PossibleService();
        non_car->collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
        non_car->collision_info->owner = non_car;
        non_car->collision_info->flags_0x238 = 1;
        non_car->collision_info->field_0x1a0 = 0xffff;
        non_car->collision_info->field_0x1a4 = 0;
        if (i < count_noncar_objects) {
            GetAString(f, s);
        } else {
            strcpy(s, gSmashable_noncars[i - count_noncar_objects]);
        }
        PathCat(race_desc_path, gApplication_path, "NONCARS");
        PathCat(race_desc_path, race_desc_path, s);
        g = DRfopen(race_desc_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CantOpen_S, race_desc_path);
        }
        ReadNonCarMechanicsData(g, non_car);
        PossibleService();
        gNon_car_spec_indices[non_car->index] = i + 1;
        PFfclose(g);
    }
    DisposeSmashEnvNonCars();

    CheckNonCarModelBounds();

    GetSmokeShadeTables(f);
    pRace_info->count_network_start_points = GetAnInt(f);
    for (i = 0; i < pRace_info->count_network_start_points; i++) {
        GetThreeScalars(f, &pRace_info->net_starts[i].pos.v[0], &pRace_info->net_starts[i].pos.v[1], &pRace_info->net_starts[i].pos.v[2]);
        pRace_info->net_starts[i].yaw = GetAFloat(f);
    }
    LoadInKevStuff(f);
    gYon_multiplier = GetAFloat(f);
    GetAString(f, s);
    if (DRStricmp(s, pFile_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pFile_name);
    }
    PFfclose(f);

    FreeExceptions();
    PrintMemoryDump(0, "FINISHED LOADING TRACK");
    LoadStaticLightingForRace(race_lighting_path);
    ClosePackFileAndSetTiffLoading(twt);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* Crush-list counters and lists cleared by ResetCrushSystems (crush.c globals) */
// GLOBAL: CARMA2_HW 0x006796bc
int gCount_queued_drone_crushes;

// GLOBAL: CARMA2_HW 0x006796c8
tCrush_list_item gCrush_lists[8];

// GLOBAL: CARMA2_HW 0x0067b7c8
int gCount_net_crush_reattach_bit_list;

// GLOBAL: CARMA2_HW 0x0067b7cc
int gCount_car_damage_crush_list;

// GLOBAL: CARMA2_HW 0x0067b838
int gCount_net_crush_semi_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067bd58
int gCount_toggled_doors;

// GLOBAL: CARMA2_HW 0x0067bd60
int gCount_net_crush_full_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067bdf0
int gCount_net_crush_detach_list;

// GLOBAL: CARMA2_HW 0x0067bdfc
int gINT_0067bdfc;

// GLOBAL: CARMA2_HW 0x0067be08
tU8 gCrush_data_entry_counter;

/* powerup globals owned by ResetPowerups */
// GLOBAL: CARMA2_HW 0x006a0944
int gNumber_of_icons;

// GLOBAL: CARMA2_HW 0x006a0958
undefined4 gDAT_006a0958;

// GLOBAL: CARMA2_HW 0x006a0aec
tU32 gNext_goody_time;

/* netgame globals written by InitPlayers */
// GLOBAL: CARMA2_HW 0x0068d924
tNet_game_player_info* gLast_lepper = NULL;

// GLOBAL: CARMA2_HW 0x0068d928
tU32 gLast_it_change = 0;

// GLOBAL: CARMA2_HW 0x0068d938
int gWinner_declared = 0;
