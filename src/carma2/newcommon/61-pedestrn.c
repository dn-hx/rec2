#include "61-pedestrn.h"

#include "10-loading2.h"
#include "27-powerup.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "rec2_macros.h"
#include "globvars.h"
#include "41-utility.h"
#include "08-loading1.h"
#include "c2_string.h"
#include "70-packfile.h"
#include "52-errors.h"
#include "28-world3.h"
#include "00-car.h"
#include "23-piping.h"
#include "60-phil.h"

#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x006a0414
const char* gPedsFolder;

// GLOBAL: CARMA2_HW 0x0065d7d4
int gGoreLevel = 1;

/* explicit order keeps gPed_cache_sizes_1 directly in front of gAnimalsOn (loop-end compare in InitPolyPedSystem) */
// GLOBAL: CARMA2_HW 0x0065d778
tPedForms_vtable gPed_forms_vtable = {
    NULL,
    NULL,
    NULL,
    NULL,
    CBFillInObject,
    CBLoadForm,
    CBDisposeForm,
    CBLoadPersonality,
    (void (C2_HOOK_FASTCALL *)(undefined4*))CBDisposePersonality,
};

// UNCERTAIN: unreferenced gap
int gUNK_0065d7a0[2] = { 0 };

// GLOBAL: CARMA2_HW 0x0065d7a8
int gPed_cache_sizes_2[4] = {
    5, 10, 40, 75,
};

// GLOBAL: CARMA2_HW 0x0065d7b8
int gPed_cache_sizes_1[4] = {
    40, 25, 15, 5,
};

// GLOBAL: CARMA2_HW 0x0065d7c8
int gAnimalsOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7cc
int gFlameThrowerOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7d0
int gExplosives_on = 1;

// GLOBAL: CARMA2_HW 0x0065d7f0
int gPed_move_fsm[118][7] = {
    {   0,  -2,   5,   6, 111,  31, 112 },
    {   0,   0,   5,   6, 111,  31, 112 },
    {   0,   1,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,   2,   2,   6, 111,  31, 112 },
    {   0,   2,   2,   6, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,  10,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,  10,   5,   6, 111,  31, 112 },
    {   0,   1,   5,   6, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -2,  50,  51, 111,  31, 112 },
    {   1,  30,  31,  31, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -2,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  52,  50,  51, 111,  31, 112 },
    {   1,  52,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  72,  71,  71,  71,  71,  71 },
    {   1,  -2,  72,  72,  72,  72,  72 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -2,  74,  74,  74,  74,  74 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -2,  82,  83,  80,  80,  80 },
    {   0,  -2,  84,  85,  81,  81,  81 },
    {   0,  80,  82,  83,  80,  80,  80 },
    {   0,  80,  82,  83,  80,  80,  80 },
    {   0,  81,  84,  85,  81,  81,  81 },
    {   0,  81,  84,  85,  81,  81,  81 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  91,  90,  90,  90,  90,  90 },
    {   0,  -2,  91,  91,  91,  91,  91 },
    {   0,  91,  92,  92,  92,  92,  92 },
    {   0,  94,  93,  93,  93,  93,  93 },
    {   0,  -2,  94,  94,  94,  94,  94 },
    {   0,  -2,  95,  95,  95,  95,  95 },
    {   0,  91,  96,  96,  96,  96,  96 },
    {   0,  72,  97,  97,  97,  97,  97 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  72, 110, 110, 110, 110, 110 },
    {   1,  -2, 111, 111, 111,  31, 112 },
    {   1,  -2, 112, 112, 111,  31, 112 },
    {   0,  91, 113, 113, 113, 113, 113 },
    {   2,   2,   2,   2,   2,   2,   3 },
    {   4,   2,   2,   2,  10,  11,  13 },
    { -41, -41, -41,   0,   2,   2,   2 },
    {   2,   2,   3,   4,   4,  10,  11 },
};

// GLOBAL: CARMA2_HW 0x007447e0
float gPedestrian_speed_factor;

// GLOBAL: CARMA2_HW 0x007447ac
// GLOBAL: CARMA2_SW 0x0073b26c
int gPeds_suicidal;

// GLOBAL: CARMA2_HW 0x007447b0
int gExploding_pedestrians;

/* explicit initializers keep gPed_cache_006944c0, gNapalm_bolts and gPed_slick_material adjacent (loop-end compares in InitPedsForRace) */
// GLOBAL: CARMA2_HW 0x006944c0
tPed_cache_006944c0 gPed_cache_006944c0[100] = { 0 };

// GLOBAL: CARMA2_HW 0x0069b9f0
tNapalm_bolt gNapalm_bolts[5] = { 0 };

// UNCERTAIN: unreferenced 4-byte gap
int gUNK_0069bc0c = 0;

// GLOBAL: CARMA2_HW 0x0069bc10
br_material* gPed_slick_material = 0;

// GLOBAL: CARMA2_HW 0x007447d4
int gPed_count;

/* explicit initializers keep gPed_forms and gPed_overall_movement_disabled adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x00676928
tPed_form* gPed_forms[20] = { 0 };

// GLOBAL: CARMA2_HW 0x00676978
int gPed_overall_movement_disabled = 0;

// GLOBAL: CARMA2_HW 0x0067697c
tPedForms_vtable* gPed_vtable = 0;

// GLOBAL: CARMA2_HW 0x00676980
tPed_move* gPed_moves[250] = { 0 };

// GLOBAL: CARMA2_HW 0x00676d68
br_material* gPedestrian_character_cloned_materials[50] = { 0 };

// GLOBAL: CARMA2_HW 0x00676e30
float gPed_cos_table[256] = { 0 };

// GLOBAL: CARMA2_HW 0x00677230
float gFLOAT_00677230 = 0.f;

// GLOBAL: CARMA2_HW 0x00677238
tPed_remap* gPed_remaps[10] = { 0 };

/* explicit initializers keep gPed_personalities and gPed_sin_table adjacent (loop-end compares) */
// GLOBAL: CARMA2_HW 0x00677260
tPed_personality* gPed_personalities[50] = { 0 };

// GLOBAL: CARMA2_HW 0x00677328
float gPed_sin_table[256] = { 0 };

// GLOBAL: CARMA2_HW 0x0067772c
int gCount_pedestrian_personality_cloned_materials;

// GLOBAL: CARMA2_HW 0x00677234
int gPed_retain_root_mode;

// GLOBAL: CARMA2_HW 0x00677728
int gDAT_00677728 = 0;

/* 20 morph slots of 0x15c bytes; only the first int is cleared by InitBoner */
typedef struct {
    int field_0x0;
    char pad[0x158];
} tPed_morph_slot;

// GLOBAL: CARMA2_HW 0x00677734
tPed_morph_slot gPed_morph_slots[20] = { 0 };

// GLOBAL: CARMA2_HW 0x00679264
br_actor* gMr_blendy = 0;

// GLOBAL: CARMA2_HW 0x00744808
tPedestrian* gPedestrian_array;

// GLOBAL: CARMA2_HW 0x006a041c
int gPed_valium_left;

/* tPed_anim_seq has a flexible poses[] member: use a fixed-size twin for the initializer */
typedef struct {
    tU8 count;
    tU8 poses[2];
} tPed_anim_seq_2;

// GLOBAL: CARMA2_HW 0x0065e4e4
tPed_anim_seq_2 gCalm_ped_anim_sequence = {
    2, { 0, 99 },
};

// GLOBAL: CARMA2_HW 0x0065e4d8
const tU8 gScare_ped_anim_sequence[8] = { 4, 1, 2, 1, 99 };

// GLOBAL: CARMA2_HW 0x0065e4e0
const tU8 gPed_anim_sequence_0065e4e0[4] = { 3, 1, 2, 1 };

// GLOBAL: CARMA2_HW 0x007447f0
int gSelected_ped;

// GLOBAL: CARMA2_HW 0x00694498
tTWTVFS gTwtPeds;

// GLOBAL: CARMA2_HW 0x006a0424
br_actor* gLimbs_actor;

// GLOBAL: CARMA2_HW 0x006a0420
int gPed_count_limbed_actors;

// GLOBAL: CARMA2_HW 0x00694280
br_actor* gPed_limbed_actors[30];

// GLOBAL: CARMA2_HW 0x0058f2d8
const int gPow2_array[32] = {
           0x1,           0x2,           0x4,           0x8,
          0x10,          0x20,          0x40,          0x80,
         0x100,         0x200,         0x400,         0x800,
        0x1000,        0x2000,        0x4000,        0x8000,
       0x10000,       0x20000,       0x40000,       0x80000,
      0x100000,      0x200000,      0x400000,      0x800000,
     0x1000000,     0x2000000,     0x4000000,     0x8000000,
    0x10000000,    0x20000000,    0x40000000,    0x80000000
};

// GLOBAL: CARMA2_HW 0x00694490
int gPed_nearness;

/* gEthereal_pedestrians (0x007447a0) is defined in 66-tintedpoly.c, directly after gTintedPolys as in the original layout */

// GLOBAL: CARMA2_HW 0x007447b4
tPedestrian* gPedestrians_in_sight;

// GLOBAL: CARMA2_HW 0x0065e560
const char* gForm_non_humanoid_names[1] = {
    "NONHUMANOID",
};

// GLOBAL: CARMA2_HW 0x0069bce8
int gCount_race_pedestrian_specs;

// GLOBAL: CARMA2_HW 0x00694138
tRace_ped_spec* gRace_pedestrian_specs;

// GLOBAL: CARMA2_HW 0x00694134
int gPed_groups_count;

// GLOBAL: CARMA2_HW 0x0069bc14
tPed_group_spec* gPed_groups;

// GLOBAL: CARMA2_HW 0x0069bcec
int gPed_movements_count;

// GLOBAL: CARMA2_HW 0x0069bcf0
tPed_movement_spec* gPed_movements;

/* NOT annotated: the original 0x0058a988 is the compiler's shared 0.0f constant; a GLOBAL annotation makes
   reccmp name it and breaks every function that compares against a literal 0.0f. */
float gConst_replay_rate_zero;

// GLOBAL: CARMA2_HW 0x007447a8
int gDancing_peds;

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

// PedApplyPedDir

// SetCharacterMoveAR

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

// SetCharacterPhysicsLevelAR

// PedApplyPedPhysics

// PedApplyPedPos

// PedApplyVanishDismembered

// SetCharacterPositionAR

/* local declarations: the callees are defined further down in this file */
extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4);
extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, undefined4 pArg2, int pArg3);

// FUNCTION: CARMA2_HW 0x004cfa30
void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, int pArg4) {

    if (pCharacter->field_0x4 >= 0) {
        if (pBone_index < 0) {
            PipeSinglePedModelChange(pCharacter->ped, pBone_index, NULL, -1, GetCharacterModelSet(pCharacter), pArg4);
            SetCharacterAllBonesModel(pCharacter, pArg3, pArg4);
        } else {
            PipeSinglePedModelChange(pCharacter->ped, pBone_index, GetCharacterBoneModel(pCharacter, pBone_index), pArg3, GetCharacterModelSet(pCharacter), pArg4);
            SetCharacterBoneModel(pCharacter, pBone_index, pArg3, pArg4);
        }
    }
}

// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

// PedUndoDismember

// FUNCTION: CARMA2_HW 0x004ca9f0
void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF) {
    int i;
    int j;
    int index;
    char s[256];
    char s2[32];
    int exclusions_ok_when_scared;
    int len;

    gCount_race_pedestrian_specs = GetAnInt(pF);
    gRace_pedestrian_specs = BrMemAllocate(gCount_race_pedestrian_specs * sizeof(tRace_ped_spec), kMem_misc_poly_ped);
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        GetAString(pF, s);
        gRace_pedestrian_specs[i].spawn_material = BrMaterialFind(s);
        if (gRace_pedestrian_specs[i].spawn_material == NULL) {
            FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s);
        }
        index = GetAnInt(pF);
        if (index < 0 || index >= gPed_movements_count) {
            FatalError(kFatalError_CannotMovePedIndexOutOfRangeForMaterial_S, s);
        }
        gRace_pedestrian_specs[i].movement = &gPed_movements[index];
        index = GetAnInt(pF);
        if (index < 0 || index >= gPed_groups_count) {
            FatalError(kFatalError_CannotGroupPedIndexOutOfRangeForMaterial_S, s);
        }
        gRace_pedestrian_specs[i].group = &gPed_groups[index];
        gRace_pedestrian_specs[i].density = GetAScalar(pF);
        gRace_pedestrian_specs[i].field_0x10 = 0;
        gRace_pedestrian_specs[i].exclusion.count_exclusion_materials = GetAnInt(pF);
        gRace_pedestrian_specs[i].exclusion.exclusion_materials = BrMemAllocate(gRace_pedestrian_specs[i].exclusion.count_exclusion_materials * sizeof(tRace_spec_exclusion_material), kMem_misc_poly_ped);
        exclusions_ok_when_scared = 1;
        for (j = 0; j < gRace_pedestrian_specs[i].exclusion.count_exclusion_materials; j++) {
            gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].flags = GetAnInt(pF);
            exclusions_ok_when_scared &= gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].flags;
            GetAString(pF, s2);
            gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].material = BrMaterialFind(s2);
            if (gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].material == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
        if (exclusions_ok_when_scared & 1) {
            gRace_pedestrian_specs[i].exclusion.exclusions_ok_when_scared = 1;
        }
        gRace_pedestrian_specs[i].exclusion.count_exception_materials = GetAnInt(pF);
        gRace_pedestrian_specs[i].exclusion.exception_materials = BrMemAllocate(gRace_pedestrian_specs[i].exclusion.count_exception_materials * sizeof(br_material*), kMem_misc_poly_ped);
        for (j = 0; j < gRace_pedestrian_specs[i].exclusion.count_exception_materials; j++) {
            GetAString(pF, s2);
            gRace_pedestrian_specs[i].exclusion.exception_materials[j] = BrMaterialFind(s2);
            if (gRace_pedestrian_specs[i].exclusion.exception_materials[j] == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
    }
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        len = strlen(gRace_pedestrian_specs[i].spawn_material->identifier);
        if (len < 4) {
            len = 4;
        }
        if (len - 4 <= 7) {
            strcpy(s2, gRace_pedestrian_specs[i].spawn_material->identifier);
            strcpy(&s2[len - 4], "xxxxxxxx.MAT");
            BrResFree(gRace_pedestrian_specs[i].spawn_material->identifier);
            gRace_pedestrian_specs[i].spawn_material->identifier = BrResStrDup(gRace_pedestrian_specs[i].spawn_material, s2);
        }
        gRace_pedestrian_specs[i].spawn_material->identifier[7] = '?';
    }
    gPedestrian_array = BrMemAllocate(PEDESTRIAN_MAX_COUNT * sizeof(tPedestrian), kMem_ped_array);
}

// FUNCTION: CARMA2_HW 0x004cadb0
br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed) {

    return (br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3];
}

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

/* the helpers below are inlined into InitPolyPedSystem in the original */
/* local declarations: defined further down in this file / in other files */
extern tPed_face_cache_0x34* gPed_face_cache_heads[4];
extern br_pixelmap* gRender_palette;
extern void C2_HOOK_FASTCALL ReadExplosion(FILE* pF, tExplosion_animation* pAnimation);

// GLOBAL: CARMA2_HW 0x00694118
float gPed_process_distance;

// GLOBAL: CARMA2_HW 0x0069bc1c
float gPed_process_distance_inner;

// GLOBAL: CARMA2_HW 0x006944bc
float gPed_popup_distance;

// GLOBAL: CARMA2_HW 0x00694128
float gPed_popup_distance_inner;

// GLOBAL: CARMA2_HW 0x00694464
float gPed_cos_max_slope;

// GLOBAL: CARMA2_HW 0x0069413c
float gPed_reach_squared;

// GLOBAL: CARMA2_HW 0x00694300
float gPed_min_dist_avoid_collisions_squared;

// GLOBAL: CARMA2_HW 0x007447fc
float gPed_buoyancy_factor;

// GLOBAL: CARMA2_HW 0x0069bcf8
int gPed_points[3];

// GLOBAL: CARMA2_HW 0x00694268
int gPed_times[3];

// GLOBAL: CARMA2_HW 0x00694234
int gPed_severing_damage;

// GLOBAL: CARMA2_HW 0x00694494
float gPed_max_survivable_impact;

// GLOBAL: CARMA2_HW 0x0069bc20
float gPed_damage_per_force;

// GLOBAL: CARMA2_HW 0x00694140
tProximity_ray gProximity_rays[20] = { { 0 } };

// GLOBAL: CARMA2_HW 0x00694230
int gPed_animal_count = 0;

/* UNCERTAIN: stride 0x20 in the original (tPed_animal_name in rec2_types.h is char[50]) */
// GLOBAL: CARMA2_HW 0x0069fdb8
char gPed_animal_names[50][32];

// GLOBAL: CARMA2_HW 0x0069bc28
tExplosion_animation gNapalmed_ped_animation;

// GLOBAL: CARMA2_HW 0x0069bc54
tPed_face_cache_0x50* gPed_face_cache;

// GLOBAL: CARMA2_HW 0x00694458
br_pixelmap* gProx_ray_shade_table;

// GLOBAL: CARMA2_HW 0x0069411c
float gGiblet_scrape_start;

// GLOBAL: CARMA2_HW 0x00694238
tSlick_spec gPed_slick;

// GLOBAL: CARMA2_HW 0x00694274
tPed_giblet_size_spec* gGiblet_sizes;

// GLOBAL: CARMA2_HW 0x00694278
br_model* gPed_spurt_model;

// GLOBAL: CARMA2_HW 0x0069427c
int gPed_spurty_lumps_count;

// GLOBAL: CARMA2_HW 0x006942f8
int gPed_spurt_min_max_time[2];

// GLOBAL: CARMA2_HW 0x00694454
int gPed_dismember_spurting_member_count;

// GLOBAL: CARMA2_HW 0x0069445c
int gPed_spurting_period;

// GLOBAL: CARMA2_HW 0x00694460
br_material* gPed_smear_material;

// GLOBAL: CARMA2_HW 0x00694468
tSmashable_initial_position_spec gGiblet_initial_position;

// GLOBAL: CARMA2_HW 0x00694478
tExplosion_animation gPed_explosion_small_blood_cloud;

// GLOBAL: CARMA2_HW 0x00694480
tExplosion_animation gPed_medium_blood_cloud;

// GLOBAL: CARMA2_HW 0x00694488
tExplosion_animation gPed_large_blood_cloud;

// GLOBAL: CARMA2_HW 0x006944a0
tSmashable_initial_speed_spec gPed_spurt_initial_speed;

// GLOBAL: CARMA2_HW 0x0069bc18
int gGiblet_size_count;

// GLOBAL: CARMA2_HW 0x0069bc38
tSmashable_initial_speed_spec gGiblet_initial_speed;

// GLOBAL: CARMA2_HW 0x0069fda8
tSmashable_initial_position_spec gPed_spurt_initial_position;

// GLOBAL: CARMA2_HW 0x006a0408
int gGiblet_min_max_time[2];

extern void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec* pInitial_pos);
extern void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed);
extern void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes);
extern void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick);
extern void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00403ed0
void C2_HOOK_FASTCALL InitBoner(tPedForms_vtable* pTable) {
    int i;

    gPed_vtable = pTable;
    memset(gPed_forms, 0, sizeof(gPed_forms));
    memset(gPed_personalities, 0, sizeof(gPed_personalities));
    memset(gPed_moves, 0, sizeof(gPed_moves));
    memset(gPed_remaps, 0, sizeof(gPed_remaps));

    for (i = 0; i < 20; i++) {
        gPed_morph_slots[i].field_0x0 = 0;
    }

    for (i = 0; i < 256; i++) {
        float a = (i * 256) * 2 * 3.14159265358979323846 / 256. / 256.;
        gPed_sin_table[i] = (float)sin(a);
        gPed_cos_table[i] = (float)cos(a);
    }

    gDAT_00677728 = 0;
    gFLOAT_00677230 = 1.f;
    gCount_pedestrian_personality_cloned_materials = 0;
    gPed_overall_movement_disabled = 0;
    gPed_retain_root_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004cb1e0
void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pF) {
    tPath_name the_path;
    float scale;
    int index;
    int r, g, b;
    char s[256];
    tPed_giblet_size_spec* spec;
    int j;
    int i;

    PathCat(the_path, gApplication_path, gPedsFolder);
    PathCat(the_path, the_path, "GIBLETS");
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Diffuse1);

    gGiblet_scrape_start = (float)GetAnInt(pF);
    ReadSmashableInitialPosition(pF, &gGiblet_initial_position);
    ReadSmashableInitialSpeed(pF, &gGiblet_initial_speed);
    ReadMinMaxTimeInMilliseconds(pF, gGiblet_min_max_time);

    gGiblet_size_count = GetAnInt(pF);
    gGiblet_sizes = BrMemAllocate(gGiblet_size_count * sizeof(tPed_giblet_size_spec), kMem_misc_poly_ped);

    for (i = 0, spec = gGiblet_sizes; i < gGiblet_size_count; i++, spec++) {
        spec->flags = GetAnInt(pF);
        spec->min_force = GetAScalar(pF);
        spec->instance_force = GetAScalar(pF);
        spec->max_count = GetAnInt(pF);
        spec->model_count = GetAnInt(pF);

        for (j = 0; j < spec->model_count; j++) {
            scale = GetAScalar(pF);
            index = GetAnInt(pF);
            GetThreeInts(pF, &r, &g, &b);
            GetAString(pF, s);
            spec->models[j] = BrModelFind(s);
            if (spec->models[j] == NULL) {
                FatalError(kFatalError_CannotFindGibletModel_S, s);
            }
            if (i == gGiblet_size_count - 1) {
                scale = scale * 3.f * .25f;
            }
            ScaleModel(spec->models[j], 0, scale);
            spec->models[j]->faces[0].material->index_base = index;
            spec->models[j]->faces[0].material->index_range = 1;
            spec->models[j]->faces[0].material->colour = BR_COLOUR_RGB(r, g, b);
            BrMaterialUpdate(spec->models[j]->faces[0].material, BR_MATU_ALL);
        }
    }

    ReadExplosion(pF, &gPed_explosion_small_blood_cloud);
    ReadExplosion(pF, &gPed_medium_blood_cloud);
    ReadExplosion(pF, &gPed_large_blood_cloud);

    gPed_slick_material = BrMaterialAllocate("GIBSLICK");
    BrMaterialAdd(gPed_slick_material);
    gPed_slick_material->flags |= BR_MATF_LIGHT;
    gPed_slick_material->flags |= BR_MATF_PERSPECTIVE;
    gPed_slick_material->flags |= BR_MATF_SMOOTH;
    gPed_slick_material->ka = .99f;
    gPed_slick_material->kd = .0f;
    gPed_slick_material->ks = .0f;
    gPed_slick_material->power = .0f;
    gPed_slick_material->index_base = 0;
    gPed_slick_material->index_range = 0;
    BrMatrix23Identity(&gPed_slick_material->map_transform);
    gPed_slick_material->index_shade = BrTableFind("IDENTITY.TAB");
    gPed_slick_material->colour_map = BrMapFind("GIBSLICK");
    GlorifyMaterial(&gPed_slick_material, 1, kRendererShadingType_Diffuse2);
    BrMaterialUpdate(gPed_slick_material, BR_MATU_ALL);

    gPed_smear_material = BrMaterialFind("PEDSMEAR");

    ReadSpillData(pF, &gPed_slick);

    gPed_spurty_lumps_count = GetAnInt(pF);
    gPed_spurting_period = GetAnInt(pF);
    gPed_dismember_spurting_member_count = GetAnInt(pF);
    ReadSmashableInitialPosition(pF, &gPed_spurt_initial_position);
    ReadSmashableInitialSpeed(pF, &gPed_spurt_initial_speed);
    ReadMinMaxTimeInMilliseconds(pF, gPed_spurt_min_max_time);

    scale = GetAScalar(pF);
    index = GetAnInt(pF);
    GetThreeInts(pF, &r, &g, &b);
    GetAString(pF, s);
    gPed_spurt_model = BrModelFind(s);
    if (gPed_spurt_model == NULL) {
        FatalError(kFatalError_CannotFindGibletModel_S, s);
    }
    ScaleModel(gPed_spurt_model, 0, scale);
    gPed_spurt_model->faces[0].material->index_base = index;
    gPed_spurt_model->faces[0].material->index_range = 1;
    gPed_spurt_model->faces[0].material->colour = BR_COLOUR_RGB(r, g, b);
    BrMaterialUpdate(gPed_spurt_model->faces[0].material, BR_MATU_ALL);
}

// GLOBAL: CARMA2_HW 0x0065e598
const char* gBurning_ped_map_names[7] = {
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
};

// GLOBAL: CARMA2_HW 0x00694308
br_model* gBurning_ped_models[7];

/* explicit initializers keep the loop-end compare address (materials + 7) on gUNK_0069fda4 */
// GLOBAL: CARMA2_HW 0x0069fd88
br_material* gBurning_ped_materials[7] = { 0 };

// UNCERTAIN: unreferenced 4-byte gap before gPed_spurt_initial_position (0x69fda8)
// GLOBAL: CARMA2_HW 0x0069fda4
int gUNK_0069fda4 = 0;

// FUNCTION: CARMA2_HW 0x004cb630
void C2_HOOK_FASTCALL InitNapalmNolts(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gBurning_ped_materials); i++) {
        gBurning_ped_models[i] = BrModelAllocate(NULL, 4, 2);
        gBurning_ped_models[i]->faces[0].vertices[0] = 0;
        gBurning_ped_models[i]->faces[0].vertices[1] = 2;
        gBurning_ped_models[i]->faces[0].vertices[2] = 1;
        gBurning_ped_models[i]->faces[0].material = NULL;
        gBurning_ped_models[i]->faces[0].smoothing = 1;
        gBurning_ped_models[i]->faces[1].vertices[0] = 0;
        gBurning_ped_models[i]->faces[1].vertices[1] = 3;
        gBurning_ped_models[i]->faces[1].vertices[2] = 2;
        gBurning_ped_models[i]->faces[1].material = NULL;
        gBurning_ped_models[i]->faces[1].smoothing = 1;

        BrVector3Set(&gBurning_ped_models[i]->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&gBurning_ped_models[i]->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&gBurning_ped_models[i]->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&gBurning_ped_models[i]->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[3].map, 1.f, 1.f);

        BrModelAdd(gBurning_ped_models[i]);

        gBurning_ped_materials[i] = BrMaterialAllocate("BURN!");
        gBurning_ped_materials[i]->flags &= ~BR_MATF_LIGHT;
        gBurning_ped_materials[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gBurning_ped_materials[i]->colour_map = BrMapFind(gBurning_ped_map_names[i]);
        if (gBurning_ped_materials[i]->colour_map == NULL) {
            FatalError(kFatalError_CantFindPedTexture_S, gBurning_ped_map_names[i]);
        }
        GlorifyMaterial(&gBurning_ped_materials[i], 1, kRendererShadingType_Diffuse2);
        BrMaterialAdd(gBurning_ped_materials[i]);
    }

    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gBurning_ped_materials); j++) {
            gNapalm_bolts[i].actors[j] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gNapalm_bolts[i].actors[j]->material = gBurning_ped_materials[j];
            gNapalm_bolts[i].actors[j]->model = gBurning_ped_models[j];
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
            BrActorAdd(gNon_track_actor, gNapalm_bolts[i].actors[j]);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static void C2_HOOK_FASTCALL ReadSettingsFile(void) {
    tPath_name the_path;
    FILE* file;
    int i;
    int j;
    float v;

    PathCat(the_path, gApplication_path, gPedsFolder);
    PathCat(the_path, the_path, "SETTINGS.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, "SETTINGS.TXT");
    }
    gPed_process_distance = GetAScalar(file);
    gPed_process_distance_inner = 0.6 * gPed_process_distance;
    gPed_popup_distance = GetAScalar(file);
    gPed_popup_distance_inner = 0.6 * gPed_popup_distance;
    gPed_cos_max_slope = BR_COS(BR_ANGLE_DEG(GetAScalar(file)));

    v = GetAScalar(file);
    gPed_reach_squared = v * v;

    v = GetAScalar(file);
    gPed_min_dist_avoid_collisions_squared = v * v;
    gPed_buoyancy_factor = GetAScalar(file);
    GetThreeInts(file, gPed_points, &gPed_points[1], &gPed_points[2]);
    GetThreeInts(file, &gPed_times[0], &gPed_times[1], &gPed_times[2]);
    gPed_severing_damage = GetAnInt(file);
    gUNK_0069bc0c = GetAnInt(file);
    gPed_max_survivable_impact = GetAScalar(file);
    gPed_damage_per_force = GetAScalar(file);
    ReadPedGiblets(file);

    ReadExplosion(file, &gNapalmed_ped_animation);

    gPed_movements_count = GetAnInt(file);
    gPed_movements = BrMemAllocate(gPed_movements_count * sizeof(tPed_movement_spec), kMem_misc_poly_ped);

    for (i = 0; i < gPed_movements_count; i++) {
        gPed_movements[i].min_time_between = (int)(1000.f * GetAScalar(file));
        gPed_movements[i].max_time_between = (int)(1000.f * GetAScalar(file));
        gPed_movements[i].max_random_angle = BR_ANGLE_DEG(GetAScalar(file));
        gPed_movements[i].min_walk_speed_factor = 1000. / (GetAScalar(file) * 30.);
        gPed_movements[i].max_walk_speed_factor = 1000. / (GetAScalar(file) * 30.);
    }

    gPed_groups_count = GetAnInt(file);
    gPed_groups = BrMemAllocate(gPed_groups_count * sizeof(tPed_group_spec), kMem_misc_poly_ped);

    for (i = 0; i < gPed_groups_count; i++) {
        gPed_groups[i].count = GetAnInt(file);
        gPed_groups[i].members = BrMemAllocate(gPed_groups[i].count * sizeof(tPed_group_member_spec), kMem_misc_poly_ped);

        for (j = 0; j < gPed_groups[i].count; j++) {
            GetAString(file, gPed_groups[i].members[j].name);
        }
    }

    gPed_animal_count = GetAnInt(file);
    for (i = 0; i < gPed_animal_count; i++) {
        GetAString(file, gPed_animal_names[i]);
    }

    PFfclose(file);
}

static void C2_HOOK_FASTCALL InitFaceCaches(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
        gPed_face_cache_heads[i] = BrMemAllocate(gPed_cache_sizes_1[i] * sizeof(tPed_face_cache_0x34), kMem_ped_face_cache);
        for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
            gPed_face_cache_heads[i][j].field_0x0 = BrMemAllocate(gPed_cache_sizes_2[i] * sizeof(tPed_face_cache_0x50), kMem_ped_face_cache);
            gPed_face_cache_heads[i][j].count = gPed_cache_sizes_2[i];
        }
    }
    gPed_face_cache = BrMemAllocate((gPed_cache_sizes_2[3] + 1) * sizeof(tPed_face_cache_0x50), kMem_ped_face_cache);
}

static void C2_HOOK_FASTCALL InitOtherPedStuff(void) {

    gSelected_ped = 0;
    gProx_ray_shade_table = GenerateShadeTable(8, gRender_palette, 0xd7, 0xff, 0xe9, .5f, .75f, .9f);
}

// FUNCTION: CARMA2_HW 0x004cadc0
void C2_HOOK_FASTCALL InitPolyPedSystem(void) {

    if (gPedsFolder == NULL) {
        SetDefaultPedFolderNames();
    }

    InitBoner(&gPed_forms_vtable);
    ReadSettingsFile();
    InitFaceCaches();
    InitNapalmNolts();
    InitOtherPedStuff();
}

// DisposePedSpawnSpec

// DisposeCharacterInstances

// FUNCTION: CARMA2_HW 0x004cb8d0
void C2_HOOK_FASTCALL DisposePedStuff(void) {
    int i;

    DisposeCharacterInstances();

    for (i = 0; i < gPed_count; i++) {
        DisposeCharacterInstance(gPedestrian_array[i].character);
    }
    ClearOutStorageSpace(&gPedStorage);
    if (gPedestrian_array != NULL) {
        BrMemFree(gPedestrian_array);
    }
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        if (gRace_pedestrian_specs[i].exclusion.exclusion_materials != NULL) {
            BrMemFree(gRace_pedestrian_specs[i].exclusion.exclusion_materials);
        }
        if (gRace_pedestrian_specs[i].exclusion.exception_materials != NULL) {
            BrMemFree(gRace_pedestrian_specs[i].exclusion.exception_materials);
        }
    }
    if (gRace_pedestrian_specs != NULL) {
        BrMemFree(gRace_pedestrian_specs);
    }
}

// FUNCTION: CARMA2_HW 0x004cb980
FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "FORMS");
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}
// FUNCTION: CARMA2_HW 0x004cba10
FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}
// FUNCTION: CARMA2_HW 0x004cbaa0
FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    gTwtPeds = OpenPackFileAndSetTiffLoading(path);
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}
// FUNCTION: CARMA2_HW 0x004cbb50
FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    return DRfopen(path, "rb");
}
// FUNCTION: CARMA2_HW 0x004cbbb0
FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "REMAPS");
    PathCat(path, path, pFile_name);
    return DRfopen(path, "rb");
}

// FUNCTION: CARMA2_HW 0x004cbc10
void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    DisallowDuplicates();
    LoadAllStuffInDirectory(&gPedStorage, path, kRendererShadingType_Diffuse1);
    AllowDuplicates();
}

