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

int squareSize = 4;
int infinite = 99999999;
int* dist = new int[squareSize];
int* path = new int[squareSize];

int source = 0;
int dest = 2;

int GetMin(int dist[], bool visited[])
{
	int minDis = infinite;
	int vertexIndex = 0;

	for (int i = 0; i < squareSize; i++)
	{
		if (!visited[i] && minDis >= dist[i]) {
			minDis = dist[i];
			vertexIndex = i;
		}
	}
	return vertexIndex;
}

void printParentPath(int parent[], int i, std::string& str)
{
	if (parent[i] == -1)
	{
		return;
	}

	printParentPath(parent, parent[i], str);
	str += std::to_string(i) + " ";
	//std::cout << i << " ";
}

void processDijkstras(int source, int destination, int** nodes, int* dist, int* path)
{
	//int* dist = new int[squareSize];
	//int* prev = new int[squareSize];
	bool* explored = new bool[squareSize];

	//For each vertex
	for (int i = 0; i < squareSize; i++)
	{
		dist[i] = infinite;
		path[i] = -1;
		explored[i] = false;
	}
	dist[source] = 0;

	for (int i = 0; i < squareSize - 1; i++)
	{
		int minDistIndex = GetMin(dist, explored);
		explored[minDistIndex] = true;

		for (int j = 0; j < squareSize; j++)
		{
			if (!explored[j] && dist[minDistIndex] != infinite && nodes[minDistIndex][j] && dist[minDistIndex] + nodes[minDistIndex][j] < dist[j])
			{
				dist[j] = dist[minDistIndex] + nodes[minDistIndex][j];
				path[j] = minDistIndex;
			}
		}
	}
}

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

	int block = 0;

	nodes = new int* [squareSize] {
		new int[squareSize] {0, 2, block, 12},//A
			new int[squareSize] {block, 0, 10, 5},//B
			new int[squareSize] {block, block, block, block},//C
			new int[squareSize] {12, block, 8, block}//D
	};

	processDijkstras(source, dest, nodes, dist, path);

	/*std::cout << "Shortest Paths from " << source << ": " << std::endl;
	for (int i = 0; i < squareSize; i++)
	{
		std::cout << "End point: " << i << ", distance: " << dist[i] << ", path: " << source << " ";
		printParentPath(path, i);
		std::cout << std::endl;
	}
	std::cout << std::endl;*/
	
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
	std::string finalPath = "";
	finalPath += "Shortest Paths from " + std::to_string(source) + ": \n";

	for (int i = 0; i < squareSize; i++)
	{
		finalPath += "End point: " + std::to_string(i) + ", distance: " + std::to_string(dist[i]) + ", path: " + std::to_string(source) + " ";
		//std::cout << "End point: " << i << ", distance: " << dist[i] << ", path: " << source << " ";
		printParentPath(path, i, finalPath);
		finalPath += "\n";
		//std::cout << std::endl;
	}
	//std::cout << std::endl;
	UiManager::RenderTextCanvas(finalPath, 0.5, 0.5, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Center, V_Center, *AssetManager::GetShader(7));
}