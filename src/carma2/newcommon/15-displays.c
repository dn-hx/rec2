#include "15-displays.h"

#include "08-loading1.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"
#include "globvrpb.h"

// GLOBAL: CARMA2_HW 0x0067c500
tHeadup gHeadups[37];

// GLOBAL: CARMA2_HW 0x0067fcd8
br_font* gBR_fonts[4];

// GLOBAL: CARMA2_HW 0x0067fd00
br_pixelmap* gRevs[13];

// GLOBAL: CARMA2_HW 0x0074cf84
br_pixelmap* gArmour_colour_map;

// GLOBAL: CARMA2_HW 0x0067fcd0
br_pixelmap* gPowerbar;

// GLOBAL: CARMA2_HW 0x0074cab4
br_pixelmap* gPower_colour_map;

// GLOBAL: CARMA2_HW 0x0074cf5c
br_pixelmap* gOffensive_colour_map;

// GLOBAL: CARMA2_HW 0x006815bc
br_pixelmap* gIcon_test;

// GLOBAL: CARMA2_HW 0x0074ca1c
br_pixelmap* gCurrent_rev;

// GLOBAL: CARMA2_HW 0x0067c4f8
br_pixelmap* gHud_curve1;

// GLOBAL: CARMA2_HW 0x0067fcfc
br_pixelmap* gHud_curve2;

// GLOBAL: CARMA2_HW 0x0067f87c
br_pixelmap* gHud_curve3;

// GLOBAL: CARMA2_HW 0x0067fcf4
br_pixelmap* gGrey_top5;

// GLOBAL: CARMA2_HW 0x0074cf00
br_pixelmap* gDamage_hud;

// GLOBAL: CARMA2_HW 0x0074ca80
br_pixelmap* gTimer_lhs;

// GLOBAL: CARMA2_HW 0x0074ca44
br_pixelmap* gHud_timer_rhs;

// GLOBAL: CARMA2_HW 0x006815b8
br_pixelmap* gIcon_armour;

// GLOBAL: CARMA2_HW 0x006815b4
br_pixelmap* gIcon_power;

// GLOBAL: CARMA2_HW 0x0067c4f0
br_pixelmap* gIcon_offense;

// GLOBAL: CARMA2_HW 0x0067c4f4
br_pixelmap* gIcon_greyBloc1;

// GLOBAL: CARMA2_HW 0x0067fce8
br_pixelmap* gIcon_litBloc1;

// GLOBAL: CARMA2_HW 0x0067fcd4
br_pixelmap* gIcon_grnBlock1;

// GLOBAL: CARMA2_HW 0x0067fd40
tHud_message gHud_messages[46];

// GLOBAL: CARMA2_HW 0x00655e54
int gHeadup_detail_level = 5;

// GLOBAL: CARMA2_HW 0x0067fcc0
tU32 gLast_fancy_headup;

// GLOBAL: CARMA2_HW 0x005913d4
int gLast_fancy_index = -1;

// GLOBAL: CARMA2_HW 0x0067fd34
tU32 gLast_fancy_time;

// GLOBAL: CARMA2_HW 0x0067fcf8
tU32 gLast_centre_headup;

// GLOBAL: CARMA2_HW 0x005913d8
int gLast_credit_headup__displays = -1;

// GLOBAL: CARMA2_HW 0x0067f874
tU32 gLast_earn_time;

// GLOBAL: CARMA2_HW 0x0067f878
int gLast_credit_amount;

// GLOBAL: CARMA2_HW 0x005913dc
int gLast_time_credit_headup = -1;

// GLOBAL: CARMA2_HW 0x005913e0
int gPS_apo_level_changed[3] = { 1, 1, 1 };

// GLOBAL: CARMA2_HW 0x005913ec
int gPrev_ps_apo_levels[3] = { -1, -1, -1 };

// GLOBAL: CARMA2_HW 0x005913f8
int gPrev_ps_drawn_levels[3] = { -1, -1, -1 };

// GLOBAL: CARMA2_HW 0x0067fcc4
tU32 gLast_time_earn_time;

// GLOBAL: CARMA2_HW 0x0079eac0
int gOld_times[10];

// GLOBAL: CARMA2_HW 0x0067f888
int gQueued_headup_count;

// GLOBAL: CARMA2_HW 0x0067f880
int gLast_time_credit_amount;


