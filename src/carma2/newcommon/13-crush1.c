#define REC2_NO_CRUSH_DECLS

#define REC2_NO_SMASH_DECLS

/* This TU's x87 codegen is sensitive to the count of shared-header declarations
 * (DRActorEnumRecurseWithTranslation 0x42b760); the net-message types are unused here. */
#define REC2_NO_NET_MESSAGE_TYPEDEFS

#include "13-crush1.h"

#include "08-loading1.h"
#include "09-funks.h"
#include "23-piping.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "52-errors.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"
#include "globvrpb.h"
#include "04-netgame.h"
#include "globvars.h"

#include <ctype.h>

// GLOBAL: CARMA2_HW 0x00679698
br_scalar gDistortion_factor;

// GLOBAL: CARMA2_HW 0x006796b0
br_scalar gForce_to_movement_factor;

// GLOBAL: CARMA2_HW 0x00679550
br_scalar gMax_detach_time_ms;

// GLOBAL: CARMA2_HW 0x006796b8
br_scalar gMin_crush_force;

// GLOBAL: CARMA2_HW 0x0067944c
br_scalar gNormal_force_to_detach;

// GLOBAL: CARMA2_HW 0x0067a18c
br_scalar gMin_bend_angle;

// GLOBAL: CARMA2_HW 0x0067b7b0
int gMin_bend_damage;

// GLOBAL: CARMA2_HW 0x0067b7b4
br_scalar gFlap_inertia_fudge_biscuit;

// GLOBAL: CARMA2_HW 0x0067b7d0
int gMax_split_damage;

// GLOBAL: CARMA2_HW 0x0067bac8
br_scalar gMin_force_to_split_XZ_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bacc
br_scalar gMax_crush_dist_sq;

// GLOBAL: CARMA2_HW 0x0067bd5c
br_scalar gMin_force_to_split_Y_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd64
int gMin_split_damage;

/* explicit initializers keep gTrack_crush_joints and gMin_bend_force adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x0067bd68
tPhysics_joint* gTrack_crush_joints[32] = { 0 };

// GLOBAL: CARMA2_HW 0x0067bde8
br_scalar gMin_bend_force = 0.f;

// GLOBAL: CARMA2_HW 0x0067bdec
br_scalar gMax_crush_force;

// GLOBAL: CARMA2_HW 0x0067bdf4
int gMax_bend_damage;

// GLOBAL: CARMA2_HW 0x0067bdf8
br_scalar gTorque_to_snap_per_tonne;

// GLOBAL: CARMA2_HW 0x0067be00
br_scalar gChance_of_bending;

// GLOBAL: CARMA2_HW 0x0067be04
br_scalar gChance_of_inverse_buckle;

// GLOBAL: CARMA2_HW 0x0067be78
br_vector3 gBatty_gravity;

// GLOBAL: CARMA2_HW 0x0067a190
undefined4 gCrush_array_0067a190[10];

// GLOBAL: CARMA2_HW 0x00679440
undefined4 gDAT_00679440;

// GLOBAL: CARMA2_HW 0x006796a0
tCrush_info_buffer gDetached_bit_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x00679448
tPhysics_object* gDetached_bit_collision_infos;

// GLOBAL: CARMA2_HW 0x006796ac
tDriver gDetached_bit_driver;


// GLOBAL: CARMA2_HW 0x0067b7b8
tCrush_info_buffer gSplit_car_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x0067a188
tPhysics_object* gSplit_car_collision_infos;

// GLOBAL: CARMA2_HW 0x0067b7c4
tDriver gSplit_car_driver;

// GLOBAL: CARMA2_HW 0x006796b4
tPhysics_shape* gGonad_sphere_collision_shape;

// GLOBAL: CARMA2_HW 0x00589a40
br_scalar gCrushZero = 0.0f;

// GLOBAL: CARMA2_HW 0x0067bad0
int gCount_crush_detach_list;

// GLOBAL: CARMA2_HW 0x00679558
tCrush_detach_list_item gCrush_detach_list[16];

// GLOBAL: CARMA2_HW 0x0067be84
tU32 gINT_0067be84;

// FUNCTION: CARMA2_HW 0x00429bb0
void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF) {
    char s[256];
    char *result;

    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("START OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
    }
    if (GetAnInt(pF) != 4) {
        PDFatalError("Wrong version of CRUSH SETTINGS");
    }
    /* CRUSHING */
    gDistortion_factor = GetAScalar(pF);
    gMin_crush_force = GetAScalar(pF);
    gMax_crush_force = GetAScalar(pF);
    gForce_to_movement_factor = GetAScalar(pF);
    gMax_crush_dist_sq = GetAScalar(pF);

    /* SPLITTING */
    gMin_force_to_split_XZ_per_tonne = GetAScalar(pF);
    gMin_force_to_split_Y_per_tonne = GetAScalar(pF);
    gMin_split_damage = GetAnInt(pF);
    gMax_split_damage = GetAnInt(pF);

    /* BUCKLING */
    gChance_of_inverse_buckle = GetAScalar(pF);

    /* FLAPPING AND JOINT SNAPPING */
    gFlap_inertia_fudge_biscuit = GetAScalar(pF);
    gBatty_gravity.v[1] = GetAScalar(pF);
    gTorque_to_snap_per_tonne = GetAScalar(pF);

    /* DETACHING */
    gMax_detach_time_ms = GetAScalar(pF);
    gNormal_force_to_detach = GetAScalar(pF);

    /* BENDING */
    gMin_bend_force = GetAScalar(pF);
    gChance_of_bending = GetAScalar(pF);
    gMin_bend_angle = GetAScalar(pF);
    gMin_bend_damage = GetAnInt(pF);
    gMax_bend_damage = GetAnInt(pF);
    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("END OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find end of CRUSH SETTINGS in .TXT file");
    }
}

// ClearCrushLists

// RemoveCarFromCrushLists

// FUNCTION: CARMA2_HW 0x00429fa0
void C2_HOOK_FASTCALL InitCrushSystems(void) {
    int i;

    memset(gCrush_array_0067a190, 0, sizeof(gCrush_array_0067a190));
    gDAT_00679440 = 0;
    gDetached_bit_crush_info_buffer.capacity = 16;
    gDetached_bit_crush_info_buffer.crush_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gDetached_bit_collision_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gDetached_bit_crush_info_buffer.capacity; i++) {
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xdc = 1.f;
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xe0 = 0.28985506f;
        gDetached_bit_crush_info_buffer.crush_infos[i].collision_object = &gDetached_bit_collision_infos[i];
        gDetached_bit_collision_infos[i].shape = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);
    }
    gDetached_bit_driver = eDriver_detached_bit;
    for (i = 0; i < (int)REC2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i] = AllocatePhysicsJoint(3, kMem_crush_data);
    }
    gSplit_car_crush_info_buffer.capacity = 3;
    gSplit_car_crush_info_buffer.crush_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gSplit_car_collision_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gSplit_car_crush_info_buffer.capacity; i++) {
        int j;
        tPhysics_shape* shape = NULL;

        gSplit_car_crush_info_buffer.crush_infos[i].field_0xdc = 0.f;
        gSplit_car_crush_info_buffer.crush_infos[i].collision_object = &gSplit_car_collision_infos[i];
        for (j = 0; j < 4; j++) {
            tPhysics_shape* polyhedron = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);

            if (shape != NULL) {
                shape->common.next = polyhedron;
            } else {
                gSplit_car_collision_infos[i].shape = polyhedron;
            }
            shape = polyhedron;
        }
        if (shape != NULL) {
            shape->common.next = NULL;
        }
    }
    gSplit_car_driver = eDriver_split_car;
    gGonad_sphere_collision_shape = (tPhysics_shape*)AllocateShapeSphere(kMem_crush_data);
}

// ResetCrushSystems

// ReadCrushData

