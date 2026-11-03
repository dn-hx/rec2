#include "21-mainloop.h"

#include "41-utility.h"
#include "69-sound.h"
#include "rec2_macros.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "68-timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef REC2_MATCHING
#include <windows.h>
#endif

// GLOBAL: CARMA2_HW 0x0068c814
// GLOBAL: CARMA2_SW 0x00686868
tU32 gOld_camera_time;

// GLOBAL: CARMA2_HW 0x0068c820
// GLOBAL: CARMA2_SW 0x00686874
tU32 gActual_last_tick_count;

// GLOBAL: CARMA2_HW 0x0068c824
// GLOBAL: CARMA2_SW 0x00686878
tU32 gAverage_frame_period;

// GLOBAL: CARMA2_HW 0x00761ef0
int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */

// GLOBAL: CARMA2_HW 0x0074b740
int gAuto_increase_credits_dt[3];

// GLOBAL: CARMA2_HW 0x0074b720
tU32 gNet_auto_increase_credits_dt[8];

// GLOBAL: CARMA2_HW 0x0068c760
int gQueued_wasted_messages_count;

// GLOBAL: CARMA2_HW 0x0068c77c
tU32 gLast_wasted_message_start;

// GLOBAL: CARMA2_HW 0x0068c748
int gQueued_wasted_messages[5] = { 0 };

// GLOBAL: CARMA2_HW 0x0068c75c
int gCompletion_bonus_post_race = 0;

// CalculateFrameRate

// LoseOldestWastedMassage

// FUNCTION: CARMA2_HW 0x00492620
// FUNCTION: CARMA2_SW 0x0048f840
void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex) {

    if (gQueued_wasted_messages_count == REC2_ASIZE(gQueued_wasted_messages)) {
        int i;

        for (i = 1; i < gQueued_wasted_messages_count; i++) {
            gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
        }
        gQueued_wasted_messages_count = REC2_ASIZE(gQueued_wasted_messages) - 1;
        gLast_wasted_message_start = GetTotalTime();
    }
    if (gQueued_wasted_messages_count == 0) {
        gLast_wasted_message_start = GetTotalTime();
    }
    gQueued_wasted_messages[gQueued_wasted_messages_count] = pIndex;
    gQueued_wasted_messages_count++;
}

// MungeHeadups

// FUNCTION: CARMA2_HW 0x00492680
// FUNCTION: CARMA2_SW 0x0048f8a0
void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (gAction_replay_mode) {
        tU32 camera_period;

        gFrame_period = abs((int)(gLast_replay_frame_time - gLast_tick_count));
        gLast_tick_count = gLast_replay_frame_time;
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (gOld_camera_time != 0) {
            camera_period = new_camera_tick_count - gOld_camera_time;
        } else {
            camera_period = 0;
        }
        *pCamera_period = camera_period;
        gOld_camera_time = new_camera_tick_count;
        if (gFrame_period != 0) {
            *pCamera_period = gFrame_period;
        }
    } else {
        int new_period;

        new_tick_count = PDGetTotalTime();
        gLast_tick_count += gFrame_period;
        error = new_tick_count - gLast_tick_count;
        gFrame_period = new_tick_count - gActual_last_tick_count;
        if (gFrame_period > 500 && new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
            gLast_tick_count = new_tick_count;
            if (gNet_mode != eNet_mode_none) {
                switch (gNet_mode) {
                case eNet_mode_client:
                    gProgram_state.current_car.collision_info->field_0x49c = 0;
                    break;
                case eNet_mode_host:
                    break;
                default:
                    break;
                }
            }
        }
        gAverage_frame_period = 9 * gAverage_frame_period / 10 + gFrame_period;
        if (new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
        }
        new_period = gFrame_period + error;
        if (new_period <= 0) {
            gLast_tick_count = new_tick_count;
        } else {
            gFrame_period = new_period;
        }
        *pCamera_period = gFrame_period;
        gActual_last_tick_count = new_tick_count;
    }
    if (gFrame_period < 10) {
        gFrame_period = 10;
        gLast_tick_count = new_tick_count;
    } else if (gFrame_period > 1000) {
        gFrame_period = 1000;
        gLast_tick_count = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
}

// CheckTimer

// MungeRaceFinished

// FUNCTION: CARMA2_HW 0x004940e0
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {
    extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);
    extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
    extern void C2_HOOK_FASTCALL ResetPedNearness(void);
    extern br_vector3 gPrev_camera_position;
    extern float gCamera_to_horiz_angle;
    extern float gYon_squared;

    gOur_pos = &gPlayer_car_master_actor->t.t.translate.t;
    PositionExternalCamera(&gProgram_state.current_car, pCamera_period);
    BrActorToActorMatrix34(&gCamera_to_world, gCamera, gUniverse_actor);
    BrActorToActorMatrix34(&gRearview_camera_to_world, gRearview_camera, gUniverse_actor);
    gCamera_to_horiz_angle = FastScalarArcTan2(gCamera_to_world.m[2][1], gCamera_to_world.m[1][1]);
    gYon_squared = gYon_multiplier * gYon_multiplier * ((br_camera*)gCamera->type_data)->yon_z * ((br_camera*)gCamera->type_data)->yon_z;
    if ((REC2_SQR(gCamera_to_world.m[3][0] - gPrev_camera_position.v[0]) + REC2_SQR(gCamera_to_world.m[3][1] - gPrev_camera_position.v[1])) + REC2_SQR(gCamera_to_world.m[3][2] - gPrev_camera_position.v[2]) > 400.0) {
        ResetPedNearness();
    }
    gPrev_camera_position = *(br_vector3*)gCamera_to_world.m[3];
}

/* MainGameLoop: externs for the callees and globals it needs (local, no header changes) */
/* owned by 23-piping.c; do NOT redefine them here (duplicate 0x403d00/0x403d20 cost both 100%) */
extern int C2_HOOK_FASTCALL ARGoForwards(void);
extern int C2_HOOK_FASTCALL ARGoBackwards(void);
extern tCar_spec* gCar_to_view;
extern tU32 gCar_to_view_id;
extern tU32 gCamera_period;
extern br_vector3 gPrev_camera_position;
extern int gHeadup_detail_level;
extern char gHeadup_oppo_ped_text[64];
extern int gCount_killed_peds;
extern int gPed_count;
extern int gTotal_count_smash_peds;
extern int gCredits_won_headup;
extern int gPed_kill_count_headup;
extern int gCar_kill_count_headup;
extern int gTimer_headup;
extern int gLaps_headup;
extern tU32 gPalette_fade_time;
extern int gEntering_message;
extern br_actor* gAdditional_actors;
extern int gInitialised_grid;
extern int gIt_or_fox;
extern int gNum_active_cars;
extern tCar_spec* gActive_car_list[139];
extern int gReceived_game_scores;
extern tRace_over_reason gRace_over_reason;
extern int gTimers_stack[8];
extern int gTimers_max_index;
extern int gTimers_stack_size;
extern tTimer gTimers[19];
extern tU32 gWasted_last_flash;
extern tU32 gFrame_rate_last_time;
extern int gLast_credit_headup__mainloop;
extern int gWasted_flash_state;
extern int gLast_time_headup;
extern tU32 gLast_net_auto_increase_credits;
extern int gTotal_bonus_post_race;
extern float gFLOAT_0068c778;
extern int gTime_bonus;
extern tTime_bonus_state gTime_bonus_state;
extern tU32 gTime_bonus_start;
extern int gFrame_rate_last_rates[30];
extern int gTime_bonus_headup;
extern int gCredits_post_race;
extern int gRace_bonus_headup;
extern tU32 gLast_rattle_time;
extern int gLast_time_in_seconds;
extern tU32 gPrev_task_switched_message;
extern int gWindow_inactive;
extern void C2_HOOK_FASTCALL ARMainLoopStart(void);
extern void C2_HOOK_FASTCALL RevertPalette(void);
extern void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void);
extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);
extern void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);
extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);
extern void C2_HOOK_FASTCALL Timers_Init(void);
extern void C2_HOOK_FASTCALL Timers_StartFrame(void);
extern void C2_HOOK_FASTCALL Timers_EndFrame(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL ResetPalette(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL CyclePollKeys(void);
extern void C2_HOOK_FASTCALL InitSmashQueue(void);
extern void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing);
extern void C2_HOOK_FASTCALL InitialiseExtraRenders(void);
extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);
extern void C2_HOOK_FASTCALL ToggleMap(void);
extern void C2_HOOK_FASTCALL ResetGrooveFlags(void);
extern void C2_HOOK_FASTCALL ServiceGameInRace(void);
extern void C2_HOOK_FASTCALL EnterUserMessage(void);
extern void C2_HOOK_FASTCALL ARService(void);
extern void C2_HOOK_FASTCALL ResetLollipopQueue(void);
extern void C2_HOOK_FASTCALL MungePalette(void);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index);
extern void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index);
extern void C2_HOOK_FASTCALL ClearQueuedHeadups(void);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text);
extern void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period);
/* the original passes gFrame_period in ecx to PollCarControls and DoWheelDamage (rec2 defines them with no parameter) */
extern void C2_HOOK_FASTCALL PollCarControls(void);
#ifdef REC2_MATCHING
typedef void (C2_HOOK_FASTCALL *tML_frame_period_proc)(tU32 pFrame_period);
#define ML_CALL_WITH_FRAME_PERIOD(F) ((tML_frame_period_proc)(F))(gFrame_period)
#else
#define ML_CALL_WITH_FRAME_PERIOD(F) (F)()
#endif
extern void C2_HOOK_FASTCALL MungePedestrians(void);
extern void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference);
extern void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 last_frame_time, tU32 pTime_difference);
/* the original passes gFrame_period in ecx (rec2 defines it with no parameter) */
extern void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void);
extern void C2_HOOK_FASTCALL PipeCarPositions(void);
extern void C2_HOOK_FASTCALL CrushBendFlapRend(void);
extern void C2_HOOK_FASTCALL DoNetGameManagement(void);
extern void C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime);
extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);
extern void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period);
extern void C2_HOOK_FASTCALL MungeNapalm(void);
extern void C2_HOOK_FASTCALL DoPowerupPeriodics(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL CheckCheckpoints(void);
extern void C2_HOOK_FASTCALL MungeEnvironmentalSound(void);
extern void C2_HOOK_FASTCALL ChangingView(void);
extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL FunkThoseTronics(void);
extern void C2_HOOK_FASTCALL GrooveThoseDelics(void);
extern void C2_HOOK_FASTCALL DoWheelDamage(void);
extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);
extern void C2_HOOK_FASTCALL MungeJoystickHeadups(void);
extern void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);
extern void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);
extern void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);
extern void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime);
extern void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pArg1);
extern void C2_HOOK_FASTCALL ChangeDepthEffect(void);
extern void C2_HOOK_FASTCALL SkidsPerFrame(void);
extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);
extern void C2_HOOK_FASTCALL MungeGlassFragments(void);
extern void C2_HOOK_FASTCALL AnimateSky(void);
extern void C2_HOOK_FASTCALL MungeExplosions(void);
extern void C2_HOOK_FASTCALL LastChanceForPedEffects(void);
extern void C2_HOOK_FASTCALL DoLODCarModels(void);
extern void C2_HOOK_FASTCALL MungeLightingEffects(void);
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern void C2_HOOK_FASTCALL DoComplexCarModels(void);
extern void C2_HOOK_FASTCALL CheckReplayTurnOn(void);
extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);
extern void C2_HOOK_FASTCALL EnsurePaletteUp(void);
extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);
extern void C2_HOOK_FASTCALL DoOptionsMenu(void);
extern void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern void C2_HOOK_FASTCALL PollActionReplayControls(tU32* pFrame_period, tU32* pAverage_frame_period);
extern void C2_HOOK_FASTCALL InitialiseActionReplay(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PHILDisable(void);
extern void C2_HOOK_FASTCALL SuspendPendingFlic(void);
extern void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void);
extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);
extern void C2_HOOK_FASTCALL ResetCarScreens(void);
extern void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason);
extern void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);

