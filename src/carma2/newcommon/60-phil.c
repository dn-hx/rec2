#include "60-phil.h"
#include "04-netgame.h"
#include "rec2_macros.h"

#include "55-volume.h"

#include <math.h>
#include <stdarg.h>
#include <string.h>

extern void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList);

// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_disabled = 1;

// GLOBAL: CARMA2_HW 0x0065d008
br_scalar gFLOAT_0065d008 = 1.f;

// GLOBAL: CARMA2_HW 0x0065d00c
br_scalar gFLOAT_0065d00c = 1.f;

// GLOBAL: CARMA2_HW 0x0074a5f0
tPhysics_object* gPHIL_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5f8
int gCrush_deferred;

// GLOBAL: CARMA2_HW 0x006923e0
int gPHIL_count_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5e4
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x0074a5e8
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x006923e4
int gPHIL_object_added;

/* explicit initializers keep gPhil_queued_objects and the following globals adjacent, in address order (PHILInit loop-end compare) */
// GLOBAL: CARMA2_HW 0x006923f0
tPHIL_queued_objects gPhil_queued_objects = { 0 };

// GLOBAL: CARMA2_HW 0x00692db8
tWorld_callback_active_passive_cbfn* gPHIL_original_activate_passive = NULL;

// GLOBAL: CARMA2_HW 0x00692dbc
int gPHIL_count_queued_objects_for_removal = 0;

// GLOBAL: CARMA2_HW 0x00692dc0
int gPHIL_count_queued_objects = 0;


// FractionToScalar

// FUNCTION: CARMA2_HW 0x0049be90
// FUNCTION: CARMA2_SW 0x00498010
void C2_HOOK_FASTCALL GetOrientationFromMatrix(tFixed_quat* pOrientation, const br_matrix34* pMatrix) {
    br_quat quaternion;
    br_scalar value;

    BrMatrix34ToQuat(&quaternion, pMatrix);
    value = quaternion.x;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[0] = (tU16)(value + .5f);
    value = quaternion.y;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[1] = (tU16)(value + .5f);
    value = quaternion.z;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[2] = (tU16)(value + .5f);
    value = quaternion.w;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[3] = (tU16)(value + .5f);
}

// GetMatrixFromOrientation

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// PARTIAL 96.35: the hinge dot product (omega . hinge_axis) is scheduled differently -- VC5 loads all
// five operands first (5 fld, fxch, fmulp, fmulp, faddp, fmul, faddp = 28 B); VC11 evaluates it with 3
// x87 slots (30 B, +2 B), which also shifts the jump-table offsets at the switch dispatch. Same value.
// FUNCTION: CARMA2_HW 0x0049c0d0
int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, tU32 pSize) {
    tU16* pBuffer16;
    br_quat quat;
    float value;
    int i;

    if (pObject->physics_joint1 != NULL) {
        switch (pObject->physics_joint1->type) {
        case eJoint_hinge:
        case eJoint_quick_hinge:
            if (pSize < 0xc) {
                return -1;
            }
            pBuffer[0] = (tU8)pObject->physics_joint1->type;
            pBuffer[1] = pObject->uid;
            value = pObject->omega.v[0] * pObject->physics_joint1->hinge_axis.v[0]
                + pObject->omega.v[1] * pObject->physics_joint1->hinge_axis.v[1]
                + pObject->omega.v[2] * pObject->physics_joint1->hinge_axis.v[2];
            if (value > 39.269909f) {
                value = 39.269909f;
            }
            if (value < -39.269909f) {
                value = -39.269909f;
            }
            value = (value + 39.269909f) * 0.012732396f;
            value *= 65534.f;
            ((tU16*)pBuffer)[1] = (tU16)(value + .5f);
            pBuffer16 = (tU16*)(pBuffer + 4);
            BrMatrix34ToQuat(&quat, &pObject->transform_matrix);

            value = quat.x;
            if (value > 1.f) {
                value = 1.f;
            }
            if (value < -1.f) {
                value = -1.f;
            }
            value = (value + 1.f) * .5f;
            value *= 65534.f;
            pBuffer16[0] = (tU16)(value + .5f);

            value = quat.y;
            if (value > 1.f) {
                value = 1.f;
            }
            if (value < -1.f) {
                value = -1.f;
            }
            value = (value + 1.f) * .5f;
            value *= 65534.f;
            pBuffer16[1] = (tU16)(value + .5f);

            pBuffer16[2] = (tU16)QuantizeFloatToInt(quat.z, -1.f, 1.f);
            pBuffer16[3] = (tU16)QuantizeFloatToInt(quat.w, -1.f, 1.f);
            return 0xc;
        case eJoint_universal:
        case eJoint_ball_n_socket:
            if (pSize < 0x10) {
                return -1;
            }
            pBuffer[0] = (tU8)pObject->physics_joint1->type;
            pBuffer[1] = pObject->uid;
            pBuffer16 = (tU16*)(pBuffer + 2);
            for (i = 0; i < 3; i++) {
                value = pObject->omega.v[i];
                if (value > 39.269909f) {
                    value = 39.269909f;
                }
                if (value < -39.269909f) {
                    value = -39.269909f;
                }
                value = (value + 39.269909f) * 0.012732396f;
                value *= 65534.f;
                pBuffer16[i] = (tU16)(value + .5f);
            }
            BrMatrix34ToQuat(&quat, &pObject->transform_matrix);

            value = quat.x;
            if (value > 1.f) {
                value = 1.f;
            }
            if (value < -1.f) {
                value = -1.f;
            }
            value = (value + 1.f) * .5f;
            value *= 65534.f;
            ((tU16*)pBuffer)[4] = (tU16)(value + .5f);

            value = quat.y;
            if (value > 1.f) {
                value = 1.f;
            }
            if (value < -1.f) {
                value = -1.f;
            }
            value = (value + 1.f) * .5f;
            value *= 65534.f;
            ((tU16*)pBuffer)[5] = (tU16)(value + .5f);

            ((tU16*)pBuffer)[6] = (tU16)QuantizeFloatToInt(quat.z, -1.f, 1.f);
            ((tU16*)pBuffer)[7] = (tU16)QuantizeFloatToInt(quat.w, -1.f, 1.f);
            return 0x10;
        case eJoint_none:
            break;
        default:
            PhysicsError((tPhysicsError)9, "netphys got messed up");
        }
    }

    if (pSize < 0x28) {
        return -1;
    }
    pBuffer[0] = (tU8)eJoint_none;
    pBuffer[1] = pObject->uid;
    pBuffer16 = (tU16*)(pBuffer + 2);
    for (i = 0; i < 3; i++) {
        value = pObject->omega.v[i];
        if (value > 39.269909f) {
            value = 39.269909f;
        }
        if (value < -39.269909f) {
            value = -39.269909f;
        }
        value = (value + 39.269909f) * 0.012732396f;
        value *= 65534.f;
        pBuffer16[i] = (tU16)(value + .5f);
    }
    ((float*)(pBuffer + 8))[0] = pObject->v.v[0];
    ((float*)(pBuffer + 8))[1] = pObject->v.v[1];
    ((float*)(pBuffer + 8))[2] = pObject->v.v[2];
    ((float*)(pBuffer + 0x1c))[0] = pObject->transform_matrix.m[3][0];
    ((float*)(pBuffer + 0x1c))[1] = pObject->transform_matrix.m[3][1];
    ((float*)(pBuffer + 0x1c))[2] = pObject->transform_matrix.m[3][2];
    BrMatrix34ToQuat(&quat, &pObject->transform_matrix);

    value = quat.x;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    ((tU16*)(pBuffer + 0x14))[0] = (tU16)(value + .5f);

    value = quat.y;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    ((tU16*)(pBuffer + 0x14))[1] = (tU16)(value + .5f);

    ((tU16*)(pBuffer + 0x14))[2] = (tU16)QuantizeFloatToInt(quat.z, -1.f, 1.f);
    ((tU16*)(pBuffer + 0x14))[3] = (tU16)QuantizeFloatToInt(quat.w, -1.f, 1.f);
    return 0x28;
}

