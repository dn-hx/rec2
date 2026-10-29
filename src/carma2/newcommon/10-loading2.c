#include "10-loading2.h"

#include "00-car.h"
#include "02-init.h"
#include "05-drmem.h"
#include "07-structur.h"
#include "08-loading1.h"
#include "09-funks.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "33-depth.h"
#include "37-brucetrk.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "53-controls.h"
#include "55-volume.h"
#include "57-drone.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "61-pedestrn.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "64-movie.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "71-newgame.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"
#include "51-smash.h"

extern tCar_spec* gCurrent_car_spec;
extern char gCurrent_load_name[256];
extern char gCurrent_load_directory[256];
extern tGraf_spec gGraf_specs[2];
extern int gGraf_spec_index;
extern tGraf_data* gCurrent_graf_data;
extern void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);
extern void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void);
extern void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type);
extern void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);
extern void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);
extern void C2_HOOK_FASTCALL CreateDefaultCrushData(br_actor* pActor, tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL LinkCrushDataToActors(br_actor* pActor, tCar_crush_buffer* pBuffer);
extern void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location);
extern int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec);
extern void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec);
extern intptr_t C2_HOOK_CDECL LinkCrushModel(br_actor* pActor, void* pData);
extern const char* gWheel_actor_names[6];
extern const char* gPivot_actor_names[6];
extern void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF);
extern const char* gDamage_names[12];
extern tU8 gCrush_data_entry_counter;
extern void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);

// GLOBAL: CARMA2_HW 0x0068c6ec
FILE* gTempFile;

// GLOBAL: CARMA2_HW 0x0068c718
const char* gPedTextTxtPath;

// GLOBAL: CARMA2_HW 0x0068b88c
int gKey_map_index;

#define DECODE_OFFSET 50
// GLOBAL: CARMA2_HW 0x00655e38
char gDecode_string[14] = {
    (char)('i' + DECODE_OFFSET),
    (char)(' ' + DECODE_OFFSET),
    (char)('a' + DECODE_OFFSET),
    (char)('m' + DECODE_OFFSET),
    (char)(' ' + DECODE_OFFSET),
    (char)('f' + DECODE_OFFSET),
    (char)('i' + DECODE_OFFSET),
    (char)('d' + DECODE_OFFSET),
    (char)('d' + DECODE_OFFSET),
    (char)('l' + DECODE_OFFSET),
    (char)('i' + DECODE_OFFSET),
    (char)('n' + DECODE_OFFSET),
    (char)('g' + DECODE_OFFSET),
    '\0'
};

// GLOBAL: CARMA2_HW 0x0068c6f8
char gUnderwater_screen_name[32];

// GLOBAL: CARMA2_HW 0x00764ea0
char gDefault_car[32];

// GLOBAL: CARMA2_HW 0x00764ec0
char gBasic_car_name[32];

// GLOBAL: CARMA2_HW 0x007638c0
int gNet_score_targets[8];

// GLOBAL: CARMA2_HW 0x0074b58c
int gDemo_race_rank_equivalent;

// GLOBAL: CARMA2_HW 0x0074b4fc
int gCount_demo_opponents;

// GLOBAL: CARMA2_HW 0x0074b4c0
int gDemo_opponents[15];

// GLOBAL: CARMA2_HW 0x00679308
int gCamera_type;

// GLOBAL: CARMA2_HW 0x0074b4a8
int gGroove_funk_offset;

// GLOBAL: CARMA2_HW 0x00657558
const char* gNet_avail_names[4] = {
    "never",
    "eagle",
    "hawk",
    "all",
};

// GLOBAL: CARMA2_HW 0x00657320
tHeadup_info gHeadup_image_info[45] = {
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "GO.PIX", eNet_or_otherwise },
    { "NUMBER1.PIX", eNet_or_otherwise },
    { "NUMBER2.PIX", eNet_or_otherwise },
    { "NUMBER3.PIX", eNet_or_otherwise },
    { "NUMBER4.PIX", eNet_or_otherwise },
    { "NUMBER5.PIX", eNet_or_otherwise },
    { "SPLATTER.PIX", eNet_or_otherwise },
    { "PILEDRIV.PIX", eNet_or_otherwise },
    { "EXTRASTY.PIX", eNet_or_otherwise },
    { "ARTISTIC.PIX", eNet_or_otherwise },
    { "PLAYWITH.PIX", eNet_or_otherwise },
    { "PUNT.PIX", eNet_or_otherwise },
    { "DROP.PIX", eNet_or_otherwise },
    { "POLITE.PIX", eNet_or_otherwise },
    { "2XCOMBO.PIX", eNet_or_otherwise },
    { "3XCOMBO.PIX", eNet_or_otherwise },
    { "4XCOMBO.PIX", eNet_or_otherwise },
    { "5XCOMBO.PIX", eNet_or_otherwise },
    { "6XCOMBO.PIX", eNet_or_otherwise },
    { "7XCOMBO.PIX", eNet_or_otherwise },
    { "8XCOMBO.PIX", eNet_or_otherwise },
    { "9XCOMBO.PIX", eNet_or_otherwise },
    { "10XCOMBO.PIX", eNet_or_otherwise },
    { "BILLIARD.PIX", eNet_or_otherwise },
    { "POLITE.PIX", eNet_or_otherwise },
    { "HEADON.PIX", eNet_or_otherwise },
    { "DESTROY.PIX", eNet_or_otherwise },
    { "CHECKPNT.PIX", eNet_or_otherwise },
    { "TIMEUP.PIX", eNot_net },
    { "RACEOVER.PIX", eNet_or_otherwise },
    { "UWASTED.PIX", eNet_or_otherwise },
    { "MAD.PIX", eNet_or_otherwise },
    { "GAMEOVER.PIX", eNet_or_otherwise },
    { "UBROKE.PIX", eNet_or_otherwise },
    { "ULOST.PIX", eNet_or_otherwise },
    { "UWON.PIX", eNet_or_otherwise },
    { "BBQBON.PIX", eNet_or_otherwise },
    { "TAILBON.PIX", eNet_or_otherwise },
    { "DETTAILBON.PIX", eNet_or_otherwise },
    { "MISSIONFAIL.PIX", eNet_or_otherwise },
    { "MISSIONDONE.PIX", eNet_or_otherwise },
};

// GLOBAL: CARMA2_HW 0x0079eb60
br_pixelmap* gHeadup_images[45];

void C2_HOOK_FASTCALL LoadInRegisteeDir(const char *pRoot, const char *pSubDir, int pInitialize_palettes) {
    tPath_name the_path;
    tPath_name the_path2;
    tTWTVFS twt;

    PathCat(the_path, pRoot, pSubDir);
    LoadInFiles(the_path, "PALETTES", DRLoadPalette);
    LoadInFiles(the_path, "SHADETAB", DRLoadShadeTable);

    if (pInitialize_palettes) {
        InitializePalettes();
    }

    PathCat(the_path2, the_path, "PIXELMAP");
    twt = OpenPackFileAndSetTiffLoading(the_path2);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path2);
    ClosePackFileAndSetTiffLoading(twt);

    LoadInFiles(the_path, "MATERIAL", DRLoadMaterials);
    LoadInFiles(the_path, "MODELS", DRLoadModels);
    LoadInFiles(the_path, "ACTORS", DRLoadActors);
    LoadInFiles(the_path, "LIGHTS", DRLoadLights);
}

// FUNCTION: CARMA2_HW 0x00486e10
void C2_HOOK_FASTCALL LoadInRegistees(void) {

    LoadInRegisteeDir(gApplication_path, "REG", 1);
}

