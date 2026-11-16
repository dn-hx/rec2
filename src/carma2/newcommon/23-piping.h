/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_23_PIPING_H
#define GUARD_23_PIPING_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include <s3/s3.h>

typedef void (C2_HOOK_FASTCALL * tReplay_between_sessions_callback)(int pFrames);

/* Replay pipe chunk layouts (kept out of rec2_types.h: adding declarations there perturbs MSVC codegen of unrelated functions) */
typedef struct {
    tU32 value;
} tPipe_stack_word;

typedef struct {
    int field_0x0;
    tU16 field_0x4;
} tPipe_chunk_funk_enable;

typedef struct {
    uintptr_t pOwner;
    tU8 type_from;
    tU8 type_to;
} tPipe_chunk_transform_type;

typedef struct {
    int field_0x0;
    int field_0x4;
    tU16 field_0x8;
    tU16 field_0xa;
} tPipe_chunk_drone_straight_pos;

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
    tU32 count;
} tPipe_chunk_remove_faces;

typedef struct {
    int field_0x0;
} tPipe_chunk_non_car_creation;

typedef struct {
    int field_0x0;
    int field_0x4;
    br_vector3 field_0x8;
} tPipe_chunk_non_car_replay;

typedef struct {
    int field_0x0;
    tU32 field_0x4;
} tPipe_chunk_frame_bound;

typedef struct {
    br_vector3* p_vector;
    br_vector3 delta;
} tPipe_chunk_vector3;

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
    undefined4 field_0x8;
} tPipe_chunk_vanish;

typedef struct {
    int field_0x0;
    int field_0x4;
} tPipe_chunk_powerup;

typedef struct {
    int field_0x0;
    int field_0x4;
    tU32 field_0x8;
} tPipe_chunk_groove_on_off;

typedef struct {
    uintptr_t pOwner;
    undefined4 field_0x4;
    int field_0x8;
    br_vector3 field_0xc;
    br_vector3 field_0x18;
    br_vector3 field_0x24;
} tPipe_chunk_blood_spurt;

typedef struct {
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    int field_0x10;
    br_vector3 field_0x14;
    br_vector3 field_0x20;
    br_vector3 field_0x2c;
} tPipe_chunk_gib_shower;

typedef struct {
    int field_0x0;
    br_matrix34 field_0x4;
} tPipe_chunk_phil_object;

typedef struct {
    int field_0x0;
    tS16 field_0x4;
    tS16 field_0x6;
    tS16 field_0x8;
    tS16 field_0xa;
    int field_0xc;
} tPipe_chunk_ped_model_change;

typedef struct {
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    br_vector3 field_0x10;
} tPipe_chunk_ped_pos;

typedef struct {
    int field_0x0;
    int field_0x4;
    int field_0x8;
} tPipe_chunk_ped_fp_change;

typedef struct {
    int field_0x0;
    tS16 field_0x4;
    tU8 field_0x6;
    tS8 field_0x7;
} tPipe_chunk_ped_dismember;

typedef struct {
    int field_0x0;
    br_matrix34 field_0x4;
} tPipe_chunk_ped_physics;

typedef struct {
    int field_0x0;
    tS16 field_0x4;
    tS16 field_0x6;
    tU8 field_0x8;
    tU8 field_0x9;
    tS16 field_0xa;
    int field_0xc;
    int field_0x10;
    int field_0x14;
    br_vector3 field_0x18;
    br_vector3 field_0x24;
    int field_0x30;
    int field_0x34;
    br_vector3 field_0x38;
} tPipe_chunk_ped_move;

typedef struct {
    int field_0x0;
    br_vector3 field_0x4;
} tPipe_chunk_ped_dir;

typedef struct {
    int field_0x0;
    tU8 field_0x4[8];
    br_vector3 field_0xc;
    br_vector3 field_0x18;
    br_vector3 field_0x24;
} tPipe_chunk_ped_status;

typedef struct {
    tU8 pad[0x50];
    br_scalar x;
    br_scalar y;
    br_scalar z;
} tReplay_actor_trans_effect;

typedef struct {
    tU32 field_0x0;
    br_vector3 field_0x4;
    tU8 field_0x10[4];
    tU32 field_0x14;
} tPipe_chunk_split_weld;

typedef struct {
    undefined4 field_0x0;
    uintptr_t field_0x4;
    br_vector3 delta;
} tPipe_chunk_actor_trans;

typedef struct {
    int field_0x0;
    int field_0x4;
    tU8 field_0x8[4];
} tPipe_chunk_model_mash;

typedef struct {
    int skid_num;
    br_matrix34 matrix;
    br_material* material;
} tPipe_chunk_skid_adjust;

typedef struct {
    int field_0x0;
    tU16 field_0x4;
    tU16 field_0x6;
    int field_0x8;
} tPipe_chunk_prox_ray_replay;

