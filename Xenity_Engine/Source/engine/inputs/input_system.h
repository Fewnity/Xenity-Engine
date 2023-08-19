#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <map>

#if defined(_WIN32) || defined(_WIN64)
#include <SDL2/SDL_events.h>
#endif

#include "../vectors/vector2.h"
#include "../vectors/vector2_int.h"
#include <vector>

enum KeyCode
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
	DELETE = 100,
};

#define INPUT_COUNT 102 // Number = Last Enum + 2

class API Input
{
public:
	KeyCode code = EMPTY;
	bool pressed = false;
	bool released = false;
	bool held = false;
};

class API Touch
{
public:
	Vector2Int position = Vector2Int(0);
	Vector2Int startPosition = Vector2Int(0);
	int fingerId = -1;
	float force = 0;
};

class API InputSystem
{
public:
	static void Init();
	static void Read();
#if defined(_WIN32) || defined(_WIN64)
	static void Read(const SDL_Event event);
#endif
	static bool GetKeyDown(const KeyCode keyCode);
	static bool GetKey(const KeyCode keyCode);
	static bool GetKeyUp(const KeyCode keyCode);
	static void ClearInputs();
	static int GetTouchScreenCount();
	static int GetTouchCount(const int screenIndex);
	static Touch GetTouch(const int touchIndex, const int screenIndex);

	static Vector2 leftJoystick;
	static Vector2 rightJoystick;
	static Vector2 mousePosition;
	static Vector2 mouseSpeed;
	static Vector2 mouseSpeedRaw;
	static float mouseWheel;
	static void HideMouse();
	static void ShowMouse();

private:
	class TouchScreen
	{
	public:
		std::vector<Touch> touches;
		std::vector<bool> updated;
	};
	static void ChangeInputState(const bool pressed, const int keyCode);
	static void SetInput(const bool pressed, const int keyCode);
	static void SetInputPressed(const int keyCode);
	static void SetInputReleased(const int keyCode);
	static void SetInputInactive(const int keyCode);

	static bool hidedMouse;
	static Input inputs[INPUT_COUNT];
	static std::map<int, Input *> keyMap;
	static std::vector<TouchScreen *> screens;
};
