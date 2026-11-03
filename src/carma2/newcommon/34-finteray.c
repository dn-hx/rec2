#include "34-finteray.h"

#include "rec2_macros.h"

#include <math.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x005964c0
int gPling_materials = 1;

// GLOBAL: CARMA2_HW 0x00686190
br_scalar gNearest_T;

// GLOBAL: CARMA2_HW 0x006861c8
int gTemp_group;

// GLOBAL: CARMA2_HW 0x006861cc
br_actor* gNearest_actor;

// GLOBAL: CARMA2_HW 0x00686188
int gNearest_face;

// GLOBAL: CARMA2_HW 0x00686194
int gNearest_face_group;

// GLOBAL: CARMA2_HW 0x0068618c
br_model* gNearest_model;

// GLOBAL: CARMA2_HW 0x00686198
br_matrix34 gPick_model_to_view__finteray;

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0045cb70
int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_scalar t_far;
    br_scalar t_near;
    int r;
    br_vector3 dir;
    br_vector3 pos;
    br_matrix34 invmat;
    br_matrix34 mat;

    t_near = 0.f;
    t_far = 1.f;
    r = 0;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        BrTransformToMatrix34(&mat, &ap->t);
        BrMatrix34Inverse(&invmat, &mat);
        BrMatrix34ApplyP(&pos, pPosition, &invmat);
        BrMatrix34ApplyV(&dir, pDir, &invmat);
        pPosition = &pos;
        pDir = &dir;
    }
    switch (ap->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            if (PickBoundsTestRay__finteray(&this_model->bounds, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
                t_near = 0.f;
                if (gNearest_T > 1.f) {
                    t_far = 1.f;
                } else {
                    t_far = gNearest_T;
                }
                r = callback(ap, this_model, this_material, pPosition, pDir, t_near, t_far, NULL);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
        /* fall through */
    default:
        for (a = ap->children; a != NULL; a = a->next) {
            r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
            if (r) {
                break;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        if (PickBoundsTestRay__finteray(ap->type_data, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
            for (a = ap->children; a != NULL; a = a->next) {
                r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
                if (r) {
                    break;
                }
            }
        }
        break;
    }
    return r;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DRSceneRayPick2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045cf60
int C2_HOOK_FASTCALL DRModelPick2D(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg) {
    DR_FACE* fp;
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    br_scalar t;
    br_scalar d;
    br_vector3 p;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    float du;
    float dv;
    float alpha;
    float beta;
    float f_d;
    float f_n;
    float weight;
    br_vector2 map;
    int v;
    int e;
    int r;
    br_material* this_material;
    br_scalar abs_d;
    br_scalar numerator;
    int group;

    t_near -= 1e-5f;
    t_far -= -1e-5f;
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        fp = V11MODEL(model)->groups[group].faces;
        for (f = 0; f < V11MODEL(model)->groups[group].nfaces; f++, fp++) {
            this_material = *V11MODEL(model)->groups[group].face_colours.materials;
            if (!this_material) {
                this_material = material;
            }
            d = fp->eqn.v[0] * ray_dir->v[0] + fp->eqn.v[1] * ray_dir->v[1] + fp->eqn.v[2] * ray_dir->v[2];
            abs_d = (float)fabs(d);
            if (abs_d < 2.3841858e-7f) {
                continue;
            }
            if (this_material && this_material->identifier
                    && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?')
                    && gPling_materials) {
                continue;
            }
            if (this_material && (this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.0f) {
                continue;
            }
            numerator = fp->eqn.v[0] * ray_pos->v[0] + fp->eqn.v[2] * ray_pos->v[2] + fp->eqn.v[1] * ray_pos->v[1] - fp->eqn.v[3];
            if (abs_d < 1.0f) {
                if (abs_d * BR_SCALAR_MAX < (float)fabs(numerator)) {
                    continue;
                }
            }
            t = -(numerator / d);
            if (t < t_near || t > t_far) {
                continue;
            }
            p.v[0] = ray_dir->v[0] * t + ray_pos->v[0];
            p.v[1] = ray_dir->v[1] * t + ray_pos->v[1];
            p.v[2] = ray_dir->v[2] * t + ray_pos->v[2];
            axis_m = (float)fabs(fp->eqn.v[0]) < (float)fabs(fp->eqn.v[1]);
            if ((float)fabs(fp->eqn.v[2]) > (float)fabs(fp->eqn.v[axis_m])) {
                axis_m = 2;
            }
            switch (axis_m) {
            case 0:
                axis_0 = 1;
                axis_1 = 2;
                break;
            case 1:
                axis_0 = 0;
                axis_1 = 2;
                break;
            case 2:
                axis_0 = 0;
                axis_1 = 1;
                break;
            }
            v0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_0];
            u0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_1];
            v1 = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_0] - v0;
            u1 = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_1] - u0;
            v2 = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_0] - v0;
            u2 = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_1] - u0;
            du = p.v[axis_1] - u0;
            dv = p.v[axis_0] - v0;
            f_n = u2 * v1 - u1 * v2;
            f_d = du * v1 - u1 * dv;
            if ((float)fabs(f_n) < (float)fabs(f_d)) {
                continue;
            }
            if (f_n == 0.0f) {
                continue;
            }
            beta = f_d / f_n;
            if (beta < 0.0f) {
                continue;
            }
            f_d = du * v2 - u2 * dv;
            f_n = u1 * v2 - u2 * v1;
            if ((float)fabs(f_n) < (float)fabs(f_d)) {
                continue;
            }
            if (f_n == 0.0f) {
                continue;
            }
            alpha = f_d / f_n;
            if (alpha < 0.0f) {
                continue;
            }
            if (alpha + beta > 1.0f) {
                continue;
            }
            map.v[0] = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[0] * alpha;
            map.v[1] = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[1] * alpha;
            map.v[0] = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].map.v[0] * beta + map.v[0];
            map.v[1] = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].map.v[1] * beta + map.v[1];
            weight = 1.0f - (alpha + beta);
            map.v[0] = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].map.v[0] * weight + map.v[0];
            map.v[1] = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].map.v[1] * weight + map.v[1];
            v = 0;
            e = 1;
            if (alpha >= beta) {
                if (alpha < 1.0f - beta * 2.0f) {
                    e = 2;
                } else {
                    e = 1;
                }
                if (alpha < 0.5f - beta * 0.5f) {
                    v = 0;
                } else {
                    v = 2;
                }
            } else {
                if (alpha < 0.5f - beta * 0.5f) {
                    e = 0;
                } else {
                    e = 1;
                }
                if (alpha < 1.0f - beta * 2.0f) {
                    v = 0;
                } else {
                    v = 1;
                }
            }
            gTemp_group = group;
            r = callback(model, actor, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
            if (r != 0) {
                return r;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045d570
int C2_HOOK_CDECL FindHighestPolyCallBack(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData) {

    if (pT < gNearest_T) {
        gNearest_T = pT;
        gNearest_model = pModel;
        gNearest_actor = pActor;
        gNearest_face = pF;
        gNearest_face_group = gTemp_group;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045cf20
int C2_HOOK_CDECL FindHighestCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    DRModelPick2D(pModel, pMaterial, pActor, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack, pArg);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045ca60
void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* pNormal, br_scalar* pT, br_material** pMaterial, br_actor** pActor) {
    int group;

    gNearest_T = 100.f;
    BrMatrix34Inverse(&gPick_model_to_view__finteray, &pWorld->t.t.mat);
    ActorRayPick2D(pWorld, pPosition, pDir, NULL, NULL, FindHighestCallBack);
    *pT = gNearest_T;
    if (gNearest_T < 100.f) {
        group = gNearest_face_group;
        BrVector3Copy(pNormal, &V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn);
        *pMaterial = *V11MODEL(gNearest_model)->groups[group].face_colours.materials;
        if (pActor != NULL) {
            *pActor = gNearest_actor;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045cd90
int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] < -2.3841858e-7f) {
            s = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] > 2.3841858e-7f) {
            s = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->max.v[i]);
            if (s < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (s < t_far) {
                t_far = s;
            }
            t = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->min.v[i]);
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

// FUNCTION: CARMA2_HW 0x0045d5c0
void C2_HOOK_FASTCALL EnablePlingMaterials(void) {

    gPling_materials = 1;
}

// FUNCTION: CARMA2_HW 0x0045d5d0
void C2_HOOK_FASTCALL DisablePlingMaterials(void) {

    gPling_materials = 0;
}

// StationarySphereSphereCollide

// SphereSphereCollide

// FUNCTION: CARMA2_HW 0x0045dea0
void C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_scalar fVar1;
    br_scalar t;

    t = ((pSphere_end->v[2] - pEdge_start->v[2]) * pEdge_direction->v[2] + pEdge_direction->v[1] * (pSphere_end->v[1] - pEdge_start->v[1]) + (pSphere_end->v[0] - pEdge_start->v[0]) * pEdge_direction->v[0]) / pEdge_length_squared;
    if (0.0f < t) {
        if (t < 1.0f) {
            pPoint->v[0] = pEdge_direction->v[0] * t;
            pPoint->v[1] = pEdge_direction->v[1] * t;
            pPoint->v[2] = pEdge_direction->v[2] * t;
            pPoint->v[0] = pPoint->v[0] + pEdge_start->v[0];
            pPoint->v[1] = pEdge_start->v[1] + pPoint->v[1];
            pPoint->v[2] = pEdge_start->v[2] + pPoint->v[2];
        } else {
            pPoint->v[0] = pEdge_end->v[0];
            pPoint->v[1] = pEdge_end->v[1];
            pPoint->v[2] = pEdge_end->v[2];
        }
    } else {
        pPoint->v[0] = pEdge_start->v[0];
        pPoint->v[1] = pEdge_start->v[1];
        pPoint->v[2] = pEdge_start->v[2];
    }
    pNormal->v[0] = pPoint->v[0] - pSphere_end->v[0];
    pNormal->v[1] = pPoint->v[1] - pSphere_end->v[1];
    fVar1 = pPoint->v[2] - pSphere_end->v[2];
    pNormal->v[2] = fVar1;
    fVar1 = pNormal->v[0] * pNormal->v[0] + fVar1 * fVar1 + pNormal->v[1] * pNormal->v[1];
    if ((pSphere->radius_squared < fVar1) && (fVar1 < pSphere->radius_squared - pSphere->radius * 0.002 * -2.0)) {
        fVar1 = sqrt(fVar1);
        *pT = 1.0f;
        pNormal->v[0] = pNormal->v[0] / fVar1;
        pNormal->v[1] = pNormal->v[1] / fVar1;
        pNormal->v[2] = pNormal->v[2] / fVar1;
    }
}


// FUNCTION: CARMA2_HW 0x0045d860
void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pPoint, br_vector3* pSphere_center, br_scalar pRadius, br_vector3* pNormal, br_scalar* pCollision_t) {
    br_scalar dx;
    br_scalar dy;
    br_scalar dz;
    volatile br_scalar distance_squared;
    br_scalar radius_squared;
    br_scalar scale;

    dx = pPoint->v[0] - pSphere_center->v[0];
    dy = pPoint->v[1] - pSphere_center->v[1];
    dz = pPoint->v[2] - pSphere_center->v[2];
    distance_squared = dx * dx + dy * dy + dz * dz;
    if ((pRadius - -0.002f) * (pRadius - -0.002f) > distance_squared) {
        radius_squared = pRadius * pRadius;
        if (radius_squared < distance_squared) {
            scale = (br_scalar)(1.0 / sqrt(distance_squared));
            pNormal->v[0] = scale * dx;
            pNormal->v[1] = scale * dy;
            pNormal->v[2] = scale * dz;
            *pCollision_t = 1.0f;
        }
    }
}

extern void C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);
extern void C2_HOOK_FASTCALL SpherePolyFaceCollide(br_vector3* pEnd, br_vector3* pStart, int pUnused, br_vector3* pDirection, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

// FUNCTION: CARMA2_HW 0x0045d920
void C2_HOOK_FASTCALL SphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT, tU32 pFlags_a, tU8 pFlags_b) {
    br_scalar fVar5;
    br_scalar fVar4;
    br_scalar fVar3;
    br_scalar fVar2;
    br_scalar fVar1;
    br_scalar local_4;
    br_scalar local_8;
    br_scalar local_c;
    br_scalar local_10;
    br_scalar local_14;
    br_scalar local_18;
    br_scalar local_1c;
    br_scalar local_20;
    br_scalar local_24;
    br_scalar local_28;
    br_scalar local_2c;
    br_scalar local_30;
    br_scalar local_34;
    br_scalar local_38;
    br_scalar local_3c;
    br_scalar local_40;
    br_scalar local_44;
    br_scalar local_48;

    *pT = 100.0f;
    local_48 = pEdge_end->v[0] - pEdge_start->v[0];
    local_44 = pEdge_end->v[1] - pEdge_start->v[1];
    local_18 = pEnd->v[0] - pStart->v[0];
    local_14 = pEnd->v[1] - pStart->v[1];
    local_40 = pEdge_end->v[2] - pEdge_start->v[2];
    local_10 = pEnd->v[2] - pStart->v[2];
    fVar2 = local_40 * local_40 + local_44 * local_44 + local_48 * local_48;
    local_4 = (local_10 * local_40 + local_14 * local_44 + local_18 * local_48) / fVar2;
    local_c = local_48 * local_4;
    local_3c = local_18 - local_c;
    local_8 = local_44 * local_4;
    local_38 = local_14 - local_8;
    local_4 = local_40 * local_4;
    local_24 = pEdge_start->v[0] - pStart->v[0];
    local_34 = local_10 - local_4;
    local_20 = pEdge_start->v[1] - pStart->v[1];
    local_1c = pEdge_start->v[2] - pStart->v[2];
    fVar3 = local_34 * local_34 + local_38 * local_38 + local_3c * local_3c;
    fVar1 = -((local_1c * local_40 + local_20 * local_44 + local_24 * local_48) / fVar2);
    local_30 = local_48 * fVar1 + local_24;
    local_2c = local_44 * fVar1 + local_20;
    local_28 = local_40 * fVar1 + local_1c;
    if (fVar3 < 1e-06) {
        StationarySphereEdgeCollide(pEdge_start, pEdge_end, (br_vector3*)&local_48, *(br_scalar*)&pT, pEnd, pSphere, pPoint, pNormal, pT);
        return;
    }
    fVar1 = (local_28 * local_34 + local_2c * local_38 + local_30 * local_3c) / fVar3;
    local_3c = local_3c * fVar1;
    local_38 = local_38 * fVar1;
    fVar4 = (local_28 * local_28 + local_2c * local_2c + local_30 * local_30) -
            (local_34 * fVar1 * local_34 * fVar1 + local_38 * local_38 + local_3c * local_3c);
    if (pSphere->radius_squared - pSphere->radius * 0.002 * -2.0 < fVar4) {
        return;
    }
    if (pSphere->radius_squared < fVar4) {
        goto LAB_0045de75;
    }
    fVar1 = fVar1 - sqrt((pSphere->radius_squared - fVar4) / fVar3);
    local_c = (local_c * fVar1 + local_30) - local_24;
    local_8 = (local_8 * fVar1 + local_2c) - local_20;
    local_4 = (local_4 * fVar1 + local_28) - local_1c;
    fVar3 = (local_4 * local_40 + local_8 * local_44 + local_c * local_48) / fVar2;
    if (0.0 <= fVar3) {
        if (fVar3 <= 1.0) {
            if ((0.0 <= fVar1) && (fVar1 <= 1.0)) {
                pPoint->v[0] = pEdge_start->v[0] + local_c;
                pPoint->v[1] = local_8 + pEdge_start->v[1];
                fVar2 = pEdge_start->v[2];
                pPoint->v[2] = local_4 + fVar2;
                fVar4 = pPoint->v[0] - (pStart->v[0] + (pEnd->v[0] - pStart->v[0]) * fVar1);
                fVar3 = pPoint->v[1] - ((pEnd->v[1] - pStart->v[1]) * fVar1 + pStart->v[1]);
                fVar5 = (local_4 + fVar2) - ((pEnd->v[2] - pStart->v[2]) * fVar1 + pStart->v[2]);
                fVar2 = sqrt(fVar5 * fVar5 + fVar3 * fVar3 + fVar4 * fVar4);
                if (fVar2 <= 2.3841858e-07) {
                    pNormal->v[0] = 1.0f;
                    pNormal->v[1] = 0.0f;
                    pNormal->v[2] = 0.0f;
                    *pT = fVar1;
                    return;
                }
                fVar2 = 1.0 / fVar2;
                pNormal->v[0] = fVar2 * fVar4;
                pNormal->v[1] = fVar2 * fVar3;
                pNormal->v[2] = fVar2 * fVar5;
                *pT = fVar1;
                return;
            }
            goto LAB_0045de75;
        }
        if ((pFlags_b & 0x80) == 0) {
            SpherePolyFaceCollide(pEnd, pStart, 0, (br_vector3*)&local_18, pSphere, pPoint, pNormal, pT);
        }
LAB_0045dd10:
        fVar1 = *pT;
    } else {
        if ((pFlags_a & 0x80) != 0) {
            goto LAB_0045dd10;
        }
        SpherePolyFaceCollide(pEnd, pStart, 0, (br_vector3*)&local_18, pSphere, pPoint, pNormal, pT);
        fVar1 = *pT;
    }
    if (fVar1 < 2.0) {
        return;
    }
LAB_0045de75:
    StationarySphereEdgeCollide(pEdge_start, pEdge_end, (br_vector3*)&local_48, *(br_scalar*)&pT, pEnd, pSphere, pPoint, pNormal, pT);
}

