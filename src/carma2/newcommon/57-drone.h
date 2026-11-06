/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_57_DRONE_H
#define GUARD_57_DRONE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern br_vector3 gRender_bounds_centre;
extern int gINT_00681fb0;
extern int gFrame;
extern int gCurrent_selected_drone;

// gCount_rendered_drones
extern int gCount_drones;
extern tDrone_form gDrone_forms[64];
extern int gCount_drone_forms;
extern int gCount_drone_path_nodes;
extern tDrone_spec* gDrone_specs;
extern tDrone_path_node* gDrone_path_nodes;
extern int gCount_drones;
extern int gCurrent_selected_drone;

// DoNotDprintf

// CrappyLittleVector3DPrintf

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name);

// LoadDroneTypeInfo

extern void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone);

// InitDroneCollisionObject

// ReallyAddDroneToPHIL

// AddDroneToPHIL

// PreprocessDronePaths

extern int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData);

// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

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

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

extern int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

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

extern int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

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

extern void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

// InitDrones

extern void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void);

extern void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void);

// GetDroneCrushData

// AllocateAndInitDrones

extern void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

extern void C2_HOOK_FASTCALL DronePreCollisionStuff(void);

// DoDefaultDroneStateAction

// DoDronePerGameFrameStuff

extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);

// MyDroneHathCollideth

extern int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject);

// MoveMyDroneBaby

extern void C2_HOOK_FASTCALL DroneLookWhereYoureGoingYouBlindCunt(void);

extern void C2_HOOK_FASTCALL SelectNextDrone(void);

extern int C2_HOOK_FASTCALL OKToViewDrones(void);

extern br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void);

// GetCurrentViewDroneDirection

// ApplyDroneRender2

extern void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int pDrone_id, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance);

extern void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int pDrone_id, tU16 pSpeed, tU16 pField_0x74, tU16 pRadius, const tCompressed_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pCount_faces, int pCount_vertices);

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

extern void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...);

extern float gDrone_render_zero;

// gDefault_drone_direction

extern tDrone_state_function* gDrone_state_functions[6];

extern tU32 gINT_00705b70;

typedef struct {
    br_model* model;
    float crushability;
    br_bounds3 bounds;
} tDrone_crush_data;


extern tDrone_crush_data gDrone_crush_data;

// gShow_drone_paths

#endif // GUARD_57_DRONE_H
