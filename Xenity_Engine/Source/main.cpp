#include "main.h"

#define STB_IMAGE_IMPLEMENTATION

#include "xenity.h"

// PSP
#ifdef __PSP__
#include "psp/callbacks.h"
#include <pspdisplay.h>
#include "psp/graphics/graphics.h"
// #include <pspgu.h>

PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#endif

// PSVITA
#ifdef __vita__
#include <psp2/kernel/processmgr.h>
#include "psvita/graphics/graphics.h"
// #include <psp2/kernel/clib.h>
//  #include <psp2/kernel/threadmgr.h>
#include <psp2/display.h>

#endif

#include "engine/graphics/texture.h"

// #include <string>

// auto str = std::string("Hello");

int main(int argc, char *argv[])
{
#ifdef __PSP__
	SetupCallbacks();
#endif

	// std::string exePath = argv[0];
	std::string exePath = "";
	if (Engine::Init(exePath) != 0)
	{
		Debug::Print("Engine failed to init");
		return -1;
	}
	// Debug::Print("argv: " + exePath);
	// std::string str2 = "hello";
	// Debug::Print("str " + str);
	// Debug::Print("str2" + str2);

	// Engine::Loop();
	// Debug::Print("---- Game loop ended ----");
	// Engine::Stop();

	Vector2 v2 = Vector2(0);
	Vector2Int v2i = Vector2Int(0);
	Vector3 v3 = Vector3(0);
	Vector4 v4 = Vector4(0);
	GameObject *go = new GameObject();
	Debug::Print(std::to_string(Noise::noise2(0.6f, 0)));
	// Debug::Print(std::to_string(Time::GetTime()));
	Color blue = Color::CreateFromRGBAFloat(0, 0, 1, 1);
	CrossGraphicsInit();

	Texture *texture = new Texture();
	texture->Load("container.jpg", 1);
	bool drawed = true;

	while (true)
	{
		StartDraw();

		Time::UpdateTime();
		// CrossGraphicsLoop();
		InputSystem::ClearInputs();
		InputSystem::Read();

		if ((int)Time::GetTime() % 2 == 0)
			DrawSprite(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0), texture);

		if (InputSystem::GetKeyDown(SQUARE))
		{
			Debug::Print("SQUARE DOWN");
		}
		if (InputSystem::GetKeyUp(SQUARE))
		{
			Debug::Print("SQUARE UP");
		}
		if (InputSystem::GetKeyDown(CIRCLE))
		{
			Debug::Print("CIRCLE DOWN");
		}
		if (InputSystem::GetKeyUp(CIRCLE))
		{
			Debug::Print("CIRCLE UP");
		}
		if (InputSystem::GetKeyUp(TRIANGLE))
		{
			Debug::Print("TRIANGLE UP");
		}
		if (InputSystem::GetKeyDown(TRIANGLE))
		{
			Debug::Print("TRIANGLE DOWN");
		}
		if (InputSystem::GetKeyUp(CROSS))
		{
			Debug::Print("CROSS UP");
		}
		if (InputSystem::GetKeyDown(CROSS))
		{
			Debug::Print("CROSS DOWN");
		}

		if (InputSystem::GetKeyDown(START))
		{
			Debug::Print("START DOWN");
		}
		if (InputSystem::GetKeyUp(START))
		{
			Debug::Print("START UP");
		}
		if (InputSystem::GetKeyDown(SELECT))
		{
			Debug::Print("SELECT DOWN");
		}
		if (InputSystem::GetKeyUp(SELECT))
		{
			Debug::Print("SELECT UP");
		}

		if (InputSystem::GetKeyUp(LTRIGGER1))
		{
			Debug::Print("LTRIGGER1 UP");
		}
		if (InputSystem::GetKeyDown(LTRIGGER1))
		{
			Debug::Print("LTRIGGER1 DOWN");
		}
		if (InputSystem::GetKeyUp(RTRIGGER1))
		{
			Debug::Print("RTRIGGER1 UP");
		}
		if (InputSystem::GetKeyDown(RTRIGGER1))
		{
			Debug::Print("RTRIGGER1 DOWN");
		}

		if (InputSystem::GetKeyDown(RIGHT))
		{
			Debug::Print("RIGHT DOWN");
		}
		if (InputSystem::GetKeyUp(RIGHT))
		{
			Debug::Print("RIGHT UP");
		}
		if (InputSystem::GetKeyDown(LEFT))
		{
			Debug::Print("LEFT DOWN");
		}
		if (InputSystem::GetKeyUp(LEFT))
		{
			Debug::Print("LEFT UP");
		}
		if (InputSystem::GetKeyUp(DOWN))
		{
			Debug::Print("DOWN UP");
		}
		if (InputSystem::GetKeyDown(DOWN))
		{
			Debug::Print("DOWN DOWN");
		}
		if (InputSystem::GetKeyUp(UP))
		{
			Debug::Print("UP UP");
		}
		if (InputSystem::GetKeyDown(UP))
		{
			Debug::Print("UP DOWN");
		}

		// std::string analogStick = "Stick X =" + std::to_string(((pad.Lx - 128) / 256.0) * 2) + ", Y =" + std::to_string(((pad.Ly - 128) / 256.0) * 2);
		EndDraw();
#ifdef __vita__
		sceKernelDelayThread(16000);
#endif
	}
	CrossGraphicsStop();
#ifdef __vita__
	sceKernelExitProcess(0);
#endif

	return 0;
}