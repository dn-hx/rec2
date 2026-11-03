/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "27-powerup.h"

#include "00-car.h"
#include "01-network.h"

#include "16-graphics1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "32-spark.h"
#include "41-utility.h"
#include "42-input.h"

#include "49-physics.h"
#include "51-smash.h"
#include "53-controls.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"

typedef struct {
    tNet_message_chunk_header header;
    tPlayer_ID player_id;
    int powerup_index;
    int time_remaining;
    int field_0x10;
} tNet_message_chunk_powerup;

// GLOBAL: CARMA2_HW 0x006a0a70
// GLOBAL: CARMA2_SW 0x0069aac8
int gInventory[20];

// GLOBAL: CARMA2_HW 0x006a0a6c
// GLOBAL: CARMA2_SW 0x0069aac4
int gINT_006a0a6c;

// GLOBAL: CARMA2_HW 0x006a0a5c
// GLOBAL: CARMA2_SW 0x0069aab4
int gINT_006a0a5c;

// GLOBAL: CARMA2_HW 0x006a0a58
// GLOBAL: CARMA2_SW 0x0069aab0
int gInventory_selected = 0;

// GLOBAL: CARMA2_HW 0x006a0ad4
// GLOBAL: CARMA2_SW 0x0069ab2c
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x006a0ad8
// GLOBAL: CARMA2_SW 0x0069ab30
tU32* gReal_render_palette;

// GLOBAL: CARMA2_HW 0x006a0ae0
// GLOBAL: CARMA2_SW 0x0069ab38
br_model* gModel_powerup_armor;

// GLOBAL: CARMA2_HW 0x006a0ae4
// GLOBAL: CARMA2_SW 0x0069ab3c
br_model* gModel_powerup_power;

// GLOBAL: CARMA2_HW 0x006a0ae8
// GLOBAL: CARMA2_SW 0x0069ab40
br_model* gModel_powerup_offence;

// GLOBAL: CARMA2_HW 0x006a0ac0
br_pixelmap* gFizzle_in[3];

// GLOBAL: CARMA2_HW 0x007059e0
// GLOBAL: CARMA2_SW 0x006fc4a0
tHeadup_icon gPickedup_powerups[20];

// GLOBAL: CARMA2_HW 0x00763480
// GLOBAL: CARMA2_SW 0x007599e0
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
// GLOBAL: CARMA2_SW 0x00751e60
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
// GLOBAL: CARMA2_SW 0x007438e0
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
// GLOBAL: CARMA2_SW 0x007581c0
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
// GLOBAL: CARMA2_SW 0x00743704
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
// GLOBAL: CARMA2_SW 0x0073b298
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
// GLOBAL: CARMA2_SW 0x0073b2a8
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007447d0
int gBlind_pedestrians;

// GLOBAL: CARMA2_HW 0x007447dc
// GLOBAL: CARMA2_SW 0x0073b29c
float gPed_gravity_multiplier;

// GLOBAL: CARMA2_HW 0x007447e4
int gPed_dismemberfest;

// GLOBAL: CARMA2_HW 0x007447f8
// GLOBAL: CARMA2_SW 0x0073b2b8
int gPanicking_peds;

/* explicit initializers keep gShit_mines and the following block adjacent (loop-end compares) */
// GLOBAL: CARMA2_HW 0x00705560
tShit_mine gShit_mines[20] = { 0 };

// GLOBAL: CARMA2_HW 0x007059c0
// GLOBAL: CARMA2_SW 0x006fc480
int gCount_mutant_tail_parts = 0;

/* UNCERTAIN: unreferenced dwords after gCount_mutant_tail_parts (only seen as gShit_mines loop ends); scalars, an initialized array would be 8-aligned */
// GLOBAL: CARMA2_HW 0x007059c4
int gUNK_007059c4 = 0;

// GLOBAL: CARMA2_HW 0x007059c8
int gUNK_007059c8 = 0;