void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info) {
    char s[256];
    const char *str;
    int i;

    /* (armour|power|offensive), single player, each skill level */
    GetThreeInts(gTempFile, &pSlot_info->initial[0], &pSlot_info->initial[1], &pSlot_info->initial[2]);
    /* (armour|power|offensive), each network game type */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(pSlot_info->initial_network); i++) {
        sscanf(str, "%d", &pSlot_info->initial_network[i]);
        str = strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch) {
    char s[256];
    const char *str;
    int i;

    GetThreeFloats(gTempFile, &pBunch->initial[0], &pBunch->initial[1], &pBunch->initial[2]);
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(pBunch->initial_network); i++) {
        sscanf(str, "%f", &pBunch->initial_network[i]);
        str = strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL GetHithers(void) {
    char s[256];
    int result;
    int position;
    float camera_hither;
    float cockpit_hither;
    char whitespace[] = "\t ,";

    /* Hithers, general then cockpit mode */
    GetALineAndDontArgue(gTempFile, s);
    result = sscanf(&s[strspn(s, whitespace)], "%f%n", &camera_hither, &position);
    if (result == 0) {
        FatalError(kFatalError_Mysterious_SS, s, "GENERAL.TXT");
    }
    gCamera_hither = camera_hither;
    if (sscanf(&s[position + strspn(&s[position], "\t ,")], "%f", &cockpit_hither) == 0) {
        cockpit_hither = gCamera_hither;
    }
    gCamera_hither *= 2;
    gCamera_cockpit_hither = 2 * cockpit_hither;
}

// FUNCTION: CARMA2_HW 0x00486ef0
void C2_HOOK_FASTCALL LoadGeneralParameters(void) {
    tPath_name the_path;
    int i;
    char* str;
    char s[256];
    br_scalar armour_mult, power_mult, offensive_mult;
    int time;

    PathCat(the_path, gApplication_path, "ACTORS");
    PathCat(the_path, the_path, "PROG.ACT");

    gTempFile = PFfopen(the_path, "rb");
    if (gTempFile != NULL) {
        PFfgets(s, REC2_ASIZE(s) - 1, gTempFile);
        PFfclose(gTempFile);

        for (i = 0; (unsigned)i < strlen(gDecode_string); i++) {
            gDecode_string[i] -= DECODE_OFFSET;
        }

        // trim trailing CRLF etc
        while (s[0] != '\0' && s[strlen(s) - 1] < 0x20) {
            s[strlen(s) - 1] = 0;
        }

        if (strcmp(s, gDecode_string) == 0) {
            gDecode_thing = 0;
        }

        for (i = 0; (unsigned)i < strlen(gDecode_string); i++) {
            gDecode_string[i] += DECODE_OFFSET;
        }
    }
    PathCat(the_path, gApplication_path, "GENERAL.TXT");
    gTempFile = DRfopen(the_path, "rt");
    if (gTempFile == NULL) {
        FatalError(kFatalError_FailToOpenGeneralSettings);
    }

    /* Disable TIFF conversion */
    gDisableTiffConversion = GetAnInt(gTempFile);

    GetHithers();

    /* Yon */
    gCamera_yon = GetAFloat(gTempFile);
    /* Camera angle */
    gCamera_angle = GetAFloat(gTempFile);
    /* Headup background brightness amount */
    gDim_amount = GetAnInt(gTempFile);
    /* Initial rank */
    gInitial_rank = GetAnInt(gTempFile);
    /* Credits per rank for each skill level */
    GetThreeInts(gTempFile, &gCredits_per_rank[0], &gCredits_per_rank[1], &gCredits_per_rank[2]);

    ReadCrushSettings(gTempFile);

    /* Time per ped kill for each skill level */
    GetThreeInts(gTempFile, &gTime_per_ped_kill[0], &gTime_per_ped_kill[1], &gTime_per_ped_kill[2]);
    /* Seconds per unit car damage for each skill level (with peds */
    GetThreeFloats(gTempFile, &gSeconds_per_unit_car_damage[0], &gSeconds_per_unit_car_damage[1], &gSeconds_per_unit_car_damage[2]);
    /* Credits per unit car damage for each skill level (with peds) */
    GetThreeFloats(gTempFile, &gCredits_per_unit_car_damage[0], &gCredits_per_unit_car_damage[1], &gCredits_per_unit_car_damage[2]);
    /* Time awarded for wasting car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gTime_wasting_car[0], &gTime_wasting_car[1], &gTime_wasting_car[2]);
    /* Credits awarded for wasting car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_wasting_car[0], &gCredits_wasting_car[1], &gCredits_wasting_car[2]);
    /* Time awarded for rolling car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gTime_rolling_car[0], &gTime_rolling_car[1], &gTime_rolling_car[2]);
    /* Credits awarded for rolling car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_rolling_car[0], &gCredits_rolling_car[1], &gCredits_rolling_car[2]);
    /* Credits awarded for checkpoints for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_checkpoint[0], &gCredits_checkpoint[1], &gCredits_checkpoint[2]);
    /* Jump start fine for each level */
    GetThreeInts(gTempFile, &gJump_start_fine[0], &gJump_start_fine[1], &gJump_start_fine[2]);
    /* Credits per second of time bonus */
    GetThreeInts(gTempFile, &gCredits_per_second_time_bonus[0], &gCredits_per_second_time_bonus[1], &gCredits_per_second_time_bonus[2]);
    /* Cunning stunt bonus for each skill level */
    GetThreeInts(gTempFile, &gCunning_stunt_bonus[0], &gCunning_stunt_bonus[1], &gCunning_stunt_bonus[2]);

    /* Cars to use as defaults: */
    GetAString(gTempFile, gDefault_car);
    GetAString(gTempFile, gBasic_car_name);

    gKnobbled_frame_period = 0;
    gOpponent_nastyness_frigger = 1.0f;
    /* Min time in secs after last contact with play before opponent considers repairing */
    gMinTimeOpponentRepair = (int)GetAScalar(gTempFile);
    /* Max time in secs after last contact with play before opponent considers repairing */
    gMaxTimeOpponentRepair = (int)GetAScalar(gTempFile);

    /* Default underwater special volume parameters */
    ParseSpecialVolume(gTempFile, &gDefault_default_water_spec_vol, gUnderwater_screen_name, 0);

    /* Initial armour, single player, each skill level */
    /* Initial armour, each network game type */
    LoadBunchOParameters(&gInitial_APO[0]);

    /* Initial power, single player, each skill level */
    /* Initial power, each network game type */
    LoadBunchOParameters(&gInitial_APO[1]);

    /* Initial offensive, single player, each skill level */
    /* Initial offensive, each network game type */
    LoadBunchOParameters(&gInitial_APO[2]);

    /* Initial potential armour, single player, each skill level */
    /* Initial potential armour, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[0]);

    /* Initial potential power, single player, each skill level */
    /* Initial potential power, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[1]);

    /* Initial potential offensive, single player, each skill level */
    /* Initial potential offensive, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[2]);

    /* Max armour, single player, each skill level */
    /* Max armour, each network game type */
    LoadBunchOParameters(&gMax_APO[0]);

    /* Max power, single player, each skill level */
    /* Max power, each network game type */
    LoadBunchOParameters(&gMax_APO[1]);

    /* Max offensive, single player, each skill level */
    /* Max offensive, each network game type */
    LoadBunchOParameters(&gMax_APO[2]);

    /* APO cost, single player */
    /* APO cost, each network game type */
    LoadBunchOParameters(&gCost_APO);

    /* Trade-in APO value, single player */
    /* Trade-in APO value, each network game type */
    LoadBunchOParameters(&gTrade_in_value_APO);

    /* APO substitution value, single player */
    /* APO substitution, each network game type */
    LoadBunchOParameters(&gSubstitution_value_APO);

    /* APO potential substitution value, single player */
    /* APO potential substitution, each network game type */
    LoadBunchOParameters(&gPotential_substitution_value_APO);

    /* Armour starting value */
    gArmour_starting_value[0] = GetAScalar(gTempFile);
    /* Power starting value */
    gPower_starting_value[0] = GetAScalar(gTempFile);
    /* Offensive starting value */
    gOffensive_starting_value[0] = GetAScalar(gTempFile);
    /* Armour per-level multiplier */
    armour_mult = GetAScalar(gTempFile);
    /* Power per-level multiplier */
    power_mult = GetAScalar(gTempFile);
    /* Offensive per-level multiplier */
    offensive_mult = GetAScalar(gTempFile);
    for (i = 1; i < (int)REC2_ASIZE(gArmour_starting_value); i++) {
        gArmour_starting_value[i] = gArmour_starting_value[i - 1] * armour_mult;
        gPower_starting_value[i] = gPower_starting_value[i - 1] * power_mult;
        gOffensive_starting_value[i] = gOffensive_starting_value[i - 1] * offensive_mult;
    }

    /* Powerup number to use when time powerup got during network game */
    gNet_powerup_time_replacement = GetAnInt(gTempFile);

    /* Starting money for each skill level */
    GetThreeInts(gTempFile, &gStarting_money[0], &gStarting_money[1], &gStarting_money[2]);
    /* Starting money in network mode */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
#if defined(REC2_FIX_BUGS)
    for (i = 0; i < (int)REC2_ASIZE(gNet_starting_money); i++) {
#else
    for (i = 0; i < 5; i++) {
#endif
        sscanf(str, "%d", &gNet_starting_money[i]);
        str = strtok(NULL, "\t ,/");
    }

    /* Repair cost for each skill level (cred per % damage) */
    /* Repair cost for each net game (cred per % damage) */
    LoadBunchOFloatParameters(&gRepair_cost);

    /* Recovery cost for each skill level */
    /* Recovery cost for each net game type */
    LoadBunchOFloatParameters(&gRecovery_cost);

    /* Car softness factor for each net skill level */
    /* Car softness factor for each net game type */
    LoadBunchOFloatParameters(&gCar_softness);

    /* Car-to-car damage multiplier for each net skill level */
    /* Car-to-car damage multiplier for each net game type */
    LoadBunchOFloatParameters(&gCar_car_damage_multiplier);

    /* Score targets for each net game type */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(gNet_score_targets); i++) {
        sscanf(str, "%d", &gNet_score_targets[i]);
        str = strtok(NULL, "\t ,/");
    }

    /* Pickup respawn min time (seconds) */
    gPickup_respawn_min_time_ms = 1000 * GetAnInt(gTempFile);
    /* Pickup respawn max extra time (seconds) */
    gPickup_respawn_max_extra_time_ms = 1000 * GetAnInt(gTempFile);

    /* Demo race rank equivalent */
    gDemo_race_rank_equivalent = GetAnInt(gTempFile);

    /* Number of demo opponents */
    gCount_demo_opponents = GetAnInt(gTempFile);
    /* Demo opponents */
    for (i = 0; (int)i < gCount_demo_opponents; i++) {
        gDemo_opponents[i] = GetAnInt(gTempFile);
    }

    /* default Gravity Multiplier */
    gDefault_gravity = GetAScalar(gTempFile);

    /* Flic sound delays */
    /* Delay (in seconds) before sound during pre-smack flic */
    gFlic_sound_delay_pre_smack = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during post-smack flic */
    gFlic_sound_delay_post_smack = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during 'not in demo' flic */
    gFlic_sound_delay_not_in_demo = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during post-demo slideshow flic */
    gFlic_sound_delay_post_demo = GetAFloat(gTempFile);

    /* Time (in seconds) that credits take before they reach the recovery
     * amount when self-increasing (if starting at zero)
     *
     * First line is for single-player games, second is for each type of
     * network game. Zero means don't tick up.
     */
    time = 1000 * GetAnInt(gTempFile);
    for (i = 0; i < (int)REC2_ASIZE(gAuto_increase_credits_dt); i++) {
        gAuto_increase_credits_dt[i] = (int)((float)time / (0.02f * (float)gRecovery_cost.initial[i]));
    }
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(gNet_score_targets); i++) {
        int t;
        sscanf(str, "%d", &t);
        gNet_auto_increase_credits_dt[i] = (int)((float)(1000 * t) / (.02f * (float)gRecovery_cost.initial_network[i]));
        str = strtok(NULL, "\t ,/");
    }

    /* Mutant tail thing settings */
    /* Number of links including ball */
    gCount_mutant_tail_parts = GetAnInt(gTempFile);
    /* Mass of each link */
    gMass_mutant_tail_link = GetAFloat(gTempFile);
    /* Mass of ball */
    gMass_mutant_tail_ball = GetAFloat(gTempFile);

    /*  Mine / Mortar weight */
    gMass_mine = GetAFloat(gTempFile);
}

// FUNCTION: CARMA2_HW 0x00487dc0
// FUNCTION: CARMA2_SW 0x00484ef0
void C2_HOOK_FASTCALL FinishLoadingGeneral(void) {
    gDefault_default_water_spec_vol.screen_pixelmap = BrMapFind(gUnderwater_screen_name);
    LoadWastedExplosion(gTempFile, &gWasted_explosion_chance, &gExplosion_sound_id, &gExplosion_pix_animation_groups);
    ReadPowerupSmash(gTempFile);
    PFfclose(gTempFile);
}

// FUNCTION: CARMA2_HW 0x00487e10
void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "KEYMAP_X.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }

    for (i = 0; i < (int)REC2_ASIZE(gKey_mapping); i++) {
        fscanf((FILE*)f, "%d", &gKey_mapping[i]);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00487ea0
void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00487eb0
void C2_HOOK_FASTCALL InitInterfaceLoadState(void) {

    // empty
}

// AdjustCarCoordinates

// FUNCTION: CARMA2_HW 0x0048b560
void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        pCar_spec->speedo_radius_2[pIndex] = -1;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        if (pCar_spec->speedo_image[pIndex] == NULL) {
            FatalError(kFatalError_CannotLoadSpeedoImage);
        }
        pCar_spec->speedo_y_pitch[pIndex] = pCar_spec->speedo_image[pIndex]->height / 10;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x_pitch[pIndex]);
    } else {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_centre_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_centre_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_radius_1[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_radius_2[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_start_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_end_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_needle_colour[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->max_speed);
    }
}

// FUNCTION: CARMA2_HW 0x0048b810
void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    /* tacho type, x, y, filename, x-pitch */
    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        pCar_spec->tacho_radius_2[pIndex] = -1;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
    } else {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_centre_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_centre_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_radius_1[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_radius_2[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_start_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_end_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_needle_colour[pIndex]);
    }
}

// FUNCTION: CARMA2_HW 0x0048ba60
// FUNCTION: CARMA2_SW 0x00488ca0
void C2_HOOK_FASTCALL LoadHeadups(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    int j;
    int number_of_slots;

    number_of_slots = GetAnInt(pF);
    for (j = 0; j < number_of_slots; j++) {
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].field_0x28);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].x);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].y);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].colour);
        str = strtok(NULL, "\t ,/");
        strcpy(s, str);
        switch (s[0]) {
        case 'l':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_left;
            break;
        case 'r':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_right;
            break;
        case 'c':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_centre;
            break;
        }
        if (s[1] == 'c') {
            pCar_spec->headup_slots[pIndex][j].cockpit_anchored = 1;
        }
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_left);
        if (pCar_spec->headup_slots[pIndex][j].dim_left >= 0) {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 1;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_top);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_right);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_bottom);
        } else {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 0;
        }
    }
}

// OldReadNonCarMechanicsData

// GetPerpendicular

// ReadNonCarMechanicsData

// ReadPartHeader

// ReadMechanicsShapes

// NewReadMechanicsData

// LoadGear

// AddRefOffset

// GetDamageProgram

// FUNCTION: CARMA2_HW 0x0048bf20
intptr_t C2_HOOK_CDECL AllocateCarActorData(br_actor* pActor, void* pData) {

    C2_HOOK_BUG_ON(sizeof(tUser_crush_data) != 0x10);

    if (pActor->type == BR_ACTOR_MODEL) {
        pActor->user = BrMemAllocate(sizeof(tUser_crush_data), kMem_crush_data);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048bf50
// FUNCTION: CARMA2_SW 0x00489190
intptr_t C2_HOOK_CDECL AllocateCarModelData(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;
    tCar_spec* car_spec;
    tUser_detail_level_model* model_data;
    int i;

    C2_HOOK_BUG_ON(sizeof(tUser_detail_level_model) != 0xc);

    car_spec = pData;
    user_crush = pActor->user;
    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                model_data = BrMemAllocate(sizeof(tUser_detail_level_model), kMem_crush_data);
                user_crush->models[i]->user = model_data;
            }
        }
    }
    return 0;
}

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

extern void C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);

// GLOBAL: CARMA2_HW 0x006572f0
tU32 gSimple_material_colours[12] = { 0x005d0303, 0x02a56e04, 0x03a9a905, 0x08006868, 0x0d3c3626, 0x0e212121, 0x15284c21, 0x1900326a, 0x1a93491d, 0x1ec0bfc0, 0x23141414, 0x00000000, };

// GLOBAL: CARMA2_HW 0x0074b500
br_material* gSimple_materials[32];

// FUNCTION: CARMA2_HW 0x00488dd0
br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type) {
    int i;
    int j;
    int r, g, b;
    br_colour colour;

    if (pName[0] == 'M') {
        int code;
        strtok(pName, "\t ,/");
        code = 10 * (pName[1] - '0') + pName[2] - '0';
        for (j = 0; gSimple_material_colours[j] != 0; j++) {
            if ((int)(gSimple_material_colours[j] >> 24) == code) {
                break;
            }
        }
        if (gSimple_material_colours[j] == 0) {
            PDFatalError("Obsolete simple material");
        }
        colour = gSimple_material_colours[j] & 0xffffff;
    } else {
        sscanf(pName, "%d,%d,%d", &r, &g, &b);
        colour = (((r << 8) | g) << 8) | b;
    }

    for (i = 0; gSimple_materials[i] != NULL; i++) {
        if (gSimple_materials[i]->colour == colour) {
            return gSimple_materials[i];
        }
    }
    if (i < 32) {
        gSimple_materials[i] = BrMaterialAllocate("Simple");
        gSimple_materials[i]->colour = colour;
        gSimple_materials[i]->index_base = FindBestColourMatch(gSimple_materials[i]->colour);
        gSimple_materials[i]->index_range = 0;
        GlorifyMaterial(&gSimple_materials[i], 1, pShading_type);
        BrMaterialAdd(gSimple_materials[i]);
    }
    return gSimple_materials[i];
}

