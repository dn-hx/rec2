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

#define COLOR_BITS_BLOCK(SIZE, WRITE, SRC, DEST, COLOUR)                                                    \
    do {                                                                                                    \
        br_uint_8* src_ptr = (SRC);                                                                         \
        void* dest_ptr = (DEST);                                                                            \
        for (; height != 0; height--, src_ptr += s_stride, dest_ptr = (br_uint_8*)dest_ptr + dest_stride) { \
            br_uint_32 remaining_row = row_bytes;                                                           \
            br_uint_8 bits = *src_ptr & start_mask;                                                         \
                                                                                                            \
            for (; remaining_row >= 0; remaining_row--) {                                                   \
                COLOR_BITS_BYTE((SIZE), WRITE, bits, dest_ptr, src_ptr, (COLOUR));                          \
                ++src_ptr;                                                                                  \
                dest_ptr = (br_uint_8*)dest_ptr + 8 * (SIZE);                                               \
                bits = *src_ptr;                                                                            \
                if (remaining_row == 0) {                                                                   \
                    bits &= end_mask;                                                                       \
                }                                                                                           \
            }                                                                                               \
        }                                                                                                   \
    } while (0)

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
            abort();
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
            abort();
            break;
        }
    }
}

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
        abort();
        return;
    }
}

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
        abort();
        return;
    }
}

// FUNCTION: CARMA2_HW 0x0053ee16
void C2_HOOK_CDECL pm_mem_copy_colour(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp) {

    memcpy(dst, src, nbpixels * bpp);
}

