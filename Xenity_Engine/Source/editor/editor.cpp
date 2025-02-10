// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

#include "editor.h"

#include <filesystem>
#include <thread>

#include <imgui/imgui_internal.h>

#include <editor/ui/menus/hierarchy_menu.h>
#include <editor/ui/menus/inspector_menu.h>
#include <editor/ui/menus/main_bar_menu.h>
#include <editor/ui/menus/scene_menu.h>
#include <editor/ui/menus/bottom_bar_menu.h>
#include <editor/compilation/compiler.h>
#include <editor/command/command_manager.h>
#include <editor/ui/editor_ui.h>
#include <editor/update_checker/update_checker.h>
#include "command/commands/delete.h"
#include "command/commands/create.h"
#include "file_handler/file_handler.h"

#include <engine/engine.h>
#include <engine/audio/audio_source.h>
#include <engine/graphics/texture.h>
#include <engine/class_registry/class_registry.h>
#include <engine/file_system/file_reference.h>
#include <engine/file_system/mesh_loader/wavefront_loader.h>
#include <engine/asset_management/project_manager.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/inputs/input_system.h>
#include <engine/file_system/file_system.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/file_system/file.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/engine_settings.h>
#include <engine/debug/debug.h>
#include <engine/event_system/event_system.h>
#include <engine/debug/stack_debug_object.h>
#include "ui/menus/update_available_menu.h"
using json = nlohmann::json;

std::weak_ptr<AudioSource> Editor::audioSource;
std::shared_ptr <ProjectDirectory> Editor::currentProjectDirectory = nullptr;

MenuGroup Editor::currentMenu = MenuGroup::Menu_Select_Project;

std::vector<std::shared_ptr<Menu>> Editor::menus;
std::weak_ptr <Menu> Editor::lastFocusedGameMenu;

std::shared_ptr <MainBarMenu> Editor::mainBar = nullptr;
std::shared_ptr <BottomBarMenu> Editor::bottomBar = nullptr;

std::vector <std::weak_ptr<GameObject>> Editor::selectedGameObjects;
std::shared_ptr<FileReference> Editor::selectedFileReference = nullptr;

std::shared_ptr <MeshData> Editor::rightArrow = nullptr;
std::shared_ptr <MeshData> Editor::upArrow = nullptr;
std::shared_ptr <MeshData> Editor::forwardArrow = nullptr;
std::shared_ptr <MeshData> Editor::rotationCircleX = nullptr;
std::shared_ptr <MeshData> Editor::rotationCircleY = nullptr;
std::shared_ptr <MeshData> Editor::rotationCircleZ = nullptr;
std::shared_ptr <Texture> Editor::toolArrowsTexture = nullptr;

std::vector<std::string> Editor::dragdropEntries;

Editor::MenuSettings Editor::menuSettings;

int Editor::menuCount = 0;
bool Editor::isToolLocalMode;
Event<bool>* Editor::onUpdateCheckedEvent = new Event<bool>();
bool Editor::updateAvailable = false;
float Editor::cameraSpeed = 30;

void Editor::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	ClassRegistry::RegisterMenus();
	LoadMenuSettings();
	SaveMenuSettings();
	CreateMenus();

	onUpdateCheckedEvent->Bind(&OnUpdateChecked);
	std::thread thread(&UpdateChecker::CheckForUpdate, onUpdateCheckedEvent);
	thread.detach();

	// Create audio source for audio clip preview
	std::shared_ptr<GameObject> audioSourceGO = CreateGameObjectEditor("AudioSource");
	audioSource = audioSourceGO->AddComponent<AudioSource>();
	audioSource.lock()->m_isEditor = true;

	// Load Assets
	rightArrow = MeshManager::LoadMesh("engine_assets/right_arrow.obj");
	upArrow = MeshManager::LoadMesh("engine_assets/up_arrow.obj");
	forwardArrow = MeshManager::LoadMesh("engine_assets/forward_arrow.obj");

	rotationCircleX = MeshManager::LoadMesh("engine_assets/rotation_circleX.obj");
	rotationCircleY = MeshManager::LoadMesh("engine_assets/rotation_circleY.obj");
	rotationCircleZ = MeshManager::LoadMesh("engine_assets/rotation_circleZ.obj");

	toolArrowsTexture = Texture::MakeTexture();
	toolArrowsTexture->m_file = FileSystem::MakeFile("engine_assets/tool_arrows_colors.png");
	toolArrowsTexture->SetFilter(Filter::Point);
	FileReference::LoadOptions loadOptions;
	loadOptions.platform = Application::GetPlatform();
	loadOptions.threaded = true;
	toolArrowsTexture->LoadFileReference(loadOptions);

	Engine::GetOnWindowFocusEvent()->Bind(&OnWindowFocused);

	CheckIntegrity();
}

