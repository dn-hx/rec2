#include "33-depth.h"

#include "00-car.h"
#include "15-displays.h"
#include "17-world2.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "07-structur.h"
#include "55-volume.h"
#include "66-tintedpoly.h"
#include "platform.h"
#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"
#include "41-utility.h"
#include "15-displays.h"
#include "52-errors.h"

#include "c2_stdio.h"
#include "c2_math.h"

// GLOBAL: CARMA2_HW 0x00591188
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0079ec1c
br_pixelmap* gSwap_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4e0
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4a0
br_model* gForward_sky_model;

// GLOBAL: CARMA2_HW 0x0067c4b4
br_scalar gSky_height;

// GLOBAL: CARMA2_HW 0x0067c4c8
br_scalar gSky_width;

// GLOBAL: CARMA2_HW 0x0067c4d0
br_scalar gSky_x_multiplier;

// GLOBAL: CARMA2_HW 0x0067c4d4
br_scalar gSky_y_multiplier;

/* defined in globvrbm.c (0x0074cee8), directly before gHUDsquare_actor as in the original layout */
extern br_material* gMaterial[2];

// GLOBAL: CARMA2_HW 0x0059118c
int gDepth_cueing_on = 1;

// GLOBAL: CARMA2_HW 0x00591190
tDepth_effect_type gSwap_depth_effect_type = eDepth_effect_none;

// GLOBAL: CARMA2_HW 0x0079ec4c
int gSwap_depth_effect_start;

// GLOBAL: CARMA2_HW 0x0079ec48
int gSwap_depth_effect_end;

// GLOBAL: CARMA2_HW 0x0079ec34
int gSwap_depth_effect_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ec3c
int gSwap_depth_effect_colour_red;

// GLOBAL: CARMA2_HW 0x0079ec40
int gSwap_depth_effect_colour_green;

// GLOBAL: CARMA2_HW 0x0067c4a8
int gHas_sky_texture;

// GLOBAL: CARMA2_HW 0x0079ec20
br_pixelmap* gDepth_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4bc
int gDepth_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec38
br_pixelmap* gFog_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4a4
int gFog_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec24
br_pixelmap* gAcid_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4dc
int gAcid_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec28
br_pixelmap* gWater_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4cc
int gWater_shade_table_power;

// GLOBAL: CARMA2_HW 0x0067c4d8
br_actor* gForward_sky_actor;

// GLOBAL: CARMA2_HW 0x0067c4ac
br_angle gOld_fov;

// GLOBAL: CARMA2_HW 0x0067c4b8
br_scalar gOld_yon;

// GLOBAL: CARMA2_HW 0x0067c4c0
br_actor* gRearview_sky_actor;

static void C2_HOOK_FASTCALL FrobFog(void);

extern void C2_HOOK_FASTCALL GenerateSmokeShades(void);

// Log2

// CalculateWrappingMultiplier

br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance) {

    return gCamera_yon * (br_scalar)pow(10.0, 0.1 * (double)pDistance);
}

