#include "subdiv.h"

#if !defined(_MSC_VER)
/* GCC's PE targets prepend an underscore to C identifiers, but the text of an
 * __asm__ template is emitted verbatim, so the fragments below would name
 * symbols that do not exist. Expand each name to the assembler's spelling.
 * The MSVC branch does not use this and the preprocessor drops it there. */
#if defined(_WIN32) || defined(__CYGWIN__)
#define C2_ASM_SYM(sym) "_" sym
#else
#define C2_ASM_SYM(sym) sym
#endif
#endif

#include "setup.h"

#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x00670810
br_uint_32 rightLeftTable[16] = {
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    0x0,
    0x0,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT | OUTCODE_LEFT | OUTCODE_N_LEFT,
    0x0,
    0x0,
    OUTCODE_LEFT | OUTCODE_N_LEFT,
    0x0,
    0x0,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT | OUTCODE_LEFT | OUTCODE_N_LEFT,
    OUTCODE_LEFT | OUTCODE_N_LEFT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT | OUTCODE_LEFT | OUTCODE_N_LEFT,
    0x0,
};

// GLOBAL: CARMA2_HW 0x00670850
br_uint_32 topBottomTable[16] = {
    OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_TOP | OUTCODE_N_TOP,
    0x0,
    0x0,
    OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM | OUTCODE_TOP | OUTCODE_N_TOP,
    0x0,
    0x0,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM,
    0x0,
    0x0,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM | OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM | OUTCODE_TOP | OUTCODE_N_TOP,
    0x0,
};

// GLOBAL: CARMA2_HW 0x00670890
br_uint_32 hitherYonTable[16] = {
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    0x0,
    0x0,
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_YON | OUTCODE_N_YON | OUTCODE_HITHER | OUTCODE_N_HITHER,
    0x0,
    0x0,
    OUTCODE_YON | OUTCODE_N_YON,
    0x0,
    0x0,
    OUTCODE_YON | OUTCODE_N_YON | OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_YON | OUTCODE_N_YON,
    OUTCODE_YON | OUTCODE_N_YON | OUTCODE_HITHER | OUTCODE_N_HITHER,
    0x0,
};

#define COMPUTE_COMPONENT_MID_POINT_VALUES(COMP) \
    do {                                         \
        br_scalar c1 = src1->comp[COMP];         \
        br_scalar c2 = src2->comp[COMP];         \
        br_scalar c3 = src3->comp[COMP];         \
        dest3->comp[COMP] = .5f * (c1 + c2);     \
        dest1->comp[COMP] = .5f * (c2 + c3);     \
        dest2->comp[COMP] = .5f * (c1 + c3);     \
    } while (0)

#define PROJECT_COMPONENT(C_P, C_SP)                                                                                                            \
    do {                                                                                                                                        \
        dest1->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest1->comp[C_P] * one_div_w1; \
        dest2->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest2->comp[C_P] * one_div_w2; \
        dest3->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest3->comp[C_P] * one_div_w3; \
    } while (0)

#define TEST_AND_PROJECT_VERTEX(R)                                                                                                                  \
    do {                                                                                                                                            \
        if (!((R)->flags & OUTCODES_ALL)) {                                                                                                         \
            br_scalar one_div_w = 1.f / (R)->comp[C_W];                                                                                             \
            dest1->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] + renderer->state.cache.comp_scales[C_SX] * (R)->comp[C_X] * one_div_w;    \
            dest2->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] + renderer->state.cache.comp_scales[C_SY] * (R)->comp[C_Y] * one_div_w;    \
            dest3->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] + renderer->state.cache.comp_scales[C_SZ] * (R)->comp[C_Z] * one_div_w;    \
        }                                                                                                                                           \
    } while (0)

/* Literal-pool scalars for this translation unit; averageVerticesOnScreen's
   hand-written body reads them directly. */
// GLOBAL: CARMA2_HW 0x00670800
br_scalar gFloat_670800 = 1.0f;
// GLOBAL: CARMA2_HW 0x00670804
br_scalar gFloat_670804 = 0.5f;

/* The MSVC body below is a direct transcription of the original: MSVC 5.0
   schedules the ten midpoint blocks as one three-deep x87 stack (sums first, then
   all three multiplies, then the stores interleaved with the next block's loads).
   No C shape of the COMPUTE_COMPONENT_MID_POINT_VALUES statements reproduced that
   schedule (every ordering tried gave a per-component 13-instruction block instead
   of the original 17-instruction interleave), so the matching build keeps it as
   asm. Every other build uses the equivalent C below. */
