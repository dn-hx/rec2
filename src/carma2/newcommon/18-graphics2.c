#include "18-graphics2.h"

#include "25-grafdata.h"
#include "33-depth.h"
#include "41-utility.h"
#include "52-errors.h"
#include "66-tintedpoly.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0058fdc8
int gCursor_tinted_top = -1;

// GLOBAL: CARMA2_HW 0x0058fdd0
int gCursor_tinted_left = -1;

// GLOBAL: CARMA2_HW 0x0058fdcc
int gCursor_tinted_bottom = -1;

// GLOBAL: CARMA2_HW 0x0058fdd4
int gCursor_tinted_right = -1;

// GLOBAL: CARMA2_HW 0x0058fdd8
int gCursor_tinted_center = -1;

// GLOBAL: CARMA2_HW 0x0058fde0
int gCursor_line_width = 5;

/* explicit initializers keep gTransient_bitmaps, gNext_transient and the gap dword adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x0067be98
tTransient_bm gTransient_bitmaps[50] = { 0 };

// GLOBAL: CARMA2_HW 0x0067c348
int gNext_transient = 0;

/* UNCERTAIN: unreferenced dword, only seen as gTransient_bitmaps loop end */
// GLOBAL: CARMA2_HW 0x0067c34c
int gUNK_0067c34c = 0;

/* UNCERTAIN: unreferenced dwords, only seen as gTransient_bitmaps loop ends (field walks) */
// GLOBAL: CARMA2_HW 0x0067c350
int gUNK_0067c350 = 0;

// GLOBAL: CARMA2_HW 0x0067c354
int gUNK_0067c354 = 0;

// GLOBAL: CARMA2_HW 0x0067c358
int gUNK_0067c358 = 0;

// GLOBAL: CARMA2_HW 0x006923a4
tU32 gLast_palette_change;

// GLOBAL: CARMA2_HW 0x006923b4
int gPalette_index;

// GLOBAL: CARMA2_HW 0x0074ca28
int gNoTransients;

// GLOBAL: CARMA2_HW 0x0074a674
br_pixelmap* gRender_palette;

// GLOBAL: CARMA2_HW 0x0079ec14
int gDim_amount;

// GLOBAL: CARMA2_HW 0x0074a680
char* gCurrent_palette_pixels;

// GLOBAL: CARMA2_HW 0x0074cf04
int gPalette_changed;

// GLOBAL: CARMA2_HW 0x0074a678
br_pixelmap* gCurrent_palette;

// GLOBAL: CARMA2_HW 0x006923ac
br_pixelmap* gOrig_render_palette;

// GLOBAL: CARMA2_HW 0x0074a67c
br_pixelmap* gFlic_palette;

// GLOBAL: CARMA2_HW 0x006923c8
int gFaded_palette;

// GLOBAL: CARMA2_HW 0x006923c0
int gPalette_munged;

// GLOBAL: CARMA2_HW 0x006923b8
br_uint_8* gScratch_pixels;

// GLOBAL: CARMA2_HW 0x006923a8
br_pixelmap* gScratch_palette;

// GLOBAL: CARMA2_HW 0x0074a604
br_pixelmap* gPalette_0074a604;

// GLOBAL: CARMA2_HW 0x0074a600
br_pixelmap* gPalette_0074a600;

// GLOBAL: CARMA2_HW 0x0074a66c
br_pixelmap* gPalette_0074a66c;

// GLOBAL: CARMA2_HW 0x0074a5fc
br_pixelmap* gPalette_0074a5fc;

// GLOBAL: CARMA2_HW 0x0074a670
br_pixelmap* gPalette_0074a670;

// GLOBAL: CARMA2_HW 0x0074a660
br_pixelmap* gMini_map_glowing_line_palettes[3];

// GLOBAL: CARMA2_HW 0x0068be20
br_pixelmap* gCurrent_splash;

