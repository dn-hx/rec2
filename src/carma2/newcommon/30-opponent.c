#include "30-opponent.h"

#include <c2_hooks.h>
#include "00-car.h"
#include "08-loading1.h"
#include "globvars.h"
#include "globvrpb.h"
#include "60-phil.h"
#include "23-piping.h"
#include "rec2_macros.h"
#include "41-utility.h"

// GLOBAL: CARMA2_HW 0x0065a3c4
// GLOBAL: CARMA2_SW 0x00654cdc
int gBIG_APC_index = -1;

extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);

// GLOBAL: CARMA2_HW 0x0065a3cc
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0074a684
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
int gMaxTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x00691744
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x0069173c
int gActive_car_list_rebuild_required;

// GLOBAL: CARMA2_HW 0x00691764
br_scalar gIn_view_distance;

// GLOBAL: CARMA2_HW 0x0069175c
// GLOBAL: CARMA2_SW 0x0068b7b4
tU8* gBit_per_node;

// GLOBAL: CARMA2_HW 0x0058a478
float gOppo_render_zero = 0.0f;

// FUNCTION: CARMA2_HW 0x004a8160
void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...) {

}

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

// FUNCTION: CARMA2_HW 0x004aa400
int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo) {
    br_vector3 norm;
    br_vector3 dir;
    br_vector3 from;
    br_material* material;
    br_scalar t;

    BrVector3Sub(&dir, pTo, pFrom);
    BrVector3Copy(&from, pFrom);
    from.v[1] += 0.15f;
    dir.v[1] += 0.15f;
    FindFace(&from, &dir, &norm, &t, &material);
    return t > 1.f;
}
// WeightedFindNearestNodeAndSection

// FindNearestGeneralSection

// FindNearestPathSection

// DeadStopCar

// FUNCTION: CARMA2_HW 0x004a7960
void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec) {
    pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    PipeSingleOppoRenderage(pOpponent_spec, 1);
    if (!pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 1;
        gActive_car_list_rebuild_required = 1;
    }
}
// FUNCTION: CARMA2_HW 0x004a79a0
void C2_HOOK_FASTCALL TurnOpponentPhysicsOff(tOpponent_spec* pOpponent_spec) {
    tCar_spec* car = pOpponent_spec->car_spec;

    car->acc_force = 0.f;
    car->brake_force = 0.f;
    car->curvature = 0.f;
    car->gear = 0.f;
    car->revs = 0.f;
    BrVector3Set(&car->collision_info->omega, 0.f, 0.f, 0.f);
    BrVector3Set(&car->collision_info->v, 0.f, 0.f, 0.f);
    if (pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 0;
        pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_NONE;
        PipeSingleOppoRenderage(pOpponent_spec, 0);
        gActive_car_list_rebuild_required = 1;
    }
}
// ApplyOppoRenderage2

// NewObjective

// CalcRaceRoute

// TopUpRandomRoute

// SearchForSection

// UpdatePlayersSection

// AllowForDecel

// FUNCTION: CARMA2_HW 0x004ab100
// FUNCTION: CARMA2_SW 0x004a7270
int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    if (pOpponent_spec->nnext_sections > pPlaces) {
        for (i = 0; i < (int)REC2_ASIZE(pOpponent_spec->next_sections) - pPlaces; i++) {
            pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
            pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
        }
        pOpponent_spec->nnext_sections -= pPlaces;
        return 1;
    } else {
        return 0;
    }
}

// NewCalcGetNearPlayerRoute

// CalcReturnToStartPointRoute

// ClearOpponentsProjectedRoute

// FUNCTION: CARMA2_HW 0x004ab150
int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {

    if (pOpponent_spec->nnext_sections < (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no = pSection_no;
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].direction = pDirection;
        pOpponent_spec->nnext_sections += 1;
        return 1;
    }
    return 0;
}
// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

// FUNCTION: CARMA2_HW 0x004abc00
// FUNCTION: CARMA2_SW 0x004a7d70
void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        return;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        return;
    case ePOC_die:
        return;
    }
}

// ProcessReturnToStart

// ProcessLevitate

// ProcessGetNearPlayer

// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

// ObjectiveComplete

// TeleportOpponentToNearestSafeLocation

// ChooseNewObjective

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// AddIfNotInList

// FUNCTION: CARMA2_HW 0x004a7a20
void C2_HOOK_FASTCALL SetOppoRender(tOpponent_spec* pOpponent_spec, int pRender) {
    if (gAction_replay_mode && ARGetReplayRate() < gOppo_render_zero) {
        pRender = (pRender == 0);
    }
    pOpponent_spec->car_spec->car_master_actor->render_style = (pRender == 0) ? BR_RSTYLE_NONE : BR_RSTYLE_DEFAULT;
}

