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

extern tFunk_temp_buffer* gFunk_temp_vertices;
extern float gFunk_scalar;
extern tCar_spec* gCurrent_car_spec;
extern tFunkotronic_spec* gFunkotronics_array;
extern int gFunkotronics_array_size;
extern tGroove_funk_binding gGroove_funk_bindings[1440];
extern const char* gFunk_nature_names[4];
extern const char* gFunk_type_names[5];
extern const char* gFunk_move_names[7];
extern const char* gFunk_anim_names[4];
extern const char* gTime_mode_names[2];
extern const char* gCamera_animation_names[2];
extern br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData);
extern br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData);
extern void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap);
extern int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);
extern int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);

/* based on carpocalypse2 src/carma2/common/world.c:1143 */
// FUNCTION: CARMA2_HW 0x00474ac0
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {
    int first_time;
    char s[256];
    char spin_texture_string[32];
    char frames_texture_string[32];
    char rock_texture_string[32];
    char lighting_texture_string[32];
    tFunkotronic_spec* the_funk;
    tFunkotronic_spec* new_array;
    tFunk_texturebits* texture_bits;
    tCar_crush_buffer_entry* crush_entry;
    br_material* material;
    char* str;
    float x_0;
    float x_1;
    int g0;
    int g1;
    float speed1;
    float speed2;
    int i;
    int j;
    int k;
    int binding_slot;
    float a_min;
    float d_min;
    float s_min;
    float a_max;
    float d_max;
    float s_max;
    float yon_factor;
    float fov_factor;
    br_matrix34 mat;
    br_vector3 n;
    br_vector3 cam_pos;
    br_euler e;
    float angle1;
    float angle2;
    br_actor* actor;
    br_camera* camera;
    br_camera* global_camera;
    char* the_pixels;
    br_pixelmap* the_pixelmap;
    int size_x;
    int pos_x;
    int size_y;
    int pos_y;

    gFunk_temp_vertices = BrMemAllocate(500 * sizeof(tFunk_temp_buffer), BR_MEMORY_APPLICATION);
    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF FUNK") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT FUNK") != 0) {
                FatalError(kFatalError_FunkotronicFile);
            } else {
                GetALineAndDontArgue(pF, s);
            }
        }
        first_time = 0;
        str = strtok(s, "\t ,/");

        for (i = 0; i < gFunkotronics_array_size; i++) {
            if (gFunkotronics_array[i].owner == -999) {
                the_funk = &gFunkotronics_array[i];
                memset(the_funk, 0, sizeof(tFunkotronic_spec));
                goto have_funk;
            }
        }
        gFunkotronics_array_size += 16;
        new_array = BrMemCalloc(gFunkotronics_array_size, sizeof(tFunkotronic_spec), kMem_funk_spec);
        if (gFunkotronics_array != NULL) {
            memcpy(new_array, gFunkotronics_array, (gFunkotronics_array_size - 16) * sizeof(tFunkotronic_spec));
            for (j = 0; j < (int)REC2_ASIZE(gGroove_funk_bindings); j++) {
                if ((char*)gFunkotronics_array <= (char*)gGroove_funk_bindings[j].v
                        && (char*)gGroove_funk_bindings[j].v < (char*)&gFunkotronics_array[gFunkotronics_array_size - 16]) {
                    gGroove_funk_bindings[j].v = (float*)((char*)gGroove_funk_bindings[j].v + ((char*)new_array - (char*)gFunkotronics_array));
                }
            }
            BrMemFree(gFunkotronics_array);
        }
        gFunkotronics_array = new_array;
        for (i = 0; i < 16; i++) {
            gFunkotronics_array[gFunkotronics_array_size - 16 + i].owner = -999;
        }
        the_funk = &gFunkotronics_array[gFunkotronics_array_size - 16];
