#include "main.h"

#define STB_IMAGE_IMPLEMENTATION

#include "xenity.h"

// PSP
#ifdef __PSP__
#include "psp/callbacks.h"
#include <pspdisplay.h>
#include "psp/graphics/graphics.h"

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
// #include <psp2/usbserial.h>
// #include <psp2/usbd.h>

#endif

#include "engine/graphics/texture.h"

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
	CrossGraphicsInit();
	// SpriteManager::Init();

	// int lastTime = 0;

	Engine::Loop();
	Debug::Print("---- Game loop ended ----");
	Engine::Stop();

	// 	// cameraGO->GetTransform()->SetRotation(Vector3(27.014, -11.887, 83.2110));
	// 	while (true)
	// 	{
	// 		Time::UpdateTime();
	// 		InputSystem::ClearInputs();
	// 		InputSystem::Read();

	// 		// Rotate camera
	// 		Vector3 rot = cameraGO->GetTransform()->GetRotation();
	// 		rot.x += InputSystem::leftJoystick.y;
	// 		rot.y += InputSystem::leftJoystick.x;
	// 		cameraGO->GetTransform()->SetRotation(rot);

	// 		// Move camera
	// 		Vector3 pos = cameraGO->GetTransform()->GetPosition();
	// 		pos.x += InputSystem::rightJoystick.x / 5.0f;
	// 		pos.z -= InputSystem::rightJoystick.y / 5.0f;
	// 		cameraGO->GetTransform()->SetPosition(pos);

	// 		SpriteManager::StartDraw();

	// 		Graphics::DrawAllDrawable();

	// 		if ((int)Time::GetTime() % 2 == 0 && (int)Time::GetTime() != lastTime)
	// 		{
	// 			lastTime = (int)Time::GetTime();
	// 			Debug::Print("FPS: " + std::to_string(1.0f / Time::GetUnscaledDeltaTime()));
	// 		}

	// 		if (InputSystem::GetKeyDown(SQUARE))
	// 		{
	// 			Debug::Print("SQUARE DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(SQUARE))
	// 		{
	// 			Debug::Print("SQUARE UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(CIRCLE))
	// 		{
	// 			Debug::Print("CIRCLE DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(CIRCLE))
	// 		{
	// 			Debug::Print("CIRCLE UP");
	// 		}
	// 		if (InputSystem::GetKeyUp(TRIANGLE))
	// 		{
	// 			Debug::Print("TRIANGLE UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(TRIANGLE))
	// 		{
	// 			Debug::Print("TRIANGLE DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(CROSS))
	// 		{
	// 			Debug::Print("CROSS UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(CROSS))
	// 		{
	// 			Debug::Print("CROSS DOWN");
	// 		}

	// 		if (InputSystem::GetKeyDown(START))
	// 		{
	// 			Debug::Print("START DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(START))
	// 		{
	// 			Debug::Print("START UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(SELECT))
	// 		{
	// 			Debug::Print("SELECT DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(SELECT))
	// 		{
	// 			Debug::Print("SELECT UP");
	// 		}

	// 		if (InputSystem::GetKeyUp(LTRIGGER1))
	// 		{
	// 			Debug::Print("LTRIGGER1 UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(LTRIGGER1))
	// 		{
	// 			Debug::Print("LTRIGGER1 DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(RTRIGGER1))
	// 		{
	// 			Debug::Print("RTRIGGER1 UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(RTRIGGER1))
	// 		{
	// 			Debug::Print("RTRIGGER1 DOWN");
	// 		}

	// 		if (InputSystem::GetKeyDown(RIGHT))
	// 		{
	// 			Debug::Print("RIGHT DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(RIGHT))
	// 		{
	// 			Debug::Print("RIGHT UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(LEFT))
	// 		{
	// 			Debug::Print("LEFT DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(LEFT))
	// 		{
	// 			Debug::Print("LEFT UP");
	// 		}
	// 		if (InputSystem::GetKeyUp(DOWN))
	// 		{
	// 			Debug::Print("DOWN UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(DOWN))
	// 		{
	// 			Debug::Print("DOWN DOWN");
	// 		}
	// 		if (InputSystem::GetKeyUp(UP))
	// 		{
	// 			Debug::Print("UP UP");
	// 		}
	// 		if (InputSystem::GetKeyDown(UP))
	// 		{
	// 			Debug::Print("UP DOWN");
	// 		}

	// 		// std::string analogStick = "Stick X =" + std::to_string(((pad.Lx - 128) / 256.0) * 2) + ", Y =" + std::to_string(((pad.Ly - 128) / 256.0) * 2);
	// 		SpriteManager::EndDraw();
	// #ifdef __vita__
	// 		sceKernelDelayThread(16000);
	// #endif
	// 	}
#ifdef __vita__
	sceKernelExitProcess(0);
#endif

	return 0;
}