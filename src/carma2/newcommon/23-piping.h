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

// tPipe_chunk_blood_spurt

typedef struct {
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
} tPipe_chunk_remove_faces_replay;

// tPipe_chunk_frame_bound

// tPipe_chunk_transform_type

// tPipe_chunk_napalm_bolt

// tPipe_chunk_drone_straight_pos

// tPipe_chunk_drone_corner_pos

// tPipe_chunk_vanish

// tPipe_chunk_funk_enable

// tPipe_chunk_powerup

// tPipe_chunk_phil_object

// tPipe_chunk_ped_model_change

// tPipe_chunk_ped_pos

// tPipe_chunk_ped_fp_change

// tPipe_chunk_ped_physics

// tPipe_chunk_ped_move

// tPipe_chunk_ped_dir

// tPipe_chunk_ped_status

// tPipe_chunk_actor_trans

// tReplay_actor_trans_effect

// tPipe_chunk_remove_faces

// tPipe_chunk_shrapnel_shower

// tPipe_chunk_model_mash

// tPipe_chunk_skid_adjust

// tPipe_chunk_prox_ray_replay

// tPipe_chunk_graphpical_wheel_stuff

// tPipe_chunk_sound

// tPipe_chunk_oil_spill

// tPipe_chunk_extended_splash

// tPipe_chunk_splash

// tReplay_splash_effect

// tPipe_chunk_groove_stop

// tPipe_chunk_shrapnel_replay

// tPipe_chunk_spark_replay

// tPipe_chunk_crush

extern tU8* gLocal_buffer;
extern tU8* gPipe_buffer_working_end;
extern int gReentrancy_count;
extern tU8* gPipe_buffer_phys_end;
extern tU32 gLocal_buffer_size;
extern tU8* gMr_chunky;
extern tU8* gPipe_buffer_start;
extern int gPlay_direction;
extern int gDisable_piped_sounds;

typedef void C2_HOOK_FASTCALL tReplay_session_callback(int pFrames);
extern float gReplay_rate;
extern const tReplay_callback* gPipe_callbacks;

// gPipe_buffer_size
extern tU8* gPipe_play_ptr;
extern tU8* gPipe_buffer_oldest;
extern tU8* gPipe_record_ptr;
extern tU32 gPipe_sound_start_time;
extern tU32 gPipe_sound_time_window;

// ARReplayForwards

extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

// ARReplayPauseState

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);

extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);

extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);

extern void C2_HOOK_FASTCALL ARResetPiping(void);

// DisposePiping

// ARInitialise

// ARDispose

extern int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType);

extern tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_session* pSession);

// ARAdvanceChunkPtr

// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

extern int C2_HOOK_FASTCALL ARSomeReplayLeft(void);

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

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

// ARDisableAdvance

// AREnableAdvance

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

// WorkerSplitWeldApply

// WorkerModelMashUndo

// WorkerCrushUndo

// WorkerDroneStraightPos

// WorkerDroneCornerPos

// WorkerCrush

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// PipeObjectPosition

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

// AddCrushToPipingSession

// AddSmudgeToPipingSession

extern void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);

extern void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial);

extern void C2_HOOK_FASTCALL AddNonCarToPipingSession(int pIndex, br_actor* pActor);

// AddSmokeToPipingSession

extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

// AddFlameToPipingSession

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession

extern void C2_HOOK_FASTCALL AddFlapToPipingSession(br_actor* pActor, br_angle pAngle);

// AddModelMashToPipingSession

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

extern void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

// PipeSingleCrush

extern void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y);

// PipeSingleGrooveStop

extern void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor);

// PipeSingleOilSpill

extern void C2_HOOK_FASTCALL PipeSingleFrameFinish(void);

extern void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

extern void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType);

// PipeSingleCarIncident

// PipeSinglePedIncident

extern void C2_HOOK_FASTCALL PipeSingleWallIncident(float pSeverity, br_vector3* pImpact_point);

extern void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

extern void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle);

// PipeSingleRelink

extern void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor);

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

extern void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pExplosion);

extern void C2_HOOK_FASTCALL PipeSingleNonCarCreation(int pIndex);

extern void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

// PipeSingleSmashRemoveFaces

extern void C2_HOOK_FASTCALL PipeSingleSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);

// PipeSinglePedStatus

extern void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir);

// PipeSinglePedMove

extern void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat);

extern void C2_HOOK_FASTCALL PipeSinglePedDismember(tPedestrian* pPed, tU8 pBone_index, tS8 pOld_value, tS8 pNew_value);

// PipeSinglePedFPChange

extern void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3* pVelocity);

// PipeSinglePedModelChange

extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject);

// PipeSingleGibShower

// PipeSingleBloodSpurt

extern void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags);

// PipeSinglePowerupGot

extern void C2_HOOK_FASTCALL PipeSinglePowerupLose(tPowerup* pPowerup, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags);

extern void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, tU32 pOld_value, tU32 pNew_value);

extern void C2_HOOK_FASTCALL PipeSingleDSModel(int pDouble_sided, void* pThing);

extern void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);

// PipeSingleDroneCornerPos

extern void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance);

extern void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id);

// PipeSingleShitMine

extern void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

extern void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec* pOpponent_spec, int pEnable);

// ApplyCrush

extern void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk);

// ApplySpark

// ApplyShrapnel

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

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

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

// ApplyRepulseRay

// ApplyActorTrans

// ApplyPedStatus

// ApplyPedDir

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

// ApplyPedFPChange

// ApplyPedPos

// ApplyPedModelChange

// ApplyPHILObject

// ApplyGibShower

// ApplyBloodSpurt

extern void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk);

// ApplyPowerupGot

// ApplyPowerupLose

// ApplyFunkEnable

// ApplyVanishDismembered

// ApplyDSModel

// ApplyPedDiagnostics

extern void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk);

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

extern void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk);

// ApplyNapalmBolt

extern void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk);

// ApplyTransformType

// ApplyOppoRenderage

// UndoCrush

extern void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// ApplyReplayFrameTime

// UndoDamage

// UndoSpecial

// UndoScreenWobble

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

// UndoFlap

// UndoModelMash

// UndoRelink

// UndoSplitWeld

extern void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoUnBend(void);

// UndoShrapnelShower

extern void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

extern void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

extern void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoPowerupGot

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

extern void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoNapalmBolt

extern void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// CalcCrushLength

extern int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk);

// DoBloodSpurtUndo

// WorkerPedStatusUndo

// WorkerPedDiagnostics

// DoVanishDismembered

// WorkerPowerupLose

// WorkerPowerupGot

// WorkerPedPos

// WorkerPedDir

// WorkerPedStatus

// WorkerProxRay

// WorkerSpecial

// CalcRemoveFacesLength

extern tPipe_smudge_data* gSmudge_space;

// gCar_pos

// gCar_ptr

// gReference_pos

// gMax_distance

extern int gPipe_undo_enabled;

// gOldest_time

// gPipe_count_callbacks

extern volatile undefined4 gUNK_0069410c;

// gTrigger_time

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern undefined4* gCrush_space;

// gYoungest_time

extern int gUNK_006a23b4;

// gShrapnel_shower_matrix

extern tU8* gPipe_play_ptr;

extern tU8* gPipe_record_ptr;

typedef struct {
    tU32 value;
} tPipe_stack_word;


// FUN_DisposeReplayBuffer

// gReentrancy_array

// gReplay_callbacks

// PositionFragment

// InitFragmentSpeed

// FUN_00564e5e

#endif // GUARD_23_PIPING_H
