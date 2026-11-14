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

extern void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF);

extern void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

extern void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period);

extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

extern br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex);

// MakeAIEditCubeHere

extern void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pResult, br_vector3* pStart, br_vector3* pFinish, br_scalar pWidth);
extern void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2);
extern void C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info);


extern void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void);

extern void C2_HOOK_FASTCALL DisposeOpponents(void);

extern void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void);

extern void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pFaces, int pVertices);


extern void C2_HOOK_FASTCALL MungeShatMines(tU32 pTime);
extern void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF);
extern void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone_spec);
extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pObject1, tPhysics_object* pObject2);
extern void C2_HOOK_FASTCALL DamageUnit2(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude);
extern int C2_HOOK_FASTCALL MineExplode(tShit_mine* pMine);
extern int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void);
extern void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void);
extern void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce);
extern intptr_t C2_HOOK_CDECL FireOffBitsCB(br_actor* pActor, void* pArg);
extern int C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg);
extern void C2_HOOK_FASTCALL RandomlyXZSplitCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVert_index, int pFace_index, br_vector3* pPosition, br_material* pMaterial1, br_material* pMaterial2, br_material* pMaterial3);

#endif // GUARD_56_AIWORLD_H
