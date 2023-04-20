#include "engine.h"
#include "ui/ui_manager.h"
#include "file_system/file.h"
#include "inputs/input_system.h"
#include "scene_manager/scene.h"
#include "ui/window.h"
#include "gameobject.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include "asset_manager.h"
#include "engine_settings.h"
#include "graphics/sprite_manager.h"
#include "debug/debug.h"
#include "tools/benchmark.h"
#include "time/time.h"
#include "debug/performance.h"
#include "ui/TextAlignments.h"
#include "rts_game/game.h"
#include "pathfinding/pathfinding.h"
#include "tools/profiler_benchmark.h"
#include "gameOld.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

std::vector<GameObject*> Engine::gameObjects;
GameObject* Engine::selectedGameObject = nullptr;

int Engine::gameObjectCount = 0;
float lastTick = 0;

ProfilerBenchmark* engineLoopBenchmark = new ProfilerBenchmark("Engine loop");
ProfilerBenchmark* gameLoopBenchmark = new ProfilerBenchmark("Game loop");
ProfilerBenchmark* componentsUpdateBenchmark = new ProfilerBenchmark("Components update");
ProfilerBenchmark* drawIDrawablesBenchmark = new ProfilerBenchmark("Draw");
bool Engine::componentsListDirty = true;
std::vector<Component*> Engine::orderedComponents;
int Engine::componentsCount = 0;

/// <summary>
/// Init engine
/// </summary>
/// <returns></returns>
int Engine::Init(const std::string exePath)
{
	/* Initialize libraries */
	Debug::Init();
	File::InitFileSystem(exePath);

	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}
	InputSystem::Init();
	SpriteManager::Init();
	AssetManager::Init();

	//Init random
	srand((unsigned int)time(NULL));


	ImGui::GetStyle().WindowRounding = 10;


	return 0;
}

void Engine::DrawProfiler()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Debug");
	ImGui::Text("FPS: %d, ImGui: %.1f", (int)(1 / Time::GetUnscaledDeltaTime()), io.Framerate);
	ImGui::Text("DrawCall Count: %d", Performance::GetDrawCallCount());
	ImGui::Text("Updated Materials: %d", Performance::GetUpdatedMaterialCount());

	if (Performance::IsProfilerEnabled())
	{
		//Add profiler texts
		for (const auto& kv : Performance::profilerList)
		{
			ImGui::Text("%s: %ld, avg %ld", kv.first.c_str(), kv.second->GetValue(), kv.second->average);
		}
	}

	//ImGui::SliderFloat("float", &cameraZoom, 1.0f, 2.8f);

	ImGui::End();
}

void Engine::DrawInspector()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector");
	if (selectedGameObject != nullptr) {
		ImGui::Text("Name %s", selectedGameObject->name.c_str());
		ImGui::Text("Local Position %f %f %f", selectedGameObject->transform.GetLocalPosition().x, selectedGameObject->transform.GetLocalPosition().y, selectedGameObject->transform.GetLocalPosition().z);
		ImGui::Text("World Position %f %f %f", selectedGameObject->transform.GetPosition().x, selectedGameObject->transform.GetPosition().y, selectedGameObject->transform.GetPosition().z);
	}
	ImGui::End();
}

void Engine::DrawTreeItem(GameObject* child)
{
	int childCount = child->children.size();
	int flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
	if (Engine::selectedGameObject == child)
		flags |= ImGuiTreeNodeFlags_Selected;
	if (childCount != 0)
	{

		if (ImGui::TreeNodeEx(child->name.c_str(), flags))
		{
			for (int i = 0; i < childCount; i++)
			{
				DrawTreeItem(child->children[i]);
			}
			ImGui::TreePop();
		}
		if (ImGui::IsItemActivated() && ImGui::IsItemClicked())
		{
			SetSelectedGameObject(child);
			std::cout << "HELO" << std::endl;
		}
	}
	else
	{
		//ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetCursorScreenPos().x + 100, ImGui::GetCursorScreenPos().y + 220), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)));
		ImGui::Indent();
		ImGui::Text(child->name.c_str());
		ImGui::Unindent();
		if (ImGui::IsItemClicked())
		{
			SetSelectedGameObject(child);
		}
		/*if (ImGui::TreeNodeEx(child->name.c_str(), flags | ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::IsItemClicked() && ImGui::IsItemActivated() && ImGui::IsItemFocused() && ImGui::IsItemHovered())
			{
				SetSelectedGameObject(child);
				std::cout << "TTT" << std::endl;
			}
			ImGui::TreePop();
		}*/
	}
}

void Engine::DrawHierarchy()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Hierarchy");
	//ImGui::SetWindowFontScale(2);

	ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->parent == nullptr)
		{
			DrawTreeItem(gameObjects[i]);
		}
	}
	ImGui::EndChild();


	ImGui::End();
}

