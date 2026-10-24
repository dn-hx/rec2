#ifndef GUARD_60_PHIL_H
#define GUARD_60_PHIL_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPHIL_disabled;
extern tPhysics_object* gPHIL_list_collision_infos;
extern int gCrush_deferred;
extern int gPHIL_count_list_collision_infos;
extern int gPHIL_count_queued_objects;
extern int gPHIL_count_queued_objects_for_removal;
extern int gPHIL_mechanics_time_sync;
extern int gPHIL_munging_objects;
extern int gPHIL_object_added;
extern tPHIL_queued_objects gPhil_queued_objects;

// ScalarToFraction

// FractionToScalar

extern void C2_HOOK_FASTCALL GetOrientationFromMatrix(tFixed_quat* pOrientation, const br_matrix34* pMatrix);

// GetMatrixFromOrientation

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

extern int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, tU32 pSize);

extern int C2_HOOK_STDCALL QuantizeFloatToInt(float pVal, float pMin, float pMax);

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);

// FindObject

// WriteHierarchyNetworkStuff

// GetHierarchyNetworkSize

// GetNetworkDataSize

// GetSingleMatrixFromNetworkData

extern int C2_HOOK_FASTCALL PHILInit(void);

extern void C2_HOOK_FASTCALL PHILDisable(void);

// PHILAddObject

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject);

extern tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);

extern int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix);

// PHILAddActiveObject

extern int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject);

// PHILAddActiveObjectImmediately

// PHILSetObjectProperty

extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object *pCollision_info, int pParam);
// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

extern void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject);

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

// PHILApplyPHILObject

#endif // GUARD_60_PHIL_H
