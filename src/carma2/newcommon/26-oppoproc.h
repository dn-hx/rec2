#ifndef GUARD_26_OPPOPROC_H
#define GUARD_26_OPPOPROC_H

#include "rec2_types.h"

extern void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

extern int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec);
extern int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec);

// TimeToBeBrakingWhenStruggling

extern void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

extern int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);

// Distance2D

// SectionLength2D

// NearestSectionInStraight

extern void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* socs, int count_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorner, tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar);

// StuffDataFromCorner

// SetCurvature

extern tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle);
// FollowCheatyPath


extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pSection);

typedef struct {
    int count;
    int y;
    int current;
    int x[5];
} tFlic_cycle_spec;

extern tFlic_cycle_spec gFlic_cycle_specs[2];

extern int C2_HOOK_FASTCALL FUN_004b2430(int* pIndex);

extern int C2_HOOK_FASTCALL FUN_004b24f0(int* pIndex);

extern int C2_HOOK_FASTCALL FUN_004b2740(int* pIndex);

extern int C2_HOOK_FASTCALL RadioClick(int* pNew_choice, int* pNew_mode, int pX, int pY);

#endif // GUARD_26_OPPOPROC_H
