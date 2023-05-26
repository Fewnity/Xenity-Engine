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

Texture *texture = nullptr;
Camera *camera = nullptr;

void CrossGraphicsInit()
{
    // Initialize Graphics
    guglInit(list);

    glClearColor(0xFFFF0000); // ABGR
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);
    glPerspective(60, (float)PSP_SCR_WIDTH / (float)PSP_SCR_HEIGHT, 0.06f, 100);

    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    texture = new Texture();
    texture->Load("container.jpg", GL_TRUE);

    camera = new Camera();
    camera->x = 0;
    camera->y = 0;
    camera->z = 0;
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
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