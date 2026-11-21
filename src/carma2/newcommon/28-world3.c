/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "28-world3.h"

#include "08-loading1.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "51-smash.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"
#include "38-flicplay.h"
#include "33-depth.h"
#include "platform.h"
#include "27-powerup.h"
#include "67-lighting.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x006b75c0
// GLOBAL: CARMA2_SW 0x006ae2a0
br_actor* gAdditional_actors;

// GLOBAL: CARMA2_HW 0x006b7820
// GLOBAL: CARMA2_SW 0x006ae500
tBrender_storage* gStorage_for_callbacks;

// GLOBAL: CARMA2_HW 0x0074caa8
int gTrack_depth_colour_red;

// GLOBAL: CARMA2_HW 0x0074cad0
int gTrack_depth_colour_blue;

// GLOBAL: CARMA2_HW 0x0074cf2c
int gTrack_depth_colour_green;

// GLOBAL: CARMA2_HW 0x0079ec2c
br_angle gSky_image_height;


// GLOBAL: CARMA2_HW 0x0079ec2e
br_angle gSky_image_width;
// GLOBAL: CARMA2_HW 0x0079ec30
br_angle gSky_image_underground;

// GLOBAL: CARMA2_HW 0x0079ef40
tS8 gNon_car_spec_indices[100];

// GLOBAL: CARMA2_HW 0x0079efac
int gMax_count_non_cars;

// GLOBAL: CARMA2_HW 0x0058f230
const char* gPed_form_collision_type_names[9] = { 0 };

// GLOBAL: CARMA2_HW 0x0058f258
const char* gPed_form_bone_joint_type_names[10] = { 0 };

// GLOBAL: CARMA2_HW 0x0058f280
const char* gPed_form_joint_limit_type_names[2] = { 0 };

// GLOBAL: CARMA2_HW 0x0058f288
const char* gPed_personality_grounding_type_names[1] = { 0 };

// GLOBAL: CARMA2_HW 0x0058f290
const char* gPed_remap_axis_choices[6] = { 0 };

// GLOBAL: CARMA2_HW 0x0058f2a8
const br_vector3 g_Ped_x_unit_vector = { 0 };

// GLOBAL: CARMA2_HW 0x0058f2b8
const br_vector3 g_Ped_y_unit_vector = { 0 };

// GLOBAL: CARMA2_HW 0x0058f2c8
const br_vector3 g_Ped_z_unit_vector = { 0 };

// GLOBAL: CARMA2_HW 0x006aaa2c
// GLOBAL: CARMA2_SW 0x006a447c
int gDisallow_duplicates;
// GLOBAL: CARMA2_HW 0x00655bb8
const char* gAxis_names[3] = {
    "x",
    "y",
    "z",
};

// GLOBAL: CARMA2_HW 0x00655bc8
const char* gLollipop_names[3] = {
    "xlollipop",
    "ylollipop",
    "zlollipop",
};

// GLOBAL: CARMA2_HW 0x00655bd8
const char* gGroove_path_names[2] = {
    "straight",
    "circular",
};


// GLOBAL: CARMA2_HW 0x006aaf40
tU8* gTrack_flic_buffer;

// GLOBAL: CARMA2_HW 0x006aaf48
tU32 gTrack_flic_buffer_size;

// GLOBAL: CARMA2_HW 0x006aaf50
tFlic_descriptor gTrack_flic_descriptor;

// GLOBAL: CARMA2_HW 0x006aafc0
tU32 prev_sky_animation;

// GLOBAL: CARMA2_HW 0x006ab948
int gNumber_of_additional_models;

// GLOBAL: CARMA2_HW 0x006b6400
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6600
br_actor* gLast_actor;

// GLOBAL: CARMA2_HW 0x006b6620
br_model* gAdditional_models[1000];

// GLOBAL: CARMA2_HW 0x006aaa20
// GLOBAL: CARMA2_SW 0x006a4470
br_pixelmap* gDuplicate_pixelmap;

// GLOBAL: CARMA2_HW 0x006aaa28
// GLOBAL: CARMA2_SW 0x006a4478
br_material* gDuplicate_material;

// GLOBAL: CARMA2_HW 0x006aaa24
br_model* gDuplicate_model;

// GLOBAL: CARMA2_HW 0x00655bb0
const char* gGroove_nature_names[2] = {
    "constant",
    "distance",
};

