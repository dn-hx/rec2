/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include "18-graphics2.h"
#include "30-opponent.h"
#include "globvars.h"
#include "41-utility.h"
#include "13-crush1.h"
#include "51-smash.h"
#include "32-spark.h"

// GLOBAL: CARMA2_HW 0x006768fc
// GLOBAL: CARMA2_SW 0x0067106c
int gPaused;

// GLOBAL: CARMA2_HW 0x0058f2d8
// GLOBAL: CARMA2_SW 0x0058a2d8
const int gPow2_array[32] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

// GLOBAL: CARMA2_HW 0x0058f600
// GLOBAL: CARMA2_SW 0x0058a600
tU8 gCamera_type_allowed_replay[9] = {
    1, 1, 1, 1, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f610
// GLOBAL: CARMA2_SW 0x0058a610
tU8 gCamera_type_allowed_gameplay[9] = {
    1, 0, 0, 0, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f620
// GLOBAL: CARMA2_SW 0x0058a620
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x0058f6a4
// GLOBAL: CARMA2_SW 0x0058a6a4
int gCar_car_collisions = 1;

// GLOBAL: CARMA2_HW 0x0058f6b0
// GLOBAL: CARMA2_SW 0x0058a6b0
const float gCar_simplification_factor[2][5] = {
    { 50.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.5f, 0.0f }
};

// GLOBAL: CARMA2_HW 0x0058f7f0
// GLOBAL: CARMA2_SW 0x0058a7f0
tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

// ActualActionReplayHeadups

// FUNCTION: CARMA2_HW 0x004e6260
// FUNCTION: CARMA2_SW 0x004e2280
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {
    RemoveTransientBitmaps(1);
}

// DoZappyActionReplayHeadups

// DoActionReplayHeadups

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// SynchronizeActionReplay

// TurnOnActionReplay

// TurnOffActionReplay

// FUNCTION: CARMA2_HW 0x004e72d0
// FUNCTION: CARMA2_SW 0x004e3110
void C2_HOOK_FASTCALL AfterActionReplay(void) {
    ForceRebuildActiveCarList();
}

// FUNCTION: CARMA2_HW 0x004e7350
void C2_HOOK_FASTCALL DrawStatusMessage(int pReplay_only) {
    extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
    extern void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText);
    extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
    extern tGraf_data* gCurrent_graf_data;
    extern tDR_font gFonts[28];
    extern int gShow_status_message;
    extern int gNext_replay_record_state;
    extern int gStatus_message_1;
    extern int gStatus_message_2;
    int x;
    int y;

    if (gShow_status_message != 0) {
        if (gNext_replay_record_state == 4) {
            x = gCurrent_graf_data->field_0x350;
            y = gCurrent_graf_data->field_0x354;
            if (pReplay_only == 0) {
                DRPixelmapText(gBack_screen, x, y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
            }
        } else {
            x = gCurrent_graf_data->width / 2;
            y = gCurrent_graf_data->height / 2;
            DRPixelmapCentredText(gBack_screen, x, y - 0x40, &gFonts[4], GetMiscString(gStatus_message_1));
            if (gStatus_message_2 != 0) {
                DRPixelmapCentredText(gBack_screen, x, y - 0x20, &gFonts[6], GetMiscString(gStatus_message_2));
            }
        }
    }
}

// ToggleReplay

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
// FUNCTION: CARMA2_SW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
// FUNCTION: CARMA2_SW 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gPaused = pPause;
}

// FUNCTION: CARMA2_HW 0x0040b500
void C2_HOOK_FASTCALL FUN_0040b500(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4) {
    if (pCharacter->field_0x4 >= 0) {
        pCharacter->field_0xa = (tS8)pArg4;
        pCharacter->personality->form->actor_sets[(tS8)pCharacter->field_0x4].actors[pBone_index]->model = (br_model*)pArg3;
    }
}

// FUNCTION: CARMA2_HW 0x0040c660
br_uint_8 C2_HOOK_CDECL ActorToActorMatrix34Local(br_matrix34* m, br_actor* a, br_actor* b) {
    br_uint_8 transform_type_combine[7][7] = {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1, 2, 2 },
        { 0, 1, 1, 1, 1, 3, 3 },
        { 0, 1, 1, 1, 1, 4, 4 },
        { 0, 1, 2, 3, 4, 5, 5 },
        { 0, 1, 2, 3, 4, 5, 6 },
    };
    br_matrix34 mat_a;
    br_matrix34 mat_b;
    br_matrix34 mat_c;
    br_uint_8 a_type;
    br_uint_8 b_type;

    if (a == b) {
        BrMatrix34Identity(m);
        return BR_TRANSFORM_IDENTITY;
    }
    if (a->parent == b) {
        BrTransformToMatrix34(m, &a->t);
        return (br_uint_8)a->t.type;
    }
    if (b->parent == a) {
        BrTransformToMatrix34(&mat_b, &b->t);
        if (b->t.type != BR_TRANSFORM_MATRIX34) {
            BrMatrix34LPInverse(m, &mat_b);
        } else {
            BrMatrix34Inverse(m, &mat_b);
        }
        return (br_uint_8)b->t.type;
    }
    a_type = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mat_a);
    b_type = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mat_b);
    while (a != NULL && b != NULL && a != b) {
        if (a->depth > b->depth) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_a, &a->t);
                a_type = transform_type_combine[a_type][a->t.type];
            }
            a = a->parent;
        } else if (b->depth > a->depth) {
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_b, &b->t);
                b_type = transform_type_combine[b_type][b->t.type];
            }
            b = b->parent;
        } else {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_a, &a->t);
                a_type = transform_type_combine[a_type][a->t.type];
            }
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_b, &b->t);
                b_type = transform_type_combine[b_type][b->t.type];
            }
            b = b->parent;
            a = a->parent;
        }
    }
    if (b_type == BR_TRANSFORM_IDENTITY) {
        BrMatrix34Copy(m, &mat_a);
        return a_type;
    }
    if (a_type == BR_TRANSFORM_IDENTITY) {
        if (b_type != BR_TRANSFORM_MATRIX34) {
            BrMatrix34LPInverse(m, &mat_b);
        } else {
            BrMatrix34Inverse(m, &mat_b);
        }
        return b_type;
    }
    if (b_type != BR_TRANSFORM_MATRIX34) {
        BrMatrix34LPInverse(&mat_c, &mat_b);
    } else {
        BrMatrix34Inverse(&mat_c, &mat_b);
    }
    BrMatrix34Mul(m, &mat_a, &mat_c);
    return transform_type_combine[a_type][b_type];
}

