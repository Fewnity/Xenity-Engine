#include "game.h"

#include <engine/class_registry/class_registry.h>

//----- Include all your classes here:
// #include "my_class.h"
// #include "scripts/my_other/class.h"

/**
* Register all your classes here:
*/
void Game::Start()
{
	// REGISTER_COMPONENT(MyClass);
	// Disable update function of not needed for better performances
	// REGISTER_COMPONENT(MyOtherClass).DisableUpdateFunction();
}