// FUNCTION: CARMA2_HW 0x004451a0
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {
    int start;
    int end;
    int red;
    int green;
    int blue;

    red = gProgram_state.current_depth_effect.colour.red;
    green = gProgram_state.current_depth_effect.colour.green;
    blue = gProgram_state.current_depth_effect.colour.blue;
    start = gProgram_state.current_depth_effect.start;
    end = gProgram_state.current_depth_effect.end;

    switch (gProgram_state.current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags &= ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(red, green, blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

void C2_HOOK_FASTCALL FogCars(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        int j;
        tCar_spec* car = gCurrent_race.opponent_list[i].car_spec;

        for (j = 0; j < car->count_detail_levels; j++) {
            SwitchCarModels(car, j);
            ProcessMaterials(car->car_model_actor, FogAccordingToGPSCDE);
        }
        SwitchCarModels(car, 0);
    }
}

void C2_HOOK_FASTCALL FrobFog(void) {
    int i;
    br_material* material;

    if (gTrack_actor != NULL) {
        ProcessMaterials(gTrack_actor, FogAccordingToGPSCDE);
    }
    if (gNon_track_actor != NULL) {
        ProcessMaterials(gNon_track_actor, FogAccordingToGPSCDE);
    }
    FogCars();

    material = BrMaterialFind("GIBSLICK");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    material = BrMaterialFind("PEDSMEAR");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    for (i = 0; i < (int)REC2_ASIZE(gMaterial); i++) {
        FogAccordingToGPSCDE(gMaterial[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gCurrent_race.material_modifiers); i++) {
        if (gCurrent_race.material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(gCurrent_race.material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(gDefault_track_material);
}

// FUNCTION: CARMA2_HW 0x00445500
void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial) {
    int i;
    float tex_y;
    float f_height;
    int height;
    int vertex_idx;

    if (pMaterial->colour_map == NULL) {
        return;
    }
    if (pMaterial->flags & BR_MATF_MAP_INTERPOLATION) {
        height = pMaterial->colour_map->height - 2;
        tex_y = 1.0f / (float)pMaterial->colour_map->height;
    } else {
        height = pMaterial->colour_map->height;
        tex_y = 0.0f;
    }
    for (i = 0; i < 12; i++) {
        pModel->vertices[i].map.v[1] = 1.0f - tex_y;
    }
    for (i = 80; i < 88; i++) {
        pModel->vertices[i].map.v[1] = tex_y;
    }
    f_height = (float)height;
    vertex_idx = 12;
    for (i = 0; i < 17; vertex_idx+=4, i++) {
        int j;

        pModel->vertices[vertex_idx].map.v[1] = tex_y + (float)(17 - i) * f_height / 18.0f / pMaterial->colour_map->height;
        for (j = 1; j < 4; j++) {
            pModel->vertices[vertex_idx + j].map.v[1] = pModel->vertices[vertex_idx].map.v[1];
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00445340
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {

    if (pType == eDepth_effect_none) {
        pStart = 3;
        pEnd = 3;
    }
    gProgram_state.current_depth_effect.sky_texture = pSky_texture;

    gHorizon_material->colour_map = pSky_texture;
    BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
    MungeSkyVs(gForward_sky_model, gHorizon_material);

    gProgram_state.current_depth_effect.colour.red = pRed;
    gProgram_state.current_depth_effect.colour.green = pGreen;
    gProgram_state.current_depth_effect.colour.blue = pBlue;

    gProgram_state.default_depth_effect.colour.red = pRed;
    gProgram_state.default_depth_effect.colour.green = pGreen;
    gProgram_state.default_depth_effect.colour.blue = pBlue;

    gProgram_state.current_depth_effect.type = pType;
    gProgram_state.current_depth_effect.start = pStart;
    gProgram_state.current_depth_effect.end = pEnd;

    gProgram_state.default_depth_effect.type = pType;
    gProgram_state.default_depth_effect.start = pStart;
    gProgram_state.default_depth_effect.end = pEnd;
    if (gNo_fog && pParam_8) {
        FrobFog();
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// Tan

// EdgeU

// CreateHorizonModel

static int Log2(int pNumber) {
    int i;
    int bits[16];

    bits[0] = 1;
    bits[1] = 2;
    bits[2] = 4;
    bits[3] = 8;
    bits[4] = 16;
    bits[5] = 32;
    bits[6] = 64;
    bits[7] = 128;
    bits[8] = 256;
    bits[9] = 512;
    bits[10] = 1024;
    bits[11] = 2048;
    bits[12] = 4096;
    bits[13] = 0x2000;
    bits[14] = 0x4000;
    bits[15] = 0x8000;
    for (i = REC2_ASIZE(bits) - 1; i >= 0; --i) {
        if (bits[i] & pNumber) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00445860
void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    *pTable = DRPixelmapLoad(the_path);
    if (*pTable == NULL) {
        FatalError(kFatalError_CannotLoadDepthCueShadeTable);
    }
    *pPower = Log2((*pTable)->height);
    for (i = 0; i < (*pTable)->width; i++) {
        for (j = 0; j < (*pTable)->height / 2; j++) {
            temp = PTABLE_PIXEL_AT(i, j);
            PTABLE_PIXEL_AT(i, j) = PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1));
            PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1)) = temp;
        }
    }

#undef PTABLE_PIXEL_AT
}

// FUNCTION: CARMA2_HW 0x00445620
void C2_HOOK_FASTCALL InitDepthEffects(void) {
    br_model* sky_model;
    int row;
    int col;
    int u_base;
    int v_base;
    tU8 band;
    int vtxidx;
    tU8 stripe;

    LoadDepthTable("DEPTHCUE.TAB", &gDepth_shade_table, &gDepth_shade_table_power);
    LoadDepthTable("FOG.TAB", &gFog_shade_table, &gFog_shade_table_power);
    LoadDepthTable("ACIDFOG.TAB", &gAcid_shade_table, &gAcid_shade_table_power);
    LoadDepthTable("BLUEGIT.TAB", &gWater_shade_table, &gWater_shade_table_power);
    GenerateSmokeShades();

    gHorizon_material = BrMaterialFind("HORIZON.MAT");
    if (gHorizon_material == NULL) {
        FatalError(kFatalError_CannotFindSkyMaterial_S);
    }

    if (gScreen->type == BR_PMT_INDEX_8 && gNo_fog == 0) {
        gHorizon_material->index_blend = BrPixelmapAllocate(BR_PMT_INDEX_8, 0x100, 0x100, NULL, 0);
        BrTableAdd(gHorizon_material->index_blend);
        for (row = 0; row < 0x10000; row += 0x100) {
            for (col = 0; col < 0x100; col++) {
                ((tU8*)gHorizon_material->index_blend->pixels)[row + col] = (tU8)col;
            }
        }
        gHorizon_material->flags |= BR_MATF_PERSPECTIVE;
    }

    gHorizon_material->flags |= BR_MATF_MAP_INTERPOLATION;

    sky_model = BrModelAllocate(NULL, 0x58, 0x7e);
    sky_model->flags |= BR_MODF_KEEP_ORIGINAL;

    band = 0;
    vtxidx = 0;
    do {
        u_base = 4 * band;
        v_base = u_base + 4;
        stripe = 0;
        do {
            *((tU16*)((tU8*)sky_model->faces + vtxidx)) = (tU16)(u_base + stripe);
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 2)) = (tU16)(u_base + stripe + 1);
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 4)) = (tU16)(u_base + stripe + 5);
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 0x28)) = (tU16)(u_base + stripe);
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 0x2a)) = (tU16)(u_base + stripe + 1);
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 0x2c)) = (tU16)(v_base + stripe);
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 6)) = 1;
            *((tU16*)((tU8*)sky_model->faces + vtxidx + 0x2e)) = 1;
            *((tU32*)((tU8*)sky_model->faces + vtxidx + 8)) = 0;
            *((tU32*)((tU8*)sky_model->faces + vtxidx + 0x30)) = 0;
            vtxidx += 0x50;
            stripe++;
        } while (stripe < 3);
        band++;
    } while (band < 0x15);

    gForward_sky_model = sky_model;
    BrModelAdd(gForward_sky_model);

    gForward_sky_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gForward_sky_actor->model = gForward_sky_model;
    gForward_sky_actor->material = gHorizon_material;
    gForward_sky_actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(gUniverse_actor, gForward_sky_actor);
}

