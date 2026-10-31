#include "53-controls.h"
#include "62-graphics3.h"

#include "15-displays.h"
#include "17-world2.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <stdio.h>
#include "69-sound.h"
#include "42-input.h"
#include "globvrpb.h"
#include "17-world2.h"
#include "globvrkm.h"
#include "16-graphics1.h"
#include "43-pratcam.h"
#include "37-brucetrk.h"
#include "07-structur.h"

// GLOBAL: CARMA2_HW 0x00659b2c
float gMap_render_x = 80.f;

// GLOBAL: CARMA2_HW 0x00659b30
float gMap_render_y = 6.f;

// GLOBAL: CARMA2_HW 0x00659b34
float gMap_render_width = 128.f;

// GLOBAL: CARMA2_HW 0x00659b38
float gMap_render_height = 80.f;

// GLOBAL: CARMA2_HW 0x0068d8d4
int gMap_trans;

// GLOBAL: CARMA2_HW 0x0074abac
int gHeadup_map_x;

// GLOBAL: CARMA2_HW 0x0074abb0
int gHeadup_map_y;

// GLOBAL: CARMA2_HW 0x0074aba8
int gHeadup_map_w;

// GLOBAL: CARMA2_HW 0x0074abb4
int gHeadup_map_h;

// GLOBAL: CARMA2_HW 0x0067c400
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c3bc
int gRecover_timer;

// GLOBAL: CARMA2_HW 0x0067c3c8
char* gAbuse_text[10] = { 0 };

/* UNCERTAIN: unknown, unreferenced 4 bytes after gAbuse_text (only seen as its loop end); keeps the layout adjacent */
// GLOBAL: CARMA2_HW 0x0067c3f0
int gUNK_0067c3f0 = 0;

// GLOBAL: CARMA2_HW 0x0067c3f8
int gRecovery_voucher_count;

// GLOBAL: CARMA2_HW 0x0067c460
int gHad_auto_recover;

// GLOBAL: CARMA2_HW 0x0067c46c
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x0067c470
int gINT_0067c470;

// GLOBAL: CARMA2_HW 0x0067c474
int gEntering_message;

// GLOBAL: CARMA2_HW 0x0067c478
tU32 gPalette_fade_time;

// GLOBAL: CARMA2_HW 0x0067c47c
tU32 gToo_poor_for_recovery_timeout;

// GLOBAL: CARMA2_HW 0x0079efa4
tCamera_key_flags gCamera_key_flags;

// CompleteRace

// FUNCTION: CARMA2_HW 0x00441490
void C2_HOOK_FASTCALL AbortRace(void) {

    if (!gRace_finished) {
        gAbandon_game = 1;
    }
}
// F4Key

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004414b0
void C2_HOOK_FASTCALL F4Key(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00441580
void C2_HOOK_FASTCALL SetFlag(int i) {

    if (gI_am_cheating == i) {
        gI_am_cheating = 0;
        NewTextHeadupSlot(4, 0, 3000, -4, "Cheating Off");
    } else {
        gI_am_cheating = i;
    }
    F4Key();
}

// FUNCTION: CARMA2_HW 0x004415c0
void C2_HOOK_FASTCALL FinishRace(int i) {
    gRace_over_reason = eRace_over_0;
    gAbandon_game = 2;
}
// FUNCTION: CARMA2_HW 0x004415e0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

}

// FUNCTION: CARMA2_HW 0x004415f0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

}

// GLOBAL: CARMA2_HW 0x0067c468
int gWhich_edit_mode;

// GLOBAL: CARMA2_HW 0x005904f0
tEdit_func* gEdit_funcs[2][18][8];

static void C2_HOOK_FASTCALL DoEditModeKey(int pIndex) {
    int modifiers;

    if (gI_am_cheating == 0xa11ee75d || (gI_am_cheating == 0x564e78b9 && gNet_mode == eNet_mode_none)) {
        modifiers = PDKeyDown(0) ? 4 : 0;
        modifiers |= PDKeyDown(1) ? 2 : 0;
        modifiers |= PDKeyDown(2) ? 1 : 0;
        if (gEdit_funcs[gWhich_edit_mode][pIndex][modifiers] != NULL) {
            gEdit_funcs[gWhich_edit_mode][pIndex][modifiers]();
        }
    } else {
        gWhich_edit_mode = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00441600
void C2_HOOK_FASTCALL F5Key(void) {

    DoEditModeKey(0);
}

// FUNCTION: CARMA2_HW 0x00441680
void C2_HOOK_FASTCALL F6Key(void) {

    DoEditModeKey(1);
}

// FUNCTION: CARMA2_HW 0x00441700
void C2_HOOK_FASTCALL F7Key(void) {

    DoEditModeKey(2);
}

// FUNCTION: CARMA2_HW 0x00441780
void C2_HOOK_FASTCALL F8Key(void) {

    DoEditModeKey(3);
}

// FUNCTION: CARMA2_HW 0x00441800
void C2_HOOK_FASTCALL F10Key(void) {

    DoEditModeKey(4);
}

// FUNCTION: CARMA2_HW 0x00441880
void C2_HOOK_FASTCALL F11Key(void) {

    DoEditModeKey(5);
}

// FUNCTION: CARMA2_HW 0x00441900
void C2_HOOK_FASTCALL F12Key(void) {
}

// FUNCTION: CARMA2_HW 0x00441910
void C2_HOOK_FASTCALL NumberKey0(void) {

    DoEditModeKey(7);
}

// FUNCTION: CARMA2_HW 0x00441990
void C2_HOOK_FASTCALL NumberKey1(void) {

    DoEditModeKey(8);
}

// FUNCTION: CARMA2_HW 0x00441a10
void C2_HOOK_FASTCALL NumberKey2(void) {

    DoEditModeKey(9);
}

// FUNCTION: CARMA2_HW 0x00441a90
void C2_HOOK_FASTCALL NumberKey3(void) {

    DoEditModeKey(10);
}

// FUNCTION: CARMA2_HW 0x00441b10
void C2_HOOK_FASTCALL NumberKey4(void) {

    DoEditModeKey(11);
}

// FUNCTION: CARMA2_HW 0x00441b90
void C2_HOOK_FASTCALL NumberKey5(void) {

    DoEditModeKey(12);
}

// FUNCTION: CARMA2_HW 0x00441c10
void C2_HOOK_FASTCALL NumberKey6(void) {

    DoEditModeKey(13);
}

// FUNCTION: CARMA2_HW 0x00441c90
void C2_HOOK_FASTCALL NumberKey7(void) {

    DoEditModeKey(14);
}

// FUNCTION: CARMA2_HW 0x00441d10
void C2_HOOK_FASTCALL NumberKey8(void) {

    DoEditModeKey(15);
}

// FUNCTION: CARMA2_HW 0x00441d90
void C2_HOOK_FASTCALL NumberKey9(void) {

    DoEditModeKey(16);
}

// FUNCTION: CARMA2_HW 0x00441e10
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

// FUNCTION: CARMA2_HW 0x004420e0
void C2_HOOK_FASTCALL ToggleMiniMap(void) {

    if (gMap_view == 2) {
        return;
    }
    gMini_map_visible = !gMini_map_visible;
    if (gMini_map_visible) {
        NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_on));
    } else {
        NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_off));
    }
    UpdateMapAndSaveOptions();
}

