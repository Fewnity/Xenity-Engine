#pragma once
#include "../vectors/vector2.h"
#include <vector>

class Astar
{
public:
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
		bool isPath = false;
	};

	Astar() = delete;
	Astar(int xGridSize, int yGridSize);
	Tile* GetTile(int x, int y);
	void SetTileIsObstacle(int x, int y, bool isObstacle);
	void Process();
	void ProcessOneStep();
	void GetLowestFTile();
	void ResetGrid(bool clearObstacles);
	void SetFinalPath();
	std::vector<Vector2> GetPath();

	int xGridSize = 0;
	int yGridSize = 0;
	Vector2 startPos;
	Vector2 endPos;
	Tile* currentTile = nullptr;
	Vector2 currentTilePosition;
	bool cantAccess = false;
	Tile* grid = nullptr;
	bool lowAccuracy = false;
	bool canPassCorners = false;
	Tile* endTile = nullptr;
	void SetDestination(Vector2 start, Vector2 end);

private:

};

