#include "32-spark.h"

#include "08-loading1.h"
#include "18-graphics2.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_string.h"
#include "10-loading2.h"

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithSnart(br_actor* pActor, br_matrix34* pMat, tDRActorEnumRecurseWithSnart_cbfn* pCallback, void* pContext);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);
extern br_pixelmap* gFog_shade_table;
extern br_pixelmap* gAcid_shade_table;
extern int gRendering_mirror;

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

// GLOBAL: CARMA2_HW 0x006aa580
int gShrapnel_index;

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006aa588
br_model* gShrapnel_model[2];

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

// based on ApexWorm/rec2 f218c58ca, carpocalypse2 src/carma2/common/spark.c:2370 (reference only)
// FUNCTION: CARMA2_HW 0x004f7450
void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    br_matrix4 projection;
    br_matrix4 screen_scale;
    br_camera* cam;
    br_vector3 tv;
    br_vector3 o;
    br_vector3 pos;
    br_vector3 new_pos;
    br_vector3 p;
    tSpark* spark;
    br_scalar ts;
    int time;
    int i;

    (void)pCamera_to_world;
    cam = pCamera->type_data;
    gSpark_cam = cam;
    BrMatrix4Perspective(&projection, cam->field_of_view, cam->aspect, -cam->hither_z, -cam->yon_z);
    BrMatrix4Scale(&screen_scale, (br_scalar)(pRender_screen->width >> 1), (br_scalar)(pRender_screen->height >> 1), 1.f);
    BrMatrix4Mul(&gCameraToScreen, &projection, &screen_scale);
    if (gSpark_flags == 0) {
        return;
    }
    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(pCamera, gLine_actor);
    }
    if (gAction_replay_mode) {
        for (i = 0; i < (int)REC2_ASIZE(gSparks); i++) {
            spark = &gSparks[i];
            if (!(gSpark_flags & (1 << i))) {
                continue;
            }
            if (spark->car != NULL) {
                BrMatrix34ApplyP(&p, &o, &spark->car->car_master_actor->t.t.mat);
                o = p;
                BrMatrix34ApplyP(&pos, &spark->pos, &spark->car->car_master_actor->t.t.mat);
            } else {
                pos = spark->pos;
            }
            new_pos.v[0] = spark->length.v[0] + pos.v[0];
            new_pos.v[1] = spark->length.v[1] + pos.v[1];
            new_pos.v[2] = spark->length.v[2] + pos.v[2];
            if (gRendering_mirror) {
                BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&o, &tv, &gRearview_camera_to_world);
                BrVector3Sub(&tv, &new_pos, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
            } else {
                BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&o, &tv, &gCamera_to_world);
                BrVector3Sub(&tv, &new_pos, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
            }
            if (gNo_2d_effects) {
                SetLineModelCols(spark->colour);
            }
            if (spark->colour != 0) {
                DrawLine3D(&p, &o, pRender_screen, pDepth_buffer, gFog_shade_table);
            } else {
                DrawLine3D(&p, &o, pRender_screen, pDepth_buffer, gAcid_shade_table);
            }
        }
        if (!gNo_2d_effects) {
            return;
        }
        BrActorRemove(gLine_actor);
    } else {
        ARStartPipingSession(ePipe_chunk_spark);
        for (i = 0; i < (int)REC2_ASIZE(gSparks); i++) {
            spark = &gSparks[i];
            if (!(gSpark_flags & (1 << i))) {
                continue;
            }
            if (spark->count <= 0) {
                spark->count = 0;
                gSpark_flags &= ~(1 << i);
            }
            ts = BrVector3Dot(&spark->normal, &spark->v);
            BrVector3Scale(&tv, &spark->normal, ts);
            BrVector3Sub(&spark->v, &spark->v, &tv);
            if (spark->time_sync != 0) {
                BrVector3Scale(&o, &spark->v, spark->time_sync * 0.001f);
                spark->count += spark->time_sync - pTime;
                spark->time_sync = 0;
            } else {
                BrVector3Scale(&o, &spark->v, pTime * 0.001f);
                spark->count -= pTime;
            }
            BrVector3Accumulate(&spark->pos, &o);
            time = 1000 - spark->count;
            if (time > 150) {
                time = 150;
            }
            ts = -time * 0.001f;
            if (spark->colour) {
                ts *= 0.5f;
            }
            BrVector3Scale(&spark->length, &spark->v, ts);
            ts = pTime * 10.f * 0.00014492753623188405;
            if (spark->car != NULL) {
                BrMatrix34ApplyV(&tv, &spark->length, &spark->car->car_master_actor->t.t.mat);
                BrVector3Copy(&spark->length, &tv);
                BrMatrix34ApplyP(&pos, &spark->pos, &spark->car->car_master_actor->t.t.mat);
                spark->v.v[0] = spark->v.v[0] - spark->car->car_master_actor->t.t.mat.m[0][1] * ts;
                spark->v.v[1] = spark->v.v[1] - spark->car->car_master_actor->t.t.mat.m[1][1] * ts;
                spark->v.v[2] = spark->v.v[2] - spark->car->car_master_actor->t.t.mat.m[2][1] * ts;
            } else {
                BrVector3Copy(&pos, &spark->pos);
                spark->v.v[1] = spark->v.v[1] - ts;
            }
            AddSparkToPipingSession((spark->colour << 8) + i, &pos, &spark->length);
            BrVector3Add(&o, &spark->length, &pos);
            if (gRendering_mirror) {
                BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
            } else {
                BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
            }
            BrVector3SetFloat(&tv, FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f));
            BrVector3Accumulate(&spark->v, &tv);
            ts = 1.0f - BrVector3Length(&spark->v) * 0.71428573f * pTime * 0.001f;
            if (ts < 0.1f) {
                ts = 0.1f;
            }
            BrVector3Scale(&spark->v, &spark->v, ts);
            if (gNo_2d_effects) {
                SetLineModelCols(spark->colour);
            }
            if (spark->colour != 0) {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
            } else {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
            }
        }
        AREndPipingSession();
        if (!gNo_2d_effects) {
            return;
        }
        BrActorRemove(gLine_actor);
    }
    BrActorAdd(gDont_render_actor, gLine_actor);
}

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

