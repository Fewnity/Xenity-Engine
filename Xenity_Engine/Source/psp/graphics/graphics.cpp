#include "graphics.h"

#ifdef __PSP__

#define GUGL_IMPLEMENTATION
#include "../gu2gl.h"

#include "../../engine/debug/debug.h"

#include "../../engine/graphics/texture.h"
#include "../../engine/graphics/camera.h"

#include <pspkernel.h>
#include <malloc.h>
#include <string.h>

// PSP GE List
static unsigned int __attribute__((aligned(16))) list[262144];

void CrossGraphicsInit()
{
    // Initialize Graphics
    guglInit(list);

    // 314D79 794D31

    glClearColor(0xFF794D31); // ABGR
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);

    float halfRatio = (480.0f / 272.0f) / 2.0f * 10;
    float halfOne = 0.5f * 10;

    // glOrtho(-halfRatio, halfRatio, -halfOne, halfOne, 0.1f, 100.0f);
    glPerspective(60, (float)PSP_SCR_WIDTH / (float)PSP_SCR_HEIGHT, 0.06f, 100);

    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODEL);
    glLoadIdentity();
}

void PspStartFrame()
{
    guglStartFrame(list, GL_FALSE);
}

void CrossGraphicsLoop()
{
}

void CrossGraphicsStop()
{
    guglTerm();
}

void CrossGraphicsSwapBuffer()
{
    guglSwapBuffers(GL_TRUE, GL_FALSE);
}

#endif