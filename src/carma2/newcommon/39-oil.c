#include "39-oil.h"

#include "01-network.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0065a398
const char* gOil_pixie_names[1] = {
    "OIL.PIX",
};

// GLOBAL: CARMA2_HW 0x00690c88
br_pixelmap* gOil_pixies[1];

// GLOBAL: CARMA2_HW 0x0074a68c
br_material* oily_material;

/* explicit initializers keep gOily_spills and gNext_oil_pixie adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x00690c90
tOil_spill_info gOily_spills[32] = { 0 };

// GLOBAL: CARMA2_HW 0x00691710
int gNext_oil_pixie = 0;

/* defined here so it directly follows gNext_oil_pixie (RemoveCarsOilSlicks loop-end compare) */
// GLOBAL: CARMA2_HW 0x00691714
int gBig_bang = 0;

/* unannotated filler for the opponent globals at 0x00691718..0x0069175f so that
   gMinimum_yness_before_knackerisation sits 0x50 bytes after the end of gOily_spills (GetOilFrictionFactors loop-end compare) */
int gUNK_oil_opponent_gap[2] = { 0 };

/* defined here so that it sits exactly 8 bytes after gBig_bang;
   the QueueOilSpill2/RemoveCarsOilSlicks loop-end compares end at its address */
// GLOBAL: CARMA2_HW 0x00691720
int gStart_jumped = 0;

/* split so the array starts 8-aligned (0x00691728): MSVC 8-aligns initialized arrays, so a single
   int[15] here would be padded to 0x00691728 and push gMinimum_yness_before_knackerisation 4 bytes late */
int gUNK_oil_opponent_gap_hi_lo = 0;

int gUNK_oil_opponent_gap_hi[14] = { 0 };

// GLOBAL: CARMA2_HW 0x00691760
br_scalar gMinimum_yness_before_knackerisation = 0.f;

// FUNCTION: CARMA2_HW 0x004a6a10
void C2_HOOK_FASTCALL InitOilSpills(void) {
    int i;
    br_model* the_model;

    C2_HOOK_BUG_ON(sizeof(tOil_spill_info) != 0x54);

    for (i = 0; i < (int)REC2_ASIZE(gOil_pixie_names); i++) {
        gOil_pixies[i] = LoadPixelmap(gOil_pixie_names[i]);
        BrMapAdd(gOil_pixies[i]);
    }

    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        oily_material = BrMaterialAllocate(NULL);
        BrMaterialAdd(oily_material);
        oily_material->flags |= BR_MATF_LIGHT;
        oily_material->flags |= BR_MATF_PERSPECTIVE;
        oily_material->flags |= BR_MATF_SMOOTH;
        oily_material->ka = 0.99f;
        oily_material->kd = 0.0f;
        oily_material->ks = 0.0f;
        oily_material->power = 0.0f;
        oily_material->index_base = 0;
        oily_material->index_range = 0;
        oily_material->colour_map = gOil_pixies[0];
        BrMatrix23Identity(&oily_material->map_transform);
        oily_material->index_shade = BrTableFind("IDENTITY.TAB");
        GlorifyMaterial(&oily_material, 1, kRendererShadingType_Specular);
        BrMaterialUpdate(oily_material, BR_MATU_ALL);
        the_model = BrModelAllocate(NULL, 4, 2);
        the_model->flags |= BR_MODF_KEEP_ORIGINAL;

        the_model->faces[0].vertices[0] = 2;
        the_model->faces[0].vertices[1] = 1;
        the_model->faces[0].vertices[2] = 0;
        the_model->faces[0].material = NULL;
        the_model->faces[0].smoothing = 1;

        the_model->faces[1].vertices[0] = 3;
        the_model->faces[1].vertices[1] = 2;
        the_model->faces[1].vertices[2] = 0;
        the_model->faces[1].material = NULL;
        the_model->faces[1].smoothing = 1;

        BrVector3Set(&the_model->vertices[0].p, -1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&the_model->vertices[1].p, 1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&the_model->vertices[2].p, 1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&the_model->vertices[3].p, -1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[3].map, 1.f, 1.f);
        gOily_spills[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gOily_spills[i].actor->model = the_model;
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].actor->material = oily_material;
        BrActorAdd(gOther_selfs[2], gOily_spills[i].actor);
    }
}

