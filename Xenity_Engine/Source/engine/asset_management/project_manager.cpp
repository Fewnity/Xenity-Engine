#include "project_manager.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"
#include <json.hpp>
#include "../reflection/reflection_utils.h"
#include "../dynamic_lib/dynamic_lib.h"
#include "../scene_management/scene_manager.h"
#include "../game_interface.h"
#include "code_file.h"
#include "../graphics/skybox.h"

#if !defined(EDITOR)
#include "../../game_test/game.h"
#endif

using json = nlohmann::json;

std::unordered_map<uint64_t, FileChange> ProjectManager::oldProjectFilesIds;
std::unordered_map<uint64_t, FileAndPath> ProjectManager::projectFilesIds;
ProjectDirectory* ProjectManager::projectDirectory = nullptr;
std::string ProjectManager::projectName = "";
std::string ProjectManager::gameName = "";
std::shared_ptr<Scene> ProjectManager::startScene = nullptr;
std::string ProjectManager::projectFolderPath = "";
std::string ProjectManager::assetFolderPath = "";
std::string ProjectManager::engineAssetsFolderPath = "";
bool ProjectManager::projectLoaded;
Directory* ProjectManager::projectDirectoryBase = nullptr;

ProjectDirectory* ProjectManager::FindProjectDirectory(ProjectDirectory* directoryToCheck, std::string directoryPath)
{
	int dirCount = directoryToCheck->subdirectories.size();
	for (int i = 0; i < dirCount; i++)
	{
		ProjectDirectory* subDir = directoryToCheck->subdirectories[i];
		if (subDir->path == directoryPath)
		{
			return subDir;
		}
		else
		{
			ProjectDirectory* foundSubDir = FindProjectDirectory(subDir, directoryPath);
			if (foundSubDir)
				return foundSubDir;
		}
	}
	return nullptr;
}

void ProjectManager::FindAllProjectFiles()
{
	std::string oldPath;
	if (Engine::GetCurrentProjectDirectory())
		oldPath = Engine::GetCurrentProjectDirectory()->path;

	Engine::SetCurrentProjectDirectory(nullptr);

	for (auto& kv : projectFilesIds)
	{
		FileChange fileChange = FileChange();
		fileChange.path = kv.second.path;
		oldProjectFilesIds[kv.first] = fileChange;
	}

	projectFilesIds.clear();
	//Get all files of the project
	std::vector<std::shared_ptr<File>> projectFiles = projectDirectoryBase->GetAllFiles(true);
	if (projectDirectory)
		delete projectDirectory;

	projectDirectory = new ProjectDirectory(assetFolderPath);

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
		std::shared_ptr<File> metaFile = FileSystem::MakeFile(file->GetPath() + ".meta");
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


	for (auto& kv : compatibleFiles)
	{
		FileAndPath fileAndPath = FileAndPath();
		fileAndPath.file = kv.first;
		fileAndPath.path = kv.first->GetPath();
		projectFilesIds[kv.first->GetUniqueId()] = fileAndPath;
	}

	for (auto& kv : projectFilesIds)
	{
		CreateFilReference(kv.second.path, kv.first);
	}

	for (auto& kv : projectFilesIds)
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

	for (auto& kv : oldProjectFilesIds)
	{
		if (kv.second.hasChanged) 
		{
			GetFileReferenceById(kv.first)->file = projectFilesIds[kv.first].file;
			Debug::Print("File renamed: " + std::to_string(kv.first));
		}
		else if (kv.second.hasBeenDeleted)
		{
			AssetManager::ForceDeleteFileReference(GetFileReferenceById(kv.first));
			Debug::Print("File not found: " + std::to_string(kv.first));
		}
	}


#if defined(EDITOR)
	CreateProjectDirectories(projectDirectoryBase, projectDirectory);
	ProjectDirectory* lastOpenedDir = FindProjectDirectory(projectDirectory, oldPath);
	if (lastOpenedDir)
		Engine::SetCurrentProjectDirectory(lastOpenedDir);
	else
		Engine::SetCurrentProjectDirectory(projectDirectory);
#endif

	oldProjectFilesIds.clear();
	projectFiles.clear();
	allFoundFiles.clear();
	fileWithoutMeta.clear();
	compatibleFiles.clear();
}