static void C2_HOOK_FASTCALL LoadHeadupMessageFile(void) {
    FILE* f;
    tPath_name path;
    int i;
    int count;

    PathCat(path, gApplication_path, "SCROLLERS.TXT");
    f = DRfopen(path, "rt");
    if (f == NULL) {
        BrFailure("Couldn't open ", path);
    }
    /*
     * # of Headup scrolly messages
     * Format:  Font index, Message string
     * N.B use underscore "_" where you want a space in the string
     **/
    count = GetAnInt(f);

    C2_HOOK_BUG_ON(sizeof(tHud_message) != 0x88);
    C2_HOOK_BUG_ON(REC2_ASIZE(gHud_messages) != 46);

    if (count > (int)REC2_ASIZE(gHud_messages)) {
        BrFailure("Error - Too many Headup messages ", path);
    }
    for (i = 0; i < count; i++) {
        char s[256];
        char *str;
        tHud_message *hud_message;
        int len;
        int j;

        hud_message = &gHud_messages[i];
        GetALineAndDontArgue(f, s);

        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &hud_message->font1);

        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &hud_message->font2);

        str = strtok(NULL, "\t ,/");
        strcpy(hud_message->message, str);

        len = strlen(hud_message->message);
        for (j = 0; j < len; j++) {
            if (hud_message->message[j] == '_') {
                hud_message->message[j] = ' ';
            }
        }
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00449090
void C2_HOOK_FASTCALL InitHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++) {
        gHeadups[i].type = eHeadup_unused;
    }

    gBR_fonts[0] = BrFontProp4x6;
    gBR_fonts[1] = BrFontProp7x9;
    gBR_fonts[2] = gFont_7;
    gBR_fonts[3] = gHeadup_font;
    gRevs[ 0] = LoadPixelmap("rev1.tif");
    gRevs[ 1] = LoadPixelmap("rev2.tif");
    gRevs[ 2] = LoadPixelmap("rev3.tif");
    gRevs[ 3] = LoadPixelmap("rev4.tif");
    gRevs[ 4] = LoadPixelmap("rev5.tif");
    gRevs[ 5] = LoadPixelmap("rev6.tif");
    gRevs[ 6] = LoadPixelmap("rev7.tif");
    gRevs[ 7] = LoadPixelmap("rev8.tif");
    gRevs[ 8] = LoadPixelmap("rev9.tif");
    gRevs[ 9] = LoadPixelmap("rev10.tif");
    gRevs[10] = LoadPixelmap("rev11.tif");
    gRevs[11] = LoadPixelmap("rev12.tif");
    gRevs[12] = LoadPixelmap("rev13.tif");
    gPowerbar = LoadPixelmap("powerbar.tif");

    gArmour_colour_map = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        gPowerbar->width, gPowerbar->height,
        NULL, 0);
    gArmour_colour_map->identifier = BrResStrDup(gArmour_colour_map, "Armour_area");
    BrMapAdd(gArmour_colour_map);

    gPower_colour_map = LoadPixelmap("powerbar.tif");
    BrMapAdd(gPower_colour_map);

    gOffensive_colour_map = LoadPixelmap("powerbar.tif");
    BrMapAdd(gOffensive_colour_map);

    gIcon_test = LoadPixelmap("icontest.tif");

    gCurrent_rev = gRevs[0];
    BrMapAdd(gRevs[0]);

    gHud_curve1 = LoadPixelmap("hudcurve1.tif");
    BrMapAdd(gHud_curve1);
    gHud_curve2 = LoadPixelmap("hudcurve2.tif");
    BrMapAdd(gHud_curve2);
    gHud_curve3 = LoadPixelmap("hudcurve3.tif");
    BrMapAdd(gHud_curve3);

    gGrey_top5 = LoadPixelmap("greytop5.tif");
    gDamage_hud = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        gGrey_top5->width, gGrey_top5->height,
        NULL, 0);
    BrMapAdd(gDamage_hud);
    BrMapUpdate(gDamage_hud, BR_MAPU_ALL);

    gStatbarHUD5_material->colour_map = gDamage_hud;
    BrMaterialUpdate(gStatbarHUD5_material, BR_MATU_COLOURMAP);

    gTimer_lhs = LoadPixelmap("timerlhs.tif");
    BrMapAdd(gTimer_lhs);
    gTimerLeftHUD_material->colour_map = gTimer_lhs;
    BrMaterialUpdate(gTimerLeftHUD_material, BR_MATU_COLOURMAP);
    gTimerLeftHUD_model->vertices[1].p.v[0] = (float)gTimerLeftHUD_dim_x;
    gTimerLeftHUD_model->vertices[0].p.v[0] = gTimerLeftHUD_model->vertices[1].p.v[0];
    gTimerLeftHUD_model->vertices[3].p.v[1] = -(float)gTimerLeftHUD_dim_y;
    gTimerLeftHUD_model->vertices[0].p.v[1] = gTimerLeftHUD_model->vertices[3].p.v[1];
    gTimerLeftHUD_model->vertices[3].p.v[0] = (float)gTimerLeftHUD_dim_w + gTimerLeftHUD_model->vertices[1].p.v[0];
    gTimerLeftHUD_model->vertices[2].p.v[0] = gTimerLeftHUD_model->vertices[3].p.v[0];
    gTimerLeftHUD_model->vertices[2].p.v[1] = gTimerLeftHUD_model->vertices[3].p.v[1] - (float)gTimerLeftHUD_dim_h;
    gTimerLeftHUD_model->vertices[1].p.v[1] = gTimerLeftHUD_model->vertices[2].p.v[1];
    BrModelUpdate(gTimerLeftHUD_model, BR_MODU_VERTEX_POSITIONS);

    gHud_timer_rhs = LoadPixelmap("timerrhs.tif");
    BrMapAdd(gHud_timer_rhs);
    gTimerRightHUD_material->colour_map = gHud_timer_rhs;
    BrMaterialUpdate(gTimerRightHUD_material, BR_MATU_COLOURMAP);

    gTimerRightHUD_model->vertices[1].p.v[0] = (float)gTimerRightHUD_dim_x;
    gTimerRightHUD_model->vertices[0].p.v[0] = gTimerRightHUD_model->vertices[1].p.v[0];
    gTimerRightHUD_model->vertices[3].p.v[1] = -(float)gTimerRightHUD_dim_y;
    gTimerRightHUD_model->vertices[0].p.v[1] = gTimerRightHUD_model->vertices[3].p.v[1];
    gTimerRightHUD_model->vertices[3].p.v[0] = (float)gTimerRightHUD_dim_w + gTimerRightHUD_model->vertices[1].p.v[0];
    gTimerRightHUD_model->vertices[2].p.v[0] = gTimerRightHUD_model->vertices[3].p.v[0];
    gTimerRightHUD_model->vertices[2].p.v[1] = gTimerRightHUD_model->vertices[3].p.v[1] - (float)gTimerRightHUD_dim_h;
    gTimerRightHUD_model->vertices[1].p.v[1] = gTimerRightHUD_model->vertices[2].p.v[1];
    BrModelUpdate(gTimerRightHUD_model, BR_MODU_VERTEX_POSITIONS);

    gIcon_armour = LoadPixelmap("iconarmour.tif");
    gIcon_power = LoadPixelmap("iconpower.tif");
    gIcon_offense = LoadPixelmap("iconoffense.tif");
    gIcon_greyBloc1 = LoadPixelmap("greybloc1.tif");
    gIcon_litBloc1 = LoadPixelmap("litbloc1.tif");
    gIcon_grnBlock1 = LoadPixelmap("grnblock1.tif");

    LoadHeadupMessageFile();
}

