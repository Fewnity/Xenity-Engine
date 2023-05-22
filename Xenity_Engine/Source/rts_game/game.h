#pragma once

#include "../engine/game_elements/gameobject.h"
#include "../engine/vectors/vector2.h"
#include "../engine/vectors/vector4.h"

class Unit;
class UnitData;
class LineRenderer;
class Astar;
class CameraManager;
class UnitManager;
class MapManager;
class Material;
class Texture;
class Shader;
class TextRendererCanvas;
class BuildManager;
class TeamManager;

enum ManageMode 
{
	ManageUnits,
	ManageBuildings,
	SpawnUnit,
};

class Game
{
public:
	static Game* game;
	static Game* GetGame()
	{
		return game;
	}

	void Init();
	void Loop();

	void SetSelectionUI(bool isSelecting);
	bool isPointInsideAABB(Vector2 point, Vector2 aMin, Vector2 aMax);
	bool intersect(Vector2 aMin, Vector2 aMax, Vector2 bMin, Vector2 bMax);

	Vector2 startSelectionPos = Vector2(0, 0);
	bool isDragging = false;
	LineRenderer* lineRendererTop = nullptr;
	LineRenderer* lineRendererBottom = nullptr;
	LineRenderer* lineRendererLeft = nullptr;
	LineRenderer* lineRendererRight = nullptr;
	Texture* crosshair = nullptr;
	MapManager* mapManager = nullptr;
	BuildManager* buildManager = nullptr;
	UnitManager* unitManager = nullptr;
	TeamManager* teamManager = nullptr;

	ManageMode manageMode = ManageMode::ManageUnits;

	int wood = 0;
	int rock = 0;
	int gold = 0;
	int crystal = 0;
	//Materials
	Material* material2D = nullptr;
	Material* material2DWithZ = nullptr;

private:
	void CheckMouseInput();
	void CheckKeyboardInput();
	void LoadGameData();
	void MoveCursor();
	void OnMouseUp();
	void UpdateRessourcesText();
	void UpdateModeText();

	GameObject* gameObjectCrosshair = new GameObject("Crosshair");
	GameObject* gameObjectLineRenderers = new GameObject("Lines");

	CameraManager* cameraManager = nullptr;;

	Vector2 cursorPosition = Vector2(0, 0);
	Vector2 startMousePosition = Vector2(0, 0);
	Vector4 selectionColor = Vector4(1, 1, 1, 1);


	//Components
	TextRendererCanvas* ressourcesTextRenderer = nullptr;
	TextRendererCanvas* modeTextRenderer = nullptr;


	//Textures
	Texture* textureShip = nullptr;
	Texture* gradient = nullptr;

	//Shaders
	Shader* shaderTextCanvas = nullptr;
};

