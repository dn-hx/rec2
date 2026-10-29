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

#include <string.h>

extern tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tOpponent_spec* pOpponent_spec, br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance);

// GLOBAL: CARMA2_HW 0x0065a3c4
// GLOBAL: CARMA2_SW 0x00654cdc
int gBIG_APC_index = -1;

extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);

extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);

extern tCar_spec* gCar_to_view;

static int C2_HOOK_FASTCALL NetCarIsActive(const tCar_spec* pCar);

extern void C2_HOOK_FASTCALL AddIfNotInList(tPhysics_object* pObject, tPhysics_object** pList, int pCount);

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
// FUNCTION: CARMA2_HW 0x004a7a80
void C2_HOOK_FASTCALL RebuildActiveCarList(void) {
    int new_count_active_cars = gNum_active_cars;

    if (gActive_car_list_rebuild_required) {
        int i;
        tPhysics_object* used_cars[1000];
        int count_used_cars;
        tPhysics_object* info;
        tPhysics_object* next_info;
        tCar_spec* car;

        gActive_car_list_rebuild_required = 0;

        count_used_cars = 0;
        info = PHILGetFirstObject();
        while (info != NULL) {
            next_info = PHILGetNextObject(info);
            if (info != NULL && info->owner != NULL && info->flags_0x238 == 1 && ((tCar_spec*)info->owner)->driver > 5) {
                used_cars[count_used_cars] = info;
                count_used_cars += 1;
            }
            info = next_info;
        }

        new_count_active_cars = 0;
        for (i = 0; i < GetCarCount(eVehicle_self); i++) {
            if (i > 0) {
                PDFatalError("mGet_car_count( eVehicle_self ) > 1 - I didn't know this could happen!");
            }
            car = &gProgram_state.current_car;
            if (!car->disabled || gAction_replay_mode) {
                gActive_car_list[new_count_active_cars] = car;
                new_count_active_cars += 1;
                car->active = 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            }
        }
        if (gNet_mode == eNet_mode_host) {
            for (i = 0; i < GetCarCount(eVehicle_net_player); i++) {
                if (i < gThis_net_player_index) {
                    car = gNet_players[i].car;
                } else {
                    car = gNet_players[i + 1].car;
                }
                if (!car->disabled) {
                    gActive_car_list[new_count_active_cars] = car;
                    new_count_active_cars += 1;
                    car->active = 1;
                    AddIfNotInList(car->collision_info, used_cars, count_used_cars);
                } else {
                    car->active = 0;
                }
            }
        } else if (gNet_mode == eNet_mode_client) {
            for (i = 0; i < GetCarCount(eVehicle_net_player); i++) {
                if (i < gThis_net_player_index) {
                    car = gNet_players[i].car;
                } else {
                    car = gNet_players[i + 1].car;
                }
                if (!car->disabled && NetCarIsActive(car)) {
                    gActive_car_list[new_count_active_cars] = car;
                    new_count_active_cars += 1;
                    car->active = 1;
                    AddIfNotInList(car->collision_info, used_cars, count_used_cars);
                } else {
                    car->active = 0;
                }
            }
        }

        for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
            car = GetCarSpec(eVehicle_opponent, i);
            if (gProgram_state.AI_vehicles.opponents[i].physics_me || gAction_replay_mode) {
                gActive_car_list[new_count_active_cars] = car;
                new_count_active_cars += 1;
                car->active = 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            } else {
                car->active = 0;
            }
        }

        for (i = 0; i < gNumber_of_cops_before_faffage; i++) {
            car = GetCarSpec(eVehicle_rozzer, i);
            if (gProgram_state.AI_vehicles.cops[i].physics_me || gAction_replay_mode) {
                gActive_car_list[new_count_active_cars] = car;
                new_count_active_cars += 1;
                car->active = 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            } else {
                car->active = 0;
            }
        }
        RemoveAnythingStillInList(used_cars, count_used_cars);
    }
    if (gNum_active_cars != new_count_active_cars) {
        gNum_active_cars = new_count_active_cars;
    }
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

