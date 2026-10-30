#include "21-mainloop.h"

#include "41-utility.h"
#include "69-sound.h"
#include "rec2_macros.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include <stdlib.h>

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
        tU32 last_tick_count;
        tU32 camera_period;

        last_tick_count = gLast_tick_count;
        gFrame_period = abs((int)(gLast_replay_frame_time - last_tick_count));
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

/* based on carpocalypse2 src/carma2/common/mainloop.c:151 */
// FUNCTION: CARMA2_HW 0x004940e0
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {
    extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);
    extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
    extern void C2_HOOK_FASTCALL ResetPedNearness(void);
    extern br_vector3 gPrev_camera_position;
    extern float gCamera_to_horiz_angle;
    extern float gYon_squared;
    br_camera* camera;

    gOur_pos = &gPlayer_car_master_actor->t.t.translate.t;
    PositionExternalCamera(&gProgram_state.current_car, pCamera_period);
    BrActorToActorMatrix34(&gCamera_to_world, gCamera, gUniverse_actor);
    BrActorToActorMatrix34(&gRearview_camera_to_world, gRearview_camera, gUniverse_actor);
    gCamera_to_horiz_angle = FastScalarArcTan2(gCamera_to_world.m[2][1], gCamera_to_world.m[1][1]);
    camera = gCamera->type_data;
    gYon_squared = gYon_multiplier * gYon_multiplier * camera->yon_z * camera->yon_z;
    if (REC2_SQR(gCamera_to_world.m[3][0] - gPrev_camera_position.v[0]) + REC2_SQR(gCamera_to_world.m[3][1] - gPrev_camera_position.v[1]) + REC2_SQR(gCamera_to_world.m[3][2] - gPrev_camera_position.v[2]) > 400.0) {
        ResetPedNearness();
    }
    gPrev_camera_position = *(br_vector3*)gCamera_to_world.m[3];
}

// CheckActiveCarList

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00492980
// STUB: CARMA2_SW 0x0048fbf0
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {

    NOT_IMPLEMENTED();
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
