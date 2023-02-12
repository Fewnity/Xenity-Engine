#include "engine.h"
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

#include "game.h"
#include "debug.h"

#undef main

using namespace std;

/*
 * TODO :
 * Update local position when updating position, rotation and scale
 * Update position when parent is set to nullptr
 * Clean code
 * Text renderer component
 * Fix light rotation
 * 3D Sounds
 * Fix transparency order
 * Check mesh/shader destructor
 * Add scale usage in local position calculation
 * Add local scale usage
 */

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

bool wireframe = false;

int main(void)
{
	if (Engine::Init() != 0) {
		Debug::Print("Engine failed to init");
		return -1;
	}

	Debug::Print("Initiating game...");
	Game::Init();
	Debug::Print("---- Game initiated ----");
	Engine::Loop();
	Debug::Print("---- Game loop ended ----");

	glfwTerminate();
	return 0;
}