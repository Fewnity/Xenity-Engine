// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "astar.h"
#include <malloc.h>
#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>
#include <cstdint>

Astar::Astar()
{
}

void Astar::SetGridSize(int xSize, int ySize)
{
	// Destroy current data
	DeleteGrid();
	if (xSize > 0 && ySize > 0)
	{
		xGridSize = xSize;
		yGridSize = ySize;

		// Create new grid
		grid = (Tile*)malloc((size_t)xGridSize * yGridSize * sizeof(Tile));

		// Fill grid with default values
		ResetGrid();
	}
}

void Astar::DeleteGrid()
{
	free(grid);
}

Astar::~Astar()
{
	nextTilesToCheck.clear();
	DeleteGrid();
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
			tile->previousTile = nullptr;
		}
	}
	nextTilesToCheck.clear();
	cantAccess = false;
}

void Astar::SetTileIsObstacle(int x, int y, bool isObstacle)
{
	Tile* tile = GetTile(x, y);

	if (tile)
		tile->isObstacle = isObstacle;
}

void Astar::GetLowestFTile()
{
	int nextTileCount = (int)nextTilesToCheck.size();
	if (nextTileCount != 0)
	{

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
	// SetFinalPath();
}

void Astar::SetFinalPath()
{
	for (int x = 0; x < xGridSize; x++)
	{
		for (int y = 0; y < yGridSize; y++)
		{
			Tile* t = GetTileFast(x, y);
		}
	}

	Tile* nextTile = currentTile;
	while (nextTile != nullptr)
	{
		nextTile = nextTile->previousTile;
	}
}

std::vector<Vector2> Astar::GetPath()
{
	ResetGrid(false);
	std::vector<Vector2> path;

	endTile = GetTile((int)endPos.x, (int)endPos.y);
	currentTile = GetTile((int)startPos.x, (int)startPos.y);
	currentTile->closed = true;
	currentTile->g = 0;

	if (startPos != endPos)
	{
		if (endTile->isObstacle)
		{
			cantAccess = true;
		}
		else
		{
			while (!endTile->closed && !cantAccess)
			{
				ProcessOneStep();
			}

			if (!cantAccess)
			{
				// Start from the end
				Tile* nextTile = currentTile;

				// Navigate to the start tile
				while (nextTile != nullptr)
				{
					path.insert(path.begin(), Vector2((float)nextTile->x, (float)nextTile->y));
					nextTile = nextTile->previousTile;
				}
			}
		}
	}

	return path;
}

void Astar::SetDestination(const Vector2& start, const Vector2& end)
{
	if (IsValidPosition((int)start.x, (int)start.y) && IsValidPosition((int)end.x, (int)end.y))
	{
		startPos = start;
		endPos = end;
	}
}