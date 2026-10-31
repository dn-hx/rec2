#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "57-drone.h"
#include "08-loading1.h"
#include "globvars.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "41-utility.h"
#include "platform.h"
#include "30-opponent.h"
#include "52-errors.h"
#include "rec2_macros.h"
#include "68-timer.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0058f038
int gFirst_drone_processing = 1;

// GLOBAL: CARMA2_HW 0x0079efc4
tU32 gFrame_period_for_this_munging;

// GLOBAL: CARMA2_HW 0x0079efc0
float gDrone_delta_time;

// GLOBAL: CARMA2_HW 0x0079efbc
float gTrack_drone_min_y;

// MakeVertexAndOffsetIt

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}

// FUNCTION: CARMA2_HW 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
}

extern void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF);
extern void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

// FUNCTION: CARMA2_HW 0x00401030
void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}

// FUNCTION: CARMA2_HW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0,"BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars(pRace_info);
    PrintMemoryDump(0,"AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0,"AFTER LoadPerRaceDroneStuff()");
}

// FUNCTION: CARMA2_HW 0x004010b0
void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info) {
    br_bounds3 track_bounds;

    gFirst_drone_processing = 1;
    gTime_stamp_for_this_munging = GetTotalTime();
    if (gTime_stamp_for_this_munging == 0) {
        gTime_stamp_for_this_munging = PDGetTotalTime();
    }
    gFrame_period_for_this_munging = 100;
    gDrone_delta_time = 0.1f;
    BrActorToBounds(&track_bounds, gTrack_actor);
    if (track_bounds.min.v[1] < track_bounds.max.v[1]) {
        gTrack_drone_min_y = track_bounds.min.v[1];
    } else {
        gTrack_drone_min_y = track_bounds.max.v[1];
    }
    PrintMemoryDump(0, "BEFORE InitOpponents()");
    InitOpponents(pRace_info);
    PrintMemoryDump(0, "AFTER InitOpponents(), BEFORE InitDrones()");
    InitDrones(pRace_info);
    PrintMemoryDump(0, "AFTER InitDrones()");
}

// FUNCTION: CARMA2_HW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {
    DisposeOpponents();
    DisposeDronesRaceStuff();
}

extern int gTimers_stack[8];
extern int gTimers_max_index;
extern int gTimers_stack_size;
extern tTimer gTimers[19];
extern void C2_HOOK_FASTCALL MungeOpponents(void);
extern void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void);
extern void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime);

static void TimersPush(int pType) {
    int prev_type;

    gTimers_stack[gTimers_stack_size] = pType;
    if (gTimers_stack_size == 0) {
        prev_type = gTimers_max_index;
    } else {
        prev_type = gTimers_stack[gTimers_stack_size - 1];
    }
    gTimers[prev_type].durations[gTimers[prev_type].index] += PDGetMicroseconds() - gTimers[prev_type].start_time;
    gTimers[pType].start_time = PDGetMicroseconds();
    gTimers_stack_size += 1;
}

static void TimersPop(int pType) {
    int prev_type;

    gTimers[pType].durations[gTimers[pType].index] += PDGetMicroseconds() - gTimers[pType].start_time;
    if (gTimers_stack_size > 1) {
        prev_type = gTimers_stack_size - 1;
    } else {
        prev_type = gTimers_max_index;
    }
    gTimers[prev_type].start_time = PDGetMicroseconds();
    gTimers_stack_size -= 1;
}

/* local declarations for MungeOpponents (no new includes/externs in headers) */
extern int gNumber_of_cops_before_faffage;
extern int gAcknowledged_start;
extern int gStart_jumped;
extern int gNum_of_opponents_pursuing;
extern int gNum_of_opponents_getting_near;
extern int gNum_of_opponents_completing_race;
extern tU32 gNext_grudge_reduction;
extern int gGrudge_reduction_per_period;
extern int gFirst_frame;
extern tU32 gAcme_frame_count;
extern tOpponent* gOpponents;
extern void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee);
extern br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees);
extern void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pResult, br_vector3* pStart, br_vector3* pFinish, br_scalar pWidth);
extern void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL ProcessThisOpponent(tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL RebuildActiveCarList(void);

/* inlined copy of UnStunTheBugger (0x4a????) at this call site in the original */
static __inline void C2_HOOK_FASTCALL UnStunTheBugger_Impl(tOpponent_spec* pOpponent_spec) {
    pOpponent_spec->stun_time_ends = 0;
}

/* inlined copy of ClearTwattageOccurrenceVariables (0x4a????) at this call site in the original */
static __inline void C2_HOOK_FASTCALL ClearTwattageOccurrenceVariables_Impl(tOpponent_spec* pOpponent_spec) {
    pOpponent_spec->car_spec->big_bang = 0;
    pOpponent_spec->car_spec->scary_bang = 0;
    pOpponent_spec->car_spec->grudge_raised_recently = 0;
    pOpponent_spec->car_spec->last_person_to_hit_us = NULL;
    pOpponent_spec->car_spec->last_person_we_hit = NULL;
}

// FUNCTION: CARMA2_HW 0x004a9e10
void C2_HOOK_FASTCALL MungeOpponents(void) {
    int i;
    int un_stun_flag;

    if (gProgram_state.AI_vehicles.number_of_opponents == 0 && gNumber_of_cops_before_faffage == 0) {
        return;
    }
    if (gProgram_state.AI_vehicles.number_of_path_nodes == 0) {
        return;
    }
    if (gProgram_state.AI_vehicles.number_of_path_sections == 0) {
        return;
    }
    un_stun_flag = 0;
    gAcme_frame_count += 1;
    if (!gAcknowledged_start && gCountdown == 0) {
        gAcknowledged_start = 1;
        if (!gStart_jumped) {
            un_stun_flag = 1;
        }
    }
    if (gProgram_state.current_car.no_of_processes_recording_my_trail == 0) {
        StartRecordingTrail(&gProgram_state.current_car);
    } else {
        tPursuee_trail* trail;
        br_scalar length;
        br_vector3 car_to_last_point_v;
        br_vector3 offset_v;
        br_vector3 start1;
        br_vector3 finish1;
        br_vector3 start2;
        br_vector3 finish2;
        int visible;

        trail = &gProgram_state.current_car.my_trail;
        trail->nodes_shifted_this_frame = 0;
        if (gTime_stamp_for_this_munging > trail->time_of_next_recording) {
            trail->time_of_next_recording = gTime_stamp_for_this_munging + 500;
            if (BrVector3Dot(&gProgram_state.current_car.direction, &trail->base_heading) < FastScalarCos(30)) {
                trail->has_deviated_recently = 1;
            }
            BrVector3Sub(&car_to_last_point_v, &trail->trail_nodes[trail->number_of_nodes - 2], &gProgram_state.current_car.car_master_actor->t.t.translate.t);
            length = BrVector3Length(&car_to_last_point_v);
            if (length >= .3f) {
                CalcNegativeXVector(&offset_v, &trail->trail_nodes[trail->number_of_nodes - 2], &gProgram_state.current_car.car_master_actor->t.t.translate.t, .5f);
                BrVector3Add(&start1, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
                BrVector3Add(&finish1, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &offset_v);
                BrVector3Sub(&finish2, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &offset_v);
                BrVector3Sub(&start2, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
                visible = 1;
                if (!trail->has_deviated_recently) {
                    visible = PointVisibleFromHere(&start1, &finish1)
                        && PointVisibleFromHere(&start2, &finish2)
                        && PointVisibleFromHere(&trail->trail_nodes[trail->number_of_nodes - 2], &gProgram_state.current_car.car_master_actor->t.t.translate.t);
                }
                if ((visible && length > 2.0f) || (!visible && length > 1.5f)) {
                    if (trail->number_of_nodes < BR_ASIZE(trail->trail_nodes)) {
                        trail->number_of_nodes += 1;
                    } else {
                        memmove(&trail->trail_nodes[0], &trail->trail_nodes[1], sizeof(trail->trail_nodes) - sizeof(trail->trail_nodes[0]));
                        trail->nodes_shifted_this_frame = 1;
                    }
                    trail->has_deviated_recently = 0;
                    BrVector3Copy(&trail->base_heading, &gProgram_state.current_car.direction);
                }
                BrVector3Copy(&trail->trail_nodes[trail->number_of_nodes - 1], &gProgram_state.current_car.car_master_actor->t.t.translate.t);
            }
        }
    }
    gNum_of_opponents_pursuing = 0;
    gNum_of_opponents_getting_near = 0;
    gNum_of_opponents_completing_race = 0;
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        tOpponent_spec* oppo = &gProgram_state.AI_vehicles.opponents[i];

        if (!oppo->finished_for_this_race) {
            switch (oppo->current_objective) {
            case eOOT_pursue_and_twat:
                gNum_of_opponents_pursuing += 1;
                break;
            case eOOT_get_near_player:
                gNum_of_opponents_getting_near += 1;
                break;
            case eOOT_complete_race:
                gNum_of_opponents_completing_race += 1;
                break;
            default:
                break;
            }
        }
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        tOpponent_spec* oppo = &gProgram_state.AI_vehicles.opponents[i];

        if (!oppo->finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger_Impl(oppo);
            }
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(oppo);
            CalcPlayerConspicuousness(oppo);
            ProcessThisOpponent(oppo);
            ClearTwattageOccurrenceVariables_Impl(oppo);
        }
    }
    for (i = 0; i < gNumber_of_cops_before_faffage; i++) {
        tOpponent_spec* oppo = &gProgram_state.AI_vehicles.cops[i];

        if (!oppo->finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger_Impl(oppo);
            }
            CalcDistanceFromHome(oppo);
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(oppo);
            CalcPlayerConspicuousness(oppo);
            ProcessThisOpponent(oppo);
            ClearTwattageOccurrenceVariables_Impl(oppo);
            oppo->murder_reported = 0;
        }
    }
    if (gNext_grudge_reduction < gTime_stamp_for_this_munging) {
        gNext_grudge_reduction = gTime_stamp_for_this_munging + 5000;

        for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
            tOpponent_spec* oppo = &gProgram_state.AI_vehicles.opponents[i];

            if (!oppo->finished_for_this_race) {
                if (gOpponents[oppo->index].psyche.grudge_against_player > gGrudge_reduction_per_period) {
                    gOpponents[oppo->index].psyche.grudge_against_player -= gGrudge_reduction_per_period;
                }
            }
        }
    }
    RebuildActiveCarList();
    gFirst_frame = 0;
}

