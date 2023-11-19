#if defined(EDITOR)
#include "editor.h"
#include "../xenity.h"
#include "../xenity_editor.h"
#include "../engine/engine.h"

#include "ui/menus/project_settings_menu.h"
#include "ui/menus/engine_settings_menu.h"
#include "ui/menus/file_explorer_menu.h"
#include "ui/menus/hierarchy_menu.h"
#include "ui/menus/inspector_menu.h"
#include "ui/menus/main_bar_menu.h"
#include "ui/menus/profiler_menu.h"
#include "ui/menus/game_menu.h"
#include "ui/menus/scene_menu.h"
#include "ui/menus/compiling_menu.h"
#include "ui/menus/select_project_menu.h"
#include "ui/menus/create_project_menu.h"
#include "ui/menus/lighting_menu.h"
#include "ui/menus/create_class_menu.h"

#include <functional>
#include "../engine/class_registry/class_registry.h"
#include "../engine/file_system/file_reference.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h"
#include "../engine/asset_management/project_manager.h"
#include "../engine/reflection/reflection_utils.h"
#include "../engine/scene_management/scene_manager.h"
#include "command/command_manager.h"

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

void Editor::Update()
{
	// Check user input and camera movement when in the scene menu
	if (sceneMenu->isFocused)
	{
		// Get camera transform
		std::shared_ptr<Transform> cameraTransform = cameraGO.lock()->GetTransform();
		Vector3 rot = cameraTransform->GetRotation();
		Vector3 pos = cameraTransform->GetPosition();

		// Rotate the camera when dragging the mouse right click
		if (InputSystem::GetKey(MOUSE_RIGHT) && sceneMenu->isHovered)
		{
			rot.x += -InputSystem::mouseSpeed.y * 70;
			rot.y += InputSystem::mouseSpeed.x * 70;
		}

		// Move the camera when using keyboard's arrows
		float fwd = 0;
		float up = 0;
		float side = 0;

		if (InputSystem::GetKey(UP))
			fwd = -1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(DOWN))
			fwd = 1 * Time::GetDeltaTime();

		if (InputSystem::GetKey(RIGHT))
			side = 1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(LEFT))
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

	if ((InputSystem::GetKey(LEFT_CONTROL) && (/*InputSystem::GetKeyDown(C) || */ InputSystem::GetKeyDown(D))))
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

	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(S)))
		{
			SceneManager::SaveScene(SaveSceneToFile);
		}
	}
}

void Editor::Draw()
{
	EditorUI::NewFrame();

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
		mainBar->Draw();
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
		std::shared_ptr<GameObject> newGameObject = CreateGameObject(goToDuplicate->name);

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
	if (newFile->Open(true))
	{
		if (fillWithDefaultData)
			newFile->Write(AssetManager::GetDefaultFileData(type));
		newFile->Close();
	}

	ProjectManager::RefreshProjectDirectory();

	return newFile;
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