/* explicit initializers keep gRespawn_powerups and gMutant_tail_first_collision_info adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x006a0458
tRespawn_powerup gRespawn_powerups[100] = { 0 };

// GLOBAL: CARMA2_HW 0x006a0908
// GLOBAL: CARMA2_SW 0x0069a960
tPhysics_object* gMutant_tail_object = NULL;

// GLOBAL: CARMA2_HW 0x006a0acc
// GLOBAL: CARMA2_SW 0x0069ab24
tPhysics_joint* gMutant_tail_first_joint;

// GLOBAL: CARMA2_HW 0x00705540
// GLOBAL: CARMA2_SW 0x006fc000
int gMutant_tail_state;

// GLOBAL: CARMA2_HW 0x00705b78
// GLOBAL: CARMA2_SW 0x006fc638
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x00705b74
// GLOBAL: CARMA2_SW 0x006fc634
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x0065ebbc
// GLOBAL: CARMA2_SW 0x0065951c
float gMass_mine = 1.0f;

// GLOBAL: CARMA2_HW 0x006a0454
// GLOBAL: CARMA2_SW 0x0069a4ac
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
// GLOBAL: CARMA2_SW 0x0069a968
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0940
// GLOBAL: CARMA2_SW 0x0069a998
int gInventory_cycling;

// GLOBAL: CARMA2_HW 0x006a0954
// GLOBAL: CARMA2_SW 0x0069a9ac
tU32 gInventory_timeout;

// GLOBAL: CARMA2_HW 0x0079ec50
// GLOBAL: CARMA2_SW 0x007951b0
int gInstant_handbrake;

// GLOBAL: CARMA2_HW 0x006a0960
tRepulse_link gRepulse_links[6] = { 0 };

// GLOBAL: CARMA2_HW 0x006a0a50
tU8* gRace_powerup_respawn_bools = NULL;

// GLOBAL: CARMA2_HW 0x006a0a54
// GLOBAL: CARMA2_SW 0x0069aaac
tPowerup* gPowerup_array = NULL;

// FUNCTION: CARMA2_HW 0x004d8d10
// FUNCTION: CARMA2_SW 0x004d5240
char* C2_HOOK_FASTCALL GetPowerupMessage(int pPowerup_index) {

    return gPowerup_array[pPowerup_index].message;
}

// GetNextGoodyTime

// DoPowerupTextHeadup

// FUNCTION: CARMA2_HW 0x004d9670
void C2_HOOK_FASTCALL QueueIconGoing(tPowerup* pPowerup, tHeadup_icon* pHeadup_icons, int* pCount_icons) {
    int i;

    for (i = 0; i < *pCount_icons; i++) {
        if (pPowerup == pHeadup_icons[i].powerup) {
            pHeadup_icons[i].fizzle_stage = 3;
            pHeadup_icons[i].fizzle_direction = -1;
            pHeadup_icons[i].fizzle_start = GetTotalTime();
            break;
        }
    }
}

// QueueIconComing

// LosePowerupX

// LosePowerup

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups

// ExcludedPowerup

// GotPowerupX

// GotPowerupXX

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d8d40
// STUB: CARMA2_SW 0x004d5270
void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// AutoGoody

// FUNCTION: CARMA2_HW 0x004d8d30
// FUNCTION: CARMA2_SW 0x004d5260
void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex) {
    GotPowerupEarwig(pCar, pIndex, 1, 1);
}

// DRMapFindMeAPixie


// STUB: CARMA2_HW 0x004d96c0
// STUB: CARMA2_SW 0x004d5b90
void C2_HOOK_FASTCALL LoadPowerups(void) {
    NOT_IMPLEMENTED();
}

// InitRepulseEffects

// MungeBoxObject

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

extern tPhysics_object* C2_HOOK_FASTCALL MungeSphereObject(br_model* pModel, float pWeight);

// FUNCTION: CARMA2_HW 0x004da530
void C2_HOOK_FASTCALL InitMineShit(void) {
    tPath_name the_path;
    tTWTVFS twt;
    int i;
    br_model* model;

    PathCat(the_path, gApplication_path, "MINE");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);

    C2_HOOK_BUG_ON(sizeof(tShit_mine) != 56);
    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);
    for (i = 0; i < (int)REC2_ASIZE(gShit_mines); i++) {
        gShit_mines[i].flags = 4;
        gShit_mines[i].field_0x35 = 0;
        model = BrModelFind("SHITMINE.ACT");
        gShit_mines[i].collision_info = MungeSphereObject(model, gMass_mine);
        gShit_mines[i].collision_info->uid = i;
        gShit_mines[i].collision_info->flags_0x238 = 0x40;
        gShit_mines[i].collision_info->owner = &gShit_mines[i];
        BrMatrix34Translate(&gShit_mines[i].collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gShit_mines[i].collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
    }
}

// STUB: CARMA2_HW 0x004d9ea0
// STUB: CARMA2_SW 0x004d6370
void C2_HOOK_FASTCALL InitPowerups(void) {
    NOT_IMPLEMENTED();
}

// RemoveRepulseEffect

// ResetRepulseEffects

// FUNCTION: CARMA2_HW 0x004da600
// FUNCTION: CARMA2_SW 0x004d6ad0
void C2_HOOK_FASTCALL EmptyInventory(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gInventory) != 20);

    for (i = 0; i < (int)REC2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
}

// ResetPowerups

// STUB: CARMA2_HW 0x004da730
// STUB: CARMA2_SW 0x004d6c00
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {
    NOT_IMPLEMENTED();
}

// NewRepulseRay

// RemoveRepulsiveReferences

// MungeRepulseRays

// AdjustRepulseRay

// FUNCTION: CARMA2_HW 0x004da990
void C2_HOOK_FASTCALL ResetRepulseRay(void) {
    int i = 0;

    do {
        tRepulse_link* link = &gRepulse_links[i];
        if (link->actor->parent == gNon_track_actor) {
            BrActorRemove(link->actor);
        }
        link->time = 0;
        i++;
    } while ((int)&gRepulse_links[i].actor < (int)&gINT_006a0a5c);
}

// FUNCTION: CARMA2_HW 0x004daff0
int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x) {
    br_pixelmap *fizzle_pix;
    char s[8];
    int timer;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dx, 0x294);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dy, 0x2a0);

    if (pIcon != NULL && pIcon->fizzle_stage < 4) {
        fizzle_pix = gFizzle_in[pPowerup->fizzle_type];
        if (pDraw) {
            PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 1, pTime);
        }
        if (pUpdate_x) {
            return pX + fizzle_pix->width;
        }
        return 0;
    }
    if (pDraw) {
        PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 0, pTime);
    }
    if (pPowerup->initial_value < 0) {
        sprintf(s, "%d", -pPowerup->value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->initial_value > 0) {
        TimerString(pPowerup->value, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->type == ePowerup_timed) {

        timer = pPowerup->lose_time - pTime;
        timer = timer < 0 ? 0 : timer;
        TimerString(timer, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[1],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[1], s);
        }
    } else if (pPowerup->current_value > 0) {

        sprintf(s, "%d", pPowerup->current_value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    }
    return 0;
}

// DrawPowerupIcons2

// DrawKeyPowerups

// FUNCTION: CARMA2_HW 0x004da9d0
// FUNCTION: CARMA2_SW 0x004d6ea0
void C2_HOOK_FASTCALL ToggleInventory(void) {

    gInventory_cycling = !gInventory_cycling;
    if (gInventory_cycling) {
        gInventory_timeout = GetTotalTime() + 3000;
    }
}

// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

/* inlined into MungePowerupStuff in the original (no standalone copy) */
static void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
        tRespawn_powerup* respawn_powerup = &gRespawn_powerups[i];
        if (respawn_powerup->actor == NULL || (int)pTime < (int)respawn_powerup->respawn_time) {
            continue;
        }
        if (respawn_powerup->actor->render_style != BR_RSTYLE_FACES) {
            respawn_powerup->actor->render_style = BR_RSTYLE_FACES;
        }
        PipeSinglePowerupRespawn(respawn_powerup->actor, respawn_powerup->index);
        DoPowerupRespawnSmash(respawn_powerup->actor);
        respawn_powerup->actor = NULL;
    }
}

/* MungeRepulseRays (0x004db8e0) is not part of this replay batch; an inert stub
   keeps MungePowerupStuff linkable. */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004db8e0
