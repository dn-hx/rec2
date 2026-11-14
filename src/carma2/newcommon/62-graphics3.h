#ifndef GUARD_62_GRAPHICS3_H
#define GUARD_62_GRAPHICS3_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gMini_map_visible;
extern int gCount_smashable_race_targets;
extern tSmashable_race_target gSmashable_race_targets[300];
extern tCar_icon gCar_icons[128];
extern br_actor* gCar_icons_model_actor;
extern int gTarget_lock_enabled;
extern int gMap_render_x_i;
extern int gMap_render_y_i;
extern int gMap_render_width_i;
extern int gMap_render_height_i;
extern br_vector3 gOrigin_map;
extern br_vector2 gOrigin_headup_map;
extern br_matrix23 gMatrix23_0068c880;
extern int gTrack_version;

// CarArrowColour

extern void C2_HOOK_FASTCALL ToggleTargetLock(void);
extern void C2_HOOK_FASTCALL CycleTargetLock(void);

extern void C2_HOOK_FASTCALL CycleMapOptions(void);
extern void C2_HOOK_FASTCALL ToggleMapTrans(void);

extern void C2_HOOK_FASTCALL InitTargetLock(void);

extern void C2_HOOK_FASTCALL ReceivedViewCar(int* pA, int* pB);

extern void C2_HOOK_FASTCALL SetIntegerMapRenders(void);
// OppositeColour

extern void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3* pPos, br_uint_32 pColour);
extern void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour);
// DrawSmashBlip

extern void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour);
// CopyMapToScreen

// DrawCarArrow

extern void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev);

extern void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL DrawOffsetCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL DoMapOverlays(br_pixelmap* pScreen);
// StartMap

// FinishMap

extern void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);

// DoDirectionFinderStuff

// MungeMoodMessage

extern void C2_HOOK_FASTCALL DoOpponentStatusHeadup(void);

extern void C2_HOOK_FASTCALL AllocateOppoBarActor(int pX, int pY);
// DoNearestOpponentRelativeHeightMarkerStuff

extern void C2_HOOK_FASTCALL MapOverlay(void);

// InitNearestCar

extern void C2_HOOK_FASTCALL MapStuffBeforeRender(void);
extern void C2_HOOK_FASTCALL MapStuffAfterRender(void);

// static void C2_HOOK_FASTCALL ReadMoodMessages(void);

extern void C2_HOOK_FASTCALL InitMap(void);

extern void C2_HOOK_FASTCALL ConvertCarIcons(br_pixelmap* pMap);

// static void C2_HOOK_FASTCALL MapChangedStatic(void);

extern void C2_HOOK_FASTCALL MapChanged(void);

extern void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0, int pKey1);

extern void C2_HOOK_FASTCALL InitSmashTargets(void);

extern void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown);

extern int C2_HOOK_FASTCALL NearestPowerOfTwo(int pValue);


extern int C2_HOOK_FASTCALL GetOpponentMood(tCar_spec* pCar);

#endif // GUARD_62_GRAPHICS3_H
