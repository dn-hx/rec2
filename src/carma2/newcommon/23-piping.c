#include "23-piping.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "51-smash.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>
#include "13-crush1.h"
#include "41-utility.h"
#include "30-opponent.h"
#include "22-replay.h"
#include "c2_string.h"
#include "09-funks.h"
#include "39-oil.h"
#include "18-graphics2.h"
#include "20-skidmark.h"
#include "32-spark.h"
#include "61-pedestrn.h"

#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))
#define CRUSH_SPACE_SIZE 0x4000

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

// GLOBAL: CARMA2_HW 0x00676908
tU8* gPipe_buffer_start = NULL;

// GLOBAL: CARMA2_HW 0x006768e4
int gReentrancy_count = 0;

// GLOBAL: CARMA2_HW 0x006768bc
tU8* gPipe_record_ptr = NULL;

// GLOBAL: CARMA2_HW 0x006768c4
tU8* gPipe_buffer_working_end = NULL;

// GLOBAL: CARMA2_HW 0x006768f0
tU8* gPipe_buffer_phys_end = NULL;

// GLOBAL: CARMA2_HW 0x006768ac
tU8* gPipe_buffer_oldest = NULL;

// GLOBAL: CARMA2_HW 0x006768fc
int gReplay_pause_state = 0;

// GLOBAL: CARMA2_HW 0x0067690c
int gPipe_undo_enabled = 0;

// GLOBAL: CARMA2_HW 0x0069410c
volatile undefined4 gUNK_0069410c;

// GLOBAL: CARMA2_HW 0x00676904
tU32 gPipe_sound_start_time = 0;

// GLOBAL: CARMA2_HW 0x006768a8
tU32 gPipe_sound_time_window = 0;

// GLOBAL: CARMA2_HW 0x00694108
tU32 gYoungest_time;

// GLOBAL: CARMA2_HW 0x006768c8
tU8* gPipe_buffer_end_marker = NULL;

// GLOBAL: CARMA2_HW 0x006768cc
int gUNK_006768cc = 0;

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5] = { 0, 0, 0, 0, 0 };

// GLOBAL: CARMA2_HW 0x00694104
undefined4* gCrush_space;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00402300
int C2_HOOK_FASTCALL ARReplayForwards(void) {

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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == 0.f;
}

// FUNCTION: CARMA2_HW 0x00402380
int C2_HOOK_FASTCALL ARReplayPauseState(void) {
    return gReplay_pause_state;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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
// FUNCTION: CARMA2_SW 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    gPlay_direction = (int)pDirection;
}

// ARResetPiping

// ARDispose

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004023e0
void C2_HOOK_FASTCALL ARResetPiping(void) {

    gPipe_record_ptr = gPipe_buffer_start;
    gPipe_buffer_working_end = gPipe_buffer_phys_end;
    gPipe_buffer_oldest = NULL;
    gReentrancy_count = 0;
}

// GLOBAL: CARMA2_HW 0x006768b4
tU8* gLocal_buffer = NULL;

// GLOBAL: CARMA2_HW 0x006768ec
tU32 gPipe_buffer_size = 0;

// GLOBAL: CARMA2_HW 0x006768f4
tU32 gLocal_buffer_size = 0;

// GLOBAL: CARMA2_HW 0x006768a0
tU8* gMr_chunky = NULL;

// GLOBAL: CARMA2_HW 0x00676910
int gPipe_count_callbacks = 0;

extern void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** pBuffer, tU32* pSize);

// FUNCTION: CARMA2_HW 0x00402410
void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks) {

    if (!pEnable) {
        gPipe_buffer_start = NULL;
        gLocal_buffer = NULL;
    } else {
        gPipe_callbacks = pCallbacks;
        gPipe_count_callbacks = pCount_callbacks;
        PDAllocateActionReplayBuffer(&gPipe_buffer_start, &gPipe_buffer_size);
        gPipe_buffer_phys_end = gPipe_buffer_start + gPipe_buffer_size;
        gLocal_buffer = BrMemAllocate(15000, kMem_pipe_model_geometry);
    }
    ARResetPiping();
}

// ARDispose

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004030d0
void C2_HOOK_FASTCALL SetPipeUndoFlag(void) {
    gPipe_undo_enabled = 1;
}

// FUNCTION: CARMA2_HW 0x004030e0
void C2_HOOK_FASTCALL ClearPipeUndoFlag(void) {
    gPipe_undo_enabled = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// LengthOfSession

// ARAdvanceChunkPtr

// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

// ARSomeReplayLeft

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2 has no standalone address in CARMA2_HW: the original inlines it into
// ARStartPipingSession (0x004025e0) and into EndPipingSession2 (0x00402660, pMunge=0 path).
static void StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy) {

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    /* Header of gLocal_buffer is { int chunk_type, int chunk_data_size } */
    if (pMunge_reentrancy) {
        if (gReentrancy_count != 0) {
            gReentrancy_array[gReentrancy_count - 1] = ((int*)gLocal_buffer)[0];
            EndPipingSession2(0);
        }
        gReentrancy_count += 1;
    }
    ((int*)gLocal_buffer)[0] = pType;
    ((int*)gLocal_buffer)[1] = 0;
    gLocal_buffer_size = 2 * sizeof(int);
    gMr_chunky = gLocal_buffer + 2 * sizeof(int);
}