void Editor::SaveMenuSettings()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::shared_ptr<File> file = FileSystem::MakeFile("menu_settings.json");
	ReflectionUtils::ReflectiveDataToFile(menuSettings.GetReflectiveData(), file);
}

void Editor::LoadMenuSettings()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	bool success = false;
	std::shared_ptr<File> file = FileSystem::MakeFile("menu_settings.json");
	if (file->CheckIfExist())
	{
		success = ReflectionUtils::FileToReflectiveData(file, menuSettings.GetReflectiveData());
	}

	// If the file does not exist or is corrupted, create a new one
	if (!success || menuSettings.settings.empty())
	{
		CreateNewMenuSettings();
		SaveMenuSettings();
	}
}

Editor::MenuSetting* Editor::AddMenuSetting(std::vector<MenuSetting*>& menuSettingList, const std::string& name, bool isActive, bool isUnique, int id = 0)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	MenuSetting* newMenuSetting = new MenuSetting();
	newMenuSetting->name = name;
	newMenuSetting->isActive = isActive;
	newMenuSetting->isUnique = isUnique;
	newMenuSetting->id = id;
	menuSettingList.push_back(newMenuSetting);
	return newMenuSetting;
}

Editor::MenuSetting* Editor::UpdateOrAddMenuSetting(std::vector<MenuSetting*>& menuSettingList, const std::string& name, bool isActive, bool isUnique, int id)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t menuSize = menuSettings.settings.size();
	bool found = false;
	MenuSetting* menuSetting = nullptr;
	for (size_t i = 0; i < menuSize; i++)
	{
		MenuSetting& setting = *menuSettings.settings[i];
		if (setting.name == name && setting.id == id)
		{
			setting.isActive = isActive;
			setting.id = id;
			found = true;
			menuSetting = &setting;
			break;
		}
	}

	if (!found)
	{
		return AddMenuSetting(menuSettingList, name, isActive, isUnique, id);
	}

	return menuSetting;
}

void Editor::CreateNewMenuSettings()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	menuSettings = MenuSettings();
	std::vector<MenuSetting*>& menuSettingList = menuSettings.settings;

	AddMenuSetting(menuSettingList, "CreateClassMenu", false, true);
	AddMenuSetting(menuSettingList, "LightingMenu", false, true);
	AddMenuSetting(menuSettingList, "ProjectSettingsMenu", false, true);
	AddMenuSetting(menuSettingList, "EngineSettingsMenu", false, true);
	AddMenuSetting(menuSettingList, "DockerConfigMenu", false, true);
	AddMenuSetting(menuSettingList, "AboutMenu", false, true);
	AddMenuSetting(menuSettingList, "BuildSettingsMenu", false, true);
	AddMenuSetting(menuSettingList, "EngineAssetManagerMenu", false, true);
	AddMenuSetting(menuSettingList, "EngineDebugMenu", false, true);
	AddMenuSetting(menuSettingList, "DataBaseCheckerMenu", false, true);
	AddMenuSetting(menuSettingList, "UpdateAvailableMenu", false, true);

	AddMenuSetting(menuSettingList, "FileExplorerMenu", true, false);
	AddMenuSetting(menuSettingList, "HierarchyMenu", true, false);
	AddMenuSetting(menuSettingList, "InspectorMenu", true, false);
	AddMenuSetting(menuSettingList, "ProfilerMenu", true, false);
	AddMenuSetting(menuSettingList, "GameMenu", true, false);
	AddMenuSetting(menuSettingList, "SceneMenu", true, false);
	AddMenuSetting(menuSettingList, "CompilingMenu", true, false);
	AddMenuSetting(menuSettingList, "SelectProjectMenu", true, false);
	AddMenuSetting(menuSettingList, "CreateProjectMenu", true, false);
	AddMenuSetting(menuSettingList, "ConsoleMenu", true, false);

	XASSERT(menuSettingList.size() == ClassRegistry::GetMenuClassInfosCount(), "[Editor::CreateNewMenuSettings] Menu count is not correct");
}

