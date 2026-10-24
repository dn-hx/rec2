#include "71-newgame.h"

#include "01-network.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"

/* explicit initializers keep gFrontend_billboard_actors and gNet_settings adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x00763940
br_actor* gFrontend_billboard_actors[8] = { 0 };

// GLOBAL: CARMA2_HW 0x00763960
tNet_game_options gNet_settings[9] = { 0 };

/* UNCERTAIN: unknown dword after gNet_settings, only seen as its loop end; keeps the layout adjacent */
// GLOBAL: CARMA2_HW 0x00763b10
int gUNK_00763b10 = 0;

// GLOBAL: CARMA2_HW 0x0059c7e8
int gNet_grid_starts[10] = {
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
};

// SetGameTarget

// FUNCTION: CARMA2_HW 0x00468140
void C2_HOOK_FASTCALL SetOptions(tNet_game_type pGame_type, tNet_game_options* pGame_options) {

    pGame_options->show_players_on_map = gNet_settings[0].show_players_on_map;
    pGame_options->enable_text_messages = gNet_settings[0].enable_text_messages;
    pGame_options->show_powerups_on_map = gNet_settings[0].show_powerups_on_map;
    pGame_options->powerup_respawn = gNet_settings[0].powerup_respawn;
    pGame_options->open_game = gNet_settings[pGame_type + 1].open_game;
    pGame_options->starting_credits = gNet_settings[0].starting_credits;
    pGame_options->grid_start = gNet_settings[pGame_type + 1].grid_start || gNet_grid_starts[pGame_type + 1];
    pGame_options->waste_to_transfer = gNet_settings[pGame_type + 1].waste_to_transfer;
    pGame_options->random_car_choice = gNet_settings[pGame_type + 1].random_car_choice;
    pGame_options->race_sequence_type = gNet_settings[0].race_sequence_type;
    pGame_options->car_choice = gNet_settings[0].car_choice;
    pGame_options->starting_target = gNet_settings[pGame_type + 1].starting_target;

}
// ReadNetGameChoices

// FUNCTION: CARMA2_HW 0x004662c0
int C2_HOOK_FASTCALL PickARandomCar(void) {
    int i;
    int cars[2 * REC2_ASIZE(gCar_details)];
    int array_size;

    C2_HOOK_BUG_ON(REC2_ASIZE(cars) != 120);

    array_size = 0;
    for (i = 0; i < gNumber_of_racers; i++) {
        if (gCar_details[i].ownership == eCar_owner_none) {
            cars[array_size] = i;
            array_size++;
            if (gOpponents[i].network_availability == eNet_avail_all) {
                cars[array_size] = i;
                array_size++;
            }
        }
    }
    return cars[IRandomBetween(0, array_size - 1)];
}

// RequestCarDetails

// FUNCTION: CARMA2_HW 0x00466330
// FUNCTION: CARMA2_SW 0x004660c0
void C2_HOOK_FASTCALL PollCarDetails(tNet_game_details* pNet_game) {
    tNet_message* message;

    if (gReceived_car_details) {
        gReceived_car_details = 0;
        if (gNet_mode == eNet_mode_none) {
            gNet_mode = eNet_mode_thinking_about_it;
        }
        message = NetBuildGuaranteedMessage(6, 0);
        NetGuaranteedSendMessageToHost(pNet_game, message, NULL);
    }
}

// FUNCTION: CARMA2_HW 0x00466370
void C2_HOOK_FASTCALL SetNetAvailability(tNet_game_options* pOptions) {
    int i;

    for (i = 0; i < gNumber_of_racers; i++) {
        switch (gOpponents[i].network_availability) {
        case eNet_avail_never:
            gCar_details[i].ownership = eCar_owner_not_allowed;
            break;
        case eNet_avail_eagle:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_eagle) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        case eNet_avail_all:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_novelty) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004663e0
void C2_HOOK_FASTCALL DefaultNetSettings(void) {
    FILE* file;
    tNet_game_options* pOptions;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "NETDEFLT.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        return;
    }
    ReadNetworkSettings(file, &gNet_settings[0]);
    PFrewind(file);
    for (i = 1; i < (int)REC2_ASIZE(gNet_settings); i++) {
        pOptions = &gNet_settings[i];
        ReadNetworkSettings(file, pOptions);
    }
    PFfclose(file);
}

// DisposeJoinableGame

// InitGamesToJoin

// DisposeJoinList

// AddToJoinList

// FillInRaceDescription