// GLOBAL: CARMA2_HW 0x00655be0
const char* gGroove_object_names[4] = {
    "spin",
    "rock",
    "throb",
    "shear",
};

// GLOBAL: CARMA2_HW 0x0065fe88
const char* gSmashable_item_mode_names[6] = { 0 };

// GLOBAL: CARMA2_HW 0x00660cb8
// GLOBAL: CARMA2_SW 0x0065b62c
tRendererShadingType gMaterial_shading_for_callback = kRendererShadingType_Undefined;

// GLOBAL: CARMA2_HW 0x00660e90
const char* gDepth_effect_names[3] = { 0 };

// GLOBAL: CARMA2_HW 0x0068b854
br_actor* gGroove_by_proxy_actor;

// GLOBAL: CARMA2_HW 0x006a3330
int gCapacity_track_smashables;

// FUNCTION: CARMA2_HW 0x00504b30
// FUNCTION: CARMA2_SW 0x005002f0
void C2_HOOK_FASTCALL InitTreeSurgery(void) {
    tPath_name the_path;
    FILE* file;
    int i;

    PathCat(the_path, gApplication_path, "TreeSurgery.TXT");
    file = PFfopen(the_path, "rt");
    if (file != NULL) {
        gTree_surgery_pass1_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass1_count; i++) {
            GetAString(file, gTree_surgery_pass1[i].name);
        }
        gTree_surgery_pass2_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass2_count; i++) {
            GetAString(file, gTree_surgery_pass2[i].original);
            GetAString(file, gTree_surgery_pass2[i].replacement);
        }
#ifdef REC2_FIX_BUGS
        PFfclose(file);
#endif
    } else {
        gTree_surgery_pass1_count = 0;
        gTree_surgery_pass2_count = 0;
    }
}

// ModelIsATree

// PerformTreeSurgery

// PossibleTreeSurgery

// AddExceptionToList

// LoadExceptionsFile

// LoadExceptionsFileForTrack

// FreeExceptions

// GetFaceFlag

// MungeTrackModel

// LoadTrack

// FUNCTION: CARMA2_HW 0x00506e20
// FUNCTION: CARMA2_SW 0x00502270
intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, type, 0x12);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, type_data, 0x5c);
    (void)pArg;

    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}

// RemoveBoundsStructures

// STUB: CARMA2_HW 0x00506c20
// STUB: CARMA2_SW 0x00502070
void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00506e50
// FUNCTION: CARMA2_SW 0x005022a0
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world) {

    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world);
    if (gAdditional_actors != NULL) {
        BrZbsSceneRenderAdd(gAdditional_actors);
    }
}

// STUB: CARMA2_HW 0x00506e80
void C2_HOOK_FASTCALL AnimateSky(void) {
    NOT_IMPLEMENTED();
}

// AnimateSky

// CombineBounds

// CheckNonCarModelBounds

// STUB: CARMA2_HW 0x00404150
void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004047b0
tPed_move* C2_HOOK_FASTCALL FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00404840
tPed_move* C2_HOOK_FASTCALL ReadMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004079a0
tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pName, br_matrix34* pMat) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0040cc50
void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0040cf10
intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0040d1f0
intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* pData) {
    char s[256];
    tTrack_spec* pTrack_spec = pData;

    SetSmashableModel(pActor);
    if (pActor->identifier[0] == '&' && pActor->model == NULL) {
        sprintf(s, "Accessory %s has no model. Well how does it pose? Awful!", pActor->identifier);
        PDFatalError(s);
    }
    if (pActor->identifier[0] == '&' && '0' <= pActor->identifier[1] && pActor->identifier[1] <= '9') {
        int id;
        br_scalar n0;
        br_scalar n1;
        br_scalar n2;

        id = TEXT_TO_NUMBER_4(&pActor->identifier[4]);
        if (id < 0 || id >= pTrack_spec->count_non_cars) {
            return 1;
        }
        n0 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[0]);
        n1 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[1]);
        n2 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[2]);
        if (n0 < 0.999f || n1 < 0.999f || n2 < 0.999f) {
            dr_dprintf("non car was scaled down %s", pActor->identifier);
            pActor->t.t.translate.t.v[0] += 2000.f;
        }
        if (n0 > 1.001f || n1 > 1.001f || n2 > 1.001f) {
            BrVector3InvScale((br_vector3*)pActor->t.t.mat.m[0],
                (br_vector3*)pActor->t.t.mat.m[0],
                (float)sqrt((double)n0));
            BrVector3InvScale((br_vector3*)pActor->t.t.mat.m[1],
                (br_vector3*)pActor->t.t.mat.m[1],
                (float)sqrt((double)n1));
            BrVector3InvScale((br_vector3*)pActor->t.t.mat.m[2],
                (br_vector3*)pActor->t.t.mat.m[2],
                (float)sqrt((double)n2));
            dr_dprintf("non car was scaled up %s", pActor->identifier);
        }
        pTrack_spec->non_car_list[id] = pActor;
        pActor->type_data = NULL;
        return 0;
    }
    if (pActor != NULL && pActor->identifier != NULL && pActor->identifier[1] == '\xa3') {
        int id;

        SmoothificatePowerups(pActor);
        id = TEXT_TO_NUMBER_2(&pActor->identifier[2]);
        if (66 <= id && id < 86) {
            SetSpinningPowerup(pActor, 0);
        } else if (86 <= id && id < 88) {
            SetChangingPowerup(pActor);
        }
    }
    return BrActorEnum(pActor, FindNonCarsCB, pTrack_spec);
}