/* local declarations for DoDronePerGameFrameStuff (no new includes/externs in headers) */
extern br_model* gElasticatey_drone_path_model;
extern br_actor* gDrone_path_model_actor;
extern int gDrone_paths_elasticating;
extern int gSelected_drone_path_node_index;
extern int gSelected_edit_drone_path;
extern int gSelected_drone_path_index;
extern tU32 gNext_drone_edit_path_munge;
extern br_material* gMat_dk_grn;
extern br_material* gMat_lt_grn;
extern tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4];
extern tDrone_form_within_processing_distance_cbfn* gDrone_form_within_processing_distance_functions[4];
extern void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2);
extern void C2_HOOK_FASTCALL InitialiseEditModelsEtc(void);
extern void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void);
extern void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce);
extern void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);
extern int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone);
extern int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex);
extern int gShow_drone_paths;
extern int gCount_active_drones;
extern int gDrones_unmodified;
extern int gCount_drones;
extern int gINT_006844fc;
extern int gINT_006820d4;
extern tDrone_state_function* gDrone_state_functions[6];

/* inlined copy of UpdateEditModels (0x4512ba0) at this call site in the original */
static void C2_HOOK_FASTCALL UpdateEditModels_Impl(void) {

    if (gDrone_path_model_actor != NULL && gElasticatey_drone_path_model != NULL && gDrone_paths_elasticating && gSelected_drone_path_node_index >= 0) {
        MakeAISimpleEditSectionHere(gElasticatey_drone_path_model, 0, 0,
            &gDrone_path_nodes[gSelected_drone_path_node_index],
            &gProgram_state.current_car.car_master_actor->t.t.translate.t,
            gMat_dk_grn, gMat_lt_grn);
        BrModelUpdate(gElasticatey_drone_path_model, BR_MATU_ALL);
    }
    if (gSelected_edit_drone_path >= 0 && gTime_stamp_for_this_munging > gNext_drone_edit_path_munge) {
        gSelected_edit_drone_path = -1;
        InitialiseEditModelsEtc();
    }
    if (gSelected_drone_path_index >= 0) {
        if (gTime_stamp_for_this_munging % 1000 > 700) {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_BOUNDING_FACES;
        } else {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_FACES;
        }
    }
}

/* inlined copy of DoDroneDistanceChecks (0x4514??) at this call site in the original */
static void C2_HOOK_FASTCALL DoDroneDistanceChecks_Impl(tDrone_spec* pDrone) {

    if (gTrack_drone_min_y - 50 > pDrone->actor->t.t.translate.t.v[1]) {
        DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 5);
        if (pDrone->current_state != 5) {
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
            }
            pDrone->time_last_munge = gTime_stamp_for_this_munging;
            pDrone->prev_states[1] = pDrone->prev_states[0];
            pDrone->prev_states[0] = pDrone->current_state;
            pDrone->current_state = 5;
            if (gDrone_state_functions[5] != NULL) {
                gDrone_state_functions[5](pDrone, eDrone_state_START);
            }
        }
    } else {
        br_uint_8 process_drone;

        CalcRenderBoundsCentre();
        process_drone = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
        pDrone->field_0x45 = process_drone;
        if (!process_drone) {
            process_drone = (pDrone->form->flags & 0x4) != 0 || gDrone_form_within_processing_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
        }
        if (!process_drone) {
            StopProcessingThisDrone(pDrone, 0);
        } else if ((!pDrone->field_0x45 || gFirst_drone_processing) && !gShow_drone_paths && pDrone->field_0x44 == 0
                && ((pDrone->form->flags & 0x4) != 0 || gCount_active_drones < 12)) {
            if (CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(pDrone) && ReallyAddDroneToPHIL(pDrone)) {
                BrMatrix34Copy(&pDrone->collision_info.transform_matrix, &pDrone->actor->t.t.mat);
                pDrone->field_0x44 = 1;
                if ((pDrone->form->flags & 0x4) == 0) {
                    gCount_active_drones += 1;
                }
                DoDefaultDroneStateAction(pDrone);
                DoNotDprintf("PROCESSING ON: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
                CrappyLittleVector3DPrintf("    Pos", &pDrone->actor->t.t.translate.t);
            }
        }
        if (pDrone->field_0x45) {
            StartRenderingThisDrone(pDrone);
        } else {
            StopRenderingThisDrone(pDrone);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004512f0
void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void) {
    int i;

    UpdateEditModels_Impl();
    gINT_006844fc = 0;
    gINT_006820d4 = 0;
    if (gDrones_unmodified) {
        for (i = 0; i < gCount_drones; i++) {
            DoDroneDistanceChecks_Impl(&gDrone_specs[i]);
            if (gDrone_specs[i].current_state != 5) {
                ProcessThisDrone(i);
            }
            DoNotDprintf("FINISHED processing Drone %d, state %d", gDrone_specs[i].id, gDrone_specs[i].current_state);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* local declarations for ProcessShitMines (no new includes/externs in headers) */
extern tShit_mine gShit_mines[20];
extern tU32 gPHIL_last_physics_tick;
extern void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage);
extern void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude);
extern int C2_HOOK_FASTCALL MineExplode(tShit_mine* pMine);
extern void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);

static int C2_HOOK_FASTCALL CarVulnerableByMine_Impl(tCar_spec* pCar) {
    if (pCar->field_0x4c8 > 1.0) {
        return 0;
    }
    if (gNet_mode != eNet_mode_none) {
        if (NetPlayerFromCar(pCar) != NULL && NetPlayerFromCar(pCar)->field_0x80 != 0) {
            return 0;
        }
    }
    if (pCar->invulnerable_no_crushage) {
        return 0;
    }
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004dde20
void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime) {
    int i;
    int play_effect;

    for (i = 0; i < 20; i++) {
        tShit_mine* mine = &gShit_mines[i];

        if (!(mine->flags & 0x1)) {
            continue;
        }
        if ((mine->flags & 0x80) && mine->car != NULL) {
            tCar_spec* car = mine->car;
            int car_vulnerable;

            mine->flags &= ~0x80;
            car_vulnerable = CarVulnerableByMine_Impl(car);
            if (car_vulnerable) {
                TotallySpamTheModel(car, 0.1f * mine->max_damage);
            }
            if (car_vulnerable && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)) {
                car->collision_info->v.v[1] += FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (car->collision_info->M * 6.9);
                car->collision_info->omega.v[2] = FRandomPosNeg(mine->initial_z_omega_factor) * 6.283185307179586 / car->collision_info->M + car->collision_info->omega.v[2];
                car->collision_info->omega.v[0] = FRandomPosNeg(mine->initial_x_omega_factor) * 6.283185307179586 / car->collision_info->M + car->collision_info->omega.v[0];
                car->collision_info->disable_move_rotate = 0;
                car->collision_info->field_0x49c = gPHIL_last_physics_tick + 40;
                if (!car->invulnerable_no_damage) {
                    for (i = 0; i < 12; i++) {
                        DoDamage(car, i, IRandomBetween(0, (int)(25.f * mine->max_damage)));
                    }
                    if (car != mine->field_0x30) {
                        car->last_hit_by = mine->field_0x30;
                        car->time_last_hit = PDGetTotalTime();
                        CrashEarnings(car, NULL);
                    }
                }
            }
        }
        play_effect = 0;
        if (mine->flags & 0x20) {
            mine->flags &= ~0x20;
            play_effect = 1;
        } else {
            if (!(mine->flags & 0x2)) {
                if ((mine->flags & 0x4) && pTime > mine->next_think_time) {
                    play_effect = 1;
                }
            } else if (pTime > mine->next_think_time) {
                mine->flags &= ~0x3;
            }
        }
        if (play_effect) {
            MineExplode(mine);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on carpocalypse2 src/carma2/common/mainloop.c:132 (GPL-3.0), Timers_Push/Pop inlined
// FUNCTION: CARMA2_HW 0x00401170
void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period) {

    gTime_stamp_for_this_munging = GetTotalTime();
    if (gTime_stamp_for_this_munging == 0) {
        gTime_stamp_for_this_munging = PDGetTotalTime();
    }
    gDrone_delta_time = (float)pFrame_period / 1000.f;
    gFrame_period_for_this_munging = pFrame_period;
    TimersPush(TIMER_DRN);
    DoDronePerGameFrameStuff();
    TimersPop(TIMER_DRN);
    TimersPush(TIMER_OPP);
    MungeOpponents();
    TimersPop(TIMER_OPP);
    gFirst_drone_processing = 0;
    ProcessShitMines(gTime_stamp_for_this_munging);
}

// FUNCTION: CARMA2_HW 0x004013d0
// FUNCTION: CARMA2_SW 0x004013d0
void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401760
// FUNCTION: CARMA2_SW 0x00401760
br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex) {
    br_material* material;

    material = BrMaterialAllocate(NULL);
    material->colour = ((br_colour*)gRender_palette->pixels)[pIndex];
    material->index_base = (br_uint_8)pIndex;
    material->index_range = 1;
    material->flags = BR_MATF_TWO_SIDED;
    material->index_shade = NULL;
    material->colour_map = NULL;
    material->identifier = NULL;
    BrMaterialAdd(material);
    return material;
}

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

/* local declarations (no new includes/externs in headers) */
extern int gCount_drones;
extern int gINT_006844fc;
extern int gINT_006820d4;
extern int gShow_drone_paths;
extern int gCount_active_drones;
extern int gDrones_unmodified;
extern int gINT_006820d4;
extern int gINT_006844fc;
extern tPhysics_object* gPHIL_list_collision_infos;
extern tDrone_state_function* gDrone_state_functions[6];
extern void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone_spec);
extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pObject1, tPhysics_object* pObject2);
extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);