void Editor::OnFileModified()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ProjectManager::RefreshProjectDirectory();
}

void Editor::OnCodeModified()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (EngineSettings::values.compileOnCodeChanged)
		Compiler::HotReloadGame();
}

void Editor::OnWindowFocused()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (ProjectManager::IsProjectLoaded() && Engine::IsRunning(true))
	{
		FileHandler::HasFileChangedOrAddedThreaded(ProjectManager::GetAssetFolderPath(), OnFileModified);
		FileHandler::HasCodeChangedThreaded(ProjectManager::GetAssetFolderPath(), OnCodeModified);
	}
}

void Editor::Update()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (ProjectManager::IsProjectLoaded())
	{
		//------- Check shortcuts

		if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::Z)))
		{
			CommandManager::Undo();
		}
		if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::Y)))
		{
			CommandManager::Redo();
		}

		if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::D)))
		{
			std::vector <std::shared_ptr<GameObject>> selectedGameObjectsToCheck;
			for (auto& weakGo : GetSelectedGameObjects())
			{
				selectedGameObjectsToCheck.push_back(weakGo.lock());
			}

			std::vector<std::shared_ptr<GameObject>> gameObjectsToDuplicate = RemoveChildren(selectedGameObjectsToCheck);
			for (std::shared_ptr<GameObject>& gameObjectToDuplicate : gameObjectsToDuplicate)
			{
				std::shared_ptr<GameObject> newGameObject = Instantiate(gameObjectToDuplicate);
				if (gameObjectToDuplicate->GetParent().lock() != nullptr)
				{
					newGameObject->SetParent(gameObjectToDuplicate->GetParent().lock());
				}
				std::shared_ptr<Transform> newTransform = newGameObject->GetTransform();
				std::shared_ptr<Transform> transformToDuplicate = gameObjectToDuplicate->GetTransform();
				newTransform->SetLocalPosition(transformToDuplicate->GetLocalPosition());
				newTransform->SetLocalRotation(transformToDuplicate->GetLocalEulerAngles());
				newTransform->SetLocalScale(transformToDuplicate->GetLocalScale());
				SetSelectedGameObject(newGameObject);
			}
		}

		if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::N)))
		{
			CreateEmpty();
		}

		if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::NUM_1)))
		{
			std::shared_ptr<SceneMenu> sceneMenu = Editor::GetMenu<SceneMenu>();
			if (sceneMenu)
				sceneMenu->Focus();
		}

		if ((InputSystem::GetKey(KeyCode::LEFT_SHIFT) && InputSystem::GetKeyDown(KeyCode::D)) && !EditorUI::IsEditingElement())
		{
			SetSelectedGameObject(nullptr);
			SetSelectedFileReference(nullptr);
		}

		if (InputSystem::GetKeyDown(KeyCode::DEL))
		{
			const std::shared_ptr<SceneMenu> sceneMenu = Editor::GetMenu<SceneMenu>();
			const std::shared_ptr<HierarchyMenu> hierarchy = Editor::GetMenu<HierarchyMenu>();
			if ((sceneMenu && sceneMenu->IsFocused()) || (hierarchy && hierarchy->IsFocused()))
			{
				for (std::weak_ptr<GameObject>& currentGameObject : selectedGameObjects)
				{
					auto command = std::make_shared<InspectorDeleteGameObjectCommand>(currentGameObject);
					CommandManager::AddCommandAndExecute(command);
				}
				selectedGameObjects.clear();
			}
		}

		if (InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKey(KeyCode::LEFT_SHIFT) && InputSystem::GetKeyDown(KeyCode::P)) // Pause / UnPause game
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
		else if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::P))) // Start / Stop game
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
			if ((InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::S)))
			{
				SceneManager::SaveScene(SaveSceneType::SaveSceneToFile);
			}
		}
	}
}

