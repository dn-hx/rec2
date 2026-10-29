#include "27-powerup.h"

#include "16-graphics1.h"
#include "32-spark.h"
#include "49-physics.h"
#include "61-pedestrn.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"
#include "41-utility.h"
#include "25-grafdata.h"
#include "19-font.h"
#include "06-apo.h"
#include "23-piping.h"
#include "51-smash.h"
#include "53-controls.h"
#include "18-graphics2.h"
#include "00-car.h"
#include "49-physics.h"
#include "70-packfile.h"
#include "28-world3.h"
#include "42-input.h"
#include "08-loading1.h"
#include "63-loading3.h"
#include "platform.h"

// GLOBAL: CARMA2_HW 0x006a0ad4
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x007059e0
tHeadup_icon gPickedup_powerups[20];

// GLOBAL: CARMA2_HW 0x00763480
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007447d0
int gBlind_pedestrians;

// GLOBAL: CARMA2_HW 0x007447dc
float gPed_gravity_multiplier;

// GLOBAL: CARMA2_HW 0x007447e4
int gPed_dismemberfest;

// GLOBAL: CARMA2_HW 0x007447f8
int gPanicking_peds;

// GLOBAL: CARMA2_HW 0x00705b78
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x00705b74
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x006a0acc
tPhysics_joint* gMutant_tail_first_joint;

// GLOBAL: CARMA2_HW 0x00705540
int gMutant_tail_state;

// GLOBAL: CARMA2_HW 0x0065ebbc
float gMass_mine = 1.0f;

// GLOBAL: CARMA2_HW 0x006a0454
int gCount_cloaked_cars;

/* explicit initializers keep gRespawn_powerups and gMutant_tail_first_collision_info adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x006a0458
tRespawn_powerup gRespawn_powerups[100] = { 0 };

// GLOBAL: CARMA2_HW 0x006a0908
tPhysics_object* gMutant_tail_first_collision_info = NULL;

// GLOBAL: CARMA2_HW 0x006a0a50
tU8* gRace_powerup_respawn_bools;

// GLOBAL: CARMA2_HW 0x006a0910
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0a54
tPowerup* gPowerup_array;

// GLOBAL: CARMA2_HW 0x006a0940
int gInventory_cycling;

// GLOBAL: CARMA2_HW 0x006a0954
tU32 gInventory_timeout;

// GLOBAL: CARMA2_HW 0x006a0a70
int gInventory[20];

// GLOBAL: CARMA2_HW 0x006a0a58
int gInventory_selected;

// GLOBAL: CARMA2_HW 0x006a0a6c
int gINT_006a0a6c;

// GLOBAL: CARMA2_HW 0x006a0a5c
int gINT_006a0a5c;

// GLOBAL: CARMA2_HW 0x006a0ae0
br_model* gModel_powerup_armor;

// GLOBAL: CARMA2_HW 0x006a0ae4
br_model* gModel_powerup_power;

// GLOBAL: CARMA2_HW 0x006a0ae8
br_model* gModel_powerup_offence;

// GLOBAL: CARMA2_HW 0x006a0ac0
br_pixelmap* gFizzle_in[3];

// GLOBAL: CARMA2_HW 0x006a0950
int gFizzle_height;

// GLOBAL: CARMA2_HW 0x006a0ad0
int gNumber_of_powerups;

// GLOBAL: CARMA2_HW 0x006a0948
int gNumber_of_powerup_respawn_specs;

// GLOBAL: CARMA2_HW 0x006a0a64
tPowerup_respawn_spec* gPowerup_respawn_specs;

// GLOBAL: CARMA2_HW 0x006a0a60
int gNet_powerup_time_between_goodies_min;

// GLOBAL: CARMA2_HW 0x006a090c
int gNet_powerup_time_between_goodies_max;

// GLOBAL: CARMA2_HW 0x006a0450
int gNet_count_powerup_goodies;

// GLOBAL: CARMA2_HW 0x006a094c
int* gNet_powerup_goodies;

// GLOBAL: CARMA2_HW 0x007447b8
int gDrunk_pedestrians;

// GLOBAL: CARMA2_HW 0x0079ec50
int gInstant_handbrake;

// GLOBAL: CARMA2_HW 0x007447a4
float gMutant_speed;

// GLOBAL: CARMA2_HW 0x006a0960
tRepulse_link gRepulse_links[6];

// GLOBAL: CARMA2_HW 0x0058ac50
const float gBounce_rate_const = 1000.0f;

// GLOBAL: CARMA2_HW 0x00705be0
int gCredit_multiplier;

// GLOBAL: CARMA2_HW 0x006a0ad8
void* gDrug_trip_data;

// GLOBAL: CARMA2_HW 0x006a0adc
int gCollision_info_uid_counter;

/* explicit initializers keep gShit_mines and the following block adjacent (loop-end compares) */
// GLOBAL: CARMA2_HW 0x00705560
tShit_mine gShit_mines[20] = { 0 };

// GLOBAL: CARMA2_HW 0x007059c0
int gCount_mutant_tail_parts = 0;

/* UNCERTAIN: unreferenced dwords after gCount_mutant_tail_parts (only seen as gShit_mines loop ends); scalars, an initialized array would be 8-aligned */
// GLOBAL: CARMA2_HW 0x007059c4
int gUNK_007059c4 = 0;

// GLOBAL: CARMA2_HW 0x007059c8
int gUNK_007059c8 = 0;

// GetNextGoodyTime

// DoPowerupTextHeadup

// QueueIconGoing

// QueueIconComing

// LosePowerupX

// LosePowerup

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups

// ExcludedPowerup

// GotPowerupX

// GotPowerupXX