static int C2_HOOK_FASTCALL DroneInSensiblePlace_Impl(tDrone_spec* pDrone) {
    tPhysics_object* collision;

    if (gTime_stamp_for_this_munging > pDrone->last_collide_check + 1000) {
        pDrone->last_collide_check = gTime_stamp_for_this_munging;
        InitDroneCollisionObject(pDrone);
        for (collision = gPHIL_list_collision_infos; collision != NULL; collision = collision->next) {
            if (&pDrone->collision_info != collision && TestObjectOverlap(&pDrone->collision_info, collision)) {
                return 0;
            }
        }
        return 1;
    } else {
        return 0;
    }
}

static int C2_HOOK_FASTCALL AddDroneToPHIL_Impl(tDrone_spec* pDrone) {

    if (PHILAddObject(&pDrone->collision_info)) {
        return 0;
    }
    PHILSetObjectProperty(&pDrone->collision_info, 0, 1.0);
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 7, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 6, 1);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0044f700
void C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info) {
    int i;
    tDrone_spec* drone;
    tDrone_form* form;

    gDrones_unmodified = 1;
    gCount_rendered_drones = 0;
    gCurrent_selected_drone = 0;
    gINT_006820d4 = 0;
    gINT_006844fc = 0;
    gINT_00681fb0 = 0;

    for (i = 0; i < gCount_drones; i++) {
        drone = &gDrone_specs[i];
        form = drone->form;

        DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", drone->id, drone->current_state, 1);
        if (drone->current_state != 1) {
            if (gDrone_state_functions[drone->current_state] != NULL) {
                gDrone_state_functions[drone->current_state](drone, eDrone_state_STOP);
            }
            drone->time_last_munge = gTime_stamp_for_this_munging;
            drone->prev_states[1] = drone->prev_states[0];
            drone->prev_states[0] = drone->current_state;
            drone->current_state = 1;
            if (gDrone_state_functions[1] != NULL) {
                gDrone_state_functions[1](drone, eDrone_state_START);
            }
        }
        if (form->type == kDroneType_plane && !gShow_drone_paths && drone->field_0x44 == 0 && ((form->flags & 0x4) != 0 || gCount_active_drones < 12)) {
            if (DroneInSensiblePlace_Impl(drone) && AddDroneToPHIL_Impl(drone)) {
                BrMatrix34Copy(&drone->collision_info.transform_matrix, &drone->actor->t.t.mat);
                drone->field_0x44 = 1;
                if ((drone->form->flags & 0x4) == 0) {
                    gCount_active_drones += 1;
                }
                if (gDrone_state_functions[drone->current_state] != NULL) {
                    gDrone_state_functions[drone->current_state](drone, eDrone_state_DEFAULT);
                }
                DoNotDprintf("PROCESSING ON: Frame %d, Drone %d, state %d", gFrame, drone->id, drone->current_state);
                DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, "    Pos", drone->actor->t.t.translate.t.v[0], drone->actor->t.t.translate.t.v[1], drone->actor->t.t.translate.t.v[2]);
            }
        }
    }
}

extern int gBig_bang;
extern br_scalar gMinimum_yness_before_knackerisation;
extern void C2_HOOK_FASTCALL RebuildActiveCarList(void);
extern tU32 gNext_grudge_reduction;
extern int gFirst_frame;
extern int gGrudge_reduction_per_period;
extern tU32 gAcme_frame_count;
extern br_scalar gHead_on_cos_value;
extern int gChallenger_index__opponent;
extern int gAcknowledged_start;
extern int gStart_jumped;

static tCar_spec* GetCarSpecFromGlobalOppoIndex_Impl(int pIndex) {
    int i;

    i = 0;
    while (i < gProgram_state.AI_vehicles.number_of_opponents) {
        if (gProgram_state.AI_vehicles.opponents[i].index == pIndex) {
            return gProgram_state.AI_vehicles.opponents[i].car_spec;
        }
        i++;
    }
    return NULL;
}

static tOpponent_spec* GetOpponentSpecFromCarSpec_Impl(tCar_spec* pCar_spec) {
    int i;
    int type;

    type = (tU16)pCar_spec->car_ID & ~0xff;
    if (type == 0x200) {
        i = 0;
        while (i < gProgram_state.AI_vehicles.number_of_opponents) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
            i++;
        }
    } else if (type == 0x300) {
        i = 0;
        while (i < gNumber_of_cops_before_faffage) {
            if (gProgram_state.AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.cops[i];
            }
            i++;
        }
    }
    return NULL;
}

static int NumberOfOpponentsStillRunning_Impl(void) {
    int i;
    int car_count;
    int result;
    tCar_spec* the_car;

    result = 0;
    car_count = gProgram_state.AI_vehicles.number_of_opponents;
    for (i = 0; i < car_count; i++) {
        the_car = gProgram_state.AI_vehicles.opponents[i].car_spec;
        if (!the_car->knackered) {
            result++;
        }
    }
    return result;
}

static void StunTheBugger(tOpponent_spec* pOpponent_spec, int pMilliseconds) {
    tU32 stun_end;

    pOpponent_spec->car_spec->acc_force = 0.f;
    pOpponent_spec->car_spec->brake_force = 0.f;
    pOpponent_spec->car_spec->curvature = 0.f;
    stun_end = gTime_stamp_for_this_munging + pMilliseconds;
    if (pOpponent_spec->stun_time_ends > stun_end) {
        stun_end = pOpponent_spec->stun_time_ends;
    }
    pOpponent_spec->stun_time_ends = stun_end;
}

