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

#include <stdlib.h>
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

// (reference only)
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
extern int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode);

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

extern int gINT_0074c6d4;
extern int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);
extern int C2_HOOK_FASTCALL PickARandomCar(void);
extern void C2_HOOK_FASTCALL SendCurrentPowerups(void);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

// FUNCTION: CARMA2_HW 0x00498e70
void C2_HOOK_FASTCALL SignalToStartRace(void) {
    int i;
    int j;
    int inactive;
    tU8* dst;
    tNet_message* message;

    gCurrent_net_game->no_races_yet = 0;
    gINT_0074c6d4 = 0;
    gINT_0074b780 = 1;
    message = NetBuildGuaranteedMessage(0xc, 0);
    dst = (tU8*)message + 0x1e;
    ((tU8*)message)[0x1b] = (tU8)gProgram_state.racing;
    ((tU8*)message)[0x1a] = (tU8)gCurrent_race.number_of_racers;
    for (i = 0; i < gCurrent_race.number_of_racers; i++, dst += 0x14) {
        CompressMatrix34((tCompressed_matrix3*)(dst + 2), &inactive,
            &gCurrent_race.opponent_list[i].car_spec->car_master_actor->t.t.mat);
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (gCurrent_race.opponent_list[i].car_spec == gNet_players[j].car) {
                dst[0] = (tU8)j;
                if (inactive) {
                    dst[0] |= 0x80;
                }
                break;
            }
        }
        if (gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty) {
            if (gNet_players[dst[0] & 0x7f].next_car_index < 0) {
                gNet_players[dst[0] & 0x7f].next_car_index = PickARandomCar();
                gCar_details[gNet_players[dst[0] & 0x7f].next_car_index].ownership = 1;
            }
            dst[1] = (tU8)gNet_players[dst[0] & 0x7f].next_car_index;
        }
    }
    if (gPending_race < 0) {
        gPending_race = PickNetRace(gProgram_state.current_race_index, gCurrent_net_game->options.race_sequence_type);
    }
    ((tU8*)message)[0x1c] = (tU8)gPending_race;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    if (gProgram_state.racing) {
        SendCurrentPowerups();
    }
}

// SignalToRepositionCar

// SetUpNetCarPositions

extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL EmptyInventory(void);

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

/* The net score board works on twelve 12-byte records; the qsort callbacks below
   sort them (ClipName and DrawScoreBoxes of the original are inlined into
   DoNetScores2, so they have no body of their own in this build). */
typedef struct tNet_score_entry {
    int player_index;
    int score;
    int out_of_game;
} tNet_score_entry;

// GLOBAL: CARMA2_HW 0x0068d93c
int gNet_score_scene_initialised;

// GLOBAL: CARMA2_HW 0x0068d94c
tU32 gNet_score_last_flash;

// GLOBAL: CARMA2_HW 0x0068d950
int gNet_score_flash_state;

// GLOBAL: CARMA2_HW 0x0074a71c
br_actor* gNet_score_root;

// GLOBAL: CARMA2_HW 0x0074a720
br_material* gNet_score_materials[4];

// GLOBAL: CARMA2_HW 0x0074a730
br_actor* gNet_score_camera;

// GLOBAL: CARMA2_HW 0x0074a73c
br_model* gNet_score_model;

// GLOBAL: CARMA2_HW 0x0074a740
br_actor* gNet_score_actors[12];

extern int gIt_or_fox;
extern tDR_font gFonts[28];
extern br_pixelmap* gTexture_maps[1024];
extern tCar_icon gCar_icons[128];
extern br_actor* gCar_icons_model_actor;
extern int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);
extern void C2_HOOK_FASTCALL PolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0049a330
int C2_HOOK_CDECL SortNetHeadAscending(const void* pFirst, const void* pSecond) {
    const tNet_score_entry* first;
    const tNet_score_entry* second;

    first = (const tNet_score_entry*)pFirst;
    second = (const tNet_score_entry*)pSecond;
    if (first->out_of_game != 0) {
        if (second->out_of_game != 0) {
            return first->out_of_game - second->out_of_game;
        }
        return 2147483647;
    }
    if (second->out_of_game != 0) {
        return -2147483647;
    }
    if (first->score != second->score) {
        return first->score - second->score;
    }
    return *(int*)&gNet_players[first->player_index].field_0x94
        - *(int*)&gNet_players[second->player_index].field_0x94;
}

// FUNCTION: CARMA2_HW 0x0049a3a0
int C2_HOOK_CDECL SortNetHeadDescending(const void* pFirst, const void* pSecond) {
    const tNet_score_entry* first;
    const tNet_score_entry* second;

    first = (const tNet_score_entry*)pFirst;
    second = (const tNet_score_entry*)pSecond;
    if (first->out_of_game != 0) {
        if (second->out_of_game != 0) {
            return first->out_of_game - second->out_of_game;
        }
        return 2147483647;
    }
    if (second->out_of_game != 0) {
        return -2147483647;
    }
    if (first->score != second->score) {
        return second->score - first->score;
    }
    return *(int*)&gNet_players[first->player_index].field_0x94
        - *(int*)&gNet_players[second->player_index].field_0x94;
}

