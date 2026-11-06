/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_72_NETSYNC_H
#define GUARD_72_NETSYNC_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETSYNC;

extern int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend);

extern int gFrontend_scrollbars_updated;

extern tFrontend_slider* gCurrent_frontend_scrollbars;

extern tConnected_items* gConnected_items;

extern tStruct_00686508* gPTR_00686508;

extern tNet_mode gFrontend_net_mode;

#endif /* GUARD_72_NETSYNC_H */
