#include "09-funks.h"

#include "23-piping.h"
#include "16-graphics1.h"
#include "36-trig.h"
#include "17-world2.h"
#include "38-flicplay.h"
#include "globvars.h"
#include "41-utility.h"
#include "52-errors.h"
#include "rec2_macros.h"

extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);

static float MapSawToTriangle(float pNumber);
static br_scalar NormaliseDegreeAngle(br_scalar pAngle);

// GLOBAL: CARMA2_HW 0x0068b840
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x0068b84c
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b850
tGroovidelic_spec* gGroovidelics_array;

// GLOBAL: CARMA2_HW 0x0068b848
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b838
tU32 gPrevious_groove_times[2];

// GLOBAL: CARMA2_HW 0x00688b28
float gFunk_scalar;

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x0068b830
tFunk_temp_buffer* gFunk_temp_vertices;

// GLOBAL: CARMA2_HW 0x0068b844
// GLOBAL: CARMA2_SW 0x00685da4
int gFunkotronics_array_size;

// GLOBAL: CARMA2_HW 0x00762100
float gYon_squared = 0.f;

// GLOBAL: CARMA2_HW 0x0074b584
tCar_spec* gCurrent_car_spec;

// GLOBAL: CARMA2_HW 0x00655ba0
const char* gFunk_nature_names[4] = {
    "constant",
    "distance",
    "lastlap",
    "otherlaps",
};

// GLOBAL: CARMA2_HW 0x00655b50
const char* gFunk_type_names[5] = {
    "spin",
    "rock",
    "throb",
    "slither",
    "roll",
};

// GLOBAL: CARMA2_HW 0x00655b80
const char* gFunk_move_names[7] = {
    "linear",
    "harmonic",
    "flash",
    "controlled",
    "absolute",
    "continuous",
    "texturebits",
};

// GLOBAL: CARMA2_HW 0x00655b68
const char* gFunk_anim_names[4] = {
    "frames",
    "flic",
    "camera",
    "mirror",
};

// GLOBAL: CARMA2_HW 0x00655b78
const char* gTime_mode_names[2] = {
    "approximate",
    "accurate",
};

// GLOBAL: CARMA2_HW 0x00655bf0
const char* gCamera_animation_names[2] = {
    "static",
    "tracking",
};

// CalculateTextureBitsValue

// MapSawToTriangle

// FUNCTION: CARMA2_HW 0x00474880
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

// FUNCTION: CARMA2_HW 0x004748e0
float C2_HOOK_FAKE_THISCALL ControlBoundFunkGroovePlus(int pSlot_number, undefined4 pArg2, float pValue) {

    if (pSlot_number < 0) {
        return 0.f;
    }
    if (pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = fmodf(*gGroove_funk_bindings[pSlot_number].v + pValue, 1.f);
        return *gGroove_funk_bindings[pSlot_number].v;
    }
    return *gGroove_funk_bindings[pSlot_number].v;
}
// ShiftBoundGrooveFunks

// AddNewFunkotronic

// based on ApexWorm/rec2 4a25846ee
// FUNCTION: CARMA2_HW 0x00474950
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    tFunkotronic_spec* the_funk;
    int i;

    if (gFunkotronics_array != NULL && gFunkotronics_array_size > 0) {
        the_funk = gFunkotronics_array;
        for (i = 0; i < gFunkotronics_array_size; i++, the_funk++) {
            PossibleService();
            if (the_funk->owner == pOwner) {
                the_funk->owner = -999;
                if (the_funk->proximity_array != NULL) {
                    BrMemFree(the_funk->proximity_array);
                }
                if (the_funk->texture_animation_type == eTexture_animation_frames
                    && the_funk->texture_animation_data.frames_info.mode == eMove_texturebits) {
                    BrMemFree(the_funk->texture_animation_data.frames_info.texture_info.data);
                }
                if (the_funk->texture_animation_type == eTexture_animation_flic) {
                    BrMemFree(the_funk->texture_animation_data.flic_info.flic_data);
                    EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                    BrMemFree(the_funk->material->colour_map->pixels);
                    the_funk->material->colour_map->pixels = NULL;
                    BrPixelmapFree(the_funk->material->colour_map);
                    the_funk->material->colour_map = NULL;
                } else if (the_funk->texture_animation_type == eTexture_animation_camera) {
                    /* Camera disposal shares this counter with the outer traversal. */
                    for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                        BrActorRemove(the_funk->texture_animation_data.camera_info.actors[i]);
                        BrActorFree(the_funk->texture_animation_data.camera_info.actors[i]);
                    }
                } else if (the_funk->texture_animation_type == eTexture_animation_mirror) {
                    if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                        BrActorRemove(the_funk->texture_animation_data.mirror_info.actor);
                        BrActorFree(the_funk->texture_animation_data.mirror_info.actor);
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004761e0
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

// FUNCTION: CARMA2_HW 0x00476430
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

// FUNCTION: CARMA2_HW 0x00477160
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

#define SAW(T, PERIOD) (fmodf((T), (PERIOD)) / (PERIOD))

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

// (reference)
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

// PathGrooveBastard

// ObjectGrooveBastard

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern int gAction_replay_mode;
extern br_matrix34 gCamera_to_world;
extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, br_scalar pMax_distance);
extern void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it);
extern void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it);
extern void C2_HOOK_FASTCALL LollipopizeActor(br_actor* pSubject_actor, br_matrix34* ref_to_world, tLollipop_mode pWhich_axis);

