#include "16-graphics1.h"

#include "02-init.h"
#include "15-displays.h"
#include "19-font.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "brender/brender.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_types.h"
#include "rec2_macros.h"
#include "00-car.h"
#include "18-graphics2.h"
#include "33-depth.h"
#include "34-finteray.h"
#include "39-oil.h"
#include "49-physics.h"
#include "67-lighting.h"

#define SHADOW_D_IGNORE_FLAG 10000.f

br_scalar C2_HOOK_FASTCALL DistanceFromPlane(br_vector3* pPos, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD);

#include <string.h>

// GLOBAL: CARMA2_HW 0x00703e24
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
int gHeight;

// GLOBAL: CARMA2_HW 0x00705060
int gX_offset;

// GLOBAL: CARMA2_HW 0x006baa2c
int gY_offset;

// GLOBAL: CARMA2_HW 0x0068be30
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x006a22bc
br_pixelmap* gRear_pixelmap;

// GLOBAL: CARMA2_HW 0x006a22b8
br_pixelmap* gEvalu;

/* explicit initializers keep both blocks adjacent and in address order (InitWobbleStuff loop-end compares) */
// GLOBAL: CARMA2_HW 0x006a22f8
tWobble_spec gWobble_array[5] = { 0 };

/* UNCERTAIN: unreferenced dword */
// GLOBAL: CARMA2_HW 0x006a2348
int gUNK_006a2348 = 0;

// GLOBAL: CARMA2_HW 0x006a234c
float gPrevious_rear_yon = 0.f;

/* UNCERTAIN: unreferenced dwords (0x006a2354 is only seen as gWobble_array loop end) */
// GLOBAL: CARMA2_HW 0x006a2350
int gUNK_006a2350 = 0;

// GLOBAL: CARMA2_HW 0x006a2354
int gUNK_006a2354 = 0;

/* 65 entries: InitWobbleStuff fills 0..64 inclusive (loop ends at 0x00705184 = gScreen_wobble_y) */
// GLOBAL: CARMA2_HW 0x00705080
float gCosine_array[65] = { 0 };

// GLOBAL: CARMA2_HW 0x00705184
int gScreen_wobble_y = 0;

// GLOBAL: CARMA2_HW 0x00705188
int gScreen_wobble_x = 0;

// GLOBAL: CARMA2_HW 0x0065fdc8
tShadow_level gShadow_level = eShadow_us_only;

// GLOBAL: CARMA2_HW 0x0068be38
int gRender_indent;

// GLOBAL: CARMA2_HW 0x006baa40
tU8 gTemporary_physics_render_buffer[300000];

// GLOBAL: CARMA2_HW 0x006a2448
tClip_details gShadow_clip_planes[8];

// GLOBAL: CARMA2_HW 0x006a23d8
int gFancy_shadow;

// GLOBAL: CARMA2_HW 0x006a27e0
br_material* gShadow_material;

// GLOBAL: CARMA2_HW 0x006a27d0
br_vector3 gShadow_light_ray;

// GLOBAL: CARMA2_HW 0x006a27c0
br_vector3 gShadow_light_z;

// GLOBAL: CARMA2_HW 0x006a27b0
br_vector3 gShadow_light_x;

// GLOBAL: CARMA2_HW 0x006a27e4
br_model* gShadow_model;

// GLOBAL: CARMA2_HW 0x006a2444
br_actor* gShadow_actor;


// GLOBAL: CARMA2_HW 0x006a2488
tSaved_table gSaved_shade_tables[100];

// GLOBAL: CARMA2_HW 0x006a27a8
int gSaved_table_count;

// GLOBAL: CARMA2_HW 0x00703e28
int gHud_actor_storage_size;

// GLOBAL: CARMA2_HW 0x00704e60
br_actor* gHud_actor_storage[128];

// GLOBAL: CARMA2_HW 0x006a22c4
int gNumber_of_lollipops;

