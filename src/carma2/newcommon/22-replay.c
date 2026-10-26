#include "22-replay.h"

#include "globvars.h"
#include "30-opponent.h"
#include "18-graphics2.h"
#include "23-piping.h"

// GLOBAL: CARMA2_HW 0x0058f620
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x006a2374
int gKey_down;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004e6280
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e6260
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {

    RemoveTransientBitmaps(1);
}
// STUB: CARMA2_HW 0x004e6900
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e6270
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {

    ActualActionReplayHeadups(0);
}

// STUB: CARMA2_HW 0x004e6950
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {

    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e6980
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) {

    NOT_IMPLEMENTED();
}

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
extern int gEntering_message;
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);

// FUNCTION: CARMA2_HW 0x004e6ff0
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {

    if (!gAction_replay_mode) {
        if (!KeyIsDown(58) || gEntering_message) {
            gKey_down = -1;
        } else if (gKey_down == -1) {
            if (gAction_replay_mode) {
                RenderAFrame(1);
                RenderAFrame(1);
            }
            if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
                    DoZappyActionReplayHeadups, PreProcess, PostProcess, NULL, NULL)) {
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
            }
        }
    }
}

// InitializeActionReplayPix

// FreeActionReplayPix

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004922a0
void C2_HOOK_FASTCALL ServiceGameInRace(void) {

    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a23c4
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a23d4
tU32 gLast_synch_time;

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);

// FUNCTION: CARMA2_HW 0x004e7110
void C2_HOOK_FASTCALL SynchronizeActionReplay(void) {

    while (ARGetReplayRate() != 0.f && (double)(tU32)(PDGetTotalTime() - gLast_synch_time) < (double)gFrame_period / fabs(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
}

// STUB: CARMA2_HW 0x004e71c0
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {

    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e7270
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e72d0
void C2_HOOK_FASTCALL AfterActionReplay(void) {

    ForceRebuildActiveCarList();
}
// FUNCTION: CARMA2_HW 0x004e72e0
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) {

    if (gAction_replay_mode) {
        RenderAFrame(1);
        RenderAFrame(1);
    }
    if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
            DoZappyActionReplayHeadups, PreProcess, PostProcess, pArg1, pArg2)) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
    }
}

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gReplay_pause_state = pPause;
}

// Replay pipe workers (no PDB symbols; names from carpocalypse2). Kept in this TU so that
// the Apply/Undo wrappers in 23-piping.c tail-call them instead of inlining.

// FUNCTION: CARMA2_HW 0x0043bd70
void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043be50
void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe) {

    NOT_IMPLEMENTED();
}

/* 0x0043bf80 is ARApplyModelMash (13-crush1.c) */
void C2_HOOK_FASTCALL WorkerModelMash(int field_0x4, tU8* pField_0x8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043c030
void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043c0e0
void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tU16 field_0x8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00452880
void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004529d0
void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, tU16 field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004b7220
void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9070
void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca4d0
void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca500
void C2_HOOK_FASTCALL WorkerPedMove(int field_0x0, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca5a0
void C2_HOOK_FASTCALL WorkerPedMoveUndo(int field_0x0, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca650
void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca680
void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca6e0
void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca7d0
void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca830
void C2_HOOK_FASTCALL WorkerPedPos(int field_0x0, br_vector3* pVec) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca890
void C2_HOOK_FASTCALL DoVanishDismembered(uintptr_t pOwner, tS32 value) {
    *((tS32*)(*(undefined4**)pOwner) + 4) = value;
}

// FUNCTION: CARMA2_HW 0x004ca8a0
void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca8d0
void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca900
void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca980
void C2_HOOK_FASTCALL WorkerPedDismemberUndo(int field_0x0, int field_0x4, int field_0x6) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ccae0
void C2_HOOK_FASTCALL WorkerGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ccc40
void C2_HOOK_FASTCALL WorkerGibShowerUndo(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d6180
void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d6db0
void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004da960
void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de000
void C2_HOOK_FASTCALL WorkerShitMine(int field_0x0, br_vector3* pPos) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de020
void C2_HOOK_FASTCALL WorkerEndShitMine(int field_0x0, br_vector3* pPos) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004df200
void C2_HOOK_FASTCALL WorkerPowerupGot(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004df210
void C2_HOOK_FASTCALL WorkerPowerupLose(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e0710
void C2_HOOK_FASTCALL WorkerSpecial(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e07d0
void C2_HOOK_FASTCALL WorkerPowerupRespawn(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e07e0
void C2_HOOK_FASTCALL WorkerPowerupRespawnUndo(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004eb8f0
void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 field_0x4, tU32 field_0x84, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f0380
void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f5fa0
void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f8630
void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f86f0
void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fb580
void C2_HOOK_FASTCALL WorkerSmokeColumn(int column_flags, br_vector3* pPos, int vertex, int pFlags_hi) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fba00
void C2_HOOK_FASTCALL WorkerFlame(int index, int flag, float f1, float f2, float f3, float f4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fef40
void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e0730
void C2_HOOK_FASTCALL WorkerSpecialUndo(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0040ca40
void C2_HOOK_CDECL RelinkActorKeepWorldTransform(br_actor* pParent, br_actor* pActor) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043c240
void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* roadMachine, br_vector3* pVector) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043c3d0
void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine) {

    NOT_IMPLEMENTED();
}
