/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "09-funks.h"

#include "16-graphics1.h"
#include "17-world2.h"
#include "23-piping.h"
#include "36-trig.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "38-flicplay.h"

// GLOBAL: CARMA2_HW 0x0068b840
// GLOBAL: CARMA2_SW 0x00685da0
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x0068b84c
// GLOBAL: CARMA2_SW 0x00685dac
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b850
// GLOBAL: CARMA2_SW 0x00685db0
tGroovidelic_spec* gGroovidelics_array;

// GLOBAL: CARMA2_HW 0x0068b848
// GLOBAL: CARMA2_SW 0x00685da8
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b838
// GLOBAL: CARMA2_SW 0x00685d98
tU32 gPrevious_groove_times[2];

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x0068b844
// GLOBAL: CARMA2_SW 0x00685da4
int gFunkotronics_array_size;

inline int C2_HOOK_FASTCALL CalculateTextureBitsValue(tFunk_texturebits* pTexturebits) {
    int i;
    int value;

    value = 0;
    for (i = 0; i < pTexturebits->count; i++) {
        if (pTexturebits->car->field_0x18cc & (1 << pTexturebits->bits[i])) {
            value += 1 << i;
        }
    }
    return value;
}

inline float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {

    if (pNumber < 0.5) {
        return pNumber * 4.0f - 1.0f;
    } else {
        return 3.0f - pNumber * 4.0f;
    }
}

// FUNCTION: CARMA2_HW 0x00474880
// FUNCTION: CARMA2_SW 0x00473de0
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)REC2_SQR(pYon * 1.02);
}

// AddFunkGrooveBinding

// FUNCTION: CARMA2_HW 0x004748a0
void C2_HOOK_FASTCALL ControlBoundFunkGroove(int pSlot_number, float pValue) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroove_funk_binding, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tGroove_funk_binding, field_0x4, 0x1);
    C2_HOOK_BUG_ON(sizeof(tGroove_funk_binding) != 0x8);
    C2_HOOK_BUG_ON(REC2_ASIZE(gGroove_funk_bindings) != 1440);

    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

// STUB: CARMA2_HW 0x00474950
// STUB: CARMA2_SW 0x00473eb0
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004761e0
// FUNCTION: CARMA2_SW 0x00475740
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;

    (void)pMat;
    the_funk = pData;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);

    if (pActor->model != NULL) {
        for (i = 0, the_face = pActor->model->faces; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

// FUNCTION: CARMA2_HW 0x00476430
// FUNCTION: CARMA2_SW 0x00475990
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    tGroovidelic_spec* the_groove;
    int i;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            PossibleService();
            if (the_groove->owner == pOwner) {
                the_groove->owner = -999;
            }
        }
    }
}

// AddNewGroovidelic

// AddGroovidelics

