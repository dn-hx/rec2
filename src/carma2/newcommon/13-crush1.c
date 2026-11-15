#define REC2_NO_CRUSH_DECLS

#define REC2_NO_SMASH_DECLS

/* This TU's x87 codegen is sensitive to the count of shared-header declarations
 * (DRActorEnumRecurseWithTranslation 0x42b760); the net-message types are unused here. */
#define REC2_NO_NET_MESSAGE_TYPEDEFS

#include "13-crush1.h"
#include "57-drone.h"

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
#include "56-aiworld.h"
#include "58-crush2.h"
#include "60-phil.h"
#include "globvars.h"

#include <ctype.h>
#include "07-structur.h"
#include "15-displays.h"
#include "22-replay.h"
#include "27-powerup.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "69-sound.h"
#include "globvrkm.h"
#include "00-car.h"
#include "01-network.h"

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

// GLOBAL: CARMA2_HW 0x00589a2c
br_scalar gFLOAT_00589a2c = 0.5f;

// GLOBAL: CARMA2_HW 0x00589a40
br_scalar gCrushZero = 0.0f;

// GLOBAL: CARMA2_HW 0x00589a5c
br_scalar gFLT_EPSILON_00589a5c = 2.3841858e-07f;
// GLOBAL: CARMA2_HW 0x00589a60
double gDouble_00589a60 = 1.0;
// (same value as gFLOAT_00589a54 below; not annotated)
br_scalar gFLOAT_1_0_00589a54 = 1.0f;
// GLOBAL: CARMA2_HW 0x00589b2c
br_scalar gFLOAT_NEG1_0_00589b2c = -1.0f;
// GLOBAL: CARMA2_HW 0x00589a58
br_scalar gFLOAT_0_01_00589a58 = 0.01f;
// GLOBAL: CARMA2_HW 0x00589b30
br_scalar gFLOAT_0_015_00589b30 = 0.015f;

// GLOBAL: CARMA2_HW 0x0067bad0
int gCount_crush_detach_list;

// GLOBAL: CARMA2_HW 0x00679558
tCrush_detach_list_item gCrush_detach_list[16];

// GLOBAL: CARMA2_HW 0x0067be84
tU32 gINT_0067be84;

// GLOBAL: CARMA2_HW 0x0067be88
static tU32 last_flap_bits;


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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
extern tWorld_callbacks gWorld_callbacks;