have_funk:
        the_funk->owner = pOwner;

        if (pCar_crush_datas != NULL) {
            material = NULL;
            for (i = 0; i < pCar_crush_datas->count_entries && material == NULL; i++) {
                crush_entry = pCar_crush_datas->entries[i];
                if (crush_entry != NULL) {
                    for (j = 0; j < (int)crush_entry->count_smashables; j++) {
                        if (strcmp(crush_entry->smashables[j].material_name, str) == 0) {
                            crush_entry->smashables[j].funk = (int)(the_funk - gFunkotronics_array);
                            material = crush_entry->smashables[j].funk_material;
                            break;
                        }
                    }
                }
            }
            the_funk->material = material;
        }
        if (the_funk->material == NULL) {
            the_funk->material = BrMaterialFind(str);
            if (the_funk->material == NULL) {
                FatalError(kFatalError_CannotFindMainMaterialInFunkotronicFile_S, str);
            }
        }
        str = strtok(NULL, "\t ,/");
        if (str != NULL && strcmp(str, "multiple") == 0 && the_funk->material->identifier != NULL) {
            BrResFree(the_funk->material->identifier);
            the_funk->material->identifier = NULL;
        }

        the_funk->mode = GetALineAndInterpretCommand(pF, gFunk_nature_names, REC2_ASIZE(gFunk_nature_names));
        the_funk->proximity_count = 0;
        the_funk->proximity_array = NULL;
        if (the_funk->mode == eFunk_mode_distance) {
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, CalcProximities, the_funk);
            the_funk->proximity_array = BrMemAllocate(the_funk->proximity_count * sizeof(tFunk_proximity), kMem_funk_prox_array);
            the_funk->proximity_count = 0;
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, AddProximities, the_funk);
        }

        the_funk->matrix_mod_type = GetALineAndInterpretCommand(pF, gFunk_type_names, REC2_ASIZE(gFunk_type_names));
        the_funk->flags &= ~0x1;
        if (the_funk->matrix_mod_type != eMatrix_mod_none) {
            the_funk->matrix_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }

        switch (the_funk->matrix_mod_type) {
        case eMatrix_mod_spin:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.spin_info.period;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_funk->matrix_mod_data.spin_info.period = 0.f;
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->matrix_mod_data.texture_info.data = texture_bits;
                GetAString(pF, spin_texture_string);
                texture_bits->count = (tU8)strlen(spin_texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (spin_texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                gFunk_scalar = GetAScalar(pF);
                the_funk->matrix_mod_data.spin_info.period = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
                break;
            }
            break;
        case eMatrix_mod_rock:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.rock_info.period;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_funk->matrix_mod_data.rock_info.period = 0.f;
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->matrix_mod_data.texture_info.data = texture_bits;
                GetAString(pF, rock_texture_string);
                texture_bits->count = (tU8)strlen(rock_texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (rock_texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                gFunk_scalar = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
                break;
            }
            the_funk->matrix_mod_data.rock_info.rock_angle = GetAScalar(pF);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.rock_info.x_centre = x_0 * 0.01f;
            the_funk->matrix_mod_data.rock_info.y_centre = x_1 * 0.01f;
            break;
        case eMatrix_mod_throb:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    binding_slot = pRef_offset + g0;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.throb_info.x_period;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_funk->matrix_mod_data.throb_info.x_period = 0.f;
                }
                if (g1 >= 0) {
                    binding_slot = pRef_offset + g1;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.throb_info.y_period;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_funk->matrix_mod_data.throb_info.y_period = 0.f;
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.throb_info.x_period = (speed1 == 0.f) ? 0.f : 1000.0 / speed1;
                gFunk_scalar = speed2;
                the_funk->matrix_mod_data.throb_info.y_period = (speed2 == 0.f) ? 0.f : 1000.0 / speed2;
                break;
            }
            GetPairOfFloatPercents(pF, &the_funk->matrix_mod_data.throb_info.x_magnitude, &the_funk->matrix_mod_data.throb_info.y_magnitude);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.throb_info.x_centre = x_0 * 0.01f;
            the_funk->matrix_mod_data.throb_info.y_centre = x_1 * 0.01f;
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.throb_info.x_period == 0.f) {
                    the_funk->matrix_mod_data.throb_info.x_period = 1.f;
                    the_funk->matrix_mod_data.throb_info.x_magnitude = 0.f;
                }
                if (the_funk->matrix_mod_data.throb_info.y_period == 0.f) {
                    the_funk->matrix_mod_data.throb_info.y_period = 1.f;
                    the_funk->matrix_mod_data.throb_info.y_magnitude = 0.f;
                }
            }
            break;
        case eMatrix_mod_slither:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    binding_slot = pRef_offset + g0;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.slither_info.x_period;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_funk->matrix_mod_data.slither_info.x_period = 0.f;
                }
                if (g1 >= 0) {
                    binding_slot = pRef_offset + g1;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.slither_info.y_period;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_funk->matrix_mod_data.slither_info.y_period = 0.f;
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.slither_info.x_period = (speed1 == 0.f) ? 0.f : 1000.0 / speed1;
                gFunk_scalar = speed2;
                the_funk->matrix_mod_data.slither_info.y_period = (speed2 == 0.f) ? 0.f : 1000.0 / speed2;
                break;
            }
            GetPairOfFloatPercents(pF, &the_funk->matrix_mod_data.slither_info.x_magnitude, &the_funk->matrix_mod_data.slither_info.y_magnitude);
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.slither_info.x_period == 0.f) {
                    the_funk->matrix_mod_data.slither_info.x_period = 1.f;
                    the_funk->matrix_mod_data.slither_info.x_magnitude = 0.f;
                }
                if (the_funk->matrix_mod_data.slither_info.y_period == 0.f) {
                    the_funk->matrix_mod_data.slither_info.y_period = 1.f;
                    the_funk->matrix_mod_data.slither_info.y_magnitude = 0.f;
                }
            }
            break;
        case eMatrix_mod_roll:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    binding_slot = pRef_offset + g0;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.roll_info.x_period;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_funk->matrix_mod_data.roll_info.x_period = 0.f;
                }
                if (g1 >= 0) {
                    binding_slot = pRef_offset + g1;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_funk->matrix_mod_data.roll_info.y_period;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_funk->matrix_mod_data.roll_info.y_period = 0.f;
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.roll_info.x_period = (speed1 == 0.f) ? 0.f : 1000.0 / speed1;
                gFunk_scalar = speed2;
                the_funk->matrix_mod_data.roll_info.y_period = (speed2 == 0.f) ? 0.f : 1000.0 / speed2;
                break;
            }
            break;
        default:
            break;
        }

        the_funk->lighting_animation_type = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        if (the_funk->lighting_animation_type != eMove_none) {
            switch (the_funk->lighting_animation_type) {
            case eMove_controlled:
            case eMove_absolute:
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_funk->lighting_animation_data.controlled.period;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_funk->lighting_animation_data.controlled.period = 0.f;
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->lighting_animation_data.texture_info.data = texture_bits;
                GetAString(pF, lighting_texture_string);
                texture_bits->count = (tU8)strlen(lighting_texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (lighting_texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                gFunk_scalar = GetAScalar(pF);
                the_funk->lighting_animation_data.rock_info.period = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
                break;
            }
            GetThreeFloatPercents(pF, &a_min, &d_min, &s_min);
            GetThreeFloatPercents(pF, &a_max, &d_max, &s_max);
            the_funk->ambient_base = (a_max + a_min) * 0.5f;
            the_funk->direct_base = (d_max + d_min) * 0.5f;
            the_funk->specular_base = (s_max + s_min) * 0.5f;
            the_funk->ambient_delta = (a_max - a_min) * 0.5f;
            the_funk->direct_delta = (d_max - d_min) * 0.5f;
            the_funk->specular_delta = (s_max - s_min) * 0.5f;
        }

        the_funk->texture_animation_type = GetALineAndInterpretCommand(pF, gFunk_anim_names, REC2_ASIZE(gFunk_anim_names));
        if (the_funk->texture_animation_type != eTexture_animation_none
                && the_funk->texture_animation_type != eTexture_animation_mirror
                && the_funk->texture_animation_type != eTexture_animation_camera) {
            the_funk->time_mode = GetALineAndInterpretCommand(pF, gTime_mode_names, REC2_ASIZE(gTime_mode_names));
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic && gAusterity_mode) {
            the_funk->texture_animation_type = eTexture_animation_none;
            GetALineAndDontArgue(pF, s);
        }
        the_funk->last_frame = 0.f;

        switch (the_funk->texture_animation_type) {
        case eTexture_animation_frames:
            the_funk->texture_animation_data.frames_info.mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
            switch (the_funk->texture_animation_data.frames_info.mode) {
            case eMove_controlled:
            case eMove_absolute:
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_funk->texture_animation_data.frames_info.controlled.period;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_funk->texture_animation_data.frames_info.controlled.period = 0.f;
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->texture_animation_data.frames_info.texture_info.data = texture_bits;
                GetAString(pF, frames_texture_string);
                texture_bits->count = (tU8)strlen(frames_texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (frames_texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                gFunk_scalar = GetAScalar(pF);
                the_funk->texture_animation_data.frames_info.rock_info.period = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
                break;
            }
            the_funk->texture_animation_data.frames_info.texture_count = (int)GetAScalar(pF);
            the_funk->texture_animation_data.frames_info.current_frame = 0;
            the_funk->texture_animation_data.frames_info.has_matrix = 0;
            for (i = 0; i < the_funk->texture_animation_data.frames_info.texture_count; i++) {
                GetALineAndDontArgue(pF, s);
                str = strtok(s, "\t ,/");
                the_funk->texture_animation_data.frames_info.textures[i] = BrMapFind(str);
                if (the_funk->texture_animation_data.frames_info.textures[i] == NULL) {
                    FatalError(kFatalError_CannotFindAnimationFramePixelmapReferencedInFunkotronicFile);
                }
                BrMatrix23Identity(&the_funk->texture_animation_data.frames_info.mat[i]);
                str = strtok(NULL, "\t ,/");
                if (str != NULL && strlen(str) != 0) {
                    the_funk->texture_animation_data.frames_info.has_matrix = 1;
                    sscanf(str, "%d", &size_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &size_y);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_y);
                    the_funk->texture_animation_data.frames_info.mat[i].m[0][0] = 1.0 / size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[1][1] = 1.0 / size_y;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][0] = (float)pos_x / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][1] = (float)pos_y / (float)size_y;
                }
            }
            break;
        case eTexture_animation_flic:
            GetAString(pF, s);
            the_funk->texture_animation_data.flic_info.flic_data = NULL;
            if (LoadFlicData(s, &the_funk->texture_animation_data.flic_info.flic_data, &the_funk->texture_animation_data.flic_info.flic_data_length)) {
                the_funk->texture_animation_data.flic_info.flic_descriptor.data_start = NULL;
                StartFlic(s, -1, &the_funk->texture_animation_data.flic_info.flic_descriptor, the_funk->texture_animation_data.flic_info.flic_data_length, (tS8*)the_funk->texture_animation_data.flic_info.flic_data, NULL, 0, 0, 0);
                the_funk->last_frame = 0.f;
                the_pixels = BrMemAllocate(the_funk->texture_animation_data.flic_info.flic_descriptor.height * ((the_funk->texture_animation_data.flic_info.flic_descriptor.width + 3) & ~3), kMem_video_pixels);
                if (gScreen->row_bytes < 0) {
                    BrFatal("C:\\Carma2\\Source\\Common\\Funkgroo.c", 732, "Bruce bug at line %d, file C:\\Carma2\\Source\\Common\\Funkgroo.c", 732);
                }
                the_pixelmap = DRPixelmapAllocate(gScreen->type, (br_uint_16)the_funk->texture_animation_data.flic_info.flic_descriptor.width, (br_uint_16)the_funk->texture_animation_data.flic_info.flic_descriptor.height, the_pixels, 0);
                AssertFlicPixelmap(&the_funk->texture_animation_data.flic_info.flic_descriptor, the_pixelmap);
                the_funk->material->colour_map = the_pixelmap;
                the_funk->material->user = the_pixelmap;
                BrMaterialUpdate(the_funk->material, BR_MATU_ALL);
            } else {
                the_funk->texture_animation_type = eTexture_animation_none;
            }
            break;
        case eTexture_animation_camera:
            the_funk->texture_animation_data.camera_info.mode = GetALineAndInterpretCommand(pF, gCamera_animation_names, REC2_ASIZE(gCamera_animation_names));
            GetPairOfFloats(pF, &yon_factor, &fov_factor);
            the_funk->texture_animation_data.camera_info.field_0x60 = GetAnInt(pF);
            the_funk->texture_animation_data.camera_info.count = GetAnInt(pF);
            for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                the_funk->texture_animation_data.camera_info.actors[i] = actor;
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                camera = actor->type_data;
                global_camera = gCamera->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = BrDegreeToAngle(fov_factor * global_camera->field_of_view);
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = yon_factor * global_camera->yon_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                GetThreeFloats(pF, &cam_pos.v[0], &cam_pos.v[1], &cam_pos.v[2]);
                if (the_funk->texture_animation_data.camera_info.mode == eFunk_camera_static) {
                    GetPairOfFloats(pF, &angle1, &angle2);
                    e.order = 0x12;
                    e.a = BrDegreeToAngle(angle1);
                    e.b = BrDegreeToAngle(360.0f - angle2);
                    e.c = 0;
                    BrEulerToMatrix34(&actor->t.t.mat, &e);
                }
                actor->t.t.mat.m[3][0] = cam_pos.v[0];
                actor->t.t.mat.m[3][1] = cam_pos.v[1];
                actor->t.t.mat.m[3][2] = cam_pos.v[2];
            }
            break;
        case eTexture_animation_mirror:
            if (the_funk->proximity_count == 0) {
                the_funk->texture_animation_data.mirror_info.actor = NULL;
                GetALineAndDontArgue(pF, s);
                GetALineAndDontArgue(pF, s);
            } else {
                global_camera = gCamera->type_data;
                yon_factor = GetAScalar(pF);
                the_funk->texture_animation_data.mirror_info.yon = yon_factor * global_camera->yon_z;
                the_funk->texture_animation_data.mirror_info.field_0x5c = GetAnInt(pF);
                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                the_funk->texture_animation_data.mirror_info.actor = actor;
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                camera = actor->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = global_camera->field_of_view;
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = 2.f * global_camera->yon_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                n = the_funk->proximity_array->n;
                the_funk->texture_animation_data.mirror_info.mat.m[0][0] = 1.f - 2.f * n.v[0] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[0][1] = 2.f * n.v[0] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[0][2] = 2.f * n.v[0] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[1][0] = 2.f * n.v[1] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[1][1] = 1.f - 2.f * n.v[1] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[1][2] = 2.f * n.v[1] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[2][0] = 2.f * n.v[2] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[2][1] = 2.f * n.v[2] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[2][2] = 1.f - 2.f * n.v[2] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[3][0] = -2.f * n.v[0] * the_funk->proximity_array->d;
                the_funk->texture_animation_data.mirror_info.mat.m[3][1] = -2.f * n.v[1] * the_funk->proximity_array->d;
                the_funk->texture_animation_data.mirror_info.mat.m[3][2] = -2.f * n.v[2] * the_funk->proximity_array->d;
                mat.m[0][0] = the_funk->proximity_array->v[1].v[0] - the_funk->proximity_array->v[0].v[0];
                mat.m[0][1] = the_funk->proximity_array->v[1].v[1] - the_funk->proximity_array->v[0].v[1];
                mat.m[0][2] = the_funk->proximity_array->v[1].v[2] - the_funk->proximity_array->v[0].v[2];
                mat.m[1][0] = the_funk->proximity_array->n.v[0];
                mat.m[1][1] = the_funk->proximity_array->n.v[1];
                mat.m[1][2] = the_funk->proximity_array->n.v[2];
                mat.m[2][0] = mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1];
                mat.m[2][1] = mat.m[0][2] * mat.m[1][0] - mat.m[0][0] * mat.m[1][2];
                mat.m[2][2] = mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0];
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.min, (float)0x7fffffff, (float)0x7fffffff, (float)0x7fffffff);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.max, (float)-0x7fffffff, (float)-0x7fffffff, (float)-0x7fffffff);
                for (i = 0; i < the_funk->proximity_count; i++) {
                    for (j = 0; j < 3; j++) {
                        BrMatrix34ApplyP(&gFunk_temp_vertices[i].points[j], &the_funk->proximity_array->v[j], &mat);
                        for (k = 0; k < 3; k++) {
                            if (!(gFunk_temp_vertices[i].points[j].v[k] >= the_funk->texture_animation_data.mirror_info.bounds.min.v[k])) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
                for (i = 0; i < the_funk->proximity_count; i++) {
                    for (j = 0; j < 3; j++) {
                        gFunk_temp_vertices[i].vertices[j]->map.v[0] = (gFunk_temp_vertices[i].points[j].v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]) / (the_funk->texture_animation_data.mirror_info.bounds.max.v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
                        gFunk_temp_vertices[i].vertices[j]->map.v[1] = (gFunk_temp_vertices[i].points[j].v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2]) / (the_funk->texture_animation_data.mirror_info.bounds.max.v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
                        for (k = 0; k < 3; k++) {
                            if (!(gFunk_temp_vertices[i].points[j].v[k] >= the_funk->texture_animation_data.mirror_info.bounds.min.v[k])) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    BrMemFree(gFunk_temp_vertices);
}

extern tGroovidelic_spec* gGroovidelics_array;
extern int gGroovidelics_array_size;
extern br_actor* gGroove_by_proxy_actor;
extern const char* gLollipop_names[3];
extern const char* gGroove_nature_names[2];
extern const char* gGroove_path_names[2];
extern const char* gGroove_object_names[4];
extern const char* gAxis_names[3];

/* based on the REA pseudocode for 0x476470 (inlines AddNewGroovidelic) */
// FUNCTION: CARMA2_HW 0x00476470
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {
    volatile int first_time;
    char s[256];
    char circular_texture_name[32];
    char spin_texture_name[32];
    char rock_texture_name[32];
    char straight_texture_name[32];
    tGroovidelic_spec* the_groove;
    tGroovidelic_spec* new_array;
    tFunk_texturebits* texture_bits;
    br_model* proxy_model;
    char* str;
    int binding_slot;
    int i;
    int j;
    float straight_centre_0;
    float straight_centre_1;
    float straight_centre_2;
    float circular_centre_0;
    float circular_centre_1;
    float circular_centre_2;
    float spin_centre_0;
    float spin_centre_1;
    float spin_centre_2;
    float rock_centre_0;
    float rock_centre_1;
    float rock_centre_2;
    float throb_centre_0;
    float throb_centre_1;
    float throb_centre_2;
    float shear_centre_0;
    float shear_centre_1;
    float shear_centre_2;
    float throb_speed_0;
    float throb_speed_1;
    float throb_speed_2;
    float shear_speed_0;
    float shear_speed_1;
    float shear_speed_2;
    int throb_0;
    int throb_1;
    int throb_2;
    int shear_0;
    int shear_1;
    int shear_2;

    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF GROOVE") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT GROOVE") != 0) {
                FatalError(kFatalError_ErrorWithinGroovidelicFile);
            } else {
                GetALineAndDontArgue(pF, s);
            }
        }
        first_time = 0;
        str = strtok(s, "\t ,/");
        for (i = 0; i < gGroovidelics_array_size; i++) {
            if (gGroovidelics_array[i].owner == -999) {
                the_groove = &gGroovidelics_array[i];
                memset(the_groove, 0, sizeof(tGroovidelic_spec));
                goto have_groove;
            }
        }
        gGroovidelics_array_size += 16;
        new_array = BrMemCalloc(gGroovidelics_array_size, sizeof(tGroovidelic_spec), kMem_groove_spec);
        if (gGroovidelics_array != NULL) {
            memcpy(new_array, gGroovidelics_array, (gGroovidelics_array_size - 16) * sizeof(tGroovidelic_spec));
            for (j = 0; j < (int)REC2_ASIZE(gGroove_funk_bindings); j++) {
                if ((char*)gGroovidelics_array <= (char*)gGroove_funk_bindings[j].v
                        && (char*)gGroove_funk_bindings[j].v < (char*)&gGroovidelics_array[gGroovidelics_array_size - 16]) {
                    gGroove_funk_bindings[j].v = (float*)((char*)gGroove_funk_bindings[j].v + ((char*)new_array - (char*)gGroovidelics_array));
                }
            }
            BrMemFree(gGroovidelics_array);
        }
        gGroovidelics_array = new_array;
        for (i = 0; i < 16; i++) {
            gGroovidelics_array[gGroovidelics_array_size - 16 + i].owner = -999;
        }
        the_groove = &gGroovidelics_array[gGroovidelics_array_size - 16];
have_groove:
        the_groove->owner = pOwner;
        the_groove->block_flags = 0;
        the_groove->actor = DRActorFindRecurse(pParent_actor, str);
        if (the_groove->actor == NULL) {
            if (!pAllowed_to_be_absent && !gAusterity_mode) {
                FatalError(kFatalError_CannotFindActorReferencedInGroovidelicFile_S, str);
            } else {
                if (gGroove_by_proxy_actor == NULL) {
                    gGroove_by_proxy_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                    proxy_model = LoadModel("PROXY.DAT");
                    gGroove_by_proxy_actor->model = proxy_model;
                    BrModelAdd(gGroove_by_proxy_actor->model);
                    BrActorAdd(gDont_render_actor, gGroove_by_proxy_actor);
                }
                the_groove->actor = gGroove_by_proxy_actor;
            }
        }
        the_groove->lollipop_mode = GetALineAndInterpretCommand(pF, gLollipop_names, REC2_ASIZE(gLollipop_names));
        the_groove->mode = GetALineAndInterpretCommand(pF, gGroove_nature_names, REC2_ASIZE(gGroove_nature_names));
        the_groove->path_type = GetALineAndInterpretCommand(pF, gGroove_path_names, REC2_ASIZE(gGroove_path_names));
        the_groove->path_interrupt_status = eInterrupt_none;
        the_groove->object_interrupt_status = eInterrupt_none;
        if (the_groove->path_type != eGroove_path_none) {
            the_groove->path_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->path_type) {
        case eGroove_path_straight:
            GetThreeFloats(pF, &straight_centre_0, &straight_centre_1, &straight_centre_2);
            the_groove->path_data.straight_info.centre.v[0] = straight_centre_0;
            the_groove->path_data.straight_info.centre.v[1] = straight_centre_1;
            the_groove->path_data.straight_info.centre.v[2] = straight_centre_2;
            if (the_groove->path_data.straight_info.centre.v[0] == 0.f
                    && the_groove->path_data.straight_info.centre.v[1] == 0.f
                    && the_groove->path_data.straight_info.centre.v[2] == 0.f) {
                the_groove->path_data.straight_info.centre = the_groove->actor->t.t.translate.t;
            }
            if (the_groove->path_mode == eMove_controlled || the_groove->path_mode == eMove_absolute) {
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_groove->path_data.straight_info.period.value;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_groove->path_data.straight_info.period.value = 0.f;
                } else if (the_groove->path_mode == eMove_texturebits) {
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->path_data.straight_info.texture_info.data = texture_bits;
                GetAString(pF, straight_texture_name);
                texture_bits->count = (tU8)strlen(straight_texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (straight_texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                } else {
                gFunk_scalar = GetAScalar(pF);
                the_groove->path_data.straight_info.period.value = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
            }
            GetThreeFloats(pF, &the_groove->path_data.straight_info.x_delta, &the_groove->path_data.straight_info.y_delta, &the_groove->path_data.straight_info.z_delta);
            break;
        case eGroove_path_circular:
            GetThreeFloats(pF, &circular_centre_0, &circular_centre_1, &circular_centre_2);
            the_groove->path_data.circular_info.centre.v[0] = circular_centre_0;
            the_groove->path_data.circular_info.centre.v[1] = circular_centre_1;
            the_groove->path_data.circular_info.centre.v[2] = circular_centre_2;
            if (the_groove->path_data.straight_info.centre.v[0] == 0.f
                    && the_groove->path_data.straight_info.centre.v[1] == 0.f
                    && the_groove->path_data.straight_info.centre.v[2] == 0.f) {
                the_groove->path_data.straight_info.centre = the_groove->actor->t.t.translate.t;
            }
            if (the_groove->path_mode == eMove_controlled || the_groove->path_mode == eMove_absolute) {
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_groove->path_data.circular_info.period.value;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_groove->path_data.circular_info.period.value = 0.f;
                } else if (the_groove->path_mode == eMove_texturebits) {
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->path_data.circular_info.texture_info.data = texture_bits;
                GetAString(pF, circular_texture_name);
                texture_bits->count = (tU8)strlen(circular_texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (circular_texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                } else {
                gFunk_scalar = GetAScalar(pF);
                the_groove->path_data.circular_info.period.value = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
            }
            the_groove->path_data.circular_info.radius = GetAScalar(pF);
            the_groove->path_data.circular_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            break;
        default:
            break;
        }
        the_groove->object_type = GetALineAndInterpretCommand(pF, gGroove_object_names, REC2_ASIZE(gGroove_object_names));
        the_groove->object_position = the_groove->actor->t.t.translate.t;
        if (the_groove->object_type != eGroove_object_none) {
            the_groove->object_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->object_type) {
        case eGroove_object_spin:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.spin_info.period.value;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_groove->object_data.spin_info.period.value = 0.f;
                } else if (the_groove->object_mode == eMove_texturebits) {
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->object_data.spin_info.texture_info.data = texture_bits;
                GetAString(pF, spin_texture_name);
                texture_bits->count = (tU8)strlen(spin_texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (spin_texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                } else {
                gFunk_scalar = GetAScalar(pF);
                the_groove->object_data.spin_info.period.value = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
            }
            GetThreeFloats(pF, &spin_centre_0, &spin_centre_1, &spin_centre_2);
            the_groove->object_centre.v[0] = spin_centre_0;
            the_groove->object_centre.v[1] = spin_centre_1;
            the_groove->object_centre.v[2] = spin_centre_2;
            the_groove->object_data.spin_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            break;
        case eGroove_object_rock:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                binding_slot = pRef_offset + GetAnInt(pF);
                if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                    FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                }
                gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.rock_info.period.value;
                gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                the_groove->object_data.rock_info.period.value = 0.f;
                } else if (the_groove->object_mode == eMove_texturebits) {
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->object_data.rock_info.texture_info.data = texture_bits;
                GetAString(pF, rock_texture_name);
                texture_bits->count = (tU8)strlen(rock_texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (rock_texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                } else {
                gFunk_scalar = GetAScalar(pF);
                the_groove->object_data.rock_info.period.value = (gFunk_scalar == 0.f) ? 0.f : 1000.0 / gFunk_scalar;
            }
            GetThreeFloats(pF, &rock_centre_0, &rock_centre_1, &rock_centre_2);
            the_groove->object_centre.v[0] = rock_centre_0;
            the_groove->object_centre.v[1] = rock_centre_1;
            the_groove->object_centre.v[2] = rock_centre_2;
            the_groove->object_data.rock_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            the_groove->object_data.rock_info.max_angle = GetAScalar(pF);
            break;
        case eGroove_object_throb:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                GetThreeInts(pF, &throb_0, &throb_1, &throb_2);
                if (throb_0 >= 0) {
                    binding_slot = pRef_offset + throb_0;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.throb_info.x_period.value;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_groove->object_data.throb_info.x_period.value = 0.f;
                }
                if (throb_1 >= 0) {
                    binding_slot = pRef_offset + throb_1;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.throb_info.y_period.value;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_groove->object_data.throb_info.y_period.value = 0.f;
                }
                if (throb_2 >= 0) {
                    binding_slot = pRef_offset + throb_2;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.throb_info.z_period.value;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_groove->object_data.throb_info.z_period.value = 0.f;
                }
            } else {
                GetThreeFloats(pF, &throb_speed_0, &throb_speed_1, &throb_speed_2);
                the_groove->object_data.throb_info.x_period.value = (throb_speed_0 == 0.f) ? 0.f : 1000.0 / throb_speed_0;
                the_groove->object_data.throb_info.y_period.value = (throb_speed_1 == 0.f) ? 0.f : 1000.0 / throb_speed_1;
                gFunk_scalar = throb_speed_2;
                the_groove->object_data.throb_info.z_period.value = (throb_speed_2 == 0.f) ? 0.f : 1000.0 / throb_speed_2;
            }
            GetThreeFloats(pF, &throb_centre_0, &throb_centre_1, &throb_centre_2);
            the_groove->object_centre.v[0] = throb_centre_0;
            the_groove->object_centre.v[1] = throb_centre_1;
            the_groove->object_centre.v[2] = throb_centre_2;
            GetThreeFloatPercents(pF, &the_groove->object_data.throb_info.x_magnitude, &the_groove->object_data.throb_info.y_magnitude, &the_groove->object_data.throb_info.z_magnitude);
            break;
        case eGroove_object_shear:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                GetThreeInts(pF, &shear_0, &shear_1, &shear_2);
                if (shear_0 >= 0) {
                    binding_slot = pRef_offset + shear_0;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.shear_info.x_period.value;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_groove->object_data.shear_info.x_period.value = 0.f;
                }
                if (shear_1 >= 0) {
                    binding_slot = pRef_offset + shear_1;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.shear_info.y_period.value;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_groove->object_data.shear_info.y_period.value = 0.f;
                }
                if (shear_2 >= 0) {
                    binding_slot = pRef_offset + shear_2;
                    if (binding_slot < 0 || binding_slot >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
                        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
                    }
                    gGroove_funk_bindings[binding_slot].v = &the_groove->object_data.shear_info.z_period.value;
                    gGroove_funk_bindings[binding_slot].field_0x4 = 0;
                    the_groove->object_data.shear_info.z_period.value = 0.f;
                }
            } else {
                GetThreeFloats(pF, &shear_speed_0, &shear_speed_1, &shear_speed_2);
                the_groove->object_data.shear_info.x_period.value = (shear_speed_0 == 0.f) ? 0.f : 1000.0 / shear_speed_0;
                the_groove->object_data.shear_info.y_period.value = (shear_speed_1 == 0.f) ? 0.f : 1000.0 / shear_speed_1;
                gFunk_scalar = shear_speed_2;
                the_groove->object_data.shear_info.z_period.value = (shear_speed_2 == 0.f) ? 0.f : 1000.0 / shear_speed_2;
            }
            GetThreeFloats(pF, &shear_centre_0, &shear_centre_1, &shear_centre_2);
            the_groove->object_centre.v[0] = shear_centre_0;
            the_groove->object_centre.v[1] = shear_centre_1;
            the_groove->object_centre.v[2] = shear_centre_2;
            GetThreeFloatPercents(pF, &the_groove->object_data.shear_info.x_magnitude, &the_groove->object_data.shear_info.y_magnitude, &the_groove->object_data.shear_info.z_magnitude);
            break;
        default:
            break;
        }
    }
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

extern tPed_personality* gPed_personalities[50];
extern tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);

static tPed_personality* FindOrOpenPersonality(const char* pName) {
    int i;

    for (i = 0; i < 50; i++) {
        if (gPed_personalities[i] == NULL) {
            continue;
        }
        if (strcmp(gPed_personalities[i]->name, pName) == 0) {
            return gPed_personalities[i];
        }
    }
    return ReadPersonality(pName);
}

extern tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance* pPed);
extern void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir);
extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);
extern tPedestrian* gPedestrian_array;
extern int gBOOL_00744804;

/* based on carpocalypse2 pedestrn.c:1820 (inlined in the original) */
static int OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle) {

    return BrVector3Dot((br_vector3*)pOrientation_1->m[0], (br_vector3*)pOrientation_2->m[0]) < pCos_angle
        || BrVector3Dot((br_vector3*)pOrientation_1->m[1], (br_vector3*)pOrientation_2->m[1]) < pCos_angle
        || BrVector3Dot((br_vector3*)pOrientation_2->m[2], (br_vector3*)pOrientation_1->m[2]) < pCos_angle;
}

