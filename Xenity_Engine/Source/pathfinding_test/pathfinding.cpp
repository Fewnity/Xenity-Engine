#include "pathfinding.h"
#include "../xenity.h"

#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>


//Dijkstras
int source = 0;
int dest = 2;

int aniamationStep = 0;
float animationTimer = 0;
bool showAStar = false;
bool processStarted = true;

/// <summary>
/// Init game
/// </summary>
void PathFinding::Init()
{
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");
	Shader* shaderStandard2DText = new Shader("vertexStandard2DText.shader", "fragmentStandard2DText.shader");

	material2D = new Material("2D Standard");
	material2D->shader = shaderStandard2D;

	LoadGameData();

	int block = 0;

	dijkstras.nodes = new int* [dijkstras.squareSize] {
		new int[dijkstras.squareSize] {block, 2, block, 12},//A
			new int[dijkstras.squareSize] {block, block, 20, 5},//B
			new int[dijkstras.squareSize] {block, block, block, block},//C
			new int[dijkstras.squareSize] {12, block, 8, block}//D
	};

	dijkstras.processDijkstras(source, dest);

	Texture* circle = new Texture("pathfinding/circle.png", "circle");
	Texture* square = new Texture("pathfinding/square.png", "square");
	circle->SetPixelPerUnit(128);
	square->SetPixelPerUnit(64);
	square->SetFilter(Texture::Point);

	VisualNode* newVisualNodeA = new VisualNode(Vector2(-1, -1), circle); // A
	newVisualNodeA->textRenderer->text = "A";
	newVisualNodeA->index = 0;
	visualNodes.push_back(newVisualNodeA);

	VisualNode* newVisualNodeB = new VisualNode(Vector2(-1, 1), circle); // B
	newVisualNodeB->textRenderer->text = "B";
	newVisualNodeB->index = 1;
	visualNodes.push_back(newVisualNodeB);

	VisualNode* newVisualNodeC = new VisualNode(Vector2(1, 1), circle); // C
	newVisualNodeC->textRenderer->text = "C";
	newVisualNodeC->index = 2;
	visualNodes.push_back(newVisualNodeC);

	VisualNode* newVisualNodeD = new VisualNode(Vector2(1, -1), circle); // D
	newVisualNodeD->textRenderer->text = "D";
	newVisualNodeD->index = 3;
	visualNodes.push_back(newVisualNodeD);

	newVisualNodeA->connectedTo.push_back(newVisualNodeB);
	newVisualNodeA->connectedTo.push_back(newVisualNodeD);

	newVisualNodeB->connectedTo.push_back(newVisualNodeC);
	newVisualNodeB->connectedTo.push_back(newVisualNodeD);

	newVisualNodeD->connectedTo.push_back(newVisualNodeA);
	newVisualNodeD->connectedTo.push_back(newVisualNodeC);

	aStarVisualNodes = (AStarVisualNode*)malloc(astar.GetXGridSize() * astar.GetYGridSize() * sizeof(AStarVisualNode));

	for (int x = 0; x < astar.GetXGridSize(); x++)
	{
		for (int y = 0; y < astar.GetYGridSize(); y++)
		{
			aStarVisualNodes[x * astar.GetYGridSize() + y].Setup(Vector2(x, y), square);
		}
	}

	RandomiseAStarGrid();

	astarStartPos = Vector2(1, 1);
	astarEndPos = Vector2(76, 76);

	StartAStarProcess();

	/*std::cout << "Shortest Paths from " << source << ": " << std::endl;
	for (int i = 0; i < squareSize; i++)
	{
		std::cout << "End point: " << i << ", distance: " << dist[i] << ", path: " << source << " ";
		printParentPath(path, i);
		std::cout << std::endl;
	}
	std::cout << std::endl;*/

	SwitchMode();

}

void PathFinding::RandomiseAStarGrid()
{
	processStarted = false;
	astar.ResetGrid();

	//for (int i = 0; i < 6000; i++)
	for (int i = 0; i < 3000; i++)
	{
		int x = rand() % 100;
		int y = rand() % 100;
		if (x == astarStartPos.x && y == astarStartPos.y || x == astarEndPos.x && y == astarEndPos.y)
			continue;

		astar.SetTileIsObstacle(x, y, true);
	}
}

