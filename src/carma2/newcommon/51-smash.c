#define REC2_EXPLOSION_SIGNED_FRAMES

#include "51-smash.h"

#include <math.h>

#include "08-loading1.h"
#include "41-utility.h"
#include "globvars.h"
#include "rec2_types.h"
#include "rec2_macros.h"
#include "52-errors.h"
#include "08-loading1.h"
#include "c2_string.h"
#include "28-world3.h"
#include "globvrpb.h"

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
void C2_HOOK_FASTCALL DoDelayedNonCar(tU32 pTime, tDelayed_non_car* pNon_car);
void C2_HOOK_FASTCALL DoDelayedSmash(tDelayed_smash* pSmash);
extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);
void C2_HOOK_FASTCALL FUN_004d35e0(tPedestrian* pPed);
int C2_HOOK_FASTCALL FUN_004cd640(tPedestrian* pPed, int pSpeed, int pMove1, int pMove2, tPhysics_object* pCollision_info);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int gCrush_deferred;
extern int gPed_dismemberfest;
extern const double gConst_world_scale;
extern const double gConst_2pi;

// GLOBAL: CARMA2_HW 0x0065feb0
const char* gRepeatability_names[4] = {
    "none",
    "singleshot",
    "repeated",
    "doitregardless",
};

// GLOBAL: CARMA2_HW 0x006a3660
tSmashable_item_spec gUnknown_smashable_006a3660 = { 0 };

// GLOBAL: CARMA2_HW 0x006a52d0
tSmashable_item_spec gPowerup_pickup_smashable = { 0 };

/* explicit initializers keep gExplosions, gSmash_explosions and gPowerup_respawn_smashable adjacent as in the original (loop-end compares) */
// GLOBAL: CARMA2_HW 0x006a55c8
tExplosion gExplosions[50] = { 0 };

// GLOBAL: CARMA2_HW 0x006a6d38
int gMaterials_to_adapt_count = 0;

// GLOBAL: CARMA2_HW 0x006a6d3c
int gCount_host_smashes = 0;

// GLOBAL: CARMA2_HW 0x006a6d40
tSmash_explosion gSmash_explosions[20] = { 0 };

// GLOBAL: CARMA2_HW 0x006a7ce0
tSmashable_item_spec gPowerup_respawn_smashable = { 0 };

// GLOBAL: CARMA2_HW 0x006a7fc0
int gPrev_glass_munge = 0;

/* explicit initializers keep gRepair_animations, the gap dword and gDecals adjacent (loop-end compares) */
// GLOBAL: CARMA2_HW 0x006a7fc8
tRepair_animation gRepair_animations[25] = { 0 };

/* UNCERTAIN: unknown dword after gRepair_animations, only seen as its loop end */
// GLOBAL: CARMA2_HW 0x006a80f4
int gUNK_006a80f4 = 0;

// GLOBAL: CARMA2_HW 0x006a80f8
tDecal gDecals[50] = { 0 };

// GLOBAL: CARMA2_HW 0x006a8288
int gCount_track_smashable_environment_specs_2 = 0;

// GLOBAL: CARMA2_HW 0x006a828c
int gCount_queued_smashes = 0;

// GLOBAL: CARMA2_HW 0x006a8294
int gINT_006a8294 = 0;


// GLOBAL: CARMA2_HW 0x006abee8
br_vector3 gZero_vector__smash;

// GLOBAL: CARMA2_HW 0x006b7888
tSmashable_environment_name* gSmashable_noncars;

// GLOBAL: CARMA2_HW 0x0065fee8
const char* gExplosion_rotate_names[2] = {
    "norotate",
    "randomrotate",
};

// GLOBAL: CARMA2_HW 0x006a3334
int gINT_006a3334 = 0;

// GLOBAL: CARMA2_HW 0x006a8290
int gINT_006a8290 = 0;

// GLOBAL: CARMA2_HW 0x006a3338
int gShrapnel_ghost_actor_count;

// GLOBAL: CARMA2_HW 0x0065fec0
const char* gShrapnel_type_names[4] = {
    "abstract",
    "shards",
    "ghostparts",
    "noncars",
};

// GLOBAL: CARMA2_HW 0x006a5140
br_actor* gSmashable_noncar_shrapnel_actors[100];

// GLOBAL: CARMA2_HW 0x006a55b0
int gCount_smashable_noncar_shrapnel_actors;

// GLOBAL: CARMA2_HW 0x006a55b8
const char* gSmashable_track_environment_path;

// GLOBAL: CARMA2_HW 0x006b788c
int gCount_smashable_noncars;

extern intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, void* pContext);
extern void C2_HOOK_FASTCALL AddFogificateMaterial(br_material* pMaterial);
extern br_material* gMaterials_to_adapt[200];

// GLOBAL: CARMA2_HW 0x0065fed0
const char* gInitial_smashable_position_type_names[3] = {
    "sphereclumped",
    "boxclumped",
    "actorbased",
};

// GLOBAL: CARMA2_HW 0x0065fee0
const char* gInitial_position_sphere_where[2] = {
    "impact",
    "model",
};

/* explicit initializers keep gSmash_glass_fragments and gSmashable_track_indexable_triggers adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x006b78e0
tSmash_vertex gSmash_glass_fragments[200] = { 0 };

// GLOBAL: CARMA2_HW 0x006ba4a0
tSmashable_item_spec* gSmashable_track_indexable_triggers[100] = { 0 };


// GLOBAL: CARMA2_HW 0x006a55c0
tNet_stored_smash* gNet_host_smashes;

// GLOBAL: CARMA2_HW 0x006a5138
tSmashable_item_spec* gTrack_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006a55b4
int gCount_track_smashable_environment_specs;

/* explicit initializers keep gQueued_net_smashes and gPowerup_queue adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x006a3940
tQueued_net_smash gQueued_net_smashes[50] = { 0 };

// GLOBAL: CARMA2_HW 0x006a4430
tPowerup_queue_item gPowerup_queue[50] = { 0 };

// GLOBAL: CARMA2_HW 0x006a55bc
int gSize_powerup_queue;


// FUNCTION: CARMA2_HW 0x004ea880
void C2_HOOK_FASTCALL InitExplosions(void) {
    int i;
    int capacity;
    tExplosion* explosion;

    capacity = (int)REC2_ASIZE(gExplosions);
    for (i = 0, explosion = gExplosions; i < capacity; explosion++, i++) {

        explosion->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

        explosion->actor->model = BrModelAllocate(NULL, 4, 2);
        explosion->actor->model->faces[0].vertices[0] = 0;
        explosion->actor->model->faces[0].vertices[1] = 2;
        explosion->actor->model->faces[0].vertices[2] = 1;
        explosion->actor->model->faces[0].material = NULL;
        explosion->actor->model->faces[0].smoothing = 1;
        explosion->actor->model->faces[1].vertices[0] = 0;
        explosion->actor->model->faces[1].vertices[1] = 3;
        explosion->actor->model->faces[1].vertices[2] = 2;
        explosion->actor->model->faces[1].material = NULL;
        explosion->actor->model->faces[1].smoothing = 1;
        BrVector3Set(&explosion->actor->model->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&explosion->actor->model->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[3].map, 1.f, 1.f);
        BrModelAdd(explosion->actor->model);

        explosion->actor->material = BrMaterialAllocate("BANG!");
        explosion->actor->material->flags &= ~BR_MATF_LIGHT;
        explosion->actor->material->flags |= BR_MATF_ALWAYS_VISIBLE;
        explosion->actor->material->colour_map = gBack_screen;
        GlorifyMaterial(&explosion->actor->material, 1, kRendererShadingType_AmbientOnly);
        BrMaterialAdd(explosion->actor->material);
        explosion->actor->render_style = BR_RSTYLE_FACES;

        explosion->start = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004eaac0
void C2_HOOK_FASTCALL ResetExplosions(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gExplosions) != 50);
    C2_HOOK_BUG_ON(sizeof(gExplosions[0]) != 0x78);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_explosions) != 20);
    C2_HOOK_BUG_ON(sizeof(gSmash_explosions[0]) != 0xc8);

    for (i = 0; i < (int)REC2_ASIZE(gExplosions); i++) {
        gExplosions[i].start = 0;
        gExplosions[i].finished = 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmash_explosions); i++) {
        gSmash_explosions[i].active = 0;
    }
}

// RemoveAllExplosionettes

extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, br_scalar pMax_distance);

// (reference only)
// FUNCTION: CARMA2_HW 0x004eaaf0
void C2_HOOK_FASTCALL MungeExplosions(void) {
    int i;
    int frame;
    int now;
    br_vector3 tv;
    tExplosion* explosion;
    int prev_frame;

    now = GetTotalTime();
    for (i = 0, explosion = gExplosions; i < (int)REC2_ASIZE(gExplosions); i++, explosion++) {
        if (explosion->start == 0) {
            continue;
        }
        frame = (now - (int)explosion->start) / (int)explosion->period;
        prev_frame = explosion->field_0x8;
        if (now < (int)explosion->start) {
            goto remove;
        }
        if (frame >= explosion->count_frames) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
            continue;
        }
        if (!gAction_replay_mode && PointOutOfSight(&explosion->collision_actor->t.t.translate.t, 0.f)) {
            goto remove;
        }
        explosion->field_0x8 = frame;
        if (explosion->actor->parent == NULL) {
            BrActorAdd(gNon_track_actor, explosion->actor);
        }
        if (frame != prev_frame) {
            explosion->actor->material->colour_map = explosion->frames[frame].map;
            BrMaterialUpdate(explosion->actor->material, BR_MATU_ALL);
        }
        if (explosion->actor->material->colour_map == NULL) {
            goto remove;
        }
        explosion->actor->t.t.mat = gCamera->t.t.mat;
        BrMatrix34ApplyV(&tv, &explosion->field_0x1c, &explosion->collision_actor->t.t.mat);
        explosion->actor->t.t.translate.t.v[0] = explosion->collision_actor->t.t.translate.t.v[0] + tv.v[0];
        explosion->actor->t.t.translate.t.v[1] = explosion->collision_actor->t.t.translate.t.v[1] + tv.v[1];
        explosion->actor->t.t.translate.t.v[2] = explosion->collision_actor->t.t.translate.t.v[2] + tv.v[2];
        BrMatrix34PreScale(&explosion->actor->t.t.mat,
            (float)explosion->actor->material->colour_map->width * explosion->field_0x14 / 128.f,
            (float)explosion->actor->material->colour_map->height * explosion->field_0x14 / 128.f,
            1.f);
        BrMatrix34PreRotateZ(&explosion->actor->t.t.mat, explosion->angle);
        continue;
    remove:
        if (explosion->actor->parent != NULL) {
            BrActorRemove(explosion->actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004eace0
void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion) {
    C2_HOOK_BUG_ON(sizeof(tExplosion) != 0x78);

    gExplosions[pIndex] = *pExplosion;
}

// DoExplosionette

// Explode

// FUNCTION: CARMA2_HW 0x004eb020
// FUNCTION: CARMA2_SW 0x004e6d00
void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec) {
    int i;
    tExplosion* explosion;

    for (i = REC2_ASIZE(gExplosions), explosion = gExplosions; i > 0; i--, explosion++) {
        if (explosion->start != 0 && explosion->collision_actor == pCar_spec->collision_info->actor) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
        }
    }
}

// CalcRandomPointBetween

// FUNCTION: CARMA2_HW 0x004eb120
void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9) {

    ApplyInitialMovement((undefined4*)pSpeed, pArg3, &pFragment->v, &pFragment->omega, pArg4, pArg7, (br_vector3*)(uintptr_t)pArg8, (br_vector3*)(uintptr_t)pArg9);
    pFragment->field_0xc = pArg6;
    pFragment->time_last_move = pArg6;
    pFragment->end_time = pArg6 + pArg5[1];
    pFragment->field_0x10 = IRandomBetween(pArg5[0], pArg5[1]) + pArg6;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// (reference only)
// FUNCTION: CARMA2_HW 0x004eb180
void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    float* f = (float*)pArg1;
    br_vector3 unit;
    float len;
    int i;

    pArg3->v[0] = f[2] * pArg2->v[0];
    pArg3->v[1] = pArg2->v[1] * f[2];
    pArg3->v[2] = pArg2->v[2] * f[2];

    if (pArg7 != NULL && pArg8 != NULL && f[0] != 0.f && f[1] != 0.f) {
        BrVector3Sub(&unit, pArg7, pArg8);
        len = BrVector3Length(&unit);
        unit.v[0] = FRandomBetween(f[0], f[1]) / len * unit.v[0];
        unit.v[1] = FRandomBetween(f[0], f[1]) / len * unit.v[1];
        unit.v[2] = FRandomBetween(f[0], f[1]) / len * unit.v[2];
        pArg3->v[0] += unit.v[0];
        pArg3->v[1] += unit.v[1];
        pArg3->v[2] += unit.v[2];
    }

    for (i = 0; i < 3; i++) {
        pArg3->v[i] = (FRandomPosNeg(f[3]) + pArg3->v[i]) * pArg5;
        pArg4->v[i] = FRandomBetween(0.f, 1.f);
    }

    pArg3->v[1] += FRandomBetween(0.f, f[4]);

    BrVector3Normalise(&unit, pArg6);
    unit.v[0] = FRandomBetween(0.f, f[5]) * unit.v[0];
    unit.v[1] = FRandomBetween(0.f, f[5]) * unit.v[1];
    unit.v[2] = FRandomBetween(0.f, f[5]) * unit.v[2];
    pArg3->v[0] += unit.v[0];
    pArg3->v[1] += unit.v[1];
    pArg3->v[2] += unit.v[2];

    BrVector3Normalise(pArg4, pArg4);

    pArg4->v[0] *= FRandomPosNeg(pArg5 * f[6]);
    pArg4->v[1] *= FRandomPosNeg(pArg5 * f[6]);
    pArg4->v[2] *= FRandomPosNeg(pArg5 * f[6]);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ApplyTimeSpec

extern br_actor* gNon_track_actor;

// FUNCTION: CARMA2_HW 0x004eb070
tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority) {
    int i;
    int lowest_priority;
    tSmash_vertex* fragment;
    tSmash_vertex* lowest_priority_fragment;

    lowest_priority = 10000;
    for (i = 0, fragment = gSmash_glass_fragments; i < (int)REC2_ASIZE(gSmash_glass_fragments); i++, fragment++) {
        if (fragment->end_time == 0) {
            i = -1;
            break;
        }
        if (fragment->field_0x35[0] < lowest_priority) {
            lowest_priority = fragment->field_0x35[0];
            lowest_priority_fragment = fragment;
        }
    }
    if (i >= 0) {
        if (lowest_priority < pPriority) {
            fragment = lowest_priority_fragment;
        } else {
            return NULL;
        }
    }
    if (pModel != NULL) {
        fragment->actor->model = pModel;
    } else {
        fragment->actor->model = fragment->model;
    }
    if (fragment->actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, fragment->actor);
    }
    fragment->field_0x35[0] = (tU8)pPriority;
    fragment->field_0x18 = pId;
    fragment->field_0xc = GetTotalTime();
    fragment->field_0x34 = 1;
    return fragment;
}

// SetShrapnelFragment

// ShatterObject

// MungeInternalCarGlass

// ActuallySmashPart

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// SetInitPosition

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

extern void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50);

// FUNCTION: CARMA2_HW 0x004ecc40
void C2_HOOK_FASTCALL DoSpillage(tSlick_spec* pSlick, br_actor* pActor) {
    QueueOilSpill2(NULL, pActor, pActor->model, pSlick->field_0x0,
        pSlick->field_0x4, pSlick->field_0x8, pSlick->field_0xc, pSlick->field_0x10,
        pSlick->field_0x14, pSlick->field_0x20, pSlick->field_0x24, pSlick->field_0x18,
        pSlick->field_0x1c, pSlick->material, pSlick->material2);
}

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

// (reference)
// FUNCTION: CARMA2_HW 0x004ecc80
void C2_HOOK_FASTCALL MungeDelayedSideEffects(void) {
    int i;
    tU32 the_time;

    the_time = PDGetTotalTime();
    for (i = 0; i < REC2_ASIZE(gSmash_explosions); i++) {
        tSmash_explosion* delayed_effect = &gSmash_explosions[i];

        if (delayed_effect->active) {
            switch (delayed_effect->type) {
            case 0:
                DoDelayedNonCar(the_time, &delayed_effect->what.non_car);
                if (delayed_effect->what.non_car.count_actions == 0) {
                    delayed_effect->active = 0;
                }
                break;
            case 1:
                if (the_time >= delayed_effect->what.smash.field_0x18) {
                    DoDelayedSmash(&delayed_effect->what.smash);
                    delayed_effect->active = 0;
                }
                break;
            }
        }

    }
}


// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern tS3_outlet* gCar_outlet;
extern br_vector3 gZero_v__car;
extern float gYon_squared;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// and the original disassembly
// FUNCTION: CARMA2_HW 0x004ecea0
void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor* pActor) {
    br_bounds3* pBounds;
    float yon_squared;

    if (gPowerup_respawn_smashable.mode_data.shrapnel.connotations.count_sounds != 0) {
        DRS3StartSound3D(gCar_outlet, gPowerup_respawn_smashable.mode_data.shrapnel.connotations.sounds[IRandomBetween(0, gPowerup_respawn_smashable.mode_data.shrapnel.connotations.count_sounds - 1)], &pActor->t.t.translate.t, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    pBounds = &pActor->model->bounds;
    yon_squared = gYon_squared;
    if (gAction_replay_mode != 0 || PointOutOfSight(&pActor->t.t.translate.t, yon_squared) == 0) {
        if (gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.explosion_animation.count_groups != 0) {
            Explode(&gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.explosion_animation, pActor, pBounds, NULL);
        }
    }
    if (gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.material != NULL) {
        QueueOilSpill2(NULL, pActor, pActor->model,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x0,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x4,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x8,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0xc,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x10,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x14,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x20,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x24,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x18,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.field_0x1c,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.material,
            gPowerup_respawn_smashable.mode_data.shrapnel.connotations.special_effects.slick.material2);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DoConnotationsMultiLevel

extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);
extern void C2_HOOK_FASTCALL EarnCredits100(int pAmount);
extern void C2_HOOK_FASTCALL AwardTime(tU32 pTime);
extern void C2_HOOK_FASTCALL AwardTime5(tU32 pTime);
extern void C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);
extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pDelta);
extern int C2_HOOK_FASTCALL ToggleRuntimeVariable(int pAddress);
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern void C2_HOOK_FASTCALL QueueNonCarCuboidActivation(br_actor* pActor, tNon_car_cuboid_activations* pActivations, void* pContext);
extern void C2_HOOK_FASTCALL FUN_004edbe0(tSmashable_item_spec* pSpec, void* pObject, tShrapnel_side_effects* pSide_effects, undefined4 pArg16, float pDistance);
extern void C2_HOOK_FASTCALL FUN_004f1030(tPhysics_object* pObject, int pMode, br_actor* pActor);
extern void C2_HOOK_FASTCALL WorkerShrapnelShower(int pArg3, tShrapnel_spec* pShrapnel, tU32 pSeed, undefined4 pArg6, const br_vector3* pPoint, const br_vector3* pSound_position, br_actor* pActor, tU32 pTime, undefined4 pArg11, br_matrix34* pTransform, br_bounds3* pBounds, br_vector3* pPosition, float pDistance, const br_vector3* pOffset, int pFlag);
extern tS3_outlet* gCar_outlet;
extern br_vector3 gZero_v__car;
extern float gYon_squared;

// (reference only)
// FUNCTION: CARMA2_HW 0x004ed490
void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tPhysics_object* pObject, tSmashable_item_spec* pSpec, undefined4 pArg3, tConnotations* pConnotations, tU32 pSeed, undefined4 pArg6, const br_vector3* pPoint, const br_vector3* pSound_position, br_actor* pActor, tU32 pTime, undefined4 pArg11, br_matrix34* pTransform, br_bounds3* pBounds, br_vector3* pPosition, float pDistance, undefined4 pArg16, int pFlag) {
    int i;
    tCar_spec* owner;
    tSide_effects* side_effects;
    tSpecial_effects_spec* special_effects;
    tSmash_run_time_variable_change* variable_change;
    int value;
    extern float gFloat_0058aec0;

    if (pConnotations->count_sounds != 0) {
        DRS3StartSound3D(gCar_outlet, pConnotations->sounds[IRandomBetween(0, pConnotations->count_sounds - 1)], pSound_position, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    if (pFlag == 0) {
        side_effects = &pConnotations->side_effects;
        if (side_effects->activations.count_activations != 0) {
            QueueNonCarCuboidActivation(pActor, &side_effects->activations, pObject);
        }
        if (side_effects->side_effects.count_side_effects != 0) {
            FUN_004edbe0(pSpec, pObject, &side_effects->side_effects, pArg16, pDistance);
        }
        if (side_effects->room_turn_on_code != 0
                && pObject != NULL
                && pObject->owner != NULL
                && pObject->flags_0x238 == 1
                && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human) {
            if (BR_SQR3(pObject->actor->t.t.translate.t.v[0] - pPoint->v[0], pObject->actor->t.t.translate.t.v[1] - pPoint->v[1], pObject->actor->t.t.translate.t.v[2] - pPoint->v[2]) < 2.0) {
                FUN_004f1030(pObject, side_effects->room_turn_on_code, NULL);
            }
        }
    }
    for (i = 0; i < pConnotations->count_shrapnel; i++) {
        WorkerShrapnelShower(pArg3, &pConnotations->shrapnel[i], pSeed, pArg6, pPoint, pSound_position, pActor, pTime, pArg11, pTransform, pBounds, pPosition, pDistance, (br_vector3*)&pObject->transform_matrix.m[3], pFlag);
    }
    if (pFlag == 0) {
        owner = (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) ? (tCar_spec*)pObject->owner : NULL;
        if ((owner != NULL
                    && (pConnotations->award.frequency == kRepeatability_Repeated
                        || (pConnotations->award.frequency == kRepeatability_SingleShot && *(int*)pConnotations->field_0x28c == 0))
                    && owner->driver == eDriver_local_human)
                || pConnotations->award.frequency == kRepeatability_DoItRegardless) {
            *(int*)pConnotations->field_0x28c = 1;
            if (pConnotations->award.field_0x4 > gFloat_0058aec0) {
                EarnCredits((int)pConnotations->award.field_0x4);
            } else {
                float credits;
                credits = -(pDistance * pConnotations->award.field_0x4);
                if (credits >= 20000.0f) {
                    credits = 20000.0f;
                }
                EarnCredits100((int)credits);
            }
            if (pConnotations->award.field_0x8 > gFloat_0058aec0) {
                AwardTime((int)pConnotations->award.field_0x8);
            } else {
                float time;
                time = -(pDistance * pConnotations->award.field_0x8);
                if (time >= 60.0f) {
                    time = 60.0f;
                }
                AwardTime5((int)time);
            }
            if (pConnotations->award.field_0xc != 0) {
                NewTextHeadupSlot2(4, 0, 3000, -4, GetMiscString(pConnotations->award.field_0xc), 0);
            }
            if (pConnotations->award.field_0x10 != 0) {
                DoFancyHeadup(pConnotations->award.field_0x10);
            }
        }
    }
    special_effects = &pConnotations->special_effects;
    pDistance = gYon_squared;
    if (gAction_replay_mode != 0 || PointOutOfSight(&pActor->t.t.translate.t, pDistance) == 0) {
        if (special_effects->explosion_animation.count_groups != 0) {
            Explode(&special_effects->explosion_animation, pActor, pBounds, NULL);
        }
    }
    if (special_effects->slick.material != NULL) {
        DoSpillage(&special_effects->slick, pActor);
    }
    variable_change = pConnotations->variable_changes.runtime_changes;
    for (i = 0; i < pConnotations->variable_changes.count; i++, variable_change++) {
        value = variable_change->field_0x0;
        if (value != 9999) {
            AccumulateRuntimeVariable((tU16)variable_change->field_0x2, value);
        } else {
            ToggleRuntimeVariable((tU16)variable_change->field_0x2);
        }
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash
            && GetRuntimeVariable(gCurrent_race.race_spec->options.smash.var_smash_number) >= gCurrent_race.race_spec->options.smash.var_smash_target) {
        RaceCompleted(eRace_over_3);
    }
}

// FUNCTION: CARMA2_HW 0x004ecfa0
void C2_HOOK_FASTCALL InitSmashQueue(void) {
    gCount_queued_smashes = 0;
}

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// ReadShrapnelMovementSpec

// ReadShrapnelInitialPosition

// ReadShrapnelTimeInfo

// NoteChildren

// ReadSmokeFireChance

// LoadActorFromRace

// ReadShrapnelSpec

// FUNCTION: CARMA2_HW 0x004ee5a0
// FUNCTION: CARMA2_SW 0x004ea270
void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, tMin_max_time* pTimes) {
    float min;
    float max;

    GetPairOfFloats(pFile, &min, &max);
    pTimes->min_time = (int)(min * 1000.0);
    pTimes->max_time = (int)(max * 1000.0);
}

// FUNCTION: CARMA2_HW 0x004ee500
void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_initial_speed_spec) != 28);

    /* Min,max speed towards car */
    GetPairOfFloats(pFile, &pInitial_speed->min_speed, &pInitial_speed->max_speed);
    /* Impactee velocity factor */
    pInitial_speed->impactee_velocity_factor = GetAScalar(pFile);
    /* Random velocity (max) */
    pInitial_speed->random_velocity_max = GetAScalar(pFile);
    /* Random up velocity (max) */
    pInitial_speed->random_up_velocity_max = GetAScalar(pFile);
    /* Random normal velocity (max) */
    pInitial_speed->random_normal_velocity_max = GetAScalar(pFile);
    /* Random spin rate (max) */
    pInitial_speed->random_spin_rate_max = GetAScalar(pFile) * 0.1f;
}

