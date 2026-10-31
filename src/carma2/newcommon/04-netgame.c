#include "04-netgame.h"

#include "01-network.h"
#include "30-opponent.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "globvrpb.h"
#include "15-displays.h"
#include "41-utility.h"
#include "platform.h"
#include "30-opponent.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0075b8f8
br_pixelmap* gDigits_pix;

// GLOBAL: CARMA2_HW 0x0074a5ec
tU32 gPHIL_last_physics_tick;

// GLOBAL: CARMA2_HW 0x0068d934
int gNot_shown_race_type_headup;

// GLOBAL: CARMA2_HW 0x0068d92c
tU32 gTime_for_punishment;

// GLOBAL: CARMA2_HW 0x0079eae8
int gNet_cash_headup;

// GLOBAL: CARMA2_HW 0x0068d920
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0079eaa4
int gNet_ped_headup;

// SendShapeNumbers

// FUNCTION: CARMA2_HW 0x00497ef0
// FUNCTION: CARMA2_SW 0x00494b20
void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            pShapes->items[i].field_0x4 = 0;
            return;
        }
    }
}

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tU32 non_car_id;
    tU32 player_id;
    undefined4 field_0xc;
    tU16 flags;
    tU8 object_hierarchy[];
} tNet_message_chunk_non_car_send;

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider);
extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);

// FUNCTION: CARMA2_HW 0x00497f10
void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pPlayer_id) {
    tNet_message_chunk_non_car_send* message;
    int hierarchy_size;
    int written_size;
    int raw_flags;

    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    message = (tNet_message_chunk_non_car_send*)NetStartBroadcastContents((tNet_message_type)0x11, hierarchy_size + 0x12);
    if (message != NULL) {
        message->non_car_id = (tU16)pNon_car->car_ID;
        message->player_id = pPlayer_id;
        raw_flags = pNon_car->flags;
        message->flags = (tU16)(((raw_flags & 0xff00) != 0) * 8
                | ((char)raw_flags != '\0') * 4
                | pNon_car->collision_info->disable_move_rotate * 2
                | (pNon_car->actor->identifier[3] == '!'));
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, message->object_hierarchy, 0x48);
        if (written_size != hierarchy_size) {
            FatalError(0x72, "in non_car object data");
        }
        NetBroadcastContents((tNet_message_chunk*)message);
    }
}

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// FUNCTION: CARMA2_HW 0x00498370
int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            return (tS8)pShapes->items[i].field_0x4;
        }
    }
    return 0;
}

// AddShapeStuffToMechanicsMessage

// FUNCTION: CARMA2_HW 0x004978d0
int C2_HOOK_FASTCALL TimeToSendData(void) {
    // GLOBAL: CARMA2_HW 0x00659c30
    static tU32 last_physics_send_time = -1;

    if (gPHIL_last_physics_tick > last_physics_send_time && gPHIL_last_physics_tick < last_physics_send_time + 80) {
        return 0;
    }
    last_physics_send_time = gPHIL_last_physics_tick;
    return 1;
}
// SendCarData

// based on carpocalypse2 src/carma2/common/netgame.c:1232
// FUNCTION: CARMA2_HW 0x00498390
void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents) {
    int i;

    if (gNet_players[gThis_net_player_index].ID != pContents->player) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == pContents->player) {
                gNet_players[i].car->time_to_recover = pContents->time;
            }
        }
    }
}

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// FUNCTION: CARMA2_HW 0x00498bb0
void C2_HOOK_FASTCALL ReceivedCopInfo(void) {
}

// SendAllNonCarPositions

// based on carpocalypse2 src/carma2/common/netgame.c:910
// FUNCTION: CARMA2_HW 0x00498bc0
void C2_HOOK_FASTCALL ReceivedNonCarPosition(tNet_message_chunk_non_car_position* pContents) {
    br_actor* actor;

    actor = gProgram_state.track_spec.non_car_list[pContents->ID];
    if (actor != NULL && gNet_mode != eNet_mode_host) {
        BrMatrix34Copy(&actor->t.t.mat, &pContents->mat);
        if (pContents->flags != 0) {
            actor->identifier[3] = '!';
        }
    }
}