void Editor::Draw()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	EditorUI::NewFrame();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ApplyEditorStyle();
	if (currentMenu == MenuGroup::Menu_Editor)
		mainBar->Draw();

	float offset = mainBar->GetHeight();
	if (currentMenu != MenuGroup::Menu_Editor)
		offset = 0;

	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset - 32));
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	// Dock windows if they are not already docked (first time using the editor)
	const ImGuiID dsId = ImGui::GetID("BackgroundDock");
	const ImGuiDockNode* first_time = ImGui::DockBuilderGetNode(dsId);
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
		ImGui::DockBuilderDockWindow("###Console0", fileExplorerNode);
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
		if (menus[i]->IsActive() && menus[i]->group == currentMenu)
			menus[i]->Draw();
	}

	ImGui::PopStyleVar();
	ImGui::End();

	if (currentMenu == MenuGroup::Menu_Editor)
	{
		bottomBar->Draw();
	}

	RemoveEditorStyle();
	EditorUI::Render();
}

void Editor::CheckItemIntegrity(const std::string& itemPath, bool& success)
{
	if (!std::filesystem::exists(itemPath))
	{
		Debug::PrintError("File/Folder does not exist: " + itemPath);
		success = false;
	}
}

bool Editor::CheckIntegrity()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	bool success = true;

	// Check folders
	CheckItemIntegrity("engine_assets", success);
	CheckItemIntegrity("icons", success);
	CheckItemIntegrity("include", success);
	CheckItemIntegrity("psp_images", success);
	CheckItemIntegrity("psvita_images", success);
	CheckItemIntegrity("public_engine_assets", success);
	CheckItemIntegrity("Source", success);

	// Check files
#if defined(_WIN32) || defined(_WIN64)
	CheckItemIntegrity("freetype.dll", success);
	CheckItemIntegrity("assimp-vc143-mt.dll", success);
	CheckItemIntegrity("SDL3.dll", success);

	CheckItemIntegrity("Xenity_Editor.dll", success);
	CheckItemIntegrity("Xenity_Editor.lib", success);
	CheckItemIntegrity("Xenity_Engine.dll", success);
	CheckItemIntegrity("Xenity_Engine.lib", success);

	CheckItemIntegrity("res.rc", success);
	CheckItemIntegrity("logo.ico", success);
	CheckItemIntegrity("resource.h", success);
#endif

	CheckItemIntegrity("Roboto Regular.ttf", success);
	CheckItemIntegrity("CMakeLists.txt", success);
	CheckItemIntegrity("Dockerfile", success);
	CheckItemIntegrity("compile_shaders.sh", success);
	CheckItemIntegrity("main.cpp", success);

	if (!success)
	{
		Debug::PrintError("Some files/folders are missing, please check the integrity of the engine files");
	}

	return success;
}