// FUNCTION: CARMA2_HW 0x0049c5e0
int C2_HOOK_STDCALL QuantizeFloatToInt(float pVal, float pMin, float pMax) {

    if (pVal > pMax) {
        pVal = pMax;
    }
    if (pVal < pMin) {
        pVal = pMin;
    }
    return (int)((pVal - pMin) / (pMax - pMin) * 65534.f - 0.5f);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

// FUNCTION: CARMA2_HW 0x0049cd00
int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize) {
    int size;
    int child_size;
    tPhysics_object* child;
    tU8* child_buffer;

    size = GetObjectNetworkStuff(pObject, pBuffer, pSize);
    if (size < 0) {
        return -1;
    }
    child = pObject->child;
    pSize -= size;
    if (child != NULL) {
        *pBuffer |= 0x80;
        for (; child != NULL; child = child->next) {
            child_buffer = pBuffer + size;
            child_size = GetHierarchyNetworkStuff(child, child_buffer, pSize);
            if (child_size < 0) {
                return -1;
            }
            pSize -= child_size;
            size += child_size;
            *child_buffer |= 0x40;
        }
        *child_buffer &= ~0x40;
    }
    return size;
}

// FindObject

// WriteHierarchyNetworkStuff

// The original inlines the joint size switch (no separate call), so keep it static.
static int SizeOfObjectNetworkStuff(tPhysics_joint_type pType) {
    switch (pType & 0xf) {
    case eJoint_none:
        return 0x28;
    case eJoint_hinge:
    case eJoint_quick_hinge:
        return 0xc;
    case eJoint_universal:
    case eJoint_ball_n_socket:
        return 0x10;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0049ce60
int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject) {
    int size;
    tPhysics_object* child;

    size = SizeOfObjectNetworkStuff(pObject->physics_joint1 != NULL ? pObject->physics_joint1->type : eJoint_none);
    for (child = pObject->child; child != NULL; child = child->next) {
        size += GetHierarchyNetworkSize(child);
    }
    return size;
}

// (reference only)
// FUNCTION: CARMA2_HW 0x0049cee0
int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data) {
    int size;
    tU8* next;
    tU8 more;

    switch (*pNet_data & 0xf) {
    case 0:
        size = 0x28;
        break;
    case 1:
    case 4:
        size = 0xc;
        break;
    case 2:
    case 3:
        size = 0x10;
        break;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        size = 0;
    }
    more = *pNet_data & 0x80;
    while (more != 0) {
        next = pNet_data + size;
        size += GetNetworkDataSize(next);
        more = *next & 0x40;
    }
    return size;
}

extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

// (reference only)
// FUNCTION: CARMA2_HW 0x0049cf50
int C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix) {
    br_quat quaternion;

    switch (pNet_data[0] & 0xF) {
    case 0:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 0x14))[0] * 1.5259255e-05f;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 0x14))[1] * 1.5259255e-05f;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 0x14))[2] * 1.5259255e-05f;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 0x14))[3] * 1.5259255e-05f;
        quaternion.x = quaternion.x + quaternion.x;
        quaternion.y = quaternion.y + quaternion.y;
        quaternion.z = quaternion.z + quaternion.z;
        quaternion.w = quaternion.w + quaternion.w;
        quaternion.x = quaternion.x - 1.0f;
        quaternion.y = quaternion.y - 1.0f;
        quaternion.z = quaternion.z - 1.0f;
        quaternion.w = quaternion.w - 1.0f;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        memcpy(pMatrix->m[3], pNet_data + 0x1c, sizeof(pMatrix->m[3]));
        return 0x28;
    case 1:
    case 4:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 4))[0] * 1.5259255e-05f;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 4))[1] * 1.5259255e-05f;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 4))[2] * 1.5259255e-05f;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 4))[3] * 1.5259255e-05f;
        quaternion.x = quaternion.x + quaternion.x;
        quaternion.y = quaternion.y + quaternion.y;
        quaternion.z = quaternion.z + quaternion.z;
        quaternion.w = quaternion.w + quaternion.w;
        quaternion.x = quaternion.x - 1.0f;
        quaternion.y = quaternion.y - 1.0f;
        quaternion.z = quaternion.z - 1.0f;
        quaternion.w = quaternion.w - 1.0f;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        return 0xc;
    case 2:
    case 3:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 8))[0] * 1.5259255e-05f;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 8))[1] * 1.5259255e-05f;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 8))[2] * 1.5259255e-05f;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 8))[3] * 1.5259255e-05f;
        quaternion.x = quaternion.x + quaternion.x;
        quaternion.y = quaternion.y + quaternion.y;
        quaternion.z = quaternion.z + quaternion.z;
        quaternion.w = quaternion.w + quaternion.w;
        quaternion.x = quaternion.x - 1.0f;
        quaternion.y = quaternion.y - 1.0f;
        quaternion.z = quaternion.z - 1.0f;
        quaternion.w = quaternion.w - 1.0f;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        return 0x10;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        return -1;
    }
}