void C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004db880
void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pArg1) {
    tU32 time;

    time = GetTotalTime();
    MungeRepulseRays(time);
    CheckRespawnQueue(time);
}

void C2_HOOK_FASTCALL GotPowerupN(int pN) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + pN, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbdb0
// FUNCTION: CARMA2_SW 0x004d82b0
void C2_HOOK_FASTCALL GotPowerup0(void) {
    GotPowerupN(0);
}

// FUNCTION: CARMA2_HW 0x004dbe10
// FUNCTION: CARMA2_SW 0x004d8310
void C2_HOOK_FASTCALL GotPowerup1(void) {
    GotPowerupN(1);
}

// FUNCTION: CARMA2_HW 0x004dbe70
// FUNCTION: CARMA2_SW 0x004d8370
void C2_HOOK_FASTCALL GotPowerup2(void) {
    GotPowerupN(2);
}

// FUNCTION: CARMA2_HW 0x004dbed0
// FUNCTION: CARMA2_SW 0x004d83d0
void C2_HOOK_FASTCALL GotPowerup3(void) {
    GotPowerupN(3);
}

// FUNCTION: CARMA2_HW 0x004dbf30
// FUNCTION: CARMA2_SW 0x004d8430
void C2_HOOK_FASTCALL GotPowerup4(void) {
    GotPowerupN(4);
}

// FUNCTION: CARMA2_HW 0x004dbf90
// FUNCTION: CARMA2_SW 0x004d8490
void C2_HOOK_FASTCALL GotPowerup5(void) {
    GotPowerupN(5);
}

// FUNCTION: CARMA2_HW 0x004dbff0
// FUNCTION: CARMA2_SW 0x004d84f0
void C2_HOOK_FASTCALL GotPowerup6(void) {
    GotPowerupN(6);
}

// FUNCTION: CARMA2_HW 0x004dc050
// FUNCTION: CARMA2_SW 0x004d8550
void C2_HOOK_FASTCALL GotPowerup7(void) {
    GotPowerupN(7);
}

// FUNCTION: CARMA2_HW 0x004dc0b0
// FUNCTION: CARMA2_SW 0x004d85b0
void C2_HOOK_FASTCALL GotPowerup8(void) {
    GotPowerupN(8);
}

// FUNCTION: CARMA2_HW 0x004dc110
// FUNCTION: CARMA2_SW 0x004d8610
void C2_HOOK_FASTCALL GotPowerup9(void) {
    GotPowerupN(9);
}

// GotCredits

// FUNCTION: CARMA2_HW 0x004dc3e0
int C2_HOOK_FASTCALL GainAPO2(tCar_spec* pCar, int pIndex, int pAmount) {
    int old;

    while (pAmount < 0) {
        if (pCar->power_up_levels[pIndex] >= pCar->power_up_slots[pIndex]) {
            return 0;
        }
        pAmount = pCar->power_up_slots[pIndex] - pCar->power_up_levels[pIndex];
    }
    if (pIndex == 1 && gNet_mode != eNet_mode_none
        && gCurrent_net_game->type == eNet_game_type_foxy
        && gThis_net_player_index == gIt_or_fox) {
        return 0;
    }
    old = pCar->power_up_levels[pIndex];
    pCar->power_up_levels[pIndex] = old + pAmount;
    if (pCar->power_up_levels[pIndex] > pCar->power_up_slots[pIndex]) {
        pCar->power_up_levels[pIndex] = pCar->power_up_slots[pIndex];
    }
    return pCar->power_up_levels[pIndex] - old;
}

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

// FUNCTION: CARMA2_HW 0x004dc720
// FUNCTION: CARMA2_SW 0x004d8c20
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gPedestrian_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc750
// FUNCTION: CARMA2_SW 0x004d8c50
int C2_HOOK_FASTCALL SetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    br_vector3 scale;

    BrVector3Set(&scale, pPowerup->float_params[0], pPowerup->float_params[1], pPowerup->float_params[2]);
    ChangePedScaling(&scale);
    return pPowerup - gPowerup_array;
}

// SetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dc7d0
// FUNCTION: CARMA2_SW 0x004d8cd0
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar) {

    gExploding_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc800
// FUNCTION: CARMA2_SW 0x004d8d00
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc830
// FUNCTION: CARMA2_SW 0x004d8d30
int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    pCar->invulnerable_no_crushage = 1;
    pCar->invulnerable_no_damage = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc860
// FUNCTION: CARMA2_SW 0x004d8d60
void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && NetPlayerFromCar(pCar)->field_0x80 != 0) {
        return;
    }
    pCar->invulnerable_no_crushage = 0;
    pCar->invulnerable_no_damage = 0;
}

// FUNCTION: CARMA2_HW 0x004dc8a0
// FUNCTION: CARMA2_SW 0x004d8da0
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc8d0
// FUNCTION: CARMA2_SW 0x004d8dd0
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc8f0
// FUNCTION: CARMA2_SW 0x004d8df0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc920
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dc930
// FUNCTION: CARMA2_SW 0x004d8e30
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = !gFreeze_timer;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc970
// FUNCTION: CARMA2_SW 0x004d8e70
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = 0;
    }
}

// DoInstantRepair

// FUNCTION: CARMA2_HW 0x004dc9c0
// FUNCTION: CARMA2_SW 0x004d8ec0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    gPedestrian_speed_factor = 1.f;
}

// ResetPedSize

// ResetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dca10
// FUNCTION: CARMA2_SW 0x004d8f10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {

    gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 0;
}

// SetEngineFactor

// FUNCTION: CARMA2_HW 0x004dcab0
// FUNCTION: CARMA2_SW 0x004d8fb0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 1;
    return pPowerup - gPowerup_array;
}

// TrashBodywork

// FUNCTION: CARMA2_HW 0x004dcb90
// FUNCTION: CARMA2_SW 0x004d9090
int C2_HOOK_FASTCALL TakeDrugs(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_BUG_ON(sizeof(tU32) != 4);

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        if (gReal_render_palette == NULL) {
            gReal_render_palette = BrMemAllocate(256 * sizeof(tU32), kMem_misc);
            memcpy(gReal_render_palette, gRender_palette->pixels, 256 * sizeof(tU32));
        }
        gOn_drugs = 1;
    }
    return pPowerup - gPowerup_array;
}

