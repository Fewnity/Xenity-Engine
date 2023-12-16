#pragma once
#include <engine/api.h>

class API CrashHandler
{
public:
	static void Init();
private:
	static void Handler(int signum);
};