void PathFinding::StartAStarProcess()
{
	processStarted = true;
	astar.SetDestination(astarStartPos, astarEndPos);
	//astar.Process(astarStartPos, astarEndPos);
}

void PathFinding::SwitchMode()
{
	animationTimer = 0;
	aniamationStep = 0;

	showAStar = !showAStar;
	int visualNodesCount = visualNodes.size();

	for (int i = 0; i < visualNodesCount; i++)
	{
		visualNodes[i]->gameobject->SetActive(!showAStar);
	}

	for (int x = 0; x < astar.GetXGridSize(); x++)
	{
		for (int y = 0; y < astar.GetYGridSize(); y++)
		{
			aStarVisualNodes[x * astar.GetYGridSize() + y].gameobject->SetActive(showAStar);
		}
	}

	if (showAStar)
	{
		camera->SetProjectionSize(40);
		camera->GetGameObject()->transform.SetPosition(Vector3(20, 39.5f, -10));
	}
	else
	{
		camera->SetProjectionSize(3);
		camera->GetGameObject()->transform.SetPosition(Vector3(0, 0, -10));
	}
}

void PathFinding::LoadGameData()
{
	camera = cameraGameObject->AddComponent<Camera>();
	//cameraGameObject->AddExistingComponent(camera);
	camera->GetGameObject()->transform.SetPosition(Vector3(0, 0, -10));

	camera->SetProjectionType(Orthographic);
	camera->SetProjectionSize(3);
}

void DrawArrow(Vector2 start, Vector2 end, float width, Color color, Material* material)
{
	Vector2 dir = (end - start).normalize();
	float angle = atan2(dir.x, dir.y);
	Vector2 leftEnd = end - dir / 2 + Vector2(-cos(angle) / 4, sin(angle) / 4);
	Vector2 rightEnd = end - dir / 2 + Vector2(cos(angle) / 4, -sin(angle) / 4);

	SpriteManager::Render2DLine(Vector3(start.x, start.y, 0), Vector3(end.x, end.y, 0), 0.05f, color, AssetManager::GetMaterialByName("2D Standard")); //Line
	SpriteManager::Render2DLine(Vector3(end.x, end.y, 0), Vector3(leftEnd.x, leftEnd.y, 0), 0.05f, color, AssetManager::GetMaterialByName("2D Standard"));//Left
	SpriteManager::Render2DLine(Vector3(end.x, end.y, 0), Vector3(rightEnd.x, rightEnd.y, 0), 0.05f, color, AssetManager::GetMaterialByName("2D Standard"));//Right
}

void PathFinding::DrawAStar()
{
	if (!astar.cantAccess && astar.currentTile != astar.endTile && processStarted)
	{
		animationTimer += Time::GetDeltaTime();
		if (animationTimer >= 0.03f) 
		{
			animationTimer = 0;
			astar.ProcessOneStep();
		}
	}

	for (int x = 0; x < astar.GetXGridSize(); x++)
	{
		for (int y = 0; y < astar.GetYGridSize(); y++)
		{
			Astar::Tile* tile = astar.GetTile(x, y);
			AStarVisualNode* visualNode = &aStarVisualNodes[x * astar.GetYGridSize() + y];
			if (tile->isObstacle)
				visualNode->SetColor(4);
			else if ((x == astarEndPos.x && y == astarEndPos.y) || (x == astarStartPos.x && y == astarStartPos.y))
				visualNode->SetColor(5);
			else if (tile->isPath)
				visualNode->SetColor(2);
			else if (tile->closed)
				visualNode->SetColor(1);
			else if (tile->f != 0)
				visualNode->SetColor(3);
			else
				visualNode->SetColor(0);
		}
	}

	std::string inputsText = "R to randomise the grid\n";
	inputsText += "\nSpace to re/start the process\n";
	if (astar.lowAccuracy)
		inputsText += "\nLow accuracy mode:\nEnabled (A to change)\n";
	else
		inputsText += "\nLow accuracy mode:\nDisabled (A to change)\n";
	if (astar.canPassCorners)
		inputsText += "\nCan pass corners mode:\nEnabled (C to change)\n";
	else
		inputsText += "\nCan pass corners mode:\nDisabled (C to change)\n";

	UiManager::RenderTextCanvas(inputsText, 0, 0.5f, 0, 0.7f, 0, Color::CreateFromRGBAFloat(0.5f, 0.0f, 0.2f,1), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));
}