// CheckToggles

extern int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around);

// FUNCTION: CARMA2_HW 0x00442130
int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar) {

    return CarWorldOffFallenCheckThingy(pCar, 1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00442140
int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CheckForBeingOutOfThisWorld

// CheckHornLocal

// FUNCTION: CARMA2_HW 0x00443ba0
void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar) {

    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound3D(gEffects_outlet, eSoundId_Horn,
                                                &pCar->car_master_actor->t.t.translate.t, &pCar->vel,
                                                0, 0xff, ((pCar->car_ID & 0x7) + 0xc) << 12, 0x10000);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag) != 0) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}

// CheckHorns

// SetQuickRecovery

/* based on carpocalypse2 src/carma2/common/controls.c:896 */
// FUNCTION: CARMA2_HW 0x00442300
void C2_HOOK_FASTCALL SetRecovery(void) {
    extern int gINT_0068b8e4;
    extern int gINT_0068b8e8;
    extern int gCancel_recover;
    extern int gIt_or_fox;
    extern int gToo_late;

    if (gINT_0068b8e4
        || gINT_0068b8e8
        || gRace_finished
        || gProgram_state.current_car.knackered
        || gWait_for_it
        || gHad_auto_recover
        || gPalette_fade_time != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        gRecover_car = 1;
        gRecover_timer = 0;
    } else if (gProgram_state.current_car.time_to_recover == 0) {
        gCancel_recover = 1;
        if (CheckRecoverCost()) {
            if (gCurrent_net_game->type == eNet_game_type_foxy) {
                if (gThis_net_player_index == gIt_or_fox) {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 5000;
                } else {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 1000;
                }
            } else {
                if (gCurrent_net_game->type == 6 &&
                    gNet_players[gThis_net_player_index].field_0x80 != 0) {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
                } else {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 3000;
                }
            }
        }
    }
    else if (gCancel_recover) {
        if (GetRaceTime() + 600 < gProgram_state.current_car.time_to_recover) {
            gProgram_state.current_car.time_to_recover = 0;
            if (!gINT_0067c470) {
                float cost = (gNet_mode == eNet_mode_none) ? gRecovery_cost.initial[gProgram_state.skill_level] : gRecovery_cost.initial_network[gCurrent_net_game->type];
                EarnCredits((int)cost);
            }
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_recovery_cancelled), 0);
        } else {
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_too_late_to_cancel), 1);
            gToo_late = 1;
        }
    }
}

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

/* based on the original disassembly (carpocalypse2 src/carma2/common/controls.c:2412 as reference) */
// FUNCTION: CARMA2_HW 0x00442500
void C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime) {
    extern int gToo_late;
    extern int gCancel_recover;
    extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
    extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

    if (gProgram_state.current_car.time_to_recover != 0) {
        if (gProgram_state.current_car.knackered) {
            gProgram_state.current_car.time_to_recover = 0;
        } else {
            int time;
            char str[256];

            time = (gProgram_state.current_car.time_to_recover - pEndFrameTime + 1000) / 1000;
            sprintf(str, "%s %d %s", GetMiscString(eMiscString_recovery_in), time, GetMiscString(time > 1 ? eMiscString_seconds : eMiscString_second));
            if (!gToo_late && gCancel_recover) {
                NewTextHeadupSlot2(4, 0, 2000, -4, str, 0);
            }
            if (gProgram_state.current_car.time_to_recover <= pEndFrameTime) {
                if (gNet_mode == eNet_mode_none || gPalette_fade_time == 0) {
                    gRecover_car = 1;
                }
                gProgram_state.current_car.time_to_recover = 0;
            }
        }
    }
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {
            tCar_spec* car;
            br_vector3 pos;
            br_vector3 vel;
            br_vector3 coll_pos;

            if (i != gThis_net_player_index && gNet_players[i].car->time_to_recover != 0 && gNet_players[i].car->time_to_recover <= pEndFrameTime) {
                gNet_players[i].player_status = ePlayer_status_recovering;
                gNet_players[i].car->collision_info->field_0x261 = 0xff;
                gNet_players[i].car->collision_info->message_time = pEndFrameTime;
                gNet_players[i].car->collision_info->field_0x49c = 0;
                car = gNet_players[i].car;
                pos = car->collision_info->cmpos;
                vel.v[0] = car->collision_info->v.v[0] * 6.9;
                vel.v[1] = car->collision_info->v.v[1] * 6.9;
                vel.v[2] = car->collision_info->v.v[2] * 6.9;
                pos.v[2] = car->collision_info->bb2.min.v[2] - (car->collision_info->bb2.max.v[2] - car->collision_info->bb2.min.v[2]) * -0.3;
                BrMatrix34ApplyP(&coll_pos, &pos, &car->car_master_actor->t.t.mat);
                CreatePuffOfSmoke(&coll_pos, &vel, 1.f, 1.f, 7);
                pos.v[2] = car->collision_info->bb2.min.v[2] - (car->collision_info->bb2.max.v[2] - car->collision_info->bb2.min.v[2]) * -0.7;
                BrMatrix34ApplyP(&coll_pos, &pos, &car->car_master_actor->t.t.mat);
                CreatePuffOfSmoke(&coll_pos, &vel, 1.f, 1.f, 7);
                if (car->car_crush_spec->field_0x4b8 != 0) {
                    WeldCar(car);
                }
                DisableCar(car);
                car->time_to_recover = 0;
            }
        }
    }
}

