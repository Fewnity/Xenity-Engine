// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "input_system.h"

#include <map>
#include <vector>
#if defined(__vita__)
#include <psvita/inputs/inputs.h>
#elif defined(__PSP__)
#include <psp/inputs/inputs.h>
#elif defined(_EE)
#include <ps2/inputs/inputs.h>
#elif defined(__PS3__)
#include <ps3/inputs/inputs.h>
#else
#include <windows/inputs/inputs.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#endif

#if defined(EDITOR)
#include <editor/ui/menus/basic/game_menu.h> // Need to remove this include
#include <editor/ui/menus/basic/scene_menu.h> // Need to remove this include
#include <editor/editor.h>
#endif

#include <engine/graphics/camera.h>
#include <engine/graphics/graphics.h>
#include <engine/debug/debug.h>

#include "input_pad.h"
#include "input_touch_raw.h"
#include <engine/debug/stack_debug_object.h>
#include <engine/debug/performance.h>
#include <engine/ui/window.h>


Vector2 InputSystem::mousePosition = Vector2(); // TODO : use a Vector2Int
Vector2 InputSystem::mouseSpeed = Vector2();
Vector2 InputSystem::mouseSpeedRaw = Vector2();

Vector2 InputSystem::leftJoystick[MAX_CONTROLLER];
Vector2 InputSystem::rightJoystick[MAX_CONTROLLER];
Input InputSystem::s_inputs[MAX_CONTROLLER][INPUT_COUNT];
float InputSystem::mouseWheel = 0;
bool InputSystem::s_hidedMouse = false;
std::map<int, Input*> InputSystem::s_keyMap[MAX_CONTROLLER];
std::map<int, Input*> InputSystem::s_buttonMap[MAX_CONTROLLER];
std::vector<InputSystem::TouchScreen*> InputSystem::screens;
bool InputSystem::s_blockGameInput = false;

void InputSystem::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	for (int controllerIndex = 0; controllerIndex < MAX_CONTROLLER; controllerIndex++)
	{
		s_keyMap[controllerIndex] = std::map<int, Input*>();
		s_buttonMap[controllerIndex] = std::map<int, Input*>();
		for (int i = 0; i < INPUT_COUNT; i++)
		{
			s_inputs[controllerIndex][i] = Input();
			s_inputs[controllerIndex][i].code = static_cast<KeyCode>(i);
		}
		CrossAddInputs(s_keyMap[controllerIndex], s_buttonMap[controllerIndex], s_inputs[controllerIndex]);
	}
	CrossInputsInit();

#if defined(__vita__)
	screens.push_back(new InputSystem::TouchScreen()); // Front
	screens.push_back(new InputSystem::TouchScreen()); // Back
#endif

	Debug::Print("-------- Input System initiated --------", true);
}

void InputSystem::HideMouse()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	SDL_SetWindowRelativeMouseMode(Window::s_window, true);
#endif
	s_hidedMouse = true;
}

void InputSystem::ShowMouse()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	SDL_SetWindowRelativeMouseMode(Window::s_window, false);
#endif
	s_hidedMouse = false;
}

int InputSystem::GetTouchScreenCount()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	return static_cast<int>(screens.size());
}

int InputSystem::GetTouchCount(const int screenIndex)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	const int screenCount = static_cast<int>(screens.size());

	if (screenCount <= screenIndex)
		return 0;

	return static_cast<int>(screens[screenIndex]->touches.size());
}

Touch InputSystem::GetTouch(const int touchIndex, const int screenIndex)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	const int screenCount = static_cast<int>(screens.size());

	if (screenCount <= screenIndex || screens[screenIndex]->touches.size() <= touchIndex)
		return Touch();

	return screens[screenIndex]->touches[touchIndex];
}

