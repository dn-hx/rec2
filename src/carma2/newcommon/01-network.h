#ifndef GUARD_01_NETWORK_H
#define GUARD_01_NETWORK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int C2_HOOK_FASTCALL NetInitialise(void);

extern int C2_HOOK_FASTCALL NetShutdown(void);

extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

extern void C2_HOOK_FASTCALL DisableNetService(void);

extern void C2_HOOK_FASTCALL ReenableNetService(void);

extern int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void);

extern void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void);

// NetSendHeadupToAllPlayers

// NetSendHeadupToEverybody

// NetSendHeadupToPlayer

// InitialisePlayerStati

extern void C2_HOOK_FASTCALL LeaveTempGame(void);

extern void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void);

extern void C2_HOOK_FASTCALL DoNextJoinPoll(void);

extern void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc);
extern void C2_HOOK_FASTCALL NetEndJoinList(void);
extern void C2_HOOK_FASTCALL NetDisposePIDGameInfo(tNet_game_details* pDetails);

extern void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails);

extern tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void);

extern void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void);

extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

// NetSetPlayerSystemInfo

extern void C2_HOOK_FASTCALL FillInThisPlayer(tNet_game_details* pGame, tNet_game_player_info* pPlayer, int pCar_index, int pHost);
// LoadCarN

extern void C2_HOOK_FASTCALL DisposeCarN(int pPlayer_index);
// PlayerHasLeft

// RemoveReferencesToCar

extern void C2_HOOK_FASTCALL NetPlayersChanged(int pPlayer_count, tNet_game_player_info* pPlayers);
// NetHostGame

extern int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName);

// NetJoinGame

extern void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

// NetExtractGameID

// NetExtractPlayerID

extern int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

extern int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id);
extern int C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id);
extern int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage);

extern int C2_HOOK_FASTCALL NetSendMessageToAllPlayers(tNet_game_details* pNet_game, tNet_message* pMessage);
extern tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tS32 pSize_decider);
// NetGetMessageSize

extern tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);

extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);

// NetGetToPlayerContentsSize (defined in 01-network.c; no extern here: an extra declaration in this header shifts MSVC codegen of DistanceFromFaceND 0x005162a0)

// NetGetToPlayerContents

// NetGetToHostContents

extern tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider);
// NetBroadcastContents

extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);

extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);

extern void C2_HOOK_FASTCALL NetFreeExcessMemory(void);

extern void C2_HOOK_FASTCALL DisposeExcessMemory(void);

extern int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage);

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

extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);

extern void C2_HOOK_FASTCALL BroadcastStatus(void);

extern void C2_HOOK_FASTCALL CheckForDisappearees(void);

extern void C2_HOOK_FASTCALL CheckForPendingStartRace(void);

extern void C2_HOOK_FASTCALL NetService(int pIn_race);

// NetFinishRace

extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

extern tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void);

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

extern void C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, int pArg3, void* pNotifyFail);
// NetGuaranteedSendMessageToPlayer

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress, void* pNotifyFail);
extern void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);

// NetWaitForGuaranteeReplies

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer);

extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket


extern void* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pNet_player, int pSize);
extern int C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);
extern void C2_HOOK_FASTCALL RemoveCarFromEffects(tCar_spec* pCar);
extern tPlayer_ID C2_HOOK_FASTCALL PDNetExtractPlayerID(tNet_game_details* pDetails);
extern void C2_HOOK_FASTCALL CheckForVampireWinner(void);

#endif // GUARD_01_NETWORK_H
