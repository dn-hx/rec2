#ifndef GUARD_57_DRONE_H
#define GUARD_57_DRONE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gFrame;
extern tDrone_spec* gDrone_specs;
extern int gCurrent_selected_drone;
extern tDrone_path_node* gDrone_path_nodes;
extern tDrone_form gDrone_forms[64];
extern int gINT_00681fb0;
extern int gCount_rendered_drones;
extern br_vector3 gRender_bounds_centre;

extern void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition);
// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(const char* pDrone_name);
// LoadDroneTypeInfo

// ReallyRemoveDroneFromPHIL

// RemoveDroneFromPHIL

// InitDroneCollisionObject

// ReallyAddDroneToPHIL

// AddDroneToPHIL

// PreprocessDronePaths

extern int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* data);
// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

// MrFindy

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

// DroneCarWithinRenderingDistance

// DronePlaneWithinRenderingDistance

// DroneTrainWithinRenderingDistance

// SemiInitDroneSpec

// InitDroneSpec

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// StopRenderingThisDrone

// StopProcessingThisDrone

// StartRenderingThisDrone

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

// DroneCarWithinProcessingDistance

// DronePlaneWithinProcessingDistance

// DroneTrainWithinProcessingDistance

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

// PipeDroneMatrix

// PossiblyPipeDroneMovement

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

extern void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

// InitDrones

// DisposeDronesRaceStuff

// LoadPerRaceDroneStuff

// GetDroneCrushData

// AllocateAndInitDrones

// LoadInDronePaths

// DronePreCollisionStuff

// DoDronePerGameFrameStuff

// MyDroneHathCollideth

// MyDroneHathHalteth

// MoveMyDroneBaby

// DroneLookWhereYoureGoingYouBlindCunt

extern int C2_HOOK_FASTCALL OKToViewDrones(void);

// GetCurrentViewDroneMat

// GetCurrentViewDroneDirection

// ApplyDroneRender2

// ApplyDroneStraightPos2

// ApplyDroneCornerPos2

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

#endif // GUARD_57_DRONE_H
