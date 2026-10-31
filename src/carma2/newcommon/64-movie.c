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

// GLOBAL: CARMA2_HW 0x006a23b8
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

// InitMovie

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

// based on carpocalypse2 src/carma2/common/movie.c:148
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
extern void C2_HOOK_FASTCALL WriteBannerFrame(void);
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern void C2_HOOK_FASTCALL PDFileDelete(const char *pPath, int pIgnore_read_only);

// based on carpocalypse2 movie.c:88 and the disassembly (inlined MovieStopRecordingAndSave)
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
extern int C2_HOOK_FASTCALL CreateMovie(void);
extern short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);
extern void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen);
extern short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);
extern short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);
extern FSSpec gQt_file_spec;
extern struct Rect gQuicktime_rect;
extern int gRecording_paused;
extern tU32 gRecording_time;
extern int gRecording_mouse_disabled;

// based on carpocalypse2 src/carma2/common/video.c:190 (reference only)
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
        break;
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

// ShowStatusMessage

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

// TidyMovieARDisplay

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

// STUB: CARMA2_HW 0x004e1250
void C2_HOOK_FASTCALL WriteBannerFrame(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e0fe0
int C2_HOOK_FASTCALL CreateMovie(void) {
    NOT_IMPLEMENTED();
    return 0;
}

// based on carpocalypse2 src/carma2/common/video.c:322 (GPL-3.0)
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
// STUB: CARMA2_HW 0x004e1400
void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen) {
    NOT_IMPLEMENTED();
}
// STUB: CARMA2_HW 0x0056d8c0
short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    NOT_IMPLEMENTED();
    return 0;
}
// STUB: CARMA2_HW 0x0056b440
short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006a0c4c
int gQuickTime_compressor_code;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00575340
int C2_HOOK_CDECL FUN_00575340(char* pString) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