#ifdef REC2_MATCHING
/* PDPageInMemory (0x004928a0) is inlined here by the original */
typedef struct tML_heap_block_information {
    undefined field_0x0[0x48];
    struct tML_heap_block_information* next;
    undefined field_0x4c[0x26];
    tU16 field_0x72;
} tML_heap_block_information;

typedef union {
    tU32 flags;
    tU8 data[1];
} tML_heap_memory;

extern SYSTEM_INFO gSystem_info;
extern DWORD gPage_size;

static void MLPageInMemory(void* pMemory) {
    tML_heap_block_information* heap_block;

    heap_block = pMemory;
    if (!IsBadReadPtr(heap_block, sizeof(tML_heap_block_information)) && heap_block->field_0x72 == 0x4948) {
        for (; heap_block != NULL; heap_block = heap_block->next) {
            tML_heap_memory* block;

            block = (tML_heap_memory*)(heap_block + 1);
            for (;;) {
                DWORD size = block->flags & 0x5ffffffc;
                DWORD current;
                if (size == 0) {
                    break;
                }
                if (!(block->flags & 0x1)) {
                    if (gPage_size == 0) {
                        GetSystemInfo(&gSystem_info);
                        gPage_size = gSystem_info.dwPageSize;
                    }
                    for (current = 16 * gPage_size; current < size - 4; current += gPage_size) {
                        volatile tU8* ptr1 = &block->data[current + 16 - 16 * gPage_size];
                        volatile tU8* ptr2 = &block->data[current + 16];
                        *ptr1 = *ptr1;
                        *ptr2 = *ptr2;
                    }
                }
                block = (tML_heap_memory*)((BYTE*)block + size);
            }
        }
    }
}

static void MLPageInProcessMemory(void) {
    MLPageInMemory(GetProcessHeap());
    Sleep(1500);
}
#else
#define MLPageInProcessMemory PDPageInProcessMemory
#endif

static void MLTimersPush(int pType) {

    gTimers_stack[gTimers_stack_size] = pType;
    if (gTimers_stack_size != 0) {
        gTimers[gTimers_stack[gTimers_stack_size - 1]].durations[gTimers[gTimers_stack[gTimers_stack_size - 1]].index] += PDGetMicroseconds() - gTimers[gTimers_stack[gTimers_stack_size - 1]].start_time;
    } else {
        gTimers[gTimers_max_index].durations[gTimers[gTimers_max_index].index] += PDGetMicroseconds() - gTimers[gTimers_max_index].start_time;
    }
    gTimers[pType].start_time = PDGetMicroseconds();
    gTimers_stack_size += 1;
}

static void MLTimersPop(int pType) {

    gTimers[pType].durations[gTimers[pType].index] += PDGetMicroseconds() - gTimers[pType].start_time;
    if (gTimers_stack_size > 1) {
        gTimers[gTimers_stack[gTimers_stack_size - 1]].start_time = PDGetMicroseconds();
    } else {
        gTimers[gTimers_max_index].start_time = PDGetMicroseconds();
    }
    gTimers_stack_size -= 1;
}

static void MLTimersForce(int pType) {

    gTimers[gTimers_max_index].durations[gTimers[gTimers_max_index].index] += PDGetMicroseconds() - gTimers[gTimers_max_index].start_time;
    gTimers_max_index = pType;
    gTimers[gTimers_max_index].start_time = PDGetMicroseconds();
}

// CheckActiveCarList
static void CheckActiveCarList(void) {
    int i;
    int new_count;

    for (i = 0, new_count = 0; i < gNum_active_cars; i++) {
        tCar_spec* c;

        c = gActive_car_list[i];
        if (c != NULL && c->driver > 5) {
            if (new_count != i) {
                gActive_car_list[new_count] = c;
            }
            new_count += 1;
        }
    }
    gNum_active_cars = new_count;
}

static void CalculateFrameRate(void) {
    tU32 new_time;
    int i;
    int new_rate;

    new_time = PDGetTotalTime();
    if (new_time != gFrame_rate_last_time) {
        new_rate = 10000 / (new_time - gFrame_rate_last_time);
        gFrame_rate = new_rate;
        for (i = 0; i < 30; i++) {
            gFrame_rate += gFrame_rate_last_rates[i];
        }
        gFrame_rate /= 31;
        for (i = 0; i < 29; i++) {
            gFrame_rate_last_rates[i] = gFrame_rate_last_rates[i + 1];
        }
        gFrame_rate_last_rates[29] = new_rate;
        gFrame_rate_last_time = new_time;
    }
}

static void LoseOldestWastedMassage(void) {
    int i;

    for (i = 1; i < gQueued_wasted_messages_count; i++) {
        gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
    }
    gQueued_wasted_messages_count -= 1;
    gLast_wasted_message_start = GetTotalTime();
}

static void CheckTimer(void) {
    tS32 time_in_seconds;

    if (!gFreeze_timer && !gCountdown && !gRace_finished) {
        if (gTimer <= gFrame_period) {
            gTimer = 0;
            RaceCompleted(eRace_over_out_of_time);
        } else {
            if (gNet_mode == eNet_mode_none) {
                gTimer -= gFrame_period;
            }
            time_in_seconds = (tS32)(gTimer + 500) / 1000;
            if (time_in_seconds != gLast_time_in_seconds && time_in_seconds <= 10) {
                DRS3StartSound(gPedestrians_outlet, 1001);
            }
            gLast_time_in_seconds = time_in_seconds;
        }
    }
}