// STUB: CARMA2_HW 0x0040d530
void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00487ec0
void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00488b00
void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004d2cc0
void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004d3360
void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pCharacter, br_vector3* pDir, br_vector3* pUp) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f0450
void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f0bc0
void C2_HOOK_FASTCALL MungeSmashMaterialNames(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00500d50
// FUNCTION: CARMA2_SW 0x004fc6b0
void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds) {

    pStorage_space->pixelmaps_count = 0;
    pStorage_space->shade_tables_count = 0;
    pStorage_space->materials_count = 0;
    pStorage_space->models_count = 0;
    pStorage_space->sounds_count = 0;
    pStorage_space->flags = pUnknown;
    pStorage_space->max_pixelmaps = pMax_pixelmaps;
    pStorage_space->max_shade_tables = pMax_shade_tables;
    pStorage_space->max_materials = pMax_materials;
    pStorage_space->max_models = pMax_models;
    pStorage_space->max_sounds = pMax_sounds;
    pStorage_space->pixelmaps = BrMemCalloc(pMax_pixelmaps, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->shade_tables = BrMemCalloc(pMax_shade_tables, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->materials = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
    pStorage_space->models = BrMemCalloc(pMax_models, sizeof(br_model*), kMem_stor_space_table);
    pStorage_space->sounds = BrMemCalloc(pMax_sounds, sizeof(int), kMem_stor_space_table);
    pStorage_space->materialProps = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
}

// FUNCTION: CARMA2_HW 0x00500e10
// FUNCTION: CARMA2_SW 0x004fc770
void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {

    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}

// FUNCTION: CARMA2_HW 0x00500e60
// FUNCTION: CARMA2_SW 0x004fc7c0
void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd) {
    int i;
    int move_to;

    for (i = pStart; i < pEnd && i < pStorage_space->materials_count; i++) {
        BrMaterialRemove(pStorage_space->materials[i]);
        BrMaterialFree(pStorage_space->materials[i]);
        RemoveMaterialFromFogification(pStorage_space->materials[i]);
    }
    for (move_to = pStart, i = pEnd; i < pStorage_space->materials_count; ) {
        pStorage_space->materials[move_to++] = pStorage_space->materials[i++];
    }
    pStorage_space->materials_count = move_to;
    if (pStorage_space == &gNet_cars_storage_space) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            tCar_spec* car = gNet_players[i].car;
            if (car != NULL && car->old_material_count >= pEnd) {
                car->old_material_count += pStart - pEnd;
                car->new_material_count += pStart - pEnd;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00500f30
// FUNCTION: CARMA2_SW 0x004fc890
void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space) {
    int i;

    DRS3StopAllOutletSoundsExceptCDA();
    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i] != NULL) {
            BrMapRemove(pStorage_space->pixelmaps[i]);
            BrPixelmapFree(pStorage_space->pixelmaps[i]);
        }
    }
    pStorage_space->pixelmaps_count = 0;
    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i] != NULL) {
            BrTableRemove(pStorage_space->shade_tables[i]);
            BrPixelmapFree(pStorage_space->shade_tables[i]);
        }
    }
    pStorage_space->shade_tables_count = 0;
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i] != NULL) {
            BrMaterialRemove(pStorage_space->materials[i]);
            BrMaterialFree(pStorage_space->materials[i]);
        }
    }
    pStorage_space->materials_count = 0;
    for (i = 0; i < pStorage_space->models_count; i++) {
        if (pStorage_space->models[i] != NULL) {
            BrModelRemove(pStorage_space->models[i]);
            BrModelFree(pStorage_space->models[i]);
        }
    }
    pStorage_space->models_count = 0;
}

