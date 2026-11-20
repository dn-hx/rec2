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
#include "brender/br_types.h"
#include "rec2_types.h"
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
extern ImageSequence gQuicktime_image_sequence;
extern Media gQuicktime_media;
extern Track gQuicktime_track;
extern short gQuicktime_movie_file;
extern int gQuicktime_movie_counter;
extern Movie gQuicktime_movie;
extern Handle gQuicktime_frame_pixmap_handle;
extern Handle gQuicktime_image_description_handle;
extern Handle gQuicktime_compressed_frame_handle;
extern br_uint_16* gQuicktime_frame_pixel_conversion_table;
extern br_uint_16* gQuicktime_frame_pixels;
extern struct Rect gQuicktime_rect;
extern FSSpec gQt_file_spec;
extern short gQuicktime_movie_file;
extern Movie gQuicktime_movie;
extern int gRecording_paused;
extern tU32 gRecording_time;
extern int gRecording_mouse_disabled;
extern Handle gQuicktime_frame_pixmap_handle;
extern Handle gQuicktime_image_description_handle;
extern Handle gQuicktime_compressed_frame_handle;

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

extern int C2_HOOK_FASTCALL WriteBannerFrame(void);

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

extern short C2_HOOK_FASTCALL CreateMovie(void);

// MovieStartNewMovie

// MovieStopRecordingAndSave

// FreeOffQTshite

extern void C2_HOOK_CDECL InitQuickTimeStuff(void);

extern void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

// MovieRecordButtonPressed

extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

extern short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);

extern int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap* pScreen, int pFrame_period);

extern int C2_HOOK_FASTCALL MovieRecording(void);

extern void C2_HOOK_FASTCALL KillStatusMessage(void);

extern void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2);

extern int C2_HOOK_FASTCALL GetMovieWidth(void);

extern int C2_HOOK_FASTCALL GetMovieHeight(void);

extern void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

// TrapS3SoundOutput

// FreeS3SoundOutput

#endif // GUARD_64_MOVIE_H