void C2_HOOK_FASTCALL PaletteFuckedUpByDrugs(br_pixelmap* pPalette, int pOffset) {
    int i;

    (void)pPalette;
    if (pOffset == 0) {
        memcpy(gRender_palette->pixels, gReal_render_palette, 256 * sizeof(tU32));
    }
    for (i = 1; i < 224; i++) {
        ((tU32*)gRender_palette->pixels)[i] = gReal_render_palette[(i + pOffset) & 0xff];
    }
    ((tU32*)gRender_palette->pixels)[0] = gReal_render_palette[0];
    for (i = 224; i < 256; i++) {
        ((tU32*)gRender_palette->pixels)[i] = gReal_render_palette[i];
    }
}

// FUNCTION: CARMA2_HW 0x004dcc00
// FUNCTION: CARMA2_SW 0x004d9100
void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* pPowerup, tU32 pPeriod) {

    PaletteFuckedUpByDrugs(gRender_palette, GetTotalTime() / 100);
    ResetPalette();
}

// SetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004dcd70
// FUNCTION: CARMA2_SW 0x004d9270
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gCop_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcda0
// FUNCTION: CARMA2_SW 0x004d92a0
int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcdd0
// FUNCTION: CARMA2_SW 0x004d92d0
int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar) {

    gPinball_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce00
// FUNCTION: CARMA2_SW 0x004d9300
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce30
// FUNCTION: CARMA2_SW 0x004d9330
int C2_HOOK_FASTCALL SetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_bounce, 0x1598);

    pCar->bounce_rate = 1000.f / pPowerup->float_params[0];
    pCar->bounce_amount = pPowerup->float_params[1];
    pCar->last_bounce = 0;
    return pPowerup - gPowerup_array;
}

// SetSuspension

// FUNCTION: CARMA2_HW 0x004dced0
// FUNCTION: CARMA2_SW 0x004d93d0
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf00
// FUNCTION: CARMA2_SW 0x004d9400
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf30
// FUNCTION: CARMA2_SW 0x004d9430
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf40
// FUNCTION: CARMA2_SW 0x004d9440
int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf70
// FUNCTION: CARMA2_SW 0x004d9470
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf80
// FUNCTION: CARMA2_SW 0x004d9480
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FillInMine

// SendMines

// MinePreCollisionStuff

// ActivateMine

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// MineExplode

// FUNCTION: CARMA2_HW 0x004ddc10
// FUNCTION: CARMA2_SW 0x004da110
void C2_HOOK_FASTCALL TiggerMineToExplode(tCar_spec* pCar) {

    pCar->index |= 0x20;
}

// HitAShatOutMine

// ClearMineReferencesToCar

// MungeShatMines

// FUNCTION: CARMA2_HW 0x004de000
// FUNCTION: CARMA2_SW 0x004da500
void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor, br_matrix34* pMatrix) {
    (void)pMatrix;
    if (pActor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pActor);
    }
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004de020
// FUNCTION: CARMA2_SW 0x004da520
void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor, br_matrix34* pMatrix) {
    (void)pMatrix;
    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
    pActor->render_style = BR_RSTYLE_NONE;
}

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004de1f0
// STUB: CARMA2_SW 0x004da6f0
int C2_HOOK_FASTCALL RepulseOpponents2(tPowerup* pPowerup, tCar_spec* pCar, int pSend_message) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004de1e0
// FUNCTION: CARMA2_SW 0x004da6e0
int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* pPowerup, tCar_spec* pCar) {

    return RepulseOpponents2(pPowerup, pCar, 1);
}

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

// FUNCTION: CARMA2_HW 0x004de600
// FUNCTION: CARMA2_SW 0x004dab00
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 0;
}

// FUNCTION: CARMA2_HW 0x004de610
// FUNCTION: CARMA2_SW 0x004dab10
void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gOn_drugs = 0;
        PaletteFuckedUpByDrugs(gRender_palette, 0);
        ResetPalette();
    }
}

// ResetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004de710
// FUNCTION: CARMA2_SW 0x004dac10
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {

    gCop_speed_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de720
// FUNCTION: CARMA2_SW 0x004dac20
void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = gDefault_gravity;
}

// FUNCTION: CARMA2_HW 0x004de730
// FUNCTION: CARMA2_SW 0x004dac30
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {

    gPinball_factor = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de740
// FUNCTION: CARMA2_SW 0x004dac40
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004de750
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);

    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de760
// FUNCTION: CARMA2_SW 0x004dac60
void C2_HOOK_FASTCALL ResetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {

    SetCarSuspGiveAndHeight(pCar, 1.f, 1.f, 1.f, 0.f, 0.f);
}

// FUNCTION: CARMA2_HW 0x004de780
// FUNCTION: CARMA2_SW 0x004dac80
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de790
// FUNCTION: CARMA2_SW 0x004dac90
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de7a0
// FUNCTION: CARMA2_SW 0x004daca0
int C2_HOOK_FASTCALL PickAtRandom(tPowerup* pPowerup, tCar_spec* pCar) {
    int selected_powerup;

    selected_powerup = pPowerup->integer_params[IRandomBetween(0, pPowerup->number_of_integer_params - 1)];
    GotPowerup(pCar, selected_powerup);
    return -1;
}

