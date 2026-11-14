#ifndef GUARD_57_DRONE_H
#define GUARD_57_DRONE_H

#include "c2_hooks.h"
#include "rec2_types.h"

typedef struct {
    br_model* model;
    float crushability;
    br_bounds3 bounds;
} tDrone_crush_data;

extern int gFrame;
extern tDrone_spec* gDrone_specs;
extern int gCurrent_selected_drone;
extern tDrone_path_node* gDrone_path_nodes;
extern tDrone_form gDrone_forms[64];
extern int gINT_00681fb0;
extern int gCount_rendered_drones;
extern br_vector3 gRender_bounds_centre;
extern tDrone_crush_data gDrone_crush_data;

extern void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition);

extern void C2_HOOK_FASTCALL DroneLookWhereYoureGoingYouBlindCunt(void);

extern void C2_HOOK_FASTCALL SelectNextDrone(void);
// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(const char* pDrone_name);
// LoadDroneTypeInfo

// ReallyRemoveDroneFromPHIL

// RemoveDroneFromPHIL

// InitDroneCollisionObject

extern int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec);

// AddDroneToPHIL

extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);
extern int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* data);
// LinkDroneActorsToModelsAndSetRenderStyle

extern void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone);

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

extern br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName);

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

extern void C2_HOOK_FASTCALL CalculateDrivingInfo(tDrone_spec* pDrone);
// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

extern int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

// SemiInitDroneSpec

extern void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode);
// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

extern void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec);

// StopProcessingThisDrone

extern void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);
extern int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone);
// StartProcessingThisDrone

extern int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

extern void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex);
extern void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state);
// InitDroneDrivingInfo

extern void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);
// PossiblyPipeDroneMovement

extern void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state);
// DoDroneHaltingStuff

extern void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state);
extern void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state);
// GetNewCrushyModelForDrone

extern void C2_HOOK_FASTCALL SetDronesOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

// InitDrones

// DisposeDronesRaceStuff

// LoadPerRaceDroneStuff

extern tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject);

// AllocateAndInitDrones

extern void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);
extern void C2_HOOK_FASTCALL DronePreCollisionStuff(void);

// DoDronePerGameFrameStuff

extern int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther);
extern int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2);

// DroneLookWhereYoureGoingYouBlindCunt

extern int C2_HOOK_FASTCALL OKToViewDrones(void);

extern br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void);
extern br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void);

extern void C2_HOOK_FASTCALL ApplyDroneRender2(int pIndex, int pRender);

// ApplyDroneStraightPos2

// ApplyDroneCornerPos2

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

extern void C2_HOOK_FASTCALL SaveDronePaths(void);

extern int C2_HOOK_FASTCALL AdvanceDroneSection(tDrone_spec* pDrone);
extern void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...);

#endif // GUARD_57_DRONE_H