// FUNCTION: CARMA2_HW 0x0042a220
intptr_t C2_HOOK_CDECL CreateDefaultCrushDataCB(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    user_crush = pActor->user;
    if (user_crush == NULL || user_crush->crush_data != NULL || user_crush->groove != NULL) {
        return 0;
    }
    user_crush->crush_data = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
    strncpy(user_crush->crush_data->actor_name, pActor->identifier, sizeof(user_crush->crush_data->actor_name) - 1);
    user_crush->crush_data->id = 0xe9;
    user_crush->crush_data->softness_factor = 1.f;
    user_crush->crush_data->field_0x2c = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042a210
void C2_HOOK_FASTCALL CreateDefaultCrushData(br_actor* pActor, tCar_spec* pCar_spec) {

    DRActorEnumRecurse(pActor, CreateDefaultCrushDataCB, pCar_spec);
}

// ReadMasterCrushData

// LoadCrushDataFile

// FUNCTION: CARMA2_HW 0x0042a950
intptr_t C2_HOOK_CDECL LinkCrushDataToActor(br_actor* pActor, void* pData) {
    int i;
    tUser_crush_data* user_crush;
    tCar_crush_buffer* crush_buffer;

    crush_buffer = pData;
    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    user_crush->crush_data = NULL;
    for (i = 0; i < crush_buffer->count_entries; i++) {
        if (crush_buffer->entries[i] != NULL && DRStricmp(crush_buffer->entries[i]->actor_name, pActor->identifier) == 0) {
            user_crush->crush_data = crush_buffer->entries[i];
            return 0;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042a940
void C2_HOOK_FASTCALL LinkCrushDataToActors(br_actor* pActor, tCar_crush_buffer* pBuffer) {

    DRActorEnumRecurse(pActor, LinkCrushDataToActor, pBuffer);
}

// FUNCTION: CARMA2_HW 0x0042ac70
intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData) {
    tAccumulateSquashVertices_UserData* context = pData;
    tUser_crush_data* crush_data = pActor->user;

    if (crush_data != NULL && crush_data->crush_data != NULL) {
        int i;

        for (i = 0; i < REC2_ASIZE(crush_data->models); i++) {
            if (crush_data->models[i] != NULL) {
                context->count_vertices += crush_data->models[i]->nvertices;
            }
        }
    }
    return 0;
}

// InitModelMasterCrushData

// FUNCTION: CARMA2_HW 0x0042acb0
// FUNCTION: CARMA2_SW 0x0042ad40
int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2) {
    if (*(const float*)pValue1 < *(const float*)pValue2) {
        return -1;
    } else if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x0042ace0
int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2) {
    if (*(const float*)pValue1 > *(const float*)pValue2) {
        return -1;
    } else if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    } else {
        return 1;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0042b4c0
tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid) {
    int number;
    int i;
    int mi;
    int ma;

    *pInvalid = 1;
    number = 0;
    for (i = REC2_ASIZE(pPoint->v) - 1; i >= 0; i--) {
        for (mi = 0; mi < pCount_limits->counts[i][1] - 1; mi++) {
            if (pPoint->v[i] < pLimits->limits[i][1].values[mi]) {
                *pInvalid = 0;
                break;
            }
        }
        number = (number << 2) | mi;
        for (ma = 0; ma < pCount_limits->counts[i][0] - 1; ma++) {
            if (pLimits->limits[i][0].values[ma] < pPoint->v[i]) {
                *pInvalid = 0;
                break;
            }
        }
        number = (number << 2) | ma;
    }
    return (tU16)number;
}

// FUNCTION: CARMA2_HW 0x0042b590
intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tSoftnessOfNearestPointCB_Context* context;
    const br_vector3* point;
    br_vector3 diagonal;
    br_vector3 delta_point;
    float dist_bb_min;
    float dist_bb_max;
    float dist_bb_diagonal;
    float dist_p;
    tU16 i;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    context = pData;
    point = context->point;
    BrVector3Sub(&delta_point, point, pPoint);
    BrVector3Sub(&diagonal, &model->bounds.max, &model->bounds.min);
    dist_bb_diagonal = BrVector3LengthSquared(&diagonal);
    dist_bb_min = Vector3DistanceSquared(&model->bounds.min, &delta_point);
    dist_bb_max = Vector3DistanceSquared(&model->bounds.max, &delta_point);
    dist_p = dist_bb_min > dist_bb_max ? dist_bb_min : dist_bb_max;
    if (dist_p - dist_bb_diagonal <= context->distance) {
        for (i = 0; i < model->nvertices; i++) {
            float d;

            d = Vector3DistanceSquared(&model->vertices[i].p, &delta_point);
            if (d < context->distance) {
                context->distance = d;
                context->softness_factor = crush_data->softness_factor;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042b760
// FUNCTION: CARMA2_SW 0x0042b7f0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* pCallback, void* pContext) {
    br_vector3 delta;
    intptr_t result;
    br_actor* a;

    if (pDelta != NULL) {
        BrVector3Add(&delta, (br_vector3*)pActor->t.t.mat.m[3], pDelta);
    } else {
        BrVector3Set(&delta, 0.0f, 0.0f, 0.0f);
    }
    result = pCallback(pActor, &delta, pContext);
    if (result != 0) {
        return result;
    }
    for (a = pActor->children; a != NULL; a = a->next) {
        result = DRActorEnumRecurseWithTranslation(a, &delta, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

static float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint) {
    tSoftnessOfNearestPointCB_Context softcb_data;

    softcb_data.point = pPoint;
    softcb_data.distance = BR_SCALAR_MAX;
    softcb_data.softness_factor = 1.f;

    DRActorEnumRecurseWithTranslation(pCar_spec->car_model_actor, NULL, SoftnessOfNearestPointCB, &softcb_data);
    return softcb_data.softness_factor;
}

// based on carpocalypse2 src/carma2/common/crush.c:1204 and ApexWorm/rec2 747fd912a
// FUNCTION: CARMA2_HW 0x0042b0e0
void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec) {
    br_bounds3 wheel_all_bounds;
    const double inverse_world_scale = 1.0 / 6.9;
    int i;

    BrVector3Copy(&wheel_all_bounds.min, &pCar_spec->wpos[0]);
    BrVector3Copy(&wheel_all_bounds.max, &wheel_all_bounds.min);
    for (i = 1; i < 4; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (!(pCar_spec->wpos[i].v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = pCar_spec->wpos[i].v[j];
            } else if (pCar_spec->wpos[i].v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = pCar_spec->wpos[i].v[j];
            }
        }
    }
    wheel_all_bounds.min.v[0] = (float)(wheel_all_bounds.min.v[0] * inverse_world_scale);
    wheel_all_bounds.min.v[1] = (float)(wheel_all_bounds.min.v[1] * inverse_world_scale);
    wheel_all_bounds.min.v[2] = (float)(wheel_all_bounds.min.v[2] * inverse_world_scale);
    wheel_all_bounds.max.v[0] = (float)(wheel_all_bounds.max.v[0] * inverse_world_scale);
    wheel_all_bounds.max.v[1] = (float)(wheel_all_bounds.max.v[1] * inverse_world_scale);
    wheel_all_bounds.max.v[2] = (float)(wheel_all_bounds.max.v[2] * inverse_world_scale);
    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor* wheel_actor = pCar_spec->wheel_actors[i];
        tUser_crush_data* user_crush_data;
        br_model* wheel_model;
        br_matrix34 wheel_to_car;
        br_bounds3 wheel_bounds;
        int j;

        if (wheel_actor == NULL) {
            continue;
        }
        user_crush_data = wheel_actor->user;
        if (user_crush_data == NULL) {
            continue;
        }
        wheel_model = user_crush_data->models[0];
        if (wheel_model == NULL) {
            continue;
        }
        BrActorToActorMatrix34(&wheel_to_car, wheel_actor, pCar_spec->car_model_actor);
        BrVector3Add(&wheel_bounds.min, &wheel_model->bounds.min, (br_vector3*)wheel_to_car.m[3]);
        BrVector3Add(&wheel_bounds.max, &wheel_model->bounds.max, (br_vector3*)wheel_to_car.m[3]);
        for (j = 0; j < 3; j++) {
            if (!(wheel_bounds.min.v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = wheel_bounds.min.v[j];
            } else if (wheel_bounds.max.v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = wheel_bounds.max.v[j];
            }
        }
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < pCar_crush->count_limits.counts[i][0]; j++) {
            pCar_crush->field_0xbc.limits[i][0].values[j] = pCar_crush->limits.limits[i][0].values[j];
        }
        pCar_crush->field_0xbc.limits[i][0].values[j] = wheel_all_bounds.max.v[i];
        pCar_crush->field_0xa4.counts[i][0] = j + 1;
        for (j = 0; j < pCar_crush->count_limits.counts[i][1]; j++) {
            pCar_crush->field_0xbc.limits[i][1].values[j] = pCar_crush->limits.limits[i][1].values[j];
        }
        pCar_crush->field_0xbc.limits[i][1].values[j] = wheel_all_bounds.min.v[i];
        pCar_crush->field_0xa4.counts[i][1] = j + 1;
    }
    for (i = 0; i < 3; i++) {
        qsort(pCar_crush->field_0xbc.limits[i][0].values, pCar_crush->field_0xa4.counts[i][0], sizeof(float), DecreasingCompare);
        qsort(pCar_crush->field_0xbc.limits[i][1].values, pCar_crush->field_0xa4.counts[i][1], sizeof(float), IncreasingCompare);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        int j;
        tCar_crush_shape_info* shape_info = &pCar_crush->field_0x4[i];

        for (j = 0; j < shape_info->count_points; j++) {
            int failed;

            shape_info->field_0x18[j].field_0x28 = CrushLimitNumber(&shape_info->field_0x18[j].field_0x0, &pCar_crush->field_0xbc, &pCar_crush->field_0xa4, &failed);
            if (failed != 0) {
                shape_info->field_0x18[j].field_0x24 = 0.0f;
            } else {
                shape_info->field_0x18[j].field_0x24 = SoftnessOfNearestPoint(pCar_spec, &shape_info->field_0x18[j].field_0x0);
            }
        }
    }
}

// CopyShapePolyhedron

// InitShapeStuff

// based on carpocalypse2 src/carma2/common/crush.c:1483 (GPL-3.0); the original inlines
// InitShapeStuff, CopyShapePolyhedron, InitNetworkShapesStuff and BrVector3InvScale.
extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);
extern void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec);

// FUNCTION: CARMA2_HW 0x0042ad10
void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush;
    tPhysics_object* collision_info;
    int i;

    car_crush = pCar_spec->car_crush_spec;
    collision_info = pCar_spec->collision_info;
    if (car_crush == NULL || collision_info == NULL) {
        return;
    }
    for (i = 0; i < 3; i++) {
        car_crush->limits.limits[i][0].values[car_crush->count_limits.counts[i][0]] = collision_info->bb1.min.v[i] - 0.02898551f;
        car_crush->limits.limits[i][1].values[car_crush->count_limits.counts[i][1]] = collision_info->bb1.max.v[i] + 0.02898551f;
        car_crush->count_limits.counts[i][0] += 1;
        car_crush->count_limits.counts[i][1] += 1;
    }
    {
        tPhysics_shape* shape;
        int j;

        car_crush->count_shapes = 0;
        for (shape = collision_info->shape; shape != NULL; shape = shape->common.next) {
            car_crush->count_shapes += 1;
        }
        car_crush->field_0x4 = BrMemAllocate(car_crush->count_shapes * sizeof(tCar_crush_shape_info), kMem_crush_data);
        for (i = 0, shape = collision_info->shape; shape != NULL; shape = shape->common.next, i++) {
            tCar_crush_shape_info* crush_shape_info = &car_crush->field_0x4[i];
            tCollision_shape_polyhedron* dest;

            crush_shape_info->count_points = shape->polyhedron.polyhedron.count_points;
            crush_shape_info->count_points_add_8 = crush_shape_info->count_points + 8;
            crush_shape_info->field_0x0 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
            dest = crush_shape_info->field_0x0;
            dest->common.type = kCollisionShapeType_Polyhedron;
            dest->polyhedron.count_points = shape->polyhedron.polyhedron.count_points;
            for (j = 0; j < shape->polyhedron.polyhedron.count_points; j++) {
                BrVector3Copy(&dest->polyhedron.points[j], &shape->polyhedron.polyhedron.points[j]);
            }
            FillInShape((tPhysics_shape*)dest);
            crush_shape_info->field_0x4 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
            crush_shape_info->field_0x18 = BrMemAllocate(crush_shape_info->count_points_add_8 * sizeof(tCar_crush_reordered_shape_info), kMem_crush_data);
            for (j = 0; j < crush_shape_info->count_points; j++) {
                BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x0, &shape->polyhedron.polyhedron.points[j]);
                BrVector3Copy(&crush_shape_info->field_0x18[j].field_0xc, &crush_shape_info->field_0x18[j].field_0x0);
                BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x18, &crush_shape_info->field_0x18[j].field_0x0);
            }
            crush_shape_info->field_0x8 = 0;
        }
        car_crush->field_0xc = collision_info->shape;
        collision_info->shape = (tPhysics_shape*)car_crush->field_0x4->field_0x0;
        for (i = 0; i < car_crush->count_shapes - 1; i++) {
            car_crush->field_0x4[i].field_0x0->common.next = (tPhysics_shape*)car_crush->field_0x4[i + 1].field_0x0;
        }
        car_crush->field_0x4[car_crush->count_shapes - 1].field_0x0->common.next = NULL;
        car_crush->expand_bounding_box = 0;
        SetUpShapeLimitingStuff(car_crush, pCar_spec);
        if (gNet_mode != eNet_mode_none) {
            car_crush->field_0x24 = 0;
            car_crush->field_0x26 = 0;
            if (gNet_mode == eNet_mode_host) {
                car_crush->network_stuff = BrMemAllocate(sizeof(tCar_crush_network_shapes), kMem_crush_data);
                for (i = 1; i < gNumber_of_net_players; i++) {
                    car_crush->network_stuff->items[i - 1].network_id = gNet_players[i].ID;
                    car_crush->network_stuff->items[i - 1].field_0x4 = 0;
                }
            }
        }
    }
    for (i = 0; i < 4; i++) {
        car_crush->field_0x564[i] = AllocateShapeWireFrame(2, 1, kMem_crush_data);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[0], &pCar_spec->wpos[i], WORLD_SCALE);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[1], &pCar_spec->wpos[i], WORLD_SCALE);
        car_crush->field_0x564[i]->wireframe.points[1].v[1] = 0.f;
        car_crush->field_0x564[i]->wireframe.lines[0].index1 = 0;
        car_crush->field_0x564[i]->wireframe.lines[0].index2 = 1;

        FillInShape((tPhysics_shape*)car_crush->field_0x564[i]);
    }
    car_crush->actor->user = pCar_spec;
    car_crush->field_0x574 = 0;
    for (i = 0; ; i++) {
        int c = tolower(pCar_spec->name[i]);
        if (c == 0) {
            break;
        }
        car_crush->field_0x574 *= 2;
        car_crush->field_0x574 += c;
        car_crush->field_0x574 &= 0xffff;
    }
}

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

extern tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel);
extern void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds);

static __inline void C2_HOOK_FASTCALL CheckHingePointOrder(tCar_crush_flap_data* pHinge, br_model* pModel, br_vector3* pPos) {
    const br_vertex* vertices;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 normal;
    br_vector3 center_offset;

    vertices = pModel->vertices;
    BrVector3Sub(&hinge01, &vertices[pHinge->hinge1].p, &vertices[pHinge->hinge0].p);
    BrVector3Sub(&hinge02, &vertices[pHinge->hinge2].p, &vertices[pHinge->hinge0].p);
    BrVector3Cross(&normal, &hinge02, &hinge01);
    Vector3Average(&center_offset, &pModel->bounds.min, &pModel->bounds.max);
    BrVector3Sub(&center_offset, &center_offset, pPos);
    if (BrVector3Dot(&center_offset, &normal) > 0.0f) {
        tU16 hinge1;

        hinge1 = pHinge->hinge1;
        pHinge->hinge1 = pHinge->hinge0;
        pHinge->hinge0 = hinge1;
    }
}

static __inline tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos) {
    br_actor* parent;
    tUser_crush_data* parent_user;
    br_model* parent_model;
    br_vector3 point;

    parent_model = NULL;
    parent = pActor->parent;
    if (parent != NULL) {
        parent_user = parent->user;
        if (parent_user != NULL) {
            parent_model = parent_user->models[0];
        }
    }
    if (parent_model == NULL) {
        PDFatalError("I can't cope flapping bit doesn't have a valid parent");
    }
    BrMatrix34ApplyP(&point, pPos, &pActor->t.t.mat);
    return FindNearestVertex(&point, parent_model);
}

// FUNCTION: CARMA2_HW 0x0042b7f0
intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data) {
    tInitPhysCrushDataCB_Data* context = data;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tCar_crush_flap_data* flap_data;
    tCar_crush_detach_data* detach_data;
    br_scalar volume;

    user_crush_data = actor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    volume = (model->bounds.max.v[0] - model->bounds.min.v[0])
           * (model->bounds.max.v[1] - model->bounds.min.v[1])
           * (model->bounds.max.v[2] - model->bounds.min.v[2])
           * context->value;
    crush_data->field_0x24 = volume;
    if (!(volume >= 0.01f)) {
        crush_data->field_0x24 = 0.01f;
    }
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        br_vector3 tv;
        const br_vertex* vertices;
        tU16 parent_vertex;

        if ((flap_data->kev_o_flap & 0x2) == 0) {
            CheckHingePointOrder(flap_data, model, &context->car->collision_info->cmpos);
        }
        parent_vertex = FindNearestParentVertex(actor, &model->vertices[crush_data->flap_data->hinge0].p);
        crush_data->flap_data->field_0xc = parent_vertex;
        parent_vertex = FindNearestParentVertex(actor, &model->vertices[crush_data->flap_data->hinge2].p);
        crush_data->flap_data->field_0x12 = parent_vertex;
        vertices = model->vertices;
        BrVector3Sub(&tv, &vertices[flap_data->hinge2].p, &vertices[flap_data->hinge1].p);
        crush_data->flap_data->field_0x1c = BrVector3Length(&tv) * 0.5f;
    }
    detach_data = crush_data->detach_data;
    if (detach_data != NULL) {
        detach_data->field_0x2c = context->car;
        SetUpSemiDetachJointStuff(detach_data, model, &context->car->collision_info->bb1);
    }
    return 0;
}

// InitPhysCrushData

// CalculateReferencePoints

// FUNCTION: CARMA2_HW 0x0042be20
intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data) {
    tUser_crush_data* user_crush_data;
    tCar_crush_detach_data* detach_data;

    user_crush_data = actor->user;
    if (user_crush_data != NULL && user_crush_data->models[0] != NULL
            && user_crush_data->crush_data != NULL && (detach_data = user_crush_data->crush_data->detach_data) != NULL) {
        tUser_crush_data* parent_user_crush_data;
        br_model* model;
        br_model* parent_model;
        tU16 i;
        float d_min, d_max;
        br_vector3 min_v, max_v;

        model = user_crush_data->models[0];
        parent_user_crush_data = actor->parent->user;
        parent_model = parent_user_crush_data->models[0];
        d_max = d_min = BR_SCALAR_MAX;
        BrMatrix34ApplyP(&min_v, &model->bounds.min, &actor->t.t.mat);
        BrMatrix34ApplyP(&max_v, &model->bounds.max, &actor->t.t.mat);

        for (i = 0; i < parent_model->nvertices; i++) {
            float d;
            br_vector3 delta;

            BrVector3Sub(&delta, &min_v, &parent_model->vertices[i].p);
            d = BrVector3LengthSquared(&delta);
            if (d < d_min) {
                detach_data->min_vertex = i;
                d_min = d;
            }
            BrVector3Sub(&delta, &max_v, &parent_model->vertices[i].p);
            d = BrVector3LengthSquared(&delta);
            if (d < d_max) {
                detach_data->max_vertex = i;
                d_max = d;
            }
        }
    }
    return 0;
}

