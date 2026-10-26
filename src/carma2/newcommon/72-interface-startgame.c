#include "72-interface-startgame.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x0059c828
tFrontend_spec gFrontend_START_GAME = {
    "StartGame",
    0,
    0,
    StartGameInfunc,
    NULL,
    FRONTEND_StartGameMenuHandler,
    NULL,
    0,
    0,
    0,
    0,
    0,
    0,
{ { 0 } }
};