// FUNCTION: CARMA2_HW 0x004ee550
void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos) {

    /* Initial position type */
    pInitial_pos->type = GetALineAndInterpretCommand(pFile, gInitial_smashable_position_type_names, REC2_ASIZE(gInitial_smashable_position_type_names));

    switch (pInitial_pos->type) {
        case kInitialSmashablePosition_SphereClumped:
            /* Sphere radius */
            pInitial_pos->position.sphere.radius = GetAScalar(pFile);
            /* Sphere centre */
            pInitial_pos->position.sphere.where = GetALineAndInterpretCommand(pFile, gInitial_position_sphere_where, REC2_ASIZE(gInitial_position_sphere_where));
            break;
        case kInitialSmashablePosition_BoxClumped:
            GetThreeFloats(pFile, &pInitial_pos->position.box.v[0], &pInitial_pos->position.box.v[1], &pInitial_pos->position.box.v[2]);
            break;
        default:
            break;
    }
}

// FUNCTION: CARMA2_HW 0x004ee780
void C2_HOOK_FASTCALL ReadExplosion(FILE *pF, tExplosion_animation* pAnimation) {
    int i;
    int j;
    char s[256];
    tExplosion_animation_group* group;
    float f1, f2;
    int d1, d2;

    C2_HOOK_BUG_ON(sizeof(tExplosion_animation) != 8);
    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_group) != 68);
    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_frame) != 8);

    /* Number of explosion groups */
    pAnimation->count_groups = GetAnInt(pF);
    if (pAnimation->count_groups == 0) {
        return;
    }
    pAnimation->groups = BrMemAllocate(pAnimation->count_groups * sizeof(tExplosion_animation_group), kMem_explosion_data);
    group = pAnimation->groups;
    for (i = 0; i < pAnimation->count_groups; i++, group++) {

        /* min count, max count */
        GetPairOfInts(pF, &d1, &d2);
        group->min_count = d1;
        group->max_count = d2;
        /* min start delay, max start delay */
        GetPairOfFloats(pF, &f1, &f2);
        group->min_start_delay_ms = (tU16)(1000 * f1);
        group->max_start_delay_ms = (tU16)(1000 * f2);
        /* offset */
        GetThreeFloats(pF, &group->offset.v[0], &group->offset.v[1], &group->offset.v[2]);
        /* min x factor, max x factor */
        GetPairOfFloats(pF, &group->min_xyz_factor.v[0], &group->max_xyz_factor.v[0]);
        /* min y factor, max y factor */
        GetPairOfFloats(pF, &group->min_xyz_factor.v[1], &group->max_xyz_factor.v[1]);
        /* min z factor, max z factor */
        GetPairOfFloats(pF, &group->min_xyz_factor.v[2], &group->max_xyz_factor.v[2]);
        /*  min frame rate, max frame rate */
        GetPairOfFloats(pF, &f1, &f2);
        group->min_frametime_ms = (tU16)(1000.f / f1);
        group->max_frametime_ms = (tU16)(1000.f / f2);
        /* min scaling factor, max scaling factor */
        GetPairOfFloats(pF, &group->min_scaling_factor, &group->max_scaling_factor);
        /* rotate mode */
        group->rotate_modus = GetALineAndInterpretCommand(pF, gExplosion_rotate_names, REC2_ASIZE(gExplosion_rotate_names));
        /* number of frames */
        group->count_frames = GetAnInt(pF);
        group->frames = BrMemAllocate(group->count_frames * sizeof(tExplosion_animation_frame), kMem_explosion_data);
        for (j = 0; j < group->count_frames; j++) {
            /* opacity */
            group->frames[j].opacity = GetAScalar(pF);
            group->frames[j].opacity = 100.f;

            /* frame pix name */
            GetAString(pF, s);
            group->frames[j].map = BrMapFind(s);
            if (group->frames[j].map == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}
// (reference only)
// FUNCTION: CARMA2_HW 0x004ee950
void C2_HOOK_FASTCALL FreeExplosionAnimation(tExplosion_animation* pAnimation) {
    int i;
    tExplosion_animation_group* group;

    if (pAnimation->count_groups != 0) {
        i = 0;
        group = pAnimation->groups;
        if (pAnimation->count_groups > 0) {
            do {
                BrMemFree(group->frames);
                i++;
                group++;
            } while (i < pAnimation->count_groups);
        }
        BrMemFree(pAnimation->groups);
    }
}

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

// ReadSmashLevels

// ReadSmashableData

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004eea20
void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pLevels, int* pCount, int pIs_texture_change, tBrender_storage* pStorage) {
    extern const char* gSmashable_collision_type_names[3];
    extern const char* gPosition_type_names[2];
    extern void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count);
    extern void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick);
    extern void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
    char s[256];
    tSmashable_level* level;
    int j;
    int i1;
    int v1;
    int i2;
    int v2;
    int i;

    GetAString(pF, s);
    *pCount = GetAnInt(pF) + 1;
    *pLevels = BrMemAllocate(*pCount * sizeof(tSmashable_level), kMem_smash_levels);
    (*pLevels)[0].pixelmaps[0] = BrMapFind(s);
    if ((*pLevels)[0].pixelmaps[0] == NULL) {
        FatalError(kFatalError_CantLoadSmashPix_S, s);
    }
    (*pLevels)[0].count_shrapnels = 0;
    (*pLevels)[0].trigger_threshold = 0;
    level = *pLevels + 1;
    for (i = 0; i < *pCount - 1; i++, level++) {
        level->trigger_threshold = GetAScalar(pF);
        level->flags = GetAnInt(pF);
        if (pIs_texture_change) {
            level->collision_type = GetALineAndInterpretCommand(pF, gSmashable_collision_type_names, REC2_ASIZE(gSmashable_collision_type_names));
        }
        level->count_sounds = GetAnInt(pF);
        for (j = 0; j < level->count_sounds; j++) {
            level->sounds[j] = LoadSingleSound(pStorage, GetAnInt(pF));
        }
        ReadShrapnelSpec(pF, level->shrapnels, &level->count_shrapnels);
        ReadExplosion(pF, &level->animation);
        ReadSpillData(pF, &level->slick);
        level->count_noncar_activations = GetAnInt(pF);
        if (level->count_noncar_activations != 0) {
            level->noncar_activations = BrMemAllocate(level->count_noncar_activations * sizeof(tNoncar_activation), kMem_smash_side_effects);
            for (j = 0; j < level->count_noncar_activations; j++) {
                GetPairOfInts(pF, &i1, &i2);
                level->noncar_activations[j].time_0x0 = (tS16)(i1 * 1000);
                level->noncar_activations[j].time_0x2 = (tS16)(i2 * 1000);
                level->noncar_activations[j].location_type = GetALineAndInterpretCommand(pF, gPosition_type_names, REC2_ASIZE(gPosition_type_names));
                level->noncar_activations[j].field_0x8 = (tS8)GetAnInt(pF);
                LoadMinMax(pF, &level->noncar_activations[j].bounds);
                ReadSmashableInitialSpeed(pF, &level->noncar_activations[j].speed_info);
            }
        }
        ReadShrapnelSideEffects(pF, &level->side_effects);
        level->extension_flags = GetAnInt(pF);
        if (level->extension_flags & 1) {
            level->field_0x280 = GetAnInt(pF);
        }
        level->room_turn_on_code = GetAnInt(pF);
        level->award.frequency = GetALineAndInterpretCommand(pF, gRepeatability_names, REC2_ASIZE(gRepeatability_names));
        if (level->award.frequency != 0) {
            level->award.field_0x4 = GetAScalar(pF);
            level->award.field_0x8 = GetAScalar(pF);
            level->award.field_0xc = GetAnInt(pF);
            level->award.field_0x10 = GetAnInt(pF);
        }
        level->count_runtime_variable_changes = GetAnInt(pF);
        for (j = 0; j < level->count_runtime_variable_changes; j++) {
            GetPairOfInts(pF, &v1, &v2);
            level->runtime_variable_changes[j].field_0x2 = v1;
            level->runtime_variable_changes[j].field_0x0 = v2;
        }
        level->count_pixelmaps = GetAnInt(pF);
        for (j = 0; j < level->count_pixelmaps; j++) {
            GetAString(pF, s);
            level->pixelmaps[j] = BrMapFind(s);
            if (level->pixelmaps[j] == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ee990
void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage) {
    int i;
    tCar_crush_smashable_part* part;

    pCar_crush_buffer_entry->count_smashables = GetAnInt(pF);
    part = pCar_crush_buffer_entry->smashables = BrMemAllocate(pCar_crush_buffer_entry->count_smashables * sizeof(tCar_crush_smashable_part), kMem_smashable_info);
    for (i = 0; i < pCar_crush_buffer_entry->count_smashables; i++, part++) {
        GetAString(pF, part->material_name);
        part->funk = -1;
        LoadSmashableLevels(pF, &part->levels, &part->count_smashable_levels, 0, pBrender_storage);
    }
}

/* local declarations instead of including 23-piping.h / 09-funks.h / 69-sound.h here */
extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);
extern void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern tS3_outlet* gCar_outlet;
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

// (reference only)
// FUNCTION: CARMA2_HW 0x004ef830
void C2_HOOK_FASTCALL ActuallyRepairSmashThunk(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pUnused, int pLevel) {
    extern void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

    ActuallyRepairSmash(pCar_spec, pSmashable, pLevel);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on ApexWorm/rec2 a6a8cc00e
// FUNCTION: CARMA2_HW 0x004ef840
void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    br_pixelmap* texture;

    pSmashable->field_0x4c = pLevel;
    texture = pSmashable->levels[pLevel].pixelmaps[IRandomBetween(0, pSmashable->levels[pLevel].count_pixelmaps - 1)];
    PipeSingleSmashTextureChange(pCar_spec, pSmashable->funk_material, texture);
    pSmashable->funk_material->user = texture;
    if (pCar_spec->field_0x1960 == NULL) {
        pSmashable->funk_material->colour_map = texture;
    }
    BrMaterialUpdate(pSmashable->funk_material, BR_MATU_COLOURMAP);
    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
    if (pLevel == 0 && pSmashable->funk >= 0) {
        EnableFunkotronic(pSmashable->funk);
    }
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gProgram_state.racing) {
        DRS3StartSound(gCar_outlet, eSoundId_SmashRepair);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// KillFragment

// AddRepairAnimationToQueue

// FUNCTION: CARMA2_HW 0x004ef910
void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data) {
    tCar_crush_smashable_part* smashable;
    int index;
    int i;
    int j;

    smashable = pSmash_data->smashables;
    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_spec, smashable, smashable->field_0x4c - 1);
            index = -1;
            for (j = 0; j < (int)REC2_ASIZE(gRepair_animations); j++) {
                if (gRepair_animations[j].field_0x0 == (void*)smashable) {
                    index = j;
                    break;
                }
            }
            if (index < 0) {
                for (j = 0; j < (int)REC2_ASIZE(gRepair_animations); j++) {
                    if (gRepair_animations[j].field_0x0 == NULL) {
                        index = j;
                        break;
                    }
                }
            }
            /* j equals index on every path that reaches here; the original indexes the array with it */
            if (index >= 0) {
                gRepair_animations[j].field_0x0 = (void*)smashable;
                gRepair_animations[j].field_0x4 = GetTotalTime();
                gRepair_animations[j].field_0x8 = (undefined4)(uintptr_t)pCar_spec;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ef440
intptr_t C2_HOOK_CDECL AddGhostActorToBuffer(br_actor* pActor, void* data) {
    br_actor** pActorBuffer = data;

    pActorBuffer[gShrapnel_ghost_actor_count] = pActor;
    gShrapnel_ghost_actor_count += 1;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ef460
void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds) {
    float x1, x2;
    float y1, y2;
    float z1, z2;

    GetThreeFloats(pF, &x1, &y1, &z1);
    GetThreeFloats(pF, &x2, &y2, &z2);
    pBounds->min.v[0] = MIN(x1, x2);
    pBounds->min.v[1] = MIN(y1, y2);
    pBounds->min.v[2] = MIN(z1, z2);
    pBounds->max.v[0] = MAX(x1, x2);
    pBounds->max.v[1] = MAX(y1, y2);
    pBounds->max.v[2] = MAX(z1, z2);
}

// FUNCTION: CARMA2_HW 0x004ef9c0
void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data) {
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_Spec, smashable, 0);
        }
    }
}

// InitGlassFragments

// InitDecals

