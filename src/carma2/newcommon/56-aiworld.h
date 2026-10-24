#ifndef GUARD_56_AIWORLD_H
#define GUARD_56_AIWORLD_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gFirst_drone_processing;
extern tU32 gFrame_period_for_this_munging;
extern float gDrone_delta_time;
extern float gTrack_drone_min_y;

// MakeVertexAndOffsetIt

// MakeFaceAndTextureIt

extern void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

// LoadAIWorldTrackInfo

// LoadAIWorldForRace

// InitialiseAIWorldForRace

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

// MungeAIWorld

// PointActorAlongThisBloodyVector

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

extern void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info);

#endif // GUARD_56_AIWORLD_H
