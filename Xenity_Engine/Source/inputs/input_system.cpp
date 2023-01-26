#include "input_system.h"
#include <fstream>
#include <map>

Vector2 InputSystem::mousePosition = Vector2();
Vector2 InputSystem::mouseSpeed = Vector2();
Input InputSystem::inputs[INPUT_COUNT];

void InputSystem::Init() {
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		inputs[i] = Input();
		inputs[i].code = (KeyCode)i;
	}
}

void InputSystem::ClearInputs() {
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		SetInputInactive(i);
	}
	mouseSpeed.x = 0;
	mouseSpeed.y = 0;
}

void InputSystem::UpdateInputs(SDL_Event event) {
	switch (event.type)
	{
	case SDL_MOUSEMOTION: {
		//Get mouse position
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		mousePosition.x = mouseX;
		mousePosition.y = mouseY;

		//Get mouse speed
		int xSpeed = event.motion.xrel;
		int ySpeed = -event.motion.yrel;
		mouseSpeed.x = xSpeed;
		mouseSpeed.y = ySpeed;
		break;
	}

	case SDL_MOUSEBUTTONDOWN: {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		switch (event.button.button) {
		case SDL_BUTTON_RIGHT:
			break;
		case SDL_BUTTON_LEFT:
			break;
		}
		break;
	}

	case SDL_KEYDOWN:
		InuptUpdate(true, event.key.keysym.sym);
		break;
	
	case SDL_KEYUP:
		InuptUpdate(false, event.key.keysym.sym);
		break;

	}
}

std::map<int, int> keyMap; //TODO : To use later
//To use like that : SetInput(pressed, keyMap.at(keyCode));

void  InputSystem::InuptUpdate(bool pressed, int keyCode) {
	switch (keyCode) {
	case SDLK_LEFT:
		SetInput(pressed, LEFT);
		break;
	case SDLK_RIGHT:
		SetInput(pressed, RIGHT);
		break;
	case SDLK_UP:
		SetInput(pressed, UP);
		break;
	case SDLK_DOWN:
		SetInput(pressed, DOWN);
		break;
	case SDLK_a:
		SetInput(pressed, A);
		break;
	case SDLK_b:
		SetInput(pressed, B);
		break;
	case SDLK_c:
		SetInput(pressed, C);
		break;
	case SDLK_d:
		SetInput(pressed, D);
		break;
	case SDLK_e:
		SetInput(pressed, E);
		break;
	case SDLK_f:
		SetInput(pressed, F);
		break;
	case SDLK_g:
		SetInput(pressed, G);
		break;
	case SDLK_h:
		SetInput(pressed, H);
		break;
	case SDLK_i:
		SetInput(pressed, I);
		break;
	case SDLK_j:
		SetInput(pressed, J);
		break;
	case SDLK_k:
		SetInput(pressed, K);
		break;
	case SDLK_l:
		SetInput(pressed, L);
		break;
	case SDLK_m:
		SetInput(pressed, M);
		break;
	case SDLK_n:
		SetInput(pressed, N);
		break;
	case SDLK_o:
		SetInput(pressed, O);
		break;
	case SDLK_p:
		SetInput(pressed, P);
		break;
	case SDLK_q:
		SetInput(pressed, Q);
		break;
	case SDLK_r:
		SetInput(pressed, R);
		break;
	case SDLK_s:
		SetInput(pressed, S);
		break;
	case SDLK_t:
		SetInput(pressed, T);
		break;
	case SDLK_u:
		SetInput(pressed, U);
		break;
	case SDLK_v:
		SetInput(pressed, V);
		break;
	case SDLK_w:
		SetInput(pressed, W);
		break;
	case SDLK_x:
		SetInput(pressed, X);
		break;
	case SDLK_y:
		SetInput(pressed, Y);
		break;
	case SDLK_z:
		SetInput(pressed, Z);
		break;

	case SDLK_ESCAPE:
		SetInput(pressed, ESCAPE);
		break;
	}
}

void InputSystem::SetInput(bool pressed, int keyCode) {
	if (pressed)
		SetInputPressed(keyCode);
	else
		SetInputReleased(keyCode);
}

void InputSystem::SetInputPressed(int keyCode) {
	if (!inputs[keyCode].held) {
		inputs[keyCode].pressed = true;
		inputs[keyCode].held = true;
	}
}

void InputSystem::SetInputReleased(int keyCode) {
	inputs[keyCode].released = true;
	inputs[keyCode].held = false;
}

void InputSystem::SetInputInactive(int keyCode) {
	inputs[keyCode].pressed = false;
	inputs[keyCode].released = false;
}

bool InputSystem::GetKeyDown(KeyCode keyCode) {
	return inputs[keyCode].pressed;
}

bool InputSystem::GetKey(KeyCode keyCode) {
	return inputs[keyCode].held;
}

bool InputSystem::GetKeyUp(KeyCode keyCode) {
	return inputs[keyCode].released;
}