// RemoveAllDecals

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004eed70
void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count) {
    char s[256];
    tShrapnel_spec* spec = pShrapnel_specs;
    int i;
    int j;
    int k;

    *pShrapnel_count = GetAnInt(pF);

    for (i = 0; i < *pShrapnel_count; i++, spec++) {
        spec[0].type = GetALineAndInterpretCommand(pF, gShrapnel_type_names, REC2_ASIZE(gShrapnel_type_names));
        if (spec[0].type == kShrapnelType_Abstract) {
            spec[0].type_info.abstract.count_materials = GetAnInt(pF);
            for (j = 0; j < spec[0].type_info.abstract.count_materials; j++) {
                br_material* material;

                GetAString(pF, s);
                material = BrMaterialFind(s);
                spec[0].type_info.abstract.materials[j] = material;
                if (material == NULL) {
                    FatalError(kFatalError_CannotFindSmashMaterial_S, s);
                }
                if (gMaterials_to_adapt_count < 200) {
                    gMaterials_to_adapt[gMaterials_to_adapt_count] = spec[0].type_info.abstract.materials[j];
                    gMaterials_to_adapt_count++;
                }
            }
            continue;
        }

        GetPairOfFloats(pF, &spec[0].min_speed_towards_you, &spec[0].max_speed_towards_you);
        spec[0].impactee_velocity_factor = GetAScalar(pF);
        spec[0].random_velocity_max = GetAScalar(pF);
        spec[0].random_up_velocity_max = GetAScalar(pF);
        spec[0].random_normal_velocity_max = GetAScalar(pF);
        spec[0].random_spin_rate_max = GetAScalar(pF) * 0.1f;

        if (spec[0].type != kShrapnelType_Shards) {
            spec[0].initial_pos.type = GetALineAndInterpretCommand(pF, gInitial_smashable_position_type_names, REC2_ASIZE(gInitial_smashable_position_type_names));
            switch (spec[0].initial_pos.type) {
                case kInitialSmashablePosition_SphereClumped:
                    spec[0].initial_pos.position.sphere.radius = GetAScalar(pF);
                    spec[0].initial_pos.position.sphere.where = GetALineAndInterpretCommand(pF, gInitial_position_sphere_where, REC2_ASIZE(gInitial_position_sphere_where));
                    break;
                case kInitialSmashablePosition_BoxClumped:
                    GetThreeFloats(pF, &spec[0].initial_pos.position.box.v[0], &spec[0].initial_pos.position.box.v[1], &spec[0].initial_pos.position.box.v[2]);
                    break;
                default:
                    break;
            }
        }
        if (spec[0].type != kShrapnelType_NonCars) {
            float min_time;
            float max_time;

            GetPairOfFloats(pF, &min_time, &max_time);
            spec[0].min_time_ms = (int)(min_time * 1000.0);
            spec[0].max_time_ms = (int)(max_time * 1000.0);
        }
        switch (spec[0].type) {
        case kShrapnelType_Shards:
            spec[0].type_info.shard.field_0x4 = GetAScalar(pF);
            spec[0].type_info.shard.field_0x8 = GetAnInt(pF);
            GetAString(pF, s);
            spec[0].type_info.shard.material = BrMaterialFind(s);
            if (spec[0].type_info.shard.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            if (gMaterials_to_adapt_count < 200) {
                gMaterials_to_adapt[gMaterials_to_adapt_count] = spec[0].type_info.shard.material;
                gMaterials_to_adapt_count++;
            }
            spec[0].type_info.shard.material->flags = BR_MATF_TWO_SIDED;
            BrMatrix23Identity(&spec[0].type_info.shard.material->map_transform);
            BrMaterialUpdate(spec[0].type_info.shard.material, BR_MATU_ALL);
            break;

        case kShrapnelType_GhostParts:
            GetPairOfInts(pF, &spec[0].type_info.ghost.field_0x0, &spec[0].type_info.ghost.field_0x4);
            spec[0].type_info.ghost.count_actors = GetAnInt(pF);
            if (spec[0].type_info.ghost.count_actors > 0) {

                spec[0].type_info.ghost.actors = BrMemAllocate(spec[0].type_info.ghost.count_actors * sizeof(br_actor*), kMem_smashable_env_info);
                spec[0].type_info.ghost.bounds_dx = 0.f;
                spec[0].type_info.ghost.bounds_dy = 0.f;
                for (j = 0; j < spec[0].type_info.ghost.count_actors; j++) {
                    char s2[256];

                    GetAString(pF, s);
                    PathCat(s2, gSmashable_track_environment_path, s);
                    spec[0].type_info.ghost.actors[j] = BrActorLoad(s2);
                    if (spec[0].type_info.ghost.actors[j] == NULL) {
                        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                    }
                    ProcessMaterials(spec[0].type_info.ghost.actors[j], (void*)AddFogificateMaterial);
                }
            } else {
                br_actor* actor;
                br_actor* actor_buffer[50];
                br_bounds bounds;
                char s2[256];

                GetAString(pF, s);
                PathCat(s2, gSmashable_track_environment_path, s);
                actor = BrActorLoad(s2);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                ProcessMaterials(actor, (void*)AddFogificateMaterial);

                gShrapnel_ghost_actor_count = 0;
                BrActorToBounds(&bounds, actor);
                spec[0].type_info.ghost.bounds_dx = bounds.max.v[0] - bounds.min.v[0];
                spec[0].type_info.ghost.bounds_dy = bounds.max.v[1] - bounds.min.v[1];

                BrActorEnum(actor, AddGhostActorToBuffer, actor_buffer);
                spec[0].type_info.ghost.count_actors = gShrapnel_ghost_actor_count;
                spec[0].type_info.ghost.actors = BrMemAllocate(gShrapnel_ghost_actor_count * sizeof(br_actor*), kMem_smashable_env_info);
                for (k = 0; k < spec[0].type_info.ghost.count_actors; k++) {
                    BrActorRemove(actor_buffer[k]);
                    spec[0].type_info.ghost.actors[k] = actor_buffer[k];
                }
                BrActorFree(actor);
            }
            break;

        case kShrapnelType_NonCars: {
            br_actor* actor;
            char s2[256];

            GetPairOfInts(pF, &spec[0].type_info.noncar.amount_min, &spec[0].type_info.noncar.amount_max);
            spec[0].type_info.noncar.probability_fire = GetAnInt(pF);
            if (spec[0].type_info.noncar.probability_fire != 0) {
                spec[0].type_info.noncar.count_fire_columns = GetAnInt(pF);
                GetPairOfInts(pF, &spec[0].type_info.noncar.min_smokiness, &spec[0].type_info.noncar.max_smokiness);
            }
            GetAString(pF, s);
            PathCat(s2, gSmashable_track_environment_path, s);
            actor = BrActorLoad(s2);
            if (actor == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s);
            }
            ProcessMaterials(actor, (void*)AddFogificateMaterial);
            spec[0].type_info.noncar.count_actors = GetAnInt(pF);
            spec[0].type_info.noncar.actors = BrMemAllocate(spec[0].type_info.noncar.count_actors * sizeof(br_actor*), kMem_smashable_env_info);

            for (j = 0; j < spec[0].type_info.noncar.count_actors; j++) {
                br_actor* sep_actor;
                br_actor* match_sep_actor;
                char s3[256];

                GetAString(pF, s);
                sep_actor = DRActorFindRecurse(actor, s);
                if (sep_actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                match_sep_actor = sep_actor;
                for (k = 0; k < gCount_smashable_noncar_shrapnel_actors; k++) {
                    if (strcmp(gSmashable_noncar_shrapnel_actors[k]->identifier, sep_actor->identifier) == 0) {
                        match_sep_actor = gSmashable_noncar_shrapnel_actors[k];
                        break;
                    }
                }
                if (k == gCount_smashable_noncar_shrapnel_actors) {
                    if (gCount_smashable_noncar_shrapnel_actors < 100) {
                        gSmashable_noncar_shrapnel_actors[gCount_smashable_noncar_shrapnel_actors] = match_sep_actor;
                        gCount_smashable_noncar_shrapnel_actors += 1;
                    }
                    if (match_sep_actor != actor) {
                        BrActorRemove(match_sep_actor);
                    }
                }
                spec[0].type_info.noncar.actors[j] = match_sep_actor;
                GetAString(pF, s);
                for (k = 0; k < gCount_smashable_noncars; k++) {
                    if (strcmp(gSmashable_noncars[k], s) == 0) {
                        break;
                    }
                }
                if (k == gCount_smashable_noncars) {
                    strcpy(gSmashable_noncars[gCount_smashable_noncars], s);
                    gCount_smashable_noncars += 1;
                }
                BrResFree(match_sep_actor->identifier);
                sprintf(s3, "&%c%c!bbbb.ACT", s[0], s[1]);
                match_sep_actor->identifier = BrResStrDup(match_sep_actor, s3);
            }
            if (spec[0].type_info.noncar.count_actors != 1) {
                BrActorFree(actor);
            } else if (spec[0].type_info.noncar.actors[0] != actor) {
                BrActorFree(actor);
            }
            break;
        }
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ee5e0
void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick) {
    char s[256];
    float a;
    float b;
    float c;

    GetAString(pF, s);
    if (strcmp(s, "none") != 0) {
        pSlick->material = BrMaterialFind(s);
        if (pSlick->material == NULL) {
            FatalError(kFatalError_CannotFindSmashMaterial_S, s);
        }
        if (gMaterials_to_adapt_count < 200) {
            gMaterials_to_adapt[gMaterials_to_adapt_count] = pSlick->material;
            gMaterials_to_adapt_count++;
        }
        GetThreeFloats(pF, &b, &a, &c);
        pSlick->field_0x0 = (int)b;
        pSlick->field_0xc = a;
        pSlick->field_0x10 = c;
        GetThreeFloats(pF, &c, &a, &b);
        pSlick->field_0x14 = c;
        pSlick->field_0x4 = a;
        pSlick->field_0x8 = b;
        GetPairOfFloats(pF, &c, &b);
        pSlick->field_0x20 = c;
        pSlick->field_0x24 = b;
        if (b != 0.0f) {
            GetPairOfFloats(pF, &c, &a);
            pSlick->field_0x18 = c;
            pSlick->field_0x1c = a;
            GetAString(pF, s);
            pSlick->material2 = BrMaterialFind(s);
            if (pSlick->material2 == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            if (gMaterials_to_adapt_count < 200) {
                gMaterials_to_adapt[gMaterials_to_adapt_count] = pSlick->material2;
                gMaterials_to_adapt_count++;
            }
        }
    } else {
        pSlick->material = NULL;
    }
}

// GLOBAL: CARMA2_HW 0x0065fef0
const char* gSmash_side_effect_direction_names[3] = {
    "parallel",
    "towards",
    "away",
};

// GLOBAL: CARMA2_HW 0x0065ff00
const char* gPosition_type_names[2] = {
    "relative",
    "absolute",
};

// FUNCTION: CARMA2_HW 0x004efce0
void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations) {
    int i;
    int i1, i2;
    float* p;

    /* no. of non car cuboids activated */
    pNon_car_cuboid_activations->count_activations = GetAnInt(pF);
    if (pNon_car_cuboid_activations->count_activations != 0) {
        pNon_car_cuboid_activations->activations = BrMemAllocate(pNon_car_cuboid_activations->count_activations * sizeof(tNon_car_cuboid_activation), kMem_smash_side_effects);
        for (i = 0; i < pNon_car_cuboid_activations->count_activations; i++) {
            GetPairOfInts(pF, &i1, &i2);
            pNon_car_cuboid_activations->activations[i].field_0x0 = (tS16)(i1 * 1000);
            pNon_car_cuboid_activations->activations[i].field_0x2 = (tS16)(i2 * 1000);
            pNon_car_cuboid_activations->activations[i].field_0x4 = GetALineAndInterpretCommand(pF, gPosition_type_names, REC2_ASIZE(gPosition_type_names));
            pNon_car_cuboid_activations->activations[i].field_0x8 = (tS8)GetAnInt(pF);
            LoadMinMax(pF, &pNon_car_cuboid_activations->activations[i].bounds);
            p = &pNon_car_cuboid_activations->activations[i].field_0x24;
            GetPairOfFloats(pF, &p[0], &p[1]);
            p[2] = GetAScalar(pF);
            p[3] = GetAScalar(pF);
            p[4] = GetAScalar(pF);
            p[5] = GetAScalar(pF);
            p[6] = GetAScalar(pF) / 10.f;
        }
    }
}

extern const char gAct_file_extension[];

// PARTIAL 99.15%: ReadShrapnelSideEffects - the only residual is the two .ACT operands: the original
// symbol table types original 0x594e9c as a named STRING (gAct_file_extension) while our reference to
// the reconstruted const char[] global is typed DATA (a string literal there shows up as ".ACT" (STRING)).
// Nothing in C can make our side a named STRING entity; both forms score the same.
// FUNCTION: CARMA2_HW 0x004ef550
void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects) {
    int i;
    int i1, i2;
    char s[256];

    pShrapnel_side_effects->count_side_effects = GetAnInt(pF);
    if (pShrapnel_side_effects->count_side_effects != 0) {
        pShrapnel_side_effects->side_effects = BrMemAllocate(pShrapnel_side_effects->count_side_effects * sizeof(tSmash_side_effect), kMem_smash_side_effects);
        for (i = 0; i < pShrapnel_side_effects->count_side_effects; i++) {
            GetPairOfInts(pF, &i1, &i2);
            pShrapnel_side_effects->side_effects[i].field_0x0 = 1000 * i1;
            pShrapnel_side_effects->side_effects[i].field_0x2 = 1000 * i2;
            GetAString(pF, s);
            strcpy(pShrapnel_side_effects->side_effects[i].field_0x8, s);
            if (strcmp(s, "*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 0;
            } else if (s[0] == '*' && DRStricmp(s + strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 1;
            } else if (s[0] == '*' && (DRStricmp(s + strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + strlen(s) - 4, gAct_file_extension) == 0)) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 2;
            } else if (DRStricmp(s, "&*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 3;
            } else if (s[0] == '&' && s[1] >= '0' && s[1] <= '9') {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 6;
            } else if (DRStricmp(s + strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 4;
            } else if (DRStricmp(s + strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + strlen(s) - 4, gAct_file_extension) == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 5;
            } else {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 0;
            }
            pShrapnel_side_effects->side_effects[i].field_0x4 = GetALineAndInterpretCommand(pF, gPosition_type_names, REC2_ASIZE(gPosition_type_names));
            LoadMinMax(pF, &pShrapnel_side_effects->side_effects[i].bounds);
            pShrapnel_side_effects->side_effects[i].field_0x44 = GetALineAndInterpretCommand(pF, gSmash_side_effect_direction_names, REC2_ASIZE(gSmash_side_effect_direction_names));
            pShrapnel_side_effects->side_effects[i].field_0x48 = GetAScalar(pF);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* (ReadPowerupSmashable); manually expanded as the original inlines it twice */
#define READ_POWERUP_SMASHABLE(SPEC, A, B) do { \
    int i; \
    int d1, d2; \
    memset((SPEC), 0, sizeof(tSmashable_item_spec)); \
    (SPEC)->mode_data.shrapnel.connotations.count_sounds = GetAnInt(pF); \
    for (i = 0; i < (SPEC)->mode_data.shrapnel.connotations.count_sounds; i++) { \
        (SPEC)->mode_data.shrapnel.connotations.sounds[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF)); \
    } \
    ReadShrapnelSpec(pF, (SPEC)->mode_data.shrapnel.connotations.shrapnel, &(SPEC)->mode_data.shrapnel.connotations.count_shrapnel); \
    ReadExplosion(pF, &(SPEC)->mode_data.shrapnel.connotations.special_effects.explosion_animation); \
    ReadSpillData(pF, &(SPEC)->mode_data.shrapnel.connotations.special_effects.slick); \
    ReadNonCarCuboidActivation(pF, &(SPEC)->mode_data.shrapnel.connotations.side_effects.activations); \
    ReadShrapnelSideEffects(pF, &(SPEC)->mode_data.shrapnel.connotations.side_effects.side_effects); \
    (SPEC)->mode_data.shrapnel.connotations.side_effects.extension_flags = GetAnInt(pF); \
    if ((SPEC)->mode_data.shrapnel.connotations.side_effects.extension_flags & 0x1) { \
        (SPEC)->mode_data.shrapnel.connotations.side_effects.extension_arg = GetAnInt(pF); \
    } \
    (SPEC)->mode_data.shrapnel.connotations.side_effects.room_turn_on_code = GetAnInt(pF); \
    (SPEC)->mode_data.shrapnel.connotations.award.frequency = GetALineAndInterpretCommand(pF, gRepeatability_names, REC2_ASIZE(gRepeatability_names)); \
    if ((SPEC)->mode_data.shrapnel.connotations.award.frequency != kRepeatability_None) { \
        (SPEC)->mode_data.shrapnel.connotations.award.field_0x4 = GetAScalar(pF); \
        (SPEC)->mode_data.shrapnel.connotations.award.field_0x8 = GetAScalar(pF); \
        (SPEC)->mode_data.shrapnel.connotations.award.field_0xc = GetAnInt(pF); \
        (SPEC)->mode_data.shrapnel.connotations.award.field_0x10 = GetAnInt(pF); \
    } \
    (SPEC)->mode_data.shrapnel.connotations.variable_changes.count = GetAnInt(pF); \
    for (i = 0; i < (SPEC)->mode_data.shrapnel.connotations.variable_changes.count; i++) { \
        GetPairOfInts(pF, &(A), &(B)); \
        (SPEC)->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x2 = (A); \
        (SPEC)->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x0 = (B); \
    } \
    (SPEC)->trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number; \
    (SPEC)->mode = kSmashableMode_Remove; \
} while (0)

// FUNCTION: CARMA2_HW 0x004efa00
// FUNCTION: CARMA2_SW 0x004eb660
void C2_HOOK_FASTCALL ReadPowerupSmash(FILE* pF) {
    READ_POWERUP_SMASHABLE(&gPowerup_pickup_smashable, d1, d2);
    READ_POWERUP_SMASHABLE(&gPowerup_respawn_smashable, d2, d1);

    memset(&gUnknown_smashable_006a3660, 0, sizeof(gUnknown_smashable_006a3660));
    gUnknown_smashable_006a3660.trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    gUnknown_smashable_006a3660.mode = kSmashableMode_Remove;
}

// FUNCTION: CARMA2_HW 0x004efe00
void C2_HOOK_FASTCALL InitSmashing(void) {
    int i;
    tSmash_vertex* frag;
    tDecal* decal;

    for (i = 200, frag = gSmash_glass_fragments; i != 0; i--, frag++) {
        tSmash_vertex* smash_vertex;

        smash_vertex = frag;
        smash_vertex->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_vertex->actor->model = BrModelAllocate(NULL, 3, 1);
        smash_vertex->actor->model->faces[0].vertices[0] = 0;
        smash_vertex->actor->model->faces[0].vertices[1] = 1;
        smash_vertex->actor->model->faces[0].vertices[2] = 2;
        smash_vertex->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_vertex->actor->model);
        smash_vertex->model = smash_vertex->actor->model;
        smash_vertex->actor->render_style = BR_RSTYLE_FACES;
        smash_vertex->end_time = 0;
    }
    for (i = 50, decal = gDecals; i != 0; i--, decal++) {
        tDecal* smash_quad;

        smash_quad = decal;
        smash_quad->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_quad->actor->model = BrModelAllocate(NULL, 4, 2);
        smash_quad->actor->model->faces[0].vertices[0] = 0;
        smash_quad->actor->model->faces[0].vertices[1] = 1;
        smash_quad->actor->model->faces[0].vertices[2] = 3;
        smash_quad->actor->model->faces[1].vertices[0] = 0;
        smash_quad->actor->model->faces[1].vertices[1] = 3;
        smash_quad->actor->model->faces[1].vertices[2] = 2;
        smash_quad->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_quad->actor->model);
        smash_quad->actor->render_style = BR_RSTYLE_FACES;
        smash_quad->time = 0;
    }
}

// VerySimpleRotate

// DoFragMovement

// MungeGlassFragments2

// FUNCTION: CARMA2_HW 0x004f00f0
void C2_HOOK_FASTCALL MungeGlassFragments(void) {
    tU32 the_time;
    tRepair_animation* animation;
    tCar_crush_smashable_part* smashable;
    int i;

    MungeGlassFragments2(0);
    the_time = GetTotalTime();
    for (i = 0; i < (int)REC2_ASIZE(gRepair_animations); i++) {
        animation = &gRepair_animations[i];
        if (animation->field_0x0 != NULL && the_time - animation->field_0x4 >= 150) {
            smashable = (tCar_crush_smashable_part*)animation->field_0x0;
            if (smashable->field_0x4c != 0) {
                ActuallyRepairSmash((tCar_spec*)animation->field_0x8, smashable, smashable->field_0x4c - 1);
                animation->field_0x4 += 150;
            }
            if (smashable->field_0x4c == 0) {
                animation->field_0x0 = NULL;
            }
        }
    }
}

// KillAllGlassFragments

// FUNCTION: CARMA2_HW 0x004f02b0
void C2_HOOK_FASTCALL CleanUpSmashStuff(void) {
    int i;
    tDecal* decal;
    tExplosion* explosion;

    MungeGlassFragments2(1);
    for (i = REC2_ASIZE(gDecals), decal = gDecals; i != 0; i--, decal++) {
        if (decal->time != 0) {
            BrActorRemove(decal->actor);
        }
        decal->time = 0;
    }
    for (i = REC2_ASIZE(gExplosions), explosion = gExplosions; i != 0; i--, explosion++) {
        if (explosion->actor->parent != NULL) {
            BrActorRemove(explosion->actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f0310
void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data) {
    int i;

    if (pCrush_data->smashables != NULL) {
        for (i = 0; i < pCrush_data->count_smashables; i++) {
            if (pCrush_data->smashables[i].field_0x70 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x70);
            }
            if (pCrush_data->smashables[i].field_0x78 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x78);
            }
            if (pCrush_data->smashables[i].levels != NULL) {
                BrMemFree(pCrush_data->smashables[i].levels);
            }
        }
        BrMemFree(pCrush_data->smashables);
    }
}

// DoBackwardShrapnel

// FUNCTION: CARMA2_HW 0x004f0430
void C2_HOOK_FASTCALL InitRepairAnimations(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRepair_animation) != 0xc);
    C2_HOOK_BUG_ON(REC2_ASIZE(gRepair_animations) != 25);

    for (i = 0; i < (int)REC2_ASIZE(gRepair_animations); i++) {
        gRepair_animations[i].field_0x0 = 0;
    }
}

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// FUNCTION: CARMA2_HW 0x004f0940
void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void) {

    BrMemFree(gSmashable_noncars);
}
// CloneMemory

// CloneEnvSmash

// FUNCTION: CARMA2_HW 0x004f0a80
void C2_HOOK_FASTCALL FreeConnotations(tConnotations* pConnotations, int pKeep_actors) {
    int i;
    int j;
    tExplosion_animation_group* group;

    for (i = 0; i < pConnotations->count_shrapnel; i++) {
        if (pConnotations->shrapnel[i].type == kShrapnelType_GhostParts && !pKeep_actors) {
            for (j = 0; j < pConnotations->shrapnel[i].type_info.ghost.count_actors; j++) {
                BrActorFree(pConnotations->shrapnel[i].type_info.ghost.actors[j]);
            }
            BrMemFree(pConnotations->shrapnel[i].type_info.ghost.actors);
        } else if (pConnotations->shrapnel[i].type == kShrapnelType_NonCars && !pKeep_actors) {
            for (j = 0; j < pConnotations->shrapnel[i].type_info.noncar.count_actors; j++) {
                if (pConnotations->shrapnel[i].type_info.noncar.actors[j]->parent == NULL) {
                    BrActorFree(pConnotations->shrapnel[i].type_info.noncar.actors[j]);
                }
            }
            BrMemFree(pConnotations->shrapnel[i].type_info.noncar.actors);
        }
    }
    if (pConnotations->special_effects.explosion_animation.count_groups != 0) {
        for (i = 0, group = pConnotations->special_effects.explosion_animation.groups; i < pConnotations->special_effects.explosion_animation.count_groups; i++, group++) {
            BrMemFree(group->frames);
        }
        BrMemFree(pConnotations->special_effects.explosion_animation.groups);
    }
    if (pConnotations->side_effects.activations.count_activations != 0) {
        BrMemFree(pConnotations->side_effects.activations.activations);
    }
    if (pConnotations->side_effects.side_effects.count_side_effects != 0) {
        BrMemFree(pConnotations->side_effects.side_effects.side_effects);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on ApexWorm/rec2 df6832dbd
// FUNCTION: CARMA2_HW 0x004f09c0
void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tSmashable_level) != 0x2c4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, count_levels, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, levels, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x70, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x78, 0x78);

    if (pSmash->trigger_type != -1) {
        if ((pSmash->mode == kSmashableMode_Decal || pSmash->mode == kSmashableMode_TextureChange) && pSmash->mode_data.texture_change.levels != NULL) {
            if ((void*)pSmash->mode_data.texture_change.field_0x70 != NULL) {
                BrMemFree((void*)pSmash->mode_data.texture_change.field_0x70);
            }
            if ((void*)pSmash->mode_data.texture_change.field_0x78 != NULL) {
                BrMemFree((void*)pSmash->mode_data.texture_change.field_0x78);
            }
            for (i = 1; i < pSmash->mode_data.texture_change.count_levels; i++) {
                FreeConnotations((tConnotations*)&pSmash->mode_data.texture_change.levels[i].count_sounds, pSmash->field_0x2dc);
            }
            BrMemFree(pSmash->mode_data.texture_change.levels);
        } else {
            FreeConnotations(&pSmash->mode_data.shrapnel.connotations, pSmash->field_0x2dc);
        }
        if (pSmash->flags & 0x80000000) {
            BrMaterialRemove(pSmash->trigger_object.material);
            BrMaterialFree(pSmash->trigger_object.material);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f0960
void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void) {
    int i;
    tSmashable_item_spec* spec;

    for (i = 0, spec = gTrack_smashable_environment_specs; i < gCount_track_smashable_environment_specs; i++, spec++) {
        FreeEnvSmash(spec);
    }
    if (gCount_track_smashable_environment_specs != 0) {
        BrMemFree(gTrack_smashable_environment_specs);
    }
    if (gNet_mode == eNet_mode_host) {
        BrMemFree(gNet_host_smashes);
    }
}

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

// ExtrapolateGeometryCoordinates

// ExtractDeTiledFaces

// AddDecal

// InsertQuad

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

extern float C2_HOOK_FASTCALL ApplySmashTriggerEffect(tSmashable_item_spec* pSpec, tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int* pFlag, int pNetwork);
extern void C2_HOOK_FASTCALL ApplySmashTriggerNumber(tPhysics_object* pObject, tFace_ref* pFace, br_scalar pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg7);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004f52f0
// FUNCTION: CARMA2_SW 0x004f0f50
float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork) {
    float result;
    int flag;
    char* identifier;
    tSmashable_item_spec* trigger;

    result = -1.f;
    flag = 0;
    if (pActor != NULL) {
        identifier = pActor->identifier;
        if (identifier != NULL && identifier[0] == '&') {
            trigger = gSmashable_track_indexable_triggers[10 * (identifier[1] - '0') + (identifier[2] - '0')];
            if (trigger != NULL) {
                if ((pSnap == 0 && (trigger->trigger_object.number.field_0x1 & 1) && identifier[3] != '!')
                        || (pSnap == 0 && (trigger->trigger_object.number.field_0x1 & 4) && identifier[3] == '!')
                        || (pSnap != 0 && (trigger->trigger_object.number.field_0x1 & 2))) {
                    gINT_006a3334 = 0;
                    result = ApplySmashTriggerEffect(trigger, pObject, pActor, pMagnitude, pPosition, pNormal, pForce, &flag, pNetwork);
                    if (flag != 0 && pNetwork == 0) {
                        ApplySmashTriggerNumber(pObject, NULL, pMagnitude, pPosition, pNormal, pForce, 1);
                    }
                }
            }
        }
    }
    return result;
}
struct tNet_stored_smash {
    undefined4 field_0x0;
    undefined4 field_0x4;
    tU32 player;
    br_scalar field_0xc;
    br_vector3 field_0x10;
    br_vector3 field_0x1c;
    br_vector3 field_0x28;
    undefined4 field_0x34;
};

extern void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);
extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);
extern void C2_HOOK_FASTCALL AddChangedPoint(const br_vector3* pPoint);
extern float C2_HOOK_FASTCALL DistanceFromFace(const br_vector3* pPoint, tFace_ref* pFace);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, undefined4* pArg3);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern tU32 gPHIL_last_physics_tick;
extern int gFace_num__car;
extern int gCount_shrapnel_shower_calls;
extern void C2_HOOK_FASTCALL PipeSingleSmashRemoveFaces(br_model* pModel, int pCount, const void* pFaces, br_material* pMaterial);
extern int gINT_006a8290;
extern float C2_HOOK_FASTCALL FUN_004f18d0(tPhysics_object* pObject, br_material* pMaterial, br_actor* pActor, tSmashable_item_spec* pSpec, tSmashable_item_spec* pSpec2, tFace_ref* pFace, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg11, int pArg12, int* pFlag, int pNetwork);

