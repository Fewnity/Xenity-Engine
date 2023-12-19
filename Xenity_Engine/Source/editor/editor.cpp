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
#include <editor/ui/menus/about_menu.h>
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

std::weak_ptr<AudioSource> Editor::audioSource;
std::shared_ptr <ProjectDirectory> Editor::currentProjectDirectory = nullptr;

MenuGroup Editor::currentMenu = Menu_Select_Project;

std::vector<std::shared_ptr<Menu>> Editor::menus;

std::shared_ptr <MainBarMenu> Editor::mainBar = nullptr;

std::weak_ptr<GameObject> Editor::selectedGameObject;
std::shared_ptr<FileReference> Editor::selectedFileReference = nullptr;

std::shared_ptr <MeshData> Editor::rightArrow = nullptr;
std::shared_ptr <MeshData> Editor::upArrow = nullptr;
std::shared_ptr <MeshData> Editor::forwardArrow = nullptr;
std::shared_ptr <Texture> Editor::toolArrowsTexture = nullptr;

std::vector<std::string> Editor::dragdropEntries;
int Editor::menuCount = 0;

void Editor::Init()
{
	CreateMenus();

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

		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(N)))
		{
			CreateEmpty();
		}

		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(NUM_1)))
		{
			auto sceneMenu = Editor::GetMenu<SceneMenu>();
			sceneMenu->Focus();
		}

		if ((InputSystem::GetKey(LEFT_SHIFT) && InputSystem::GetKeyDown(D)))
		{
			SetSelectedGameObject(nullptr);
			SetSelectedFileReference(nullptr);
		}

		if (InputSystem::GetKey(DELETE))
		{
			auto sceneMenu = Editor::GetMenu<SceneMenu>();
			auto hierarchy = Editor::GetMenu<HierarchyMenu>();
			if (sceneMenu->IsFocused() || hierarchy->IsFocused())
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

	ApplyEditorStyle();
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

		ImGui::DockBuilderDockWindow("###Hierarchy0", hierarchyNode);
		ImGui::DockBuilderDockWindow("###File_Explorer0", fileExplorerNode);
		ImGui::DockBuilderDockWindow("###Inspector0", inspectorNode);
		ImGui::DockBuilderDockWindow("###Debug0", inspectorNode);
		ImGui::DockBuilderDockWindow("###Scene0", SceneNode);
		ImGui::DockBuilderDockWindow("###Game0", SceneNode);

		ImGui::DockBuilderFinish(dsId);
	}
	ImGui::DockSpace(dsId);

	//int menuCount = menus.size();
	for (int i = 0; i < menuCount; i++)
	{
		if (menus[i]->GetActive() && menus[i]->group == currentMenu)
			menus[i]->Draw();
	}

	ImGui::PopStyleVar();
	ImGui::End();

	RemoveEditorStyle();
	EditorUI::Render();
}

