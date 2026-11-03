/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "53-controls.h"

#include "00-car.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "27-powerup.h"
#include "24-loadsave.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <stdio.h>

// GLOBAL: CARMA2_HW 0x0067c478
tU32 gPalette_fade_time;

// GLOBAL: CARMA2_HW 0x00659b28
// GLOBAL: CARMA2_SW 0x00654510
int gCheckpoint_finder_enabled = 1;

// GLOBAL: CARMA2_HW 0x00659b2c
// GLOBAL: CARMA2_SW 0x00654514
float gMap_render_x = 80.f;

// GLOBAL: CARMA2_HW 0x00659b30
// GLOBAL: CARMA2_SW 0x00654518
float gMap_render_y = 6.f;

// GLOBAL: CARMA2_HW 0x00659b34
// GLOBAL: CARMA2_SW 0x0065451c
float gMap_render_width = 128.f;

// GLOBAL: CARMA2_HW 0x00659b38
// GLOBAL: CARMA2_SW 0x00654520
float gMap_render_height = 80.f;

// GLOBAL: CARMA2_HW 0x0068d8d4
// GLOBAL: CARMA2_SW 0x0068792c
int gMap_trans;

// GLOBAL: CARMA2_HW 0x0074abac
// GLOBAL: CARMA2_SW 0x00741258
int gHeadup_map_x;

// GLOBAL: CARMA2_HW 0x0074abb0
// GLOBAL: CARMA2_SW 0x0074125c
int gHeadup_map_y;

// GLOBAL: CARMA2_HW 0x0074aba8
// GLOBAL: CARMA2_SW 0x00741254
int gHeadup_map_w;

// GLOBAL: CARMA2_HW 0x0074abb4
// GLOBAL: CARMA2_SW 0x00741260
int gHeadup_map_h;

// GLOBAL: CARMA2_HW 0x0067c400
// GLOBAL: CARMA2_SW 0x00676b78
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c3c8
// GLOBAL: CARMA2_SW 0x00676b40
char* gAbuse_text[10];

// GLOBAL: CARMA2_HW 0x0067c3f8
// GLOBAL: CARMA2_SW 0x00676b70
int gRecovery_voucher_count;

// GLOBAL: CARMA2_HW 0x0067c46c
// GLOBAL: CARMA2_SW 0x00676be4
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x0067c474
// GLOBAL: CARMA2_SW 0x00676bec
int gEntering_message;

// GLOBAL: CARMA2_HW 0x006b781c
// GLOBAL: CARMA2_SW 0x006ae4fc
tRace_over_reason gRace_over_reason;

void C2_HOOK_FASTCALL CompleteRace(void) {
    gRace_over_reason = eRace_over_0;
    gAbandon_game = 2;
}

// FUNCTION: CARMA2_HW 0x00441490
// FUNCTION: CARMA2_SW 0x00441570
int C2_HOOK_FASTCALL AbortRace(tFrontend_spec* pFrontend) {

    if (!gRace_finished) {
        gAbandon_game = 1;
    }
    return gRace_finished;
}

// F4Key

// SetFlag

// FUNCTION: CARMA2_HW 0x004415c0
// FUNCTION: CARMA2_SW 0x004416a0
void C2_HOOK_FASTCALL FinishRace(int pArg) {
    (void)pArg;
    CompleteRace();
}

// FUNCTION: CARMA2_HW 0x004415e0
// FUNCTION: CARMA2_SW 0x004416c0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

}

// FUNCTION: CARMA2_HW 0x004415f0
// FUNCTION: CARMA2_SW 0x004416d0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

}

// DoEditModeKey

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

// FUNCTION: CARMA2_HW 0x00441900
// FUNCTION: CARMA2_SW 0x004419e0
void C2_HOOK_FASTCALL F12Key(void) {
}

// NumberKey0

// NumberKey1

// NumberKey2

// NumberKey3

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