void Editor::ApplyEditorStyle()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	// Default colors
	// BG 15 15 15
	// Input 29 47 73
	// tab 31 57 88
	// tab selected 51 105 173
	// Button 35 69 109
	// Element selected 56 123 203

	const Vector4 bgColor = EngineSettings::values.backbgroundColor.GetRGBA().ToVector4();
	const Vector4 secondaryColor = EngineSettings::values.secondaryColor.GetRGBA().ToVector4();
	const Vector4 playTint = EngineSettings::values.playTintColor.GetRGBA().ToVector4();
	Vector4 pTint = Vector4(0);

	if (GameplayManager::GetGameState() != GameState::Stopped)
		pTint = playTint;
	else if (!EngineSettings::values.isPlayTintAdditive)
		pTint = Vector4(1);

	if (EngineSettings::values.isPlayTintAdditive)
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(bgColor.x + pTint.x, bgColor.y + pTint.y, bgColor.z + pTint.z, 1));
	else
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(bgColor.x * pTint.x, bgColor.y * pTint.y, bgColor.z * pTint.z, 1));

	const ImVec4 colorLevel0 = ImVec4(secondaryColor.x, secondaryColor.y, secondaryColor.z, 1);
	const ImVec4 colorLevel05 = ImVec4(secondaryColor.x - 0.05f, secondaryColor.y - 0.05f, secondaryColor.z - 0.05f, 1);
	const ImVec4 colorLevel1 = ImVec4(secondaryColor.x - 0.1f, secondaryColor.y - 0.1f, secondaryColor.z - 0.1f, 1);
	const ImVec4 colorLevel15 = ImVec4(secondaryColor.x - 0.15f, secondaryColor.y - 0.15f, secondaryColor.z - 0.15f, 1);
	const ImVec4 colorLevel2 = ImVec4(secondaryColor.x - 0.2f, secondaryColor.y - 0.2f, secondaryColor.z - 0.2f, 1);
	const ImVec4 colorLevel25 = ImVec4(secondaryColor.x - 0.25f, secondaryColor.y - 0.25f, secondaryColor.z - 0.25f, 1);
	const ImVec4 colorLevel3 = ImVec4(secondaryColor.x - 0.3f, secondaryColor.y - 0.3f, secondaryColor.z - 0.3f, 1);
	const ImVec4 colorLevel4 = ImVec4(secondaryColor.x - 0.4f, secondaryColor.y - 0.4f, secondaryColor.z - 0.4f, 1);
	const ImVec4 colorLevel4Alpha = ImVec4(secondaryColor.x - 0.4f, secondaryColor.y - 0.4f, secondaryColor.z - 0.4f, 0.5f);

	ImGui::PushStyleColor(ImGuiCol_Button, colorLevel25);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorLevel1);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorLevel0);

	ImGui::PushStyleColor(ImGuiCol_Tab, colorLevel2);
	ImGui::PushStyleColor(ImGuiCol_TabSelected, colorLevel1);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, colorLevel0);
	ImGui::PushStyleColor(ImGuiCol_TabDimmed, colorLevel3);
	ImGui::PushStyleColor(ImGuiCol_TabDimmedSelected, colorLevel2);

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
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	ImGui::PopStyleVar();

	ImGui::PopStyleColor(16);
}

void Editor::CreateEmpty()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	auto command = std::make_shared<InspectorCreateGameObjectCommand>(std::vector<std::weak_ptr<GameObject>>(), CreateGameObjectMode::CreateEmpty);
	CommandManager::AddCommandAndExecute(command);
}

void Editor::CreateEmptyChild()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	auto command = std::make_shared<InspectorCreateGameObjectCommand>(selectedGameObjects, CreateGameObjectMode::CreateChild);
	CommandManager::AddCommandAndExecute(command);
}

void Editor::CreateEmptyParent()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	auto command = std::make_shared<InspectorCreateGameObjectCommand>(selectedGameObjects, CreateGameObjectMode::CreateParent);
	CommandManager::AddCommandAndExecute(command);
}

void Editor::SetSelectedFileReference(const std::shared_ptr<FileReference>& fileReference)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	selectedFileReference = fileReference;
	std::shared_ptr<InspectorMenu> inspector = Editor::GetMenu<InspectorMenu>();
	if (inspector)
		inspector->loadedPreview = nullptr;

	if (fileReference)
		SetSelectedGameObject(nullptr);
}

std::shared_ptr<FileReference> Editor::GetSelectedFileReference()
{
	return selectedFileReference;
}