extern br_angle gSky_image_height;
extern br_angle gSky_image_underground;
extern void C2_HOOK_FASTCALL DRPixelmapRectangleFill(br_pixelmap* dst, br_int_16 x, br_int_16 y, br_uint_16 w, br_uint_16 h, br_uint_32 colour);


// FUNCTION: CARMA2_HW 0x004459e0
void C2_HOOK_FASTCALL DoHorizon(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    int dst_x;
    int src_x;
    int dx;
    int hori_y;
    int top_y;
    int bot_height;
    int repetitions;
    br_angle yaw;
    br_angle hori_sky;
    br_camera* camera;
    br_scalar tan_half_fov;
    br_scalar tan_half_hori_fov;
    br_scalar tan_half_hori_sky;
    br_scalar hshift;
    br_scalar tan_pitch;
    br_scalar sin_v;
    br_scalar cos_v;
    br_scalar col_w;
    int rw;
    int cw;
    tU8 top_col;
    tU8 bot_col;
    br_pixelmap* col_map;

    (void)pDepth_buffer;
    dst_x = 0;
    col_map = gHorizon_material->colour_map;
    camera = (br_camera*)pCamera->type_data;
    sin_v = (br_scalar)sin(BrAngleToRadian((br_scalar)(camera->field_of_view / 2)));
    cos_v = (br_scalar)cos(BrAngleToRadian((br_scalar)(camera->field_of_view / 2)));
    tan_half_fov = sin_v / cos_v;
    tan_half_hori_fov = tan_half_fov * camera->aspect;

    rw = pRender_buffer->width;
    cw = col_map->width;
    col_w = (br_scalar)cw;
    tan_half_hori_sky = tan_half_hori_fov * col_w / rw;
    hori_sky = 2 * BrRadianToAngle(atan2(tan_half_hori_sky, 1.0));
    repetitions = (int)(1.f / (hori_sky * 1.52587890625e-05) + 0.5f);
    hori_sky = 0xfff0 / repetitions;
    yaw = BrRadianToAngle(atan2(pCamera_to_world->m[2][0], pCamera_to_world->m[2][2]));
    hshift = -((yaw * 1.52587890625e-05) / (hori_sky * 1.52587890625e-05));

    src_x = col_map->width * hshift;
    while (src_x < 0) {
        src_x += col_map->width;
    }
    while (src_x > col_map->width) {
        src_x -= col_map->width;
    }

    tan_pitch = pCamera_to_world->m[2][1] / sqrt(pCamera_to_world->m[2][0] * pCamera_to_world->m[2][0] + pCamera_to_world->m[2][2] * pCamera_to_world->m[2][2]);
    hori_y = tan_pitch / tan_half_fov * pRender_buffer->height / 2.0f;
    top_y = gSky_image_underground * col_map->height / gSky_image_height - hori_y - col_map->height;

    while (dst_x < pRender_buffer->width) {
        dx = pRender_buffer->width - dst_x < col_map->width - src_x ? pRender_buffer->width - dst_x : col_map->width - src_x;
        DRPixelmapRectangleCopy(pRender_buffer, dst_x - pRender_buffer->origin_x, top_y, col_map,
                src_x - col_map->origin_x, -col_map->origin_y, dx, col_map->height);
        dst_x += dx;
        src_x = 0;
    }

    top_col = ((tU8*)col_map->pixels)[0];
    if (pRender_buffer->origin_y + top_y > 0) {
        DRPixelmapRectangleFill(pRender_buffer, -pRender_buffer->origin_x, -pRender_buffer->origin_y,
                pRender_buffer->width, pRender_buffer->origin_y + top_y, top_col);
    }

    bot_col = ((tU8*)col_map->pixels)[col_map->row_bytes * (col_map->height - 1)];
    bot_height = pRender_buffer->height - top_y - col_map->height - pRender_buffer->origin_y;
    if (bot_height > 0) {
        DRPixelmapRectangleFill(pRender_buffer, -pRender_buffer->origin_x, top_y + col_map->height,
                pRender_buffer->width, bot_height, bot_col);
    }
}

