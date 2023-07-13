#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include <vector>
#include "../game_elements/gameobject.h"
#include "../vectors/vector2.h"


class API Dijkstras
{
public:
	Dijkstras() = delete;
	Dijkstras(int size);

	int squareSize = 4;

	int* dist = nullptr;
	int* path = nullptr;
	int* usedPath = nullptr;
	int pathCount = 0;
	int** nodes = nullptr;
	void processDijkstras(int source, int destination);
	void printParentPath(int parent[], int i, std::string& str);
	void fillPath(int parent[], int path[], int i, int pathIndex);
	int GetMin(int dist[], bool visited[]);

private:
};