// FUNCTION: CARMA2_HW 0x004b5cc0
int C2_HOOK_FASTCALL PHILInit(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(gPhil_queued_objects.headers[0]) != 0x18);

    for (i = 0; i < (int)REC2_ASIZE(gPhil_queued_objects.headers); i++) {
        gPhil_queued_objects.headers[i].collision_info = NULL;
    }
    gPHIL_count_list_collision_infos = 0;
    gPHIL_count_queued_objects = 0;
    gPHIL_count_queued_objects_for_removal = 0;
    gPHIL_list_collision_infos = NULL;
    gPHIL_munging_objects = 0;
    gCrush_deferred = 0;
    gPHIL_object_added = 0;
    gPHIL_last_physics_tick = 0;
    gPHIL_mechanics_time_sync = 1;
    gPHIL_disabled = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5d20
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_disabled = 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b5d40
int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject) {
    int i;
    tPHIL_queued_header* object_info;

    object_info = NULL;
    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects && !gPHIL_object_added) {
        gPhil_queued_objects.objects[gPHIL_count_queued_objects].collision_info = pObject;
        gPhil_queued_objects.objects[gPHIL_count_queued_objects].field_0x8 = 1;
        gPhil_queued_objects.objects[gPHIL_count_queued_objects].flags = 0;
        pObject->field_0x240 = &gPhil_queued_objects.objects[gPHIL_count_queued_objects];
        pObject->field_0x239 = 2;
        gPHIL_count_queued_objects += 1;
        return 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gPhil_queued_objects.headers); i++) {
        tPHIL_queued_header* current_object_info = &gPhil_queued_objects.headers[i];

        if (current_object_info->collision_info == NULL) {
            if (object_info == NULL) {
                object_info = current_object_info;
            }
        } else if (pObject == current_object_info->collision_info) {
            return 2;
        }
    }
    if (object_info != NULL) {
        memset(object_info, 0, sizeof(*object_info));
        object_info->collision_info = pObject;
        object_info->field_0x8 = 1;
        pObject->field_0x240 = object_info;
        pObject->flags |= 0x20;
        pObject->field_0x239 = 1;
        gPHIL_count_list_collision_infos += 1;
        if (gPHIL_list_collision_infos != NULL) {
            pObject->next = gPHIL_list_collision_infos->next;
            gPHIL_list_collision_infos->next = pObject;
            pObject->prev = gPHIL_list_collision_infos;
            if (pObject->next != NULL) {
                pObject->next->prev = pObject;
            }
        } else {
            gPHIL_list_collision_infos = pObject;
            pObject->next = NULL;
            pObject->prev = NULL;
        }
        if (gCrush_deferred) {
            PrepareObject(pObject, &gPHIL_list_collision_infos);
        }
        return 0;
    } else {
        return 1;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern tPhysics_object* gPHIL_queued_objects_for_removal[1];

// FUNCTION: CARMA2_HW 0x004b5ea0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {
    int i;
    tPhysics_object* object_info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        ((tPHIL_queued_header*)pObject->field_0x240)->collision_info = NULL;
        pObject->field_0x240 = NULL;
        pObject->field_0x239 = 0;
        return 0;
    }
    if (gPHIL_munging_objects && !gPHIL_object_added) {
        gPHIL_queued_objects_for_removal[gPHIL_count_queued_objects_for_removal] = pObject;
        gPHIL_count_queued_objects_for_removal = gPHIL_count_queued_objects_for_removal + 1;
        return 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gPhil_queued_objects.headers); i++) {
        if (gPhil_queued_objects.headers[i].collision_info == pObject) {
            break;
        }
    }
    if (i != (int)REC2_ASIZE(gPhil_queued_objects.headers) && pObject->field_0x240 != NULL) {
        ((tPHIL_queued_header*)pObject->field_0x240)->collision_info = NULL;
        pObject->field_0x240 = NULL;
        gPHIL_count_list_collision_infos = gPHIL_count_list_collision_infos + -1;
        if (pObject == gPHIL_list_collision_infos) {
            gPHIL_list_collision_infos = pObject->next;
            if (pObject->next != NULL) {
                pObject->next->prev = NULL;
            }
        }
        for (i = 0; i < (int)REC2_ASIZE(gPhil_queued_objects.headers); i++) {
            object_info = gPhil_queued_objects.headers[i].collision_info;
            if (object_info != NULL && object_info->next == pObject) {
                gPhil_queued_objects.headers[i].collision_info->next = pObject->next;
                if (pObject->next != NULL) {
                    pObject->next->prev = gPhil_queued_objects.headers[i].collision_info;
                }
                break;
            }
        }
        return 0;
    }
    return 3;
}

// FUNCTION: CARMA2_HW 0x004b5fd0
tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    if (gPHIL_disabled) {
        return NULL;
    } else {
        return gPHIL_list_collision_infos;
    }
}

// FUNCTION: CARMA2_HW 0x004b5ff0
// FUNCTION: CARMA2_SW 0x004b2710
tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject) {
    if (gPHIL_disabled) {
        return NULL;
    } else {
        return pObject->next;
    }
}
// FUNCTION: CARMA2_HW 0x004b6010
tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tCollision_info_owner* owner;

    if (!gPHIL_disabled) {
        if (pObject->field_0x239 == 2) {
            return ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
        }
        owner = pObject->field_0x240;
        if (owner != NULL) {
            return owner->field_0x08;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6040
// FUNCTION: CARMA2_SW 0x004b2760
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 1;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 1;
        pObject->flags |= 0x20;
        return 0;
    } else {
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b6090
// FUNCTION: CARMA2_SW 0x004b27b0
int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 2;
        if (pVelocity != NULL) {
            info->field_0x18 = *pVelocity;
            info->flags |= 0x1;
        }
        if (pOmega != NULL) {
            info->field_0x24 = *pOmega;
            info->flags |= 0x2;
        }
        if (pFlag) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 2;
        pObject->flags &= ~0x20;
        if (pFlag) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        if (pVelocity != NULL) {
            pObject->v = *pVelocity;
        }
        if (pOmega != NULL) {
            pObject->omega = *pOmega;
        }
        ResetCarSpecialVolume(pObject);
        return 0;
    } else {
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b61a0
// FUNCTION: CARMA2_SW 0x004b28c0
int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix) {
    tPHIL_queued_object_info* queued;
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        queued = pObject->field_0x240;
        queued->field_0x30 = *pMatrix;
        queued->flags |= 0x4;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        if (info->field_0x8 != 1) {
            return 4;
        }
        info->collision_info->actor->t.t.mat = *pMatrix;
        return 0;
    } else {
        return 3;
    }
}

/* the body after the gPHIL_disabled test; inlined (its edi save appears after that test in the original); name invented */
static int C2_HOOK_FASTCALL PHILAddActiveObjectInner(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4) {
    int result;

    result = PHILAddObject(pInfo);
    if (result != 0) {
        return result;
    }
    if (pMatrix != NULL && !gPHIL_disabled) {
        if (pInfo->field_0x239 == 2) {
            tQueued_object_info* object_info = pInfo->field_0x240;
            object_info->field_0x30 = *pMatrix;
            object_info->flags |= 0x4;
        } else {
            tQueued_object_info* object_info = pInfo->field_0x240;
            if (object_info != NULL) {
                if (object_info->field_0x8 != 1) {
                    return 4;
                }
                object_info->object->actor->t.t.mat = *pMatrix;
            } else {
                return 3;
            }
        }
    }
    return PHILMakeObjectActive(pInfo, pArg3, pArg4, 0);
}


// PHILSetObjectProperty