// FUNCTION: CARMA2_HW 0x004de7d0
// FUNCTION: CARMA2_SW 0x004dacd0
int C2_HOOK_FASTCALL GotVouchers(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        AddVouchers(pPowerup->integer_params[0]);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de810
// FUNCTION: CARMA2_SW 0x004dad10
void C2_HOOK_FASTCALL MungeVouchers(tPowerup* pPowerup, tU32 pPeriod) {

    pPowerup->current_value = GetRecoverVoucherCount();
}

// FUNCTION: CARMA2_HW 0x004de820
// FUNCTION: CARMA2_SW 0x004dad20
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gInstant_handbrake = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de850
// FUNCTION: CARMA2_SW 0x004dad50
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gInstant_handbrake = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004de870
// FUNCTION: CARMA2_SW 0x004dad70
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8a0
// FUNCTION: CARMA2_SW 0x004dada0
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8d0
// FUNCTION: CARMA2_SW 0x004dadd0
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de900
// FUNCTION: CARMA2_SW 0x004dae00
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// SetGhostPeds

// FUNCTION: CARMA2_HW 0x004de960
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de970
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de980
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de990
// FUNCTION: CARMA2_SW 0x004dae90
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 0;
}

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// FUNCTION: CARMA2_HW 0x004debf0
// FUNCTION: CARMA2_SW 0x004db0f0
void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x4c8, 0x4c8);

    (void)pPowerup;
    pCar->field_0x4c8 = 1.f;
    if (gNet_mode != eNet_mode_none) {
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004dec10
int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec40
// FUNCTION: CARMA2_SW 0x004db140
void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dec60
int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = pPowerup->float_params[0] * pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec90
// FUNCTION: CARMA2_SW 0x004db190
void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = 0.f;
}

// FUNCTION: CARMA2_HW 0x004deca0
// FUNCTION: CARMA2_SW 0x004db1a0
int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar) {

    gMutant_speed = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004decd0
// FUNCTION: CARMA2_SW 0x004db1d0
void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar) {

    gMutant_speed = 0.f;
}

// GLOBAL: CARMA2_HW 0x006a0ad0
int gNumber_of_powerups;

extern tPlayer_ID gLocal_net_ID;

/* GotPowerupX (0x004d9150) is not implemented in this replay yet; an inert stub
   keeps ReceivedPowerup linkable and preserves the call target name. */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004d9150
int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004dece0
void C2_HOOK_FASTCALL ReceivedPowerup(tNet_message_chunk_powerup* pMessage) {
    tPowerup* powerup;
    tCar_spec* car;

    if (!gProgram_state.racing || pMessage->powerup_index >= gNumber_of_powerups || pMessage->powerup_index < 0) {
        return;
    }
    powerup = &gPowerup_array[pMessage->powerup_index];
    if (pMessage->field_0x10 != 0 && pMessage->field_0x10 != 1) {
        if ((powerup->net_type & 1) == 0 && powerup->lose_proc != NULL) {
            car = NetCarFromPlayerID(pMessage->player_id);
            if (car != NULL) {
                powerup->lose_proc(powerup, car);
                car->powerups[pMessage->powerup_index] = 0;
            }
        }
        return;
    }
    if ((powerup->net_type & 1) != 0) {
        GotPowerupX(&gProgram_state.current_car, pMessage->powerup_index, 0, pMessage->field_0x10 == 0, pMessage->time_remaining);
    } else if (pMessage->player_id != gLocal_net_ID && powerup->got_proc != NULL) {
        car = NetCarFromPlayerID(pMessage->player_id);
        if (car != NULL) {
            powerup->got_proc(powerup, car);
            switch (powerup->type) {
            case ePowerup_instantaneous:
                break;
            case ePowerup_timed:
                car->powerups[pMessage->powerup_index] = GetTotalTime() + pMessage->time_remaining;
                break;
            case ePowerup_whole_race:
                car->powerups[pMessage->powerup_index] = -1;
                return;
            default:
                break;
            }
        }
    }
}

// STUB: CARMA2_HW 0x004dedf0
// STUB: CARMA2_SW 0x004db2f0
void C2_HOOK_FASTCALL SendCurrentPowerups(void) {
    NOT_IMPLEMENTED();
}

// LoseAllLocalPowerups

// LoseDodgyPowerups

// FUNCTION: CARMA2_HW 0x004df1e0
// FUNCTION: CARMA2_SW 0x004db6e0
int C2_HOOK_FASTCALL GetPowerupCount(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004df1f0
// FUNCTION: CARMA2_SW 0x004db6f0
void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition) {

}

// FUNCTION: CARMA2_HW 0x004df200
// FUNCTION: CARMA2_SW 0x004db700
void C2_HOOK_FASTCALL FUN_004df200(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->got_proc != NULL) {
        pPowerup->got_proc(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004df210
// FUNCTION: CARMA2_SW 0x004db710
void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pCar);
    }
}

// SetPissed

// ResetPissed

// PissedEffect

// FUNCTION: CARMA2_HW 0x004df650
// FUNCTION: CARMA2_SW 0x004dbb50
void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    if (pActor->user == NULL) {
        pActor->user = (void*)(uintptr_t)GetTotalTime();
    } else {
        tU32 prev;
        tU32 now;
        tU32 elapsed;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)pActor->user;
        elapsed = (now - prev) / 16;
        pActor->user = (void*)(uintptr_t)now;
        BrMatrix34PreRotateY(&pActor->t.t.mat, BR_ANGLE_DEG(elapsed));
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004dfe10
// FUNCTION: CARMA2_SW 0x004dc310
void C2_HOOK_CDECL RenderChangingPowerup(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    if (pActor->user == NULL) {
        pActor->user = (void*)(uintptr_t)GetTotalTime();
        pActor->model = gModel_powerup_armor;
    } else {
        tU32 now;
        tU32 elapsed;

        now = GetTotalTime();
        elapsed = (now - (tU32)(uintptr_t)pActor->user) / 16;
        if (elapsed > 15) {
            if (pActor->model == gModel_powerup_armor) {
                pActor->model = gModel_powerup_power;
            } else if (pActor->model == gModel_powerup_power) {
                pActor->model = gModel_powerup_offence;
            } else {
                pActor->model = gModel_powerup_armor;
            }
            pActor->user = (void*)(uintptr_t)now;
        }
        BrMatrix34PreRotateY(&pActor->t.t.mat, BR_ANGLE_DEG(elapsed + 12));
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004df570
// FUNCTION: CARMA2_SW 0x004dba70
void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity) {
    // GLOBAL: CARMA2_HW 0x0065ebc0
    // GLOBAL: CARMA2_SW 0x00659520
    static br_token_value spinning_powerup_extra_prim[2] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { BrScalarToFixed(75) } },
    };
    unsigned int i;

    if (pActor == NULL) {
        return;
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.f, .1f, 0.f);
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderSpinningPowerup;
    if (pOpacity == 0 || pOpacity == 255) {
        return;
    }
    if (pActor->material != NULL) {
        pActor->material->extra_prim = spinning_powerup_extra_prim;
        pActor->material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
        BrMaterialUpdate(pActor->material, BR_MATU_EXTRA_PRIM);
        return;
    }
    for (i = 0; i < pActor->model->nfaces; i++) {
        br_material* material;

        material = pActor->model->faces[i].material;
        if (material != NULL) {
            material->extra_prim = spinning_powerup_extra_prim;
            pActor->model->faces[i].material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
            BrMaterialUpdate(pActor->model->faces[i].material, BR_MATU_EXTRA_PRIM);
        }
    }
}