// based on carpocalypse2 src/carma2/common/spark.c:3675 (reference only)
extern int gPHIL_mechanics_time_sync;

// FUNCTION: CARMA2_HW 0x004f8800
void C2_HOOK_FASTCALL CreateShrapnelShower(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar7;
    float fVar8;
    float fVar9;
    float fVar10;
    int shrapnel_count;
    int material_index;
    tCar_spec* selected_car;
    br_vector3 random_vector;
    br_vector3 scaled_direction;
    br_vector3 velocity;

    if (pStrength < 10.0f) {
        return;
    }
    if (pMaterials != NULL) {
        fVar8 = 0.7f;
    } else {
        fVar8 = 0.3f;
    }
    if (pVelocity->v[1] < 0.0f) {
        fVar8 -= pVelocity->v[1];
    }
    fVar9 = pDirection->v[1] * fVar8;
    fVar1 = pDirection->v[2];
    fVar2 = pDirection->v[0];
    fVar3 = pVelocity->v[0];
    fVar4 = pVelocity->v[2];
    fVar8 = (fVar8 + pVelocity->v[1]) - pDirection->v[1] * fVar9;
    BrVector3Set(&velocity, fVar3 - fVar2 * fVar9, fVar8, fVar4 - fVar1 * fVar9);
    shrapnel_count = (int)(pStrength * 0.1f);
    shrapnel_count *= 12;
    if (shrapnel_count > 20) {
        shrapnel_count = 20;
    }
    fVar10 = pStrength - -20.0f;
    fVar10 *= 3.0f;
    fVar10 *= 0.005f;
    if (shrapnel_count > 0) {
        do {
            if (((1 << gShrapnel_index) & gShrapnel_flags) == 0) {
                BrActorAdd(gNon_track_actor, gShrapnel[gShrapnel_index].actor);
            }
            gShrapnel_flags |= 1 << gShrapnel_index;
            BrVector3Copy(&gShrapnel[gShrapnel_index].actor->t.t.translate.t, pPosition);
            BrVector3SetFloat(&random_vector, FRandomBetween(-fVar10, fVar10), FRandomBetween((float)(0.3 - fVar8), fVar10), FRandomBetween(-fVar10, fVar10));
            fVar7 = BrVector3Dot(pDirection, &random_vector);
            BrVector3Scale(&scaled_direction, pDirection, fVar7);
            BrVector3Sub(&gShrapnel[gShrapnel_index].v, &random_vector, &scaled_direction);
            BrVector3Accumulate(&gShrapnel[gShrapnel_index].v, &velocity);
            gShrapnel[gShrapnel_index].time_sync = gPHIL_mechanics_time_sync;
            gShrapnel[gShrapnel_index].age = 0;
            if (pMaterials != NULL) {
                material_index = IRandomBetween(0, pMaterialCount - 1);
                gShrapnel[gShrapnel_index].actor->material = pMaterials[material_index];
            } else if (IRandomBetween(0, 2) != 0) {
                selected_car = pCar1;
                if (IRandomBetween(0, 1) == 0) {
                    selected_car = pCar2;
                }
                material_index = IRandomBetween(0, selected_car->count_shrapnel_materials - 1);
                gShrapnel[gShrapnel_index].actor->material = selected_car->shrapnel_materials[material_index];
            } else {
                gShrapnel[gShrapnel_index].actor->material = gBlack_material;
            }
            gShrapnel_index += 1;
            if (gShrapnel_index >= 30) {
                gShrapnel_index = 0;
            }
            shrapnel_count--;
        } while (shrapnel_count != 0);
    }
}

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