// FUNCTION: CARMA2_HW 0x0049a410
void C2_HOOK_FASTCALL CreateBoxes(void) {
    br_actor** actor_ptr;
    br_material** material_ptr;
    br_pixelmap* pixelmap;
    br_material* material;
    br_camera* camera;
    int material_index;
    char name_buffer[32];

    gNet_score_root = BrActorAllocate(0, NULL);
    actor_ptr = gNet_score_actors;
    do {
        *actor_ptr = BrActorAllocate(1, NULL);
        if (*actor_ptr == NULL) {
            BrFailure("Out of memory for NetPlay");
        }
        (*actor_ptr)->render_style = 4;
        actor_ptr++;
    } while ((int)actor_ptr < (int)(gNet_score_actors + 12));

    gNet_score_model = BrModelAllocate("NetScore Model", 4, 2);
    gNet_score_model->faces[0].vertices[0] = 0;
    gNet_score_model->faces[0].vertices[1] = 1;
    gNet_score_model->faces[0].vertices[2] = 2;
    gNet_score_model->faces[1].vertices[0] = 1;
    gNet_score_model->faces[1].vertices[1] = 3;
    gNet_score_model->faces[1].vertices[2] = 2;
    gNet_score_model->faces[0].material = NULL;
    gNet_score_model->faces[1].material = NULL;
    gNet_score_model->vertices[0].p.v[0] = 0.0f;
    gNet_score_model->vertices[0].p.v[1] = 0.0f;
    gNet_score_model->vertices[0].p.v[2] = -2.0f;
    gNet_score_model->vertices[1].p.v[0] = 100.0f;
    gNet_score_model->vertices[1].p.v[1] = 0.0f;
    gNet_score_model->vertices[1].p.v[2] = -2.0f;
    gNet_score_model->vertices[2].p.v[0] = 0.0f;
    gNet_score_model->vertices[2].p.v[1] = -38.0f;
    gNet_score_model->vertices[2].p.v[2] = -2.0f;
    gNet_score_model->vertices[3].p.v[0] = 100.0f;
    gNet_score_model->vertices[3].p.v[1] = -38.0f;
    gNet_score_model->vertices[3].p.v[2] = -2.0f;
    gNet_score_model->vertices[0].map.v[0] = 0.0f;
    gNet_score_model->vertices[0].map.v[1] = 0.0f;
    gNet_score_model->vertices[1].map.v[0] = 0.78125f;
    gNet_score_model->vertices[1].map.v[1] = 0.0f;
    gNet_score_model->vertices[2].map.v[0] = 0.0f;
    gNet_score_model->vertices[2].map.v[1] = 0.296875f;
    gNet_score_model->vertices[3].map.v[0] = 0.78125f;
    gNet_score_model->vertices[3].map.v[1] = 0.296875f;
    gNet_score_model->flags |= 2;
    BrModelAdd(gNet_score_model);

    material_index = 0;
    material_ptr = gNet_score_materials;
    do {
        material_index++;
        sprintf(name_buffer, "NETBOX%i.PIX", material_index);
        pixelmap = LoadPixelmap(name_buffer);
        if (pixelmap == NULL) {
            BrFailure("Cannot find headup graphic %s", name_buffer);
        }
        BrMapAdd(pixelmap);
        material = BrMaterialAllocate("NetScore Material");
        material->colour = 0;
        material->colour_map = pixelmap;
        material->flags = 0x2800;
        BrMaterialAdd(material);
        *material_ptr = material;
        material_ptr++;
    } while ((int)material_ptr < (int)(gNet_score_materials + 4));

    gNet_score_camera = BrActorAllocate(3, NULL);
    gNet_score_camera->identifier = "Net HUD Camera";
    camera = (br_camera*)gNet_score_camera->type_data;
    camera->type = 0;
    camera->hither_z = 1.0f;
    camera->yon_z = 3.0f;
    camera->width = (float)gScreen->width;
    camera->height = (float)gScreen->height;
    BrActorAdd(gNet_score_root, gNet_score_camera);
    gNet_score_scene_initialised = 1;
}

