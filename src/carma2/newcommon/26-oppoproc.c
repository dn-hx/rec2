#include "26-oppoproc.h"

#include "08-loading1.h"

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// GetStraight

// FUNCTION: CARMA2_HW 0x004af8d0
int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec) {

    if (gTime_stamp_for_this_munging >
        (float)(750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1))
            - pOpponent_spec->car_spec->collision_info->M * -30.f
            + (float)(pOpponent_spec->follow_path_data.struggle_time + 2750)) {
        return 1;
    } else {
        return 0;
    }
}

// TimeToBeBrakingWhenStruggling

// FUNCTION: CARMA2_HW 0x004b13c0
// FUNCTION: CARMA2_SW 0x004ad530
void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {

}

// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// SetMaxSpeedFromSOCs

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

#include "globvars.h"
#include "56-aiworld.h"
#include "30-opponent.h"
#include "rec2_macros.h"
#include "brender/brender.h"

extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: Vector2Cross
br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2) { NOT_IMPLEMENTED(); return 0.f; }
// STUB: FollowCheatyPath
tFollow_path_result FollowCheatyPath(tOpponent_spec* pOpponent_spec) { NOT_IMPLEMENTED(); return eFPR_OK; }
// STUB: TimeToBeBrakingWhenStruggling
int C2_HOOK_FASTCALL TimeToBeBrakingWhenStruggling(tOpponent_spec* pOpponent_spec) { NOT_IMPLEMENTED(); return 0; }
// STUB: GetStraight
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec) { NOT_IMPLEMENTED(); return 0; }
// STUB: CalcCorners
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec) { NOT_IMPLEMENTED(); return 0; }
// STUB: CalcSOCs
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar) { NOT_IMPLEMENTED(); return 0; }
// STUB: SetMaxSpeedFromSOCs
void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* socs, int count_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorner, tOpponent_spec* pOpponent_spec) { NOT_IMPLEMENTED(); }
// STUB: NearestSectionInStraight
int C2_HOOK_FASTCALL NearestSectionInStraight(const br_vector2* pPos2d, const br_vector2* pStart2d, const br_vector2* pFinish2d, tS16 pSection_no, tS16 pEnd_section, const tOpponent_spec* pOpponent_spec) { NOT_IMPLEMENTED(); return 0; }
// STUB: MaxCurvatureForCarSpeed
float C2_HOOK_FASTCALL MaxCurvatureForCarSpeed(const tCar_spec* pCar, float pSpeed) { NOT_IMPLEMENTED(); return 0.f; }
// STUB: StuffDataFromCorner
void C2_HOOK_FASTCALL StuffDataFromCorner(tFollow_path_data* pFollow_path, tCorner* pCorner) { NOT_IMPLEMENTED(); }
// STUB: SetCurvature
void C2_HOOK_FASTCALL SetCurvature(tCar_spec* pCar_spec, float pCurvature) { NOT_IMPLEMENTED(); }
// STUB: RematerialiseOpponentOnNearestSection
int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) { NOT_IMPLEMENTED(); return 0; }
// STUB: GetOpponentsSectionFinishNodePoint
const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) { NOT_IMPLEMENTED(); return NULL; }
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// based on carpocalypse2 src/carma2/common/opponent.c (GPL-3.0)
// FUNCTION: CARMA2_HW 0x004af960
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
    tFollow_path_data* data;
    tCorner corners[4];
    tSOC socs[10];
    br_vector3 wank;
    br_vector3 section_dir;
    br_vector3 section_v;
    br_vector3 goal_dir;
    br_actor* car_master_actor;
    br_scalar stopped_speed;
    br_scalar t;
    br_scalar acc;
    br_scalar speed;
    br_scalar dist_along;
    br_scalar acc_factor;
    br_scalar max_acc;
    br_scalar error;
    tCar_spec* car_spec;
    int engine_damage;
    int trans_damage;
    br_vector2 oppo_pos2d;
    br_vector2 start2d;
    br_vector2 finish2d;
    br_vector2 oppo_pos_rel;
    br_vector2 oppo_pos_rel_next;
    br_vector2 v2d;
    br_scalar width;
    br_scalar effective_speed_factor;
    int count_corners;
    int count_socs;
    int straight_section_no;
    br_scalar unk1;
    br_scalar unk2;
    br_scalar unk3;
    br_scalar curv;

    car_spec = pOpponent_spec->car_spec;
    engine_damage = car_spec->damage_units[eDamage_engine].damage_level;
    trans_damage = car_spec->damage_units[eDamage_transmission].damage_level;
    data = &pOpponent_spec->follow_path_data;
    car_master_actor = car_spec->car_master_actor;

    switch (pCommand) {
    case ePOC_start:
        data->first_section_no = GetOpponentsFirstSection(pOpponent_spec);
        data->section_no = data->first_section_no;
        DoNotDprintf_oppoproc("%s: ProcessFollowPath() - new task started, first real section #%d", pOpponent_spec->car_spec->driver_name, GetOpponentsRealSection(pOpponent_spec, data->first_section_no));
        data->has_moved_during_this_task = 0;
        data->struggle_time = 0;
        data->last_finished_struggle_time = gTime_stamp_for_this_munging;
        data->prev_acc = 0.f;
        data->prev_acc_error = 0.f;
        data->borrowed_time_start = gTime_stamp_for_this_munging;
        data->last_struggle_section = -1;
        data->made_it = 1;
        data->cheating = 0;
        data->cornering = 0;
        if (!pOpponent_spec->cheating && !pOpponent_spec->physics_me) {
            DoNotDprintf_oppoproc("%s: Rematerialising from ePOC_start in ProcessFollowPath().+..", pOpponent_spec->car_spec->driver_name);
            /* speed is not initialized in exe */
            RematerialiseOpponentOnNearestSection(pOpponent_spec, BrVector3Length(&car_spec->collision_info->v));
        }
        return eFPR_OK;
    case ePOC_run:
        if (data->cheating || pOpponent_spec->cheating != data->cheating) {
            return FollowCheatyPath(pOpponent_spec);
        }
        if (!pIgnore_end && !data->made_it && gTime_stamp_for_this_munging > data->borrowed_time_start + 1000 && gTime_stamp_for_this_munging < data->borrowed_time_start + 10000) {
            BrVector3Sub(&section_dir, GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no), GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
            BrVector3Sub(&goal_dir, &car_master_actor->t.t.translate.t, GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
            dist_along = BrVector3Dot(&section_dir, &goal_dir) / BrVector3LengthSquared(&section_dir);
            BrVector3Scale(&section_v, &section_dir, dist_along);
            BrVector3Sub(&wank, &goal_dir, &section_v);
            if (GetOpponentsSectionWidth(pOpponent_spec, data->section_no) >= BrVector3Length(&wank)) {
                data->made_it = 1;
            }
        }
        if (gTime_stamp_for_this_munging > data->borrowed_time_start + 10000 && !data->made_it) {
            DoNotDprintf_oppoproc("%s: ProcessFollowPath() giving up due to not making the corner", pOpponent_spec->car_spec->driver_name);
            return eFPR_given_up;
        }
        car_spec->keys.acc = 1;
        speed = WORLD_SCALE * BrVector3Length(&car_spec->collision_info->v);
        if (speed > 0.2f) {
            data->has_moved_during_this_task = 1;
            pOpponent_spec->has_moved_at_some_point = 1;
        }
        if (data->struggle_time != 0) {

            if (!TimeToStopStruggling(pOpponent_spec)) {
                if (TimeToBeBrakingWhenStruggling(pOpponent_spec)) {
                    car_spec->acc_force = 0.f;
                    car_spec->brake_force = 15.f * car_spec->collision_info->M;
                    SetCurvature(car_spec, 0.f);
                } else {
                    car_spec->brake_force = 0.f;
                    car_spec->acc_force = -6.f * car_spec->collision_info->M;
                    SetCurvature(car_spec, 0.f);
                }
                return eFPR_OK;
            }
            DoNotDprintf_oppoproc("%s: done struggling. speed = %.2f m/s", pOpponent_spec->car_spec->driver_name, speed);
            data->made_it = 0;
            data->borrowed_time_start = gTime_stamp_for_this_munging;
            data->struggle_time = 0;
            data->last_finished_struggle_time = gTime_stamp_for_this_munging;
            car_spec->brake_force = 0.0f;
            car_spec->acc_force = 0.0f;
        } else {
            stopped_speed = (pIgnore_end ? 2.f : 6.f) / 30.f;
            if (!pNever_struggle && stopped_speed >= speed && gTime_stamp_for_this_munging > data->last_finished_struggle_time + 2000 && (pPursuit_mode || data->has_moved_during_this_task)) {
                DoNotDprintf_oppoproc("%s: 'Stopped,' section #%d, speed = %.2f m/s, about to start a-strugglin'", pOpponent_spec->car_spec->driver_name, data->section_no, speed);
                data->struggle_time = gTime_stamp_for_this_munging;
                if (pIgnore_end || data->section_no != data->last_struggle_section) {
                    data->last_struggle_section = data->section_no;
                    data->number_of_struggles = 0;
                } else {
                    if (data->number_of_struggles >= 3) {
                        car_spec->acc_force = 0.0f;
                        car_spec->brake_force = 0.0f;
                        DoNotDprintf_oppoproc("%s: Giving up trying to follow path 'cos we've struggled too much", pOpponent_spec->car_spec->driver_name);
                        return eFPR_given_up;
                    }
                    data->number_of_struggles += 1;
                }
            }
        }

        straight_section_no = GetStraight(&start2d, &finish2d, &width, pOpponent_spec->follow_path_data.section_no, pOpponent_spec);
        BrVector2Set(&oppo_pos2d,
            car_spec->car_master_actor->t.t.translate.t.v[2],
            car_spec->car_master_actor->t.t.translate.t.v[0]);
        BrVector2Sub(&oppo_pos_rel_next, &oppo_pos2d, &finish2d);
        BrVector2Set(&v2d,
            car_spec->collision_info->v.v[2],
            car_spec->collision_info->v.v[0]);
        BrVector2Scale(&v2d, &v2d, WORLD_SCALE);
        speed = BrVector2Length(&v2d);
        if (!data->cornering) {
            tS16 nearest_straight_section;

            count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
            nearest_straight_section = NearestSectionInStraight(&oppo_pos2d, &start2d, &finish2d, pOpponent_spec->follow_path_data.section_no, straight_section_no, pOpponent_spec);
            count_socs = CalcSOCs(nearest_straight_section, count_corners, corners, pOpponent_spec, socs, pOpponent_spec->car_spec);
            if (count_corners != 0 && gFrame_period_for_this_munging * speed * .0005f > WORLD_SCALE * (BrVector2Length(&oppo_pos_rel_next) - corners[0].field_0x0c)) {

                StuffDataFromCorner(data, &corners[0]);
                pOpponent_spec->follow_path_data.corner_width = GetOpponentsSectionWidth(pOpponent_spec, corners[0].section);
            } else {
                data->desired_speed = 80.f;
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, &corners[0], pOpponent_spec);
            }
        } else {
            if (BrVector2LengthSquared(&oppo_pos_rel_next) <= REC2_SQR(data->field_0x4c)) {

                count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
                count_socs = CalcSOCs(corners[0].section, count_corners, corners, pOpponent_spec, socs, pOpponent_spec->car_spec);
                if (corners[0].field_0x18 > 80.f) {
                    data->desired_speed = 80.f;
                } else {
                    data->desired_speed = (float)corners[0].field_0x18;
                }
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, &corners[0], pOpponent_spec);
            } else {
                data->cornering = 0;
                data->section_no = data->corner_section;
                if (data->section_no >= 20000) {
                    GetStraight(&start2d, &finish2d, &width, data->section_no, pOpponent_spec);
                }
            }
        }
        if (data->cornering) {
            br_vector2 delta;

            BrVector2Sub(&delta, &oppo_pos2d, &data->field_0x40);
            unk1 = (BrVector2Length(&delta) - data->field_0x48) / data->corner_width;
            if (speed > BR_SCALAR_EPSILON) {
                unk2 = BrVector2Dot(&v2d, &delta) / data->field_0x48 * speed;
            } else {
                unk2 = 0.f;
            }
            unk3 = 1.f / (WORLD_SCALE * data->field_0x48);
            if (data->left_not_right) {
                unk2 = -unk2;
            } else {
                unk1 = -unk1;
                unk3 = -unk3;
            }
        } else {
            br_scalar l;
            br_vector2 delta;
            br_vector2 oppo_pos_rel;

            BrVector2Sub(&delta, &finish2d, &start2d);
            BrVector2Sub(&oppo_pos_rel, &oppo_pos2d, &start2d);
            l = BrVector2Length(&delta);
            if (l <= BR_SCALAR_EPSILON) {
                unk1 = 0.f;
            } else {
                unk1 = Vector2Cross(&oppo_pos_rel, &delta) / l;
                if (unk1 > 0.f) {
                    if (unk1 > l) {
                        unk1 -= l;
                    } else {
                        unk1 /= l;
                    }
                }
            }
            if (l > BR_SCALAR_EPSILON && speed > BR_SCALAR_EPSILON) {
                unk2 = Vector2Cross(&delta, &v2d) / (speed * l);
                unk3 = 0.f;
            } else {
                unk2 = 0.f;
                unk3 = 0.f;
            }
        }
        if (unk1 <= 1.f) {
            data->field_0x08 = 0;
            if (pOpponent_spec->follow_path_data.desired_speed > 6.f) {
                pOpponent_spec->follow_path_data.desired_speed += (6.f - pOpponent_spec->follow_path_data.desired_speed) * fabs(unk2);
            }
            curv = unk3 + MaxCurvatureForCarSpeed(car_spec, speed) * (unk1 / 10.f - unk2);
            if (curv > car_spec->maxcurve) {
                curv = car_spec->maxcurve;
            } else if (curv < -car_spec->maxcurve) {
                curv = -car_spec->maxcurve;
            }
        } else {
            curv = 0.f;
            pOpponent_spec->follow_path_data.desired_speed = 6.f;
            if (speed < 12.0) {
                br_vector2 delta;
                br_scalar l;
                br_scalar crs;

                BrVector2Sub(&delta, &finish2d, &start2d);
                BrVector2Sub(&oppo_pos_rel, &oppo_pos2d, &start2d);
                t = BrVector2Dot(&delta, &oppo_pos_rel) / BrVector2Dot(&delta, &delta);
                if (t < 0.f) {
                    BrVector2Set(&delta, 0.f, 0.f);
                } else if (t <= 1.f) {
                    BrVector2Scale(&delta, &delta, t);
                }
                BrVector2Sub(&delta, &oppo_pos_rel, &delta);
                l = BrVector2Length(&delta);
                crs = Vector2Cross(&v2d, &delta);
                if (speed != 0.f && l != 0.f) {
                    crs /= speed * l;
                }
                curv = -crs * MaxCurvatureForCarSpeed(car_spec, speed);
                if (BrVector2Dot(&delta, &v2d) > 0.f) {
                    if (data->field_0x08 == 0) {
                        data->field_0x38 = 0;
                        data->field_0x08 = gTime_stamp_for_this_munging;
                    } else if (gTime_stamp_for_this_munging - data->field_0x08 > 5000) {
                        data->field_0x08 = gTime_stamp_for_this_munging;
                        data->field_0x38 ^= 0x1;
                    }
                    if (data->field_0x38) {
                        curv = -1.f * curv;
                    }
                }
            }
        }
        effective_speed_factor = (VEHICLE_TYPE_FROM_ID(car_spec->car_ID) == eVehicle_rozzer) ? gCop_speed_factor : gOpponent_speed_factor;
        acc_factor = MAX(effective_speed_factor, 1.f);
        if (engine_damage > 50 && engine_damage < 98) {
            acc_factor -= (engine_damage - 50) / 80.f;
        } else if (engine_damage >= 98) {
            acc_factor -= .6f;
        }
        if (trans_damage > 50 && trans_damage < 98) {
            acc_factor -= (trans_damage - 50) / 160.f;
        } else if (trans_damage >= 98) {
            acc_factor -= .3f;
        }
        if (engine_damage >= 99 || trans_damage >= 99) {
            acc_factor = 0.f;
        }
        max_acc = acc_factor * 12.0f;
        error = data->desired_speed * effective_speed_factor - speed;
        acc = (error - data->prev_acc_error) * 1000.0f / gFrame_period_for_this_munging / 10.f + error + data->prev_acc;
        if (acc > max_acc) {
            acc = max_acc;
        }
        if (acc < -max_acc) {
            acc = -max_acc;
        }
        data->prev_acc = acc;
        data->prev_acc_error = error;
        acc *= car_spec->collision_info->M;
        if (acc > 0.0f) {
            car_spec->acc_force = acc;
            car_spec->brake_force = 0.0f;
        } else {
            car_spec->acc_force = 0.0f;
            car_spec->brake_force = -acc;
        }
        SetCurvature(car_spec, curv);
        return eFPR_OK;
    default:
        BrFatal("C:\\Carma2\\Source\\Common\\Oppoproc.c", 1266, "C:\\Carma2\\Source\\Common\\Oppoproc.c line %d", 1266);
        return eFPR_OK;
    }
}

// FollowCheatyPath