// InitModelCrushData

// based on ApexWorm/rec2 5ae3d3cb2
// FUNCTION: CARMA2_HW 0x0042c300
float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB) {
    br_vector3 pAB;
    br_vector3 pAP;
    br_vector3 pProj;
    float proj;

    BrVector3Sub(&pAP, pP, pA);
    BrVector3Sub(&pAB, pB, pA);
    proj = BrVector3Dot(&pAP, &pAB) / BrVector3Dot(&pAB, &pAB);
    if (!(proj >= gCrushZero)) {
        proj = gCrushZero;
    } else if (proj > 1.0f) {
        proj = 1.0f;
    }
    BrVector3Scale(&pProj, &pAB, proj);
    BrVector3Add(&pProj, &pProj, pA);
    BrVector3Sub(&pProj, pP, &pProj);
    return BrVector3LengthSquared(&pProj);
}

// based on carpocalypse2 src/carma2/common/crush.c:1455
// FUNCTION: CARMA2_HW 0x0042c0e0
void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data) {
    const br_vector3 *p0, *p1, *p2;
    tU16 i;
    br_scalar plane;
    br_scalar distance;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 hinge_normal;
    br_scalar hinge_dot;

    p0 = &pModel->vertices[pFlap_data->hinge0].p;
    p1 = &pModel->vertices[pFlap_data->hinge1].p;
    p2 = &pModel->vertices[pFlap_data->hinge2].p;
    BrVector3Sub(&hinge02, p2, p0);
    BrVector3Sub(&hinge01, p1, p0);
    BrVector3Cross(&hinge_normal, &hinge01, &hinge02);
    BrVector3Normalise(&hinge_normal, &hinge_normal);
    hinge_dot = BrVector3Dot(&hinge_normal, p0);

    for (i = 0; i < pModel->nvertices; i++) {
        distance = PointEdgeDistSq(&pModel->vertices[i].p, &pModel->vertices[pFlap_data->hinge0].p, &pModel->vertices[pFlap_data->hinge1].p);
        plane = BrVector3Dot(&hinge_normal, &pModel->vertices[i].p) - hinge_dot;
        if (distance > 0.00021003991f && plane > -0.0014492754f) {
            pVertex_data[i].flags |= 0x2;
        } else {
            pVertex_data[i].flags &= ~0x2;
        }
    }
}

// InitModelVertexData

extern void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data);
extern void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data);

// FUNCTION: CARMA2_HW 0x0042bf80
intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_spec* car_crush_spec;
    tCar_spec* car_spec;
    int i;

    user_crush_data = pActor->user;
    car_spec = data;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL || car_spec->car_crush_spec == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    car_crush_spec = car_spec->car_crush_spec;
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        br_model* model = user_crush_data->models[i];
        if (model != NULL && model->nvertices != 0 && model->user != NULL) {
            tModel_detail_vertex_data* vertex_data;
            tUser_detail_level_model* model_user_data;
            int v;

            model_user_data = model->user;

            model_user_data->field_0x4 = BrMemAllocate(model->nvertices * sizeof(tModel_detail_vertex_data), kMem_crush_data);
            vertex_data = model_user_data->field_0x4;
            for (v = 0; v < model->nvertices; v++) {
                int invalid;

                CompressVector3(&vertex_data[v].p, &model->vertices[v].p, -10.0f, 10.0f);
                vertex_data[v].limit_number = CrushLimitNumber(&model->vertices[v].p, &car_crush_spec->limits, &car_crush_spec->count_limits, &invalid);
            }
            if (i == 0 && crush_data->flap_data != NULL) {
                SetFlapCheckVertices(crush_data->flap_data, model, vertex_data);
            }
            if (i == 0) {
                LinkSmashies(pActor, crush_data, model_user_data->field_0x4);
            }
        }
    }
    return 0;
}

// InitVertexData

// CheckWheelPositions

// based on carpocalypse2 src/carma2/common/crush.c:1576 (GPL-3.0); the original inlines CheckWheelPositions,
// InitModelMasterCrushData, InitModelCrushData, InitPhysCrushData and InitVertexData.
extern void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

// FUNCTION: CARMA2_HW 0x0042aa20
void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec) {
    int wheel_i;
    float* wpos_y;
    tPhysics_shape* shape;
    tCar_crush_spec* car_crush;
    int i;

    wpos_y = &pCar_spec->wpos[0].v[1];
    for (wheel_i = 4; wheel_i != 0; wheel_i--) {
        int wheel_not_found = 1;

        for (shape = pCar_spec->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron) {
                int plane_i;
                int inside = 1;
                float local[3];

                local[0] = wpos_y[-1] * (1.0 / 6.9);
                local[1] = wpos_y[0] * (1.0 / 6.9);
                local[2] = wpos_y[1] * (1.0 / 6.9);
                for (plane_i = 0; plane_i < shape->polyhedron.polyhedron.count_planes; plane_i++) {
                    const br_vector4* plane = &shape->polyhedron.polyhedron.planes[plane_i];

                    if (plane->v[2] * local[2] + plane->v[1] * local[1] + plane->v[0] * local[0] + plane->v[3] < 0.f) {
                        inside = 0;
                        break;
                    }
                }
                if (inside) {
                    wheel_not_found = 0;
                    break;
                }
            }
        }
        if (wheel_not_found) {
            PDFatalError("Wheels outside all shapes");
        }
        wpos_y += 3;
    }

    if (pCar_spec->car_crush_spec != NULL) {
        SwitchCarModels(pCar_spec, 0);

        car_crush = pCar_spec->car_crush_spec;
        if (car_crush != NULL) {
            tAccumulateSquashVertices_UserData data;

            data.count_vertices = 0;
            data.bens_z_min = car_crush->bend_z_min;
            data.bens_z_max = car_crush->bend_z_max;
            DRActorEnumRecurse(pCar_spec->car_model_actor, AccumulateSquashVertices, &data);
            car_crush->field_0x1cc = BrMemAllocate(data.count_vertices * sizeof(tCar_crush_vertex_data), kMem_crush_data);
            BrMatrix34Copy(&car_crush->model_actor->t.t.mat, &pCar_spec->car_model_actor->t.t.mat);
        }

        DRActorEnumRecurse(pCar_spec->car_model_actor, InitModelCrushDataCB, pCar_spec);
        InitPhysMasterCrushData(pCar_spec);

        {
            tInitPhysCrushDataCB_Data data;

            data.car = pCar_spec;
            data.value = pCar_spec->collision_info->M /
                ((pCar_spec->collision_info->bb2.max.v[0] - pCar_spec->collision_info->bb2.min.v[0]) *
                 (pCar_spec->collision_info->bb2.max.v[1] - pCar_spec->collision_info->bb2.min.v[1]) *
                 (pCar_spec->collision_info->bb2.max.v[2] - pCar_spec->collision_info->bb2.min.v[2]));
            DRActorEnumRecurse(pCar_spec->car_model_actor, InitPhysCrushDataCB, &data);
        }

        if (pCar_spec->car_crush_spec != NULL) {
            car_crush = pCar_spec->car_crush_spec;
            for (i = 0; i < 3; i++) {
                qsort(car_crush->limits.limits[i][0].values, car_crush->count_limits.counts[i][0], sizeof(float), DecreasingCompare);
                qsort(car_crush->limits.limits[i][1].values, car_crush->count_limits.counts[i][1], sizeof(float), IncreasingCompare);
            }
            DRActorEnumRecurse(pCar_spec->car_model_actor, InitVertexDataCB, pCar_spec);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042c420
void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar) {
    int i;

    if (pCar_crush != NULL) {
        BrActorFree(pCar_crush->actor);
        if (pCar_crush->field_0x1cc != NULL) {
            BrMemFree(pCar_crush->field_0x1cc);
        }
        for (i = 0; i < pCar_crush->count_shapes; i++) {
            BrMemFree(pCar_crush->field_0x4[i].field_0x18);
            BrMemFree(pCar_crush->field_0x4[i].field_0x0);
            BrMemFree(pCar_crush->field_0x4[i].field_0x4);
        }
        BrMemFree(pCar_crush->field_0x4);
        pCar->collision_info->shape = pCar_crush->field_0xc;
        if (pCar_crush->network_stuff != NULL) {
            BrMemFree(pCar_crush->network_stuff);
        }
        for (i = 0; i < (int)REC2_ASIZE(pCar_crush->field_0x564); i++) {
            BrMemFree(pCar_crush->field_0x564[i]);
        }
        BrMemFree(pCar_crush);
    }
}

// FUNCTION: CARMA2_HW 0x0042c4f0
void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data) {

    if (pCrush_data != NULL) {
        if (pCrush_data->flap_data != NULL) {
            BrMemFree(pCrush_data->flap_data);
        }
        if (pCrush_data->detach_data != NULL) {
            BrMemFree(pCrush_data->detach_data);
        }
        DisposeSmashData(pCrush_data);
        BrMemFree(pCrush_data);
    }
}

// FUNCTION: CARMA2_HW 0x0042c530
void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data) {

    if (pVertex_data != NULL) {
        BrMemFree(pVertex_data);
    }
}

// BuckleTheBugger

// OutsideEdgeLine

// PointTriangleDistSq

// FUNCTION: CARMA2_HW 0x0042ce00
float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    // based on ApexWorm/rec2 08141eedd
    br_vector3 u;
    br_vector3 v;
    br_vector3 offset;
    br_vector3 projected;
    br_vector3 residual;
    float u_scale;
    float v_scale;

    BrVector3Sub(&u, pC, pA);
    BrVector3Sub(&v, pB, pA);
    if ((float)fabs(BrVector3Dot(&u, &v)) > 0.001f) {
        BrVector3Sub(&u, pC, pB);
        BrVector3Sub(&v, pA, pB);
        pA = pB;
        if ((float)fabs(BrVector3Dot(&u, &v)) > 0.001f) {
            BrVector3Sub(&u, pB, pC);
            BrVector3Sub(&v, pA, pC);
            pA = pC;
        }
    }
    BrVector3Sub(&offset, pP, pA);
    v_scale = BrVector3Dot(&offset, &v) / BrVector3LengthSquared(&v);
    u_scale = BrVector3Dot(&offset, &u) / BrVector3LengthSquared(&u);
    if (!(v_scale >= 0.0f)) {
        v_scale = 0.0f;
    } else if (v_scale > 1.0f) {
        v_scale = 1.0f;
    }
    if (!(u_scale >= 0.0f)) {
        u_scale = 0.0f;
    } else if (u_scale > 1.0f) {
        u_scale = 1.0f;
    }
    projected.v[0] = u_scale * u.v[0] + v_scale * v.v[0] + pA->v[0];
    projected.v[1] = u_scale * u.v[1] + v_scale * v.v[1] + pA->v[1];
    projected.v[2] = u_scale * u.v[2] + v_scale * v.v[2] + pA->v[2];
    BrVector3Sub(&residual, &projected, pP);
    return BrVector3LengthSquared(&residual);
}

// PointSimplexDistSq

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0042ca30
float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    NOT_IMPLEMENTED();
}

// based on ApexWorm/rec2 dfac42acd
// FUNCTION: CARMA2_HW 0x0042c8b0
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    float closest_distance;
    int i;
    int j;
    int k;

    switch (pCount) {
    case 0:
        return gCrushZero;
    case 1:
        return Vector3DistanceSquared(pP, &pVertices[0]);
    case 2:
        return PointEdgeDistSq(pP, &pVertices[0], &pVertices[1]);
    case 3:
        return PointTriangleDistSq(pP, &pVertices[0], &pVertices[1], &pVertices[2]);
    case 4:
        return PointRectDistSq(pP, &pVertices[0], &pVertices[1], &pVertices[2]);
    }
    closest_distance = BR_SCALAR_MAX;
    for (i = 0; i < pCount; i++) {
        for (j = i + 1; j < pCount; j++) {
            for (k = j + 1; k < pCount; k++) {
                float distance;

                distance = PointTriangleDistSq(pP, &pVertices[i], &pVertices[j], &pVertices[k]);
                closest_distance = MIN(distance, closest_distance);
            }
        }
    }
    return closest_distance;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// LimitImpactScale

// GetImpactScale

// FUNCTION: CARMA2_HW 0x0042d0e0
int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq) {
    br_vector3 diagonal;
    float threshold;

    BrVector3Sub(&diagonal, &pModel->bounds.max, &pModel->bounds.min);
    threshold = BrVector3LengthSquared(&diagonal) + pMax_dist_sq;
    if (DistSq(&pModel->bounds.min, pCount, pVertices) > threshold
        || DistSq(&pModel->bounds.max, pCount, pVertices) > threshold) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004367a0
void C2_HOOK_FASTCALL CrushShapeMorphVertex(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, tCar_crush_limits* pLimits, int pFound) {
    int index;
    float t;

    index = ((pFlags & 0xffff) >> ((pAxis * 2 + (pDirectionComponent > gCrushZero)) * 2)) & 3;
    if (index > pFound) {
        *pOut = 0.0f;
        return;
    }
    t = (pLimits->limits[pAxis][pDirectionComponent > gCrushZero].values[index] - pVertexComponent) / pDirectionComponent;
    if (t < gCrushZero) {
        *pOut = 0.0f;
    } else if (t < *pOut) {
        *pOut = t;
    }
}

// FUNCTION: CARMA2_HW 0x00436850
float C2_HOOK_FASTCALL CrushShapeBendFactor(br_vector3* pVertexPos, int pCount, br_vector3* pVectors, br_scalar pMaxDistSq) {
    float dist_sq;

    dist_sq = DistSq(pVertexPos, pCount, pVectors);
    if (dist_sq > gMax_crush_dist_sq) {
        return gCrushZero;
    }
    return 1.0f - (float)sqrt(dist_sq / pMaxDistSq);
}

// FUNCTION: CARMA2_HW 0x00436890
int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector) {

    if ((float)fabs(pVector->v[0]) > 0.707107f) {
        return 0;
    } else if ((float)fabs(pVector->v[1]) > 0.707107f) {
        return 1;
    } else {
        return 2;
    }
}

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

