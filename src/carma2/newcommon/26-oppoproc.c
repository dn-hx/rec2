#include "26-oppoproc.h"

#include "08-loading1.h"

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed);
extern float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);


#include "globvars.h"
#include "56-aiworld.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "rec2_macros.h"
#include "brender/brender.h"
#include "platform.h"
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
static int C2_HOOK_FASTCALL NearestSectionInStraight(const br_vector2* pPos2d, const br_vector2* pStart2d, const br_vector2* pFinish2d, tS16 pSection_no, tS16 pEnd_section, const tOpponent_spec* pOpponent_spec) {
    float dx;
    float dz;
    float dist_along;
    float total;
    int section;
    int next;

    dx = pFinish2d->v[0] - pStart2d->v[0];
    dz = pFinish2d->v[1] - pStart2d->v[1];
    dist_along = ((pPos2d->v[1] - pStart2d->v[1]) * dz + (pPos2d->v[0] - pStart2d->v[0]) * dx) / sqrt(dz * dz + dx * dx);
    total = 0.f;
    section = pSection_no;
    for (;;) {
        const br_vector3* start;
        const br_vector3* finish;

        next = GetOpponentsNextSection(pOpponent_spec, section);
        if (next == pEnd_section) {
            break;
        }
        finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
        start = GetOpponentsSectionStartNodePoint(pOpponent_spec, section);
        dx = start->v[0] - finish->v[0];
        dz = start->v[2] - finish->v[2];
        total = sqrt(dx * dx + dz * dz) + total;
        if (!(total < dist_along)) {
            break;
        }
        section = next;
    }
    return section;
}
static int C2_HOOK_FASTCALL TimeToBeBrakingWhenStruggling(tOpponent_spec* pOpponent_spec) {

    if (gTime_stamp_for_this_munging >
        (float)(750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1))
            - pOpponent_spec->car_spec->collision_info->M * -15.f
            + (float)(pOpponent_spec->follow_path_data.struggle_time + 2000)) {
        return 1;
    } else {
        return 0;
    }
}
static br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2) {

    return pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
}
static float C2_HOOK_FASTCALL MaxCurvatureForCarSpeed(const tCar_spec* pCar, float pSpeed) {
    float result;

    result = pCar->maxcurve;
    if (pSpeed >= 12.5f) {
        result = result * 12.5f / pSpeed;
    }
    return result;
}
// PARTIAL (helper, no address): StuffDataFromCorner, inlined into ProcessFollowPath 0x004af960 at 0x004b1349.
// With this body ProcessFollowPath measures 48.45 (committed 48.75 without it): the not-cornering block lays out
// inline after the cornering block instead of out of line at 0x4b11d3.
// Store mapping from the original (esi = pOpponent_spec, follow_path_data at +0xbc):
//   +0xfc/+0x100 field_0x40 <- corner+0/+4, +0x104 field_0x48 <- corner+8, +0x108 field_0x4c <- corner+0xc,
//   +0x110 corner_section <- corner+0x14, bit 12 left_not_right <- corner+0x10 & 1, bit 11 cornering = 1.
static void C2_HOOK_FASTCALL StuffDataFromCorner(tFollow_path_data* pFollow_path, tCorner* pCorner) {
    pFollow_path->left_not_right = pCorner->left_not_right;
    pFollow_path->cornering = 1;
    pFollow_path->field_0x40 = pCorner->field_0x00;
    pFollow_path->field_0x48 = pCorner->field_0x08;
    pFollow_path->field_0x4c = pCorner->field_0x0c;
    pFollow_path->corner_section = pCorner->section;
}
static void C2_HOOK_FASTCALL SetCurvature(tCar_spec* pCar_spec, float pCurvature) {
    float delta;
    float step;

    delta = pCurvature - pCar_spec->curvature;
    step = gFrame_period_for_this_munging * 0.0005;
    if (delta > step) {
        pCar_spec->curvature += step;
        return;
    }
    if (delta < -step) {
        pCurvature = pCar_spec->curvature - step;
    }
    pCar_spec->curvature = pCurvature;
}

