#include "32-spark.h"

#include "08-loading1.h"
#include "18-graphics2.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00660110
int gSmoke_on = 1;

// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8];

// GLOBAL: CARMA2_HW 0x006a82b4
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa57c
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006aa590
int gSmoke_flags;

/* explicit initializers keep gBR_smoke_structs, the 4-byte gap and gSmoke adjacent (InitSmokeStuff loop-end compares) */
// GLOBAL: CARMA2_HW 0x006a87f0
tBRender_smoke gBR_smoke_structs[35] = { 0 };

/* UNCERTAIN: unknown, unreferenced 4 bytes between gBR_smoke_structs and gSmoke */
// GLOBAL: CARMA2_HW 0x006a8cdc
int gUNK_006a8cdc = 0;

// GLOBAL: CARMA2_HW 0x006a8ce0
tSmoke gSmoke[25] = { 0 };

// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x006a82b8
tSplash gSplash[32] = { 0 };

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20] = { 0 };

/* UNCERTAIN: unknown, unreferenced 0x90 bytes after gFlame_map (only seen as its loop end); keeps the layout adjacent */
// GLOBAL: CARMA2_HW 0x006a8688
tU8 gUNK_006a8688[0x90] = { 0 };

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006a9180
tShrapnel gShrapnel[30] = { 0 };

// GLOBAL: CARMA2_HW 0x006a96a8
tSmoke_column gSmoke_column[10] = { 0 };

/* explicit initializers keep gSmoke_column and gSparks adjacent as in the original (loop-end compares) */
// GLOBAL: CARMA2_HW 0x006a9b80
tSpark gSparks[32] = { 0 };

// GLOBAL: CARMA2_HW 0x006aa380
br_model* gLollipop_model;

// GLOBAL: CARMA2_HW 0x006aa59c
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006b7880
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x006a8718
br_matrix4 gCameraToScreen;

// GLOBAL: CARMA2_HW 0x006a82b0
br_camera* gSpark_cam;
// GLOBAL: CARMA2_HW 0x006aa574
int gReplay_splash_flags;

// GLOBAL: CARMA2_HW 0x006aa388
tReplay_splash gReplay_splashes[10];

// GLOBAL: CARMA2_HW 0x00660148
br_token_value* gDAT_00660148;

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// FUNCTION: CARMA2_HW 0x004f6b80
int C2_HOOK_FASTCALL DrawLine3D(const br_vector3* pStart, const br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_pixelmap* pShade) {
    br_vector3 start;
    br_vector3 end;
    br_vector4 projected_start;
    br_vector4 projected_end;
    br_scalar fraction;

    if (gNo_2d_effects) {
        BrVector3Copy(&gLine_model->vertices[0].p, pStart);
        BrVector3Copy(&gLine_model->vertices[1].p, pEnd);
        BrModelUpdate(gLine_model, BR_MODU_VERTEX_POSITIONS);
        BrZbsSceneRenderAdd(gLine_actor);
        return 999;
    }
    BrVector3Copy(&start, pStart);
    BrVector3Copy(&end, pEnd);
    if (start.v[2] > -gSpark_cam->hither_z || end.v[2] > -gSpark_cam->hither_z) {
        if (start.v[2] > -gSpark_cam->hither_z && end.v[2] > -gSpark_cam->hither_z) {
            return 0;
        }
        fraction = (end.v[2] + gSpark_cam->hither_z) / (end.v[2] - start.v[2]);
        if (start.v[2] > -gSpark_cam->hither_z) {
            BrVector3Set(&start,
                end.v[0] - (end.v[0] - start.v[0]) * fraction,
                end.v[1] - (end.v[1] - start.v[1]) * fraction,
                -gSpark_cam->hither_z);
        }
        if (end.v[2] > -gSpark_cam->hither_z) {
            BrVector3Set(&end,
                end.v[0] - (end.v[0] - start.v[0]) * fraction,
                end.v[1] - (end.v[1] - start.v[1]) * fraction,
                -gSpark_cam->hither_z);
        }
    }
    BrMatrix4ApplyP(&projected_start, &start, &gCameraToScreen);
    BrMatrix4ApplyP(&projected_end, &end, &gCameraToScreen);
    BrVector3Set(&start,
        projected_start.v[0] / projected_start.v[3],
        projected_start.v[1] / projected_start.v[3],
        projected_start.v[2] / projected_start.v[3]);
    BrVector3Set(&end,
        projected_end.v[0] / projected_end.v[3],
        projected_end.v[1] / projected_end.v[3],
        projected_end.v[2] / projected_end.v[3]);
    return DrawLine2D(&start, &end, pScreen, pDepth, 1.f, pShade);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f6de0
// STUB: CARMA2_SW 0x004f2690
int C2_HOOK_FASTCALL DrawLine2D(br_vector3* pStart, br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_scalar pBrightness, br_pixelmap* pShade) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f7cb0
void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol) {

    if (pCol != 0) {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0xff;
        gLine_model->vertices[0].blu = 0xff;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0xff;
    } else {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0x00;
        gLine_model->vertices[0].blu = 0x00;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0x00;
    }
    BrModelUpdate(gLine_model, BR_MODU_ALL);
}

