#include "modrend.h"

#include "dbsetup.h"

// GLOBAL: CARMA2_HW 0x00665090
render_style_cbfn RenderStyleCalls[] = {
    renderFaces,                /* BR_RSTYLE_DEFAULT			*/
    nullRender,                 /* BR_RSTYLE_NONE				*/
    renderPoints,               /* BR_RSTYLE_POINTS				*/
    renderEdges,                /* BR_RSTYLE_EDGES				*/
    renderFaces,                /* BR_RSTYLE_FACES				*/
    boundingBoxRenderPoints,    /* BR_RSTYLE_BOUNDING_POINTS	*/
    boundingBoxRenderEdges,     /* BR_RSTYLE_BOUNDING_EDGES		*/
    boundingBoxRenderFaces,     /* BR_RSTYLE_BOUNDING_FACES		*/
};

// GLOBAL: CARMA2_HW 0x00664e98
v11face bounds_faces[] = {
    { { 5, 6, 7 }, { 0, 13, 14 }, { { 1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 5, 4, 6 }, { 3, 4, 0 }, { { 1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 7, 6, 2 }, { 13, 6, 0 }, { { 0.0f, 1.0f, 0.0f, 1.0f } } },
    { { 7, 2, 3 }, { 0, 5, 16 }, { { 0.0f, 1.0f, 0.0f, 1.0f } } },
    { { 1, 5, 7 }, { 11, 14, 0 }, { { 0.0f, 0.0f, 1.0f, 1.0f } } },
    { { 1, 7, 3 }, { 0, 16, 12 }, { { 0.0f, 0.0f, 1.0f, 1.0f } } },
    { { 3, 0, 1 }, { 0, 8, 12 }, { { -1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 3, 2, 0 }, { 5, 1, 0 }, { { -1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 1, 0, 4 }, { 8, 9, 0 }, { { 0.0f, -1.0f, 0.0f, 1.0f } } },
    { { 1, 4, 5 }, { 0, 3, 11 }, { { 0.0f, -1.0f, 0.0f, 1.0f } } },
    { { 0, 6, 4 }, { 0, 4, 9 }, { { 0.0f, 0.0f, -1.0f, 1.0f } } },
    { { 0, 2, 6 }, { 1, 6, 0 }, { { 0.0f, 0.0f, -1.0f, 1.0f } } },
};

// GLOBAL: CARMA2_HW 0x006ad4f0
br_colour bounds_colours[12] = { 0 };

// GLOBAL: CARMA2_HW 0x00664d98
fmt_vertex bounds_vertices[] = {
    { { { -1.0f, -1.0f, -1.0f } }, { { 0.0f, 0.0f } }, { { -0.666f, -0.333f, -0.666f } } },
    { { { -1.0f, -1.0f, 1.0f } }, { { 0.0f, 0.0f } }, { { -0.333f, -0.666f, 0.666f } } },
    { { { -1.0f, 1.0f, -1.0f } }, { { 0.0f, 1.0f } }, { { -0.408f, 0.816f, -0.408f } } },
    { { { -1.0f, 1.0f, 1.0f } }, { { 0.0f, 1.0f } }, { { -0.816f, 0.408f, 0.408f } } },
    { { { 1.0f, -1.0f, -1.0f } }, { { 1.0f, 0.0f } }, { { 0.408f, -0.816f, -0.408f } } },
    { { { 1.0f, -1.0f, 1.0f } }, { { 1.0f, 0.0f } }, { { 0.816f, -0.408f, 0.408f } } },
    { { { 1.0f, 1.0f, -1.0f } }, { { 1.0f, 1.0f } }, { { 0.666f, 0.333f, -0.666f } } },
    { { { 1.0f, 1.0f, 1.0f } }, { { 1.0f, 1.0f } }, { { 0.333f, 0.666f, 0.666f } } },
};

// GLOBAL: CARMA2_HW 0x00664fe8
v11group bounds_face_groups[] = {
    { NULL, bounds_faces, {(void*)bounds_colours}, NULL, bounds_vertices, bounds_colours, NULL, BR_ASIZE(bounds_faces), BR_ASIZE(bounds_vertices), 18 },
};

// GLOBAL: CARMA2_HW 0x00665010
v11model bounds_prepared = {
    0, 0, BR_ASIZE(bounds_face_groups), { { 0 } }, bounds_face_groups,
};


// GLOBAL: CARMA2_HW 0x00665030
br_model bounds_model = {
    0,
    "Bounds",
    NULL,
    NULL,
    0,
    0,
    { { 0.f, 0.f, 0.f } },
    0,
    NULL,
    NULL,
    0,
    0.f,
    { { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } } },
    &bounds_prepared,
    NULL,
    0,
    NULL,
};

// FUNCTION: CARMA2_HW 0x00525fc0
void C2_HOOK_STDCALL renderFaces(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {
    // br_geometry_stored* stored_geometry;
    // br_geometry_v1_model* v1_model;

    if (model->stored != NULL) {
        // stored_geometry = (br_geometry_stored*)model->stored;
        if (on_screen == BRT_ACCEPT) {
            ((br_geometry_stored*)model->stored)->dispatch->_renderOnScreen((br_geometry_stored*)model->stored, v1db.renderer);
        } else {
            ((br_geometry_stored*)model->stored)->dispatch->_render((br_geometry_stored*)model->stored, v1db.renderer);
        }
    } else if (model->prepared != NULL) {
        // v1_model = (br_geometry_v1_model*)v1db.format_model;
        if (on_screen == BRT_ACCEPT) {
            ((br_geometry_v1_model*)v1db.format_model)->dispatch->_renderOnScreenF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, model->prepared, material->stored, BRT_TRIANGLE);
        } else {
            ((br_geometry_v1_model*)v1db.format_model)->dispatch->_renderF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, model->prepared, material->stored, BRT_TRIANGLE);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00526040
void C2_HOOK_STDCALL renderEdges(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    if (on_screen == BRT_ACCEPT) {
        ((br_geometry_v1_model*)v1db.format_model)->dispatch->_renderOnScreenF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, model->prepared, material->stored, BRT_LINE);
    } else {
        ((br_geometry_v1_model*)v1db.format_model)->dispatch->_renderF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, model->prepared, material->stored, BRT_LINE);
    }
}

// FUNCTION: CARMA2_HW 0x00526090
void C2_HOOK_STDCALL renderPoints(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {
    if (on_screen == BRT_ACCEPT) {
        ((br_geometry_v1_model*)v1db.format_model)->dispatch->_renderOnScreenF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, model->prepared, material->stored, BRT_POINT);
    } else {
        ((br_geometry_v1_model*)v1db.format_model)->dispatch->_renderF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, model->prepared, material->stored, BRT_POINT);
    }
}

// FUNCTION: CARMA2_HW 0x005260e0
void C2_HOOK_STDCALL nullRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

}

// FUNCTION: CARMA2_HW 0x005260f0
void C2_HOOK_STDCALL boundingBoxRenderPoints(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    renderPoints(actor, makeMeshFromBounds(&model->bounds), material, render_data, style, on_screen);
}

// PARTIAL 74.32%: makeMeshFromBounds 0x00526150 [structure] - min.x copies int-mov in orig (v0..v3) but x87 here; loop walks with biased pointer; see state/findings/makeMeshFromBounds.md
// FUNCTION: CARMA2_HW 0x00526150
br_model* C2_HOOK_STDCALL makeMeshFromBounds(br_bounds* b) {
    float* src;
    float* dst;
    float fVar1;

    src = (float*)b;

    // 1. Modify vertex points
    fVar1 = src[0];
    bounds_vertices[0].p.v[0] = fVar1;
    fVar1 = src[1];
    bounds_vertices[0].p.v[1] = fVar1;
    fVar1 = src[2];
    bounds_vertices[0].p.v[2] = fVar1;

    fVar1 = src[0];
    bounds_vertices[1].p.v[0] = fVar1;
    fVar1 = src[1];
    bounds_vertices[1].p.v[1] = fVar1;
    fVar1 = src[5];
    bounds_vertices[1].p.v[2] = fVar1;

    fVar1 = src[0];
    bounds_vertices[2].p.v[0] = fVar1;
    fVar1 = src[4];
    bounds_vertices[2].p.v[1] = fVar1;
    fVar1 = src[2];
    bounds_vertices[2].p.v[2] = fVar1;

    fVar1 = src[0];
    bounds_vertices[3].p.v[0] = fVar1;
    fVar1 = src[4];
    bounds_vertices[3].p.v[1] = fVar1;
    fVar1 = src[5];
    bounds_vertices[3].p.v[2] = fVar1;

    fVar1 = src[3];
    bounds_vertices[4].p.v[0] = fVar1;
    fVar1 = src[1];
    bounds_vertices[4].p.v[1] = fVar1;
    fVar1 = src[2];
    bounds_vertices[4].p.v[2] = fVar1;

    fVar1 = src[3];
    bounds_vertices[5].p.v[0] = fVar1;
    fVar1 = src[1];
    bounds_vertices[5].p.v[1] = fVar1;
    fVar1 = src[5];
    bounds_vertices[5].p.v[2] = fVar1;

    fVar1 = src[3];
    bounds_vertices[6].p.v[0] = fVar1;
    fVar1 = src[4];
    bounds_vertices[6].p.v[1] = fVar1;
    fVar1 = src[2];
    bounds_vertices[6].p.v[2] = fVar1;

    fVar1 = src[3];
    bounds_vertices[7].p.v[0] = fVar1;
    fVar1 = src[4];
    bounds_vertices[7].p.v[1] = fVar1;
    fVar1 = src[5];
    bounds_vertices[7].p.v[2] = fVar1;

    // 2. Modify d in the face equation (a*x+b*y+c*y+d=0)
    src = (float*)b;
    dst = &bounds_faces[0].eqn.v[3];
    do {
        fVar1 = src[3];
        dst[7] = fVar1;
        dst[0] = fVar1;
        fVar1 = src[0];
        dst[0x31] = -fVar1;
        dst[0x2a] = -fVar1;
        dst += 14;
        src++;
    } while (dst < &bounds_faces[6].eqn.v[3]);

    // 3. Copy bounds
    bounds_model.bounds = *b;

    return &bounds_model;
}

// FUNCTION: CARMA2_HW 0x00526270
void C2_HOOK_STDCALL boundingBoxRenderEdges(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    renderEdges(actor, makeMeshFromBounds(&model->bounds), material, render_data, style, on_screen);
}

// FUNCTION: CARMA2_HW 0x005262d0
void C2_HOOK_STDCALL boundingBoxRenderFaces(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    renderFaces(actor, makeMeshFromBounds(&model->bounds), material, render_data, style, on_screen);
}
