#ifndef GUARD_28_WORLD3_H
#define GUARD_28_WORLD3_H

#include "brender/brender.h"
#include "rec2_types.h"

extern br_actor* gAdditional_actors;

extern tBrender_storage* gStorage_for_callbacks;

extern tRendererShadingType gMaterial_shading_for_callback;

extern void C2_HOOK_FASTCALL InitTreeSurgery(void);

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

extern intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg);
// RemoveBoundsStructures

// FreeTrack

extern void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);
extern void C2_HOOK_FASTCALL AnimateSky(void);
// CombineBounds

// CheckNonCarModelBounds

extern void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds);

extern void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd);

extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path);

extern br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName);

extern br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName);

extern br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName);

extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath);

extern int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath);

extern int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM);

extern void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath);

extern int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList);

extern int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList);

extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);

extern void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL DisallowDuplicates(void);

extern void C2_HOOK_FASTCALL AllowDuplicates(void);

extern br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage);


extern void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath);
extern intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data);
extern void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec);
extern void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);
extern void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);
extern void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);
extern void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);
extern void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec);
extern void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel);
extern void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea1, int* pArea2);
extern void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor);
extern intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* pData);
extern void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel);
extern void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF);
extern tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pName, br_matrix34* pMat);
extern void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pCharacter, br_vector3* pDir, br_vector3* pUp);
extern tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);
extern tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);
extern tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name);
extern void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
extern float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees);
extern void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
extern void C2_HOOK_FASTCALL DRMatrix34RotateCos(br_matrix34* pDest, const br_vector3* pAxis, float pCosA);
extern void C2_HOOK_FASTCALL GetGlassFragmentCounts(int* pVisible_count, int* pActive_count);

#endif // GUARD_28_WORLD3_H