// FUNCTION: CARMA2_HW 0x004a6c50
void C2_HOOK_FASTCALL ResetOilSpills(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].car = NULL;
        gOily_spills[i].car_actor = NULL;
        gOily_spills[i].stop_time = 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on ApexWorm/rec2 afa4cf121
// FUNCTION: CARMA2_HW 0x004a6c80
void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50) {
    int i;
    int free_index;
    int oldest_index;
    tU32 the_time;
    tU32 oldest_time;

    oldest_index = 0;
    free_index = -1;
    the_time = GetTotalTime();
    oldest_time = GetTotalTime();
    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        if (pCar != NULL && gOily_spills[i].car_actor == pCar_actor
            && the_time < gOily_spills[i].spill_time + 5000) {
            return;
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor == NULL) {
            free_index = i;
            break;
        }
        if (gOily_spills[i].spill_time < oldest_time) {
            oldest_time = gOily_spills[i].spill_time;
            oldest_index = i;
        }
    }
    if (free_index < 0) {
        free_index = oldest_index;
    }
    gOily_spills[free_index].actor->material = pMaterial;
    gOily_spills[free_index].car = pCar;
    gOily_spills[free_index].car_actor = pCar_actor;
    gOily_spills[free_index].car_model = pCar_model;
    gOily_spills[free_index].spill_time = the_time + pTime_delay;
    gOily_spills[free_index].field_0x20 = pInitial_size;
    gOily_spills[free_index].full_size = (float)SRandomBetween(pMin_size, pMax_size);
    gOily_spills[free_index].grow_rate = (float)SRandomBetween(pMin_grow_rate, pMax_grow_rate);
    gOily_spills[free_index].current_size = pInitial_size;
    gOily_spills[free_index].actor->render_style = BR_RSTYLE_NONE;
    gOily_spills[free_index].field_0x40 = pField_0x40;
    gOily_spills[free_index].field_0x44 = pField_0x44;
    gOily_spills[free_index].field_0x48 = pField_0x48;
    gOily_spills[free_index].field_0x4c = pField_0x4c;
    gOily_spills[free_index].field_0x50 = (undefined4)pField_0x50;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a6dd0
void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar) {
    QueueOilSpill2(pCar, pCar->car_master_actor, pCar->car_actor->model, 500,
        0.35f, 0.6f, 0.00003f, 0.0001f, 0.1f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
}

/* OKToSpillOil: inlined into ProcessOilSpills 0x4a6e50 in the original, no own address */
/* MungeOilsHeightAboveGround: inlined into its callers in the original */
static void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil) {

    pOil->actor->t.t.translate.t = pOil->position;
}

// FUNCTION: CARMA2_HW 0x004a6e20
void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex) {

    MungeOilsHeightAboveGround(&gOily_spills[pIndex]);
}

// FUNCTION: CARMA2_HW 0x004a7460
void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel) {
    /* Resize the spill in the ground plane without changing vertex heights. */
    pModel->vertices[0].p.v[0] = -pOil->field_0x20;
    pModel->vertices[0].p.v[2] = -pOil->field_0x20;
    pModel->vertices[1].p.v[0] = pOil->field_0x20;
    pModel->vertices[1].p.v[2] = -pOil->field_0x20;
    pModel->vertices[2].p.v[0] = pOil->field_0x20;
    pModel->vertices[2].p.v[2] = pOil->field_0x20;
    pModel->vertices[3].p.v[0] = -pOil->field_0x20;
    pModel->vertices[3].p.v[2] = pOil->field_0x20;
    pOil->actor->render_style = BR_RSTYLE_FACES;
    BrMaterialUpdate(pOil->actor->material, BR_MATU_ALL);
    BrModelUpdate(pModel, BR_MODU_ALL);
}

/* local externs (the shared 23-piping.h declares the scrap global volatile) */
extern volatile undefined4 gUNK_0069410c;
extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

// FUNCTION: CARMA2_HW 0x004c82e0
void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {
    (void)pGrow_rate;
    *(volatile float*)&gUNK_0069410c = pFull_size;
    ARDoSingleVariedSession(0x10, pIndex, 8,
        0x30, 0, pMat,
        4, 0x30, *(tU32*)&gUNK_0069410c,
        4, 0x34, *(tU32*)&gUNK_0069410c,
        4, 0x38, pSpill_time,
        4, 0x3c, pStop_time,
        4, 0x40, pCar,
        0xc, 0x44, pOriginal_pos,
        4, 0x50, pPixelmap);
}

extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);
extern void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

// PARTIAL 38.98%: loop regalloc (esi/edi swapped), frame 4 bytes larger
// FUNCTION: CARMA2_HW 0x004a6e50
void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period) {
    extern tNet_mode gNet_mode;
    int i;
    tU32 time;
    br_scalar this_size;
    tNet_message* message;

    time = GetTotalTime();
    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        br_model* the_model;
        tOil_spill_info* oil = &gOily_spills[i];

        if (oil->car_actor == NULL) {
            oil->actor->render_style = BR_RSTYLE_NONE;
        } else {
            the_model = oil->actor->model;
            if (oil->actor->render_style == BR_RSTYLE_NONE
                    && time >= oil->spill_time
                    && (oil->car == NULL || ((float)fabs(oil->car->collision_info->v.v[0]) < 0.0014492754f
                                          && (float)fabs(oil->car->collision_info->v.v[1]) < 0.0014492754f
                                          && (float)fabs(oil->car->collision_info->v.v[2]) < 0.0014492754f))) {
                if (gAction_replay_mode) {
                    SetInitialOilStuff(oil, the_model);
                } else {
                    br_scalar distance;
                    br_scalar mr_dotty;
                    br_vector3 pos;
                    br_vector3 ray_pos;
                    br_vector3 ray_dir;
                    br_vector3 normal;
                    int j;
                    int face_count;
                    int found_one;
                    br_angle angle_to_rotate_by;
                    tBounds kev_bounds;
                    tFace_ref the_list[10];

                    if (oil->car == NULL || oil->car->driver < 7
                            || oil->car->damage_units[eDamage_engine].damage_level >= 99
                            || oil->car->damage_units[eDamage_transmission].damage_level >= 99) {
                        angle_to_rotate_by = IRandomBetween(0, 0xffff);
                        BrVector3Set(&kev_bounds.original_bounds.min,
                            -oil->full_size,
                            oil->car_model->bounds.min.v[1] - 1.f / WORLD_SCALE,
                            -oil->full_size);
                        BrVector3Set(&kev_bounds.original_bounds.max,
                            oil->full_size,
                            oil->car_model->bounds.max.v[1],
                            oil->full_size);
                        BrMatrix34PreRotateY(&oil->actor->t.t.mat, angle_to_rotate_by);
                        kev_bounds.mat = &oil->car_actor->t.t.mat;
                        face_count = FindFacesInBox(&kev_bounds, the_list, REC2_ASIZE(the_list), NULL);
                        ray_pos.v[0] = oil->car_actor->t.t.translate.t.v[0];
                        ray_pos.v[1] = oil->car_actor->t.t.translate.t.v[1] - -0.2f;
                        ray_pos.v[2] = oil->car_actor->t.t.translate.t.v[2];
                        BrVector3Set(&ray_dir, 0.f, kev_bounds.original_bounds.min.v[1] - kev_bounds.original_bounds.max.v[1], 0.f);
                        BrVector3Normalise(&ray_dir, &ray_dir);
                        if (face_count != 0) {
                            found_one = 0;
                            for (j = 0; j < face_count; j++) {
                                if (!found_one) {
                                    CheckSingleFace(&the_list[j], &ray_pos, &ray_dir, &normal, &distance, &pos);
                                    if (distance < 100.f) {
                                        found_one = 1;
                                        BrVector3Copy(&oil->actor->t.t.look_up.up, &normal);
                                        oil->actor->t.t.mat.m[0][0] = normal.v[1];
                                        oil->actor->t.t.mat.m[0][1] = -normal.v[0];
                                        oil->actor->t.t.mat.m[0][2] = 0.f;
                                        oil->actor->t.t.mat.m[2][0] = 0.f;
                                        oil->actor->t.t.mat.m[2][1] = -normal.v[2];
                                        oil->actor->t.t.mat.m[2][2] = normal.v[1];
                                        BrVector3Copy(&oil->position, &pos);
                                        BrMatrix34PreRotateY(&oil->actor->t.t.mat, angle_to_rotate_by);
                                    }
                                }
                            }
                            if (found_one && normal.v[1] >= .97f) {
                                for (j = 0; j < face_count; j++) {
                                    mr_dotty = BrVector3Dot(&the_list[j].normal, &normal);
                                    if (mr_dotty < .98f && (mr_dotty > .8f || !NormalSideOfPlane(&oil->actor->t.t.translate.t, &the_list[j].normal, the_list[j].d))) {
                                        goto not_ok;
                                    }
                                }
                                oil->spill_time = time;
                                if (gNext_oil_pixie > 0) {
                                    gNext_oil_pixie = 0;
                                }
                                if (oil->car == NULL) {
                                    BrVector3Copy(&oil->original_pos, &oil->car_actor->t.t.translate.t);
                                } else {
                                    BrVector3Copy(&oil->original_pos, &oil->car->pos);
                                }
                                PipeSingleOilSpill(i,
                                    &oil->actor->t.t.mat,
                                    oil->full_size,
                                    oil->grow_rate,
                                    oil->spill_time,
                                    oil->stop_time,
                                    oil->car,
                                    &oil->original_pos,
                                    oil->actor->material->colour_map);
                                oil->stop_time = 0;
                                SetInitialOilStuff(oil, the_model);
                                if (gNet_mode == eNet_mode_host && oil->car != NULL) {
                                    message = NetBuildGuaranteedMessage(31, 0);
                                    message->contents.oil_spill.player = NetPlayerFromCar(oil->car)->ID;
                                    message->contents.oil_spill.full_size = oil->full_size;
                                    message->contents.oil_spill.grow_rate = oil->grow_rate;
                                    message->contents.oil_spill.current_size = oil->current_size;
                                    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                                }
                                goto munge;
                            }
                        }
                    }
not_ok:
                    oil->car = NULL;
                    oil->car_actor = NULL;
                }
            } else {
                if (oil->actor->render_style == BR_RSTYLE_FACES
                        && (oil->stop_time == 0 || time < oil->stop_time)) {

                    int grow = 1;
                    if (oil->car != NULL) {
                        br_scalar grow_amount = (oil->original_pos.v[0] - oil->car->pos.v[0]) * (oil->original_pos.v[0] - oil->car->pos.v[0])
                            + (oil->original_pos.v[1] - oil->car->pos.v[1]) * (oil->original_pos.v[1] - oil->car->pos.v[1])
                            + (oil->original_pos.v[2] - oil->car->pos.v[2]) * (oil->original_pos.v[2] - oil->car->pos.v[2]);
                        grow = oil->stop_time == 0 || grow_amount <= 0.2f;
                    }
                    if (grow) {
                        this_size = oil->field_0x20 + (time - oil->spill_time) * oil->grow_rate;
                        if (this_size >= oil->field_0x20) {
                            oil->actor->render_style = BR_RSTYLE_FACES;
                            if (this_size <= oil->full_size) {
                                the_model->vertices[0].p.v[0] = -this_size;
                                the_model->vertices[0].p.v[2] = -this_size;
                                the_model->vertices[1].p.v[0] =  this_size;
                                the_model->vertices[1].p.v[2] = -this_size;
                                the_model->vertices[2].p.v[0] =  this_size;
                                the_model->vertices[2].p.v[2] =  this_size;
                                the_model->vertices[3].p.v[0] = -this_size;
                                the_model->vertices[3].p.v[2] =  this_size;
                                oil->current_size = this_size;
                            } else {
                                the_model->vertices[0].p.v[0] = -oil->full_size;
                                the_model->vertices[0].p.v[2] = -oil->full_size;
                                the_model->vertices[1].p.v[0] =  oil->full_size;
                                the_model->vertices[1].p.v[2] = -oil->full_size;
                                the_model->vertices[2].p.v[0] =  oil->full_size;
                                the_model->vertices[2].p.v[2] =  oil->full_size;
                                the_model->vertices[3].p.v[0] = -oil->full_size;
                                the_model->vertices[3].p.v[2] =  oil->full_size;
                                oil->current_size = oil->full_size;
                            }
                            BrModelUpdate(the_model, BR_MODU_ALL);
                        } else {
                            oil->actor->render_style = BR_RSTYLE_NONE;
                        }
                    } else {
                        oil->stop_time = time;
                        continue;
                    }
                }
            }
        }
munge:
        if (oil->actor->render_style == BR_RSTYLE_FACES) {
            MungeOilsHeightAboveGround(oil);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a74e0
int C2_HOOK_FASTCALL GetOilSpillCount(void) {
    return (int)REC2_ASIZE(gOily_spills);
}

// FUNCTION: CARMA2_HW 0x004a74f0
void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize) {
    if (gOily_spills[pIndex].car_actor != NULL) {
        *pActor = gOily_spills[pIndex].actor;
        *pSize = gOily_spills[pIndex].full_size;
    } else {
        *pActor = NULL;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on ApexWorm/rec2 78828de5b
// FUNCTION: CARMA2_HW 0x004a77d0
int C2_HOOK_FASTCALL PointInSpill(br_vector3* pPoint, int pIndex) {
    br_actor* actor;
    br_scalar delta;
    br_scalar range_squared;

    actor = gOily_spills[pIndex].actor;
    delta = pPoint->v[0] * (1.f / WORLD_SCALE) - actor->t.t.mat.m[3][0];
    range_squared = gOily_spills[pIndex].current_size * gOily_spills[pIndex].current_size * 0.8f;
    if (delta * delta < range_squared && (delta = pPoint->v[2] * (1.f / WORLD_SCALE) - actor->t.t.mat.m[3][2], delta * delta < range_squared) && (float)fabs(pPoint->v[1] * (1.f / WORLD_SCALE) - actor->t.t.mat.m[3][1]) < 0.1f) {
        return 1;
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a7530
void C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFl, br_scalar* pFr, br_scalar* pRl, br_scalar* pRr) {
    int i;
    br_vector3 v;
    br_scalar a, b, c, d;

    *pFl = 1.f;
    *pFr = 1.f;
    *pRl = 1.f;
    *pRr = 1.f;
    if (pCar != NULL && (int)pCar->driver > 5) {
        if (pCar->shadow_intersection_flags != 0) {
            for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
                c = gOily_spills[i].field_0x48;
                d = gOily_spills[i].field_0x4c;
                if (d != 0.f && (pCar->shadow_intersection_flags & (1 << i))) {
                    a = gOily_spills[i].field_0x40;
                    b = gOily_spills[i].field_0x44;
                    BrMatrix34ApplyP(&v, &pCar->wpos[2], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[2] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[2] = SRandomBetween(a, b);
                        pCar->field_0x16fc[2] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&v, &pCar->wpos[3], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[3] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[3] = SRandomBetween(a, b);
                        pCar->field_0x16fc[3] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&v, &pCar->wpos[0], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[0] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[0] = SRandomBetween(a, b);
                        pCar->field_0x16fc[0] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&v, &pCar->wpos[1], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[1] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[1] = SRandomBetween(a, b);
                        pCar->field_0x16fc[1] = (br_material*)gOily_spills[i].field_0x50;
                    }
                }
            }
        }
        if (pCar->oil_remaining[2] != 0.f) {
            *pFl = pCar->maybe_blood_remaining[2];
        }
        if (pCar->oil_remaining[3] != 0.f) {
            *pFr = pCar->maybe_blood_remaining[3];
        }
        if (pCar->oil_remaining[0] != 0.f) {
            *pRl = pCar->maybe_blood_remaining[0];
        }
        if (pCar->oil_remaining[1] != 0.f) {
            *pRr = pCar->maybe_blood_remaining[1];
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a7860
void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {

    BrMatrix34Copy(&gOily_spills[pIndex].actor->t.t.mat, pMat);
    gOily_spills[pIndex].full_size = pFull_size;
    gOily_spills[pIndex].grow_rate = pGrow_rate;
    gOily_spills[pIndex].spill_time = pSpill_time;
    gOily_spills[pIndex].stop_time = 0;
    gOily_spills[pIndex].car = pCar;
    BrVector3Copy(&gOily_spills[pIndex].original_pos, pOriginal_pos);
    gOily_spills[pIndex].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004a78e0
// FUNCTION: CARMA2_SW 0x004a3a50
void C2_HOOK_FASTCALL ReceivedOilSpill(tNet_message_chunk* pContents) {
    tCar_spec* car;

    car = NetCarFromPlayerID(pContents->oil_spill.player);
    if (car == NULL) {
        return;
    }
    QueueOilSpill2(car, car->car_master_actor, car->car_actor->model, 500,
        0.35f, 0.6f, 0.00003f, 0.0001f, 0.1f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
}

// FUNCTION: CARMA2_HW 0x004a7940
void C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor != NULL && gOily_spills[i].car == pCar) {
            gOily_spills[i].car = NULL;
            gOily_spills[i].car_actor = NULL;
        }
    }
}