void ProjectManager::CreateProjectDirectories(Directory* projectDirectoryBase, ProjectDirectory* realProjectDirectory)
{
	int dirCount = projectDirectoryBase->subdirectories.size();
	for (int i = 0; i < dirCount; i++)
	{
		ProjectDirectory* newDir = new ProjectDirectory(projectDirectoryBase->subdirectories[i]->GetPath());
		realProjectDirectory->subdirectories.push_back(newDir);
		CreateProjectDirectories(projectDirectoryBase->subdirectories[i], newDir);
	}
}

void ProjectManager::RefreshProjectDirectory()
{
	FindAllProjectFiles();
}

void ProjectManager::FillProjectDirectory(ProjectDirectory* realProjectDirectory)
{
	realProjectDirectory->files.clear();

	for (auto& kv : ProjectManager::projectFilesIds)
	{
		if (realProjectDirectory->path == kv.second.file->GetFolderPath())
		{
			realProjectDirectory->files.push_back(ProjectManager::GetFileReferenceById(kv.first));
		}
	}
}

bool ProjectManager::CreateProject(std::string name, std::string folderPath)
{
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\");

	return LoadProject(folderPath + name + "\\");
}

FileType ProjectManager::GetFileType(std::string extension)
{
	FileType fileType = File_Other;

	int extLen = extension.size();
	for (int i = 1; i < extLen; i++)
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
	else if (extension == ".h" || extension == ".cpp") //If the file is a scene
	{
		fileType = File_Code;
	}
	else if (extension == ".sky") //If the file is a scene
	{
		fileType = File_Skybox;
	}
	else if (extension == ".ttf") //If the file is a scene
	{
		fileType = File_Font;
	}

	return fileType;
}

bool ProjectManager::LoadProject(std::string projectPathToLoad)
{
	Debug::Print("Loading project: " + projectPathToLoad);
	projectLoaded = false;

	projectFolderPath = projectPathToLoad;
	assetFolderPath = projectPathToLoad + "assets\\";
	engineAssetsFolderPath = ".\\engine_assets\\";

	projectDirectoryBase = new Directory(assetFolderPath);
	if (!projectDirectoryBase->CheckIfExist())
	{
		return projectLoaded;
	}

	FindAllProjectFiles();

	LoadProjectSettings();
#if defined(EDITOR)
	SaveProjectSettigs();
#endif

#if defined(_WIN32) || defined(_WIN64)
#if defined(EDITOR)
	DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "game_editor");
#else
	DynamicLibrary::LoadGameLibrary("game");
#endif
	Engine::game = DynamicLibrary::CreateGame();
#else
	Engine::game = new Game();
