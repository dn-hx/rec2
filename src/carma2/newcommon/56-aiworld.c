#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "57-drone.h"
#include "08-loading1.h"
#include "globvars.h"
#include "41-utility.h"
#include "platform.h"
#include "30-opponent.h"

// GLOBAL: CARMA2_HW 0x0058f038
int gFirst_drone_processing = 1;

// GLOBAL: CARMA2_HW 0x0079efc4
tU32 gFrame_period_for_this_munging;

// GLOBAL: CARMA2_HW 0x0079efc0
float gDrone_delta_time;

// GLOBAL: CARMA2_HW 0x0079efbc
float gTrack_drone_min_y;

// MakeVertexAndOffsetIt

// MakeFaceAndTextureIt

// FUNCTION: CARMA2_HW 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
}

// LoadAIWorldTrackInfo

// FUNCTION: CARMA2_HW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0,"BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars(pRace_info);
    PrintMemoryDump(0,"AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0,"AFTER LoadPerRaceDroneStuff()");
}

// STUB: CARMA2_HW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {

    NOT_IMPLEMENTED();
}

// MungeAIWorld

// PointActorAlongThisBloodyVector

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
