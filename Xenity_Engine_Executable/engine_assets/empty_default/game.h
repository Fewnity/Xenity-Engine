// DO NOT MODIFY THIS FILE

#if defined(EXPORT)
#define API2
#elif defined(IMPORT)
#define API2 __declspec(dllexport)
#else
#define API2
#endif

#pragma once

#include <engine/game_interface.h>

class Game : public GameInterface
{
public:
	void Start() override;
};

extern "C"
{
	inline API2 GameInterface* CreateGame()
	{
		return new Game();
	}
};