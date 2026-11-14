#include "69-sound.h"

#include "00-car.h"
#include "08-loading1.h"
#include "23-piping.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00684550
const char* gPed_sound_path;

// GLOBAL: CARMA2_HW 0x0068b898
int gCD_fully_installed;

// GLOBAL: CARMA2_HW 0x00684558
int gServicing_sound;

// GLOBAL: CARMA2_HW 0x00684560
int gSound_sources_inited;

// GLOBAL: CARMA2_HW 0x00684544
tU32 gLast_sound_service;

// GLOBAL: CARMA2_HW 0x0068454c
int gCDA_started_playing;

// GLOBAL: CARMA2_HW 0x00684548
tU32 gNext_track_finished_check;

// GLOBAL: CARMA2_HW 0x00684564
int gMusic_available;

// GLOBAL: CARMA2_HW 0x00684568
int gINT_00684568;

// GLOBAL: CARMA2_HW 0x00684554
int gINT_00684554;

// GLOBAL: CARMA2_HW 0x00595c44
int gINT_00595c44 = 1;

// GLOBAL: CARMA2_HW 0x00595c20
int gINT_00595c20 = 9998;

// GLOBAL: CARMA2_HW 0x0068455c
undefined4 gUNK_0068455c;

// GLOBAL: CARMA2_HW 0x0079e13c
int gCD_is_disabled;