// FUNCTION: CARMA2_HW 0x004f1140
float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork) {
    br_uint_16 buffer[1026];
    br_matrix34 matrix;
    br_vector3 v;
    br_actor* actor;
    tU32 time;
    tSmashable_item_spec* pSpec;
    int seed;
    tFace_ref* pFace;
    tPhysics_object* object;
    br_uint_16* pRemoved;
    br_uint_16* pRemovedEnd;
    char* identifier;
    float result;
    int index;
    int flag;
    br_model* model;
    br_face* face;
    int changed;
    tSmashable_item_spec* trigger;

    pFace = (tFace_ref*)pArg2;
    result = -1.f;
    object = pObject;
    time = GetTotalTime();
    gINT_006a3334 = 0;
    flag = 0;
    seed = (int)pForce->v[2] ^ (int)pForce->v[1] ^ (int)pForce->v[0];
    v.v[0] = ((float*)object->field_0x1b4)[0] * 6.9;
    v.v[1] = ((float*)object->field_0x1b4)[1] * 6.9;
    v.v[2] = ((float*)object->field_0x1b4)[2] * 6.9;
    if (pFace->a != NULL && pFace->a->identifier != NULL && (tU8)pFace->a->identifier[1] == 0xa3) {
        if (pFace->a->render_style != 1) {
            index = 10 * (pFace->a->identifier[2] - '0') + (pFace->a->identifier[3] - '0');
            FUN_004f1030(object, index, pFace->a);
            if (index == 0xe) {
                Explode(&gExplosion_pix_animation_groups, pFace->a, &pFace->a->model->bounds, NULL);
                ApplySmashTriggerEffect(&gUnknown_smashable_006a3660, object, pFace->a, pMagnitude, pPosition, pNormal, pForce, &flag, pNetwork);
                DRS3StartSound3D(gCar_outlet, 0x1518, pPosition, &gZero_v__car, 1, 0xff, 0x10000, 0x10000);
            } else {
                ApplySmashTriggerEffect(&gPowerup_pickup_smashable, object, pFace->a, pMagnitude, pPosition, pNormal, pForce, &flag, pNetwork);
            }
            if (flag != 0 && pNetwork == 0) {
                ApplySmashTriggerNumber(object, pFace, pMagnitude, pPosition, pNormal, pForce, pSnap);
            }
        }
        object->box_face_ref = gFace_num__car - 2;
        result = 0.f;
        object->box_face_end = 0;
        object->box_face_start = 0;
        return result;
    }
    if (pFace->a != NULL && pFace->a->identifier != NULL && pFace->a->identifier[0] == '&' && pFace->a->identifier[1] >= '0' && pFace->a->identifier[1] <= '9') {
        trigger = gSmashable_track_indexable_triggers[10 * (pFace->a->identifier[1] - '0') + (pFace->a->identifier[2] - '0')];
        if (trigger != NULL && (((trigger->trigger_object.number.field_0x1 & 1) && pFace->a->identifier[3] != '!') || ((trigger->trigger_object.number.field_0x1 & 4) && pFace->a->identifier[3] == '!'))) {
            result = ApplySmashTriggerEffect(trigger, object, pFace->a, pMagnitude, pPosition, pNormal, pForce, &flag, pNetwork);
            if (flag != 0 && pNetwork == 0) {
                ApplySmashTriggerNumber(object, pFace, pMagnitude, pPosition, pNormal, pForce, pSnap);
            }
            return result;
        }
    }
    if (pFace->material != NULL && (identifier = pFace->material->identifier) != NULL && strlen(identifier) == 0xb && identifier[5] == '|') {
        gINT_006a8290 = 0;
        identifier = (char*)pFace->material;
        pSpec = &gTrack_smashable_environment_specs[(tU8)pFace->material->identifier[6] - 1];
        if (pSnap != 0 && (pSpec->flags & 2)) {
            return -1.f;
        }
        if (pSnap == 0 && (pSpec->flags & 1)) {
            return -1.f;
        }
        if (pSpec->mode == kSmashableMode_Decal || pSpec->mode == kSmashableMode_TextureChange) {
            result = FUN_004f18d0(object, (br_material*)identifier, pFace->a, pSpec, pSpec, pFace, pMagnitude, pPosition, pNormal, pForce, -1, pSpec->field_0x2dc == 0, &flag, pNetwork);
            if (flag != 0 && pNetwork == 0) {
                ApplySmashTriggerNumber(object, pFace, pMagnitude, pPosition, pNormal, pForce, pSnap);
            }
            return result;
        }
        if (pMagnitude < pSpec->mode_data.shrapnel.removal_threshold) {
            return -1.f;
        }
        flag = 0;
        actor = pFace->a;
        if (pSpec->mode == kSmashableMode_Remove) {
            model = actor->model;
            if (model != NULL && (model->flags & 0x80)) {
                changed = 0;
                index = 0;
                pRemoved = &buffer[0];
                pRemovedEnd = &buffer[2];
                for (face = model->faces; index < model->nfaces; index++, face++) {
                    if (face->material == (br_material*)identifier) {
                        if (pRemovedEnd < &buffer[1026]) {
                            *pRemoved = face->vertices[0];
                            pRemovedEnd[-1] = face->vertices[1];
                            *pRemovedEnd = face->vertices[2];
                            pRemovedEnd[1] = face->smoothing;
                            pRemoved += 4;
                            pRemovedEnd += 4;
                        }
                        memmove(face, face + 1, model->nfaces * sizeof(br_face) - (index + 1) * sizeof(br_face));
                        model->nfaces--;
                        index--;
                        face--;
                        changed = 1;
                    }
                }
                if (changed) {
                    flag = 1;
                    if (model->nfaces == 0) {
                        actor->render_style = 1;
                        PipeSingleSmashModelChange(actor, model, NULL);
                    } else {
                        DRModelUpdateDeluxTurbo(actor, model, 0x7fff);
                        PipeSingleSmashRemoveFaces(model, (int)((char*)pRemoved - (char*)&buffer[0]) >> 3, &buffer[0], (br_material*)identifier);
                    }
                }
                object->box_face_ref = gFace_num__car - 2;
                object->box_face_end = 0;
                object->box_face_start = 0;
            }
        } else if (pSpec->mode == kSmashableMode_NoChange) {
            flag = pSpec->field_0x10 == 0;
            pSpec->field_0x10 = GetTotalTime();
        }
        if (flag == 0) {
            return result;
        }
        result = pSpec->mode_data.shrapnel.removal_threshold;
        BrActorToActorMatrix34(&matrix, pFace->a, gUniverse_actor);
        SmashItIntoVerySmallPiecesIndeed(object, pSpec, 0, &pSpec->mode_data.shrapnel.connotations, seed, (undefined4)&v, pPosition, pPosition, pFace->a, time, gCount_shrapnel_shower_calls, &matrix, NULL, &pFace->normal, pMagnitude, (undefined4)pForce, pNetwork);
        if (pNetwork == 0) {
            ApplySmashTriggerNumber(object, pFace, pMagnitude, pPosition, pNormal, pForce, pSnap);
        }
        return result;
    }
    if (pFace->a != NULL && pFace->a->identifier != NULL && ((strlen(pFace->a->identifier) == 0xb && pFace->a->identifier[5] == '|') || (tU8)pFace->a->identifier[1] == 0xa3)) {
        if (pSnap != 0 && (gTrack_smashable_environment_specs[(tU8)pFace->a->identifier[6] - 1].flags & 2)) {
            return -1.f;
        }
        if (pSnap == 0 && (gTrack_smashable_environment_specs[(tU8)pFace->a->identifier[6] - 1].flags & 1)) {
            return -1.f;
        }
        if (pMagnitude < gTrack_smashable_environment_specs[(tU8)pFace->a->identifier[6] - 1].mode_data.shrapnel.removal_threshold) {
            return -1.f;
        }
        result = ApplySmashTriggerEffect(&gTrack_smashable_environment_specs[(tU8)pFace->a->identifier[6] - 1], object, pFace->a, pMagnitude, pPosition, pNormal, pForce, &flag, pNetwork);
        if (flag != 0 && pNetwork == 0) {
            ApplySmashTriggerNumber(object, pFace, pMagnitude, pPosition, pNormal, pForce, pSnap);
        }
    }
    return result;
}

extern int C2_HOOK_FASTCALL FUN_004f4c70(tFace_ref* pFace, br_vector3* pPoint, tFace_ref* pOut, int pFlag);
extern void C2_HOOK_FASTCALL FUN_004f36c0(const void* pMap, br_vector2* pUV, br_vector3* pOut);
extern br_actor* C2_HOOK_FASTCALL FUN_004f3820(tFace_ref* pFace, br_vertex* pQuad, br_material* pMaterial, undefined4 pPoly_array, undefined4 pVertex_array);
extern int C2_HOOK_FASTCALL FUN_004f39a0(const void* pMap, br_actor* pActor, br_vertex* pQuad, tU8* pEdge_at_bounds, tU8** pCorner, br_material* pMaterial, undefined4 pPoly_array, undefined4 pVertex_array);
extern void C2_HOOK_FASTCALL MungeSmashMaterialName(br_material* pMaterial, char** pIdentifier, char pIndex);
extern void C2_HOOK_CDECL BrMatrix23ApplyP(br_vector2* pA, const br_vector2* pB, const br_matrix23* pC);
extern int gCapacity_track_smashables;
extern void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);
extern const double gDouble_0058aee8;
extern const double gDOUBLE_0058aef8;
extern const float gFLOAT_0058af20;
extern const double gDOUBLE_0058af88;
extern const double gDOUBLE_0058afa8;
extern float gFloat_0058afb0;
extern br_matrix34 gShrapnel_shower_matrix;
extern br_vector3 gVector3_006a4688;
extern tSmashable_level* C2_HOOK_FASTCALL FUN_004ee290(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, undefined4 pArg9, undefined4 pArg10);

/* Per-triangle uv -> position mapping (size 0x30), as used by FUN_004f36c0. */
typedef struct {
    float p0[3];
    float uv0[2];
    float uv_apex[2];
    float m[4];
    br_face* face;
} tSmash_uv_triangle;

/* Mapping info of a quad made of two triangles sharing the longest edge (size 0x84). */
typedef struct {
    int axis[3];
    float plane[4];
    float uv_delta1[2];
    tSmash_uv_triangle tri[2];
} tSmash_uv_map;

/* Fields of the smashable environment spec that are not named in tSmashable_item_spec. */
#define SMASH_SPEC_FACE_A(pSpec)    (*(br_face**)((tU8*)(pSpec) + 0x98))
#define SMASH_SPEC_FACE_B(pSpec)    (*(br_face**)((tU8*)(pSpec) + 0x9c))
#define SMASH_SPEC_FLAGS_A(pSpec)   (*((tU8*)(pSpec) + 0xa0))
#define SMASH_SPEC_FLAGS_B(pSpec)   (*((tU8*)(pSpec) + 0xa1))

/* Remember which edge of which face was cut (a spec keeps up to two faces). */
static void SmashSpecRecordEdgeFace(tSmashable_item_spec* pSpec, br_face* pFace, tU8 pEdge_flag) {

    if (SMASH_SPEC_FACE_A(pSpec) == pFace) {
        SMASH_SPEC_FLAGS_A(pSpec) |= pEdge_flag;
    } else if (SMASH_SPEC_FACE_B(pSpec) == pFace) {
        SMASH_SPEC_FLAGS_B(pSpec) |= pEdge_flag;
    }
    if (SMASH_SPEC_FACE_A(pSpec) == NULL) {
        SMASH_SPEC_FLAGS_A(pSpec) |= pEdge_flag;
        SMASH_SPEC_FACE_A(pSpec) = pFace;
    } else {
        SMASH_SPEC_FLAGS_B(pSpec) |= pEdge_flag;
        SMASH_SPEC_FACE_B(pSpec) = pFace;
    }
}

/* Mark an edge of a face: directly when the level already exists, otherwise remember it in the spec. */
static void SmashSpecMarkEdgeFace(tSmashable_item_spec* pSpec, int pAlready_marked, int pRecord, br_face* pFace, tU8 pEdge_flag) {

    if (pAlready_marked) {
        if (pFace != NULL) {
            pFace->flags |= pEdge_flag;
        }
    } else if (pRecord) {
        SmashSpecRecordEdgeFace(pSpec, pFace, pEdge_flag);
    }
}

/* Apply the edge flags remembered in the spec to the faces. Returns 0 when nothing was remembered. */
static int SmashSpecApplyRecordedEdgeFlags(tSmashable_item_spec* pSpec) {
    br_face* face_a;
    br_face* face_b;

    face_a = SMASH_SPEC_FACE_A(pSpec);
    face_b = SMASH_SPEC_FACE_B(pSpec);
    if (face_a == NULL && face_b == NULL) {
        return 0;
    }
    if (face_a != NULL) {
        face_a->flags |= SMASH_SPEC_FLAGS_A(pSpec);
    }
    face_b = SMASH_SPEC_FACE_B(pSpec);
    if (face_b != NULL) {
        face_b->flags |= SMASH_SPEC_FLAGS_B(pSpec);
    }
    return 1;
}

/* Which edge flag to set for the cut edge. */
static void SmashSpecMarkCutEdge(tSmashable_item_spec* pSpec, int pAlready_marked, int pRecord, int pEdge, br_face* pCurrent_face, br_face* pAdjacent_face) {

    switch (pEdge) {
    case 0:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pAdjacent_face, 4);
        break;
    case 1:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pCurrent_face, 1);
        break;
    case 2:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pCurrent_face, 2);
        break;
    case 3:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pAdjacent_face, 2);
        break;
    }
}

/* Reorder a triangle so that its two first vertices form the longest edge. */
static void SmashOrderTriangleByLongestEdge(tU8** pCorner) {
    float* v0;
    float* v1;
    float* v2;
    tU8* swap;
    float d01;
    float d02;
    float d12;

    v0 = (float*)pCorner[0];
    v1 = (float*)pCorner[1];
    v2 = (float*)pCorner[2];
    d01 = (v0[0] - v1[0]) * (v0[0] - v1[0]) + (v0[2] - v1[2]) * (v0[2] - v1[2]) + (v0[1] - v1[1]) * (v0[1] - v1[1]);
    d02 = (v0[0] - v2[0]) * (v0[0] - v2[0]) + (v0[2] - v2[2]) * (v0[2] - v2[2]) + (v0[1] - v2[1]) * (v0[1] - v2[1]);
    d12 = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[2] - v2[2]) * (v1[2] - v2[2]) + (v1[1] - v2[1]) * (v1[1] - v2[1]);
    swap = pCorner[0];
    if (d02 <= d01 || d02 <= d12) {
        if (d01 < d12 && d02 < d12) {
            pCorner[0] = pCorner[2];
            pCorner[2] = swap;
        }
    } else {
        swap = pCorner[1];
        pCorner[1] = pCorner[2];
        pCorner[2] = swap;
    }
}