// FUNCTION: CARMA2_HW 0x00501020
// FUNCTION: CARMA2_SW 0x004fc980
tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm) {
    int i;

    gDuplicate_pixelmap = NULL;
    if (pStorage_space->pixelmaps_count < pStorage_space->max_pixelmaps) {
        for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
            if (pStorage_space->pixelmaps[i]->identifier != NULL
                    && pThe_pm->identifier != NULL
                    && strcmp(pStorage_space->pixelmaps[i]->identifier, pThe_pm->identifier) == 0) {
                gDuplicate_pixelmap = pStorage_space->pixelmaps[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->pixelmaps[pStorage_space->pixelmaps_count] = pThe_pm;
        pStorage_space->pixelmaps_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x005010e0
// FUNCTION: CARMA2_SW 0x004fca40
tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count < pStorage_space->max_shade_tables) {
        for (i = 0; i < pStorage_space->shade_tables_count; i++) {
            if (pStorage_space->shade_tables[i]->identifier != NULL
                && pThe_st->identifier != NULL
                && strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier) == 0) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
        pStorage_space->shade_tables_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501190
// FUNCTION: CARMA2_SW 0x004fcaf0
tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat) {
    int i;

    gDuplicate_material = NULL;
    if (pStorage_space->materials_count < pStorage_space->max_materials) {
        for (i = 0; i < pStorage_space->materials_count; i++) {
            if (pStorage_space->materials[i]->identifier != NULL
                    && pThe_mat->identifier != NULL
                    && strcmp(pStorage_space->materials[i]->identifier, pThe_mat->identifier) == 0) {
                gDuplicate_material = pStorage_space->materials[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->materialProps[pStorage_space->materials_count] = 0; /* FIXME */
        pStorage_space->materials[pStorage_space->materials_count] = pThe_mat;
        pStorage_space->materials_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501260
// FUNCTION: CARMA2_SW 0x004fcbc0
tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    gDuplicate_model = NULL;
    if (pStorage_space->models_count < pStorage_space->max_models) {
        if (pStorage_space->flags & 0x1) { /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
            for (i = 0; i < pStorage_space->models_count; i++) {
                if (pStorage_space->models[i] != NULL
                    && pStorage_space->models[i]->identifier != NULL
                        && pThe_mod->identifier != NULL
                        && strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier) == 0) {
                    gDuplicate_model = pStorage_space->models[i];
                    return eStorage_duplicate;
                }
            }
        }
        pStorage_space->models[pStorage_space->models_count] = pThe_mod;
        pStorage_space->models_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501330
// FUNCTION: CARMA2_SW 0x004fcc90
tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id) {
    int i;

    if (pStorage_space->sounds_count < pStorage_space->max_sounds) {
        for (i = 0; i < pStorage_space->sounds_count; i++) {
            if (pStorage_space->sounds[i] != 0 && pStorage_space->sounds[i] == pSound_id) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->sounds[pStorage_space->sounds_count] = pSound_id;
        pStorage_space->sounds_count++;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x005024f0
// FUNCTION: CARMA2_SW 0x004fde40
int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int new_ones;
    int total;
    tPath_name path_dirname;
    char path_stem[32];
    br_pixelmap* temp_array[500];

    total = 0;
    if (gDisableTiffConversion) {
        SepDirAndFilename(path, path_dirname, path_stem);
        new_ones = LoadBunchOfPixies(path_dirname, path_stem, temp_array, REC2_ASIZE(temp_array));
    } else {
        new_ones = DRPixelmapLoadMany(path, temp_array, REC2_ASIZE(temp_array));
    }
    if (new_ones == 0) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }
    for (i = 0; i < new_ones; i++) {
        if (temp_array[i] != NULL) {
            EnsurePixelmapAllowed(temp_array[i], 1);
            switch (AddPixelmapToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMapAdd(temp_array[i]);
                total += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
                } else {
                    BrPixelmapFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientPixelmapSlots);
                break;
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501560
// FUNCTION: CARMA2_SW 0x004fcec0
br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName) {
    br_pixelmap* map;
    tAdd_to_storage_result addResult;

    map = LoadPixelmap(pName);
    if (map == NULL) {
        return BrMapFind(pName);
    }

    addResult = AddPixelmapToStorage(pStorage, map);
    switch (addResult) {
    case eStorage_allocated:
        BrMapAdd(map);
        return map;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, map->identifier);
        } else {
            BrPixelmapFree(map);
        }
        return gDuplicate_pixelmap;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientPixelmapSlots);
        break;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005016a0
// FUNCTION: CARMA2_SW 0x004fd000
br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName) {
    br_pixelmap* temp;

    temp = LoadShadeTable(pName);
    if (temp == NULL) {
        return BrTableFind(pName);
    }

    switch (AddShadeTableToStorage(pStorage_space, temp)) {
    case eStorage_allocated:
        BrTableAdd(temp);
        return temp;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, temp->identifier);
        } else {
            BrPixelmapFree(temp);
        }
        return BrTableFind(pName);
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientShadeTableSlots);
        break;
    }

    return NULL;
}

// FUNCTION: CARMA2_HW 0x005017e0
// FUNCTION: CARMA2_SW 0x004fd140
br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName) {
    br_material* material;

    material = LoadMaterial(pName);
    if (material == NULL) {
        return BrMaterialFind(pName);
    }

    switch (AddMaterialToStorage(pStorage_space, material)) {
    case eStorage_allocated:
        BrMaterialAdd(material);
        return material;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicateMaterial_S, material->identifier);
        } else {
            BrMaterialFree(material);
        }
        return gDuplicate_material;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientMaterialSlots);
        return NULL;
    }
#ifdef REC2_FIX_BUGS
    return NULL;
#endif
}

// FUNCTION: CARMA2_HW 0x00501930
// FUNCTION: CARMA2_SW 0x004fd290
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id) {

    if (S3GetBufferDescription(pSound_id) != NULL) {
        return pSound_id;
    }

    switch (AddSoundToStorage(pStorage_space, pSound_id)) {

    case eStorage_duplicate:
        return pSound_id;

    case eStorage_allocated:
        S3LoadSample(pSound_id);
        return pSound_id;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientSoundSlotsInStorageArea);
        break;
    }
    return 0;
}

