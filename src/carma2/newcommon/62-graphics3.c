#include "62-graphics3.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "53-controls.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <limits.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00655dfc
int gMini_map_visible = 2;

// GLOBAL: CARMA2_HW 0x0074a780
tCar_icon gCar_icons[128];

// GLOBAL: CARMA2_HW 0x0074ab84
br_actor* gCar_icons_model_actor;

// GLOBAL: CARMA2_HW 0x0068d6f0
tCar_spec* gTarget_lock_car_2;

// GLOBAL: CARMA2_HW 0x0068d8c4
tCar_spec* gTarget_lock_car_1;

// GLOBAL: CARMA2_HW 0x0068d8c8
int gTarget_lock_enabled;

// GLOBAL: CARMA2_HW 0x0068d8cc
int gInitMap_done;

// GLOBAL: CARMA2_HW 0x0074abdc
float gHeadup_map_x_float;

// GLOBAL: CARMA2_HW 0x0074abd8
float gHeadup_map_y_float;

// GLOBAL: CARMA2_HW 0x0074abcc
float gHeadup_map_w_float;

// GLOBAL: CARMA2_HW 0x0074ab80
float gHeadup_map_h_float;

// GLOBAL: CARMA2_HW 0x0074ab88
int gINT_0074ab88;

// GLOBAL: CARMA2_HW 0x0074ab8c
int gINT_0074ab8c;

// GLOBAL: CARMA2_HW 0x0074ab94
int gINT_0074ab94;

// GLOBAL: CARMA2_HW 0x0074ab98
int gHeadup_map_half_height;

// GLOBAL: CARMA2_HW 0x0074abe4
int gHeadup_map_half_width;

// GLOBAL: CARMA2_HW 0x0074abec
int gINT_0074abec;

// GLOBAL: CARMA2_HW 0x0068d8d8
br_pixelmap* gMap_overlay;

// GLOBAL: CARMA2_HW 0x0074aba0
int gMini_map_x;

// GLOBAL: CARMA2_HW 0x0074aba4
int gMini_map_y;

// GLOBAL: CARMA2_HW 0x0074abd4
int gINT_0074abd4;

// GLOBAL: CARMA2_HW 0x0074abd0
int gINT_0074abd0;

// GLOBAL: CARMA2_HW 0x0068d8dc
br_pixelmap* gMini_map;

// GLOBAL: CARMA2_HW 0x00655f04
int gINT_00655f04;

// GLOBAL: CARMA2_HW 0x00655f08
int gINT_00655f08;

// GLOBAL: CARMA2_HW 0x0068d8a0
float gFLOAT_0068d8a0;

// GLOBAL: CARMA2_HW 0x0068d8a4
float gFLOAT_0068d8a4;

// GLOBAL: CARMA2_HW 0x0068d8a8
float gMini_map_arrow_z;

// GLOBAL: CARMA2_HW 0x0074ab9c
float gFLOAT_0074ab9c;

// GLOBAL: CARMA2_HW 0x0074abb8
float gFLOAT_0074abb8;

// GLOBAL: CARMA2_HW 0x0068d6e4
br_pixelmap* gCheckpoint_numbers;

// GLOBAL: CARMA2_HW 0x0068d884
br_pixelmap* gSmashy_dot;

// GLOBAL: CARMA2_HW 0x0068c87c
br_pixelmap* gAlt_meter;

// GLOBAL: CARMA2_HW 0x0068c880
// GLOBAL: CARMA2_SW 0x006868d8
br_matrix23 gMatrix23_0068c880;

// GLOBAL: CARMA2_HW 0x0068c868
// GLOBAL: CARMA2_SW 0x006868c0
br_vector3 gOrigin_map;

// GLOBAL: CARMA2_HW 0x0068d898
// GLOBAL: CARMA2_SW 0x006878f0
br_vector2 gOrigin_headup_map;

// GLOBAL: CARMA2_HW 0x0068c860
int gCheckpoint_digit_height;

// GLOBAL: CARMA2_HW 0x0068d8bc
int gCheckpoint_digit_center_x;

// GLOBAL: CARMA2_HW 0x0068c85c
int gCheckpoint_digit_center_y;

// GLOBAL: CARMA2_HW 0x0068d6f8
tOppo_status_messages gOppo_status_messages[9];

// GLOBAL: CARMA2_HW 0x0074abe0
int gCount_smashable_race_targets;

// GLOBAL: CARMA2_HW 0x0068c898
tSmashable_race_target gSmashable_race_targets[300];

// GLOBAL: CARMA2_HW 0x0074abc0
int gMap_render_x_i;

// GLOBAL: CARMA2_HW 0x0074abbc
int gMap_render_y_i;

// GLOBAL: CARMA2_HW 0x0074abe8
int gMap_render_width_i;

// GLOBAL: CARMA2_HW 0x0074abc8
int gMap_render_height_i;

// GLOBAL: CARMA2_HW 0x007634b4
int gTrack_version;

// CarArrowColour

/* local declarations */
#include "01-network.h"
#include "30-opponent.h"
#include <stdio.h>
extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);

