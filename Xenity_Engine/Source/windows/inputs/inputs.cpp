#include "inputs.h"

#if defined(_WIN32) || defined(_WIN64)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include "../../engine/debug/debug.h"
#include <SDL2/SDL_events.h>

void CrossAddInputs(std::map<int, Input*>& keyMap, std::map<int, Input*>& buttonMap, Input* inputs)
{
	keyMap[SDLK_LEFT] = &inputs[(int)KeyCode::LEFT];
	keyMap[SDLK_RIGHT] = &inputs[(int)KeyCode::RIGHT];
	keyMap[SDLK_UP] = &inputs[(int)KeyCode::UP];
	keyMap[SDLK_DOWN] = &inputs[(int)KeyCode::DOWN];

	keyMap[SDLK_a] = &inputs[(int)KeyCode::A];
	keyMap[SDLK_b] = &inputs[(int)KeyCode::B];
	keyMap[SDLK_c] = &inputs[(int)KeyCode::C];
	keyMap[SDLK_d] = &inputs[(int)KeyCode::D];
	keyMap[SDLK_e] = &inputs[(int)KeyCode::E];
	keyMap[SDLK_f] = &inputs[(int)KeyCode::F];
	keyMap[SDLK_g] = &inputs[(int)KeyCode::G];
	keyMap[SDLK_h] = &inputs[(int)KeyCode::H];
	keyMap[SDLK_i] = &inputs[(int)KeyCode::I];
	keyMap[SDLK_j] = &inputs[(int)KeyCode::J];
	keyMap[SDLK_k] = &inputs[(int)KeyCode::K];
	keyMap[SDLK_l] = &inputs[(int)KeyCode::L];
	keyMap[SDLK_m] = &inputs[(int)KeyCode::M];
	keyMap[SDLK_n] = &inputs[(int)KeyCode::N];
	keyMap[SDLK_o] = &inputs[(int)KeyCode::O];
	keyMap[SDLK_p] = &inputs[(int)KeyCode::P];
	keyMap[SDLK_q] = &inputs[(int)KeyCode::Q];
	keyMap[SDLK_r] = &inputs[(int)KeyCode::R];
	keyMap[SDLK_s] = &inputs[(int)KeyCode::S];
	keyMap[SDLK_t] = &inputs[(int)KeyCode::T];
	keyMap[SDLK_u] = &inputs[(int)KeyCode::U];
	keyMap[SDLK_v] = &inputs[(int)KeyCode::V];
	keyMap[SDLK_w] = &inputs[(int)KeyCode::W];
	keyMap[SDLK_x] = &inputs[(int)KeyCode::X];
	keyMap[SDLK_y] = &inputs[(int)KeyCode::Y];
	keyMap[SDLK_z] = &inputs[(int)KeyCode::Z];

	keyMap[SDLK_0] = &inputs[(int)KeyCode::NUM_0];
	keyMap[SDLK_1] = &inputs[(int)KeyCode::NUM_1];
	keyMap[SDLK_2] = &inputs[(int)KeyCode::NUM_2];
	keyMap[SDLK_3] = &inputs[(int)KeyCode::NUM_3];
	keyMap[SDLK_4] = &inputs[(int)KeyCode::NUM_4];
	keyMap[SDLK_5] = &inputs[(int)KeyCode::NUM_5];
	keyMap[SDLK_6] = &inputs[(int)KeyCode::NUM_6];
	keyMap[SDLK_7] = &inputs[(int)KeyCode::NUM_7];
	keyMap[SDLK_8] = &inputs[(int)KeyCode::NUM_8];
	keyMap[SDLK_9] = &inputs[(int)KeyCode::NUM_9];

	keyMap[SDLK_F1] = &inputs[(int)KeyCode::F1];
	keyMap[SDLK_F2] = &inputs[(int)KeyCode::F2];
	keyMap[SDLK_F3] = &inputs[(int)KeyCode::F3];
	keyMap[SDLK_F4] = &inputs[(int)KeyCode::F4];
	keyMap[SDLK_F5] = &inputs[(int)KeyCode::F5];
	keyMap[SDLK_F6] = &inputs[(int)KeyCode::F6];
	keyMap[SDLK_F7] = &inputs[(int)KeyCode::F7];
	keyMap[SDLK_F8] = &inputs[(int)KeyCode::F8];
	keyMap[SDLK_F9] = &inputs[(int)KeyCode::F9];
	keyMap[SDLK_F10] = &inputs[(int)KeyCode::F10];
	keyMap[SDLK_F11] = &inputs[(int)KeyCode::F11];
	keyMap[SDLK_F12] = &inputs[(int)KeyCode::F12];
	// TODO: Add to F24

	keyMap[SDLK_CAPSLOCK] = &inputs[(int)KeyCode::CAPSLOCK];
	keyMap[SDLK_SPACE] = &inputs[(int)KeyCode::SPACE];
	keyMap[SDLK_ESCAPE] = &inputs[(int)KeyCode::ESCAPE];
	keyMap[SDLK_RETURN] = &inputs[(int)KeyCode::RETURN];
	keyMap[SDLK_LCTRL] = &inputs[(int)KeyCode::LEFT_CONTROL];
	keyMap[SDLK_RCTRL] = &inputs[(int)KeyCode::RIGHT_CONTROL];
	keyMap[SDLK_LALT] = &inputs[(int)KeyCode::LEFT_ALT];
	keyMap[SDLK_RALT] = &inputs[(int)KeyCode::RIGHT_ALT];
	keyMap[SDLK_DELETE] = &inputs[(int)KeyCode::DELETE];
	keyMap[SDLK_LSHIFT] = &inputs[(int)KeyCode::LEFT_SHIFT];
	keyMap[SDLK_RSHIFT] = &inputs[(int)KeyCode::RIGHT_SHIFT];

	buttonMap[SDL_CONTROLLER_BUTTON_A] = &inputs[(int)KeyCode::CROSS];
	buttonMap[SDL_CONTROLLER_BUTTON_B] = &inputs[(int)KeyCode::CIRCLE];
	buttonMap[SDL_CONTROLLER_BUTTON_X] = &inputs[(int)KeyCode::SQUARE];
	buttonMap[SDL_CONTROLLER_BUTTON_Y] = &inputs[(int)KeyCode::TRIANGLE];
	buttonMap[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = &inputs[(int)KeyCode::DPAD_LEFT];
	buttonMap[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = &inputs[(int)KeyCode::DPAD_RIGHT];
	buttonMap[SDL_CONTROLLER_BUTTON_DPAD_UP] = &inputs[(int)KeyCode::DPAD_UP];
	buttonMap[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = &inputs[(int)KeyCode::DPAD_DOWN];

	buttonMap[SDL_CONTROLLER_BUTTON_START] = &inputs[(int)KeyCode::START];
	buttonMap[SDL_CONTROLLER_BUTTON_BACK] = &inputs[(int)KeyCode::SELECT];
	//buttonMap[SDL_CONTROLLER_BUTTON_GUIDE] = &inputs[(int)KeyCode::SELECT];
}

SDL_GameController* controller0;

void CrossInputsInit()
{
	controller0 = SDL_GameControllerOpen(0);
	//int numJoy = SDL_NumJoysticks();
}

InputPad CrossGetInputPad()
{
	InputPad pad = InputPad();

	pad.pressedButtons[SDL_CONTROLLER_BUTTON_A] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_A);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_B] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_B);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_X] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_X);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_Y] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_Y);

	pad.pressedButtons[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_DPAD_UP] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_DPAD_UP);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_DPAD_DOWN);

	pad.pressedButtons[SDL_CONTROLLER_BUTTON_BACK] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_BACK);
	pad.pressedButtons[SDL_CONTROLLER_BUTTON_START] = SDL_GameControllerGetButton(controller0, SDL_CONTROLLER_BUTTON_START);

	int16_t rightXValue = SDL_GameControllerGetAxis(controller0, SDL_CONTROLLER_AXIS_RIGHTX);
	int16_t rightYValue = SDL_GameControllerGetAxis(controller0, SDL_CONTROLLER_AXIS_RIGHTY);
	int16_t leftXValue = SDL_GameControllerGetAxis(controller0, SDL_CONTROLLER_AXIS_LEFTX);
	int16_t leftYValue = SDL_GameControllerGetAxis(controller0, SDL_CONTROLLER_AXIS_LEFTY);

	pad.lx = ((leftXValue) / 65536.0f) * 2;
	pad.ly = ((leftYValue) / 65536.0f) * 2;


	pad.rx = ((rightXValue) / 65536.0f) * 2;
	pad.ry = ((rightYValue) / 65536.0f) * 2;

	if (pad.lx < 0.2f && pad.lx > -0.2f)
	{
		pad.lx = 0;
	}
	if (pad.ly < 0.2f && pad.ly > -0.2f)
	{
		pad.ly = 0;
	}
	if (pad.rx < 0.2f && pad.rx > -0.2f)
	{
		pad.rx = 0;
	}
	if (pad.ry < 0.2f && pad.ry > -0.2f)
	{
		pad.ry = 0;
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