void Editor::SetSelectedGameObject(const std::shared_ptr<GameObject>& newSelected)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ClearSelectedGameObjects();

	if (newSelected == nullptr)
		return;

	SetSelectedFileReference(nullptr);

	newSelected->m_isSelected = true;
	selectedGameObjects.push_back(newSelected);
}

void Editor::ClearSelectedGameObjects()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	for (std::weak_ptr<GameObject>& currentGameObject : selectedGameObjects)
	{
		if (currentGameObject.lock())
			currentGameObject.lock()->m_isSelected = false;
	}
	selectedGameObjects.clear();
}

void Editor::AddSelectedGameObject(const std::shared_ptr<GameObject>& gameObjectToAdd)
{
	XASSERT(gameObjectToAdd != nullptr, "[Editor::AddSelectedGameObject] gameObjectToAdd is nullptr");

	bool found = false;
	for (std::weak_ptr<GameObject>& currentGameObject : selectedGameObjects)
	{
		if (currentGameObject.lock() == gameObjectToAdd)
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		gameObjectToAdd->m_isSelected = true;
		selectedGameObjects.push_back(gameObjectToAdd);
	}
}

void Editor::RemoveSelectedGameObject(const std::shared_ptr<GameObject>& gameObjectToRemove)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(gameObjectToRemove != nullptr, "[Editor::RemoveSelectedGameObject] gameObjectToRemove is nullptr");

	const size_t goCount = selectedGameObjects.size();
	for (size_t i = 0; i < goCount; i++)
	{
		if (selectedGameObjects[i].lock() == gameObjectToRemove)
		{
			gameObjectToRemove->m_isSelected = false;
			selectedGameObjects.erase(selectedGameObjects.begin() + i);
			break;
		}
	}
}

bool Editor::IsInSelectedGameObjects(const std::shared_ptr<GameObject>& gameObjectToCheck)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	XASSERT(gameObjectToCheck != nullptr, "[Editor::IsInSelectedGameObjects] gameObjectToCheck is nullptr");

	for (std::weak_ptr<GameObject>& currentGameObject : selectedGameObjects)
	{
		if (currentGameObject.lock() == gameObjectToCheck)
		{
			return true;
		}
	}

	return false;
}

const std::vector<std::weak_ptr<GameObject>>& Editor::GetSelectedGameObjects()
{
	return selectedGameObjects;
}

void Editor::SetCurrentProjectDirectory(const std::shared_ptr <ProjectDirectory>& dir)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (currentProjectDirectory)
		currentProjectDirectory->files.clear();
	currentProjectDirectory = dir;
	if (currentProjectDirectory)
	{
		ProjectManager::FillProjectDirectory(*currentProjectDirectory);
		const size_t itemCount = currentProjectDirectory->files.size();
		FileReference::LoadOptions loadOptions;
		loadOptions.platform = Application::GetPlatform();
		loadOptions.threaded = true;
		for (size_t i = 0; i < itemCount; i++)
		{
			currentProjectDirectory->files[i]->LoadFileReference(loadOptions);
		}
	}
}

std::shared_ptr <ProjectDirectory> Editor::GetCurrentProjectDirectory()
{
	return currentProjectDirectory;
}

std::shared_ptr<File> Editor::CreateNewFile(const std::string& fileName, FileType type, bool fillWithDefaultData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::string fileExt = "";
	switch (type)
	{
	case FileType::File_Scene:
		fileExt = ".xen";
		break;
	case FileType::File_Skybox:
		fileExt = ".sky";
		break;
	case FileType::File_Code:
		fileExt = ".cpp";
		break;
	case FileType::File_Header:
		fileExt = ".h";
		break;
	case FileType::File_Material:
		fileExt = ".mat";
		break;
	case FileType::File_Shader:
		fileExt = ".shader";
		break;
	default:
	{
		XASSERT(false, "[Editor::CreateNewFile] Try to created an unsupported file");
		return nullptr;
	}
	}

	std::shared_ptr<File> newFile = FileSystem::MakeFile(fileName + fileExt);
	
	// Find an available name
	int index = 0;
	while (newFile->CheckIfExist())
	{
		index++;
		newFile = FileSystem::MakeFile(fileName + " (" + std::to_string(index) + ")" + fileExt);
	}

	// Create file and write default data
	if (newFile->Open(FileMode::WriteCreateFile))
	{
		if (fillWithDefaultData)
			newFile->Write(AssetManager::GetDefaultFileData(type));
		newFile->Close();
	}

	ProjectManager::RefreshProjectDirectory();

	return newFile;
}