// TradeInAPO

// CheckOtherRacingKeys

/* based on carpocalypse2 src/carma2/common/controls.c:948 */
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern tSlot_info gTrade_in_value_APO;

// FUNCTION: CARMA2_HW 0x00442750
int C2_HOOK_FASTCALL CheckRecoverCost(void) {
    int recovery_cost;
    int remaining_cost;
    int which;
    int part_value;

    if (gNet_mode != eNet_mode_none) {
    if (gProgram_state.current_car.knackered
        || gRecovery_voucher_count != 0
        || (NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80 != 0)) {
        gINT_0067c470 = 1;
    } else {
    gINT_0067c470 = 0;
    if (gProgram_state.credits < (gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
        recovery_cost = (int)(gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]);
        remaining_cost = recovery_cost;
        while (remaining_cost > 0
            && (gProgram_state.current_car.power_up_levels[0] != 0
                || gProgram_state.current_car.power_up_levels[1] != 0
                || gProgram_state.current_car.power_up_levels[2] != 0)) {
            which = IRandomBetween(0, 2);
            if (gProgram_state.current_car.power_up_levels[which] != 0) {
                gProgram_state.current_car.power_up_levels[which] -= 1;
                if (gNet_mode) {
                    part_value = gTrade_in_value_APO.initial_network[gCurrent_net_game->type];
                } else {
                    part_value = gTrade_in_value_APO.initial[gProgram_state.skill_level];
                }
                remaining_cost -= part_value;
                gProgram_state.credits += gNet_mode ? gTrade_in_value_APO.initial_network[gCurrent_net_game->type] : gTrade_in_value_APO.initial[gProgram_state.skill_level];
            }
        }
    }
    if (gProgram_state.credits < (gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
        gToo_poor_for_recovery_timeout = PDGetTotalTime() + 2000;
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_press_return_to_commit_suicide));
        return 0;
    }
    SpendCredits((int)(gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
    return 1;
    }
    }
    return 1;
}

// SortOutRecover

// based on carpocalypse2 src/carma2/common/controls.c:2143 and upstream controls.c (reference only)
extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object* pCollision_info, tU32 pDisable);
extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);
extern void C2_HOOK_FASTCALL FlipUpCollisionInfo(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);
extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3);
extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

static void AlignChildren(tPhysics_object* pCollision_info) {
    tPhysics_object* child;
    int i;
    int j;

    for (child = pCollision_info->child; child != NULL; child = child->next) {
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                child->transform_matrix.m[i][j] = pCollision_info->transform_matrix.m[i][j];
            }
        }
        BrVector3SetFloat(&child->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&child->omega, 0.f, 0.f, 0.f);
        FlipUpCollisionInfo(child);
    }
}

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

// FUNCTION: CARMA2_HW 0x004429b0
void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {
    int new_pos;
    int i = 0;
    int j;
    int k;
    br_scalar t;
    br_material* mat;
    br_vector3 nor;
    br_vector3 p;
    br_vector3 dir;

    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gNet_mode == eNet_mode_none) {
        FadePaletteDown();
        while (KeyIsDown(0x3c)) {
        }
    }

    if (pCar_spec->car_crush_spec != NULL && pCar_spec->car_crush_spec->field_0x4b8) {
        WeldCar(pCar_spec);
    }
    SetCollisionInfoChildsDoNothing(pCar_spec->collision_info, 0);
    EnableCar(pCar_spec);

    new_pos = 1;
    for (k = 0; k < 4; k++) {
        if (pCar_spec->oldd[k] >= pCar_spec->susp_height[k >> 1]) {
            new_pos = 0;
        }
    }
    do {
        BrVector3Sub(&nor, &pCar_spec->car_master_actor->t.t.translate.t, (br_vector3*)pCar_spec->last_safe_positions[0].m[3]);
        if (BrVector3LengthSquared(&nor) > 8.3015966f) {
            new_pos = 0;
        }
        BrMatrix34Copy(&pCar_spec->car_master_actor->t.t.mat, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->collision_info->transform_matrix, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->old_frame_mat, &pCar_spec->last_safe_positions[new_pos]);
        BrVector3Set(&dir, 0.f, 0.28985506f, 0.f);
        FindFace(&pCar_spec->car_master_actor->t.t.translate.t, &dir, &nor, &t, &mat);
        if (t > 1.f) {
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &dir);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
        BrVector3Set(&nor, 0.f, 0.f, -0.0002f);
        BrMatrix34ApplyV(&pCar_spec->collision_info->v, &nor, &pCar_spec->car_master_actor->t.t.mat);
        BrVector3Set(&pCar_spec->collision_info->omega, 0.f, 0.f, 0.f);
        BrVector3Negate(&pCar_spec->direction, (br_vector3*)pCar_spec->collision_info->transform_matrix.m[2]);

        for (j = 0; j <= new_pos; j++) {
            for (k = 0; k < REC2_ASIZE(pCar_spec->last_safe_positions) - 1; k++) {
                BrMatrix34Copy(&pCar_spec->last_safe_positions[k], &pCar_spec->last_safe_positions[k + 1]);
            }
        }
        AlignChildren(pCar_spec->collision_info);
        PositionChildren(pCar_spec->collision_info);
        for (j = 0; !TestForCarInSensiblePlace(pCar_spec, &p) && j < 10; j++) {
            BrVector3InvScale(&p, &p, 2.f * WORLD_SCALE);
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &p);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
        i++;
    } while (j == 10 && i < 3);
    BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3],
        &pCar_spec->car_master_actor->t.t.translate.t);
    pCar_spec->curvature = 0.f;
    for (i = 0; i < REC2_ASIZE(pCar_spec->oldd); i++) {
        pCar_spec->oldd[i] = pCar_spec->ride_height;
    }
    pCar_spec->revs = 0.f;
    pCar_spec->gear = 0.f;
    pCar_spec->collision_info->auto_special_volume = NULL;
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human) {
        InitialiseExternalCamera();
        PositionExternalCamera(pCar_spec, 100);
    }
}

