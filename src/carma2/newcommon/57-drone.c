#include "57-drone.h"
#include "globvars.h"
#include "52-errors.h"
#include "08-loading1.h"
#include "41-utility.h"
#include "c2_string.h"
#include "platform.h"
#include "23-piping.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x00684518
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00684514
int gFrame;

// GLOBAL: CARMA2_HW 0x00684504
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x006844f8
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x00684508
tDrone_path_node* gDrone_path_nodes;

// GLOBAL: CARMA2_HW 0x00682178
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x00681fb0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684500
int gCount_rendered_drones;

/* separate 0.0f in .rdata used only here (not the shared 0x0058a988 literal) */
// GLOBAL: CARMA2_HW 0x00589dc4
float gDrone_render_zero = 0.0f;

// FUNCTION: CARMA2_HW 0x0044cfc0
static void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...) {

}

// FUNCTION: CARMA2_HW 0x00451620
void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}
// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

// FUNCTION: CARMA2_HW 0x0044f640
FILE* C2_HOOK_FASTCALL OpenDroneFile(const char* pDrone_name) {
    tPath_name the_path;
    FILE* f;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    PathCat(the_path, the_path, pDrone_name);
    strcat(the_path, ".TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    return f;
}
// LoadDroneTypeInfo

// ReallyRemoveDroneFromPHIL

// RemoveDroneFromPHIL

// InitDroneCollisionObject

/* local declarations (no new includes/externs; 60-phil.h is not included here) */
extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);


// AddDroneToPHIL

// PreprocessDronePaths

// LinkyCallback

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

// FUNCTION: CARMA2_HW 0x0044ecf0
void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on) {

    gTraffic_disabled = !pTraffic_on;
}

// FUNCTION: CARMA2_HW 0x0044ed00
int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void) {
    return !gTraffic_disabled;
}

// STUB: CARMA2_HW 0x0044ed10
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    NOT_IMPLEMENTED();
}

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

// FUNCTION: CARMA2_HW 0x00452680
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

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

