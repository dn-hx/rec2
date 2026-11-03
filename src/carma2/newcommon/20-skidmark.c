#include "20-skidmark.h"

#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0065fe08
char* gBoring_material_names[2] = { "OILSMEAR.MAT", "ROBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x0065fe10
char* gMaterial_names[2] = { "OILSMEAR.MAT", "GIBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x006a27f0
tSkid gSkids[100] = { 0 };

/* UNCERTAIN: unknown, unreferenced 0x50 bytes after gSkids (only seen as its loop end); keeps the layout adjacent */
// GLOBAL: CARMA2_HW 0x006a32e0
tU8 gUNK_006a32e0[0x50] = { 0 };

// GLOBAL: CARMA2_HW 0x006a27e8
int gCurrent_skid;

// FUNCTION: CARMA2_HW 0x004ea2c0
void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step) {
    br_vector3 temp;
    br_vector3* rows;
    br_scalar len;
    br_scalar step_over_len;
    br_scalar unit;
    br_model* model;

    temp.v[0] = pTo->v[0] - pFrom->v[0];
    temp.v[1] = pTo->v[1] - pFrom->v[1];
    temp.v[2] = pTo->v[2] - pFrom->v[2];
    len = sqrt(temp.v[2] * temp.v[2] + temp.v[1] * temp.v[1] + temp.v[0] * temp.v[0]);
    if (len < 0.001f) {
        BrMatrix34Scale(&pMark->actor->t.t.mat, 0.001f, 1.0f, 0.001f);
        return;
    }
    step_over_len = pTexture_step / len;
    rows = (br_vector3*)&pMark->actor->t.t.mat.m;
    rows[1].v[0] = pMark->normal.v[0];
    rows[1].v[1] = pMark->normal.v[1];
    rows[1].v[2] = pMark->normal.v[2];
    rows[2].v[0] = pMark->normal.v[2] * temp.v[1] - pMark->normal.v[1] * temp.v[2];
    rows[2].v[1] = pMark->normal.v[0] * temp.v[2] - pMark->normal.v[2] * temp.v[0];
    rows[2].v[2] = pMark->normal.v[1] * temp.v[0] - pMark->normal.v[0] * temp.v[1];
    rows[2].v[0] = rows[2].v[0] * step_over_len;
    rows[2].v[1] = rows[2].v[1] * step_over_len;
    rows[2].v[2] = rows[2].v[2] * step_over_len;
    unit = len / len;
    rows[0].v[0] = temp.v[0] * unit;
    rows[0].v[1] = temp.v[1] * unit;
    rows[0].v[2] = temp.v[2] * unit;
    pMark->pos.v[0] = (pFrom->v[0] + pTo->v[0]) * 0.5f;
    pMark->pos.v[1] = (pFrom->v[1] + pTo->v[1]) * 0.5f;
    pMark->pos.v[2] = (pFrom->v[2] + pTo->v[2]) * 0.5f;
    rows[3].v[0] = pMark->pos.v[0];
    rows[3].v[1] = pMark->pos.v[1];
    rows[3].v[2] = pMark->pos.v[2];
    model = pMark->actor->model;
    model->vertices[0].map.v[0] = model->vertices[1].map.v[0] = pTexture_start / pTexture_step;
    model->vertices[2].map.v[0] = model->vertices[3].map.v[0] = (pTexture_start + len) / pTexture_step;
    BrModelUpdate(model, 4);
}

// MaterialFromIndex

// FUNCTION: CARMA2_HW 0x004e9be0
// FUNCTION: CARMA2_SW 0x004e59a0
void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    gSkids[pSkid_num].actor->t.t.mat = *pMatrix;
    gSkids[pSkid_num].pos = *(br_vector3*)pMatrix->m[3];
    gSkids[pSkid_num].actor->material = pMaterial;
    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}

// FarFromLine2D

// Reflex2D