// FUNCTION: CARMA2_HW 0x004b6530
float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object *pCollision_info, int pParam) {
    tCollision_info_owner* owner;

    if (!gPHIL_disabled) {
        owner = (tCollision_info_owner*)(pCollision_info->field_0x239 == 2 ? pCollision_info->field_0x240 : pCollision_info->field_0x240);
        if (owner != NULL) {
            if (pParam >= 0 && pParam < 9) {
                switch (pParam) {
                case 0:
                    return owner->field_0x10;
                case 1:
                    return (float)!!(owner->field_0x04 & 0x1);
                case 2:
                    return owner->field_0x0c;
                case 3:
                    return (float)!!(pCollision_info->flags & 0x40);
                case 4:
                    return (float)!!(pCollision_info->flags & 0x8);
                case 5:
                    return (float)!!(pCollision_info->flags & 0x10);
                case 6:
                    return (float)!!(owner->field_0x04 & 0x4);
                case 7:
                    return (float)!!(owner->field_0x04 & 0x8);
                case 8:
                    return owner->field_0x14;
                }
            } else {
                return 5.f;
            }
        }
    }
    return 0.f;
}

// FUNCTION: CARMA2_HW 0x004b6210
int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4) {

    if (gPHIL_disabled) {
        return 0;
    }
    return PHILAddActiveObjectInner(pInfo, pMatrix, pArg3, pArg4);
}

// FUNCTION: CARMA2_HW 0x004b62b0
int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddObject(pObject);
    gPHIL_object_added = 0;
    return result;
}


// FUNCTION: CARMA2_HW 0x004b62e0
int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddActiveObject(pInfo, pMatrix, pArg3, pArg4);
    gPHIL_object_added = 0;
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int gFace_num__car;
extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);

// FUNCTION: CARMA2_HW 0x004b63b0
int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...) {
    va_list va;
    tPHIL_queued_header* owner;
    int value;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 != 2) {
        owner = pObject->field_0x240;
    } else {
        owner = pObject->field_0x240;
    }
    if (owner != NULL) {
    if (pProperty < 0 || pProperty >= 9) {
        return 5;
    }
    va_start(va, pProperty);
    switch (pProperty) {
    case 0:
        owner->field_0x10 = (float)va_arg(va, double);
        break;
    case 1:
        if (va_arg(va, int)) {
            owner->field_0x4 |= 1;
            PositionChildren(pObject);
        } else {
            owner->field_0x4 &= 1;
        }
        break;
    case 2:
        owner->field_0xc = (float)va_arg(va, double);
        break;
    case 3:
        value = (va_arg(va, int) != 0) & 1;
        pObject->flags = (pObject->flags & ~0x40) | (value << 6);
        if (!(pObject->flags & 0x40)) {
            pObject->box_face_ref = (tU16)((tU32)gFace_num__car - 2u);
        }
        break;
    case 4:
        value = (va_arg(va, int) != 0) & 1;
        pObject->flags = (pObject->flags & ~8) | (value << 3);
        break;
    case 5:
        value = (va_arg(va, int) != 0) & 1;
        pObject->flags = (pObject->flags & ~0x10) | (value << 4);
        break;
    case 6:
        if (va_arg(va, int)) {
            owner->field_0x4 |= 4;
        } else {
            owner->field_0x4 &= 4;
        }
        break;
    case 7:
        if (va_arg(va, int)) {
            owner->field_0x4 |= 8;
            pObject->water_d = 10000.f;
        } else {
            owner->field_0x4 &= 8;
        }
        break;
    case 8:
        owner->field_0x14 = (float)va_arg(va, double);
        break;
    }
    va_end(va);
    return 0;
    }
    return 3;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

// FUNCTION: CARMA2_HW 0x004b6e90
void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject) {
    tPhysics_object* child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xf0, 0xf0);

    pObject->field_0xf0 = 2;
    for (child = pObject->child; child != NULL; child = child->next) {
        MarkObjectAndChildrenAsPassive(child);
    }
}

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// GLOBAL: CARMA2_HW 0x006923e8
tPhysics_callbacks* gPHIL_callbacks;

// GLOBAL: CARMA2_HW 0x006923ec
tPhysics_object* gPHIL_queued_objects_for_removal[1];

// GLOBAL: CARMA2_HW 0x0074a5f4
int gOver_shoot;

extern void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b71b0
void C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* pObject, tPhysics_object* pActivator) {
    if (pObject->field_0x240 == NULL || ((tPHIL_queued_header*)pObject->field_0x240)->field_0x8 != 1) {
        pObject->flags |= 0x1000;
        return;
    }
    if (gPHIL_original_activate_passive != NULL) {
        gPHIL_original_activate_passive(pObject);
    }
    if (((int (C2_HOOK_FASTCALL*)(tPhysics_object*, tPhysics_object*))gPHIL_callbacks->passive_activated) != NULL) {
        if (((int (C2_HOOK_FASTCALL*)(tPhysics_object*, tPhysics_object*))gPHIL_callbacks->passive_activated)(pObject, pActivator)) {
            PHILMakeObjectActive(pObject, NULL, NULL, 1);
        }
    } else {
        PHILMakeObjectActive(pObject, NULL, NULL, 1);
    }
}
extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject);

// FUNCTION: CARMA2_HW 0x004b6be0
void C2_HOOK_FASTCALL ChangedObjectsCallbacks(tPhysics_object* pObjects, tPhysics_callbacks* pCallbacks, tU32 pPeriod) {
    tPhysics_object* pObject;
    tCollision_info_owner* owner;
    tPhysics_object* child;
    tU32 val;

    for (pObject = pObjects; pObject != NULL; pObject = pObject->next) {
        if (!gPHIL_disabled) {
            if (pObject->field_0x239 == 2) {
                val = ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
            } else if (pObject->field_0x240 != NULL) {
                val = ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
            } else {
                val = 0;
            }
        } else {
            val = 0;
        }
        if (val != 2) {
            continue;
        }
        if (pObject->disable_move_rotate) {
            owner = pObject->field_0x240;
            if (owner == NULL) {
                continue;
            }
            if (!(owner->field_0x04 & 0x2)) {
                continue;
            }
            if (pCallbacks->active_halted != NULL && !pCallbacks->active_halted(pObject)) {
                continue;
            }
            if (gPHIL_disabled) {
                continue;
            }
            if (pObject->field_0x239 == 2) {
                ((tCollision_info_owner*)pObject->field_0x240)->field_0x08 = 1;
            } else {
                owner = pObject->field_0x240;
                if (owner == NULL) {
                    continue;
                }
                owner->field_0x08 = 1;
                pObject->flags |= 0x20;
            }
        } else {
            if (pCallbacks->changed_objects != NULL) {
                pCallbacks->changed_objects(pObject, pPeriod);
            }
            owner = pObject->field_0x240;
            if (owner == NULL) {
                continue;
            }
            if (!(owner->field_0x04 & 0x4)) {
                continue;
            }
            for (child = pObject; child != NULL; child = child->child) {
                PipeSinglePHILObject(child);
            }
        }
    }
}
extern void C2_HOOK_FAKE_THISCALL InterpolateSingleObject(tPhysics_object* pObject, br_scalar pDt);
extern void C2_HOOK_FASTCALL PhysicsObjectSetImpulse(tPhysics_object* pObject, br_vector3* pImpulse);
extern void C2_HOOK_FASTCALL PhysicsObjectMoveVelocity(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL DRVector3Diminish(br_vector3* pV1, const br_vector3* pV2);
extern void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV);
extern float gGravity_multiplier;