// PARTIAL 48.75%: block layout matches (not-cornering block and 80/field else sunk to the end as in the original);
//   residual: frame 0x118 vs 0x130, rotated section loop (2 GetOpponentsNextSection calls), StuffDataFromCorner stub, FollowCheatyPath
//   is inlined in the original (inlining it here scored 35.98), car_spec spill in the prologue.
static tFollow_path_result C2_HOOK_FASTCALL FollowCheatyPath(tOpponent_spec* pOpponent_spec);
// FUNCTION: CARMA2_HW 0x004af960
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
    tFollow_path_data* data;
    tCorner corners[4];
    tSOC socs[10];
    br_vector3 wank;
    br_vector3 section_dir;
    br_vector3 section_v;
    br_vector3 goal_dir;
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
            BrVector3Sub(&goal_dir, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
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
        if (data->cornering) {
            if (BrVector2LengthSquared(&oppo_pos_rel_next) > REC2_SQR(data->field_0x4c)) {
                data->cornering = 0;
                data->section_no = data->corner_section;
                if (data->section_no >= 20000) {
                    GetStraight(&start2d, &finish2d, &width, data->section_no, pOpponent_spec);
                }
            } else {

                count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
                count_socs = CalcSOCs(corners[0].section, count_corners, corners, pOpponent_spec, socs, pOpponent_spec->car_spec);
                if (80.f < corners[0].field_0x18) {
                    data->desired_speed = 80.f;
                } else {
                    data->desired_speed = (float)corners[0].field_0x18;
                }
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, &corners[0], pOpponent_spec);
            }
        } else {
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
        }
        if (!data->cornering) {
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
        } else {
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

// TimeToBeBrakingWhenStruggling

// FUNCTION: CARMA2_HW 0x004b13c0
// FUNCTION: CARMA2_SW 0x004ad530
void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {

}
// FUNCTION: CARMA2_HW 0x004b13d0
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec) {
    const br_vector3* point;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float width;
    int section;
    int straight;

    section = pSection;
    point = GetOpponentsSectionStartNodePoint(pOpponent_spec, section);
    pStart->v[0] = point->v[2];
    point = GetOpponentsSectionStartNodePoint(pOpponent_spec, section);
    pStart->v[1] = point->v[0];
    point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
    pFinish->v[0] = point->v[2];
    point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
    pFinish->v[1] = point->v[0];
    *pWidth = GetOpponentsSectionWidth(pOpponent_spec, section);
    section = GetOpponentsNextSection(pOpponent_spec, section);
    if (section != -1) {
        for (;;) {
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            fVar2 = point->v[2];
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            fVar1 = point->v[0];
            fVar3 = pFinish->v[0] - pStart->v[0];
            fVar6 = pFinish->v[1] - pStart->v[1];
            fVar4 = (fVar2 - pFinish->v[0]) + fVar3;
            fVar5 = (fVar1 - pFinish->v[1]) + fVar6;
            if ((fVar5 * fVar6 + fVar4 * fVar3 < 0.0) || (0.1 <= fabs((fVar5 * fVar3 + -fVar6 * fVar4) / sqrt(fVar5 * fVar5 + fVar4 * fVar4)))) {
                straight = 0;
            } else {
                straight = 1;
            }
            if (!straight) {
                break;
            }
            pFinish->v[0] = fVar2;
            pFinish->v[1] = fVar1;
            width = GetOpponentsSectionWidth(pOpponent_spec, section);
            if (width < *pWidth) {
                *pWidth = width;
            }
            section = GetOpponentsNextSection(pOpponent_spec, section);
            if (section == -1) {
                return -1;
            }
        }
    }
    return section;
}

// StuffDataFromCorner

// SetCurvature