// based on ApexWorm/rec2 77227643f (reference only)
// FUNCTION: CARMA2_HW 0x004ada90
void C2_HOOK_FASTCALL CalcDistanceFromHome(tOpponent_spec* pOpponent_spec) {
    br_vector3 wank;

    BrVector3Sub(&wank, (br_vector3*)pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[3], &pOpponent_spec->start_pos);
    pOpponent_spec->distance_from_home = BrVector3Length(&wank);
}

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

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

// based on ApexWorm/rec2 28019627a (reference only)
// FUNCTION: CARMA2_HW 0x004ae750
void C2_HOOK_FASTCALL ReportMurderToPoliceDepartment(tCar_spec* pCar_spec) {
    int i;

    if (pCar_spec == &gProgram_state.current_car) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            gProgram_state.AI_vehicles.cops[i].murder_reported = 1;
        }
    }
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004ae7e0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// FUNCTION: CARMA2_HW 0x004adae0
void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec) {
    br_vector3 pos_in_cop_space;
    br_matrix34 inverse_transform;

    if (gTime_stamp_for_this_munging <= pOpponent_spec->next_player_visibility_check) {
        return;
    }
    pOpponent_spec->player_in_view_now = 0;
    if (((tU16)pOpponent_spec->car_spec->car_ID & ~0xff) == 0x300) {
        pOpponent_spec->next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 100;
        if (pOpponent_spec->player_to_oppo_d < 20.f) {
            BrMatrix34LPInverse(&inverse_transform, &pOpponent_spec->car_spec->car_master_actor->t.t.mat);
            BrMatrix34ApplyP(&pos_in_cop_space, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &inverse_transform);
            if (pos_in_cop_space.v[2] < gOppo_render_zero && PointVisibleFromHere(&gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
                pOpponent_spec->player_in_view_now = 1;
                pOpponent_spec->acknowledged_piv = 0;
            }
        }
    } else {
        pOpponent_spec->next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 4000;
        DoNotDprintf("%s: Time now: %9.2f; next vis check at %9.2f", pOpponent_spec->car_spec->driver_name, gTime_stamp_for_this_munging * 0.001, pOpponent_spec->next_player_visibility_check * 0.001);
        if (pOpponent_spec->player_to_oppo_d < 50.f && PointVisibleFromHere(&gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
            pOpponent_spec->player_in_view_now = 1;
            pOpponent_spec->acknowledged_piv = 0;
        }
    }
}

extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);

// FUNCTION: CARMA2_HW 0x004a7ea0
void C2_HOOK_FASTCALL AddIfNotInList(tPhysics_object* pObject, tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] == pObject) {
            pList[i] = NULL;
            return;
        }
    }
    PHILAddActiveObject(pObject, NULL, NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x004aa490
void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee) {

    if (pPursuee->no_of_processes_recording_my_trail == 0) {
        DoNotDprintf("StartRecordingTrail - starting from scratch");
        pPursuee->no_of_processes_recording_my_trail = 1;
        pPursuee->my_trail.nodes_shifted_this_frame = 0;
        pPursuee->my_trail.has_deviated_recently = 0;
        pPursuee->my_trail.number_of_nodes = 2;
        pPursuee->my_trail.time_of_next_recording = gTime_stamp_for_this_munging + 500;
        BrVector3Copy(&pPursuee->my_trail.base_heading, &pPursuee->direction);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[0], &pPursuee->car_master_actor->t.t.translate.t);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[1], &pPursuee->car_master_actor->t.t.translate.t);
        pPursuee->my_trail.trail_nodes[0].v[2] += 0.2f;
    } else {
        DoNotDprintf("StartRecordingTrail - another pursuer attaching");
        pPursuee->no_of_processes_recording_my_trail += 1;
    }
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