// FUNCTION: CARMA2_HW 0x004b4fd0
void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pThe_palette, int pFirst_colour, int pCount) {

    if (!pFirst_colour) {
        ((br_int_32*)pThe_palette->pixels)[0] = 0;
    }
    memcpy(gCurrent_palette_pixels + 4 * pFirst_colour, (char*)pThe_palette->pixels + 4 * pFirst_colour, 4 * pCount);
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPaletteEntries(pThe_palette, pFirst_colour, pCount);
    }
    gPalette_munged = 1;
}

void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette) {

    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= pThe_palette != gRender_palette;
}

void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette) {

    ((br_int_32*)pThe_palette->pixels)[0] = 0;
    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= pThe_palette != gRender_palette;
}

// FUNCTION: CARMA2_HW 0x004b5030
void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette) {

    DRSetPalette2(pThe_palette, 1);
}

// FUNCTION: CARMA2_HW 0x004b5090
void C2_HOOK_FASTCALL InitializePalettes(void) {
    br_pixelmap* render_palette;

    gCurrent_palette_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gPalette_changed = 0;
    gCurrent_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gCurrent_palette_pixels, 0);
    gRender_palette = BrTableFind("DRRENDER.PAL");
    if (gRender_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    NobbleNonzeroBlacks(gRender_palette);
    gOrig_render_palette = BrPixelmapAllocateSub(gRender_palette, 0, 0, gRender_palette->width, gRender_palette->height);
    gOrig_render_palette->pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    memcpy(gOrig_render_palette->pixels, gRender_palette->pixels, 256 * sizeof(br_uint_32));
    gFlic_palette = BrTableFind("DRACEFLC.PAL");
    if (gFlic_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    render_palette = gRender_palette;
    ((br_uint_32*)render_palette->pixels)[0] = 0;
    memcpy(gCurrent_palette_pixels, render_palette->pixels, 256 * sizeof(br_uint_32));
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(render_palette);
    }
    gPalette_munged |= render_palette != gRender_palette;
    gScratch_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gScratch_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gScratch_pixels, 0);
    gMini_map_glowing_line_palettes[0] = (br_pixelmap*)(intptr_t)-1; /* FIXME: invalid pointer! */
    gMini_map_glowing_line_palettes[1] = gPalette_0074a604;
    gMini_map_glowing_line_palettes[2] = gPalette_0074a600;
    gPalette_0074a66c = gPalette_0074a5fc;
    gPalette_0074a670 = NULL;
}

// FUNCTION: CARMA2_HW 0x004b52a0
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {

    gLast_palette_change = 0;
    gPalette_index = 0;
}

// FUNCTION: CARMA2_HW 0x004b52b0
void C2_HOOK_FASTCALL RevertPalette(void) {

    memcpy(gRender_palette->pixels, gOrig_render_palette->pixels, 256 * sizeof(br_colour));
    DRSetPalette3(gRender_palette, 1);
}
// FUNCTION: CARMA2_HW 0x004b5320
void C2_HOOK_FASTCALL MungePalette(void) {
}

// FUNCTION: CARMA2_HW 0x004b5330
void C2_HOOK_FASTCALL ResetPalette(void) {

    InitPaletteAnimate();
    DRSetPalette(gRender_palette);
}

// Darken

// SetFadedPalette

// FUNCTION: CARMA2_HW 0x004b5460
void C2_HOOK_FASTCALL FadePaletteDown(void) {
}

// EnsurePaletteUp

// FUNCTION: CARMA2_HW 0x004b5770
void C2_HOOK_FASTCALL EnsureRenderPalette(void) {

    if (gPalette_munged) {
        ResetPalette();
        gPalette_munged = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
        gTransient_bitmaps[i].pixmap = NULL;
        gTransient_bitmaps[i].in_use = 0;
    }
}

// DeallocateTransientBitmap

// DeallocateAllTransientBitmaps

