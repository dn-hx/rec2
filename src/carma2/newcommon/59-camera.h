#ifndef GUARD_59_CAMERA_H
#define GUARD_59_CAMERA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCamera_frozen;

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

// ReallyViewOpponent

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);
// SetCameraType

// ChangeCameraType

// ToggleCockpit

// PositionCarMountedCamera

// PositionExternalCamera

// CameraBugFix

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

// IncidentCam

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

// SaveCameraPosition

// RestoreCameraPosition

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

// CollideCameraWithOtherCars

// InitialiseExternalCamera

extern void C2_HOOK_FASTCALL FreezeCamera(void);

// GetAverageGridPosition

// FindACar

// FindAPed

// MouseClickInActionReplay

#endif // GUARD_59_CAMERA_H
