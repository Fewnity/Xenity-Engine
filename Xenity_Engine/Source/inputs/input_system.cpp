#include "input_system.h"
#include <fstream>
#include <map>
#include "../debug/debug.h"
#include "../ui/window.h"

Vector2 InputSystem::mousePosition = Vector2();
Vector2 InputSystem::mouseSpeed = Vector2();
Vector2 InputSystem::mouseSpeedRaw = Vector2();
Input InputSystem::inputs[INPUT_COUNT];
float InputSystem::mouseWheel = 0;
bool InputSystem::hideMouse = false;


/// <summary>
/// Init input system
/// </summary>
void InputSystem::Init() 
{
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		inputs[i] = Input();
		inputs[i].code = (KeyCode)i;
	}
	Debug::Print("---- Input System initiated ----");
}


/// <summary>
/// Get inputs events
/// </summary>
/// <param name="event"></param>
void InputSystem::Read(const SDL_Event event)
{
	switch (event.type)
	{
	case SDL_MOUSEMOTION: {
		//Get mouse position
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		mousePosition.x = mouseX;
		mousePosition.y = mouseY;

		float aspect = Window::GetAspectRatio();

		//Get mouse speed
		float xSpeed = event.motion.xrel / (float)Window::GetWidth() * aspect;
		float ySpeed = -event.motion.yrel / (float)Window::GetHeight();
		float xSpeedRaw = event.motion.xrel;
		float ySpeedRaw = -event.motion.yrel;

		mouseSpeed.x = xSpeed;
		mouseSpeed.y = ySpeed;

		mouseSpeedRaw.x = xSpeedRaw;
		mouseSpeedRaw.y = ySpeedRaw;
		break;
	}

	case SDL_MOUSEBUTTONDOWN: {
		if(hideMouse)
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
		ChangeInputState(true, event.key.keysym.sym);
		break;
	
	case SDL_KEYUP:
		ChangeInputState(false, event.key.keysym.sym);
		break;

	case SDL_MOUSEWHEEL:
		mouseWheel = event.wheel.preciseY;
		break;
	}
}

std::map<int, int> keyMap; //TODO : To use later
//To use like that : SetInput(pressed, keyMap.at(keyCode));

void InputSystem::ChangeInputState(const bool pressed, const int keyCode)
{
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

	case SDLK_0:
		SetInput(pressed, NUM_0);
		break;
	case SDLK_1:
		SetInput(pressed, NUM_1);
		break;
	case SDLK_2:
		SetInput(pressed, NUM_2);
		break;
	case SDLK_3:
		SetInput(pressed, NUM_3);
		break;
	case SDLK_4:
		SetInput(pressed, NUM_4);
		break;
	case SDLK_5:
		SetInput(pressed, NUM_5);
		break;
	case SDLK_6:
		SetInput(pressed, NUM_6);
		break;
	case SDLK_7:
		SetInput(pressed, NUM_7);
		break;
	case SDLK_8:
		SetInput(pressed, NUM_8);
		break;
	case SDLK_9:
		SetInput(pressed, NUM_9);
		break;

	case SDLK_F1:
		SetInput(pressed, F1);
		break;
	case SDLK_F2:
		SetInput(pressed, F2);
		break;
	case SDLK_F3:
		SetInput(pressed, F3);
		break;
	case SDLK_F4:
		SetInput(pressed, F4);
		break;
	case SDLK_F5:
		SetInput(pressed, F5);
		break;
	case SDLK_F6:
		SetInput(pressed, F6);
		break;
	case SDLK_F7:
		SetInput(pressed, F7);
		break;
	case SDLK_F8:
		SetInput(pressed, F8);
		break;
	case SDLK_F9:
		SetInput(pressed, F9);
		break;
	case SDLK_F10:
		SetInput(pressed, F10);
		break;
	case SDLK_F11:
		SetInput(pressed, F11);
		break;
	case SDLK_F12:
		SetInput(pressed, F12);
		break;

	case SDLK_CAPSLOCK:
		SetInput(pressed, CAPSLOCK);
		break;

	case SDLK_SPACE:
		SetInput(pressed, SPACE);
		break;

	case SDLK_ESCAPE:
		SetInput(pressed, ESCAPE);
		break;
	}
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
	if (!inputs[keyCode].held) {
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
	return inputs[keyCode].pressed;
}

/// <summary>
/// Return if the key is hold
/// </summary>
/// <param name="keyCode"></param>
/// <returns></returns>
bool InputSystem::GetKey(const KeyCode keyCode)
{
	return inputs[keyCode].held;
}

/// <summary>
/// Return if the key has just been released
/// </summary>
/// <param name="keyCode"></param>
/// <returns></returns>
bool InputSystem::GetKeyUp(const KeyCode keyCode)
{
	return inputs[keyCode].released;
}

#pragma endregion