// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inputs.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include "../../engine/debug/debug.h"
#include <SDL3/SDL.h>

void CrossAddInputs(std::map<int, Input*>& s_keyMap, std::map<int, Input*>& s_buttonMap, Input* inputs)
{
	s_keyMap[SDLK_LEFT] = &inputs[(int)KeyCode::LEFT];
	s_keyMap[SDLK_RIGHT] = &inputs[(int)KeyCode::RIGHT];
	s_keyMap[SDLK_UP] = &inputs[(int)KeyCode::UP];
	s_keyMap[SDLK_DOWN] = &inputs[(int)KeyCode::DOWN];

	s_keyMap[SDLK_A] = &inputs[(int)KeyCode::A];
	s_keyMap[SDLK_B] = &inputs[(int)KeyCode::B];
	s_keyMap[SDLK_C] = &inputs[(int)KeyCode::C];
	s_keyMap[SDLK_D] = &inputs[(int)KeyCode::D];
	s_keyMap[SDLK_E] = &inputs[(int)KeyCode::E];
	s_keyMap[SDLK_F] = &inputs[(int)KeyCode::F];
	s_keyMap[SDLK_G] = &inputs[(int)KeyCode::G];
	s_keyMap[SDLK_H] = &inputs[(int)KeyCode::H];
	s_keyMap[SDLK_I] = &inputs[(int)KeyCode::I];
	s_keyMap[SDLK_J] = &inputs[(int)KeyCode::J];
	s_keyMap[SDLK_K] = &inputs[(int)KeyCode::K];
	s_keyMap[SDLK_L] = &inputs[(int)KeyCode::L];
	s_keyMap[SDLK_M] = &inputs[(int)KeyCode::M];
	s_keyMap[SDLK_N] = &inputs[(int)KeyCode::N];
	s_keyMap[SDLK_O] = &inputs[(int)KeyCode::O];
	s_keyMap[SDLK_P] = &inputs[(int)KeyCode::P];
	s_keyMap[SDLK_Q] = &inputs[(int)KeyCode::Q];
	s_keyMap[SDLK_R] = &inputs[(int)KeyCode::R];
	s_keyMap[SDLK_S] = &inputs[(int)KeyCode::S];
	s_keyMap[SDLK_T] = &inputs[(int)KeyCode::T];
	s_keyMap[SDLK_U] = &inputs[(int)KeyCode::U];
	s_keyMap[SDLK_V] = &inputs[(int)KeyCode::V];
	s_keyMap[SDLK_W] = &inputs[(int)KeyCode::W];
	s_keyMap[SDLK_X] = &inputs[(int)KeyCode::X];
	s_keyMap[SDLK_Y] = &inputs[(int)KeyCode::Y];
	s_keyMap[SDLK_Z] = &inputs[(int)KeyCode::Z];

	s_keyMap[SDLK_0] = &inputs[(int)KeyCode::NUM_0];
	s_keyMap[SDLK_1] = &inputs[(int)KeyCode::NUM_1];
	s_keyMap[SDLK_2] = &inputs[(int)KeyCode::NUM_2];
	s_keyMap[SDLK_3] = &inputs[(int)KeyCode::NUM_3];
	s_keyMap[SDLK_4] = &inputs[(int)KeyCode::NUM_4];
	s_keyMap[SDLK_5] = &inputs[(int)KeyCode::NUM_5];
	s_keyMap[SDLK_6] = &inputs[(int)KeyCode::NUM_6];
	s_keyMap[SDLK_7] = &inputs[(int)KeyCode::NUM_7];
	s_keyMap[SDLK_8] = &inputs[(int)KeyCode::NUM_8];
	s_keyMap[SDLK_9] = &inputs[(int)KeyCode::NUM_9];

	s_keyMap[SDLK_F1] = &inputs[(int)KeyCode::F1];
	s_keyMap[SDLK_F2] = &inputs[(int)KeyCode::F2];
	s_keyMap[SDLK_F3] = &inputs[(int)KeyCode::F3];
	s_keyMap[SDLK_F4] = &inputs[(int)KeyCode::F4];
	s_keyMap[SDLK_F5] = &inputs[(int)KeyCode::F5];
	s_keyMap[SDLK_F6] = &inputs[(int)KeyCode::F6];
	s_keyMap[SDLK_F7] = &inputs[(int)KeyCode::F7];
	s_keyMap[SDLK_F8] = &inputs[(int)KeyCode::F8];
	s_keyMap[SDLK_F9] = &inputs[(int)KeyCode::F9];
	s_keyMap[SDLK_F10] = &inputs[(int)KeyCode::F10];
	s_keyMap[SDLK_F11] = &inputs[(int)KeyCode::F11];
	s_keyMap[SDLK_F12] = &inputs[(int)KeyCode::F12];
	// TODO: Add to F24

	s_keyMap[SDLK_CAPSLOCK] = &inputs[(int)KeyCode::CAPSLOCK];
	s_keyMap[SDLK_SPACE] = &inputs[(int)KeyCode::SPACE];
	s_keyMap[SDLK_ESCAPE] = &inputs[(int)KeyCode::ESCAPE];
	s_keyMap[SDLK_RETURN] = &inputs[(int)KeyCode::RETURN];
	s_keyMap[SDLK_LCTRL] = &inputs[(int)KeyCode::LEFT_CONTROL];
	s_keyMap[SDLK_RCTRL] = &inputs[(int)KeyCode::RIGHT_CONTROL];
	s_keyMap[SDLK_LALT] = &inputs[(int)KeyCode::LEFT_ALT];
	s_keyMap[SDLK_RALT] = &inputs[(int)KeyCode::RIGHT_ALT];
	s_keyMap[SDLK_DELETE] = &inputs[(int)KeyCode::DEL];
	s_keyMap[SDLK_LSHIFT] = &inputs[(int)KeyCode::LEFT_SHIFT];
	s_keyMap[SDLK_RSHIFT] = &inputs[(int)KeyCode::RIGHT_SHIFT];

	s_buttonMap[SDL_GAMEPAD_BUTTON_SOUTH] = &inputs[(int)KeyCode::CROSS];
	s_buttonMap[SDL_GAMEPAD_BUTTON_EAST] = &inputs[(int)KeyCode::CIRCLE];
	s_buttonMap[SDL_GAMEPAD_BUTTON_WEST] = &inputs[(int)KeyCode::SQUARE];
	s_buttonMap[SDL_GAMEPAD_BUTTON_NORTH] = &inputs[(int)KeyCode::TRIANGLE];
	s_buttonMap[SDL_GAMEPAD_BUTTON_DPAD_LEFT] = &inputs[(int)KeyCode::DPAD_LEFT];
	s_buttonMap[SDL_GAMEPAD_BUTTON_DPAD_RIGHT] = &inputs[(int)KeyCode::DPAD_RIGHT];
	s_buttonMap[SDL_GAMEPAD_BUTTON_DPAD_UP] = &inputs[(int)KeyCode::DPAD_UP];
	s_buttonMap[SDL_GAMEPAD_BUTTON_DPAD_DOWN] = &inputs[(int)KeyCode::DPAD_DOWN];

	s_buttonMap[SDL_GAMEPAD_BUTTON_START] = &inputs[(int)KeyCode::START];
	s_buttonMap[SDL_GAMEPAD_BUTTON_BACK] = &inputs[(int)KeyCode::SELECT];
	s_buttonMap[SDL_GAMEPAD_BUTTON_LEFT_SHOULDER] = &inputs[(int)KeyCode::LTRIGGER1];
	s_buttonMap[SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER] = &inputs[(int)KeyCode::RTRIGGER1];
	//s_buttonMap[SDL_CONTROLLER_BUTTON_GUIDE] = &inputs[(int)KeyCode::SELECT];
}

