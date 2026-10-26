#include "28-world3.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "50-fog.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <math.h>
#include <string.h>
#include "platform.h"

// GLOBAL: CARMA2_HW 0x006b75c0
br_actor* gAdditional_actors;

/* gStorage_for_callbacks (0x006b7820) is defined in 68-timer.c, adjacent to gTimers as in the original layout */

// GLOBAL: CARMA2_HW 0x006aaa2c
int gDisallow_duplicates;

// GLOBAL: CARMA2_HW 0x006aaa20
br_pixelmap* gDuplicate_pixelmap;

// GLOBAL: CARMA2_HW 0x006aaa28
br_material* gDuplicate_material;

// GLOBAL: CARMA2_HW 0x006aaa24
br_model* gDuplicate_model;

// GLOBAL: CARMA2_HW 0x00660cb8
tRendererShadingType gMaterial_shading_for_callback = kRendererShadingType_Undefined;

// FUNCTION: CARMA2_HW 0x00504b30
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
intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {

    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}
// RemoveBoundsStructures

// FreeTrack

extern void C2_HOOK_FASTCALL RenderTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world);

// FUNCTION: CARMA2_HW 0x00506e50
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {

    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world_transform);
    if (gAdditional_actors) {
        BrZbsSceneRenderAdd(gAdditional_actors);
    }
}

// GLOBAL: CARMA2_HW 0x006aaf40
tU8* gTrack_flic_buffer;

// GLOBAL: CARMA2_HW 0x006aaf48
tU32 gTrack_flic_buffer_size;

// GLOBAL: CARMA2_HW 0x006aaf50
tFlic_descriptor gTrack_flic_descriptor;

// GLOBAL: CARMA2_HW 0x006aafc0
static tU32 prev_sky_animation;

extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info);
extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);
extern int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);
extern void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap);
extern void C2_HOOK_FASTCALL SkyTextureChanged(void);

