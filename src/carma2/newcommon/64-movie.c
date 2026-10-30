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

// GLOBAL: CARMA2_HW 0x0065ee50
char gDefault_quick_time_quality[] = "normal";

// GLOBAL: CARMA2_HW 0x0065ed30
char gDefault_quick_time_compressor[] = "animation";

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

// MovieRecordButtonPressed

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
extern int C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);
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

// STUB: CARMA2_HW 0x004e1c70
int C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    NOT_IMPLEMENTED();
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