// DoDepthCue

// DoFog

// DepthEffect

// FUNCTION: CARMA2_HW 0x00445ca0
void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
}

// DepthEffectSky

extern br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees);

// FUNCTION: CARMA2_HW 0x00446680
void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera) {
    float f_time;

    // GLOBAL: CARMA2_HW 0x00591194
    static float mag00 = 0.02f;
    // GLOBAL: CARMA2_HW 0x00591198
    static float mag01 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0059119c
    static float mag02 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911a0
    static float mag10 = 0.15f;
    // GLOBAL: CARMA2_HW 0x005911a4
    static float mag11 = 0.05f;
    // GLOBAL: CARMA2_HW 0x005911a8
    static float mag12 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0067c4e4
    static float mag20;
    // GLOBAL: CARMA2_HW 0x0067c4e8
    static float mag21;
    // GLOBAL: CARMA2_HW 0x0067c4ec
    static float mag22;
    // GLOBAL: CARMA2_HW 0x005911ac
    static float period00 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b0
    static float period01 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b4
    static float period02 = 4000.f;
    // GLOBAL: CARMA2_HW 0x005911b8
    static float period10 = 2200.f;
    // GLOBAL: CARMA2_HW 0x005911bc
    static float period11 = 3300.f;
    // GLOBAL: CARMA2_HW 0x005911c0
    static float period12 = 3100.f;
    // GLOBAL: CARMA2_HW 0x005911c4
    static float period20 = 2800.f;
    // GLOBAL: CARMA2_HW 0x005911c8
    static float period21 = 2500.f;
    // GLOBAL: CARMA2_HW 0x005911cc
    static float period22 = 3900.f;

    f_time = (float)GetTotalTime();
    pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
    pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
    pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
    pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
    pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
    pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
    pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
    pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
    pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
}