// based on ApexWorm/rec2 8d0df7422
// FUNCTION: CARMA2_HW 0x00478c80
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
        || pGroove->path_mode == eMove_absolute) && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if (((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
            && (!gAction_replay_mode
                    || !ARReplayIsReallyPaused()
                    || pGroove->object_mode == eMove_controlled
                    || pGroove->object_mode == eMove_absolute)) && !(pGroove->block_flags & 0x2)) {
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
        PipeSingleGrooveStop(
            pGroove - gGroovidelics_array,
            the_mat,
            pGroove->path_interrupt_status,
            pGroove->object_interrupt_status,
            pGroove->path_resumption_value,
            pGroove->object_resumption_value);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00478c00
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
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x1;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x1;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b2b0
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x2;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x2;
}

// FUNCTION: CARMA2_HW 0x0047b310
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        EnableFunkotronic(pIndex);
    } else {
        DisableFunkotronic(pIndex);
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
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

/* ; called through DRActorEnumRecurseWithMat */
// FUNCTION: CARMA2_HW 0x00476230
br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;
    int j;

    (void)pMat;
    the_funk = pData;

    if (pActor->model != NULL) {
        for (i = 0; i < pActor->model->nfaces; i++) {
            the_face = &pActor->model->faces[i];
            if (the_face->material == the_funk->material) {
                for (j = 0; j < 3; j++) {
                    BrVector3Copy(&the_funk->proximity_array[the_funk->proximity_count].v[j], &pActor->model->vertices[the_face->vertices[j]].p);
                    gFunk_temp_vertices[the_funk->proximity_count].vertices[j] = &pActor->model->vertices[the_face->vertices[j]];
                }
                BrVector3Normalise(&the_funk->proximity_array[the_funk->proximity_count].n, &the_face->n);
                the_funk->proximity_array[the_funk->proximity_count].d = -the_face->d;
                the_funk->proximity_count += 1;
            }
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// (reference)
// FUNCTION: CARMA2_HW 0x00478e30
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;
    tU32 f_the_time = pTime;


    switch (pGroove->path_type) {
    case eGroove_path_circular:
        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.circular_info.centre);
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = BR_COS(BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0)) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = BR_SIN(BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0)) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0] + pos;
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = BR_SIN(BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0)) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = BR_COS(BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0)) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1] + pos;
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = BR_COS(BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0)) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = BR_SIN(BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0)) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2] + pos;
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    case eGroove_path_straight:
        if (pGroove->path_data.straight_info.x_delta != 0.0f) {
            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.x_delta, -pGroove->path_data.straight_info.x_delta);
            pos += pGroove->path_data.straight_info.centre.v[0];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                pMat->m[3][0] = pos;
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][0] = pos;
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                        pMat->m[3][0] = pos;
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][0] = pos;
                }
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.y_delta, -pGroove->path_data.straight_info.y_delta);
            pos += pGroove->path_data.straight_info.centre.v[1];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                pMat->m[3][1] = pos;
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][1] = pos;
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                        pMat->m[3][1] = pos;
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][1] = pos;
                }
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.z_delta, -pGroove->path_data.straight_info.z_delta);
            pos += pGroove->path_data.straight_info.centre.v[2];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                pMat->m[3][2] = pos;
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][2] = pos;
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                        pMat->m[3][2] = pos;
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][2] = pos;
                }
            }
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    default:
        break;
    }
}

