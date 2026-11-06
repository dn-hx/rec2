/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_72_WRECKS_H
#define GUARD_72_WRECKS_H

#include "rec2_types.h"

extern tFrontend_spec gFrontend_WRECKS;

// BuyCurrentCar

extern int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend);

// WrecksUpdateFunc

extern br_actor* gFrontend_wrecks_light;

extern br_pixelmap* gFrontend_wrecks_pixelmap;

extern tWreck_gallery_sell_info gWreck_gallery_sell_infos[32];

extern int gFrontend_wrecks_current;

extern tU32 gFrontend_wrecks_previous_update;

#endif /* GUARD_72_WRECKS_H */