// kept out of line: MakeCharacterRenderable2 0x408200 calls it (original has a real call)
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004cbc70
void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pIndex, tPed_character_instance* pCharacter) {

    if (pActor->parent == NULL) {
        if (pIndex == 0 || (pCharacter->field_0xc & gPow2_array[pIndex]) != 0) {
            BrActorAdd(gNon_track_actor, pActor);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// kept out of line: DisposePedForm 0x4067e0 calls both (original has real calls)
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004cbca0
void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor) {

    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004cbcc0
int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor) {
    if (pActor->parent != NULL) {
        return 1;
    } else {
        return 0;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SetPedXZDirection

// StopPedNoise

extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);
extern void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13);

// SetPedMove: (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004cbcd0
void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pAction) {
    int original_action;
    int original_move_id;
    int original_field_0x1e;
    int original_field_0x1c;
    br_vector3 original_pos;
    br_vector3* new_pos;
    int new_move_lut_id;
    int i;
    int move_result;
    float speed;
    char buffer[256];

    original_action = pPed->action;
    i = 0;
    original_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
    original_field_0x1e = (tS16)pPed->character->field_0x1e;
    original_field_0x1c = (tS16)pPed->character->field_0x1c;
    new_pos = (br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3];
    original_pos.v[0] = new_pos->v[0];
    original_pos.v[1] = new_pos->v[1];
    original_pos.v[2] = new_pos->v[2];
    if (pAction >= 0 && pMove_action >= 0 && pPed->action != ePed_action_dead) {
        pPed->action = pAction;
    }
    if (gDrunk_pedestrians) {
        new_move_lut_id = 4;
    } else if (gDancing_peds) {
        new_move_lut_id = 3;
    } else if (gPanicking_peds) {
        new_move_lut_id = 5;
    } else if (pPed->character->field_0xc & gPow2_array[3]) {
        new_move_lut_id = 1;
    } else if (pPed->character->field_0xc & gPow2_array[5]) {
        new_move_lut_id = 2;
    } else {
        new_move_lut_id = 0;
    }
    if (new_move_lut_id != 0) {
        pMove_action = gPed_move_fsm[pMove_action][new_move_lut_id + 1];
    }
    if (pMove_action == 112 || pMove_action == 31 || pMove_action == 111) {
        pPed->action = ePed_action_running;
    }
    if (pPed->field_0x0c != NULL && pMove_action != 112 && pMove_action != 31
            && pMove_action != 40 && pMove_action != 41 && pMove_action != 42) {
        pPed->field_0x0c->field_0x80 = 0;
    }
    for (; i < 10; i++) {
        if (gPedestrian_speed_factor != 0.f) {
            speed = (float)pWalk_speed_factor / gPedestrian_speed_factor;
        } else {
            speed = (float)pWalk_speed_factor;
        }
        move_result = SetCharacterMove(pPed->character, pMove_action, speed, pArg4, pArg5, pArg6);
        if (move_result == 2) {
            pMove_action = gPed_move_fsm[pMove_action][0 + 1];
            if (pMove_action <= -2) {
                sprintf(buffer, "%d", pMove_action);
                FatalError(pMove_action != -2 ? kFatalError_BadMoveAttemptedFor_SS : kFatalError_CoreMoveMissingFor_SS, buffer, pPed->character->personality->name);
            }
        }
        if (pMove_action == -1 || move_result != 2) {
            break;
        }
    }
    if (i >= 10) {
        sprintf(buffer, "%d", pMove_action);
        FatalError(kFatalError_CyclicMoveDefinitionFor_SS, buffer, pPed->character->personality->name);
    }
    PipeSinglePedMove(pPed, original_field_0x1c, original_move_id,
        pMove_action, (float)original_field_0x1e, (float)pWalk_speed_factor, pArg5, pArg4,
        &original_pos, new_pos, original_action, pPed->action, &pPed->character->field_0x8c);
}

// FUNCTION: CARMA2_HW 0x004cc660
void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3) {

    if (!(pPed->character->field_0xc & gPow2_array[3]) && !(pPed->character->field_0xc & gPow2_array[5]) && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x80 = 100;
        pPed->field_0x0c->field_0xb8 = .09f;
        SetPedMove(pPed, pArg3 ? 40 : 41, pPed->field_0x0c->field_0x80, 0, 0, pTime, ePed_action_running);
    } else {
        SetPedMove(pPed, pArg3 ? 40 : 41, -1, 0, 0, pTime, ePed_action_running);
    }
}

// AcceleratePed

// MungePedHeadAnim

/* SetPedHeadAnim: inlined into its callers in the original */
static void C2_HOOK_FASTCALL SetPedHeadAnim(tPedestrian* pPed, const tPed_anim_seq* pSeq, tU32 pTime) {

    if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x1c = pSeq;
        pPed->field_0x0c->field_0x78 = pTime;
        pPed->field_0x0c->field_0x1a = 0;
        SetCharacterBoneModelAR(pPed->character, pPed->character->personality->form->index_head_bone, pPed->field_0x0c->field_0x1c->poses[0], 0);
    }
}

// ClearPedHeadAnim

// ScarePedestrian

// PedAnimCausesMovement

// StopSmoothTurning

// SetKnockedOverMove

// DoPostElectricution

// CBMoveCompleted

// FUNCTION: CARMA2_HW 0x004cc900
void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, non_humanoid, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, index_head_bone, 0x4d);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_A, 0x4e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_B, 0x4f);

    /* Other stuff */

    pPed_form->non_humanoid = GetALineAndInterpretCommand(pF, gForm_non_humanoid_names, REC2_ASIZE(gForm_non_humanoid_names)) == 0;

    /* Index of head bone */
    pPed_form->index_head_bone = GetAnInt(pF);

    /* Direction axis when lying down (A) */
    pPed_form->axis_when_lying_down_A = GetAnInt(pF);

    /* Direction axis when lying down (B) */
    pPed_form->axis_when_lying_down_B = GetAnInt(pF);
}

// FUNCTION: CARMA2_HW 0x004cc940
void C2_HOOK_FASTCALL CBDisposeForm(tPed_form* pForm) {
}

// FUNCTION: CARMA2_HW 0x004cc950
void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF) {
    char s[256];
    tPath_name path;
    FILE* sound_f;
    int i;
    tPed_personality_sounds* sounds;

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON(sizeof(tPed_personality_sounds) != 0x84);
    C2_HOOK_BUG_ON(REC2_ASIZE(((tPed_personality_sounds*)NULL)->sounds) != 11);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, sounds, 0x64)
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, jump_height, 0x50)
#endif

    /* Name of sound definition file */
    GetAString(pF, s);

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "SOUND");
    PathCat(path, path, s);
    sound_f = DRfopen(path, "rt");
    if (sound_f == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, s);
    }

    sounds = BrMemAllocate(sizeof(tPed_personality_sounds), kMem_misc_poly_ped);
    pPersonality->sounds = sounds;

    for (i = 0; i < (int)REC2_ASIZE(sounds->sounds); i++) {
        int j;

        sounds->sounds[i].count_sounds = GetAnInt(sound_f);
        for (j = 0; j < sounds->sounds[i].count_sounds; j++) {
            sounds->sounds[i].sounds[j] = LoadSingleSound(&gPedStorage, GetAnInt(sound_f));
        }
    }
    PFfclose(sound_f);

    pPersonality->jump_height = 0.f;
    if (!PFfeof(pF)) {
        GetAString(pF, s);
        if (DRStricmp(s,"JUMPHEIGHT") == 0) {
            pPersonality->jump_height = GetAScalar(pF);
        }
    }
    ClosePackFileAndSetTiffLoading(gTwtPeds);
}


/* placed here (before 0x004ccaa0): the TU context decides the x87 schedule of the inlined
   dot product. With the ped bodies now at the end of this TU, this position is the one that
   reproduces the original's point-resident schedule (92.50%); after 0x40b570 it drops to 71.79. */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040b030
void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest) {
    float dot;

    dot = BrVector3Dot(pPlane, pPoint);
    BrVector3Scale(pDest, pPlane, -dot);
    BrVector3Accumulate(pDest, pPoint);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ccaa0
// FUNCTION: CARMA2_SW 0x004c91a0
void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, sounds, 0x64);

    BrMemFree(pPersonality->sounds);
}

// FUNCTION: CARMA2_HW 0x004ccab0
void C2_HOOK_FASTCALL CBFillInObject(undefined4* pArg1, undefined4 pArg2) {
    char* base = (char*)pArg1;
    if (pArg2 == 5) {
        *(int*)(base + 0x1a0) = 0x40000;
        *(int*)(base + 0x1a4) = 0x50000;
    } else {
        *(int*)(base + 0x1a0) = 0x10000;
        *(int*)(base + 0x1a4) = 0x50000;
    }
}

// DoGibShower

// PedApplyGibShower

// PedUndoGibShower

// DoGiblets

extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority);
extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8);
extern void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9);

// DoSpurt

// PedApplyBloodSpurt

// PedUndoBloodSpurt

// OneLessPed

// ScoreForKilledPedestrian

// KillPedestrian

// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

// FUNCTION: CARMA2_HW 0x004ce030
int C2_HOOK_FASTCALL FindPedestrianNearCar(tCar_spec* pCar, br_scalar pRadius) {

    return DoToPeds(pCar, 1, pRadius, 1, NULL, (tForEachPedestrian_cbfn*)DoPedAnnihilator);
}

// FUNCTION: CARMA2_HW 0x004ce140
int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback) {
    tPedestrian* ped;
    tU32 now;
    br_vector3* pos;
    br_scalar dist_sq;
    float t;
    int result = 0;

    if (gEthereal_pedestrians) {
        return 0;
    }
    pos = &pCar_spec->pos;
    now = GetTotalTime();

    for (ped = gPedestrians_in_sight; ped != NULL; ped = ped->next) {
        dist_sq = (pos->v[0] - ped->pos.v[0]) * (pos->v[0] - ped->pos.v[0])
                + (pos->v[1] - ped->pos.v[1]) * (pos->v[1] - ped->pos.v[1])
                + (pos->v[2] - ped->pos.v[2]) * (pos->v[2] - ped->pos.v[2]);

        if (pOnly_alive != 0 && ped->hit_points <= 0) {
            continue;
        }
        if (pMax_distance != 0.f && dist_sq >= pMax_distance) {
            continue;
        }
        if (pParam_4 != 0) {
            br_material* material;
            br_vector3 nor;
            br_vector3 dir;

            dir.v[0] = ped->pos.v[0] - pos->v[0];
            dir.v[1] = ped->pos.v[1] - pos->v[1];
            dir.v[2] = ped->pos.v[2] - pos->v[2];
            FindFace(pos, &dir, &nor, &t, &material);
        } else {
            t = 100.f;
        }
        if (t < 1.0) {
            continue;
        }
        pCallback(ped, pCar_spec, dist_sq, now, pData);
        result = 1;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004ce050
int C2_HOOK_FASTCALL DoPedAnnihilator(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pArg4, tPedestrian_distances* pDistances) {
    extern void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor);
    extern void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info);
    extern tS3_outlet* gCar_outlet;
    int i;

    pPed->field_0x0c->field_0xc0 = pCar;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        PipeSinglePedIncident(pPed, pCar->collision_info->actor);
    }
    if (!gImmortal_peds) {
        KillPedestrian(pPed, pCar->collision_info);
        SetPedMove(pPed, 0x71, -1, 0, 1, pArg4, ePed_action_dead);
    }
    i = 0;
    while (gProximity_rays[i].start_time != 0) {
        i++;
        /* original compares the walk pointer with the address just past the table */
        if ((tU32*)&gProximity_rays[i].start_time >= (tU32*)&gPed_slick) {
            return 0;
        }
    }
    gProximity_rays[i].start_time = GetTotalTime();
    gProximity_rays[i].car = pCar;
    gProximity_rays[i].ped = pPed;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound(gCar_outlet, 0x157c);
        return 0;
    }
    DRS3StartSound3D(gCar_outlet, 0x157c, &pCar->pos, &gZero_v__car, 1, 0xff, 0x10000, 0x10000);
    return 0;
}

extern tS3_outlet* gCar_outlet;

// FUNCTION: CARMA2_HW 0x004ce280
int C2_HOOK_FASTCALL DoPedestrianRepulsifictor(tCar_spec* pCar, float pF0, float pF1, float pF2, float pF3) {
    struct {
        float f0;
        float f1;
        float f2;
        float f3;
    } data;
    int res;

    data.f0 = pF0;
    data.f1 = pF1;
    data.f2 = pF2;
    data.f3 = pF3;
    res = DoToPeds(pCar, 1, pF3, 1, &data, (tForEachPedestrian_cbfn*)RepulseIt);
    if (res != 0) {
        if (pCar != NULL && pCar->driver == eDriver_local_human) {
            DRS3StartSound(gCar_outlet, 0x1324);
            return 1;
        }
        DRS3StartSound3D(gCar_outlet, 0x1324, &pCar->pos, &gZero_v__car, 1, 0xff, 0x10000, 0x10000);
        return 1;
    }
    return 0;
}

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

// RescanPedProximity

// RecacheAndSetFace

// PedScanForObjects

// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

// ActiveHalted

// CBActiveHalted

// RecordPedSlide

// PedProcessForces

/* CBMovedByPhysics (0x004d2930): body at the TU end; a body at this slot broke the canaries. */

// FUNCTION: CARMA2_HW 0x004d2c40
int C2_HOOK_FASTCALL GetPedCount(void) {
    return gPed_count;
}
// GetPedPosition

// FUNCTION: CARMA2_HW 0x004d34e0
void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    if (gPed_count_limbed_actors < 30) {
        gPed_limbed_actors[gPed_count_limbed_actors] = actor;
        gPed_count_limbed_actors += 1;
    }
    BrZsModelRender(actor, model, material, (br_order_table*)render_data, style, on_screen, 0);
}

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

// FinishUpLoadingPeds

// FUNCTION: CARMA2_HW 0x004d35d0
void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void) {

}

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

// SetThisPedPhysicing

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

// FUNCTION: CARMA2_HW 0x004d3610
void C2_HOOK_FASTCALL RenderLimbs(void) {
    int i;
    int j;

    if (gLimbs_actor == NULL) {
        gLimbs_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        gLimbs_actor->identifier = "Limbs_actor";
        BrActorAdd(gUniverse_actor, gLimbs_actor);
    }
    for (i = 0; i < gPed_count_limbed_actors; i++) {
        br_actor* a = gPed_limbed_actors[i];
        tPed_character_instance* ped = a->type_data;
        tPed_form* form = ped->personality->form;
        br_actor** bone_actors = &form->actor_sets[ped->field_0x4].actors[1];
        for (j = 1; j < form->count_bones; j++, bone_actors++) {
            if (!(gPow2_array[j] & ped->field_0xc)) {
                BrActorAdd(gLimbs_actor, *bone_actors);
            }
        }
    }
    BrZbsSceneRenderAdd(gLimbs_actor);
    while (gLimbs_actor->children != NULL) {
        BrActorRemove(gLimbs_actor->children);
    }
    gPed_count_limbed_actors = 0;
}
// GLOBAL: CARMA2_HW 0x007447ec
int gCount_active_peds;

// GLOBAL: CARMA2_HW 0x0069bce0
int gPeds_already_munged;

// GLOBAL: CARMA2_HW 0x00694130
tU32 gPed_last_munging;

// GLOBAL: CARMA2_HW 0x006a042c
tU32 gLast_scare_time;

// GLOBAL: CARMA2_HW 0x00694328
tPed_cache_006944c0 gPed_cache_00694328;

extern int gBOOL_00744804;
extern int gCount_changed_points;
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tS8 pArg7, tS8 pArg8, tS8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12);
extern void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, undefined4 pArg2, undefined4 pArg3);
/* temporary placeholder, address not identified yet */
int C2_HOOK_FASTCALL PedFallingForever(tPedestrian* pPed) {
    NOT_IMPLEMENTED();
    return 0;
}
/* temporary placeholder, address not identified yet */
int C2_HOOK_FASTCALL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, int pLevel) {
    NOT_IMPLEMENTED();
    return 0;
}
extern int C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);
extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);
extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);
extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);
extern void C2_HOOK_FASTCALL AwardTime(tU32 pTime);
extern tU32 gPed_last_score_time;
extern tPedestrian* gPed_recent_ped;
extern int gPed_score_multiplier;
extern int gPed_recent_points;
extern void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject);
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL MakePedVanish(tPedestrian* pPed) {
    NOT_IMPLEMENTED();
}
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL KillNapalmBolt(tNapalm_bolt* pBolt) {
    NOT_IMPLEMENTED();
}
/* no standalone symbol in the original: inlined at the only call site (MungePedestrians);
   runtime-faithful body at the TU end */
extern void C2_HOOK_FASTCALL InitProcessData(tPedestrian* pPed, tU32 pTime);
extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object *pCollision_info, int pParam);
/* not inlined: DoPedAnnihilator must keep its call 0x004cce70 */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#define GET_PED_COLLISION_OBJECT(PED) ( ((PED)->character->field_0x14 & 1) ? (PED)->character->personality->form->simple_physicing[(PED)->character->field_0x5].collision_info : GetRootObject((PED)->character))
extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter);
extern void C2_HOOK_FASTCALL DoGibShower(volatile int pId, volatile int pTime, int pSeed, float pForce, int pSize_index, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pDelta);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern int gCount_killed_peds;
extern int gTotal_count_smash_peds;

// GLOBAL: CARMA2_HW 0x0065d79c
int gGib_shower_id = -1;

// FUNCTION: CARMA2_HW 0x004ccff0
void C2_HOOK_FASTCALL DoGiblets(tPedestrian* pPed, tPhysics_object* pObject, tPhysics_object* pOpt_speed_object, float pArg4, br_vector3* pArg5, int pArg6) {
    tU32 time;
    int seed;
    br_vector3 vec;

    if (gGoreLevel <= 0) {
        return;
    }
    time = GetTotalTime();
    if (pArg4 > 0.5) {
        Explode(&gPed_large_blood_cloud, GetCharacterActorPtr(pPed->character), &pPed->character->personality->bb, &pObject->cmpos);
    } else if (pArg4 > 0.25) {
        Explode(&gPed_medium_blood_cloud, GetCharacterActorPtr(pPed->character), &pPed->character->personality->bb, &pObject->cmpos);
    } else if (pArg4 > 0.05) {
        Explode(&gPed_explosion_small_blood_cloud, GetCharacterActorPtr(pPed->character), &pPed->character->personality->bb, &pObject->cmpos);
    }
    seed = (int)pArg5->v[2] ^ (int)pArg5->v[1] ^ time ^ (int)pOpt_speed_object ^ (int)pPed ^ (int)pArg5->v[0];
    if (pOpt_speed_object == NULL) {
        pOpt_speed_object = pObject;
    }
    vec.v[0] = pOpt_speed_object->v.v[0] * 6.9;
    vec.v[1] = pOpt_speed_object->v.v[1] * 6.9;
    vec.v[2] = pOpt_speed_object->v.v[2] * 6.9;
    DoGibShower(gGib_shower_id, time, seed, pArg4, pArg6, &pPed->pos, pArg5, &vec);
    PipeSingleGibShower(gGib_shower_id, time, seed, *(int*)&pArg4, pArg6, &pPed->pos, pArg5, &vec);
    gGib_shower_id--;
}

// FUNCTION: CARMA2_HW 0x004cd160
void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed) {

    if ((tS16)pPed->flags & 0x100) {
        return;
    }
    pPed->flags |= 0x100;
    if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
        gCount_killed_peds++;
        if (gCount_killed_peds >= gPed_count) {
            NewTextHeadupSlot(4, 0, 0x1388, -4, GetMiscString(0x1a));
            RaceCompleted(eRace_over_1);
            return;
        }
    }
    if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
        if ((tS16)pPed->flags & 0x200) {
            gCount_killed_peds++;
            if (gCount_killed_peds >= gTotal_count_smash_peds) {
                RaceCompleted(eRace_over_1);
                return;
            }
        }
    }
    if (gCurrent_race.race_spec->race_type == kRaceType_SmashNPed) {
        if ((tS16)pPed->flags & 0x200) {
            AccumulateRuntimeVariable(gCurrent_race.race_spec->options.smash_and_peds.var_smash_number, 1);
            if (GetRuntimeVariable(gCurrent_race.race_spec->options.smash_and_peds.var_smash_number) >= gCurrent_race.race_spec->options.smash_and_peds.var_smash_target) {
                RaceCompleted(eRace_over_3);
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL DoPostElectricution(tPedestrian* pPed, tU32 pTime, float pF1, float pF2) {
    NOT_IMPLEMENTED();
}
/* AcceleratePed: no standalone symbol in the original (inlined into MungePedestrians);
   runtime-faithful body at the TU end. A body here flips DisposePedPersonality 0x4077f0. */
extern void C2_HOOK_FASTCALL AcceleratePed(tPedestrian* pPed, tU32 pTime);

/* SetCharacterPositionAR: no standalone symbol (the original inlines it);
   body at the TU end. */
extern void C2_HOOK_FASTCALL SetCharacterPositionAR(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);

/* StopSmoothTurning: no standalone symbol in the original; inlined into MungePedestrians
   0x4d3740 (two sites) and into SetThisPedPhysicing/SmoothTurnPedestrian. static so MSVC
   expands it at the call sites; no // FUNCTION: annotation because it has no address. */
static void C2_HOOK_FASTCALL StopSmoothTurning(tPedestrian* pPed) {
    pPed->field_0x0c->field_0x6c = 0;
}
/* SetPedXZDirection: the original has a standalone symbol at 0x004cc2c0, but a body at
   this slot flips DisposePedPersonality 0x4077f0, BonerPedMovedByPhysics 0x40b770 and
   FUN_00404290 0x404290, so the body lives at the TU end. */
extern void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pArg3, tU32 pTime);
/* SetThisPedPhysicing: no standalone symbol in the original; inlined into MungePedestrians
   0x4d3740 (two sites: the falling branch and the low-gravity branch). The original expands
   to `field_0x6c = 0` followed by SetCharacterPhysicsLevelAR(character, 5). */
static void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed) {
    extern void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel);

    StopSmoothTurning(pPed);
    SetCharacterPhysicsLevelAR(pPed->character, 5);
}
/* RescanPedProximity: the original has a standalone symbol at 0x004cfab0 (0x004a01f0 is the
   fork's recompiled address); body at the TU end. */
extern float C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, br_vector3* pArg2);
/* PedAnimCausesMovement: no standalone symbol in the original; it is inlined into
   MungePedestrians (0x004d4323). static so MSVC expands it at the call site.
   Placement is load-bearing: at this slot BonerPedMovedByPhysics 0x40b770 stays 100;
   anywhere above MungePedestrians, or at the TU end (with or without a prototype),
   flips it to 80. */
static int C2_HOOK_FASTCALL PedAnimCausesMovement(tPedestrian* pPed) {
    return !pPed->character->personality->form->moves[pPed->character->field_0x7].move->field_0x2a;
}
/* SmoothTurnPedestrian: no standalone symbol in the original; inlined into MungePedestrians
   0x4d3740 (the smooth-turn block). static, body right after DisposePedPersonality below. */
static void C2_HOOK_FASTCALL SmoothTurnPedestrian(tPedestrian* pPed, tU32 pTime);
/* RandomWander: no standalone symbol in the original; inlined into MungePedestrians 0x4d3740
   (block 0x4d45d9-0x4d4732). Body right after DisposePedPersonality below; this forward
   declaration is needed because a body at this slot flips DisposePedPersonality 0x4077f0. */
static void C2_HOOK_FASTCALL RandomWander(tPedestrian* pPed, tU32 pTime);
/* Address resolved from MungePedestrians 0x4d3740: the call at 0x4d4739
   (`mov edx,[esp+0x14]; mov ecx,ebp; call 0x4cef80`, i.e. (ped, time)) is followed by
   `test BYTE PTR [ped->field_0x0c+0x18],...` which guards the inlined CheckForAvoidingAction.
   Constants from the original's .rdata. */
// FUNCTION: CARMA2_HW 0x004cef80
void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime) {
    extern br_vector3 gY_unit_vector;
    extern void C2_HOOK_FASTCALL DroneNoOp(void);
    tPed_cache_006944c0* ped_cache;
    tPhysics_object* object;
    tPhysics_object* best_object;
    tPhysics_object* player_object;
    tCar_spec* car;
    br_actor* actor;
    const br_vector3* object_pos;
    br_vector3 target_pos;
    br_matrix34 rotation;
    float search_distance;
    float best_distance;
    float delta_x;
    float delta_y;
    float delta_z;
    float relative_x;
    float relative_z;
    float distance;
    float projection;
    int move;

    if (pPed->field_0x0c == NULL) {
        return;
    }
    pPed->field_0x0c->field_0x18 &= ~4;
    srand((tU32)pPed);
    search_distance = SRandomBetween(0.8f, 1.2f) * 7.0;
    best_distance = 3.402823466e+38f;
    best_object = NULL;
    object = PHILGetFirstObject();
    player_object = gProgram_state.current_car.collision_info;
    ped_cache = pPed->field_0x0c;

    if (gProgram_state.current_car.keys.horn) {
        best_object = player_object;
        actor = best_object->actor;
        object_pos = &actor->t.t.translate.t;
        delta_x = object_pos->v[0] - pPed->pos.v[0];
        delta_y = object_pos->v[1] - pPed->pos.v[1];
        delta_z = object_pos->v[2] - pPed->pos.v[2];
        if ((float)fabs(delta_x) < 5.f
                && (float)fabs(delta_y) < 5.f
                && (float)fabs(delta_z) < 5.f) {
            target_pos = *object_pos;
            best_distance = 0.f;
            goto found_target;
        }
    }

    while (object != NULL) {
        car = (tCar_spec*)object->owner;
        if (car == NULL || ((object->flags_0x238 < 2 || object->flags_0x238 > 4)
                && (car == NULL || (tS8)object->flags_0x238 != 5))) {
            actor = object->actor;
            object_pos = &actor->t.t.translate.t;
            delta_x = object_pos->v[0] - pPed->pos.v[0];
            delta_y = object_pos->v[1] - pPed->pos.v[1];
            delta_z = object_pos->v[2] - pPed->pos.v[2];
            if ((float)fabs(delta_x) < 5.f
                    && (float)fabs(delta_y) < 5.f
                    && (float)fabs(delta_z) < 5.f) {
                relative_x = object->v.v[0] - pPed->character->field_0xd8.v[0];
                relative_z = object->v.v[2] - pPed->character->field_0xd8.v[2];
                if (gPeds_suicidal) {
                    distance = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
                    if (!gEthereal_pedestrians && car != NULL && object->flags_0x238 == 1
                            && car != NULL && car->driver > 5 && distance < car->proxy_ray_distance) {
                        DoPedAnnihilator(pPed, car, distance, pTime, NULL);
                        goto calm_down;
                    }
                    if (distance < best_distance) {
                        best_distance = distance;
                        best_object = object;
                        target_pos = *object_pos;
                    }
                } else if (relative_x != gConst_replay_rate_zero || relative_z != gConst_replay_rate_zero
                        || (car != NULL && ((tS8)object->flags_0x238 == 1
                            && car != NULL && car->driver > 5
                            && car->proxy_ray_distance != gConst_replay_rate_zero))) {
                    if (relative_x == gConst_replay_rate_zero && relative_z == gConst_replay_rate_zero) {
                        distance = delta_x * delta_x + delta_z * delta_z;
                    } else {
                        float cross;

                        cross = relative_z * delta_x - relative_x * delta_z;
                        distance = (cross * cross) / (relative_x * relative_x + relative_z * relative_z);
                    }
                    if (!gEthereal_pedestrians && car != NULL && (tS8)object->flags_0x238 == 1
                            && car != NULL && car->driver > 5 && distance < car->proxy_ray_distance) {
                        DoPedAnnihilator(pPed, car, distance, pTime, NULL);
                        goto calm_down;
                    }
                    if (distance < object->radius + object->radius) {
                        projection = -((relative_z * delta_z + relative_x * delta_x)
                                / (relative_x * relative_x + relative_z * relative_z));
                        if (projection >= gConst_replay_rate_zero && projection < search_distance) {
                            best_distance = distance;
                            best_object = object;
                            target_pos = *object_pos;
                            if (object->flags_0x238 == 16) {
                                DroneNoOp();
                            }
                            srand(pTime ^ (tU32)pPed);
                            goto found_target;
                        }
                    }
                }
            }
        }
        object = PHILGetNextObject(object);
    }

    srand(pTime ^ (tU32)pPed);
    if (!gPeds_suicidal || best_distance == 3.402823466e+38f) {
        goto calm_down;
    }

found_target:
    if ((gBlind_pedestrians != 0 || gPed_valium_left != 0) && !gProgram_state.current_car.keys.horn) {
        goto calm_down;
    }
    if ((ped_cache->field_0x18 & 2) != 0
            && !(best_distance / *(float*)ped_cache->field_0xbc < 0.25)) {
        goto calm_down;
    }
    ped_cache->field_0x18 |= 4;
    if (gPeds_suicidal) {
        target_pos.v[0] -= pPed->pos.v[0];
        target_pos.v[1] -= pPed->pos.v[1];
        target_pos.v[2] -= pPed->pos.v[2];
        SetPedXZDirection(pPed, &target_pos, 0.f, pTime);
        RescanPedProximity(pPed, NULL);
        return;
    }

    ped_cache->field_0x30 = best_object->v;
    ped_cache->field_0x3c = target_pos;
    target_pos.v[0] = pPed->pos.v[0] - target_pos.v[0];
    target_pos.v[1] = pPed->pos.v[1] - target_pos.v[1];
    target_pos.v[2] = pPed->pos.v[2] - target_pos.v[2];
    distance = (float)sqrt(target_pos.v[0] * target_pos.v[0] + target_pos.v[1] * target_pos.v[1] + target_pos.v[2] * target_pos.v[2]);
    if (distance <= 2.384185791015625e-07f) {
        ped_cache->field_0xa4.v[0] = 1.f;
        ped_cache->field_0xa4.v[1] = 0.f;
        ped_cache->field_0xa4.v[2] = 0.f;
    } else {
        float scale;

        scale = (float)(1.0 / distance);
        ped_cache->field_0xa4.v[0] = scale * target_pos.v[0];
        ped_cache->field_0xa4.v[1] = scale * target_pos.v[1];
        ped_cache->field_0xa4.v[2] = scale * target_pos.v[2];
    }
    BrMatrix34Rotate(&rotation, IRandomPosNeg(0x2aa8), &gY_unit_vector);
    if (best_object->v.v[0] * best_object->v.v[0]
            + best_object->v.v[1] * best_object->v.v[1]
            + best_object->v.v[2] * best_object->v.v[2] <= 1.0) {
        if (!gProgram_state.current_car.keys.horn || best_object != player_object) {
            return;
        }
    }
    if (pPed->action == ePed_action_walking) {
        target_pos.v[0] = -ped_cache->field_0xa4.v[0];
        target_pos.v[1] = -ped_cache->field_0xa4.v[1];
        target_pos.v[2] = -ped_cache->field_0xa4.v[2];
        SetPedXZDirection(pPed, &target_pos, 0.f, pTime);
        RescanPedProximity(pPed, NULL);
        if (pPed->action != ePed_action_running && pPed->action != ePed_action_panicking
                && pPed->action != ePed_action_getting_up) {
            SetPedMove(pPed, gPed_move_fsm[114][IRandomBetween(0, 7)], -1, 0, 1, pTime, 2);
            MakePedNoise(pPed, 6, 0, NULL);
            if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
                pPed->field_0x0c->field_0x1c = (const tPed_anim_seq*)gScare_ped_anim_sequence;
                pPed->field_0x0c->field_0x78 = pTime;
                pPed->field_0x0c->field_0x1a = 0;
                SetCharacterBoneModelAR(pPed->character,
                    pPed->character->personality->form->index_head_bone,
                    pPed->field_0x0c->field_0x1c->poses[0],
                    0);
            }
        }
        pPed->field_0x28 = pTime;
        *(tU32*)pPed->field_0x2c = pTime;
        return;
    }

    if (pTime - *(tU32*)pPed->field_0x2c > 9999 && ped_cache->field_0x98 < 1) {
        if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
            int bone_index;
            int model_set;
            int pose;

            ped_cache->field_0x1c = (const tPed_anim_seq*)gPed_anim_sequence_0065e4e0;
            ped_cache->field_0x78 = pTime;
            ped_cache->field_0x1a = 0;
            bone_index = pPed->character->personality->form->index_head_bone;
            pose = ped_cache->field_0x1c->poses[0];
            if (pPed->character->field_0x4 >= 0) {
                model_set = GetCharacterModelSet(pPed->character);
                if (bone_index < 0) {
                    PipeSinglePedModelChange(pPed, bone_index, NULL, -1, model_set, 0);
                    SetCharacterAllBonesModel(pPed->character, pose, 0);
                } else {
                    PipeSinglePedModelChange(pPed, bone_index,
                        GetCharacterBoneModel(pPed->character, bone_index), pose, model_set, 0);
                    SetCharacterBoneModel(pPed->character, bone_index, pose, 0);
                }
            }
        }
        move = ((int*)((char*)gPed_move_fsm + 0xc98))[IRandomBetween(0, 8)];
        if (move < 0) {
            move = -move;
            SetPedMove(pPed, move, -1, 0, 1, pTime, 1);
        } else {
            SetPedMove(pPed, move, -1, 0, 1, pTime, 2);
        }
        if (move == 2) {
            MakePedNoise(pPed, 6, 0, NULL);
        } else if (move == 10 || move == 11) {
            MakePedNoise(pPed, 8, 0, NULL);
        } else if (move == 13) {
            MakePedNoise(pPed, 10, 1, NULL);
        }
    }
    if (pPed->action != ePed_action_running && pPed->action != ePed_action_panicking
            && pPed->action != ePed_action_getting_up) {
        SetPedMove(pPed, gPed_move_fsm[114][IRandomBetween(0, 7)], -1, 0, 1, pTime, 2);
        MakePedNoise(pPed, 6, 0, NULL);
        if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
            ped_cache->field_0x1c = (const tPed_anim_seq*)gScare_ped_anim_sequence;
            ped_cache->field_0x78 = pTime;
            ped_cache->field_0x1a = 0;
            SetCharacterBoneModelAR(pPed->character,
                pPed->character->personality->form->index_head_bone,
                ped_cache->field_0x1c->poses[0],
                0);
        }
    }
    pPed->field_0x28 = pTime;
    return;

calm_down:
    if (pPed->hit_points > 0 && pPed->action == ePed_action_running
            && ped_cache->field_0x18 == 0 && pTime - pPed->field_0x28 > 9999
            && pPed->character->field_0xc == 0 && ped_cache->field_0x98 < 1) {
        int bone_index;
        int model_set;
        int pose;

        move = IRandomBetween(pPed->movement_spec->min_walk_speed_factor, pPed->movement_spec->max_walk_speed_factor);
        SetPedMove(pPed, 30, move, 0, 1, pTime, 0);
        if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
            ped_cache->field_0x1c = (const tPed_anim_seq*)&gCalm_ped_anim_sequence;
            ped_cache->field_0x78 = pTime;
            ped_cache->field_0x1a = 0;
            bone_index = pPed->character->personality->form->index_head_bone;
            pose = ped_cache->field_0x1c->poses[0];
            if (pPed->character->field_0x4 >= 0) {
                model_set = GetCharacterModelSet(pPed->character);
                if (bone_index < 0) {
                    PipeSinglePedModelChange(pPed, bone_index, NULL, -1, model_set, 0);
                    SetCharacterAllBonesModel(pPed->character, pose, 0);
                    return;
                }
                PipeSinglePedModelChange(pPed, bone_index,
                    GetCharacterBoneModel(pPed->character, bone_index), pose, model_set, 0);
                SetCharacterBoneModel(pPed->character, bone_index, pose, 0);
            }
        }
    }
}

/* CheckForAvoidingAction: NO standalone symbol in the original; the whole body is inlined
   into MungePedestrians 0x4d3740 (0x4d474f-0x4d4b5a, guarded by `if (ped->field_0x0c->field_0x18)`).
   Evidence: its two thresholds 0x00694300 (gPed_min_dist_avoid_collisions_squared) and
   0x0069413c (gPed_reach_squared) are referenced at exactly one site in the binary, 0x4d479b /
   0x4d47b0, inside MungePedestrians. Body at the TU end; no `// FUNCTION:` annotation. */
