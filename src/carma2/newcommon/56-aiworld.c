/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "30-opponent.h"
#include "57-drone.h"
#include "27-powerup.h"
#include "13-crush1.h"
#include "globvrpb.h"
#include "41-utility.h"
#include "60-phil.h"
#include "58-crush2.h"
#include "platform.h"
#include "01-network.h"

// MakeVertexAndOffsetIt

// GLOBAL: CARMA2_HW 0x0067baa0
int gCrush_panel_ids[10];

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}

// FUNCTION: CARMA2_HW 0x00401000
// FUNCTION: CARMA2_SW 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
}

// FUNCTION: CARMA2_HW 0x00401030
// FUNCTION: CARMA2_SW 0x00401030
void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}

// FUNCTION: CARMA2_HW 0x00401070
// FUNCTION: CARMA2_SW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0, "BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars(pRace_info);
    PrintMemoryDump(0, "AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0, "AFTER LoadPerRaceDroneStuff()");
}

// InitialiseAIWorldForRace

// FUNCTION: CARMA2_HW 0x00401160
// FUNCTION: CARMA2_SW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {

    DisposeOpponents();
    DisposeDronesRaceStuff();
}

// MungeAIWorld

// FUNCTION: CARMA2_HW 0x004013d0
// FUNCTION: CARMA2_SW 0x004013d0
void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401430
// FUNCTION: CARMA2_SW 0x00401430
void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pLook);
    BrVector3Copy(&trans.t.look_up.up, pUp);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401760
// FUNCTION: CARMA2_SW 0x00401760
br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex) {
    br_material* material;

    material = BrMaterialAllocate(NULL);
    material->colour = ((br_colour*)gRender_palette->pixels)[pIndex];
    material->index_base = (br_uint_8)pIndex;
    material->index_range = 1;
    material->flags = BR_MATF_TWO_SIDED;
    material->index_shade = NULL;
    material->colour_map = NULL;
    material->identifier = NULL;
    BrMaterialAdd(material);
    return material;
}

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere


static int C2_HOOK_FASTCALL CarVulnerableByMine_Impl(tCar_spec* pCar) {
    if (pCar->field_0x4c8 > 1.0) {
        return 0;
    }
    if (gNet_mode != eNet_mode_none) {
        if (NetPlayerFromCar(pCar) != NULL && NetPlayerFromCar(pCar)->field_0x80 != 0) {
            return 0;
        }
    }
    if (pCar->invulnerable_no_crushage) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004dde20
void C2_HOOK_FASTCALL MungeShatMines(tU32 pTime) {
    int i;
    int play_effect;

    for (i = 0; i < 20; i++) {
        tShit_mine* mine = &gShit_mines[i];

        if (!(mine->flags & 0x1)) {
            continue;
        }
        if ((mine->flags & 0x80) && mine->car != NULL) {
            tCar_spec* car = mine->car;
            int car_vulnerable;

            mine->flags &= ~0x80;
            car_vulnerable = CarVulnerableByMine_Impl(car);
            if (car_vulnerable) {
                TotallySpamTheCar(car, 0.1f * mine->max_damage);
            }
            if (car_vulnerable && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)) {
                car->collision_info->v.v[1] += FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (car->collision_info->M * 6.9);
                car->collision_info->omega.v[2] = FRandomPosNeg(mine->initial_z_omega_factor) * 6.283185307179586 / car->collision_info->M + car->collision_info->omega.v[2];
                car->collision_info->omega.v[0] = FRandomPosNeg(mine->initial_x_omega_factor) * 6.283185307179586 / car->collision_info->M + car->collision_info->omega.v[0];
                car->collision_info->disable_move_rotate = 0;
                car->collision_info->field_0x49c = gPHIL_last_physics_tick + 40;
                if (!car->invulnerable_no_damage) {
                    for (i = 0; i < 12; i++) {
                        DamageUnit2(car, i, IRandomBetween(0, (int)(25.f * mine->max_damage)));
                    }
                    if (car != mine->field_0x30) {
                        car->last_hit_by = mine->field_0x30;
                        car->time_last_hit = PDGetTotalTime();
                        CrashEarnings(car, NULL);
                    }
                }
            }
        }
        play_effect = 0;
        if (mine->flags & 0x20) {
            mine->flags &= ~0x20;
            play_effect = 1;
        } else {
            if (!(mine->flags & 0x2)) {
                if ((mine->flags & 0x4) && pTime > mine->next_think_time) {
                    play_effect = 1;
                }
            } else if (pTime > mine->next_think_time) {
                mine->flags &= ~0x3;
            }
        }
        if (play_effect) {
            MineExplode(mine);
        }
    }
}