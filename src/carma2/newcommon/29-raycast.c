#include "29-raycast.h"

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

// PickBoundsTestRay

// FUNCTION: CARMA2_HW 0x004e3840
int C2_HOOK_FASTCALL ActorPick2D(br_actor* ap, br_model* model, br_material* material, dr_pick2d_cbfn* callback, void* arg) {
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
    m_to_v = gPick_model_to_view__raycast;

    BrMatrix34PreTransform(&gPick_model_to_view__raycast, &ap->t);
    switch (ap->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
            if (PickBoundsTestRay__raycast(&this_model->bounds,
                    (br_vector3 *) v_to_m.m[3], (br_vector3 *) v_to_m.m[2],
                    0.f, BR_SCALAR_MAX, &t_near, &t_far) != 0) {
                BrVector3Negate(&dir, (br_vector3 *) v_to_m.m[2]);
                r = callback(
                        ap, this_model, this_material, (br_vector3 *) v_to_m.m[3],
                        &dir, t_near, t_far, arg);
                if (r != 0) {
                    goto done;
                }
            }
            if (r != 0) {
                goto done;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
        if (PickBoundsTestRay__raycast((br_bounds*)ap->type_data,
                (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2],
                0.0f, BR_SCALAR_MAX, &t_near, &t_far) != 0) {
            for (a = ap->children; a != NULL; a = a->next) {
                r = ActorPick2D(a, this_model, this_material, callback, arg);
                if (r != 0) {
                    break;
                }
            }
        }
        goto done;
    }
    for (a = ap->children; a != NULL; a = a->next) {
        r = ActorPick2D(a, this_model, this_material, callback, arg);
        if (r != 0) {
            break;
        }
    }
done:
    gPick_model_to_view__raycast = m_to_v;
    return r;
}

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

// FUNCTION: CARMA2_HW 0x004e3a30
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

// FindYVerticallyBelowCallBack

// FindYVerticallyBelow

// FindYVerticallyBelow2

