#include "graphics.h"

#ifdef __vita__

#include <vitaGL.h>
#include "../../engine/debug/debug.h"

#include "../../engine/graphics/texture.h"
#include "../../engine/graphics/camera.h"
#include <string.h>

void CrossGraphicsInit()
{
    vglInit(0x100000);

    // Setting screen clear color
    glClearColor(0.0, 0, 0.5, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(0, 960, 544, 0, -1, 1);
    gluPerspective(60, (float)960 / (float)544, 0.06f, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_TEXTURE_2D);
}

void CrossGraphicsLoop()
{
}

void CrossGraphicsStop()
{
    vglEnd();
}

void CrossGraphicsSwapBuffer()
{
    vglSwapBuffers(GL_FALSE);
}

#endif