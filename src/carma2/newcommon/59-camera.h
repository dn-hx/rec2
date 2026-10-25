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

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// GetRaceLeader

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// ViewOpponent

extern void C2_HOOK_FASTCALL ReallyViewOpponent(void);

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);
extern void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

// ChangeCameraType

// ToggleCockpit

// PositionCarMountedCamera

extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

extern int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime);

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

extern void C2_HOOK_FASTCALL SaveCameraPosition(int i);
extern void C2_HOOK_FASTCALL RestoreCameraPosition(int i);
// DrVector3RotateY

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

// PointCameraAtCar

// PointCamera

// CollideCamera2

extern int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos);

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL FreezeCamera(void);

extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

// FindACar

// FindAPed

// MouseClickInActionReplay

#endif // GUARD_59_CAMERA_H
