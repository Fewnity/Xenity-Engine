// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <map>
#include <vector>

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <SDL3/SDL_events.h>
#endif

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/constants.h>

#define PLAYER_1 0
#define PLAYER_2 1
#define PLAYER_3 2
#define PLAYER_4 3
#define PLAYER_5 4
#define PLAYER_6 5
#define PLAYER_7 6
#define PLAYER_8 7

enum class KeyCode
{
	EMPTY = -1,
	RETURN = 0,
	ESCAPE = 1,
	BACKSPACE = 2,
	TAB = 3,
	SPACE = 4,
	EXCLAIM = 5,
	QUOTEDBL = 6,
	HASH = 7,
	PERCENT = 8,
	DOLLAR = 9,
	AMPERSAND = 10,
	QUOTE = 11,
	LEFTPAREN = 12,
	RIGHTPAREN = 13,
	ASTERISK = 14,
	PLUS = 15,
	COMMA = 16,
	MINUS = 17,
	PERIOD = 18,
	SLASH = 19,
	NUM_0 = 20,
	NUM_1 = 21,
	NUM_2 = 22,
	NUM_3 = 23,
	NUM_4 = 24,
	NUM_5 = 25,
	NUM_6 = 26,
	NUM_7 = 27,
	NUM_8 = 28,
	NUM_9 = 29,
	COLON = 30,
	SEMICOLON = 31,
	LESS = 32,
	EQUALS = 33,
	GREATER = 34,
	QUESTION = 35,
	AT = 36,
	LEFTBRACKET = 37,
	BACKSLASH = 38,
	RIGHTBRACKET = 39,
	CARET = 40,
	UNDERSCORE = 41,
	BACKQUOTE = 42,
	A = 43,
	B = 44,
	C = 45,
	D = 46,
	E = 47,
	F = 48,
	G = 49,
	H = 50,
	I = 51,
	J = 52,
	K = 53,
	L = 54,
	M = 55,
	N = 56,
	O = 57,
	P = 58,
	Q = 59,
	R = 60,
	S = 61,
	T = 62,
	U = 63,
	V = 64,
	W = 65,
	X = 66,
	Y = 67,
	Z = 68,

	CAPSLOCK = 69,

	F1 = 70,
	F2 = 71,
	F3 = 72,
	F4 = 73,
	F5 = 74,
	F6 = 75,
	F7 = 76,
	F8 = 77,
	F9 = 78,
	F10 = 79,
	F11 = 80,
	F12 = 81,

	RIGHT = 82,
	LEFT = 83,
	DOWN = 84,
	UP = 85,

	MOUSE_LEFT = 86,
	MOUSE_RIGHT = 87,

	LEFT_CONTROL = 88,
	RIGHT_CONTROL = 89,
	LEFT_ALT = 90,
	RIGHT_ALT = 91,

	CROSS = 92,
	CIRCLE = 93,
	SQUARE = 94,
	TRIANGLE = 95,
	START = 96,
	SELECT = 97,
	LTRIGGER1 = 98,
	RTRIGGER1 = 99,
	DEL = 100,
	MOUSE_MIDDLE = 101,
	LEFT_SHIFT = 102,
	RIGHT_SHIFT = 103,
	DPAD_LEFT = 104,
	DPAD_RIGHT = 105,
	DPAD_UP = 106,
	DPAD_DOWN = 107,
	L_JOYSTICK_CLICK = 108,
	R_JOYSTICK_CLICK = 109,
};

#define INPUT_COUNT 111 // Number = Last Enum + 2

struct Input
{
	KeyCode code = KeyCode::EMPTY;
	bool pressed = false;
	bool released = false;
	bool held = false;
};

struct Touch
{
	Vector2Int position = Vector2Int(0);
	Vector2Int startPosition = Vector2Int(0);
	int fingerId = -1;
	float force = 0;
};

/**
* @brief Class to read inputs (GamePads, Touch Screens, Keyboard, Mouse)
*/
class API InputSystem
{
public:

	/**
	* @brief Return true if the key has just been pressed
	* @param Key code to check
	*/
	[[nodiscard]] static inline bool GetKeyDown(const KeyCode keyCode, const int controllerIndex = 0)
	{
#if defined(EDITOR)
		if (s_blockGameInput)
		{
			return false;
		}
#endif

		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLER)
		{
			return false;
		}