// ClipPointToLimits

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

// FUNCTION: CARMA2_HW 0x004368d0
void C2_HOOK_FASTCALL CrushShapeFinishVertex(br_vector3* pVertexPos, tU16 pFlags, tCar_crush_limits* pLimits) {
    int i;

    for (i = 0; i < 3; i++) {
        if (pVertexPos->v[i] < pLimits->limits[i][0].values[pFlags & 3]) {
            pVertexPos->v[i] = pLimits->limits[i][0].values[pFlags & 3];
            pFlags >>= 4;
        } else {
            pFlags >>= 2;
            if (pVertexPos->v[i] > pLimits->limits[i][1].values[pFlags & 3]) {
                pVertexPos->v[i] = pLimits->limits[i][1].values[pFlags & 3];
            }
            pFlags >>= 2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00436940
void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc) {
    int i;

    pDest->count = (tU8)pSrc->count;
    for (i = 0; i < pSrc->count; i++) {
        CompressVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
    }
    pDest->field_0x1a = DRScalarToU16(pSrc->field_0x40, 0.0f, 10.0f);
    CompressVector3(&pDest->field_0x1c, &pSrc->field_0x44, -1.0f, 1.0f);
    pDest->field_0x24 = pSrc->field_0x50;
}

// FUNCTION: CARMA2_HW 0x004369c0
void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc) {
    tU8 i;

    pDest->count = pSrc->count;
    ExpandVector3(&pDest->vectors[0], &pSrc->vectors[0], -2.5f, 2.5f);
    BrVector3Copy(&pDest->field_0x34, &pDest->vectors[0]);
    for (i = 1; i < pSrc->count; i++) {
        ExpandVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
        BrVector3Accumulate(&pDest->field_0x34, &pDest->vectors[i]);
    }
    BrVector3Set(&pDest->field_0x34, pDest->field_0x34.v[0] / pDest->count, pDest->field_0x34.v[1] / pDest->count, pDest->field_0x34.v[2] / pDest->count);
    pDest->field_0x40 = DRU16ToScalar(pSrc->field_0x1a, 0.f, 10.f);
    ExpandVector3(&pDest->field_0x44, &pSrc->field_0x1c, -1.f, 1.f);
    pDest->field_0x50 = pSrc->field_0x24;
}
// RemoveDuplicatePoints

// SimpleCrushModel

// CrushDrone

// CrushDroneObject

// DoDroneCrushing

// based on ApexWorm/rec2 773b90994 (reference only)
// FUNCTION: CARMA2_HW 0x0042d390
// FUNCTION: CARMA2_SW 0x0042d400
void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints) {
    int i;

    if (pCount > 4) {
        pCount = 4;
    }
    BrVector3Copy(&pCrush->field_0x38, pVector);
    pCrush->field_0x34 = pScale;
    pCrush->count = pCount;
    for (i = 0; i < pCrush->count; i++) {
        BrVector3Copy(&((br_vector3*)pCrush->field_0x4)[i], &pPoints[i]);
    }
}

extern int gCount_queued_drone_crushes;
extern tQueued_drone_crush gQueued_drone_crushes[4];

// FUNCTION: CARMA2_HW 0x0042d1e0
void C2_HOOK_FASTCALL QueueDroneCrush(tPhysics_object* pObject, int pChain_count, br_vector3* pChain, br_vector3* pForce) {
    br_vector3 normal;
    float length;
    float dot;
    int i;
    int j;

    length = (float)sqrt(pForce->v[0] * pForce->v[0] + pForce->v[2] * pForce->v[2] + pForce->v[1] * pForce->v[1]);
    normal.v[0] = pForce->v[0] / length;
    normal.v[1] = pForce->v[1] / length;
    normal.v[2] = pForce->v[2] / length;
    for (i = 0; i < gCount_queued_drone_crushes; i++) {
        if (gQueued_drone_crushes[i].field_0x0 != (undefined4*)pObject) {
            continue;
        }
        for (j = 0; j < gQueued_drone_crushes[i].count; j++) {
            dot = gQueued_drone_crushes[i].items[j].field_0x38.v[0] * normal.v[0]
                + gQueued_drone_crushes[i].items[j].field_0x38.v[1] * normal.v[1]
                + gQueued_drone_crushes[i].items[j].field_0x38.v[2] * normal.v[2];
            if (dot > 0.707107f) {
                gQueued_drone_crushes[i].items[j].field_0x34 += length;
                return;
            }
        }
        if (gQueued_drone_crushes[i].count < 2) {
            AddDroneCrush(&gQueued_drone_crushes[i].items[gQueued_drone_crushes[i].count], &normal, length, pChain_count, pChain);
            gQueued_drone_crushes[i].count++;
        }
    }
    if (gCount_queued_drone_crushes < 4) {
        gQueued_drone_crushes[gCount_queued_drone_crushes].field_0x0 = (undefined4*)pObject;
        gQueued_drone_crushes[gCount_queued_drone_crushes].count = 0;
        AddDroneCrush(&gQueued_drone_crushes[gCount_queued_drone_crushes].items[0], &normal, length, pChain_count, pChain);
        gCount_queued_drone_crushes++;
    }
}

// DoCrushing

// QueueCarCrush

// FUNCTION: CARMA2_HW 0x0042d7e0
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {
    int i;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_spec* car_crush_spec;

    if (gCount_crush_detach_list == REC2_ASIZE(gCrush_detach_list)) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        if (pActor == NULL) {
            return;
        }
        user = pActor->user;
        if (user == NULL) {
            return;
        }
        crush_data = user->crush_data;
        if (crush_data == NULL) {
            return;
        }
        detach_data = crush_data->detach_data;
        if (detach_data == NULL) {
            return;
        }
        for (i = 0; i < gCount_crush_detach_list; i++) {
            if (gCrush_detach_list[i].actor == pActor) {
                return;
            }
        }
        car_crush_spec = pCar->car_crush_spec;
        for (i = 0; i < car_crush_spec->field_0x270; i++) {
            if (car_crush_spec->field_0x274[i].field_0x0 == pActor) {
                return;
            }
        }
        gCrush_detach_list[gCount_crush_detach_list].actor = pActor;
        gCrush_detach_list[gCount_crush_detach_list].car = pCar;
        gCrush_detach_list[gCount_crush_detach_list].field_0x8 = pArg3;
        if (pArg4) {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick;
        } else {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick > gINT_0067be84 + 0x3e8 ? gPHIL_last_physics_tick : gINT_0067be84 + 0x3e8;
        }
        if (gCrush_detach_list[gCount_crush_detach_list].time > gINT_0067be84) {
            gINT_0067be84 = gCrush_detach_list[gCount_crush_detach_list].time;
        }
        gCrush_detach_list[gCount_crush_detach_list].field_0x10 = (detach_data->type == eDetachType_fully_detachable);
        gCount_crush_detach_list = gCount_crush_detach_list + 1;
    }
}

// SetFlappage

// GetUniqueID

// InitializeCollisionInfo

// RemoveBit

// RemoveNonCarChildren

// RemoveNonCar

// GetNonCar

// GetNonCarObject

// SetRelationsDoingNothingFlags

// SetUpDetachedBitObject

// SetUpSemiDetachedBitJoint

// BitIsInBentPartOfCar

// SetUpFlapJoint

// FUNCTION: CARMA2_HW 0x0042d950
void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* material = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
            material->flags |= BR_MATF_TWO_SIDED;
            BrMaterialUpdate(material, BR_MATU_RENDERING);
        }
    }
}

// CheckFlapAngles

// FUNCTION: CARMA2_HW 0x0042d9b0
void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* mat = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (mat != NULL && (mat->flags & BR_MATF_TWO_SIDED) && mat->identifier != NULL && mat->identifier[0] == 'S') {
            mat->flags &= ~BR_MATF_TWO_SIDED;
            BrMaterialUpdate(mat, BR_MATU_RENDERING);
        }
    }
}

// ReAttachBit

// CentreModelOnPoint

// SemiDetachBit

// FUNCTION: CARMA2_HW 0x0042dbd0
int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor) {
    tUser_crush_data* user;
    br_model* model;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_flap_data* flap_data;

    user = pActor->user;
    if (user == NULL) {
        return 1;
    }
    model = user->models[0];
    if (model == NULL) {
        return 1;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        return 1;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == NULL) {
        return 1;
    }
    DRBoundsCopy(pBounds, &model->bounds);
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        pP1->v[0] = model->vertices[flap_data->hinge0].p.v[0];
        pP1->v[1] = model->vertices[flap_data->hinge0].p.v[1];
        pP1->v[2] = model->vertices[flap_data->hinge0].p.v[2];
        pP2->v[0] = model->vertices[flap_data->hinge1].p.v[0];
        pP2->v[1] = model->vertices[flap_data->hinge1].p.v[1];
        pP2->v[2] = model->vertices[flap_data->hinge1].p.v[2];
        pP33->v[0] = model->vertices[flap_data->hinge2].p.v[0];
        pP33->v[1] = model->vertices[flap_data->hinge2].p.v[1];
        pP33->v[2] = model->vertices[flap_data->hinge2].p.v[2];
    } else {
        pP1->v[0] = model->vertices[detach_data->field_0x30].p.v[0];
        pP1->v[1] = model->vertices[detach_data->field_0x30].p.v[1];
        pP1->v[2] = model->vertices[detach_data->field_0x30].p.v[2];
        pP2->v[0] = 0.f;
        pP2->v[1] = 0.f;
        pP2->v[2] = 0.f;
        pP33->v[0] = 0.f;
        pP33->v[1] = 0.f;
        pP33->v[2] = 0.f;
    }
    return 0;
}

// SendSemiDetachBit

// SendDetachBit

// SendAllSemiDetachedBits

// AddNonCar

// DetachBit

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: not implemented yet
int C2_HOOK_FASTCALL BitIsInBentPartOfCar(br_actor* pActor, float pArg2, float pArg3) {
    return 0;
}

// STUB: not implemented yet
tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds) {
    return NULL;
}

// STUB: not implemented yet
tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    return NULL;
}

// STUB: not implemented yet
void C2_HOOK_FASTCALL SendDetachBit(tCar_spec* pCar, br_actor* pActor) {
}

// STUB: CARMA2_HW 0x0042dab0
void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4) {
}

/* based on carpocalypse2 src/carma2/common/flap.c:188 */
// FUNCTION: CARMA2_HW 0x00436ad0
void C2_HOOK_FASTCALL DoDetaching(void) {
    extern int gCount_net_crush_detach_list;
    extern int gCount_net_crush_semi_detach_bit_list;
    extern tCrush_net_detach_list_item gNet_crush_detach_list[8];
    extern tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];

    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        int new_count_crush_detach_list;
        int i;

        new_count_crush_detach_list = 0;
        for (i = 0; i < gCount_crush_detach_list; i++) {
            tCrush_detach_list_item* detach_list_item;
            int keep;
            int synced;

            detach_list_item = &gCrush_detach_list[i];
            keep = 1;
            synced = 0;
            if (gPHIL_last_physics_tick >= detach_list_item->time) {
                keep = 0;

                if (!((detach_list_item->car->car_crush_spec->field_0x144
                            && BitIsInBentPartOfCar(detach_list_item->actor,
                                   detach_list_item->car->car_crush_spec->field_0x174,
                                   detach_list_item->car->car_crush_spec->field_0x180))
                        || detach_list_item->car->car_crush_spec->field_0x4b8)) {

                    if (gNet_mode == eNet_mode_none) {
                        if (detach_list_item->field_0x10) {

                            DetachBit(detach_list_item->car, detach_list_item->actor, NULL);
                        } else {
                            SemiDetachBit(detach_list_item->car, detach_list_item->actor, detach_list_item->field_0x8, &keep, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        if (detach_list_item->field_0x10) {
                            int j;

                            for (j = 0; j < gCount_net_crush_detach_list; j++) {
                                if (gNet_crush_detach_list[j].actor == detach_list_item->actor) {
                                    synced = 1;
                                    break;
                                }
                            }
                            if (!synced) {
                                SendDetachBit(detach_list_item->car, detach_list_item->actor);
                            }
                        } else {
                            int j;

                            for (j = 0; j < gCount_net_crush_semi_detach_bit_list; j++) {
                                if (gNet_crush_semi_detach_bit_list[j].actor == detach_list_item->actor) {
                                    synced = 1;
                                    break;
                                }
                            }
                            if (!synced) {
                                SendSemiDetachBit(detach_list_item->car, detach_list_item->actor, detach_list_item->field_0x8, NULL);
                            }
                        }
                    }
                }
            }
            if (keep && i != new_count_crush_detach_list) {
                gCrush_detach_list[new_count_crush_detach_list] = gCrush_detach_list[i];
                new_count_crush_detach_list += 1;
            }
        }
        gCount_crush_detach_list = new_count_crush_detach_list;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FlapBit

// FlapBits

// StartFlapping

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0042f3d0
void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush) {
    int i;
    int new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x270; i++) {
        tCar_bit_spec* car_bit = &pCar_crush->field_0x274[i];

        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        switch (car_bit->field_0xc[10]) {
        case 1:
            if (new_count != i) {
                pCar_crush->field_0x274[new_count] = *car_bit;
            }
            new_count += 1;
            break;
        case 2:
            pCar_crush->field_0x2b4[pCar_crush->field_0x2b0] = *car_bit;
            pCar_crush->field_0x2b0 += 1;
            break;
        }
    }
    pCar_crush->field_0x270 = new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x2b0; i++) {
        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        switch (pCar_crush->field_0x2b4[i].field_0xc[10]) {
        case 1:
            pCar_crush->field_0x274[pCar_crush->field_0x270] = pCar_crush->field_0x2b4[i];
            pCar_crush->field_0x270 += 1;
            break;
        case 2:
            if (new_count != i) {
                pCar_crush->field_0x2b4[new_count] = pCar_crush->field_0x2b4[i];
            }
            new_count += 1;
            break;
        }
    }
    pCar_crush->field_0x2b0 = new_count;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SendReAttachBit