// (upstream rec2 car.c)
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
// GLOBAL: CARMA2_HW 0x00679430
int gCollision_face_mask__raycast;
// GLOBAL: CARMA2_HW 0x00679438
int gSensible_place_collect_active;
extern tPolyhedron_edge_indexes gBox_edge_indexes[12];
// FUNCTION: CARMA2_HW 0x004228b0
int C2_HOOK_FASTCALL GetObjectWallForceInfo(tPhysics_object* pObject, tSensible_place_contact* pContacts, int pArg) {
    struct {
        br_vector3 points[128];
        int point_flags[64];
        tU16 point_count;
        tU16 edge_count;
        tPolyhedron_edge_indexes* edges;
    } data;
    br_matrix34 object_to_actor_matrix;
    br_matrix34 inverse_actor_matrix;
    br_vector3 actor_point;
    br_vector3 world_point;
    int result_index;
    tFace_ref* pClosest_face;
    br_vector3 closest_point;
    br_vector3 local_point;
    br_scalar distance;
    br_vector3 normal;
    int has_collision;
    int has_any_collision;
    br_scalar closest_distance;
    int tmp_count;
    br_matrix34* pTransform;
    int collision_count;
    int* pFlags;
    int i;
    tPhysics_shape* pShape;
    br_matrix34* pShape_transform;
    br_scalar neg_radius;
    tFace_ref* pFace;
    int face_index;
    tU32 three;

    three = 3;
    collision_count = 0;
    has_any_collision = 0;
    pShape = pObject->shape;
    result_index = 1;
    if (pObject->box_face_start == pObject->box_face_end) {
        return 0;
    }
    if (pArg > 0x24) {
        return 0;
    }
    BrMatrix34LPInverse(&inverse_actor_matrix, &pObject->actor->t.t.mat);
    pTransform = &pObject->transform_matrix;
    BrMatrix34Mul(&object_to_actor_matrix, pTransform, &inverse_actor_matrix);
    for (; pShape != NULL; pShape = pShape->common.next) {
        if (!EliminateFaces(pObject, (int*)pShape, &object_to_actor_matrix)) {
            continue;
        }
        if (gSensible_place_collect_active != 0) {
            pShape_transform = pTransform;
        } else {
            pShape_transform = &pObject->actor->t.t.mat;
        }
        switch (pShape->common.type) {
        case kCollisionShapeType_Box:
            data.point_count = 8;
            data.edge_count = 12;
            for (i = 0, pFlags = data.point_flags; i < 8; i++, pFlags++) {
                if ((i & 2) != 0) {
                    local_point.v[0] = pShape->box.common.bb.min.v[0];
                } else {
                    local_point.v[0] = pShape->box.common.bb.max.v[0];
                }
                if ((i & 1) != 0) {
                    local_point.v[1] = pShape->box.common.bb.min.v[1];
                } else {
                    local_point.v[1] = pShape->box.common.bb.max.v[1];
                }
                if ((i & 4) != 0) {
                    local_point.v[2] = pShape->box.common.bb.min.v[2];
                } else {
                    local_point.v[2] = pShape->box.common.bb.max.v[2];
                }
                BrMatrix34ApplyP(&data.points[i], &local_point, pTransform);
                BrMatrix34ApplyP(&data.points[i + 64], &local_point, pShape_transform);
                *pFlags = 0;
            }
            data.edges = gBox_edge_indexes;
            break;
        case kCollisionShapeType_Wireframe:
            data.point_count = (tU16)pShape->wireframe.wireframe.count_points;
            data.edge_count = (tU16)pShape->wireframe.wireframe.count_lines;
            if (data.point_count != 0) {
                for (i = 0, pFlags = data.point_flags; i < (int)data.point_count; i++, pFlags++) {
                    BrMatrix34ApplyP(&data.points[i], &pShape->wireframe.wireframe.points[i], pTransform);
                    BrMatrix34ApplyP(&data.points[i + 64], &pShape->wireframe.wireframe.points[i], pShape_transform);
                    *pFlags = 0;
                }
            }
            data.edges = pShape->wireframe.wireframe.lines;
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            data.point_count = (tU16)pShape->polyhedron.polyhedron.count_points;
            data.edge_count = (tU16)pShape->polyhedron.polyhedron.count_edges;
            if (data.point_count != 0) {
                for (i = 0, pFlags = data.point_flags; i < (int)data.point_count; i++, pFlags++) {
                    BrMatrix34ApplyP(&data.points[i], &pShape->polyhedron.polyhedron.points[i], pTransform);
                    BrMatrix34ApplyP(&data.points[i + 64], &pShape->polyhedron.polyhedron.points[i], pShape_transform);
                    *pFlags = 0;
                }
            }
            data.edges = pShape->polyhedron.polyhedron.edges;
            break;
        case kCollisionShapeType_Sphere:
            data.point_count = 1;
            data.edge_count = 0;
            BrMatrix34ApplyP(&data.points[0], &pShape->sphere.sphere.center, pTransform);
            BrMatrix34ApplyP(&data.points[64], &pShape->sphere.sphere.center, pShape_transform);
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, "");
            break;
        }
        if (gSensible_place_collect_active != 0) {
            BrMatrix34ApplyP(&data.points[0], &pObject->cmpos, pTransform);
            for (i = 1; i < (int)data.point_count; i++) {
                data.points[i] = data.points[0];
            }
        }
        if (pShape->common.type == kCollisionShapeType_Sphere) {
            tmp_count = collision_count;
            closest_distance = 2.0f;
            for (face_index = pObject->box_face_start, pFace = &gFace_list__car[face_index]; face_index < (int)pObject->box_face_end; face_index++, pFace++) {
                SphereFaceCollide(pFace, &data.points[0], &data.points[64], pShape->sphere.sphere.radius, &normal, &closest_point, &distance);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    pClosest_face = pFace;
                    world_point = closest_point;
                }
            }
            if (closest_distance <= 1.0f) {
                BrMatrix34TApplyV(&closest_point, &world_point, pTransform);
                BrMatrix34TApplyV(&actor_point, &world_point, &pObject->actor->t.t.mat);
                neg_radius = -pShape->sphere.sphere.radius;
                normal.v[0] = pShape->sphere.sphere.center.v[0] + actor_point.v[0] * neg_radius;
                normal.v[1] = pShape->sphere.sphere.center.v[1] + actor_point.v[1] * neg_radius;
                normal.v[2] = pShape->sphere.sphere.center.v[2] + actor_point.v[2] * neg_radius;
                /* pPoint is the sphere surface point pulled towards the wall (our "normal"),
                 * pNormal is the face normal returned by SphereFaceCollide (our "closest_point"). */
                tmp_count = AddObjectWallForce((float*)&pContacts[pArg], &normal, closest_distance, &closest_point, 0, collision_count, pObject, *(float*)&three, *(float*)&pClosest_face, NULL);
            }
            i = GetSphereToEdgePoints(pObject, (int)&pShape->sphere.sphere.radius, (int)&pContacts[pArg], tmp_count, 0x28 - pArg, (int)&data.points[0], (int)&data.points[64], (int)&object_to_actor_matrix);
            collision_count += (collision_count < i);
        } else {
            has_collision = 0;
            if (pObject->field_0xed != 0) {
                collision_count = GetFaceEdgeIntersection(pObject, pShape, &pContacts[pArg], collision_count, 0x28 - pArg, &data.points[0], &object_to_actor_matrix, &has_collision);
            }
            has_any_collision |= has_collision;
            collision_count = GetCornerToFacePoints(pObject, (int)pShape, (tU32)(int)&pContacts[pArg], collision_count, 0x28 - pArg, (char*)&data.points[0], (tU32)(int)&object_to_actor_matrix);
            if (has_collision == 0) {
                collision_count = GetEdgeToEdgePoints(pObject, pShape, (tPhysics_shape*)&pContacts[pArg], collision_count, 0x28 - pArg, (tU8*)&data.points[0], (undefined4)&object_to_actor_matrix);
            }
        }
    }
    pObject->field_0xed = (tU8)has_any_collision;
    if (collision_count == 0) {
        return 0;
    }
    for (; result_index < collision_count; result_index++) {
        if (pContacts[pArg].normal.v[0] * pContacts[pArg + result_index].normal.v[0]
                + pContacts[pArg].normal.v[1] * pContacts[pArg + result_index].normal.v[1]
                + pContacts[pArg].normal.v[2] * pContacts[pArg + result_index].normal.v[2] <= 0.95) {
            break;
        }
    }
    pObject->omega.v[0] = pObject->rotate_omega.v[0];
    pObject->omega.v[1] = pObject->rotate_omega.v[1];
    pObject->omega.v[2] = pObject->rotate_omega.v[2];
    return result_index;
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
// FUNCTION: CARMA2_HW 0x004236a0
int C2_HOOK_FASTCALL GetFaceEdgeIntersection(tPhysics_object* pObject, tPhysics_shape* pShape, tSensible_place_contact* pContacts, int pCount, int pMax, br_vector3* pPoints, br_matrix34* pScratch, int* pHas_collision) {
    float local_90;
    float local_8c;
    float local_88;
    float local_84;
    float local_80;
    float local_7c;
    float local_78;
    int local_74;
    float local_70;
    float local_6c;
    float local_68;
    int local_64;
    float local_60;
    float local_5c;
    float local_58;
    float local_54;
    unsigned int local_50;
    float local_4c;
    float local_48;
    float local_44;
    unsigned char local_40[4];
    float local_3c;
    float local_38;
    float local_34;
    float local_30;
    float local_2c;
    float local_28;
    float local_24;
    float local_20;
    float local_1c;
    float local_18;
    float local_14;
    float local_10;
    unsigned char local_c[12];
    unsigned char* vertex_data;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    int iVar7;
    unsigned int uVar8;
    br_scalar ray_t;
    br_vector3 ray_hit;

    (void)pContacts;
    (void)pMax;
    (void)pScratch;
    (void)local_14;
    (void)local_10;
    (void)ray_t;
    (void)local_3c;
    (void)local_38;
    (void)local_34;
    (void)local_30;
    (void)local_2c;
    (void)local_28;

    vertex_data = (unsigned char*)pPoints;

    if (*(int*)pShape != 4) {
        BrMatrix34ApplyP((br_vector3*)&local_70, (br_vector3*)((char*)pObject + 0x14), (br_matrix34*)((char*)pObject + 0x8c));
        local_74 = 0;
        if (*(short*)(vertex_data + 0x702) != 0) {
            do {
                local_50 = (unsigned int)*(unsigned char*)( *(int*)(vertex_data + 0x704) + local_74 * 2 );
                uVar8 = (unsigned int)*(unsigned char*)( *(int*)(vertex_data + 0x704) + local_74 * 2 + 1 );
                local_30 = *(float*)(vertex_data + uVar8 * 0xc) - *(float*)(vertex_data + local_50 * 0xc);
                local_2c = *(float*)(vertex_data + uVar8 * 0xc + 4) - *(float*)(vertex_data + local_50 * 0xc + 4);
                local_28 = *(float*)(vertex_data + uVar8 * 0xc + 8) - *(float*)(vertex_data + local_50 * 0xc + 8);
                local_64 = (int)RayCastThruFaceList((br_vector3*)(vertex_data + local_50 * 0xc), (br_vector3*)&local_84, (br_vector3*)&local_90,
                            (br_scalar*)local_40, &ray_hit, &gFace_list__car[*(unsigned short*)((char*)pObject + 0x174)],
                            (unsigned int)*(unsigned short*)((char*)pObject + 0x176) -
                            (unsigned int)*(unsigned short*)((char*)pObject + 0x174));
                local_3c = *(float*)(vertex_data + local_50 * 0xc) - *(float*)(vertex_data + uVar8 * 0xc);
                local_38 = *(float*)(vertex_data + local_50 * 0xc + 4) - *(float*)(vertex_data + uVar8 * 0xc + 4);
                local_34 = *(float*)(vertex_data + local_50 * 0xc + 8) - *(float*)(vertex_data + uVar8 * 0xc + 8);
                if (local_64 == 0) {
                    local_64 = (int)RayCastThruFaceList((br_vector3*)(vertex_data + local_50 * 0xc), (br_vector3*)&local_84, (br_vector3*)&local_90,
                                (br_scalar*)local_40, &ray_hit, &gFace_list__car[*(unsigned short*)((char*)pObject + 0x174)],
                                (unsigned int)*(unsigned short*)((char*)pObject + 0x176) -
                                (unsigned int)*(unsigned short*)((char*)pObject + 0x174));
                    if (local_64 != 0) {
                        fVar5 = local_70 - local_90;
                        fVar3 = local_6c - local_8c;
                        fVar6 = local_68 - local_88;
                        fVar4 = (float)sqrt(fVar6 * fVar6 + fVar3 * fVar3 + fVar5 * fVar5);
                        if (fVar4 <= 2.3841858e-07f) {
                            fVar5 = 1.0f;
                            fVar3 = 0.0f;
                            fVar4 = 0.0f;
                        }
                        else {
                            fVar4 = 1.0f / fVar4;
                            fVar5 = fVar4 * fVar5;
                            fVar3 = fVar4 * fVar3;
                            fVar4 = fVar4 * fVar6;
                        }
                        if (0.01f <= fVar4 * local_7c + fVar3 * local_80 + fVar5 * local_84) {
                            *(int*)(vertex_data + 0x600 + local_50 * 4) = 1;
                            goto LAB_00423aa6;
                        }
                    }
                }
                else {
                    fVar5 = local_70 - local_90;
                    fVar3 = local_6c - local_8c;
                    fVar6 = local_68 - local_88;
                    fVar4 = (float)sqrt(fVar6 * fVar6 + fVar3 * fVar3 + fVar5 * fVar5);
                    if (fVar4 <= 2.3841858e-07f) {
                        fVar5 = 1.0f;
                        fVar3 = 0.0f;
                        fVar4 = 0.0f;
                    }
                    else {
                        fVar4 = 1.0f / fVar4;
                        fVar5 = fVar4 * fVar5;
                        fVar3 = fVar4 * fVar3;
                        fVar4 = fVar4 * fVar6;
                    }
                    local_78 = fVar4 * local_7c + fVar3 * local_80 + fVar5 * local_84;
                    iVar7 = (int)RayCastThruFaceList((br_vector3*)&local_5c, (br_vector3*)&local_60, (br_vector3*)&local_4c,
                              (br_scalar*)local_40, &ray_hit, &gFace_list__car[*(unsigned short*)((char*)pObject + 0x174)],
                              (unsigned int)*(unsigned short*)((char*)pObject + 0x176) -
                              (unsigned int)*(unsigned short*)((char*)pObject + 0x174));
                    if (iVar7 == 0) {
                        if (0.01f < local_78) {
                            *(int*)(vertex_data + 0x600 + uVar8 * 4) = 1;
                            goto LAB_00423aa6;
                        }
                    }
                    else {
                        fVar5 = local_70 - local_4c;
                        fVar3 = local_6c - local_48;
                        fVar6 = local_68 - local_44;
                        fVar4 = (float)sqrt(fVar6 * fVar6 + fVar3 * fVar3 + fVar5 * fVar5);
                        if (fVar4 <= 2.3841858e-07f) {
                            fVar5 = 1.0f;
                            fVar3 = 0.0f;
                            fVar4 = 0.0f;
                        }
                        else {
                            fVar4 = 1.0f / fVar4;
                            fVar5 = fVar4 * fVar5;
                            fVar3 = fVar4 * fVar3;
                            fVar4 = fVar4 * fVar6;
                        }
                        fVar3 = fVar4 * local_54 + fVar3 * local_58 + fVar5 * local_5c;
                        if ((0.05f <= local_78) || (0.05f <= fVar3)) {
                            if (fVar3 <= local_78) {
                                *(int*)(vertex_data + 0x600 + uVar8 * 4) = 1;
                            }
                            else {
                                local_90 = local_4c;
                                local_8c = local_48;
                                local_88 = local_44;
                                local_84 = local_5c;
                                local_80 = local_58;
                                local_7c = local_54;
                                *(int*)(vertex_data + 0x600 + local_50 * 4) = 1;
                            }
LAB_00423aa6:
                            local_18 = local_90 - *(float*)((char*)pObject + 0xb0);
                            local_14 = local_8c - *(float*)((char*)pObject + 0xb4);
                            local_10 = local_88 - *(float*)((char*)pObject + 0xb8);
                            BrMatrix34TApplyV((br_vector3*)local_c, (br_vector3*)&local_18, (br_matrix34*)((char*)pObject + 0x8c));
                            BrMatrix34ApplyP((br_vector3*)&local_24, (br_vector3*)local_c, (br_matrix34*)((char*)pObject->actor + 0x2c));
                            local_60 = (local_88 - local_1c) * local_7c +
                                     (local_8c - local_20) * local_80 + (local_90 - local_24) * local_84;
                            if (0.0f < local_60) {
                                BrMatrix34TApplyV((br_vector3*)&local_5c, (br_vector3*)&local_84, (br_matrix34*)((char*)pObject + 0x8c));
                                pCount = AddObjectWallForce(0, (br_vector3*)&local_5c, local_60, (br_vector3*)((char*)pObject + 0xbc), 0, pCount, pObject, 0.0f, 0.0f, NULL);
                            }
                            *pHas_collision = 1;
                        }
                    }
                }
                local_74 = local_74 + 1;
            } while (local_74 < (int)(unsigned int)*(unsigned short*)(vertex_data + 0x702));
        }
    }
    return pCount;
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
// FUNCTION: CARMA2_HW 0x00424140
int C2_HOOK_FASTCALL GetEdgeToEdgePoints(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7) {
    br_vector3 actor_translation;
    int face_index;
    tFace_ref* face;

    (void)pRay_shape;
    (void)pArg5;
    (void)pArg7;
    gCollision_face_mask__raycast = 0;
    BrMatrix34TApplyV(&actor_translation, (const br_vector3*)&pObject->actor->t.t.mat.m[3], &pObject->actor->t.t.mat);
    pBounds_shape->common.field_0x1c.min.v[0] += actor_translation.v[0];
    pBounds_shape->common.field_0x1c.min.v[1] += actor_translation.v[1];
    pBounds_shape->common.field_0x1c.min.v[2] += actor_translation.v[2];
    pBounds_shape->common.field_0x1c.max.v[0] += actor_translation.v[0];
    pBounds_shape->common.field_0x1c.max.v[1] += actor_translation.v[1];
    pBounds_shape->common.field_0x1c.max.v[2] += actor_translation.v[2];

    face_index = (int)pObject->box_face_start;
    face = &gFace_list__car[face_index];
    if (face_index < pObject->box_face_end) {
        do {
            if ((face->flags & 0x80) == 0) {
                if ((face->flags & 1) == 0) {
                    pCollision_count = CheckSingleEdgeWithObjectEdges((float*)&face->v[0], (float*)&face->v[1], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
                if ((face->flags & 2) == 0) {
                    pCollision_count = CheckSingleEdgeWithObjectEdges((float*)&face->v[1], (float*)&face->v[2], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
                if ((face->flags & 4) == 0) {
                    pCollision_count = CheckSingleEdgeWithObjectEdges((float*)&face->v[2], (float*)&face->v[0], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
            }
            face_index++;
            face++;
        } while (face_index < pObject->box_face_end);
    }
    return pCollision_count;
}

typedef int (C2_HOOK_FASTCALL * tEdge_face_wall_force_fn)(float* pWalls, const br_vector3* pPoint, float pDistance, const br_vector3* pNormal, tU32 pUnused, int pCount, tPhysics_object* pObject, int pType, tFace_ref* pFace, const float* pVector);

// Sweeps one end point of the edge from the object's transform_matrix frame to its actor frame against pShape
// and adds a wall contact (type 2) for the hit. Inlined twice into CheckSingleEdgeWithObjectEdges.
static __inline int EdgeFaceSweepPoint(float* pPoint, tPhysics_object* pObject, tPhysics_shape* pShape, float* pWalls, int pCount, tFace_ref* pFace) {
    br_vector3 tmp_object;
    br_vector3 tmp_actor;
    br_vector3 object_point;
    br_vector3 actor_point;
    br_vector3 point;
    br_vector3 normal;
    br_scalar t;
    int hit_count;
    tEdge_face_wall_force_fn add_wall_force;

    add_wall_force = (tEdge_face_wall_force_fn)(void (*)(void))AddObjectWallForce;
    tmp_object.v[0] = pPoint[0] - pObject->transform_matrix.m[3][0];
    tmp_object.v[1] = pPoint[1] - pObject->transform_matrix.m[3][1];
    tmp_object.v[2] = pPoint[2] - pObject->transform_matrix.m[3][2];
    BrMatrix34TApplyV(&object_point, &tmp_object, &pObject->transform_matrix);
    tmp_actor.v[0] = pPoint[0] - pObject->actor->t.t.mat.m[3][0];
    tmp_actor.v[1] = pPoint[1] - pObject->actor->t.t.mat.m[3][1];
    tmp_actor.v[2] = pPoint[2] - pObject->actor->t.t.mat.m[3][2];
    BrMatrix34TApplyV(&actor_point, &tmp_actor, &pObject->actor->t.t.mat);
    hit_count = ShapeRayCastWithSphere(&object_point, &actor_point, pShape, &point, &normal, &t);
    if (hit_count > 0) {
        if (hit_count > 0x40) {
            t = 1.1f;
        }
        pCount = add_wall_force(pWalls, &point, t, &normal, 0, pCount, pObject, 2, pFace, NULL);
    }
    return pCount;
}

// PARTIAL 41.05%: 0x00424270 - logic follows the original; frame 0x184 vs 0x170 and register roles (pObject in ebx, axis/edge counters) differ (keep)
// Edge (pStart -> pEnd) of a car face against the bounds shape: sweeps both end points from the object's
// transform_matrix frame to the actor frame, then tests the edge against every edge of the swept polyhedron.
// FUNCTION: CARMA2_HW 0x00424270
int C2_HOOK_FASTCALL CheckSingleEdgeWithObjectEdges(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data) {
    typedef struct {
        br_scalar t;
        tPhysics_object* object;
        br_scalar field_0x8;
        tFace_ref* face;
        br_scalar field_0x10;
        br_vector3 point;
        br_vector3 vector;
        br_vector3 normal;
        br_vector3 field_0x38;
        undefined field_0x44[0x1c];
    } tEdge_face_hit;
    typedef struct {
        br_vector3 points[128];
        int point_flags[64];
        tU16 point_count;
        tU16 edge_count;
        tPolyhedron_edge_indexes* edges;
    } tEdge_face_polyhedron;
    extern int C2_HOOK_FASTCALL CheckTwoEdges(br_vector3* pPoint, br_vector3* pDiff, const br_vector3* pVertices, br_vector3* pDelta, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, void* pUnused, tPhysics_object* pObject1, tPhysics_object* pObject2);
    br_matrix34* matrix;
    tEdge_face_polyhedron* polyhedron;
    int axis;
    int start_flag;
    int end_flag;
    float start_dot;
    float end_dot;
    float low;
    float high;
    int i;
    int cur;
    int best_edge;
    int index_a;
    int index_b;
    br_vector3 tmp_actor;
    br_vector3 edge;
    br_vector3 delta;
    br_vector3 direction;
    br_vector3 twist;
    br_vector3 cast_start;
    br_vector3 cast_end;
    br_vector3 cast_pos;
    float factor;
    tEdge_face_hit hits[2];
    tEdge_face_wall_force_fn add_wall_force;

    add_wall_force = (tEdge_face_wall_force_fn)(void (*)(void))AddObjectWallForce;
    start_flag = 1;
    end_flag = 1;
    matrix = &((tPhysics_object*)pObject)->actor->t.t.mat;
    for (axis = 0; axis < 3; axis++) {
        start_dot = pStart[1] * matrix->m[axis][1] + pStart[2] * matrix->m[axis][2] + matrix->m[axis][0] * pStart[0];
        low = ((tPhysics_shape*)pBounds_shape)->common.field_0x1c.min.v[axis] - 0.005;
        end_dot = pEnd[1] * matrix->m[axis][1] + pEnd[2] * matrix->m[axis][2] + matrix->m[axis][0] * pEnd[0];
        start_flag |= start_dot < low;
        end_flag |= end_dot < low;
        if (start_dot < low && end_dot < low) {
            return pCollision_count;
        }
        high = ((tPhysics_shape*)pBounds_shape)->common.field_0x1c.max.v[axis] - -0.005;
        start_flag |= start_dot < high;
        end_flag |= end_dot < high;
        if (start_dot > high && end_dot > high) {
            return pCollision_count;
        }
    }
    if (((tPhysics_object*)pObject)->field_0xed == 0) {
        if (start_flag) {
            pCollision_count = EdgeFaceSweepPoint(pStart, (tPhysics_object*)pObject, (tPhysics_shape*)pBounds_shape, (float*)pRay_shape, pCollision_count, (tFace_ref*)pFace);
        }
        if (end_flag) {
            pCollision_count = EdgeFaceSweepPoint(pEnd, (tPhysics_object*)pObject, (tPhysics_shape*)pBounds_shape, (float*)pRay_shape, pCollision_count, (tFace_ref*)pFace);
        }
    }
    edge.v[0] = pEnd[0] - pStart[0];
    edge.v[1] = pEnd[1] - pStart[1];
    edge.v[2] = pEnd[2] - pStart[2];
    polyhedron = (tEdge_face_polyhedron*)pPolyhedron_data;
    cur = 0;
    hits[0].face = (tFace_ref*)pFace;
    hits[1].face = (tFace_ref*)pFace;
    hits[0].t = 2.0f;
    best_edge = -1;
    hits[1].t = 2.0f;
    for (i = 0; i < polyhedron->edge_count; i++) {
        index_a = polyhedron->edges[i].index1;
        index_b = polyhedron->edges[i].index2;
        direction.v[0] = polyhedron->points[index_a + 64].v[0] - polyhedron->points[index_a].v[0];
        direction.v[1] = polyhedron->points[index_a + 64].v[1] - polyhedron->points[index_a].v[1];
        direction.v[2] = polyhedron->points[index_a + 64].v[2] - polyhedron->points[index_a].v[2];
        delta.v[0] = polyhedron->points[index_b].v[0] - polyhedron->points[index_a].v[0];
        delta.v[1] = polyhedron->points[index_b].v[1] - polyhedron->points[index_a].v[1];
        delta.v[2] = polyhedron->points[index_b].v[2] - polyhedron->points[index_a].v[2];
        twist.v[0] = polyhedron->points[index_b + 64].v[0] - polyhedron->points[index_a + 64].v[0];
        twist.v[1] = polyhedron->points[index_b + 64].v[1] - polyhedron->points[index_a + 64].v[1];
        twist.v[2] = polyhedron->points[index_b + 64].v[2] - polyhedron->points[index_a + 64].v[2];
        twist.v[0] = twist.v[0] - delta.v[0];
        twist.v[1] = twist.v[1] - delta.v[1];
        twist.v[2] = twist.v[2] - delta.v[2];
        if (CheckTwoEdges((br_vector3*)pStart, &edge, &polyhedron->points[index_a], &delta, &direction, &twist, &hits[cur].t, (void*)pCollision_count, (tPhysics_object*)pObject, NULL) != 0) {
            best_edge = i;
            if (hits[cur].t < hits[cur == 0].t) {
                cur = cur == 0;
            }
        }
    }
    if (hits[0].t >= 2.0f) {
        return pCollision_count;
    }
    if (((tFace_ref*)pFace)->normal.v[0] * hits[0].field_0x38.v[0] + ((tFace_ref*)pFace)->normal.v[1] * hits[0].field_0x38.v[1] + ((tFace_ref*)pFace)->normal.v[2] * hits[0].field_0x38.v[2] < 0.0f) {
        tmp_actor.v[0] = pStart[0] - ((tPhysics_object*)pObject)->transform_matrix.m[3][0];
        tmp_actor.v[1] = pStart[1] - ((tPhysics_object*)pObject)->transform_matrix.m[3][1];
        tmp_actor.v[2] = pStart[2] - ((tPhysics_object*)pObject)->transform_matrix.m[3][2];
        BrMatrix34TApplyV(&cast_start, &tmp_actor, &((tPhysics_object*)pObject)->transform_matrix);
        tmp_actor.v[0] = pEnd[0] - ((tPhysics_object*)pObject)->transform_matrix.m[3][0];
        tmp_actor.v[1] = pEnd[1] - ((tPhysics_object*)pObject)->transform_matrix.m[3][1];
        tmp_actor.v[2] = pEnd[2] - ((tPhysics_object*)pObject)->transform_matrix.m[3][2];
        BrMatrix34TApplyV(&cast_end, &tmp_actor, &((tPhysics_object*)pObject)->transform_matrix);
        if (ShapeRayCast(&cast_start, &cast_end, (tPhysics_shape*)pBounds_shape, &cast_pos, &factor, NULL) != 0) {
            return pCollision_count;
        }
    }
    gCollision_face_mask__raycast |= 1 << best_edge;
    pCollision_count = add_wall_force((float*)pRay_shape, &hits[0].point, hits[0].t, &hits[0].normal, 0, pCollision_count, (tPhysics_object*)pObject, 0, (tFace_ref*)pFace, hits[0].vector.v);
    if (hits[1].t < 2.0f) {
        pCollision_count = add_wall_force((float*)pRay_shape, &hits[1].point, hits[1].t, &hits[1].normal, 0, pCollision_count, (tPhysics_object*)pObject, 0, (tFace_ref*)pFace, hits[1].vector.v);
    }
    return pCollision_count;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0065cf78
tWorld_callbacks gWorld_callbacks = {
    (int (C2_HOOK_FASTCALL *)(void*, float*, int))ProcessForcesCallback,
    ProcessJointForcesCallback,
    NewFaceListCallBack,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
};

extern tPhysics_object* gUNK_0079ecbc;
extern int gSensible_place_collect_active;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00429070
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    tSensible_place_contact contacts[8];
    tPhysics_object* p1;
    tPhysics_object* p2;
    tPhysics_object* child;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    int count;
    int i;
    int found;
    br_vector3 dir;
    float len;

    gUNK_0079ecbc = NULL;
    p1 = pCollision_info_1;
    p2 = pCollision_info_2;
    GetNewBoundingBox(&p1->field_0xf4, &p1->bb1, &p1->transform_matrix);
    GetFacesNearObject(p1, pCar_callbacks);
    gSensible_place_collect_active = 1;
    count = GetObjectWallForceInfo(p1, contacts, 0);
    gSensible_place_collect_active = 0;
    if (count != 0) {
        dir.v[0] = contacts[0].normal.v[0];
        dir.v[1] = contacts[0].normal.v[1];
        dir.v[2] = contacts[0].normal.v[2];
        if (count > 1) {
            for (i = 1; i < count; i++) {
                dir.v[0] += contacts[i].normal.v[0];
                dir.v[1] += contacts[i].normal.v[1];
                dir.v[2] += contacts[i].normal.v[2];
            }
            len = (float)sqrt(dir.v[0] * dir.v[0] + dir.v[1] * dir.v[1] + dir.v[2] * dir.v[2]);
            if (len > 2.3841858e-07f) {
                len = 1.0 / len;
                dir.v[0] *= len;
                dir.v[1] *= len;
                dir.v[2] *= len;
            } else {
                dir.v[0] = 1.f;
                dir.v[1] = 0.f;
                dir.v[2] = 0.f;
            }
        }
        BrMatrix34ApplyV(pVec3, &dir, &p1->transform_matrix);
        return 0;
    }
    if (p2 == NULL) {
        return -1;
    }
    do {
        found = 0;
        if (p2 != p1 && p1->parent != p2 && p2->parent != p1) {
            shape1 = p1->shape;
            shape2 = p2->shape;
            if (FlagsAreGo(p1, p2) && BoundsOverlapTest(&p1->field_0xf4, &p2->field_0xf4)) {
                while (shape1 != NULL) {
                    while (shape2 != NULL) {
                        if (TestShapeEdgeIntersection(p1, p2, shape1, shape2) || TestShapeEdgeIntersection(p2, p1, shape2, shape1)) {
                            found = 1;
                            goto found_body;
                        }
                        shape2 = shape2->common.next;
                    }
                    shape1 = shape1->common.next;
                }
            }
        }
        if (found != 0) {
            goto found_body;
        }
        for (child = p2->child; child != NULL; child = child->next) {
            if (TestPairOverlap(p1, child)) {
                goto found_body;
            }
        }
    } while ((p2 = p2->next) != NULL);
    return -1;
found_body:
    {
        pVec3->v[0] = p1->field_0x10c.min.v[0] + p1->field_0x10c.max.v[0];
        pVec3->v[1] = p1->field_0x10c.min.v[1] + p1->field_0x10c.max.v[1];
        pVec3->v[2] = p1->field_0x10c.min.v[2] + p1->field_0x10c.max.v[2];
        pVec3->v[0] -= p2->field_0x10c.min.v[0];
        pVec3->v[1] -= p2->field_0x10c.min.v[1];
        pVec3->v[2] -= p2->field_0x10c.min.v[2];
        pVec3->v[0] -= p2->field_0x10c.max.v[0];
        pVec3->v[1] -= p2->field_0x10c.max.v[1];
        pVec3->v[2] -= p2->field_0x10c.max.v[2];
        len = (float)sqrt(pVec3->v[0] * pVec3->v[0] + pVec3->v[1] * pVec3->v[1] + pVec3->v[2] * pVec3->v[2]);
        if (len > 2.3841858e-07f) {
            len = 1.0 / len;
            pVec3->v[0] *= len;
            pVec3->v[1] *= len;
            pVec3->v[2] *= len;
        } else {
            pVec3->v[0] = 1.f;
            pVec3->v[1] = 0.f;
            pVec3->v[2] = 0.f;
        }
        gUNK_0079ecbc = p2;
        return 0;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x004293c0
int C2_HOOK_FASTCALL TestPairOverlap(tPhysics_object* pObject, tPhysics_object* pOther) {
    tPhysics_object* child;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    int found;

    found = 0;
    if (pOther != pObject && pObject->parent != pOther && pOther->parent != pObject) {
        shape1 = pObject->shape;
        shape2 = pOther->shape;
        if (FlagsAreGo(pObject, pOther) && BoundsOverlapTest(&pObject->field_0xf4, &pOther->field_0xf4)) {
            while (shape1 != NULL) {
                while (shape2 != NULL) {
                    if (TestShapeEdgeIntersection(pObject, pOther, shape1, shape2) || TestShapeEdgeIntersection(pOther, pObject, shape2, shape1)) {
                        found = 1;
                        goto done;
                    }
                    shape2 = shape2->common.next;
                }
                shape1 = shape1->common.next;
            }
        }
    }
done:
    if (found != 0) {
        return 1;
    }
    for (child = pOther->child; child != NULL; child = child->next) {
        if (TestPairOverlap(pObject, child)) {
            return 1;
        }
    }
    return 0;
}
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
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// FUNCTION: CARMA2_HW 0x0042a210
void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec) {

    DRActorEnumRecurse(pActor, CreateDefaultCrushDataCB, pCar_spec);
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

// FUNCTION: CARMA2_HW 0x0042a940
void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor* pActor, tCar_crush_buffer* pBuffer) {

    DRActorEnumRecurse(pActor, LinkCarCrushData, pBuffer);
}

// ReadMasterCrushData

// LoadCrushDataFile

// FUNCTION: CARMA2_HW 0x0042a950
intptr_t C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData) {
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// InitVertexData

// CheckWheelPositions

// (GPL-3.0); the original inlines CheckWheelPositions,
// InitModelMasterCrushData, InitModelCrushData, InitPhysCrushData and InitVertexData.

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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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
// CopyShapePolyhedron

// InitShapeStuff

// (GPL-3.0); the original inlines
// InitShapeStuff, CopyShapePolyhedron, InitNetworkShapesStuff and BrVector3InvScale.

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
        car_crush->field_0x564[i] = AllocateWireFrameCollisionShape(2, 1, kMem_crush_data);
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

static float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint) {
    tSoftnessOfNearestPointCB_Context softcb_data;

    softcb_data.point = pPoint;
    softcb_data.distance = BR_SCALAR_MAX;
    softcb_data.softness_factor = 1.f;

    DRActorEnumRecurseWithTranslation(pCar_spec->car_model_actor, NULL, SoftnessOfNearestPointCB, &softcb_data);
    return softcb_data.softness_factor;
}

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
        BrActorToActorMatrix34_2(&wheel_to_car, wheel_actor, pCar_spec->car_model_actor);
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
        br_scalar m1;

        m1 = ((br_vector3*)pActor->t.t.mat.m[3])->v[0];
        delta.v[0] = m1 + pDelta->v[0];
        m1 = ((br_vector3*)pActor->t.t.mat.m[3])->v[1];
        delta.v[1] = m1 + pDelta->v[1];
        m1 = ((br_vector3*)pActor->t.t.mat.m[3])->v[2];
        delta.v[2] = m1 + pDelta->v[2];
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

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex


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

// InitPhysCrushData

// CalculateReferencePoints

// PARTIAL 97.46%: InitModelCrushDataCB 0x0042be20 [x87 memory operand register order [ecx+edi] vs [edi+ecx]; effective match]
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

// InitModelVertexData


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

// InitModelCrushData

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
void C2_HOOK_FASTCALL FreeCrushData(tCar_crush_buffer_entry* pCrush_data) {

    if (pCrush_data != NULL) {
        if (pCrush_data->flap_data != NULL) {
            BrMemFree(pCrush_data->flap_data);
        }
        if (pCrush_data->detach_data != NULL) {
            BrMemFree(pCrush_data->detach_data);
        }
        FreeSmashableParts(pCrush_data);
        BrMemFree(pCrush_data);
    }
}

// FUNCTION: CARMA2_HW 0x0042c530
void C2_HOOK_FASTCALL FreeGroovidelic(tModel_detail_vertex_data* pVertex_data) {

    if (pVertex_data != NULL) {
        BrMemFree(pVertex_data);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// RemoveDuplicatePoints


// FUNCTION: CARMA2_HW 0x0042c540
void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds) {
    float unique[12];
    float weights[1024];
    float* pNext;
    float* pOut;
    float* pVert;
    int remaining;
    int count;
    int axis;
    int found;
    int k;
    float len;
    float scale;
    float norm_x;
    float norm_y;
    float bend;
    float a;
    float b;
    float dist_sq;
    tU16 v;

    if (pModel == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("SimpleCrushModel can't cope with that many vertices");
    }
    if (pCount < 1) {
        return;
    }
    if (pCount > 4) {
        pCount = 4;
    }
    count = 1;
    unique[0] = pPoints[0];
    unique[1] = pPoints[1];
    unique[2] = pPoints[2];
    if (pCount > 1) {
        remaining = pCount - 1;
        pNext = pPoints + 3;
        pOut = unique + 4;
        do {
            found = 1;
            for (k = 0; k < count; k++) {
                float* pU = unique + k * 3;

                if ((float)fabs(pU[0] - pNext[0]) < 0.005f && (float)fabs(pU[1] - pNext[1]) < 0.005f
                    && (float)fabs(pU[2] - pNext[2]) < 0.005f) {
                    found = 0;
                    break;
                }
            }
            if (found) {
                pOut[-1] = pNext[0];
                pOut[0] = pNext[1];
                pOut[1] = pNext[2];
                pOut += 3;
                count++;
            }
            remaining--;
            pNext += 3;
        } while (remaining != 0);
    }
    if (TooFarToCrush(pModel, count, (br_vector3*)unique, gMax_crush_dist_sq) != 0) {
        return;
    }
    len = (float)sqrt(pDir[1] * pDir[1] + pDir[2] * pDir[2] + pDir[0] * pDir[0]);
    if (len > 2.3841858e-07f) {
        scale = 1.0 / len;
        norm_x = pDir[0] * scale;
        norm_y = pDir[1] * scale;
    } else {
        norm_x = 1.0f;
        norm_y = 0.0f;
    }
    if ((float)fabs(norm_x) <= 0.707107f) {
        axis = 1;
        if ((float)fabs(norm_y) <= 0.707107f) {
            axis = 2;
        }
    } else {
        axis = 0;
    }
    v = 0;
    if (pModel->nvertices > 0) {
        do {
            pVert = (float*)((tU8*)pModel->vertices + (tU32)v * 0x28);
            dist_sq = DistSq((br_vector3*)pVert, count, (br_vector3*)unique);
            if (dist_sq <= gMax_crush_dist_sq) {
                bend = 1.0f - (float)sqrt(dist_sq * 47.61f);
            } else {
                bend = 0.0f;
            }
            weights[v] = bend;
            if (bend != gCrushZero) {
                a = (pBounds[axis] - pVert[axis]) / pDir[axis];
                b = (pBounds[axis + 3] - pVert[axis]) / pDir[axis];
                if (b <= a) {
                    a = b;
                }
                if (a > gCrushZero) {
                    if (a < bend) {
                        weights[v] = a;
                    }
                } else {
                    weights[v] = 0.0f;
                }
                if (weights[v] != gCrushZero) {
                    pVert[0] = pVert[0] + weights[v] * pDir[0];
                    pVert[1] = pDir[1] * weights[v] + pVert[1];
                    pVert[2] = pDir[2] * weights[v] + pVert[2];
                }
            }
            v++;
        } while (v < pModel->nvertices);
    }
    PipeSingleCrush(pModel, (br_vector3*)pDir, weights);
    if ((pModel->flags & 0x20) != 0 && pModel->user != NULL) {
        *(int*)pModel->user = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags = pModel->flags | 0x20;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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

// PointSimplexDistSq


// FUNCTION: CARMA2_HW 0x0042ca30
float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    br_vector3 ab;
    br_vector3 bc;
    br_vector3 ca;
    br_vector3 projected;
    float nx;
    float ny;
    float nz;
    float length;
    float distance;

    BrVector3Sub(&ab, pB, pA);
    BrVector3Sub(&bc, pC, pB);
    nx = bc.v[2] * ab.v[1] - bc.v[1] * ab.v[2];
    BrVector3Sub(&ca, pA, pC);
    ny = bc.v[0] * ab.v[2] - bc.v[2] * ab.v[0];
    nz = bc.v[1] * ab.v[0] - bc.v[0] * ab.v[1];
    length = (br_scalar)sqrt(nz * nz + ny * ny + nx * nx);
    if (length > BR_SCALAR_EPSILON * 2) {
        length = (br_scalar)(1.0 / length);
        nx = nx * length;
        ny = ny * length;
        nz = nz * length;
    } else {
        nx = 1.0f;
        ny = 0.0f;
        nz = 0.0f;
    }
    distance = (pP->v[0] - pA->v[0]) * nx + (pP->v[2] - pA->v[2]) * nz + (pP->v[1] - pA->v[1]) * ny;
    projected.v[0] = pP->v[0] - distance * nx;
    projected.v[1] = pP->v[1] - distance * ny;
    projected.v[2] = pP->v[2] - distance * nz;
    if (OutsideEdgeLine(&projected, pA, &ab, pC)) {
        return PointEdgeDistSq(pP, pA, pB);
    }
    if (OutsideEdgeLine(&projected, pB, &bc, pA)) {
        return PointEdgeDistSq(pP, pB, pC);
    }
    if (OutsideEdgeLine(&projected, pA, &ca, pB)) {
        return PointEdgeDistSq(pP, pA, pC);
    }
    return distance * distance;
}
// PARTIAL 47.41%: dot-chain from Apex; original is cross(dir,point-A) . cross(dir,inside-A) < 0 with frame 0x1c
// FUNCTION: CARMA2_HW 0x0042cc40
int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside) {
    br_vector3 direction;
    br_vector3 offset;
    br_vector3 inside_offset;

    BrVector3Normalise(&direction, pDirection);
    BrVector3Sub(&offset, pPoint, pA);
    direction.v[0] *= BrVector3Dot(&offset, &direction);
    direction.v[1] *= BrVector3Dot(&offset, &direction);
    direction.v[2] *= BrVector3Dot(&offset, &direction);
    BrVector3Sub(&offset, &offset, &direction);
    BrVector3Sub(&inside_offset, pInside, pA);
    return !(BrVector3Dot(&offset, &inside_offset) >= 0.0f);
}

// BuckleTheBugger

// OutsideEdgeLine


// PointTriangleDistSq

// FUNCTION: CARMA2_HW 0x0042ce00
float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
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

// CrushDrone

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0042d180
void C2_HOOK_FASTCALL CrushDroneObject(tPhysics_object* pObject, int pCount, float* pPoints, br_vector3* pScale) {
    tDrone_crush_data* crush_data;
    br_vector3 scaled;

    crush_data = GetDroneCrushData(pObject);
    if (crush_data != NULL && pCount >= 1) {
        scaled.v[0] = crush_data->crushability * pScale->v[0];
        scaled.v[1] = crush_data->crushability * pScale->v[1];
        scaled.v[2] = crush_data->crushability * pScale->v[2];
        SimpleCrushModel(crush_data->model, pCount, pPoints, scaled.v, (float*)&crush_data->bounds);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// DoDroneCrushing

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

extern tCrush_list_item gCrush_lists[8];
extern void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 49.06%: QueueCarCrush - body complete (crush force accumulation); remaining diff:
// force components spilled through integer registers instead of fld, x87 scheduling.
// FUNCTION: CARMA2_HW 0x0042d400
void C2_HOOK_FASTCALL QueueCarCrush(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick) {
    int point_count;
    int unique_count;
    int point_i;
    int list_i;
    int crush_i;
    int i;
    int is_unique;
    float force_length;
    float force_magnitude;
    float scale;
    br_vector3 unique_points[4];
    br_vector3 average;
    br_vector3 scaled_force;

    if (pCar->car_crush_spec == NULL || *(int*)((tU8*)pCar + 0x1b4) != 0 || pCar->car_crush_spec->field_0x4b8 != 0) {
        return;
    }
    force_length = (float)sqrt((double)(pForce->v[0] * pForce->v[0] + pForce->v[2] * pForce->v[2] + pForce->v[1] * pForce->v[1]));
    force_magnitude = pCar->softness_factor * force_length;
    if (!(force_magnitude > gMin_crush_force)) {
        return;
    }
    if (force_magnitude > 10.0f) {
        force_magnitude = 10.0f;
    }
    scale = (float)(1.0 / force_length);
    scaled_force.v[0] = pForce->v[0] * scale;
    scaled_force.v[1] = pForce->v[1] * scale;
    scaled_force.v[2] = pForce->v[2] * scale;
    unique_count = 1;
    unique_points[0] = pChain[0];
    average = pChain[0];
    point_count = pChain_count;
    if (point_count > 4) {
        point_count = 4;
    }
    for (point_i = 1; point_i < point_count; point_i++) {
        is_unique = 1;
        for (i = 0; i < unique_count; i++) {
            if (fabs((double)pChain[point_i].v[0] - (double)unique_points[i].v[0]) < 0.005
                    && fabs((double)pChain[point_i].v[1] - (double)unique_points[i].v[1]) < 0.005
                    && fabs((double)pChain[point_i].v[2] - (double)unique_points[i].v[2]) < 0.005) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_points[unique_count] = pChain[point_i];
            average.v[0] += pChain[point_i].v[0];
            average.v[1] += pChain[point_i].v[1];
            average.v[2] += pChain[point_i].v[2];
            unique_count++;
        }
    }
    average.v[0] /= (float)unique_count;
    average.v[1] /= (float)unique_count;
    average.v[2] /= (float)unique_count;
    for (list_i = 0; list_i < 8; list_i++) {
        if (gCrush_lists[list_i].car_spec == pCar || gCrush_lists[list_i].car_spec == NULL) {
            break;
        }
    }
    if (list_i >= 8) {
        return;
    }
    if (gCrush_lists[list_i].car_spec == NULL) {
        gCrush_lists[list_i].car_spec = pCar;
        gCrush_lists[list_i].count = 1;
        crush_i = 0;
    } else if (gCrush_lists[list_i].count < 4) {
        crush_i = gCrush_lists[list_i].count;
        gCrush_lists[list_i].count++;
    } else {
        float weakest_force;

        weakest_force = gCrush_lists[list_i].items[0].field_0x40;
        crush_i = 0;
        for (i = 1; i < gCrush_lists[list_i].count; i++) {
            if (gCrush_lists[list_i].items[i].field_0x40 < weakest_force) {
                weakest_force = gCrush_lists[list_i].items[i].field_0x40;
                crush_i = i;
            }
        }
        if (!(weakest_force < force_magnitude)) {
            return;
        }
    }
    SetCrush(&gCrush_lists[list_i].items[crush_i], unique_count, unique_points, &average, &scaled_force, force_magnitude, pTick);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// FUNCTION: CARMA2_HW 0x0042d8f0
void C2_HOOK_FASTCALL SetFlappage(br_actor* pActor, tCar_spec* pCar, int pFlag, float pDelay) {
    extern tToggled_door gToggled_doors[16];
    extern int gCount_toggled_doors;
    int i;

    if (gCount_toggled_doors != 16) {
        for (i = 0; i < gCount_toggled_doors; i++) {
            if (gToggled_doors[i].actor == pActor) {
                return;
            }
        }
        gToggled_doors[gCount_toggled_doors].actor = pActor;
        gToggled_doors[gCount_toggled_doors].car = pCar;
        gToggled_doors[gCount_toggled_doors].field_0x8 = pFlag;
        gToggled_doors[gCount_toggled_doors].field_0xc = pDelay;
        gCount_toggled_doors = gCount_toggled_doors + 1;
    }
}

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
    /* WeldCar frees each model and leaves models[0] - already freed - in the array, which the
       original also does; there the freed block keeps its contents, but our allocator can hand
       back zeroed memory and pModel->prepared reads 0 (race crash reproduced 2026-11-14). */
    if (pModel->prepared == NULL) {
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

extern tCar_spec* gActive_car_list[139];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0042da20
void C2_HOOK_FASTCALL SendAllSemiDetachedBits(undefined4* pRecipient) {
    int i;
    int j;
    tCar_spec* car;
    tCar_crush_spec* crush_spec;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1) {
            crush_spec = car->car_crush_spec;
            if (crush_spec != NULL) {
                for (j = 0; j < crush_spec->field_0x270; j++) {
                    SendSemiDetachBit(gActive_car_list[i], crush_spec->field_0x274[j].field_0x0, 0.f, pRecipient);
                }
            }
        }
    }
}

// STUB: not implemented yet
void C2_HOOK_FASTCALL SendDetachBit(tCar_spec* pCar, br_actor* pActor) {
}

// FUNCTION: CARMA2_HW 0x0042dab0
void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4) {
    typedef struct {
        int ID;
        tU8 field_0x4;
        tU8 field_0x5[3];
        tU32 field_0x8;
        tCompressed_vector3 p1;
        tCompressed_vector3 p2;
        tCompressed_vector3 p3;
        tCompressed_vector3 bounds_min;
        tCompressed_vector3 bounds_max;
        tU16 field_0x2a;
    } tSemi_detach_bit_chunk;
    extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);
    extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
    extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
    tNet_message* message;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bnds;
    tSemi_detach_bit_chunk* semi;
    tUser_crush_data* user;

    message = NetBuildGuaranteedMessage(0x24, 0);
    semi = (tSemi_detach_bit_chunk*)&message->guaranteed.contents.detach_bit;
    semi->ID = NetPlayerFromCar(pCar)->ID;
    user = pActor->user;
    semi->field_0x4 = user->crush_data->id;
    semi->field_0x8 = gPHIL_last_physics_tick + 120;
    if (!GetSDBJointPosAndBounds(&p1, &p2, &p3, &bnds, pActor)) {
        CompressVector3(&semi->p1, &p1, -10.f, 10.f);
        CompressVector3(&semi->p2, &p2, -10.f, 10.f);
        CompressVector3(&semi->p3, &p3, -10.f, 10.f);
        CompressVector3(&semi->bounds_min, &bnds.min, -10.f, 10.f);
        CompressVector3(&semi->bounds_max, &bnds.max, -10.f, 10.f);
        semi->field_0x2a = DRScalarToU16(pArg3, 0.f, 40.f);
        if (pArg4) {
            NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, pArg4[0x9], 0);
        } else {
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// SendToggleFlaps

extern tU32 gToo_poor_for_recovery_timeout;

// FUNCTION: CARMA2_HW 0x0042dd50
void C2_HOOK_FASTCALL ToggleFlaps(void) {
    tNet_message_chunk* chunk;
    tNet_game_player_info* player_info;

    if (gCountdown || gToo_poor_for_recovery_timeout != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        PhysicsObjectRecurse(gProgram_state.current_car.collision_info, ToggleFlapsCB, &gProgram_state.current_car);
    } else {
        chunk = NetGetToHostContents(eNet_message_chunk_type_toggle_doors, 0);
        player_info = NetPlayerFromCar(&gProgram_state.current_car);
        if (player_info != NULL) {
            chunk->toggle_doors.id = player_info->ID;
        }
    }
}

// ToggleFlapsCB2


// FUNCTION: CARMA2_HW 0x0042ddb0
int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data) {
    tCar_spec* pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleFlapsCB2, pCar_spec);
    return 0;
}

extern int gCount_toggled_doors;
// FUNCTION: CARMA2_HW 0x0042ddc0
int C2_HOOK_CDECL ToggleFlapsCB2(br_actor* pActor, void* data) {
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

extern tNon_car_spec* gActive_non_car_list[99];

// PARTIAL 32.73%: SplitCar 0x0042e2e0 - logic complete (all 5 zero-compares, both
// half-mass paths, shape/wireframe relink, actor moves); residual is MSVC codegen
// only: x87 stack scheduling and stack-slot/register allocation.
// auto_inline(off) on PipeSingleRelink/AddCarDamageToPipingSession keeps their 6-8 calls real (32.40 -> 32.73).
// Splits the car in two along pPlane: builds the two new car models, hands the
// other half a collision object from gSplit_car_crush_info_buffer, moves the
// crush actors onto the half their bounds lie on, splits the collision shapes
// and rebinds the inertia/velocity of both halves.
extern br_scalar gCrushSqrtHalf;
// FUNCTION: CARMA2_HW 0x0042e2e0
void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag) {
    tPhysics_object* collision_info;
    tPhysics_object* split_collision_info;
    tCar_crush_spec* crush;
    tUser_crush_data* user;
    tCrush_info* split_car;
    tCar_crush_shape_info* shape_info;
    tPhysics_shape* shape;
    tCollision_shape_wireframe* wire_shape;
    tPhysics_shape* old_shape;
    tPhysics_shape* prev_shape;
    tPhysics_joint* joint;
    tPhysics_object* phys_child;
    tPhysics_object* next_phys_child;
    br_model* model;
    br_model* swap_model;
    br_actor* child;
    br_actor* next_child;
    br_vector3 half_size;
    br_vector4 split_plane;
    br_vector3 offset;
    br_scalar plane_axis;
    br_scalar plane_y;
    br_scalar plane_z;
    br_scalar split_distance;
    br_scalar dist_max;
    br_scalar dist_min;
    br_scalar size_along;
    br_scalar weight;
    br_scalar weight1;
    br_scalar mass_positive;
    br_scalar mass_remaining;
    br_scalar moment;
    br_scalar second_moment;
    br_scalar scale;
    br_scalar dot;
    int axis;
    int next_axis;
    int other_axis;
    int flip;
    int flip2;
    int plane_side;
    int i;
    int count;
    int shape_index;
    int axis_offset;
    int next_offset;
    int other_offset;

    if (((pCar->invulnerable_no_crushage == 0) || (pCar->knackered != 0)) && (pCar->field_0x4c8 == 1.0)) {
        collision_info = pCar->collision_info;
        crush = pCar->car_crush_spec;
        half_size.v[0] = (collision_info->bb1.max.v[0] - collision_info->bb1.min.v[0]) * 0.25f;
        half_size.v[1] = (collision_info->bb1.max.v[1] - collision_info->bb1.min.v[1]) * 0.25f;
        half_size.v[2] = (collision_info->bb1.max.v[2] - collision_info->bb1.min.v[2]) * 0.25f;
        if ((float)fabs(pPlane->v[0]) <= gCrushSqrtHalf) {
            axis = 1;
            if ((float)fabs(pPlane->v[1]) <= gCrushSqrtHalf) {
                axis = 2;
            }
        } else {
            axis = 0;
        }
        plane_y = pPlane->v[1];
        next_axis = (axis + 1) % 3;
        plane_z = pPlane->v[2];
        plane_axis = pPlane->v[axis];
        plane_side = pPlane->v[axis] < gCrushZero;
        half_size.v[axis] = 0.f;
        other_axis = (axis + 2) % 3;
        weight = collision_info->bb1.min.v[other_axis];
        half_size.v[next_axis] = (collision_info->bb1.max.v[next_axis] + collision_info->bb1.min.v[next_axis]) * 0.5f;
        half_size.v[other_axis] = (weight + collision_info->bb1.max.v[other_axis]) * 0.5f;
        split_distance = -(((half_size.v[0] * pPlane->v[0] + plane_z * half_size.v[2] + plane_y * half_size.v[1]) - pPlane->v[3]) / plane_axis);
        DRActorEnumRecurse(pCar->car_model_actor, MakeCarModelsMaterialsDoubleSided, pCar);
        split_plane.v[0] = pPlane->v[0];
        split_plane.v[1] = pPlane->v[1];
        split_plane.v[2] = pPlane->v[2];
        user = pCar->car_model_actor->user;
        split_plane.v[3] = pPlane->v[3] - (pCar->car_model_actor->t.t.translate.t.v[2] * split_plane.v[2] + pCar->car_model_actor->t.t.translate.t.v[1] * split_plane.v[1] + pCar->car_model_actor->t.t.translate.t.v[0] * split_plane.v[0]);
        SplitModel(user->models[0], (br_model**)crush->field_0x4c4, &crush->model_actor->model, split_plane.v, 0);
        i = 1;
        if (1 < pCar->count_detail_levels) {
            do {
                if (user->models[i] != NULL) {
                    SplitModel(user->models[i], (br_model**)crush->field_0x4c4 + i, &model, split_plane.v, 1);
                }
                i++;
            } while (i < pCar->count_detail_levels);
        }
        split_car = GetNonCar(&gSplit_car_crush_info_buffer, crush->actor, pCar);
        if (split_car == NULL) {
            i = 0;
            if (0 < pCar->count_detail_levels) {
                do {
                    BrModelFree(BrModelRemove(((br_model**)crush->field_0x4c4)[i]));
                    i++;
                } while (i < pCar->count_detail_levels);
            }
            BrModelFree(crush->model_actor->model);
            DRActorEnumRecurse(pCar->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar);
            return;
        }
        LoseAllLocalPowerups(pCar);
        while (split_car->collision_object->shape->common.type == kCollisionShapeType_Wireframe) {
            split_car->collision_object->shape = split_car->collision_object->shape->common.next;
        }
        i = 0;
        if (0 < pCar->count_detail_levels) {
            do {
                swap_model = user->models[i];
                user->models[i] = ((br_model**)crush->field_0x4c4)[i];
                ((br_model**)crush->field_0x4c4)[i] = swap_model;
                i++;
            } while (i < pCar->count_detail_levels);
        }
        pCar->car_model_actor->model = user->models[0];
        SmashEverything(pCar);
        if (gNet_mode != 0) {
            i = 0;
            if (0 < crush->field_0x270) {
                do {
                    FullyDetachBit(pCar, crush->field_0x274[i].field_0x8);
                    i++;
                } while (i < crush->field_0x270);
            }
            MungeDetachLists(crush);
        }
        PipeSingleRelink((uintptr_t)gNon_track_actor, crush->actor);
        BrActorAdd(gNon_track_actor, crush->actor);
        BrMatrix34Copy(&crush->actor->t.t.mat, &pCar->car_master_actor->t.t.mat);
        PipeSingleNonCar((tU16)pCar->car_ID, crush->actor);
        child = pCar->car_model_actor->children;
        while (child != NULL) {
            next_child = child->next;
            user = child->user;
            if ((user == NULL) || (user->groove == NULL)) {
                i = 0;
                do {
                    if ((child == pCar->pivot_actors[i]) || (child == pCar->wheel_actors[i])) {
                        goto on_non_car_half;
                    }
                    i++;
                } while (i < 6);
                model = user->models[0];
                if ((user != NULL) && (model != NULL)) {
                    flip = gCrushZero < (model->bounds.min.v[0] * split_plane.v[0] + model->bounds.min.v[2] * split_plane.v[2] + model->bounds.min.v[1] * split_plane.v[1]) - split_plane.v[3];
                    flip2 = gCrushZero < (model->bounds.max.v[1] * split_plane.v[1] + model->bounds.max.v[0] * split_plane.v[0] + model->bounds.max.v[2] * split_plane.v[2]) - split_plane.v[3];
                    if (flip == flip2) {
                        if (!flip) {
                            PipeSingleRelink(0, child);
                            BrActorRemove(child);
                            PipeSingleRelink((uintptr_t)crush->model_actor, child);
                            BrActorAdd(crush->model_actor, child);
                            child->model = model;
                        }
                    } else if (*(int*)((tU8*)crush + 0x494) != 8) {
                        PipeSingleRelink(0, child);
                        BrActorRemove(child);
                        *(br_actor**)((tU8*)crush + 0x498 + *(int*)((tU8*)crush + 0x494) * 4) = child;
                        (*(int*)((tU8*)crush + 0x494))++;
                    }
                }
            } else {
on_non_car_half:
                if ((child->t.t.translate.t.v[0] * pPlane->v[0] + pPlane->v[2] * child->t.t.translate.t.v[2] + pPlane->v[1] * child->t.t.translate.t.v[1]) - pPlane->v[3] < gCrushZero) {
                    PipeSingleRelink(0, child);
                    BrActorRemove(child);
                    PipeSingleRelink((uintptr_t)crush->model_actor, child);
                    BrActorAdd(crush->model_actor, child);
                }
            }
            child = next_child;
        }
        gCount_crush_detach_list = 0;
        collision_info = pCar->collision_info;
        *(br_actor**)((tU8*)crush + 0x4d4) = collision_info->actor;
        *(br_scalar*)((tU8*)crush + 0x4d8) = collision_info->M / pCar->field_0x4c8;
        *(br_scalar*)((tU8*)crush + 0x4dc) = collision_info->I.v[0] / pCar->field_0x4c8;
        *(br_scalar*)((tU8*)crush + 0x4e0) = collision_info->I.v[1] / pCar->field_0x4c8;
        *(br_scalar*)((tU8*)crush + 0x4e4) = collision_info->I.v[2] / pCar->field_0x4c8;
        *(br_scalar*)((tU8*)crush + 0x4e8) = collision_info->cmpos.v[0];
        *(br_scalar*)((tU8*)crush + 0x4ec) = collision_info->cmpos.v[1];
        *(br_scalar*)((tU8*)crush + 0x4f0) = collision_info->cmpos.v[2];
        DRBoundsCopy((br_bounds3*)((tU8*)crush + 0x4f4), &collision_info->bb1);
        DRBoundsCopy((br_bounds3*)((tU8*)crush + 0x50c), &collision_info->bb2);
        *(br_scalar*)((tU8*)crush + 0x524) = collision_info->field_0x54.v[0];
        *(br_scalar*)((tU8*)crush + 0x528) = collision_info->field_0x54.v[1];
        *(br_scalar*)((tU8*)crush + 0x52c) = collision_info->field_0x54.v[2];
        *(br_scalar*)((tU8*)crush + 0x530) = collision_info->v.v[0];
        *(br_scalar*)((tU8*)crush + 0x534) = collision_info->v.v[1];
        *(br_scalar*)((tU8*)crush + 0x538) = collision_info->v.v[2];
        *(br_scalar*)((tU8*)crush + 0x53c) = collision_info->omega.v[0];
        *(br_scalar*)((tU8*)crush + 0x540) = collision_info->omega.v[1];
        *(br_scalar*)((tU8*)crush + 0x544) = collision_info->omega.v[2];
        split_collision_info = split_car->collision_object;
        BrMatrix34Copy(&split_collision_info->transform_matrix, &collision_info->transform_matrix);
        split_collision_info->last_special_volume = collision_info->last_special_volume;
        split_collision_info->auto_special_volume = collision_info->auto_special_volume;
        split_collision_info->water_normal = collision_info->water_normal;
        split_collision_info->water_d = collision_info->water_d;
        split_collision_info->water_depth_factor = collision_info->water_depth_factor;
        dist_max = *(br_scalar*)((tU8*)crush + 0x500 + axis * 4) - split_distance;
        dist_min = split_distance - *(br_scalar*)((tU8*)crush + 0x4f4 + axis * 4);
        size_along = *(br_scalar*)((tU8*)crush + 0x4e8 + axis * 4) - *(br_scalar*)((tU8*)crush + 0x4f4 + axis * 4);
        weight = (size_along * dist_min) / (dist_max + dist_min);
        weight1 = (dist_min - weight) * dist_min;
        mass_positive = (*(br_scalar*)((tU8*)crush + 0x4d8) * weight1) / (weight * dist_max + weight1);
        mass_remaining = *(br_scalar*)((tU8*)crush + 0x4d8);
        collision_info->cmpos.v[0] = *(br_scalar*)((tU8*)crush + 0x4e8);
        mass_remaining = mass_remaining - mass_positive;
        collision_info->cmpos.v[1] = *(br_scalar*)((tU8*)crush + 0x4ec);
        collision_info->cmpos.v[2] = *(br_scalar*)((tU8*)crush + 0x4f0);
        split_collision_info->cmpos.v[0] = *(br_scalar*)((tU8*)crush + 0x4e8);
        split_collision_info->cmpos.v[1] = *(br_scalar*)((tU8*)crush + 0x4ec);
        split_collision_info->cmpos.v[2] = *(br_scalar*)((tU8*)crush + 0x4f0);
        weight = *(br_scalar*)((tU8*)crush + 0x4f4 + axis * 4) + weight;
        dist_max = (dist_max * size_along) / (dist_max + dist_min) + dist_min + *(br_scalar*)((tU8*)crush + 0x4f4 + axis * 4);
        if (split_distance - 0.014492754f < weight) {
            weight = split_distance - 0.014492754f;
        }
        if (dist_max > split_distance - -0.014492754f) {
            dist_max = split_distance - -0.014492754f;
        }
        if (!plane_side) {
            collision_info->M = mass_remaining;
            split_collision_info->M = mass_positive;
            collision_info->cmpos.v[axis] = dist_max;
        } else {
            collision_info->M = mass_positive;
            split_collision_info->M = mass_remaining;
            collision_info->cmpos.v[axis] = weight;
            weight = dist_max;
        }
        split_collision_info->cmpos.v[axis] = weight;
        collision_info->omega.v[0] = *(br_scalar*)((tU8*)crush + 0x53c);
        collision_info->omega.v[1] = *(br_scalar*)((tU8*)crush + 0x540);
        collision_info->omega.v[2] = *(br_scalar*)((tU8*)crush + 0x544);
        split_collision_info->omega.v[0] = *(br_scalar*)((tU8*)crush + 0x53c);
        split_collision_info->omega.v[1] = *(br_scalar*)((tU8*)crush + 0x540);
        split_collision_info->omega.v[2] = *(br_scalar*)((tU8*)crush + 0x544);
        offset.v[1] = collision_info->cmpos.v[1] - *(br_scalar*)((tU8*)crush + 0x4ec);
        offset.v[2] = collision_info->cmpos.v[2] - *(br_scalar*)((tU8*)crush + 0x4f0);
        offset.v[0] = collision_info->cmpos.v[0] - *(br_scalar*)((tU8*)crush + 0x4e8);
        collision_info->v.v[0] = *(br_scalar*)((tU8*)crush + 0x540) * offset.v[2] - *(br_scalar*)((tU8*)crush + 0x544) * offset.v[1];
        collision_info->v.v[1] = *(br_scalar*)((tU8*)crush + 0x544) * offset.v[0] - *(br_scalar*)((tU8*)crush + 0x53c) * offset.v[2];
        collision_info->v.v[2] = *(br_scalar*)((tU8*)crush + 0x53c) * offset.v[1] - *(br_scalar*)((tU8*)crush + 0x540) * offset.v[0];
        BrMatrix34ApplyV(&offset, &collision_info->v, &(*(br_actor**)((tU8*)crush + 0x4d4))->t.t.mat);
        collision_info->v.v[0] = offset.v[0] + *(br_scalar*)((tU8*)crush + 0x530);
        collision_info->v.v[1] = offset.v[1] + *(br_scalar*)((tU8*)crush + 0x534);
        collision_info->v.v[2] = offset.v[2] + *(br_scalar*)((tU8*)crush + 0x538);
        BrMatrix34TApplyV(&collision_info->velocity_car_space, &collision_info->v, &collision_info->actor->t.t.mat);
        offset.v[1] = split_collision_info->cmpos.v[1] - *(br_scalar*)((tU8*)crush + 0x4ec);
        offset.v[2] = split_collision_info->cmpos.v[2] - *(br_scalar*)((tU8*)crush + 0x4f0);
        offset.v[0] = split_collision_info->cmpos.v[0] - *(br_scalar*)((tU8*)crush + 0x4e8);
        split_collision_info->v.v[0] = *(br_scalar*)((tU8*)crush + 0x540) * offset.v[2] - *(br_scalar*)((tU8*)crush + 0x544) * offset.v[1];
        split_collision_info->v.v[1] = *(br_scalar*)((tU8*)crush + 0x544) * offset.v[0] - *(br_scalar*)((tU8*)crush + 0x53c) * offset.v[2];
        split_collision_info->v.v[2] = *(br_scalar*)((tU8*)crush + 0x53c) * offset.v[1] - *(br_scalar*)((tU8*)crush + 0x540) * offset.v[0];
        BrMatrix34ApplyV(&offset, &split_collision_info->v, &(*(br_actor**)((tU8*)crush + 0x4d4))->t.t.mat);
        split_collision_info->v.v[0] = offset.v[0] + *(br_scalar*)((tU8*)crush + 0x530);
        split_collision_info->v.v[1] = offset.v[1] + *(br_scalar*)((tU8*)crush + 0x534);
        split_collision_info->v.v[2] = offset.v[2] + *(br_scalar*)((tU8*)crush + 0x538);
        BrMatrix34TApplyV(&split_collision_info->velocity_car_space, &split_collision_info->v, &split_collision_info->actor->t.t.mat);
        axis_offset = (axis + 2) * 4;
        next_offset = ((axis + 1) % 3) * 4 + 8;
        other_offset = ((axis + 2) % 3) * 4 + 8;
        *(br_scalar*)((tU8*)collision_info + axis_offset) = (*(br_scalar*)((tU8*)crush + 0x4d4 + axis_offset) * collision_info->M) / *(br_scalar*)((tU8*)crush + 0x4d8);
        *(br_scalar*)((tU8*)split_collision_info + axis_offset) = (split_collision_info->M * *(br_scalar*)((tU8*)crush + 0x4d4 + axis_offset)) / *(br_scalar*)((tU8*)crush + 0x4d8);
        moment = split_collision_info->cmpos.v[axis] - *(br_scalar*)((tU8*)crush + 0x4e8 + axis * 4);
        second_moment = collision_info->cmpos.v[axis] - *(br_scalar*)((tU8*)crush + 0x4e8 + axis * 4);
        moment = split_collision_info->M * moment * moment + collision_info->M * second_moment * second_moment;
        mass_remaining = collision_info->M;
        weight1 = *(br_scalar*)((tU8*)crush + 0x4d4 + next_offset) - moment;
        scale = *(br_scalar*)((tU8*)crush + 0x4d8);
        offset.v[(axis + 1) % 3] = weight1;
        mass_positive = offset.v[(axis + 1) % 3];
        *(br_scalar*)((tU8*)collision_info + next_offset) = (mass_remaining * weight1) / scale;
        *(br_scalar*)((tU8*)split_collision_info + next_offset) = (split_collision_info->M * mass_positive) / *(br_scalar*)((tU8*)crush + 0x4d8);
        moment = *(br_scalar*)((tU8*)crush + 0x4d4 + other_offset) - moment;
        mass_remaining = collision_info->M;
        shape_index = 0;
        count = 0;
        scale = *(br_scalar*)((tU8*)crush + 0x4d8);
        offset.v[(axis + 2) % 3] = moment;
        mass_positive = offset.v[(axis + 2) % 3];
        *(br_scalar*)((tU8*)collision_info + other_offset) = (mass_remaining * moment) / scale;
        *(br_scalar*)((tU8*)split_collision_info + other_offset) = (split_collision_info->M * mass_positive) / *(br_scalar*)((tU8*)crush + 0x4d8);
        *(tU8*)((tU8*)collision_info + 0xed) = 1;
        *(tU8*)((tU8*)split_collision_info + 0xed) = 1;
        *(tU8*)((tU8*)collision_info + 0xec) = 0;
        *(tU8*)((tU8*)split_collision_info + 0xec) = 0;
        shape_info = crush->field_0x4;
        shape = split_car->collision_object->shape;
        prev_shape = NULL;
        count = 0;
        if (0 < crush->count_shapes) {
            i = 0;
            do {
                if (shape_info[i].field_0x8 == 0) {
                    SplitCollisionShape(shape_info[i].field_0x0, shape_info[i].field_0x4, shape, pPlane->v);
                    shape = shape->common.next;
                    if (prev_shape == NULL) {
                        pCar->collision_info->shape = (tPhysics_shape*)shape_info[i].field_0x4;
                    } else {
                        prev_shape->common.next = (tPhysics_shape*)shape_info[i].field_0x4;
                    }
                    prev_shape = (tPhysics_shape*)shape_info[i].field_0x4;
                }
                count++;
            } while (count < crush->count_shapes);
        }
        if (prev_shape != NULL) {
            prev_shape->common.next = NULL;
        }
        if (count < 4) {
            shape_index = 4 - count;
            do {
                *(undefined4*)((tU8*)shape + 0x38) = 0;
                *(undefined4*)((tU8*)shape + 0x3c) = 0;
                *(undefined4*)((tU8*)shape + 0x40) = 0;
                shape = shape->common.next;
                shape_index--;
            } while (shape_index != 0);
        }
        i = 0;
        count = 4;
        do {
            wire_shape = crush->field_0x564[i];
            dot = (wire_shape->wireframe.points->v[0] * pPlane->v[0] + pPlane->v[2] * wire_shape->wireframe.points->v[2] + pPlane->v[1] * wire_shape->wireframe.points->v[1]) - pPlane->v[3];
            if (dot <= gCrushZero) {
                old_shape = split_collision_info->shape;
                split_collision_info->shape = (tPhysics_shape*)wire_shape;
                wire_shape->common.next = old_shape;
            } else {
                old_shape = pCar->collision_info->shape;
                pCar->collision_info->shape = (tPhysics_shape*)wire_shape;
                wire_shape->common.next = old_shape;
            }
            i++;
            count--;
        } while (count != 0);
        crush->field_0x28 = 1;
        pCar->oldd[0] = pCar->susp_give;
        pCar->oldd[1] = pCar->susp_give;
        pCar->oldd[2] = pCar->susp_give;
        pCar->oldd[3] = pCar->susp_give;
        UpdateCollisionObject(split_collision_info);
        UpdateCollisionObject(pCar->collision_info);
        pCar->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar);
        pCar->number_of_wheels_on_ground = 0;
        phys_child = pCar->collision_info->child;
        while (phys_child != NULL) {
            joint = phys_child->physics_joint1;
            if (gCrushZero <= (joint->field_0x14.v[0] * pPlane->v[0] + pPlane->v[2] * joint->field_0x14.v[2] + joint->field_0x14.v[1] * pPlane->v[1]) - pPlane->v[3]) {
                phys_child = phys_child->next;
            } else {
                next_phys_child = phys_child->next;
                PipeSingleNonCar(*(tU16*)((tU8*)phys_child->actor->type_data + 0x80), phys_child->actor);
                PhysicsRemoveObject(phys_child);
                TellPhysicsJointsHaveChanged(pCar->collision_info);
                PhysicsAddObject(split_collision_info, phys_child);
                TellPhysicsJointsHaveChanged(split_collision_info);
                phys_child = next_phys_child;
            }
        }
        if (gNum_active_non_cars != 99) {
            *(tDriver*)((tU8*)split_car + 0xc) = gSplit_car_driver;
            gActive_non_car_list[gNum_active_non_cars] = (tNon_car_spec*)split_car;
            gNum_active_non_cars = gNum_active_non_cars + 1;
            PHILAddActiveObjectImmediately(split_collision_info, NULL, NULL, NULL);
        }
        crush->field_0x4b8 = 1;
        pCar->use_shell_model = 0;
        crush->field_0x548.v[0] = pPlane->v[0];
        crush->field_0x548.v[1] = pPlane->v[1];
        crush->field_0x548.v[2] = pPlane->v[2];
        crush->field_0x554 = pPlane->v[3];
        if (axis != 1) {
            pCar->field_0x18c8 = 1;
            pCar->revs = 0.f;
            if (((pCar != NULL) && (pCar->driver == eDriver_oppo)) || (gNet_mode == eNet_mode_host)) {
                KnackerThisCar(pCar);
            }
        }
        if (pFlag != 0) {
            ChristmasLights(pCar, gMin_split_damage, gMax_split_damage);
        }
        AddCarDamageToPipingSession((tU16)pCar->car_ID, pPlane, 1);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#undef WRITE_SPLIT_FACE
#undef RECORD_SPLIT_EDGE
extern tU16 gNext_crush_actor_id;

#ifdef REC2_MATCHING
#pragma inline_depth(0)
#endif
// PARTIAL 85.57%: GetNonCar 0x0042f170 - inline_depth(0) stopped PipeSingleNonCar being inlined; residual is the reused zero register (xor ebp,ebp vs immediates) plus object/child-actor register roles
// FUNCTION: CARMA2_HW 0x0042f170
tCrush_info* C2_HOOK_FASTCALL GetNonCar(tCrush_info_buffer* pInfo_buffer, br_actor* pActor, tCar_spec* pCar) {
    int i;
    int count;
    int start_index;
    int j;
    tU16 id;
    tCrush_info* info;
    tCrush_info* candidate;

    count = pInfo_buffer->capacity;
    for (i = 0; i < count; i++) {
        if (*(tDriver*)((tU8*)&pInfo_buffer->crush_infos[i] + 0xc) == 0) {
            break;
        }
    }
    if (i == count) {
        tPhysics_object* object;

        if (gCrush_deferred) {
            return NULL;
        }
        start_index = (int)pInfo_buffer->field_0x8;
        for (i = 0; i < count; i++) {
            candidate = &pInfo_buffer->crush_infos[start_index];
            if (*(tDriver*)((tU8*)candidate + 0xc) != 1) {
                break;
            }
            start_index = (start_index + 1) % count;
        }
        if (i == count) {
            return NULL;
        }
        info = &pInfo_buffer->crush_infos[start_index];
        pInfo_buffer->field_0x8 = ((int)pInfo_buffer->field_0x8 + 1) % count;
        (*(br_actor**)((tU8*)info + 0x10))->type_data = NULL;
        PipeSingleNonCar(*(tU16*)((tU8*)info + 0x80), *(br_actor**)((tU8*)info + 0x10));
        PipeSingleRelink(0, *(br_actor**)((tU8*)info + 0x10));
        BrActorRemove(*(br_actor**)((tU8*)info + 0x10));

        object = info->collision_object->child;
        while (object != NULL) {
            tPhysics_object* next;
            tCrush_info* child_info;
            br_actor* child_actor;
            tCar_spec* child_car;

            next = object->next;
            child_actor = object->actor;
            child_info = child_actor->type_data;
            child_actor->type_data = NULL;
            child_car = *(tCar_spec**)(*(char**)(*(char**)((char*)child_actor->user + 8) + 0x34) + 0x2c);
            if (*(int*)((tU8*)child_car->car_crush_spec + 0x494) != 8) {
                PipeSingleRelink(0, child_actor);
                BrActorRemove(child_actor);
                *(br_actor**)((tU8*)child_car->car_crush_spec + 0x498 + *(int*)((tU8*)child_car->car_crush_spec + 0x494) * 4) = child_actor;
                (*(int*)((tU8*)child_car->car_crush_spec + 0x494))++;
            }
            *(tDriver*)((tU8*)child_info + 0xc) = 0;
            PhysicsRemoveObject(object);
            object = next;
        }
        i = 0;
        for (i = 0; i < gNum_active_non_cars; i++) {
            if (gActive_non_car_list[i] == (tNon_car_spec*)info) {
                break;
            }
        }
        if (i != gNum_active_non_cars) {
            gNum_active_non_cars--;
            if (i < gNum_active_non_cars) {
                for (j = i; j < gNum_active_non_cars; j++) {
                    gActive_non_car_list[j] = gActive_non_car_list[j + 1];
                }
            }
        }
        *(tDriver*)((tU8*)info + 0xc) = 0;
        (*(br_actor**)((tU8*)info + 0x10))->type_data = NULL;
        PHILRemoveObject(info->collision_object);
    } else {
        info = &pInfo_buffer->crush_infos[i];
    }
    pActor->type_data = info;
    *(tDriver*)((tU8*)info + 0xc) = *(tDriver*)((tU8*)pInfo_buffer + 0xc);
    info->collision_object->actor = pActor;
    *(br_actor**)((tU8*)info + 0x10) = pActor;
    *(undefined4*)((tU8*)info + 0x4) = 0;
    *(float*)((tU8*)info + 0xc4) = -1.0f;
    if (gNet_mode != eNet_mode_none) {
        id = pCar->car_ID;
    } else {
        if (gNext_crush_actor_id == 20000) {
            gNext_crush_actor_id = 10000;
        }
        id = gNext_crush_actor_id++;
    }
    *(tU16*)((tU8*)info + 0x80) = id;
    *(tCar_spec**)((tU8*)info + 0xc8) = pCar;
    info->collision_object->owner = info;
    info->collision_object->flags_0x238 = 1;
    info->collision_object->field_0x1a0 = 0xffff;
    info->collision_object->field_0x1a4 = 0;
    return info;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

extern tNon_car_spec* gActive_non_car_list[99];

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

    PhysicsRemoveObject(pObject);
    if (pObject->physics_joint1 != NULL) {
        *(int*)pObject->physics_joint1 = 0;
        pObject->physics_joint1 = NULL;
    }
    if (gCrush_deferred != 0) {
        TellPhysicsJointsHaveChanged(pCar->collision_info);
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
// Global start of the split-polyhedron tables used by the model splitter.
// GLOBAL: CARMA2_HW 0x0067a3b0
tU16 gSplit_polyhedron_edge_map[2560];
// GLOBAL: CARMA2_HW 0x00679444
float* gSplit_polyhedron_source_vertices;
// GLOBAL: CARMA2_HW 0x006796c0
float* gSplit_polyhedron_positive_vertices;
// GLOBAL: CARMA2_HW 0x006796c4
float* gSplit_polyhedron_negative_vertices;
// GLOBAL: CARMA2_HW 0x0067be70
br_face* gSplit_polyhedron_source_faces;


#define RECORD_SPLIT_EDGE(pEdge_a, pEdge_b) \
    do { \
        tU16* edge_slot = gSplit_polyhedron_edge_map; \
        while (edge_slot[2] != 0 && (edge_slot[0] != (pEdge_a) || edge_slot[1] != (pEdge_b))) { \
            edge_slot += 5; \
        } \
        if (edge_slot[2] == 0) { \
            edge_slot[0] = (pEdge_a); \
            edge_slot[1] = (pEdge_b); \
            edge_slot[2] = positive_vertex_count; \
            edge_slot[3] = negative_vertex_count; \
            ((tU8*)edge_slot)[8] = 1; \
            positive_vertex_count++; \
            negative_vertex_count++; \
        } \
    } while (0)

#define WRITE_SPLIT_FACE(pOutput_face, pInput_face, pFace_a, pFace_b, pFace_c) \
    do { \
        br_face* split_output_face = (pOutput_face); \
        split_output_face->vertices[0] = (pFace_a); \
        split_output_face->vertices[1] = (pFace_b); \
        split_output_face->vertices[2] = (pFace_c); \
        split_output_face->smoothing = (pInput_face)->smoothing; \
        split_output_face->material = (pInput_face)->material; \
    } while (0)

// PARTIAL 26.99%: SplitModel 0x0042f5d0 - logic complete (7 callees, same call order); residual is codegen: frame 0x203c vs 0x2034, array block order (output_vertex_indices lowest instead of third) and x87 load scheduling
// FUNCTION: CARMA2_HW 0x0042f5d0
void C2_HOOK_FASTCALL SplitModel(br_model* pSource_model, br_model** pModels, br_model** pModel_out, br_scalar* pPlane, int pFlag) {
    br_vertex* source_vertices;
    br_face* source_faces;
    br_model* positive_model;
    br_model* negative_model;
    br_face* positive_faces;
    br_face* negative_faces;
    br_face* input_face;
    br_vertex* vertex;
    tU16 positive_vertex_count;
    tU16 negative_vertex_count;
    tU16 positive_vertex_index;
    tU16 negative_vertex_index;
    tS16 positive_face_count;
    tS16 negative_face_count;
    tU16 positive_face_index;
    tU16 negative_face_index;
    tU16 nvertices;
    tU16 nfaces;
    tU16 i;
    tU16 v0;
    tU16 v1;
    tU16 v2;
    tU16 positive01;
    tU16 negative01;
    tU16 positive02;
    tU16 negative02;
    tU16 positive12;
    tU16 negative12;
    tU8 side;
    tU8 face_sides[1024];
    tS16 output_vertex_indices[1024];
    float distances[1024];
    tU8 vertex_sides[1024];

    if (pSource_model->nvertices > 0x400) {
        PDFatalError("By the death spanners of Mars, there's a lot of vertices on that model.");
    }
    source_vertices = pSource_model->vertices;
    source_faces = pSource_model->faces;
    nvertices = pSource_model->nvertices;
    nfaces = pSource_model->nfaces;
    gSplit_polyhedron_source_vertices = (float*)source_vertices;
    gSplit_polyhedron_source_faces = source_faces;
    positive_vertex_count = 0;
    negative_vertex_count = 0;
    for (i = 0; i < nvertices; i++) {
        float distance;

        vertex = &source_vertices[i];
        distance = vertex->p.v[1] * pPlane[1];
        distance += vertex->p.v[2] * pPlane[2];
        distance += vertex->p.v[0] * pPlane[0];
        distance -= pPlane[3];
        distances[i] = distance;
        if (distance <= gCrushZero) {
            vertex_sides[i] = 1;
            negative_vertex_count++;
        } else {
            vertex_sides[i] = 0;
            positive_vertex_count++;
        }
    }
    for (i = 0; i < 512; i++) {
        gSplit_polyhedron_edge_map[i * 5 + 2] = 0;
    }
    positive_face_count = 0;
    negative_face_count = 0;
    for (i = 0; i < nfaces; i++) {
        input_face = &source_faces[i];
        v0 = input_face->vertices[0];
        v1 = input_face->vertices[1];
        v2 = input_face->vertices[2];
        side = (tU8)((vertex_sides[v2] << 2) | (vertex_sides[v1] << 1) | vertex_sides[v0]);
        face_sides[i] = side;
        switch (side) {
        case 0:
            positive_face_count++;
            break;
        case 1:
            RECORD_SPLIT_EDGE(v1, v0);
            RECORD_SPLIT_EDGE(v2, v0);
            positive_face_count += 2;
            negative_face_count++;
            break;
        case 2:
            RECORD_SPLIT_EDGE(v0, v1);
            RECORD_SPLIT_EDGE(v2, v1);
            positive_face_count += 2;
            negative_face_count++;
            break;
        case 3:
            RECORD_SPLIT_EDGE(v2, v0);
            RECORD_SPLIT_EDGE(v2, v1);
            positive_face_count++;
            negative_face_count += 2;
            break;
        case 4:
            RECORD_SPLIT_EDGE(v0, v2);
            RECORD_SPLIT_EDGE(v1, v2);
            positive_face_count += 2;
            negative_face_count++;
            break;
        case 5:
            RECORD_SPLIT_EDGE(v1, v0);
            RECORD_SPLIT_EDGE(v1, v2);
            positive_face_count++;
            negative_face_count += 2;
            break;
        case 6:
            RECORD_SPLIT_EDGE(v0, v1);
            RECORD_SPLIT_EDGE(v0, v2);
            positive_face_count++;
            negative_face_count += 2;
            break;
        case 7:
            negative_face_count++;
            break;
        }
        gSplit_polyhedron_source_faces++;
    }
    *pModels = BrModelAllocate(pSource_model->identifier, positive_vertex_count, positive_face_count);
    *pModel_out = BrModelAllocate(pSource_model->identifier, negative_vertex_count, negative_face_count);
    positive_model = *pModels;
    negative_model = *pModel_out;
    positive_model->flags |= 0x880;
    negative_model->flags |= 0x880;
    gSplit_polyhedron_positive_vertices = (float*)positive_model->vertices;
    gSplit_polyhedron_negative_vertices = (float*)negative_model->vertices;
    positive_vertex_index = 0;
    negative_vertex_index = 0;
    for (i = 0; i < nvertices; i++) {
        if (vertex_sides[i] == 0) {
            output_vertex_indices[i] = positive_vertex_index;
            positive_model->vertices[positive_vertex_index].p = source_vertices[i].p;
            positive_model->vertices[positive_vertex_index].map = source_vertices[i].map;
            positive_vertex_index++;
        } else {
            output_vertex_indices[i] = negative_vertex_index;
            negative_model->vertices[negative_vertex_index].p = source_vertices[i].p;
            negative_model->vertices[negative_vertex_index].map = source_vertices[i].map;
            negative_vertex_index++;
        }
    }
    positive_faces = positive_model->faces;
    negative_faces = negative_model->faces;
    positive_face_index = 0;
    negative_face_index = 0;
    gSplit_polyhedron_source_faces = source_faces;
    for (i = 0; i < nfaces; i++) {
        input_face = gSplit_polyhedron_source_faces;
        v0 = input_face->vertices[0];
        v1 = input_face->vertices[1];
        v2 = input_face->vertices[2];
        switch (face_sides[i]) {
        case 0:
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], output_vertex_indices[v1], output_vertex_indices[v2]);
            break;
        case 1:
            GetSplitVertex(v1, v0, &positive01, &negative01, distances);
            GetSplitVertex(v2, v0, &positive02, &negative02, distances);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], negative01, negative02);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v1], output_vertex_indices[v2], positive01);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v2], positive02, positive01);
            break;
        case 2:
            GetSplitVertex(v0, v1, &positive01, &negative01, distances);
            GetSplitVertex(v2, v1, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], positive01, positive12);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v1], negative12, negative01);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v2], output_vertex_indices[v0], positive12);
            break;
        case 3:
            GetSplitVertex(v2, v0, &positive02, &negative02, distances);
            GetSplitVertex(v2, v1, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v2], positive02, positive12);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], negative12, negative02);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v1], negative12, output_vertex_indices[v0]);
            break;
        case 4:
            GetSplitVertex(v0, v2, &positive02, &negative02, distances);
            GetSplitVertex(v1, v2, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], positive12, positive02);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v1], positive12, output_vertex_indices[v0]);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v2], negative02, negative12);
            break;
        case 5:
            GetSplitVertex(v1, v0, &positive01, &negative01, distances);
            GetSplitVertex(v1, v2, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], negative01, negative12);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v1], positive12, positive01);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v2], output_vertex_indices[v0], negative12);
            break;
        case 6:
            GetSplitVertex(v0, v1, &positive01, &negative01, distances);
            GetSplitVertex(v0, v2, &positive02, &negative02, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], positive01, positive02);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v1], output_vertex_indices[v2], negative01);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v2], negative02, negative01);
            break;
        case 7:
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], output_vertex_indices[v1], output_vertex_indices[v2]);
            break;
        }
        gSplit_polyhedron_source_faces++;
    }
    WhitenVertexRGB(pModels, 1);
    BrModelAdd(*pModels);
    if (pFlag != 0) {
        BrModelFree(*pModel_out);
        return;
    }
    WhitenVertexRGB(pModel_out, 1);
    BrModelAdd(*pModel_out);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// GLOBAL: CARMA2_HW 0x0058f860
