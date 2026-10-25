#include "45-cutscene.h"

#include "18-graphics2.h"
#include "42-input.h"
#include "globvars.h"
#include "41-utility.h"
#include "69-sound.h"
#include "08-loading1.h"
#include "52-errors.h"
#include "globvrbm.h"
#include "platform.h"
#include "c2_string.h"
#ifdef _MSC_VER
#define __stdcall __declspec(dllimport) __stdcall
#endif
#include "smack.h"
#ifdef _MSC_VER
#undef __stdcall
#endif

// FUNCTION: CARMA2_HW 0x0043e750
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

// GLOBAL: CARMA2_HW 0x0067c3b0
void* gSmackerBuffer = NULL;

// GLOBAL: CARMA2_HW 0x0079ec60
br_material* gSmackMaterials[15] = { 0 };

// GLOBAL: CARMA2_HW 0x0079ec9c
br_pixelmap* gSmackerBackPalette = NULL;

// GLOBAL: CARMA2_HW 0x0079eca0
br_pixelmap* gSmackerPrevBackPalette = NULL;

// GLOBAL: CARMA2_HW 0x0079eca4
br_actor* gSmackerActor = NULL;

// GLOBAL: CARMA2_HW 0x0079eca8
br_pixelmap* gSmackerCurrentPalette = NULL;

