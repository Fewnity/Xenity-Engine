#pragma once

#ifdef __vita__

typedef struct PsvDebugScreenFont
{
    unsigned char *glyphs, width, height, first, last, size_w, size_h; // only values 0-255
} PsvDebugScreenFont;

#endif