// FUNCTION: CARMA2_HW 0x004025e0
void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {

    StartPipingSession2(pType, 1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);


// FUNCTION: CARMA2_HW 0x00402660
// FUNCTION: CARMA2_SW 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {
    tU32 i;
    tU32 total;
    tU32 size;
    tU32 length;
    tU32* session;
    void* the_chunk;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    *(tU16*)(gLocal_buffer + gLocal_buffer_size) = (tU16)gLocal_buffer_size;
    gLocal_buffer_size += 2;
    if (((tU32*)gLocal_buffer)[1] != 0 && gLocal_buffer_size <= 15000) {
        if ((uintptr_t)gPipe_record_ptr + gLocal_buffer_size > (uintptr_t)gPipe_buffer_phys_end) {
            gPipe_buffer_working_end = gPipe_record_ptr;
            gPipe_buffer_oldest = gPipe_buffer_start;
            gPipe_record_ptr = gPipe_buffer_start;
        }
        while ((uintptr_t)gPipe_record_ptr <= (uintptr_t)gPipe_buffer_oldest
            && (uintptr_t)gPipe_record_ptr + gLocal_buffer_size > (uintptr_t)gPipe_buffer_oldest) {
            session = (tU32*)gPipe_buffer_oldest;
            total = 0;
            for (i = 0; i < session[1]; i++) {
                the_chunk = (tU8*)(session + 2) + total;
                if (gPipe_callbacks[session[0]].calc_length != NULL) {
                    length = gPipe_callbacks[session[0]].calc_length(the_chunk);
                    size = length + gPipe_callbacks[session[0]].length + 4;
                } else {
                    size = gPipe_callbacks[session[0]].length + 4;
                }
                total += size;
            }
            total += 10;
            if ((total & 1) != 0) {
                FatalError(0x62);
            }
            gPipe_buffer_oldest += total;
            if ((uintptr_t)gPipe_buffer_working_end <= (uintptr_t)gPipe_buffer_oldest) {
                gPipe_buffer_working_end = gPipe_buffer_phys_end;
                gPipe_buffer_oldest = gPipe_buffer_start;
            }
        }
        if (gPipe_buffer_oldest == NULL) {
            gPipe_buffer_oldest = gPipe_record_ptr;
        }
        memcpy(gPipe_record_ptr, gLocal_buffer, gLocal_buffer_size);
        gPipe_record_ptr += gLocal_buffer_size;
        if ((uintptr_t)gPipe_buffer_working_end < (uintptr_t)gPipe_record_ptr) {
            gPipe_buffer_working_end = gPipe_record_ptr;
        }
    }
    if (pMunge_reentrancy && gReentrancy_count != 0) {
        gReentrancy_count--;
        if (gReentrancy_count != 0) {
            StartPipingSession2(gReentrancy_array[gReentrancy_count - 1], 0);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402890
// FUNCTION: CARMA2_SW 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {

    EndPipingSession2(1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, int pSize) {
    tU32 new_size;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    new_size = gLocal_buffer_size + pSize + sizeof(void*);
    if (new_size >= 15000) {
        return;
    }
    ((int*)gLocal_buffer)[1] += 1;
    *((uintptr_t*)gMr_chunky) = pOwner;
    gMr_chunky += sizeof(uintptr_t);
    if (pSize != 0) {
        memcpy(gMr_chunky, pData, pSize);
    }
    gMr_chunky += pSize;
    gLocal_buffer_size = new_size;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00402ab0
void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on ApexWorm/rec2 017a1a11d
// FUNCTION: CARMA2_HW 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    tU8 buffer[5000];
    va_list args;
    int i;
    int size;
    int offset;
    int length;
    int data_size;
    tU32 new_size;
    uintptr_t value;

    ARStartPipingSession((tPipe_chunk_type)pType);
    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        va_start(args, pCount);
        for (i = 0; i < pCount; i++) {
            size = va_arg(args, int);
            offset = va_arg(args, int);
            if (size < 0) {
#ifdef REC2_MATCHING
                *(float*)(buffer + offset) = va_arg(args, float);
#else
                *(float*)(buffer + offset) = (float)va_arg(args, double);
#endif
            } else {
                value = va_arg(args, uintptr_t);
                switch (size) {
                case 1:
                    buffer[4 + offset] = (tU8)value;
                    break;
                case 2:
                    *(tU16*)(buffer + 4 + offset) = (tU16)value;
                    break;
                case 4:
                    *(tU32*)(buffer + 4 + offset) = value;
                    break;
                default:
                    if (value != 0) {
                        memcpy(buffer + 4 + offset, (void*)value, size);
                    }
                    break;
                }
            }
        }
        va_end(args);
        *(uintptr_t*)buffer = pOwner;
        if (gPipe_callbacks[pType].calc_length != NULL) {
            length = gPipe_callbacks[pType].calc_length((tPipe_chunk*)buffer) + gPipe_callbacks[pType].length + 4;
        } else {
            length = gPipe_callbacks[pType].length + 4;
        }
        data_size = length - 4;
        if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
            new_size = gLocal_buffer_size + data_size + 4;
            if (new_size < 15000) {
                ((int*)gLocal_buffer)[1] += 1;
                *((uintptr_t*)gMr_chunky) = pOwner;
                gMr_chunky += sizeof(uintptr_t);
                if (data_size != 0) {
                    memcpy(gMr_chunky, buffer + 4, data_size);
                }
                gLocal_buffer_size = new_size;
                gMr_chunky += data_size;
            }
        }
    }
    EndPipingSession2(1);
}

// ARResetPipePlayToEnd

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00402df0
// FUNCTION: CARMA2_SW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {
    return gPipe_play_ptr;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// STUB: CARMA2_HW 0x00403d60
int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8) {

    NOT_IMPLEMENTED();
}

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

extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, int pSize);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);
extern int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