// FUNCTION: CARMA2_HW 0x00499a00
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {
    int i;
    int score;
    int font;
    int ascending_order;
    volatile int text_offset;
    br_actor** actor_ptr;
    int index;
    tNet_score_entry* entry;
    volatile int row_y;
    int row;
    int own_row;
    int to;
    int ry;
    char* name_ptr;
    br_actor* actor;
    br_actor** box_actor_ptr;
    br_material** box_material_ptr;
    br_pixelmap* pixelmap;
    br_material* material;
    br_camera* camera;
    int material_index;
    char name_buffer[32];
    char s[256];
    char row_text[2];
    tNet_score_entry entries[12];

    ascending_order = (gCurrent_net_game->type == eNet_game_type_checkpoint
        || gCurrent_net_game->type == eNet_game_type_6);
    Flash(200, &gNet_score_last_flash, &gNet_score_flash_state);

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].player_status >= ePlayer_status_racing) {
            score = gNet_players[i].score2;
            entries[i].player_index = i;
            entries[i].score = score;
            if (abs(score) == 1000000 || (score < 0 && gCurrent_net_game->type != eNet_game_type_1)) {
                entries[i].out_of_game = *(int*)&gNet_players[i].field_0x94 + 1;
            } else {
                entries[i].out_of_game = 0;
            }
        } else {
            entries[i].player_index = -1;
            entries[i].score = ascending_order ? 1000001 : -1000001;
            entries[i].out_of_game = 1000;
        }
    }
    for (i = gNumber_of_net_players; i < 12; i++) {
        entries[i].player_index = -1;
        entries[i].score = ascending_order ? 1000001 : -1000001;
        entries[i].out_of_game = 1000;
    }

    qsort(entries, 12, sizeof(tNet_score_entry), ascending_order ? SortNetHeadAscending : SortNetHeadDescending);

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (entries[i].player_index == gThis_net_player_index) {
            own_row = i;
            break;
        }
    }

    row = 0;
    actor_ptr = gNet_score_actors;
    text_offset = 1;
    entry = entries;
    row_y = -1;
    do {
        index = entry->player_index;
        if (index >= 0) {
            *(int*)&gNet_players[index].field_0x94 = row;
        }
        if (pOnly_sort_scores == 0 && index >= 0) {
            if (own_row == row) {
                if (gNet_score_scene_initialised == 0) {
                    CreateBoxes();
                }
                if (gNet_score_scene_initialised != 0) {
                    actor = *actor_ptr;
                    actor->material = gNet_score_materials[1];
                    actor->model = gNet_score_model;
                    BrMatrix34Translate(&actor->t.t.mat, 220.f, (float)(row_y + 0xf0), 0.f);
                    BrActorAdd(gNet_score_root, actor);
                    BrZbSceneRender(gNet_score_root, gNet_score_camera, gRender_screen, gDepth_buffer);
                    BrActorRemove(actor);
                }
                font = kPolyfont_ingame_medium_orange;
            } else {
                if (gNet_score_scene_initialised == 0) {
                    gNet_score_root = BrActorAllocate(0, NULL);
                    box_actor_ptr = gNet_score_actors;
                    do {
                        *box_actor_ptr = BrActorAllocate(1, NULL);
                        if (*box_actor_ptr == NULL) {
                            BrFailure("Out of memory for NetPlay");
                        }
                        (*box_actor_ptr)->render_style = 4;
                        box_actor_ptr++;
                    } while ((int)box_actor_ptr < (int)(gNet_score_actors + 12));

                    gNet_score_model = BrModelAllocate("NetScore Model", 4, 2);
                    gNet_score_model->faces[0].vertices[0] = 0;
                    gNet_score_model->faces[0].vertices[1] = 1;
                    gNet_score_model->faces[0].vertices[2] = 2;
                    gNet_score_model->faces[1].vertices[0] = 1;
                    gNet_score_model->faces[1].vertices[1] = 3;
                    gNet_score_model->faces[1].vertices[2] = 2;
                    gNet_score_model->faces[0].material = NULL;
                    gNet_score_model->faces[1].material = NULL;
                    gNet_score_model->vertices[0].p.v[0] = 0.0f;
                    gNet_score_model->vertices[0].p.v[1] = 0.0f;
                    gNet_score_model->vertices[0].p.v[2] = -2.0f;
                    gNet_score_model->vertices[1].p.v[0] = 100.0f;
                    gNet_score_model->vertices[1].p.v[1] = 0.0f;
                    gNet_score_model->vertices[1].p.v[2] = -2.0f;
                    gNet_score_model->vertices[2].p.v[0] = 0.0f;
                    gNet_score_model->vertices[2].p.v[1] = -38.0f;
                    gNet_score_model->vertices[2].p.v[2] = -2.0f;
                    gNet_score_model->vertices[3].p.v[0] = 100.0f;
                    gNet_score_model->vertices[3].p.v[1] = -38.0f;
                    gNet_score_model->vertices[3].p.v[2] = -2.0f;
                    gNet_score_model->vertices[0].map.v[0] = 0.0f;
                    gNet_score_model->vertices[0].map.v[1] = 0.0f;
                    gNet_score_model->vertices[1].map.v[0] = 0.78125f;
                    gNet_score_model->vertices[1].map.v[1] = 0.0f;
                    gNet_score_model->vertices[2].map.v[0] = 0.0f;
                    gNet_score_model->vertices[2].map.v[1] = 0.296875f;
                    gNet_score_model->vertices[3].map.v[0] = 0.78125f;
                    gNet_score_model->vertices[3].map.v[1] = 0.296875f;
                    gNet_score_model->flags |= 2;
                    BrModelAdd(gNet_score_model);

                    material_index = 0;
                    box_material_ptr = gNet_score_materials;
                    do {
                        material_index++;
                        sprintf(name_buffer, "NETBOX%i.PIX", material_index);
                        pixelmap = LoadPixelmap(name_buffer);
                        if (pixelmap == NULL) {
                            BrFailure("Cannot find headup graphic %s", name_buffer);
                        }
                        BrMapAdd(pixelmap);
                        material = BrMaterialAllocate("NetScore Material");
                        material->colour = 0;
                        material->colour_map = pixelmap;
                        material->flags = 0x2800;
                        BrMaterialAdd(material);
                        *box_material_ptr = material;
                        box_material_ptr++;
                    } while ((int)box_material_ptr < (int)(gNet_score_materials + 4));

                    gNet_score_camera = BrActorAllocate(3, NULL);
                    gNet_score_camera->identifier = "Net HUD Camera";
                    camera = (br_camera*)gNet_score_camera->type_data;
                    camera->type = 0;
                    camera->hither_z = 1.0f;
                    camera->yon_z = 3.0f;
                    camera->width = (float)gScreen->width;
                    camera->height = (float)gScreen->height;
                    BrActorAdd(gNet_score_root, gNet_score_camera);
                    gNet_score_scene_initialised = 1;
                }
                if (gNet_score_scene_initialised != 0) {
                    actor = *actor_ptr;
                    actor->material = gNet_score_materials[0];
                    actor->model = gNet_score_model;
                    BrMatrix34Translate(&actor->t.t.mat, 220.f, (float)(row_y + 0xf0), 0.f);
                    BrActorAdd(gNet_score_root, actor);
                    BrZbSceneRender(gNet_score_root, gNet_score_camera, gRender_screen, gDepth_buffer);
                    BrActorRemove(actor);
                }
                font = kPolyfont_ingame_medium_blue;
            }
            if (gNet_score_flash_state != 0 || gCurrent_net_game->type != eNet_game_type_foxy || index != gIt_or_fox) {
                if (*(int*)&gNet_players[index].field_0x74 != 0) {
                    name_ptr = gNet_players[index].player_name;
                    while (DRTextWidth(&gFonts[6], name_ptr) > 77) {
                        name_ptr[strlen(name_ptr) - 1] = '\0';
                    }
                    *(int*)&gNet_players[index].field_0x74 = 0;
                }
                PolyFontText(gNet_players[index].player_name, 547, text_offset + 4, font, eJust_left, 0);
            }
            score = gNet_players[index].score2;
            if (abs(score) == 1000000) {
                if (gNet_score_flash_state != 0) {
                    strcpy(s, GetMiscString(173));
                } else {
                    s[0] = '\0';
                }
            } else {
                switch (gCurrent_net_game->type) {
                case eNet_game_type_6:
                    if (score == 0) {
                        sprintf(s, "%s", GetMiscString(192));
                    } else if (gNet_score_flash_state != 0) {
                        sprintf(s, "%s", GetMiscString(193));
                    } else {
                        s[0] = '\0';
                    }
                    break;
                case eNet_game_type_fight_to_death:
                    sprintf(s, "%d%%", score);
                    break;
                case eNet_game_type_1:
                case eNet_game_type_2:
                    sprintf(s, "%d", score);
                    break;
                case eNet_game_type_checkpoint:
                    sprintf(s, "%d left", score >> 16);
                    break;
                case eNet_game_type_foxy:
                    if (index == gIt_or_fox && gNet_score_flash_state == 0) {
                        s[0] = '\0';
                    } else {
                        TimerString(score, s, 0, 1, 0);
                    }
                    break;
                case eNet_game_type_5:
                    sprintf(s, "%d-%d", score / gCurrent_race.check_point_count + 1,
                        score % gCurrent_race.check_point_count + 1);
                    break;
                case eNet_game_type_4:
                    if (score >= 0) {
                        sprintf(s, "%s -%d-", GetMiscString(92), score);
                    } else if (gNet_score_flash_state != 0) {
                        sprintf(s, "%s", GetMiscString(93));
                    } else {
                        s[0] = '\0';
                    }
                    break;
                default:
                    break;
                }
            }
            row_text[0] = '0' + row;
            to = text_offset;
            row_text[1] = '\0';
            PolyFontText(row_text, 624, to + 3, kPolyfont_netpos, eJust_left, 0);
            ry = row_y;
            BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat, 610.f, (float)(ry - 0x14), 0.f);
            gCar_icons_model_actor->material->colour_map = gTexture_maps[gCar_icons[gNet_players[index].car_index].index];
            BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
            gCar_icons_model_actor->model = gCar_icons[gNet_players[index].car_index].model;
            BrActorAdd(gNet_score_camera, gCar_icons_model_actor);
            BrZbsSceneRender(gNet_score_camera, gNet_score_camera, gBack_screen, gDepth_buffer);
            BrActorRemove(gCar_icons_model_actor);
            PolyFontText(s, 547, to + 0x12, font, eJust_left, 0);
            ry -= 0x28;
            to += 0x28;
            row_y = ry;
            text_offset = to;
            actor_ptr++;
        }
        row++;
        entry++;
    } while (row < 12);
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