// ReplaySparks

// RenderSparks

// CreateSingleSpark

// CreateSparks

// CreateSparkShower

// AdjustSpark

// AdjustShrapnel

// FUNCTION: CARMA2_HW 0x004f87a0
void C2_HOOK_FASTCALL ResetSparks(void) {
    gSpark_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004f87b0
void C2_HOOK_FASTCALL ResetShrapnel(void) {
    int i;

    if (gShrapnel_flags == 0) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        if (((1 << i) & gShrapnel_flags) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
    }
    gShrapnel_flags = 0;
}

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// LoadInShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// FUNCTION: CARMA2_HW 0x004f8dc0
intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext) {
    tFunk_index_cbfn* funk_index_callback;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    int i;
    int funk_index;

    user_crush_data = pActor->user;
    funk_index_callback = pContext;

    if (user_crush_data != NULL) {
        crush_data = user_crush_data->crush_data;
        if (crush_data != NULL) {
            if (crush_data->smashables != NULL) {
                for (i = 0; i < crush_data->count_smashables; i++) {
                    funk_index = crush_data->smashables[i].funk;
                    if (funk_index >= 0) {
                        funk_index_callback(funk_index);
                    }
                }
            }
        }
    }
    return 0;
}

// STUB: CARMA2_HW 0x004f8ca0
void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f8e10
void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// SetPixelmap

// STUB: CARMA2_HW 0x004f8f30
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// FUNCTION: CARMA2_HW 0x004f9390
int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel) {
    pModel->flags &= ~BR_MODF_CUSTOM;
    pModel->custom = NULL;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004f93b0
int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial) {

    if (pMaterial->flags & BR_MATF_PRELIT) {
        pMaterial->flags &= ~BR_MATF_PRELIT;
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
        return 1;
    } else {
        return 0;
    }
}

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// FUNCTION: CARMA2_HW 0x004f9760
// FUNCTION: CARMA2_SW 0x004f4de0
int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial) {

    if (pMaterial->colour_map != pMaterial->user) {
        pMaterial->colour_map = pMaterial->user;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    } else {
        return 0;
    }
}

// STUB: CARMA2_HW 0x004f9620
void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// MungeShrapnel

// DrMatrix34Rotate

// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    tBRender_smoke* const* a;
    tBRender_smoke* const* b;

    a = (tBRender_smoke* const*)p1;
    b = (tBRender_smoke* const*)p2;
    if ((*a)->pos.v[2] < (*b)->pos.v[2]) {
        return -1;
    } else if ((*a)->pos.v[2] > (*b)->pos.v[2]) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
#ifndef REC2_MATCHING
    int i;

    gBlend_model = BrModelAllocate("BlendModel", 4, 2);
    gBlend_model->flags |= BR_MODF_UPDATEABLE;
    BrVector3SetFloat(&gBlend_model->vertices[0].p, -0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[1].p, 0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[2].p, 0.5f, 1.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[3].p, -0.5f, 1.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gBlend_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gBlend_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[3].map, 0.f, 0.f);
    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 0;
    gBlend_model->faces[1].vertices[1] = 2;
    gBlend_model->faces[1].vertices[2] = 3;
    BrModelAdd(gBlend_model);

    gBlend_model2 = BrModelAllocate("BlendModel2", 4, 2);
    gBlend_model2->flags |= BR_MODF_UPDATEABLE;
    BrVector3SetFloat(&gBlend_model2->vertices[0].p, -0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[1].p, 0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[2].p, 0.5f, 1.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[3].p, -0.5f, 1.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gBlend_model2->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gBlend_model2->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[3].map, 0.f, 0.f);
    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 0;
    gBlend_model2->faces[1].vertices[1] = 2;
    gBlend_model2->faces[1].vertices[2] = 3;
    BrModelAdd(gBlend_model2);

    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gBlend_actor->render_style = BR_RSTYLE_FACES;
    gBlend_actor->model = gBlend_model;

    for (i = 0; i < (int)REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate(NULL);
        gBR_smoke_structs[i].material->flags &= ~BR_MATF_LIGHT;
        gBR_smoke_structs[i].material->flags |= BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }
