#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU32 field_0x8;
} tNet_message_chunk_crush_reattach_bit;

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

extern void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference);
extern void C2_HOOK_FASTCALL CalcEngineForce(tCar_spec* pCar, br_scalar pDt);
// PrepareCars

extern void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime);
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

extern void C2_HOOK_FASTCALL APTCPreCollision(void);
extern void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL APTCPostCollision(void);
extern int C2_HOOK_FASTCALL APTCActiveHalted(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL APTCPassiveActivated(tPhysics_object* pObject, undefined4 pArg2);

extern void C2_HOOK_FASTCALL APTCChangedObjects(tPhysics_object* pObject, undefined4 pArg2);

extern void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 pLast_tick_time, tU32 pFrame_period);
extern void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt);
extern void C2_HOOK_FASTCALL ControlCar2(tCar_spec* pCar, void* pUnused, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar3(tCar_spec* pCar, void* pUnused, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar4(tCar_spec* pCar, void* pUnused, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar5(tCar_spec* pCar, void* pUnused, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar1(tCar_spec* pCar, void* pUnused, br_scalar pDt);

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

extern void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car);

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

extern void C2_HOOK_FASTCALL ConditionallyNoteSkid(tCar_spec* pCar, tFace_ref** faces, int pIndex);
// NudgeObject

extern void C2_HOOK_FASTCALL CalcForce(tCar_spec* pCar, br_scalar pDt);
// DoRevs

// ScrapeNoise

extern void C2_HOOK_FASTCALL SkidNoise(tCar_spec* pCar, int pSide, br_scalar pV, int pMaterial_index);
// StopSkid

typedef struct tCollision_contact {
    float impulse;
    tPhysics_object* object1;
    tPhysics_object* object2;
    void* field_0xc;
    float field_0x10;
    br_vector3 r1;
    br_vector3 r2;
    br_vector3 normal;
    br_vector3 field_0x38;
    br_vector3 impulse1;
    br_vector3 impulse2;
    int type;
} tCollision_contact;

extern int C2_HOOK_FASTCALL ProcessForcesCallback(tCollision_contact* pContacts, float* pImpulses, int pCount);

extern void C2_HOOK_FASTCALL FUN_004d2320(tPhysics_object* pObject1, tPhysics_object* pObject2, tFace_ref* pFace, float pImpulse, br_vector3* pPoint);

extern void C2_HOOK_FASTCALL CrashNoise(br_vector3* pForce, br_vector3* pPosition, int pMaterial);

extern void C2_HOOK_FASTCALL CrushAndDamageCar(tCar_spec* pCar1, br_vector3* pPoint, br_vector3* pForce, tCar_spec* pCar2, int pChain_count, br_vector3* pChain, br_actor* pActor);
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

extern void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void);
extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);

extern intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData);
extern void C2_HOOK_FASTCALL DoLODCarModels(void);
extern void C2_HOOK_FASTCALL DoComplexCarModels(void);
// ResetCarScreens

extern void C2_HOOK_FASTCALL AdjustDownForce(void);

extern void C2_HOOK_FASTCALL FlyCar(tCar_spec* c, br_scalar dt);
// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

extern tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor);
extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
extern int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data);
// PipeNonCars

extern void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime);
extern void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs);
extern void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex);

extern void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index);

extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag);


extern float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, tU32 pDelta, float* pOut);
extern void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL WeldCarPartial(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL UpdateCrushVertices(tCar_spec* pCar, tU8* pData);
extern void C2_HOOK_FASTCALL UpdateCrushPainList(void);
extern void C2_HOOK_FASTCALL APTCPostCollisionTree(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2);
extern void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);
extern void C2_HOOK_FASTCALL PhysicsObjectApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque);
extern void C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, float pImpulse);
extern br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees);
extern void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL SwapCar(void);
extern void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index);
extern void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime);
extern void C2_HOOK_FASTCALL PipeSingleNonCarCreation(uintptr_t pId, br_actor* pActor);
extern void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_0043fb70(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, int pCars_touch);
extern void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick);
extern int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);
extern void C2_HOOK_FASTCALL ApplyDiminishingTorque(tPhysics_object* pObject, const br_vector3* pV);
extern void C2_HOOK_FASTCALL UpdateChildTransforms(tPhysics_object* pObject, br_matrix34* pParent_matrix);
extern int C2_HOOK_FASTCALL FUN_00420310(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL FUN_00420360(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL MoveNonCar(tNon_car_spec* pNon_car, br_scalar pDt);
extern void C2_HOOK_FASTCALL SmoothCarCrushShapePoints(tCar_spec* pCar, float pStep);
extern void C2_HOOK_FASTCALL FUN_0043b250(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_0043b2a0(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ApplyNonCarTumble(tPhysics_object* pObject, br_vector3* pV, br_scalar pTumbleFactor, br_scalar pTumbleThreshold);
extern int C2_HOOK_FASTCALL ScaleCollisionImpulses(tCollision_contact* pContacts, float* pImpulses, int pCount, int* pSmash_indices, int pNum_impacts, float pMax_impulse);
extern int C2_HOOK_FASTCALL FullyDetachSnappedBits(tCollision_contact* pContacts, float* pImpulses, int pCount);
extern int C2_HOOK_FASTCALL TrySplitCarOnVerticalForce(tCollision_contact* pContacts, float* pImpulses, int pCount);
extern int C2_HOOK_FASTCALL FUN_00430e30(tCollision_contact* pContacts, float* pImpulses, int pCount);
extern void C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);
extern int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(int pMagnitude, int pDirection);
extern void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time);
extern void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time);
extern void C2_HOOK_FASTCALL CompressCarDataChunk(tNet_message_chunk* pMessage, int pPlayer_index);
extern int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SendLocalCarData(tU32 pNext_frame_time);
extern void C2_HOOK_FASTCALL CycleControlCar(void);
extern void C2_HOOK_FASTCALL PutOpponentsInNeutral(void);
extern int C2_HOOK_FASTCALL GetCompressedCrushVertexDataSize(tU8* pData);
extern void C2_HOOK_FASTCALL ReceivedCrushReattachBit(tNet_message_chunk_crush_reattach_bit* pContents);
extern void C2_HOOK_FASTCALL FUN_00498ab0(tCar_spec* pCar, tU8* pContents);
extern void C2_HOOK_FASTCALL ReceivedCarData(tU8* pContents);

#endif // GUARD_00_CAR_H
