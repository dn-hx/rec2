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
#include <TextUtils.h>

#include <QTML.h>
#include <Movies.h>

#include "c2_string.h"
#include "18-graphics2.h"

// GLOBAL: CARMA2_HW 0x006a0c24
// GLOBAL: CARMA2_SW 0x0069ac7c
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a0c28
int gRecording_paused;


// GLOBAL: CARMA2_HW 0x006a0c2c
int gQuicktime_movie_counter;

// GLOBAL: CARMA2_HW 0x006a0c30
Movie gQuicktime_movie;

// GLOBAL: CARMA2_HW 0x006a0c34
Handle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c38
void* gQuicktime_compressed_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c3c
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
Handle gQuicktime_image_description_handle;

// GLOBAL: CARMA2_HW 0x006a0c44
br_uint_16* gQuicktime_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c48
br_uint_16* gQuicktime_frame_pixel_conversion_table;

// GLOBAL: CARMA2_HW 0x006a0c4c
int gQuickTime_compressor_code;

// GLOBAL: CARMA2_HW 0x006a0c50
// GLOBAL: CARMA2_SW 0x0069aca8
int gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a0c54
tU32 gRecording_time;

// GLOBAL: CARMA2_HW 0x006a23b8
// GLOBAL: CARMA2_SW 0x0069c410
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
// GLOBAL: CARMA2_SW 0x0069c414
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
// GLOBAL: CARMA2_SW 0x0069c418
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x006baa28
int gRecording_mouse_disabled;

// GLOBAL: CARMA2_HW 0x00705440
// GLOBAL: CARMA2_SW 0x006fbf00
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x0057a370
void* gUNK_0057a370;

// GLOBAL: CARMA2_HW 0x0057a3d0
void* gUNK_0057a3d0;

// GLOBAL: CARMA2_HW 0x0057a440
void* gUNK_0057a440;

// GLOBAL: CARMA2_HW 0x0057a460
void* gUNK_0057a460;

// GLOBAL: CARMA2_HW 0x0057a810
void* gUNK_0057a810;

// GLOBAL: CARMA2_HW 0x00705240
// GLOBAL: CARMA2_SW 0x006fbd00
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
// GLOBAL: CARMA2_SW 0x006fbe00
char gQuick_time_banner_texture_name[256];

// GLOBAL: CARMA2_HW 0x0065ee50
char gDefault_quick_time_quality[] = "normal";

// GLOBAL: CARMA2_HW 0x0065ef28
int gQuickTime_quality_code;

// GLOBAL: CARMA2_HW 0x0065ef2c
int gQuicktime_frame_flags;

// GLOBAL: CARMA2_HW 0x0065ef34
br_uint_16 gQuickTime_pixel_depth;

// GLOBAL: CARMA2_HW 0x006736a8
void* gQuickTime_dispatcher;

// GLOBAL: CARMA2_HW 0x006736b0
void* gQuickTime_module;

// GLOBAL: CARMA2_HW 0x006736b4
void* gQuickTime_dispatch;

// GLOBAL: CARMA2_HW 0x006736b8
void* gQuickTime_call_component;

// GLOBAL: CARMA2_HW 0x006736bc
void* gQuickTime_call_component_with_storage;

// GLOBAL: CARMA2_HW 0x00673cf8
void* PTR_FUN_00673cf8;

// GLOBAL: CARMA2_HW 0x00673cfc
void* PTR_FUN_00673cfc;

// GLOBAL: CARMA2_HW 0x00673d04
void* PTR_FUN_00673d04;

// GLOBAL: CARMA2_HW 0x00673d08
void* PTR_FUN_00673d08;

// GLOBAL: CARMA2_HW 0x00673d0c
void* PTR_FUN_00673d0c;

// GLOBAL: CARMA2_HW 0x006a0af0
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0af4
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0af8
int gQuicktime_max_compression_size;

// GLOBAL: CARMA2_HW 0x006a0afc
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0b08
FSSpec gQt_file_spec;

// GLOBAL: CARMA2_HW 0x006a0c10
Track gQuicktime_track;
// GLOBAL: CARMA2_HW 0x006a0c18
struct Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x0065ed30
// GLOBAL: CARMA2_SW 0x00659690
char gDefault_quick_time_compressor[] = "animation";

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