// StopFlapping

/* StartFlapping and FlapBits only exist inlined into their callers in the original */
static void C2_HOOK_FASTCALL StartFlapping(br_actor* pActor, tCar_spec* pCar_spec, int pArg3, float pArg4) {
    NOT_IMPLEMENTED();
}

static void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

extern int gNum_active_cars;
extern tCar_spec* gActive_car_list[139];
extern tToggled_door gToggled_doors[16];
extern int gCount_toggled_doors;
extern void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);

// FUNCTION: CARMA2_HW 0x004381b0
void C2_HOOK_FASTCALL DoFlapping(void) {
    int i;

    for (i = 0; i < gCount_toggled_doors; i++) {
        tToggled_door *toggle_door = &gToggled_doors[i];
        if (toggle_door->actor != NULL) {
            tUser_crush_data *user_crush_data = toggle_door->actor->user;
            if (user_crush_data != NULL && user_crush_data->crush_data != NULL &&
                user_crush_data->crush_data->flap_data != NULL) {
                tCar_crush_flap_data *flap_data = user_crush_data->crush_data->flap_data;
                if (!flap_data->field_0x0 && (toggle_door->field_0x8 & 0x1)) {
                    StartFlapping(toggle_door->actor, toggle_door->car, flap_data->field_0x0, toggle_door->field_0xc);
                } else if (flap_data->field_0x0 && !(toggle_door->field_0x8 & 0x1)) {
                    StopFlapping(toggle_door->actor, toggle_door->car);
                }
            }
        }
    }
    gCount_toggled_doors = 0;
    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car != NULL && car->driver >= eDriver_oppo) {
            FlapBits(car);
        }
    }
}

// ToggleFlapsCB2

extern int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data);

// FUNCTION: CARMA2_HW 0x0042ddb0
int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data) {
    tCar_spec* pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleDoorsActorCallback, pCar_spec);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ddc0
int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data) {
    tCar_spec* pCar = data;
    tUser_crush_data* user_data;
    tCar_crush_buffer_entry* crush_buffer;
    int i;

    user_data = pActor->user;
    if (user_data == NULL) {
        return 0;
    }
    crush_buffer = user_data->crush_data;
    if (crush_buffer == NULL) {
        return 0;
    }
    if (crush_buffer->flap_data == NULL || !crush_buffer->flap_data->kev_o_flap) {
        return 0;
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(gToggled_doors) != 16);

    if (crush_buffer->flap_data->field_0x0) {
        if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 0;
            gToggled_doors[gCount_toggled_doors].field_0xc = 0.f;
            gCount_toggled_doors += 1;
        }
    } else {
        if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 1;
            gToggled_doors[gCount_toggled_doors].field_0xc = 20.f;
            gCount_toggled_doors += 1;
        }
    }
    return 0;
}

// SendToggleFlaps

// ToggleFlaps

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00434910
int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject) {
    br_vector3 world_direction;
    br_vector3 parent_direction;
    const br_vector3* limit_direction;
    br_scalar alignment;
    int i;

    for (i = 0; i < pObject->physics_joint1->count_limits; i++) {
        BrMatrix34ApplyV(&world_direction, &pObject->physics_joint1->limits[i].child, &pObject->actor->t.t.mat);
        BrMatrix34TApplyV(&parent_direction, &world_direction, &pObject->parent->actor->t.t.mat);
        limit_direction = &pObject->physics_joint1->limits[i].parent;
        alignment = BrVector3Dot(limit_direction, &parent_direction);
        if (alignment > 0.707107f) {
            return 1;
        }
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FullyDetachBit

// SendFullyDetachBit

extern int gNum_active_cars;
extern tCar_spec* gActive_car_list[139];
extern tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];
extern int gCount_net_crush_full_detach_bit_list;
extern void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);

// FUNCTION: CARMA2_HW 0x00438900
void C2_HOOK_FASTCALL DoFullyDetaching(void) {
    int i;

    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        for (i = 0; i < gNum_active_cars; i++) {
            if (gActive_car_list[i] != NULL && gActive_car_list[i]->driver > 5 && gActive_car_list[i]->car_crush_spec != NULL) {
                tCar_crush_spec* car_crush = gActive_car_list[i]->car_crush_spec;
                int j;

                for (j = 0; j < car_crush->field_0x270; j++) {
                    tCar_bit_spec* bit_car = &car_crush->field_0x274[j];
                    int detach;
                    int dt;

                    switch (bit_car->field_0xc[9]) {
                    case 2:
                    case 4:
                        detach = 1;
                        break;
                    case 0:
                    case 1:
                        detach = BitObjectIsSufficientlyOutsideCarObjectToDetach(bit_car->field_0x8);
                        dt = PDGetTotalTime() - bit_car->field_0x10;
                        if (dt < gMax_detach_time_ms) {
                            detach &= PercentageChance((int)((float)(100 * dt) / gMax_detach_time_ms));
                        }
                        break;
                    default:
                        detach = 0;
                        break;
                    }
                    if (detach) {
                        if (gNet_mode != eNet_mode_none) {
                            int bit_synced;
                            int k;

                            bit_synced = 0;
                            for (k = 0; k < gCount_net_crush_full_detach_bit_list; k++) {
                                if (gNet_crush_full_detach_bit_list[k].object == bit_car->field_0x8) {
                                    bit_synced = 1;
                                    break;
                                }
                            }
                            if (!bit_synced) {
                                SendFullyDetachBit(gActive_car_list[i], bit_car->field_0x8);
                            }
                        } else {
                            FullyDetachBit(gActive_car_list[i], bit_car->field_0x8);
                        }
                    }
                }
                MungeDetachLists(car_crush);
            }
        }
    }
}

// CalculateCollisionObjectStuffForSplitHalves

// SetUpCollisionInfoStuffForSplitHalf

// SetFace

// AddSplitVertexIndex

// CreateSplitVertex

// GetSplitVertex

// SplitModel

// CreateSplitPoints

// SplitCollisionShape

// SplitCarCollisionShapes

// based on carpocalypse2 src/carma2/common/crush1.c (GPL-3.0)
// FUNCTION: CARMA2_HW 0x00430d80
int C2_HOOK_FASTCALL QuantizeWithinBounds(br_scalar pQuant, br_scalar pMin, br_scalar pMax, br_scalar* pValue) {
    br_scalar range = pMax - pMin;
    br_scalar rounded;
    br_scalar step;
    int64_t integral;
    tS32 count;
    tS32 index;

    /* Choose a whole number of cells spanning the bounds. */
    rounded = range / pQuant;
    rounded += rounded > 0.f ? 0.5f : -0.5f;
    /* Counts and indices wrap to signed 32 bits after truncation. */
    if (rounded >= -9223372036854775808.0 && rounded < 9223372036854775808.0) {
        integral = (tU32)(int64_t)rounded;
    } else {
        integral = 0;
    }
    if (integral > 0x7fffffff) {
        integral -= (int64_t)0xffffffffu + 1;
    }
    count = (tS32)integral;
    if (count == 0) {
        count = 1;
    }
    step = range / (br_scalar)count;

    /* Snap to the grid only when the selected cell is within the bounds. */
    rounded = (*pValue - pMin) / step;
    rounded += rounded > 0.f ? 0.5f : -0.5f;
    if (rounded >= -9223372036854775808.0 && rounded < 9223372036854775808.0) {
        integral = (tU32)(int64_t)rounded;
    } else {
        integral = 0;
    }
    if (integral > 0x7fffffff) {
        integral -= (int64_t)0xffffffffu + 1;
    }
    index = (tS32)integral;
    if (index < 0 || index > count) {
        return 0;
    }
    *pValue = (br_scalar)index * step + pMin;
    return 1;
}

// SaveVitalBitsOfObject

// FUNCTION: CARMA2_HW 0x00430730
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_spec* car_spec = pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsDoubleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

// SwapCarShapes

// IsAWheelOrPivotActor

// FUNCTION: CARMA2_HW 0x0042deb0
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_spec* car_spec = pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsSingleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

// IsOnNonCarHalf

// SplitCar

/* inlined into SplitCheck in the original */
static void C2_HOOK_FASTCALL QuantizePlane(br_vector4* pPlane) {
    tU16 compressed[8];

    CompressVector3((tCompressed_vector3*)&compressed[4], (br_vector3*)pPlane, -1.f, 1.f);
    compressed[7] = DRScalarToU16(pPlane->v[3], -2.5f, 2.5f);
    ExpandVector3((br_vector3*)pPlane, (tCompressed_vector3*)&compressed[0], -1.f, 1.f);
    pPlane->v[3] = DRU16ToScalar(compressed[3], -2.5f, 2.5f);
}

/* inlined into SplitCheck in the original */
static int C2_HOOK_FASTCALL WithinBounds(br_scalar pValue, br_scalar pMin, br_scalar pMax) {
    return pValue >= pMin && !(pValue > pMax);
}

// based on ApexWorm/rec2 (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004307e0
int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane) {
    tCar_crush_spec* crush;
    const br_bounds3* bounds;
    br_vector3 size;
    br_vector3 normal;
    br_vector3 point;
    br_scalar min_x;
    br_scalar max_x;
    br_scalar min_z;
    br_scalar max_z;
    br_scalar first;
    br_scalar second;
    br_scalar distance;
    int axis;
    int next_axis;
    int last_axis;
    int i;

    if (pCar->invulnerable_no_crushage != 0 && pCar->knackered == 0) {
        return 0;
    }
    if (pCar->field_0x4c8 < 1.f || pCar->field_0x4c8 > 1.f) {
        return 0;
    }
    crush = pCar->car_crush_spec;
    if (crush == NULL || crush->field_0x144 != 0 || crush->field_0x4b8 != 0) {
        return 0;
    }

    bounds = &pCar->collision_info->bb1;
    BrVector3Sub(&size, &bounds->max, &bounds->min);
    /* Keep vertical cuts within the middle half of the collision bounds. */
    min_x = bounds->min.v[0] + 0.25f * size.v[0];
    max_x = bounds->max.v[0] - 0.25f * size.v[0];
    min_z = bounds->min.v[2] + 0.25f * size.v[2];
    max_z = bounds->max.v[2] - 0.25f * size.v[2];

    if (pMode == 1) {
        pPlane->v[0] = 0.f;
        pPlane->v[1] = 1.f;
        pPlane->v[2] = 0.f;
        pPlane->v[3] = pHeight;
        if (!QuantizeWithinBounds(0.02f, bounds->min.v[1], bounds->max.v[1], &pPlane->v[3])) {
            return 0;
        }
    } else if (pMode == 0) {
        if (fabs(pLine->v[0]) > 0.001f) {
            first = (pLine->v[2] - bounds->min.v[1] * pLine->v[1]) / pLine->v[0];
            second = (pLine->v[2] - bounds->max.v[1] * pLine->v[1]) / pLine->v[0];
            if (QuantizeWithinBounds(0.02f, bounds->min.v[0], bounds->max.v[0], &first)
                    && QuantizeWithinBounds(0.02f, bounds->min.v[0], bounds->max.v[0], &second)
                    && WithinBounds(0.5f * (first + second), min_x, max_x)) {
                normal.v[0] = -size.v[1];
                normal.v[1] = 0.f;
                normal.v[2] = second - first;
                BrVector3Normalise(&normal, &normal);
                pPlane->v[0] = normal.v[0];
                pPlane->v[1] = normal.v[1];
                pPlane->v[2] = normal.v[2];
                pPlane->v[3] = pPlane->v[0] * first + pPlane->v[2] * bounds->min.v[1];
                goto check_plane;
            }
        }
        if (!(fabs(pLine->v[1]) > 0.001f)) {
            return 0;
        }
        first = (pLine->v[2] - bounds->min.v[0] * pLine->v[0]) / pLine->v[1];
        second = (pLine->v[2] - bounds->max.v[0] * pLine->v[0]) / pLine->v[1];
        if (!QuantizeWithinBounds(0.02f, bounds->min.v[2], bounds->max.v[2], &first)
                || !QuantizeWithinBounds(0.02f, bounds->min.v[2], bounds->max.v[2], &second)
                || !WithinBounds(0.5f * (first + second), min_z, max_z)) {
            return 0;
        }
        normal.v[0] = second - first;
        normal.v[1] = 0.f;
        normal.v[2] = -size.v[0];
        BrVector3Normalise(&normal, &normal);
        pPlane->v[0] = normal.v[0];
        pPlane->v[1] = normal.v[1];
        pPlane->v[2] = normal.v[2];
        pPlane->v[3] = pPlane->v[0] * bounds->min.v[0] + pPlane->v[2] * first;
    } else {
        return 0;
    }

check_plane:
    bounds = &pCar->car_actor->model->bounds;
    normal.v[0] = pPlane->v[0];
    normal.v[1] = pPlane->v[1];
    normal.v[2] = pPlane->v[2];
    axis = NearestAxis(&normal);
    next_axis = (axis + 1) % 3;
    last_axis = (axis + 2) % 3;
    point.v[axis] = 0.f;
    point.v[next_axis] = 0.5f * (bounds->min.v[next_axis] + bounds->max.v[next_axis]);
    point.v[last_axis] = 0.5f * (bounds->min.v[last_axis] + bounds->max.v[last_axis]);
    /* Leave clearance where the plane crosses the centre of the model. */
    distance = -(((point.v[1] * pPlane->v[1] + point.v[2] * pPlane->v[2])
            + point.v[0] * pPlane->v[0] - pPlane->v[3]) / pPlane->v[axis]);
    if (!(distance >= bounds->min.v[axis] + 0.057971016f)
            || distance > bounds->max.v[axis] - 0.057971016f) {
        return 0;
    }

    /* Keep the driver on the positive side of the plane. */
    distance = (crush->driver_position.v[1] * pPlane->v[1]
            + crush->driver_position.v[2] * pPlane->v[2])
            + crush->driver_position.v[0] * pPlane->v[0] - pPlane->v[3];
    if (!(distance >= 0.f)) {
        for (i = 0; i < 4; i++) {
            pPlane->v[i] = -pPlane->v[i];
        }
    }
    QuantizePlane(pPlane);
    return 1;
}

