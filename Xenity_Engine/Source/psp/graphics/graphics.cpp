#include "graphics.h"

#ifdef __PSP__

#define GUGL_IMPLEMENTATION
#include "../gu2gl.h"

#include "../../engine/debug/debug.h"

// PSP GE List
static unsigned int __attribute__((aligned(16))) list[262144];

void CrossGraphicsInit()
{
    // Initialize Graphics
    guglInit(list);
}

void CrossGraphicsStop()
{
    guglTerm();
}

#endif