// FUNCTION: CARMA2_HW 0x00441e10
// FUNCTION: CARMA2_SW 0x00441ef0
void C2_HOOK_FASTCALL LookLeft(void) {

    PratcamEvent(27);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_left) {
        LookForward();
    } else if (gProgram_state.which_view == eView_right) {
        LookForward();
        gProgram_state.pending_view = eView_left;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_left;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441ec0
// FUNCTION: CARMA2_SW 0x00441fa0
void C2_HOOK_FASTCALL LookForward(void) {

    if (gProgram_state.which_view == eView_right) {
        PratcamEvent(27);
    } else if (gProgram_state.which_view == eView_left) {
        PratcamEvent(28);
    }
    if (gProgram_state.which_view != eView_forward) {
        gProgram_state.old_view = gProgram_state.which_view;
        ClearWobbles();
        gProgram_state.new_view = eView_forward;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441f20
// FUNCTION: CARMA2_SW 0x00442000
void C2_HOOK_FASTCALL LookRight(void) {

    PratcamEvent(28);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_right) {
        LookForward();
    } else if (gProgram_state.which_view == eView_left) {
        LookForward();
        gProgram_state.pending_view = eView_right;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_right;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441fd0
// FUNCTION: CARMA2_SW 0x004420b0
void C2_HOOK_FASTCALL DamageTest(void) {

}

// FUNCTION: CARMA2_HW 0x00441fe0
// FUNCTION: CARMA2_SW 0x004420c0
void C2_HOOK_FASTCALL TDamageEngine(void) {
    DamageEngine(21);
}

// FUNCTION: CARMA2_HW 0x00441ff0
// FUNCTION: CARMA2_SW 0x004420d0
void C2_HOOK_FASTCALL TDamageDriver(void) {
    if (gProgram_state.current_car.damage_units[eDamage_driver].damage_level < 80) {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 80 - gProgram_state.current_car.damage_units[eDamage_driver].damage_level);
    } else {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 2);
    }
}

// FUNCTION: CARMA2_HW 0x00442030
// FUNCTION: CARMA2_SW 0x00442110
void C2_HOOK_FASTCALL TDamageTrans(void) {
    DamageTrans(21);
}

// FUNCTION: CARMA2_HW 0x00442050
// FUNCTION: CARMA2_SW 0x00442130
void C2_HOOK_FASTCALL TDamageLFWheel(void) {

    DamageLFWheel(21);
}

// ToggleMiniMap

// CheckToggles

// CarWorldOffFallenCheckThingy

// HasCarFallenOffWorld

// CheckForBeingOutOfThisWorld

// CheckHornLocal

// CheckHorn3D

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

// CheckRecoverCost

// SortOutRecover

// and upstream controls.c (reference only)
extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004429b0
void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00442970
void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        DisableCar(pCar);
        gPalette_fade_time = GetRaceTime();
        NetPlayerStatusChanged(ePlayer_status_recovering);
    } else {
        FlipUpCar(pCar);
    }
}


// FUNCTION: CARMA2_HW 0x00442e10
// FUNCTION: CARMA2_SW 0x00442ef0
void C2_HOOK_FASTCALL AlignChildren(tPhysics_object* pCollision_info) {
    tPhysics_object* child;

    for (child = pCollision_info->child; child != NULL; child = child->next) {
        int i;
        int j;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                child->transform_matrix.m[i][j] = pCollision_info->transform_matrix.m[i][j];
            }
        }
        BrVector3SetFloat(&child->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&child->omega, 0.f, 0.f, 0.f);
        AlignChildren(child);
    }
}

// FlipUpCar

// FUNCTION: CARMA2_HW 0x00442e80
// FUNCTION: CARMA2_SW 0x00442f60
void C2_HOOK_FASTCALL GetPowerup(int pNum) {

    GotPowerup(&gProgram_state.current_car, pNum);
}

// CheckSystemKeys

// CheckForCheatingGits

// CheckKevKeys

// BrakeInstantly

// PollCarControls

// PollCameraControls

// FUNCTION: CARMA2_HW 0x00444350
// FUNCTION: CARMA2_SW 0x00444430
void C2_HOOK_FASTCALL SetFlag2(int pArg) {
    (void)pArg;
    gAllow_car_flying = 1;
    ToggleFlying();
    gAllow_car_flying = gCar_flying;
}

// FUNCTION: CARMA2_HW 0x004443c0
// FUNCTION: CARMA2_SW 0x004444a0
void C2_HOOK_FASTCALL ToggleFlying(void) {
    if (gAllow_car_flying && gNet_mode == eNet_mode_none) {
        gCar_flying = !gCar_flying;
        if (gCar_flying) {
            NewTextHeadupSlot(4, 0, 500, -4, "We have lift off!!");
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, "Back down to Earth");
        }
    } else {
        gCar_flying = 0;
    }
}

// CycleInvulnerability

// FUNCTION: CARMA2_HW 0x00444590
void C2_HOOK_FASTCALL ToggleTimerFreeze(void) {

    gFreeze_timer = !gFreeze_timer;
    if (gFreeze_timer) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_timer_frozen));
    } else {
        NewTextHeadupSlot(4, 0, 1000, -4, "Timer thawed out");
    }
}