static int MungeRaceFinished(void) {

    if (!gRace_finished || gAction_replay_mode || (gNet_mode != eNet_mode_none && gRace_over_reason == eRace_not_over_yet)) {
        return 0;
    }
    if (gNet_mode == eNet_mode_none && gRace_over_reason <= 3 && gCredits_post_race < 0) {
        gCredits_post_race = gProgram_state.credits;
        gCompletion_bonus_post_race = (&gCurrent_race.completion_bonus)[gCurrent_race.race_spec->race_type == kRaceType_Carma1 ? gRace_over_reason : 0][gProgram_state.skill_level];
        gTotal_bonus_post_race = gCredits_per_second_time_bonus[gProgram_state.skill_level] * ((int)gTimer / 1000) + gCompletion_bonus_post_race;
        if (!GetRuntimeVariable(99)) {
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(290));
        }
    }
    if (gRace_finished <= gFrame_period) {
        if (gTimer == 0 || gNet_mode != eNet_mode_none || gRace_over_reason == eRace_over_6 || GetRuntimeVariable(99) != 0) {
            gRace_finished = 0;
            return 1;
        }
        gRace_finished = 15 * (gTimer + 300);
        gRace_bonus_headup = NewTextHeadupSlot(9, 0, 0, -4, "");
        gTime_bonus_headup = NewTextHeadupSlot(10, 0, 0, -4, "");
        gTime_bonus = 0;
        gTime_bonus_start = GetTotalTime();
        gTime_bonus_state = eTime_bonus_initial_pause;
    } else {
        gRace_finished -= gFrame_period;
    }
    return PDKeyDown(51) || PDKeyDown(52);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* (GPL-3.0) and the REA pseudocode; MungeHeadups is inlined by the original */