// FUNCTION: CARMA2_HW 0x0043c0e0
void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tU16 field_0x8) {
    br_actor* actor;
    tUser_crush_data* user;
    tUser_crush_data* parent_user;
    br_model* parent_model;
    tCar_crush_flap_data* flap;
    const br_vector3* hinge;
    br_vector3 axis;
    br_scalar nx, ny, nz;

    actor = (br_actor*)field_0x4;
    user = actor->user;
    if (user == NULL || user->crush_data == NULL) {
        return;
    }
    flap = user->crush_data->flap_data;
    if (flap == NULL) {
        return;
    }
    flap->field_0x4 = (tU16)(flap->field_0x4 + field_0x8);
    hinge = &user->models[0]->vertices[flap->hinge0].p;
    BrVector3Sub(&axis, &user->models[0]->vertices[flap->hinge1].p, hinge);
    BrVector3Normalise(&axis, &axis);
    nx = -hinge->v[0];
    ny = -hinge->v[1];
    nz = -hinge->v[2];
    BrMatrix34Translate(&actor->t.t.mat, nx, ny, nz);
    BrMatrix34PostRotate(&actor->t.t.mat, flap->field_0x4, &axis);
    parent_user = (tUser_crush_data*)actor->parent->user;
    parent_model = parent_user->models[0];
    BrMatrix34PostTranslate(&actor->t.t.mat, parent_model->vertices[flap->field_0xc].p.v[0], parent_model->vertices[flap->field_0xc].p.v[1], parent_model->vertices[flap->field_0xc].p.v[2]);
}

// FUNCTION: CARMA2_HW 0x0043c3d0
void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine) {
    tCar_spec* car;
    tCar_crush_spec* spec;
    tUser_crush_data* user;
    int i;

    car = roadMachine;
    spec = car->car_crush_spec;
    user = (tUser_crush_data*)car->car_model_actor->user;
    for (i = 0; i < car->count_detail_levels; i++) {
        if (user->models[i] != NULL) {
            BrModelFree(BrModelRemove(user->models[i]));
            user->models[i] = (br_model*)*(undefined4*)&spec->field_0x4c4[i * 4];
        }
    }
    car->car_model_actor->model = user->models[0];
    BrModelFree(BrModelRemove(spec->model_actor->model));
    DRActorEnumRecurse(car->car_model_actor, MakeCarModelsMaterialsSingleSided, car);
}

