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
#include "49-physics.h"
#include "rec2_macros.h"
#include "00-car.h"
#include "34-finteray.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

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


// FUNCTION: CARMA2_HW 0x0044f980
void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone_spec) {
    br_bounds3 bnds;
    tPhysics_object* pPhysics;
    tPhysics_shape* original_shape;
    br_scalar size_x;

    pPhysics = &pDrone_spec->collision_info;
    BrActorToBounds(&bnds, pDrone_spec->actor);
    bnds.min.v[1] = bnds.min.v[1] - -0.001f;
    if (pDrone_spec->field_0x46) {
        original_shape = pPhysics->shape;
    }
    memset(pPhysics, 0, sizeof(tPhysics_object));
    if (pDrone_spec->field_0x46) {
        pPhysics->shape = original_shape;
    }
    else {
        pPhysics->shape = (tPhysics_shape*)AllocateShapeBox(kMem_drone_collision_stuff);
        pPhysics->shape->box.common.bb = bnds;
        pPhysics->shape->box.common.field_0x1c = bnds;
        pPhysics->shape->box.common.next = NULL;
        pDrone_spec->field_0x46 = 1;
    }
    pPhysics->flags_0x238 = 16;
    pPhysics->owner = pDrone_spec;
    pPhysics->actor = pDrone_spec->actor;
    pPhysics->M = pDrone_spec->form->M;
    BrVector3Set(&pPhysics->cmpos, 0.f, .09f, 0.f);
    BrVector3Set(&pPhysics->field_0x54, 0.f, -.4f / WORLD_SCALE, 0.f);
    BrMatrix34Copy(&pPhysics->transform_matrix, &pDrone_spec->actor->t.t.mat);
    pPhysics->box_face_ref = gFace_num__car - 2;
    pPhysics->field_0x1a0 = 0x100000;
    FillInShape(pPhysics->shape);
    UpdateCollisionObject(pPhysics);
    GetNewBoundingBox(&pPhysics->field_0xf4, &pPhysics->bb1, &pPhysics->transform_matrix);
    size_x = bnds.max.v[0] - bnds.min.v[0];
    pPhysics->I.v[0] = ((bnds.max.v[2] - bnds.min.v[2]) * (bnds.max.v[2] - bnds.min.v[2]) + (bnds.max.v[1] - bnds.min.v[1]) * (bnds.max.v[1] - bnds.min.v[1])) * pPhysics->M * 0.083333336f;
    pPhysics->I.v[1] = ((bnds.max.v[2] - bnds.min.v[2]) * (bnds.max.v[2] - bnds.min.v[2]) + size_x * size_x) * pPhysics->M * 0.083333336f;
    pPhysics->I.v[2] = ((bnds.max.v[1] - bnds.min.v[1]) * (bnds.max.v[1] - bnds.min.v[1]) + size_x * size_x) * pPhysics->M * 0.083333336f;
    pPhysics->cmpos.v[0] = bnds.min.v[0] - pDrone_spec->form->center.v[1] * size_x * -0.01;
    pPhysics->cmpos.v[1] = bnds.min.v[1] - pDrone_spec->form->center.v[2] * (bnds.max.v[1] - bnds.min.v[1]) * -0.01;
    pPhysics->cmpos.v[2] = bnds.min.v[2] - pDrone_spec->form->center.v[0] * (bnds.max.v[2] - bnds.min.v[2]) * -0.01;
    if (pDrone_spec->form->type == kDroneType_train) {
        pPhysics->world_friction = -.75;
    }
    pPhysics->disable_move_rotate = 1;
    pPhysics->field_0xed = 1;
    pPhysics->flags |= 0x40;
    pPhysics->drivable_on = !!(pDrone_spec->form->flags & 0x8);
    BrVector3SetFloat(&pPhysics->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pPhysics->omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pPhysics->rotate_omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pPhysics->velocity_car_space, 0.f, 0.f, 0.f);
}