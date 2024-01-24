#pragma once
#include <engine/api.h>

#include <map>

#if defined(_WIN32) || defined(_WIN64)
#include <SDL2/SDL_events.h>
#endif

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <vector>

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
	DELETE = 100,
	MOUSE_MIDDLE = 101,
	LEFT_SHIFT = 102,
	RIGHT_SHIFT = 103
};

#define INPUT_COUNT 105 // Number = Last Enum + 2

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

class API InputSystem
{
public:

	/**
	* [Internal] Init input system
	*/
	static void Init();

	/**
	* [Internal] Read input events
	*/
	static void Read();
#if defined(_WIN32) || defined(_WIN64)

	/**
	* [Internal] Read input events
	* @parem event SDL event
	*/
	static void Read(const SDL_Event& event);
#endif

	/**
	* Return true if the key has just been pressed
	* @param Key code to check
	*/
	static bool GetKeyDown(const KeyCode keyCode);

	/**
	* Return true if the key is held
	* @param Key code to check
	*/
	static bool GetKey(const KeyCode keyCode);

	/**
	* Return true if the key has just been released
	* @param Key code to check
	*/
	static bool GetKeyUp(const KeyCode keyCode);

	/**
	* [Internal] Set all keys states to inactive
	*/
	static void ClearInputs();

	/**
	* Get how many touch screens the device has
	*/
	static int GetTouchScreenCount();

	/**
	* Get how many touch inputs the screen has
	* @param screenIndex Screen index
	*/
	static int GetTouchCount(const int screenIndex);

	/**
	* Get touch data
	* @param touchIndex Touch index
	* @param screenIndex Screen index
	*/
	static Touch GetTouch(const int touchIndex, const int screenIndex);

	/**
	* Hide mouse
	*/
	static void HideMouse();

	/**
	* Show mouse
	*/
	static void ShowMouse();

	static Vector2 leftJoystick;
	static Vector2 rightJoystick;
	static Vector2 mousePosition;
	static Vector2 mouseSpeed;
	static Vector2 mouseSpeedRaw;
	static float mouseWheel;
	static bool blockGameInput;

private:
	struct TouchScreen
	{
		std::vector<Touch> touches;
		std::vector<bool> updated;
	};

	/**
	* Set inputs state
	*/
	static void SetInput(const bool pressed, const KeyCode keyCode);

	/**
	* Set an input as pressed
	*/
	static void SetInputPressed(const KeyCode keyCode);

	/**
	* Set an input as released
	*/
	static void SetInputReleased(const KeyCode keyCode);

	/**
	* Set an input states to false
	*/
	static void SetInputInactive(const KeyCode keyCode);

	static bool hidedMouse;
	static Input inputs[INPUT_COUNT];
	static std::map<int, Input*> keyMap;
	static std::vector<TouchScreen*> screens;
};