// FUNCTION: CARMA2_HW 0x004add50
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) {
    int i;
    int opponent_number;
    int rank_dependent_difficulty;
    br_bounds bounds;
    tCar_spec* car_spec;
    tOpponent_spec* opponent_spec;
    int opponents_left;
    int count_type_2;
    int count_type_1;

    gNext_grudge_reduction = gTime_stamp_for_this_munging + 10000;
    gFirst_frame = 1;
    gNum_viewable_cars = 1;
    gGrudge_reduction_per_period = 3 - gProgram_state.skill_level;
    gAcknowledged_start = 0;
    gStart_jumped = 0;
    gViewable_car_list[0] = &gProgram_state.current_car;
    BrActorToBounds(&bounds, gProgram_state.track_spec.the_actor);
    gMinimum_yness_before_knackerisation = bounds.min.v[1] - 2.f;
    gAcme_frame_count = 0;
    gProgram_state.current_car.no_of_processes_recording_my_trail = 0;
    gProgram_state.current_car.field_0x1540 = 0;
    gProgram_state.current_car.field_0x155c = -1;
    gHead_on_cos_value = (br_scalar)cos(0.5235668420791626);
    rank_dependent_difficulty = (int)((101.0 - 99.0 * (1.0 - (float)gCurrent_race.index / gNumber_of_races)) * -0.1);
    gBig_bang = (int)(70.f - (10 * gProgram_state.skill_level - 3 * rank_dependent_difficulty) * gOpponent_nastyness_frigger);
    if (gCamera_yon - -10.f < 45.f) {
        gIn_view_distance = 45.f;
    } else {
        gIn_view_distance = gCamera_yon - -10.f;
    }
    if (gNet_mode != eNet_mode_none) {
        gProgram_state.AI_vehicles.number_of_opponents = 0;
    } else {
        gProgram_state.AI_vehicles.number_of_opponents = pRace_info->number_of_racers - 1;
    }
    for (i = 0, opponent_number = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++, opponent_number++) {
        PossibleService();
        if (pRace_info->opponent_list[opponent_number].index < 0) {
            opponent_number++;
        }
        gProgram_state.AI_vehicles.opponents[i].car_spec = pRace_info->opponent_list[opponent_number].car_spec;
        gProgram_state.AI_vehicles.opponents[i].car_spec->car_ID = i + 0x200;
        DoNotDprintf("Car '%s', car_ID %x", gProgram_state.AI_vehicles.opponents[i].car_spec->driver_name, (tU16)gProgram_state.AI_vehicles.opponents[i].car_spec->car_ID);
        gProgram_state.AI_vehicles.opponents[i].index = pRace_info->opponent_list[opponent_number].index;
        gProgram_state.AI_vehicles.opponents[i].time_last_processed = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].next_repair_check = 0;
        gProgram_state.AI_vehicles.opponents[i].repair_interval = 60000 * IRandomBetween(gMinTimeOpponentRepair, gMaxTimeOpponentRepair);
        gProgram_state.AI_vehicles.opponents[i].last_repair_time = gTime_stamp_for_this_munging;
        DoNotDprintf("*** Opponent %s, repair_interval is %d", gProgram_state.AI_vehicles.opponents[i].car_spec->driver_name, gProgram_state.AI_vehicles.opponents[i].repair_interval);
        gProgram_state.AI_vehicles.opponents[i].time_this_objective_started = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].last_moved_ok = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].last_in_view = 0;
        gProgram_state.AI_vehicles.opponents[i].stun_time_ends = 0;
        gProgram_state.AI_vehicles.opponents[i].next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 2000;
        gProgram_state.AI_vehicles.opponents[i].next_out_of_world_check = gTime_stamp_for_this_munging + 500;
        gProgram_state.AI_vehicles.opponents[i].finished_for_this_race = 0;
        gProgram_state.AI_vehicles.opponents[i].physics_me = 1;
        gProgram_state.AI_vehicles.opponents[i].pursue_from_start = 0;
        gProgram_state.AI_vehicles.opponents[i].cheating = 0;
        gProgram_state.AI_vehicles.opponents[i].knackeredness_detected = 0;
        gProgram_state.AI_vehicles.opponents[i].players_section_when_last_calced_full_path = -1;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_person_to_hit_us = NULL;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_person_we_hit = NULL;
        gProgram_state.AI_vehicles.opponents[i].car_spec->field_0x1544 = 0;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].car_spec->grudge_raised_recently = 1;
        gProgram_state.AI_vehicles.opponents[i].car_spec->no_of_processes_recording_my_trail = 0;
        gProgram_state.AI_vehicles.opponents[i].nnext_sections = 0;
        gProgram_state.AI_vehicles.opponents[i].new_objective_required = 1;
        gProgram_state.AI_vehicles.opponents[i].current_objective = eOOT_none;
        gProgram_state.AI_vehicles.opponents[i].has_moved_at_some_point = 0;
        gProgram_state.AI_vehicles.opponents[i].player_in_view_now = 0;
        gProgram_state.AI_vehicles.opponents[i].acknowledged_piv = 0;
        gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 0;
        gProgram_state.AI_vehicles.opponents[i].nastiness = (gCurrent_race.race_spec->opponent_nastiness_level
            - ((gOpponents[gProgram_state.AI_vehicles.opponents[i].index].strength_rating - 1) * -0.25
               + gProgram_state.skill_level * -0.5)) * (1.0 / 3.0) * gOpponent_nastyness_frigger;
        BrVector3Set(&gProgram_state.AI_vehicles.opponents[i].pos_last_frame, 0.f, 0.f, 0.f);
        gOpponents[gProgram_state.AI_vehicles.opponents[i].index].psyche.grudge_against_player = 10;
        gViewable_car_list[gNum_viewable_cars] = gProgram_state.AI_vehicles.opponents[i].car_spec;
        gNum_viewable_cars++;
        StunTheBugger(&gProgram_state.AI_vehicles.opponents[i], 0xfffffff);
    }
    if (gChallenger_index__opponent >= 0) {
        car_spec = GetCarSpecFromGlobalOppoIndex_Impl(gChallenger_index__opponent);
        opponent_spec = GetOpponentSpecFromCarSpec_Impl(car_spec);
        if (opponent_spec != NULL) {
            opponent_spec->pursue_from_start = 1;
        } else {
            DoNotDprintf("ERROR - can't record dare - no opponent_spec for car_spec");
        }
        gChallenger_index__opponent = -1;
    }
    opponents_left = NumberOfOpponentsStillRunning_Impl();
    if (opponents_left != 0) {
        if (gCurrent_race.race_spec->opponent_nastiness_level >= 10.f) {
            for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
                gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 3;
            }
        } else {
            int r;
            int rstart;

            for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
                gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 0;
            }
            count_type_2 = (int)(0.6 * opponents_left);
            count_type_1 = (int)(0.2 * opponents_left);
            for (i = 0; i < count_type_2; i++) {
                r = IRandomBetween(0, opponents_left - 1 - i);
                rstart = r;
                for (;;) {
                    if (gProgram_state.AI_vehicles.opponents[r].field_0xb4 == 0) {
                        gProgram_state.AI_vehicles.opponents[r].field_0xb4 = 2;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }
            }
            for (i = 0; i < count_type_1; i++) {
                r = IRandomBetween(0, opponents_left - count_type_2 - 1 - i);
                rstart = r;
                for (;;) {
                    if (gProgram_state.AI_vehicles.opponents[r].field_0xb4 == 0) {
                        gProgram_state.AI_vehicles.opponents[r].field_0xb4 = 1;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }
            }
        }
    }
    gActive_car_list_rebuild_required = 1;
    RebuildActiveCarList();
}