extern void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex);

// FUNCTION: CARMA2_HW 0x00442e80
void C2_HOOK_FASTCALL GetPowerup(int pNum) {

    GotPowerup(&gProgram_state.current_car, pNum);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00442f90
void C2_HOOK_FASTCALL CheckOtherRacingKeys(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// Action proc of gToggle_array, not implemented yet (stub)
void C2_HOOK_FASTCALL ToggleFlaps(void) {
    NOT_IMPLEMENTED();
}
// Action proc of gToggle_array, not implemented yet (stub)
void C2_HOOK_FASTCALL ViewNetPlayer(void) {
    NOT_IMPLEMENTED();
}
// Action proc of gToggle_array, not implemented yet (stub)
void C2_HOOK_FASTCALL ToggleCockpit(void) {
    NOT_IMPLEMENTED();
}
// Action proc of gToggle_array, not implemented yet (stub)
void C2_HOOK_FASTCALL LeftInventory(void) {
    NOT_IMPLEMENTED();
}
// Action proc of gToggle_array, not implemented yet (stub)
void C2_HOOK_FASTCALL RightInventory(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern void C2_HOOK_FASTCALL ToggleMiniMap(void);
extern void C2_HOOK_FASTCALL CycleMapOptions(void);
extern void C2_HOOK_FASTCALL ToggleTargetLock(void);
extern void C2_HOOK_FASTCALL SetRecovery(void);
extern void C2_HOOK_FASTCALL F4Key(void);
extern void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void);
extern void C2_HOOK_FASTCALL ViewNetPlayer(void);
extern void C2_HOOK_FASTCALL ToggleCockpit(void);
extern void C2_HOOK_FASTCALL BuyArmour(void);
extern void C2_HOOK_FASTCALL BuyPower(void);
extern void C2_HOOK_FASTCALL BuyOffense(void);
extern void C2_HOOK_FASTCALL ToggleInventory(void);
extern void C2_HOOK_FASTCALL LeftInventory(void);
extern void C2_HOOK_FASTCALL RightInventory(void);

// GLOBAL: CARMA2_HW 0x005900a0
tToggle_element gToggle_array[44] = {
    { 0x23, -2, 1, 1, 0, ToggleMiniMap },
    { 0x4a, -2, 1, 1, 0, ToggleFlaps },
    { 0x39, -2, 1, 1, 0, ToggleMap },
    { 0x39, 8, 1, 1, 0, (void (C2_HOOK_FASTCALL *)(void))ToggleHeadupMap },
    { 0x39, 7, 1, 1, 0, CycleMapOptions },
    { 0x39, 9, 1, 1, 0, ToggleMapTransparency },
    { 0x3c, -2, 1, 1, 0, SetRecovery },
    { 0x04, 7, 1, 0, 0, AbortRace },
    { 0x25, -2, 0, 1, 0, ToggleSoundEnable },
    { 0x05, 8, 0, 1, 0, PrintScreen },
    { 0x4c, -2, 1, 0, 0, ToggleHeadupLevel },
    { 0x12, -2, 0, 0, 0, F4Key },
    { 0x13, -2, 1, 0, 0, F5Key },
    { 0x14, -2, 1, 0, 0, F6Key },
    { 0x15, -2, 1, 0, 0, F7Key },
    { 0x16, -2, 1, 0, 0, F8Key },
    { 0x17, -2, 1, 0, 0, F10Key },
    { 0x18, -2, 1, 0, 0, F11Key },
    { 0x19, -2, 1, 0, 0, F12Key },
    { 0x0f, -2, 1, 0, 0, NumberKey0 },
    { 0x26, -2, 1, 0, 0, NumberKey1 },
    { 0x27, -2, 1, 0, 0, NumberKey2 },
    { 0x28, -2, 1, 0, 0, NumberKey3 },
    { 0x29, -2, 1, 0, 0, NumberKey4 },
    { 0x2a, -2, 1, 0, 0, NumberKey5 },
    { 0x2b, -2, 1, 0, 0, NumberKey6 },
    { 0x2c, -2, 1, 0, 0, NumberKey7 },
    { 0x10, -2, 1, 0, 0, NumberKey8 },
    { 0x11, -2, 1, 0, 0, NumberKey9 },
    { 0x0b, 7, 1, 1, 0, ToggleFlying },
    { 0x41, -2, 1, 0, 0, ScreenSmaller },
    { 0x42, -2, 1, 0, 0, ScreenLarger },
    { 0x43, -2, 1, 0, 0, BuyArmour },
    { 0x44, -2, 1, 0, 0, BuyPower },
    { 0x45, -2, 1, 0, 0, BuyOffense },
    { 0x46, -2, 1, 0, 0, ViewNetPlayer },
    { 0x47, -2, 1, 0, 0, UserSendMessage },
    { 0x48, -2, 1, 0, 0, ToggleTargetLock },
    { 0x49, -2, 1, 0, 0, CycleTargetLock },
    { 0x3e, -2, 1, 0, 0, ToggleInventory },
    { 0x3f, -2, 1, 0, 0, LeftInventory },
    { 0x40, -2, 1, 0, 0, RightInventory },
    { 0x48, -2, 1, 0, 0, ChangeCameraTypeInGame },
    { 0x2e, -2, 1, 0, 0, ToggleCockpit },
};

// GLOBAL: CARMA2_HW 0x005904c0
const char* gUNK_005904c0[12] = { 0 };

// FUNCTION: CARMA2_HW 0x00442e90
void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) {
    tU32 start_menu_time;
    tToggle_element* toggle;
    int new_state;

    start_menu_time = PDGetTotalTime();
    CheckQuit();
    AddLostTime(PDGetTotalTime() - start_menu_time);
    CheckKeysForMouldiness();
    for (toggle = gToggle_array; toggle < &gToggle_array[44]; toggle++) {
        if (toggle->in_game_only && !pRacing) {
            continue;
        }
        if (gTyping || gEntering_message) {
            if (toggle->key2 == -2) {
                continue;
            }
        }
        new_state = 0;
        if (toggle->key1 == -2 || KeyIsDownNoMouldiness(toggle->key1)) {
            if (toggle->key2 == -2 && toggle->exact_modifiers) {
                if (!PDKeyDown(0) && !PDKeyDown(1) && !PDKeyDown(2) && !PDKeyDown(3)) {
                    new_state = 1;
                }
            } else {
                if (KeyIsDown(toggle->key2)) {
                    new_state = 1;
                }
            }
        }
        if (toggle->on_last_time != new_state) {
            toggle->on_last_time = new_state;
            if (new_state) {
                toggle->action_proc();
            }
        }
    }
    if (pRacing && !gAction_replay_mode) {
        CheckOtherRacingKeys();
    }
}

// FUNCTION: CARMA2_HW 0x00443c50
void C2_HOOK_FASTCALL CheckForCheatingGits(void) {
    tU32* keys;

    keys = KevKeyService();
    if (keys[0] == 0x616fb8e4 && keys[1] == 0x7c6100a8) {
        ToggleSoundEnable();
        gProgram_state.game_completed = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_FlaskGone);
    }
}
// CheckKevKeys

