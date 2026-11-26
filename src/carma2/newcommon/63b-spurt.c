#include "61-pedestrn.h"

#include "23-piping.h"
#include "41-utility.h"
#include "51-smash.h"
#include "rec2_types.h"

#include <stdlib.h>

/* gPed_spurt_id is the first int of the (still unnamed) pair gUNK_0065d7a0 owned by 61-pedestrn.c */
extern int gUNK_0065d7a0[2];
extern br_model* gPed_spurt_model;
extern int gPed_spurty_lumps_count;
extern int gPed_spurt_min_max_time[2];
extern tSmashable_initial_speed_spec gPed_spurt_initial_speed;
extern tSmashable_initial_position_spec gPed_spurt_initial_position;

// (stub only; body from REA 0x004d5820)
// FUNCTION: CARMA2_HW 0x004d5820
void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pTransform, br_vector3* pPosition, br_vector3* pDirection, br_vector3* pRandom_direction) {
    tU32 time;
    int seed;
    int saved_random_seed;
    int spurt_id;
    int i;
    tSmash_vertex* fragment;
    float scale;
    br_vector3 position;
    br_vector3 vec;
    br_vector3 direction;

    time = GetTotalTime();
    BrMatrix34ApplyP(&position, pPosition, pTransform);
    BrMatrix34ApplyV(&direction, pDirection, pTransform);
    vec.v[0] = pRandom_direction->v[0] * 6.9;
    vec.v[1] = pRandom_direction->v[1] * 6.9;
    vec.v[2] = pRandom_direction->v[2] * 6.9;
    seed = (int)position.v[0] ^ (int)position.v[1] ^ (int)position.v[2] ^ time;
    spurt_id = gUNK_0065d7a0[0];
    saved_random_seed = rand();
    srand(seed);
    for (i = 0; i < gPed_spurty_lumps_count; i++) {
        fragment = FindShrapnelFragment(gPed_spurt_model, spurt_id, 1);
        if (fragment == NULL) {
            break;
        }
        scale = SetInitPosition(fragment->actor, &gPed_spurt_initial_position, 0, (int)&position, (int)&position, 0, NULL, 0);
        SetShrapnelFragment(fragment, &gPed_spurt_initial_speed, &vec, scale, gPed_spurt_min_max_time, time, &direction, 0, 0);
    }
    srand(saved_random_seed);
    PipeSingleBloodSpurt(gUNK_0065d7a0[0], time, seed, &position, &vec, &direction);
    gUNK_0065d7a0[0]--;
}

/* ActiveHalted 0x4d1030 belongs to 61-pedestrn.c in the original; it is compiled here because any body of it in
   61-pedestrn.c shifts the register allocation of CBMovedByPhysics 0x4d2930 (100 -> 85.23). The declarations it
   needs are included here, after DoSpurt, so DoSpurt's codegen does not move. */
#include "00-car.h"
#include "15-displays.h"
#include "27-powerup.h"
#include "globvars.h"

#define RECACHE_FACE_BOUNDS(cache__) ((br_bounds3*)&(cache__)->field_0x18)
extern int gGoreLevel;
extern const tU8 gPed_anim_sequence_0065e4e0[4];
extern int gPed_points[3];
extern tSlick_spec gPed_slick;
extern int gPipe_halted_ped_status;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* The original repeats this block four times inline (0x4d12c9, 0x4d1505, 0x4d1669, 0x4d17ae):
   a halted ped that is still alive starts the get-up move 0x5d with the 0x65e4e0 anim sequence and
   resets the head bone model; a dead one gets move 0x5e. */
static __inline void C2_HOOK_FASTCALL ActiveHaltedResetMove(tPedestrian* pPed, tU32 pTime) {
    tPed_character_instance* character;
    int bone;
    int head;
    int model_set;

    if (pPed->hit_points > 0) {
        SetPedMove(pPed, 0x5d, -1, 0, 0, pTime, 5);
        if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
            pPed->field_0x0c->field_0x1c = (const tPed_anim_seq*)gPed_anim_sequence_0065e4e0;
            pPed->field_0x0c->field_0x78 = pTime;
            pPed->field_0x0c->field_0x1a = 0;
            bone = ((const tU8*)pPed->field_0x0c->field_0x1c)[1];
            character = pPed->character;
            head = character->personality->form->index_head_bone;
            if (character->field_0x4 >= 0) {
                if (head < 0) {
                    model_set = GetCharacterModelSet(character);
                    PipeSinglePedModelChange(character->ped, head, NULL, -1, model_set, 0);
                    SetCharacterAllBonesModel(character, bone, 0);
                } else {
                    model_set = GetCharacterModelSet(character);
                    PipeSinglePedModelChange(character->ped, head, GetCharacterBoneModel(character, head), bone, model_set, 0);
                    SetCharacterBoneModel(character, head, bone, 0);
                }
            }
        }
    } else {
        SetPedMove(pPed, 0x5e, -1, 0, 0, pTime, 5);
    }
}