/* based on carpocalypse2 src/carma2/common/graphics.c:1901 */
// FUNCTION: CARMA2_HW 0x004e9680
void C2_HOOK_FASTCALL MungeClipPlane(br_vector3* pLight, tCar_spec* pCar, br_vector3* p1, br_vector3* p2, br_vector3* pOffset) {
    extern int gShadow_clip_plane_count;
    br_vector3 v4;
    br_vector3 v3;
    br_vector3 v1;
    br_vector3 v2;

    BrMatrix34ApplyP(&v1, p1, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyP(&v2, p2, &pCar->car_master_actor->t.t.mat);
    BrVector3Sub(&v3, p2, p1);
    v4.v[0] = pLight->v[2] * v3.v[1] - pLight->v[1] * v3.v[2];
    v4.v[1] = pLight->v[0] * v3.v[2] - pLight->v[2] * v3.v[0];
    v4.v[2] = pLight->v[1] * v3.v[0] - pLight->v[0] * v3.v[1];
    if (fabs(v4.v[0]) >= 0.01 || fabs(v4.v[1]) >= 0.01 || fabs(v4.v[2]) >= 0.01) {
        BrVector3Sub(&v3, p1, pOffset);
        if (BrVector3Dot(&v3, &v4) > 0.f) {
            BrVector3Negate(&v4, &v4);
        }
        BrVector3Normalise(&v3, &v4);
        BrMatrix34ApplyV(&v4, &v3, &pCar->car_master_actor->t.t.mat);
        ((br_vector4*)gShadow_clip_planes[gShadow_clip_plane_count].clip->type_data)->v[0] = v4.v[0];
        ((br_vector4*)gShadow_clip_planes[gShadow_clip_plane_count].clip->type_data)->v[1] = v4.v[1];
        ((br_vector4*)gShadow_clip_planes[gShadow_clip_plane_count].clip->type_data)->v[2] = v4.v[2];
        ((br_vector4*)gShadow_clip_planes[gShadow_clip_plane_count].clip->type_data)->v[3] = -BrVector3Dot(&v4, &v1);
        gShadow_clip_planes[gShadow_clip_plane_count].length = (v1.v[2] - v2.v[2]) * (v1.v[2] - v2.v[2]) + (v1.v[0] - v2.v[0]) * (v1.v[0] - v2.v[0]);
        gShadow_clip_plane_count += 1;
    }
}

void C2_HOOK_FASTCALL TryThisEdge(tCar_spec* pCar, br_vector3* pLight, int pIndex_1, br_scalar pSign_1, int pIndex_2, br_scalar pSign_2, int pPoint_index_1, int pPoint_index_2, br_vector3* pOffset) {
    extern int gShadow_clip_plane_count;
    extern br_vector3 gShadow_points[8];
    br_scalar dot_1;
    br_scalar dot_2;
    br_scalar mult;

    dot_1 = pSign_1 * pLight->v[pIndex_1];
    dot_2 = pSign_2 * pLight->v[pIndex_2];
    mult = dot_1 * dot_2;
    if (mult < 0 || (mult == 0 && (dot_1 > 0 || dot_2 > 0))) {
        if (gShadow_clip_plane_count < 6) {
            MungeClipPlane(pLight, pCar, &gShadow_points[pPoint_index_1], &gShadow_points[pPoint_index_2], pOffset);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e7650
void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor) {
    int i;
    int face_count;
    int oily_count;
    int f_num;
    br_vector3 pos;
    br_vector3 light_ray_car;
    br_vector3 shadow_points_world[8];
    br_vector3 poly_centre;
    br_vector3 car_to_poly;
    br_vector3 ray;
    br_vector3 ray_pos;
    br_vector3 normal;
    br_vector4* clip_normal;
    br_bounds3 bounds;
    br_vector3 center;
    br_scalar oily_size;
    br_scalar highest_underneath;
    br_scalar shadow_scaling_factor;
    br_scalar first_poly_below;
    br_scalar distance;
    br_scalar ray_length;
    tBounds kev_bounds;
    tFace_ref the_list[100];
    tFace_ref* face_ref;
    tFace_ref* list_ptr;
    br_actor* oily_actor;
    br_material* material;
    br_vertex verts[48];
    br_face faces[16];
    tUser_crush_data* user_crush_data;
    br_model* car_crush_model;
    extern br_matrix34 gIdentity34;
    extern int gShadow_dim_amount;
    extern br_vector3 gShadow_points[8];
    extern int gShadow_clip_plane_count;

#if defined(REC2_FIX_BUGS)
    ray_length = 0.f;
#endif

    f_num = 0;
    user_crush_data = pCar->car_model_actor->user;
    car_crush_model = user_crush_data->models[0];
    BrVector3Copy(&bounds.min, &car_crush_model->bounds.min);
    BrVector3Copy(&bounds.max, &car_crush_model->bounds.max);
    BrVector3Set(&gShadow_points[0], bounds.max.v[0], bounds.max.v[1], bounds.max.v[2]);
    BrVector3Set(&gShadow_points[1], bounds.max.v[0], bounds.max.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[2], bounds.min.v[0], bounds.max.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[3], bounds.min.v[0], bounds.max.v[1], bounds.max.v[2]);
    BrVector3Set(&gShadow_points[4], bounds.max.v[0], bounds.min.v[1], bounds.max.v[2]);
    BrVector3Set(&gShadow_points[5], bounds.max.v[0], bounds.min.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[6], bounds.min.v[0], bounds.min.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[7], bounds.min.v[0], bounds.min.v[1], bounds.max.v[2]);
    gShadow_clip_plane_count = 0;
    BrMatrix34TApplyV(&light_ray_car, &gShadow_light_ray, &pCar->car_master_actor->t.t.mat);
    Vector3Average(&center, &bounds.max, &bounds.min);
    TryThisEdge(pCar, &light_ray_car, 2,  1.f, 1,  1.f, 0, 3, &center);
    TryThisEdge(pCar, &light_ray_car, 2, -1.f, 1,  1.f, 1, 2, &center);
    TryThisEdge(pCar, &light_ray_car, 2, -1.f, 1, -1.f, 6, 5, &center);
    TryThisEdge(pCar, &light_ray_car, 2,  1.f, 1, -1.f, 7, 4, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 1,  1.f, 1, 0, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 1,  1.f, 2, 3, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 1, -1.f, 7, 6, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 1, -1.f, 4, 5, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 2,  1.f, 4, 0, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 2,  1.f, 3, 7, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 2, -1.f, 2, 6, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 2, -1.f, 5, 1, &center);

    for (i = 0; i < gShadow_clip_plane_count; ++i) {
        BrClipPlaneEnable(gShadow_clip_planes[i].clip);
    }

    face_count = 0;
    if (!gAction_replay_mode && pCar->number_of_wheels_on_ground > 2) {
        face_count = GetPrecalculatedFacesUnderCar(pCar, &face_ref);
    }
    if (face_count != 0) {
        highest_underneath = 0.0;
    } else {
        BrVector3Set(&kev_bounds.original_bounds.min,  1000.f,  1000.f,  1000.f);
        BrVector3Set(&kev_bounds.original_bounds.max, -1000.f, -1000.f, -1000.f);
        for (i = 0; i < REC2_ASIZE(shadow_points_world); i++) {
            BrMatrix34ApplyP(&shadow_points_world[i], &gShadow_points[i], &pCar->car_master_actor->t.t.mat);
            if (shadow_points_world[i].v[0] < kev_bounds.original_bounds.min.v[0]) {
                kev_bounds.original_bounds.min.v[0] = shadow_points_world[i].v[0];
            } else if (shadow_points_world[i].v[0] > kev_bounds.original_bounds.max.v[0]) {
                kev_bounds.original_bounds.max.v[0] = shadow_points_world[i].v[0];
            }
            if (shadow_points_world[i].v[1] < kev_bounds.original_bounds.min.v[1]) {
                kev_bounds.original_bounds.min.v[1] = shadow_points_world[i].v[1];
            } else if (shadow_points_world[i].v[1] > kev_bounds.original_bounds.max.v[1]) {
                kev_bounds.original_bounds.max.v[1] = shadow_points_world[i].v[1];
            }
            if (shadow_points_world[i].v[2] < kev_bounds.original_bounds.min.v[2]) {
                kev_bounds.original_bounds.min.v[2] = shadow_points_world[i].v[2];
            } else if (shadow_points_world[i].v[2] > kev_bounds.original_bounds.max.v[2]) {
                kev_bounds.original_bounds.max.v[2] = shadow_points_world[i].v[2];
            }
        }
        kev_bounds.original_bounds.min.v[1] -= 4.4f;
        kev_bounds.mat = &gIdentity34;
        face_count = FindFacesInBox(&kev_bounds, the_list, REC2_ASIZE(the_list), NULL);
        face_ref = the_list;
        highest_underneath = 1000.f;
        ray_length = kev_bounds.original_bounds.max.v[1] - kev_bounds.original_bounds.min.v[1];
        BrVector3Set(&ray, 0.f, -ray_length, 0.f);
        BrVector3Copy(&ray_pos, &pCar->car_master_actor->t.t.translate.t);
        ray_pos.v[1] = kev_bounds.original_bounds.max.v[1];
    }
    if (face_count != 0) {
        first_poly_below = -1000.0;
        for (i = 0; i < face_count; i++) {
            list_ptr = &face_ref[i];

            if (list_ptr->normal.v[1] >= -0.1 || (list_ptr->material != NULL && (list_ptr->material->flags & BR_MATF_TWO_SIDED))) {
                if (list_ptr->normal.v[1] < 0.0
                        || (list_ptr->material != NULL && list_ptr->material->identifier != NULL && (list_ptr->material->identifier[0] == '!' || list_ptr->material->identifier[0] == '>'))
                        || ((list_ptr->v[0].v[1] > pCar->pos.v[1] || list_ptr->v[1].v[1] > pCar->pos.v[1] || list_ptr->v[2].v[1] > pCar->pos.v[1]) && list_ptr->normal.v[1] < 0.1f)) {
                    list_ptr->d += SHADOW_D_IGNORE_FLAG;
                }
            } else {
                BrVector3Add(&poly_centre, &list_ptr->v[0], &list_ptr->v[1]);
                BrVector3Add(&poly_centre, &poly_centre, &list_ptr->v[2]);
                BrVector3InvScale(&poly_centre, &poly_centre, 3.f);
                if (poly_centre.v[1] > first_poly_below) {
                    BrVector3Sub(&car_to_poly, &poly_centre, &pCar->car_master_actor->t.t.translate.t);

                    if (BrVector3Dot(&list_ptr->normal, &car_to_poly) > 0.0) {
                        first_poly_below = poly_centre.v[1];
                    }
                }
                list_ptr->d += SHADOW_D_IGNORE_FLAG;
            }
        }
        for (i = 0; i < face_count; i++) {
            list_ptr = &face_ref[i];

            if (list_ptr->d < SHADOW_D_IGNORE_FLAG / 2.f) {
                if (list_ptr->v[0].v[1] >= first_poly_below || list_ptr->v[1].v[1] >= first_poly_below || list_ptr->v[2].v[1] >= first_poly_below) {
                    if (gFancy_shadow) {
                        faces[f_num].material = list_ptr->material;
                        if (gShadow_workaround) {
                            list_ptr->material->ka = 0.75f * gLighting_data.ambient_else;
                            BrMaterialUpdate(list_ptr->material, BR_MATU_LIGHTING);
                        } else if (list_ptr->material != NULL && list_ptr->material->colour_map != NULL && (!(list_ptr->material->flags & BR_MATF_LIGHT))) {
                            list_ptr->material->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
                            BrMaterialUpdate(list_ptr->material, BR_MATU_RENDERING);
                        }
                    } else {
                        faces[f_num].material = gShadow_material;
                    }

                    BrVector3Copy(&verts[3 * f_num + 0].p, &list_ptr->v[0]);
                    BrVector2Copy(&verts[3 * f_num + 0].map, list_ptr->map[0]);
                    BrVector3Copy(&verts[3 * f_num + 1].p, &list_ptr->v[1]);
                    BrVector2Copy(&verts[3 * f_num + 1].map, list_ptr->map[1]);
                    BrVector3Copy(&verts[3 * f_num + 2].p, &list_ptr->v[2]);
                    BrVector2Copy(&verts[3 * f_num + 2].map, list_ptr->map[2]);
                    faces[f_num].vertices[0] = 3 * f_num + 0;
                    faces[f_num].vertices[1] = 3 * f_num + 1;
                    faces[f_num].vertices[2] = 3 * f_num + 2;
                    f_num += 1;
                    if (highest_underneath > 0.0) {
                        CheckSingleFace(list_ptr, &ray_pos, &ray, &normal, &distance, &pos);
                        if (distance < 1.f && ray_length * distance < highest_underneath) {
                            highest_underneath = ray_length * distance;
                        }
                    }
                    if (f_num >= REC2_ASIZE(faces)) {
                        break;
                    }
                }
            } else {
                list_ptr->d -= SHADOW_D_IGNORE_FLAG;
            }
        }
        highest_underneath -= (bounds.max.v[1] - bounds.min.v[1]);
        if (highest_underneath > 2.2f) {
            highest_underneath = 2.2f;
        } else if (highest_underneath < 0.f) {
            highest_underneath = 0.0;
        }
        if (gFancy_shadow) {
            gShadow_dim_amount = (int)(2.5f + (2.2f - highest_underneath) * 5.0f / 2.2f);
            for (i = 0; i < gSaved_table_count; i++) {
                gSaved_shade_tables[i].original->height = 1;
                gSaved_shade_tables[i].original->pixels = (tU8*)gDepth_shade_table->pixels + gShadow_dim_amount * gDepth_shade_table->row_bytes;
                BrTableUpdate(gSaved_shade_tables[i].original, BR_TABU_ALL);
            }
        }
        shadow_scaling_factor = 0.4f + (2.2f - highest_underneath) * 0.52f / 2.2f;
        for (i = 0; i < gShadow_clip_plane_count; i++) {
            clip_normal = gShadow_clip_planes[i].clip->type_data;
            distance = DistanceFromPlane(&pCar->car_master_actor->t.t.translate.t, clip_normal->v[0], clip_normal->v[1], clip_normal->v[2], clip_normal->v[3]);
            BrVector3Set(&gShadow_clip_planes[i].clip->t.t.translate.t,
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[0],
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[1],
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[2]);
        }
        if (f_num != 0) {
            DisableLights();
            BrZbsSceneRenderBegin(gUniverse_actor, gCamera, gRender_screen, gDepth_buffer);
            EnableLights();
            BrSetScreenZOffset(2);
            gShadow_model->vertices = verts;
            gShadow_model->faces = faces;
            gShadow_model->nfaces = f_num;
            gShadow_model->nvertices = 3 * f_num;
            gShadow_actor->render_style = BR_RSTYLE_FACES;
            BrModelAdd(gShadow_model);
            BrZbsSceneRenderAdd(gShadow_actor);
            BrModelRemove(gShadow_model);
            if (pCar->shadow_intersection_flags != 0) {
                oily_count = GetOilSpillCount();
                for (i = 0; i < oily_count; ++i) {
                    if (pCar->shadow_intersection_flags & (1 << i)) {
                        GetOilSpillDetails(i, &oily_actor, &oily_size);
                        if (oily_actor) {
                            MungeIndexedOilsHeightAboveGround(i);
                            BrZbSceneRenderAdd(oily_actor);
                        }
                    }
                }
            }
            BrSetScreenZOffset(0);
            BrZbSceneRenderEnd();

            for (i = 0; i < f_num; i++) {
                if (gFancy_shadow) {
                    material = gShadow_model->faces[i].material;
                    if (gShadow_workaround) {
                        SmoothificateWorldMaterial(material);
                    } else {
                        if (material != NULL && material->colour_map != NULL && (material->flags & BR_MATF_LIGHT)) {
                            material->flags &= ~(BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH);
                            BrMaterialUpdate(material, BR_MATU_RENDERING);
                        }
                    }
                }
            }
        }
    }
    gShadow_actor->render_style = BR_RSTYLE_NONE;
    for (i = 0; i < gShadow_clip_plane_count; i++) {
        BrClipPlaneDisable(gShadow_clip_planes[i].clip);
    }
}

// RenderShadows

// FUNCTION: CARMA2_HW 0x004e9940
void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    gShadow_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x004e9950
tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {

    return gShadow_level;
}

// FUNCTION: CARMA2_HW 0x004e9960
void C2_HOOK_FASTCALL ToggleShadow(void) {

    if (gShadow_level++ == eShadow_everyone) {
        gShadow_level = eShadow_none;
    }
    switch (gShadow_level) {
    case eShadow_none:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowNone));
        break;
    case eShadow_us_only:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsOnly));
        break;
    case eShadow_us_and_opponents:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsAndOpponents));
        break;
    case eShadow_everyone:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowEveryone));
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004e99d0
void C2_HOOK_FASTCALL InitShadow(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(gShadow_clip_planes[0]) != 8);

    for (i = 0; i < (int)REC2_ASIZE(gShadow_clip_planes); i++) {
        gShadow_clip_planes[i].clip = BrActorAllocate(BR_ACTOR_CLIP_PLANE, NULL);
        BrActorAdd(gUniverse_actor, gShadow_clip_planes[i].clip);
        BrClipPlaneDisable(gShadow_clip_planes[i].clip);
        BrMatrix34Identity(&gShadow_clip_planes[i].clip->t.t.mat);
    }
    gFancy_shadow = 1;
    gShadow_material = BrMaterialFind("SHADOW.MAT");
    BrVector3Set(&gShadow_light_ray, 0.f, -1.f, 0.f);
    BrVector3Set(&gShadow_light_z, 0.f, 0.f, -1.f);
    BrVector3Set(&gShadow_light_x, 1.f, 0.f, 0.f);

    gShadow_model = BrModelAllocate("", 0, 0);
    gShadow_model->flags = BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL;
    gShadow_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gShadow_actor->model = gShadow_model;
    BrActorAdd(gUniverse_actor, gShadow_actor);
}