		return s_inputs[controllerIndex][(int)keyCode].pressed;
	}

	/**
	* @brief Return true if the key is held
	* @param Key code to check
	*/
	[[nodiscard]] static inline bool GetKey(const KeyCode keyCode, const int controllerIndex = 0)
	{
#if defined(EDITOR)
		if (s_blockGameInput)
		{
			return false;
		}
#endif

		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLER)
		{
			return false;
		}

		return s_inputs[controllerIndex][(int)keyCode].held;
	}

	/**
	* @brief Return true if the key has just been released
	* @param Key code to check
	*/
	[[nodiscard]] static inline bool GetKeyUp(const KeyCode keyCode, const int controllerIndex = 0)
	{
#if defined(EDITOR)
		if (s_blockGameInput)
		{
			return false;
		}
#endif

		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLER)
		{
			return false;
		}

		return s_inputs[controllerIndex][(int)keyCode].released;
	}

	[[nodiscard]] static Vector2 GetLeftJoystick(const int controllerIndex = 0)
	{
#if defined(EDITOR)
		if (s_blockGameInput)
		{
			return Vector2(0);
		}
#endif

		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLER)
		{
			return Vector2(0);
		}

		return leftJoystick[controllerIndex];
	}

	[[nodiscard]] static Vector2 GetRightJoystick(const int controllerIndex = 0)
	{
#if defined(EDITOR)
		if (s_blockGameInput)
		{
			return Vector2(0);
		}
#endif

		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLER)
		{
			return Vector2(0);
		}

		return rightJoystick[controllerIndex];
	}

	/**
	* @brief Get how many touch screens the device has
	*/
	[[nodiscard]] static int GetTouchScreenCount();

	/**
	* @brief Get how many touch inputs the screen has
	* @param screenIndex Screen index
	*/
	[[nodiscard]] static int GetTouchCount(const int screenIndex);

	/**
	* @brief Get touch data
	* @param touchIndex Touch index
	* @param screenIndex Screen index
	*/
	[[nodiscard]] static Touch GetTouch(const int touchIndex, const int screenIndex);


	/**
	* @brief Hide mouse
	*/
	static void HideMouse();

	/**
	* @brief Show mouse
	*/
	static void ShowMouse();

	static Vector2 mousePosition;
	static Vector2 mouseSpeed;
	static Vector2 mouseSpeedRaw;
	static float mouseWheel;

private:
	static Vector2 leftJoystick[MAX_CONTROLLER];
	static Vector2 rightJoystick[MAX_CONTROLLER];
	friend class Engine;

	struct TouchScreen
	{
		std::vector<Touch> touches;
		std::vector<bool> updated;
	};

	/**
	* @brief [Internal] Init input system
	*/
	static void Init();

	/**
	* @brief [Internal] Read input events
	*/
	static void Read();
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)

	/**
	* @brief [Internal] Read input events
	* @parem event SDL event
	*/
	static void Read(const SDL_Event& event);
#endif


	/**
	* @brief [Internal] Set all keys states to inactive
	*/
	static void ClearInputs();
	static bool s_blockGameInput;

	static void UpdateControllers();

	/**
	* @brief Set inputs state
	*/
	static inline void SetInput(const bool pressed, const KeyCode keyCode, const int controllerIndex)
	{
		if (pressed)
			SetInputPressed(keyCode, controllerIndex);
		else
			SetInputReleased(keyCode, controllerIndex);
	}

	/**
	* @brief Set an input as pressed
	*/
	static inline void SetInputPressed(const KeyCode keyCode, const int controllerIndex)
	{
		if (!s_inputs[controllerIndex][(int)keyCode].held)
		{
			s_inputs[controllerIndex][(int)keyCode].pressed = true;
			s_inputs[controllerIndex][(int)keyCode].held = true;
		}
	}

	/**
	* @brief Set an input as released
	*/
	static inline void SetInputReleased(const KeyCode keyCode, const int controllerIndex)
	{
		s_inputs[controllerIndex][(int)keyCode].released = true;
		s_inputs[controllerIndex][(int)keyCode].held = false;
	}

	/**
	* @brief Set an input states to false
	*/
	static inline void SetInputInactive(const KeyCode keyCode, const int controllerIndex)
	{
		s_inputs[controllerIndex][(int)keyCode].pressed = false;
		s_inputs[controllerIndex][(int)keyCode].released = false;
	}

	static bool s_hidedMouse;
	static Input s_inputs[MAX_CONTROLLER][INPUT_COUNT];
	static std::map<int, Input*> s_keyMap[MAX_CONTROLLER];
	static std::map<int, Input*> s_buttonMap[MAX_CONTROLLER];
	static std::vector<TouchScreen*> screens;
};