int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath) {
    br_pixelmap* temp_array[50];
    int total;
    int i;

    total = BrPixelmapLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddShadeTableToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            BrTableAdd(temp_array[i]);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
            } else {
                BrPixelmapFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientShadeTableSlots);
            break;
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501dd0
// FUNCTION: CARMA2_SW 0x004fd730
void C2_HOOK_FASTCALL LoadModelsFromLineInto(tBrender_storage* pStorage_space, FILE* pF) {
    tPath_name the_path;
    char s[256];
    char* str;

    PossibleService();
    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    PathCat(the_path, gApplication_path, "MODELS");
    PathCat(the_path, the_path, str);
    AddModels(pStorage_space, the_path);
}

// FUNCTION: CARMA2_HW 0x00501e40
// FUNCTION: CARMA2_SW 0x004fd7a0
int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath) {
    int i;
    int new_ones;
    int total;
    br_model* temp_array[2000];

    new_ones = 0;
    total = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, total);
    if (total == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateModel_S, temp_array[i]->identifier);
            } else {
                BrModelFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00501fe0
// FUNCTION: CARMA2_SW 0x004fd930
void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage* pStorage, FILE* pFile, tRendererShadingType pShading) {
    tPath_name the_path;
    char s[256];
    char* str;

    PossibleService();
    GetALineAndDontArgue(pFile, s);
    str = strtok(s, "\t ,/");
    PathCat(the_path, gApplication_path, "MATERIAL");
    PathCat(the_path, the_path, str);
    AddMaterials(pStorage, the_path, pShading);
}

// FUNCTION: CARMA2_HW 0x00502060
// FUNCTION: CARMA2_SW 0x004fd9b0
int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading) {
    int i;
    int new_ones;
    int total;
    br_material* temp_array[500];

    new_ones = 0;
    total = BrMaterialLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, pPath);
    }
    GlorifyMaterial(temp_array, total, pShading);
    for (i = 0; i < total; ++i) {
        if (temp_array[i] != NULL) {
            switch (AddMaterialToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMaterialAdd(temp_array[i]);
                new_ones += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicateMaterial_S, temp_array[i]->identifier);
                } else {
                    BrMaterialFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientMaterialSlots);
                break;
            }
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502210
// FUNCTION: CARMA2_SW 0x004fdb60
void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {

    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
}