void InputSystem::UpdateControllers()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	SCOPED_PROFILER("InputSystem::UpdateControllers", scopeBenchmark);

	for (int controllerIndex = 0; controllerIndex < MAX_CONTROLLER; controllerIndex++)
	{
		InputPad pad = CrossGetInputPad(controllerIndex);
		const float JoystickDeadZone = 0.25f;

		if (pad.lx < JoystickDeadZone && pad.lx > -JoystickDeadZone && fabs(pad.ly) < JoystickDeadZone)
		{
			pad.lx = 0;
		}
		if (pad.ly < JoystickDeadZone && pad.ly > -JoystickDeadZone && fabs(pad.lx) < JoystickDeadZone)
		{
			pad.ly = 0;
		}
		if (pad.rx < JoystickDeadZone && pad.rx > -JoystickDeadZone && fabs(pad.ry) < JoystickDeadZone)
		{
			pad.rx = 0;
		}
		if (pad.ry < JoystickDeadZone && pad.ry > -JoystickDeadZone && fabs(pad.rx) < JoystickDeadZone)
		{
			pad.ry = 0;
		}

		leftJoystick[controllerIndex].x = pad.lx;
		leftJoystick[controllerIndex].y = pad.ly;

		rightJoystick[controllerIndex].x = pad.rx;
		rightJoystick[controllerIndex].y = pad.ry;

#if defined(__PSP__) || defined(__vita__) || defined(__PS3__) // For system that return a single int for buttons
		const auto mapE = s_buttonMap[controllerIndex].end();
		for (auto mapB = s_buttonMap[controllerIndex].begin(); mapB != mapE; ++mapB)
		{
			if (pad.buttons & mapB->first) // If the input is pressed
			{
				if (!mapB->second->held)
				{
					SetInput(true, mapB->second->code, controllerIndex);
				}
			}
			else
			{
				if (mapB->second->held)
				{
					SetInput(false, mapB->second->code, controllerIndex);
				}
			}
		}
#else // For system that return a map of buttons
		// Windows
		const auto pressedButtonsEnd = pad.pressedButtons.end();
		for (auto pressedButtonsBeg = pad.pressedButtons.begin(); pressedButtonsBeg != pressedButtonsEnd; ++pressedButtonsBeg)
		{
			if (pressedButtonsBeg->second)
			{
				if (!s_buttonMap[controllerIndex][pressedButtonsBeg->first]->held)
					SetInput(true, s_buttonMap[controllerIndex][pressedButtonsBeg->first]->code, controllerIndex);
			}
			else
			{
				if (s_buttonMap[controllerIndex][pressedButtonsBeg->first]->held)
					SetInput(false, s_buttonMap[controllerIndex][pressedButtonsBeg->first]->code, controllerIndex);
			}
		}
#endif
	}
}

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
void InputSystem::Read(const SDL_Event& event)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	switch (event.type)
	{
	case SDL_EVENT_MOUSE_MOTION:
	{
		// Get mouse position
#if !defined(EDITOR)
		float mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		mousePosition.x = mouseX;
		mousePosition.y = mouseY;
#else
		const std::shared_ptr<GameMenu> gameMenu = Editor::GetMenu<GameMenu>();
		const std::vector<std::shared_ptr<SceneMenu>> sceneMenus = Editor::GetMenus<SceneMenu>();
		std::shared_ptr<SceneMenu> sceneMenu = nullptr;

		for (const std::shared_ptr<SceneMenu>& menu : sceneMenus)
		{
			if (menu->IsHovered()) 
			{
				sceneMenu = menu;
			}
		}

		if (gameMenu && gameMenu->IsHovered() && (!sceneMenu || !sceneMenu->startRotatingCamera))
		{
			mousePosition = gameMenu->GetMousePosition();
		}
		else if (sceneMenu && (sceneMenu->IsHovered() || sceneMenu->startRotatingCamera))
		{
			mousePosition = sceneMenu->GetMousePosition();
		}
		else
		{
			mousePosition = Vector2(0);
		}
#endif	
		float xSpeed = 0;
		float ySpeed = 0;
		int xSpeedRaw = 0;
		int ySpeedRaw = 0;
		if (Graphics::usedCamera)
		{
			float a = 0;
			float w = 0;
			float h = 0;
#if defined(EDITOR)
			if (gameMenu && gameMenu->IsHovered() && (!sceneMenu || !sceneMenu->startRotatingCamera))
			{
				w = gameMenu->GetWindowSize().x;
				h = gameMenu->GetWindowSize().y;
			}
			else if (sceneMenu && (sceneMenu->IsHovered() || sceneMenu->startRotatingCamera))
			{
				w = sceneMenu->GetWindowSize().x;
				h = sceneMenu->GetWindowSize().y;
			}
			else
			{
				// Use window instead?
				w = static_cast<float>(Graphics::usedCamera->GetWidth());
				h = static_cast<float>(Graphics::usedCamera->GetHeight());
			}
#else
			// Use window instead?
			w = static_cast<float>(Graphics::usedCamera->GetWidth());
			h = static_cast<float>(Graphics::usedCamera->GetHeight());
#endif

			a = w / h;
			// Get mouse speed
			xSpeed = event.motion.xrel / w * a;
			ySpeed = -event.motion.yrel / h;
			xSpeedRaw = static_cast<int>(event.motion.xrel);
			ySpeedRaw = static_cast<int>(-event.motion.yrel);
		}
		mouseSpeed.x += xSpeed;
		mouseSpeed.y += ySpeed;

		mouseSpeedRaw.x += static_cast<float>(xSpeedRaw);
		mouseSpeedRaw.y += static_cast<float>(ySpeedRaw);

		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	{
		if (s_hidedMouse)
			HideMouse();
		switch (event.button.button)
		{
		case SDL_BUTTON_RIGHT:
			SetInput(true, KeyCode::MOUSE_RIGHT, PLAYER_1);
			break;
		case SDL_BUTTON_LEFT:
			SetInput(true, KeyCode::MOUSE_LEFT, PLAYER_1);
			break;
		case SDL_BUTTON_MIDDLE:
			SetInput(true, KeyCode::MOUSE_MIDDLE, PLAYER_1);
			break;
		}
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_RIGHT:
			SetInput(false, KeyCode::MOUSE_RIGHT, PLAYER_1);
			break;
		case SDL_BUTTON_LEFT:
			SetInput(false, KeyCode::MOUSE_LEFT, PLAYER_1);
			break;
		case SDL_BUTTON_MIDDLE:
			SetInput(false, KeyCode::MOUSE_MIDDLE, PLAYER_1);
			break;
		}
		break;
	}

	case SDL_EVENT_KEY_DOWN:
	{
		if (s_keyMap[PLAYER_1].count(event.key.key) != 0)
		{
			SetInput(true, s_keyMap[PLAYER_1][event.key.key]->code, PLAYER_1);
		}
		break;
	}

	case SDL_EVENT_KEY_UP:
	{
		if (s_keyMap[PLAYER_1].count(event.key.key) != 0)
		{
			SetInput(false, s_keyMap[PLAYER_1][event.key.key]->code, PLAYER_1);
		}
		break;
	}

	case SDL_EVENT_GAMEPAD_ADDED:
	{
		CrossOnControllerAdded(event.gdevice.which);
#if defined(EDITOR)
		Debug::Print("Gamepad detected, ID: " + std::to_string(event.gdevice.which));
#endif
		break;
	}

	case SDL_EVENT_GAMEPAD_REMOVED:
	{
		CrossOnControllerRemoved(event.gdevice.which);
#if defined(EDITOR)
		Debug::Print("Gamepad removed, ID: " + std::to_string(event.gdevice.which));
#endif
		break;
	}

	case SDL_EVENT_MOUSE_WHEEL:
		mouseWheel += event.wheel.y;
		break;
	}

}
#endif