/* based on carpocalypse2 pedestrn.c:1827 (inlined in the original) */
static void AssertRootObjectsMatrix(tPed_character_instance* pPed) {
    tPhysics_object* collision_info;
    int i;

    if (pPed->field_0x14 & 0x1) {
        collision_info = pPed->personality->form->simple_physicing[pPed->field_0x5].collision_info;
    } else {
        collision_info = GetRootObject(pPed);
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            collision_info->transform_matrix.m[i][j] = collision_info->actor->t.t.mat.m[i][j];
        }
    }
    collision_info->flags |= 0x100;
}

extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);
extern tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType);
extern tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType);
extern tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType);
extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 physics.c:805
// FUNCTION: CARMA2_HW 0x00488b00
void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF) {
    int count;

    count = GetAnInt(pF);

    for (; count > 0; count--) {
        char s[256];
        tPhysics_shape* shape;

        GetAString(pF, s);
        if (DRStricmp(s, "box") == 0) {
            float a1, a2, a3;
            float b1, b2, b3;

            shape = (tPhysics_shape*)AllocateShapeBox(kMem_collision_shape);
            GetThreeFloats(pF, &a1, &a2, &a3);
            shape->box.common.bb.min.v[0] = a1;
            shape->box.common.bb.min.v[1] = a2;
            shape->box.common.bb.min.v[2] = a3;
            GetThreeFloats(pF, &b1, &b2, &b3);
            shape->box.common.bb.max.v[0] = b1;
            shape->box.common.bb.max.v[1] = b2;
            shape->box.common.bb.max.v[2] = b3;
        } else if (DRStricmp(s, "polyhedron") == 0) {
            int count_points;
            int j;
            float p1, p2, p3;

            count_points = GetAnInt(pF);
            if (count_points > 32) {
                sprintf(s, "Ow! Physics shape is mad (having %d points is silly)", count_points);
                PDFatalError(s);
            }
            shape = (tPhysics_shape*)AllocateShapePolyhedron(count_points, kMem_collision_shape);

            shape->polyhedron.polyhedron.count_points = count_points;
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF, &p1, &p2, &p3);
                shape->polyhedron.polyhedron.points[j].v[0] = p1;
                shape->polyhedron.polyhedron.points[j].v[1] = p2;
                shape->polyhedron.polyhedron.points[j].v[2] = p3;
            }
        } else if (DRStricmp(s, "sphere") == 0) {
            float c1, c2, c3;

            shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);

            shape->sphere.sphere.radius = GetAScalar(pF);
            GetThreeFloats(pF, &c1, &c2, &c3);
            shape->sphere.sphere.center.v[0] = c1;
            shape->sphere.sphere.center.v[1] = c2;
            shape->sphere.sphere.center.v[2] = c3;
        } else if (DRStricmp(s, "wire") == 0) {
            br_vector3 points[64];
            int count_points;
            int count_lines;
            int j;
            float w1, w2, w3;
            int index1, index2;
            br_vector3* dst_points;
            tPolyhedron_edge_indexes* dst_lines;

            count_points = GetAnInt(pF);
            if (count_points > 64) {
                FatalError(kFatalError_TooManyExtraPointsForCarIndex_S, " Too many points in wire frame shape ");
            }
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF, &w1, &w2, &w3);
                points[j].v[0] = w1;
                points[j].v[1] = w2;
                points[j].v[2] = w3;
            }
            count_lines = GetAnInt(pF);
            shape = (tPhysics_shape*)AllocateShapeWireFrame(count_points, count_lines, kMem_collision_shape);

            dst_points = shape->wireframe.wireframe.points;
            for (j = 0; j < count_points; j++) {
                dst_points[j] = points[j];
            }
            dst_lines = shape->wireframe.wireframe.lines;
            for (j = 0; j < count_lines; j++) {
                GetPairOfInts(pF, &index1, &index2);
                dst_lines[j].index1 = index1;
                dst_lines[j].index2 = index2;
            }
        } else {
            FatalError(kFatalError_ShapeDataIsWrong);
        }
        FillInShape(shape);
        *pShape = shape;
        pShape = &shape->common.next;
    }
}