// ReadShrapnelMaterials

// SetModelFlags

// FindCarShellModel

// FUNCTION: CARMA2_HW 0x0048c130
intptr_t C2_HOOK_CDECL LinkCarActorsToGroovidelicsCB(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL) {
        user_crush->groove = ActorsGroove(pActor);
    }
    return 0;
}

// LinkCarActorsToGroovidelics

// ReadCarKeywordData

// TrashModelMaterialIdentifiers

// CentreModel

// PivotizeWheel

// SetUpGraphicalWheelStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00488f70
void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space) {
    tBrender_storage* prev_storage;
    char s[256];
    tPath_name the_path;
    tPath_name car_path;
    char mech_s[256];
    char gear_s[256];
    tCar_crush_buffer car_crush_buffer;
    char actor_path[256];
    char load_name[256];
    char load_directory[256];
    tTWTVFS twt;
    FILE* f;
    FILE* g;
    FILE* h;
    int i;
    int j;
    int k;
    int version;
    int old_model_count;
    int count_vertices;
    int all_fire_zero;
    float temp_float;
    char* str;
    br_model* model;
    tUser_crush_data* user_crush;
    br_material* car_material;
    tCrush_model_pool model_pool;
    v11model* v11;
    br_scalar red_gear_speed;
    br_scalar acceleration_highest_gear;
    float ixp;
    float iyp;
    float izp;
    int count_wheels;
    br_vector3 wpos;
    int steer_flags;
    br_scalar susp_give;
    br_scalar damping;
    br_scalar friction_slipping_reduction;
    br_scalar friction_angle_1;
    br_scalar friction_angle_2;
    br_scalar traction_multiplier;
    br_scalar rolling_resistance;
    br_scalar friction_steer;
    br_scalar friction_non_steer_1;
    br_scalar friction_non_steer_2;
    int wpos_i;
    br_actor* actor;
    br_model* wmodel;

    C2_HOOK_BUG_ON(sizeof(*pCar_spec) != 6500);
    memset(pCar_spec, 0, sizeof(*pCar_spec));

    prev_storage = gStorage_for_callbacks;
    gCurrent_car_spec = pCar_spec;
    if (pDriver == eDriver_local_human) {
        strcpy(gProgram_state.car_name, pCar_name);
    }
    strcpy(gCurrent_load_directory, "CARS");
    strcpy(gCurrent_load_name, pCar_name);
    gCurrent_load_name[strlen(gCurrent_load_name) - 4] = '\0';
    strcpy(load_directory, gCurrent_load_directory);
    strcpy(load_name, gCurrent_load_name);
    PathCat(car_path, gApplication_path, load_directory);
    PathCat(car_path, car_path, load_name);
    twt = OpenPackFileAndSetTiffLoading(car_path);

    pCar_spec->is_girl = 0;
    pCar_spec->driver = pDriver;
    pCar_spec->index = pOwner;

    if (pDriver == eDriver_local_human) {
        gFunk_groove_flags[0] = 1;
        gGroove_funk_offset = 0;
    } else {
        gGroove_funk_offset = -1;
        for (i = 1; i < REC2_ASIZE(gFunk_groove_flags); i++) {
            if (gFunk_groove_flags[i] == 0) {
                pCar_spec->fg_index = i;
                gFunk_groove_flags[i] = 1;
                gGroove_funk_offset = GROOVE_FUNK_MAX_PER_CAR * i;
                break;
            }
        }
    }
    if (gGroove_funk_offset < 0) {
        FatalError(kFatalError_RanOutOfFunkGrooveSlotBunches);
    }
    if (strcmp(pCar_name, "STELLA.TXT") == 0) {
        pCar_spec->proxy_ray_distance = 6.0f;
    } else {
        pCar_spec->proxy_ray_distance = 0.0f;
    }
    PathCat(the_path, car_path, pCar_name);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }
    strcpy(the_path, car_path);
    PathCat(the_path, the_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, pCar_name);
    AllowOpenToFail();
    g = DRfopen(the_path, "rt");
    DoNotAllowOpenToFail();
    if (g == NULL) {
        PathCat(the_path, gApplication_path, "CARS");
        PathCat(the_path, the_path, gGraf_specs[gGraf_spec_index].data_dir_name);
        PathCat(the_path, the_path, gBasic_car_name);
        g = DRfopen(the_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CannotOpenCarResolutionDependentFile);
        }
    }

    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    if (strcmp(str, "VERSION") == 0) {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &version);
        GetAString(f, s);
    } else {
        version = 0;
    }
    if (strcmp(s, "BOY") == 0) {
        pCar_spec->is_girl = 0;
        GetAString(f, s);
    } else if (strcmp(s, "GIRL") == 0) {
        pCar_spec->is_girl = 1;
        GetAString(f, s);
    }
    strcpy(pCar_spec->name, s);
    if (DRStricmp(s, pCar_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pCar_name);
    }
    if (pDriver_name[0] != '\0') {
        memcpy(pCar_spec->driver_name, pDriver_name, REC2_ASIZE(pCar_spec->driver_name));
        pCar_spec->driver_name[REC2_ASIZE(pCar_spec->driver_name) - 1] = '\0';
    } else {
        strcpy(pCar_spec->driver_name, "X");
    }
    pCar_spec->can_be_stolen = 0;
    pCar_spec->has_been_stolen = 0;
    pCar_spec->knackered = 0;
    pCar_spec->time_last_hit = 0;
    pCar_spec->time_last_victim = 0;
    pCar_spec->disabled = 0;
    pCar_spec->active = 1;
    for (i = 0; i < REC2_ASIZE(pCar_spec->power_up_levels); i++) {
        pCar_spec->power_up_levels[i] = gCurrent_APO_levels[i];
        pCar_spec->power_up_slots[i] = gCurrent_APO_potential_levels[i];
    }
    pCar_spec->softness_factor = GetAFloat(f);
    GetALineAndDontArgue(f, s);
    if (pDriver == eDriver_local_human) {
        if (twt >= 0) {
            PackFileRevertTiffLoading();
        }
        for (i = 0; i < REC2_ASIZE(pCar_spec->cockpit_images); i++) {
            GetALineAndDontArgue(g, s);
            strtok(s, "\t ,/");
            pCar_spec->cockpit_images[i] = NULL;
            GetALineAndDontArgue(g, s);
            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_left[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_top[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_right[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_bottom[i]);
            PossibleService();
        }
        LoadSpeedo(g, 0, pCar_spec);
        GetALineAndDontArgue(g, s);
        PossibleService();
        LoadTacho(g, 0, pCar_spec);
        GetALineAndDontArgue(g, s);
        PossibleService();
        GetALineAndDontArgue(g, gear_s);
        str = strtok(gear_s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->gear_x[0]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->gear_y[0]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->gears_image = LoadPixelmap(str);
        GetALineAndDontArgue(g, s);
        PossibleService();
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->number_of_hands_images);
        for (i = 0; i < pCar_spec->number_of_hands_images; i++) {
            GetALineAndDontArgue(g, s);
            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &pCar_spec->lhands_x[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->lhands_y[i]);
            str = strtok(NULL, "\t ,/");
            pCar_spec->lhands_images[i] = LoadPixelmap(str);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->rhands_x[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->rhands_y[i]);
            str = strtok(NULL, "\t ,/");
            PossibleService();
        }
        pCar_spec->red_line = 8000;
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%f", &pCar_spec->driver_x_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->driver_y_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->driver_z_offset);
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%f", &pCar_spec->head_left_angle);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->head_right_angle);
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%f", &pCar_spec->mirror_x_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->mirror_y_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->mirror_z_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->rearview_camera_angle);
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_left);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_top);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_right);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_bottom);
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_left);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_top);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_right);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_bottom);
        GetALineAndDontArgue(f, s);
        PossibleService();
        str = strtok(s, "\t ,/");
        pCar_spec->prat_cam_left = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_top = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_right = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_bottom = LoadPixelmap(str);
        PossibleService();
        for (i = 0; i < REC2_ASIZE(pCar_spec->damage_units); i++) {
            if (i == eDamage_driver) {
                pCar_spec->damage_units[i].images = NULL;
            } else {
                GetALineAndDontArgue(g, s);
                str = strtok(s, "\t ,/");
                sscanf(str, "%d", &pCar_spec->damage_units[i].x_coord);
                str = strtok(NULL, "\t ,/");
                sscanf(str, "%d", &pCar_spec->damage_units[i].y_coord);
                for (k = 0; k < REC2_ASIZE(pCar_spec->damage_units[i].periods); k++) {
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%f", &temp_float);
                    pCar_spec->damage_units[i].periods[k] = (int)(1000.0 / temp_float / 2.0);
                }
                str = strtok(NULL, "\t ,/");
                pCar_spec->damage_units[i].images = LoadPixelmap(str);
                if (pCar_spec->damage_units[i].images == NULL) {
                    FatalError(kFatalError_CannotLoadDamageImage);
                }
            }
            pCar_spec->damage_units[i].damage_level = 0;
            PossibleService();
        }
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_x_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_y_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_background_x);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_background_y);
        str = strtok(NULL, "\t ,/");
        pCar_spec->damage_background = LoadPixelmap(str);
        pCar_spec->dim_count[0] = GetAnInt(g);
        for (i = 0; i < REC2_ASIZE(pCar_spec->dim_count); i++) {
            for (j = 0; j < pCar_spec->dim_count[i]; j++) {
                GetFourInts(g,
                    &pCar_spec->dim_left[i][j],
                    &pCar_spec->dim_top[i][j],
                    &pCar_spec->dim_right[i][j],
                    &pCar_spec->dim_bottom[i][j]);
            }
        }
        PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
        PathCat(the_path, the_path, "HEADUP.TXT");
        h = DRfopen(the_path, "rt");
        if (h == NULL) {
            FatalError(kFatalError_CannotOpenHeadupsFile);
        }
        PossibleService();
        LoadHeadups(h, 0, pCar_spec);
        LoadHeadups(h, 1, pCar_spec);
        PossibleService();
        PFfclose(h);
        for (i = 0; i < REC2_ASIZE(gProgram_state.current_car.render_left); i++) {
            gProgram_state.current_car.render_left[i] -= gCurrent_graf_data->cock_margin_x;
            gProgram_state.current_car.render_top[i] -= gCurrent_graf_data->cock_margin_y;
            gProgram_state.current_car.render_right[i] -= gCurrent_graf_data->cock_margin_x;
            gProgram_state.current_car.render_bottom[i] -= gCurrent_graf_data->cock_margin_y;
        }
        gProgram_state.current_car.mirror_left -= gCurrent_graf_data->cock_margin_x;
        gProgram_state.current_car.mirror_top -= gCurrent_graf_data->cock_margin_y;
        gProgram_state.current_car.mirror_right -= gCurrent_graf_data->cock_margin_x;
        gProgram_state.current_car.mirror_bottom -= gCurrent_graf_data->cock_margin_y;
        gProgram_state.current_car.speedo_centre_x[1] -= gCurrent_graf_data->cock_margin_x;
        gProgram_state.current_car.speedo_centre_y[1] -= gCurrent_graf_data->cock_margin_y;
        gProgram_state.current_car.tacho_centre_x[1] -= gCurrent_graf_data->cock_margin_x;
        gProgram_state.current_car.tacho_centre_y[1] -= gCurrent_graf_data->cock_margin_y;
        gProgram_state.current_car.speedo_x[1] -= gCurrent_graf_data->cock_margin_x;
        gProgram_state.current_car.speedo_y[1] -= gCurrent_graf_data->cock_margin_y;
        gProgram_state.current_car.tacho_x[1] -= gCurrent_graf_data->cock_margin_x;
        gProgram_state.current_car.tacho_y[1] -= gCurrent_graf_data->cock_margin_y;
        for (i = 0; i < REC2_ASIZE(gProgram_state.current_car.lhands_x); i++) {
            gProgram_state.current_car.lhands_x[i] -= gCurrent_graf_data->cock_margin_x;
            gProgram_state.current_car.lhands_y[i] -= gCurrent_graf_data->cock_margin_y;
            gProgram_state.current_car.rhands_x[i] -= gCurrent_graf_data->cock_margin_x;
            gProgram_state.current_car.rhands_y[i] -= gCurrent_graf_data->cock_margin_y;
        }
        for (i = 0; i < REC2_ASIZE(gProgram_state.current_car.damage_units); i++) {
            gProgram_state.current_car.damage_units[i].x_coord -= gCurrent_graf_data->cock_margin_x;
            gProgram_state.current_car.damage_units[i].y_coord -= gCurrent_graf_data->cock_margin_y;
        }
        AdjustRenderScreenSize();
        PossibleService();
        ReinitialiseRearviewCamera();
        GetALineAndDontArgue(f, s);
        if (twt >= 0) {
            PackFileRerevertTiffLoading();
        }
    } else {
        do {
            GetALineAndDontArgue(f, s);
            if (PFfeof(f) != 0) {
                break;
            }
        } while (strcmp(s, "END OF DRIVABLE STUFF") != 0);
        pCar_spec->red_line = 8000;
    }
    PossibleService();
    GetThreeInts(f,
        &pCar_spec->engine_noises[0],
        &pCar_spec->engine_noises[1],
        &pCar_spec->engine_noises[2]);
    GetAString(f, s);
    pCar_spec->can_be_stolen = (strcmp(s, "stealworthy") == 0);
    GetDamageProgram(f, pCar_spec, eImpact_top);
    GetDamageProgram(f, pCar_spec, eImpact_bottom);
    GetDamageProgram(f, pCar_spec, eImpact_left);
    GetDamageProgram(f, pCar_spec, eImpact_right);
    GetDamageProgram(f, pCar_spec, eImpact_front);
    GetDamageProgram(f, pCar_spec, eImpact_back);
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    strcpy(pCar_spec->grid_icon_names[0], str);
    str = strtok(NULL, "\t ,/");
    strcpy(pCar_spec->grid_icon_names[1], str);
    str = strtok(NULL, "\t ,/");
    strcpy(pCar_spec->grid_icon_names[2], str);
    pCar_spec->grid_icon_image = NULL;

    LoadAllImagesInDirectory(pStorage_space, car_path);
    LoadAllShadeTablesInDirectory(pStorage_space, car_path);
    LoadCarMaterials(pStorage_space, car_path, pCar_spec);
    old_model_count = pStorage_space->models_count;
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->count_detail_levels);
    pCar_spec->count_detail_levels++;
    if (pCar_spec->count_detail_levels > REC2_ASIZE(((tUser_crush_data*)0)->models)) {
        PDFatalError("Too many levels of detail");
    }
    pCar_spec->detail_levels[0] = 0.f;
    for (i = 1; i < pCar_spec->count_detail_levels; i++) {
        pCar_spec->detail_levels[i] = GetAScalar(f);
    }
    LoadAllModelsInDirectory(pStorage_space, car_path);
    if (version > 0) {
        PossibleService();
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        PathCat(the_path, car_path, str);
        if (LoadCarCrush(&car_crush_buffer, the_path, pStorage_space, &pCar_spec->car_crush_spec) != 0) {
            PathCat(the_path, car_path, pCar_name);
            strcpy(&the_path[strlen(the_path) - 3], "WAM");
            LoadCarCrush(&car_crush_buffer, the_path, pStorage_space, &pCar_spec->car_crush_spec);
        }
    }
    pCar_spec->car_master_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrActorAdd(gNon_track_actor, pCar_spec->car_master_actor);
    if (pDriver == eDriver_local_human) {
        gPlayer_car_master_actor = pCar_spec->car_master_actor;
    }
    PathCat(actor_path, car_path, load_name);
    strcat(actor_path, ".ACT");
    pCar_spec->car_model_actor = BrActorLoad(actor_path);
    if (pCar_spec->car_model_actor == NULL) {
        FatalError(kFatalError_CannotLoadCarActor);
    }
    BrActorAdd(pCar_spec->car_master_actor, pCar_spec->car_model_actor);
    DRActorEnumRecurse(pCar_spec->car_model_actor, AllocateCarActorData, NULL);
    model_pool.models = &pStorage_space->models[old_model_count];
    model_pool.model_count = pStorage_space->models_count - old_model_count;
    model_pool.count_detail_levels = pCar_spec->count_detail_levels;
    DRActorEnumRecurse(pCar_spec->car_model_actor, LinkCrushModel, &model_pool);
    model = NULL;
    for (i = old_model_count; i < pStorage_space->models_count; i++) {
        if (strcmp(pStorage_space->models[i]->identifier, "SHELL") == 0) {
            model = pStorage_space->models[i];
            break;
        }
    }
    pCar_spec->shell_model = model;
    pCar_spec->car_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    user_crush = pCar_spec->car_model_actor->user;
    if (user_crush == NULL || user_crush->models[0] == NULL) {
        PDFatalError("Can't find main car model");
    }
    pCar_spec->car_actor->model = user_crush->models[0];
    BrMatrix34Copy(&pCar_spec->car_actor->t.t.mat, &pCar_spec->car_model_actor->t.t.mat);
    if (version > 0) {
        LinkCrushDataToActors(pCar_spec->car_model_actor, &car_crush_buffer);
    }
    DRActorEnumRecurse(pCar_spec->car_model_actor, AllocateCarModelData, pCar_spec);
    PossibleService();
    for (i = old_model_count; i < pStorage_space->models_count; i++) {
        wmodel = pStorage_space->models[i];
        if (wmodel != NULL && wmodel->nfaces != 0) {
            if (pOwner == 0x62 || gAusterity_mode) {
                if (!(wmodel->flags & BR_MODF_UPDATEABLE)) {
                    continue;
                }
                wmodel->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
            } else {
                wmodel->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE;
            }
            BrModelUpdate(wmodel, BR_MODU_ALL);
        }
    }
    GetAString(f, s);
    if (version > 1) {
        GetAnInt(f);
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->number_of_steerable_wheels);
    for (i = 0; i < pCar_spec->number_of_steerable_wheels; i++) {
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->steering_ref[i]);
        if (pCar_spec->steering_ref[i] >= 0) {
            pCar_spec->steering_ref[i] += gGroove_funk_offset;
        }
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->lf_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->lf_sus_ref[i]);
        if (pCar_spec->lf_sus_ref[i] >= 0) {
            pCar_spec->lf_sus_ref[i] += gGroove_funk_offset;
        }
        str = strtok(NULL, "\t ,/");
    }
    PossibleService();
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->rf_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->rf_sus_ref[i]);
        if (pCar_spec->rf_sus_ref[i] >= 0) {
            pCar_spec->rf_sus_ref[i] += gGroove_funk_offset;
        }
        str = strtok(NULL, "\t ,/");
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->lr_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->lr_sus_ref[i]);
        if (pCar_spec->lr_sus_ref[i] >= 0) {
            pCar_spec->lr_sus_ref[i] += gGroove_funk_offset;
        }
        str = strtok(NULL, "\t ,/");
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->rr_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->rr_sus_ref[i]);
        if (pCar_spec->rr_sus_ref[i] >= 0) {
            pCar_spec->rr_sus_ref[i] += gGroove_funk_offset;
        }
        str = strtok(NULL, "\t ,/");
    }
    GetFourInts(f,
        &pCar_spec->driven_wheels_spin_ref_1,
        &pCar_spec->driven_wheels_spin_ref_2,
        &pCar_spec->driven_wheels_spin_ref_3,
        &pCar_spec->driven_wheels_spin_ref_4);
    if (pCar_spec->driven_wheels_spin_ref_1 >= 0) {
        pCar_spec->driven_wheels_spin_ref_1 += gGroove_funk_offset;
    }
    if (pCar_spec->driven_wheels_spin_ref_2 >= 0) {
        pCar_spec->driven_wheels_spin_ref_2 += gGroove_funk_offset;
    }
    if (pCar_spec->driven_wheels_spin_ref_3 >= 0) {
        pCar_spec->driven_wheels_spin_ref_3 += gGroove_funk_offset;
    }
    if (pCar_spec->driven_wheels_spin_ref_4 >= 0) {
        pCar_spec->driven_wheels_spin_ref_4 += gGroove_funk_offset;
    }
    PossibleService();
    GetFourInts(f,
        &pCar_spec->non_driven_wheels_spin_ref_1,
        &pCar_spec->non_driven_wheels_spin_ref_2,
        &pCar_spec->non_driven_wheels_spin_ref_3,
        &pCar_spec->non_driven_wheels_spin_ref_4);
    if (pCar_spec->non_driven_wheels_spin_ref_1 >= 0) {
        pCar_spec->non_driven_wheels_spin_ref_1 += gGroove_funk_offset;
    }
    if (pCar_spec->non_driven_wheels_spin_ref_2 >= 0) {
        pCar_spec->non_driven_wheels_spin_ref_2 += gGroove_funk_offset;
    }
    if (pCar_spec->non_driven_wheels_spin_ref_3 >= 0) {
        pCar_spec->non_driven_wheels_spin_ref_3 += gGroove_funk_offset;
    }
    if (pCar_spec->non_driven_wheels_spin_ref_4 >= 0) {
        pCar_spec->non_driven_wheels_spin_ref_4 += gGroove_funk_offset;
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pCar_spec->driven_wheels_circum = (float)((temp_float + temp_float) * REC2_PI);
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pCar_spec->car_model_variable = (pDriver != eDriver_local_human);
    pCar_spec->non_driven_wheels_circum = (float)((temp_float + temp_float) * REC2_PI);
    PossibleService();
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, pOwner, gGroove_funk_offset, &car_crush_buffer);
    PossibleService();
    for (i = 0; i < 6; i++) {
        pCar_spec->wheel_actors[i] = DRActorFindRecurse(pCar_spec->car_master_actor, gWheel_actor_names[i]);
    }
    for (i = 0; i < 6; i++) {
        pCar_spec->pivot_actors[i] = DRActorFindRecurse(pCar_spec->car_master_actor, gPivot_actor_names[i]);
    }
    for (i = 0; i < 6; i++) {
        float fVar3;
        float fVar4;
        float fVar5;
        float mn;
        float my;
        float mz;

        actor = pCar_spec->wheel_actors[i];
        if (actor != NULL && actor->model != NULL) {
            fVar3 = (actor->model->bounds.min.v[0] + actor->model->bounds.max.v[0]) * 0.5f;
            fVar4 = (actor->model->bounds.min.v[1] + actor->model->bounds.max.v[1]) * 0.5f;
            fVar5 = (actor->model->bounds.min.v[2] + actor->model->bounds.max.v[2]) * 0.5f;
            if (fVar3 != 0.0f || fVar4 != 0.0f || fVar5 != 0.0f) {
                actor->t.t.translate.t.v[0] += fVar3;
                actor->t.t.translate.t.v[1] += fVar4;
                actor->t.t.translate.t.v[2] += fVar5;
                mn = -fVar3;
                my = -fVar4;
                mz = -fVar5;
                for (j = 0; j < actor->model->nvertices; j++) {
                    actor->model->vertices[j].p.v[0] += mn;
                    actor->model->vertices[j].p.v[1] += my;
                    actor->model->vertices[j].p.v[2] += mz;
                }
                BrModelUpdate(actor->model, BR_MODU_VERTEX_POSITIONS);
            }
        }
    }
    for (i = 0; i < 6; i++) {
        actor = pCar_spec->pivot_actors[i];
        if (actor != NULL && actor->children != NULL) {
            actor->t.t.translate.t.v[0] += actor->children->t.t.translate.t.v[0];
            actor->t.t.translate.t.v[1] += actor->children->t.t.translate.t.v[1];
            actor->t.t.translate.t.v[2] += actor->children->t.t.translate.t.v[2];
            actor->children->t.t.translate.t.v[0] = 0.f;
            actor->children->t.t.translate.t.v[1] = 0.f;
            actor->children->t.t.translate.t.v[2] = 0.f;
        }
    }
    GetALineAndDontArgue(f, s);
    AddGroovidelics(f, pOwner, pCar_spec->car_master_actor, gGroove_funk_offset, 1);
    if (version == 0) {
        do {
            str = GetALineAndDontArgue(f, mech_s);
            if (str == NULL) {
                PDFatalError("Can't find old end of crush data comment");
            }
        } while (strcmp(mech_s, "START OF MECHANICS STUFF") != 0);
    } else {
        GetALineAndDontArgue(f, mech_s);
    }
    PossibleService();
    for (i = strlen(mech_s) - 1; mech_s[i] == ' '; i--) {
    }
    pCar_spec->maxcurve = 1.0f / GetAScalar(f);
    pCar_spec->initial_brake = GetAScalar(f);
    pCar_spec->brake_increase = GetAScalar(f);
    pCar_spec->max_gear = GetAnInt(f);
    red_gear_speed = GetAScalar(f);
    acceleration_highest_gear = GetAScalar(f);
    pCar_spec->collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    pCar_spec->collision_info->owner = pCar_spec;
    pCar_spec->collision_info->flags_0x238 = 1;
    pCar_spec->collision_info->field_0x1a0 = 0xffff;
    pCar_spec->collision_info->field_0x1a4 = 0;
    GetAString(f, mech_s);
    GetAString(f, mech_s);
    GetAString(f, mech_s);
    pCar_spec->collision_info->actor = pCar_spec->car_master_actor;
    GetAString(f, mech_s);
    GetThreeFloats(f,
        &pCar_spec->collision_info->cmpos.v[0],
        &pCar_spec->collision_info->cmpos.v[1],
        &pCar_spec->collision_info->cmpos.v[2]);
    pCar_spec->collision_info->M = GetAScalar(f);
    GetThreeFloats(f, &ixp, &iyp, &izp);
    pCar_spec->collision_info->I.v[2] = (ixp * ixp + izp * izp) * pCar_spec->collision_info->M * (1.f / 12.f);
    pCar_spec->collision_info->I.v[1] = (iyp * iyp + izp * izp) * pCar_spec->collision_info->M * (1.f / 12.f);
    pCar_spec->collision_info->I.v[0] = (iyp * iyp + ixp * ixp) * pCar_spec->collision_info->M * (1.f / 12.f);
    pCar_spec->downforce_to_weight = GetAScalar(f);
    count_wheels = GetAnInt(f);
    if (count_wheels != 4) {
        FatalError(0);
    }
    for (i = 0; i < count_wheels; i++) {
        GetAnInt(f);
        GetAString(f, mech_s);
        GetAString(f, mech_s);
        GetThreeFloats(f, &wpos.v[0], &wpos.v[1], &wpos.v[2]);
        if (wpos.v[0] == 0.0f && wpos.v[1] == 0.0f && wpos.v[2] == 0.0f) {
            for (j = 0; j < 4; j++) {
                br_actor* parent;

                if (pCar_spec->wheel_actors[j] == NULL) {
                    continue;
                }
                if (DRStricmp(mech_s, pCar_spec->wheel_actors[j]->identifier) != 0) {
                    continue;
                }
                wpos = pCar_spec->wheel_actors[j]->t.t.translate.t;
                parent = pCar_spec->wheel_actors[j]->parent;
                while (parent != NULL && parent != pCar_spec->car_master_actor) {
                    wpos.v[0] += parent->t.t.translate.t.v[0];
                    wpos.v[1] += parent->t.t.translate.t.v[1];
                    wpos.v[2] += parent->t.t.translate.t.v[2];
                    parent = parent->parent;
                }
                break;
            }
        }
        steer_flags = GetAnInt(f);
        GetAnInt(f);
        susp_give = GetAScalar(f);
        damping = GetAScalar(f);
        friction_slipping_reduction = GetAScalar(f);
        GetPairOfFloats(f, &friction_angle_1, &friction_angle_2);
        traction_multiplier = GetAScalar(f);
        rolling_resistance = GetAScalar(f);
        if (steer_flags != 0) {
            pCar_spec->steerable_rolling_resistance = rolling_resistance;
            pCar_spec->steerable_suspension_give = susp_give;
            friction_steer = friction_angle_2;
            wpos_i = 2;
        } else {
            pCar_spec->susp_give = susp_give;
            pCar_spec->rolling_resistance = rolling_resistance;
            pCar_spec->traction_multiplier = traction_multiplier;
            friction_non_steer_1 = friction_angle_1;
            friction_non_steer_2 = friction_angle_2;
            wpos_i = 0;
        }
        if (wpos.v[0] > 0.f) {
            wpos_i += 1;
        }
        pCar_spec->wpos[wpos_i] = wpos;
        pCar_spec->friction_slipping_reduction = friction_slipping_reduction;
        pCar_spec->damping = damping;
    }
    pCar_spec->collision_info->object_friction = 0.4f;
    ReadMechanicsShapes(&pCar_spec->collision_info->shape, f);
    UpdateCollisionObject(pCar_spec->collision_info);
    pCar_spec->maxcurve /= 6.9;
    pCar_spec->ride_height = (pCar_spec->collision_info->bb1.min.v[1] - -0.01) * 6.9;
    pCar_spec->initial_brake *= 12.f * pCar_spec->collision_info->M;
    pCar_spec->brake_increase *= 12.f * pCar_spec->collision_info->M;
    pCar_spec->centre_of_mass_world_scale.v[0] = pCar_spec->collision_info->cmpos.v[0] * WORLD_SCALE;
    pCar_spec->centre_of_mass_world_scale.v[1] = pCar_spec->collision_info->cmpos.v[1] * WORLD_SCALE;
    pCar_spec->centre_of_mass_world_scale.v[2] = pCar_spec->collision_info->cmpos.v[2] * WORLD_SCALE;
    pCar_spec->speed_revs_ratio = red_gear_speed * 4.f / 9.f / pCar_spec->max_gear / 6000.f;
    pCar_spec->force_torque_ratio = pCar_spec->collision_info->M * acceleration_highest_gear * pCar_spec->max_gear;
    pCar_spec->mu.v[0] = tan(friction_non_steer_1 * (157.0 / 9000.0)) / 4.0;
    pCar_spec->mu.v[1] = tan(friction_steer * (157.0 / 9000.0)) / 4.0;
    pCar_spec->mu.v[2] = tan(friction_non_steer_2 * (157.0 / 9000.0)) / 4.0;
    pCar_spec->mu.v[0] *= (float)sqrt((pCar_spec->wpos[2].v[2] - pCar_spec->collision_info->cmpos.v[2]) / (pCar_spec->wpos[2].v[2] - pCar_spec->wpos[0].v[2]) * pCar_spec->collision_info->M * 5.f);
    pCar_spec->mu.v[1] *= (float)sqrt((pCar_spec->wpos[0].v[2] - pCar_spec->collision_info->cmpos.v[2]) / (pCar_spec->wpos[0].v[2] - pCar_spec->wpos[2].v[2]) * pCar_spec->collision_info->M * 5.f);
    pCar_spec->mu.v[2] *= (float)sqrt((pCar_spec->wpos[2].v[2] - pCar_spec->collision_info->cmpos.v[2]) / (pCar_spec->wpos[2].v[2] - pCar_spec->wpos[0].v[2]) * pCar_spec->collision_info->M * 5.f);
    for (i = 0; i < 4; i++) {
        pCar_spec->wpos[i].v[0] *= WORLD_SCALE;
        pCar_spec->wpos[i].v[1] *= WORLD_SCALE;
        pCar_spec->wpos[i].v[2] *= WORLD_SCALE;
        pCar_spec->wpos[i].v[1] = pCar_spec->ride_height;
    }
    GetAnInt(f);
    pCar_spec->collision_info->bb2 = pCar_spec->collision_info->bb1;
    GetAString(f, mech_s);
    PossibleService();
    pCar_spec->count_shrapnel_materials = GetAnInt(f);
    for (i = 0; i < pCar_spec->count_shrapnel_materials; i++) {
        GetALineAndDontArgue(f, gear_s);
        pCar_spec->shrapnel_materials[i] = GetSimpleMaterial(gear_s, (pCar_spec == NULL || pCar_spec->driver < 6) ? 1 : 2);
    }
    count_vertices = 0;
    v11 = pCar_spec->car_actor->model->prepared;
    for (i = 0; i < v11->ngroups; i++) {
        count_vertices += v11->groups[i].nvertices;
    }
    all_fire_zero = 1;
    for (i = 0; i < REC2_ASIZE(pCar_spec->fire_vertex); i++) {
        int vertex_i;

        pCar_spec->fire_vertex[i] = GetAnInt(f);
        if ((int)pCar_spec->fire_vertex[i] >= count_vertices) {
            pCar_spec->fire_vertex[i] = 0;
        }
        if (pCar_spec->fire_vertex[i] != 0) {
            all_fire_zero = 0;
        }
        vertex_i = 0;
        for (j = 0; j < v11->ngroups; j++) {
            for (k = 0; k < v11->groups[j].nvertices; k++) {
                if (v11->groups[j].vertex_user[k] == pCar_spec->fire_vertex[i]) {
                    pCar_spec->fire_vertex[i] = vertex_i;
                    j = v11->ngroups;
                    break;
                }
                vertex_i++;
            }
        }
    }
    if (all_fire_zero) {
        for (i = 0; i < REC2_ASIZE(pCar_spec->fire_vertex); i++) {
            pCar_spec->fire_vertex[i] = IRandomBetween(0, count_vertices - 1);
        }
    }
    while (PFfeof(f) == 0) {
        GetAString(f, mech_s);
        if (DRStricmp(mech_s, "END") == 0) {
            break;
        }
        if (DRStricmp(mech_s, "CAMERA_POSITIONS") == 0) {
            GetAVector3(f, &pCar_spec->camera_bumper_position);
            GetAVector3(f, &pCar_spec->camera_cockpit_position);
        } else if (DRStricmp(mech_s, "CAMERA_TURN_OFF_MATERIALS") == 0) {
            pCar_spec->count_window_materials = GetAnInt(f);
            for (i = 0; i < pCar_spec->count_window_materials; i++) {
                GetAString(f, mech_s);
                pCar_spec->window_materials[i].material = BrMaterialFind(mech_s);
                pCar_spec->window_materials[i].count_maps = GetAnInt(f);
                for (j = 0; j < pCar_spec->window_materials[i].count_maps; j++) {
                    GetAString(f, mech_s);
                    pCar_spec->window_materials[i].maps[j] = BrMapFind(mech_s);
                }
            }
        }
    }
    PFfclose(f);
    PFfclose(g);
    gCurrent_car_spec = NULL;
    pCar_spec->field_0x18c8 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, LinkCarActorsToGroovidelicsCB, NULL);
    CreateDefaultCrushData(pCar_spec->car_model_actor, pCar_spec);
    InitPhysModCrushData(pCar_spec);
    ClosePackFileAndSetTiffLoading(twt);
    car_material = BrMaterialAllocate("PorterIsAWxnkxr");
    if (AddMaterialToStorage(pStorage_space, car_material) == eStorage_not_enough_room) {
        FatalError(kFatalError_InsufficientMaterialSlots);
    }
    BrMaterialAdd(car_material);
    pCar_spec->car_master_actor->material = car_material;
    AutoFogificateMaterials(kRendererShadingType_Specular);
    i = pCar_spec->old_material_count;
    if (i < pCar_spec->new_material_count) {
        do {
            if (pStorage_space->materials_count <= i) {
                gStorage_for_callbacks = prev_storage;
                return;
            }
            car_material = pStorage_space->materials[i];
            if (car_material->identifier != NULL && car_material->identifier[0] != '\0') {
                car_material->identifier[0] = (car_material->flags & BR_MATF_TWO_SIDED) ? 'D' : 'S';
                BrMaterialUpdate(car_material, BR_MATU_ALL);
            }
            i++;
        } while (i < pCar_spec->new_material_count);
    }
    gStorage_for_callbacks = prev_storage;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0048c150