// PARTIAL 88.61%: RepositionObjectGroup 0x004c2830 - body complete; remaining diff is x87 scheduling of the
// three v*dt products (the original keeps them in st registers, this build spills/reorders by two instructions).
// FUNCTION: CARMA2_HW 0x004c2830
void C2_HOOK_FAKE_THISCALL RepositionObjectGroup(tPhysics_object* pObject, float pDt) {
    tPhysics_object* pChild;
    float t0;
    float t1;
    float t2;

    t0 = pObject->v.v[0] * pDt;
    t1 = pObject->v.v[1] * pDt;
    t2 = pObject->v.v[2] * pDt;
    pObject->actor->t.t.mat.m[3][0] += t0;
    pObject->actor->t.t.mat.m[3][1] += t1;
    pObject->actor->t.t.mat.m[3][2] += t2;
    InterpolateSingleObject(pObject, pDt);
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        RepositionObjectGroup(pChild, pDt);
    }
}
// GLOBAL: CARMA2_HW 0x00692dd0
float gFLOAT_00692dd0;

// GLOBAL: CARMA2_HW 0x00692dd4
float gFLOAT_00692dd4;

// GLOBAL: CARMA2_HW 0x006940a0
int gINT_006940a0;

extern int gPrepared_objects;
extern int gINT_006940a4;
extern int gINT_00692ddc;
extern tPhysics_joint_force gCollision_contacts[50];
extern void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void);
extern void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL DRMatrix33Inverse(tPhysics_object* pObject, int pFlag);
extern int C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object** pObject_list, tPhysics_object* pObject, tPhysics_joint_force* pContacts, tPhysics_joint_force* pJoint_data, int pJoint_count, tWorld_callbacks* pWorld_callbacks);
extern void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage);
extern int C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL SetCollisionInfoDoNothing(tPhysics_object* pCollision_info, tU32 pDisable);
extern void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* pContact, tWorld_callbacks* pWorld_callbacks);


// PARTIAL 73.82%: DoCollisions 0x004ba6b0 - structure, call sites and globals match; this build keeps
// pWorld_callbacks in ebx so the sort loop's `head` lands in esi instead of the original's ebx.
// FUNCTION: CARMA2_HW 0x004ba6b0
void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* pWorld_callbacks) {
    tPhysics_joint_force bufB[0x28];
    tPhysics_joint_force bufA[0x28];
    tPhysics_joint_force* pBuf1;
    tPhysics_joint_force* pBuf2;
    tPhysics_joint_force* tmp;
    tPhysics_object* obj;
    tPhysics_object* head;
    tPhysics_object* tail;
    tPhysics_object* cur;
    tPhysics_object* next;
    tPhysics_object* pos;
    tPhysics_object* sibling;
    tPhysics_object* node;
    tPhysics_joint_force* contact;
    tU32 marker;
    int mask;
    int loops;
    int i;
    int r;

    pBuf1 = bufA;
    pBuf2 = bufB;

    ClearPhysicsScratchSpace();

    gINT_006940a0 = 1;
    gINT_00692ddc = 0;
    gFLOAT_00692dd0 = 0.f;
    gFLOAT_00692dd4 = 0.f;

    for (obj = *pObject_list; obj != NULL; obj = obj->next) {
        InternalPrepareObject(obj);
    }

    head = *pObject_list;
    head->prev = NULL;
    tail = head;
    cur = head->next;
    while (cur != NULL) {
        pos = tail;
        while (pos != NULL && cur->transform_matrix.m[3][0] < pos->transform_matrix.m[3][0]) {
            pos = pos->prev;
        }
        if (pos == tail) {
            tail->next = cur;
            cur->prev = tail;
            tail = cur;
            cur = cur->next;
        } else if (pos != NULL) {
            next = cur->next;
            cur->next = pos->next;
            pos->next->prev = cur;
            pos->next = cur;
            cur->prev = pos;
            cur = next;
        } else {
            head->prev = cur;
            next = cur->next;
            cur->next = head;
            head = cur;
            cur->prev = NULL;
            cur = next;
        }
    }
    tail->next = NULL;
    *pObject_list = head;

    do {
        gPrepared_objects = 0;
        for (obj = *pObject_list; obj != NULL; obj = obj->next) {
            if (obj->field_0x1dc != 0) {
                continue;
            }
            if (obj->disable_move_rotate != 0) {
                continue;
            }
            if (obj->child != NULL && obj->child->physics_joint1 != NULL && obj->child->physics_joint1->type != 0) {
                marker = gINT_006940a0;
                obj->field_0x1dc = (tU8)marker;
                for (sibling = obj->child; sibling != NULL; sibling = sibling->next) {
                    if (sibling->physics_joint1 != NULL && sibling->physics_joint1->type != 0) {
                        DRMatrix33Inverse(sibling, marker);
                    }
                }
                gINT_006940a0++;
            }
            loops = 0;
            gINT_006940a4 = 0;
            r = SetUpQuickHingeData(pObject_list, obj, pBuf1, pBuf2, 0, pWorld_callbacks);
            while (r != 0) {
                loops++;
                tmp = pBuf1;
                pBuf1 = pBuf2;
                pBuf2 = tmp;
                if (loops >= 0x1e) {
                    break;
                }
                if (gINT_006940a4 >= 3) {
                    break;
                }
                r = SetUpQuickHingeData(pObject_list, obj, pBuf1, pBuf2, r, pWorld_callbacks);
            }
            if (loops >= 0x1e) {
                PhysicsWarning("Too many loops in DoCollisions");
            }
        }
    } while (gPrepared_objects != 0);

    for (i = 0, contact = gCollision_contacts; i < gINT_00692ddc; i++, contact++) {
        NewAddFriction(contact, pWorld_callbacks);
    }

    AddDoubleTorqueToMatrix(*pObject_list);

    for (obj = *pObject_list; obj != NULL; obj = obj->next) {
        if (obj->disable_move_rotate != 0) {
            continue;
        }
        if (!(obj->field_0x1df & 0x2)) {
            continue;
        }
        mask = 0x2;
        for (node = (tPhysics_object*)obj->field183_0x1d8; node != obj; node = (tPhysics_object*)node->field183_0x1d8) {
            mask &= node->field_0x1df;
        }
        if (!(mask & 0x2)) {
            continue;
        }
        obj->disable_move_rotate = 1;
        for (cur = obj;; cur = (tPhysics_object*)cur->field183_0x1d8) {
            sibling = cur->child;
            cur->disable_move_rotate = 1;
            cur->field_0x1df = 0;
            if (sibling != NULL) {
                for (sibling = cur->child; sibling != NULL; sibling = sibling->next) {
                    sibling->disable_move_rotate = 1;
                    sibling->field_0x1df = 0;
                    if (sibling->child != NULL) {
                        SetCollisionInfoDoNothing(sibling->child, 1);
                    }
                }
            }
            if (cur->field183_0x1d8 == 0 || (tPhysics_object*)cur->field183_0x1d8 == obj) {
                break;
            }
        }
    }

    for (obj = *pObject_list; obj != NULL; obj = obj->next) {
        if (obj->disable_move_rotate == 0) {
            TranslateObject(obj);
        }
    }
}