// DuplicateModel

// SetChangingPowerup

// PedValium

// PowerupCancel

// GrowATail

// CutOffTail

// FUNCTION: CARMA2_HW 0x004e06a0
// FUNCTION: CARMA2_SW 0x004dcba0
void C2_HOOK_FASTCALL RemoveTail(void) {
    tPhysics_object* object;

    if (gMutant_tail_state != 0) {
        object = gMutant_tail_object;
        if (gMutant_tail_object != NULL) {
            while (object != NULL) {
                BrActorRemove(object->actor);
                object = object->child;
            }
        }
        if (gMutant_tail_state == 2) {
            PhysicsRemoveObject(gMutant_tail_object);
        } else {
            PHILRemoveObject(gMutant_tail_object);
        }
        gMutant_tail_object->physics_joint1 = gMutant_tail_first_joint;
        gMutant_tail_state = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e0710
// FUNCTION: CARMA2_SW 0x004dcc10
void C2_HOOK_FASTCALL DoInitialTail(void) {
    tPhysics_object* object;

    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        object->actor->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004e0730
// FUNCTION: CARMA2_SW 0x004dcc30
void C2_HOOK_FASTCALL UndoInitialTail(void) {
    tPhysics_object* object;

    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        object->actor->render_style = BR_RSTYLE_NONE;
    }
}

// MayQueuePowerupRespawn

// FUNCTION: CARMA2_HW 0x004e07d0
// FUNCTION: CARMA2_SW 0x004dccd0
void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor, int pPowerup_id) {
    (void)pPowerup_id;

    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004e07e0
// FUNCTION: CARMA2_SW 0x004dcce0
void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pPowerup_id) {
    (void)pPowerup_id;

    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e07f0
void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
    float f;
    float scale;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale != 0) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.0f;
            if (pIcon->fizzle_direction > 0) {
                f = 1.0f - f;
                scale = 1.0f + 5.0f * f;
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.0f);
            } else {
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, 1.01f - f, 1.01f - f, 1.0f);
            }
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, (br_angle)((1.0f - f / 2.0f) * 65536.0f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.0f - f) * 255.0f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT(255);
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004e09d0
// FUNCTION: CARMA2_SW 0x004dccf0
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture) {
    br_actor* actor;
    br_material* material;
    br_model* model;
    int width;
    int height;
    br_scalar left;
    br_scalar right;
    br_scalar top;
    br_scalar bottom;

    width = pTexture->width;
    height = pTexture->height;
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    left = (float)-(width / 2);
    top = (float)+(height / 2);
    BrVector3Set(&model->vertices[0].p, left,  top, -2.0f);
    bottom = (float)-(height / 2);
    BrVector3Set(&model->vertices[1].p, left, bottom, -2.0f);
    right = (float)+(width / 2);
    BrVector3Set(&model->vertices[2].p, right, bottom, -2.0f);
    BrVector3Set(&model->vertices[3].p, right,  top, -2.0f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 0.0f);
    material->colour = 0;
    material->colour_map = pTexture;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pTexture);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x004e0c00