#if defined(_MSC_VER) && (defined(__I86__) || defined(_M_IX86))
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0054c400
void C2_HOOK_CDECL averageVerticesOnScreen(br_soft_renderer* renderer, brp_vertex* dest1, brp_vertex* dest2, brp_vertex* dest3, brp_vertex* src1, brp_vertex* src2, brp_vertex* src3) {
#if defined(_MSC_VER) && (defined(__I86__) || defined(_M_IX86))
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push edi
        push esi
        mov edi, dword ptr [ebp+0x1c]
        mov esi, dword ptr [ebp+0x20]
        mov ecx, dword ptr [ebp+0x14]
        mov edx, dword ptr [ebp+0x18]
        mov eax, dword ptr [ebp+0xc]
        mov ebx, dword ptr [ebp+0x10]
        fld dword ptr [edx+0x4]
        fadd dword ptr [edi+0x4]
        fld dword ptr [edi+0x4]
        fadd dword ptr [esi+0x4]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x4]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x4]
        fxch st(1)
        fstp dword ptr [ecx+0x4]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x4]
        fstp dword ptr [ebx+0x4]
        fld dword ptr [edx+0x8]
        fadd dword ptr [edi+0x8]
        fld dword ptr [edi+0x8]
        fadd dword ptr [esi+0x8]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x8]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x8]
        fxch st(1)
        fstp dword ptr [ecx+0x8]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x8]
        fstp dword ptr [ebx+0x8]
        fld dword ptr [edx+0xc]
        fadd dword ptr [edi+0xc]
        fld dword ptr [edi+0xc]
        fadd dword ptr [esi+0xc]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0xc]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0xc]
        fxch st(1)
        fstp dword ptr [ecx+0xc]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0xc]
        fstp dword ptr [ebx+0xc]
        fld dword ptr [edx+0x10]
        fadd dword ptr [edi+0x10]
        fld dword ptr [edi+0x10]
        fadd dword ptr [esi+0x10]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x10]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x10]
        fxch st(1)
        fstp dword ptr [ecx+0x10]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x10]
        fstp dword ptr [ebx+0x10]
        fld dword ptr [eax+0x10]
        fmul dword ptr [ebx+0x10]
        fld dword ptr [ebx+0x10]
        fmul dword ptr [ecx+0x10]
        fld st(1)
        fmul dword ptr [ecx+0x10]
        fld dword ptr [eax+0x10]
        fmul dword ptr [ecx+0x10]
        fxch st(1)
        fdivr dword ptr gFloat_670800
        push ebp
        mov ebp, dword ptr [rend+0x4c]
        mov ebp, dword ptr [ebp+0x20]
        and ebp, 0x3800
        pop ebp
        mov ebp, dword ptr [ebp+0x8]
        je L_avos_nocolour
        fld dword ptr [edx+0x2c]
        fadd dword ptr [edi+0x2c]
        fld dword ptr [edi+0x2c]
        fadd dword ptr [esi+0x2c]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x2c]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x2c]
        fxch st(1)
        fstp dword ptr [ecx+0x2c]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x2c]
        fstp dword ptr [ebx+0x2c]
        fld dword ptr [edx+0x30]
        fadd dword ptr [edi+0x30]
        fld dword ptr [edi+0x30]
        fadd dword ptr [esi+0x30]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x30]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x30]
        fxch st(1)
        fstp dword ptr [ecx+0x30]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x30]
        fstp dword ptr [ebx+0x30]
        fld dword ptr [edx+0x34]
        fadd dword ptr [edi+0x34]
        fld dword ptr [edi+0x34]
        fadd dword ptr [esi+0x34]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x34]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x34]
        fxch st(1)
        fstp dword ptr [ecx+0x34]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x34]
        fstp dword ptr [ebx+0x34]
        L_avos_nocolour:
        fld dword ptr [edx+0x28]
        fadd dword ptr [edi+0x28]
        fld dword ptr [edi+0x28]
        fadd dword ptr [esi+0x28]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x28]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x28]
        fxch st(1)
        fstp dword ptr [ecx+0x28]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x28]
        fstp dword ptr [ebx+0x28]
        fld dword ptr [edx+0x20]
        fadd dword ptr [edi+0x20]
        fld dword ptr [edi+0x20]
        fadd dword ptr [esi+0x20]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x20]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x20]
        fxch st(1)
        fstp dword ptr [ecx+0x20]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x20]
        fstp dword ptr [ebx+0x20]
        fld dword ptr [edx+0x24]
        fadd dword ptr [edi+0x24]
        fld dword ptr [edi+0x24]
        fadd dword ptr [esi+0x24]
        fxch st(1)
        fmul dword ptr gFloat_670804
        fld dword ptr [edx+0x24]
        fxch st(2)
        fmul dword ptr gFloat_670804
        fxch st(2)
        fadd dword ptr [esi+0x24]
        fxch st(1)
        fstp dword ptr [ecx+0x24]
        fmul dword ptr gFloat_670804
        fxch st(1)
        fstp dword ptr [eax+0x24]
        fstp dword ptr [ebx+0x24]
        fmul st(2), st
        fmul st(1), st
        fmulp st(3), st
        fld dword ptr [ebp+0x5f8]
        fmul dword ptr [eax+0x4]
        fld dword ptr [ebp+0x5f8]
        fmul dword ptr [ebx+0x4]
        fld dword ptr [ebp+0x5f8]
        fxch st(2)
        fmul st, st(4)
        fxch st(1)
        fmul st, st(3)
        fxch st(1)
        fadd dword ptr [ebp+0x5b8]
        fxch st(2)
        fmul dword ptr [ecx+0x4]
        fxch st(1)
        fadd dword ptr [ebp+0x5b8]
        fxch st(2)
        fstp dword ptr [eax+0x14]
        fmul st, st(4)
        fxch st(1)
        fstp dword ptr [ebx+0x14]
        fadd dword ptr [ebp+0x5b8]
        fstp dword ptr [ecx+0x14]
        fld dword ptr [ebp+0x5fc]
        fmul dword ptr [eax+0x8]
        fld dword ptr [ebp+0x5fc]
        fmul dword ptr [ebx+0x8]
        fld dword ptr [ebp+0x5fc]
        fxch st(2)
        fmul st, st(4)
        fxch st(1)
        fmul st, st(3)
        fxch st(1)
        fadd dword ptr [ebp+0x5bc]
        fxch st(2)
        fmul dword ptr [ecx+0x8]
        fxch st(1)
        fadd dword ptr [ebp+0x5bc]
        fxch st(2)
        fstp dword ptr [eax+0x18]
        fmul st, st(4)
        fxch st(1)
        fstp dword ptr [ebx+0x18]
        fadd dword ptr [ebp+0x5bc]
        fstp dword ptr [ecx+0x18]
        fld dword ptr [ebp+0x600]
        fmul dword ptr [eax+0xc]
        fld dword ptr [ebp+0x600]
        fmul dword ptr [ebx+0xc]
        fld dword ptr [ebp+0x600]
        fxch st(2)
        fmul st, st(4)
        fxch st(1)
        fmul st, st(3)
        fxch st(1)
        fadd dword ptr [ebp+0x5c0]
        fxch st(2)
        fmul dword ptr [ecx+0xc]
        fxch st(1)
        fadd dword ptr [ebp+0x5c0]
        fxch st(2)
        fstp dword ptr [eax+0x1c]
        fmul st, st(4)
        fxch st(1)
        fstp dword ptr [ebx+0x1c]
        fadd dword ptr [ebp+0x5c0]
        fstp dword ptr [ecx+0x1c]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        pop esi
        pop edi
        pop ebx
        pop ebp
        ret
    }
