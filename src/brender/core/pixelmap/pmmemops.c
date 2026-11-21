#include "pmmemops.h"

#include "brender/br_defs.h"

#include "c2_stdlib.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0066fd0c
br_uint_8 g_startMasks[9] = {
    0xff, 0x7f, 0x3f, 0x1f,
    0x0f, 0x07, 0x03, 0x01,
    0x00,
};

// GLOBAL: CARMA2_HW 0x0066fd15
br_uint_8 g_endMasks[9] = {
    0x00,
    0x80, 0xc0, 0xe0, 0xf0,
    0xf8, 0xfc, 0xfe, 0xff,
};

// GLOBAL: CARMA2_HW 0x0066fd1e
br_uint_16 g_fpuControlWord = 0x1f3f;


#define WRITE_COLOUR_U8(DEST, COLOUR)               \
    do {                                            \
        *(br_uint_8*)(DEST) = (br_uint_8)(COLOUR);  \
    } while (0)
#define WRITE_COLOUR_U16(DEST, COLOUR)                  \
    do {                                                \
        *(br_uint_16*)(DEST) = (br_uint_16)(COLOUR);    \
    } while (0)
#define WRITE_COLOUR_U24(DEST, COLOUR)  \
    do {                                \
        ((br_uint_8*)(DEST))[0] = b0;   \
        ((br_uint_8*)(DEST))[1] = b1;   \
        ((br_uint_8*)(DEST))[2] = b2;   \
    } while (0)
#define WRITE_COLOUR_U32(DEST, COLOUR)                  \
    do {                                                \
        *(br_uint_32*)(DEST) = (br_uint_32)(COLOUR);    \
    } while (0)

