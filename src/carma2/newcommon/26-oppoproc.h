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

// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// SetMaxSpeedFromSOCs

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

extern tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle);
// FollowCheatyPath


extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pSection);

#endif // GUARD_26_OPPOPROC_H
