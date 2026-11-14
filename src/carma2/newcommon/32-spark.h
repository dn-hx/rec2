#ifndef GUARD_32_SPARK_H
#define GUARD_32_SPARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gShade_list[16];
extern int* gShade_table;
extern int gNum_dust_tables;
extern int gSpark_flags;
extern br_pixelmap* gIt_shade_table;
extern int gSmoke_flags;
extern int gSmoke_num;
extern tSmoke gSmoke[25];
extern br_material* gBlack_material;
extern tBRender_smoke* gBR_smoke_pointers[35];
extern int gN_BR_smoke_structs;
extern int gColumn_flags;
extern br_pixelmap* gFlame_map[20];
extern br_model* gLollipop_model;
extern tSmoke_column gSmoke_column[10];
extern br_matrix4 gCameraToScreen;
extern br_camera* gSpark_cam;
extern tSpark gSparks[32];
extern int gReplay_splash_flags;
extern tReplay_splash gReplay_splashes[10];
extern br_token_value* gDAT_00660148;

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

extern int C2_HOOK_FASTCALL DrawLine3D(const br_vector3* pStart, const br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_pixelmap* pShade);

extern int C2_HOOK_FASTCALL DrawLine2D(br_vector3* pStart, br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_scalar pBrightness, br_pixelmap* pShade);

extern void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol);

// ReplaySparks

extern void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);
// CreateSingleSpark

extern void C2_HOOK_FASTCALL CreateSparks(const br_vector3* pPosition, br_vector3* pVelocity, const br_vector3* pForce, br_scalar pStrength, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL CreateSparkShower(br_vector3* pPosition, br_vector3* pVelocity, br_vector3* pForce, tNon_car_spec* pOwner1, tNon_car_spec* pOwner2);
// AdjustSpark

// AdjustShrapnel

extern void C2_HOOK_FASTCALL ResetSparks(void);

extern void C2_HOOK_FASTCALL ResetShrapnel(void);

// CreateShrapnelShower2

extern void C2_HOOK_FASTCALL CreateShrapnelShowerWorld(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials);

extern void C2_HOOK_FASTCALL CreateShrapnelShower2(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials);
// random

// InitShrapnel

extern void C2_HOOK_FASTCALL LoadInShrapnel(void);
// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

extern intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar_spec);

extern int C2_HOOK_FASTCALL SetPixelmap(br_material* pMaterial);
extern void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DoSolidGraniteThing(tCar_spec* pCar);

// DoTrueColModelThing

// SetFoxModelCallback

extern int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel);

extern int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial);

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

extern int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial);

extern void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime);
extern void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);

extern int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2);

extern void C2_HOOK_FASTCALL InitSmokeStuff(void);

extern void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void);

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

extern void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

extern void C2_HOOK_FASTCALL ResetSmoke(void);

extern void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

// ActorError

extern void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);
extern void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime);
extern void C2_HOOK_FASTCALL GenerateSmokeShades(void);

extern void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void);

extern void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame_count, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

// ReplayFlame

extern void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime);
extern void C2_HOOK_FASTCALL DoSmokeColumn(int pColumn, tU32 pTime, br_vector3* pRet_car_pos);
// ReplaySmokeColumn

extern void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime);

extern void C2_HOOK_FASTCALL DisposeFlame(void);

extern void C2_HOOK_FASTCALL InitFlame(void);

extern void C2_HOOK_FASTCALL InitSplash(FILE* pF);
extern void C2_HOOK_FASTCALL DisposeSplash(void);

// DrawTheGlow

extern void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

extern int C2_HOOK_FASTCALL GetSmokeOn(void);

extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pLevel);

extern void C2_HOOK_FASTCALL SingleSplash(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pNormal_velocity, const br_vector3* pOmega, const br_vector3* pPoint, const br_vector3* pNormal, tU32 pTime);
extern void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime);

extern void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime);

extern void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime);
// MungeSplash

extern void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* pF);

extern void C2_HOOK_FASTCALL FreeSmokeShadeTables(void);

extern void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);
// DisposeKevStuff

extern void C2_HOOK_FASTCALL MakeCarIt(tCar_spec* pCar);

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

extern intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void* pContext);
extern intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext);
extern void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

extern int C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial);

extern void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial);

extern void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ClearSplashReplay(uintptr_t pObject);

extern void* C2_HOOK_FASTCALL GetSplashIndex(tPhysics_object* pObject);

// AdjustSplashReplay

extern void C2_HOOK_FASTCALL InitReplaySplashes(void);


extern int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3* pPos);
extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPos, br_vector3* pDir, br_vector3* pOut1, br_scalar* pDist, br_vector3* pOut2, tFace_ref* pFaces, int pCount);
extern void C2_HOOK_CDECL DoTrueColModelThing(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pArg4, br_uint_8 pArg5, int pArg6);
extern int C2_HOOK_FASTCALL SetFoxModelCallback(br_model* pModel);
extern int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial);
extern intptr_t C2_HOOK_CDECL RunCallbackOnAllModels(br_actor* pActor, void* pContext);
extern tU8* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner);

#endif // GUARD_32_SPARK_H
