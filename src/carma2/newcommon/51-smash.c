#include "51-smash.h"

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


// GLOBAL: CARMA2_HW 0x006abee8
br_vector3 gZero_vector__smash;

// GLOBAL: CARMA2_HW 0x006b7888
tSmashable_environment_name* gSmashable_noncars;

// GLOBAL: CARMA2_HW 0x0065fee8
const char* gExplosion_rotate_names[2] = {
    "norotate",
    "randomrotate",
};

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

// MungeExplosions

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
// STUB: CARMA2_HW 0x004eb180
void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    NOT_IMPLEMENTED();
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

// based on carpocalypse2 src/carma2/common/smash.c:121
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

// MungeDelayedSideEffects

// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004ecea0
void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor* pActor) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DoConnotationsMultiLevel

// SmashItIntoVerySmallPiecesIndeed

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
// FreeExplosion

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
    /* based on carpocalypse2 crush.c:321 */
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
    tRepair_animation* animation;
    int index;
    int i;
    int j;

    smashable = pSmash_data->smashables;
    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_spec, smashable, smashable->field_0x4c - 1);
            index = -1;
            animation = gRepair_animations;
            for (j = 0; j < (int)REC2_ASIZE(gRepair_animations); j++, animation++) {
                if (animation->field_0x0 == (void*)smashable) {
                    index = j;
                    break;
                }
            }
            if (index < 0) {
                animation = gRepair_animations;
                for (j = 0; j < (int)REC2_ASIZE(gRepair_animations); j++, animation++) {
                    if (animation->field_0x0 == NULL) {
                        index = j;
                        break;
                    }
                }
            }
            if (index >= 0) {
                gRepair_animations[index].field_0x0 = (void*)smashable;
                gRepair_animations[index].field_0x4 = GetTotalTime();
                gRepair_animations[index].field_0x8 = (undefined4)(uintptr_t)pCar_spec;
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
/* based on carpocalypse2 src/carma2/common/shrapnel.c:49 */
// FUNCTION: CARMA2_HW 0x004eed70
void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count) {
    char s[256];
    int i;
    int j;
    int k;

    *pShrapnel_count = GetAnInt(pF);

    for (i = 0; i < *pShrapnel_count; i++) {
        pShrapnel_specs[i].type = GetALineAndInterpretCommand(pF, gShrapnel_type_names, REC2_ASIZE(gShrapnel_type_names));
        if (pShrapnel_specs[i].type == kShrapnelType_Abstract) {
            pShrapnel_specs[i].type_info.abstract.count_materials = GetAnInt(pF);
            for (j = 0; j < pShrapnel_specs[i].type_info.abstract.count_materials; j++) {
                br_material* material;

                GetAString(pF, s);
                material = BrMaterialFind(s);
                pShrapnel_specs[i].type_info.abstract.materials[j] = material;
                if (material == NULL) {
                    FatalError(kFatalError_CannotFindSmashMaterial_S, s);
                }
                if (gMaterials_to_adapt_count < 200) {
                    gMaterials_to_adapt[gMaterials_to_adapt_count] = pShrapnel_specs[i].type_info.abstract.materials[j];
                    gMaterials_to_adapt_count++;
                }
            }
            continue;
        }

        GetPairOfFloats(pF, &pShrapnel_specs[i].min_speed_towards_you, &pShrapnel_specs[i].max_speed_towards_you);
        pShrapnel_specs[i].impactee_velocity_factor = GetAScalar(pF);
        pShrapnel_specs[i].random_velocity_max = GetAScalar(pF);
        pShrapnel_specs[i].random_up_velocity_max = GetAScalar(pF);
        pShrapnel_specs[i].random_normal_velocity_max = GetAScalar(pF);
        pShrapnel_specs[i].random_spin_rate_max = GetAScalar(pF) * 0.1f;

        if (pShrapnel_specs[i].type != kShrapnelType_Shards) {
            pShrapnel_specs[i].initial_pos.type = GetALineAndInterpretCommand(pF, gInitial_smashable_position_type_names, REC2_ASIZE(gInitial_smashable_position_type_names));
            switch (pShrapnel_specs[i].initial_pos.type) {
                case kInitialSmashablePosition_SphereClumped:
                    pShrapnel_specs[i].initial_pos.position.sphere.radius = GetAScalar(pF);
                    pShrapnel_specs[i].initial_pos.position.sphere.where = GetALineAndInterpretCommand(pF, gInitial_position_sphere_where, REC2_ASIZE(gInitial_position_sphere_where));
                    break;
                case kInitialSmashablePosition_BoxClumped:
                    GetThreeFloats(pF, &pShrapnel_specs[i].initial_pos.position.box.v[0], &pShrapnel_specs[i].initial_pos.position.box.v[1], &pShrapnel_specs[i].initial_pos.position.box.v[2]);
                    break;
                default:
                    break;
            }
        }
        if (pShrapnel_specs[i].type != kShrapnelType_NonCars) {
            float min_time;
            float max_time;

            GetPairOfFloats(pF, &min_time, &max_time);
            pShrapnel_specs[i].min_time_ms = (int)(min_time * 1000.0);
            pShrapnel_specs[i].max_time_ms = (int)(max_time * 1000.0);
        }
        switch (pShrapnel_specs[i].type) {
        case kShrapnelType_Shards:
            pShrapnel_specs[i].type_info.shard.field_0x4 = GetAScalar(pF);
            pShrapnel_specs[i].type_info.shard.field_0x8 = GetAnInt(pF);
            GetAString(pF, s);
            pShrapnel_specs[i].type_info.shard.material = BrMaterialFind(s);
            if (pShrapnel_specs[i].type_info.shard.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            if (gMaterials_to_adapt_count < 200) {
                gMaterials_to_adapt[gMaterials_to_adapt_count] = pShrapnel_specs[i].type_info.shard.material;
                gMaterials_to_adapt_count++;
            }
            pShrapnel_specs[i].type_info.shard.material->flags = BR_MATF_TWO_SIDED;
            BrMatrix23Identity(&pShrapnel_specs[i].type_info.shard.material->map_transform);
            BrMaterialUpdate(pShrapnel_specs[i].type_info.shard.material, BR_MATU_ALL);
            break;

        case kShrapnelType_GhostParts:
            GetPairOfInts(pF, &pShrapnel_specs[i].type_info.ghost.field_0x0, &pShrapnel_specs[i].type_info.ghost.field_0x4);
            pShrapnel_specs[i].type_info.ghost.count_actors = GetAnInt(pF);
            if (pShrapnel_specs[i].type_info.ghost.count_actors > 0) {

                pShrapnel_specs[i].type_info.ghost.actors = BrMemAllocate(pShrapnel_specs[i].type_info.ghost.count_actors * sizeof(br_actor*), kMem_smashable_env_info);
                pShrapnel_specs[i].type_info.ghost.bounds_dx = 0.f;
                pShrapnel_specs[i].type_info.ghost.bounds_dy = 0.f;
                for (j = 0; j < pShrapnel_specs[i].type_info.ghost.count_actors; j++) {
                    char s2[256];

                    GetAString(pF, s);
                    PathCat(s2, gSmashable_track_environment_path, s);
                    pShrapnel_specs[i].type_info.ghost.actors[j] = BrActorLoad(s2);
                    if (pShrapnel_specs[i].type_info.ghost.actors[j] == NULL) {
                        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                    }
                    ProcessMaterials(pShrapnel_specs[i].type_info.ghost.actors[j], (void*)AddFogificateMaterial);
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
                pShrapnel_specs[i].type_info.ghost.bounds_dx = bounds.max.v[0] - bounds.min.v[0];
                pShrapnel_specs[i].type_info.ghost.bounds_dy = bounds.max.v[1] - bounds.min.v[1];

                BrActorEnum(actor, AddGhostActorToBuffer, actor_buffer);
                pShrapnel_specs[i].type_info.ghost.count_actors = gShrapnel_ghost_actor_count;
                pShrapnel_specs[i].type_info.ghost.actors = BrMemAllocate(gShrapnel_ghost_actor_count * sizeof(br_actor*), kMem_smashable_env_info);
                for (k = 0; k < pShrapnel_specs[i].type_info.ghost.count_actors; k++) {
                    BrActorRemove(actor_buffer[k]);
                    pShrapnel_specs[i].type_info.ghost.actors[k] = actor_buffer[k];
                }
                BrActorFree(actor);
            }
            break;

        case kShrapnelType_NonCars: {
            br_actor* actor;
            char s2[256];

            GetPairOfInts(pF, &pShrapnel_specs[i].type_info.noncar.amount_min, &pShrapnel_specs[i].type_info.noncar.amount_max);
            pShrapnel_specs[i].type_info.noncar.probability_fire = GetAnInt(pF);
            if (pShrapnel_specs[i].type_info.noncar.probability_fire != 0) {
                pShrapnel_specs[i].type_info.noncar.count_fire_columns = GetAnInt(pF);
                GetPairOfInts(pF, &pShrapnel_specs[i].type_info.noncar.min_smokiness, &pShrapnel_specs[i].type_info.noncar.max_smokiness);
            }
            GetAString(pF, s);
            PathCat(s2, gSmashable_track_environment_path, s);
            actor = BrActorLoad(s2);
            if (actor == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s);
            }
            ProcessMaterials(actor, (void*)AddFogificateMaterial);
            pShrapnel_specs[i].type_info.noncar.count_actors = GetAnInt(pF);
            pShrapnel_specs[i].type_info.noncar.actors = BrMemAllocate(pShrapnel_specs[i].type_info.noncar.count_actors * sizeof(br_actor*), kMem_smashable_env_info);

            for (j = 0; j < pShrapnel_specs[i].type_info.noncar.count_actors; j++) {
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
                pShrapnel_specs[i].type_info.noncar.actors[j] = match_sep_actor;
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
            if (pShrapnel_specs[i].type_info.noncar.count_actors != 1) {
                BrActorFree(actor);
            } else if (pShrapnel_specs[i].type_info.noncar.actors[0] != actor) {
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

    /* based on carpocalypse2 shrapnel.c:295 */
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

// FUNCTION: CARMA2_HW 0x004ef550
void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects) {
    int i;
    int i1, i2;
    char s[256];

    /* based on carpocalypse2 shrapnel.c:246 */
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
            } else if (s[0] == '*' && (DRStricmp(s + strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + strlen(s) - 4, ".ACT") == 0)) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 2;
            } else if (DRStricmp(s, "&*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 3;
            } else if (s[0] == '&' && s[1] >= '0' && s[1] <= '9') {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 6;
            } else if (DRStricmp(s + strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 4;
            } else if (DRStricmp(s + strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + strlen(s) - 4, ".ACT") == 0) {
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

/* based on carpocalypse2 src/carma2/common/powerups.c:976 (ReadPowerupSmashable); manually expanded as the original inlines it twice */
#define READ_POWERUP_SMASHABLE(SPEC) \
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
        GetPairOfInts(pF, &d1, &d2); \
        (SPEC)->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x2 = d1; \
        (SPEC)->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x0 = d2; \
    } \
    (SPEC)->trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number; \
    (SPEC)->mode = kSmashableMode_Remove;

// FUNCTION: CARMA2_HW 0x004efa00
// FUNCTION: CARMA2_SW 0x004eb660
void C2_HOOK_FASTCALL ReadPowerupSmash(FILE* pF) {
    int i;
    int d1, d2;

    READ_POWERUP_SMASHABLE(&gPowerup_pickup_smashable)
    READ_POWERUP_SMASHABLE(&gPowerup_respawn_smashable)

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

// SmashEnvironment

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f52f0
// STUB: CARMA2_SW 0x004f0f50
float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork) {
    NOT_IMPLEMENTED();
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

// based on ApexWorm/rec2 c5b907296 and carpocalypse2 replay_workers.c:185
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004f6130
void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* local declaration instead of including 04-netgame.h here */
extern tU32 gPHIL_last_physics_tick;

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
// STUB: CARMA2_HW 0x004eff30
void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time) {
    NOT_IMPLEMENTED();
}
// STUB: CARMA2_HW 0x004c8870
void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15) {
    NOT_IMPLEMENTED();
}
// STUB: CARMA2_HW 0x004c8d40
void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
