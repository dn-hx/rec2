#include "23-piping.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "51-smash.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

#include <stddef.h>

#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

// GLOBAL: CARMA2_HW 0x006768c0
// GLOBAL: CARMA2_SW 0x00671030
int gPlay_direction;

// GLOBAL: CARMA2_HW 0x00676900
float gReplay_rate;

// GLOBAL: CARMA2_HW 0x006768e8
const tReplay_callback* gPipe_callbacks;

// GLOBAL: CARMA2_HW 0x006768b8
// GLOBAL: CARMA2_SW 0x00671028
tU8* gPipe_play_ptr;

// ARReplayForwards

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == 0.f;
}

// ARReplayPauseState

// FUNCTION: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

// FUNCTION: CARMA2_HW 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float pRate) {
    gReplay_rate = pRate;
}

// FUNCTION: CARMA2_HW 0x004023b0
// FUNCTION: CARMA2_SW 0x004023b0
int C2_HOOK_FASTCALL ARGetReplayDirection(void) {
    return gPlay_direction;
}
// FUNCTION: CARMA2_HW 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    gPlay_direction = (int)pDirection;
}

// FUNCTION: CARMA2_HW 0x004023c0
// FUNCTION: CARMA2_SW 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    gPlay_direction = (int)pDirection;
}

// ARResetPiping

// ARInitialise

// ARDispose

// FUNCTION: CARMA2_HW 0x00403720
int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType) {

    if (gPipe_callbacks[pType].calc_length != NULL) {
        return gPipe_callbacks[pType].calc_length(pChunk) + gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    } else {
        return gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    }
}

// LengthOfSession

// FUNCTION: CARMA2_HW 0x004024e0
void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tPipe_chunk** pChunk_ptr, tPipe_chunk_type pType) {
    tU8* cur;
    tU8* next;
    tU32 size;

    cur = (tU8*)*pChunk_ptr;
    if (gPipe_undo_enabled != 0) {
        return;
    }
    size = sizeof(void*);
    if (gPipe_callbacks[pType].calc_length != NULL) {
        size += gPipe_callbacks[pType].calc_length((tPipe_chunk*)cur) + gPipe_callbacks[pType].length;
    } else {
        size += gPipe_callbacks[pType].length;
    }
    next = (tU8*)*pChunk_ptr + size;
    *pChunk_ptr = (tPipe_chunk*)next;
    if (gPipe_buffer_end_marker == next || (intptr_t)next > (intptr_t)gPipe_buffer_end_marker) {
        *pChunk_ptr = (tPipe_chunk*)cur;
    }
}

// PipingFrameReset

// ARPipeSearchForwards

// FUNCTION: CARMA2_HW 0x00402540
int C2_HOOK_FASTCALL ARReplayPlaying(void) {

    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate != 0.f) {
        if (gReplay_rate > 0.f) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return gPlay_direction > 0;
    }
}

// FUNCTION: CARMA2_HW 0x004025a0
int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {

    return gPipe_buffer_start != NULL;
}
// FUNCTION: CARMA2_HW 0x004025b0
int C2_HOOK_FASTCALL ARSomeReplayLeft(void) {
    return (gPlay_direction > 0 && gPipe_play_ptr != gPipe_record_ptr)
        || (gPlay_direction < 0 && gPipe_play_ptr != gPipe_buffer_oldest);
}

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00402660
// STUB: CARMA2_SW 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402890
// FUNCTION: CARMA2_SW 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {

    EndPipingSession2(1);
}

// ARAddDataToSession

// STUB: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

// ARDoSingleSession

// STUB: CARMA2_HW 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

// ARResetPipePlayToEnd

// FUNCTION: CARMA2_HW 0x00402df0
// FUNCTION: CARMA2_SW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {
    return gPipe_play_ptr;
}
// FUNCTION: CARMA2_HW 0x00402e00
void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr) {
    gPipe_play_ptr = pPtr;
}

// FUNCTION: CARMA2_HW 0x00402e00
// FUNCTION: CARMA2_SW 0x00402e00
void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr) {
    gPipe_play_ptr = pPtr;
}

// ApplyPipedSession

// FUNCTION: CARMA2_HW 0x004037c0
int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pSession) {
    tU8* footer;
    tU16 length;

    if (*pSession == gPipe_buffer_oldest && *pSession != gPipe_record_ptr) {
        return 1;
    }
    if (*pSession == gPipe_buffer_start) {
        *pSession = gPipe_buffer_working_end;
    }
    *pSession -= sizeof(tU16);
    footer = *pSession;
    memcpy(&length, footer, sizeof(length));
    *pSession = footer - length;
    return 0;
}

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

