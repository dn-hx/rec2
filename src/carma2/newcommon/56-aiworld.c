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
#include "52-errors.h"
#include "08-loading1.h"
#include "70-packfile.h"
#include "41-utility.h"
#include "rec2_macros.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

// MakeVertexAndOffsetIt

// GLOBAL: CARMA2_HW 0x00594790
const char* gDrone_axis_names[4] = {
    "x",
    "y",
    "z",
    NULL,
};

// GLOBAL: CARMA2_HW 0x005947a0
const char* gFunk_speed_control_names[2] = {
    "controller",
    "dronespeed",
};

// GLOBAL: CARMA2_HW 0x005947a8
const char* gReverseness_type_names[2] = {
    "forward",
    "reverse",
};

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


static int GrooveAxisFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gDrone_axis_names; ppName < &gDrone_axis_names[3]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveReversenessFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gReverseness_type_names; ppName < &gReverseness_type_names[2]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveSpeedControlFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gFunk_speed_control_names; ppName < &gFunk_speed_control_names[2]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int MatchGrooveFunkType(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gGroove_funk_type_names; ppName < &gGroove_funk_type_names[3]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

/* helpers that the original inlines into LoadFunksAndGrooves */
static int ReadPastThisLine(FILE* pF, const char* pLine) {
    char s[256];

    for (;;) {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, pLine) == 0) {
            return 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00450680
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {
    char s[256];
    tFunk_groove* funk_groove;
    tU8 funk_type;
    int axis;
    int reverseness;
    int speed_control;

    if (!ReadPastThisLine(pF, "START OF FUNKYGROOVY STUFF")) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
    }
    if (pDrone->funk_grooves == NULL) {
        pDrone->funk_grooves = BrMemAllocate(sizeof(tFunk_grooves), kMem_drone_funk_groove);
    }
    GetALineAndDontArgue(pF, s);
    while (strcmp(s, "END OF FUNKYGROOVY STUFF") != 0) {
        if (PFfeof(pF)) {
            return;
        }
        if (pDrone->funk_grooves->count >= 10) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
        }
        funk_groove = &pDrone->funk_grooves->items[pDrone->funk_grooves->count];
        funk_type = MatchGrooveFunkType(s);
        funk_groove->type = funk_type;
        switch (funk_type) {
        case eFunk_groove_type_spinny_groove:
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = (br_actor*)DRActorEnumRecurse(pDrone->model_actor, (br_actor_enum_cbfn*)MrFindy, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            GetALineAndDontArgue(pF, s);
            axis = GrooveAxisFromString(s);
            if (axis < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.axis = axis;
            GetALineAndDontArgue(pF, s);
            speed_control = GrooveSpeedControlFromString(s);
            if (speed_control < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.speed_control = speed_control;
            GetALineAndDontArgue(pF, s);
            reverseness = GrooveReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.reverse = reverseness;
            funk_groove->spinny.omega = (float)(GetAScalar(pF) * REC2_PI);
            break;

        case eFunk_groove_type_steering_groove:
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = (br_actor*)DRActorEnumRecurse(pDrone->model_actor, (br_actor_enum_cbfn*)MrFindy, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            GetALineAndDontArgue(pF, s);
            reverseness = GrooveReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->steering.reverse = reverseness;
            break;
        }
        pDrone->funk_grooves->count += 1;
        GetALineAndDontArgue(pF, s);
    }
}