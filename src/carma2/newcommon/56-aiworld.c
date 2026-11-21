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

// GLOBAL: CARMA2_HW 0x0067baa0
int gCrush_panel_ids[10];

// GLOBAL: CARMA2_HW 0x00681fb4
int gCount_active_drones;

// GLOBAL: CARMA2_HW 0x006820b8
int gDrones_unmodified;

// GLOBAL: CARMA2_HW 0x006820cc
tU32 gNext_drone_edit_path_munge;

// GLOBAL: CARMA2_HW 0x006820d4
int gINT_006820d4;

// GLOBAL: CARMA2_HW 0x006844fc
int gINT_006844fc;

// GLOBAL: CARMA2_HW 0x0068451c
br_model* gStatic_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684520
br_model* gElasticatey_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684524
br_actor* gStatic_drone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684528
br_actor* gDrone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684530
int gDrone_paths_elasticating;

// GLOBAL: CARMA2_HW 0x00684538
int gStatic_drone_path_model_vertex_capacity;

// GLOBAL: CARMA2_HW 0x0068453c
int gStatic_drone_path_model_face_capacity;

// GLOBAL: CARMA2_HW 0x00691724
int gGrudge_reduction_per_period;

// GLOBAL: CARMA2_HW 0x00691728
tU32 gAcme_frame_count;

// GLOBAL: CARMA2_HW 0x00691754
tU32 gNext_grudge_reduction;

// GLOBAL: CARMA2_HW 0x0079efbc
float gTrack_drone_min_y;

// GLOBAL: CARMA2_HW 0x0079efc0
float gDrone_delta_time;

// GLOBAL: CARMA2_HW 0x0079efc4
tU32 gFrame_period_for_this_munging;

// GLOBAL: CARMA2_HW 0x0058f038
int gFirst_drone_processing;

// GLOBAL: CARMA2_HW 0x00594750
tDrone_form_within_processing_distance_cbfn* gDrone_form_within_processing_distance_functions[4];

// GLOBAL: CARMA2_HW 0x00594790
const char* gDrone_axis_names[4] = { 0 };

// GLOBAL: CARMA2_HW 0x005947a0
const char* gFunk_speed_control_names[2] = { 0 };

// GLOBAL: CARMA2_HW 0x005947a8
const char* gReverseness_type_names[2] = { 0 };

// GLOBAL: CARMA2_HW 0x005947b0
int gSelected_drone_path_node_index;

// GLOBAL: CARMA2_HW 0x005947b4
int gSelected_edit_drone_path;

// GLOBAL: CARMA2_HW 0x005947b8
int gSelected_drone_path_index;

// GLOBAL: CARMA2_HW 0x0065a3c8
int gChallenger_index__opponent;

// GLOBAL: CARMA2_HW 0x00676858
br_material* gMat_dk_yel;

// GLOBAL: CARMA2_HW 0x0067685c
br_material* gMat_md_yel;

// GLOBAL: CARMA2_HW 0x00676860
br_material* gMat_lt_yel;

// GLOBAL: CARMA2_HW 0x00676870
br_material* gMat_dk_grn;

// GLOBAL: CARMA2_HW 0x00676874
br_material* gMat_md_grn;

// GLOBAL: CARMA2_HW 0x00676878
br_material* gMat_lt_grn;

// GLOBAL: CARMA2_HW 0x00676884
br_material* gMat_dk_turq;

// GLOBAL: CARMA2_HW 0x00676888
br_material* gMat_lt_turq;

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}

// STUB: CARMA2_HW 0x00401fe0
void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pResult, br_vector3* pStart, br_vector3* pFinish, br_scalar pWidth) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004020e0
void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044f700
void C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0044f980
void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone_spec) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00450680
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00451710
void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00451bd0
void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00452ba0
int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dde20
void C2_HOOK_FASTCALL MungeShatMines(tU32 pTime) {
    NOT_IMPLEMENTED();
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

// STUB: CARMA2_HW 0x004010b0
void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info) {
    NOT_IMPLEMENTED();
}

// InitialiseAIWorldForRace

// FUNCTION: CARMA2_HW 0x00401160
// FUNCTION: CARMA2_SW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {

    DisposeOpponents();
    DisposeDronesRaceStuff();
}

// STUB: CARMA2_HW 0x00401170
void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period) {
    NOT_IMPLEMENTED();
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

// STUB: CARMA2_HW 0x00401aa0
void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVert_index, int pFace_index, br_vector3* pPosition, br_material* pMaterial1, br_material* pMaterial2, br_material* pMaterial3) {
    NOT_IMPLEMENTED();
}

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

