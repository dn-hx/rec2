#ifndef GUARD_59_CAMERA_H
#define GUARD_59_CAMERA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCamera_frozen;
extern br_actor* gPed_actor;

extern float gCamera_cockpit_hither;
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern tSave_camera gSave_camera[2];
extern br_scalar gCamera_zoom;

// DoCameraControls  (no own address: inlined into GeneralisedPositionExternalCamera 0x00410c60)

extern void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime);
// CheckCameraHither

extern tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void);
// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

extern void C2_HOOK_FASTCALL ViewOpponent(void);
extern void C2_HOOK_FASTCALL ReallyViewOpponent(void);

extern void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar);

// SetPanningFieldOfView

// IsCameraTypeAllowed

extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);
extern void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

extern void C2_HOOK_FASTCALL ChangeCameraType(void);

extern void C2_HOOK_FASTCALL ToggleCockpit(void);
extern void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime);
extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

extern int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime);

// MoveCamToIncident

extern void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime);
// CheckForWall

extern void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SaveCameraPosition(int i);
extern void C2_HOOK_FASTCALL RestoreCameraPosition(int i);
// DrVector3RotateY

extern void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference);
// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels  (no own address: inlined into GeneralisedPositionExternalCamera 0x00410c60)

extern void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor);
// PointCamera

extern int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tPhysics_object* collision_info);
extern int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos);

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL FreezeCamera(void);

extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

// FindACar

// FindAPed

// MouseClickInActionReplay


extern int C2_HOOK_FASTCALL GetNextIncident(tS32 pOffset, tS32* pMode, br_scalar* pTowards, undefined4* pData, tS32* pTime);
extern void C2_HOOK_FASTCALL PointCamera(br_vector3* pPos, br_matrix34* pMat);

#endif // GUARD_59_CAMERA_H
