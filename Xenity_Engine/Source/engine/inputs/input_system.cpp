#include "input_system.h"
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>
#include "../ui/window.h"
#include "input_touch_raw.h"

#include "../debug/debug.h"

#ifdef __vita__
#include "../../psvita/inputs/inputs.h"
#elif __PSP__
#include "../../psp/inputs/inputs.h"
#else
#include "../../windows/inputs/inputs.h"
#endif

#include "input_pad.h"

Vector2 InputSystem::mousePosition = Vector2(); // TODO : use a Vector2Int
Vector2 InputSystem::mouseSpeed = Vector2();
Vector2 InputSystem::mouseSpeedRaw = Vector2();

Vector2 InputSystem::leftJoystick = Vector2();
Vector2 InputSystem::rightJoystick = Vector2();
Input InputSystem::inputs[INPUT_COUNT];
float InputSystem::mouseWheel = 0;
bool InputSystem::hideMouse = false;
std::map<int, Input *> InputSystem::keyMap;
std::vector<InputSystem::TouchScreen *> InputSystem::screens;

/// <summary>
/// Init input system
/// </summary>
void InputSystem::Init()
{
	keyMap = std::map<int, Input *>();
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

int InputSystem::GetTouchScreenCount()
{
	return screens.size();
}

int InputSystem::GetTouchCount(const int screenIndex)
{
	int screenCount = screens.size();

	if (screenCount <= screenIndex)
		return 0;

	return screens[screenIndex]->touches.size();
}

Touch InputSystem::GetTouch(const int touchIndex, const int screenIndex)
{
	int screenCount = screens.size();

	if (screenCount <= screenIndex)
		return Touch();
	else if (screens[screenIndex]->touches.size() <= touchIndex)
		return Touch();

	return screens[screenIndex]->touches[touchIndex];
}

void InputSystem::Read(const SDL_Event event)
{
	switch (event.type)
	{
	case SDL_MOUSEMOTION: {
		//Get mouse position
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		mousePosition.x = (float)mouseX;
		mousePosition.y = (float)mouseY;

		float aspect = Window::GetAspectRatio();

		//Get mouse speed
		float xSpeed = event.motion.xrel / (float)Window::GetWidth() * aspect;
		float ySpeed = -event.motion.yrel / (float)Window::GetHeight();
		int xSpeedRaw = (int)event.motion.xrel;
		int ySpeedRaw = (int)-event.motion.yrel;

		mouseSpeed.x = xSpeed;
		mouseSpeed.y = ySpeed;

		mouseSpeedRaw.x = (float)xSpeedRaw;
		mouseSpeedRaw.y = (float)ySpeedRaw;
		break;
	}

	case SDL_MOUSEBUTTONDOWN: {
		if (hideMouse)
			SDL_SetRelativeMouseMode(SDL_TRUE);
		switch (event.button.button) {
		case SDL_BUTTON_RIGHT:
			SetInput(true, MOUSE_RIGHT);
			break;
		case SDL_BUTTON_LEFT:
			SetInput(true, MOUSE_LEFT);
			break;
		}
		break;
	}

	case SDL_MOUSEBUTTONUP:
	{
		switch (event.button.button) {
		case SDL_BUTTON_RIGHT:
			SetInput(false, MOUSE_RIGHT);
			break;
		case SDL_BUTTON_LEFT:
			SetInput(false, MOUSE_LEFT);
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

		//ChangeInputState(true, event.key.keysym.sym);
		break;
	}

	case SDL_KEYUP:
	{
		auto mapE = keyMap.end();
		for (auto mapB = keyMap.begin(); mapB != mapE; ++mapB)
			if (event.key.keysym.sym == mapB->first) // If the input is pressed
				if (mapB->second->held)
					SetInput(false, mapB->second->code);
				
		//ChangeInputState(false, event.key.keysym.sym);
		break;
	}

	case SDL_MOUSEWHEEL:
		mouseWheel = event.wheel.preciseY;
		break;
	}
}

/// <summary>
/// Get inputs events
/// </summary>
/// <param name="event"></param>
void InputSystem::Read()
{
	int screenCount = screens.size();
	std::vector<TouchRaw> touchesRaw = CrossUpdateTouch();
	int touchesRawCount = touchesRaw.size();

	for (int touchRawI = 0; touchRawI < touchesRawCount; touchRawI++)
	{
		TouchRaw touchRaw = touchesRaw[touchRawI];
		TouchScreen *screen = screens[touchRaw.screenIndex];
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
		TouchScreen *screen = screens[screenIndex];
		int touchCount = screen->updated.size();
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

// std::map<int, int> keyMap; // TODO : To use later
//  To use like that : SetInput(pressed, keyMap.at(keyCode));

void InputSystem::ChangeInputState(const bool pressed, const int keyCode)
{
}

#pragma region Change inputs states

/// <summary>
/// Set all keys states to inactive
/// </summary>
void InputSystem::ClearInputs()
{
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		SetInputInactive(i);
	}
	mouseSpeed.x = 0;
	mouseSpeed.y = 0;
	mouseSpeedRaw.x = 0;
	mouseSpeedRaw.y = 0;
	mouseWheel = 0;
}

/// <summary>
/// Set inputs state
/// </summary>
/// <param name="pressed"></param>
/// <param name="keyCode"></param>
void InputSystem::SetInput(const bool pressed, const int keyCode)
{
	if (pressed)
		SetInputPressed(keyCode);
	else
		SetInputReleased(keyCode);
}

/// <summary>
/// Set an input as pressed
/// </summary>
/// <param name="keyCode"></param>
void InputSystem::SetInputPressed(const int keyCode)
{
	if (!inputs[keyCode].held)
	{
		inputs[keyCode].pressed = true;
		inputs[keyCode].held = true;
	}
}

/// <summary>
/// Set an input as released
/// </summary>
/// <param name="keyCode"></param>
void InputSystem::SetInputReleased(const int keyCode)
{
	inputs[keyCode].released = true;
	inputs[keyCode].held = false;
}

/// <summary>
/// Set an input states to false
/// </summary>
/// <param name="keyCode"></param>
void InputSystem::SetInputInactive(const int keyCode)
{
	inputs[keyCode].pressed = false;
	inputs[keyCode].released = false;
}

#pragma endregion

#pragma region Getters

/// <summary>
/// Return if the key has just been pressed
/// </summary>
/// <param name="keyCode"></param>
/// <returns></returns>
bool InputSystem::GetKeyDown(const KeyCode keyCode)
{
	// return false;
	return inputs[keyCode].pressed;
}

/// <summary>
/// Return if the key is hold
/// </summary>
/// <param name="keyCode"></param>
/// <returns></returns>
bool InputSystem::GetKey(const KeyCode keyCode)
{
	// return false;
	return inputs[keyCode].held;
}

/// <summary>
/// Return if the key has just been released
/// </summary>
/// <param name="keyCode"></param>
/// <returns></returns>
bool InputSystem::GetKeyUp(const KeyCode keyCode)
{
	// return false;
	return inputs[keyCode].released;
}

#pragma endregion