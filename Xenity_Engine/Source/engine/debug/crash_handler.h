#pragma once
#include <engine/api.h>
//#include <functional>

class API CrashHandler
{
public:
	static void Init();
	static void CallInTry(void (*function)());
private:
	static void Handler(int signum);
};