// BrakeInstantly

extern tS3_outlet* gCar_outlet;
extern int gInstant_handbrake;
extern int gCredit_multiplier;
extern int C2_HOOK_FASTCALL HasCurrentJoystick(void);
extern void C2_HOOK_FASTCALL JumpTheStart(void);

// based on carpocalypse2 src/carma2/common/controls.c:2468 (reference only)
// FUNCTION: CARMA2_HW 0x00443e80
void C2_HOOK_FASTCALL PollCarControls(void) {
    tCar_controls keys;
    tJoystick joystick;
    tCar_spec* c;
    br_scalar speed_squared;
    int i;

    c = &gProgram_state.current_car;
    memset(&keys, 0, sizeof(tCar_controls));
    joystick.left = -1;
    joystick.right = -1;
    joystick.acc = -1;
    joystick.dec = -1;
    CheckKeysForMouldiness();
    if (gEntering_message) {
        c->keys = keys;
        c->joystick = joystick;
        return;
    }
    if (gKey_mapping[47] < 143 && gKey_mapping[48] < 143) {
        if (KeyIsDownNoMouldiness(47)) {
            keys.left = 1;
        }
        if (KeyIsDownNoMouldiness(48)) {
            keys.right = 1;
        }
    }
    if (!(keys.left || keys.right)) {
        if (PDIsJoystickDPadEnabled()) {
            if (gJoy_array[0] > 10) {
                keys.left = 1;
            }
            if (gJoy_array[1] > 10) {
                keys.right = 1;
            }
        } else {
            joystick.left = gJoy_array[0];
            joystick.right = gJoy_array[1];
            if (gJoy_array[0] < 0 && gJoy_array[1] < 0) {
                joystick.left = 0;
            }
        }
    }
    if (KeyIsDownNoMouldiness(13)) {
        keys.holdw = 1;
    }
    if (KeyIsDownNoMouldiness(54) || gRace_finished) {
        if (gInstant_handbrake && !gRace_finished) {
            gProgram_state.current_car.revs = 0.f;
            speed_squared = BrVector3LengthSquared(&gProgram_state.current_car.collision_info->v);
            if (gProgram_state.current_car.number_of_wheels_on_ground != 0 && speed_squared > .0001f) {
                PratcamEvent(41);
                for (i = 0; i < 5; i++) {
                    DRS3StartSound(gCar_outlet, eSoundId_TyreScreeching + i);
                }
            }
            if (speed_squared > .1f) {
                PDPlayFFBEffect("Instant_Handbrake");
            }
            BrVector3Set(&gProgram_state.current_car.collision_info->v, 0.f, 0.f, 0.f);
        } else {
            keys.brake = 1;
        }
    }
    if (KeyIsDownNoMouldiness(49) && !gRace_finished && !c->knackered && !gWait_for_it) {
        keys.acc = 1;
    }
    if (!keys.acc) {
        if (PDIsJoystickDPadEnabled()) {
            if (gJoy_array[2] > 10) {
                keys.acc = 1;
            }
        } else {
            if (HasCurrentJoystick()) {
                joystick.acc = gJoy_array[2];
            }
        }
    }
    if (KeyIsDownNoMouldiness(50) && !gRace_finished && !c->knackered && !gWait_for_it) {
        keys.dec = 1;
    }
    if (!keys.dec) {
        if (PDIsJoystickDPadEnabled()) {
            if (gJoy_array[3] > 10) {
                keys.dec = 1;
            }
        } else {
            if (HasCurrentJoystick()) {
                joystick.dec = gJoy_array[3];
            }
        }
    }

    if (gCredit_multiplier) {

        int temp;

        temp = joystick.acc;
        joystick.acc = joystick.dec;
        joystick.dec = temp;

        temp = joystick.left;
        joystick.left = joystick.right;
        joystick.right = temp;

        {
            int acc = keys.acc;
            int dec = keys.dec;
            keys.dec = acc;
            keys.acc = dec;
        }

        {
            int l = keys.left;
            int r = keys.right;
            keys.left = r;
            keys.right = l;
        }
    }

    if (KeyIsDownNoMouldiness(56) && c->gear >= 0) {
        keys.change_down = 1;
        c->just_changed_gear = 1;
        if (keys.acc || joystick.acc > 32000) {
            c->traction_control = 0;
        } else if (c->gear > 1) {
            c->gear -= !c->keys.change_down;
        }
        if (gCountdown != 0 && !c->keys.change_down) {
            JumpTheStart();
        }
    }
    if (gCar_flying) {
        if (KeyIsDownNoMouldiness(14)) {
            keys.up = 1;
        }
        if (KeyIsDownNoMouldiness(12)) {
            keys.down = 1;
        }
    }
    if (KeyIsDownNoMouldiness(59)) {
        if (!gEntering_message) {
            keys.horn = 1;
        }
    }
    c->keys = keys;
    c->joystick = joystick;
}