// FUNCTION: CARMA2_HW 0x004a9610
void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF) {
    br_vector3 dp;
    br_scalar scalars[8];
    br_vector3 pos;
    br_vector3 intersect;
    br_scalar distance;
    char s[256];
    char* line;
    int count;
    int i;
    int j;

    DoNotDprintf("Start of LoadInOppoPaths()...");
    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
    gBIG_APC_index = -1;

    for (;;) {
        if ((line = GetALineAndDontArgue(pF, s)) == NULL) {
            return;
        }
        if (strcmp(s, "START OF OPPONENT PATHS") == 0) {
            break;
        }
    }
    if (line == NULL) {
        return;
    }

    count = GetAnInt(pF);
    if (count != 0) {
        tPath_node* nodes;

        nodes = BrMemAllocate((count + gProgram_state.AI_vehicles.number_of_path_nodes) * sizeof(tPath_node), kMem_oppo_new_nodes);
        memcpy(nodes, gProgram_state.AI_vehicles.path_nodes, gProgram_state.AI_vehicles.number_of_path_nodes * sizeof(tPath_node));

        if (gProgram_state.AI_vehicles.path_nodes != NULL) {
            BrMemFree(gProgram_state.AI_vehicles.path_nodes);
        }
        gProgram_state.AI_vehicles.path_nodes = nodes;
        gProgram_state.AI_vehicles.number_of_path_nodes += count;
    }
    DoNotDprintf("ReallocExtraPathNodes(): Allocated %d bytes for %d path nodes", (count + gProgram_state.AI_vehicles.number_of_path_nodes) * sizeof(tPath_node), count);

    if (gProgram_state.AI_vehicles.number_of_path_nodes != 0) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_path_nodes; i++) {
            GetThreeFloats(pF, &pos.v[1], &pos.v[2], &pos.v[0]);
            gProgram_state.AI_vehicles.path_nodes[i].pos.v[0] = pos.v[1];
            gProgram_state.AI_vehicles.path_nodes[i].pos.v[1] = pos.v[2];
            gProgram_state.AI_vehicles.path_nodes[i].pos.v[2] = pos.v[0];
            gProgram_state.AI_vehicles.path_nodes[i].number_of_sections = 0;
        }
    }

    count = GetAnInt(pF);
    if (count != 0) {
        tPath_section* sections;

        sections = BrMemAllocate((count + gProgram_state.AI_vehicles.number_of_path_sections) * sizeof(tPath_section), kMem_oppo_new_sections);
        memcpy(sections, gProgram_state.AI_vehicles.path_sections, gProgram_state.AI_vehicles.number_of_path_sections * sizeof(tPath_section));

        if (gProgram_state.AI_vehicles.path_sections != NULL) {
            BrMemFree(gProgram_state.AI_vehicles.path_sections);
        }
        gProgram_state.AI_vehicles.path_sections = sections;
        gProgram_state.AI_vehicles.number_of_path_sections += count;
    }
    DoNotDprintf("ReallocExtraPathSections(): Allocated %d bytes for %d path sections", (count + gProgram_state.AI_vehicles.number_of_path_sections) * sizeof(tPath_section), count);

    if (gProgram_state.AI_vehicles.number_of_path_sections != 0) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_path_sections; i++) {
            PossibleService();

            GetNScalars(pF, BR_ASIZE(scalars), scalars);
            gProgram_state.AI_vehicles.path_sections[i].node_indices[0] = (tS16)scalars[0];
            gProgram_state.AI_vehicles.path_sections[i].node_indices[1] = (tS16)scalars[1];
            gProgram_state.AI_vehicles.path_sections[i].min_speed[0] = (tU8)scalars[2];
            gProgram_state.AI_vehicles.path_sections[i].max_speed[0] = (tU8)scalars[3];
            gProgram_state.AI_vehicles.path_sections[i].min_speed[1] = (tU8)scalars[4];
            gProgram_state.AI_vehicles.path_sections[i].max_speed[1] = (tU8)scalars[5];
            gProgram_state.AI_vehicles.path_sections[i].width = scalars[6];

            dp.v[0] = gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[1]].pos.v[0] - gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[0]].pos.v[0];
            dp.v[1] = gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[1]].pos.v[1] - gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[0]].pos.v[1];
            dp.v[2] = gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[1]].pos.v[2] - gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[0]].pos.v[2];
            gProgram_state.AI_vehicles.path_sections[i].length = BrVector3Length(&dp);

            if (scalars[7] >= 1000.f) {
                gProgram_state.AI_vehicles.path_sections[i].type = (tU8)(scalars[7] - 1000.f);
                gProgram_state.AI_vehicles.path_sections[i].one_way = 1;
            } else {
                gProgram_state.AI_vehicles.path_sections[i].type = (tU8)scalars[7];
                gProgram_state.AI_vehicles.path_sections[i].one_way = 0;
            }

            for (j = 0; j < 2; j++) {
                tPath_node* node;

                node = &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[j]];
                if (node->number_of_sections < 8) {
                    node->sections[node->number_of_sections] = (tU16)i;
                    node->number_of_sections += 1;
                } else {
                    DoNotDprintf("ERROR: Too many sections (including section #%d) attached to node #%d", i, gProgram_state.AI_vehicles.path_sections[i].node_indices[j]);
                }
            }
        }
    }

    if (gAusterity_mode != 0 || gNet_mode != eNet_mode_none) {

        gProgram_state.AI_vehicles.number_of_cops = GetAnInt(pF);
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            GetALineAndDontArgue(pF, s);
        }
        gProgram_state.AI_vehicles.number_of_cops = 0;
    } else {

        gProgram_state.AI_vehicles.number_of_cops = GetAnInt(pF);
        if (gProgram_state.AI_vehicles.number_of_cops != 0) {
            for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
                PossibleService();

                GetNScalars(pF, 6, scalars);
                gProgram_state.AI_vehicles.cop_start_points[i].v[0] = scalars[0];
                gProgram_state.AI_vehicles.cop_start_points[i].v[1] = scalars[1];
                gProgram_state.AI_vehicles.cop_start_points[i].v[2] = scalars[2];

                if (scalars[3] == 9.f && scalars[4] == 9.f && scalars[5] == 9.f) {
                    gBIG_APC_index = i;
                }

                FindNearestGeneralSection(NULL, &gProgram_state.AI_vehicles.cop_start_points[i], &dp, &intersect, &distance);

                gProgram_state.AI_vehicles.cop_start_vectors[i].v[0] = dp.v[2] - gOppo_render_zero;
                gProgram_state.AI_vehicles.cop_start_vectors[i].v[1] = 0.f;
                gProgram_state.AI_vehicles.cop_start_vectors[i].v[2] = gOppo_render_zero - dp.v[0];

                if (gProgram_state.AI_vehicles.cop_start_vectors[i].v[0] * (intersect.v[0] - gProgram_state.AI_vehicles.cop_start_points[i].v[0]) + gProgram_state.AI_vehicles.cop_start_vectors[i].v[1] * (intersect.v[1] - gProgram_state.AI_vehicles.cop_start_points[i].v[1]) + gProgram_state.AI_vehicles.cop_start_vectors[i].v[2] * (intersect.v[2] - gProgram_state.AI_vehicles.cop_start_points[i].v[2]) < gOppo_render_zero) {
                    gProgram_state.AI_vehicles.cop_start_vectors[i].v[0] = -gProgram_state.AI_vehicles.cop_start_vectors[i].v[0];
                    gProgram_state.AI_vehicles.cop_start_vectors[i].v[1] = -gProgram_state.AI_vehicles.cop_start_vectors[i].v[1];
                    gProgram_state.AI_vehicles.cop_start_vectors[i].v[2] = -gProgram_state.AI_vehicles.cop_start_vectors[i].v[2];
                }
            }
        }
    }

    do {
        GetALineAndDontArgue(pF, s);
    } while (strcmp(s, "END OF OPPONENT PATHS") != 0);

    if (gProgram_state.AI_vehicles.number_of_path_sections != 0) {
        gBit_per_node = BrMemAllocate((gProgram_state.AI_vehicles.number_of_path_nodes + 7) / 8, kMem_oppo_bit_per_node);
    } else {
        gBit_per_node = NULL;
    }
    DoNotDprintf("End of LoadInOppoPaths(), totals:");
    DoNotDprintf("Nodes: %d", gProgram_state.AI_vehicles.number_of_path_nodes);
    DoNotDprintf("Sections: %d", gProgram_state.AI_vehicles.number_of_path_sections);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004a82d0
tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tOpponent_spec* pOpponent_spec, br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static int C2_HOOK_FASTCALL NetCarIsActive(const tCar_spec* pCar) {
    br_vector3 tv;

    BrVector3Sub(&tv, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t);
    if (BrVector3LengthSquared(&tv) < 100.f) {
        return 1;
    }
    if (gCar_to_view != &gProgram_state.current_car) {
        BrVector3Sub(&tv, &gCar_to_view->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t);
        return BrVector3LengthSquared(&tv) < 100.f;
    }
    return 0;
}