// based on ApexWorm/rec2 0af164811 (src/carma2/newcommon/09-funks.c)
// FUNCTION: CARMA2_HW 0x00479890
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;
    br_scalar x_extent;
    br_scalar y_extent;
    br_scalar z_extent;
    tU32 f_the_time = pTime;

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_y:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_z:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pos));
            break;
        }
        break;
    case eGroove_object_rock:
        MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle, pGroove->object_data.rock_info.max_angle);
        rock_it = 1;
        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pos >= pGroove->object_data.rock_info.current_angle) {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            }
        } else {
            if (pGroove->object_interrupt_status == eInterrupt_less_than) {
                if (pos >= pGroove->object_resumption_value || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
            } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
                if (!(pos > pGroove->object_resumption_value) || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
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
        MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude, -pGroove->object_data.throb_info.z_magnitude);
        MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude, -pGroove->object_data.throb_info.y_magnitude);
        MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude, -pGroove->object_data.throb_info.x_magnitude);

        BrMatrix34PostScale(pMat, x_size + 1.f, y_size + 1.f, z_size + 1.f);
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        x_extent = bounds->max.v[0] - bounds->min.v[0];
        y_extent = bounds->max.v[1] - bounds->min.v[1];
        z_extent = bounds->max.v[2] - bounds->min.v[2];
        if (!(pGroove->object_data.shear_info.x_magnitude < 0.0f || pGroove->object_data.shear_info.x_magnitude > 0.0f)) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            BrMatrix34PostShearX(pMat, y_size / y_extent, z_size / z_extent);
        } else if (!(pGroove->object_data.shear_info.y_magnitude < 0.0f || pGroove->object_data.shear_info.y_magnitude > 0.0f)) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearY(pMat, x_size / x_extent, z_size / z_extent);
        } else {
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearZ(pMat, x_size / x_extent, y_size / y_extent);
        }
        break;
    default:
        return;
    }
}
// based on ApexWorm/rec2 c4dadf0df
// FUNCTION: CARMA2_HW 0x004e4650
void C2_HOOK_FASTCALL LollipopizeActor(br_actor* pSubject_actor, br_matrix34* ref_to_world, tLollipop_mode pWhich_axis) {
    br_vector3 ref_to_subject;
    br_vector3 fixed_axis;
    br_vector3 vector_a;
    br_vector3 vector_b;
    br_matrix34 subject_to_world;
    br_matrix34 mat;

    BrActorToActorMatrix34(&subject_to_world, pSubject_actor, gNon_track_actor);
    BrVector3Sub(&ref_to_subject, (br_vector3*)ref_to_world->m[3], (br_vector3*)subject_to_world.m[3]);
    switch (pWhich_axis) {
    case eLollipop_x_match:
        BrVector3SetFloat(&vector_a, 1.f, 0.f, 0.f);
        break;
    case eLollipop_y_match:
        BrVector3SetFloat(&vector_a, 0.f, 1.f, 0.f);
        break;
    case eLollipop_z_match:
        BrVector3SetFloat(&vector_a, 0.f, 0.f, 1.f);
        break;
    default:
        break;
    }
    BrVector3Cross(&vector_b, &ref_to_subject, &vector_a);
    BrVector3Normalise(&vector_b, &vector_b);
    BrVector3Cross(&fixed_axis, &vector_a, &vector_b);
    switch (pWhich_axis) {
    case eLollipop_x_match:
        mat.m[0][0] = vector_a.v[0];
        mat.m[1][0] = vector_a.v[1];
        mat.m[2][0] = vector_a.v[2];
        mat.m[0][1] = vector_b.v[0];
        mat.m[1][1] = vector_b.v[1];
        mat.m[2][1] = vector_b.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    case eLollipop_y_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = vector_a.v[0];
        mat.m[1][1] = vector_a.v[1];
        mat.m[2][1] = vector_a.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    case eLollipop_z_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = fixed_axis.v[0];
        mat.m[1][1] = fixed_axis.v[1];
        mat.m[2][1] = fixed_axis.v[2];
        mat.m[0][2] = vector_a.v[0];
        mat.m[1][2] = vector_a.v[1];
        mat.m[2][2] = vector_a.v[2];
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    default:
        break;
    }
    BrMatrix34Pre(&pSubject_actor->t.t.mat, &mat);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static float MapSawToTriangle(float pNumber) {

    if (pNumber >= 0.5) {
        return 3.0f - pNumber * 4.0f;
    } else {
        return pNumber * 4.0f - 1.0f;
    }
}

static br_scalar NormaliseDegreeAngle(br_scalar pAngle) {

    while (pAngle < .0f) {
        pAngle += 360.f;
    }
    return pAngle;
}
