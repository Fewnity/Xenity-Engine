#pragma once
#include "../vectors/vector2.h"
#include <vector>

class Astar
{
public:
	Astar();
	Astar(int xGridSize, int yGridSize);
	~Astar();

	std::vector<Vector2> GetPath();
	void SetDestination(Vector2 start, Vector2 end);
	void SetTileIsObstacle(int x, int y, bool isObstacle);
	void SetGridSize(int xSize, int ySize);
	void ResetGrid();

	int GetXGridSize()
	{
		return xGridSize;
	}

	int GetYGridSize()
	{
		return yGridSize;
	}

	bool lowAccuracy = false;
	bool canPassCorners = false;

	//To move in private
	class Tile {
	public:
		bool isObstacle = false;
		int g = 0;
		int h = 0;
		int f = 0; // = g + h
		int x = 0;
		int y = 0;

		bool closed = false;
		Tile* previousTile = nullptr;
		bool isPath = false; // TODO To remove later
		bool inList = false;
	};
	
	Tile* currentTile = nullptr;
	Tile* endTile = nullptr;
	void ProcessOneStep();
	Tile* GetTile(int x, int y)
	{
		if (grid == nullptr || !IsValidPosition(x, y))
			return nullptr;

		return &grid[x * yGridSize + y];
	}

	bool IsValidPosition(int x, int y)
	{
		if (x < 0 || y < 0 || x >= xGridSize || y >= yGridSize)
			return false;

		return true;
	}

	bool cantAccess = false;

private:
	Tile* GetTileFast(int x, int y)
	{
		return &grid[x * yGridSize + y];
	}

	Tile* GetTileUltraFast(int& row, int& col)
	{
		return &grid[row + col];
	}

	void GetLowestFTile();
	void SetFinalPath();
	void ResetGrid(bool clearObstacles);
	void DeleteGrid();

	Tile* grid = nullptr;
	std::vector<Tile*> nextTilesToCheck;
	Vector2 startPos = Vector2(0,0);
	Vector2 endPos = Vector2(0, 0);
	int xGridSize = 0;
	int yGridSize = 0;
};

