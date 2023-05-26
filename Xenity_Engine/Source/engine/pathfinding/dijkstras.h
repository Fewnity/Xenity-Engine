#pragma once
#include <vector>
#include "../game_elements/gameobject.h"
#include "../vectors/vector2.h"


class Dijkstras
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