#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
static tFollow_path_result C2_HOOK_FASTCALL FollowCheatyPath(tOpponent_spec* pOpponent_spec) {
    tFollow_path_data* data;
    tCar_spec* car_spec;
    br_actor* car_master_actor;
    const br_vector3* pStart;
    const br_vector3* pFinish;
    br_vector3 v;
    br_scalar d;
    br_scalar dx;
    br_scalar dy;
    br_scalar dz;
    br_scalar len;
    br_scalar t;
    br_scalar speed;

    data = &pOpponent_spec->follow_path_data;
    car_spec = pOpponent_spec->car_spec;
    car_master_actor = car_spec->car_master_actor;
    pStart = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
    pFinish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
    if (data->cheating != pOpponent_spec->cheating) {
        data->cheating = pOpponent_spec->cheating;
        if (pOpponent_spec->cheating) {
            DoNotDprintf_oppoproc("%s: Dematerialising", car_spec->driver_name);
            dx = pFinish->v[0] - pStart->v[0];
            dy = pFinish->v[1] - pStart->v[1];
            dz = pFinish->v[2] - pStart->v[2];
            d = ((car_master_actor->t.t.translate.t.v[0] - pStart->v[0]) * dx + (car_master_actor->t.t.translate.t.v[2] - pStart->v[2]) * dz + (car_master_actor->t.t.translate.t.v[1] - pStart->v[1]) * dy) / (dz * dz + dy * dy + dx * dx);
            if (d < 0.f) {
                data->cheaty_intersect = *pStart;
            } else if (d > 1.f) {
                data->cheaty_intersect = *pFinish;
            } else {
                data->cheaty_intersect.v[0] = pStart->v[0] + d * dx;
                data->cheaty_intersect.v[1] = pStart->v[1] + d * dy;
                data->cheaty_intersect.v[2] = pStart->v[2] + d * dz;
            }
            dx = data->cheaty_intersect.v[0] - car_master_actor->t.t.translate.t.v[0];
            dy = data->cheaty_intersect.v[1] - car_master_actor->t.t.translate.t.v[1];
            dz = data->cheaty_intersect.v[2] - car_master_actor->t.t.translate.t.v[2];
            t = gFrame_period_for_this_munging * 0.001 * 6.0f;
            TurnOpponentPhysicsOff(pOpponent_spec);
            len = (br_scalar)sqrt(dz * dz + dy * dy + dx * dx);
            if (len < t) {
                data->moving_to_intersect = 0;
                car_master_actor->t.t.translate.t = data->cheaty_intersect;
                return eFPR_OK;
            }
            data->moving_to_intersect = 1;
            if (len <= BR_SCALAR_EPSILON * 2.f) {
                dx = 1.f;
                dy = 0.f;
                dz = 0.f;
            } else {
                len = 1.0 / len;
                dx = len * dx;
                dy = len * dy;
                dz = len * dz;
            }
            car_master_actor->t.t.translate.t.v[0] = t * dx + car_master_actor->t.t.translate.t.v[0];
            car_master_actor->t.t.translate.t.v[1] = t * dy + car_master_actor->t.t.translate.t.v[1];
            car_master_actor->t.t.translate.t.v[2] = t * dz + car_master_actor->t.t.translate.t.v[2];
            return eFPR_OK;
        }
        DoNotDprintf_oppoproc("%s: Rematerialising from FollowCheatyPath()...", car_spec->driver_name);
        if (data->section_no < 0) {
            PDFatalError("No useful section number in ProcessCheatyPath()");
        }
        dx = pFinish->v[0] - car_master_actor->t.t.translate.t.v[0];
        dy = pFinish->v[1] - car_master_actor->t.t.translate.t.v[1];
        dz = pFinish->v[2] - car_master_actor->t.t.translate.t.v[2];
        len = (br_scalar)sqrt(dx * dx + dy * dy + dz * dz);
        if (GetOpponentsSectionMaxSpeed(pOpponent_spec, data->section_no, 1) < 255) {
            speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, data->section_no, 1) * (1.0 / 6.9);
        } else if (GetOpponentsSectionMinSpeed(pOpponent_spec, data->section_no, 1) > 0) {
            speed = GetOpponentsSectionMinSpeed(pOpponent_spec, data->section_no, 1) * (1.0 / 6.9);
        } else {
            speed = MAX(1.f, MIN(7.f, 2.f * len));
        }
        if (RematerialiseOpponentOnThisSection(pOpponent_spec, data->section_no, speed)) {
            car_spec->brake_force = 0.f;
            car_spec->acc_force = 0.f;
            return eFPR_OK;
        }
        data->cheating = 1;
    }
    car_spec = pOpponent_spec->car_spec;
    car_master_actor = car_spec->car_master_actor;
    car_spec->collision_info->v.v[0] = 0.f;
    car_spec->collision_info->v.v[1] = 6.0f * WORLD_SCALE;
    car_spec->collision_info->v.v[2] = 0.f;
    t = gFrame_period_for_this_munging * 0.001 * 6.0f;
    if (data->moving_to_intersect == 0) {
        while (t > 0.f) {
            dx = pFinish->v[0] - car_master_actor->t.t.translate.t.v[0];
            dy = pFinish->v[1] - car_master_actor->t.t.translate.t.v[1];
            dz = pFinish->v[2] - car_master_actor->t.t.translate.t.v[2];
            len = (br_scalar)sqrt(dx * dx + dy * dy + dz * dz);
            if (len >= t) {
                d = t / len;
                t = 0.f;
                car_master_actor->t.t.translate.t.v[0] += d * dx;
                car_master_actor->t.t.translate.t.v[1] += d * dy;
                car_master_actor->t.t.translate.t.v[2] += d * dz;
            } else {
                car_master_actor->t.t.translate.t = *pFinish;
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - current section %d(#%d) (cheating)", car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
                data->section_no = GetOpponentsNextSection(pOpponent_spec, data->section_no);
                if (data->section_no == -1) {
                    car_spec->acc_force = 0.f;
                    car_spec->brake_force = 0.f;
                    DoNotDprintf_oppoproc("%s: ProcessFollowPath() - reached end of path while cheating", car_spec->driver_name);
                    return eFPR_end_of_path;
                }
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - next section %d(#%d) (cheating)", car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
                pStart = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
                pFinish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
                t -= len;
            }
        }
        v.v[0] = pFinish->v[0] - car_master_actor->t.t.translate.t.v[0];
        v.v[1] = pFinish->v[1] - car_master_actor->t.t.translate.t.v[1];
        v.v[2] = pFinish->v[2] - car_master_actor->t.t.translate.t.v[2];
        PointActorAlongThisBloodyVector(car_master_actor, &v);
        return eFPR_OK;
    }
    dx = data->cheaty_intersect.v[0] - car_master_actor->t.t.translate.t.v[0];
    dy = data->cheaty_intersect.v[1] - car_master_actor->t.t.translate.t.v[1];
    dz = data->cheaty_intersect.v[2] - car_master_actor->t.t.translate.t.v[2];
    len = (br_scalar)sqrt(dz * dz + dy * dy + dx * dx);
    if (len < t) {
        data->moving_to_intersect = 0;
        car_master_actor->t.t.translate.t = data->cheaty_intersect;
        v.v[0] = pFinish->v[0] - pStart->v[0];
        v.v[1] = pFinish->v[1] - pStart->v[1];
        v.v[2] = pFinish->v[2] - pStart->v[2];
        PointActorAlongThisBloodyVector(car_master_actor, &v);
        return eFPR_OK;
    }
    if (len <= BR_SCALAR_EPSILON * 2.f) {
        d = 1.f;
        dx = 0.f;
        dy = 0.f;
    } else {
        len = 1.0 / len;
        d = dx * len;
        dx = dy * len;
        dy = dz * len;
    }
    car_master_actor->t.t.translate.t.v[0] = t * d + car_master_actor->t.t.translate.t.v[0];
    car_master_actor->t.t.translate.t.v[1] = t * dx + car_master_actor->t.t.translate.t.v[1];
    car_master_actor->t.t.translate.t.v[2] = t * dy + car_master_actor->t.t.translate.t.v[2];
    v.v[0] = t * d;
    v.v[1] = t * dx;
    v.v[2] = t * dy;
    PointActorAlongThisBloodyVector(car_master_actor, &v);
    return eFPR_OK;
}
// PARTIAL 39.78%: block-scoped d/nfy/sq/len/dot/cross/r; frame 0x30 vs 0x38; OPEN S00,S01 (prologue/loop structure) RESIDUAL S02 (epilogue)
// FUNCTION: CARMA2_HW 0x004b1560
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec) {
    float width;
    float fx;
    float fy;
    br_vector2 start;
    float dx;
    float dy;
    br_vector2 prev;
    br_vector2 finish;
    tCorner* corner;
    int count;

    count = 0;
    width = pWidth;
    prev = *pStart;
    corner = pCorners;
    while (pSection != -1 && count < 4) {
        corner->section = pSection;
        pSection = GetStraight(&start, &finish, &pWidth, pSection, pOpponent_spec);
        dx = prev.v[0] - start.v[0];
        dy = prev.v[1] - start.v[1];
        fx = finish.v[0] - start.v[0];
        fy = finish.v[1] - start.v[1];
        {
            float len1;
            float len2;
            float d;
            float nfy;
            float sq;
            float len;
            float dot;
            float cross;
            float r;

            len1 = dx * dx + dy * dy;
            len2 = fx * fx + fy * fy;
            if (len1 == 0.0f || len2 == 0.0f) {
                goto simple_corner;
            }
            nfy = -fy;
            dot = fy * dy + fx * dx;
            cross = fx * dy + dx * nfy;
            len = sqrt(cross * cross + dot * dot);
            if (len <= 2.3841858e-07f) {
                dot = 1.0f;
                cross = 0.0f;
            } else {
                len = 1.0 / len;
                dot = len * dot;
                cross = len * cross;
            }
            if (cross > 0.0f) {
                corner->left_not_right = 1;
            } else {
                corner->left_not_right = 0;
                cross = -cross;
            }
            if (cross < 0.001f) {
                goto simple_corner;
            }
            d = (dot * pWidth + width) / cross;
            r = cross / (dot - -1.0f);
            sq = sqrt(((r * r * pWidth - d * r * -2.0f) - pWidth) * pWidth);
            corner->field_0x0c = r * pWidth + sq + d;
            if (corner->field_0x0c * corner->field_0x0c * 9.0f > len1) {
                corner->field_0x0c = sqrt(len1) * 0.33333334f;
            }
            if (corner->field_0x0c * corner->field_0x0c * 9.0f > len2) {
                corner->field_0x0c = sqrt(len2) * 0.33333334f;
            }
            r = r * corner->field_0x0c;
            corner->field_0x08 = r;
            if (corner->left_not_right) {
                corner->field_0x00.v[0] = r * nfy + fx * corner->field_0x0c;
                corner->field_0x00.v[1] = fy * corner->field_0x0c + fx * corner->field_0x08;
            } else {
                corner->field_0x00.v[0] = fx * corner->field_0x0c + fy * r;
                corner->field_0x00.v[1] = fy * corner->field_0x0c + -corner->field_0x08 * fx;
            }
            corner->field_0x00.v[0] = corner->field_0x00.v[0] / sqrt(len2) + start.v[0];
            corner->field_0x00.v[1] = corner->field_0x00.v[1] / sqrt(len2) + start.v[1];
        }
        goto next_corner;
simple_corner:
        corner->field_0x00.v[0] = start.v[0];
        corner->field_0x00.v[1] = start.v[1];
        corner->field_0x08 = MIN(width, pWidth);
        corner->field_0x0c = corner->field_0x08 * 0.7f;
        corner->left_not_right = IRandomBetween(0, 1);
next_corner:
        count++;
        corner++;
        width = pWidth;
        prev = start;
    }
    return count;
}
// FUNCTION: CARMA2_HW 0x004b18b0
void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* socs, int count_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorner, tOpponent_spec* pOpponent_spec) {
    const br_vector3* point;
    float dist = 0.f;
    float last;
    float threshold;
    double max_speed;
    float prev_z;
    float prev_x;
    float v2;
    float v0;
    float dz;
    float dx;
    int i;
    int bVar8 = 0;

    threshold = (pSpeed * pSpeed * 0.0625f - (float)gFrame_period_for_this_munging * pSpeed * -0.001f) * 0.14492753f;
    prev_z = pPos2d->v[0];
    prev_x = pPos2d->v[1];
    for (i = 0; i < count_socs; i++) {
        if (dist >= threshold) {
            return;
        }
        if (bVar8) {
            dist = dist - last;
        }
        if ((*(int*)&socs[i] & 1) == 0) {
            bVar8 = 1;
            point = GetOpponentsSectionStartNodePoint(pOpponent_spec, pCorner[socs[i].field_0x4].section);
            max_speed = (double)(int)pCorner[socs[i].field_0x4].field_0x18;
            last = pCorner[socs[i].field_0x4].field_0x0c;
            dist = dist - last;
        } else {
            bVar8 = 0;
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)socs[i].field_0x4);
            max_speed = (double)GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)socs[i].field_0x4, 1);
        }
        v2 = point->v[2];
        v0 = point->v[0];
        dz = prev_z - v2;
        dx = prev_x - v0;
        dist = sqrt(dx * dx + dz * dz) + dist;
        if (dist < 0.f) {
            dist = 0.f;
        }
        if (dist * 6.9f < (pSpeed * pSpeed - max_speed * max_speed) * 0.0625f - (float)gFrame_period_for_this_munging * pSpeed * -0.001f) {
            if (max_speed < *pDesired_speed) {
                *pDesired_speed = max_speed;
            }
        }
        prev_z = v2;
        prev_x = v0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// PARTIAL 79.88%: structure, call sequence, SOC writes and return value match; residual is MSVC codegen.