// FUNCTION: CARMA2_HW 0x00444270
void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period) {
    int down0;
    int down1;

    down0 = PDKeyDown(0);
    down1 = PDKeyDown(1);
    if (down0 || down1) {
        CheckMapMoveKeys(down0, down1);
        gCamera_key_flags.field_0x0_bit1 = 0;
        gCamera_key_flags.field_0x0_bit2 = 0;
        gCamera_key_flags.field_0x0_bit3 = 0;
        gCamera_key_flags.field_0x0_bit4 = 0;
    } else {
        gCamera_key_flags.field_0x0_bit1 = KeyIsDownNoMouldiness(31);
        gCamera_key_flags.field_0x0_bit2 = KeyIsDownNoMouldiness(32);
        gCamera_key_flags.field_0x0_bit3 = KeyIsDownNoMouldiness(33);
        gCamera_key_flags.field_0x0_bit4 = KeyIsDownNoMouldiness(34);
        gCamera_key_flags.field_0x0_bit0 = PDKeyDown(2);
    }
}
// FUNCTION: CARMA2_HW 0x00444350
void C2_HOOK_FASTCALL SetFlag2(int i) {

    gAllow_car_flying = 1;
    if (gNet_mode == eNet_mode_none) {
        gCar_flying = !gCar_flying;
        if (gCar_flying) {
            NewTextHeadupSlot(4, 0, 500, -4, "We have lift off!!");
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, "Back down to Earth");
        }
    } else {
        gCar_flying = 0;
    }
    gAllow_car_flying = gCar_flying;
}
// FUNCTION: CARMA2_HW 0x004443c0
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
// GLOBAL: CARMA2_HW 0x0067c498
int gInvulnerability;

// FUNCTION: CARMA2_HW 0x00444420
void C2_HOOK_FASTCALL CycleInvulnerability(void) {
    char message[256];

    gInvulnerability = (gInvulnerability + 1) % 8;
    gCar_to_view->invulnerable_no_damage = gInvulnerability & 1;
    gCar_to_view->invulnerable_no_crushage = gInvulnerability & 2;
    gCar_to_view->invulnerable_no_wastage = gInvulnerability & 4;
    message[0] = '\0';
    if (gInvulnerability == 0) {
        strcpy(message, "FULLY VULNERABLE");
    }
    if (gCar_to_view->invulnerable_no_damage) {
        strcat(message, " NO DAMAGE");
    }
    if (gCar_to_view->invulnerable_no_crushage != 0) {
        strcat(message, " NO CRUSHAGE");
    }
    if (gCar_to_view->invulnerable_no_wastage != 0) {
        strcat(message, " NO WASTAGE");
    }
    NewTextHeadupSlot(4, 0, 1000, -4, message);
}

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

extern int gIt_or_fox;
extern void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);

// GLOBAL: CARMA2_HW 0x0067c458
static int old_indent;

// FUNCTION: CARMA2_HW 0x00444610
void C2_HOOK_FASTCALL ToggleMap(void) {

    if (gMap_view == 2) {
        gMap_view = 1;
        gRender_indent = old_indent;
        if (gCurrent_race.map_image != NULL) {
            PixelmapSwapByteOrder(gCurrent_race.map_image);
        }
    } else if (gAction_replay_mode == 0) {
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
        } else {
            old_indent = gRender_indent;
            gRender_indent = 0;
            gMap_view = 2;
            gMap_time = PDGetTotalTime();
            if (gCurrent_race.map_image != NULL) {
                PixelmapSwapByteOrder(gCurrent_race.map_image);
            }
        }
    }
    AdjustRenderScreenSize();
}

// FUNCTION: CARMA2_HW 0x004446f0
int C2_HOOK_FASTCALL GetRecoverVoucherCount(void) {

    return gRecovery_voucher_count;
}

// FUNCTION: CARMA2_HW 0x00444700
void C2_HOOK_FASTCALL AddVouchers(int amount) {

    gRecovery_voucher_count += amount;
}

// FUNCTION: CARMA2_HW 0x00444710
void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {

    gRecovery_voucher_count = 0;
}
// FUNCTION: CARMA2_HW 0x00444720
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
extern int gSound_detail_level;

// FUNCTION: CARMA2_HW 0x004447c0
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

extern int gCar_simplification_level;

// FUNCTION: CARMA2_HW 0x00444820
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
void C2_HOOK_FASTCALL UserSendMessage(void) {

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->options.enable_text_messages) {
        gEntering_message = 1;
    }
}
// EnterUserMessage

extern void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);
extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
extern void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred);
extern tDR_font gFonts[28];
extern tGraf_data* gCurrent_graf_data;

// FUNCTION: CARMA2_HW 0x00444c00
void C2_HOOK_FASTCALL DisplayUserMessage(void) {
    char* the_message;
    int len;
    tDR_font* font;

    font = &gFonts[6];
    the_message = &gString[20];
    if (!gEntering_message || gNet_mode == eNet_mode_none) {
        return;
    }

    len = strlen(the_message);
    if (len < 63 && (PDGetTotalTime() & 0x200)) {
        the_message[len] = '_';
        the_message[len + 1] = '\0';
    }
    DimRectangle(gBack_screen,
        15 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y - font->height,
        85 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        1);

    DRPixelmapText(gBack_screen, 20 * gBack_screen->width / 100, gCurrent_graf_data->net_message_enter_y, font, GetMiscString(227), 100);
    OoerrIveGotTextInMeBoxMissus(
        6,
        the_message,
        gBack_screen,
        20 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 2 * font->height,
        80 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        0);
    the_message[len] = '\0';
}

// FUNCTION: CARMA2_HW 0x00444d70
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
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