// DoDrugWobbleCamera

// DoSubaquaCam

extern tSpecial_volume* gLast_camera_special_volume;

// FUNCTION: CARMA2_HW 0x00446340
void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world) {
    float f_time;
    int r;
    // GLOBAL: CARMA2_HW 0x005911d0
    static float mag00 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d4
    static float mag01 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d8
    static float mag02 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911dc
    static float mag10 = 0.2f;
    // GLOBAL: CARMA2_HW 0x005911e0
    static float mag11 = 0.07f;
    // GLOBAL: CARMA2_HW 0x005911e4
    static float mag12 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911e8
    static float mag20 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911ec
    static float mag21 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911f0
    static float mag22 = 0.01f;
    // GLOBAL: CARMA2_HW 0x005911f4
    static float period00 = 550.f;
    // GLOBAL: CARMA2_HW 0x005911f8
    static float period01 = 700.f;
    // GLOBAL: CARMA2_HW 0x005911fc
    static float period02 = 200.f;
    // GLOBAL: CARMA2_HW 0x00591200
    static float period10 = 100.f;
    // GLOBAL: CARMA2_HW 0x00591204
    static float period11 = 1300.f;
    // GLOBAL: CARMA2_HW 0x00591208
    static float period12 = 500.f;
    // GLOBAL: CARMA2_HW 0x0059120c
    static float period20 = 800.f;
    // GLOBAL: CARMA2_HW 0x00591210
    static float period21 = 1500.f;
    // GLOBAL: CARMA2_HW 0x00591214
    static float period22 = 300.f;

    UpdateTintedPolys();
    if (gOn_drugs && !gAction_replay_mode) {
        TurnTintedPolyOn(gHud_tinted2);
        f_time = (float)(int)PDGetTotalTime();
        pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
        pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
        pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
        pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
        pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
        pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
        pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
        pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
        pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
    } else {
        TurnTintedPolyOff(gHud_tinted2);
        gLast_camera_special_volume = FindSpecialVolume((br_vector3*)pCamera_to_world->m[3], gLast_camera_special_volume, 0);
        if (gLast_camera_special_volume != NULL) {
            if (gLast_camera_special_volume->camera_special_effect_index == 0) {
                DoWobbleCamera(pCamera);
            }
        }
        if (gHas_sky_texture && gSky_on) {
            if ((gProgram_state.current_depth_effect.sky_texture != NULL) == (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col == -2)) {
                ToggleSkyQuietly();
            }
        }
    }
    SetTintedPolyRefMaterial(gHud_tinted1, (br_vector3*)pCamera_to_world->m[3]);
    r = 0;
    if (InWater(gHud_tinted1)) {
        TurnTintedPolyOn(gHud_tinted1);
        r = 1;
    } else {
        TurnTintedPolyOff(gHud_tinted1);
    }
    if (r) {
        DoWobbleCamera(pCamera);
    }
}