// FUNCTION: CARMA2_HW 0x00403c50
int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime) {
    return gPipe_sound_start_time < pTime
        || gPipe_sound_start_time - pTime < gPipe_sound_time_window;
}

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

// FUNCTION: CARMA2_HW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// FUNCTION: CARMA2_HW 0x00403d40
void C2_HOOK_FASTCALL ARMainLoopStart(void) {
    gAction_replay_mode = 0;
    gLast_replay_frame_time = PDGetTotalTime();
}

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// FUNCTION: CARMA2_HW 0x004c68a0
int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        ARAddVariedDataToSession(ePipe_chunk_non_car, (tU16)((tNon_car_spec*)pObject->actor->type_data)->car_ID, 2,
            4, 0, pObject->actor,
            48, 4, &pObject->actor->t.t.mat);
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x20) {
        ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)pObject, 2,
            4, 0, pObject->actor,
            48, 4, &pObject->actor->t.t.mat);
    }
    return 0;
}

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

// CheckCar

// FUNCTION: CARMA2_HW 0x004c6bc0
int C2_HOOK_FASTCALL CarTimeout(tU32 pTime) {

    if (ARReplayForwards()) {
        if (pTime > gYoungest_time) {
            return 0;
        }
    } else {
        if (pTime < gYoungest_time) {
            return 0;
        }
    }
    return 1;
}

// ScanCarsPositions

// InitialiseActionReplay

// FUNCTION: CARMA2_HW 0x004c6c60
void C2_HOOK_FASTCALL DisposeActionReplay(void) {
    DisposePiping();
    if (gCrush_space != NULL) {
        BrMemFree(gCrush_space);
        gCrush_space = NULL;
    }

}

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

// FUNCTION: CARMA2_HW 0x004c6f20
void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, v, 0xc);

    ARAddVariedDataToSession(ePipe_chunk_spark, pSpark_index, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, v), pV);
}

// FUNCTION: CARMA2_HW 0x004c6f40
void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, age, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, material, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, material, 0x4);

    if (pShrapnel_index & 0x8000) {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 3,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, age), pAge,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, material), pMaterial);
    } else {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 1,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos);
    }
}

// AddNonCarToPipingSession

// AddSmokeToPipingSession

// FUNCTION: CARMA2_HW 0x004c70c0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    pIndex += pColour << 14;
    ARAddVariedDataToSession(ePipe_chunk_smoke_column, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), pCore,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}

// FUNCTION: CARMA2_HW 0x004c70f0
void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tU32 bits;

    (void)pScale_y;
    (void)pOffset_x;
    (void)pOffset_z;
    memcpy(&bits, &pScale_x, sizeof(bits));
    gUNK_0069410c = bits;
    ARAddVariedDataToSession((tPipe_chunk_type)18, pIndex, 5,
        2, 0, pFrame, 4, 4, bits, 4, 8, bits, 4, 12, bits, 4, 16, bits);
}

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

// FUNCTION: CARMA2_HW 0x004c74c0
void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, ped_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c7520
void C2_HOOK_FASTCALL AddFlapToPipingSession(uintptr_t pOwner, tU16 pDelta) {

    ARAddVariedDataToSession(ePipe_chunk_flap, pOwner, 2,
        4, 0, pOwner,
        2, 4, pDelta);
}

// AddModelMashToPipingSession

// FUNCTION: CARMA2_HW 0x004c7940
void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, origin, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_repulse_ray, (uintptr_t)pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}

// AddNapalmBoltToPipingSession

// PipeSingleCrush

// FUNCTION: CARMA2_HW 0x004c7e70
void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {

    ARAddVariedDataToSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, positions), pPositions);
}

// FUNCTION: CARMA2_HW 0x004c8090
void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_x, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_x), pWobble_x,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_y), pWobble_y);
}

// FUNCTION: CARMA2_HW 0x004c80b0
void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {

    gUNK_0069410c = *(tU32*)&pPath_resumption;
    ARDoSingleVariedSession(0xd, pGroove_index, 5,
        4, 0, pObject_interrupt,
        4, 4, pObject_interrupt,
        4, 8, *(tU32*)&pPath_resumption,
        4, 0xc, *(tU32*)&pPath_resumption,
        0x30, 0x10, pMatrix);
}

// FUNCTION: CARMA2_HW 0x004c80f0
void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_non_car, pId, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// PipeSingleOilSpill