void PathFinding::DrawDijkstras()
{
	animationTimer += Time::GetDeltaTime();
	if (animationTimer >= 1) {
		animationTimer = 0;
		aniamationStep++;
		if (aniamationStep >= dijkstras.pathCount)
		{
			aniamationStep = 0;
		}
	}

	std::string finalPath = "";
	finalPath += "Shortest Paths from " + std::to_string(source) + ": \n";

	/*for (int i = 0; i < squareSize; i++)
	{
		finalPath += "End point: " + std::to_string(i) + ", distance: " + std::to_string(dist[i]) + ", path: " + std::to_string(source) + " ";
		printParentPath(path, i, finalPath);
		finalPath += "\n";
	}*/

	std::string sourceToDestText = "Shortest path from A to C\n";
	sourceToDestText += "step " + std::to_string(aniamationStep) + "/" + std::to_string(dijkstras.pathCount - 1);

	UiManager::RenderTextCanvas(sourceToDestText, 0.5, 0.03f, 0, 0.7f, 0, Color::CreateFromRGBAFloat(0.5f, 0.0f, 0.2f, 1), UiManager::fonts[0], H_Center, V_Bottom, *AssetManager::GetShader(7));
	//UiManager::RenderTextCanvas(finalPath, 0.5, 0.5, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Center, V_Center, *AssetManager::GetShader(7));

	Color arrowColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	Color greenArrowColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	Color textColor = Color::CreateFromRGBAFloat(0, 0, 0, 1);

	//Show graph
	int visualNodesCount = visualNodes.size();
	for (int i = 0; i < visualNodesCount; i++)
	{
		float startX = visualNodes[i]->gameobject->transform.GetPosition().x;
		float startY = visualNodes[i]->gameobject->transform.GetPosition().y;
		int visualNodesConnectionCount = visualNodes[i]->connectedTo.size();
		for (int j = 0; j < visualNodesConnectionCount; j++)
		{
			float endX = visualNodes[i]->connectedTo[j]->gameobject->transform.GetPosition().x;
			float endY = visualNodes[i]->connectedTo[j]->gameobject->transform.GetPosition().y;
			Vector2 dir = (Vector2(endX, endY) - Vector2(startX, startY)).normalize();
			Vector2 dir2 = (Vector2(endX, endY) - Vector2(startX, startY));
			Vector2 textPos = Vector2(startX, startY) + dir2 / 2.0f;

			DrawArrow(Vector2(startX, startY) + dir * 0.25f, Vector2(endX, endY) - dir * 0.25f, 0.05f, arrowColor, AssetManager::GetMaterialByName("2D Standard"));

			std::string arrowText = "" + std::to_string(dijkstras.nodes[visualNodes[i]->index][visualNodes[i]->connectedTo[j]->index]);
			UiManager::RenderText(arrowText, textPos.x, textPos.y, 0, 0.7f, 0, textColor, UiManager::fonts[0], H_Center, V_Center, *AssetManager::GetShader(10));
		}
	}

	//Show path
	for (int i = 0; i < aniamationStep; i++)
	{
		int startIndex = dijkstras.usedPath[i];
		int endIndex = dijkstras.usedPath[i + 1];

		float startX = visualNodes[startIndex]->gameobject->transform.GetPosition().x;
		float startY = visualNodes[startIndex]->gameobject->transform.GetPosition().y;

		float endX = visualNodes[endIndex]->gameobject->transform.GetPosition().x;
		float endY = visualNodes[endIndex]->gameobject->transform.GetPosition().y;
		Vector2 dir = (Vector2(endX, endY) - Vector2(startX, startY)).normalize();

		DrawArrow(Vector2(startX, startY) + dir * 0.25f, Vector2(endX, endY) - dir * 0.25f, 0.05f, greenArrowColor, AssetManager::GetMaterialByName("2D Standard"));
	}
}