// FUNCTION: CARMA2_HW 0x004e9c40
void C2_HOOK_FASTCALL InitSkids(void) {
    int skid;
    int mat;
    int sl;
    br_model* square;
    br_pixelmap* pm;
    char* str;

    for (mat = 0; mat < (int)REC2_ASIZE(gMaterial_names); mat++) {
        if (gProgram_state.sausage_eater_mode) {
            str = gBoring_material_names[mat];
        } else {
            str = gMaterial_names[mat];
        }
        gMaterial[mat] = BrMaterialFind(str);
        if (gMaterial[mat] == NULL) {
            if (gProgram_state.sausage_eater_mode) {
                str = gBoring_material_names[mat];
            } else {
                str = gMaterial_names[mat];
            }

            str = strtok(str, ".");
            sl = strlen(str);
            strcat(str, ".PIX");
            pm = LoadPixelmap(str);
            if (pm == NULL) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, str);
            }
            BrMapAdd(pm);
            str[sl] = '\0';
            strcat(str, ".MAT");
            gMaterial[mat] = LoadMaterial(str);
            if (gMaterial[mat] == NULL) {
                BrFatal("C:\\Carma2\\Source\\Common\\Skidmark.c", 215, "Couldn't find %s", gMaterial_names[mat]);
            } else {
                GlorifyMaterial(&gMaterial[mat], 1, kRendererShadingType_AmbientOnly);
            }
            BrMaterialAdd(gMaterial[mat]);
        }
    }

    C2_HOOK_BUG_ON(sizeof(tSkid) != 28);

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        gSkids[skid].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gOther_selfs[3], gSkids[skid].actor);
        gSkids[skid].actor->render_style = BR_RSTYLE_NONE;
        square = BrModelAllocate(NULL, 4, 2);
        BrVector3Set(&square->vertices[0].p, -0.5f, 0.0f, -0.5f);
        BrVector3Set(&square->vertices[1].p, -0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[2].p, 0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[3].p, 0.5f, 0.0f, -0.5f);
        BrVector2Set(&square->vertices[0].map, 0.0f, 0.0f);
        BrVector2Set(&square->vertices[1].map, 0.0f, 1.0f);
        BrVector2Set(&square->vertices[2].map, 1.0f, 1.0f);
        BrVector2Set(&square->vertices[3].map, 1.0f, 0.0f);
        square->faces[0].vertices[0] = 0;
        square->faces[0].vertices[1] = 1;
        square->faces[0].vertices[2] = 2;
        square->faces[0].smoothing = 1;
        square->faces[1].vertices[0] = 0;
        square->faces[1].vertices[1] = 2;
        square->faces[1].vertices[2] = 3;
        square->faces[1].smoothing = 1;
        square->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(square);
        gSkids[skid].actor->model = square;
    }
}

// FUNCTION: CARMA2_HW 0x004e9ee0
void C2_HOOK_FASTCALL HideSkid(int pSkid_num) {

    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e9f00
void C2_HOOK_FASTCALL HideSkids(void) {
    int skid;

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        HideSkid(skid);
    }
}

// SkidLen

// UglyWidthChange