#else
    br_pixelmap* pm;
    tPath_name path;
    int i;

    gBlend_model = BrModelAllocate("gBlend_model", 4, 2);
    gBlend_model2 = BrModelAllocate("gBlend_model2", 6, 4);
    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    for (i = 0; i < 35; i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate("some smoke");
        if (gBR_smoke_structs[i].material == NULL) {
            break;
        }
    }

    if (gBlend_model == NULL || i != 35 || gBlend_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }

    gBlend_actor->identifier = "gBlend_actor";
    gBlend_actor->model = gBlend_model;

    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 2;
    gBlend_model->faces[1].vertices[1] = 3;
    gBlend_model->faces[1].vertices[2] = 0;

    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 2;
    gBlend_model2->faces[1].vertices[1] = 3;
    gBlend_model2->faces[1].vertices[2] = 0;
    gBlend_model2->faces[2].vertices[0] = 1;
    gBlend_model2->faces[2].vertices[1] = 5;
    gBlend_model2->faces[2].vertices[2] = 2;
    gBlend_model2->faces[3].vertices[0] = 1;
    gBlend_model2->faces[3].vertices[1] = 4;
    gBlend_model2->faces[3].vertices[2] = 5;

    gBlend_model->vertices[0].p.v[0] = -1.0f;
    gBlend_model->vertices[0].p.v[1] = 1.0f;
    gBlend_model->vertices[0].p.v[2] = 0.0f;
    gBlend_model->vertices[1].p.v[0] = -1.0f;
    gBlend_model->vertices[1].p.v[1] = -1.0f;
    gBlend_model->vertices[1].p.v[2] = 0.0f;
    gBlend_model->vertices[2].p.v[0] = 1.0f;
    gBlend_model->vertices[2].p.v[1] = -1.0f;
    gBlend_model->vertices[2].p.v[2] = 0.0f;
    gBlend_model->vertices[3].p.v[0] = 1.0f;
    gBlend_model->vertices[3].p.v[1] = 1.0f;
    gBlend_model->vertices[3].p.v[2] = 0.0f;

    gBlend_model2->vertices[0].p.v[0] = -1.0f;
    gBlend_model2->vertices[0].p.v[1] = 1.0f;
    gBlend_model2->vertices[0].p.v[2] = 0.0f;
    gBlend_model2->vertices[1].p.v[0] = -1.0f;
    gBlend_model2->vertices[1].p.v[1] = 0.0f;
    gBlend_model2->vertices[1].p.v[2] = 0.0f;
    gBlend_model2->vertices[2].p.v[0] = 1.0f;
    gBlend_model2->vertices[2].p.v[1] = 0.0f;
    gBlend_model2->vertices[2].p.v[2] = 0.0f;
    gBlend_model2->vertices[3].p.v[0] = 1.0f;
    gBlend_model2->vertices[3].p.v[1] = 1.0f;
    gBlend_model2->vertices[3].p.v[2] = 0.0f;
    gBlend_model2->vertices[4].p.v[0] = -1.0f;
    gBlend_model2->vertices[4].p.v[1] = -0.25f;
    gBlend_model2->vertices[4].p.v[2] = 0.0f;
    gBlend_model2->vertices[5].p.v[0] = 1.0f;
    gBlend_model2->vertices[5].p.v[1] = -0.25f;
    gBlend_model2->vertices[5].p.v[2] = 0.0f;

    gBlend_model->flags |= BR_MODF_KEEP_ORIGINAL;
    gBlend_model2->flags |= BR_MODF_KEEP_ORIGINAL;

    memcpy(path, "SMOKE.PIX", sizeof("SMOKE.PIX"));
    pm = LoadPixelmap(path);
    if (pm == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }

    pm->map = gRender_palette;
    BrMapAdd(pm);

    for (i = 0; i < 35; i++) {
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = (br_token_value*)&gDAT_00660148;
        gBR_smoke_structs[i].material->colour_map = pm;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }

    gBlend_model->vertices[0].map.v[0] = 0.0f;
    gBlend_model->vertices[0].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model->vertices[1].map.v[0] = 0.0f;
    gBlend_model->vertices[1].map.v[1] = 0.0f;
    gBlend_model->vertices[2].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model->vertices[2].map.v[1] = 0.0f;
    gBlend_model->vertices[3].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model->vertices[3].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;

    gBlend_model2->vertices[0].map.v[0] = 0.0f;
    gBlend_model2->vertices[0].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model2->vertices[1].map.v[0] = 0.0f;
    gBlend_model2->vertices[1].map.v[1] = 0.5f;
    gBlend_model2->vertices[2].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[2].map.v[1] = 0.5f;
    gBlend_model2->vertices[3].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[3].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model2->vertices[4].map.v[0] = 0.0f;
    gBlend_model2->vertices[4].map.v[1] = 0.0f;
    gBlend_model2->vertices[5].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[5].map.v[1] = 0.0f;

    BrModelAdd(gBlend_model);
    BrModelAdd(gBlend_model2);
    BrActorAdd(gDont_render_actor, gBlend_actor);
