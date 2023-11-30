#include "game.h"
#include <engine/class_registry/class_registry.h>

//----- Include all your classes here:
//#include "my_class.h"

/**
* Register all your classes here:
*/
void Game::Start()
{
	//ClassRegistry::AddComponentClass<MyClass>("MyClass");
}

GameInterface *CreateGame()
{
	return new Game();
}