// FUNCTION: CARMA2_HW 0x004d8d30
void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex) {

    GotPowerupEarwig(pCar, pIndex, 1, 1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004d8d40
void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// AutoGoody

// GotPowerup

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004dc170
int C2_HOOK_FASTCALL GotCredits(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc240
int C2_HOOK_FASTCALL GainAPO(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc470
int C2_HOOK_FASTCALL GotTimeOrPower(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc4d0
int C2_HOOK_FASTCALL GainAPOPotential(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc750
int C2_HOOK_FASTCALL SetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc7a0
int C2_HOOK_FASTCALL SetPedHeadSize(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc860
void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc990
int C2_HOOK_FASTCALL DoInstantRepair(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dc9d0
void C2_HOOK_FASTCALL ResetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dca00
void C2_HOOK_FASTCALL ResetPedHeadSize(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dca30
int C2_HOOK_FASTCALL SetEngineFactor(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dcae0
int C2_HOOK_FASTCALL TrashBodywork(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dcc00
void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* pPowerup, tU32 pTime) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dcca0
int C2_HOOK_FASTCALL SetOpponentsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dce80
int C2_HOOK_FASTCALL SetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dd700
int C2_HOOK_FASTCALL ShitMine(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de040
int C2_HOOK_FASTCALL PissOutOil(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de0c0
int C2_HOOK_FASTCALL KangerooJump(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de140
int C2_HOOK_FASTCALL AnnihilatePeds(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de190
int C2_HOOK_FASTCALL NapalmPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de1e0
int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de540
int C2_HOOK_FASTCALL RepulsePedestrians(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de5d0
void C2_HOOK_FASTCALL ResetEngineFactor(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de610
void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de6b0
void C2_HOOK_FASTCALL ResetOpponentsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de760
void C2_HOOK_FASTCALL ResetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de7a0
int C2_HOOK_FASTCALL PickAtRandom(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de930
int C2_HOOK_FASTCALL SetGhostPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de9a0
void C2_HOOK_FASTCALL ResetGhostPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004de9b0
void C2_HOOK_FASTCALL DoBouncey(tPowerup* pPowerup, tU32 pTime) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dea40
int C2_HOOK_FASTCALL HitMine(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004debb0
int C2_HOOK_FASTCALL SetMassMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004df220
int C2_HOOK_FASTCALL SetPissed(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004df3b0
void C2_HOOK_FASTCALL PissedEffect(tPowerup* pPowerup, tU32 pTime) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dff30
int C2_HOOK_FASTCALL PowerupCancel(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e02b0
int C2_HOOK_FASTCALL GrowATail(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e04c0
int C2_HOOK_FASTCALL CutOffTail(tPowerup* pPowerup, tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL TakeDrugs(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetBouncey(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL GotVouchers(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL PedValium(tPowerup* pPowerup, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPinball(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetPissed(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL MungeVouchers(tPowerup* pPowerup, tU32 pTime);

// GLOBAL: CARMA2_HW 0x0065e910
tGot_proc* gGot_procs[56] = {
    GotCredits,
    SetPedSpeed,
    SetPedSize,
    SetPedExplode,
    PickAtRandom,
    SetInvulnerability,
    SetFreeRepairs,
    DoInstantRepair,
    FreezeTimer,
    SetEngineFactor,
    SetUnderwater,
    GotTimeOrPower,
    TrashBodywork,
    TakeDrugs,
    SetOpponentsSpeed,
    SetCopsSpeed,
    SetGravity,
    SetPinball,
    SetWallclimb,
    SetBouncey,
    SetSuspension,
    SetTyreGrip,
    SetDamageMultiplier,
    SetImmortalPeds,
    SetStupidPedestrians,
    SetSuicidalPedestrians,
    GotVouchers,
    SetMassMultiplier,
    SetInstantHandbrake,
    ShowPedestrians,
    HitMine,
    SetElectroBastard,
    SetPedHeadSize,
    SetMutantCorpses,
    ShitMine,
    PissOutOil,
    KangerooJump,
    AnnihilatePeds,
    RepulseOpponents,
    SetPedBrittle,
    SetGhostPeds,
    SetDancingPeds,
    SetPanickingPeds,
    SetLowGravityPeds,
    SetPissedPeds,
    RepulsePedestrians,
    GainAPO,
    GainAPOPotential,
    SetPissed,
    NapalmPeds,
    PedValium,
    PowerupCancel,
    GrowATail,
    CutOffTail,
    TurnOnCloaking,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065e9f0
tLose_proc* gLose_procs[56] = {
    NULL,
    ResetPedSpeed,
    ResetPedSize,
    ResetPedExplode,
    NULL,
    ResetInvulnerability,
    ResetFreeRepairs,
    NULL,
    UnfreezeTimer,
    ResetEngineFactor,
    ResetUnderwater,
    NULL,
    NULL,
    PukeDrugsBackUp,
    ResetOpponentsSpeed,
    ResetCopsSpeed,
    ResetGravity,
    ResetPinball,
    ResetWallclimb,
    ResetBouncey,
    ResetSuspension,
    ResetTyreGrip,
    ResetDamageMultiplier,
    ResetImmortalPeds,
    ResetStupidPedestrians,
    ResetSuicidalPeds,
    NULL,
    ResetMassMultiplier,
    ResetInstantHandbrake,
    HidePedestrians,
    NULL,
    ResetElectroBastard,
    ResetPedHeadSize,
    ResetMutantCorpses,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ResetPedBrittle,
    ResetGhostPeds,
    ResetDancingPeds,
    ResetPanickingPeds,
    ResetLowGravityPeds,
    ResetPissedPeds,
    NULL,
    NULL,
    NULL,
    ResetPissed,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TurnOffCloaking,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065ead0
tPeriodic_proc* gPeriodic_procs[56] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TheEffectsOfDrugs,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    DoBouncey,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MungeVouchers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PissedEffect,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065ebb0
const char* gFizzle_names[3] = {
    "CIRCLES",
    "SQUARES",
    "DIAMONDS",
};

static br_pixelmap* DRMapFindMeAPixie(const char* name) {
    char s[256];
    br_pixelmap* map;

    map = BrMapFind(name);
    if (map != NULL) {
        return map;
    }
    strcpy(s, name);
    strcat(s, ".PIX");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    strcpy(s, name);
    strcat(s, ".pix");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    return NULL;
}

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int, ...);

/* based on carpocalypse2 powerups.c:614 */
// FUNCTION: CARMA2_HW 0x004d96c0
void C2_HOOK_FASTCALL LoadPowerups(void) {
    FILE* f;
    tPath_name the_path;
    tTWTVFS twtvfs;
    int i, j, k;
    int first;
    int count_index_groups;
    int index_range;
    char s[256];
    tPowerup* the_powerup;
    char* pp;

    C2_HOOK_BUG_ON(sizeof(tPowerup) != 172);

    if (gPowerup_txt_path == NULL) {
        gPowerup_txt_path = "POWERUP.TXT";
    }
    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "POWERUPS");
    twtvfs = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ClosePackFileAndSetTiffLoading(twtvfs);

    for (i = 0; i < (int)REC2_ASIZE(gFizzle_in); i++) {
        gFizzle_in[i] = DRMapFindMeAPixie(gFizzle_names[i]);
    }
    gFizzle_height = gFizzle_in[0]->height / 4;

    PathCat(the_path, gApplication_path, gPowerup_txt_path);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }
    /* Number of powerups */
    gNumber_of_powerups = GetAnInt(f);
    gPowerup_array = BrMemAllocate(sizeof(tPowerup) * gNumber_of_powerups, kMem_powerup_array);
    pp = (char*)gPowerup_array + 0x14;
    for (i = 0; i < gNumber_of_powerups; i++, pp += sizeof(tPowerup)) {
        the_powerup = (tPowerup*)(pp - 0x14);

        GetALineAndDontArgue(f, the_powerup->message);
        if (strcmp(the_powerup->message, "dummy") == 0) {
            the_powerup->type = ePowerup_dummy;
        } else {
            if (strcmp(the_powerup->message, "n/a") == 0) {
                the_powerup->message[0] = '\0';
            }
            GetAString(f, s);
            the_powerup->icon = DRMapFindMeAPixie(s);
            if (the_powerup->icon != NULL) {
                BrMapAdd(the_powerup->icon);
                BRPM_convert(the_powerup->icon, gBack_screen->type);
            }
            if (the_powerup->icon != NULL) {
                the_powerup->icon_actor = CreateBillBoard(the_powerup->icon);
                the_powerup->icon_actor->material->extra_prim = the_powerup->material_prims;
            }

            /* Fizzle type */
            the_powerup->fizzle_type = GetAnInt(f);
            /* Time limit (-1 instantaneous, 0 whole race, x seconds) */
            j = 1000 * GetAnInt(f);
            the_powerup->duration = j;
            if (j < 0) {
                the_powerup->type = ePowerup_instantaneous;
            } else if (j == 0) {
                the_powerup->type = ePowerup_whole_race;
            } else {
                the_powerup->type = ePowerup_timed;
            }
            /* Initial value for keyboard operable powerups
             * (zero if not key operable,
             *  positive if time in seconds,
             *  negative if number of uses)
             */
            the_powerup->initial_value = GetAnInt(f);
            if (the_powerup->initial_value > 0) {
                the_powerup->initial_value *= 1000;
            }
            /* Action index */
            j = GetAnInt(f);
            if (j >= 0) {
                the_powerup->got_proc = gGot_procs[j];
                the_powerup->lose_proc = gLose_procs[j];
                the_powerup->periodic_proc = gPeriodic_procs[j];
            } else {
                the_powerup->lose_proc = NULL;
                the_powerup->periodic_proc = NULL;
                the_powerup->got_proc = NULL;
            }
            /*  Number of floating point params */
            the_powerup->number_of_float_params = GetAnInt(f);
            the_powerup->float_params = BrMemAllocate(sizeof(float) * the_powerup->number_of_float_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_float_params; j++) {
                the_powerup->float_params[j] = GetAScalar(f);
            }
            /* Number of integer params */
            the_powerup->number_of_integer_params = GetAnInt(f);
            the_powerup->integer_params = BrMemAllocate(sizeof(int) * the_powerup->number_of_integer_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_integer_params; j++) {
                the_powerup->integer_params[j] = GetAnInt(f);
            }
            /* Group inclusion */
            the_powerup->group_inclusion = GetAnInt(f);
            /* Pratcam sequence */
            the_powerup->prat_cam_event = GetAnInt(f);
            /* Flags (1 = net_global, 2 = net_inappropriate, 4 = off_before_snapweld) */
            the_powerup->net_type = GetAnInt(f);
        }
    }

    /* RESPAWN SPECS */

    /* Number of respawn specs */
    gNumber_of_powerup_respawn_specs = GetAnInt(f);
    gPowerup_respawn_specs = BrMemAllocate(gNumber_of_powerup_respawn_specs * sizeof(tPowerup_respawn_spec), kMem_powerup_array);
    for (i = 0; i < gNumber_of_powerup_respawn_specs; i++) {
        gPowerup_respawn_specs[i].bools = BrMemAllocate(gNumber_of_powerups * sizeof(tU8), kMem_powerup_array);
        /* Number of index groups */
        count_index_groups = GetAnInt(f);
        for (j = 0; j < count_index_groups; j++) {
            index_range = GetAnInt(f);
            if (index_range >= 0) {
                /* index_range is index */
                gPowerup_respawn_specs[i].bools[index_range] = 1;
            } else {
                /* index_range is range */
                first = GetAnInt(f);
                for (k = first; k < first - index_range; k++) {
                    gPowerup_respawn_specs[i].bools[k] = 1;
                }
            }
        }
    }
    gRace_powerup_respawn_bools = gPowerup_respawn_specs[0].bools;

    /* NETWORK AUTO-GOODIES */

    /* Min,max times between Auto-goodies */
    GetPairOfInts(f, &gNet_powerup_time_between_goodies_min, &gNet_powerup_time_between_goodies_max);
    gNet_powerup_time_between_goodies_min *= 1000;
    gNet_powerup_time_between_goodies_max *= 1000;
    /* Number of goodies to choose from */
    gNet_count_powerup_goodies = GetAnInt(f);
    gNet_powerup_goodies = BrMemAllocate(gNet_count_powerup_goodies * sizeof(int), kMem_powerup_array);
    for (i = 0; i < gNet_count_powerup_goodies; i++) {
        gNet_powerup_goodies[i] = GetAnInt(f);
    }
    PFfclose(f);
}

// InitRepulseEffects

// FUNCTION: CARMA2_HW 0x004da290
tPhysics_object* C2_HOOK_FAKE_THISCALL MungeBoxObject(br_model* pModel, undefined4 pArg2, float pMass) {
    tPhysics_object* box;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 0x4d8);

    box = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    box->shape = (tPhysics_shape*)AllocateShapeBox(kMem_collision_shape);
    box->shape->box.common.bb = pModel->bounds;
    FillInShape(box->shape);
    box->uid = gCollision_info_uid_counter;
    gCollision_info_uid_counter += 1;
    UpdateCollisionObject(box);
    box->M = pMass;
    BrVector3Set(&box->I,
        (REC2_SQR((pModel->bounds.max.v[1] - pModel->bounds.min.v[1]) * .5f) + REC2_SQR((pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) * .5f)) * pMass * (1.f / 12.f),
        (REC2_SQR((pModel->bounds.max.v[0] - pModel->bounds.min.v[0]) * .5f) + REC2_SQR((pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) * .5f)) * pMass * (1.f / 12.f),
        (REC2_SQR((pModel->bounds.max.v[0] - pModel->bounds.min.v[0]) * .5f) + REC2_SQR((pModel->bounds.max.v[1] - pModel->bounds.min.v[1]) * .5f)) * pMass * (1.f / 12.f));
    BrVector3SetFloat(&box->field_0x54, 0.f, 1/-17.25f, 0.f);
    box->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&box->actor->t.t.mat, &box->transform_matrix);
    box->actor->model = pModel;
    box->box_face_ref = gFace_num__car - 2;
    return box;
}

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

// FUNCTION: CARMA2_HW 0x004da530
void C2_HOOK_FASTCALL InitMineShit(void) {
    tPath_name the_path;
    tTWTVFS twt;
    int i;
    br_model* model;

    PathCat(the_path, gApplication_path, "MINE");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);

    C2_HOOK_BUG_ON(sizeof(tShit_mine) != 56);
    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);
    for (i = 0; i < (int)REC2_ASIZE(gShit_mines); i++) {
        gShit_mines[i].flags = 4;
        gShit_mines[i].field_0x35 = 0;
        model = BrModelFind("SHITMINE.ACT");
        gShit_mines[i].collision_info = MungeSphereObject(model, gMass_mine);
        gShit_mines[i].collision_info->uid = i;
        gShit_mines[i].collision_info->flags_0x238 = 0x40;
        gShit_mines[i].collision_info->owner = &gShit_mines[i];
        BrMatrix34Translate(&gShit_mines[i].collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gShit_mines[i].collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
    }
}

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);

typedef struct {
    br_actor* actor;
    undefined4 field_0x10;
    br_material* material;
    float field_0x18;
    float field_0x1c;
    tCar_spec* car;
    undefined4 field_0x24;
    undefined4 pad[3];
} tRepulse_link_tail;

// FUNCTION: CARMA2_HW 0x004d9ea0
void C2_HOOK_FASTCALL InitPowerups(void) {
    br_pixelmap* repulse;
    int i;
    int j;
    tPhysics_object* parent_collision_info;
    tPath_name the_path;
    tTWTVFS twt;
    tRepulse_link_tail* link;

    repulse = LoadPixelmap("REPULSE.PIX");
    if (!repulse) {
        FatalError(kFatalError_CantFindFile_S, "REPULSE.PIX");
    }
    BrMapAdd(repulse);

    link = (tRepulse_link_tail*)&gRepulse_links[0].actor;
    do {
        br_model* model;
        br_material* material;
        br_face* face;
        br_vertex* vertex;

        link->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        model = BrModelAllocate(NULL, 102, 100);
        model->flags |= BR_MODF_UPDATEABLE;
        material = BrMaterialAllocate("");
        material->colour_map = repulse;
        material->flags &= ~BR_MATF_LIGHT;
        material->flags |= BR_MATF_TWO_SIDED;
        BrMaterialAdd(material);
        BlendifyMaterial(material, 100);
        BrMaterialUpdate(material, BR_MATU_ALL);
        face = model->faces;
        for (j = 0; j < model->nfaces / 2; j++) {
            face->material = material;
            face->vertices[0] = j + 0;
            face->vertices[1] = j + 1;
            face->vertices[2] = j + 1 + model->nvertices / 2;
            face++;
        }
        for (j = model->nfaces / 2; j < model->nfaces; j++) {
            face->material = material;
            face->vertices[0] = j + 1;
            face->vertices[1] = j + 2;
            face->vertices[2] = j + 1 - model->nvertices / 2;
            face++;
        }
        vertex = model->vertices;
        for (j = 0; j < model->nvertices / 2; j++) {
            vertex->map.v[0] = (float)(j & 1);
            vertex->map.v[1] = 0.f;
            vertex++;
        }
        for (j = model->nvertices / 2; j < model->nvertices; j++) {
            vertex->map.v[0] = (float)(j & 1);
            vertex->map.v[1] = 1.f;
            vertex++;
        }
        link->actor->model = model;
        link->actor->render_style = BR_RSTYLE_FACES;
        link->material = material;
        BrModelAdd(model);
        link++;
    } while ((int)link < (int)&gINT_006a0a5c);

    parent_collision_info = NULL;
    PathCat(the_path, gApplication_path, "TAIL");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);
    for (i = 0; i < gCount_mutant_tail_parts; i++) {
        int is_last;
        tPhysics_object* collision_info;
        br_model* model;

        is_last = i == gCount_mutant_tail_parts - 1;
        if (is_last) {
            model = BrModelFind("TAILBALL");
            collision_info = MungeSphereObject(model, gMass_mutant_tail_ball);
        } else {
            model = BrModelFind("TAILLINK");
            collision_info = MungeBoxObject(model, 0, gMass_mutant_tail_link);
        }
        collision_info->flags_0x238 = 0x20;
        collision_info->physics_joint1 = AllocatePhysicsJoint(1, kMem_physics_joint);
        collision_info->physics_joint1->type = eJoint_ball_n_socket;
        BrVector3SetFloat(&collision_info->physics_joint1->field_0x08,
                          0.f, 0.f, model->bounds.min.v[2] * (is_last ? 0.45 : 0.99));
        if (i != 0) {
            BrVector3SetFloat(&collision_info->physics_joint1->field_0x14,
                              0.f, 0.f, model->bounds.max.v[2] * (is_last ? 0.45 : 0.99));
            PhysicsAddObject(parent_collision_info, collision_info);
        } else {
            gMutant_tail_first_collision_info = collision_info;
            gMutant_tail_first_joint = collision_info->physics_joint1;
            collision_info->physics_joint1->count_limits = 1;
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].child, 0.f, 0.f, 1.f);
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].parent, 0.f, 0.f, 1.f);
        }
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis2, 0.f, 0.f, 1.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis3, 0.f, 1.f, 0.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis, 1.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
        parent_collision_info = collision_info;
    }
    gMutant_tail_state = 0;

    InitMineShit();
}

// RemoveRepulseEffect

// ResetRepulseEffects

// FUNCTION: CARMA2_HW 0x004da600
void C2_HOOK_FASTCALL EmptyInventory(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gInventory) != 20);

    for (i = 0; i < REC2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
}
// ResetPowerups

// STUB: CARMA2_HW 0x004da730
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {
    NOT_IMPLEMENTED();
}

// NewRepulseRay

// RemoveRepulsiveReferences

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004db8e0
void C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// AdjustRepulseRay

// FUNCTION: CARMA2_HW 0x004da990
void C2_HOOK_FASTCALL ResetRepulseRay(void) {
    int i = 0;

    do {
        tRepulse_link* link = &gRepulse_links[i];
        if (link->actor->parent == gNon_track_actor) {
            BrActorRemove(link->actor);
        }
        link->time = 0;
        i++;
    } while ((int)&gRepulse_links[i].actor < (int)&gINT_006a0a5c);
}

// FUNCTION: CARMA2_HW 0x004daff0
int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x) {
    br_pixelmap *fizzle_pix;
    char s[8];
    int timer;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dx, 0x294);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dy, 0x2a0);

    if (pIcon != NULL && pIcon->fizzle_stage < 4) {
        fizzle_pix = gFizzle_in[pPowerup->fizzle_type];
        if (pDraw) {
            PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 1, pTime);
        }
        if (pUpdate_x) {
            return pX + fizzle_pix->width;
        }
        return 0;
    }
    if (pDraw) {
        PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 0, pTime);
    }
    if (pPowerup->initial_value < 0) {
        sprintf(s, "%d", -pPowerup->value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->initial_value > 0) {
        TimerString(pPowerup->value, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->type == ePowerup_timed) {

        timer = pPowerup->lose_time - pTime;
        timer = timer < 0 ? 0 : timer;
        TimerString(timer, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[1],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[1], s);
        }
    } else if (pPowerup->current_value > 0) {

        sprintf(s, "%d", pPowerup->current_value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    }
    return 0;
}
// DrawPowerupIcons2

// DrawKeyPowerups

// FUNCTION: CARMA2_HW 0x004da9d0
void C2_HOOK_FASTCALL ToggleInventory(void) {

    gInventory_cycling = !gInventory_cycling;
    if (gInventory_cycling) {
        gInventory_timeout = GetTotalTime() + 3000;
    }
}
// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

/* inlined into MungePowerupStuff in the original (no standalone copy) */
static void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
        tRespawn_powerup* respawn_powerup = &gRespawn_powerups[i];
        if (respawn_powerup->actor == NULL || (int)pTime < (int)respawn_powerup->respawn_time) {
            continue;
        }
        if (respawn_powerup->actor->render_style != BR_RSTYLE_FACES) {
            respawn_powerup->actor->render_style = BR_RSTYLE_FACES;
        }
        PipeSinglePowerupRespawn(respawn_powerup->actor, respawn_powerup->index);
        DoPowerupRespawnSmash(respawn_powerup->actor);
        respawn_powerup->actor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004db880
void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pArg1) {
    tU32 time;

    time = GetTotalTime();
    MungeRepulseRays(time);
    CheckRespawnQueue(time);
}

// GotPowerupN

// FUNCTION: CARMA2_HW 0x004dbdb0
void C2_HOOK_FASTCALL GotPowerup0(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 0, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbe10
void C2_HOOK_FASTCALL GotPowerup1(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 1, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbe70
void C2_HOOK_FASTCALL GotPowerup2(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 2, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbed0
void C2_HOOK_FASTCALL GotPowerup3(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 3, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbf30
void C2_HOOK_FASTCALL GotPowerup4(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 4, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbf90
void C2_HOOK_FASTCALL GotPowerup5(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 5, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbff0
void C2_HOOK_FASTCALL GotPowerup6(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 6, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dc050
void C2_HOOK_FASTCALL GotPowerup7(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 7, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dc0b0
void C2_HOOK_FASTCALL GotPowerup8(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 8, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dc110
void C2_HOOK_FASTCALL GotPowerup9(void) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + 9, 1, 1);
}

// GotCredits

// FUNCTION: CARMA2_HW 0x004dc3e0
int C2_HOOK_FASTCALL GainAPO2(tCar_spec* pCar, int pIndex, int pAmount) {
    int old;

    while (pAmount < 0) {
        if (pCar->power_up_levels[pIndex] >= pCar->power_up_slots[pIndex]) {
            return 0;
        }
        pAmount = pCar->power_up_slots[pIndex] - pCar->power_up_levels[pIndex];
    }
    if (pIndex == 1 && gNet_mode != eNet_mode_none
        && gCurrent_net_game->type == eNet_game_type_foxy
        && gThis_net_player_index == gIt_or_fox) {
        return 0;
    }
    old = pCar->power_up_levels[pIndex];
    pCar->power_up_levels[pIndex] = old + pAmount;
    if (pCar->power_up_levels[pIndex] > pCar->power_up_slots[pIndex]) {
        pCar->power_up_levels[pIndex] = pCar->power_up_slots[pIndex];
    }
    return pCar->power_up_levels[pIndex] - old;
}

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

// FUNCTION: CARMA2_HW 0x004dc720
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gPedestrian_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// SetPedSize

// SetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dc7d0
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar) {

    gExploding_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc800
// FUNCTION: CARMA2_SW 0x004d8d00
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc830
int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    pCar->invulnerable_no_crushage = 1;
    pCar->invulnerable_no_damage = 1;
    return pPowerup - gPowerup_array;
}

// ResetInvulnerability

// FUNCTION: CARMA2_HW 0x004dc8a0
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc8d0
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc8f0
// FUNCTION: CARMA2_SW 0x004d8df0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc920
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dc930
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = !gFreeze_timer;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc970
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = 0;
    }
}

// DoInstantRepair

// FUNCTION: CARMA2_HW 0x004dc9c0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    gPedestrian_speed_factor = 1.f;
}

// ResetPedSize

// ResetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dca10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {

    gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 0;
}

// SetEngineFactor

// FUNCTION: CARMA2_HW 0x004dcab0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 1;
    return pPowerup - gPowerup_array;
}

// TrashBodywork

// FUNCTION: CARMA2_HW 0x004dcb90
int C2_HOOK_FASTCALL TakeDrugs(tPowerup* powerup, tCar_spec* car) {
    if (car != NULL && car->driver == eDriver_local_human) {
        if (gDrug_trip_data == NULL) {
            gDrug_trip_data = BrMemAllocate(0x400, 0xfd);
            memcpy(gDrug_trip_data, gRender_palette->pixels, 0x400);
        }
        gOn_drugs = 1;
    }
    return powerup - gPowerup_array;
}

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004dcd70
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gCop_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcda0
int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcdd0
int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar) {

    gPinball_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce00
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce30
int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car) {

    car->bounce_rate = gBounce_rate_const / powerup->float_params[0];
    car->bounce_amount = powerup->float_params[1];
    car->last_bounce = 0;
    return powerup - gPowerup_array;
}

// SetSuspension

// FUNCTION: CARMA2_HW 0x004dced0
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf00
// FUNCTION: CARMA2_SW 0x004d9400
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf30
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* powerup, tCar_spec* car) {

    gImmortal_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf40
// FUNCTION: CARMA2_SW 0x004d9440
int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf40
int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* powerup, tCar_spec* car) {

    gPeds_suicidal = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf70
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* powerup, tCar_spec* car) {

    gPeds_suicidal = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf80
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FillInMine

// SendMines

// MinePreCollisionStuff

// ActivateMine

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// MineExplode

// TiggerMineToExplode

// HitAShatOutMine

// ClearMineReferencesToCar

// MungeShatMines

// ARDoShitMine

// ARUndoShitMine

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

// RepulseOpponents2

// RepulseOpponents

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

// FUNCTION: CARMA2_HW 0x004de600
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 0;
}

// PukeDrugsBackUp

// ResetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004de710
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {

    gCop_speed_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de720
void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = gDefault_gravity;
}

// FUNCTION: CARMA2_HW 0x004de730
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {

    gPinball_factor = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de740
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004de750
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);

    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
}

// ResetSuspension

// FUNCTION: CARMA2_HW 0x004de780
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de790
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = 1.f;
}

// PickAtRandom

// FUNCTION: CARMA2_HW 0x004de7d0
int C2_HOOK_FASTCALL GotVouchers(tPowerup* powerup, tCar_spec* car) {

    if (car && car->driver == eDriver_local_human) {
        AddVouchers(*powerup->integer_params);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de810
void C2_HOOK_FASTCALL MungeVouchers(tPowerup* powerup, tU32 time) {

    (void)time;
    powerup->current_value = GetRecoverVoucherCount();
}

// FUNCTION: CARMA2_HW 0x004de820
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {

    if (car && car->driver == eDriver_local_human) {
        gInstant_handbrake = 1;
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de850
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {

    if (car && car->driver == eDriver_local_human) {
        gInstant_handbrake = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004de870
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car) {

    gDancing_peds = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8a0
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car) {

    gPanicking_peds = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8d0
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* powerup, tCar_spec* car) {

    gPed_gravity_multiplier = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de900
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car) {

    gDrunk_pedestrians = 1;
    return powerup - gPowerup_array;
}

// SetGhostPeds

// FUNCTION: CARMA2_HW 0x004de960
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de970
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de980
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de990
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* powerup, tCar_spec* car) {

    gDrunk_pedestrians = 0;
}

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// FUNCTION: CARMA2_HW 0x004debf0
void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* powerup, tCar_spec* car) {

    car->field_0x4c8 = 1.0f;
    if (gNet_mode) {
        RestoreCarPixelmaps(car);
    }
}

// FUNCTION: CARMA2_HW 0x004dec10
int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec40
void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dec60
int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = pPowerup->float_params[0] * pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec90
void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = 0.f;
}

// FUNCTION: CARMA2_HW 0x004deca0
int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* powerup, tCar_spec* car) {

    gMutant_speed = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004decd0
void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* powerup, tCar_spec* car) {

    gMutant_speed = 0.0f;
}

// ReceivedPowerup

// SendCurrentPowerups

// LoseAllLocalPowerups

// LoseDodgyPowerups

// FUNCTION: CARMA2_HW 0x004df1e0
int C2_HOOK_FASTCALL GetPowerupCount(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004df1f0
// FUNCTION: CARMA2_SW 0x004db6f0
void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition) {

}
// FUNCTION: CARMA2_HW 0x004c8da0
void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {

    ARDoSingleVariedSession(0x34, (uintptr_t)pPowerup, 1, 4, 0, (uintptr_t)pCar);
}

// FUNCTION: CARMA2_HW 0x004c8dc0
void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar) {

    ARDoSingleVariedSession(0x35, (uintptr_t)pPowerup, 1, 4, 0, (uintptr_t)pCar);
}

// SetPissed

// FUNCTION: CARMA2_HW 0x004df300
void C2_HOOK_FASTCALL ResetPissed(tPowerup* powerup, tCar_spec* car) {
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    gCredit_multiplier = 0;
    sx = gCamera->t.t.mat.m[3][0];
    sy = gCamera->t.t.mat.m[3][1];
    sz = gCamera->t.t.mat.m[3][2];
    BrMatrix34Identity(&gCamera->t.t.mat);
    gCamera->t.t.mat.m[3][0] = sx;
    gCamera->t.t.mat.m[3][1] = sy;
    gCamera->t.t.mat.m[3][2] = sz;
    sx = gRearview_camera->t.t.mat.m[3][0];
    sy = gRearview_camera->t.t.mat.m[3][1];
    sz = gRearview_camera->t.t.mat.m[3][2];
    BrMatrix34Identity(&gRearview_camera->t.t.mat);
    gRearview_camera->t.t.mat.m[3][0] = sx;
    gRearview_camera->t.t.mat.m[3][1] = sy;
    gRearview_camera->t.t.mat.m[3][2] = sz;
}

// PissedEffect

// FUNCTION: CARMA2_HW 0x004df650
void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen) {

    if (actor->user == NULL) {
        actor->user = (void*)(uintptr_t)GetTotalTime();
    } else {
        tU32 now;
        tU32 delta;

        now = GetTotalTime();
        delta = now - (tU32)(uintptr_t)actor->user;
        actor->user = (void*)(uintptr_t)now;
        BrMatrix34PreRotateY(&actor->t.t.mat, BR_ANGLE_DEG(delta / 16));
    }
    BrZsModelRender(actor, model, material, order_table, style, on_screen, 0);
}
// FUNCTION: CARMA2_HW 0x004dfe10
void C2_HOOK_CDECL RenderChangingPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen) {

    if (actor->user == NULL) {
        actor->user = (void*)(uintptr_t)GetTotalTime();
        actor->model = gModel_powerup_armor;
    } else {
        tU32 prev;
        tU32 now;
        tU32 diff;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)actor->user;
        diff = (now - prev) / 16;
        if (diff > 15) {
            if (actor->model == gModel_powerup_armor) {
                actor->model = gModel_powerup_power;
            } else if (actor->model == gModel_powerup_power) {
                actor->model = gModel_powerup_offence;
            } else {
                actor->model = gModel_powerup_armor;
            }
            actor->user = (void*)(uintptr_t)now;
        }
        BrMatrix34PreRotateY(&actor->t.t.mat, BR_ANGLE_DEG(diff + 12));
    }
    BrZsModelRender(actor, model, material, order_table, style, on_screen, 0);
}
// FUNCTION: CARMA2_HW 0x004df570
void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity) {
    // GLOBAL: CARMA2_HW 0x0065ebc0
    static br_token_value spinning_powerup_extra_prim[] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { BrScalarToFixed(75) } },
    };
    br_uint_32 i;

    if (pActor == NULL) {
        return;
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.f, .1f, 0.f);
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderSpinningPowerup;
    if (pOpacity == 0 || pOpacity == 255) {
        return;
    }
    if (pActor->material != NULL) {
        pActor->material->extra_prim = spinning_powerup_extra_prim;
        pActor->material->extra_prim[1].v.x = pOpacity << 16;
        BrMaterialUpdate(pActor->material, BR_MATU_EXTRA_PRIM);
        return;
    }
    for (i = 0; i < pActor->model->nfaces; i++) {
        if (pActor->model->faces[i].material != NULL) {
            pActor->model->faces[i].material->extra_prim = spinning_powerup_extra_prim;
            pActor->model->faces[i].material->extra_prim[1].v.x = pOpacity << 16;
            BrMaterialUpdate(pActor->model->faces[i].material, BR_MATU_EXTRA_PRIM);
        }
    }
}
// DuplicateModel

static br_model* SetChangingPowerup_DupModel(br_model* pModel, const char* pName) {
    br_model* copy;

    copy = BrModelAllocate((char*)pName, pModel->nvertices, pModel->nfaces);
    if (copy == NULL) {
        return copy;
    }
    copy->pivot = pModel->pivot;
    copy->flags = pModel->flags;
    copy->user = pModel->user;
    copy->crease_angle = pModel->crease_angle;
    copy->radius = pModel->radius;
    copy->bounds = pModel->bounds;
    memcpy(copy->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    memcpy(copy->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    return copy;
}

// FUNCTION: CARMA2_HW 0x004df6c0
void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor) {
    tPath_name path;
    char s[256];
    char s2[256];
    FILE* f;
    int i;
    br_actor* actor;

    if (pActor == NULL) {
        return;
    }
    if (gModel_powerup_armor == NULL || gModel_powerup_power == NULL || gModel_powerup_offence == NULL) {
        gModel_powerup_armor = SetChangingPowerup_DupModel(BrModelFind("&68powerup1.ACT"), "PowArm");
        gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
        gModel_powerup_armor->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_armor);

        gModel_powerup_power = SetChangingPowerup_DupModel(BrModelFind("&70powerup1.ACT"), "PowPow");
        gModel_powerup_power->flags |= BR_MODF_CUSTOM;
        gModel_powerup_power->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_power);

        gModel_powerup_offence = SetChangingPowerup_DupModel(BrModelFind("&69powerup1.ACT"), "PowOff");
        gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
        gModel_powerup_offence->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_offence);
    }

    if (gModel_powerup_armor == NULL) {
        BrModelFind("&77powerup.ACT");
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 67; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_armor = SetChangingPowerup_DupModel(actor->model, "PowArm");
            gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
            gModel_powerup_armor->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_armor);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_power == NULL) {
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 68; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_power = SetChangingPowerup_DupModel(actor->model, "PowPow");
            gModel_powerup_power->flags |= BR_MODF_CUSTOM;
            gModel_powerup_power->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_power);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_offence == NULL) {
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 69; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_offence = SetChangingPowerup_DupModel(actor->model, "PowOff");
            gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
            gModel_powerup_offence->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_offence);
            BrActorFree(actor);
        }
        PFfclose(f);
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, .0f, .1f, .0f);
    pActor->model = gModel_powerup_armor;
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderChangingPowerup;
}

extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern int C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet* pOutlet, tS3_sound_id pSound, int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch);
extern tS3_outlet* gCar_outlet;

// FUNCTION: CARMA2_HW 0x004dfec0
int C2_HOOK_FASTCALL PedValium(tPowerup* powerup, tCar_spec* car) {

    if (CalmDownAllPeds()) {
        NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_pedestrians_are_calm_now));
        DRS3StartSound2(gCar_outlet, eSoundId_PedValium, 1, 0xff, 0xff, -1, -1);
        return powerup - gPowerup_array;
    }
    return -1;
}

// PowerupCancel

// GrowATail

// CutOffTail

// STUB: CARMA2_HW 0x004e06a0
void C2_HOOK_FASTCALL RemoveTail(void) {
    NOT_IMPLEMENTED();
}

// DoInitialTail

// UndoInitialTail

// FUNCTION: CARMA2_HW 0x004e0750
void C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* actor) {
    int i;

    if (actor != NULL && gRace_powerup_respawn_bools[pPowerup_index] && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.powerup_respawn)) {
        for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
            if (gRespawn_powerups[i].actor == NULL) {
                gRespawn_powerups[i].actor = actor;
                gRespawn_powerups[i].index = pPowerup_index;;
                gRespawn_powerups[i].respawn_time = GetTotalTime() + gPickup_respawn_max_extra_time_ms / 2 + gPickup_respawn_min_time_ms;
                break;
            }
        }
    }
}