// FUNCTION: CARMA2_HW 0x004a7ef0
void C2_HOOK_FASTCALL RemoveAnythingStillInList(tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] != NULL) {
            PHILRemoveObject(pList[i]);
            pList[i]->field183_0x1d8 = 0;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004a7a80
void C2_HOOK_FASTCALL RebuildActiveCarList(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a7a60
void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {

    gActive_car_list_rebuild_required = 1;
    if (gProgram_state.racing) {
        RebuildActiveCarList();
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// OpponentRepairNecessary

// PossiblyRepairOpponent

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

// CalcDistanceFromHome

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

// FUNCTION: CARMA2_HW 0x004a9550
void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec) {

    BrVector3Sub(&pOpponent_spec->player_to_oppo_v, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    pOpponent_spec->player_to_oppo_d = BrVector3Length(&pOpponent_spec->player_to_oppo_v);
    if (pOpponent_spec->player_to_oppo_d < gIn_view_distance) {
        pOpponent_spec->last_in_view = gTime_stamp_for_this_munging;
    }
}
// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

// FUNCTION: CARMA2_HW 0x004a9600
void C2_HOOK_FASTCALL LoadCopCars(tRace_info* pRace_info) {

    gProgram_state.AI_vehicles.number_of_cops = 0;
}

// LoadInOppoPaths

// FUNCTION: CARMA2_HW 0x004a9c50
// FUNCTION: CARMA2_SW 0x004a5dc0
void C2_HOOK_FASTCALL DisposeOpponentPaths(void) {

    if (gProgram_state.AI_vehicles.path_nodes != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_nodes);
    }
    if (gProgram_state.AI_vehicles.path_sections != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_sections);
    }
    if (gBit_per_node != NULL) {
        BrMemFree(gBit_per_node);
    }

    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
}

// MungeOpponents

// InitOpponents

// FUNCTION: CARMA2_HW 0x004ae5d0
// FUNCTION: CARMA2_SW 0x004aa740
void C2_HOOK_FASTCALL DisposeOpponents(void) {
    int i;

    if (gProgram_state.AI_vehicles.number_of_cops != 0) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            DisposeCar(gProgram_state.AI_vehicles.cops[i].car_spec, (i == gBIG_APC_index) ? 99 : 98);
            BrMemFree(gProgram_state.AI_vehicles.cops[i].car_spec);
        }
    }
}

// GLOBAL: CARMA2_HW 0x00691720
int gStart_jumped;

// GLOBAL: CARMA2_HW 0x00691750
int gAcknowledged_start;

// FUNCTION: CARMA2_HW 0x004ae620
void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was) {
    int i;
    int ms;
    tU32 stun_end;
    tOpponent_spec* oppo;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        oppo = &gProgram_state.AI_vehicles.opponents[i];
        oppo->stun_time_ends = 0;
        if (IRandomBetween(1000, 2500) < 1000 * pWhat_the_countdown_was) {
            ms = IRandomBetween(1000, 2500);
        } else {
            ms = 1000 * pWhat_the_countdown_was;
        }
        oppo->car_spec->acc_force = 0.f;
        oppo->car_spec->brake_force = 0.f;
        oppo->car_spec->curvature = 0.f;
        stun_end = gTime_stamp_for_this_munging + ms;
        if (oppo->stun_time_ends > stun_end) {
            stun_end = oppo->stun_time_ends;
        }
        oppo->stun_time_ends = stun_end;
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
        oppo = &gProgram_state.AI_vehicles.cops[i];
        oppo->stun_time_ends = 0;
        if (IRandomBetween(1000, 2500) < 1000 * pWhat_the_countdown_was) {
            ms = IRandomBetween(1000, 2500);
        } else {
            ms = 1000 * pWhat_the_countdown_was;
        }
        oppo->car_spec->acc_force = 0.f;
        oppo->car_spec->brake_force = 0.f;
        oppo->car_spec->curvature = 0.f;
        stun_end = gTime_stamp_for_this_munging + ms;
        if (oppo->stun_time_ends > stun_end) {
            stun_end = oppo->stun_time_ends;
        }
        oppo->stun_time_ends = stun_end;
    }
    gAcknowledged_start = 1;
    gStart_jumped = 1;
}