// GLOBAL: CARMA2_HW 0x00595c78
int gRandom_CDA_tunes[8] = { 9600, 9601, 9602, 9603, 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c98
int gRandom_CDA_tunes_1[4] = { 9600, 9601, 9602, 9603 };

// GLOBAL: CARMA2_HW 0x00595ca8
int gRandom_CDA_tunes_2[4] = { 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c50
int gLast_tune = -1;

// GLOBAL: CARMA2_HW 0x00595c28
const char* gSound_periodicity_choices[3] = {
    "RANDOM",
    "PERIODIC",
    "CONTINUOUS",
};

// GLOBAL: CARMA2_HW 0x00595c48
int gSound_detail_level = 1;

// GLOBAL: CARMA2_HW 0x00684570
tEnvironment_sound_source gEnvironment_sound_sources[5];

// GLOBAL: CARMA2_HW 0x006845fc
tS3_outlet* gEffects_outlet;

// GLOBAL: CARMA2_HW 0x00684600
tS3_outlet* gCar_outlet;

// GLOBAL: CARMA2_HW 0x00684604
tS3_outlet* gEngine_outlet;

// GLOBAL: CARMA2_HW 0x00684608
tS3_outlet* gDriver_outlet;

// GLOBAL: CARMA2_HW 0x0068460c
tS3_outlet* gPedestrians_outlet;

// GLOBAL: CARMA2_HW 0x00684610
tS3_outlet* gMusic_outlet;

// GLOBAL: CARMA2_HW 0x00684614
tS3_outlet* gXXX_outlet;

/* 7 entries: GetIndexFromOutlet scans up to 0x0079e17c (= gEnvironmental_sound_sources_buffer_index); initialized to keep them adjacent */
// GLOBAL: CARMA2_HW 0x0079e160
tS3_outlet* gIndexed_outlets[7] = { 0 };

// GLOBAL: CARMA2_HW 0x0079e17c
int gEnvironmental_sound_sources_buffer_index = 0;

// GLOBAL: CARMA2_HW 0x00595c38
const char* gSound_generator_type_names[3] = {
    "NONCAR",
    "ACTOR",
    "POINT",
};

// GLOBAL: CARMA2_HW 0x00655dd8
int gVirgin_pass = 1;

// GLOBAL: CARMA2_HW 0x00595c4c
int gOld_sound_detail_level = -1;

// GLOBAL: CARMA2_HW 0x0079e120
br_vector3 gCamera_left;

// GLOBAL: CARMA2_HW 0x0079e130
br_vector3 gCamera_position;

// GLOBAL: CARMA2_HW 0x0079e180
br_vector3 gOld_camera_position;

// GLOBAL: CARMA2_HW 0x0079ea60
br_vector3 gCamera_velocity;

// GLOBAL: CARMA2_HW 0x00684540
tU32 gNext_sound_generator_munging;

// GLOBAL: CARMA2_HW 0x0079e18c
int gCount_environmental_sound_sources;

// GLOBAL: CARMA2_HW 0x0079e140
undefined4 gUNK_0079e140;



// FUNCTION: CARMA2_HW 0x00454f40
void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {

    char line1[80];
    char line2[80];
    char line3[80];
    char path_file[80];
    FILE* fp;

    strcpy(path_file, gApplication_path);
    strcat(path_file, (char*)gDir_separator);
    strcat(path_file, "PATHS.TXT");

    if (PDCheckDriveExists(path_file)) {
        fp = PFfopen(path_file, "rt");
        if (fp == NULL) {
            return;
        }
        line1[0] = '\0';
        line2[0] = '\0';
        line3[0] = '\0';
        GetALineWithNoPossibleService(fp, line1);
        GetALineWithNoPossibleService(fp, line2);
        GetALineWithNoPossibleService(fp, line3);
        PFfclose(fp);
        if (strcmp(line3, "Full") != 0) {
            return;
        }
    }
    gCD_fully_installed = 1;
}

// FUNCTION: CARMA2_HW 0x00455080
void C2_HOOK_FASTCALL InitSound(void) {
    tPath_name the_path;
    int engine_channel_count;
    int car_channel_count;
    int ped_channel_count;
    int xxx_channel_count;

    if (gVirgin_pass) {
        PathCat(the_path, gApplication_path, "SOUND");
        PathCat(the_path, the_path, "SOUND.TXT");
        if (gSound_override) {
            gSound_available = gSound_enabled = 0;
        } else {
            gSound_available = gSound_enabled = S3Init(the_path, gAusterity_mode, gPed_sound_path) == 0;
        }
        S3Set3DSoundEnvironment(0.14492753f, -1.0f, -1.0f);
        gVirgin_pass = 0;
        gCD_is_disabled = 0;
        UsePathFileToDetermineIfFullInstallation();
    }
    if (!gSound_available) {
        return;
    }
    if (gSound_detail_level == 0) {
        engine_channel_count = 2;
        ped_channel_count = 3;
        car_channel_count = 2;
        xxx_channel_count = 1;
    } else if (gSound_detail_level == 1) {
        engine_channel_count = 4;
        car_channel_count = 3;
        ped_channel_count = 4;
        xxx_channel_count = 3;
    } else {
        ped_channel_count = 5;
        engine_channel_count = 6;
        car_channel_count = 4;
        xxx_channel_count = 5;
    }
    if (gDriver_outlet == NULL) {
        gIndexed_outlets[0] = gDriver_outlet = S3CreateOutlet(1, 1);
        if (gDriver_outlet == NULL) {
            gSound_available = 0;
            return;
        }
    }
    if (gMusic_outlet == NULL) {
        gIndexed_outlets[2] = gMusic_outlet = S3CreateOutlet(1, 1);
        gMusic_available = gMusic_outlet != NULL;
        DRS3SetOutletVolume(gMusic_outlet, gProgram_state.music_volume);
    }
    if (gSound_detail_level != gOld_sound_detail_level) {
        if (gCar_outlet != NULL) {
            S3ReleaseOutlet(gCar_outlet);
            gCar_outlet = NULL;
        }
        if (gPedestrians_outlet != NULL) {
            S3ReleaseOutlet(gPedestrians_outlet);
            gPedestrians_outlet = NULL;
        }
        if (gEngine_outlet != NULL) {
            S3ReleaseOutlet(gEngine_outlet);
            gEngine_outlet = NULL;
        }
        if (gXXX_outlet != NULL) {
            S3ReleaseOutlet(gXXX_outlet);
            gXXX_outlet = NULL;
        }
        if (gEngine_outlet == NULL) {
            gIndexed_outlets[1] = gEngine_outlet = S3CreateOutlet(engine_channel_count, engine_channel_count);
            if (gEngine_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gEngine_outlet, gProgram_state.effects_volume);
        }
        if (gCar_outlet == NULL) {
            gIndexed_outlets[3] = gCar_outlet = S3CreateOutlet(car_channel_count, car_channel_count);
            if (gCar_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gCar_outlet, gProgram_state.effects_volume);
        }
        if (gPedestrians_outlet == NULL) {
            gIndexed_outlets[4] = gPedestrians_outlet = S3CreateOutlet(ped_channel_count, ped_channel_count);
            if (gPedestrians_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gPedestrians_outlet, gProgram_state.effects_volume);
        }
        if (gXXX_outlet == NULL) {
            gIndexed_outlets[5] = gXXX_outlet = S3CreateOutlet(xxx_channel_count, xxx_channel_count);
            if (gXXX_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gXXX_outlet, gProgram_state.effects_volume);
        }
    }
    if (gEffects_outlet == NULL) {
        gIndexed_outlets[5] = gEffects_outlet = S3CreateOutlet(2, 2);
        if (gEffects_outlet == NULL) {
            gSound_available = 0;
            return;
        }
        DRS3SetOutletVolume(gEffects_outlet, gProgram_state.effects_volume);
    }
    gOld_sound_detail_level = gSound_detail_level;
    SetSoundVolumes(0);
}

// FUNCTION: CARMA2_HW 0x00455690
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound) {

    if (gSound_enabled) {
        if (pSound != eSoundId_pratcam_whirr_noise && !(pSound >= 3000 && pSound <= 3007) && !(pSound >= 5300 && pSound <= 5320)) {
            PipeSingleSound(pOutlet, pSound, 0, 0, -1, NULL);
        }
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004556f0
int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound) {

    if (gSound_enabled) {
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455710
int C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet *pOutlet, tS3_sound_id pSound, int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch) {

    if (gSound_enabled) {
        if (pOutlet != gMusic_outlet
                && pSound != 1000
                && !(pSound >= 3000 && pSound <= 3007)
                && !(pSound >= 5300 && pSound <= 5320)
                && (pLeft_volume != 0 || pRight_volume != 0)) {
            PipeSingleSound(pOutlet, pSound, pLeft_volume, pRight_volume, pLeft_pitch, NULL);
        }
        return S3StartSound2(pOutlet, pSound, pRepeats, pLeft_volume, pRight_volume, pLeft_pitch, pRight_pitch);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004557a0
int C2_HOOK_FASTCALL DRS3ChangeVolume(int pChannel_id, int pVolume) {

    if (gSound_enabled) {
        return S3SetChannelVolume(pChannel_id, pVolume);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455840
int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag) {

    if (gSound_enabled) {
        return S3StopSound(pSound_tag);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455860
// FUNCTION: CARMA2_SW 0x004555f0
tS3_error_codes C2_HOOK_FASTCALL DRS3LoadSound(tS3_sound_id pThe_sound) {

    if (gSound_enabled) {
        return S3LoadSample(pThe_sound);
    } else {
        return eS3_error_none;
    }
}

extern int C2_HOOK_FASTCALL S3ReleaseSound(int pSound_id);

// FUNCTION: CARMA2_HW 0x00455880
int C2_HOOK_FASTCALL DRS3ReleaseSound(int pSound_id) {

    if (gSound_enabled) {
        return S3ReleaseSound(pSound_id);
    } else {
        return 0;
    }
}

void C2_HOOK_FASTCALL DRS3Service(void) {

    if (gSound_enabled) {
        S3Service(gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0, 1);
    }
}

// FUNCTION: CARMA2_HW 0x004558f0
int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag) {

    if (gSound_enabled) {
        return S3SoundStillPlaying(pTag);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455910
void C2_HOOK_FASTCALL DRS3ShutDown(void) {

    if (gSound_enabled) {
        gSound_enabled = 0;
        S3DisableSound();
    }
}

// FUNCTION: CARMA2_HW 0x00455930
int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {

    if (gSound_enabled) {
        return S3SetOutletVolume(pOutlet, pVolume);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00455950
int C2_HOOK_FASTCALL DRS3SetVolume(int pVolume) {

    if (gSound_enabled) {
        return S3SetVolume(pVolume);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00455970
int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet) {

    if (gSound_enabled) {
        return S3StopOutletSound(pOutlet);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004559e0
int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void) {

    if (gSound_enabled) {
        DRS3StopOutletSound(gCar_outlet);
        DRS3StopOutletSound(gEngine_outlet);
        DRS3StopOutletSound(gXXX_outlet);
        DRS3StopOutletSound(gEffects_outlet);
        DRS3StopOutletSound(gPedestrians_outlet);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455a50
void C2_HOOK_FASTCALL ToggleSoundEnable(void) {

    if (gSound_enabled) {
        S3StopAllOutletSounds();
        S3Disable();
        gSound_enabled = 0;
    } else {
        S3Enable();
        gSound_enabled = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00455a80
void C2_HOOK_FASTCALL SoundService(void) {

    if (!gSound_enabled) {
        return;
    }
    if (gServicing_sound) {
        return;
    }
    gServicing_sound = 1;
    gLast_sound_service = PDGetTotalTime();
    if (gCDA_started_playing) {
        tU32 now = PDGetTotalTime();
        if (now > gNext_track_finished_check && !S3IsCDAPlaying()) {
            dr_dprintf("CDINFO: SoundService(): Stopping & starting CD, time %d, gNext_track_finished_check %d",
                now, gNext_track_finished_check);
            StopMusic();
            StartMusic();
        }
    }
    DRS3Service();
    gServicing_sound = 0;
}

// FUNCTION: CARMA2_HW 0x00455bb0
void C2_HOOK_FASTCALL InitSoundSources(void) {
    int toggle = 0;
    int cat;
    int car_count;
    tCar_spec* car;
    tEnvironment_sound_source* env_src;
    int i;

    if (!gSound_available) {
        return;
    }

    if (!gSound_enabled) {
        ToggleSoundEnable();
        toggle = 1;
    }
    BrVector3Copy(&gCamera_position, (br_vector3*)gCamera_to_world.m[3]);
    BrVector3Scale(&gCamera_left, (br_vector3*)gCamera_to_world.m[0], -1.0f);
    S3BindListenerPositionBRender(&gCamera_position);
    S3BindListenerVelocityBRender(&gCamera_velocity);
    S3BindListenerLeftBRender(&gCamera_left);
    if (!gSound_sources_inited) {

        for (cat = eVehicle_rozzer; cat >= eVehicle_self; cat -= 1) {

            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {

                PossibleService();
                if (cat == eVehicle_self) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(cat, i);
                }

                if ((car != NULL && car->driver == eDriver_local_human) || gSound_detail_level >= 3 || cat == eVehicle_rozzer) {

                    car->sound_source = S3CreateSoundSourceBR(&car->pos, &car->vel, gEngine_outlet);
                    if (car->sound_source != NULL) {
                        S3BindAmbientSoundToOutlet(gEngine_outlet, cat == eVehicle_rozzer ? eSoundId_Cop_Siren : car->engine_noises[0], car->sound_source, 250.0f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    }
                }
            }
        }

        for (i = 0; i < (int)REC2_ASIZE(gEnvironment_sound_sources); i++) {

            env_src = &gEnvironment_sound_sources[i];
            env_src->field_0x10 = NULL;
            if (env_src->source == NULL) {
                env_src->source = S3CreateSoundSourceBR(&env_src->position, &gZero_v__car, gXXX_outlet);
                if (env_src->source != NULL) {
                    S3BindAmbientSoundToOutlet(gXXX_outlet, eSoundId_Cop_Siren, env_src->source, 100.f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                }
            }
        }
        gSound_sources_inited = 1;
    }
    if (toggle) {
        ToggleSoundEnable();
    }

    gCount_environmental_sound_sources = 0;
    gEnvironmental_sound_sources_buffer_index = 0;
    gNext_sound_generator_munging = 0;
}

// FUNCTION: CARMA2_HW 0x00455de0
void C2_HOOK_FASTCALL DisposeSoundSources(void) {
    int cat;
    int car_count;
    int i;
    int toggle;
    tCar_spec* the_car;

    if (gSound_available) {
        if (gSound_enabled) {
            toggle = 0;
        } else {
            toggle = 1;
            ToggleSoundEnable();
        }
        if (gSound_sources_inited) {
            DRS3StopOutletSound(gEngine_outlet);
            S3Service(gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0, 0);

            for (cat = eVehicle_rozzer; cat >= eVehicle_self; cat--) {
                if (cat == eVehicle_self) {
                    car_count = 1;
                } else {
                    car_count = GetCarCount(cat);
                }
                for (i = 0; i < car_count; i++) {
                    if (cat == eVehicle_self) {
                        the_car = &gProgram_state.current_car;
                    } else {
                        the_car = GetCarSpec(cat, i);
                    }
                    if ((the_car != NULL && the_car->driver == eDriver_local_human) || gSound_detail_level == 3 || cat == eVehicle_rozzer) {
                        if (the_car->sound_source != NULL) {
                            S3UpdateSoundSource(gEngine_outlet, -1, the_car->sound_source, 0.0f, 0, 0, 0, 0x10000, 0x10000);
                            S3ReleaseSoundSource(the_car->sound_source);
                        }
                        the_car->sound_source = NULL;
                    }
                }
                // FIXME: move outside of car category loop?
                for (i = 0; i < (int)REC2_ASIZE(gEnvironment_sound_sources); i++) {
                    if (gEnvironment_sound_sources[i].source != NULL) {
                        S3BindAmbientSoundToOutlet(gXXX_outlet, -1, gEnvironment_sound_sources[i].source, 100.0f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    }
                }
            }
            gSound_sources_inited = 0;
        }
        if (toggle) {
            ToggleSoundEnable();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00455f80
int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed) {

    if (gSound_enabled) {
        if (pVolume != 0
                && pSound_id != eSoundId_pratcam_whirr_noise
                && !(pSound_id >= 3000 && pSound_id <= 3007)
                && !(pSound_id >= 5300 && pSound_id <= 5320)) {
            PipeSingleSound(pOutlet, pSound_id, pVolume, 0, pPitch, pInitial_position);
                }
        return S3StartSound3D(pOutlet, pSound_id, pInitial_position, pInitial_velocity, pRepeats, pVolume, pPitch, pSpeed);
    } else {
        return 0;
    }
}

extern tS3_sound_tag C2_HOOK_FASTCALL S3StartSoundFromSource3(tS3_sound_id pSound, tS3_sound_source* pSource, tS3_repeats pRepeats, tS3_volume pVolume, tS3_pitch pPitch, tS3_speed pSpeed);

// FUNCTION: CARMA2_HW 0x00456010
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSoundFromSource3(tS3_sound_source* pSource, tS3_sound_id pSound, tS3_repeats pRepeats, tS3_volume pVolume, tS3_pitch pPitch, tS3_speed pSpeed) {
    if (gSound_enabled) {
        return S3StartSoundFromSource3(pSound, pSource, pRepeats, pVolume, pPitch, pSpeed);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00456070
void C2_HOOK_FASTCALL MungeEngineNoise(void) {
    int cat;
    int car_count;
    tCar_spec* the_car;
    float pitch;
    float vol;
    int type_of_engine_noise;
    int i;

    if (!gSound_available || !gProgram_state.racing) {
        return;
    }
    gCamera_left.v[0] = (br_scalar)(gCamera_to_world.m[0][0] * -1.0f);
    gCamera_left.v[1] = (br_scalar)(gCamera_to_world.m[0][1] * -1.0f);
    gCamera_left.v[2] = (br_scalar)(gCamera_to_world.m[0][2] * -1.0f);
    BrVector3Copy(&gCamera_position, (br_vector3*)gCamera_to_world.m[3]);
    if (gFrame_period != 0) {
        gCamera_velocity.v[0] = (br_scalar)((double)(gCamera_position.v[0] - gOld_camera_position.v[0]) / ((double)gFrame_period / 1000.0));
        gCamera_velocity.v[1] = (br_scalar)((double)(gCamera_position.v[1] - gOld_camera_position.v[1]) / ((double)gFrame_period / 1000.0));
        gCamera_velocity.v[2] = (br_scalar)((double)(gCamera_position.v[2] - gOld_camera_position.v[2]) / ((double)gFrame_period / 1000.0));
    } else {
        BrVector3Set(&gCamera_velocity, 0.0f, 0.0f, 0.0f);
    }
    BrVector3Copy(&gOld_camera_position, &gCamera_position);
    if (gAction_replay_mode && fabs(ARGetReplayRate()) <= 1.0) {
        ARGetReplayRate();
    }
    for (cat = eVehicle_rozzer; cat >= 0; cat--) {

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {

            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            if ((the_car != NULL && the_car->driver == eDriver_local_human)
                    || gSound_detail_level >= 3 || cat == eVehicle_rozzer) {

                BrVector3Copy(&the_car->vel, &the_car->collision_info->v);
                if (cat == eVehicle_rozzer) {
                    vol = 255.0f;
                    pitch = (float)BR_FIXED_INT(1);
                } else {
                    if (the_car->collision_info->last_special_volume != NULL) {
                        type_of_engine_noise = the_car->collision_info->last_special_volume->engine_noise_index;
                    } else {
                        type_of_engine_noise = 0;
                    }
                    pitch = (float)(40960.0 + 10.0 * the_car->revs);
                    if (gAction_replay_mode) {
                        pitch = (float)(int)(fabs((double)ARGetReplayRate()) * pitch);
                    }
                    if (type_of_engine_noise == 1) {
                        pitch = (float)(pitch * 0.75);
                    } else if (type_of_engine_noise == 2) {
                        pitch = (float)(pitch * 0.55);
                    }

                    if (pitch < 4096.0f) {
                        pitch = 4096.0f;
                    }
                    if (pitch > 131072.0f) {
                        pitch = 131072.0f;
                    }

                    vol = (float)(96.0 + (double)the_car->revs * 0.0015);
                    if (type_of_engine_noise == 1) {
                        vol = (float)(vol * 5.0);
                    } else if (type_of_engine_noise == 2) {
                        vol = (float)(vol * 2.0);
                    } else {
                        vol *= 2.5;
                    }
                    vol = MIN(vol, 255);
                }
                if (the_car->field_0x4d4 > 1.0) {
                    vol = (float)(vol * (1.0 + (the_car->field_0x4d4 - 1.0f) / 12.0f));
                }
                S3UpdateSoundSource(gEngine_outlet, -1, the_car->sound_source, (float)(gAction_replay_mode ? 300 : 250), 0, 0, (int)vol, (int)pitch, BR_FIXED_INT(1));
            }
        }
    }
    SoundService();
}

// FUNCTION: CARMA2_HW 0x00456530
void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio) {

    if (!gSound_enabled) {
        return;
    }
    if (gEffects_outlet != NULL) {
        DRS3SetOutletVolume(gEffects_outlet, gProgram_state.effects_volume);
    }
    DRS3SetOutletVolume(gCar_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gEngine_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gDriver_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gPedestrians_outlet, gProgram_state.effects_volume);
    if (gINT_00595c44) {
        dr_dprintf("CD: Setting volume to %d", gProgram_state.music_volume);
        if (!gCDA_started_playing && !pCD_audio) {
            dr_dprintf("CD: Hasn't played yet, so not stopping/starting it");
        } else {
            if (gProgram_state.music_volume < 128) {
                StopMusic();
            } else {
                StartMusic();
            }
        }
    } else {
        DRS3SetOutletVolume(gMusic_outlet, gProgram_state.music_volume);
    }
}

// FUNCTION: CARMA2_HW 0x004566a0
tS3_outlet* C2_HOOK_FASTCALL GetOutletFromIndex(int pIndex) {

    return gIndexed_outlets[pIndex];
}

// FUNCTION: CARMA2_HW 0x004566b0
int C2_HOOK_FASTCALL GetIndexFromOutlet(tS3_outlet* pOutlet) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gIndexed_outlets); i++) {
        if (gIndexed_outlets[i] == pOutlet) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004566d0
int C2_HOOK_FASTCALL DRS3StartCDA(int pSound) {

    if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
        gINT_00595c20 = pSound;
    }
    if (!gCD_is_disabled && gMusic_available && !gINT_00684554 && !gINT_00684568 && S3IsCDAEnabled()) {
        dr_dprintf("CDINFO: DRS3StartCDA(): Requested track id %d", pSound);
        gCDA_started_playing = 1;
        S3StopOutletSound(gMusic_outlet);
        if (gSound_enabled) {
            S3Service(gProgram_state.cockpit_on != 0 && gProgram_state.cockpit_image_index >= 0, 0);
            /* Random CDA track */
            if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
                do {
                    switch (pSound) {
                        case 9997:
                            pSound = gRandom_CDA_tunes_2[IRandomBetween(0, REC2_ASIZE(gRandom_CDA_tunes_2) - 1)];
                            break;
                        case 9998:
                            pSound = gRandom_CDA_tunes_1[IRandomBetween(0, REC2_ASIZE(gRandom_CDA_tunes_1) - 1)];
                            break;
                        default:
                            pSound = gRandom_CDA_tunes[IRandomBetween(0, REC2_ASIZE(gRandom_CDA_tunes) - 1)];
                    }
                } while (pSound == gLast_tune);
            }
            gLast_tune = pSound;
            gINT_00684554 = DRS3StartSoundNoPiping(gMusic_outlet, pSound);
            gINT_00684568 = gINT_00684554;
            if (!gINT_00684554) {
                dr_dprintf("CDINFO: DRS3StartCDA(): Chosen actual CD track %d", pSound);
            }
            gUNK_0068455c = 0;
        }
    }
    return gINT_00684568;
}

void C2_HOOK_FASTCALL DRS3StopCDA(void) {

    if (gCD_fully_installed && gMusic_available && gINT_00684568 != 0) {
        S3StopSound(gINT_00684568);
        gINT_00684554 = 0;
        gINT_00684568 = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00456840
void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track) {

    StopMusic();
    if (!gINT_00595c44 || gProgram_state.music_volume >= 128) {
        gNext_track_finished_check = PDGetTotalTime() + 10000;
        dr_dprintf("CDINFO: StartMusicTrack(): New gNext_track_finished_check %d", (int)gNext_track_finished_check);
        gINT_00684568 = DRS3StartCDA(pMusic_track);
    }
}

// FUNCTION: CARMA2_HW 0x004568c0
void C2_HOOK_FASTCALL StartMusic(void) {

    if (!gINT_00595c44 || gProgram_state.music_volume >= 128) {
        if (!S3IsCDAPlaying()) {
            gNext_track_finished_check = PDGetTotalTime() + 10000;
            dr_dprintf("CDINFO: StartMusic(): New gNext_track_finished_check %d", gNext_track_finished_check);
            gINT_00684568 = DRS3StartCDA(gINT_00595c20);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00456910
void C2_HOOK_FASTCALL StopMusic(void) {

    if (gCD_fully_installed) {
        DRS3StopCDA();
    }
}

// FUNCTION: CARMA2_HW 0x00456950
void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {

    gSound_detail_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00456960
void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel) {

    DRS3StopAllOutletSoundsExceptCDA();
    DisposeSoundSources();
    gSound_detail_level = pLevel;
    InitSound();
    InitSoundSources();
}

// FUNCTION: CARMA2_HW 0x004569e0
int C2_HOOK_FASTCALL GetSoundDetailLevel(void) {

    return gSound_detail_level;
}

// FUNCTION: CARMA2_HW 0x004569f0
void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;
    float f1, f2;

    pSpec->periodicity = GetALineAndInterpretCommand(pF, gSound_periodicity_choices, REC2_ASIZE(gSound_periodicity_choices));
    if (pSpec->periodicity != kSoundFxPeriodicity_None) {
        switch (pSpec->periodicity) {
        case kSoundFxPeriodicity_Periodic:
            f1 = GetAScalar(pF);
            pSpec->period = (int)(1000.0f * f1);
            break;
        case kSoundFxPeriodicity_Random:
            GetPairOfFloats(pF, &f1, &f2);
            pSpec->random.min_gap = (int)(1000.0f * f1);
            pSpec->random.max_gap = (int)(1000.0f * f2);
            break;
#ifdef REC2_FIX_BUGS
        default:
            break;
#endif
        }
        pSpec->max_deviation = BR_FIXED_INT(GetAnInt(pF)) / 100;
        pSpec->count_sound_alternatives = GetAnInt(pF);

        C2_HOOK_BUG_ON(REC2_ASIZE(pSpec->sound_alternatives) != 5);
        if (pSpec->count_sound_alternatives > (int)REC2_ASIZE(pSpec->sound_alternatives)) {
            FatalError(kFatalError_TooManyEnvironmentalSoundAlternatives);
        }
        for (i = 0; i < pSpec->count_sound_alternatives; i++) {

            pSpec->sound_alternatives[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
        }
        pSpec->field_0xc = 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// PARTIAL 82.10%: DoEnvSound 0x456af0 - x87 schedule of inlined normalise differs
// FUNCTION: CARMA2_HW 0x00456af0
void C2_HOOK_FASTCALL DoEnvSound(tSpecial_volume* pVolume, br_vector3* pP, tSpecial_volume_soundfx_type pType, tSpecial_volume_soundfx_data* pSound_data, float pSound_dist, br_vector3* pSound_dir) {
    br_vector3 pos;
    br_vector3 dir;
    float neg;

    if (gAmbient_sound && pType != -1) {
        if (pType == 0) {
            if (pSound_dist != 0.0f) {
                BrVector3Normalise(&dir, pSound_dir);
                neg = -pSound_dist;
                pos.v[0] = dir.v[0] * neg + pP->v[0];
                pos.v[1] = dir.v[1] * neg + pP->v[1];
                pos.v[2] = dir.v[2] * neg + pP->v[2];
            } else {
                pos.v[0] = pP->v[0];
                pos.v[1] = pP->v[1];
                pos.v[2] = pP->v[2];
            }
        } else {
            pos.v[0] = SRandomPosNeg(10.f) + pP->v[0];
            pos.v[1] = SRandomPosNeg(10.f) + pP->v[1];
            pos.v[2] = SRandomPosNeg(10.f) + pP->v[2];
        }
        DoAnEnvironmentalSound(pVolume, (tEnvironment_sound_generator_info*)pSound_data, (int)((10.f - pSound_dist) * 255.f * 0.1f), &pos);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// PARTIAL 83.96%: semantics and the pitch/sound register allocation match. Remaining diffs: the periodic-branch block
// layout (then/else bodies swapped), the pSpeed `push -1` is scheduled earlier in the original, and the original keeps
// gXXX_outlet in edi across both play calls (our build reloads it and uses edi for pVolume).
// FUNCTION: CARMA2_HW 0x00456c60
void C2_HOOK_FASTCALL DoAnEnvironmentalSound(void* pSrc_object, tEnvironment_sound_generator_info* pEnv_info, int pVolume, br_vector3* pPos) {
    tSpecial_volume_soundfx_data* soundfx;
    tU32 time;
    tU32 last_time;
    int period;
    int* env_src;
    int found;
    int free_slot;
    int i;
    int pitch;
    int sound;
    int play;

    soundfx = &pEnv_info->soundfx;
    time = GetTotalTime();
    found = soundfx->periodicity;
    play = 0;
    switch (found) {
    case kSoundFxPeriodicity_Random:
        if (time < soundfx->field_0xc) {
            goto done;
        }
        play = 1;
        found = time + IRandomBetween(soundfx->random.min_gap, soundfx->random.max_gap);
        break;
    case kSoundFxPeriodicity_Periodic:
        last_time = soundfx->field_0xc;
        if (time < last_time) {
            goto done;
        }
        play = 1;
        period = soundfx->period;
        if (time - last_time > (tU32)period) {
            soundfx->field_0xc = time + period;
            goto done;
        }
        found = last_time + period;
        break;
    case kSoundFxPeriodicity_Continuous:
        free_slot = -1;
        i = 0;
        env_src = (int*)&gEnvironment_sound_sources[0];
        do {
            if (env_src[4] == 1 && env_src[5] == (int)pSrc_object) {
                break;
            }
            if (env_src[4] == 0) {
                free_slot = i;
            }
            i++;
            env_src += 7;
        } while (i < (int)REC2_ASIZE(gEnvironment_sound_sources));
        if (i == (int)REC2_ASIZE(gEnvironment_sound_sources)) {
            if (free_slot >= 0) {
                env_src = (int*)&gEnvironment_sound_sources[free_slot];
                env_src[4] = 1;
                env_src[5] = (int)pSrc_object;
            } else {
                env_src = NULL;
            }
        }
        if (env_src != NULL) {
            *(br_vector3*)(env_src + 1) = *pPos;
            env_src[6] = 1;
            pitch = IRandomPosNeg(soundfx->max_deviation) + 0x10000;
            S3UpdateSoundSource(gXXX_outlet, (tU16)soundfx->sound_alternatives[IRandomBetween(0, soundfx->count_sound_alternatives - 1)], (tS3_sound_source*)env_src[0], -1.f, -1, -1, pVolume, pitch, -1);
        }
        gUNK_0079e140 = 1;
        goto done;
    default:
        goto done;
    }
    soundfx->field_0xc = found;
done:
    if (play) {
        pitch = IRandomPosNeg(soundfx->max_deviation) + 0x10000;
        sound = (tU16)soundfx->sound_alternatives[IRandomBetween(0, soundfx->count_sound_alternatives - 1)];
        if (gSound_enabled != 0) {
            if (pVolume != 0
                    && sound != eSoundId_pratcam_whirr_noise
                    && !(sound >= 3000 && sound <= 3007)
                    && !(sound >= 5300 && sound <= 5320)) {
                PipeSingleSound(gXXX_outlet, sound, pVolume, 0, pitch, pPos);
            }
            S3StartSound3D(gXXX_outlet, sound, pPos, &gZero_v__car, 1, pVolume, pitch, 0x10000);
        }
    }
}

extern int gFaded_palette;
extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);

// FUNCTION: CARMA2_HW 0x00456e20
void C2_HOOK_FASTCALL MungeEnvironmentalSound(void) {
    int* puVar1;
    int* piVar2;

    if (gAmbient_sound != 0) {
        puVar1 = (int*)&gEnvironment_sound_sources[0].field_0x18;
        do {
            *puVar1 = 0;
            puVar1 = puVar1 + 7;
        } while ((int)puVar1 < (int)&gXXX_outlet);
        if (gFaded_palette == 0) {
            FindSpecialVolume((br_vector3*)&gCamera_to_world.m[3], NULL, 1);
            MungeSoundGenerators();
        }
        piVar2 = (int*)&gEnvironment_sound_sources[0].field_0x10;
        do {
            if (piVar2[2] == 0 && piVar2[0] != 0) {
                S3UpdateSoundSource(gXXX_outlet, -1, (tS3_sound_source*)piVar2[-4], -1.f, -1, -1, 0, -1, -1);
                piVar2[0] = 0;
            }
            piVar2 = piVar2 + 7;
        } while ((int)piVar2 < (int)&gPedestrians_outlet);
    }
}

extern int gAmbient_sound;
extern int gNum_active_non_cars;
extern tNon_car_spec* gActive_non_car_list[99];

static tEnvironment_sound_generator_info sEnv_sound_sources_buffer[2][64];

static void C2_HOOK_FASTCALL ProcessNearbyActors(tTrack_spec* pTrack, br_vector3* pPos, float pMax_dist, int pMatch_type, int pIdentifier_value, int pIdentifier_index, int pMatch_flags, void* pCallback, void* pContext) {
    (void)pTrack; (void)pPos; (void)pMax_dist; (void)pMatch_type; (void)pIdentifier_value;
    (void)pIdentifier_index; (void)pMatch_flags; (void)pCallback; (void)pContext;
}
// FUNCTION: CARMA2_HW 0x00456ea0
void C2_HOOK_FASTCALL MungeSoundGenerators(void) {
    tEnvironment_sound_generator_vector found_sources;
    int i;

    if (gAmbient_sound) {

        if (GetTotalTime() - gNext_sound_generator_munging >= 200) {

            gNext_sound_generator_munging += 200;
            found_sources.sources = sEnv_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index ^ 1];
            found_sources.count = 0;
            ProcessNearbyActors(&gProgram_state.track_spec, (br_vector3*)gCamera_to_world.m[3], 10.f, 1, ')', 0, 0x1003, (void*)FoundSoundSource, &found_sources);
            for (i = 0; i < gProgram_state.count_track_sound_generators; i++) {
                tTrackSoundGenerator* generator = &gProgram_state.track_sound_generators[i];
                if (generator->type == kSoundGeneratorType_point && Vector3DistanceSquared(&generator->point, (br_vector3*)gCamera_to_world.m[3]) < 10.f * 10.f) {
                    tEnvironment_sound_generator_info* src = &found_sources.sources[found_sources.count];
                    src->generator = generator;
                    src->soundfx = generator->fx;
                    src->pos = generator->point;
                    src->actor = NULL;
                    found_sources.count += 1;
                }
            }
            for (i = 0; i < gNum_active_non_cars; i++) {
                tNon_car_spec* c = gActive_non_car_list[i];
                br_actor* a = c->collision_info->actor;
                int src_count = found_sources.count;
                if (a != NULL && a->model != NULL
                        && a->model->identifier != NULL && a->model->identifier[0] == ')'
                        && Vector3DistanceSquared(&a->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]) < 10.f * 10.f) {
                    FoundSoundSource(a, &found_sources);
                    if (src_count < found_sources.count && !Vector3IsZero(&c->collision_info->v)) {
                        found_sources.sources[src_count].soundfx = found_sources.sources[src_count].generator->fx1_noncar;
                    }
                }
            }
            for (i = 0; i < found_sources.count; i++) {
                int j;
                tEnvironment_sound_generator_info* src_now = &found_sources.sources[i];

                for (j = 0; j < gCount_environmental_sound_sources; j++) {
                    tEnvironment_sound_generator_info* src_prev = &sEnv_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index][j];

                    if (src_now->generator == src_prev->generator && src_now->actor == src_prev->actor) {
                        src_now->soundfx.field_0xc = src_prev->soundfx.field_0xc;
                        break;
                    }
                }
            }
            gEnvironmental_sound_sources_buffer_index ^= 1;
            gCount_environmental_sound_sources = found_sources.count;
        }

        for (i = 0; i < gCount_environmental_sound_sources; i++) {
            br_vector3 delta;
            tEnvironment_sound_generator_info* src;
            void* src_obj;

            src = &sEnv_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index][i];
            BrVector3Sub(&delta, &src->pos, (br_vector3*)gCamera_to_world.m[3]);
            src_obj = src->actor;
            if (src_obj == NULL) {
                src_obj = src->generator;
            }
            DoAnEnvironmentalSound(src_obj, src, (int)((10.f - BrVector3LengthSquared(&delta)) * 255.f / 10.f), &src->pos);
        }
    }
}

// TryToSetEnvironmentalSound

// DoEnvSound


// PARTIAL 17.65%: semantics match the disassembly, but MSVC assigns the registers differently (pActor in ebp instead of ebx, src in ebx instead of eax; one extra callee-saved register). The allocation was identical for three different source forms, so it is not driven by the body; in the original TU MungeSoundGenerators (0x00456ea0, ends at 0x00457250) is compiled immediately before this function, while here FoundSoundSource is compiled first.
// FUNCTION: CARMA2_HW 0x00457260
intptr_t C2_HOOK_FASTCALL FoundSoundSource(br_actor* pActor, void* pContext) {
    tEnvironment_sound_generator_vector* found_sources;
    tEnvironment_sound_generator_info* src;
    tTrackSoundGenerator* generator;

    found_sources = pContext;
    if (found_sources->count < 20) {
        src = &found_sources->sources[found_sources->count++];
        src->actor = pActor;
        generator = &gProgram_state.track_sound_generators[(pActor->model->identifier[1] - '0') * 10 + (pActor->model->identifier[2] - '0')];
        src->generator = generator;
        if (generator->type == kSoundGeneratorType_noncar && pActor->identifier[3] == '!') {
            src->soundfx = generator->fx2_noncar;
        } else {
            src->soundfx = generator->fx;
        }
        src->pos = pActor->t.t.translate.t;
    }
    return 0;
}

void C2_HOOK_FASTCALL BuggerModelName(tTrack_spec* pTrack_spec, const char* pName, int pIndex) {
    br_model* model;
    char identifier[4];

    sprintf(identifier, "%c%02d", ')', pIndex);
    model = BrModelFind(pName);
    if (model == NULL || model->identifier == NULL) {
#ifdef REC2_FIX_BUGS
        FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, identifier);
#else
        FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S);
#endif
    } else {
        memcpy(model->identifier, identifier, sizeof(identifier));
    }
}

// PARTIAL (100% effective, not exact): the original looks the model up by the name read from the file (BuggerModelName
// passes pName), which now matches. The only residual is the point case: the original copies x,y,z with the three loads
// first and the three stores after, our BrVector3Set macro interleaves load/store pairs. Every 12-byte aggregate form
// (struct assignment, cast block copy) hoists generator+4 into ebp and re-rolls the whole function's registers.
// FUNCTION: CARMA2_HW 0x004572f0
void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF) {
    float x;
    float y;
    float z;
    char name[32];
    tTrackSoundGenerator* generator;
    int i;

    gProgram_state.count_track_sound_generators = GetAnInt(pF);
    if (gProgram_state.count_track_sound_generators != 0) {

        gProgram_state.track_sound_generators = BrMemAllocate(gProgram_state.count_track_sound_generators * sizeof(tTrackSoundGenerator), kMem_sound_generator);
        for (i = 0, generator = gProgram_state.track_sound_generators; i < gProgram_state.count_track_sound_generators; i++, generator++) {

            generator->type = GetALineAndInterpretCommand(pF, gSound_generator_type_names, REC2_ASIZE(gSound_generator_type_names));
            switch (generator->type) {
            case kSoundGeneratorType_point:
                GetThreeFloats(pF, &x, &y, &z);
                BrVector3Set(&generator->point, x, y, z);
                break;
            case kSoundGeneratorType_actor:
                GetAString(pF, name);
                BuggerModelName(pTrack_spec, name, i);
                break;
            case kSoundGeneratorType_noncar:
                GetAString(pF, name);
                BuggerModelName(pTrack_spec, name, i);
                break;
            }
            ReadSoundSpec(pF, &generator->fx);
            if (generator->type == kSoundGeneratorType_noncar) {
                ReadSoundSpec(pF, &generator->fx1_noncar);
                ReadSoundSpec(pF, &generator->fx2_noncar);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00457450
void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;

    if (pSpec->periodicity == kSoundFxPeriodicity_None) {
        fprintf(pF, "NONE\t\t\t\t\t// sound time type\n");
        return;
    }

    fprintf(pF, "%s\t\t\t\t\t// sound time type\n", gSound_periodicity_choices[pSpec->periodicity]);
    switch (pSpec->periodicity) {
    case kSoundFxPeriodicity_Periodic:
        fprintf(pF,"%.2f\t\t\t\t\t// period\n",
            (double)pSpec->period / 1000.0);
        break;
    case kSoundFxPeriodicity_Random:
        fprintf(pF, "%.2f,%.2f\t\t\t\t\t// min,max gap\n",
           (double)pSpec->random.min_gap / 1000.0,
           (double)pSpec->random.max_gap / 1000.0);
        break;
#ifdef REC2_FIX_BUGS
    default:
        break;
#endif
    }
    fprintf(pF, "%d\t\t\t\t\t// max deviation\n", (int)((100.0 * (double)pSpec->max_deviation) / 65536.0));
    fprintf(pF, "%d\t\t\t\t\t// num sounds\n", pSpec->count_sound_alternatives);
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        fprintf(pF, "%d\t\t\t\t\t// sound ID\n", pSpec->sound_alternatives[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00457570
void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void) {

    gPed_sound_path = NULL;
}

// FUNCTION: CARMA2_HW 0x00457580
void C2_HOOK_FASTCALL ConfigureBloodPedSoundPath(void) {
    gPed_sound_path = NULL;
}

// FUNCTION: CARMA2_HW 0x00457590
void C2_HOOK_FASTCALL ConfigureZombiePedSoundPath(void) {
    gPed_sound_path = "ZOMSOUND";
}

// FUNCTION: CARMA2_HW 0x004575a0
void C2_HOOK_FASTCALL ConfigureAlienPedSoundPath(void) {
    gPed_sound_path = "ALSOUND";
}
// FUNCTION: CARMA2_HW 0x00500060
void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size) {

    // empty
}

/* S3StartSoundFromSource3 is implemented in src/carma2/s3/src/audio.c (0x00566dc1) */
