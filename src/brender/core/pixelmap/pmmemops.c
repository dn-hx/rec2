#include "pmmemops.h"

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


#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053df30
void C2_HOOK_CDECL pm_mem_copy_bits(void* dest, br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov edi, dword ptr [ebp+0x8]
            mov esi, dword ptr [ebp+0x14]
            mov edx, dword ptr [ebp+0x1c]
            mov ecx, dword ptr [ebp+0x20]
            mov eax, ecx
            shr ecx, 0x3
            and eax, 0x7
            mov dl, byte ptr [edx+g_startMasks]
            mov dh, byte ptr [eax+g_endMasks]
            mov ebx, dword ptr [ebp+0x2c]
            mov eax, dword ptr [ebp+0x28]
            push ds
            mov ds, word ptr [ebp+0xc]
            test ecx, ecx
            jne L_53df85
            dec eax
            je L_53dfa8
            dec eax
            je L_53dff9
            dec eax
            je L_53e062
            dec eax
            je L_53e0e8
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53df85:
            dec eax
            je L_53e139
            dec eax
            je L_53e19d
            dec eax
            je L_53e219
            dec eax
            je L_53e2b6
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53dfa8:
            and dl, dh
            mov ecx, dword ptr [ebp+0x24]
    L_53dfad:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L_53dfb8
            mov byte ptr [edi], bl
    L_53dfb8:
            add al, al
            jae L_53dfbf
            mov byte ptr [edi+0x1], bl
    L_53dfbf:
            add al, al
            jae L_53dfc6
            mov byte ptr [edi+0x2], bl
    L_53dfc6:
            add al, al
            jae L_53dfcd
            mov byte ptr [edi+0x3], bl
    L_53dfcd:
            add al, al
            jae L_53dfd4
            mov byte ptr [edi+0x4], bl
    L_53dfd4:
            add al, al
            jae L_53dfdb
            mov byte ptr [edi+0x5], bl
    L_53dfdb:
            add al, al
            jae L_53dfe2
            mov byte ptr [edi+0x6], bl
    L_53dfe2:
            add al, al
            jae L_53dfe9
            mov byte ptr [edi+0x7], bl
    L_53dfe9:
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec ecx
            jne L_53dfad
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53dff9:
            and dl, dh
            mov ecx, dword ptr [ebp+0x24]
    L_53dffe:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L_53e00c
            mov byte ptr [edi], bl
            mov byte ptr [edi+0x1], bh
    L_53e00c:
            add al, al
            jae L_53e016
            mov byte ptr [edi+0x2], bl
            mov byte ptr [edi+0x3], bh
    L_53e016:
            add al, al
            jae L_53e020
            mov byte ptr [edi+0x4], bl
            mov byte ptr [edi+0x5], bh
    L_53e020:
            add al, al
            jae L_53e02a
            mov byte ptr [edi+0x6], bl
            mov byte ptr [edi+0x7], bh
    L_53e02a:
            add al, al
            jae L_53e034
            mov byte ptr [edi+0x8], bl
            mov byte ptr [edi+0x9], bh
    L_53e034:
            add al, al
            jae L_53e03e
            mov byte ptr [edi+0xa], bl
            mov byte ptr [edi+0xb], bh
    L_53e03e:
            add al, al
            jae L_53e048
            mov byte ptr [edi+0xc], bl
            mov byte ptr [edi+0xd], bh
    L_53e048:
            add al, al
            jae L_53e052
            mov byte ptr [edi+0xe], bl
            mov byte ptr [edi+0xf], bh
    L_53e052:
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec ecx
            jne L_53dffe
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53e062:
            mov eax, ebx
            shr eax, 0x8
            and dl, dh
            mov ecx, dword ptr [ebp+0x24]
    L_53e06c:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L_53e07d
            mov byte ptr [edi], bl
            mov byte ptr [edi+0x1], bh
            mov byte ptr [edi+0x2], ah
    L_53e07d:
            add al, al
            jae L_53e08a
            mov byte ptr [edi+0x3], bl
            mov byte ptr [edi+0x4], bh
            mov byte ptr [edi+0x5], ah
    L_53e08a:
            add al, al
            jae L_53e097
            mov byte ptr [edi+0x6], bl
            mov byte ptr [edi+0x7], bh
            mov byte ptr [edi+0x8], ah
    L_53e097:
            add al, al
            jae L_53e0a4
            mov byte ptr [edi+0x9], bl
            mov byte ptr [edi+0xa], bh
            mov byte ptr [edi+0xb], ah
    L_53e0a4:
            add al, al
            jae L_53e0b1
            mov byte ptr [edi+0xc], bl
            mov byte ptr [edi+0xd], bh
            mov byte ptr [edi+0xe], ah
    L_53e0b1:
            add al, al
            jae L_53e0be
            mov byte ptr [edi+0xf], bl
            mov byte ptr [edi+0x10], bh
            mov byte ptr [edi+0x11], ah
    L_53e0be:
            add al, al
            jae L_53e0cb
            mov byte ptr [edi+0x12], bl
            mov byte ptr [edi+0x13], bh
            mov byte ptr [edi+0x14], ah
    L_53e0cb:
            add al, al
            jae L_53e0d8
            mov byte ptr [edi+0x15], bl
            mov byte ptr [edi+0x16], bh
            mov byte ptr [edi+0x17], ah
    L_53e0d8:
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec ecx
            jne L_53e06c
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53e0e8:
            and dl, dh
            mov ecx, dword ptr [ebp+0x24]
    L_53e0ed:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L_53e0f8
            mov dword ptr [edi], ebx
    L_53e0f8:
            add al, al
            jae L_53e0ff
            mov dword ptr [edi+0x4], ebx
    L_53e0ff:
            add al, al
            jae L_53e106
            mov dword ptr [edi+0x8], ebx
    L_53e106:
            add al, al
            jae L_53e10d
            mov dword ptr [edi+0xc], ebx
    L_53e10d:
            add al, al
            jae L_53e114
            mov dword ptr [edi+0x10], ebx
    L_53e114:
            add al, al
            jae L_53e11b
            mov dword ptr [edi+0x14], ebx
    L_53e11b:
            add al, al
            jae L_53e122
            mov dword ptr [edi+0x18], ebx
    L_53e122:
            add al, al
            jae L_53e129
            mov dword ptr [edi+0x1c], ebx
    L_53e129:
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec ecx
            jne L_53e0ed
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53e139:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L_53e141:
            inc esi
    L_53e142:
            add al, al
            jae L_53e148
            mov byte ptr [edi], bl
    L_53e148:
            add al, al
            jae L_53e14f
            mov byte ptr [edi+0x1], bl
    L_53e14f:
            add al, al
            jae L_53e156
            mov byte ptr [edi+0x2], bl
    L_53e156:
            add al, al
            jae L_53e15d
            mov byte ptr [edi+0x3], bl
    L_53e15d:
            add al, al
            jae L_53e164
            mov byte ptr [edi+0x4], bl
    L_53e164:
            add al, al
            jae L_53e16b
            mov byte ptr [edi+0x5], bl
    L_53e16b:
            add al, al
            jae L_53e172
            mov byte ptr [edi+0x6], bl
    L_53e172:
            add al, al
            jae L_53e179
            mov byte ptr [edi+0x7], bl
    L_53e179:
            mov al, byte ptr ss:[esi]
            add edi, 0x8
            dec ecx
            jg L_53e141
            js L_53e188
            and al, dh
            jmp L_53e142
    L_53e188:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec dword ptr [ebp+0x24]
            jne L_53e139
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53e19d:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L_53e1a5:
            inc esi
    L_53e1a6:
            add al, al
            jae L_53e1af
            mov byte ptr [edi], bl
            mov byte ptr [edi+0x1], bh
    L_53e1af:
            add al, al
            jae L_53e1b9
            mov byte ptr [edi+0x2], bl
            mov byte ptr [edi+0x3], bh
    L_53e1b9:
            add al, al
            jae L_53e1c3
            mov byte ptr [edi+0x4], bl
            mov byte ptr [edi+0x5], bh
    L_53e1c3:
            add al, al
            jae L_53e1cd
            mov byte ptr [edi+0x6], bl
            mov byte ptr [edi+0x7], bh
    L_53e1cd:
            add al, al
            jae L_53e1d7
            mov byte ptr [edi+0x8], bl
            mov byte ptr [edi+0x9], bh
    L_53e1d7:
            add al, al
            jae L_53e1e1
            mov byte ptr [edi+0xa], bl
            mov byte ptr [edi+0xb], bh
    L_53e1e1:
            add al, al
            jae L_53e1eb
            mov byte ptr [edi+0xc], bl
            mov byte ptr [edi+0xd], bh
    L_53e1eb:
            add al, al
            jae L_53e1f5
            mov byte ptr [edi+0xe], bl
            mov byte ptr [edi+0xf], bh
    L_53e1f5:
            mov al, byte ptr ss:[esi]
            add edi, 0x10
            dec ecx
            jg L_53e1a5
            js L_53e204
            and al, dh
            jmp L_53e1a6
    L_53e204:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec dword ptr [ebp+0x24]
            jne L_53e19d
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53e219:
            mov eax, ebx
            shr eax, 0x8
    L_53e21e:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L_53e226:
            inc esi
    L_53e227:
            add al, al
            jae L_53e233
            mov byte ptr [edi], bl
            mov byte ptr [edi+0x1], bh
            mov byte ptr [edi+0x2], ah
    L_53e233:
            add al, al
            jae L_53e240
            mov byte ptr [edi+0x3], bl
            mov byte ptr [edi+0x4], bh
            mov byte ptr [edi+0x5], ah
    L_53e240:
            add al, al
            jae L_53e24d
            mov byte ptr [edi+0x6], bl
            mov byte ptr [edi+0x7], bh
            mov byte ptr [edi+0x8], ah
    L_53e24d:
            add al, al
            jae L_53e25a
            mov byte ptr [edi+0x9], bl
            mov byte ptr [edi+0xa], bh
            mov byte ptr [edi+0xb], ah
    L_53e25a:
            add al, al
            jae L_53e267
            mov byte ptr [edi+0xc], bl
            mov byte ptr [edi+0xd], bh
            mov byte ptr [edi+0xe], ah
    L_53e267:
            add al, al
            jae L_53e274
            mov byte ptr [edi+0xf], bl
            mov byte ptr [edi+0x10], bh
            mov byte ptr [edi+0x11], ah
    L_53e274:
            add al, al
            jae L_53e281
            mov byte ptr [edi+0x12], bl
            mov byte ptr [edi+0x13], bh
            mov byte ptr [edi+0x14], ah
    L_53e281:
            add al, al
            jae L_53e28e
            mov byte ptr [edi+0x15], bl
            mov byte ptr [edi+0x16], bh
            mov byte ptr [edi+0x17], ah
    L_53e28e:
            mov al, byte ptr ss:[esi]
            add edi, 0x18
            dec ecx
            jg L_53e226
            js L_53e29d
            and al, dh
            jmp L_53e227
    L_53e29d:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec dword ptr [ebp+0x24]
            jne L_53e21e
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L_53e2b6:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L_53e2be:
            inc esi
    L_53e2bf:
            add al, al
            jae L_53e2c5
            mov dword ptr [edi], ebx
    L_53e2c5:
            add al, al
            jae L_53e2cc
            mov dword ptr [edi+0x4], ebx
    L_53e2cc:
            add al, al
            jae L_53e2d3
            mov dword ptr [edi+0x8], ebx
    L_53e2d3:
            add al, al
            jae L_53e2da
            mov dword ptr [edi+0xc], ebx
    L_53e2da:
            add al, al
            jae L_53e2e1
            mov dword ptr [edi+0x10], ebx
    L_53e2e1:
            add al, al
            jae L_53e2e8
            mov dword ptr [edi+0x14], ebx
    L_53e2e8:
            add al, al
            jae L_53e2ef
            mov dword ptr [edi+0x18], ebx
    L_53e2ef:
            add al, al
            jae L_53e2f6
            mov dword ptr [edi+0x1c], ebx
    L_53e2f6:
            mov al, byte ptr ss:[esi]
            add edi, 0x20
            dec ecx
            jg L_53e2be
            js L_53e305
            and al, dh
            jmp L_53e2bf
    L_53e305:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp+0x10]
            add esi, dword ptr [ebp+0x18]
            dec dword ptr [ebp+0x24]
            jne L_53e2b6
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    }
#else
    __asm__(".intel_syntax noprefix\n"
        "        push ebp\n"
        "        mov ebp, esp\n"
        "        push ebx\n"
        "        push esi\n"
        "        push edi\n"
        "        push es\n"
        "        mov edi, dword ptr [ebp+0x8]\n"
        "        mov esi, dword ptr [ebp+0x14]\n"
        "        mov edx, dword ptr [ebp+0x1c]\n"
        "        mov ecx, dword ptr [ebp+0x20]\n"
        "        mov eax, ecx\n"
        "        shr ecx, 0x3\n"
        "        and eax, 0x7\n"
        "        mov dl, byte ptr [edx+" C2_ASM_SYM("g_startMasks") "]\n"
        "        mov dh, byte ptr [eax+" C2_ASM_SYM("g_endMasks") "]\n"
        "        mov ebx, dword ptr [ebp+0x2c]\n"
        "        mov eax, dword ptr [ebp+0x28]\n"
        "        push ds\n"
        "        mov ds, word ptr [ebp+0xc]\n"
        "        test ecx, ecx\n"
        "        jne L_53df85\n"
        "        dec eax\n"
        "        je L_53dfa8\n"
        "        dec eax\n"
        "        je L_53dff9\n"
        "        dec eax\n"
        "        je L_53e062\n"
        "        dec eax\n"
        "        je L_53e0e8\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53df85:\n"
        "        dec eax\n"
        "        je L_53e139\n"
        "        dec eax\n"
        "        je L_53e19d\n"
        "        dec eax\n"
        "        je L_53e219\n"
        "        dec eax\n"
        "        je L_53e2b6\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53dfa8:\n"
        "        and dl, dh\n"
        "        mov ecx, dword ptr [ebp+0x24]\n"
        "L_53dfad:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "        add al, al\n"
        "        jae L_53dfb8\n"
        "        mov byte ptr [edi], bl\n"
        "L_53dfb8:\n"
        "        add al, al\n"
        "        jae L_53dfbf\n"
        "        mov byte ptr [edi+0x1], bl\n"
        "L_53dfbf:\n"
        "        add al, al\n"
        "        jae L_53dfc6\n"
        "        mov byte ptr [edi+0x2], bl\n"
        "L_53dfc6:\n"
        "        add al, al\n"
        "        jae L_53dfcd\n"
        "        mov byte ptr [edi+0x3], bl\n"
        "L_53dfcd:\n"
        "        add al, al\n"
        "        jae L_53dfd4\n"
        "        mov byte ptr [edi+0x4], bl\n"
        "L_53dfd4:\n"
        "        add al, al\n"
        "        jae L_53dfdb\n"
        "        mov byte ptr [edi+0x5], bl\n"
        "L_53dfdb:\n"
        "        add al, al\n"
        "        jae L_53dfe2\n"
        "        mov byte ptr [edi+0x6], bl\n"
        "L_53dfe2:\n"
        "        add al, al\n"
        "        jae L_53dfe9\n"
        "        mov byte ptr [edi+0x7], bl\n"
        "L_53dfe9:\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec ecx\n"
        "        jne L_53dfad\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53dff9:\n"
        "        and dl, dh\n"
        "        mov ecx, dword ptr [ebp+0x24]\n"
        "L_53dffe:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "        add al, al\n"
        "        jae L_53e00c\n"
        "        mov byte ptr [edi], bl\n"
        "        mov byte ptr [edi+0x1], bh\n"
        "L_53e00c:\n"
        "        add al, al\n"
        "        jae L_53e016\n"
        "        mov byte ptr [edi+0x2], bl\n"
        "        mov byte ptr [edi+0x3], bh\n"
        "L_53e016:\n"
        "        add al, al\n"
        "        jae L_53e020\n"
        "        mov byte ptr [edi+0x4], bl\n"
        "        mov byte ptr [edi+0x5], bh\n"
        "L_53e020:\n"
        "        add al, al\n"
        "        jae L_53e02a\n"
        "        mov byte ptr [edi+0x6], bl\n"
        "        mov byte ptr [edi+0x7], bh\n"
        "L_53e02a:\n"
        "        add al, al\n"
        "        jae L_53e034\n"
        "        mov byte ptr [edi+0x8], bl\n"
        "        mov byte ptr [edi+0x9], bh\n"
        "L_53e034:\n"
        "        add al, al\n"
        "        jae L_53e03e\n"
        "        mov byte ptr [edi+0xa], bl\n"
        "        mov byte ptr [edi+0xb], bh\n"
        "L_53e03e:\n"
        "        add al, al\n"
        "        jae L_53e048\n"
        "        mov byte ptr [edi+0xc], bl\n"
        "        mov byte ptr [edi+0xd], bh\n"
        "L_53e048:\n"
        "        add al, al\n"
        "        jae L_53e052\n"
        "        mov byte ptr [edi+0xe], bl\n"
        "        mov byte ptr [edi+0xf], bh\n"
        "L_53e052:\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec ecx\n"
        "        jne L_53dffe\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e062:\n"
        "        mov eax, ebx\n"
        "        shr eax, 0x8\n"
        "        and dl, dh\n"
        "        mov ecx, dword ptr [ebp+0x24]\n"
        "L_53e06c:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "        add al, al\n"
        "        jae L_53e07d\n"
        "        mov byte ptr [edi], bl\n"
        "        mov byte ptr [edi+0x1], bh\n"
        "        mov byte ptr [edi+0x2], ah\n"
        "L_53e07d:\n"
        "        add al, al\n"
        "        jae L_53e08a\n"
        "        mov byte ptr [edi+0x3], bl\n"
        "        mov byte ptr [edi+0x4], bh\n"
        "        mov byte ptr [edi+0x5], ah\n"
        "L_53e08a:\n"
        "        add al, al\n"
        "        jae L_53e097\n"
        "        mov byte ptr [edi+0x6], bl\n"
        "        mov byte ptr [edi+0x7], bh\n"
        "        mov byte ptr [edi+0x8], ah\n"
        "L_53e097:\n"
        "        add al, al\n"
        "        jae L_53e0a4\n"
        "        mov byte ptr [edi+0x9], bl\n"
        "        mov byte ptr [edi+0xa], bh\n"
        "        mov byte ptr [edi+0xb], ah\n"
        "L_53e0a4:\n"
        "        add al, al\n"
        "        jae L_53e0b1\n"
        "        mov byte ptr [edi+0xc], bl\n"
        "        mov byte ptr [edi+0xd], bh\n"
        "        mov byte ptr [edi+0xe], ah\n"
        "L_53e0b1:\n"
        "        add al, al\n"
        "        jae L_53e0be\n"
        "        mov byte ptr [edi+0xf], bl\n"
        "        mov byte ptr [edi+0x10], bh\n"
        "        mov byte ptr [edi+0x11], ah\n"
        "L_53e0be:\n"
        "        add al, al\n"
        "        jae L_53e0cb\n"
        "        mov byte ptr [edi+0x12], bl\n"
        "        mov byte ptr [edi+0x13], bh\n"
        "        mov byte ptr [edi+0x14], ah\n"
        "L_53e0cb:\n"
        "        add al, al\n"
        "        jae L_53e0d8\n"
        "        mov byte ptr [edi+0x15], bl\n"
        "        mov byte ptr [edi+0x16], bh\n"
        "        mov byte ptr [edi+0x17], ah\n"
        "L_53e0d8:\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec ecx\n"
        "        jne L_53e06c\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e0e8:\n"
        "        and dl, dh\n"
        "        mov ecx, dword ptr [ebp+0x24]\n"
        "L_53e0ed:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "        add al, al\n"
        "        jae L_53e0f8\n"
        "        mov dword ptr [edi], ebx\n"
        "L_53e0f8:\n"
        "        add al, al\n"
        "        jae L_53e0ff\n"
        "        mov dword ptr [edi+0x4], ebx\n"
        "L_53e0ff:\n"
        "        add al, al\n"
        "        jae L_53e106\n"
        "        mov dword ptr [edi+0x8], ebx\n"
        "L_53e106:\n"
        "        add al, al\n"
        "        jae L_53e10d\n"
        "        mov dword ptr [edi+0xc], ebx\n"
        "L_53e10d:\n"
        "        add al, al\n"
        "        jae L_53e114\n"
        "        mov dword ptr [edi+0x10], ebx\n"
        "L_53e114:\n"
        "        add al, al\n"
        "        jae L_53e11b\n"
        "        mov dword ptr [edi+0x14], ebx\n"
        "L_53e11b:\n"
        "        add al, al\n"
        "        jae L_53e122\n"
        "        mov dword ptr [edi+0x18], ebx\n"
        "L_53e122:\n"
        "        add al, al\n"
        "        jae L_53e129\n"
        "        mov dword ptr [edi+0x1c], ebx\n"
        "L_53e129:\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec ecx\n"
        "        jne L_53e0ed\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e139:\n"
        "        push ecx\n"
        "        push esi\n"
        "        push edi\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "L_53e141:\n"
        "        inc esi\n"
        "L_53e142:\n"
        "        add al, al\n"
        "        jae L_53e148\n"
        "        mov byte ptr [edi], bl\n"
        "L_53e148:\n"
        "        add al, al\n"
        "        jae L_53e14f\n"
        "        mov byte ptr [edi+0x1], bl\n"
        "L_53e14f:\n"
        "        add al, al\n"
        "        jae L_53e156\n"
        "        mov byte ptr [edi+0x2], bl\n"
        "L_53e156:\n"
        "        add al, al\n"
        "        jae L_53e15d\n"
        "        mov byte ptr [edi+0x3], bl\n"
        "L_53e15d:\n"
        "        add al, al\n"
        "        jae L_53e164\n"
        "        mov byte ptr [edi+0x4], bl\n"
        "L_53e164:\n"
        "        add al, al\n"
        "        jae L_53e16b\n"
        "        mov byte ptr [edi+0x5], bl\n"
        "L_53e16b:\n"
        "        add al, al\n"
        "        jae L_53e172\n"
        "        mov byte ptr [edi+0x6], bl\n"
        "L_53e172:\n"
        "        add al, al\n"
        "        jae L_53e179\n"
        "        mov byte ptr [edi+0x7], bl\n"
        "L_53e179:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        add edi, 0x8\n"
        "        dec ecx\n"
        "        jg L_53e141\n"
        "        js L_53e188\n"
        "        and al, dh\n"
        "        jmp L_53e142\n"
        "L_53e188:\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ecx\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec dword ptr [ebp+0x24]\n"
        "        jne L_53e139\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e19d:\n"
        "        push ecx\n"
        "        push esi\n"
        "        push edi\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "L_53e1a5:\n"
        "        inc esi\n"
        "L_53e1a6:\n"
        "        add al, al\n"
        "        jae L_53e1af\n"
        "        mov byte ptr [edi], bl\n"
        "        mov byte ptr [edi+0x1], bh\n"
        "L_53e1af:\n"
        "        add al, al\n"
        "        jae L_53e1b9\n"
        "        mov byte ptr [edi+0x2], bl\n"
        "        mov byte ptr [edi+0x3], bh\n"
        "L_53e1b9:\n"
        "        add al, al\n"
        "        jae L_53e1c3\n"
        "        mov byte ptr [edi+0x4], bl\n"
        "        mov byte ptr [edi+0x5], bh\n"
        "L_53e1c3:\n"
        "        add al, al\n"
        "        jae L_53e1cd\n"
        "        mov byte ptr [edi+0x6], bl\n"
        "        mov byte ptr [edi+0x7], bh\n"
        "L_53e1cd:\n"
        "        add al, al\n"
        "        jae L_53e1d7\n"
        "        mov byte ptr [edi+0x8], bl\n"
        "        mov byte ptr [edi+0x9], bh\n"
        "L_53e1d7:\n"
        "        add al, al\n"
        "        jae L_53e1e1\n"
        "        mov byte ptr [edi+0xa], bl\n"
        "        mov byte ptr [edi+0xb], bh\n"
        "L_53e1e1:\n"
        "        add al, al\n"
        "        jae L_53e1eb\n"
        "        mov byte ptr [edi+0xc], bl\n"
        "        mov byte ptr [edi+0xd], bh\n"
        "L_53e1eb:\n"
        "        add al, al\n"
        "        jae L_53e1f5\n"
        "        mov byte ptr [edi+0xe], bl\n"
        "        mov byte ptr [edi+0xf], bh\n"
        "L_53e1f5:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        add edi, 0x10\n"
        "        dec ecx\n"
        "        jg L_53e1a5\n"
        "        js L_53e204\n"
        "        and al, dh\n"
        "        jmp L_53e1a6\n"
        "L_53e204:\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ecx\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec dword ptr [ebp+0x24]\n"
        "        jne L_53e19d\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e219:\n"
        "        mov eax, ebx\n"
        "        shr eax, 0x8\n"
        "L_53e21e:\n"
        "        push ecx\n"
        "        push esi\n"
        "        push edi\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "L_53e226:\n"
        "        inc esi\n"
        "L_53e227:\n"
        "        add al, al\n"
        "        jae L_53e233\n"
        "        mov byte ptr [edi], bl\n"
        "        mov byte ptr [edi+0x1], bh\n"
        "        mov byte ptr [edi+0x2], ah\n"
        "L_53e233:\n"
        "        add al, al\n"
        "        jae L_53e240\n"
        "        mov byte ptr [edi+0x3], bl\n"
        "        mov byte ptr [edi+0x4], bh\n"
        "        mov byte ptr [edi+0x5], ah\n"
        "L_53e240:\n"
        "        add al, al\n"
        "        jae L_53e24d\n"
        "        mov byte ptr [edi+0x6], bl\n"
        "        mov byte ptr [edi+0x7], bh\n"
        "        mov byte ptr [edi+0x8], ah\n"
        "L_53e24d:\n"
        "        add al, al\n"
        "        jae L_53e25a\n"
        "        mov byte ptr [edi+0x9], bl\n"
        "        mov byte ptr [edi+0xa], bh\n"
        "        mov byte ptr [edi+0xb], ah\n"
        "L_53e25a:\n"
        "        add al, al\n"
        "        jae L_53e267\n"
        "        mov byte ptr [edi+0xc], bl\n"
        "        mov byte ptr [edi+0xd], bh\n"
        "        mov byte ptr [edi+0xe], ah\n"
        "L_53e267:\n"
        "        add al, al\n"
        "        jae L_53e274\n"
        "        mov byte ptr [edi+0xf], bl\n"
        "        mov byte ptr [edi+0x10], bh\n"
        "        mov byte ptr [edi+0x11], ah\n"
        "L_53e274:\n"
        "        add al, al\n"
        "        jae L_53e281\n"
        "        mov byte ptr [edi+0x12], bl\n"
        "        mov byte ptr [edi+0x13], bh\n"
        "        mov byte ptr [edi+0x14], ah\n"
        "L_53e281:\n"
        "        add al, al\n"
        "        jae L_53e28e\n"
        "        mov byte ptr [edi+0x15], bl\n"
        "        mov byte ptr [edi+0x16], bh\n"
        "        mov byte ptr [edi+0x17], ah\n"
        "L_53e28e:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        add edi, 0x18\n"
        "        dec ecx\n"
        "        jg L_53e226\n"
        "        js L_53e29d\n"
        "        and al, dh\n"
        "        jmp L_53e227\n"
        "L_53e29d:\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ecx\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec dword ptr [ebp+0x24]\n"
        "        jne L_53e21e\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e2b6:\n"
        "        push ecx\n"
        "        push esi\n"
        "        push edi\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        and al, dl\n"
        "L_53e2be:\n"
        "        inc esi\n"
        "L_53e2bf:\n"
        "        add al, al\n"
        "        jae L_53e2c5\n"
        "        mov dword ptr [edi], ebx\n"
        "L_53e2c5:\n"
        "        add al, al\n"
        "        jae L_53e2cc\n"
        "        mov dword ptr [edi+0x4], ebx\n"
        "L_53e2cc:\n"
        "        add al, al\n"
        "        jae L_53e2d3\n"
        "        mov dword ptr [edi+0x8], ebx\n"
        "L_53e2d3:\n"
        "        add al, al\n"
        "        jae L_53e2da\n"
        "        mov dword ptr [edi+0xc], ebx\n"
        "L_53e2da:\n"
        "        add al, al\n"
        "        jae L_53e2e1\n"
        "        mov dword ptr [edi+0x10], ebx\n"
        "L_53e2e1:\n"
        "        add al, al\n"
        "        jae L_53e2e8\n"
        "        mov dword ptr [edi+0x14], ebx\n"
        "L_53e2e8:\n"
        "        add al, al\n"
        "        jae L_53e2ef\n"
        "        mov dword ptr [edi+0x18], ebx\n"
        "L_53e2ef:\n"
        "        add al, al\n"
        "        jae L_53e2f6\n"
        "        mov dword ptr [edi+0x1c], ebx\n"
        "L_53e2f6:\n"
        "        mov al, byte ptr ss:[esi]\n"
        "        add edi, 0x20\n"
        "        dec ecx\n"
        "        jg L_53e2be\n"
        "        js L_53e305\n"
        "        and al, dh\n"
        "        jmp L_53e2bf\n"
        "L_53e305:\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ecx\n"
        "        add edi, dword ptr [ebp+0x10]\n"
        "        add esi, dword ptr [ebp+0x18]\n"
        "        dec dword ptr [ebp+0x24]\n"
        "        jne L_53e2b6\n"
        "        pop ds\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        ".att_syntax prefix\n");
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
// FUNCTION: CARMA2_HW 0x0053e527
void C2_HOOK_CDECL FUN_0053e527(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dest_stride, br_uint_32 src_stride, br_uint_32 bpp) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push ds
        push es
        mov ds, word ptr [ebp+0x14]
        mov es, word ptr [ebp+0xc]
        mov esi, dword ptr [ebp+0x10]
        mov edi, dword ptr [ebp+0x8]
        mov eax, dword ptr [ebp+0x28]
        mul dword ptr [ebp+0x18]
        mov ecx, eax

    row_loop:
        push ecx
        push esi
        push edi
        mov edx, edi
        and edx, 3
        je row_aligned
        neg edx
        add edx, 4

    row_head:
        mov al, byte ptr [esi]
        mov byte ptr es:[edi], al
        inc esi
        inc edi
        dec ecx
        je row_done
        dec edx
        jne row_head

    row_aligned:
        mov edx, ecx
        shr ecx, 2
        rep movsd
        mov ecx, edx
        and ecx, 3
        rep movsb

    row_done:
        pop edi
        pop esi
        pop ecx
        add edi, dword ptr [ebp+0x20]
        add esi, dword ptr [ebp+0x24]
        dec dword ptr [ebp+0x1c]
        jne row_loop
        pop es
        pop ds
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
#else
    abort();
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053e584
void C2_HOOK_CDECL FUN_0053e584(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push ds
        push es
        mov ds, word ptr [ebp+0x14]
        mov es, word ptr [ebp+0xc]
        mov esi, dword ptr [ebp+0x10]
        mov edi, dword ptr [ebp+0x8]
        mov eax, dword ptr [ebp+0x1c]
        mul dword ptr [ebp+0x18]
        mov ecx, eax
        mov edx, edi
        and edx, 3
        je copy_aligned

    copy_head:
        mov al, byte ptr [esi]
        mov byte ptr es:[edi], al
        inc esi
        inc edi
        dec ecx
        dec edx
        jne copy_head

    copy_aligned:
        mov edx, ecx
        shr ecx, 2
        rep movsd
        mov ecx, edx
        and ecx, 3
        rep stosb
        pop es
        pop ds
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
#else
    abort();
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053e5c9
void C2_HOOK_CDECL pm_mem_set_colour(br_uint_8* dest, br_uint_32 dest_qual, br_uint_32 bpp, br_uint_32 colour) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov eax, dword ptr [ebp+0x14]
        mov ebx, dword ptr [ebp+0x10]
        mov edi, dword ptr [ebp+0x8]
        _emit 8eh
        _emit 45h
        _emit 0ch
        cmp ebx, 0x1
        je L_53e5f6
        cmp ebx, 0x2
        je L_53e5ff
        cmp ebx, 0x3
        je L_53e612
        cmp ebx, 0x4
        je L_53e609
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_53e5f6:
        mov byte ptr es:[edi], al
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_53e5ff:
        mov word ptr es:[edi], ax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_53e609:
        mov dword ptr es:[edi], eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_53e612:
        mov byte ptr es:[edi], al
        mov byte ptr es:[edi+0x1], ah
        shr eax, 0x8
        mov byte ptr es:[edi+0x2], ah
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
#else
    __asm__(".intel_syntax noprefix\n"
        "        push ebp\n"
        "        mov ebp, esp\n"
        "        push ebx\n"
        "        push esi\n"
        "        push edi\n"
        "        push es\n"
        "        mov eax, dword ptr [ebp+0x14]\n"
        "        mov ebx, dword ptr [ebp+0x10]\n"
        "        mov edi, dword ptr [ebp+0x8]\n"
        ".byte 0x8e, 0x45, 0x0c\n"
        "        cmp ebx, 0x1\n"
        "        je L_53e5f6\n"
        "        cmp ebx, 0x2\n"
        "        je L_53e5ff\n"
        "        cmp ebx, 0x3\n"
        "        je L_53e612\n"
        "        cmp ebx, 0x4\n"
        "        je L_53e609\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e5f6:\n"
        "        mov byte ptr es:[edi], al\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e5ff:\n"
        "        mov word ptr es:[edi], ax\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e609:\n"
        "        mov dword ptr es:[edi], eax\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53e612:\n"
        "        mov byte ptr es:[edi], al\n"
        "        mov byte ptr es:[edi+0x1], ah\n"
        "        shr eax, 0x8\n"
        "        mov byte ptr es:[edi+0x2], ah\n"
        "        pop es\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        ".att_syntax prefix\n");
#endif
#else
    abort();
#endif
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
        abort();
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
        abort();
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
        br_uint_8 r, g, b;

        r = (br_uint_8)((colour >> 16) & 0xff);
        g = (br_uint_8)((colour >>  8) & 0xff);
        b = (br_uint_8)((colour >>  0) & 0xff);

        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_8*)dest)[3 * x + 0] = r;
                ((br_uint_8*)dest)[3 * x + 1] = g;
                ((br_uint_8*)dest)[3 * x + 2] = b;
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


#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053ee16
void C2_HOOK_CDECL pm_mem_copy_colour(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        add esp, 0xfffffffc
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ds
        push es
        fnstcw word ptr [ebp-0x2]
        fldcw word ptr g_fpuControlWord
        mov ds, word ptr [ebp+0x14]
        mov es, word ptr [ebp+0xc]
        mov esi, dword ptr [ebp+0x10]
        mov edi, dword ptr [ebp+0x8]
        mov eax, dword ptr [ebp+0x1c]
        mov ecx, 0x8
        mul dword ptr [ebp+0x18]
        sub ecx, edi
        and ecx, 0x7
        je L_53ee56
        sub eax, ecx
        jle L_53eefd
        rep movs byte ptr es:[edi], byte ptr ds:[esi]
    L_53ee56:
        mov ebx, eax
        and eax, 0x7
        shr ebx, 0x3
        je L_53eeed
        mov ecx, ebx
        and ebx, 0x1f
        shr ecx, 0x5
        je L_53eedb
        add esi, 0x80
        add edi, 0x80
    L_53ee7a:
        mov edx, 0x4
    L_53ee7f:
        fild QWORD PTR [esi-0x80]
        fild QWORD PTR [esi-0x60]
        fild QWORD PTR [esi-0x40]
        fild QWORD PTR [esi-0x20]
        fild QWORD PTR [esi]
        fild QWORD PTR [esi+0x20]
        fild QWORD PTR [esi+0x40]
        fild QWORD PTR [esi+0x60]
        fxch st(1)
        fistp QWORD PTR es:[edi+0x40]
        fistp QWORD PTR es:[edi+0x60]
        fistp QWORD PTR es:[edi+0x20]
        fistp QWORD PTR es:[edi]
        fistp QWORD PTR es:[edi-0x20]
        fistp QWORD PTR es:[edi-0x40]
        fistp QWORD PTR es:[edi-0x60]
        fistp QWORD PTR es:[edi-0x80]
        lea esi, [esi+0x8]
        lea edi, [edi+0x8]
        dec edx
        jne L_53ee7f
        add esi, 0xe0
        add edi, 0xe0
        dec ecx
        jne L_53ee7a
        sub esi, 0x80
        sub edi, 0x80
    L_53eedb:
        test ebx, ebx
        je L_53eeed
    L_53eedf:
        fild QWORD PTR [esi]
        fistp QWORD PTR es:[edi]
        add esi, 0x8
        add edi, 0x8
        dec ebx
        jne L_53eedf
    L_53eeed:
        mov ecx, eax
        rep movs byte ptr es:[edi], byte ptr ds:[esi]
        fldcw word ptr [ebp-0x2]
        pop es
        pop ds
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        leave
        ret
    L_53eefd:
        add ecx, eax
        rep movs byte ptr es:[edi], byte ptr ds:[esi]
        fldcw word ptr [ebp-0x2]
        pop es
        pop ds
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        leave
        ret
    }
#else
    __asm__(".intel_syntax noprefix\n"
        "        push ebp\n"
        "        mov ebp, esp\n"
        "        add esp, 0xfffffffc\n"
        "        push ebx\n"
        "        push ecx\n"
        "        push edx\n"
        "        push esi\n"
        "        push edi\n"
        "        push ds\n"
        "        push es\n"
        "        fnstcw word ptr [ebp-0x2]\n"
        "        fldcw word ptr " C2_ASM_SYM("g_fpuControlWord") "\n"
        "        mov ds, word ptr [ebp+0x14]\n"
        "        mov es, word ptr [ebp+0xc]\n"
        "        mov esi, dword ptr [ebp+0x10]\n"
        "        mov edi, dword ptr [ebp+0x8]\n"
        "        mov eax, dword ptr [ebp+0x1c]\n"
        "        mov ecx, 0x8\n"
        "        mul dword ptr [ebp+0x18]\n"
        "        sub ecx, edi\n"
        "        and ecx, 0x7\n"
        "        je L_53ee56\n"
        "        sub eax, ecx\n"
        "        jle L_53eefd\n"
        "        rep movs byte ptr es:[edi], byte ptr ds:[esi]\n"
        "L_53ee56:\n"
        "        mov ebx, eax\n"
        "        and eax, 0x7\n"
        "        shr ebx, 0x3\n"
        "        je L_53eeed\n"
        "        mov ecx, ebx\n"
        "        and ebx, 0x1f\n"
        "        shr ecx, 0x5\n"
        "        je L_53eedb\n"
        "        add esi, 0x80\n"
        "        add edi, 0x80\n"
        "L_53ee7a:\n"
        "        mov edx, 0x4\n"
        "L_53ee7f:\n"
        "        fild QWORD PTR [esi-0x80]\n"
        "        fild QWORD PTR [esi-0x60]\n"
        "        fild QWORD PTR [esi-0x40]\n"
        "        fild QWORD PTR [esi-0x20]\n"
        "        fild QWORD PTR [esi]\n"
        "        fild QWORD PTR [esi+0x20]\n"
        "        fild QWORD PTR [esi+0x40]\n"
        "        fild QWORD PTR [esi+0x60]\n"
        "        fxch st(1)\n"
        "        fistp QWORD PTR es:[edi+0x40]\n"
        "        fistp QWORD PTR es:[edi+0x60]\n"
        "        fistp QWORD PTR es:[edi+0x20]\n"
        "        fistp QWORD PTR es:[edi]\n"
        "        fistp QWORD PTR es:[edi-0x20]\n"
        "        fistp QWORD PTR es:[edi-0x40]\n"
        "        fistp QWORD PTR es:[edi-0x60]\n"
        "        fistp QWORD PTR es:[edi-0x80]\n"
        "        lea esi, [esi+0x8]\n"
        "        lea edi, [edi+0x8]\n"
        "        dec edx\n"
        "        jne L_53ee7f\n"
        "        add esi, 0xe0\n"
        "        add edi, 0xe0\n"
        "        dec ecx\n"
        "        jne L_53ee7a\n"
        "        sub esi, 0x80\n"
        "        sub edi, 0x80\n"
        "L_53eedb:\n"
        "        test ebx, ebx\n"
        "        je L_53eeed\n"
        "L_53eedf:\n"
        "        fild QWORD PTR [esi]\n"
        "        fistp QWORD PTR es:[edi]\n"
        "        add esi, 0x8\n"
        "        add edi, 0x8\n"
        "        dec ebx\n"
        "        jne L_53eedf\n"
        "L_53eeed:\n"
        "        mov ecx, eax\n"
        "        rep movs byte ptr es:[edi], byte ptr ds:[esi]\n"
        "        fldcw word ptr [ebp-0x2]\n"
        "        pop es\n"
        "        pop ds\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop edx\n"
        "        pop ecx\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53eefd:\n"
        "        add ecx, eax\n"
        "        rep movs byte ptr es:[edi], byte ptr ds:[esi]\n"
        "        fldcw word ptr [ebp-0x2]\n"
        "        pop es\n"
        "        pop ds\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop edx\n"
        "        pop ecx\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        ".att_syntax prefix\n");
#endif
#else
    abort();
#endif
}


#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053ef0d
void C2_HOOK_CDECL pm_mem_copy_colour_rowbyrow(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dest_stride, br_uint_32 src_stride, br_uint_32 bpp) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        push ebp
        mov ebp, esp
        add esp, 0xffffffec
        push ebx
        push esi
        push edi
        push ds
        push es
        fnstcw word ptr [ebp-0x2]
        fldcw word ptr g_fpuControlWord
        mov ds, word ptr [ebp+0x14]
        mov es, word ptr [ebp+0xc]
        mov esi, dword ptr [ebp+0x10]
        mov edi, dword ptr [ebp+0x8]
        mov eax, dword ptr [ebp+0x28]
        mov ecx, 0x8
        mul dword ptr [ebp+0x18]
        sub ecx, edi
        and ecx, 0x7
        sub eax, ecx
        jle L_53f014
        mov ebx, eax
        and eax, 0x7
        shr ebx, 0x3
        mov dword ptr [ebp-0x8], ecx
        mov ecx, ebx
        and ebx, 0x1f
        shr ecx, 0x5
        mov dword ptr [ebp-0x14], eax
        mov dword ptr [ebp-0x10], ebx
        mov dword ptr [ebp-0xc], ecx
        mov eax, dword ptr [ebp+0x1c]
    L_53ef66:
        push esi
        push edi
        mov ecx, dword ptr [ebp-0x8]
        rep movs byte ptr es:[edi], byte ptr ds:[esi]
        mov ecx, dword ptr [ebp-0xc]
        mov ebx, dword ptr [ebp-0x10]
        test ecx, ecx
        je L_53efe4
        add esi, 0x80
        add edi, 0x80
    L_53ef83:
        mov edx, 0x4
    L_53ef88:
        fild QWORD PTR [esi-0x80]
        fild QWORD PTR [esi-0x60]
        fild QWORD PTR [esi-0x40]
        fild QWORD PTR [esi-0x20]
        fild QWORD PTR [esi]
        fild QWORD PTR [esi+0x20]
        fild QWORD PTR [esi+0x40]
        fild QWORD PTR [esi+0x60]
        fxch st(1)
        fistp QWORD PTR es:[edi+0x40]
        fistp QWORD PTR es:[edi+0x60]
        fistp QWORD PTR es:[edi+0x20]
        fistp QWORD PTR es:[edi]
        fistp QWORD PTR es:[edi-0x20]
        fistp QWORD PTR es:[edi-0x40]
        fistp QWORD PTR es:[edi-0x60]
        fistp QWORD PTR es:[edi-0x80]
        lea esi, [esi+0x8]
        lea edi, [edi+0x8]
        dec edx
        jne L_53ef88
        add esi, 0xe0
        add edi, 0xe0
        dec ecx
        jne L_53ef83
        sub esi, 0x80
        sub edi, 0x80
    L_53efe4:
        test ebx, ebx
        je L_53eff6
    L_53efe8:
        fild QWORD PTR [esi]
        fistp QWORD PTR es:[edi]
        add esi, 0x8
        add edi, 0x8
        dec ebx
        jne L_53efe8
    L_53eff6:
        mov ecx, dword ptr [ebp-0x14]
        rep movs byte ptr es:[edi], byte ptr ds:[esi]
        pop edi
        pop esi
        add esi, dword ptr [ebp+0x24]
        add edi, dword ptr [ebp+0x20]
        dec eax
        jne L_53ef66
    L_53f00a:
        fldcw word ptr [ebp-0x2]
        pop es
        pop ds
        pop edi
        pop esi
        pop ebx
        leave
        ret
    L_53f014:
        add ecx, eax
        mov eax, dword ptr [ebp+0x1c]
        mov ebx, ecx
    L_53f01b:
        push esi
        push edi
        rep movs byte ptr es:[edi], byte ptr ds:[esi]
        pop edi
        pop esi
        add esi, dword ptr [ebp+0x24]
        add edi, dword ptr [ebp+0x20]
        mov ecx, ebx
        dec eax
        jne L_53f01b
        jmp L_53f00a
    }
