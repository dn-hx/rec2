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

typedef struct {
    tU16 v[4];
} tFixed_quat;

extern void C2_HOOK_FASTCALL LPMatrixToFixedQuat(tFixed_quat* pOrientation, const br_matrix34* pMatrix);

// FixedQuatFractionToLPMatrix

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

extern int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, tU32 pSize);

extern int C2_HOOK_STDCALL FloatToFixedFraction(float pVal, float pMin, float pMax);

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);

// FindObject

// WriteHierarchyNetworkStuff

extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data);
extern int C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix);
extern int C2_HOOK_FASTCALL PHILInit(void);

extern void C2_HOOK_FASTCALL PHILDisable(void);

extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject);

extern tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);

extern int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);
extern int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object *pCollision_info, int pParam);
// FlushQueuedAddsAndRemoves

extern void C2_HOOK_FASTCALL ChangedObjectsCallbacks(tPhysics_object* pObjects, tPhysics_callbacks* pCallbacks, tU32 pPeriod);
extern void C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pDrag, int pObject_info_flags, tSpecial_volume* pSpecial_volume);
// ProcessDrag

extern void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject);

// SetStandardGravity

extern void C2_HOOK_FASTCALL ProcessGravity(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pGravity);
extern void C2_HOOK_FASTCALL LevelOutOnSurface(tPhysics_object* pObject);
// PHILMungeObjects

extern void C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* pObject, tPhysics_object* pActivator);
// PHILInterpolateObjects

int C2_HOOK_FASTCALL PHILDoPhysics(tPhysics_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period);

// PHILApplyPHILObject


extern void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* pWorld_callbacks);
extern void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList);
extern int C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);

extern void C2_HOOK_FASTCALL InterpolateSingleObject(tPhysics_object* pObject, float pDt);
extern void C2_HOOK_FAKE_THISCALL InterpolateObjects(tPhysics_object* pObject, undefined4 arg2, float pDt);

#endif // GUARD_60_PHIL_H
