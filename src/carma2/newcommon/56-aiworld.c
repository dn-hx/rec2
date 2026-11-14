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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_CDECL BrTransformToTransform(br_transform* dest, br_transform* src);
// FUNCTION: CARMA2_HW 0x00401430
// FUNCTION: CARMA2_SW 0x00401430
void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pLook);
    BrVector3Copy(&trans.t.look_up.up, pUp);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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
extern void C2_HOOK_FASTCALL MungeShatMines(tU32 pTime);

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
extern int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void);
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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* local declarations for MungeShatMines (no new includes/externs in headers) */
extern tShit_mine gShit_mines[20];
extern tU32 gPHIL_last_physics_tick;
extern void C2_HOOK_FASTCALL TotallySpamTheCar(tCar_spec* pCar_spec, float pDamage);
extern void C2_HOOK_FASTCALL DamageUnit2(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude);
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
void C2_HOOK_FASTCALL MungeShatMines(tU32 pTime) {
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
                TotallySpamTheCar(car, 0.1f * mine->max_damage);
            }
            if (car_vulnerable && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)) {
                car->collision_info->v.v[1] += FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (car->collision_info->M * 6.9);
                car->collision_info->omega.v[2] = FRandomPosNeg(mine->initial_z_omega_factor) * 6.283185307179586 / car->collision_info->M + car->collision_info->omega.v[2];
                car->collision_info->omega.v[0] = FRandomPosNeg(mine->initial_x_omega_factor) * 6.283185307179586 / car->collision_info->M + car->collision_info->omega.v[0];
                car->collision_info->disable_move_rotate = 0;
                car->collision_info->field_0x49c = gPHIL_last_physics_tick + 40;
                if (!car->invulnerable_no_damage) {
                    for (i = 0; i < 12; i++) {
                        DamageUnit2(car, i, IRandomBetween(0, (int)(25.f * mine->max_damage)));
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

// (GPL-3.0), Timers_Push/Pop inlined
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
    MungeShatMines(gTime_stamp_for_this_munging);
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

        DoNotDprintf_opponent("NewDroneState() Drone %d, Current state %d, New state %d", drone->id, drone->current_state, 1);
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
                DoNotDprintf_opponent("PROCESSING ON: Frame %d, Drone %d, state %d", gFrame, drone->id, drone->current_state);
                DoNotDprintf_opponent("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, "    Pos", drone->actor->t.t.translate.t.v[0], drone->actor->t.t.translate.t.v[1], drone->actor->t.t.translate.t.v[2]);
            }
        }
    }
}


/* local declarations for DisposeDronesRaceStuff (no new includes/externs in headers) */
extern int gCount_drone_forms;
extern int gCount_drone_path_nodes;
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);
extern void C2_HOOK_FASTCALL FreeFormThing(void* pData);

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
                FreeFormThing(gDrone_specs[i].collision_info.shape);
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
                DoNotDprintf_opponent("ReadPastBoundsShapesHeader() returning TRUE");
                DoNotDprintf_opponent("DRONE.C: Reading mechanics data for drone of type %s", drone->form->name);
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

/* callees of the functions below that live in other translation units */
extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(int pType);
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
extern int C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg);
extern void C2_HOOK_FASTCALL CrushCollisionShapes(tCar_spec* pCar, void* pContext);
extern void C2_HOOK_FASTCALL RandomlyXZSplitCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, tU32 pPanel1, tU32 pPanel2, tU32 pPanel3, br_vector3* pVec, br_scalar pForce, tU32 pArg7);
extern intptr_t C2_HOOK_CDECL FireOffBitsCB(br_actor* pActor, void* pArg);
extern intptr_t C2_HOOK_FASTCALL CrushModels(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern intptr_t C2_HOOK_FASTCALL SimpleCrushCarModelsCB(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern int gCrush_panel_ids[10];

extern int gCrush_deferred;
extern int gArrow_mode;
extern br_scalar gMax_crush_force;
extern br_scalar gCrushZero;
extern int gFace_num__car;
extern tCar_damge_crush_list_item gCar_damage_crush_list[8];
extern int gCount_car_damage_crush_list;

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern const char* gGroove_funk_type_names[4];
extern const char* gDrone_axis_names[4];
extern const char* gFunk_speed_control_names[2];
extern const char* gReverseness_type_names[2];
extern int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2);
extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2);
extern int C2_HOOK_FASTCALL BoundsOverlapTest(br_bounds* b1, br_bounds* b2);