// FUNCTION: CARMA2_HW 0x004445e0
// FUNCTION: CARMA2_SW 0x004446c0
void C2_HOOK_FASTCALL EarnDosh(void) {

    EarnCredits(5000);
}

// FUNCTION: CARMA2_HW 0x004445f0
// FUNCTION: CARMA2_SW 0x004446d0
void C2_HOOK_FASTCALL LoseDosh(void) {
    EarnCredits(-5000);
}

// FUNCTION: CARMA2_HW 0x00444600
void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg) {

}

// HackyMapFixForMac3dfx

// ToggleMap

// FUNCTION: CARMA2_HW 0x004446f0
// FUNCTION: CARMA2_SW 0x004447d0
int C2_HOOK_FASTCALL GetRecoverVoucherCount(void) {

    return gRecovery_voucher_count;
}

// FUNCTION: CARMA2_HW 0x00444700
// FUNCTION: CARMA2_SW 0x004447e0
void C2_HOOK_FASTCALL AddVouchers(int pCount) {

    gRecovery_voucher_count += pCount;
}

// FUNCTION: CARMA2_HW 0x00444710
// FUNCTION: CARMA2_SW 0x004447f0
void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {

    gRecovery_voucher_count = 0;
}

// FUNCTION: CARMA2_HW 0x00444720
// FUNCTION: CARMA2_SW 0x00444800
void C2_HOOK_FASTCALL CycleYonFactor(void) {
    float factor;

    factor = 0.5f * GetYonFactor();
    if (factor < 0.1f) {
        factor = 1.0f;
    }
    SetYonFactor(factor);
    if (factor > 0.75f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_quickly));
    } else if (factor > 0.375f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_quickly));
    } else if (factor > 0.187f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_late));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_late));
    }
}

// FUNCTION: CARMA2_HW 0x004447c0
// FUNCTION: CARMA2_SW 0x004448a0
void C2_HOOK_FASTCALL CycleSoundDetailLevel(void) {
    int new_level;

    new_level = (gSound_detail_level + 1) % 3;
    ReallySetSoundDetailLevel(new_level);
    SetSoundDetailLevel(new_level);
    switch (gSound_detail_level) {
    case 0:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_fewest_sounds));
        break;
    case 1:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_partial_sound));
        break;
    case 2:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_all_sounds));
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00444820
// FUNCTION: CARMA2_SW 0x00444900
void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void) {
    const char* src;
    char* dst;

    gCar_simplification_level++;
    gCar_simplification_level = gCar_simplification_level % 5u;
    src = GetMiscString(eMiscString_car_simplification_level_D);
    dst = BrMemAllocate(strlen(src) + 21, kMem_simp_level);
    sprintf(dst, src, gCar_simplification_level);
    NewTextHeadupSlot(4, 0, 2000, -4, dst);
    BrMemFree(dst);
}

// FUNCTION: CARMA2_HW 0x004448a0
// FUNCTION: CARMA2_SW 0x00444980
void C2_HOOK_FASTCALL ToggleAccessoryRendering(void) {
    int on;

    if (gNet_mode != eNet_mode_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_net_games_always_accessorized));
    } else {
        on = !GetAccessoryRendering();
        SetAccessoryRendering(on);
        if (on) {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_on));
        } else {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_off));
        }
    }
}

// FUNCTION: CARMA2_HW 0x004448f0
// FUNCTION: CARMA2_SW 0x004449d0
void C2_HOOK_FASTCALL UserSendMessage(void) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, options, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_options, enable_text_messages, 0x4);

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->options.enable_text_messages) {
        gEntering_message = 1;
    }
}

// EnterUserMessage

// DisplayUserMessage

