#include "inputs.h"

#if defined(_WIN32) || defined(_WIN64)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include "../../engine/debug/debug.h"
#include <SDL2/SDL_events.h>

void CrossAddInputs(std::map<int, Input*>& keyMap, Input* inputs)
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
}

void CrossInputsInit()
{
}

InputPad CrossGetInputPad()
{
	InputPad pad = InputPad();

	return pad;
}

std::vector<TouchRaw> CrossUpdateTouch()
{
	// Should be empty
	std::vector<TouchRaw> touchesRaw;
	return touchesRaw;
}

#endif