typedef struct {
    int field_0x0;
    tS8 field_0x4[12];
} tPipe_chunk_damage;

typedef struct {
    int field_0x0;
    tS16 field_0x4;
    tS8 field_0x6;
    tS8 field_0x7;
    tS8 field_0x8;
    tS8 field_0x9;
} tPipe_chunk_graphical_wheel_stuff;

typedef struct {
    int field_0x0;
    tS32 field_0x4;
    float field_0x8;
    float field_0xc;
    float field_0x10;
    tU16 field_0x14;
    tU16 field_0x16;
} tPipe_chunk_sound;

typedef struct {
    int field_0x0;
    tU8 field_0x4;
    int field_0x8;
    br_vector3 field_0xc;
    br_vector3 field_0x18;
} tPipe_chunk_extended_splash;

typedef struct {
    uintptr_t pOwner;
    float v[4];
} tPipe_chunk_splash;

typedef struct {
    tU8 pad[0x244];
    br_scalar x;
    br_scalar y;
    br_scalar z;
    br_scalar size;
} tReplay_splash_effect;

typedef struct {
    tU32 field_0x0;
    tU8 pad[0x10];
    tU16 field_0x14;
} tPipe_chunk_smoke_column_replay;

typedef struct {
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    int field_0x10;
    br_vector3 field_0x14;
} tPipe_chunk_groove_stop;

typedef struct {
    int field_0x0;
    tU8 field_0x4[0xc];
    tU16 field_0x10;
    int field_0x14;
} tPipe_chunk_shrapnel_replay;

typedef struct {
    int field_0x0;
    tU8 field_0x4[0x10];
} tPipe_chunk_spark_replay;

typedef struct {
    int field_0x0;
    int field_0x4;
    tU8 field_0x8[0x10];
} tPipe_chunk_crush;


extern tU8* gLocal_buffer;
extern tU8* gPipe_buffer_working_end;
extern int gReentrancy_count;
extern tU8* gPipe_buffer_phys_end;
extern tU32 gLocal_buffer_size;
extern tU8* gMr_chunky;
extern tU8* gPipe_buffer_start;
extern int gPlay_direction;
extern int gDisable_piped_sounds;
extern tU32 gPipe_UNK_006768b0;

typedef void C2_HOOK_FASTCALL tReplay_session_callback(int pFrames);
extern float gReplay_rate;
extern const tReplay_callback* gPipe_callbacks;
extern tU8* gPipe_play_ptr;
extern tU8* gPipe_buffer_oldest;
extern tU8* gPipe_record_ptr;
extern tU32 gPipe_sound_start_time;
extern tU32 gPipe_sound_time_window;
extern int gDisable_advance;
extern tPipe_smudge_data* gCrush_space;
extern tU8* gEnd_of_session;

extern int C2_HOOK_FASTCALL ARReplayForwards(void);

extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

extern int C2_HOOK_FASTCALL ARReplayPauseState(void);

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);

extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);

extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);

extern void C2_HOOK_FASTCALL ARResetPiping(void);

// ARInitialise

extern void C2_HOOK_FASTCALL ARDispose(void);

extern int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType);

extern tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_session* pSession);

extern void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tU8** pChunk, int pType);

// PipingFrameReset

extern int C2_HOOK_FASTCALL ARPipeSearchForwards(void);

extern int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);

extern int C2_HOOK_FASTCALL ARSomeReplayLeft(void);

// DisablePipedSounds

// EnablePipedSounds

extern void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy);

extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);

extern void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);

extern void C2_HOOK_FASTCALL AREndPipingSession(void);

extern void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);

extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

extern void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);

extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARResetPipePlayToEnd

extern tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

extern void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr);

// ApplyPipedSession

extern int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pPtr);

extern int C2_HOOK_FASTCALL MoveSessionPointerForwardOne(tU8** pPtr);

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

extern int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime);

// ScanAndPlaySoundsToBe

extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);

extern void C2_HOOK_FASTCALL ARDisableAdvance(void);

extern void C2_HOOK_FASTCALL AREnableAdvance(void);

// FindNextChunk

extern void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

// ARDoActionReplay

extern void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag);

extern int C2_HOOK_FASTCALL ARGoBackwards(void);

extern int C2_HOOK_FASTCALL ARGoForwards(void);

extern void C2_HOOK_FASTCALL ARService(void);

extern void C2_HOOK_FASTCALL ARMainLoopStart(void);

extern int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

extern void C2_HOOK_FASTCALL ResetCars(void);

extern int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext);

// PipeCarPositions

extern void C2_HOOK_FASTCALL DoSmudge(tPipe_chunk** pChunk, int pDir);

// DoOilSpillAR

// CheckCar

// CarTimeout

// ScanCarsPositions

// InitialiseActionReplay

extern void C2_HOOK_FASTCALL DisposeActionReplay(void);

// CheckIncident

// GetNextIncident