// FUNCTION: CARMA2_HW 0x00444d70
// FUNCTION: CARMA2_SW 0x00444e50
void C2_HOOK_FASTCALL InitAbuseomatic(void) {
    char path[256];
    char s[256];
    FILE* f;
    int i;
    int len;

    gString[20] = '\0';
    PDBuildAppPath(path);
    strcat(path, "ABUSE.TXT");
    for (i = 0; i < REC2_ASIZE(gAbuse_text); i++) {
        gAbuse_text[i] = NULL;
    }
    f = PFfopen(path, "rt");
    if (f == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (PFfgets(s, REC2_ASIZE(s) - 1, f) == NULL) {
            break;
        }
        // len = strlen(s);
        if (strlen(s) > 63) {
            s[63] = '\0';
        }
        len = strlen(s);
        while (len != 0 && s[len - 1] < ' ') {
            s[--len] = '\0';
        }
        gAbuse_text[i] = BrMemAllocate(strlen(s) + 1, kMem_abuse_text);
        strcpy(gAbuse_text[i], s);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00444ea0
// FUNCTION: CARMA2_SW 0x00444f80
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

// ChangeCameraTypeInGame

// SteelBollock

// FUNCTION: CARMA2_HW 0x00444f10
void C2_HOOK_FASTCALL RichardsSphere(int pNum) {

    TotallyRepairACar(gCar_to_view);
    SphericizeCar(gCar_to_view);
    NewTextHeadupSlot(4, 0, 2000, -4, "STEEL GONAD O' DEATH");
}

// ToggleHeadupLevel


// FUNCTION: CARMA2_HW 0x00444fa0
void C2_HOOK_FASTCALL ToggleShellModel(void) {

    gCar_to_view->use_shell_model = !gCar_to_view->use_shell_model;
    if (gCar_to_view->use_shell_model) {
        NewTextHeadupSlot(4, 0, 2000, -4, "SHELL MODEL");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "COMPLEX MODELS");
    }
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0045a920
int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3) {
    /* inert stub (inserts one text into a headup text buffer's linked list) */
    (void)pBuffer;
    (void)pText;
    (void)pArg3;
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0045a7f0
int C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2) {
    extern void* C2_HOOK_CDECL malloc(unsigned int pSize);
    extern int C2_HOOK_CDECL toupper(int pChar);
    extern void C2_HOOK_FASTCALL DRstrupr(char* s);
    extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3);
    char local_100[256];
    char* substr;
    void* this_ptr;
    tHeadup_text* mem;

    this_ptr = (void*)pArg2[0];
    substr = (char*)pArg2[1];
    strcpy(local_100, pFilename);
    DRstrupr(local_100);
    if (strstr(local_100, "POWERUP") == NULL && strstr(local_100, "CPOINT") == NULL && strstr(local_100, "TRACKX") == NULL) {
        if (strstr(local_100, substr) == NULL) {
            if (strcmp(substr, ".*") != 0) {
                return 0;
            }
        }
        mem = malloc(sizeof(tHeadup_text));
        if (mem != NULL) {
            mem->next = NULL;
            strncpy(mem->text, local_100, 0x28);
            DRstrlwr(mem->text);
            mem->text[0] = (char)toupper((int)mem->text[0]);
            FUN_0045a920(this_ptr, mem, 1);
        }
    }
    return 0;
}


typedef struct tJoystick_headup_text {
    const char* text;
    int key;
} tJoystick_headup_text;

// GLOBAL: CARMA2_HW 0x00596250
tJoystick_headup_text gJoystick_headup_texts[21] = {
    { "Recover", 60 },
    { "Reapir", 45 },
    { "Hand Brake", 54 },
    { "Use Powerup", 61 },
    { "Next Powerup", 64 },
    { "Prev Powerup", 63 },
    { "Wheel Spin", 56 },
    { "Map", 57 },
    { "Cockpit View", 46 },
    { "Buy Armour", 67 },
    { "Buy Power", 68 },
    { "Buy Offensive", 69 },
    { "Target Lock", 72 },
    { "Cycle Target", 73 },
    { "Camera Mode", 75 },
    { "Doors", 74 },
    { "Horn", 59 },
    { "Accelerate", 49 },
    { "Brake", 50 },
    { "Left", 47 },
    { "Right", 48 },
};

// GLOBAL: CARMA2_HW 0x005962f8
br_token_value gAccent_poly_prims[3] = {
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { 0x4b0000 } },
    { BR_NULL_TOKEN, { 0 } },
};

// GLOBAL: CARMA2_HW 0x00686174
br_actor* gAcc_poly_actor;

// GLOBAL: CARMA2_HW 0x00686178
br_material* gAcc_poly_material;

// GLOBAL: CARMA2_HW 0x0068617c
br_model* gAcc_poly_model;

// GLOBAL: CARMA2_HW 0x00686180
int gRender_acc_poly_actor;

// GLOBAL: CARMA2_HW 0x0079d8a0
tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;