// based on carpocalypse2 src/carma2/common/piping.c:451 (reference only)
// FUNCTION: CARMA2_HW 0x004c6640
void C2_HOOK_FASTCALL PipeCarPositions(void) {
    int category;
    int car_count;
    int car_index;
    int session_started;

    ARStartPipingSession(ePipe_chunk_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {
        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec* car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            /* AddCarToPipingSession inlined; tPipe_car_chunk is { mat 0/48, v 48/12, speedo_speed 60/2, flags 62/2, field_0x40 64/4 } */
            ARAddVariedDataToSession(ePipe_chunk_car, (uintptr_t)((category << 8) | car_index), 5,
                48, 0, &car->car_master_actor->t.t.mat,
                12, 48, &car->collision_info->v,
                2, 60, (int)(car->speedo_speed * 468100.0),
                2, 62, ((int)car->revs / 10 & 0x7ff) + ((*(tS32*)(void*)&car->gear + 1) << 12) + (car->frame_collision_flag ? 0x800 : 0),
                4, 64, car->field_0x18cc);
        }
    }
    AREndPipingSession();

    session_started = 0;
    for (category = eVehicle_self; category < eVehicle_net_player; category++) {
        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec* car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            if (car->active) {
                tU8 damage_deltas[12];
                int any_damage;
                int i;

                any_damage = 0;
                for (i = 0; i < 12; i++) {
                    int damage;
                    int damage_delta;

                    damage = car->damage_units[i].damage_level;
                    damage_delta = damage - car->frame_start_damage[i];
                    car->frame_start_damage[i] = damage;
                    damage_deltas[i] = damage_delta;
                    any_damage |= damage_delta;
                }
                if (any_damage) {
                    if (!session_started) {
                        ARStartPipingSession(ePipe_chunk_damage);
                        session_started = 1;
                    }
                    /* AddDamageToPipingSession inlined */
                    {
                        tU8 damage_copy[12];

                        memcpy(damage_copy, damage_deltas, sizeof(damage_copy));
                        ARAddDataToSession(ePipe_chunk_damage, (uintptr_t)((category << 8) | car_index), damage_copy, sizeof(damage_copy));
                    }
                }
            }
        }
    }
    if (session_started) {
        AREndPipingSession();
    }

    ARStartPipingSession(ePipe_chunk_non_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {
        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec* car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            PhysicsObjectRecurseChildren(car->collision_info, PipeObjectPosition, NULL);
        }
    }
    AREndPipingSession();
}

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

// GLOBAL: CARMA2_HW 0x006940d0
tU32 gOldest_time;

// GLOBAL: CARMA2_HW 0x006940d8
br_scalar gMax_distance;

// GLOBAL: CARMA2_HW 0x006940e0
br_vector3 gReference_pos;

// GLOBAL: CARMA2_HW 0x006940f0
br_vector3 gCar_pos;

// GLOBAL: CARMA2_HW 0x00694100
tCar_spec* gCar_ptr;

// GLOBAL: CARMA2_HW 0x00694110
tU32 gTrigger_time;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00402e10
int C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, void* pCallback, void* pTime_check) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c6a30
int C2_HOOK_FASTCALL CheckCar(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on carpocalypse2 src/carma2/common/piping.c:891
// FUNCTION: CARMA2_HW 0x004c6920
void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned) {
    tU8* temp_ptr;

    temp_ptr = ARGetPipePlayPtr();
    gTrigger_time = 0;
    gMax_distance = pMax_distance_sqr;
    BrVector3Copy(&gReference_pos, pSource_pos);
    gCar_ptr = pCar;

    if (ARReplayForwards()) {
        gOldest_time = GetTotalTime() + pOffset_time;
        gYoungest_time = gOldest_time + pTime_period;
    } else {
        gOldest_time = GetTotalTime() - pOffset_time;
        gYoungest_time = gOldest_time - pTime_period;
    }

    ARScanBuffer(&temp_ptr, ePipe_chunk_car, GetTotalTime(), CheckCar, CarTimeout);
    *pCar_pos = gCar_pos;
    if (pCar_pos->v[0] > 500.f) {
        BrVector3Sub(pCar_pos, pCar_pos, &gInitial_position);
    }
    *pTime_returned = gTrigger_time;
}

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

// GLOBAL: CARMA2_HW 0x006940d4
tPipe_smudge_data* gSmudge_space;

// FUNCTION: CARMA2_HW 0x004c6ea0
void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
    tU32 data_size;

    if (gSmudge_space != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        data_size = pVertex_count * sizeof(tSmudged_vertex);
        gSmudge_space->vertex_count = pVertex_count;
        gSmudge_space->model_index = pModel_index;
        memcpy(gSmudge_space->vertex_changes, pCoordinates, data_size);
        data_size += offsetof(tPipe_smudge_data, vertex_changes);
        ARAddDataToSession(ePipe_chunk_smudge, pCar_ID, gSmudge_space, data_size);
    }
}

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

extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pCollision_info, int pParam);

// FUNCTION: CARMA2_HW 0x004c7160
void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2) {
    tU8 flag;

    if (PHILGetObjectProperty(pCollision, 6) != 0.0f) {
        flag = 0xff;
    } else {
        flag = pCollision->flags_0x238;
    }
    *(volatile float*)&gUNK_0069410c = pCollision->water_d;
    ARAddVariedDataToSession((tPipe_chunk_type)0x3b, (uintptr_t)pArg2, 4,
        1, 0, (*(int*)&flag) & 0xff,
        4, 4, gUNK_0069410c,
        0xc, 8, &pCollision->water_normal,
        0x18, 0x14, &pCollision->bb2);
}

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