// FUNCTION: CARMA2_HW 0x00449630
void C2_HOOK_FASTCALL ClearHeadup(int pIndex) {
    gHeadups[pIndex].type = eHeadup_unused;
}

// FUNCTION: CARMA2_HW 0x00449650
void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index) {
    int i;
    tHeadup* the_headup;

    the_headup = gHeadups;
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++, the_headup++) {
        if (the_headup->type != eHeadup_unused && the_headup->slot_index == pSlot_index) {
            ClearHeadup(i);
            return;
        }
    }
}
// FUNCTION: CARMA2_HW 0x00449690
void C2_HOOK_FASTCALL ClearHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);
    C2_HOOK_BUG_ON(REC2_ASIZE(gOld_times) != 10);

    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++) {
        if (gHeadups[i].type != eHeadup_unused) {
            ClearHeadup(i);
        }
    }
    gLast_fancy_index = -1;
    gLast_credit_headup__displays = -1;
    gLast_time_credit_headup = -1;
    gLast_earn_time = 0;
    for (i = 0; i < (int)REC2_ASIZE(gOld_times); i++) {
        gOld_times[i] = 0;
    }
    gLast_fancy_time = 0;
    gLast_time_earn_time = 0;
    gQueued_headup_count = 0;
    gLast_centre_headup = 0;
}

// FUNCTION: CARMA2_HW 0x004496f0
// FUNCTION: CARMA2_SW 0x00449840
void C2_HOOK_FASTCALL DimAFewBits(void) {
    int i;

#define DIM_INDEX (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0)
    for (i = 0; i < gProgram_state.current_car.dim_count[DIM_INDEX]; i++) {
        DimRectangle(
            gBack_screen,
            gProgram_state.current_car.dim_left[DIM_INDEX][i],
            gProgram_state.current_car.dim_top[DIM_INDEX][i],
            gProgram_state.current_car.dim_right[DIM_INDEX][i],
            gProgram_state.current_car.dim_bottom[DIM_INDEX][i],
            1);
    }
#undef DIM_INDEX
}