extern tCar_spec* gRecalc_mass_car;
extern tFace_ref* C2_HOOK_FASTCALL FindFloorInBoxM(br_vector3* pPos, br_vector3* pDir, br_vector3* pOut1, br_scalar* pDist, br_vector3* pOut2, tFace_ref* pFaces, int pCount);
extern tFace_ref gFace_list__car[300];

// FUNCTION: CARMA2_HW 0x004f8ff0
int C2_HOOK_FASTCALL SetPixelmap(br_material* pMaterial) {

    if (pMaterial->colour_map != gRecalc_mass_car->field_0x1960) {
        pMaterial->colour_map = gRecalc_mass_car->field_0x1960;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    } else {
        return 0;
    }
}

// based on carpocalypse2 src/carma2/common/spark.c:2862 (reconstructed from the disassembly)
// FUNCTION: CARMA2_HW 0x004f8f30
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
    br_scalar dist;
    br_vector3 dir;
    br_vector3 pos;
    br_vector3 out2;
    br_vector3 out1;
    tFace_ref* face;

    dir.v[0] = 0.0f;
    dir.v[1] = -0.5f;
    dir.v[2] = 0.0f;
    dist = 10.0f;
    BrMatrix34ApplyP(&pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    face = FindFloorInBoxM(&pos, &dir, &out1, &dist, &out2, &gFace_list__car[pCar->collision_info->box_face_start], pCar->collision_info->box_face_end - pCar->collision_info->box_face_start);
    if (face != NULL && face->material != NULL && face->material->colour_map != NULL && face->material->colour_map != pCar->field_0x1960) {
        pCar->field_0x1960 = face->material->colour_map;
        gRecalc_mass_car = pCar;
        ForEveryCarMaterial(pCar, SetPixelmap, 1);
    }
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004fb630
void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/spark.c:3182 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004fb890
void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime) {

    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
        return;
    }
    SmudgeCar(pCar, pRandom);
    if (pCar->knackered) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pType, pRandom, pLifetime);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// FUNCTION: CARMA2_HW 0x004fdde0
