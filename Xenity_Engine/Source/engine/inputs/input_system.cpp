#include "input_system.h"
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>
#include "../ui/window.h"
#include "input_touch_raw.h"
#include "../graphics/camera.h"
#include "../debug/debug.h"

#ifdef __vita__
#include "../../psvita/inputs/inputs.h"
#elif __PSP__
#include "../../psp/inputs/inputs.h"
#else
#include "../../windows/inputs/inputs.h"
#endif

#include "input_pad.h"
#include "../graphics/graphics.h"

#if defined(EDITOR)
#include "../../editor/ui/menus/game_menu.h"
#include "../../editor/ui/menus/scene_menu.h"
#include "../../xenity_editor.h"
#endif

Vector2 InputSystem::mousePosition = Vector2(); // TODO : use a Vector2Int
Vector2 InputSystem::mouseSpeed = Vector2();
Vector2 InputSystem::mouseSpeedRaw = Vector2();

Vector2 InputSystem::leftJoystick = Vector2();
Vector2 InputSystem::rightJoystick = Vector2();
Input InputSystem::inputs[INPUT_COUNT];
float InputSystem::mouseWheel = 0;
bool InputSystem::hidedMouse = false;
std::map<int, Input*> InputSystem::keyMap;
std::vector<InputSystem::TouchScreen*> InputSystem::screens;

void InputSystem::Init()
{
	keyMap = std::map<int, Input*>();
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		inputs[i] = Input();
		inputs[i].code = (KeyCode)i;
	}

	CrossAddInputs(keyMap, inputs);
	CrossInputsInit();

#ifdef __vita__
	screens.push_back(new InputSystem::TouchScreen());
	screens.push_back(new InputSystem::TouchScreen());
#endif

	Debug::Print("-------- Input System initiated --------");
}

void InputSystem::HideMouse()
{
#if defined(_WIN32) || defined(_WIN64)
	SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
	hidedMouse = true;
}

void InputSystem::ShowMouse()
{
#if defined(_WIN32) || defined(_WIN64)
	SDL_SetRelativeMouseMode(SDL_FALSE);
#endif
	hidedMouse = false;
}

int InputSystem::GetTouchScreenCount()
{
	return (int)screens.size();
}

int InputSystem::GetTouchCount(const int screenIndex)
{
	int screenCount = (int)screens.size();

	if (screenCount <= screenIndex)
		return 0;

	return (int)screens[screenIndex]->touches.size();
}

Touch InputSystem::GetTouch(const int touchIndex, const int screenIndex)
{
	int screenCount = (int)screens.size();

	if (screenCount <= screenIndex)
		return Touch();
	else if (screens[screenIndex]->touches.size() <= touchIndex)
		return Touch();

	return screens[screenIndex]->touches[touchIndex];
}

#if defined(_WIN32) || defined(_WIN64)
void InputSystem::Read(const SDL_Event event)
{
	switch (event.type)
	{
	case SDL_MOUSEMOTION:
	{
		//event.motion.x
		// Get mouse position
#if !defined(EDITOR)
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		mousePosition.x = (float)mouseX;
		mousePosition.y = (float)mouseY;
#else
		if (Editor::gameMenu->isHovered)
		{
			mousePosition.x = Editor::gameMenu->mousePosition.x - Editor::gameMenu->windowPosition.x;
			mousePosition.y = Editor::gameMenu->mousePosition.y - Editor::gameMenu->windowPosition.y;
		}
		else {
			//mousePosition = Vector2(0);
		}
#endif	
		float xSpeed = 0;
		float ySpeed = 0;
		int xSpeedRaw = 0;
		int ySpeedRaw = 0;
		if (Graphics::usedCamera.lock())
		{
			float a = 0;
			int w = 0;
			int h = 0;
#if defined(EDITOR)
			if (Editor::gameMenu->isHovered)
			{
				w = Editor::gameMenu->windowSize.x;
				h = Editor::gameMenu->windowSize.y;
			}
			else if (Editor::sceneMenu->isHovered) 
			{
				w = Editor::sceneMenu->windowSize.x;
				h = Editor::sceneMenu->windowSize.y;
			}
			else 
			{
				//float aspect = Graphics::usedCamera.lock()->GetAspectRatio();
				w = Graphics::usedCamera.lock()->GetWidth();
				h = Graphics::usedCamera.lock()->GetHeight();
			}
#else
			w = Graphics::usedCamera.lock()->GetWidth();
			h = Graphics::usedCamera.lock()->GetHeight();
#endif

			a = (float)w / (float)h;
			// Get mouse speed
			xSpeed = event.motion.xrel / (float)w * a;
			ySpeed = -event.motion.yrel / (float)h;
			xSpeedRaw = (int)event.motion.xrel;
			ySpeedRaw = (int)-event.motion.yrel;
		}
		mouseSpeed.x = xSpeed;
		mouseSpeed.y = ySpeed;

		mouseSpeedRaw.x = (float)xSpeedRaw;
		mouseSpeedRaw.y = (float)ySpeedRaw;

		break;
	}

	case SDL_MOUSEBUTTONDOWN:
	{
		if (hidedMouse)
			HideMouse();
		switch (event.button.button)
		{
		case SDL_BUTTON_RIGHT:
			SetInput(true, MOUSE_RIGHT);
			break;
		case SDL_BUTTON_LEFT:
			SetInput(true, MOUSE_LEFT);
			break;
		case SDL_BUTTON_MIDDLE:
			SetInput(true, MOUSE_MIDDLE);
			break;
		}
		break;
	}

	case SDL_MOUSEBUTTONUP:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_RIGHT:
			SetInput(false, MOUSE_RIGHT);
			break;
		case SDL_BUTTON_LEFT:
			SetInput(false, MOUSE_LEFT);
			break;
		case SDL_BUTTON_MIDDLE:
			SetInput(false, MOUSE_MIDDLE);
			break;
		}
		break;
	}

	case SDL_KEYDOWN:
	{
		auto mapE = keyMap.end();
		for (auto mapB = keyMap.begin(); mapB != mapE; ++mapB)
			if (event.key.keysym.sym == mapB->first) // If the input is pressed
				if (!mapB->second->held)
					SetInput(true, mapB->second->code);
		break;

		// ChangeInputState(true, event.key.keysym.sym);
		break;
	}

	case SDL_KEYUP:
	{
		auto mapE = keyMap.end();
		for (auto mapB = keyMap.begin(); mapB != mapE; ++mapB)
			if (event.key.keysym.sym == mapB->first) // If the input is pressed
				if (mapB->second->held)
					SetInput(false, mapB->second->code);

		// ChangeInputState(false, event.key.keysym.sym);
		break;
	}

	case SDL_MOUSEWHEEL:
		mouseWheel = event.wheel.preciseY;
		break;
	}
}
#endif

