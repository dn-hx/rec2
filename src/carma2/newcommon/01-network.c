#include "01-network.h"

#include "04-netgame.h"
#include "32-spark.h"
#include "53-controls.h"
#include "platform.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"

#include <stdlib.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x0068d95c
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x0068d978
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x0068d984
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0068d98c
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c80
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690230
tAddToJoinListProc* gAdd_proc;

// GLOBAL: CARMA2_HW 0x00690238
int gTime_for_next_one;

// GLOBAL: CARMA2_HW 0x00690c34
tNet_game_details* gCurrent_join_poll_game;

// GLOBAL: CARMA2_HW 0x00690c38
int gNet_service_disable;

// GLOBAL: CARMA2_HW 0x00690c3c
int gIn_net_service;

// GLOBAL: CARMA2_HW 0x00690c48
tNet_message_memory* gDynamic_messages;

// GLOBAL: CARMA2_HW 0x00690c4c
tU32 gLast_flush_message;

// GLOBAL: CARMA2_HW 0x00690c54
int gJoin_list_mode;

// GLOBAL: CARMA2_HW 0x0074b7dc
tPlayer_ID gLocal_net_ID;

// GLOBAL: CARMA2_HW 0x00690c44
int gOnly_receive_guarantee_replies;

// GLOBAL: CARMA2_HW 0x0068d97c
tMin_message* gMin_messages;

// GLOBAL: CARMA2_HW 0x0068d970
tMid_message* gMid_messages;

// GLOBAL: CARMA2_HW 0x0068d980
tMax_message* gMax_messages;

// GLOBAL: CARMA2_HW 0x0068d96c
int gNet_initialised;

// GLOBAL: CARMA2_HW 0x0068d960
int gDont_allow_joiners;

// GLOBAL: CARMA2_HW 0x0074a690
tU32 gUINT_0074a690;

// GLOBAL: CARMA2_HW 0x0074a718
tU32 gUINT_0074a718;

// GLOBAL: CARMA2_HW 0x00659d30
tU16 gGuarantee_number = 1;

// GLOBAL: CARMA2_HW 0x0068d958
int gNext_guarantee;

// FUNCTION: CARMA2_HW 0x0049d210
int C2_HOOK_FASTCALL NetInitialise(void) {
    int i;

    InitAbuseomatic();
    gNet_service_disable = 0;
    gIn_net_service = 0;
    gMessage_header_size = PDNetGetHeaderSize();
    gOnly_receive_guarantee_replies = 0;
    gMin_messages = BrMemAllocate(MIN_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMin_message)), kMem_net_min_messages);
    gMid_messages = BrMemAllocate(MID_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMid_message)), kMem_net_mid_messages);
    gMax_messages = BrMemAllocate(MAX_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMax_message)), kMem_net_max_messages);

    for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMin_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMid_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMax_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    gNet_initialised = PDNetInitialise() == 0;
    if (gNet_initialised) {
        InitNetHeadups();
    }
    GenerateItFoxShadeTable();
    gDont_allow_joiners = 0;
    gUINT_0074a690 = PDGetTotalTime();
    gUINT_0074a718 = PDGetTotalTime();
    gNext_guarantee = 0;
    gDynamic_messages = NULL;
    gGuarantee_number = ((tU16)PDGetTotalTime() & 0x3ff) + 1;
    return !gNet_initialised;
}

int C2_HOOK_FASTCALL NetShutdown(void) {
    int err;

    err = PDNetShutdown();
    DisposeAbuseomatic();
    BrMemFree(gMin_messages);
    BrMemFree(gMid_messages);
    BrMemFree(gMax_messages);
    DisposeExcessMemory();
    DisposeNetHeadups();
    return err;
}

// FUNCTION: CARMA2_HW 0x0049d370
void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {

    if (gNet_initialised) {
        NetShutdown();
        gNet_initialised = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0049d3e0
void C2_HOOK_FASTCALL DisableNetService(void) {

    gNet_service_disable = 1;
}

// FUNCTION: CARMA2_HW 0x0049d3f0
void C2_HOOK_FASTCALL ReenableNetService(void) {

    gNet_service_disable = 0;
}

// FUNCTION: CARMA2_HW 0x0049d400
int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void) {

    if (gIn_net_service) {
        gIn_net_service = 0;
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049d420
void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void) {

    gIn_net_service = 1;
}

// NetSendHeadupToAllPlayers

// NetSendHeadupToEverybody

// NetSendHeadupToPlayer

// InitialisePlayerStati

void C2_HOOK_FASTCALL LeaveTempGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        NetLeaveGameLowLevel();
    }
    gTime_for_next_one = 1;
    gCurrent_join_poll_game = NULL;
}