// FUNCTION: CARMA2_HW 0x004c7550
void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash) {
    size_t size;
    tPipe_model_mash* pipe_model_mash;

    pipe_model_mash = (tPipe_model_mash*)gCrush_space;
    if (pipe_model_mash == NULL) {
        return;
    }
    size = pModel->nvertices * sizeof(tCompressed_vector3) + sizeof(br_model*);
    if (size > CRUSH_SPACE_SIZE) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    pipe_model_mash->model = pModel;
    ARCompressModelMash(pipe_model_mash->vectors, pModel, pMash);
    ARAddDataToSession(ePipe_model_mash, (uintptr_t)-1, pipe_model_mash, (tU32)size);
}

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

// FUNCTION: CARMA2_HW 0x004c83f0
void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {

    ARDoSingleVariedSession(ePipe_chunk_single_graphpical_wheel_stuff, (tU16)pCar->car_ID, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (int)(pCar->steering_angle * (32767.0 / 60.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), (int)(pCar->lf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), (int)(pCar->rf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), (int)(pCar->lr_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), (int)(pCar->rr_sus_position * 127.f * (20.0 / 3.0)));
}

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

// FUNCTION: CARMA2_HW 0x004c8980
void C2_HOOK_FASTCALL PipeSingleSmashRemoveFaces(br_model* pModel, int pCount, const void* pFaces, br_material* pMaterial) {
    tU32 size;
    undefined4* crush_space;

    crush_space = gCrush_space;
    if (crush_space == NULL) {
        return;
    }
    size = (tU32)pCount * 8u;
    crush_space[1] = pCount;
    *(br_material**)&crush_space[0] = pMaterial;
    memcpy(crush_space + 2, pFaces, size);
    size += 8;
    if (size > CRUSH_SPACE_SIZE) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    ARDoSingleSession(34, (uintptr_t)pModel, crush_space, size);
}

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

// FUNCTION: CARMA2_HW 0x004c8c60
void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_pos, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, v), pVelocity);
}

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

// FUNCTION: CARMA2_HW 0x004c8de0
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags) {

    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
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

// FUNCTION: CARMA2_HW 0x004c9140
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {
    tPipe_chunk_spark_replay* chunk = (tPipe_chunk_spark_replay*)*pChunk;

    return WorkerSpark(chunk->field_0x0, &chunk->field_0x4[0], &chunk->field_0x4[0xc]);
}

// FUNCTION: CARMA2_HW 0x004c9160
void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk) {
    tPipe_chunk_shrapnel_replay* chunk = (tPipe_chunk_shrapnel_replay*)*pChunk;

    return WorkerShrapnel(chunk->field_0x0, &chunk->field_0x4[0], chunk->field_0x10, chunk->field_0x14);
}

// FUNCTION: CARMA2_HW 0x004c91b0
// FUNCTION: CARMA2_SW 0x004c58e0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);

    AdjustNonCar((*pChunk)->data.non_car.actor, &(*pChunk)->data.non_car.matrix);
}

// FUNCTION: CARMA2_HW 0x004c9190
void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk) {
    tPipe_chunk_groove_stop* chunk = (tPipe_chunk_groove_stop*)*pChunk;

#ifdef REC2_MATCHING
    /* the original passes the raw ints as arguments */
    ((void (C2_HOOK_FASTCALL*)(int, br_vector3*, int, int, int, int))(void*)SetGrooveInterrupt)(chunk->field_0x0, &chunk->field_0x14, chunk->field_0x4, chunk->field_0x8, chunk->field_0xc, chunk->field_0x10);
#else
    SetGrooveInterrupt(chunk->field_0x0, (const br_matrix34*)&chunk->field_0x14, (tInterrupt_status)chunk->field_0x4, (tInterrupt_status)chunk->field_0x8, *(float*)&chunk->field_0xc, *(float*)&chunk->field_0x10);
#endif
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

// FUNCTION: CARMA2_HW 0x004c9330
void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk) {
    tPipe_chunk_oil_spill* chunk = (tPipe_chunk_oil_spill*)*pChunk;

#ifdef REC2_MATCHING
    /* the original passes the raw ints as arguments */
    ((void (C2_HOOK_FASTCALL*)(int, br_matrix34*, int, int, int, int, int, br_vector3*, int))(void*)AdjustOilSpill)(chunk->field_0x0, (br_matrix34*)&chunk->field_0x4, chunk->field_0x34, chunk->field_0x38, chunk->field_0x3c, 0, chunk->field_0x44, &chunk->field_0x48, chunk->field_0x54);
#else
    AdjustOilSpill(chunk->field_0x0, (br_matrix34*)&chunk->field_0x4, *(float*)&chunk->field_0x34, *(float*)&chunk->field_0x38, chunk->field_0x3c, 0, (tCar_spec*)chunk->field_0x44, &chunk->field_0x48, (br_pixelmap*)(intptr_t)chunk->field_0x54);
#endif
}

// GLOBAL: CARMA2_HW 0x00676918
int gDisable_piped_sounds = 0;

