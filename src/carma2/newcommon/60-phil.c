#include "60-phil.h"
#include "04-netgame.h"
#include "rec2_macros.h"

#include "55-volume.h"

#include <stdarg.h>
#include <string.h>

extern void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList);

// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_disabled = 1;

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
            value = pObject->omega.v[0] * pObject->physics_joint1->hinge_axis.v[0];
            value += pObject->omega.v[1] * pObject->physics_joint1->hinge_axis.v[1];
            value += pObject->omega.v[2] * pObject->physics_joint1->hinge_axis.v[2];
            if (value > 39.269909f) {
                value = 39.269909f;
            }
            if (value < -39.269909f) {
                value = -39.269909f;
            }
            value = (value + 39.269909f) * 0.012732396f;
            value *= 65534.f;
            ((tU16*)pBuffer)[1] = (tU16)(value + .5f);
            BrMatrix34ToQuat(&quat, &pObject->transform_matrix);
            pBuffer16 = (tU16*)(pBuffer + 4);

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
                *pBuffer16++ = (tU16)(value + .5f);
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
        *pBuffer16++ = (tU16)(value + .5f);
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

// based on carpocalypse2 src/carma2/common/phil.c:126 (reference only)
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

// based on carpocalypse2 src/carma2/common/phil.c:157 (reference only)
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
    tPHIL_queued_header* object_info;

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
    i = 0;
    object_info = gPhil_queued_objects.headers;
    while (object_info->collision_info != pObject) {
        object_info++;
        i = i + 1;
        if ((int)object_info >= (int)&gPhil_queued_objects.headers[100]) {
            break;
        }
    }
    if (i != 100 && pObject->field_0x240 != NULL) {
        ((tPHIL_queued_header*)pObject->field_0x240)->collision_info = NULL;
        pObject->field_0x240 = NULL;
        gPHIL_count_list_collision_infos = gPHIL_count_list_collision_infos + -1;
        if (pObject == gPHIL_list_collision_infos) {
            gPHIL_list_collision_infos = pObject->next;
            if (pObject->next != NULL) {
                pObject->next->prev = NULL;
            }
        }
        i = 0;
        object_info = gPhil_queued_objects.headers;
        while (object_info->collision_info == NULL || object_info->collision_info->next != pObject) {
            object_info++;
            i = i + 1;
            if ((int)object_info >= (int)&gPhil_queued_objects.headers[100]) {
                return 0;
            }
        }
        gPhil_queued_objects.headers[i].collision_info->next = pObject->next;
        if (pObject->next != NULL) {
            pObject->next->prev = gPhil_queued_objects.headers[i].collision_info;
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
// STUB: PHILMungeObjects
void C2_HOOK_FASTCALL PHILMungeObjects(tPhysics_object* pObjects) { NOT_IMPLEMENTED(); }
// STUB: PHILInterpolateObjects
void C2_HOOK_FASTCALL PHILInterpolateObjects(tPhysics_object* pObjects, tU32 pTime) { NOT_IMPLEMENTED(); }
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
// STUB: FlushQueuedAddsAndRemoves
void C2_HOOK_FASTCALL FlushQueuedAddsAndRemoves(void) { NOT_IMPLEMENTED(); }
// STUB: ChangedObjectsCallbacks
void C2_HOOK_FASTCALL ChangedObjectsCallbacks(tPhysics_object* pObjects, tPhysics_callbacks* pCallbacks, tU32 pPeriod) { NOT_IMPLEMENTED(); }
// STUB: RepositionObjectGroup
void C2_HOOK_FAKE_THISCALL RepositionObjectGroup(tPhysics_object* pObject, float pDt) { NOT_IMPLEMENTED(); }
// STUB: DoCollisions
void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* pWorld_callbacks) { NOT_IMPLEMENTED(); }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on carpocalypse2 src/carma2/common/physics.c (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004b6630
void C2_HOOK_FASTCALL PHILDoPhysics(tPhysics_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period) {
    tU32 now;
    int iteration;

    iteration = 0;
    if (gPHIL_disabled) {
        return;
    }
    now = pLast_tick_time + pFrame_period;
    gPHIL_original_activate_passive = pCallbacks->world_callbacks->activate_passive;
    pCallbacks->world_callbacks->activate_passive = (tWorld_callback_active_passive_cbfn*)PHILActivatePassive;
    gPHIL_callbacks = pCallbacks;
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
        {
            tPhysics_object* object;
            float dt;

            dt = (gPHIL_last_physics_tick - now) * 0.001;
            if (!(dt <= .04 && dt >= .0f)) {
                dt = .0f;
            }
            gOver_shoot = dt > .0f;
            for (object = gPHIL_list_collision_infos; object != NULL; object = object->next) {
                tPHIL_queued_object_info* object_info;

                object_info = object->field_0x240;
                if (object_info != NULL && object_info->field_0x8 == 2) {
                    RepositionObjectGroup(object, -dt);
                }
            }
        }
        ChangedObjectsCallbacks(gPHIL_list_collision_infos, pCallbacks, pFrame_period);
        gPHIL_mechanics_time_sync = 1;
        gCrush_deferred = 0;
    }
    pCallbacks->world_callbacks->activate_passive = gPHIL_original_activate_passive;
}

// PHILApplyPHILObject


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004b9770
void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