// FindForceEdge

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

// PointIsOutsideShape

// PointIsOutsideLimits

// ClipPointToPolyhedron

// QuantizeVector3

// MoveCarToSensiblePlace

// SwapShapesIfPossible

// DoShapeSwapping

// BendPoint

// EdgeCrossesZPlane

// BendCollisionShape

// BendCollisionShapes

// BendModel

// BendModels

// BrAngleToSignedDegree

// BendWheels

// BendActor

// BendOtherThings

// ReAttachOrFullyDetachDifficultBits

// BendCar

// QuantizeAngle

/* inlined into DoBending in the original */
static int C2_HOOK_FASTCALL BendCarOneForce(tCar_spec* pCar, tCar_crush* pCar_crush) {
    (void)pCar;
    (void)pCar_crush;
    return 0;
}

extern tCrush_list_item gCrush_lists[8];

// based on carpocalypse2 src/carma2/common/flap.c (GPL-3.0)
// FUNCTION: CARMA2_HW 0x00438a90
void C2_HOOK_FASTCALL DoBending(void) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_crush_spec, 0x18d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x4b8, 0x4b8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x144, 0x144);

    for (i = 0; gCrush_lists[i].car_spec != NULL; i++) {

        tCar_crush_spec* ptVar2 = gCrush_lists[i].car_spec->car_crush_spec;
        if (!ptVar2->field_0x4b8 && !ptVar2->field_0x144) {
            int j;

            for (j = 0; j < gCrush_lists[i].count; j++) {
                if (gCrush_lists[i].items[j].field_0x40 < 0.f) {
                    gCrush_lists[i].items[j].field_0x40 = -gCrush_lists[i].items[j].field_0x40;
                    if (BendCarOneForce(gCrush_lists[i].car_spec, &gCrush_lists[i].items[j])) {
                        break;
                    }
                }
            }
        }
        gCrush_lists[i].car_spec = NULL;
        i = i + 1;
    }
}

// FUNCTION: CARMA2_HW 0x00431590
void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    (void)pOn_screen;
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
    pModel->flags &= ~BR_MODF_CUSTOM;
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
}

// FUNCTION: CARMA2_HW 0x004315e0
void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;

    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        user_detail_model = pModel->user;
        user_detail_model->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}

// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

// DRActorEnumRecurseUltraPlus

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

static void C2_HOOK_FASTCALL DoSpams(void) { }
static void C2_HOOK_FASTCALL DoCrushing(void) { }
static void C2_HOOK_FASTCALL DoDroneCrushing(void) { }
static void C2_HOOK_FASTCALL DoShapeSwapping(void) { }
static void C2_HOOK_FASTCALL ClearCrushLists(void) { }

/* based on carpocalypse2 src/carma2/common/crush.c:4081 */
// FUNCTION: CARMA2_HW 0x00436170
void C2_HOOK_FASTCALL CrushBendFlapRend(void) {

    DoSpams();
    DoCrushing();
    DoDroneCrushing();
    DoShapeSwapping();
    DoFlapping();
    DoDetaching();
    DoFullyDetaching();
    DoBending();
    ClearCrushLists();
}

// SphericizeModel

// SphericizePhysics

// SphericizeCar

extern void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);

// FUNCTION: CARMA2_HW 0x004398d0
// FUNCTION: CARMA2_SW 0x00439910
intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    tCar_spec* car_spec = pUser;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    if (crush_data->flap_data == NULL) {
        return 0;
    }
    if (crush_data->flap_data->field_0x0 && !crush_data->flap_data->kev_o_flap) {
        StopFlapping(pActor, car_spec);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004398c0
// FUNCTION: CARMA2_SW 0x00439900
int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, BattenDownTheHatches, pUser_data);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439fd0
// FUNCTION: CARMA2_SW 0x0043a010
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape) {
    int i;

    pShape->count_points = pShape->original_count_points;
    for (i = 0; i < pShape->count_points; i++) {
        BrVector3Copy(&pShape->field_0x18[i].field_0x18, &pShape->field_0x18[i].field_0x2c);
    }
}

// CompletelyUnBendCollisionShapes

extern void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);

// based on carpocalypse2 src/carma2/common/crush.c:1875 (reference only)
// FUNCTION: CARMA2_HW 0x0043a010
void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor* actor;
        tGroovidelic_spec* groove;

        actor = pCar_spec->pivot_actors[i];
        if (actor == NULL) {
            actor = pCar_spec->wheel_actors[i];
            if (actor == NULL) {
                continue;
            }
        }
        groove = ActorsGroove(actor);
        if (groove != NULL && groove->path_type == eGroove_path_straight) {
            br_vector3* centre = &groove->path_data.straight_info.centre;
            br_vector3 delta;

            delta.v[0] = -centre->v[0];
            delta.v[1] = -centre->v[1];
            delta.v[2] = -centre->v[2];
            BrVector3Copy(centre, &pCar_spec->car_crush_spec->field_0x1ec[i]);
            delta.v[0] += centre->v[0];
            delta.v[1] += centre->v[1];
            delta.v[2] += centre->v[2];
            PipeSingleVector3(centre, &delta);
        }
    }

    for (i = 0; i < REC2_ASIZE(pCar_spec->wpos); i++) {
        BrVector3Copy(&pCar_spec->wpos[i], &pCar_spec->car_crush_spec->field_0x234[i]);
    }
    SetCarSuspGiveAndHeight(pCar_spec, 1.f, 1.f, 1.f, 0.f, 0.f);
    pCar_spec->field_0x1260 = 0.f;
}

// CompletelyUnBendOtherThings

// FUNCTION: CARMA2_HW 0x00439910
intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 0x3);
        EnableGroovidelic(pActor, 0x3);
        PipeSingleTransformType(pActor, BR_TRANSFORM_MATRIX34_LP);
        pActor->t.type = BR_TRANSFORM_MATRIX34_LP;
    }
    return 0;
}

// DoCompletelyUnBentThings

// FUNCTION: CARMA2_HW 0x00439960
void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec *pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {

        tCar_crush_shape_info *shape_info;
        int j;

        shape_info = &pCar_spec->car_crush_spec->field_0x4[i];
        for (j = 0; j < shape_info->count_points; j++) {

            BrVector3Copy(&shape_info->field_0x18[j].field_0x18,
                          &shape_info->field_0x18[j].field_0x0);
            if (DRVector3TestForNan(&shape_info->field_0x18[j].field_0x18)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

extern void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash);

// TotallyRepairModel is inlined into TotallyRepairModels by the original compiler
// (there is no out-of-line copy of it in the binary), so its body is written out here.

// FUNCTION: CARMA2_HW 0x00439a20
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {
    br_model* model;
    tCar_spec* car_spec;
    tUser_crush_data* user_crush;
    tCar_crush_buffer_entry* crush_data;
    tUser_detail_level_model* user_detail_model;
    tModel_detail_vertex_data* vertex_detail_data;
    br_vector3 vector_mash[1024];
    int i;
    tU16 j;

    car_spec = pUser;
    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        model = user_crush->models[i];
        if (model == NULL) {
            continue;
        }
        user_detail_model = model->user;
        if (user_detail_model == NULL) {
            continue;
        }
        vertex_detail_data = user_detail_model->field_0x4;
        if (vertex_detail_data == NULL) {
            continue;
        }
        if (model->nvertices > REC2_ASIZE(vector_mash)) {
            PDFatalError("Holy polygons Batman! There's a lot of vertices on that model.");
        }
        for (j = 0; j < model->nvertices; j++) {
            br_vector3 pos;

            ExpandVector3(&pos, &vertex_detail_data[j].p, -10.0f, 10.0f);
            BrVector3Sub(&vector_mash[j], &pos, &model->vertices[j].p);
            BrVector3Copy(&model->vertices[j].p, &pos);
            vertex_detail_data[j].flags &= ~0x4;
        }
        AddModelMashToPipingSession(model, vector_mash);
        SetModelForUpdate(model);
    }
    crush_data = user_crush->crush_data;
    if (crush_data != NULL) {
        TotallyRepairSmash(car_spec, crush_data);
        crush_data->field_0x2c = 0;
        if (crush_data->flap_data != NULL && !crush_data->flap_data->kev_o_flap) {
            crush_data->flap_data->field_0x15 = 0;
            crush_data->flap_data->field_0x14 = 1;
            crush_data->flap_data->field_0x18 = 0.f;
        }
        if (crush_data->detach_data != NULL) {
            crush_data->detach_data->field_0x0 = 0.f;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439a10
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}

// SendReAttachAllBits

extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pActor_arg2, br_actor* pActor_arg3, tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL PipeSingleDSModel(int pKind, void* pThing);
extern void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00439510
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
    int i;
    int j;
    tCar_crush_spec* car_crush;

    StopCarSmokingInstantly(pCar_spec);
    if (ARIsActionReplayAvailable()) {
        PipeInstantUnSmudge(pCar_spec);
    }
    pCar_spec->repair_time += 100000;
    for (i = 0; i < REC2_ASIZE(pCar_spec->damage_units); i++) {
        pCar_spec->damage_units[i].damage_level = 0;
        pCar_spec->damage_units[i].last_level = 0;
        pCar_spec->damage_units[i].smoke_last_level = 0;
    }
    if (pCar_spec->use_shell_model) {
        return;
    }
    car_crush = pCar_spec->car_crush_spec;
    if (car_crush == NULL) {
        return;
    }
    if (car_crush->field_0x4b8 != 0) {
        WeldCar(pCar_spec);
    }
    for (i = 0; i < car_crush->field_0x2b0; i++) {
        ReAttachBit(pCar_spec, car_crush->field_0x2b4[i].field_0x4, car_crush->field_0x2b4[i].field_0x0, NULL);
    }
    car_crush->field_0x2b0 = 0;
    for (i = 0; i < car_crush->field_0x270; i++) {
        ReAttachBit(pCar_spec, car_crush->field_0x274[i].field_0x4, car_crush->field_0x274[i].field_0x0, car_crush->field_0x274[i].field_0x8);
    }
    car_crush->field_0x270 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar_spec);
    PipeSingleDSModel(0, pCar_spec);
    if (car_crush->field_0x144) {
        tCar_crush_shape_info* shape;

        pCar_spec->car_crush_spec->field_0x144 = 0;
        for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
            shape = &pCar_spec->car_crush_spec->field_0x4[i];
            shape->count_points = shape->original_count_points;
            for (j = 0; j < shape->count_points; j++) {
                BrVector3Copy(&shape->field_0x18[j].field_0x18, &shape->field_0x18[j].field_0x2c);
            }
        }
        pCar_spec->car_crush_spec->expand_bounding_box = 3;
        for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
            br_actor* actor;
            tGroovidelic_spec* groove;

            actor = pCar_spec->pivot_actors[i];
            if (actor == NULL) {
                actor = pCar_spec->wheel_actors[i];
                if (actor == NULL) {
                    continue;
                }
            }
            groove = ActorsGroove(actor);
            if (groove != NULL && groove->path_type == eGroove_path_straight) {
                br_vector3 original_centre;
                br_vector3 delta;

                BrVector3Negate(&original_centre, &groove->path_data.straight_info.centre);
                BrVector3Copy(&groove->path_data.straight_info.centre, &pCar_spec->car_crush_spec->field_0x1ec[i]);
                BrVector3Add(&delta, &groove->path_data.straight_info.centre, &original_centre);
                PipeSingleVector3(&groove->path_data.straight_info.centre, &delta);
            }
        }
        for (i = 0; i < REC2_ASIZE(pCar_spec->wpos); i++) {
            BrVector3Copy(&pCar_spec->wpos[i], &pCar_spec->car_crush_spec->field_0x234[i]);
        }
        SetCarSuspGiveAndHeight(pCar_spec, 1.f, 1.f, 1.f, 0.f, 0.f);
        pCar_spec->field_0x1260 = 0.f;
        BrVector3Copy(&pCar_spec->collision_info->cmpos, &pCar_spec->car_crush_spec->field_0x264);
        DRActorEnumRecurse(pCar_spec->car_model_actor, EnableGroovers, NULL);
    }
    ARStartPipingSession(ePipe_model_mash);
    PhysicsObjectRecurse(pCar_spec->collision_info, TotallyRepairObject, pCar_spec);
    AREndPipingSession();
    dr_dprintf("TotallyRepairACar() - Point 7");
    TotallyRepairCarCollisionShapes(pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 8");
    PhysicsObjectRecurse(pCar_spec->collision_info, BattenDownTheObjects, pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 9");
    if (gProgram_state.prog_status != eProg_idling) {
        dr_dprintf("TotallyRepairACar() - Point 10");
        SwapShapesIfPossible(pCar_spec);
    }
    dr_dprintf("TotallyRepairACar() - Point 11");
    pCar_spec->use_shell_model = 1;
    RemoveCarFromCrushLists(pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 12");
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00439bf0
// FUNCTION: CARMA2_SW 0x00439c30
void C2_HOOK_FASTCALL TotallyRepairCar(void) {

    if (!gArrow_mode) {
        TotallyRepairACar(&gProgram_state.current_car);
    }
}

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

// UnBendCollisionShape

// UnBendCollisionShapes

// UnBendWheels

// UnBendOtherThings

// FUNCTION: CARMA2_HW 0x0043ac90
// FUNCTION: CARMA2_SW 0x0043ace0
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle / (double)BR_ONE_LS);
    } else {
        return (br_scalar)((br_angle)-pAngle / (double)BR_ONE_LS);
    }
}