// KillOldestQueuedHeadup

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on ApexWorm/rec2 824b76655
// FUNCTION: CARMA2_HW 0x00449830
void C2_HOOK_FASTCALL DoTestPowerHeadup(int pLevel, const char* pAPO_str) {
    int changed;
    br_pixelmap* dest_pm;
    br_pixelmap* icon;
    int count_slots;
    int level;
    int i;
    int j;
    int delta_x = 2;

    switch (pAPO_str[0]) {
    case 'A':
        dest_pm = gArmour_colour_map;
        count_slots = gProgram_state.current_car.power_up_slots[0];
        icon = gIcon_armour;
        changed = pLevel != gPrev_ps_apo_levels[0] || count_slots != gPrev_ps_drawn_levels[0];
        gPS_apo_level_changed[0] = changed;
        gPrev_ps_apo_levels[0] = pLevel;
        gPrev_ps_drawn_levels[0] = count_slots;
        break;
    case 'P':
        dest_pm = gPower_colour_map;
        count_slots = gProgram_state.current_car.power_up_slots[1];
        icon = gIcon_power;
        changed = pLevel != gPrev_ps_apo_levels[1] || count_slots != gPrev_ps_drawn_levels[1];
        gPS_apo_level_changed[1] = changed;
        gPrev_ps_apo_levels[1] = pLevel;
        gPrev_ps_drawn_levels[1] = count_slots;
        break;
    case 'O':
        dest_pm = gOffensive_colour_map;
        count_slots = gProgram_state.current_car.power_up_slots[2];
        icon = gIcon_offense;
        /* Offense redraws use the cached power slot count. */
        changed = pLevel != gPrev_ps_apo_levels[2] || count_slots != gPrev_ps_drawn_levels[1];
        gPS_apo_level_changed[2] = changed;
        gPrev_ps_drawn_levels[2] = count_slots;
        gPrev_ps_apo_levels[2] = pLevel;
        break;
    default:
        return;
    }

    if (changed) {
        DRPixelmapCopy(dest_pm, gPowerbar);
        DRPixelmapRectangleMaskedCopy(dest_pm, (br_int_16)delta_x, 0, icon, 0, 0,
            (br_int_16)icon->width, (br_int_16)icon->height);
        level = pLevel;
        count_slots -= pLevel;
        for (i = 0; i < 15; i += 5) {
            for (j = 0; j < 10; ++j) {
                if (level > 0) {
                    DRPixelmapRectangleMaskedCopy(
                        dest_pm,
                        (br_int_16)(j * gIcon_litBloc1->width + delta_x + icon->width + 2),
                        (br_int_16)i,
                        gIcon_litBloc1, 0, 0, (br_int_16)gIcon_litBloc1->width, (br_int_16)gIcon_litBloc1->height);
                    --level;
                } else if (count_slots > 0) {
                    DRPixelmapRectangleMaskedCopy(
                        dest_pm,
                        (br_int_16)(j * gIcon_grnBlock1->width + delta_x + icon->width + 2),
                        (br_int_16)i,
                        gIcon_grnBlock1, 0, 0, (br_int_16)gIcon_grnBlock1->width, (br_int_16)gIcon_grnBlock1->height);
                    --count_slots;
                } else {
                    DRPixelmapRectangleMaskedCopy(
                        dest_pm,
                        (br_int_16)(j * gIcon_greyBloc1->width + delta_x + icon->width + 2),
                        (br_int_16)i,
                        gIcon_greyBloc1, 0, 0, (br_int_16)gIcon_greyBloc1->width, (br_int_16)gIcon_greyBloc1->height);
                }
            }
            delta_x -= 2;
        }

        switch (pAPO_str[0]) {
        case 'A':
            BrMapUpdate(gArmour_colour_map, BR_MAPU_ALL);
            gArmour_material->colour_map = gArmour_colour_map;
            BrMaterialUpdate(gArmour_material, BR_MATU_COLOURMAP);
            break;
        case 'P':
            BrMapUpdate(gPower_colour_map, BR_MAPU_ALL);
            gPower_material->colour_map = gPower_colour_map;
            BrMaterialUpdate(gPower_material, BR_MATU_COLOURMAP);
            break;
        case 'O':
            BrMapUpdate(gOffensive_colour_map, BR_MAPU_ALL);
            gOffence_material->colour_map = gOffensive_colour_map;
            BrMaterialUpdate(gOffence_material, BR_MATU_COLOURMAP);
            break;
        }
    }
    gArmour_actor->render_style = BR_RSTYLE_FACES;
    gPower_actor->render_style = BR_RSTYLE_FACES;
    gOffense_actor->render_style = BR_RSTYLE_FACES;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00449800
void C2_HOOK_FASTCALL DoPSPowerupHeadups(void) {
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[0], "A");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[1], "P");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[2], "O");
}

// DoHeadups