#else
    __asm__(".intel_syntax noprefix\n"
        "        push ebp\n"
        "        mov ebp, esp\n"
        "        add esp, 0xffffffec\n"
        "        push ebx\n"
        "        push esi\n"
        "        push edi\n"
        "        push ds\n"
        "        push es\n"
        "        fnstcw word ptr [ebp-0x2]\n"
        "        fldcw word ptr " C2_ASM_SYM("g_fpuControlWord") "\n"
        "        mov ds, word ptr [ebp+0x14]\n"
        "        mov es, word ptr [ebp+0xc]\n"
        "        mov esi, dword ptr [ebp+0x10]\n"
        "        mov edi, dword ptr [ebp+0x8]\n"
        "        mov eax, dword ptr [ebp+0x28]\n"
        "        mov ecx, 0x8\n"
        "        mul dword ptr [ebp+0x18]\n"
        "        sub ecx, edi\n"
        "        and ecx, 0x7\n"
        "        sub eax, ecx\n"
        "        jle L_53f014\n"
        "        mov ebx, eax\n"
        "        and eax, 0x7\n"
        "        shr ebx, 0x3\n"
        "        mov dword ptr [ebp-0x8], ecx\n"
        "        mov ecx, ebx\n"
        "        and ebx, 0x1f\n"
        "        shr ecx, 0x5\n"
        "        mov dword ptr [ebp-0x14], eax\n"
        "        mov dword ptr [ebp-0x10], ebx\n"
        "        mov dword ptr [ebp-0xc], ecx\n"
        "        mov eax, dword ptr [ebp+0x1c]\n"
        "L_53ef66:\n"
        "        push esi\n"
        "        push edi\n"
        "        mov ecx, dword ptr [ebp-0x8]\n"
        "        rep movs byte ptr es:[edi], byte ptr ds:[esi]\n"
        "        mov ecx, dword ptr [ebp-0xc]\n"
        "        mov ebx, dword ptr [ebp-0x10]\n"
        "        test ecx, ecx\n"
        "        je L_53efe4\n"
        "        add esi, 0x80\n"
        "        add edi, 0x80\n"
        "L_53ef83:\n"
        "        mov edx, 0x4\n"
        "L_53ef88:\n"
        "        fild QWORD PTR [esi-0x80]\n"
        "        fild QWORD PTR [esi-0x60]\n"
        "        fild QWORD PTR [esi-0x40]\n"
        "        fild QWORD PTR [esi-0x20]\n"
        "        fild QWORD PTR [esi]\n"
        "        fild QWORD PTR [esi+0x20]\n"
        "        fild QWORD PTR [esi+0x40]\n"
        "        fild QWORD PTR [esi+0x60]\n"
        "        fxch st(1)\n"
        "        fistp QWORD PTR es:[edi+0x40]\n"
        "        fistp QWORD PTR es:[edi+0x60]\n"
        "        fistp QWORD PTR es:[edi+0x20]\n"
        "        fistp QWORD PTR es:[edi]\n"
        "        fistp QWORD PTR es:[edi-0x20]\n"
        "        fistp QWORD PTR es:[edi-0x40]\n"
        "        fistp QWORD PTR es:[edi-0x60]\n"
        "        fistp QWORD PTR es:[edi-0x80]\n"
        "        lea esi, [esi+0x8]\n"
        "        lea edi, [edi+0x8]\n"
        "        dec edx\n"
        "        jne L_53ef88\n"
        "        add esi, 0xe0\n"
        "        add edi, 0xe0\n"
        "        dec ecx\n"
        "        jne L_53ef83\n"
        "        sub esi, 0x80\n"
        "        sub edi, 0x80\n"
        "L_53efe4:\n"
        "        test ebx, ebx\n"
        "        je L_53eff6\n"
        "L_53efe8:\n"
        "        fild QWORD PTR [esi]\n"
        "        fistp QWORD PTR es:[edi]\n"
        "        add esi, 0x8\n"
        "        add edi, 0x8\n"
        "        dec ebx\n"
        "        jne L_53efe8\n"
        "L_53eff6:\n"
        "        mov ecx, dword ptr [ebp-0x14]\n"
        "        rep movs byte ptr es:[edi], byte ptr ds:[esi]\n"
        "        pop edi\n"
        "        pop esi\n"
        "        add esi, dword ptr [ebp+0x24]\n"
        "        add edi, dword ptr [ebp+0x20]\n"
        "        dec eax\n"
        "        jne L_53ef66\n"
        "L_53f00a:\n"
        "        fldcw word ptr [ebp-0x2]\n"
        "        pop es\n"
        "        pop ds\n"
        "        pop edi\n"
        "        pop esi\n"
        "        pop ebx\n"
        "        leave\n"
        "        ret\n"
        "L_53f014:\n"
        "        add ecx, eax\n"
        "        mov eax, dword ptr [ebp+0x1c]\n"
        "        mov ebx, ecx\n"
        "L_53f01b:\n"
        "        push esi\n"
        "        push edi\n"
        "        rep movs byte ptr es:[edi], byte ptr ds:[esi]\n"
        "        pop edi\n"
        "        pop esi\n"
        "        add esi, dword ptr [ebp+0x24]\n"
        "        add edi, dword ptr [ebp+0x20]\n"
        "        mov ecx, ebx\n"
        "        dec eax\n"
        "        jne L_53f01b\n"
        "        jmp L_53f00a\n"
        ".att_syntax prefix\n");
#endif
#else
    abort();
#endif
}
