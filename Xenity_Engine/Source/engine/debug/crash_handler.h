#pragma once
#include <engine/api.h>

class API CrashHandler
{
public:
	/**
	* Enable crash events
	*/
	static void Init();

	/**
	* Call a function in a try/catch
	*/
	static bool CallInTry(void (*function)());

private:
	/**
	* Called function on error (Not in Editor mode)
	*/
	static void Handler(int signum);
};

