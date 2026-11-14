#ifndef C2_WIN32_WIN32_H
#define C2_WIN32_WIN32_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>

#include <windows.h>
#include <dinput.h>

extern int gExit_code;

extern void* gHWnd;
extern int gWindowActiveState;  // FIXME: enum (0,1) => background(d3d or glide), 2=> active

extern int gWindowMovingResizing;
extern LPDIRECTINPUTDEVICEA gDirectInputDevice;
extern char* gRenderer;
extern int gMouseLButtonDown;
extern int gMouseRButtonDown;
extern int gMouseCaptured;
extern POINT gCursorPos_LastClick;
extern POINT gPD_mouse_position;
extern int gCursorPos_LastClick_Valid;
extern int gScaleMouse;
extern int gScreenWidth;
extern int gScreenHeight;
extern FILE *gActionReplayBufferFile;

extern int gKeyboardBufferLength;
extern unsigned char gKeyboardBuffer[20];

extern int gPerformanceCounterInitialized;
extern LARGE_INTEGER gPerformanceCounterStart;
extern LARGE_INTEGER gPerformanceCounterFrequency_s;
extern LARGE_INTEGER gPerformanceCounterFrequency_ms;
extern LARGE_INTEGER gPerformanceCounterFrequency_us;

extern tU32 gTime_app_activated;

extern HINSTANCE gHInstance;

extern char gFatalErrorMessage[512];
extern int gIsFatalError;

extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);

extern LRESULT CALLBACK Carma2MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern int C2_HOOK_FASTCALL PDFileUnlock(const char* pThe_path);

extern void C2_HOOK_FASTCALL PDRevertPalette(void);

extern void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown);

extern void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);

extern void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath);

extern void C2_HOOK_FASTCALL PDPageInMemory(void* pMemory);

extern void C2_HOOK_FASTCALL PDPageInProcessMemory(void);

extern void C2_HOOK_FASTCALL PDGetCurrentTime(char* pStr);

extern void C2_HOOK_FASTCALL PDGetCurrentDate(char* pStr);

extern void C2_HOOK_FASTCALL PDFileDelete(const char* pPath, int pIgnore_read_only);

extern void C2_HOOK_FASTCALL PDCloseReplayFile(void);

extern int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2);

extern int C2_HOOK_FASTCALL PDmemicmp(const char* str1, const char* str2, size_t count);


extern void C2_HOOK_FASTCALL Win32AllocateActionReplayBuffer(void);
extern void C2_HOOK_FASTCALL PDLockRealBackScreen(void);
extern int C2_HOOK_FASTCALL PDGetASCIIFromKey(int pKey);
extern tU32 C2_HOOK_FASTCALL GetLastTickCount(void);
extern int C2_HOOK_CDECL PDMissingMaterial(const char* pName);
extern void C2_HOOK_FASTCALL PDPixelmapToScreenRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h);
extern void C2_HOOK_FASTCALL PDConvertMousePosition(LPPOINT pPoint, LONG* pX, LONG* pY);
extern int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size);

extern int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

#endif // C2_WIN32_WIN32_H