// FUNCTION: CARMA2_SW 0x004dcf20
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c10
// FUNCTION: CARMA2_SW 0x004dcf30
void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {

    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c20
// FUNCTION: CARMA2_SW 0x004dcf40
void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {

    gPowerup_txt_path = "ZOMPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c30
// FUNCTION: CARMA2_SW 0x004dcf50
void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {

    gPowerup_txt_path = "ALPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
// FUNCTION: CARMA2_SW 0x004dcf60
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && !IsCarCloaked(pCar)) {
        BlendifyCar(pCar);
        MasterDisableCarFunks(pCar);
        gCloaked_cars[gCount_cloaked_cars++] = pCar;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {

            if (gCloaked_cars[i] == pCar) {
#ifdef REC2_FIX_BUGS
                memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1], (gCount_cloaked_cars - i - 1) * sizeof(tCar_spec*));
#else
                memcpy(&gCloaked_cars[i], &gCloaked_cars[i + 1], (REC2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec*));
#endif
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
// FUNCTION: CARMA2_SW 0x004dd030
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0d50
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(pCar);
        RemoveFromCloakingList(pCar);
        MasterEnableCarFunks(pCar);
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004e0de0
void C2_HOOK_FASTCALL PeriodicCloaking(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}

// STUB: CARMA2_HW 0x004dd1b0
void C2_HOOK_FASTCALL MinePreCollisionStuff(void) {
    NOT_IMPLEMENTED();
}


extern tPed_personality* gPed_personalities[50];
extern tPed_form* gPed_forms[20];
extern float gPed_sin_table[256];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* ScalePedPersonality (0x0040bb20) and ScalePedForm (0x0040bee0) are not implemented in rec2
   yet; inert stubs keep ScaleAllPeds linkable and runtime-safe. */
// STUB: CARMA2_HW 0x0040bb20
void C2_HOOK_FASTCALL ScalePedPersonality(tPed_personality* pPersonality, int pIndex, br_scalar* pScale, int pMode) {
    (void)pPersonality; (void)pIndex; (void)pScale; (void)pMode;
}
// STUB: CARMA2_HW 0x0040bee0
void C2_HOOK_FASTCALL ScalePedForm(tPed_form* pForm, int pIndex, br_scalar* pScale, int pMode) {
    (void)pForm; (void)pIndex; (void)pScale; (void)pMode;
}
// FUNCTION: CARMA2_HW 0x0040c420
void C2_HOOK_FASTCALL ScaleAllPeds(int pMorph_or_personality, br_scalar* pScale, int pMode) {
    int i;

    for (i = 0; i < 50; i++) {
        int index;

        if (gPed_personalities[i] == NULL) {
            continue;
        }
        if (pMorph_or_personality != -2) {
            index = pMorph_or_personality;
        } else {
            index = gPed_personalities[i]->form->index_head_bone;
        }
        ScalePedPersonality(gPed_personalities[i], index, pScale, pMode);
    }
    for (i = 0; i < 20; i++) {
        int index;

        if (gPed_forms[i] == NULL) {
            continue;
        }
        if (pMorph_or_personality != -2) {
            index = pMorph_or_personality;
        } else {
            index = gPed_forms[i]->index_head_bone;
        }
        ScalePedForm(gPed_forms[i], index, pScale, pMode);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


extern char gJoystick_effects[];
extern int gCount_joystick_effects;

extern int gForceFeedbackAvailable;
extern int gCurrent_ffb_effect_index;
extern int* C2_HOOK_FASTCALL GetJoystickEffect(int index);

// GLOBAL: CARMA2_HW 0x0059630c
int gCurrent_ffb_effect_index = -1;

/* StopDinputEffect (0x00457ed0) lives in the platform dinput TU, which is not part
   of this replay batch; an inert stub keeps PlayExclusiveFFBEffect linkable. */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00457ed0
void C2_HOOK_FASTCALL StopDinputEffect(int pIndex) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0045c6b0
void C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2) {
    int index;
    int* effect;

    if (!gForceFeedbackAvailable) {
        return;
    }
    index = PDFindJoystickEffect(pEffect_name);
    if (index == -1) {
        return;
    }
    if (gCurrent_ffb_effect_index != -1 && gCurrent_ffb_effect_index != index) {
        StopDinputEffect(gCurrent_ffb_effect_index);
        UnloadDinputFFBEffectAtIndex(gCurrent_ffb_effect_index);
        gCurrent_ffb_effect_index = -1;
    }
    if (gCurrent_ffb_effect_index != -1) {
        effect = GetJoystickEffect(gCurrent_ffb_effect_index);
        if (pArg2 == 0 && effect != NULL && effect[2] == -1) {
            return;
        }
    }
    gCurrent_ffb_effect_index = index;
    StartDinputEffect(index);
}

// FUNCTION: CARMA2_HW 0x00458430
int* C2_HOOK_FASTCALL GetJoystickEffect(int index) {

    if (index < 0 || index > gCount_joystick_effects) {
        return NULL;
    }
    return (int*)&gJoystick_effects[index * 232];
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c8da0
void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {

    ARDoSingleVariedSession(0x34, (uintptr_t)pPowerup, 1, 4, 0, (uintptr_t)pCar);
}


// FUNCTION: CARMA2_HW 0x004c8fb0
void C2_HOOK_FASTCALL PipeSingleMatrix34Type66(br_actor* pActor) {
    extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

    ARDoSingleVariedSession(0x42, (uintptr_t)pActor, 1,
        0x30, 0, (uintptr_t)&pActor->t.t.mat);
}


typedef struct {
    float distance;
    tPedestrian* ped;
} tNapalm_ped_distance;

typedef struct {
    int count;
    tNapalm_ped_distance peds[100];
} tNapalm_ped_distances;

// FUNCTION: CARMA2_HW 0x004ce770
int C2_HOOK_FASTCALL NapalmPedDistanceCallback(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pNow, tPedestrian_distances* pDistances) {
    tNapalm_ped_distances* data = (tNapalm_ped_distances*)pDistances;
    int result;

    result = (int)pPed->field_0x0c;
    if (result != 0 && pPed->field_0x0c->field_0x98 == 0) {
        data->peds[data->count].ped = pPed;
        data->peds[data->count].distance = pDistance_squared;
        data->count++;
        result = 1;
    }
    return result;
}


// FUNCTION: CARMA2_HW 0x004ce7b0
int C2_HOOK_CDECL NapalmPedDistanceCompare(const void* pValue1, const void* pValue2) {
    return (int)(((const tNapalm_ped_distance*)pValue1)->distance - ((const tNapalm_ped_distance*)pValue2)->distance);
}


extern float gPed_head_scale;
extern int gPed_count;

// FUNCTION: CARMA2_HW 0x004d6ab0
void C2_HOOK_STDCALL ScalePedHeads(float pHead_scale) {
    br_scalar s[3];
    int i;
    tPedestrian* ped;

    s[0] = pHead_scale / gPed_head_scale;
    s[1] = s[0];
    s[2] = s[0];
    ScaleAllPeds(-2, s, 0);
    gPed_head_scale = pHead_scale;
    if (gAction_replay_mode != 0) {
        if (ARGetReplayRate() < 0.0f) {
            ped = gPedestrian_array;
            for (i = 0; i < gPed_count; i++, ped++) {
                if (ped->flags & 1) {
                    SetCharacterBonePositions(ped->character, 3, 0);
                }
            }
        }
    }
}


extern int gEthereal_pedestrians;
extern void C2_HOOK_FASTCALL MungeAllCharacterMaterials(void (C2_HOOK_FASTCALL* pCallback)(void*));
extern void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL* pCallback)(void*));
extern void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL* pCallback)(void*));

// FUNCTION: CARMA2_HW 0x004d6b40
void C2_HOOK_FASTCALL MakePedsEthereal(void) {

    MungeAllCharacterMaterials((void (C2_HOOK_FASTCALL*)(void*))BlendifyMaterial25);
    MungeAllCharacterObjects((void (C2_HOOK_FASTCALL*)(void*))DisableCollisionsForEthereal);
    MungeAllCharacterModels((void (C2_HOOK_FASTCALL*)(void*))ModelUpdatePrimitiveMaterials);
    gEthereal_pedestrians = 1;
}


// FUNCTION: CARMA2_HW 0x004da870
void C2_HOOK_FASTCALL AddRepulseLink(tU32 pTime, tCar_spec* pCar, tCar_spec* pCar2, br_vector3* pOrigin, br_vector3* pVictim, float pField_0x18, float pField_0x1c) {
    tRepulse_link* link;
    tU32 link_time;
    tU32 min_time;
    int index;
    int best;
    int i;

    min_time = 0x7fffffff;
    best = (int)pOrigin;
    index = -1;
    i = 0;
    link = &gRepulse_links[0];
    while ((int)link < (int)&gRepulse_links[6]) {
        link_time = link->time;
        if (link_time == 0) {
            index = i;
            break;
        }
        if (link_time < min_time) {
            min_time = link_time;
            best = i;
        }
        link++;
        i++;
    }
    if (index < 0) {
        index = best;
    }
    gRepulse_links[index].pos_origin = pOrigin;
    gRepulse_links[index].pos_victim = pVictim;
    gRepulse_links[index].time = pTime;
    gRepulse_links[index].field_0x10 = 0xffffffff;
    gRepulse_links[index].field_0x18 = pField_0x18;
    gRepulse_links[index].field_0x1c = pField_0x1c;
    gRepulse_links[index].car = pCar;
    gRepulse_links[index].field_0x24 = (undefined4)pCar2;
}


// GLOBAL: CARMA2_HW 0x0058ac00
const float gConst_0p5 = 0.5f;

// GLOBAL: CARMA2_HW 0x0058ac70
const double gConst_minus_0p15 = -0.15;

// FUNCTION: CARMA2_HW 0x004dd590
void C2_HOOK_FASTCALL InitializeMineMotion(tShit_mine* pMine, tCar_spec* pCar) {
    extern const double gConst_minus_0p15;
    br_matrix34 car_transform;
    br_vector3 impulse;
    double d;

    if (pMine->flags & 0x40) {
        BrMatrix34Translate(&pMine->collision_info->actor->t.t.mat,
                            0.f,
                            (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gConst_0p5,
                            pCar->collision_info->bb1.min.v[2] - 0.15);
    } else {
        BrMatrix34Translate(&pMine->collision_info->actor->t.t.mat,
                            0.f,
                            (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gConst_0p5,
                            pCar->collision_info->bb1.max.v[2] - gConst_minus_0p15);
    }
    BrMatrix34Copy(&car_transform, &pCar->collision_info->transform_matrix);
    BrMatrix34Post(&pMine->collision_info->actor->t.t.mat, &car_transform);
    BrMatrix34Copy(&pMine->collision_info->transform_matrix, &pMine->collision_info->actor->t.t.mat);
    impulse.v[0] = 0.f;
    impulse.v[1] = (float)(d = ((float*)pMine->field_0x1c)[1]);
    impulse.v[2] = (float)(d = ((float*)pMine->field_0x1c)[2]);
    BrMatrix34ApplyV(&pMine->collision_info->v, &impulse, &car_transform);
    pMine->collision_info->v.v[0] = pCar->collision_info->v.v[0] + pMine->collision_info->v.v[0];
    pMine->collision_info->v.v[1] = pCar->collision_info->v.v[1] + pMine->collision_info->v.v[1];
    pMine->collision_info->v.v[2] = pCar->collision_info->v.v[2] + pMine->collision_info->v.v[2];
    if (pMine->flags & 0x40) {
        pMine->collision_info->v.v[0] = pCar->collision_info->v.v[0] + pMine->collision_info->v.v[0];
        pMine->collision_info->v.v[1] = pCar->collision_info->v.v[1] + pMine->collision_info->v.v[1];
        pMine->collision_info->v.v[2] = pCar->collision_info->v.v[2] + pMine->collision_info->v.v[2];
    }
    pMine->collision_info->omega.v[0] = 0.f;
    pMine->collision_info->omega.v[1] = 0.f;
    pMine->collision_info->omega.v[2] = 0.f;
}


// FUNCTION: CARMA2_HW 0x004de5a0
int C2_HOOK_FASTCALL ReceivedRepulseOpponents(tNet_message_chunk_powerup* pMessage) {
    extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);

    return RepulseOpponents2(&gPowerup_array[pMessage->powerup_index], NetCarFromPlayerID(pMessage->player_id), 0);
}


// FUNCTION: CARMA2_HW 0x004e0ba0
void C2_HOOK_FASTCALL DisposeBillBoard(br_actor* pActor) {
    extern br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* pMaterial);
    extern void C2_HOOK_CDECL BrMaterialFree(br_material* pMaterial);
    extern br_model* C2_HOOK_CDECL BrModelRemove(br_model* pModel);
    extern void C2_HOOK_CDECL BrModelFree(br_model* pModel);
    extern br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* pActor);
    extern void C2_HOOK_CDECL BrActorFree(br_actor* pActor);

    if (pActor != NULL) {
        if (pActor->material != NULL) {
            BrMaterialRemove(pActor->material);
            BrMaterialFree(pActor->material);
        }
        if (pActor->model != NULL) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
        }
        if (pActor->parent != NULL) {
            BrActorRemove(pActor);
        }
        BrActorFree(pActor);
    }
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004c60a0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) {
    NOT_IMPLEMENTED();
}

// GLOBAL: CARMA2_HW 0x006a0adc
int gCollision_info_uid_counter;

// FUNCTION: CARMA2_HW 0x004da400
tPhysics_object* C2_HOOK_FASTCALL MungeSphereObject(br_model* pModel, float pWeight) {
    tPhysics_object* collision_info;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);

    collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    collision_info->shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.f, 0.f, 0.f);
    collision_info->shape->sphere.sphere.radius = (pModel->bounds.max.v[0] - pModel->bounds.min.v[0] + pModel->bounds.max.v[1] - pModel->bounds.min.v[1] + pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) / 6.f;
    FillInShape(collision_info->shape);
    collision_info->uid = gCollision_info_uid_counter;
    gCollision_info_uid_counter++;
    UpdateCollisionObject(collision_info);
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f);
    BrVector3SetFloat(&collision_info->field_0x54, 0.f, 1/-17.25f, 0.f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = gFace_num__car - 2;
    return collision_info;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