// based on carpocalypse2 src/carma2/common/netgame.c:923 (reference only)
typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tU32 non_car_id;
    tU32 player_id;
    undefined field_0xc[4];
    tU16 flags;
    tU8 object_hierarchy[];
} tNet_message_chunk_non_car;

extern tS8 gNon_car_spec_indices[100];
extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
extern int C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix);
extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);
extern int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data);
extern void C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode);

// FUNCTION: CARMA2_HW 0x00498c00
void C2_HOOK_FASTCALL ReceivedNonCar(tNet_message_chunk_non_car* pContents) {
    br_actor* actor;
    tNon_car_spec* non_car;
    tNon_car_spec* candidate_non_car;
    tU32 non_car_index;
    tS8 non_car_spec_index;
    tU8 column_x;
    tU8 column_z;
    float dx;
    float dy;
    float dz;

    if (pContents->non_car_id >= (tU32)gProgram_state.track_spec.count_non_cars) {
        return;
    }
    actor = gProgram_state.track_spec.non_car_list[pContents->non_car_id];
    if (actor == NULL) {
        return;
    }
    non_car = (tNon_car_spec*)actor->type_data;
    if (non_car != NULL && (non_car->driver != eDriver_4 || (tU16)non_car->car_ID != pContents->non_car_id)) {
        non_car = NULL;
    }
    if ((pContents->flags & 1) != 0) {
        actor->identifier[3] = '!';
    } else {
        actor->identifier[3] = 'x';
    }
    if (non_car == NULL) {
        if (actor->identifier[1] >= '0' && actor->identifier[1] <= '9') {
            non_car_index = actor->identifier[1] * 10 + actor->identifier[2] - 0x210;
            non_car_spec_index = gNon_car_spec_indices[non_car_index];
            if (non_car_spec_index != 0) {
                candidate_non_car = &gProgram_state.non_cars[non_car_spec_index + 0x27];
            } else {
                candidate_non_car = NULL;
            }
            dx = gProgram_state.current_car.car_master_actor->t.t.translate.t.v[0] - actor->t.t.translate.t.v[0];
            dy = gProgram_state.current_car.car_master_actor->t.t.translate.t.v[1] - actor->t.t.translate.t.v[1];
            dz = gProgram_state.current_car.car_master_actor->t.t.translate.t.v[2] - actor->t.t.translate.t.v[2];
            if ((candidate_non_car != NULL && (candidate_non_car->flags & 0x10000) != 0)
                    || dx * dx + dy * dy + dz * dz < 900.0f) {
                DoPullActorFromWorld(actor);
                non_car = (tNon_car_spec*)actor->type_data;
            }
        }
        if (non_car == NULL) {
            GetSingleMatrixFromNetworkData(pContents->object_hierarchy, &actor->t.t.mat);
            XZToColumnXZ(&column_x, &column_z, actor->t.t.translate.t.v[0], actor->t.t.translate.t.v[2], &gProgram_state.track_spec);
            if (gProgram_state.track_spec.columns[column_z][column_x].actor_0x0 == actor->parent) {
                return;
            }
            if (gProgram_state.track_spec.columns[column_z][column_x].actor_0x0 == NULL) {
                return;
            }
            BrActorRemove(actor);
            BrActorAdd(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, actor);
            return;
        }
    }
    if ((pContents->flags & 4) != 0) {
        if ((non_car->flags & 0xff) == 0) {
            non_car->flags = (non_car->flags & ~0xfe) | 1;
        }
    } else {
        non_car->flags &= ~0xff;
    }
    if ((pContents->flags & 8) != 0) {
        if ((non_car->flags & 0xff00) == 0) {
            non_car->flags = (non_car->flags & ~0xfe00) | 0x100;
        }
    } else {
        non_car->flags &= ~0xff00;
    }
    if ((pContents->flags & 2) != 0) {
        SelectCrushNetwork((undefined4*)pContents->object_hierarchy, non_car->collision_info, 2);
        non_car->collision_info->disable_move_rotate = 1;
    } else {
        memcpy((tU8*)non_car->collision_info + 0x294, pContents->object_hierarchy, GetNetworkDataSize(pContents->object_hierarchy));
        non_car->collision_info->message_time = pContents->player_id;
        non_car->collision_info->field_0x261 = 0x11;
        non_car->collision_info->disable_move_rotate = 0;
    }
}

