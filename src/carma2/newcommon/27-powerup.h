#ifndef GUARD_27_POWERUP_H
#define GUARD_27_POWERUP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tHeadup_icon gPickedup_powerups[20];
extern tSlot_info gCost_APO;
extern tSlot_info gTrade_in_value_APO;
extern tSlot_info gSubstitution_value_APO;
extern tSlot_info gPotential_substitution_value_APO;
extern int gNet_powerup_time_replacement;
extern int gPickup_respawn_min_time_ms;
extern int gPickup_respawn_max_extra_time_ms;
extern int gBlind_pedestrians;
extern int gPed_dismemberfest;
extern int gPanicking_peds;
extern float gPed_gravity_multiplier;
extern int gCount_mutant_tail_parts;
extern float gMass_mutant_tail_link;
extern float gMass_mutant_tail_ball;
extern float gMass_mine;
extern int gInventory_cycling;
extern tU32 gInventory_timeout;
extern int gInventory[20];
extern int gInventory_selected;
extern int gINT_006a0a6c;
extern int gINT_006a0a5c;
extern br_model* gModel_powerup_armor;
extern br_model* gModel_powerup_power;
extern br_model* gModel_powerup_offence;
extern br_pixelmap* gFizzle_in[3];
extern int gDancing_peds;
extern int gImmortal_peds;
extern int gPanicking_peds;
extern int gDrunk_pedestrians;
extern int gPeds_suicidal;
extern float gPed_gravity_multiplier;
extern int gInstant_handbrake;
extern float gMutant_speed;
extern tRepulse_link gRepulse_links[6];
extern const float gBounce_rate_const;
extern int gCredit_multiplier;
extern void* gDrug_trip_data;
extern int gCollision_info_uid_counter;
extern tShit_mine gShit_mines[20];

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

// GotPowerupEarwig

// AutoGoody

// GotPowerup

// DRMapFindMeAPixie

extern void C2_HOOK_FASTCALL LoadPowerups(void);

// InitRepulseEffects

extern tPhysics_object* C2_HOOK_FAKE_THISCALL MungeBoxObject(br_model* pModel, undefined4 pArg2, float pMass);

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

extern void C2_HOOK_FASTCALL InitMineShit(void);

extern void C2_HOOK_FASTCALL InitPowerups(void);

// RemoveRepulseEffect

// ResetRepulseEffects

extern void C2_HOOK_FASTCALL EmptyInventory(void);
// ResetPowerups

extern void C2_HOOK_FASTCALL CloseDownPowerUps(void);

// NewRepulseRay

// RemoveRepulsiveReferences

// MungeRepulseRays

// AdjustRepulseRay

extern void C2_HOOK_FASTCALL ResetRepulseRay(void);

extern int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x);
// DrawPowerupIcons2

// DrawKeyPowerups

extern void C2_HOOK_FASTCALL ToggleInventory(void);
// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

// CheckRespawnQueue

// MungePowerupStuff

// GotPowerupN

extern void C2_HOOK_FASTCALL GotPowerup0(void);

extern void C2_HOOK_FASTCALL GotPowerup1(void);

extern void C2_HOOK_FASTCALL GotPowerup2(void);

extern void C2_HOOK_FASTCALL GotPowerup3(void);

extern void C2_HOOK_FASTCALL GotPowerup4(void);

extern void C2_HOOK_FASTCALL GotPowerup5(void);

extern void C2_HOOK_FASTCALL GotPowerup6(void);

extern void C2_HOOK_FASTCALL GotPowerup7(void);

extern void C2_HOOK_FASTCALL GotPowerup8(void);

extern void C2_HOOK_FASTCALL GotPowerup9(void);

// GotCredits

extern int C2_HOOK_FASTCALL GainAPO2(tCar_spec* pCar, int pIndex, int pAmount);

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

extern int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar);

// SetPedSize

// SetPedHeadSize

extern int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar);

// ResetInvulnerability

extern int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar);

// DoInstantRepair

extern void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car);

// ResetPedSize

// ResetPedHeadSize

extern void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar);

// SetEngineFactor

extern int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar);

// TrashBodywork

extern int C2_HOOK_FASTCALL TakeDrugs(tPowerup* powerup, tCar_spec* car);

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

extern int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car);

// SetSuspension

extern int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar);

// ResetImmortalPeds

extern int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar);

// ResetSuicidalPeds

extern int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar);

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

extern void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar);

// PukeDrugsBackUp

// ResetOpponentsSpeed

extern void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar);

// ResetSuspension

extern void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar);

// PickAtRandom

extern int C2_HOOK_FASTCALL GotVouchers(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL MungeVouchers(tPowerup* powerup, tU32 time);

extern int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car);

// SetGhostPeds

extern void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar);

// ResetPissedPeds

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

extern void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* powerup, tCar_spec* car);

// ReceivedPowerup

// SendCurrentPowerups

// LoseAllLocalPowerups

// LoseDodgyPowerups

extern int C2_HOOK_FASTCALL GetPowerupCount(void);

extern void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition);

// ARPowerupGot

// ARPowerupLose

// SetPissed

extern void C2_HOOK_FASTCALL ResetPissed(tPowerup* powerup, tCar_spec* car);

// PissedEffect

extern void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen);
extern void C2_HOOK_CDECL RenderChangingPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen);
extern void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity);
// DuplicateModel

// SetChangingPowerup

// PedValium

// PowerupCancel

// GrowATail

// CutOffTail

extern void C2_HOOK_FASTCALL RemoveTail(void);

// DoInitialTail

// UndoInitialTail

// MayQueuePowerupRespawn

// ARDoPowerupRespawn

// ARUndoPowerupRespawn

extern void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime);

extern br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture);

extern void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void);

extern void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void);

extern void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void);

extern void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void);

extern int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar);

// PeriodicCloaking

#endif // GUARD_27_POWERUP_H
