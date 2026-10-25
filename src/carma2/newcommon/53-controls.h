#ifndef GUARD_53_CONTROLS_H
#define GUARD_53_CONTROLS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern float gMap_render_x;
extern float gMap_render_y;
extern float gMap_render_width;
extern float gMap_render_height;
extern int gMap_trans;
extern int gHeadup_map_x;
extern int gHeadup_map_y;
extern int gHeadup_map_w;
extern int gHeadup_map_h;
extern int gRecovery_voucher_count;
extern int gAllow_car_flying;
extern int gEntering_message;
extern tCamera_key_flags gCamera_key_flags;

// CompleteRace

extern void C2_HOOK_FASTCALL AbortRace(void);
// F4Key

extern void C2_HOOK_FASTCALL SetFlag(int i);

extern void C2_HOOK_FASTCALL FinishRace(int i);
extern void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void);

extern void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void);

// DoEditModeKey

extern void C2_HOOK_FASTCALL F5Key(void);

extern void C2_HOOK_FASTCALL F6Key(void);

extern void C2_HOOK_FASTCALL F7Key(void);

extern void C2_HOOK_FASTCALL F8Key(void);

extern void C2_HOOK_FASTCALL F10Key(void);

extern void C2_HOOK_FASTCALL F11Key(void);

extern void C2_HOOK_FASTCALL F12Key(void);

extern void C2_HOOK_FASTCALL NumberKey0(void);

extern void C2_HOOK_FASTCALL NumberKey1(void);

extern void C2_HOOK_FASTCALL NumberKey2(void);

extern void C2_HOOK_FASTCALL NumberKey3(void);

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

extern void C2_HOOK_FASTCALL LookLeft(void);

extern void C2_HOOK_FASTCALL LookForward(void);
extern void C2_HOOK_FASTCALL LookRight(void);

extern void C2_HOOK_FASTCALL DamageTest(void);

// ToggleMiniMap

// CheckToggles

// CarWorldOffFallenCheckThingy

// HasCarFallenOffWorld

// CheckForBeingOutOfThisWorld

// CheckHornLocal

extern void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar);

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

extern int C2_HOOK_FASTCALL CheckRecoverCost(void);

// SortOutRecover

// SetFlipUpCar

// AlignChildren

// FlipUpCar

// GetPowerup

// CheckSystemKeys

extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);
// CheckKevKeys

// BrakeInstantly

// PollCarControls

extern void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period);
extern void C2_HOOK_FASTCALL SetFlag2(int i);
extern void C2_HOOK_FASTCALL ToggleFlying(void);
// CycleInvulnerability

extern void C2_HOOK_FASTCALL ToggleTimerFreeze(void);

extern void C2_HOOK_FASTCALL EarnDosh(void);

extern void C2_HOOK_FASTCALL LoseDosh(void);

extern void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg);

// HackyMapFixForMac3dfx

extern void C2_HOOK_FASTCALL ToggleMap(void);

extern int C2_HOOK_FASTCALL GetRecoverVoucherCount(void);

extern void C2_HOOK_FASTCALL AddVouchers(int amount);

extern void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);
extern void C2_HOOK_FASTCALL CycleYonFactor(void);
// CycleSoundDetailLevel

// CycleCarSimplificationLevel

extern void C2_HOOK_FASTCALL ToggleAccessoryRendering(void);
extern void C2_HOOK_FASTCALL UserSendMessage(void);
// EnterUserMessage

// DisplayUserMessage

extern void C2_HOOK_FASTCALL InitAbuseomatic(void);

extern void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

// ChangeCameraTypeInGame

// SteelBollock

// RichardsSphere

extern void C2_HOOK_FASTCALL ToggleHeadupLevel(void);

#endif // GUARD_53_CONTROLS_H