// FUNCTION: CARMA2_HW 0x0043e010
void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    int i;
    int order_number;

    if (!gNoTransients && pGraphically_remove_them) {
        for (order_number = gNext_transient - 1; order_number >= 0; order_number--) {
            for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
                if (gTransient_bitmaps[i].pixmap != NULL && order_number == gTransient_bitmaps[i].order_number) {
                    if (gTransient_bitmaps[i].in_use) {
                        BrPixelmapRectangleCopy(gBack_screen,
                            gTransient_bitmaps[i].x_coord,
                            gTransient_bitmaps[i].y_coord,
                            gTransient_bitmaps[i].pixmap,
                            0,
                            0,
                            gTransient_bitmaps[i].pixmap->width,
                            gTransient_bitmaps[i].pixmap->height);
                    }
                    break;
                }
            }
        }
    }
    gNext_transient = 0;
}

// SaveTransient

// STUB: CARMA2_HW 0x0043e0a0
int C2_HOOK_FASTCALL DoMouseCursor(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043e3f0
void C2_HOOK_FASTCALL AllocateCursorTransient(void) {
    br_pixelmap* cross;
    br_pixelmap* line;

    BrMapFind("mse_corn");
    cross = BrMapFind("mse_cros");
    line = BrMapFind("mse_line");
    gCursor_tinted_top = CreateTintedPoly(0, 0, gCursor_line_width, gCurrent_graf_data->height, 1, 127, 192, 0);
    gCursor_tinted_left = CreateTintedPoly(0, 0, gCurrent_graf_data->width, gCursor_line_width, 1, 127, 192, 0);
    gCursor_tinted_bottom = CreateTintedPoly(0, 0, gCursor_line_width, gCurrent_graf_data->height, 1, 127, 192, 0);
    gCursor_tinted_right = CreateTintedPoly(0, 0, gCurrent_graf_data->width, gCursor_line_width, 1, 127, 192, 0);
    gCursor_tinted_center = CreateTintedPoly(0, 0, 16, 16, 1, 127, 192, 0);
    if (line != NULL) {
        gTintedPolys[gCursor_tinted_left].material->colour_map = line;
        gTintedPolys[gCursor_tinted_left].material->ka = 1.0f;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_left].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_right].material->colour_map = line;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_right].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_top].material->colour_map = line;
        BrMatrix23Rotate(&gTintedPolys[gCursor_tinted_top].material->map_transform, BR_ANGLE_DEG(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_top].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_bottom].material->colour_map = line;
        BrMatrix23Rotate(&gTintedPolys[gCursor_tinted_bottom].material->map_transform, BR_ANGLE_DEG(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_bottom].material, BR_MATU_ALL);
    }
    if (cross != NULL) {
        gTintedPolys[gCursor_tinted_center].material->colour_map = cross;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_center].material, BR_MATU_ALL);
    }
}

// InitMouseTargetLines

// STUB: CARMA2_HW 0x0043e710
void C2_HOOK_FASTCALL EndMouseCursor(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0047b450
void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour) {

    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapLine(pDestn, pX1, pY1, pX2, pY2, pColour);
}

// FUNCTION: CARMA2_HW 0x0047b880
void C2_HOOK_FASTCALL ClearEntireScreen(void) {

    if (gScreen != NULL) {
        BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
    }
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    PDScreenBufferSwap(0);
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
}

// DistanceFromPlane

