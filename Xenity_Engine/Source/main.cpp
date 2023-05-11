#include "main.h"
#include <iostream>

#include "xenity.h"

using namespace std;


/*
 * TODO :
 * IMPORTANT : Fix text cull when a sprite has a inverted scale (reset opengl value before drawing
 * SetBufferSize and SetBufferSubData change size system (get vertices size and not byteCount)
 * Optimise GetLocalPositionFromMatrices with references
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
 * Check crash if there is no active camera
 * Create Engine stop function
 * Check if sprite facing is correct while batching with negative sprite scales
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
	Engine::Stop();
	return 0;
}