// FUNCTION: CARMA2_HW 0x004c9370
void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk) {

    if (gDisable_piped_sounds != 0) return;

    if (((tPipe_chunk_sound*)*pChunk)->field_0x16 == 0) {
        DRS3StartSound2(
            GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
            ((tPipe_chunk_sound*)*pChunk)->field_0x0,
            1, -1, -1,
            (int)(fabs(ARGetReplayRate()) * 65536.0),
            0x10000);
        return;
    }

    if (BrVector3LengthSquared((br_vector3*)&((tPipe_chunk_sound*)*pChunk)->field_0x8) == 0.0f) {
        DRS3StartSound2(
            GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
            ((tPipe_chunk_sound*)*pChunk)->field_0x0,
            1,
            ((tPipe_chunk_sound*)*pChunk)->field_0x16 & 0xff,
            (((tPipe_chunk_sound*)*pChunk)->field_0x16 >> 8) & 0xff,
            (int)(fabs(ARGetReplayRate()) * (double)((tPipe_chunk_sound*)*pChunk)->field_0x4),
            0x10000);
    } else {
        DRS3StartSound3D(
            GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
            ((tPipe_chunk_sound*)*pChunk)->field_0x0,
            (br_vector3*)((tU8*)&((tPipe_chunk_sound*)*pChunk)->field_0x8),
            &gZero_vector__smash,
            1,
            ((tPipe_chunk_sound*)*pChunk)->field_0x16,
            (int)(fabs(ARGetReplayRate()) * (double)((tPipe_chunk_sound*)*pChunk)->field_0x4),
            0x10000);
    }
}

// FUNCTION: CARMA2_HW 0x004c9360
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {

    return ApplyTime(((tPipe_chunk_frame_bound*)*pChunk)->field_0x4);
}

// ApplyCar

// FUNCTION: CARMA2_HW 0x004c96c0
void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x0;

    if ((code & 0xffffff00u) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    *(float*)((tU8*)car + 0x4a8) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x6 * 0.001181102362);
    *(float*)((tU8*)car + 0x4ac) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x7 * 0.001181102362);
    *(float*)((tU8*)car + 0x4b0) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x8 * 0.001181102362);
    *(float*)((tU8*)car + 0x4b4) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x9 * 0.001181102362);
    *(float*)((tU8*)car + 0x4a0) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x4 * 0.001831110569);
}

// FUNCTION: CARMA2_HW 0x004c9770
void C2_HOOK_FASTCALL ApplyDamage(tPipe_chunk** pChunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_damage*)*pChunk)->field_0x0;
    int* p;
    int i;

    if ((code & 0xffffff00u) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    p = (int*)((tU8*)car + 0x4e0);

    for (i = 0; i < 12; i++) {
        *p += ((tPipe_chunk_damage*)*pChunk)->field_0x4[i];
        p = (int*)((tU8*)p + 0x2c);
    }
}

// FUNCTION: CARMA2_HW 0x004c97c0
void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk) {
    tU32 type = *(tU32*)*pChunk;

    switch (type) {
        case 0:
            if (fabs(ARGetReplayRate()) <= 1.0) {
                return FadePaletteDown();
            }
            return;
        case 1:
            return WorkerSpecial();
    }
}

// FUNCTION: CARMA2_HW 0x004c9890
// FUNCTION: CARMA2_SW 0x004c5fc0
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    chunk = *pChunk;
    BrMatrix34Identity(&chunk->data.identity.actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c97f0
void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk) {
    tPipe_chunk_prox_ray_replay* chunk = (tPipe_chunk_prox_ray_replay*)*pChunk;

    return WorkerProxRay(chunk->field_0x0, chunk->field_0x6, chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9810
void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk) {
    tPipe_chunk_skid_adjust* chunk = (tPipe_chunk_skid_adjust*)*pChunk;
    AdjustSkid(chunk->skid_num, &chunk->matrix, chunk->material);
}

// FUNCTION: CARMA2_HW 0x004c9830
void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk) {

    return WorkerFlap(*(int*)((tU8*)*pChunk + 4), *(tU16*)((tU8*)*pChunk + 8));
}

