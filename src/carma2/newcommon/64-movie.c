#include "64-movie.h"

#include "25-grafdata.h"
#include "41-utility.h"
#include "globvars.h"
#include "platform.h"

#include <QTML.h>
#include <Movies.h>
#include <TextUtils.h>

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0c24
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a0c50
// GLOBAL: CARMA2_SW 0x0069aca8
int gNext_replay_record_state;

// GLOBAL: CARMA2_SW 0x0069c410
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x00705440
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
char gQuick_time_banner_texture_name[256];

// Byte image of the original option tables: 8 (resp. 6) entries of
// { char name[32]; unsigned int code; }. The name at offset 0 is also the
// default name used by SetQuickTimeDefaults.
// GLOBAL: CARMA2_HW 0x0065ed30
char gDefault_quick_time_compressor[0x120] =
    /* animation    0x726c6520 */
    "animation\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\040\145\154\162"
    /* video        0x72707a61 */
    "video\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\141\172\160\162"
    /* jpeg         0x6a706567 */
    "jpeg\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\147\145\160\152"
    /* raw          0x72617720 */
    "raw\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\040\167\141\162"
    /* flic         0x666c6963 */
    "flic\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\143\151\154\146"
    /* gif          0x67696620 */
    "gif\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\040\146\151\147"
    /* cinepak      0x63766964 */
    "cinepak\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\144\151\166\143"
    /* compactvideo 0x63647663 */
    "compactvideo\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\143\166\144\143";

// GLOBAL: CARMA2_HW 0x0065ee50
char gDefault_quick_time_quality[0xd8] =
    /* normal       0x00000200 */
    "normal\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\000\002\000\000"
    /* min          0x00000000 */
    "min\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\000\000\000\000"
    /* low          0x00000100 */
    "low\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\000\001\000\000"
    /* high         0x00000300 */
    "high\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\000\003\000\000"
    /* max          0x000003ff */
    "max\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\377\003\000\000"
    /* lossless     0x00000400 */
    "lossless\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""\000\004\000\000";

// GLOBAL: CARMA2_HW 0x0065ef28
int gQuickTime_quality_code = -1;

// GLOBAL: CARMA2_HW 0x006a23b8
int gShow_status_message;

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

// WriteBannerFrame

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

extern short gQuicktime_movie_file;
extern Movie gQuicktime_movie;
extern FSSpec gQt_file_spec;
extern struct Rect gQuicktime_rect;

// FUNCTION: CARMA2_HW 0x004e0f20
int C2_HOOK_FASTCALL InitMovie(void) {
    tGraf_data* pGraf;
    int bits;
    int w;
    OSErr err;

    pGraf = gCurrent_graf_data;
    bits = gNbPixelBits;
    gQuicktime_rect.top = 0;
    gQuicktime_rect.left = 0;
    w = pGraf->width;
    if (bits == 16) {
        w = w / 2;
    }
    gQuicktime_rect.right = w;
    w = pGraf->height;
    if (bits == 16) {
        w = w / 2;
    }
    gQuicktime_rect.bottom = w;
    err = ((OSErr (C2_HOOK_CDECL *)(const FSSpec*, OSType, ScriptCode, long, short*, Movie*))CreateMovieFile)(&gQt_file_spec, 0x54564f44, -2, 0x80000000, &gQuicktime_movie_file, &gQuicktime_movie);
    if (err == 0) {
        return err;
    }
    PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    return ((OSErr (C2_HOOK_CDECL *)(const FSSpec*, OSType, ScriptCode, long, short*, Movie*))CreateMovieFile)(&gQt_file_spec, 0x54564f44, -2, 0x80000000, &gQuicktime_movie_file, &gQuicktime_movie);
}

// CreateMovie

// MovieStartNewMovie

// MovieStopRecordingAndSave

// FreeOffQTshite