// PARTIAL 93.23%: MainGameLoop 0x492980 - logic complete (219 calls, same order); residual is MSVC codegen only: the inlined PDPageInMemory loop comes out rotated (next block->flags && mask computed on the back edge, +4 insns) and the exit-path constants are allocated differently (original keeps 2 in esi / 0 in edi / 5 in ebx; ours uses immediates, 0 in ebx, 5 in esi, 1 in ebp). for(;;)+break, while and goto forms all compile to the identical rotated code.
// FUNCTION: CARMA2_HW 0x00492980
// FUNCTION: CARMA2_SW 0x0048fbf0
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {
    int tried_to_allocate_AR;
    tRace_result result;

    tried_to_allocate_AR = 0;
    gCar_to_view = &gProgram_state.current_car;
    gCar_to_view_id = 0;
    gOld_camera_time = 0;
    gLast_net_auto_increase_credits = 0;
    gLast_credit_headup__mainloop = 0;
    gLast_time_headup = gTimer;
    gRace_bonus_headup = -1;
    gTime_bonus_headup = -1;
    gTime_bonus_start = 0;
    gTime_bonus_state = eTime_bonus_none;
    ARMainLoopStart();
    gLast_tick_count = PDGetTotalTime();
    gActual_last_tick_count = gLast_tick_count;
    gFrame_period = 100;
    gQueued_wasted_messages_count = 0;
    gWasted_flash_state = 0;
    gWasted_last_flash = 0;
    RevertPalette();
    gHost_abandon_game = 0;
    gNo_races_yet = 0;
    gRace_over_reason = eRace_not_over_yet;
    gFLOAT_0068c778 = 0.f;
    gNo_credits_APO_restore = 0;
    gReceived_game_scores = 0;
    if (gNet_mode == eNet_mode_none) {
        gProgram_state.field_0x2c = 1;
    }
    gCredits_post_race = -1;
    ShowSpecialVolumesIfRequ();
    gProgram_state.racing = 1;
    if (gNet_mode == eNet_mode_host) {
        gCurrent_net_game->status.stage = eNet_game_playing;
    }
    NetPlayerStatusChanged(ePlayer_status_racing);
    GetAverageGridPosition(&gCurrent_race);
    ForceRebuildActiveCarList();
    PrintMemoryDump(0, "ABOUT TO ENTER MAINLOOP");
    UpdateFramePeriod(&gCamera_period);
    gPrev_camera_position.v[0] = 1000.f;
    gPrev_camera_position.v[1] = 1000.f;
    gPrev_camera_position.v[2] = 1000.f;
    CalculateCameraStuff(gCamera_period);
    MLPageInProcessMemory();
    gRace_start = PDGetTotalTime();
    Timers_Init();
    if (gWait_for_it != 0) {
        gWait_for_it += GetTotalTime();
    }
    ClearEntireScreen();
    ResetPalette();
    for (;;) {
        int window_inactive;
        tU32 frame_start_time;

        Timers_StartFrame();
        EdgeTriggerModeOff();
        if (!gAction_replay_mode) {
            UpdateFramePeriod(&gCamera_period);
        }
        frame_start_time = GetTotalTime();
        CyclePollKeys();
        InitSmashQueue();
        CheckSystemKeys(1);
        InitialiseExtraRenders();
        NetReceiveAndProcessMessages();
        if (gHost_abandon_game || gProgram_state.prog_status == eProg_idling) {
            break;
        }
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox && gMap_view == 2) {
            ToggleMap();
        }
        ResetGrooveFlags();
        ServiceGameInRace();
        EnterUserMessage();
        ARService();
        ResetLollipopQueue();
        MungePalette();
        window_inactive = PDIsWindowInactive();
        if (window_inactive) {
            if (PDGetTotalTime() > gPrev_task_switched_message + 1000) {
                gPrev_task_switched_message = GetTotalTime();
                NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(139));
            }
        } else {
            if (gWindow_inactive) {
                ClearHeadupSlot(4);
                ClearQueuedHeadups();
            }
        }
        gWindow_inactive = window_inactive;
        if (!gAction_replay_mode) {
            MungeAIWorld(gFrame_period);
            ML_CALL_WITH_FRAME_PERIOD(PollCarControls);
        }
        if (!gAction_replay_mode) {
            CheckActiveCarList();
            MLTimersPush(TIMER_PED);
            ML_CALL_WITH_FRAME_PERIOD(MungePedestrians);
            MLTimersPop(TIMER_PED);
            ControlOurCar(gFrame_period);
            MLTimersPush(TIMER_PHS);
            ApplyPhysicsToCars(gLast_tick_count - gRace_start, gFrame_period);
            MLTimersPop(TIMER_PHS);
            ML_CALL_WITH_FRAME_PERIOD(MungeSomeOtherCarGraphics);
            PipeCarPositions();
            MLTimersPush(TIMER_CRS);
            CrushBendFlapRend();
            MLTimersPop(TIMER_CRS);
            DoNetGameManagement();
            CheckRecoveryOfCars(gFrame_period + (gLast_tick_count - gRace_start));
            NetSendMessageStacks();
        } else {
            DoActionReplay(gFrame_period);
        }
        PollCameraControls(gCamera_period);
        MungeEngineNoise();
        CalculateCameraStuff(gCamera_period);
        MungeNapalm();
        if (!gAction_replay_mode) {
            DoPowerupPeriodics(gFrame_period);
        }
        if (!gAction_replay_mode) {
            CheckCheckpoints();
            MungeEnvironmentalSound();
        }
        ChangingView();
        MungeCarGraphics(gFrame_period);
        FunkThoseTronics();
        GrooveThoseDelics();
        ML_CALL_WITH_FRAME_PERIOD(DoWheelDamage);
        CalculateFrameRate();
        CameraBugFix(&gProgram_state.current_car, gCamera_period);
        if (!gAction_replay_mode) {
            /* MungeHeadups (inlined) */
            char the_text[256];
            int net_credits;
            int new_countdown;
            int previous_gtimer;
            int previous_time_bonus;
            int effective_timer;
            int count_oppos;
            tU32 the_time;

            ClearHeadupSlot(3);
            MungeJoystickHeadups();
            net_credits = gProgram_state.credits;
            if (fabs((float)net_credits - (float)gLast_credit_headup__mainloop) / (double)gFrame_period > 1.2) {
                if (net_credits - gLast_credit_headup__mainloop > 0) {
                    net_credits = (int)((net_credits - gLast_credit_headup__mainloop + 1000.0) * (double)gFrame_period * 1.2 / 1000.0 + (double)gLast_credit_headup__mainloop);
                } else {
                    net_credits = (int)((double)gLast_credit_headup__mainloop - ((double)(gLast_credit_headup__mainloop - net_credits) + 1000.0) * (double)gFrame_period * 1.2 / 1000.0);
                }
            }
            if (gNet_mode != eNet_mode_none
                    && gCountdown == 0
                    && gNet_auto_increase_credits_dt[gCurrent_net_game->type] != 0
                    && net_credits < gRecovery_cost.initial_network[gCurrent_net_game->type]
                    && PDGetTotalTime() - gLast_net_auto_increase_credits >= gNet_auto_increase_credits_dt[gCurrent_net_game->type]) {
                gLast_net_auto_increase_credits = PDGetTotalTime();
                gProgram_state.credits += 50;
                if (net_credits + 50 > (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
                    gProgram_state.credits -= net_credits - (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]) + 50.f;
                    net_credits = (int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]);
                } else {
                    net_credits += 50;
                }
            }
            gLast_credit_headup__mainloop = net_credits;
            if (gCountdown != 0) {
                new_countdown = (int)(7.5 - (double)GetRaceTime() / 1000.0);
                if (new_countdown < 0) {
                    new_countdown = 0;
                }
                if (new_countdown != gCountdown && new_countdown <= 5) {
                    gCountdown = new_countdown;
                    NewImageHeadupSlot(5, 0, 800, new_countdown + 4);
                    DRS3StartSound(gPedestrians_outlet, gCountdown + 8000);
                    if (new_countdown == 0) {
                        MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo();
                    }
                }
            }
            if (fabs((float)(int)gTimer - (float)gLast_time_headup) / (double)gFrame_period > 10.0) {
                if ((int)gTimer - gLast_time_headup > 0) {
                    effective_timer = (int)((double)gFrame_period * 10.0 + gLast_time_headup);
                } else {
                    effective_timer = gTimer;
                }
            } else {
                effective_timer = gTimer;
            }
            gLast_time_headup = effective_timer;
            if (gNet_mode != eNet_mode_none) {
                DoNetworkHeadups(net_credits);
            } else {
                if (net_credits >= 0) {
                    sprintf(the_text, "\xf8%d\xfa %s", net_credits, GetMiscString(net_credits >= 100000 ? 47 : 16));
                } else {
                    sprintf(the_text, "\xf8%d\xfa %s", -net_credits, GetMiscString(17));
                }
                ChangeHeadupText(gCredits_won_headup, the_text);
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                    sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", gCount_killed_peds, gPed_count, GetMiscString(18));
                    ChangeHeadupText(gPed_kill_count_headup, the_text);
                } else if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
                    sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", gCount_killed_peds, gTotal_count_smash_peds, GetMiscString(18));
                    ChangeHeadupText(gPed_kill_count_headup, the_text);
                }
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
                    if (gQueued_wasted_messages_count != 0 && GetTotalTime() > gLast_wasted_message_start + 5000) {
                        LoseOldestWastedMassage();
                    }
                    if (gQueued_wasted_messages_count != 0) {
                        if (Flash(150, &gWasted_last_flash, &gWasted_flash_state)) {
                            sprintf(the_text, "\xf9%s %s", gOpponents[gQueued_wasted_messages[0]].abbrev_name, GetMiscString(46));
                        } else {
                            sprintf(the_text, " ");
                        }
                    } else {
                        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                            count_oppos = GetCarCount(eVehicle_opponent);
                        } else {
                            count_oppos = gCount_opponents;
                        }
                        sprintf(the_text, "%s \xf8%d\xfa/\xf8%d", GetMiscString(19), count_oppos - NumberOfOpponentsLeft(), count_oppos);
                    }
                    ChangeHeadupText(gCar_kill_count_headup, the_text);
                }
                TimerString(effective_timer, the_text, 1, 0, 0);
                ChangeHeadupText(gTimer_headup, the_text);
                if (gHeadup_detail_level == 1 || gHeadup_detail_level == 4) {
                    sprintf(gHeadup_oppo_ped_text, "%i  %i  %i", GetCarCount(eVehicle_opponent) - NumberOfOpponentsLeft(), net_credits, gCount_killed_peds);
                }
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
                    sprintf(the_text, "%s \xf8%d\xfa/\xf8%d \xfa%s \xf8%d\xfa/\xf8%d", GetMiscString(21), gCheckpoint, gCheckpoint_count, GetMiscString(20), gLap, gTotal_laps);
                    ChangeHeadupText(gLaps_headup, the_text);
                }
                if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
                    sprintf(the_text, "%s \xf8%d\xfa/\xf8%d", GetMiscString(276), GetRuntimeVariable(gCurrent_race.race_spec->options.smash.var_smash_number), gCurrent_race.race_spec->options.smash.var_smash_target);
                    ChangeHeadupText(gLaps_headup, the_text);
                }
                the_time = GetTotalTime() - gTime_bonus_start;
                switch (gTime_bonus_state) {
                case eTime_bonus_none:
                    break;
                case eTime_bonus_initial_pause:
                    if (the_time >= 500) {
                        sprintf(the_text, "%s %d", GetMiscString(22), gCompletion_bonus_post_race);
                        DRS3StartSound(gPedestrians_outlet, 8015);
                        ChangeHeadupText(gRace_bonus_headup, the_text);
                        gProgram_state.credits += gCompletion_bonus_post_race;
                        gTime_bonus_state = eTime_bonus_race_bonus;
                        gTime_bonus_start = GetTotalTime();
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_race_bonus:
                    if (the_time >= 2000) {
                        gTime_bonus_state = eTime_bonus_tb_up;
                        gTime_bonus_start = GetTotalTime();
                        gLast_rattle_time = 0;
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_tb_up:
                    if (gTimer != 0) {
                        if (the_time - gLast_rattle_time > 15) {
                            previous_gtimer = gTimer;
                            gTimer -= 1000 * ((the_time - gLast_rattle_time) / 15);
                            if ((int)gTimer < 0) {
                                gTimer = 0;
                            }
                            gTime_bonus += (previous_gtimer - (int)gTimer) / 1000 * gCredits_per_second_time_bonus[gProgram_state.skill_level];
                            gLast_rattle_time += 15 * ((the_time - gLast_rattle_time) / 15);
                        }
                        sprintf(the_text, "%s %d", GetMiscString(23), gTime_bonus);
                        ChangeHeadupText(gTime_bonus_headup, the_text);
                    } else {
                        gTime_bonus_state = eTime_bonus_tb_pause;
                        gTime_bonus_start = GetTotalTime();
                        gLast_rattle_time = 0;
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_tb_pause:
                    if (the_time >= 1000) {
                        gTime_bonus_state = eTime_bonus_tb_down;
                        gTime_bonus_start = GetTotalTime();
                        gLast_rattle_time = 0;
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_tb_down:
                    if (gTime_bonus != 0) {
                        if (the_time - gLast_rattle_time > 15) {
                            previous_time_bonus = gTime_bonus;
                            gTime_bonus -= (the_time - gLast_rattle_time) * gCredits_per_second_time_bonus[gProgram_state.skill_level] / 15;
                            if (gTime_bonus < 0) {
                                gTime_bonus = 0;
                            }
                            gProgram_state.credits += previous_time_bonus - gTime_bonus;
                            gLast_rattle_time += 15 * ((the_time - gLast_rattle_time) / 15);
                        }
                        sprintf(the_text, "%s %d", GetMiscString(23), gTime_bonus);
                        ChangeHeadupText(gTime_bonus_headup, the_text);
                    } else {
                        gTime_bonus_state = eTime_bonus_end_pause;
                        gTime_bonus_start = GetTotalTime();
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_end_pause:
                    if (the_time >= 2000 && gRace_finished > 1) {
                        gRace_finished = 1;
                    }
                    break;
                default:
                    break;
                }
            }
            ProcessOilSpills(gFrame_period);
            MungeSmashEdgeTriggers(GetTotalTime());
        }
        MungeDelayedSideEffects();
        MungeShrapnel(gFrame_period);
        if (!gAction_replay_mode) {
            MungePowerupStuff(gFrame_period);
        }
        ChangeDepthEffect();
        ServiceGameInRace();
        EnterUserMessage();
        SkidsPerFrame();
        FlushSmashQueue(0);
        MungeGlassFragments();
        AnimateSky();
        MungeExplosions();
        LastChanceForPedEffects();
        DoLODCarModels();
        MungeLightingEffects();
        if (gWait_for_it != 0) {
            if (GetTotalTime() >= gWait_for_it) {
                gWait_for_it = 0;
            }
        }
        if (gWait_for_it == 0) {
            MLTimersForce(TIMER_RQQ);
            RenderAFrame(1);
            MLTimersForce(TIMER_OQQ);
        }
        DoComplexCarModels();
        CheckReplayTurnOn();
        if (!gRecover_car
                && gProgram_state.prog_status == eProg_game_ongoing
                && gPalette_fade_time == 0
                && (gNet_mode == eNet_mode_none
                        || !gAction_replay_mode
                        || gProgram_state.current_car.car_master_actor->t.t.translate.t.v[0] < 500.f)) {
            EnsureRenderPalette();
            EnsurePaletteUp();
        }
        if (KeyIsDown(0) && !gEntering_message) {
            WaitForNoKeys();
            if (gAction_replay_mode) {
                ToggleReplay(NULL, NULL);
            } else {
                tU32 start_menu_time;

                if (gMap_view == 2) {
                    ToggleMap();
                }
                start_menu_time = PDGetTotalTime();
                FadePaletteDown();
                ClearEntireScreen();
                GoingToInterfaceFromRace();
                DoOptionsMenu();
                GoingBackToRaceFromInterface();
                AddLostTime(PDGetTotalTime() - start_menu_time);
            }
        }
        if (!gAction_replay_mode) {
            CheckTimer();
        } else {
            PollActionReplayControls(&gFrame_period, &gAverage_frame_period);
        }
        if (!gAction_replay_mode && gKnobbled_frame_period != 0) {
            while (GetTotalTime() - frame_start_time < gKnobbled_frame_period) {
            }
        }
        if (!tried_to_allocate_AR) {
            tried_to_allocate_AR = 1;
            PrintMemoryDump(0, "JUST RENDERED 1ST STUFF");
            InitialiseActionReplay();
            PrintMemoryDump(0, "JUST ALLOCATED ACTION REPLAY BUFFER");
        }
        if (gNet_mode == eNet_mode_client && gAbandon_game) {
            gProgram_state.prog_status = eProg_idling;
            gAbandon_game = 0;
        }
        Timers_EndFrame();
        if (gProgram_state.prog_status != eProg_game_ongoing) {
            break;
        }
        if (MungeRaceFinished()) {
            break;
        }
        if (gAbandon_game || gHost_abandon_game) {
            break;
        }
    }
    if (gCredits_post_race >= 0) {
        gProgram_state.credits = gTotal_bonus_post_race + gCredits_post_race;
    }
    PHILDisable();
    PrintMemoryDump(0, "JUST EXITED MAINLOOP");
    FadePaletteDown();
    ClearEntireScreen();
    SuspendPendingFlic();
    RevertPalette();
    if (gMap_view == 2) {
        ToggleMap();
    }
    EnsureSpecialVolumesHidden();
    FadePaletteDown();
    NetPlayerStatusChanged(ePlayer_status_loading);
    if (gAdditional_actors != NULL && gAdditional_actors->children != NULL) {
        AutoSaveAdditionalStuff();
    }
    if (gProgram_state.prog_status == eProg_game_ongoing) {
        ResetCarScreens();
    }
    if (gAbandon_game && gNet_mode == eNet_mode_host && gRace_over_reason < 8) {
        NetFinishRace(gCurrent_net_game, eRace_over_abandoned);
    }
    if (gAction_replay_mode) {
        ToggleReplay(NULL, NULL);
    }
    result = (gHost_abandon_game || gProgram_state.prog_status != eProg_game_ongoing) ? eRace_game_abandonned
            : (gAbandon_game == 1 ? eRace_aborted
                : ((gRace_over_reason == eRace_over_out_of_time || gRace_over_reason == eRace_over_6 || gRace_over_reason == eRace_over_7) ? eRace_timed_out : eRace_completed));
    gProgram_state.redo_race_index = result < eRace_completed ? gProgram_state.current_race_index : -1;
    if (gNet_mode != eNet_mode_none && result == eRace_game_abandonned && !gAbandon_game && gProgram_state.prog_status != eProg_idling) {
        gProgram_state.prog_status = eProg_game_starting;
    }
    gAbandon_game = 0;
    gStart_race_sent = 0;
    gInitialised_grid = 0;
    gHost_abandon_game = 0;
    DRS3StopAllOutletSoundsExceptCDA();
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {
            StopCarBeingIt(gNet_players[i].car);
        }
    }
    gProgram_state.racing = 0;
    if (gNet_mode == eNet_mode_host) {
        gCurrent_net_game->status.stage = eNet_game_stage0;
    }
    WaitForNoKeys();
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00492950
// FUNCTION: CARMA2_SW 0x0048fbc0
tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    if (PercentageChance(50)) {
        StartMusicTrack(9998);
    } else {
        StartMusicTrack(9997);
    }
    gRace_start = PDGetTotalTime();
    result = MainGameLoop();
    return result;
}


