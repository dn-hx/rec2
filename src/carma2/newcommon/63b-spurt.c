#include "61-pedestrn.h"

#include "23-piping.h"
#include "41-utility.h"
#include "51-smash.h"
#include "rec2_types.h"

#include <stdlib.h>

/* gPed_spurt_id is the first int of the (still unnamed) pair gUNK_0065d7a0 owned by 61-pedestrn.c */
extern int gUNK_0065d7a0[2];
extern br_model* gPed_spurt_model;
extern int gPed_spurty_lumps_count;
extern int gPed_spurt_min_max_time[2];
extern tSmashable_initial_speed_spec gPed_spurt_initial_speed;
extern tSmashable_initial_position_spec gPed_spurt_initial_position;
extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8);
extern void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

// (stub only; body from REA 0x004d5820)
// FUNCTION: CARMA2_HW 0x004d5820
void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pTransform, br_vector3* pPosition, br_vector3* pDirection, br_vector3* pRandom_direction) {
    tU32 time;
    int seed;
    int saved_random_seed;
    int spurt_id;
    int i;
    tSmash_vertex* fragment;
    float scale;
    br_vector3 position;
    br_vector3 vec;
    br_vector3 direction;

    time = GetTotalTime();
    BrMatrix34ApplyP(&position, pPosition, pTransform);
    BrMatrix34ApplyV(&direction, pDirection, pTransform);
    vec.v[0] = pRandom_direction->v[0] * 6.9;
    vec.v[1] = pRandom_direction->v[1] * 6.9;
    vec.v[2] = pRandom_direction->v[2] * 6.9;
    seed = (int)position.v[0] ^ (int)position.v[1] ^ (int)position.v[2] ^ time;
    spurt_id = gUNK_0065d7a0[0];
    saved_random_seed = rand();
    srand(seed);
    for (i = 0; i < gPed_spurty_lumps_count; i++) {
        fragment = FindShrapnelFragment(gPed_spurt_model, spurt_id, 1);
        if (fragment == NULL) {
            break;
        }
        scale = PositionFragment(fragment->actor, &gPed_spurt_initial_position, 0, (int)&position, (int)&position, 0, NULL, 0);
        InitFragmentSpeed(fragment, &gPed_spurt_initial_speed, &vec, scale, gPed_spurt_min_max_time, time, &direction, 0, 0);
    }
    srand(saved_random_seed);
    PipeSingleBloodSpurt(gUNK_0065d7a0[0], time, seed, &position, &vec, &direction);
    gUNK_0065d7a0[0]--;
}