// STUB: CARMA2_HW 0x00498e70
void C2_HOOK_FASTCALL SignalToStartRace(void) {
    NOT_IMPLEMENTED();
}

// SignalToRepositionCar

// SetUpNetCarPositions

extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL EmptyInventory(void);

// based on carpocalypse2 src/carma2/common/netgame.c:1299
// FUNCTION: CARMA2_HW 0x00499220
void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar) {

    StopCarSmokingInstantly(pCar);
    CloseDownPowerUpsForCar(pCar);
    InitialiseCar(pCar);
    TotallyRepairACar(pCar);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gTime_for_punishment = PDGetTotalTime() + 2000;
        EmptyInventory();
    }
}

// RepositionPlayer

// FUNCTION: CARMA2_HW 0x00499260
// FUNCTION: CARMA2_SW 0x00495e90
void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (!pCar->disabled) {
            pCar->disabled = 1;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] < 500.0f) {
            BrVector3Accumulate(&pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004992e0
void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (pCar->disabled) {
            pCar->disabled = 0;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] > 500.0f) {
            BrVector3Sub(&pCar->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00499360
void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits) {
    char s[256];
    char s2[256];
    // GLOBAL: CARMA2_HW 0x0068d944
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d948
    static int flash_state;

    if (gNot_shown_race_type_headup) {
        gNot_shown_race_type_headup = 0;
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(gCurrent_net_game->type + 59));
    }
    if (gTime_for_punishment != 0 && PDGetTotalTime() >= gTime_for_punishment) {
        gTime_for_punishment = 0;
        switch (gCurrent_net_game->type) {
        case 2:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(219));
            break;
        case 3:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(220));
            break;
        case 4:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(221));
            break;
        case 7:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(222));
            break;
        default:
            break;
        }
    }
    if (gNet_mode == eNet_mode_none
        || (float)gProgram_state.credits >= gRecovery_cost.initial_network[gCurrent_net_game->type]
        || Flash(200, &last_flash, &flash_state)) {
        sprintf(s, "\xf8%d\xfa %s", pCredits, GetMiscString(94));
        ChangeHeadupText(gNet_cash_headup, s);
    } else {
        ChangeHeadupText(gNet_cash_headup, "");
    }
    switch (gCurrent_net_game->type) {
    case 5:
        sprintf(s, "\xf8%d\xfa %s", gCurrent_net_game->options.starting_target, GetMiscString(241));
        break;
    case 2:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(180), gINT_0068d920);
        break;
    case 1:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(181), gCurrent_net_game->options.starting_target);
        break;
    case 7:
        TimerString(gCurrent_net_game->options.starting_target, s2, 1, 1, 0);
        sprintf(s, "%s \xf8%s\xfa", GetMiscString(182), s2);
        break;
    default:
        s[0] = '\0';
        break;
    }
    ChangeHeadupText(gNet_ped_headup, s);
    switch (gHeadup_detail_level) {
    case 0:
    case 3:
        MoveHeadupKeepOffset(gNet_cash_headup, 0x50, 0x19);
        MoveHeadupKeepOffset(gNet_ped_headup, 0x50, 0);
        break;
    case 1:
    case 4:
        MoveHeadupKeepOffset(gNet_cash_headup, 0x1e0, 0x19);
        MoveHeadupKeepOffset(gNet_ped_headup, 0x1e0, 0);
        break;
    case 2:
    case 5:
        MoveHeadupKeepOffset(gNet_cash_headup, 0x1ae, 0x3c);
        MoveHeadupKeepOffset(gNet_ped_headup, 0x1ae, 0x14);
        break;
    default:
        break;
    }
}

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

// CreateBoxes

// DrawScoreBoxes

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00499a00
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004999f0
// FUNCTION: CARMA2_SW 0x00496290
void C2_HOOK_FASTCALL DoNetScores(void) {

    DoNetScores2(0);
}

// FUNCTION: CARMA2_HW 0x0049a710
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

// EverybodysLost

// DeclareWinner

