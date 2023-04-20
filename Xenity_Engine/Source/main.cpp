#include "main.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include <iostream>

#include "gameobject.h"
#include "ui/window.h"

#include "ui/ui_manager.h"
#include "file_system/file.h"
#include "inputs/input_system.h"
#include "scene_manager/scene.h"
#include "engine.h"

#include "debug/debug.h"

#undef main

using namespace std;

/*
 * TODO :
 * Create on move event
 * 3D text
 * 3D Sounds
 * Disable light when component is disabled
 * Fix transparency order
 * Check mesh/shader destructor
 * Fix tilemap rotation
 * Clean sprite manager class
 * Create color class
 * Optimise component update system
 * Link IDrawable and Component for IsEnabled
 * Check shader destructor : check if delete is needed if a shader can't compile
 * Performance problem : if a material "A" is used, then a material with the same shader is used, then go back to the material "A", the values are reupdated
 * Improvement : When a component is added/remove from a gameobject, just modify the ordered component list in Engine instead of clearing the list (create a function like OnComponentAddedOnGameObject)
 */

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//int main(void)
int main(int argc, char* argv[])
{
	//std::cout << "Main function : GameObject count " << Engine::gameObjects.size() << std::endl;
	std::string exePath = argv[0];
	if (Engine::Init(exePath) != 0)
	{
		Debug::Print("Engine failed to init");
		return -1;
	}

	Engine::Loop();
	Debug::Print("---- Game loop ended ----");

	glfwTerminate();
	return 0;
}