//   (1) the 2D-length <=/> corner->field_0x0c compares compile to fld [ebp-8] + fcompp + fxch/fstp with the
//       opposite flag test (test ah,0x1) instead of the original's `fcomp dword ptr [ebp-8]` + test ah,0x41,
//       in both places; (2) our frame is 0xc where the original reserves 0x24, so every [esp+N] operand and
//       all relative jump displacements render differently. Both callers (0x4b0131, 0x4b12dd) use the
//       returned count, and every exit path leaves the count in eax.
// FUNCTION: CARMA2_HW 0x004b1ab0
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar) {
    int count;
    int i;
    int section;
    tCorner* corner;

    count = 0;
    section = pNext_section;
    for (i = 0, corner = pCorners; i < pCount_corners; i++, corner++) {
        while (section != -1) {
            const br_vector3* start;
            const br_vector3* finish;
            float dx;
            float dz;

            if (count >= 10) {
                goto next_corner;
            }
            if (section == (int)corner->section) {
                break;
            }
            start = GetOpponentsSectionStartNodePoint(pOpponent_spec, corner->section);
            finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            dx = finish->v[0] - start->v[0];
            dz = finish->v[2] - start->v[2];
            if (sqrt(dx * dx + dz * dz) <= corner->field_0x0c) {
                break;
            }
            pSocs[count].field_0x0 |= 1;
            pSocs[count].field_0x4 = section;
            count += 1;
            section = GetOpponentsNextSection(pOpponent_spec, section);
        }
        if (count >= 10) {
            goto next_corner;
        }
        pSocs[count].field_0x0 &= ~1;
        pSocs[count].field_0x4 = i;
        corner->field_0x18 = (int)sqrt(corner->field_0x08 * 6.9001222f * 7.5f * 1.96f);
        while (section != (int)corner->section) {
            if ((int)corner->field_0x18 > GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1)) {
                corner->field_0x18 = GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1);
            }
            section = GetOpponentsNextSection(pOpponent_spec, section);
        }
        while (section != -1) {
            const br_vector3* start;
            const br_vector3* finish;
            float dx;
            float dz;

            start = GetOpponentsSectionStartNodePoint(pOpponent_spec, corner->section);
            finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, section);
            dx = finish->v[0] - start->v[0];
            dz = finish->v[2] - start->v[2];
            if (sqrt(dx * dx + dz * dz) >= corner->field_0x0c) {
                break;
            }
            if ((int)corner->field_0x18 > GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1)) {
                corner->field_0x18 = GetOpponentsSectionMaxSpeed(pOpponent_spec, section, 1);
            }
            section = GetOpponentsNextSection(pOpponent_spec, section);
        }
        count += 1;