// FUNCTION: CARMA2_HW 0x00449fa0
int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    the_headup = gHeadups;
    empty_one = -1;
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++, the_headup++) {
        if (pSlot_index >= 0 && the_headup->slot_index == pSlot_index) {
            return i;
        } else if (the_headup->type == eHeadup_unused) {
            empty_one = i;
        }
    }
    return empty_one;
}

// IsHeadupTextClever

static int HeadupTextIsClever(const char* pText) {
    char c;

    c = *pText;
    while (c != '\0') {
        pText++;
        if (c < 0) {
            return 1;
        }
        c = *pText;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0044a220
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {
    int width;

    width = 0;
    if (pHeadup->type == eHeadup_box_text) {
        return 0;
    } else if (pHeadup->type == eHeadup_coloured_text) {
        pHeadup->clever = HeadupTextIsClever(pHeadup->data.text_info.text);
        switch (pHeadup->justification) {
        case eJust_left:
            pHeadup->x = pHeadup->original_x;
            break;
        case eJust_right:
            if (pHeadup->clever) {
                width = DRTextCleverWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
                pHeadup->x = pHeadup->original_x - width;
            } else {
                width = DRTextWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
                pHeadup->x = pHeadup->original_x - width;
            }
            break;
        case eJust_centre:
            if (pHeadup->clever) {
                width = DRTextCleverWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
                pHeadup->x = pHeadup->original_x - width / 2;
            } else {
                width = DRTextWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
                pHeadup->x = pHeadup->original_x - width / 2;
            }
            break;
        }
    } else if (pHeadup->type == eHeadup_fancy) {
        return 0;
    } else {
        pHeadup->clever = 0;
        switch (pHeadup->justification) {
        case eJust_left:
            pHeadup->x = pHeadup->original_x;
            break;
        case eJust_right:
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width;
            break;
        case eJust_centre:
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width / 2;
            break;
        }
    }
    return width;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tQueued_headup gQueued_headups[4];

// FUNCTION: CARMA2_HW 0x00449fd0
int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it) {
    int index;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;
    tU32 time;

    time = PDGetTotalTime();
    if (pQueue_it && pSlot_index == 4 && time - gLast_centre_headup < 1000) {
        if (gQueued_headup_count == 4) {
            gQueued_headup_count = 3;
            memcpy(&gQueued_headups[0], &gQueued_headups[1], gQueued_headup_count * sizeof(tQueued_headup));
        }
        gQueued_headups[gQueued_headup_count].flash_rate = pFlash_rate;
        gQueued_headups[gQueued_headup_count].lifetime = pLifetime;
        gQueued_headups[gQueued_headup_count].font_index = pFont_index;
        strcpy(gQueued_headups[gQueued_headup_count].text, pText);
        gQueued_headup_count++;
        return -1;
    }
    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index < 0) {
        return index;
    }
    if (pSlot_index == 4) {
        gLast_centre_headup = time;
    }
    headup_slot = &gProgram_state.current_car.headup_slots[gProgram_state.cockpit_on][pSlot_index];
    the_headup = &gHeadups[index];
    the_headup->data.coloured_text_info.coloured_font = &gFonts[-pFont_index];
    if (pSlot_index == 4) {
        the_headup->type = eHeadup_box_text;
    } else {
        the_headup->type = eHeadup_coloured_text;
    }
    strcpy(the_headup->data.coloured_text_info.text, pText);
    the_headup->slot_index = pSlot_index;
    the_headup->justification = headup_slot->justification;
    if (pSlot_index >= 0) {
        the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
    } else {
        the_headup->cockpit_anchored = 0;
    }
    the_headup->field_0x3c = headup_slot->field_0x28;
    the_headup->dimmed_background = headup_slot->dimmed_background;
    the_headup->dim_left = headup_slot->dim_left;
    the_headup->dim_top = headup_slot->dim_top;
    the_headup->dim_right = headup_slot->dim_right;
    the_headup->dim_bottom = headup_slot->dim_bottom;
    the_headup->original_x = headup_slot->x;
    the_headup->right_edge = MungeHeadupWidth(the_headup) + the_headup->x;
    the_headup->y = headup_slot->y;
    if (pFlash_rate) {
        the_headup->flash_period = 1000 / pFlash_rate;
    } else {
        the_headup->flash_period = 0;
    }
    the_headup->last_flash = 0;
    the_headup->flash_state = 0;
    if (pLifetime) {
        the_headup->end_time = GetTotalTime() + pLifetime;
    } else {
        the_headup->end_time = 0;
    }
    return index;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0044a380
int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText) {
    return NewTextHeadupSlot2(pSlot_index, pFlash_rate, pLifetime, pFont_index, pText, 1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on ApexWorm/rec2 677697708
// FUNCTION: CARMA2_HW 0x0044a3a0
int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index) {
    int index;
    int width;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;

    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index >= 0) {
        headup_slot = &gProgram_state.current_car.headup_slots[gProgram_state.cockpit_on][pSlot_index];
        the_headup = &gHeadups[index];
        the_headup->type = eHeadup_image;
        the_headup->slot_index = pSlot_index;
        the_headup->justification = headup_slot->justification;
        if (pSlot_index >= 0) {
            the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
        } else {
            the_headup->cockpit_anchored = 0;
        }
        the_headup->dimmed_background = headup_slot->dimmed_background;
        the_headup->dim_left = headup_slot->dim_left;
        the_headup->dim_top = headup_slot->dim_top;
        the_headup->dim_right = headup_slot->dim_right;
        the_headup->dim_bottom = headup_slot->dim_bottom;
        the_headup->original_x = headup_slot->x;

        switch (headup_slot->justification) {
        case eJust_left:
            the_headup->x = the_headup->original_x;
            break;
        case eJust_right:
            width = DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            the_headup->x = the_headup->original_x + width;
            break;
        case eJust_centre:
            width = DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            the_headup->x = the_headup->original_x + width / 2;
            break;
        }
        the_headup->y = headup_slot->y;
        if (pFlash_rate != 0) {
            the_headup->flash_period = 1000 / pFlash_rate;
        } else {
            the_headup->flash_period = 0;
        }
        the_headup->last_flash = 0;
        the_headup->flash_state = 0;
        if (pLifetime != 0) {
            the_headup->end_time = GetTotalTime() + pLifetime;
        } else {
            the_headup->end_time = 0;
        }
        strcpy(the_headup->data.image_info.text, gHud_messages[pImage_index].message);
        the_headup->data.image_info.font_index = gHud_messages[pImage_index].font2;
        the_headup->data.image_info.font = &gFonts[gHud_messages[pImage_index].font1];
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.image_info.field_0x104 = 0;
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = -width;
            break;
        case eJust_right:
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x104 = width;
            the_headup->data.image_info.field_0x10c = 0;
            break;
        case eJust_centre:
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x104 = width / 2;
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = -(width / 2);
            break;
        }
    }
    return index;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0044a600
void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex) {
    tU32 the_time;
    int temp_ref;
    int center;
    int width;
    tHeadup* the_headup;

    center = gCurrent_graf_data->width / 2;
    the_time = GetTotalTime();
    if (gMap_view == 2) {
        return;
    }
    if (gLast_fancy_index >= 0 && the_time - gLast_fancy_time <= 2000 && gLast_fancy_index > pIndex) {
        return;
    }
    temp_ref = NewImageHeadupSlot(6, 0, 2000, pIndex + 10);
    if (temp_ref >= 0) {
        gLast_fancy_index = pIndex;
        gLast_fancy_headup = temp_ref;
        gLast_fancy_time = the_time;

        the_headup = &gHeadups[temp_ref];
        the_headup->type = eHeadup_fancy;
        the_headup->data.fancy_info.fancy_stage = 0;
        the_headup->data.fancy_info.field_0x108 = 0;
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.fancy_info.offset = center;
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = -width;
            break;
        case eJust_right:
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = center;
            the_headup->data.fancy_info.offset = center + width;
            break;
        case eJust_centre:
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.offset = center + width / 2;
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = -center - width / 2;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044a860
void C2_HOOK_FASTCALL MoveHeadupKeepOffset(int pHeadup_index, int pX, int pY) {
    tHeadup* the_headup;
    int delta_x;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        delta_x = the_headup->x - the_headup->original_x;
        the_headup->original_x = pX;
        the_headup->x = delta_x + pX;
        the_headup->y = pY;
    }
}

// FUNCTION: CARMA2_HW 0x0044a8a0
void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pNew_x, int pNew_y) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        the_headup->justification = eJust_left;
        the_headup->original_x = pNew_x;
        the_headup->x = pNew_x;
        the_headup->y = pNew_y;
    }
}

