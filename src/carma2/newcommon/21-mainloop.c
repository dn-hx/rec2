#include "21-mainloop.h"

#include "41-utility.h"
#include "rec2_macros.h"

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

// UpdateFramePeriod

// CheckTimer

// MungeRaceFinished

// CalculateCameraStuff

// CheckActiveCarList

// MainGameLoop

// DoRace

