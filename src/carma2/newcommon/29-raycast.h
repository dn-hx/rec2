#ifndef GUARD_29_RAYCAST_H
#define GUARD_29_RAYCAST_H

#include "rec2_types.h"

extern br_actor* gY_picking_camera;
extern br_scalar gHighest_y_below;
extern br_material* gMaterial_below;
extern br_scalar gCurrent_y;
extern int gBelow_face_index;
extern br_model* gBelow_model;
extern br_scalar gLowest_y_above;
extern int gAbove_face_index;
extern br_model* gAbove_model;
extern br_matrix34 gPick_model_to_view__raycast;

// DRActorToRoot

extern void C2_HOOK_FASTCALL InitRayCasting(void);

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

extern int C2_HOOK_FASTCALL ActorPick2D(br_actor* ap, br_model* model, br_material* material, dr_pick2d_cbfn* callback, void* arg);

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

extern int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

// FindYVerticallyBelowCallBack

// FindYVerticallyBelow

// FindYVerticallyBelow2

#endif // GUARD_29_RAYCAST_H