// FUNCTION: CARMA2_HW 0x004e1700
void C2_HOOK_CDECL InitQuickTimeStuff(void) {

    gQuickTime_initialized = 0;
    if (InitializeQTML(0) == noErr && EnterMovies() == noErr) {//
        gQuickTime_initialized = 1;
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e1740
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

typedef struct {
    char name[32];
    unsigned int code;
} tQuickTime_name_code;

extern tS3_outlet* gEffects_outlet;
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern int gStatus_message_1;
extern int gStatus_message_2;
extern int gRecording_paused;
extern int gQuickTime_quality_code;
extern int gQuickTime_compressor_code;
extern short gQuicktime_movie_file;
extern FSSpec gQt_file_spec;
extern int C2_HOOK_CDECL FUN_00575340(char* pString);

#pragma pack(push, 1)
typedef struct {
    short vRefNum;
    long parID;
    char name[64];
} tMoviePackedFSSpec;
#pragma pack(pop)

// PARTIAL 99.32: single diff line is the folded compressor-code operand (orig 0x65ed50 renders as STRING " elrvideo" from carma2_hw-ascii.csv, ours as OFFSET gDefault_quick_time_compressor[32]); instruction bytes identical.
// FUNCTION: CARMA2_HW 0x004e1810
void C2_HOOK_FASTCALL MovieRecordButtonPressed(void) {
    const tQuickTime_name_code* pOption;
    int i;

    if (gQuickTime_initialized != 0) {
        if (gNext_replay_record_state == 4) {
            gRecording_paused = gRecording_paused == 0;
            if (gRecording_paused != 0) {
                DRS3StartSound(gEffects_outlet, 0x232b);
                DRS3StartSound(gEffects_outlet, 0x157c);
                gShow_status_message = 1;
                gStatus_message_1 = 0x119;
                gStatus_message_2 = 0;
                return;
            }
            DRS3StartSound(gEffects_outlet, 0x2333);
            gShow_status_message = 1;
            gStatus_message_1 = 0x117;
            gStatus_message_2 = 0;
            return;
        }
        if (gNext_replay_record_state == 0) {
            gStatus_message_1 = 0x116;
            gNext_replay_record_state = 1;
            gShow_status_message = 1;
            gStatus_message_2 = 0x118;
            gQuicktime_movie_file = 0;
            gRecording_paused = 0;
            if (gQuickTime_quality_code == -1) {
                for (i = 0, pOption = (const tQuickTime_name_code*)gDefault_quick_time_quality;
                    pOption < (const tQuickTime_name_code*)gDefault_quick_time_quality + 6;
                    i++, pOption++) {
                    if (strcmp(pOption->name, gQuick_time_quality) == 0) {
                        goto quality_found;
                    }
                }
                gQuickTime_quality_code = 0x100;
                goto quality_continue;
            }
quality_continue:
            if (gQuickTime_compressor_code == 0) {
                for (i = 0, pOption = (const tQuickTime_name_code*)gDefault_quick_time_compressor;
                    pOption < (const tQuickTime_name_code*)gDefault_quick_time_compressor + 8;
                    i++, pOption++) {
                    if (strcmp(pOption->name, gQuick_time_compressor) == 0) {
                        goto compressor_found;
                    }
                }
                gQuickTime_compressor_code = 0x726c6520;
                goto compressor_done;
compressor_found:
                gQuickTime_compressor_code = ((const tQuickTime_name_code*)gDefault_quick_time_compressor)[i].code;
compressor_done:
                ;
            }
            ((tMoviePackedFSSpec*)&gQt_file_spec)->vRefNum = 0;
            ((tMoviePackedFSSpec*)&gQt_file_spec)->parID = 0;
            strcpy(((tMoviePackedFSSpec*)&gQt_file_spec)->name, gQuick_time_temp_path);
            FUN_00575340(((tMoviePackedFSSpec*)&gQt_file_spec)->name);
        }
    }
    return;
quality_found:
    gQuickTime_quality_code = ((const tQuickTime_name_code*)gDefault_quick_time_quality)[i].code;
    goto quality_continue;
}

#pragma pack(push, 1)
typedef struct {
    short vRefNum;
    long parID;
    char name[64];
} tPackedFSSpec;
#pragma pack(pop)

#define QT_CDSequenceEnd ((OSErr (C2_HOOK_CDECL *)(ImageSequence))CDSequenceEnd)
#define QT_EndMediaEdits ((OSErr (C2_HOOK_CDECL *)(Media))EndMediaEdits)
#define QT_GetMediaDuration ((TimeValue (C2_HOOK_CDECL *)(Media))GetMediaDuration)
#define QT_InsertMediaIntoTrack ((OSErr (C2_HOOK_CDECL *)(Track, TimeValue, TimeValue, TimeValue, Fixed))InsertMediaIntoTrack)
#define QT_CloseMovieFile ((OSErr (C2_HOOK_CDECL *)(short))CloseMovieFile)
#define QT_c2pstr ((StringPtr (C2_HOOK_CDECL *)(char*))c2pstr)
#define QT_FlattenMovieData ((Movie (C2_HOOK_CDECL *)(Movie, long, const FSSpec*, OSType, ScriptCode, long))FlattenMovieData)
#define QT_DisposeMovie ((void (C2_HOOK_CDECL *)(Movie))DisposeMovie)
#define QT_DisposeHandle ((void (C2_HOOK_CDECL *)(Handle))DisposeHandle)
#define QT_CreateMovieFile ((OSErr (C2_HOOK_CDECL *)(const FSSpec*, OSType, ScriptCode, long, short*, Movie*))CreateMovieFile)

extern short gQuicktime_movie_file;
extern Movie gQuicktime_movie;
extern Handle gQuicktime_frame_pixmap_handle;
extern Handle gQuicktime_image_description_handle;
extern Handle gQuicktime_compressed_frame_handle;
extern ImageSequence gQuicktime_image_sequence;
extern Media gQuicktime_media;
extern Track gQuicktime_track;
extern int gQuicktime_movie_counter;
extern int gStatus_message_1;
extern int gStatus_message_2;
extern int C2_HOOK_FASTCALL WriteBannerFrame(void);
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern void C2_HOOK_FASTCALL PDFileDelete(const char *pPath, int pIgnore_read_only);

// based on the disassembly (inlined MovieStopRecordingAndSave)
// FUNCTION: CARMA2_HW 0x004e1a20
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {

    if (gNext_replay_record_state == 4) {
        tPackedFSSpec fs_spec;
        TimeValue duration;
        tPath_name path;
        tPath_name path2;

        gShow_status_message = 1;
        gStatus_message_1 = 282;
        gStatus_message_2 = 280;
        gNext_replay_record_state = 20;
        RenderAFrame(1);
        RenderAFrame(1);
        gNext_replay_record_state = 5;
        WriteBannerFrame();
        gNext_replay_record_state = 0;
        QT_CDSequenceEnd(gQuicktime_image_sequence);
        QT_EndMediaEdits(gQuicktime_media);
        duration = QT_GetMediaDuration(gQuicktime_media);
        if (duration != 0) {
            QT_InsertMediaIntoTrack(gQuicktime_track, 0, 0, duration, 0x10000);
        }
        if (gQuicktime_movie_file != 0) {
            QT_CloseMovieFile(gQuicktime_movie_file);
        }
        sprintf(path, "%s%3.3d.MOV", gQuick_time_movie_path_stub, gQuicktime_movie_counter);
        gQuicktime_movie_counter += 1;
        fs_spec.vRefNum = 0;
        fs_spec.parID = 0;
        strcpy(fs_spec.name, path);
        QT_c2pstr(fs_spec.name);
        QT_FlattenMovieData(gQuicktime_movie, 1, (const FSSpec *)&fs_spec, 0x54564f44, -1, createMovieFileDeleteCurFile);
        if (gQuicktime_movie_file != 0) {
            QT_CloseMovieFile(gQuicktime_movie_file);
        }
        QT_DisposeMovie(gQuicktime_movie);
        PDFileDelete(gQuick_time_temp_path, 1);
        sprintf(path2, "%s.#Res", gQuick_time_temp_path);
        PDFileDelete(path2, 1);
        if (gQuicktime_frame_pixmap_handle != NULL) {
            QT_DisposeHandle(gQuicktime_frame_pixmap_handle);
            gQuicktime_frame_pixmap_handle = NULL;
        }
        if (gQuicktime_image_description_handle != NULL) {
            QT_DisposeHandle(gQuicktime_image_description_handle);
            gQuicktime_image_description_handle = NULL;
        }
        if (gQuicktime_compressed_frame_handle != NULL) {
            QT_DisposeHandle(gQuicktime_compressed_frame_handle);
            gQuicktime_compressed_frame_handle = NULL;
        }
    }
    gShow_status_message = 0;
}

// GenerateOneMovieFrame

extern void C2_HOOK_FASTCALL WaitFor(tU32 pDelay);
extern short C2_HOOK_FASTCALL CreateMovie(void);
extern short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);
extern void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen);
extern br_uint_16 gQuickTime_pixel_depth;
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);
extern int gQuicktime_max_compression_size;
extern void* gQuicktime_compressed_frame_pixels;
extern br_uint_16* gQuicktime_frame_pixel_conversion_table;
extern void* C2_HOOK_CDECL FUN_0056fde0(int pWidth, int pHeight, void* pCompressor);
extern void* C2_HOOK_CDECL FUN_0056c640(void* pMovie, void* pCompressor);
extern int C2_HOOK_CDECL FUN_0056a920(void);
extern void* C2_HOOK_CDECL FUN_0056c6e0(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality);
extern int C2_HOOK_CDECL FUN_0056b340(void* pMedia);
extern void* C2_HOOK_CDECL FUN_00570600(int pSize);
extern int C2_HOOK_CDECL FUN_005705c0(void);
extern int C2_HOOK_CDECL FUN_0056d7e0(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size);
extern void C2_HOOK_CDECL FUN_00570a40(void* pHandle);
extern void C2_HOOK_CDECL FUN_005707e0(void* pHandle);
extern int C2_HOOK_CDECL FUN_0056d8a0(void* pImage_sequence, void* pFrame_pixmap_handle, int pFrame_number, void* pFrame_rect1, void* pFrame_rect2, br_uint_16 pPixel_depth, int pCompressor, int pColor_table_id, int pSpatial_quality, int pTemporal_quality, int pFrame_type, int pUnused12, int pFlags, void* pImage_description_handle);
extern br_pixelmap* gRender_palette;
extern short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);
extern short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);
extern FSSpec gQt_file_spec;
extern struct Rect gQuicktime_rect;
extern int gRecording_paused;
extern tU32 gRecording_time;
extern int gRecording_mouse_disabled;