// based on carpocalypse2 pedestrn.c:1288
// FUNCTION: CARMA2_HW 0x004079a0
tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pName, br_matrix34* pMat) {
    tPed_character_instance* instance;

    instance = BrMemAllocate(sizeof(tPed_character_instance), kBoner_mem_type_char_instance);
    if (instance == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    instance->personality = FindOrOpenPersonality(pName);
    instance->field_0x4 = 0xff;
    instance->field_0x7 = 0xff;
    instance->field_0x6 = 0xff;
    instance->field_0x28 = 0;
    instance->field_0x2c = *pMat;
    instance->field_0x8 = 0xff;
    instance->field_0x9 = 0;
    instance->field_0xa = 0;
    instance->field_0x14 = 0;
    instance->field_0xc = 0;
    BrMatrix34Identity(&instance->field_0x5c);
    BrMatrix34Identity(&instance->field_0x8c);
    return instance;
}

// based on carpocalypse2 pedestrn.c:1847
// FUNCTION: CARMA2_HW 0x004d3360
void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pCharacter, br_vector3* pDir, br_vector3* pUp) {
    tPhysics_object* collision_info;

    if (pCharacter->field_0x14 & 0x1) {
        collision_info = pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
    } else {
        collision_info = GetRootObject(pCharacter);
    }
    if (!gAction_replay_mode || gBOOL_00744804) {
        if (!(pCharacter->field_0x14 & 0x4) && gProgram_state.racing) {
            PipeSinglePedDir((((short)pCharacter->ped->field_0x06) << 16) | (pCharacter->ped - gPedestrian_array), pDir);
        }
        SetCharacterDirection(pCharacter, pDir, pUp);
        if (gProgram_state.racing && OrientationChanged(&collision_info->transform_matrix, &collision_info->actor->t.t.mat, .7f)) {
            AssertRootObjectsMatrix(pCharacter);
        }
    }
}

extern tPed_form* gPed_forms[20];
extern tPedForms_vtable* gPed_vtable;
extern br_material* gPedestrian_character_cloned_materials[50];
extern int gCount_pedestrian_personality_cloned_materials;
extern const char* gPed_personality_grounding_type_names[1];
extern tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);
extern void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);
extern tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* based on carpocalypse2 pedestrn.c (inlined in the original) */
static tPed_form* FindOrOpenForm(const char* pName) {
    int i;

    for (i = 0; i < 20; i++) {
        if (gPed_forms[i] != NULL && strcmp(pName, gPed_forms[i]->name) == 0) {
            return gPed_forms[i];
        }
    }
    return SetUpCharacterForm(pName);
}

/* based on carpocalypse2 pedestrn.c:4758 (inlined in the original) */
static void RemapVector(br_vector3* pV, const tPed_remap_bone* pRemap) {
    int i;
    br_vector3 original;

    if (pRemap == NULL) {
        return;
    }
    original = *pV;
    for (i = 0; i < 3; i++) {
        pV->v[i] = original.v[pRemap->general_remap[i] % 3];
        if (pRemap->general_remap[i] >= 3) {
            pV->v[i] = -pV->v[i];
        }
    }
}

/* based on carpocalypse2 pedestrn.c:5312 (inlined in the original) */
static void RemapModelAxis(br_model* pModel, const tPed_remap_bone* pRemap) {
    int i;

    if (pRemap != NULL) {
        for (i = 0; i < pModel->nvertices; i++) {
            RemapVector(&pModel->vertices[i].p, pRemap);
        }
    }
    BrModelUpdate(pModel, BR_MODU_ALL);
}

