#ifndef GUARD_56_AIWORLD_H
#define GUARD_56_AIWORLD_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gFirst_drone_processing;
extern tU32 gFrame_period_for_this_munging;
extern float gDrone_delta_time;
extern float gTrack_drone_min_y;

#include "brender/brender.h"

// MakeVertexAndOffsetIt

extern void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

// LoadAIWorldTrackInfo

extern void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

// MungeAIWorld

extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

extern br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex);

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

extern void C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void);

extern void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info);

#endif // GUARD_56_AIWORLD_H