// FUNCTION: CARMA2_HW 0x0045c240
void C2_HOOK_FASTCALL FUN_0045c240(void) {
    extern void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);
    extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3);
    extern void* C2_HOOK_CDECL malloc(unsigned int pSize);
    extern tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;
    tJoystick_headup_text* entry;
    tHeadup_text* text;
    int i;

    if (gINT_0068b8e8) {
        gINT_0068b8e8 = 0;
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor != 0) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
    } else {
        gINT_0068b8e8 = 1;
        MoveHeadupTo(gProgram_state.frame_rate_headup, 10, 350);
        for (i = 0; i < 22; i++) {
            MoveHeadupTo(gRace_head_ups[i], 10, 120 + i * 10);
            ChangeHeadupFont(gRace_head_ups[i], -2);
        }
        CreateAccentPolyActor(8.f, 118.f, 130.f, 224.f);
        if (gJoystick_headup_buffer_0079d8a0.count == 0) {
            if (gJoystick_headup_buffer_0079d8a0.field_0x14 < 5) {
                gJoystick_headup_buffer_0079d8a0.field_0x14 = 10;
            }
            gJoystick_headup_buffer_0079d8a0.texts = NULL;
            gJoystick_headup_buffer_0079d8a0.count = 0;
            gJoystick_headup_buffer_0079d8a0.pos_last_add = 0;
            gJoystick_headup_buffer_0079d8a0.text_last_add = NULL;
            gJoystick_headup_buffer_0079d8a0.field_0x4 = NULL;
            gJoystick_headup_buffer_0079d8a0.field_0x10 = 22;
            entry = gJoystick_headup_texts;
            do {
                text = malloc(sizeof(tHeadup_text));
                if (text != NULL) {
                    text->next = NULL;
                    strncpy(text->text, entry->text, 0x28);
                    FUN_0045a920(&gJoystick_headup_buffer_0079d8a0, text, 0);
                }
                entry++;
            } while ((int)entry < (int)gAccent_poly_prims);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045aa60
void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight) {
#ifdef REC2_HW
    extern br_actor* g2d_camera;
#endif

    if (gAcc_poly_material == NULL) {
        gAcc_poly_material = BrMaterialAllocate("Acc Poly Mat");
        gAcc_poly_material->colour = 0x80;
        gAcc_poly_material->flags = BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gAcc_poly_material);
        gAcc_poly_material->extra_prim = gAccent_poly_prims;
        gAcc_poly_material->extra_prim[1].v.x = 0x800000;
        BrMaterialUpdate(gAcc_poly_material, BR_MATU_EXTRA_PRIM);
    }
    if (gAcc_poly_actor == NULL) {
        gAcc_poly_model = BrModelAllocate("Tint Poly", 6, 2);
        BrVector3Set(&gAcc_poly_model->vertices[0].p, pX, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[1].p, pX + pWidth, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[2].p, pX, -(pY + pHeight), -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[3].p, pX + pWidth, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[4].p, pX + pWidth, -(pY + pHeight), -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[5].p, pX, -(pY + pHeight), -1.01f);
        gAcc_poly_model->faces[0].vertices[0] = 0;
        gAcc_poly_model->faces[0].vertices[1] = 1;
        gAcc_poly_model->faces[0].vertices[2] = 2;
        gAcc_poly_model->faces[1].vertices[0] = 3;
        gAcc_poly_model->faces[1].vertices[1] = 4;
        gAcc_poly_model->faces[1].vertices[2] = 5;
        gAcc_poly_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gAcc_poly_actor->material = gAcc_poly_material;
        gAcc_poly_model->faces[0].material = gAcc_poly_material;
        gAcc_poly_model->faces[1].material = gAcc_poly_material;
        gAcc_poly_actor->model = gAcc_poly_model;
        BrModelAdd(gAcc_poly_model);
    }
#ifdef REC2_HW
    if (!gRender_acc_poly_actor) {
        gRender_acc_poly_actor = 1;
        BrActorAdd(g2d_camera, gAcc_poly_actor);
    }
#endif
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0045acf0
void C2_HOOK_FASTCALL FUN_0045acf0(void) {
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00511200
void C2_HOOK_FASTCALL FUN_00511200(void) {
    extern void C2_HOOK_FASTCALL FUN_0045acf0(void);

    FUN_0045acf0();
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0045a670
void C2_HOOK_FASTCALL FUN_0045a670(void) {
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00511210
void C2_HOOK_FASTCALL FUN_00511210(void) {
    extern void C2_HOOK_FASTCALL FUN_0045a670(void);

    FUN_0045a670();
}