static __inline void C2_HOOK_FASTCALL ActiveHaltedResetMoveIfDown(tPed_character_instance* pCharacter, tPedestrian* pPed, tU32 pTime) {
    EnableRetainRootMode();
    switch (pCharacter->personality->form->moves[pCharacter->field_0x7].id) {
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x61:
    case 0x71:
        ActiveHaltedResetMove(pPed, pTime);
        break;
    }
    DisableRetainRootMode();
}

static __inline tPhysics_object* C2_HOOK_FASTCALL ActiveHaltedRootObject(tPed_character_instance* pCharacter) {
    if (pCharacter->field_0x14 & 1) {
        return pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
    }
    return GetRootObject(pCharacter);
}

static __inline void C2_HOOK_FASTCALL ActiveHaltedSyncRoot(tPed_character_instance* pCharacter) {
    tPhysics_object* root;
    int r;
    int c;

    root = ActiveHaltedRootObject(pCharacter);
    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            root->transform_matrix.m[r][c] = root->actor->t.t.mat.m[r][c];
        }
    }
    root->flags |= 0x100;
}

static __inline void C2_HOOK_FASTCALL ActiveHaltedAward(tPedestrian* pPed) {
    tPed_cache_006944c0* cache;

    cache = pPed->field_0x0c;
    if (cache != NULL && cache->field_0xc0 == &gProgram_state.current_car && (cache->field_0x2e & 4) == 0) {
        cache->field_0x2e |= 4;
        EarnCredits(gPed_points[gProgram_state.skill_level]);
        DoFancyHeadup(4);
    }
}

/* A ped's ragdoll has come to rest (pObject is the halted physics object, pFace an optional contact
   face). Decides whether the ped tumbles on, dies, or gets up, awards the knock-down credits and
   spills blood. Translated from the original 0x4d1030..0x4d1c08 (the port had only the two early
   returns, so knocked-down peds never died, scored or got up). */