// GLOBAL: CARMA2_HW 0x0068c738
br_vector3 gPrev_camera_position;

// GLOBAL: CARMA2_HW 0x00704e44
float gCamera_to_horiz_angle;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0041f300
void C2_HOOK_FASTCALL ResetCarScreens(void) {
    int i;
    int j;
    int count;
    tCar_spec* car;

    for (i = 0; i <= 3; i++) {
        if (i == 0) {
            count = 1;
        } else {
            count = GetCarCount(i);
        }
        for (j = 0; j < count; j++) {
            if (i == 0) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(i, j);
            }
            car->collision_info->last_special_volume = NULL;
        }
    }
    MungeCarGraphics(gFrame_period);
}

extern int gQueued_headup_count;
extern tQueued_headup gQueued_headups[4];

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004497b0
void C2_NAKED C2_HOOK_FASTCALL ClearQueuedHeadups(void) {
    __asm {
        mov eax, dword ptr [gQueued_headup_count]
        push ebx
        mov ecx, eax
        push esi
        shl ecx, 5
        add ecx, eax
        push edi
        lea edx, [eax + ecx*2]
        shl edx, 2
        test eax, eax
        je clear_queued_headups_done

    clear_queued_headups_loop:
        sub edx, 0x10c
        mov esi, offset gQueued_headups + 0x10c
        mov ecx, edx
        mov edi, offset gQueued_headups
        mov ebx, ecx
        dec eax
        shr ecx, 2
        rep movsd
        mov ecx, ebx
        and ecx, 3
        test eax, eax
        rep movsb
        jne clear_queued_headups_loop

    clear_queued_headups_done:
        pop edi
        pop esi
        mov dword ptr [gQueued_headup_count], eax
        pop ebx
        ret
    }
}
#else
void C2_HOOK_FASTCALL ClearQueuedHeadups(void) {
    int size = gQueued_headup_count * sizeof(tQueued_headup);

    C2_HOOK_BUG_ON(sizeof(tQueued_headup) != 0x10c);

    while (gQueued_headup_count != 0) {
        size -= sizeof(tQueued_headup);
        gQueued_headup_count -= 1;
        memcpy(&gQueued_headups[0], &gQueued_headups[1], size);
    }
}
#endif

