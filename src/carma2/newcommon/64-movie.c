/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "64-movie.h"

#include "16-graphics1.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "globvars.h"
#include "platform.h"

#include <QTML.h>
#include <Movies.h>

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006736a8
void* gQuickTime_dispatcher;

// GLOBAL: CARMA2_HW 0x006736b4
void* gQuickTime_dispatch;

// GLOBAL: CARMA2_HW 0x006a0c24
// GLOBAL: CARMA2_SW 0x0069ac7c
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a0c50
// GLOBAL: CARMA2_SW 0x0069aca8
int gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a23b8
// GLOBAL: CARMA2_SW 0x0069c410
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
// GLOBAL: CARMA2_SW 0x0069c414
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
// GLOBAL: CARMA2_SW 0x0069c418
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x00705440
// GLOBAL: CARMA2_SW 0x006fbf00
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
// GLOBAL: CARMA2_SW 0x006fbd00
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
// GLOBAL: CARMA2_SW 0x006fbe00
char gQuick_time_banner_texture_name[256];

// GLOBAL: CARMA2_HW 0x0065ee50
char gDefault_quick_time_quality[] = "normal";

// GLOBAL: CARMA2_HW 0x0065ed30
// GLOBAL: CARMA2_SW 0x00659690
char gDefault_quick_time_compressor[] = "animation";

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

// WriteBannerFrame

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

// CreateMovie

// MovieStartNewMovie

// MovieStopRecordingAndSave

// FreeOffQTshite

// FUNCTION: CARMA2_HW 0x004e1700
// FUNCTION: CARMA2_SW 0x004dda00
void C2_HOOK_CDECL InitQuickTimeStuff(void) {
    OSErr err;

    gQuickTime_initialized = 0;
    err = InitializeQTML(0);
    if (err == noErr) {
        err = EnterMovies();
        if (err == noErr) {
            gQuickTime_initialized = 1;
        }
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e1740
// FUNCTION: CARMA2_SW 0x004dda40
void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (gQuick_time_quality[0] == '\0') {
        strcpy(gQuick_time_quality, gDefault_quick_time_quality);
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, gDefault_quick_time_compressor);
    }
    if (gQuick_time_temp_path[0] == '\0') {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    }
    if (gQuick_time_movie_path_stub[0] == '\0') {
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    }
    if (gQuick_time_banner_texture_name[0] == '\0') {
        strcpy(gQuick_time_banner_texture_name, "ARBANNER.PIX");
    }
}

// MovieRecordButtonPressed

// STUB: CARMA2_HW 0x004e1a20
// STUB: CARMA2_SW 0x004ddd20
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {
    NOT_IMPLEMENTED();
}

// GenerateOneMovieFrame

// MovieRecordFrame

// FUNCTION: CARMA2_HW 0x004e2100
// FUNCTION: CARMA2_SW 0x004de480
int C2_HOOK_FASTCALL MovieRecording(void) {
    return gNext_replay_record_state;
}

// FUNCTION: CARMA2_HW 0x004e2110
// FUNCTION: CARMA2_SW 0x004de490
void C2_HOOK_FASTCALL KillStatusMessage(void) {

    gShow_status_message = 0;
}

// FUNCTION: CARMA2_HW 0x004e2120
// FUNCTION: CARMA2_SW 0x004de4a0
void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2) {

    gShow_status_message = 1;
    gStatus_message_1 = pMessage1;
    gStatus_message_2 = pMessage2;
}

// FUNCTION: CARMA2_HW 0x004e2140
// FUNCTION: CARMA2_SW 0x004de4c0
int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
// FUNCTION: CARMA2_SW 0x004de4e0
int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e2190
// FUNCTION: CARMA2_SW 0x004de510
void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    int old_state;

    old_state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = old_state;
}

// TrapS3SoundOutput

// FreeS3SoundOutput