// SpherePolyFaceCollide

// SphereBoxFaceCollide

#define ABS(a) (((a) < 0) ? -(a) : (a))

// FUNCTION: CARMA2_HW 0x0045e850
void C2_HOOK_FASTCALL SphereFaceCollide(tFace_ref* pFace, br_vector3* pStart, br_vector3* pEnd, br_scalar pRadius, void* pUnused_point, br_vector3* pNormal, br_scalar* pT) {
    br_scalar fVar1;
    br_scalar fVar2;
    char cVar3;
    br_material* pMat;
    br_scalar fVar5;
    br_scalar fVar6;
    br_scalar fVar7;
    br_scalar fVar8;
    br_scalar fVar9;
    br_scalar fVar11;
    int uVar10;
    int iAxis_b;
    int iAxis_a;
    br_scalar local_18[6];

    *pT = 100.0f;
    pMat = pFace->material;
    if ((pMat == 0) || (pMat->identifier == 0) ||
        ((cVar3 = *pMat->identifier, cVar3 != '!') && (cVar3 != '>') && (cVar3 != '?'))) {
        fVar9 = (pStart->v[2] * pFace->normal.v[2] + pStart->v[1] * pFace->normal.v[1] + pFace->normal.v[0] * pStart->v[0]) - pFace->d;
        if (0.0 <= fVar9) {
            if (fVar9 < pRadius) {
                return;
            }
            local_18[2] = pFace->normal.v[2];
            local_18[1] = pFace->normal.v[1];
            local_18[0] = pFace->normal.v[0];
            fVar11 = ((pFace->normal.v[0] * pEnd->v[0] + pEnd->v[1] * pFace->normal.v[1]) + pEnd->v[2] * local_18[2]) - pFace->d;
        } else {
            if (-pRadius < fVar9) {
                return;
            }
            if (pMat == 0) {
                return;
            }
            if ((pMat->flags & 0x1000) == 0) {
                return;
            }
            local_18[2] = -pFace->normal.v[2];
            local_18[1] = -pFace->normal.v[1];
            local_18[0] = -pFace->normal.v[0];
            fVar11 = -pFace->normal.v[0] * pEnd->v[0] + pEnd->v[1] * -pFace->normal.v[1] + pEnd->v[2] * local_18[2] + pFace->d;
        }
        if (fVar11 <= pRadius - -0.002) {
            pRadius = (pRadius - fVar9) / (fVar11 - fVar9);
            if (1.0 < pRadius) {
                pRadius = 1.0;
            }
            if (pRadius < 0.0) {
                pRadius = 0.0;
            }
            fVar11 = (pEnd->v[0] - pStart->v[0]) * pRadius + pStart->v[0];
            fVar9 = (pEnd->v[1] - pStart->v[1]) * pRadius + pStart->v[1];
            fVar1 = (pEnd->v[2] - pStart->v[2]) * pRadius + pStart->v[2];
            fVar2 = (pFace->v[0].v[2] - fVar1) * local_18[2] + (pFace->v[0].v[1] - fVar9) * local_18[1] + (pFace->v[0].v[0] - fVar11) * local_18[0];
            local_18[3] = local_18[0] * fVar2 + fVar11;
            local_18[4] = local_18[1] * fVar2 + fVar9;
            local_18[5] = local_18[2] * fVar2 + fVar1;
            uVar10 = (int)(ABS(local_18[0]) < ABS(local_18[1]));
            if (ABS(local_18[uVar10]) < ABS(local_18[2])) {
                uVar10 = 2;
            }
            if (uVar10 == 0) {
                iAxis_b = 1;
                iAxis_a = 2;
            } else if (uVar10 == 1) {
                iAxis_b = 0;
                iAxis_a = 2;
            } else {
                iAxis_b = 0;
                iAxis_a = 1;
            }
            fVar1 = pFace->v[0].v[iAxis_a];
            fVar2 = pFace->v[0].v[iAxis_b];
            fVar5 = (pFace->v[2].v[iAxis_a] - fVar1) * (pFace->v[1].v[iAxis_b] - fVar2);
            fVar8 = (pFace->v[1].v[iAxis_a] - fVar1) * (pFace->v[2].v[iAxis_b] - fVar2);
            fVar6 = fVar5 - fVar8;
            fVar7 = (local_18[iAxis_a + 3] - fVar1) * (pFace->v[1].v[iAxis_b] - fVar2) - (pFace->v[1].v[iAxis_a] - fVar1) * (local_18[iAxis_b + 3] - fVar2);
            if (((ABS(fVar7) <= ABS(fVar6) * 1.001) && (fVar6 != 0.0)) && (fVar7 = fVar7 / fVar6, -0.0001 <= fVar7)) {
                fVar8 = fVar8 - fVar5;
                fVar9 = (local_18[iAxis_a + 3] - fVar1) * (pFace->v[2].v[iAxis_b] - fVar2) - (pFace->v[2].v[iAxis_a] - fVar1) * (local_18[iAxis_b + 3] - fVar2);
                if (((ABS(fVar9) <= ABS(fVar8) * 1.001) && (fVar8 != 0.0)) &&
                    ((fVar9 = fVar9 / fVar8, -0.0001 <= fVar9) && (fVar7 + fVar9 <= 1.0001))) {
                    *pT = pRadius;
                    pNormal->v[1] = local_18[1];
                    pNormal->v[0] = local_18[0];
                    pNormal->v[2] = local_18[2];
                    return;
                }
            }
        }
    }
    return;
}