extern tRace_over_reason gRace_over_reason;
extern int gCount_smashable_race_targets;
extern int gINT_0074c6d4;
extern int gScore_winner;
extern void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SendCurrentPowerups(void);
extern void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);
extern int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL CancelPendingCunningStunt(void);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL EmptyInventory(void);
extern int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar);

/* The race board is a view over each net player starting at field_0x94. */
typedef struct {
    int meter;
    undefined4 field_0x04[12];
    tCar_spec* car;
    undefined4 field_0x38[39];
} tRace_board_entry;

#define gRace_board ((tRace_board_entry*)&gNet_players[0].field_0x94)

// FUNCTION: CARMA2_HW 0x0049abf0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
    extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);
    extern int C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);
    int i;
    int j;
    int knock_out_bit;
    int cars_left;
    int car_left;
    int flags;
    int score;
    int highest;
    int next_highest;
    int lowest_score;
    tNet_game_player_info* lowest_score_player;
    int player_left;
    tCar_spec* car;
    tNet_message* message;
    tS32 time;
    char s[256];

    time = GetTotalTime();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        highest = 0;
        next_highest = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 > highest) {
                next_highest = highest;
                highest = gNet_players[i].score2;
            } else if (gNet_players[i].score2 > next_highest) {
                next_highest = gNet_players[i].score2;
            }
        }
        gINT_0068d920 = (next_highest + highest + gCount_smashable_race_targets - GetRuntimeVariable(1)) / 2 + 1;
        if (gINT_0068d920 > gCurrent_net_game->options.starting_target) {
            gINT_0068d920 = gCurrent_net_game->options.starting_target;
        }
        break;
    case eNet_game_type_foxy:
        if (gIt_or_fox < 0) {
            gLast_lepper = NULL;
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (i != -1 && gRace_board[i].meter == gNumber_of_net_players - 1) {
                    goto select_initial_it;
                }
            }
            do {
                i = IRandomBetween(0, gNumber_of_net_players - 1);
            } while (i == -1 && !gRace_board[i].car->knackered);
