#include <engine/engine.h>
#include <engine/debug/debug.h>

// PSP
#if defined(__PSP__)
#include <pspkernel.h>
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

//------------------------------- Link to the documentation: https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc

/*
--- To do quick:
Add map reflection support
Check File::GetFolderPath() for psvita, remove ux0:
Add network profiler
Add default material/Shader in the project when creating new project
Fix engine assets system

--- To do soon:
Fix Awake : Call awake when a gameobject was disabled and then enabled
Add filter to file dialog
Add abitily to move file into a folder
Add more unit tests
Add delete texture function in renderer for psp
Create console window
RGBA or RGB in vertex color?

--- To do in a long time
Add LOD
Add transparency setting on textures
Packing asset system
Add a tool to rotate selected gameobject
Fix tool with child and parent negative scale
Preview part in inspector : Add mesh preview
Add find function for child only in a gameobject
Change ID system, find a way to have an unique ID for files
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add  input state : Begin, Hold (and End?)
Text scale
Windows controller support
Create one shader per material to avoid reupdate shader when using the same shader on multiple material?
*/

#undef main

#include <stdio.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>

#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <fileio.h>

#include <sifrpc.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <iopcontrol.h>
#include <iopheap.h>

/* References to IOMANX.IRX */
extern unsigned char iomanX_irx[] __attribute__((aligned(16)));
extern unsigned int size_iomanX_irx;

/* References to FILEXIO.IRX */
extern unsigned char fileXio_irx[] __attribute__((aligned(16)));
extern unsigned int size_fileXio_irx;

void reset_IOP()
{
	SifInitRpc(0);
#if !defined(DEBUG) || defined(BUILD_FOR_PCSX2)
	/* Comment this line if you don't wanna debug the output */
	// while (!SifIopReset(NULL, 0))
	// {
	// };
#endif

	while (!SifIopSync())
	{
	};
	SifInitRpc(0);
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();
}

int loadIRXs()
{
	/* IOMANX.IRX */
	int __iomanX_id = SifExecModuleBuffer(&iomanX_irx, size_iomanX_irx, 0, NULL, NULL);
	if (__iomanX_id < 0)
		return -1;

	/* FILEXIO.IRX */
	int __fileXio_id = SifExecModuleBuffer(&fileXio_irx, size_fileXio_irx, 0, NULL, NULL);
	if (__fileXio_id < 0)
		return -2;

	return 0;
}

int init_fileXio_driver()
{
	int __fileXio_init_status = loadIRXs();
	if (__fileXio_init_status < 0)
		return __fileXio_init_status;

	__fileXio_init_status = fileXioInit();

	return __fileXio_init_status;
}

int main(int argc, char *argv[])
{
	init_scr();
	reset_IOP();
	int res = init_fileXio_driver();
	// SifInitRpc(0);
	// SifLoadFileInit();

	// scr_printf("Hello, World!\n");

	// // After 5 seconds, clear the screen.
	// sleep(5);
	// scr_clear();

	// // Move cursor to 20, 20
	// scr_setXY(20, 20);
	// scr_printf("Hello Again, World!\n");

	// sleep(10);

	// int ret = SifLoadModule("host0:iomanX.irx", 0, NULL);
	// int ret2 = SifLoadModule("host0:fileXio.irx", 0, NULL);

	// int ret = SifLoadModule("rom0:iomanX.irx", 0, NULL);
	// int ret2 = SifLoadModule("rom0:fileXio.irx", 0, NULL);

	// int ret3 = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	// int ret4 = SifLoadModule("rom0:XSIO2MAN", 0, NULL);

	// int ret5 = SifLoadModule("rom0:IOMANX.irx", 0, NULL);
	// int ret6 = SifLoadModule("rom0:FILEXIO.irx", 0, NULL);

	// int r = fileXioInit();
	int file0 = fileXioOpen("mass:test.txt", FIO_O_RDWR | FIO_O_CREAT, 0777);

	fileXioLseek(file0, 0, SEEK_END);
	int b = fileXioWrite(file0, "HELLO WORLDDDDD", 16);
	fileXioClose(file0);

	scr_setXY(20, 20);
	scr_printf("Hello Again, World! res %d file0 %d b %d\n", res, file0, b);
	// scr_printf("Hello Again, World! %d %d %d %d %d %d, r %d file0 %d b %d\n", ret, ret2, ret3, ret4, ret5, ret6, r, file0, b);
	sleep(10);

	// fileXioInitSkipOverride();

	// // Init engine
	// int engineInitResult = Engine::Init();
	// if (engineInitResult != 0)
	// {
	// 	Debug::PrintError("-------- Engine failed to init --------");
	// 	return -1;
	// }

	// // Engine and game loop
	// Engine::Loop();
	// Debug::Print("-------- Game loop ended --------");
	// Engine::Stop();

	return 0;
}