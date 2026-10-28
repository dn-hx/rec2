#ifndef GUARD_23_PIPING_H
#define GUARD_23_PIPING_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include <s3/s3.h>

/* Replay pipe chunk layouts (kept out of rec2_types.h: adding declarations there perturbs MSVC codegen of unrelated functions) */
typedef struct {
    tU32 value;
} tPipe_stack_word;

typedef struct {
    int field_0x0;
    tU16 field_0x4;
} tPipe_chunk_funk_enable;

typedef struct {
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
} tPipe_chunk_remove_faces_replay;

typedef struct {
    uintptr_t pOwner;
    tU8 type_from;
    tU8 type_to;
} tPipe_chunk_transform_type;

typedef struct {
    int field_0x0;
    br_vector3 field_0x4;
} tPipe_chunk_shit_mine;

typedef struct {
    int field_0x0;
    int field_0x4;
    br_vector3 field_0x8;
} tPipe_chunk_napalm_bolt;

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
    int field_0x0;
    br_vector3 field_0x4;
} tPipe_chunk_explosion;

typedef struct {
    int field_0x0;
    tU16 field_0x4;
    tU16 field_0x6;
    tU16 field_0x8;
    tU16 field_0xa;
} tPipe_chunk_drone_corner_pos;

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
    tU32 field_0x4;
    br_vector3 field_0x8;
    br_vector3 field_0x14;
    br_vector3 field_0x20;
    br_vector3 field_0x2c;
    br_vector3 field_0x38;
    tU32 field_0x44;
    tU32 field_0x48;
    tU32 field_0x4c;
    tU32 field_0x50;
    br_vector3 field_0x54;
    tU8 field_0x60[0x24];
    tU32 field_0x84;
    br_vector3 field_0x88;
    tU8 field_0x94[0xc];
    tU32 field_0xa0;
    br_matrix34 field_0xa4;
} tPipe_chunk_shrapnel_shower;

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
    undefined4 field_0x0;
    br_actor* actor;
    undefined4 field_0x8;
    br_actor* field_0xc;
} tPipe_chunk_relink;

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
} tPipe_chunk_graphpical_wheel_stuff;

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
    br_vector3 field_0x4;
    tU8 field_0x10[0x24];
    int field_0x34;
    int field_0x38;
    int field_0x3c;
    tU8 field_0x40[4];
    int field_0x44;
    br_vector3 field_0x48;
    int field_0x54;
} tPipe_chunk_oil_spill;

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

extern float gReplay_rate;
extern const tReplay_callback* gPipe_callbacks;
extern tU8* gPipe_play_ptr;
extern int gPlay_direction;
extern tU8* gPipe_buffer_start;
extern int gReentrancy_count;
extern tU8* gPipe_record_ptr;
extern tU8* gPipe_buffer_working_end;
extern tU8* gPipe_buffer_phys_end;
extern tU8* gPipe_buffer_oldest;
extern int gReplay_pause_state;
extern int gPipe_undo_enabled;
extern volatile undefined4 gUNK_0069410c;
extern tU32 gPipe_sound_start_time;
extern tU32 gPipe_sound_time_window;
extern tU32 gYoungest_time;
extern tU8* gPipe_buffer_end_marker;
extern undefined4* gCrush_space;

extern int C2_HOOK_FASTCALL ARReplayForwards(void);

extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

extern int C2_HOOK_FASTCALL ARReplayPauseState(void);
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);

extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);

// ARResetPiping

// ARInitialise

// ARDispose

extern void C2_HOOK_FASTCALL SetPipeUndoFlag(void);

extern void C2_HOOK_FASTCALL ClearPipeUndoFlag(void);

extern int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType);

// LengthOfSession

extern void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tPipe_chunk** pChunk_ptr, tPipe_chunk_type pType);

// PipingFrameReset

// ARPipeSearchForwards

extern int C2_HOOK_FASTCALL ARReplayPlaying(void);

extern int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);
extern int C2_HOOK_FASTCALL ARSomeReplayLeft(void);

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

extern void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);

extern void C2_HOOK_FASTCALL AREndPipingSession(void);

// ARAddDataToSession

extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARDoSingleSession

extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARResetPipePlayToEnd

extern tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);
extern void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr);

// ApplyPipedSession

extern int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pSession);

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

extern int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime);

// ScanAndPlaySoundsToBe

// ARGetStartTime

// ARDisableAdvance

// AREnableAdvance

// FindNextChunk

// MoveReplayBuffer

// ARMoveToEndOfReplay

// ARMoveToStartOfReplay

// ARDoActionReplay

// ReverseSound

// ARGoBackwards

// ARGoForwards

extern void C2_HOOK_FASTCALL ARService(void);

extern void C2_HOOK_FASTCALL ARMainLoopStart(void);

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// PipeObjectPosition

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

// CheckCar

extern int C2_HOOK_FASTCALL CarTimeout(tU32 pTime);

// ScanCarsPositions

// InitialiseActionReplay

extern void C2_HOOK_FASTCALL DisposeActionReplay(void);

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

extern void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);

extern void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial);

// AddNonCarToPipingSession

// AddSmokeToPipingSession

extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

extern void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

extern void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime);

// AddFlapToPipingSession

// AddModelMashToPipingSession

extern void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime);

// AddNapalmBoltToPipingSession

// PipeSingleCrush

extern void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

extern void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y);

extern void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption);

extern void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor);

// PipeSingleOilSpill

extern void C2_HOOK_FASTCALL PipeSingleFrameFinish(void);

// PipeSingleGraphicalWheelStuff

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

extern void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType);

extern void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce);

// PipeSinglePedIncident

// PipeSingleWallIncident

extern void C2_HOOK_FASTCALL PipeSingleLocalCarIncident(br_vector3* pPosition, float pForce);

extern void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

extern void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle);

// PipeSingleRelink

extern void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor);

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

extern void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData);

// PipeSingleNonCarCreation

extern void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);

// PipeSinglePedStatus

extern void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir);

// PipeSinglePedMove

extern void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat);

// PipeSinglePedDismember

extern void C2_HOOK_FASTCALL PipeSinglePedFPChange(tPedestrian* pPed, float pValue, float pUnused);

extern void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity);

extern void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6);

extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

// PipeSingleBloodSpurt

extern void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags);

// PipeSinglePowerupGot

// PipeSinglePowerupLose

extern void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags);

// PipeSingleVanishedDismembered (defined in 23-piping.c; no extern here: an extra declaration shifts codegen of BitObjectIsSufficientlyOutsideCarObjectToDetach 0x00434910 in 13-crush1.c)

// PipeSingleDSModel

extern void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);

// PipeSingleDroneCornerPos (defined in 23-piping.c; no extern here: the extra declaration changed codegen of BitObjectIsSufficientlyOutsideCarObjectToDetach 0x00434910 in 13-crush1.c)

extern void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3);

extern void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id);

// PipeSingleShitMine

extern void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

extern void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable);
extern void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk);

// ApplySmoke

extern void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk);

// ApplyCar

extern void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyDamage(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk);

// ApplyBloodSpurt

extern void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk);

// ApplyFunkEnable

extern void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk);

// ApplyDroneRender

extern void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoDamage(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSpecial

extern void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoFlap

extern void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoUnBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

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

extern int C2_HOOK_FASTCALL CalcCrushLength(tPipe_chunk* pChunk);

extern int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

// CalcSmudgeLength

extern int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk);

// CalcEndMyBendLength

// CalcRemoveFacesLength

extern void C2_HOOK_FASTCALL DisposePiping(void);

#endif // GUARD_23_PIPING_H
