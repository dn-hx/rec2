#ifndef GUARD_58_CRUSH2_H
#define GUARD_58_CRUSH2_H

#include "c2_hooks.h"
#include "rec2_types.h"

typedef struct {
    tCar_spec* car;
    float step;
    float* total_displacement;
    float displacement;
} tCrush_displacement_context_local;

typedef struct {
    tCar_spec* car;
    float step;
    float displacement;
    float* total_displacement;
} tModel_mash_context_local;

extern tU32 gLast_credit_award_time;
extern int gCredits_wasting_car[3];
extern float gCredits_per_unit_car_damage[3];
extern int gTime_rolling_car[3];
extern int gCredits_rolling_car[3];
extern float gSeconds_per_unit_car_damage[3];
extern int gTime_wasting_car[3];
extern int gTime_per_ped_kill[3];

// CheckLastCar

extern void C2_HOOK_FASTCALL KnackerThisCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ExplodeMeCarUpMeOldMatey(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar);

// DamageUnit2

// RecordLastDamage

// DamageUnit2

extern void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity);

// DamageSystems

// GetDirection

extern void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);

extern void C2_HOOK_FASTCALL DoWheelDamage(void);
extern void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);

extern int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar);

// ChristmasLights

extern intptr_t C2_HOOK_CDECL AddTotalDeflection(br_actor* pActor, tCrush_displacement_context_local* pContext);
extern int C2_HOOK_FASTCALL RepairObject(tPhysics_object* pObject, tCrush_displacement_context_local* pContext);
extern intptr_t C2_HOOK_CDECL RepairModels_2(br_actor* pActor, tModel_mash_context_local* pContext);
extern int C2_HOOK_FASTCALL RepairModel(br_model* pModel, tModel_mash_context_local* pContext);

#endif // GUARD_58_CRUSH2_H