// FUNCTION: CARMA2_HW 0x004945f0
void C2_HOOK_FASTCALL ToggleTargetLock(void) {
    char name[256];
    char s[256];

    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_foxy) {
            return;
        }
        if (!gCurrent_net_game->options.show_players_on_map) {
            return;
        }
    }
    if (gTarget_lock_car_1 == NULL) {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_no_target_to_lock_onto), 0);
    } else {
        gTarget_lock_enabled = !gTarget_lock_enabled;
        if (gTarget_lock_enabled) {
            if (gNet_mode != eNet_mode_none) {
                strcpy(name, NetPlayerFromCar(gTarget_lock_car_2)->player_name);
            } else {
                strcpy(name, GetDriverName((tU16)gTarget_lock_car_2->car_ID >> 8, (tU16)gTarget_lock_car_2->car_ID & 0xff));
            }
            sprintf(s, "%s %s", GetMiscString(eMiscString_locked_onto), name);
            NewTextHeadupSlot2(4, 0, 1500, -4, s, 0);
        } else {
            NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_target_lock_off), 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00494700
void C2_HOOK_FASTCALL CycleTargetLock(void) {
    /* based on carpocalypse2 src/carma2/common/controls.c:1367 */
    int car_count;
    int car_index;
    int category;
    tCar_spec* car;
    int next_index;
    tCar_spec* c;

    if (gNet_mode == eNet_mode_none || (gCurrent_net_game->type != eNet_game_type_foxy && gCurrent_net_game->options.show_players_on_map)) {
        category = gNet_mode != eNet_mode_none ? eVehicle_net_player : eVehicle_opponent;
        if (gTarget_lock_car_1 == NULL) {
            if (category == eVehicle_self) {
                gTarget_lock_car_1 = &gProgram_state.current_car;
            } else {
                gTarget_lock_car_1 = GetCarSpec(category, 0);
            }
        }
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (car_index = 0;; car_index++) {
            if (car_index >= car_count) {
                return;
            }
            if (category == eVehicle_self) {
                c = &gProgram_state.current_car;
            } else {
                c = GetCarSpec(category, car_index);
            }
            if (c == gTarget_lock_car_1) {
                break;
            }
        }
        if (car_index == car_count - 1) {
            next_index = 0;
        } else {
            next_index = car_index + 1;
        }

        do {
            do {
                if (category == eVehicle_self) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(category, next_index);
                }
                if (next_index == car_count - 1) {
                    next_index = 0;
                } else {
                    next_index += 1;
                }
            } while (car != gTarget_lock_car_1 && car->knackered);
        } while ((gNet_mode == eNet_mode_none && gCurrent_race.race_spec->race_type == kRaceType_Cars && !car->is_race_goal)
                 || (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_6 && NetPlayerFromCar(car)->field_0x80));
        gTarget_lock_car_1 = car;
        gTarget_lock_car_2 = car;
        gTarget_lock_enabled = 0;
        ToggleTargetLock();
    }
}

// FUNCTION: CARMA2_HW 0x00494880
// FUNCTION: CARMA2_SW 0x00491b80
void C2_HOOK_FASTCALL ToggleMapTransparency(void) {

    gMap_trans = !gMap_trans;
    SaveOptions();
}

// GLOBAL: CARMA2_HW 0x00659b28
int gCheckpoint_finder_enabled = 1;

/* local declaration (15-displays.h is not included in this file) */
extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);

// FUNCTION: CARMA2_HW 0x00494840
void C2_HOOK_FASTCALL CycleMapOptions(void) {

    gCheckpoint_finder_enabled = !gCheckpoint_finder_enabled;
    if (gCheckpoint_finder_enabled) {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_checkpoint_finder_turned_on), 0);
    } else {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_checkpoint_finder_turned_off), 0);
    }
}

// FUNCTION: CARMA2_HW 0x004948a0
void C2_HOOK_FASTCALL InitTargetLock(void) {

    gTarget_lock_enabled = 0;
}

// FUNCTION: CARMA2_HW 0x004948b0
void C2_HOOK_FASTCALL SetIntegerMapRenders(void) {

    gMap_render_width = 128.f;
    gMap_render_height = 80.f;
    gMap_render_x_i = ((int)gMap_render_x) & ~3;
    gMap_render_y_i = ((int)gMap_render_y) & ~1;
    gMap_render_width_i = (int)gMap_render_width;
    gMap_render_height_i = (int)gMap_render_height;
}
// OppositeColour

// GLOBAL: CARMA2_HW 0x006593a0
const int gArrows[2][4][60] = {
    {
        // inner arrow (=fill)
        { 10,  0,  0, -1,  0,  1,  0,  0, -1,  0, -2,  0,  1, -1,  1,  1,  1, -2,  2,  2,  2, },
        { 11,  0,  0, -1,  0,  1,  0,  0, -1,  1, -1,  1, -2, -2,  1, -1,  1,  0,  1,  1,  1,  1,  2, },
        {  9,  0,  0, -2,  0, -1,  0,  1,  0,  0, -1,  1, -1,  2, -2,  0,  1,  0,  2, },
        { 11,  0,  0, -1,  0,  1,  0, -2, -1, -1, -1,  0, -1,  1, -1,  2, -1, -1,  1,  0,  1, -1,  2, },
    },
    {
        // outer arrow (=border)
        { 26,  1, -3,  1, -2,  1, -1,  2, -1,  2,  0,  2,  1,  3,  1,  3,  2,  3,  3,  2,  3,  1,  3,  1,  2,  0,  2, -1,  2,
          -1,  3, -2,  3, -3,  3, -3,  2, -3,  1, -2,  1, -2,  0, -2, -1, -1, -1, -1, -2, -1, -3,  0, -3, },
        { 22,  0, -3,  1, -3,  2, -3,  2, -2,  2, -1,  2,  0,  2,  1,  2,  2,  2,  3,  1,  3,  0,  3,  0,  2, -1,  2, -2,  2,
          -3,  2, -3,  1, -3,  0, -2,  0, -2, -1, -1, -1, -1, -2,  0, -2, },
        { 24,  1, -3,  2, -3,  3, -3,  3, -2,  3, -1,  2, -1,  2,  0,  2,  1,  1,  1,  1,  2,  1,  3,  0,  3, -1,  3, -1,  2,
          -1,  1, -2,  1, -3,  1, -3,  0, -3, -1, -2, -1, -1, -1, -1, -2,  0, -2,  1, -2, },
        { 22, -3, -2, -2, -2, -1, -2,  0, -2,  1, -2,  2, -2,  3, -2,  3, -1,  3,  0,  2,  0,  2,  1,  1,  1,  1,  2,  0,  2,
           0,  3, -1,  3, -2,  3, -2,  2, -2,  1, -2,  0, -3,  0, -3, -1, },
    },
};

// based on carpocalypse2 src/carma2/common/graphics.c:1327 (reference only)
extern tU16 C2_HOOK_FASTCALL Colour24BitTo16Bit(br_colour pColour);