// FUNCTION: CARMA2_HW 0x00477160
// FUNCTION: CARMA2_SW 0x004766c0
void C2_HOOK_FASTCALL KillGroovadelic(int pOwner) {
    int i;
    tGroovidelic_spec* the_groove;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++) {
            if (the_groove[i].owner == pOwner
                && the_groove[i].path_mode != eMove_controlled
                && the_groove[i].path_mode != eMove_absolute
                && the_groove[i].object_mode != eMove_controlled
                && the_groove[i].object_mode != eMove_absolute) {
                the_groove[i].owner = -999;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004771c0
// FUNCTION: CARMA2_SW 0x00476720
void C2_HOOK_FASTCALL KillFunkotronic(int pOwner) {
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array != NULL) {
        for (i = 0, the_funk = gFunkotronics_array; i < gFunkotronics_array_size; i++) {
            if (the_funk[i].owner == pOwner
                && the_funk[i].matrix_mode != eMove_controlled
                && the_funk[i].matrix_mode != eMove_absolute
                && the_funk[i].lighting_animation_type != eMove_controlled
                && the_funk[i].lighting_animation_type != eMove_absolute
                && ((the_funk[i].texture_animation_data.frames_info.mode != eMove_controlled
                        && the_funk[i].texture_animation_data.frames_info.mode != eMove_texturebits)
                    || the_funk[i].texture_animation_type != eTexture_animation_frames)) {
                the_funk[i].owner = -999;
            }
        }
    }
}

// NormaliseDegreeAngle

#define MOVE_FUNK_PARAMETER(DEST, MODE, PERIOD, TEXTUREBITS, AMPLITUDE, FLASH_VALUE)            \
    do {                                                                                        \
        switch (MODE) {                                                                         \
        case eMove_continuous:                                                                  \
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*SAW(f_the_time, (PERIOD));                                   \
            }                                                                                   \
            break;                                                                              \
        case eMove_controlled:                                                                  \
            DEST = (PERIOD) * (AMPLITUDE);                                                      \
            break;                                                                              \
        case eMove_absolute:                                                                    \
            DEST = (PERIOD);                                                                    \
            break;                                                                              \
        case eMove_linear:                                                                      \
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*MapSawToTriangle(SAW(f_the_time, (PERIOD)));                 \
            }                                                                                   \
            break;                                                                              \
        case eMove_flash:                                                                       \
            if (2 * fmodf(f_the_time, (PERIOD)) > (PERIOD)) {                                   \
                DEST = (FLASH_VALUE);                                                           \
            } else {                                                                            \
                DEST = -(FLASH_VALUE);                                                          \
            }                                                                                   \
            break;                                                                              \
        case eMove_texturebits:                                                                 \
            {                                                                                   \
                int sum_flags = 0;                                                              \
                int ii;                                                                         \
                for (ii = 0; ii < (TEXTUREBITS)->count; ii++) {                                 \
                    if ((TEXTUREBITS)->car->field_0x18cc & (1 << (TEXTUREBITS)->bits[ii])) {    \
                        sum_flags |= 1 << ii;                                                   \
                    }                                                                           \
                }                                                                               \
                DEST = (br_scalar)sum_flags;                                                    \
            }                                                                                   \
            break;                                                                              \
        case eMove_harmonic:                                                                    \
        default:                                                                                \
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*BR_SIN(BrDegreeToAngle(SAW(f_the_time, (PERIOD)) * 360.f));  \
            }                                                                                   \
            break;                                                                              \
        }                                                                                       \
    } while (0)

#define SAW(T, PERIOD) (fmodf((T), (PERIOD)) / (PERIOD))

