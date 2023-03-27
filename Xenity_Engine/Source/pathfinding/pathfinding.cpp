#include "pathfinding.h"
#include "../audio/audio_clip.h"
#include "../gameobject.h"
#include "../graphics/graphics.h"
#include "../graphics/mesh.h"
#include "../graphics/material.h"

#include <SDL2/SDL.h>
#include "../scene_manager/SceneManager.h"
#include "../game/sceneGame1.h"
#include "../lighting/lighting.h"
#include "../inputs/input_system.h"
#include "../engine_settings.h"
#include "../vectors/vector3.h"
#include <chrono>
#include <iostream>
#include "../graphics/spriteRenderer.h"
#include "../asset_manager.h"
#include "../debug/debug.h"
#include "../graphics/text_renderer.h"
#include "../ui/ui_manager.h"
#include "../tools/shape_spawner.h"
#include "../tools/curve.h"
#include "../tools/math.h"
#include "../tools/benchmark.h"
#include "../time/time.h"
#include "../ui/window.h"

/// <summary>
/// Init game
/// </summary>
void PathFinding::Init()
{
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");

	LoadGameData();

	int infinite = -1;

	int squareSize = 4;
	nodes = new int* [squareSize] {
		new int[squareSize] {0, 2, infinite, 12},//A
		new int[squareSize] {infinite, 0, 10, infinite},//B
		new int[squareSize] {infinite, infinite, infinite, infinite},//C
		new int[squareSize] {infinite, infinite, 8, infinite}//D
	};

	int dest = 2;
	int source = 0;

	int * dist = new int [squareSize];
	int* prev = new int[squareSize];

	//For each vertex

	for (int i = 0; i < squareSize; i++)
	{
		dist[i] = -1;
		prev[i] = -1;
	}
	dist[source] = 0;
}

void PathFinding::LoadGameData()
{
	cameraGameObject->AddExistingComponent(camera);
	camera->gameObject->transform.SetPosition(Vector3(0, 0, -10));

	camera->SetProjectionType(Orthographic);
	camera->SetProjectionSize(3);
}


/// <summary>
/// Game loop
/// </summary>
void PathFinding::Loop()
{
}