/// <summary>
/// Update all components
/// </summary>
void Engine::UpdateComponents()
{
	//Update all gameobjects components
	if (componentsListDirty)
	{
		componentsListDirty = false;
		orderedComponents.clear();

		std::vector<Component*> orderedComponentsToInit;
		componentsCount = 0;
		int componentsToInitCount = 0;
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			GameObject* gameObjectToCheck = gameObjects[gIndex];
			if (gameObjectToCheck->GetActive())
			{
				int componentCount = (int)gameObjectToCheck->components.size();
				bool placeFound = false;
				for (int cIndex = 0; cIndex < componentCount; cIndex++)
				{
					Component* componentToCheck = gameObjectToCheck->components[cIndex];

					if (componentToCheck->GetIsEnabled())
					{
						for (int i = 0; i < componentsCount; i++)
						{
							//Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->updatePriority <= orderedComponents[i]->updatePriority)
							{
								orderedComponents.insert(orderedComponents.begin() + i, componentToCheck);
								placeFound = true;
								break;
							}
						}
						//if the priority is lower than all components's priorities in the list, add it the end of the list
						if (!placeFound)
						{
							orderedComponents.push_back(componentToCheck);
						}
						componentsCount++;
					}
				}
			}
		}

		//Find uninitiated components and order them
		for (int i = 0; i < componentsCount; i++)
		{
			Component* componentToCheck = orderedComponents[i];
			if (!orderedComponents[i]->initiated)
			{
				bool placeFound = false;
				for (int componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
				{
					//Check if the checked has a higher priority (lower value) than the component in the list
					if (componentToCheck->updatePriority <= orderedComponentsToInit[componentToInitIndex]->updatePriority)
					{
						orderedComponentsToInit.insert(orderedComponentsToInit.begin() + componentToInitIndex, componentToCheck);
						placeFound = true;
						break;
					}
				}
				//if the priority is lower than all components's priorities in the list, add it the end of the list
				if (!placeFound)
				{
					orderedComponentsToInit.push_back(componentToCheck);
				}
				componentsToInitCount++;
			}
		}

		//Init components
		for (int i = 0; i < componentsToInitCount; i++)
		{
			orderedComponentsToInit[i]->Awake();
		}

		for (int i = 0; i < componentsToInitCount; i++)
		{
			orderedComponentsToInit[i]->Start();
			orderedComponentsToInit[i]->initiated = true;
		}
	}
	//Update components
	for (int i = 0; i < componentsCount; i++)
	{
		orderedComponents[i]->Update();
	}
}

void Engine::SetSelectedGameObject(GameObject* newSelected)
{
	selectedGameObject = newSelected;
}

/// <summary>
/// Engine loop
/// </summary>
void Engine::Loop()
{
	Debug::Print("Initiating game...");
	Game* game = new Game();
	//Game3D* game = new Game3D();
	//PathFinding* game = new PathFinding();
	game->Init();
	Debug::Print("---- Game initiated ----");

	bool running = true;

	while (running)
	{
		engineLoopBenchmark->Start();

		Time::UpdateTime();

		SDL_Event event;
		InputSystem::ClearInputs();
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			InputSystem::Read(event);
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					Window::OnResize(event.window.data1, event.window.data2);
				}
				break;
			default:
				break;
			}
		}

		//Clear the OpenGL window
		glClearColor(0.529f, 0.808f, 0.922f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set wireframe
		if (EngineSettings::isWireframe)
		{
			glLineWidth(2);
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT, GL_FILL);
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		gameLoopBenchmark->Start();
		game->Loop();
		gameLoopBenchmark->Stop();

		componentsUpdateBenchmark->Start();
		UpdateComponents();
		componentsUpdateBenchmark->Stop();

		AssetManager::ResetMaterialsUpdates();

		drawIDrawablesBenchmark->Start();
		Graphics::DrawAllDrawable();
		drawIDrawablesBenchmark->Stop();
		glPolygonMode(GL_FRONT, GL_FILL);

		/*if (InputSystem::GetKeyDown(A))
		{
			EngineSettings::isWireframe = !EngineSettings::isWireframe;
		}*/

		std::string debugText = std::string("Wireframe (A): ") + (EngineSettings::isWireframe ? "True" : "False");

		std::string fpsText = std::to_string((int)(1 / Time::GetUnscaledDeltaTime())) + " fps";

		//Draw screen tester
		/*UiManager::RenderTextCanvas("Left", 0, 0.5, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Right", 1, 0.5, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Center, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Top Left", 0, 0, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Bottom, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Top Right", 1, 0, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Bottom, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Bottom Left", 0, 1, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Top, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Bottom Right", 1, 1, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Top, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Center", 0.5, 0.5, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Center, V_Center, *AssetManager::GetShader(7));*/

		DrawProfiler();
		DrawInspector();
		DrawHierarchy();

		Vector4 lineColor = Vector4(0.5f, 1.0f, 0.2f, 1.0f);

		/*Vector2 arrowStart = Vector2(2, 0);
		Vector2 arrowEnd = Vector2(0, -2);
		Vector2 dir = (arrowEnd - arrowStart).normalize();
		float angle = atan2(dir.x, dir.y);
		Vector2 leftEnd = arrowEnd - dir / 2 + Vector2(-cos(angle) /4, sin(angle) / 4);
		Vector2 rightEnd = arrowEnd - dir / 2 + Vector2(cos(angle) / 4, -sin(angle) / 4);

		SpriteManager::Render2DLine(arrowStart, arrowEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard")); //Line
		SpriteManager::Render2DLine(arrowEnd, leftEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard"));//Left
		SpriteManager::Render2DLine(arrowEnd, rightEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard"));//Right*/

		//SpriteManager::Render2DLine(arrowEnd, arrowEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard"));

		/*SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, 2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, 0), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, -2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));

		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(-2, 2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(-2, 0), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(-2, -2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));

		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(0, -2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(0, 2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));*/


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		engineLoopBenchmark->Stop();
		Window::UpdateScreen();
		Performance::Update();
		Performance::ResetCounters();
	}
	delete game;
}

/// <summary>
/// 
/// </summary>
/// <param name="gameObject"></param>
void Engine::AddGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
	gameObjectCount++;
	//componentsListDirty = true;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
std::vector<GameObject*> Engine::GetGameObjects()
{
	return std::vector<GameObject*>(gameObjects);
}
