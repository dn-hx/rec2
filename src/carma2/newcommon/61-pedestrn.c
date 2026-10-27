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

// GLOBAL: CARMA2_HW 0x006a0414
const char* gPedsFolder;

// GLOBAL: CARMA2_HW 0x0065d7d4
int gGoreLevel = 1;

/* explicit order keeps gPed_cache_sizes_1 directly in front of gAnimalsOn (loop-end compare in InitPolyPedSystem) */
// GLOBAL: CARMA2_HW 0x0065d778
tPedForms_vtable gPed_forms_vtable = { 0 };

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
// GLOBAL: CARMA2_HW 0x0069bc0c
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
br_material* gPedestrian_character_cloned_materials[50];

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
extern void C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, int pArg4);
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

/* based on carpocalypse2 pedestrn.c:7366 (GPL-3.0); the helpers below are inlined into InitPolyPedSystem in the original */
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

// GLOBAL: CARMA2_HW 0x00694230
int gPed_animal_count;

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

// GLOBAL: CARMA2_HW 0x0069fd88
br_material* gBurning_ped_materials[7];

// UNCERTAIN: unreferenced 20-byte gap, loop-end compare in InitNapalmNolts
// GLOBAL: CARMA2_HW 0x0069fda4
int gUNK_0069fda4[5];

// based on carpocalypse2 pedestrn.c:1064
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

// FUNCTION: CARMA2_HW 0x004cbc70
void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pIndex, tPed_character_instance* pCharacter) {

    if (pActor->parent == NULL) {
        if (pIndex == 0 || (pCharacter->field_0xc & gPow2_array[pIndex]) != 0) {
            BrActorAdd(gNon_track_actor, pActor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004cbca0
void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor) {

    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x004cbcc0
int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor) {
    if (pActor->parent != NULL) {
        return 1;
    } else {
        return 0;
    }
}

// SetPedXZDirection

// StopPedNoise

// MakePedNoise

extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);
extern void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, int pArg2, int pOriginal_move_id, int pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13);

// SetPedMove: based on carpocalypse2 src/carma2/common/pedestrn.c:1625 (GPL-3.0)
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

// DoBloodSpurt

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

// STUB: CARMA2_HW 0x004ce050
void C2_HOOK_FASTCALL DoPedAnnihilator(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pArg4, tPedestrian_distances* pDistances) {
    NOT_IMPLEMENTED();
}

// RepulseIt

// DoPedestrianRepulsifictor

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

// CBMovedByPhysics

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
// MungePedestrians

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
// PedPreCollisionStuff

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

// AddChangedPoint

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

// FUNCTION: CARMA2_HW 0x00404290
void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ) {
    int i;
    br_vertex* v;

    REC2_THISCALL_UNUSED(pArg2);

    v = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++, v++) {
        v->p.v[0] *= pX;
        v->p.v[1] *= pY;
        v->p.v[2] *= pZ;
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}
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
// DisposeMove

// DisposeAllLoadedMoves

// ReadRemap

// DisposeRemap

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

// FindOrOpenForm

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

// FindOrOpenPersonality

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00408200
int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// CharacterNoLongerRenderable

// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00408c30
int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00409030
int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance *pCharacter, int pArg2, undefined4 pArg3) {

    return MakeCharacterCollideworthy2(pCharacter, pArg2, pArg3, -1);
}

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
// CharacterNoLongerCollideworthy

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

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// MakeCharacterPhysicworthy

// RecalculateOrientationOfRoot

// BonerActiveHalted2

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

// StopCharacterMorphing

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

// SetCharacterMove

// DRMatrix34RotateCos

// DropPointOntoPlane

// SetCharacterDirection

// SetCharacterPosition

// DisposeClonedMaterials

// DisposeAllBonerData

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0040b420
void C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, int pArg4) {
    NOT_IMPLEMENTED();
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
// BonerPassiveCollision

// BonerActiveHalted

// BonerPedMovedByPhysics

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
// STUB: CARMA2_HW 0x0040b2e0
void C2_HOOK_FASTCALL DisposeCharacterInstances(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);
extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);

// based on carpocalypse2 src/carma2/common/pedestrn.c:1480 (GPL-3.0), reworked after the disassembly
// FUNCTION: CARMA2_HW 0x0040a050
int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6) {
    tPed_personality* personality;
    tPed_form* form;
    tPed_move* original_ped_move;
    tPed_move* ped_move;
    tPed_move_frame* ped_frame;
    br_matrix34* character_matrix;
    br_matrix34 inverse;
    br_euler e;
    int move_id;
    int move_flag;
    int original_field_0x7;
    int i;
    int speed;
    short original_speed;
    float sum;
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
        for (i = pCharacter->field_0x1c; i > 0; i--) {
            sx += original_ped_move->frames[i].mat.m[3][0];
            sy += original_ped_move->frames[i].mat.m[3][1];
            sz += original_ped_move->frames[i].mat.m[3][2];
        }
    }
    if (original_ped_move != NULL) {
        ped_frame = &original_ped_move->frames[pCharacter->field_0x1c];
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
            if (*(float*)&gPed_morph_slots[i].field_0x0 == 0.f) {
                pCharacter->field_0xbc = (tPed_morph*)((char*)&gPed_morph_slots[i] - 4);
                break;
            }
        }
    }
    if (pCharacter->field_0xbc == NULL) {
        pArg4 = 0;
    }
    if (pArg4 != 0) {
        pCharacter->field_0xbc->field_0x20 = original_ped_move->field_0x40;
        if (original_speed != 0 && original_ped_move->field_0x2a == 0) {
            BrVector3InvScale(&pCharacter->field_0xbc->field_0x8, &pCharacter->field_0xbc->field_0x20, (float)(original_ped_move->count_frames * original_speed));
        } else {
            BrVector3Set(&pCharacter->field_0xbc->field_0x8, 0.f, 0.f, 0.f);
        }
        if ((short)speed != 0 && ped_move->field_0x2a == 0) {
            BrVector3InvScale(&pCharacter->field_0xbc->field_0x14, &ped_move->field_0x34, (float)(ped_move->count_frames * (short)speed));
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
        sum = 0.f;
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
        p[0] = p[0];
        p[1] = FindYVerticallyBelow2((br_vector3*)p) + pCharacter->personality->moves[pCharacter->field_0x7].grounding_offset;
        p[2] = p[2];
    }
    SetCharacterBonePositions(pCharacter, 0, 0);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004097b0
int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00407e70
void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00407b30
void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3) {
    int i = 0;
    tPed_form_actor_set* actor_set = NULL;
    tPed_move_frame* ped_move_frame;
    tPed_form* form;
    tPed_personality *personality;

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
    }
    ped_move_frame = &form->moves[pPed->field_0x7].move->frames[(tS16)pPed->field_0x1c];
    for (; i < form->count_bones; i++) {
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
                    mat = &form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
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

// STUB: CARMA2_HW 0x004c8b30
void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, int pArg2, int pOriginal_move_id, int pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