// FUNCTION: CARMA2_HW 0x00506e80
void C2_HOOK_FASTCALL AnimateSky(void) {
    int now;
    int now2;
    int frame_increment;

    now2 = GetTotalTime();
    if (gTrack_flic_buffer == NULL) {
        return;
    }
    now = now2;
    if (now < (int)prev_sky_animation) {
        now2 = 2 * prev_sky_animation - now;
    }
    if (prev_sky_animation != 0) {
        frame_increment = ((now2 - prev_sky_animation) / gTrack_flic_descriptor.frame_period) % (gTrack_flic_descriptor.frames_left + gTrack_flic_descriptor.current_frame);
    } else {
        frame_increment = 1;
    }
    for (; frame_increment > 0; frame_increment--) {
        if (PlayNextFlicFrame(&gTrack_flic_descriptor)) {
            EndFlic(&gTrack_flic_descriptor);
            StartFlic(gTrack_flic_descriptor.file_name, -1, &gTrack_flic_descriptor, gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
            AssertFlicPixelmap(&gTrack_flic_descriptor, gProgram_state.default_depth_effect.sky_texture);
        }
        prev_sky_animation = now;
        SkyTextureChanged();
    }
}

// CombineBounds

// CheckNonCarModelBounds

// FUNCTION: CARMA2_HW 0x00500d50
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
void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {

    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}

// FUNCTION: CARMA2_HW 0x00500e60
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

// FUNCTION: CARMA2_HW 0x00501e40
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

// FUNCTION: CARMA2_HW 0x00502060
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
void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {

    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
}

// FUNCTION: CARMA2_HW 0x005024b0
void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".PIX") != NULL) {
        AddPixelmaps(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502490
void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}

// FUNCTION: CARMA2_HW 0x005026b0
int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList) {
    tPath_name pathCopy;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;

    strcpy(pathCopy, path);
    Uppercaseificate(pathUpper, pathCopy);
    if (strstr(pathUpper, ".PIX") == NULL
            && strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    strcpy(pList->items[pList->size], stem_path);
    if (pList->size < (int)REC2_ASIZE(pList->items)) {
        pList->size += 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00502780
int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList) {
    tPath_name pathCopy;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    int i;

    strcpy(pathCopy, path);
    Uppercaseificate(upperPath, pathCopy);

    if (strstr(upperPath, ".PIX") == NULL
            && strstr(upperPath, ".P16") == NULL
            && strstr(upperPath, ".P08") == NULL
            && strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    for (i = 0, alreadyInList = 0; i < pList->size; i++) {
        if (strcmp(pList->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        strcpy(pList->items[pList->size], stem_path);
        if (pList->size < (int)REC2_ASIZE(pList->items)) {
            pList->size += 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005028f0
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
        PFForEveryFile2(pathCopy, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFX");
        PFForEveryFile2(tifPath, (tEnumPathCallback)GetFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX8");
    PFForEveryFile2(pixPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tifPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX16");
    PFForEveryFile2(pixPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(pathCopy, path, list.items[i]);
        AddPixelmaps(gStorage_for_callbacks, pathCopy);
    }
}

// FUNCTION: CARMA2_HW 0x00502a70
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
void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    gMaterial_shading_for_callback = pShading;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
}

// FUNCTION: CARMA2_HW 0x00502b20
void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b00
void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00502b80
void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    char path[256];

    Uppercaseificate(path, pPath);
    if (strstr(path, ".TAB") != NULL) {
        AddShadeTables(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b60
void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}

// FUNCTION: CARMA2_HW 0x00502cf0
void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}

// FUNCTION: CARMA2_HW 0x00502d60
void C2_HOOK_FASTCALL DisallowDuplicates(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00502d70
void C2_HOOK_FASTCALL AllowDuplicates(void) {

    gDisallow_duplicates = 0;
}

// FUNCTION: CARMA2_HW 0x00502d80
br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {
        if (pOriginal == pStorage->pixelmaps[i]) {
            BrMapRemove(pOriginal);
            BrPixelmapFree(pOriginal);
            pStorage->pixelmaps[i] = pNew;
            BrMapAdd(pNew);
            return pNew;
        }
    }
    PDFatalError("Can't swap in pixelmap");
    return NULL;
}

// GLOBAL: CARMA2_HW 0x00660e90
const char* gDepth_effect_names[3] = {
    "dark",
    "fog",
    "colour",
};

// GLOBAL: CARMA2_HW 0x00761a80
char gCurrent_load_name[256];

// GLOBAL: CARMA2_HW 0x006b6400
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6620
br_model* gAdditional_models[1000];

// GLOBAL: CARMA2_HW 0x006ab948
int gNumber_of_additional_models;

// GLOBAL: CARMA2_HW 0x006b6600
br_actor* gLast_actor;

// GLOBAL: CARMA2_HW 0x0079ec2e
br_angle gSky_image_width;

// GLOBAL: CARMA2_HW 0x0079ec2c
br_angle gSky_image_height;

// GLOBAL: CARMA2_HW 0x0079ec30
br_angle gSky_image_underground;

// GLOBAL: CARMA2_HW 0x0074caa8
int gTrack_depth_colour_red;

// GLOBAL: CARMA2_HW 0x0074cf2c
int gTrack_depth_colour_green;

// GLOBAL: CARMA2_HW 0x0074cad0
int gTrack_depth_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ef40
tS8 gNon_car_spec_indices[100];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004f0450
void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath) {
    extern int gCount_smashable_noncars;
    extern int gCount_smashable_noncar_shrapnel_actors;
    extern tSmashable_environment_name* gSmashable_noncars;
    extern int gCount_track_smashable_environment_specs;
    extern int gCount_track_smashable_environment_specs_2;
    extern int gCapacity_track_smashables;
    extern tSmashable_item_spec* gTrack_smashable_environment_specs;
    extern const char* gSmashable_track_environment_path;
    extern tSmashable_item_spec* gSmashable_track_indexable_triggers[100];
    extern const char* gSmashable_item_mode_names[6];
    extern const char* gRepeatability_names[4];
    extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
    extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);
    extern void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count);
    extern void C2_HOOK_FASTCALL ReadExplosion(FILE* pF, tExplosion_animation* pAnimation);
    extern void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick);
    extern void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);
    extern void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
    extern void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pLevels, int* pCount, int pIs_texture_change, tBrender_storage* pStorage);
    int i;
    tSmashable_item_spec* spec;
    int d1, d2;
    int j;
    int* snd;
    tSmash_run_time_variable_change* chg;
    char s[256];
    char s2[256];

    gCount_smashable_noncars = 0;
    gCount_smashable_noncar_shrapnel_actors = 0;
    gSmashable_noncars = BrMemAllocate(100 * sizeof(tSmashable_environment_name), kMem_smashable_env_info);

    gCount_track_smashable_environment_specs = GetAnInt(pF);
    gCount_track_smashable_environment_specs_2 = gCount_track_smashable_environment_specs;
    gCapacity_track_smashables = 5 * (gCount_track_smashable_environment_specs + 30);
    gTrack_smashable_environment_specs = BrMemAllocate(gCapacity_track_smashables * sizeof(tSmashable_item_spec), kMem_smashable_env_info);
    gSmashable_track_environment_path = pPath;
    memset(gSmashable_track_indexable_triggers, 0, sizeof(gSmashable_track_indexable_triggers));

    spec = gTrack_smashable_environment_specs;
    for (i = 0; i < gCount_track_smashable_environment_specs; i++, spec++) {

        spec->flags = GetAnInt(pF);
        spec->field_0x10 = 0;

        GetAString(pF, s);
        if (s[0] == '&') {
            if (s[1] >= '0' && s[1] <= '9' && s[3] == '\0') {
                spec->trigger_type = kSmashableTrigger_Number;
            } else {
                spec->trigger_type = kSmashableTrigger_Model;
            }
        } else {
            if (strstr(s, ".DAT") != NULL || strstr(s, ".dat") != NULL || strstr(s, ".ACT") != NULL || strstr(s, ".act") != NULL) {
                spec->trigger_type = kSmashableTrigger_Model;
            } else {
                spec->trigger_type = kSmashableTrigger_Material;
            }
        }

        switch (spec->trigger_type) {
        case kSmashableTrigger_Material:
            spec->trigger_object.material = BrMaterialFind(s);
            if (spec->trigger_object.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            break;
        case kSmashableTrigger_Model:
            spec->trigger_object.model = BrModelFind(s);
            if (spec->trigger_object.model == NULL) {
                FatalError(kFatalError_CannotFindSmashModel_S, s);
            }
            break;
        case kSmashableTrigger_Number:
            spec->trigger_object.number.field_0x0 = (s[1] + 24) * 10 + s[2];
            spec->trigger_object.number.field_0x1 = GetAnInt(pF);
            gSmashable_track_indexable_triggers[spec->trigger_object.number.field_0x0] = spec;
            break;
        }

        spec->mode = GetALineAndInterpretCommand(pF, gSmashable_item_mode_names, REC2_ASIZE(gSmashable_item_mode_names));
        if (spec->mode == kSmashableMode_Decal || spec->mode == kSmashableMode_TextureChange) {
            spec->mode_data.texture_change.levels = NULL;
            strcpy(spec->mode_data.texture_change.undefined_0x0, s2);
            spec->mode_data.texture_change.field_0x40 = -1;
            spec->mode_data.texture_change.trigger = spec->trigger_object;
            spec->mode_data.texture_change.field_0x4c = 0;
            spec->mode_data.texture_change.field_0x40 = -1;
            spec->mode_data.texture_change.field_0x6c = 0;
            spec->mode_data.texture_change.field_0x70 = 0;
            spec->mode_data.texture_change.field_0x74 = 0;
            spec->mode_data.texture_change.field_0x78 = 0;
            spec->mode_data.texture_change.field_0x54 = 0.f;
            spec->mode_data.texture_change.field_0x58 = 0.f;
            spec->mode_data.texture_change.field_0x5c = 0.f;
            spec->mode_data.texture_change.field_0x60 = 1.f;
            spec->mode_data.texture_change.field_0x64 = 0.f;
            spec->mode_data.texture_change.field_0x68 = 0.f;
            LoadSmashableLevels(pF, &spec->mode_data.texture_change.levels, &spec->mode_data.texture_change.count_levels, spec->mode == kSmashableMode_TextureChange, &gTrack_storage_space);
        } else {
            spec->mode_data.shrapnel.removal_threshold = GetAScalar(pF);
            spec->mode_data.shrapnel.connotations.count_sounds = GetAnInt(pF);
            for (j = 0, snd = spec->mode_data.shrapnel.connotations.sounds; j < spec->mode_data.shrapnel.connotations.count_sounds; j++, snd++) {
                *snd = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
            }
            ReadShrapnelSpec(pF, spec->mode_data.shrapnel.connotations.shrapnel, &spec->mode_data.shrapnel.connotations.count_shrapnel);
            ReadExplosion(pF, &spec->mode_data.shrapnel.connotations.special_effects.explosion_animation);
            ReadSpillData(pF, &spec->mode_data.shrapnel.connotations.special_effects.slick);
            ReadNonCarCuboidActivation(pF, &spec->mode_data.shrapnel.connotations.side_effects.activations);
            ReadShrapnelSideEffects(pF, &spec->mode_data.shrapnel.connotations.side_effects.side_effects);
            spec->mode_data.shrapnel.connotations.side_effects.extension_flags = GetAnInt(pF);
            if (spec->mode_data.shrapnel.connotations.side_effects.extension_flags & 0x1) {
                spec->mode_data.shrapnel.connotations.side_effects.extension_arg = GetAnInt(pF);
            }
            spec->mode_data.shrapnel.connotations.side_effects.room_turn_on_code = GetAnInt(pF);
            spec->mode_data.shrapnel.connotations.award.frequency = GetALineAndInterpretCommand(pF, gRepeatability_names, REC2_ASIZE(gRepeatability_names));
            if (spec->mode_data.shrapnel.connotations.award.frequency != kRepeatability_None) {
                spec->mode_data.shrapnel.connotations.award.field_0x4 = GetAScalar(pF);
                spec->mode_data.shrapnel.connotations.award.field_0x8 = GetAScalar(pF);
                spec->mode_data.shrapnel.connotations.award.field_0xc = GetAnInt(pF);
                spec->mode_data.shrapnel.connotations.award.field_0x10 = GetAnInt(pF);
            }
            spec->mode_data.shrapnel.connotations.variable_changes.count = GetAnInt(pF);
            for (j = 0, chg = spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes; j < spec->mode_data.shrapnel.connotations.variable_changes.count; j++, chg++) {
                GetPairOfInts(pF, &d1, &d2);
                chg->field_0x2 = d1;
                chg->field_0x0 = d2;
            }
        }
        if (spec->mode == kSmashableMode_ReplaceModel) {
            GetAString(pF, s2);
            spec->replace.model = BrModelFind(s2);
            if (spec->replace.model == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s2);
            }
            spec->replace.smoke_fire_chance.chance_fire = GetAnInt(pF);
            if (spec->replace.smoke_fire_chance.chance_fire != 0) {
                spec->replace.smoke_fire_chance.model_1_int = GetAnInt(pF);
                GetPairOfInts(pF, &spec->replace.smoke_fire_chance.model_2_int, &spec->replace.smoke_fire_chance.model_3_int);
            }
        }
        GetAnInt(pF);
        GetAnInt(pF);
        GetAnInt(pF);
        GetAnInt(pF);

        spec->field_0x2d4 = 0;
        spec->field_0x2d8 = 0;
        spec->field_0x2dc = 0;
    }
    for (i = gCount_track_smashable_environment_specs; i < gCapacity_track_smashables; i++) {
        gTrack_smashable_environment_specs[i].trigger_type = -1;
    }
}

extern br_actor* gMr_blendy;
extern void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel);
extern int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material* pMaterial);
extern void C2_HOOK_CDECL RelinkActorKeepWorldTransform(br_actor* pParent, br_actor* pActor);
extern br_material* gDefault_track_material;
extern void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel);
extern void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea1, int* pArea2);
extern void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor);
extern intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* data);

