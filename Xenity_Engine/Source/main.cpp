#ifdef __PSP__
// #define __PSP__
#elif __vita__
#define __VITA__
#endif

#include "main.h"

#include "xenity.h"

// PSP
#ifdef __PSP__
#include "psp/callbacks.h"
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

// PSVITA
#ifdef __VITA__
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/display.h>
#include <psp2/pgf.h>

#include <vita2d.h>
// #include <cstdio>
#endif

// int main(int argc, char *argv[])
int main()
{
#ifdef __PSP__
	SetupCallbacks();
	pspDebugScreenInit(); // initialize the debug screen

	pspDebugScreenPrintf("Hello PSP!\n");
	// Debug::Print("Hello PSP!");

	while (true)
	{
		sceDisplayWaitVblankStart(); // wait for vblank
	}
#endif

#ifdef __VITA__
	vita2d_pgf *pgf;
	vita2d_init();
	vita2d_set_clear_color(RGBA8(0, 0, 0, 255));
	pgf = vita2d_load_default_pgf();

	while (true)
	{
		vita2d_start_drawing();
		vita2d_clear_screen();
		// Debug::Print("Hello PsVita!");
		vita2d_pgf_draw_text(pgf, 0, 30, RGBA8(255, 255, 255, 255), 1.0f, "Hello PsVita2!");

		vita2d_end_drawing();
		vita2d_swap_buffers();
	}

	vita2d_fini();
	vita2d_free_pgf(pgf);

	sceKernelExitProcess(0);
#endif
	return 0;
}