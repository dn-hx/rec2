#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

#include "rec2_types.h"

/* DirectInput 5 type subset, declared locally: the DirectX SDK include directory
   is only visible to the platform target, not to newcommon.  Layouts match the
   SDK headers (DIEFFECT is 0x34 bytes, matching tJoystick_force_effect in
   win32_types.h, whose name field sits at 0x68). */
typedef struct {
    tU32 dwSize;
    tU32 dwFlags;
    tU32 dwDuration;
    tU32 dwSamplePeriod;
    tU32 dwGain;
    tU32 dwTriggerButton;
    tU32 dwTriggerRepeatInterval;
    tU32 cAxes;
    tU32* rgdwAxes;
    tS32* rglDirection;
    void* lpEnvelope;
    tU32 cbTypeSpecificParams;
    void* lpvTypeSpecificParams;
} tDIEFFECT;

typedef struct {
    tU32 Data1;
    tU16 Data2;
    tU16 Data3;
    tU8 Data4[8];
} tGUID;

typedef struct tJoystick_effect_spec {
    tGUID guid;
    const tDIEFFECT* effect;
} tJoystick_effect_spec;

extern int gJoystick_index;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;
extern int gINT_00595f84;
extern int gINT_00596310;
extern int gJoystick_preferences_backup_valid;
extern tButtonJoystickInfo gJoystick_preferences_backup;

extern int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name);

extern int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo);

extern void C2_HOOK_FASTCALL EnableJoysticks(void);

extern void C2_HOOK_FASTCALL DisableJoysticks(void);

extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);
extern int C2_HOOK_FASTCALL SaveJoystickPreferences(void);
extern void C2_HOOK_FASTCALL BackupJoystickPreferences(void);
extern void C2_HOOK_FASTCALL RestoreJoystickPreferences(void);

extern int C2_HOOK_FASTCALL HasCurrentJoystick(void);

extern int C2_HOOK_FASTCALL ReadUntilCharacter(FILE* pF, char* pBuffer, int pBuffer_size, char pLimit_char);

extern int C2_HOOK_FASTCALL PeekCharacter(FILE* pFile);

extern unsigned int C2_HOOK_FASTCALL Carma2_GetKeycode_For_Action(int pKey);

extern int C2_HOOK_FASTCALL WriteJoystickPreferences(FILE* pF, const tButtonJoystickInfo* pInfo);

extern int C2_HOOK_FASTCALL CountJoystickPreferences(FILE* pF);

extern int C2_HOOK_FASTCALL ToggleJoystickThrottle(void);

extern void C2_HOOK_FASTCALL SetJoystickXAxis(float pValue);

extern void C2_HOOK_FASTCALL SetJoystickYAxis(float pValue);

extern void C2_HOOK_FASTCALL SetJoystickDPadEnable(int pEnabled);

extern int C2_HOOK_FASTCALL JoystickAddFFBGain(void);

extern void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue);

extern int C2_HOOK_FASTCALL GetJoystickFBBGain(void);

extern void C2_HOOK_FASTCALL SetJoystickX(float pValue);

extern void C2_HOOK_FASTCALL SetJoystickY(float pValue);

extern float C2_HOOK_FASTCALL GetJoystickX(void);

extern float C2_HOOK_FASTCALL GetJoystickY(void);

extern void C2_HOOK_FASTCALL ToggleJoystickYAxis(void);

extern void C2_HOOK_FASTCALL ToggleJoystickEnable(void);

extern void C2_HOOK_FASTCALL ToggleJoystickDPad(void);

extern void C2_HOOK_FASTCALL IncreaseJoystickFFB(void);

extern int C2_HOOK_FASTCALL CreateDinputEffectFromSpec(int pJoystickIndex, const tJoystick_effect_spec* pEffectSpec, const char* pName);

#endif /* REC2_JOYSTICK_H */