#endif
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    br_model* model;
    tU8 red;
    tU8 grn;
    tU8 blu;

    C2_HOOK_BUG_ON(REC2_ASIZE(gBR_smoke_pointers) != 35);
    C2_HOOK_BUG_ON(sizeof(gBR_smoke_pointers[0]) != 4);

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(gBR_smoke_pointers[0]), CmpSmokeZ);
    for (i = gN_BR_smoke_structs - 1; i >= 0; i--) {
        smoke = gBR_smoke_pointers[i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        gBlend_actor->t.t.mat.m[0][0] = smoke->r;
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        smoke->material->extra_prim[1].v.x = BrFloatToFixed(smoke->strength * 150.f);
        BrMaterialUpdate(smoke->material, BR_MATU_EXTRA_PRIM);
        red = BR_RED(smoke->col);
        grn = BR_GRN(smoke->col);
        blu = BR_BLU(smoke->col);
        model = gBR_smoke_pointers[i]->model;
        for (j = 0; j < model->nvertices; j++) {
            model->vertices[j].red = red;
            model->vertices[j].grn = grn;
            model->vertices[j].blu = blu;
        }
        BrModelUpdate(model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

// CreatePuffOfSmoke

// FUNCTION: CARMA2_HW 0x004fb510
void C2_HOOK_FASTCALL ResetSmoke(void) {
    gSmoke_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004fb520
void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    gSmoke[pIndex].type = pType;
    gSmoke[pIndex].radius = pRadius;
    gSmoke[pIndex].strength = pStrength;
    BrVector3Copy(&gSmoke[pIndex].pos, pPos);
    gSmoke_flags |= 1 << pIndex;
}

// ActorError

// FUNCTION: CARMA2_HW 0x004fb580
// FUNCTION: CARMA2_SW 0x004f6ac0
void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    tU32 bit;

    bit = 1 << pIndex;
    gColumn_flags ^= bit;
    gSmoke_column[pIndex].core = *pCore;
    gSmoke_column[pIndex].vertex_index = pVertex;
    gSmoke_column[pIndex].colour = pColour;
    gSmoke_column[pIndex].frame_count[0] = 100;
    gSmoke_column[pIndex].frame_count[1] = 100;
    gSmoke_column[pIndex].frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[pIndex].flame_actor);
        } else if (gSmoke_column[pIndex].flame_actor->depth != 0) {
            BrActorRemove(gSmoke_column[pIndex].flame_actor);
        }
    }
}

// CreateSmokeColumn2

// CreateSmokeColumn

// FUNCTION: CARMA2_HW 0x004fb910
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    // GLOBAL: CARMA2_HW 0x006aa5b4
    static int rb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5b8
    static int gb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5bc
    static int bb = 0;
    // GLOBAL: CARMA2_HW 0x00660160
    static int rl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660164
    static int gl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660168
    static int bl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660178
    static int rs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066017c
    static int gs = 0xc8;
    // GLOBAL: CARMA2_HW 0x00660180
    static int bs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066016c
    static int rm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660170
    static int gm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660174
    static int bm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660184
    static int rd = 0xfe;
    // GLOBAL: CARMA2_HW 0x00660188
    static int gd = 0xdf;
    // GLOBAL: CARMA2_HW 0x0066018c
    static int bd = 0x43;

    gShade_list[0] = BR_COLOUR_RGB(rb, gb, bb);
    gShade_list[1] = BR_COLOUR_RGB(rl, gl, bl);
    gShade_list[2] = BR_COLOUR_RGB(rm, gm, bm);
    gShade_list[3] = BR_COLOUR_RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[7] = BR_COLOUR_RGB(rd, gd, bd);
}