/// <summary>
/// Game loop
/// </summary>
void PathFinding::Loop()
{
	if (!showAStar)
		DrawDijkstras();
	else
		DrawAStar();

	if (InputSystem::GetKeyDown(M))
	{
		SwitchMode();
	}

	if (showAStar)
	{
		if (InputSystem::GetKeyDown(SPACE))
		{
			StartAStarProcess();
		}
		if (InputSystem::GetKeyDown(A))
		{
			astar.lowAccuracy = !astar.lowAccuracy;
		}
		if (InputSystem::GetKeyDown(R))
		{
			RandomiseAStarGrid();
		}
		if (InputSystem::GetKeyDown(C))
		{
			astar.canPassCorners = !astar.canPassCorners;
		}
	}

	//Move camera
	float cameraArrowMoveSpeed = 10;
	Vector3 newCameraPosition = camera->GetGameObject()->transform.GetPosition();
	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetUp();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(S))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetDown();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetRight();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetLeft();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	camera->GetGameObject()->transform.SetPosition(newCameraPosition);
}

VisualNode::VisualNode(Vector2 position, Texture* sprite)
{
	gameobject = new GameObject();
	gameobject->transform.SetPosition(Vector3(position.x, position.y, 0));
	//SpriteRenderer* spr = new SpriteRenderer(sprite, AssetManager::GetMaterialByName("2D Standard"));
	SpriteRenderer* spr = gameobject->AddComponent<SpriteRenderer>();
	spr->texture = sprite;
	spr->material = AssetManager::GetMaterialByName("2D Standard");
	//gameobject->AddExistingComponent(spr);
	//gameobject->AddExistingComponent(textRenderer);

	textRenderer = gameobject->AddComponent<TextRenderer>();
	textRenderer->color = Color::CreateFromRGBAFloat(0, 0, 0, 1);
	textRenderer->font = UiManager::fonts[0];
	textRenderer->shader = AssetManager::GetShader(10);
	textRenderer->size = 0.7f;
	//textRenderer = new TextRenderer(UiManager::fonts[0], 0.7f, AssetManager::GetShader(10));
}

void AStarVisualNode::Setup(Vector2 position, Texture* sprite)
{
	gameobject = new GameObject();
	gameobject->transform.SetPosition(Vector3(position.x, position.y, 0));
	//spriteRenderer = new SpriteRenderer(sprite, AssetManager::GetMaterialByName("2D Standard"));
	spriteRenderer = gameobject->AddComponent<SpriteRenderer>();
	spriteRenderer->texture = sprite;
	spriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");

	//textRenderer = new TextRenderer(UiManager::fonts[0], 0.6f, AssetManager::GetShader(10));
	//textRenderer->color = Vector3(0, 0, 0);
	//textRenderer->text = "00 00\n\n00";
	//gameobject->AddExistingComponent(spriteRenderer);
	//spriteRenderer = gameobject->AddComponent<TextRenderer>();

	//gameobject->AddExistingComponent(textRenderer);
}

void AStarVisualNode::SetColor(int color)
{
	if (color == 0)
		spriteRenderer->color = Color::CreateFromRGBAFloat(1, 1, 1, 1); // Not scanned
	else if (color == 1)
		spriteRenderer->color = Color::CreateFromRGBAFloat(1, 0.5, 0.5, 1); //closed
	else if (color == 2)
		spriteRenderer->color = Color::CreateFromRGBAFloat(0.5, 1, 0.5, 1); //Final path
	else if (color == 3)
		spriteRenderer->color = Color::CreateFromRGBAFloat(0.5, 0.5, 1, 1); //Scanned but not locked
	else if (color == 4)
		spriteRenderer->color = Color::CreateFromRGBAFloat(0, 0, 0, 1); //Obstacle
	else if (color == 5)
		spriteRenderer->color = Color::CreateFromRGBAFloat(1, 0, 1, 1); //Source and End
}