void InputSystem::Read()
{
	int screenCount = (int)screens.size();
	std::vector<TouchRaw> touchesRaw = CrossUpdateTouch();
	int touchesRawCount = (int)touchesRaw.size();

	for (int touchRawI = 0; touchRawI < touchesRawCount; touchRawI++)
	{
		TouchRaw touchRaw = touchesRaw[touchRawI];
		TouchScreen* screen = screens[touchRaw.screenIndex];
		bool newInput = true;
		int foundInputIndex = -1;

		int fingerId = touchRaw.fingerId;

		for (int touchI = 0; touchI < screen->touches.size(); touchI++)
		{
			if (screen->touches[touchI].fingerId == fingerId)
			{
				newInput = false;
				screen->updated[touchI] = true;
				foundInputIndex = touchI;
				break;
			}
		}

		// If the input begins
		if (newInput)
		{
			Touch newTouch = Touch();
			newTouch.fingerId = touchRaw.fingerId;
			newTouch.position = Vector2Int(touchRaw.position.x, touchRaw.position.y);
			newTouch.force = 0; // Not implemented
			screen->touches.push_back(newTouch);
			screen->updated.push_back(true);
		}
		else // If the input is held, update it
		{
			screen->touches[foundInputIndex].position = Vector2Int(touchRaw.position.x, touchRaw.position.y);
			screen->touches[foundInputIndex].force = 0; // Not implemented
		}
	}

	// Remove not updated inputs
	for (int screenIndex = 0; screenIndex < screenCount; screenIndex++)
	{
		TouchScreen* screen = screens[screenIndex];
		int touchCount = (int)screen->updated.size();
		for (int updatedI = 0; updatedI < touchCount; updatedI++)
		{
			// if the input has not been updated last frame
			if (!screen->updated[updatedI])
			{
				// Remove the input from the list
				screen->touches.erase(screen->touches.begin() + updatedI);
				screen->updated.erase(screen->updated.begin() + updatedI);
				updatedI--;
				touchCount--;
			}
			else
			{
				screen->updated[updatedI] = false;
			}
		}
	}

	InputPad pad = CrossGetInputPad();
	leftJoystick.x = pad.lx;
	leftJoystick.y = pad.ly;

	rightJoystick.x = pad.rx;
	rightJoystick.y = pad.ry;

	auto mapE = keyMap.end();

	for (auto mapB = keyMap.begin(); mapB != mapE; ++mapB)
	{
		if (pad.buttons & mapB->first) // If the input is pressed
		{
			if (!mapB->second->held)
			{
				SetInput(true, mapB->second->code);
			}
		}
		else
		{
			if (mapB->second->held)
			{
				SetInput(false, mapB->second->code);
			}
		}
	}
}

void InputSystem::ChangeInputState(const bool pressed, const KeyCode keyCode)
{
}

#pragma region Change inputs states

void InputSystem::ClearInputs()
{
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		SetInputInactive((KeyCode)i);
	}
	mouseSpeed.x = 0;
	mouseSpeed.y = 0;
	mouseSpeedRaw.x = 0;
	mouseSpeedRaw.y = 0;
	mouseWheel = 0;
}

void InputSystem::SetInput(const bool pressed, const KeyCode keyCode)
{
	if (pressed)
		SetInputPressed(keyCode);
	else
		SetInputReleased(keyCode);
}

void InputSystem::SetInputPressed(const KeyCode keyCode)
{
	if (!inputs[keyCode].held)
	{
		inputs[keyCode].pressed = true;
		inputs[keyCode].held = true;
	}
}

void InputSystem::SetInputReleased(const KeyCode keyCode)
{
	inputs[keyCode].released = true;
	inputs[keyCode].held = false;
}

void InputSystem::SetInputInactive(const KeyCode keyCode)
{
	inputs[keyCode].pressed = false;
	inputs[keyCode].released = false;
}

#pragma endregion

#pragma region Getters

bool InputSystem::GetKeyDown(const KeyCode keyCode)
{
	// return false;
	return inputs[keyCode].pressed;
}

bool InputSystem::GetKey(const KeyCode keyCode)
{
	// return false;
	return inputs[keyCode].held;
}

bool InputSystem::GetKeyUp(const KeyCode keyCode)
{
	// return false;
	return inputs[keyCode].released;
}

#pragma endregion