// FUNCTION: CARMA2_HW 0x004d1030
int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace) {
    tPedestrian* ped;
    tPhysics_object* root;
    tU32 time;
    br_vector3 up;
    br_vector3 axis;
    br_vector3 v;
    br_vector3 face_point;
    int recache_a[2];
    int recache_b;
    float best_y;
    float dot;
    int upright;
    int lying;
    int row;
    int i;
    int axis_index;

    if (gAction_replay_mode != 0) {
        return 1;
    }
    if (gPipe_halted_ped_status != 0) {
        PipeSinglePedStatus(pCharacter->ped, -1, -1, 5, 1, pCharacter->field_0x4, pCharacter->field_0x4, pCharacter->field_0x5, pCharacter->field_0x5, &pCharacter->field_0xc0, &pCharacter->field_0xcc, &pCharacter->field_0x8c);
        return 1;
    }
    ped = pCharacter->ped;
    ped->pos.v[0] = pObject->actor->t.t.mat.m[3][0];
    ped->pos.v[1] = pObject->actor->t.t.mat.m[3][1];
    ped->pos.v[2] = pObject->actor->t.t.mat.m[3][2];
    time = GetTotalTime();
    if ((pCharacter->field_0x14 & 1) == 0 && pFace == NULL) {
        goto done;
    }
    BrVector3Set(&up, 0.f, 0.f, 0.f);
    if (pFace != NULL) {
        up = pFace->normal;
        best_y = pFace->normal.v[1];
    } else {
        best_y = 0.f;
        for (i = pObject->box_face_start; i < pObject->box_face_end; i++) {
            if (best_y < gFace_list__car[i].normal.v[1]) {
                up = gFace_list__car[i].normal;
                best_y = gFace_list__car[i].normal.v[1];
            }
        }
    }
    dot = 0.f;
    if (pCharacter->field_0x8c.m[1][1] > 0.93 && ped->hit_points > 0) {
        upright = 1;
        lying = 0;
        BrVector3Set(&axis, 0.f, 0.f, 0.f);
    } else {
        upright = 0;
        if (pObject->I.v[0] > pObject->I.v[1] && pObject->I.v[0] > pObject->I.v[2]) {
            row = 0;
        } else if (pObject->I.v[1] > pObject->I.v[2] && pObject->I.v[1] > pObject->I.v[0]) {
            row = 1;
        } else {
            row = 2;
        }
        axis.v[0] = pObject->actor->t.t.mat.m[row][0] * -1.f;
        axis.v[1] = pObject->actor->t.t.mat.m[row][1] * -1.f;
        axis.v[2] = pObject->actor->t.t.mat.m[row][2] * -1.f;
        dot = up.v[0] * axis.v[0] + up.v[1] * axis.v[1] + up.v[2] * axis.v[2];
        lying = best_y != 0.f && fabs(dot) < 0.706;
    }

    /* orig 0x4d126b: lying on its side -> tumble it over (at most 2 or 3 times) */
    if (lying && ped->field_0x0c != NULL && ped->field_0x0c->field_0x19 < (ped->hit_points > 0) + 2) {
        ActiveHaltedResetMoveIfDown(ped->character, ped, time);
        if ((pCharacter->field_0x14 & 1) == 0) {
            SetCharacterPhysicsLevelAR(pCharacter, 5);
        }
        if (PercentageChance(50)) {
            gPed_tumble_factor = -gPed_tumble_factor;
        }
        MakePedNoise(ped, 3, 0, NULL);
        v.v[0] = axis.v[0] * gPed_tumble_factor;
        v.v[1] = axis.v[1] * gPed_tumble_factor;
        v.v[2] = axis.v[2] * gPed_tumble_factor;
        TumbleObjectWithV(pObject, &v, 1.39f, 0.f);
        if (ped->field_0x0c != NULL) {
            ped->field_0x0c->field_0x19++;
        }
        pObject->disable_move_rotate = 0;
        ActiveHaltedSyncRoot(pCharacter);
        return 0;
    }

    /* orig 0x4d1497: it stays down */
    PipeSinglePedStatus(pCharacter->ped, -1, -1, 5, 1, pCharacter->field_0x4, pCharacter->field_0x4, pCharacter->field_0x5, pCharacter->field_0x5, &pCharacter->field_0xc0, &pCharacter->field_0xcc, &pCharacter->field_0x8c);
    pCharacter->ped->field_0x06++;
    ActiveHaltedResetMoveIfDown(ped->character, ped, time);
    if (ped->field_0x0c != NULL) {
        ped->field_0x0c->field_0x19 = 0;
    }
    if (ped->hit_points > 0) {
        RecacheAndSetFace(ped, recache_a, &recache_b, &face_point, 1, time);
        if ((lying || (upright && gPeds_suicidal)) && !gImmortal_peds) {
            /* orig 0x4d164d: landed badly -> dies */
            MakePedNoise(ped, 5, 0, NULL);
            KillPedestrian(ped, NULL);
            ActiveHaltedResetMove(ped, time);
            SetCharacterMoveAR(ped->character, -1, gMutant_speed != 0.f ? -gMutant_speed : 0.f, 0, 0, time);
        } else if ((pCharacter->field_0xc & gPow2_array[3]) != 0 && (gPow2_array[5] & pCharacter->field_0xc) != 0) {
            ActiveHaltedResetMove(ped, time);
            ActiveHaltedAward(ped);
        } else {
            if (ped->character->field_0xc != 0) {
                ActiveHaltedAward(ped);
            }
            ped->flags &= ~0x8;
            if (upright) {
                /* orig 0x4d192a: back on its feet */
                SetPedXZDirection(ped, (br_vector3*)pCharacter->field_0x8c.m[2], 0.f, time);
                ped->action = 0;
                if ((ped->character->field_0xc & gPow2_array[3]) == 0 && (gPow2_array[5] & ped->character->field_0xc) == 0 && ped->field_0x0c != NULL) {
                    ped->field_0x0c->field_0x80 = 100;
                    ped->field_0x0c->field_0xb8 = 0.09f;
                    SetPedMove(ped, 0x29, ped->field_0x0c->field_0x80, 0, 0, time, 1);
                } else {
                    SetPedMove(ped, 0x29, -1, 0, 0, time, 1);
                }
                ped->field_0x28 = time;
            } else {
                /* orig 0x4d19b8: lying down, face along the body */
                if (dot > 0.f) {
                    axis_index = pCharacter->personality->form->axis_when_lying_down_A;
                } else {
                    axis_index = pCharacter->personality->form->axis_when_lying_down_B;
                }
                if (axis_index < 0) {
                    v.v[0] = pCharacter->field_0x8c.m[-axis_index][0] * -1.f;
                    v.v[1] = pCharacter->field_0x8c.m[-axis_index][1] * -1.f;
                    v.v[2] = pCharacter->field_0x8c.m[-axis_index][2] * -1.f;
                } else {
                    v.v[0] = pCharacter->field_0x8c.m[axis_index][0];
                    v.v[1] = pCharacter->field_0x8c.m[axis_index][1];
                    v.v[2] = pCharacter->field_0x8c.m[axis_index][2];
                }
                SetPedXZDirection(ped, &v, 0.f, time);
                SetPedMove(ped, dot > 0.f ? 0x50 : 0x51, -1, 0, 1, time, 4);
            }
            RescanPedProximity(ped, NULL);
            PedScanForObjects(ped, time);
        }
    } else {
        /* orig 0x4d1a9e: dead -> lie still, maybe a blood pool */
        SetCharacterMoveAR(ped->character, -1, gMutant_speed != 0.f ? -gMutant_speed : 0.f, 0, 0, time);
        root = ActiveHaltedRootObject(ped->character);
        if (root->last_special_volume == NULL || root->last_special_volume->gravity_multiplier >= 1.f) {
            if (!gAction_replay_mode && PercentageChance(75) && (ped->flags & 0x10) == 0 && gGoreLevel > 0) {
                ped->flags |= 0x10;
                DoSpillage(&gPed_slick, GetCharacterActorPtr(ped->character));
            }
        }
    }
    ActiveHaltedSyncRoot(pCharacter);
done:
    if (pObject->flags_0x238 == 5) {
        pObject->field_0x1a0 = 0x40000;
    } else {
        pObject->field_0x1a0 = 0x10000;
    }
    pObject->field_0x1a4 = 0x50000;
    ClearSplashes(pObject);
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
#undef RECACHE_FACE_BOUNDS
