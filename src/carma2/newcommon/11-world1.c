#include "11-world1.h"

#include "28-world3.h"

#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x006ab7a8
// GLOBAL: CARMA2_SW 0x006a51f8
int gDelete_count;

// GLOBAL: CARMA2_HW 0x006aafc8
// GLOBAL: CARMA2_SW 0x006a4a18
br_actor* gDelete_list[500];

// FUNCTION: CARMA2_HW 0x00507210
// FUNCTION: CARMA2_SW 0x00502660
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMatrix, void* pArg) {
    int i;
    int parent_already_doomed;

    if ((pActor != gAdditional_actors
            && (pActor->identifier == NULL || pActor->identifier[0] == '&')
            && Vector3IsZero((br_vector3*)pMatrix->m[3]))
        || (pActor->model == NULL && pActor->type == BR_ACTOR_MODEL)) {
        parent_already_doomed = 0;
        for (i = 0; i < gDelete_count; i++) {
            if (gDelete_list[i] == pActor) {
                parent_already_doomed = 1;
                break;
            }
        }
        if (!parent_already_doomed) {
            gDelete_list[gDelete_count] = pActor;
            gDelete_count++;
        }
    }
    return 0;
}

// inlined into SaveAdditionalActors (no standalone address in the original)
static __inline void C2_HOOK_FASTCALL DeleteAnyZeroBastardsInline(void) {
    extern int gDelete_count;
    extern br_actor* gDelete_list[500];
    extern br_actor* gAdditional_actors;
    extern br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg);
    br_actor** pp;
    int count;
    int i;

    gDelete_count = 0;
    DRActorEnumRecurseWithTrans(gAdditional_actors, NULL, DeleteBastards, NULL);
    i = gDelete_count - 1;
    if (i >= 0) {
        count = i + 1;
        pp = &gDelete_list[i];
        do {
            BrActorRemove(*pp);
            pp--;
            count--;
        } while (count != 0);
    }
}

// FUNCTION: CARMA2_HW 0x005072b0
br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMatrix, void* pArg) {
    int i;
    br_vector3 temp_point;

    if (pActor->identifier != NULL && pActor->identifier[0] != '&') {
        if (pActor->model != NULL) {
            for (i = 0; i < pActor->model->nvertices; i++) {
                temp_point = pActor->model->vertices[i].p;
                BrMatrix34ApplyP(&pActor->model->vertices[i].p, &temp_point, pMatrix);
            }
            BrModelUpdate(pActor->model, BR_MODU_ALL);
        }
        BrMatrix34Identity(&pActor->t.t.mat);
    }
    return 0;
}

// FindSpecVolIndex

// FUNCTION: CARMA2_HW 0x005074d0
void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1) {
    pMat_1->map_transform.m[0][0] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_0]);
    pMat_1->map_transform.m[1][1] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_1]);
    BrMatrix23Copy(&pMat_2->map_transform, &pMat_1->map_transform);
}

// SetSpecVolMatSize

// UpdateSpecVol

// inlined into SaveAdditionalActors (no standalone address in the original)
static __inline void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToAdditionalActors(void) {
    extern tNon_car_spec* gActive_non_car_list[99];
    extern int gNum_active_non_cars;
    extern br_actor* gNon_track_actor;
    extern br_actor* gAdditional_actors;
    int i;

    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec* non_car;

        non_car = gActive_non_car_list[i];
        if ((tU16)non_car->car_ID == 0xffff && non_car->actor->parent == gNon_track_actor) {
            BrActorRemove(non_car->actor);
            BrActorAdd(gAdditional_actors, non_car->actor);
        }
    }
}

// inlined into SaveAdditionalActors (no standalone address in the original)
static __inline void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToNonTrackActor(void) {
    extern tNon_car_spec* gActive_non_car_list[99];
    extern int gNum_active_non_cars;
    extern br_actor* gNon_track_actor;
    extern br_actor* gAdditional_actors;
    int i;

    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec* non_car;

        non_car = gActive_non_car_list[i];
        if ((tU16)non_car->car_ID == 0xffff && non_car->actor->parent == gAdditional_actors) {
            BrActorRemove(non_car->actor);
            BrActorAdd(gNon_track_actor, non_car->actor);
        }
    }
}

// DoSaveAdditionalStuff

extern tProgram_state gProgram_state;
extern br_actor* gSpec_vol_actors[100];