void Editor::OpenExplorerWindow(const std::string& path, bool isSelected)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const std::string convertedPath = FileSystem::ConvertBasicPathToWindowsPath(path);
	std::string command = "explorer.exe ";
	if (isSelected)
	{
		command += "/select, \"";
	}
	else
	{
		command += "\"";
	}

	command += convertedPath;

	command += "\"";

	system(command.c_str());
}

void Editor::AddDragAndDrop(const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (path.empty())
		return;

	dragdropEntries.push_back(path);
}

void Editor::StartFolderCopy(const std::string& path, const std::string& newPath)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (path.empty() || newPath.empty())
		return;

	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		// Check is file
		if (!file.is_regular_file())
		{
			const std::string newFolderPath = newPath + file.path().filename().string() + '\\';
			FileSystem::s_fileSystem->CreateFolder(newFolderPath);
			StartFolderCopy(file.path().string() + '\\', newFolderPath);
		}
		else
		{
			FileSystem::s_fileSystem->CopyFile(file.path().string(), newPath + file.path().filename().string(), true); // TODO ask if we want to replace files
		}
	}
}

void Editor::GetIncrementedGameObjectNameInfo(const std::string& name, std::string& baseName, int& number)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	int endParenthesis = -1;
	int startParenthesis = -1;
	const int nameLenght = (int)name.size();
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

void Editor::OnUpdateChecked(bool newVersionAvailable)
{
	updateAvailable = newVersionAvailable;
	if (newVersionAvailable)
	{
		GetMenu<UpdateAvailableMenu>()->SetActive(true);
		GetMenu<UpdateAvailableMenu>()->Focus();
	}
}

std::string Editor::GetIncrementedGameObjectName(const std::string& name)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::string finalName = "";
	int number = 1;
	bool foundOne = false;
	GetIncrementedGameObjectNameInfo(name, finalName, number);

	//const int gameObjectCount = GameplayManager::gameObjects.size();
	//for (int i = 0; i < gameObjectCount; i++)
	for (const std::shared_ptr<GameObject>& gameObject : GameplayManager::gameObjects)
	{
		std::string tempName;
		int tempNumber;
		GetIncrementedGameObjectNameInfo(gameObject->GetName(), tempName, tempNumber);
		if (tempName == finalName)
		{
			foundOne = true;
			if (number < tempNumber)
				number = tempNumber;
		}
	}

	if (foundOne)
		finalName = finalName + " (" + std::to_string(number) + ")";

	return finalName;
}