void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        BrMemFree(gCurrent_join_poll_game);
        gCurrent_join_poll_game = NULL;
    }
}

// STUB: CARMA2_HW 0x0049ec70
void C2_HOOK_FASTCALL DoNextJoinPoll(void) {

    if (gTime_for_next_one) {
        gCurrent_join_poll_game = NetAllocatePIDGameDetails();
        if (gCurrent_join_poll_game == NULL) {
            return;
        }
        if (PDNetGetNextJoinGame(gCurrent_join_poll_game, gJoin_poll_index)) {
            if (!NetJoinGameLowLevel(gCurrent_join_poll_game, "!TEMP!")) {
                tNet_message* message;
                gTime_for_next_one = 0;
                message = NetBuildMessage(0, 0);
                NetSendMessageToAddress(gCurrent_join_poll_game, message, &gCurrent_join_poll_game->pd_net_info);
                gBastard_has_answered = 0;
                gAsk_time = PDGetTotalTime();
            } else {
                DisposeCurrentJoinPollGame();
            }
            gJoin_poll_index += 1;
            return;
        } else {
            gJoin_poll_index = 0;
            DisposeCurrentJoinPollGame();
            return;
        }
    } else {
        if (gBastard_has_answered) {
            gAdd_proc(gCurrent_join_poll_game);
            LeaveTempGame();
        } else {
            if (PDGetTotalTime() - gAsk_time > 10000) {
                LeaveTempGame();
                DisposeCurrentJoinPollGame();
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049dbd0
void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc) {

    gAdd_proc = pAdd_proc;
    gJoin_list_mode = 1;
    gBastard_has_answered = 0;
    gTime_for_next_one = 1;
    gJoin_poll_index = 0;
    gCurrent_join_poll_game = NULL;
    PDNetStartProducingJoinList();
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049dc00
void C2_HOOK_FASTCALL NetEndJoinList(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on ApexWorm/rec2 b74fa8df4 (reference only)
// FUNCTION: CARMA2_HW 0x0049dc70
// FUNCTION: CARMA2_SW 0x00499df0
void C2_HOOK_FASTCALL NetDisposePIDGameInfo(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

// FUNCTION: CARMA2_HW 0x0049dc80
// FUNCTION: CARMA2_SW 0x00499e00
void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);

    return BrMemAllocate(sizeof(tNet_game_details), kMem_net_pid_details);
}

void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void) {

    if (gNet_mode == eNet_mode_host) {
        PDNetHostFinishGame();
    } else {
        PDNetLeaveGame();
    }
}

// STUB: CARMA2_HW 0x0049dc90
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
    NOT_IMPLEMENTED();
}

// NetSetPlayerSystemInfo

// FillInThisPlayer

// LoadCarN

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL RemoveCarFromEffects(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL DisposeCar(tCar_spec* pCar_spec, int pOwner);
extern void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);
extern void C2_HOOK_FASTCALL ARResetPiping(void);

// based on carpocalypse2 src/carma2/common/network.c:1043 (reference only)
// FUNCTION: CARMA2_HW 0x0049e100
void C2_HOOK_FASTCALL DisposeCarN(int pPlayer_index) {
    int i;
    int j;

    if (gNet_players[pPlayer_index].car != NULL) {
        for (i = 0; i < gCurrent_race.number_of_racers; i++) {
            if (gCurrent_race.opponent_list[i].car_spec == gNet_players[pPlayer_index].car) {
                gCurrent_race.number_of_racers--;
                for (j = i; j < gCurrent_race.number_of_racers; j++) {
                    gCurrent_race.opponent_list[j] = gCurrent_race.opponent_list[j + 1];
                }
            }
        }
        PHILRemoveObject(gNet_players[pPlayer_index].car->collision_info);
        if (gProgram_state.racing != 0) {
            RemoveCarFromEffects(gNet_players[pPlayer_index].car);
        }
        DisposeCar(gNet_players[pPlayer_index].car, gNet_players[pPlayer_index].car_index);
        if (pPlayer_index != gThis_net_player_index) {
            BrMemFree(gNet_players[pPlayer_index].car);
            gNet_players[pPlayer_index].car = NULL;
        }
        if (gAction_replay_mode != 0) {
            ToggleReplay(NULL, NULL);
        }
        ARResetPiping();
    }
}

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName) {

    return PDNetJoinGame(pGame_details, pName);
}

// NetJoinGame

// FUNCTION: CARMA2_HW 0x0049ee20
void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}

// NetExtractGameID

// NetExtractPlayerID

// FUNCTION: CARMA2_HW 0x0049ee50
int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {

    if (gNet_mode == eNet_mode_none && !gJoin_list_mode) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
}

// FUNCTION: CARMA2_HW 0x0049eea0
int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();

        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == pPlayer_id) {
                return PDNetSendMessageToAddress(pNet_game, pMessage, &gNet_players[i].pd_net_info);
            }
        }
    }
    return -3;
}
// NetSendMessageToPlayer

// FUNCTION: CARMA2_HW 0x0049f0d0
int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (gNet_mode != eNet_mode_none) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();
        return PDNetSendMessageToAddress(pDetails, pMessage, &pDetails->pd_net_info);
    } else {
        return -3;
    }
}

// FUNCTION: CARMA2_HW 0x0049f2c0
int C2_HOOK_FASTCALL NetSendMessageToAllPlayers(tNet_game_details* pNet_game, tNet_message* pMessage) {
    int i;
    int j;
    int contents_size;
    tPlayer_ID player_id;
    tNet_game_details* game;
    tNet_message* message;
    tU8* destination;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);

    contents_size = pMessage->header.field_0x16 - sizeof(tNet_message_header);
    pMessage->contents.raw.header.size = (tU8)contents_size;
    i = 0;
    if (0 < gNumber_of_net_players) {
        do {
            if (i != gThis_net_player_index) {
                if (&gNet_players[i].field_0xcc == &gNet_players[gThis_net_player_index].field_0xcc) {
                    PDFatalError("Trying to send network message to self");
                }
                message = gNet_players[i].field_0xcc;
                if (message != NULL && message->header.field_0x16 + contents_size > 0x200) {
                    player_id = gNet_players[i].ID;
                    game = gCurrent_net_game;
                    if (gNet_mode != eNet_mode_none) {
                        message->header.field_0x8 = gLocal_net_ID;
                        message->header.field_0x10 = PDGetTotalTime();
                        for (j = 0; j < gNumber_of_net_players; j++) {
                            if (gNet_players[j].ID == player_id) {
                                PDNetSendMessageToAddress(game, message, &gNet_players[j].pd_net_info);
                                break;
                            }
                        }
                    }
                    gNet_players[i].field_0xcc = message = NULL;
                }
                if (message == NULL) {
                    message = gNet_players[i].field_0xcc = NetAllocateMessage(0x200);
                    message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
                    message->header.field_0x14 = 0;
                }
                destination = (tU8*)message + message->header.field_0x16;
                message->header.field_0x14++;
                message->header.field_0x16 += (tU16)contents_size;
                if (destination != NULL) {
                    memcpy(destination, &pMessage->contents, contents_size);
                }
            }
            i++;
        } while (i < gNumber_of_net_players);
    }
    if (pMessage->header.field_0x14 != 0xff) {
        pMessage->contents.raw.header.type = 0xff;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049f440
tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tS32 pSize_decider) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// NetGetMessageSize

// based on ApexWorm/rec2 7e7ab41cc
// FUNCTION: CARMA2_HW 0x0049f650
tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2) {
    tNet_message* the_message;
    tU32 the_size;

    the_size = NetGetContentsSize(pArg1, pArg2) + sizeof(tNet_message_header);
    the_message = NetAllocateMessage(the_size);
    if (the_message != NULL) {
        the_message->header.field_0x14 = 1;
        the_message->header.field_0x16 = (tU16)the_size;
        the_message->contents.raw.header.type = pArg1;
    }
    return the_message;
}

