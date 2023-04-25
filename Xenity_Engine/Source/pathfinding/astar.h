#pragma once
#include "../vectors/vector2.h"
#include <vector>

class Astar
{
public:

	Astar() = delete;
	Astar(int xGridSize, int yGridSize);
	void SetTileIsObstacle(int x, int y, bool isObstacle);
	void Process();
	void ResetGrid();
	std::vector<Vector2> GetPath();
	int GetXGridSize();
	int GetYGridSize();

	bool cantAccess = false;
	bool lowAccuracy = false;
	bool canPassCorners = false;
	void SetDestination(Vector2 start, Vector2 end);

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
		bool isPath = false;
		bool inList = false;
	};
	Tile* GetTile(int x, int y);
	Tile* currentTile = nullptr;
	Tile* endTile = nullptr;
	void ProcessOneStep();

private:
	void GetLowestFTile();
	void SetFinalPath();
	Tile* GetTileFast(int x, int y);
	Tile* GetTileUltraFast(int& row, int& col);
	void ResetGrid(bool clearObstacles);
	Tile* grid = nullptr;
	std::vector<Tile*> nextTilesToCheck;
	Vector2 startPos;
	Vector2 endPos;
	int xGridSize = 0;
	int yGridSize = 0;
};