// FUNCTION: CARMA2_HW 0x0053ef0d
void C2_HOOK_CDECL pm_mem_copy_colour_rowbyrow(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dest_stride, br_uint_32 src_stride, br_uint_32 bpp) {

    for (; height != 0; height--, dest += dest_stride, src += src_stride) {
        memcpy(dest, src, width * bpp);
    }
}

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
        abort();
    }
}

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
        abort();
        return 0;
    }
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053e3fe
void C2_HOOK_CDECL FUN_0053e3fe(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov es, word ptr [ebp+0xc]
        mov esi, dword ptr [ebp+0x18]
        mov edi, dword ptr [ebp+8]
        mov eax, dword ptr [ebp+0x20]
        mov ecx, dword ptr [ebp+0x10]
        mov ebx, dword ptr [ebp+0x1c]
        cmp ebx, 4
        je L_bpp4
        cmp ebx, 3
        je L_bpp3
        cmp ebx, 2
        je L_bpp2
        cmp ebx, 1
        je L_bpp1
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp1:
        mov ah, al
        mov ebx, eax
        shl eax, 0x10
        mov al, bl
        mov ah, bl
    L_fill:
        sub esi, ecx
        mov ebx, ecx
    L_row1:
        mov ecx, ebx
        mov edx, edi
        and edx, 3
        je L_aligned
    L_head:
        mov byte ptr es:[edi], al
        inc edi
        dec ecx
        je L_rowend
        dec edx
        jne L_head
    L_aligned:
        mov edx, ecx
        shr ecx, 2
        rep stosd
        mov ecx, edx
        and ecx, 3
        rep stosb
    L_rowend:
        add edi, esi
        dec dword ptr [ebp+0x14]
        jne L_row1
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp2:
        mov ebx, eax
        shl eax, 0x10
        mov al, bl
        mov ah, bh
        sub esi, ecx
        sub esi, ecx
        mov ebx, ecx
    L_row2:
        mov ecx, ebx
        shr ecx, 1
        rep stosd
        adc ecx, ecx
        rep stosw
        add edi, esi
        dec dword ptr [ebp+0x14]
        jne L_row2
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp4:
        sub esi, ecx
        sub esi, ecx
        sub esi, ecx
        sub esi, ecx
        mov ebx, ecx
    L_row4:
        mov ecx, ebx
        rep stosd
        add edi, esi
        dec dword ptr [ebp+0x14]
        jne L_row4
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp3:
        cmp al, ah
        jne L_bpp3_mixed
        mov ebx, eax
        shr ebx, 8
        cmp bh, al
        jne L_bpp3_mixed
        lea ecx, [ecx+ecx*2]
        rol eax, 8
        mov al, ah
        jmp L_fill
    L_bpp3_mixed:
        rol eax, 8
        mov edx, eax
        mov al, dh
        ror eax, 0x10
        mov ebx, edx
        rol ebx, 8
        mov bl, al
        mov dl, ah
        mov bh, ah
        rol eax, 8
        push ds
        mov ds, word ptr [ebp+0xc]
    L_row3:
        push ecx
        push edi
        mov esi, ecx
        shr esi, 2
        je L_tail3
    L_quad3:
        mov dword ptr [edi], eax
        mov dword ptr [edi+4], ebx
        mov dword ptr [edi+8], edx
        add edi, 0xc
        dec esi
        jne L_quad3
    L_tail3:
        and ecx, 3
        je L_rowend3
    L_byte3:
        mov byte ptr [edi], al
        mov byte ptr [edi+1], ah
        mov byte ptr [edi+2], bh
        add edi, 3
        dec ecx
        jne L_byte3
    L_rowend3:
        pop edi
        pop ecx
        add edi, dword ptr [ebp+0x18]
        dec dword ptr [ebp+0x14]
        jne L_row3
        pop ds
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
#else
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %ebx\n"
        "push %esi\n"
        "push %edi\n"
        "push %es\n"
        "mov 0xc(%ebp), %es\n"
        "mov 0x18(%ebp), %esi\n"
        "mov 8(%ebp), %edi\n"
        "mov 0x20(%ebp), %eax\n"
        "mov 0x10(%ebp), %ecx\n"
        "mov 0x1c(%ebp), %ebx\n"
        "cmp $4, %ebx\n"
        "je 14f\n"
        "cmp $3, %ebx\n"
        "je 16f\n"
        "cmp $2, %ebx\n"
        "je 13f\n"
        "cmp $1, %ebx\n"
        "je 10f\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "10:\n"
        "mov %al, %ah\n"
        "mov %eax, %ebx\n"
        "shl $0x10, %eax\n"
        "mov %bl, %al\n"
        "mov %bl, %ah\n"
        "11:\n"
        "sub %ecx, %esi\n"
        "mov %ecx, %ebx\n"
        "1:\n"
        "mov %ebx, %ecx\n"
        "mov %edi, %edx\n"
        "and $3, %edx\n"
        "je 3f\n"
        "2:\n"
        "mov %al, %es:(%edi)\n"
        "inc %edi\n"
        "dec %ecx\n"
        "je 4f\n"
        "dec %edx\n"
        "jne 2b\n"
        "3:\n"
        "mov %ecx, %edx\n"
        "shr $2, %ecx\n"
        "rep stosl %eax, %es:(%edi)\n"
        "mov %edx, %ecx\n"
        "and $3, %ecx\n"
        "rep stosb %al, %es:(%edi)\n"
        "4:\n"
        "add %esi, %edi\n"
        "decl 0x14(%ebp)\n"
        "jne 1b\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "13:\n"
        "mov %eax, %ebx\n"
        "shl $0x10, %eax\n"
        "mov %bl, %al\n"
        "mov %bh, %ah\n"
        "sub %ecx, %esi\n"
        "sub %ecx, %esi\n"
        "mov %ecx, %ebx\n"
        "5:\n"
        "mov %ebx, %ecx\n"
        "shr $1, %ecx\n"
        "rep stosl %eax, %es:(%edi)\n"
        "adc %ecx, %ecx\n"
        "rep stosw %ax, %es:(%edi)\n"
        "add %esi, %edi\n"
        "decl 0x14(%ebp)\n"
        "jne 5b\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "14:\n"
        "sub %ecx, %esi\n"
        "sub %ecx, %esi\n"
        "sub %ecx, %esi\n"
        "sub %ecx, %esi\n"
        "mov %ecx, %ebx\n"
        "6:\n"
        "mov %ebx, %ecx\n"
        "rep stosl %eax, %es:(%edi)\n"
        "add %esi, %edi\n"
        "decl 0x14(%ebp)\n"
        "jne 6b\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "16:\n"
        "cmp %ah, %al\n"
        "jne 17f\n"
        "mov %eax, %ebx\n"
        "shr $8, %ebx\n"
        "cmp %al, %bh\n"
        "jne 17f\n"
        "lea (%ecx,%ecx,2), %ecx\n"
        "rol $8, %eax\n"
        "mov %ah, %al\n"
        "jmp 11b\n"
        "17:\n"
        "rol $8, %eax\n"
        "mov %eax, %edx\n"
        "mov %dh, %al\n"
        "ror $0x10, %eax\n"
        "mov %edx, %ebx\n"
        "rol $8, %ebx\n"
        "mov %al, %bl\n"
        "mov %ah, %dl\n"
        "mov %ah, %bh\n"
        "rol $8, %eax\n"
        "push %ds\n"
        "mov 0xc(%ebp), %ds\n"
        "7:\n"
        "push %ecx\n"
        "push %edi\n"
        "mov %ecx, %esi\n"
        "shr $2, %esi\n"
        "je 9f\n"
        "8:\n"
        "mov %eax, (%edi)\n"
        "mov %ebx, 4(%edi)\n"
        "mov %edx, 8(%edi)\n"
        "add $0xc, %edi\n"
        "dec %esi\n"
        "jne 8b\n"
        "9:\n"
        "and $3, %ecx\n"
        "je 12f\n"
        "15:\n"
        "mov %al, (%edi)\n"
        "mov %ah, 1(%edi)\n"
        "mov %bh, 2(%edi)\n"
        "add $3, %edi\n"
        "dec %ecx\n"
        "jne 15b\n"
        "12:\n"
        "pop %edi\n"
        "pop %ecx\n"
        "add 0x18(%ebp), %edi\n"
        "decl 0x14(%ebp)\n"
        "jne 7b\n"
        "pop %ds\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053e31a
void C2_HOOK_CDECL FUN_0053e31a(br_uint_8* dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov es, word ptr [ebp+0xc]
        mov edi, dword ptr [ebp+8]
        mov eax, dword ptr [ebp+0x18]
        mov ecx, dword ptr [ebp+0x10]
        mov ebx, dword ptr [ebp+0x14]
        cmp ebx, 4
        je L_bpp4
        cmp ebx, 3
        je L_bpp3
        cmp ebx, 2
        je L_bpp2
        cmp ebx, 1
        je L_bpp1
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp1:
        mov ah, al
        mov ebx, eax
        shl eax, 0x10
        mov al, bl
        mov ah, bl
    L_fill:
        mov ebx, edi
        and ebx, 3
        je L_aligned
    L_head:
        mov byte ptr es:[edi], al
        inc edi
        dec ecx
        je L_done
        dec ebx
        jne L_head
    L_aligned:
        mov ebx, ecx
        shr ecx, 2
        rep stosd
        mov ecx, ebx
        and ecx, 3
        rep stosb
    L_done:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp2:
        mov ebx, eax
        shl eax, 0x10
        mov al, bl
        mov ah, bh
        shr ecx, 1
        rep stosd
        adc ecx, ecx
        rep stosw
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp4:
        rep stosd
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_bpp3:
        cmp al, ah
        jne L_bpp3_mixed
        mov ebx, eax
        shr ebx, 8
        cmp bh, al
        jne L_bpp3_mixed
        lea ecx, [ecx+ecx*2]
        rol eax, 8
        mov al, ah
        jmp L_fill
    L_bpp3_mixed:
        rol eax, 8
        mov edx, eax
        mov al, dh
        ror eax, 0x10
        mov ebx, edx
        rol ebx, 8
        mov bl, al
        mov dl, ah
        mov bh, ah
        rol eax, 8
        push ds
        mov ds, word ptr [ebp+0xc]
        mov esi, ecx
        shr esi, 2
        je L_tail3
    L_quad3:
        mov dword ptr [edi], eax
        mov dword ptr [edi+4], ebx
        mov dword ptr [edi+8], edx
        add edi, 0xc
        dec esi
        jne L_quad3
    L_tail3:
        and ecx, 3
        je L_end3
    L_byte3:
        mov byte ptr [edi], al
        mov byte ptr [edi+1], ah
        mov byte ptr [edi+2], bh
        add edi, 3
        dec ecx
        jne L_byte3
    L_end3:
        pop ds
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
#else
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %ebx\n"
        "push %esi\n"
        "push %edi\n"
        "push %es\n"
        "mov 0xc(%ebp), %es\n"
        "mov 8(%ebp), %edi\n"
        "mov 0x18(%ebp), %eax\n"
        "mov 0x10(%ebp), %ecx\n"
        "mov 0x14(%ebp), %ebx\n"
        "cmp $4, %ebx\n"
        "je 14f\n"
        "cmp $3, %ebx\n"
        "je 16f\n"
        "cmp $2, %ebx\n"
        "je 13f\n"
        "cmp $1, %ebx\n"
        "je 10f\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "10:\n"
        "mov %al, %ah\n"
        "mov %eax, %ebx\n"
        "shl $0x10, %eax\n"
        "mov %bl, %al\n"
        "mov %bl, %ah\n"
        "11:\n"
        "mov %edi, %ebx\n"
        "and $3, %ebx\n"
        "je 3f\n"
        "2:\n"
        "mov %al, %es:(%edi)\n"
        "inc %edi\n"
        "dec %ecx\n"
        "je 4f\n"
        "dec %ebx\n"
        "jne 2b\n"
        "3:\n"
        "mov %ecx, %ebx\n"
        "shr $2, %ecx\n"
        "rep stosl %eax, %es:(%edi)\n"
        "mov %ebx, %ecx\n"
        "and $3, %ecx\n"
        "rep stosb %al, %es:(%edi)\n"
        "4:\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "13:\n"
        "mov %eax, %ebx\n"
        "shl $0x10, %eax\n"
        "mov %bl, %al\n"
        "mov %bh, %ah\n"
        "shr $1, %ecx\n"
        "rep stosl %eax, %es:(%edi)\n"
        "adc %ecx, %ecx\n"
        "rep stosw %ax, %es:(%edi)\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "14:\n"
        "rep stosl %eax, %es:(%edi)\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
        "16:\n"
        "cmp %ah, %al\n"
        "jne 17f\n"
        "mov %eax, %ebx\n"
        "shr $8, %ebx\n"
        "cmp %al, %bh\n"
        "jne 17f\n"
        "lea (%ecx,%ecx,2), %ecx\n"
        "rol $8, %eax\n"
        "mov %ah, %al\n"
        "jmp 11b\n"
        "17:\n"
        "rol $8, %eax\n"
        "mov %eax, %edx\n"
        "mov %dh, %al\n"
        "ror $0x10, %eax\n"
        "mov %edx, %ebx\n"
        "rol $8, %ebx\n"
        "mov %al, %bl\n"
        "mov %ah, %dl\n"
        "mov %ah, %bh\n"
        "rol $8, %eax\n"
        "push %ds\n"
        "mov 0xc(%ebp), %ds\n"
        "mov %ecx, %esi\n"
        "shr $2, %esi\n"
        "je 9f\n"
        "8:\n"
        "mov %eax, (%edi)\n"
        "mov %ebx, 4(%edi)\n"
        "mov %edx, 8(%edi)\n"
        "add $0xc, %edi\n"
        "dec %esi\n"
        "jne 8b\n"
        "9:\n"
        "and $3, %ecx\n"
        "je 12f\n"
        "15:\n"
        "mov %al, (%edi)\n"
        "mov %ah, 1(%edi)\n"
        "mov %bh, 2(%edi)\n"
        "add $3, %edi\n"
        "dec %ecx\n"
        "jne 15b\n"
        "12:\n"
        "pop %ds\n"
        "pop %es\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ebx\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}