// PlayerIsIt

// CheckForVampireWinner

extern void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

// based on carpocalypse2 src/carma2/common/netgame.c:609
// FUNCTION: CARMA2_HW 0x0049b540
void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer) {
    int i;
    int infected_count;
    int winner_index;
    tPlayer_ID* player_id;
    int* player_infected;
    char message[256];

    if (pPlayer->field_0x80 == 0) {
        DisposeKevStuffCar(pPlayer->car);
        *(int*)((tU8*)pPlayer->car + 0x1bc) = 1;
        *(int*)((tU8*)pPlayer->car + 0x1b4) = 1;
        *(int*)((tU8*)pPlayer->car + 0x1b8) = 1;
        pPlayer->field_0x80 = 1;
        for (i = 0; i < 3; i++) {
            pPlayer->car->power_up_levels[i] = pPlayer->car->power_up_slots[i];
        }
        if (gNet_mode == eNet_mode_host) {
            sprintf(message, "%s", GetMiscString(0xba));
            NetSendHeadupToPlayer(message, pPlayer->ID);
            i = 0;
            if (0 < gNumber_of_net_players) {
                player_id = &gNet_players[0].ID;
                do {
                    if ((tNet_game_player_info*)(player_id - 9) != pPlayer) {
                        sprintf(message, "%s %s", pPlayer->player_name, GetMiscString(0xb9));
                        NetSendHeadupToPlayer(message, *player_id);
                    }
                    i++;
                    player_id += 0x35;
                } while (i < gNumber_of_net_players);
            }
        }
        pPlayer->score2 = -1 - GetTotalTime();
        infected_count = 0;
        i = gProgram_state.racing;
        winner_index = 0;
        if (i != 0 && gNet_mode == eNet_mode_host) {
            i = 0;
            if (0 < gNumber_of_net_players) {
                player_infected = &gNet_players[0].field_0x80;
                do {
                    if (*player_infected == 0) {
                        infected_count++;
                        winner_index = i;
                    }
                    i++;
                    player_infected += 0x35;
                } while (i < gNumber_of_net_players);
            }
            if (infected_count <= 1) {
                DeclareWinner(winner_index);
            }
        }
    }
}

// FarEnoughAway

extern tU32 gLast_it_change;
extern tNet_game_player_info* gLast_lepper;
extern int gIt_or_fox;
extern void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer);

// based on carpocalypse2 src/carma2/common/netgame.c:1074
// FUNCTION: CARMA2_HW 0x0049a990
void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer) {

    if ((int)(PDGetTotalTime() - gLast_it_change) > 500) {
        if (gLast_lepper != pPlayer) {
            gLast_it_change = PDGetTotalTime();
            if (gIt_or_fox >= 0) {
                gLast_lepper = &gNet_players[gIt_or_fox];
            }
            PlayerIsIt(pPlayer);
        }
    }
}

// SelectRandomFox

extern int gIt_or_fox;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049abf0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SendPlayerScores

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/netgame.c:600 (SendPlayerScores inlined)
// FUNCTION: CARMA2_HW 0x0049ab00
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    static tU32 last_playerscores_sent = -1;
    tNet_message_chunk* the_contents;
    int i;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            the_contents = NetStartBroadcastContents((tNet_message_type)24, 0);
            if (the_contents != NULL) {
                switch (gCurrent_net_game->type) {
                case eNet_game_type_2:
                    the_contents->scores.general_score = gINT_0068d920;
                    break;
                case eNet_game_type_6:
                    the_contents->scores.general_score = 0;
                    for (i = 0; i < gNumber_of_net_players; i++) {
                        if (gNet_players[i].field_0x80) {
                            the_contents->scores.general_score |= 1 << i;
                        }
                    }
                    break;
                case eNet_game_type_foxy:
                    the_contents->scores.general_score = gNet_players[gIt_or_fox].ID;
                    break;
                default:
                    break;
                }
                for (i = 0; i < gNumber_of_net_players; i++) {
                    the_contents->scores.scores[i] = gNet_players[i].score2;
                }
                NetBroadcastContents(the_contents);
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0049b690
void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer) {

    PossibleService();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_fight_to_death:
        pPlayer->score2 = 100;
        break;
    case eNet_game_type_checkpoint:
        pPlayer->score2 = 0xffff;
        break;
    case eNet_game_type_1:
    case eNet_game_type_2:
    case eNet_game_type_4:
    case eNet_game_type_5:
    case eNet_game_type_6:
    case eNet_game_type_foxy:
        pPlayer->score2 = 0;
        break;
    }
    pPlayer->credits = gCurrent_net_game->options.starting_credits;
    pPlayer->wasted = 0;
    pPlayer->reposition_time = 0;
}