/* If the triangle contains both vertices of the edge (pIndex0, pIndex1), return its third vertex index, else -1. */
static int SmashFindTriangleApex(const br_uint_16* pVertices, int pIndex0, int pIndex1) {
    int a;
    int b;
    int c;

    a = pVertices[0];
    b = pVertices[1];
    c = pVertices[2];
    if ((a == pIndex0 && b == pIndex1) || (b == pIndex0 && a == pIndex1)) {
        return c;
    }
    if ((a == pIndex0 && c == pIndex1) || (c == pIndex0 && a == pIndex1)) {
        return b;
    }
    if ((b == pIndex0 && c == pIndex1) || (c == pIndex0 && b == pIndex1)) {
        return a;
    }
    return -1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004f18d0
float C2_HOOK_FASTCALL FUN_004f18d0(tPhysics_object* pObject, br_material* pMaterial, br_actor* pActor, tSmashable_item_spec* pSpec, tSmashable_item_spec* pSpec2, tFace_ref* pFace, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg11, int pArg12, int* pFlag, int pNetwork) {
    int tri_verts[8] = { 0, 0, 1, 2, 2, 1, 3, 3 };
    tSmash_uv_map mapinfo;
    tFace_ref replacement_face_ref;
    tFace_ref neighbour_face_ref;
    tFace_ref* face_ref;
    tSmashable_item_spec* spec;
    tSmashable_item_spec* candidate;
    tSmashable_level* level;
    br_actor* level_actor;
    br_actor* child_actor;
    br_model* model;
    br_vertex* child_vertices;
    br_vertex* corner[4];
    float* p0;
    float* p1;
    float* p2;
    float* p3;
    br_face* faces;
    br_face* current_face;
    br_face* adjacent_face;
    br_vector2 uv[4];
    br_vertex quad[4];
    br_vector3 contact;
    br_vector3 nudged;
    br_vector3 mid;
    br_vector3 opposite;
    float plane[4];
    float e1[3];
    float e2[3];
    float e3[3];
    float result;
    float dist;
    float abs_x;
    float abs_y;
    float abs_z;
    float du1;
    float dv1;
    float du2;
    float dv2;
    float du3;
    float dv3;
    float det;
    float neg_det;
    float delta_a;
    float delta_b;
    float v_hit;
    float u_floor;
    float v_floor;
    float u_ceil;
    float v_ceil;
    float u_min;
    float v_min;
    float u_max;
    float v_max;
    float u_center;
    float v_center;
    double u_hit;
    tU8 edge_cut[4];
    tU8 edge_at_bounds[4];
    int depth;
    int already_marked;
    int use_bounds;
    int num_faces;
    int idx0;
    int idx1;
    int apex;
    int axis_a;
    int axis_b;
    int axis_c;
    int found;
    int opposite_edge;
    int a;
    int b;
    int n;
    int k;
    int i;
    int dummy_flag;

    face_ref = pFace;
    result = -1.0f;
    already_marked = 0;
    *pFlag = 0;
    depth = gINT_006a8290;
    gINT_006a8290 = depth + 1;
    if (depth >= 0x10
            || (pSpec2->mode == kSmashableMode_Decal) == ((pActor->model->flags >> 7) & 1)
            || gCount_track_smashable_environment_specs >= gCapacity_track_smashables
            || pSpec2->mode_data.texture_change.count_levels <= 1
            || pMagnitude < gDOUBLE_0058aef8
            || pSpec2->mode_data.texture_change.count_levels - 1 <= (int)pSpec2->mode_data.texture_change.field_0x4c) {
        return result;
    }

    dist = DistanceFromFace(pPosition, face_ref);
    contact.v[0] = face_ref->normal.v[0] * -dist + pPosition->v[0];
    contact.v[1] = face_ref->normal.v[1] * -dist + pPosition->v[1];
    contact.v[2] = face_ref->normal.v[2] * -dist + pPosition->v[2];

    if (pArg11 < 0 && pSpec->field_0x2d8 != 0 && pSpec->mode == kSmashableMode_TextureChange) {
        if (!FUN_004f4c70(face_ref, &contact, &replacement_face_ref, 1)) {
            return gFLOAT_0058af20;
        }
        pMaterial = replacement_face_ref.material;
        face_ref = &replacement_face_ref;
        pSpec = &gTrack_smashable_environment_specs[(tU8)replacement_face_ref.material->identifier[6] - 1];
        pSpec2 = pSpec;
        pArg12 = (pSpec2->field_0x2dc == 0);
    }

    use_bounds = ((tSmashable_level*)pSpec2->mode_data.texture_change.levels)[1].flags & 2;
    model = face_ref->a->model;
    current_face = NULL;
    adjacent_face = NULL;

    if (face_ref->g < 0) {
        faces = model->faces;
        current_face = &faces[(tU16)face_ref->f];
        num_faces = model->nfaces;
        for (k = 0; k < 3; k++) {
            corner[k] = &model->vertices[current_face->vertices[k]];
        }
        SmashOrderTriangleByLongestEdge((tU8**)corner);
        idx0 = corner[0] - model->vertices;
        idx1 = corner[1] - model->vertices;
        apex = -1;
        for (n = 0; n < num_faces; n++) {
            br_face* candidate_face;

            candidate_face = &faces[n];
            if (candidate_face == current_face) {
                continue;
            }
            if (current_face->material != candidate_face->material) {
                continue;
            }
            apex = SmashFindTriangleApex(candidate_face->vertices, idx0, idx1);
            if (apex >= 0) {
                adjacent_face = candidate_face;
                break;
            }
        }
        if (n >= num_faces) {
            return gFLOAT_0058af20;
        }
        corner[3] = &model->vertices[apex];
        plane[0] = current_face->n.v[0];
        plane[1] = current_face->n.v[1];
        plane[2] = current_face->n.v[2];
        plane[3] = current_face->d;
        p0 = (float*)corner[0];
        p1 = (float*)corner[1];
        p2 = (float*)corner[2];
        p3 = (float*)corner[3];
        for (k = 0; k < 4; k++) {
            BrMatrix23ApplyP(&uv[k], &corner[k]->map, &face_ref->material->map_transform);
        }
    } else {
        v11group* group;
        v11face* group_faces;
        v11face* current_group_face;
        fmt_vertex* corner2[4];

        group = &model->prepared->groups[face_ref->g];
        group_faces = group->faces;
        current_group_face = &group_faces[(tU16)face_ref->f];
        num_faces = group->nfaces;
        for (k = 0; k < 3; k++) {
            corner2[k] = &group->vertices[current_group_face->vertices[k]];
        }
        SmashOrderTriangleByLongestEdge((tU8**)corner2);
        idx0 = corner2[0] - group->vertices;
        idx1 = corner2[1] - group->vertices;
        apex = -1;
        for (n = 0; n < num_faces; n++) {
            if (&group_faces[n] == current_group_face) {
                continue;
            }
            apex = SmashFindTriangleApex(group_faces[n].vertices, idx0, idx1);
            if (apex >= 0) {
                break;
            }
        }
        if (n >= num_faces) {
            return gFLOAT_0058af20;
        }
        corner2[3] = &group->vertices[apex];
        plane[0] = current_group_face->eqn.v[0];
        plane[1] = current_group_face->eqn.v[1];
        plane[2] = current_group_face->eqn.v[2];
        plane[3] = current_group_face->eqn.v[3];
        p0 = (float*)corner2[0];
        p1 = (float*)corner2[1];
        p2 = (float*)corner2[2];
        p3 = (float*)corner2[3];
        for (k = 0; k < 4; k++) {
            BrMatrix23ApplyP(&uv[k], &corner2[k]->map, &face_ref->material->map_transform);
        }
    }

    abs_x = fabs(face_ref->normal.v[0]);
    abs_y = fabs(face_ref->normal.v[1]);
    abs_z = fabs(face_ref->normal.v[2]);
    if (abs_x > abs_y && abs_x > abs_z) {
        axis_a = 2;
        axis_b = 1;
        axis_c = 0;
    } else if (abs_y > abs_x && abs_y > abs_z) {
        axis_a = 0;
        axis_b = 2;
        axis_c = 1;
    } else {
        axis_a = 0;
        axis_b = 1;
        axis_c = 2;
    }

    e1[0] = p1[0] - p0[0];
    e1[1] = p1[1] - p0[1];
    e1[2] = p1[2] - p0[2];
    e2[0] = p2[0] - p0[0];
    e2[1] = p2[1] - p0[1];
    e2[2] = p2[2] - p0[2];
    du1 = uv[1].v[0] - uv[0].v[0];
    dv1 = uv[1].v[1] - uv[0].v[1];
    du2 = uv[2].v[0] - uv[0].v[0];
    dv2 = uv[2].v[1] - uv[0].v[1];

    mapinfo.axis[0] = axis_a;
    mapinfo.axis[1] = axis_b;
    mapinfo.axis[2] = axis_c;
    mapinfo.plane[0] = plane[0];
    mapinfo.plane[1] = plane[1];
    mapinfo.plane[2] = plane[2];
    mapinfo.plane[3] = plane[3];
    mapinfo.uv_delta1[0] = du1;
    mapinfo.uv_delta1[1] = dv1;

    det = e1[axis_a] * e2[axis_b] - e1[axis_b] * e2[axis_a];
    neg_det = e1[axis_b] * e2[axis_a] - e1[axis_a] * e2[axis_b];
    mapinfo.tri[0].p0[0] = p0[0];
    mapinfo.tri[0].p0[1] = p0[1];
    mapinfo.tri[0].p0[2] = p0[2];
    mapinfo.tri[0].uv0[0] = uv[0].v[0];
    mapinfo.tri[0].uv0[1] = uv[0].v[1];
    mapinfo.tri[0].uv_apex[0] = uv[2].v[0];
    mapinfo.tri[0].uv_apex[1] = uv[2].v[1];
    mapinfo.tri[0].m[0] = (e2[axis_b] * du1 - e1[axis_b] * du2) / det;
    mapinfo.tri[0].m[1] = (e2[axis_a] * du1 - e1[axis_a] * du2) / neg_det;
    mapinfo.tri[0].m[2] = (e2[axis_b] * dv1 - e1[axis_b] * dv2) / det;
    mapinfo.tri[0].m[3] = (e2[axis_a] * dv1 - e1[axis_a] * dv2) / neg_det;
    mapinfo.tri[0].face = current_face;

    e3[0] = p3[0] - p0[0];
    e3[1] = p3[1] - p0[1];
    e3[2] = p3[2] - p0[2];
    du3 = uv[3].v[0] - uv[0].v[0];
    dv3 = uv[3].v[1] - uv[0].v[1];
    det = e1[axis_a] * e3[axis_b] - e1[axis_b] * e3[axis_a];
    neg_det = e1[axis_b] * e3[axis_a] - e1[axis_a] * e3[axis_b];
    mapinfo.tri[1].p0[0] = p0[0];
    mapinfo.tri[1].p0[1] = p0[1];
    mapinfo.tri[1].p0[2] = p0[2];
    mapinfo.tri[1].uv0[0] = uv[0].v[0];
    mapinfo.tri[1].uv0[1] = uv[0].v[1];
    mapinfo.tri[1].uv_apex[0] = uv[3].v[0];
    mapinfo.tri[1].uv_apex[1] = uv[3].v[1];
    mapinfo.tri[1].m[0] = (e3[axis_b] * du1 - e1[axis_b] * du3) / det;
    mapinfo.tri[1].m[1] = (e3[axis_a] * du1 - e1[axis_a] * du3) / neg_det;
    mapinfo.tri[1].m[2] = (e3[axis_b] * dv1 - e1[axis_b] * dv3) / det;
    mapinfo.tri[1].m[3] = (e3[axis_a] * dv1 - e1[axis_a] * dv3) / neg_det;
    mapinfo.tri[1].face = adjacent_face;

    if (uv[3].v[0] == uv[2].v[0] && uv[3].v[1] == uv[2].v[1]) {
        return gFLOAT_0058af20;
    }

    delta_a = contact.v[axis_a] - mapinfo.tri[0].p0[axis_a];
    delta_b = contact.v[axis_b] - mapinfo.tri[0].p0[axis_b];
    u_hit = mapinfo.tri[0].m[0] * delta_a + mapinfo.tri[0].m[1] * delta_b + uv[0].v[0];
    v_hit = mapinfo.tri[0].m[2] * delta_a + mapinfo.tri[0].m[3] * delta_b + uv[0].v[1];
    u_floor = (float)floor(u_hit);
    v_floor = (float)floor((double)v_hit);
    u_ceil = (float)ceil(u_hit);
    v_ceil = (float)ceil((double)v_hit);

    u_min = uv[0].v[0];
    v_min = uv[0].v[1];
    u_max = uv[0].v[0];
    v_max = uv[0].v[1];
    for (k = 1; k < 4; k++) {
        if (uv[k].v[0] < u_min) {
            u_min = uv[k].v[0];
        }
        if (uv[k].v[1] < v_min) {
            v_min = uv[k].v[1];
        }
        if (uv[k].v[0] > u_max) {
            u_max = uv[k].v[0];
        }
        if (v_max < uv[k].v[1]) {
            v_max = uv[k].v[1];
        }
    }
    if (use_bounds) {
        u_floor = u_min;
        v_floor = v_min;
        u_ceil = u_max;
        v_ceil = v_max;
    }

    edge_cut[0] = 0;
    edge_cut[1] = 0;
    edge_cut[2] = 0;
    edge_cut[3] = 0;
    edge_at_bounds[0] = 0;
    edge_at_bounds[1] = 0;
    edge_at_bounds[2] = 0;
    edge_at_bounds[3] = 0;
    if (fabs(u_floor - u_min) < gDOUBLE_0058afa8) {
        edge_at_bounds[0] = 1;
        u_floor = u_min;
    } else if (u_floor < u_min) {
        edge_at_bounds[0] = 1;
        u_floor = u_min;
        edge_cut[0] = 1;
    }
    if (fabs(v_floor - v_min) < gDOUBLE_0058afa8) {
        edge_at_bounds[1] = 1;
        v_floor = v_min;
    } else if (v_floor < v_min) {
        edge_at_bounds[1] = 1;
        v_floor = v_min;
        edge_cut[1] = 1;
    }
    if (fabs(u_ceil - u_max) < gDOUBLE_0058afa8) {
        edge_at_bounds[2] = 1;
        u_ceil = u_max;
    } else if (u_ceil > u_max) {
        edge_at_bounds[2] = 1;
        u_ceil = u_max;
        edge_cut[2] = 1;
    }
    if (fabs(v_ceil - v_max) < gDOUBLE_0058afa8) {
        edge_at_bounds[3] = 1;
        v_ceil = v_max;
    } else if (v_ceil > v_max) {
        edge_at_bounds[3] = 1;
        v_ceil = v_max;
        edge_cut[3] = 1;
    }
    if (u_floor == u_ceil || v_floor == v_ceil) {
        return gFLOAT_0058af20;
    }

    quad[0].map.v[0] = u_floor;
    quad[0].map.v[1] = v_floor;
    quad[1].map.v[0] = u_ceil;
    quad[1].map.v[1] = v_floor;
    quad[2].map.v[0] = u_floor;
    quad[2].map.v[1] = v_ceil;
    quad[3].map.v[0] = u_ceil;
    quad[3].map.v[1] = v_ceil;
    for (k = 0; k < 4; k++) {
        quad[k].n.v[0] = face_ref->normal.v[0];
        quad[k].n.v[1] = face_ref->normal.v[1];
        quad[k].n.v[2] = face_ref->normal.v[2];
        quad[k].red = 0xff;
        quad[k].grn = 0xff;
        quad[k].blu = 0xff;
    }
    for (k = 0; k < 4; k++) {
        FUN_004f36c0(&mapinfo, &quad[k].map, &quad[k].p);
    }

    u_center = (quad[3].map.v[0] + quad[2].map.v[0] + quad[1].map.v[0] + quad[0].map.v[0]) * gFloat_0058afb0;
    v_center = (quad[3].map.v[1] + quad[2].map.v[1] + quad[1].map.v[1] + quad[0].map.v[1]) * gFloat_0058afb0;
    for (k = 0; k < 4; k++) {
        i = (u_center <= quad[k].map.v[0]);
        if (v_center < quad[k].map.v[1]) {
            i |= 2;
        }
        gShrapnel_shower_matrix.m[i][0] = quad[k].p.v[0];
        gShrapnel_shower_matrix.m[i][1] = quad[k].p.v[1];
        gShrapnel_shower_matrix.m[i][2] = quad[k].p.v[2];
    }
    gVector3_006a4688.v[0] = mapinfo.plane[0];
    gVector3_006a4688.v[1] = mapinfo.plane[1];
    gVector3_006a4688.v[2] = mapinfo.plane[2];
    gINT_006a3334 = 1;

    spec = pSpec2;
    level_actor = pActor;
    if (pSpec2->field_0x2dc == 0) {
        found = 0;
        for (candidate = (tSmashable_item_spec*)pSpec->field_0x2d8; candidate != NULL && !found; candidate = (tSmashable_item_spec*)candidate->field_0x2d8) {
            child_actor = (br_actor*)candidate->field_0x2d4;
            if (child_actor != NULL) {
                child_vertices = child_actor->model->vertices;
                for (k = 0; k < 4; k++) {
                    if (fabs(child_vertices[k].p.v[0] - quad[k].p.v[0]) >= gDOUBLE_0058afa8
                            || fabs(child_vertices[k].p.v[1] - quad[k].p.v[1]) >= gDOUBLE_0058afa8
                            || fabs(child_vertices[k].p.v[2] - quad[k].p.v[2]) >= gDOUBLE_0058afa8) {
                        break;
                    }
                }
                if (k == 4) {
                    pSpec2 = candidate;
                    level_actor = child_actor;
                    found = 1;
                }
            }
        }
        spec = pSpec2;

        if (!found && (spec->flags & 0x80000000) == 0) {
            tSmashable_item_spec* clone;
            tSmashable_level* levels;
            int levels_size;

            clone = &gTrack_smashable_environment_specs[gCount_track_smashable_environment_specs];
            memcpy(clone, spec, sizeof(tSmashable_item_spec));
            if (spec->trigger_type != (tSmashable_trigger_type)-1
                    && (spec->mode == kSmashableMode_Decal || spec->mode == kSmashableMode_TextureChange)
                    && spec->mode_data.texture_change.levels != NULL) {
                levels_size = spec->mode_data.texture_change.count_levels * sizeof(tSmashable_level);
                levels = BrMemAllocate(levels_size, kMem_cloned_block);
                memcpy(levels, spec->mode_data.texture_change.levels, levels_size);
                clone->mode_data.texture_change.levels = levels;
            }
            if (clone->trigger_object.material != NULL) {
                clone->trigger_object.material = DRMaterialClone(clone->trigger_object.material, 0);
                clone->trigger_object.material->identifier = BrResStrDup(clone->trigger_object.material, spec->trigger_object.material->identifier);
                clone->mode_data.texture_change.trigger.material = clone->trigger_object.material;
                clone->flags |= 0x80000000;
                MungeSmashMaterialName(clone->trigger_object.material, &clone->trigger_object.material->identifier, (int)(clone - gTrack_smashable_environment_specs));
            }

            gCount_track_smashable_environment_specs++;
            clone->mode_data.texture_change.field_0x74 = 2;
            clone->mode_data.texture_change.field_0x78 = (undefined4)(uintptr_t)BrMemAllocate(12, kMem_smash_poly_array);
            clone->mode_data.texture_change.field_0x6c = 4;
            clone->mode_data.texture_change.field_0x70 = (undefined4)(uintptr_t)BrMemAllocate(8, kMem_smash_vertex_array);
            SMASH_SPEC_FACE_A(clone) = NULL;
            SMASH_SPEC_FACE_B(clone) = NULL;
            clone->field_0x2dc = 1;
            FogAccordingToGPSCDE(clone->trigger_object.material);

            if (clone->mode == kSmashableMode_Decal) {
                level_actor = FUN_004f3820(face_ref, quad, clone->trigger_object.material, clone->mode_data.texture_change.field_0x78, clone->mode_data.texture_change.field_0x70);
                clone->field_0x2d4 = (undefined4)(uintptr_t)level_actor;
            } else {
                if (!FUN_004f39a0(&mapinfo, face_ref->a, quad, edge_at_bounds, (tU8**)corner, clone->trigger_object.material, clone->mode_data.texture_change.field_0x78, clone->mode_data.texture_change.field_0x70)) {
                    FreeEnvSmash(clone);
                    gCount_track_smashable_environment_specs--;
                    return gFLOAT_0058af20;
                }
                pObject->box_face_ref = (tU16)(gFace_num__car - 2);
                pObject->box_face_start = 0;
                pObject->box_face_end = 0;
            }
            clone->field_0x2d8 = spec->field_0x2d8;
            spec->field_0x2d8 = (undefined4)(uintptr_t)clone;
            spec = clone;
        }
    }

    level = FUN_004ee290(NULL, pObject, &contact, pForce, level_actor, (undefined4)(uintptr_t)pSpec,
        (void*)&spec->mode_data, pMagnitude, spec->mode != kSmashableMode_Decal, pNetwork);
    already_marked = (level != NULL && level->collision_type == kTextureLevelCollisionChange_Edges);
    if (level != NULL && (level->collision_type == kTextureLevelCollisionChange_PassThrough
            || level->collision_type == kTextureLevelCollisionChange_Edges)) {
        result = level->trigger_threshold;
    }
    *pFlag = 1;

    if (pArg11 >= 0) {
        if (already_marked && SmashSpecApplyRecordedEdgeFlags(spec)) {
            /* the recorded flags were applied to the faces */
        } else if (pArg11 <= 3) {
            SmashSpecMarkCutEdge(spec, already_marked, pArg12, pArg11, current_face, adjacent_face);
        }
    }

    for (i = 0; i < 4; i++) {
        if (i == pArg11 || !edge_cut[i]) {
            continue;
        }
        a = tri_verts[i];
        b = tri_verts[i + 4];
        mid.v[0] = (float)((quad[a].p.v[0] + quad[b].p.v[0]) * gDOUBLE_0058af88);
        mid.v[1] = (float)((quad[a].p.v[1] + quad[b].p.v[1]) * gDOUBLE_0058af88);
        mid.v[2] = (float)((quad[a].p.v[2] + quad[b].p.v[2]) * gDOUBLE_0058af88);
        opposite_edge = (i + 2) & 3;
        a = tri_verts[opposite_edge];
        b = tri_verts[opposite_edge + 4];
        opposite.v[0] = (float)((quad[a].p.v[0] + quad[b].p.v[0]) * gDOUBLE_0058af88);
        opposite.v[1] = (float)((quad[a].p.v[1] + quad[b].p.v[1]) * gDOUBLE_0058af88);
        opposite.v[2] = (float)((quad[a].p.v[2] + quad[b].p.v[2]) * gDOUBLE_0058af88);
        nudged.v[0] = (float)((mid.v[0] - opposite.v[0]) * gDouble_0058aee8 + mid.v[0]);
        nudged.v[1] = (float)((mid.v[1] - opposite.v[1]) * gDouble_0058aee8 + mid.v[1]);
        nudged.v[2] = (float)((mid.v[2] - opposite.v[2]) * gDouble_0058aee8 + mid.v[2]);

        if (!FUN_004f4c70(face_ref, &nudged, &neighbour_face_ref, 0)) {
            continue;
        }
        if (already_marked) {
            if (!SmashSpecApplyRecordedEdgeFlags(spec)) {
                SmashSpecMarkCutEdge(spec, already_marked, pArg12, i, current_face, adjacent_face);
            }
        } else {
            SmashSpecMarkCutEdge(spec, already_marked, pArg12, i, current_face, adjacent_face);
        }
        if (pSpec2->field_0x2dc != 0) {
            for (n = 0; n < gCount_track_smashable_environment_specs; n++) {
                candidate = &gTrack_smashable_environment_specs[n];
                if (candidate->trigger_type == kSmashableTrigger_Material
                        && candidate->trigger_object.material == neighbour_face_ref.material
                        && (candidate->mode == kSmashableMode_TextureChange || candidate->mode == kSmashableMode_Decal)) {
                    pSpec2 = candidate;
                    break;
                }
            }
        }
        FUN_004f18d0(pObject, pMaterial, pActor, pSpec, pSpec2, &neighbour_face_ref, pMagnitude, &nudged, pNormal, pForce, opposite_edge, pArg12, &dummy_flag, pNetwork);
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004f4e20
float C2_HOOK_FASTCALL ApplySmashTriggerEffect(tSmashable_item_spec* pSpec, tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int* pFlag, int pNetwork) {
    br_matrix34 matrix;
    br_bounds3 bounds;
    br_vector3 v;
    int seed;
    tU32 time;
    float result;
    tPhysics_object* object;
    char* identifier;
    br_actor* child;
    int changed;
    int i;

    object = pObject;
    result = -1.f;
    time = GetTotalTime();
    gINT_006a3334 = 0;
    *pFlag = 0;
    if (pMagnitude < pSpec->mode_data.shrapnel.removal_threshold) {
        return -1.f;
    }
    v.v[0] = ((float*)object->field_0x1b4)[0] * 6.9;
    v.v[1] = ((float*)object->field_0x1b4)[1] * 6.9;
    v.v[2] = ((float*)object->field_0x1b4)[2] * 6.9;
    seed = (int)pForce->v[1] ^ (int)pForce->v[2] ^ (int)pForce->v[0];
    bounds = pActor->model->bounds;
    gINT_006a3334 = 0;
    changed = 0;
    switch (pSpec->mode) {
    case kSmashableMode_NoChange:
        if (pSpec->field_0x10 == 0) {
            changed = 1;
        }
        pSpec->field_0x10 = GetTotalTime();
        break;
    case kSmashableMode_Remove:
        if (pActor->render_style != 1) {
            changed = 1;
            pActor->render_style = 1;
            PipeSingleSmashModelChange(pActor, pActor->model, NULL);
            if (pActor != NULL && (identifier = pActor->identifier) != NULL && identifier[0] == '&'
                    && gSmashable_track_indexable_triggers[10 * (identifier[1] - '0') + (identifier[2] - '0')] != NULL
                    && pActor == object->actor) {
                pActor->t.t.translate.t.v[1] = -1000.f;
                object->transform_matrix.m[3][1] = -1000.f;
            }
        }
        object->box_face_end = 0;
        object->box_face_start = 0;
        object->box_face_ref = gFace_num__car - 2;
        break;
    case kSmashableMode_ReplaceModel:
        if (pActor->model != pSpec->replace.model) {
            changed = 1;
            PipeSingleSmashModelChange(pActor, pActor->model, pSpec->replace.model);
            pActor->model = pSpec->replace.model;
            if (PercentageChance(pSpec->replace.smoke_fire_chance.chance_fire) && pSpec->replace.smoke_fire_chance.model_1_int > 0) {
                for (i = 0; i < pSpec->replace.smoke_fire_chance.model_1_int; i++) {
                    CreateSmokeColumn2(0, pActor, NULL, NULL, IRandomBetween(pSpec->replace.smoke_fire_chance.model_2_int, pSpec->replace.smoke_fire_chance.model_3_int), IRandomBetween(0, pActor->model->prepared->groups->nvertices), 10000000);
                }
            }
            for (child = pActor->children; child != NULL; child = child->next) {
                PipeSingleSmashModelChange(child, child->model, NULL);
                child->render_style = 1;
            }
            AddChangedPoint(&pActor->t.t.translate.t);
        }
        object->box_face_ref = gFace_num__car - 2;
        object->box_face_end = 0;
        object->box_face_start = 0;
        break;
    default:
        break;
    }
    if (changed) {
        result = pSpec->mode_data.shrapnel.removal_threshold;
        *pFlag = 1;
        BrActorToActorMatrix34(&matrix, pActor, gUniverse_actor);
        SmashItIntoVerySmallPiecesIndeed(object, pSpec, 0, &pSpec->mode_data.shrapnel.connotations, seed, (undefined4)&v, &pActor->t.t.translate.t, pPosition, pActor, time, gCount_shrapnel_shower_calls, &matrix, &bounds, &gZero_vector__smash, pMagnitude, (undefined4)pForce, pNetwork);
    }
    return result;
}
// FUNCTION: CARMA2_HW 0x004f5110
void C2_HOOK_FASTCALL ApplySmashTriggerNumber(tPhysics_object* pObject, tFace_ref* pFace, br_scalar pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg7) {
    tCar_spec* car;
    void* owner;
    int driver;
    tNet_message* message;
    tNet_stored_smash* chunk;
    tNet_stored_smash* stored;
    br_vector3 point;
    float d;
    int id;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        owner = pObject->owner;
        driver = *(int*)((char*)owner + 0xc);
        if (driver > 5) {
            if (owner == NULL || driver != eDriver_local_human) {
                return;
            }
        }
    }
    if (pFace != NULL) {
        d = DistanceFromFace(pPosition, pFace);
        BrVector3Scale(&point, &pFace->normal, -d);
        BrVector3Add(&point, &point, pPosition);
    } else {
        BrVector3Copy(&point, pPosition);
    }
    message = NetBuildGuaranteedMessage(0x21, 0);
    chunk = (tNet_stored_smash*)&message->guaranteed;
    chunk->field_0x4 = gPHIL_last_physics_tick + 40;
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        car = (tCar_spec*)pObject->owner;
        if (car != NULL && car->driver > 5) {
            if (gNet_mode != eNet_mode_none) {
                id = (int)(NetPlayerFromCar(car) - gNet_players);
            } else {
                id = (tU16)car->car_ID;
            }
        } else {
            id = -1;
        }
    } else {
        id = -1;
    }
    chunk->player = id;
    chunk->field_0xc = pMagnitude;
    BrVector3Copy(&chunk->field_0x10, &point);
    chunk->field_0x1c = *pNormal;
    chunk->field_0x28 = *pForce;
    if (pArg7 != 0) {
        chunk->field_0x34 = 2;
    } else {
        chunk->field_0x34 = 0;
    }
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    if (gNet_mode == eNet_mode_host && gCount_host_smashes < 1000) {
        stored = &gNet_host_smashes[gCount_host_smashes];
        gCount_host_smashes++;
        *stored = *chunk;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f53e0
// FUNCTION: CARMA2_SW 0x004f1040
void C2_HOOK_FASTCALL NonCarSnapSmashCallback(tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce) {

    BashObject(pObject, pObject->actor, BrVector3Length(pForce), pPosition, pNormal, pForce, 1, 0);
}

// FUNCTION: CARMA2_HW 0x004f5470
int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material* pMaterial) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* spec = &gTrack_smashable_environment_specs[i];

        if (spec->trigger_type == kSmashableTrigger_Material && spec->trigger_object.material == pMaterial
                && ((spec->mode_data.texture_change.count_levels > 0 && spec->mode == kSmashableMode_TextureChange)
                    || spec->mode == kSmashableMode_Remove
                    || spec->mode == kSmashableMode_ReplaceModel)) {
            return 1;
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004f55d0
// FUNCTION: CARMA2_SW 0x004f1230
intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* data) {
    tMungeMaterialCB_Context* context = data;
    int face_i;
    br_face* face;

    if (pActor->model != NULL) {
        for (face_i = 0, face = pActor->model->faces; face_i < pActor->model->nfaces; face_i++, face++) {
            if (face->material == context->material_0x0) {
                face->material = context->material_0x4;
                if (context->indices != NULL && context->count_indices < context->capacity_indices) {
                    int i;

                    for (i = 0; i < 3; i++) {
                        int j;
                        int found = 0;

                        for (j = 0; j < context->count_indices; j++) {
                            if (face->vertices[i] == context->indices[j]) {
                                if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                    context->vertices[context->count_vertices].field_0x0[i] = (tU16)j;
                                }
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            if (context->count_indices != 0) {
                                BrVector3Accumulate(&context->field_0x20, &face->n);
                            } else {
                                BrVector3Copy(&context->field_0x20, &face->n);
                            }
                            if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                context->vertices[context->count_vertices].field_0x0[i] = (tU16)context->count_indices;
                            }
                            context->indices[context->count_indices] = face->vertices[i];
                            context->count_indices += 1;
                        }
                    }
                }
                if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                    context->count_vertices += 1;
                }
            }
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f54d0
// FUNCTION: CARMA2_SW 0x004f1130
void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal) {
    tMungeMaterialCB_Context munge_material_cb_ctx;

    munge_material_cb_ctx.material_0x0 = pMaterial_1;
    munge_material_cb_ctx.material_0x4 = pMaterial_2;
    munge_material_cb_ctx.capacity_indices = pCapacity_indices;
    munge_material_cb_ctx.indices = pIndices;
    munge_material_cb_ctx.count_indices = 0;
    munge_material_cb_ctx.capacity_vertices = pCapacity_vertices;
    munge_material_cb_ctx.vertices = pVertices;
    munge_material_cb_ctx.count_vertices = 0;

    if (pRecursive) {
        DRActorEnumRecurse(pActor, MungeMaterialCB, &munge_material_cb_ctx);
    } else {
        MungeMaterialCB(pActor, &munge_material_cb_ctx);
    }
    if (munge_material_cb_ctx.count_indices != 0) {
        BrVector3Normalise(pNormal, &munge_material_cb_ctx.field_0x20);
    }
    *pCount_indices = munge_material_cb_ctx.count_indices;
    *pCount_vertices = munge_material_cb_ctx.count_vertices;
}

// FUNCTION: CARMA2_HW 0x004f5750
void C2_HOOK_FASTCALL ReinitSmashing(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(sizeof(gSmash_glass_fragments[0]) != 0x38);
    C2_HOOK_BUG_ON(REC2_ASIZE(gDecals) != 50);
    C2_HOOK_BUG_ON(sizeof(gDecals[0]) != 0x8);
    C2_HOOK_BUG_ON(REC2_ASIZE(gQueued_net_smashes) != 50);
    C2_HOOK_BUG_ON(sizeof(gQueued_net_smashes[0]) != 0x38);
    C2_HOOK_BUG_ON(sizeof(tStored_smash) != 0x38);

    for (i = 0; i < (int)REC2_ASIZE(gSmash_glass_fragments); i++) {
        gSmash_glass_fragments[i].end_time = 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gDecals); i++) {
        gDecals[i].time = 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gQueued_net_smashes); i++) {
        gQueued_net_smashes[i].field_0x4 = 0;
    }
    gSize_powerup_queue = 0;
    gCount_host_smashes = 0;
    if (gNet_mode == eNet_mode_host) {
        gNet_host_smashes = BrMemAllocate(1000 * sizeof(tStored_smash), kMem_stored_smash);
    }
}