select_initial_it:
            PlayerIsIt(&gNet_players[i]);
        }
        if (gLast_lepper != NULL && gIt_or_fox >= 0) {
            br_vector3 difference;

            difference.v[0] = gLast_lepper->car->pos.v[0] - gRace_board[gIt_or_fox].car->pos.v[0];
            difference.v[1] = gLast_lepper->car->pos.v[1] - gRace_board[gIt_or_fox].car->pos.v[1];
            difference.v[2] = gLast_lepper->car->pos.v[2] - gRace_board[gIt_or_fox].car->pos.v[2];
            if (difference.v[0] * difference.v[0] + difference.v[1] * difference.v[1] + difference.v[2] * difference.v[2] >= 4.0) {
                gLast_lepper = NULL;
            }
        }
        break;
    default:
        break;
    }
    lowest_score = 9999;
    lowest_score_player = NULL;
    cars_left = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        car = gRace_board[i].car;
        if (gNet_players[i].reposition_time != 0
                && time >= (int)gNet_players[i].reposition_time
                && (!gRace_finished || gRace_over_reason == eRace_not_over_yet)) {
            gNet_players[i].wasted = 0;
            gNet_players[i].reposition_time = 0;
            switch (gCurrent_net_game->type) {
            case eNet_game_type_2:
            case eNet_game_type_checkpoint:
            case eNet_game_type_4:
                BrMatrix34Copy(&car->car_master_actor->t.t.mat, (br_matrix34*)gRace_board[i].field_0x04);
                break;
            default:
                SetInitialPosition(&gCurrent_race, gNet_players[i].opponent_list_index, gNet_players[i].grid_index);
                break;
            }
            StopCarSmokingInstantly(car);
            CloseDownPowerUpsForCar(car);
            InitialiseCar(car);
            TotallyRepairACar(car);
            if (car != NULL && car->driver == eDriver_local_human) {
                gTime_for_punishment = PDGetTotalTime() + 2000;
                EmptyInventory();
            }
            SetFlipUpCar(car);
            car->collision_info->field_0x49c = GetRaceTime() + 100;
            gINT_0074c6d4 = 0;
            gINT_0074b780 = 1;
            message = NetBuildGuaranteedMessage(0x32, 0);
            *(int*)((tU8*)message + 0x1c) = gProgram_state.racing;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            ((tU8*)message)[0x24] = (tU8)i;
            CompressMatrix34((tCompressed_matrix3*)((tU8*)message + 0x26), (int*)((tU8*)message + 0x20), &gCurrent_race.opponent_list[gNet_players[i].opponent_list_index].car_spec->car_master_actor->t.t.mat);
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
            if (gProgram_state.racing) {
                SendCurrentPowerups();
            }
            if (car != NULL && car->driver == eDriver_local_human) {
                CancelPendingCunningStunt();
                gProgram_state.credits = gCurrent_net_game->options.starting_credits;
            }
        }
        if (gNet_players[i].last_waste_message != 0
                && !gNet_players[i].wasteage_attributed
                && time - gNet_players[i].last_waste_message > 500) {
            sprintf(s, "%s %s", gNet_players[i].player_name, GetMiscString(0xac));
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            if (gCurrent_net_game->type == eNet_game_type_1) {
                gNet_players[i].score2--;
            }
            NetSendHeadupToEverybody(s);
        }
        SetKnackeredFlag(car);
        if (car->knackered && !gNet_players[i].wasted) {
            gNet_players[i].wasted = 1;
            message = NetBuildGuaranteedMessage(0x19, 0);
            *(int*)((tU8*)message + 0x20) = -1;
            *(int*)((tU8*)message + 0x1c) = gNet_players[i].ID;
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
            switch (gCurrent_net_game->type) {
            case eNet_game_type_fight_to_death:
                cars_left = 0;
                for (j = 0; j < gNumber_of_net_players; j++) {
                    if (!gNet_players[j].wasted) {
                        cars_left++;
                        car_left = j;
                    }
                }
                gNet_players[i].score += (&gScore_winner)[-cars_left];
                if (cars_left == 1) {
                    DeclareWinner(car_left);
                } else if (cars_left <= 0) {
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        gNet_players[j].field_0x7c++;
                        message = NetBuildGuaranteedMessage(0xa, 0);
                        *(int*)((tU8*)message + 0x1c) = 9;
                        SendGuaranteedMessageToPlayer(gCurrent_net_game, message, gNet_players[j].ID, 0);
                    }
                }
                break;
            case eNet_game_type_1:
            case eNet_game_type_4:
            case eNet_game_type_5:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_2:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                break;
            case eNet_game_type_checkpoint:
                if ((gNet_players[i].score2 >> 16) != gCurrent_race.check_point_count) {
                    knock_out_bit = IRandomBetween(0, gCurrent_race.check_point_count - 1);
                    while ((gNet_players[i].score2 & (1 << knock_out_bit)) != 0) {
                        knock_out_bit++;
                        if (knock_out_bit >= gCurrent_race.check_point_count) {
                            knock_out_bit = 0;
                        }
                    }
                    gNet_players[i].score2 |= 1 << knock_out_bit;
                }
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_6:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                if (gNet_players[i].field_0x80 == 0) {
                    PlayerIsInfected(&gNet_players[i]);
                }
                break;
            case eNet_game_type_foxy:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                if (gNumber_of_net_players > 1 && gIt_or_fox == i) {
                    gLast_lepper = NULL;
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        if (j != i && gRace_board[j].meter == gNumber_of_net_players - 1) {
                            goto select_wasted_it;
                        }
                    }
                    do {
                        j = IRandomBetween(0, gNumber_of_net_players - 1);
                    } while (j == i && !gRace_board[j].car->knackered);
select_wasted_it:
                    PlayerIsIt(&gNet_players[j]);
                }
                break;
            }
        }
        switch (gCurrent_net_game->type) {
        case eNet_game_type_fight_to_death:
            if (car->knackered) {
                gNet_players[i].score2 = -1000000;
            } else {
                gNet_players[i].score2 = PercentageCarHealth(car);
            }
            break;
        case eNet_game_type_2:
            if (gNet_players[i].score2 >= gINT_0068d920 && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_checkpoint:
            score = 0;
            gNet_players[i].score2 = gNet_players[i].score2 & 0xffff;
            flags = gNet_players[i].score2;
            for (j = 0; j < gCurrent_race.check_point_count; j++) {
                if ((flags & 1) != 0) {
                    score++;
                }
                flags >>= 1;
            }
            gNet_players[i].score2 |= score << 16;
            if (!score && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_4:
            if (gNet_players[i].score2 >= 0) {
                if (gNet_players[i].score2 < lowest_score) {
                    lowest_score = gNet_players[i].score2;
                    lowest_score_player = &gNet_players[i];
                } else if (gNet_players[i].score2 == lowest_score) {
                    lowest_score_player = NULL;
                }
            }
            break;
        case eNet_game_type_foxy:
            if (i == gIt_or_fox && !gCountdown && !gRace_finished) {
                gNet_players[i].score2 += gFrame_period;
                if (gNet_players[i].score2 >= gCurrent_net_game->options.starting_target) {
                    DeclareWinner(i);
                }
            }
            break;
        default:
            break;
        }
    }
    if (gCurrent_net_game->type == eNet_game_type_4
            && lowest_score_player != NULL
            && lowest_score_player->score2 >= 0) {
        player_left = -1;
        cars_left = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 >= 0 && &gNet_players[i] != lowest_score_player) {
                cars_left++;
                if (player_left == -1) {
                    player_left = i;
                } else {
                    player_left = -2;
                }
            }
        }
        if (cars_left) {
            lowest_score_player->car->knackered = 1;
            lowest_score_player->wasted = 1;
            lowest_score_player->score2 += (&gScore_winner)[-cars_left];
            lowest_score_player->score2 = -1;
            if (player_left == -1) {
                for (i = 0; i < gNumber_of_net_players; i++) {
                    gNet_players[i].field_0x7c++;
                    message = NetBuildGuaranteedMessage(0xa, 0);
                    *(int*)((tU8*)message + 0x1c) = 9;
                    SendGuaranteedMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID, 0);
                }
            } else if (player_left >= 0) {
                DeclareWinner(player_left);
            } else {
                message = NetBuildGuaranteedMessage(0x1b, 0);
                *(int*)((tU8*)message + 0x1c) = 2;
                *(int*)((tU8*)message + 0x20) = 0;
                *(int*)((tU8*)message + 0x24) = 0;
                *(int*)((tU8*)message + 0x28) = 0;
                *(int*)((tU8*)message + 0x2c) = 0;
                SendGuaranteedMessageToPlayer(gCurrent_net_game, message, lowest_score_player->ID, 0);
                sprintf(s, "%s %s", lowest_score_player->player_name, GetMiscString(0xb8));
                NetSendHeadupToEverybody(s);
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SendPlayerScores

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// (SendPlayerScores inlined)
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

// FUNCTION: CARMA2_HW 0x0049b9a0
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
    int i;
    int j;
    int* needy;
    tNet_game_player_info* player;
    br_actor** non_car_list;
    tNon_car_spec* non_car;
    tNet_message_chunk_non_car_position* message;

    i = 1;
    if (1 < gNumber_of_net_players) {
        needy = (int*)((tU8*)&gNet_players[1] + 0x58);
        do {
            player = (tNet_game_player_info*)((tU8*)needy - 0x58);
            if (*needy != 0 && player->player_status == ePlayer_status_racing) {
                non_car_list = gProgram_state.track_spec.non_car_list;
                for (j = 0; j < gProgram_state.track_spec.count_non_cars; j++, non_car_list++) {
                    if ((*non_car_list)->identifier[3] != '!' && (*non_car_list)->identifier[3] != '&') {
                        continue;
                    }
                    non_car = (tNon_car_spec*)(*non_car_list)->type_data;
                    if (non_car != NULL
                            && non_car->driver != 0
                            && (tU16)non_car->car_ID == j
                            && (non_car->flags & 0x100000) != 0) {
                        continue;
                    }
                    message = (tNet_message_chunk_non_car_position*)NetGetToPlayerContents(0x1c, 0, player->ID);
                    if (message != NULL) {
                        BrMatrix34Copy(&message->mat, &(*non_car_list)->t.t.mat);
                        message->ID = j;
                        message->flags = (*non_car_list)->identifier[3] == '!';
                    }
                }
                SendDetachedNonCarDataToPlayer(player);
                FUN_43d7d0(player);
                FUN_42da20((undefined4*)player);
                FUN_4f0f80(player);
                NetSendMessageStacks();
                *needy = 0;
                NetSendMessageStacks();
            }
            i++;
            needy = (int*)((tU8*)needy + 0xd4);
        } while (i < gNumber_of_net_players);
    }
}

