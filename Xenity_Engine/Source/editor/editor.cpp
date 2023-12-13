#if defined(EDITOR)

#include "editor.h"
#include <engine/engine.h>
#include <engine/audio/audio_source.h>
#include <engine/graphics/texture.h>

#include <editor/ui/menus/project_settings_menu.h>
#include <editor/ui/menus/engine_settings_menu.h>
#include <editor/ui/menus/file_explorer_menu.h>
#include <editor/ui/menus/hierarchy_menu.h>
#include <editor/ui/menus/inspector_menu.h>
#include <editor/ui/menus/main_bar_menu.h>
#include <editor/ui/menus/profiler_menu.h>
#include <editor/ui/menus/game_menu.h>
#include <editor/ui/menus/scene_menu.h>
#include <editor/ui/menus/compiling_menu.h>
#include <editor/ui/menus/select_project_menu.h>
#include <editor/ui/menus/create_project_menu.h>
#include <editor/ui/menus/lighting_menu.h>
#include <editor/ui/menus/create_class_menu.h>
#include <editor/compiler.h>

#include <functional>
#include <engine/class_registry/class_registry.h>
#include <engine/file_system/file_reference.h>
#include <engine/file_system/mesh_loader/wavefront_loader.h>
#include <engine/asset_management/project_manager.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/scene_management/scene_manager.h>
#include <editor/command/command_manager.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/inputs/input_system.h>
#include <editor/ui/editor_ui.h>
#include <engine/file_system/file_system.h>
#include <engine/time/time.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/file_system/file.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/engine_settings.h>
#include "file_handler.h"
#include <imgui/imgui_internal.h>
#include <engine/debug/debug.h>

using json = nlohmann::json;

std::weak_ptr<GameObject> Editor::cameraGO;
std::weak_ptr<AudioSource> Editor::audioSource;
std::shared_ptr <ProjectDirectory> Editor::currentProjectDirectory = nullptr;

MenuNames Editor::currentMenu = Menu_Select_Project;

ProjectSettingsMenu* Editor::projectSettings = nullptr;
EngineSettingsMenu* Editor::engineSettings = nullptr;
FileExplorerMenu* Editor::fileExplorer = nullptr;
HierarchyMenu* Editor::hierarchy = nullptr;
InspectorMenu* Editor::inspector = nullptr;
MainBarMenu* Editor::mainBar = nullptr;
ProfilerMenu* Editor::profiler = nullptr;
GameMenu* Editor::gameMenu = nullptr;
SceneMenu* Editor::sceneMenu = nullptr;
CompilingMenu* Editor::compilingMenu = nullptr;
SelectProjectMenu* Editor::selectProjectMenu = nullptr;
CreateProjectMenu* Editor::createProjectMenu = nullptr;
LightingMenu* Editor::lightingMenu = nullptr;
CreateClassMenu* Editor::createClassMenu = nullptr;

std::weak_ptr<GameObject> Editor::selectedGameObject;
std::shared_ptr<FileReference> Editor::selectedFileReference = nullptr;

std::shared_ptr <MeshData> Editor::rightArrow = nullptr;
std::shared_ptr <MeshData> Editor::upArrow = nullptr;
std::shared_ptr <MeshData> Editor::forwardArrow = nullptr;
std::shared_ptr <Texture> Editor::toolArrowsTexture = nullptr;
bool Editor::startRotatingCamera = false;

std::vector<std::string> Editor::dragdropEntries;

void Editor::Init()
{
	CreateMenus();

	// Create scene camera TODO : Move to scene_menu.cpp?
	cameraGO = CreateGameObjectEditor("Camera");
	std::shared_ptr<Camera> camera = cameraGO.lock()->AddComponent<Camera>();
	camera->SetNearClippingPlane(0.01f);
	camera->SetFarClippingPlane(500);
	camera->SetProjectionSize(5.0f);
	camera->SetFov(70);
	camera->isEditor = true;
	camera->GetTransform()->SetPosition(Vector3(0, 1, 0));

	// Create audio source for audio clip preview
	std::shared_ptr<GameObject> audioSourceGO = CreateGameObjectEditor("AudioSource");
	audioSource = audioSourceGO->AddComponent<AudioSource>();

	// Load Assets
	rightArrow = MeshManager::LoadMesh("engine_assets\\right_arrow.obj");
	upArrow = MeshManager::LoadMesh("engine_assets\\up_arrow.obj");
	forwardArrow = MeshManager::LoadMesh("engine_assets\\forward_arrow.obj");

	toolArrowsTexture = Texture::MakeTexture();
	toolArrowsTexture->file = FileSystem::MakeFile("engine_assets\\tool_arrows_colors.png");
	toolArrowsTexture->SetFilter(Texture::Point);
	toolArrowsTexture->LoadFileReference();
}