// FUNCTION: CARMA2_HW 0x00446890
// FUNCTION: CARMA2_SW 0x00446910
void C2_HOOK_FASTCALL ReduceDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start > 3) {
        gProgram_state.current_depth_effect.start--;
    }
    sprintf(s, "Depth start reduced to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x00446a10
// FUNCTION: CARMA2_SW 0x00446a90
void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;
    br_scalar yon_multiplier;

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); ++i) {
        camera_ptr = gCamera_list[i]->type_data;
        yon_multiplier = gYon_multiplier;
        camera_ptr->yon_z = yon_multiplier * gCamera_yon;
    }
}

// FUNCTION: CARMA2_HW 0x00446a40
// FUNCTION: CARMA2_SW 0x00446ac0
void C2_HOOK_FASTCALL IncreaseYon(void) {
    char s[256];

    gCamera_yon += 5.f;
    AssertYons();
    sprintf(s, GetMiscString(eMiscString_yon_increased_to_D), (int)((br_camera*)gCamera_list[1]->type_data)->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}
// FUNCTION: CARMA2_HW 0x00446ad0
void C2_HOOK_FASTCALL DecreaseYon(void) {
    char text[256];
    br_camera* camera;

    gCamera_yon -= 5.f;
    if (gCamera_yon < 5.0f) {
        gCamera_yon = 5.0f;
    }
    AssertYons();
    camera = gCamera_list[1]->type_data;
    sprintf(text, GetMiscString(eMiscString_yon_decreased_to_D), (int)camera->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, text);
}
// FUNCTION: CARMA2_HW 0x00446b70
void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        if (gCamera_list[i] != NULL) {
            camera_ptr = gCamera_list[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    gCamera_yon = pYon;
}

// FUNCTION: CARMA2_HW 0x00446bb0
br_scalar C2_HOOK_STDCALL GetYon(void) {

    return gCamera_yon;
}

// FUNCTION: CARMA2_HW 0x00446bc0
// FUNCTION: CARMA2_SW 0x00446c40
void C2_HOOK_STDCALL IncreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view += BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view > BrDegreeToAngle(170)) {
            camera_ptr->field_of_view = BrDegreeToAngle(170);
        }
        sprintf(s, "Camera angle increased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446e00
int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return gSky_on;
}

// FUNCTION: CARMA2_HW 0x00446e10
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {

    if (gSky_on != skyTextureOn) {
        ToggleSkyQuietly();
    }
    gSky_on = skyTextureOn;
}

// FUNCTION: CARMA2_HW 0x00446e80
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {
    br_pixelmap* temp;

    temp = gProgram_state.current_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.sky_texture = gSwap_sky_texture;
    gSwap_sky_texture = temp;
    gProgram_state.default_depth_effect.sky_texture = gProgram_state.current_depth_effect.sky_texture;
    if (gHorizon_material != NULL && gProgram_state.current_depth_effect.sky_texture != NULL) {
        gHorizon_material->colour_map = gProgram_state.current_depth_effect.sky_texture;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        MungeSkyVs(gForward_sky_model, gHorizon_material);
    }
}

// FUNCTION: CARMA2_HW 0x00446ee0
void C2_HOOK_FASTCALL ToggleSky(void) {

    SetSkyTextureOn(!GetSkyTextureOn());
    if (gProgram_state.current_depth_effect.sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_on));
    } else if (gSwap_sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_no_sky_texture_for_this_race));
    }
}
// FUNCTION: CARMA2_HW 0x00446f90
int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return gDepth_cueing_on;
}

// FUNCTION: CARMA2_HW 0x00446fa0
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {

    if (gDepth_cueing_on != pOn && gHorizon_material != NULL) {
        ToggleDepthCueingQuietly();
    }
    gDepth_cueing_on = pOn;
}

