#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"
#include "../graphics/iDrawable.h"

class Unit;
class UnitData;
class LineRenderer;
class Astar;
class CameraManager;
class UnitManager;
class MapManager;

class Game
{
public:
	static Game* game;
	static Game* GetGame();

	void Init();
	void Loop();
	Vector2 startSelectionPos = Vector2(0, 0);
	bool isDragging = false;
	LineRenderer* lineRendererTop = nullptr;
	LineRenderer* lineRendererBottom = nullptr;
	LineRenderer* lineRendererLeft = nullptr;
	LineRenderer* lineRendererRight = nullptr;
	void SetSelection(bool isSelecting);
	bool isPointInsideAABB(Vector2 point, Vector2 aMin, Vector2 aMax);
	bool intersect(Vector2 aMin, Vector2 aMax, Vector2 bMin, Vector2 bMax);
	Texture* crosshair = nullptr;
	MapManager* mapManager = nullptr;
	UnitManager* unitManager = nullptr;;

private:
	void LoadGameData();
	void MoveCursor();
	void OnMouseUp();
	
	GameObject* gameObjectCrosshair = new GameObject("Crosshair");
	GameObject* gameObjectCrosshair2 = new GameObject("Sprite test");

	GameObject* gameObjectLineRenderers = new GameObject("Lines");


	CameraManager* cameraManager = nullptr;;

	Vector2 cursorPosition = Vector2(0, 0);



	Vector2 startMousePosition = Vector2(0, 0);

	Vector4 selectionColor = Vector4(1, 1, 1, 1);

	//Textures

	Texture* textureShip = nullptr;
	Texture* gradient = nullptr;

	//Shaders

	//Materials
	Material* material2D = nullptr;
	Material* material2DWithZ = nullptr;
};