// FUNCTION: CARMA2_HW 0x0045b3b0
void C2_HOOK_FASTCALL MungeJoystickHeadups(void) {
    extern tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;
    extern int gINT_00596308;
    extern int gINT_0068b8e4;
    extern int gINT_0068b8e8;
    extern int gRace_head_ups[22];
    extern void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pBuffer);
    extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
    tButtonJoystickInfo* joystick_info;
    tHeadup_text* headup_text;
    tHeadup_text* text;
    tHeadup_text_buffer* pBuffer;
    tU32 the_time;
    int last_index;
    int i;
    int n;
    size_t j;
    char s[80];

    last_index = -1;
    joystick_info = NULL;
    pBuffer = &gJoystick_headup_buffer_0079d8a0;
    if (gINT_0068b8e8) {
        the_time = PDGetTotalTime();
        if (PDKeyDown(65)) {
            if (gINT_00596308 != -1) {
                gINT_00596308 = -1;
            } else {
                joystick_info = PDGetCurrentJoystickData();
                if (joystick_info == NULL) {
                    goto munge;
                }
                last_index = -1;
                i = 0;
                for (text = pBuffer->texts; text != NULL; text = text->next, i++) {
                    if (text == pBuffer->text_last_add) {
                        last_index = i;
                    }
                }
                for (j = 0; j < joystick_info->count_buttons; j++) {
                    if (joystick_info->buttons[j] == last_index && joystick_info->buttons[j] != -1) {
                        joystick_info->buttons[j] = -1;
                    }
                }
            }
            while (PDKeyDown(65)) {
            }
        }
        if (gINT_00596308 != -1) {
            goto munge;
        }
        if (PDKeyDown(64)) {
            if (gINT_00596308 == -1) {
                last_index = -1;
                i = 0;
                for (text = pBuffer->texts; text != NULL; text = text->next, i++) {
                    if (text == pBuffer->text_last_add) {
                        last_index = i;
                    }
                }
                gINT_00596308 = last_index;
            }
            while (PDKeyDown(64)) {
            }
        }
        if (PDKeyDown(66)) {
            if (pBuffer != NULL && pBuffer->text_last_add != NULL && pBuffer->text_last_add != pBuffer->texts) {
                pBuffer->pos_last_add -= 1;
                pBuffer->text_last_add = pBuffer->texts;
                for (i = 0; i < pBuffer->pos_last_add; i++) {
                    pBuffer->text_last_add = pBuffer->text_last_add->next;
                }
            }
            do {
            } while (PDGetTotalTime() - the_time < 100);
        }
        if (PDKeyDown(67)) {
            if (pBuffer != NULL && pBuffer->text_last_add != NULL && pBuffer->text_last_add->next != NULL) {
                pBuffer->text_last_add = pBuffer->text_last_add->next;
                pBuffer->pos_last_add += 1;
            }
            do {
            } while (PDGetTotalTime() - the_time < 100);
        }
        if (PDKeyDown(68)) {
            for (i = pBuffer->field_0x10 - 1; i > 0; i--) {
                if (pBuffer != NULL && pBuffer->text_last_add != NULL && pBuffer->text_last_add != pBuffer->texts) {
                    pBuffer->pos_last_add -= 1;
                    pBuffer->text_last_add = pBuffer->texts;
                    for (n = 0; n < pBuffer->pos_last_add; n++) {
                        pBuffer->text_last_add = pBuffer->text_last_add->next;
                    }
                }
            }
            while (PDKeyDown(68)) {
            }
        }
        if (PDKeyDown(69)) {
            for (i = pBuffer->field_0x10 - 1; i > 0; i--) {
                if (pBuffer != NULL && pBuffer->text_last_add != NULL && pBuffer->text_last_add->next != NULL) {
                    pBuffer->text_last_add = pBuffer->text_last_add->next;
                    pBuffer->pos_last_add += 1;
                }
            }
            while (PDKeyDown(69)) {
            }
        }
munge:
        AdjustHeadupTextBuffer(pBuffer);
        headup_text = pBuffer->field_0x4;
        joystick_info = PDGetCurrentJoystickData();
        if (joystick_info == NULL) {
            return;
        }
    } else if (gINT_0068b8e4) {
        return;
    }

    for (i = 0; i < 22; i++) {
        if (gINT_0068b8e8 && headup_text != NULL) {
            if (i == gINT_00596308) {
                ChangeHeadupFont(gRace_head_ups[i], -3);
            } else if (headup_text == pBuffer->text_last_add) {
                ChangeHeadupFont(gRace_head_ups[i], -1);
            } else {
                ChangeHeadupFont(gRace_head_ups[i], -2);
            }
            if (i == gINT_00596308) {
                sprintf(s, "%s      ???", headup_text->text);
            } else if (joystick_info == NULL) {
                sprintf(s, "%s", headup_text->text);
            } else {
                sprintf(s, "%s", headup_text->text);
                for (j = 0; j < joystick_info->count_buttons; j++) {
                    if (joystick_info->buttons[j] == i) {
                        sprintf(s, "%s      #%02d", headup_text->text, (int)j);
                    }
                }
            }
            ChangeHeadupText(gRace_head_ups[i], s);
            headup_text = headup_text->next;
        } else {
            ChangeHeadupText(gRace_head_ups[i], "");
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d5970
void C2_HOOK_FASTCALL LastChanceForPedEffects(void) {
    extern tPedestrian* gPedestrian_array;
    extern br_material* gPed_smear_material;
    extern int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step);
    tPedestrian* ped;
    int i;

    /* PossiblePedSmear inlined by the original */
    ped = gPedestrian_array;
    for (i = 0; i < gPed_count; i++, ped++) {
        tPed_cache_006944c0* smear_info;
        br_vector3 pos;

        if (ped->flags & 0x1) {
            smear_info = ped->field_0x0c;
            if (smear_info == NULL) {
                continue;
            }
            if ((smear_info->field_0xc4 & 0x2) && smear_info->field_0xca >= 2) {
                pos.v[0] = smear_info->field_0xe4.v[0] * (1.0 / smear_info->field_0xca);
                pos.v[1] = smear_info->field_0xe4.v[1] * (1.0 / smear_info->field_0xca);
                pos.v[2] = smear_info->field_0xe4.v[2] * (1.0 / smear_info->field_0xca);
                if (!(smear_info->field_0xc4 & 0x1)) {
                    smear_info->field_0xc4 |= 0x1;
                    smear_info->field_0xc8 = -1;
                } else if (SkidSection(
                        &smear_info->field_0xc8,
                        &smear_info->field_0xcc,
                        &smear_info->field_0xd8,
                        gPed_smear_material,
                        &pos,
                        &smear_info->field_0xf0,
                        &smear_info->field_0xfc,
                        &smear_info->field_0x108,
                        0.0f,
                        smear_info->field_0x124 - smear_info->field_0x128 + 0.01f)) {
                    smear_info->field_0xc4 &= ~0x1;
                }
                smear_info->field_0xfc = pos;
                smear_info->field_0x108 = smear_info->field_0xf0;
            } else {
                smear_info->field_0xc4 &= ~0x1;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00403cc0
void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, void (C2_HOOK_FASTCALL * pBetween_sessions)(int), tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    typedef void (C2_HOOK_FASTCALL * tReplay_between_sessions_callback)(int pFrames);
    extern float gReplay_rate;
    extern void C2_HOOK_FASTCALL ARDoActionReplay(int pFrames, tReplay_between_sessions_callback pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

    if (gReplay_rate != 0.0f) {
        ARDoActionReplay((int)gReplay_rate, (tReplay_between_sessions_callback)pBetween_sessions, pBefore_frame, pAfter_frame);
    }
}

// FUNCTION: CARMA2_HW 0x004e68e0
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {
    extern tU32 gTime_of_last_zappy_headups;
    extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);
    extern tPreprocess_AR_callback PreProcess;
    extern tPostProcess_AR_callback PostProcess;
    extern void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, void (C2_HOOK_FASTCALL * pBetween_sessions)(int), tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

    gTime_of_last_zappy_headups = 0;
    FUN_00403cc0(pFrame_period, DoZappyActionReplayHeadups, PreProcess, PostProcess);
}

// GLOBAL: CARMA2_HW 0x0065fbf8
int gReplay_button_keys[10] = { 0x58, 0x55, 0x3b, 0x6a, 0x3a, 0x57, 0x5a, 0x4c, 0x4b, 0 };

// GLOBAL: CARMA2_HW 0x0065fc20
int gReplay_button_rects[2][9][4] = {
    { { 0x10c, 0x9c, 0x116, 0xae }, { 0x117, 0x96, 0x121, 0xa0 }, { 0x117, 0xaa, 0x121, 0xb4 }, { 0x119, 0xa0, 0x12a, 0xaa }, { 0x122, 0xaa, 0x12c, 0xb4 }, { 0x122, 0x96, 0x12c, 0xa0 }, { 0x12d, 0x9c, 0x137, 0xae }, { 0xef, 0xa8, 0xfb, 0xb4 }, { 0xfe, 0xa8, 0x10a, 0xb4 } },
    { { 0x207, 0x18b, 0x21f, 0x1a3 }, { 0x21e, 0x174, 0x236, 0x18c }, { 0x221, 0x1a2, 0x239, 0x1ba }, { 0x22b, 0x18b, 0x243, 0x1a3 }, { 0x236, 0x1a2, 0x24e, 0x1ba }, { 0x239, 0x174, 0x251, 0x18c }, { 0x253, 0x18b, 0x26b, 0x1a3 }, { 0x1bf, 0x19f, 0x1d7, 0x1b7 }, { 0x1e1, 0x1a1, 0x1f9, 0x1b9 } },
};

// GLOBAL: CARMA2_HW 0x006a2368
int gReplay_show_controls;

// GLOBAL: CARMA2_HW 0x006a23cc
int gUnk_006a23cc;

// GLOBAL: CARMA2_HW 0x006a23d0
tU32 gUnk_006a23d0;

// FUNCTION: CARMA2_HW 0x00413860
void C2_HOOK_FASTCALL PickActionReplayManualCameraTarget(int pCursor_x, int pCursor_y, int pArg3) {
    extern tActionReplayCameraMode gAction_replay_camera_mode;
    extern br_vector3 gAction_replay_manual_camera_target_position;
    extern int gAction_replay_manual_camera_target_type;
    extern tCar_spec* gAction_replay_manual_camera_target_car;
    extern tPedestrian* gAction_replay_manual_camera_target_pedestrian;
    extern br_actor* gAction_replay_manual_camera_target_character_actor;
    extern tPedestrian* gPedestrian_array;
    extern int gPed_count;
    extern double gDouble_005895a8;
    extern double gDouble_00589508;
    extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
    extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
    extern void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix);
    extern void C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C);
    extern int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT);
    extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter);
    extern void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* pNormal, br_scalar* pT, br_material** pMaterial, br_actor** pActor);
    br_actor* camera_actor;
    br_camera* camera_data;
    br_vector3 ray_direction;
    br_vector3 face_normal;
    br_material* face_material;
    br_actor* face_actor;
    float t;
    float horizontal_offset;
    float vertical_offset;
    br_vector3* camera_position;
    br_vector3* pedestrian_camera_position;
    tCar_spec* nearest_car;
    tPedestrian* nearest_pedestrian;
    tPedestrian* pedestrian;
    int category;
    int car_count;
    int car_index;
    int pedestrian_index;

    camera_actor = gCamera;
    camera_data = (br_camera*)camera_actor->type_data;
    if (gAction_replay_camera_mode != kActionReplayCameraMode_Manual || pArg3 != 0) {
        return;
    }
    ray_direction.v[0] = camera_actor->t.t.mat.m[2][0] * -(camera_data->yon_z);
    ray_direction.v[1] = camera_actor->t.t.mat.m[2][1] * -(camera_data->yon_z);
    ray_direction.v[2] = camera_actor->t.t.mat.m[2][2] * -(camera_data->yon_z);
    vertical_offset = (float)((240 - pCursor_y) * gDouble_005895a8
        * tan((camera_data->field_of_view >> 1) * gDouble_00589508) * camera_data->yon_z);
    ray_direction.v[0] = camera_actor->t.t.mat.m[1][0] * vertical_offset + ray_direction.v[0];
    ray_direction.v[1] = camera_actor->t.t.mat.m[1][1] * vertical_offset + ray_direction.v[1];
    ray_direction.v[2] = camera_actor->t.t.mat.m[1][2] * vertical_offset + ray_direction.v[2];
    t = (float)(tan((camera_data->field_of_view >> 1) * gDouble_00589508) * camera_data->yon_z);
    horizontal_offset = (float)((pCursor_x - 320) * gDouble_005895a8 * t);
    ray_direction.v[0] = camera_actor->t.t.mat.m[0][0] * horizontal_offset + ray_direction.v[0];
    ray_direction.v[1] = camera_actor->t.t.mat.m[0][1] * horizontal_offset + ray_direction.v[1];
    ray_direction.v[2] = camera_actor->t.t.mat.m[0][2] * horizontal_offset + ray_direction.v[2];

    ActorFindFace(&camera_actor->t.t.translate.t, &ray_direction, gTrack_actor, &face_normal, &t, &face_material, &face_actor);
    if (t < 1.0f) {
        ray_direction.v[0] *= t;
        ray_direction.v[1] *= t;
        ray_direction.v[2] *= t;
    }
    camera_position = &gCamera->t.t.translate.t;
    nearest_car = NULL;
    gAction_replay_manual_camera_target_position.v[0] = gCamera->t.t.translate.t.v[0] + ray_direction.v[0];
    gAction_replay_manual_camera_target_position.v[1] = gCamera->t.t.translate.t.v[1] + ray_direction.v[1];
    gAction_replay_manual_camera_target_position.v[2] = gCamera->t.t.translate.t.v[2] + ray_direction.v[2];
    t = 1.0f;
    gAction_replay_manual_camera_target_type = 1;
    category = 0;
    do {
        br_vector3 local_origin;
        br_vector3 local_direction;
        br_vector3 local_end;
        br_vector3 hit_point;
        tCar_spec* car;

        car_count = (category == eVehicle_self) ? 1 : GetCarCount(category);
        car_index = 0;
        if (car_count > 0) {
            do {
                car = (category == eVehicle_self) ? &gProgram_state.current_car : GetCarSpec(category, car_index);
                DrMatrix34ApplyLPInverse(&local_origin, camera_position, &car->car_master_actor->t.t.mat);
                BrMatrix34TApplyV(&local_direction, &ray_direction, &car->car_master_actor->t.t.mat);
                local_end.v[0] = local_origin.v[0] + local_direction.v[0];
                local_end.v[1] = local_origin.v[1] + local_direction.v[1];
                local_end.v[2] = local_origin.v[2] + local_direction.v[2];
                LineBoxColl(&local_origin, &local_end, &car->collision_info->bb2, &hit_point, &t);
                if (t < 1.0f) {
                    ray_direction.v[0] *= t;
                    ray_direction.v[1] *= t;
                    ray_direction.v[2] *= t;
                    t = 1.0f;
                    nearest_car = car;
                }
                car_index++;
            } while (car_index < car_count);
        }
        category++;
    } while (category < 5);
    gAction_replay_manual_camera_target_car = nearest_car;
    if (nearest_car != NULL) {
        gAction_replay_manual_camera_target_type = 2;
    }

    pedestrian_camera_position = &gCamera->t.t.translate.t;
    nearest_pedestrian = NULL;
    pedestrian = gPedestrian_array;
    pedestrian_index = 0;
    if (gPed_count > 0) {
        do {
            if ((pedestrian->flags & 1) != 0) {
                br_vector3 local_origin;
                br_vector3 local_direction;
                br_vector3 local_end;
                br_vector3 hit_point;
                tPed_character_instance* character;
                br_actor* character_actor;

                character = pedestrian->character;
                character_actor = GetCharacterActorPtr(character);
                DrMatrix34ApplyLPInverse(&local_origin, pedestrian_camera_position, &character_actor->t.t.mat);
                BrMatrix34TApplyV(&local_direction, &ray_direction, &character_actor->t.t.mat);
                local_end.v[0] = local_origin.v[0] + local_direction.v[0];
                local_end.v[1] = local_origin.v[1] + local_direction.v[1];
                local_end.v[2] = local_origin.v[2] + local_direction.v[2];
                LineBoxColl(&local_origin, &local_end, &character->personality->bb, &hit_point, &t);
                if (t < 1.0f) {
                    ray_direction.v[0] *= t;
                    ray_direction.v[1] *= t;
                    ray_direction.v[2] *= t;
                    t = 1.0f;
                    nearest_pedestrian = pedestrian;
                    face_actor = character_actor;
                }
            }
            pedestrian_index++;
            pedestrian++;
        } while (pedestrian_index < gPed_count);
    }
    gAction_replay_manual_camera_target_pedestrian = nearest_pedestrian;
    if (nearest_pedestrian != NULL) {
        gAction_replay_manual_camera_target_type = 3;
        gAction_replay_manual_camera_target_character_actor = face_actor;
    }
}