/* local declarations for DisposeDronesRaceStuff (no new includes/externs in headers) */
extern int gCount_drone_forms;
extern int gCount_drone_path_nodes;
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);
extern void C2_HOOK_FASTCALL FreeShape(void* pData);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0044fc10
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;
    int j;

    if (gCount_drones != 0) {
        for (i = 0; i < gCount_drones; i++) {
            if (gDrone_specs[i].actor->parent != NULL) {
                BrActorRemove(gDrone_specs[i].actor);
            }
            BrActorFree(gDrone_specs[i].actor);
            PHILRemoveObject(&gDrone_specs[i].collision_info);
            if (gDrone_specs[i].field_0x46 != 0) {
                FreeShape(gDrone_specs[i].collision_info.shape);
                gDrone_specs[i].field_0x46 = 0;
            }
            if (gDrone_specs[i].funk_grooves != NULL) {
                BrMemFree(gDrone_specs[i].funk_grooves);
                gDrone_specs[i].funk_grooves = NULL;
            }
        }
        for (i = 0; i < gCount_drone_forms; i++) {
            if (gDrone_forms[i].field_0x80 != NULL) {
                BrMemFree(gDrone_forms[i].field_0x80);
                gDrone_forms[i].field_0x80 = NULL;
                for (j = 0; j < gDrone_forms[i].count_models; j++) {
                    BrModelRemove(gDrone_forms[i].models[j]);
                    gDrone_forms[i].models[j]->nfaces = 0;
                    gDrone_forms[i].models[j]->faces = NULL;
                    BrModelFree(gDrone_forms[i].models[j]);
                }
                if (gDrone_forms[i].models != NULL) {
                    BrMemFree(gDrone_forms[i].models);
                    gDrone_forms[i].models = NULL;
                }
            }
        }
        ClearOutStorageSpace(&gDroneStorage);
        BrMemFree(gDrone_specs);
        gDrone_specs = NULL;
        gCount_drones = 0;
        BrMemFree(gDrone_path_nodes);
        gDrone_path_nodes = NULL;
        gCount_drone_path_nodes = 0;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern float gDrone_render_zero;
extern int gCount_drone_forms;
extern int gCount_drones;
extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF);
extern void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF);

// FUNCTION: CARMA2_HW 0x0044fda0
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {
    int i;
    tDrone_spec* drone;
    int j;
    int count_models;
    tTWTVFS twt;
    tPath_name the_path;
    tPath_name path;
    char s[256];
    tDrone_form* form;
    br_model* orig_model;
    FILE* f;

    for (i = 0; i < gCount_drone_forms; i++) {
        form = &gDrone_forms[i];
        form->field_0x6c = 0;
        form->model_start = -1;
        form->model_end = 0;
        form->model_index = -1;
        form->count_models = 0;
        form->field_0x80 = NULL;
        form->models = NULL;
    }

    for (i = 0; i < gCount_drones; i++) {
        gDrone_specs[i].form->field_0x6c += 1;
        drone = &gDrone_specs[i];

        strcpy(path, gApplication_path);
        PathCat(path, path, "DRONES");
        PathCat(path, path, drone->form->name);
        twt = OpenPackFileAndSetTiffLoading(path);
        LoadDroneActorsModels(drone);

        strcpy(the_path, gApplication_path);
        PathCat(the_path, the_path, "DRONES");
        PathCat(the_path, the_path, drone->form->name);
        PathCat(the_path, the_path, drone->form->name);
        strcat(the_path, ".TXT");
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
        }

        while (PFfeof(f) == 0) {
            GetALineAndDontArgue(f, s);
            if (strcmp(s, "START OF BOUNDING SHAPES") == 0) {
                DoNotDprintf("ReadPastBoundsShapesHeader() returning TRUE");
                DoNotDprintf("DRONE.C: Reading mechanics data for drone of type %s", drone->form->name);
                ReadMechanicsShapes(&drone->collision_info.shape, f);
                if (drone->collision_info.shape != NULL) {
                    drone->field_0x46 = 1;
                }
                goto shapes_found;
            }
        }
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, drone->form->name);
    shapes_found:

        LoadFunksAndGrooves(drone, f);
        PFfclose(f);
        ClosePackFileAndSetTiffLoading(twt);
    }

    for (i = 0; i < gCount_drone_forms; i++) {
        form = &gDrone_forms[i];
        if (form->field_0x6c == 0) {
            continue;
        }
        if (form->crushability == gDrone_render_zero) {
            count_models = 0;
        } else {
            count_models = 10 < form->field_0x6c ? 10 : form->field_0x6c;
        }
        form->count_models = count_models;
        if (count_models != 0) {
            form->models = BrMemCalloc(form->count_models, sizeof(br_model*), kMem_drone_model_dup);
            form->field_0x80 = BrMemCalloc(form->count_models, sizeof(tU8), kMem_drone_model_dup);
            for (j = 0; j < form->count_models; j++) {
                orig_model = gDroneStorage.models[form->model_index];
                form->models[j] = BrModelAllocate(orig_model->identifier, orig_model->nvertices, 0);
                form->models[j]->flags |= BR_MODF_UPDATEABLE;
                form->models[j]->faces = orig_model->faces;
                form->models[j]->nfaces = orig_model->nfaces;
                memcpy(form->models[j]->vertices, orig_model->vertices, form->models[j]->nvertices * sizeof(br_vertex));
                BrModelAdd(form->models[j]);
            }
        }
    }
}

extern char gCurrent_load_name[256];
extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);
extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);
extern void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);
extern void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage* pStorage, const char* pPath);

/* inlined copy of LinkDroneActorsToModelsAndSetRenderStyle (0x4505a0) at this call site */
static void C2_HOOK_FASTCALL LinkDroneActorsToModelsAndSetRenderStyle_Impl(tDrone_spec* pDrone, int pModel_start, int pModel_end) {
    tLinkyCallback_context data;

    data.drone = pDrone;
    data.model_start = pModel_start;
    data.model_end = pModel_end;
    DRActorEnumRecurse(pDrone->model_actor, LinkyCallback, &data);
}

// FUNCTION: CARMA2_HW 0x00450150
void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone) {
    br_matrix34 mat;
    tPath_name dir_path;
    char s[256];
    tPath_name actor_path;
    int i;

    PossibleService();
    BrMatrix34Identity(&mat);
    if (pDrone->model_actor != NULL) {
        if (pDrone->model_actor->parent != NULL) {
            BrActorRemove(pDrone->model_actor);
        }
        BrActorFree(pDrone->model_actor);
        pDrone->model_actor = NULL;
    }
    if (pDrone->actor != NULL) {
        BrMatrix34Copy(&mat, &pDrone->actor->t.t.mat);
        if (pDrone->actor->parent != NULL) {
            BrActorRemove(pDrone->actor);
        }
        BrActorFree(pDrone->actor);
        pDrone->actor = NULL;
    }
    strcpy(gCurrent_load_directory, "DRONES");
    strcpy(gCurrent_load_name, pDrone->form->name);
    strcpy(dir_path, gApplication_path);
    PathCat(dir_path, dir_path, gCurrent_load_directory);
    PathCat(dir_path, dir_path, gCurrent_load_name);

    if (pDrone->form->model_start < 0) {
        int material_start;

        PrintMemoryDump(0, "BEFORE LOADING DRONE ACTORS/MODELS");
        LoadAllImagesInDirectory(&gDroneStorage, dir_path);
        LoadAllShadeTablesInDirectory(&gDroneStorage, dir_path);
        material_start = gDroneStorage.materials_count;
        LoadAllMaterialsInDirectory(&gDroneStorage, dir_path, kRendererShadingType_Specular);
        for (i = material_start; i < gDroneStorage.materials_count; i++) {
            if (gDroneStorage.materials[i] == NULL || gDroneStorage.materials[i]->colour_map == NULL) {
                continue;
            }
            gDroneStorage.materials[i]->flags |= BR_MATF_LIGHT | BR_MATF_PRELIT;
            gDroneStorage.materials[i]->flags |= BR_MATF_SMOOTH;
            BrMaterialUpdate(gDroneStorage.materials[i], BR_MATU_ALL);
            PossibleService();
        }
        pDrone->form->model_start = gDroneStorage.models_count;
        LoadAllModelsInDirectory(&gDroneStorage, dir_path);
        pDrone->form->model_end = gDroneStorage.models_count - pDrone->form->model_start;
        if (pDrone->form->crushability != gDrone_render_zero) {
            pDrone->form->model_index = -1;

            for (i = pDrone->form->model_start; i < pDrone->form->model_start + pDrone->form->model_end; i++) {
                if (DRStricmp(gDroneStorage.models[i]->identifier, pDrone->form->name) == 0) {
                    pDrone->form->model_index = i;
                    break;
                }
            }
            if (pDrone->form->model_index < 0) {
                sprintf(s, "Can't find model called '%s' for drone '%s'",
                    pDrone->form->name, pDrone->form->name);
                PDFatalError(s);
            }
        }
        PrintMemoryDump(0, "AFTER LOADING DRONE ACTORS/MODELS");
    }
    pDrone->actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrMatrix34Copy(&pDrone->actor->t.t.mat, &mat);
    sprintf(s, "Drone%d", pDrone->id);
    pDrone->actor->identifier = BrResStrDup(pDrone->actor, s);
    BrActorAdd(gNon_track_actor, pDrone->actor);
    PathCat(actor_path, dir_path, pDrone->form->name);
    strcat(actor_path, ".ACT");
    pDrone->model_actor = BrActorLoad(actor_path);
    if (pDrone->model_actor == NULL) {
        sprintf(s, "Can't load drone car actor file for %s", pDrone->form->name);
        PDFatalError(s);
    }
    LinkDroneActorsToModelsAndSetRenderStyle_Impl(pDrone, pDrone->form->model_start, pDrone->form->model_end);
    pDrone->actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(pDrone->actor, pDrone->model_actor);
    if (!(pDrone->form->flags & 0x20)) {
        br_model* model = pDrone->model_actor->model;

        pDrone->form->field_0x54.min.v[0] = model->bounds.min.v[0] + pDrone->form->crush_limits_left    * 0.01f * (model->bounds.max.v[0] - model->bounds.min.v[0]);
        pDrone->form->field_0x54.min.v[1] = model->bounds.min.v[1] + pDrone->form->crush_limits_bottom  * 0.01f * (model->bounds.max.v[1] - model->bounds.min.v[1]);
        pDrone->form->field_0x54.min.v[2] = model->bounds.min.v[2] + pDrone->form->crush_limits_back    * 0.01f * (model->bounds.max.v[2] - model->bounds.min.v[2]);
        pDrone->form->field_0x54.max.v[0] = model->bounds.max.v[0] - pDrone->form->crush_limits_right   * 0.01f * (model->bounds.max.v[0] - model->bounds.min.v[0]);
        pDrone->form->field_0x54.max.v[1] = model->bounds.max.v[1] - pDrone->form->crush_limits_top     * 0.01f * (model->bounds.max.v[1] - model->bounds.min.v[1]);
        pDrone->form->field_0x54.max.v[2] = model->bounds.max.v[2] - pDrone->form->crush_limits_front   * 0.01 * (model->bounds.max.v[2] - model->bounds.min.v[2]);
    }
}