// PARTIAL 99.99%: ProcessGravity 0x004b6ec0 - only the two commutative operands of the first product
// (gravity_multiplier * water_depth_factor) are loaded in the other order; semantics identical.
// FUNCTION: CARMA2_HW 0x004b6ec0
void C2_HOOK_FASTCALL ProcessGravity(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pGravity) {
    br_vector3 gravity;
    tSpecial_volume* pSpecial_volume;

    pSpecial_volume = NULL;
    if (pObject_info->field_0x4 & 0x8) {
        pSpecial_volume = pObject->last_special_volume;
    }
    if (pSpecial_volume != NULL) {
        if (pObject_info->field_0x14 == 0.0f || pSpecial_volume->viscosity_multiplier <= 2.0) {
            gravity.v[1] = pObject->water_depth_factor * pSpecial_volume->gravity_multiplier * pGravity * gGravity_multiplier * -0.05681159420289855;
            gravity.v[0] = 0.0f;
            gravity.v[2] = 0.0f;
        } else {
            gravity.v[0] = 0.0f;
            gravity.v[1] = ((1.0f - pSpecial_volume->gravity_multiplier) * pObject->water_depth_factor * pObject_info->field_0x14 - gGravity_multiplier * 1.4202898550724639 * pGravity) * 0.04;
            gravity.v[2] = 0.0f;
        }
    } else {
        gravity.v[0] = 0.0f;
        gravity.v[2] = 0.0f;
        gravity.v[1] = pGravity * gGravity_multiplier * -0.05681159420289855;
    }
    PhysicsObjectSetImpulse(pObject, &gravity);
    PhysicsObjectMoveVelocity(pObject);
}
// PARTIAL 79.86%: ProcessDrag2 0x004b6ce0 - body complete; remaining diff is x87 scheduling in the drag
// coefficient and the two length/scale blocks plus one commutative operand swap.
// FUNCTION: CARMA2_HW 0x004b6ce0
void C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pDrag, int pObject_info_flags, tSpecial_volume* pSpecial_volume) {
    br_scalar drag;
    br_scalar s;
    tSpecial_volume* sp;
    br_vector3 v;

    drag = pDrag * 2e-05;
    sp = NULL;
    if (pObject_info_flags & 8) {
        sp = pSpecial_volume;
    }
    if (sp != NULL) {
        if (pObject_info->field_0x14 == 0.0f || sp->viscosity_multiplier <= 2.0) {
            drag = sp->viscosity_multiplier * pObject->water_depth_factor * drag;
        } else {
            drag = pObject->water_depth_factor * sp->viscosity_multiplier * drag * 0.5;
        }
    }

    s = BrVector3Length(&pObject->v);
    v.v[0] = pObject->v.v[0] * (s * drag * 6.9 / pObject->M);
    v.v[1] = pObject->v.v[1] * (s * drag * 6.9 / pObject->M);
    v.v[2] = pObject->v.v[2] * (s * drag * 6.9 / pObject->M);
    DRVector3Diminish(&pObject->v, &v);

    s = BrVector3Length(&pObject->omega);
    v.v[0] = pObject->omega.v[0] * (s * drag);
    v.v[1] = pObject->omega.v[1] * (s * drag);
    v.v[2] = pObject->omega.v[2] * (s * drag);
    ApplyWaterOmegaBrake(pObject, &v);

    for (pObject = pObject->child; pObject != NULL; pObject = pObject->next) {
        ProcessDrag2(pObject_info, pObject, pDrag, pObject_info_flags, pSpecial_volume);
    }
}
// PARTIAL 40.41%: LevelOutOnSurface 0x004b6fb0 - body complete; this build keeps the negated normal in st
// registers and multiplies by water_normal from memory, the original stores n first and multiplies by n.
// FUNCTION: CARMA2_HW 0x004b6fb0
void C2_HOOK_FASTCALL LevelOutOnSurface(tPhysics_object* pObject) {
    br_scalar v0;
    br_scalar v1;
    br_scalar v2;
    br_scalar dot;
    br_scalar p1;
    br_scalar tv;
    br_scalar px;
    br_scalar py;
    br_scalar pz;
    br_vector3 n;
    br_vector3 u;

    if (pObject->I.v[0] > pObject->I.v[1] && pObject->I.v[0] > pObject->I.v[2]) {
        v0 = pObject->actor->t.t.mat.m[0][0];
        v1 = pObject->actor->t.t.mat.m[0][1];
        v2 = pObject->actor->t.t.mat.m[0][2];
    } else if (pObject->I.v[1] > pObject->I.v[2] && pObject->I.v[1] > pObject->I.v[0]) {
        v0 = pObject->actor->t.t.mat.m[1][0];
        v1 = pObject->actor->t.t.mat.m[1][1];
        v2 = pObject->actor->t.t.mat.m[1][2];
    } else {
        v0 = pObject->actor->t.t.mat.m[2][0];
        v1 = pObject->actor->t.t.mat.m[2][1];
        v2 = pObject->actor->t.t.mat.m[2][2];
    }

    n.v[0] = -1.0f * v0;
    n.v[1] = -1.0f * v1;
    n.v[2] = -1.0f * v2;

    dot = pObject->water_normal.v[2] * n.v[2] + pObject->water_normal.v[0] * n.v[0] + pObject->water_normal.v[1] * n.v[1];

    if (dot > -0.99) {
        px = pObject->water_normal.v[1] * n.v[2] - pObject->water_normal.v[2] * n.v[1];
        py = pObject->water_normal.v[2] * n.v[0] - pObject->water_normal.v[0] * n.v[2];
        pz = pObject->water_normal.v[0] * n.v[1] - pObject->water_normal.v[1] * n.v[0];
        p1 = dot + 1.0f;
        tv = sqrt(px * px + py * py + pz * pz);
        u.v[0] = px * (p1 / tv);
        tv = sqrt(pz * pz + py * py + u.v[0] * u.v[0]);
        u.v[1] = py * (p1 / tv);
        tv = sqrt(u.v[1] * u.v[1] + pz * pz + u.v[0] * u.v[0]);
        u.v[2] = pz * (p1 / tv);
        BrMatrix34TApplyV(&n, &u, &pObject->actor->t.t.mat);
        pObject->omega.v[0] += n.v[0];
        pObject->omega.v[1] += n.v[1];
        pObject->omega.v[2] += n.v[2];
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);

/* inlined into PHILDoPhysics by the original (both bodies appear inside it) */
static void C2_HOOK_FASTCALL PHILMungeObjects(tPhysics_object* pObjects) {
    tPhysics_object* object;

    for (object = pObjects; object != NULL; object = object->next) {
        tPHIL_queued_header* object_info;
        tSpecial_volume* original_last_special_volume;
        float original_water_depth_factor;

        object_info = object->field_0x240;
        if (!object->disable_move_rotate && object_info != NULL && object_info->field_0x8 == 2) {
            if (object_info->field_0x4 & 0x8) {
                original_water_depth_factor = object->water_depth_factor;
                original_last_special_volume = object->last_special_volume;
                if (object->water_d != 10000.f) {
                    TestAutoSpecialVolume(object);
                }
                MungeSpecialVolume(object);
            }
            if (object_info->field_0x10 != 0.f || (object->last_special_volume != NULL && (object_info->field_0x4 & 0x8))) {
                float gravity;

                gravity = object_info->field_0x10 != 0.f ? object_info->field_0x10 : 1.0;
                ProcessGravity(object_info, object, gravity);
            }
            if (object_info->field_0xc != 0.f || (object->last_special_volume != NULL && (object_info->field_0x4 & 0x8))) {
                float drag;

                drag = object_info->field_0xc != 0.f ? object_info->field_0xc : 1.0;
                ProcessDrag2(object_info, object, drag, object_info->field_0x4, object->last_special_volume);
            }
            if (object_info->field_0x4 & 0x8) {
                if (!(object->last_special_volume != NULL && object->last_special_volume->viscosity_multiplier > 2.0 && object->water_depth_factor == 1.0)
                        || (original_last_special_volume != NULL && original_last_special_volume->viscosity_multiplier > 2.0 && original_water_depth_factor >= 1.0)) {

                    if (!(object->last_special_volume != NULL && object->last_special_volume->viscosity_multiplier > 2.0)
                            && (original_last_special_volume != NULL && original_last_special_volume->viscosity_multiplier > 2.0)) {
                        ProcessDrag2(object_info, object, gFLOAT_0065d00c, object_info->field_0x4, object->last_special_volume);
                    }
                } else {
                    ProcessDrag2(object_info, object, gFLOAT_0065d008, object_info->field_0x4, object->last_special_volume);
                }
                if (object_info->field_0x14 != 0.f
                        && object->last_special_volume != NULL
                        && object->last_special_volume->viscosity_multiplier > 2.0
                        && object->water_depth_factor < 1.0) {
                    LevelOutOnSurface(object);
                }
            }
            if (object_info->field_0x4 & 0x1) {
                PositionChildren(object);
            }
        } else if (object_info != NULL && object_info->field_0x8 == 1) {
            MarkObjectAndChildrenAsPassive(object);
        }
    }
}

/* inlined into PHILDoPhysics by the original */
static void C2_HOOK_FASTCALL FlushQueuedAddsAndRemoves(void) {
    int i;

    for (i = 0; i < gPHIL_count_queued_objects; i++) {
        tPHIL_queued_object_info* queued_object_info;
        tPHIL_queued_header* object_info;

        queued_object_info = &gPhil_queued_objects.objects[i];
        if (queued_object_info->collision_info == NULL) {
            continue;
        }
        PHILAddObject(queued_object_info->collision_info);
        if (queued_object_info->flags & 0x4) {
            PHILSetPassiveObjectsMatrix(queued_object_info->collision_info, &queued_object_info->field_0x30);
        }
        if (queued_object_info->field_0x8 == 2) {
            br_vector3* omega;
            br_vector3* v;

            v = (queued_object_info->flags & 0x1) ? &queued_object_info->field_0x18 : NULL;
            omega = (queued_object_info->flags & 0x2) ? &queued_object_info->field_0x24 : NULL;
            PHILMakeObjectActive(queued_object_info->collision_info, v, omega, 0);
        }
        object_info = queued_object_info->collision_info->field_0x240;
        if (object_info != NULL) {
            object_info->field_0x4 &= ~0x2;
            object_info->field_0x4 |= queued_object_info->field_0x4 & ~0x2;
            object_info->field_0xc = queued_object_info->field_0xc;
            object_info->field_0x10 = queued_object_info->field_0x10;
            object_info->field_0x14 = queued_object_info->field_0x14;
        }
    }
    for (i = 0; i < gPHIL_count_queued_objects_for_removal; i++) {
        tPhysics_object* object;

        object = gPHIL_queued_objects_for_removal[i];
        if (object != NULL) {
            PHILRemoveObject(object);
        }
    }
}

/* inlined twice by the original (once per PHILDoPhysics branch) */
static void C2_HOOK_FASTCALL PHILInterpolateObjects(tPhysics_object* pObjects, tU32 pTime) {
    tPhysics_object* object;
    float dt;

    dt = (int)(gPHIL_last_physics_tick - pTime) * 0.001;
    if (!(dt <= .04 && dt >= .0f)) {
        dt = .0f;
    }
    gOver_shoot = dt > .0f;
    for (object = pObjects; object != NULL; object = object->next) {
        tPHIL_queued_object_info* object_info;

        object_info = object->field_0x240;
        if (object_info != NULL && object_info->field_0x8 == 2) {
            RepositionObjectGroup(object, -dt);
        }
    }
}

// PARTIAL 83.67%: PHILDoPhysics 0x004b6630 - code structure and all call sites match; remaining diff is register
// allocation in the inlined PHILMungeObjects loop and the deferred push esi/edi of the prologue.
// FUNCTION: CARMA2_HW 0x004b6630
void C2_HOOK_FASTCALL PHILDoPhysics(tPhysics_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period) {
    tU32 now;
    int iteration;

    iteration = 0;
    if (gPHIL_disabled) {
        return;
    }
    now = pLast_tick_time + pFrame_period;
    gPHIL_callbacks = pCallbacks;
    gPHIL_original_activate_passive = pCallbacks->world_callbacks->activate_passive;
    pCallbacks->world_callbacks->activate_passive = (tWorld_callback_active_passive_cbfn*)PHILActivatePassive;
    if (gPHIL_last_physics_tick < pLast_tick_time) {
        gPHIL_last_physics_tick = 40 * (pLast_tick_time / 40);
    }
    if (now <= gPHIL_last_physics_tick) {
        ResetObjectList(gPHIL_list_collision_infos);
        PHILInterpolateObjects(gPHIL_list_collision_infos, now);
    } else {
        gCrush_deferred = 1;
        gPHIL_mechanics_time_sync = now - gPHIL_last_physics_tick;
        while (gPHIL_last_physics_tick < now && iteration < 5) {

            iteration += 1;
            if (pCallbacks->pre_collision != NULL) {
                pCallbacks->pre_collision();
            }
            PHILMungeObjects(gPHIL_list_collision_infos);
            gPHIL_munging_objects = 1;
            if (gPHIL_list_collision_infos != NULL) {
                DoCollisions(&gPHIL_list_collision_infos, pCallbacks->world_callbacks);
            }
            gPHIL_munging_objects = 0;
            if (pCallbacks->post_collision != NULL) {
                pCallbacks->post_collision();
            }
            FlushQueuedAddsAndRemoves();
            gPHIL_last_physics_tick += 40;
            gPHIL_mechanics_time_sync -= 40;
            pLast_tick_time = gPHIL_mechanics_time_sync;
        }
        PHILInterpolateObjects(gPHIL_list_collision_infos, now);
        ChangedObjectsCallbacks(gPHIL_list_collision_infos, pCallbacks, pFrame_period);
        gPHIL_mechanics_time_sync = 1;
        gCrush_deferred = 0;
    }
    pCallbacks->world_callbacks->activate_passive = gPHIL_original_activate_passive;
}

// PHILApplyPHILObject


extern void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject);
extern void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time);
extern void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);
extern void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);
extern int gPrepared_objects;
extern tPhysics_object** gReduced_object_list;
extern int gINT_006940ac;

