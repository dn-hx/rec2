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

// MakeVertexAndOffsetIt

// FUNCTION: CARMA2_HW 0x00401aa0
void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVert_index, int pFace_index, br_vector3* pPosition, br_material* pMaterial1, br_material* pMaterial2, br_material* pMaterial3) {
    int vertex1;
    int vertex2;
    int vertex3;
    int vertex4;
    int vertex5;
    int vertex6;
    int vertex7;
    float fVar0;
    float fVar1;
    float fVar2;
    float* pfVar11;

    fVar0 = pPosition->v[0];
    fVar2 = pPosition->v[2];
    fVar1 = pPosition->v[1] - -0.15f;
    pModel->vertices[pVert_index].p.v[0] = fVar0;
    pModel->vertices[pVert_index].p.v[1] = fVar1;
    pModel->vertices[pVert_index].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[pVert_index].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[pVert_index].p.v[1] = pModel->vertices[pVert_index].p.v[1] - -0.1f;
    pModel->vertices[pVert_index].p.v[2] = pModel->vertices[pVert_index].p.v[2] - -0.1f;

    vertex1 = pVert_index + 1;
    pModel->vertices[vertex1].p.v[0] = fVar0;
    pModel->vertices[vertex1].p.v[1] = fVar1;
    pModel->vertices[vertex1].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex1].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[vertex1].p.v[1] = pModel->vertices[vertex1].p.v[1] - 0.1f;
    pModel->vertices[vertex1].p.v[2] = pModel->vertices[vertex1].p.v[2] - -0.1f;

    vertex2 = pVert_index + 2;
    pModel->vertices[vertex2].p.v[0] = fVar0;
    pModel->vertices[vertex2].p.v[1] = fVar1;
    pModel->vertices[vertex2].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex2].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex2].p.v[1] = pModel->vertices[vertex2].p.v[1] - 0.1f;
    pModel->vertices[vertex2].p.v[2] = pModel->vertices[vertex2].p.v[2] - -0.1f;

    vertex3 = pVert_index + 3;
    pModel->vertices[vertex3].p.v[0] = fVar0;
    pModel->vertices[vertex3].p.v[1] = fVar1;
    pModel->vertices[vertex3].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex3].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex3].p.v[1] = pModel->vertices[vertex3].p.v[1] - -0.1f;
    pModel->vertices[vertex3].p.v[2] = pModel->vertices[vertex3].p.v[2] - -0.1f;

    vertex4 = pVert_index + 4;
    pModel->vertices[vertex4].p.v[0] = fVar0;
    pModel->vertices[vertex4].p.v[1] = fVar1;
    pModel->vertices[vertex4].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex4].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[vertex4].p.v[1] = pModel->vertices[vertex4].p.v[1] - -0.1f;
    pModel->vertices[vertex4].p.v[2] = pModel->vertices[vertex4].p.v[2] - 0.1f;

    vertex5 = pVert_index + 5;
    pModel->vertices[vertex5].p.v[0] = fVar0;
    pModel->vertices[vertex5].p.v[1] = fVar1;
    pModel->vertices[vertex5].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex5].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[vertex5].p.v[1] = pModel->vertices[vertex5].p.v[1] - 0.1f;
    pModel->vertices[vertex5].p.v[2] = pModel->vertices[vertex5].p.v[2] - 0.1f;

    vertex6 = pVert_index + 6;
    pModel->vertices[vertex6].p.v[0] = fVar0;
    pModel->vertices[vertex6].p.v[1] = fVar1;
    pModel->vertices[vertex6].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex6].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex6].p.v[1] = pModel->vertices[vertex6].p.v[1] - 0.1f;
    pModel->vertices[vertex6].p.v[2] = pModel->vertices[vertex6].p.v[2] - 0.1f;

    vertex7 = pVert_index + 7;
    pModel->vertices[vertex7].p.v[0] = fVar0;
    pModel->vertices[vertex7].p.v[1] = fVar1;
    pModel->vertices[vertex7].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex7].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex7].p.v[1] = pModel->vertices[vertex7].p.v[1] - -0.1f;
    pModel->vertices[vertex7].p.v[2] = pModel->vertices[vertex7].p.v[2] - 0.1f;

    pModel->faces[pFace_index].vertices[0] = pVert_index + 3;
    pModel->faces[pFace_index].vertices[1] = pVert_index + 2;
    pModel->faces[pFace_index].vertices[2] = pVert_index + 1;
    pModel->faces[pFace_index].smoothing = 0xffff;
    pModel->faces[pFace_index].material = pMaterial1;

    pModel->faces[pFace_index + 1].vertices[0] = pVert_index;
    pModel->faces[pFace_index + 1].vertices[1] = pVert_index + 3;
    pModel->faces[pFace_index + 1].vertices[2] = pVert_index + 1;
    pModel->faces[pFace_index + 1].smoothing = 0xffff;
    pModel->faces[pFace_index + 1].material = pMaterial1;

    pModel->faces[pFace_index + 2].vertices[0] = pVert_index + 1;
    pModel->faces[pFace_index + 2].vertices[1] = pVert_index + 5;
    pModel->faces[pFace_index + 2].vertices[2] = pVert_index + 4;
    pModel->faces[pFace_index + 2].smoothing = 0xffff;
    pModel->faces[pFace_index + 2].material = pMaterial2;

    pModel->faces[pFace_index + 3].vertices[0] = pVert_index + 1;
    pModel->faces[pFace_index + 3].vertices[1] = pVert_index + 4;
    pModel->faces[pFace_index + 3].vertices[2] = pVert_index;
    pModel->faces[pFace_index + 3].smoothing = 0xffff;
    pModel->faces[pFace_index + 3].material = pMaterial2;

    pModel->faces[pFace_index + 4].vertices[0] = pVert_index;
    pModel->faces[pFace_index + 4].vertices[1] = pVert_index + 4;
    pModel->faces[pFace_index + 4].vertices[2] = pVert_index + 3;
    pModel->faces[pFace_index + 4].smoothing = 0xffff;
    pModel->faces[pFace_index + 4].material = pMaterial3;

    pModel->faces[pFace_index + 5].vertices[0] = pVert_index + 3;
    pModel->faces[pFace_index + 5].vertices[1] = pVert_index + 4;
    pModel->faces[pFace_index + 5].vertices[2] = pVert_index + 7;
    pModel->faces[pFace_index + 5].smoothing = 0xffff;
    pModel->faces[pFace_index + 5].material = pMaterial3;

    MakeFaceAndTextureIt(pModel, pFace_index + 6, vertex4, vertex5, vertex7, pMaterial1);
    MakeFaceAndTextureIt(pModel, pFace_index + 7, vertex5, vertex6, vertex7, pMaterial1);
    MakeFaceAndTextureIt(pModel, pFace_index + 8, vertex2, vertex7, vertex6, pMaterial2);
    MakeFaceAndTextureIt(pModel, pFace_index + 9, vertex2, vertex3, vertex7, pMaterial2);
    MakeFaceAndTextureIt(pModel, pFace_index + 10, vertex1, vertex2, vertex6, pMaterial3);
    MakeFaceAndTextureIt(pModel, pFace_index + 11, vertex1, vertex6, vertex5, pMaterial3);
}
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