void InputSystem::Read()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	SCOPED_PROFILER("InputSystem::Read", scopeBenchmark);

	const size_t screenCount = screens.size();
	const std::vector<TouchRaw> touchesRaw = CrossUpdateTouch();
	const size_t touchesRawCount = touchesRaw.size();

	for (size_t touchRawI = 0; touchRawI < touchesRawCount; touchRawI++)
	{
		const TouchRaw touchRaw = touchesRaw[touchRawI];
		TouchScreen* screen = screens[touchRaw.screenIndex];
		bool newInput = true;
		size_t foundInputIndex = 0;

		const int fingerId = touchRaw.fingerId;

		for (size_t touchI = 0; touchI < screen->touches.size(); touchI++)
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
			newTouch.startPosition = newTouch.position;
			newTouch.pressed = true;
			newTouch.held = true;
			screen->touches.push_back(newTouch);
			screen->updated.push_back(true);
		}
		else // If the input is held, update it
		{
			screen->touches[foundInputIndex].position = Vector2Int(touchRaw.position.x, touchRaw.position.y);
			screen->touches[foundInputIndex].pressed = false; // The input is not pressed anymore
		}
	}

	// Remove not updated inputs
	for (size_t screenIndex = 0; screenIndex < screenCount; screenIndex++)
	{
		TouchScreen* screen = screens[screenIndex];
		size_t touchCount = screen->updated.size();
		for (size_t updatedI = 0; updatedI < touchCount; updatedI++)
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

	UpdateControllers();
}

#pragma region Change inputs states

void InputSystem::ClearInputs()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	SCOPED_PROFILER("InputSystem::ClearInputs", scopeBenchmark);

	for (int controllerIndex = 0; controllerIndex < MAX_CONTROLLER; controllerIndex++)
	{
		for (int i = 0; i < INPUT_COUNT; i++)
		{
			SetInputInactive(static_cast<KeyCode>(i), controllerIndex);
		}
	}

	mouseSpeed.x = 0;
	mouseSpeed.y = 0;
	mouseSpeedRaw.x = 0;
	mouseSpeedRaw.y = 0;
	mouseWheel = 0;
}

#pragma endregion