/* based on carpocalypse2 src/carma2/common/brucetrk.c:255 */
// FUNCTION: CARMA2_HW 0x0040cf10
intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data) {
    unsigned int x;
    unsigned int z;
    br_model* model;
    tTrack_spec* pTrack_spec = data;

    if (sscanf(pActor->identifier, "%u%u", &x, &z) == 2 && x < pTrack_spec->ncolumns_x && z < pTrack_spec->ncolumns_z) {
        pActor->material = gDefault_track_material;
        pTrack_spec->columns[z][x].actor_0x0 = BrActorAllocate(BR_ACTOR_NONE, NULL);
        BrActorAdd(pActor->parent, pTrack_spec->columns[z][x].actor_0x0);
        pTrack_spec->columns[z][x].actor_0x4 = pActor;
        pTrack_spec->columns[z][x].actor_0x8 = NULL;
        RelinkActorKeepWorldTransform(pTrack_spec->columns[z][x].actor_0x0, pActor);
        gMr_blendy = NULL;
        if (pActor->model != NULL && !gAusterity_mode) {
            MungeFaces(pActor, pActor->model);
        } else {
            br_actor* child;

            for (child = pActor->children; child != NULL; child = child->next) {
                if (child->identifier[0] != '&') {
                    break;
                }
            }
            if (child != NULL) {
                MungeFaces(child, child->model);
            }
        }
        BrActorEnum(pActor, FindNonCarsCB, pTrack_spec);
        if (gMr_blendy != NULL) {
            int area2;
            int area1;
            int nverts;
            int nfaces;
            br_vertex* new_vertices;
            br_face* new_faces;

            model = gMr_blendy->model;
            BrActorAdd(pTrack_spec->columns[z][x].actor_0x0, gMr_blendy);
            gMr_blendy->model->flags |= BR_MODU_FACES;
            BrModelAdd(gMr_blendy->model);
            GetModelTextureArea(gMr_blendy->model, &area1, &area2);

            nverts = model->nvertices + area2;
            nfaces = model->nfaces + area1;
            new_vertices = BrResAllocate(gMr_blendy->model, nverts * sizeof(br_vertex), BR_MEMORY_VERTICES);
            memcpy(new_vertices, gMr_blendy->model->vertices, gMr_blendy->model->nvertices * sizeof(br_vertex));
            BrResFree(gMr_blendy->model->vertices);
            gMr_blendy->model->vertices = new_vertices;

            new_faces = BrResAllocate(gMr_blendy->model, nfaces * sizeof(br_face), BR_MEMORY_FACES);
            memcpy(new_faces, gMr_blendy->model->faces, gMr_blendy->model->nfaces * sizeof(br_face));
            BrResFree(gMr_blendy->model->faces);
            gMr_blendy->model->faces = new_faces;

            DRModelUpdateAndKevificateMaterials(gMr_blendy->model, BR_MODU_ALL);
            pTrack_spec->columns[z][x].actor_0x8 = gMr_blendy;
        }
    } else {
        BrActorEnum(pActor, ProcessModelsCB, pTrack_spec);
    }
    SetSmashableModel(pActor);
    return 0;
}

