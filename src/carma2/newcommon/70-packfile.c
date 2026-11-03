#include "70-packfile.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "52-errors.h"
#include "69-sound.h"
#include "platform.h"
#include "rec2_macros.h"

#include <string.h>

/* explicit initializers keep gTwatVfsMountPoints, gDisableTiffConversionStack and gTwatVfsFiles adjacent as in the original */
// GLOBAL: CARMA2_HW 0x00691b40
tTwatVfsMountPoint gTwatVfsMountPoints[5] = { 0 };

// GLOBAL: CARMA2_HW 0x00692068
int gDisableTiffConversionStack[2] = { 0 };

// GLOBAL: CARMA2_HW 0x00692070
tTwatVfsFile gTwatVfsFiles[51] = { 0 };

// GLOBAL: CARMA2_HW 0x006923a0
int gDisableTiffConversionStackPos;

// GLOBAL: CARMA2_HW 0x0068c724
int gDisableTiffConversion;

// FUNCTION: CARMA2_HW 0x004b4570
void C2_HOOK_FASTCALL InitPackFiles(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsMountPoint) != 264);

    for (i = 1; i < (int)REC2_ASIZE(gTwatVfsFiles); i++) {
        gTwatVfsFiles[i].start = NULL;
    }
    for (i = 0; i < (int)REC2_ASIZE(gTwatVfsMountPoints); i++) {
        gTwatVfsMountPoints[i].header = NULL;
    }

    gDisableTiffConversionStackPos = 0;
}