// FUNCTION: CARMA2_HW 0x004fb9c0
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {
    if (gIt_shade_table == NULL) {
        gIt_shade_table = GenerateDarkenedShadeTable(16, gRender_palette, 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}

// FUNCTION: CARMA2_HW 0x004fba00
// FUNCTION: CARMA2_SW 0x004f6f60
void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame_count, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    int i;
    tSmoke_column* col;

    i = pIndex & 0xf;
    col = &gSmoke_column[(pIndex & ~0xf) >> 4];
    col->frame_count[i] = pFrame_count;
    col->scale_x[i] = pScale_x;
    col->scale_y[i] = pScale_y;
    col->offset_x[i] = pOffset_x;
    col->offset_z[i] = pOffset_z;
}

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// FUNCTION: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlame(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < (int)REC2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fc3a0
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    int num;
    char the_path[256];
    br_actor* actor;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(the_path, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    num = DRLoadMultiplePix(the_path, gFlame_map, REC2_ASIZE(gFlame_map));
    if (num != (int)REC2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
    }
    BrMapAddMany(gFlame_map, num);
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < (int)REC2_ASIZE(gSmoke_column[0].frame_count); j++) {
            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, actor);
            actor->model = gLollipop_model;
            actor->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = gFlame_map[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            gSmoke_column[i].frame_count[j] = 100;
        }
    }
    gLollipop_model->nvertices = 4;
    BrVector3SetFloat(&gLollipop_model->vertices[0].p, -.5f, 0.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[1].p, .5f, 0.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[2].p, .5f, 1.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[3].p, -.5f, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[3].map, 0.f, 0.f);
    gLollipop_model->nfaces = 2;
    gLollipop_model->faces[0].vertices[0] = 0;
    gLollipop_model->faces[0].vertices[1] = 1;
    gLollipop_model->faces[0].vertices[2] = 2;
    gLollipop_model->faces[1].vertices[0] = 0;
    gLollipop_model->faces[1].vertices[1] = 2;
    gLollipop_model->faces[1].vertices[2] = 3;
    gLollipop_model->faces[0].smoothing = 1;
    gLollipop_model->faces[1].smoothing = 1;
    BrModelAdd(gLollipop_model);
}

/* based on carpocalypse2 src/carma2/common/spark.c:408 */
// GLOBAL: CARMA2_HW 0x006aa570
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a8758
br_model* gSplash_model;

// GLOBAL: CARMA2_HW 0x006a9130
br_material* gSplash_material[20];

// GLOBAL: CARMA2_HW 0x006aa5a4
int gNum_splash_types;


// DisposeSplash

// DrawTheGlow

// PipeInstantUnSmudge

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point) {
    (void)pCar;
    (void)pFire_point;
}

// FUNCTION: CARMA2_HW 0x004fc9d0
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca30
int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return gSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca40
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca70
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar_spec) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {

        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// ConditionalSmokeColumn

// SingleSplash

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004fcbb0
void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004fda40
void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime) {
    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal,
        pObject->water_d, &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}

// GetVelocitiesFromMatrices

// MungeSplash

// FUNCTION: CARMA2_HW 0x004fdb70
void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* pF) {
    int i;
    int red;
    int green;
    int blue;
    br_scalar quarter;
    br_scalar half;
    br_scalar three_quarter;

    gNum_dust_tables = GetAnInt(pF);
    if (gNum_dust_tables > 8) {
        gNum_dust_tables = 8;
    }
    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        GetThreeInts(pF, &red, &green, &blue);
        GetThreeScalars(pF, &quarter, &half, &three_quarter);
        gShade_table[i] = BR_COLOUR_RGB(red, green, blue);
    }
}

// FUNCTION: CARMA2_HW 0x004fdc00
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {
}

// LoadInKevStuff

// DisposeKevStuff

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004fe640
// STUB: CARMA2_SW 0x004f9f20
void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// STUB: CARMA2_HW 0x004fed00
// STUB: CARMA2_SW 0x004fa5e0
int C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fecf0
// FUNCTION: CARMA2_SW 0x004fa5d0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    ForEveryCarMaterial(pCar, BlendifyMaterialCB, 1);
}

// STUB: CARMA2_HW 0x004fed50
// STUB: CARMA2_SW 0x004fa630
int C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fed40
// FUNCTION: CARMA2_SW 0x004fa620
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {
    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed90
int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fedd0
void C2_HOOK_FASTCALL ClearSplashReplay(uintptr_t pObject) {
    int flags;
    int i;

    flags = gReplay_splash_flags;
    for (i = 0; i < (int)REC2_ASIZE(gReplay_splashes); i++) {
        uintptr_t object;

        memcpy(&object, &gReplay_splashes[i].object, sizeof(object));
        if (pObject == object) {
            flags &= ~(1 << i);
        }
    }
    gReplay_splash_flags = flags;
}

// GetSplashIndex

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes

