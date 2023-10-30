#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include "../vectors/vector2.h"
#include <vector>

class API Astar
{
public:
	Astar();
	~Astar();

	/**
	* Compute pathfinding and get the path
	* @return Points positions (start and end included) or empty if not path was found
	*/
	std::vector<Vector2> GetPath();

	/**
	* Set destination
	* @param start Start point position
	* @param end End point position
	*/
	void SetDestination(const Vector2& start, const Vector2& end);

	/**
	* Set if a tile is an obstacle or not
	* @param x Tile X position
	* @param y Tile Y position
	* @param isObstacle Is tile an obstacle
	*/
	void SetTileIsObstacle(int x, int y, bool isObstacle);

	/**
	* Set the grid size
	* @param xSize Grid X size (columns)
	* @param ySize Grid Y size (rows)
	*/
	void SetGridSize(int xSize, int ySize);

	/**
	* Clear obstacles
	*/
	void ResetGrid();

	/**
	* Get X (columns) grid size
	*/
	int GetXGridSize() const
	{
		return xGridSize;
	}

	/**
	* Get Y (rows) grid size
	*/
	int GetYGridSize() const
	{
		return yGridSize;
	}

	/**
	* Set to true to reduce accurary but improve speed
	*/
	bool lowAccuracy = false;

	/**
	* Set to true to allow the path to cross corners
	*/
	bool canPassCorners = false;

private:

	class Tile
	{
	public:
		bool isObstacle = false;
		int g = 0;
		int h = 0;
		int f = 0; // = g + h
		int x = 0;
		int y = 0;

		bool closed = false;
		Tile* previousTile = nullptr;
		bool inList = false;
	};

	/**
	* Get tile (fast version)
	*/
	Tile* GetTileFast(int x, int y) const
	{
		return &grid[x * yGridSize + y];
	}

	/**
	* Get tile (fastest version)
	*/
	Tile* GetTileUltraFast(int row, int col) const
	{
		return &grid[row + col];
	}

	/**
	* Get tile
	*/
	Tile* GetTile(int x, int y) const
	{
		if (grid == nullptr || !IsValidPosition(x, y))
			return nullptr;

		return &grid[x * yGridSize + y];
	}

	/**
	* Get if the position if valid or out of bounds
	*/
	bool IsValidPosition(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= xGridSize || y >= yGridSize)
			return false;

		return true;
	}

	/**
	* Get the tile with the lowest F value
	*/
	void GetLowestFTile();

	/**
	* To remove?
	*/
	void SetFinalPath();

	/**
	* Reset pathfinding values (Clear obstacles if clearObstacles is true)
	*/
	void ResetGrid(bool clearObstacles);

	/**
	* Free grid memory
	*/
	void DeleteGrid();

	/**
	* Process one tick of the pathfinding system
	*/
	void ProcessOneStep();

	Tile* grid = nullptr;
	std::vector<Tile*> nextTilesToCheck;
	Vector2 startPos = Vector2(0, 0);
	Vector2 endPos = Vector2(0, 0);
	Tile* currentTile = nullptr;
	Tile* endTile = nullptr;
	bool cantAccess = false;
	int xGridSize = 0;
	int yGridSize = 0;
};