// FUNCTION: CARMA2_HW 0x004e9b10
br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg) {
    br_pixelmap* copy;

    (void)pArg;
    C2_HOOK_BUG_ON(REC2_ASIZE(gSaved_shade_tables) != 100);
    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);

    if (gSaved_table_count == (int)REC2_ASIZE(gSaved_shade_tables)) {
        return 1;
    } else {
        gSaved_shade_tables[gSaved_table_count].original = pTable;
        copy = BrMemAllocate(sizeof(br_pixelmap), kMem_misc);
        gSaved_shade_tables[gSaved_table_count].copy = copy;
        gSaved_table_count++;
        memcpy(copy, pTable, sizeof(br_pixelmap));
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e9ae0
void C2_HOOK_FASTCALL SaveShadeTables(void) {
    PossibleService();
    gSaved_table_count = 0;
    BrTableEnum("*", SaveShadeTable, NULL);
}

// FUNCTION: CARMA2_HW 0x004e9b60
void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
    int i;

    for (i = 0; i < gSaved_table_count; i++) {
        BrMemFree(gSaved_shade_tables[i].copy);
    }
}

// FUNCTION: CARMA2_HW 0x004e9b90
void C2_HOOK_FASTCALL ShadowMode(void) {

    gFancy_shadow = !gFancy_shadow;
    if (gFancy_shadow) {
        NewTextHeadupSlot(4, 0, 2000, -4, "Translucent shadow");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "Solid shadow");
    }
}
// LollipopizeActor

