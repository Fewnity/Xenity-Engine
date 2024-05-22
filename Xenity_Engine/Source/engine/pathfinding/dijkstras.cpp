// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "dijkstras.h"

int infinite = 99999999;

int Dijkstras::GetMin(int dist[], bool visited[])
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

void Dijkstras::fillPath(int parent[], int path[], int i, int pathIndex)
{
	if (parent[i] == -1)
	{
		return;
	}

	pathIndex++;
	fillPath(parent, path, parent[i], pathIndex);
	path[pathIndex - 1] = i;
}

void Dijkstras::printParentPath(int parent[], int i, std::string& str)
{
	if (parent[i] == -1)
	{
		return;
	}

	printParentPath(parent, parent[i], str);
	str += std::to_string(i) + " ";
	//std::cout << i << " ";
}

Dijkstras::Dijkstras(int size)
{
	squareSize = size;
	dist = new int[squareSize];
	path = new int[squareSize];
	usedPath = new int[squareSize];
}

void Dijkstras::processDijkstras(int source, int destination)
{
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

	for (int i = 0; i < 4; i++)
	{
		usedPath[i] = -1;
	}
	fillPath(path, usedPath, 2, 0);

	for (int i = 0; i < 4; i++)
	{
		if (usedPath[i] != -1) {
			pathCount++;
		}
		else {
			usedPath[i] = source;
			pathCount++;
			break;
		}
	}
	int swapTemp = 0;
	for (int i = 0; i < pathCount / 2; i++)
	{
		swapTemp = usedPath[i];
		usedPath[i] = usedPath[(pathCount - 1) - i];
		usedPath[(pathCount - 1) - i] = swapTemp;
	}
}