// FUNCTION: CARMA2_HW 0x00477230
void C2_HOOK_FASTCALL FunkThoseTronics(void) {
    int i;
    int j;
    int iteration_count;
    int finished;
    tFunkotronic_spec* the_funk;
    br_vector3* the_proximity;
    tS32 the_time;
    br_matrix23* mat_matrix;
    br_material* the_material;
    float f_the_time;
    float rot_amount;
    float f_time_diff;
    br_vector2 tmp_v2;

    if (gFunkotronics_array == NULL) {
        return;
    }
    DontLetFlicFuckWithPalettes();
    the_time = GetTotalTime();
    f_the_time = (float)the_time;
    for (i = 0; i < gFunkotronics_array_size; i++) {
        the_funk = &gFunkotronics_array[i];
        if (the_funk->owner == -999) {
            continue;
        }
        if (the_funk->flags != 0) {
            continue;
        }
        j = 0;
        if (the_funk->mode == eFunk_mode_distance && the_funk->proximity_array != NULL) {
            for (j = 0; j < the_funk->proximity_count; j++) {
                int k;
                for (k = 0; k < 3; k++) {
                    the_proximity = &the_funk->proximity_array[j].v[k];
                    if (Vector3DistanceSquared(the_proximity, gOur_pos) <= gSight_distance_squared) {
                        br_vector3 tmp;

                        BrVector3Sub(&tmp, the_proximity, (br_vector3*)gCamera_to_world.m[3]);
                        if (BrVector3Dot(&tmp, &the_funk->proximity_array[j].n) < 0.f && BrVector3Dot(&tmp, (br_vector3*)gCamera_to_world.m[2]) < 0.f) {
                            j = -999;
                            break;
                        }
                    }
                }
                if (j < 0) {
                    break;
                }
            }
            if (j >= 0) {
                continue;
            }
        } else if (the_funk->mode == eFunk_mode_last_lap_only && gLap < gTotal_laps) {
            continue;
        } else if (the_funk->mode == eFunk_mode_all_laps_but_last && gLap >= gTotal_laps) {
            continue;
        }

        the_material = the_funk->material;
        mat_matrix = &the_material->map_transform;
        if (!gAction_replay_mode || !ARReplayIsReallyPaused() || the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
            switch (the_funk->matrix_mod_type) {
            case eMatrix_mod_spin:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= .5f;
                the_material->map_transform.m[2][1] -= .5f;
                if (the_funk->matrix_mod_data.spin_info.period > 0.f) {
                    f_time_diff = 1.f - fmodf(the_funk->matrix_mod_data.spin_info.period, 1.f);
                } else {
                    f_time_diff = fmodf(-the_funk->matrix_mod_data.spin_info.period, 1.f);
                }

                MOVE_FUNK_PARAMETER(rot_amount, the_funk->matrix_mode, f_time_diff, the_funk->matrix_mod_data.texture_info.data, 65536.f, -65536.f);
                DRMatrix23PostRotate(mat_matrix, (br_angle)rot_amount);

                the_material->map_transform.m[2][0] += .5f;
                the_material->map_transform.m[2][1] += .5f;
                break;
            case eMatrix_mod_rock:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= the_funk->matrix_mod_data.rock_info.x_centre;
                the_material->map_transform.m[2][1] -= the_funk->matrix_mod_data.rock_info.y_centre;

                MOVE_FUNK_PARAMETER(rot_amount,
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.rock_info.period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.rock_info.rock_angle,
                    the_funk->matrix_mod_data.rock_info.rock_angle);
                DRMatrix23PostRotate(mat_matrix, BrDegreeToAngle(NormaliseDegreeAngle(rot_amount)));

                the_material->map_transform.m[2][0] += the_funk->matrix_mod_data.rock_info.x_centre;
                the_material->map_transform.m[2][1] += the_funk->matrix_mod_data.rock_info.y_centre;
                break;
            case eMatrix_mod_throb:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= the_funk->matrix_mod_data.throb_info.x_centre;
                the_material->map_transform.m[2][1] -= the_funk->matrix_mod_data.throb_info.y_centre;

                MOVE_FUNK_PARAMETER(tmp_v2.v[1],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.throb_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.throb_info.y_magnitude,
                    the_funk->matrix_mod_data.throb_info.y_magnitude);
                MOVE_FUNK_PARAMETER(tmp_v2.v[0],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.throb_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.throb_info.x_magnitude,
                    the_funk->matrix_mod_data.throb_info.x_magnitude);
                BrMatrix23PostScale(mat_matrix, tmp_v2.v[0] + 1.f, tmp_v2.v[1] + 1.f);

                the_material->map_transform.m[2][0] += the_funk->matrix_mod_data.throb_info.x_centre;
                the_material->map_transform.m[2][1] += the_funk->matrix_mod_data.throb_info.y_centre;
                break;
            case eMatrix_mod_slither:
                MOVE_FUNK_PARAMETER(tmp_v2.v[0],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.slither_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.slither_info.x_magnitude,
                    the_funk->matrix_mod_data.slither_info.x_magnitude);
                the_material->map_transform.m[2][0] = tmp_v2.v[0];
                MOVE_FUNK_PARAMETER(tmp_v2.v[1],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.slither_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.slither_info.y_magnitude,
                    the_funk->matrix_mod_data.slither_info.y_magnitude);
                the_material->map_transform.m[2][1] = tmp_v2.v[1];
                break;
            case eMatrix_mod_roll:
                MOVE_FUNK_PARAMETER(tmp_v2.v[0], the_funk->matrix_mode,
                    the_funk->matrix_mod_data.roll_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    1.f,
                    -1.f);
                the_material->map_transform.m[2][0] = tmp_v2.v[0];
                MOVE_FUNK_PARAMETER(tmp_v2.v[1], the_funk->matrix_mode,
                    the_funk->matrix_mod_data.roll_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    1.f,
                    -1.f);
                the_material->map_transform.m[2][1] = tmp_v2.v[1];
                break;
            case eMatrix_mod_none:
                break;
            }
            if (the_funk->matrix_mod_type != eMatrix_mod_none) {
                BrMaterialUpdate(the_funk->material, BR_MATU_MAP_TRANSFORM);
            }
        }
        if (the_funk->lighting_animation_type != eMove_none) {
            MOVE_FUNK_PARAMETER(the_material->ka, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->ambient_delta, -the_funk->ambient_delta);
            the_material->ka += the_funk->ambient_base;

            MOVE_FUNK_PARAMETER(the_material->kd, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->direct_delta,
                -the_funk->direct_delta);
            the_material->kd += the_funk->direct_base;

            MOVE_FUNK_PARAMETER(the_material->ks, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->specular_delta,
                -the_funk->specular_delta);
            the_material->ks += the_funk->specular_base;
        }
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_none:
            break;
        case eTexture_animation_frames:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused() ||
                the_funk->mode == eFunk_mode_all_laps_but_last || the_funk->mode == 4) {
                br_uint_16 update_flags;
                br_pixelmap* new_colour_map;

                if (the_funk->time_mode == eTime_mode_accurate) {
                    MOVE_FUNK_PARAMETER(rot_amount, the_funk->texture_animation_data.frames_info.mode,
                        the_funk->texture_animation_data.frames_info.controlled.period,
                        the_funk->texture_animation_data.frames_info.texture_info.data,
                        (br_scalar)the_funk->texture_animation_data.frames_info.texture_count,
                        (br_scalar)-the_funk->texture_animation_data.frames_info.texture_count);
                    the_funk->texture_animation_data.frames_info.current_frame = (int) rot_amount;
                } else if (f_the_time - the_funk->last_frame >= the_funk->texture_animation_data.frames_info.controlled.period) {
                    the_funk->last_frame = f_the_time;
                    the_funk->texture_animation_data.frames_info.current_frame += 1;
                    if (the_funk->texture_animation_data.frames_info.current_frame >=
                        the_funk->texture_animation_data.frames_info.texture_count) {
                        the_funk->texture_animation_data.frames_info.current_frame = 0;
                    }
                }
                update_flags = 0;
                new_colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                if (the_material->colour_map != new_colour_map) {
                    the_material->colour_map = new_colour_map;
                    the_material->user = new_colour_map;
                    update_flags |= BR_MATU_COLOURMAP;
                }
                the_material->colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                if (the_material->colour_map != new_colour_map) {
                    BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                }
                if (the_funk->texture_animation_data.frames_info.has_matrix) {
                    br_matrix23 *frame_mat = &the_funk->texture_animation_data.frames_info.mat[the_funk->texture_animation_data.frames_info.current_frame];
                    if (the_material->map_transform.m[0][0] != frame_mat->m[0][0]
                        || the_material->map_transform.m[1][1] != frame_mat->m[1][1]
                        || the_material->map_transform.m[2][0] != frame_mat->m[2][0]
                        || the_material->map_transform.m[2][1] != frame_mat->m[2][1]) {

                        BrMatrix23Copy(&the_material->map_transform, frame_mat);
                        update_flags |= BR_MATU_MAP_TRANSFORM;
                    }
                }
                if (update_flags != 0) {
                    BrMaterialUpdate(the_material, update_flags);
                }
            }
            break;
        case eTexture_animation_flic:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                tS32 i_last_frame = (tS32)the_funk->last_frame;
                tS32 i_time_diff;
                if (the_time < i_last_frame) {
                    i_time_diff = 2 * i_last_frame - the_time;
                } else {
                    i_time_diff = the_time;
                }
                if (the_funk->time_mode == eTime_mode_accurate) {
                    if (the_funk->last_frame) {
                        iteration_count = (i_time_diff - i_last_frame) /
                                          the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period;
                    } else {
                        iteration_count = 1;
                    }
                } else {
                    if ((br_scalar)(i_time_diff - the_funk->last_frame) >= the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period) {
                        iteration_count = 1;
                    } else {
                        iteration_count = 0;
                    }
                }
                for (j = 0; j < iteration_count; j++) {
                    finished = PlayNextFlicFrame(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                    BrMapUpdate(the_funk->material->colour_map, BR_MAPU_ALL);
                    BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                    if (finished) {
                        EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                        StartFlic(
                                the_funk->texture_animation_data.flic_info.flic_descriptor.file_name,
                                -1,
                                &the_funk->texture_animation_data.flic_info.flic_descriptor,
                                the_funk->texture_animation_data.flic_info.flic_data_length,
                                (tS8 *) the_funk->texture_animation_data.flic_info.flic_data,
                                the_material->colour_map, 0, 0, 0);
                    }
                    the_funk->last_frame = f_the_time;
                }
            }
            break;
        case eTexture_animation_camera:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                br_vector3 camera_look;
                br_actor *camera_actor;
                if (the_funk->texture_animation_data.camera_info.count == 1) {
                    camera_actor = the_funk->texture_animation_data.camera_info.actors[0];
#ifdef REC2_FIX_BUGS
                    BrVector3Sub(&camera_look, &gProgram_state.current_car.pos,
                                 &camera_actor->t.t.translate.t);
#endif
                } else {
                    float closest_distance = (float) 0xffffffffu;
                    camera_actor = NULL;
                    for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                        br_vector3 delta;
                        br_actor *act = the_funk->texture_animation_data.camera_info.actors[i];
                        float dist;
                        BrVector3Sub(&delta, &gProgram_state.current_car.pos, &act->t.t.translate.t);
                        dist = BrVector3LengthSquared(&delta);
                        if (dist < closest_distance) {
                            closest_distance = dist;
                            BrVector3Copy(&camera_look, &delta);
                            camera_actor = act;
                        }
                    }
                }
                if (camera_actor != NULL) {
                    if (the_funk->texture_animation_data.camera_info.mode == 1) {
                        camera_actor->t.type = BR_TRANSFORM_LOOK_UP;
                        BrVector3Copy(&camera_actor->t.t.look_up.look, &camera_look);
                        BrVector3Set(&camera_actor->t.t.look_up.up, 0.f, 1.f, 0.f);
                    }
                    AddExtraRender(camera_actor, the_material);
                }
            }
            break;
        case eTexture_animation_mirror:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                    BrMatrix34Mul(&the_funk->texture_animation_data.mirror_info.actor->t.t.mat,
                                  &gCamera_to_world,
                                  &the_funk->texture_animation_data.mirror_info.mat);
                    AddExtraRender(the_funk->texture_animation_data.mirror_info.actor, the_material);
                }
            }
            break;
        }
    }
    LetFlicFuckWithPalettes();
}