// InitPlayers

// UseGeneralScore

extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tU8 pType, int pSize_decider, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern int gNum_active_non_cars;
extern tNon_car_spec* gActive_non_car_list[99];
extern void C2_HOOK_FASTCALL FUN_43d7d0(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL FUN_42da20(undefined4* pRecipient);
extern void C2_HOOK_FASTCALL FUN_4f0f80(tNet_game_player_info* pRecipient);

// based on carpocalypse2 src/carma2/common/netgame.c:1131
// FUNCTION: CARMA2_HW 0x0049b8d0
void C2_HOOK_FASTCALL NetSendEnvironmentChanges(tNet_game_player_info* pRecipient) {
    int i;
    char exclamation;
    br_actor** non_car_list;
    tNon_car_spec* non_car;
    tNet_message_chunk_non_car_position* message;

    exclamation = '!';
    non_car_list = gProgram_state.track_spec.non_car_list;
    for (i = 0; i < gProgram_state.track_spec.count_non_cars; i++, non_car_list++) {
        if ((*non_car_list)->identifier[3] != exclamation && (*non_car_list)->identifier[3] != '&') {
            continue;
        }
        non_car = (tNon_car_spec*)(*non_car_list)->type_data;
        if (non_car != NULL
                && non_car->driver != 0
                && (tU16)non_car->car_ID == i
                && (non_car->flags & 0x100000) != 0) {
            continue;
        }
        message = (tNet_message_chunk_non_car_position*)NetGetToPlayerContents(0x1c, 0, pRecipient->ID);
        if (message != NULL) {
            BrMatrix34Copy(&message->mat, &(*non_car_list)->t.t.mat);
            message->ID = i;
            message->flags = (*non_car_list)->identifier[3] == exclamation;
        }
    }
    SendDetachedNonCarDataToPlayer(pRecipient);
    FUN_43d7d0(pRecipient);
    FUN_42da20((undefined4*)pRecipient);
    FUN_4f0f80(pRecipient);
    NetSendMessageStacks();
}

// STUB: CARMA2_HW 0x0049b9a0
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
    NOT_IMPLEMENTED();
}

// ReceivedGameplay

// SendGameplay

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

// based on ApexWorm/rec2 704cfc73b
// FUNCTION: CARMA2_HW 0x0049bc70
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    the_message = NetBuildGuaranteedMessage(0x1b, 0);
    *(int*)((tU8*)the_message + 0x24) = pParam_2;
    *(int*)((tU8*)the_message + 0x1c) = pMess;
    *(int*)((tU8*)the_message + 0x20) = pParam_1;
    *(int*)((tU8*)the_message + 0x28) = pParam_3;
    *(int*)((tU8*)the_message + 0x2c) = pParam_4;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
}

// SendGameplayToHost

// FUNCTION: CARMA2_HW 0x0049bd00
void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {

}

// FUNCTION: CARMA2_HW 0x0049bd10
void C2_HOOK_FASTCALL DefaultNetName(void) {

    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// FUNCTION: CARMA2_HW 0x0049bde0
// FUNCTION: CARMA2_SW 0x00497f60
void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers) {
    tCar_crush_network_shapes* shapes;
    int j;

    for (; pCount > 0; pCount--, pPlayers++) {
        if (pPlayers->car != NULL && pPlayers->car->car_crush_spec != NULL) {
            shapes = pPlayers->car->car_crush_spec->network_stuff;
            for (j = 0; j < (int)REC2_ASIZE(shapes->items); j++) {
                if (shapes->items[j].network_id == 0) {
                    shapes->items[j].network_id = pPlayer_id;
                    shapes->items[j].field_0x4 = 1;
                    break;
                }
            }
        }
    }
}