// GLOBAL: CARMA2_HW 0x0079ecac
br_model* gSmackerModel = NULL;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// based on carpocalypse2 src/carma2/common/cutscene.c:67
// FUNCTION: CARMA2_HW 0x0043e770
// FUNCTION: CARMA2_SW 0x0043e880
void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name) {
    int mati;
    int i;
    int j;
    int k;
    int currentFrame;
    tU32 timeStart, timeEnd;
    int paletteChangeCount;
    int frameCount;
    tU32 frameStart;
    char* palette_pixels;
    int vertex_start;
    int face_start;
    int smackBufferFlags;
    int smackBlitFlags;
    tPath_name the_path;
    HSMACK smackHandle;
    HSMACKBLIT smackBlitHandle;
    int paletteChanged;
    br_pixelmap* tmpPalette;
    br_scalar l, r;
    br_scalar b, u;
    br_uint_32 paletteColour;
    int keyPressed;
    int buffer_start;
    int current_buffer_start;
    char* material_pixels;
    br_uint_8* current_buffer;
    br_model* model;

    smackBlitHandle = NULL;
    paletteChangeCount = 0;
    frameCount = 0;
    if (gSound_override) {
        return;
    }
    if (gNoCutscenes) {
        return;
    }
    palette_pixels = NULL;
    if (gBack_screen->type == BR_PMT_INDEX_8) {
        smackBufferFlags = 0;
        palette_pixels = gCurrent_palette->pixels;
    } else if (gBack_screen->type == BR_PMT_RGB_555) {
        smackBufferFlags = SMACKBUFFER555;
        smackBlitFlags = SMACKBUFFER555 | SMACKBLIT2X;
    } else if (gBack_screen->type == BR_PMT_RGB_565) {
        smackBufferFlags = SMACKBUFFER565;
        smackBlitFlags = SMACKBUFFER565 | SMACKBLIT2X;
    } else {
        PDEnterDebugger("Unexpected backscreen format for Smacker player");
        return;
    }

    if (gSmackerBuffer == NULL) {
        gSmackerBuffer = BrMemAllocate(320 * 200, kMem_misc);
    }

    StopMusic();
    FadePaletteDown();
    ClearEntireScreen();

    SmackUseMMX(1);
    SmackSoundUseDirectSound(NULL);

    PathCat(the_path, gApplication_path, "CUTSCENE");
    PathCat(the_path, the_path, pSmack_name);
    dr_dprintf("Trying to open smack file '%s'", the_path);

    smackHandle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
    if (smackHandle == NULL) {
        smackHandle = SmackOpen(the_path, SMACKTRACKS, -1);
    }
    if (smackHandle == NULL) {
        dr_dprintf("Unable to open smack file - attempt to load smack from CD...");
        if (!GetRegisterSourceLocation(the_path)) {
            dr_dprintf("Can't get CD directory name");
        } else {
            if (strlen(the_path) != 0 && the_path[strlen(the_path) - 1] != gDir_separator[0]) {
                strcat(the_path, gDir_separator);
            }
            strcat(the_path, "DATA");
            PathCat(the_path, the_path, "CUTSCENE");
            PathCat(the_path, the_path, pSmack_name);
            dr_dprintf("Got CD path: '%s'", the_path);
            if (PDCheckDriveExists(the_path)) {
                smackHandle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
                if (smackHandle == NULL) {
                    smackHandle = SmackOpen(the_path, SMACKTRACKS, -1);
                }
            }
        }
    }
    if (smackHandle == NULL) {
        dr_dprintf("Smack file '%s' failed to open", pSmack_name);
        PDServiceSystem(0);
        return;
    }
    dr_dprintf("Smack file opened OK");
    timeStart = PDGetTotalTime();
    if (gNbPixelBits == 16) {
        gSmackerActor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gSmackerActor->render_style = BR_RSTYLE_FACES;
        gSmackerCurrentPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        gSmackerBackPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        BrMapAdd(gSmackerCurrentPalette);

        model = BrModelAllocate("Smack Model", 60, 30);
        if (model == NULL) {
            PDEnterDebugger("No Smack model");
            BrActorFree(gSmackerActor);
            gSmackerActor = NULL;
            BrMapRemove(gSmackerCurrentPalette);
            BrPixelmapFree(gSmackerCurrentPalette);
            goto load_failed;
        }
        for (i = 0; i < 15; i++) {
            gSmackMaterials[i] = BrMaterialAllocate("Devious Smack Material");
            if (gSmackMaterials[i] == NULL) {
                PDEnterDebugger("No Smack material");
                BrActorFree(gSmackerActor);
                gSmackerActor = NULL;
                BrMapRemove(gSmackerCurrentPalette);
                BrPixelmapFree(gSmackerCurrentPalette);
                goto load_failed;
            }
            gSmackMaterials[i]->colour_map = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 64, NULL, 0);
            gSmackMaterials[i]->colour_map->map = gSmackerCurrentPalette;
            gSmackMaterials[i]->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
            gSmackMaterials[i]->flags |= BR_MATF_MAP_INTERPOLATION;
            gSmackMaterials[i]->flags |= BR_MATF_MAP_ANTIALIASING;
            BrMapAdd(gSmackMaterials[i]->colour_map->map);
            BrMapAdd(gSmackMaterials[i]->colour_map);
            BrMaterialAdd(gSmackMaterials[i]);
        }
        for (mati = 0, i = 0, vertex_start = 0, face_start = 0; mati < 15; i++) {
            b = (float) (-128 * i - 48);
            u = (float) (-128 * (i + 1) - 48);
            for (j = 0; j < 5; mati++, j++, vertex_start += 4, face_start += 2) {
                l = (float) (128 * j);
                r = (float) (128 * (j + 1));
                model->vertices[vertex_start + 0].p.v[0] = l;
                model->vertices[vertex_start + 0].p.v[1] = u;
                model->vertices[vertex_start + 0].p.v[2] = -2.f;
                model->vertices[vertex_start + 0].map.v[0] = 0.0078125f;
                model->vertices[vertex_start + 0].map.v[1] = 0.9921875f;

                model->vertices[vertex_start + 1].p.v[0] = r;
                model->vertices[vertex_start + 1].p.v[1] = u;
                model->vertices[vertex_start + 1].p.v[2] = -2.f;
                model->vertices[vertex_start + 1].map.v[0] = 0.9921875f;
                model->vertices[vertex_start + 1].map.v[1] = 0.9921875f;

                model->vertices[vertex_start + 2].p.v[0] = r;
                model->vertices[vertex_start + 2].p.v[1] = b;
                model->vertices[vertex_start + 2].p.v[2] = -2.f;
                model->vertices[vertex_start + 2].map.v[0] = 0.9921875f;
                model->vertices[vertex_start + 2].map.v[1] = 0.0078125f;

                model->vertices[vertex_start + 3].p.v[0] = l;
                model->vertices[vertex_start + 3].p.v[1] = b;
                model->vertices[vertex_start + 3].p.v[2] = -2.f;
                model->vertices[vertex_start + 3].map.v[0] = 0.0078125f;
                model->vertices[vertex_start + 3].map.v[1] = 0.0078125f;

                model->faces[face_start + 0].vertices[0] = vertex_start + 0;
                model->faces[face_start + 0].vertices[1] = vertex_start + 3;
                model->faces[face_start + 0].vertices[2] = vertex_start + 1;
                model->faces[face_start + 0].smoothing = 0;
                model->faces[face_start + 0].material = gSmackMaterials[mati];
                model->faces[face_start + 0].flags = 0;

                model->faces[face_start + 1].vertices[0] = vertex_start + 1;
                model->faces[face_start + 1].vertices[1] = vertex_start + 3;
                model->faces[face_start + 1].vertices[2] = vertex_start + 2;
                model->faces[face_start + 1].smoothing = 0;
                model->faces[face_start + 1].material = gSmackMaterials[mati];
                model->faces[face_start + 1].flags = 0;
            }
        }
        BrModelAdd(model);
        gSmackerModel = model;
        gSmackerActor->model = gSmackerModel;
        BrActorAdd(g2d_camera, gSmackerActor);
load_failed:
        if (gSmackerActor != NULL) {
            if (SmackBufferOpen(gHWnd, SMACKFRAMERATE, 320, 200, 320, 200) == 0) {
                PDEnterDebugger("sbuf");
            }
            SmackToBuffer(smackHandle, 0, 0, 320, 200, gSmackerBuffer, 0);
            for (currentFrame = 1; (tU32) currentFrame <= smackHandle->Frames; currentFrame++) {
                frameStart = PDGetTotalTime();
                if (smackHandle->NewPalette) {
                    BrMapRemove(gSmackerCurrentPalette);
                    gSmackerPrevBackPalette = gSmackerBackPalette;
                    tmpPalette = gSmackerCurrentPalette;
                    gSmackerCurrentPalette = gSmackerBackPalette;
                    gSmackerBackPalette = tmpPalette;
                    for (i = 0; i < 256; i++) {
                        paletteColour = BR_COLOUR_RGB(smackHandle->Palette[3 * i], smackHandle->Palette[3 * i + 1], smackHandle->Palette[3 * i + 2]);
                        if (paletteColour == 0) {
                            paletteColour = BR_COLOUR_RGB(4, 4, 4);
                        }
                        ((br_uint_32*) gSmackerCurrentPalette->pixels)[i] = paletteColour;
                    }
                    BrMapAdd(gSmackerCurrentPalette);
                    for (i = 0; i < 15; i++) {
                        gSmackMaterials[i]->colour_map->map = gSmackerCurrentPalette;
                        BrMapUpdate(gSmackMaterials[i]->colour_map->map, BR_MAPU_ALL);
                        BrMapUpdate(gSmackMaterials[i]->colour_map, BR_MAPU_ALL);
                        BrMaterialUpdate(gSmackMaterials[i], BR_MATU_ALL);
                    }
                }
                SmackDoFrame(smackHandle);
                BrPixelmapFill(gDepth_buffer, -1);
                for (i = 0, buffer_start = 0; i < 15; buffer_start += 64 * 320) {
                    current_buffer_start = buffer_start;
                    for (j = 0; j < 5; j++, current_buffer_start += 64, i++) {
                        material_pixels = gSmackMaterials[i]->colour_map->pixels;
                        current_buffer = (br_uint_8*) gSmackerBuffer + current_buffer_start;
                        for (k = 0; k < 64; k++) {
                            memcpy(material_pixels + 64 * k, current_buffer + 320 * k, 64);
                        }
                        BrMapUpdate(gSmackMaterials[i]->colour_map, BR_MAPU_ALL);
                        BrMaterialUpdate(gSmackMaterials[i], BR_MATU_ALL);
                    }
                }
                BrZbSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
                PDScreenBufferSwap(0);
                PDServiceSystem(0);
                if ((tU32) currentFrame != smackHandle->Frames) {
                    SmackNextFrame(smackHandle);
                }
                do {
                    keyPressed = AnyKeyDown();
                } while (SmackWait(smackHandle) && !keyPressed && PDGetTotalTime() < frameStart + 67u);
                if (keyPressed) {
                    break;
                }
            }
            if (gSmackerActor != NULL) {
                for (i = 0; i < 15; i++) {
                    BrMapRemove(gSmackMaterials[i]->colour_map);
                    BrPixelmapFree(gSmackMaterials[i]->colour_map);
                    gSmackMaterials[i]->colour_map = NULL;
                    BrMaterialRemove(gSmackMaterials[i]);
                    gSmackMaterials[i] = NULL;
                }
                BrModelRemove(gSmackerModel);
                BrActorRemove(gSmackerActor);
                BrModelFree(gSmackerModel);
                BrActorFree(gSmackerActor);
            }
        }
        FadePaletteDown();
        ClearEntireScreen();
    } else {
        if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
            smackBlitHandle = SmackBlitOpen(smackBlitFlags);
            if (smackBlitHandle == NULL) {
                goto finish;
            }
        }
        for (currentFrame = 1; (tU32) currentFrame <= smackHandle->Frames; currentFrame++) {
            frameCount++;
            if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
                SmackToBuffer(smackHandle, 0, 0, 320, 200, gSmackerBuffer, 0);
            } else {
                SmackToBuffer(smackHandle, 0, 0, gBack_screen->row_bytes, gBack_screen->height, gBack_screen->pixels, smackBufferFlags);
            }
            paletteChanged = 0;
            if (smackHandle->NewPalette) {
                paletteChanged = 1;
                paletteChangeCount += 1;
                if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
                    SmackBlitSetPalette(smackBlitHandle, smackHandle->Palette, smackHandle->PalType);
                } else {
                    for (i = 0; i < 256; i++) {
                        ((br_uint_32*) palette_pixels)[i] = BR_COLOUR_RGB(smackHandle->Palette[3 * i + 0], smackHandle->Palette[3 * i + 1], smackHandle->Palette[3 * i + 2]);
                    }
                }
            }
            SmackDoFrame(smackHandle);
            if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
                SmackBlit(smackBlitHandle, gBack_screen->pixels, gBack_screen->row_bytes, 0, 40, gSmackerBuffer, 320, 0, 0, 320, 200);
            }
            if ((tU32) currentFrame != smackHandle->Frames) {
                SmackNextFrame(smackHandle);
            }
            if (paletteChanged) {
                DRSetPalette(gCurrent_palette);
            }
            PDScreenBufferSwap(0);
            PDServiceSystem(0);
            frameStart = PDGetTotalTime();
            do {
                keyPressed = AnyKeyDown();
            } while (SmackWait(smackHandle) && !keyPressed);
            if (keyPressed) {
                break;
            }
        }
        FadePaletteDown();
        ClearEntireScreen();
        if (smackBlitHandle != NULL) {
            SmackBlitClose(smackBlitHandle);
        }
    }