// FUNCTION: CARMA2_HW 0x005024b0
// FUNCTION: CARMA2_SW 0x004fde00
void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".PIX") != NULL) {
        AddPixelmaps(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502490
// FUNCTION: CARMA2_SW 0x004fdde0
void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}

// FUNCTION: CARMA2_HW 0x005026b0
// FUNCTION: CARMA2_SW 0x004fe000
int C2_HOOK_FASTCALL GetFileName(const char *pPath, void* pContext) {
    tPath_name path;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;
    tName_list *list;

    list = pContext;
    strcpy(path, pPath);
    Uppercaseificate(pathUpper, path);
    if (strstr(pathUpper, ".PIX") == NULL
            && strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    strcpy(list->items[list->size], stem_path);
    if (list->size < (int)REC2_ASIZE(list->items)) {
        list->size += 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00502780
// FUNCTION: CARMA2_SW 0x004fe0d0
int C2_HOOK_FASTCALL GetAdditionalFileName(const char* pPath, void* pContext) {
    tPath_name path;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    int i;
    tName_list* list;

    list = pContext;
    strcpy(path, pPath);
    Uppercaseificate(upperPath, path);

    if (strstr(upperPath, ".PIX") == NULL
            && strstr(upperPath, ".P16") == NULL
            && strstr(upperPath, ".P08") == NULL
            && strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    for (i = 0, alreadyInList = 0; i < list->size; i++) {
        if (strcmp(list->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        strcpy(list->items[list->size], stem_path);
        if (list->size < (int)REC2_ASIZE(list->items)) {
            list->size += 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005028f0
// FUNCTION: CARMA2_SW 0x004fe240
void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name pixPath;
    tPath_name tifPath;
    tName_list list;
    int i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    gStorage_for_callbacks = pStorage_space;
    // TwatPIX16(path);
    list.size = 0;
    strcpy(pathCopy, path);
    if (gDisableTiffConversion) {
        PFForEveryFile2(pathCopy, GetAdditionalFileName, &list);
    }
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFX");
        PFForEveryFile2(tifPath, GetFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX8");
    PFForEveryFile2(pixPath, GetAdditionalFileName, &list);
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tifPath, GetAdditionalFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX16");
    PFForEveryFile2(pixPath, GetAdditionalFileName, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(pathCopy, path, list.items[i]);
        AddPixelmaps(gStorage_for_callbacks, pathCopy);
    }
}

// FUNCTION: CARMA2_HW 0x00502a70
// FUNCTION: CARMA2_SW 0x004fe3c0
void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath) {
    char path[256];
    tRendererShadingType shading;

    if (gMaterial_shading_for_callback == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    } else {
        shading = gMaterial_shading_for_callback;
    }
    Uppercaseificate(path, pPath);
    if (strstr(path, ".MAT") != NULL) {
        AddMaterials(gStorage_for_callbacks, pPath, shading);
    }
}

// FUNCTION: CARMA2_HW 0x00502ad0
// FUNCTION: CARMA2_SW 0x004fe420
void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    gMaterial_shading_for_callback = pShading;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
}

// FUNCTION: CARMA2_HW 0x00502b20
// FUNCTION: CARMA2_SW 0x004fe470
void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b00
// FUNCTION: CARMA2_SW 0x004fe450
void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00502b80
// FUNCTION: CARMA2_SW 0x004fe4d0
void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    char path[256];

    Uppercaseificate(path, pPath);
    if (strstr(path, ".TAB") != NULL) {
        AddShadeTables(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b60
// FUNCTION: CARMA2_SW 0x004fe4b0
void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}

// FUNCTION: CARMA2_HW 0x00502cf0
// FUNCTION: CARMA2_SW 0x004fe640
void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}

// FUNCTION: CARMA2_HW 0x00502d60
// FUNCTION: CARMA2_SW 0x004fe6b0
void C2_HOOK_FASTCALL DisallowDuplicates(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00502d70
// FUNCTION: CARMA2_SW 0x004fe6c0
void C2_HOOK_FASTCALL AllowDuplicates(void) {

    gDisallow_duplicates = 0;
}

// STUB: CARMA2_HW 0x00502d80
br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage) {
    NOT_IMPLEMENTED();
}