/* callees of the functions below that live in other translation units */
extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType);
extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);
extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m);
extern void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar);
extern tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithSnart(br_actor* pActor, br_matrix34* pMat, tDRActorEnumRecurseWithSnart_cbfn* pCallback, void* pContext);
extern int C2_HOOK_FASTCALL DRActorRecurseWithPredicate(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg);
extern void C2_HOOK_FASTCALL InitialiseSpamCrush(tCar_spec* pCar, void* pContext);
extern void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, tU32 pPanel1, tU32 pPanel2, tU32 pPanel3, br_vector3* pVec, br_scalar pForce, tU32 pArg7);
extern intptr_t C2_HOOK_CDECL PredicateSpamPanel(br_actor* pActor, void* pArg);
extern intptr_t C2_HOOK_FASTCALL CrushCarActorCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern intptr_t C2_HOOK_FASTCALL CrushCarModelCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern int gCrush_panel_ids[10];

extern int gCrush_deferred;
extern int gArrow_mode;
extern br_scalar gMax_crush_force;
extern br_scalar gCrushZero;
extern int gFace_num__car;
extern tCar_damge_crush_list_item gCar_damage_crush_list[8];
extern int gCount_car_damage_crush_list;

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern const char* gGroove_funk_type_names[3];
extern const char* gDrone_axis_names[3];
extern const char* gFunk_speed_control_names[2];
extern const char* gReverseness_type_names[2];
extern int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2);
extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2);
extern int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);

/* helpers that the original inlines into LoadFunksAndGrooves (carpocalypse2 loading.c) */
static int ReadPastThisLine(FILE* pF, const char* pLine) {
    char s[256];

    for (;;) {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, pLine) == 0) {
            return 1;
        }
    }
}

