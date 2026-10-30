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

// StationarySphereEdgeCollide

// PointSphereCollision

// SphereEdgeCollide

// SpherePolyFaceCollide

// SphereBoxFaceCollide

// SphereFaceCollide

// based on carpocalypse2 src/carma2/common/finteray.c:642 (reference only)
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

// based on carpocalypse2 src/carma2/common/finteray.c:184 (reference only)
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

// STUB: CARMA2_HW 0x0045f690
void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0045f8c0
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

/* FP matching helpers, based on carpocalypse2 src/carma2/common/finteray.c */
static float C2_HOOK_FASTCALL FinterayScalar(double value) {
    volatile float scalar = (float)value;

    return scalar;
}

static int C2_HOOK_FASTCALL FinterayBoxNaN(double value) {
    union { double value; tU32 words[2]; } bits;

    bits.value = value;
    return (bits.words[1] & 0x7ff00000u) == 0x7ff00000u && ((bits.words[1] & 0xfffffu) != 0 || bits.words[0] != 0);
}

static void C2_HOOK_FASTCALL FinterayBoxCopyScalar(br_scalar* output, const br_scalar* input) {
    tU32 value;

    memcpy(&value, input, sizeof(value));
    memcpy(output, &value, sizeof(value));
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/finteray.c:1944
// FUNCTION: CARMA2_HW 0x00460bd0
int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT) {
    br_vector3 direction;
    br_vector3 plane;
    br_vector3 candidate;
    int quadrant[3];
    int inside = 1;
    int axis;
    int selected = 0;
    double coordinate;

    for (axis = 0; axis < 3; axis++) {
        direction.v[axis] = FinterayScalar((double)pEnd->v[axis] - pStart->v[axis]);
    }
    for (axis = 0; axis < 3; axis++) {
        if (!(pStart->v[axis] >= pBounds->min.v[axis])) {
            quadrant[axis] = 1;
            FinterayBoxCopyScalar(&plane.v[axis], &pBounds->min.v[axis]);
            inside = 0;
        } else if (pStart->v[axis] > pBounds->max.v[axis]) {
            quadrant[axis] = 0;
            FinterayBoxCopyScalar(&plane.v[axis], &pBounds->max.v[axis]);
            inside = 0;
        } else {
            quadrant[axis] = 2;
        }
    }
    if (inside) {
        for (axis = 0; axis < 3; axis++) {
            FinterayBoxCopyScalar(&pPoint->v[axis], &pStart->v[axis]);
        }
        return -1;
    }
    for (axis = 0; axis < 3; axis++) {
        if (quadrant[axis] == 2 || direction.v[axis] == 0.0f || FinterayBoxNaN(direction.v[axis])) {
            candidate.v[axis] = -1.0f;
        } else {
            candidate.v[axis] = FinterayScalar(((double)plane.v[axis] - pStart->v[axis]) / direction.v[axis]);
        }
    }
    for (axis = 1; axis < 3; axis++) {
        if (candidate.v[axis] > candidate.v[selected]) {
            selected = axis;
        }
    }
    if (!(candidate.v[selected] >= 0.0f) || candidate.v[selected] > 1.0f) {
        return 0;
    }
    for (axis = 0; axis < 3; axis++) {
        if (axis == selected) {
            FinterayBoxCopyScalar(&pPoint->v[axis], &plane.v[axis]);
        } else {
            coordinate = (double)direction.v[axis] * candidate.v[selected] + pStart->v[axis];
            pPoint->v[axis] = FinterayScalar(coordinate);
            if (!(coordinate >= pBounds->min.v[axis]) || coordinate > pBounds->max.v[axis]) {
                return 0;
            }
        }
    }
    FinterayBoxCopyScalar(pT, &candidate.v[selected]);
    return selected + 1 + 4 * quadrant[selected];
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

// LineBoxCollWithSphere

// LineSphereColl

// LineSphereCollWithSphere

// LinePolyColl

// SpherePolyIntersection

// LinePolyCollWithSphere

// ShapeRayCast

// ShapeRayCastWithSphere

// CastRayDownThruFaces


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
            for (component = 0; component < 3; component++) {
                clipped[result].v[component] = polygon[vertex].v[component];
            }
            result++;
        }
        previous = vertex;
    }
    for (vertex = 0; vertex < result; vertex++) {
        for (component = 0; component < 3; component++) {
            polygon[vertex].v[component] = clipped[vertex].v[component];
        }
    }
    return result;
}

