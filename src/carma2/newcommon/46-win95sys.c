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

// FUNCTION: CARMA2_HW 0x004625d0
// FUNCTION: CARMA2_SW 0x00462060
void C2_HOOK_FASTCALL DoRunLengthX(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int k;
    int number_of_packets;
    int size_count;
    int the_row_bytes;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;

    (void)pChunk_length;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            size_count = MemReadS8(&pFlic_info->data);
            if (size_count < 0) {
                for (k = 0; k < -size_count; k++) {
                    *line_pixel_ptr = MemReadU8(&pFlic_info->data);
                    line_pixel_ptr++;
                }
            } else {
                the_byte = MemReadU8(&pFlic_info->data);
                for (k = 0; k < size_count; k++) {
                    *line_pixel_ptr = the_byte;
                    line_pixel_ptr++;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

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

// PDShutdownSystem

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
void C2_HOOK_FASTCALL FUN_DisposeReplayBuffer(void* pBuffer) {

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
tU32 C2_HOOK_FASTCALL PDGetElapsedMilliseconds(void) {
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