// (reference only)
// FUNCTION: CARMA2_HW 0x004e1da0
int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap* pScreen, int pFrame_period) {
    tU32 remaining;
    int width;
    int height;
    char path[260];

    switch (gNext_replay_record_state) {
    case 1:
        gNext_replay_record_state = 2;
        return 0;
    case 2:
        WaitFor(2000);
        width = gCurrent_graf_data->width;
        if (gNbPixelBits == 16) {
            width = width / 2;
        }
        height = gCurrent_graf_data->height;
        if (gNbPixelBits == 16) {
            height = height / 2;
        }
        gQuicktime_rect.top = 0;
        gQuicktime_rect.left = 0;
        gQuicktime_rect.right = width;
        gQuicktime_rect.bottom = height;
        if (QT_CreateMovieFile(&gQt_file_spec, 0x54564f44, -2, 0x80000000, &gQuicktime_movie_file, &gQuicktime_movie) != 0) {
            PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
            PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
            if (QT_CreateMovieFile(&gQt_file_spec, 0x54564f44, -2, 0x80000000, &gQuicktime_movie_file, &gQuicktime_movie) != 0) {
                gNext_replay_record_state = 10;
                gShow_status_message = 1;
                gStatus_message_1 = eMiscString_movie_error;
                gStatus_message_2 = eMiscString_aborting;
                return 0;
            }
        }
        gNext_replay_record_state = 3;
        gShow_status_message = 1;
        gStatus_message_1 = eMiscString_initialising_movie;
        gStatus_message_2 = eMiscString_please_wait;
        return 0;
    case 3:
        if (CreateMovie() != 0) {
            gNext_replay_record_state = 10;
            gShow_status_message = 1;
            gStatus_message_1 = eMiscString_movie_error;
            gStatus_message_2 = eMiscString_aborting;
            return 0;
        }
        gNext_replay_record_state = 4;
        gShow_status_message = 1;
        gStatus_message_1 = eMiscString_recording_movie;
        gStatus_message_2 = 0;
        return 0;
    case 4:
        if (gRecording_paused == 0) {
            remaining = gRecording_time + pFrame_period;
            while (remaining >= 1000 / 18) {
                if (GenerateOneMovieFrame(pScreen) != 0) {
                    gNext_replay_record_state = 10;
                    gShow_status_message = 1;
                    gStatus_message_1 = eMiscString_movie_error;
                    gStatus_message_2 = eMiscString_aborting;
                    return 0;
                }
                remaining -= 1000 / 18;
            }
            gRecording_time = remaining;
            return 0;
        }
        break;
    case 5:
        gRecording_mouse_disabled = 0;
        break;
    case 10:
        WaitFor(2000);
        gNext_replay_record_state = 0;
        if (gQuicktime_movie_file != 0) {
            QT_CloseMovieFile(gQuicktime_movie_file);
        }
        QT_DisposeMovie(gQuicktime_movie);
        PDFileDelete(gQuick_time_temp_path, 1);
        sprintf(path, "%s.#Res", gQuick_time_temp_path);
        PDFileDelete(path, 1);
        if (gQuicktime_frame_pixmap_handle != NULL) {
            QT_DisposeHandle(gQuicktime_frame_pixmap_handle);
            gQuicktime_frame_pixmap_handle = NULL;
        }
        if (gQuicktime_image_description_handle != NULL) {
            QT_DisposeHandle(gQuicktime_image_description_handle);
            gQuicktime_image_description_handle = NULL;
        }
        if (gQuicktime_compressed_frame_handle != NULL) {
            QT_DisposeHandle(gQuicktime_compressed_frame_handle);
            gQuicktime_compressed_frame_handle = NULL;
        }
        return 0;
    case 20:
        return 0;
    }
    return 0;
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

// Body adapted from ApexWorm/rec2 branch match-show-status-message (GPL-3.0).

// FUNCTION: CARMA2_HW 0x004e2120
void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2) {

    gShow_status_message = 1;
    gStatus_message_1 = msg1;
    gStatus_message_2 = msg2;
}