/* local declarations (no new includes in this file) */
extern void C2_HOOK_FASTCALL ChangeCameraType(void);
extern tActionReplayCameraMode gAction_replay_camera_mode;

// FUNCTION: CARMA2_HW 0x00444ed0
void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void) {

    if (!gAction_replay_mode) {
        ChangeCameraType();
        NewTextHeadupSlot2(4, 0, 1000, -4, GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode), 0);
    }
}

// SteelBollock

extern tCar_spec* gCar_to_view;
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar);

// FUNCTION: CARMA2_HW 0x00444f10
void C2_HOOK_FASTCALL RichardsSphere(int pNum) {

    TotallyRepairACar(gCar_to_view);
    SphericizeCar(gCar_to_view);
    NewTextHeadupSlot(4, 0, 2000, -4, "STEEL GONAD O' DEATH");
}

// FUNCTION: CARMA2_HW 0x00444f40
void C2_HOOK_FASTCALL ToggleHeadupLevel(void) {

    if (PDKeyDown(0)) {
        gHeadup_detail_level -= 1;
        if (gHeadup_detail_level < 0) {
            gHeadup_detail_level = kMax_headup_detail_level;
        }
    } else {
        gHeadup_detail_level += 1;
        if (gHeadup_detail_level > kMax_headup_detail_level) {
            gHeadup_detail_level = 0;
        }
    }
    NewTextHeadupSlot2(4, 0, 250, -4, GetMiscString(eMiscString_no_HUD + (gHeadup_detail_level % 3)), 0);
}


