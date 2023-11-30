#if defined(EXPORT)
// #define API __declspec(dllexport)
#define API2
#elif defined(IMPORT)
#define API2 __declspec(dllexport)
#else
#define API2
#endif

#pragma once

#include <memory>
#include <engine/game_interface.h>

class Game : public GameInterface
{
public:
	void Start() override;
};

extern "C"
{
	API2 GameInterface *CreateGame();
};