// CalculateAmountRepairedFraction

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00439c30
br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00439c10
void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec) {
    br_scalar original_amount;

    UnBendCar(pCar_spec, 2000, &original_amount);
}

// ReAttachBits

// RepairObject

// RepairCarCollisionShapes

// RepairCar2

// RepairCar

// RestoreVitalBitsOfObject

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

// FUNCTION: CARMA2_HW 0x0043bf30
void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc) {
    tU16 i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pDest[i], &pSrc[i], -10.0f, 10.0f);
    }
}

// FUNCTION: CARMA2_HW 0x0043bf80
// FUNCTION: CARMA2_SW 0x0043bfd0
void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc) {
    tU16 i;
    br_vector3 delta;

    for (i = 0; i < pModel->nvertices; i++) {
        ExpandVector3(&delta, &pSrc[i], -10.0f, 10.0f);
        BrVector3Accumulate(&pModel->vertices[i].p, &delta);
    }
    SetModelForUpdate(pModel);
}

// ARUndoModelMash

// ARApplyFlap

// ARApplySplitWeld

// ARUndoSplitWeld

// ReceivedCrush

// FindBitWithID

// AttachedBitFromID

// ReceivedSemiDetachBit

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

// ReceivedFullyDetachBit

// DetachedBitFromID

// SemiDetachedBitFromID

// ReceivedReAttachBit

// ReceivedReAttachAllBits

// DoQueuedNetDetaching

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

// ReceivedToggleFlaps

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

// GetNetShapeDataLengthFromShape

// GetNetShapeDataLengthFromData

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

// FUNCTION: CARMA2_HW 0x0043deb0
void C2_HOOK_FASTCALL CompressPlane(tU16* pDest, const br_vector4* pSrc) {

    CompressVector3((tCompressed_vector3*)pDest, (const br_vector3*)pSrc, -1.f, 1.f);
    pDest[3] = DRScalarToU16(pSrc->v[3], -2.5f, 2.5f);
}

// FUNCTION: CARMA2_HW 0x0043dee0
void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16* pSrc) {

    ExpandVector3((br_vector3*)pDest, (const tCompressed_vector3*)pSrc, -1.f, 1.f);
    pDest->v[3] = DRU16ToScalar(pSrc[3], -2.5f, 2.5f);
}

// CopySplitInMessage

// FUNCTION: CARMA2_HW 0x0043dfd0
void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id != 0) {
            pShapes->items[i].field_0x4 = 1;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0042bb00
tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel) {
    tU16 closest_vertex;
    br_scalar closest_dist;
    tU16 i;

    closest_vertex = 0;
    closest_dist = BR_SCALAR_MAX;
    for (i = 0; i < pModel->nvertices; i++) {
        br_scalar d;

        d = Vector3DistanceSquared(pPoint, &pModel->vertices[i].p);
        if (d >= closest_dist) {
            goto next_vertex;
        }
        closest_dist = d;
        closest_vertex = i;
next_vertex:;
    }
    return closest_vertex;
}

// FUNCTION: CARMA2_HW 0x0042bb90
void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds) {
    float closest;
    const br_vector3* model_bounds[2];
    br_vector3 corner;
    br_vector3 closest_corner;
    tU8 closest_corner_index;
    tU8 closest_side;
    int i;

    model_bounds[0] = &pModel->bounds.min;
    model_bounds[1] = &pModel->bounds.max;
    if (pDetach_data->type == eDetachType_joint_index) {
        float dist_bb;

        closest_corner = pModel->vertices[pDetach_data->field_0x30].p;
        closest = BR_SCALAR_MAX;
        for (i = 0; i < 8; i++) {
            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_bb = Vector3DistanceSquared(&corner, &closest_corner);
            if (dist_bb < closest) {
                closest = dist_bb;
                closest_corner_index = i;
            }
        }
    } else {
        closest = BR_SCALAR_MAX;
        for (i = 0; i < 8; i++) {
            float dist_closest_side;
            int j;

            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_closest_side = BR_SCALAR_MAX;
            for (j = 0; j < 3; j++) {
                float d;

                d = (float)fabs(corner.v[j] - pBounds->min.v[j]);
                if (d < dist_closest_side) {
                    dist_closest_side = d;
                }
                d = (float)fabs(corner.v[j] - pBounds->max.v[j]);
                if (d < dist_closest_side) {
                    dist_closest_side = d;
                }
            }
            if (dist_closest_side < closest) {
                closest = dist_closest_side;
                closest_corner = corner;
                closest_corner_index = i;
            }
        }
        pDetach_data->field_0x30 = FindNearestVertex(&closest_corner, pModel);
    }
    pDetach_data->field_0x32 = closest_corner_index;
    Vector3Average(&corner, &pModel->bounds.min, &pModel->bounds.max);
    closest = BR_SCALAR_MAX;
    closest_side = 0;
    model_bounds[0] = &pBounds->min;
    model_bounds[1] = &pBounds->max;
    for (i = 0; i < 6; i++) {
        if (i != 2) {
            float d;

            d = (float)fabs(corner.v[i >> 1] - model_bounds[i & 1]->v[i >> 1]);
            if (d < closest) {
                closest = d;
                closest_side = i;
            }
        }
    }
    pDetach_data->field_0x33 = closest_side;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004f57c0
void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data) {
    int count_indices;
    int count_vertices;
    int i;
    int j;
    tCar_crush_smashable_part* smashable;
    tU16 indices[50];
    tCar_crush_vertex_data vertices[50];

    smashable = pCrush_data->smashables;
    for (i = 0; i < pCrush_data->count_smashables; i++, smashable++) {
        smashable->funk_material = FindMaterial(smashable->material_name, pActor, 0);
        if (smashable->funk_material == NULL) {
            FatalError(kFatalError_CannotFindSmashMaterial_S, smashable->material_name);
        }
        MungeMaterial(pActor, 0, smashable->funk_material, smashable->funk_material, 50, indices, &count_indices, 50, vertices, &count_vertices, &smashable->field_0x60);
        for (j = 0; j < count_indices; j++) {
            if (j != 0) {
                BrVector3Accumulate(&smashable->field_0x54, &pActor->model->vertices[indices[j]].p);
            } else {
                BrVector3Copy(&smashable->field_0x54, &pActor->model->vertices[indices[0]].p);
            }
        }
        if (count_indices != 0) {
            smashable->field_0x54.v[0] = smashable->field_0x54.v[0] / (double)count_indices;
            smashable->field_0x54.v[1] = smashable->field_0x54.v[1] / (double)count_indices;
            smashable->field_0x54.v[2] = smashable->field_0x54.v[2] / (double)count_indices;
        }
        smashable->field_0x6c = count_indices;
        if (count_indices != 0) {
            smashable->field_0x70 = BrMemAllocate(count_indices * sizeof(tU16), kMem_smash_poly_array);
            memcpy(smashable->field_0x70, indices, count_indices * sizeof(tU16));
        } else {
            smashable->field_0x70 = NULL;
        }
        smashable->field_0x74 = count_vertices;
        if (count_vertices != 0) {
            smashable->field_0x78 = BrMemAllocate(count_vertices * sizeof(tCar_crush_vertex_data), kMem_smash_vertex_array);
            memcpy(smashable->field_0x78, vertices, count_vertices * sizeof(tCar_crush_vertex_data));
        } else {
            smashable->field_0x78 = NULL;
        }
        BrResFree(smashable->funk_material->identifier);
        smashable->funk_material->identifier = NULL;
        smashable->field_0x4c = 0;
        smashable->funk_material->colour_map = smashable->levels->pixelmaps[0];
        BrMaterialUpdate(smashable->funk_material, BR_MATU_COLOURMAP);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec);
extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3);

static void Crush1ClipPointToPolyhedron(br_vector3* pPoint, tCollision_shape_polyhedron_data* pPolyhedron) {
    int i;

    for (i = 0; i < pPolyhedron->count_planes; i++) {
        br_vector4* plane = &pPolyhedron->planes[i];
        br_scalar proj_factor;

        proj_factor = BrVector3Dot(plane, pPoint) + plane->v[3];
        if (proj_factor < gCrushZero) {
            br_vector3 proj;

            BrVector3Scale(&proj, plane, -proj_factor);
            BrVector3Accumulate(pPoint, &proj);
        }
    }
}

static int Crush1PointIsOutsideShape(br_vector3* pPoint, tCollision_shape_polyhedron_data* pPolyhedron) {
    int i;

    for (i = 0; i < pPolyhedron->count_planes; i++) {
        br_vector4* plane = &pPolyhedron->planes[i];

        if (BrVector3Dot(plane, pPoint) + plane->v[3] < -1e-5) {
            return 1;
        }
    }
    return 0;
}

static int Crush1PointIsOutsideLimits(tCar_crush_shape_info* pShape, int pPoint_index, tCar_crush_limits* pLimits) {
    int i;
    tU32 flags;

    flags = pShape->field_0x18[pPoint_index].field_0x28;
    for (i = 0; i < 3; i++) {
        br_scalar v = pShape->field_0x18[pPoint_index].field_0x18.v[i];

        if (v < pLimits->limits[i][0].values[(flags >> 0) & 0x3]) {
            return 1;
        }
        if (v > pLimits->limits[i][1].values[(flags >> 2) & 0x3]) {
            return 1;
        }
        flags >>= 4;
    }
    return 0;
}

static void Crush1QuantizeVector3(br_vector3* pV) {
    tCompressed_vector3 cv3;

    CompressVector3(&cv3, pV, -2.5f, 2.5f);
    ExpandVector3(pV, &cv3, -2.5f, 2.5f);
}

static void Crush1SwapCarShapes(tCar_spec* pCar_spec, tPhysics_shape* pCollision_shape) {
    pCar_spec->collision_info->shape = pCollision_shape;
    UpdateCollisionObject(pCar_spec->collision_info);
    pCar_spec->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar_spec);
}

static int Crush1MoveCarToSensiblePlace(tCar_spec* pCar_spec) {
    br_vector3 original_pos;
    float factor;
    int i;
    br_vector3 prev_delta;

    factor = 0.007246377f;
    BrVector3Set(&prev_delta, 0.f, 0.f, 0.f);
    BrVector3Copy(&original_pos, (br_vector3*)&pCar_spec->collision_info->transform_matrix.m[3]);
    for (i = 0; i < 10; i++) {
        br_vector3 delta;

        if (TestForCarInSensiblePlace(pCar_spec, &delta)) {
            return 1;
        }
        if (BrVector3Dot(&prev_delta, &delta) > 0.707107f) {
            factor *= 2.f;
        } else {
            BrVector3Copy(&prev_delta, &delta);
            factor = 0.007246377f;
        }
        delta.v[0] = factor * delta.v[0];
        delta.v[1] = factor * delta.v[1];
        delta.v[2] = factor * delta.v[2];
        BrVector3Accumulate((br_vector3*)&pCar_spec->collision_info->transform_matrix.m[3], &delta);
    }
    BrVector3Copy((br_vector3*)&pCar_spec->collision_info->transform_matrix.m[3], &original_pos);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004fc740
void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}
// STUB: CARMA2_HW 0x0043b8a0
void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}
// STUB: CARMA2_HW 0x00434010
void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pActor_arg2, br_actor* pActor_arg3, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
// FUNCTION: CARMA2_HW 0x004349c0
int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush = pCar_spec->car_crush_spec;
    tCollision_shape_polyhedron* first_poly;
    tCollision_shape_polyhedron* last_poly;
    tPhysics_shape* original_shape;
    int i;

    if (car_crush == NULL) {
        return 0;
    }
    if (car_crush->count_shapes == 0) {
        return 0;
    }
    if (car_crush->field_0x4 == NULL) {
        return 0;
    }
    if (!car_crush->version_le_100) {
        car_crush->expand_bounding_box = 0;
        return 0;
    }
    first_poly = NULL;
    last_poly = NULL;
    for (i = 0; i < car_crush->count_shapes; i++) {
        tCar_crush_shape_info* shape = &car_crush->field_0x4[i];

        if (!shape->field_0x8) {
            if (car_crush->expand_bounding_box & 0x4) {
                int j;

                for (j = 0; j < shape->count_points; j++) {
                    Crush1ClipPointToPolyhedron(&shape->field_0x18[j].field_0x18, &shape->field_0x0->polyhedron);
                    if (Crush1PointIsOutsideShape(&shape->field_0x18[j].field_0x18, &shape->field_0x0->polyhedron)
                            || Crush1PointIsOutsideLimits(shape, j, &car_crush->field_0xbc)) {
                        BrVector3Copy(&shape->field_0x18[j].field_0x18, &shape->field_0x18[j].field_0xc);
                    }
                    if (DRVector3TestForNan(&shape->field_0x18[j].field_0x18)) {
                        PDEnterDebugger("NaN");
                    }
                }
            }
            if (car_crush->expand_bounding_box & 0x2) {
                int j;

                shape->field_0x4->polyhedron.count_points = shape->count_points;
                for (j = 0; j < shape->count_points; j++) {
                    if (gNet_mode != eNet_mode_none) {
                        Crush1QuantizeVector3(&shape->field_0x18[j].field_0x18);
                    }
                    BrVector3Copy(&shape->field_0x4->polyhedron.points[j], &shape->field_0x18[j].field_0x18);
                }
                FillInShape((tPhysics_shape*)shape->field_0x4);
            }
            if (first_poly == NULL) {
                first_poly = shape->field_0x4;
            }
            if (last_poly != NULL) {
                last_poly->common.next = (tPhysics_shape*)shape->field_0x4;
            }
            last_poly = shape->field_0x4;
        }
    }
    if (first_poly == NULL) {
        return 0;
    }
    original_shape = pCar_spec->collision_info->shape;
    Crush1SwapCarShapes(pCar_spec, (tPhysics_shape*)first_poly);
    if (!(car_crush->expand_bounding_box & 0x4)) {
        if (!Crush1MoveCarToSensiblePlace(pCar_spec)) {
            Crush1SwapCarShapes(pCar_spec, original_shape);
            return 1;
        }
    }
    if (car_crush->expand_bounding_box & 0x8) {
        BrVector3Copy(&pCar_spec->collision_info->cmpos, &car_crush->field_0x10);
    }
    car_crush->expand_bounding_box = 0;
    for (i = 0; i < car_crush->count_shapes; i++) {
        tCar_crush_shape_info* shape = &car_crush->field_0x4[i];

        if (!shape->field_0x8) {
            tCollision_shape_polyhedron* poly;
            int j;

            poly = shape->field_0x0;
            shape->field_0x0 = shape->field_0x4;
            shape->field_0x4 = poly;
            for (j = 0; j < shape->count_points; j++) {
                BrVector3Copy(&shape->field_0x18[j].field_0xc, &shape->field_0x18[j].field_0x18);
            }
        }
    }
    car_crush->field_0x28 = 1;
    SetCollisionInfoChildsDoNothing(pCar_spec->collision_info, 0);
    return 0;
}
// GLOBAL: CARMA2_HW 0x00679450
tToggled_door gToggled_doors[16];

