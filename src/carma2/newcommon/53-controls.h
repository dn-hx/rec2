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

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

extern void C2_HOOK_FASTCALL F12Key(void);

// NumberKey0

// NumberKey1

// NumberKey2

// NumberKey3

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

extern void C2_HOOK_FASTCALL LookLeft(void);

// LookForward

// LookRight

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

// CheckRecoverCost

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
// CycleInvulnerability

extern void C2_HOOK_FASTCALL ToggleTimerFreeze(void);

extern void C2_HOOK_FASTCALL EarnDosh(void);

extern void C2_HOOK_FASTCALL LoseDosh(void);

extern void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg);

// HackyMapFixForMac3dfx

// ToggleMap

// GetRecoverVoucherCount

// AddVouchers

// ResetRecoveryVouchers

// CycleYonFactor

// CycleSoundDetailLevel

// CycleCarSimplificationLevel

extern void C2_HOOK_FASTCALL ToggleAccessoryRendering(void);

// UserSendMessage

// EnterUserMessage

// DisplayUserMessage

extern void C2_HOOK_FASTCALL InitAbuseomatic(void);

extern void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

// ChangeCameraTypeInGame

// SteelBollock

// RichardsSphere

// ToggleHeadupLevel

#endif // GUARD_53_CONTROLS_H