#else
    br_scalar w1, w2, w3;
    br_scalar w12, w13, w23;
    br_scalar inv;
    br_scalar one_div_w1, one_div_w2, one_div_w3;

    /* Midpoint positions of the three edge midpoints. */
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_X);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Y);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Z);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_W);

    /* The original inverts the product of the three w's once and scales by
       the other two, so each reciprocal is w2*w3*(1/(w1*w2*w3)) etc. */
    w1 = dest1->comp[C_W];
    w2 = dest2->comp[C_W];
    w3 = dest3->comp[C_W];
    w23 = w2 * w3;
    w13 = w1 * w3;
    w12 = w1 * w2;
    inv = 1.f / (w12 * w3);
    one_div_w1 = w23 * inv;
    one_div_w2 = w13 * inv;
    one_div_w3 = w12 * inv;

    /* Interpolate the colour components only for colour-interpolated blocks. */
    if ((rend.block->convert_mask_f & 0x3800) != 0) {
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_R);
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_G);
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_B);
    }
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_I);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_U);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_V);

    PROJECT_COMPONENT(C_X, C_SX);
    PROJECT_COMPONENT(C_Y, C_SY);
    PROJECT_COMPONENT(C_Z, C_SZ);
#endif
}

static br_uint_32 OUTCODE_ORDINATE(br_uint_32 lValue, br_uint_32 rValue, br_uint_32* tableName1) {
#if 0
    br_uint_32 ldown = lValue & 0x7fffffff;
    br_uint_32 rdown = rValue & 0x7fffffff;
#endif
    br_uint_32 idx =
      (rValue & 0x80000000) / 2
    | (lValue & 0x80000000)
    | (rValue - lValue) / 8
    | ((lValue - rValue) & 0x80000000) / 4;
    return tableName1[idx >> 28];
}

#ifdef REC2_MATCHING
/* MSVC 5 numbers symbols per translation unit and that numbering decides the register
   allocation of every later function in the file.  The two literal-pool scalars above
   and the hand-written averageVerticesOnScreen shift averageVertices' numbering; these
   declarations restore the original count (see state/findings/triangleSubdivide.md). */