void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gHeadup_image_info); i++) {
        PossibleService();
        if ((gHeadup_image_info[i].avail == eNet_or_otherwise) ||
            ((gHeadup_image_info[i].avail == eNot_net) && (gNet_mode == 0)) ||
            ((gHeadup_image_info[i].avail == eNet_only) && (gNet_mode != 0))) {
            gHeadup_images[i] = LoadPixelmap(gHeadup_image_info[i].name);
            BRPM_convert(gHeadup_images[i], gBack_screen->type);
        } else {
            gHeadup_images[i] = NULL;
        }
    }
}

// OpenRaceFile

// GLOBAL: CARMA2_HW 0x00657530
char* gRaces_file_names[9] = {
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "PEDRACES.TXT",
    "RACES.TXT",
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT"
};

// GLOBAL: CARMA2_HW 0x0068c6f4
int gCurrent_race_file_index;

// GLOBAL: CARMA2_HW 0x007634ec
int gCountRaceGroups;

extern tRace_group_spec* gRace_groups;

// FUNCTION: CARMA2_HW 0x0048c1c0
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {
    FILE* f;
    float low_count;
    float high_count;
    float x_offset;
    float y_offset;
    float x_span;
    float y_span;
    float low_nastiness;
    float high_nastiness;
    tTWTVFS twt;
    int powerup_exclusions[50];
    tPath_name the_path;
    char s[256];
    char* str;
    tRace_group_spec* group;
    float v;
    float r;
    int i;
    int j;
    int count;
    int val;
    int last_race;

    if (!gApplicationDataTwtMounted) {
        twt = OpenPackFileAndSetTiffLoading(gApplication_path);
    }
    gCurrent_race_file_index = pRace_type_index + 1;
    PathCat(the_path, gApplication_path, gRaces_file_names[gCurrent_race_file_index]);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }

    GetAnInt(f);
    gDev_initial_race = 0;
    low_count = GetAScalar(f);
    high_count = GetAScalar(f);
    GetPairOfFloats(f, &x_offset, &y_offset);
    GetPairOfFloats(f, &x_span, &y_span);
    low_nastiness = GetAScalar(f);
    high_nastiness = GetAScalar(f);
    gCountRaceGroups = 0;

    i = 0;
    do {
        GetALineAndDontArgue(f, pRace_list[i].name);
        if (strcmp(pRace_list[i].name, "END") == 0) {
            last_race = 1;
        } else {
            GetAString(f, pRace_list[i].file_name);
            GetAString(f, pRace_list[i].interface_name);
            pRace_list[i].count_opponents = GetAnInt(f);
            pRace_list[i].count_explicit_opponents = GetAnInt(f);
            for (j = 0; j < pRace_list[i].count_explicit_opponents; j++) {
                pRace_list[i].explicit_opponents[j] = GetAnInt(f);
            }
            pRace_list[i].opponent_nastiness_level = GetAScalar(f);
            GetALineAndDontArgue(f, s);
            str = strtok(s, "\t ,/");
            count = 0;
            while (str != NULL) {
                sscanf(str, "%d", &powerup_exclusions[count]);
                if (powerup_exclusions[count] < 0) {
                    break;
                }
                count++;
                str = strtok(NULL, "\t ,/");
            }
            pRace_list[i].count_powerup_exclusions = count;
            if (count != 0) {
                pRace_list[i].powerup_exclusions = BrMemAllocate(count * sizeof(int), kMem_misc);
                memcpy(pRace_list[i].powerup_exclusions, powerup_exclusions, count * sizeof(int));
            }
            pRace_list[i].no_time_awards = GetAnInt(f);
            pRace_list[i].is_boundary = GetAnInt(f);
            if (pRace_list[i].is_boundary) {
                gCountRaceGroups++;
            }
            pRace_list[i].race_type = GetAnInt(f);
            GetThreeInts(f, &pRace_list[i].initial_timer[0], &pRace_list[i].initial_timer[1], &pRace_list[i].initial_timer[2]);
            switch (pRace_list[i].race_type) {
            case kRaceType_Cars:
                pRace_list[i].options.cars.count_opponents = GetAnInt(f);
                for (j = 0; j < pRace_list[i].options.cars.count_opponents; j++) {
                    pRace_list[i].options.cars.opponents[j] = GetAnInt(f);
                }
                break;
            case kRaceType_Peds:
                pRace_list[i].options.peds.count_ped_groups = GetAnInt(f);
                for (j = 0; j < pRace_list[i].options.peds.count_ped_groups; j++) {
                    pRace_list[i].options.peds.ped_groups[j] = GetAnInt(f);
                }
                break;
            case kRaceType_Carma1:
            case kRaceType_Checkpoints:
                pRace_list[i].count_laps = GetAnInt(f);
                break;
            case kRaceType_Smash:
                pRace_list[i].options.smash.var_smash_number = GetAnInt(f);
                pRace_list[i].options.smash.var_smash_target = GetAnInt(f);
                break;
            case kRaceType_SmashNPed:
                pRace_list[i].options.smash_and_peds.var_smash_number = GetAnInt(f);
                pRace_list[i].options.smash_and_peds.var_smash_target = GetAnInt(f);
                pRace_list[i].options.smash_and_peds.ped_group_index = GetAnInt(f);
                break;
            }
            GetThreeInts(f, &pRace_list[i].completion_bonus[0], &pRace_list[i].completion_bonus[1], &pRace_list[i].completion_bonus[2]);
            if (pRace_list[i].race_type == kRaceType_Carma1) {
                GetThreeInts(f, &pRace_list[i].completion_bonus_peds[0], &pRace_list[i].completion_bonus_peds[1], &pRace_list[i].completion_bonus_peds[2]);
                GetThreeInts(f, &pRace_list[i].completion_bonus_opponents[0], &pRace_list[i].completion_bonus_opponents[1], &pRace_list[i].completion_bonus_opponents[2]);
            }
            GetALineAndDontArgue(f, pRace_list[i].description);
            if (DRStricmp(pRace_list[i].description, "none") == 0) {
                pRace_list[i].description[0] = '\0';
            }
            pRace_list[i].expansion = GetAnInt(f) & 1;
            if (pRace_type_index < 0 || pRace_list[i].race_type == kRaceType_Carma1) {
                i++;
            }
            last_race = 0;
        }
    } while (!last_race);

    *pCount = i;
    PFfclose(f);
    if (gRace_groups != NULL) {
        BrMemFree(gRace_groups);
    }
    group = gRace_groups = BrMemAllocate((gCountRaceGroups == 0 ? 1 : gCountRaceGroups) * sizeof(tRace_group_spec), kMem_misc);
    group->count_races = 0;
    group->races = pRace_list;

    for (i = 0; i < *pCount; i++) {
        if (pRace_list[i].opponent_nastiness_level <= 0.0f) {
            pRace_list[i].opponent_nastiness_level = low_nastiness + (high_nastiness - low_nastiness) * ((float)i / (*pCount - 1.0));
        }
        if (pRace_list[i].count_opponents < 0) {
            count = (int)(low_count + (high_count - low_count) * ((float)i / (*pCount - 1.0)));
        } else {
            count = pRace_list[i].count_opponents;
        }
        pRace_list[i].count_opponents = 0;
        if (count <= pRace_list[i].count_explicit_opponents) {
            pRace_list[i].count_explicit_opponents = count;
        } else {
            v = x_offset + (x_span - x_offset) * ((float)i / (*pCount - 1.0));
            r = y_offset + (y_span - y_offset) * ((float)i / (*pCount - 1.0));
            for (j = pRace_list[i].count_explicit_opponents; j < count; j++) {
                val = (int)(v + (r - v) * ((float)(j - pRace_list[i].count_explicit_opponents) / ((float)count - pRace_list[i].count_explicit_opponents - 1.0)));
                if (val < 1) {
                    pRace_list[i].explicit_opponents[j] = -1;
                } else if (val > 5) {
                    pRace_list[i].explicit_opponents[j] = -5;
                } else {
                    pRace_list[i].explicit_opponents[j] = -val;
                }
            }
        }
        pRace_list[i].count_explicit_opponents = count;
        pRace_list[i].group = group;
        if (pRace_list[i].is_boundary) {
            group->mission = &pRace_list[i];
            group++;
            if (group - gRace_groups < gCountRaceGroups) {
                group->count_races = 0;
                group->races = &pRace_list[i + 1];
            }
        } else {
            group->count_races++;
        }
    }
    gCurrent_race_group = gRace_groups;
    if (!gApplicationDataTwtMounted) {
        ClosePackFileAndSetTiffLoading(twt);
    }
}