tU16 gNext_crush_actor_id = 10000;

// PARTIAL 47.48%: 0x00430150 - clamp chain, copy block and store order match; residual x87 scheduling and 0xc vs 0x20 frame (keep)
// GLOBAL: CARMA2_HW 0x00589a54
const br_scalar gFLOAT_00589a54 = 1.0f;
// GLOBAL: CARMA2_HW 0x00589a98
const br_scalar gFLOAT_00589a98 = 0.0001f;
// GLOBAL: CARMA2_HW 0x00589a9c
const br_scalar gFLOAT_00589a9c = 0.9999f;
// GLOBAL: CARMA2_HW 0x00589aa0
const br_scalar gFLOAT_00589aa0 = 0.007246376946568489f;
// GLOBAL: CARMA2_HW 0x00589aa4
const br_scalar gFLOAT_00589aa4 = -0.007246376946568489f;

// FUNCTION: CARMA2_HW 0x00430150
void C2_HOOK_FASTCALL GetSplitVertex(tU16 pVertex_a, tU16 pVertex_b, tU16* pOutput_a, tU16* pOutput_b, float* pDistances) {
    tU16* edge_slot;
    float* target;
    float* source;
    float ratio;
    float other;
    int offset_a;
    int offset_b;

    edge_slot = gSplit_polyhedron_edge_map;
    if (gSplit_polyhedron_edge_map[2] == 0) {
        return;
    }
    while (edge_slot[0] != pVertex_a || edge_slot[1] != pVertex_b) {
        edge_slot += 5;
        if (edge_slot[2] == 0) {
            return;
        }
    }
    *pOutput_a = edge_slot[2];
    *pOutput_b = edge_slot[3];
    if (((tU8*)edge_slot)[8] != 0) {
        target = gSplit_polyhedron_positive_vertices + *pOutput_a * 10;
        ratio = (float)fabs((pDistances[pVertex_b] + SRandomBetween(-0.028985507786273956f, 0.028985507786273956f)) / (pDistances[pVertex_a] - pDistances[pVertex_b]));
        if (ratio > gCrushZero) {
            if (ratio >= gFLOAT_00589a54) {
                ratio = gFLOAT_00589a9c;
            }
        } else {
            ratio = gFLOAT_00589a98;
        }
        other = gFLOAT_00589a54 - ratio;
        source = gSplit_polyhedron_source_vertices;
        offset_b = pVertex_b * 0x28;
        offset_a = pVertex_a * 0x28;
        target[1] = *(float*)((tU8*)source + 4 + offset_b) * other + *(float*)((tU8*)source + 4 + offset_a) * ratio;
        target[2] = *(float*)((tU8*)source + 8 + offset_b) * other + *(float*)((tU8*)source + 8 + offset_a) * ratio;
        target[0] = *(float*)((tU8*)source + offset_b) * other + *(float*)((tU8*)source + offset_a) * ratio;
        target[4] = *(float*)((tU8*)source + 0x10 + offset_b) * other + *(float*)((tU8*)source + 0x10 + offset_a) * ratio;
        target[3] = *(float*)((tU8*)source + 0xc + offset_b) * other + *(float*)((tU8*)source + 0xc + offset_a) * ratio;
        gSplit_polyhedron_negative_vertices[*pOutput_b * 10 + 0] = gSplit_polyhedron_positive_vertices[*pOutput_a * 10 + 0];
        gSplit_polyhedron_negative_vertices[*pOutput_b * 10 + 1] = gSplit_polyhedron_positive_vertices[*pOutput_a * 10 + 1];
        gSplit_polyhedron_negative_vertices[*pOutput_b * 10 + 2] = gSplit_polyhedron_positive_vertices[*pOutput_a * 10 + 2];
        gSplit_polyhedron_negative_vertices[*pOutput_b * 10 + 3] = gSplit_polyhedron_positive_vertices[*pOutput_a * 10 + 3];
        gSplit_polyhedron_negative_vertices[*pOutput_b * 10 + 4] = gSplit_polyhedron_positive_vertices[*pOutput_a * 10 + 4];
        ((tU8*)edge_slot)[8] = 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 77.65%: SplitCollisionShape - array slots and second loop now match (two tail-merged call sites); residual: x87 load scheduling of the plane-distance dot product and the first loop's point-copy store order
// FUNCTION: CARMA2_HW 0x004303c0
void C2_HOOK_FASTCALL SplitCollisionShape(tCollision_shape_polyhedron* pSource, tCollision_shape_polyhedron* pPositive_output, tPhysics_shape* pNegative_output, br_scalar* pPlane) {
    int i;
    int negative_count;
    int positive_count;
    int j;
    br_scalar distances[16];
    int sides[16];
    br_scalar* positive_points;
    br_scalar* negative_points;
    tU32 value;
    br_scalar distance;
    unsigned int index_a;
    unsigned int index_b;

    if (pSource->common.type != kCollisionShapeType_Polyhedron) {
        PDFatalError("Oooh no. I can't split that sort of shape, Mrs Jabberwok.");
    }
    positive_points = (br_scalar*)pPositive_output->polyhedron.points;
    j = 0;
    negative_points = (br_scalar*)pNegative_output->polyhedron.polyhedron.points;
    positive_count = 0;
    negative_count = 0;
    i = 0;
    while (i < pSource->polyhedron.count_points) {
        if (j >= 0xc0) {
            break;
        }
        value = *(tU32*)((tU8*)pSource->polyhedron.points + j);
        distance = pPlane[1] * *(br_scalar*)((tU8*)pSource->polyhedron.points + j + 4)
                + pPlane[2] * *(br_scalar*)((tU8*)pSource->polyhedron.points + j + 8)
                + pPlane[0] * *(br_scalar*)((tU8*)pSource->polyhedron.points + j)
                - pPlane[3];
        distances[i] = distance;
        if (distance > gCrushZero) {
            *(tU32*)positive_points = value;
            positive_points[1] = *(br_scalar*)((tU8*)pSource->polyhedron.points + j + 4);
            positive_points[2] = *(br_scalar*)((tU8*)pSource->polyhedron.points + j + 8);
            positive_points += 3;
            positive_count++;
            sides[i] = 1;
        } else {
            *(tU32*)negative_points = value;
            negative_points[1] = *(br_scalar*)((tU8*)pSource->polyhedron.points + j + 4);
            negative_points[2] = *(br_scalar*)((tU8*)pSource->polyhedron.points + j + 8);
            negative_points += 3;
            negative_count++;
            sides[i] = 2;
        }
        i++;
        j += 0xc;
    }
    i = 0;
    while (i < pSource->polyhedron.count_edges) {
        if (positive_count >= 16 || negative_count >= 16) {
            break;
        }
        index_a = pSource->polyhedron.edges[i].index1;
        index_b = pSource->polyhedron.edges[i].index2;
        if (sides[index_a] != sides[index_b]) {
            if (sides[index_a] == 1) {
                CreateSplitPoints((br_vector3*)positive_points, (br_vector3*)negative_points,
                        (br_vector3*)((tU8*)pSource->polyhedron.points + index_a * 0xc),
                        (br_vector3*)((tU8*)pSource->polyhedron.points + index_b * 0xc),
                        distances[index_a], distances[index_b]);
            } else {
                CreateSplitPoints((br_vector3*)positive_points, (br_vector3*)negative_points,
                        (br_vector3*)((tU8*)pSource->polyhedron.points + index_b * 0xc),
                        (br_vector3*)((tU8*)pSource->polyhedron.points + index_a * 0xc),
                        distances[index_b], distances[index_a]);
            }
            positive_points += 3;
            negative_points += 3;
            positive_count++;
            negative_count++;
        }
        i++;
    }
    pPositive_output->polyhedron.count_points = positive_count;
    pNegative_output->polyhedron.polyhedron.count_points = negative_count;
    if (positive_count < 4) {
        pPositive_output->polyhedron.count_points = 0;
        pPositive_output->polyhedron.count_edges = 0;
        pPositive_output->polyhedron.count_planes = 0;
    } else {
        FillInShape((tPhysics_shape*)pPositive_output);
    }
    if (negative_count < 4) {
        pNegative_output->polyhedron.polyhedron.count_points = 0;
        pNegative_output->polyhedron.polyhedron.count_edges = 0;
        pNegative_output->polyhedron.polyhedron.count_planes = 0;
        return;
    }
    FillInShape(pNegative_output);
}
#ifdef REC2_MATCHING
#pragma inline_depth()
#endif
// PARTIAL 31.37%: 0x00430600 - value flow/store order match; frame is 0xc/2 pushes vs original 0x18/4 pushes, x87 spills differ (keep)
// FUNCTION: CARMA2_HW 0x00430600
void C2_HOOK_FASTCALL CreateSplitPoints(br_vector3* pPositive_intersection, br_vector3* pNegative_intersection, const br_vector3* pPositive_point, const br_vector3* pNegative_point, float pPositive_distance, float pNegative_distance) {
    float ratio1;
    float ratio2;
    float other1;
    float other2;
    float zn;
    float zp;
    float xn;
    float xp;

    ratio1 = -((pPositive_distance - gFLOAT_00589aa0) / (pNegative_distance - pPositive_distance));
    ratio2 = -((pPositive_distance - gFLOAT_00589aa4) / (pNegative_distance - pPositive_distance));
    other1 = gFLOAT_00589a54 - ratio1;
    zn = pNegative_point->v[2];
    zp = pPositive_point->v[2];
    xp = pPositive_point->v[0];
    xn = pNegative_point->v[0];
    pPositive_intersection->v[1] = pNegative_point->v[1] * ratio1 + pPositive_point->v[1] * other1;
    pPositive_intersection->v[2] = zn * ratio1 + zp * other1;
    pPositive_intersection->v[0] = ratio1 * xn + xp * other1;
    other2 = gFLOAT_00589a54 - ratio2;
    zp = pPositive_point->v[2];
    zn = pNegative_point->v[2];
    xp = pPositive_point->v[0];
    xn = pNegative_point->v[0];
    pNegative_intersection->v[1] = pNegative_point->v[1] * ratio2 + pPositive_point->v[1] * other2;
    pNegative_intersection->v[2] = zn * ratio2 + zp * other2;
    pNegative_intersection->v[0] = ratio2 * xn + xp * other2;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

/* inlined into SplitCheck in the original */
static void C2_HOOK_FASTCALL QuantizePlane(br_vector4* pPlane) {
    tU16 compressed[4];

    CompressVector3((tCompressed_vector3*)compressed, (const br_vector3*)pPlane, -1.f, 1.f);
    compressed[3] = DRScalarToU16(pPlane->v[3], -2.5f, 2.5f);
    ExpandVector3((br_vector3*)pPlane, (const tCompressed_vector3*)compressed, -1.f, 1.f);
    pPlane->v[3] = DRU16ToScalar(compressed[3], -2.5f, 2.5f);
}

/* inlined into SplitCheck in the original */
static int C2_HOOK_FASTCALL WithinBounds(br_scalar pValue, br_scalar pMin, br_scalar pMax) {
    return pValue >= pMin && !(pValue > pMax);
}

// PARTIAL 73.66%: SplitCheck - frame 0x44 and prologue match; residual: x87 load scheduling of first/second, param-slot choice for first_z/distance (orig: first_z in pLine slot, distance at esp+0x18) and the n2 register copies in the normalise. (Re-measured 69.86 on 2026-11-07; the earlier 73.16 predates the v5 tree's header/type context. A 3-attempt retune on 2026-11-07 (macro expansion, axis-select order, split tail checks) gained nothing; the macro-expansion variant scored 66.36.)
// FUNCTION: CARMA2_HW 0x004307e0
int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane) {
    tCar_crush_spec* crush;
    const br_bounds3* bounds;
    br_vector3 point;
    br_vector3 size;
    br_vector3 lo;
    br_vector3 hi;
    br_scalar second;
    br_scalar first_z;
    br_scalar second_z;
    br_scalar distance;
    br_scalar length;
    br_scalar n0;
    br_scalar plane_x;
    br_scalar plane_y;
    br_scalar plane_z;
    br_scalar plane_d;
    br_scalar plane_axis;
    br_scalar min_axis;
    int axis;
    int next_axis;
    int last_axis;

    if (pCar->invulnerable_no_crushage != 0 && pCar->knackered == 0) {
        return 0;
    }
    if (pCar->field_0x4c8 != gDouble_00589a60) {
        return 0;
    }
    crush = pCar->car_crush_spec;
    if (crush == NULL) {
        return 0;
    }
    if (crush->field_0x144 != 0) {
        return 0;
    }
    if (crush->field_0x4b8 != 0) {
        return 0;
    }

    bounds = &pCar->collision_info->bb1;
    BrVector3Sub(&size, &bounds->max, &bounds->min);
    BrVector3Scale(&point, &size, 0.25f);
    BrVector3Add(&lo, &bounds->min, &point);
    BrVector3Sub(&hi, &bounds->max, &point);

    switch (pMode) {
    case 0:
        if ((float)fabs(pLine->v[0]) > 0.001f) {
            pHeight = (pLine->v[2] - pLine->v[1] * bounds->min.v[1]) / pLine->v[0];
            second = (pLine->v[2] - pLine->v[1] * bounds->max.v[1]) / pLine->v[0];
            if (QuantizeWithinBounds(&pHeight, 0.02f, bounds->min.v[0], bounds->max.v[0])
                    && QuantizeWithinBounds(&second, 0.02f, bounds->min.v[0], bounds->max.v[0])
                    && WithinBounds((second + pHeight) * gFLOAT_00589a2c, lo.v[0], hi.v[0])) {
                n0 = -size.v[1];
                distance = second - pHeight;
                pPlane->v[0] = n0;
                pPlane->v[2] = distance;
                pPlane->v[1] = 0.f;
                length = (float)sqrt(n0 * n0 + distance * distance);
                if (length > gFLT_EPSILON_00589a5c) {
                    length = (br_scalar)(gDouble_00589a60 / length);
                    pPlane->v[1] = 0.f;
                    pPlane->v[0] = length * n0;
                    pPlane->v[2] = length * distance;
                } else {
                    pPlane->v[0] = 1.f;
                    pPlane->v[1] = 0.f;
                    pPlane->v[2] = 0.f;
                }
                point.v[1] = 0.f;
                point.v[0] = pHeight;
                point.v[2] = bounds->min.v[1];
                pPlane->v[3] = BrVector3Dot(&point, (br_vector3*)pPlane);
                break;
            }
        }
        if (!((float)fabs(pLine->v[1]) > 0.001f)) {
            return 0;
        } else {
            first_z = (pLine->v[2] - bounds->min.v[0] * pLine->v[0]) / pLine->v[1];
            second_z = (pLine->v[2] - pLine->v[0] * bounds->max.v[0]) / pLine->v[1];
            if (!QuantizeWithinBounds(&first_z, 0.02f, bounds->min.v[2], bounds->max.v[2])
                    || !QuantizeWithinBounds(&second_z, 0.02f, bounds->min.v[2], bounds->max.v[2])
                    || !WithinBounds((second_z + first_z) * gFLOAT_00589a2c, lo.v[2], hi.v[2])) {
                return 0;
            }
            n0 = second_z - first_z;
            distance = -size.v[0];
            pPlane->v[0] = n0;
            pPlane->v[2] = distance;
            pPlane->v[1] = 0.f;
            length = (float)sqrt(n0 * n0 + distance * distance);
            if (length > gFLT_EPSILON_00589a5c) {
                length = (br_scalar)(gDouble_00589a60 / length);
                pPlane->v[1] = 0.f;
                pPlane->v[0] = length * n0;
                pPlane->v[2] = length * distance;
            } else {
                pPlane->v[0] = 1.f;
                pPlane->v[1] = 0.f;
                pPlane->v[2] = 0.f;
            }
            point.v[0] = bounds->min.v[0];
            point.v[1] = 0.f;
            point.v[2] = first_z;
            pPlane->v[3] = BrVector3Dot(&point, (br_vector3*)pPlane);
        }
        break;
    case 1:
        pPlane->v[0] = 0.f;
        pPlane->v[1] = 1.f;
        pPlane->v[2] = 0.f;
        pPlane->v[3] = pHeight;
        if (!QuantizeWithinBounds(&pPlane->v[3], 0.02f, bounds->min.v[1], bounds->max.v[1])) {
            return 0;
        }
        break;
    default:
        return 0;
    }

    bounds = &pCar->car_actor->model->bounds;
    if ((float)fabs(pPlane->v[0]) > gCrushSqrtHalf) {
        axis = 0;
    } else if ((float)fabs(pPlane->v[1]) > gCrushSqrtHalf) {
        axis = 1;
    } else {
        axis = 2;
    }
    plane_y = pPlane->v[1];
    next_axis = (axis + 1) % 3;
    plane_z = pPlane->v[2];
    plane_x = pPlane->v[0];
    plane_d = pPlane->v[3];
    plane_axis = pPlane->v[axis];
    min_axis = bounds->min.v[axis] - -0.057971016f;
    point.v[axis] = 0.f;
    last_axis = (axis + 2) % 3;
    point.v[next_axis] = (bounds->min.v[next_axis] + bounds->max.v[next_axis]) * gFLOAT_00589a2c;
    point.v[last_axis] = (bounds->min.v[last_axis] + bounds->max.v[last_axis]) * gFLOAT_00589a2c;
    distance = -(((plane_x * point.v[0] + plane_z * point.v[2] + plane_y * point.v[1]) - plane_d) / plane_axis);
    if (WithinBounds(distance, min_axis, bounds->max.v[axis] - 0.057971016f)) {
        if (BrVector3Dot(&crush->driver_position, (br_vector3*)pPlane) - pPlane->v[3] < gCrushZero) {
            pPlane->v[0] = -pPlane->v[0];
            pPlane->v[1] = -pPlane->v[1];
            pPlane->v[2] = -pPlane->v[2];
            pPlane->v[3] = -pPlane->v[3];
        }
        QuantizePlane(pPlane);
        return 1;
    }
    return 0;
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00430d80
int C2_HOOK_FASTCALL QuantizeWithinBounds(br_scalar* pValue, br_scalar pQuant, br_scalar pMin, br_scalar pMax) {
    br_scalar range = pMax - pMin;
    br_scalar t;
    int count;
    int index;

    t = range / pQuant;
    count = (int)(t > gCrushZero ? t - -0.5f : t - 0.5f);
    if (count == 0) {
        count = 1;
    }
    /* step reuses the dead pQuant argument home slot, as in the original */
    pQuant = range / (br_scalar)count;
    t = (*pValue - pMin) / pQuant;
    index = (int)(t > gCrushZero ? t - -0.5f : t - 0.5f);
    if (index >= 0 && index <= count) {
        *pValue = (br_scalar)index * pQuant + pMin;
        return 1;
    }
    return 0;
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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

extern int gCrush_panel_ids[10];
extern tCar_damge_crush_list_item gCar_damage_crush_list[8];
extern int gCount_car_damage_crush_list;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00431610
void C2_HOOK_FASTCALL TotallySpamTheCar(tCar_spec* pCar_spec, float pDamage) {
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
        if (DRActorEnumRecurseUltraPlus(node, FireOffBitsCB, pCar_spec) != 0) {
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
                DRActorEnumRecurseWithSnart(pCar_spec->car_model_actor, NULL, SimpleCrushCarModelsCB, &context);
                AREndPipingSession();
            } else {
                CrushCollisionShapes(pCar_spec, &context);
                ARStartPipingSession((tPipe_chunk_type)3);
                DRActorEnumRecurseWithSnart(pCar_spec->car_master_actor, NULL, CrushModels, &context);
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
        RandomlyXZSplitCar(pCar_spec);
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
// PARTIAL 60.67%: CrushModels 0x431e20 - logic complete (all 9 calls, frame 0x78, 313 vs 314 insns); residual is MSVC codegen only: stack-slot permutation (user/entry/pMat/found get different of the same 12 offsets), flip kept in a register instead of a memory slot, and the smashable-loop x87/pointer-base scheduling.
// GLOBAL: CARMA2_HW 0x00589a74
br_scalar gCrushSqrtHalf = 0.70710701f;
// GLOBAL: CARMA2_HW 0x00589ae0
br_scalar gFLOAT_00589ae0 = 0.083333336f;
// GLOBAL: CARMA2_HW 0x00589acc
br_scalar gFLOAT_00589acc = 0.0014492754f;
// FUNCTION: CARMA2_HW 0x00431e20
intptr_t C2_HOOK_FASTCALL CrushModels(br_actor* pActor, br_matrix34* pMat, void* pContext) {
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    }* context;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* entry;
    tCar_crush_spec* crush_spec;
    tCar_crush_limits* limits;
    tCar_crush_smashable_part* smashable;
    br_vector3* crush_center;
    tCar_spec* car;
    br_vector3 centre;
    br_vector3 direction;
    br_vector3 points[4];
    float force;
    float max_bend;
    float max_distance;
    float dist_sq;
    tU32 max_vertex;
    volatile tU32 smashable_count;
    int limit_count;
    int axis;
    int flip;
    int found;
    int i;

    if (pActor->type != 1) {
        return 0;
    }
    user = pActor->user;
    if (user == NULL) {
        return 0;
    }
    entry = user->crush_data;
    if (entry == NULL) {
        return 0;
    }
    context = pContext;
    if (context->crush->count > 4) {
        context->crush->count = 4;
    }
    centre.v[0] = 0.f;
    centre.v[1] = 0.f;
    centre.v[2] = 0.f;
    for (i = 0; i < context->crush->count; i++) {
        DrMatrix34ApplyLPInverse(&points[i], &context->crush->vectors[i], pMat);
        centre.v[0] += points[i].v[0];
        centre.v[1] += points[i].v[1];
        centre.v[2] += points[i].v[2];
    }
    DrMatrix34ApplyLPInverse(&centre, &context->crush->field_0x34, pMat);
    BrMatrix34TApplyV(&direction, &context->crush->field_0x44, pMat);
    force = entry->softness_factor * context->force;
    if (TooFarToCrush(pActor->model, context->crush->count, points, gMax_crush_dist_sq) != 0) {
        return 0;
    }
    if ((float)fabs(direction.v[0]) > gCrushSqrtHalf) {
        axis = 0;
    } else if ((float)fabs(direction.v[1]) > gCrushSqrtHalf) {
        axis = 1;
    } else {
        axis = 2;
    }
    flip = direction.v[axis] > gCrushZero;
    crush_spec = context->car->car_crush_spec;
    limit_count = crush_spec->count_limits.counts[axis][flip];
    limits = &crush_spec->limits;
    found = 0;
    for (i = 0; i < limit_count; i++) {
        if (flip != 0) {
            if (limits->limits[axis][1].values[i] > direction.v[axis]) {
                found = i;
                break;
            }
        } else {
            if (limits->limits[axis][0].values[i] < direction.v[axis]) {
                found = i;
                break;
            }
        }
    }
    CrushModel(user->models[0], context->crush->count, points, &direction, force, limits, axis, found,
        &max_bend, &max_distance, &max_vertex);
    car = context->car;
    crush_center = &context->crush->field_0x34;
    smashable_count = entry->count_smashables;
    if (smashable_count != 0) {
        smashable = entry->smashables;
        do {
            dist_sq = (smashable->field_0x54.v[0] - crush_center->v[0]) * (smashable->field_0x54.v[0] - crush_center->v[0])
                    + (smashable->field_0x54.v[2] - crush_center->v[2]) * (smashable->field_0x54.v[2] - crush_center->v[2])
                    + (smashable->field_0x54.v[1] - crush_center->v[1]) * (smashable->field_0x54.v[1] - crush_center->v[1]);
            if (dist_sq < gFLOAT_00589acc) {
                dist_sq = gFLOAT_00589acc;
            }
            QueueSmashIfNecessary2(car, car->collision_info, &car->pos, NULL, pActor, 0, smashable,
                (force * gDistortion_factor) / (float)sqrt((double)dist_sq), 0);
            smashable++;
            smashable_count--;
        } while (smashable_count != 0);
    }
    if (entry->detach_data != NULL) {
        entry->detach_data->field_0x0 += context->force * max_bend;
        if (entry->detach_data->field_0x0 > entry->detach_data->force) {
            SetBitForDetachment(pActor, car, 0.f, 0);
        }
    }
    if (entry->flap_data != NULL && entry->flap_data->kev_o_flap == 0 && max_distance > gCrushZero) {
        BuckleTheBugger(context->car, pActor, &direction, max_distance, max_vertex, context->crush->field_0x50);
    }
    if (max_bend != gCrushZero) {
        entry->field_0x2c = 1;
    }
    for (i = 1; i < context->car->count_detail_levels; i++) {
        CrushModel(user->models[i], context->crush->count, points, &direction, force, limits, axis, found,
            &max_bend, &max_distance, &max_vertex);
    }
    return 0;
}
extern void __cdecl srand(unsigned int pSeed);

// FUNCTION: CARMA2_HW 0x004321d0
void C2_HOOK_FASTCALL BuckleTheBugger(tCar_spec* pCar, br_actor* pActor, br_vector3* pDirection, float pDistance, tU32 pMax_vertex, tU32 pField_0x50) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap;
    tUser_detail_level_model* detail;
    tModel_detail_vertex_data* vertex_data;
    tUser_crush_data* parent_user;
    br_model* model;
    br_vertex* vertices;
    br_vector3* hinge;
    br_vertex* max_vertex;
    const br_vector3* parent_point;
    br_vector3 offset;
    br_vector3 hinge01;
    br_vector3 perpendicular;
    br_vector3 hinge_normal;
    float weights[1024];
    br_scalar projection;
    br_scalar distance;
    br_scalar offset_value;
    br_scalar rand_distance;
    br_scalar temp;
    br_scalar limit_diff;
    int reference_angle;
    int min_angle;
    int max_angle;
    int angle;
    int current_angle;
    tU16 v;
    tU8 changed;
    int index;

    user = pActor->user;
    if (user == NULL) {
        return;
    }
    model = user->models[0];
    if (model == NULL) {
        return;
    }
    detail = model->user;
    if (detail == NULL) {
        return;
    }
    vertex_data = detail->field_0x4;
    if (vertex_data == NULL) {
        return;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        return;
    }
    flap = crush_data->flap_data;
    if (flap == NULL) {
        return;
    }
    if (flap->field_0x14 == 0) {
        return;
    }
    if (model->nvertices > 0x400) {
        PDFatalError("Oh my goodness, there's a lot of vertices on that model.");
    }
    vertices = model->vertices;
    hinge = &vertices[flap->hinge0].p;
    BrVector3Sub(&hinge01, &vertices[flap->hinge1].p, hinge);
    BrVector3Normalise(&hinge01, &hinge01);
    BrVector3Sub(&offset, &vertices[flap->hinge2].p, hinge);
    BrVector3Normalise(&offset, &offset);
    BrVector3Cross(&hinge_normal, &hinge01, &offset);
    BrVector3Normalise(&hinge_normal, &hinge_normal);
    projection = BrVector3Dot(&offset, &hinge01);
    BrVector3Scale(&perpendicular, &hinge01, projection);
    BrVector3Sub(&perpendicular, &perpendicular, &offset);
    BrVector3Normalise(&perpendicular, &perpendicular);
    parent_user = pActor->parent->user;
    parent_point = &parent_user->models[0]->vertices[flap->field_0x12].p;
    DrMatrix34ApplyLPInverse(&offset, parent_point, &pActor->t.t.mat);
    BrVector3Sub(&offset, &offset, hinge);
    projection = -BrVector3Dot(&perpendicular, &offset);
    if (projection == gCrushZero) {
        reference_angle = 0;
    } else {
        reference_angle = (int)(atan2(BrVector3Dot(&hinge_normal, &offset), projection) * (32768.0 / PI));
    }
    max_vertex = &vertices[(tU16)pMax_vertex];
    BrVector3Sub(&offset, hinge, &max_vertex->p);
    distance = BrVector3Dot(&perpendicular, &offset) / projection;
    if (distance <= gCrushZero) {
        return;
    }
    if (pDistance <= gCrushZero) {
        return;
    }
    offset_value = (float)sqrt((distance + pDistance) * (distance + pDistance) - distance * distance) * 0.5f;
    srand(pField_0x50);
    if (PercentageChance((int)gChance_of_inverse_buckle) != 0) {
        offset_value = -offset_value;
    }
    flap->field_0x18 = flap->field_0x18 + offset_value;
    limit_diff = (float)fabs(flap->field_0x18) - flap->field_0x1c;
    if (limit_diff > gCrushZero) {
        flap->field_0x14 = 0;
        if (offset_value > gCrushZero) {
            offset_value = offset_value - limit_diff;
        } else {
            offset_value = limit_diff + offset_value;
        }
    }
    rand_distance = SRandomBetween(0.3f, 0.7f) * distance;
    min_angle = 0x3ffc;
    max_angle = 0xc004;
    for (v = 0; v < model->nvertices; v++) {
        BrVector3Sub(&offset, &vertices[v].p, &max_vertex->p);
        distance = BrVector3Dot(pDirection, &offset);
        if (distance > gCrushZero) {
            temp = distance - rand_distance;
            if (temp > gCrushZero) {
                BrVector3Sub(&offset, hinge, &vertices[v].p);
                distance = BrVector3Dot(&perpendicular, &offset) / projection;
                if (distance > gCrushZero) {
                    if (temp + distance != gCrushZero) {
                        weights[v] = offset_value * distance / (temp + distance);
                    } else {
                        weights[v] = 0.0f;
                    }
                } else {
                    weights[v] = 0.0f;
                }
            } else {
                weights[v] = offset_value * distance / rand_distance;
            }
            BrVector3Scale(&offset, &hinge_normal, weights[v]);
            BrVector3Add(&vertices[v].p, &vertices[v].p, &offset);
        } else {
            weights[v] = 0.0f;
        }
        if (vertex_data[v].flags & 2) {
            BrVector3Sub(&offset, &vertices[v].p, hinge);
            projection = -BrVector3Dot(&perpendicular, &offset);
            if (projection != gCrushZero) {
                angle = (int)(atan2(BrVector3Dot(&hinge_normal, &offset), projection) * (32768.0 / PI));
                if ((tU16)(angle - max_angle) < 0x7ff8) {
                    max_angle = angle;
                }
                if ((tU16)(min_angle - angle) < 0x7ff8) {
                    min_angle = angle;
                }
            }
        }
    }
    current_angle = flap->field_0x4;
    flap->field_0x2 = (tU16)(current_angle + reference_angle - min_angle);
    flap->field_0x6 = (tU16)(current_angle + reference_angle + 0x3ffc - max_angle);
    flap->field_0x8 = (tU16)(flap->field_0x6 - flap->field_0x2);
    changed = 0;
    if ((tU16)(current_angle - flap->field_0x2) > flap->field_0x8) {
        if ((tU16)(current_angle - flap->field_0x6) < 0x7ff8) {
            PipeSingleFlap(pActor, (tU16)(flap->field_0x6 - current_angle));
            flap->field_0x4 = flap->field_0x6;
        } else {
            PipeSingleFlap(pActor, (tU16)(flap->field_0x2 - current_angle));
            flap->field_0x4 = flap->field_0x2;
        }
        changed = 1;
    }
    if (changed) {
        BrMatrix34Translate(&pActor->t.t.mat, -hinge->v[0], -hinge->v[1], -hinge->v[2]);
        BrMatrix34PostRotate(&pActor->t.t.mat, flap->field_0x4, &hinge01);
        parent_user = pActor->parent->user;
        parent_point = &parent_user->models[0]->vertices[flap->field_0xc].p;
        BrMatrix34PostTranslate(&pActor->t.t.mat, parent_point->v[0], parent_point->v[1], parent_point->v[2]);
    }
    flap->field_0x15 = 1;
    if (flap->field_0x0 == 0 && gCount_toggled_doors != 16) {
        for (index = 0; index < gCount_toggled_doors; index++) {
            if (gToggled_doors[index].actor == pActor) {
                goto already_toggled;
            }
        }
        gToggled_doors[gCount_toggled_doors].actor = pActor;
        gToggled_doors[gCount_toggled_doors].car = pCar;
        gToggled_doors[gCount_toggled_doors].field_0x8 = 1;
        gToggled_doors[gCount_toggled_doors].field_0xc = 0.f;
        gCount_toggled_doors += 1;
    already_toggled:
        ;
    }
    AddCrushToPipingSession(model, &hinge_normal, weights);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// The original 0x432b80 inlines GetImpactScale (0x436850) and
// LimitImpactScale (0x4367a0); this TU compiles with auto_inline(off),
// so equivalent local copies are force-inlined instead.
#ifdef __GNUC__
#define FUN_00432b80_INLINE static inline __attribute__((always_inline))
#else
#define FUN_00432b80_INLINE static __inline
#endif
FUN_00432b80_INLINE float FUN_00432b80_bend_factor(br_vector3* pVertexPos, int pCount, br_vector3* pVectors, br_scalar pMaxDistSq) {
    float dist_sq;

    dist_sq = DistSq(pVertexPos, pCount, pVectors);
    if (dist_sq > gMax_crush_dist_sq) {
        return gCrushZero;
    }
    return gFLOAT_00589a54 - (float)sqrt(dist_sq / pMaxDistSq);
}

FUN_00432b80_INLINE void FUN_00432b80_morph_vertex(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, const tCar_crush_limits* pLimits, int pFound) {
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00432b80
void C2_HOOK_FASTCALL CrushModel(br_model* pModel, int pCount, br_vector3* pPoints, br_vector3* pDirection, float pForce, const tCar_crush_limits* pLimits, int pAxis, int pFound, float* pMax_bend, float* pMax_distance, tU32* pMax_vertex) {
    float weights[1024];
    tModel_detail_vertex_data* pVertex_data;
    tUser_detail_level_model* pUser;
    float* pVert;
    float bend;
    float force;
    tU16 v;

    *pMax_bend = 0.f;
    *pMax_distance = 0.f;
    *(tU16*)pMax_vertex = 0;
    if (pModel == NULL) {
        return;
    }
    pUser = pModel->user;
    if (pUser == NULL) {
        return;
    }
    pVertex_data = pUser->field_0x4;
    if (pVertex_data == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("Well stone the crows, there's a lot of vertices on that model.");
    }
    v = 0;
    if (pModel->nvertices > 0) {
        do {
            pVert = (float*)((tU8*)pModel->vertices + (tU32)v * 0x28);
            bend = FUN_00432b80_bend_factor((br_vector3*)pVert, pCount, pPoints, gMax_crush_dist_sq);
            weights[v] = bend;
            if (bend > *pMax_bend) {
                *pMax_bend = bend;
                *(tU16*)pMax_vertex = v;
            }
            v++;
        } while (v < pModel->nvertices);
    }
    if (pForce != gCrushZero && *pMax_bend != gCrushZero) {
        force = pForce * gForce_to_movement_factor;
        v = 0;
        if (pModel->nvertices > 0) {
            do {
                pVert = (float*)((tU8*)pModel->vertices + (tU32)v * 0x28);
                if (weights[v] != gCrushZero) {
                    weights[v] = weights[v] * force;
                    FUN_00432b80_morph_vertex(&weights[v], pAxis, pVert[pAxis], pDirection->v[pAxis], (tU32)pVertex_data[v].limit_number & 0xffff, pLimits, pFound);
                    pVert[0] = pVert[0] + weights[v] * pDirection->v[0];
                    pVert[1] = pDirection->v[1] * weights[v] + pVert[1];
                    pVert[2] = pDirection->v[2] * weights[v] + pVert[2];
                }
                v++;
            } while (v < pModel->nvertices);
        }
        *pMax_distance = weights[*(tU16*)pMax_vertex];
        AddCrushToPipingSession(pModel, pDirection, weights);
        if ((pModel->flags & 0x20) != 0 && pModel->user != NULL) {
            *(int*)pModel->user = 1;
            return;
        }
        pModel->custom = JitModelUpdate;
        pModel->flags = pModel->flags | 0x20;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// Helpers of CrushCollisionShapes: the original build inlined these four (they also exist as
// out-of-line functions in 13-crush1.c); keep local copies so the inlining is reproduced.
// UNCERTAIN: pContext points to the caller's anonymous struct { tCar_spec* car; int* pCount; float rate; };
// the original reads a variable-length point list and vector data through the second field, so
// those accesses stay raw byte offsets from the binary.
extern br_scalar gMax_crush_dist_sq;
extern br_scalar gForce_to_movement_factor;

static __inline int SpamCrushNearestAxis(const br_vector3* pVector) {

    if ((float)fabs(pVector->v[0]) > gCrushSqrtHalf) {
        return 0;
    } else if ((float)fabs(pVector->v[1]) > gCrushSqrtHalf) {
        return 1;
    } else {
        return 2;
    }
}

static __inline float SpamCrushBendFactor(br_vector3* pVertexPos, int pCount, br_vector3* pVectors, br_scalar pMaxDistSq) {
    float dist_sq;

    dist_sq = DistSq(pVertexPos, pCount, pVectors);
    if (dist_sq > gMax_crush_dist_sq) {
        return gCrushZero;
    }
    return 1.0f - (float)sqrt(dist_sq / pMaxDistSq);
}

static __inline void SpamCrushMorphVertex(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, tCar_crush_limits* pLimits, int pFound) {
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

static __inline void SpamCrushFinishVertex(br_vector3* pVertexPos, tU16 pFlags, tCar_crush_limits* pLimits) {
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00432e70
void C2_HOOK_FASTCALL CrushCollisionShapes(tCar_spec* pCar, void* pContext) {
    float fVar2;
    float fVar6;
    float fVar7;
    float len;
    float factor;
    float value;
    int axis;
    int k;
    int found;
    int i;
    int j;
    tCar_crush_shape_info* shape;
    br_vector3* vert;
    br_vector3 local_c;
    tCar_crush_spec* spec;


    fVar2 = *(float*)((char*)(*(void**)((char*)pContext + 4)) + 0x48) * *(float*)((char*)pContext + 8);
    fVar6 = *(float*)((char*)(*(void**)((char*)pContext + 4)) + 0x4c) * *(float*)((char*)pContext + 8);
    if (fVar2 > gCrushZero) {
        fVar2 = gCrushZero;
    }
    len = (float)sqrt(fVar6 * fVar6 + fVar2 * fVar2);
    if (len < 1e-05f) {
        return;
    }
    spec = pCar->car_crush_spec;
    local_c.v[0] = gCrushZero / len;
    fVar7 = (float)fabs(local_c.v[0]);
    (void)fVar7;
    local_c.v[1] = fVar2 / len;
    local_c.v[2] = fVar6 / len;
    axis = SpamCrushNearestAxis(&local_c);
    k = local_c.v[axis] > gCrushZero;
    found = 0;
    i = 0;
    if (spec->field_0xa4.counts[axis][k] > 0) {
        do {
            if (k) {
                if (spec->field_0xbc.limits[axis][1].values[i] > *(float*)((char*)(*(void**)((char*)pContext + 4)) + 0x34 + axis * 4)) {
                    found = i;
                    break;
                }
            } else {
                if (spec->field_0xbc.limits[axis][0].values[i] < *(float*)((char*)(*(void**)((char*)pContext + 4)) + 0x34 + axis * 4)) {
                    found = i;
                    break;
                }
            }
            i++;
        } while (i < spec->field_0xa4.counts[axis][k]);
    }
    i = 0;
    if (spec->count_shapes > 0) {
        do {
            shape = &spec->field_0x4[i];
            if (shape->field_0x8 == 0 && shape->count_points > 0) {
                j = 0;
                do {
                    vert = &shape->field_0x18[j].field_0x18;
                    value = *(float*)((char*)vert + 0xc) * len;
                    factor = SpamCrushBendFactor(vert, *(int*)(*(void**)((char*)pContext + 4)), (br_vector3*)((char*)(*(void**)((char*)pContext + 4)) + 4), gMax_crush_dist_sq);
                    if (factor != gCrushZero) {
                        factor = value * gForce_to_movement_factor * factor;
                        axis = SpamCrushNearestAxis(&local_c);
                        SpamCrushMorphVertex(&factor, axis, vert->v[axis], local_c.v[axis], *(tU16*)((char*)vert + 0x10), &spec->field_0xbc, found);
                        if (factor != gCrushZero) {
                            vert->v[0] = local_c.v[0] * factor + vert->v[0];
                            vert->v[1] = local_c.v[1] * factor + vert->v[1];
                            vert->v[2] = local_c.v[2] * factor + vert->v[2];
                        }
                    }
                    if (DRVector3TestForNan(vert) != 0) {
                        PDEnterDebugger("NaN");
                    }
                    SpamCrushFinishVertex(vert, *(tU16*)((char*)vert + 0x10), &spec->field_0xbc);
                    j++;
                } while (j < shape->count_points);
            }
            i++;
        } while (i < spec->count_shapes);
    }
    spec->expand_bounding_box = 7;
}
// PARTIAL 99.99% effective: SimpleCrushCarModelsCB 0x433260 - only fld [esi+8] scheduled before 'mov edx,crush_data' instead of after
// FUNCTION: CARMA2_HW 0x00433260
intptr_t C2_HOOK_FASTCALL SimpleCrushCarModelsCB(br_actor* pActor, br_matrix34* pMat, void* pContext) {
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    }* context;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    br_vector3 points[4];
    br_vector3 direction;
    float scale;
    int i;

    context = pContext;
    if (pActor->type == 1 && pActor->user != NULL) {
        user = pActor->user;
        crush_data = user->crush_data;
        if (crush_data != NULL) {
            for (i = 0; i < context->crush->count; i++) {
                DrMatrix34ApplyLPInverse(&points[i], &context->crush->vectors[i], pMat);
            }
            BrMatrix34TApplyV(&direction, &context->crush->field_0x44, pMat);
            scale = context->force;
            scale *= crush_data->softness_factor;
            direction.v[0] = direction.v[0] * scale;
            direction.v[1] = direction.v[1] * scale;
            direction.v[2] = direction.v[2] * scale;
            for (i = 0; i < context->car->count_detail_levels; i++) {
                SimpleCrushModel(user->models[i], context->crush->count, points[0].v, direction.v,
                    (float*)&context->car->car_crush_spec->field_0x11c);
            }
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// FUNCTION: CARMA2_HW 0x00433350
tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds) {
    extern void C2_HOOK_FASTCALL PipeSingleRelink(tU32 pArg1, br_actor* pActor);
    extern intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);
    extern int gNum_active_non_cars;
    extern tNon_car_spec* gActive_non_car_list[99];

    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCrush_info* info;
    tPhysics_object* bit_object;
    tCar_bit_spec* bit_car;
    br_model* model;
    br_material* material;
    br_vector3 centre;
    int flag;
    int j;

    if (*(int*)((char*)pCar + 0x1b4) != 0
            || pCar->car_crush_spec == NULL
            || pCar->car_crush_spec->field_0x4b8 != 0
            || (user = pActor->user) == NULL
            || (crush_data = user->crush_data) == NULL
            || (detach_data = crush_data->detach_data) == NULL
            || detach_data->field_0x28 != 0
            || pCar->car_crush_spec->field_0x2b0 == 0x18
            || (info = GetNonCar(&gDetached_bit_crush_info_buffer, pActor, pCar)) == NULL) {
        return NULL;
    }
    bit_object = info->collision_object;
    *(float*)((tU8*)detach_data + 0xc) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[0];
    *(float*)((tU8*)detach_data + 0x10) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[1];
    *(float*)((tU8*)detach_data + 0x14) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[2];
    *(float*)((tU8*)detach_data + 0x18) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[0];
    *(float*)((tU8*)detach_data + 0x1c) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[1];
    detach_data->field_0x28 = 2;
    *(float*)((tU8*)detach_data + 0x20) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[2];
    bit_car = &pCar->car_crush_spec->field_0x2b4[pCar->car_crush_spec->field_0x2b0];
    bit_car->field_0x0 = pActor;
    bit_car->field_0x4 = pActor->parent;
    bit_car->field_0x8 = bit_object;
    bit_car->field_0xc = (undefined4*)detach_data;
    bit_car->field_0x10 = PDGetTotalTime();
    pCar->car_crush_spec->field_0x2b0++;
    PipeSingleIdentity(pActor);
    PipeSingleRelink((tU32)gNon_track_actor, pActor);
    ActorRelink(gNon_track_actor, pActor);
    BrMatrix34Copy(&bit_object->transform_matrix, &pActor->t.t.mat);
    PipeSingleNonCar((tU16)pCar->car_ID, pActor);
    pActor->identifier[3] = '!';
    SetUpDetachedBitObject(bit_object, pActor, user->models[0], detach_data, pCar->car_crush_spec, pBounds, pCar->collision_info);
    bit_object->flags |= 0x100;
    if (gNum_active_non_cars != 99) {
        *(tDriver*)&info->field_0xc = gDetached_bit_driver;
        gActive_non_car_list[gNum_active_non_cars] = (tNon_car_spec*)info;
        gNum_active_non_cars++;
        PHILAddActiveObjectImmediately(info->collision_object, NULL, NULL, NULL);
    }
    if (user->groove == NULL) {
        model = user->models[0];
        if (model != NULL) {
            for (j = 0; j < model->prepared->ngroups; j++) {
                material = model->faces[*model->prepared->groups[j].face_user].material;
                if (material != NULL && (material->flags & BR_MATF_TWO_SIDED) == 0) {
                    material->flags |= BR_MATF_TWO_SIDED;
                    BrMaterialUpdate(material, BR_MATU_RENDERING);
                }
            }
        }
        PipeSingleDSModel(1, user->models[0]);
    }
    flag = 0;
    SwitchCarModel(pActor, &flag);
    if (pBounds != NULL) {
        centre.v[0] = pBounds->max.v[0] + pBounds->min.v[0];
        centre.v[1] = pBounds->max.v[1] + pBounds->min.v[1];
        centre.v[2] = pBounds->max.v[2] + pBounds->min.v[2];
        centre.v[0] *= 0.5f;
        centre.v[1] *= 0.5f;
        centre.v[2] *= 0.5f;
        LazyMoveModel(user->models[0], &centre);
    }
    pCar->use_shell_model = 0;
    if (user->groove != NULL) {
        DisableGroovidelic(pActor, 3);
        PipeSingleGrooveOnOff(0, pActor, 3);
    }
    CompressVector3((tCompressed_vector3*)((tU8*)detach_data + 0x7c), &bit_object->bb1.min, -10.f, 10.f);
    CompressVector3((tCompressed_vector3*)((tU8*)detach_data + 0x82), &bit_object->bb1.max, -10.f, 10.f);
    return bit_object;
}

// PARTIAL 61.24%: SetUpDetachedBitObject 0x004336f0 - float operand order, arg reload, x87 schedule
extern tS3_outlet* gCar_outlet;
// FUNCTION: CARMA2_HW 0x004336f0
void C2_HOOK_FASTCALL SetUpDetachedBitObject(tPhysics_object* pObject, br_actor* pActor, br_model* pModel, tCar_crush_detach_data* pDetach_data, tCar_crush_spec* pCar_crush, const br_bounds3* pBounds, tPhysics_object* pParent_collision_info) {
    br_vector3* parent_bb[2];
    br_vector3 half_min;
    br_vector3 half_max;
    br_scalar best_dist;
    int best_index;
    int i;
    int axis;
    br_scalar dx;
    br_scalar dy;
    br_scalar dz;
    tPhysics_object* root;
    br_scalar tmp;

    pObject->actor = pActor;
    if (pBounds != NULL) {
        pObject->shape->common.type = 0;
        DRBoundsCopy(&pObject->shape->common.bb, pBounds);
    } else {
        if (pDetach_data->type == 4) {
            tPhysics_shape* old_shape;

            old_shape = pObject->shape;
            pObject->shape = (tPhysics_shape*)pCar_crush->field_0x4[*(int*)((tU8*)pDetach_data + 0x78)].field_0x0;
            pCar_crush->field_0x4[*(int*)((tU8*)pDetach_data + 0x78)].field_0x0 = (tCollision_shape_polyhedron*)old_shape;
            pCar_crush->field_0x4[*(int*)((tU8*)pDetach_data + 0x78)].field_0x8 = 1;
            pCar_crush->expand_bounding_box |= 1;
            goto LAB_004337fa;
        }
        if (pDetach_data->count_shape_points > 0) {
            switch (NPointConvexHullUsingSample(&pObject->shape->polyhedron.polyhedron, pModel, 8, pDetach_data->count_shape_points, pDetach_data->shape_points)) {
            case 0:
                pObject->shape->common.type = 1;
                GetBoundsFromPointList(pObject->shape->polyhedron.polyhedron.points, pObject->shape->polyhedron.polyhedron.count_points, &pObject->shape->common.bb);
                goto LAB_004337fa;
            case 2:
                pObject->shape->common.type = 3;
                GetBoundsFromPointList(pObject->shape->polyhedron.polyhedron.points, pObject->shape->polyhedron.polyhedron.count_points, &pObject->shape->common.bb);
                goto LAB_004337fa;
            default:
                pObject->shape->common.type = 0;
                break;
            }
        } else {
            pObject->shape->common.type = 0;
        }
        DRBoundsCopy(&pObject->shape->common.bb, &pModel->bounds);
    }
LAB_004337fa:
    DRBoundsCopy(&pObject->bb1, &pObject->shape->common.bb);
    DRBoundsCopy(&pObject->bb2, &pObject->shape->common.bb);
    half_min.v[0] = pObject->bb1.min.v[0] * gFLOAT_00589a2c;
    parent_bb[0] = &pParent_collision_info->bb1.min;
    half_min.v[1] = pObject->bb1.min.v[1] * gFLOAT_00589a2c;
    pObject->M = ((tUser_crush_data*)pActor->user)->crush_data->field_0x24;
    half_min.v[2] = pObject->bb1.min.v[2] * gFLOAT_00589a2c;
    parent_bb[1] = &pParent_collision_info->bb1.max;
    half_max.v[0] = pObject->bb1.max.v[0] * gFLOAT_00589a2c;
    half_max.v[1] = pObject->bb1.max.v[1] * gFLOAT_00589a2c;
    half_max.v[2] = pObject->bb1.max.v[2] * gFLOAT_00589a2c;
    pObject->cmpos.v[0] = half_max.v[0] + half_min.v[0];
    pObject->cmpos.v[1] = half_max.v[1] + half_min.v[1];
    pObject->cmpos.v[2] = half_max.v[2] + half_min.v[2];
    BrMatrix34ApplyP(&half_min, &pObject->cmpos, &pObject->actor->t.t.mat);
    DrMatrix34ApplyLPInverse(&half_max, &half_min, &pParent_collision_info->actor->t.t.mat);
    best_index = 0;
    best_dist = 3.4028235e+38f;
    for (i = 0; i < 6; i++) {
        if (i != 2) {
            tmp = (float)fabs(half_max.v[i >> 1] - parent_bb[i & 1]->v[i >> 1]);
            if (tmp < best_dist) {
                best_dist = tmp;
                best_index = i;
            }
        }
    }
    axis = best_index >> 1;
    if ((best_index & 1) == 0) {
        tmp = pObject->bb1.min.v[axis];
    } else {
        tmp = pObject->bb1.max.v[axis];
    }
    pObject->cmpos.v[axis] = (tmp + pObject->cmpos.v[axis]) * gFLOAT_00589a2c;
    dx = pObject->bb1.max.v[0] - pObject->bb1.min.v[0];
    dy = pObject->bb1.max.v[1] - pObject->bb1.min.v[1];
    dz = pObject->bb1.max.v[2] - pObject->bb1.min.v[2];
    pObject->I.v[2] = pObject->M * (dy * dy + dx * dx) * gFLOAT_00589ae0;
    pObject->I.v[1] = (dz * dz + dx * dx) * pObject->M * gFLOAT_00589ae0;
    pObject->I.v[0] = pObject->M * (dz * dz + dy * dy) * gFLOAT_00589ae0;
    BrMatrix34ApplyV(&half_min, &pParent_collision_info->omega, &pParent_collision_info->actor->t.t.mat);
    BrMatrix34TApplyV(&pObject->omega, &half_min, &pObject->actor->t.t.mat);
    BrMatrix34ApplyP(&half_min, &pParent_collision_info->cmpos, &pParent_collision_info->actor->t.t.mat);
    DrMatrix34ApplyLPInverse(&half_max, &half_min, &pObject->actor->t.t.mat);
    half_min.v[0] = pObject->cmpos.v[0] - half_max.v[0];
    half_min.v[1] = pObject->cmpos.v[1] - half_max.v[1];
    half_min.v[2] = pObject->cmpos.v[2] - half_max.v[2];
    pObject->v.v[0] = pObject->omega.v[1] * half_min.v[2] - pObject->omega.v[2] * half_min.v[1];
    pObject->v.v[1] = pObject->omega.v[2] * half_min.v[0] - pObject->omega.v[0] * half_min.v[2];
    pObject->v.v[2] = pObject->omega.v[0] * half_min.v[1] - pObject->omega.v[1] * half_min.v[0];
    BrMatrix34ApplyV(&half_min, &pObject->v, &pObject->actor->t.t.mat);
    pObject->v.v[0] = pParent_collision_info->v.v[0] + half_min.v[0];
    pObject->v.v[1] = pParent_collision_info->v.v[1] + half_min.v[1];
    pObject->v.v[2] = pParent_collision_info->v.v[2] + half_min.v[2];
    BrVector3SetFloat(&pObject->field_0x54, 0.0f, -0.057971016f, 0.0f);
    pObject->box_face_ref = (tU16)(gFace_num__car - 2);
    for (root = pObject; root->parent != NULL; root = root->parent) {
    }
    SetHierarchyDoingNothingFlag(root, 0);
    pObject->field_0x261 = 0;
    pObject->flags |= 0x100;
    pObject->message_time = 0;
    pObject->field_0x49c = 1;
    pObject->uid = ((tUser_crush_data*)pActor->user)->crush_data->id;
    pObject->auto_special_volume = pParent_collision_info->auto_special_volume;
    pObject->last_special_volume = pParent_collision_info->last_special_volume;
    pObject->water_d = pParent_collision_info->water_d;
}

// FUNCTION: CARMA2_HW 0x00433b90
void C2_HOOK_FASTCALL LazyMoveModel(br_model* pModel, const br_vector3* pOffset) {
    volatile br_scalar center_x;
    volatile br_scalar center_y;
    volatile br_scalar center_z;
    volatile br_vector3 offset;
    volatile br_scalar delta_x;
    volatile br_scalar delta_y;
    volatile br_scalar delta_z;
    tU16 i;

    center_x = (pModel->bounds.max.v[0] + pModel->bounds.min.v[0]) * gFLOAT_00589a2c;
    center_y = (pModel->bounds.max.v[1] + pModel->bounds.min.v[1]) * gFLOAT_00589a2c;
    center_z = (pModel->bounds.max.v[2] + pModel->bounds.min.v[2]) * gFLOAT_00589a2c;
    offset.v[0] = pOffset->v[0];
    offset.v[1] = pOffset->v[1];
    offset.v[2] = pOffset->v[2];
    delta_x = offset.v[0] - center_x;
    delta_y = offset.v[1] - center_y;
    delta_z = offset.v[2] - center_z;
    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] = delta_x + pModel->vertices[i].p.v[0];
        pModel->vertices[i].p.v[1] = delta_y + pModel->vertices[i].p.v[1];
        pModel->vertices[i].p.v[2] = delta_z + pModel->vertices[i].p.v[2];
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        *(int*)pModel->user = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// AddTotalDeflection

// AddTotalDeflection

// RepairModel

// RepairModels_1

// UnBendModel

// UnBendModels

// UnBendActor

// UnBendCollisionShape

// UnBendCollisionShapes

// UnBendWheels



void C2_HOOK_FASTCALL UnBendCollisionShapes(tCar_spec* pCar_spec, const tBend_point_context_local* pContext) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        UnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i], pContext);
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

void C2_HOOK_FASTCALL UnBendOtherThings(tCar_spec* pCar_spec, tBend_data_local* pBend) {
    tBend_point_context_local context;

    context.car = pCar_spec;
    context.bend = pBend;
    BrMatrix34Translate(&context.matrix,
        -pBend->field_0x18.v[0], -pBend->field_0x18.v[1], -pBend->field_0x18.v[2]);
    if (pBend->field_0x3e != 0) {
        BrMatrix34PostRotateY(&context.matrix, pBend->field_0x3e);
    }
    if (pBend->field_0x3c != 0) {
        BrMatrix34PostRotateX(&context.matrix, pBend->field_0x3c);
    }
    if (pBend->field_0x40 != 0) {
        BrMatrix34PostRotateZ(&context.matrix, pBend->field_0x40);
    }
    BrMatrix34PostTranslate(&context.matrix,
        pBend->field_0x18.v[0], pBend->field_0x18.v[1], pBend->field_0x18.v[2]);
    UnBendCollisionShapes(pCar_spec, &context);
    UnBendWheels(pCar_spec, &context);
    memcpy(&pCar_spec->collision_info->cmpos, &pCar_spec->car_crush_spec->field_0x264, sizeof(pCar_spec->collision_info->cmpos));
    BendPoint(&pCar_spec->collision_info->cmpos, &context);
}


// PARTIAL 87.31%: loop compare uses fcomp mem instead of fld mem + fcompp; branch order; context-setup push/lea schedule; bounds x87 order
// FUNCTION: CARMA2_HW 0x00433c70
void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7) {
    tCar_crush_spec* car_crush;
    tBend_data_local* bend;
    br_bounds3* bb;
    tPhysics_object* obj;
    tPhysics_object** user;
    float limit;
    float edge;
    float z;
    br_vector3 d;
    tBend_point_context_local context;
    int i;

    car_crush = pCar->car_crush_spec;
    /* UNCERTAIN: offset 0x1b4 of tCar_spec has no name in rec2_types.h */
    if (car_crush != NULL && *(int*)((char*)pCar + 0x1b4) == 0 && car_crush->field_0x144 == 0) {
        z = pArg5->v[2];
        for (i = 0; i < car_crush->field_0x270; i++) {
            user = (tPhysics_object**)car_crush->field_0x274[i].field_0x0->user;
            if (user != NULL && (obj = *user) != NULL) {
                limit = (z - pArg6) * 0.4f;
                edge = limit + pArg6;
                if (limit >= gCrushZero) {
                    if (obj->bb2.max.v[0] <= edge) {
                        goto detach;
                    }
                } else if (obj->bb2.min.v[0] < edge) {
                    goto detach;
                }
                continue;
            }
detach:
            if (BitObjectIsSufficientlyOutsideCarObjectToDetach(car_crush->field_0x274[i].field_0x8)) {
                FullyDetachBit(pCar, car_crush->field_0x274[i].field_0x8);
            } else {
                ReAttachBit(pCar, car_crush->field_0x274[i].field_0x4, car_crush->field_0x274[i].field_0x0, car_crush->field_0x274[i].field_0x8);
            }
        }
        MungeDetachLists(car_crush);
        BendActor(pCar, pCar->car_master_actor, pAngle_x, pAngle_y, pAngle_z, pArg5, pArg6, (tBend_data_local*)pCar->car_crush_spec->field_0x154);
        bend = (tBend_data_local*)pCar->car_crush_spec->field_0x154;
        context.car = pCar;
        context.bend = bend;
        BrMatrix34Translate(&context.matrix, -bend->field_0x18.v[0], -bend->field_0x18.v[1], -bend->field_0x18.v[2]);
        if (bend->field_0x3e != 0) {
            BrMatrix34PostRotateY(&context.matrix, bend->field_0x3e);
        }
        if (bend->field_0x3c != 0) {
            BrMatrix34PostRotateX(&context.matrix, bend->field_0x3c);
        }
        if (bend->field_0x40 != 0) {
            BrMatrix34PostRotateZ(&context.matrix, bend->field_0x40);
        }
        BrMatrix34PostTranslate(&context.matrix, bend->field_0x18.v[0], bend->field_0x18.v[1], bend->field_0x18.v[2]);
        LoseDodgyPowerups(pCar);
        if (pCar->car_crush_spec->expand_bounding_box != 0) {
            SwapShapesIfPossible(pCar);
        }
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            BendCollisionShape(&pCar->car_crush_spec->field_0x4[i], &context);
        }
        pCar->car_crush_spec->field_0x264.v[0] = pCar->collision_info->cmpos.v[0];
        pCar->car_crush_spec->field_0x264.v[1] = pCar->collision_info->cmpos.v[1];
        pCar->car_crush_spec->field_0x264.v[2] = pCar->collision_info->cmpos.v[2];
        pCar->car_crush_spec->field_0x10.v[0] = pCar->collision_info->cmpos.v[0];
        pCar->car_crush_spec->field_0x10.v[1] = pCar->collision_info->cmpos.v[1];
        pCar->car_crush_spec->field_0x10.v[2] = pCar->collision_info->cmpos.v[2];
        BendPoint(&pCar->car_crush_spec->field_0x10, &context);
        pCar->car_crush_spec->expand_bounding_box = 11;
        SwapShapesIfPossible(pCar);
        BendWheels(pCar, &context);
        SmashEverything(pCar);
        if (pArg7 != 0) {
            ChristmasLights(pCar, gMin_bend_damage, gMax_bend_damage);
        }
        bb = &pCar->collision_info->bb1;
        pCar->use_shell_model = 0;
        d.v[0] = (bb->max.v[0] - bb->min.v[0]) * 0.15f;
        d.v[1] = (bb->max.v[1] - bb->min.v[1]) * 0.15f;
        d.v[2] = (bb->max.v[2] - bb->min.v[2]) * 0.15f;
        pCar->car_crush_spec->field_0x11c.min.v[0] = d.v[0] + bb->min.v[0];
        pCar->car_crush_spec->field_0x11c.min.v[1] = bb->min.v[1] + d.v[1];
        pCar->car_crush_spec->field_0x11c.min.v[2] = bb->min.v[2] + d.v[2];
        pCar->car_crush_spec->field_0x11c.max.v[0] = bb->max.v[0] - d.v[0];
        pCar->car_crush_spec->field_0x11c.max.v[1] = bb->max.v[1] - d.v[1];
        pCar->car_crush_spec->field_0x11c.max.v[2] = bb->max.v[2] - d.v[2];
        *(int*)((char*)pCar->car_crush_spec + 0x1d0) = pCar->car_crush_spec->field_0x2b0;
        pCar->car_crush_spec->field_0x144 = 1;
        CrashEarnings(pCar, NULL);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// PARTIAL 96.41%: residual is instruction scheduling only -- in the BrMatrix34Translate
// argument setup the original emits 'fchs; push ecx; fstp [esp]' where msvc500 emits
// 'push ecx; fchs' (4 diff lines), and the BrMatrix34PostTranslate setup gets a rotated
// temp-register assignment (index/vertex/v[k] in eax/ecx/edx vs edx/eax/ecx).
// Verified with the local msvc500 oracle: fld-based negated pushes never come out as
// fchs-then-push in this toolchain, so this is a scheduler artifact, not source shape.
// FUNCTION: CARMA2_HW 0x00434010
void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_spec* crush_spec;
    tNon_car_spec* non_car;
    tCar_crush_flap_data* flap_data;
    tPhysics_shape* physics_shape;
    tNon_car_spec** active_non_car;
    tUser_crush_data* parent_user_crush_data;
    br_model* model;
    br_model* parent_model;
    br_vertex* vertex;
    int index;
    int remaining;

    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == NULL) {
        return;
    }
    if (detach_data->field_0x28 == 0) {
        return;
    }
    crush_spec = pCar_spec->car_crush_spec;
    non_car = pActor->type_data;
    if (non_car != NULL) {
        pActor->type_data = NULL;
        non_car->driver = 0;
        if (pObject != NULL) {
            PhysicsRemoveObject(pObject);
            if (pObject->physics_joint1 != NULL) {
                pObject->physics_joint1->type = 0;
                pObject->physics_joint1 = NULL;
            }
            if (detach_data->type == 4) {
                physics_shape = pObject->shape;
                pObject->shape = (tPhysics_shape*)crush_spec->field_0x4[*(int*)((tU8*)detach_data + 0x78)].field_0x0;
                crush_spec->field_0x4[*(int*)((tU8*)detach_data + 0x78)].field_0x0 = (tCollision_shape_polyhedron*)physics_shape;
                crush_spec->field_0x4[*(int*)((tU8*)detach_data + 0x78)].field_0x8 = 0;
                crush_spec->expand_bounding_box |= 1;
            }
        } else {
            PipeSingleNonCar(*(tU16*)((tU8*)non_car + 0x80), pActor);
            index = 0;
            while (index < gNum_active_non_cars && gActive_non_car_list[index] != non_car) {
                index++;
            }
            if (index != gNum_active_non_cars) {
                gNum_active_non_cars--;
                if (index < gNum_active_non_cars) {
                    active_non_car = &gActive_non_car_list[index];
                    remaining = gNum_active_non_cars - index;
                    do {
                        *active_non_car = active_non_car[1];
                        active_non_car++;
                        remaining--;
                    } while (remaining != 0);
                }
            }
            non_car->driver = 0;
            non_car->actor->type_data = NULL;
            PHILRemoveObject(non_car->collision_info);
        }
    }
    PipeSingleRelink((uintptr_t)pParent_actor, pActor);
    if (pActor->parent != NULL) {
        ActorRelink(pParent_actor, pActor);
    } else {
        BrActorAdd(pParent_actor, pActor);
    }
    flap_data = crush_data->flap_data;
    if (flap_data == NULL || flap_data->kev_o_flap != 0) {
        BrMatrix34Identity(&pActor->t.t.mat);
        PipeSingleIdentity(pActor);
        if (flap_data != NULL) {
            flap_data->field_0x0 = 0;
        }
    } else {
        model = ((tUser_crush_data*)pActor->user)->models[0];
        vertex = &model->vertices[flap_data->hinge0];
        BrMatrix34Translate(&pActor->t.t.mat, -vertex->p.v[0], -vertex->p.v[1], -vertex->p.v[2]);
        parent_user_crush_data = pParent_actor->user;
        parent_model = parent_user_crush_data->models[0];
        BrMatrix34PostTranslate(&pActor->t.t.mat, (&parent_model->vertices[flap_data->field_0xc])->p.v[0], (&parent_model->vertices[flap_data->field_0xc])->p.v[1], (&parent_model->vertices[flap_data->field_0xc])->p.v[2]);
        PipeSingleFlap(pActor, -flap_data->field_0x4);
        flap_data->field_0x4 = 0;
        if (gCount_toggled_doors != 16) {
            for (index = 0; index < gCount_toggled_doors; index++) {
                if (gToggled_doors[index].actor == pActor) {
                    goto already_toggled;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar_spec;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 1;
            gToggled_doors[gCount_toggled_doors].field_0xc = 0.f;
            gCount_toggled_doors += 1;
        already_toggled:
            ;
        }
        CheckFlapAngles(pActor, 0);
    }
    PipeSingleDSModel(1, user_crush_data->models[0]);
    pActor->model = user_crush_data->models[0];
    if (gProgram_state.racing != 0 && pObject == NULL) {
        DRS3StartSound3D(gCar_outlet, 0x1483, &pCar_spec->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 0xff, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    if (user_crush_data->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 3);
        EnableGroovidelic(pActor, 3);
        PipeSingleTransformType(pActor, 1);
        pActor->t.type = 1;
    }
    detach_data->field_0x28 = 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x00434320
intptr_t C2_HOOK_CDECL CheckFlapAngles(br_actor* pActor, int pValue) {
    tUser_crush_data* user;
    tCar_crush_flap_data* flap;
    tUser_detail_level_model* detail;
    tModel_detail_vertex_data* vertex_data;
    br_model* model;
    br_vertex* vertices;
    const br_vector3* hinge;
    tUser_crush_data* parent_user;
    const br_vector3* parent_point;
    br_vector3 offset;
    br_vector3 hinge01;
    br_vector3 perpendicular;
    br_vector3 hinge_normal;
    br_scalar projection;
    br_angle reference_angle;
    br_angle min_angle;
    br_angle max_angle;
    br_angle angle;
    br_angle current_angle;
    tU16 i;

    (void)pValue;
    user = pActor->user;
    if (user == NULL || user->crush_data == NULL) {
        return 0;
    }
    flap = user->crush_data->flap_data;
    if (flap == NULL || flap->kev_o_flap || !flap->field_0x0 || !flap->field_0x15) {
        return 0;
    }
    model = user->models[0];
    if (model == NULL || model->user == NULL) {
        return 0;
    }
    detail = model->user;
    vertex_data = detail->field_0x4;
    if (vertex_data == NULL) {
        return 0;
    }
    vertices = model->vertices;
    hinge = &vertices[flap->hinge0].p;
    BrVector3Sub(&hinge01, &vertices[flap->hinge1].p, hinge);
    BrVector3Normalise(&hinge01, &hinge01);
    BrVector3Sub(&offset, &vertices[flap->hinge2].p, hinge);
    BrVector3Normalise(&offset, &offset);
    BrVector3Cross(&hinge_normal, &hinge01, &offset);
    BrVector3Normalise(&hinge_normal, &hinge_normal);
    projection = BrVector3Dot(&offset, &hinge01);
    BrVector3Scale(&perpendicular, &hinge01, projection);
    BrVector3Sub(&perpendicular, &offset, &perpendicular);
    BrVector3Normalise(&perpendicular, &perpendicular);

    parent_user = pActor->parent->user;
    parent_point = &parent_user->models[0]->vertices[flap->field_0x12].p;
    DrMatrix34ApplyLPInverse(&offset, parent_point, &pActor->t.t.mat);
    BrVector3Sub(&offset, &offset, hinge);
    projection = BrVector3Dot(&offset, &perpendicular);
    if (projection == gCrushZero) {
        reference_angle = 0;
    } else {
        reference_angle = BrRadianToAngle(atan2(BrVector3Dot(&offset, &hinge_normal), projection));
    }

    min_angle = 0x3ffc;
    max_angle = 0xc004;
    for (i = 0; i < model->nvertices; i++) {
        if (vertex_data[i].flags & 2) {
            BrVector3Sub(&offset, &vertices[i].p, hinge);
            projection = BrVector3Dot(&offset, &perpendicular);
            if (projection != gCrushZero) {
                angle = BrRadianToAngle(atan2(BrVector3Dot(&offset, &hinge_normal), projection));
                if ((br_angle)(angle - max_angle) < 0x7ff8) {
                    max_angle = angle;
                }
                if ((br_angle)(min_angle - angle) < 0x7ff8) {
                    min_angle = angle;
                }
            }
        }
    }
    current_angle = flap->field_0x4;
    flap->field_0x2 = (br_angle)(current_angle + reference_angle - min_angle);
    flap->field_0x6 = (br_angle)(current_angle + reference_angle + 0x3ffc - max_angle);
    flap->field_0x8 = (br_angle)(flap->field_0x6 - flap->field_0x2);
    if ((br_angle)(current_angle - flap->field_0x2) > flap->field_0x8) {
        if ((br_angle)(current_angle - flap->field_0x6) < 0x7ff8) {
            PipeSingleFlap(pActor, (br_angle)(flap->field_0x6 - current_angle));
            flap->field_0x4 = flap->field_0x6;
        } else {
            PipeSingleFlap(pActor, (br_angle)(flap->field_0x2 - current_angle));
            flap->field_0x4 = flap->field_0x2;
        }
    }
    BrMatrix34Translate(&pActor->t.t.mat, -hinge->v[0], -hinge->v[1], -hinge->v[2]);
    BrMatrix34PostRotate(&pActor->t.t.mat, flap->field_0x4, &hinge01);
    parent_user = pActor->parent->user;
    parent_point = &parent_user->models[0]->vertices[flap->field_0xc].p;
    BrMatrix34PostTranslate(&pActor->t.t.mat, parent_point->v[0], parent_point->v[1], parent_point->v[2]);
    return 0;
}

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
    SetHierarchyDoingNothingFlag(pCar_spec->collision_info, 0);
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// PARTIAL 46.03%: BendPoint 0x00434f60 [x87] - frame 0xc vs 0x18 (a,b,c spilled in original), x87 order, final store/call order
// FUNCTION: CARMA2_HW 0x00434f60
void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context_local* pContext) {
    tBend_data_local* bend;
    br_vector3 tmp;
    br_vector3 d;
    float z;
    float a;
    float b;
    float c;
    float ab;

    bend = pContext->bend;
    z = pPoint->v[2] - bend->field_0x18.v[2];
    if ((bend->field_0x74 != 0 && z <= 0.f) || (bend->field_0x74 == 0 && z >= 0.f)) {
        BrMatrix34ApplyP(&tmp, pPoint, &pContext->matrix);
        pPoint->v[0] = tmp.v[0];
        pPoint->v[1] = tmp.v[1];
        pPoint->v[2] = tmp.v[2];
        return;
    }
    d.v[0] = pPoint->v[0] - bend->field_0x24.v[0];
    d.v[1] = pPoint->v[1] - bend->field_0x24.v[1];
    d.v[2] = pPoint->v[2] - bend->field_0x24.v[2];
    if ((bend->field_0x74 != 0 && d.v[2] <= 0.f) || (bend->field_0x74 == 0 && d.v[2] >= 0.f)) {
        b = d.v[1] / bend->field_0x30.v[1];
        a = d.v[0] / bend->field_0x30.v[0];
        c = d.v[2] / bend->field_0x30.v[2];
        ab = b * a;
        pPoint->v[0] = (bend->field_0x68.v[0] + bend->field_0x5c.v[0] * b + bend->field_0x50.v[0] * a + bend->field_0x44.v[0] * ab) * c + pPoint->v[0];
        pPoint->v[1] = (bend->field_0x68.v[1] + bend->field_0x5c.v[1] * b + bend->field_0x50.v[1] * a + bend->field_0x44.v[1] * ab) * c + pPoint->v[1];
        pPoint->v[2] = (bend->field_0x68.v[2] + bend->field_0x5c.v[2] * b + bend->field_0x50.v[2] * a + bend->field_0x44.v[2] * ab) * c + pPoint->v[2];
    }
}
// Adds the points where the polyhedron edges of a crush shape cross the bend
// plane (field_0x18.v[2] of the bend data) to the reordered point array, then
// stores the result as the unbent reference and bends every point.
// FUNCTION: CARMA2_HW 0x00435110
void C2_HOOK_FASTCALL BendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context_local* pContext) {
    float intersections[24];
    float plane_z;
    int i;
    int j;
    br_vector3* points;
    int count_edges;
    int count_points_add_8;
    int added;
    tU8* edges;
    float* pIntersection;
    float* pNext_intersection;
    br_vector3* point1;
    br_vector3* point2;
    float ratio;
    int crosses;

    count_points_add_8 = pShape->count_points_add_8;
    if (pShape->count_points == count_points_add_8) {
        return;
    }
    plane_z = pContext->bend->field_0x18.v[2];
    i = 0;
    added = 0;
    count_edges = pShape->field_0x0->polyhedron.count_edges;
    if (0 < count_edges) {
        points = pShape->field_0x0->polyhedron.points;
        edges = (tU8*)pShape->field_0x0->polyhedron.edges;
        pIntersection = &intersections[1];
        do {
            point1 = &points[edges[0]];
            point2 = &points[edges[1]];
            if (point2->v[2] < plane_z) {
                crosses = point1->v[2] > plane_z;
            } else {
                crosses = point1->v[2] < plane_z;
            }
            pNext_intersection = pIntersection;
            if (crosses && 0.028985508f <= (float)fabs(point1->v[2] - plane_z) && 0.028985508f <= (float)fabs(point2->v[2] - plane_z)) {
                ratio = (plane_z - point1->v[2]) / (point1->v[2] - point2->v[2]);
                added++;
                pNext_intersection = pIntersection + 3;
                pIntersection[-1] = point1->v[0] + (point1->v[0] - point2->v[0]) * ratio;
                pIntersection[0] = point1->v[1] + (point1->v[1] - point2->v[1]) * ratio;
                pIntersection[1] = plane_z;
                if (added == 8) {
                    break;
                }
            }
            i++;
            edges += 2;
            pIntersection = pNext_intersection;
        } while (i < count_edges);
    }
    if (pShape->count_points + added > count_points_add_8) {
        added = count_points_add_8 - pShape->count_points;
    }
    if (added > 0) {
        pIntersection = &intersections[1];
        for (j = 0, i = pShape->count_points; j < added; j++, i++) {
            pShape->field_0x18[i].field_0x18.v[0] = pIntersection[-1];
            pShape->field_0x18[i].field_0x18.v[1] = pIntersection[0];
            pShape->field_0x18[i].field_0x18.v[2] = pIntersection[1];
            pIntersection += 3;
        }
    }
    pShape->original_count_points = pShape->count_points;
    pShape->count_points = pShape->count_points + added;
    for (j = 0; j < pShape->count_points; j++) {
        pShape->field_0x18[j].field_0x2c.v[0] = pShape->field_0x18[j].field_0x18.v[0];
        pShape->field_0x18[j].field_0x2c.v[1] = pShape->field_0x18[j].field_0x18.v[1];
        pShape->field_0x18[j].field_0x2c.v[2] = pShape->field_0x18[j].field_0x18.v[2];
    }
    for (j = 0; j < pShape->count_points; j++) {
        BendPoint(&pShape->field_0x18[j].field_0x18, pContext);
    }
}
// Bend-side counterpart of UnBendWheels: rebends the groove centres of the
// front/rear wheels, saves and rebends wpos, then stores the bend angles.
// FUNCTION: CARMA2_HW 0x00435310
void C2_HOOK_FASTCALL BendWheels(tCar_spec* pCar_spec, const tBend_point_context_local* pContext) {
    extern void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar);
    int i;
    br_actor* actor;
    tGroovidelic_spec* groove;
    float angle;
    float fVar2;
    float fVar3;
    float fVar4;
    br_vector3 delta;
    br_vector3 tmp;

    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
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

            pCar_spec->car_crush_spec->field_0x1ec[i].v[0] = centre->v[0];
            pCar_spec->car_crush_spec->field_0x1ec[i].v[1] = centre->v[1];
            pCar_spec->car_crush_spec->field_0x1ec[i].v[2] = centre->v[2];
            delta.v[0] = -centre->v[0];
            delta.v[1] = -centre->v[1];
            delta.v[2] = -centre->v[2];
            BendPoint(centre, pContext);
            centre->v[1] = pCar_spec->car_crush_spec->field_0x1ec[i].v[1];
            delta.v[0] = centre->v[0] + delta.v[0];
            delta.v[1] = centre->v[1] + delta.v[1];
            delta.v[2] = centre->v[2] + delta.v[2];
            PipeSingleVector3(centre, &delta);
        }
    }
    for (i = 0; i < REC2_ASIZE(pCar_spec->wpos); i++) {
        fVar2 = pCar_spec->wpos[i].v[0];
        fVar3 = pCar_spec->wpos[i].v[1];
        fVar4 = pCar_spec->wpos[i].v[2];
        pCar_spec->car_crush_spec->field_0x234[i].v[0] = fVar2;
        pCar_spec->car_crush_spec->field_0x234[i].v[1] = fVar3;
        pCar_spec->car_crush_spec->field_0x234[i].v[2] = fVar4;
        tmp.v[0] = fVar2 * 0.14492753623188406;
        tmp.v[1] = fVar3 * 0.14492753623188406;
        tmp.v[2] = fVar4 * 0.14492753623188406;
        BendPoint(&tmp, pContext);
        pCar_spec->wpos[i].v[0] = tmp.v[0] * 6.9;
        pCar_spec->wpos[i].v[1] = tmp.v[1] * 6.9;
        pCar_spec->wpos[i].v[2] = tmp.v[2] * 6.9;
    }
    SetCarSuspGiveAndHeight(pCar_spec, 1.f, 1.f, 1.f, 0.f, 0.f);
    if (pContext->bend->field_0x18.v[2] == pCar_spec->car_crush_spec->bend_z_min) {
        angle = (tU16)pContext->bend->field_0x3e * 0.005493164;
        if (angle >= 180.f) {
            angle = angle - 360.f;
        }
        pCar_spec->field_0x1260 = angle * 0.03f;
    }
    CalcGraphicalWheelStuff(pCar_spec);
}

// Sets up the bend transform (translate to the pivot, rotate, translate back),
// fills the four offset corner points plus the bend bounding box into the bend
// data, then pipes the actor's compressed vertices through BendModels.
tU16 gCrush_vertex_compression_index;
// FUNCTION: CARMA2_HW 0x00435550
void C2_HOOK_FASTCALL BendActor(tCar_spec* pCar, br_actor* pActor, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg6, float pArg7, tBend_data_local* pBend) {
    br_vector3 transformed;
    struct {
        tCar_spec* car;
        br_matrix34 matrix;
        tBend_data_local* bend;
    } context;
    float* pBounds;

    context.bend = pBend;
    pBounds = (float*)pBend;
    BrMatrix34Translate(&context.matrix, -pArg6->v[0], -pArg6->v[1], -pArg6->v[2]);
    if (pAngle_y != 0) {
        BrMatrix34PostRotateY(&context.matrix, pAngle_y);
    }
    if (pAngle_x != 0) {
        BrMatrix34PostRotateX(&context.matrix, pAngle_x);
    }
    if (pAngle_z != 0) {
        BrMatrix34PostRotateZ(&context.matrix, pAngle_z);
    }
    BrMatrix34PostTranslate(&context.matrix, pArg6->v[0], pArg6->v[1], pArg6->v[2]);
    BrActorToBounds((br_bounds*)pBend, pActor);
    pBounds[0x11] = pBounds[3];
    pBounds[0x12] = pBounds[4];
    pBounds[0x13] = pArg6->v[2];
    pBounds[0x14] = pBounds[3];
    pBounds[0x15] = pBounds[1];
    pBounds[0x16] = pArg6->v[2];
    pBounds[0x17] = pBounds[0];
    pBounds[0x18] = pBounds[4];
    pBounds[0x19] = pArg6->v[2];
    pBounds[0x1a] = pBounds[0];
    pBounds[0x1b] = pBounds[1];
    pBounds[0x1c] = pArg6->v[2];
    pBounds[9] = pBounds[0];
    pBounds[10] = pBounds[1];
    pBounds[0xb] = pArg7;
    pBounds[0xc] = pBounds[0x11] - pBounds[0];
    pBounds[0xd] = pBounds[0x12] - pBounds[1];
    pBounds[0xe] = pBounds[0x13] - pArg7;
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x11], &context.matrix);
    pBounds[0x11] = transformed.v[0] - pBounds[0x11];
    pBounds[0x12] = transformed.v[1] - pBounds[0x12];
    pBounds[0x13] = transformed.v[2] - pBounds[0x13];
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x14], &context.matrix);
    pBounds[0x14] = transformed.v[0] - pBounds[0x14];
    pBounds[0x15] = transformed.v[1] - pBounds[0x15];
    pBounds[0x16] = transformed.v[2] - pBounds[0x16];
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x17], &context.matrix);
    pBounds[0x17] = transformed.v[0] - pBounds[0x17];
    pBounds[0x18] = transformed.v[1] - pBounds[0x18];
    pBounds[0x19] = transformed.v[2] - pBounds[0x19];
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x1a], &context.matrix);
    pBounds[0x1a] = transformed.v[0] - pBounds[0x1a];
    pBounds[0x1b] = transformed.v[1] - pBounds[0x1b];
    pBounds[0x1c] = transformed.v[2] - pBounds[0x1c];
    pBounds[0x11] = ((pBounds[0x11] - pBounds[0x14]) - pBounds[0x17]) + pBounds[0x1a];
    pBounds[0x12] = ((pBounds[0x12] - pBounds[0x15]) - pBounds[0x18]) + pBounds[0x1b];
    pBounds[0x13] = ((pBounds[0x13] - pBounds[0x16]) - pBounds[0x19]) + pBounds[0x1c];
    pBounds[0x14] = pBounds[0x14] - pBounds[0x1a];
    pBounds[0x15] = pBounds[0x15] - pBounds[0x1b];
    pBounds[0x16] = pBounds[0x16] - pBounds[0x1c];
    pBounds[0x17] = pBounds[0x17] - pBounds[0x1a];
    pBounds[0x18] = pBounds[0x18] - pBounds[0x1b];
    pBounds[0x19] = pBounds[0x19] - pBounds[0x1c];
    pBend->field_0x74 = pArg6->v[2] < pArg7;
    pBend->field_0x18.v[0] = pArg6->v[0];
    pBend->field_0x18.v[1] = pArg6->v[1];
    pBend->field_0x18.v[2] = pArg6->v[2];
    gCrush_vertex_compression_index = 0;
    context.car = pCar;
    ARStartPipingSession(ePipe_model_mash);
    DRActorEnumRecurseWithTranslation(pActor, NULL, (tDRActorEnumRecurseWithTranslation_cbfn*)BendModels, &context.car);
    AREndPipingSession();
    pBend->field_0x3c = pAngle_x;
    pBend->field_0x3e = pAngle_y;
    pBend->field_0x40 = pAngle_z;
}

