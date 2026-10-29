#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "57-drone.h"
#include "08-loading1.h"
#include "globvars.h"
#include "41-utility.h"
#include "platform.h"
#include "30-opponent.h"
#include "52-errors.h"

#include <string.h>

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

// MungeAIWorld

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0044f700
void C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004add50
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0044fc10
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    NOT_IMPLEMENTED();
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00450150
void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00450680
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