// STUB: CARMA2_HW 0x004e1250
int C2_HOOK_FASTCALL WriteBannerFrame(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e1400
void C2_HOOK_FASTCALL Convert565BrPixelmapTo555BigEndianPixmap(void* pFrame_pixmap_handle, br_pixelmap* pScreen) {
    NOT_IMPLEMENTED();
}

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

// STUB: CARMA2_HW 0x004e0f20
int C2_HOOK_FASTCALL InitMovie(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004e0fe0
short C2_HOOK_FASTCALL CreateMovie(void) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x004e1810
void C2_HOOK_FASTCALL MovieRecordButtonPressed(void) {
    NOT_IMPLEMENTED();
}

// MovieRecordButtonPressed

// STUB: CARMA2_HW 0x004e1a20
// STUB: CARMA2_SW 0x004ddd20
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e1c30
// FUNCTION: CARMA2_SW 0x004ddf30
void C2_HOOK_FASTCALL FUN_004e1c30(void) {
    /* Unused */
}

// STUB: CARMA2_HW 0x004e1c70
short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    NOT_IMPLEMENTED();
}

#define QT_CloseMovieFile ((OSErr (C2_HOOK_CDECL *)(short))CloseMovieFile)
// FUNCTION: CARMA2_HW 0x004e1c40
// FUNCTION: CARMA2_SW 0x004ddf40
void C2_HOOK_FASTCALL FUN_004e1c40(void) {
    /* Unused */
}

// FUNCTION: CARMA2_HW 0x004e1c50
// FUNCTION: CARMA2_SW 0x004ddf50
void C2_HOOK_FASTCALL FUN_004e1c50(void) {
    /* Unused */
}

// FUNCTION: CARMA2_HW 0x004e1c60
// FUNCTION: CARMA2_SW 0x004ddf60
void C2_HOOK_FASTCALL FUN_004e1c60(undefined4 pArg1, undefined4 pArg2, undefined4 pArg3) {
    /* Unused */
}

// GenerateOneMovieFrame

// STUB: CARMA2_HW 0x004e1da0
int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap* pScreen, int pFrame_period) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x0056a900
int C2_HOOK_CDECL ExitMovies(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056a920
int C2_HOOK_CDECL GetMoviesError(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056abe0
void C2_HOOK_CDECL FUN_0056abe0(void* pMovie) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056b240
int C2_HOOK_CDECL FUN_0056b240(void* pMedia) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056b340
int C2_HOOK_CDECL BeginMediaEdits(void* pMedia) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056b360
short C2_HOOK_CDECL FUN_0056b360(void* pMedia) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056b440
short C2_HOOK_CDECL AddMediaSample(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056baa0
short C2_HOOK_CDECL FUN_0056baa0(short pRes_ref_num) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056c600
short C2_HOOK_CDECL FUN_0056c600(void* pTrack, int pTrack_start, int pMedia_time, int pMedia_duration, int pMedia_rate) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056c640
void* C2_HOOK_CDECL NewMovieTrack(void* pMovie, void* pWidth, void* pHeight, int pVolume) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056c6e0
void* C2_HOOK_CDECL NewTrackMedia(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056c720
short C2_HOOK_CDECL FUN_0056c720(void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags, short* pRes_ref_num, void** pMovie) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056c800
void* C2_HOOK_CDECL FUN_0056c800(void* pMovie, int pFlatten_flags, const void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056d7e0
int C2_HOOK_CDECL GetMaxCompressionSize(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056d8a0
int C2_HOOK_CDECL CompressSequenceBegin(void* pImage_sequence, void* pFrame_pixmap_handle, int pFrame_number, void* pFrame_rect1, void* pFrame_rect2, br_uint_16 pPixel_depth, int pCompressor, int pColor_table_id, int pSpatial_quality, int pTemporal_quality, int pFrame_type, int pUnused12, int pFlags, void* pImage_description_handle) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056d8c0
short C2_HOOK_CDECL CompressSequenceFrame(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056dac0
short C2_HOOK_CDECL FUN_0056dac0(void* pSequence) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056e760
void C2_HOOK_CDECL QTMLInitInternals(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0056fde0
void* C2_HOOK_CDECL FixRatio(int pWidth, int pHeight) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x005705c0
int C2_HOOK_CDECL MemError(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00570600
void* C2_HOOK_CDECL NewHandle(int pSize) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x005707e0
void C2_HOOK_CDECL HLock(void* pHandle) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00570a40
void C2_HOOK_CDECL MoveHHi(void* pHandle) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00570ac0
void C2_HOOK_CDECL FUN_00570ac0(void* pHandle) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00575340
int C2_HOOK_CDECL FUN_00575340(char* pString) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00575730
void C2_HOOK_CDECL autoPatchThoseAddresses(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x005757d0
void* C2_HOOK_CDECL _QTLoadLibrary(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00575b30
void C2_HOOK_CDECL FUN_00575b30(void) {
    NOT_IMPLEMENTED();
}

// TrapS3SoundOutput

// FreeS3SoundOutput