// Per-actor callback of the bend piping pass (DRActorEnumRecurseWithTranslation).
// Leaves the wheel/pivot actors alone; disables the groove on the remaining
// actors, unbends every detail-level model and flags the crush data.
// FUNCTION: CARMA2_HW 0x004358a0
intptr_t C2_HOOK_FASTCALL BendModels(br_actor* pActor, br_vector3* pDelta, void* pContext) {
    tUser_crush_data* user_crush_data;
    tCar_spec** ppCar;
    br_actor** piVar2;
    int i;

    user_crush_data = pActor->user;
    if (user_crush_data != NULL) {
        ppCar = pContext;
        for (i = 0, piVar2 = (*ppCar)->wheel_actors; i < 6; i++, piVar2++) {
            if (pActor == piVar2[6]) {
                return 0;
            }
            if (pActor == piVar2[0]) {
                return 0;
            }
        }
        if (user_crush_data->groove != NULL) {
            PipeSingleGrooveOnOff(0, pActor, 3);
            DisableGroovidelic(pActor, 3);
            PipeSingleTransformType(pActor, 5);
            pActor->t.type = 5;
        }
        if (user_crush_data->crush_data != NULL) {
            i = 0;
            if (0 < (*ppCar)->count_detail_levels) {
                do {
                    BendModel(((void**)user_crush_data)[i], pDelta, ppCar, pActor);
                    i++;
                } while (i < (*ppCar)->count_detail_levels);
            }
            user_crush_data->crush_data->field_0x2c = 1;
        }
    }
    return 0;
}
// GLOBAL: CARMA2_HW 0x0067b83c

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00435960
void C2_HOOK_FASTCALL BendModel(br_model* pModel, const br_vector3* pOffset, void* pContext, br_actor* pActor) {
    br_vector3 mash[1024];
    tUser_detail_level_model* detail_model;
    tModel_detail_vertex_data* vertex_data;
    tBend_data_local* bend;
    br_vertex* vertex;
    br_vector3 pos;
    br_vector3 tmp;
    br_vector3 d;
    br_vector3 delta;
    float a;
    float b;
    float c;
    float ab;
    int changed;
    tU16 i;

    if (pModel == NULL) {
        return;
    }
    detail_model = pModel->user;
    if (detail_model == NULL) {
        return;
    }
    vertex_data = detail_model->field_0x4;
    if (vertex_data == NULL) {
        return;
    }
    if (pModel->nvertices > REC2_ASIZE(mash)) {
        PDFatalError("Alas, there's a lot of vertices on that model.");
    }
    bend = ((const tBend_point_context_local*)pContext)->bend;
    *(tU16*)&detail_model->field_0x8[0] = gCrush_vertex_compression_index;
    changed = 0;
    for (i = 0; i < pModel->nvertices; i++) {
        vertex = &pModel->vertices[i];
        pos.v[0] = vertex->p.v[0] + pOffset->v[0];
        pos.v[1] = pOffset->v[1] + vertex->p.v[1];
        pos.v[2] = pOffset->v[2] + vertex->p.v[2];
        if ((bend->field_0x74 != 0 && pos.v[2] - bend->field_0x18.v[2] <= gCrushZero)
                || (bend->field_0x74 == 0 && pos.v[2] - bend->field_0x18.v[2] >= gCrushZero)) {
            BrMatrix34ApplyP(&tmp, &pos, &((const tBend_point_context_local*)pContext)->matrix);
            mash[i].v[0] = -vertex->p.v[0];
            mash[i].v[1] = -vertex->p.v[1];
            mash[i].v[2] = -vertex->p.v[2];
            vertex->p.v[0] = tmp.v[0] - pOffset->v[0];
            vertex->p.v[1] = tmp.v[1] - pOffset->v[1];
            vertex->p.v[2] = tmp.v[2] - pOffset->v[2];
            mash[i].v[0] += vertex->p.v[0];
            mash[i].v[1] += vertex->p.v[1];
            mash[i].v[2] += vertex->p.v[2];
            vertex_data[i].flags |= 0x18;
            changed = 1;
        } else {
            d.v[0] = pos.v[0] - bend->field_0x24.v[0];
            d.v[1] = pos.v[1] - bend->field_0x24.v[1];
            d.v[2] = pos.v[2] - bend->field_0x24.v[2];
            if ((bend->field_0x74 != 0 && d.v[2] > gCrushZero)
                    || (bend->field_0x74 == 0 && d.v[2] < gCrushZero)) {
                mash[i].v[0] = 0.f;
                mash[i].v[1] = 0.f;
                mash[i].v[2] = 0.f;
                vertex_data[i].flags &= ~0x8;
            } else {
                vertex_data[i].flags = (vertex_data[i].flags & ~0x10) | 0x8;
                CompressVector3((tCompressed_vector3*)&bend->field_0x78[gCrush_vertex_compression_index], &pos, -10.f, 10.f);
                gCrush_vertex_compression_index++;
                a = d.v[0] / bend->field_0x30.v[0];
                b = d.v[1] / bend->field_0x30.v[1];
                c = d.v[2] / bend->field_0x30.v[2];
                ab = a * b;
                delta.v[0] = (bend->field_0x68.v[0] + bend->field_0x5c.v[0] * b + bend->field_0x50.v[0] * a + bend->field_0x44.v[0] * ab) * c;
                delta.v[1] = (bend->field_0x68.v[1] + bend->field_0x5c.v[1] * b + bend->field_0x50.v[1] * a + bend->field_0x44.v[1] * ab) * c;
                delta.v[2] = (bend->field_0x68.v[2] + bend->field_0x5c.v[2] * b + bend->field_0x50.v[2] * a + bend->field_0x44.v[2] * ab) * c;
                pos.v[0] += delta.v[0];
                pos.v[1] += delta.v[1];
                pos.v[2] += delta.v[2];
                mash[i].v[0] = delta.v[0];
                mash[i].v[1] = delta.v[1];
                mash[i].v[2] = delta.v[2];
                vertex->p.v[0] = pos.v[0] - pOffset->v[0];
                vertex->p.v[1] = pos.v[1] - pOffset->v[1];
                vertex->p.v[2] = pos.v[2] - pOffset->v[2];
                changed = 1;
            }
        }
    }
    if (changed != 0) {
        if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
            detail_model->field_0x0 = 1;
        } else {
            pModel->custom = JitModelUpdate;
            pModel->flags |= BR_MODF_CUSTOM;
        }
        AddModelMashToPipingSession(pModel, mash);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

// DRActorEnumRecurseUltraPlus

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

/* DoSpams/DoCrushing/DoDroneCrushing/DoShapeSwapping/ClearCrushLists are
   inlined into their only caller CrushBendFlapRend (0x436170). */
extern tCar_damge_crush_list_item gCar_damage_crush_list[8];
extern int gCount_car_damage_crush_list;
extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);