// (reference only)
// FUNCTION: CARMA2_HW 0x0045ecc0
void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos) {
    br_scalar d;
    br_scalar numerator;
    br_scalar t;
    br_vector3 p;
    int axis_m;
    int axis_0;
    int axis_1;
    float u0;
    float u1;
    float v0;
    float v1;
    float u2;
    float v2;
    float alpha;
    float f_d;
    float beta;
    br_material* this_material;

    this_material = pFace->material;
    *rt = 100.f;
    d = ray_dir->v[0] * pFace->normal.v[0] + ray_dir->v[2] * pFace->normal.v[2] + ray_dir->v[1] * pFace->normal.v[1];
    if (this_material != NULL && (this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
        return;
    }
    if (this_material != NULL && this_material->identifier != NULL
            && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?')
            && gPling_materials) {
        return;
    }
    if ((float)fabs(d) < 2.3841858e-7f) {
        return;
    }
    p.v[0] = ray_pos->v[0] - pFace->v[0].v[0];
    p.v[1] = ray_pos->v[1] - pFace->v[0].v[1];
    p.v[2] = ray_pos->v[2] - pFace->v[0].v[2];
    numerator = pFace->normal.v[0] * p.v[0] + pFace->normal.v[2] * p.v[2] + pFace->normal.v[1] * p.v[1];
    if ((float)fabs(d) < 1.f) {
        if ((float)fabs(d) * BR_SCALAR_MAX < (float)fabs(numerator)) {
            return;
        }
    }
    if (d > 0.f) {
        if (-numerator < 0.f || -numerator > 0.003 + d) {
            return;
        }
    } else {
        if (numerator < 0.f || numerator > 0.003 - d) {
            return;
        }
    }
    t = -(numerator / d);
    if (t > 1.f) {
        t = 1.f;
    }
    coll_pos->v[0] = t * ray_dir->v[0];
    coll_pos->v[1] = ray_dir->v[1] * t;
    coll_pos->v[2] = ray_dir->v[2] * t;
    coll_pos->v[0] = coll_pos->v[0] + ray_pos->v[0];
    coll_pos->v[1] = coll_pos->v[1] + ray_pos->v[1];
    coll_pos->v[2] = ray_pos->v[2] + coll_pos->v[2];
    axis_m = 0;
    if ((float)fabs(pFace->normal.v[0]) < (float)fabs(pFace->normal.v[1])) {
        axis_m = 1;
    }
    if ((float)fabs(pFace->normal.v[2]) > (float)fabs(pFace->normal.v[axis_m])) {
        axis_m = 2;
    }
    switch (axis_m) {
    case 0:
        axis_0 = 1;
        axis_1 = 2;
        break;
    case 1:
        axis_0 = 0;
        axis_1 = 2;
        break;
    case 2:
        axis_0 = 0;
        axis_1 = 1;
        break;
    }
    u0 = pFace->v[1].v[axis_0] - pFace->v[0].v[axis_0];
    u1 = pFace->v[1].v[axis_1] - pFace->v[0].v[axis_1];
    v0 = pFace->v[2].v[axis_0] - pFace->v[0].v[axis_0];
    v1 = pFace->v[2].v[axis_1] - pFace->v[0].v[axis_1];
    u2 = coll_pos->v[axis_0] - pFace->v[0].v[axis_0];
    v2 = coll_pos->v[axis_1] - pFace->v[0].v[axis_1];
    f_d = u0 * v1 - u1 * v0;
    alpha = u0 * v2 - u1 * u2;
    if ((float)fabs(alpha) > 1.0001 * fabs(f_d) || f_d == 0.f || alpha / f_d < -0.0001) {
        return;
    }
    beta = v0 * v2 - v1 * u2;
    if ((float)fabs(beta) > 1.0001 * fabs(f_d) || f_d == 0.f || beta / f_d >= -0.0001 || alpha / f_d - beta / f_d > 1.0001) {
        return;
    }
    *rt = t;
    normal->v[0] = pFace->normal.v[0];
    normal->v[1] = pFace->normal.v[1];
    normal->v[2] = pFace->normal.v[2];
    if (d > 0.f) {
        normal->v[0] = -normal->v[0];
        normal->v[1] = -normal->v[1];
        normal->v[2] = -normal->v[2];
    }
}

// (reference only)
// FUNCTION: CARMA2_HW 0x0045f0d0
void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt) {
    int i;
    br_scalar t[4];
    br_scalar numerator;
    br_scalar d;
    br_vector3 p[4];
    int axis_m;
    int axis_0;
    int axis_1;
    br_scalar u0[4];
    br_scalar u1;
    br_scalar u2;
    br_scalar v0[4];
    br_scalar v1;
    br_scalar v2;
    br_scalar v0i1;
    br_scalar v0i2;
    br_material* this_material;

    this_material = pFace->material;
    d = BrVector3Dot(ray_dir, &pFace->normal);
    for (i = 0; i < pNum_rays; i++) {
        rt[i] = 100.f;
    }
    if (this_material != NULL && (this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
        return;
    }
    if (this_material != NULL && this_material->identifier != NULL
            && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?')
            && gPling_materials) {
        return;
    }
    if ((float)fabs(d) < 2.3841858e-7f) {
        return;
    }
    for (i = 0; i < pNum_rays; i++) {
        numerator = (ray_pos[i].v[0] - pFace->v[0].v[0]) * pFace->normal.v[0]
            + (ray_pos[i].v[1] - pFace->v[0].v[1]) * pFace->normal.v[1]
            + (ray_pos[i].v[2] - pFace->v[0].v[2]) * pFace->normal.v[2];
        if ((float)fabs(d) < 1.f && (float)fabs(d) * BR_SCALAR_MAX < (float)fabs(numerator)) {
            return;
        }
        if (d > 0.f) {
            if (numerator < -0.001 || numerator > 0.003 - d) {
                t[i] = 100.f;
                continue;
            }
        } else if (-numerator < -0.001 || d - -0.003 < -numerator) {
            t[i] = 100.f;
            continue;
        }
        t[i] = -(numerator / d);
        if (t[i] > 1.f) {
            t[i] = 1.f;
        }
        p[i].v[0] = t[i] * ray_dir->v[0];
        p[i].v[1] = ray_dir->v[1] * t[i];
        p[i].v[2] = ray_dir->v[2] * t[i];
        p[i].v[0] = p[i].v[0] + ray_pos[i].v[0];
        p[i].v[1] = p[i].v[1] + ray_pos[i].v[1];
        p[i].v[2] = ray_pos[i].v[2] + p[i].v[2];
    }
    axis_m = 0;
    if ((float)fabs(pFace->normal.v[0]) < (float)fabs(pFace->normal.v[1])) {
        axis_m = 1;
    }
    if ((float)fabs(pFace->normal.v[2]) > (float)fabs(pFace->normal.v[axis_m])) {
        axis_m = 2;
    }
    switch (axis_m) {
    case 0:
        axis_0 = 1;
        axis_1 = 2;
        break;
    case 1:
        axis_0 = 0;
        axis_1 = 2;
        break;
    case 2:
        axis_0 = 0;
        axis_1 = 1;
        break;
    }
    v0i1 = pFace->v[0].v[axis_0];
    v0i2 = pFace->v[0].v[axis_1];
    u1 = pFace->v[1].v[axis_0] - v0i1;
    u2 = pFace->v[2].v[axis_0] - v0i1;
    v1 = pFace->v[1].v[axis_1] - v0i2;
    v2 = pFace->v[2].v[axis_1] - v0i2;
    for (i = 0; i < pNum_rays; i++) {
        br_scalar s1;
        br_scalar s2;
        br_scalar s3;
        br_scalar s4;

        if (t[i] == 100.f) {
            continue;
        }
        u0[i] = p[i].v[axis_0] - v0i1;
        v0[i] = p[i].v[axis_1] - v0i2;
        s1 = u1 * v0[i] - v1 * u0[i];
        s2 = v2 * u1 - v1 * u2;
        if ((float)fabs(s1) <= 1.0001 * (float)fabs(s2) && s2 != 0.f) {
            s1 = s1 / s2;
            if (s1 >= -0.0001) {
                s3 = u2 * v0[i] - v2 * u0[i];
                s4 = -s2;
                if ((float)fabs(s3) <= 1.0001 * (float)fabs(s4) && s4 != 0.f) {
                    s3 = s3 / s4;
                    if (s3 >= -0.0001 && s1 + s3 <= 1.0001) {
                        rt[i] = t[i];
                        normal->v[0] = pFace->normal.v[0];
                        normal->v[1] = pFace->normal.v[1];
                        normal->v[2] = pFace->normal.v[2];
                        if (d > 0.f) {
                            normal->v[0] = -pFace->normal.v[0];
                            normal->v[1] = -pFace->normal.v[1];
                            normal->v[2] = -pFace->normal.v[2];
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f5b0
void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
    br_vector3 a;
    br_vector3 c[3];
    int i;
    int j;

    BrMatrix34ApplyP(&b2->min, &b1->min, m);
    b2->max.v[0] = b2->min.v[0];
    b2->max.v[1] = b2->min.v[1];
    b2->max.v[2] = b2->min.v[2];
    a.v[0] = b1->max.v[0] - b1->min.v[0];
    a.v[1] = b1->max.v[1] - b1->min.v[1];
    a.v[2] = b1->max.v[2] - b1->min.v[2];
    for (j = 0; j < 3; j++) {
        c[j].v[0] = a.v[j] * m->m[j][0];
        c[j].v[1] = a.v[j] * m->m[j][1];
        c[j].v[2] = a.v[j] * m->m[j][2];
    }
    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            if (c[i].v[j] < 0.f) {
                b2->min.v[j] += c[i].v[j];
            } else {
                b2->max.v[j] += c[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f690
void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds) {
    br_vector3 a;
    br_vector3 c[3];
    int n2;
    int n1;
    int n0;
    int i;

    BrVector3Add(&a, &bnds->original_bounds.min, &bnds->original_bounds.max);
    BrVector3Scale(&a, &a, 0.5f);
    BrMatrix34ApplyP(&bnds->box_centre, &a, bnds->mat);
    BrVector3Sub(&a, &bnds->original_bounds.max, &bnds->original_bounds.min);
    bnds->radius = BrVector3Length(&a) * 0.5f;
    BrMatrix34ApplyP(&bnds->real_bounds.min, &bnds->original_bounds.min, bnds->mat);
    BrVector3Copy(&bnds->real_bounds.max, &bnds->real_bounds.min);
    for (i = 0; i < 3; i++) {
        c[i].v[0] = a.v[i] * bnds->mat->m[i][0];
        c[i].v[1] = a.v[i] * bnds->mat->m[i][1];
        c[i].v[2] = a.v[i] * bnds->mat->m[i][2];
    }
    for (i = 0; i < 3; i++) {
        n0 = c[0].v[i] < 0.f;
        n1 = c[1].v[i] < 0.f;
        n2 = c[2].v[i] < 0.f;
        bnds->real_bounds.min.v[i] += (float)n2 * c[2].v[i] + (float)n0 * c[0].v[i] + (float)n1 * c[1].v[i];
        n0 = c[0].v[i] > 0.f;
        n1 = c[1].v[i] > 0.f;
        n2 = c[2].v[i] > 0.f;
        bnds->real_bounds.max.v[i] += (float)n1 * c[1].v[i] + (float)n0 * c[0].v[i] + (float)n2 * c[2].v[i];
    }
}

extern int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);
extern int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat);
extern int gActorBoxPick_StopGroovidelics;

// (BoundsTransformTest inlined, as in the original)
// FUNCTION: CARMA2_HW 0x0045f8c0
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    br_model* this_model;
    br_material* this_material;
    br_bounds br_bnds;
    int test_children;
    tBounds new_bounds;
    br_matrix34 invmat;
    br_matrix34 mat;
    br_matrix34 box_to_actor;
    br_matrix34 mat2;
    int i;
    int n;
    br_actor* a;
    br_actor* next_a;

    i = 0;
    test_children = 1;
    this_model = ap->model;
    if (this_model == NULL) {
        this_model = model;
    }
    this_material = ap->material;
    if (this_material == NULL) {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return max_face;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        if (this_model == NULL) {
            return max_face;
        }
        if (ap->children == NULL) {
            br_vector3 o;
            br_scalar val;

            if (ap->type != BR_ACTOR_MODEL) {
                return max_face;
            }
            BrVector3Sub(&o, &this_model->bounds.max, &this_model->bounds.min);
            val = this_model->bounds.min.v[0] * ap->t.t.mat.m[0][0] + this_model->bounds.min.v[1] * ap->t.t.mat.m[1][0] + this_model->bounds.min.v[2] * ap->t.t.mat.m[2][0] + ap->t.t.mat.m[3][0];
            if ((ap->t.t.mat.m[0][0] <= 0.0f ? 0.0f : o.v[0] * ap->t.t.mat.m[0][0])
                    + (ap->t.t.mat.m[1][0] <= 0.0f ? 0.0f : o.v[1] * ap->t.t.mat.m[1][0])
                    + (ap->t.t.mat.m[2][0] <= 0.0f ? 0.0f : o.v[2] * ap->t.t.mat.m[2][0])
                    + val
                    < bnds->real_bounds.min.v[0]) {
                return max_face;
            }
            if ((ap->t.t.mat.m[0][0] < 0.0f ? o.v[0] * ap->t.t.mat.m[0][0] : 0.0f)
                    + (ap->t.t.mat.m[1][0] < 0.0f ? o.v[1] * ap->t.t.mat.m[1][0] : 0.0f)
                    + (ap->t.t.mat.m[2][0] < 0.0f ? o.v[2] * ap->t.t.mat.m[2][0] : 0.0f)
                    + val
                    > bnds->real_bounds.max.v[0]) {
                return max_face;
            }
            val = this_model->bounds.min.v[0] * ap->t.t.mat.m[0][2] + this_model->bounds.min.v[1] * ap->t.t.mat.m[1][2] + this_model->bounds.min.v[2] * ap->t.t.mat.m[2][2] + ap->t.t.mat.m[3][2];
            if ((ap->t.t.mat.m[0][2] <= 0.0f ? 0.0f : o.v[0] * ap->t.t.mat.m[0][2])
                    + (ap->t.t.mat.m[1][2] <= 0.0f ? 0.0f : o.v[1] * ap->t.t.mat.m[1][2])
                    + (ap->t.t.mat.m[2][2] <= 0.0f ? 0.0f : o.v[2] * ap->t.t.mat.m[2][2])
                    + val
                    < bnds->real_bounds.min.v[2]) {
                return max_face;
            }
            if ((ap->t.t.mat.m[0][2] < 0.0f ? o.v[0] * ap->t.t.mat.m[0][2] : 0.0f)
                    + (ap->t.t.mat.m[1][2] < 0.0f ? o.v[1] * ap->t.t.mat.m[1][2] : 0.0f)
                    + (ap->t.t.mat.m[2][2] < 0.0f ? o.v[2] * ap->t.t.mat.m[2][2] : 0.0f)
                    + val
                    > bnds->real_bounds.max.v[2]) {
                return max_face;
            }
            val = this_model->bounds.min.v[0] * ap->t.t.mat.m[0][1] + this_model->bounds.min.v[1] * ap->t.t.mat.m[1][1] + this_model->bounds.min.v[2] * ap->t.t.mat.m[2][1] + ap->t.t.mat.m[3][1];
            if ((ap->t.t.mat.m[0][1] <= 0.0f ? 0.0f : o.v[0] * ap->t.t.mat.m[0][1])
                    + (ap->t.t.mat.m[1][1] <= 0.0f ? 0.0f : o.v[1] * ap->t.t.mat.m[1][1])
                    + (ap->t.t.mat.m[2][1] <= 0.0f ? 0.0f : o.v[2] * ap->t.t.mat.m[2][1])
                    + val
                    < bnds->real_bounds.min.v[1]) {
                return max_face;
            }
            if ((ap->t.t.mat.m[0][1] < 0.0f ? o.v[0] * ap->t.t.mat.m[0][1] : 0.0f)
                    + (ap->t.t.mat.m[1][1] < 0.0f ? o.v[1] * ap->t.t.mat.m[1][1] : 0.0f)
                    + (ap->t.t.mat.m[2][1] < 0.0f ? o.v[2] * ap->t.t.mat.m[2][1] : 0.0f)
                    + val
                    > bnds->real_bounds.max.v[1]) {
                return max_face;
            }
        }
        if (pMat != NULL) {
            BrMatrix34Mul(&mat, &ap->t.t.mat, pMat);
            pMat = &mat;
        } else {
            pMat = &ap->t.t.mat;
        }
        BrMatrix34LPInverse(&invmat, &ap->t.t.mat);
        BrMatrix34Mul(&mat2, bnds->mat, &invmat);
        new_bounds.mat = &mat2;
        BrVector3Copy(&new_bounds.original_bounds.min, &bnds->original_bounds.min);
        BrVector3Copy(&new_bounds.original_bounds.max, &bnds->original_bounds.max);
        BrMatrix34ApplyP(&new_bounds.box_centre, &bnds->box_centre, &invmat);
        new_bounds.radius = bnds->radius;
        GetNewBoundingBox(&new_bounds.real_bounds, &new_bounds.original_bounds, new_bounds.mat);
        if (ap->identifier[1] >= '0' && ap->identifier[1] <= '9') {
            if (!BoundsOverlapTest__finteray(&new_bounds.real_bounds, &this_model->bounds)) {
                return max_face;
            }
            BrMatrix34LPInverse(&invmat, bnds->mat);
            BrMatrix34Mul(&box_to_actor, &ap->t.t.mat, &invmat);
            GetNewBoundingBox(&br_bnds, &ap->model->bounds, &box_to_actor);
            if (!BoundsOverlapTest__finteray(&br_bnds, &bnds->original_bounds)) {
                return max_face;
            }
            if (pWorld_callbacks != NULL && pWorld_callbacks->pull_actor_from_world != NULL) {
                if (pWorld_callbacks->pull_actor_from_world(ap) != NULL) {
                    return max_face;
                }
            }
        }
        bnds = &new_bounds;
    }
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model == NULL) {
            return max_face;
        }
        if (gActorBoxPick_StopGroovidelics != 0 && BoundsOverlapTest__finteray(&bnds->real_bounds, &this_model->bounds) != 0) {
            n = ModelPickBox(ap, bnds, this_model, this_material, face_list, max_face, pMat);
            if (pMat != NULL && max_face != n && pWorld_callbacks != NULL && pWorld_callbacks->stop_groovidelic != NULL) {
                pWorld_callbacks->stop_groovidelic(ap);
            }
            i = max_face - n;
            max_face = n;
        }
    } else if (ap->type >= BR_ACTOR_BOUNDS && ap->type <= BR_ACTOR_BOUNDS_CORRECT) {
        test_children = BoundsOverlapTest__finteray(&bnds->real_bounds, (br_bounds*)ap->type_data);
    }
    if (test_children != 0) {
        a = ap->children;
        while (a != NULL) {
            next_a = a->next;
            n = ActorBoxPick(bnds, a, this_model, this_material, &face_list[i], max_face, pMat, pWorld_callbacks);
            i += max_face - n;
            max_face = n;
            a = next_a;
        }
    }
    return max_face;
}

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00460bd0
int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT) {
    br_vector3 dir;
    int inside;
    int quad[3];
    int i;
    int which_plane;
    br_scalar max_t[3];
    br_scalar cp[3];

    inside = 1;
    dir.v[0] = pEnd->v[0] - pStart->v[0];
    dir.v[1] = pEnd->v[1] - pStart->v[1];
    dir.v[2] = pEnd->v[2] - pStart->v[2];
    for (i = 0; i < 3; ++i) {
        if (pStart->v[i] < pBounds->min.v[i]) {
            quad[i] = 1;
            cp[i] = pBounds->min.v[i];
            inside = 0;
        } else if (pStart->v[i] > pBounds->max.v[i]) {
            quad[i] = 0;
            cp[i] = pBounds->max.v[i];
            inside = 0;
        } else {
            quad[i] = 2;
        }
    }
    if (inside) {
        pPoint->v[0] = pStart->v[0];
        pPoint->v[1] = pStart->v[1];
        pPoint->v[2] = pStart->v[2];
        return -1;
    }
    for (i = 0; i < 3; ++i) {
        if (quad[i] != 2 && dir.v[i] != 0.0f) {
            max_t[i] = (cp[i] - pStart->v[i]) / dir.v[i];
        } else {
            max_t[i] = -1.0f;
        }
    }
    which_plane = 0;
    for (i = 1; i < 3; ++i) {
        if (max_t[i] > max_t[which_plane]) {
            which_plane = i;
        }
    }
    if (max_t[which_plane] < 0.0f || max_t[which_plane] > 1.0f) {
        return 0;
    }
    for (i = 0; i < 3; ++i) {
        if (which_plane != i) {
            pPoint->v[i] = dir.v[i] * max_t[which_plane] + pStart->v[i];
            if (pPoint->v[i] < pBounds->min.v[i] || pBounds->max.v[i] < pPoint->v[i]) {
                return 0;
            }
        } else {
            pPoint->v[i] = cp[i];
        }
    }
    *pT = max_t[which_plane];
    return which_plane + 4 * quad[which_plane] + 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00460da0
int C2_HOOK_FASTCALL SphereBoxIntersection(const br_bounds* pBounds, const br_vector3* pCenter, br_scalar pRadius_squared, br_vector3* pClosest) {
    int axis;
    br_scalar closest;
    br_scalar distance;

    distance = 0.f;
    for (axis = 0; axis < 3; axis++) {
        if (pCenter->v[axis] <= pBounds->min.v[axis]) {
            closest = pBounds->min.v[axis];
        } else if (pCenter->v[axis] >= pBounds->max.v[axis]) {
            closest = pBounds->max.v[axis];
        } else {
            closest = pCenter->v[axis];
        }
        pClosest->v[axis] = closest;
        distance += (pCenter->v[axis] - closest) * (pCenter->v[axis] - closest);
    }
    return distance <= pRadius_squared;
}

// FUNCTION: CARMA2_HW 0x00460e10
int C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int uVar2;
    int uVar4;
    int i;
    br_scalar fVar1;
    br_scalar n1;
    br_scalar n2;

    uVar2 = LineBoxColl(pStart, pEnd, pBounds, pPoint, pT);
    if (uVar2 != 0) {
        pNormal->v[0] = 0.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
        uVar4 = (uVar2 - 1) & 3;
        pNormal->v[uVar4] = (uVar2 & 4) ? 1.0f : -1.0f;
        return uVar2;
    }
    if (SphereBoxIntersection(pBounds, pEnd, 2.49999994e-05f, pPoint) != 0) {
        *pT = 1.0f;
        pNormal->v[0] = 0.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
        for (i = 0; i < 3; i++) {
            if (pPoint->v[i] == pBounds->max.v[i]) {
                pNormal->v[i] = -1.0f;
            } else if (pPoint->v[i] == pBounds->min.v[i]) {
                pNormal->v[i] = 1.0f;
            }
        }
        n1 = pNormal->v[1];
        n2 = pNormal->v[2];
        fVar1 = sqrt(n1 * n1 + n2 * n2 + pNormal->v[0] * pNormal->v[0]);
        if (fVar1 > 2.3841858e-07f) {
            fVar1 = 1.0 / fVar1;
            pNormal->v[0] = fVar1 * pNormal->v[0];
            pNormal->v[1] = pNormal->v[1] * fVar1;
            pNormal->v[2] = pNormal->v[2] * fVar1;
            return 0x40;
        }
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
        return 0x40;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00460f60
int C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_scalar fVar1;
    br_scalar fVar2;
    br_scalar fVar3;
    br_scalar fVar4;
    br_scalar fVar5;
    br_scalar fVar6;
    br_scalar fVar7;
    br_scalar fVar8;

    fVar1 = pSphere->center.v[0] - pStart->v[0];
    fVar3 = pSphere->center.v[1] - pStart->v[1];
    fVar4 = pSphere->center.v[2] - pStart->v[2];
    fVar2 = fVar4 * fVar4 + fVar3 * fVar3 + fVar1 * fVar1;
    if (fVar2 < pSphere->radius_squared) {
        return -1;
    }
    fVar7 = pEnd->v[0] - pStart->v[0];
    fVar5 = pEnd->v[1] - pStart->v[1];
    fVar8 = pEnd->v[2] - pStart->v[2];
    fVar6 = fVar8 * fVar8 + fVar5 * fVar5 + fVar7 * fVar7;
    if (1e-06f <= fVar6) {
        fVar1 = (fVar4 * fVar8 + fVar3 * fVar5 + fVar1 * fVar7) / fVar6;
        fVar2 = fVar2 - fVar1 * fVar1 * fVar6;
        if (pSphere->radius_squared < fVar2) {
            return 0;
        }
        fVar1 = fVar1 - sqrt((pSphere->radius_squared - fVar2) / fVar6);
        *pT = fVar1;
        if ((0.0f <= fVar1) && (fVar1 <= 1.0f)) {
            pPoint->v[0] = fVar7 * fVar1 + pStart->v[0];
            pPoint->v[1] = fVar5 * fVar1 + pStart->v[1];
            pPoint->v[2] = fVar8 * fVar1 + pStart->v[2];
            if (pNormal != 0) {
                pNormal->v[0] = pSphere->center.v[0] - pPoint->v[0];
                pNormal->v[1] = pSphere->center.v[1] - pPoint->v[1];
                fVar1 = pSphere->center.v[2] - pPoint->v[2];
                pNormal->v[2] = fVar1;
                fVar1 = sqrt(pNormal->v[0] * pNormal->v[0] + fVar1 * fVar1 + pNormal->v[1] * pNormal->v[1]);
                if (2.3841858e-07 < fVar1) {
                    fVar1 = 1.0 / fVar1;
                    pNormal->v[0] = fVar1 * pNormal->v[0];
                    pNormal->v[1] = pNormal->v[1] * fVar1;
                    pNormal->v[2] = pNormal->v[2] * fVar1;
                    return 1;
                }
                pNormal->v[0] = 1.0f;
                pNormal->v[1] = 0.0f;
                pNormal->v[2] = 0.0f;
            }
            return 1;
        }
    }
    return 0;
}

// LineSphereCollWithSphere

// LinePolyColl

// SpherePolyIntersection

// LinePolyCollWithSphere

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);
extern int C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT);
extern int C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

// FUNCTION: CARMA2_HW 0x00461740
int C2_HOOK_FASTCALL ShapeRayCast(const br_vector3* p1, const br_vector3* p2, const tPhysics_shape* pShape, br_vector3* pPos, float* pFactor, br_vector3* pNormal) {
    int index;
    int result;

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        result = LineBoxColl((br_vector3*)p1, (br_vector3*)p2, (br_bounds*)&pShape->box.common.bb, pPos, pFactor);
        if (pNormal != NULL && result > 0) {
            index = (result - 1) & 3;
            pNormal->v[0] = 0.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
            pNormal->v[index] = (result & 4) ? 1.0f : -1.0f;
        }
        return result;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        result = LinePolyColl((br_vector3*)p1, (br_vector3*)p2, (tCollision_shape_polyhedron_data*)&pShape->polyhedron.polyhedron, pPos, 0, pFactor);
        if (pNormal != NULL && result > 0) {
            index = (result & 0x3f) - 1;
            pNormal->v[0] = pShape->polyhedron.polyhedron.planes[index].v[0];
            pNormal->v[1] = pShape->polyhedron.polyhedron.planes[index].v[1];
            pNormal->v[2] = pShape->polyhedron.polyhedron.planes[index].v[2];
        }
        return result;
    case kCollisionShapeType_Sphere:
        return LineSphereColl((br_vector3*)p1, (br_vector3*)p2, (tCollision_shape_sphere_data*)&pShape->sphere.sphere, pPos, pNormal, pFactor);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
    case kCollisionShapeType_Wireframe:
        return 0;
    }
}

extern int C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);
extern int C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

