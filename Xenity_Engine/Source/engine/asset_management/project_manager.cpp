#include "project_manager.h"

#include "code_file.h"

#include <engine/engine.h>
#include <engine/game_interface.h>

#include <engine/reflection/reflection_utils.h>
#include <engine/dynamic_lib/dynamic_lib.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/scene_management/scene.h>

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/ui/window.h>

#include <engine/audio/audio_clip.h>

#include <editor/editor.h>

#include <json.hpp>

#include <engine/debug/debug.h>
#if defined(EDITOR)
#include <editor/file_handler.h>
#include <editor/compiler.h>
#endif
#if !defined(EDITOR) && !defined(_WIN32) && !defined(_WIN64)
	#include <game_dungeon/game.h>
#endif
#include <engine/engine_settings.h>

using json = nlohmann::json;

std::unordered_map<uint64_t, FileChange> ProjectManager::oldProjectFilesIds;
std::unordered_map<uint64_t, FileAndPath> ProjectManager::projectFilesIds;
std::shared_ptr<ProjectDirectory> ProjectManager::projectDirectory = nullptr;
ProjectSettings  ProjectManager::projectSettings;
std::string ProjectManager::projectFolderPath = "";
std::string ProjectManager::assetFolderPath = "";
std::string ProjectManager::engineAssetsFolderPath = "";
bool ProjectManager::projectLoaded = false;
std::shared_ptr<Directory> ProjectManager::projectDirectoryBase = nullptr;

std::shared_ptr <ProjectDirectory> ProjectManager::FindProjectDirectory(std::shared_ptr <ProjectDirectory> directoryToCheck, const std::string& directoryPath)
{
	if (!directoryToCheck)
		return nullptr;

	size_t dirCount = directoryToCheck->subdirectories.size();
	for (size_t i = 0; i < dirCount; i++)
	{
		std::shared_ptr <ProjectDirectory> subDir = directoryToCheck->subdirectories[i];
		// Check if the sub directory is the directory to find
		if (subDir->path == directoryPath)
		{
			return subDir;
		}
		else
		{
			// Start recursive to search in the sub directory
			std::shared_ptr <ProjectDirectory> foundSubDir = FindProjectDirectory(subDir, directoryPath);
			if (foundSubDir)
				return foundSubDir;
		}
	}
	return nullptr;
}