// ARDoPowerupRespawn

// ARUndoPowerupRespawn

// FUNCTION: CARMA2_HW 0x004e07f0
void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
    float f;
    float scale;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale != 0) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.0f;
            if (pIcon->fizzle_direction > 0) {
                f = 1.0f - f;
                scale = 1.0f + 5.0f * f;
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.0f);
            } else {
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, 1.01f - f, 1.01f - f, 1.0f);
            }
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, (br_angle)((1.0f - f / 2.0f) * 65536.0f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.0f - f) * 255.0f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT(255);
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004e09d0
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture) {
    br_actor* actor;
    br_material* material;
    br_model* model;
    int width;
    int height;
    br_scalar left;
    br_scalar right;
    br_scalar top;
    br_scalar bottom;

    width = pTexture->width;
    height = pTexture->height;
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    left = (float)-(width / 2);
    top = (float)+(height / 2);
    BrVector3Set(&model->vertices[0].p, left,  top, -2.0f);
    bottom = (float)-(height / 2);
    BrVector3Set(&model->vertices[1].p, left, bottom, -2.0f);
    right = (float)+(width / 2);
    BrVector3Set(&model->vertices[2].p, right, bottom, -2.0f);
    BrVector3Set(&model->vertices[3].p, right,  top, -2.0f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 0.0f);
    material->colour = 0;
    material->colour_map = pTexture;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pTexture);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x004e0c00
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c10
void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {

    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c20
void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {

    gPowerup_txt_path = "ZOMPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c30
void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {

    gPowerup_txt_path = "ALPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && !IsCarCloaked(pCar)) {
        BlendifyCar(pCar);
        MasterDisableCarFunks(pCar);
        gCloaked_cars[gCount_cloaked_cars++] = pCar;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {

            if (gCloaked_cars[i] == pCar) {
#ifdef REC2_FIX_BUGS
                memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1], (gCount_cloaked_cars - i - 1) * sizeof(tCar_spec*));
#else
                memcpy(&gCloaked_cars[i], &gCloaked_cars[i + 1], (REC2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec*));
#endif
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0d50
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(pCar);
        RemoveFromCloakingList(pCar);
        MasterEnableCarFunks(pCar);
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004e0de0
void C2_HOOK_FASTCALL PeriodicCloaking(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00420910
tPhysicsError C2_HOOK_FASTCALL AddPolyhedronCollisionShapePlanes(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

extern int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);

// FUNCTION: CARMA2_HW 0x004211f0
tPhysicsError C2_HOOK_FASTCALL ProcessTetrahedronPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdges) {
    int first_non_colinear;
    int i;
    br_vector3 tv2;
    br_vector3 tv1;
    br_scalar len;
    double scale;

    first_non_colinear = 0;
    while (Collinear(&pPolyhedron->points[first_non_colinear + 0], &pPolyhedron->points[first_non_colinear + 1], &pPolyhedron->points[first_non_colinear + 2])) {
        first_non_colinear++;
        if (first_non_colinear == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }

    if (first_non_colinear != 0) {
        pPolyhedron->points[0].v[0] = pPolyhedron->points[first_non_colinear + 0].v[0];
        pPolyhedron->points[0].v[1] = pPolyhedron->points[first_non_colinear + 0].v[1];
        pPolyhedron->points[0].v[2] = pPolyhedron->points[first_non_colinear + 0].v[2];
        pPolyhedron->points[1].v[0] = pPolyhedron->points[first_non_colinear + 1].v[0];
        pPolyhedron->points[1].v[1] = pPolyhedron->points[first_non_colinear + 1].v[1];
        pPolyhedron->points[1].v[2] = pPolyhedron->points[first_non_colinear + 1].v[2];
        pPolyhedron->points[2].v[0] = pPolyhedron->points[first_non_colinear + 2].v[0];
        pPolyhedron->points[2].v[1] = pPolyhedron->points[first_non_colinear + 2].v[1];
        pPolyhedron->points[2].v[2] = pPolyhedron->points[first_non_colinear + 2].v[2];
    }
    pPolyhedron->edges[0].index1 = 0;
    pPolyhedron->edges[0].index2 = 1;
    pPolyhedron->edges[1].index1 = 1;
    pPolyhedron->edges[1].index2 = 2;
    pPolyhedron->edges[2].index1 = 0;
    pPolyhedron->edges[2].index2 = 2;

    /* PlaneEquation */
    tv1.v[0] = pPolyhedron->points[2].v[0] - pPolyhedron->points[1].v[0];
    tv1.v[1] = pPolyhedron->points[2].v[1] - pPolyhedron->points[1].v[1];
    tv1.v[2] = pPolyhedron->points[2].v[2] - pPolyhedron->points[1].v[2];
    tv2.v[0] = pPolyhedron->points[1].v[0] - pPolyhedron->points[0].v[0];
    tv2.v[1] = pPolyhedron->points[1].v[1] - pPolyhedron->points[0].v[1];
    tv2.v[2] = pPolyhedron->points[1].v[2] - pPolyhedron->points[0].v[2];
    pPolyhedron->planes[0].v[0] = tv2.v[1] * tv1.v[2] - tv2.v[2] * tv1.v[1];
    pPolyhedron->planes[0].v[1] = tv2.v[2] * tv1.v[0] - tv2.v[0] * tv1.v[2];
    pPolyhedron->planes[0].v[2] = tv2.v[0] * tv1.v[1] - tv2.v[1] * tv1.v[0];
    len = BR_LENGTH3(pPolyhedron->planes[0].v[0], pPolyhedron->planes[0].v[1], pPolyhedron->planes[0].v[2]);
    if (len > BR_SCALAR_EPSILON * 2) {
        scale = 1.0 / len;
        pPolyhedron->planes[0].v[0] = pPolyhedron->planes[0].v[0] * scale;
        pPolyhedron->planes[0].v[1] = pPolyhedron->planes[0].v[1] * scale;
        pPolyhedron->planes[0].v[2] = pPolyhedron->planes[0].v[2] * scale;
    } else {
        pPolyhedron->planes[0].v[0] = 1.0f;
        pPolyhedron->planes[0].v[1] = 0.0f;
        pPolyhedron->planes[0].v[2] = 0.0f;
    }
    pPolyhedron->planes[0].v[3] = -(pPolyhedron->planes[0].v[1] * pPolyhedron->points[0].v[1] + pPolyhedron->planes[0].v[2] * pPolyhedron->points[0].v[2] + pPolyhedron->planes[0].v[0] * pPolyhedron->points[0].v[0]);

    pEdges[2].index1 = 0;
    pEdges[1].index1 = 0;
    pEdges[0].index1 = 0;

    for (i = 0; i < pPolyhedron->count_points; i++) {

        if ((float)fabs(pPolyhedron->planes[0].v[0] * pPolyhedron->points[i].v[0] + pPolyhedron->planes[0].v[1] * pPolyhedron->points[i].v[1] + pPolyhedron->planes[0].v[2] * pPolyhedron->points[i].v[2] + pPolyhedron->planes[0].v[3]) > 1e-6f) {
            break;
        }
    }
    if (i == pPolyhedron->count_points) {
        return 7;
    }
    pPolyhedron->points[3].v[0] = pPolyhedron->points[i].v[0];
    pPolyhedron->points[3].v[1] = pPolyhedron->points[i].v[1];
    pPolyhedron->points[3].v[2] = pPolyhedron->points[i].v[2];
    /* Points inside the polyhedron must have positive plane value */
    if (pPolyhedron->planes[0].v[0] * pPolyhedron->points[i].v[0] + pPolyhedron->planes[0].v[1] * pPolyhedron->points[i].v[1] + pPolyhedron->planes[0].v[2] * pPolyhedron->points[i].v[2] + pPolyhedron->planes[0].v[3] < 1e-6f) {
        pPolyhedron->planes[0].v[0] = -pPolyhedron->planes[0].v[0];
        pPolyhedron->planes[0].v[1] = -pPolyhedron->planes[0].v[1];
        pPolyhedron->planes[0].v[2] = -pPolyhedron->planes[0].v[2];
        pPolyhedron->planes[0].v[3] = -pPolyhedron->planes[0].v[3];
    }
    for (i = 0; i < 3; i++) {
        int o;
        int o_i;

        o = i + 1;
        o_i = o % 3;

        pPolyhedron->edges[3 + i].index1 = i;
        pPolyhedron->edges[3 + i].index2 = 3;

        /* PlaneEquation */
        tv1.v[0] = pPolyhedron->points[3].v[0] - pPolyhedron->points[i].v[0];
        tv1.v[1] = pPolyhedron->points[3].v[1] - pPolyhedron->points[i].v[1];
        tv1.v[2] = pPolyhedron->points[3].v[2] - pPolyhedron->points[i].v[2];
        tv2.v[0] = pPolyhedron->points[o_i].v[0] - pPolyhedron->points[i].v[0];
        tv2.v[1] = pPolyhedron->points[o_i].v[1] - pPolyhedron->points[i].v[1];
        tv2.v[2] = pPolyhedron->points[o_i].v[2] - pPolyhedron->points[i].v[2];
        pPolyhedron->planes[o].v[0] = tv1.v[1] * tv2.v[2] - tv1.v[2] * tv2.v[1];
        pPolyhedron->planes[o].v[1] = tv1.v[2] * tv2.v[0] - tv1.v[0] * tv2.v[2];
        pPolyhedron->planes[o].v[2] = tv1.v[0] * tv2.v[1] - tv1.v[1] * tv2.v[0];
        len = BR_LENGTH3(pPolyhedron->planes[o].v[0], pPolyhedron->planes[o].v[1], pPolyhedron->planes[o].v[2]);
        if (len > BR_SCALAR_EPSILON * 2) {
            scale = 1.0 / len;
            pPolyhedron->planes[o].v[0] = pPolyhedron->planes[o].v[0] * scale;
            pPolyhedron->planes[o].v[1] = pPolyhedron->planes[o].v[1] * scale;
            pPolyhedron->planes[o].v[2] = pPolyhedron->planes[o].v[2] * scale;
        } else {
            pPolyhedron->planes[o].v[0] = 1.0f;
            pPolyhedron->planes[o].v[1] = 0.0f;
            pPolyhedron->planes[o].v[2] = 0.0f;
        }
        pPolyhedron->planes[o].v[3] = -(pPolyhedron->planes[o].v[1] * pPolyhedron->points[3].v[1] + pPolyhedron->planes[o].v[2] * pPolyhedron->points[3].v[2] + pPolyhedron->planes[o].v[0] * pPolyhedron->points[3].v[0]);

        pEdges[i].index2 = o;
        pEdges[3 + i].index1 = o;
        pEdges[3 + i].index2 = 1 + (o + 1) % 3;
    }

    for (i = 0; i < 3; i++) {

        if (pPolyhedron->planes[1 + i].v[0] * pPolyhedron->points[(2 + i) % 3].v[0] + pPolyhedron->planes[1 + i].v[1] * pPolyhedron->points[(2 + i) % 3].v[1] + pPolyhedron->planes[1 + i].v[2] * pPolyhedron->points[(2 + i) % 3].v[2] + pPolyhedron->planes[1 + i].v[3] < 1e-6f) {
            pPolyhedron->planes[1 + i].v[0] = -pPolyhedron->planes[1 + i].v[0];
            pPolyhedron->planes[1 + i].v[1] = -pPolyhedron->planes[1 + i].v[1];
            pPolyhedron->planes[1 + i].v[2] = -pPolyhedron->planes[1 + i].v[2];
            pPolyhedron->planes[1 + i].v[3] = -pPolyhedron->planes[1 + i].v[3];
        }
    }

    pPolyhedron->count_points = 4;
    pPolyhedron->count_planes = 4;
    pPolyhedron->count_edges = 6;
    return ePhysicsError_Ok;
}

#define PLANE_VALUE(pPolyhedron, i, pVertex) (BrVector3Dot(pVertex, &(pPolyhedron)->planes[i]) + (pPolyhedron)->planes[i].v[3])

typedef enum {
    ePlaneSide_positive = 0,
    ePlaneSide_negative = 1,
    ePlaneSide_zero = 2,
} tPlane_side;

// FUNCTION: CARMA2_HW 0x00421790
int C2_HOOK_FASTCALL PolyhedronCollisionShape_AddPoint(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pVertex, tU8 pVertex_index, tPolyhedron_edge_indexes* pEdge_to_plane_indices) {
    tPolyhedron_edge_indexes new_edges[894];
    tPolyhedron_edge_indexes new_edge_to_plane_indices[894];
    tPlane_side plane_side_buffer[596];
    int positive_side_edges[894];
    br_vector4 new_planes[596];
    int all_positive;
    tU8 i;
    int j;
    int count_new_planes;
    int count_new_edges;
    tU8 final_count_edges;
    tU8 final_count_planes;

    all_positive = 1;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        plane_side_buffer[i] = PLANE_VALUE(pPolyhedron, i, pVertex) < -1e-6f ? ePlaneSide_negative : (PLANE_VALUE(pPolyhedron, i, pVertex) < 1e-6f ? ePlaneSide_zero : ePlaneSide_positive);
        all_positive &= !(plane_side_buffer[i] & ePlaneSide_negative);
    }
    /* If all positive ==> point is inside polyhedron */
    if (all_positive) {
        return 0;
    }

    count_new_planes = 0;
    count_new_edges = 0;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        tU8 plane1;
        tPlane_side side1;
        tU8 plane2;
        tPlane_side side2;

        plane1 = pEdge_to_plane_indices[i].index1;
        side1 = plane_side_buffer[plane1];
        plane2 = pEdge_to_plane_indices[i].index2;
        side2 = plane_side_buffer[plane2];
        positive_side_edges[i] = side1 == ePlaneSide_positive || side2 == ePlaneSide_positive;
        if ((side1 ^ side2) & ePlaneSide_negative) {
            /* Exactly one plane of edge has vertex on negative side */
            if (!(side1 & ePlaneSide_zero) && !(side2 & ePlaneSide_zero)) {
                br_vector3 tv1, tv2;
                /* vertex does not lie on edge */
                BrVector3Sub(&tv1, pVertex, &pPolyhedron->points[pPolyhedron->edges[i].index1]);
                BrVector3Sub(&tv2, &pPolyhedron->points[pPolyhedron->edges[i].index2], &pPolyhedron->points[pPolyhedron->edges[i].index1]);
                BrVector3Cross(&new_planes[count_new_planes], &tv1, &tv2);
                BrVector3Normalise(&new_planes[count_new_planes], &new_planes[count_new_planes]);
                if (BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane1]) * BrVector3Dot(&new_planes[count_new_planes], &pPolyhedron->planes[plane2])
                        - BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane2]) * BrVector3Dot(&new_planes[count_new_planes], &pPolyhedron->planes[plane1]) < 0.f) {
                    BrVector4Negate(&new_planes[count_new_planes], &new_planes[count_new_planes]);
                }
                new_planes[count_new_planes].v[3] = -BrVector3Dot(&new_planes[count_new_planes], pVertex);
                if (side1 == ePlaneSide_positive) {
                    pEdge_to_plane_indices[i].index2 = count_new_planes + pPolyhedron->count_planes;
                } else {
                    pEdge_to_plane_indices[i].index1 = count_new_planes + pPolyhedron->count_planes;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        count_new_planes += 1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                    count_new_planes += 1;
                }
            } else {
                if (!(side1 & ePlaneSide_zero)) {
                    plane1 = plane2;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

            }
        }
    }

    final_count_edges = 0;
    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (positive_side_edges[i]) {
            if (i != final_count_edges) {
                pPolyhedron->edges[final_count_edges] = pPolyhedron->edges[i];
                pEdge_to_plane_indices[final_count_edges] = pEdge_to_plane_indices[i];
            }
            final_count_edges += 1;
        }
    }
    for (i = 0; i < count_new_edges; i++) {
        pPolyhedron->edges[final_count_edges] = new_edges[i];
        pEdge_to_plane_indices[final_count_edges] = new_edge_to_plane_indices[i];
        final_count_edges += 1;
    }
    pPolyhedron->count_edges = final_count_edges;

    final_count_planes = 0;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        if (!(plane_side_buffer[i] & ePlaneSide_negative)) {
            if (i != final_count_planes) {
                BrVector4Copy(&pPolyhedron->planes[final_count_planes], &pPolyhedron->planes[i]);
                for (j = 0; j < pPolyhedron->count_edges; j++) {
                    if (pEdge_to_plane_indices[j].index1 == i) {
                        pEdge_to_plane_indices[j].index1 = final_count_planes;
                    } else if (pEdge_to_plane_indices[j].index2 == i) {
                        pEdge_to_plane_indices[j].index2 = final_count_planes;
                    }
                }
            }
            final_count_planes += 1;
        }
    }
    for (i = 0; i < count_new_planes; i++) {
        BrVector4Copy(&pPolyhedron->planes[final_count_planes], &new_planes[i]);
        for (j = 0; j < pPolyhedron->count_edges; j++) {
            if (pEdge_to_plane_indices[j].index1 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index1 = final_count_planes;
            } else if (pEdge_to_plane_indices[j].index2 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index2 = final_count_planes;
            }
        }
        final_count_planes += 1;
    }
    pPolyhedron->count_planes = final_count_planes;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00420fb0
tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    tU8 new_count_points;
    tU8 i;
    int original_count_points;
    tPolyhedron_edge_indexes indices[894];
    br_vector3 original_points[300];
    int j;
    tPhysicsError error;

    if (pPolyhedron->count_points > 300) {
        pPolyhedron->count_points = 300;
    }

    original_count_points = pPolyhedron->count_points;
    memcpy(original_points, pPolyhedron->points, original_count_points * sizeof(br_vector3));
    error = ProcessTetrahedronPolyhedronCollisionShape(pPolyhedron, indices);
    if (error != ePhysicsError_Ok) {
        if (error == ePhysicsError_UnknownShapeType) {
            return error;
        }
        pPolyhedron->count_points = original_count_points;
        memcpy(pPolyhedron->points, original_points, original_count_points * sizeof(br_vector3));
        error = AddPolyhedronCollisionShapePlanes(pPolyhedron);
        if (error == ePhysicsError_Ok) {
            return ePhysicsError_WireFrameHasNoPoints;
        }
        return error;
    }
    {
        new_count_points = 4;
        for (i = 0; i < original_count_points; i++) {
            if (PolyhedronCollisionShape_AddPoint(pPolyhedron, &original_points[i], new_count_points, indices)) {
                if (new_count_points != i) {
                    BrVector3Copy(&pPolyhedron->points[new_count_points], &original_points[i]);
                }
                new_count_points++;
            }
        }
        pPolyhedron->count_points = new_count_points;

        new_count_points = 0;
        for (i = 0; i < pPolyhedron->count_points; i++) {
            for (j = 0; j < pPolyhedron->count_edges; j++) {
                if (pPolyhedron->edges[j].index1 == i || pPolyhedron->edges[j].index2 == i) {
                    if (new_count_points != i) {
                        BrVector3Copy(&pPolyhedron->points[new_count_points], &pPolyhedron->points[i]);
                        for (j = 0; j < pPolyhedron->count_edges; j++) {
                            if (pPolyhedron->edges[j].index1 == i) {
                                pPolyhedron->edges[j].index1 = new_count_points;
                            } else if (pPolyhedron->edges[j].index2 == i) {
                                pPolyhedron->edges[j].index2 = new_count_points;
                            }
                        }
                    }
                    new_count_points++;
                    break;
                }
            }
        }
        pPolyhedron->count_points = new_count_points;
        return ePhysicsError_Ok;
    }
}