/* helpers that the original inlines into LoadFunksAndGrooves */
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
        pPhysics->shape = (tPhysics_shape*)AllocateBoxCollisionShape(kMem_drone_collision_stuff);
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
    if (FlagsAreGo(pObject1, pObject2) && BoundsOverlapTest(&pObject1->field_0xf4, &pObject2->field_0xf4)) {
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
// FUNCTION: CARMA2_HW 0x0043f930
void C2_HOOK_FASTCALL DamageUnit2(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude) {
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

/* local declarations for MineExplode (no new includes/externs in headers) */
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor);
extern tS3_outlet* gCar_outlet;
extern br_vector3 gZero_v__car;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004ddab0
int C2_HOOK_FASTCALL MineExplode(tShit_mine* pMine) {
    tNet_message* message;
    br_vector3 offset;
    int hierarchy_size;

    if (pMine->flags & 0x2) {
        return 0;
    }
    pMine->flags |= 0x2;
    if (gNet_mode == eNet_mode_host) {
        hierarchy_size = GetHierarchyNetworkSize(pMine->collision_info);
        message = NetBuildGuaranteedMessage(0x31, hierarchy_size + 12);
        message->contents.mine_explode.mine_index = pMine - gShit_mines;
        message->contents.mine_explode.field_0x3 = pMine->field_0x35;
        message->contents.mine_explode.field_0x4 = pMine->field_0x34;
        message->contents.mine_explode.field_0x5 = NetPlayerFromCar(pMine->field_0x30) - gNet_players;
        message->contents.mine_explode.field_0x8 = -1;
        GetHierarchyNetworkStuff(pMine->collision_info, message->contents.mine_explode.object_hierarchy, 0x48);
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    }
    PHILRemoveObject(pMine->collision_info);
    pMine->next_think_time = GetTotalTime() + 1600;
    BrVector3Negate(&offset, &gExplosion_pix_animation_groups.groups[0].offset);
    Explode(&gExplosion_pix_animation_groups, pMine->collision_info->actor, &pMine->collision_info->actor->model->bounds, &offset);
    DRS3StartSound3D(gCar_outlet, eSoundId_Explosion, &pMine->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, 0x10000, 0x10000);
    PipeSingleEndShitMine(pMine->collision_info->actor);
    if (pMine->collision_info->actor->parent != NULL) {
        BrActorRemove(pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_NONE;
    return 1;
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
const char* gDrone_axis_names[4] = {
    "x",
    "y",
    "z",
    NULL,
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
// FUNCTION: CARMA2_HW 0x004020e0
void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2) {
    br_vector3 p1;
    br_vector3 p2;

    p1 = pNode->position;
    p1.v[1] -= -0.15f;
    p2 = *pPos;
    p2.v[1] -= -0.15f;

    pModel->vertices[pVert_index].p.v[0] = p1.v[0];
    pModel->vertices[pVert_index].p.v[1] = p1.v[1];
    pModel->vertices[pVert_index].p.v[2] = p1.v[2];
    pModel->vertices[pVert_index].p.v[1] -= -0.1f;

    pModel->vertices[pVert_index + 1].p.v[0] = p2.v[0];
    pModel->vertices[pVert_index + 1].p.v[1] = p2.v[1];
    pModel->vertices[pVert_index + 1].p.v[2] = p2.v[2];
    pModel->vertices[pVert_index + 1].p.v[1] -= -0.1f;

    pModel->vertices[pVert_index + 2].p.v[0] = p1.v[0];
    pModel->vertices[pVert_index + 2].p.v[1] = p1.v[1];
    pModel->vertices[pVert_index + 2].p.v[2] = p1.v[2];
    pModel->vertices[pVert_index + 2].p.v[1] -= 0.1f;

    pModel->vertices[pVert_index + 3].p.v[0] = p2.v[0];
    pModel->vertices[pVert_index + 3].p.v[1] = p2.v[1];
    pModel->vertices[pVert_index + 3].p.v[2] = p2.v[2];
    pModel->vertices[pVert_index + 3].p.v[1] -= 0.1f;

    pModel->faces[pFace_index].vertices[0] = pVert_index + 1;
    pModel->faces[pFace_index].vertices[1] = pVert_index + 3;
    pModel->faces[pFace_index].vertices[2] = pVert_index + 2;
    pModel->faces[pFace_index].smoothing = 0xffff;
    pModel->faces[pFace_index].material = pMaterial1;

    pModel->faces[pFace_index + 1].vertices[0] = pVert_index;
    pModel->faces[pFace_index + 1].vertices[1] = pVert_index + 1;
    pModel->faces[pFace_index + 1].vertices[2] = pVert_index + 2;
    pModel->faces[pFace_index + 1].smoothing = 0xffff;
    pModel->faces[pFace_index + 1].material = pMaterial2;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* local declarations for InitialiseEditModelsEtc (no new includes/externs in headers) */
extern br_model* gStatic_drone_path_model;
extern br_actor* gStatic_drone_path_model_actor;
extern int gStatic_drone_path_model_vertex_capacity;
extern int gStatic_drone_path_model_face_capacity;
extern br_material* gMat_dk_yel;
extern br_material* gMat_md_yel;
extern br_material* gMat_lt_yel;
extern br_material* gMat_md_grn;
extern br_material* gMat_dk_turq;
extern br_material* gMat_lt_turq;
extern void C2_HOOK_FASTCALL SaveDronePaths(void);
extern void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pFaces, int pVertices);
extern void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVert_index, int pFace_index, br_vector3* pPosition, br_material* pMaterial1, br_material* pMaterial2, br_material* pMaterial3);

// FUNCTION: CARMA2_HW 0x00452ba0
int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {
    tU16* pCount;
    tS16* pNode1;
    tDrone_path_node* pPath_node;
    br_material* pMaterial1;
    br_material* pMaterial2;
    br_material* pMaterial3;
    int i;
    int vertex_count;
    int face_count;
    int vertex_index;
    int face_index;
    int node_index;
    int section_index;
    int node_offset;
    int count;

    if (!gStatic_drone_path_model_actor) {
        gStatic_drone_path_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gNon_track_actor, gStatic_drone_path_model_actor);
    }
    if (!gDrone_path_model_actor) {
        gDrone_path_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gNon_track_actor, gDrone_path_model_actor);
    }
    if (!gStatic_drone_path_model) {
        gStatic_drone_path_model = BrModelAllocate("StaticDronePathModel", 3, 1);
        gStatic_drone_path_model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_GENERATE_TAGS;
        BrModelAdd(gStatic_drone_path_model);
    }
    if (!gElasticatey_drone_path_model) {
        gElasticatey_drone_path_model = BrModelAllocate("ElasticateyDronePathModel", 3, 1);
        gElasticatey_drone_path_model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_GENERATE_TAGS;
        BrModelAdd(gElasticatey_drone_path_model);
    }
    if (!gStatic_drone_path_model_actor || !gDrone_path_model_actor || !gStatic_drone_path_model || !gElasticatey_drone_path_model) {
        return 0;
    }
    gStatic_drone_path_model_actor->model = gStatic_drone_path_model;
    gStatic_drone_path_model_actor->type = BR_ACTOR_MODEL;
    gStatic_drone_path_model_actor->render_style = gCount_drone_path_nodes != 0 ? BR_RSTYLE_FACES : BR_RSTYLE_NONE;
    gDrone_path_model_actor->model = gElasticatey_drone_path_model;
    gDrone_path_model_actor->type = BR_ACTOR_MODEL;
    gDrone_path_model_actor->render_style = gDrone_paths_elasticating != 0 ? BR_RSTYLE_FACES : BR_RSTYLE_NONE;

    ReallocModelFacesAndVertices(gElasticatey_drone_path_model, 2, 4);

    vertex_count = gCount_drone_path_nodes * 8;
    face_count = gCount_drone_path_nodes * 0xc;
    if (gCount_drone_path_nodes > 0) {
        pCount = (tU16*)((tU8*)gDrone_path_nodes + 0x12c);
        i = gCount_drone_path_nodes;
        do {
            count = *pCount;
            if (count > 0) {
                vertex_count += count * 4;
                face_count += count * 2;
            }
            pCount += 0x9a;
            i--;
        } while (i != 0);
    }
    if (vertex_count > gStatic_drone_path_model_vertex_capacity || face_count > gStatic_drone_path_model_face_capacity) {
        ReallocModelFacesAndVertices(gStatic_drone_path_model, face_count, vertex_count);
        gStatic_drone_path_model_vertex_capacity = vertex_count;
        gStatic_drone_path_model_face_capacity = face_count;
    } else {
        gStatic_drone_path_model->nvertices = (br_uint_16)vertex_count;
        gStatic_drone_path_model->nfaces = (br_uint_16)face_count;
    }

    vertex_index = 0;
    face_index = 0;
    node_index = 0;
    if (gCount_drone_path_nodes > 0) {
        node_offset = 0;
        do {
            pPath_node = (tDrone_path_node*)((tU8*)gDrone_path_nodes + node_offset);
            if (node_index == gSelected_edit_drone_path) {
                pMaterial1 = gMat_dk_turq;
                pMaterial2 = gMat_lt_turq;
                pMaterial3 = gMat_lt_turq;
            } else if (pPath_node->field_0x12e & 1) {
                pMaterial1 = gMat_dk_yel;
                pMaterial2 = gMat_md_yel;
                pMaterial3 = gMat_lt_yel;
            } else {
                pMaterial1 = gMat_dk_grn;
                pMaterial2 = gMat_md_grn;
                pMaterial3 = gMat_lt_grn;
            }
            MakeAIEditCubeHere(gStatic_drone_path_model, vertex_index, face_index, &pPath_node->position, pMaterial1, pMaterial2, pMaterial3);
            face_index += 0xc;
            vertex_index += 8;
            node_index++;
            node_offset += 0x134;
        } while (node_index < gCount_drone_path_nodes);
    }

    node_index = 0;
    if (gCount_drone_path_nodes > 0) {
        node_offset = 0;
        do {
            section_index = 0;
            pPath_node = (tDrone_path_node*)((tU8*)gDrone_path_nodes + node_offset);
            pNode1 = (tS16*)((tU8*)pPath_node + 0xc);
            while (1) {
                if (pPath_node->field_0x12e & 1) {
                    count = 1;
                } else {
                    count = pPath_node->count_sections;
                }
                if (section_index >= count) {
                    break;
                }
                if (pPath_node->field_0x12e & 1) {
                    if (*pNode1 >= 0) {
                        MakeAISimpleEditSectionHere(gStatic_drone_path_model, vertex_index, face_index, pPath_node, &gDrone_path_nodes[*pNode1].position, gMat_dk_yel, gMat_lt_yel);
                    }
                } else {
                    MakeAISimpleEditSectionHere(gStatic_drone_path_model, vertex_index, face_index, pPath_node, &gDrone_path_nodes[*pNode1].position, gMat_dk_grn, gMat_lt_grn);
                }
                face_index += 2;
                vertex_index += 4;
                section_index++;
                pNode1 += 0x12;
            }
            node_index++;
            node_offset += 0x134;
        } while (node_index < gCount_drone_path_nodes);
    }

    if (vertex_index != 0) {
        BrModelUpdate(gStatic_drone_path_model, BR_MODU_ALL);
        SaveDronePaths();
        return 1;
    }
    gStatic_drone_path_model_actor->render_style = BR_RSTYLE_NONE;
    SaveDronePaths();
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00451bd0
void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
    br_vector3 tv;

    BrVector3Negate(&tv, (br_vector3*)gCamera_to_world.m[2]);
    BrVector3Normalise(&tv, &tv);
    BrVector3Scale(&tv, &tv, 5.f);
    BrVector3Add(&gRender_bounds_centre, (br_vector3*)gCamera_to_world.m[3], &tv);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* local declarations for StopProcessingThisDrone (no new includes/externs in headers) */
extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);

/* inlined copy of CrappyLittleVector3DPrintf (0x451620) at this call site in the original */
static void C2_HOOK_CDECL DroneCrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00451710
void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {

    if (pDrone->field_0x44) {
        if (pForce || (!(pDrone->form->flags & 0x4) && pDrone->current_state != 3 && gCount_active_drones > 0)) {
            pDrone->field_0x44 = 0;
            gCount_active_drones -= 1;
            if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
                DoNotDprintf("STOP RENDERING: %d", pDrone->id);
                gCount_rendered_drones -= 1;
                pDrone->actor->render_style = BR_RSTYLE_NONE;
                PipeSingleDroneRender(pDrone, 0);
            }
            PHILRemoveObject(&pDrone->collision_info);
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, (tDroneStateFuncState)2);
            }
            DoNotDprintf("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            DroneCrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00401fe0
void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pResult, br_vector3* pStart, br_vector3* pFinish, br_scalar pWidth) {
    br_vector3 positive_y_vector;
    br_vector3 path_vector;

    BrVector3Sub(&path_vector, pFinish, pStart);
    BrVector3Set(&positive_y_vector, 0.f, 1.f, 0.f);
    BrVector3Cross(pResult, &path_vector, &positive_y_vector);
    BrVector3Normalise(pResult, pResult);
    BrVector3Scale(pResult, pResult, pWidth);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif



#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00435e10
intptr_t C2_HOOK_CDECL FireOffBitsCB(br_actor* pActor, void* pArg) {
    tCar_spec* pCar;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tPhysics_object* panel_actor;
    br_vector3 tmp1;
    br_vector3 tmp2;
    float len;
    float inv_len;
    float dx;
    float dy;
    float dz;
    int rnd;
    int i;
    int found;

    pCar = pArg;
    user = (tUser_crush_data*)pActor->user;
    if (user == NULL) {
        goto fail;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        goto fail;
    }
    if (crush_data->detach_data == NULL) {
        goto fail;
    }
    rnd = crush_data->id;
    found = 0;
    for (i = 0; i < 10; i++) {
        if (gCrush_panel_ids[i] == rnd) {
            found = 1;
            break;
        }
    }
    if (found == 0) {
        goto fail;
    }
    panel_actor = DetachBit(pCar, pActor, NULL);
    if (panel_actor == NULL) {
        goto fail;
    }
    BrMatrix34ApplyP(&tmp1, &pCar->collision_info->cmpos, (br_matrix34*)((tU8*)pCar->collision_info->actor + 0x2c));
    BrMatrix34ApplyP(&tmp2, &panel_actor->cmpos, (br_matrix34*)((tU8*)panel_actor->actor + 0x2c));
    dx = tmp2.v[0] - tmp1.v[0];
    dy = tmp2.v[1] - tmp1.v[1];
    dz = tmp2.v[2] - tmp1.v[2];
    len = (float)sqrt(dx * dx + dy * dy + dz * dz);
    if (len > 2.3841858e-07f) {
        inv_len = (float)(1.0 / len);
        dx = inv_len * dx;
        dy = inv_len * dy;
        dz = inv_len * dz;
    } else {
        dx = 1.0f;
        dy = 0.0f;
        dz = 0.0f;
    }
    panel_actor->v.v[0] = dx * 1.1594203f + panel_actor->v.v[0];
    panel_actor->v.v[1] = dy * 1.1594203f + panel_actor->v.v[1];
    panel_actor->v.v[2] = dz * 1.1594203f + panel_actor->v.v[2];
    panel_actor->v.v[0] = pCar->collision_info->v.v[0] + panel_actor->v.v[0];
    panel_actor->v.v[1] = pCar->collision_info->v.v[1] + panel_actor->v.v[1];
    panel_actor->v.v[2] = pCar->collision_info->v.v[2] + panel_actor->v.v[2];
    return 0;
fail:
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00435dd0
int C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg) {
    int result;
    br_actor* actor;
    br_actor* next;

    actor = pActor;
    for (pActor = pActor->children; pActor != NULL; pActor = next) {
        next = pActor->next;
        result = DRActorEnumRecurseUltraPlus(pActor, pCallback, pArg);
        if (result != 0) {
            return result;
        }
    }
    return pCallback(actor, pArg);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00435d40
void C2_HOOK_FASTCALL RandomlyXZSplitCar(tCar_spec* pCar) {
    extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane);
    extern void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag);
    const br_bounds3* bounds;
    br_vector3 line;
    br_vector4 plane;
    br_scalar angle;

    bounds = &pCar->collision_info->bb1;
    angle = FRandomBetween(0.0f, 6.2831855f);
    *(int*)&line.v[2] = 0;
    line.v[0] = sinf(angle);
    line.v[1] = cosf(angle);
    line.v[2] = (bounds->min.v[2] + bounds->max.v[2]) * 0.5f * line.v[1] + (bounds->min.v[0] + bounds->max.v[0]) * 0.5f * line.v[0];
    if (SplitCheck(pCar, 0, &line, 0.0f, &plane) != 0) {
        SplitCar(pCar, &plane, 0);
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

/* globals and stubs used by InitialiseEditModelsEtc */
// GLOBAL: CARMA2_HW 0x0068451c
br_model* gStatic_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684524
br_actor* gStatic_drone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684538
int gStatic_drone_path_model_vertex_capacity;

// GLOBAL: CARMA2_HW 0x0068453c
int gStatic_drone_path_model_face_capacity;

// GLOBAL: CARMA2_HW 0x00676858
br_material* gMat_dk_yel;

// GLOBAL: CARMA2_HW 0x0067685c
br_material* gMat_md_yel;

// GLOBAL: CARMA2_HW 0x00676860
br_material* gMat_lt_yel;

// GLOBAL: CARMA2_HW 0x00676874
br_material* gMat_md_grn;

// GLOBAL: CARMA2_HW 0x00676884
br_material* gMat_dk_turq;

// GLOBAL: CARMA2_HW 0x00676888
br_material* gMat_lt_turq;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* file-local declaration: the definition lives in src/brender/core/fw/resreg.c */
extern void C2_HOOK_CDECL BrBlockFill(br_uint_32* pDestination, br_uint_32 pValue, int pCount);

// FUNCTION: CARMA2_HW 0x00452f40
void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pFaces, int pVertices) {
    br_vertex* new_vertices;
    br_face* new_faces;
    int vertex_size;
    int face_size;
    int offset;
    int i;

    vertex_size = pVertices * sizeof(br_vertex);
    new_vertices = BrResAllocate(pModel, vertex_size, BR_MEMORY_VERTICES);
    BrBlockFill((br_uint_32*)new_vertices, 0, vertex_size / sizeof(br_uint_32));
    if (pModel->nvertices != 0) {
        i = 0;
        offset = 0;
        while (i < (pModel->nvertices < pVertices ? pModel->nvertices : pVertices)) {
            memcpy(&new_vertices[i], &pModel->vertices[i], sizeof(br_vertex));
            i++;
            offset += sizeof(br_vertex);
        }
        BrResRemove(pModel->vertices);
        BrResFree(pModel->vertices);
    }
    pModel->nvertices = (br_uint_16)pVertices;
    pModel->vertices = new_vertices;

    face_size = pFaces * sizeof(br_face);
    new_faces = BrResAllocate(pModel, face_size, BR_MEMORY_FACES);
    BrBlockFill((br_uint_32*)new_faces, 0, face_size / sizeof(br_uint_32));
    if (pModel->nfaces != 0) {
        i = 0;
        offset = 0;
        while (i < (pModel->nfaces < pFaces ? pModel->nfaces : pFaces)) {
            memcpy(&new_faces[i], &pModel->faces[i], sizeof(br_face));
            i++;
            offset += sizeof(br_face);
        }
        BrResRemove(pModel->faces);
        BrResFree(pModel->faces);
    }
    pModel->nfaces = (br_uint_16)pFaces;
    pModel->faces = new_faces;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00401aa0
void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVert_index, int pFace_index, br_vector3* pPosition, br_material* pMaterial1, br_material* pMaterial2, br_material* pMaterial3) {
    int vertex1;
    int vertex2;
    int vertex3;
    int vertex4;
    int vertex5;
    int vertex6;
    int vertex7;
    float fVar0;
    float fVar1;
    float fVar2;
    float* pfVar11;

    fVar0 = pPosition->v[0];
    fVar2 = pPosition->v[2];
    fVar1 = pPosition->v[1] - -0.15f;
    pModel->vertices[pVert_index].p.v[0] = fVar0;
    pModel->vertices[pVert_index].p.v[1] = fVar1;
    pModel->vertices[pVert_index].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[pVert_index].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[pVert_index].p.v[1] = pModel->vertices[pVert_index].p.v[1] - -0.1f;
    pModel->vertices[pVert_index].p.v[2] = pModel->vertices[pVert_index].p.v[2] - -0.1f;

    vertex1 = pVert_index + 1;
    pModel->vertices[vertex1].p.v[0] = fVar0;
    pModel->vertices[vertex1].p.v[1] = fVar1;
    pModel->vertices[vertex1].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex1].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[vertex1].p.v[1] = pModel->vertices[vertex1].p.v[1] - 0.1f;
    pModel->vertices[vertex1].p.v[2] = pModel->vertices[vertex1].p.v[2] - -0.1f;

    vertex2 = pVert_index + 2;
    pModel->vertices[vertex2].p.v[0] = fVar0;
    pModel->vertices[vertex2].p.v[1] = fVar1;
    pModel->vertices[vertex2].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex2].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex2].p.v[1] = pModel->vertices[vertex2].p.v[1] - 0.1f;
    pModel->vertices[vertex2].p.v[2] = pModel->vertices[vertex2].p.v[2] - -0.1f;

    vertex3 = pVert_index + 3;
    pModel->vertices[vertex3].p.v[0] = fVar0;
    pModel->vertices[vertex3].p.v[1] = fVar1;
    pModel->vertices[vertex3].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex3].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex3].p.v[1] = pModel->vertices[vertex3].p.v[1] - -0.1f;
    pModel->vertices[vertex3].p.v[2] = pModel->vertices[vertex3].p.v[2] - -0.1f;

    vertex4 = pVert_index + 4;
    pModel->vertices[vertex4].p.v[0] = fVar0;
    pModel->vertices[vertex4].p.v[1] = fVar1;
    pModel->vertices[vertex4].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex4].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[vertex4].p.v[1] = pModel->vertices[vertex4].p.v[1] - -0.1f;
    pModel->vertices[vertex4].p.v[2] = pModel->vertices[vertex4].p.v[2] - 0.1f;

    vertex5 = pVert_index + 5;
    pModel->vertices[vertex5].p.v[0] = fVar0;
    pModel->vertices[vertex5].p.v[1] = fVar1;
    pModel->vertices[vertex5].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex5].p.v[0];
    *pfVar11 = *pfVar11 - -0.1f;
    pModel->vertices[vertex5].p.v[1] = pModel->vertices[vertex5].p.v[1] - 0.1f;
    pModel->vertices[vertex5].p.v[2] = pModel->vertices[vertex5].p.v[2] - 0.1f;

    vertex6 = pVert_index + 6;
    pModel->vertices[vertex6].p.v[0] = fVar0;
    pModel->vertices[vertex6].p.v[1] = fVar1;
    pModel->vertices[vertex6].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex6].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex6].p.v[1] = pModel->vertices[vertex6].p.v[1] - 0.1f;
    pModel->vertices[vertex6].p.v[2] = pModel->vertices[vertex6].p.v[2] - 0.1f;

    vertex7 = pVert_index + 7;
    pModel->vertices[vertex7].p.v[0] = fVar0;
    pModel->vertices[vertex7].p.v[1] = fVar1;
    pModel->vertices[vertex7].p.v[2] = fVar2;
    pfVar11 = &pModel->vertices[vertex7].p.v[0];
    *pfVar11 = *pfVar11 - 0.1f;
    pModel->vertices[vertex7].p.v[1] = pModel->vertices[vertex7].p.v[1] - -0.1f;
    pModel->vertices[vertex7].p.v[2] = pModel->vertices[vertex7].p.v[2] - 0.1f;

    pModel->faces[pFace_index].vertices[0] = pVert_index + 3;
    pModel->faces[pFace_index].vertices[1] = pVert_index + 2;
    pModel->faces[pFace_index].vertices[2] = pVert_index + 1;
    pModel->faces[pFace_index].smoothing = 0xffff;
    pModel->faces[pFace_index].material = pMaterial1;

    pModel->faces[pFace_index + 1].vertices[0] = pVert_index;
    pModel->faces[pFace_index + 1].vertices[1] = pVert_index + 3;
    pModel->faces[pFace_index + 1].vertices[2] = pVert_index + 1;
    pModel->faces[pFace_index + 1].smoothing = 0xffff;
    pModel->faces[pFace_index + 1].material = pMaterial1;

    pModel->faces[pFace_index + 2].vertices[0] = pVert_index + 1;
    pModel->faces[pFace_index + 2].vertices[1] = pVert_index + 5;
    pModel->faces[pFace_index + 2].vertices[2] = pVert_index + 4;
    pModel->faces[pFace_index + 2].smoothing = 0xffff;
    pModel->faces[pFace_index + 2].material = pMaterial2;

    pModel->faces[pFace_index + 3].vertices[0] = pVert_index + 1;
    pModel->faces[pFace_index + 3].vertices[1] = pVert_index + 4;
    pModel->faces[pFace_index + 3].vertices[2] = pVert_index;
    pModel->faces[pFace_index + 3].smoothing = 0xffff;
    pModel->faces[pFace_index + 3].material = pMaterial2;

    pModel->faces[pFace_index + 4].vertices[0] = pVert_index;
    pModel->faces[pFace_index + 4].vertices[1] = pVert_index + 4;
    pModel->faces[pFace_index + 4].vertices[2] = pVert_index + 3;
    pModel->faces[pFace_index + 4].smoothing = 0xffff;
    pModel->faces[pFace_index + 4].material = pMaterial3;

    pModel->faces[pFace_index + 5].vertices[0] = pVert_index + 3;
    pModel->faces[pFace_index + 5].vertices[1] = pVert_index + 4;
    pModel->faces[pFace_index + 5].vertices[2] = pVert_index + 7;
    pModel->faces[pFace_index + 5].smoothing = 0xffff;
    pModel->faces[pFace_index + 5].material = pMaterial3;

    MakeFaceAndTextureIt(pModel, pFace_index + 6, vertex4, vertex5, vertex7, pMaterial1);
    MakeFaceAndTextureIt(pModel, pFace_index + 7, vertex5, vertex6, vertex7, pMaterial1);
    MakeFaceAndTextureIt(pModel, pFace_index + 8, vertex2, vertex7, vertex6, pMaterial2);
    MakeFaceAndTextureIt(pModel, pFace_index + 9, vertex2, vertex3, vertex7, pMaterial2);
    MakeFaceAndTextureIt(pModel, pFace_index + 10, vertex1, vertex2, vertex6, pMaterial3);
    MakeFaceAndTextureIt(pModel, pFace_index + 11, vertex1, vertex6, vertex5, pMaterial3);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