/* based on carpocalypse2 src/carma2/common/brucetrk.c:352 */
// FUNCTION: CARMA2_HW 0x0040cc50
void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec) {
    extern int gMax_count_non_cars;
    extern int gCount_track_non_cars;
    const char* identifier;
    char s[256];
    br_bounds bounds;
    int ad;
    unsigned int z;
    unsigned int x;
    br_scalar extra_room;
    br_scalar extra;
    int is_old;
    tU16 row;

    identifier = pTrack_spec->the_actor->identifier;
    is_old = 0;
    if (identifier[0] == 'P' && identifier[1] == 'P') {
        if (identifier[3] != '1') {
            sprintf(s, "The World has officially ended: Wrong format of track!!! '");
            strncat(s, pTrack_spec->the_actor->identifier, 4);
            strcat(s, "'");
            PDFatalError(s);
        }
        identifier = &identifier[5];
    }

    switch (sscanf(identifier, "%u%u%f%d", &x, &z, &extra_room, &ad)) {
    case 3:
        BrFailure(
            "Attempt to extract columns from invalid track\n"
            "(It might have been produced by an ancient preproc.\n"
            "This is no longer supported.\n");
        break;
    case 4:
        pTrack_spec->count_non_cars = ad;
        break;
    default:
        x = 1;
        z = 1;
        extra_room = 0.f;
        is_old = 1;
        pTrack_spec->count_non_cars = 0;
        break;
    }

    pTrack_spec->ncolumns_x = x;
    pTrack_spec->ncolumns_z = z;

    extra = extra_room;
    BrActorToBounds(&bounds, pTrack_spec->the_actor);
    pTrack_spec->column_size_x = (bounds.max.v[0] - bounds.min.v[0] + 2.f * extra) / pTrack_spec->ncolumns_x;
    pTrack_spec->column_size_z = (bounds.max.v[2] - bounds.min.v[2] + 2.f * extra) / pTrack_spec->ncolumns_z;
    pTrack_spec->origin_x = bounds.min.v[0] - extra;
    pTrack_spec->origin_z = bounds.min.v[2] - extra;

    pTrack_spec->columns = BrMemAllocate(pTrack_spec->ncolumns_z * sizeof(tTrack_square*), kMem_columns_z);
    for (row = 0; row != pTrack_spec->ncolumns_z; row++) {
        pTrack_spec->columns[row] = BrMemAllocate(pTrack_spec->ncolumns_x * sizeof(tTrack_square), kMem_columns_x);
        memset(pTrack_spec->columns[row], 0, pTrack_spec->ncolumns_x * sizeof(tTrack_square));
    }

    gMax_count_non_cars = pTrack_spec->count_non_cars + 250;
    gCount_track_non_cars = pTrack_spec->count_non_cars;
    pTrack_spec->non_car_list = BrMemAllocate(gMax_count_non_cars * sizeof(br_actor*), kMem_non_car_list);
    if (is_old) {
        pTrack_spec->columns[0][0].actor_0x0 = pTrack_spec->the_actor;
    } else {
        BrActorEnum(pTrack_spec->the_actor, ProcessModelsCB, pTrack_spec);
    }

    {
        int i;
        int count_nulls;
        int count_non_nulls;
        br_actor** non_cars;

        non_cars = pTrack_spec->non_car_list;
        count_nulls = 0;
        count_non_nulls = 0;
        for (i = 0; i < pTrack_spec->count_non_cars; i++) {
            if (pTrack_spec->non_car_list[i] == NULL) {
                count_nulls += 1;
            } else {
                non_cars[count_non_nulls] = pTrack_spec->non_car_list[i];
                sprintf(&non_cars[count_non_nulls]->identifier[4], "%04d", count_non_nulls);
                count_non_nulls += 1;
            }
        }
        gMax_count_non_cars -= count_nulls;
        gCount_track_non_cars -= count_nulls;
        pTrack_spec->count_non_cars -= count_nulls;
    }
}

/* based on carpocalypse2 src/carma2/common/pedestrn.c:2113 */
// FUNCTION: CARMA2_HW 0x004d3520
void C2_HOOK_FASTCALL FinishUpLoadingPeds(void) {
    extern int gPed_count;
    extern tPedestrian* gPedestrian_array;
    tPedestrian* new_pedestrians;
    int i;

    dr_dprintf("%d pedestrians, %d bytes each, total = %d bytes\n", gPed_count, sizeof(tRace_pedestrian), gPed_count * sizeof(tRace_pedestrian));
    new_pedestrians = BrMemAllocate(gPed_count * sizeof(tPedestrian), kMem_ped_array);
    memcpy(new_pedestrians, gPedestrian_array, gPed_count * sizeof(tPedestrian));
    BrMemFree(gPedestrian_array);
    gPedestrian_array = new_pedestrians;
    for (i = 0; i < gPed_count; i++) {
        gPedestrian_array[i].character->ped = &gPedestrian_array[i];
    }
}

// STUB: CARMA2_HW 0x00474ac0
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00476470
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {
    NOT_IMPLEMENTED();
}

/* based on carpocalypse2 src/carma2/common/smashing.c:190 (inlined into MungeSmashMaterialNames in the original) */
static __inline void SplondificatalizeIdentifier(br_material* pMaterial, char** ppIdentifier, int pIndex) {
    int len;
    char original_suffix[64];
    char new_identifier[64];
    char* suffix_ptr;

    strcpy(new_identifier, *ppIdentifier);
    len = strlen(new_identifier);
    if (len < 4) {
        sprintf(new_identifier, "Smash material %s has a name that is less than 4 characters long", *ppIdentifier);
        PDFatalError(new_identifier);
    }
    suffix_ptr = new_identifier + len - 4;
    strcpy(original_suffix, suffix_ptr);
    sprintf(suffix_ptr, "        ");
    sprintf(&new_identifier[5], "%cx", '|');
    new_identifier[6] = (char)(pIndex + 1);
    strcat(new_identifier, original_suffix);
    new_identifier[11] = '\0';
    if (strlen(*ppIdentifier) < 12) {
        BrResFree(*ppIdentifier);
        *ppIdentifier = BrResStrDup(pMaterial, new_identifier);
    } else {
        strcpy(*ppIdentifier, new_identifier);
    }
}

/* based on carpocalypse2 src/carma2/common/smashing.c:215 */
// FUNCTION: CARMA2_HW 0x004f0bc0
void C2_HOOK_FASTCALL MungeSmashMaterialNames(void) {
    extern tSmashable_item_spec* gTrack_smashable_environment_specs;
    extern int gCount_track_smashable_environment_specs;
    int i;
    tSmashable_item_spec* item_spec;

    i = 0;
    if (i < gCount_track_smashable_environment_specs) {
        item_spec = gTrack_smashable_environment_specs;
        do {
            if (item_spec->trigger_type == kSmashableTrigger_Material) {
                SplondificatalizeIdentifier(
                    item_spec->trigger_object.material,
                    &item_spec->trigger_object.material->identifier,
                    i);
            }
            i++;
            item_spec++;
        } while (i < gCount_track_smashable_environment_specs);
    }
}

