#include "72-interface-wrecks.h"

#include "72-interface.h"
#include "72-interface-main.h"

#include "30-opponent.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "c2_string.h"

/* BuyCurrentCar and its globals live here, not in 72-interface.c: 72-interface.c must not include 30-opponent.h
   (that include changes MSVC's codegen of SortScores 0x004743c0 from 100% to 68.97%) */

// GLOBAL: CARMA2_HW 0x00763720
tWreck_gallery_sell_info gWreck_gallery_sell_infos[32];

// GLOBAL: CARMA2_HW 0x00763890
int gFrontend_wrecks_current;

// GLOBAL: CARMA2_HW 0x007635e4
br_pixelmap* gFrontend_wrecks_pixelmap;

// GLOBAL: CARMA2_HW 0x007635ec
tU32 gFrontend_wrecks_previous_update;

// GLOBAL: CARMA2_HW 0x00763844
br_actor* gFrontend_wrecks_light;

// FUNCTION: CARMA2_HW 0x0046e780
int C2_HOOK_FASTCALL BuyCurrentCar(tFrontend_spec* pFrontend) {
    tCar_spec* car;
    tOpponent_spec* opponent;

    if (gFrontend_wreck_bought_car_dz != 0.f || gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold) {
        return 0;
    }
    car = GetCarSpec(eVehicle_opponent, gFrontend_wrecks_current);
    opponent = GetOpponentSpecFromCarSpec(car);
    gProgram_state.cars_available[gProgram_state.number_of_cars] = car->index;
    gProgram_state.number_of_cars += 1;
    gProgram_state.current_car_index = car->index;
    gProgram_state.credits -= gOpponents[opponent->index].price;
    gFrontend_wreck_bought_car_dz = 0.001f;
    DRS3StartSound(gEffects_outlet, eSoundId_Done);
    return 0;
}

// based on carpocalypse2 src/carma2/common/frontend_wrecks.c:57
// FUNCTION: CARMA2_HW 0x0046e830
int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend) {
    int car_count;
    int i;

    gCurrent_frontend_spec = pFrontend;
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    gFrontend_wrecks_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_wrecks_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gFrontend_wrecks_light = BrActorAllocate(BR_ACTOR_LIGHT, NULL);

    ((br_camera*)gFrontend_wrecks_camera->type_data)->aspect = 1.6346154f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->field_of_view = BR_ANGLE_DEG(55);
    ((br_camera*)gFrontend_wrecks_camera->type_data)->hither_z = 1.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->yon_z = 100.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->type = BR_CAMERA_PERSPECTIVE_FOV;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->width = 640.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->height = 480.f;
    BrActorAdd(gFrontend_wrecks_actor, gFrontend_wrecks_camera);

    gFrontend_wrecks_pixelmap = BrPixelmapAllocateSub(gBack_screen, 65, 55, 510, 312);
    gFrontend_wrecks_pixelmap->origin_x = 255;
    gFrontend_wrecks_pixelmap->origin_y = 120;

    BrMatrix34RotateY(&gFrontend_wrecks_camera->t.t.mat, BR_ANGLE_DEG(180));
    BrMatrix34PostTranslate(&gFrontend_wrecks_camera->t.t.mat, 0.f, 0.f, -3.f);

    car_count = GetCarCount(eVehicle_opponent);
    gFrontend_wrecks_car_count = 0;

    for (i = 0; i < car_count; i++) {
        tCar_spec* car;
        br_actor* actor;

        car = GetCarSpec(eVehicle_opponent, i);
        actor = car->car_master_actor;
        actor->render_style = BR_RSTYLE_FACES;
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
        BrActorAdd(gFrontend_wrecks_actor, actor);
        gWreck_gallery_car_infos[gFrontend_wrecks_car_count].original_matrix = actor->t.t.mat;
        BrMatrix34Identity(&actor->t.t.mat);
        BrMatrix34Identity(&gWreck_gallery_car_infos[gFrontend_wrecks_car_count].field_0x14);
        BrMatrix34Identity(&gWreck_gallery_car_infos[gFrontend_wrecks_car_count].field_0x74);
        gWreck_gallery_car_infos[gFrontend_wrecks_car_count].actor = actor;
        gFrontend_wrecks_car_count += 1;
    }
    memset(gWreck_gallery_sell_infos, 0, sizeof(gWreck_gallery_sell_infos));
    gFrontend_wreck_bought_car_dz = 0;

    gFrontend_selected_item_index = 0;
    gFrontend_wrecks_current = 0;
    gFrontend_wrecks_hscroll = 0;
    gFrontend_wrecks_pending_hscroll = 0;
    gFrontend_wrecks_previous_update = PDGetTotalTime();
    gFrontend_wrecks_rotate_prev_x = -1;
    gFrontend_wrecks_rotate_prev_y = -1;
    FuckWithWidths(pFrontend);
    gFrontend_opponent_profile_pic_needs_update = 1;
    return 1;
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0046eb80
int C2_HOOK_FASTCALL WrecksUpdateFunc(tFrontend_spec* pFrontend) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x005f8e68
tFrontend_spec gFrontend_WRECKS = {
    "Wrecks",
    0,
    12,
    WrecksInFunc,
    WrecksOutFunc,
    WrecksUpdateFunc,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    8,
    0,
    {
        { 0xd7,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x403, ScrollToPrevCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x403, ScrollToNextCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xd9,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xdd,  BuyCurrentCar,     NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x7,   temp,              NULL, 1, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
    }
};