// GenericSkidSection

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e9f20
// FUNCTION: CARMA2_SW 0x004e5cc0
int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step) {
    tU16 idx;
    br_scalar fVar1;
    br_scalar fVar2;

    extern void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

    if (((pNorm->v[0] * pPrev_norm->v[0] + pNorm->v[2] * pPrev_norm->v[2] + pNorm->v[1] * pPrev_norm->v[1]) < 0.997f) ||
        (0.01f < fabs((pNorm->v[0] * pPos->v[0] + pNorm->v[2] * pPos->v[2] + pNorm->v[1] * pPos->v[1]) -
                      (pNorm->v[0] * pPrev_pos->v[0] + pNorm->v[2] * pPrev_pos->v[2] + pNorm->v[1] * pPrev_pos->v[1])))) {
        *pSkid_id = -1;
        return 1;
    }
    if ((tU16)*pSkid_id < 100 && pMaterial == gSkids[(tU16)*pSkid_id].actor->material) {
        idx = (tU16)*pSkid_id;
        fVar1 = pTexture_step * pTexture_step;
        fVar2 = gSkids[idx].actor->t.t.mat.m[2][2] * gSkids[idx].actor->t.t.mat.m[2][2] +
                gSkids[idx].actor->t.t.mat.m[2][1] * gSkids[idx].actor->t.t.mat.m[2][1] +
                gSkids[idx].actor->t.t.mat.m[2][0] * gSkids[idx].actor->t.t.mat.m[2][0];
        if (fVar2 <= fVar1 + fVar1 && fVar1 <= fVar2 + fVar2 &&
            sqrt(gSkids[idx].actor->t.t.mat.m[0][2] * gSkids[idx].actor->t.t.mat.m[0][2] +
                 gSkids[idx].actor->t.t.mat.m[0][1] * gSkids[idx].actor->t.t.mat.m[0][1] +
                 gSkids[idx].actor->t.t.mat.m[0][0] * gSkids[idx].actor->t.t.mat.m[0][0]) <= 0.5f) {
            fVar1 = pSkid_end->v[0] - pSkid_start->v[0];
            fVar2 = pSkid_end->v[2] - pSkid_start->v[2];
            if (fabs(-fVar1 * (pPos->v[2] - pSkid_end->v[2]) + (pPos->v[0] - pSkid_end->v[0]) * fVar2) <=
                    sqrt(fVar2 * fVar2 + fVar1 * fVar1) * 0.05f &&
                0.0f <= (pPos->v[0] - pPrev_pos->v[0]) * (pPrev_pos->v[0] - pSkid_start->v[0]) +
                        (pPos->v[2] - pPrev_pos->v[2]) * (pPrev_pos->v[2] - pSkid_start->v[2])) {
                StretchMark(&gSkids[idx], pSkid_start, pPos, pTexture_start, pTexture_step);
                PipeSingleSkidAdjustment(idx, &gSkids[idx].actor->t.t.mat, pMaterial);
                return 0;
            }
        }
    }
    pSkid_start->v[0] = pPrev_pos->v[0];
    pSkid_start->v[1] = pPrev_pos->v[1];
    pSkid_start->v[2] = pPrev_pos->v[2];
    pSkid_end->v[0] = pPos->v[0];
    pSkid_end->v[1] = pPos->v[1];
    pSkid_end->v[2] = pPos->v[2];
    gSkids[gCurrent_skid].actor->render_style = BR_RSTYLE_DEFAULT;
    gSkids[gCurrent_skid].actor->material = pMaterial;
    gSkids[gCurrent_skid].normal.v[0] = pNorm->v[0];
    gSkids[gCurrent_skid].normal.v[1] = pNorm->v[1];
    gSkids[gCurrent_skid].normal.v[2] = pNorm->v[2];
    StretchMark(&gSkids[gCurrent_skid], pPrev_pos, pPos, pTexture_start, pTexture_step);
    PipeSingleSkidAdjustment(gCurrent_skid, &gSkids[gCurrent_skid].actor->t.t.mat, pMaterial);
    *pSkid_id = (tS16)gCurrent_skid;
    gCurrent_skid = (tU16)(gCurrent_skid + 1) % 100;
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ea490
// FUNCTION: CARMA2_SW 0x004e6230
void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num) {
    br_material* material;
    int skid_mask;
    br_vector3 disp;
    br_scalar dist;
    int on_ground;
    int old_skidding;

    on_ground = pCar_spec->oldd[pWheel_num] < pCar_spec->susp_height[pWheel_num >> 1];
    material = gCurrent_race.material_modifiers[pCar_spec->material_index[pWheel_num]].skid_mark_material;
    if (pCar_spec->oil_remaining[pWheel_num] != 0.0f && on_ground) {
        pCar_spec->new_skidding |= 1 << pWheel_num;
        if (pCar_spec->field_0x16fc[pWheel_num] != NULL) {
            material = pCar_spec->field_0x16fc[pWheel_num];
        } else {
            material = gMaterial[0];
        }
    }
    if (material == NULL) {
        pCar_spec->old_skidding &= ~(1 << pWheel_num);
        return;
    }
    skid_mask = 1 << pWheel_num;
    if (!(pCar_spec->new_skidding & skid_mask) && !(pCar_spec->old_skidding & skid_mask)) {
        return;
    }
    if (pCar_spec->new_skidding & skid_mask) {
        old_skidding = pCar_spec->old_skidding;
        if (old_skidding & skid_mask) {
            BrVector3Sub(&disp, &pCar_spec->special_start[pWheel_num], &pCar_spec->prev_skid_pos[pWheel_num]);
            dist = BrVector3Length(&disp);
            if (dist < 0.05f) {
                return;
            }
            if (SkidSection(&pCar_spec->old_skid[pWheel_num],
                    &pCar_spec->field_0x160c[pWheel_num],
                    &pCar_spec->field_0x163c[pWheel_num],
                    material,
                    &pCar_spec->special_start[pWheel_num],
                    &pCar_spec->nor[pWheel_num],
                    &pCar_spec->prev_skid_pos[pWheel_num],
                    &pCar_spec->prev_nor[pWheel_num],
                    pCar_spec->total_length[pWheel_num],
                    0.05f)) {
                pCar_spec->old_skidding &= ~skid_mask;
            }
            pCar_spec->total_length[pWheel_num] += dist;
            pCar_spec->oil_remaining[pWheel_num] -= dist;
            if (pCar_spec->oil_remaining[pWheel_num] < 0.0f) {
                pCar_spec->oil_remaining[pWheel_num] = 0.0f;
            }
        } else {
            pCar_spec->old_skidding = old_skidding | skid_mask;
            pCar_spec->total_length[pWheel_num] = 0.0f;
            pCar_spec->old_skid[pWheel_num] = -1;
        }
    } else {
        pCar_spec->old_skidding &= ~skid_mask;
    }
    pCar_spec->prev_skid_pos[pWheel_num] = pCar_spec->special_start[pWheel_num];
    pCar_spec->prev_nor[pWheel_num] = pCar_spec->nor[pWheel_num];
}

// FUNCTION: CARMA2_HW 0x004ea700
void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar) {
    int wheel;

    pCar->old_skidding = 0;
    for (wheel = 0; wheel < (int)REC2_ASIZE(pCar->oil_remaining); wheel++) {
        pCar->oil_remaining[wheel] = 0.0f;
    }
}

// FUNCTION: CARMA2_HW 0x004ea720
void C2_HOOK_FASTCALL SkidsPerFrame(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSkids); i++) {
        tSkid *skid = &gSkids[i];

        if (skid->actor->render_style != BR_RSTYLE_NONE) {
            skid->actor->t.t.translate.t = skid->pos;
        }
    }
}