// ReceivedGameplay

// FUNCTION: CARMA2_HW 0x0049bc20
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    extern int C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);
    tNet_message* the_message;

    the_message = NetBuildGuaranteedMessage(0x1b, 0);
    *(int*)((tU8*)the_message + 0x20) = pParam_1;
    *(int*)((tU8*)the_message + 0x24) = pParam_2;
    *(int*)((tU8*)the_message + 0x1c) = pMess;
    *(int*)((tU8*)the_message + 0x28) = pParam_3;
    *(int*)((tU8*)the_message + 0x2c) = pParam_4;
    SendGuaranteedMessageToPlayer(gCurrent_net_game, the_message, pPlayer, 0);
}

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
extern int C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);

// FUNCTION: CARMA2_HW 0x004a5b50
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    int i;
    int result;

    result = 0;
    if (gNumber_of_net_players == 1) {
        pMessage->contents.raw.header.type = 0xff;
        return result;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            result |= SendGuaranteedMessageToPlayer(pDetails, pMessage, gNet_players[i].ID, pNotifyFail);
        }
    }
    return result;
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
extern int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);
extern void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag);
extern int gInitialised_grid;

// FUNCTION: CARMA2_HW 0x00499000
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    int i;
    int j;
    int k;
    int grid_index;
    int racer_count;

    DisableNetService();
    if (!gInitialised_grid) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gNet_players[i].grid_position_set = 0;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
        }
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        gCurrent_race.opponent_list[i].index = -1;
        gCurrent_race.opponent_list[i].ranking = IRandomBetween(0, 99);
        gCurrent_race.opponent_list[i].car_spec = gNet_players[i].car;
        gCurrent_race.opponent_list[i].net_player_index = i;
        gNet_players[i].opponent_list_index = i;
    }
    racer_count = gNumber_of_net_players;
    if (!gInitialised_grid && gCurrent_net_game->options.grid_start) {
        qsort(gCurrent_race.opponent_list, gNumber_of_net_players, sizeof(tOpp_spec), SortGridFunction);
    }
    gCurrent_race.number_of_racers = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[gCurrent_race.opponent_list[i].net_player_index].opponent_list_index = i;
    }
    for (i = 0; i < racer_count; i++) {
        if ((gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver == eDriver_oppo
                && !gInitialised_grid)
            || (gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver >= eDriver_net_human
                && !gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set)) {
            grid_index = -1;
            for (j = 0; j < 12 && grid_index < 0; j++) {
                grid_index = j;
                for (k = 0; k < racer_count; k++) {
                    if (k != i
                            && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_position_set
                            && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_index == j) {
                        grid_index = -1;
                        break;
                    }
                }
            }
            if (grid_index < 0) {
                FatalError(0x6e);
            }
            SetInitialPosition(&gCurrent_race, i, grid_index);
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_index = grid_index;
            if (!gInitialised_grid) {
                gCurrent_race.number_of_racers = i + 1;
            } else {
                InitialiseCar2(gCurrent_race.opponent_list[i].car_spec, 0);
            }
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set = 1;
        }
    }
    gCurrent_race.number_of_racers = racer_count;
    gInitialised_grid = 1;
    ReenableNetService();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tS32 pSize_decider);

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