// FUNCTION: CARMA2_HW 0x0048c860
void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(pRace_info->initial_timer) != 3);

    pRace_info->index = pRace_index;
    pRace_info->race_spec = &gRace_list[pRace_index];
    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
    }
    pRace_info->total_laps = pRace_info->race_spec->count_laps;
    for (i = 0; i < (int)REC2_ASIZE(pRace_info->initial_timer); i++) {
        pRace_info->initial_timer[i] = pRace_info->race_spec->initial_timer[i];
        pRace_info->completion_bonus[i] = pRace_info->race_spec->completion_bonus[i];
        pRace_info->completion_bonus_peds[i] = pRace_info->race_spec->completion_bonus_peds[i];
        pRace_info->completion_bonus_opponents[i] = pRace_info->race_spec->completion_bonus_opponents[i];
    }
}

// FUNCTION: CARMA2_HW 0x0048c930
void C2_HOOK_FASTCALL LoadOpponents(void) {
    FILE* f;
    tPath_name the_path;
    int i;
    int j;
    int k;
    char s[256];
    char* str;
    tText_chunk* the_chunk;

    C2_HOOK_BUG_ON(sizeof(tOpponent) != 0x1c0);
    C2_HOOK_BUG_ON(sizeof(tText_chunk) != 52);
    C2_HOOK_BUG_ON(REC2_ASIZE(gNet_avail_names) != 4);

    PathCat(the_path, gApplication_path, "OPPONENT.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenOpponentsFile);
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &gNumber_of_racers);
    gOpponents = BrMemAllocate(sizeof(tOpponent) * gNumber_of_racers, kMem_oppo_array);

    for (i = 0; i < gNumber_of_racers; i++) {
        PossibleService();
        GetALineAndDontArgue(f, gOpponents[i].name);
        if (strcmp(gOpponents[i].name, "END") == 0) {
            FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
        }
        GetALineAndDontArgue(f, s);
        strcpy(gOpponents[i].abbrev_name, strtok(s, "\t ,/"));
        GetALineAndDontArgue(f, gOpponents[i].car_name);
        /* Strength rating (1-5) */
        gOpponents[i].strength_rating = GetAnInt(f);
        /* Cost to buy it */
        gOpponents[i].price = GetAnInt(f);
        /* Network availability ('eagle', or 'all') */
        gOpponents[i].network_availability = GetALineAndInterpretCommand(f, gNet_avail_names, REC2_ASIZE(gNet_avail_names));

        gOpponents[i].mug_shot_image_data = NULL;
        gOpponents[i].grid_icon_image = NULL;
        gOpponents[i].stolen_car_image_data = NULL;

        /* vehicle filename */
        GetALineAndDontArgue(f, s);
        strcpy(gOpponents[i].car_file_name, strtok(s, "\t ,/"));

        /* vehicle description */
        GetALineAndDontArgue(f, gOpponents[i].line1_topspeed);
        GetALineAndDontArgue(f, gOpponents[i].line2_weight);
        GetALineAndDontArgue(f, gOpponents[i].line3_acceleration);
        GetALineAndDontArgue(f, gOpponents[i].line4_description);

        gOpponents[i].text_chunk_count = 0;
        gOpponents[i].text_chunks = BrMemAllocate(sizeof(tText_chunk) * gOpponents[i].text_chunk_count, kMem_oppo_text_chunk);
        for (j = 0, the_chunk = gOpponents[i].text_chunks; j < gOpponents[i].text_chunk_count; j++, the_chunk++) {
            PossibleService();
            GetPairOfInts(f, &the_chunk->x_coord, &the_chunk->y_coord);
            GetPairOfInts(f, &the_chunk->frame_cue, &the_chunk->frame_end);
            the_chunk->line_count = GetAnInt(f);
            while (the_chunk->line_count > (int)REC2_ASIZE(the_chunk->text)) {
                the_chunk->line_count--;
                GetALineAndDontArgue(f, s);
            }
            for (k = 0; k < the_chunk->line_count; k++) {
                GetALineAndDontArgue(f, s);
                the_chunk->text[k] = BrMemAllocate(strlen(s) + 1, kMem_oppo_text_str);
                strcpy(the_chunk->text[k], s);
            }
        }
        gOpponents[i].dead = 0;
        InitOpponentPsyche(i);
    }
    GetALineAndDontArgue(f, s);
    if (strcmp(s, "END") != 0) {
        FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x0048cd80
void C2_HOOK_FASTCALL AboutToLoadFirstCar(void) {

    InitFunkGrooveFlags();
    gGroove_funk_offset = 0;
}

// FUNCTION: CARMA2_HW 0x0048cda0
// FUNCTION: CARMA2_SW 0x00489fe0
void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info) {
    int i;
    int j;
    tOpp_spec* opponent;
    int sod_counter;
    char buffer[256];

    C2_HOOK_BUG_ON(sizeof(tCar_spec) != 6500);

    sod_counter = 0;
    if (gNet_mode == eNet_mode_none) {
        AboutToLoadFirstCar();
        PrintMemoryDump(0, "JUST BEFORE LOADING YOUR CAR");
        LoadCar(gOpponents[gProgram_state.current_car_index].car_file_name, eDriver_local_human, &gProgram_state.current_car, 0, gProgram_state.player_name, &gOur_car_storage_space);
        SetCarStorageTexturingLevel(&gOur_car_storage_space, GetCarTexturingLevel(), eCTL_full);
        PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING YOUR CAR");
    }

    gGroove_funk_offset = GROOVE_FUNK_MAX_PER_CAR;
    gCount_opponents = 0;
    if (gNet_mode == eNet_mode_none) {
        for (i = 0; i < pRace_info->number_of_racers; i++) {
            opponent = &pRace_info->opponent_list[i];
            PossibleService();
            if (opponent->index >= 0) {
                opponent->car_spec = BrMemAllocate(sizeof(tCar_spec), kMem_oppo_car_spec);
                if (DRStricmp("MAX DAMAGE", gOpponents[opponent->index].name) == 0) {
                    sod_counter++;
                    sprintf(buffer, "POOR SOD %d", sod_counter);
                    LoadCar(gOpponents[opponent->index].car_file_name, eDriver_oppo, opponent->car_spec, opponent->index, buffer, &gTheir_cars_storage_space);
                } else {
                    LoadCar(gOpponents[opponent->index].car_file_name, eDriver_oppo, opponent->car_spec, opponent->index, gOpponents[opponent->index].name, &gTheir_cars_storage_space);
                }
                PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING AN OPPONENT");
            }
            if (gCurrent_race.race_spec->race_type == kRaceType_Cars) {
                if (gCurrent_race.race_spec->options.cars.count_opponents < 0) {
                    gCount_opponents++;
                    opponent->car_spec->is_race_goal = 1;
                } else {
                    for (j = 0; j < gCurrent_race.race_spec->options.cars.count_opponents; j++) {
                        if (opponent->index == gCurrent_race.race_spec->options.cars.opponents[j]) {
                            gCount_opponents++;
                            opponent->car_spec->is_race_goal = 1;
                            break;
                        }
                    }
                }
            }
        }
        SetCarStorageTexturingLevel(&gTheir_cars_storage_space, GetCarTexturingLevel(), eCTL_full);
    }
}

// FUNCTION: CARMA2_HW 0x0048cfc0
void C2_HOOK_FASTCALL LoadMiscStrings(void) {
    int i;
    FILE *f;
    char s[256];
    tPath_name the_path;

    if (gPedTextTxtPath == NULL) {
        SetDefaultTextFileName();
    }
    PathCat(the_path, gApplication_path, gPedTextTxtPath);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    for (i = 0; i < (int)REC2_ASIZE(gMisc_strings); i++) {
        if (PFfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        gMisc_strings[i] = BrMemAllocate(strlen(s) + 1, kMem_misc_string);
        strcpy(gMisc_strings[i], s);
    }
    // Thousands delimiter
    gMisc_strings[294][1] = '\0';
    if (gMisc_strings[294][0] == 'C') {
        gMisc_strings[294][0] = ',';
    } else if (gMisc_strings[294][0] == 'P') {
        gMisc_strings[294][0] = '.';
    } else if (gMisc_strings[294][0] == 'S') {
        gMisc_strings[294][0] = ' ';
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x0048d0e0
void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, track_file_name, 0x1a1e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, race_spec, 0x98);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_list_spec, file_name, 0x60);

    strcpy(gProgram_state.track_file_name, pThe_race->race_spec->file_name);
}

// FUNCTION: CARMA2_HW 0x0048d110
void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions) {

    pOptions->enable_text_messages = GetAnInt(pF);
    pOptions->show_players_on_map = GetAnInt(pF);
    pOptions->show_powerups_on_map = GetAnInt(pF);
    pOptions->powerup_respawn = GetAnInt(pF);
    pOptions->waste_to_transfer = GetAnInt(pF);
    pOptions->open_game = GetAnInt(pF);
    pOptions->grid_start = GetAnInt(pF);
    pOptions->race_sequence_type = GetAnInt(pF);
    pOptions->random_car_choice = GetAnInt(pF);
    pOptions->car_choice = GetAnInt(pF);
    pOptions->starting_credits = GetAnInt(pF);
    pOptions->starting_target = GetAnInt(pF);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0048d7d0
int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex) {

    fprintf(pF, "NETSETTINGS %d\n\n", pIndex);
    fprintf(pF, "%d // Allow the sending of Abuse-o-Matic(tm) text messages\n", gNet_settings[pIndex].enable_text_messages);
    fprintf(pF, "%d // Show cars on map\n", gNet_settings[pIndex].show_players_on_map);
    fprintf(pF, "%d // Show pickups on map\n", gNet_settings[pIndex].show_powerups_on_map);
    fprintf(pF, "%d // Pickup respawn\n", gNet_settings[pIndex].powerup_respawn);
    fprintf(pF, "%d // Waste to transfer\n", gNet_settings[pIndex].waste_to_transfer);
    fprintf(pF, "%d // Open game\n", gNet_settings[pIndex].open_game);
    fprintf(pF, "%d // Grid start\n", gNet_settings[pIndex].grid_start);
    fprintf(pF, "%d // Race order\n", gNet_settings[pIndex].race_sequence_type);
    fprintf(pF, "%d // Auto-goody\n", gNet_settings[pIndex].random_car_choice);
    fprintf(pF, "%d // Car choice mode\n", gNet_settings[pIndex].car_choice);
    fprintf(pF, "%d // Starting credits\n\n", gNet_settings[pIndex].starting_credits);
    fprintf(pF, "%d // Target\n\n", gNet_settings[pIndex].starting_target);
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int C2_HOOK_FASTCALL PDFileUnlock(const char* pThe_path);

#define SAVE_OPTIONS_CHECK(X) if ((X) < 0) { goto fail; }

// FUNCTION: CARMA2_HW 0x0048d190
int C2_HOOK_FASTCALL SaveOptions(void) {
    tPath_name the_path;
    FILE* f;

    PathCat(the_path, gApplication_path, "OPTIONS.TXT");
    PDFileUnlock(the_path);
    f = DRfopen(the_path, "wt");
    if (f == NULL) {
        goto fail;
    }
    SAVE_OPTIONS_CHECK(fprintf(f, "YonFactor %f\n", GetYonFactor()));
    SAVE_OPTIONS_CHECK(fprintf(f, "SkyTextureOn %d\n", GetSkyTextureOn()));
    SAVE_OPTIONS_CHECK(fprintf(f, "CarTexturingLevel %d\n", GetCarTexturingLevel()));
    SAVE_OPTIONS_CHECK(fprintf(f, "RoadTexturingLevel %d\n", GetRoadTexturingLevel()));
    SAVE_OPTIONS_CHECK(fprintf(f, "WallTexturingLevel %d\n", GetWallTexturingLevel()));
    SAVE_OPTIONS_CHECK(fprintf(f, "ShadowLevel %d\n", GetShadowLevel()));
    SAVE_OPTIONS_CHECK(fprintf(f, "DepthCueingOn %d\n", GetDepthCueingOn()));
    SAVE_OPTIONS_CHECK(fprintf(f, "Yon %f\n", GetYon()));
    SAVE_OPTIONS_CHECK(fprintf(f, "CarSimplificationLevel %d\n", GetCarSimplificationLevel()));
    SAVE_OPTIONS_CHECK(fprintf(f, "AccessoryRendering %d\n", GetAccessoryRendering()));
    SAVE_OPTIONS_CHECK(fprintf(f, "SmokeOn %d\n", GetSmokeOn()));
    SAVE_OPTIONS_CHECK(fprintf(f, "SoundDetailLevel %d\n", GetSoundDetailLevel()));
    SAVE_OPTIONS_CHECK(fprintf(f, "ScreenSize %d\n", GetScreenSize()));
    SAVE_OPTIONS_CHECK(fprintf(f, "MapRenderX %f\n", gMap_render_x));
    SAVE_OPTIONS_CHECK(fprintf(f, "MapRenderY %f\n", gMap_render_y));
    SAVE_OPTIONS_CHECK(fprintf(f, "MapRenderWidth %f\n", gMap_render_width));
    SAVE_OPTIONS_CHECK(fprintf(f, "MapRenderHeight %f\n", gMap_render_height));
    SAVE_OPTIONS_CHECK(fprintf(f, "MapMode %d\n", 1));
    SAVE_OPTIONS_CHECK(fprintf(f, "PlayerName 0\n%s\n", gProgram_state.player_name[0] != '\0' ? gProgram_state.player_name : "MAX DAMAGE"));
    SAVE_OPTIONS_CHECK(fprintf(f, "EVolume %d\n", gProgram_state.effects_volume));
    SAVE_OPTIONS_CHECK(fprintf(f, "MVolume %d\n", gProgram_state.music_volume));
    SAVE_OPTIONS_CHECK(fprintf(f, "KeyMapIndex %d\n", gKey_map_index));
    SAVE_OPTIONS_CHECK(fprintf(f, "MapTrans %d\n", gMap_trans));
    SAVE_OPTIONS_CHECK(fprintf(f, "HeadupMapX %d\n", gHeadup_map_x));
    SAVE_OPTIONS_CHECK(fprintf(f, "HeadupMapY %d\n", gHeadup_map_y));
    SAVE_OPTIONS_CHECK(fprintf(f, "HeadupMapW %d\n", gHeadup_map_w));
    SAVE_OPTIONS_CHECK(fprintf(f, "HeadupMapH %d\n", gHeadup_map_h));
    SAVE_OPTIONS_CHECK(fprintf(f, "CameraType %d\n", gCamera_type));
    SAVE_OPTIONS_CHECK(fprintf(f, "ARCameraType %d\n", gAR_camera_type));
    SAVE_OPTIONS_CHECK(fprintf(f, "GoreLevel %d\n", GetHowMuchBloodAndSnotToSmearAbout()));
    SAVE_OPTIONS_CHECK(fprintf(f, "AnimalsOn %d\n", AreWeGoingToOutrageAnimalLoversAroundTheGlobe()));
    SAVE_OPTIONS_CHECK(fprintf(f, "FlameThrowerOn %d\n", IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense()));
    SAVE_OPTIONS_CHECK(fprintf(f, "MinesOn %d\n", IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()));
    SAVE_OPTIONS_CHECK(fprintf(f, "DronesOn %d\n", OtherPerambulatoryInstasificationIsEnablificated()));
    SAVE_OPTIONS_CHECK(fprintf(f, "MiniMapVisible %i\n", gMini_map_visible));
    SAVE_OPTIONS_CHECK(fprintf(f, "SkillLevel %d\n", gProgram_state.skill_level));
    SAVE_OPTIONS_CHECK(fprintf(f, "AmbientSound %i\n", gAmbient_sound));
    SAVE_OPTIONS_CHECK(fprintf(f, "AutoLoad %d\n", gAuto_load));
    SAVE_OPTIONS_CHECK(fprintf(f, "RussellsFannies %d\n", gRussels_fannies));
    SAVE_OPTIONS_CHECK(fprintf(f, "QuickTimeQuality 0\n%s\n", gQuick_time_quality));
    SAVE_OPTIONS_CHECK(fprintf(f, "QuickTimeCompressor 0\n%s\n", gQuick_time_compressor));
    SAVE_OPTIONS_CHECK(fprintf(f, "QuickTimeBanner %d\n%s\n", gQuick_time_banner_number, gQuick_time_banner_texture_name));
    SAVE_OPTIONS_CHECK(fprintf(f, "QuickTimeTempPath 0\n%s\n", gQuick_time_temp_path));
    SAVE_OPTIONS_CHECK(fprintf(f, "QuickTimeMoviePathStub 0\n%s\n", gQuick_time_movie_path_stub));
    SAVE_OPTIONS_CHECK(fprintf(f, "NETGAMETYPE %d\n", gNet_last_game_type));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 0));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 1));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 2));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 3));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 4));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 5));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 6));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 7));
    SAVE_OPTIONS_CHECK(PrintNetOptions(f, 8));
    SAVE_OPTIONS_CHECK(fprintf(f, "HeadupDetailLevel %d\n", gHeadup_detail_level));
    /* the original returns 0 when the close fails; rec2's PFfclose returns void */
    PFfclose(f);
    return 1;
