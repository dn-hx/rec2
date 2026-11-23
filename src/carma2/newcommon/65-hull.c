/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "65-hull.h"

// GLOBAL: CARMA2_HW 0x0067942c
// GLOBAL: CARMA2_SW 0x00673b9c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// GLOBAL: CARMA2_HW 0x00679420
// GLOBAL: CARMA2_SW 0x00673b90
// GLOBAL: CARMA2_CHEAT 0x00680cd0
br_vector3 gPhysics_reference_normal_comparison;

// STUB: CARMA2_HW 0x00420910
// STUB: CARMA2_SW 0x00420990
// STUB: CARMA2_CHEAT 0x00420b70
tPhysicsError C2_HOOK_FASTCALL ConvexHull2D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00420d60
// FUNCTION: CARMA2_CHEAT 0x00420fc0
br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2) {
    br_scalar x;
    br_scalar y;
    br_scalar z;

    y = pV1->v[2] * pV2->v[0] - pV1->v[0] * pV2->v[2];
    x = pV1->v[1] * pV2->v[2] - pV1->v[2] * pV2->v[1];
    z = pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
    return x * gPhysics_reference_normal_comparison.v[0] + y * gPhysics_reference_normal_comparison.v[1] + z * gPhysics_reference_normal_comparison.v[2];
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x00420dc0
// FUNCTION: CARMA2_SW 0x00420e40
// FUNCTION: CARMA2_CHEAT 0x00421010
int C2_HOOK_CDECL CompareAngle(const void *pLhs, const void *pRhs) {
    tCollision_shape_polyhedron_data* polyhedron;
    br_scalar s1, s2;
    br_vector3 tv1, tv2;
    const int* const pIndex1 = pLhs;
    const int* const pIndex2 = pRhs;

    polyhedron = gPolyhedron_to_sort;
    BrVector3Sub(&tv1, &polyhedron->points[*pIndex1], &polyhedron->points[0]);
    BrVector3Sub(&tv2, &polyhedron->points[*pIndex2], &polyhedron->points[0]);
    s1 = SignedTwiceAreaOf3DTriangle(&tv1, &tv2);
    if (s1 > 1e-6f) {
        return -1;
    }
    if (s1 < -1e-6f) {
        return 1;
    }
    s1 = BrVector3Length(&tv1);
    s2 = BrVector3Length(&tv2);
    if (s1 < s2) {
        return -1;
    }
    if (s1 > s2) {
        return 1;
    }
    return 0;
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// FUNCTION: CARMA2_HW 0x00420ef0
// FUNCTION: CARMA2_SW 0x00420f70
// FUNCTION: CARMA2_CHEAT 0x00421140
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {

    return (float)fabs((pV3->v[2] - pV1->v[2]) * (pV2->v[1] - pV1->v[1]) - (pV3->v[1] - pV1->v[1]) * (pV2->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV3->v[0] - pV1->v[0]) * (pV2->v[2] - pV1->v[2]) - (pV2->v[0] - pV1->v[0]) * (pV3->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV2->v[0] - pV1->v[0]) * (pV3->v[1] - pV1->v[1]) - (pV3->v[0] - pV1->v[0]) * (pV2->v[1] - pV1->v[1])) < 1e-6f;
}

// Left

// DRVector4Negate

// FUNCTION: CARMA2_HW 0x004211f0
tPhysicsError C2_HOOK_FASTCALL ConstructTetrahedron(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdges) {
    int first_non_colinear;
    int i;
    br_vector3 tv2;
    br_vector3 tv1;
    br_scalar len;
    double scale;

    first_non_colinear = 0;
    while (Collinear(&pPolyhedron->points[first_non_colinear + 0], &pPolyhedron->points[first_non_colinear + 1], &pPolyhedron->points[first_non_colinear + 2])) {
        first_non_colinear++;
        if (first_non_colinear == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }

    if (first_non_colinear != 0) {
        pPolyhedron->points[0].v[0] = pPolyhedron->points[first_non_colinear + 0].v[0];
        pPolyhedron->points[0].v[1] = pPolyhedron->points[first_non_colinear + 0].v[1];
        pPolyhedron->points[0].v[2] = pPolyhedron->points[first_non_colinear + 0].v[2];
        pPolyhedron->points[1].v[0] = pPolyhedron->points[first_non_colinear + 1].v[0];
        pPolyhedron->points[1].v[1] = pPolyhedron->points[first_non_colinear + 1].v[1];
        pPolyhedron->points[1].v[2] = pPolyhedron->points[first_non_colinear + 1].v[2];
        pPolyhedron->points[2].v[0] = pPolyhedron->points[first_non_colinear + 2].v[0];
        pPolyhedron->points[2].v[1] = pPolyhedron->points[first_non_colinear + 2].v[1];
        pPolyhedron->points[2].v[2] = pPolyhedron->points[first_non_colinear + 2].v[2];
    }
    pPolyhedron->edges[0].index1 = 0;
    pPolyhedron->edges[0].index2 = 1;
    pPolyhedron->edges[1].index1 = 1;
    pPolyhedron->edges[1].index2 = 2;
    pPolyhedron->edges[2].index1 = 0;
    pPolyhedron->edges[2].index2 = 2;

    /* PlaneEquation */
    tv1.v[0] = pPolyhedron->points[2].v[0] - pPolyhedron->points[1].v[0];
    tv1.v[1] = pPolyhedron->points[2].v[1] - pPolyhedron->points[1].v[1];
    tv1.v[2] = pPolyhedron->points[2].v[2] - pPolyhedron->points[1].v[2];
    tv2.v[0] = pPolyhedron->points[1].v[0] - pPolyhedron->points[0].v[0];
    tv2.v[1] = pPolyhedron->points[1].v[1] - pPolyhedron->points[0].v[1];
    tv2.v[2] = pPolyhedron->points[1].v[2] - pPolyhedron->points[0].v[2];
    pPolyhedron->planes[0].v[0] = tv2.v[1] * tv1.v[2] - tv2.v[2] * tv1.v[1];
    pPolyhedron->planes[0].v[1] = tv2.v[2] * tv1.v[0] - tv2.v[0] * tv1.v[2];
    pPolyhedron->planes[0].v[2] = tv2.v[0] * tv1.v[1] - tv2.v[1] * tv1.v[0];
    len = BR_LENGTH3(pPolyhedron->planes[0].v[0], pPolyhedron->planes[0].v[1], pPolyhedron->planes[0].v[2]);
    if (len > BR_SCALAR_EPSILON * 2) {
        scale = 1.0 / len;
        pPolyhedron->planes[0].v[0] = pPolyhedron->planes[0].v[0] * scale;
        pPolyhedron->planes[0].v[1] = pPolyhedron->planes[0].v[1] * scale;
        pPolyhedron->planes[0].v[2] = pPolyhedron->planes[0].v[2] * scale;
    } else {
        pPolyhedron->planes[0].v[0] = 1.0f;
        pPolyhedron->planes[0].v[1] = 0.0f;
        pPolyhedron->planes[0].v[2] = 0.0f;
    }
    pPolyhedron->planes[0].v[3] = -(pPolyhedron->planes[0].v[1] * pPolyhedron->points[0].v[1] + pPolyhedron->planes[0].v[2] * pPolyhedron->points[0].v[2] + pPolyhedron->planes[0].v[0] * pPolyhedron->points[0].v[0]);

    pEdges[2].index1 = 0;
    pEdges[1].index1 = 0;
    pEdges[0].index1 = 0;

    for (i = 0; i < pPolyhedron->count_points; i++) {

        if ((float)fabs(pPolyhedron->planes[0].v[0] * pPolyhedron->points[i].v[0] + pPolyhedron->planes[0].v[1] * pPolyhedron->points[i].v[1] + pPolyhedron->planes[0].v[2] * pPolyhedron->points[i].v[2] + pPolyhedron->planes[0].v[3]) > 1e-6f) {
            break;
        }
    }
    if (i == pPolyhedron->count_points) {
        return 7;
    }
    pPolyhedron->points[3].v[0] = pPolyhedron->points[i].v[0];
    pPolyhedron->points[3].v[1] = pPolyhedron->points[i].v[1];
    pPolyhedron->points[3].v[2] = pPolyhedron->points[i].v[2];
    /* Points inside the polyhedron must have positive plane value */
    if (pPolyhedron->planes[0].v[0] * pPolyhedron->points[i].v[0] + pPolyhedron->planes[0].v[1] * pPolyhedron->points[i].v[1] + pPolyhedron->planes[0].v[2] * pPolyhedron->points[i].v[2] + pPolyhedron->planes[0].v[3] < 1e-6f) {
        pPolyhedron->planes[0].v[0] = -pPolyhedron->planes[0].v[0];
        pPolyhedron->planes[0].v[1] = -pPolyhedron->planes[0].v[1];
        pPolyhedron->planes[0].v[2] = -pPolyhedron->planes[0].v[2];
        pPolyhedron->planes[0].v[3] = -pPolyhedron->planes[0].v[3];
    }
    for (i = 0; i < 3; i++) {
        int o;
        int o_i;

        o = i + 1;
        o_i = o % 3;

        pPolyhedron->edges[3 + i].index1 = i;
        pPolyhedron->edges[3 + i].index2 = 3;

        /* PlaneEquation */
        tv1.v[0] = pPolyhedron->points[3].v[0] - pPolyhedron->points[i].v[0];
        tv1.v[1] = pPolyhedron->points[3].v[1] - pPolyhedron->points[i].v[1];
        tv1.v[2] = pPolyhedron->points[3].v[2] - pPolyhedron->points[i].v[2];
        tv2.v[0] = pPolyhedron->points[o_i].v[0] - pPolyhedron->points[i].v[0];
        tv2.v[1] = pPolyhedron->points[o_i].v[1] - pPolyhedron->points[i].v[1];
        tv2.v[2] = pPolyhedron->points[o_i].v[2] - pPolyhedron->points[i].v[2];
        pPolyhedron->planes[o].v[0] = tv1.v[1] * tv2.v[2] - tv1.v[2] * tv2.v[1];
        pPolyhedron->planes[o].v[1] = tv1.v[2] * tv2.v[0] - tv1.v[0] * tv2.v[2];
        pPolyhedron->planes[o].v[2] = tv1.v[0] * tv2.v[1] - tv1.v[1] * tv2.v[0];
        len = BR_LENGTH3(pPolyhedron->planes[o].v[0], pPolyhedron->planes[o].v[1], pPolyhedron->planes[o].v[2]);
        if (len > BR_SCALAR_EPSILON * 2) {
            scale = 1.0 / len;
            pPolyhedron->planes[o].v[0] = pPolyhedron->planes[o].v[0] * scale;
            pPolyhedron->planes[o].v[1] = pPolyhedron->planes[o].v[1] * scale;
            pPolyhedron->planes[o].v[2] = pPolyhedron->planes[o].v[2] * scale;
        } else {
            pPolyhedron->planes[o].v[0] = 1.0f;
            pPolyhedron->planes[o].v[1] = 0.0f;
            pPolyhedron->planes[o].v[2] = 0.0f;
        }
        pPolyhedron->planes[o].v[3] = -(pPolyhedron->planes[o].v[1] * pPolyhedron->points[3].v[1] + pPolyhedron->planes[o].v[2] * pPolyhedron->points[3].v[2] + pPolyhedron->planes[o].v[0] * pPolyhedron->points[3].v[0]);

        pEdges[i].index2 = o;
        pEdges[3 + i].index1 = o;
        pEdges[3 + i].index2 = 1 + (o + 1) % 3;
    }

    for (i = 0; i < 3; i++) {

        if (pPolyhedron->planes[1 + i].v[0] * pPolyhedron->points[(2 + i) % 3].v[0] + pPolyhedron->planes[1 + i].v[1] * pPolyhedron->points[(2 + i) % 3].v[1] + pPolyhedron->planes[1 + i].v[2] * pPolyhedron->points[(2 + i) % 3].v[2] + pPolyhedron->planes[1 + i].v[3] < 1e-6f) {
            pPolyhedron->planes[1 + i].v[0] = -pPolyhedron->planes[1 + i].v[0];
            pPolyhedron->planes[1 + i].v[1] = -pPolyhedron->planes[1 + i].v[1];
            pPolyhedron->planes[1 + i].v[2] = -pPolyhedron->planes[1 + i].v[2];
            pPolyhedron->planes[1 + i].v[3] = -pPolyhedron->planes[1 + i].v[3];
        }
    }

    pPolyhedron->count_points = 4;
    pPolyhedron->count_planes = 4;
    pPolyhedron->count_edges = 6;
    return ePhysicsError_Ok;
}

// CheckSignOfNormal

// AddNewEdge

// PointOutsidePlane

// AdjustEdgePlanes

// AddPointToPolyhedron

#ifdef REC2_MATCHING
// STUB: CARMA2_HW 0x00421fd0
// STUB: CARMA2_SW 0x00422050
undefined4* C2_HOOK_FASTCALL FUN_00421fd0(int pCount_points) {
    NOT_IMPLEMENTED();
    /* Unused */
}
#endif

// AdjustEdges

// PointOnShape

// STUB: CARMA2_HW 0x00420fb0
// STUB: CARMA2_SW 0x00421030
// STUB: CARMA2_CHEAT 0x00421200
tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

// GetPointSample

// SampledConvexHull

// NPointConvexHullUsingSample