// FUNCTION: CARMA2_HW 0x004c8340
void C2_HOOK_FASTCALL PipeSingleFrameFinish(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_finish, time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_frame_finish, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_frame_finish, 0, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_frame_finish, time), GetTotalTime());
}

// PipeSingleGraphicalWheelStuff

// FUNCTION: CARMA2_HW 0x004c84a0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, pitch, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, position, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, position, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, outlet, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, outlet, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, volume, 0x12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, volume, 0x2);

    if (!gAction_replay_mode && gProgram_state.racing) {
        if (pPosition == NULL) {
            pPosition = &gZero_vector__smash;
        }
        ARDoSingleVariedSession(ePipe_chunk_single_sound, pSound, 4,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, pitch), pPitch,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, position), pPosition,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, outlet), GetIndexFromOutlet(pOutlet),
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, volume), (pR_volume << 8) + pL_volume);
    }
}

// FUNCTION: CARMA2_HW 0x004c8530
void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {
    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// FUNCTION: CARMA2_HW 0x004c8540
void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce) {

    gUNK_0069410c = *(tU32*)&pForce;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 1, 3,
        4, 0, *(tU32*)&pForce,
        2, 4, (tU16)pCar->car_ID,
        0xc, 8, pPosition);
}

// FUNCTION: CARMA2_HW 0x004c8580
void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor) {

    gUNK_0069410c = 0;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 0, 3,
        4, 0, gUNK_0069410c,
        4, 4, pPed,
        4, 8, pActor);
}

// PipeSingleWallIncident

// FUNCTION: CARMA2_HW 0x004c85b0
void C2_HOOK_FASTCALL PipeSingleLocalCarIncident(br_vector3* pPosition, float pForce) {
}

// FUNCTION: CARMA2_HW 0x004c8600
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

    ARDoSingleVariedSession(0x16, pSkid_num, 2,
        0x30, 0, pMatrix,
        4, 0x30, pMaterial);
}

// FUNCTION: CARMA2_HW 0x004c8620
void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, angle, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_flap, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, angle), pAngle);
}

// PipeSingleRelink

// FUNCTION: CARMA2_HW 0x004c86d0
void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_identity, actor, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_identity, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_identity, actor), pActor);
}

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// FUNCTION: CARMA2_HW 0x004c8920
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData) {
    C2_HOOK_BUG_ON(sizeof(tExplosion) != 120);

    ARDoSingleVariedSession(ePipe_chunk_explosion, (uintptr_t)pIndex, 1, sizeof(tExplosion), 0, pData);
}

// PipeSingleNonCarCreation

// FUNCTION: CARMA2_HW 0x004c8960
void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, old_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, new_model, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_smash_model_change, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, old_model), pOld_model,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, new_model), pNew_model);
}

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// FUNCTION: CARMA2_HW 0x004c8a10
void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, car_spec, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, texture1, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, texture2, 0x4);

    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, car_spec), pCar_spec,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture1), pMaterial->colour_map,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture2), pTexture);
}

// FUNCTION: CARMA2_HW 0x004c8a90
void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tS8 pArg7, tS8 pArg8, tS8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_status, (uintptr_t)pPed, 11,
        1, 0, pArg2,
        1, 1, pArg3,
        1, 2, pArg4,
        1, 3, pArg5,
        1, 4, pArg6,
        1, 5, pArg7,
        1, 6, pArg8,
        1, 7, pArg9,
        12, 8, pArg10,
        12, 20, pArg11,
        48, 32, pArg12);
}

// FUNCTION: CARMA2_HW 0x004c8b10
void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_dir_data, direction, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction),       pDir);
}

// PipeSinglePedMove

// FUNCTION: CARMA2_HW 0x004c8bd0
void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_physics_data, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_physics_data, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_physics, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_physics_data, matrix),       pMat);
}

// PipeSinglePedDismember

// FUNCTION: CARMA2_HW 0x004c8c30
void C2_HOOK_FASTCALL PipeSinglePedFPChange(tPedestrian* pPed, float pValue, float pUnused) {

    gUNK_0069410c = *(tU32*)&pValue;
    /* chunk type 45 (0x2d): no tPipe_chunk_type constant yet (adding enum constants to rec2_types.h shifts codegen elsewhere) */
    ARDoSingleVariedSession(45, (uintptr_t)pPed, 2,
        4, 0, *(tU32*)&pValue,
        4, 4, *(tU32*)&pValue);
}

// PipeSinglePedModelChange