// FUNCTION: CARMA2_HW 0x004c9840
void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return ARApplyModelMash((br_model*)chunk->field_0x4, (tCompressed_vector3*)chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9850
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {
    tPipe_chunk_relink* chunk = (tPipe_chunk_relink*)*pChunk;
    br_actor* actor = chunk->actor;
    br_actor* parent = chunk->field_0xc;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            RelinkActorKeepWorldTransform(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c98b0
void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk) {
    tU32 fn = *(tU32*)*pChunk;

    if (*(int*)((tU8*)*pChunk + 0x14)) {
        if ((fn & 0xffffff00u) == 0) {
            return WorkerSplitWeldApply(&gProgram_state.current_car, (br_vector3*)((tU8*)*pChunk + 4));
        }

        return WorkerSplitWeldApply(GetCarSpec(fn >> 8, fn & 0xffu), (br_vector3*)((tU8*)*pChunk + 4));
    }

    if ((fn & 0xffffff00u) == 0) {
        return WorkerSplitWeldUndo(&gProgram_state.current_car);
    }

    return WorkerSplitWeldUndo(GetCarSpec(fn >> 8, fn & 0xffu));
}

// FUNCTION: CARMA2_HW 0x004c9930
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9940
// FUNCTION: CARMA2_SW 0x004c6070
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9950
void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk) {
}

// GLOBAL: CARMA2_HW 0x006b78a0
br_matrix34 gShrapnel_shower_matrix;

// FUNCTION: CARMA2_HW 0x004c9960
void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk) {
    tPipe_chunk_shrapnel_shower* chunk = (tPipe_chunk_shrapnel_shower*)*pChunk;

    if (chunk->field_0x84 == 0) {
        return;
    }

    memcpy(&gShrapnel_shower_matrix, &chunk->field_0xa4, sizeof(chunk->field_0xa4));

    return WorkerShrapnelShower(((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x4, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x44, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x8, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x20, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x2c, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x50, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x48, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x54, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x88, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x14, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0xa0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x38, 0);
}

// FUNCTION: CARMA2_HW 0x004c99d0
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {
    tPipe_chunk_explosion* chunk = (tPipe_chunk_explosion*)*pChunk;

    return CopyExplosionData(chunk->field_0x0, (tExplosion*)&chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c99e0
void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk) {
    tPipe_chunk_non_car_creation* chunk = (tPipe_chunk_non_car_creation*)*pChunk;

    return TurnOnNonCar(chunk->field_0x0);
}

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

// FUNCTION: CARMA2_HW 0x004c9a10
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {
    tPipe_chunk_remove_faces* chunk = (tPipe_chunk_remove_faces*)*pChunk;

    return SmashRemoveSomeFaces((br_model*)(intptr_t)chunk->field_0x0, (int)chunk->count);
}

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

// FUNCTION: CARMA2_HW 0x004c9a50
void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk) {

    return WorkerRepulseRay(*(int*)*pChunk, *(int*)((tU8*)*pChunk + 4), *(int*)((tU8*)*pChunk + 8), *(int*)((tU8*)*pChunk + 0xc));
}

// FUNCTION: CARMA2_HW 0x004c9a70
void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk) {
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->x += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[0];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->y += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[1];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->z += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004c9aa0
void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_status* chunk = (tPipe_chunk_ped_status*)*pChunk;

    return WorkerPedStatus(chunk->field_0x0, chunk->field_0x4[0], chunk->field_0x4[1], chunk->field_0x4[2], chunk->field_0x4[3], chunk->field_0x4[5], chunk->field_0x4[7], &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004c9ad0
void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_dir* chunk = (tPipe_chunk_ped_dir*)*pChunk;

    return WorkerPedDir(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9ae0
void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    return WorkerPedMove(chunk->field_0x0, chunk->field_0x4, chunk->field_0xc, chunk->field_0x8 & 1, chunk->field_0x14, 0, &chunk->field_0x24, chunk->field_0x34, &chunk->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004c9b10
void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_physics* chunk = (tPipe_chunk_ped_physics*)*pChunk;

    return WorkerPedPhysics(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b20
void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_dismember* chunk = (tPipe_chunk_ped_dismember*)*pChunk;

    return WorkerPedDismember(chunk->field_0x0, (int)chunk->field_0x4, (int)chunk->field_0x7);
}

// FUNCTION: CARMA2_HW 0x004c9b40
void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return WorkerPedFPChange(chunk->field_0x0, *(tPipe_stack_word*)&chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9b50
void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_pos* chunk = (tPipe_chunk_ped_pos*)*pChunk;

    return WorkerPedPos(chunk->field_0x0, &chunk->field_0x10);
}

// FUNCTION: CARMA2_HW 0x004c9b60
void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_model_change* chunk = (tPipe_chunk_ped_model_change*)*pChunk;

    return WorkerPedModelChange(chunk->field_0x0, (int)chunk->field_0x4, (int)chunk->field_0x6, (int)chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9b80
void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk) {
    tPipe_chunk_phil_object* chunk = (tPipe_chunk_phil_object*)*pChunk;

    return WorkerPHILObject(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b90
void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk) {
    tPipe_chunk_gib_shower* chunk = (tPipe_chunk_gib_shower*)*pChunk;

    return WorkerGibShower(chunk->field_0x0, chunk->field_0xc, chunk->field_0x8, chunk->field_0x10, chunk->field_0x4, &chunk->field_0x14, &chunk->field_0x20, &chunk->field_0x2c);
}

// ApplyBloodSpurt

// FUNCTION: CARMA2_HW 0x004c9be0
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {
    tPipe_chunk_groove_on_off* chunk = (tPipe_chunk_groove_on_off*)*pChunk;

    if (chunk->field_0x4 != 0) {
        return EnableGroovidelic((br_actor*)(intptr_t)chunk->field_0x0, chunk->field_0x8);
    }
    return DisableGroovidelic((br_actor*)(intptr_t)chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9c00
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupGot(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c10
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupLose(chunk->field_0x0, chunk->field_0x4);
}

// ApplyFunkEnable

// FUNCTION: CARMA2_HW 0x004c9c30
void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk) {
    tPipe_chunk_vanish* chunk = (tPipe_chunk_vanish*)*pChunk;

    DoVanishDismembered(chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9c40
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk = *pChunk;

    if (*(tS16*)((tU8*)chunk + 4) != 0) {
        MakeModelMaterialsDoubleSided((br_model*)*(void**)chunk);
    } else {
        DRActorEnumRecurse(((tCar_spec*)*(void**)chunk)->car_model_actor, (br_actor_enum_cbfn*)MakeCarModelsMaterialsSingleSided, *(void**)chunk);
    }
}

// FUNCTION: CARMA2_HW 0x004c9c70
void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;

    return WorkerPedDiagnostics(*(void**)chunk, chunk + 4, chunk + 0x58, chunk + 0x144);
}

// FUNCTION: CARMA2_HW 0x004c9c90
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[0] += ((tPipe_chunk_vector3*)*pChunk)->delta.v[0];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[1] += ((tPipe_chunk_vector3*)*pChunk)->delta.v[1];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[2] += ((tPipe_chunk_vector3*)*pChunk)->delta.v[2];
}

// ApplyDroneRender

// FUNCTION: CARMA2_HW 0x004c9cd0
void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk) {
    tPipe_chunk_drone_corner_pos* chunk = (tPipe_chunk_drone_corner_pos*)*pChunk;

    return WorkerDroneCornerPos(chunk->field_0x0, chunk->field_0x4, chunk->field_0x6, chunk->field_0x8, &chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004c9cf0
void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk) {
    tPipe_chunk_drone_straight_pos* chunk = (tPipe_chunk_drone_straight_pos*)*pChunk;

    return WorkerDroneStraightPos(chunk->field_0x0, chunk->field_0x4, chunk->field_0x8, chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004c9d10
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9d20
void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk) {
    tPipe_chunk_napalm_bolt* chunk = (tPipe_chunk_napalm_bolt*)*pChunk;

    return AdjustNapalmBolt(chunk->field_0x0, (tPedestrian*)(intptr_t)chunk->field_0x4, &chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9d40
void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupRespawn(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d50
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d60
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerEndShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d70
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {
    ((br_actor*)((tPipe_chunk_transform_type*)*pChunk)->pOwner)->t.type = ((tPipe_chunk_transform_type*)*pChunk)->type_to;
}

// FUNCTION: CARMA2_HW 0x004c9d80
void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk) {
    SetOppoRender((tOpponent_spec*)*(tU32*)*pChunk, *(tU16*)((tU8*)*pChunk + 4));
}

// FUNCTION: CARMA2_HW 0x004c9d90
void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_crush* chunk = (tPipe_chunk_crush*)*pChunk;

    return WorkerCrushUndo(chunk->field_0x4, &chunk->field_0x8[0], &chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9db0
void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    return WorkerSmudge(pChunk, -1);
}

// FUNCTION: CARMA2_HW 0x004c9dd0
void C2_HOOK_FASTCALL UndoDamage(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_damage*)*pChunk)->field_0x0;
    int* p;
    int i;

    if ((code & 0xffffff00u) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    p = (int*)((tU8*)car + 0x4e0);

    for (i = 0; i < 12; i++) {
        *p -= ((tPipe_chunk_damage*)*pChunk)->field_0x4[i];
        p = (int*)((tU8*)p + 0x2c);
    }
}

// UndoSpecial

// FUNCTION: CARMA2_HW 0x004c9e50
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_screen_wobble* wobble = (tPipe_chunk_screen_wobble*)((tU8*)pPrev_chunk + 4);
        SetScreenWobble(wobble->wobble_x, wobble->wobble_y);
    } else {
        SetScreenWobble(0, 0);
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9e80
void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_splash* chunk = (tPipe_chunk_splash*)pPrev_chunk;
        ((tReplay_splash_effect*)chunk->pOwner)->size = ((tPipe_chunk_splash*)pPrev_chunk)->v[0];
        ((tReplay_splash_effect*)chunk->pOwner)->x = ((tPipe_chunk_splash*)pPrev_chunk)->v[1];
        ((tReplay_splash_effect*)chunk->pOwner)->y = ((tPipe_chunk_splash*)pPrev_chunk)->v[2];
        ((tReplay_splash_effect*)chunk->pOwner)->z = ((tPipe_chunk_splash*)pPrev_chunk)->v[3];
    } else {
        ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->size = 10000.0f;
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9ee0
void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)pPrev_chunk;

        WorkerExtendedSplash(chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, &chunk->field_0xc, &chunk->field_0x18);
    } else {
        ClearSplashReplay(*(uintptr_t*)*pChunk);
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9f20
void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_skid_adjust* chunk = (tPipe_chunk_skid_adjust*)pPrev_chunk;
        AdjustSkid(chunk->skid_num, &chunk->matrix, chunk->material);
    } else {
        HideSkid(((tPipe_chunk_skid_adjust*)*pChunk)->skid_num);
    }
    ClearPipeUndoFlag();
}

// UndoFlap

// FUNCTION: CARMA2_HW 0x004c9f80
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return WorkerModelMashUndo(chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9f90
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_relink* chunk = (tPipe_chunk_relink*)*pChunk;
    br_actor* actor = chunk->actor;
    br_actor* parent = (br_actor*)chunk->field_0x8;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            RelinkActorKeepWorldTransform(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9fd0
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tU32 fn;
    tCar_spec* car;

    if (*(int*)((tU8*)*pChunk + 0x14)) {
        fn = *(tU32*)*pChunk;
        if ((fn & 0xffffff00u) == 0) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec(fn >> 8, fn & 0xffu);
        }
        WorkerSplitWeldUndo(car);
    } else {
        fn = *(tU32*)*pChunk;
        if ((fn & 0xffffff00u) == 0) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec(fn >> 8, fn & 0xffu);
        }
        WorkerSplitWeldApply(car, (br_vector3*)((tU8*)*pChunk + 4));
    }
}

// FUNCTION: CARMA2_HW 0x004ca050
void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
}

// FUNCTION: CARMA2_HW 0x004ca060
void C2_HOOK_FASTCALL UndoUnBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
}

// FUNCTION: CARMA2_HW 0x004ca070
void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shrapnel_shower* prev = (tPipe_chunk_shrapnel_shower*)pPrev_chunk;

    if (((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84 != 0 || prev == NULL || prev->field_0x4 == 0 || prev->field_0x84 == 0) {
        return;
    }

    memcpy(&gShrapnel_shower_matrix, &prev->field_0xa4, sizeof(prev->field_0xa4));

    return WorkerShrapnelShowerUndo(((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x48, prev->field_0x4, prev->field_0x84, prev->field_0x44, &prev->field_0x8, &prev->field_0x20, &prev->field_0x2c, prev->field_0x50, prev->field_0x48, prev->field_0x0, &prev->field_0x54, &prev->field_0x88, &prev->field_0x14, prev->field_0xa0, &prev->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004ca100
void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_non_car_creation* chunk = (tPipe_chunk_non_car_creation*)*pChunk;

    return TurnOffNonCar(chunk->field_0x0);
}

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

// FUNCTION: CARMA2_HW 0x004ca130
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_remove_faces_replay* chunk = (tPipe_chunk_remove_faces_replay*)*pChunk;

    return WorkerUndoSmashRemoveFaces(chunk->field_0x0, chunk->field_0x8, &chunk->field_0xc, chunk->field_0x4);
}

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

// FUNCTION: CARMA2_HW 0x004ca180
void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->x -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[0];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->y -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[1];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->z -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004ca1b0
void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_status* chunk = (tPipe_chunk_ped_status*)*pChunk;

    return WorkerPedStatusUndo(chunk->field_0x0, chunk->field_0x4[0], chunk->field_0x4[1], chunk->field_0x4[2], chunk->field_0x4[3], chunk->field_0x4[4], chunk->field_0x4[6], &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004ca1e0
void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    if (!chunk->field_0x14) {
        return WorkerPedMoveUndo(chunk->field_0x0, chunk->field_0x6, chunk->field_0x10, 0, 0, chunk->field_0xa, &chunk->field_0x18, chunk->field_0x30, &chunk->field_0x38);
    }
    return;
}

// FUNCTION: CARMA2_HW 0x004ca210
void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_dismember* chunk = (tPipe_chunk_ped_dismember*)*pChunk;

    return WorkerPedDismemberUndo(chunk->field_0x0, (int)chunk->field_0x4, (int)(*(tS8*)((tU8*)chunk + 6)));
}

// FUNCTION: CARMA2_HW 0x004ca230
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return WorkerPedFPChange(chunk->field_0x0, *(tPipe_stack_word*)&chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca240
void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_pos* chunk = (tPipe_chunk_ped_pos*)*pChunk;

    return WorkerPedPos(chunk->field_0x0, (br_vector3*)&chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca250
void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_model_change* chunk = (tPipe_chunk_ped_model_change*)*pChunk;

    return WorkerPedModelChangeUndo(chunk->field_0x0, (int)chunk->field_0x4, chunk->field_0xc, (int)chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004ca270
void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_gib_shower* prev = (tPipe_chunk_gib_shower*)pPrev_chunk;

    if (prev == NULL) {
        return;
    }

    return WorkerGibShowerUndo(((tPipe_chunk_gib_shower*)*pChunk)->field_0xc, prev->field_0x0, prev->field_0xc, prev->field_0x8, prev->field_0x10, prev->field_0x4, &prev->field_0x14, &prev->field_0x20, &prev->field_0x2c);
}

// UndoBloodSpurt

// FUNCTION: CARMA2_HW 0x004ca2d0
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_groove_on_off* chunk = (tPipe_chunk_groove_on_off*)*pChunk;

    if (chunk->field_0x4 != 0) {
        return DisableGroovidelic((br_actor*)(intptr_t)chunk->field_0x0, chunk->field_0x8);
    }
    return EnableGroovidelic((br_actor*)(intptr_t)chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca2f0
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupLose(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca300
void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupGot(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca310
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_funk_enable* chunk = (tPipe_chunk_funk_enable*)*pChunk;

    return AdjustFunkEnable(chunk->field_0x0, chunk->field_0x4 == 0);
}

// FUNCTION: CARMA2_HW 0x004ca330
void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_vanish* chunk = (tPipe_chunk_vanish*)*pChunk;

    DoVanishDismembered(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca360
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[0] -= ((tPipe_chunk_vector3*)*pChunk)->delta.v[0];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[1] -= ((tPipe_chunk_vector3*)*pChunk)->delta.v[1];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[2] -= ((tPipe_chunk_vector3*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004ca390
void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    if (pPrev_chunk != NULL) {
        tPipe_chunk_napalm_bolt* chunk = (tPipe_chunk_napalm_bolt*)*pChunk;
        tPipe_chunk_napalm_bolt* prev = (tPipe_chunk_napalm_bolt*)pPrev_chunk;

        if (prev->field_0x4 == chunk->field_0x4) {
            return AdjustNapalmBolt(prev->field_0x0, (tPedestrian*)(intptr_t)prev->field_0x4, &prev->field_0x8);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca3b0
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupRespawnUndo(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3c0
void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerEndShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3d0
void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3e0
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((br_actor*)((tPipe_chunk_transform_type*)*pChunk)->pOwner)->t.type = ((tPipe_chunk_transform_type*)*pChunk)->type_from;
}

// FUNCTION: CARMA2_HW 0x004ca3f0
int C2_HOOK_FASTCALL CalcCrushLength(tPipe_chunk* pChunk) {
    br_model* model = *(br_model**)((tU8*)pChunk + 4);
    return 0xa + 2 * model->nvertices;
}

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

// FUNCTION: CARMA2_HW 0x004ca430
int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk) {
    br_model* model = *(br_model**)((tU8*)pChunk + 4);
    return 4 + 6 * model->nvertices;
}

// CalcEndMyBendLength

// CalcRemoveFacesLength

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
void C2_HOOK_FASTCALL FUN_DisposeReplayBuffer(void* pBuffer);

// FUNCTION: CARMA2_HW 0x004024a0
void C2_HOOK_FASTCALL DisposePiping(void) {

    if (gPipe_buffer_start != NULL) {
        FUN_DisposeReplayBuffer(gPipe_buffer_start);
    }
    gPipe_buffer_start = NULL;
    if (gLocal_buffer != NULL) {
        BrMemFree(gLocal_buffer);
        gLocal_buffer = NULL;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