// PARTIAL 80.51%: CrushBendFlapRend 0x00436170 - register roles in the crush-list loops (original: list in ebp, item pointer in esi spilled to [esp+0x14]; ours: list in esi spilled, item pointer in ebp) and the drone-scale x87 schedule (original loads the scalar once, then v2/v0/v1 with fxch/fmulp; ours reloads the scalar per component)
// FUNCTION: CARMA2_HW 0x00436170
void C2_HOOK_FASTCALL CrushBendFlapRend(void) {
    tCompressed_car_crush compressed;
    br_vector3 tv;
    double factor;
    tCrush_list_item* crush_list;

    tQueued_drone_crush* queued_crush;
    tQueued_drone_crush_item* drone_item;
    float force;
    int i;
    int drone_i;
    int drone_j;

    for (i = 0; i < gCount_car_damage_crush_list; i++) {
        TotallySpamTheCar(gCar_damage_crush_list[i].car, gCar_damage_crush_list[i].damage);
    }
    gCount_car_damage_crush_list = 0;

    for (crush_list = gCrush_lists; crush_list->car_spec != NULL; crush_list++) {
        for (i = 0; i < crush_list->count; i++) {
            force = crush_list->car_spec->car_crush_spec->bendability_factor * crush_list->items[i].field_0x40;
            if (crush_list->car_spec == NULL || crush_list->car_spec->driver != eDriver_local_human) {
                factor = (gDouble_00589a60 - (float)gCurrent_race.index / gNumber_of_races) * gDouble_00589b18;
                force = force * (gDouble_00589a60 - factor * gDouble_00589b20);
            }
            if (force > gMin_bend_force) {
                if (PercentageChance((int)gChance_of_bending)) {
                    crush_list->items[i].field_0x40 = -crush_list->items[i].field_0x40;
                }
            } else {
                tNet_message_chunk* message;

                if (gNet_mode != eNet_mode_none) {
                    CompressCrush(&compressed, &crush_list->items[i]);
                    message = NetStartBroadcastContents(0x20, 0);
                    if (message != NULL) {
                        message->car_crush.car_id = NetPlayerFromCar(crush_list->car_spec)->ID;
                        memcpy(&message->car_crush.car_crush, &compressed, sizeof(tCompressed_car_crush));
                        NetBroadcastContents(message);
                    }
                    ExpandCrush(&crush_list->items[i], &compressed);
                }
                CrushCar(crush_list->car_spec, crush_list->car_spec->car_master_actor, &crush_list->items[i]);
            }
        }
    }

    for (drone_i = 0; drone_i < gCount_queued_drone_crushes; drone_i++) {
        queued_crush = &gQueued_drone_crushes[drone_i];
        for (drone_j = 0; drone_j < queued_crush->count; drone_j++) {
            drone_item = &queued_crush->items[drone_j];
            if (drone_item->field_0x34 > gFloat_00589b28) {
                drone_item->field_0x34 = 5.f;
            }
            BrVector3Scale(&tv, &drone_item->field_0x38, drone_item->field_0x34);
            CrushDroneObject((tPhysics_object*)queued_crush->field_0x0, drone_item->count, (float*)drone_item->field_0x4, &tv);
        }
    }
    gCount_queued_drone_crushes = 0;

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car = gActive_car_list[i];

        if (car != NULL && car->driver > 5 && car->car_crush_spec != NULL
                && (car->car_crush_spec->expand_bounding_box & 1)) {
            SwapShapesIfPossible(car);
        }
    }

    DoFlapping();
    DoDetaching();
    DoFullyDetaching();
    DoBending();

    for (crush_list = gCrush_lists; crush_list->car_spec != NULL; crush_list++) {
        crush_list->car_spec = NULL;
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

// PARTIAL 42.93%: CrushCar - body taken from the disassembly of 0x004363f0; the residual diff is x87
// scheduling / register allocation and stack-slot assignment (our frame 0x4c vs the original 0x58, the
// original keeps direction.v[1] and the BendFactor result in memory) plus NearestAxis, which the original
// inlines before the limits search but calls inside the vertex loop, while MSVC inlines our call at both.
// FUNCTION: CARMA2_HW 0x004363f0
void C2_HOOK_FASTCALL CrushCar(tCar_spec* pCar, br_actor* pActor, tCar_crush* pCrush) {
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    } context;
    tCar_crush_spec* crush_spec;
    tCar_crush_limits* limits;
    tCar_crush_shape_info* shape;
    tCar_crush_reordered_shape_info* entry;
    br_vector3* pVertex;
    br_vector3 direction;
    float magnitude;
    float bend;
    float factor;
    float force;
    int count;
    int axis;
    int flip;
    int found;
    int i;
    int j;

    crush_spec = pCar->car_crush_spec;
    if (crush_spec == NULL || pCar->invulnerable_no_crushage != 0 || crush_spec->field_0x4b8 != 0) {
        return;
    }
    context.car = pCar;
    context.crush = pCrush;
    context.force = pCrush->field_0x40;
    if (context.force > gMax_crush_force) {
        context.force = gMax_crush_force;
    }
    context.force = context.force / (float)pCrush->count;
    if (crush_spec->field_0x144 != 0) {
        ARStartPipingSession(3);
        DRActorEnumRecurseWithSnart(pCar->car_model_actor, NULL, SimpleCrushCarModelsCB, &context);
        AREndPipingSession();
        return;
    }
    if (pActor == pCar->car_master_actor) {
        direction.v[0] = pCrush->field_0x44.v[0] * context.force;
        direction.v[1] = pCrush->field_0x44.v[1] * context.force;
        direction.v[2] = pCrush->field_0x44.v[2] * context.force;
        direction.v[0] = gCrushZero;
        if (direction.v[1] > gCrushZero) {
            direction.v[1] = gCrushZero;
        }
        magnitude = (float)sqrt(direction.v[1] * direction.v[1] + direction.v[2] * direction.v[2]);
        if (magnitude >= 1e-05f) {
            direction.v[0] = gCrushZero / magnitude;
            direction.v[1] = direction.v[1] / magnitude;
            direction.v[2] = direction.v[2] / magnitude;
            axis = NearestAxis(&direction);
            flip = (direction.v[axis] > gCrushZero) ? 1 : 0;
            found = 0;
            count = crush_spec->field_0xa4.counts[axis][flip];
            limits = &crush_spec->field_0xbc;
            if (count > 0) {
                i = 0;
                do {
                    if (flip == 0) {
                        if (limits->limits[axis][0].values[i] < pCrush->field_0x34.v[axis]) {
                            found = i;
                            break;
                        }
                    } else {
                        if (pCrush->field_0x34.v[axis] < limits->limits[axis][1].values[i]) {
                            found = i;
                            break;
                        }
                    }
                    i++;
                } while (i < count);
            }
            if (crush_spec->count_shapes > 0) {
                i = 0;
                do {
                    shape = &crush_spec->field_0x4[i];
                    if (shape->field_0x8 == 0) {
                        for (j = 0; j < shape->count_points; j++) {
                            entry = &shape->field_0x18[j];
                            pVertex = &entry->field_0x18;
                            bend = entry->field_0x24 * magnitude;
                            factor = GetImpactScale(pVertex, pCrush->count, pCrush->vectors, gMax_crush_dist_sq);
                            force = gForce_to_movement_factor * bend;
                            if (factor != gCrushZero) {
                                force *= factor;
                                axis = NearestAxis(&direction);
                                LimitImpactScale(&force, axis, pVertex->v[axis], direction.v[axis], entry->field_0x28, limits, found);
                                if (force != gCrushZero) {
                                    pVertex->v[1] = direction.v[1] * force + pVertex->v[1];
                                    pVertex->v[2] = direction.v[2] * force + pVertex->v[2];
                                    pVertex->v[0] = direction.v[0] * force + pVertex->v[0];
                                }
                            }
                            if (DRVector3TestForNan(pVertex) != 0) {
                                PDEnterDebugger("NaN");
                            }
                            ClipPointToLimits(pVertex, entry->field_0x28, limits);
                        }
                    }
                    i++;
                } while (i < crush_spec->count_shapes);
            }
            crush_spec->expand_bounding_box = 7;
        }
    }
    ARStartPipingSession(3);
    DRActorEnumRecurseWithSnart(pActor, NULL, CrushModels, &context);
    AREndPipingSession();
    pCar->use_shell_model = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004367a0
void C2_HOOK_FASTCALL LimitImpactScale(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, tCar_crush_limits* pLimits, int pFound) {
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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
extern const br_scalar gFLOAT_00589a54;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00436850
float C2_HOOK_FASTCALL GetImpactScale(br_vector3* pVertexPos, int pCount, br_vector3* pVectors, br_scalar pMaxDistSq) {
    float dist_sq;

    dist_sq = DistSq(pVertexPos, pCount, pVectors);
    if (dist_sq > gMax_crush_dist_sq) {
        return gCrushZero;
    }
    return gFLOAT_00589a54 - (float)sqrt(dist_sq / pMaxDistSq);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
extern br_scalar gCrushSqrtHalf;

// FUNCTION: CARMA2_HW 0x00436890
int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector) {

    if ((float)fabs(pVector->v[0]) > gCrushSqrtHalf) {
        return 0;
    } else if ((float)fabs(pVector->v[1]) > gCrushSqrtHalf) {
        return 1;
    } else {
        return 2;
    }
}

// SendCrush

// FUNCTION: CARMA2_HW 0x004368d0
void C2_HOOK_FASTCALL ClipPointToLimits(br_vector3* pVertexPos, tU16 pFlags, tCar_crush_limits* pLimits) {
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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

// FUNCTION: CARMA2_HW 0x00436d60
tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    extern void C2_HOOK_FASTCALL PipeSingleRelink(tU32 pArg1, br_actor* pActor);
    extern intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);
    extern int gNum_active_non_cars;
    extern tNon_car_spec* gActive_non_car_list[99];
    extern tWorld_callbacks gWorld_callbacks;
    extern tU16 gNext_crush_actor_id;

    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_flap_data* flap_data;
    tCar_bit_spec* bit_car;
    tPhysics_object* bit_object;
    tPhysics_object* object;
    tPhysics_object* next;
    br_actor* child_actor;
    tCrush_info* child_info;
    tCar_spec* child_car;
    tCar_crush_spec* child_crush_spec;
    tNon_car_spec** non_car_p;
    tCrush_info* info;
    tCrush_info* candidate;
    tPhysics_joint** joint_p;
    br_model* model;
    br_material* material;
    br_vector3 centre;
    int start_index;
    int count;
    int i;
    int j;
    int k;
    int index;
    int flag;
    int zero;

    *pArg4 = 0;
    if (*(int*)((char*)pCar + 0x1b4) != 0) {
        goto fail;
    }
    if (pCar->car_crush_spec == NULL) {
        goto fail;
    }
    if (pCar->car_crush_spec->field_0x4b8 != 0) {
        goto fail;
    }
    user = pActor->user;
    if (user == NULL) {
        goto fail;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        goto fail;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == NULL) {
        goto fail;
    }
    if (detach_data->field_0x28 != 0) {
        goto fail;
    }
    if (pCar->car_crush_spec->field_0x270 == 3) {
        goto fail;
    }
    i = 0;
    joint_p = gTrack_crush_joints;
    while (joint_p < &gTrack_crush_joints[32]) {
        if ((*joint_p)->type == eJoint_none) {
            break;
        }
        joint_p++;
        i++;
    }
    if (i == 32) {
        goto fail;
    }
    count = gDetached_bit_crush_info_buffer.capacity;
    for (index = 0; index < count; index++) {
        if (*(tDriver*)&gDetached_bit_crush_info_buffer.crush_infos[index].field_0xc == eDriver_non_car_unused_slot) {
            break;
        }
    }
    zero = 0;
    if (index == count) {
        if (gCrush_deferred != 0) {
            bit_object = NULL;
            goto bit_found;
        }
        start_index = (int)gDetached_bit_crush_info_buffer.field_0x8;
        for (j = 0; j < count; j++) {
            candidate = &gDetached_bit_crush_info_buffer.crush_infos[start_index];
            if (*(tDriver*)&candidate->field_0xc != 1) {
                break;
            }
            start_index = (start_index + 1) % count;
        }
        if (j == count) {
            bit_object = NULL;
            goto bit_found;
        }
        info = &gDetached_bit_crush_info_buffer.crush_infos[start_index];
        gDetached_bit_crush_info_buffer.field_0x8 = ((int)gDetached_bit_crush_info_buffer.field_0x8 + 1) % count;
        (*(br_actor**)((tU8*)info + 0x10))->type_data = NULL;
        PipeSingleNonCar(*(tU16*)((tU8*)info + 0x80), *(br_actor**)((tU8*)info + 0x10));
        PipeSingleRelink(0, *(br_actor**)((tU8*)info + 0x10));
        BrActorRemove(*(br_actor**)((tU8*)info + 0x10));
        object = info->collision_object->child;
        while (object != (tPhysics_object*)zero) {
            next = object->next;
            child_actor = object->actor;
            child_info = child_actor->type_data;
            child_actor->type_data = (void*)zero;
            child_car = *(tCar_spec**)(*(char**)(*(char**)((char*)child_actor->user + 8) + 0x34) + 0x2c);
            child_crush_spec = child_car->car_crush_spec;
            if (*(int*)((tU8*)child_crush_spec + 0x494) != 8) {
                PipeSingleRelink(0, child_actor);
                BrActorRemove(child_actor);
                *(br_actor**)((tU8*)child_crush_spec + 0x498 + *(int*)((tU8*)child_crush_spec + 0x494) * 4) = child_actor;
                (*(int*)((tU8*)child_crush_spec + 0x494))++;
            }
            *(tDriver*)&child_info->field_0xc = eDriver_non_car_unused_slot;
            PhysicsRemoveObject(object);
            object = next;
        }
        j = 0;
        if (gNum_active_non_cars > 0) {
            non_car_p = gActive_non_car_list;
            do {
                if (*non_car_p == (tNon_car_spec*)info) {
                    break;
                }
                j++;
                non_car_p++;
            } while (j < gNum_active_non_cars);
        }
        if (j != gNum_active_non_cars) {
            gNum_active_non_cars = gNum_active_non_cars - 1;
            if (j < gNum_active_non_cars) {
                for (k = j; k < gNum_active_non_cars; k++) {
                    gActive_non_car_list[k] = gActive_non_car_list[k + 1];
                }
            }
        }
        *(tDriver*)&info->field_0xc = eDriver_non_car_unused_slot;
        (*(br_actor**)((tU8*)info + 0x10))->type_data = (void*)zero;
        PHILRemoveObject(info->collision_object);
    } else {
        info = &gDetached_bit_crush_info_buffer.crush_infos[index];
    }
    pActor->type_data = info;
    *(tDriver*)&info->field_0xc = 1;
    info->collision_object->actor = pActor;
    *(br_actor**)((tU8*)info + 0x10) = pActor;
    *(undefined4*)((tU8*)info + 0x4) = (undefined4)zero;
    *(float*)((tU8*)info + 0xc4) = -1.0f;
    if (gNet_mode == eNet_mode_none) {
        if (gNext_crush_actor_id == 20000) {
            gNext_crush_actor_id = 10000;
        }
        *(tU16*)((tU8*)info + 0x80) = gNext_crush_actor_id++;
    } else {
        *(tU16*)((tU8*)info + 0x80) = pCar->car_ID;
    }
    *(tCar_spec**)((tU8*)info + 0xc8) = pCar;
    info->collision_object->owner = info;
    info->collision_object->flags_0x238 = 1;
    info->collision_object->field_0x1a0 = 0xffff;
    info->collision_object->field_0x1a4 = 0;
    bit_object = info->collision_object;
bit_found:
    if (bit_object == NULL) {
        goto fail;
    }
    bit_object->physics_joint1 = gTrack_crush_joints[i];
    *(float*)((tU8*)detach_data + 0xc) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[0];
    *(float*)((tU8*)detach_data + 0x10) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[1];
    *(float*)((tU8*)detach_data + 0x14) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[2];
    *(float*)((tU8*)detach_data + 0x18) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[0];
    *(float*)((tU8*)detach_data + 0x1c) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[1];
    *(float*)((tU8*)detach_data + 0x20) = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[2];
    PhysicsAddObject(pCar->collision_info, bit_object);
    detach_data->field_0x28 = 1;
    bit_car = &pCar->car_crush_spec->field_0x274[pCar->car_crush_spec->field_0x270];
    bit_car->field_0x0 = pActor;
    bit_car->field_0x4 = pActor->parent;
    bit_car->field_0x8 = bit_object;
    bit_car->field_0xc = (undefined4*)detach_data;
    bit_car->field_0x10 = PDGetTotalTime();
    pCar->car_crush_spec->field_0x270++;
    PipeSingleIdentity(pActor);
    PipeSingleRelink((tU32)gNon_track_actor, pActor);
    ActorRelink(gNon_track_actor, pActor);
    BrMatrix34Copy(&bit_object->transform_matrix, &pActor->t.t.mat);
    PipeSingleNonCar(*(tU16*)((tU8*)pActor->type_data + 0x80), bit_object->actor);
    SetUpDetachedBitObject(bit_object, pActor, user->models[0], detach_data, pCar->car_crush_spec, pArg6, pCar->collision_info);
    flap_data = crush_data->flap_data;
    if (flap_data == NULL) {
        SetUpSemiDetachedBitJoint(bit_object->physics_joint1, detach_data, 0, user->models[0], bit_object, pArg5);
    } else {
        SetUpFlapJoint(bit_object->physics_joint1, flap_data, user->models[0], bit_object, pArg5, pArg7, pArg8);
        *(tPhysics_object**)((tU8*)flap_data + 0x24) = bit_object;
    }
    if (pArg3 != gCrushZero) {
        centre.v[0] = bit_object->physics_joint1->hinge_axis.v[0] * pArg3;
        centre.v[1] = bit_object->physics_joint1->hinge_axis.v[1] * pArg3;
        centre.v[2] = bit_object->physics_joint1->hinge_axis.v[2] * pArg3;
        bit_object->omega.v[0] += centre.v[0];
        bit_object->omega.v[1] += centre.v[1];
        bit_object->omega.v[2] += centre.v[2];
    }
    if (user->groove == NULL) {
        model = user->models[0];
        if (model != NULL) {
            for (j = 0; j < model->prepared->ngroups; j++) {
                material = model->faces[*model->prepared->groups[j].face_user].material;
                if (material != NULL && (material->flags & BR_MATF_TWO_SIDED) == 0) {
                    material->flags |= BR_MATF_TWO_SIDED;
                    BrMaterialUpdate(material, BR_MATU_RENDERING);
                }
            }
        }
        PipeSingleDSModel(1, user->models[0]);
    }
    flag = 0;
    SwitchCarModel(pActor, &flag);
    if (TestForObjectInSensiblePlace(bit_object, gPHIL_list_collision_infos, &centre, &gWorld_callbacks) == 0) {
        ReAttachBit(pCar, bit_car->field_0x4, pActor, bit_object);
        pCar->car_crush_spec->field_0x270--;
        *pArg4 = 1;
        return NULL;
    }
    if (pArg6 != NULL) {
        centre.v[0] = (pArg6->max.v[0] + pArg6->min.v[0]) * 0.5f;
        centre.v[1] = (pArg6->max.v[1] + pArg6->min.v[1]) * 0.5f;
        centre.v[2] = (pArg6->max.v[2] + pArg6->min.v[2]) * 0.5f;
        LazyMoveModel(user->models[0], &centre);
    }
    *(int*)((tU8*)pCar + 0xe24) = 0;
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        flap_data->field_0x0 = 1;
    }
    if (gNet_mode != eNet_mode_none) {
        CompressVector3((tCompressed_vector3*)((tU8*)detach_data + 0x7c), &bit_object->bb1.min, -10.f, 10.f);
        CompressVector3((tCompressed_vector3*)((tU8*)detach_data + 0x82), &bit_object->bb1.max, -10.f, 10.f);
    }
    if (user->groove != NULL) {
        DisableGroovidelic(pActor, 3);
        PipeSingleGrooveOnOff(0, pActor, 3);
    }
    return bit_object;
fail:
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00437480
void C2_HOOK_FASTCALL SetUpSemiDetachedBitJoint(tPhysics_joint* pJoint, tCar_crush_detach_data* pDetach_data, int pVertex_index1, br_model* pModel, tPhysics_object* pObject, const br_vector3* pOptional_pos) {
    br_vector3 local_c;
    br_scalar fVar3;
    br_scalar fVar4;
    br_scalar fVar5;
    int iVar8;
    int* puVar6;
    br_scalar uVar7;
    tU32 uVar9;
    tU32 uVar10;
    tU8 bVar1;
    tU8 bVar2;

    pJoint->type = 3;
    if (pOptional_pos != NULL) {
        const int* pos = (const int*)pOptional_pos;
        pJoint->field_0x08.v[0] = *(br_scalar*)&pos[0];
        pJoint->field_0x08.v[1] = *(br_scalar*)&pos[1];
        uVar7 = *(br_scalar*)&pos[2];
    } else {
        int* crush_data_ptr = (int*)pModel->vertices;
        tU32 idx;
        if (pVertex_index1 != 0) {
            tU32 idx = (tU32)*(tU16*)(pVertex_index1 + 10);
            pJoint->field_0x08.v[0] = *(br_scalar*)(crush_data_ptr + idx * 0xa);
            pJoint->field_0x08.v[1] = *(br_scalar*)(crush_data_ptr + idx * 0xa + 1);
            pJoint->field_0x08.v[2] = *(br_scalar*)(crush_data_ptr + idx * 0xa + 2);
            goto LAB_0043752a;
        }
        idx = (tU32)*(tU16*)((tU8*)pDetach_data + 0x30);
        pJoint->field_0x08.v[0] = *(br_scalar*)(crush_data_ptr + idx * 0xa);
        pJoint->field_0x08.v[1] = *(br_scalar*)(crush_data_ptr + idx * 0xa + 1);
        uVar7 = *(br_scalar*)(crush_data_ptr + idx * 0xa + 2);
    }
    pJoint->field_0x08.v[2] = uVar7;
LAB_0043752a:
    BrMatrix34ApplyP(&local_c, &pJoint->field_0x08, &pObject->actor->t.t.mat);
    DrMatrix34ApplyLPInverse(&pJoint->field_0x14, &local_c, &pObject->parent->actor->t.t.mat);
    pJoint->count_limits = 2;
    iVar8 = 0;
    puVar6 = (int*)(&pJoint->limits[0].value);
    do {
        puVar6[-1] = 0;
        *puVar6 = 0;
        iVar8 = iVar8 + 1;
        puVar6 = puVar6 + 8;
    } while (iVar8 < pJoint->count_limits);
    if (pVertex_index1 != 0) {
        int* crush_data_ptr = (int*)pModel->vertices;
        tU32 idx1 = (tU32)*(tU16*)(pVertex_index1 + 0xe);
        tU32 idx2 = (tU32)*(tU16*)(pVertex_index1 + 10);
        tU32 idx3;
        pJoint->limits[0].child.v[0] = *(br_scalar*)(crush_data_ptr + idx1 * 0xa) - *(br_scalar*)(crush_data_ptr + idx2 * 0xa);
        pJoint->limits[0].child.v[1] = *(br_scalar*)(crush_data_ptr + idx1 * 0xa + 1) - *(br_scalar*)(crush_data_ptr + idx2 * 0xa + 1);
        fVar3 = *(br_scalar*)(crush_data_ptr + idx1 * 0xa + 2) - *(br_scalar*)(crush_data_ptr + idx2 * 0xa + 2);
        pJoint->limits[0].child.v[2] = fVar3;
        fVar3 = sqrt(fVar3 * fVar3 +
              pJoint->limits[0].child.v[1] * pJoint->limits[0].child.v[1] +
              pJoint->limits[0].child.v[0] * pJoint->limits[0].child.v[0]);
        if (fVar3 <= gFLT_EPSILON_00589a5c) {
            pJoint->limits[0].child.v[0] = gFLOAT_00589a54;
            pJoint->limits[0].child.v[1] = 0;
            pJoint->limits[0].child.v[2] = 0;
        } else {
            fVar3 = 1.0f / fVar3;
            pJoint->limits[0].child.v[0] = pJoint->limits[0].child.v[0] * fVar3;
            pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] * fVar3;
            pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] * fVar3;
        }
        idx3 = (tU32)*(tU16*)(pVertex_index1 + 0x10);
        pJoint->limits[1].child.v[0] = *(br_scalar*)(crush_data_ptr + idx3 * 0xa) - *(br_scalar*)(crush_data_ptr + idx2 * 0xa);
        pJoint->limits[1].child.v[1] = *(br_scalar*)(crush_data_ptr + idx3 * 0xa + 1) - *(br_scalar*)(crush_data_ptr + idx2 * 0xa + 1);
        fVar3 = *(br_scalar*)(crush_data_ptr + idx3 * 0xa + 2) - *(br_scalar*)(crush_data_ptr + idx2 * 0xa + 2);
        pJoint->limits[1].child.v[2] = fVar3;
        fVar3 = sqrt(fVar3 * fVar3 +
              pJoint->limits[1].child.v[1] * pJoint->limits[1].child.v[1] +
              pJoint->limits[1].child.v[0] * pJoint->limits[1].child.v[0]);
        if (fVar3 <= gFLT_EPSILON_00589a5c) {
            pJoint->limits[1].child.v[0] = gFLOAT_00589a54;
            pJoint->limits[1].child.v[1] = 0;
            pJoint->limits[1].child.v[2] = 0;
        } else {
            fVar3 = 1.0f / fVar3;
            pJoint->limits[1].child.v[0] = pJoint->limits[1].child.v[0] * fVar3;
            pJoint->limits[1].child.v[1] = pJoint->limits[1].child.v[1] * fVar3;
            pJoint->limits[1].child.v[2] = pJoint->limits[1].child.v[2] * fVar3;
        }
        pJoint->limits[0].parent.v[0] = pJoint->limits[1].child.v[2] * pJoint->limits[0].child.v[1] -
               pJoint->limits[1].child.v[1] * pJoint->limits[0].child.v[2];
        fVar3 = pJoint->limits[0].child.v[2] * pJoint->limits[1].child.v[0] - pJoint->limits[0].child.v[0] * pJoint->limits[1].child.v[2];
        pJoint->limits[0].parent.v[1] = fVar3;
        pJoint->limits[0].parent.v[2] = pJoint->limits[0].child.v[0] * 0.01f +
               (pJoint->limits[0].child.v[1] * pJoint->limits[1].child.v[1] -
               pJoint->limits[0].child.v[0] * pJoint->limits[1].child.v[0]);
        fVar4 = pJoint->limits[1].child.v[0] * 0.01f + pJoint->limits[0].child.v[0] * 0.01f + pJoint->limits[0].parent.v[0];
        fVar3 = pJoint->limits[1].child.v[1] * 0.01f + pJoint->limits[0].child.v[1] * 0.01f + fVar3;
        fVar5 = pJoint->limits[1].child.v[2] * 0.01f + pJoint->limits[0].parent.v[2];
        pJoint->limits[0].parent.v[0] = fVar4;
        pJoint->limits[0].parent.v[1] = fVar3;
        pJoint->limits[0].parent.v[2] = fVar5;
        fVar5 = sqrt(fVar5 * fVar5 + fVar3 * fVar3 + fVar4 * fVar4);
        if (fVar5 <= gFLT_EPSILON_00589a5c) {
            pJoint->limits[0].parent.v[0] = gFLOAT_00589a54;
            pJoint->limits[0].parent.v[1] = 0;
            pJoint->limits[0].parent.v[2] = 0;
        } else {
            fVar5 = 1.0f / fVar5;
            pJoint->limits[0].parent.v[0] = fVar5 * fVar4;
            pJoint->limits[0].parent.v[1] = fVar5 * fVar3;
            pJoint->limits[0].parent.v[2] = pJoint->limits[0].parent.v[2] * fVar5;
        }
        pJoint->limits[1].parent.v[0] = pJoint->limits[0].parent.v[0];
        pJoint->limits[1].parent.v[1] = pJoint->limits[0].parent.v[1];
        pJoint->limits[1].parent.v[2] = pJoint->limits[0].parent.v[2];
        return;
    }
    bVar1 = *(tU8*)((tU8*)pDetach_data + 0x33);
    bVar2 = *(tU8*)((tU8*)pDetach_data + 0x32);
    iVar8 = (int)(tU32)bVar1 >> 1;
    pJoint->limits[0].parent.v[0] = 0;
    pJoint->limits[0].parent.v[1] = 0;
    pJoint->limits[0].parent.v[2] = 0;
    if ((bVar1 & 1) == 0) {
        uVar7 = gFLOAT_NEG1_0_00589b2c;
    } else {
        uVar7 = gFLOAT_00589a54;
    }
    (&pJoint->limits[0].parent.v[0])[iVar8] = uVar7;
    pJoint->limits[1].parent.v[2] = pJoint->limits[0].parent.v[2];
    pJoint->limits[1].parent.v[1] = pJoint->limits[0].parent.v[1];
    pJoint->limits[1].parent.v[0] = pJoint->limits[0].parent.v[0];
    uVar9 = (iVar8 + 1) % 3;
    if ((((int)(tU32)bVar2 >> (char)uVar9) & 1) == 0) {
        uVar7 = gFLOAT_00589a54;
    } else {
        uVar7 = gFLOAT_NEG1_0_00589b2c;
    }
    (&pJoint->limits[0].child.v[0])[uVar9] = uVar7;
    uVar10 = (iVar8 + 2) % 3;
    (&pJoint->limits[0].child.v[0])[uVar10] = 0;
    if ((bVar1 & 1) == 0) {
        uVar7 = gFLOAT_0_015_00589b30;
    } else {
        uVar7 = -gFLOAT_0_015_00589b30;
    }
    (&pJoint->limits[0].child.v[0])[iVar8] = uVar7;
    fVar3 = sqrt(pJoint->limits[0].child.v[2] * pJoint->limits[0].child.v[2] +
          pJoint->limits[0].child.v[1] * pJoint->limits[0].child.v[1] +
          pJoint->limits[0].child.v[0] * pJoint->limits[0].child.v[0]);
    if (fVar3 <= gFLT_EPSILON_00589a5c) {
        pJoint->limits[0].child.v[0] = gFLOAT_00589a54;
        pJoint->limits[0].child.v[1] = 0;
        pJoint->limits[0].child.v[2] = 0;
    } else {
        fVar3 = 1.0f / fVar3;
        pJoint->limits[0].child.v[0] = pJoint->limits[0].child.v[0] * fVar3;
        pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] * fVar3;
        pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] * fVar3;
    }
    (&pJoint->limits[1].child.v[0])[uVar9] = 0;
    if ((((int)(tU32)bVar2 >> (char)uVar10) & 1) == 0) {
        uVar7 = gFLOAT_00589a54;
    } else {
        uVar7 = gFLOAT_NEG1_0_00589b2c;
    }
    (&pJoint->limits[1].child.v[0])[uVar10] = uVar7;
    if ((bVar1 & 1) == 0) {
        uVar7 = -gFLOAT_0_015_00589b30;
    } else {
        uVar7 = gFLOAT_0_015_00589b30;
    }
    (&pJoint->limits[1].child.v[0])[iVar8] = uVar7;
    fVar3 = sqrt(pJoint->limits[1].child.v[2] * pJoint->limits[1].child.v[2] +
          pJoint->limits[1].child.v[1] * pJoint->limits[1].child.v[1] +
          pJoint->limits[1].child.v[0] * pJoint->limits[1].child.v[0]);
    if (gFLT_EPSILON_00589a5c < fVar3) {
        fVar3 = 1.0f / fVar3;
        pJoint->limits[1].child.v[0] = pJoint->limits[1].child.v[0] * fVar3;
        pJoint->limits[1].child.v[1] = pJoint->limits[1].child.v[1] * fVar3;
        pJoint->limits[1].child.v[2] = pJoint->limits[1].child.v[2] * fVar3;
        return;
    }
    pJoint->limits[1].child.v[0] = gFLOAT_00589a54;
    pJoint->limits[1].child.v[1] = 0;
    pJoint->limits[1].child.v[2] = 0;
    return;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