// FUNCTION: CARMA2_HW 0x00495a00
// STUB: CARMA2_SW 0x00492400
void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3* pPos, br_uint_32 pColour) {
    int colours[2];
    int colour;
    int x;
    int y;
    int temp;
    int point_count;
    const int* arrow_ptr;
    int i;
    int j;

    if (pColour == 0xffffffff) {
        colours[0] = 0;
        colours[1] = 0x84;
    } else {
        colours[0] = pColour;
        if ((int)pColour < 0xe0) {
            if ((int)(pColour & 0x7) < 4) {
                colours[1] = 0xff;
            } else {
                colours[1] = 0;
            }
        } else {
            if ((int)(pColour & 0xf) < 8) {
                colours[1] = 0xff;
            } else {
                colours[1] = 0;
            }
        }
    }
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        colours[0] = PaletteEntry16Bit(gRender_palette, colours[0]);
        colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        if (pColour == 0xffffffff) {
            colours[1] = Colour24BitTo16Bit(0xff00ff);
        }
    }
    for (i = 0; i < 2; i++) {
        arrow_ptr = &gArrows[i][pArrow_index & 0x3][0];
        point_count = *arrow_ptr++;
        for (j = 0; j < point_count; j++) {
            colour = colours[i];
            if (pArrow_index & 0x8) {
                x = -*arrow_ptr++;
                y = -*arrow_ptr++;
            } else {
                x = *arrow_ptr++;
                y = *arrow_ptr++;
            }
            if (pArrow_index & 0x4) {
                temp = y;
                y = x;
                x = -temp;
            }
            BrPixelmapPixelSet(pScreen, (int)(pPos->v[0] + x), (int)(pPos->v[1] + y), colour);
        }
    }
}

// GLOBAL: CARMA2_HW 0x0068c858
int gINT_0068c858;

// GLOBAL: CARMA2_HW 0x0068c874
int gINT_0068c874;

// GLOBAL: CARMA2_HW 0x0068c878
int gINT_0068c878;

// GLOBAL: CARMA2_HW 0x0068d6a8
br_matrix34 gCar_in_map_space;

// GLOBAL: CARMA2_HW 0x0068d6d8
br_vector2 gVector2_0068d6d8;

// GLOBAL: CARMA2_HW 0x0068d6e0
float gFLOAT_0068d6e0;

// GLOBAL: CARMA2_HW 0x0068d890
int gINT_0068d890;

// based on carpocalypse2 src/carma2/common/graphics.c:1387 (reference only)
// FUNCTION: CARMA2_HW 0x00496270
void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour) {
    tU32 time_diff;
    br_vector3 map_pos;
    float tv1;
    float tv2;
    float local_pos[2];
    int colour;
    int colours[2];
    int arrow_index;
    int size;
    extern float gFLOAT_0074ab90;
    extern int gIt_or_fox;
    extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);

    time_diff = pTime - gMap_time;
    BrMatrix34ApplyP(&map_pos, pPos, &gCurrent_race.map_transformation);
    map_pos.v[2] = 0.0;
    if (((pCar != NULL && pCar->driver == eDriver_oppo) || (pCar != NULL && pCar->driver == eDriver_net_human))
            && (!pCar->knackered || pCar == gTarget_lock_car_1)
            && (gCurrent_race.race_spec->race_type != kRaceType_Cars || pCar->is_race_goal)
            && (gNet_mode == eNet_mode_none
                    || (gCurrent_net_game->type == eNet_game_type_6 && NetPlayerFromCar(pCar)->field_0x80 == 0)
                    || (gCurrent_net_game->type == eNet_game_type_foxy && pCar == gNet_players[gIt_or_fox].car)
                    || (gCurrent_net_game->type != eNet_game_type_6 && gCurrent_net_game->type != eNet_game_type_foxy))) {
        local_pos[0] = (float)(gHeadup_map_half_width + gINT_0068d890 + gINT_0074ab8c) - map_pos.v[0];
        local_pos[1] = (float)(gHeadup_map_half_height + gINT_0068c858 + gINT_0074ab88) - map_pos.v[1];
        tv1 = -map_pos.v[2];
        tv2 = local_pos[1] * local_pos[1] + local_pos[0] * local_pos[0] + tv1 * tv1;
        if (((gTarget_lock_car_2 == NULL
                || (!gTarget_lock_enabled && tv2 < gFLOAT_0074ab90)
                || (gTarget_lock_enabled && pCar == gTarget_lock_car_2)))
                        && !pCar->knackered) {
            gVector2_0068d6d8.v[0] = local_pos[0];
            gVector2_0068d6d8.v[1] = local_pos[1];
            gFLOAT_0068d6e0 = tv1;
            gTarget_lock_car_2 = pCar;
            gFLOAT_0074ab90 = tv2;
        }
    }
    if (gMini_map_visible || gMap_view == 2) {
        BrMatrix34Mul(&gCar_in_map_space, pTrans, &gCurrent_race.map_transformation);
        arrow_index = ((int)((int)(360.f - FastScalarArcTan2(gCar_in_map_space.m[2][0], gCar_in_map_space.m[2][1]) + 12.25) * (1.0 / 22.5))) % 16;
        if (pCar == NULL || pCar->driver != eDriver_local_human || (pTime & 0x100)) {
            DrawArrow(pScreen, arrow_index, &map_pos, pColour);
        }
        colours[0] = pColour;
        colours[1] = (pColour < 0xe0) ? (((pColour & 7) < 4) ? 0xff : 0) : (((pColour & 0xf) < 8) ? 0xff : 0);
        if (gBack_screen->type != BR_PMT_INDEX_8) {
            colours[0] = PaletteEntry16Bit(gRender_palette, pColour);
            colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        }
        colour = colours[pTime & 0x100];
        if (gNet_mode != eNet_mode_none
                && gCurrent_net_game->type == eNet_game_type_foxy
                && pCar == gNet_players[gIt_or_fox].car) {
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] - 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] + 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
        }
        if (time_diff <= 500 && pCar != NULL && pCar->driver == eDriver_local_human) {
            size = (35000 - 70 * time_diff) / 500;
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] - 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] + 0.5f) + size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
        }
    }
}

