#include "astar.h"
#include <corecrt_malloc.h>
#include <corecrt_math.h>
#include <iostream>

Astar::Astar(int xGridSize, int yGridSize)
{
	this->xGridSize = xGridSize;
	this->yGridSize = yGridSize;
	grid = (Tile*)malloc(xGridSize * yGridSize * sizeof(Tile));

	ResetGrid(true);
}

void Astar::ResetGrid(bool clearObstacles)
{
	for (int x = 0; x < xGridSize; x++)
	{
		for (int y = 0; y < yGridSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->g = INT32_MAX;
			tile->h = 0;
			tile->f = 0;
			tile->closed = false;
			if (clearObstacles)
				tile->isObstacle = false;
			tile->isPath = false;
			tile->previousTile = nullptr;
		}
	}
}

Astar::Tile* Astar::GetTile(int x, int y)
{
	if (grid == nullptr || x < 0 || y < 0 || x >= xGridSize || y >= yGridSize)
		return nullptr;

	return &grid[x * yGridSize + y];
}

void Astar::SetTileIsObstacle(int x, int y, bool isObstacle)
{
	if (grid == nullptr || x < 0 || y < 0 || x >= xGridSize || y >= yGridSize)
		return;

	grid[x * yGridSize + y].isObstacle = isObstacle;
}

void Astar::GetLowestFTile()
{
	int lowestF = INT32_MAX;
	int lowestH = INT32_MAX;
	Tile* lowestTile = nullptr;
	Vector2 lowestTilePosition;

	for (int x = 0; x < xGridSize; x++)
	{
		for (int y = 0; y < yGridSize; y++)
		{
			Tile* tile = GetTile(x, y);
			if (tile == currentTile || tile == nullptr || tile->closed || tile->isObstacle || tile->f == 0)
				continue;

			if (lowestF > tile->f)
			{
				lowestF = tile->f;
				lowestTile = tile;
				lowestTilePosition = Vector2(x, y);
			}
			else if (lowestF == tile->f)
			{
				if (lowestH > tile->h)
				{
					lowestH = tile->h;
					lowestTile = tile;
					lowestTilePosition = Vector2(x, y);
				}
			}
		}
	}
	if (lowestTile != nullptr) {
		lowestTile->closed = true;
		currentTile = lowestTile;
		currentTilePosition = lowestTilePosition;
	}
	else {
		cantAccess = true;
	}
}

void Astar::ProcessOneStep()
{
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			Tile* tile = GetTile(x + currentTilePosition.x, y + currentTilePosition.y);
			//if (tile == currentTile || tile == nullptr || tile->closed || tile->isObstacle || tile->g != INT32_MAX)
			if (tile == currentTile || tile == nullptr || tile->closed || tile->isObstacle)
				continue;

			int newG;
			if (abs(x) == 1 && abs(y) == 1) 
			{
				if (!canPassCorners) 
				{
					Tile* neighbor1 = GetTile(currentTilePosition.x, y + currentTilePosition.y);
					Tile* neighbor2 = GetTile(x + currentTilePosition.x, currentTilePosition.y);
					if (neighbor1->isObstacle && neighbor2->isObstacle) 
					{
						continue;
					}
				}
				newG = currentTile->g + 14;
			}
			else
				newG = currentTile->g + 10;

			if (lowAccuracy || newG < tile->g)
			{
				tile->previousTile = currentTile;
				tile->g = newG;
				tile->h = abs((int)currentTilePosition.x + x - (int)endPos.x) * 10 + abs((int)currentTilePosition.y + y - (int)endPos.y) * 10;
				tile->f = tile->g + tile->h;
			}
		}
	}
	GetLowestFTile();
	SetFinalPath();
}

void Astar::SetFinalPath() 
{
	for (int x = 0; x < xGridSize; x++)
	{
		for (int y = 0; y < yGridSize; y++)
		{
			Tile* t = GetTile(x, y);
			t->isPath = false;
		}
	}
	Tile* nextTile = currentTile;

	while (nextTile != nullptr)
	{
		nextTile->isPath = true;
		nextTile = nextTile->previousTile;
	}
}

void Astar::SetDestination(Vector2 start, Vector2 end)
{
	ResetGrid(false);

	cantAccess = false;
	startPos = start;
	endPos = end;

	Tile* startTile = GetTile(start.x, start.y);
	endTile = GetTile(end.x, end.y);

	currentTile = startTile;
	currentTile->closed = true;
	currentTile->g = 0;
	currentTilePosition = start;
}

void Astar::Process()
{
	while (!endTile->closed && !cantAccess)
	{
		ProcessOneStep();
	}
}