// FUNCTION: CARMA2_HW 0x004e4900
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {
    gNumber_of_lollipops = 0;
}

// FUNCTION: CARMA2_HW 0x004e4940
void C2_HOOK_FASTCALL SetupDepthBuffer(void) {

    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = NULL;
    }
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    if (gDepth_buffer == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }
}

// FUNCTION: CARMA2_HW 0x004e4980
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = gBack_screen->width;
    }
    if (pHeight == 0) {
        pHeight = gBack_screen->height;
    }
    gRender_screen = DRPixelmapAllocateSub(gBack_screen, (br_uint_16)pX_offset, (br_uint_16)pY_offset, (br_uint_16)pWidth, (br_uint_16)pHeight);
    gWidth = pWidth;
    gX_offset = pX_offset;
    gY_offset = pY_offset;
    gHeight = pHeight;
    if (gGraf_specs[gGraf_spec_index].doubled) {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - 2 * gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - 2 * gGraf_specs[gGraf_spec_index].total_height) / 2;
    } else {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - gGraf_specs[gGraf_spec_index].total_height) / 2;
    }

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    if (gBack_screen == NULL) {
        FatalError(kFatalError_AllocateOffScreenBuffer);
    }

    SetupDepthBuffer();

    BrZbsBegin(gRender_screen->type, gDepth_buffer->type, gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
    gBrZb_initialized = 1;
    gRear_pixelmap = DRPixelmapAllocate(gScreen->type, 64, 64, NULL, 0);
    BrMapAdd(gRear_pixelmap);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted4;
extern int gMap_render_x_i;
extern int gMap_render_y_i;
extern int gMap_render_width_i;
extern int gMap_render_height_i;
extern void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height);