void ProjectManager::FindAllProjectFiles()
{
	// Keep in memory the old opened directory path to re-open it later
	std::string oldPath = "";
#if defined(EDITOR)
	if (Editor::GetCurrentProjectDirectory())
		oldPath = Editor::GetCurrentProjectDirectory()->path;

	Editor::SetCurrentProjectDirectory(nullptr);
#endif
	for (const auto& kv : projectFilesIds)
	{
		FileChange fileChange = FileChange();
		fileChange.path = kv.second.path;
		oldProjectFilesIds[kv.first] = fileChange;
	}

	projectFilesIds.clear();
	//Get all files of the project
	std::vector<std::shared_ptr<File>> projectFiles = projectDirectoryBase->GetAllFiles(true);

	projectDirectory = std::make_shared<ProjectDirectory>(assetFolderPath, 0);

	std::vector<std::shared_ptr<File>> allFoundFiles;
	std::unordered_map<std::shared_ptr<File>, FileType> compatibleFiles;

	//Get all files supported by the engine
	int fileCount = (int)projectFiles.size();
	int allFoundFileCount = 0;
	for (int i = 0; i < fileCount; i++)
	{
		std::shared_ptr<File> file = projectFiles[i];
		FileType fileType = GetFileType(file->GetFileExtension());

		if (fileType != File_Other)
		{
			compatibleFiles[file] = fileType;
			allFoundFiles.push_back(file);
			allFoundFileCount++;
		}
	}

	std::vector<std::shared_ptr<File>> fileWithoutMeta;
	int fileWithoutMetaCount = 0;
	uint64_t biggestId = 0;
	for (int i = 0; i < allFoundFileCount; i++)
	{
		std::shared_ptr<File> file = allFoundFiles[i];
		std::shared_ptr<File> metaFile = FileSystem::MakeFile(file->GetPath() + META_EXTENSION);
		if (!metaFile->CheckIfExist()) //If there is not meta for this file
		{
			//Create one later
			fileWithoutMeta.push_back(file);
			fileWithoutMetaCount++;
		}
		else // Or read meta file
		{
			if (metaFile->Open(false))
			{
				std::string jsonString = metaFile->ReadAll();
				metaFile->Close();

				json data;
				data = json::parse(jsonString);
				uint64_t id = data["id"];
				if (id > biggestId)
					biggestId = id;
				file->SetUniqueId(id);
			}
		}
	}

	//Set new files ids
	UniqueId::lastFileUniqueId = biggestId;
	for (int i = 0; i < fileWithoutMetaCount; i++)
	{
		uint64_t id = UniqueId::GenerateUniqueId(true);
		fileWithoutMeta[i]->SetUniqueId(id);
	}

	// Fill projectFilesIds
	for (const auto& kv : compatibleFiles)
	{
		FileAndPath fileAndPath = FileAndPath();
		fileAndPath.file = kv.first;
		fileAndPath.path = kv.first->GetPath();
		projectFilesIds[kv.first->GetUniqueId()] = fileAndPath;
	}

	// Create files references
	for (const auto& kv : projectFilesIds)
	{
		CreateFilReference(kv.second.path, kv.first);
	}

	// Check if a file has changed or has been deleted
	for (const auto& kv : projectFilesIds)
	{
		bool contains = oldProjectFilesIds.contains(kv.first);
		if (contains)
		{
			oldProjectFilesIds[kv.first].hasBeenDeleted = false;
		}
		if (contains && oldProjectFilesIds[kv.first].path != kv.second.path)
		{
			oldProjectFilesIds[kv.first].hasChanged = true;
		}
	}

	// Update file or delete files references
	for (const auto& kv : oldProjectFilesIds)
	{
		if (kv.second.hasChanged)
		{
			GetFileReferenceById(kv.first)->file = projectFilesIds[kv.first].file;
		}
		else if (kv.second.hasBeenDeleted)
		{
			AssetManager::ForceDeleteFileReference(GetFileReferenceById(kv.first));
		}
	}

#if defined(EDITOR)
	// Get all project directories and open one
	CreateProjectDirectories(projectDirectoryBase, projectDirectory);
	std::shared_ptr <ProjectDirectory> lastOpenedDir = FindProjectDirectory(projectDirectory, oldPath);
	if (lastOpenedDir)
		Editor::SetCurrentProjectDirectory(lastOpenedDir);
	else
		Editor::SetCurrentProjectDirectory(projectDirectory);
#endif

	oldProjectFilesIds.clear();
	projectFiles.clear();
	allFoundFiles.clear();
	fileWithoutMeta.clear();
	compatibleFiles.clear();
}

void ProjectManager::CreateProjectDirectories(std::shared_ptr <Directory> projectDirectoryBase, std::shared_ptr <ProjectDirectory> realProjectDirectory)
{
	size_t dirCount = projectDirectoryBase->subdirectories.size();
	for (size_t i = 0; i < dirCount; i++)
	{
		std::shared_ptr <ProjectDirectory> newDir = std::make_shared<ProjectDirectory>(projectDirectoryBase->subdirectories[i]->GetPath(), projectDirectoryBase->subdirectories[i]->GetUniqueId());
		realProjectDirectory->subdirectories.push_back(newDir);
		CreateProjectDirectories(projectDirectoryBase->subdirectories[i], newDir);
	}
}

void ProjectManager::RefreshProjectDirectory()
{
	FindAllProjectFiles();
}

void ProjectManager::FillProjectDirectory(std::shared_ptr <ProjectDirectory> realProjectDirectory)
{
	realProjectDirectory->files.clear();

	for (auto& kv : ProjectManager::projectFilesIds)
	{
		// Check if this file is in this folder
		if (realProjectDirectory->path == kv.second.file->GetFolderPath())
		{
			realProjectDirectory->files.push_back(ProjectManager::GetFileReferenceById(kv.first));
		}
	}
}