// LinkSmashies

// FUNCTION: CARMA2_HW 0x004f5a00
intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec) {
    return DRActorEnumRecurse(pCar_spec->car_master_actor, RepairModels, pCar_spec);
}

// FUNCTION: CARMA2_HW 0x004f5a10
intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_crush_buffer_entry* crush_data;

    if (user_crush != NULL) {
        crush_data = user_crush->crush_data;
        if (crush_data != NULL) {
            TotallyRepairSmash(pUser, crush_data);
        }
    }
    return 0;
}

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

// FUNCTION: CARMA2_HW 0x004f5ec0
// FUNCTION: CARMA2_SW 0x004f1b20
void C2_HOOK_FASTCALL LoadWastedExplosion(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups) {
    tPath_name the_path;
    char* name;
    char s[256];

    /* Wasted explosion settings */

    /* Percentage chance of car exploding */
    *pChance_explosion = GetAnInt(pF);
    /* Sound ID */
    *pExplosion_sound_id = GetAnInt(pF);

    PathCat(the_path, gApplication_path, "COMMON");
    /* Name of pixelmap file */
    GetALineAndDontArgue(pF, s);
    name = strtok(s, "\t ,/");
    name[strlen(name) - 4] = '\0';
    PathCat(the_path, the_path, name);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ReadExplosion(pF, pExplosion_groups);
}

// FUNCTION: CARMA2_HW 0x004f5f60
void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    (void)pOld_model;
    pActor->model = pNew_model;
}

// FUNCTION: CARMA2_HW 0x004f5f70
void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004f5f80
void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004f5f90
void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, int pCount) {
    pModel->nfaces -= pCount;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}

// SmashRestoreSomeFaces

// FUNCTION: CARMA2_HW 0x004f6020
void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex) {
    BrActorAdd(gOther_selfs[0], gDecals[pIndex].actor);
}

// FUNCTION: CARMA2_HW 0x004f6040
void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex) {
    BrActorRemove(gDecals[pIndex].actor);
}

/* local declarations instead of including 59-camera.h here */
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

// based on ApexWorm/rec2 c5b907296
// FUNCTION: CARMA2_HW 0x004f6060
void C2_HOOK_FASTCALL ChangeSmashTexture(br_material* pMaterial, tCar_spec* pCar_spec, br_pixelmap* pOld_texture, br_pixelmap* pNew_texture) {
    (void)pOld_texture;
    if (pCar_spec != NULL && pCar_spec->field_0x1960 != NULL) {
        pMaterial->user = pNew_texture;
    } else {
        pMaterial->colour_map = pNew_texture;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_COLOURMAP);
    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
}

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern void C2_HOOK_FASTCALL FillInBounds(tBounds* pBounds);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);
extern tWorld_callbacks gWorld_callbacks;
extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8);
extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);
extern int C2_HOOK_FASTCALL FUN_004f0d50(const float* pPoint, const float* pNormal, float pPlane_distance, const float* pVertex0, const float* pVertex1, const float* pVertex2);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004f6130
void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash) {
    tCar_spec* car;
    tPhysics_object* pCollision;
    br_vector3* pPoint;
    tBounds bounds;
    br_matrix34 matrix;
    tFace_ref faces[20];
    int car_id;
    int count;
    int i;

    pCollision = NULL;
    car_id = pSmash->field_0x8[0];
    if (car_id != -1) {
        if (gNet_mode == eNet_mode_none) {
            if ((car_id & 0xffffff00) == 0) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(car_id >> 8, car_id & 0xff);
            }
        } else if (car_id < gNumber_of_net_players) {
            car = gNet_players[car_id].car;
        } else {
            car = NULL;
        }
        if (car != NULL) {
            pCollision = car->collision_info;
        }
    }
    if (pCollision != NULL) {
        BrMatrix34Identity(&matrix);
        bounds.mat = &matrix;
        pPoint = (br_vector3*)&pSmash->field_0x8[2];
        *(br_vector3*)matrix.m[3] = *pPoint;
        BrVector3Set(&bounds.original_bounds.min, -0.05f, -0.05f, -0.05f);
        BrVector3Set(&bounds.original_bounds.max, 0.05f, 0.05f, 0.05f);
        FillInBounds(&bounds);
        count = FindFacesInBox(&bounds, faces, 20, &gWorld_callbacks);
        for (i = 0; i < count; i++) {
            if (FUN_004f0d50((const float*)pPoint, (const float*)&faces[i].normal, faces[i].d, (const float*)&faces[i].v[0], (const float*)&faces[i].v[1], (const float*)&faces[i].v[2]) != 0) {
                break;
            }
        }
        if (i == count) {
            return;
        }
        gINT_006a8294 = pSmash->field_0x8[11] & 1;
        SmashEnvironment((tPhysics_object*)pPoint, (undefined4*)&faces[i], *(float*)&pSmash->field_0x8[1], (br_vector3*)pSmash, (br_vector3*)&pSmash->field_0x8[5], (br_vector3*)&pSmash->field_0x8[8], (pSmash->field_0x8[11] >> 1) & 1, 1);
        FlushSmashQueue(0);
        gINT_006a8294 = 0;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* local declaration instead of including 04-netgame.h here */
extern tU32 gPHIL_last_physics_tick;
extern int gFace_num__car;
extern int gCount_shrapnel_shower_calls;
extern int gNext_explosion_slot;
extern void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData);
extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);
extern intptr_t C2_HOOK_CDECL FUN_004edb20(br_actor* pActor, void* pContext);

// FUNCTION: CARMA2_HW 0x004f62e0
void C2_HOOK_FASTCALL UpdateCrushTimers(void) {
    tQueued_net_smash* smash;
    int count;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    smash = gQueued_net_smashes;
    count = REC2_ASIZE(gQueued_net_smashes);
    while (count-- != 0) {
        if (smash->field_0x4 != 0 && gPHIL_last_physics_tick >= smash->field_0x4) {
            smash->field_0x4 = 0;
            DoNetSmash(smash);
        }
        smash++;
    }
}

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);
extern void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);
extern void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