std::vector<SDL_Gamepad*> controllers;

void CrossInputsInit()
{
	controllers.resize(8);
}

API void CrossOnControllerAdded(const int controllerId)
{
	SDL_Gamepad* controller = SDL_OpenGamepad(controllerId);

	const int playerIndex = SDL_GetGamepadPlayerIndex(controller);
	if (playerIndex < 0)
	{
		SDL_CloseGamepad(controller);
		return;
	}

	controllers[playerIndex] = controller;
}

API void CrossOnControllerRemoved(const int controllerId)
{
	size_t controllerCount = controllers.size();
	for (size_t i = 0; i < controllerCount; i++)
	{
		if (SDL_GetGamepadInstanceID(controllers[i]) == controllerId)
		{
			SDL_CloseGamepad(controllers[i]);
			controllers[i] = nullptr;
			break;
		}
	}
}

InputPad CrossGetInputPad(const int controllerId)
{
	InputPad pad = InputPad();
	SDL_Gamepad* controller = controllers[controllerId];
	if (controller)
	{
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_SOUTH] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_SOUTH);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_EAST] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_EAST);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_WEST] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_WEST);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_NORTH] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_NORTH);

		pad.pressedButtons[SDL_GAMEPAD_BUTTON_DPAD_LEFT] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_DPAD_RIGHT] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_DPAD_UP] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_UP);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_DPAD_DOWN] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_DPAD_DOWN);

		pad.pressedButtons[SDL_GAMEPAD_BUTTON_BACK] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_BACK);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_START] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_START);

		pad.pressedButtons[SDL_GAMEPAD_BUTTON_LEFT_SHOULDER] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
		pad.pressedButtons[SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER] = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);

		const int16_t rightXValue = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTX);
		const int16_t rightYValue = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTY);
		const int16_t leftXValue = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTX);
		const int16_t leftYValue = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTY);

		pad.lx = ((leftXValue) / 65536.0f) * 2;
		pad.ly = ((leftYValue) / 65536.0f) * 2;

		pad.rx = ((rightXValue) / 65536.0f) * 2;
		pad.ry = ((rightYValue) / 65536.0f) * 2;
	}

	return pad;
}

std::vector<TouchRaw> CrossUpdateTouch()
{
	// Should be empty
	std::vector<TouchRaw> touchesRaw;
	return touchesRaw;
}

#endif