// based on ApexWorm/rec2 6ef908265 (reference only)
// FUNCTION: CARMA2_HW 0x0045ff90
int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
    v11model* prepared = ((volatile br_model*)model)->prepared;
    v11group* prepared_group;
    v11face* face;
    br_vector3 polygon[12];
    br_vector3 normal;
    int group, f, vertex, component, axis, count;
    int indices[3];
    float distance;
    br_scalar limit;
    int rejected;
    br_uint_16 original_face;

    if (max_face <= 0) {
        return 0;
    }
    for (group = 0; group < model->prepared->ngroups; group++) {
        prepared_group = &model->prepared->groups[group];
        face = prepared_group->faces;
        for (f = 0; f < model->prepared->groups[group].nfaces; f++, face++) {
            indices[0] = face->vertices[0];
            distance = (prepared_group->vertices[indices[0]].p.v[0] - bnds->box_centre.v[0]) * face->eqn.v[0]
                + (prepared_group->vertices[indices[0]].p.v[2] - bnds->box_centre.v[2]) * face->eqn.v[2]
                + (prepared_group->vertices[indices[0]].p.v[1] - bnds->box_centre.v[1]) * face->eqn.v[1];
            if (fabs(distance) > bnds->radius) {
                continue;
            }
            indices[1] = face->vertices[1];
            indices[2] = face->vertices[2];
            rejected = 0;
            for (axis = 0; axis < 3; axis++) {
                limit = bnds->real_bounds.min.v[axis];
                if (!(prepared_group->vertices[indices[0]].p.v[axis] >= limit)
                    && !(prepared_group->vertices[indices[1]].p.v[axis] >= limit)
                    && !(prepared_group->vertices[indices[2]].p.v[axis] >= limit)) {
                    rejected = 1;
                    break;
                }
                limit = bnds->real_bounds.max.v[axis];
                if (prepared_group->vertices[indices[0]].p.v[axis] > limit
                    && prepared_group->vertices[indices[1]].p.v[axis] > limit
                    && prepared_group->vertices[indices[2]].p.v[axis] > limit) {
                    rejected = 1;
                    break;
                }
            }
            if (rejected) {
                continue;
            }
            for (vertex = 0; vertex < 3; vertex++) {
                for (component = 0; component < 3; component++) {
                    polygon[vertex + 1].v[component] = model->prepared->groups[group].vertices[indices[vertex]].p.v[component] - bnds->mat->m[3][component];
                }
            }
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
            for (vertex = 0; vertex < 3; vertex++) {
                if (pMat != NULL) {
                    BrMatrix34ApplyP(&face_list->v[vertex], &model->prepared->groups[group].vertices[indices[vertex]].p, pMat);
                } else {
                    for (component = 0; component < 3; component++) {
                        face_list->v[vertex].v[component] = model->prepared->groups[group].vertices[indices[vertex]].p.v[component];
                    }
                }
            }
            for (component = 0; component < 3; component++) {
                if (pMat != NULL) {
                    normal.v[component] = face->eqn.v[component];
                } else {
                    face_list->normal.v[component] = face->eqn.v[component];
                }
            }
            if (pMat != NULL) {
                BrMatrix34ApplyV(&face_list->normal, &normal, pMat);
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
                for (vertex = 0; vertex < 3; vertex++) {
                    face_list->map[vertex] = &model->vertices[model->faces[original_face].vertices[vertex]].map;
                }
            } else {
                face_list->g = (tS16)group;
                face_list->f = (tS16)f;
                face_list->flags = 0;
                for (vertex = 0; vertex < 3; vertex++) {
                    if (prepared->groups[group].faces[f].edges[vertex] == 0) {
                        face_list->flags |= 1 << vertex;
                    }
                }
                for (vertex = 0; vertex < 3; vertex++) {
                    face_list->map[vertex] = &model->prepared->groups[group].vertices[indices[vertex]].map;
                }
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
            return 0;
        }
    }
    return max_face;
}