#define FILLINSHAPE_ERROR(PERR) \
    do { \
        if (gPhysics_error_cb != NULL) { \
            gPhysics_error_cb(PERR, NULL); \
        } \
        exit(PERR); \
    } while (0)

// based on carpocalypse2 physics.c:1444
// FUNCTION: CARMA2_HW 0x004c5f20
void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape) {
    int i;
    tPhysicsError error;

    switch (pShape->common.type) {
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            FILLINSHAPE_ERROR(ePhysicsError_WireFrameHasNoPoints);
            return;
        }
        GetBoundsFromPointList(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        return;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            FILLINSHAPE_ERROR(ePhysicsError_PolyhedronHasNoPoints);
            return;
        }
        error = ConvexHull3D(&pShape->polyhedron.polyhedron);
        if (error != ePhysicsError_Ok) {
            if (error == ePhysicsError_WireFrameHasNoPoints) {
                pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
            } else {
                PDFatalError("This polyhedron is too wierd for me");
            }
        }
        GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        return;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            FILLINSHAPE_ERROR(4);
            return;
        }
        AddPolyhedronCollisionShapePlanes(&pShape->polyhedron.polyhedron);
        GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        return;
    case kCollisionShapeType_Sphere:
        BrVector3Copy(&pShape->common.bb.min, &pShape->sphere.sphere.center);
        BrVector3Copy(&pShape->common.bb.max, &pShape->sphere.sphere.center);
        for (i = 0; i < 3; i++) {
            pShape->common.bb.min.v[i] -= pShape->sphere.sphere.radius;
            pShape->common.bb.max.v[i] += pShape->sphere.sphere.radius;
        }
        pShape->sphere.sphere.radius_squared = pShape->sphere.sphere.radius * pShape->sphere.sphere.radius;
        return;
    case kCollisionShapeType_Box:
        return;
    default:
        FILLINSHAPE_ERROR(ePhysicsError_UnknownShapeType);
        return;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on carpocalypse2 src/carma2/common/physics.c:893