/* based on carpocalypse2 pedestrn.c:5323 (inlined in the original; faces of the clone get the cloned material) */
static br_model* BonerCloneModel(br_model* pModel, int pIndex, int pUpdateable) {
    char identifier[40];
    char* str;
    br_model* clone;
    br_material* original_material;
    br_material* cloned_material;
    br_material* material;
    int i;

    strcpy(identifier, pModel->identifier);
    str = strtok(identifier, ".");
    sprintf(str + strlen(str), "%d.DAT", pIndex);
    clone = BrModelAllocate(str, pModel->nvertices, pModel->nfaces);
    memcpy(clone->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    memcpy(clone->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    strcpy(identifier, pModel->faces->material->identifier);
    str = strtok(identifier, ".");
    sprintf(str + strlen(str), "%d.MAT", pIndex);
    original_material = pModel->faces[0].material;
    cloned_material = BrMaterialFind(str);
    if (cloned_material == NULL && gCount_pedestrian_personality_cloned_materials < 50) {
        cloned_material = DRMaterialClone(original_material, 0);
        cloned_material->identifier = BrResStrDup(cloned_material, str);
        BrMaterialUpdate(cloned_material, BR_MATU_ALL);
        gPedestrian_character_cloned_materials[gCount_pedestrian_personality_cloned_materials] = cloned_material;
        gCount_pedestrian_personality_cloned_materials++;
    }
    for (i = 0; i < clone->nfaces; i++) {
        material = pModel->faces[i].material;
        if (material == original_material) {
            clone->faces[i].material = cloned_material;
        } else {
            clone->faces[i].material = material;
        }
    }
    if (pUpdateable) {
        clone->flags |= BR_MODF_UPDATEABLE;
    } else {
        clone->flags &= ~BR_MODF_UPDATEABLE;
    }
    BrModelAdd(clone);
    return clone;
}

/* based on carpocalypse2 pedestrn.c:5363 (inlined in the original) */
static float CalcBoundsRadius(const br_bounds3* pBounds) {
    float v;
    float r;

    r = 0.f;
    v = (float)fabs(pBounds->min.v[0]);
    if (v > r) {
        r = v;
    }
    v = (float)fabs(pBounds->min.v[1]);
    if (v > r) {
        r = v;
    }
    v = (float)fabs(pBounds->min.v[2]);
    if (v > r) {
        r = v;
    }
    v = (float)fabs(pBounds->max.v[0]);
    if (v > r) {
        r = v;
    }
    v = (float)fabs(pBounds->max.v[1]);
    if (v > r) {
        r = v;
    }
    v = (float)fabs(pBounds->max.v[2]);
    if (v > r) {
        r = v;
    }
    return r;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* based on carpocalypse2 pedestrn.c:5397, control flow follows the original */
// FUNCTION: CARMA2_HW 0x00406ab0
tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName) {
    FILE* f;
    FILE* move_f;
    tPed_personality* personality;
    char s[256];
    char default_moves_file[40];
    char bone_name[40];
    int i;
    int bone_index;
    int model_i;
    int j;
    int k;
    int m;
    int done;
    int move_id;
    char* str;
    char* next_str;
    tPhysics_joint* hinge_copy;

    f = BonerOpenPersonality(pName);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pName);
    }
    personality = BrMemAllocate(sizeof(tPed_personality), kBoner_mem_type_person);
    if (personality == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }

    GetALineAndDontArgue(f, s);
    personality->form = FindOrOpenForm(s);
    if (personality->form == NULL) {
        FatalError(kFatalError_BonerError_UnableToLinkForm_to_personality_S, s);
    }

    GetALineAndDontArgue(f, default_moves_file);
    BonerReadPersonalityModels(pName);

    GetALineAndDontArgue(f, s);
    if (strcmp("START OF BONES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }

    personality->bones = BrMemAllocate(personality->form->count_bones * sizeof(tPed_personality_bone), kBoner_mem_type_person_parts);
    for (i = 0; i < personality->form->count_bones; i++) {
        GetALineAndDontArgue(f, bone_name);
        for (bone_index = 0; bone_index < personality->form->count_bones; bone_index++) {
            if (strcmp(bone_name, personality->form->bones[bone_index].name) == 0) {
                break;
            }
        }
        GetALineAndDontArgue(f, s);
        if (DRStricmp(s, "NULL") != 0 && bone_index >= personality->form->count_bones) {
            FatalError(kFatalError_BonerError_UnableToFindBone_S, bone_name);
        }
        str = s;
        model_i = 0;
        do {
            next_str = strchr(str, ',');
            if (next_str != NULL) {
                *next_str = '\0';
                next_str++;
            } else {
                next_str = NULL;
            }
            if (DRStricmp(str, "NULL") != 0) {
                personality->bones[bone_index].models[model_i].models[0] = BrModelFind(str);
                if (personality->bones[bone_index].models[model_i++].models[0] == NULL) {
                    FatalError(kFatalError_BonerError_UnableToFindModel_S, str);
                }
                RemapModelAxis(personality->bones[bone_index].models[model_i - 1].models[0], personality->form->bones[bone_index].remapped_bone);
            }
            str = next_str;
        } while (str != NULL);
        for (j = 0; j < 4; j++) {
            if (personality->bones[bone_index].models[j].models[0] == NULL) {
                break;
            }
            for (k = 1; k < 2; k++) {
                personality->bones[bone_index].models[j].models[k] = BonerCloneModel(personality->bones[bone_index].models[j].models[0], k, 1);
            }
            personality->bones[bone_index].models[j].models[0]->flags |= BR_MODF_UPDATEABLE;
            BrModelUpdate(personality->bones[bone_index].models[j].models[0], BR_MODU_ALL);
        }
        if (personality->form->bones[bone_index].indices[0] >= 0) {
            GetAVector3(f, &personality->bones[bone_index].field_0x20);
            RemapVector(&personality->bones[bone_index].field_0x20, personality->form->bones[bone_index].remapped_bone);
            GetAVector3(f, &personality->bones[bone_index].field_0x2c);
            RemapVector(&personality->bones[bone_index].field_0x2c, personality->form->bones[personality->form->bones[bone_index].indices[0]].remapped_bone);
            if (personality->form->bones[bone_index].hinge != NULL) {
                hinge_copy = ClonePhysicsJoint(personality->form->bones[bone_index].hinge, kMem_physics_joint);
                BrVector3Copy(&hinge_copy->field_0x08, &personality->bones[bone_index].field_0x20);
                BrVector3Copy(&hinge_copy->field_0x14, &personality->bones[bone_index].field_0x2c);
                personality->bones[bone_index].hinge = hinge_copy;
            }
        }
    }
    personality->M = personality->form->simple_physicing[0].collision_info->M;
    GetALineAndDontArgue(f, s);
    if (strcmp("END OF BONES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "END OF BONES");
    }
    GetALineAndDontArgue(f, s);
    if (strcmp("START OF MOVES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF MOVES");
    }

    personality->moves = BrMemAllocate(personality->form->count_moves * sizeof(tPed_personality_move), kBoner_mem_type_person_moves);
    for (k = 0; k < personality->form->count_moves; k++) {
        personality->moves[k].scale_factor = 1.f;
        personality->moves[k].grounding_mode = ePed_personality_floating;
    }
    move_f = BonerOpenDefaultMoves(default_moves_file);
    if (move_f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, default_moves_file);
    }
    GetALineAndDontArgue(move_f, s);
    if (strcmp("START OF MOVES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF MOVES");
    }
    do {
        GetALineAndDontArgue(move_f, s);
        done = strcmp(s, "END OF MOVES") == 0;
        if (!done) {
            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &move_id);
            for (m = 0; m < personality->form->count_moves; m++) {
                if (personality->form->moves[m].id == move_id) {
                    personality->moves[m].scale_factor = GetAScalar(move_f);
                    personality->moves[m].grounding_mode = GetALineAndInterpretCommand(move_f, gPed_personality_grounding_type_names, 1);
                    personality->moves[m].grounding_offset = GetAScalar(move_f);
                    break;
                }
            }
            if (m == personality->form->count_moves) {
                sprintf(s, "%s/%d", personality->name, move_id);
                FatalError(kFatalError_BonerError_IllegalMoveID_S, s);
            }
        }
        if (PFfeof(move_f) || done) {
            if (move_f != f) {
                PFfclose(move_f);
                move_f = f;
            } else {
                move_f = NULL;
            }
        }
    } while (move_f != NULL);

    GetAVector3(f, &personality->bb.min);
    GetAVector3(f, &personality->bb.max);
    RemapVector(&personality->bb.min, personality->form->bones[0].remapped_bone);
    RemapVector(&personality->bb.max, personality->form->bones[0].remapped_bone);
    if (personality->bb.min.v[0] > personality->bb.max.v[0]) {
        float tv;
        tv = personality->bb.min.v[0];
        personality->bb.min.v[0] = personality->bb.max.v[0];
        personality->bb.max.v[0] = tv;
    }
    if (personality->bb.min.v[1] > personality->bb.max.v[1]) {
        float tv;
        tv = personality->bb.min.v[1];
        personality->bb.min.v[1] = personality->bb.max.v[1];
        personality->bb.max.v[1] = tv;
    }
    if (personality->bb.min.v[2] > personality->bb.max.v[2]) {
        float tv;
        tv = personality->bb.min.v[2];
        personality->bb.min.v[2] = personality->bb.max.v[2];
        personality->bb.max.v[2] = tv;
    }
    GetAVector3(f, &personality->centre_of_mass);
    RemapVector(&personality->centre_of_mass, personality->form->bones[0].remapped_bone);
    personality->radius = CalcBoundsRadius(&personality->bb);

    if (gPed_vtable->load_personality != NULL) {
        gPed_vtable->load_personality(personality, f);
    }
    PFfclose(f);
    strcpy(personality->name, pName);
    for (i = 0; i < 50; i++) {
        if (gPed_personalities[i] == NULL) {
            gPed_personalities[i] = personality;
            return personality;
        }
    }
    FatalError(kFatalError_BonerError_TooManyPersonalitiesLoaded, 0);
}

extern FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);
extern FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);
extern tPed_remap* gPed_remaps[10];
extern const char* gPed_remap_axis_choices[6];
extern tPed_move* gPed_moves[250];
extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);
extern tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name);
extern void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
extern float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees);
extern float gFloat_sine_table[91];
extern const br_vector3 g_Ped_x_unit_vector;
extern const br_vector3 g_Ped_y_unit_vector;
extern const br_vector3 g_Ped_z_unit_vector;
extern const char* gPed_form_collision_type_names[9];
extern const char* gPed_form_bone_joint_type_names[10];
extern const char* gPed_form_joint_limit_type_names[2];
extern const br_vector3 g_Ped_x_unit_vector;
extern const br_vector3 g_Ped_y_unit_vector;
extern const br_vector3 g_Ped_z_unit_vector;

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* BrVector3Cross as an inlined helper (operand order of the original) */
static void PedVector3Cross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB) {
    BrVector3Cross(pDest, pA, pB);
}