extern tCheat gKev_keys[];
extern void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);
extern void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on carpocalypse2 src/carma2/common/controls.c:1436 (reference only)
// FUNCTION: CARMA2_HW 0x00443c90
void C2_HOOK_FASTCALL CheckKevKeys(void) {
    int i;
    tU32* value;
    char s[128];

    value = KevKeyService();
    if (value[0] == 0) {
        return;
    }

    for (i = 0; gKev_keys[i].action_proc != NULL; i++) {
        if (gKev_keys[i].code == value[0] && gKev_keys[i].code2 == value[1]) {
            break;
        }
    }

    if (gKev_keys[i].action_proc != NULL) {
        if (gNet_mode != eNet_mode_none) {
            if (gKev_keys[i].num != 0xa11ee75d) {
                strcpy(s, gNet_players[gThis_net_player_index].player_name);
                strcat(s, " ");
                strcat(s, GetMiscString(224));
                NetSendHeadupToAllPlayers(s);
            } else {
                strcpy(s, gNet_players[gThis_net_player_index].player_name);
                strcat(s, " ");
                strcat(s, GetMiscString(225));
                NetSendHeadupToEverybody(s);
                gKev_keys[i].action_proc(gKev_keys[i].num);
            }
            return;
        }
        gKev_keys[i].action_proc(gKev_keys[i].num);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004391d0
void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on carpocalypse2 src/carma2/common/controls.c:1519 (reference only)
// FUNCTION: CARMA2_HW 0x00442e10
void C2_HOOK_FASTCALL FlipUpCollisionInfo(tPhysics_object* pCollision_info) {
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
        FlipUpCollisionInfo(child);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0067c3b8
int gToo_late;

// GLOBAL: CARMA2_HW 0x00590f60
int gCancel_recover = 1;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049d430
void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0049d780
void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00590970
tCheat gKev_keys[] = {
    { 0xa11ee75d, 0xf805eddd, SetFlag      , 0xa11ee75d },
    { 0x398da28c, 0x44339dd4, SetFlag      , 0x564e78b9 },
    { 0x7dc510f3, 0x65c61537, SetFlag2     , 0x00000001 },
    { 0x309e4f55, 0xecc7daaf, FinishRace   , 0x00000000 },
    { 0x1bcbe148, 0x040161b1, GetPowerup   , 0x00000000 },
    { 0x1d5e7725, 0x0ed62a70, GetPowerup   , 0x00000001 },
    { 0x22c65063, 0xe4331bc8, GetPowerup   , 0x00000002 },
    { 0x1a37d28a, 0x139787e4, GetPowerup   , 0x00000003 },
    { 0x1dcba360, 0x1e38bfa1, GetPowerup   , 0x00000004 },
    { 0x24c99afb, 0xd908f952, GetPowerup   , 0x00000005 },
    { 0x200c1bd4, 0x663de391, GetPowerup   , 0x00000006 },
    { 0x252a2e6b, 0x3304d647, GetPowerup   , 0x00000007 },
    { 0x218f555c, 0xe2d3ac58, GetPowerup   , 0x00000008 },
    { 0x1fc7655b, 0xa12f9258, GetPowerup   , 0x00000009 },
    { 0x2b2e6891, 0x4bd611c2, GetPowerup   , 0x0000000a },
    { 0x2db8b34a, 0x4418ac58, GetPowerup   , 0x0000000b },
    { 0x3001467e, 0xb323f944, GetPowerup   , 0x0000000c },
    { 0x23968eda, 0x9259246e, GetPowerup   , 0x0000000d },
    { 0x1f3baa55, 0x56c505a9, GetPowerup   , 0x0000000f },
    { 0x214a2558, 0x56cbf421, GetPowerup   , 0x00000010 },
    { 0x373ae69a, 0xef8c998f, GetPowerup   , 0x00000011 },
    { 0x327ebd75, 0x605a9e3e, GetPowerup   , 0x00000012 },
    { 0x350c0384, 0x73e576d2, GetPowerup   , 0x00000013 },
    { 0x17f03c24, 0x0071650c, GetPowerup   , 0x00000014 },
    { 0x32aeca21, 0x689d3168, GetPowerup   , 0x00000015 },
    { 0x191841aa, 0x10fbd770, GetPowerup   , 0x00000016 },
    { 0x26026896, 0x630e5fa9, GetPowerup   , 0x00000017 },
    { 0x2440ca1b, 0x2e68304c, GetPowerup   , 0x00000018 },
    { 0x37a11b1b, 0x6820b87d, GetPowerup   , 0x00000019 },
    { 0x2f2ea509, 0x6bb804b7, GetPowerup   , 0x0000001a },
    { 0x28f522f1, 0x2f52f8c0, GetPowerup   , 0x0000001b },
    { 0x26c15553, 0xba19a354, GetPowerup   , 0x00000020 },
    { 0x3964b52b, 0x40c94648, GetPowerup   , 0x00000021 },
    { 0x18bf123a, 0x0080c0a9, GetPowerup   , 0x00000022 },
    { 0x2a439e13, 0x3356c0b0, GetPowerup   , 0x00000023 },
    { 0x28769902, 0x50a5d8d1, GetPowerup   , 0x00000024 },
    { 0x2d5aa4e5, 0x427f9d82, GetPowerup   , 0x00000025 },
    { 0x1e73b354, 0x17741619, GetPowerup   , 0x00000026 },
    { 0x1cac0a7c, 0x0a461bb1, GetPowerup   , 0x00000027 },
    { 0x1e3c613a, 0x6b56e92c, GetPowerup   , 0x00000028 },
    { 0x2f4c3519, 0x082321f8, GetPowerup   , 0x00000029 },
    { 0x21f0d261, 0xdae090b9, GetPowerup   , 0x0000002a },
    { 0x1c727344, 0x78f65c91, GetPowerup   , 0x0000002b },
    { 0x2f574845, 0x75ff1428, GetPowerup   , 0x0000002c },
    { 0x1f0601e3, 0x9455c4c8, GetPowerup   , 0x0000002d },
    { 0x26219ff3, 0xfdfd8b46, GetPowerup   , 0x0000002f },
    { 0x26afbb31, 0xe3275e40, GetPowerup   , 0x00000030 },
    { 0x25205546, 0xcf86a14c, GetPowerup   , 0x00000031 },
    { 0x1a0a8e5b, 0x02035340, GetPowerup   , 0x00000033 },
    { 0x1bdea925, 0x5d98fd0c, GetPowerup   , 0x00000034 },
    { 0x2d4dd2a9, 0xf01ba696, GetPowerup   , 0x00000035 },
    { 0x2e7a7505, 0x8920e4f6, GetPowerup   , 0x00000036 },
    { 0x17290940, 0x00901801, GetPowerup   , 0x00000037 },
    { 0x1d4e7a9c, 0x030e2650, GetPowerup   , 0x00000038 },
    { 0x3579d64a, 0x3d2e34c3, GetPowerup   , 0x00000039 },
    { 0x28f4d49c, 0xb3418148, GetPowerup   , 0x0000003a },
    { 0x1d6ba9c3, 0x0e017749, GetPowerup   , 0x0000003b },
    { 0x310971ab, 0xcb973702, GetPowerup   , 0x0000003c },
    { 0x28451eeb, 0x30ff63cb, GetPowerup   , 0x0000003d },
    { 0x1ebfa5ba, 0x92e034ec, GetPowerup   , 0x0000003e },
    { 0x27079773, 0xd1ef511c, GetPowerup   , 0x0000003f },
    { 0x388de72c, 0x047a8dca, GetPowerup   , 0x00000040 },
    { 0x1a0da9fc, 0x0180e010, GetPowerup   , 0x00000041 },
    { 0x2975a10c, 0xefd65f5d, GetPowerup   , 0x00000042 },
    { 0x1e5cc6ca, 0x17b76391, GetPowerup   , 0x00000043 },
    { 0x250c6f99, 0xbdda24cc, GetPowerup   , 0x00000044 },
    { 0x33950e49, 0x2890738c, GetPowerup   , 0x00000045 },
    { 0x4e5f487a, 0x3dc635b8, GetPowerup   , 0x00000046 },
    { 0x3815584c, 0x91bbc26e, GetPowerup   , 0x00000047 },
    { 0x459732b2, 0xb571e010, GetPowerup   , 0x00000048 },
    { 0x62871003, 0x79b15084, GetPowerup   , 0x00000049 },
    { 0x2d72ebb4, 0x5fd4d3ca, GetPowerup   , 0x0000004a },
    { 0x2c3be2aa, 0x90e0eb9c, GetPowerup   , 0x0000004b },
    { 0x2b2be28b, 0x30e0eb7b, GetPowerup   , 0x0000004c },
    { 0x29be089d, 0x635ceb96, GetPowerup   , 0x0000004d },
    { 0x4897982d, 0x06c4fa99, GetPowerup   , 0x0000004e },
    { 0x44d50f49, 0x010edb42, GetPowerup   , 0x0000004f },
    { 0x403afae5, 0x0104a7d2, GetPowerup   , 0x00000050 },
    { 0x45c19e5e, 0x011b2cf9, GetPowerup   , 0x00000051 },
    { 0x2f790ebd, 0x2fd87f6b, GetPowerup   , 0x00000052 },
    { 0x244f60c9, 0x31f4fda3, GetPowerup   , 0x00000053 },
    { 0x2b0794d3, 0x12927dc9, GetPowerup   , 0x00000054 },
    { 0x2a44b628, 0x0c3e7edb, GetPowerup   , 0x00000055 },
    { 0x2998e46d, 0x1360a63e, GetPowerup   , 0x00000056 },
    { 0x23248728, 0xc84d9d51, GetPowerup   , 0x00000057 },
    { 0x289c1822, 0x136e9fc3, GetPowerup   , 0x00000058 },
    { 0x35abb7d0, 0xa08da57c, GetPowerup   , 0x0000005a },
    { 0x1c1fdd92, 0x01dd060c, GetPowerup   , 0x0000005b },
    { 0x253069c1, 0x4972796a, GetPowerup   , 0x0000005c },
    { 0x33ca4873, 0x3b005b24, GetPowerup   , 0x0000005d },
    { 0x1f56cde5, 0x8f213aae, GetPowerup   , 0x0000005e },
    { 0x1784995b, 0x0163c389, GetPowerup   , 0x00000060 },
    { 0x3003eccb, 0x1d74f36f, GetPowerup   , 0x00000061 },
    { 0x4b054b60, 0x6b6736cb, RichardsSphere, 0x00000000 },
    { 0x00000000, 0x00000000, NULL,        0x00000000 },
};