// FUNCTION: CARMA2_HW 0x004ae790
int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {
    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.number_of_opponents;
    case eVehicle_net_player:
        if (gNet_mode != eNet_mode_none) {
            return gNumber_of_net_players - 1;
        } else {
            return 0;
        }
    case eVehicle_rozzer:
        return gNumber_of_cops_before_faffage;
    case eVehicle_drone:
        return 0;
    case eVehicle_not_really:
        return gNum_active_non_cars;
    }
    return 0;
}

// STUB: CARMA2_HW 0x004ae7e0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    NOT_IMPLEMENTED();
}

// GLOBAL: CARMA2_HW 0x0065a3bc
char* gFaceless_cop_name = "Faceless Cop";

// GLOBAL: CARMA2_HW 0x0065a3c0
char* gInnocent_civilian_name = "Innocent Civilian";

// FUNCTION: CARMA2_HW 0x004ae880
const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex) {
    tCar_spec* c;
    char* name;

    switch (pCategory) {
    case eVehicle_self:
        return gProgram_state.player_name;
    case eVehicle_opponent:
        c = gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
        if (c == NULL || (name = c->driver_name) == NULL) {
            return "???";
        }
        return name;
    case eVehicle_rozzer:
        return gFaceless_cop_name;
    case eVehicle_drone:
        return gInnocent_civilian_name;
    case eVehicle_not_really:
        return NULL;
#ifdef REC2_FIX_BUGS
    case eVehicle_net_player:
        break;
#endif
    }
    return NULL;
}
// FUNCTION: CARMA2_HW 0x004ae8e0
tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;
    int type;

    type = (tU16)pCar_spec->car_ID & ~0xff;
    if (type == 0x200) {
        for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
        }
    } else if (type == 0x300) {
        for (i = 0; i < GetCarCount(eVehicle_rozzer); i++) {
            if (gProgram_state.AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}
// FUNCTION: CARMA2_HW 0x004ae970
tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (gProgram_state.AI_vehicles.opponents[i].index == pIndex) {
            return gProgram_state.AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae9e0
int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no) {

    if (pSection_no >= 20000) {
        return pOpponent_spec->next_sections[pSection_no - 20000].section_no;
    } else if (pSection_no >= 15000) {
        return pSection_no - 15000;
    } else {
        return pSection_no;
    }
}

// FUNCTION: CARMA2_HW 0x004aea00
int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec) {
    if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_trail) {
            return pOpponent_spec->follow_path_data.section_no;
        } else if (pOpponent_spec->pursue_car_data.state == ePCS_following_line_of_sight) {
            return 10000;
        }
    }
    return 20000;
}

// GetOpponentsNextSection

// FUNCTION: CARMA2_HW 0x004aeb90
const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    tS16 section_no;
    tS16 node_no;
    tU8 node_index_index;
    const tRoute_section* route;

    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        route = &pOpponent_spec->next_sections[pSection - 20000];
        node_index_index = route->direction;
        section_no = route->section_no;
        node_no = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[node_index_index == 0];
        return &gProgram_state.AI_vehicles.path_nodes[node_no].pos;
    }

    if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection - 15000];
    }
    if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[0].pos;
    }
    DoNotDprintf("BIG ERROR - GetOpponentsSectionStartNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionStartNodePoint()");
    return NULL;
}

// GetOpponentsSectionFinishNodePoint

// FUNCTION: CARMA2_HW 0x004aece0
float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000) {
        int sections = pOpponent_spec->nnext_sections;
        int s = pSection - 20000;
        if (sections > s) {
            tS16 idx = pOpponent_spec->next_sections[pSection - 20000].section_no;
            return gProgram_state.AI_vehicles.path_sections[idx].width;
        }
    }
    if (pSection >= 15000) {
        return 0.5f;
    }
    if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.width;
    }
    return gProgram_state.AI_vehicles.path_sections[(tS32)pSection].width;
}

// GetOpponentsSectionMinSpeed

// GetOpponentsSectionMaxSpeed

// FUNCTION: CARMA2_HW 0x004aee90
void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {

    gOpponents[pOpponent_index].psyche.grudge_against_player = 0;
}

// ClearTwattageOccurrenceVariables

// FUNCTION: CARMA2_HW 0x004aeeb0
void C2_HOOK_FASTCALL TwoCarsHitEachOther(tCar_spec* pA_car, tCar_spec* pAnother_car) {

    if (pA_car != NULL && pA_car->driver == eDriver_local_human) {
        pAnother_car->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
    }
    if (pAnother_car != NULL && pAnother_car->driver == eDriver_local_human) {
        pA_car->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
    }
}

// RecordOpponentTwattageOccurrence

// GetOpponentMood