// FUNCTION: CARMA2_HW 0x00403c70
void C2_HOOK_FASTCALL FUN_00403c70(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost) {
    typedef void (C2_HOOK_FASTCALL * tReplay_between_sessions_callback)(int pFrames);
    extern float gReplay_rate;
    extern int gDisable_piped_sounds;
    extern void C2_HOOK_FASTCALL ARDoActionReplay(int pFrames, tReplay_between_sessions_callback pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);
    float saved_replay_rate;

    gDisable_piped_sounds = 1;
    saved_replay_rate = gReplay_rate;
    gReplay_rate = -100.0f;
    ARDoActionReplay(-2147483647, (tReplay_between_sessions_callback)pZappy, pPre, pPost);
    gDisable_piped_sounds = 0;
    gReplay_rate = saved_replay_rate;
}

// FUNCTION: CARMA2_HW 0x00403210
void C2_HOOK_FASTCALL FUN_00403210(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost) {
    typedef void (C2_HOOK_FASTCALL * tReplay_between_sessions_callback)(int pFrames);
    extern float gReplay_rate;
    extern int gDisable_piped_sounds;
    extern void C2_HOOK_FASTCALL ARDoActionReplay(int pFrames, tReplay_between_sessions_callback pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);
    float saved_replay_rate;

    gDisable_piped_sounds = 1;
    saved_replay_rate = gReplay_rate;
    gReplay_rate = 100.0f;
    ARDoActionReplay(0x7fffffff, (tReplay_between_sessions_callback)pZappy, pPre, pPost);
    gDisable_piped_sounds = 0;
    gReplay_rate = saved_replay_rate;
}