// FUNCTION: CARMA2_HW 0x0044a8d0
void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text) {
    tHeadup* the_headup;

    if (pHeadup_index < 0) {
        return;
    }
    the_headup = &gHeadups[pHeadup_index];
    strcpy(the_headup->data.text_info.text, pNew_text);
    MungeHeadupWidth(the_headup);
}

// FUNCTION: CARMA2_HW 0x0044a920
// FUNCTION: CARMA2_SW 0x0044aab0
void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont) {

    if (pHeadup_index >= 0) {
        gHeadups[pHeadup_index].data.image_info.font = &gFonts[-pFont];
    }
}

// FUNCTION: CARMA2_HW 0x0044a9d0
void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time) {
    int i;
    int y_pitch;
    int the_step;
    int the_wobble_x;
    int the_wobble_y;
    br_pixelmap* the_image;
    tDamage_unit* the_damage;

    if (&gProgram_state.current_car != gCar_to_view) {
        return;
    }
    if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
        if (gProgram_state.which_view != eView_forward) {
            return;
        }
        the_wobble_x = gScreen_wobble_x;
        the_wobble_y = gScreen_wobble_y;
    } else {
        the_wobble_x = gProgram_state.current_car.damage_x_offset;
        the_wobble_y = gProgram_state.current_car.damage_y_offset;
    }

    DRPixelmapCopy(gDamage_hud, gGrey_top5);
    for (i = 0; i < (int)REC2_ASIZE(gProgram_state.current_car.damage_units); ++i) {
        if (i != eDamage_driver) {
            the_damage = &gProgram_state.current_car.damage_units[i];
            the_image = the_damage->images;
            the_step = 5 * the_damage->damage_level / 100;
            y_pitch = (the_image->height / 2) / 5;
            DRPixelmapRectangleMaskedCopy(
                gDamage_hud,
                (br_int_16)(the_wobble_x + the_damage->x_coord),
                (br_int_16)(the_wobble_y + the_damage->y_coord),
                the_image,
                0,
                (br_int_16)(y_pitch * (2 * the_step + ((pThe_time / the_damage->periods[the_step]) & 1))),
                (br_int_16)the_image->width,
                (br_int_16)y_pitch);
        }
    }
}

