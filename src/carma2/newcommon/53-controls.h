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

extern void C2_HOOK_FASTCALL NumberKey4(void);

extern void C2_HOOK_FASTCALL NumberKey5(void);

extern void C2_HOOK_FASTCALL NumberKey6(void);

extern void C2_HOOK_FASTCALL NumberKey7(void);

extern void C2_HOOK_FASTCALL NumberKey8(void);

extern void C2_HOOK_FASTCALL NumberKey9(void);

extern void C2_HOOK_FASTCALL LookLeft(void);

extern void C2_HOOK_FASTCALL LookForward(void);
extern void C2_HOOK_FASTCALL LookRight(void);

extern void C2_HOOK_FASTCALL DamageTest(void);

extern void C2_HOOK_FASTCALL ToggleMiniMap(void);
// CheckToggles

extern int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around);
extern int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar);

// CheckForBeingOutOfThisWorld

// CheckHornLocal

extern void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar);

// CheckHorns

// SetQuickRecovery

extern void C2_HOOK_FASTCALL SetRecovery(void);

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

extern void C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime);
// TradeInAPO

extern void C2_HOOK_FASTCALL CheckOtherRacingKeys(void);
extern int C2_HOOK_FASTCALL CheckRecoverCost(void);

// SortOutRecover

extern void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar);
// AlignChildren

extern void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL GetPowerup(int pNum);

extern void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing);
extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);
extern void C2_HOOK_FASTCALL CheckKevKeys(void);
// BrakeInstantly

extern void C2_HOOK_FASTCALL PollCarControls(void);

extern void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period);
extern void C2_HOOK_FASTCALL SetFlag2(int i);
extern void C2_HOOK_FASTCALL ToggleFlying(void);
extern void C2_HOOK_FASTCALL CycleInvulnerability(void);
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
extern void C2_HOOK_FASTCALL CycleSoundDetailLevel(void);
extern void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void);
extern void C2_HOOK_FASTCALL ToggleAccessoryRendering(void);
extern void C2_HOOK_FASTCALL UserSendMessage(void);
extern void C2_HOOK_FASTCALL EnterUserMessage(void);

extern void C2_HOOK_FASTCALL DisplayUserMessage(void);
extern void C2_HOOK_FASTCALL InitAbuseomatic(void);

extern void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

extern void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void);
// SteelBollock

extern void C2_HOOK_FASTCALL RichardsSphere(int pNum);

extern void C2_HOOK_FASTCALL ToggleHeadupLevel(void);


extern void C2_HOOK_FASTCALL FUN_00511210(void);
extern void C2_HOOK_FASTCALL FUN_00511200(void);
extern void C2_HOOK_FASTCALL ToggleJoystickHeadup(void);
extern void C2_HOOK_FASTCALL F4Key(void);
extern void C2_HOOK_FASTCALL TDamageEngine(void);
extern void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
extern float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection);
extern void C2_HOOK_FASTCALL LeftInventory(void);
extern void C2_HOOK_FASTCALL RightInventory(void);
extern void C2_HOOK_FASTCALL ToggleShellModel(void);
extern void C2_HOOK_FASTCALL FlipUpCollisionInfo(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);
extern void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);
extern void C2_HOOK_FASTCALL FUN_0045c240(void);
extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3);
extern int C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2);
extern void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);
extern void C2_HOOK_FASTCALL FUN_0045a670(void);
extern void C2_HOOK_FASTCALL FUN_0045acf0(void);
extern void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount);

#endif // GUARD_53_CONTROLS_H