/* inlined into PrepareObject in the original (no calls to these) */
static void C2_HOOK_FASTCALL PHILSwapPair(tPhysics_object* pObject1, tPhysics_object* pObject2) {
    tPhysics_object* object1_prev;
    tPhysics_object* object2_next;

    object1_prev = pObject1->prev;
    object2_next = pObject2->next;
    if (object1_prev != NULL) {
        object1_prev->next = pObject2;
    }
    if (object2_next != NULL) {
        object2_next->prev = pObject1;
    }
    pObject1->prev = pObject2;
    pObject2->prev = object1_prev;
    pObject1->next = object2_next;
    pObject2->next = pObject1;
}

static void C2_HOOK_FASTCALL CheckObjectsPostionInList(tPhysics_object* pObject, tPhysics_object** pList) {
    tPhysics_object* neighbor;

    neighbor = pObject->prev;
    if (neighbor != NULL && pObject->transform_matrix.m[3][0] < neighbor->transform_matrix.m[3][0]) {
        do {
            PHILSwapPair(neighbor, pObject);
            neighbor = pObject->prev;
        } while (neighbor != NULL && pObject->transform_matrix.m[3][0] < neighbor->transform_matrix.m[3][0]);
        if (neighbor == NULL) {
            *pList = pObject;
        }
    } else {
        neighbor = pObject->next;
        if (neighbor != NULL && pObject->transform_matrix.m[3][0] > neighbor->transform_matrix.m[3][0]) {
            do {
                PHILSwapPair(pObject, neighbor);
                neighbor = pObject->next;
            } while (neighbor != NULL && pObject->transform_matrix.m[3][0] > neighbor->transform_matrix.m[3][0]);
        }
    }
}