// DrawSmashBlip

// DrawMapSmallBlip

// CopyMapToScreen

// DrawCarArrow

// FUNCTION: CARMA2_HW 0x00495ba0
void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev) {
    int i;
    int checkpoint;
    int checkpoints[10];
    int checkpoint_flags;

    C2_HOOK_ASSERT(gCurrent_race.check_point_count <= REC2_ASIZE(checkpoints));
    for (i = 0; i < gCurrent_race.check_point_count; i++) {
        checkpoints[i] = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
            checkpoint_flags = gNet_players[gThis_net_player_index].score2;
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (checkpoint_flags & 0x1) {
                    pCallback(pMap, i, pTime, 1);
                    checkpoints[i] = 1;
                }
                checkpoint_flags >>= 1;
            }
        } else if (gCurrent_net_game->type == eNet_game_type_5 || gCurrent_net_game->type == eNet_game_type_4) {
            if (gNet_players[gThis_net_player_index].score2 >= 0) {
                checkpoint = gNet_players[gThis_net_player_index].score2 % gCurrent_race.check_point_count;
                pCallback(pMap, checkpoint, pTime, 1);
                checkpoints[checkpoint] = 1;
            }
        }
    } else {
        pCallback(pMap, gCheckpoint - 1, pTime, 1);
        checkpoints[gCheckpoint - 1] = 1;
    }
    if (pDraw_prev) {
        if (gNet_mode == eNet_mode_none
                || gCurrent_net_game->type == eNet_game_type_checkpoint
                || gCurrent_net_game->type == eNet_game_type_4
                || gCurrent_net_game->type == eNet_game_type_5) {
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (!checkpoints[i]) {
                    pCallback(pMap, i, pTime, 0);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004968f0
void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 p;
    tRace_checkpoint_spec* checkpoint;

    checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
    BrMatrix34ApplyP(&p, &checkpoint->pos, &gCurrent_race.map_transformation);
    BrVector2Copy(&checkpoint->map_position, &p);
}

// FUNCTION: CARMA2_HW 0x00496940
void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 pos_mapspace;
    br_vector2 p;
    tRace_checkpoint_spec* checkpoint;

    checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
    BrMatrix34ApplyP(&pos_mapspace, &checkpoint->pos, &gCurrent_race.map_transformation);
    BrVector2Set(&p, pos_mapspace.v[0] - gOrigin_map.v[0] + gOrigin_headup_map.v[0], pos_mapspace.v[1] - gOrigin_map.v[1] + gOrigin_headup_map.v[1]);
    BrMatrix23TApplyV(&checkpoint->map_position, &p, &gMatrix23_0068c880);
    checkpoint->map_position.v[0] += (float)gINT_0074abd4;
    checkpoint->map_position.v[1] += (float)gINT_0074abd0;
}
// FUNCTION: CARMA2_HW 0x004969e0
void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    // GLOBAL: CARMA2_HW 0x0068d8e8
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d8ec
    static int flash_rate;
    tRace_checkpoint_spec* checkpoint;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count
            && gTrack_version > 0 && pTarget == 0) || Flash(300, &last_flash, &flash_rate)) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];

        if (checkpoint->map_position.v[0] >= 0.f
                && checkpoint->map_position.v[0] < (float)pMap->width
                && checkpoint->map_position.v[1] >= 0.f
                && checkpoint->map_position.v[1] < (float)pMap->height) {

            if (gMap_view == 2) {
                PossibleLock(1);
            }
            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(checkpoint->map_position.v[0] - (float)gCheckpoint_digit_center_x),
                (int)(checkpoint->map_position.v[1] - (float)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0,
                pCheckpoint * gCheckpoint_digit_height,
                gCheckpoint_numbers->width,
                gCheckpoint_digit_height);
            if (gMap_view == 2) {
                PossibleUnlock(1);
            }
        }
    }
}
// FUNCTION: CARMA2_HW 0x00495d00
void C2_HOOK_FASTCALL DrawOffsetCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    // GLOBAL: CARMA2_HW 0x0068d8f0
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d8f4
    static int flash_state;
    tRace_checkpoint_spec* checkpoint;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count && gTrack_version > 0 && !pTarget)
            || Flash(300, &last_flash, &flash_state)) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];

        if (checkpoint->map_position.v[0] >= 0.f
                && checkpoint->map_position.v[0] < (float)pMap->width
                && checkpoint->map_position.v[1] >= 0.f
                && checkpoint->map_position.v[1] < (float)pMap->height) {

            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(checkpoint->map_position.v[0] + 8.f - (float)gCheckpoint_digit_center_x),
                (int)(checkpoint->map_position.v[1] - (float)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0, pCheckpoint * gCheckpoint_digit_height,
                gCheckpoint_numbers->width, gCheckpoint_digit_height);
        }
    }
}
extern int C2_HOOK_FASTCALL GetPedCount(void);
extern int C2_HOOK_FASTCALL GetPowerupCount(void);
extern void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tPedestrian* gPedestrian_array;

static int C2_HOOK_FASTCALL GetPedPosition(int pIndex, br_vector3* pPos) {
    (void)pIndex; (void)pPos;
    return 0;
}

static void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3* pPos, int pColour) {
    (void)pScreen; (void)pTime; (void)pPos; (void)pColour;
}

static void C2_HOOK_FASTCALL DrawCarArrow(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_vector3* pPos, int pCategory) {
    (void)pScreen; (void)pCar; (void)pTime; (void)pPos; (void)pCategory;
}

static void C2_HOOK_FASTCALL DrawSmashBlip(br_pixelmap* pScreen, tU32 pTime, br_vector3* pPos, int pIndex) {
    (void)pScreen; (void)pTime; (void)pPos; (void)pIndex;
}