// FUNCTION: CARMA2_HW 0x004e2140
int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e2190
void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    int state;

    state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = state;
}

// TrapS3SoundOutput

// FreeS3SoundOutput

// GLOBAL: CARMA2_HW 0x006a0af0
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0c30
Movie gQuicktime_movie;

// GLOBAL: CARMA2_HW 0x006a0c34
Handle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
Handle gQuicktime_image_description_handle;

// GLOBAL: CARMA2_HW 0x006a0c3c
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0af4
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0afc
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0c10
Track gQuicktime_track;

// GLOBAL: CARMA2_HW 0x006a0c2c
int gQuicktime_movie_counter;

// GLOBAL: CARMA2_HW 0x006a0b08
FSSpec gQt_file_spec;

// GLOBAL: CARMA2_HW 0x006a0c18
struct Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x006a0c28
int gRecording_paused;

// GLOBAL: CARMA2_HW 0x006a0c54
tU32 gRecording_time;

// GLOBAL: CARMA2_HW 0x006baa28
int gRecording_mouse_disabled;

// GLOBAL: CARMA2_HW 0x006a23bc
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x0065ef2c
int gQuicktime_frame_flags = 1;

// GLOBAL: CARMA2_HW 0x006a0c44
br_uint_16* gQuicktime_frame_pixels;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e1250
int C2_HOOK_FASTCALL WriteBannerFrame(void) {
    br_pixelmap* pBanner;
    int frame_size;
    char codec_info;
    short result;
    int i;

    if (gQuick_time_banner_number != 0) {
        pBanner = LoadPixelmap(gQuick_time_banner_texture_name);
        if (pBanner != NULL) {
            if (pBanner->width != 320 || pBanner->height != 200) {
                BrFailure("Banner picture must be 320x200");
            }
            *(void**)(*gQuicktime_frame_pixmap_handle) = gQuicktime_frame_pixels;
            *(short*)(*gQuicktime_frame_pixmap_handle + 4) = (gNbPixelBits == 16 ? gCurrent_graf_data->width / 2 : gCurrent_graf_data->width) * 2;
            memset(gQuicktime_frame_pixels, 0, (gNbPixelBits == 16 ? gCurrent_graf_data->width / 2 : gCurrent_graf_data->width) * (gNbPixelBits == 16 ? gCurrent_graf_data->height / 2 : gCurrent_graf_data->height) * 2);
            dr_dprintf("Converting banner. w=%d, h=%d, type=%d", pBanner->width, pBanner->height, pBanner->type);
            FUN_004e1400(gQuicktime_frame_pixmap_handle, pBanner);
            for (i = 0; i < 0x12; i++) {
                result = FUN_0056d8c0(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle, &gQuicktime_rect, 0xc0, *gQuicktime_compressed_frame_handle, &frame_size, &codec_info, 0);
                if (result != 0) {
                    return result;
                }
                *(int*)(*gQuicktime_image_description_handle + 0x2c) = frame_size;
                result = FUN_0056b440(gQuicktime_media, gQuicktime_compressed_frame_handle, 0, *(int*)(*gQuicktime_image_description_handle + 0x2c), 0x37, gQuicktime_image_description_handle, 1, 0, 0);
                if (result != 0) {
                    return result;
                }
            }
            gQuicktime_frame_flags = 1;
            return 0;
        }
    }
    gQuicktime_frame_flags = 1;
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e0fe0
short C2_HOOK_FASTCALL CreateMovie(void) {
    int height;
    int width;
    int result;

    height = (gNbPixelBits == 16 ? gCurrent_graf_data->height / 2 : gCurrent_graf_data->height);
    width = (gNbPixelBits == 16 ? gCurrent_graf_data->width / 2 : gCurrent_graf_data->width);
    gQuicktime_track = FUN_0056c640(gQuicktime_movie, FUN_0056fde0(width * 2, 1, FUN_0056fde0(height * 2, 1, (void*)0x100)));
    result = FUN_0056a920();
    if ((short)result == 0) {
        gQuicktime_media = FUN_0056c6e0(gQuicktime_track, 0x76696465, 1000, 0, 0);
        result = FUN_0056a920();
        if ((short)result == 0) {
            result = FUN_0056b340(gQuicktime_media);
            if ((short)result == 0) {
                gQuicktime_frame_pixmap_handle = FUN_00570600(0x32);
                result = FUN_005705c0();
                if ((short)result == 0) {
                    memset(*gQuicktime_frame_pixmap_handle, 0, 0x32);
                    *(short*)(*gQuicktime_frame_pixmap_handle + 0x1e) = 0x10;
                    *(short*)(*gQuicktime_frame_pixmap_handle + 0x20) = 0x10;
                    *(short*)(*gQuicktime_frame_pixmap_handle + 0x22) = 3;
                    *(short*)(*gQuicktime_frame_pixmap_handle + 0x24) = 5;
                    result = FUN_0056d7e0(gQuicktime_frame_pixmap_handle, &gQuicktime_rect, 0, 0x200, gQuickTime_compressor_code, 0xfffffffe, &gQuicktime_max_compression_size);
                    if ((short)result == 0) {
                        gQuicktime_compressed_frame_handle = FUN_00570600(gQuicktime_max_compression_size);
                        result = FUN_005705c0();
                        if ((short)result == 0) {
                            FUN_00570a40(gQuicktime_compressed_frame_handle);
                            FUN_005707e0(gQuicktime_compressed_frame_handle);
                            gQuicktime_compressed_frame_pixels = *gQuicktime_compressed_frame_handle;
                            gQuicktime_image_description_handle = FUN_00570600(4);
                            result = FUN_005705c0();
                            if ((short)result == 0) {
                                if (gQuicktime_frame_pixels == NULL) {
                                    gQuicktime_frame_pixels = BrMemAllocate((gNbPixelBits == 16 ? gCurrent_graf_data->width / 2 : gCurrent_graf_data->width) * (gNbPixelBits == 16 ? gCurrent_graf_data->height / 2 : gCurrent_graf_data->height) * 2, 0xfd);
                                    if (gQuicktime_frame_pixels == NULL) {
                                        return 1;
                                    }
                                }
                                result = FUN_0056d8a0(&gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle, 0, &gQuicktime_rect, &gQuicktime_rect, gQuickTime_pixel_depth, gQuickTime_compressor_code, 0xfffffffe, 0x200, 0x200, 0xf, 0, 0xc0, gQuicktime_image_description_handle);
                                if ((short)result == 0) {
                                    result = WriteBannerFrame();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004e1c70
short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    int frame_flags;
    int pixmap_width;
    int frame_size;
    char codec_info;
    short result;

    if (gQuicktime_frame_flags != 0) {
        frame_flags = 0xc0;
        gQuicktime_frame_flags = 0;
    } else {
        frame_flags = 0x40;
    }
    PossibleLock(1);
    *(void**)(*gQuicktime_frame_pixmap_handle) = gQuicktime_frame_pixels;
    if (gNbPixelBits == 16) {
        pixmap_width = gCurrent_graf_data->width / 2;
    } else {
        pixmap_width = gCurrent_graf_data->width;
    }
    *(short*)(*gQuicktime_frame_pixmap_handle + 4) = pixmap_width * 2;
    FUN_004e1400(gQuicktime_frame_pixmap_handle, pScreen);
    result = FUN_0056d8c0(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle,
            &gQuicktime_rect, frame_flags, *gQuicktime_compressed_frame_handle,
            &frame_size, &codec_info, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    *(int*)(*gQuicktime_image_description_handle + 0x2c) = frame_size;
    result = FUN_0056b440(gQuicktime_media, gQuicktime_compressed_frame_handle, 0,
            *(int*)(*gQuicktime_image_description_handle + 0x2c),
            0x37, gQuicktime_image_description_handle, 1, 0, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    PossibleUnlock(1);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004e1400
void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen) {
    int row;
    int height;
    int destination_offset;
    int col;
    int width;
    int i;
    int pixel_step;
    int top;
    int source_x;
    int source_pixel;
    int dest;

    (void)pFrame_pixmap_handle;
    if (gQuicktime_frame_pixel_conversion_table == NULL) {
        gQuicktime_frame_pixel_conversion_table = BrMemAllocate(0x20000, 0xfd);
        if (gNbPixelBits == 0x10) {
            i = 0;
            do {
                unsigned int value;
                br_uint_8 blue;

                value = ((unsigned int)i >> 1) & 0x7fe0;
                blue = (br_uint_8)i;
                value |= blue & 0x1f;
                i++;
                gQuicktime_frame_pixel_conversion_table[i - 1] = ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            } while (i < 0x10000);
        } else {
            for (i = 0; i < 256; i++) {
                br_uint_32 colour;
                unsigned int value;

                colour = ((br_uint_32*)gRender_palette->pixels)[i];
                value = (((colour >> 19) & 0x1f) << 5) | ((colour >> 11) & 0x1f);
                value = (value << 5) | ((colour >> 3) & 0x1f);
                gQuicktime_frame_pixel_conversion_table[i] = ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            }
        }
    }

    PossibleLock(1);
    if (gNbPixelBits == 0x10) {
        width = gCurrent_graf_data->width / 2;
    } else {
        width = gCurrent_graf_data->width;
    }
    height = gCurrent_graf_data->height;
    if (gNbPixelBits == 0x10) {
        height /= 2;
    }

    if (gNbPixelBits == 0x10) {
        pixel_step = 2;
        if (pScreen->height == 200) {
            top = 20;
            height = 200;
            pixel_step = 1;
        } else {
            top = 0;
        }
        row = 0;
        if (height > 0) {
            destination_offset = top * width * 2;
            do {
                if (width > 0) {
                    dest = destination_offset;
                    source_x = 0;
                    col = width;
                    do {
                        source_pixel = (pixel_step * row * (int)pScreen->row_bytes) / 2 + source_x;
                        *(br_uint_16*)((char*)gQuicktime_frame_pixels + dest) = gQuicktime_frame_pixel_conversion_table[((br_uint_16*)pScreen->pixels)[source_pixel]];
                        dest += 2;
                        source_x += pixel_step;
                        col--;
                    } while (col != 0);
                }
                row++;
                destination_offset += width * 2;
            } while (row < height);
            PossibleUnlock(1);
            return;
        }
    } else {
        row = 0;
        destination_offset = 0;
        do {
            if (width > 0) {
                dest = destination_offset;
                col = 0;
                do {
                    source_pixel = row * (int)pScreen->row_bytes + col;
                    *(br_uint_16*)((char*)gQuicktime_frame_pixels + dest) = gQuicktime_frame_pixel_conversion_table[((br_uint_8*)pScreen->pixels)[source_pixel]];
                    dest += 2;
                    col++;
                } while (col < width);
            }
            row++;
            destination_offset += width * 2;
        } while (row < 200);
    }
    PossibleUnlock(1);
}
extern void* gQuickTime_dispatcher;
extern void* gQuickTime_dispatch;

#ifdef REC2_MATCHING
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
#else
short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
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
#else
short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a0c4c
int gQuickTime_compressor_code;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x280017).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00575340
__declspec(naked) int C2_HOOK_CDECL FUN_00575340(char* pString) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_5340
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_5340:
        mov     eax, 0x280017
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
int C2_HOOK_CDECL FUN_00575340(char* pString) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006736a8
void* gQuickTime_dispatcher;

// GLOBAL: CARMA2_HW 0x006736b4
void* gQuickTime_dispatch;

// GLOBAL: CARMA2_HW 0x006a0af8
int gQuicktime_max_compression_size;

// GLOBAL: CARMA2_HW 0x006a0c38
void* gQuicktime_compressed_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c48
br_uint_16* gQuicktime_frame_pixel_conversion_table;

// GLOBAL: CARMA2_HW 0x0065ef34
br_uint_16 gQuickTime_pixel_depth = 0x10;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x100001).
#ifdef REC2_MATCHING
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
#else
void* C2_HOOK_CDECL FUN_0056fde0(int pWidth, int pHeight, void* pCompressor) {
    return NULL;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20188).
#ifdef REC2_MATCHING
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
#else
void* C2_HOOK_CDECL FUN_0056c640(void* pMovie, void* pCompressor) {
    return NULL;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20003).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_0056a920(void) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x2018e).
#ifdef REC2_MATCHING
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
#else
void* C2_HOOK_CDECL FUN_0056c6e0(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality) {
    return NULL;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20072).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_0056b340(void* pMedia) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x150008).
#ifdef REC2_MATCHING
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
#else
void* C2_HOOK_CDECL FUN_00570600(int pSize) {
    return NULL;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x150006).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_005705c0(void) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x30004).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_0056d7e0(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x15002e).
#ifdef REC2_MATCHING
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
#else
void C2_HOOK_CDECL FUN_00570a40(void* pHandle) {
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x150019).
#ifdef REC2_MATCHING
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
#else
void C2_HOOK_CDECL FUN_005707e0(void* pHandle) {
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x3000a).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_0056d8a0(void* pImage_sequence, void* pFrame_pixmap_handle, int pFrame_number, void* pFrame_rect1, void* pFrame_rect2, br_uint_16 pPixel_depth, int pCompressor, int pColor_table_id, int pSpatial_quality, int pTemporal_quality, int pFrame_type, int pUnused12, int pFlags, void* pImage_description_handle) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20002, GetMoviesError).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_0056a900(void) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20023, DisposeMovie).
#ifdef REC2_MATCHING
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
#else
void C2_HOOK_CDECL FUN_0056abe0(void* pMovie) {
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x2006a, GetMediaDuration).
#ifdef REC2_MATCHING
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
#else
int C2_HOOK_CDECL FUN_0056b240(void* pMedia) {
    return 0;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20073, EndMediaEdits).
#ifdef REC2_MATCHING
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
#else
short C2_HOOK_CDECL FUN_0056b360(void* pMedia) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x200d5, CloseMovieFile).
#ifdef REC2_MATCHING
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
#else
short C2_HOOK_CDECL FUN_0056baa0(short pRes_ref_num) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20183, InsertMediaIntoTrack).
#ifdef REC2_MATCHING
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
#else
short C2_HOOK_CDECL FUN_0056c600(void* pTrack, int pTrack_start, int pMedia_time, int pMedia_duration, int pMedia_rate) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x20191, CreateMovieFile).
#ifdef REC2_MATCHING
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
#else
short C2_HOOK_CDECL FUN_0056c720(void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags, short* pRes_ref_num, void** pMovie) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x2019c, FlattenMovieData).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056c800
__declspec(naked) void* C2_HOOK_CDECL FUN_0056c800(void* pMovie, int pFlatten_flags, const void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c800
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c800:
        mov     eax, 0x2019c
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void* C2_HOOK_CDECL FUN_0056c800(void* pMovie, int pFlatten_flags, const void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags) {
    return NULL;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x3001b, CDSequenceEnd).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056dac0
__declspec(naked) short C2_HOOK_CDECL FUN_0056dac0(void* pSequence) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_dac0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_dac0:
        mov     eax, 0x3001b
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
short C2_HOOK_CDECL FUN_0056dac0(void* pSequence) {
    return 1;
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// QuickTime dispatcher trampoline (selector 0x150032, DisposeHandle).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00570ac0
__declspec(naked) void C2_HOOK_CDECL FUN_00570ac0(void* pHandle) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_0ac0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_0ac0:
        mov     eax, 0x150032
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void C2_HOOK_CDECL FUN_00570ac0(void* pHandle) {
}
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// No-op callback ()
// FUNCTION: CARMA2_HW 0x004e1c30
void C2_HOOK_FASTCALL NoOp_004e1c30(void) {
}
