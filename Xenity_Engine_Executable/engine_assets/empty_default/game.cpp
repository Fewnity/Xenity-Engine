#include "game.h"

#include <engine/class_registry/class_registry.h>

//----- Include all your classes here:
// #include "Scripts/my_class.h"
// #include "Scripts/my_other_class.h"

/**
* @brief Register all your classes here
* @brief Only use this function to register your classes!
*/
void Game::Start()
{
	// REGISTER_COMPONENT(MyClass);
	// Disable update function of not needed for better performances
	// REGISTER_COMPONENT(MyOtherClass).DisableUpdateFunction();
}

/**
* Do not add any code after this line.
*/