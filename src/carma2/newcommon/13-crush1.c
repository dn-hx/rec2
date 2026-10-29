#include "13-crush1.h"

#include "08-loading1.h"
#include "09-funks.h"
#include "23-piping.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"
#include "globvrpb.h"
#include "04-netgame.h"
#include "globvars.h"

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

// InitNetworkShapesStuff

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

// CrushLimitNumber

// SoftnessOfNearestPointCB

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

// SoftnessOfNearestPoint

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0042ad10
void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

// SetUpSemiDetachJointStuff

// STUB: CARMA2_HW 0x0042b7f0
intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data) {
    NOT_IMPLEMENTED();
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

// PointEdgeDistSq

// SetFlapCheckVertices

// InitModelVertexData

// STUB: CARMA2_HW 0x0042bf80
intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data) {
    NOT_IMPLEMENTED();
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

// STUB: CARMA2_HW 0x0042c8b0
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    NOT_IMPLEMENTED();
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

// QueueDroneCrush

// DoCrushing

// SetCrush

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

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

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
// SendReAttachBit

// StopFlapping

// DoFlapping

// ToggleFlapsCB2

extern int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data);

// FUNCTION: CARMA2_HW 0x0042ddb0
int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data) {
    tCar_spec* pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleDoorsActorCallback, pCar_spec);
    return 0;
}

// STUB: CARMA2_HW 0x0042ddc0
int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data) {
    NOT_IMPLEMENTED();
}

// SendToggleFlaps

// ToggleFlaps

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

// FullyDetachBit

// SendFullyDetachBit

// DoFullyDetaching

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

// QuantizeWithinBounds

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

// QuantizePlane

// WithinBounds

// SplitCheck

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

// BendCarOneForce

// DoBending

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

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

// SphericizeCar

// STUB: CARMA2_HW 0x004398d0
// STUB: CARMA2_SW 0x00439910
intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser) {
    NOT_IMPLEMENTED();
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

// CompletelyUnBendWheels

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
// TotallyRepairModel

// STUB: CARMA2_HW 0x00439a20
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00439a10
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}

// SendReAttachAllBits

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00439510
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
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

// EncodeNetShapeData

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

