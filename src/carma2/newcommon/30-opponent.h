#ifndef GUARD_30_OPPONENT_H
#define GUARD_30_OPPONENT_H

#include "rec2_types.h"

extern int gBIG_APC_index;
extern float gOpponent_nastyness_frigger;
extern int gMinTimeOpponentRepair;
extern int gMaxTimeOpponentRepair;
extern int gNumber_of_cops_before_faffage;
extern int gActive_car_list_rebuild_required;
extern br_scalar gIn_view_distance;
extern tU8* gBit_per_node;
extern float gOppo_render_zero;

extern void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...);

extern void C2_HOOK_FASTCALL ProcessCurrentObjective(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
// ReallocExtraPathNodes

// ReallocExtraPathSections

extern int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo);
// WeightedFindNearestNodeAndSection

extern tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance);
// FindNearestPathSection

// DeadStopCar

extern void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL TurnOpponentPhysicsOff(tOpponent_spec* pOpponent_spec);
// ApplyOppoRenderage2

extern void C2_HOOK_CDECL NewObjective(tOpponent_spec* pOpponent_spec, tOpponent_objective_type pObjective_type, ...);
extern void C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add);
extern int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pTemp_store, tRoute_section* pPerm_store, int* pNum_of_perm_store_sections, tS16 pTarget_section, int pDepth, br_scalar pDistance_so_far, tOpponent_spec* pOpponent_spec);
// UpdatePlayersSection

// AllowForDecel

extern int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces);

extern void C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce);
extern void C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec);

// ClearOpponentsProjectedRoute

extern int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection);
// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

extern void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee);
// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

extern void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
// ProcessRunAway

extern void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);

extern void C2_HOOK_FASTCALL ProcessReturnToStart(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
extern void C2_HOOK_FASTCALL ProcessLevitate(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
extern void C2_HOOK_FASTCALL ProcessGetNearPlayer(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

extern void C2_HOOK_FASTCALL ObjectiveComplete(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL TeleportOpponentToNearestSafeLocation(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL ChooseNewObjective(tOpponent_spec* pOpponent_spec, int pMust_choose_one);
extern void C2_HOOK_FASTCALL ProcessThisOpponent(tOpponent_spec* pOpponent_spec);
// IsNetCarActive

// NoteCarsCurrentlyUsed

extern void C2_HOOK_FASTCALL AddIfNotInList(tPhysics_object* pObject, tPhysics_object** pList, int pCount);
extern void C2_HOOK_FASTCALL ApplyOppoRenderage2(tOpponent_spec* pOpponent_spec, int pRender);

extern void C2_HOOK_FASTCALL RemoveAnythingStillInList(tPhysics_object** pList, int pCount);

extern void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);

// OpponentRepairNecessary

// PossiblyRepairOpponent

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

extern void C2_HOOK_FASTCALL CalcDistanceFromHome(tOpponent_spec* pOpponent_spec);

// MassageOpponentPosition

extern int C2_HOOK_FASTCALL RematerialiseOpponentOnThisSection(tOpponent_spec* pOpponent_spec, tS16 pSection_no, br_scalar pSpeed);
extern int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed);

extern int C2_HOOK_FASTCALL RematerialiseOpponent(tOpponent_spec* pOpponent_spec, br_scalar pSpeed);
extern void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec);
// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

extern void C2_HOOK_FASTCALL LoadCopCars(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF);
extern void C2_HOOK_FASTCALL DisposeOpponentPaths(void);

extern void C2_HOOK_FASTCALL MungeOpponents(void);

extern void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL DisposeOpponents(void);

extern void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was);

extern void C2_HOOK_FASTCALL ReportMurderToPoliceDepartment(tCar_spec* pCar_spec);

extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

extern const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex);
extern tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex);

extern int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no);

extern int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pSection);

extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection);

extern int C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);

extern int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);

extern void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

// ClearTwattageOccurrenceVariables

extern void C2_HOOK_FASTCALL TwoCarsHitEachOther(tCar_spec* pA_car, tCar_spec* pAnother_car);

extern void C2_HOOK_FASTCALL RecordOpponentTwattageOccurrence(tCar_spec* pTwatter, tCar_spec* pTwattee);

// GetOpponentMood


extern void C2_HOOK_FASTCALL RebuildActiveCarList(void);
extern void C2_HOOK_FASTCALL PossiblyRepairOpponent(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL findfloor(br_vector3* pPos, br_vector3* pDir, br_vector3* pNormal, br_scalar* pDist);
extern void C2_HOOK_FASTCALL SetChallengerIndexIf(int pIndex, int pSet);

#endif // GUARD_30_OPPONENT_H