/* based on carpocalypse2 src/carma2/common/car.c (ReadNonCarMechanicsData) */
// FUNCTION: CARMA2_HW 0x00487ec0
void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec) {
    extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);
    extern void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF);
    extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
    extern br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type);
    int index_version;
    int extra_point_num;
    int i;
    br_scalar len, wid, het;
    br_scalar snap_angle;
    br_scalar ts;
    br_vector3 attached_cmpos;

    pNon_car_spec->driver = eDriver_non_car_unused_slot;

    /* Carmageddon 1: number of non car */
    /* Carmageddon 2: version number */
    index_version = GetAnInt(pF);
    if (index_version < 100) {
        br_vector3 attached_cmpos;
        br_bounds3 bounds;

        pNon_car_spec->index = index_version;

        /* centre of mass position */
        GetThreeFloats(pF,
            &pNon_car_spec->collision_info->cmpos.v[0],
            &pNon_car_spec->collision_info->cmpos.v[1],
            &pNon_car_spec->collision_info->cmpos.v[2]);

        /* centre of mass position when attached */
        GetThreeFloats(pF,
            &attached_cmpos.v[0],
            &attached_cmpos.v[1],
            &attached_cmpos.v[2]);

        /* min x, min y, min z */
        GetThreeFloats(pF, &bounds.min.v[0], &bounds.min.v[1], &bounds.min.v[2]);
        /* max x, max y, max z */
        GetThreeFloats(pF, &bounds.max.v[0], &bounds.max.v[1], &bounds.max.v[2]);

        /* number of extra points */
        extra_point_num = GetAnInt(pF);
        if (extra_point_num > 6) {
            FatalError(kFatalError_TooManyExtraPointsForCarIndex_S, index_version);
        }
        for (i = 0; i < extra_point_num; i++) {
            br_vector3 tmp;

            GetThreeFloats(pF, &tmp.v[0], &tmp.v[1], &tmp.v[2]);
        }

        /* mass in tonnes */
        GetPairOfFloats(pF, &pNon_car_spec->free_mass, &pNon_car_spec->attached_mass);

        /* am length, width, height */
        GetThreeFloats(pF, &len, &wid, &het);

        /* bend angle before snapping */
        snap_angle = GetAFloat(pF);
        pNon_car_spec->snap_off_cosine = cosf(BrAngleToRadian(BrDegreeToAngle(snap_angle)));
        pNon_car_spec->break_off_radians_squared = snap_angle * 3.14f / 180.f * (snap_angle * 3.14f / 180.f);

        /* torque (KN m) needed to move object */
        ts = GetAFloat(pF);
        pNon_car_spec->min_torque_squared = REC2_SQR(ts / WORLD_SCALE);

        BrVector3Set(&pNon_car_spec->I_over_M,
            (het * het + wid * wid) / 12.f,
            (het * het + len * len) / 12.f,
            (wid * wid + len * len) / 12.f);

        pNon_car_spec->collision_info->shape = BrMemAllocate(sizeof(tCollision_shape_polyhedron), kMem_collision_shape);
        memcpy(&pNon_car_spec->collision_info->shape->polyhedron.common.bb, &bounds, sizeof(br_bounds3));
        if (snap_angle != 0.f) {
            pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
            pNon_car_spec->collision_info->physics_joint2->type = eJoint_ball_n_socket;
            BrVector3Copy(&pNon_car_spec->collision_info->physics_joint2->field_0x08, &attached_cmpos);
            pNon_car_spec->collision_info->physics_joint2->count_limits = 1;
            pNon_car_spec->collision_info->physics_joint2->limits[0].type = eJoint_limit_10;
            pNon_car_spec->collision_info->physics_joint2->limits[0].value = pNon_car_spec->min_torque_squared;
        }
        UpdateCollisionObject(pNon_car_spec->collision_info);
        pNon_car_spec->collision_info->actor = pNon_car_spec->actor;
        pNon_car_spec->collision_info->bb2 = pNon_car_spec->collision_info->bb1;
    } else {
        /* non car number */
        pNon_car_spec->index = (int)GetAScalar(pF);

        /* centre of mass position */
        GetThreeFloats(pF,
            &pNon_car_spec->collision_info->cmpos.v[0],
            &pNon_car_spec->collision_info->cmpos.v[1],
            &pNon_car_spec->collision_info->cmpos.v[2]);

        /* centre of mass position when attached */
        GetThreeFloats(pF,
            &attached_cmpos.v[0],
            &attached_cmpos.v[1],
            &attached_cmpos.v[2]);

        ReadMechanicsShapes(&pNon_car_spec->collision_info->shape, pF);

        /* mass unattached, mass attached */
        GetPairOfFloats(pF, &pNon_car_spec->free_mass, &pNon_car_spec->attached_mass);

        /* am width height and length */
        GetThreeFloats(pF, &len, &wid, &het);

        /* bend angle before snapping */
        snap_angle = GetAFloat(pF);
        pNon_car_spec->snap_off_cosine = cosf(BrAngleToRadian(BrDegreeToAngle(snap_angle)));
        pNon_car_spec->break_off_radians_squared = snap_angle * 3.14f / 180.f * (snap_angle * 3.14f / 180.f);

        /* torque (KN m) needed to move object */
        ts = GetAFloat(pF);
        pNon_car_spec->min_torque_squared = REC2_SQR(ts / WORLD_SCALE);

        BrVector3Set(&pNon_car_spec->I_over_M,
            (het * het + wid * wid) / 12.f,
            (het * het + len * len) / 12.f,
            (wid * wid + len * len) / 12.f);

        if (snap_angle != 0.f) {
            pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
            pNon_car_spec->collision_info->physics_joint2->type = eJoint_ball_n_socket;
            BrVector3Copy(&pNon_car_spec->collision_info->physics_joint2->field_0x08, &attached_cmpos);
            pNon_car_spec->collision_info->physics_joint2->count_limits = 1;
            pNon_car_spec->collision_info->physics_joint2->limits[0].type = eJoint_limit_10;
            pNon_car_spec->collision_info->physics_joint2->limits[0].value = pNon_car_spec->min_torque_squared;
        }
        UpdateCollisionObject(pNon_car_spec->collision_info);
        pNon_car_spec->collision_info->actor = pNon_car_spec->actor;
        pNon_car_spec->collision_info->bb2 = pNon_car_spec->collision_info->bb1;

        /* Materials for shrapnel */
        pNon_car_spec->count_shrapnel_materials = GetAnInt(pF);

        for (i = 0; i < pNon_car_spec->count_shrapnel_materials; i++) {
            char s[256];

            GetALineAndDontArgue(pF, s);
            pNon_car_spec->shrapnel_materials[i] = GetSimpleMaterial(s, (pNon_car_spec != NULL && pNon_car_spec->driver >= 6) ? kRendererShadingType_Specular : kRendererShadingType_Diffuse1);
        }
        pNon_car_spec->collision_info->object_friction = 0.4f;
        if (index_version > 100) {
            for (;;) {
                char s[256];

                GetAString(pF, s);
                if (DRStricmp(s, "END") == 0) {
                    break;
                } else if (DRStricmp(s, "WORLD_FRICTION") == 0) {
                    pNon_car_spec->collision_info->world_friction = GetAScalar(pF) - 1.f;
                } else if (DRStricmp(s, "OBJECT_FRICTION") == 0) {
                    pNon_car_spec->collision_info->object_friction = GetAScalar(pF) - 1.f;
                } else if (DRStricmp(s, "TUMBLE") == 0) {
                    pNon_car_spec->tumble_factor = GetAScalar(pF);
                    pNon_car_spec->tumble_threshold = GetAScalar(pF);
                } else if (DRStricmp(s, "WORLD_BALL_JOINT") == 0) {
                    br_vector3 tv;
                    br_scalar friction;
                    int count_limits;

                    GetThreeFloats(pF, &tv.v[0], &tv.v[1], &tv.v[2]);

                    /* friction */
                    friction = GetAScalar(pF);

                    /* num limits */
                    count_limits = GetAnInt(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    pNon_car_spec->field_0xf0->friction = friction;
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &tv);
                    pNon_car_spec->field_0xf0->type = eJoint_ball_n_socket;
                } else if (DRStricmp(s, "WORLD_HINGE_JOINT") == 0) {
                    br_vector3 tv1;
                    br_vector3 tv2;
                    br_scalar friction;
                    int count_limits;

                    GetThreeFloats(pF, &tv1.v[0], &tv1.v[1], &tv1.v[2]);
                    GetThreeFloats(pF, &tv2.v[0], &tv2.v[1], &tv2.v[2]);

                    /* friction */
                    friction = GetAScalar(pF);

                    /* num limits */
                    count_limits = GetAnInt(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    pNon_car_spec->field_0xf0->friction = friction;
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &tv1);
                    pNon_car_spec->field_0xf0->type = eJoint_ball_n_socket;
                } else if (DRStricmp(s, "WORLD_TRANSLATION_JOINT") == 0) {
                    br_vector3 pos;
                    br_vector3 dir;
                    br_scalar limit;

                    /* joint position */
                    GetThreeFloats(pF, &pos.v[0], &pos.v[1], &pos.v[2]);

                    /* axis (direction of movement) */
                    GetThreeFloats(pF, &dir.v[0], &dir.v[1], &dir.v[2]);

                    /* lift height */
                    limit = GetAScalar(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &pos);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->hinge_axis, &dir);

                    if (fabs(dir.v[0]) <= fabs(dir.v[1]) && fabs(dir.v[0]) <= fabs(dir.v[2])) {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, 0.f, dir.v[2], -dir.v[1]);
                    } else if (fabs(dir.v[1]) <= fabs(dir.v[2])) {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, -dir.v[2], 0.f, -dir.v[0]);
                    } else {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, dir.v[1], -dir.v[0], 0.f);
                    }
                    BrVector3Normalise(&pNon_car_spec->field_0xf0->hinge_axis2, &pNon_car_spec->field_0xf0->hinge_axis2);
                    BrVector3Cross(&pNon_car_spec->field_0xf0->hinge_axis3, &pNon_car_spec->field_0xf0->hinge_axis, &pNon_car_spec->field_0xf0->hinge_axis2);
                    pNon_car_spec->field_0xf0->type = eJoint_translation;
                    pNon_car_spec->flags |= 0x10000;
                    pNon_car_spec->field_0xf0->count_limits = 1;
                    pNon_car_spec->field_0xf0->limits[0].type = eJoint_limit_11;
                    pNon_car_spec->field_0xf0->limits[0].value = limit;

                    pNon_car_spec->translation_parameters = BrMemAllocate(sizeof(tJoint_translation_params), kMem_non_car_spec);

                    /* forward accel in g */
                    pNon_car_spec->translation_parameters->forward_acceleration = GetAScalar(pF);

                    /* reverse accel in g (nb grvity will pull it down) */
                    pNon_car_spec->translation_parameters->reverse_acceleration = GetAScalar(pF);

                    /* resistance going up */
                    pNon_car_spec->translation_parameters->forward_resistance = GetAScalar(pF);

                    /* resistance going down */
                    pNon_car_spec->translation_parameters->reverse_resistance = GetAScalar(pF);

                    /* pause at the top */
                    pNon_car_spec->translation_parameters->pause_at_top = GetAnInt(pF);

                    pNon_car_spec->translation_parameters->forward_resistance = (float)exp(0.43 * (double)pNon_car_spec->translation_parameters->forward_resistance);
                    pNon_car_spec->translation_parameters->reverse_resistance = (float)exp(0.43 * (double)pNon_car_spec->translation_parameters->reverse_resistance);
                } else if (DRStricmp(s, "RISE_WHEN_HIT") == 0) {
                    pNon_car_spec->flags |= 0x20000;
                } else if (DRStricmp(s, "RISE_WHEN_DRIVEN_ON") == 0) {
                    pNon_car_spec->flags |= 0x40000;
                } else if (DRStricmp(s, "NUMBER_OF_PUSHES") == 0) {
                    pNon_car_spec->number_of_pushes = GetAnInt(pF);
                } else if (DRStricmp(s, "DRIVABLE_ON") == 0) {
                    pNon_car_spec->collision_info->drivable_on = 1;
                } else if (DRStricmp(s, "INFINITE_I") == 0) {
                    pNon_car_spec->collision_info->flags |= 0x800;
                } else if (DRStricmp(s, "IGNORE_WORLD_COLLISIONS") == 0) {
                    pNon_car_spec->collision_info->flags |= 0x80;
                } else if (DRStricmp(s, "BLOCK_CAMERA") == 0) {
                    pNon_car_spec->flags |= 0x80000;
                } else if (DRStricmp(s, "SPARKY") == 0) {
                    pNon_car_spec->flags |= 0x200000;
                } else {
                    FatalError(kFatalError_UnknownNonCarCommand);
                }
            }
        }
    }
}