// FUNCTION: CARMA2_HW 0x0047b960
void C2_HOOK_FASTCALL KillSplashScreen(void) {

    if (gCurrent_splash != NULL) {
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// SplashScreenWith

// FUNCTION: CARMA2_HW 0x0047ba80
void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
    int y;
    int x;
    int dest_row_wrap;
    int source_row_wrap;
    tU16 the_byte;
    tU16* source_ptr;
    tU16* dest_ptr;
    int dest_x;
    int dest_y;

    source_ptr = (tU16*)pSource->pixels + pSource->row_bytes * pSource_y / sizeof(tU16) + pSource_x;
    dest_x = pDest_x;
    dest_y = pDest_y;
    dest_ptr = (tU16*)pDest->pixels + dest_y * pDest->row_bytes / sizeof(tU16) + pDest->base_x / sizeof(tU16) + dest_x;
    source_row_wrap = pSource->row_bytes / sizeof(tU16) - pWidth;
    dest_row_wrap = pDest->row_bytes / sizeof(tU16) - pWidth;

    if (dest_y < 0) {
        pHeight += dest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= dest_y * pSource->row_bytes / sizeof(tU16);
        dest_ptr -= dest_y * pDest->row_bytes / sizeof(tU16);
        dest_y = 0;
    }
    if (dest_y < pDest->height) {
        if (pDest->height < dest_y + pHeight) {
            pHeight = pDest->height - dest_y;
        }
        if (dest_x < 0) {
            pWidth += dest_x;
            if (pWidth <= 0) {
                return;
            }
            dest_x = 0;
            source_ptr -= dest_x;
            dest_ptr -= dest_x;
            source_row_wrap -= dest_x;
            dest_row_wrap -= dest_x;
        }
        if (dest_x < pDest->width) {
            if (dest_x + pWidth > pDest->width) {
                pWidth -= pWidth + dest_x - pDest->width;
                source_row_wrap += pWidth + dest_x - pDest->width;
                dest_row_wrap += pWidth + dest_x - pDest->width;
            }
            for (y = 0; y < pHeight; y++) {
                for (x = 0; x < pWidth; x++) {
                    the_byte = *source_ptr;
                    if (the_byte != 0) {
                        *dest_ptr = the_byte;
                    }
                    source_ptr++;
                    dest_ptr++;
                }
                source_ptr += source_row_wrap;
                dest_ptr += dest_row_wrap;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047bc40
// FUNCTION: CARMA2_SW 0x0047af60
void C2_HOOK_FASTCALL DRMaskedStamp(br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource) {

    DRPixelmapRectangleMaskedCopy(gBack_screen, pDest_x, pDest_y, pSource, 0, 0, pSource->width, pSource->height);
}

// FUNCTION: CARMA2_HW 0x0047c650
int C2_HOOK_FASTCALL SwitchToHiresMode(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0047c660
int C2_HOOK_FASTCALL SwitchToLoresMode(void) {

    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* local declarations (globvrbm.h is not included in this file) */
extern br_model* gDim_model;
extern br_actor* gDim_actor;
extern br_actor* g2d_camera;

// FUNCTION: CARMA2_HW 0x0047cad0
void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    gDim_model->vertices[1].p.v[0] = (float)pLeft;
    gDim_model->vertices[0].p.v[0] = gDim_model->vertices[1].p.v[0];
    gDim_model->vertices[3].p.v[0] = (float)pRight;
    gDim_model->vertices[2].p.v[0] = gDim_model->vertices[3].p.v[0];
    gDim_model->vertices[3].p.v[1] = -(float)pTop;
    gDim_model->vertices[0].p.v[1] = gDim_model->vertices[3].p.v[1];
    gDim_model->vertices[2].p.v[1] = -(float)pBottom;
    gDim_model->vertices[1].p.v[1] = gDim_model->vertices[2].p.v[1];
    BrModelUpdate(gDim_model, BR_MODU_VERTEX_POSITIONS);
    gDim_actor->render_style = BR_RSTYLE_FACES;
    if (gDim_actor->prev == NULL) {
        BrActorAdd(g2d_camera, gDim_actor);
    }
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    if (gDim_actor->parent != NULL) {
        BrActorRemove(gDim_actor);
    }
    gDim_actor->render_style = BR_RSTYLE_NONE;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0047cbd0
void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    tU8* ptr;
    tU8* depth_table_ptr;
    tU8* right_ptr;
    int y;
    int line_skip;
    int width;

    if (gDevious_2d) {
        DeviouslyDimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
        return;
    }
    ptr = (tU8*)pPixelmap->pixels + pLeft + pPixelmap->row_bytes * pTop;
    line_skip = pPixelmap->row_bytes - pRight + pLeft;
    depth_table_ptr = (tU8*)gDepth_shade_table->pixels + gDepth_shade_table->row_bytes * gDim_amount;
    width = pRight - pLeft;
    if (pKnock_out_corners) {
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
        ptr += line_skip + 1;
        for (y = pTop + 1; y < pBottom - 1; y++, ptr += line_skip) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
        }
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
    } else {
        for (y = pTop; y < pBottom; y++) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
            ptr += line_skip;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047c740
void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDestn, int pX1, float pAngle, int pY1, int pX2, int pY2, int pColour) {

    if (pColour >= 0) {
        DRDrawLine(pDestn, pX1, pY1, pX2, pY2, pColour);
    } else {
        if (pAngle >= 0.785 && pAngle <= 5.498 && (pAngle <= 2.356 || pAngle >= 3.926)) {
            if ((pAngle <= 0.785 || pAngle >= 1.57) && (pAngle <= 3.926 || pAngle >= 4.712)) {
                DRDrawLine(pDestn, pX1 - 1, pY1, pX2 - 1, pY2, -pColour - 1);
                DRDrawLine(pDestn, pX1 + 1, pY1, pX2 + 1, pY2, 1 - pColour);
            } else {
                DRDrawLine(pDestn, pX1 - 1, pY1, pX2 - 1, pY2, 1 - pColour);
                DRDrawLine(pDestn, pX1 + 1, pY1, pX2 + 1, pY2, -pColour - 1);
            }
        } else {
            DRDrawLine(pDestn, pX1, pY1 + 1, pX2, pY2 + 1, -pColour - 1);
            DRDrawLine(pDestn, pX1, pY1 - 1, pX2, pY2 - 1, 1 - pColour);
        }
        DRDrawLine(pDestn, pX1, pY1, pX2, pY2, -pColour);
    }
}

// FUNCTION: CARMA2_HW 0x0047cce0
void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    if (pLeft < 0) {
        pLeft = 0;
    }
    if (pTop < 0) {
        pTop = 0;
    }
    if (pRight > pPixelmap->width) {
        pRight = pPixelmap->width;
    }
    if (pBottom > pPixelmap->height) {
        pBottom = pPixelmap->height;
    }
    if (pLeft < pRight && pTop < pBottom) {
        DimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
    }
}

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// FUNCTION: CARMA2_HW 0x0047d260
void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette) {
    (void)pPalette;
    DRDrawLine(pMap, pX1, pY1, pX2, pY2, (int)pColour);
}

// DR8BitFancyDrawLine

// STUB: CARMA2_HW 0x0047d2b0
void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {
    NOT_IMPLEMENTED();
}

// DRPixelmapCopyMapBlack8Bit (no own entry in the original: inlined into DRPixelmapCopyMapBlack)
static inline void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack8Bit(br_pixelmap* pDest, br_pixelmap* pSrc) {
    int y;
    int dwords_remaining;
    int src_width_dwords;
    tU8* dest_pixels;
    tU8* src_pixels;
    int src_skip;
    int dest_skip;
    tU8 p0;
    tU8 p1;
    tU8 p2;
    tU8 p3;

    dest_pixels = pDest->pixels;
    src_pixels = pSrc->pixels;
    src_skip = pSrc->row_bytes - pSrc->width;
    dest_skip = pDest->row_bytes - pDest->width;
    src_width_dwords = pSrc->width / sizeof(tU32);
    for (y = 0; y < pSrc->height; y++) {
        for (dwords_remaining = src_width_dwords; dwords_remaining > 0; dwords_remaining--) {
            p0 = *src_pixels++;
            p1 = *src_pixels++;
            p2 = *src_pixels++;
            p3 = *src_pixels++;
            if (p0 != 0) {
                *dest_pixels = p0;
            } else {
                *dest_pixels = 0xf0;
            }
            dest_pixels++;
            if (p1 != 0) {
                *dest_pixels = p1;
            } else {
                *dest_pixels = 0xf0;
            }
            dest_pixels++;
            if (p2 != 0) {
                *dest_pixels = p2;
            } else {
                *dest_pixels = 0xf0;
            }
            dest_pixels++;
            if (p3 != 0) {
                *dest_pixels = p3;
            } else {
                *dest_pixels = 0xf0;
            }
            dest_pixels++;
        }
        src_pixels += src_skip;
        dest_pixels += dest_skip;
    }
}

// FUNCTION: CARMA2_HW 0x0047d5b0
void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack(br_pixelmap* pDest, br_pixelmap* pSrc) {

    if (pDest->type == BR_PMT_INDEX_8) {
        DRPixelmapCopyMapBlack8Bit(pDest, pSrc);
    } else {
        BrPixelmapCopy(pDest, pSrc);
    }
}

// FUNCTION: CARMA2_HW 0x0047d6a0
void C2_HOOK_FASTCALL DisableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightDisable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047d6d0
void C2_HOOK_FASTCALL EnableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightEnable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x004b5470
void C2_HOOK_FASTCALL FadePaletteUp(void) {
    int j;
    int start_time;
    int the_time;
    unsigned int scale;
    br_pixelmap* pal;

    if (gFaded_palette) {
        gFaded_palette = 0;
        start_time = PDGetTotalTime();
        the_time = PDGetTotalTime() - start_time;
        while (the_time < 500) {
            scale = (the_time * 256) / 500;
            memcpy(gScratch_pixels, gCurrent_palette->pixels, 4 * 256);
            for (j = 0; j < 256; j++) {
                gScratch_pixels[j * 4] = (gScratch_pixels[j * 4] * scale) >> 8;
                gScratch_pixels[j * 4 + 1] = (gScratch_pixels[j * 4 + 1] * scale) >> 8;
                gScratch_pixels[j * 4 + 2] = (gScratch_pixels[j * 4 + 2] * scale) >> 8;
                gScratch_pixels[j * 4 + 3] = (gScratch_pixels[j * 4 + 3] * scale) >> 8;
            }
            pal = gScratch_palette;
            *((br_int_32*)pal->pixels) = 0;
            gPalette_changed = 0;
            if (!gFaded_palette) {
                PDSetPalette(pal);
            }
            gPalette_munged |= (pal != gRender_palette);
            the_time = PDGetTotalTime() - start_time;
        }
        pal = gCurrent_palette;
        *((br_int_32*)pal->pixels) = 0;
        memcpy(gCurrent_palette_pixels, pal->pixels, 4 * 256);
        gPalette_changed = 0;
        if (!gFaded_palette) {
            PDSetPalette(pal);
        }
        gPalette_munged |= (pal != gRender_palette);
    }
}

extern int gMouse_in_use;
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

// GLOBAL: CARMA2_HW 0x0067c390
int gMouse_started;

// GLOBAL: CARMA2_HW 0x0067be94
int gTransient_bitmap_index;

// GLOBAL: CARMA2_HW 0x0067be90
int gCurrent_cursor_index;

// GLOBAL: CARMA2_HW 0x0079ecb0
tMouse_coord gMouse_last_pos;

// FUNCTION: CARMA2_HW 0x0043e6c0
void C2_HOOK_FASTCALL StartMouseCursor(void) {
    if (gCursor_tinted_top == -1) {
        AllocateCursorTransient();
    }
    gNext_transient = 0;
    gTransient_bitmap_index = 0;
    GetMousePosition(&gMouse_last_pos.x, &gMouse_last_pos.y);
    gMouse_in_use = 0;
    gCurrent_cursor_index = 2;
    gMouse_started = 1;
}
