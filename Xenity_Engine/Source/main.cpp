#include "main.h"
#include <SDL2/SDL.h>
#include <iostream>

#include "xenity.h"

#undef main

using namespace std;

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


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
 * Check error in AStar
 * Optimise component update system
 * Fix children local scale values
 * fix set parent function
 * Create Engine stop function
 * Check shader destructor : check if delete is needed if a shader can't compile
 * Performance problem : if a material "A" is used, then a material with the same shader is used, then go back to the material "A", the values are reupdated
 * Improvement : When a component is added/remove from a gameobject, just modify the ordered component list in Engine instead of clearing the list (create a function like OnComponentAddedOnGameObject)
 */

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char* argv[])
{
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