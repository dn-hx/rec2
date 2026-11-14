#include "46-win95sys.h"

#include "08-loading1.h"
#include "platform.h"
#include "08-loading1.h"

#ifdef REC2_MATCHING
#include <windows.h>
#endif

// FUN_100bf5a4

// AsmDoDeltaX

// DoDeltaX

// FUN_100bf768

// main

// CentreRect

// GetCentredDialog

// CentredStopAlert

// CentredCautionAlert

// GetDRect

// BoldDefault

// BoldDefault1

// PDFatalError

// PrefsError

// InitializePreferences

// MungePrefs

// WriteOutPrefs

// EncryptPassword

// GoDecideOnPassword

// AskForPassword

// DoPassword

// PDInitialiseSystem

// PDShutDownSystem

// PDSaveOriginalPalette

// SafeSetEntries

// PDRevertPalette

// PDInitScreenVars

// PDInitScreen

// PDAllocateScreenAndBack

// DoubleToScreen

// PDScreenBufferSwap

// DoWarningMessage

// DoErrorMessage

// PDInstallErrorHandlers

// strinsert

// HCalcPathName

// PDSetFileVariables

// PDBuildAppPath

// PDForEveryFile

// PDSetPaletteEntries

// PDSetPalette

// KeyDown2

// PDSetKeyArray

// PDGetASCIIFromKey

// PDMouseButtons

// PDMouseDoubleClickedAt

// PDGetMousePosition

// PDGetTotalTime

// PDInitTimers

// PDGetMicroseconds

// PDServiceSystem

// PDEnterDebugger

// FUNCTION: CARMA2_HW 0x0051b9a0
void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** pBuffer, tU32* pSize) {

    PDReallyAllocateActionReplayBuffer();
    if (pBuffer != NULL) {
        *pBuffer = gPDActionReplayBuffer;
    }
    if (pSize != NULL) {
        *pSize = gPDActionReplayBufferSize;
    }
}

// FUNCTION: CARMA2_HW 0x0051b9d0
void C2_HOOK_FASTCALL PDDisposeActionReplayBuffer(void* pBuffer) {

}

// PDInitJoysticks

// PDReadJoySticks

// PDGetJoy1X

// PDGetJoy1Y

// PDGetJoy2X

// PDGetJoy2Y

// PDGetJoy1Button1

// PDGetJoy1Button2

// PDGetJoy1Button3

// PDGetJoy1Button4

// PDGetJoy2Button1

// PDGetJoy2Button2

// PDGetJoy2Button3

// PDGetJoy2Button4

// PDFileUnlock

// PDCheckDriveExists2

// PDDoWeLeadAnAustereExistance

// NextSmackerFrame

// SetUpDeviousSmack

// FreeDeviousSmack

// MacPlaySmackerFile

// PDFileDelete

// TwatPIX16

// PDGetCurrentTime

// PDGetCurrentDate

// PDPhysicalMemorySize

// iMacKeyboard

// ShowError

// PDNetInitialise

// MapperNotifier

#ifdef REC2_MATCHING
extern int gPerformanceCounterInitialized;
extern LARGE_INTEGER gPerformanceCounterStart;
extern LARGE_INTEGER gPerformanceCounterFrequency_ms;

#ifdef _MSC_VER
#pragma optimize("y", off)
#endif

// FUNCTION: CARMA2_HW 0x0051d900
tU32 C2_HOOK_FASTCALL PDGetTotalTime2(void) {
    if (gPerformanceCounterInitialized) {
        LARGE_INTEGER perfCountValue;
        QueryPerformanceCounter(&perfCountValue);
        return (tU32)((((LONGLONG)perfCountValue.HighPart << 32) + perfCountValue.LowPart - gPerformanceCounterStart.QuadPart) / gPerformanceCounterFrequency_ms.QuadPart);
    }
    return 0;
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif
#endif