extern void C2_HOOK_FASTCALL AddCrushToPipingSession(br_model* pModel, br_vector3* pSrc, const float* pScalars);

// AddSmudgeToPipingSession

extern void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);

extern void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial);

extern void C2_HOOK_FASTCALL AddNonCarToPipingSession(int pIndex, br_actor* pActor);

extern void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

extern void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

extern void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pObject, void* pSubject);

// AddCarToPipingSession

// AddDamageToPipingSession

extern void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime);

extern void C2_HOOK_FASTCALL AddFlapToPipingSession(br_actor* pActor, br_angle pAngle);

extern void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash);

extern void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime);

extern void C2_HOOK_FASTCALL AddNapalmBoltToPipingSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

extern void C2_HOOK_FASTCALL PipeSingleCrush(br_model* pModel, br_vector3* pSrc, const float* pScalars);

extern void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y);

extern void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption);

extern void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);

extern void C2_HOOK_FASTCALL PipeSingleFrameFinish(void);

extern void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

extern void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType);

// PipeSingleCarIncident

extern void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleWallIncident(float pSeverity, br_vector3* pImpact_point);

extern void C2_HOOK_FASTCALL PipeSingleProxRay(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime);

extern void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

extern void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle);

extern void C2_HOOK_FASTCALL PipeSingleRelink(br_actor* pParent, br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor);

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

extern void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pExplosion);

extern void C2_HOOK_FASTCALL PipeSingleNonCarCreation(int pIndex);

extern void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

// PipeSingleSmashRemoveFaces

extern void C2_HOOK_FASTCALL PipeSingleSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);

extern void C2_HOOK_FASTCALL PipeSingleRepulseRay(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime);

extern void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tU8 pArg7, tS8 pArg8, tU8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12);

extern void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir);

extern void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pOriginal_speed, float pSpeed, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat);

extern void C2_HOOK_FASTCALL PipeSinglePedDismember(tPedestrian* pPed, tU8 pBone_index, tS8 pOld_value, tS8 pNew_value);

extern void C2_HOOK_FASTCALL PipeSinglePedFPChange(tPedestrian* pPed, float pOld_speed, float pNew_speed);

extern void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3* pVelocity);

extern void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, int pBone_index, br_model* pOld_model, int pModel_index, int pOld_model_set, int pModel_set);

extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject);

// PipeSingleGibShower

extern void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

extern void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags);

extern void C2_HOOK_FASTCALL PipeSinglePowerupGot(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL PipeSinglePowerupLose(tPowerup* pPowerup, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags);

extern void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, tU32 pOld_value, tU32 pNew_value);

extern void C2_HOOK_FASTCALL PipeSingleDSModel(int pDouble_sided, void* pThing);

extern void C2_HOOK_FASTCALL PipeSinglePedDiagnostic(tPedestrian* pPed);

extern void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);

extern void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pRadius, tCompressed_matrix34* pMat);

extern void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance);

extern void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id);

// PipeSingleShitMine

extern void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

extern void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec* pOpponent_spec, int pEnable);

// ApplyCrush

extern void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk);

// ApplySpark

extern void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk);

// ApplyGrooveStop

extern void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk);

// ApplySmoke

extern void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk);

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

extern void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk);

// ApplySound

// ApplyCar

extern void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk);

// ApplyDamage

// ApplySpecial

extern void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk);

// ApplySkidAdjustment

extern void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk);

// ApplyRelink

extern void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk);

// ApplySplitWeld

extern void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk);

// ApplyShrapnelShower

extern void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk);

// ApplySmashRemoveFaces

extern void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk);

// ApplyActorTrans

// ApplyPedStatus

extern void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk);

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

extern void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk);

// ApplyPedPos

// ApplyPedModelChange

extern void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk);

// ApplyGibShower

// ApplyBloodSpurt

extern void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk);

// ApplyPedDiagnostics

extern void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk);

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

extern void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk);

// ApplyOppoRenderage

// UndoCrush

extern void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoDamage

// UndoSpecial

extern void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSplash

// UndoExtendedSplash

extern void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSplitWeld

extern void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoUnBend(void);

// UndoShrapnelShower

extern void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSmashRemoveFaces

extern void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

extern void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

extern void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// CalcCrushLength

extern int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcRemoveFacesLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL FUN_00564e5e(int pSound);

extern int C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, void* pCallback, void* pTime_check);

extern int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime);

extern int C2_HOOK_FASTCALL CarTimeout(tU32 pTime);

extern tPipe_chunk* C2_HOOK_FASTCALL FUN_00403800(tPipe_chunk* pSession, tPipe_chunk_type pType, tU32 pOwner);

extern void C2_HOOK_FASTCALL ScanAndPlaySoundsToBe(tU8* pFirst_session, tU32 pStart_time, tU32 pEnd_time);

#endif // GUARD_23_PIPING_H