static int MatchGrooveFunkType(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gGroove_funk_type_names; ppName < &gGroove_funk_type_names[3]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveAxisFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gDrone_axis_names; ppName < &gDrone_axis_names[3]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveSpeedControlFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gFunk_speed_control_names; ppName < &gFunk_speed_control_names[2]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveReversenessFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gReverseness_type_names; ppName < &gReverseness_type_names[2]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00450680
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {
    char s[256];
    tFunk_groove* funk_groove;
    tU8 funk_type;
    int axis;
    int reverseness;
    int speed_control;

    if (!ReadPastThisLine(pF, "START OF FUNKYGROOVY STUFF")) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
    }
    if (pDrone->funk_grooves == NULL) {
        pDrone->funk_grooves = BrMemAllocate(sizeof(tFunk_grooves), kMem_drone_funk_groove);
    }
    GetALineAndDontArgue(pF, s);
    while (strcmp(s, "END OF FUNKYGROOVY STUFF") != 0) {
        if (PFfeof(pF)) {
            return;
        }
        if (pDrone->funk_grooves->count >= 10) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
        }
        funk_groove = &pDrone->funk_grooves->items[pDrone->funk_grooves->count];
        funk_type = MatchGrooveFunkType(s);
        funk_groove->type = funk_type;
        switch (funk_type) {
        case eFunk_groove_type_spinny_groove:
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = (br_actor*)DRActorEnumRecurse(pDrone->model_actor, (br_actor_enum_cbfn*)MrFindy, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            GetALineAndDontArgue(pF, s);
            axis = GrooveAxisFromString(s);
            if (axis < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.axis = axis;
            GetALineAndDontArgue(pF, s);
            speed_control = GrooveSpeedControlFromString(s);
            if (speed_control < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.speed_control = speed_control;
            GetALineAndDontArgue(pF, s);
            reverseness = GrooveReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.reverse = reverseness;
            funk_groove->spinny.omega = (float)(GetAScalar(pF) * REC2_PI);
            break;

        case eFunk_groove_type_steering_groove:
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = (br_actor*)DRActorEnumRecurse(pDrone->model_actor, (br_actor_enum_cbfn*)MrFindy, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            GetALineAndDontArgue(pF, s);
            reverseness = GrooveReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->steering.reverse = reverseness;
            break;
        }
        pDrone->funk_grooves->count += 1;
        GetALineAndDontArgue(pF, s);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00691754
tU32 gNext_grudge_reduction;

// GLOBAL: CARMA2_HW 0x00691738
int gFirst_frame;

// GLOBAL: CARMA2_HW 0x00691724
int gGrudge_reduction_per_period;

// GLOBAL: CARMA2_HW 0x00691728
tU32 gAcme_frame_count;

// GLOBAL: CARMA2_HW 0x00691768
br_scalar gHead_on_cos_value;

// GLOBAL: CARMA2_HW 0x0065a3c8
int gChallenger_index__opponent = -1;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0044f980
void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone_spec) {
    br_bounds3 bnds;
    tPhysics_object* pPhysics;
    tPhysics_shape* original_shape;
    br_scalar size_x;

    pPhysics = &pDrone_spec->collision_info;
    BrActorToBounds(&bnds, pDrone_spec->actor);
    bnds.min.v[1] = bnds.min.v[1] - -0.001f;
    if (pDrone_spec->field_0x46) {
        original_shape = pPhysics->shape;
    }
    memset(pPhysics, 0, sizeof(tPhysics_object));
    if (pDrone_spec->field_0x46) {
        pPhysics->shape = original_shape;
    }
    else {
        pPhysics->shape = (tPhysics_shape*)AllocateShapeBox(kMem_drone_collision_stuff);
        pPhysics->shape->box.common.bb = bnds;
        pPhysics->shape->box.common.field_0x1c = bnds;
        pPhysics->shape->box.common.next = NULL;
        pDrone_spec->field_0x46 = 1;
    }
    pPhysics->flags_0x238 = 16;
    pPhysics->owner = pDrone_spec;
    pPhysics->actor = pDrone_spec->actor;
    pPhysics->M = pDrone_spec->form->M;
    BrVector3Set(&pPhysics->cmpos, 0.f, .09f, 0.f);
    BrVector3Set(&pPhysics->field_0x54, 0.f, -.4f / WORLD_SCALE, 0.f);
    BrMatrix34Copy(&pPhysics->transform_matrix, &pDrone_spec->actor->t.t.mat);
    pPhysics->box_face_ref = gFace_num__car - 2;
    pPhysics->field_0x1a0 = 0x100000;
    FillInShape(pPhysics->shape);
    UpdateCollisionObject(pPhysics);
    GetNewBoundingBox(&pPhysics->field_0xf4, &pPhysics->bb1, &pPhysics->transform_matrix);
    size_x = bnds.max.v[0] - bnds.min.v[0];
    pPhysics->I.v[0] = ((bnds.max.v[2] - bnds.min.v[2]) * (bnds.max.v[2] - bnds.min.v[2]) + (bnds.max.v[1] - bnds.min.v[1]) * (bnds.max.v[1] - bnds.min.v[1])) * pPhysics->M * 0.083333336f;
    pPhysics->I.v[1] = ((bnds.max.v[2] - bnds.min.v[2]) * (bnds.max.v[2] - bnds.min.v[2]) + size_x * size_x) * pPhysics->M * 0.083333336f;
    pPhysics->I.v[2] = ((bnds.max.v[1] - bnds.min.v[1]) * (bnds.max.v[1] - bnds.min.v[1]) + size_x * size_x) * pPhysics->M * 0.083333336f;
    pPhysics->cmpos.v[0] = bnds.min.v[0] - pDrone_spec->form->center.v[1] * size_x * -0.01;
    pPhysics->cmpos.v[1] = bnds.min.v[1] - pDrone_spec->form->center.v[2] * (bnds.max.v[1] - bnds.min.v[1]) * -0.01;
    pPhysics->cmpos.v[2] = bnds.min.v[2] - pDrone_spec->form->center.v[0] * (bnds.max.v[2] - bnds.min.v[2]) * -0.01;
    if (pDrone_spec->form->type == kDroneType_train) {
        pPhysics->world_friction = -.75;
    }
    pPhysics->disable_move_rotate = 1;
    pPhysics->field_0xed = 1;
    pPhysics->flags |= 0x40;
    pPhysics->drivable_on = !!(pDrone_spec->form->flags & 0x8);
    BrVector3SetFloat(&pPhysics->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pPhysics->omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pPhysics->rotate_omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pPhysics->velocity_car_space, 0.f, 0.f, 0.f);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00428d40
int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pObject1, tPhysics_object* pObject2) {
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;

    shape1 = pObject1->shape;
    shape2 = pObject2->shape;
    if (FlagsAreGo(pObject1, pObject2) && BoundsOverlapTest__finteray(&pObject1->field_0xf4, &pObject2->field_0xf4)) {
        while (shape1 != NULL) {
            while (shape2 != NULL) {
                if (TestShapeEdgeIntersection(pObject1, pObject2, shape1, shape2)) {
                    return 1;
                }
                if (TestShapeEdgeIntersection(pObject2, pObject1, shape2, shape1)) {
                    return 1;
                }
                shape2 = shape2->common.next;
            }
            shape1 = shape1->common.next;
        }
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x00681fb4
int gCount_active_drones;

// GLOBAL: CARMA2_HW 0x006820b8
int gDrones_unmodified;

// GLOBAL: CARMA2_HW 0x006820d4
int gINT_006820d4;

// GLOBAL: CARMA2_HW 0x006844fc
int gINT_006844fc;

/* stubs for original helpers that are inlined into the functions above */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00431610
void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage) {
    tCar_crush_spec* spec;
    tPhysics_object* collision_info;
    tPhysics_object* panel_actor;
    br_actor* model_actor;
    br_actor* node;
    br_actor* next_node;
    br_actor* shell;
    float damage2;
    float vdim[6];
    float vlow[3];
    float vhigh[3];
    float f1;
    float len;
    float inv_len;
    br_vector3 vec_dir;
    br_vector3 spam_vec;
    br_vector3 tmp1;
    br_vector3 tmp2;
    struct {
        tCar_spec* car;
        int* pCount;
        float rate;
    } context;
    int pCount;
    tU32 tick;
    int i;
    int j;
    int axis;
    int flip;
    int rnd;
    int found;
    int count;

    if (gArrow_mode != 0) {
        return;
    }
    if (gCrush_deferred != 0) {
        count = gCount_car_damage_crush_list;
        if (count == 8) {
            return;
        }
        for (i = 0; i < count; i++) {
            if (gCar_damage_crush_list[i].car == pCar_spec) {
                if (pDamage > gCar_damage_crush_list[i].damage) {
                    gCar_damage_crush_list[i].damage = pDamage;
                }
                return;
            }
        }
        gCar_damage_crush_list[count].car = pCar_spec;
        gCar_damage_crush_list[count].damage = pDamage;
        gCount_car_damage_crush_list = count + 1;
        return;
    }
    spec = pCar_spec->car_crush_spec;
    if (spec == NULL) {
        return;
    }
    if (pCar_spec->invulnerable_no_crushage != 0) {
        return;
    }
    if (spec->field_0x4b8 != 0) {
        return;
    }
    if (spec->field_0x144 != 0) {
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        srand(pCar_spec->car_crush_spec->field_0x574);
        pCar_spec->car_crush_spec->field_0x574 = rand();
    }
    for (i = 0; i < 10; i++) {
        gCrush_panel_ids[i] = IRandomBetween(1, pCar_spec->car_crush_spec->nb_entries);
    }

    model_actor = pCar_spec->car_model_actor;
    node = model_actor->children;
    while (node != NULL) {
        next_node = node->next;
        if (DRActorRecurseWithPredicate(node, PredicateSpamPanel, pCar_spec) != 0) {
            goto paint;
        }
        node = next_node;
    }
    shell = (br_actor*)model_actor->user;
    if (shell != NULL) {
        shell = shell->children;
        if (shell != NULL && *(void**)((tU8*)shell + 0x34) != NULL) {
            rnd = *(tU8*)((tU8*)shell + 0x20);
            found = 0;
            for (i = 0; i < 10; i++) {
                if (gCrush_panel_ids[i] == rnd) {
                    found = 1;
                    break;
                }
            }
            if (found != 0) {
                panel_actor = DetachBit(pCar_spec, model_actor, NULL);
                if (panel_actor != NULL) {
                    BrMatrix34ApplyP(&tmp1, &pCar_spec->collision_info->cmpos, (br_matrix34*)((tU8*)pCar_spec->collision_info->actor + 0x2c));
                    BrMatrix34ApplyP(&tmp2, &panel_actor->cmpos, (br_matrix34*)((tU8*)panel_actor->actor + 0x2c));
                    tmp2.v[0] = tmp2.v[0] - tmp1.v[0];
                    tmp2.v[1] = tmp2.v[1] - tmp1.v[1];
                    tmp2.v[2] = tmp2.v[2] - tmp1.v[2];
                    len = (float)sqrt(tmp2.v[0] * tmp2.v[0] + tmp2.v[1] * tmp2.v[1] + tmp2.v[2] * tmp2.v[2]);
                    if (len <= 2.3841858e-07f) {
                        tmp2.v[0] = 1.0f;
                        tmp2.v[1] = 0.0f;
                        tmp2.v[2] = 0.0f;
                    } else {
                        inv_len = (float)(1.0 / len);
                        tmp2.v[0] = inv_len * tmp2.v[0];
                        tmp2.v[1] = inv_len * tmp2.v[1];
                        tmp2.v[2] = inv_len * tmp2.v[2];
                    }
                    panel_actor->v.v[0] = tmp2.v[0] * 1.1594203f + panel_actor->v.v[0];
                    panel_actor->v.v[1] = tmp2.v[1] * 1.1594203f + panel_actor->v.v[1];
                    panel_actor->v.v[2] = tmp2.v[2] * 1.1594203f + panel_actor->v.v[2];
                    panel_actor->v.v[0] = pCar_spec->collision_info->v.v[0] + tmp2.v[0] * 1.1594203f;
                    panel_actor->v.v[1] = pCar_spec->collision_info->v.v[1] + tmp2.v[1] * 1.1594203f;
                    panel_actor->v.v[2] = pCar_spec->collision_info->v.v[2] + tmp2.v[2] * 1.1594203f;
                }
            }
        }
    }

paint:
    damage2 = pDamage * 1.5f;
    collision_info = pCar_spec->collision_info;
    pCount = 1;
    tick = gPHIL_last_physics_tick;
    vdim[0] = collision_info->bb1.min.v[0] - 0.028985508f;
    vdim[1] = collision_info->bb1.min.v[1] - 0.028985508f;
    vdim[2] = collision_info->bb1.min.v[2] - 0.028985508f;
    vdim[3] = collision_info->bb1.max.v[0] - -0.028985508f;
    vdim[4] = collision_info->bb1.max.v[1] - -0.028985508f;
    vdim[5] = collision_info->bb1.max.v[2] - -0.028985508f;
    i = 10;
    do {
        rnd = IRandomBetween(0, 5);
        axis = rnd >> 1;
        flip = rnd & 1;
        vlow[axis] = flip != 0 ? vdim[axis] : vdim[axis + 3];
        vhigh[axis] = flip != 0 ? vdim[axis + 3] : vdim[axis];
        j = (axis + 1) % 3;
        vlow[j] = SRandomBetween(vdim[j], vdim[j + 3]);
        vhigh[j] = SRandomBetween(vdim[j], vdim[j + 3]);
        j = (j + 1) % 3;
        vlow[j] = SRandomBetween(vdim[j], vdim[j + 3]);
        vhigh[j] = SRandomBetween(vdim[j], vdim[j + 3]);
        vhigh[0] = vhigh[0] - vlow[0];
        vhigh[1] = vhigh[1] - vlow[1];
        f1 = vhigh[2] - vlow[2];
        vhigh[2] = (float)sqrt(f1 * f1 + vhigh[1] * vhigh[1] + vhigh[0] * vhigh[0]);
        if (vhigh[2] <= 2.3841858e-07f) {
            vhigh[0] = 1.0f;
            vhigh[1] = 0.0f;
            vhigh[2] = 0.0f;
        } else {
            vhigh[2] = (float)(1.0 / vhigh[2]);
            vhigh[0] = vhigh[2] * vhigh[0];
            vhigh[1] = vhigh[2] * vhigh[1];
            vhigh[2] = vhigh[2] * f1;
        }
        memcpy(&vec_dir, vlow, sizeof(vec_dir));
        (void)vec_dir.v[0];
        spec = pCar_spec->car_crush_spec;
        if (spec != NULL && pCar_spec->invulnerable_no_crushage == 0 && spec->field_0x4b8 == 0) {
            context.car = pCar_spec;
            context.pCount = &pCount;
            context.rate = damage2;
            if (context.rate > gMax_crush_force) {
                context.rate = gMax_crush_force;
            }
            context.rate = context.rate / (float)pCount;
            if (spec->field_0x144 != 0) {
                ARStartPipingSession((tPipe_chunk_type)3);
                DRActorEnumRecurseWithSnart(pCar_spec->car_model_actor, NULL, CrushCarModelCbfn, &context);
                AREndPipingSession();
            } else {
                InitialiseSpamCrush(pCar_spec, &context);
                ARStartPipingSession((tPipe_chunk_type)3);
                DRActorEnumRecurseWithSnart(pCar_spec->car_master_actor, NULL, CrushCarActorCbfn, &context);
                AREndPipingSession();
                pCar_spec->use_shell_model = 0;
            }
        }
        i = i - 1;
    } while (i != 0);
    (void)tick;
    if (pCar_spec->car_crush_spec->field_0x144 != 0) {
        return;
    }
    if (PercentageCarHealth(pCar_spec) >= 0x4b) {
        return;
    }
    if (PercentageChance((int)(pCar_spec->car_crush_spec->snappability_factor * 25.0f)) != 0) {
        DeleteFarCrushes(pCar_spec);
        return;
    }
    if (pCar_spec->car_crush_spec->bendability_factor == 0.0f) {
        return;
    }
    rnd = IRandomBetween(1, 8) - 1;
    switch (rnd) {
    case 0:
        spam_vec.v[0] = 0.0f;
        spam_vec.v[1] = pCar_spec->collision_info->bb1.max.v[1];
        spam_vec.v[2] = pCar_spec->car_crush_spec->bend_z_min;
        BendCar(pCar_spec, 0xe002, 0, 0, &spam_vec, pCar_spec->collision_info->bb1.max.v[2], 0);
        return;
    case 1:
        spam_vec.v[0] = 0.0f;
        spam_vec.v[1] = pCar_spec->collision_info->bb1.min.v[1];
        spam_vec.v[2] = pCar_spec->car_crush_spec->bend_z_min;
        BendCar(pCar_spec, 0x1ffe, 0, 0, &spam_vec, pCar_spec->collision_info->bb1.max.v[2], 0);
        return;
    case 2:
        spam_vec.v[0] = pCar_spec->collision_info->bb1.min.v[0];
        spam_vec.v[1] = 0.0f;
        spam_vec.v[2] = pCar_spec->car_crush_spec->bend_z_min;
        BendCar(pCar_spec, 0, 0xe002, 0, &spam_vec, pCar_spec->collision_info->bb1.max.v[2], 0);
        return;
    case 3:
        spam_vec.v[0] = pCar_spec->collision_info->bb1.max.v[0];
        spam_vec.v[1] = 0.0f;
        spam_vec.v[2] = pCar_spec->car_crush_spec->bend_z_min;
        BendCar(pCar_spec, 0, 0x1ffe, 0, &spam_vec, pCar_spec->collision_info->bb1.max.v[2], 0);
        return;
    case 4:
    case 5:
    case 6:
    case 7:
        spam_vec.v[0] = (pCar_spec->collision_info->bb1.min.v[0] + pCar_spec->collision_info->bb1.max.v[0]) * 0.5f;
        spam_vec.v[1] = (pCar_spec->collision_info->bb1.min.v[1] + pCar_spec->collision_info->bb1.max.v[1]) * 0.5f;
        spam_vec.v[2] = pCar_spec->car_crush_spec->bend_z_max;
        BendCar(pCar_spec, 0, 0, 0x1ffe, &spam_vec, pCar_spec->collision_info->bb1.min.v[2], 0);
        return;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0043f930
void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude) {
    tDamage_unit* damage_unit;
    int old_level;
    int new_level;

    if (pCar != NULL && pCar->driver >= eDriver_net_human && pDamage_type == eDamage_driver) {
        return;
    }
    if (pCar->invulnerable_no_damage || pMagnitude < 5) {
        return;
    }
    damage_unit = &pCar->damage_units[pDamage_type];
    old_level = damage_unit->damage_level;
    new_level = pMagnitude + damage_unit->damage_level;
    damage_unit->damage_level = new_level;
    if (new_level >= 99) {
        if (pMagnitude < 10) {
            damage_unit->damage_level = old_level;
        }
        else {
            damage_unit->damage_level = 99;
        }
    }
    if (SetKnackeredFlag(pCar)) {
        return;
    }
    if (pCar == NULL || pCar->driver != eDriver_oppo) {
        if (gNet_mode == eNet_mode_none
                && ((pCar->damage_units[eDamage_engine].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99)
                    || (pCar->damage_units[eDamage_transmission].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99))) {
            QueueOilSpill(pCar);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004ddab0
int C2_HOOK_FASTCALL MineExplode(tShit_mine* pMine) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* data and helpers referenced by DoDronePerGameFrameStuff */
// GLOBAL: CARMA2_HW 0x00594750
tDrone_form_within_processing_distance_cbfn* gDrone_form_within_processing_distance_functions[4] = {
    DroneCarWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
    DroneTrainWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
};

// GLOBAL: CARMA2_HW 0x00594790
const char* gDrone_axis_names[3] = {
    "x",
    "y",
    "z",
};

// GLOBAL: CARMA2_HW 0x005947a0
const char* gFunk_speed_control_names[2] = {
    "controller",
    "dronespeed",
};

// GLOBAL: CARMA2_HW 0x005947a8
const char* gReverseness_type_names[2] = {
    "forward",
    "reverse",
};

// GLOBAL: CARMA2_HW 0x005947b0
int gSelected_drone_path_node_index = -1;

// GLOBAL: CARMA2_HW 0x005947b4
int gSelected_edit_drone_path = -1;

// GLOBAL: CARMA2_HW 0x005947b8
int gSelected_drone_path_index = -1;

// GLOBAL: CARMA2_HW 0x006820cc
tU32 gNext_drone_edit_path_munge;

// GLOBAL: CARMA2_HW 0x00684520
br_model* gElasticatey_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684528
br_actor* gDrone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684530
int gDrone_paths_elasticating;

// GLOBAL: CARMA2_HW 0x00676870
br_material* gMat_dk_grn;

// GLOBAL: CARMA2_HW 0x00676878
br_material* gMat_lt_grn;

// GLOBAL: CARMA2_HW 0x0067baa0
int gCrush_panel_ids[10];

/* stubs for original helpers that are inlined into DoDronePerGameFrameStuff */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004020e0
void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00452ba0
void C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00451bd0
void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00451710
void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00401fe0
void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pResult, br_vector3* pStart, br_vector3* pFinish, br_scalar pWidth) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004ba4f0
int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00428dc0
int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00435e10
intptr_t C2_HOOK_CDECL PredicateSpamPanel(br_actor* pActor, void* pArg) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00435dd0
int C2_HOOK_FASTCALL DRActorRecurseWithPredicate(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00432e70
void C2_HOOK_FASTCALL InitialiseSpamCrush(tCar_spec* pCar, void* pContext) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00435d40
void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00433c70
void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, tU32 pPanel1, tU32 pPanel2, tU32 pPanel3, br_vector3* pVec, br_scalar pForce, tU32 pArg7) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