// FUNCTION: CARMA2_HW 0x00437ac0
void C2_HOOK_FASTCALL SetUpFlapJoint(tPhysics_joint* pJoint, tCar_crush_flap_data* pFlap_data, br_model* pModel, tPhysics_object* pObject, br_vector3* pPoint1, br_vector3* pPoint2, br_vector3* pPoint3) {
    float* pfVar1;
    float* pfVar8;
    float* joint;
    float* point_1;
    float* point_2;
    float* point_3;
    br_vector3 local_point;
    br_matrix34 rotation;
    float fVar2;
    float fVar3;
    float fVar5;
    float fVar6;

    joint = (float*)pJoint;
    point_1 = (float*)pPoint1;
    point_2 = (float*)pPoint2;
    point_3 = (float*)pPoint3;
    pJoint->type = eJoint_quick_hinge;
    if (point_1 == NULL) {
        point_1 = (float*)&pModel->vertices[pFlap_data->hinge0].p;
    }
    pfVar8 = point_2;
    if (point_2 == NULL) {
        pfVar8 = (float*)&pModel->vertices[pFlap_data->hinge1].p;
    }
    if (point_3 != NULL) {
        point_2 = point_3;
    } else {
        point_2 = (float*)&pModel->vertices[pFlap_data->hinge2].p;
    }
    joint[2] = *point_1;
    joint[3] = point_1[1];
    joint[4] = point_1[2];
    BrMatrix34ApplyP(&local_point, (br_vector3*)&joint[2], &pObject->actor->t.t.mat);
    DrMatrix34ApplyLPInverse((br_vector3*)&joint[5], &local_point, &pObject->parent->actor->t.t.mat);
    pfVar1 = (float*)(joint + 0xe);
    *pfVar1 = *pfVar8 - *point_1;
    joint[0xf] = pfVar8[1] - point_1[1];
    fVar2 = pfVar8[2] - point_1[2];
    joint[0x10] = fVar2;
    fVar2 = (float)sqrt(fVar2 * fVar2 + (float)joint[0xf] * (float)joint[0xf] + *pfVar1 * *pfVar1);
    if (fVar2 > 2.3841858e-07f) {
        fVar2 = 1.0 / fVar2;
        *pfVar1 = *pfVar1 * fVar2;
        joint[0xf] = (float)joint[0xf] * fVar2;
        joint[0x10] = (float)joint[0x10] * fVar2;
    } else {
        *pfVar1 = 1.0;
        joint[0xf] = 0;
        joint[0x10] = 0;
    }
    joint[0x11] = *pfVar1;
    joint[0x12] = joint[0xf];
    joint[0x13] = joint[0x10];
    if (*pfVar1 == gCrushZero && (float)joint[0x10] == gCrushZero) {
        joint[8] = 1.0f;
        joint[9] = 0;
        joint[10] = 0;
    } else {
        joint[8] = -(float)joint[0x10];
        joint[10] = *pfVar1;
        joint[9] = 0;
    }
    fVar2 = (float)sqrt((float)joint[10] * (float)joint[10] + (float)joint[8] * (float)joint[8]);
    if (fVar2 > 2.3841858e-07f) {
        fVar2 = 1.0 / fVar2;
        joint[9] = 0;
        joint[8] = (float)joint[8] * fVar2;
        joint[10] = (float)joint[10] * fVar2;
    } else {
        joint[8] = 1.0f;
        joint[9] = 0;
        joint[10] = 0;
    }
    joint[0xb] = (float)joint[0xf] * (float)joint[10] - (float)joint[0x10] * (float)joint[9];
    joint[0xc] = (float)joint[8] * (float)joint[0x10] - *pfVar1 * (float)joint[10];
    fVar3 = *pfVar1 * (float)joint[9] - (float)joint[8] * (float)joint[0xf];
    joint[0xd] = fVar3;
    fVar2 = (float)sqrt(fVar3 * fVar3 + (float)joint[0xc] * (float)joint[0xc] + (float)joint[0xb] * (float)joint[0xb]);
    if (fVar2 > 2.3841858e-07f) {
        fVar2 = 1.0 / fVar2;
        joint[0xb] = (float)joint[0xb] * fVar2;
        joint[0xc] = (float)joint[0xc] * fVar2;
        joint[0xd] = fVar2 * fVar3;
    } else {
        joint[0xb] = 1.0f;
        joint[0xc] = 0;
        joint[0xd] = 0;
    }
    pJoint->count_limits = 2;
    joint[0x17] = *point_2 - *point_1;
    joint[0x18] = point_2[1] - point_1[1];
    fVar2 = point_2[2];
    fVar3 = point_1[2];
    joint[0x19] = fVar2 - fVar3;
    fVar2 = (float)joint[0x17] - *pfVar1 * (*pfVar1 * (float)joint[0x17] + (float)joint[0x10] * (fVar2 - fVar3) + (float)joint[0xf] * (float)joint[0x18]);
    fVar3 = (float)joint[0x18] - ((float)joint[0x10] * (float)joint[0x19] + (float)joint[0xf] * (float)joint[0x18] + (float)joint[0x17] * *pfVar1) * (float)joint[0xf];
    fVar6 = (float)joint[0x19] - (float)joint[0x10] * ((float)joint[0x18] * (float)joint[0xf] + (float)joint[0x10] * (float)joint[0x19] + (float)joint[0x17] * *pfVar1);
    joint[0x17] = fVar2;
    fVar5 = (float)sqrt(fVar6 * fVar6 + fVar3 * fVar3 + fVar2 * fVar2);
    joint[0x18] = fVar3;
    joint[0x19] = fVar6;
    if (fVar5 > 2.3841858e-07f) {
        fVar5 = 1.0 / fVar5;
        joint[0x17] = fVar5 * fVar2;
        joint[0x18] = fVar5 * fVar3;
        joint[0x19] = (float)joint[0x19] * fVar5;
    } else {
        joint[0x17] = 1.0f;
        joint[0x18] = 0;
        joint[0x19] = 0;
    }
    joint[0x1a] = (float)joint[0xf] * (float)joint[0x19] - (float)joint[0x10] * (float)joint[0x18];
    pfVar8 = (float*)(joint + 0x1f);
    joint[0x1b] = (float)joint[0x10] * (float)joint[0x17] - *pfVar1 * (float)joint[0x19];
    *pfVar8 = (float)joint[0x17];
    fVar3 = *pfVar1 * (float)joint[0x18] - (float)joint[0x17] * (float)joint[0xf];
    joint[0x20] = joint[0x18];
    joint[0x21] = joint[0x19];
    joint[0x1c] = fVar3;
    fVar2 = (float)sqrt(fVar3 * fVar3 + (float)joint[0x1b] * (float)joint[0x1b] + (float)joint[0x1a] * (float)joint[0x1a]);
    if (fVar2 > 2.3841858e-07f) {
        fVar2 = 1.0 / fVar2;
        joint[0x1a] = (float)joint[0x1a] * fVar2;
        joint[0x1b] = (float)joint[0x1b] * fVar2;
        joint[0x1c] = fVar2 * fVar3;
    } else {
        joint[0x1a] = 1.0f;
        joint[0x1b] = 0;
        joint[0x1c] = 0;
    }
    BrMatrix34Rotate(&rotation, (br_angle)(pFlap_data->field_0x6 - 0x3ffc), (br_vector3*)pfVar1);
    BrMatrix34ApplyV((br_vector3*)&joint[0x22], (br_vector3*)&joint[0x1f], &rotation);
    fVar2 = (float)joint[0x1a] * 0.01f + (float)joint[0x17];
    fVar6 = (float)joint[0x1b] * 0.01f + (float)joint[0x18];
    fVar5 = (float)joint[0x1c] * 0.01f + (float)joint[0x19];
    fVar3 = (float)sqrt(fVar5 * fVar5 + fVar6 * fVar6 + fVar2 * fVar2);
    joint[0x17] = fVar2;
    joint[0x18] = fVar6;
    joint[0x19] = fVar5;
    if (fVar3 > 2.3841858e-07f) {
        fVar3 = 1.0 / fVar3;
        joint[0x17] = fVar3 * fVar2;
        joint[0x18] = fVar3 * fVar6;
        joint[0x19] = (float)joint[0x19] * fVar3;
    } else {
        joint[0x17] = 1.0f;
        joint[0x18] = 0;
        joint[0x19] = 0;
    }
    fVar2 = (float)joint[0x22] * 0.01f + *pfVar8;
    fVar6 = (float)joint[0x23] * 0.01f + (float)joint[0x20];
    fVar5 = (float)joint[0x24] * 0.01f + (float)joint[0x21];
    *pfVar8 = fVar2;
    fVar3 = (float)sqrt(fVar5 * fVar5 + fVar6 * fVar6 + fVar2 * fVar2);
    joint[0x20] = fVar6;
    joint[0x21] = fVar5;
    if (fVar3 > 2.3841858e-07f) {
        fVar3 = 1.0 / fVar3;
        *pfVar8 = fVar3 * fVar2;
        joint[0x20] = fVar3 * fVar6;
        joint[0x21] = fVar3 * fVar5;
        return;
    }
    *pfVar8 = 1.0;
    joint[0x20] = 0;
    joint[0x21] = 0;
}