// FUNCTION: CARMA2_HW 0x00461850
int C2_HOOK_FASTCALL ShapeRayCastWithSphere(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        return LineBoxCollWithSphere(pStart, pEnd, &pShape->common.bb, pPoint, pNormal, pT);
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        return LinePolyCollWithSphere(pStart, pEnd, &pShape->polyhedron, pPoint, pNormal, pT);
    case kCollisionShapeType_Sphere:
        return LineSphereCollWithSphere(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
    case kCollisionShapeType_Wireframe:
        return 0;
    }
}

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

// FUNCTION: CARMA2_HW 0x004618e0
tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pCount, br_scalar pOffset, br_vector3* pCollision_position) {
    br_vector3 direction;
    br_vector3 position;
    br_vector3 normal;
    br_scalar t;
    tFace_ref* face;

    BrVector3Set(&direction, 0.f, -1.f, 0.f);
    face = RayCastThruFaceList(pPosition, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    if (face == NULL && pOffset != 0.f) {
        BrVector3Set(&position, pPosition->v[0], pPosition->v[1] + pOffset, pPosition->v[2]);
        face = RayCastThruFaceList(&position, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    }
    return face;
}


// extern so 49-physics.c keeps ownership of the global
extern tFace_ref* gPling_face;

static __inline int C2_HOOK_FASTCALL ModelPickBox_Clip(br_vector3* polygon, int count, int axis, br_scalar limit, int upper) {
    br_vector3 clipped[12];
    int previous = count - 1;
    int vertex, component;
    int result = 0;

    for (vertex = 0; vertex < count; vertex++) {
        if ((polygon[vertex].v[axis] > limit) != (polygon[previous].v[axis] > limit)) {
            for (component = 0; component < 3; component++) {
                if (component != axis) {
                    clipped[result].v[component] = ((polygon[vertex].v[component] - polygon[previous].v[component])
                        * (limit - polygon[previous].v[axis]))
                        / (polygon[vertex].v[axis] - polygon[previous].v[axis]) + polygon[previous].v[component];
                }
            }
            clipped[result].v[axis] = limit;
            result++;
        }
        if ((!upper && polygon[vertex].v[axis] >= limit)
            || (upper && polygon[vertex].v[axis] <= limit)) {
            clipped[result].v[0] = polygon[vertex].v[0];
            clipped[result].v[1] = polygon[vertex].v[1];
            clipped[result].v[2] = polygon[vertex].v[2];
            result++;
        }
        previous = vertex;
    }
    for (vertex = 0; vertex < result; vertex++) {
        polygon[vertex].v[0] = clipped[vertex].v[0];
        polygon[vertex].v[1] = clipped[vertex].v[1];
        polygon[vertex].v[2] = clipped[vertex].v[2];
    }
    return result;
}

// PARTIAL 58.18%: ModelPickBox 0x0045ff90 - frame 0x230 vs 0x22c (one scalar slot extra), pushes not shrink-wrapped after the max_face test, x87 schedule of the distance test
// based on ApexWorm/rec2 6ef908265 (reference only)
// FUNCTION: CARMA2_HW 0x0045ff90
int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
    v11model* prepared = ((volatile br_model*)model)->prepared;
    v11group* prepared_group;
    v11face* face;
    br_vector3 polygon[12];
    br_vector3 normal;
    int group, f, axis, count;
    int i0, i1, i2;
    br_scalar limit;
    br_uint_16 original_face;

    if (max_face <= 0) {
        return 0;
    }
    for (group = 0; group < model->prepared->ngroups; group++) {
        prepared_group = &model->prepared->groups[group];
        face = prepared_group->faces;
        for (f = 0; f < model->prepared->groups[group].nfaces; f++, face++) {
            i0 = face->vertices[0];
            if ((float)fabs((prepared_group->vertices[i0].p.v[0] - bnds->box_centre.v[0]) * face->eqn.v[0]
                + face->eqn.v[2] * (prepared_group->vertices[i0].p.v[2] - bnds->box_centre.v[2])
                + face->eqn.v[1] * (prepared_group->vertices[i0].p.v[1] - bnds->box_centre.v[1])) > bnds->radius) {
                continue;
            }
            i1 = face->vertices[1];
            i2 = face->vertices[2];
            limit = bnds->real_bounds.min.v[0];
            if (prepared_group->vertices[i0].p.v[0] < limit && prepared_group->vertices[i1].p.v[0] < limit && prepared_group->vertices[i2].p.v[0] < limit) {
                continue;
            }
            limit = bnds->real_bounds.max.v[0];
            if (prepared_group->vertices[i0].p.v[0] > limit && prepared_group->vertices[i1].p.v[0] > limit && prepared_group->vertices[i2].p.v[0] > limit) {
                continue;
            }
            limit = bnds->real_bounds.min.v[1];
            if (prepared_group->vertices[i0].p.v[1] < limit && prepared_group->vertices[i1].p.v[1] < limit && prepared_group->vertices[i2].p.v[1] < limit) {
                continue;
            }
            limit = bnds->real_bounds.max.v[1];
            if (prepared_group->vertices[i0].p.v[1] > limit && prepared_group->vertices[i1].p.v[1] > limit && prepared_group->vertices[i2].p.v[1] > limit) {
                continue;
            }
            limit = bnds->real_bounds.min.v[2];
            if (prepared_group->vertices[i0].p.v[2] < limit && prepared_group->vertices[i1].p.v[2] < limit && prepared_group->vertices[i2].p.v[2] < limit) {
                continue;
            }
            limit = bnds->real_bounds.max.v[2];
            if (prepared_group->vertices[i0].p.v[2] > limit && prepared_group->vertices[i1].p.v[2] > limit && prepared_group->vertices[i2].p.v[2] > limit) {
                continue;
            }
            polygon[1].v[0] = prepared_group->vertices[i0].p.v[0] - bnds->mat->m[3][0];
            polygon[1].v[1] = model->prepared->groups[group].vertices[i0].p.v[1] - bnds->mat->m[3][1];
            polygon[1].v[2] = model->prepared->groups[group].vertices[i0].p.v[2] - bnds->mat->m[3][2];
            polygon[2].v[0] = model->prepared->groups[group].vertices[i1].p.v[0] - bnds->mat->m[3][0];
            polygon[2].v[1] = model->prepared->groups[group].vertices[i1].p.v[1] - bnds->mat->m[3][1];
            polygon[2].v[2] = model->prepared->groups[group].vertices[i1].p.v[2] - bnds->mat->m[3][2];
            polygon[3].v[0] = model->prepared->groups[group].vertices[i2].p.v[0] - bnds->mat->m[3][0];
            polygon[3].v[1] = model->prepared->groups[group].vertices[i2].p.v[1] - bnds->mat->m[3][1];
            polygon[3].v[2] = model->prepared->groups[group].vertices[i2].p.v[2] - bnds->mat->m[3][2];
            BrMatrix34TApplyV(&polygon[0], &polygon[1], bnds->mat);
            BrMatrix34TApplyV(&polygon[1], &polygon[2], bnds->mat);
            BrMatrix34TApplyV(&polygon[2], &polygon[3], bnds->mat);
            count = 3;
            for (axis = 0; axis < 3; axis++) {
                count = ModelPickBox_Clip(polygon, count, axis, bnds->original_bounds.min.v[axis], 0);
                if (count < 3) {
                    break;
                }
                count = ModelPickBox_Clip(polygon, count, axis, bnds->original_bounds.max.v[axis], 1);
                if (count < 3) {
                    break;
                }
            }
            if (count < 3) {
                continue;
            }
            if (pMat != NULL) {
                BrMatrix34ApplyP(&face_list->v[0], &model->prepared->groups[group].vertices[i0].p, pMat);
                BrMatrix34ApplyP(&face_list->v[1], &model->prepared->groups[group].vertices[i1].p, pMat);
                BrMatrix34ApplyP(&face_list->v[2], &model->prepared->groups[group].vertices[i2].p, pMat);
                normal.v[0] = face->eqn.v[0];
                normal.v[1] = face->eqn.v[1];
                normal.v[2] = face->eqn.v[2];
                BrMatrix34ApplyV(&face_list->normal, &normal, pMat);
            } else {
                face_list->v[0].v[0] = model->prepared->groups[group].vertices[i0].p.v[0];
                face_list->v[0].v[1] = model->prepared->groups[group].vertices[i0].p.v[1];
                face_list->v[0].v[2] = model->prepared->groups[group].vertices[i0].p.v[2];
                face_list->v[1].v[0] = model->prepared->groups[group].vertices[i1].p.v[0];
                face_list->v[1].v[1] = model->prepared->groups[group].vertices[i1].p.v[1];
                face_list->v[1].v[2] = model->prepared->groups[group].vertices[i1].p.v[2];
                face_list->v[2].v[0] = model->prepared->groups[group].vertices[i2].p.v[0];
                face_list->v[2].v[1] = model->prepared->groups[group].vertices[i2].p.v[1];
                face_list->v[2].v[2] = model->prepared->groups[group].vertices[i2].p.v[2];
                face_list->normal.v[0] = face->eqn.v[0];
                face_list->normal.v[1] = face->eqn.v[1];
                face_list->normal.v[2] = face->eqn.v[2];
            }
            face_list->material = *model->prepared->groups[group].face_colours.materials;
            if (face_list->material == NULL) {
                face_list->material = model_material;
            }
            if (pMat != NULL) {
                face_list->d = face_list->normal.v[2] * face_list->v[0].v[2] + face_list->normal.v[1] * face_list->v[0].v[1] + face_list->v[0].v[0] * face_list->normal.v[0];
            } else {
                face_list->d = face->eqn.v[3];
            }
            face_list->a = actor;
            if (model->flags & BR_MODF_UPDATEABLE) {
                face_list->g = -1;
                original_face = model->prepared->groups[group].face_user[f];
                face_list->f = original_face;
                face_list->flags = model->faces[original_face].flags;
                face_list->map[0] = &model->vertices[model->faces[original_face].vertices[0]].map;
                face_list->map[1] = &model->vertices[model->faces[original_face].vertices[1]].map;
                face_list->map[2] = &model->vertices[model->faces[original_face].vertices[2]].map;
            } else {
                face_list->g = group;
                face_list->f = f;
                face_list->flags = 0;
                if (prepared->groups[group].faces[f].edges[0] == 0) {
                    face_list->flags |= 1;
                }
                if (prepared->groups[group].faces[f].edges[1] == 0) {
                    face_list->flags |= 2;
                }
                if (prepared->groups[group].faces[f].edges[2] == 0) {
                    face_list->flags |= 4;
                }
                face_list->map[0] = &model->prepared->groups[group].vertices[i0].map;
                face_list->map[1] = &model->prepared->groups[group].vertices[i1].map;
                face_list->map[2] = &model->prepared->groups[group].vertices[i2].map;
            }
            if (face_list->material != NULL && face_list->material->identifier != NULL && face_list->material->identifier[0] == '!') {
                gPling_face = face_list;
            }
            face_list++;
            max_face--;
            if (max_face == 0) {
                break;
            }
        }
        if (max_face == 0) {
            break;
        }
    }
    return max_face;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#define LSWS_LOAD(p) (*(const volatile br_scalar*)(p))
#define LSWS_SC(v) ((br_scalar)(v))
// FUNCTION: CARMA2_HW 0x004611b0
int C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    double z;
    double squared;
    float squared_scalar;
    double length;
    double scale;
    double y;
    double normal_z;
    double point_y;
    double point_z;
    float point_x;

    result = LineSphereColl(pStart, pEnd, pSphere, pPoint, pNormal, pT);
    if (result != 0) {
        return result;
    }
    pNormal->v[0] = LSWS_SC(LSWS_LOAD(&pSphere->center.v[0]) - pEnd->v[0]);
    pNormal->v[1] = LSWS_SC(LSWS_LOAD(&pSphere->center.v[1]) - pEnd->v[1]);
    z = LSWS_LOAD(&pSphere->center.v[2]) - pEnd->v[2];
    squared = ((double)LSWS_LOAD(&pNormal->v[0]) * LSWS_LOAD(&pNormal->v[0]) + (double)LSWS_LOAD(&pNormal->v[1]) * LSWS_LOAD(&pNormal->v[1])) + z * LSWS_SC(z);
    pNormal->v[2] = LSWS_SC(z);
    squared_scalar = LSWS_SC(squared);
    if (squared > LSWS_LOAD(&pSphere->radius_squared) - (LSWS_LOAD(&pSphere->radius_squared) * 0.002f) * -2.0f) {
        return 0;
    }
    if (!(squared >= LSWS_LOAD(&pSphere->radius_squared))) {
        return 0;
    }
    length = sqrt((double)squared_scalar);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        y = LSWS_LOAD(&pNormal->v[1]);
        normal_z = LSWS_LOAD(&pNormal->v[2]);
        pNormal->v[0] = LSWS_SC(scale * LSWS_LOAD(&pNormal->v[0]));
        pNormal->v[1] = LSWS_SC(y * scale);
        pNormal->v[2] = LSWS_SC(normal_z * scale);
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    pPoint->v[0] = LSWS_SC(LSWS_LOAD(&pSphere->radius) * LSWS_LOAD(&pNormal->v[0]));
    point_x = (br_scalar)LSWS_LOAD(&pPoint->v[0]);
    point_y = (double)LSWS_LOAD(&pNormal->v[1]) * LSWS_LOAD(&pSphere->radius);
    pPoint->v[1] = LSWS_SC(point_y);
    point_y = LSWS_LOAD(&pPoint->v[1]);
    point_z = (double)LSWS_LOAD(&pNormal->v[2]) * LSWS_LOAD(&pSphere->radius);
    pPoint->v[2] = LSWS_SC(point_z);
    point_z = LSWS_LOAD(&pPoint->v[2]);
    pPoint->v[0] = LSWS_SC((double)point_x + LSWS_LOAD(&pSphere->center.v[0]));
    pPoint->v[1] = LSWS_SC(point_y + LSWS_LOAD(&pSphere->center.v[1]));
    pPoint->v[2] = LSWS_SC(point_z + LSWS_LOAD(&pSphere->center.v[2]));
    *pT = 1.0f;
    return 2;
}
#undef LSWS_LOAD
#undef LSWS_SC
// PARTIAL 81.89%: LinePolyCollWithSphere 0x461520 [sonnet] - x87 schedule of plane dot product, normal copy via int moves, normalize branch layout
// FUNCTION: CARMA2_HW 0x00461520
int C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    int i;
    int count;
    float dist[64];
    br_scalar len;
    int idx;
    tCollision_shape_polyhedron_data* poly;

    poly = &pShape->polyhedron;
    result = LinePolyColl(pStart, pEnd, poly, pPoint, 0.0f, pT);
    i = 0;
    if (result != i) {
        if (result < i) {
            BrVector3SetFloat(pNormal, 1.0f, 0.0f, 0.0f);
            return result;
        }
        idx = (result & 0x3f) - 1;
        pNormal->v[0] = pShape->polyhedron.planes[idx].v[0];
        pNormal->v[1] = pShape->polyhedron.planes[idx].v[1];
        pNormal->v[2] = pShape->polyhedron.planes[idx].v[2];
        return result;
    }
    for (; i < poly->count_planes; i++) {
        dist[i] = poly->planes[i].v[0] * pEnd->v[0] + poly->planes[i].v[1] * pEnd->v[1] + poly->planes[i].v[2] * pEnd->v[2] + poly->planes[i].v[3];
        if (dist[i] < -0.005f) {
            return 0;
        }
    }
    count = 0;
    pNormal->v[0] = 0.0f;
    pNormal->v[1] = 0.0f;
    pNormal->v[2] = 0.0f;
    for (i = 0; i < poly->count_planes; i++) {
        if (dist[i] < 0.0f) {
            count++;
            pNormal->v[0] = poly->planes[i].v[0] + pNormal->v[0];
            pNormal->v[1] = poly->planes[i].v[1] + pNormal->v[1];
            pNormal->v[2] = poly->planes[i].v[2] + pNormal->v[2];
        }
    }
    if (count > 1) {
        len = (br_scalar)sqrt(pNormal->v[0] * pNormal->v[0] + pNormal->v[2] * pNormal->v[2] + pNormal->v[1] * pNormal->v[1]);
        if (len <= 2.3841858e-07f) {
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
        } else {
            len = 1.0f / len;
            pNormal->v[0] = len * pNormal->v[0];
            pNormal->v[1] = pNormal->v[1] * len;
            pNormal->v[2] = pNormal->v[2] * len;
        }
    }
    pPoint->v[0] = pEnd->v[0];
    pPoint->v[1] = pEnd->v[1];
    pPoint->v[2] = pEnd->v[2];
    *pT = 1.0f;
    return 0x40;
}
// PARTIAL 59.26%: stack slot layout of locals and x87 schedule differ
// FUNCTION: CARMA2_HW 0x00461310
int C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT) {
    br_scalar denominators[64];
    br_scalar candidate[64];
    br_scalar dot;
    int selected;
    br_vector3 direction;
    int count;
    int plane;
    int inside;
    br_vector4* eq;
    br_scalar best_t;
    br_scalar den, dist;

    *pT = 2.0f;
    direction.v[0] = pEnd->v[0] - pStart->v[0];
    direction.v[1] = pEnd->v[1] - pStart->v[1];
    direction.v[2] = pEnd->v[2] - pStart->v[2];
    count = pPoly->count_planes;
    plane = 0;
    inside = 1;
    selected = -1;
    best_t = 0.0f;
    if (count > 0) {
        eq = pPoly->planes;
        do {
            den = eq->v[2] * direction.v[2] + eq->v[1] * direction.v[1] + eq->v[0] * direction.v[0];
            denominators[plane] = den;
            dot = eq->v[0] * pStart->v[0] + pStart->v[1] * eq->v[1] + eq->v[2] * pStart->v[2] + pRadius;
            dist = eq->v[3] + dot;
            if (dist < 0.0f) {
                inside = 0;
            }
            if (den == 0.0f) {
                if (dist < 0.0f) {
                    return 0;
                }
            } else {
                candidate[plane] = -(dist / den);
            }
            if (den > 0.0f && best_t <= candidate[plane]) {
                if (candidate[plane] > 1.0f) {
                    return 0;
                }
                best_t = candidate[plane];
                selected = plane;
            }
            plane++;
            eq++;
        } while (plane < count);
    }
    if (inside) {
        return -1;
    }
    if (selected < 0) {
        return 0;
    }
    for (plane = 0; plane < count; plane++) {
        if (denominators[plane] < 0.0f && candidate[plane] < best_t) {
            return 0;
        }
    }
    pPoint->v[0] = best_t * direction.v[0];
    pPoint->v[1] = best_t * direction.v[1];
    pPoint->v[2] = best_t * direction.v[2];
    pPoint->v[0] = pStart->v[0] + pPoint->v[0];
    pPoint->v[1] = pPoint->v[1] + pStart->v[1];
    pPoint->v[2] = pPoint->v[2] + pStart->v[2];
    *pT = best_t;
    return selected + 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 45.58%: SpherePolyFaceCollide 0x45e020 [fastcall] - x87 schedule/frame size (orig 0x10 frame, delta spilled)
// FUNCTION: CARMA2_HW 0x0045e020
void C2_HOOK_FASTCALL SpherePolyFaceCollide(br_vector3* pEnd, br_vector3* pStart, int pUnused, br_vector3* pDirection, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_scalar fVar1;
    br_scalar fVar2;
    br_scalar fVar3;
    br_scalar fVar4;
    br_scalar fVar5;
    br_vector3 delta;

    delta.v[1] = pEnd->v[1] - pStart->v[1];
    delta.v[2] = pEnd->v[2] - pStart->v[2];
    delta.v[0] = pEnd->v[0] - pStart->v[0];
    fVar1 = pDirection->v[0] * pDirection->v[0] + pDirection->v[2] * pDirection->v[2] + pDirection->v[1] * pDirection->v[1];
    fVar3 = (pDirection->v[0] * delta.v[0] + pDirection->v[1] * delta.v[1] + pDirection->v[2] * delta.v[2]) / fVar1;
    fVar2 = (delta.v[2] * delta.v[2] + delta.v[1] * delta.v[1] + delta.v[0] * delta.v[0]) - fVar3 * fVar3 * fVar1;
    if (fVar2 <= pSphere->radius_squared) {
        fVar3 = fVar3 - sqrt((pSphere->radius_squared - fVar2) / fVar1);
        if (0.0f <= fVar3 && fVar3 <= 1.0f) {
            pPoint->v[0] = pEnd->v[0];
            pPoint->v[1] = pEnd->v[1];
            pPoint->v[2] = pEnd->v[2];
            fVar4 = pPoint->v[0] - (pDirection->v[0] * fVar3 + pStart->v[0]);
            fVar5 = pPoint->v[1] - (pDirection->v[1] * fVar3 + pStart->v[1]);
            fVar1 = pPoint->v[2] - (pDirection->v[2] * fVar3 + pStart->v[2]);
            fVar2 = sqrt(fVar1 * fVar1 + fVar5 * fVar5 + fVar4 * fVar4);
            if (2.3841858e-07f < fVar2) {
                fVar2 = 1.0f / fVar2;
                pNormal->v[0] = fVar2 * fVar4;
                pNormal->v[1] = fVar2 * fVar5;
                pNormal->v[2] = fVar2 * fVar1;
                *pT = fVar3;
                return;
            }
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
            *pT = fVar3;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