/* based on carpocalypse2 pedestrn.c:4776 (inlined in the original) */
static tPed_remap* FindOrOpenRemap(const char* pName) {
    int i;

    for (i = 0; i < 10; i++) {
        if (gPed_remaps[i] != NULL && strcmp(pName, gPed_remaps[i]->name) == 0) {
            return gPed_remaps[i];
        }
    }
    return ReadRemap(pName);
}

/* based on carpocalypse2 pedestrn.c:4787 (inlined in the original) */
static void Flip3DStoBRaxes(br_matrix34* pMat) {
    br_vector3 tv;
    float f;

    BrVector3Copy(&tv, (br_vector3*)pMat->m[1]);
    BrVector3Copy((br_vector3*)pMat->m[1], (br_vector3*)pMat->m[2]);
    BrVector3Negate((br_vector3*)pMat->m[2], &tv);

    f = pMat->m[0][1];
    pMat->m[0][1] = pMat->m[0][2];
    pMat->m[0][2] = f * -1.f;

    f = pMat->m[1][1];
    pMat->m[1][1] = pMat->m[1][2];
    pMat->m[1][2] = f * -1.f;

    f = pMat->m[2][1];
    pMat->m[2][1] = pMat->m[2][2];
    pMat->m[2][2] = f * -1.f;

    f = pMat->m[3][1];
    pMat->m[3][1] = pMat->m[3][2];
    pMat->m[3][2] = f * -1.f;
}

/* based on carpocalypse2 pedestrn.c:4811 (inlined in the original) */
static tPed_move* ReadMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags) {
    FILE* f;
    tPed_move* move;
    int i;
    int j;
    int k;
    int l;
    char bone_names[50][36];
    tS32 count_bones_in_anim_file;
    br_vector3 rot_pos;
    br_vector3 move_pos;
    br_matrix34 mat;
    br_matrix34 mat_lp;
    br_euler e;
    tPed_move_frame* frame;

    f = BonerOpenMoves(pName);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pName);
    }
    move = BrMemAllocate(sizeof(tPed_move), kBoner_mem_type_sequence);
    if (move == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    count_bones_in_anim_file = ReadS32(f);
    move->count_frames = (tS16)ReadS32(f);
    for (i = 0; i < count_bones_in_anim_file; i++) {
        PFfread(bone_names[i], 1, 33, f);
        PFfseek(f, sizeof(tU32) * ReadS32(f), SEEK_CUR);
    }
    BrVector3Set(&move_pos, 0.f, 0.f, 0.f);
    move->frames = BrMemAllocate(move->count_frames * sizeof(tPed_move_frame), kMem_boner_type_frames);
    frame = move->frames;
    for (i = 0; i < move->count_frames; i++, frame++) {
        frame->axis = BrMemAllocate(pForm->count_bones * sizeof(tPed_move_frame_axis), kBoner_mem_type_frame_2);
        for (j = 0; j < count_bones_in_anim_file; j++) {
            for (k = 0; k < 4; k++) {
                for (l = 0; l < 3; l++) {
                    mat.m[k][l] = ReadF32(f);
                }
            }
            for (k = 0; k < pForm->count_bones; k++) {
                if (strcmp(bone_names[j], pForm->bones[k].name) == 0) {
                    Flip3DStoBRaxes(&mat);
                    BrMatrix34LPNormalise(&mat_lp, &mat);
                    if (pForm->bones[k].indices[0] == -1) {
                        memcpy(&frame->mat, &mat_lp, 9 * sizeof(float));
                        if (i != 0) {
                            BrVector3Sub((br_vector3*)frame->mat.m[3], (br_vector3*)mat_lp.m[3], &rot_pos);
                            BrVector3Accumulate(&move_pos, (br_vector3*)frame->mat.m[3]);
                        }
                        rot_pos = *(br_vector3*)mat_lp.m[3];
                    }
                    e.order = 0;
                    BrMatrix34ToEuler(&e, &mat_lp);
                    frame->axis[k].euler1 = e.a / 256;
                    frame->axis[k].euler2 = e.b / 256;
                    frame->axis[k].euler3 = e.c / 256;
                    break;
                }
            }
        }
    }
    if (strchr(pLooping_reset_flags, 'X') != NULL) {
        move->frames[0].mat.m[3][0] = -move_pos.v[0];
        move_pos.v[0] = 0.f;
        move->looping_reset_flags |= 0x1;
    } else if (move->count_frames > 1) {
        move->frames[0].mat.m[3][0] = move->frames[1].mat.m[3][0];
        move_pos.v[0] += move->frames[0].mat.m[3][0];
    } else {
        move->frames[0].mat.m[3][0] = 0.f;
    }
    if (strchr(pLooping_reset_flags, 'Y') != NULL) {
        move->frames[0].mat.m[3][1] = -move_pos.v[1];
        move_pos.v[1] = 0.f;
        move->looping_reset_flags |= 0x2;
    } else if (move->count_frames > 1) {
        move->frames[0].mat.m[3][1] = move->frames[1].mat.m[3][1];
        move_pos.v[1] += move->frames[0].mat.m[3][1];
    } else {
        move->frames[0].mat.m[3][1] = 0.f;
    }
    if (strchr(pLooping_reset_flags, 'Z') != NULL) {
        move->frames[0].mat.m[3][2] = -move_pos.v[2];
        move_pos.v[2] = 0.f;
        move->looping_reset_flags |= 0x4;
    } else if (move->count_frames > 1) {
        move->frames[0].mat.m[3][2] = move->frames[1].mat.m[3][2];
        move_pos.v[2] += move->frames[0].mat.m[3][2];
    } else {
        move->frames[0].mat.m[3][2] = 0.f;
    }

    if (BrVector3LengthSquared(&move_pos) != 0.f && !(move->looping_reset_flags & 0x1)) {
        BrVector3Normalise(&move->field_0x34, &move_pos);
        move->field_0x2a = 0;
        if (BrVector3LengthSquared(pMovement_direction) == 0.f) {
            move->field_0x40 = move->field_0x34;
        } else {
            BrVector3Copy(&move->field_0x40, pMovement_direction);
        }
    } else {
        move->field_0x2a = 1;
        if (BrVector3LengthSquared(pMovement_direction) == 0.f) {
            BrVector3Set(&move->field_0x40, 0.f, 0.f, 1.f);
        } else {
            BrVector3Copy(&move->field_0x40, pMovement_direction);
        }
        BrVector3Set(&move->field_0x34, 0.f, 0.f, 0.f);
    }
    move->move_flags = pFlags;
    PFfclose(f);
    strcpy(move->name, pName);
    for (i = 0; i < 250; i++) {
        if (gPed_moves[i] == NULL) {
            gPed_moves[i] = move;
            return move;
        }
    }
    FatalError(kFatalError_BonerError_TooManyMovesLoaded, 0);
}