void Editor::OnWindowFocused()
{
	if (ProjectManager::GetIsProjectLoaded())
	{
		bool needRefresh = FileHandler::HasFileChangedOrAdded(ProjectManager::GetAssetFolderPath());
		if (needRefresh)
		{
			ProjectManager::RefreshProjectDirectory();
		}
		bool needCompile = FileHandler::HasCodeChanged(ProjectManager::GetAssetFolderPath());
		if (needCompile && EngineSettings::compileOnCodeChanged)
		{
			Compiler::HotReloadGame();
		}
	}
}

void Editor::Update()
{
	if (ProjectManager::GetIsProjectLoaded())
	{
		// Check user input and camera movement when in the scene menu
		if (InputSystem::GetKeyUp(MOUSE_RIGHT))
		{
			startRotatingCamera = false;
		}
		if (sceneMenu->isFocused)
		{
			// Get camera transform
			std::shared_ptr<Transform> cameraTransform = cameraGO.lock()->GetTransform();
			Vector3 rot = cameraTransform->GetRotation();
			Vector3 pos = cameraTransform->GetPosition();

			if (InputSystem::GetKeyDown(MOUSE_RIGHT) && sceneMenu->isHovered)
			{
				startRotatingCamera = true;
			}

			// Rotate the camera when dragging the mouse right click
			if (InputSystem::GetKey(MOUSE_RIGHT) && startRotatingCamera)
			{
				rot.x += -InputSystem::mouseSpeed.y * 70;
				rot.y += InputSystem::mouseSpeed.x * 70;
			}

			// Move the camera when using keyboard's arrows
			float fwd = 0;
			float up = 0;
			float side = 0;

			if (InputSystem::GetKey(UP) || InputSystem::GetKey(Z))
				fwd = -1 * Time::GetDeltaTime();
			else if (InputSystem::GetKey(DOWN) || InputSystem::GetKey(S))
				fwd = 1 * Time::GetDeltaTime();

			if (InputSystem::GetKey(RIGHT) || InputSystem::GetKey(D))
				side = 1 * Time::GetDeltaTime();
			else if (InputSystem::GetKey(LEFT) || InputSystem::GetKey(Q))
				side = -1 * Time::GetDeltaTime();

			if (InputSystem::GetKey(MOUSE_MIDDLE))
			{
				up += InputSystem::InputSystem::mouseSpeed.y * 1.5f;
				side -= InputSystem::InputSystem::mouseSpeed.x * 1.5f;
			}

			// Move the camera when using the mouse's wheel (Do not use delta time)
			if (sceneMenu->isHovered)
				fwd -= InputSystem::mouseWheel / 15.0f;

			// Apply values
			pos -= cameraTransform->GetForward() * (fwd / 7.0f) * 30;
			pos -= cameraTransform->GetLeft() * (side / 7.0f) * 30;
			pos -= cameraTransform->GetUp() * (up / 7.0f) * 30;

			cameraTransform->SetPosition(pos);
			cameraTransform->SetRotation(rot);
		}

		//------- Check shortcuts

		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(Z)))
		{
			CommandManager::Undo();
		}
		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(Y)))
		{
			CommandManager::Redo();
		}

		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(D)))
		{
			if (selectedGameObject.lock())
				DuplicateGameObject(selectedGameObject.lock());
		}

		if (InputSystem::GetKey(DELETE))
		{
			if (sceneMenu->isFocused || hierarchy->isFocused)
			{
				if (selectedGameObject.lock())
					Destroy(selectedGameObject.lock());
			}
		}

		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKey(LEFT_SHIFT) && InputSystem::GetKeyDown(P))) // Pause / UnPause game
		{
			if (GameplayManager::GetGameState() == GameState::Playing)
			{
				GameplayManager::SetGameState(GameState::Paused, true);
			}
			else if (GameplayManager::GetGameState() == GameState::Paused)
			{
				GameplayManager::SetGameState(GameState::Playing, true);
			}
		}
		else if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(P))) // Start / Stop game
		{
			if (GameplayManager::GetGameState() == GameState::Stopped)
			{
				GameplayManager::SetGameState(GameState::Playing, true);
			}
			else
			{
				GameplayManager::SetGameState(GameState::Stopped, true);
			}
		}


		if (GameplayManager::GetGameState() == GameState::Stopped)
		{
			if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(S)))
			{
				SceneManager::SaveScene(SaveSceneToFile);
			}
		}
	}
}