// FUNCTION: CARMA2_HW 0x004c8cc0
// FUNCTION: CARMA2_SW 0x004c53f0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8c80
void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_model_change, (uintptr_t)pPed, 5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, model), pModel,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x2), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x6), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x4), pArg6);
}

// FUNCTION: CARMA2_HW 0x004c8cc0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_phil_object, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8ce0
void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {

    gUNK_0069410c = pArg4;
    ARDoSingleVariedSession(0x31, pId, 7,
        4, 8, pArg2,
        4, 4, pArg3,
        4, 0xc, gUNK_0069410c,
        4, 0, pArg5,
        0xc, 0x10, pArg6,
        0xc, 0x1c, pArg7,
        0xc, 0x28, pArg8);
}

// PipeSingleBloodSpurt

// FUNCTION: CARMA2_HW 0x004c8d80
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, enable, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, block_flags, 0x4);

    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}

// PipeSinglePowerupGot

// PipeSinglePowerupLose

// STUB: CARMA2_HW 0x004c8de0
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c8e10
void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, undefined4 pArg2, undefined4 pArg3) {

    ARDoSingleVariedSession(ePipe_chunk_single_vanished_dismembered, (uintptr_t)pPed, 2,
        4, 0, pArg2,
        4, 4, pArg3);
}

// FUNCTION: CARMA2_HW 0x004c8e30
void C2_HOOK_FASTCALL PipeSingleDSModel(int pKind, void* pThing) {

    /* pKind 0: pThing is a tCar_spec*, 1: a br_model* (carpocalypse2 comment) */
    ARDoSingleVariedSession(ePipe_single_ds_model, (uintptr_t)pThing, 1,
        2, 0, pKind);
}

// FUNCTION: CARMA2_HW 0x004c8e80
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vector, vector, 0xc);

    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}

// FUNCTION: CARMA2_HW 0x004c8ea0
void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_render, render, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_render, render, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_render, pDrone_spec->id, 1,
        SIZE_OFFSET_PIPING(tPipe_drone_render, render),         pRender);
}

// FUNCTION: CARMA2_HW 0x004c8ec0
void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pField_0x70, tCompressed_matrix34* pMatrix) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x48, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x48, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x70, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x70, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, matrix, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, matrix, 0x18);

    ARDoSingleVariedSession(ePipe_chunk_drone_corner_pos, pDrone->id, 7,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x48),         pField_0x48,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x74),         pField_0x74,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x70),         pField_0x70,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[0]),        &pMatrix->m[0],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[1]),        &pMatrix->m[1],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[2]),        &pMatrix->m[2],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[3]),        &pMatrix->m[3]);
}

// FUNCTION: CARMA2_HW 0x004c8f30
void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3) {

    // FIXME: is this a bug, or is pParam2 intentionally ignored?
    ARDoSingleVariedSession(ePipe_chunk_single_drone_straight_pos, pDrone->id, 3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x0), pParam1,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x4), pParam3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x6), pParam3);
}

// FUNCTION: CARMA2_HW 0x004c8f90
void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_respawn, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_respawn, index, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_respawn, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_respawn, index), pPowerup_id);
}

// PipeSingleShitMine

// FUNCTION: CARMA2_HW 0x004c8fd0
void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_end_shit_mine, m, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_end_shit_mine, m, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_end_shit_mine, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_end_shit_mine, m), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8ff0
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {
    tU16 type_from = pActor->t.type;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_from, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_transform_type, type_to, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_to, 0x1);

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), type_from,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), (tU16)pTransform_type);
}

// FUNCTION: CARMA2_HW 0x004c9020
void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable) {

    ARDoSingleVariedSession(ePipe_chunk_single_oppo_renderage, (uintptr_t)pOpponent_spec, 1,
        SIZE_OFFSET_PIPING(tPipe_single_oppo_renderage, enable), pEnable);
}
// FUNCTION: CARMA2_HW 0x004c9040
void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk) {
    tPipe_chunk_crush* chunk = (tPipe_chunk_crush*)*pChunk;

    return WorkerCrush(chunk->field_0x4, &chunk->field_0x8[0], &chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9060
void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk) {

    return WorkerSmudge(pChunk, 1);
}

// FUNCTION: CARMA2_HW 0x004c9180
// FUNCTION: CARMA2_SW 0x004c58b0
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    SetScreenWobble(((tPipe_chunk_screen_wobble*)(*pChunk)->data.raw_data)->wobble_x,
        ((tPipe_chunk_screen_wobble*)(*pChunk)->data.raw_data)->wobble_y);
}

