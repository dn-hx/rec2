#ifndef C2_WIN32_NET_H
#define C2_WIN32_NET_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>
#include <winsock.h>

extern char gPathNetworkIni[240];
extern int gPathNetworkIniValid;
extern struct sockaddr gLocal_address;

void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr);

int C2_HOOK_FASTCALL BroadcastMessage(void);

int C2_HOOK_FASTCALL PDNetGetHeaderSize(void);

int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName);

void C2_HOOK_FASTCALL PDNetHostFinishGame(void);

void C2_HOOK_FASTCALL PDNetLeaveGame(void);

tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** gRemote_addr);

int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);


extern int C2_HOOK_FASTCALL PDNetSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer_id);
extern int C2_HOOK_FASTCALL PDNetSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage);
extern void C2_HOOK_FASTCALL PDNetDisposePlayer(void);
extern int C2_HOOK_FASTCALL PDNetInitClient(void);

#endif // C2_WIN32_NET_H