// DoInstruments

// ChangingView

extern int gCredit_multiplier;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on ApexWorm/rec2 24233b73a
// FUNCTION: CARMA2_HW 0x0044b300
// FUNCTION: CARMA2_SW 0x0044b490
void C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text) {
    char s[256];
    int original_amount;
    tU32 the_time;

    if (gRace_finished) {
        return;
    }
    the_time = GetTotalTime();
    if (pAmount == 0) {
        return;
    }
    if (gCredit_multiplier != 0) {
        pAmount *= gCredit_multiplier;
    }
    original_amount = pAmount;
    if (gLast_credit_headup__displays >= 0 && the_time - gLast_earn_time < 2000) {
        pAmount += gLast_credit_amount;
    }
    gLast_credit_amount = pAmount;
    if (pAmount > 1) {
        sprintf(s, "%s%d %s", pPrefix_text, pAmount, GetMiscString(eMiscString_credits));
    } else if (pAmount > 0) {
        sprintf(s, "%s1 %s", pPrefix_text, GetMiscString(eMiscString_credit));
    } else if (pAmount < -1) {
        sprintf(s, "%s%s %d %s", GetMiscString(eMiscString_lost_credits_prefix), pPrefix_text, -pAmount, GetMiscString(eMiscString_credits));
    } else {
        sprintf(s, "%s%s 1 %s", pPrefix_text, GetMiscString(eMiscString_lost_credits_prefix), GetMiscString(eMiscString_credit));
    }
    gProgram_state.credits += original_amount;
    gLast_credit_headup__displays = NewTextHeadupSlot2(4, 0, 2000, -4, s, 1);
    gLast_earn_time = the_time;
}