void Editor::ApplyEditorStyle()
{
	//Default colors
	// BG 15 15 15
	// Input 29 47 73
	// tab 31 57 88
	// tab selected 51 105 173
	// Button 35 69 109
	// Element selected 56 123 203

	Vector4 bgColor = EngineSettings::backbgroundColor.GetRGBA().ToVector4();
	Vector4 secondaryColor = EngineSettings::secondaryColor.GetRGBA().ToVector4();
	Vector4 playTint = EngineSettings::playTintColor.GetRGBA().ToVector4();
	Vector4 pTint = Vector4(0);

	if (GameplayManager::GetGameState() == Playing) 
		pTint = playTint;
	else if (!EngineSettings::isPlayTintAdditive)
		pTint = Vector4(1);

	if (EngineSettings::isPlayTintAdditive)
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(bgColor.x + pTint.x, bgColor.y + pTint.y, bgColor.z + pTint.z, 1));
	else
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(bgColor.x * pTint.x, bgColor.y * pTint.y, bgColor.z * pTint.z, 1));

	ImVec4 colorLevel0 = ImVec4(secondaryColor.x, secondaryColor.y, secondaryColor.z, 1);
	ImVec4 colorLevel05 = ImVec4(secondaryColor.x - 0.05f, secondaryColor.y - 0.05f, secondaryColor.z - 0.05f, 1);
	ImVec4 colorLevel1 = ImVec4(secondaryColor.x - 0.1f, secondaryColor.y - 0.1f, secondaryColor.z - 0.1f, 1);
	ImVec4 colorLevel15 = ImVec4(secondaryColor.x - 0.15f, secondaryColor.y - 0.15f, secondaryColor.z - 0.15f, 1);
	ImVec4 colorLevel2 = ImVec4(secondaryColor.x - 0.2f, secondaryColor.y - 0.2f, secondaryColor.z - 0.2f, 1);
	ImVec4 colorLevel25 = ImVec4(secondaryColor.x - 0.25f, secondaryColor.y - 0.25f, secondaryColor.z - 0.25f, 1);
	ImVec4 colorLevel3 = ImVec4(secondaryColor.x - 0.3f, secondaryColor.y - 0.3f, secondaryColor.z - 0.3f, 1);
	ImVec4 colorLevel4 = ImVec4(secondaryColor.x - 0.4f, secondaryColor.y - 0.4f, secondaryColor.z - 0.4f, 1);
	ImVec4 colorLevel4Alpha = ImVec4(secondaryColor.x - 0.4f, secondaryColor.y - 0.4f, secondaryColor.z - 0.4f, 0.5f);

	ImGui::PushStyleColor(ImGuiCol_Button, colorLevel25);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorLevel1);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorLevel0);

	ImGui::PushStyleColor(ImGuiCol_Tab, colorLevel2);
	ImGui::PushStyleColor(ImGuiCol_TabActive, colorLevel1);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, colorLevel0);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, colorLevel3);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, colorLevel2);

	ImGui::PushStyleColor(ImGuiCol_Header, colorLevel1);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, colorLevel05);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorLevel0);

	ImGui::PushStyleColor(ImGuiCol_TitleBg, colorLevel4);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, colorLevel3);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, colorLevel4Alpha);

	ImGui::PushStyleColor(ImGuiCol_CheckMark, colorLevel0);

	//ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(secondaryColor.x - 0.4, secondaryColor.y - 0.4, secondaryColor.z - 0.4, 0.5f));
	//ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(secondaryColor.x - 0.4, secondaryColor.y - 0.4, secondaryColor.z - 0.4, 0.5f));
}

void Editor::RemoveEditorStyle()
{
	ImGui::PopStyleVar();

	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
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
	auto inspector = Editor::GetMenu<InspectorMenu>();
	if (inspector)
		inspector->loadedPreview = nullptr;
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
		number = std::stoi(name.substr(startParenthesis + 1, endParenthesis - startParenthesis - 1)) + 1;
		baseName = name.substr(0, startParenthesis - 1);
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
	bool foundOne = false;
	GetIncrementedGameObjectNameInfo(name, finalName, number);

	int gameObjectCount = GameplayManager::gameObjects.size();
	for (int i = 0; i < gameObjectCount; i++)
	{
		std::string tempName;
		int tempNumber;
		GetIncrementedGameObjectNameInfo(GameplayManager::gameObjects[i]->name, tempName, tempNumber);
		if (tempName == finalName)
		{
			foundOne = true;
			if (number < tempNumber)
				number = tempNumber;
		}
	}

	if(foundOne)
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
	AddMenu<CreateClassMenu>(false);
	AddMenu<LightingMenu>(false);
	AddMenu<ProjectSettingsMenu>(false);
	AddMenu<EngineSettingsMenu>(false);
	AddMenu<AboutMenu>(false);

	AddMenu<FileExplorerMenu>(true);
	AddMenu<HierarchyMenu>(true);
	AddMenu<InspectorMenu>(true);
	AddMenu<ProfilerMenu>(true);
	AddMenu<GameMenu>(true);
	AddMenu<SceneMenu>(true);
	AddMenu<CompilingMenu>(true);
	AddMenu<SelectProjectMenu>(true);
	AddMenu<CreateProjectMenu>(true);

	mainBar = std::make_shared<MainBarMenu>();
	mainBar->Init();
}

#endif