extern int gCount_host_smashes;
extern tU8* gNet_host_smashes;
extern int C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id);

// FUNCTION: CARMA2_HW 0x004f0f80
void C2_HOOK_FASTCALL FUN_4f0f80(tNet_game_player_info* pRecipient) {
    tNet_message* message;
    tU8 field_0x18;
    tU8 field_0x19;
    int i;
    int offset;

    message = NetBuildMessage(0x21, 0);
    field_0x18 = ((tU8*)message)[0x18];
    field_0x19 = ((tU8*)message)[0x19];
    i = 0;
    if (0 < gCount_host_smashes) {
        offset = 0;
        do {
            memcpy((tU8*)message + 0x18, gNet_host_smashes + offset, 0x38);
            ((tU8*)message)[0x18] = field_0x18;
            *(tU32*)((tU8*)message + 0x4c) |= 1;
            ((tU8*)message)[0x19] = field_0x19;
            NetSendMessageToPlayer(gCurrent_net_game, message, pRecipient->ID);
            i++;
            offset += 0x38;
        } while (i < gCount_host_smashes);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int C2_HOOK_FASTCALL SetObjectNetworkStuff(undefined4* pField, tPhysics_object* pObj, int pMode);
extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

// FUNCTION: CARMA2_HW 0x0049cd90
int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode) {
    int total_size;
    tU8* current_data;
    tPhysics_object* matching_child;
    tPhysics_object* child;
    tPhysics_object* recursive_child;

    if (pObj != NULL && ((tU8*)pField)[1] != pObj->uid) {
        PhysicsError((tPhysicsError)9, "netphys got messed up");
    }
    total_size = SetObjectNetworkStuff(pField, pObj, pMode);
    if ((((tU8*)pField)[0] & 0x80) != 0) {
        do {
            current_data = (tU8*)pField + total_size;
            if (pObj != NULL) {
                child = pObj->child;
                while (child != NULL && child->uid != current_data[1]) {
                    child = child->next;
                }
                matching_child = child;
            } else {
                matching_child = NULL;
            }
            if (pObj != NULL) {
                child = pObj->child;
                while (child != NULL && child->uid != current_data[1]) {
                    child = child->next;
                }
                recursive_child = child;
            } else {
                recursive_child = NULL;
            }
            total_size += SelectCrushNetwork((undefined4*)current_data, recursive_child, pMode);
            if (matching_child != NULL) {
                matching_child->field_0xf0 = pObj->field_0xf0;
            }
        } while ((current_data[0] & 0x40) != 0);
    }
    return total_size;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pGame, tNet_message* pMessage, int pUnused);

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
// FUNCTION: CARMA2_HW 0x0043d690
void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) {
    tU8* message;
    int hierarchy_size;
    int contents_size;
    int written_size;
    tCar_spec* car;
    tNet_message* guaranteed_message;
    br_vector3* crush_vector;

    guaranteed_message = NULL;
    car = NULL;
    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    contents_size = hierarchy_size + 0x16;
    if (pRecipient == NULL) {
        if (pNon_car->collision_info->disable_move_rotate == 0 || (pNon_car->flags & 0x100000) == 0) {
            message = (tU8*)NetStartBroadcastContents(0x29, contents_size);
        } else {
            guaranteed_message = NetBuildGuaranteedMessage(0x29, contents_size);
            message = (tU8*)guaranteed_message + 0x18;
            pNon_car->flags &= ~0x100000;
        }
    } else {
        message = (tU8*)NetGetToPlayerContents(0x29, contents_size, pRecipient->ID);
    }
    if (message != NULL) {
        if (pNon_car->actor != NULL) {
            car = (tCar_spec*)pNon_car->actor->user;
        }
        *(undefined4*)(message + 8) = pArg2;
        *(tPlayer_ID*)(message + 4) = NetPlayerFromCar(car)->ID;
        *(tU16*)(message + 0xc) = (tU16)(pNon_car->collision_info->disable_move_rotate << 1);
        crush_vector = &car->car_crush_spec->field_0x548;
        CompressVector3((tCompressed_vector3*)(message + 0xe), crush_vector, -1.0f, 1.0f);
        *(tU16*)(message + 0x14) = DRScalarToU16(*(float*)((tU8*)crush_vector + 0xc), -2.5f, 2.5f);
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, message + 0x16, 0x48);
        if (written_size != hierarchy_size) {
            FatalError(0x72, "in split half non_car object data");
        }
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
extern intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext);
extern int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel);
extern int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial);
extern void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);