// based on carpocalypse2 src/carma2/common/netgame.c:1386
// FUNCTION: CARMA2_HW 0x0049be40
void C2_HOOK_FASTCALL RemovePlayerFromShapeStatusLists(tPlayer_ID pPlayer_id, int pPlayer_count, tNet_game_player_info* pPlayers) {
    tCar_crush_network_shapes* shapes;
    int j;

    for (; pPlayer_count > 0; pPlayer_count--, pPlayers++) {
        if (pPlayers->car != NULL && pPlayers->car->car_crush_spec != NULL) {
            shapes = pPlayers->car->car_crush_spec->network_stuff;
            for (j = 0; j < (int)REC2_ASIZE(shapes->items); j++) {
                if (shapes->items[j].network_id == pPlayer_id) {
                    shapes->items[j].network_id = 0;
                    break;
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004a5b50
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tPowerup* gPowerup_array;
extern int gNumber_of_powerups;
extern tHeadup_icon gPickedup_powerups[20];
extern int gNumber_of_icons;
extern tPlayer_ID gLocal_net_ID;
extern void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar);

// based on carpocalypse2 src/carma2/common/powerups.c:1105
// FUNCTION: CARMA2_HW 0x004def00
void C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar) {
    int powerup_address;
    int message_address;
    tU32 total_time;
    int* active_powerup;
    int* picked_up_powerup;
    int powerup_offset;
    int i;
    int j;

    if (((pCar != NULL) && (pCar->driver == eDriver_local_human)) && (i = 0, 0 < gNumber_of_powerups)) {
        powerup_offset = 0;
        active_powerup = pCar->powerups;
        do {
            if ((*active_powerup != 0) && (powerup_address = (int)((char*)gPowerup_array + powerup_offset), (((tPowerup*)powerup_address)->net_type & 1) == 0)) {
                if (((tPowerup*)powerup_address)->got_time != 0 && ((tPowerup*)powerup_address)->lose_proc != NULL) {
                    ((tPowerup*)powerup_address)->lose_proc((tPowerup*)powerup_address, ((tPowerup*)powerup_address)->car);
                }
                if (((tPowerup*)powerup_address)->net_type & 0x80) {
                    ARPowerupLose((tPowerup*)powerup_address, ((tPowerup*)powerup_address)->car);
                }
                if (gNet_mode != 0) {
                    message_address = (int)NetBuildGuaranteedMessage(0x16, 0);
                    *(int*)(message_address + 0x28) = 2;
                    *(tPlayer_ID*)(message_address + 0x1c) = gLocal_net_ID;
                    *(int*)(message_address + 0x20) = (powerup_address - (int)gPowerup_array) / 0xac;
                    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, (tNet_message*)message_address, 0);
                }
                gProgram_state.current_car.powerups[(powerup_address - (int)gPowerup_array) / 0xac] = 0;
                if (((tPowerup*)powerup_address)->initial_value == 0 && (j = 0, 0 < gNumber_of_icons)) {
                    picked_up_powerup = (int*)&gPickedup_powerups[0].powerup;
                    while (j < gNumber_of_icons) {
                        if (powerup_address == *picked_up_powerup) {
                            gPickedup_powerups[j].fizzle_stage = 3;
                            gPickedup_powerups[j].fizzle_direction = -1;
                            total_time = GetTotalTime();
                            gPickedup_powerups[j].fizzle_start = total_time;
                            break;
                        }
                        j++;
                        picked_up_powerup += 5;
                    }
                }
                ((tPowerup*)powerup_address)->got_time = 0;
            }
            i++;
            active_powerup++;
            powerup_offset += 0xac;
        } while (i < gNumber_of_powerups);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id);

// based on carpocalypse2 src/carma2/common/netgame.c:304
// FUNCTION: CARMA2_HW 0x0049a9e0
void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    if (gIt_or_fox == pPlayer - gNet_players) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        StopCarBeingIt(gNet_players[i].car);
    }
    if (gCurrent_net_game->type == eNet_game_type_foxy) {
        pPlayer->car->power_up_levels[1] = 0;
    }
    DisposeKevStuffCar(pPlayer->car);
    gIt_or_fox = pPlayer - gNet_players;
    sprintf(s, "%s", GetMiscString(0xbc));
    NetSendHeadupToPlayer(s, pPlayer->ID);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (&gNet_players[i] != pPlayer) {
            sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(0xbb));
            NetSendHeadupToPlayer(s, gNet_players[i].ID);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GLOBAL: CARMA2_HW 0x006864d0
tU32 gNet_synch_start;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00499000
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tS32 pSize_decider);

// based on carpocalypse2 src/carma2/common/network.c:1732
// FUNCTION: CARMA2_HW 0x0049f6f0
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tU8 pType, int pSize_decider, tPlayer_ID pPlayer_id) {
    int i;
    int chunk_size;
    int offset;
    tNet_message* message;
    tNet_message_chunk* chunk;
    tNet_game_player_info* player;

    chunk_size = NetGetContentsSize(pType, pSize_decider);
    i = 0;
    if (gNumber_of_net_players <= 0) {
        return NULL;
    }
    for (; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer_id) {
            goto found_player;
        }
    }
    return NULL;

