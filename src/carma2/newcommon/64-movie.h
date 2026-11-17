/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_64_MOVIE_H
#define GUARD_64_MOVIE_H

#include "c2_hooks.h"
#include "brender/br_types.h"

/* the unit's prototypes are written in the shim's own types (Handle, Track, Movie, ...) */
#include <QTML.h>
#include <Movies.h>
#include <TextUtils.h>

extern int gNext_replay_record_state;
extern int gShow_status_message;

extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;

extern char gQuick_time_temp_path[256];
extern char gQuick_time_movie_path_stub[256];
extern char gQuick_time_banner_texture_name[256];
extern Handle gQuicktime_frame_pixmap_handle;
extern br_uint_16* gQuicktime_frame_pixels;
extern ImageSequence gQuicktime_image_sequence;
extern struct Rect gQuicktime_rect;
extern Handle gQuicktime_compressed_frame_handle;
extern Handle gQuicktime_image_description_handle;
extern Media gQuicktime_media;
extern int gQuicktime_frame_flags;

// Create555BELookupTable

extern void C2_HOOK_FASTCALL Convert565BrPixelmapTo555BigEndianPixmap(void* pFrame_pixmap_handle, br_pixelmap* pScreen);

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

extern void C2_HOOK_CDECL InitQuickTimeStuff(void);

extern void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

// MovieRecordButtonPressed

extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

// GenerateOneMovieFrame

// MovieRecordFrame

extern int C2_HOOK_FASTCALL MovieRecording(void);

extern void C2_HOOK_FASTCALL KillStatusMessage(void);

extern void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2);

extern int C2_HOOK_FASTCALL GetMovieWidth(void);

extern int C2_HOOK_FASTCALL GetMovieHeight(void);

extern void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

// TrapS3SoundOutput

// FreeS3SoundOutput

extern short C2_HOOK_CDECL CompressSequenceFrame(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);

extern short C2_HOOK_CDECL AddMediaSample(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);

#endif // GUARD_64_MOVIE_H