fail:
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048d8f0
int C2_HOOK_FASTCALL RestoreOptions(void) {
    tPath_name the_path;
    FILE* f;
    float arg;
    char line[80];
    char token[80];
    char* s;

    gProgram_state.music_volume = 200;
    gProgram_state.effects_volume = 200;
    gProgram_state.skill_level = 1;
    gMap_render_x = 6.0;
    gMap_render_y = 6.0;
    gMap_render_width = 64.0;
    gMap_render_height = 40.0;
    gMap_trans = 0;
    gHeadup_map_x = 228;
    gHeadup_map_y = 150;
    gHeadup_map_w = 48;
    gHeadup_map_h = 48;
    gHeadup_detail_level = kMax_headup_detail_level;
    gMap_view = 1;
    gMini_map_visible = 1;

    DefaultNetSettings();
    SetQuickTimeDefaults();

    PathCat(the_path, gApplication_path, "OPTIONS.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        return 0;
    }

    while (PFfgets(line, sizeof(line), f)) {
        if (sscanf(line, "%79s%f", token, &arg) == 2) {
            if (strcmp(token, "YonFactor") == 0) {
                SetYonFactor(arg);
            } else if (strcmp(token, "SkyTextureOn") == 0) {
                SetSkyTextureOn((int)arg);
            } else if (strcmp(token, "CarTexturingLevel") == 0) {
                SetCarTexturingLevel((tCar_texturing_level)arg);
            } else if (strcmp(token, "RoadTexturingLevel") == 0) {
                SetRoadTexturingLevel((tRoad_texturing_level)arg);
            } else if (strcmp(token, "WallTexturingLevel") == 0) {
                SetWallTexturingLevel((tWall_texturing_level)arg);
            } else if (strcmp(token, "ShadowLevel") == 0) {
                SetShadowLevel((tShadow_level)arg);
            } else if (strcmp(token, "DepthCueingOn") == 0) {
                SetDepthCueingOn((int)arg);
            } else if (strcmp(token, "Yon") == 0) {
                SetYon(arg);
            } else if (strcmp(token, "CarSimplificationLevel") == 0) {
                SetCarSimplificationLevel((int)arg);
            } else if (strcmp(token, "AccessoryRendering") == 0) {
                SetAccessoryRendering((int)arg);
            } else if (strcmp(token, "SmokeOn") == 0) {
                SetSmokeOn((int)arg);
            } else if (strcmp(token, "SoundDetailLevel") == 0) {
                SetSoundDetailLevel((int)arg);
            } else if (strcmp(token, "ScreenSize") == 0) {
                SetScreenSize((int)arg);
            } else if (strcmp(token, "MapRenderX") == 0) {
                gMap_render_x = arg;
            } else if (strcmp(token, "MapRenderY") == 0) {
                gMap_render_y = arg;
            } else if (strcmp(token, "MapRenderWidth") == 0) {
                gMap_render_width = arg;
            } else if (strcmp(token, "MapRenderHeight") == 0) {
                gMap_render_height = arg;
            } else if (strcmp(token, "MapMode") == 0) {
            } else if (strcmp(token, "MapTrans") == 0) {
                gMap_trans = (int)arg;
            } else if (strcmp(token, "HeadupMapX") == 0) {
                gHeadup_map_x = (int)arg;
            } else if (strcmp(token, "HeadupMapY") == 0) {
                gHeadup_map_y = (int)arg;
            } else if (strcmp(token, "HeadupMapW") == 0) {
                gHeadup_map_w = (int)arg;
            } else if (strcmp(token, "HeadupMapH") == 0) {
                gHeadup_map_h = (int)arg;
            } else if (strcmp(token, "PlayerName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gProgram_state.player_name, s);
            } else if (strcmp(token, "EVolume") == 0) {
                gProgram_state.effects_volume = (int)arg;
            } else if (strcmp(token, "MVolume") == 0) {
                gProgram_state.music_volume = (int)arg;
            } else if (strcmp(token, "KeyMapIndex") == 0) {
                gKey_map_index = (int)arg;
            } else if (strcmp(token, "CameraType") == 0) {
                gAction_replay_camera_mode = gCamera_type = (int)arg;
            } else if (strcmp(token, "ARCameraType") == 0) {
                gAR_camera_type = (int)arg;
            } else if (strcmp(token, "GoreLevel") == 0) {
                SetGoreLevel((int)arg);
            } else if (strcmp(token, "AnimalsOn") == 0) {
                SetAnimalsOn((int)arg);
            } else if (strcmp(token, "FlameThrowerOn") == 0) {
                SetFlameThrowerOn((int)arg);
            } else if (strcmp(token, "MinesOn") == 0) {
                SetExplosivesOn((int)arg);
            } else if (strcmp(token, "DronesOn") == 0) {
                SetTrafficOn((int)arg);
            } else if (strcmp(token, "MiniMapVisible") == 0) {
                gMini_map_visible = (int)arg;
            } else if (strcmp(token, "SkillLevel") == 0) {
                gProgram_state.skill_level = (int)arg;
            } else if (strcmp(token, "AmbientSound") == 0) {
                gAmbient_sound = (int)arg;
            } else if (strcmp(token, "AutoLoad") == 0) {
                gAuto_load = (int)arg;
            } else if (strcmp(token, "RussellsFannies") == 0) {
                gRussels_fannies = (int)arg;
            } else if (strcmp(token, "QuickTimeQuality") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_quality, s);
            } else if (strcmp(token, "QuickTimeCompressor") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_compressor, s);
            } else if (strcmp(token, "QuickTimeBanner") == 0) {
                gQuick_time_banner_number = (int)arg;
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_banner_texture_name, s);
            } else if (strcmp(token, "QuickTimeTempPath") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_temp_path, s);
            } else if (strcmp(token, "QuickTimeMoviePathStub") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_movie_path_stub, s);
            } else if (strcmp(token, "NetName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
            } else if (strcmp(token, "NETGAMETYPE") == 0) {
                gNet_last_game_type = (int)arg;
            } else if (strcmp(token, "NETSETTINGS") == 0) {
                ReadNetworkSettings(f, &gNet_settings[(int)arg]);
            } else if (strcmp(token, "HeadupDetailLevel") == 0) {
                gHeadup_detail_level = (int)arg;
            }
        }
    }
    PFfclose(f);
    gMap_view = 1;
    gMap_render_x = 80.f;
    gMap_render_y = 400.f;
    gMap_render_width = 128.f;
    gMap_render_height = 80.f;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0048e9e0