// FUNCTION: CARMA2_HW 0x004c60a0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) {
    tPhysics_shape* shape;
    tPhysics_shape* current;

    shape = pCollision_info->shape;
    if (shape == NULL) {
        return;
    }
    pCollision_info->bb1 = shape->common.bb;

    for (current = shape->common.next; current != NULL; current = current->common.next) {
        int i;

        for (i = 0; i < 3; i++) {
            pCollision_info->bb1.min.v[i] = pCollision_info->bb1.min.v[i] < current->common.bb.min.v[i]
                ? pCollision_info->bb1.min.v[i] : current->common.bb.min.v[i];
            pCollision_info->bb1.max.v[i] = pCollision_info->bb1.max.v[i] > current->common.bb.max.v[i]
                ? pCollision_info->bb1.max.v[i] : current->common.bb.max.v[i];
        }
    }
    pCollision_info->bb2 = pCollision_info->bb1;

    pCollision_info->radius_squared = MAX(pCollision_info->bb2.min.v[0] * pCollision_info->bb2.min.v[0], pCollision_info->bb2.max.v[0] * pCollision_info->bb2.max.v[0])
            + MAX(pCollision_info->bb2.min.v[1] * pCollision_info->bb2.min.v[1], pCollision_info->bb2.max.v[1] * pCollision_info->bb2.max.v[1])
            + MAX(pCollision_info->bb2.min.v[2] * pCollision_info->bb2.min.v[2], pCollision_info->bb2.max.v[2] * pCollision_info->bb2.max.v[2]);
    pCollision_info->radius = (float)sqrt(pCollision_info->radius_squared);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004da400
tPhysics_object* C2_HOOK_FASTCALL MungeSphereObject(br_model* pModel, float pWeight) {
    tPhysics_object* collision_info;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);

    collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    collision_info->shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.f, 0.f, 0.f);
    collision_info->shape->sphere.sphere.radius = (pModel->bounds.max.v[0] - pModel->bounds.min.v[0] + pModel->bounds.max.v[1] - pModel->bounds.min.v[1] + pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) / 6.f;
    FillInShape(collision_info->shape);
    collision_info->uid = gCollision_info_uid_counter;
    gCollision_info_uid_counter++;
    UpdateCollisionObject(collision_info);
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f);
    BrVector3SetFloat(&collision_info->field_0x54, 0.f, 1/-17.25f, 0.f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = gFace_num__car - 2;
    return collision_info;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