// FUNCTION: CARMA2_HW 0x00447070
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {
    int temp_red;
    int temp_green;
    int temp_blue;
    int temp_end;
    int temp_start;
    tDepth_effect_type temp_type;

    temp_type = gProgram_state.current_depth_effect.type;
    temp_start = gProgram_state.current_depth_effect.start;
    temp_end = gProgram_state.current_depth_effect.end;
    temp_red = gProgram_state.current_depth_effect.colour.red;
    temp_green = gProgram_state.current_depth_effect.colour.green;
    temp_blue = gProgram_state.current_depth_effect.colour.blue;
    InstantDepthChange(
        gSwap_depth_effect_type,
        gProgram_state.current_depth_effect.sky_texture,
        gSwap_depth_effect_start,
        gSwap_depth_effect_end,
        gSwap_depth_effect_colour_red,
        gSwap_depth_effect_colour_green,
        gSwap_depth_effect_colour_blue,
        gProgram_state.racing);
    gSwap_depth_effect_type = temp_type;
    gSwap_depth_effect_start = temp_start;
    gSwap_depth_effect_end = temp_end;
    gSwap_depth_effect_colour_red = temp_red;
    gSwap_depth_effect_colour_green = temp_green;
    gSwap_depth_effect_colour_blue = temp_blue;
}

// FUNCTION: CARMA2_HW 0x00447110
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {
    int enabled;

    enabled = GetDepthCueingOn();
    SetDepthCueingOn(!enabled);
    if (gProgram_state.current_depth_effect.type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_on));
    } else if (gSwap_depth_effect_type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_there_is_no_depth_cueing_for_this_race));
    }
}
// FUNCTION: CARMA2_HW 0x00447220
void C2_HOOK_FASTCALL ChangeDepthEffect(void) {
    gProgram_state.current_depth_effect.type = gProgram_state.default_depth_effect.type;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.start = gProgram_state.default_depth_effect.start;
    gProgram_state.current_depth_effect.end = gProgram_state.default_depth_effect.end;
}

extern br_angle gSky_image_width;
extern br_angle gSky_image_height;
extern br_angle gSky_image_underground;

static br_scalar MungeSkyModel_Tan(br_scalar pAngle) {
    br_scalar radians;

    radians = BrAngleToRadian(pAngle);
    return sinf(radians) / cosf(radians);
}

static br_scalar MungeSkyModel_EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect) {
    br_scalar a;
    br_scalar b;
    br_scalar c;

    a = (br_scalar)cos(BrAngleToRadian(pPerfect));
    b = (br_scalar)sin(BrAngleToRadian(pView));
    c = (br_scalar)cos(BrAngleToRadian(pView));
    return b * a * a / (BrAngleToRadian(pSky) * (1.f + c));
}