// FUNCTION: CARMA2_HW 0x004b45b0
tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path) {
    tPath_name twatFilePath;
    FILE* f;
    tTWTVFS twt;
    tU32 fileSize;
    tU8* fileData;
    int i;

    // file header must be 56 bytes for compatibility with .TWT files
    REC2_BUG_ON(sizeof(tTwatFileHeader) != 56);

    strcpy(twatFilePath, path);
    strcat(twatFilePath, ".TWT");

    f = fopen(twatFilePath, "rb");
    if (f != NULL) {
        for (twt = 0; ; twt++) {
            if (twt >= (int)REC2_ASIZE(gTwatVfsMountPoints)) {
                fclose(f);
                return -1;
            }
            if (gTwatVfsMountPoints[twt].header == NULL) {
                break;
            }
        }
        strcpy(gTwatVfsMountPoints[twt].path, path);
        fileSize = ReadU32(f);
        rewind(f);

        gTwatVfsMountPoints[twt].header = BrMemAllocate(fileSize, kMem_packed_file);
        fread(gTwatVfsMountPoints[twt].header, fileSize, 1, f);
        SplungeSomeData(gTwatVfsMountPoints[twt].header, fileSize);
        gTwatVfsMountPoints[twt].data = (tU8*)gTwatVfsMountPoints[twt].header;
        fclose(f);

        fileData = (tU8*)&gTwatVfsMountPoints[twt].header->fileHeaders[gTwatVfsMountPoints[twt].header->nbFiles];
        for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
            gTwatVfsMountPoints[twt].header->fileHeaders[i].data = fileData;
            fileData += (gTwatVfsMountPoints[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
        }
        return twt;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004b4730
void C2_HOOK_FASTCALL ClosePackFile(tTWTVFS twt) {

    if (twt >= 0) {
        BrMemFree(gTwatVfsMountPoints[twt].header);
        gTwatVfsMountPoints[twt].header = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004b4760
int C2_HOOK_FASTCALL PFfclose(FILE* pFile) {

    if ((uintptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        fclose(pFile);
    } else {
        gTwatVfsFiles[(uintptr_t)pFile].start = NULL;
    }
#ifdef __GNUC__
    /* MSVC matches the original by leaving eax untouched here; GCC needs a return */
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x004b4780
FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode) {
    int twt;
    int i;
    int file_index;
    size_t twt_path_len;

    for (twt = 0; twt < (int)REC2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header != NULL && strstr(pPath, gTwatVfsMountPoints[twt].path) == pPath) {
            twt_path_len = strlen(gTwatVfsMountPoints[twt].path);
            for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
                if (DRStricmp(gTwatVfsMountPoints[twt].header->fileHeaders[i].filename, &pPath[twt_path_len + 1]) == 0) {
                    for (file_index = 0; file_index < (int)REC2_ASIZE(gTwatVfsFiles) - 1; file_index++) {
                        if (gTwatVfsFiles[file_index + 1].start != NULL) {
                            continue;
                        }
                        gTwatVfsFiles[file_index + 1].start = gTwatVfsMountPoints[twt].header->fileHeaders[i].data;
                        gTwatVfsFiles[file_index + 1].pos = gTwatVfsMountPoints[twt].header->fileHeaders[i].data;
                        gTwatVfsFiles[file_index + 1].end = gTwatVfsFiles[file_index + 1].start + gTwatVfsMountPoints[twt].header->fileHeaders[i].fileSize;
                        gTwatVfsFiles[file_index + 1].error = 0;
                        return (FILE*)(file_index + 1);
                    }
                    return NULL;
                }
            }
        }
    }
    return fopen(pPath, mode);
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004b4880
C2_NAKED int C2_HOOK_FASTCALL PFfgetc(FILE* pFile) {
    __asm {
        cmp ecx, 0x33
        jb pffgetc_small
        push ecx
        call fgetc
        add esp, 4
        ret

    pffgetc_small:
        shl ecx, 4
        mov edx, dword ptr [ecx + gTwatVfsFiles + 4]
        mov eax, dword ptr [ecx + gTwatVfsFiles + 8]
        lea ecx, dword ptr [ecx + gTwatVfsFiles]
        cmp edx, eax
        jb pffgetc_ok
        or eax, 0xffffffff
        mov dword ptr [ecx + 0xc], eax
        ret

    pffgetc_ok:
        mov dword ptr [ecx + 0xc], 0
        movsx eax, byte ptr [edx]
        inc edx
        mov dword ptr [ecx + 4], edx
        ret
    }
}
#else
int C2_HOOK_FASTCALL PFfgetc(FILE* pFile) {
    tTwatVfsFile* twtFile;
    int result;

    if ((intptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return fgetc(pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    if (twtFile->pos >= twtFile->end) {
        twtFile->error = -1;
        return EOF;
    }
    twtFile->error = 0;
    result = *(tS8*)twtFile->pos;
    twtFile->pos++;
    return result;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004b48c0
C2_NAKED int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile) {
    __asm {
        cmp ecx, 0x33
        jb drfgetc2_small
        push ecx
        call fgetc
        add esp, 4
        ret

    drfgetc2_small:
        shl ecx, 4
        mov edx, dword ptr [ecx + gTwatVfsFiles + 4]
        mov eax, dword ptr [ecx + gTwatVfsFiles + 8]
        lea ecx, dword ptr [ecx + gTwatVfsFiles]
        cmp edx, eax
        jb drfgetc2_read
        or eax, 0xffffffff
        mov dword ptr [ecx + 0xc], eax
        ret

    drfgetc2_read:
        mov dword ptr [ecx + 0xc], 0
        movsx eax, byte ptr [edx]
        inc edx
        mov dword ptr [ecx + 4], edx
        ret
    }
}
#else
int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile) {
    tTwatVfsFile* twtFile;
    int result;

    if ((intptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return fgetc(pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    if (twtFile->pos >= twtFile->end) {
        twtFile->error = -1;
        return EOF;
    }
    twtFile->error = 0;
    result = *(tS8*)twtFile->pos;
    twtFile->pos++;
    return result;
}
#endif

// PFgetc

// FUNCTION: CARMA2_HW 0x004b4900
char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile) {
    tTwatVfsFile* twtFile;
    int c;
    char* writePtr;
    int i;

    if ((intptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return fgets(buffer, size, pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    writePtr = buffer;
    for (i = 0; i < (int)size - 1; i++) {
        c = (char)*twtFile->pos++;
        if (c == -1) {
            *writePtr = '\0';
            twtFile->error = -1;
            return NULL;
        }
        *writePtr++ = c;
        if (c == '\n') {
            break;
        }
        if (twtFile->pos >= twtFile->end) {
            break;
        }
    }
    *writePtr = '\0';
    twtFile->error = 0;
    return buffer;
}

// FUNCTION: CARMA2_HW 0x004b49a0
int C2_HOOK_FASTCALL PFungetc(int ch, FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((uintptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return ungetc(ch, pFile);
    }
    twtFile = &gTwatVfsFiles[(intptr_t)pFile];
    if (twtFile->pos > twtFile->start && ch != -1) {
        twtFile->pos--;
        *twtFile->pos = ch;
        twtFile->error = 0;
        return ch;
    } else {
        twtFile->error = -1;
        return -1;
    }
}

// FUNCTION: CARMA2_HW 0x004b49f0
br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f) {
    int read_size;
    tU8 *pos;
    tU8 *end;

    if ((uintptr_t)f >= REC2_ASIZE(gTwatVfsFiles) + 1) {
        return fread(buf, size, n, f);
    }
    read_size = size * n;
    end = gTwatVfsFiles[(uintptr_t)f].end;
    pos = gTwatVfsFiles[(uintptr_t)f].pos;
    if (end - pos >= read_size) {
        end = pos + read_size;
    } else {
        n = ((end - pos) / size) * size;
        read_size = size * n;//
    }
    memcpy(buf, gTwatVfsFiles[(uintptr_t)f].pos, read_size);
    gTwatVfsFiles[(uintptr_t)f].pos = end;
    gTwatVfsFiles[(uintptr_t)f].error = 0;
    return n;
}

// FUNCTION: CARMA2_HW 0x004b4a80
int C2_HOOK_FASTCALL PFfwrite(const void* buf, br_size_t size, unsigned int n, void* f) {

    if ((uintptr_t)f >= REC2_ASIZE(gTwatVfsFiles)) {
        return fwrite(buf, size, n, f);
    }
    FatalError(kFatalError_WriteAttemptToPackedFile_S, "unknown");
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004b4b00
C2_NAKED int C2_HOOK_FASTCALL PFftell(FILE* pF) {
    __asm {
        cmp ecx, 0x33
        jb pfftell_small
        push ecx
        call ftell
        add esp, 4
        ret

    pfftell_small:
        shl ecx, 4
        mov eax, dword ptr [ecx + gTwatVfsFiles + 4]
        mov edx, dword ptr [ecx + gTwatVfsFiles]
        lea ecx, dword ptr [ecx + gTwatVfsFiles]
        sub eax, edx
        mov dword ptr [ecx + 0xc], 0
        ret
    }
}
#else
int C2_HOOK_FASTCALL PFftell(FILE* pF) {
    int pos;

    if ((uintptr_t)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        return ftell(pF);
    }
    pos = gTwatVfsFiles[(uintptr_t)pF].pos - gTwatVfsFiles[(uintptr_t)pF].start;
    gTwatVfsFiles[(uintptr_t)pF].error = 0;
    return pos;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004b4b30
C2_NAKED int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, fpos_t* pos) {
    __asm {
        cmp ecx, 0x33
        jb drfsetpos_small
        push edx
        push ecx
        call fsetpos
        add esp, 8
        ret

    drfsetpos_small:
        shl ecx, 4
        lea eax, [ecx + gTwatVfsFiles]
        mov ecx, dword ptr [ecx + gTwatVfsFiles]
        cmp edx, ecx
        jb drfsetpos_fail
        cmp edx, dword ptr [eax + 8]
        ja drfsetpos_fail
        mov dword ptr [eax + 4], edx
        mov dword ptr [eax + 0xc], 0
        xor eax, eax
        ret

    drfsetpos_fail:
        mov dword ptr [eax + 0xc], 0xffffffff
        or eax, 0xffffffff
        ret
    }
}
#else
int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, fpos_t* pos) {
    tTwatVfsFile* twtFile;

    if ((intptr_t)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        return fsetpos(pF, pos);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pF];
    if ((tU8*)pos < twtFile->start || (tU8*)pos > twtFile->end) {
        twtFile->error = -1;
        return -1;
    }
    twtFile->pos = (tU8*)pos;
    twtFile->error = 0;
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004b4b70
C2_NAKED int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence) {
    __asm {
        cmp ecx, 0x33
        jb pffseek_small

        mov eax, dword ptr [esp + 4]
        push eax
        push edx
        push ecx
        call fseek
        add esp, 0xc
        ret 4

    pffseek_small:
        mov eax, dword ptr [esp + 4]
        shl ecx, 4
        sub eax, 0
        lea ecx, dword ptr [ecx + gTwatVfsFiles]
        je pffseek_set
        dec eax
        je pffseek_cur
        dec eax
        jne pffseek_default
        mov eax, dword ptr [ecx + 8]
        add eax, edx
        jmp pffseek_check

    pffseek_cur:
        mov eax, dword ptr [ecx + 4]
        add eax, edx
        jmp pffseek_check

    pffseek_set:
        add edx, dword ptr [ecx]
        mov eax, edx
        jmp pffseek_check

    pffseek_default:
        mov eax, dword ptr [esp + 4]

    pffseek_check:
        cmp eax, dword ptr [ecx]
        jb pffseek_fail
        cmp eax, dword ptr [ecx + 8]
        ja pffseek_fail
        mov dword ptr [ecx + 4], eax
        mov dword ptr [ecx + 0xc], 0
        xor eax, eax
        ret 4

    pffseek_fail:
        or eax, 0xffffffff
        mov dword ptr [ecx + 0xc], eax
        ret 4
    }
}
#else
int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence) {
    tTwatVfsFile* twtFile;
    tU8 *newpos;

    if ((intptr_t)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        return fseek(pF, offset, whence);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pF];
    switch (whence) {
    case SEEK_SET:
        newpos = twtFile->start + offset;
        break;
    case SEEK_END:
        newpos = twtFile->end + offset;
        break;
    case SEEK_CUR:
        newpos = twtFile->pos + offset;
        break;
    }
    if (newpos >= twtFile->start && newpos <= twtFile->end) {
        twtFile->pos = newpos;
        twtFile->error = 0;
        return 0;
    } else {
        twtFile->error = -1;
        return -1;
    }
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004b4be0
C2_NAKED void C2_HOOK_FASTCALL PFrewind(FILE* pF) {
    __asm {
        cmp ecx, 0x33
        jb pfrewind_small
        push ecx
        call rewind
        add esp, 4
        ret

    pfrewind_small:
        shl ecx, 4
        lea eax, dword ptr [ecx + gTwatVfsFiles]
        mov ecx, dword ptr [ecx + gTwatVfsFiles]
        mov dword ptr [eax + 0xc], 0
        mov dword ptr [eax + 4], ecx
        ret
    }
}

// FUNCTION: CARMA2_HW 0x004b4c10
C2_NAKED int C2_HOOK_FASTCALL PFfeof(FILE* pFile) {
    __asm {
        cmp ecx, 0x33
        jb pffeof_small
        mov eax, dword ptr [ecx + 0xc]
        and eax, 0x10
        ret

    pffeof_small:
        shl ecx, 4
        mov edx, dword ptr [ecx + gTwatVfsFiles + 8]
        lea eax, dword ptr [ecx + gTwatVfsFiles]
        mov ecx, dword ptr [ecx + gTwatVfsFiles + 4]
        cmp ecx, edx
        sbb eax, eax
        inc eax
        ret
    }
}
#else
void C2_HOOK_FASTCALL PFrewind(FILE* pF) {
    tTwatVfsFile* twtFile;

    if ((int)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        rewind(pF);
    } else {
        twtFile = &gTwatVfsFiles[(uintptr_t)pF];
        twtFile->error = 0;
        twtFile->pos = gTwatVfsFiles[(uintptr_t)pF].start;
    }
}

int C2_HOOK_FASTCALL PFfeof(FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((int)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return feof(pFile);
    } else {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        return twtFile->pos >= twtFile->end;
    }
}
#endif

// FUNCTION: CARMA2_HW 0x004b4c40
// FUNCTION: CARMA2_SW 0x004b0db0
int C2_HOOK_FASTCALL DRferror(FILE* pFile) {

    if ((uintptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return ferror(pFile);
    } else {
        return gTwatVfsFiles[(uintptr_t)pFile].error;
    }
}

// FUNCTION: CARMA2_HW 0x004b4c60
void C2_HOOK_FASTCALL PFclearerr(FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((int)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        clearerr(pFile);
    } else {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        twtFile->error = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b4c80
void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    int twt;
    int i;
    char buffer[256];

    for (twt = 0; twt < (int)REC2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header == NULL) {
            continue;
        }
        if (DRStricmp(pThe_path, gTwatVfsMountPoints[twt].path) != 0) {
            continue;
        }
        for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
            PathCat(buffer, pThe_path, gTwatVfsMountPoints[twt].header->fileHeaders[i].filename);
            pAction_routine(buffer);
        }
        return;
    }
    PDForEveryFile(pThe_path, pAction_routine);
}

// FUNCTION: CARMA2_HW 0x004b4d30
void C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data) {
    int twt;
    int i;
    tPath_name twt_filePath;

    for (twt = 0; twt < (int)REC2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header != NULL && DRStricmp(gTwatVfsMountPoints[twt].path, path) == 0) {
            for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
                PathCat(twt_filePath, path, gTwatVfsMountPoints[twt].header->fileHeaders[i].filename);
                pCallback(twt_filePath, data);
            }
            return;
        }
    }
    PDEnumPath(path, pCallback, data);
}

// FUNCTION: CARMA2_HW 0x004b4df0
tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path) {
    tTWTVFS res;

    res = OpenPackFile(path);
    if (res >= 0) {
        gDisableTiffConversionStack[gDisableTiffConversionStackPos] = gDisableTiffConversion;
        gDisableTiffConversionStackPos++;
        gDisableTiffConversion = 1;
    }
    return res;
}

// FUNCTION: CARMA2_HW 0x004b4e20
void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt) {

    if (twt >= 0) {
        gDisableTiffConversionStackPos -= 1;
        gDisableTiffConversion = gDisableTiffConversionStack[gDisableTiffConversionStackPos];
        ClosePackFile(twt);
    }
}

// FUNCTION: CARMA2_HW 0x004b4e60
void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void) {
    int count;

    count = gDisableTiffConversionStackPos;
    if (count != 0) { // or > 0 if it is unsigned
        gDisableTiffConversionStack[count] = gDisableTiffConversion;
        gDisableTiffConversion = gDisableTiffConversionStack[count - 1];
    }
}

// FUNCTION: CARMA2_HW 0x004b4e90
void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void) {

    if (gDisableTiffConversionStackPos != 0) {
        gDisableTiffConversion = gDisableTiffConversionStack[gDisableTiffConversionStackPos];
    }
}

// FUNCTION: CARMA2_HW 0x004b4eb0
void C2_HOOK_FASTCALL PackfileNoOp(void) {
}