void Editor::Draw()
{
	EditorUI::NewFrame();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	if (currentMenu == Menu_Editor)
		mainBar->Draw();

	float offset = mainBar->GetHeight();
	if (currentMenu != Menu_Editor)
		offset = 0;

	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset));
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGuiID dsId = ImGui::GetID("BackgroundDock");
	ImGuiDockNode* first_time = ImGui::DockBuilderGetNode(dsId);
	if (!first_time)
	{
		ImGui::DockBuilderRemoveNode(dsId);
		ImGui::DockBuilderAddNode(dsId, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGuiID inspectorNode;
		ImGuiID fileExplorerNode;
		ImGuiID SceneNode;
		ImGuiID hierarchyNode;

		ImGuiID left;
		ImGuiID leftTop;

		ImGui::DockBuilderSplitNode(dsId, ImGuiDir_Left, 0.8f, &left, &inspectorNode);
		ImGui::DockBuilderSplitNode(left, ImGuiDir_Up, 0.7f, &leftTop, &fileExplorerNode);
		ImGui::DockBuilderSplitNode(leftTop, ImGuiDir_Left, 0.2f, &hierarchyNode, &SceneNode);

		ImGui::DockBuilderDockWindow("Hierarchy", hierarchyNode);
		ImGui::DockBuilderDockWindow("File Explorer", fileExplorerNode);
		ImGui::DockBuilderDockWindow("Inspector", inspectorNode);
		ImGui::DockBuilderDockWindow("Debug", inspectorNode);
		ImGui::DockBuilderDockWindow("###Scene", SceneNode);
		ImGui::DockBuilderDockWindow("###Game", SceneNode);

		ImGui::DockBuilderFinish(dsId);
	}
	ImGui::DockSpace(dsId);

	if (currentMenu == Menu_Create_Project)
	{
		createProjectMenu->Draw();
	}
	else if (currentMenu == Menu_Select_Project)
	{
		selectProjectMenu->Draw();
	}
	else
	{
		EditorUI::SetRoundedCorner(10);
		if (EditorUI::showProjectsSettings)
		{
			projectSettings->Draw();
		}
		if (EditorUI::showEngineSettings)
		{
			engineSettings->Draw();
		}
		if (EditorUI::showEditor)
		{
			fileExplorer->Draw();
			hierarchy->Draw();
			inspector->Draw();
		}
		//mainBar->Draw();
		if (EditorUI::showProfiler)
		{
			profiler->Draw();
		}
		if (EditorUI::showCreateClass)
		{
			createClassMenu->Draw();
		}
		compilingMenu->Draw();
		gameMenu->Draw();
		sceneMenu->Draw();
		if (EditorUI::showLightingSettings)
		{
			lightingMenu->Draw();
		}
	}
	ImGui::PopStyleVar();
	ImGui::End();
	ImGui::PopStyleVar();
	EditorUI::Render();
}

void Editor::CreateEmpty()
{
	auto command = std::make_shared<InspectorCreateGameObjectCommand>(std::weak_ptr<GameObject>(), 0);
	CommandManager::AddCommand(command);
	command->Execute();
}

void Editor::CreateEmptyChild()
{
	auto command = std::make_shared<InspectorCreateGameObjectCommand>(selectedGameObject, 1);
	CommandManager::AddCommand(command);
	command->Execute();
}

void Editor::CreateEmptyParent()
{
	auto command = std::make_shared<InspectorCreateGameObjectCommand>(selectedGameObject, 2);
	CommandManager::AddCommand(command);
	command->Execute();
}

void Editor::SetSelectedFileReference(const std::shared_ptr<FileReference>& fileReference)
{
	selectedFileReference = fileReference;
#if  defined(EDITOR)
	Editor::inspector->loadedPreview = nullptr;
#endif
}

std::shared_ptr<FileReference> Editor::GetSelectedFileReference()
{
	return selectedFileReference;
}

void Editor::SetSelectedGameObject(const std::shared_ptr<GameObject>& newSelected)
{
	selectedGameObject = newSelected;
}

std::shared_ptr<GameObject> Editor::GetSelectedGameObject()
{
	return selectedGameObject.lock();
}