void C2_HOOK_FASTCALL InitSplash(FILE* pF) {
    int num;
    int num_files;
    int i;
    br_actor* actor;
    char the_path[256];
    char s[256];
    br_pixelmap* splash_maps[20];

    gSplash_flags = 0;
    gSplash_model = BrModelAllocate("Splash", 4, 2);
    strcpy(gCurrent_load_directory, "COMMON");
    if (pF != NULL) {
        num = GetAnInt(pF);
        gNum_splash_types = 0;
        for (i = 0; i < num; i++) {
            GetAString(pF, s);
            PathCat(the_path, the_path, s);
            strcpy(the_path, s);
            num_files = DRLoadMultiplePix(the_path, &splash_maps[gNum_splash_types], 20 - gNum_splash_types);
            if (!num_files) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
            }
            gNum_splash_types += num_files;
        }
    } else {
        strcpy(the_path, "SPLSHBLU.PIX");
        gNum_splash_types = DRLoadMultiplePix(the_path, splash_maps, 20);
    }
    BrMapAddMany(splash_maps, gNum_splash_types);
    for (i = 0; i < gNum_splash_types; i++) {
        gSplash_material[i] = BrMaterialAllocate(NULL);
        gSplash_material[i]->flags &= ~BR_MATF_LIGHT;
        gSplash_material[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gSplash_material[i]->colour_map = splash_maps[i];
        BrMaterialAdd(gSplash_material[i]);
    }
    gSplash_model->nvertices = 4;
    BrVector3SetFloat(&gSplash_model->vertices[0].p, -0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[1].p,  0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[2].p,  0.5f, 1.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[3].p, -0.5f, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[0].map, 0.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[1].map, 1.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[2].map, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[3].map, 0.0f, 0.0f);
    gSplash_model->nfaces = 2;
    gSplash_model->faces[0].vertices[0] = 0;
    gSplash_model->faces[0].vertices[1] = 1;
    gSplash_model->faces[0].vertices[2] = 2;
    gSplash_model->faces[1].vertices[0] = 0;
    gSplash_model->faces[1].vertices[1] = 2;
    gSplash_model->faces[1].vertices[2] = 3;
    gSplash_model->faces[0].smoothing = 1;
    gSplash_model->faces[1].smoothing = 1;
    BrModelAdd(gSplash_model);
    for (i = 0; i < (int)REC2_ASIZE(gSplash); i++) {
        gSplash[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        actor = gSplash[i].actor;
        actor->model = gSplash_model;
        actor->material = gNum_splash_types != 0 ? gSplash_material[IRandomBetween(0, gNum_splash_types - 1)] : NULL;
        gSplash[i].scale_x = SRandomBetween(0.9f, 1.1f) * (2 * IRandomBetween(0, 1) - 1);
    }
}

// FUNCTION: CARMA2_HW 0x004fc680
void C2_HOOK_FASTCALL DisposeSplash(void) {
    int i;
    tSplash* splash;

    for (i = 0; i < gNum_splash_types; i++) {
        BrMapRemove(gSplash_material[i]->colour_map);
        BrPixelmapFree(gSplash_material[i]->colour_map);
        BrMaterialRemove(gSplash_material[i]);
        BrMaterialFree(gSplash_material[i]);
    }
    i = 0;
    splash = &gSplash[0];
    do {
        if ((gSplash_flags & (1 << i)) != 0 && splash->actor->parent != NULL) {
            BrActorRemove(splash->actor);
        }
        BrActorFree(splash->actor);
        splash++;
        i++;
    } while ((int)splash < (int)((tU8*)gSplash + sizeof(gSplash)));
    BrModelRemove(gSplash_model);
    BrModelFree(gSplash_model);
}

// DrawTheGlow

// PipeInstantUnSmudge

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point) {
    (void)pCar;
    (void)pFire_point;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// based on carpocalypse2 src/carma2/common/spark.c:1236 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004fcab0
void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pLevel) {
    int i;
    int j;
    int flags;
    int fire_value;

    if (pLevel == 0 && (pCar == NULL || pCar->driver < 7)) {
        pLevel = 1;
    }
    flags = gColumn_flags;
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            for (j = 0; j < (int)REC2_ASIZE(gSmoke_column); j++) {
                if (gSmoke_column[j].core.car != pCar) {
                    continue;
                }
                if (!(flags & (1 << j)) || gSmoke_column[j].colour > pLevel) {
                    gSmoke_column[j].lifetime = 0x7d0;
                } else if (gSmoke_column[j].lifetime != 0) {
                    return;
                } else {
                    gSmoke_column[j].lifetime = 0x7d0;
                }
            }
            break;
        }
    }
    fire_value = pCar->fire_vertex[pFire_point];
    if (pCar->collision_info->last_special_volume != NULL) {
        if (pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
            return;
        }
    }
    SmudgeCar(pCar, fire_value);
    if (pCar->knackered) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pLevel, fire_value, 0x989680);
}

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

/* based on carpocalypse2 src/carma2/common/spark.c:383 (LoadInShrapnel and InitShrapnel inlined) */
extern br_model* C2_HOOK_FASTCALL LoadModel(const char* pName);
extern void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);
extern void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void);