next_corner:;
    }
    return count;
}

#include "18-graphics2.h"
#include "25-grafdata.h"
#include "38-flicplay.h"
#include "69-sound.h"

// GLOBAL: CARMA2_HW 0x0065bd78
tFlic_cycle_spec gFlic_cycle_specs[2] = {
    { 4, 36, 0, { 132, 175, 221, 253, 0 } },
    { 3, 45, 0, { 132, 157, 217, 0, 0 } },
};

// PARTIAL 75%: current load CSE vs original reload
// FUNCTION: CARMA2_HW 0x004b2430
int C2_HOOK_FASTCALL FUN_004b2430(int* pIndex) {
    int prev;
    int cur;
    int n;
    int m;
    int idx;

    DRS3StartSound(gEffects_outlet, 3000);
    idx = *pIndex;
    n = idx - 2;
    prev = gFlic_cycle_specs[n].current - 1;
    if (prev < 0) {
        prev = gFlic_cycle_specs[n].count - 1;
    }
    m = n;
    cur = ((int*)&gFlic_cycle_specs[0])[m * 8 + 2];
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(287, gFlic_cycle_specs[m].x[cur], gFlic_cycle_specs[m].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(288, gFlic_cycle_specs[m].x[prev], gFlic_cycle_specs[m].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    gFlic_cycle_specs[m].current = prev;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b24f0
int C2_HOOK_FASTCALL FUN_004b24f0(int* pIndex) {
    int n;
    int current;
    int count;
    int new_current;
    int y;
    int x;

    DRS3StartSound(gEffects_outlet, 3000);
    n = *pIndex - 2;
    current = gFlic_cycle_specs[n].current;
    count = gFlic_cycle_specs[n].count;
    new_current = current + 1;
    if (new_current == count) {
        new_current = 0;
    }
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    y = gFlic_cycle_specs[n].y;
    x = gFlic_cycle_specs[n].x[current];
    RunFlicAt(287, x, y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    y = gFlic_cycle_specs[n].y;
    x = gFlic_cycle_specs[n].x[new_current];
    RunFlicAt(288, x, y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    gFlic_cycle_specs[n].current = new_current;
    return 1;
}

// GLOBAL: CARMA2_HW 0x006918cc
const tInterface_spec* gInterface_spec;

// FUNCTION: CARMA2_HW 0x004b2640
int C2_HOOK_FASTCALL RadioClick(int* pNew_choice, int* pNew_mode, int pX, int pY) {
    int i;
    int n;
    int current;
    int x;
    int y;

    i = gFlic_cycle_specs[*pNew_choice - 2].count - 1;
    if (i >= 0) {
        int limit = gInterface_spec->mouse_areas[2].left[gReal_graf_data_index] + 3 + pX;
        int* px = &gFlic_cycle_specs[*pNew_choice - 2].x[i];

        while (1) {
            if (limit >= *px) {
                break;
            }
            i--;
            px--;
            if (i < 0) {
                return 0;
            }
        }
        DRS3StartSound(gEffects_outlet, 3000);
        n = *pNew_choice - 2;
        current = gFlic_cycle_specs[n].current;
        RemoveTransientBitmaps(1);
        DontLetFlicFuckWithPalettes();
        TurnFlicTransparencyOn();
        y = gFlic_cycle_specs[n].y;
        x = gFlic_cycle_specs[n].x[current];
        RunFlicAt(287, x, y);
        TurnFlicTransparencyOff();
        LetFlicFuckWithPalettes();
        RemoveTransientBitmaps(1);
        DontLetFlicFuckWithPalettes();
        TurnFlicTransparencyOn();
        y = gFlic_cycle_specs[n].y;
        x = gFlic_cycle_specs[n].x[i];
        RunFlicAt(288, x, y);
        TurnFlicTransparencyOff();
        LetFlicFuckWithPalettes();
        gFlic_cycle_specs[n].current = i;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b2740
int C2_HOOK_FASTCALL FUN_004b2740(int* pIndex) {
    int n;
    int current;
    int count;
    int new_current;
    int y;
    int x;

    DRS3StartSound(gEffects_outlet, 3000);
    n = *pIndex - 2;
    current = gFlic_cycle_specs[n].current;
    count = gFlic_cycle_specs[n].count;
    new_current = current + 1;
    if (new_current == count) {
        new_current = 0;
    }
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    y = gFlic_cycle_specs[n].y;
    x = gFlic_cycle_specs[n].x[current];
    RunFlicAt(287, x, y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    y = gFlic_cycle_specs[n].y;
    x = gFlic_cycle_specs[n].x[new_current];
    RunFlicAt(288, x, y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    gFlic_cycle_specs[n].current = new_current;
    return 0;
}