// ApplyGrooveStop

// FUNCTION: CARMA2_HW 0x004c91b0
// FUNCTION: CARMA2_SW 0x004c58e0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);

    AdjustNonCar((*pChunk)->data.non_car.actor, &(*pChunk)->data.non_car.matrix);
}

// ApplySmoke

// FUNCTION: CARMA2_HW 0x004c9280
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {
    tPipe_chunk_smoke_column_replay* chunk = (tPipe_chunk_smoke_column_replay*)*pChunk;

    return WorkerSmokeColumn(chunk->field_0x0 & 0x3fff, (br_vector3*)chunk->pad, chunk->field_0x14, chunk->field_0x0 >> 14);
}

// FUNCTION: CARMA2_HW 0x004c92a0
void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;

    return WorkerFlame(*(int*)(chunk + 0), (int)(tS16)*(tU16*)(chunk + 4), *(float*)(chunk + 8), *(float*)(chunk + 0xc), *(float*)(chunk + 0x10), *(float*)(chunk + 0x10));
}

// FUNCTION: CARMA2_HW 0x004c92d0
void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk) {
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->size = ((tPipe_chunk_splash*)*pChunk)->v[0];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->x = ((tPipe_chunk_splash*)*pChunk)->v[1];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->y = ((tPipe_chunk_splash*)*pChunk)->v[2];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->z = ((tPipe_chunk_splash*)*pChunk)->v[3];
}

// FUNCTION: CARMA2_HW 0x004c9310
void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk) {
    tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)*pChunk;

    return WorkerExtendedSplash(chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, &chunk->field_0xc, &chunk->field_0x18);
}


// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

// ApplyRelink

// FUNCTION: CARMA2_HW 0x004c9890
// FUNCTION: CARMA2_SW 0x004c5fc0
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    chunk = *pChunk;
    BrMatrix34Identity(&chunk->data.identity.actor->t.t.mat);
}

// ApplySplitWeld

// FUNCTION: CARMA2_HW 0x004c9930
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9940
// FUNCTION: CARMA2_SW 0x004c6070
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {
}

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

// FUNCTION: CARMA2_HW 0x004c99f0
void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);

    chunk = *pChunk;
    if (chunk->data.smash_model_change.new_model != NULL) {
        ChangeSmashModel((br_actor*)chunk->subject_index, chunk->data.smash_model_change.old_model, chunk->data.smash_model_change.new_model);
    } else {
        TurnOffSmashModel((br_actor*)chunk->subject_index);
    }
}

// ApplySmashRemoveFaces

// FUNCTION: CARMA2_HW 0x004c9a20
// FUNCTION: CARMA2_SW 0x004c6150
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    chunk = *pChunk;
    TurnOnSmashDecal(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004c9a30
void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);

    chunk = *pChunk;
    ChangeSmashTexture((br_material*)chunk->subject_index,
        chunk->data.smash_texture_change.car_spec,
        chunk->data.smash_texture_change.texture1,
        chunk->data.smash_texture_change.texture2);
}

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

// FUNCTION: CARMA2_HW 0x004ca110
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    if (chunk->data.smash_model_change.new_model != NULL) {
        ChangeSmashModel((br_actor*)chunk->subject_index, chunk->data.smash_model_change.new_model, chunk->data.smash_model_change.old_model);
    } else {
        TurnOnSmashModel((br_actor*)chunk->subject_index);
    }
}

// UndoSmashRemoveFaces

// FUNCTION: CARMA2_HW 0x004ca150
// FUNCTION: CARMA2_SW 0x004c6880
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    (void)pPrev_chunk;
    chunk = *pChunk;
    TurnOffSmashDecal(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004ca160
// FUNCTION: CARMA2_SW 0x004c6890
void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);

    (void)pPrev_chunk;
    chunk = *pChunk;
    ChangeSmashTexture((br_material*)chunk->subject_index,
        chunk->data.smash_texture_change.car_spec,
        chunk->data.smash_texture_change.texture2,
        chunk->data.smash_texture_change.texture1);
}

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

// FUNCTION: CARMA2_HW 0x004ca400
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {
    C2_HOOK_BUG_ON(sizeof(tPipe_chunk_shrapnel) != 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);

    /* AddShrapnelToPipingSession only stores age and material for new shrapnel (0x8000 in the index) */
    if (pChunk->subject_index & 0x8000) {
        return sizeof(tPipe_chunk_shrapnel);
    } else {
        return offsetof(tPipe_chunk_shrapnel, age);
    }
}

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength

