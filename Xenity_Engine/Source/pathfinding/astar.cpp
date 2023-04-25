#include "astar.h"
#include <corecrt_malloc.h>
#include <corecrt_math.h>
#include <iostream>

Astar::Astar(int xGridSize, int yGridSize)
{
	this->xGridSize = xGridSize;
	this->yGridSize = yGridSize;
	grid = (Tile*)malloc(xGridSize * yGridSize * sizeof(Tile));

	ResetGrid();
}

void Astar::ResetGrid()
{
	ResetGrid(true);
}

void Astar::ResetGrid(bool clearObstacles)
{
	for (int x = 0; x < xGridSize; x++)
	{
		for (int y = 0; y < yGridSize; y++)
		{
			Tile* tile = GetTileFast(x, y);
			tile->g = INT32_MAX;
			tile->h = 0;
			tile->f = 0;
			tile->x = x;
			tile->y = y;
			tile->closed = false;
			tile->inList = false;
			if (clearObstacles)
				tile->isObstacle = false;
			tile->isPath = false;
			tile->previousTile = nullptr;
		}
	}
	nextTilesToCheck.clear();
	cantAccess = false;
}

Astar::Tile* Astar::GetTile(int x, int y)
{
	if (grid == nullptr || x < 0 || y < 0 || x >= xGridSize || y >= yGridSize)
		return nullptr;

	return &grid[x * yGridSize + y];
}

Astar::Tile* Astar::GetTileFast(int x, int y)
{
	return &grid[x * yGridSize + y];
}

Astar::Tile* Astar::GetTileUltraFast(int& row, int& col)
{
	return &grid[row + col];
}

void Astar::SetTileIsObstacle(int x, int y, bool isObstacle)
{
	Tile* tile = GetTile(x, y);

	if (tile)
		tile->isObstacle = isObstacle;
}

void Astar::GetLowestFTile()
{
	int nextTileCount = nextTilesToCheck.size();
	if (nextTileCount != 0) {

		Tile* lowestTile = nextTilesToCheck[0];
		int index = 0;

		for (int i = 1; i < nextTileCount; i++)
		{
			Tile* tile = nextTilesToCheck[i];

			if (lowestTile->f > tile->f)
			{
				lowestTile = tile;
				index = i;
			}
			else if (lowestTile->f == tile->f)
			{
				if (lowestTile->h > tile->h)
				{
					lowestTile = tile;
					index = i;
				}
			}
		}

		nextTilesToCheck.erase(nextTilesToCheck.begin() + index);
		lowestTile->closed = true;
		currentTile = lowestTile;
	}
	else
	{
		cantAccess = true;
	}
}

void Astar::ProcessOneStep()
{
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			Tile* tile = GetTile(x + currentTile->x, y + currentTile->y);
			if (tile == currentTile || tile == nullptr || tile->closed || tile->isObstacle)
				continue;

			int newG;
			if (abs(x) == 1 && abs(y) == 1)
			{
				if (!canPassCorners)
				{
					Tile* neighbor1 = GetTile(currentTile->x, y + currentTile->y);
					Tile* neighbor2 = GetTile(x + currentTile->x, currentTile->y);
					if (neighbor1->isObstacle || neighbor2->isObstacle)
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
				tile->h = abs(currentTile->x + x - (int)endPos.x) * 10 + abs(currentTile->y + y - (int)endPos.y) * 10;
				tile->f = tile->g + tile->h;

				if (!tile->inList)
				{
					tile->inList = true;
					nextTilesToCheck.push_back(tile);
				}
			}
		}
	}
	GetLowestFTile();
	//SetFinalPath();
}

void Astar::SetFinalPath()
{
	for (int x = 0; x < xGridSize; x++)
	{
		for (int y = 0; y < yGridSize; y++)
		{
			Tile* t = GetTileFast(x, y);
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

/// <summary>
/// Get found path
/// </summary>
/// <returns></returns>
std::vector<Vector2> Astar::GetPath()
{
	std::vector<Vector2> path;

	Tile* nextTile = currentTile;

	while (nextTile != nullptr)
	{
		path.insert(path.begin(), Vector2(nextTile->x, nextTile->y));
		nextTile->isPath = true;
		nextTile = nextTile->previousTile;
	}
	return path;
}

int Astar::GetXGridSize()
{
	return xGridSize;
}

int Astar::GetYGridSize()
{
	return yGridSize;
}

/// <summary>
/// Set pathfinding start position and end position
/// </summary>
/// <param name="start">Start position</param>
/// <param name="end">End position</param>
void Astar::SetDestination(Vector2 start, Vector2 end)
{
	ResetGrid(false);

	startPos = start;
	endPos = end;

	endTile = GetTile(end.x, end.y);

	currentTile = GetTile(start.x, start.y);
	currentTile->closed = true;
	currentTile->g = 0;
}

/// <summary>
/// Start pathfinding procress until a path is found or no path is possible
/// </summary>
void Astar::Process()
{
	while (!endTile->closed && !cantAccess)
	{
		ProcessOneStep();
	}
}