/* based on carpocalypse2 src/carma2/common/displays.c:553 */
// FUNCTION: CARMA2_HW 0x00495e10
void C2_HOOK_FASTCALL DoMapOverlays(br_pixelmap* pScreen) {
    tU32 the_time;
    int category;
    int car_count;
    tCar_spec* car;
    int ped_count;
    int powerup_count;
    tNet_game_player_info* net_player;
    int i;
    br_vector3 ped_position;
    br_vector3* smash_position;
    br_actor* actor;
    tSmashable_race_target* smash_target;

    the_time = PDGetTotalTime();
    if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
        if (gMap_view == 2) {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint, 1);
            DoSomeThingsToCheckpoints(pScreen, the_time, DrawCheckpoint, 1);
        } else {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint2, 1);
        }
    }
    if ((gShow_peds_on_map || gCurrent_race.race_spec->race_type == kRaceType_Peds)
            || (gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)) {

        ped_count = GetPedCount();
        for (i = 0; i < ped_count; i++) {
            if (GetPedPosition(i, &ped_position)) {
                DrawMapSmallBlip(pScreen, the_time, &ped_position, 52);
            }
        }
    }

    if ((gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)
            || gCurrent_race.race_spec->expansion) {

        powerup_count = GetPowerupCount();
        for (i = 0; i < powerup_count; i++) {
            GetPowerupPosition(i, &ped_position);
            DrawMapSmallBlip(pScreen, the_time, &ped_position, 4);
        }
    }
    for (category = gShow_opponents ? eVehicle_opponent : eVehicle_self; category >= (gMap_view == 1 ? eVehicle_net_player : eVehicle_self); category -= 1) {
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (i = 0; i < car_count; i++) {
            if (category == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(category, i);
            }
            if (gNet_mode != eNet_mode_none) {
                net_player = NetPlayerFromCar(car);
            }
            if (gNet_mode == eNet_mode_none
                    || (!car->knackered && !net_player->wasted && net_player->player_status >= ePlayer_status_racing)) {

                if (category == eVehicle_self) {
                    actor = gPlayer_car_master_actor;
                } else {
                    actor = GetCarSpec(category, i)->car_master_actor;
                }
                DrawCarArrow(pScreen, car, the_time, &actor->t.t.translate.t, category);
            }
        }
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        for (i = 0; i < gCount_smashable_race_targets; i++) {
            smash_target = &gSmashable_race_targets[i];

            if ((smash_target->model != NULL && smash_target->actor->render_style != BR_RSTYLE_NONE && smash_target->actor->model == smash_target->model)
                    || (smash_target->field_0x8 >= 0 && gPedestrian_array[smash_target->field_0x8].hit_points > 0)) {
                if (smash_target->field_0x8 >= 0) {
                    smash_position = &gPedestrian_array[smash_target->field_0x8].pos;
                } else {
                    smash_position = &smash_target->actor->t.t.translate.t;
                }

                DrawSmashBlip(pScreen, the_time, smash_position, i);
            }
        }
    }
}

// StartMap

// FinishMap

// FUNCTION: CARMA2_HW 0x00496b10
void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    int p2_x;
    int p2_y;
    tRace_checkpoint_spec* checkpoint;

    if (pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] < 0.f) {
            p2_x = 8;
            p2_y = (int)((checkpoint->map_position.v[1] - (float)gINT_0074abd0)
                * ((float)gINT_0074abd4 / ((float)gINT_0074abd4 - checkpoint->map_position.v[0]))
                + (float)gINT_0074abd0);
        } else {
            p2_x = (int)(checkpoint->map_position.v[0] + 8.f);
            p2_y = (int)checkpoint->map_position.v[1];
        }
        FancyDrawLine(pMap, gINT_0074abd4 + 8, gINT_0074abd0, p2_x, p2_y, 132, 100);
    }
}

// DoDirectionFinderStuff

// MungeMoodMessage

// DoOpponentStatusHeadup

// AllocateOppoBarActor

// DoNearestOpponentRelativeHeightMarkerStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00494900
void C2_HOOK_FASTCALL DoOpponentStatusHeadup(void) {
    NOT_IMPLEMENTED();
}

// STUB: not implemented yet
void C2_HOOK_FASTCALL DoNearestOpponentRelativeheightMarkerStuff(br_pixelmap* pMap) {
}

// STUB: not implemented yet
static void C2_HOOK_FASTCALL DoDirectionFinderStuff(br_pixelmap* pMap) {
}

// STUB: not implemented yet
static br_uint_32 C2_HOOK_FASTCALL CarArrowColour(tCar_spec* pCar, tVehicle_type pCategory) {
    return 0;
}

extern void C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);