static int FindSpecVolIndexInline(br_actor* pActor) {
    int i;

    if (pActor == NULL) {
        return -1;
    }
    for (i = 0; i < gProgram_state.special_volume_count; i++) {
        if (gSpec_vol_actors[i] == pActor) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x00507360
void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void) {
    extern char gAdditional_actor_path[256];
    extern char gAdditional_model_path[256];
    extern br_actor* gLast_actor;
    extern int gSpec_vol_mode;
    extern void C2_HOOK_FASTCALL FindInverseAndWorldBox(tSpecial_volume* pSpec);
    extern void C2_HOOK_FASTCALL SaveSpecialVolumes(void);
    extern void C2_HOOK_FASTCALL SaveAdditionalActors(void);
    extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);
    int len1;
    int len2;

    len1 = strlen(gAdditional_actor_path);
    len2 = strlen(gAdditional_actor_path);
    strcat(gAdditional_actor_path, " autosave");
    strcat(gAdditional_model_path, " autosave");
    if (gSpec_vol_mode != 0) {
        int index;

        index = FindSpecVolIndexInline(gLast_actor);
        if (index >= 0) {
            tSpecial_volume* v;
            br_model* model;
            br_matrix34* mat;

            v = &gProgram_state.special_volumes[index];
            BrMatrix34Copy(&v->boundary.box.mat, &gLast_actor->t.t.mat);
            FindInverseAndWorldBox(v);
            model = gLast_actor->model;
            mat = &gLast_actor->t.t.mat;
            MungeMaterialSV(mat, model->faces[5].material, model->faces[17].material, 0, 1);
            MungeMaterialSV(mat, model->faces[11].material, model->faces[23].material, 1, 2);
            MungeMaterialSV(mat, model->faces[7].material, model->faces[19].material, 0, 2);
        }
        SaveSpecialVolumes();
    } else {
        SaveAdditionalActors();
    }
    gAdditional_actor_path[len1] = 0;
    gAdditional_model_path[len2] = 0;
    FlushAllPedCaches();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL: SaveAdditionalActors 0x00506f40 - all instructions match except the delete-loop preheader, where the
// pointer lea (esi) and the trip-count lea (edi) are emitted in the opposite order (same bytes, same addresses).
// FUNCTION: CARMA2_HW 0x00506f40
void C2_HOOK_FASTCALL SaveAdditionalActors(void) {
    extern tNon_car_spec* gActive_non_car_list[99];
    extern int gNum_active_non_cars;
    extern br_actor* gNon_track_actor;
    extern char gAdditional_actor_path[256];
    extern char gAdditional_model_path[256];
    extern br_model* gAdditional_models[1000];
    extern int gNumber_of_additional_models;
    extern br_actor* gLast_actor;
    extern br_actor* gAdditional_actors;
    extern br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg);
    extern void* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
    extern int C2_HOOK_FASTCALL PFfclose(void* pFile);
    extern int sprintf(char* buffer, const char* format, ...);
    char local_200[256];
    char local_100[256];
    int file_index;
    int done;

    file_index = 0;
    done = 0;
    PutAdditionalNonCarsOnToAdditionalActors();
    DeleteAnyZeroBastardsInline();
    if (gLast_actor != NULL) {
        DRActorEnumRecurseWithTrans(gLast_actor, NULL, ApplyTransToModels, NULL);
    }
    if (strstr(gAdditional_actor_path, "autosave") != NULL) {
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, (br_uint_16)gNumber_of_additional_models);
    } else {
        char* str;
        void* f;

        sprintf(local_200, "%s", gAdditional_actor_path);
        str = strstr(local_200, ".");
        if (str != NULL) {
            *str = '\0';
        }
        do {
            sprintf(local_100, "%s%04d.ACT", local_200, file_index);
            f = DRfopen(local_100, "rb");
            if (f != NULL) {
                PFfclose(f);
                file_index++;
            } else {
                done = 1;
            }
        } while (!done);
        strcpy(local_100, local_200);
        sprintf(local_200, "%s%04d.ACT", local_100, file_index);
        BrActorSave(local_200, gAdditional_actors);
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        sprintf(local_100, "%s", gAdditional_model_path);
        str = strstr(local_100, ".");
        if (str != NULL) {
            *str = '\0';
        }
        sprintf(local_200, "%s%04d.DAT", local_100, file_index);
        BrModelSaveMany(local_200, gAdditional_models, (br_uint_16)gNumber_of_additional_models);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, (br_uint_16)gNumber_of_additional_models);
    }
    PutAdditionalNonCarsOnToNonTrackActor();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006ab7b0
br_actor* gSpec_vol_actors[100];

// GLOBAL: CARMA2_HW 0x006ab940
int gSpec_vol_mode;


// FUNCTION: CARMA2_HW 0x00507580
void C2_HOOK_FASTCALL LoadAdditionalStuff(void) {
    extern int gNum_active_non_cars;
    extern int gNumber_of_additional_models;
    extern br_model* gAdditional_models[1000];
    extern char gAdditional_model_path[256];
    extern char gAdditional_actor_path[256];
    extern br_actor* gLast_actor;
    extern br_actor* gUniverse_actor;
    extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
    int i;

    if (gNum_active_non_cars != 0) {
        NewTextHeadupSlot(4, 0, 2000, -2, "Can't load with active non cars");
        return;
    }
    NewTextHeadupSlot(4, 0, 2000, -2, "Loading Additional Stuff");
    for (i = 0; i < gNumber_of_additional_models; i++) {
        BrModelRemove(gAdditional_models[i]);
        BrModelFree(gAdditional_models[i]);
    }
    BrActorRemove(gAdditional_actors);
    BrActorFree(gAdditional_actors);
    gLast_actor = NULL;
    gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, 1000);
    for (i = 0; i < gNumber_of_additional_models; i++) {
        gAdditional_models[i]->flags = 0x80;
    }
    BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
    gAdditional_actors = BrActorLoad(gAdditional_actor_path);
    if (gAdditional_actors == NULL) {
        gAdditional_actors = BrActorAllocate(0, NULL);
    }
    BrActorAdd(gUniverse_actor, gAdditional_actors);
}

// FUNCTION: CARMA2_HW 0x00508e50
intptr_t C2_HOOK_CDECL MoveModelVertices(br_actor* pActor, br_vector3* pOffset) {
    int i;

    if (pActor->model != NULL) {
        i = 0;
        if (pActor->model->nvertices > 0) {
            do {
                pActor->model->vertices[i].p.v[0] += pOffset->v[0];
                pActor->model->vertices[i].p.v[1] += pOffset->v[1];
                pActor->model->vertices[i].p.v[2] += pOffset->v[2];
                i++;
            } while (i < pActor->model->nvertices);
        }
    }
    return 0;
}