/* based on carpocalypse2 src/carma2/common/brucetrk.c:41 */
// FUNCTION: CARMA2_HW 0x0040d530
void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel) {
    int i;
    int j;
    int k;
    br_face* face;
    br_face* blendy_face;
    br_vertex* bv;
    int changed_one;

    changed_one = 0;

    for (i = 0, face = pModel->faces; i < pModel->nfaces; i++, face++) {
        SpawnPedsOnFace(face, pModel);
        if (SmashFaceMustBeUpdateable(face->material)) {
            if (gMr_blendy == NULL) {
                gMr_blendy = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                gMr_blendy->render_style = BR_RSTYLE_FACES;
                gMr_blendy->model = BrModelAllocate(NULL, 2000, 2000);
                gMr_blendy->model->nfaces = 0;
                gMr_blendy->model->nvertices = 0;
                gMr_blendy->model->flags |= BR_MODF_UPDATEABLE;
            }
            blendy_face = &gMr_blendy->model->faces[gMr_blendy->model->nfaces];
            gMr_blendy->model->nfaces += 1;
            memcpy(blendy_face, face, sizeof(br_face));
            for (j = 0; j < 3; j++) {
                br_vertex* vertex_j = &pModel->vertices[face->vertices[j]];
                for (k = 0, bv = gMr_blendy->model->vertices; k < gMr_blendy->model->nvertices; k++, bv++) {
                    if (Vector3Equals(&bv->p, &vertex_j->p)
                            && Vector2Equals(&bv->map, &vertex_j->map)) {
                        blendy_face->vertices[j] = k;
                        vertex_j = NULL;
                        break;
                    }
                }
                if (vertex_j != NULL) {
                    blendy_face->vertices[j] = gMr_blendy->model->nvertices;
                    memcpy(&gMr_blendy->model->vertices[gMr_blendy->model->nvertices], vertex_j, sizeof(br_vertex));
                    gMr_blendy->model->nvertices += 1;
                }
            }
            if (i < pModel->nfaces - 1) {
                memmove(&pModel->faces[i], &pModel->faces[i + 1], (pModel->nfaces - (i + 1)) * sizeof(br_vertex));
            }
            pModel->nfaces -= 1;
            i -= 1;
            face = &pModel->faces[i];
            changed_one = 1;
        }
    }
    if (changed_one) {
        if (pModel->nfaces != 0) {
            BrModelUpdate(pModel, BR_MODU_ALL);
        } else {
            pActor->model = NULL;
            pActor->render_style = BR_RSTYLE_NONE;
        }
    }
}