// based on ApexWorm/rec2 4375aec8e
// FUNCTION: CARMA2_HW 0x004f0150
void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race) {
    tU32 the_time;
    float delta_time;
    tSmash_vertex* fragment;
    tSmash_vertex* other;
    tU32 field_0x18;
    int i;
    int j;

    the_time = GetTotalTime();
    if (gPrev_glass_munge != 0) {
        delta_time = (float)the_time - gPrev_glass_munge;
        fragment = gSmash_glass_fragments;

        for (i = (int)REC2_ASIZE(gSmash_glass_fragments); i != 0; i--, fragment++) {
            if (fragment->end_time != 0) {
                if (!pEnd_race
                    && (the_time < fragment->end_time
                        || (gAction_replay_mode && !(ARGetReplayRate() > 0.f)))
                    && the_time >= fragment->field_0xc) {
                    DoFragMovement(fragment, the_time, delta_time);
                    if (fragment->field_0x34) {
                        fragment->field_0x34 = 0;
                    }
                    fragment->time_last_move = the_time;
                } else {
                    field_0x18 = fragment->field_0x18;
                    if (the_time >= fragment->end_time) {
                        if ((tS32)field_0x18 >= 0) {
                            PipeSingleShrapnelShower(field_0x18, fragment->time_last_move, 0, 0,
                                NULL, NULL, 0.f, NULL, NULL, 0, NULL, 0, NULL, NULL, NULL);
                        } else if (field_0x18 & 0x40000000u) {
                            PipeSingleGibShower(field_0x18, fragment->time_last_move, 0, 0, 0,
                                NULL, NULL, NULL);
                        } else {
                            PipeSingleBloodSpurt(field_0x18, fragment->time_last_move, 0,
                                NULL, NULL, NULL);
                        }
                    }
                    for (j = (int)REC2_ASIZE(gSmash_glass_fragments), other = gSmash_glass_fragments; j != 0; j--, other++) {
                        if (other->end_time != 0
                            && other->field_0x18 == field_0x18) {
                            BrActorRemove(other->actor);
                            other->end_time = 0;
                        }
                    }
                }
            }
        }
    }
    gPrev_glass_munge = the_time;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0065fea0
const char* gSmashable_collision_type_names[3] = {
    "solid",
    "passthrough",
    "edges",
};

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL: DoFragMovement - frame (0x20), prologue, matrix-pointer tail and branch layout match; the x87 scheduling of
// the translate/omega block still differs (original keeps f_dt on the x87 stack, spills CSE temps at [esp+0x1c]/[esp+0x18]
// and finishes the v[1] update before the omega loads). 'pos' doubles as the per-axis delta temporary.
// (reference only)
// (reference only)
// FUNCTION: CARMA2_HW 0x004eff30
void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time) {
    extern float gFloat_0058aec0;
    float f_dt;
    double angle;
    float omega;
    br_vector3 pos;
    br_vector3 axis;
    br_matrix34* mat;

    f_dt = pDelta_time * 0.001f;
    if (f_dt != gFloat_0058aec0 && pFragment->field_0x34 == 0) {
        pos.v[0] = pFragment->v.v[0] * f_dt;
        pos.v[1] = pFragment->v.v[1] * f_dt - 4.9 * gGravity_multiplier * f_dt * f_dt;
        pos.v[2] = pFragment->v.v[2] * f_dt;
        pFragment->actor->t.t.translate.t.v[0] -= pos.v[0] * -0.14492753;
        pFragment->actor->t.t.translate.t.v[1] -= pos.v[1] * -0.14492753;
        pFragment->actor->t.t.translate.t.v[2] -= pos.v[2] * -0.14492753;
        mat = &pFragment->actor->t.t.mat;
        pos.v[1] = f_dt * gGravity_multiplier * 9.8;
        pFragment->v.v[1] -= pos.v[1];
        omega = BrVector3Length(&pFragment->omega);
        axis.v[0] = pFragment->omega.v[0] / omega;
        axis.v[1] = pFragment->omega.v[1] / omega;
        axis.v[2] = pFragment->omega.v[2] / omega;
        angle = omega * f_dt;
        if (fabs(angle) >= 0.0001) {
            pos.v[0] = mat->m[3][0];
            pos.v[1] = mat->m[3][1];
            pos.v[2] = mat->m[3][2];
            mat->m[3][0] = 0.f;
            mat->m[3][1] = 0.f;
            mat->m[3][2] = 0.f;
            BrMatrix34PostRotate(mat, BrRadianToAngle(angle), &axis);
            mat->m[3][0] = pos.v[0];
            mat->m[3][1] = pos.v[1];
            mat->m[3][2] = pos.v[2];
        }
    }
    if (pTime >= pFragment->field_0x10) {
        pFragment->actor->render_style = BR_RSTYLE_NONE;
    } else {
        pFragment->actor->render_style = BR_RSTYLE_FACES;
    }
}
extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);
// FUNCTION: CARMA2_HW 0x004c8870
void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15) {

    ARDoSingleVariedSession(0x1e, pId, 14,
        4, 0x40, pArg3,
        4, 0, pArg4,
        0xc, 4, pArg5,
        0xc, 0x10, pArg6,
        4, 0x9c, (int)pArg7,
        0xc, 0x1c, pArg8,
        0xc, 0x28, pArg9,
        4, 0x44, pArg2,
        0x30, 0x50, pArg13,
        4, 0x4c, pArg12,
        4, 0x80, pArg10,
        0x18, 0x84, pArg11,
        0xc, 0x34, pArg14,
        0x30, 0xa0, pArg15);
}
// based on ApexWorm/rec2 239956756 src/carma2/newcommon/23-piping.c
// FUNCTION: CARMA2_HW 0x004c8d40
void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {

    ARDoSingleVariedSession(50, pId, 5,
        4, 4, (uintptr_t)pArg2,
        4, 0, (uintptr_t)pArg3,
        12, 8, (uintptr_t)pArg4,
        12, 20, (uintptr_t)pArg5,
        12, 32, (uintptr_t)pArg6);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004ead00
void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos) {
    tExplosion_animation_group* group;
    tExplosion* slot;
    tU32 now;
    int count;
    int slot_index;
    int slot_found;
    int i;
    int j;
    int k;
    int frametime;
    int start_delay;
    tU16 angle;
    float scaling;
    float sx_min;
    float sx_max;
    float sy_min;
    float sy_max;
    float sz_min;
    float sz_max;
    float px;
    float py;
    float pz;
    float rand_value;
    int sign;

    now = GetTotalTime();
    if (pActor == NULL) {
        return;
    }
    for (i = 0; i < pExplosion_animation->count_groups; i++) {
        group = &pExplosion_animation->groups[i];
        count = IRandomBetween((tS16)group->min_count, (tS16)group->max_count);
        sx_min = group->min_xyz_factor.v[0] * pBounds->max.v[0];
        sx_max = group->max_xyz_factor.v[0] * pBounds->max.v[0];
        sy_min = group->min_xyz_factor.v[1] * pBounds->max.v[1];
        sy_max = group->max_xyz_factor.v[1] * pBounds->max.v[1];
        sz_min = group->min_xyz_factor.v[2] * pBounds->max.v[2];
        sz_max = group->max_xyz_factor.v[2] * pBounds->max.v[2];
        if (count <= 0) {
            continue;
        }
        for (j = 0; j < count; j++) {
            rand_value = FRandomBetween(sx_min, sx_max);
            sign = PercentageChance(50) ? 1 : -1;
            px = sign * rand_value;
            rand_value = FRandomBetween(sy_min, sy_max);
            sign = PercentageChance(50) ? 1 : -1;
            py = sign * rand_value;
            rand_value = FRandomBetween(sz_min, sz_max);
            sign = PercentageChance(50) ? 1 : -1;
            pz = sign * rand_value;
            if (pPos != NULL) {
                px += pPos->v[0];
                py += pPos->v[1];
                pz += pPos->v[2];
            }
            px += group->offset.v[0];
            py += group->offset.v[1];
            pz += group->offset.v[2];
            if (group->rotate_modus != kExplosionRotateModus_NoRotate) {
                angle = (tU16)IRandomBetween(0, 0xffff);
            } else {
                angle = 0;
            }
            scaling = FRandomBetween(group->min_scaling_factor, group->max_scaling_factor);
            frametime = IRandomBetween(group->min_frametime_ms, group->max_frametime_ms);
            start_delay = IRandomBetween(group->min_start_delay_ms, group->max_start_delay_ms);
            slot_found = 0;
            slot_index = 0;
            slot = gExplosions;
            do {
                if (slot->start == 0) {
                    slot_found = 1;
                    break;
                }
                slot_index++;
                slot++;
            } while (slot_index < (int)REC2_ASIZE(gExplosions));
            if (slot_index == (int)REC2_ASIZE(gExplosions)) {
                slot_index = gNext_explosion_slot;
                slot = &gExplosions[slot_index];
                do {
                    slot_index++;
                    if (slot_index < (int)REC2_ASIZE(gExplosions)) {
                        slot++;
                    } else {
                        slot_index = 0;
                        slot = gExplosions;
                    }
                    if (slot->finished != 0) {
                        slot_found = 1;
                        gNext_explosion_slot = slot_index;
                        break;
                    }
                } while (slot_index != gNext_explosion_slot);
            }
            if (!slot_found) {
                return;
            }
            slot->start = now + start_delay;
            slot->count_frames = (tS8)group->count_frames;
            slot->period = frametime;
            slot->field_0x14 = scaling * 0.14492753;
            slot->collision_actor = pActor;
            slot->finished = 0;
            slot->field_0x8 = 0xff;
            for (k = 0; k < group->count_frames; k++) {
                *(tExplosion_frame*)&slot->frames[k] = *(tExplosion_frame*)&group->frames[k];
            }
            slot->field_0x1c.v[0] = px;
            slot->field_0x1c.v[1] = py;
            slot->field_0x1c.v[2] = pz;
            slot->angle = angle;
            PipeSingleExplosion(slot_index, slot);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: DoDelayedNonCar
void C2_HOOK_FASTCALL DoDelayedNonCar(tU32 pTime, tDelayed_non_car* pNon_car) { NOT_IMPLEMENTED(); }
// STUB: DoDelayedSmash
void C2_HOOK_FASTCALL DoDelayedSmash(tDelayed_smash* pSmash) { NOT_IMPLEMENTED(); }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* context filled by the DRActorEnumRecurse callback FUN_004edb20 */
typedef struct {
    br_bounds3* bounds;
    tU8 field_0x4;
    tS8 count;
    undefined field_0x6[2];
    br_actor* actors[20];
} tNon_car_search_context;

// FUNCTION: CARMA2_HW 0x004ed860
void C2_HOOK_FASTCALL QueueNonCarCuboidActivation(br_actor* pActor, tNon_car_cuboid_activations* pActivations, void* pContext) {
    tNon_car_search_context search;
    tDelayed_non_car delayed;
    br_bounds3 bounds;
    br_matrix34* matrix;
    tNon_car_cuboid_activation* activation;
    int* pActive;
    tU32 time;
    tU32 min_column_x;
    tU32 min_column_z;
    tU32 max_column_x;
    tU32 max_column_z;
    int slot_index;
    int column_x;
    int column_z;
    int axis;
    int i;
    int j;

    time = GetTotalTime();
    if (pActor == NULL || pActor->identifier == NULL || pActor->identifier[0] != '&') {
        matrix = (br_matrix34*)((tU8*)pContext + 0x8c);
    } else {
        matrix = (br_matrix34*)((tU8*)pActor + 0x2c);
    }
    activation = pActivations->activations;
    for (i = 0; i < pActivations->count_activations; i++, activation++) {
        search.count = 0;
        if (activation->field_0x4 == 1) {
            bounds = activation->bounds;
        } else {
            BrMatrix34ApplyP(&bounds.min, &activation->bounds.min, matrix);
            BrMatrix34ApplyP(&bounds.max, &activation->bounds.max, matrix);
            for (axis = 0; axis < 3; axis++) {
                if (bounds.min.v[axis] > bounds.max.v[axis]) {
                    br_scalar value = bounds.min.v[axis];

                    bounds.min.v[axis] = bounds.max.v[axis];
                    bounds.max.v[axis] = value;
                }
            }
        }
        search.bounds = &bounds;
        search.field_0x4 = (tU8)activation->field_0x8;
        XZToColumnXZ((tU8*)&min_column_x, (tU8*)&min_column_z, bounds.min.v[0], bounds.min.v[2], &gProgram_state.track_spec);
        XZToColumnXZ((tU8*)&max_column_x, (tU8*)&max_column_z, bounds.max.v[0], bounds.max.v[2], &gProgram_state.track_spec);
        for (column_x = (int)(min_column_x & 0xff); column_x <= (int)(max_column_x & 0xff); column_x++) {
            for (column_z = (int)(min_column_z & 0xff); column_z <= (int)(max_column_z & 0xff); column_z++) {
                if (gProgram_state.track_spec.columns[column_z][column_x].actor_0x0 != NULL
                    && DRActorEnumRecurse(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, FUN_004edb20, &search) != 0) {
                    break;
                }
            }
        }
        delayed.field_0x0 = *(br_vector3*)((tU8*)pContext + 0x68);
        delayed.field_0xc = *(br_vector3*)((tU8*)matrix + 0x24);
        delayed.field_0x18 = (undefined4*)&activation->field_0x24;
        delayed.count_actions = search.count;
        for (j = 0; j < delayed.count_actions; j++) {
            delayed.actor_time_actions[j].actor = search.actors[j];
            delayed.actor_time_actions[j].time = time + IRandomBetween((tU16)activation->field_0x0, (tU16)activation->field_0x2);
        }
        pActive = &gSmash_explosions[0].active;
        slot_index = 0;
test_active:
        if (*pActive == 0) {
            goto have_free_slot;
        }
        pActive = (int*)((tU8*)pActive + sizeof(tSmash_explosion));
        slot_index++;
        if ((intptr_t)pActive < (intptr_t)&gSmash_explosions[REC2_ASIZE(gSmash_explosions)].active) {
            goto test_active;
        }
        goto no_free_slot;
have_free_slot:
        gSmash_explosions[slot_index].type = 0;
        gSmash_explosions[slot_index].active = (int)(intptr_t)pContext;
        gSmash_explosions[slot_index].what.non_car = delayed;
no_free_slot:
        ;
    }
}
/* the smash record this function builds and copies into gSmash_explosions[].what
   (0x94 bytes: the original copies 0x25 dwords, not sizeof(tDelayed_smash)) */
typedef struct {
    br_vector3 velocity;
    br_vector3 position;
    tU32 time;
    br_vector3 impulse;
    tU8 field_0x28[0xc];
    tSmashable_item_spec* spec;
    tFace_ref face;
    undefined4 type;
    float scale;
    void* object;
} tSmash_record;
extern float gFloat_0058aec0;
extern float gFloat_0058aec4;
extern double gDouble_0058aec8;
extern float gFloat_0058af38;
extern int gUnknown_006b9c60[];

// FUNCTION: CARMA2_HW 0x004edbe0
void C2_HOOK_FASTCALL FUN_004edbe0(tSmashable_item_spec* pSpec, void* pObject, tShrapnel_side_effects* pSide_effects, undefined4 pArg16, float pDistance) {
    tSmash_record record;
    tBounds bounds;
    tFace_ref face_list[400];
    tSmash_side_effect* eff;
    br_vector3 direction;
    tSmashable_item_spec* volatile spec;
    br_vector3 centroid;
    tFace_ref* pFace;
    br_actor* actor;
    const char* identifier;
    int side_effect_index;
    int face_count;
    int type;
    int axis;
    int slot_index;
    int* pActive;
    tU32 time;
    tU32 random_time;
    float offset_x;
    float offset_y;
    float offset_z;
    float length;

    time = GetTotalTime();
    eff = pSide_effects->side_effects;
    for (side_effect_index = 0; side_effect_index < pSide_effects->count_side_effects; side_effect_index++, eff++) {
        if (eff->field_0x4 == 1) {
            bounds.original_bounds = eff->bounds;
        } else {
            BrMatrix34ApplyP(&bounds.original_bounds.min, &eff->bounds.min, (br_matrix34*)((tU8*)pObject + 0x8c));
            BrMatrix34ApplyP(&bounds.original_bounds.max, &eff->bounds.max, (br_matrix34*)((tU8*)pObject + 0x8c));
            for (axis = 0; axis < 3; axis++) {
                if (bounds.original_bounds.min.v[axis] > bounds.original_bounds.max.v[axis]) {
                    br_scalar value = bounds.original_bounds.min.v[axis];

                    bounds.original_bounds.min.v[axis] = bounds.original_bounds.max.v[axis];
                    bounds.original_bounds.max.v[axis] = value;
                }
            }
        }
        bounds.mat = (br_matrix34*)((tU8*)gTrack_actor + 0x2c);
        face_count = FindFacesInBox(&bounds, face_list, 400, NULL);
        if (eff->field_0x44 == 0) {
            direction.v[0] = direction.v[0] / pDistance;
            direction.v[1] = direction.v[1] / pDistance;
            direction.v[2] = direction.v[2] / pDistance;
        }
        if (face_count > 0) {
            pFace = face_list;
            do {
                type = -1;
                spec = NULL;
                actor = NULL;
                identifier = NULL;
                if (pFace->material != NULL) {
                    identifier = pFace->material->identifier;
                }
                if (identifier != NULL && strlen(identifier) == 0xb && identifier[5] == '|') {
                    if (eff->field_0x28 == 0 || eff->field_0x28 == 1) {
                        goto material_centroid;
                    }
                    if (eff->field_0x28 != 4) {
                        goto next_face;
                    }
                    if (strcmp(identifier, eff->field_0x8) != 0) {
                        goto next_face;
                    }
material_centroid:
                    centroid.v[0] = (pFace->v[0].v[0] + pFace->v[1].v[0] + pFace->v[2].v[0]) * gFloat_0058af38;
                    centroid.v[1] = (pFace->v[0].v[1] + pFace->v[1].v[1] + pFace->v[2].v[1]) * gFloat_0058af38;
                    centroid.v[2] = (pFace->v[0].v[2] + pFace->v[2].v[2] + pFace->v[1].v[2]) * gFloat_0058af38;
                    type = 0;
                    spec = &gTrack_smashable_environment_specs[(tU8)identifier[6] - 1];
                    goto check;
                }
                actor = pFace->a;
                if (actor != NULL) {
                    identifier = actor->identifier;
                }
                if (identifier == NULL) {
                    goto check;
                }
                if (!((strlen(identifier) == 0xb && identifier[5] == '|') || ((tU8)identifier[1] == 0xa3))) {
                    if (*identifier != '&') {
                        goto check;
                    }
                    if (gUnknown_006b9c60[(tS8)identifier[2] + 10 * (tS8)identifier[1]] == 0) {
                        goto check;
                    }
                }
                if (*identifier == '&' && identifier[1] >= '0' && identifier[1] <= '9') {
                    if (identifier[3] == '!') {
                        goto next_face;
                    }
                    if (eff->field_0x28 == 0 || eff->field_0x28 == 3) {
                        goto actor_centroid;
                    }
                    if (eff->field_0x28 != 6) {
                        goto next_face;
                    }
                    if (identifier[1] != eff->field_0x8[1] || identifier[2] != eff->field_0x8[2]) {
                        goto next_face;
                    }
                } else {
                    if (eff->field_0x28 == 0 || eff->field_0x28 == 2) {
                        goto actor_centroid;
                    }
                    if (eff->field_0x28 != 5) {
                        goto next_face;
                    }
                    if (strcmp(identifier, eff->field_0x8) != 0) {
                        goto next_face;
                    }
                }
actor_centroid:
                centroid.v[0] = *(br_scalar*)((tU8*)actor + 0x50);
                centroid.v[1] = *(br_scalar*)((tU8*)actor + 0x54);
                centroid.v[2] = *(br_scalar*)((tU8*)actor + 0x58);
                type = 1;
                spec = &gTrack_smashable_environment_specs[(tU8)identifier[6] - 1];
check:
                if (type == -1) {
                    goto next_face;
                }
                pActive = (int*)&gSmash_explosions[0].what.smash.field_0x80;
                do {
                    if (*(pActive - 0x21) != 0 && *(pActive - 0x22) == 1) {
                        if (type == 1 && *pActive == (int)(intptr_t)pFace->a) {
                            spec = NULL;
                            break;
                        }
                        if (type == 0 && *pActive == (int)(intptr_t)pFace->a
                                && *(tS16*)((tU8*)pActive + 4) == pFace->g
                                && *(tS16*)((tU8*)pActive + 6) == pFace->f) {
                            spec = NULL;
                            break;
                        }
                    }
                    pActive += 0x32;
                } while ((tU8*)pActive < (tU8*)&gSmash_explosions[REC2_ASIZE(gSmash_explosions)].what.smash.field_0x80);
                if (spec != NULL && pFace->a != *(br_actor**)pObject) {
                        record.velocity = *(br_vector3*)((tU8*)pObject + 0x68);
                        random_time = IRandomBetween((tU16)eff->field_0x0, (tU16)eff->field_0x2);
                        record.type = type;
                        record.time = random_time + time;
                        if (eff->field_0x44 == 1) {
                            offset_x = *(br_scalar*)((tU8*)pObject + 0xb0) - centroid.v[0];
                            offset_y = *(br_scalar*)((tU8*)pObject + 0xb4) - centroid.v[1];
                            offset_z = *(br_scalar*)((tU8*)pObject + 0xb8) - centroid.v[2];
                            length = sqrt(offset_x * offset_x + offset_y * offset_y + offset_z * offset_z);
                            if (length <= gFloat_0058aec4) {
                                direction.v[0] = 1.0f;
                                direction.v[1] = 0.0f;
                                direction.v[2] = 0.0f;
                            } else {
                                direction.v[0] = offset_x * (gDouble_0058aec8 / length);
                                direction.v[1] = offset_y * (gDouble_0058aec8 / length);
                                direction.v[2] = offset_z * (gDouble_0058aec8 / length);
                            }
                        } else if (eff->field_0x44 == 2) {
                            offset_x = centroid.v[0] - *(br_scalar*)((tU8*)pObject + 0xb0);
                            offset_y = centroid.v[1] - *(br_scalar*)((tU8*)pObject + 0xb4);
                            offset_z = centroid.v[2] - *(br_scalar*)((tU8*)pObject + 0xb8);
                            length = sqrt(offset_x * offset_x + offset_y * offset_y + offset_z * offset_z);
                            if (length <= gFloat_0058aec4) {
                                direction.v[0] = 1.0f;
                                direction.v[1] = 0.0f;
                                direction.v[2] = 0.0f;
                            } else {
                                direction.v[0] = offset_x * (gDouble_0058aec8 / length);
                                direction.v[1] = offset_y * (gDouble_0058aec8 / length);
                                direction.v[2] = offset_z * (gDouble_0058aec8 / length);
                            }
                        }
                        if (eff->field_0x48 < gFloat_0058aec0) {
                            record.scale = -(pDistance * eff->field_0x48);
                            record.impulse.v[0] = direction.v[0] * record.scale;
                            record.impulse.v[1] = direction.v[1] * record.scale;
                            record.impulse.v[2] = direction.v[2] * record.scale;
                        } else {
                            record.impulse.v[0] = eff->field_0x48 * direction.v[0];
                            record.impulse.v[1] = eff->field_0x48 * direction.v[1];
                            record.impulse.v[2] = eff->field_0x48 * direction.v[2];
                            record.scale = eff->field_0x48;
                        }
                        record.position = centroid;
                        record.spec = spec;
                        record.face = *pFace;
                        record.object = pObject;
                        pActive = &gSmash_explosions[0].active;
                        for (slot_index = 0; *pActive != 0; slot_index++) {
                            pActive = (int*)((tU8*)pActive + sizeof(tSmash_explosion));
                            if ((tU8*)pActive >= (tU8*)&gSmash_explosions[REC2_ASIZE(gSmash_explosions)].active) {
                                break;
                            }
                        }
                        if (slot_index < (int)REC2_ASIZE(gSmash_explosions)) {
                            gSmash_explosions[slot_index].type = 1;
                            gSmash_explosions[slot_index].active = (int)(intptr_t)pObject;
                            *(tSmash_record*)&gSmash_explosions[slot_index].what.smash = record;
                        }
                    }
next_face:
                pFace++;
            } while (--face_count != 0);
        }
    }
}
extern int gMutant_tail_state;
extern tU32 gINT_00705b70;
extern void C2_HOOK_FASTCALL FUN_004d9d00(tPhysics_object* pObject, int pMode);

// FUNCTION: CARMA2_HW 0x004f1030
void C2_HOOK_FASTCALL FUN_004f1030(tPhysics_object* pObject, int pMode, br_actor* pActor) {
    void* info;
    int type;

    info = pObject->owner;
    if (pMode == 0xe) {
        if (pObject != NULL && info != NULL && pObject->flags_0x238 == 1) {
            type = *(volatile int*)((tU8*)info + 0xc);
            if ((type == 0 || (type != 2 && type != 3))
                    && gSize_powerup_queue < 0x32
                    && *(volatile int*)((tU8*)info + 0xc) > 5) {
                gPowerup_queue[gSize_powerup_queue].actor = pActor;
                gPowerup_queue[gSize_powerup_queue].car = pObject->owner;
                gPowerup_queue[gSize_powerup_queue].powerup_index = 0xe;
                gSize_powerup_queue = gSize_powerup_queue + 1;
                return;
            }
        }
        FUN_004d9d00(pObject, 0xe);
        return;
    }
    if (pObject != NULL && info != NULL && pObject->flags_0x238 == 1) {
        type = *(volatile int*)((tU8*)info + 0xc);
        if ((type == 0 || (type != 2 && type != 3))
                && gSize_powerup_queue < 0x32
                && *(volatile int*)((tU8*)info + 0xc) == 8) {
            goto enqueue;
        }
    }
    if (pObject->flags_0x238 == 0x20
            && (gMutant_tail_state == 2 || GetTotalTime() - gINT_00705b70 < 0x1d4c)) {
        goto enqueue;
    }
    return;
enqueue:
    gPowerup_queue[gSize_powerup_queue].actor = pActor;
    gPowerup_queue[gSize_powerup_queue].car = &gProgram_state.current_car;
    gPowerup_queue[gSize_powerup_queue].powerup_index = pMode;
    gSize_powerup_queue = gSize_powerup_queue + 1;
}
// FUNCTION: CARMA2_HW 0x004f0d50
int C2_HOOK_FASTCALL FUN_004f0d50(const float* pPoint, const float* pNormal, float pPlane_distance, const float* pVertex0, const float* pVertex1, const float* pVertex2) {
    float local_c[3];
    float a, b, c, d, e, f, g, h, denom, u, v;
    int axis;
    int i8;
    int i11;

    f = (pNormal[0] * pPoint[0] + (pNormal[1] * pPoint[1] + pNormal[2] * pPoint[2])) - pPlane_distance;
    if (fabs(f) > 0.5) {
        return 0;
    }
    f = -f;
    local_c[0] = pNormal[0] * f + pPoint[0];
    local_c[1] = pNormal[1] * f + pPoint[1];
    local_c[2] = pNormal[2] * f + pPoint[2];
    axis = 0;
    if (fabs(pNormal[0]) < fabs(pNormal[1])) {
        axis = 1;
    }
    if (fabs(pNormal[axis]) < fabs(pNormal[2])) {
        axis = 2;
    }
    switch (axis) {
    case 0:
        i8 = 1;
        i11 = 2;
        break;
    case 1:
        i8 = 0;
        i11 = 2;
        break;
    case 2:
        i8 = 0;
        i11 = 1;
        break;
    }
    a = pVertex0[i8];
    b = pVertex1[i8] - a;
    c = pVertex0[i11];
    d = pVertex1[i11] - c;
    e = pVertex2[i11] - c;
    f = pVertex2[i8] - a;
    g = local_c[i8] - a;
    h = local_c[i11] - c;
    if (fabs(b) > 2.3841858e-07) {
        denom = e * b - d * f;
        if (denom == 0.0f) {
            return 0;
        }
        u = (h * b - d * g) / denom;
        v = (g - u * f) / b;
    } else {
        u = g / f;
        v = (h - u * e) / d;
    }
    if (v >= -0.0001 && u >= -0.0001 && u + v <= 1.0001) {
        return 1;
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a829c
int gCount_shrapnel_shower_calls = 0;

// GLOBAL: CARMA2_HW 0x006a82a0
int gNext_explosion_slot = 0;

// DistanceFromFace (defined in 41-utility.c, in address order)

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004edb20
intptr_t C2_HOOK_CDECL FUN_004edb20(br_actor* pActor, void* pContext) {
    tNon_car_search_context* context;
    char* identifier;
    char first_digit;
    br_vector3* position;

    context = (tNon_car_search_context*)pContext;
    identifier = pActor->identifier;
    if (identifier == NULL || identifier[0] != '&') {
        return 0;
    }
    position = &pActor->t.t.translate.t;
    if (position->v[0] < context->bounds->min.v[0]) {
        return 0;
    }
    if (position->v[1] < context->bounds->min.v[1]) {
        return 0;
    }
    if (position->v[2] < context->bounds->min.v[2]) {
        return 0;
    }
    if (position->v[0] > context->bounds->max.v[0]) {
        return 0;
    }
    if (position->v[1] > context->bounds->max.v[1]) {
        return 0;
    }
    if (position->v[2] > context->bounds->max.v[2]) {
        return 0;
    }
    first_digit = identifier[1];
    if (first_digit < '0' || first_digit > '9') {
        return 0;
    }
    if ((tS8)context->field_0x4 < 0 || (tS8)context->field_0x4 == identifier[2] - 0x210 + first_digit * 10) {
        if (context->count >= 20) {
            return -1;
        }
        context->actors[context->count] = pActor;
        context->count++;
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// (reference)
// FUNCTION: CARMA2_HW 0x004d9d00
void C2_HOOK_FASTCALL FUN_004d9d00(tPhysics_object* pObject, int pMode) {
    int previous_dismemberfest;

    if ((pObject->field_0x1a0 & 0xf0000) != 0) {
        if (!IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()) {
            return;
        }
        if ((((tU8*)((tPed_character_instance*)pObject->owner)->ped->character)[0x14] & 4) == 0 && gCrush_deferred == 0) {
            FUN_004d35e0(((tPed_character_instance*)pObject->owner)->ped);
        }
        if (pMode != 0xe) {
            pObject->v.v[0] = 0.0f;
            pObject->v.v[1] = 0.0f;
            pObject->v.v[2] = 0.0f;
        }
        pObject->v.v[1] = (float)((double)FRandomBetween(0.7f, 1.0f) / ((double)pObject->M * gConst_world_scale) + pObject->v.v[1]);
        pObject->omega.v[2] = (float)((double)FRandomPosNeg(0.5f) * gConst_2pi / pObject->M + pObject->omega.v[2]);
        pObject->omega.v[0] = (float)((double)FRandomPosNeg(0.5f) * gConst_2pi / pObject->M + pObject->omega.v[0]);
        pObject->disable_move_rotate = 0;
        previous_dismemberfest = gPed_dismemberfest;
        gPed_dismemberfest = 1;
        FUN_004cd640(((tPed_character_instance*)pObject->owner)->ped, 500, 0x61, 0x61, gProgram_state.current_car.collision_info);
        gPed_dismemberfest = previous_dismemberfest;
        return;
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        pObject->v.v[1] = (float)((double)FRandomBetween(3.0f, 4.0f) / ((double)pObject->M * gConst_world_scale) + pObject->v.v[1]);
        pObject->omega.v[2] = (float)((double)FRandomPosNeg(1.75f) * gConst_2pi / pObject->M + pObject->omega.v[2]);
        pObject->omega.v[0] = (float)((double)FRandomPosNeg(1.75f) * gConst_2pi / pObject->M + pObject->omega.v[0]);
        pObject->disable_move_rotate = 0;
    } else {
        pObject->v.v[1] = (float)((double)FRandomBetween(1.0f, 2.0f) / ((double)pObject->M * gConst_world_scale) + pObject->v.v[1]);
        pObject->omega.v[2] = (float)((double)FRandomPosNeg(0.5f) * gConst_2pi / pObject->M + pObject->omega.v[2]);
        pObject->omega.v[0] = (float)((double)FRandomPosNeg(0.5f) * gConst_2pi / pObject->M + pObject->omega.v[0]);
        pObject->disable_move_rotate = 0;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x0058aec0
float gFloat_0058aec0 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058aec4
float gFloat_0058aec4 = 2.3841858e-07f;

// GLOBAL: CARMA2_HW 0x0058aec8
double gDouble_0058aec8 = 1.0;

// GLOBAL: CARMA2_HW 0x0058af38
float gFloat_0058af38 = 0.33333334f;

// GLOBAL: CARMA2_HW 0x006b9c60
int gUnknown_006b9c60[100] = { 0 };

// GLOBAL: CARMA2_HW 0x0058aee8
const double gDouble_0058aee8 = 0.001;

// GLOBAL: CARMA2_HW 0x0058aef8
const double gDOUBLE_0058aef8 = 0.1;

// GLOBAL: CARMA2_HW 0x0058af20
const float gFLOAT_0058af20 = -1.0f;

// GLOBAL: CARMA2_HW 0x0058af88
const double gDOUBLE_0058af88 = 0.5;

// GLOBAL: CARMA2_HW 0x0058afa8
const double gDOUBLE_0058afa8 = 0.01;

// GLOBAL: CARMA2_HW 0x0058afb0
float gFloat_0058afb0 = 0.25f;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// Walks the triangles/polygons attached to a face and finds the one that
// contains pPoint; the matching triangle's material is written to pOut.
// pFlag != 0 allows re-selecting the current edge, otherwise it is skipped.
// FUNCTION: CARMA2_HW 0x004f4c70
int C2_HOOK_FASTCALL FUN_004f4c70(tFace_ref* pFace, br_vector3* pPoint, tFace_ref* pOut, int pFlag) {
    tU8* p1;
    tU8* yNode;
    tU8* zNode;
    tU8* wNode;
    tU16* puVar7;
    int iVar2;
    int iVar5;
    int iVar1;
    int iVar9;
    int iVar3;

    p1 = (tU8*)pFace;
    memcpy(pOut, p1, 0x50);
    yNode = *(tU8**)(*(tU8**)(p1 + 0x48) + 0x18);
    if (*(tS16*)(p1 + 0x4c) < 0) {
        iVar2 = *(int*)(yNode + 8);
        puVar7 = *(tU16**)(yNode + 0xc);
        iVar1 = *(tU16*)(yNode + 0x12);
        for (iVar5 = 0; iVar5 < iVar1; iVar5++, puVar7 += 0x14) {
            if (pFlag == 0 && iVar5 == *(tU16*)(p1 + 0x4e)) {
                continue;
            }
            if (FUN_004f0d50((const float*)pPoint, (const float*)((tU8*)puVar7 + 0x18), *(br_scalar*)((tU8*)puVar7 + 0x24),
                             (const float*)(iVar2 + (tU32)puVar7[0] * 0x28),
                             (const float*)(iVar2 + (tU32)puVar7[1] * 0x28),
                             (const float*)(iVar2 + (tU32)puVar7[2] * 0x28)) != 0) {
                goto found_triangle;
            }
        }
        return 0;
found_triangle:
        pOut->f = (tS16)iVar5;
        pOut->normal = *(br_vector3*)((tU8*)puVar7 + 0x18);
        pOut->d = *(br_scalar*)((tU8*)puVar7 + 0x24);
        pOut->material = *(br_material**)((tU8*)puVar7 + 8);
        return 1;
    } else {
        zNode = *(tU8**)(yNode + 0x4c);
        iVar3 = (int)*(tS16*)(p1 + 0x4c);
        wNode = *(tU8**)(zNode + 0x18);
        iVar5 = *(tU16*)(wNode + 0x1c + iVar3 * 0x24);
        iVar2 = *(int*)(wNode + 0x10 + iVar3 * 0x24);
        puVar7 = *(tU16**)(wNode + 4 + iVar3 * 0x24);
        for (iVar9 = 0; iVar9 < iVar5; iVar9++, puVar7 += 0xe) {
            if (iVar9 == *(tU16*)(p1 + 0x4e)) {
                continue;
            }
            if (FUN_004f0d50((const float*)pPoint, (const float*)((tU8*)puVar7 + 0xc), *(br_scalar*)((tU8*)puVar7 + 0x18),
                             (const float*)((tU32)puVar7[0] * 0x20 + iVar2),
                             (const float*)((tU32)puVar7[1] * 0x20 + iVar2),
                             (const float*)((tU32)puVar7[2] * 0x20 + iVar2)) != 0) {
                goto found_polygon;
            }
        }
        return 0;
found_polygon:
        pOut->f = (tS16)iVar9;
        pOut->normal = *(br_vector3*)((tU8*)puVar7 + 0xc);
        pOut->d = *(br_scalar*)((tU8*)puVar7 + 0x18);
        return 1;
    }
    return 0;
}

// PARTIAL 60.47%: only x87 scheduling/fld order of the first two statements differs
// FUNCTION: CARMA2_HW 0x004f3740
void C2_HOOK_FASTCALL FUN_004f3740(const int* pIndices, const float* pTransform, const float* pPoint, float* pResult) {
    const float* pData = (const float*)pIndices;

    pResult[pIndices[0]] = ((pTransform[7] * pTransform[pIndices[0]] + (pPoint[0] - pTransform[3])) * pTransform[10]
        - ((pPoint[1] - pTransform[4]) + pTransform[9] * pTransform[pIndices[0]]) * pTransform[8])
        / (pTransform[10] * pTransform[7] - pTransform[9] * pTransform[8]);
    pResult[pIndices[1]] = ((pTransform[pIndices[1]] * pTransform[8] + (pPoint[0] - pTransform[3])) * pTransform[9]
        - ((pPoint[1] - pTransform[4]) + pTransform[10] * pTransform[pIndices[1]]) * pTransform[7])
        / (pTransform[9] * pTransform[8] - pTransform[10] * pTransform[7]);
    pResult[pIndices[2]] = (pData[6] - pResult[pIndices[0]] * pData[pIndices[0] + 3]
        - pResult[pIndices[1]] * pData[pIndices[1] + 3])
        / pData[pIndices[2] + 3];
}

// Picks the uv triangle of a smash mapping that is nearest (in uv space) to
// the given texture coordinate and lets FUN_004f3740 map it back to a position.
// FUNCTION: CARMA2_HW 0x004f36c0
void C2_HOOK_FASTCALL FUN_004f36c0(const void* pMap, br_vector2* pUV, br_vector3* pOut) {
    const float* pData;
    float dy1;
    float dx1;
    float dy0;
    float dx0;
    float distance1;
    float distance0;

    pData = (const float*)pMap;
    dy1 = pData[27] - pUV->v[1];
    dx1 = pData[26] - pUV->v[0];
    dy0 = pData[15] - pUV->v[1];
    dx0 = pData[14] - pUV->v[0];
    distance1 = dy1 * dy1 + dx1 * dx1;
    distance0 = dy0 * dy0 + dx0 * dx0;
    if (distance1 < distance0) {
        FUN_004f3740(pMap, (const float*)((const tU8*)pMap + 0x54), (const float*)pUV, (float*)pOut);
    } else {
        FUN_004f3740(pMap, (const float*)((const tU8*)pMap + 0x24), (const float*)pUV, (float*)pOut);
    }
}

// STUB: CARMA2_HW 0x004f3820
br_actor* C2_HOOK_FASTCALL FUN_004f3820(tFace_ref* pFace, br_vertex* pQuad, br_material* pMaterial, undefined4 pPoly_array, undefined4 pVertex_array) {
    NOT_IMPLEMENTED();
    return NULL;
}

// STUB: CARMA2_HW 0x004f39a0
int C2_HOOK_FASTCALL FUN_004f39a0(const void* pMap, br_actor* pActor, br_vertex* pQuad, tU8* pEdge_at_bounds, tU8** pCorner, br_material* pMaterial, undefined4 pPoly_array, undefined4 pVertex_array) {
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f3560
void C2_HOOK_FASTCALL MungeSmashMaterialName(br_material* pMaterial, char** pIdentifier, char pIndex) {
    char name[64];
    char suffix[64];
    int name_length;

    strcpy(name, *pIdentifier);
    name_length = strlen(name);
    if (name_length < 4) {
        sprintf(name, "Smash material %s has a name that is less than 4 characters long", *pIdentifier);
        PDFatalError(name);
    }
    strcpy(suffix, name + name_length - 4);
    sprintf(name + name_length - 4, "        ");
    sprintf(name + 5, "%cx", '|');
    name[6] = pIndex + 1;
    strcat(name, suffix);
    name[11] = 0;

    if (strlen(*pIdentifier) < 12) {
        BrResFree(*pIdentifier);
        *pIdentifier = BrResStrDup(pMaterial, name);
        return;
    }
    strcpy(*pIdentifier, name);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 96.72%: only the two calls to 0x409570 (unnamed in the diff; local cdecl placeholder below) differ
// FUNCTION: CARMA2_HW 0x004cc860
void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel) {
    extern int C2_HOOK_CDECL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, int pLevel, ...);
    extern void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tS8 pArg7, tS8 pArg8, tS8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12);
    tU32 old_flags;
    tU32 old_bit;
    tS8 old_4;
    tS8 old_5;

    old_flags = (tU32)pCharacter->field_0x14;
    old_4 = pCharacter->field_0x4;
    old_5 = pCharacter->field_0x5;
    old_bit = (old_flags >> 2) & 1;
    if (pLevel == 1) {
        SetCharacterPhysicsLevel(pCharacter, 1, *(undefined4*)(&pLevel + 1));
    } else {
        SetCharacterPhysicsLevel(pCharacter, pLevel);
    }
    if ((((pLevel >> 2) & 1) ^ old_bit) != 0) {
        PipeSinglePedStatus(pCharacter->ped, -1, -1, old_flags, pLevel, old_4, pCharacter->field_0x4, old_5, pCharacter->field_0x5, &pCharacter->field_0xc0, &pCharacter->field_0xcc, &pCharacter->field_0x8c);
        pCharacter->ped->field_0x06++;
    }
}

// Clears the pedestrian's smooth-turn cache entry and raises its character
// physics level to 5 (called when a ped becomes "physicing").
// FUNCTION: CARMA2_HW 0x004d35e0
void C2_HOOK_FASTCALL FUN_004d35e0(tPedestrian* pPed) {
    pPed->field_0x0c->field_0x6c = 0;
    SetCharacterPhysicsLevelAR(pPed->character, 5);
}

extern int gExploding_pedestrians;
extern int gImmortal_peds;
extern int gGoreLevel;
extern int gPed_severing_damage;
extern const int gPow2_array[32];
extern int C2_HOOK_FASTCALL FUN_004cd9e0(tPedestrian* pPed, int pBoneIndex);
extern void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pAction);

// GLOBAL: CARMA2_HW 0x0069bc0c
int gPed_severing_chance;

// STUB: CARMA2_HW 0x004cd9e0
int C2_HOOK_FASTCALL FUN_004cd9e0(tPedestrian* pPed, int pBoneIndex) {
    NOT_IMPLEMENTED();
    return 0;
}

/* Applies falling damage to a pedestrian: turns the impact speed into a move
   time, optionally severs limbs (dismemberfest / exploding pedestrians),
   updates hit points and plays the matching ped move and noise.
   Returns 1 when the ped is dead. */
// FUNCTION: CARMA2_HW 0x004cd640
int C2_HOOK_FASTCALL FUN_004cd640(tPedestrian* pPed, int pSpeed, int pMove1, int pMove2, tPhysics_object* pCollision_info) {
    int limit;
    double a;
    int move_time;
    int count;
    float time_left;

    count = 0;
    a = 0.068 / pPed->character->personality->form->simple_physicing[pPed->character->field_0x5].collision_info->M;
    move_time = (int)(a * a * pSpeed);
    if (pPed->character->personality->form->non_humanoid != 0 && (gPed_dismemberfest != 0 || gExploding_pedestrians != 0)) {
        if (move_time < 100) {
            move_time = 100;
        }
    }
    time_left = (float)move_time;
    if (pCollision_info != NULL && gImmortal_peds == 0 && gGoreLevel >= 2) {
        while (1) {
            if (gPed_dismemberfest != 0) {
                limit = 5;
            } else {
                limit = gPed_severing_damage;
            }
            time_left = time_left - (float)limit;
            if (time_left < 0.0f) {
                break;
            }
            if (count >= 5) {
                break;
            }
            if (gExploding_pedestrians == 0 && gPed_dismemberfest == 0) {
                if (!PercentageChance(gPed_severing_chance)) {
                    continue;
                }
            }
            count += FUN_004cd9e0(pPed, IRandomBetween(1, pPed->character->personality->form->count_bones - 1));
        }
    }
    if (count > 4) {
        ((tU8*)pPed->field_0x0c)[0x2e] |= 2;
    }
    if (pPed->hit_points <= 0) {
        if (move_time <= 2) {
            return 1;
        }
        MakePedNoise(pPed, 5, 0, pCollision_info);
        return 1;
    }
    if (pPed->character->personality->form->non_humanoid != 0 && pPed->character->field_0xc != 0) {
        pPed->hit_points = 0;
        move_time = 0;
    }
    if ((pPed->character->field_0xc & gPow2_array[pPed->character->personality->form->index_head_bone]) != 0) {
        pPed->hit_points = 0;
    } else if (move_time >= 100) {
        pPed->hit_points = 0;
    } else {
        pPed->hit_points -= (tS8)move_time;
    }
    if (gImmortal_peds != 0) {
        if (pPed->hit_points <= 0) {
            pPed->hit_points = 1;
        }
    }
    if (pPed->hit_points > 0) {
        if (pMove1 >= 0) {
            SetPedMove(pPed, pMove1, -1, 0, 1, GetTotalTime(), 5);
        }
        MakePedNoise(pPed, 4, 0, pCollision_info);
        return 0;
    }
    if (pMove2 >= 0) {
        SetPedMove(pPed, pMove2, -1, 0, 1, GetTotalTime(), 5);
    }
    MakePedNoise(pPed, 1, 0, pCollision_info);
    KillPedestrian(pPed, pCollision_info);
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* temporary placeholder: the original at 0x00409570 is cdecl (2 or 3 stack args); the 61-pedestrn.c placeholder is fastcall */
int C2_HOOK_CDECL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, int pLevel, ...) {
    NOT_IMPLEMENTED();
    return 0;
}