// STUB: CARMA2_HW 0x0049f680
// STUB: CARMA2_SW 0x0049b800
tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5140
void* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pNet_player, int pSize) {
    tNet_message* message;
    tU8* contents;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);

    if (pNet_player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = pNet_player->field_0xcc;
    if (pNet_player->field_0xcc != NULL && pNet_player->field_0xcc->header.field_0x16 + pSize > 512) {
        NetReallySendMessageToPlayer(gCurrent_net_game, pNet_player->field_0xcc, pNet_player->ID);
        pNet_player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        message = pNet_player->field_0xcc = NetAllocateMessage(512);
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    contents = (tU8*)message + message->header.field_0x16;
    message->header.field_0x16 += (tU16)pSize;
    message->header.field_0x14++;
    return contents;
}

// NetGetToPlayerContents

// NetGetToHostContents

// NetStartBroadcastContents

// NetBroadcastContents

// based on ApexWorm/rec2 b994fa681
// FUNCTION: CARMA2_HW 0x0049fcf0
void C2_HOOK_FASTCALL NetSendMessageStacks(void) {
    int i;

    gLast_flush_message = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].field_0xcc != NULL) {
            NetReallySendMessageToPlayer(gCurrent_net_game, gNet_players[i].field_0xcc, gNet_players[i].ID);
            gNet_players[i].field_0xcc = NULL;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049fdb0
tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize) {
    NOT_IMPLEMENTED();
    return NULL;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL NetFreeExcessMemory(void) {

    while (gDynamic_messages != NULL && *(&gDynamic_messages->message.contents.raw.header.type + gMessage_header_size) == eNetMsg_none) {
        tNet_message_memory* next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

void C2_HOOK_FASTCALL DisposeExcessMemory(void) {
    tNet_message_memory* next;

    while (gDynamic_messages != NULL) {
        next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

// FUNCTION: CARMA2_HW 0x0049ff50
int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (pMessage->header.field_0x14 == 0xff) {
        return -1;
    }
    pMessage->contents.raw.header.type = eNetMsg_none;
    return 0;
}

// NetReallyDisposeMessage

// NetGetNextMessage

// ReceivedSendMeDetails

// ReceivedDetails

// SendOutPlayerList

// ReceivedJoin

// KickPlayerOut

// ReceivedLeave

// NetFullScreenMessage

// HostHasBittenTheDust

// ReceivedHosticide

// ReceivedNewPlayerList

// ReceivedRaceOver

// ReceivedStatusReport

// ReceivedGroupStatus

// ReceivedRepositionCar

// ReceivedStartRace

// ReceivedGuaranteeReply

// ReceivedHeadup

// ReceivedHostQuery

// ReceivedHostReply

// ReceivedGuaranteedMessage

// PlayerIsInList

// ReceivedTimeSync

// ReceivedConfirm

// ReceivedDisableCar

// ReceivedEnableCar

// ReceivedActivateLift

// ReceivedScores

// ReceivedWasted

// ReceivedCarDetailsReq

// ReceivedCarDetails

// ReceivedGameScores

// ProcessGuaranteeStuff

// ReceivedMessage

// STUB: CARMA2_HW 0x004a4a40
void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a4ac0
void C2_HOOK_FASTCALL BroadcastStatus(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a4da0
void C2_HOOK_FASTCALL CheckForDisappearees(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a5240
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a5280
void C2_HOOK_FASTCALL NetService(int pIn_race) {
    tU32 time;

    if (gIn_net_service || gNet_service_disable) {
        return;
    }
    time = PDGetTotalTime();
    gIn_net_service = 1;
    if (gJoin_list_mode) {
        NetFreeExcessMemory();
        DoNextJoinPoll();
        NetReceiveAndProcessMessages();
    } else {
        if (gNet_mode != eNet_mode_none) {
            NetFreeExcessMemory();
            if (!pIn_race) {
                NetReceiveAndProcessMessages();
            }
            if (time - gLast_status_broadcast > 1000) {
                gLast_status_broadcast = PDGetTotalTime();
                BroadcastStatus();
            }
            CheckForDisappearees();
            if (gNet_mode == eNet_mode_host) {
                CheckForPendingStartRace();
                CheckForNeedyEnvironmentRecipients();
            }
        }
    }
    if (gJoin_list_mode || gNet_mode != eNet_mode_none) {
        ResendGuaranteedMessages();
        if (time > gLast_flush_message + 200u || !gProgram_state.racing) {
            NetSendMessageStacks();
        }
    }
    gIn_net_service = 0;
}

// NetFinishRace

// FUNCTION: CARMA2_HW 0x004a57e0
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
    tNet_message* message;
    tNet_message_chunk* chunk;
    int size;
    int i;
    int j;
    int csize;
    tU8* contents;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (gNet_players[gThis_net_player_index].player_status == pNew_status) {
        return;
    }
    gNet_players[gThis_net_player_index].player_status = pNew_status;
    /* BroadcastStatus inlined */
    if (gNet_mode == eNet_mode_client) {
        size = NetGetContentsSize(0xb, 0) + 0x18;
        message = NetAllocateMessage(size);
        if (message != NULL) {
            message->header.field_0x14 = 1;
            message->header.field_0x16 = (tU16)size;
            message->contents.raw.header.type = 0xb;
        }
        *(tU16*)((tU8*)message + 0x1a) = (tU16)gNet_players[gThis_net_player_index].player_status;
        NetSendMessageToHost(gCurrent_net_game, message);
    } else if (gNet_mode == eNet_mode_host) {
        size = NetGetContentsSize(0x2e, gNumber_of_net_players) + 0x18;
        message = NetAllocateMessage(size);
        if (message != NULL) {
            message->header.field_0x14 = 1;
            message->header.field_0x16 = (tU16)size;
            message->contents.raw.header.type = 0x2e;
        }
        *(tU32*)((tU8*)message + 0x1c) = 0;
        *((tU8*)message + 0x20) = (tU8)gNumber_of_net_players;
        for (i = 0; i < gNumber_of_net_players; i++) {
            *(tU32*)((tU8*)message + 0x1c) += gNet_players[i].ID;
            *((tU8*)message + 0x21 + i) = (tU8)gNet_players[i].player_status;
        }
        chunk = &message->contents;
        csize = (tU16)message->header.field_0x16 - 0x18;
        chunk->raw.header.size = (tU8)csize;
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (j != gThis_net_player_index) {
                contents = (tU8*)NetGetToPlayerContentsSize(&gNet_players[j], csize);
                if (contents != NULL) {
                    memcpy(contents, chunk, csize);
                }
            }
        }
        NetDisposeMessage(gCurrent_net_game, message);
    }
    /* NetSendMessageStacks inlined */
    gLast_flush_message = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].field_0xcc != NULL) {
            NetReallySendMessageToPlayer(gCurrent_net_game, gNet_players[i].field_0xcc, gNet_players[i].ID);
            gNet_players[i].field_0xcc = NULL;
        }
    }
    if (gProgram_state.current_car.disabled) {
        if (pNew_status >= ePlayer_status_racing && pNew_status != ePlayer_status_recovering) {
            EnableCar(&gProgram_state.current_car);
        }
    } else if (pNew_status < ePlayer_status_racing) {
        DisableCar(&gProgram_state.current_car);
    }
}

// FUNCTION: CARMA2_HW 0x004a5b30
tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void) {

    return gNet_players[gThis_net_player_index].player_status;
}

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// STUB: CARMA2_HW 0x004a5c80
// STUB: CARMA2_SW 0x004a1e00
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004a5ca0
void C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, int pArg3, void* pNotifyFail) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// NetGuaranteedSendMessageToPlayer

// FUNCTION: CARMA2_HW 0x004a6020
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress, void* pNotifyFail) {

    if (gNet_mode == eNet_mode_none && !gJoin_list_mode) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    AddToGuaranteeList(pMessage, pAddress, 0, pNotifyFail);
    return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
}

// STUB: CARMA2_HW 0x004a6080
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
    NOT_IMPLEMENTED();
}

// NetWaitForGuaranteeReplies

// FUNCTION: CARMA2_HW 0x004a6920
// FUNCTION: CARMA2_SW 0x004a2aa0
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// based on ApexWorm/rec2 8e9995ff9 (reference only)
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004a6960
// FUNCTION: CARMA2_SW 0x004a2ae0
tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer) {
    tNet_game_player_info* player;

    player = NetPlayerFromID(pPlayer);
    if (player != NULL) {
        return player->car;
    } else {
        return NULL;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a69b0
// FUNCTION: CARMA2_SW 0x004a2b30
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].car == pCar) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004fe2c0
void C2_HOOK_FASTCALL RemoveCarFromEffects(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049f9c0
tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider) {
    NOT_IMPLEMENTED();
    return NULL;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049fb70
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