// FUNCTION: CARMA2_HW 0x004b7220
void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4) {
    tPhysics_object* object;

    object = (tPhysics_object*)field_0x0;
    BrMatrix34Copy(&object->transform_matrix, pField_0x4);
    BrMatrix34Copy(&object->actor->t.t.mat, pField_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca500
void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38) {
    extern int gBOOL_00744804;
    extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);
    extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);
    extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
    int current_move_id;

    if (!gBOOL_00744804) {
        if (field_0x4 != -1) {
            current_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
            BrMatrix34Copy(&pPed->character->field_0x8c, (br_matrix34*)pField_0x38);
            SetCharacterMove(pPed->character, field_0x4, *(float*)&field_0xc, field_0x14, field_0x8_1, GetTotalTime());
            if (!(pPed->character->field_0x14 & 4)) {
                SetCharacterPosition(pPed->character, pField_0x24, -1);
            }
            if (current_move_id != field_0x4) {
                pPed->character->field_0x1c = (tU16)zero;
                SetCharacterBonePositions(pPed->character, 0, 0);
            }
        }
        if (field_0x34 != -1) {
            pPed->action = (tPed_action)field_0x34;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca8d0
void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8) {
    extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, undefined4 pArg2, int pArg3);
    extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4);
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (field_0x4 < 0) {
        SetCharacterAllBonesModel(ped->character, field_0x6, field_0x8);
    } else {
        SetCharacterBoneModel(ped->character, field_0x4, field_0x6, field_0x8);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004eb8f0
void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 pOwner, tU32 pShrapnel, tU32 pSeed, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, tU32 pActor, tU32 pTime, tU32 pArg9, br_vector3* pTransform, br_vector3* pFragment_owner, br_vector3* pPosition, tU32 pDistance, br_vector3* pArg14, tU32 pArg15) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f0380
void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38) {
    int i;
    tSmash_vertex* fragment;
    float delta_time;

    WorkerShrapnelShower(prev_0x4, prev_0x84, prev_0x44, prev_0x8, prev_0x20, prev_0x2c, prev_0x50, prev_0x48, prev_0x0, prev_0x54, prev_0x88, prev_0x14, prev_0xa0, prev_0x38, 0);
    delta_time = (float)(chunk_0x48 - prev_0x48);
    for (i = 0, fragment = gSmash_glass_fragments; i < 200; i++, fragment++) {
        if (fragment->end_time != 0 && fragment->field_0x18 == prev_0x0) {
            fragment->field_0x34 = 0;
            DoFragMovement(fragment, GetTotalTime(), delta_time);
            fragment->field_0x34 = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f5fa0
void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4) {
    br_model* pModel;
    br_material* pMaterial;
    const br_uint_16* source;
    br_face* face;
    int i;

    pModel = (br_model*)field_0x0;
    source = pField_0xc;
    pMaterial = (br_material*)field_0x4;
    face = pModel->faces + pModel->nfaces;
    for (i = 0; i < field_0x8; i++) {
        face->material = pMaterial;
        face->vertices[0] = source[0];
        face->vertices[1] = source[1];
        face->vertices[2] = source[2];
        face->smoothing = source[3];
        face->flags = 0;
        face++;
        source += 4;
    }
    pModel->nfaces += field_0x8;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f8630
void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10) {
    br_vector3* pPos;
    br_vector3* pLength;
    int i;
    br_actor* actor;
    br_vector3 delta;

    pPos = (br_vector3*)pField_0x4;
    pLength = (br_vector3*)pField_0x10;
    i = field_0x0 & 0xff;
    gSpark_flags |= 1 << field_0x0;
    if (gSparks[i].car != NULL) {
        actor = gSparks[i].car->car_master_actor;
        delta.v[0] = pPos->v[0] - actor->t.t.translate.t.v[0];
        delta.v[1] = pPos->v[1] - actor->t.t.translate.t.v[1];
        delta.v[2] = pPos->v[2] - actor->t.t.translate.t.v[2];
        BrMatrix34TApplyV(&gSparks[i].pos, &delta, &actor->t.t.mat);
    } else {
        gSparks[i].pos.v[0] = pPos->v[0];
        gSparks[i].pos.v[1] = pPos->v[1];
        gSparks[i].pos.v[2] = pPos->v[2];
    }
    gSparks[i].length.v[0] = pLength->v[0];
    gSparks[i].length.v[1] = pLength->v[1];
    gSparks[i].length.v[2] = pLength->v[2];
    gSparks[i].colour = field_0x0 >> 8;
}

// FUNCTION: CARMA2_HW 0x004f86f0
void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14) {
    int i;

    i = field_0x0 & 0x7fff;
    if ((gShrapnel_flags & (1 << i)) == 0) {
        BrActorAdd(gNon_track_actor, gShrapnel[i].actor);
    }
    gShrapnel_flags |= 1 << i;
    gShrapnel[i].actor->t.t.translate.t.v[0] = ((br_vector3*)pField_0x4)->v[0];
    gShrapnel[i].actor->t.t.translate.t.v[1] = ((br_vector3*)pField_0x4)->v[1];
    gShrapnel[i].actor->t.t.translate.t.v[2] = ((br_vector3*)pField_0x4)->v[2];
    if ((field_0x0 & 0x8000) != 0) {
        gShrapnel[i].age = field_0x10;
        gShrapnel[i].actor->material = (br_material*)field_0x14;
    }
}

// FUNCTION: CARMA2_HW 0x004f8b50
void C2_HOOK_FASTCALL FUN_004f8b50(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials) {
    extern void C2_HOOK_FASTCALL CreateShrapnelShower(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials);
    CreateShrapnelShower(pPosition, pVelocity, pDirection, pStrength, NULL, NULL, pMaterialCount, pMaterials);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

