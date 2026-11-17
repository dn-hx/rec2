/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "65-hull.h"
#include "49-physics.h"

// GLOBAL: CARMA2_HW 0x0067942c
// GLOBAL: CARMA2_SW 0x00673b9c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// GLOBAL: CARMA2_HW 0x00679420
// GLOBAL: CARMA2_SW 0x00673b90
br_vector3 gPhysics_reference_normal_comparison;

// STUB: CARMA2_HW 0x00420910
// STUB: CARMA2_SW 0x00420990
tPhysicsError C2_HOOK_FASTCALL ConvexHull2D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00420d60
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
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {

    return (float)fabs((pV3->v[2] - pV1->v[2]) * (pV2->v[1] - pV1->v[1]) - (pV3->v[1] - pV1->v[1]) * (pV2->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV3->v[0] - pV1->v[0]) * (pV2->v[2] - pV1->v[2]) - (pV2->v[0] - pV1->v[0]) * (pV3->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV2->v[0] - pV1->v[0]) * (pV3->v[1] - pV1->v[1]) - (pV3->v[0] - pV1->v[0]) * (pV2->v[1] - pV1->v[1])) < 1e-6f;
}

// Left

// DRVector4Negate

// ConstructTetrahedron

// CheckSignOfNormal

// AddNewEdge

// PointOutsidePlane

// AdjustEdgePlanes

#define PLANE_VALUE(pPolyhedron, i, pVertex) (BrVector3Dot(pVertex, &(pPolyhedron)->planes[i]) + (pPolyhedron)->planes[i].v[3])

typedef enum {
    ePlaneSide_positive = 0,
    ePlaneSide_negative = 1,
    ePlaneSide_zero = 2,
} tPlane_side;

// FUNCTION: CARMA2_HW 0x00421790
int C2_HOOK_FASTCALL AddPointToPolyhedron(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pVertex, tU8 pVertex_index, tPolyhedron_edge_indexes* pEdge_to_plane_indices) {
    tPolyhedron_edge_indexes new_edges[894];
    tPolyhedron_edge_indexes new_edge_to_plane_indices[894];
    tPlane_side plane_side_buffer[596];
    int positive_side_edges[894];
    br_vector4 new_planes[596];
    int all_positive;
    tU8 i;
    int j;
    int count_new_planes;
    int count_new_edges;
    tU8 final_count_edges;
    tU8 final_count_planes;

    all_positive = 1;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        plane_side_buffer[i] = PLANE_VALUE(pPolyhedron, i, pVertex) < -1e-6f ? ePlaneSide_negative : (PLANE_VALUE(pPolyhedron, i, pVertex) < 1e-6f ? ePlaneSide_zero : ePlaneSide_positive);
        all_positive &= !(plane_side_buffer[i] & ePlaneSide_negative);
    }
    /* If all positive ==> point is inside polyhedron */
    if (all_positive) {
        return 0;
    }

    count_new_planes = 0;
    count_new_edges = 0;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        tU8 plane1;
        tPlane_side side1;
        tU8 plane2;
        tPlane_side side2;

        plane1 = pEdge_to_plane_indices[i].index1;
        side1 = plane_side_buffer[plane1];
        plane2 = pEdge_to_plane_indices[i].index2;
        side2 = plane_side_buffer[plane2];
        positive_side_edges[i] = side1 == ePlaneSide_positive || side2 == ePlaneSide_positive;
        if ((side1 ^ side2) & ePlaneSide_negative) {
            /* Exactly one plane of edge has vertex on negative side */
            if (!(side1 & ePlaneSide_zero) && !(side2 & ePlaneSide_zero)) {
                br_vector3 tv1, tv2;
                /* vertex does not lie on edge */
                BrVector3Sub(&tv1, pVertex, &pPolyhedron->points[pPolyhedron->edges[i].index1]);
                BrVector3Sub(&tv2, &pPolyhedron->points[pPolyhedron->edges[i].index2], &pPolyhedron->points[pPolyhedron->edges[i].index1]);
                BrVector3Cross(&new_planes[count_new_planes], &tv1, &tv2);
                BrVector3Normalise(&new_planes[count_new_planes], &new_planes[count_new_planes]);
                if (BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane1]) * BrVector3Dot(&new_planes[count_new_planes], &pPolyhedron->planes[plane2])
                        - BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane2]) * BrVector3Dot(&new_planes[count_new_planes], &pPolyhedron->planes[plane1]) < 0.f) {
                    BrVector4Negate(&new_planes[count_new_planes], &new_planes[count_new_planes]);
                }
                new_planes[count_new_planes].v[3] = -BrVector3Dot(&new_planes[count_new_planes], pVertex);
                if (side1 == ePlaneSide_positive) {
                    pEdge_to_plane_indices[i].index2 = count_new_planes + pPolyhedron->count_planes;
                } else {
                    pEdge_to_plane_indices[i].index1 = count_new_planes + pPolyhedron->count_planes;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        count_new_planes += 1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                    count_new_planes += 1;
                }
            } else {
                if (!(side1 & ePlaneSide_zero)) {
                    plane1 = plane2;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

            }
        }
    }

    final_count_edges = 0;
    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (positive_side_edges[i]) {
            if (i != final_count_edges) {
                pPolyhedron->edges[final_count_edges] = pPolyhedron->edges[i];
                pEdge_to_plane_indices[final_count_edges] = pEdge_to_plane_indices[i];
            }
            final_count_edges += 1;
        }
    }
    for (i = 0; i < count_new_edges; i++) {
        pPolyhedron->edges[final_count_edges] = new_edges[i];
        pEdge_to_plane_indices[final_count_edges] = new_edge_to_plane_indices[i];
        final_count_edges += 1;
    }
    pPolyhedron->count_edges = final_count_edges;

    final_count_planes = 0;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        if (!(plane_side_buffer[i] & ePlaneSide_negative)) {
            if (i != final_count_planes) {
                BrVector4Copy(&pPolyhedron->planes[final_count_planes], &pPolyhedron->planes[i]);
                for (j = 0; j < pPolyhedron->count_edges; j++) {
                    if (pEdge_to_plane_indices[j].index1 == i) {
                        pEdge_to_plane_indices[j].index1 = final_count_planes;
                    } else if (pEdge_to_plane_indices[j].index2 == i) {
                        pEdge_to_plane_indices[j].index2 = final_count_planes;
                    }
                }
            }
            final_count_planes += 1;
        }
    }
    for (i = 0; i < count_new_planes; i++) {
        BrVector4Copy(&pPolyhedron->planes[final_count_planes], &new_planes[i]);
        for (j = 0; j < pPolyhedron->count_edges; j++) {
            if (pEdge_to_plane_indices[j].index1 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index1 = final_count_planes;
            } else if (pEdge_to_plane_indices[j].index2 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index2 = final_count_planes;
            }
        }
        final_count_planes += 1;
    }
    pPolyhedron->count_planes = final_count_planes;
    return 1;
}

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
tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

// GetPointSample

// SampledConvexHull

// STUB: CARMA2_HW 0x00422500
int C2_HOOK_FASTCALL NPointConvexHullUsingSample(tCollision_shape_polyhedron_data* pPolyhedron, br_model* pModel, int pMin_point_count, int pMax_point_count, int* pVertex_indices) {
    NOT_IMPLEMENTED();
}