finish:
    timeEnd = PDGetTotalTime();
    SmackClose(smackHandle);
    dr_dprintf("Frames %d, time %d, pal changes %d, pal changes per sec %f, av fps %f",
               frameCount, 1000 * timeEnd - 1000 * timeStart, paletteChangeCount,
               (double) paletteChangeCount / (timeEnd - timeStart) * 1000.f, (double) frameCount / (timeEnd - timeStart) * 1000.f);
    PDServiceSystem(0);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0043f330
// FUNCTION: CARMA2_SW 0x0043f410
void C2_HOOK_FASTCALL DoOpeningAnimation(void) {
    PlaySmackerFile("INTERPLAYGO.SMK");
    PlaySmackerFile("MIX_INTR.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f350
// FUNCTION: CARMA2_SW 0x0043f430
void C2_HOOK_FASTCALL DoNewGameAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f360
void C2_HOOK_FASTCALL DoGoToRaceAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f370
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x0043f380
// FUNCTION: CARMA2_SW 0x0043f460
void C2_HOOK_FASTCALL DoGameOverAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f390
// FUNCTION: CARMA2_SW 0x0043f470
void C2_HOOK_FASTCALL DoGameCompletedAnimation(void) {

    PlaySmackerFile("TOPRANK.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f5c0
void C2_HOOK_FASTCALL StartLoadingScreen(void) {

    SwitchToLoresMode();
    StartMusicTrack(9998);
    PossibleService();
    if (gProgram_state.sausage_eater_mode) {
        SplashScreenWith("LOADSCRN.PIX");
    } else {
        SplashScreenWith("LOADSCRN.PIX");
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x0047b990
void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