void Editor::OnDragAndDropFileFinished()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t size = dragdropEntries.size();
	for (size_t dragIndex = 0; dragIndex < size; dragIndex++)
	{
		try
		{
			std::string& path = dragdropEntries[dragIndex];
			const bool isDirectory = std::filesystem::is_directory(path);
			const int pathSize = static_cast<int>(path.size());

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
			const std::string newPath = Editor::GetCurrentProjectDirectory()->path + path.substr(lastBackSlash + 1);

			if (isDirectory)
			{
				FileSystem::s_fileSystem->CreateFolder(newPath + '\\');
				StartFolderCopy(dragdropEntries[dragIndex] + '\\', newPath + '\\');
			}
			else
			{
				const int copyResult = FileSystem::s_fileSystem->CopyFile(path, newPath, false);
				if (copyResult == -1)
				{
					DialogResult result = EditorUI::OpenDialog("File copy error", "This file already exists in this location.\nDo you want to replace it?", DialogType::Dialog_Type_YES_NO_CANCEL);
					if (result == DialogResult::Dialog_YES)
					{
						FileSystem::s_fileSystem->CopyFile(path, newPath, true);
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			Debug::PrintError(e.what(), true);
		}
	}

	dragdropEntries.clear();
	ProjectManager::RefreshProjectDirectory();
}

bool Editor::IsParentOf(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& child)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	if (parent == nullptr || child == nullptr)
		return false;

	for (std::weak_ptr<GameObject> curChild : parent->GetChildren())
	{
		if (auto curChildLock = curChild.lock())
		{
			if (curChildLock == child)
			{
				return true;
			}
			else
			{
				if (IsParentOf(curChildLock, child))
				{
					return true;
				}
			}
		}
	}

	return false;
}

std::vector<std::shared_ptr<GameObject>> Editor::RemoveChildren(std::vector<std::shared_ptr<GameObject>> parentsAndChildren)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	size_t goCount = parentsAndChildren.size();
	for (size_t i = 0; i < goCount; i++)
	{
		std::shared_ptr<GameObject> currentGameObject = parentsAndChildren[i];
		if (currentGameObject == nullptr)
		{
			goCount--;
			parentsAndChildren.erase(parentsAndChildren.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < goCount; i++)
	{
		const std::shared_ptr<GameObject>& currentGameObject = parentsAndChildren[i];

		for (int j = 0; j < goCount; j++)
		{
			const std::shared_ptr<GameObject>& currentGameObject2 = parentsAndChildren[j];
			if (currentGameObject == currentGameObject2)
				continue;

			if (IsParentOf(currentGameObject, currentGameObject2))
			{
				goCount--;
				parentsAndChildren.erase(parentsAndChildren.begin() + j);
				if (j <= i)
					i--;
				j--;
				continue;
			}
		}
	}

	return parentsAndChildren;
}

void Editor::CreateMenus()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t menuSize = menuSettings.settings.size();
	for (size_t i = 0; i < menuSize; i++)
	{
		MenuSetting& setting = *menuSettings.settings[i];
		if (setting.isUnique || setting.isActive)
			AddMenu(setting.name, setting.isActive, setting.id);
	}

	mainBar = std::make_shared<MainBarMenu>();
	mainBar->Init();

	bottomBar = std::make_shared<BottomBarMenu>();
}

bool Editor::SeparateFileFromPath(const std::string& fullPath, std::string& folderPath, std::string& fileName)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	if (fullPath.empty())
		return false;

	const int backSlash = (int)fullPath.find_last_of('/');
	const int backSlash2 = (int)fullPath.find_last_of('\\');
	int finalBackSlashPos = -1;
	if (backSlash < backSlash2)
	{
		finalBackSlashPos = backSlash2;
	}
	else
	{
		finalBackSlashPos = backSlash;
	}

	if (finalBackSlashPos == -1)
		return false;

	fileName = fullPath.substr(finalBackSlashPos + 1);
	folderPath = fullPath.substr(0, finalBackSlashPos + 1);

	return true;
}

bool Editor::OpenExecutableFile(const std::string& executablePath)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(!executablePath.empty(), "[Editor::OpenExecutableFile] executablePath is empty");
	if (executablePath.empty())
		return false;

	std::string finalName;
	std::string path;

	if (SeparateFileFromPath(executablePath, path, finalName))
	{
		std::string command = "cd \"" + path + "\"" + " && " + "\"" + finalName + "\"";
		const int result = system(command.c_str());
		return result == 0;
	}
	else
	{
		return false;
	}
}

int Editor::ExecuteSystemCommand(const std::string& command, std::string& outputText)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(__LINUX__)
	FILE* f = popen(command.c_str(), "r");
#else
	FILE* f = _popen(command.c_str(), "r");
#endif
	constexpr size_t bufferSize = 2048;
	char output[bufferSize];
	fgets(output, bufferSize, f);
#if defined(__LINUX__)
	int ret = pclose(f);
#else
	int ret = _pclose(f);
#endif
	outputText = std::string(output);
	return ret;
}

#endif