void Editor::SetCurrentProjectDirectory(std::shared_ptr <ProjectDirectory> dir)
{
	if (currentProjectDirectory)
		currentProjectDirectory->files.clear();
	currentProjectDirectory = dir;
	if (currentProjectDirectory)
	{
		ProjectManager::FillProjectDirectory(currentProjectDirectory);
		size_t itemCount = currentProjectDirectory->files.size();
		for (size_t i = 0; i < itemCount; i++)
		{
			currentProjectDirectory->files[i]->LoadFileReference();
		}
	}
}

std::shared_ptr <ProjectDirectory> Editor::GetCurrentProjectDirectory()
{
	return currentProjectDirectory;
}

void Editor::DuplicateGameObject(const std::shared_ptr<GameObject>& goToDuplicate)
{
	if (goToDuplicate)
	{
		// Create new gameobject
		std::shared_ptr<GameObject> newGameObject = CreateGameObject(GetIncrementedGameObjectName(goToDuplicate->name));

		// Set parent and position/rotation/scale
		if (goToDuplicate->parent.lock())
		{
			newGameObject->SetParent(goToDuplicate->parent);
		}
		std::shared_ptr<Transform> newTransform = newGameObject->GetTransform();
		std::shared_ptr<Transform> transformToDuplicate = goToDuplicate->GetTransform();
		newTransform->SetLocalPosition(transformToDuplicate->GetLocalPosition());
		newTransform->SetLocalRotation(transformToDuplicate->GetLocalRotation());
		newTransform->SetLocalScale(transformToDuplicate->GetLocalScale());

		// Duplicate all components
		size_t componentCount = goToDuplicate->components.size();
		for (size_t i = 0; i < componentCount; i++)
		{
			std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(goToDuplicate->components[i]->GetComponentName(), newGameObject);
			auto newReflection = newComponent->GetReflectiveData();
			auto reflectionToCopy = goToDuplicate->components[i]->GetReflectiveData();

			json copiedValues;
			copiedValues["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectionToCopy);
			ReflectionUtils::JsonToReflectiveData(copiedValues, newReflection);
			newComponent->OnReflectionUpdated();
		}
		SetSelectedGameObject(newGameObject);
	}
}

std::shared_ptr<File> Editor::CreateNewFile(const std::string& fileName, FileType type, bool fillWithDefaultData)
{
	std::string fileExt = "";
	switch (type)
	{
	case File_Scene:
		fileExt = ".xen";
		break;
	case File_Skybox:
		fileExt = ".sky";
		break;
	case File_Code:
		fileExt = ".cpp";
		break;
	case File_Header:
		fileExt = ".h";
		break;
	case File_Material:
		fileExt = ".mat";
		break;
	case File_Shader:
		fileExt = ".shader";
		break;
	}

	std::shared_ptr<File> newFile = FileSystem::MakeFile(fileName + fileExt);
	int id = 0;
	while (newFile->CheckIfExist())
	{
		id++;
		newFile = FileSystem::MakeFile(fileName + " (" + std::to_string(id) + ")" + fileExt);
	}

	if (newFile->Open(true))
	{
		if (fillWithDefaultData)
			newFile->Write(AssetManager::GetDefaultFileData(type));
		newFile->Close();
	}

	ProjectManager::RefreshProjectDirectory();

	return newFile;
}

void Editor::OpenExplorerWindow(std::string path, bool isSelected)
{
	std::string command = "explorer.exe ";
	if (isSelected)
	{
		command += "/select, \"";
	}
	else
	{
		command += "\"";
	}

	command += path;

	command += "\"";

	system(command.c_str());
}

void Editor::AddDragAndDrop(const std::string& path)
{
	dragdropEntries.push_back(path);
}

void Editor::StartFolderCopy(std::string path, std::string newPath)
{
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		// Check is file
		if (!file.is_regular_file())
		{
			std::string newFolderPath = newPath + file.path().filename().string() + '\\';
			FileSystem::fileSystem->CreateDirectory(newFolderPath);
			StartFolderCopy(file.path().string() + '\\', newFolderPath);
		}
		else 
		{
			FileSystem::fileSystem->CopyFile(file.path().string(), newPath + file.path().filename().string(), true); // TODO ask if we want to replace files
		}
	}
}