#endif

	// Fill class registery
	if (Engine::game)
		Engine::game->Start();

	// Load start scene
	if (ProjectManager::GetStartScene())
	{
		SceneManager::LoadScene(ProjectManager::GetStartScene());
	}

	Debug::Print("Project loaded");
	projectLoaded = true;
	return projectLoaded;
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceById(uint64_t id)
{
	std::shared_ptr<FileReference> fileRef = nullptr;
	int fileRefCount = AssetManager::GetFileReferenceCount();
	for (int i = 0; i < fileRefCount; i++)
	{
		std::shared_ptr<FileReference> tempFileRef = AssetManager::GetFileReference(i);
		if (tempFileRef->fileId == id)
		{
			fileRef = tempFileRef;
			break;
		}
	}

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

void ProjectManager::LoadProjectSettings()
{
	std::shared_ptr<File> projectFile = FileSystem::MakeFile(projectFolderPath + PROJECT_SETTINGS_FILE_NAME);
	if (projectFile->CheckIfExist())
	{
		std::string jsonString = "";
		projectFile->Open(true);
		jsonString = projectFile->ReadAll();
		projectFile->Close();

		json projectData;
		try
		{
			projectData = json::parse(jsonString);
		}
		catch (const std::exception&)
		{
			Debug::PrintError("Meta file error");
			return;
		}

		ReflectionUtils::JsonToMap(projectData, GetProjetSettingsReflection());
	}
}

void ProjectManager::SaveProjectSettigs()
{
	FileSystem::fileSystem->DeleteFile(projectFolderPath + PROJECT_SETTINGS_FILE_NAME);
	json projectData;

	projectData["Values"] = ReflectionUtils::MapToJson(GetProjetSettingsReflection());

	std::shared_ptr<File> projectFile = FileSystem::MakeFile(projectFolderPath + PROJECT_SETTINGS_FILE_NAME);
	projectFile->Open(true);
	projectFile->Write(projectData.dump(0));
	projectFile->Close();
}

void ProjectManager::SaveMetaFile(std::shared_ptr<FileReference> fileReference)
{
	FileSystem::fileSystem->DeleteFile(fileReference->file->GetPath() + ".meta");
	json metaData;
	metaData["id"] = fileReference->fileId;
	metaData["Values"] = ReflectionUtils::MapToJson(fileReference->GetMetaReflection());

	std::shared_ptr<File> metaFile = FileSystem::MakeFile(fileReference->file->GetPath() + ".meta");
	metaFile->Open(true);
	metaFile->Write(metaData.dump(0));
	metaFile->Close();
}

std::vector<ProjectListItem> ProjectManager::GetProjectsList()
{
	std::vector<ProjectListItem> projects;
	std::shared_ptr<File> file = FileSystem::MakeFile("projects.json");
	bool isOpen = file->Open(false);
	if (isOpen)
	{
		json j;
		j = json::parse(file->ReadAll());

		int projectCount = j.size();
		for (int i = 0; i < projectCount; i++)
		{
			ProjectListItem projectItem;
			projectItem.name = j[i]["name"];
			projectItem.path = j[i]["path"];
			projects.push_back(projectItem);
		}
	}
	file->Close();
	return projects;
}

void ProjectManager::SaveProjectsList(std::vector<ProjectListItem> projects)
{
	int projectSize = projects.size();
	json j;
	for (int i = 0; i < projectSize; i++)
	{
		j[i]["name"] = projects[i].name;
		j[i]["path"] = projects[i].path;
	}
	FileSystem::fileSystem->DeleteFile("projects.json");
	std::shared_ptr<File> file = FileSystem::MakeFile("projects.json");
	file->Open(true);
	file->Write(j.dump(3));
	file->Close();
}

std::shared_ptr<FileReference> ProjectManager::CreateFilReference(std::string path, int id)
{
	std::shared_ptr<FileReference> fileRef = nullptr;
	std::shared_ptr<File> file = FileSystem::MakeFile(path);
	FileType type = GetFileType(file->GetFileExtension());
	switch (type)
	{
	case File_Audio:
		fileRef = AudioClip::MakeAudioClip(file->GetPath());
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
	case File_Code:
		fileRef = CodeFile::MakeScene(file->GetFileExtension());
		break;
	case File_Skybox:
		fileRef = SkyBox::MakeSkyBox();
		break;
	case File_Font:
		fileRef = Font::MakeFont();
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
		//if (type == File_Skybox)
			//fileRef->LoadFileReference();
	}
	return fileRef;
}

void ProjectManager::LoadMetaFile(std::shared_ptr<FileReference> fileReference)
{
	std::shared_ptr<File> metaFile = FileSystem::MakeFile(fileReference->file->GetPath() + ".meta");
	if (metaFile->CheckIfExist())
	{
		std::string jsonString = "";
		metaFile->Open(true);
		jsonString = metaFile->ReadAll();
		metaFile->Close();

		json metaData;
		try
		{
			metaData = json::parse(jsonString);
		}
		catch (const std::exception&)
		{
			Debug::PrintError("Meta file error");
			return;
		}

		ReflectionUtils::JsonToMap(metaData, fileReference->GetMetaReflection());
	}
}

ProjectDirectory::~ProjectDirectory()
{
	int dirCount = subdirectories.size();
	for (int i = 0; i < dirCount; i++)
	{
		delete subdirectories[i];
	}
	subdirectories.clear();
}

std::string ProjectDirectory::GetFolderName()
{
	if (path.size() == 0)
		return "";
	int textLen = path.size();

	int lastSlashPos = path.find_last_of('\\', textLen - 2);

	std::string fileName = path.substr(lastSlashPos + 1, textLen - lastSlashPos - 2);

	return fileName;
}

std::unordered_map<std::string, ReflectionEntry> ProjectManager::GetProjetSettingsReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, projectName, "projectName", true);
	Reflection::AddReflectionVariable(reflectedVariables, gameName, "gameName", true);
	Reflection::AddReflectionVariable(reflectedVariables, startScene, "startScene", true);
	return reflectedVariables;
}