// SendReAttachBit

// StopFlapping


/* StartFlapping and FlapBits only exist inlined into their callers in the original */
static void C2_HOOK_FASTCALL StartFlapping(br_actor* pActor, tCar_spec* pCar_spec, int pArg3, float pArg4) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->crush_data != NULL && user_crush->crush_data->flap_data != NULL) {
        tCar_crush_flap_data* flap_data = user_crush->crush_data->flap_data;

        if (flap_data->kev_o_flap) {
            SetBitForDetachment(pActor, pCar_spec, pArg4, 1);
        } else {
            MakeModelMaterialsDoubleSided(user_crush->models[0]);
            PipeSingleDSModel(1, user_crush->models[0]);
            pCar_spec->car_crush_spec->field_0x134 += 1;
            flap_data->field_0x0 = 1;
        }
        pCar_spec->use_shell_model = 0;
    }
}

static void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush_spec;

    car_crush_spec = pCar_spec->car_crush_spec;
    if (car_crush_spec != NULL) {
        if (car_crush_spec->field_0x134) {
            float dt;
            br_vector3 force;
            tFlapBit_arg flapbit_arg;

            dt = (float)(gPHIL_last_physics_tick - last_flap_bits) * 0.001f;
            BrVector3Sub(&force, &pCar_spec->collision_info->v, &car_crush_spec->field_0x138);
            force.v[0] = force.v[0] / dt;
            force.v[1] = force.v[1] / dt;
            force.v[2] = force.v[2] / dt;
            BrVector3Add(&force, &gBatty_gravity, &force);
            BrMatrix34TApplyV(&flapbit_arg.force, &force, &pCar_spec->car_master_actor->t.t.mat);
            flapbit_arg.car = pCar_spec;
            ARStartPipingSession(ePipe_chunk_flap);
            DRActorEnumRecurseWithSnart(pCar_spec->car_model_actor, NULL, FlapBit, &flapbit_arg);
            AREndPipingSession();
        }
        BrVector3Copy(&car_crush_spec->field_0x138, &pCar_spec->collision_info->v);
        last_flap_bits = gPHIL_last_physics_tick;
    }
}

extern tCar_spec* gActive_car_list[139];
extern tToggled_door gToggled_doors[16];

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
        if (car != NULL && car->driver > 5) {
            FlapBits(car);
        }
    }
}

// PARTIAL 84.36%: FlapBit 0x004384c0 - logic and frame match; residual is MSVC x87 scheduling only:
// the original keeps hinge01 in the FPU stack and multiplies world_offset from memory (and evaluates the
// three triple-product terms in the order fx, fz, fy); our build does the mirror image, plus the
// BrMatrix34Translate argument negations (fld/fchs/push order).
// FUNCTION: CARMA2_HW 0x004384c0
intptr_t C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap_data;
    tFlapBit_arg* arg;
    br_vertex* hinge;
    br_vector3 hinge01;
    br_vector3 offset;
    br_vector3 world_offset;
    br_vector3* parent_point;
    br_angle angle;
    br_angle new_angle;
    tU16 old_angle;

    user = pActor->user;
    if (user == NULL) {
        return 0;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    flap_data = crush_data->flap_data;
    if (flap_data == NULL) {
        return 0;
    }
    if (flap_data->kev_o_flap) {
        return 0;
    }
    if (!flap_data->field_0x0) {
        return 0;
    }
    hinge = &user->models[0]->vertices[flap_data->hinge0];
    BrVector3Sub(&hinge01, &user->models[0]->vertices[flap_data->hinge1].p, &hinge->p);
    {
        br_scalar scale;
        scale = BR_LENGTH3(hinge01.v[0], hinge01.v[1], hinge01.v[2]);
        if (scale > gFLT_EPSILON_00589a5c) {
            scale = (br_scalar)(gDouble_00589a60 / scale);
            hinge01.v[0] = hinge01.v[0] * scale;
            hinge01.v[1] = hinge01.v[1] * scale;
            hinge01.v[2] = hinge01.v[2] * scale;
        } else {
            hinge01.v[0] = 1.0f;
            hinge01.v[1] = 0.0f;
            hinge01.v[2] = 0.0f;
        }
    }
    BrVector3Sub(&offset, &user->models[0]->vertices[flap_data->hinge2].p, &hinge->p);
    {
        br_scalar scale;
        scale = BR_LENGTH3(offset.v[0], offset.v[1], offset.v[2]);
        if (scale > gFLT_EPSILON_00589a5c) {
            scale = (br_scalar)(gDouble_00589a60 / scale);
            offset.v[0] = offset.v[0] * scale;
            offset.v[1] = offset.v[1] * scale;
            offset.v[2] = offset.v[2] * scale;
        } else {
            offset.v[0] = 1.0f;
            offset.v[1] = 0.0f;
            offset.v[2] = 0.0f;
        }
    }
    BrMatrix34ApplyV(&world_offset, &offset, pMat);
    arg = pContext;
    angle = BrRadianToAngle(atan2(
        ((hinge01.v[2] * world_offset.v[1] - hinge01.v[1] * world_offset.v[2]) * arg->force.v[0]
            + (hinge01.v[1] * world_offset.v[0] - hinge01.v[0] * world_offset.v[1]) * arg->force.v[2]
            + (hinge01.v[0] * world_offset.v[2] - hinge01.v[2] * world_offset.v[0]) * arg->force.v[1])
        * gFlap_inertia_fudge_biscuit,
        crush_data->field_0x24));
    if (angle == 0) {
        return 0;
    }
    old_angle = flap_data->field_0x4;
    new_angle = flap_data->field_0x4 + angle;
    flap_data->field_0x4 = new_angle;
    if ((br_angle)(new_angle - flap_data->field_0x2) > flap_data->field_0x8) {
        if ((br_angle)(new_angle - flap_data->field_0x6) < 0x3ffc) {
            flap_data->field_0x4 = flap_data->field_0x6;
        } else {
            flap_data->field_0x4 = flap_data->field_0x2;
        }
    }
    BrMatrix34Translate(&pActor->t.t.mat, -hinge->p.v[0], -hinge->p.v[1], -hinge->p.v[2]);
    BrMatrix34PostRotate(&pActor->t.t.mat, flap_data->field_0x4, &hinge01);
    parent_point = &((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[flap_data->field_0xc].p;
    BrMatrix34PostTranslate(&pActor->t.t.mat, parent_point->v[0], parent_point->v[1], parent_point->v[2]);
    if (old_angle != flap_data->field_0x4) {
        AddFlapToPipingSession((uintptr_t)pActor, (tU16)(flap_data->field_0x4 - old_angle));
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tS3_outlet* gCar_outlet;

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
// FullyDetachBit

// SendFullyDetachBit

extern tCar_spec* gActive_car_list[139];
extern tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];
extern int gCount_net_crush_full_detach_bit_list;

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

// QuantizeAngle

/* inlined into DoBending in the original */
static int C2_HOOK_FASTCALL BendCarOneForce(tCar_spec* pCar, tCar_crush* pCar_crush) {
    (void)pCar;
    (void)pCar_crush;
    return 0;
}

extern tCrush_list_item gCrush_lists[8];

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

/* The crush scale constants are defined after CrushBendFlapRend so that their
   values are not constant-propagated into the body (the original loads them
   from memory: fmul QWORD PTR ds:0x589b18 / ds:0x589b20). */
// GLOBAL: CARMA2_HW 0x00589b18
double gDouble_00589b18 = 99.0;
// GLOBAL: CARMA2_HW 0x00589b20
double gDouble_00589b20 = -0.05;
// GLOBAL: CARMA2_HW 0x00589b28
br_scalar gFloat_00589b28 = 5.0f;

// SphericizeModel

// SphericizePhysics



// PARTIAL 66.95%: SphericizeCar - centre x87 order differs, pCar in ebx instead of esi
// FUNCTION: CARMA2_HW 0x004391d0
void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar) {
    br_vector3 center;
    br_scalar radius;
    br_vector3 vec;
    br_model* shell_model;
    tU16 i;
    br_scalar hx, hy, hz, lx, ly, lz;
    br_scalar dx;
    br_scalar dy;
    br_scalar dz;
    br_scalar len;
    br_scalar nx;
    br_scalar ny;
    br_scalar nz;

    shell_model = pCar->shell_model;
    if (shell_model != NULL) {
        hx = shell_model->bounds.max.v[0] * 0.5f;
        hy = shell_model->bounds.max.v[1] * 0.5f;
        lx = shell_model->bounds.min.v[0] * 0.5f;
        ly = shell_model->bounds.min.v[1] * 0.5f;
        hz = shell_model->bounds.max.v[2] * 0.5f;
        lz = shell_model->bounds.min.v[2] * 0.5f;
        center.v[0] = hx + lx;
        center.v[1] = hy + ly;
        center.v[2] = hz + lz;
        radius = (float)sqrt(Vector3DistanceSquared(&center, &shell_model->bounds.min));
        for (i = 0; i < shell_model->nvertices; i++) {
            dx = shell_model->vertices[i].p.v[0] - center.v[0];
            dy = shell_model->vertices[i].p.v[1] - center.v[1];
            dz = shell_model->vertices[i].p.v[2] - center.v[2];
            len = (float)sqrt(dx * dx + dy * dy + dz * dz);
            if (len > 0.01f) {
                nx = dx * (1.0 / len);
                ny = dy * (1.0 / len);
                nz = dz * (1.0 / len);
            } else {
                nx = 1.0f;
                ny = 0.0f;
                nz = 0.0f;
            }
            shell_model->vertices[i].p.v[0] = nx * radius + center.v[0];
            shell_model->vertices[i].p.v[1] = ny * radius + center.v[1];
            shell_model->vertices[i].p.v[2] = nz * radius + center.v[2];
        }
        BrModelUpdate(shell_model, 1);
        pCar->car_model_actor->children = NULL;
        gGonad_sphere_collision_shape->sphere.sphere.radius = radius;
        gGonad_sphere_collision_shape->sphere.sphere.center.v[0] = center.v[0];
        gGonad_sphere_collision_shape->sphere.sphere.center.v[1] = center.v[1];
        gGonad_sphere_collision_shape->sphere.sphere.center.v[2] = center.v[2];
        gGonad_sphere_collision_shape->common.next = NULL;
        FillInShape(gGonad_sphere_collision_shape);
        pCar->collision_info->shape = gGonad_sphere_collision_shape;
        UpdateCollisionObject(pCar->collision_info);
        pCar->collision_info->cmpos.v[0] = center.v[0];
        pCar->collision_info->cmpos.v[1] = center.v[1];
        pCar->collision_info->cmpos.v[2] = center.v[2];
        pCar->collision_info->M = 50.0f;
        pCar->collision_info->I.v[0] = pCar->collision_info->shape->sphere.sphere.radius * pCar->collision_info->shape->sphere.sphere.radius * pCar->collision_info->M * 0.166666672f;
        pCar->collision_info->I.v[1] = pCar->collision_info->shape->sphere.sphere.radius * pCar->collision_info->shape->sphere.sphere.radius * pCar->collision_info->M * 0.166666672f;
        pCar->collision_info->I.v[2] = pCar->collision_info->shape->sphere.sphere.radius * pCar->collision_info->shape->sphere.sphere.radius * pCar->collision_info->M * 0.166666672f;
        pCar->collision_info->world_friction = 16.0f;
        gGonad_sphere_collision_shape->common.type = 0;
        while (!TestForCarInSensiblePlace(pCar, &vec)) {
            vec.v[0] = vec.v[0] * 0.0724637657f;
            vec.v[1] = vec.v[1] * 0.0724637657f;
            vec.v[2] = vec.v[2] * 0.0724637657f;
            pCar->collision_info->transform_matrix.m[3][0] += vec.v[0];
            pCar->collision_info->transform_matrix.m[3][1] += vec.v[1];
            pCar->collision_info->transform_matrix.m[3][2] += vec.v[2];
        }
        gGonad_sphere_collision_shape->common.type = 4;
        pCar->invulnerable_no_crushage = 1;
        pCar->invulnerable_no_damage = 1;
        pCar->use_shell_model = 1;
        pCar->field_0x18c8 = 2;
    }
}

// FUNCTION: CARMA2_HW 0x00439500
void C2_HOOK_FASTCALL FUN_00439500(void) {

    RandomlyXZSplitCar(gCar_to_view);
}

// SendReAttachAllBits


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
// FUNCTION: CARMA2_HW 0x004398c0
// FUNCTION: CARMA2_SW 0x00439900
int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, BattenDownTheHatches, pUser_data);
    return 0;
}


#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShapes(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        CompletelyUnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i]);
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}
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

void C2_HOOK_FASTCALL CompletelyUnBendOtherThings(tCar_spec* pCar_spec) {
    CompletelyUnBendCollisionShapes(pCar_spec);
    CompletelyUnBendWheels(pCar_spec);
    memcpy(&pCar_spec->collision_info->cmpos, &pCar_spec->car_crush_spec->field_0x264, sizeof(pCar_spec->collision_info->cmpos));
}

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

void C2_HOOK_FASTCALL DoCompletelyUnBentThings(tCar_spec* pCar_spec) {
    pCar_spec->car_crush_spec->field_0x144 = 0;
    CompletelyUnBendOtherThings(pCar_spec);
    DRActorEnumRecurse(pCar_spec->car_model_actor, EnableGroovers, NULL);
}

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

// FUNCTION: CARMA2_HW 0x00439a10
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}


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

// FUNCTION: CARMA2_HW 0x00439bf0
// FUNCTION: CARMA2_SW 0x00439c30
void C2_HOOK_FASTCALL TotallyRepairCar(void) {

    if (!gArrow_mode) {
        TotallyRepairACar(&gProgram_state.current_car);
    }
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00439c10
void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec) {
    br_scalar original_amount;

    UnBendCar(pCar_spec, 2000, &original_amount);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
br_scalar C2_HOOK_FASTCALL CalculateAmountRepairedFraction(br_angle pOriginal_x, br_angle pOriginal_y, br_angle pOriginal_z, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, br_scalar* pOriginal_amount) {
    br_scalar original_amount = NormalizedAngle(pOriginal_x) + NormalizedAngle(pOriginal_y) + NormalizedAngle(pOriginal_z);
    br_scalar repaired_amount = NormalizedAngle(pRepair_x) + NormalizedAngle(pRepair_y) + NormalizedAngle(pRepair_z);

    *pOriginal_amount = 2.0f * original_amount;
    return repaired_amount;
}

// Angle conversion inlined 4x by the original (NormalizedAngle body); NormalizedAngle itself stays a call for the
// 5th and 6th sites -- keep both helpers so the inliner reproduces that split.
static br_scalar UnBendCarAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle / (double)BR_ONE_LS);
    } else {
        return (br_scalar)((br_angle)-pAngle / (double)BR_ONE_LS);
    }
}

// PARTIAL 82.09%: UnBendCar 0x00439c30 [x87-schedule] - local slots ([esp+0x10]/[esp+0x14] swapped), original sums (fxch/faddp order, repair_x kept in eax) not reproduced
// FUNCTION: CARMA2_HW 0x00439c30
br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount) {
    tCar_crush_spec* car_crush;
    tBend_data_local* bend;
    tBend_point_context_local context;
    br_angle original_x, original_y, original_z;
    br_angle repair_x, repair_y, repair_z;
    br_angle angle_step;
    br_scalar original_amount;
    br_scalar repaired_amount;
    int i;

    car_crush = pCar_spec->car_crush_spec;
    angle_step = (br_angle)(pDelta_time * 8u);
    original_x = *(br_angle*)((char*)car_crush + 0x190);
    if (original_x < BR_ANGLE_DEG(90)) {
        if (original_x < angle_step) {
            repair_x = (br_angle)-original_x;
        } else {
            repair_x = (br_angle)-angle_step;
        }
    } else {
        repair_x = (br_angle)-original_x;
        if (repair_x >= angle_step) {
            repair_x = angle_step;
        }
    }
    original_y = *(br_angle*)((char*)car_crush + 0x192);
    if (original_y < BR_ANGLE_DEG(90)) {
        if (original_y < angle_step) {
            repair_y = (br_angle)-original_y;
        } else {
            repair_y = (br_angle)-angle_step;
        }
    } else {
        repair_y = (br_angle)-original_y;
        if (repair_y >= angle_step) {
            repair_y = angle_step;
        }
    }
    original_z = *(br_angle*)((char*)car_crush + 0x194);
    if (original_z < BR_ANGLE_DEG(90)) {
        if (original_z < angle_step) {
            repair_z = (br_angle)-original_z;
        } else {
            repair_z = (br_angle)-angle_step;
        }
    } else {
        repair_z = (br_angle)-original_z;
        if (repair_z >= angle_step) {
            repair_z = angle_step;
        }
    }

    original_amount = UnBendCarAngle(original_x);
    original_amount += UnBendCarAngle(original_y);
    original_amount += UnBendCarAngle(original_z);
    repaired_amount = UnBendCarAngle(repair_x);
    repaired_amount += NormalizedAngle(repair_y);
    repaired_amount += NormalizedAngle(repair_z);
    *pOriginal_amount = original_amount + original_amount;
    bend = (tBend_data_local*)car_crush->field_0x154;
    UnBendActor(pCar_spec->car_master_actor, bend, repair_x, repair_y, repair_z, pCar_spec);

    context.car = pCar_spec;
    context.bend = bend;
    BrMatrix34Translate(&context.matrix,
        -bend->field_0x18.v[0], -bend->field_0x18.v[1], -bend->field_0x18.v[2]);
    if (bend->field_0x3e != 0) {
        BrMatrix34PostRotateY(&context.matrix, bend->field_0x3e);
    }
    if (bend->field_0x3c != 0) {
        BrMatrix34PostRotateX(&context.matrix, bend->field_0x3c);
    }
    if (bend->field_0x40 != 0) {
        BrMatrix34PostRotateZ(&context.matrix, bend->field_0x40);
    }
    BrMatrix34PostTranslate(&context.matrix,
        bend->field_0x18.v[0], bend->field_0x18.v[1], bend->field_0x18.v[2]);
    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        UnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i], &context);
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
    UnBendWheels(pCar_spec, &context);
    pCar_spec->collision_info->cmpos.v[0] = pCar_spec->car_crush_spec->field_0x264.v[0];
    pCar_spec->collision_info->cmpos.v[1] = pCar_spec->car_crush_spec->field_0x264.v[1];
    pCar_spec->collision_info->cmpos.v[2] = pCar_spec->car_crush_spec->field_0x264.v[2];
    BendPoint(&pCar_spec->collision_info->cmpos, &context);

    if (*(br_angle*)((char*)car_crush + 0x190) == 0
        && *(br_angle*)((char*)car_crush + 0x192) == 0
        && *(br_angle*)((char*)car_crush + 0x194) == 0) {
        i = 0;
        pCar_spec->car_crush_spec->field_0x144 = 0;
        for (; i < pCar_spec->car_crush_spec->count_shapes; i++) {
            CompletelyUnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i]);
        }
        pCar_spec->car_crush_spec->expand_bounding_box = 3;
        CompletelyUnBendWheels(pCar_spec);
        pCar_spec->collision_info->cmpos.v[0] = pCar_spec->car_crush_spec->field_0x264.v[0];
        pCar_spec->collision_info->cmpos.v[1] = pCar_spec->car_crush_spec->field_0x264.v[1];
        pCar_spec->collision_info->cmpos.v[2] = pCar_spec->car_crush_spec->field_0x264.v[2];
        DRActorEnumRecurse(pCar_spec->car_model_actor, EnableGroovers, NULL);
    }
    return repaired_amount;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00439fd0
// FUNCTION: CARMA2_SW 0x0043a010
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape) {
    int i;

    pShape->count_points = pShape->original_count_points;
    for (i = 0; i < pShape->count_points; i++) {
        BrVector3Copy(&pShape->field_0x18[i].field_0x18, &pShape->field_0x18[i].field_0x2c);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// (reference only)
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

// PARTIAL 55.09%: pt init copies via integer mov (original fld/fstp, no CSE), arg load order; context holds matrix, four corner deltas, bend and car (read by UnBendModels)
// Inferred layout of the context handed to the model-bend callbacks
// (matrix at 0x0, corner displacements, bend data at 0x60, car at 0x64).
typedef struct tUnbend_model_context_local {
    br_matrix34 matrix;
    br_vector3 field_0x30;
    br_vector3 field_0x3c;
    br_vector3 field_0x48;
    br_vector3 field_0x54;
    tBend_data_local* bend;
    tCar_spec* car;
} tUnbend_model_context_local;
// FUNCTION: CARMA2_HW 0x0043a150
void C2_HOOK_FASTCALL UnBendActor(br_actor* pActor, tBend_data_local* pBend, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, tCar_spec* pCar_spec) {
    tUnbend_model_context_local context;
    br_vector3 tmp;

    context.bend = pBend;
    context.car = pCar_spec;
    pBend->field_0x3c += pRepair_x;
    pBend->field_0x3e += pRepair_y;
    pBend->field_0x40 += pRepair_z;
    BrMatrix34Translate(&context.matrix, -pBend->field_0x18.v[0], -pBend->field_0x18.v[1], -pBend->field_0x18.v[2]);
    if (pBend->field_0x3e != 0) {
        BrMatrix34PostRotateY(&context.matrix, pBend->field_0x3e);
    }
    if (pBend->field_0x3c != 0) {
        BrMatrix34PostRotateX(&context.matrix, pBend->field_0x3c);
    }
    if (pBend->field_0x40 != 0) {
        BrMatrix34PostRotateZ(&context.matrix, pBend->field_0x40);
    }
    BrMatrix34PostTranslate(&context.matrix, pBend->field_0x18.v[0], pBend->field_0x18.v[1], pBend->field_0x18.v[2]);
    context.field_0x30.v[0] = pBend->field_0xc;
    context.field_0x30.v[1] = pBend->field_0x10;
    context.field_0x30.v[2] = pBend->field_0x18.v[2];
    context.field_0x3c.v[0] = pBend->field_0xc;
    context.field_0x3c.v[1] = pBend->field_0x4;
    context.field_0x3c.v[2] = pBend->field_0x18.v[2];
    context.field_0x48.v[0] = pBend->field_0x0;
    context.field_0x48.v[1] = pBend->field_0x10;
    context.field_0x48.v[2] = pBend->field_0x18.v[2];
    context.field_0x54.v[0] = pBend->field_0x0;
    context.field_0x54.v[1] = pBend->field_0x4;
    context.field_0x54.v[2] = pBend->field_0x18.v[2];
    BrMatrix34ApplyP(&tmp, &context.field_0x30, &context.matrix);
    context.field_0x30.v[0] = tmp.v[0] - context.field_0x30.v[0];
    context.field_0x30.v[1] = tmp.v[1] - context.field_0x30.v[1];
    context.field_0x30.v[2] = tmp.v[2] - context.field_0x30.v[2];
    BrMatrix34ApplyP(&tmp, &context.field_0x3c, &context.matrix);
    context.field_0x3c.v[0] = tmp.v[0] - context.field_0x3c.v[0];
    context.field_0x3c.v[1] = tmp.v[1] - context.field_0x3c.v[1];
    context.field_0x3c.v[2] = tmp.v[2] - context.field_0x3c.v[2];
    BrMatrix34ApplyP(&tmp, &context.field_0x48, &context.matrix);
    context.field_0x48.v[0] = tmp.v[0] - context.field_0x48.v[0];
    context.field_0x48.v[1] = tmp.v[1] - context.field_0x48.v[1];
    context.field_0x48.v[2] = tmp.v[2] - context.field_0x48.v[2];
    BrMatrix34ApplyP(&tmp, &context.field_0x54, &context.matrix);
    context.field_0x54.v[0] = tmp.v[0] - context.field_0x54.v[0];
    context.field_0x54.v[1] = tmp.v[1] - context.field_0x54.v[1];
    context.field_0x54.v[2] = tmp.v[2] - context.field_0x54.v[2];
    context.field_0x30.v[0] = ((context.field_0x30.v[0] - context.field_0x3c.v[0]) - context.field_0x48.v[0]) + context.field_0x54.v[0];
    context.field_0x30.v[1] = ((context.field_0x30.v[1] - context.field_0x3c.v[1]) - context.field_0x48.v[1]) + context.field_0x54.v[1];
    context.field_0x30.v[2] = ((context.field_0x30.v[2] - context.field_0x3c.v[2]) - context.field_0x48.v[2]) + context.field_0x54.v[2];
    context.field_0x3c.v[0] = context.field_0x3c.v[0] - context.field_0x54.v[0];
    context.field_0x3c.v[1] = context.field_0x3c.v[1] - context.field_0x54.v[1];
    context.field_0x3c.v[2] = context.field_0x3c.v[2] - context.field_0x54.v[2];
    context.field_0x48.v[0] = context.field_0x48.v[0] - context.field_0x54.v[0];
    context.field_0x48.v[1] = context.field_0x48.v[1] - context.field_0x54.v[1];
    context.field_0x48.v[2] = context.field_0x48.v[2] - context.field_0x54.v[2];
    BrMatrix34Translate(&context.matrix, -pBend->field_0x18.v[0], -pBend->field_0x18.v[1], -pBend->field_0x18.v[2]);
    if (pRepair_y != 0) {
        BrMatrix34PostRotateY(&context.matrix, pRepair_y);
    }
    if (pRepair_x != 0) {
        BrMatrix34PostRotateX(&context.matrix, pRepair_x);
    }
    if (pRepair_z != 0) {
        BrMatrix34PostRotateZ(&context.matrix, pRepair_z);
    }
    BrMatrix34PostTranslate(&context.matrix, pBend->field_0x18.v[0], pBend->field_0x18.v[1], pBend->field_0x18.v[2]);
    ARStartPipingSession((tPipe_chunk_type)0x18);
    DRActorEnumRecurseWithTranslation(pActor, NULL, UnBendModels, &context.matrix);
    AREndPipingSession();
    pBend->field_0x44 = context.field_0x30;
    pBend->field_0x50 = context.field_0x3c;
    pBend->field_0x5c = context.field_0x48;
    pBend->field_0x68 = context.field_0x54;
}

// Unbends every detail-level model of the actor (callback of UnBendActor).
// FUNCTION: CARMA2_HW 0x0043a560
intptr_t C2_HOOK_FASTCALL UnBendModels(br_actor* pActor, br_vector3* pDelta, void* pData) {
    tUser_crush_data* user_crush;
    const tUnbend_model_context_local* context;
    int i;

    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    if (user_crush->crush_data != NULL) {
        context = pData;
        for (i = 0; i < context->car->count_detail_levels; i++) {
            UnBendModel(user_crush->models[i], pDelta, pData);
        }
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif


// Re-applies the bend transform of one detail-level model: every vertex that
// the bend field covers is moved from its previous bend displacement to the
// new one, and the per-vertex displacement is piped as a model mash.
// FUNCTION: CARMA2_HW 0x0043a5b0
void C2_HOOK_FASTCALL UnBendModel(br_model* pModel, br_vector3* pTranslation, void* pContext) {
    br_vector3 mash[1024];
    tUnbend_model_context_local* context;
    tBend_data_local* bend;
    tUser_detail_level_model* detail_model;
    tModel_detail_vertex_data* vertex_data;
    tUser_detail_level_model* user_detail_model;
    br_vertex* vertex;
    br_vector3 pos;
    br_vector3 tmp;
    br_vector3 offset;
    br_vector3 d;
    br_vector3 delta;
    br_vector3 old_delta;
    tU16 index;
    tU16 i;
    int changed;
    float a;
    float b;
    float c;
    float ab;

    if (pModel == NULL) {
        return;
    }
    detail_model = pModel->user;
    if (detail_model == NULL) {
        return;
    }
    vertex_data = detail_model->field_0x4;
    if (vertex_data == NULL) {
        return;
    }
    if (pModel->nvertices > REC2_ASIZE(mash)) {
        PDFatalError("My Lord! There's a lot of vertices on that model.");
    }
    context = (tUnbend_model_context_local*)pContext;
    bend = context->bend;
    index = *(tU16*)&detail_model->field_0x8[0];
    changed = 0;
    for (i = 0; i < pModel->nvertices; i++) {
        vertex = &pModel->vertices[i];
        if ((vertex_data[i].flags & 0x8) == 0) {
            mash[i].v[0] = 0.f;
            mash[i].v[1] = 0.f;
            mash[i].v[2] = 0.f;
        } else {
            if ((vertex_data[i].flags & 0x10) != 0) {
                mash[i].v[0] = -vertex->p.v[0];
                mash[i].v[1] = -vertex->p.v[1];
                mash[i].v[2] = -vertex->p.v[2];
                d.v[0] = vertex->p.v[0] + pTranslation->v[0];
                d.v[1] = pTranslation->v[1] + vertex->p.v[1];
                d.v[2] = pTranslation->v[2] + vertex->p.v[2];
                BrMatrix34ApplyP(&tmp, &d, &context->matrix);
                vertex->p.v[0] = tmp.v[0] - pTranslation->v[0];
                vertex->p.v[1] = tmp.v[1] - pTranslation->v[1];
                vertex->p.v[2] = tmp.v[2] - pTranslation->v[2];
                mash[i].v[0] += vertex->p.v[0];
                mash[i].v[1] += vertex->p.v[1];
            } else {
                ExpandVector3(&pos, (const tCompressed_vector3*)&bend->field_0x78[index], -10.f, 10.f);
                offset.v[0] = pos.v[0] - bend->field_0x24.v[0];
                offset.v[1] = pos.v[1] - bend->field_0x24.v[1];
                offset.v[2] = pos.v[2] - bend->field_0x24.v[2];
                a = offset.v[0] / bend->field_0x30.v[0];
                index++;
                b = offset.v[1] / bend->field_0x30.v[1];
                c = offset.v[2] / bend->field_0x30.v[2];
                ab = a * b;
                delta.v[0] = (context->field_0x54.v[0] + context->field_0x48.v[0] * b + context->field_0x3c.v[0] * a + context->field_0x30.v[0] * ab) * c;
                delta.v[1] = (context->field_0x54.v[1] + context->field_0x48.v[1] * b + context->field_0x3c.v[1] * a + context->field_0x30.v[1] * ab) * c;
                delta.v[2] = (context->field_0x54.v[2] + context->field_0x48.v[2] * b + context->field_0x3c.v[2] * a + context->field_0x30.v[2] * ab) * c;
                old_delta.v[0] = (bend->field_0x68.v[0] + bend->field_0x5c.v[0] * b + bend->field_0x50.v[0] * a + bend->field_0x44.v[0] * ab) * c;
                old_delta.v[1] = (bend->field_0x68.v[1] + bend->field_0x5c.v[1] * b + bend->field_0x50.v[1] * a + bend->field_0x44.v[1] * ab) * c;
                old_delta.v[2] = (bend->field_0x68.v[2] + bend->field_0x5c.v[2] * b + bend->field_0x50.v[2] * a + bend->field_0x44.v[2] * ab) * c;
                d.v[0] = pos.v[0] - pTranslation->v[0];
                d.v[1] = pos.v[1] - pTranslation->v[1];
                d.v[2] = pos.v[2] - pTranslation->v[2];
                mash[i].v[0] = -vertex->p.v[0];
                mash[i].v[1] = -vertex->p.v[1];
                mash[i].v[2] = -vertex->p.v[2];
                vertex->p.v[0] = (vertex->p.v[0] - d.v[0] - old_delta.v[0]) + (d.v[0] + delta.v[0]);
                vertex->p.v[1] = (vertex->p.v[1] - d.v[1] - old_delta.v[1]) + (d.v[1] + delta.v[1]);
                vertex->p.v[2] = (vertex->p.v[2] - d.v[2] - old_delta.v[2]) + (d.v[2] + delta.v[2]);
                mash[i].v[0] += vertex->p.v[0];
                mash[i].v[1] += vertex->p.v[1];
            }
            changed = 1;
            mash[i].v[2] += vertex->p.v[2];
        }
    }
    if (changed != 0) {
        if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
            user_detail_model = pModel->user;
            user_detail_model->field_0x0 = 1;
        } else {
            pModel->custom = JitModelUpdate;
            pModel->flags |= BR_MODF_CUSTOM;
        }
        AddModelMashToPipingSession(pModel, mash);
    }
}
// FUNCTION: CARMA2_HW 0x0043aa20
void C2_HOOK_FASTCALL UnBendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context_local* pContext) {
    int i;

    for (i = 0; i < pShape->count_points; i++) {
        pShape->field_0x18[i].field_0x18.v[0] = pShape->field_0x18[i].field_0x2c.v[0];
        pShape->field_0x18[i].field_0x18.v[1] = pShape->field_0x18[i].field_0x2c.v[1];
        pShape->field_0x18[i].field_0x18.v[2] = pShape->field_0x18[i].field_0x2c.v[2];
        BendPoint(&pShape->field_0x18[i].field_0x18, pContext);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// PARTIAL 96.73%: wpos loop addressing differs (original pointer-offset form)
// FUNCTION: CARMA2_HW 0x0043aa80
void C2_HOOK_FASTCALL UnBendWheels(tCar_spec* pCar_spec, const tBend_point_context_local* pContext) {
    int i;
    float angle;
    br_vector3 delta;
    br_vector3 tmp;

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

            delta.v[0] = -centre->v[0];
            delta.v[1] = -centre->v[1];
            delta.v[2] = -centre->v[2];
            centre->v[0] = pCar_spec->car_crush_spec->field_0x1ec[i].v[0];
            centre->v[1] = pCar_spec->car_crush_spec->field_0x1ec[i].v[1];
            centre->v[2] = pCar_spec->car_crush_spec->field_0x1ec[i].v[2];
            BendPoint(centre, pContext);
            centre->v[1] = pCar_spec->car_crush_spec->field_0x1ec[i].v[1];
            delta.v[0] = centre->v[0] + delta.v[0];
            delta.v[1] = centre->v[1] + delta.v[1];
            delta.v[2] = centre->v[2] + delta.v[2];
            PipeSingleVector3(centre, &delta);
        }
    }

    for (i = 0; i < REC2_ASIZE(pCar_spec->wpos); i++) {
        tmp.v[0] = pCar_spec->car_crush_spec->field_0x234[i].v[0] * 6.9;
        tmp.v[1] = pCar_spec->car_crush_spec->field_0x234[i].v[1] * 6.9;
        tmp.v[2] = pCar_spec->car_crush_spec->field_0x234[i].v[2] * 6.9;
        BendPoint(&tmp, pContext);
        pCar_spec->wpos[i].v[0] = tmp.v[0] * 0.14492753;
        pCar_spec->wpos[i].v[1] = tmp.v[1] * 0.14492753;
        pCar_spec->wpos[i].v[2] = tmp.v[2] * 0.14492753;
    }
    SetCarSuspGiveAndHeight(pCar_spec, 1.f, 1.f, 1.f, 0.f, 0.f);
    if (pContext->bend->field_0x18.v[2] == pCar_spec->car_crush_spec->bend_z_min) {
        angle = (tU16)pContext->bend->field_0x3e * 0.005493164;
        if (angle >= 180.f) {
            angle = angle - 360.f;
        }
        pCar_spec->field_0x1260 = angle * 0.03f;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0043ac90
// FUNCTION: CARMA2_SW 0x0043ace0
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle / (double)BR_ONE_LS);
    } else {
        return (br_scalar)((br_angle)-pAngle / (double)BR_ONE_LS);
    }
}


#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern tNon_car_spec* gActive_non_car_list[99];

// FUNCTION: CARMA2_HW 0x0043b8a0
void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec) {
    tNon_car_spec* non_car;
    tPhysics_object* object;
    tPhysics_object* next_object;
    tPhysics_object* collision_info;
    tCar_crush_shape_info* shape_info;
    tCollision_shape_polyhedron* prev_shape;
    br_actor* actor;
    br_actor** actor_ptr;
    br_model** models;
    br_model** model_source;
    br_model** model_slot;
    tCar_crush_spec* car_crush;
    char text[256];
    int i;
    int j;
    int offset;

    car_crush = pCar_spec->car_crush_spec;
    if (car_crush->field_0x4b8 == 0) {
        return;
    }
    LoseAllLocalPowerups(pCar_spec);
    non_car = (tNon_car_spec*)car_crush->actor->type_data;
    if (non_car != NULL) {
        while (non_car->collision_info->shape->common.type == kCollisionShapeType_Wireframe) {
            non_car->collision_info->shape = non_car->collision_info->shape->common.next;
        }
        object = non_car->collision_info->child;
        while (object != NULL) {
            PipeSingleNonCar(*(tU16*)((tU8*)object->actor->type_data + 0x80), object->actor);
            next_object = object->next;
            PhysicsRemoveObject(object);
            PhysicsAddObject(pCar_spec->collision_info, object);
            object = next_object;
        }
        PipeSingleNonCar(*(tU16*)((tU8*)non_car + 0x80), car_crush->actor);
        for (i = 0; i < gNum_active_non_cars; i++) {
            if (gActive_non_car_list[i] == non_car) {
                break;
            }
        }
        if (i != gNum_active_non_cars) {
            gNum_active_non_cars--;
            if (i < gNum_active_non_cars) {
                for (j = i; j < gNum_active_non_cars; j++) {
                    gActive_non_car_list[j] = gActive_non_car_list[j + 1];
                }
            }
        }
        non_car->driver = 0;
        non_car->actor->type_data = NULL;
        PHILRemoveObject(non_car->collision_info);
    }
    for (;;) {
        actor = car_crush->model_actor->children;
        if (actor == NULL) {
            break;
        }
        PipeSingleRelink(0, actor);
        BrActorRemove(actor);
        PipeSingleRelink((uintptr_t)pCar_spec->car_model_actor, actor);
        BrActorAdd(pCar_spec->car_model_actor, actor);
    }
    i = 0;
    if (0 < *(int*)((tU8*)car_crush + 0x494)) {
        actor_ptr = (br_actor**)((tU8*)car_crush + 0x498);
        do {
            PipeSingleRelink((uintptr_t)pCar_spec->car_model_actor, *actor_ptr);
            BrActorAdd(pCar_spec->car_model_actor, *actor_ptr);
            i++;
            actor_ptr++;
        } while (i < *(int*)((tU8*)car_crush + 0x494));
    }
    i = 0;
    *(int*)((tU8*)car_crush + 0x494) = 0;
    models = (br_model**)pCar_spec->car_model_actor->user;
    if (0 < pCar_spec->count_detail_levels) {
        model_source = (br_model**)((tU8*)car_crush + 0x4c4);
        /* The original keeps this walked-but-unused cursor in memory; taking its address
           (never executed) is what makes MSVC 5.0 materialise the slot and the add/store. */
        if (0) {
            PipeSingleRelink((uintptr_t)&model_source, actor);
        }
        model_slot = models;
        do {
            if (*model_slot != NULL) {
                BrModelFree(BrModelRemove(*model_slot));
                *model_slot = models[0];
            }
            i++;
            model_source++;
            model_slot++;
        } while (i < pCar_spec->count_detail_levels);
    }
    pCar_spec->car_model_actor->model = models[0];
    BrModelFree(BrModelRemove(car_crush->model_actor->model));
    PipeSingleRelink(0, car_crush->actor);
    BrActorRemove(car_crush->actor);
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar_spec);
    prev_shape = NULL;
    i = 0;
    if (0 < car_crush->count_shapes) {
        offset = 0;
        do {
            shape_info = (tCar_crush_shape_info*)((tU8*)car_crush->field_0x4 + offset);
            if (shape_info->field_0x8 == 0) {
                if (prev_shape != NULL) {
                    prev_shape->common.next = (tPhysics_shape*)shape_info->field_0x0;
                } else {
                    pCar_spec->collision_info->shape = (tPhysics_shape*)shape_info->field_0x0;
                }
                prev_shape = *(tCollision_shape_polyhedron**)((tU8*)car_crush->field_0x4 + offset);
            }
            i++;
            offset += 0x1c;
        } while (i < car_crush->count_shapes);
    }
    if (prev_shape != NULL) {
        prev_shape->common.next = NULL;
    }
    UpdateCollisionObject(pCar_spec->collision_info);
    pCar_spec->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar_spec);
    collision_info = pCar_spec->collision_info;
    pCar_spec->number_of_wheels_on_ground = 0;
    collision_info->actor = *(br_actor**)((tU8*)car_crush + 0x4d4);
    collision_info->M = *(float*)((tU8*)car_crush + 0x4d8);
    collision_info->I.v[0] = *(float*)((tU8*)car_crush + 0x4dc);
    collision_info->I.v[1] = *(float*)((tU8*)car_crush + 0x4e0);
    collision_info->I.v[2] = *(float*)((tU8*)car_crush + 0x4e4);
    collision_info->cmpos.v[0] = *(float*)((tU8*)car_crush + 0x4e8);
    collision_info->cmpos.v[1] = *(float*)((tU8*)car_crush + 0x4ec);
    collision_info->cmpos.v[2] = *(float*)((tU8*)car_crush + 0x4f0);
    collision_info->field_0x54.v[0] = *(float*)((tU8*)car_crush + 0x524);
    collision_info->field_0x54.v[1] = *(float*)((tU8*)car_crush + 0x528);
    collision_info->field_0x54.v[2] = *(float*)((tU8*)car_crush + 0x52c);
    pCar_spec->collision_info->box_face_ref = (tU16)(gFace_num__car - 2);
    SetCarSuspGiveAndHeight(pCar_spec, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    pCar_spec->car_crush_spec->field_0x28 = 1;
    pCar_spec->field_0x18c8 = 0;
    car_crush->field_0x4b8 = 0;
    if (gProgram_state.racing != 0) {
        DRS3StartSound3D(gCar_outlet, 0x1484, &pCar_spec->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 0xff, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human) {
        SpendCredits(2000);
        sprintf(text, "%s %d", "REPAIR COST", 2000);
        NewTextHeadupSlot(4, 0, 0x5dc, -4, text);
    }
    AddCarDamageToPipingSession((tU16)pCar_spec->car_ID, &car_crush->field_0x548, 0);
}
// PARTIAL 75.91%: the CompressVector3 call and the DRScalarToU16 loop match exactly; the residual is
// the normalise prologue only. The original keeps the three components on the x87 stack and spills
// v[1]/v[2] with fst (0x43bcaa..0x43bd00), while this source makes MSVC 11 stage them through GP
// registers into the same spill slots, shifting the whole front block by a few instructions.
// FUNCTION: CARMA2_HW 0x0043bca0
void C2_HOOK_FASTCALL ARCompressCrush(undefined4* pArg1, void* pArg2, br_model* pModel, const br_vector3* pSrc, const float* pScalars) {
    br_vector3 normalised;
    float length;
    tU16 i;

    length = (float)sqrt(pSrc->v[1] * pSrc->v[1] + pSrc->v[2] * pSrc->v[2] + pSrc->v[0] * pSrc->v[0]);
    normalised.v[0] = pSrc->v[0] / length;
    normalised.v[1] = pSrc->v[1] / length;
    normalised.v[2] = pSrc->v[2] / length;
    CompressVector3((tCompressed_vector3*)pArg1, &normalised, -1.0f, 1.0f);
    for (i = 0; i < pModel->nvertices; i++) {
        ((tU16*)pArg2)[i] = DRScalarToU16(pScalars[i] * length, 0.0f, 10.0f);
    }
}


#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
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

extern br_scalar gCrushSqrtHalf;
extern br_scalar gFLOAT_00589acc;

// FUNCTION: CARMA2_HW 0x0043c480
void C2_HOOK_FASTCALL ReceivedCrush(tU8* pContents) {
    tCar_crush crush;
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    } context;
    br_actor* master_actor;
    tCar_spec* car;
    tCar_crush_spec* crush_spec;
    br_vector3 direction;
    float crush_y;
    float crush_z;
    float magnitude;
    int axis;
    int sign;
    int found;
    int count;
    int i;
    int j;
    tCar_crush_shape_info* shape;
    tCar_crush_reordered_shape_info* entry;

    car = NetCarFromPlayerID(*(tPlayer_ID*)(pContents + 4));
    if (car != NULL && gNet_mode != 2 && car->active == 0 && gArrow_mode == 0) {
        ExpandCrush(&crush, (tCompressed_car_crush*)(pContents + 8));
        crush_spec = car->car_crush_spec;
        master_actor = car->car_master_actor;
        if (crush_spec != NULL && car->invulnerable_no_crushage == 0 && crush_spec->field_0x4b8 == 0) {
            context.car = car;
            context.crush = &crush;
            context.force = crush.field_0x40;
            if (context.force > gMax_crush_force) {
                context.force = gMax_crush_force;
            }
            context.force = context.force / (float)crush.count;

            if (crush_spec->field_0x144 != 0) {
                ARStartPipingSession(3);
                DRActorEnumRecurseWithSnart(car->car_model_actor, NULL, SimpleCrushCarModelsCB, &context);
                AREndPipingSession();
                return;
            }

            crush_y = context.force * crush.field_0x44.v[1];
            crush_z = context.force * crush.field_0x44.v[2];
            direction.v[0] = gCrushZero;
            if (crush_y > gCrushZero) {
                crush_y = gCrushZero;
            }
            magnitude = (float)sqrt(crush_y * crush_y + crush_z * crush_z);
            if (magnitude >= 1e-05f) {
                direction.v[0] = gCrushZero / magnitude;
                direction.v[1] = crush_y / magnitude;
                direction.v[2] = crush_z / magnitude;
                if ((float)fabs(direction.v[0]) <= 0.707107f) {
                    axis = 1;
                    if ((float)fabs(direction.v[1]) <= 0.707107f) {
                        axis = 2;
                    }
                } else {
                    axis = 0;
                }
                sign = (direction.v[axis] > gCrushZero) ? 1 : 0;
                found = 0;
                count = crush_spec->field_0xa4.counts[axis][sign];
                if (count > 0) {
                    i = 0;
                    do {
                        if (sign == 0) {
                            if (crush_spec->field_0xbc.limits[axis][0].values[i] < crush.field_0x34.v[axis]) {
                                found = i;
                                break;
                            }
                        } else {
                            if (crush.field_0x34.v[axis] < crush_spec->field_0xbc.limits[axis][1].values[i]) {
                                found = i;
                                break;
                            }
                        }
                        i++;
                    } while (i < count);
                }

                if (crush_spec->count_shapes > 0) {
                    i = 0;
                    do {
                        shape = &crush_spec->field_0x4[i];
                        if (shape->field_0x8 == 0) {
                            for (j = 0; j < shape->count_points; j++) {
                                entry = &shape->field_0x18[j];
                                FUN_0043c7a0(&entry->field_0x18, crush.count, crush.vectors[0].v, &direction,
                                    entry->field_0x24 * magnitude, entry->field_0x28, &crush_spec->field_0xbc, found);
                                ClipPointToLimits(&entry->field_0x18, entry->field_0x28, &crush_spec->field_0xbc);
                            }
                        }
                        i++;
                    } while (i < crush_spec->count_shapes);
                }
                crush_spec->expand_bounding_box = 7;
            }

            ARStartPipingSession(3);
            DRActorEnumRecurseWithSnart(master_actor, NULL, CrushModels, &context);
            AREndPipingSession();
            car->use_shell_model = 0;
        }
    }
}