// FUNCTION: CARMA2_HW 0x004fdc10
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {
    int i;
    br_scalar scale;
    double dscale;

    PackFileRevertTiffLoading();
    PossibleService();
    gShrapnel_model[0] = LoadModel("FRAG4.DAT");
    DRScaleModel(gShrapnel_model[0], 0.5f);
    gShrapnel_model[1] = LoadModel("FRAG5.DAT");
    DRScaleModel(gShrapnel_model[1], 0.5f);
    BrModelAdd(gShrapnel_model[0]);
    BrModelAdd(gShrapnel_model[1]);
    gBlack_material = GetSimpleMaterial("M14.MAT", 4);
    gNon_track_actor->material = gBlack_material;
    PossibleService();
    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        gShrapnel[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gShrapnel[i].actor->parent = NULL;
        gShrapnel[i].actor->model = gShrapnel_model[i & 1];
        gShrapnel[i].actor->render_style = BR_RSTYLE_DEFAULT;
        gShrapnel[i].actor->t.type = BR_TRANSFORM_MATRIX34;
        gShrapnel[i].actor->material = BrMaterialFind("DEBRIS.MAT");
        gShrapnel[i].age = 0;
        gShrapnel[i].shear1 = FRandomBetween(-2.f, 2.f);
        gShrapnel[i].shear2 = FRandomBetween(-2.f, 2.f);
        BrVector3SetFloat(&gShrapnel[i].axis,
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f));
        scale = BR_LENGTH3(gShrapnel[i].axis.v[0], gShrapnel[i].axis.v[1], gShrapnel[i].axis.v[2]);
        if (scale > BR_SCALAR_EPSILON * 2) {
            dscale = 1.0 / scale;
            gShrapnel[i].axis.v[0] = gShrapnel[i].axis.v[0] * dscale;
            gShrapnel[i].axis.v[1] = gShrapnel[i].axis.v[1] * dscale;
            gShrapnel[i].axis.v[2] = gShrapnel[i].axis.v[2] * dscale;
        } else {
            gShrapnel[i].axis.v[0] = 1.0f;
            gShrapnel[i].axis.v[1] = 0.0f;
            gShrapnel[i].axis.v[2] = 0.0f;
        }
    }
    PossibleService();
    InitFlame();
    PossibleService();
    InitSplash(pF);
    PackFileRerevertTiffLoading();
}

// DisposeKevStuff

// GLOBAL: CARMA2_HW 0x006601b0
const char gSolidgrn_material[10] = "solidgrn";

extern void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex);
extern int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial);
extern void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pArg4, br_uint_8 pArg5, int pArg6);
extern intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext);
extern int C2_HOOK_FASTCALL SetModelAFEUpdateCB(br_model* pModel);

// based on carpocalypse2 src/carma2/common/spark.c:687
// FUNCTION: CARMA2_HW 0x004fe360
void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar) {
    int i;
    int j;
    tCar_crush_spec* spec;
    br_actor* actor;
    br_model* model;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;
    br_material* solid_mat;
    int funk_index;

    if (!gAFE) {
        return;
    }
    spec = pCar->car_crush_spec;
    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterDisableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    solid_mat = BrMaterialFind(gSolidgrn_material);
    if (solid_mat == NULL) {
        return;
    }
    if (solid_mat->colour_map == NULL) {
        return;
    }
    gRecalc_mass_car = pCar;
    pCar->field_0x1960 = solid_mat->colour_map;
    ForEveryCarMaterial(pCar, SetMaterialPrelitCB, 1);
    pCar->shell_model->custom = AFEModelUpdate;
    pCar->shell_model->flags |= BR_MODF_CUSTOM;
    user = (tUser_crush_data*)pCar->car_model_actor->user;
    model = user->models[1];
    if (model != NULL) {
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
    DRActorEnumRecurse(pCar->car_model_actor, ForEveryActorModelNoGrooves, (void*)SetModelAFEUpdateCB);
    for (i = 0; i < spec->field_0x270; i++) {
        model = spec->field_0x274[i].field_0x0->model;
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        model = spec->field_0x2b4[i].field_0x0->model;
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
}

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

extern int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterial(br_material* pMaterial);

// FUNCTION: CARMA2_HW 0x004fed00
// STUB: CARMA2_SW 0x004fa5e0
int C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) != 25) {
        BlendifyMaterial(pMaterial, 25);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fecf0
// FUNCTION: CARMA2_SW 0x004fa5d0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    ForEveryCarMaterial(pCar, BlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed50
// STUB: CARMA2_SW 0x004fa630
int C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) != 100) {
        BlendifyMaterial(pMaterial, 100);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
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

extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pCollision_info, int pParam);

// FUNCTION: CARMA2_HW 0x004fee10
void* C2_HOOK_FASTCALL GetSplashIndex(tPhysics_object* pObject) {

    if (PHILGetObjectProperty(pObject, 6) != 0.f) {
        return pObject;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        return pObject->actor;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return pObject->owner;
    }
    return NULL;
}

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes

// GLOBAL: CARMA2_HW 0x006a82a8
tCar_spec* gRecalc_mass_car = NULL;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004226d0
tFace_ref* C2_HOOK_FASTCALL FindFloorInBoxM(br_vector3* pPos, br_vector3* pDir, br_vector3* pOut1, br_scalar* pDist, br_vector3* pOut2, tFace_ref* pFaces, int pCount) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f9180
void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pArg4, br_uint_8 pArg5, int pArg6) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f95e0
int C2_HOOK_FASTCALL SetModelAFEUpdateCB(br_model* pModel) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f9600
int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004fec30
intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x004fb330
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {
    NOT_IMPLEMENTED();
}