#define SAW(T, PERIOD) fmod((T), PERIOD) / (PERIOD)

#define GROOVE_PATH_SIN_COS(TRIG, TIME, PERIOD, AMPLITUDE) \
    ((PERIOD) != 0.0f \
            ? (br_scalar)(TRIG(BrAngleToRadian(BrDegreeToAngle( \
                              SAW(TIME, PERIOD) * 360.0f))) \
                  * (AMPLITUDE)) \
            : 0.0f)

#define GROOVE_PATH_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f \
                      ? SAW(TIME, PERIOD) * (AMPLITUDE) \
                      : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? PERIOD * AMPLITUDE \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f \
                                                    ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) \
                                                        * (AMPLITUDE) \
                                                    : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) \
                                                              ? -(AMPLITUDE) \
                                                              : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : GROOVE_PATH_SIN_COS(sinf, TIME, PERIOD, AMPLITUDE)))))))

// FUNCTION: CARMA2_HW 0x00478e30
// FUNCTION: CARMA2_SW 0x00478390
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;

    switch (pGroove->path_type) {
    case eGroove_path_straight:
        if (pGroove->path_data.straight_info.x_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[0]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.x_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][0] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[1]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.y_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][1] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[2]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.z_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][2] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            }
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    case eGroove_path_circular:
        pGroove->actor->t.t.translate.t = pGroove->path_data.circular_info.centre;
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    default:
        break;
    }
}