found_player:
    player = &gNet_players[i];
    if (player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = player->field_0xcc;
    if (message != NULL && message->header.field_0x16 + chunk_size > 0x200) {
        tPlayer_ID player_id = player->ID;
        tNet_game_details* game = gCurrent_net_game;

        if (gNet_mode != eNet_mode_none) {
            message->header.field_0x8 = gLocal_net_ID;
            message->header.field_0x10 = PDGetTotalTime();
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == player_id) {
                    goto send_message;
                }
            }
            goto clear_message;

send_message:
            PDNetSendMessageToAddress(game, message, &gNet_players[i]);
        }
clear_message:
        player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        player->field_0xcc = NetAllocateMessage(0x200);
        message = player->field_0xcc;
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    chunk = (tNet_message_chunk*)((tU8*)message + offset);
    message->header.field_0x16 += (tU16)chunk_size;
    message->header.field_0x14 += 1;
    chunk->raw.header.type = pType;
    chunk->raw.header.size = (tU8)chunk_size;
    return chunk;
}

// based on carpocalypse2 src/carma2/common/crush.c
// FUNCTION: CARMA2_HW 0x0043d5b0
void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient) {
    int i;
    tNon_car_spec** pNon_car;

    i = 0;
    if (gNum_active_non_cars > 0) {
        pNon_car = gActive_non_car_list;
        do {
            if ((*pNon_car)->driver == 2) {
                SendDetachedNonCarData(*pNon_car, -1, pRecipient);
            }
            i++;
            pNon_car++;
        } while (i < gNum_active_non_cars);
    }
}

// FUNCTION: CARMA2_HW 0x0043d7d0
void C2_HOOK_FASTCALL FUN_43d7d0(tNet_game_player_info* pRecipient) {
    int i;
    tNon_car_spec** pNon_car;

    i = 0;
    if (gNum_active_non_cars > 0) {
        pNon_car = gActive_non_car_list;
        do {
            if ((*pNon_car)->driver == 3) {
                SendSplitHalfNonCarData(*pNon_car, -1, pRecipient);
            }
            i++;
            pNon_car++;
        } while (i < gNum_active_non_cars);
    }
}

extern int gNum_active_cars;
extern tCar_spec* gActive_car_list[139];
extern void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4);

// based on ApexWorm/rec2 bf0127d92 src/carma2/newcommon/13-crush1.c
// FUNCTION: CARMA2_HW 0x0042da20
void C2_HOOK_FASTCALL FUN_42da20(undefined4* pRecipient) {
    int i;
    int j;
    tCar_spec* car;
    tCar_crush_spec* crush_spec;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1) {
            crush_spec = car->car_crush_spec;
            if (crush_spec != NULL) {
                for (j = 0; j < crush_spec->field_0x270; j++) {
                    SendSemiDetachBit(gActive_car_list[i], crush_spec->field_0x274[j].field_0x0, 0.f, pRecipient);
                }
            }
        }
    }
}

// STUB: CARMA2_HW 0x004f0f80
void C2_HOOK_FASTCALL FUN_4f0f80(tNet_game_player_info* pRecipient) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049cd90
void C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);

