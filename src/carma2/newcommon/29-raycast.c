#include "29-raycast.h"

#include "globvars.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006a20d0
br_actor* gY_picking_camera;

// GLOBAL: CARMA2_HW 0x006a2120
br_scalar gHighest_y_below;

// GLOBAL: CARMA2_HW 0x0070518c
br_material* gMaterial_below;

// GLOBAL: CARMA2_HW 0x006a20e0
br_scalar gCurrent_y;

// GLOBAL: CARMA2_HW 0x006a20d8
int gBelow_face_index;

// GLOBAL: CARMA2_HW 0x006a20d4
br_model* gBelow_model;

// GLOBAL: CARMA2_HW 0x006a20dc
br_scalar gLowest_y_above;

// GLOBAL: CARMA2_HW 0x006a20e8
int gAbove_face_index;

// GLOBAL: CARMA2_HW 0x006a20e4
br_model* gAbove_model;

// GLOBAL: CARMA2_HW 0x006a20f0
// GLOBAL: CARMA2_SW 0x0069c148
br_matrix34 gPick_model_to_view__raycast;

// DRActorToRoot

// FUNCTION: CARMA2_HW 0x004e3660
void C2_HOOK_FASTCALL InitRayCasting(void) {
    br_camera* camera_ptr;

    gY_picking_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera_ptr = gY_picking_camera->type_data;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->field_of_view = BrDegreeToAngle(70.0f);
    camera_ptr->hither_z = 0.001f;
    camera_ptr->yon_z = 1000.0f;
    camera_ptr->aspect = 1.0f;
    gY_picking_camera->t.t.mat.m[0][0] =  1.0f;
    gY_picking_camera->t.t.mat.m[0][1] =  0.0f;
    gY_picking_camera->t.t.mat.m[0][2] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][0] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][1] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][2] = -1.0f;
    gY_picking_camera->t.t.mat.m[2][0] =  0.0f;
    gY_picking_camera->t.t.mat.m[2][1] =  1.0f;
    gY_picking_camera->t.t.mat.m[2][2] =  0.0f;
}

// BadDiv