#define GROOVE_ANGLE(MODE, TIME, PERIOD, TEXTUREBITS) \
    BrDegreeToAngle(((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? SAW(TIME, PERIOD) * 360.0 : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * 360.0 \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) * 360.0 : 0.0) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -360.0 : 360.0) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? sin(BrAngleToRadian(BrDegreeToAngle(SAW(TIME, PERIOD) * 360.0))) * 360.0 : 0.0))))))))

#define GROOVE_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? SAW(TIME, PERIOD) * (AMPLITUDE) : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * (AMPLITUDE) \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) * (AMPLITUDE) : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -(AMPLITUDE) : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? BR_SIN(BrDegreeToAngle(SAW(TIME, PERIOD) * 360.0)) * (AMPLITUDE) : 0.0f)))))))

// FUNCTION: CARMA2_HW 0x00479890
// FUNCTION: CARMA2_SW 0x00478df0
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        }
        break;
    case eGroove_object_rock:
        rock_it = 1;
        pos = (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
            pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle));

        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pGroove->object_data.rock_info.current_angle > pos) {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_less_than) {
            if (pGroove->object_resumption_value > pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
            if (pGroove->object_resumption_value < pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        }
        if (rock_it) {
            pGroove->object_data.rock_info.current_angle = pos;
        }

        switch (pGroove->object_data.rock_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        }
        break;
    case eGroove_object_throb:
        BrMatrix34PostScale(pMat,
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude)
                + 1.0));
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        x_size = bounds->max.v[0] - bounds->min.v[0];
        y_size = bounds->max.v[1] - bounds->min.v[1];
        z_size = bounds->max.v[2] - bounds->min.v[2];
        if (pGroove->object_data.shear_info.x_magnitude == 0.0f) {
            BrMatrix34PostShearX(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else if (pGroove->object_data.shear_info.y_magnitude == 0.0f) {
            BrMatrix34PostShearY(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else {
            BrMatrix34PostShearZ(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size));
        }
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00478c80
// FUNCTION: CARMA2_SW 0x004781e0
void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it) {
    br_actor* the_actor;
    br_vector3 actor_pos;
    br_matrix34* the_mat;
    tInterrupt_status old_path_interrupt;
    tInterrupt_status old_object_interrupt;

    old_path_interrupt = pGroove->path_interrupt_status;
    old_object_interrupt = pGroove->object_interrupt_status;
    the_actor = pGroove->actor;
    pGroove->done_this_frame = 1;
    CalcActorGlobalPos(&actor_pos, the_actor);
    if (PointOutOfSight(&actor_pos, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->path_mode == eMove_controlled
            || pGroove->path_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if ((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
        && (!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->object_mode == eMove_controlled
            || pGroove->object_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x2)) {
        the_mat->m[0][0] = 1.0f;
        the_mat->m[0][1] = 0.0f;
        the_mat->m[0][2] = 0.0f;
        the_mat->m[1][0] = 0.0f;
        the_mat->m[1][1] = 1.0f;
        the_mat->m[1][2] = 0.0f;
        the_mat->m[2][0] = 0.0f;
        the_mat->m[2][1] = 0.0f;
        the_mat->m[2][2] = 1.0f;
        the_mat->m[3][0] = -pGroove->object_centre.v[0];
        the_mat->m[3][1] = -pGroove->object_centre.v[1];
        the_mat->m[3][2] = -pGroove->object_centre.v[2];
        ObjectGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
        the_mat->m[3][0] = pGroove->object_position.v[0] + the_mat->m[3][0] + pGroove->object_centre.v[0];
        the_mat->m[3][1] = pGroove->object_position.v[1] + the_mat->m[3][1] + pGroove->object_centre.v[1];
        the_mat->m[3][2] = pGroove->object_position.v[2] + the_mat->m[3][2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (old_path_interrupt != pGroove->path_interrupt_status || old_object_interrupt != pGroove->object_interrupt_status) {
        PipeSingleGrooveStop(pGroove - gGroovidelics_array, the_mat,
            pGroove->path_interrupt_status, pGroove->object_interrupt_status,
            pGroove->path_resumption_value, pGroove->object_resumption_value);
    }
}

// FUNCTION: CARMA2_HW 0x00478c00
// FUNCTION: CARMA2_SW 0x00478160
void C2_HOOK_FASTCALL GrooveThoseDelics(void) {
    tGroovidelic_spec* the_groove;
    float f_the_time;
    int i;

    if (gGroovidelics_array != NULL) {
        f_the_time = (float)GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        gPrevious_groove_times[0] = (tU32)f_the_time;
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, (tU32)f_the_time, 0);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b0e0
// FUNCTION: CARMA2_SW 0x0047a640
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            if (the_groove->path_interrupt_status == eInterrupt_none
                && the_groove->object_interrupt_status == eInterrupt_none) {
                GrooveThisDelic(the_groove, gPrevious_groove_times[1], 1);
            }
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b130
// FUNCTION: CARMA2_SW 0x0047a690
void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags |= pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b160
// FUNCTION: CARMA2_SW 0x0047a6c0
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags &= ~pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b1a0
// FUNCTION: CARMA2_SW 0x0047a700
void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value) {
    tGroovidelic_spec* the_groove;

    the_groove = &gGroovidelics_array[pIndex];
    the_groove->path_interrupt_status = pPath_interrupt_status;
    the_groove->object_interrupt_status = pObject_interrupt_status;
    the_groove->path_resumption_value = pPath_resumption_value;
    the_groove->object_resumption_value = pObject_resumption_value;
    BrMatrix34Copy(&the_groove->actor->t.t.mat, pMatrix);
}

// FUNCTION: CARMA2_HW 0x0047b1e0
// FUNCTION: CARMA2_SW 0x0047a740
void C2_HOOK_FASTCALL ResetGrooveFlags(void) {
    int i;
    tGroovidelic_spec* the_groove;

    i = 0;
    if (gGroovidelics_array_size > 0) {
        the_groove = gGroovidelics_array;
        do {
            the_groove->done_this_frame = 0;
            i++;
            the_groove++;
        } while (i < gGroovidelics_array_size);
    }
}

// FUNCTION: CARMA2_HW 0x0047b210
// FUNCTION: CARMA2_SW 0x0047a770
intptr_t C2_HOOK_CDECL DelGrooveRef(br_actor* pActor, void* pArg) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (pActor == the_groove->actor) {
            the_groove->owner = -999;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0047b250
// FUNCTION: CARMA2_SW 0x0047a7b0
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x1;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
// FUNCTION: CARMA2_SW 0x0047a7e0
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x1;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b2b0
// FUNCTION: CARMA2_SW 0x0047a810
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x2;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
// FUNCTION: CARMA2_SW 0x0047a840
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x2;
}

// FUNCTION: CARMA2_HW 0x0047b310
// FUNCTION: CARMA2_SW 0x0047a870
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        EnableFunkotronic(pIndex);
    } else {
        DisableFunkotronic(pIndex);
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
// FUNCTION: CARMA2_SW 0x0047a8c0
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    tGroovidelic_spec* groove;
    int count;
    int i;

    groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, groove++) {
        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}