void Editor::GetIncrementedGameObjectNameInfo(const std::string& name, std::string& baseName, int& number)
{
	int endParenthesis = -1;
	int startParenthesis = -1;
	int nameLenght = name.size();
	int numberState = 2; // 0 Other than number, 1 only number, 2 nothing found

	for (int i = nameLenght - 1; i > 0; i--)
	{
		if (name[i] == ')')
		{
			if (endParenthesis == -1 && startParenthesis == -1)
				endParenthesis = i;
			else
				break;
		}
		else if (name[i] == '(')
		{
			if (endParenthesis == -1 || numberState != 1)
				break;

			if (startParenthesis == -1)
			{
				if (name[i - 1] == ' ')
				{
					startParenthesis = i;
				}
			}
			break;
		}
		else
		{
			if (!isdigit(name[i]))
			{
				numberState = 0;
				break;
			}
			else
			{
				numberState = 1;
			}
		}
	}

	if (startParenthesis != -1)
	{
		std::string t = name.substr(startParenthesis + 1, endParenthesis - startParenthesis - 1);
		number = std::stoi(name.substr(startParenthesis + 1, endParenthesis - startParenthesis - 1)) + 1;
		baseName = name.substr(0, startParenthesis-1);
	}
	else
	{
		baseName = name;
		number = 1;
	}
}

std::string Editor::GetIncrementedGameObjectName(std::string name)
{
	std::string finalName = "";
	int number = 1;
	int foundNumberLocation = -1;
	int endParenthesis = -1;
	int startParenthesis = -1;
	int numberState = 2; // 0 Other than number, 1 only number, 2 nothing found

	GetIncrementedGameObjectNameInfo(name, finalName, number);

	int gameObjectCount = GameplayManager::gameObjects.size();
	for (int i = 0; i < gameObjectCount; i++)
	{
		std::string tempName;
		int tempNumber;
		GetIncrementedGameObjectNameInfo(GameplayManager::gameObjects[i]->name, tempName, tempNumber);
		if (tempName == finalName) 
		{
			if(number < tempNumber)
				number = tempNumber;
		}
	}
	
	finalName = finalName + " (" + std::to_string(number) + ")";

	return finalName;
}

void Editor::OnDragAndDropFileFinished()
{
	int size = dragdropEntries.size();
	for (int dragIndex = 0; dragIndex < size; dragIndex++)
	{
		try
		{
			std::string& path = dragdropEntries[dragIndex];
			bool isDirectory = std::filesystem::is_directory(path);
			int pathSize = path.size();

			// Find the last backslash
			int lastBackSlash = -1;
			for (int textIndex = pathSize; textIndex > 0; textIndex--)
			{
				if (path[textIndex] == '\\')
				{
					lastBackSlash = textIndex;
					break;
				}
			}
			// Remove the parent's path of the file/folder
			std::string newPath = Editor::GetCurrentProjectDirectory()->path + path.substr(lastBackSlash + 1);

			if (isDirectory) 
			{
				FileSystem::fileSystem->CreateDirectory(newPath + '\\');
				StartFolderCopy(dragdropEntries[dragIndex] + '\\', newPath + '\\');
			}
			else 
			{
				int copyResult = FileSystem::fileSystem->CopyFile(path, newPath, false);
				if (copyResult == -1) 
				{
					DialogResult result = EditorUI::OpenDialog("File copy error", "This file already exists in this location.\nDo you want to replace it?", Dialog_Type_YES_NO_CANCEL);
					if (result == Dialog_YES) 
					{
						FileSystem::fileSystem->CopyFile(path, newPath, true);
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			Debug::PrintError(e.what());
		}
	}

	dragdropEntries.clear();
	ProjectManager::RefreshProjectDirectory();
}

void Editor::CreateMenus()
{
	projectSettings = new ProjectSettingsMenu();
	engineSettings = new EngineSettingsMenu();
	fileExplorer = new FileExplorerMenu();
	hierarchy = new HierarchyMenu();
	inspector = new InspectorMenu();
	mainBar = new MainBarMenu();
	profiler = new ProfilerMenu();
	gameMenu = new GameMenu();
	sceneMenu = new SceneMenu();
	compilingMenu = new CompilingMenu();
	selectProjectMenu = new SelectProjectMenu();
	createProjectMenu = new CreateProjectMenu();
	lightingMenu = new LightingMenu();
	createClassMenu = new CreateClassMenu();

	projectSettings->Init();
	engineSettings->Init();
	fileExplorer->Init();
	hierarchy->Init();
	inspector->Init();
	mainBar->Init();
	profiler->Init();
	gameMenu->Init();
	sceneMenu->Init();
	selectProjectMenu->Init();
	createProjectMenu->Init();
	lightingMenu->Init();
	createClassMenu->Init();
}

#endif