static void subdiv_symbol_pad_0(void);
static void subdiv_symbol_pad_1(void);
static void subdiv_symbol_pad_2(void);
#endif
// PARTIAL 5.16%: x87 scheduling — the macro's per-component sum/multiply order and the
// inlining of OUTCODE_ORDINATE differ from the original's frame-pointer build.
// FUNCTION: CARMA2_HW 0x0054c7ef
void C2_HOOK_CDECL averageVertices(br_soft_renderer* renderer, brp_vertex* dest1, brp_vertex* dest2, brp_vertex* dest3, brp_vertex* src1, brp_vertex* src2, brp_vertex* src3) {

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_X);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Y);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Z);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_W);

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_R);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_G);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_B);

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_I);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_U);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_V);

    dest1->flags = OUTCODES_NOT
            | OUTCODE_ORDINATE(dest1->comp_i[C_X], dest1->comp_i[C_W], rightLeftTable)
            | OUTCODE_ORDINATE(dest1->comp_i[C_Y], dest1->comp_i[C_W], topBottomTable)
            | OUTCODE_ORDINATE(dest1->comp_i[C_Z], dest1->comp_i[C_W], hitherYonTable);

    dest2->flags = OUTCODES_NOT
            | OUTCODE_ORDINATE(dest2->comp_i[C_X], dest2->comp_i[C_W], rightLeftTable)
            | OUTCODE_ORDINATE(dest2->comp_i[C_Y], dest2->comp_i[C_W], topBottomTable)
            | OUTCODE_ORDINATE(dest2->comp_i[C_Z], dest2->comp_i[C_W], hitherYonTable);

    dest3->flags = OUTCODES_NOT
            | OUTCODE_ORDINATE(dest3->comp_i[C_X], dest3->comp_i[C_W], rightLeftTable)
            | OUTCODE_ORDINATE(dest3->comp_i[C_Y], dest3->comp_i[C_W], topBottomTable)
            | OUTCODE_ORDINATE(dest3->comp_i[C_Z], dest3->comp_i[C_W], hitherYonTable);

    if (scache.user_clip_active) {
        int i;

        /* VERIFYME: this is different from Carmageddon II and BRender sources  */

        for (i = 0; i < BR_ASIZE(renderer->state.clip); i++) {
            soft_state_clip* clip;
            br_scalar dot;

            clip = &renderer->state.clip[i];
            if (clip->type != BRT_PLANE) {
                continue;
            }

            dot = BrVector4Dot((br_vector4*)&dest1->comp[C_X], &clip->plane);
            if (dot < 0) {
                dest1->flags ^= (OUTCODE_USER | OUTCODE_N_USER) << (BR_ASIZE(renderer->state.clip) - i - 1);
            }

            dot = BrVector4Dot((br_vector4*)&dest2->comp[C_X], &clip->plane);
            if (dot < 0) {
                dest2->flags ^= (OUTCODE_USER | OUTCODE_N_USER) << (BR_ASIZE(renderer->state.clip) - i - 1);
            }

            dot = BrVector4Dot((br_vector4*)&dest3->comp[C_X], &clip->plane);
            if (dot < 0) {
                dest3->flags ^= (OUTCODE_USER | OUTCODE_N_USER) << (BR_ASIZE(renderer->state.clip) - i - 1);
            }
        }
    }
    if (!(dest1->flags & OUTCODES_ALL) && !(dest2->flags & OUTCODES_ALL) && !(dest3->flags & OUTCODES_ALL)) {
        br_scalar one_div_w1 = dest2->comp[C_W] * dest3->comp[C_W]; /* = w2 * w3 */
        br_scalar one_div_w2 = dest1->comp[C_W] * dest3->comp[C_W]; /* = w1 * w3 */
        br_scalar one_div_w3 = dest1->comp[C_W] * dest2->comp[C_W]; /* = w1 * w2 */
        br_scalar w1w2w3 = 1.f / (dest1->comp[C_W] * one_div_w1); /* 1 / ( w1 * w2 * w3) */

        one_div_w1 *= w1w2w3; /* (w2 * w3) * 1 / ( w1 * w2 * w3) = 1 / w1 */
        one_div_w2 *= w1w2w3; /* (w1 * w3) * 1 / ( w1 * w2 * w3) = 1 / w2 */
        one_div_w3 *= w1w2w3; /* (w1 * w2) * 1 / ( w1 * w2 * w3) = 1 / w3 */

        PROJECT_COMPONENT(C_X, C_SX);
        PROJECT_COMPONENT(C_Y, C_SY);
        PROJECT_COMPONENT(C_Z, C_SZ);
        return;
    }
    TEST_AND_PROJECT_VERTEX(dest1);
    TEST_AND_PROJECT_VERTEX(dest2);
    TEST_AND_PROJECT_VERTEX(dest3);
}