// DRVector2AccumulateScale

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e3a30
// FUNCTION: CARMA2_SW 0x004dfdc0
int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] > 2.384186e-07f) {
            s = (rp->v[i] - b->max.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (rp->v[i] - b->min.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] < -2.384186e-07f) {
            s = (rp->v[i] - b->max.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (s < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (s < t_far) {
                t_far = s;
            }
            t = (rp->v[i] - b->min.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else if (rp->v[i] > b->max.v[i] || rp->v[i] < b->min.v[i]) {
            return 0;
        }
    }
    if (t_near <= t_far) {
        *new_t_near = t_near;
        *new_t_far = t_far;
        return 1;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e3840
// FUNCTION: CARMA2_SW 0x004dfbd0
int C2_HOOK_FASTCALL ActorPick2D(br_actor* pActor, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback, void* pArg) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_matrix34 m_to_v;
    br_matrix34 v_to_m;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_vector3 dir;

    r = 0;
    if (pActor->model != NULL) {
        this_model = pActor->model;
    } else {
        this_model = pModel;
    }
    if (pActor->material != NULL) {
        this_material = pActor->material;
    } else {
        this_material = pMaterial;
    }
    if (pActor->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    m_to_v = gPick_model_to_view__raycast;
    BrMatrix34PreTransform(&gPick_model_to_view__raycast, &pActor->t);
    switch (pActor->type) {
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
        if (PickBoundsTestRay__raycast(pActor->type_data, (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2], 0.0f, BR_SCALAR_MAX, &t_near, &t_far)) {
            for (a = pActor->children; a != NULL; a = a->next) {
                r = ActorPick2D(a, this_model, this_material, pCallback, pArg);
                if (r != 0) {
                    break;
                }
            }
        }
        break;
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
            if (PickBoundsTestRay__raycast(&this_model->bounds, (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2], 0.0f, BR_SCALAR_MAX, &t_near, &t_far)) {
                BrVector3Negate(&dir, (br_vector3*)v_to_m.m[2]);
                r = pCallback(pActor, this_model, this_material, (br_vector3*)v_to_m.m[3], &dir, t_near, t_far, pArg);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
    default:
        for (a = pActor->children; a != NULL; a = a->next) {
            r = ActorPick2D(a, this_model, this_material, pCallback, pArg);
            if (r != 0) {
                break;
            }
        }
        break;
    }
    gPick_model_to_view__raycast = m_to_v;
    return r;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004e36f0
int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DRScenePick2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/raycast.c:254
// FUNCTION: CARMA2_HW 0x004e3d90
// FUNCTION: CARMA2_SW 0x004e0120
int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* model, br_material* material, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_raycast_cbfn* callback, void* arg) {
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    int group;
    br_scalar t;
    br_scalar d;
    br_vector3 p;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    br_scalar v0i1;
    br_scalar v0i2;
    float alpha;
    float beta;
    float f_d;
    float f_n;
    br_scalar s_alpha;
    br_scalar s_beta;
    br_vector2 map;
    int v;
    int e;
    int r;
    br_material* this_material;
    br_scalar numerator;
    v11face* v11f;
    v11group* v11g;
    br_vector4* eqn;

    t_near -= 0.001f;
    t_far += 0.001f;
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        v11g = &V11MODEL(model)->groups[group];
        for (f = 0; f < v11g->nfaces; f++) {
            v11f = &v11g->faces[f];
            eqn = &v11f->eqn;
            if (v11g->face_colours.materials[0] != NULL) {
                this_material = v11g->face_colours.materials[0];
            } else {
                this_material = material;
            }
            d = BrVector3Dot(eqn, ray_dir);
            if ((fabs(d) >= 2.3841858e-7f && this_material != NULL && (this_material->flags & (BR_MATF_TWO_SIDED | BR_MATF_ALWAYS_VISIBLE)) != 0) || d <= 0.0) {
                numerator = BrVector3Dot(eqn, ray_pos) - eqn->v[3];
                if (!(fabs(d) < 1.0f && fabs(numerator) > fabs(d) * BR_SCALAR_MAX)) {
                    t = -(numerator / d);
                    if (t >= t_near && t <= t_far) {
                        BrVector3Scale(&p, ray_dir, t);
                        BrVector3Accumulate(&p, ray_pos);
                        axis_m = (fabs(eqn->v[1]) > fabs(eqn->v[0])) ? 1 : 0;
                        if (fabs(eqn->v[2]) > fabs(eqn->v[axis_m])) {
                            axis_m = 2;
                        }
                        if (axis_m == 0) {
                            axis_0 = 1;
                            axis_1 = 2;
                        } else if (axis_m == 1) {
                            axis_0 = 0;
                            axis_1 = 2;
                        } else if (axis_m == 2) {
                            axis_0 = 0;
                            axis_1 = 1;
                        }

                        v0 = v11g->vertices[v11f->vertices[0]].p.v[axis_0];
                        u0 = v11g->vertices[v11f->vertices[0]].p.v[axis_1];

                        v1 = v11g->vertices[v11f->vertices[1]].p.v[axis_0] - v0;
                        u1 = v11g->vertices[v11f->vertices[1]].p.v[axis_1] - u0;
                        v2 = v11g->vertices[v11f->vertices[2]].p.v[axis_0] - v0;
                        u2 = v11g->vertices[v11f->vertices[2]].p.v[axis_1] - u0;

                        v0i1 = p.v[axis_0] - v0;
                        v0i2 = p.v[axis_1] - u0;
                        if (fabs(v1) > 2.3841858e-7f) {
                            f_d = v0i2 * v1 - u1 * v0i1;
                            f_n = u2 * v1 - u1 * v2;
                            if (f_n == 0.) {
                                continue;
                            }
                            beta = f_d / f_n;
                            alpha = (v0i1 - beta * v2) / v1;
                        } else {
                            beta = v0i1 / v2;
                            alpha = (v0i2 - beta * u2) / u1;
                        }

                        if (alpha >= 0.0 && beta >= 0.0 && beta + alpha <= 1.0) {
                            s_alpha = alpha;
                            s_beta = beta;
                            BrVector2Scale(&map, &v11g->vertices[v11f->vertices[1]].map, s_alpha);
                            {
                                br_vector2* vm2 = &v11g->vertices[v11f->vertices[2]].map;
                                map.v[0] = vm2->v[0] * s_beta + map.v[0];
                                map.v[1] = vm2->v[1] * s_beta + map.v[1];
                            }
                            {
                                br_vector2* vm0 = &v11g->vertices[v11f->vertices[0]].map;
                                map.v[0] = vm0->v[0] * (1.0f - (s_alpha + s_beta)) + map.v[0];
                                map.v[1] = vm0->v[1] * (1.0f - (s_alpha + s_beta)) + map.v[1];
                            }
                            v = 0;
                            e = 1;
                            if (s_alpha <= s_beta) {
                                if (0.5f - s_beta / 2.0f > s_alpha) {
                                    e = 0;
                                }
                                if (1.0f - s_beta * 2.0f < s_alpha) {
                                    v = 1;
                                }
                            } else {
                                if (1.0f - s_beta * 2.0f > s_alpha) {
                                    e = 2;
                                }
                                if (0.5f - s_beta / 2.0f < s_alpha) {
                                    v = 2;
                                }
                            }
                            r = callback(model, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
                            if (r != 0) {
                                return r;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4300
// FUNCTION: CARMA2_SW 0x004e0690
int C2_HOOK_CDECL FindHighestPolyCallBack__raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pArg) {
    br_scalar the_y;

    the_y = pPoint->v[1];
    if (the_y <= gCurrent_y) {
        if (the_y > gHighest_y_below) {
            gHighest_y_below = the_y;
            gBelow_face_index = pF;
            gBelow_model = pModel;
        }
    } else {
        if (the_y < gLowest_y_above) {
            gLowest_y_above = the_y;
            gAbove_face_index = pF;
            gAbove_model = pModel;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e3d50
// FUNCTION: CARMA2_SW 0x004e00e0
int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__raycast, pArg);
    }
    return 0;
}

// FindBestY

// FUNCTION: CARMA2_HW 0x004e45b0
// FUNCTION: CARMA2_SW 0x004e0940
int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext) {
    float y;

    if (pMaterial == NULL || pMaterial->identifier == NULL
            || (pMaterial->identifier[0] != '!' && pMaterial->identifier[0] != '>')) {
        y = pPoint->v[1];
        if (y > gHighest_y_below) {
            gHighest_y_below = y;
            gMaterial_below = pMaterial;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e4570
// FUNCTION: CARMA2_SW 0x004e0900
int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindYVerticallyBelowPolyCallBack, pArg);
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/raycast.c:425
// FUNCTION: CARMA2_HW 0x004e4370
// FUNCTION: CARMA2_SW 0x004e0700
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition) {
    tU8 cx;
    tU8 cz;
    tU8 x;
    tU8 z;
    tTrack_spec* track_spec;
    br_actor* world;
    br_actor* camera;
    extern br_material* gBlack_material;
    extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);
    br_matrix34 camera_tfm;
    br_camera* camera_data;
    br_angle view_over_2;
    br_scalar aspect;
    br_scalar angle;
    br_scalar cos_angle;
    br_scalar sin_angle;
    br_scalar scale;

    track_spec = &gProgram_state.track_spec;
    XZToColumnXZ(&cx, &cz, pPosition->v[0], pPosition->v[2], track_spec);
    gHighest_y_below = BR_SCALAR_MIN;
    gY_picking_camera->t.t.translate.t = *pPosition;
    for (x = MAX(0, cx - 1); x < MIN(track_spec->ncolumns_x, cx + 2); x++) {
        for (z = MAX(0, cz - 1); z < MIN(track_spec->ncolumns_z, cz + 2); z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                world = track_spec->columns[z][x].actor_0x0;
                camera = gY_picking_camera;
                camera_data = (br_camera*)camera->type_data;
                if (camera == world) {
                    BrMatrix34Identity(&camera_tfm);
                } else {
                    BrTransformToMatrix34(&camera_tfm, &camera->t);
                    for (camera = camera->parent; camera != NULL && camera != world; camera = camera->parent) {
                        if (camera->t.type != BR_TRANSFORM_IDENTITY) {
                            BrMatrix34PostTransform(&camera_tfm, &camera->t);
                        }
                    }
                }
                BrMatrix34Inverse(&gPick_model_to_view__raycast, &camera_tfm);
                view_over_2 = camera_data->field_of_view / 2;
                aspect = camera_data->aspect;
                angle = BrAngleToRadian(view_over_2);
                cos_angle = cosf(angle);
                sin_angle = sinf(angle);
                scale = cos_angle / sin_angle;
                BrMatrix34PostScale(&gPick_model_to_view__raycast, scale / aspect, scale, 1.0f);
                ActorPick2D(world, NULL, gBlack_material, FindYVerticallyBelowCallBack, NULL);
            }
        }
    }
    return gHighest_y_below;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4600
// FUNCTION: CARMA2_SW 0x004e0990
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point) {
    br_scalar result;
    int number_of_attempts;
    br_vector3 cast_point;

    number_of_attempts = 0;
    BrVector3Copy(&cast_point, pCast_point);
    do {
        result = FindYVerticallyBelow(&cast_point);
        cast_point.v[1] += .2f;
    } while (result < -100.f && number_of_attempts++ < 10);
    return result;
}

