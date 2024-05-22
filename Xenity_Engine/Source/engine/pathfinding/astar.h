// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/vectors/vector2.h>
#include <vector>

/**
* @brief Class used for path finding
*/
class API Astar
{
public:
	Astar();
	~Astar();

	/**
	* @brief Compute pathfinding and get the path
	* @return Points positions (start and end included) or empty if not path was found
	*/
	std::vector<Vector2> GetPath();

	/**
	* @brief Set destination
	* @param start Start point position
	* @param end End point position
	*/
	void SetDestination(const Vector2& start, const Vector2& end);

	/**
	* @brief Set if a tile is an obstacle or not
	* @param x Tile X position
	* @param y Tile Y position
	* @param isObstacle Is tile an obstacle
	*/
	void SetTileIsObstacle(int x, int y, bool isObstacle);

	/**
	* @brief Set the grid size
	* @param xSize Grid X size (columns)
	* @param ySize Grid Y size (rows)
	*/
	void SetGridSize(int xSize, int ySize);

	/**
	* @brief Clear obstacles
	*/
	void ResetGrid();

	/**
	* @brief Get X (columns) grid size
	*/
	int GetXGridSize() const
	{
		return xGridSize;
	}

	/**
	* @brief Get Y (rows) grid size
	*/
	int GetYGridSize() const
	{
		return yGridSize;
	}

private:

	class Tile
	{
	public:
		Tile* previousTile = nullptr;
		int g = 0;
		int h = 0;
		int f = 0; // = g + h
		int x = 0;
		int y = 0;

		bool closed = false;
		bool isObstacle = false;
		bool inList = false;
	};

	/**
	* @brief Get tile (fast version)
	*/
	Tile* GetTileFast(int x, int y) const
	{
		return &grid[x * yGridSize + y];
	}

	/**
	* @brief Get tile (fastest version)
	*/
	Tile* GetTileUltraFast(int row, int col) const
	{
		return &grid[row + col];
	}

	/**
	* @brief Get tile
	*/
	Tile* GetTile(int x, int y) const
	{
		if (grid == nullptr || !IsValidPosition(x, y))
			return nullptr;

		return &grid[x * yGridSize + y];
	}

	/**
	* @brief Get if the position if valid or out of bounds
	*/
	bool IsValidPosition(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= xGridSize || y >= yGridSize)
			return false;

		return true;
	}

	/**
	* @brief Get the tile with the lowest F value
	*/
	void GetLowestFTile();

	/**
	* @brief To remove?
	*/
	void SetFinalPath();

	/**
	* @brief Reset pathfinding values (Clear obstacles if clearObstacles is true)
	*/
	void ResetGrid(bool clearObstacles);

	/**
	* @brief Free grid memory
	*/
	void DeleteGrid();

	/**
	* @brief Process one tick of the pathfinding system
	*/
	void ProcessOneStep();

	Tile* grid = nullptr;
	std::vector<Tile*> nextTilesToCheck;
	Vector2 startPos = Vector2(0, 0);
	Vector2 endPos = Vector2(0, 0);
	Tile* currentTile = nullptr;
	Tile* endTile = nullptr;
	int xGridSize = 0;
	int yGridSize = 0;
	bool cantAccess = false;

public:
	/**
	* @brief Set to true to reduce accurary but improve speed
	*/
	bool lowAccuracy = false;

	/**
	* @brief Set to true to allow the path to cross corners
	*/
	bool canPassCorners = false;
};