// based on carpocalypse2 src/carma2/common/graphics.c:270
// FUNCTION: CARMA2_HW 0x004e4b40
// FUNCTION: CARMA2_SW 0x004e0ed0
void C2_HOOK_FASTCALL AdjustRenderScreenSize(void) {

    ReinitialiseRenderStuff();
    if (gMap_view == 2) {
        gRender_screen->base_x = gMap_render_x_i;
        gRender_screen->base_y = gMap_render_y_i;
        gRender_screen->width = gMap_render_width_i;
        gRender_screen->height = gMap_render_height_i;
    } else {
        gRender_screen->base_x = gProgram_state.current_render_left;
        gRender_screen->base_y = gProgram_state.current_render_top;
        gRender_screen->height = gProgram_state.current_render_bottom - gProgram_state.current_render_top;
        gRender_screen->width = gProgram_state.current_render_right - gProgram_state.current_render_left;
    }
    if (gRender_screen->width != gRender_screen->row_bytes) {
        gRender_screen->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
    } else {
        gRender_screen->flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    gRender_screen->origin_x = gRender_screen->width / 2;
    gRender_screen->origin_y = gRender_screen->height / 2;
    gWidth = gRender_screen->width;
    gHeight = gRender_screen->height;
    ReinitialiseForwardCamera();
    SetTintedPolySize(gHud_tinted1, gRender_screen->base_x, gRender_screen->base_y, gRender_screen->width, gRender_screen->height);
    SetTintedPolySize(gHud_tinted2, gRender_screen->base_x, gRender_screen->base_y, gRender_screen->width, gRender_screen->height);
    SetTintedPolySize(gHud_tinted4, gRender_screen->base_x, gRender_screen->base_y, gRender_screen->width, gRender_screen->height);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4cd0
void C2_HOOK_FASTCALL ScreenSmaller(void) {

    if (gMap_view != 2) {
        gRender_indent++;
        if (gRender_indent > 8) {
            gRender_indent = 8;
        }
        AdjustRenderScreenSize();
    }
}

// FUNCTION: CARMA2_HW 0x004e4d00
void C2_HOOK_FASTCALL ScreenLarger(void) {

    if (gMap_view != 2) {
        gRender_indent--;
        if (gRender_indent < 0) {
            gRender_indent = 0;
        }
        AdjustRenderScreenSize();
    }
}

// FUNCTION: CARMA2_HW 0x004e4d30
void C2_HOOK_FASTCALL ClearWobbles(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); ++i) {
        gWobble_array[i].time_started = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e4d50
void C2_HOOK_FASTCALL InitWobbleStuff(void) {
    int i;

    ClearWobbles();
    for (i = 0; i < (int)REC2_ASIZE(gCosine_array); i++) {
        gCosine_array[i] = (float)cos((double)i / 64.0 * 3.141592653589793 / 2.0);
    }

    gEvalu = LoadPixelmap("Evalu01.PIX");
}

// NewScreenWobble

// FUNCTION: CARMA2_HW 0x004e4e20
void C2_HOOK_FASTCALL SetScreenWobble(int pX, int pY) {
    gScreen_wobble_x = pX;
    gScreen_wobble_y = pY;
}

// FUNCTION: CARMA2_HW 0x004e4e30
void C2_HOOK_FASTCALL ResetScreenWobble(void) {
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
}

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

extern int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);
extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);
extern void C2_HOOK_FASTCALL ProcessTintedPoly(int pTintedIndex);
extern void C2_HOOK_FASTCALL RenderTintedPolys(void);
extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);
extern void C2_HOOK_FASTCALL SetSkyColour(br_colour c);
extern int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);
extern void C2_HOOK_FASTCALL RenderLimbs(void);
extern br_pixelmap* gRender_palette;
extern int gIt_or_fox;
extern tSpecial_volume* gLast_camera_special_volume;
extern tNet_game_player_info gNet_players[12];
extern tNet_mode gNet_mode;
extern int gNumber_of_net_players;
extern tDepth_effect_type gSwap_depth_effect_type;
extern void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);
extern void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world);
extern void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);
extern void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);
extern void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);
extern void C2_HOOK_FASTCALL RenderElectroBastardRays(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

// FUNCTION: CARMA2_HW 0x004e5680
void C2_HOOK_FASTCALL DoARenderPass(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, float pYon_factor, int pShadows, int pEffects, int pEffects_extra) {
    br_camera* camera_data = (br_camera*)pCamera->type_data;
    int bgnd_col;
    br_pixelmap* sky_pixelmap;
    tCar_spec* fox_car;
    int j;
    int i;
    int count;
    br_scalar original_yon = camera_data->yon_z;

    camera_data->yon_z *= pYon_factor;
    BrPixelmapFill(pDepth, 0xffffffff);
    if (!gAFE) {
        count = 0;
        if (gNet_mode != eNet_mode_none && (gCurrent_net_game->type == eNet_game_type_6 || gCurrent_net_game->type == eNet_game_type_foxy)) {
            BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
            for (i = 0; i < gNumber_of_net_players; i++) {
                tNet_game_player_info* net_player = &gNet_players[i];
                if ((net_player->field_0x80 || i == gIt_or_fox) && !IsCarCloaked(net_player->car)) {
                    fox_car = net_player->car;
                    if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                        br_actor** wheel_actors;

                        count += 1;
                        wheel_actors = fox_car->wheel_actors;
                        for (j = 0; j < 6; j++) {
                            if (wheel_actors[j] != NULL) {
                                wheel_actors[j]->render_style = BR_RSTYLE_NONE;
                            }
                        }
                        BrZbsSceneRenderAdd(fox_car->car_master_actor);
                    }
                }
            }
            BrZbsSceneRenderEnd();
            if (count != 0) {
                if (gHud_tinted4 == -1) {
                    gHud_tinted4 = CreateTintedPoly(0, 0, 640, 480, 3, 2, 0, 0);
                }
                TurnTintedPolyOn(gHud_tinted4);
                ProcessTintedPoly(gHud_tinted4);
                RenderTintedPolys();
                TurnTintedPolyOff(gHud_tinted4);
                BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
                for (i = 0; i < gNumber_of_net_players; i++) {
                    tNet_game_player_info* net_player = &gNet_players[i];
                    if (net_player->field_0x80 || i == gIt_or_fox) {
                        fox_car = net_player->car;
                        if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                            br_actor** wheel_actors;

                            wheel_actors = fox_car->wheel_actors;
                            for (j = 0; j < 6; j++) {
                                if (wheel_actors[j] != NULL) {
                                    wheel_actors[j]->render_style = BR_RSTYLE_DEFAULT;
                                    BrZbsSceneRenderAdd(wheel_actors[j]);
                                }
                            }
                        }
                    }
                }
                BrZbsSceneRenderEnd();
                fox_car->car_master_actor->render_style = BR_RSTYLE_NONE;
            }
        }
    }
    sky_pixelmap = (pColour->width == gBack_screen->width) ? gBack_screen : pColour;
    if (gProgram_state.current_depth_effect.sky_texture != NULL && (gLast_camera_special_volume == NULL || gLast_camera_special_volume->sky_col <= -1)) {
        BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
        DepthEffectSky(pColour, pDepth, pCamera, pMat34);
        BrZbsSceneRenderEnd();
    } else {
        if (gProgram_state.current_depth_effect.type == eDepth_effect_fog || gSwap_depth_effect_type == eDepth_effect_fog) {
            bgnd_col = 0xff;
        } else if (gProgram_state.current_depth_effect.type == eDepth_effect_darkness || gSwap_depth_effect_type == eDepth_effect_darkness) {
            bgnd_col = 0x00;
        } else if (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col >= 0) {
            bgnd_col = gLast_camera_special_volume->sky_col;
        } else {
            bgnd_col = 0x00;
        }
        if (gNet_mode != eNet_mode_none && (gCurrent_net_game->type == eNet_game_type_6 || gCurrent_net_game->type == eNet_game_type_foxy)) {
            SetSkyColour(((tU32*)gRender_palette->pixels)[bgnd_col]);
            BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
            DepthEffectSky(pColour, pDepth, pCamera, pMat34);
            BrZbsSceneRenderEnd();
        } else {
            if (sky_pixelmap->type != BR_PMT_INDEX_8) {
                bgnd_col = PaletteEntry16Bit(gRender_palette, bgnd_col);
                bgnd_col = (bgnd_col << 16) | bgnd_col;
            }
            BrPixelmapFill(sky_pixelmap, bgnd_col);
        }
    }
    DoSpecialCameraEffect(pCamera, pMat34);
    if (pShadows) {
        RenderShadows(gUniverse_actor, &gProgram_state.track_spec, pCamera, pMat34);
    }
    BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
    for (i = 4; i > 0; i--) {
        if (i != 2) {
            BrSetScreenZOffset(i);
            BrZbsSceneRenderAdd(gOther_selfs[i - 1]);
        }
    }
    BrSetScreenZOffset(0);
    ProcessTrack(gUniverse_actor, &gProgram_state.track_spec, pCamera, pMat34);
    BrZbsSceneRenderAdd(gNon_track_actor);
    RenderLimbs();
    RenderLollipops(pColour, pDepth, pCamera, pMat34);
    if (pEffects) {
        RenderSparks(pColour, pDepth, pCamera, pMat34, gFrame_period);
        RenderSmoke(pColour, pDepth, pCamera, pMat34, gFrame_period);
        RenderElectroBastardRays(pColour, pDepth, pCamera, pMat34, gFrame_period);
    }
    BrZbsSceneRenderEnd();
    camera_data->yon_z = original_yon;
}

