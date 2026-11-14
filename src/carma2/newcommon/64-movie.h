#ifndef GUARD_64_MOVIE_H
#define GUARD_64_MOVIE_H

#include "brender/br_types.h"
#include "c2_hooks.h"

extern int gNext_replay_record_state;
extern int gShow_status_message;

extern char gQuick_time_temp_path[256];
extern char gQuick_time_movie_path_stub[256];
extern char gQuick_time_banner_texture_name[256];
extern int gShow_status_message;

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

extern int C2_HOOK_FASTCALL WriteBannerFrame(void);
// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

extern int C2_HOOK_FASTCALL InitMovie(void);
extern short C2_HOOK_FASTCALL CreateMovie(void);
// MovieStartNewMovie

// MovieStopRecordingAndSave

// FreeOffQTshite

extern void C2_HOOK_CDECL InitQuickTimeStuff(void);

extern void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

extern void C2_HOOK_FASTCALL MovieRecordButtonPressed(void);
extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

extern short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);
extern int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap* pScreen, int pFrame_period);
extern int C2_HOOK_FASTCALL MovieRecording(void);

extern void C2_HOOK_FASTCALL KillStatusMessage(void);

extern void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2);

extern int C2_HOOK_FASTCALL GetMovieWidth(void);

extern int C2_HOOK_FASTCALL GetMovieHeight(void);

extern void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

// TrapS3SoundOutput

// FreeS3SoundOutput


extern void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen);
extern short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);
extern short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);
extern int C2_HOOK_CDECL FUN_00575340(char* pString);
extern void* C2_HOOK_CDECL FUN_0056fde0(int pWidth, int pHeight);
extern void* C2_HOOK_CDECL FUN_0056c640(void* pMovie, void* pWidth, void* pHeight, int pVolume);
extern int C2_HOOK_CDECL FUN_0056a920(void);
extern void* C2_HOOK_CDECL FUN_0056c6e0(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality);
extern int C2_HOOK_CDECL FUN_0056b340(void* pMedia);
extern void* C2_HOOK_CDECL FUN_00570600(int pSize);
extern int C2_HOOK_CDECL FUN_005705c0(void);
extern int C2_HOOK_CDECL FUN_0056d7e0(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size);
extern void C2_HOOK_CDECL FUN_00570a40(void* pHandle);
extern void C2_HOOK_CDECL FUN_005707e0(void* pHandle);
extern int C2_HOOK_CDECL FUN_0056d8a0(void* pImage_sequence, void* pFrame_pixmap_handle, int pFrame_number, void* pFrame_rect1, void* pFrame_rect2, br_uint_16 pPixel_depth, int pCompressor, int pColor_table_id, int pSpatial_quality, int pTemporal_quality, int pFrame_type, int pUnused12, int pFlags, void* pImage_description_handle);
extern int C2_HOOK_CDECL FUN_0056a900(void);
extern void C2_HOOK_CDECL FUN_0056abe0(void* pMovie);
extern int C2_HOOK_CDECL FUN_0056b240(void* pMedia);
extern short C2_HOOK_CDECL FUN_0056b360(void* pMedia);
extern short C2_HOOK_CDECL FUN_0056baa0(short pRes_ref_num);
extern short C2_HOOK_CDECL FUN_0056c600(void* pTrack, int pTrack_start, int pMedia_time, int pMedia_duration, int pMedia_rate);
extern short C2_HOOK_CDECL FUN_0056c720(void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags, short* pRes_ref_num, void** pMovie);
extern void* C2_HOOK_CDECL FUN_0056c800(void* pMovie, int pFlatten_flags, const void* pFile_spec, int pCreator, short pScript_tag, int pCreate_flags);
extern short C2_HOOK_CDECL FUN_0056dac0(void* pSequence);
extern void C2_HOOK_CDECL FUN_00570ac0(void* pHandle);
extern void C2_HOOK_FASTCALL NoOp_004e1c30(void);
extern void C2_HOOK_CDECL FUN_0056e760(void);
extern void C2_HOOK_CDECL FUN_00575730(void);
extern void* C2_HOOK_CDECL FUN_005757d0(void);

#endif // GUARD_64_MOVIE_H
