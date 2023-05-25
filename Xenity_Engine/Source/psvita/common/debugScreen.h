#pragma once

#ifdef __vita__

#include "debugScreen_custom.h"

typedef struct ColorState
{
	int fgTrueColorFlag; // flag if truecolors or ANSI/VTERM/GREYSCALE colors are used
	int bgTrueColorFlag; // flag if truecolors or ANSI/VTERM/GREYSCALE colors are used
	// truecolors
	unsigned int fgTrueColor; // color in RGB (internal BGR)
	unsigned int bgTrueColor; // color in RGB (internal BGR)
	// ANSI/VTERM/GREYSCALE colors
	unsigned char fgIndex;	   // ANSI/VTERM/GREYSCALE color code (0-255)
	unsigned char fgIntensity; // 22=normal, 1=increased ("bright"), 2=decreased ("dark")
	unsigned char bgIndex;	   // ANSI/VTERM/GREYSCALE color code (0-255)
	unsigned char bgIntensity; // 22=normal, 1=increased ("bright")
	int inversion;			   // flag if bg/fg colors are inverted

	// default colors (ANSI/VTERM/GREYSCALE)
	unsigned char fgIndexDefault;	  // default ANSI/VTERM/GREYSCALE color code
	unsigned char fgIntensityDefault; // 22=normal, 1=increased, 2=decreased
	unsigned char bgIndexDefault;	  // default ANSI/VTERM/GREYSCALE color code
	unsigned char bgIntensityDefault; // 22=normal, 1=increased
	int inversionDefault;			  // flag if bg/fg colors are inverted

	// current colors (e.g. inverted)
	unsigned int color_fg; // color in RGB (internal BGR)
	unsigned int color_bg; // color in RGB (internal BGR)
} ColorState;

#include "psvDebugScreenFont.h"

#define SCREEN_WIDTH (960)	// screen resolution x
#define SCREEN_HEIGHT (544) // screen resolution y

#ifdef DEBUG_SCREEN_CODE_INCLUDE // not recommended for your own projects, but for sake of backward compatibility
#include "debugScreen.c"
#else
#ifdef __cplusplus
extern "C"
{
#endif
	int psvDebugScreenInit();
	int psvDebugScreenPuts(const char *_text);
	int psvDebugScreenPrintf(const char *format, ...);
	void psvDebugScreenGetColorStateCopy(ColorState *copy);
	void psvDebugScreenGetCoordsXY(int *x, int *y);
	void psvDebugScreenSetCoordsXY(int *x, int *y);
	PsvDebugScreenFont *psvDebugScreenGetFont(void);
	PsvDebugScreenFont *psvDebugScreenSetFont(PsvDebugScreenFont *font);
	PsvDebugScreenFont *psvDebugScreenScaleFont2x(PsvDebugScreenFont *source_font);
#ifdef __cplusplus
}
#endif
#endif

#endif