bool ProjectManager::CreateProject(const std::string& name, const std::string& folderPath)
{
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\materials\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\shaders\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\textures\\");

	// TODO improve this
	try
	{
		// Copy basic materials
		std::filesystem::copy_file("materials\\standardMaterial.mat", folderPath + name + "\\assets\\materials\\standardMaterial.mat", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("materials\\standardMaterial.mat.meta", folderPath + name + "\\assets\\materials\\standardMaterial.mat.meta", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("materials\\unlitMaterial.mat", folderPath + name + "\\assets\\materials\\unlitMaterial.mat", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("materials\\unlitMaterial.mat.meta", folderPath + name + "\\assets\\materials\\unlitMaterial.mat.meta", std::filesystem::copy_options::overwrite_existing);

		// Copy basic shaders
		std::filesystem::copy_file("shaders\\standard.shader", folderPath + name + "\\assets\\shaders\\standard.shader", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("shaders\\standard.shader.meta", folderPath + name + "\\assets\\shaders\\standard.shader.meta", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("shaders\\unlit.shader", folderPath + name + "\\assets\\shaders\\unlit.shader", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("shaders\\unlit.shader.meta", folderPath + name + "\\assets\\shaders\\unlit.shader.meta", std::filesystem::copy_options::overwrite_existing);

		// Copy basic 3D models
		std::filesystem::copy("engine_assets\\models\\", folderPath + name + "\\assets\\models\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);

		// Copy basic texture
		std::filesystem::copy_file("engine_assets\\default_texture.png", folderPath + name + "\\assets\\textures\\default_texture.png", std::filesystem::copy_options::overwrite_existing);
	
		std::filesystem::copy_file("engine_assets\\empty_default\\game.cpp", folderPath + name + "\\assets\\game.cpp", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("engine_assets\\empty_default\\game.h", folderPath + name + "\\assets\\game.h", std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::exception&)
	{
		Debug::PrintError("[ProjectManager::CreateProject] Error when copying default assets into the project.");
	}

	projectSettings.projectName = name;
	projectSettings.gameName = name;
	projectFolderPath = folderPath + name + "\\";
	SaveProjectSettings();
	
	return LoadProject(projectFolderPath);
}

FileType ProjectManager::GetFileType(std::string extension)
{
	FileType fileType = File_Other;

	// Replace uppercase letters by lowercase letters
	size_t extLen = extension.size();
	for (size_t i = 1; i < extLen; i++)
	{
		extension[i] = tolower(extension[i]);
	}

	if (extension == ".png" || extension == ".jpg" || extension == ".bmp") //If the file is an image
	{
		fileType = File_Texture;
	}
	else if (extension == ".wav" || extension == ".mp3") //If the file is a sound/music
	{
		fileType = File_Audio;
	}
	else if (extension == ".obj") //If the file is a 3D object
	{
		fileType = File_Mesh;
	}
	else if (extension == ".xen") //If the file is a scene
	{
		fileType = File_Scene;
	}
	else if (extension == ".cpp") //If the file is a code file/header
	{
		fileType = File_Code;
	}
	else if (extension == ".h") //If the file is a code file/header
	{
		fileType = File_Header;
	}
	else if (extension == ".sky") //If the file is a skybox
	{
		fileType = File_Skybox;
	}
	else if (extension == ".ttf") //If the file is a font
	{
		fileType = File_Font;
	}
	else if (extension == ".mat") //If the file is a font
	{
		fileType = File_Material;
	}
	else if (extension == ".shader") //If the file is a font
	{
		fileType = File_Shader;
	}

	return fileType;
}

bool ProjectManager::LoadProject(const std::string& projectPathToLoad)
{
	Debug::Print("Loading project: " + projectPathToLoad);
	projectLoaded = false;

	projectFolderPath = projectPathToLoad;
	assetFolderPath = projectPathToLoad + "assets\\";
	engineAssetsFolderPath = ".\\engine_assets\\";

	projectDirectoryBase = std::make_shared<Directory>(assetFolderPath);
	if (!projectDirectoryBase->CheckIfExist())
	{
		return projectLoaded;
	}

	FindAllProjectFiles();

	LoadProjectSettings();
#if defined(EDITOR)
	SaveProjectSettings();
#endif

	//Load dynamic library and create game
#if defined(_WIN32) || defined(_WIN64)
#if defined(EDITOR)
	DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "game_editor");
#else
	DynamicLibrary::LoadGameLibrary("game");
#endif // defined(EDITOR)
	Engine::game = DynamicLibrary::CreateGame();
#else
	Engine::game = std::make_unique<Game>();
#endif //  defined(_WIN32) || defined(_WIN64)


	// Fill class registery
	if (Engine::game)
		Engine::game->Start();

#if defined(EDITOR)
	if (EngineSettings::compileWhenOpeningProject)
	{
		Compiler::HotReloadGame();
	}
#endif

	// Load start scene
	if (ProjectManager::GetStartScene())
	{
		SceneManager::LoadScene(ProjectManager::GetStartScene());
	}

	Debug::Print("Project loaded");
	projectLoaded = true;
#if defined(EDITOR)
	FileHandler::HasCodeChanged(GetAssetFolderPath());
	FileHandler::HasFileChangedOrAdded(GetAssetFolderPath());
#endif
	return projectLoaded;
}

void ProjectManager::UnloadProject()
{
#if defined(EDITOR)
	Editor::SetCurrentProjectDirectory(nullptr);
#endif
	SceneManager::SetOpenedScene(nullptr);
	SceneManager::CreateEmptyScene();
	Graphics::SetDefaultValues();

	projectSettings.startScene.reset();
	projectDirectoryBase.reset();
	projectDirectory.reset();
	projectFilesIds.clear();
	oldProjectFilesIds.clear();
	projectLoaded = false;
	projectSettings.projectName.clear();
	projectSettings.gameName.clear();
	Window::UpdateWindowTitle();
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceById(uint64_t id)
{
	std::shared_ptr<FileReference> fileRef = nullptr;

	// Find if the File Reference is already instanciated
	int fileRefCount = AssetManager::GetFileReferenceCount();
	for (int i = 0; i < fileRefCount; i++)
	{
		std::shared_ptr<FileReference> tempFileRef = AssetManager::GetFileReference(i);
		if (tempFileRef->fileId == id)
		{
			fileRef = std::move(tempFileRef);
			break;
		}
	}

	// If the file is not instanciated, create the File Reference
	if (fileRef == nullptr)
	{
		if (projectFilesIds.contains(id))
		{
			fileRef = CreateFilReference(projectFilesIds[id].path, id);
			if (fileRef)
			{
				if (fileRef->fileType == File_Skybox)
					fileRef->LoadFileReference();
			}
		}
	}

	return fileRef;
}

ProjectSettings ProjectManager::GetProjectSettings(const std::string &path)
{
	ProjectSettings settings;
	std::shared_ptr<File> projectFile = FileSystem::MakeFile(path + PROJECT_SETTINGS_FILE_NAME);
	if (projectFile->CheckIfExist())
	{
		std::string jsonString = "";

		// Read file
		if (projectFile->Open(true))
		{
			jsonString = projectFile->ReadAll();
			projectFile->Close();

		}
		else
		{
			Debug::PrintError("[ProjectManager::LoadProjectSettings] Fail to open the project settings file");
		}

		if (!jsonString.empty())
		{
			// Parse Json
			json projectData;
			try
			{
				projectData = json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadProjectSettings] Meta file error");
				return settings;
			}

			// Change settings
			ReflectionUtils::JsonToReflectiveData(projectData, settings.GetReflectiveData());
		}
	}
	return settings;
}

void ProjectManager::LoadProjectSettings()
{
	projectSettings = GetProjectSettings(projectFolderPath);
}

void ProjectManager::SaveProjectSettings()
{
	std::string path = projectFolderPath + PROJECT_SETTINGS_FILE_NAME;
	FileSystem::fileSystem->DeleteFile(path);
	json projectData;

	projectData["Values"] = ReflectionUtils::ReflectiveDataToJson(projectSettings.GetReflectiveData());

	std::shared_ptr<File> projectFile = FileSystem::MakeFile(path);
	if (projectFile->Open(true))
	{
		projectFile->Write(projectData.dump(0));
		projectFile->Close();
	}
	else
	{
		Debug::PrintError("[ProjectManager::SaveProjectSettings] Cannot save project settings: " + path);
	}
}

void ProjectManager::SaveMetaFile(const std::shared_ptr<FileReference>& fileReference)
{
	std::shared_ptr<File> file = fileReference->file;
	FileSystem::fileSystem->DeleteFile(file->GetPath() + META_EXTENSION);
	json metaData;
	metaData["id"] = fileReference->fileId;
	metaData["Values"] = ReflectionUtils::ReflectiveDataToJson(fileReference->GetMetaReflectiveData());

	std::shared_ptr<File> metaFile = FileSystem::MakeFile(file->GetPath() + META_EXTENSION);
	if (metaFile->Open(true))
	{
		metaFile->Write(metaData.dump(0));
		metaFile->Close();
	}
	else
	{
		Debug::PrintError("[ProjectManager::SaveMetaFile] Cannot save meta file: " + file->GetPath());
	}
}

std::vector<ProjectListItem> ProjectManager::GetProjectsList()
{
	std::vector<ProjectListItem> projects;
	std::shared_ptr<File> file = FileSystem::MakeFile(PROJECTS_LIST_FILE);
	bool isOpen = file->Open(false);
	if (isOpen)
	{
		const std::string projectFileString = file->ReadAll();
		if (!projectFileString.empty())
		{
			json j;
			try
			{
				j = json::parse(projectFileString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::GetProjectsList] Fail to load projects list: " + file->GetPath());
			}

			size_t projectCount = j.size();
			for (size_t i = 0; i < projectCount; i++)
			{
				// Get project informations (name and path)
				ProjectListItem projectItem;
				projectItem.path = j[i]["path"];
				ProjectSettings settings = GetProjectSettings(projectItem.path);
				if (settings.projectName.empty()) 
				{
					projectItem.name = j[i]["name"];
				}
				else 
				{
					projectItem.name = settings.projectName;
				}
				projects.push_back(projectItem);
			}
		}
		file->Close();
	}
	return projects;
}

void ProjectManager::SaveProjectsList(const std::vector<ProjectListItem>& projects)
{
	size_t projectSize = projects.size();
	json j;
	for (size_t i = 0; i < projectSize; i++)
	{
		j[i]["name"] = projects[i].name;
		j[i]["path"] = projects[i].path;
	}
	FileSystem::fileSystem->DeleteFile(PROJECTS_LIST_FILE);
	std::shared_ptr<File> file = FileSystem::MakeFile(PROJECTS_LIST_FILE);
	if (file->Open(true))
	{
		file->Write(j.dump(3));
		file->Close();
	}
	else
	{
		Debug::PrintError(std::string("[ProjectManager::SaveProjectsList] Cannot save projects list: ") + PROJECTS_LIST_FILE);
	}
}

std::shared_ptr<FileReference> ProjectManager::CreateFilReference(const std::string& path, uint64_t id)
{
	std::shared_ptr<FileReference> fileRef = nullptr;
	std::shared_ptr<File> file = FileSystem::MakeFile(path);

	FileType type = GetFileType(file->GetFileExtension());
	switch (type)
	{
	case File_Audio:
		fileRef = AudioClip::MakeAudioClip();
		break;
	case File_Mesh:
		fileRef = MeshData::MakeMeshData();
		break;
	case File_Texture:
		fileRef = Texture::MakeTexture();
		break;
	case File_Scene:
		fileRef = Scene::MakeScene();
		break;
	case File_Header:
		fileRef = CodeFile::MakeCode(true);
		break;
	case File_Code:
		fileRef = CodeFile::MakeCode(false);
		break;
	case File_Skybox:
		fileRef = SkyBox::MakeSkyBox();
		break;
	case File_Font:
		fileRef = Font::MakeFont();
		break;
	case File_Material:
		fileRef = Material::MakeMaterial();
		break;
	case File_Shader:
		fileRef = Shader::MakeShader();
		break;

	case File_Other:
		// Do nothing
		break;
	}

	if (fileRef)
	{
		fileRef->fileId = id;
		fileRef->file = file;
		fileRef->fileType = type;
		LoadMetaFile(fileRef);
#if defined(EDITOR)
		SaveMetaFile(fileRef);
#endif
	}
	return fileRef;
}

void ProjectManager::LoadMetaFile(const std::shared_ptr<FileReference>& fileReference)
{
	const std::string path = fileReference->file->GetPath() + META_EXTENSION;
	std::shared_ptr<File> metaFile = FileSystem::MakeFile(path);
	if (metaFile->CheckIfExist())
	{
		if (metaFile->Open(true))
		{
			std::string jsonString = "";
			jsonString = metaFile->ReadAll();
			metaFile->Close();

			json metaData;
			try
			{
				metaData = json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadMetaFile] Meta file error");
				return;
			}

			ReflectionUtils::JsonToReflectiveData(metaData, fileReference->GetMetaReflectiveData());
		}
		else
		{
			Debug::PrintError("[ProjectManager::LoadMetaFile] Cannot open the meta file" + path);
		}
	}
}

ProjectDirectory::~ProjectDirectory()
{
	subdirectories.clear();
}

std::string ProjectDirectory::GetFolderName()
{
	if (path.size() == 0)
		return "";
	size_t textLen = path.size();

	size_t lastSlashPos = path.find_last_of('\\', textLen - 2);

	std::string fileName = path.substr(lastSlashPos + 1, textLen - lastSlashPos - 2);

	return fileName;
}

ReflectiveData ProjectSettings::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, projectName, "projectName", true);
	Reflective::AddVariable(reflectedVariables, gameName, "gameName", true);
	Reflective::AddVariable(reflectedVariables, startScene, "startScene", true);
	return reflectedVariables;
}

