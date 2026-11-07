#ifndef GUARD_07_STRUCTUR_H
#define GUARD_07_STRUCTUR_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tRace_over_reason gRace_over_reason;
extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted3;
extern int gCredits_checkpoint[3];
extern int gLast_wrong_checkpoint;
extern tU32 gLast_checkpoint_time;

extern int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);

extern void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound);

extern void C2_HOOK_FASTCALL IncrementCheckpoint(void);

extern void C2_HOOK_FASTCALL IncrementLap(void);

// RayHitFace

extern void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index);
extern void C2_HOOK_FASTCALL CheckCheckpoints(void);
extern void C2_HOOK_FASTCALL TotalRepair(void);

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

extern int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness);

extern void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pThe_race);
extern int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);

// SwapNetCarsLoad

// SwapNetCarsDispose

extern int C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result);

// DoGame

// InitialiseProgramState

extern void C2_HOOK_FASTCALL DoProgram(void);

extern void C2_HOOK_FASTCALL JumpTheStart(void);
extern void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);
extern void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);

extern void C2_HOOK_FASTCALL ResetCrushSystems(void);
extern void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);
extern void C2_HOOK_FASTCALL ResetPowerups(void);
extern void C2_HOOK_FASTCALL InitPlayers(void);
extern void C2_HOOK_FASTCALL InitRace(void);
extern void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void);
extern void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info);

#endif // GUARD_07_STRUCTUR_H