// FUNCTION: CARMA2_HW 0x0044b440
// FUNCTION: CARMA2_SW 0x0044b5d0
void C2_HOOK_FASTCALL EarnCredits(int pAmount) {
    EarnCredits2(pAmount, "");
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0044b450
void C2_HOOK_FASTCALL EarnCredits100(int pAmount) {

    EarnCredits(pAmount - (pAmount + 50) % 100 + 50);
}

// FUNCTION: CARMA2_HW 0x0044b470
int C2_HOOK_FASTCALL SpendCredits(int pAmount) {
    int amount;

    amount = gProgram_state.credits - pAmount;
    gProgram_state.credits = amount;
    if (amount < 0) {
        gProgram_state.credits = 0;
        return amount;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044b490
void C2_HOOK_FASTCALL AwardTime(tU32 pTime) {
    char s[256];
    tU32 original_amount;
    tU32 the_time;
    int i;

    if (gRace_finished || gFreeze_timer || gNet_mode != eNet_mode_none || (*(tGraf_data**)((char*)&gCurrent_race + 0x98))->field_0x18c) {
        return;
    }
    the_time = GetTotalTime();
    if (pTime != 0) {
        for (i = 9; i > 0; i--) {
            gOld_times[i] = gOld_times[i - 1];
        }
        gOld_times[0] = pTime;
        original_amount = pTime;
        if (gLast_time_credit_headup >= 0 && (the_time - gLast_time_earn_time) < 2000) {
            pTime += gLast_time_credit_amount;
        }
        gLast_time_credit_amount = pTime;
        gTimer += original_amount * 1000;
        s[0] = '+';
        TimerString(1000 * pTime, &s[1], 0, 0, 0);
        gLast_time_credit_headup = NewTextHeadupSlot2(0xb, 0, 2000, -2, s, 1);
        gLast_time_earn_time = the_time;
    }
}

// AwardTime5

// GLOBAL: CARMA2_HW 0x0079eb00
char gHeadup_oppo_ped_text[64];

// FUNCTION: CARMA2_HW 0x0044b6a0
void C2_HOOK_FASTCALL DoTestHeadup(void) {
    // GLOBAL: CARMA2_HW 0x006815c0
    static int do_headup_material;

    do_headup_material = !do_headup_material;
    if (do_headup_material) {
        BrMapUpdate(gCurrent_rev, BR_MAPU_ALL);
        gStatbarHUD1_material->colour_map = gCurrent_rev;
        BrMaterialUpdate(gStatbarHUD1_material, BR_MATU_COLOURMAP);
    }
    if (!do_headup_material) {
        BrMapUpdate(gDamage_hud, BR_MAPU_ALL);
        gStatbarHUD5_material->colour_map = gDamage_hud;
        BrMaterialUpdate(gStatbarHUD5_material, BR_MATU_COLOURMAP);
    }

    if (gNet_mode != eNet_mode_none) {
        if (gHeadup_detail_level % 3 > 0) {
            BrMatrix34Translate(&gStatbarHUD5_actor->t.t.mat, -448.f, 0.f, 0.f);
            RenderThisHeadup(gStatbarHUD5_actor);
            RenderThisHeadup(gStatbarHUD1_actor);
            if (gHeadup_detail_level % 3 > 1) {
                BrMatrix34Translate(&gArmour_actor->t.t.mat, 48.f, 35.f, 0.f);
                BrMatrix34Translate(&gPower_actor->t.t.mat, -102.f, 5.f, 0.f);
                BrMatrix34Translate(&gOffense_actor->t.t.mat, -252.f, -25.f, 0.f);
                RenderThisHeadup(gArmour_actor);
                RenderThisHeadup(gPower_actor);
                RenderThisHeadup(gOffense_actor);
            }
        }
        if (gHeadup_detail_level > 2) {
            RenderThisHeadup(gHeadup_actor);
        }
    } else {
        BrMatrix34Translate(&gArmour_actor->t.t.mat, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gPower_actor->t.t.mat, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gOffense_actor->t.t.mat, 0.f, 0.f, 0.f);
        if (gHeadup_detail_level % 3 > 0) {
            BrMatrix34Translate(&gStatbarHUD1_actor->t.t.mat, 142.f, 0.f, 0.f);
            BrMatrix34Translate(&gStatbarHUD5_actor->t.t.mat, -178.f, 0.f, 0.f);
            RenderThisHeadup(gStatbarHUD1_actor);
            RenderThisHeadup(gStatbarHUD5_actor);
            RenderThisHeadup(gTimerLeftHUD_actor);
            RenderThisHeadup(gTimerRightHUD_actor);
            if (gHeadup_detail_level % 3 > 1) {
                RenderThisHeadup(gArmour_actor);
                RenderThisHeadup(gPower_actor);
                RenderThisHeadup(gOffense_actor);
                RenderThisHeadup(gStatbarRightHUD_actor);
                RenderThisHeadup(gStatbarHUD3_actor);
                BrMatrix34Translate(&gStatbarHUD1_actor->t.t.mat, 0.f, 0.f, 0.f);
                BrMatrix34Translate(&gStatbarHUD5_actor->t.t.mat, 0.f, 0.f, 0.f);
            } else {
                PolyFontText(gHeadup_oppo_ped_text, 335, 50, kPolyfont_ingame_medium_blue, eJust_centre, 0);
            }
        }
        if (gHeadup_detail_level > 2) {
            RenderThisHeadup(gHeadup_actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044ba60
int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth) {
    int width;

    if (gDevious_2d && gTexture_power_of_2) {
        width = 1;
        while (width < pWidth) {
            width <<= 1;
        }
        return width;
    }
    return pWidth;
}

// FUNCTION: CARMA2_HW 0x0044ba90
int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight) {
    int height;

    if (gDevious_2d && gTexture_power_of_2) {
        height = 1;
        while (height < pHeight) {
            height <<= 1;
        }
        return height;
    }
    return pHeight;
}

// FUNCTION: CARMA2_HW 0x0044bac0
void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY) {
    gCar_icons_model_actor->material->colour_map = gTexture_maps[gCar_icons[pCar_index].index];
    BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
    gCar_icons_model_actor->model = gCar_icons[pCar_index].model;
    BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat, (br_scalar)pX, (br_scalar)-pY, 0.f);
    BrActorAdd(g2d_camera, gCar_icons_model_actor);
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gCar_icons_model_actor);
}


// GLOBAL: CARMA2_HW 0x0067f890
tQueued_headup gQueued_headups[4];