// GLOBAL: CARMA2_HW 0x00692dc8
int gINT_00692dc8 = 0;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0043c7a0
void C2_HOOK_FASTCALL FUN_0043c7a0(br_vector3* pVertex, int pCount, float* pVectors, br_vector3* pDirection, br_scalar pForce, tU32 pFlags, const tCar_crush_limits* pLimits, int pFound) {
    br_vector3 displacement;
    float force;
    int axis;

    force = GetImpactScale(pVertex, pCount, (br_vector3*)pVectors, gMax_crush_dist_sq);
    if (force != gCrushZero) {
        force = pForce * gForce_to_movement_factor * force;
        axis = NearestAxis(pDirection);
        LimitImpactScale(&force, axis, pVertex->v[axis], pDirection->v[axis], pFlags, (tCar_crush_limits*)pLimits, pFound);
        if (force != gCrushZero) {
            BrVector3Scale(&displacement, pDirection, force);
            BrVector3Accumulate(pVertex, &displacement);
        }
    }
    if (DRVector3TestForNan(pVertex)) {
        PDEnterDebugger("NaN");
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// PARTIAL 68.37%: ReceivedSemiDetachBit 0x0043c940 [regalloc/frame] - id kept in edi (orig eax), count in eax (orig ecx), desc slot 4 B off; state/findings/FUN_0043c940.md
// FUNCTION: CARMA2_HW 0x0043c940
void C2_HOOK_FASTCALL ReceivedSemiDetachBit(tU8* pContents)
{
    extern int gCount_net_crush_semi_detach_bit_list;
    struct {
        tU8 id;
        tU32 found;
    } desc;
    tCar_spec* car;
    intptr_t found;
    tU32 id;
    int count;
    int i;
    tCrush_net_semi_detach_bit_list_item* item;
    br_scalar scale;

    if (gCount_net_crush_semi_detach_bit_list == 8) {
        return;
    }
    car = NetCarFromPlayerID(*(tPlayer_ID*)(pContents + 4));
    if (car == NULL) {
        return;
    }
    desc.id = pContents[8];
    desc.found = 0;
    found = DRActorEnumRecurse(car->car_model_actor, FindBitWithID, &desc);
    id = (found != 0) ? desc.found : 0;
    if (id == 0) {
        return;
    }
    count = gCount_net_crush_semi_detach_bit_list;
    item = gNet_crush_semi_detach_bit_list;
    for (i = 0; i < count; i++, item++) {
        if (*(tU32*)&item->actor == id) {
            return;
        }
    }
    gNet_crush_semi_detach_bit_list[count].car = car;
    *(tU32*)&gNet_crush_semi_detach_bit_list[count].actor = id;
    *(tU32*)&gNet_crush_semi_detach_bit_list[count].field_0x8[0] = *(tU32*)(pContents + 0xc);
    ExpandVector3((br_vector3*)&gNet_crush_semi_detach_bit_list[count].field_0x8[4], (tCompressed_vector3*)(pContents + 0x10), -10.0f, 10.0f);
    ExpandVector3((br_vector3*)&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x8[0x28], (tCompressed_vector3*)(pContents + 0x22), -10.0f, 10.0f);
    ExpandVector3((br_vector3*)&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x8[0x34], (tCompressed_vector3*)(pContents + 0x28), -10.0f, 10.0f);
    ExpandVector3((br_vector3*)&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x8[0x10], (tCompressed_vector3*)(pContents + 0x16), -10.0f, 10.0f);
    ExpandVector3((br_vector3*)&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x8[0x1c], (tCompressed_vector3*)(pContents + 0x1c), -10.0f, 10.0f);
    scale = DRU16ToScalar(*(tU16*)(pContents + 0x2e), 0.0f, 40.0f);
    count = gCount_net_crush_semi_detach_bit_list;
    gCount_net_crush_semi_detach_bit_list = count + 1;
    *(br_scalar*)&gNet_crush_semi_detach_bit_list[count].field_0x8[0x40] = scale;
}

typedef struct {
    tU8 id;
    undefined field_0x1[3];
    br_actor* actor;
} tFind_crush_actor_arg;

// FUNCTION: CARMA2_HW 0x0043cac0
int C2_HOOK_CDECL FindBitWithID(br_actor* pActor, void* pArg) {
    tFind_crush_actor_arg* arg = pArg;
    tUser_crush_data* user_crush_data;

    user_crush_data = pActor->user;
    if (user_crush_data != NULL && user_crush_data->crush_data != NULL && user_crush_data->crush_data->id == arg->id) {
        arg->actor = pActor;
        return 1;
    }
    return 0;
}

typedef struct {
    undefined field_0x0[4];
    tPlayer_ID player_id;
    tU8 crush_id;
    undefined field_0x9[3];
    float time;
    tCompressed_vector3 min_pos;
    tCompressed_vector3 max_pos;
} tNet_crush_detach_msg;

// PARTIAL 87.60%: x87-free; arg stack init order, early-return epilogue differ
// FUNCTION: CARMA2_HW 0x0043caf0
void C2_HOOK_FASTCALL ReceivedDetachBit(void* pMsg_) {
    tNet_crush_detach_msg* pMsg = pMsg_;
    extern int gCount_net_crush_detach_list;
    extern tCrush_net_detach_list_item gNet_crush_detach_list[8];
    tFind_crush_actor_arg arg;
    tCar_spec* car;
    br_actor* actor;

    if (gCount_net_crush_detach_list != 8) {
        car = NetCarFromPlayerID(pMsg->player_id);
        if (car != NULL) {
            arg.id = pMsg->crush_id;
            arg.actor = NULL;
            actor = (br_actor*)((DRActorEnumRecurse(car->car_model_actor, FindBitWithID, &arg) != 0 ? 0xffffffff : 0) & (tU32)arg.actor);
            if (actor != NULL) {
                int i;

                for (i = 0; i < gCount_net_crush_detach_list; i++) {
                    if (gNet_crush_detach_list[i].actor == actor) {
                        return;
                    }
                }
                gNet_crush_detach_list[gCount_net_crush_detach_list].car = car;
                gNet_crush_detach_list[gCount_net_crush_detach_list].actor = actor;
                *(float*)&gNet_crush_detach_list[gCount_net_crush_detach_list].field_0x8[0] = pMsg->time;
                ExpandVector3((br_vector3*)&gNet_crush_detach_list[gCount_net_crush_detach_list].field_0x8[4], &pMsg->min_pos, -10.0f, 10.0f);
                ExpandVector3((br_vector3*)&gNet_crush_detach_list[gCount_net_crush_detach_list].field_0x8[16], &pMsg->max_pos, -10.0f, 10.0f);
                gCount_net_crush_detach_list++;
            }
        }
    }
}

// PARTIAL 38.81%: ReceivedFullyDetachBit 0x0043cbd0 [regalloc] - pContents/car register roles swapped (ebp/ebx), frame 0x4c with spill, neg/sbb found-test not reproduced
// FUNCTION: CARMA2_HW 0x0043cbd0
void C2_HOOK_FASTCALL ReceivedFullyDetachBit(tU8* pContents) {
    /* frame: +0x0 keep int, +0x4 {u8 id; pad[3]; br_actor* actor} (DRActorEnumRecurse arg),
       +0xc/+0x18/+0x24 vectors, +0x2c vector (ExpandVector3 dst), +0x30 bounds3 (min +0x30, max +0x3c). */
    extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);
    tU8 frame[0x48];
    tCar_spec* car;
    tCar_crush_spec* crush;
    tPhysics_object* bit;
    tPhysics_object** slot;
    br_actor* actor;
    intptr_t hit;
    tCrush_net_full_detach_bit_list_item* item;
    int count;
    int i;
    int n;

    if (gCount_net_crush_full_detach_bit_list == 8) {
        return;
    }
    car = NetCarFromPlayerID(*(tPlayer_ID*)(pContents + 4));
    if (car == NULL) {
        return;
    }
    bit = NULL;
    crush = car->car_crush_spec;
    if (crush != NULL) {
        count = crush->field_0x270;
        if (count > 0) {
            slot = (tPhysics_object**)((char*)crush + 0x27c);
            i = 0;
            do {
                if (*(tU8*)((char*)*slot + 0x190) == pContents[8]) {
                    bit = *slot;
                    break;
                }
                i++;
                slot += 5;
            } while (i < count);
        }
    }
    if (bit == NULL) {
        frame[4] = pContents[8];
        *(tU32*)(frame + 8) = 0;
        hit = DRActorEnumRecurse(car->car_model_actor, FindBitWithID, frame + 4);
        actor = (br_actor*)(-(tU32)(hit != 0) & (tU32)((tFind_crush_actor_arg*)(frame + 4))->actor);
        if (actor == NULL) {
            return;
        }
        ExpandVector3((br_vector3*)(frame + 0x2c), (tCompressed_vector3*)(pContents + 0x10), -10.0f, 10.0f);
        ExpandVector3((br_vector3*)(frame + 0x18), (tCompressed_vector3*)(pContents + 0x16), -10.0f, 10.0f);
        ExpandVector3((br_vector3*)(frame + 0xc), (tCompressed_vector3*)(pContents + 0x1c), -10.0f, 10.0f);
        ExpandVector3((br_vector3*)(frame + 0x30), (tCompressed_vector3*)(pContents + 0x22), -10.0f, 10.0f);
        ExpandVector3((br_vector3*)(frame + 0x3c), (tCompressed_vector3*)(pContents + 0x28), -10.0f, 10.0f);
        bit = SemiDetachBit(car, actor, 0.0f, (int*)frame, (br_vector3*)(frame + 0x24), (br_bounds3*)(frame + 0x30),
                            (br_vector3*)(frame + 0x18), (br_vector3*)(frame + 0xc));
        if (bit == NULL) {
            return;
        }
    }
    count = gCount_net_crush_full_detach_bit_list;
    if (count > 0) {
        item = gNet_crush_full_detach_bit_list;
        i = 0;
        do {
            if (item->object == bit) {
                return;
            }
            i++;
            item++;
        } while (i < count);
    }
    n = gCount_net_crush_full_detach_bit_list;
    gNet_crush_full_detach_bit_list[n].car = car;
    gNet_crush_full_detach_bit_list[n].object = bit;
    gCount_net_crush_full_detach_bit_list = n + 1;
    *(tU32*)(gNet_crush_full_detach_bit_list[n].field_0x8) = *(tU32*)(pContents + 0xc);
}

static __inline tCar_spec* AttachedBitFromID(tCar_crush_spec* pCar_crush, tU8 pId) {
    int i;

    if (pCar_crush != NULL) {
        for (i = 0; i < pCar_crush->field_0x2b0; i++) {
            if (*(tU8*)((char*)pCar_crush->field_0x2b4[i].field_0x8 + 0x190) == pId) {
                return (tCar_spec*)pCar_crush->field_0x2b4[i].field_0x0;
            }
        }
    }
    return NULL;
}

static __inline tCar_spec* SemiDetachedBitFromID(tCar_crush_spec* pCar_crush, tU8 pId) {
    int i;

    if (pCar_crush != NULL) {
        for (i = 0; i < pCar_crush->field_0x270; i++) {
            if (*(tU8*)((char*)pCar_crush->field_0x274[i].field_0x8 + 0x190) == pId) {
                return (tCar_spec*)pCar_crush->field_0x274[i].field_0x0;
            }
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0043cd60
void C2_HOOK_FASTCALL ReceivedReAttachBit(tU8* pContents) {
    extern int gCount_net_crush_reattach_bit_list;
    tCar_spec* car;
    tCar_crush_spec* crush;
    tCar_spec* bit;
    int count;
    int i;
    int n;

    if (gCount_net_crush_reattach_bit_list == 8) {
        return;
    }
    car = NetCarFromPlayerID(*(tPlayer_ID*)(pContents + 4));
    if (car == NULL) {
        return;
    }
    crush = car->car_crush_spec;
    bit = AttachedBitFromID(crush, pContents[8]);
    if (bit == NULL) {
        bit = SemiDetachedBitFromID(crush, pContents[8]);
        if (bit == NULL) {
            return;
        }
    }
    count = gCount_net_crush_reattach_bit_list;
    for (i = 0; i < count; i++) {
        if (gNet_crush_reattach_bit_list[i].actor == bit) {
            return;
        }
    }
    n = gCount_net_crush_reattach_bit_list;
    gNet_crush_reattach_bit_list[n].car = car;
    gNet_crush_reattach_bit_list[n].actor = bit;
    gNet_crush_reattach_bit_list[n].field_0x8 = *(tU32*)(pContents + 0xc);
    gCount_net_crush_reattach_bit_list = n + 1;
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

// FindBitWithID

// AttachedBitFromID

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

// ReceivedFullyDetachBit

// DetachedBitFromID

// SemiDetachedBitFromID

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
// GLOBAL: CARMA2_HW 0x00589c40
float gFloat_00589c40 = -0.5f;


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00441420
void C2_HOOK_FASTCALL ChristmasLights(tCar_spec* pCar_spec, int pMin_damage, int pMax_damage) {
    int damage_type;
    int damage_count;

    damage_type = 0;
    damage_count = 0;
    for (;;) {
        DamageUnit2(pCar_spec, (tDamage_type)damage_type, IRandomBetween(pMin_damage, pMax_damage));
        if (damage_type == eDamage_engine && pCar_spec->damage_units[eDamage_engine].damage_level < 0x50) {
            damage_count++;
            if (damage_count < 0x1e) {
                continue;
            }
        }
        damage_type++;
        if (damage_type >= 0xc) {
            pCar_spec->damage_magnitude_accumulator -= gFloat_00589c40;
            return;
        }
    }
}


// NetGetToPlayerContents, inlined with gNet_players[0].ID
extern tPlayer_ID gLocal_net_ID;
// FUNCTION: CARMA2_HW 0x0049f860
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tU8 pType, int pSize_decider) {
    int i;
    int chunk_size;
    int offset;
    tPlayer_ID target_id;
    tNet_message* message;
    tNet_message_chunk* chunk;
    tNet_game_player_info* player;

    target_id = gNet_players[0].ID;
    chunk_size = NetGetContentsSize(pType, pSize_decider);
    i = 0;
    if (gNumber_of_net_players <= 0) {
        return NULL;
    }
    for (; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == target_id) {
            goto found_player;
        }
    }
    return NULL;

found_player:
    player = &gNet_players[i];
    if (player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = player->field_0xcc;
    if (message != NULL && message->header.field_0x16 + chunk_size > 0x200) {
        tPlayer_ID player_id = player->ID;
        tNet_game_details* game = gCurrent_net_game;

        if (gNet_mode != eNet_mode_none) {
            message->header.field_0x8 = gLocal_net_ID;
            message->header.field_0x10 = PDGetTotalTime();
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == player_id) {
                    goto send_message;
                }
            }
            goto clear_message;

send_message:
            PDNetSendMessageToAddress(game, message, &gNet_players[i]);
        }
clear_message:
        player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        player->field_0xcc = NetAllocateMessage(0x200);
        message = player->field_0xcc;
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    chunk = (tNet_message_chunk*)((tU8*)message + offset);
    message->header.field_0x16 += (tU16)chunk_size;
    message->header.field_0x14 += 1;
    chunk->raw.header.type = pType;
    chunk->raw.header.size = (tU8)chunk_size;
    return chunk;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// GLOBAL: CARMA2_HW 0x0067bad8
tQueued_drone_crush gQueued_drone_crushes[4];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time);

// FUNCTION: CARMA2_HW 0x004a5bc0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused) {
    int i;
    int result;

    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guarantee message not built proper");
    }
    pMessage->header.field_0x14 = 1;
    ReceivedMessage(pMessage, &gNet_players[gThis_net_player_index], GetRaceTime());
    pMessage->header.field_0x14 = 0xff;
    result = 0;
    if (gNumber_of_net_players == 1) {
        ((tU8*)pMessage)[0x19] = 0xff;
        return 0;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            result |= (int)NetGuaranteedSendMessageToPlayer(pGame, pMessage, gNet_players[i].ID, (void*)pUnused);
        }
    }
    return result;
}

extern int gINT_00692dc8;

// FUNCTION: CARMA2_HW 0x004b9750
void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pObject) {
    pObject->field_0x218 = 0;
    MoveJointedObject(pObject, 0.f);
    gINT_00692dc8 = 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c6e40
void C2_HOOK_FASTCALL AddCrushToPipingSession(br_model* pModel, br_vector3* pSrc, const float* pScalars) {
    undefined4* crush_space;
    tU32 size;

    crush_space = gCrush_space;
    if (crush_space == NULL) {
        return;
    }
    size = (tU32)pModel->nvertices * 2 + 0xa;
    if (size > 0x4000) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    *(br_model**)crush_space = pModel;
    ARCompressCrush(crush_space + 1, (char*)crush_space + 0xa, pModel, pSrc, pScalars);
    ARAddDataToSession(3, (uintptr_t)-1, crush_space, (int)size);
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c7f40
void C2_HOOK_FASTCALL PipeSingleCrush(br_model* pModel, br_vector3* pSrc, const float* pScalars) {
    undefined4* crush_space;
    tU32 size;

    crush_space = gCrush_space;
    if (crush_space == NULL) {
        return;
    }
    size = (tU32)pModel->nvertices * 2 + 0xa;
    if (size > 0x4000) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    *(br_model**)crush_space = pModel;
    ARCompressCrush(crush_space + 1, (char*)crush_space + 0xa, pModel, pSrc, pScalars);
    ARDoSingleSession(3, (uintptr_t)-1, crush_space, size);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x004c7fa0
void C2_HOOK_FASTCALL FUN_004c7fa0(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
    extern tPipe_smudge_data* gSmudge_space;
    tU32 copy_size;

    if (gSmudge_space != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        copy_size = pVertex_count * sizeof(tSmudged_vertex);
        gSmudge_space->vertex_count = (tU16)pVertex_count;
        gSmudge_space->model_index = pModel_index;
        memcpy(gSmudge_space->vertex_changes, pCoordinates, copy_size);
        /* 8 = offsetof(tPipe_smudge_data, vertex_changes) */
        ARDoSingleSession(ePipe_chunk_smudge, pCar_ID, gSmudge_space, copy_size + 8);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c86b0
void C2_HOOK_FASTCALL PipeSingleRelink(uintptr_t pId, br_actor* pActor) {
    br_actor* old_parent;

    old_parent = pActor->parent;
    ARDoSingleVariedSession(0x19, (uintptr_t)pActor, 3,
        4, 0, (uintptr_t)pActor,
        4, 4, (uintptr_t)old_parent,
        4, 8, pId);
}
// FUNCTION: CARMA2_HW 0x004c86f0
void C2_HOOK_FASTCALL AddCarDamageToPipingSession(int pOwner, void* pData, tU32 pArg3) {
    ARDoSingleVariedSession(0x1b, pOwner, 2, 0x10, 0, pData, 4, 0x10, pArg3);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
extern tPowerup* gPowerup_array;
extern int gNumber_of_powerups;
extern int gNumber_of_icons;

// FUNCTION: CARMA2_HW 0x004df070
void C2_HOOK_FASTCALL LoseDodgyPowerups(tCar_spec* pCar_spec) {
    if (pCar_spec == NULL || pCar_spec->driver != eDriver_local_human) {
        return;
    }
    {
        int i;

        i = 0;
        if (gNumber_of_powerups > 0) {
            int j;
            int powerup_offset;
            int* car_powerup;
            tPowerup* powerup;
            tNet_message* message;

            powerup_offset = 0;
            car_powerup = pCar_spec->powerups;
            do {
                if (*car_powerup != 0) {
                    powerup = (tPowerup*)((tU8*)gPowerup_array + powerup_offset);
                    if (powerup->net_type & eNet_powerup_off_before_snapweld) {
                        if (powerup->got_time != 0 && powerup->lose_proc != NULL) {
                            powerup->lose_proc(powerup, powerup->car);
                        }
                        if (powerup->net_type & 0x80) {
                            PipeSinglePowerupLose(powerup, powerup->car);
                        }
                        if (gNet_mode != eNet_mode_none) {
                            message = NetBuildGuaranteedMessage(0x16, 0);
                            *(int*)((tU8*)message + 0x28) = 2;
                            *(tPlayer_ID*)((tU8*)message + 0x1c) = gLocal_net_ID;
                            *(int*)((tU8*)message + 0x20) = powerup - gPowerup_array;
                            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                        }
                        gProgram_state.current_car.powerups[powerup - gPowerup_array] = 0;
                        if (powerup->initial_value == 0) {
                            for (j = 0; j < gNumber_of_icons; j++) {
                                if (powerup == gPickedup_powerups[j].powerup) {
                                    gPickedup_powerups[j].fizzle_stage = 3;
                                    gPickedup_powerups[j].fizzle_direction = -1;
                                    gPickedup_powerups[j].fizzle_start = GetTotalTime();
                                    break;
                                }
                            }
                        }
                        powerup->got_time = 0;
                    }
                }
                i++;
                car_powerup++;
                powerup_offset += sizeof(tPowerup);
            } while (i < gNumber_of_powerups);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int gINT_006a3334;
extern tQueued_smash gQueued_smashes[20];
extern int gCount_queued_smashes;
extern br_matrix34 gShrapnel_shower_matrix;
extern br_vector3 gVector3_006a4688;
extern tSmashable_level* C2_HOOK_FASTCALL QueueSmashIfNecessary3(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, undefined4 pArg9, undefined4 pArg10);
// FUNCTION: CARMA2_HW 0x004ee250
void C2_HOOK_FASTCALL QueueSmashIfNecessary2(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, void* pArg4, br_actor* pActor, int pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, int pArg9) {
    gINT_006a3334 = 0;
    QueueSmashIfNecessary3(pCar, pCollision_info, pPos, pArg4, pActor, pArg6, pSmashable, pDamage, pArg9, 0);
}

// PARTIAL 61.05%: QueueSmashIfNecessary3 0x004ee290 - the pSmashable parameter lands in EDX and
// gCount_queued_smashes in EBP, the original has them the other way round (EBP / EDX). Pure
// global-register-allocator role swap: mnemonic stream, field offsets, statement order, loop forms
// and the x87 6.9 product are identical. Tried: local alias, decl order, initialisers, local count,
// pointer search loop, dword field_0x3c store (all in), a symbol pad - the swap does not move.
// FUNCTION: CARMA2_HW 0x004ee290
tSmashable_level* C2_HOOK_FASTCALL QueueSmashIfNecessary3(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, undefined4 pArg9, undefined4 pArg10) {
    tSmashable_level* result;
    tQueued_smash* q;
    int level;
    int current_level;
    int i;

    result = NULL;
    level = pSmashable->count_smashable_levels - 1;
    current_level = pSmashable->field_0x4c;
    if (current_level >= level) {
        return NULL;
    }
    if (gCount_queued_smashes >= 20) {
        return NULL;
    }
    if (pActor->model == NULL) {
        return NULL;
    }
    while (level > current_level) {
        if (pDamage >= pSmashable->levels[level].trigger_threshold) {
            q = gQueued_smashes;
            for (i = 0; i < gCount_queued_smashes; i++, q++) {
                if (q->field_0x10 == (undefined4)(uintptr_t)pSmashable) {
                    if (level <= *(int*)&q->field_0x38) {
                        return NULL;
                    }
                    *(int*)&q->field_0x38 = level;
                    result = &pSmashable->levels[level];
                    break;
                }
            }
            if (result == NULL) {
                gQueued_smashes[gCount_queued_smashes].car = pCar;
                gQueued_smashes[gCount_queued_smashes].field_0x4 = (undefined4)(uintptr_t)pCollision_info;
                gQueued_smashes[gCount_queued_smashes].actor = pActor;
                gQueued_smashes[gCount_queued_smashes].field_0x10 = (undefined4)(uintptr_t)pSmashable;
                gQueued_smashes[gCount_queued_smashes].field_0x48 = pArg6;
                *(int*)&gQueued_smashes[gCount_queued_smashes].field_0x38 = level;
                *(br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x2c = *pPos;
                *(undefined4*)&gQueued_smashes[gCount_queued_smashes].field_0x3c = pArg10;
                if (gINT_006a3334 != 0) {
                    gQueued_smashes[gCount_queued_smashes].field_0x40 = 1;
                    gQueued_smashes[gCount_queued_smashes].field_0x4c = gShrapnel_shower_matrix;
                    gQueued_smashes[gCount_queued_smashes].field_0x7c = gVector3_006a4688;
                } else {
                    gQueued_smashes[gCount_queued_smashes].field_0x40 = 0;
                }
                if (pOptional_position != NULL) {
                    gQueued_smashes[gCount_queued_smashes].field_0x20 = *pOptional_position;
                } else {
                    gQueued_smashes[gCount_queued_smashes].field_0x20 = gZero_vector__smash;
                }
                gQueued_smashes[gCount_queued_smashes].field_0x44 = pArg9;
                if (pCollision_info != NULL && pCollision_info->owner != NULL && pCollision_info->flags_0x238 == 1) {
                    ((br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x14)->v[0] = ((float*)pCollision_info->field_0x1b4)[0] * 6.9;
                    ((br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x14)->v[1] = ((float*)pCollision_info->field_0x1b4)[1] * 6.9;
                    ((br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x14)->v[2] = ((float*)pCollision_info->field_0x1b4)[2] * 6.9;
                } else {
                    ((br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x14)->v[0] = 0.f;
                    ((br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x14)->v[1] = 0.f;
                    ((br_vector3*)&gQueued_smashes[gCount_queued_smashes].field_0x14)->v[2] = 0.f;
                }
                gCount_queued_smashes++;
                result = &pSmashable->levels[level];
            }
            return result;
        }
        level--;
    }
    return NULL;
}



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
// Walks the car's master actor tree applying the crush callback to every actor.
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004f5a60
void C2_HOOK_FASTCALL SmashEverything(tCar_spec* pCar_spec) {
    DRActorEnumRecurse(pCar_spec->car_master_actor, SmashEverythingCB, pCar_spec);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// Per-actor crush pass callback (calls QueueSmashIfNecessary3 for each smashable part).
// PARTIAL 70.45%: SmashEverythingCB 0x004f5a70 - the original sinks the gINT_006a3334 = 0 store
// below the argument pushes (as if QueueSmashIfNecessary2, which is exactly "gINT = 0; Q3(...)",
// had been inlined there); MSVC keeps it in source order here, and reloads count_smashables into
// EAX where the original uses EDX. Everything else (loads, pushes, loop, epilogue) matches.
// FUNCTION: CARMA2_HW 0x004f5a70
intptr_t C2_HOOK_CDECL SmashEverythingCB(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_spec* car;
    int i;

    car = pContext;
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    for (i = 0; i < crush_data->count_smashables; i++) {
        gINT_006a3334 = 0;
        QueueSmashIfNecessary3(car, car->collision_info, &car->pos, NULL, pActor, 0, &crush_data->smashables[i], 1000.0f, 0, 0);
    }
    return 0;
}