// DoACompleteRenderPass

// STUB: CARMA2_HW 0x004e4e40
void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e5a70
void C2_HOOK_FASTCALL StartRenderingHeadups(void) {
    CleanPolyFontDanglers();
    gHud_actor_storage_size = 0;
    gRender_poly_text = 0;
}

// FUNCTION: CARMA2_HW 0x004e5ad0
void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor) {
    if (gHud_actor_storage_size >= (int)REC2_ASIZE(gHud_actor_storage)) {
        BrFailure("Not enough HUD actor storage");
    }
    gHud_actor_storage[gHud_actor_storage_size] = pActor;
    gHud_actor_storage_size += 1;
}

// FUNCTION: CARMA2_HW 0x004e5b00
void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
    int original_origin_x;
    int original_origin_y;
    int original_base_x;
    int original_base_y;
    br_actor* actor;
    int i;

    if (gCount_polyfont_glyph_actors != 0) {
        BrActorAdd(gHUD_root, gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->prev == NULL) {
            BrActorAdd(gHUD_root, actor);
        }
    }
    original_origin_x = gRender_screen->origin_x;
    original_origin_y = gRender_screen->origin_y;
    original_base_x = gRender_screen->base_x;
    original_base_y = gRender_screen->base_y;
    gRender_screen->origin_x = 0;
    gRender_screen->origin_y = 0;
    gRender_screen->base_x = 0;
    gRender_screen->base_y = 0;
    BrZbSceneRender(gHUD_root, gHUD_camera, gRender_screen, gDepth_buffer);
    gRender_screen->origin_x = original_origin_x;
    gRender_screen->origin_y = original_origin_y;
    gRender_screen->base_x = original_base_x;
    gRender_screen->base_y = original_base_y;
    if (gString_root_actor->parent != NULL) {
        BrActorRemove(gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gHud_actor_storage_size = 0;
    CleanPolyFontDanglers();
    gRender_poly_text = 1;
}

// FUNCTION: CARMA2_HW 0x004e5c70
void C2_HOOK_FASTCALL CleanPolyFontDanglers(void) {
    int i;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        br_actor* actor = gPolyfont_glyph_actors[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}

// GLOBAL: CARMA2_HW 0x0065fb54
int gHud_tinted4 = -1;

// GLOBAL: CARMA2_HW 0x006a27dc
int gShadow_clip_plane_count;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00445cb0
void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00446340
void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world) {
    NOT_IMPLEMENTED();
}

extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

// FUNCTION: CARMA2_HW 0x004e74d0
void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    int i;
    int cat;
    int car_count;
    tCar_spec* the_car;
    br_scalar distance_factor;
    br_vector3 camera_to_car;

    if (gShadow_level != eShadow_none) {
        for (cat = eVehicle_self; cat <= (gShadow_level == eShadow_everyone ? eVehicle_drone : gShadow_level == eShadow_us_and_opponents ? eVehicle_rozzer : eVehicle_self); cat++) {
            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {
                if (cat == eVehicle_self) {
                    the_car = &gProgram_state.current_car;
                } else {
                    the_car = GetCarSpec(cat, i);
                }
                if (!the_car->active) {
                    continue;
                }
                if (IsCarCloaked(the_car)) {
                    continue;
                }
                if (!gAction_replay_mode) {
                    BrVector3Sub(&camera_to_car, (br_vector3*)gCamera_to_world.m[3], &the_car->car_master_actor->t.t.translate.t);
                    distance_factor = BrVector3LengthSquared(&camera_to_car);
                    if (distance_factor > 36.0f) {
                        continue;
                    }
                }
                ProcessShadow(the_car, gUniverse_actor, &gProgram_state.track_spec, gCamera, &gCamera_to_world, distance_factor);
            }
        }
        if (gFancy_shadow) {
            for (i = 0; i < gSaved_table_count; i++) {
                gSaved_shade_tables[i].original->height = gSaved_shade_tables[i].copy->height;
                gSaved_shade_tables[i].original->pixels = gSaved_shade_tables[i].copy->pixels;
                BrTableUpdate(gSaved_shade_tables[i].original, BR_TABU_ALL);
            }
        }
    }
}

// STUB: CARMA2_HW 0x004fa910
void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004d5d60
void C2_HOOK_FASTCALL RenderElectroBastardRays(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a23e0
br_vector3 gShadow_points[8] = { { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } },
                                 { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } } };

// GLOBAL: CARMA2_HW 0x00655e00
br_matrix34 gIdentity34 = {
    {
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        { 0.0, 0.0, 0.0 },
    }
};

// GLOBAL: CARMA2_HW 0x006a2440
int gShadow_dim_amount;

