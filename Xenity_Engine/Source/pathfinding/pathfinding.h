#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"
#include "dijkstras.h"
#include "astar.h"

class TextRenderer;
class SpriteRenderer;

class VisualNode {
public:
	VisualNode() = delete;
	VisualNode(Vector2 position, Texture* sprite);
	TextRenderer* textRenderer = nullptr;
	int index = 0;
	std::vector<VisualNode*> connectedTo;
	GameObject* gameobject = nullptr;
};

class AStarVisualNode {
public:
	AStarVisualNode() = delete;
	void Setup(Vector2 position, Texture* sprite);
	void SetColor(int color);
	TextRenderer* textRenderer = nullptr;
	SpriteRenderer* spriteRenderer = nullptr;
	GameObject* gameobject = nullptr;
};

class PathFinding
{
public:
	void Init();
	void Loop();
private:
	void LoadGameData();
	void DrawDijkstras();
	void DrawAStar();
	void SwitchMode();
	void RandomiseAStarGrid();
	void StartAStarProcess();

	std::vector< VisualNode*> visualNodes;
	AStarVisualNode* aStarVisualNodes = nullptr;

	GameObject* cameraGameObject = new GameObject("cameraGameObject");
	Camera* camera = new Camera();
	Material* material2D = nullptr;
	Dijkstras dijkstras = Dijkstras(4);
	Astar astar = Astar(80, 80);

	Vector2 astarStartPos;
	Vector2 astarEndPos;
};