extern void C2_HOOK_FASTCALL CheckForAvoidingAction(tPedestrian* pPed, tU32 pTime);
/* SetRandomOmega: NO standalone symbol in the original either. Its only call site
   (MungePedestrians 0x4d3740, low-gravity branch) has the three SRandomPosNeg(1.0f) calls
   inlined at 0x4d4c69/0x4d4c76/0x4d4c83, storing to omega (+0x74/+0x78/+0x7c), followed by
   the length/scale code (2.0f constant at 0x0058aaa4). Body at the TU end; no `// FUNCTION:`. */
extern void C2_HOOK_FASTCALL SetRandomOmega(tPhysics_object* pObject, float pMax);
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL CheckPowerupMoveSubstitution(tPedestrian* pPed, tU32 pTime) {
    NOT_IMPLEMENTED();
}
/* Forwarder kept at the old stub slot (a real body here flips 0x4091e0/0x40b770); the real
   MungeCharacterAnimation (0x409ca0) is MungeCharacterAnimationBody at the end of the TU. */
extern void C2_HOOK_FASTCALL MungeCharacterAnimationBody(tPed_character_instance* pCharacter, tU32 pTime, undefined4 pArg3);
void C2_HOOK_FASTCALL MungeCharacterAnimation(tPed_character_instance* pCharacter, tU32 pTime, undefined4 pArg3) {
    MungeCharacterAnimationBody(pCharacter, pTime, pArg3);
}
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL StillifyCorpse(tPedestrian* pPed, tU32 pTime, undefined4 pArg3) {
    NOT_IMPLEMENTED();
}
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL MungePedHeadAnim(tPedestrian* pPed, tU32 pTime) {
    NOT_IMPLEMENTED();
}
/* temporary placeholder, address not identified yet */
void C2_HOOK_FASTCALL ScarePedestrian(tPedestrian* pPed, tU32 pTime, int pArg3, int pArg4) {
    NOT_IMPLEMENTED();
}

#define MUNGE_PED_COLLISION_OBJECT(PED) ( ((PED)->character->field_0x14 & 1) ? (PED)->character->personality->form->simple_physicing[(PED)->character->field_0x5].collision_info : GetRootObject((PED)->character))

// PARTIAL 16.6%: MungePedestrians 0x4d3740 - upstream-shaped body; original inlines many helpers (frame 0x3b8, ped halt, napalm kill, wander, avoid)
// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

// ARResetNapalmBolts

// FUNCTION: CARMA2_HW 0x004d5b40
void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    int i;

    gNapalm_bolts[pIndex].ped = pPed;
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts[pIndex].actors); i++) {
        if (Vector3IsZero(&pPositions[i])) {
            break;
        }
        BrVector3Copy(&gNapalm_bolts[pIndex].actors[i]->t.t.translate.t, &pPositions[i]);
        gNapalm_bolts[pIndex].actors[i]->render_style = BR_RSTYLE_FACES;
    }
}

// GLOBAL: CARMA2_HW 0x0069bc58
tPed_face_cache_0x34* gPed_face_cache_heads[4];

// GLOBAL: CARMA2_HW 0x006a03f8
tPed_face_cache_0x34* gPed_face_caches[4];

// GLOBAL: CARMA2_HW 0x0069bc68
br_vector3 gChanged_points[10];

// GLOBAL: CARMA2_HW 0x0069412c
int gCount_changed_points;

// GLOBAL: CARMA2_HW 0x0074480c
int gTotal_count_smash_peds;

// GLOBAL: CARMA2_HW 0x0069bce4
int gPed_score_multiplier;

// GLOBAL: CARMA2_HW 0x0069bc30
int gPed_recent_points;

// GLOBAL: CARMA2_HW 0x006a0410
int gPipe_halted_ped_status;

// GLOBAL: CARMA2_HW 0x007447c0
br_vector3 gPed_scale;

// GLOBAL: CARMA2_HW 0x0069bc24
float gAverage_ped_scale;

// GLOBAL: CARMA2_HW 0x00744800
float gPed_head_scale;

// GLOBAL: CARMA2_HW 0x00744804
int gBOOL_00744804;

// GLOBAL: CARMA2_HW 0x007447cc
int gCount_killed_peds;

// FUNCTION: CARMA2_HW 0x004d5bd0
void C2_HOOK_FASTCALL InitPedsForRace(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
        gPed_face_caches[i] = gPed_face_cache_heads[i];
        for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
            gPed_face_cache_heads[i][j].field_0x14 = NULL;
            if (j != gPed_cache_sizes_1[i] - 1) {
                gPed_face_cache_heads[i][j].next = &gPed_face_cache_heads[i][j + 1];
                gPed_face_cache_heads[i][j + 1].prev = &gPed_face_cache_heads[i][j];
            } else {
                gPed_face_cache_heads[i][j].next = &gPed_face_cache_heads[i][0];
                gPed_face_cache_heads[i][0].prev = &gPed_face_cache_heads[i][j];
            }
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_006944c0); i++) {
        gPed_cache_006944c0[i].field_0xb4 = NULL;
        gPed_cache_006944c0[i].field_0xb0 = NULL;
        gPed_cache_006944c0[i].field_0x74 = 0;
    }
    gPed_count = 0;
    gCount_changed_points = 0;
    gTwtPeds = -1;
    gTotal_count_smash_peds = 0;
    gPed_score_multiplier = 0;
    gPed_recent_points = 0;
    gPipe_halted_ped_status = 0;
    gImmortal_peds = 0;
    gPeds_suicidal = 0;
    gMutant_speed = 0.f;
    gExploding_pedestrians = 0;
    gPed_dismemberfest = 0;
    gBlind_pedestrians = 0;
    gDancing_peds = 0;
    gPanicking_peds = 0;
    gDrunk_pedestrians = 0;
    gEthereal_pedestrians = 0;
    gPedestrian_speed_factor = 1.f;
    BrVector3Set(&gPed_scale, 1.f, 1.f, 1.f);
    gAverage_ped_scale = 1.f;
    gPed_head_scale = 1.f;
    gPed_gravity_multiplier = 1.f;
    gBOOL_00744804 = 0;
    gCount_killed_peds = 0;
    gPed_valium_left = 0;
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gNapalm_bolts[i].actors); j++) {
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
        }
    }
    memset(gPed_cache_006944c0, 0, sizeof(gPed_cache_006944c0));
}

// RenderElectroBastardRays

// AdjustProxRay

// ResetProxRay

// FUNCTION: CARMA2_HW 0x004d6210
tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void) {

    if (gPed_count != 0) {
        return gPedestrian_array[gSelected_ped].character;
    }
    return NULL;
}
// NextPedCam

// FUNCTION: CARMA2_HW 0x004d6290
void C2_HOOK_FASTCALL PrevPedCam(void) {
    int count;
    int i;
    int start;

    count = gPed_count;
    if (count == 0) {
        return;
    }
    i = gSelected_ped;
    start = i;
    do {
        i -= 1;
        gSelected_ped = i;
        if (i < 0) {
            i = count - 1;
            gSelected_ped = i;
        }
    } while (!(gPedestrian_array[i].flags & 1) && start != i);
}

// FindNearestPed

// FUNCTION: CARMA2_HW 0x004d6390
void C2_HOOK_FASTCALL ResetPedNearness(void) {

    gPed_nearness = 1;
}

extern void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV);

// FUNCTION: CARMA2_HW 0x004d6880
void C2_HOOK_FASTCALL PedPreCollisionStuff(void) {
    tPhysics_object* pObject;
    br_vector3 v;

    for (pObject = PHILGetFirstObject(); pObject != NULL; pObject = PHILGetNextObject(pObject)) {
        if (pObject->owner != NULL && pObject->flags_0x238 == 5) {
            v.v[0] = pObject->omega.v[0] * (BrVector3Length(&pObject->omega) * 4e-06);
            v.v[1] = pObject->omega.v[1] * (BrVector3Length(&pObject->omega) * 4e-06);
            v.v[2] = (float)(BrVector3Length(&pObject->omega) * 4e-06) * pObject->omega.v[2];
            ApplyWaterOmegaBrake(pObject, &v);
        }
    }
}

// UpdateIfBackwardsAR

// ChangePedScaling

// ChangeHeadScaling

// MakeMaterialBlendy

// UnmakeMaterialBlendy

// TurnOffCollisions

// TurnOnCollisions

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

// MutatePed

// FUNCTION: CARMA2_HW 0x004d6c70
void C2_HOOK_FASTCALL FlushAllPedCaches(void) {
    int i;
    int count;
    tPedestrian* base;
    tPed_cache_006944c0** pp;

    count = gPed_count;
    base = gPedestrian_array;
    i = 0;
    if (0 < count) {
        pp = &base->field_0x0c;
        do {
            if (*pp != NULL && (*pp)->field_0xb0 != NULL) {
                (*pp)->field_0xb0->field_0x14 = NULL;
                (*pp)->field_0xb0 = NULL;
            }
            i++;
            pp = (tPed_cache_006944c0**)((tU8*)pp + sizeof(tPedestrian));
        } while (i < gPed_count);
    }
}

// FUNCTION: CARMA2_HW 0x004d6b70
void C2_HOOK_FASTCALL EtherealiseMaterial1(void* pMaterial) {
    BlendifyMaterial((br_material*)pMaterial, 0x19);
    BrMaterialUpdate((br_material*)pMaterial, 0x7fff);
}

// FUNCTION: CARMA2_HW 0x004d6b90
void C2_HOOK_FASTCALL EtherealiseCharacter1(void* pCharacter) {

    *(tU32*)((char*)pCharacter + 0x19c) |= 4;
}

// FUNCTION: CARMA2_HW 0x004d6ba0
void C2_HOOK_FASTCALL EtherealiseMaterial2(void* pMaterial) {
    BrModelUpdate((br_model*)pMaterial, 0x10);
}

// FUNCTION: CARMA2_HW 0x004d6bb0
void C2_HOOK_FASTCALL MakePedsNotEthereal(void) {

    MungeAllCharacterMaterials(DeetherealiseMaterial1);
    MungeAllCharacterObjects(DeetherealiseCharacter1);
    MungeAllCharacterModels(EtherealiseMaterial2);
    gEthereal_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004d6be0
void C2_HOOK_FASTCALL DeetherealiseMaterial1(void* pMaterial) {
    BlendifyMaterial((br_material*)pMaterial, 0x64);
    BrMaterialUpdate((br_material*)pMaterial, 0x7fff);
}

// FUNCTION: CARMA2_HW 0x004d6c00
void C2_HOOK_FASTCALL DeetherealiseCharacter1(void* pCharacter) {

    *(tU32*)((char*)pCharacter + 0x19c) &= ~4;
}

// FUNCTION: CARMA2_HW 0x004d6d90
void C2_HOOK_FASTCALL SavePedStuff(void) {

}

// ComparePedStuff

// AdjustPedDiagnostics

// FUNCTION: CARMA2_HW 0x004d6dc0
int C2_HOOK_FASTCALL CalmDownAllPeds(void) {
    tU32 now;
    int i;
    tPedestrian* ped;

    now = GetTotalTime();
    ped = gPedestrian_array;
    for (i = 0; i < gPed_count; i++, ped++) {
        if (ped->field_0x0c != NULL && ped->hit_points > 0 && ped->action == ePed_action_running) {
            SetPedMove(ped, 30, IRandomBetween(ped->movement_spec->min_walk_speed_factor, ped->movement_spec->max_walk_speed_factor), 0, 1, now, ePed_action_walking);
            SetPedHeadAnim(ped, (const tPed_anim_seq*)&gCalm_ped_anim_sequence, now);
        }
    }
    gPed_valium_left = 5000;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004d6f10
void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void) {
    gPedsFolder = "PEDS";
    SetDefaultSoundFolderName();
    SetDefaultPowerupFilename();
    SetDefaultTextFileName();
    SetDefaultPixelmapFolderName();
}

// FUNCTION: CARMA2_HW 0x004d6f50
void C2_HOOK_FASTCALL SetZombiePedFolderNames(void) {
    gPedsFolder = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f30
void C2_HOOK_FASTCALL SetBloodPedFolderNames(void) {
    gPedsFolder = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f70
void C2_HOOK_FASTCALL SetAlienPedFolderNames(void) {
    gPedsFolder = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f90
void C2_HOOK_FASTCALL AddChangedPoint(const br_vector3* pPoint) {

    BrVector3Copy(&gChanged_points[gCount_changed_points], pPoint);
    gCount_changed_points++;
}

// FUNCTION: CARMA2_HW 0x004d6fc0
int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void) {

    return 2 - gGoreLevel;
}

// FUNCTION: CARMA2_HW 0x004d6fd0
int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void) {

    return gAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d6fe0
int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void) {

    return gFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d6ff0
int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void) {

    return gExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7000
void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn) {

    gAnimalsOn = pNewAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d7010
void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    gFlameThrowerOn = pNewFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d7020
void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on) {

    gExplosives_on = pExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7030
void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel) {

    gGoreLevel = 2 - pNewLevel;
}

// ClearOutMorphs

// DRVector3SafeCross

// DRVector3SafeCrossXZ

// FUNCTION: CARMA2_HW 0x004042f0
void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale) {
    int i;
    br_vertex* v;

    REC2_THISCALL_UNUSED(pArg2);

    v = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++, v++) {
        BrVector3Scale(&v->p, &v->p, pScale);
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// based on REA pseudocode 0x4cd260 and /* kept out of line: KillPedestrian must keep its call 0x004cd260 */
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004cd260
void C2_HOOK_FASTCALL ScoreForKilledPedestrian(tPedestrian* pPed, float pHeight) {
    tU32 now;
    tPed_action action;
    tCar_spec* car;
    tPhysics_object* obj;
    int points;
    /* NB: single struct (not two locals) so VC5 packs rel at [esp+0x14] and out at [esp+0x20] as in the original. */
    struct {
        br_vector3 rel;
        br_vector3 out;
    } vecs;
    br_actor* actor;
    tU16 flags;

    now = GetTotalTime();
    action = pPed->action;
    pPed->hit_points = 0;
    if (action != ePed_action_dead) {
        PipeSinglePedMove(pPed, -1, -1, -1, 0.f, 0.f, 0, 0, NULL, NULL, action, ePed_action_dead, NULL);
        pPed->action = ePed_action_dead;
    }
    *(tU32*)((char*)pPed + 0x30) = now;
    if (pPed->field_0x0c == NULL) {
        return;
    }
    car = pPed->field_0x0c->field_0xc0;
    if (car == NULL) {
        return;
    }
    if (car->driver != eDriver_local_human) {
        return;
    }
    if (now - gPed_last_score_time <= 1000) {
        if (gPed_score_multiplier < 10) {
            gPed_score_multiplier = gPed_score_multiplier + 1;
        }
    } else {
        gPed_score_multiplier = 1;
    }
    gPed_last_score_time = now;
    points = gPed_points[gProgram_state.skill_level];
    points *= gPed_score_multiplier;
    obj = pPed->character->personality->form->simple_physicing[pPed->character->field_0x5].collision_info;
    vecs.rel.v[0] = pPed->pos.v[0] - car->pos.v[0];
    vecs.rel.v[1] = pPed->pos.v[1] - car->pos.v[1];
    vecs.rel.v[2] = pPed->pos.v[2] - car->pos.v[2];
    BrMatrix34TApplyV(&vecs.out, &vecs.rel, &car->collision_info->actor->t.t.mat);
    flags = *(tU16*)&pPed->field_0x0c->field_0x2e;
    if (flags & 0x10) {
        points *= 20;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x20);
    } else if (flags & 8) {
        points *= 5;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x1f);
    } else if (flags & 0x40) {
        points *= 2;
        PratcamEvent(0x1e);
        DoFancyHeadup(2);
    } else if (flags & 1) {
        points *= 4;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x11);
    } else if (flags & 0x20) {
        points *= 2;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x1e);
    } else if ((pHeight == 0.0f
            && ((float)fabs(car->collision_info->omega.v[0]) > 5.0f
                || (float)fabs(car->collision_info->omega.v[2]) > 5.0f
                || (actor = car->collision_info->actor, actor->t.t.mat.m[1][1] < 0.1f)
                || actor->t.t.mat.m[3][1] - pPed->pos.v[1] > 0.05f))
            || pPed->field_0x0c->field_0x98 > 0) {
        points *= 4;
        PratcamEvent(0x1e);
        DoFancyHeadup(3);
    } else if (pHeight > 1.0) {
        points *= 2;
        DoFancyHeadup(6);
        PratcamEvent(0x1e);
    } else if (obj == NULL
            || (BrVector3LengthSquared(&obj->v) < 2.0
                && BrVector3LengthSquared((br_vector3*)obj->field_0x1b4) < 2.0)) {
        if (pHeight == 0.0f
                && (((float)fabs(vecs.out.v[0]) / (float)fabs(vecs.out.v[2]) >= 0.5
                        && ((float)fabs(car->collision_info->velocity_car_space.v[0]) > (float)fabs(car->collision_info->velocity_car_space.v[2])
                            || (float)fabs(car->collision_info->omega.v[1] / car->collision_info->velocity_car_space.v[2]) > 600.0f))
                    || (vecs.out.v[2] > 0.0f && car->collision_info->velocity_car_space.v[2] > 0.0f))) {
            points *= 2;
            DoFancyHeadup(2);
        } else {
            if (gPed_score_multiplier > 1) {
                DoFancyHeadup(gPed_score_multiplier + 6);
                PratcamEvent(0x1e);
            } else if (flags & 2) {
                points *= 2;
                DoFancyHeadup(0);
                PratcamEvent(0x1e);
            }
        }
    } else {
        points *= 4;
        DoFancyHeadup(5);
        PratcamEvent(0x1e);
    }
    EarnCredits(points);
    gPed_recent_points = points;
    gPed_recent_ped = pPed;
    AwardTime(gPed_times[gProgram_state.skill_level]);
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on REA pseudocode 0x4cce70 and // FUNCTION: CARMA2_HW 0x004cce70
void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info) {
    tPed_character_instance* character;
    int head_bone;

    if (GET_PED_COLLISION_OBJECT(pPed)->last_special_volume == NULL
            || GET_PED_COLLISION_OBJECT(pPed)->last_special_volume->gravity_multiplier >= 1.f) {
        if (gGoreLevel > 0) {
            character = pPed->character;
            if (character->field_0x4 >= 0) {
                PipeSinglePedModelChange(character->ped, -1, NULL, -1, GetCharacterModelSet(character), 1);
                SetCharacterAllBonesModel(character, 0, 1);
            }
            character = pPed->character;
            head_bone = character->personality->form->index_head_bone;
            if (character->field_0x4 >= 0) {
                if (head_bone < 0) {
                    PipeSinglePedModelChange(character->ped, head_bone, NULL, -1, GetCharacterModelSet(character), 1);
                    SetCharacterAllBonesModel(character, 2, 1);
                } else {
                    PipeSinglePedModelChange(character->ped, head_bone,
                        GetCharacterBoneModel(character, head_bone), 2,
                        GetCharacterModelSet(character), 1);
                    SetCharacterBoneModel(character, head_bone, 2, 1);
                }
            }
            if (pPed->field_0x0c != NULL) {
                pPed->field_0x0c->field_0x1c = NULL;
            }
            DoGiblets(pPed, GET_PED_COLLISION_OBJECT(pPed), pCollision_info, 0.4f, &pPed->pos, 1);
        }
    }
    OneLessPed(pPed);
    ScoreForKilledPedestrian(pPed, 0.f);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00404350
void C2_HOOK_FASTCALL DisposeMove(tPed_move* pMove) {
    int i;

    for (i = 0; i < pMove->count_frames; i++) {
        if (pMove->frames[i].axis != NULL) {
            BrMemFree(pMove->frames[i].axis);
        }
    }
    if (pMove->frames != NULL) {
        BrMemFree(pMove->frames);
    }
    if (pMove != NULL) {
        BrMemFree(pMove);
    }
}

// DisposeAllLoadedMoves

// ReadRemap

// FUNCTION: CARMA2_HW 0x004046d0
void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap) {

    if (pRemap != NULL) {
        if (pRemap->bones != NULL) {
            BrMemFree(pRemap->bones);
        }
        BrMemFree(pRemap);
    }
}

// DisposeAllLoadedRemaps

// FindOrOpenRemap

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

// SetUpCharacterForm

// DisposeCharacterForm

// DisposeAllLoadedForms

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00406a30
void C2_HOOK_FASTCALL DisposeCharacterInstance(tPed_character_instance* pCharacter) {

    if (pCharacter != NULL) {
        BrMemFree(pCharacter);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);

// FUNCTION: CARMA2_HW 0x00406a40
tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_forms); i++) {
        if (gPed_forms[i] != NULL) {
            if (strcmp(pName, gPed_forms[i]->name) == 0) {
                return gPed_forms[i];
            }
        }
    }
    return SetUpCharacterForm(pName);
}

// FUNCTION: CARMA2_HW 0x004080d0
void C2_HOOK_FASTCALL MakeRotationMatrix(br_matrix34* pMat, tU8 pAngle_x, tU32 pAngle_y, tU32 pAngle_z) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    tU32 local_4;

    local_4 = (tU32)pAngle_x;
    fVar4 = gPed_cos_table[pAngle_z & 0xff];
    fVar5 = gPed_sin_table[pAngle_z & 0xff];
    fVar6 = gPed_sin_table[local_4];
    fVar1 = gPed_cos_table[local_4];
    fVar2 = gPed_sin_table[pAngle_y & 0xff];
    fVar3 = gPed_cos_table[pAngle_y & 0xff];

    pMat->m[0][0] = fVar4 * fVar3;
    pMat->m[3][2] = 0.0f;
    pMat->m[3][1] = 0.0f;
    pMat->m[1][0] = fVar6 * fVar4 * fVar2 - fVar5 * fVar1;
    pMat->m[3][0] = 0.0f;
    pMat->m[2][0] = fVar4 * fVar1 * fVar2 + fVar5 * fVar6;
    pMat->m[0][1] = fVar5 * fVar3;
    pMat->m[1][1] = fVar5 * fVar6 * fVar2 + fVar4 * fVar1;
    pMat->m[2][1] = fVar5 * fVar1 * fVar2 - fVar6 * fVar4;
    pMat->m[0][2] = -fVar2;
    pMat->m[1][2] = fVar6 * fVar3;
    pMat->m[2][2] = fVar3 * fVar1;
}

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

extern tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);

// FUNCTION: CARMA2_HW 0x00407930
tPed_personality* C2_HOOK_FASTCALL FindOrOpenPersonality(const char* pName) {
    int i;
    tPed_personality** ppersonality;

    i = 0;
    ppersonality = gPed_personalities;
    while ((int)ppersonality < (int)(gPed_personalities + REC2_ASIZE(gPed_personalities))) {
        if (*ppersonality != NULL) {
            if (strcmp(pName, (*ppersonality)->name) == 0) {
                return gPed_personalities[i];
            }
        }
        ppersonality++;
        i++;
    }
    return ReadPersonality(pName);
}

// BuildCharacterInstance

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00407aa0
br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance *pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return pCharacter->field_0xe8;
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x00407ad0
br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index) {
    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return &pCharacter->field_0xe8[pBone_index];
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->t.t.mat;
}
// FUNCTION: CARMA2_HW 0x00407b10
br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return NULL;
    }
    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0];
}
// SetCharacterBonePositions

extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00408200
int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex) {
    tPed_personality* personality;
    tPed_form* form;
    tPed_form_actor_set* actor_set;
    br_matrix34* matrix;
    int i;

    if (pCharacter->field_0x4 >= 0) {
        return 2;
    }
    personality = pCharacter->personality;
    form = personality->form;
    actor_set = &form->actor_sets[pIndex];
    if (gAction_replay_mode == 0) {
        actor_set->field_0x0 = 1;
    }
    pCharacter->field_0x4 = pIndex;
    if (pCharacter->field_0x4 < 0) {
        matrix = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        matrix = pCharacter->field_0xe8;
    } else {
        matrix = &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34Copy(matrix, &pCharacter->field_0x2c);

    if (pCharacter->field_0xc != 0) {
        if (pCharacter->field_0x28 != 0) {
            for (i = 1; i < form->count_bones; i++) {
                if (pCharacter->field_0xc & gPow2_array[i]) {
                    BrMatrix34Copy(GetBoneMatrixPtr(pCharacter, i), (br_matrix34*)((char*)pCharacter->field_0x28 + i * 0x30));
                }
            }
            *(int*)pCharacter->field_0x28 = 0;
        } else {
            pCharacter->field_0x10 = pCharacter->field_0xc;
        }
    }

    for (i = 0; i < form->count_bones; i++) {
        actor_set->actors[i]->type_data = pCharacter;
        actor_set->actors[i]->render_style = BR_RSTYLE_DEFAULT;
        actor_set->actors[i]->model = personality->bones[i].models[0].models[(tS8)pCharacter->field_0xa];
        if (actor_set->actors[i]->model != NULL) {
            actor_set->actors[i]->type = BR_ACTOR_MODEL;
        } else {
            actor_set->actors[i]->type = BR_ACTOR_NONE;
        }
        if (!(pCharacter->field_0xc & gPow2_array[i]) || !(pCharacter->field_0x10 & gPow2_array[i])) {
            MakeActorRenderable(actor_set->actors[i], i, pCharacter);
        }
    }
    SetCharacterBonePositions(pCharacter, 0, 0);
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// Moved down here from the top of the file: at its original position this TU compiles it as
// `fld vertex; fmul pX` (reccmp 94.12); at this position it is 100%. Measured with reccmp.
// FUNCTION: CARMA2_HW 0x00404290
void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ) {
    int i;
    br_vertex* v;

    REC2_THISCALL_UNUSED(pArg2);

    v = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++, v++) {
        v->p.v[0] = pX * v->p.v[0];
        v->p.v[1] = pY * v->p.v[1];
        v->p.v[2] = pZ * v->p.v[2];
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x004083b0
int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter) {
    int i;
    tPed_form* form;
    tPed_form_actor_set* actor_sets;

    if (pCharacter->field_0x4 >= 0) {
        return 2;
    }
    form = pCharacter->personality->form;
    actor_sets = form->actor_sets;
    for (i = 0; i < form->max_rendering_at_once; i++) {
        if (!actor_sets[i].field_0x0) {
            break;
        }
    }
    if (i == form->max_rendering_at_once) {
        return 1;
    }
    return MakeCharacterRenderable2(pCharacter, i);
}

extern int C2_HOOK_FASTCALL CharacterNoLongerCollideworthy(tPed_character_instance* pCharacter);

// GLOBAL: CARMA2_HW 0x00589358
const br_scalar gFloat_00589358 = 0.f;

// FUNCTION: CARMA2_HW 0x00408400
int C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter) {
    tPed_form* form;
    tPed_form_actor_set* actor_set;
    br_matrix34* matrix;
    int i;
    int j;

    if (pCharacter->field_0x4 < 0) {
        return 0xb;
    }
    if (gAction_replay_mode == 0) {
        CharacterNoLongerCollideworthy(pCharacter);
    }
    form = pCharacter->personality->form;
    actor_set = &form->actor_sets[pCharacter->field_0x4];
    if (gAction_replay_mode == 0) {
        actor_set->field_0x0 = 0;
    }
    if (pCharacter->field_0x4 < 0) {
        matrix = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        matrix = pCharacter->field_0xe8;
    } else {
        matrix = &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34Copy(&pCharacter->field_0x2c, matrix);

    if (pCharacter->field_0xc != 0) {
        for (i = 0; i < form->max_stored_dismembered_characters; i++) {
            if (*(float*)form->stored_dismembered_characters[i] == gFloat_00589358) {
                break;
            }
        }
        if (i == form->max_stored_dismembered_characters) {
            pCharacter->field_0x28 = 0;
        } else {
            *(float*)form->stored_dismembered_characters[i] = 1.f;
            pCharacter->field_0x28 = (undefined4)form->stored_dismembered_characters[i];
            for (j = 1; j < form->count_bones; j++) {
                if (pCharacter->field_0xc & gPow2_array[j]) {
                    BrMatrix34Copy((br_matrix34*)((char*)pCharacter->field_0x28 + j * 0x30), GetBoneMatrixPtr(pCharacter, j));
                }
            }
        }
    }
    for (i = 0; i < form->count_bones; i++) {
        StopActorBeingRenderable(actor_set->actors[i]);
    }
    pCharacter->field_0x4 = -1;
    return 0;
}

// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int C2_HOOK_FASTCALL MakeCharacterPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, br_actor* pActor, void* pFrame, br_vector3* pBonePos, int pKind, undefined4 pArg3, int pFlag, int pBoneIndex, br_vector3* pBoneVec, br_vector3* pBonePtr);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);
// FUNCTION: CARMA2_HW 0x00408c30
int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4) {
    tPed_personality* personality;
    tPed_form* form;
    char* pcVar4;
    char* pcVar9;
    int iVar5;
    int iVar8;
    int iVar10;
    int iVar12;
    int iVar13;
    int uVar6;
    int uVar11;
    int param_2;
    int local_14;
    int local_10;
    br_vector3* local_8;
    float local_4;
    int bVar3;
    tPhysics_joint* uVar7;
    tPhysics_object* obj;
    br_actor* actor;
    char cVar1;

    if (((pCharacter->field_0x14 & 1) != 0 && pArg2 == 0) || ((pCharacter->field_0x14 & 2) != 0 && pArg2 != 0)) {
        return 0x17;
    }
    if (pCharacter->field_0x4 < 0) {
        uVar11 = 0;
        form = pCharacter->personality->form;
        uVar6 = form->max_rendering_at_once;
        pcVar4 = (char*)form->actor_sets;
        if (uVar6 != 0) {
            do {
                if (*pcVar4 == 0) {
                    break;
                }
                uVar11 = uVar11 + 1;
                pcVar4 = pcVar4 + 8;
            } while (uVar11 < uVar6);
        }
        if (uVar11 == uVar6) {
            iVar5 = 1;
        } else {
            iVar5 = MakeCharacterRenderable2(pCharacter, uVar11);
        }
        if (iVar5 == 1) {
            return 0x15;
        }
    }
    personality = pCharacter->personality;
    form = personality->form;
    if (pArg2 != 0) {
        if (pArg4 < 0) {
            pcVar4 = (char*)form->boned_physicing;
            uVar6 = 0;
            if (form->max_boned_physicing_at_once != 0) {
                do {
                    if (*pcVar4 == 0) {
                        break;
                    }
                    uVar6 = uVar6 + 1;
                    pcVar4 = pcVar4 + 8;
                } while (uVar6 < form->max_boned_physicing_at_once);
            }
        } else {
            pcVar4 = (char*)form->boned_physicing + pArg4 * 8;
            uVar6 = pArg4;
        }
        if (uVar6 != form->max_boned_physicing_at_once) {
            pCharacter->field_0x14 = 2;
            pCharacter->field_0x5 = (tS8)uVar6;
            *pcVar4 = 1;
            iVar13 = 0;
            local_10 = 1;
            bVar3 = 0;
            if (form->count_bones != 0) {
                param_2 = 0;
                local_14 = 0;
                do {
                    obj = ((tPed_form_boned_phys*)pcVar4)->collision_infos[iVar13];
                    if (obj->shape != NULL) {
                        *(tU32*)((char*)obj + 0x19c) = *(tU32*)((char*)obj + 0x19c) & 0xfffffbff;
                        if (personality->bones[param_2 / 0x3c].hinge == NULL) {
                            uVar7 = NULL;
                        } else {
                            uVar7 = ClonePhysicsJoint(personality->bones[param_2 / 0x3c].hinge, 0xdb);
                        }
                        obj->physics_joint1 = uVar7;
                        iVar8 = 0;
                        if (form->count_bones != 0) {
                            pcVar9 = (char*)&form->bones[0].indices[0];
                            do {
                                if (*pcVar9 == iVar13) {
                                    local_8 = (br_vector3*)((char*)personality->bones + 0x2c + iVar8 * 0x3c);
                                    break;
                                }
                                iVar8 = iVar8 + 1;
                                pcVar9 = pcVar9 + 0x34;
                            } while (iVar8 < form->count_bones);
                        }
                        actor = form->actor_sets[pCharacter->field_0x4].actors[iVar13];
                        iVar10 = *(int*)((char*)actor + 0x18);
                        if (*(char*)((char*)form->bones + 0x28 + local_14) < 0) {
                            iVar12 = 0;
                        } else {
                            iVar12 = (int)((char*)personality->bones + 0x20 + param_2);
                        }
                        if (iVar10 == 0) {
                            iVar10 = 0;
                        } else {
                            iVar10 = iVar10 + 0x34;
                        }
                        MakeCharacterPhysics(pCharacter, obj, actor, (void*)iVar10, NULL, (iVar13 > 1) + 1, pArg3, local_10, iVar13, (br_vector3*)iVar12, local_8);
                        local_10 = 0;
                        if (bVar3) {
                            if (obj->physics_joint1 != NULL) {
                                *(float*)((char*)obj->physics_joint1 + 4) = local_4;
                            }
                        } else {
                            bVar3 = 1;
                            local_4 = obj->I.v[0] * 25.0;
                        }
                    }
                    param_2 = param_2 + 0x3c;
                    iVar13 = iVar13 + 1;
                    local_14 = local_14 + 0x34;
                } while (iVar13 < form->count_bones);
            }
            PHILSetObjectProperty(((tPed_form_boned_phys*)pcVar4)->collision_infos[0], 1, 1);
            return 0;
        }
    }
    if (pArg4 < 0) {
        pcVar4 = (char*)form->simple_physicing;
        uVar6 = 0;
        pArg4 = 0;
        if (form->max_simple_physicing_at_once != 0) {
            do {
                if (*pcVar4 == 0) {
                    break;
                }
                pArg4 = pArg4 + 1;
                pcVar4 = pcVar4 + 8;
            } while (pArg4 < form->max_simple_physicing_at_once);
        }
        if (pArg4 == form->max_simple_physicing_at_once) {
            return 0x15;
        }
    } else {
        pcVar4 = (char*)form->simple_physicing + pArg4 * 8;
    }
    iVar5 = MakeCharacterPhysics(pCharacter, ((tPed_form_simple_phys*)pcVar4)->collision_info,
        form->actor_sets[pCharacter->field_0x4].actors[0], &personality->bb, &personality->centre_of_mass, 0, pArg3, 1, 0, NULL, NULL);
    if (iVar5 == 0) {
        return 0x15;
    }
    pCharacter->field_0x14 = 1;
    pCharacter->field_0x5 = (tS8)pArg4;
    *pcVar4 = 1;
    if (pCharacter->field_0xc != 0) {
        if (pCharacter->field_0x6 < 0) {
            uVar6 = 0;
            uVar11 = form->max_boned_physicing_at_once;
            pcVar4 = (char*)form->boned_physicing;
            if (uVar11 != 0) {
                do {
                    if (*pcVar4 == 0) {
                        break;
                    }
                    uVar6 = uVar6 + 1;
                    pcVar4 = pcVar4 + 8;
                } while (uVar6 < uVar11);
            }
            if (uVar6 == uVar11) {
                return 0;
            }
            pCharacter->field_0x6 = (tS8)uVar6;
            *pcVar4 = 1;
        }
        iVar5 = 1;
        if (form->count_bones > 1) {
            do {
                if (((pCharacter->field_0xc & gPow2_array[iVar5]) != 0) && ((pCharacter->field_0x10 & gPow2_array[iVar5]) == 0)) {
                    cVar1 = pCharacter->field_0x6;
                    actor = form->actor_sets[pCharacter->field_0x4].actors[iVar5];
                    iVar8 = MakeCharacterPhysics(pCharacter, NULL, actor, (void*)(*(int*)((char*)actor + 0x18) + 0x34), NULL, 3, pArg3, 1, 0, NULL, NULL);
                    if (iVar8 == 0) {
                        obj = NULL;
                    } else {
                        obj = ((tPed_form_boned_phys*)((char*)form->boned_physicing + cVar1 * 8))->collision_infos[iVar5];
                    }
                    if (obj != NULL) {
                        *(undefined4*)((char*)obj + 0x68) = 0;
                        *(undefined4*)((char*)obj + 0x6c) = 0;
                        *(undefined4*)((char*)obj + 0x70) = 0;
                        *(undefined4*)((char*)obj + 0x74) = 0;
                        *(undefined4*)((char*)obj + 0x78) = 0;
                        *(undefined4*)((char*)obj + 0x7c) = 0;
                    }
                }
                iVar5 = iVar5 + 1;
            } while (iVar5 < form->count_bones);
        }
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00409030
int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance *pCharacter, int pArg2, undefined4 pArg3) {

    return MakeCharacterCollideworthy2(pCharacter, pArg2, pArg3, -1);
}

/* not inlined: KillPedestrian must keep its call 0x00409040 */
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00409040
tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance *pPed) {
    tPed_form* form;
    tPed_form_boned_phys* boned_phys;
    int i;

    form = pPed->personality->form;
    boned_phys = &form->boned_physicing[pPed->field_0x5];
    for (i = 0; i < form->count_bones; i++) {
        if (boned_phys->collision_infos[i]->shape != NULL) {
            return boned_phys->collision_infos[i];
        }
    }
    return NULL;
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(on)
#endif
// CharacterNoLongerCollideworthy

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00409300
void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, br_vector3* pV) {
    tPhysics_object* child;

    pObject->v.v[0] = pV->v[0];
    pObject->v.v[1] = pV->v[1];
    pObject->v.v[2] = pV->v[2];
    for (child = pObject->child; child != NULL; child = child->next) {
        SetObjectV(child, pV);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);

// Inline copy of PedGetRootObjectInline for the MakeCharacterPhysicworthy call site: the original
// inlines the root-object search there, so keep a local static copy above the caller.
static tPhysics_object* C2_HOOK_FASTCALL PedGetRootObjectInlineMW(tPed_character_instance *pPed) {
    tPed_form* form;
    tPed_form_boned_phys* boned_phys;
    int i;

    form = pPed->personality->form;
    boned_phys = &form->boned_physicing[pPed->field_0x5];
    for (i = 0; i < form->count_bones; i++) {
        if (boned_phys->collision_infos[i]->shape != NULL) {
            return boned_phys->collision_infos[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004091e0
int C2_HOOK_FASTCALL MakeCharacterPhysicworthy(tPed_character_instance* pCharacter) {
    tPhysics_object* obj;
    tPhysics_object* child;
    br_vector3* velocity;
    tU32 flags;

    flags = pCharacter->field_0x14;
    if (flags & 4) {
        return 0x17;
    }
    if (flags == 0) {
        return 0x15;
    }
    if (flags & 1) {
        obj = pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
    } else {
        obj = PedGetRootObjectInlineMW(pCharacter);
    }
    obj->disable_move_rotate = 0;
    velocity = &pCharacter->field_0xd8;
    if (velocity != NULL) {
        obj->v.v[0] = velocity->v[0];
        obj->v.v[1] = velocity->v[1];
        obj->v.v[2] = velocity->v[2];
        for (child = obj->child; child != NULL; child = child->next) {
            SetObjectV(child, velocity);
        }
    }
    PHILMakeObjectActive(obj, NULL, NULL, 1);
    obj->field_0xed = 1;
    PHILSetObjectProperty(obj, 0, 1.0);
    PHILSetObjectProperty(obj, 3, 0);
    PHILSetObjectProperty(obj, 4, 0);
    PHILSetObjectProperty(obj, 5, 0);
    pCharacter->field_0x14 |= 4;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00409340
void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed) {
    br_matrix34* character_matrix;
    tPed_form* form;
    br_matrix34 inverse;

    form = pPed->personality->form;
    if (pPed->field_0x4 < 0) {
        character_matrix = &pPed->field_0x2c;
    } else if (pPed->field_0xe8 != NULL) {
        character_matrix = pPed->field_0xe8;
    } else {
        character_matrix = &form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34LPInverse(&inverse, &form->moves[pPed->field_0x7].move->frames[(short)pPed->field_0x1c].mat);
    BrMatrix34Mul(&pPed->field_0x8c, &inverse, character_matrix);
    pPed->field_0x8c.m[3][0] = 0.f;
    pPed->field_0x8c.m[3][1] = 0.f;
    pPed->field_0x8c.m[3][2] = 0.f;
}

static tPhysics_object* C2_HOOK_FASTCALL PedGetRootObjectInline(tPed_character_instance *pPed) {
    tPed_form* form;
    tPed_form_boned_phys* boned_phys;
    int i;

    form = pPed->personality->form;
    boned_phys = &form->boned_physicing[pPed->field_0x5];
    for (i = 0; i < form->count_bones; i++) {
        if (boned_phys->collision_infos[i]->shape != NULL) {
            return boned_phys->collision_infos[i];
        }
    }
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00409400
int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2) {
    tPed_personality* personality;
    tPed_form* form;
    tPhysics_object* obj;
    br_matrix34* character_matrix;
    br_matrix34 mat34;

    personality = pCharacter->personality;
    if (pCharacter->field_0x14 & 1) {
        obj = personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
    } else {
        obj = PedGetRootObjectInline(pCharacter);
    }

    form = personality->form;
    if (pCharacter->field_0x4 < 0) {
        character_matrix = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        character_matrix = pCharacter->field_0xe8;
    } else {
        character_matrix = &form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34LPInverse(&mat34, &form->moves[pCharacter->field_0x7].move->frames[(tS16)pCharacter->field_0x1c].mat);
    BrMatrix34Mul(&pCharacter->field_0x8c, &mat34, character_matrix);
    pCharacter->field_0x8c.m[3][0] = 0.f;
    pCharacter->field_0x8c.m[3][1] = 0.f;
    pCharacter->field_0x8c.m[3][2] = 0.f;
    SetCharacterBonePositions(pCharacter, 3, 0);
    pCharacter->field_0x14 &= ~4;

    if (gPed_vtable->active_halted != NULL) {
        if (gPed_vtable->active_halted((undefined4*)pCharacter, (undefined4*)obj) == 0) {
            pCharacter->field_0x14 |= 4;
            return 0;
        }
    }

    obj->disable_move_rotate = 1;
    pCharacter->field_0xd8.v[0] = obj->v.v[0];
    pCharacter->field_0xd8.v[1] = obj->v.v[1];
    pCharacter->field_0xd8.v[2] = obj->v.v[2];
    PHILSetObjectProperty(obj, 3, 1);
    PHILSetObjectProperty(obj, 4, 1);
    PHILSetObjectProperty(obj, 5, 1);
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

// FUNCTION: CARMA2_HW 0x004096f0
void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter) {

    if (pCharacter->field_0xbc != NULL) {
        pCharacter->field_0xbc->field_0x4 = 0;
        if (pCharacter->field_0xbc->field_0x2c != 0) {
            tPed_form* form = pCharacter->personality->form;
            br_matrix34* destination;
            tPed_move* move;
            tPed_move_frame* frame;
            br_matrix34 local;

            if (pCharacter->field_0x4 < 0) {
                destination = &pCharacter->field_0x2c;
            } else if (pCharacter->field_0xe8 != NULL) {
                destination = pCharacter->field_0xe8;
            } else {
                destination = &form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
            }

            move = form->moves[pCharacter->field_0x7].move;
            frame = &move->frames[(short)pCharacter->field_0x1c];
            BrMatrix34LPInverse(&local, &frame->mat);
            BrMatrix34Mul(&pCharacter->field_0x8c, &local, destination);

            pCharacter->field_0x8c.m[3][0] = 0.0f;
            pCharacter->field_0x8c.m[3][1] = 0.0f;
            pCharacter->field_0x8c.m[3][2] = 0.0f;
        }

        pCharacter->field_0xbc = NULL;
        SetCharacterBonePositions(pCharacter, 0, 0);
    }
}

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

// SetCharacterMove

// DRMatrix34RotateCos

/* SetCharacterDirection is implemented at the end of this file (0x0040a730) */

// PARTIAL 39.89%: register allocation (esi/edi swapped), stack frame 0x10 vs 0x18
// FUNCTION: CARMA2_HW 0x0040b0a0
void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pChar, br_vector3* pPosition, int pMode) {
    register tPed_character_instance* pCharacter = pChar;
    register br_vector3* pPos = pPosition;
    extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);
    tPed_personality* personality;
    tPed_move* move;
    br_matrix34* mat;
    br_vector3 target;
    float dx;
    float dz;
    float dy;
    int mode;
    int n;
    tPed_move_frame* frame;

    if ((pCharacter->field_0x14 & 4) && gAction_replay_mode == 0) {
        return;
    }
    personality = pCharacter->personality;
    if (pCharacter->field_0x7 < 0) {
        move = NULL;
    } else {
        move = personality->form->moves[pCharacter->field_0x7].move;
    }
    if ((tS16)pCharacter->field_0x1c < 0) {
        mode = -1;
    } else {
        mode = pMode;
    }
    if (mode == -1) {
        target.v[0] = pPos->v[0];
        target.v[1] = pPos->v[1];
        target.v[2] = pPos->v[2];
    } else if (mode == 0) {
        float below = FindYVerticallyBelow2(pPos);
        target.v[0] = pPos->v[0];
        target.v[2] = pPos->v[2];
        target.v[1] = personality->moves[pCharacter->field_0x7].grounding_offset + below;
    } else if (mode == 1) {
        target.v[2] = pPos->v[2];
        target.v[0] = pPos->v[0];
        target.v[1] = personality->moves[pCharacter->field_0x7].grounding_offset + pPos->v[1];
    }
    if (pCharacter->field_0x4 < 0) {
        mat = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        mat = pCharacter->field_0xe8;
    } else {
        mat = &personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    dx = 0.0f;
    dy = 0.0f;
    dz = 0.0f;
    if (move != NULL && move->field_0x2a == 0) {
        n = (tS16)pCharacter->field_0x1c;
        if (n > 0) {
            frame = &move->frames[n];
            do {
                dx = frame->mat.m[3][0] + dx;
                dy = dy + frame->mat.m[3][1];
                dz = frame->mat.m[3][2] + dz;
                frame--;
                n--;
            } while (n != 0);
        }
        if (!(move->looping_reset_flags & 1)) {
            dx = 0.0f;
        }
        if (!(move->looping_reset_flags & 2)) {
            dy = 0.0f;
        }
        if (!(move->looping_reset_flags & 4)) {
            dz = 0.0f;
        }
    } else {
        dz = 0.0f;
    }
    mat->m[3][0] = target.v[0];
    mat->m[3][1] = target.v[1];
    mat->m[3][2] = target.v[2];
    if (gAction_replay_mode == 0) {
        float scale = personality->moves[pCharacter->field_0x7].scale_factor;
        mat->m[3][1] = scale * dy + mat->m[3][1];
        mat->m[3][2] = scale * dz + mat->m[3][2];
        mat->m[3][0] = scale * dx + mat->m[3][0];
    }
    if ((pCharacter->field_0x14 & 1) && gAction_replay_mode == 0) {
        BrMatrix34Copy((br_matrix34*)((char*)pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info + 0x8c), mat);
    }
    SetCharacterBonePositions(pCharacter, 0, 0);
}

// DisposeClonedMaterials

// DisposeAllBonerData

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on ApexWorm/rec2 1bc72df97
// FUNCTION: CARMA2_HW 0x0040b420
int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set) {

    C2_HOOK_BUG_ON(REC2_ASIZE(pCharacter->personality->bones[0].models) != 4);

    if (pModel_index >= (int)REC2_ASIZE(pCharacter->personality->bones[0].models)
            || pBone_index >= pCharacter->personality->form->count_bones
            || pBone_index < 0
            || pCharacter->field_0x4 < 0) {
        return 0;
    }
    if (pModel_index < 0) {
        br_actor* actor;
        br_model* old_model;
        int i;

        actor = pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index];
        old_model = actor->model;
        for (i = 0; i < (int)REC2_ASIZE(pCharacter->personality->bones[pBone_index].models); i++) {
            if (pCharacter->personality->bones[pBone_index].models[i].models[pCharacter->field_0xa] == old_model) {
                break;
            }
        }
        if (i == (int)REC2_ASIZE(pCharacter->personality->bones[pBone_index].models)) {
            i = 0;
        }
        actor->model = pCharacter->personality->bones[pBone_index].models[i].models[pModel_set];
        pCharacter->field_0xa = (tS8)pModel_set;
    } else {
        br_model* model;
        br_actor* actor;

        pCharacter->field_0xa = (tS8)pModel_set;
        model = pCharacter->personality->bones[pBone_index].models[pModel_index].models[pCharacter->field_0xa];
        if (model == NULL) {
            return 0;
        }
        actor = pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index];
        actor->model = model;
    }
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SetCharacterBoneModelP

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040b530
void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, undefined4 pArg2, int pArg3) {
    int i;

    for (i = 0; i < pCharacter->personality->form->count_bones; i++) {
        SetCharacterBoneModel(pCharacter, i, pArg2, pArg3);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040b570
br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index) {

    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->model;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040b590
int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter) {

    return pCharacter->field_0xa;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x0040b760
int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter) {

    return BonerActiveHalted2(pCharacter, 0);
}

// DismemberCharacter

// FUNCTION: CARMA2_HW 0x0040bae0
float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum) {

    if (pMoveNum < 0 || pMoveNum > pCharacter->personality->form->count_moves) {
        BrFailure("MoveNum out of range in GetClearance()");
    }
    return pCharacter->personality->moves[pMoveNum].grounding_offset;
}

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040c490
void C2_HOOK_FASTCALL MungeAllCharacterMaterials(void (C2_HOOK_FASTCALL* pCallback)(void*)) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_personalities); i++) {
        if (gPed_personalities[i] != NULL) {
            pCallback(gPed_personalities[i]->bones[0].models[0].models[0]->faces->material);
        }
    }
    for (i = 0; i < gCount_pedestrian_personality_cloned_materials; i++) {
        pCallback(gPedestrian_character_cloned_materials[i]);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040c4e0
void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL* pCallback)(void*)) {
    int i;
    int j;
    int k;

    for (i = 0; i < (int)REC2_ASIZE(gPed_forms); i++) {
        if (gPed_forms[i] == NULL) {
            continue;
        }
        for (j = 0; j < gPed_forms[i]->max_boned_physicing_at_once; j++) {
            for (k = 0; k < gPed_forms[i]->count_bones; k++) {
                pCallback(gPed_forms[i]->boned_physicing[j].collision_infos[k]);
            }
        }
        for (k = 0; k < gPed_forms[i]->max_simple_physicing_at_once; k++) {
            pCallback(gPed_forms[i]->simple_physicing[k].collision_info);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0040c560
void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL* pCallback)(void*)) {
    int i;
    int j;
    int k;
    int l;

    for (i = 0; i < (int)REC2_ASIZE(gPed_personalities); i++) {
        if (gPed_personalities[i] == NULL) {
            continue;
        }
        for (j = 0; j < gPed_personalities[i]->form->count_bones; j++) {
            tPed_personality_bone* bone = &gPed_personalities[i]->bones[j];
            for (k = 0; k < 2; k++) {
                for (l = 0; l < 4; l++) {
                    if (bone->models[l].models[k] != NULL) {
                        pCallback(bone->models[l].models[k]);
                    }
                }
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040c600
void C2_HOOK_FASTCALL DisableOverallMovement(void) {

    gPed_overall_movement_disabled = 1;
}
// FUNCTION: CARMA2_HW 0x0040c610
void C2_HOOK_FASTCALL EnableOverallMovement(void) {

    gPed_overall_movement_disabled = 0;
}
// FUNCTION: CARMA2_HW 0x0040c620
void C2_HOOK_FASTCALL EnableRetainRootMode(void) {

    gPed_retain_root_mode = 1;
}

// FUNCTION: CARMA2_HW 0x0040c630
void C2_HOOK_FASTCALL DisableRetainRootMode(void) {

    gPed_retain_root_mode = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_FASTCALL DisposePedPersonality(tPed_personality* pPersonality);
extern void C2_HOOK_FASTCALL DisposePedForm(tPed_form* pForm);
// PARTIAL 99.99%: DisposeCharacterInstances 0x0040b2e0 - one instruction encodes the frame-axis
// load as [eax+edi*1+0x30] where the original has [edi+eax*1+0x30] (same address, SIB base/index
// swapped); every other instruction pairs.
// FUNCTION: CARMA2_HW 0x0040b2e0
void C2_HOOK_FASTCALL DisposeCharacterInstances(void) {
    tPed_remap* remap;
    tPed_personality* personality;
    tPed_move* move;
    tPed_form* form;
    int i;
    int j;

    for (i = 0; i < 10; i++) {
        remap = gPed_remaps[i];
        if (remap != NULL) {
            if (remap->bones != NULL) {
                BrMemFree(remap->bones);
            }
            BrMemFree(remap);
            gPed_remaps[i] = NULL;
        }
    }
    for (i = 0; i < 50; i++) {
        personality = gPed_personalities[i];
        if (personality != NULL) {
            DisposePedPersonality(personality);
            gPed_personalities[i] = NULL;
        }
    }
    for (i = 0; i < 250; i++) {
        if (gPed_moves[i] != NULL) {
            move = gPed_moves[i];
            for (j = 0; j < move->count_frames; j++) {
                if (move->frames[j].axis != NULL) {
                    BrMemFree(move->frames[j].axis);
                }
            }
            if (move->frames != NULL) {
                BrMemFree(move->frames);
            }
            if (move != NULL) {
                BrMemFree(move);
            }
            gPed_moves[i] = NULL;
        }
    }
    for (i = 0; i < 20; i++) {
        form = gPed_forms[i];
        if (form != NULL) {
            DisposePedForm(form);
            gPed_forms[i] = NULL;
        }
    }
    for (i = 0; i < 20; i++) {
        gPed_morph_slots[i].field_0x0 = 0;
    }

    for (i = 0; i < gCount_pedestrian_personality_cloned_materials; i++) {
        BrMaterialRemove(gPedestrian_character_cloned_materials[i]);
        BrMaterialFree(gPedestrian_character_cloned_materials[i]);
    }
    gCount_pedestrian_personality_cloned_materials = 0;
    gPed_overall_movement_disabled = 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);
extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);

// (GPL-3.0), reworked after the disassembly
// FUNCTION: CARMA2_HW 0x0040a050
int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6) {
    tPed_personality* personality;
    tPed_form* form;
    tPed_move* original_ped_move;
    tPed_move* ped_move;
    tPed_move_frame* ped_frame;
    br_matrix34* character_matrix;
    br_matrix34 inverse;
    int move_id;
    int move_flag;
    int original_field_0x7;
    int i;
    int speed;
    short original_speed;
    float sx;
    float sy;
    float sz;
    float* p;

    personality = pCharacter->personality;
    form = personality->form;
    original_field_0x7 = pCharacter->field_0x7;
    original_ped_move = NULL;
    if (original_field_0x7 >= 0) {
        original_ped_move = form->moves[pCharacter->field_0x7].move;
    }
    if (pCharacter->field_0x4 < 0) {
        character_matrix = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        character_matrix = pCharacter->field_0xe8;
    } else {
        character_matrix = &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    move_flag = pMove_action & 0x40000000;
    move_id = pMove_action & ~0x40000000;
    if (move_id >= 0) {
        for (i = 0; i < form->count_moves; i++) {
            if (form->moves[i].id == move_id) {
                if (pArg4 < 0) {
                    pCharacter->field_0x8 = (tS8)i;
                    if (pArg5 != 0) {
                        pCharacter->field_0x9 |= 0x4;
                    } else {
                        pCharacter->field_0x9 &= ~0x4;
                    }
                } else {
                    pCharacter->field_0x7 = (tS8)i;
                    if (pArg5 != 0) {
                        pCharacter->field_0x9 |= 0x2;
                    } else {
                        pCharacter->field_0x9 &= ~0x2;
                    }
                    pCharacter->field_0x8 = -1;
                }
                break;
            }
        }
        if (i == form->count_moves) {
            return 2;
        }
    }
    original_speed = pCharacter->field_0x1e;
    speed = (int)((pSpeed >= 0.f) ? pSpeed : -((float)form->moves[pCharacter->field_0x7].move->frametime * pSpeed));
    if (pArg4 >= 0) {
        pCharacter->field_0x1e = (undefined2)speed;
    } else {
        pCharacter->field_0x20 = (undefined2)speed;
    }
    if (original_field_0x7 >= 0) {
        if ((form->moves[original_field_0x7].id == move_id && move_flag == 0) || move_id < 0 || pArg4 < 0) {
            return 1;
        }
    }
    ped_move = personality->form->moves[pCharacter->field_0x7].move;
    sx = 0.f;
    sy = 0.f;
    sz = 0.f;
    if (original_ped_move != NULL && original_ped_move->field_0x2a == 0 && !gPed_overall_movement_disabled) {
        for (i = (tS16)pCharacter->field_0x1c; i > 0; i--) {
            sx += original_ped_move->frames[i].mat.m[3][0];
            sy += original_ped_move->frames[i].mat.m[3][1];
            sz += original_ped_move->frames[i].mat.m[3][2];
        }
    }
    if (original_ped_move != NULL) {
        ped_frame = &original_ped_move->frames[(tS16)pCharacter->field_0x1c];
    }
    pCharacter->field_0x1c = 0;
    pCharacter->field_0x24 = 0;
    pCharacter->field_0x18 = 0;
    pCharacter->field_0x8 = -1;
    if (pCharacter->field_0xbc != NULL) {
        pCharacter->field_0xbc->field_0x4 = 0.f;
        pCharacter->field_0xbc = NULL;
    }
    if (pArg4 != 0 && original_ped_move != NULL && pArg6 != 0) {
        for (i = 0; i < 20; i++) {
            if (((tPed_morph*)((char*)gPed_morph_slots - 4))[i].field_0x4 == 0.f) {
                pCharacter->field_0xbc = &((tPed_morph*)((char*)gPed_morph_slots - 4))[i];
                break;
            }
        }
    }
    if (pCharacter->field_0xbc == NULL) {
        pArg4 = 0;
    }
    if (pArg4 != 0) {
        br_euler e;
        float sum;
        int i;

        sum = 0.f;
        pCharacter->field_0xbc->field_0x20 = original_ped_move->field_0x40;
        if (original_speed != 0 && original_ped_move->field_0x2a == 0) {
            pCharacter->field_0xbc->field_0x8.v[0] = pCharacter->field_0xbc->field_0x20.v[0] / (float)(original_ped_move->count_frames * original_speed);
            pCharacter->field_0xbc->field_0x8.v[1] = pCharacter->field_0xbc->field_0x20.v[1] / (float)(original_ped_move->count_frames * original_speed);
            pCharacter->field_0xbc->field_0x8.v[2] = pCharacter->field_0xbc->field_0x20.v[2] / (float)(original_ped_move->count_frames * original_speed);
        } else {
            BrVector3Set(&pCharacter->field_0xbc->field_0x8, 0.f, 0.f, 0.f);
        }
        if ((short)speed != 0 && ped_move->field_0x2a == 0) {
            pCharacter->field_0xbc->field_0x14.v[0] = ped_move->field_0x34.v[0] / (float)(ped_move->count_frames * (short)speed);
            pCharacter->field_0xbc->field_0x14.v[1] = ped_move->field_0x34.v[1] / (float)(ped_move->count_frames * (short)speed);
            pCharacter->field_0xbc->field_0x14.v[2] = ped_move->field_0x34.v[2] / (float)(ped_move->count_frames * (short)speed);
        } else {
            BrVector3Set(&pCharacter->field_0xbc->field_0x14, 0.f, 0.f, 0.f);
        }
        e.order = 0;
        BrMatrix34ToEuler(&e, &ped_frame->mat);
        pCharacter->field_0xbc->field_0x2d[0][0] = e.a / 256;
        pCharacter->field_0xbc->field_0x2d[0][1] = e.b / 256;
        pCharacter->field_0xbc->field_0x2d[0][2] = e.c / 256;
        BrMatrix34ToEuler(&e, &ped_move->frames[0].mat);
        pCharacter->field_0xbc->field_0x2d[0][3] = e.a / 256 - pCharacter->field_0xbc->field_0x2d[0][0];
        pCharacter->field_0xbc->field_0x2d[0][4] = e.b / 256 - pCharacter->field_0xbc->field_0x2d[0][1];
        pCharacter->field_0xbc->field_0x2d[0][5] = e.c / 256 - pCharacter->field_0xbc->field_0x2d[0][2];
        for (i = 1; i < form->count_bones; i++) {
            pCharacter->field_0xbc->field_0x2d[i][0] = ped_frame->axis[i].euler1;
            pCharacter->field_0xbc->field_0x2d[i][1] = ped_frame->axis[i].euler2;
            pCharacter->field_0xbc->field_0x2d[i][2] = ped_frame->axis[i].euler3;
            pCharacter->field_0xbc->field_0x2d[i][3] = ped_move->frames->axis[i].euler1 - ped_frame->axis[i].euler1;
            pCharacter->field_0xbc->field_0x2d[i][4] = ped_move->frames->axis[i].euler2 - ped_frame->axis[i].euler2;
            pCharacter->field_0xbc->field_0x2d[i][5] = ped_move->frames->axis[i].euler3 - ped_frame->axis[i].euler3;
            sum += (float)(abs((signed char)pCharacter->field_0xbc->field_0x2d[i][3])
                + abs((signed char)pCharacter->field_0xbc->field_0x2d[i][4])
                + abs((signed char)pCharacter->field_0xbc->field_0x2d[i][5]));
        }
        pCharacter->field_0xbc->field_0x2c = (tU8)(((tU32)pArg4 >> 30) & 1);
        pArg4 &= ~0x40000000;
        pCharacter->field_0xbc->field_0x0 = 0.f;
        pCharacter->field_0xbc->field_0x4 = (float)(1.0 / (sum / ((double)form->count_bones * 3.0) * pArg4 * 0.015625));
        pCharacter->field_0x24 = 0;
    }
    if (gPed_retain_root_mode) {
        if (pCharacter->field_0x4 < 0) {
            character_matrix = &pCharacter->field_0x2c;
        } else if (pCharacter->field_0xe8 != NULL) {
            character_matrix = pCharacter->field_0xe8;
        } else {
            character_matrix = &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
        }
        BrMatrix34LPInverse(&inverse, &pCharacter->personality->form->moves[pCharacter->field_0x7].move->frames[pCharacter->field_0x1c].mat);
        BrMatrix34Mul(&pCharacter->field_0x8c, &inverse, character_matrix);
        pCharacter->field_0x8c.m[3][0] = 0.f;
        pCharacter->field_0x8c.m[3][1] = 0.f;
        pCharacter->field_0x8c.m[3][2] = 0.f;
        SetCharacterBonePositions(pCharacter, 3, 0);
        return 0;
    }
    if (personality->moves[pCharacter->field_0x7].grounding_mode == ePed_personality_grounded) {
        p = character_matrix->m[3];
        p[1] = pCharacter->personality->moves[pCharacter->field_0x7].grounding_offset + FindYVerticallyBelow2((br_vector3*)p);
        p[0] = p[0];
        p[2] = p[2];
    }
    SetCharacterBonePositions(pCharacter, 0, 0);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9);

// PARTIAL 41.51%: regalloc/frame-size differences (register swap ebx/edx, locals layout)
// FUNCTION: CARMA2_HW 0x004097b0
int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2) {
    float time;
    br_matrix34 inverse;
    tPed_form* form;
    tPed_personality* personality;
    br_matrix34* mat;
    int i;
    tPed_form_actor_set* actor_set;

    time = (float)pArg2;
    pPed->field_0xbc->field_0x0 = pPed->field_0xbc->field_0x4 * time + pPed->field_0xbc->field_0x0;
    if (pPed->field_0xbc->field_0x0 >= 1.0f) {
        if (pPed->field_0xbc != NULL) {
            pPed->field_0xbc->field_0x4 = 0.f;
            if (*(tU8*)&pPed->field_0xbc->field_0x2c != 0) {
                form = pPed->personality->form;
                if (pPed->field_0x4 < 0) {
                    mat = &pPed->field_0x2c;
                } else {
                    mat = pPed->field_0xe8;
                    if (mat == NULL) {
                        mat = &form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
                    }
                }
                BrMatrix34LPInverse(&inverse, &form->moves[pPed->field_0x7].move->frames[(tS16)pPed->field_0x1c].mat);
                BrMatrix34Mul(&pPed->field_0x8c, &inverse, mat);
                pPed->field_0x8c.m[3][0] = 0.f;
                pPed->field_0x8c.m[3][1] = 0.f;
                pPed->field_0x8c.m[3][2] = 0.f;
            }
            pPed->field_0xbc = NULL;
            SetCharacterBonePositions(pPed, 0, 0);
        }
        return 0;
    }
    personality = pPed->personality;
    form = personality->form;
    if (pPed->field_0x4 < 0) {
        actor_set = NULL;
    } else {
        actor_set = &form->actor_sets[pPed->field_0x4];
    }
    for (i = 0; i < form->count_bones; i++) {
        int parent_index = form->bones[i].indices[0];

        if (parent_index >= 0) {
            if (!(gPow2_array[i] & pPed->field_0xc) && !(gPow2_array[parent_index] & pPed->field_0xc) && actor_set != NULL) {
                br_matrix34* parent_mat;
                tS8* b = (tS8*)pPed->field_0xbc->field_0x2d[i];

                if (pPed->field_0xe8 != NULL) {
                    parent_mat = &pPed->field_0xe8[i];
                } else {
                    parent_mat = &actor_set->actors[parent_index]->t.t.mat;
                }
                if (pPed->field_0xe8 != NULL) {
                    mat = &pPed->field_0xe8[i];
                } else {
                    mat = &actor_set->actors[i]->t.t.mat;
                }
                SetBoner(pPed, mat, parent_mat, &pPed->field_0x8c,
                    (tU8)(int)(b[3] * pPed->field_0xbc->field_0x0 + b[0]),
                    (int)(b[4] * pPed->field_0xbc->field_0x0 + b[1]),
                    (int)(b[5] * pPed->field_0xbc->field_0x0 + b[2]),
                    &personality->bones[i].field_0x20, &personality->bones[i].field_0x2c);
            }
        } else {
            if (!(pPed->field_0x14 & 0x4) && *(tU8*)&pPed->field_0xbc->field_0x2c == 0) {
                tS8* b = (tS8*)pPed->field_0xbc->field_0x2d[i];
                br_matrix34 rot;
                br_vector3 original_pos;
                br_vector3 vec;
                br_vector3 delta;
                float t;
                tPed_morph* morph;

                if (pPed->field_0x4 < 0) {
                    mat = &pPed->field_0x2c;
                } else if (pPed->field_0xe8 == NULL) {
                    mat = &form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
                } else {
                    mat = pPed->field_0xe8;
                }
                BrVector3Copy(&original_pos, (br_vector3*)mat->m[3]);
                MakeRotationMatrix(&rot,
                    (tU8)(int)(b[3] * pPed->field_0xbc->field_0x0 + b[0]),
                    (int)(b[4] * pPed->field_0xbc->field_0x0 + b[1]),
                    (int)(b[5] * pPed->field_0xbc->field_0x0 + b[2]));
                BrMatrix34Mul(mat, &rot, &pPed->field_0x8c);
                morph = pPed->field_0xbc;
                t = 1.0f - morph->field_0x0;
                vec.v[0] = (morph->field_0x14.v[0] * morph->field_0x0 + morph->field_0x8.v[0] * t) * time;
                vec.v[1] = (morph->field_0x14.v[1] * morph->field_0x0 + morph->field_0x8.v[1] * t) * time;
                vec.v[2] = (morph->field_0x14.v[2] * morph->field_0x0 + morph->field_0x8.v[2] * t) * time;
                BrMatrix34ApplyV(&delta, &vec, &pPed->field_0x5c);
                mat->m[3][0] = delta.v[0] + original_pos.v[0];
                mat->m[3][1] = delta.v[1] + original_pos.v[1];
                mat->m[3][2] = delta.v[2] + original_pos.v[2];
            }
        }
    }
    return 1;
}

// PARTIAL 38.29%: SetBoner 0x407e70 [x87] - original keeps per-operand fchs (MSVC5 here factors -a+-b+-c into -(a+b+c)) so the x87 block differs
// The original's gMutant_speed branch calls FUN_004d6c10 (0x004d6c10), which does not exist in rec2 yet.
// The call is omitted: a stub call in this TU drops BonerPedMovedByPhysics (0x0040b770) from 100% to 80%;
// an empty stub is a no-op, so behaviour is unchanged. Restore the call once FUN_004d6c10 is implemented.
// FUNCTION: CARMA2_HW 0x00407e70
void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9) {
    MakeRotationMatrix(pMat, pAngle_x, pAngle_y, pAngle_z);
    if (pArg4 != NULL) {
        pMat->m[3][0] = -pArg8->v[0] * pMat->m[0][0] + -pArg8->v[2] * pMat->m[2][0] + -pArg8->v[1] * pMat->m[1][0];
        pMat->m[3][1] = -pArg8->v[0] * pMat->m[0][1] + -pArg8->v[1] * pMat->m[1][1] + -pArg8->v[2] * pMat->m[2][1];
        pMat->m[3][2] = -pArg8->v[0] * pMat->m[0][2] + -pArg8->v[2] * pMat->m[2][2] + -pArg8->v[1] * pMat->m[1][2];
        BrMatrix34Post(pMat, pArg4);
        pMat->m[3][0] = pParg9->v[0] * pParent_mat->m[0][0] + pParg9->v[2] * pParent_mat->m[2][0] + pParg9->v[1] * pParent_mat->m[1][0] + pParent_mat->m[3][0] + pMat->m[3][0];
        pMat->m[3][1] = pParent_mat->m[0][1] * pParg9->v[0] + pParent_mat->m[1][1] * pParg9->v[1] + pParg9->v[2] * pParent_mat->m[2][1] + pParent_mat->m[3][1] + pMat->m[3][1];
        pMat->m[3][2] = pParent_mat->m[0][2] * pParg9->v[0] + pParent_mat->m[1][2] * pParg9->v[1] + pParg9->v[2] * pParent_mat->m[2][2] + pParent_mat->m[3][2] + pMat->m[3][2];
        return;
    }
    pMat->m[3][0] = pParg9->v[0] * pParent_mat->m[0][0] + pParg9->v[2] * pParent_mat->m[2][0] + pParg9->v[1] * pParent_mat->m[1][0] + -pArg8->v[0] * pMat->m[0][0] + -pArg8->v[2] * pMat->m[2][0] + -pArg8->v[1] * pMat->m[1][0] + pParent_mat->m[3][0];
    pMat->m[3][1] = -pArg8->v[0] * pMat->m[0][1] + -pArg8->v[1] * pMat->m[1][1] + -pArg8->v[2] * pMat->m[2][1] + pParent_mat->m[0][1] * pParg9->v[0] + pParent_mat->m[1][1] * pParg9->v[1] + pParg9->v[2] * pParent_mat->m[2][1] + pParent_mat->m[3][1];
    pMat->m[3][2] = pParent_mat->m[0][2] * pParg9->v[0] + pParg9->v[1] * pParent_mat->m[1][2] + pParg9->v[2] * pParent_mat->m[2][2] + -pArg8->v[0] * pMat->m[0][2] + -pArg8->v[2] * pMat->m[2][2] + -pArg8->v[1] * pMat->m[1][2] + pParent_mat->m[3][2];
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// PARTIAL 47.32%: regalloc (original: move_frame in ebx and i/zero in ebp, frame 0x4c; ours swapped, frame 0x48),
// mat->m[3] pointer folding and scale-factor reload pattern differ. Reference: upstream pedestrn.c (GPL-3.0).
// FUNCTION: CARMA2_HW 0x00407b30
void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3) {
    tPed_personality *personality;
    tPed_form_actor_set* actor_set;
    tPed_form* form;
    int i;
    tPed_move_frame* ped_move_frame;

    if (pPed->field_0x7 < 0) {
        return;
    }
    if ((pPed->field_0x14 & 0x4) && (pPed->field_0x14 & 0x2) && pPed->field_0xe8 == NULL) {
        return;
    }
    pPed->field_0x9 &= ~0x1;
    if (pPed->field_0xbc != NULL) {
        MorphCharacterBonePositions(pPed, 0);
        return;
    }
    personality = pPed->personality;
    form = personality->form;
    if (pPed->field_0x4 >= 0) {
        actor_set = &form->actor_sets[pPed->field_0x4];
    } else {
        actor_set = NULL;
    }
    ped_move_frame = &form->moves[pPed->field_0x7].move->frames[(tS16)pPed->field_0x1c];
    for (i = 0; i < form->count_bones; i++) {
        int parent_index = form->bones[i].indices[0];

        if (parent_index >= 0) {
            if (!(gPow2_array[i] & pPed->field_0xc) && !(gPow2_array[parent_index] & pPed->field_0xc)
                    && pArg2 != 1 && pArg2 != 2 && actor_set != NULL) {
                br_matrix34* mat;
                br_matrix34* parent_mat;
                tPed_move_frame_axis* axis;

                if (pPed->field_0xe8 != NULL) {
                    mat = &pPed->field_0xe8[i];
                    parent_mat = &pPed->field_0xe8[parent_index];
                } else {
                    mat = &actor_set->actors[i]->t.t.mat;
                    parent_mat = &actor_set->actors[parent_index]->t.t.mat;
                }
                axis = &ped_move_frame->axis[i];

                SetBoner(pPed, mat, parent_mat, &pPed->field_0x8c, axis->euler1, axis->euler2, axis->euler3,
                    &personality->bones[i].field_0x20, &personality->bones[i].field_0x2c);
            }
        } else {
            if (pArg2 != 3 && !(pPed->field_0x14 & 0x4)) {
                br_matrix34* mat;
                br_vector3 original_pos;

                if (pPed->field_0x4 < 0) {
                    mat = &pPed->field_0x2c;
                } else if (pPed->field_0xe8 == NULL) {
                    mat = &pPed->personality->form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
                } else {
                    mat = pPed->field_0xe8;
                }
                BrVector3Copy(&original_pos, (br_vector3*)mat->m[3]);
                if (pArg2 != 2) {
                    BrMatrix34Mul(mat, &ped_move_frame->mat, &pPed->field_0x8c);
                }
                if (pArg3 == 0 || gPed_overall_movement_disabled) {
                    BrVector3Copy((br_vector3*)mat->m[3], &original_pos);
                } else {
                    br_vector3 move;
                    br_vector3 delta;

                    BrVector3Scale(&move, (br_vector3*)ped_move_frame->mat.m[3], personality->moves[pPed->field_0x7].scale_factor);
                    BrMatrix34ApplyV(&delta, &move, &pPed->field_0x5c);
                    if (gAction_replay_mode && ARGetReplayRate() < 0) {
                        BrVector3Sub((br_vector3*)mat->m[3], &original_pos, &delta);
                    } else {
                        BrVector3Add((br_vector3*)mat->m[3], &original_pos, &delta);
                    }
                }
            }
        }
    }
}

/* not inlined: ScoreForKilledPedestrian/SetPedMove keep their call 0x004c8b30 */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c8b30
void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13) {
    tU32 bits;
    int flag;

    (void)pArg6;
    flag = 0;
    if (pArg7) {
        flag = 1;
    }
    memcpy(&bits, &pArg5, sizeof(bits));
    gUNK_0069410c = bits;
    ARDoSingleVariedSession(ePipe_chunk_ped_move, (uintptr_t)pPed, 12,
        2, 2, pOriginal_move_id,
        2, 0, pMove_id,
        2, 6, pArg2,
        4, 12, gUNK_0069410c,
        4, 8, gUNK_0069410c,
        2, 4, flag,
        4, 16, pArg8,
        12, 20, pOriginal_pos,
        12, 32, pPos,
        4, 44, pOriginal_action,
        4, 48, pAction,
        48, 52, pArg13);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);

// FUNCTION: CARMA2_HW 0x0040b620
int C2_HOOK_FASTCALL BonerPassiveCollision(tPed_character_instance* pCharacter, undefined4 pArg2) {
    tPhysics_object* child;
    tPhysics_object* obj;

    if (pCharacter->field_0x14 == 0) {
        return 1;
    }
    if (pCharacter->field_0x14 & 4) {
        obj = NULL;
    } else {
        if (pCharacter->field_0x14 & 1) {
            obj = pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
        } else {
            tPed_form_boned_phys* bp = &pCharacter->personality->form->boned_physicing[pCharacter->field_0x5];
            int i;

            obj = NULL;
            for (i = 0; i < pCharacter->personality->form->count_bones; i++) {
                if (bp->collision_infos[i]->shape != NULL) {
                    obj = bp->collision_infos[i];
                    break;
                }
            }
        }

        obj->disable_move_rotate = 0;
        {
            br_vector3* field_0xd8 = &pCharacter->field_0xd8;

            if (field_0xd8 != NULL) {
                obj->v.v[0] = field_0xd8->v[0];
                obj->v.v[1] = field_0xd8->v[1];
                obj->v.v[2] = field_0xd8->v[2];
                {
                    child = obj->child;
                    while (child != NULL) {
                        SetObjectV(child, field_0xd8);
                        child = child->next;
                    }
                }
            }
        }

        PHILMakeObjectActive(obj, NULL, NULL, 1);
        obj->field_0xed = 1;
        PHILSetObjectProperty(obj, 0, 1.0);
        PHILSetObjectProperty(obj, 3, 0);
        PHILSetObjectProperty(obj, 4, 0);
        PHILSetObjectProperty(obj, 5, 0);
        pCharacter->field_0x14 |= 4;
    }
    pCharacter->field_0x14 |= 4;
    if (gPed_vtable->passive_collision != NULL) {
        gPed_vtable->passive_collision((undefined4*)pCharacter, (undefined4)obj, (undefined4*)pArg2);
    }
    return 0;
}

// gFloat_0058aec0 is the constant 0.0f used when no random ranges are supplied
extern float gFloat_0058aec0;
extern float gFloat_0058aec4;
extern double gDouble_0058aec8;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004eb6a0
void C2_HOOK_FASTCALL FUN_004eb6a0(br_matrix34* pMatrix) {
    br_vector3 v;
    br_scalar _scale;

    pMatrix->m[3][0] = 0;
    pMatrix->m[3][1] = 0;
    pMatrix->m[3][2] = 0;
    pMatrix->m[1][0] = FRandomPosNeg(1.0f);
    pMatrix->m[1][1] = FRandomPosNeg(1.0f);
    pMatrix->m[1][2] = FRandomPosNeg(1.0f);
    pMatrix->m[2][0] = FRandomPosNeg(1.0f);
    pMatrix->m[2][1] = FRandomPosNeg(1.0f);
    pMatrix->m[2][2] = FRandomPosNeg(1.0f);
    _scale = BR_LENGTH3(pMatrix->m[2][0], pMatrix->m[2][1], pMatrix->m[2][2]);
    if (_scale > gFloat_0058aec4) {
        _scale = (br_scalar)(gDouble_0058aec8 / _scale);
        v.v[0] = pMatrix->m[2][0] * _scale;
        v.v[1] = pMatrix->m[2][1] * _scale;
        v.v[2] = pMatrix->m[2][2] * _scale;
    } else {
        v.v[0] = 1.0f;
        v.v[1] = 0.0f;
        v.v[2] = 0.0f;
    }
    BrVector3Copy((br_vector3*)&pMatrix->m[2], &v);
    BrVector3Cross((br_vector3*)&pMatrix->m[0], (br_vector3*)&pMatrix->m[1], (br_vector3*)&pMatrix->m[2]);
    _scale = BR_LENGTH3(pMatrix->m[0][0], pMatrix->m[0][1], pMatrix->m[0][2]);
    if (_scale > gFloat_0058aec4) {
        _scale = (br_scalar)(gDouble_0058aec8 / _scale);
        pMatrix->m[0][0] = pMatrix->m[0][0] * _scale;
        pMatrix->m[0][1] = pMatrix->m[0][1] * _scale;
        pMatrix->m[0][2] = pMatrix->m[0][2] * _scale;
    } else {
        pMatrix->m[0][0] = 1.0f;
        pMatrix->m[0][1] = 0.0f;
        pMatrix->m[0][2] = 0.0f;
    }
    BrVector3Cross((br_vector3*)&pMatrix->m[1], (br_vector3*)&pMatrix->m[2], (br_vector3*)&pMatrix->m[0]);
    if (BrVector3LengthSquared((br_vector3*)&pMatrix->m[1]) < 0.9) {
        BrMatrix34Identity(pMatrix);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 99.99%: PositionFragment 0x4eb4a0 - sphere and actor cases and the whole frame are
// byte-identical; the 4 remaining insns are pure x87/loop scheduling in the BoxClumped case
// (`add edi,4` vs `fmul st,st(1)` order and the fld/fadd operand order of the translate add).
// The box value needs the named `b` temporary to get `fld` + `fmul st,st(1)` instead of `fmul [edi]`.
// FUNCTION: CARMA2_HW 0x004eb4a0
float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8) {
    int i;
    float local_1c;
    br_vector3 local_c;
    br_vector3 transformed_vector;
    br_vector3* pPos;
    float f1;
    float f2;
    float f3;
    float f4;
    float f5;

    local_1c = 1.f;
    switch (pPosition->type) {
    case kInitialSmashablePosition_SphereClumped:
        FUN_004eb6a0(&pActor->t.t.mat);
        pPos = (br_vector3*)pArg5;
        if (pPosition->position.sphere.where != 0) {
            pPos = (br_vector3*)pArg4;
        }
        pActor->t.t.translate.t = *pPos;
        f1 = FRandomPosNeg(1.f);
        f2 = FRandomPosNeg(1.f);
        f3 = FRandomPosNeg(1.f);
        f4 = FRandomBetween(0.f, pPosition->position.sphere.radius);
        local_1c = f4 / pPosition->position.sphere.radius;
        f5 = f4 / (float)sqrt(f1 * f1 + f2 * f2 + f3 * f3);
        pActor->t.t.translate.t.v[0] = f5 * f1 + pPos->v[0];
        pActor->t.t.translate.t.v[1] = f5 * f2 + pPos->v[1];
        pActor->t.t.translate.t.v[2] = f5 * f3 + pPos->v[2];
        break;
    case kInitialSmashablePosition_BoxClumped:
        FUN_004eb6a0(&pActor->t.t.mat);
        for (i = 0; i < 3; i++) {
            float r;
            float b;

            if (pArg7 != NULL) {
                r = FRandomBetween(((float*)pArg7)[i], ((float*)pArg7)[i + 3]);
            } else {
                r = gFloat_0058aec0;
            }
            b = pPosition->position.box.v[i];
            local_c.v[i] = b * r;
        }
        BrMatrix34ApplyV(&transformed_vector, &local_c, &((br_actor*)pArg6)->t.t.mat);
        pActor->t.t.translate.t.v[0] = transformed_vector.v[0] + ((br_vector3*)pArg4)->v[0];
        pActor->t.t.translate.t.v[1] = transformed_vector.v[1] + ((br_vector3*)pArg4)->v[1];
        pActor->t.t.translate.t.v[2] = transformed_vector.v[2] + ((br_vector3*)pArg4)->v[2];
        break;
    case kInitialSmashablePosition_ActorBased:
        if (pArg8 != 0) {
            pActor->t.t.mat = ((br_actor*)pArg3)->t.t.mat;
            BrMatrix34Post(&pActor->t.t.mat, (br_matrix34*)pArg8);
        } else {
            pActor->t.t.mat = ((br_actor*)pArg3)->t.t.mat;
            BrMatrix34Post(&pActor->t.t.mat, &((br_actor*)pArg6)->t.t.mat);
        }
        break;
    }
    return local_1c;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004cccc0
void C2_HOOK_FASTCALL DoBloodSpurt(uintptr_t pArg1, int pArg2, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {
    int saved_random_seed;
    int i;
    tSmash_vertex* fragment;
    float scale;

    saved_random_seed = rand();
    srand(pArg3);
    for (i = 0; i < gPed_spurty_lumps_count; i++) {
        fragment = FindShrapnelFragment(gPed_spurt_model, pArg1, 1);
        if (fragment == NULL) {
            break;
        }
        scale = PositionFragment(fragment->actor, &gPed_spurt_initial_position, 0, (int)pArg4, (int)pArg4, 0, NULL, 0);
        InitFragmentSpeed(fragment, &gPed_spurt_initial_speed, pArg5, scale, gPed_spurt_min_max_time, pArg2, pArg6, 0, 0);
    }
    srand(saved_random_seed);
}

extern br_vector3 gZero_vector__smash;

// based on REA pseudocode 0x4ccb10 and // FUNCTION: CARMA2_HW 0x004ccb10
void C2_HOOK_FASTCALL DoGibShower(volatile int pId, volatile int pTime, int pSeed, float pForce, int pSize_index, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    int old_rand;
    int i;
    int j;
    int max_instances;
    tPed_giblet_size_spec* spec;

    old_rand = rand();
    srand(pSeed);
    spec = &gGiblet_sizes[pSize_index];
    for (i = pSize_index; i < gGiblet_size_count; i++, spec++) {
        if (pForce < spec->min_force) {
            continue;
        }
        max_instances = (int)((pForce - spec->min_force) / spec->instance_force - -1.0f);
        if (max_instances > spec->max_count) {
            max_instances = spec->max_count;
        }
        for (j = 0; j < max_instances; j++) {
            tSmash_vertex* fragment;
            float v;

            fragment = FindShrapnelFragment(spec->models[IRandomBetween(0, spec->model_count - 1)], pTime, 2);
            if (fragment == NULL) {
                break;
            }
            v = PositionFragment(fragment->actor, &gGiblet_initial_position, 0, (int)pArg6, (int)pArg7, 0, 0, 0);
            InitFragmentSpeed(fragment, &gGiblet_initial_speed, pArg8, v, gGiblet_min_max_time, pTime, &gZero_vector__smash, 0, 0);
        }
    }
    srand(old_rand);
}

extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);
extern void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor);

// GLOBAL: CARMA2_HW 0x0065e5b8
const float gFlamed_ped_flame_scales[7] = {
    .275f,
    .2245f,
    .2f,
    .175f,
    .15f,
    .075f,
    .05f,
};

static __inline void MungeNapalm_KillBolt(tNapalm_bolt* pBolt) {
    int j;

    pBolt->ped->field_0x0c->field_0x98 = 0;
    for (j = 0; j < (int)REC2_ASIZE(pBolt->actors); j++) {
        pBolt->actors[j]->render_style = BR_RSTYLE_NONE;
    }
    pBolt->ped = NULL;
}

// (reference only)
// FUNCTION: CARMA2_HW 0x004ce7d0
void C2_HOOK_FASTCALL MungeNapalm(void) {
    tU32 the_time;
    int i;
    tNapalm_bolt* bolt;

    the_time = GetTotalTime();
    ARStartPipingSession(ePipe_chunk_burning_ped);
    bolt = gNapalm_bolts;
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++, bolt++) {
        int bolt_hit_target;

        if (bolt->ped == NULL) {
            continue;
        }
        bolt_hit_target = 0;
        if (gAction_replay_mode) {
            bolt_hit_target = 1;
        } else {
            if (!(bolt->ped->flags & 0x1)) {
                MungeNapalm_KillBolt(bolt);
            } else {
                tU32 delta_time;
                tPhysics_object* object;
                float dt;
                br_vector3 delta_move;
                float delta_length_sq;

                delta_time = the_time - bolt->field_0x28;
                if (the_time - bolt->field_0x24 > 50) {
                    memmove(&bolt->field_0x2c[1], &bolt->field_0x2c[0], (REC2_ASIZE(bolt->field_0x2c) - 1) * sizeof(br_vector3));
                    BrVector3Sub(&bolt->field_0x2c[0], &bolt->ped->pos, &bolt->actors[0]->t.t.translate.t);
                    BrVector3Normalise(&bolt->field_0x2c[0], &bolt->field_0x2c[0]);
                    BrVector3Scale(&bolt->field_0x2c[0], &bolt->field_0x2c[0], FRandomBetween(.8f, 1.25f));
                    if (delta_time < 500) {
                        bolt->field_0x2c[0].v[1] += (float)(500 - delta_time) * 2.f / 1000.f;
                    }
                }
                dt = (float)gFrame_period * 2.f / 1000.f;
                bolt->actors[0]->t.t.translate.t.v[0] += dt * bolt->field_0x2c[0].v[0];
                bolt->actors[0]->t.t.translate.t.v[1] += dt * bolt->field_0x2c[0].v[1];
                bolt->actors[0]->t.t.translate.t.v[2] += dt * bolt->field_0x2c[0].v[2];
                object = bolt->ped->character->personality->form->simple_physicing[bolt->ped->character->field_0x5].collision_info;
                BrVector3Sub(&delta_move, &bolt->actors[0]->t.t.translate.t, &object->actor->t.t.translate.t);
                delta_length_sq = BrVector3LengthSquared(&delta_move);
                if (delta_length_sq < REC2_SQR(object->bb1.max.v[0])
                        || delta_length_sq < REC2_SQR(object->bb1.max.v[1])
                        || delta_length_sq < REC2_SQR(object->bb1.max.v[2])
                        || delta_length_sq < REC2_SQR(object->bb1.min.v[0])
                        || delta_length_sq < REC2_SQR(object->bb1.min.v[1])
                        || delta_length_sq < REC2_SQR(object->bb1.min.v[2])) {
                    int j;

                    for (j = 0; j < (int)REC2_ASIZE(bolt->actors); j++) {
                        br_vector3 delta_world;

                        if (j != 0) {
                            BrVector3Sub(&delta_move, &bolt->actors[j]->t.t.translate.t, &object->actor->t.t.translate.t);
                        }
                        BrMatrix34TApplyP(&delta_world, &delta_move, &object->actor->t.t.mat);
                        if (delta_world.v[0] <= object->bb1.max.v[0]
                                && delta_world.v[0] >= object->bb1.min.v[0]
                                && delta_world.v[1] <= object->bb1.max.v[1]
                                && delta_world.v[1] >= object->bb1.min.v[1]
                                && delta_world.v[2] <= object->bb1.max.v[2]
                                && delta_world.v[2] >= object->bb1.min.v[2]) {
                            bolt_hit_target = 1;
                            break;
                        }
                    }
                }
                if (bolt_hit_target) {
                    br_actor* character_actor;

                    Explode(&gNapalmed_ped_animation, GetCharacterActorPtr(bolt->ped->character), &bolt->ped->character->personality->bb, &bolt->ped->character->personality->form->simple_physicing[bolt->ped->character->field_0x5].collision_info->cmpos);
                    bolt->ped->field_0x0c->field_0x98 = the_time + 15000;
                    character_actor = GetCharacterActorPtr(bolt->ped->character);
                    bolt->ped->field_0x0c->field_0xc0 = bolt->field_0x68;
                    bolt->ped->field_0x0c->field_0x2e |= 0x20;
                    CreateSmokeColumn2(2, character_actor, NULL, bolt->ped->character->personality->form->simple_physicing[bolt->ped->character->field_0x5].collision_info, 0, IRandomBetween(0, character_actor->model->prepared->groups->nvertices), 25000);
                    DRS3StartSound3D(gPedestrians_outlet, eSoundId_NapalmHitCrackls,
                        &bolt->ped->pos, &gZero_v__car, 1, 255, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    if (bolt->ped->hit_points > 0 && bolt->field_0x68 != NULL && bolt->field_0x68->driver == eDriver_local_human) {
                        PipeSinglePedIncident(bolt->ped, bolt->field_0x68->collision_info->actor);
                    }
                    MungeNapalm_KillBolt(bolt);
                }
            }
        }
        if (!bolt_hit_target) {
            br_vector3 flame_positions[7];
            int j;
            tU32 time = the_time;

            memset(flame_positions, 0, sizeof(flame_positions));
            for (j = 0; j < (int)REC2_ASIZE(flame_positions); j++) {
                br_actor* actor = bolt->actors[j];

                if (!gAction_replay_mode) {
                    time = (int)((float)time - 0.036f);
                    if (j != 0) {
                        int vert_index;

                        if (time < bolt->field_0x28) {
                            break;
                        }
                        vert_index = (bolt->field_0x24 - time) / 50;
                        vert_index = vert_index >= 0 ? vert_index : 0;
                        actor->t.t.translate.t.v[0] = bolt->actors[j - 1]->t.t.translate.t.v[0] - bolt->field_0x2c[vert_index].v[0] * .036f;
                        actor->t.t.translate.t.v[1] = bolt->actors[j - 1]->t.t.translate.t.v[1] - bolt->field_0x2c[vert_index].v[1] * .036f;
                        actor->t.t.translate.t.v[2] = bolt->actors[j - 1]->t.t.translate.t.v[2] - bolt->field_0x2c[vert_index].v[2] * .036f;
                        actor->render_style = BR_RSTYLE_FACES;
                    }
                } else if (actor->render_style != BR_RSTYLE_FACES) {
                    break;
                }
                actor->t = gCamera->t;
                BrMatrix34PreScale(&actor->t.t.mat,
                    (float)actor->material->colour_map->width * gFlamed_ped_flame_scales[j] / 128.f,
                    (float)actor->material->colour_map->height * gFlamed_ped_flame_scales[j] / 128.f,
                    1.f);
                BrMatrix34PreRotateZ(&actor->t.t.mat,
                    (br_angle)actor->t.t.translate.t.v[0] ^ (br_angle)actor->t.t.translate.t.v[1] ^ (br_angle)actor->t.t.translate.t.v[2]);
                actor->render_style = BR_RSTYLE_FACES;
                BrVector3Copy(&flame_positions[j], &actor->t.t.translate.t);
            }
            AddBurningPedToSession(j, bolt->ped, flame_positions);
        }
    }
    AREndPipingSession();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: SetPedFall
int C2_HOOK_FASTCALL SetPedFall(tPedestrian* pPed, int pSpeed, int pMove1, int pMove2, tPhysics_object* pCollision_info) { NOT_IMPLEMENTED(); return 0; }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// (reference)
// FUNCTION: CARMA2_HW 0x004ce330
void C2_HOOK_FASTCALL RepulseIt(tPedestrian* pPed, tCar_spec* pCar, float pDist_sq, tU32 pNow, void* pData) {
    int speed;

    (void)pDist_sq;
    (void)pNow;
    (void)pData;
    speed = IRandomBetween(0x4b, 0xc8);
    SetPedFall(pPed, speed, 0x5b, 0x5b, NULL);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL: only the replay-mode and halted-ped-status early returns are implemented
// FUNCTION: CARMA2_HW 0x004d1030
int C2_HOOK_FASTCALL ActiveHalted(tPedestrian* pPed, undefined4* pArg2, int pArg3) {
    extern void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tS8 pArg7, tS8 pArg8, tS8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12);
    tPed_character_instance* character = (tPed_character_instance*)pPed;

    if (gAction_replay_mode != 0) {
        return 1;
    }
    if (gPipe_halted_ped_status != 0) {
        PipeSinglePedStatus(NULL, 5, 1, character->field_0x4, character->field_0x4, character->field_0x5, character->field_0x5, 0, 0, &character->field_0xc0, &character->field_0xcc, &character->field_0x8c);
        return 1;
    }
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004d1020
int C2_HOOK_FASTCALL CBActiveHalted(undefined4* pArg1, undefined4* pArg2) {
    extern int C2_HOOK_FASTCALL ActiveHalted(tPedestrian* pPed, undefined4* pArg2, int pArg3);

    return ActiveHalted((tPedestrian*)pArg1, pArg2, 0);
}

extern tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime);

// FUNCTION: CARMA2_HW 0x0040b770
void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2) {
    tPhysics_object* obj;
    br_matrix34* character_matrix;
    br_matrix34 mat34;
    tPed_form* form;

    if (!(pCharacter->field_0x14 & 1)) {
        return;
    }

    form = pCharacter->personality->form;
    if (pCharacter->field_0x4 < 0) {
        character_matrix = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        character_matrix = pCharacter->field_0xe8;
    } else {
        character_matrix = &form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34LPInverse(&mat34, &form->moves[pCharacter->field_0x7].move->frames[(tS16)pCharacter->field_0x1c].mat);
    BrMatrix34Mul(&pCharacter->field_0x8c, &mat34, character_matrix);
    pCharacter->field_0x8c.m[3][0] = 0.f;
    pCharacter->field_0x8c.m[3][1] = 0.f;
    pCharacter->field_0x8c.m[3][2] = 0.f;
    SetCharacterBonePositions(pCharacter, 3, 0);

    obj = pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
    pCharacter->field_0xd8.v[0] = obj->v.v[0];
    pCharacter->field_0xd8.v[1] = obj->v.v[1];
    pCharacter->field_0xd8.v[2] = obj->v.v[2];

    if (gPed_vtable->moved_by_physics != NULL) {
        gPed_vtable->moved_by_physics((undefined4*)pCharacter, (undefined4*)obj, pArg2);
    }
}

extern tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name);

// FUNCTION: CARMA2_HW 0x00404740
tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName) {
    int i;
    tPed_remap** remap;

    for (i = 0, remap = gPed_remaps; (int)remap < (int)&gPed_remaps[10]; remap++, i++) {
        if (*remap != NULL && strcmp(pName, (*remap)->name) == 0) {
            return gPed_remaps[i];
        }
    }
    return ReadRemap(pName);
}

// Declared here instead of including 49-physics.h: pulling that header into this TU flips the
// module-level codegen of BonerPedMovedByPhysics (80%), MakePedNoise (73.79%) and SetCharacterMoveAR
// (69.89%) away from 100%. Measured with reccmp.
extern void C2_HOOK_FASTCALL FreeShape(void* pData);
extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);

// Placement is load-bearing: with three or more helper bodies above it, MSVC gives
// DisposePedPersonality a 0xc frame (reccmp 68.21) instead of the original 0x10 frame /
// induction-pointer form. Here (between FindOrOpenRemap and CharacterNoLongerCollideworthy,
// above the RandomWander/SmoothTurnPedestrian bodies) it is 100. The committed slot right before
// MungePedestrians is also 100, but RandomWander there drops BonerPedMovedByPhysics 0x40b770 to 80.
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004077f0
void C2_HOOK_FASTCALL DisposePedPersonality(tPed_personality* pPersonality) {
    int i;
    int j;
    int k;

    if (gPed_vtable->dispose_personality != NULL) {
        gPed_vtable->dispose_personality((undefined4*)pPersonality);
    }
    if (pPersonality->moves != NULL) {
        BrMemFree(pPersonality->moves);
    }
    for (i = 0; i < pPersonality->form->count_bones; i++) {
        for (j = 0; j < 4; j++) {
            if (pPersonality->bones[i].models[j].models[0] == NULL) {
                break;
            }
            for (k = 1; k < 2; k++) {
                BrModelRemove(pPersonality->bones[i].models[j].models[k]);
                BrModelFree(pPersonality->bones[i].models[j].models[k]);
            }
        }
        if (pPersonality->bones[i].hinge != NULL) {
            FreePhysicsJoint(pPersonality->bones[i].hinge);
        }
    }
    if (pPersonality->bones != NULL) {
        BrMemFree(pPersonality->bones);
    }
    if (pPersonality != NULL) {
        BrMemFree(pPersonality);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* RandomWander: no standalone symbol in the original; inlined into MungePedestrians 0x4d3740
   (block 0x4d45d9-0x4d4732). Re-derived from that block; SetNextRandomTurn is inlined too.
   Static body right after DisposePedPersonality (which tolerates at most two added bodies
   above it): the slot flips DisposePedPersonality 0x4077f0 to 68.21; right before MungePedestrians flips Boner to 80. */
static void C2_HOOK_FASTCALL RandomWander(tPedestrian* pPed, tU32 pTime) {
    extern br_vector3 gY_unit_vector;
    br_matrix34 mat;
    br_vector3 dir_old;
    br_vector3 dirxz_old;
    br_vector3 dirxz_new;
    tPed_cache_006944c0 state_old;
    int angle;

    if (pPed->field_0x0c->next_turn_time != 0 && pTime >= pPed->field_0x0c->next_turn_time) {
        if (pPed->movement_spec->max_time_between != 0) {
            pPed->field_0x0c->next_turn_time = pTime + IRandomBetween(pPed->movement_spec->min_time_between, pPed->movement_spec->max_time_between);
        } else {
            pPed->field_0x0c->next_turn_time = 0;
        }
        if (PercentageChance(50)) {
            angle = IRandomBetween(BR_ANGLE_DEG(10), pPed->movement_spec->max_random_angle);
        } else {
            angle = -IRandomBetween(BR_ANGLE_DEG(10), pPed->movement_spec->max_random_angle);
        }
        BrMatrix34Rotate(&mat, angle, &gY_unit_vector);
        dirxz_old.v[0] = pPed->character->field_0xc0.v[0];
        dirxz_old.v[1] = 0.f;
        dirxz_old.v[2] = pPed->character->field_0xc0.v[2];
        BrMatrix34ApplyV(&dirxz_new, &dirxz_old, &mat);
        state_old = *pPed->field_0x0c;
        dir_old.v[0] = pPed->character->field_0xc0.v[0];
        dir_old.v[1] = pPed->character->field_0xc0.v[1];
        dir_old.v[2] = pPed->character->field_0xc0.v[2];
        SetPedXZDirection(pPed, &dirxz_new, 1.f, pTime);
        RescanPedProximity(pPed, &dirxz_new);
        if (pPed->field_0x0c->field_0x18) {
            *pPed->field_0x0c = state_old;
            SetPedXZDirection(pPed, &dir_old, 0.f, pTime);
        }
    }
}

/* SmoothTurnPedestrian body: `field_0x6c` is the turn start time, `field_0x70` the end time, and
   `field_0x60`/`field_0x54` the source/target XZ directions (block at 0x4d45xx). */
static void C2_HOOK_FASTCALL SmoothTurnPedestrian(tPedestrian* pPed, tU32 pTime) {
    extern void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pCharacter, br_vector3* pDir, br_vector3* pUp);
    tPed_cache_006944c0* ped_cache;
    float t;
    float invt;
    br_vector3 dir;

    ped_cache = pPed->field_0x0c;
    if (pTime < ped_cache->field_0x70) {
        t = (float)(pTime - ped_cache->field_0x6c) / (float)(ped_cache->field_0x70 - ped_cache->field_0x6c);
        invt = 1.f - t;
        dir.v[0] = t * ped_cache->field_0x54.v[0] + invt * ped_cache->field_0x60.v[0];
        dir.v[1] = t * ped_cache->field_0x54.v[1] + invt * ped_cache->field_0x60.v[1];
        dir.v[2] = t * ped_cache->field_0x54.v[2] + invt * ped_cache->field_0x60.v[2];
        SetCharacterDirectionAR(pPed->character, &dir, &pPed->character->field_0xcc);
    } else {
        StopSmoothTurning(pPed);
        SetCharacterDirectionAR(pPed->character, &pPed->field_0x0c->field_0x48, &pPed->character->field_0xcc);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 78.73%: MSVC lazy register saves (ebx/esi pushed at entry here, after the early
// return in the original), swapped gPow2_array/field_0xc load order and lea-vs-folded
// element address in the else branch.

// FUNCTION: CARMA2_HW 0x00409090
int C2_HOOK_FASTCALL CharacterNoLongerCollideworthy(tPed_character_instance* pCharacter) {
    tPed_form* form;
    tPhysics_object* obj;
    tPhysics_object** infos;
    int i;
    int count;

    if (pCharacter->field_0x14 == 0) {
        return 0x1f;
    }
    form = pCharacter->personality->form;
    if (pCharacter->field_0x14 & 1) {
        obj = form->simple_physicing[pCharacter->field_0x5].collision_info;
        PHILRemoveObject(obj);
        obj->field183_0x1d8 = 0;
        *(int*)((char*)obj + 0x23c) = 0;
        *(tU8*)&form->simple_physicing[pCharacter->field_0x5].type = 0;
        if (pCharacter->field_0xc != 0 && *(tS8*)&pCharacter->field_0x6 >= 0) {
            tPed_form_boned_phys* boned = &form->boned_physicing[*(tS8*)&pCharacter->field_0x6];
            for (i = 1; i < form->count_bones; i++) {
                if ((pCharacter->field_0xc & gPow2_array[i]) != 0) {
                    if (PHILReturnObjectStatus(boned->collision_infos[i]) == 2) {
                        pCharacter->field_0x10 |= gPow2_array[i];
                    }
                    PHILRemoveObject(boned->collision_infos[i]);
                    boned->collision_infos[i]->field183_0x1d8 = 0;
                    *(int*)((char*)boned->collision_infos[i] + 0x23c) = 0;
                }
            }
            *(tS8*)&pCharacter->field_0x6 = -1;
            boned->field_0x0 = 0;
            pCharacter->field_0x14 = 0;
            return 0;
        }
    } else {
        count = form->count_bones;
        infos = form->boned_physicing[pCharacter->field_0x5].collision_infos;
        for (i = 0; i < count; i++) {
            if (*(int*)((char*)infos[i] + 0x20) != 0) {
                obj = infos[i];
                goto found;
            }
        }
        obj = NULL;
found:
        PHILRemoveObject(obj);
        obj->field183_0x1d8 = 0;
        form->boned_physicing[pCharacter->field_0x5].field_0x0 = 0;
    }
    pCharacter->field_0x14 = 0;
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004067e0
void C2_HOOK_FASTCALL DisposePedForm(tPed_form* pForm) {
    int i;
    int j;

    if (gPed_vtable->dispose_form != NULL) {
        gPed_vtable->dispose_form(pForm);
    }
    for (i = 0; i < pForm->max_rendering_at_once; i++) {
        for (j = 0; j < pForm->count_bones; j++) {
            if (IsActorRenderworthy(pForm->actor_sets[i].actors[j])) {
                StopActorBeingRenderable(pForm->actor_sets[i].actors[j]);
            }
            BrActorFree(pForm->actor_sets[i].actors[j]);
        }
        if (pForm->actor_sets[i].actors != NULL) {
            BrMemFree(pForm->actor_sets[i].actors);
        }
    }
    for (i = 0; i < pForm->count_bones; i++) {
        if (pForm->boned_physicing[0].collision_infos[i] != NULL) {
            if (pForm->bones[i].hinge != NULL) {
                FreePhysicsJoint(pForm->bones[i].hinge);
            }
            for (j = 0; j < pForm->max_boned_physicing_at_once; j++) {
                if (pForm->boned_physicing[j].collision_infos[i]->shape != NULL) {
                    FreeShape(pForm->boned_physicing[j].collision_infos[i]->shape);
                }
                if (pForm->boned_physicing[j].collision_infos[i] != NULL && pForm->boned_physicing[j].collision_infos[i] != NULL) {
                    BrMemFree(pForm->boned_physicing[j].collision_infos[i]);
                }
            }
        }
    }
    for (i = 0; i < pForm->max_boned_physicing_at_once; i++) {
        if (pForm->boned_physicing[i].collision_infos != NULL) {
            BrMemFree(pForm->boned_physicing[i].collision_infos);
        }
    }
    for (i = 0; i < pForm->max_simple_physicing_at_once; i++) {
        FreeShape(pForm->simple_physicing[i].collision_info->shape);
        if (pForm->simple_physicing[i].collision_info != NULL) {
            BrMemFree(pForm->simple_physicing[i].collision_info);
        }
    }
    for (i = 0; i < pForm->max_stored_dismembered_characters; i++) {
        if (pForm->stored_dismembered_characters[i] != NULL) {
            BrMemFree(pForm->stored_dismembered_characters[i]);
        }
    }
    if (pForm->stored_dismembered_characters != NULL) {
        BrMemFree(pForm->stored_dismembered_characters);
    }
    if (pForm->moves != NULL) {
        BrMemFree(pForm->moves);
    }
    if (pForm->actor_sets != NULL) {
        BrMemFree(pForm->actor_sets);
    }
    if (pForm->simple_physicing != NULL) {
        BrMemFree(pForm->simple_physicing);
    }
    if (pForm->boned_physicing != NULL) {
        BrMemFree(pForm->boned_physicing);
    }
    if (pForm->bones != NULL) {
        BrMemFree(pForm->bones);
    }
    if (pForm != NULL) {
        BrMemFree(pForm);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004085f0
int C2_HOOK_FASTCALL MakeCharacterPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, br_actor* pActor, void* pFrame, br_vector3* pBonePos, int pKind, undefined4 pArg3, int pFlag, int pBoneIndex, br_vector3* pBoneVec, br_vector3* pBonePtr) {
    extern void C2_HOOK_FASTCALL MakeCharacterPhysicsSetup(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr);

    BrMatrix34Copy(&pObject->transform_matrix, &pActor->t.t.mat);
    pObject->actor = pActor;
    MakeCharacterPhysicsSetup(pObject, pFrame, pBonePos == NULL, pBoneVec, pBonePtr);
    pObject->flags_0x238 = (tU8)pKind + 2;
    pObject->owner = pCharacter;
    if (pBonePos != NULL) {
        pObject->cmpos.v[0] = pBonePos->v[0];
        pObject->cmpos.v[1] = pBonePos->v[1];
        pObject->cmpos.v[2] = pBonePos->v[2];
    }
    pObject->disable_move_rotate = 1;
    pObject->field_0xed = 1;
    BrVector3SetFloat(&pObject->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pObject->omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pObject->rotate_omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pObject->velocity_car_space, 0.f, 0.f, 0.f);
    pObject->uid = (tU8)pBoneIndex;
    pObject->world_friction = 1.f;
    pObject->last_special_volume = NULL;
    pObject->auto_special_volume = NULL;
    if (pKind == 1) {
        pObject->flags |= pKind;
    } else if (pKind == 2) {
        pObject->flags |= pKind;
    }
    if (pFlag != 0) {
        if (PHILAddObjectImmediately(pObject) != 0) {
            return 0;
        }
        PHILSetObjectProperty(pObject, 0, 1.0);
        PHILSetObjectProperty(pObject, 3, pArg3 == 0);
        PHILSetObjectProperty(pObject, 4, 1);
        PHILSetObjectProperty(pObject, 5, 1);
        PHILSetObjectProperty(pObject, 7, 1);
        PHILSetObjectProperty(pObject, 8, gPed_buoyancy_factor);
        if (pKind == 3) {
            PHILSetObjectProperty(pObject, 6, 1);
        }
    }
    if (gPed_vtable->fill_in_object != NULL) {
        gPed_vtable->fill_in_object((undefined4*)pObject, pKind + 2);
    }
    return 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004087b0
void C2_HOOK_FASTCALL MakeCharacterPhysicsSetup(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr) {
    br_scalar P[3];
    br_scalar Q[3];
    br_scalar dmax[3];
    br_scalar d[3];
    br_scalar a;
    br_scalar b;
    br_scalar maxcomp;
    br_scalar Vx;
    br_scalar Vy;
    br_scalar Vz;
    int index;
    int r1;
    int r2;
    int i;

    if (pFrame != NULL) {
        d[0] = pFrame->max.v[0] - pFrame->min.v[0];
        d[1] = pFrame->max.v[1] - pFrame->min.v[1];
        d[2] = pFrame->max.v[2] - pFrame->min.v[2];
        dmax[0] = d[0];
        dmax[1] = d[1];
        dmax[2] = d[2];
    }
    switch (pObject->shape->common.type) {
    case kCollisionShapeType_Box:
        pObject->shape->common.bb = *pFrame;
        break;
    case kCollisionShapeType_Wireframe:
        if (pBoneVec != NULL && pBonePtr != NULL) {
            pObject->shape->wireframe.wireframe.points[0].v[0] = pBoneVec->v[0];
            pObject->shape->wireframe.wireframe.points[0].v[1] = pBoneVec->v[1];
            pObject->shape->wireframe.wireframe.points[0].v[2] = pBoneVec->v[2];
            pObject->shape->wireframe.wireframe.points[1].v[0] = pBonePtr->v[0];
            pObject->shape->wireframe.wireframe.points[1].v[1] = pBonePtr->v[1];
            pObject->shape->wireframe.wireframe.points[1].v[2] = pBonePtr->v[2];
        } else {
            maxcomp = -1.0f;
            index = -1;
            for (i = 0; i < 3; i++) {
                if (maxcomp < dmax[i]) {
                    maxcomp = dmax[i];
                    index = i;
                }
            }
            r1 = (index + 1) % 3;
            r2 = (index + 2) % 3;
            P[index] = pFrame->min.v[index];
            Q[index] = pFrame->max.v[index];
            P[r1] = (pFrame->min.v[r1] + pFrame->max.v[r1]) * 0.5f;
            Q[r1] = P[r1];
            P[r2] = (pFrame->min.v[r2] + pFrame->max.v[r2]) * 0.5f;
            Q[r2] = P[r2];
            if (pBoneVec == NULL && pBonePtr == NULL) {
                pObject->shape->wireframe.wireframe.points[0].v[0] = P[0];
                pObject->shape->wireframe.wireframe.points[0].v[1] = P[1];
                pObject->shape->wireframe.wireframe.points[0].v[2] = P[2];
                pObject->shape->wireframe.wireframe.points[1].v[0] = Q[0];
                pObject->shape->wireframe.wireframe.points[1].v[1] = Q[1];
                pObject->shape->wireframe.wireframe.points[1].v[2] = Q[2];
            } else {
                if (pBoneVec != NULL) {
                    Vx = pBoneVec->v[0];
                    Vy = pBoneVec->v[1];
                    Vz = pBoneVec->v[2];
                } else {
                    Vx = pBonePtr->v[0];
                    Vy = pBonePtr->v[1];
                    Vz = pBonePtr->v[2];
                }
                a = (Vx - P[0]) * (Vx - P[0]) + (Vy - P[1]) * (Vy - P[1]) + (Vz - P[2]) * (Vz - P[2]);
                b = (Vx - Q[0]) * (Vx - Q[0]) + (Vy - Q[1]) * (Vy - Q[1]) + (Vz - Q[2]) * (Vz - Q[2]);
                if (a > b) {
                    pObject->shape->wireframe.wireframe.points[0].v[0] = P[0];
                    pObject->shape->wireframe.wireframe.points[0].v[1] = P[1];
                    pObject->shape->wireframe.wireframe.points[0].v[2] = P[2];
                } else {
                    pObject->shape->wireframe.wireframe.points[0].v[0] = Q[0];
                    pObject->shape->wireframe.wireframe.points[0].v[1] = Q[1];
                    pObject->shape->wireframe.wireframe.points[0].v[2] = Q[2];
                }
                pObject->shape->wireframe.wireframe.points[1].v[0] = Vx;
                pObject->shape->wireframe.wireframe.points[1].v[1] = Vy;
                pObject->shape->wireframe.wireframe.points[1].v[2] = Vz;
            }
        }
        break;
    case kCollisionShapeType_Sphere:
        pObject->shape->sphere.sphere.radius = (d[0] + d[1] + d[2]) * 0.166666672f;
        pObject->shape->sphere.sphere.radius_squared = pObject->shape->sphere.sphere.radius * pObject->shape->sphere.sphere.radius;
        pObject->shape->sphere.sphere.center.v[0] = pFrame->min.v[0] - -0.5f * d[0];
        pObject->shape->sphere.sphere.center.v[1] = pFrame->min.v[1] - -0.5f * d[1];
        pObject->shape->sphere.sphere.center.v[2] = pFrame->min.v[2] - -0.5f * d[2];
        break;
    default:
        break;
    }
    FillInShape(pObject->shape);
    UpdateCollisionObject(pObject);
    if (pFrame == NULL) {
        pFrame = &pObject->shape->common.bb;
        d[0] = pFrame->max.v[0] - pFrame->min.v[0];
        d[1] = pFrame->max.v[1] - pFrame->min.v[1];
        d[2] = pFrame->max.v[2] - pFrame->min.v[2];
    }
    pObject->I.v[2] = pObject->M * (d[1] * d[1] + d[0] * d[0]) * 0.025f;
    pObject->I.v[1] = pObject->M * (d[2] * d[2] + d[0] * d[0]) * 0.025f;
    pObject->I.v[0] = pObject->M * (d[2] * d[2] + d[1] * d[1]) * 0.025f;
    if (pNull != 0) {
        pObject->cmpos.v[0] = pFrame->min.v[0] - -0.5f * d[0];
        pObject->cmpos.v[1] = pFrame->min.v[1] - -0.5f * d[1];
        pObject->cmpos.v[2] = pFrame->min.v[2] - -0.5f * d[2];
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* based on upstream madebr/rec2 a373d899a src/carma2/common/pedestrn.c
   (moved here from 28-world3.c: new external symbols in that file re-roll
   SetCharacterDirectionAR 0x004d3360). */
extern void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
extern void C2_HOOK_FASTCALL DRMatrix34RotateCos(br_matrix34* pDest, const br_vector3* pAxis, float pCosA);
extern void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);

#define SCD_NORMALISE_D(v1, v2)                                  \
    do {                                                         \
        len = BR_LENGTH3((v2)->v[0], (v2)->v[1], (v2)->v[2]);    \
        if (len > BR_SCALAR_EPSILON * 2) {                       \
            scale = 1.0 / len;                                   \
            (v1)->v[0] = (v2)->v[0] * scale;                     \
            (v1)->v[1] = (v2)->v[1] * scale;                     \
            (v1)->v[2] = (v2)->v[2] * scale;                     \
        } else {                                                 \
            (v1)->v[0] = 1.0f;                                   \
            (v1)->v[1] = 0.0f;                                   \
            (v1)->v[2] = 0.0f;                                   \
        }                                                        \
    } while (0)

// FUNCTION: CARMA2_HW 0x0040a730
void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp) {
    double scale;
    br_scalar len;
    int keep_direction;
    tPed_move* move;
    br_matrix34* character_matrix;
    float cos_a;
    br_vector3 norm_dir;
    br_vector3 norm_up;
    br_vector3 local_138;
    br_vector3 local_12c;
    br_vector3 local_120;
    br_vector3 local_114;
    br_vector3 local_fc;
    br_vector3 local_e4;
    br_matrix34 inv_field_0x8c;
    br_matrix34 local_c0;
    br_matrix34 local_60;
    br_matrix34 local_30;
    br_vector3 original_pos;

    if (pPed->field_0x7 < 0) {
        return;
    }
    if ((pPed->field_0x14 & 0x4) && !gAction_replay_mode) {
        return;
    }
    if (pDir == NULL) {
        pDir = &pPed->field_0xc0;
        pUp = &pPed->field_0xcc;
        keep_direction = 1;
    } else {
        SCD_NORMALISE_D(&pPed->field_0xc0, pDir);
        pPed->field_0xcc = *pUp;
        keep_direction = 0;
    }
    move = pPed->personality->form->moves[pPed->field_0x7].move;
    if (pPed->field_0x4 < 0) {
        character_matrix = &pPed->field_0x2c;
    } else if (pPed->field_0xe8 != NULL) {
        character_matrix = pPed->field_0xe8;
    } else {
        character_matrix = &pPed->personality->form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
    }
    BrMatrix34Inverse(&inv_field_0x8c, &pPed->field_0x8c);
    original_pos = *(br_vector3*)character_matrix->m[3];
    BrVector3Set((br_vector3*)character_matrix->m[3], 0.f, 0.f, 0.f);
    BrMatrix34Mul(&local_c0, character_matrix, &inv_field_0x8c);
    if (pUp != NULL) {
        SCD_NORMALISE_D(&norm_up, pUp);
    } else {
        BrVector3Set(&norm_up, 0.f, 1.f, 0.f);
    }
    SCD_NORMALISE_D(&norm_dir, pDir);
    BrVector3Copy(&local_12c, &move->field_0x40);
    if (pPed->field_0xbc != NULL) {
        local_12c.v[0] = (1.f - pPed->field_0xbc->field_0x0) * pPed->field_0xbc->field_0x20.v[0] + local_12c.v[0] * pPed->field_0xbc->field_0x0;
        local_12c.v[1] = (1.f - pPed->field_0xbc->field_0x0) * pPed->field_0xbc->field_0x20.v[1] + local_12c.v[1] * pPed->field_0xbc->field_0x0;
        local_12c.v[2] = (1.f - pPed->field_0xbc->field_0x0) * pPed->field_0xbc->field_0x20.v[2] + local_12c.v[2] * pPed->field_0xbc->field_0x0;
        BrVector3Normalise(&local_12c, &local_12c);
    }
    DropPointOntoPlane(&norm_dir, &norm_up, &local_e4);
    DropPointOntoPlane(&local_12c, &norm_up, &local_114);
    BrVector3Normalise(&local_fc, &local_e4);
    BrVector3Normalise(&local_114, &local_114);
    cos_a = BrVector3Dot(&local_fc, &local_114);
    OrthogonalVector(&local_138, &local_114, &local_fc);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&pPed->field_0x8c, &local_120, cos_a);
    BrVector3Set(&local_138, norm_up.v[2], 0.f, -norm_up.v[0]);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&local_60, &local_120, norm_up.v[1]);
    BrMatrix34Pre(&pPed->field_0x8c, &local_60);
    cos_a = BrVector3Dot(&local_fc, &norm_dir);
    DRVector3SafeCross(&local_138, &local_fc, &norm_dir);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&local_30, &local_120, cos_a);
    BrMatrix34Mul(&pPed->field_0x5c, &pPed->field_0x8c, &local_30);
    BrMatrix34Post(&local_c0, &pPed->field_0x8c);
    BrVector3Add((br_vector3*)local_c0.m[3], (br_vector3*)local_c0.m[3], &original_pos);
    BrMatrix34LPNormalise(character_matrix, &local_c0);
    if (!keep_direction) {
        SetCharacterBonePositions(pPed, 3, 0);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a0418
tU32 gPed_last_score_time;

// GLOBAL: CARMA2_HW 0x00694120
tPedestrian* gPed_recent_ped;

// FUNCTION: CARMA2_HW 0x004d3740
void C2_HOOK_FASTCALL MungePedestrians(void) {
    tU32 the_time;
    unsigned int count_scared_pedestrians;
    tPedestrian* scared_pedestrians[10];
    int scare_now;
    int i;

    the_time = GetTotalTime();

    if (gPed_valium_left != 0) {
        gPed_valium_left -= gFrame_period;
        if (gPed_valium_left < 0) {
            gPed_valium_left = 0;
        }
    }
    gCount_active_peds = 0;
    gPedestrians_in_sight = NULL;
    gPeds_already_munged = GetTotalTime() == gPed_last_munging;
    gPed_last_munging = GetTotalTime();
    count_scared_pedestrians = 0;
    if (the_time > gLast_scare_time + 300 && !gBlind_pedestrians && gPed_valium_left == 0) {
        scare_now = 1;
        gLast_scare_time = the_time;
    } else {
        scare_now = 0;
    }
    if (!gAction_replay_mode) {
        for (i = 0; i < gPed_count; i++) {
            tPedestrian* ped;
            br_vector3 ped_distance;

            ped = &gPedestrian_array[i];
            BrVector3Sub(&ped_distance, (br_vector3*)gCamera_to_world.m[3], &ped->pos);
            if ((((ped->flags & 0x80)
                        || fabs(ped_distance.v[0]) >= gPed_process_distance
                        || fabs(ped_distance.v[1]) >= gPed_process_distance_inner
                        || fabs(ped_distance.v[2]) >= gPed_process_distance) && (ped->field_0x0c == NULL || ped->field_0x0c->field_0x8c == 0 || the_time - ped->field_0x0c->field_0x8c >= 7500))
                    || (!(ped->flags & 0x1)
                        && !gPed_nearness
                        && fabs(ped_distance.v[0]) <= gPed_popup_distance
                        && fabs(ped_distance.v[1]) <= gPed_popup_distance_inner
                        && fabs(ped_distance.v[2]) <= gPed_popup_distance)) {
                ped->flags &= ~0x1;
            } else {
                ped->flags |= 0x1;
                if (scare_now && ped->action == ePed_action_walking && count_scared_pedestrians < REC2_ASIZE(scared_pedestrians)) {
                    scared_pedestrians[count_scared_pedestrians] = ped;
                    count_scared_pedestrians += 1;
                }
            }
        }
    }
    for (i = 0; i < gPed_count; i++) {
        tPedestrian* ped = &gPedestrian_array[i];
        br_vector3* ped_pos;

        if (!(ped->flags & 0x1)) {
            tPed_character_instance *character;

            character = ped->character;
            if (character->field_0x4 >= 0 && !gAction_replay_mode) {
                undefined4 prev_field_0x10;
                PipeSinglePedStatus(ped,
                    1, 0, 1, 0,
                    character->field_0x4, 255,
                    character->field_0x5, 255,
                    &character->field_0xc0,
                    &character->field_0xcc,
                    &character->field_0x8c);
                if (character->field_0xbc != NULL) {
                    StopCharacterMorphing(character);
                }
                if (character->field_0x14 & 0x4) {
                    PedFallingForever(ped);
                }
                prev_field_0x10 = ped->character->field_0x10;
                SetCharacterPhysicsLevel(ped->character, 0);
                CharacterNoLongerRenderable(ped->character);
                if (ped->character->field_0x10 != prev_field_0x10) {
                    PipeSingleVanishedDismembered(ped, prev_field_0x10, ped->character->field_0x10);
                }
                if (ped->field_0x0c != NULL && ped->field_0x0c->field_0x98 != 0) {
                    if (ped->field_0x0c->field_0x98 >= 0) {
                        if (ped->hit_points > 0) {
                            ScoreForKilledPedestrian(ped, 0.f);
                        }
                        StopObjectSmokingInstantly(ped->character->personality->form->simple_physicing[ped->character->field_0x5].collision_info);
                        ped->field_0x0c->field_0x98 = 0;
                        MakePedVanish(ped);
                    } else {
                        KillNapalmBolt(&gNapalm_bolts[-ped->field_0x0c->field_0x98 + 1]);
                    }
                }
            }
            continue;
        }
        if (ped->character->field_0x4 < 0 && !gAction_replay_mode) {
            if (MakeCharacterRenderable(ped->character) == 1) {
                ped->flags &= ~0x1;
                continue;
            }
            if (MakeCharacterCollideworthy(ped->character, 0, 0) == 21) {
                CharacterNoLongerRenderable(ped->character);
                ped->flags &= ~0x1;
                continue;
            }
            PipeSinglePedStatus(ped, 0, 1, 0, 1,
                255, ped->character->field_0x4,
                255, ped->character->field_0x5,
                &ped->character->field_0xc0,
                &ped->character->field_0xcc,
                &ped->character->field_0x8c);
        }
        ped->next = gPedestrians_in_sight;
        gPedestrians_in_sight = ped;
        if (!gAction_replay_mode) {
            if (ped->flags & 0x20) {
                ped->flags |= 0x40;
            } else {
                ped->flags |= 0x20;
            }
        }
        if (ped->speed_factor != gPedestrian_speed_factor && ped->hit_points > 0) {
            SetCharacterMove(ped->character, -1, -1.f, 0, 0, 0);
            if (ped->speed_factor == 0.f) {
                EnableOverallMovement();
            }
            if (gPedestrian_speed_factor != 0.f) {
                SetCharacterMove(ped->character, -1, (float)ped->character->field_0x1e / gPedestrian_speed_factor, 0, 0, 0);
            } else {
                DisableOverallMovement();
            }
            ped->speed_factor = gPedestrian_speed_factor;
        }
        gCount_active_peds += 1;
        if (ped->field_0x0c == NULL || ped->field_0x0c == &gPed_cache_00694328) {
            if (gAction_replay_mode) {
                ped->field_0x0c = &gPed_cache_00694328;
            } else {
                tU32 oldest_time;
                int oldest_index;
                size_t j;

                oldest_time = the_time + 1;
                oldest_index = 0;

                for (j = 0; j < REC2_ASIZE(gPed_cache_006944c0); j++) {
                    if (gPed_cache_006944c0[j].field_0xb4 == NULL) {
                        ped->field_0x0c = &gPed_cache_006944c0[j];
                        break;
                    }
                    if (gPed_cache_006944c0[j].field_0x74 < oldest_time) {
                        oldest_index = j;
                        oldest_time = gPed_cache_006944c0[j].field_0x74;
                    }
                }
                if (j == REC2_ASIZE(gPed_cache_006944c0)) {
                    ped->field_0x0c = &gPed_cache_006944c0[oldest_index];
                    gPed_cache_006944c0[oldest_index].field_0xb4->field_0x0c = NULL;
                }
            }
            InitProcessData(ped, the_time);
        }
        ped->field_0x0c->field_0xb4 = ped;
        ped->field_0x0c->field_0x74 = the_time;
        ped->field_0x0c->field_0xc4 &= ~0x2;

        ped_pos = GetPedPos(ped);
        if (!gAction_replay_mode) {
            if (ped->character->field_0x14) {
                if (gPed_gravity_multiplier != PHILGetObjectProperty(MUNGE_PED_COLLISION_OBJECT(ped), 0)) {
                    PHILSetObjectProperty(MUNGE_PED_COLLISION_OBJECT(ped), 0, gPed_gravity_multiplier);
                    if (ped->character->field_0x6 >= 0) {
                        int j;

                        for (j = 0; j < ped->character->personality->form->count_bones; j++) {
                            PHILSetObjectProperty(ped->character->personality->form->boned_physicing[ped->character->field_0x6].collision_infos[j], 0, gPed_gravity_multiplier);
                        }
                    }
                }
            }
            if (!gAction_replay_mode) {
                if (MUNGE_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL && MUNGE_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.f) {
                    if (ped->field_0x0c->field_0x98 < 0) {
                        KillNapalmBolt(&gNapalm_bolts[1 - ped->field_0x0c->field_0x98]);
                    } else if (ped->field_0x0c->field_0x98 > 0) {
                        StopObjectSmokingInstantly(ped->character->personality->form->simple_physicing[ped->character->field_0x5].collision_info);
                        ped->field_0x0c->field_0x98 = 0;
                    }
                    if (ped->field_0x0c->field_0x94 == 0) {
                        ped->field_0x0c->field_0x94 = the_time;
                    }
                    if (ped->hit_points > 0) {
                        if (ped->character->personality->form->moves[ped->character->field_0x7].id != 110) {
                            SetPedMove(ped, 110, -1, 0, 0, the_time, ePed_action_impacting);
                        }
                        if (the_time - ped->field_0x0c->field_0x94 > 30000) {
                            KillPedestrian(ped, NULL);
                        }
                    } else {
                        if (ped->character->personality->form->moves[ped->character->field_0x7].id != 94) {
                            SetPedMove(ped, 94, -1, 0, 0, the_time, ePed_action_impacting);
                        }
                    }
                } else {
                    ped->field_0x0c->field_0x94 = 0;
                }
                if (ped->field_0x0c->field_0x98 > 0 && ped->hit_points > 0) {
                    if (the_time >= (tU32)ped->field_0x0c->field_0x98 && !gImmortal_peds) {
                        KillPedestrian(ped, NULL);
                        DoPostElectricution(ped, the_time, .0f, .5f);
                        ped->field_0x0c->field_0x98 = 0;
                    } else {
                        MakePedNoise(ped, 6, 0, NULL);
                    }
                }
            }
        }

        if (!(ped->character->field_0x14 & 0x4)
                && (ped->action == ePed_action_walking || ped->action == ePed_action_running)
                && (!gAction_replay_mode || gBOOL_00744804)) {
            int j;
            int nearby_changed_point;
            tPed_face_cache_0x34* face_cache;
            int cache_bool1;
            int cache_bool2;
            br_vector3 cache_pos1;

            AcceleratePed(ped, the_time);
            ped->flags &= ~0x6;
            nearby_changed_point = 0;
            for (j = 0; j < gCount_changed_points; j++) {
                if (Vector3DistanceSquared(&gChanged_points[j], &ped->pos) < 400.f) {
                    nearby_changed_point = 1;
                    break;
                }
            }
            face_cache = RecacheAndSetFace(ped, &cache_bool1, &cache_bool2, &cache_pos1, gBOOL_00744804 | nearby_changed_point, the_time);
            if (face_cache == NULL) {
                face_cache = RecacheAndSetFace(ped, &cache_bool1, &cache_bool2, &cache_pos1, gBOOL_00744804 | nearby_changed_point, the_time);
            }
            if (cache_bool1 && ped->action != ePed_action_falling && ped->action != ePed_action_dead && ped->action != ePed_action_impacting) {
                if (ped->action == ePed_action_falling || (face_cache->field_0x30 != NULL && ped_pos->v[1] - cache_pos1.v[1] - GetClearanceFromCharacterInstance(ped->character, 0) <= ped->character->personality->jump_height + .4347826f && gPed_cos_max_slope < face_cache->field_0x30->normal.v[1])) {
                    SetCharacterPositionAR(ped->character, &cache_pos1, 1);
                    if (ped->field_0x0c->field_0x6c) {
                        StopSmoothTurning(ped);
                        SetPedXZDirection(ped, &ped->field_0x0c->field_0x54, 0.f, the_time);
                    } else {
                        SetPedXZDirection(ped, &ped->character->field_0xc0, 0.f, the_time);
                    }
                    cache_bool2 = 1;
                } else {
                    if (!(ped->flags & 0x40)) {
                        MakePedVanish(ped);
                        continue;
                    }
                    face_cache->field_0x30 = NULL;
                    SetThisPedPhysicing(ped);
                    SetPedMove(ped, 72, -1, 0, 0, the_time, ePed_action_falling);
                    ped->action = ePed_action_falling;
                }
            }

            if (!(ped->character->field_0x14 & 0x4) && ped->hit_points > 0) {
                if (gBOOL_00744804) {
                    cache_bool2 = 1;
                }
                if (!cache_bool2) {
                    cache_bool2 = BrVector3Dot(&ped->character->field_0xc0, &ped->field_0x0c->field_0x0c) < .9998f;
                }
                if (cache_bool2 && face_cache->field_0x30 != NULL && !ped->field_0x0c->field_0x6c) {
                    RescanPedProximity(ped, NULL);
                }
                if (face_cache->field_0x30 != NULL && PedAnimCausesMovement(ped)) {
                    if (ped->field_0x0c->field_0x6c) {
                        SmoothTurnPedestrian(ped, the_time);
                        RescanPedProximity(ped, NULL);
                    } else {
                        RandomWander(ped, the_time);
                    }
                    PedScanForObjects(ped, the_time);
                    if (ped->field_0x0c->field_0x18) {
                        CheckForAvoidingAction(ped, the_time);
                    }
                }
            }
        }
        if (!gAction_replay_mode) {
            if (!(ped->character->field_0x14 & 0x4)
                    && MUNGE_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL
                    && MUNGE_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.f
                    && gGravity_multiplier == 1.f
                    && MUNGE_PED_COLLISION_OBJECT(ped)->water_depth_factor == 1.f) {
                SetThisPedPhysicing(ped);
                SetRandomOmega(MUNGE_PED_COLLISION_OBJECT(ped), 2.f);
            }
        }
        {
            int cur_move;
            int lut_id;

            cur_move = ped->character->personality->form->moves[ped->character->field_0x7].id;
            lut_id = -1;
            if (gDrunk_pedestrians) {
                lut_id = 4;
            } else if (gDancing_peds) {
                lut_id = 3;
            } else if (gPanicking_peds) {
                lut_id = 5;
            }
            if (lut_id >= 0) {
                if (cur_move != gPed_move_fsm[cur_move][lut_id + 1]) {
                    SetPedMove(ped, gPed_move_fsm[cur_move][lut_id + 1], -1, 0, 0, the_time, -1);
                }
            } else if (cur_move == 112 || cur_move == 31 || cur_move == 111) {
                SetPedMove(ped, 30, -1, 0, 0, the_time, -1);
            }
        }
        MungeCharacterAnimation(ped->character, the_time, 0);
        if (!(ped->character->field_0x14 & 0x4)) {
            tPhysics_object* simple_coll_info = ped->character->personality->form->simple_physicing[ped->character->field_0x5].collision_info;
            if (simple_coll_info != NULL) {
                BrVector3Copy(&simple_coll_info->v, &ped->character->field_0xd8);
                if (ped->hit_points <= 0
                        && ped->action == ePed_action_dead && ped->character->personality->form->moves[ped->character->field_0x7].id != 113
                        && ((gMutant_speed != 0.f && ped->character->field_0x1e == 0)
                                || (gMutant_speed == .0f && ped->character->field_0x1e != 0))) {
                    float still_speed;
                    extern void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime);

                    if (gMutant_speed == 0.f) {
                        still_speed = 0.f;
                    } else {
                        still_speed = -gMutant_speed;
                    }
                    SetCharacterMoveAR(ped->character, -1, still_speed, 0, 0, the_time);
                }
            }
        }
        BrVector3Copy(&ped->pos, ped_pos);
        if (!gAction_replay_mode) {
            /* MungePedHeadAnim, inlined in the original */
            if (ped->field_0x0c != NULL
                    && ped->field_0x0c->field_0x1c != NULL
                    && the_time > ped->field_0x0c->field_0x78 + 125) {

                ped->field_0x0c->field_0x1a += 1;
                if (ped->field_0x0c->field_0x1a >= ped->field_0x0c->field_0x1c->count) {
                    ped->field_0x0c->field_0x1a = 1;
                }
                if (ped->field_0x0c->field_0x1c->poses[ped->field_0x0c->field_0x1a] == 'c') {
                    ped->field_0x0c->field_0x1c = NULL;
                } else {
                    tPed_character_instance* head_character;
                    int head_pose;
                    int head_bone;
                    int head_hit;

                    ped->field_0x0c->field_0x78 = the_time;
                    head_hit = ped->hit_points <= 0;
                    head_character = ped->character;
                    head_pose = ped->field_0x0c->field_0x1c->poses[ped->field_0x0c->field_0x1a];
                    head_bone = head_character->personality->form->index_head_bone;
                    if (head_character->field_0x4 >= 0) {
                        if (head_bone < 0) {
                            PipeSinglePedModelChange(head_character->ped, head_bone, NULL, -1, GetCharacterModelSet(head_character), head_hit);
                            SetCharacterAllBonesModel(head_character, head_pose, head_hit);
                        } else {
                            PipeSinglePedModelChange(head_character->ped, head_bone, GetCharacterBoneModel(head_character, head_bone), head_pose, GetCharacterModelSet(head_character), head_hit);
                            SetCharacterBoneModel(head_character, head_bone, head_pose, head_hit);
                        }
                    }
                }
            }
            {
                extern void C2_HOOK_FASTCALL MakeEmBleed(tPedestrian* pPed, tU32 pTime);

                MakeEmBleed(ped, the_time);
            }
            if (scare_now && (ped->action == ePed_action_running || ped->hit_points <= 0 || ped->action == ePed_action_impacting)) {
                unsigned int j;

                for (j = 0; j < count_scared_pedestrians; j++) {
                    if (scared_pedestrians[j]->action == ePed_action_walking && Vector3DistanceSquared(ped_pos, &scared_pedestrians[j]->pos) < REC2_SQR(3.f)) {
                        ScarePedestrian(ped, the_time, 1, 0);
                    }
                }
            }
        }
    }

    gPed_nearness = 0;
    gCount_changed_points = 0;
}
// Placement is load-bearing: MakePedNoise must be defined AFTER DisposePedPersonality (0x4077f0).
// A body at the old placeholder position (~line 1707) adds one more function to the TU and flips
// DisposePedPersonality's frame 0x10 -> 0xc (reccmp 100 -> 68.21). At this position (end of the TU,
// after MungePedestrians) MakePedNoise is still 100 and the neighbours 0x4077f0, 0x40b770, 0x4042f0
// and 0x404290 stay 100.
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004cd880
void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info) {
    extern char gPed_noise_table[16];
    extern float gAverage_ped_scale;
    typedef struct { tU16 count_sounds; tU16 sounds[5]; } tPed_sound_group;
    tPed_personality_sounds* sounds;
    tPed_sound_group* sound_group;
    int sound;

    if (pPed->field_0x0c == NULL) {
        return;
    }
    if (pPed->field_0x0c->field_0x9c != 0
            && pPed->field_0x0c->field_0xa0 >= 0
            && DRS3SoundStillPlaying(pPed->field_0x0c->field_0x9c)) {
        if (pPed->field_0x0c->field_0xa0 == pArg2) {
            return;
        }
        if (gPed_noise_table[pArg2] != 0) {
            return;
        }
        if (gPed_noise_table[pPed->field_0x0c->field_0xa0] == 0) {
            return;
        }
        DRS3StopSound(pPed->field_0x0c->field_0x9c);
        pPed->field_0x0c->field_0x9c = 0;
    }
    pPed->field_0x0c->field_0x20[0] = pArg3;
    pPed->field_0x0c->field_0xa0 = pArg2;
    sounds = pPed->character->personality->sounds;
    sound_group = (tPed_sound_group*)&sounds->sounds[pArg2];
    sound = sound_group->sounds[IRandomBetween(0, sound_group->count_sounds - 1)];
    if (pCollision_info != NULL
            && pCollision_info->owner != NULL
            && pCollision_info->flags_0x238 == 1
            && ((tCar_spec*)pCollision_info->owner)->driver == eDriver_local_human) {
        pPed->field_0x0c->field_0x9c = DRS3StartSound2(gPedestrians_outlet, sound, pArg3 == 0, 0xff, 0xff, (int)(65536.0 / gAverage_ped_scale), 0x10000);
    } else {
        pPed->field_0x0c->field_0x9c = DRS3StartSound3D(gPedestrians_outlet, sound, &pPed->pos, &gZero_v__car, pArg3 == 0, 0xff, (int)(65536.0 / gAverage_ped_scale), 0x10000);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// GLOBAL: CARMA2_HW 0x0065d758
char gPed_noise_table[16] = {
    0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

/* No standalone symbols in the original: SetNextRandomTurn and InitProcessData are inlined at
   their only call sites (MungePedestrians 0x4d3b36-0x4d3bce). Runtime-faithful bodies only;
   placed at the end of the TU so they cannot shift MakePedNoise (0x4cd880) or the neighbours. */
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
void C2_HOOK_FASTCALL InitProcessData(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* ped_field_0x0c;

    ped_field_0x0c = pPed->field_0x0c;
    if (ped_field_0x0c->field_0xb0 != NULL) {
        ped_field_0x0c->field_0xb0->field_0x14 = NULL;
        ped_field_0x0c->field_0xb0 = NULL;
    }
    if (pPed->movement_spec->max_time_between != 0) {
        ped_field_0x0c->next_turn_time = pTime + IRandomBetween(pPed->movement_spec->min_time_between, pPed->movement_spec->max_time_between);
    } else {
        ped_field_0x0c->next_turn_time = 0;
    }
    ped_field_0x0c->field_0x18 = 0;
    ped_field_0x0c->field_0x6c = 0;
    ped_field_0x0c->field_0x80 = 0;
    ped_field_0x0c->field_0x19 = 0;
    ped_field_0x0c->field_0xc0 = NULL;
    ped_field_0x0c->field_0x1c = NULL;
    ped_field_0x0c->field_0x2e = 0;
    ped_field_0x0c->field_0x21 = 0;
    ped_field_0x0c->field_0xa0 = -1;
    ped_field_0x0c->field_0x9c = 0;
    ped_field_0x0c->field_0x88 = 0;
    ped_field_0x0c->field_0x8c = 0;
    ped_field_0x0c->field_0x84 = gPed_spurting_period;
    ped_field_0x0c->field_0xc4 &= ~0x1;
    ped_field_0x0c->field_0x94 = 0;
    ped_field_0x0c->field_0x90 = 0;
    ped_field_0x0c->field_0x98 = 0;
    ped_field_0x0c->field_0x23[0] = 0;
    ped_field_0x0c->field_0x23[1] = 0;
    ped_field_0x0c->field_0x23[2] = 0;
    ped_field_0x0c->field_0x23[3] = 0;
    ped_field_0x0c->field_0x23[4] = 0;
    ped_field_0x0c->field_0x23[5] = 0;
    ped_field_0x0c->field_0x23[6] = 0;
    ped_field_0x0c->field_0x23[7] = 0;
    ped_field_0x0c->field_0x23[8] = 0;
    ped_field_0x0c->field_0x23[9] = 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004d1c60
void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime) {
    br_vector3 original_pos;
    br_matrix34* matrix;
    int original_move_id;
    int original_field_0x1c;
    float move_anim;

    if (gAction_replay_mode == 0 || gBOOL_00744804 != 0) {
        if (pSpeed == 0.f) {
            pSpeed = 0.f;
        }
        matrix = GetCharacterMatrixPtr(pCharacter);
        original_pos.v[0] = matrix->m[3][0];
        original_pos.v[1] = matrix->m[3][1];
        original_pos.v[2] = matrix->m[3][2];
        original_move_id = pCharacter->personality->form->moves[pCharacter->field_0x7].id;
        original_field_0x1c = (tS16)pCharacter->field_0x1c;
        move_anim = (float)(tS16)pCharacter->field_0x1e;
        SetCharacterMove(pCharacter, pMove_action, pSpeed, pArg4, pArg5, pTime);
        if (pMove_action == -1) {
            PipeSinglePedFPChange(pCharacter->ped, (float)(tS16)pCharacter->field_0x1e, pSpeed);
            return;
        }
        PipeSinglePedMove(pCharacter->ped, (tS16)original_field_0x1c, (tS16)original_move_id, (tS16)pMove_action,
            move_anim, pSpeed, (int)pArg5, (int)pArg4, &original_pos, (br_vector3*)matrix->m[3],
            -1, -1, &pCharacter->field_0x8c);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* AcceleratePed: no standalone symbol in the original (inlined into MungePedestrians); runtime-faithful body.
   Must NOT be defined near its old placeholder slot: a function body at that position flips
   DisposePedPersonality 0x4077f0 from 100% to 68.21%. */
void C2_HOOK_FASTCALL AcceleratePed(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* ped_cache;

    ped_cache = pPed->field_0x0c;
    if (ped_cache != NULL && ped_cache->field_0x80 != 0) {
        int speed;

        if (!gAction_replay_mode || ARGetReplayRate() >= 0.f) {
            speed = (int)((float)ped_cache->field_0x80 - (float)gFrame_period * ped_cache->field_0xb8);
            ped_cache->field_0x80 = speed;
            if (speed <= 30) {
                speed = 30;
                ped_cache->field_0x80 = 0;
            }
            ped_cache->field_0x80 = MAX(30, ped_cache->field_0x80);
        } else {
            speed = (int)((float)ped_cache->field_0x80 + (float)gFrame_period * ped_cache->field_0xb8);
            ped_cache->field_0x80 = speed;
            if (speed >= 100) {
                speed = 100;
                ped_cache->field_0x80 = 0;
            }
        }
        if (gPedestrian_speed_factor == 0.f) {
            SetCharacterMoveAR(pPed->character, -1, (float)speed, 0, 0, pTime);
        } else {
            SetCharacterMoveAR(pPed->character, -1, (float)speed / gPedestrian_speed_factor, 0, 0, pTime);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// GLOBAL: CARMA2_HW 0x0058aae0
double gPed_face_cache_margin = -0.21739130434782608;

// GLOBAL: CARMA2_HW 0x0058aae8
double gPed_face_normal_threshold = 0.99;

#define RECACHE_FACE_BOUNDS(cache__) ((br_bounds3*)&(cache__)->field_0x18)

// GLOBAL: CARMA2_HW 0x00694124
int gPed_face_cache_flags;

// GLOBAL: CARMA2_HW 0x0058aaf0
const float gPed_face_bounds_min_x = -0.43478259444236755f;

// GLOBAL: CARMA2_HW 0x0058aaf8
const double gPed_face_bounds_vertical_margin = -0.36231884057971014;

// GLOBAL: CARMA2_HW 0x0058ab00
const float gPed_face_bounds_min_z = -0.28985506296157837f;

// GLOBAL: CARMA2_HW 0x0058ab04
const float gPed_face_bounds_max_x = 0.43478259444236755f;

// GLOBAL: CARMA2_HW 0x0058ab08
const float gPed_face_bounds_max_z = 1.4492753744125366f;

// GLOBAL: CARMA2_HW 0x0065e568
const br_matrix34 gPed_face_bounds_identity = {
    { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }
};

typedef struct {
    br_matrix34 matrix;
    tBounds bounds;
} tPed_face_bounds_context;

extern tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pCount, br_scalar pOffset, br_vector3* pCollision_position);
extern void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);
extern tWorld_callbacks gWorld_callbacks;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004d2060
void C2_HOOK_FASTCALL CachePedestrianFaces(tPedestrian* pPed, br_vector3* pHit_pos, tU32 pCache_key) {
    tPed_face_bounds_context bounds_context;
    br_matrix34* character_matrix;
    tPed_face_cache_0x34* face_cache;
    tPed_face_cache_0x34* head;
    int* cache_size;
    int face_count;
    int cache_index;

    if (pPed->hit_points <= 0 && gBOOL_00744804 == 0) {
        SetPedMove(pPed, 0x5e, -1, 0, 1, GetTotalTime(), 6);
        return;
    }

    character_matrix = GetCharacterMatrixPtr(pPed->character);
    bounds_context.matrix.m[0][0] = pPed->character->field_0x5c.m[0][0];
    bounds_context.matrix.m[0][1] = pPed->character->field_0x5c.m[0][1];
    bounds_context.matrix.m[0][2] = pPed->character->field_0x5c.m[0][2];
    bounds_context.matrix.m[1][0] = pPed->character->field_0x5c.m[1][0];
    bounds_context.matrix.m[1][1] = pPed->character->field_0x5c.m[1][1];
    bounds_context.matrix.m[1][2] = pPed->character->field_0x5c.m[1][2];
    bounds_context.matrix.m[2][0] = pPed->character->field_0x5c.m[2][0];
    bounds_context.matrix.m[2][1] = pPed->character->field_0x5c.m[2][1];
    bounds_context.matrix.m[2][2] = pPed->character->field_0x5c.m[2][2];
    bounds_context.matrix.m[3][0] = character_matrix->m[3][0];
    bounds_context.matrix.m[3][1] = character_matrix->m[3][1];
    bounds_context.matrix.m[3][2] = character_matrix->m[3][2];

    bounds_context.bounds.mat = &bounds_context.matrix;
    bounds_context.bounds.original_bounds.min.v[0] = gPed_face_bounds_min_x;
    bounds_context.bounds.original_bounds.min.v[1] = (br_scalar)(gPed_face_bounds_vertical_margin - pPed->character->personality->jump_height);
    bounds_context.bounds.original_bounds.min.v[2] = gPed_face_bounds_min_z;
    bounds_context.bounds.original_bounds.max.v[0] = gPed_face_bounds_max_x;
    bounds_context.bounds.original_bounds.max.v[1] = (br_scalar)(pPed->character->personality->jump_height - gPed_face_bounds_vertical_margin);
    bounds_context.bounds.original_bounds.max.v[2] = gPed_face_bounds_max_z;
    FillInBounds(&bounds_context.bounds);

    bounds_context.bounds.original_bounds.min.v[0] = bounds_context.bounds.real_bounds.min.v[0];
    bounds_context.bounds.original_bounds.min.v[1] = bounds_context.bounds.real_bounds.min.v[1];
    bounds_context.bounds.original_bounds.min.v[2] = bounds_context.bounds.real_bounds.min.v[2];
    bounds_context.bounds.original_bounds.max.v[0] = bounds_context.bounds.real_bounds.max.v[0];
    bounds_context.bounds.original_bounds.max.v[1] = bounds_context.bounds.real_bounds.max.v[1];
    bounds_context.bounds.original_bounds.max.v[2] = bounds_context.bounds.real_bounds.max.v[2];
    bounds_context.bounds.mat = (br_matrix34*)&gPed_face_bounds_identity;
    face_count = FindFacesInBox(&bounds_context.bounds, (tFace_ref*)gPed_face_cache, gPed_cache_sizes_2[3] + 1, &gWorld_callbacks);
    if (face_count > gPed_cache_sizes_2[3]) {
        gPed_face_cache_flags |= 2;
        face_count = gPed_cache_sizes_2[3];
    }

    cache_index = 0;
    cache_size = gPed_cache_sizes_2;
    do {
        if (face_count <= *cache_size) {
            break;
        }
        cache_size++;
        cache_index++;
    } while (cache_size < &gPed_cache_sizes_2[4]);

    face_cache = pPed->field_0x0c->field_0xb0;
    if (face_cache == NULL || face_cache->count != gPed_cache_sizes_2[cache_index]) {
        if (face_cache != NULL) {
            face_cache->field_0x14 = NULL;
        }
        face_cache = gPed_face_caches[cache_index];
        if (face_cache->field_0x14 != NULL) {
            face_cache->field_0x14->field_0x0c->field_0xb0 = NULL;
            if (face_cache->field_0x8 == pCache_key) {
                gPed_face_cache_flags |= 1;
            }
        }
    }

    head = gPed_face_caches[cache_index];
    if (face_cache == head) {
        gPed_face_caches[cache_index] = head->next;
    } else {
        face_cache->next->prev = face_cache->prev;
        face_cache->prev->next = face_cache->next;
        face_cache->prev = head->prev;
        face_cache->next = head;
        head->prev->next = face_cache;
        head->prev = face_cache;
    }

    memcpy(face_cache->field_0x0, gPed_face_cache, face_count * sizeof(tFace_ref));
    face_cache->field_0x8 = pCache_key;
    face_cache->field_0x14 = pPed;
    face_cache->field_0x4 = (tU8)face_count;
    *RECACHE_FACE_BOUNDS(face_cache) = bounds_context.bounds.real_bounds;
    pPed->field_0x0c->field_0xb0 = face_cache;

    if (pPed->field_0x0c != NULL) {
        face_cache = pPed->field_0x0c->field_0xb0;
        character_matrix = GetCharacterMatrixPtr(pPed->character);
        face_cache->field_0x30 = CastRayDownThruFaces((br_vector3*)character_matrix->m[3], (tFace_ref*)face_cache->field_0x0,
                face_cache->field_0x4, GetClearanceFromCharacterInstance(pPed->character, 0), pHit_pos);
    }
}
// FUNCTION: CARMA2_HW 0x004d1d70
tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime) {
    br_matrix34* mat;
    tPed_face_cache_0x34* face_cache;
    tFace_ref* face;
    br_vector3* pos;
    float f3;
    float f4;
    float f5;
    float f6;
    float f7;
    float f8;
    int inside;

    *pArg2 = 0;
    *pArg3 = 0;
    mat = GetCharacterMatrixPtr(pPed->character);
    pos = (br_vector3*)mat->m[3];
    face_cache = pPed->field_0x0c->field_0xb0;
    if (pArg5 != 0 || face_cache == NULL
            || gPed_face_cache_margin < pos->v[0] - RECACHE_FACE_BOUNDS(face_cache)->max.v[0]
            || gPed_face_cache_margin < pos->v[1] - RECACHE_FACE_BOUNDS(face_cache)->max.v[1]
            || gPed_face_cache_margin < pos->v[2] - RECACHE_FACE_BOUNDS(face_cache)->max.v[2]
            || gPed_face_cache_margin < RECACHE_FACE_BOUNDS(face_cache)->min.v[0] - pos->v[0]
            || gPed_face_cache_margin < RECACHE_FACE_BOUNDS(face_cache)->min.v[1] - pos->v[1]
            || gPed_face_cache_margin < RECACHE_FACE_BOUNDS(face_cache)->min.v[2] - pos->v[2]) {
        CachePedestrianFaces(pPed, pArg4, pTime);
        face_cache = pPed->field_0x0c->field_0xb0;
        *pArg2 = face_cache != NULL;
        *pArg3 = 1;
    } else {
        face = face_cache->field_0x30;
        inside = 1;
        if (face != NULL) {
            f6 = face->v[1].v[0] - face->v[0].v[0];
            f3 = face->v[1].v[2] - face->v[0].v[2];
            f8 = face->v[2].v[2] - face->v[0].v[2];
            f7 = face->v[2].v[0] - face->v[0].v[0];
            f4 = pos->v[2] - face->v[0].v[2];
            f5 = pos->v[0] - face->v[0].v[0];
            if (gConst_replay_rate_zero <= (f6 * f8 - f3 * f7) * (f6 * f4 - f3 * f5)
                    && gConst_replay_rate_zero <= (f7 * f3 - f8 * f6) * (f7 * f4 - f8 * f5)) {
                f3 = face->v[2].v[0] - face->v[1].v[0];
                f4 = face->v[2].v[2] - face->v[1].v[2];
                inside = ((face->v[0].v[2] - face->v[1].v[2]) * f3
                        - (face->v[0].v[0] - face->v[1].v[0]) * f4)
                        * ((pos->v[2] - face->v[1].v[2]) * f3
                        - (pos->v[0] - face->v[1].v[0]) * f4) < gConst_replay_rate_zero;
            }
        }
        if (inside) {
            if (pPed->field_0x0c != NULL) {
                mat = GetCharacterMatrixPtr(pPed->character);
                face_cache->field_0x30 = CastRayDownThruFaces((br_vector3*)mat->m[3], (tFace_ref*)face_cache->field_0x0,
                        (int)face_cache->field_0x4, GetClearanceFromCharacterInstance(pPed->character, 0), pArg4);
            }
            *pArg2 = 1;
            if (face == NULL || face_cache->field_0x30 == NULL
                    || face->normal.v[0] * pPed->field_0x0c->field_0x00.v[0]
                        + pPed->field_0x0c->field_0x00.v[2] * face->normal.v[2]
                        + pPed->field_0x0c->field_0x00.v[1] * face->normal.v[1] < gPed_face_normal_threshold) {
                *pArg3 = 1;
            }
        }
    }
    return face_cache;
}
#undef RECACHE_FACE_BOUNDS
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* No standalone address: the original compiler inlines this into every caller
   (e.g. 0x4d41eb inside the 0x4d3740 body); case 0x4d41eb is the reference. */
void C2_HOOK_FASTCALL SetCharacterPositionAR(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3) {
    br_vector3 current_pos;
    br_matrix34* mat;

    mat = GetCharacterMatrixPtr(pCharacter);
    current_pos.v[0] = mat->m[3][0];
    current_pos.v[1] = mat->m[3][1];
    current_pos.v[2] = mat->m[3][2];
    SetCharacterPosition(pCharacter, pPos, pArg3);
    if (!(pCharacter->field_0x14 & 0x4)) {
        PipeSinglePedPos(pCharacter->ped, &current_pos, (br_vector3*)GetCharacterMatrixPtr(pCharacter)->m[3]);
    }
}

/* SetPedXZDirection 0x004cc2c0: re-derived from the original (no upstream C in the fork);
   the tail mirrors SetCharacterDirectionAR 0x004d3360 with OrientationChanged /
   AssertRootObjectsMatrix inlined. DAT_0075bba8 is gProgram_state.racing. */
extern br_vector3 gY_unit_vector;

// FUNCTION: CARMA2_HW 0x004cc2c0
void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pArg3, tU32 pTime) {
    tPed_face_cache_0x34* face_cache;
    tPed_character_instance* pCharacter;
    tPhysics_object* collision_info;
    br_vector3* pDirxz;
    br_matrix34* m1;
    br_matrix34* m2;
    br_vector3 cross1;
    br_vector3 cross2;
    float len;
    float scale;
    float value;
    int i;
    int j;

    face_cache = pPed->field_0x0c->field_0xb0;
    if (face_cache == NULL) {
        return;
    }
    if (face_cache->field_0x30 == NULL) {
        return;
    }
    DRVector3SafeCross(&cross1, &gY_unit_vector, pDir);
    DRVector3SafeCross(&cross2, &cross1, &face_cache->field_0x30->normal);
    len = BR_LENGTH3(cross2.v[0], cross2.v[1], cross2.v[2]);
    if (len > BR_SCALAR_EPSILON * 2) {
        scale = 1.0 / len;
        cross1.v[0] = cross2.v[0] * scale;
        cross1.v[1] = cross2.v[1] * scale;
        cross1.v[2] = cross2.v[2] * scale;
    } else {
        cross1.v[0] = 1.0f;
        cross1.v[1] = 0.0f;
        cross1.v[2] = 0.0f;
    }
    if (pPed->field_0x0c->field_0x6c != 0 && BrVector3Dot(&pPed->field_0x0c->field_0x48, &cross1) > 0.95) {
        return;
    }
    pPed->field_0x0c->field_0x48.v[0] = cross1.v[0];
    pPed->field_0x0c->field_0x48.v[1] = cross1.v[1];
    pPed->field_0x0c->field_0x48.v[2] = cross1.v[2];
    if (pArg3 != 0.f) {
        pPed->field_0x0c->field_0x54.v[0] = pPed->field_0x0c->field_0x48.v[0];
        pPed->field_0x0c->field_0x54.v[1] = pPed->field_0x0c->field_0x48.v[1];
        pPed->field_0x0c->field_0x54.v[2] = pPed->field_0x0c->field_0x48.v[2];
        len = BR_LENGTH3(pPed->character->field_0xd8.v[0], pPed->character->field_0xd8.v[1], pPed->character->field_0xd8.v[2]);
        value = (pArg3 / len) * (float)(tS16)pPed->character->field_0x1e * (1.f / 30.f);
        if (value > 0.5) {
            value = 0.5f;
        }
        pPed->field_0x0c->field_0x6c = pTime;
        pPed->field_0x0c->field_0x70 = pTime - value * -1000.f;
        pPed->field_0x0c->field_0x60.v[0] = pPed->character->field_0xc0.v[0];
        pPed->field_0x0c->field_0x60.v[1] = pPed->character->field_0xc0.v[1];
        pPed->field_0x0c->field_0x60.v[2] = pPed->character->field_0xc0.v[2];
        return;
    }
    pPed->field_0x0c->field_0x6c = 0;
    pCharacter = pPed->character;
    pDirxz = &pPed->field_0x0c->field_0x48;
    if (pCharacter->field_0x14 & 0x1) {
        collision_info = pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
    } else {
        collision_info = GetRootObject(pCharacter);
    }
    if (!gAction_replay_mode || gBOOL_00744804) {
        if (!(pCharacter->field_0x14 & 0x4) && gProgram_state.racing) {
            PipeSinglePedDir((((short)pCharacter->ped->field_0x06) << 16) | (pCharacter->ped - gPedestrian_array), pDirxz);
        }
        SetCharacterDirection(pCharacter, pDirxz, &pCharacter->field_0xcc);
        if (gProgram_state.racing) {
            m1 = &collision_info->transform_matrix;
            m2 = &collision_info->actor->t.t.mat;
            if (BrVector3Dot((br_vector3*)m1->m[0], (br_vector3*)m2->m[0]) < 0.7f
                    || BrVector3Dot((br_vector3*)m1->m[1], (br_vector3*)m2->m[1]) < 0.7f
                    || BrVector3Dot((br_vector3*)m2->m[2], (br_vector3*)m1->m[2]) < 0.7f) {
                if (pCharacter->field_0x14 & 0x1) {
                    collision_info = pCharacter->personality->form->simple_physicing[pCharacter->field_0x5].collision_info;
                } else {
                    collision_info = GetRootObject(pCharacter);
                }
                for (i = 0; i < 3; i++) {
                    for (j = 0; j < 3; j++) {
                        collision_info->transform_matrix.m[i][j] = collision_info->actor->t.t.mat.m[i][j];
                    }
                }
                collision_info->flags |= 0x100;
            }
        }
    }
}

/* RescanPedProximity 0x004cfab0: re-derived from the original (the fork's body at 39.46% has wrong
   triangle/phase logic, so its code was not reused). The types
   below mirror what the original walks: head->field_0x0 is an array of 0x50-byte face entries,
   entry->field_0x48 selects an adjacency block whose records hold the three vertex indices, and
   the per-vertex side tests are memoised in a 16-entry flag cache at 0x0069bd08. */
typedef struct {
    undefined field_0x00[0x0c];
    void* field_0x0c;
    tU16 field_0x10;
    tU16 field_0x12;
    undefined field_0x14[0x38];
    void* field_0x4c;
} tPed_prox_adjacency;

typedef struct {
    undefined field_0x00[0x18];
    tPed_prox_adjacency* field_0x18;
} tPed_prox_face_block;

typedef struct {
    undefined field_0x00[0x08];
    tU16 field_0x08;
    undefined field_0x0a[0x0e];
    void* field_0x18;
} tPed_prox_level;

typedef struct {
    undefined field_0x00[0x04];
    void* field_0x04;
    undefined field_0x08[0x08];
    void* field_0x10;
    undefined field_0x14[0x08];
    tU16 field_0x1c;
    undefined field_0x1e[0x06];
} tPed_prox_row;

typedef struct {
    br_vector3 point;
    undefined field_0x0c[0x14];
} tPed_prox_vertex;

typedef struct {
    br_material* material;
    br_vector3 normal1;
    br_vector3 normal2;
    br_vector3 normal3;
    undefined field_0x28[0x0c];
    br_vector3 plane;
    undefined field_0x40[0x04];
    float d;
    void* field_0x48;
    tS16 index;
    tU16 field_0x4e;
} tPed_prox_entry;

typedef struct {
    void* field_0x00;
    int field_0x04;
    tU16 flags[0x200];
} tPed_prox_flag_cell;

// GLOBAL: CARMA2_HW 0x0069bd08
tPed_prox_flag_cell gPed_prox_flag_cells[16];

/* One triangle-vertex side test with the original's memoisation (it is inlined there): a cached
   entry returns the stored result, a miss evaluates and caches test/result bits. */
static int C2_HOOK_FASTCALL PedProxSideTest(tPed_prox_flag_cell* pCell, int pIndex, br_vector3* pPoint, br_vector3* pNormal, br_scalar pD, tU16 pTest_flag, tU16 pResult_flag) {
    int side;

    if (pCell != NULL && pIndex < 0x200) {
        if ((pCell->flags[pIndex] & pTest_flag) != 0) {
            return pCell->flags[pIndex] & pResult_flag;
        }
        side = NormalSideOfPlane(pPoint, pNormal, pD);
        pCell->flags[pIndex] |= pTest_flag;
        if (side != 0) {
            pCell->flags[pIndex] |= pResult_flag;
        }
        return side;
    }
    return NormalSideOfPlane(pPoint, pNormal, pD);
}

// FUNCTION: CARMA2_HW 0x004cfab0
float C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, br_vector3* pArg2) {
    tPed_cache_006944c0* cache;
    tPed_face_cache_0x34* head;
    tFace_ref* face;
    tPed_prox_entry* entry;
    tPed_prox_face_block* adj;
    tPed_prox_adjacency* adj2;
    tPed_prox_level* level;
    tPed_prox_row* row;
    tPed_prox_vertex* verts;
    tPed_prox_vertex* v0;
    tPed_prox_vertex* v1;
    tPed_prox_vertex* v2;
    tPed_prox_flag_cell* cell;
    tRace_ped_exclusion_spec* spec;
    br_vector3* pPos;
    br_vector3* pSrc;
    br_vector3 dir;
    br_vector3 n;
    br_vector3 neg;
    br_vector3 neg_normal;
    br_vector3 v;
    br_vector3 w;
    tU16* rec;
    br_scalar best;
    br_scalar result = 0.f;
    br_scalar len;
    br_scalar scale;
    br_scalar nx;
    br_scalar nz;
    br_scalar dot1;
    br_scalar dot2;
    br_scalar d0;
    br_scalar d1;
    br_scalar denom;
    br_scalar e1x;
    br_scalar e1z;
    br_scalar e2x;
    br_scalar e2z;
    br_scalar e3x;
    br_scalar e3z;
    br_scalar c1;
    br_scalar c2;
    br_scalar c3;
    br_scalar t1;
    br_scalar t2;
    br_scalar t3;
    br_scalar ex;
    br_scalar ez;
    br_scalar k;
    int is_running;
    int matched;
    int done;
    int cell_count;
    int entry_count;
    int i;
    int j;
    int idx;
    int idx0;
    int idx1;
    int idx2;
    tU16 id;

    cache = pPed->field_0x0c;
    head = cache->field_0xb0;
    if (head == NULL) {
        return 10000.f;
    }
    face = head->field_0x30;
    if (face == NULL) {
        return 10000.f;
    }
    cache->field_0x00 = face->normal;

    pSrc = pArg2 != NULL ? pArg2 : &pPed->character->field_0xc0;
    len = (br_scalar)sqrt(pSrc->v[0] * pSrc->v[0] + pSrc->v[1] * pSrc->v[1] + pSrc->v[2] * pSrc->v[2]);
    if (len > 2.3841858e-7f) {
        scale = (br_scalar)(1.0 / len);
        dir.v[0] = pSrc->v[0] * scale;
        dir.v[1] = pSrc->v[1] * scale;
        dir.v[2] = pSrc->v[2] * scale;
    } else {
        dir.v[0] = 1.f;
        dir.v[1] = 0.f;
        dir.v[2] = 0.f;
    }
    cache->field_0x0c = dir;

    best = 10000.f;
    cache->field_0x18 &= 0xf4;

    len = (br_scalar)sqrt(dir.v[0] * dir.v[0] + dir.v[2] * dir.v[2]);
    nx = dir.v[0] / len;
    nz = dir.v[2] / len;

    is_running = pPed->action == ePed_action_running;
    pPos = (br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3];

    entry = (tPed_prox_entry*)head->field_0x0;
    entry_count = ((tU8*)head)[0x4];
    if (entry_count == 0) {
        return best;
    }

    done = 0;
    cell_count = 0;
    for (i = 0; i < entry_count; i++, entry++) {
        matched = 0;
        if (entry->plane.v[1] > gPed_cos_max_slope) {
            if (gPeds_suicidal) {
                continue;
            }
            spec = pPed->exclusion_spec;
            if (spec == NULL) {
                continue;
            }
            if (is_running && spec->exclusions_ok_when_scared) {
                continue;
            }
            for (j = 0; j < spec->count_exclusion_materials; j++) {
                if (spec->exclusion_materials[j].material == entry->material
                        && (!is_running || spec->exclusion_materials[j].flags == 0)) {
                    break;
                }
            }
            if (j == spec->count_exclusion_materials) {
                continue;
            }
            matched = 1;
        } else {
            spec = pPed->exclusion_spec;
            if (spec != NULL) {
                for (j = 0; j < spec->count_exception_materials; j++) {
                    if (spec->exception_materials[j] == entry->material) {
                        break;
                    }
                }
                if (j != spec->count_exception_materials) {
                    continue;
                }
            }
        }

        if (!matched) {
            denom = BrVector3Dot(&dir, &entry->plane);
            if (fabs(denom) < 0.001) {
                result = 20.f;
            } else {
                result = (entry->d - BrVector3Dot(pPos, &entry->plane)) / denom;
            }
            if (result < 0.f) {
                continue;
            }
            if (result > 20.f) {
                continue;
            }
            if (result > best) {
                continue;
            }
        }

        if (!done) {
            DRVector3SafeCross(&n, &gY_unit_vector, &dir);
            len = (br_scalar)sqrt(n.v[0] * n.v[0] + n.v[1] * n.v[1] + n.v[2] * n.v[2]);
            if (len > 2.3841858e-7f) {
                scale = (br_scalar)(1.0 / len);
                n.v[0] *= scale;
                n.v[1] *= scale;
                n.v[2] *= scale;
            } else {
                n.v[0] = 1.f;
                n.v[1] = 0.f;
                n.v[2] = 0.f;
            }
            neg.v[0] = -n.v[0];
            neg.v[1] = -n.v[1];
            neg.v[2] = -n.v[2];
            scale = (br_scalar)((double)pPed->character->personality->bb.max.v[0] * 1.3);
            v.v[0] = pPos->v[0] + neg.v[0] * scale;
            v.v[1] = pPos->v[1] + neg.v[1] * scale;
            v.v[2] = pPos->v[2] + neg.v[2] * scale;
            dot1 = BrVector3Dot(&v, &n);
            w.v[0] = pPos->v[0] + n.v[0] * scale;
            w.v[1] = pPos->v[1] + n.v[1] * scale;
            w.v[2] = pPos->v[2] + n.v[2] * scale;
            dot2 = BrVector3Dot(&w, &neg);
            d0 = BrVector3Dot(pPos, &dir);
            neg_normal.v[0] = -face->normal.v[0];
            neg_normal.v[1] = -face->normal.v[1];
            neg_normal.v[2] = -face->normal.v[2];
            d1 = BrVector3Dot(pPos, &neg_normal);
            done = 1;
        }

        adj = (tPed_prox_face_block*)entry->field_0x48;
        idx = entry->index;
        adj2 = adj->field_0x18;
        if (idx < 0) {
            id = entry->field_0x4e;
            if (id >= *(tU16*)((char*)adj2 + 0x12)) {
                continue;
            }
            rec = (tU16*)((char*)adj2->field_0x0c + id * 0x28);
        } else {
            level = (tPed_prox_level*)adj2->field_0x4c;
            if (idx >= level->field_0x08) {
                continue;
            }
            row = (tPed_prox_row*)((char*)level->field_0x18 + idx * 0x24);
            id = entry->field_0x4e;
            if (id >= row->field_0x1c) {
                continue;
            }
            rec = (tU16*)((char*)row->field_0x04 + id * 0x1c);
        }
        idx0 = rec[0];
        idx1 = rec[1];
        idx2 = rec[2];

        cell = NULL;
        for (j = 0; j < cell_count; j++) {
            if (gPed_prox_flag_cells[j].field_0x00 == adj2 && gPed_prox_flag_cells[j].field_0x04 == idx) {
                cell = &gPed_prox_flag_cells[j];
                break;
            }
        }
        if (cell == NULL && cell_count < 16) {
            cell = &gPed_prox_flag_cells[cell_count++];
            for (j = 0; j < 0x200; j++) {
                cell->flags[j] = 0;
            }
            cell->field_0x00 = adj2;
            cell->field_0x04 = idx;
        }

        /* The three movement half-spaces: along the direction, then the two extra planes of the
           pedestrian's breadth; the side test uses the three triangle-face normals. */
        if (!PedProxSideTest(cell, idx0, &entry->normal1, &dir, d0, 0x8, 0x800)
                && !PedProxSideTest(cell, idx1, &entry->normal2, &dir, d0, 0x8, 0x800)
                && !PedProxSideTest(cell, idx2, &entry->normal3, &dir, d0, 0x8, 0x800)) {
            continue;
        }
        if (!PedProxSideTest(cell, idx0, &entry->normal1, &n, dot1, 0x2, 0x200)
                && !PedProxSideTest(cell, idx1, &entry->normal2, &n, dot1, 0x2, 0x200)
                && !PedProxSideTest(cell, idx2, &entry->normal3, &n, dot1, 0x2, 0x200)) {
            continue;
        }
        if (!PedProxSideTest(cell, idx0, &entry->normal1, &neg, dot2, 0x4, 0x400)
                && !PedProxSideTest(cell, idx1, &entry->normal2, &neg, dot2, 0x4, 0x400)
                && !PedProxSideTest(cell, idx2, &entry->normal3, &neg, dot2, 0x4, 0x400)) {
            continue;
        }

        if (matched) {
            /* Sliced face: shoot the direction through the triangle in the XZ plane. The original
               keeps the v0/v1 cross term as the constant base term for all three edges. */
            /* The original re-walks level->rows here (it does not reuse the row above, so this
               also happens for a negative index). */
            level = (tPed_prox_level*)adj2->field_0x4c;
            row = (tPed_prox_row*)((char*)level->field_0x18 + idx * 0x24);
            verts = (tPed_prox_vertex*)row->field_0x10;
            v0 = &verts[idx0];
            v1 = &verts[idx1];
            v2 = &verts[idx2];
            k = v0->point.v[0] * v1->point.v[2] - v1->point.v[0] * v0->point.v[2];

            e1x = v1->point.v[0] - v0->point.v[0];
            e1z = v1->point.v[2] - v0->point.v[2];
            c1 = e1z * nx - e1x * nz;
            if (fabs(c1) <= 0.0001) {
                t1 = 3.402823466e+38f;
            } else {
                t1 = (k + pPos->v[2] * e1x - e1z * pPos->v[0]) / c1;
                if (t1 < 0.f) {
                    t1 = 3.402823466e+38f;
                }
            }
            e2x = v2->point.v[0] - v1->point.v[0];
            e2z = v2->point.v[2] - v1->point.v[2];
            c2 = e2z * nx - e2x * nz;
            if (fabs(c2) <= 0.0001) {
                t2 = 3.402823466e+38f;
            } else {
                t2 = (k + pPos->v[2] * e2x - e2z * pPos->v[0]) / c2;
                if (t2 < 0.f) {
                    t2 = 3.402823466e+38f;
                }
            }
            e3x = v0->point.v[0] - v2->point.v[0];
            e3z = v0->point.v[2] - v2->point.v[2];
            c3 = e3z * nx - e3x * nz;
            if (fabs(c3) <= 0.0001) {
                t3 = 3.402823466e+38f;
            } else {
                t3 = (k + pPos->v[2] * e3x - e3z * pPos->v[0]) / c3;
                if (t3 < 0.f) {
                    t3 = 3.402823466e+38f;
                }
            }
            if (t1 < t2 && t1 < t3 && t1 < best) {
                best = t1;
                ex = e1x;
                ez = e1z;
            } else if (t2 < t1 && t2 < t3 && t2 < best) {
                best = t2;
                ex = e2x;
                ez = e2z;
            } else if (t3 < best) {
                best = t3;
                ex = e3x;
                ez = e3z;
            } else {
                continue;
            }
            cache->field_0xa4.v[0] = ex;
            cache->field_0xa4.v[1] = 0.f;
            cache->field_0xa4.v[2] = -ez;
            if (best > 20.f) {
                continue;
            }
            *(float*)cache->field_0xbc = best * best;
            cache->field_0x30.v[0] = 0.f;
            cache->field_0x30.v[1] = 0.f;
            cache->field_0x30.v[2] = 0.f;
            cache->field_0x3c.v[0] = pPos->v[0] + nx * best;
            cache->field_0x3c.v[1] = pPos->v[1];
            cache->field_0x3c.v[2] = pPos->v[2] + nz * best;
            len = (br_scalar)sqrt(cache->field_0xa4.v[0] * cache->field_0xa4.v[0]
                    + cache->field_0xa4.v[1] * cache->field_0xa4.v[1]
                    + cache->field_0xa4.v[2] * cache->field_0xa4.v[2]);
            if (len > 2.3841858e-7f) {
                scale = (br_scalar)(1.0 / len);
                cache->field_0xa4.v[0] *= scale;
                cache->field_0xa4.v[1] *= scale;
                cache->field_0xa4.v[2] *= scale;
            } else {
                cache->field_0xa4.v[0] = 1.f;
                cache->field_0xa4.v[1] = 0.f;
                cache->field_0xa4.v[2] = 0.f;
            }
            cache->field_0x18 |= 0x8;
            continue;
        }

        /* Not a sliced face: the material decides whether the entry plane is flipped before the
           last two half-space tests (the face plane and its opposite). */
        denom = BrVector3Dot(&entry->plane, &dir);
        if (denom > 0.f) {
            if ((((tU8*)entry->material)[0x21] & 0x10) != 0) {
                entry->plane.v[0] = -entry->plane.v[0];
                entry->plane.v[1] = -entry->plane.v[1];
                entry->plane.v[2] = -entry->plane.v[2];
                entry->d = -entry->d;
            } else {
                if (PedProxSideTest(cell, idx0, &entry->normal1, &neg_normal, d1, 0x10, 0x1000)
                        || PedProxSideTest(cell, idx1, &entry->normal2, &neg_normal, d1, 0x10, 0x1000)
                        || PedProxSideTest(cell, idx2, &entry->normal3, &neg_normal, d1, 0x10, 0x1000)) {
                    cache->field_0x18 |= 0x1;
                }
                goto ped_prox_final;
            }
        }
        if (PedProxSideTest(cell, idx0, &entry->normal1, &face->normal, face->d, 0x1, 0x100)
                || PedProxSideTest(cell, idx1, &entry->normal2, &face->normal, face->d, 0x1, 0x100)
                || PedProxSideTest(cell, idx2, &entry->normal3, &face->normal, face->d, 0x1, 0x100)) {
            cache->field_0x18 |= 0x2;
        }
    ped_prox_final:
        if (cache->field_0x18 == 0) {
            continue;
        }
        *(float*)cache->field_0xbc = result * result;
        cache->field_0x30.v[0] = 0.f;
        cache->field_0x30.v[1] = 0.f;
        cache->field_0x30.v[2] = 0.f;
        cache->field_0x3c.v[0] = pPos->v[0] + result * dir.v[0];
        cache->field_0x3c.v[1] = pPos->v[1] + result * dir.v[1];
        cache->field_0x3c.v[2] = pPos->v[2] + result * dir.v[2];
        best = result;
        cache->field_0xa4 = entry->plane;
    }
    return best;
}

// FUNCTION: CARMA2_HW 0x004043a0
void C2_HOOK_FASTCALL DisposePedMoves(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_moves); i++) {
        if (gPed_moves[i] != NULL) {
            DisposeMove(gPed_moves[i]);
            gPed_moves[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00404700
void C2_HOOK_FASTCALL DisposeAllRemaps(void) {
    tPed_remap** remap;
    tPed_remap* the_remap;

    for (remap = gPed_remaps; (int)remap < (int)&gPed_remaps[10]; remap++) {
        the_remap = *remap;
        if (the_remap != NULL) {
            if (the_remap->bones != NULL) {
                BrMemFree(the_remap->bones);
            }
            BrMemFree(the_remap);
            *remap = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00406a00
void C2_HOOK_FASTCALL DisposePedForms(void) {
    int i;

    for (i = 0; i < 20; i++) {
        if (gPed_forms[i] != NULL) {
            DisposePedForm(gPed_forms[i]);
            gPed_forms[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00407900
void C2_HOOK_FASTCALL DisposeAllPedPersonalities(void) {
    tPed_personality** p;

    p = gPed_personalities;
    do {
        if (*p != NULL) {
            DisposePedPersonality(*p);
            *p = NULL;
        }
        p++;
    } while ((int)p < (int)&gPed_personalities[REC2_ASIZE(gPed_personalities)]);
}

// FUNCTION: CARMA2_HW 0x00402dd0
void C2_HOOK_FASTCALL FUN_00402dd0(void) {

    gPipe_play_ptr = gPipe_record_ptr;
}

// FUNCTION: CARMA2_HW 0x004093e0
int C2_HOOK_FASTCALL FUN_004093e0(tPed_character_instance* pCharacter) {

    if ((pCharacter->field_0x14 & 4) == 0) {
        return 0x1f;
    }
    BonerActiveHalted2(pCharacter, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0040c640
void C2_HOOK_FASTCALL PedNoOp(void) {
}

// FUNCTION: CARMA2_HW 0x0040b5a0
int C2_HOOK_FASTCALL GetCharacterBoneIndexByName(tPed_character_instance* pCharacter, const char* pBone_name) {
    int bone_index;
    int count_bones;

    bone_index = 0;
    count_bones = pCharacter->personality->form->count_bones;
    if (count_bones <= 0) {
        return -1;
    }
    for (; bone_index < count_bones; bone_index++) {
        if (strcmp(pBone_name, pCharacter->personality->form->bones[bone_index].name) == 0) {
            return bone_index;
        }
    }
    return -1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL SetPhysicsObjectOmegaRecursive(tPhysics_object* pObject, br_vector3* pOmega);

// FUNCTION: CARMA2_HW 0x0040b860
int C2_HOOK_FASTCALL MakeCharacterBonePhysicworthy(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pOmega) {
    extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
    tPed_form* form;
    tPed_form_boned_phys* boned_physicing;
    tPed_form_actor_set* actor_set;
    tPhysics_object* obj;
    tPhysics_object* child;
    br_actor* actor;
    br_vector3 omega;
    int i;

    if ((pCharacter->field_0xc & gPow2_array[pBone_index]) == 0) {
        if ((tS8)pCharacter->field_0x6 < 0) {
            form = pCharacter->personality->form;
            boned_physicing = form->boned_physicing;
            for (i = 0; i < form->max_boned_physicing_at_once; i++, boned_physicing++) {
                if (boned_physicing->field_0x0 == 0) {
                    break;
                }
            }
            if (i == form->max_boned_physicing_at_once) {
                goto done;
            }
            pCharacter->field_0x6 = (tU8)i;
            boned_physicing->field_0x0 = 1;
        }

        if (pOmega == NULL) {
            omega.v[0] = SRandomPosNeg(1.f);
            omega.v[1] = SRandomPosNeg(1.f);
            omega.v[2] = SRandomPosNeg(1.f);
        } else {
            omega = *pOmega;
        }

        form = pCharacter->personality->form;
        boned_physicing = &form->boned_physicing[(tS8)pCharacter->field_0x6];
        actor_set = &form->actor_sets[(tS8)pCharacter->field_0x4];
        actor = actor_set->actors[pBone_index];
        if (MakeCharacterPhysics(pCharacter, boned_physicing->collision_infos[pBone_index], actor, (tU8*)actor->model + 0x34, NULL, 3, 1, 1, 0, NULL, NULL) != 0) {
            obj = boned_physicing->collision_infos[pBone_index];
        } else {
            obj = NULL;
        }
        if (obj != NULL) {
            pCharacter->field_0xc |= gPow2_array[pBone_index];
            if (obj->parent != NULL) {
                obj->parent->child = NULL;
                obj->parent = NULL;
            }
            obj->child = NULL;
            obj->physics_joint1 = NULL;

            if (pCharacter->field_0x14 & 4) {
                if (pVelocity == NULL) {
                    pVelocity = &pCharacter->field_0xd8;
                }
                obj->disable_move_rotate = 0;
                if (pVelocity != NULL) {
                    obj->v.v[0] = pVelocity->v[0];
                    obj->v.v[1] = pVelocity->v[1];
                    obj->v.v[2] = pVelocity->v[2];
                    for (child = obj->child; child != NULL; child = child->next) {
                        SetObjectV(child, pVelocity);
                    }
                }
                if ((int)&omega != 0) {
                    obj->omega.v[0] = omega.v[0];
                    obj->omega.v[1] = omega.v[1];
                    obj->omega.v[2] = omega.v[2];
                    for (child = obj->child; child != NULL; child = child->next) {
                        SetPhysicsObjectOmegaRecursive(child, &omega);
                    }
                }
                PHILMakeObjectActive(obj, NULL, NULL, 0);
                obj->field_0xed = 1;
                PHILSetObjectProperty(obj, 0, 1.0);
                PHILSetObjectProperty(obj, 3, 0);
                PHILSetObjectProperty(obj, 4, 0);
                PHILSetObjectProperty(obj, 5, 0);
            }
            return 1;
        }
    }
done:
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040ba80
void C2_HOOK_FASTCALL SetPhysicsObjectOmegaRecursive(tPhysics_object* pObject, br_vector3* pOmega) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->omega.v[0] = pOmega->v[0];
    pObject->omega.v[1] = pOmega->v[1];
    pObject->omega.v[2] = pOmega->v[2];
    for (; child != NULL; child = child->next) {
        SetPhysicsObjectOmegaRecursive(child, pOmega);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int gMutant_tail_state;
extern tU32 gINT_00705b70;

// FUNCTION: CARMA2_HW 0x004cdeb0
void C2_HOOK_FASTCALL PedProcessContact(tPedestrian* pPed, tPhysics_object* pCollision_info) {
    tPedestrian* other_ped;
    tCar_spec* pCar;

    if (pCollision_info == NULL) {
        return;
    }
    if (pPed->field_0x0c == NULL) {
        return;
    }
    if (pCollision_info->owner != NULL
            && pCollision_info->flags_0x238 >= 2
            && pCollision_info->flags_0x238 <= 4
            && pPed->field_0x0c->field_0xc0 == NULL) {
        other_ped = ((tPed_character_instance*)pCollision_info->owner)->ped;
        if (other_ped != NULL && other_ped->field_0x0c != NULL) {
            pPed->field_0x0c->field_0xc0 = other_ped->field_0x0c->field_0xc0;
        }
    }
    if ((pCollision_info->owner == NULL || pCollision_info->flags_0x238 != 1)
            && (pCollision_info->flags_0x238 != 0x20
                || (gMutant_tail_state != 2 && GetTotalTime() - gINT_00705b70 >= 7500))
            && pCollision_info->flags_0x238 != 0x40
            && pCollision_info->flags_0x238 != 0x10) {
        return;
    }
    if (pCollision_info->flags_0x238 == 0x40) {
        pCar = ((tShit_mine*)pCollision_info->owner)->field_0x30;
    } else if (pCollision_info->flags_0x238 == 0x10) {
        pCar = (tCar_spec*)((tDrone_spec*)pCollision_info->owner)->field_0x5d4;
    } else {
        pCar = (tCar_spec*)pCollision_info->owner;
    }
    if ((pCar != NULL && pCar->driver > 5) || pCollision_info->parent != NULL) {
        pPed->field_0x0c->field_0xc0 = pCar;
        if (pCollision_info->owner != NULL && pCollision_info->flags_0x238 == 0x20) {
            ((tU8*)pPed->field_0x0c)[0x2e] |= 8;
        }
        if (pCollision_info->owner != NULL
                && pCollision_info->flags_0x238 == 0x20
                && gMutant_tail_state == 1) {
            ((tU8*)pPed->field_0x0c)[0x2e] |= 0x10;
        }
        if (pCollision_info->flags_0x238 == 0x40) {
            pPed->field_0x0c->field_0x2e = 0x40;
        }
        if (pCollision_info->flags_0x238 == 0x10) {
            pPed->field_0x0c->field_0x2e = 1;
            return;
        }
    } else if (pCar != NULL && pPed->field_0x0c->field_0xc0 == NULL) {
        pPed->field_0x0c->field_0xc0 = (tCar_spec*)pCar->field_0xc8;
        ((tU8*)pPed->field_0x0c)[0x2e] |= 1;
    }
}

// FUNCTION: CARMA2_HW 0x004d3600
void C2_HOOK_FASTCALL FUN_004d3600(void) {
}

// FUNCTION: CARMA2_HW 0x004d6c10
void C2_HOOK_FAKE_THISCALL FUN_004d6c10(tPed_character_instance* pCharacter, undefined4 pArg2, char* pX, char* pY, char* pZ) {
    char r;

    (void)pArg2;
    if (pCharacter->ped->hit_points <= 0 && pCharacter->ped->action == ePed_action_dead && gPeds_already_munged == 0) {
        r = IRandomPosNeg(30);
        *pX = *pX + r;
        r = IRandomPosNeg(30);
        *pY = *pY + r;
        r = IRandomPosNeg(30);
        *pZ += r;
    }
}

/* CheckForAvoidingAction: NO standalone symbol in the original. The whole body is inlined
   into MungePedestrians 0x4d3740 (0x4d474f-0x4d4b5a, guarded by `if (ped->field_0x0c->field_0x18)`).
   Evidence: its two thresholds 0x00694300 (gPed_min_dist_avoid_collisions_squared) and
   0x0069413c (gPed_reach_squared) are referenced at exactly one site in the binary, 0x4d479b /
   0x4d47b0, inside MungePedestrians. No `// FUNCTION:` annotation: there is no address to claim. */
void C2_HOOK_FASTCALL CheckForAvoidingAction(tPedestrian* pPed, tU32 pTime) {
    extern void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
    br_vector3* ped_pos;
    float dist_squared;

    ped_pos = GetPedPos(pPed);
    dist_squared = Vector3DistanceSquared(ped_pos, &pPed->field_0x0c->field_0x3c);
    if (dist_squared >= gPed_min_dist_avoid_collisions_squared) {
        BrVector3Set(&pPed->field_0x0c->field_0x0c, 0.f, 0.f, 0.f);
    } else {
        float act_dist;

        if (dist_squared > gPed_reach_squared) {
            act_dist = ((float)sqrt(dist_squared) - (float)sqrt(gPed_reach_squared)) * FRandomBetween(.7f, 1.f);
            act_dist = MIN(act_dist, .5f);
        } else {
            act_dist = .0f;
        }
        if (pPed->field_0x0c->field_0x18 == 1 && (gPeds_suicidal || gBlind_pedestrians || (act_dist == .0f && pPed->action == ePed_action_running && PercentageChance(100)))) {
            pPed->field_0x0c->field_0x18 = 0;
        } else if (!(gPeds_suicidal && (pPed->field_0x0c->field_0x18 & 0x4))) {
            float first_ped_prox;
            int v_non_zero;
            br_vector3 new_dir;
            br_vector3 new_0xa4;
            br_vector3 tv;

            if (pPed->field_0x0c->field_0x18 == 4) {
                if (pTime < pPed->field_0x0c->field_0x90) {
                    return;
                }
                pPed->field_0x0c->field_0x90 = pTime + IRandomBetween(200, 500);
            }
            v_non_zero = !Vector3IsZero(&pPed->field_0x0c->field_0x30);

            if (dist_squared == .0f) {
                BrVector3Copy(&new_dir, &pPed->field_0x0c->field_0xa4);
            } else {

                if ((float)fabs(BrVector3Dot(&pPed->field_0x0c->field_0xa4, &pPed->character->field_0xc0)) > .985f) {
                    BrVector3Set(&tv, 0.f, (float)(PercentageChance(50) ? -1 : 1), 0.f);
                } else {
                    DRVector3SafeCross(&tv, &pPed->field_0x0c->field_0xa4, &pPed->character->field_0xc0);
                }
                DRVector3SafeCross(&new_dir, &tv, &pPed->field_0x0c->field_0xa4);
                if (v_non_zero && BrVector3Dot(&pPed->field_0x0c->field_0x30, &new_dir) < 0.f) {
                    BrVector3Negate(&tv, &tv);
                    DRVector3SafeCross(&new_dir, &tv, &pPed->field_0x0c->field_0xa4);
                }
            }
            SetPedXZDirection(pPed, &new_dir, act_dist, pTime);
            BrVector3Copy(&new_0xa4, &pPed->field_0x0c->field_0xa4);
            first_ped_prox = RescanPedProximity(pPed, &new_dir);
            if (pPed->field_0x0c->field_0x18 != 0 && dist_squared != 0.f) {
                tPed_cache_006944c0 prev_field_0x0c;
                br_vector3 other_dir;

                BrVector3Negate(&tv, &tv);
                DRVector3SafeCross(&other_dir, &tv, &new_0xa4);
                SetPedXZDirection(pPed, &other_dir, act_dist, pTime);
                prev_field_0x0c = *pPed->field_0x0c;
                if (RescanPedProximity(pPed, &other_dir) <= first_ped_prox) {
                    *pPed->field_0x0c = prev_field_0x0c;
                    SetPedXZDirection(pPed, &new_dir, act_dist, pTime);
                    BrVector3Set(&pPed->field_0x0c->field_0x0c, 0.f, 0.f, 0.f);
                }
            }
        }
    }
}
/* SetRandomOmega: NO standalone symbol in the original either. Its only call site
   (MungePedestrians 0x4d3740, low-gravity branch) has the three SRandomPosNeg(1.0f) calls
   inlined at 0x4d4c69/0x4d4c76/0x4d4c83, storing to omega (+0x74/+0x78/+0x7c), followed by
   the length/scale code (2.0f constant at 0x0058aaa4). There is no standalone copy anywhere
   in the binary (all 0x513650 call sites checked). No `// FUNCTION:` annotation. */
void C2_HOOK_FASTCALL SetRandomOmega(tPhysics_object* pObject, float pMax) {
    extern br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN);

    BrVector3Set(&pObject->omega, SRandomPosNeg(1.f), SRandomPosNeg(1.f), SRandomPosNeg(1.f));
    BrVector3Scale(&pObject->omega, &pObject->omega, pMax / BrVector3Length(&pObject->omega));
}

extern int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2);
extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
// PARTIAL 42.01%: MungeCharacterAnimation 0x409ca0 - regalloc (original keeps 0 in ebx, pTime in ebp), frame 0x24 vs 0x28, tail x87 order
// FUNCTION: CARMA2_HW 0x00409ca0
void C2_HOOK_FASTCALL MungeCharacterAnimationBody(tPed_character_instance* pCharacter, tU32 pTime, undefined4 pArg3) {
    tPed_move* model;
    br_matrix34* work;
    float kx;
    float ky;
    float kz;
    double t;
    tS32 loop_count = 0;
    int i;

    if (pCharacter->field_0x7 < 0) {
        return;
    }
    if ((pCharacter->field_0x14 & 0x4) && (pCharacter->field_0x14 & 0x2) && pCharacter->field_0xe8 == NULL) {
        return;
    }
    if (pCharacter->field_0xbc != NULL) {
        if (MorphCharacterBonePositions(pCharacter, pTime - pCharacter->field_0x24)) {
            pCharacter->field_0x24 = pTime;
            return;
        }
        pCharacter->field_0x24 = pTime - (tS16)pCharacter->field_0x1e;
    }
    model = pCharacter->personality->form->moves[pCharacter->field_0x7].move;
    if (pCharacter->field_0x4 < 0) {
        work = &pCharacter->field_0x2c;
    } else if (pCharacter->field_0xe8 != NULL) {
        work = pCharacter->field_0xe8;
    } else {
        work = &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
    }
    kx = work->m[3][0];
    ky = work->m[3][1];
    kz = work->m[3][2];
    if ((tS16)pCharacter->field_0x1e != 0) {
        if (pCharacter->field_0x24 == 0) {
            SetCharacterBonePositions(pCharacter, 0, 0);
        } else {
            float rate;

            rate = (float)(tS32)(pTime - pCharacter->field_0x24) / (tS32)(tS16)pCharacter->field_0x1e;
            if (rate > 10.0f) {
                rate = 10.0f;
            } else if (rate < -10.0f) {
                rate = -10.0f;
            }
            t = rate + (double)*(float*)&pCharacter->field_0x18 + (tS32)(tS16)pCharacter->field_0x1c;
            *(float*)&pCharacter->field_0x18 = (float)(t - floor(t));
            loop_count = (tS32)(floor(t) - (double)(tS32)(tS16)pCharacter->field_0x1c);
            if (loop_count > 0) {
                for (i = 0; i < loop_count; i++) {
                    pCharacter->field_0x1c++;
                    if ((tS16)pCharacter->field_0x1c >= model->count_frames) {
                        if (gAction_replay_mode == 0) {
                            pCharacter->field_0x1c = (undefined2)(model->count_frames - 1);
                            if (pCharacter->field_0x9 & 0x2) {
                                pCharacter->field_0x9 &= (tU8)~0x2;
                                gPed_vtable->move_completed((undefined4*)pCharacter);
                            }
                            if (pCharacter->field_0x8 >= 0) {
                                SetCharacterMove(pCharacter, pCharacter->field_0x8, (float)(tS32)(tS16)pCharacter->field_0x20, 0, (pCharacter->field_0x9 & 0x4) >> 2, pTime);
                                SetCharacterBonePositions(pCharacter, 0, 0);
                                break;
                            }
                            pCharacter->field_0x1c = 0;
                        } else if (model->move_flags & 0x1) {
                            pCharacter->field_0x1c--;
                            i = loop_count - 1;
                        } else {
                            pCharacter->field_0x1c = 0;
                        }
                    }
                    SetCharacterBonePositions(pCharacter, i != loop_count - 1 ? 2 : 0, 1);
                }
            }
            if (loop_count == 0) {
                if ((pCharacter->field_0x14 & 0x4) || gAction_replay_mode != 0) {
                    SetCharacterBonePositions(pCharacter, 0, 0);
                }
            }
            if (loop_count < 0) {
                for (i = loop_count; i < 0; i++) {
                    SetCharacterBonePositions(pCharacter, i < -1 ? 2 : 0, 1);
                    pCharacter->field_0x1c--;
                    if ((tS16)pCharacter->field_0x1c < 0) {
                        if (model->move_flags & 0x1) {
                            pCharacter->field_0x1c++;
                            SetCharacterBonePositions(pCharacter, 3, 1);
                            i = 0;
                        } else {
                            pCharacter->field_0x1c = (undefined2)(model->count_frames - 1);
                        }
                    }
                }
            }
        }
    }
    pCharacter->field_0x24 = pTime;
    if (loop_count != 0) {
        float dx;
        float dy;
        float dz;
        double x;

        dx = work->m[3][0] - kx;
        dy = work->m[3][1] - ky;
        dz = work->m[3][2] - kz;
        pCharacter->field_0xd8.v[0] = dx;
        pCharacter->field_0xd8.v[1] = dy;
        pCharacter->field_0xd8.v[2] = dz;
        x = (double)(tS32)loop_count * 0.001 * (tS16)pCharacter->field_0x1e;
        pCharacter->field_0xd8.v[0] = (float)(pCharacter->field_0xd8.v[0] / x);
        pCharacter->field_0xd8.v[1] = (float)(pCharacter->field_0xd8.v[1] / x);
        pCharacter->field_0xd8.v[2] = (float)(pCharacter->field_0xd8.v[2] / x);
    } else if ((tS16)pCharacter->field_0x1e == 0) {
        pCharacter->field_0xd8.v[0] = 0.0f;
        pCharacter->field_0xd8.v[1] = 0.0f;
        pCharacter->field_0xd8.v[2] = 0.0f;
    }
}

extern void C2_HOOK_FASTCALL ApplyNonCarTumble(tPhysics_object* pObject, br_vector3* pV, br_scalar pTumbleFactor, br_scalar pTumbleThreshold);

// FUNCTION: CARMA2_HW 0x004d2930
void C2_HOOK_FASTCALL CBMovedByPhysics(tPed_character_instance* pCharacter, tPhysics_object* pPhysics, undefined4 pArg3) {
    tPedestrian* ped;
    br_scalar speed;
    int move_speed;
    int move_id;

    ped = pCharacter->ped;
    ped->pos.v[0] = pPhysics->actor->t.t.mat.m[3][0];
    ped->pos.v[1] = pPhysics->actor->t.t.mat.m[3][1];
    ped->pos.v[2] = pPhysics->actor->t.t.mat.m[3][2];
    pCharacter->field_0xd8.v[0] = pPhysics->v.v[0];
    pCharacter->field_0xd8.v[1] = pPhysics->v.v[1];
    pCharacter->field_0xd8.v[2] = pPhysics->v.v[2];
    speed = BrVector3Length(&pPhysics->v);
    ApplyNonCarTumble(pPhysics, &pPhysics->v, 1.39f, 0.1f);

    move_id = pCharacter->personality->form->moves[pCharacter->field_0x7].id;
    if (move_id == 0x5d || move_id == 0x5e || move_id == 0x61) {
        if (speed != 0.0f) {
            move_speed = (int)(FRandomBetween(0.6f, 1.4f) * (25.0 / speed));
            if (move_speed < 20) {
                move_speed = 20;
            } else if (move_speed > 150) {
                move_speed = 150;
            }
            if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL) {
                if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.0f) {
                    move_speed = (int)(move_speed * 1.5);
                }
            }
        } else {
            move_speed = 0;
        }
        SetCharacterMoveAR(pCharacter, -1, (float)move_speed, 0, 0, GetTotalTime());
    }

    if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL) {
        if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.0f) {
            if (ped->hit_points > 0) {
                if (BrVector3Length(&pPhysics->omega) < 1.0) {
                    pPhysics->omega.v[0] += SRandomPosNeg(5.f);
                    pPhysics->omega.v[1] += SRandomPosNeg(5.f);
                    pPhysics->omega.v[2] += SRandomPosNeg(5.f);
                }
                if (pPhysics->water_depth_factor <= 1.0 && pPhysics->v.v[1] > -0.02) {
                    if (pPhysics->water_depth_factor < 1.0) {
                        pPhysics->v.v[1] += FRandomBetween(0.f, -0.3f);
                    }
                }
            }
        }
    }

    if (!gAction_replay_mode) {
        PipeSinglePedPhysics((((tS16)ped->field_0x06) << 16) | (ped - gPedestrian_array), GetCharacterMatrixPtr(pCharacter));
    }
}

// GLOBAL: CARMA2_HW 0x0065e4f4
const br_vector3 gPed_bone_look_vecs[9] = {
    { {  0.f,  1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  1.f,  0.f,  0.f } },
    { {  1.f,  0.f,  0.f } },
    { { -1.f,  0.f,  0.f } },
    { { -1.f,  0.f,  0.f } },
};

// GLOBAL: CARMA2_HW 0x006a0428
tU32 gLast_bleed_giblet_time;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
void C2_HOOK_FASTCALL MakeEmBleed(tPedestrian* pPed, tU32 pTime) {
    /* MakeEmBleed, inlined in the original */
    {
        extern tU32 gLast_bleed_giblet_time;
        extern void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pTransform, br_vector3* pPosition, br_vector3* pDirection, br_vector3* pRandom_direction);
        extern const br_vector3 gPed_bone_look_vecs[9];

        if (!(MUNGE_PED_COLLISION_OBJECT(pPed)->last_special_volume != NULL && MUNGE_PED_COLLISION_OBJECT(pPed)->last_special_volume->gravity_multiplier < 1.f)
                && gGoreLevel > 0) {
            if (gMutant_speed != 0.f && pPed->hit_points <= 0 && pPed->action == ePed_action_dead && pTime - gLast_bleed_giblet_time > 300 && PercentageChance(40)) {
                gLast_bleed_giblet_time = pTime;
                DoGiblets(pPed, MUNGE_PED_COLLISION_OBJECT(pPed), NULL, .06f, &pPed->pos, (int)gGiblet_scrape_start);
            }
            if (pPed->character->field_0xc != 0 && pPed->field_0x0c != NULL && pPed->field_0x0c->field_0x84 < pTime - pPed->field_0x0c->field_0x88) {
                int bleed_i;

                if (pPed->hit_points <= 0) {
                    pPed->field_0x0c->field_0x84 = (tU32)((float)pPed->field_0x0c->field_0x84 * 1.2f);
                    if (pPed->field_0x0c->field_0x84 > 1000) {
                        pPed->field_0x0c->field_0x84 = 0;
                    }
                }
                pPed->field_0x0c->field_0x88 = pTime;
                for (bleed_i = 1; bleed_i < pPed->character->personality->form->count_bones; bleed_i++) {
                    if ((pPed->character->field_0xc & gPow2_array[bleed_i]) && !(pPed->character->field_0x10 & gPow2_array[bleed_i])) {
                        tPhysics_object* object;
                        br_vector3* bone_v;
                        int other_bone_index;

                        object = pPed->character->personality->form->boned_physicing[pPed->character->field_0x6].collision_infos[bleed_i];
                        if (pPed->field_0x0c->field_0x23[bleed_i] < gPed_spurty_lumps_count && BrVector3LengthSquared(&object->v) == 0.f) {
                            pPed->field_0x0c->field_0x23[bleed_i] = 0;
                        } else {
                            br_matrix34* bone_matrix;
                            br_vector3 bone_look;

                            if (pPed->field_0x0c->field_0x23[bleed_i] != 0) {
                                pPed->field_0x0c->field_0x23[bleed_i] -= gPed_spurty_lumps_count;
                            }
                            bone_matrix = GetBoneMatrixPtr(pPed->character, bleed_i);
                            BrVector3Copy(&bone_look, (br_vector3*)bone_matrix->m[0]);
                            DoSpurt(bone_matrix, &pPed->character->personality->bones[bleed_i].field_0x20, &bone_look, &object->v);
                        }
                        other_bone_index = pPed->character->personality->form->bones[bleed_i].indices[0];
                        if (pPed->character->field_0xc & gPow2_array[other_bone_index]) {
                            bone_v = &pPed->character->personality->form->boned_physicing[pPed->character->field_0x6].collision_infos[other_bone_index]->v;
                            if (pPed->field_0x0c->field_0x23[bleed_i] == 0) {
                                continue;
                            }
                        } else {
                            bone_v = &pPed->character->field_0xd8;
                        }
                        if (BrVector3LengthSquared(bone_v) != 0.f || pPed->field_0x0c->field_0x84 != 0) {
                            br_matrix34* other_bone_matrix;
                            br_vector3 other_bone_look;

                            other_bone_matrix = GetBoneMatrixPtr(pPed->character, other_bone_index);
                            if (other_bone_index != 0) {
                                BrVector3Copy(&other_bone_look, (br_vector3*)other_bone_matrix->m[0]);
                            } else {
                                BrMatrix34ApplyV(&other_bone_look, &gPed_bone_look_vecs[bleed_i - 1], other_bone_matrix);
                            }
                            DoSpurt(other_bone_matrix, &pPed->character->personality->bones[bleed_i].field_0x2c, &other_bone_look, bone_v);
                        }
                    }
                }
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