// GLOBAL: CARMA2_HW 0x0067a290
tCrush_net_detach_list_item gNet_crush_detach_list[8];

// GLOBAL: CARMA2_HW 0x0067b7d8
tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b840
tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067bd18
tCar_damge_crush_list_item gCar_damage_crush_list[8];

typedef struct tCrush_net_reattach_list_item_local {
    tCar_spec* actor;
    tCar_spec* car;
    undefined4 field_0x8;
} tCrush_net_reattach_list_item_local;

// GLOBAL: CARMA2_HW 0x0067be10
tCrush_net_reattach_list_item_local gNet_crush_reattach_bit_list[8];

// FUNCTION: CARMA2_HW 0x00429d90
void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec) {
    extern int gCount_net_crush_detach_list;
    extern int gCount_net_crush_semi_detach_bit_list;
    extern int gCount_net_crush_full_detach_bit_list;
    extern int gCount_net_crush_reattach_bit_list;
    extern int gCount_toggled_doors;
    extern int gCount_car_damage_crush_list;
    int i;
    int new_count;

    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_detach_list; i++) {
        if (gNet_crush_detach_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_detach_list[new_count] = gNet_crush_detach_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_detach_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_semi_detach_bit_list; i++) {
        if (gNet_crush_semi_detach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_semi_detach_bit_list[new_count] = gNet_crush_semi_detach_bit_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_semi_detach_bit_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_full_detach_bit_list; i++) {
        if (gNet_crush_full_detach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_full_detach_bit_list[new_count] = gNet_crush_full_detach_bit_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_full_detach_bit_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_reattach_bit_list; i++) {
        if (gNet_crush_reattach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_reattach_bit_list[new_count] = gNet_crush_reattach_bit_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_reattach_bit_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_crush_detach_list; i++) {
        if (gCrush_detach_list[i].car != pCar_spec) {
            if (new_count != i) {
                gCrush_detach_list[new_count] = gCrush_detach_list[i];
            }
            new_count += 1;
        }
    }
    gCount_crush_detach_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_toggled_doors; i++) {
        if (gToggled_doors[i].car != pCar_spec) {
            if (new_count != i) {
                gToggled_doors[new_count] = gToggled_doors[i];
            }
            new_count += 1;
        }
    }
    gCount_toggled_doors = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_car_damage_crush_list; i++) {
        if (gCar_damage_crush_list[i].car != pCar_spec) {
            if (new_count != i) {
                gCar_damage_crush_list[new_count] = gCar_damage_crush_list[i];
            }
            new_count += 1;
        }
    }
    gCount_car_damage_crush_list = new_count;
}
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern tS3_outlet* gCar_outlet;
extern br_vector3 gZero_v__car;
extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

// based on carpocalypse2 src/carma2/common/crush.c:9022
// FUNCTION: CARMA2_HW 0x004387d0
void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap_data;

    user = pActor->user;
    if (user == NULL) {
        return;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        return;
    }
    flap_data = crush_data->flap_data;
    if (flap_data == NULL) {
        return;
    }
    if (flap_data->kev_o_flap) {
        if (gNet_mode != eNet_mode_none) {
            if (gNet_mode == eNet_mode_host) {
                tPhysics_object* object;
                tNet_message* message;

                object = *(tPhysics_object**)((char*)flap_data + 0x24);
                message = NetBuildGuaranteedMessage(0x26, 0);
                message->guaranteed.contents.detach_bit.ID = NetPlayerFromCar(pCar_spec)->ID;
                message->guaranteed.contents.detach_bit.field_0x4 = object->uid;
                message->guaranteed.contents.detach_bit.field_0x8 = gPHIL_last_physics_tick + 120;
                NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
            }
        } else {
            ReAttachBit(pCar_spec, pCar_spec->car_model_actor, pActor, *(tPhysics_object**)((char*)flap_data + 0x24));
            MungeDetachLists(pCar_spec->car_crush_spec);
        }
    } else {
        pCar_spec->car_crush_spec->field_0x134 -= 1;
        BrMatrix34Identity(&pActor->t.t.mat);
        PipeSingleFlap(pActor, -flap_data->field_0x4);
        flap_data->field_0x4 = 0;
        flap_data->field_0x2 = 0;
        flap_data->field_0x6 = 0x3ffc;
        flap_data->field_0x8 = 0x3ffc;
        flap_data->field_0x0 = 0;
    }
    DRS3StartSound3D(gCar_outlet, 0x1482, &pCar_spec->car_master_actor->t.t.translate.t, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on ApexWorm/rec2 d56e70047
// FUNCTION: CARMA2_HW 0x0041fbe0
float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec) {
    float rear;
    float rear_susp;
    float rear_min;
    float front_min;
    float min_value;

    rear = pCar_spec->wpos[0].v[1];
    rear_susp = pCar_spec->susp_height[0];
    rear_min = rear - rear_susp;
    front_min = pCar_spec->wpos[2].v[1] - pCar_spec->susp_height[1];
    rear_min = rear_min * (1.0f / WORLD_SCALE);
    front_min = front_min * (1.0f / WORLD_SCALE);
    min_value = pCar_spec->collision_info->bb1.min.v[1];

    if (rear_min < front_min) {
        if (rear_min < min_value) {
            return rear_min;
        }
    }
    if (front_min < min_value) {
        return front_min;
    }
    return min_value;
}
extern int gTesting_car_for_sensible_place;
extern tPhysics_object* gPHIL_list_collision_infos;
extern tWorld_callbacks gWorld_callbacks;
extern int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);

// based on carpocalypse2 src/carma2/common/car.c:551 (upstream rec2 car.c)
// FUNCTION: CARMA2_HW 0x0041fe50
int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3) {
    int r;

    if (!gProgram_state.racing) {
        return 1;
    }
    gTesting_car_for_sensible_place = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info, gPHIL_list_collision_infos, pVec3, &gWorld_callbacks);
    gTesting_car_for_sensible_place = 0;
    return r;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0042a9b0
void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush) {
    int i;

    pCar_crush->field_0x24 = 0;
    pCar_crush->field_0x26 = 0;

    if (gNet_mode == eNet_mode_host) {
        pCar_crush->network_stuff = BrMemAllocate(sizeof(tCar_crush_network_shapes), kMem_crush_data);
        for (i = 1; i < gNumber_of_net_players; i++) {
            pCar_crush->network_stuff->items[i - 1].network_id = gNet_players[i].ID;
            pCar_crush->network_stuff->items[i - 1].field_0x4 = 0;
        }
    }
}

// based on carpocalypse2 src/carma2/common/crush1.c:602
// FUNCTION: CARMA2_HW 0x0042d770
void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue) {
    int i;

    pCrush->count = pCount_vectors;
    for (i = 0; i < pCount_vectors; i++) {
        BrVector3Copy(&pCrush->vectors[i], &pVectors[i]);
    }
    BrVector3Copy(&pCrush->field_0x34, pVector1);
    BrVector3Copy(&pCrush->field_0x44, pVector2);
    pCrush->field_0x40 = pScalar;
    pCrush->field_0x50 = pValue;
}

// FUNCTION: CARMA2_HW 0x0043d9c0
void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar) {
    int i;
    int j;
    int count;
    tCar_crush_shape_info* shape_info;
    tPhysics_shape* shape;
    br_vector3* point;

    C2_HOOK_BUG_ON(sizeof(tCompressed_vector3) != 6);

    if (pCar->car_crush_spec->field_0x4b8) {
        count = 0;
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                count++;
            }
        }
        *pData++ = (tU8)count;
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                *pData = (tU8)shape->polyhedron.polyhedron.count_points;
                pData++;
                for (j = 0; j < shape->polyhedron.polyhedron.count_points; j++) {
                    point = shape->polyhedron.polyhedron.points;
                    point += j;
                    CompressVector3((tCompressed_vector3*)pData, point, -2.5f, 2.5f);
                    pData += sizeof(tCompressed_vector3);
                }
            }
        }
    } else {
        count = 0;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            count += pCar->car_crush_spec->field_0x4[i].count_points;
        }
        *pData++ = (tU8)count;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            shape_info = &pCar->car_crush_spec->field_0x4[i];
            for (j = 0; j < shape_info->count_points; j++) {
                CompressVector3((tCompressed_vector3*)pData, &shape_info->field_0x18[j].field_0xc, -2.5f, 2.5f);
                pData += sizeof(tCompressed_vector3);
            }
        }
    }
}

// GLOBAL: CARMA2_HW 0x0065cf78
tWorld_callbacks gWorld_callbacks = { 0 };

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00429070
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

typedef struct {
    int ID;
    tU8 field_0x4;
    undefined field_0x5[0x8 - 0x5];
    tU32 field_0x8;
    tCompressed_vector3 field_0xc;
    tCompressed_vector3 field_0x12;
    tCompressed_vector3 field_0x18;
    tCompressed_vector3 field_0x1e;
    tCompressed_vector3 field_0x24;
} tNet_message_chunk_semi_detach_bit;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004314b0
void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    tNet_message* message;
    tNet_message_chunk_semi_detach_bit* chunk;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bnds;

    message = NetBuildGuaranteedMessage(0x25, 0);
    chunk = (tNet_message_chunk_semi_detach_bit*)&message->guaranteed.contents.detach_bit;
    chunk->ID = NetPlayerFromCar(pCar)->ID;
    chunk->field_0x4 = pObject->uid;
    chunk->field_0x8 = gPHIL_last_physics_tick + 120;
    if (!GetSDBJointPosAndBounds(&p1, &p2, &p3, &bnds, pObject->actor)) {
        CompressVector3(&chunk->field_0xc, &p1, -10.f, 10.f);
        CompressVector3(&chunk->field_0x12, &p2, -10.f, 10.f);
        CompressVector3(&chunk->field_0x18, &p3, -10.f, 10.f);
        CompressVector3(&chunk->field_0x1e, &bnds.min, -10.f, 10.f);
        CompressVector3(&chunk->field_0x24, &bnds.max, -10.f, 10.f);
        NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
    }
}

extern int gCrush_deferred;
extern int gNum_active_non_cars;
extern tNon_car_spec* gActive_non_car_list[99];
extern int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);
extern void C2_HOOK_FASTCALL PHILDetachObjectState(tPhysics_object* pObject);

// based on carpocalypse2 src/carma2/common/flap.c
// FUNCTION: CARMA2_HW 0x0042f4e0
void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    void* bit;
    void* b2;
    void* b3;
    void* state;

    bit = pObject->actor;
    if (bit == NULL) {
        return;
    }
    b2 = *(void**)((tU8*)bit + 0x60);
    if (b2 == NULL) {
        return;
    }
    b3 = *(void**)((tU8*)b2 + 0x8);
    if (b3 == NULL) {
        return;
    }
    state = *(void**)((tU8*)b3 + 0x34);
    if (state == NULL) {
        return;
    }
    if (*(int*)((tU8*)state + 0x28) != 1) {
        return;
    }

    PHILRemoveFromParentChain(pObject);
    if (pObject->physics_joint1 != NULL) {
        *(int*)pObject->physics_joint1 = 0;
        pObject->physics_joint1 = NULL;
    }
    if (gCrush_deferred != 0) {
        PHILDetachObjectState(pCar->collision_info);
    }
    pObject->flags |= 0x100;

    b2 = *(void**)((tU8*)bit + 0x5c);
    if (gNum_active_non_cars != 0x63) {
        *(int*)((tU8*)b2 + 0xc) = gDetached_bit_driver;
        gActive_non_car_list[gNum_active_non_cars] = (tNon_car_spec*)b2;
        gNum_active_non_cars += 1;
        PHILAddActiveObjectImmediately(*(void**)((tU8*)b2 + 0x8), NULL, NULL, NULL);
    }
    PipeSingleNonCar(*(tU16*)((tU8*)b2 + 0x80), bit);
    *(tU8*)((tU8*)(*(void**)((tU8*)bit + 0x14)) + 0x3) = 0x21;
    *(int*)((tU8*)state + 0x28) = 2;
    pCar->use_shell_model = 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004b9750
void C2_HOOK_FASTCALL PHILDetachObjectState(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0067bad8
tQueued_drone_crush gQueued_drone_crushes[4];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004a5bc0
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