/* based on carpocalypse2 src/carma2/common/brucetrk.c:159 */
// FUNCTION: CARMA2_HW 0x004f5ae0
void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea1, int* pArea2) {
    int i;
    int j;
    int k;
    int nfaces;
    int dx;
    int dy;
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    v11group* v11g;
    v11face* v11f;
    fmt_vertex* vertices;

    *pArea1 = 0;
    *pArea2 = 0;

    for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
        v11g = &V11MODEL(pModel)->groups[i];
        v11f = v11g->faces;
        vertices = v11g->vertices;
        nfaces = v11g->nfaces;

        for (j = 0; j < nfaces; j++, v11f++) {
            min_x = 10000.f;
            max_x = -10000.f;
            min_y = 10000.f;
            max_y = -10000.f;

            for (k = 0; k < 3; k++) {
                int vertex_i = v11f->vertices[k];

                if (vertices[vertex_i].map.v[0] < min_x) {
                    min_x = vertices[vertex_i].map.v[0];
                }
                if (vertices[vertex_i].map.v[0] > max_x) {
                    max_x = vertices[vertex_i].map.v[0];
                }
                if (vertices[vertex_i].map.v[1] < min_y) {
                    min_y = vertices[vertex_i].map.v[1];
                }
                if (vertices[vertex_i].map.v[1] > max_y) {
                    max_y = vertices[vertex_i].map.v[1];
                }
            }
            dy = (int)(max_y - min_y + 0.9);
            dx = (int)(max_x - min_x + 0.9);
            *pArea1 += dx * dy;
            *pArea2 += dx * dy;
        }
    }
    *pArea1 += 50;
    *pArea2 += 50;
    if (*pArea1 > 250) {
        *pArea1 = 250;
    }
    if (*pArea2 > 250) {
        *pArea2 = 250;
    }
}

/* based on carpocalypse2 src/carma2/common/brucetrk.c:211 */
// FUNCTION: CARMA2_HW 0x004f5cb0
void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor) {
    extern int gCount_track_smashable_environment_specs;
    extern tSmashable_item_spec* gTrack_smashable_environment_specs;
    extern void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown);
    int i;
    tSmashable_item_spec* spec;

    if (pActor->model == NULL || pActor->model->identifier == NULL) {
        return;
    }
    for (i = 0, spec = gTrack_smashable_environment_specs; i < gCount_track_smashable_environment_specs; i++, spec++) {
        if (spec->trigger_type == kSmashableTrigger_Model && pActor->model == spec->trigger_object.model) {
            char s[64];
            char s2[64];
            int len;

            strcpy(s, pActor->identifier);
            len = strlen(s);
            if (len < 4) {
                sprintf(s, "Smash material %s has a name that is less than 4 characters long", pActor->identifier);
                PDFatalError(s);
            }
            strcpy(s2, &s[len - 4]);
            sprintf(&s[len - 4], "        ");
            sprintf(&s[5], "%cx", '|');
            s[6] = i + 1;
            strcat(s, s2);
            s[11] = '\0';
            if (strlen(pActor->identifier) < 12) {
                BrResFree(pActor->identifier);
                pActor->identifier = BrResStrDup(pActor, s);
            } else {
                strcpy(pActor->identifier, s);
            }

            if ((gCurrent_race.race_spec->race_type >= 4 && *(tU16*)((char*)spec + 0x2ac) == gCurrent_race.race_spec->options.cars.count_opponents)
                    || (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_2 && *(tU16*)((char*)spec + 0x2ac) == 1)) {
                AddSmashTarget(pActor->model, pActor, -1);
            }
        }
    }
}

/* based on carpocalypse2 src/carma2/common/brucetrk.c:101 (GPL-3.0) */
// FUNCTION: CARMA2_HW 0x0040d1f0
intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* pData) {
    extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);
    extern void C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor);
    extern void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity);
    extern void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor);
    char s[256];
    tTrack_spec* pTrack_spec;
    br_scalar n0;
    br_scalar n1;
    br_scalar n2;
    br_scalar m0;
    br_scalar m1;
    br_scalar m2;

    pTrack_spec = pData;
    SetSmashableModel(pActor);
    if (pActor->identifier[0] == '&' && pActor->model == NULL) {
        sprintf(s, "Accessory %s has no model. Well how does it pose? Awful!", pActor->identifier);
        PDFatalError(s);
    }
    if (pActor->identifier[0] == '&' && '0' <= pActor->identifier[1] && pActor->identifier[1] <= '9') {
        int id;

        id = pActor->identifier[7] + 10 * (pActor->identifier[6] + 10 * (pActor->identifier[5] + 10 * pActor->identifier[4])) - 53328;
        if (id < 0 || id >= pTrack_spec->count_non_cars) {
            return 1;
        }
        n0 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[0]);
        n1 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[1]);
        n2 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[2]);
        if (n0 < 0.999 || n1 < 0.999 || n2 < 0.999) {
            dr_dprintf("non car was scaled down %s", pActor->identifier);
            pActor->t.t.translate.t.v[0] -= -2000.f;
        }
        if (n0 > 1.001 || n1 > 1.001 || n2 > 1.001) {
            m0 = 1.0 / sqrt(n0);
            m1 = 1.0 / sqrt(n1);
            m2 = 1.0 / sqrt(n2);
            pActor->t.t.mat.m[0][0] *= m0;
            pActor->t.t.mat.m[0][1] *= m0;
            pActor->t.t.mat.m[0][2] *= m0;
            pActor->t.t.mat.m[1][0] *= m1;
            pActor->t.t.mat.m[1][1] *= m1;
            pActor->t.t.mat.m[1][2] *= m1;
            pActor->t.t.mat.m[2][0] *= m2;
            pActor->t.t.mat.m[2][1] *= m2;
            pActor->t.t.mat.m[2][2] *= m2;
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

/* based on carpocalypse2 src/carma2/common/pedestrn.c:1874 (inlined into SpawnPedsOnFace in the original) */
static __inline void MakeRandomPointInTriangle(br_vector3* pDest, br_vector3* pV0, br_vector3* pV1, br_vector3* pV2) {
    float alpha;
    float beta;

    alpha = SRandomBetween(0.1f, 0.9f);
    beta = SRandomBetween(0.1f, 0.9f);
    if (alpha + beta > 1.f) {
        alpha = 1.f - alpha;
        beta = 1.f - beta;
    }
    pDest->v[0] = pV0->v[0] * (1.f - alpha - beta) + pV1->v[0] * alpha + pV2->v[0] * beta;
    pDest->v[1] = pV0->v[1] * (1.f - alpha - beta) + pV1->v[1] * alpha + pV2->v[1] * beta;
    pDest->v[2] = pV0->v[2] * (1.f - alpha - beta) + pV1->v[2] * alpha + pV2->v[2] * beta;
}

/* based on carpocalypse2 src/carma2/common/pedestrn.c:1927 (SetModelCallbacks and BuildPedestrian, inlined in the original) */
static __inline void BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec) {
    extern void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);
    extern void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pAction);
    extern tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pName, br_matrix34* pMat);
    extern void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pCharacter, br_vector3* pDir, br_vector3* pUp);
    extern br_vector3 gY_unit_vector;
    br_matrix34 mat;
    char texture_name[32];
    char material_name[32];
    br_pixelmap* texture;
    br_material* material;
    int i;
    int j;

    BrMatrix34Identity(&mat);
    BrVector3Copy((br_vector3*)mat.m[3], pPos);
    pPed->character = BuildCharacterInstance(pGroup_name, &mat);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            br_model* model;

            model = pPed->character->personality->bones->models[i].models[j];
            if (model != NULL) {
                model->flags |= BR_MODF_CUSTOM;
                model->custom = TurnLimbsOnAndOff;
            }
        }
    }
    sprintf(texture_name, "%sB", pGroup_name);
    texture = BrMapFind(texture_name);
    if (texture == NULL) {
        FatalError(kFatalError_CantFindPedTexture_S, texture_name);
    }
    strcpy(material_name, pGroup_name);
    sprintf(material_name + strlen(material_name), "%d.MAT", 1);
    material = BrMaterialFind(material_name);
    if (material != NULL) {
        material->colour_map = texture;
        BrMaterialUpdate(material, BR_MATU_COLOURMAP);
    }
    pPed->hit_points = 100;
    pPed->movement_spec = pMovement_spec;
    pPed->speed_factor = 1.f;
    pPed->exclusion_spec = pExclusion_spec;
    if (pMovement_spec->max_walk_speed_factor != 0) {
        SetPedMove(pPed, 30, IRandomBetween(pMovement_spec->min_walk_speed_factor, pMovement_spec->max_walk_speed_factor), 0, 0, 0, 0);
    } else {
        SetPedMove(pPed, 0, -1, 0, 0, 0, 0);
    }
    BrVector3Normalise(pDir, pDir);
    SetCharacterDirectionAR(pPed->character, pDir, &gY_unit_vector);
    BrVector3Copy(&pPed->pos, pPos);
}

