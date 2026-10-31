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

// DeleteAnyZeroBastards

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

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

// SaveAdditionalActors

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
// STUB: CARMA2_HW 0x00506f40
void C2_HOOK_FASTCALL SaveAdditionalActors(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006ab7b0
br_actor* gSpec_vol_actors[100];

// GLOBAL: CARMA2_HW 0x006ab940
int gSpec_vol_mode;

