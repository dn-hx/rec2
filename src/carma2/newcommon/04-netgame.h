#ifndef GUARD_04_NETGAME_H
#define GUARD_04_NETGAME_H

#include "c2_hooks.h"
#include "rec2_types.h"

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player;
    tU32 time;
} tNet_message_chunk_recover;

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[6];
    tU32 ID;
    br_matrix34 mat;
    tU16 flags;
} tNet_message_chunk_non_car_position;

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tU32 non_car_id;
    tU32 player_id;
    undefined field_0xc[4];
    tU16 flags;
    tU8 object_hierarchy[];
} tNet_message_chunk_non_car;

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU8 uid;
    tCompressed_vector3 bounds_min;
    tCompressed_vector3 bounds_max;
    undefined field_0x16[2];
    tU32 time;
    undefined field_0x1c[4];
    tU8 flags;
    undefined field_0x21;
    tU8 object_hierarchy[];
} tNet_message_chunk_detached_non_car;

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU8 count;
    undefined field_0x9[3];
    struct {
        tPlayer_ID player;
        tU16 crush_state;
        undefined field_0x6[2];
    } players[1];
} tNet_message_chunk_car_crush_sync;

extern tU32 gPHIL_last_physics_tick;
extern int gNot_shown_race_type_headup;
extern tU32 gTime_for_punishment;
extern int gNet_cash_headup;
extern int gINT_0068d920;
extern int gNet_ped_headup;
extern int gNet_fullscreen_active;

// SendShapeNumbers

extern void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id);

extern void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pPlayer_id);
// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

extern int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id);

// AddShapeStuffToMechanicsMessage

extern int C2_HOOK_FASTCALL TimeToSendData(void);
// SendCarData

extern void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents);
// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

extern void C2_HOOK_FASTCALL ReceivedCopInfo(void);

// SendAllNonCarPositions

extern void C2_HOOK_FASTCALL ReceivedNonCarPosition(tNet_message_chunk_non_car_position* pContents);
extern void C2_HOOK_FASTCALL ReceivedNonCar(tNet_message_chunk_non_car* pContents);
extern void C2_HOOK_FASTCALL SignalToStartRace(void);
// SignalToRepositionCar

extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);
extern void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar);
// RepositionPlayer

extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits);

extern int C2_HOOK_CDECL SortNetHeadAscending(const void* pFirst, const void* pSecond);
extern int C2_HOOK_CDECL SortNetHeadDescending(const void* pFirst, const void* pSecond);
// ClipName

extern void C2_HOOK_FASTCALL CreateBoxes(void);
// DrawScoreBoxes

extern void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);

extern void C2_HOOK_FASTCALL DoNetScores(void);

extern void C2_HOOK_FASTCALL InitNetHeadups(void);

extern void C2_HOOK_FASTCALL DisposeNetHeadups(void);

// EverybodysLost

// DeclareWinner

extern void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer);
// CheckForVampireWinner

extern void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer);
// FarEnoughAway

extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);
// SelectRandomFox

extern void C2_HOOK_FASTCALL CalcPlayerScores(void);
// SendPlayerScores

extern void C2_HOOK_FASTCALL DoNetGameManagement(void);

extern void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);
// InitPlayers

// UseGeneralScore

extern void C2_HOOK_FASTCALL NetSendEnvironmentChanges(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

// ReceivedGameplay

extern void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
// SendGameplayToHost

extern void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

extern void C2_HOOK_FASTCALL DefaultNetName(void);

extern void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers);

extern void C2_HOOK_FASTCALL RemovePlayerFromShapeStatusLists(tPlayer_ID pPlayer_id, int pPlayer_count, tNet_game_player_info* pPlayers);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tU8 pType, int pSize_decider, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL FUN_43d7d0(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL FUN_42da20(undefined4* pRecipient);
extern void C2_HOOK_FASTCALL FUN_4f0f80(tNet_game_player_info* pRecipient);
extern int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode);
extern void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL ToggleFlapsFromNetwork(tNet_message_chunk* pChunk);
extern void C2_HOOK_FASTCALL ReceivedCrushSync(tNet_message_chunk_car_crush_sync* pContents);
extern void C2_HOOK_FASTCALL ReceivedDetachedNonCarData(tNet_message_chunk_detached_non_car* pContents);
extern void C2_HOOK_FASTCALL FUN_0043d810(tU8* pContents);
extern void C2_HOOK_FASTCALL FUN_0049cb00(tPhysics_object* pObj, br_matrix34* pMat, br_matrix34* pParent_mat);

// FUNCTION: CARMA2_HW 0x0049bad0
extern void C2_HOOK_FAKE_THISCALL FUN_0049bad0(void* pThis);

#endif // GUARD_04_NETGAME_H
