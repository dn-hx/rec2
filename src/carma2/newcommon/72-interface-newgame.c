#include "72-interface-newgame.h"

#include "72-interface.h"

extern tFrontend_spec gFrontend_MAIN;
int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend);

// GLOBAL: CARMA2_HW 0x005bf280
tFrontend_spec gFrontend_NEWGAME = {
    "newgame",
    0,
    6,
    NewGameInfunc,
    NewGameOutfunc,
    FRONTEND_NewGameMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    10,
    0,
    0,
    {
        { 0, temp,          NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 1, temp,          NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 4, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 3, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 2, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 7, temp,          NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
};