// FUNCTION: CARMA2_HW 0x0056a900
__declspec(naked) int C2_HOOK_CDECL FUN_0056a900(void) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_a900
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_a900:
        mov     eax, 0x20002
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056a920
__declspec(naked) int C2_HOOK_CDECL FUN_0056a920(void) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_a920
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_a920:
        mov     eax, 0x20003
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056b340
__declspec(naked) int C2_HOOK_CDECL FUN_0056b340(void* pMedia) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_b340
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_b340:
        mov     eax, 0x20072
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056b440
__declspec(naked) short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_b440
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_b440:
        mov     eax, 0x2007c
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056c640
__declspec(naked) void* C2_HOOK_CDECL FUN_0056c640(void* pMovie, void* pCompressor) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c640
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c640:
        mov     eax, 0x20188
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056c6e0
__declspec(naked) void* C2_HOOK_CDECL FUN_0056c6e0(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c6e0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c6e0:
        mov     eax, 0x2018e
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056d7e0
__declspec(naked) int C2_HOOK_CDECL FUN_0056d7e0(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_d7e0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_d7e0:
        mov     eax, 0x30004
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056d8a0
__declspec(naked) int C2_HOOK_CDECL FUN_0056d8a0(void* pImage_sequence, void* pFrame_pixmap_handle, int pFrame_number, void* pFrame_rect1, void* pFrame_rect2, br_uint_16 pPixel_depth, int pCompressor, int pColor_table_id, int pSpatial_quality, int pTemporal_quality, int pFrame_type, int pUnused12, int pFlags, void* pImage_description_handle) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_d8a0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_d8a0:
        mov     eax, 0x3000a
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056d8c0
__declspec(naked) short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_d8c0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_d8c0:
        mov     eax, 0x3000b
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056fde0
__declspec(naked) void* C2_HOOK_CDECL FUN_0056fde0(int pWidth, int pHeight, void* pCompressor) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_fde0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_fde0:
        mov     eax, 0x100001
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x005705c0
__declspec(naked) int C2_HOOK_CDECL FUN_005705c0(void) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_05c0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_05c0:
        mov     eax, 0x150006
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x00570600
__declspec(naked) void* C2_HOOK_CDECL FUN_00570600(int pSize) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_0600
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_0600:
        mov     eax, 0x150008
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x005707e0
__declspec(naked) void C2_HOOK_CDECL FUN_005707e0(void* pHandle) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_07e0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_07e0:
        mov     eax, 0x150019
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x00570a40
__declspec(naked) void C2_HOOK_CDECL FUN_00570a40(void* pHandle) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_0a40
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_0a40:
        mov     eax, 0x15002e
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056abe0
__declspec(naked) void C2_HOOK_CDECL FUN_0056abe0(void* pMovie) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_abe0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_abe0:
        mov     eax, 0x20023
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056b240
__declspec(naked) int C2_HOOK_CDECL FUN_0056b240(void* pMedia) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_b240
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_b240:
        mov     eax, 0x2006a
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056b360
__declspec(naked) short C2_HOOK_CDECL FUN_0056b360(void* pMedia) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_b360
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_b360:
        mov     eax, 0x20073
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056baa0
__declspec(naked) short C2_HOOK_CDECL FUN_0056baa0(short pRes_ref_num) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_baa0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_baa0:
        mov     eax, 0x200d5
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056c600
__declspec(naked) short C2_HOOK_CDECL FUN_0056c600(void* pTrack, int pTrack_start, int pMedia_time, int pMedia_duration, int pMedia_rate) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c600
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c600:
        mov     eax, 0x20183
        jmp     dword ptr [gQuickTime_dispatch]
    }
}


// FUNCTION: CARMA2_HW 0x0056c720
__declspec(naked) short C2_HOOK_CDECL FUN_0056c720(void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags, short* pRes_ref_num, void** pMovie) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c720
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c720:
        mov     eax, 0x20191
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