void C2_HOOK_FASTCALL InitFunkGrooveFlags(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gFunk_groove_flags) != 30);

    // Starting from 1
    for (i = 1; i < REC2_ASIZE(gFunk_groove_flags); i++) {
        gFunk_groove_flags[i] = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0048ea00
void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    gPedTextTxtPath = "TEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea10
void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void) {
    gPedTextTxtPath = "TEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea20
void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void) {
    gPedTextTxtPath = "ZOMTEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea30
void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void) {
    gPedTextTxtPath = "ALTEXT.TXT";
}


// GLOBAL: CARMA2_HW 0x006574d0
const char* gDamage_names[12] = {
    "engine",
    "transmission",
    "driver",
    "steering",
    "lf_brake",
    "rf_brake",
    "lr_brake",
    "rr_brake",
    "lf_wheel",
    "rf_wheel",
    "lr_wheel",
    "rr_wheel",
};

// GLOBAL: CARMA2_HW 0x00657500
const char* gWheel_actor_names[6] = {
    "FLWHEEL.ACT",
    "FRWHEEL.ACT",
    "RLWHEEL.ACT",
    "RRWHEEL.ACT",
    "IRLWHEEL.ACT",
    "IRRWHEEL.ACT",
};

// GLOBAL: CARMA2_HW 0x00657518
const char* gPivot_actor_names[6] = {
    "FLPIVOT.ACT",
    "FRPIVOT.ACT",
    "RLPIVOT.ACT",
    "RRPIVOT.ACT",
    "IRLPIVOT.ACT",
    "IRRPIVOT.ACT",
};

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0048bca0
void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location) {
    int i;
    int j;
    int k;
    int count;
    char s[256];
    char delim[64];
    char* str;
    tDamage_clause* the_clause;

    PossibleService();
    count = GetAnInt(pF);
    pCar_spec->damage_programs[pImpact_location].clause_count = count;
    pCar_spec->damage_programs[pImpact_location].clauses = BrMemAllocate(count * sizeof(tDamage_clause), kMem_damage_clauses);
    strcpy(delim, "\t ,/");
    strcat(delim, "&");
    the_clause = pCar_spec->damage_programs[pImpact_location].clauses;
    for (i = 0; i < count; i++, the_clause++) {
        the_clause->condition_count = 0;
        GetALineAndDontArgue(pF, s);
        str = strtok(s, delim);
        do {
            if (str[0] == 'x') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_x;
            } else if (str[0] == 'y') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_y;
            } else if (str[0] == 'z') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_z;
            } else {
                break;
            }
            if (str[1] == '>') {
                the_clause->conditions[the_clause->condition_count].condition_operator = eCondition_greater_than;
            } else if (str[1] == '<') {
                the_clause->conditions[the_clause->condition_count].condition_operator = eCondition_less_than;
            } else {
                FatalError(kFatalError_ConfusedByFormatOfConditionalDamageInCarFile);
            }
            sscanf(&str[2], "%f", &the_clause->conditions[the_clause->condition_count].comparitor);
            the_clause->condition_count++;
            str = strtok(NULL, delim);
        } while (the_clause->condition_count < 2);

        the_clause->effect_count = GetAnInt(pF);
        for (j = 0; j < the_clause->effect_count; j++) {
            the_clause->effects[j].type = (tDamage_type)-1;
            GetALineAndDontArgue(pF, s);
            str = strtok(s, "\t ,/");
            for (k = 0; k < (int)REC2_ASIZE(gDamage_names); k++) {
                if (strcmp(str, gDamage_names[k]) == 0) {
                    the_clause->effects[j].type = (tDamage_type)k;
                    break;
                }
            }
            if ((int)the_clause->effects[j].type < 0) {
                FatalError(kFatalError_UnknownDamageType_S, str);
            }
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%f", &the_clause->effects[j].weakness_factor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042a280
int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec) {
    int i;
    int j;
    int k;
    FILE* f;
    char s[256];
    int version;
    tCar_crush_spec* car_crush;
    int version_le_100;
    tCar_crush_limit* pLimit;
    int* pCount;
    float f1;
    float f2;
    float f3;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_buffer) != 260);

    *pCar_crush_spec = NULL;
    pCar_crush_buffer->count_entries = 0;
    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return 1;
    }
    /* VERSION %d */
    GetALineAndDontArgue(f, s);
    if (sscanf(s, "VERSION %d", &version) != 1) {
        FatalError(kFatalError_WrongCrushDataFileVersion_SDD, pPath, 0, 4);
    }
    if (version > 100) {
        version -= 100;
        version_le_100 = 0;
    } else {
        version_le_100 = 1;
    }
    if (version != 4) {
        FatalError(kFatalError_WrongCrushDataFileVersion_SDD, pPath, version, 4);
    }

    C2_HOOK_BUG_ON(sizeof(tCar_crush_spec) != 0x57c);
    car_crush = BrMemAllocate(sizeof(tCar_crush_spec), kMem_crush_data);
    *pCar_crush_spec = car_crush;

    /* Sub member: Master-crush data */
    pLimit = &car_crush->limits.limits[0][0];
    pCount = &car_crush->count_limits.counts[0][0];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            /* [0] Number of 'X/Y/Z mins' entries.
             * [1] Number of 'X/Y/Z maxs' entries. */
            *pCount = GetAnInt(f);
            if (*pCount != 0) {
                if (*pCount > 2) {
                    FatalError(kFatalError_TooManyCrushLimits);
                }
                for (k = 0; k < *pCount; k++) {
                    pLimit->values[k] = GetAScalar(f);
                }
            }
            pLimit++;
            pCount++;
        }
    }
    car_crush->field_0x134 = 0;
    car_crush->field_0x144 = 0;
    /* Bendability factor */
    car_crush->bendability_factor = GetAScalar(f);
    /* Bend point Z min */
    car_crush->bend_z_min = GetAScalar(f);
    /* Bend point Z max */
    car_crush->bend_z_max = GetAScalar(f);
    car_crush->field_0x270 = 0;
    car_crush->field_0x2b0 = 0;
    car_crush->field_0x4b8 = 0;
    /* Snappability factor */
    car_crush->snappability_factor = GetAScalar(f);
    /* Y split position */
    car_crush->split_y_pos = GetAScalar(f);
    /* Driver position */
    GetThreeFloats(f, &f1, &f2, &f3);
    car_crush->driver_position.v[0] = f1;
    car_crush->driver_position.v[1] = f2;
    car_crush->driver_position.v[2] = f3;
    car_crush->model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    car_crush->actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrActorAdd(car_crush->actor, car_crush->model_actor);
    car_crush->actor->identifier = BrResStrDup(car_crush->actor, "Sx?!");
    (*pCar_crush_spec)->version_le_100 = version_le_100;
    /* Number of 'Crush data' entries. */
    pCar_crush_buffer->count_entries = (*pCar_crush_spec)->nb_entries = GetAnInt(f);
    if (pCar_crush_buffer->count_entries != 0) {
        if (pCar_crush_buffer->count_entries > kMax_crush_datas) {
            FatalError(kFatalError_TooManyCrushDatas);
        }
        gCrush_data_entry_counter = 1;
        for (i = 0; i < pCar_crush_buffer->count_entries; i++) {
            pCar_crush_buffer->entries[i] = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
            LoadCarCrushDataEntry(f, pCar_crush_buffer->entries[i], pBrender_storage);
        }
    }
    PFfclose(f);
    return 0;
}

// STUB: CARMA2_HW 0x0042aa20
void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0048bfa0
intptr_t C2_HOOK_CDECL LinkCrushModel(br_actor* pActor, void* pData) {
    NOT_IMPLEMENTED();
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0042a550
void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
