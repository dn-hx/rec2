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

// ActorPick2D

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

// FindYVerticallyBelowPolyCallBack

// FindYVerticallyBelowCallBack

// FindYVerticallyBelow

// FindYVerticallyBelow2