/* based on carpocalypse2 src/carma2/common/pedestrn.c:1965 (AreaOfTriangle is inlined) */
// FUNCTION: CARMA2_HW 0x004d2cc0
void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel) {
    extern int gPed_count;
    extern tPedestrian* gPedestrian_array;
    extern int gCount_race_pedestrian_specs;
    extern tRace_ped_spec* gRace_pedestrian_specs;
    extern tPed_group_spec* gPed_groups;
    extern float gPed_cos_max_slope;
    extern int gPed_animal_count;
    extern char gPed_animal_names[50][32];
    extern int gAnimalsOn;
    extern int gTotal_count_smash_peds;
    extern void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown);
    br_material* material;
    tRace_ped_spec* ped_spec;
    int i;
    int j;
    int n;
    br_vector3 v0;
    br_vector3 v1;
    br_vector3 v2;
    float fNew_ped_count;
    int new_ped_count;
    br_vector3 ped_dir;
    br_vector3 ped_pos;
    char group_name[32];
    int illegal_ped;

    if (gNet_mode != eNet_mode_none) {
        return;
    }
    if (pFace->n.v[1] <= gPed_cos_max_slope) {
        return;
    }
    material = pFace->material;
    if (material == NULL) {
        return;
    }
    if (material->identifier == NULL || strlen(material->identifier) - 4 <= 7) {
        return;
    }
    if (material->identifier[7] != '?') {
        return;
    }
    if (gPed_count >= 2000) {
        return;
    }

    ped_spec = gRace_pedestrian_specs;
    for (n = 0; n < gCount_race_pedestrian_specs; n++, ped_spec++) {
        if (ped_spec->spawn_material == material) {
            goto found;
        }
    }
    return;
found:
    v0 = pModel->vertices[pFace->vertices[0]].p;
    v1 = pModel->vertices[pFace->vertices[1]].p;
    v2 = pModel->vertices[pFace->vertices[2]].p;
    if (ped_spec->density >= 0.f) {
        fNew_ped_count = ped_spec->field_0x10 - (float)fabs(v1.v[2] * v0.v[0] - v1.v[0] * v0.v[2] + v2.v[2] * v1.v[0] - v2.v[0] * v1.v[2] + v2.v[0] * v0.v[2] - v2.v[2] * v0.v[0]) * 0.5f * (6.9 * 6.9) * ped_spec->density * -0.001f;
    } else {
        fNew_ped_count = -ped_spec->density;
    }
    new_ped_count = (int)floor(fNew_ped_count);

    for (i = 0; i < new_ped_count; i++) {
        illegal_ped = 0;
        if (gPed_count >= 2000) {
            return;
        }
        strcpy(group_name, ped_spec->group->members[IRandomBetween(0, ped_spec->group->count - 1)].name);
        if (!gAnimalsOn) {
            for (j = 0; j < gPed_animal_count; j++) {
                if (DRStricmp(group_name, gPed_animal_names[j]) == 0) {
                    illegal_ped = 1;
                    break;
                }
            }
        }
        if (illegal_ped) {
            continue;
        }
        ped_dir.v[0] = SRandomPosNeg(1.f);
        ped_dir.v[1] = 0.f;
        ped_dir.v[2] = SRandomPosNeg(1.f);
        MakeRandomPointInTriangle(&ped_pos, &v0, &v1, &v2);
        BuildPedestrian(&gPedestrian_array[gPed_count], group_name, &ped_pos, &ped_dir, ped_spec->movement, &ped_spec->exclusion);

        if (gCurrent_race.race_spec->race_type == kRaceType_SmashNPed) {
            if (gCurrent_race.race_spec->options.smash_and_peds.ped_group_index == ped_spec->group - gPed_groups) {
                gPedestrian_array[gPed_count].flags |= 0x200;
                AddSmashTarget(NULL, NULL, gPed_count);
            }
        } else if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
            if (gCurrent_race.race_spec->options.peds.count_ped_groups < 0) {
                gTotal_count_smash_peds += 1;
                gPedestrian_array[gPed_count].flags |= 0x200;
            } else {
                for (j = 0; j < gCurrent_race.race_spec->options.peds.count_ped_groups; j++) {
                    if (ped_spec->group - gPed_groups == gCurrent_race.race_spec->options.peds.ped_groups[j]) {
                        gTotal_count_smash_peds += 1;
                        gPedestrian_array[gPed_count].flags |= 0x200;
                        break;
                    }
                }
            }
        }
        gPed_count += 1;
    }
    ped_spec->field_0x10 = fNew_ped_count - new_ped_count;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* based on carpocalypse2 src/carma2/common/world.c:169 */
// GLOBAL: CARMA2_HW 0x0065fe88
const char* gSmashable_item_mode_names[6] = {
    "nochange",
    "decal",
    "texturechange",
    "remove",
    "replacemodel",
    "crush",
};

// GLOBAL: CARMA2_HW 0x006a3330
int gCapacity_track_smashables;

// GLOBAL: CARMA2_HW 0x0079efac
int gMax_count_non_cars;


// GLOBAL: CARMA2_HW 0x00655df0
br_vector3 gY_unit_vector = { { 0.f, 1.f, 0.f } };

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00488b00
void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004079a0
tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pName, br_matrix34* pMat) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004d3360
void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pCharacter, br_vector3* pDir, br_vector3* pUp) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