/* based on carpocalypse2 src/carma2/common/displays.c:1059 */
// FUNCTION: CARMA2_HW 0x004950b0
void C2_HOOK_FASTCALL MapOverlay(void) {
    extern int gHeadup_detail_level;
    tU32 the_time;
    br_uint_16 original_base_x;
    br_matrix23 mat23;
    br_vector3 player_pos;

    BrMatrix34ApplyP(&gOrigin_map, &gPlayer_car_master_actor->t.t.translate.t, &gCurrent_race.map_transformation);
    gOrigin_headup_map.v[0] = gOrigin_map.v[0] - (float)(gINT_0074ab94 / 2);
    gOrigin_headup_map.v[1] = gOrigin_map.v[1] - (float)(gINT_0074abec / 2);
    gINT_0068c878 = (int)(gOrigin_headup_map.v[0] + .5);
    gINT_0068c874 = (int)(gOrigin_headup_map.v[1] + .5);
    if (gINT_0068c878 < 0 || gINT_0068c874 < 0
            || gINT_0068c878 > gCurrent_race.map_image->width
            || gINT_0068c874 > gCurrent_race.map_image->height) {
        BrPixelmapFill(gMap_overlay, 0);
    }
    if (gINT_0074ab94 > gINT_0074abec) {
        gINT_0068c858 = (gINT_0074ab94 - gINT_0074abec) / 2;
        gINT_0068d890 = 0;
        BrPixelmapRectangleCopy(gMap_overlay,
            0, 0,
            gCurrent_race.map_image,
            gINT_0068c878, gINT_0068c874 - gINT_0068c858,
            gINT_0074ab94, gINT_0074ab94);
    } else {
        gINT_0068c858 = 0;
        gINT_0068d890 = (gINT_0074abec - gINT_0074ab94) / 2;
        BrPixelmapRectangleCopy(gMap_overlay,
            0, 0,
            gCurrent_race.map_image,
            gINT_0068c878 - gINT_0068d890, gINT_0068c874,
            gINT_0074abec, gINT_0074abec);
    }
    gCurrent_race.map_transformation.m[3][0] -= gOrigin_headup_map.v[0] - (float)gINT_0068d890;
    gCurrent_race.map_transformation.m[3][1] -= gOrigin_headup_map.v[1] - (float)gINT_0068c858;
    BrMatrix34Mul(&gCar_in_map_space,
        &gProgram_state.current_car.car_master_actor->t.t.mat,
        &gCurrent_race.map_transformation);
    mat23.m[0][0] = gCar_in_map_space.m[0][0];
    mat23.m[0][1] = gCar_in_map_space.m[0][1];
    mat23.m[1][0] = gCar_in_map_space.m[2][0];
    mat23.m[1][1] = gCar_in_map_space.m[2][1];
    mat23.m[2][0] = 0.f;
    mat23.m[2][1] = 0.f;
    BrMatrix23LPNormalise(&gMatrix23_0068c880, &mat23);
    DoMapOverlays(gMap_overlay);
    gCurrent_race.map_transformation.m[3][0] += gOrigin_headup_map.v[0] - (float)gINT_0068d890;
    gCurrent_race.map_transformation.m[3][1] += gOrigin_headup_map.v[1] - (float)gINT_0068c858;
    the_time = PDGetTotalTime();
    PossibleLock(1);
    if (gMini_map_visible && gMap_overlay != NULL) {
        DRPixelmapRotatedAndFeatheredCopy(&gMatrix23_0068c880,
            gBack_screen,
            gHeadup_map_x + 8, gHeadup_map_y,
            gMap_overlay,
            gINT_0068d890 + gINT_0074ab8c, gINT_0068c858 + gINT_0074ab88,
            gHeadup_map_w, gHeadup_map_h,
            gMap_trans);
    }
    InitMap();

    original_base_x = gMini_map->base_x;
    gMini_map->base_x = gMini_map->base_x / 2;
    DoDirectionFinderStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    if (gMini_map_visible) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1
                || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {

            DoSomeThingsToCheckpoints(gMini_map, the_time, DrawOffsetCheckpoint, 1);
        }
        if (gMini_map->type == BR_PMT_INDEX_8) {
            gMini_map->base_x = 0;
        } else {
            gMini_map->base_x /= 2;
        }
        player_pos.v[0] = (float)(gHeadup_map_w / 2 + gHeadup_map_x + 8);
        player_pos.v[1] = (float)(gHeadup_map_h / 2 + gHeadup_map_y);
        player_pos.v[2] = gMini_map_arrow_z;
        DrawArrow(gBack_screen, 0, &player_pos, CarArrowColour(&gProgram_state.current_car, eVehicle_self));
    }
    DoNearestOpponentRelativeheightMarkerStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    PossibleUnlock(1);
    if (gHeadup_detail_level > 2) {
        DoOpponentStatusHeadup();
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// InitNearestCar

// GLOBAL: CARMA2_HW 0x00659b20
int gPrevious_opponent_status = -1;

// GLOBAL: CARMA2_HW 0x0068d8d0
tU32 gTime_oppobar_target_wasted;

// GLOBAL: CARMA2_HW 0x0074ab90
float gFLOAT_0074ab90;

// GLOBAL: CARMA2_HW 0x0068d6f4
int gINT_0068d6f4;

// GLOBAL: CARMA2_HW 0x0068d6e8
int gBack_original_origin_y;

// GLOBAL: CARMA2_HW 0x0068d6ec
int gBack_original_origin_x;

// GLOBAL: CARMA2_HW 0x0068d888
int gBack_original_base_y;

// GLOBAL: CARMA2_HW 0x0068d88c
int gBack_original_base_x;

#if defined(REC2_MATCHING)
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x005191b0
void C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h) {

    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}
#if defined(REC2_MATCHING)
#pragma auto_inline(on)
#endif
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);

// FUNCTION: CARMA2_HW 0x00496be0
void C2_HOOK_FASTCALL MapStuffBeforeRender(void) {
    gINT_0068d6f4 = -1;
    if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy) {
        gTarget_lock_car_2 = NULL;
    } else if (gPrevious_opponent_status == eOpponent_status_Wasted && gTarget_lock_car_1 != NULL) {
        if (PDGetTotalTime() - gTime_oppobar_target_wasted <= 2500) {
            gFLOAT_0074ab90 = 0.f;
        } else {
            gTarget_lock_car_2 = NULL;
            gTarget_lock_car_1 = NULL;
            if (gTarget_lock_enabled) {
                gTarget_lock_enabled = 0;
            }
        }
    } else if (gTarget_lock_enabled) {
        gFLOAT_0074ab90 = 0.f;
    } else {
        gTarget_lock_car_2 = NULL;
    }
    if (gCurrent_race.map_image != NULL && gMap_view == 2) {
        gBack_original_origin_x = gBack_screen->origin_x;
        gBack_original_origin_y = gBack_screen->origin_y;
        gBack_original_base_x = gBack_screen->base_x;
        gBack_original_base_y = gBack_screen->base_y;
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;
        gBack_screen->base_x = 0;
        gBack_screen->base_y = 0;
        if (gCurrent_race.map_image != NULL) {
            if (gGraf_data_index != 0) {
                PossibleUnlock(1);
                FRONTEND_Redraw(gBack_screen, -gBack_screen->origin_x, -gBack_screen->origin_y, gCurrent_race.map_image, gCurrent_race.map_image->origin_x, gCurrent_race.map_image->origin_y, 640, 480);
            } else {
                DRPixelmapCopy(gBack_screen, gCurrent_race.map_image);
            }
        }
        DimRectangleClipped(gBack_screen, gMap_render_x_i - gCurrent_graf_data->map_render_x_marg, gMap_render_y_i - gCurrent_graf_data->map_render_y_marg, gMap_render_width_i + gMap_render_x_i + gCurrent_graf_data->map_render_x_marg, gMap_render_height_i + gMap_render_y_i + gCurrent_graf_data->map_render_y_marg, 1);
    }
}