// PARTIAL 98.17%: PollActionReplayControls - regalloc: hit-test loop i/mouse_x in ecx/esi swapped
// FUNCTION: CARMA2_HW 0x004e69b0
void C2_HOOK_FASTCALL PollActionReplayControls(tU32* pFrame_period, tU32* pAverage_frame_period) {
    extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
    extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
    extern void C2_HOOK_FASTCALL AfterActionReplay(void);
    extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);
    extern void C2_HOOK_FASTCALL PreProcess(int pFrame_period);
    extern void C2_HOOK_FASTCALL PostProcess(int pFrame_period);
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
    extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
    extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);
    extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
    extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);
    extern int C2_HOOK_FASTCALL ARReplayPauseState(void);
    extern void C2_HOOK_FASTCALL ARSetPause(int pPause);
    extern int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);
    extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
    extern void C2_HOOK_FASTCALL GetMousePosition(int* pX, int* pY);
    extern int C2_HOOK_FASTCALL MovieRecording(void);
    extern void C2_HOOK_FASTCALL MovieRecordButtonPressed(void);
    extern void C2_HOOK_FASTCALL ChangeCameraType(void);
    extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
    extern int gMouse_in_use;
    extern int gReal_graf_data_index;
    extern int gSingle_frame_mode;
    extern tU32 gTime_of_last_zappy_headups;
    extern int gRecording_mouse_disabled;
    extern int gNoTransients;
    extern tU32 gUnk_006a23c8;
    extern int gUnk_006a2378;
    extern int gKey_down;
    int i;
    int* rect;
    int mouse_y;
    int mouse_x;
    int prev_key;
    tU32 time;
    float old_rate;

    EdgeTriggerModeOff();
    time = PDGetTotalTime();
    old_rate = ARGetReplayRate();
    prev_key = gKey_down;
    gKey_down = PDAnyKeyDown();
    if ((prev_key == -1 || prev_key == 4) && KeyIsDown(0x3a)) {
        DRS3StopAllOutletSoundsExceptCDA();
        if (gAction_replay_mode) {
            RenderAFrame(1);
            RenderAFrame(1);
        }
        if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
                (tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess, (int*)pFrame_period, (int*)pAverage_frame_period)) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(0x25));
        }
        return;
    }
    if (gKey_down != -1) {
        gMouse_in_use = 0;
    } else {
        if ((prev_key == -1 || prev_key == 4 || prev_key == 0x55 || prev_key == 0x57 || prev_key == 0x4c) && EitherMouseButtonDown()) {
            GetMousePosition(&mouse_x, &mouse_y);
            i = 0;
            rect = &gReplay_button_rects[gReal_graf_data_index][0][1];
            for (; i < 9; i++, rect += 4) {
                if (mouse_x >= rect[-1] && mouse_y >= rect[0] && mouse_x < rect[1] && mouse_y < rect[2]) {
                    gKey_down = gReplay_button_keys[i];
                    break;
                }
            }
            if (i == 9 && !gUnk_006a23cc) {
                PickActionReplayManualCameraTarget(mouse_x, mouse_y, 0);
            }
            gUnk_006a23cc = 1;
        } else {
            gUnk_006a23cc = 0;
        }
    }
    if (gKey_down == 0x4b && (prev_key == -1 || prev_key == 4)) {
        MovieRecordButtonPressed();
    } else if (gKey_down == 0x4c) {
        gReplay_show_controls = 1;
        if (prev_key != 0x4c) {
            ChangeCameraType();
            gUnk_006a23c8 = PDGetTotalTime();
        }
    } else {
        gReplay_show_controls = 0;
    }
    if ((gKey_down == 0x56 || gKey_down == 0x6a) && (prev_key == -1 || prev_key == 4)) {
        ARSetPause(!ARReplayPauseState());
    } else if ((gKey_down == 0x51 || gKey_down == 0x32) && (prev_key == -1 || prev_key == 4)) {
        ARSetReplayDirection((float)-ARGetReplayDirection());
        if (ARReplayPauseState()) {
            ARSetPause(0);
        }
    }
    if (gKey_down == 0x52 && (prev_key == -1 || prev_key == 4)) {
        ARSetReplayRate(-1.0f);
        ARSetReplayDirection(-1.0f);
        gSingle_frame_mode = 1;
    } else if (gKey_down == 0x54 && (prev_key == -1 || prev_key == 4)) {
        ARSetReplayRate(1.0f);
        ARSetReplayDirection(1.0f);
        gSingle_frame_mode = 1;
    } else if (gKey_down == 0x55 || gKey_down == 0x44) {
        if (ARGetReplayRate() > -1.2) {
            ARSetReplayRate(-1.2f);
        }
        if (gUnk_006a23d0) {
            ARSetReplayRate((float)(ARGetReplayRate() - (double)(tU32)(time - gUnk_006a23d0) * 0.002));
        }
        if (ARGetReplayRate() < -8.0f) {
            ARSetReplayRate(-8.0f);
        }
    } else if (gKey_down == 0x57 || gKey_down == 0x45) {
        if (ARGetReplayRate() < 1.2) {
            ARSetReplayRate(1.2f);
        }
        if (gUnk_006a23d0) {
            ARSetReplayRate((float)(ARGetReplayRate() - (double)(tU32)(time - gUnk_006a23d0) * -0.002));
        }
        if (ARGetReplayRate() > 8.0f) {
            ARSetReplayRate(8.0f);
        }
    } else if (gKey_down == 0x3b) {
        ARSetReplayRate(-1.0f);
        ARSetReplayDirection(-1.0f);
        ARSetPause(0);
    } else if (gKey_down == 0x3a) {
        ARSetReplayRate(1.0f);
        ARSetReplayDirection(1.0f);
        ARSetPause(0);
    } else {
        if (ARReplayPauseState()) {
            ARSetReplayRate(0.0f);
        } else {
            ARSetReplayRate((float)ARGetReplayDirection());
        }
    }
    if ((gKey_down == 0x58 || gKey_down == 0x42) && (prev_key == -1 || prev_key == 4)) {
        gTime_of_last_zappy_headups = 0;
        if (gNoTransients) {
            gRecording_mouse_disabled = 1;
        }
        RenderAFrame(1);
        RenderAFrame(1);
        FUN_00403c70((tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess);
        if (gNoTransients) {
            gRecording_mouse_disabled = 0;
        }
        ARSetReplayRate(1.0f);
        MungeCarGraphics(gFrame_period);
        GrooveThoseDelics();
        ARSetReplayRate(0.0f);
        ARSetReplayDirection(1.0f);
        ARSetPause(1);
    } else if ((gKey_down == 0x5a || gKey_down == 0x43) && (prev_key == -1 || prev_key == 4)) {
        gTime_of_last_zappy_headups = 0;
        if (gNoTransients) {
            gRecording_mouse_disabled = 1;
        }
        RenderAFrame(1);
        RenderAFrame(1);
        FUN_00403210((tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess);
        if (gNoTransients && !MovieRecording()) {
            gRecording_mouse_disabled = 0;
        }
        ARSetReplayRate(-1.0f);
        MungeCarGraphics(gFrame_period);
        GrooveThoseDelics();
        ARSetReplayRate(0.0f);
        ARSetReplayDirection(-1.0f);
        ARSetPause(1);
    }
    if (*(float*)&gUnk_006a2378 != 0.0f) {
        ARSetReplayRate(*(float*)&gUnk_006a2378);
        *(float*)&gUnk_006a2378 = 0.0f;
    }
    if (ARGetReplayRate() * old_rate < 0.0f) {
        *(float*)&gUnk_006a2378 = ARGetReplayRate();
        ARSetReplayRate(0.0f);
    } else {
        *(float*)&gUnk_006a2378 = 0.0f;
    }
    if (old_rate != 0.0f) {
        gFrame_period = (tU32)(ARGetReplayRate() / old_rate * gFrame_period);
    }
    if (fabs(ARGetReplayRate()) < 1.2) {
        gUnk_006a23d0 = 0;
    } else {
        gUnk_006a23d0 = time;
    }
    if (old_rate <= 0.0f && ARGetReplayRate() > 0.0f) {
        ARGoForwards();
        return;
    }
    if (old_rate >= 0.0f && ARGetReplayRate() < 0.0f) {
        ARGoBackwards();
    }
}

extern tU32 gPHIL_last_physics_tick;
extern tU32 gLast_munge_smash_edge_triggers;
extern tSmashable_item_spec* gTrack_smashable_environment_specs;
extern int gCount_track_smashable_environment_specs;

// FUNCTION: CARMA2_HW 0x004f64d0
void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime) {
    undefined4* p;
    int i;

    if (gLast_munge_smash_edge_triggers != gPHIL_last_physics_tick) {
        i = 0;
        if (i < gCount_track_smashable_environment_specs) {
            p = &gTrack_smashable_environment_specs->field_0x10;
            do {
                if (*p != 0 && *p != pTime) {
                    *p = 0;
                }
                i++;
                p = (undefined4*)((char*)p + sizeof(tSmashable_item_spec));
            } while (i < gCount_track_smashable_environment_specs);
        }
    }
    gLast_munge_smash_edge_triggers = gPHIL_last_physics_tick;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* MainGameLoop / MungeHeadups / CalculateFrameRate / CheckTimer state (address order) */
// GLOBAL: CARMA2_HW 0x0068c730
tU32 gWasted_last_flash;

// GLOBAL: CARMA2_HW 0x0068c734
tU32 gFrame_rate_last_time;

// GLOBAL: CARMA2_HW 0x0068c764
int gLast_credit_headup__mainloop;

// GLOBAL: CARMA2_HW 0x0068c768
int gWasted_flash_state;

// GLOBAL: CARMA2_HW 0x0068c76c
int gLast_time_headup;

// GLOBAL: CARMA2_HW 0x0068c770
tU32 gLast_net_auto_increase_credits;

// GLOBAL: CARMA2_HW 0x0068c774
int gTotal_bonus_post_race;

// GLOBAL: CARMA2_HW 0x0068c778
float gFLOAT_0068c778;

// GLOBAL: CARMA2_HW 0x0068c780
int gTime_bonus;

// GLOBAL: CARMA2_HW 0x0068c784
tTime_bonus_state gTime_bonus_state;

// GLOBAL: CARMA2_HW 0x0068c788
tU32 gTime_bonus_start;

/* initialized and adjacent: CalculateFrameRate compares against the end of this array (= gTime_bonus_headup) */
// GLOBAL: CARMA2_HW 0x0068c790
int gFrame_rate_last_rates[30] = { 0 };

// GLOBAL: CARMA2_HW 0x0068c808
int gTime_bonus_headup = 0;

// GLOBAL: CARMA2_HW 0x0068c80c
int gCredits_post_race;

// GLOBAL: CARMA2_HW 0x0068c810
int gRace_bonus_headup;

// GLOBAL: CARMA2_HW 0x0068c818
tU32 gLast_rattle_time;

// GLOBAL: CARMA2_HW 0x0068c828
int gLast_time_in_seconds;

// GLOBAL: CARMA2_HW 0x0068c834
tU32 gPrev_task_switched_message;

// GLOBAL: CARMA2_HW 0x0068c838
int gWindow_inactive;

// GLOBAL: CARMA2_HW 0x006a82a4
tU32 gLast_munge_smash_edge_triggers;

// GLOBAL: CARMA2_HW 0x00596308
int gINT_00596308 = -1;

// GLOBAL: CARMA2_HW 0x0079d8a0
tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;

// The original calls this every frame from MungeJoystickHeadups; the body only refreshes
// the headup text buffer, so an inert stub keeps the race running until it is implemented.
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045b0a0
void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pBuffer) {
    tHeadup_text* node;
    tHeadup_text* current;
    int index;
    int i;

    node = pBuffer->texts;
    if (node == NULL) {
        return;
    }
    if (pBuffer->field_0x4 == NULL) {
        pBuffer->field_0x4 = pBuffer->texts;
        if (pBuffer->count - pBuffer->pos_last_add < pBuffer->field_0x10) {
            i = pBuffer->count - pBuffer->field_0x10;
            node = pBuffer->texts;
            for (; i > 0; i--) {
                node = node->next;
            }
            pBuffer->field_0x4 = node;
            return;
        }
    }
    current = pBuffer->field_0x4;
    index = 0;
    for (node = pBuffer->texts; node != current; node = node->next) {
        index++;
    }
    if (pBuffer->pos_last_add >= index + pBuffer->field_0x10
            || pBuffer->pos_last_add <= index) {
        if (pBuffer->pos_last_add == index + pBuffer->field_0x10) {
            if (pBuffer->count - index < pBuffer->field_0x10) {
                return;
            }
            pBuffer->field_0x4 = current->next;
            return;
        }
        if (pBuffer->count - pBuffer->pos_last_add < pBuffer->field_0x10) {
            node = pBuffer->texts;
            for (i = pBuffer->count - pBuffer->field_0x10; i > 0; i--) {
                node = node->next;
            }
            pBuffer->field_0x4 = node;
            return;
        }
        pBuffer->field_0x4 = pBuffer->text_last_add;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00589440
float gFLOAT_00589440 = 1.0f;

// GLOBAL: CARMA2_HW 0x005895a8
double gDouble_005895a8 = 0.0041666666666666666;

// GLOBAL: CARMA2_HW 0x00589508
double gDouble_00589508 = 9.587379924285257e-05;