// FUNCTION: CARMA2_HW 0x004fe570
void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {
    tUser_crush_data* user_crush_data;
    tCar_crush_spec* crush_spec;
    br_model* model;
    int i;

    if (gAFE == 0) {
        return;
    }
    crush_spec = pCar->car_crush_spec;
    model = pCar->shell_model;
    model->custom = NULL;
    model->flags &= ~BR_MODF_CUSTOM;
    user_crush_data = pCar->car_model_actor->user;
    model = user_crush_data->models[1];
    if (model != NULL) {
        model->flags &= ~BR_MODF_CUSTOM;
        model->custom = NULL;
    }
    DRActorEnumRecurse(pCar->car_model_actor, ForEveryActorModelNoGrooves, (void*)ResetFoxModelCallback);
    for (i = 0; i < crush_spec->field_0x270; i++) {
        model = crush_spec->field_0x274[i].field_0x0->model;
        model->flags &= ~BR_MODF_CUSTOM;
        model->custom = NULL;
    }
    for (i = 0; i < crush_spec->field_0x2b0; i++) {
        model = crush_spec->field_0x2b4[i].field_0x0->model;
        model->flags &= ~BR_MODF_CUSTOM;
        model->custom = NULL;
    }
    ForEveryCarMaterial(pCar, ResetFoxLighting, 1);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);

// Known remaining diff (partial, category=other): 0x49db88 SIB base/index order only
// (original [edx + ebx + 0x19], ours [ebx + edx + 0x19]); no C spelling changes MSVC's choice.
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
                *((tU8*)message + (tU8)size_headup + 0x19) = 5;
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

// GLOBAL: CARMA2_HW 0x0074c6d4
int gINT_0074c6d4;

// GLOBAL: CARMA2_HW 0x00659c2c
int gScore_winner = 20;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0049c630
int C2_HOOK_FASTCALL SetObjectNetworkStuff(undefined4* pField, tPhysics_object* pObj, int pMode) {
    NOT_IMPLEMENTED();
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// FUNCTION: CARMA2_HW 0x0043d670
void C2_HOOK_FASTCALL ToggleFlapsFromNetwork(tNet_message_chunk* pChunk) {
    extern int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data);
    extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);
    tCar_spec* car;

    car = NetCarFromPlayerID(pChunk->toggle_doors.id);
    if (car != NULL) {
        PhysicsObjectRecurse(car->collision_info, ToggleFlapsCB, car);
    }
}

// FUNCTION: CARMA2_HW 0x004983f0
void C2_HOOK_FASTCALL ReceivedCrushSync(tNet_message_chunk_car_crush_sync* pContents) {
    int i;
    int j;
    int player_id;
    tCar_spec* car;
    tCar_crush_network_shapes* network_stuff;
    tU16* pCrush_state;
    int* network_id;

    i = 0;
    if (pContents->count > 0) {
        pCrush_state = &pContents->players[0].crush_state;
        do {
            car = NetCarFromPlayerID(*((tPlayer_ID*)pCrush_state - 1));
            if (car != NULL && *pCrush_state == car->car_crush_spec->field_0x24) {
                network_stuff = car->car_crush_spec->network_stuff;
                player_id = pContents->player_id;
                if (network_stuff != NULL) {
                    j = 0;
                    network_id = (int*)network_stuff;
                    while (j < 12) {
                        if (*network_id == player_id) {
                            goto clear_network_flag;
                        }
                        j++;
                        network_id += 2;
                    }
                    goto next_crush_state;
clear_network_flag:
                    network_stuff->items[j].field_0x4 = 0;
next_crush_state:
                    ;
                }
            }
            i++;
            pCrush_state += 4;
        } while (i < pContents->count);
    }
}