// MapStuffAfterRender

static void C2_HOOK_FASTCALL ReadMoodMessages(void) {
    tPath_name path;
    FILE *f;
    int mood;
    char s[256];

    PathCat(path, gApplication_path, "STATUS.TXT");
    f = PFfopen(path, "rt");
    if (f != NULL) {
        for (mood = 0; mood < (int)REC2_ASIZE(gOppo_status_messages); mood += 1) {
            int i;

            C2_HOOK_ASSERT(mood < REC2_ASIZE(gOppo_status_messages));

            gOppo_status_messages[mood].count = GetAnInt(f);
            C2_HOOK_ASSERT(gOppo_status_messages[mood].count < REC2_ASIZE(gOppo_status_messages[mood].messages));
            for (i = 0; i < gOppo_status_messages[mood].count; i++) {


                GetALineAndDontArgue(f, s);
                gOppo_status_messages[mood].messages[i] = (char *) BrMemAllocate(strlen(s) + 1, kMem_misc_string);
                strcpy(gOppo_status_messages[mood].messages[i], s);
            }
        }
        PFfclose(f);
    } else {
        FatalError(kFatalError_CantOpen_S, "STATUS.TXT");
    }
}

// FUNCTION: CARMA2_HW 0x00496e30
void C2_HOOK_FASTCALL InitMap(void) {

    if (!gInitMap_done) {
        gHeadup_map_x_float = (float)gHeadup_map_x;
        gHeadup_map_y_float = (float)gHeadup_map_y;
        gHeadup_map_w_float = (float)gHeadup_map_w;
        gHeadup_map_h_float = (float)gHeadup_map_h;
    } else {
        gHeadup_map_x = ((int)gHeadup_map_x_float + 3) & (tU16)~3;
        gHeadup_map_y = (int)gHeadup_map_y_float;
        gHeadup_map_w = ((int)gHeadup_map_w_float + 3) & (tU16)~3;
        gHeadup_map_h = (int)gHeadup_map_h_float;
    }
    if (gHeadup_map_x % 4 != 0) {
        gHeadup_map_x -= gHeadup_map_x % 4;
        gHeadup_map_x_float = (float)gHeadup_map_x;
    }
    if (gHeadup_map_w % 4 != 0) {
        gHeadup_map_w -= gHeadup_map_w % 4;
        gHeadup_map_w_float = (float)gHeadup_map_w;
    }
    if (gHeadup_map_w > 64) {
        gHeadup_map_w = 64;
        gHeadup_map_w_float = 64.0f;
    }
    if (gHeadup_map_h > 64) {
        gHeadup_map_h = 64;
        gHeadup_map_h_float = 64.0f;
    }
    gINT_0074ab8c = (int)(3.0 + 0.3 * gHeadup_map_w) & (tU16)~3;
    gINT_0074ab88 = (int)(3.0 + 0.3 * gHeadup_map_h) & (tU16)~3;
    if (gINT_0074ab8c > gINT_0074ab88) {
        gINT_0074ab88 = gINT_0074ab8c;
    } else {
        gINT_0074ab8c = gINT_0074ab88;
    }
    gINT_0074ab94 = gHeadup_map_w + 2 * gINT_0074ab88;
    gINT_0074abec = gHeadup_map_h + 2 * gINT_0074ab88;
    gHeadup_map_half_width = gHeadup_map_w / 2;
    gHeadup_map_half_height = gHeadup_map_h / 2;
    if (gMap_overlay == NULL) {
        gMap_overlay = DRPixelmapAllocate(gBack_screen->type, MAX(gINT_0074abec, gINT_0074ab94), MAX(gINT_0074abec, gINT_0074ab94), NULL, 0);
    }
    gMini_map_x = gHeadup_map_x + gHeadup_map_w / 16;
    gMini_map_y = gHeadup_map_y + gHeadup_map_h / 16;
    if (gMini_map_x % 2 != 0) {
        gMini_map_x -= 1;
    }
    gINT_0074abd4 = gHeadup_map_w / 2 - gMini_map_x + gHeadup_map_x;
    gINT_0074abd0 = gHeadup_map_h / 2 - gMini_map_y + gHeadup_map_y;
    if (gMini_map != NULL) {
        BrPixelmapFree(gMini_map);
    }
    gINT_00655f04 = gHeadup_map_x;
    gINT_00655f08 = gHeadup_map_y;
    gMini_map = DRPixelmapAllocateSub(gBack_screen,
        gMini_map_x,
        gMini_map_y,
        gHeadup_map_w - (gHeadup_map_w + 8) / 8 + 8,
        gHeadup_map_h - gHeadup_map_h / 8);
    gMini_map->pixels = (br_uint_8*)gMini_map->pixels + gMini_map_y * gMini_map->row_bytes + gMini_map_x;
    gMini_map->base_x = gMini_map_x;
    gMini_map->base_y = 0;
    gFLOAT_0068d8a0 = (float)gINT_0074abd4;
    gFLOAT_0068d8a4 = (float)gINT_0074abd0;
    gMini_map_arrow_z = 0.0f;
    gFLOAT_0074ab9c = (float)REC2_SQR((double)(gHeadup_map_half_height + gHeadup_map_half_width) * 0.45);
    gFLOAT_0074abb8 = (float)REC2_SQR(gCurrent_graf_data->field_0x51c + 1);
    if (!gInitMap_done) {
        gCheckpoint_numbers = LoadPixelmap("CPNUMB.PIX");
        if (gCheckpoint_numbers == NULL) {
            FatalError(kFatalError_CantFindFile_S, "CPNUMB.PIX");
        }
        BRPM_convert(gCheckpoint_numbers, gBack_screen->type);
        gSmashy_dot = LoadPixelmap("SMASHY.PIX");
        if (gSmashy_dot == NULL) {
            FatalError(kFatalError_CantFindFile_S, "SMASHY.PIX");
        }
        BRPM_convert(gSmashy_dot, gBack_screen->type);
        gAlt_meter = LoadPixelmap("ALTMETER.PIX");
        if (gAlt_meter == NULL) {
            FatalError(kFatalError_CantFindFile_S, "ALTMETER.PIX");
        }
        gCheckpoint_digit_height = gCheckpoint_numbers->height / 10;
        gCheckpoint_digit_center_y = gCheckpoint_digit_height / 2;
        gCheckpoint_digit_center_x = gCheckpoint_numbers->width / 2;
        gIcons_pix = LoadPixelmap("CARICONS.PIX");
        if (gIcons_pix != NULL) {
            BRPM_convert(gIcons_pix, gBack_screen->type);
            BrMapAdd(gIcons_pix);
        }
        ReadMoodMessages();
    }
    gInitMap_done = 1;
}

