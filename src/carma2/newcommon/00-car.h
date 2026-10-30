#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

extern int gCunning_stunt_bonus[3];
extern float gArmour_starting_value[100];
extern int gExplosion_sound_id;
extern float gPower_starting_value[100];
extern float gOffensive_starting_value[100];
extern tFloat_bunch_info gCar_softness;
extern tFloat_bunch_info gCar_car_damage_multiplier;
extern br_vector3 gZero_v__car;
extern int gNum_active_non_cars;
extern int gFace_num__car;
extern int gInTheSea;
extern tU32 gQuite_wild_end;
extern tU32 gQuite_wild_start;
extern tU32 gOn_me_wheels_start;
extern tU32 gWoz_upside_down_at_all;
extern tU32 gWild_start;
extern int gNum_active_cars;
extern tCar_spec* gActive_car_list[139]; /* FIXME: uncertain about array length */  // GLOBAL: CARMA2_HW 0x0079ef2c int gNum_cars_and_non_cars;
extern int gFace_count;
extern tFace_ref gFace_list__car[300];
extern br_scalar gMin_world_y;
extern tPhysics_object* gUnknown_car_collision_info;

extern void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount);

extern intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

// InitialiseCar2

// InitialiseCar

extern void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race);

// SetInitialPosition

extern void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race);

// InitialiseNonCar

// NewFaceListCallBack

extern int C2_HOOK_FASTCALL IsCarInTheSea(void);
// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

// CalcGraphicalWheelStuff

// FinishCars

// GetNonCars

// GetNetPos

// MungeCarsMass

extern void C2_HOOK_FASTCALL AddDrag(tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDt);

extern void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pChild);

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// APTCPreCollision

// SetCollisionFlagsAndStuff

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

// ControlCar2

extern void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* pCar, void* pUnused, br_scalar pDt);

// ControlCar4

// ControlCar5

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

extern void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car);

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

// CalcForce

// DoRevs

// ScrapeNoise

// SkidNoise

// StopSkid

// CrashNoise

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// ProcessForcesCallBack

// ProcessJointForcesCallBack

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

extern int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3);

extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

// findfloor

// FindFloorInBoxBU

extern void C2_HOOK_FASTCALL CancelPendingCunningStunt(void);
// SetAmbientPratCam

// SetTextureBits

// MungeSomeOtherCarGraphics

extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);

extern intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData);
// DoLODCarModels

extern void C2_HOOK_FASTCALL DoComplexCarModels(void);
// ResetCarScreens

extern void C2_HOOK_FASTCALL AdjustDownForce(void);

// FlyCar

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

// PullActorFromWorld

// DoPullActorFromWorld

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

extern void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs);
extern void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex);

extern void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index);

extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag);

#endif // GUARD_00_CAR_H
