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

#endif // GUARD_29_RAYCAST_H