// FUNCTION: CARMA2_HW 0x004973b0
void C2_HOOK_FASTCALL ConvertCarIcons(br_pixelmap* pMap) {
    int i;
    int j;
    int count_car_icons;
    int icon_width;
    int icon_height;
    int texture_x;
    int texture_y;
    tCar_icon* icon;
    int car_icon_height;
    br_model* model;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_icons) != 128);
    memset(gCar_icons, 0, sizeof(gCar_icons));
    car_icon_height = gCurrent_graf_data->car_icon_height;
    count_car_icons = gIcons_pix->height / car_icon_height;

    icon_width = NearestPowerOfTwo(pMap->width);
    icon_height = NearestPowerOfTwo(car_icon_height);

    texture_x = 0;
    texture_y = 0;
    for (i = 0; i < count_car_icons; i++) {
        icon = &gCar_icons[i];

        if (gTexture_maps[gSize_font_texture_pages] == NULL) {
            gTexture_maps[gSize_font_texture_pages] = BrPixelmapAllocate(pMap->type, 64, 64, NULL, 0);
            if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                FatalError(kFatalError_CouldNotCreateTexturesPages_S, "Car Icons");
            }
            BrMapAdd(gTexture_maps[gSize_font_texture_pages]);
        }
        DRPixelmapRectangleCopy(
            gTexture_maps[gSize_font_texture_pages],
            texture_x,
            texture_y,
            pMap,
            0,
            car_icon_height * i,
            icon_width,
            car_icon_height);
        BrMapUpdate(gTexture_maps[gSize_font_texture_pages], BR_MAPU_ALL);
        icon->index = gSize_font_texture_pages;
        icon->model = model = CreateCharacterModel(icon_width, icon_height, texture_x, texture_y, "CAR ICONS");
        for (j = 0; j < 4; j++) {
            model->vertices[j].red = 0xff;
            model->vertices[j].grn = 0xff;
            model->vertices[j].blu = 0xff;
        }
        model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
        BrModelUpdate(model, BR_MODU_ALL);
        texture_x += icon_width;
        if (texture_x >= 64) {
            texture_x = 0;
            texture_y += icon_height;
            if (texture_y >= 64) {
                gSize_font_texture_pages++;
                if (gSize_font_texture_pages >= (int)REC2_ASIZE(gTexture_maps)) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, "CAR ICONS");
                }
                texture_x = 0;
                texture_y = 0;
            }
        }
    }
    gSize_font_texture_pages++;
    gCar_icons_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gCar_icons_model_actor->material = BrMaterialAllocate("Car Icon Material");
    gCar_icons_model_actor->material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(gCar_icons_model_actor->material);
}

static void C2_HOOK_FASTCALL MapChanged(void) {

    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00497610
void C2_HOOK_FASTCALL UpdateMapAndSaveOptions(void) {
    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00497620
void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0, int pKey1) {

    if (KeyIsDown(31) && pKey0) {
        gHeadup_map_y_float = (float)((double)gHeadup_map_y_float - (double)gFrame_period / 20.0);
        if (gHeadup_map_y_float < 0.0f) {
            gHeadup_map_y_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(32) && pKey0) {
        gHeadup_map_y_float = (float)((double)gHeadup_map_y_float + (double)gFrame_period / 20.0);
        if (gHeadup_map_y_float + gHeadup_map_h_float > (float)gBack_screen->height) {
            gHeadup_map_y_float = (float)(gBack_screen->height - gHeadup_map_h);
        }
        MapChanged();
    }
    if (KeyIsDown(33) && pKey0) {
        gHeadup_map_x_float = (float)((double)gHeadup_map_x_float - (double)gFrame_period / 20.0);
        if (gHeadup_map_x_float < 0.0f) {
            gHeadup_map_x_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(34) && pKey0) {
        gHeadup_map_x_float = (float)((double)gHeadup_map_x_float + (double)gFrame_period / 20.0);
        if (gHeadup_map_x_float + gHeadup_map_w_float + 8.0f > (float)gBack_screen->width) {
            gHeadup_map_x_float = (float)(gBack_screen->width - gHeadup_map_w - 8);
        }
        MapChanged();
    }
}

// FUNCTION: CARMA2_HW 0x004977e0
void C2_HOOK_FASTCALL InitSmashTargets(void) {
    gCount_smashable_race_targets = 0;
}

// FUNCTION: CARMA2_HW 0x004977f0
void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown) {
    int index;

    index = gCount_smashable_race_targets;
    if (index < (int)REC2_ASIZE(gSmashable_race_targets)) {
        gSmashable_race_targets[index].model = pModel;
        gSmashable_race_targets[index].actor = pActor;
        gSmashable_race_targets[index].field_0x8 = pUnknown;
        gCount_smashable_race_targets = index + 1;
    }
}

int NearestPowerOfTwo(int pValue) {
    int power;

    for (power = 1; power < 32; power++) {
        if (pValue <= 1 << power) {
            return 1 << power;
        }
    }
    return INT_MIN;
}