/* based on carpocalypse2 pedestrn.c:4957 (inlined in the original) */
static tPed_move* FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags) {
    int i;

    for (i = 0; i < 250; i++) {
        if (gPed_moves[i] != NULL && strcmp(pName, gPed_moves[i]->name) == 0) {
            return gPed_moves[i];
        }
    }
    return ReadMove(pName, pForm, pLooping_reset_flags, pMovement_direction, pFlags);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* based on carpocalypse2 pedestrn.c:4970, control flow follows the original */
// FUNCTION: CARMA2_HW 0x00404f60
tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName) {
    FILE* f;
    tPed_form* form;
    int max_simple_physicing_at_once;
    int max_boned_physicing_at_once;
    int max_rendering_at_once;
    tPed_form_move_buffer_item form_moves[100];
    char s[256];
    int i;
    int j;
    int done;
    float total_mass;
    float mass;
    int collision_type;
    int joint_type;
    tPhysics_joint* joint;
    int count_limits;
    int is_rotate;
    br_scalar angle;
    br_matrix34 mat;
    tPhysics_object* coll_info;
    char* str;

    total_mass = 0.f;
    f = BonerOpenCharacterForm(pName);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pName);
    }
    form = BrMemAllocate(sizeof(tPed_form), kBoner_mem_type_form);
    if (form == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    max_simple_physicing_at_once = GetAnInt(f);
    max_boned_physicing_at_once = GetAnInt(f);
    max_rendering_at_once = GetAnInt(f);
    form->max_stored_dismembered_characters = GetAnInt(f);
    GetALineAndDontArgue(f, s);
    form->remap = FindOrOpenRemap(s);

    done = 0;
    while (!PFfeof(f) && !done) {
        GetALineAndDontArgue(f, s);
        if (strcmp(s, "NEXT BONE") == 0) {
            form->count_bones++;
        } else {
            done = strcmp(s, "END OF BONES") == 0;
            if (done) {
                form->count_bones++;
            }
        }
    }
    form->max_boned_physicing_at_once = max_boned_physicing_at_once;
    form->max_simple_physicing_at_once = max_simple_physicing_at_once;
    form->max_rendering_at_once = max_rendering_at_once;
    form->actor_sets = BrMemAllocate(max_rendering_at_once * sizeof(tPed_form_actor_set), kBoner_mem_type_actor_sets);
    form->simple_physicing = BrMemAllocate(max_simple_physicing_at_once * sizeof(tPed_form_simple_phys), kBoner_mem_type_simple_phys);
    form->boned_physicing = BrMemAllocate(max_boned_physicing_at_once * sizeof(tPed_form_boned_phys), kBoner_mem_type_boned_phys);
    form->bones = BrMemAllocate(form->count_bones * sizeof(tPed_form_bone), kBoner_mem_type_bone_info);
    PFrewind(f);
    for (i = 0; i < 5; i++) {
        GetALineAndDontArgue(f, s);
    }
    GetALineAndDontArgue(f, s);
    if (strcmp("START OF BONES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }
    for (i = 0; i < max_boned_physicing_at_once; i++) {
        form->boned_physicing[i].collision_infos = BrMemAllocate(form->count_bones * sizeof(tPhysics_object*), kBoner_mem_type_boned_coll);
    }
    for (i = 0; i < form->count_bones; i++) {
        GetALineAndDontArgue(f, form->bones[i].name);
        form->bones[i].remapped_bone = NULL;
        if (form->remap != NULL) {
            for (j = 0; j < form->remap->count_bones; j++) {
                if (strcmp(form->remap->bones[j].name, form->bones[i].name) == 0) {
                    form->bones[i].remapped_bone = &form->remap->bones[j];
                    break;
                }
            }
        }
        mass = GetAScalar(f) * .001;
        total_mass += mass;
        if (mass != 0.f) {
            collision_type = GetALineAndInterpretCommand(f, gPed_form_collision_type_names, 9);
            joint_type = GetALineAndInterpretCommand(f, gPed_form_bone_joint_type_names, 10);
            if (joint_type > 0) {
                form->bones[i].indices[0] = GetAnInt(f);
            } else {
                form->bones[i].indices[0] = -1;
            }
            if (joint_type > 0 && joint_type < 8) {
                count_limits = GetAnInt(f);
                joint = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                joint->type = joint_type;
                joint->count_limits = count_limits;
                is_rotate = 0;
                for (j = 0; j < count_limits; j++) {
                    if (is_rotate) {
                        is_rotate = 0;
                        angle = GetAScalar(f);
                        DRVector3SafeCross(&joint->limits[j].child, &joint->limits[j - 1].child, &joint->limits[j - 1].parent);
                        BrMatrix34Rotate(&mat, BR_ANGLE_DEG(angle), &joint->limits[j - 1].parent);
                        BrMatrix34ApplyV(&joint->limits[j].parent, &joint->limits[j].child, &mat);
                    } else {
                        joint->limits[j].type = GetALineAndInterpretCommand(f, gPed_form_joint_limit_type_names, 2);
                        GetAVector3(f, &joint->limits[j].child);
                        GetAVector3(f, &joint->limits[j].parent);
                        RemapVector(&joint->limits[j].child, form->bones[i].remapped_bone);
                        RemapVector(&joint->limits[j].parent, form->bones[form->bones[i].indices[0]].remapped_bone);
                        is_rotate = joint->limits[j].type == eJoint_limit_universal;
                        joint->limits[j].value = FastFloatCos((int)GetAScalar(f));
                    }
                    BrVector3Normalise(&joint->limits[j].child, &joint->limits[j].child);
                    BrVector3Normalise(&joint->limits[j].parent, &joint->limits[j].parent);
                }
                if (joint_type == eJoint_hinge || joint_type == eJoint_quick_hinge) {
                    GetAVector3(f, &joint->hinge_axis);
                    joint->parent_bone_axis = joint->hinge_axis;
                    RemapVector(&joint->hinge_axis, form->bones[i].remapped_bone);
                    RemapVector(&joint->parent_bone_axis, form->bones[form->bones[i].indices[0]].remapped_bone);
                    if ((float)fabs(joint->hinge_axis.v[0]) > (float)fabs(joint->hinge_axis.v[1]) && (float)fabs(joint->hinge_axis.v[0]) > (float)fabs(joint->hinge_axis.v[2])) {
                        PedVector3Cross(&joint->hinge_axis2, &joint->hinge_axis, &g_Ped_y_unit_vector);
                        PedVector3Cross(&joint->hinge_axis3, &joint->hinge_axis, &g_Ped_z_unit_vector);
                    } else if ((float)fabs(joint->hinge_axis.v[1]) > (float)fabs(joint->hinge_axis.v[0]) && (float)fabs(joint->hinge_axis.v[1]) > (float)fabs(joint->hinge_axis.v[2])) {
                        PedVector3Cross(&joint->hinge_axis2, &joint->hinge_axis, &g_Ped_x_unit_vector);
                        PedVector3Cross(&joint->hinge_axis3, &joint->hinge_axis, &g_Ped_z_unit_vector);
                    } else {
                        PedVector3Cross(&joint->hinge_axis2, &joint->hinge_axis, &g_Ped_x_unit_vector);
                        PedVector3Cross(&joint->hinge_axis3, &joint->hinge_axis, &g_Ped_y_unit_vector);
                    }
                }
            } else {
                joint = NULL;
            }
            form->bones[i].hinge = joint;
            for (j = 0; j < max_boned_physicing_at_once; j++) {
                form->boned_physicing[j].collision_infos[i] = BrMemAllocate(sizeof(tPhysics_object), kBoner_mem_type_boned_coll_obj);
                coll_info = form->boned_physicing[j].collision_infos[i];
                coll_info->uid = i;
                coll_info->M = mass;
                switch (collision_type) {
                case ePed_form_collision_type_box:
                    coll_info->shape = (tPhysics_shape*)AllocateShapeBox(kMem_collision_shape);
                    break;
                case ePed_form_collision_type_line:
                    coll_info->shape = (tPhysics_shape*)AllocateShapeWireFrame(2, 1, kMem_collision_shape);
                    coll_info->shape->wireframe.wireframe.lines[0].index1 = 0;
                    coll_info->shape->wireframe.wireframe.lines[0].index2 = 1;
                    break;
                case ePed_form_collision_type_sphere:
                    coll_info->shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);
                    break;
                case ePed_form_collision_type_polyhedron:
                case ePed_form_collision_type_polygon:
                case ePed_form_collision_type_none:
                    coll_info->shape = NULL;
                    break;
                }
                form->boned_physicing[j].field_0x0 = 0;
            }
        } else {
            for (j = 0; j < max_boned_physicing_at_once; j++) {
                form->boned_physicing[j].collision_infos[i] = NULL;
                form->boned_physicing[j].field_0x0 = 0;
            }
        }
        GetALineAndDontArgue(f, s);
        if (strcmp("NEXT BONE", s) != 0 && strcmp("END OF BONES", s) != 0) {
            FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "NEXT BONE");
        }
    }
    for (i = 0; i < max_simple_physicing_at_once; i++) {
        form->simple_physicing[i].collision_info = BrMemAllocate(sizeof(tPhysics_object), kBoner_mem_type_simple_coll_obj);
        coll_info = form->simple_physicing[i].collision_info;
        coll_info->M = total_mass;
        coll_info->shape = (tPhysics_shape*)AllocateShapeBox(kMem_collision_shape);
        *(tU8*)&form->simple_physicing[i].type = ePed_form_collision_type_box;
    }
    for (i = 0; i < max_rendering_at_once; i++) {
        form->actor_sets[i].actors = BrMemAllocate(form->count_bones * sizeof(br_actor*), kBoner_mem_type_actor_ptrs);
        for (j = 0; j < form->count_bones; j++) {
            form->actor_sets[i].actors[j] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            form->actor_sets[i].actors[j]->render_style = BR_RSTYLE_FACES;
        }
        form->actor_sets[i].field_0x0 = 0;
    }
    GetALineAndDontArgue(f, s);
    if (strcmp("START OF MOVES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF MOVES");
    }
    form->count_moves = 0;
    do {
        GetALineAndDontArgue(f, s);
        done = strcmp(s, "END OF MOVES") == 0;
        if (!done) {
            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &form_moves[form->count_moves].id);
            form_moves[form->count_moves].frametime = 1000 / GetAnInt(f);
            GetAString(f, form_moves[form->count_moves].looping_reset_flags);
            GetAVector3(f, &form_moves[form->count_moves].movement_direction);
            if (DRVector3NonZero(&form_moves[form->count_moves].movement_direction)) {
                BrVector3Normalise(&form_moves[form->count_moves].movement_direction, &form_moves[form->count_moves].movement_direction);
            }
            GetAString(f, form_moves[form->count_moves].biped_export_filename);
            form_moves[form->count_moves].flags = GetAnInt(f);
            form->count_moves++;
        }
    } while (!PFfeof(f) && !done);
    if (!done) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "END OF MOVES");
    }
    if (gPed_vtable->load_form != NULL) {
        gPed_vtable->load_form(form, f);
    }
    PFfclose(f);
    form->moves = BrMemAllocate(form->count_moves * sizeof(tPed_form_move), kBoner_mem_type_moves);
    for (i = 0; i < form->count_moves; i++) {
        form->moves[i].id = form_moves[i].id;
        form->moves[i].move = FindOrOpenMove(form_moves[i].biped_export_filename, form, form_moves[i].looping_reset_flags, &form_moves[i].movement_direction, form_moves[i].flags);
        form->moves[i].move->frametime = form_moves[i].frametime;
    }
    form->stored_dismembered_characters = BrMemAllocate(form->max_stored_dismembered_characters * sizeof(tPed_form_dismembered_character*), kBoner_mem_type_stored_ptrs);
    for (i = 0; i < form->max_stored_dismembered_characters; i++) {
        form->stored_dismembered_characters[i] = BrMemAllocate(form->count_bones * sizeof(tPed_form_dismembered_character), kBoner_mem_type_stored);
    }
    strcpy(form->name, pName);
    for (i = 0; i < 20; i++) {
        if (gPed_forms[i] == NULL) {
            gPed_forms[i] = form;
            return form;
        }
    }
    FatalError(kFatalError_BonerError_TooManyFormsLoaded, 0);
}