static br_scalar MungeSkyModel_CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon) {
    br_scalar k;
    br_scalar trunc_k;
    int int_k;

    k = pYon * 1.3f * (br_scalar)REC2_TAU / pValue;
    int_k = (int)k;
    trunc_k = (br_scalar)int_k;
    if (k - trunc_k > .5f) {
        trunc_k += 1.f;
    }
    return trunc_k * (br_scalar)(1.0 / REC2_TAU) * pValue;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00445e20
void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel) {
    br_camera* camera_data;
    br_scalar horizon_half_height;
    br_scalar horizon_half_width;
    br_scalar horizon_half_diag;
    br_scalar tan_half_fov;
    br_scalar sky_distance;
    br_angle half_diag_fov;
    tU8 nbands;
    tU8 band;
    tU8 vertex;
    tU8 stripe;
    br_scalar edge_u;
    br_scalar narrow_u;
    br_angle min_angle;
    br_angle angle_range;
    br_angle angle;

    camera_data = pCamera->type_data;
    tan_half_fov = MungeSkyModel_Tan((br_scalar)(camera_data->field_of_view / 2));
    sky_distance = camera_data->yon_z - 1.f;
    horizon_half_width = sky_distance * tan_half_fov;
    horizon_half_height = horizon_half_width * camera_data->aspect;
    horizon_half_diag = (br_scalar)sqrt(horizon_half_height * horizon_half_height + horizon_half_width * horizon_half_width);
    half_diag_fov = BrRadianToAngle(atan2(horizon_half_diag, sky_distance));
    edge_u = MungeSkyModel_EdgeU(gSky_image_width, (br_angle)(2 * half_diag_fov), BR_ANGLE_DEG(10));
    narrow_u = edge_u / 2.f;
    gSky_width = horizon_half_width * 2.f;
    gSky_height = horizon_half_height * 2.f;
    gSky_x_multiplier = MungeSkyModel_CalculateWrappingMultiplier(gSky_width, camera_data->yon_z);
    gSky_y_multiplier = MungeSkyModel_CalculateWrappingMultiplier(gSky_height, camera_data->yon_z);

    for (vertex = 0; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = -edge_u;
    }
    for (vertex = 1; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = -narrow_u;
    }
    for (vertex = 2; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = narrow_u;
    }
    for (vertex = 3; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = edge_u;
    }
    for (vertex = 0; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = -horizon_half_diag;
    }
    for (vertex = 1; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = -horizon_half_diag / 2.f;
    }
    for (vertex = 2; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = horizon_half_diag / 2.f;
    }
    for (vertex = 3; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = horizon_half_diag;
    }
    PossibleService();

    min_angle = (br_angle)(-BR_ANGLE_DEG(90) - half_diag_fov);
    angle_range = (br_angle)(-gSky_image_underground - min_angle);
    for (band = 0; band < 2; ++band) {
        vertex = 4 * band;
        angle = (br_angle)(min_angle + angle_range * band / 2);
        pModel->vertices[vertex].p.v[1] = (br_scalar)(sin(BrAngleToRadian(angle)) * sky_distance);
        pModel->vertices[vertex].p.v[2] = (br_scalar)(-cos(BrAngleToRadian(angle)) * sky_distance);
    }
    min_angle = (br_angle)-gSky_image_underground;
    angle_range = gSky_image_height;
    nbands = 18;
    for (band = 0; band < nbands; ++band) {
        vertex = 4 * band + 8;
        angle = (br_angle)(min_angle + angle_range * band / nbands);
        pModel->vertices[vertex].p.v[1] = (br_scalar)(sin(BrAngleToRadian(angle)) * sky_distance);
        pModel->vertices[vertex].p.v[2] = (br_scalar)(-cos(BrAngleToRadian(angle)) * sky_distance);
    }
    min_angle = (br_angle)(gSky_image_height - gSky_image_underground);
    angle_range = (br_angle)(half_diag_fov + BR_ANGLE_DEG(90) - min_angle);
    for (band = 0; band < 2; ++band) {
        vertex = 4 * band + 80;
        angle = (br_angle)(min_angle + angle_range * band);
        pModel->vertices[vertex].p.v[1] = (br_scalar)(sin(BrAngleToRadian(angle)) * sky_distance);
        pModel->vertices[vertex].p.v[2] = (br_scalar)(-cos(BrAngleToRadian(angle)) * sky_distance);
    }
    PossibleService();
    for (band = 0; band < 22; ++band) {
        vertex = 4 * band;
        for (stripe = 1; stripe < 4; ++stripe) {
            pModel->vertices[vertex + stripe].p.v[1] = pModel->vertices[vertex].p.v[1];
            pModel->vertices[vertex + stripe].p.v[2] = pModel->vertices[vertex].p.v[2];
        }
    }
    BrModelUpdate(pModel, BR_MODU_ALL & ~BR_MODU_FACES);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00447250
void C2_HOOK_FASTCALL MungeForwardSky(void) {
    MungeSkyModel(gCamera, gForward_sky_model);
    gHas_sky_texture = gProgram_state.default_depth_effect.sky_texture != NULL;
}

// FUNCTION: CARMA2_HW 0x00447280
void C2_HOOK_FASTCALL MungeRearviewSky(void) {
}

// FUNCTION: CARMA2_HW 0x00447290
void C2_HOOK_FASTCALL SkyTextureChanged(void) {

    BrMaterialUpdate(gHorizon_material, BR_MATU_COLOURMAP);
}

// FUNCTION: CARMA2_HW 0x004472b0
void C2_HOOK_FASTCALL SetSkyColour(br_colour c) {

    gHorizon_material->colour = c;
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x005156d0
void C2_HOOK_FASTCALL DRPixelmapRectangleFill(br_pixelmap* dst, br_int_16 x, br_int_16 y, br_uint_16 w, br_uint_16 h, br_uint_32 colour) {

    BrPixelmapRectangleFill(dst, x, y, w, h, colour);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