#define COLOR_BITS_BYTE(SIZE, WRITE, BITS, DEST, SRC, COLOUR)   \
    do {                                                        \
        if ((BITS) & 0x80) {                                    \
            WRITE(((br_uint_8*)(DEST) + 0 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x40) {                                    \
            WRITE(((br_uint_8*)(DEST) + 1 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x20) {                                    \
            WRITE(((br_uint_8*)(DEST) + 2 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x10) {                                    \
            WRITE(((br_uint_8*)(DEST) + 3 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x08) {                                    \
            WRITE(((br_uint_8*)(DEST) + 4 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x04) {                                    \
            WRITE(((br_uint_8*)(DEST) + 5 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x02) {                                    \
            WRITE(((br_uint_8*)(DEST) + 6 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x01) {                                    \
            WRITE(((br_uint_8*)(DEST) + 7 * (SIZE)), COLOUR);   \
        }                                                       \
    } while (0)

/* Rows that are more than one source byte wide: row_bytes + 1 source bytes per row, the first masked with
 * start_mask and the last with end_mask. Row start pointers are kept so that the strides apply to them. */
#define COLOR_BITS_BLOCK(SIZE, WRITE, SRC, DEST, COLOUR)                                                    \
    do {                                                                                                    \
        br_uint_8* src_ptr = (SRC);                                                                         \
        br_uint_8* dest_ptr = (DEST);                                                                       \
        for (; height != 0; height--, src_ptr += s_stride, dest_ptr += dest_stride) {                       \
            br_uint_8* row_src = src_ptr;                                                                   \
            br_uint_8* row_dest = dest_ptr;                                                                 \
            br_uint_32 remaining_row = row_bytes;                                                           \
            br_uint_8 bits = *row_src & start_mask;                                                         \
                                                                                                            \
            for (;;) {                                                                                      \
                COLOR_BITS_BYTE((SIZE), WRITE, bits, row_dest, row_src, (COLOUR));                          \
                row_dest += 8 * (SIZE);                                                                     \
                row_src++;                                                                                  \
                if (remaining_row == 0) {                                                                   \
                    break;                                                                                  \
                }                                                                                           \
                bits = *row_src;                                                                            \
                remaining_row--;                                                                            \
                if (remaining_row == 0) {                                                                   \
                    bits &= end_mask;                                                                       \
                }                                                                                           \
            }                                                                                               \
        }                                                                                                   \
    } while (0)

// PARTIAL 5.74%: pm_mem_copy_bits 0x0053df30 - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053df30
void C2_HOOK_CDECL pm_mem_copy_bits(void* dest, br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour) {
    br_uint_8 start_mask;
    br_uint_8 end_mask;
    br_uint_32 row_bytes;

    start_mask = g_startMasks[start_bit];
    end_mask = g_endMasks[end_bit & 0x7];
    row_bytes = end_bit >> 3;

    if (row_bytes == 0) {
        switch (bpp) {
        case 1:
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(1, WRITE_COLOUR_U8, bits, dest, src, colour);
            }
            break;
        case 2:
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(2, WRITE_COLOUR_U16, bits, dest, src, colour);
            }
            break;
        case 3: {
            br_uint_8 b0 = (br_uint_8)((colour >>  0) & 0xff);
            br_uint_8 b1 = (br_uint_8)((colour >>  8) & 0xff);
            br_uint_8 b2 = (br_uint_8)((colour >> 16) & 0xff);
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(3, WRITE_COLOUR_U24, bits, dest, src, colour);
            }
            break;
        }
        case 4: {
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(4, WRITE_COLOUR_U32, bits, dest, src, colour);
            }
            break;
        }
        default:
            /* the original returns without writing for any bpp other than 1-4 */
            break;
        }
    } else {
        switch (bpp) {
        case 1:
            COLOR_BITS_BLOCK(1, WRITE_COLOUR_U8, src, dest, colour);
            break;
        case 2:
            COLOR_BITS_BLOCK(2, WRITE_COLOUR_U16, src, dest, colour);
            break;
        case 3: {
            br_uint_8 b0 = (br_uint_8)((colour >>  0) & 0xff);
            br_uint_8 b1 = (br_uint_8)((colour >>  8) & 0xff);
            br_uint_8 b2 = (br_uint_8)((colour >> 16) & 0xff);
            COLOR_BITS_BLOCK(3, WRITE_COLOUR_U24, src, dest, colour);
            break;
        }
        case 4:
            COLOR_BITS_BLOCK(4, WRITE_COLOUR_U32, src, dest, colour);
            break;
        default:
            /* the original returns without writing for any bpp other than 1-4 */
            break;
        }
    }
}

// PARTIAL 20.00%: FUN_0053e31a 0x0053e31a - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053e31a
void C2_HOOK_CDECL FUN_0053e31a(br_uint_8* dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour) {
    br_uint_8 c8;
    br_uint_16 c16;
    br_uint_8 b0, b1, b2;

    switch (bpp) {
    case 1:
        c8 = (br_uint_8)colour;
        for (; nbpixels != 0; nbpixels--, dest++) {
            *dest = c8;
        }
        break;
    case 2:
        c16 = (br_uint_16)colour;
        for (; nbpixels != 0; nbpixels--, dest += 2) {
            *(br_uint_16*)dest = c16;
        }
        break;
    case 3:
        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);
        for (; nbpixels != 0; nbpixels--, dest += 3) {
            dest[0] = b0;
            dest[1] = b1;
            dest[2] = b2;
        }
        break;
    case 4:
        for (; nbpixels != 0; nbpixels--, dest += 4) {
            *(br_uint_32*)dest = colour;
        }
        break;
    default:
        /* the original returns without writing for any bpp other than 1-4 */
        break;
    }
}

// PARTIAL 12.15%: FUN_0053e3fe 0x0053e3fe - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053e3fe
void C2_HOOK_CDECL FUN_0053e3fe(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour) {
    br_uint_32 x;
    br_uint_8* row_dest;
    br_uint_8 c8;
    br_uint_16 c16;
    br_uint_8 b0, b1, b2;

    for (; height != 0; height--, dest += stride) {
        row_dest = dest;
        switch (bpp) {
        case 1:
            c8 = (br_uint_8)colour;
            for (x = 0; x < width; x++) {
                row_dest[x] = c8;
            }
            break;
        case 2:
            c16 = (br_uint_16)colour;
            for (x = 0; x < width; x++) {
                ((br_uint_16*)row_dest)[x] = c16;
            }
            break;
        case 3:
            b0 = (br_uint_8)((colour >>  0) & 0xff);
            b1 = (br_uint_8)((colour >>  8) & 0xff);
            b2 = (br_uint_8)((colour >> 16) & 0xff);
            for (x = 0; x < width; x++) {
                row_dest[3 * x + 0] = b0;
                row_dest[3 * x + 1] = b1;
                row_dest[3 * x + 2] = b2;
            }
            break;
        case 4:
            for (x = 0; x < width; x++) {
                ((br_uint_32*)row_dest)[x] = colour;
            }
            break;
        default:
            /* the original returns without writing for any bpp other than 1-4 */
            break;
        }
    }
}

// PARTIAL 18.67%: FUN_0053e527 0x0053e527 - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053e527
void C2_HOOK_CDECL FUN_0053e527(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dest_stride, br_uint_32 src_stride, br_uint_32 bpp) {
    for (; height != 0; height--, dest += dest_stride, src += src_stride) {
        memcpy(dest, src, width * bpp);
    }
}

// PARTIAL 19.35%: FUN_0053e584 0x0053e584 - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053e584
void C2_HOOK_CDECL FUN_0053e584(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height) {
    br_uint_32 total = width * height;
    memcpy(dest, src, total);
}

// PARTIAL 9.09%: pm_mem_set_colour 0x0053e5c9 - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053e5c9
void C2_HOOK_CDECL pm_mem_set_colour(br_uint_8* dest, br_uint_32 dest_qual, br_uint_32 bpp, br_uint_32 colour) {
    switch (bpp) {
    case 1:
        *(br_uint_8*)dest = colour;
        break;
    case 2:
        *(br_uint_16*)dest = colour;
        break;
    case 3:
        ((br_uint_8*)dest)[0] = (br_uint_8)((colour >>  0) & 0xff);
        ((br_uint_8*)dest)[1] = (br_uint_8)((colour >>  8) & 0xff);
        ((br_uint_8*)dest)[2] = (br_uint_8)((colour >> 16) & 0xff);
        break;
    case 4:
        *(br_uint_32*)dest = colour;
        break;
    default:
        /* the original returns without writing for any bpp other than 1-4 */
        break;
    }
}

// PARTIAL 13.33%: pm_mem_read_colour 0x0053e626 - plain C body restored 2026-11-16; the 100% body was a
// hand-written __asm block (no longer allowed: it cannot be ported to other compilers/systems).
// FUNCTION: CARMA2_HW 0x0053e626
br_uint_32 C2_HOOK_CDECL pm_mem_read_colour(void* src, br_uint_32 src_qual, br_uint_32 bpp) {

    switch (bpp) {
    case 1:
        return (br_uint_32)*(br_uint_8*)src;
    case 2:
        return (br_uint_32)*(br_uint_16*)src;
    case 3: {
        return BR_COLOUR_RGB(((br_uint_8*)src)[2], ((br_uint_8*)src)[1], ((br_uint_8*)src)[0]);
    }
    case 4:
        return (br_uint_32)*(br_uint_32*)src;
    default:
        /* the original returns 0 for any bpp other than 1-4 */
        return 0;
    }
}

// PARTIAL 8.75%: pm_mem_fill_colour 0x0053e686 - plain C body restored 2026-11-16; the 87.57% body was a
// hand-written __asm block (no longer allowed: it cannot be ported to other compilers/systems).
// FUNCTION: CARMA2_HW 0x0053e686
void C2_HOOK_CDECL pm_mem_fill_colour(br_uint_8 *dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour) {

    switch (bpp) {
    case 1:
        memset(dest, colour, nbpixels);
        break;
    case 2: {
        br_uint_16 p = colour;
        int i;
        for (i = nbpixels; i != 0; i--, dest += 2) {
            *(br_uint_16*)dest = p;
        }
        break;
    }
    break;
    case 3: {
        int i;
        br_uint_8 b0, b1, b2;

        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);
        for (i = nbpixels; i != 0; i--, dest += 3) {
            dest[0] = b0;
            dest[1] = b1;
            dest[2] = b2;
        }
        break;
    }
    case 4: {
        int i;
        for (i = nbpixels; i != 0; i--, dest += 4) {
            *(br_uint_32*)dest = colour;
        }
        break;
    }
    default:
        /* the original returns without writing for any bpp other than 1-4 */
        return;
    }
}

// PARTIAL 11.59%: pm_mem_fill_colour_rect 0x0053e9f3 - plain C body restored 2026-11-16. The 57.89% inline-asm
// attempt stored its locals over its own pushed registers and left the x87 stack unbalanced, which crashed the
// frontend as the main menu appeared (bisected: 31c682f84).
// FUNCTION: CARMA2_HW 0x0053e9f3
void C2_HOOK_CDECL pm_mem_fill_colour_rect(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour) {
    br_uint_32 x;
    br_uint_32 y;

    switch (bpp) {
    case 1:
        for (y = 0; y < height; y++, dest += stride) {
            memset(dest, colour, width);
        }
        break;
    case 2:
        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_16*)dest)[x] = colour;
            }
        }
        break;
    case 3: {
        br_uint_8 b0, b1, b2;

        /* the original stores the colour low byte first, like pm_mem_fill_colour */
        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);

        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_8*)dest)[3 * x + 0] = b0;
                ((br_uint_8*)dest)[3 * x + 1] = b1;
                ((br_uint_8*)dest)[3 * x + 2] = b2;
            }
        }
        break;
    }
    case 4:
        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_32*)dest)[x] = colour;
            }
        }
        break;
    default:
        /* the original returns without writing for any bpp other than 1-4 */
        return;
    }
}


// PARTIAL 12.90%: pm_mem_copy_colour 0x0053ee16 - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053ee16
void C2_HOOK_CDECL pm_mem_copy_colour(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp) {
    memcpy(dst, src, nbpixels * bpp);
}


// PARTIAL 9.52%: pm_mem_copy_colour_rowbyrow 0x0053ef0d - plain C restored 2026-11-16 (inline asm removed: not portable).
// FUNCTION: CARMA2_HW 0x0053ef0d
void C2_HOOK_CDECL pm_mem_copy_colour_rowbyrow(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dest_stride, br_uint_32 src_stride, br_uint_32 bpp) {
    for (; height != 0; height--, dest += dest_stride, src += src_stride) {
        memcpy(dest, src, width * bpp);
    }
}