#ifndef C2_WIN32_INPUT_H
#define C2_WIN32_INPUT_H

#include "c2_hooks.h"

#include "platform.h"
#include "win32_types.h"

extern int gASCII_table[151];
extern int gASCII_shift_table[151];
extern int gScan_code[256];
extern int gJoystick_deadzone;

extern void C2_HOOK_FASTCALL KeyBegin(void);

extern void C2_HOOK_FASTCALL Win32InitInputDevice(void);

extern void C2_HOOK_FASTCALL CloseDirectInput(void);

extern int C2_HOOK_FASTCALL InitForceFeedback(void);

extern void C2_HOOK_FASTCALL KeyBegin(void);

extern void C2_HOOK_FASTCALL ShowCurrentJoystickName(void);

extern void C2_HOOK_FASTCALL FUN_00459a70(void);

extern void C2_HOOK_FASTCALL StartDinputEffect(int pIndex);

extern void C2_HOOK_FASTCALL StopDinputEffect(int pIndex);

extern int C2_HOOK_FASTCALL SetDinputEffectGain(int pIndex, int pGain);

extern int C2_HOOK_FASTCALL SetJoystickFFBForce(int pMagnitude);

extern int C2_HOOK_FASTCALL SetJoystickFFBSteeringForce(int pMagnitude);

extern int C2_HOOK_FASTCALL JoystickDInputBegin(void);

extern int C2_HOOK_FASTCALL InitForceFeedback(void);

extern void C2_HOOK_FASTCALL ResetDInputJoystickFFB(int pIndex);

extern BOOL CALLBACK Win32DInputJoystickEnum(const DIDEVICEINSTANCEA* pDeviceInstance, void* pContext);

extern void C2_HOOK_FASTCALL RegisterBasicJoystickFFBEffects(void);

extern void C2_HOOK_FASTCALL Win32InitInputDevice(void);

extern int C2_HOOK_FASTCALL JoystickDInputBegin(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void);

extern void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark);

extern void C2_HOOK_FASTCALL PDReadJoySticks(void);

extern int C2_HOOK_FASTCALL GetDirectInputJoy1X(void);

extern int C2_HOOK_FASTCALL GetDirectInputJoy1Y(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy1X(void);

extern int C2_HOOK_FASTCALL PDGetJoy1Y(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy2X(void);

extern int C2_HOOK_FASTCALL PDGetJoy2Y(void);

extern void C2_HOOK_FASTCALL PDInitJoysticks(void);

extern void C2_HOOK_FASTCALL PDPlayFFBEffectIndex(int index);

extern int C2_HOOK_FASTCALL PDFindJoystickEffect(const char* effectName);

extern void C2_HOOK_FASTCALL PDPlayFFBEffect(const char* effectName);

extern int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void);

extern void C2_HOOK_FASTCALL UnloadDInputFFBEffectAtIndex(int index);

extern void C2_HOOK_FASTCALL DownloadDinputFFBEffectAtIndex(int index);


extern tDirectInputJoystickInfo* C2_HOOK_FASTCALL JoystickDInputGetInfo(int pJoystickIndex, tU32* pButtonMask, tU32* pAxisMask);
extern void C2_HOOK_FASTCALL AttachJoystickButtonInfos(size_t pSize, tWin32_void_voidptr_cbfn pCallback);
extern size_t C2_HOOK_FASTCALL GetCurrentJoystickCountButtons(void);
extern void C2_HOOK_FASTCALL CollectJoystickButtonInfo(tButtonJoystickInfo* pInfo);
extern void C2_HOOK_FASTCALL CollectJoystickButtonInfos(void);
extern int C2_HOOK_FASTCALL CreateDinputEffect(int joystick_index, tJoystick_effect_description* description, const char *name);
extern int C2_HOOK_FASTCALL ReadIforceEffectsNames(const char *path, char **names, size_t capacity);
extern int C2_HOOK_FASTCALL UnloadDinputFFBEfectwithName(const char *name);
extern void C2_HOOK_FASTCALL PDStopFFBEffect(const char* effectName);
extern tButtonJoystickInfo* C2_HOOK_FASTCALL AllocateCurrentJoystickInfoData(size_t pSize);
extern void C2_HOOK_FASTCALL DisposeCurrentJoystickData(void);
extern int* C2_HOOK_FASTCALL GetJoystickEffect(int index);
extern int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(int pMagnitude, int pDirection);

#endif // C2_WIN32_INPUT_H
