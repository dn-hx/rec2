/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_04_NETGAME_H
#define GUARD_04_NETGAME_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include "01-network.h"

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

/* Used by 04-netgame.c only. Declaring it here adds symbols to every including TU,
 * which rotates MSVC 5 register allocation in 04-netgame.c itself
 * (SortNetHeadAscending 0x49a330); that TU opts out and keeps a local copy. */
#ifndef REC2_NO_NET_CHUNK_BIT_POSITION_DECLS
typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU8 uid;
    undefined field_0x9[3];
    br_matrix34 mat;
} tNet_message_chunk_bit_position;
#endif

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

extern int gINT_0068d920;
extern tNet_game_player_info* gLast_lepper;
extern int gLast_it_change;

// SendShapeNumbers

extern void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id);

extern void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pPlayer_id);

// HostFillInCarContents

extern void C2_HOOK_FASTCALL ClientSendCarData(tU32 pNext_frame_time);

// DistanceBetweenCars

extern int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id);

// AddShapeStuffToMechanicsMessage

extern int C2_HOOK_FASTCALL TimeToSendData(void);

// SendCarData

// ReceivedRecover

// CopyMechanics

// ReceivedShapeNumbers

extern void C2_HOOK_FASTCALL FillInSimpleCarPos(tNet_message_chunk* pMessage, int pPlayer_index);

// ReceivedSimpleCarPos

// ReceivedMechanics

extern void C2_HOOK_FASTCALL ReceivedCopInfo(void);

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

#ifndef REC2_NO_NET_CHUNK_BIT_POSITION_DECLS
extern void C2_HOOK_FASTCALL ReceivedDBNonCarPosition(tNet_message_chunk_bit_position* pContents);
#endif

extern void C2_HOOK_FASTCALL SignalToStartRace(void);

// SignalToRepositionCar

extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);

// ReinitialiseCar

// RepositionPlayer

extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

#ifdef REC2_HW
extern void C2_HOOK_FASTCALL CreateBoxes(void);
extern void C2_HOOK_FASTCALL SendSHNonCar(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);

#endif

// DrawScoreBoxes

extern void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);

extern void C2_HOOK_FASTCALL DoNetScores(void);

extern void C2_HOOK_FASTCALL InitNetHeadups(void);

extern void C2_HOOK_FASTCALL DisposeNetHeadups(void);

// EverybodysLost

extern void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

extern void C2_HOOK_FASTCALL CheckForVampireWinner(void);

extern void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer);

// FarEnoughAway

extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer);

// SelectRandomFox

extern void C2_HOOK_FASTCALL CalcPlayerScores(void);

extern void C2_HOOK_FASTCALL SendPlayerScores(void);

extern void C2_HOOK_FASTCALL DoNetGameManagement(void);

extern void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);

// InitPlayers

extern void C2_HOOK_FASTCALL UseGeneralScore(int pScore);

// NetSendEnvironmentChanges

extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

extern void C2_HOOK_FASTCALL ReceivedGameplay(tNet_message_chunk* pContents, tNet_message* pMessage, tU32 pReceive_time);

extern void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

extern void C2_HOOK_FASTCALL DefaultNetName(void);

extern void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers);

// RemovePlayerFromShapeStatusLists

extern int C2_HOOK_CDECL SortNetHeadAscending(const void* pFirst, const void* pSecond);

extern int C2_HOOK_CDECL SortNetHeadDescending(const void* pFirst, const void* pSecond);

extern int gNot_shown_race_type_headup;

extern void C2_HOOK_FASTCALL SendAllDBNonCars(tNet_game_player_info* pRecipient);

extern void C2_HOOK_FASTCALL SendAllSHNonCars(tNet_game_player_info* pRecipient);

extern int gNet_cash_headup;

extern int gNet_ped_headup;

#endif // GUARD_04_NETGAME_H
