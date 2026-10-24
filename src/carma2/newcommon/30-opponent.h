#ifndef GUARD_30_OPPONENT_H
#define GUARD_30_OPPONENT_H

#include "rec2_types.h"

extern float gOpponent_nastyness_frigger;
extern int gMinTimeOpponentRepair;
extern int gMaxTimeOpponentRepair;
extern int gNumber_of_cops_before_faffage;
extern int gActive_car_list_rebuild_required;
extern br_scalar gIn_view_distance;
extern tU8* gBit_per_node;
extern float gOppo_render_zero;

extern void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...);

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

extern int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo);
// WeightedFindNearestNodeAndSection

// FindNearestGeneralSection

// FindNearestPathSection

// DeadStopCar

extern void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL TurnOpponentPhysicsOff(tOpponent_spec* pOpponent_spec);
// ApplyOppoRenderage2

// NewObjective

// CalcRaceRoute

// TopUpRandomRoute

// SearchForSection

// UpdatePlayersSection

// AllowForDecel

extern int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces);
// NewCalcGetNearPlayerRoute

// CalcReturnToStartPointRoute

// ClearOpponentsProjectedRoute

extern int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection);
// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

extern void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
// ProcessReturnToStart

// ProcessLevitate

// ProcessGetNearPlayer

// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

// ObjectiveComplete

// TeleportOpponentToNearestSafeLocation

// ChooseNewObjective

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// AddIfNotInList

extern void C2_HOOK_FASTCALL SetOppoRender(tOpponent_spec* pOpponent_spec, int pRender);

extern void C2_HOOK_FASTCALL RemoveAnythingStillInList(tPhysics_object** pList, int pCount);
extern void C2_HOOK_FASTCALL RebuildActiveCarList(void);

extern void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);

// OpponentRepairNecessary

// PossiblyRepairOpponent

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

// CalcDistanceFromHome

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

extern void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec);
// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

extern void C2_HOOK_FASTCALL LoadCopCars(tRace_info* pRace_info);

// LoadInOppoPaths

// DisposeOpponentPaths

// MungeOpponents

// InitOpponents

// DisposeOpponents

// WakeUpOpponentsToTheFactThatTheStartHasBeenJumped

extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

// GetDriverName

// GetOpponentSpecFromCarSpec

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex);

extern int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no);

extern int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec);

// GetOpponentsNextSection

// GetOpponentsSectionStartNodePoint

// GetOpponentsSectionFinishNodePoint

// GetOpponentsSectionWidth

// GetOpponentsSectionMinSpeed

// GetOpponentsSectionMaxSpeed

extern void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

// ClearTwattageOccurrenceVariables

extern void C2_HOOK_FASTCALL TwoCarsHitEachOther(tCar_spec* pA_car, tCar_spec* pAnother_car);

// RecordOpponentTwattageOccurrence

// GetOpponentMood

#endif // GUARD_30_OPPONENT_H
