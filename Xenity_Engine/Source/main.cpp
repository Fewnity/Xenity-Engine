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

#undef main

using namespace std;

/*
 * TODO :
 * Sounds
 * Fix normals when models doesn't have normals
 * Fix transparency order
 * Better engine structure (lighting)
 * Better structure
 * Check mesh/shader destructor
 * Add scale usage in local position calculation
 * Add local scale usage
 * Update position when updating local position
 * Update local position when updating position
 */

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

bool wireframe = false;

int main(void)
{
	if (Engine::Init() != 0) {
		return -1;
	}
	Game::Init();
	Engine::Loop();

	glfwTerminate();
	return 0;
}