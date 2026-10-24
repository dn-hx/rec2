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

// ARInitialise

// ARDispose

extern int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType);

// LengthOfSession

// ARAdvanceChunkPtr

// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

// ARSomeReplayLeft

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

// EndPipingSession2

// AREndPipingSession

// ARAddDataToSession

extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARDoSingleSession

extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARResetPipePlayToEnd

// ARGetPipePlayPtr

// ARSetPipePlayPtr

// ApplyPipedSession

// MoveSessionPointerBackOne

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

// SoundTimeout

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

// AddNonCarToPipingSession

// AddSmokeToPipingSession

extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

// AddFlameToPipingSession

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession

// AddFlapToPipingSession

// AddModelMashToPipingSession

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

// PipeSingleCrush

extern void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y);

// PipeSingleGrooveStop

extern void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor);

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

extern void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType);

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

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

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// PipeSinglePHILObject

// PipeSingleGibShower

// PipeSingleBloodSpurt

extern void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags);

// PipeSinglePowerupGot

// PipeSinglePowerupLose

extern void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags);

// PipeSingleVanishedDismembered

// PipeSingleDSModel

extern void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

extern void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// ApplyScreenWobble

// ApplyGrooveStop

// ApplyNonCar

// ApplySmoke

// ApplySmokeColumn

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

// ApplyFrameBoundary

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

// ApplyIdentity

// ApplySplitWeld

extern void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk);

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

extern void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk);

// ApplySmashRemoveFaces

// ApplySmashDecal

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

// ApplyGrooveOnOff

// ApplyPowerupGot

// ApplyPowerupLose

// ApplyFunkEnable

// ApplyVanishDismembered

// ApplyDSModel

// ApplyPedDiagnostics

// ApplyVector3

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

// ApplyTransformType

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

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

// UndoBend

// UndoUnBend

// UndoShrapnelShower

// UndoNonCarCreation

extern void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSmashRemoveFaces

// UndoSmashDecal

// UndoSmashTextureChange

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

// UndoPedDismember

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

// UndoGrooveOnOff

// UndoPowerupGot

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

// UndoVector3

// UndoNapalmBolt

// UndoPowerupRespawn

// UndoShitMine

// UndoEndShitMine

// UndoTransformType

// CalcCrushLength

extern int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength

#endif // GUARD_23_PIPING_H
