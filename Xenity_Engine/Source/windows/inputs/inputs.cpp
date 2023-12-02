#include "inputs.h"

#if defined(_WIN32) || defined(_WIN64)

#include "../../engine/inputs/input_system.h"
#include "../../engine/inputs/input_touch_raw.h"
#include "../../engine/debug/debug.h"
#include <SDL2/SDL_events.h>

void CrossAddInputs(std::map<int, Input*>& keyMap, Input* inputs)
{
	keyMap[SDLK_LEFT] = &inputs[LEFT];
	keyMap[SDLK_RIGHT] = &inputs[RIGHT];
	keyMap[SDLK_UP] = &inputs[UP];
	keyMap[SDLK_DOWN] = &inputs[DOWN];

	keyMap[SDLK_a] = &inputs[A];
	keyMap[SDLK_b] = &inputs[B];
	keyMap[SDLK_c] = &inputs[C];
	keyMap[SDLK_d] = &inputs[D];
	keyMap[SDLK_e] = &inputs[E];
	keyMap[SDLK_f] = &inputs[F];
	keyMap[SDLK_g] = &inputs[G];
	keyMap[SDLK_h] = &inputs[H];
	keyMap[SDLK_i] = &inputs[I];
	keyMap[SDLK_j] = &inputs[J];
	keyMap[SDLK_k] = &inputs[K];
	keyMap[SDLK_l] = &inputs[L];
	keyMap[SDLK_m] = &inputs[M];
	keyMap[SDLK_n] = &inputs[N];
	keyMap[SDLK_o] = &inputs[O];
	keyMap[SDLK_p] = &inputs[P];
	keyMap[SDLK_q] = &inputs[Q];
	keyMap[SDLK_r] = &inputs[R];
	keyMap[SDLK_s] = &inputs[S];
	keyMap[SDLK_t] = &inputs[T];
	keyMap[SDLK_u] = &inputs[U];
	keyMap[SDLK_v] = &inputs[V];
	keyMap[SDLK_w] = &inputs[W];
	keyMap[SDLK_x] = &inputs[X];
	keyMap[SDLK_y] = &inputs[Y];
	keyMap[SDLK_z] = &inputs[Z];

	keyMap[SDLK_0] = &inputs[NUM_0];
	keyMap[SDLK_1] = &inputs[NUM_1];
	keyMap[SDLK_2] = &inputs[NUM_2];
	keyMap[SDLK_3] = &inputs[NUM_3];
	keyMap[SDLK_4] = &inputs[NUM_4];
	keyMap[SDLK_5] = &inputs[NUM_5];
	keyMap[SDLK_6] = &inputs[NUM_6];
	keyMap[SDLK_7] = &inputs[NUM_7];
	keyMap[SDLK_8] = &inputs[NUM_8];
	keyMap[SDLK_9] = &inputs[NUM_9];

	keyMap[SDLK_F1] = &inputs[F1];
	keyMap[SDLK_F2] = &inputs[F2];
	keyMap[SDLK_F3] = &inputs[F3];
	keyMap[SDLK_F4] = &inputs[F4];
	keyMap[SDLK_F5] = &inputs[F5];
	keyMap[SDLK_F6] = &inputs[F6];
	keyMap[SDLK_F7] = &inputs[F7];
	keyMap[SDLK_F8] = &inputs[F8];
	keyMap[SDLK_F9] = &inputs[F9];
	keyMap[SDLK_F10] = &inputs[F10];
	keyMap[SDLK_F11] = &inputs[F11];
	keyMap[SDLK_F12] = &inputs[F12];
	// TODO: Add to F24

	keyMap[SDLK_CAPSLOCK] = &inputs[CAPSLOCK];
	keyMap[SDLK_SPACE] = &inputs[SPACE];
	keyMap[SDLK_ESCAPE] = &inputs[ESCAPE];
	keyMap[SDLK_RETURN] = &inputs[RETURN];
	keyMap[SDLK_LCTRL] = &inputs[LEFT_CONTROL];
	keyMap[SDLK_RCTRL] = &inputs[RIGHT_CONTROL];
	keyMap[SDLK_LALT] = &inputs[LEFT_ALT];
	keyMap[SDLK_RALT] = &inputs[RIGHT_ALT];
	keyMap[SDLK_DELETE] = &inputs[DELETE];
	keyMap[SDLK_LSHIFT] = &inputs[LEFT_SHIFT];
	keyMap[SDLK_RSHIFT] = &inputs[RIGHT_SHIFT];
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