static void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList) {
    tPhysics_object* child;
    tPhysics_object* current;

    if (pObject->disable_move_rotate) {
        while (pObject->parent != NULL && pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none) {
            pObject = pObject->parent;
        }
        pObject->box_face_ref = gFace_num__car - 2;
        pObject->disable_move_rotate = 0;
        if (pObject->child != NULL || pObject->physics_joint1 != NULL || pObject->physics_joint2 != NULL) {
            if (pObject->field_0x218 == 0) {
                MoveJointedObject(pObject, 0.f);
            }
            for (child = pObject->child; child != NULL; child = child->next) {
                if (child->disable_move_rotate) {
                    child->box_face_ref = gFace_num__car - 2;
                    child->disable_move_rotate = 0;
                }
                if (child->child != NULL) {
                    MakeObjectListDoSomething(child->child);
                }
            }
        }
        for (current = pList; current != NULL; current = current->next) {
            if (current->disable_move_rotate) {
                CheckForObjectHierachyTouchingAnotherObject(pObject, current, pList);
                child = pObject->child;
                if (child != NULL && current->disable_move_rotate) {
                    while (child != NULL) {
                        if (CheckForObjectHierachyTouchingAnotherObject(child, current, pList)) {
                            break;
                        }
                        child = child->next;
                    }
                }
                if (current->child != NULL) {
                    CheckForObjectHierachyTouchingObjectList(pObject, current->child, pList);
                }
            }
        }
    }
}

// PARTIAL 95.24: VC5 keeps a redundant NULL re-test of `child` before the child loop (test edi,edi at
// 0x4b9987) that VC11 proves dead and removes, so every later branch target is 4 bytes short; the extra
// reload of gReduced_object_list (writing *gReduced_object_list) compensates 1 of those bytes. Reading
// gReduced_object_list through a local pointer reproduces the original tail but scores 94.89 (the tail
// then mis-aligns); #pragma optimize("g", off) drops the whole body to /Od (10.17). Optimizer-version
// artifact, not source-controllable.
// FUNCTION: CARMA2_HW 0x004b9770
void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList) {
    InternalPrepareObject(pObject);
    gPrepared_objects = 1;
    CheckObjectsPostionInList(pObject, pList);
    if (gINT_006940ac) {
        MakeObjectDoSomething(pObject, *pList);
    }
    if (gReduced_object_list != NULL) {
        pObject->field_0x234 = *gReduced_object_list;
        *gReduced_object_list = pObject;
    }
}

// GLOBAL: CARMA2_HW 0x00692dcc
int gPrepared_objects;

// GLOBAL: CARMA2_HW 0x006940a8
tPhysics_object** gReduced_object_list;

// GLOBAL: CARMA2_HW 0x006940ac
int gINT_006940ac;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004b9f90
int C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original) {
    tPhysics_object* obj;
    tPhysics_object* child;
    tPhysics_object* current;

    if (pObject->field_0xf4.min.v[0] - pList->field_0xf4.max.v[0] > 0.01
        || pObject->field_0xf4.min.v[1] - pList->field_0xf4.max.v[1] > 0.01
        || pObject->field_0xf4.min.v[2] - pList->field_0xf4.max.v[2] > 0.01
        || pList->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] > 0.01
        || pList->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] > 0.01
        || pList->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] > 0.01) {
        return 0;
    }
    obj = pList;
    if (obj->disable_move_rotate) {
        while (obj->parent != NULL && obj->physics_joint1 != NULL && obj->physics_joint1->type != eJoint_none) {
            obj = obj->parent;
        }
        obj->disable_move_rotate = 0;
        obj->box_face_ref = gFace_num__car - 2;
        if (obj->child != NULL || obj->physics_joint1 != NULL || obj->physics_joint2 != NULL) {
            if (obj->field_0x218 == 0) {
                MoveJointedObject(obj, 0.f);
            }
            for (child = obj->child; child != NULL; child = child->next) {
                if (child->disable_move_rotate) {
                    child->disable_move_rotate = 0;
                    child->box_face_ref = gFace_num__car - 2;
                }
                if (child->child != NULL) {
                    MakeObjectListDoSomething(child->child);
                }
            }
        }
        for (current = pList_original; current != NULL; current = current->next) {
            if (current->disable_move_rotate) {
                CheckForObjectHierachyTouchingAnotherObject(obj, current, pList_original);
                child = obj->child;
                if (child != NULL && current->disable_move_rotate) {
                    while (child != NULL) {
                        if (CheckForObjectHierachyTouchingAnotherObject(child, current, pList_original)) {
                            break;
                        }
                        child = child->next;
                    }
                }
                if (current->child != NULL) {
                    CheckForObjectHierachyTouchingObjectList(obj, current->child, pList_original);
                }
            }
        }
    }
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
