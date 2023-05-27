#include "graphics.h"

#ifdef __vita__

#include <vitaGL.h>

void CrossGraphicsInit()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

#endif