// FUNCTION: CARMA2_HW 0x00404410
tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name) {
    FILE* f;
    tPed_remap* remap;
    char s[256];
    int i;
    int j;

    if (strcmp(pFile_name, "NONE") == 0) {
        return NULL;
    }
    f = BonerOpenRemaps(pFile_name);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pFile_name);
    }
    remap = BrMemAllocate(sizeof(tPed_remap), kBoner_mem_type_remap);
    if (remap == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    remap->count_bones = 0;
    GetALineAndDontArgue(f, s);
    if (strcmp("START OF BONES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }
    while (!PFfeof(f)) {
        GetALineAndDontArgue(f, s);
        if ((remap->count_bones == 0 && strcmp("END OF BONES", s) != 0) || strcmp("NEXT BONE", s) == 0) {
            remap->count_bones++;
        }
    }
    remap->bones = BrMemAllocate(sizeof(tPed_remap_bone) * remap->count_bones, kBoner_mem_type_remap);
    PFrewind(f);
    GetALineAndDontArgue(f, s);
    for (i = 0; i < remap->count_bones; i++) {
        GetALineAndDontArgue(f, remap->bones[i].name);
        for (j = 0; j < 3; j++) {
            remap->bones[i].general_remap[j] = GetALineAndInterpretCommand(f, gPed_remap_axis_choices, REC2_ASIZE(gPed_remap_axis_choices));
        }
        for (j = 0; j < 3; j++) {
            remap->bones[i].powerup_axis[j] = GetALineAndInterpretCommand(f, gPed_remap_axis_choices, REC2_ASIZE(gPed_remap_axis_choices));
        }
        GetALineAndDontArgue(f, s);
    }
    strcpy(remap->name, pFile_name);
    for (i = 0; i < (int)REC2_ASIZE(gPed_remaps); i++) {
        if (gPed_remaps[i] == NULL) {
            gPed_remaps[i] = remap;
            return remap;
        }
    }
    FatalError(kFatalError_BonerError_TooManyRemapsLoaded, 0);
}

// FUNCTION: CARMA2_HW 0x00403f90
void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB) {
    float z;

    pDest->v[0] = pA->v[1] * pB->v[2] - pA->v[2] * pB->v[1];
    pDest->v[1] = pA->v[2] * pB->v[0] - pA->v[0] * pB->v[2];
    pDest->v[2] = z = pA->v[0] * pB->v[1] - pA->v[1] * pB->v[0];
    if ((float)fabs(pDest->v[0]) < 2.384186e-6f && (float)fabs(pDest->v[1]) < 2.384186e-6f && (float)fabs(z) < 2.384186e-6f) {
        if ((float)fabs(pA->v[0]) <= (float)fabs(pA->v[1]) && (float)fabs(pA->v[0]) <= (float)fabs(pA->v[2])) {
            BrVector3Cross(pDest, pA, &g_Ped_x_unit_vector);
        } else if ((float)fabs(pA->v[1]) <= (float)fabs(pA->v[0]) && (float)fabs(pA->v[1]) <= (float)fabs(pA->v[2])) {
            BrVector3Cross(pDest, pA, &g_Ped_y_unit_vector);
        } else {
            BrVector3Cross(pDest, pA, &g_Ped_z_unit_vector);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00511440
float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees) {

    if (pAngle_in_degrees < 0) {
        pAngle_in_degrees = 360 - -pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    }
    if (pAngle_in_degrees > 270) {
        return gFloat_sine_table[pAngle_in_degrees - 270];
    }
    if (pAngle_in_degrees > 180) {
        return -*(gFloat_sine_table + 270 - pAngle_in_degrees);
    }
    if (pAngle_in_degrees > 90) {
        return -gFloat_sine_table[pAngle_in_degrees - 90];
    }
    return *(gFloat_sine_table + 90 - pAngle_in_degrees);
}

extern void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest);
extern void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
extern void C2_HOOK_FASTCALL DRMatrix34RotateCos(br_matrix34* pDest, const br_vector3* pAxis, float pCosA);
extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);

#define SCD_NORMALISE_D(v1, v2)                                  \
    do {                                                         \
        len = BR_LENGTH3((v2)->v[0], (v2)->v[1], (v2)->v[2]);    \
        if (len > BR_SCALAR_EPSILON * 2) {                       \
            scale = 1.0 / len;                                   \
            (v1)->v[0] = (v2)->v[0] * scale;                     \
            (v1)->v[1] = (v2)->v[1] * scale;                     \
            (v1)->v[2] = (v2)->v[2] * scale;                     \
        } else {                                                 \
            (v1)->v[0] = 1.0f;                                   \
            (v1)->v[1] = 0.0f;                                   \
            (v1)->v[2] = 0.0f;                                   \
        }                                                        \
    } while (0)

/* based on upstream madebr/rec2 a373d899a src/carma2/common/pedestrn.c */
// FUNCTION: CARMA2_HW 0x0040a730
void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp) {
    double scale;
    br_scalar len;
    int keep_direction;
    tPed_move* move;
    br_matrix34* character_matrix;
    float cos_a;
    br_vector3 norm_dir;
    br_vector3 norm_up;
    br_vector3 local_138;
    br_vector3 local_12c;
    br_vector3 local_120;
    br_vector3 local_114;
    br_vector3 local_fc;
    br_vector3 local_e4;
    br_matrix34 inv_field_0x8c;
    br_matrix34 local_c0;
    br_matrix34 local_60;
    br_matrix34 local_30;
    br_vector3 original_pos;

    if (pPed->field_0x7 < 0) {
        return;
    }
    if ((pPed->field_0x14 & 0x4) && !gAction_replay_mode) {
        return;
    }
    if (pDir == NULL) {
        pDir = &pPed->field_0xc0;
        pUp = &pPed->field_0xcc;
        keep_direction = 1;
    } else {
        SCD_NORMALISE_D(&pPed->field_0xc0, pDir);
        pPed->field_0xcc = *pUp;
        keep_direction = 0;
    }
    move = pPed->personality->form->moves[pPed->field_0x7].move;
    if (pPed->field_0x4 < 0) {
        character_matrix = &pPed->field_0x2c;
    } else if (pPed->field_0xe8 != NULL) {
        character_matrix = pPed->field_0xe8;
    } else {
        character_matrix = &pPed->personality->form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34Inverse(&inv_field_0x8c, &pPed->field_0x8c);
    original_pos = *(br_vector3*)character_matrix->m[3];
    BrVector3Set((br_vector3*)character_matrix->m[3], 0.f, 0.f, 0.f);
    BrMatrix34Mul(&local_c0, character_matrix, &inv_field_0x8c);
    if (pUp != NULL) {
        SCD_NORMALISE_D(&norm_up, pUp);
    } else {
        BrVector3Set(&norm_up, 0.f, 1.f, 0.f);
    }
    SCD_NORMALISE_D(&norm_dir, pDir);
    BrVector3Copy(&local_12c, &move->field_0x40);
    if (pPed->field_0xbc != NULL) {
        local_12c.v[0] = (1.f - pPed->field_0xbc->field_0x0) * pPed->field_0xbc->field_0x20.v[0] + local_12c.v[0] * pPed->field_0xbc->field_0x0;
        local_12c.v[1] = (1.f - pPed->field_0xbc->field_0x0) * pPed->field_0xbc->field_0x20.v[1] + local_12c.v[1] * pPed->field_0xbc->field_0x0;
        local_12c.v[2] = (1.f - pPed->field_0xbc->field_0x0) * pPed->field_0xbc->field_0x20.v[2] + local_12c.v[2] * pPed->field_0xbc->field_0x0;
        BrVector3Normalise(&local_12c, &local_12c);
    }
    DropPointOntoPlane(&norm_dir, &norm_up, &local_e4);
    DropPointOntoPlane(&local_12c, &norm_up, &local_114);
    BrVector3Normalise(&local_fc, &local_e4);
    BrVector3Normalise(&local_114, &local_114);
    cos_a = BrVector3Dot(&local_fc, &local_114);
    OrthogonalVector(&local_138, &local_114, &local_fc);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&pPed->field_0x8c, &local_120, cos_a);
    BrVector3Set(&local_138, norm_up.v[2], 0.f, -norm_up.v[0]);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&local_60, &local_120, norm_up.v[1]);
    BrMatrix34Pre(&pPed->field_0x8c, &local_60);
    cos_a = BrVector3Dot(&local_fc, &norm_dir);
    DRVector3SafeCross(&local_138, &local_fc, &norm_dir);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&local_30, &local_120, cos_a);
    BrMatrix34Mul(&pPed->field_0x5c, &pPed->field_0x8c, &local_30);
    BrMatrix34Post(&local_c0, &pPed->field_0x8c);
    BrVector3Add((br_vector3*)local_c0.m[3], (br_vector3*)local_c0.m[3], &original_pos);
    BrMatrix34LPNormalise(character_matrix, &local_c0);
    if (!keep_direction) {
        SetCharacterBonePositions(pPed, 3, 0);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* based on carpocalypse2 pedestrn.c:160 */
// GLOBAL: CARMA2_HW 0x0058f230
const char* gPed_form_collision_type_names[9] = {
    "BOX",
    "POLYHEDRON",
    "LINE",
    "POLYGON",
    "SPHERE",
    "",
    "",
    "",
    "NONE",
};

// GLOBAL: CARMA2_HW 0x0058f258
const char* gPed_form_bone_joint_type_names[10] = {
    "NONE",
    "HINGE",
    "UNIVERSAL",
    "BALLNSOCKET",
    "QUICKHINGE",
    "",
    "",
    "",
    "PHANTOM",
    "FALSE",
};

// GLOBAL: CARMA2_HW 0x0058f280
const char* gPed_form_joint_limit_type_names[2] = {
    "PLANE",
    "UNIVERSAL",
};

/* based on carpocalypse2 pedestrn.c:194 */
// GLOBAL: CARMA2_HW 0x0058f288
const char* gPed_personality_grounding_type_names[1] = {
    "GROUND",
};

// GLOBAL: CARMA2_HW 0x0058f290
const char* gPed_remap_axis_choices[6] = {
    "+X",
    "+Y",
    "+Z",
    "-X",
    "-Y",
    "-Z",
};

/* based on carpocalypse2 pedestrn.c:151 */
// GLOBAL: CARMA2_HW 0x0058f2a8
const br_vector3 g_Ped_x_unit_vector = { { 1.f, 0.f, 0.f } };

// GLOBAL: CARMA2_HW 0x0058f2b8
const br_vector3 g_Ped_y_unit_vector = { { 0.f, 1.f, 0.f } };

// GLOBAL: CARMA2_HW 0x0058f2c8
const br_vector3 g_Ped_z_unit_vector = { { 0.f, 0.f, 1.f } };

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* based on carpocalypse2 pedestrn.c:1732 */
// FUNCTION: CARMA2_HW 0x00404150
void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB) {

    BrVector3Cross(pDest, pA, pB);
    if ((float)fabs(pDest->v[0]) < 2.384186e-06f && (float)fabs(pDest->v[1]) < 2.384186e-06f && (float)fabs(pDest->v[2]) < 2.384186e-06f) {
        if ((float)fabs(pA->v[0]) <= (float)fabs(pA->v[2])) {
            BrVector3Cross(pDest, pA, &g_Ped_x_unit_vector);
        } else {
            BrVector3Cross(pDest, pA, &g_Ped_z_unit_vector);
        }
    }
}

/* based on carpocalypse2 trig.c:567 */
// FUNCTION: CARMA2_HW 0x0040aed0
void C2_HOOK_FASTCALL DRMatrix34RotateCos(br_matrix34* pDest, const br_vector3* pAxis, float pCosA) {
    float sinA;
    float mc;
    float mcx;
    float mcxz;
    float mcxy;
    float sz;
    float sy;
    float sx;
    float mcyz;

    if (pCosA < -1.f) {
        pCosA = -1.f;
    } else if (pCosA > 1.f) {
        pCosA = 1.f;
    }
    sinA = (float)sqrt(1.f - pCosA * pCosA);
    mc = 1.f - pCosA;
    mcx = pAxis->v[0] * mc;
    mcxy = pAxis->v[1] * mcx;
    mcxz = pAxis->v[2] * mcx;
    mcyz = pAxis->v[2] * pAxis->v[1] * mc;
    sx = pAxis->v[0] * sinA;
    sy = pAxis->v[1] * sinA;
    sz = pAxis->v[2] * sinA;
    pDest->m[0][0] = pAxis->v[0] * pAxis->v[0] * mc + pCosA;
    pDest->m[0][1] = sz + mcxy;
    pDest->m[0][2] = mcxz - sy;
    pDest->m[1][0] = mcxy - sz;
    pDest->m[1][1] = pAxis->v[1] * pAxis->v[1] * mc + pCosA;
    pDest->m[1][2] = sx + mcyz;
    pDest->m[2][0] = sy + mcxz;
    pDest->m[2][1] = mcyz - sx;
    pDest->m[2][2] = pAxis->v[2] * pAxis->v[2] * mc + pCosA;
    pDest->m[3][0] = 0.f;
    pDest->m[3][1] = 0.f;
    pDest->m[3][2] = 0.f;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00655bb0
const char* gGroove_nature_names[2] = {
    "constant",
    "distance",
};

// GLOBAL: CARMA2_HW 0x00655bb8
const char* gAxis_names[3] = {
    "z",
    "y",
    "x",
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

// GLOBAL: CARMA2_HW 0x00655be0
const char* gGroove_object_names[4] = {
    "spin",
    "rock",
    "throb",
    "shear",
};

// GLOBAL: CARMA2_HW 0x0068b854
br_actor* gGroove_by_proxy_actor;