// based on carpocalypse2 src/carma2/common/netgame.c
// FUNCTION: CARMA2_HW 0x0043d1d0
void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) {
    int hierarchy_size;
    int contents_size;
    int message;
    int written_size;
    undefined4 car;
    tNet_message* guaranteed_message;

    guaranteed_message = NULL;
    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    contents_size = hierarchy_size + 0x22;
    if (pRecipient == NULL) {
        if (pNon_car->collision_info->disable_move_rotate == 0 || (pNon_car->flags & 0x100000) == 0) {
            message = (int)NetStartBroadcastContents(0x27, contents_size);
        } else {
            guaranteed_message = NetBuildGuaranteedMessage(0x27, contents_size);
            message = (int)guaranteed_message + 0x18;
            pNon_car->flags &= ~0x100000;
        }
    } else {
        message = (int)NetGetToPlayerContents(0x27, contents_size, pRecipient->ID);
    }
    if (message != 0) {
        *(undefined4*)(message + 0x18) = pArg2;
        *(tU16*)(message + 0x20) = (tU16)((pNon_car->actor->identifier[3] == '!') | (pNon_car->collision_info->disable_move_rotate << 1));
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, (tU8*)(message + 0x22), 0x48);
        if (written_size != hierarchy_size) {
            FatalError(0x72, "in detached bit non_car object data");
        }
        if (pNon_car->driver == 2
                && pNon_car->actor != NULL
                && (car = *(undefined4*)((tU8*)pNon_car->actor + 0x60)) != 0
                && (car = *(undefined4*)((tU8*)car + 8)) != 0
                && (car = *(undefined4*)((tU8*)car + 0x34)) != 0) {
            car = *(undefined4*)((tU8*)car + 0x2c);
        } else {
            car = 0;
        }
        *(tPlayer_ID*)(message + 4) = NetPlayerFromCar((tCar_spec*)car)->ID;
        *(tU8*)(message + 8) = *(tU8*)((tU8*)*(void**)((tU8*)*(void**)((tU8*)pNon_car->actor + 0x60) + 8) + 0x20);
        CompressVector3((tCompressed_vector3*)(message + 0xa), (br_vector3*)((tU8*)pNon_car->collision_info + 0x24), -10.f, 10.f);
        CompressVector3((tCompressed_vector3*)(message + 0x10), (br_vector3*)((tU8*)pNon_car->collision_info + 0x30), -10.f, 10.f);
        if (pRecipient == NULL) {
            if (guaranteed_message == NULL) {
                NetBroadcastContents((tNet_message_chunk*)message);
                return;
            }
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, guaranteed_message, 0);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0043d690
void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x004fe570
void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern void C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);

// based on carpocalypse2 src/carma2/common/network.c:305
// FUNCTION: CARMA2_HW 0x0049daf0
void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id) {
    if (gNet_mode != eNet_mode_none) {
        if (pPlayer_id == gLocal_net_ID) {
            if (gProgram_state.racing) {
                NewTextHeadupSlot(4, 0, 3000, -4, pMessage);
                return;
            }
        } else {
            int size_headup;
            int size_guaranteed;
            int size_headup_again;
            int full_size;
            tNet_message* message;

            size_headup = NetGetContentsSize(0xd, 0);
            size_guaranteed = NetGetContentsSize(5, 0);
            size_headup_again = NetGetContentsSize(0xd, 0);
            full_size = size_headup_again + 0x18 + size_guaranteed;
            message = NetAllocateMessage(full_size);
            if (message != NULL) {
                message->contents.raw.header.size = (tU8)size_headup;
                message->header.field_0x14 = 0xff;
                message->header.field_0x16 = (tU16)full_size;
                message->contents.raw.header.type = 0xd;
                ((tU8*)message)[0x19 + (tU8)size_headup] = 5;
                ((tU8*)message)[0x18 + message->contents.raw.header.size] = (tU8)size_guaranteed;
            }
            strcpy((char*)message + 0x1a, pMessage);
            SendGuaranteedMessageToPlayer(gCurrent_net_game, message, pPlayer_id, 0);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049a760
void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
