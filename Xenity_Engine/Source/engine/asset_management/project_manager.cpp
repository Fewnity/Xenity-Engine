#include "project_manager.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"
#include <json.hpp>
#include "../reflection/reflection_utils.h"
#include "../dynamic_lib/dynamic_lib.h"
#include "../scene_management/scene_manager.h"
#include "../game_interface.h"
#include "code_file.h"

#if !defined(EDITOR)
#include "../../game_test/game.h"
#endif

using json = nlohmann::json;

std::unordered_map<uint64_t, FileReference*> ProjectManager::projectFilesRef;
ProjectDirectory* ProjectManager::projectDirectory = nullptr;
std::string ProjectManager::projectName = "";
std::string ProjectManager::gameName = "";
Scene *ProjectManager::startScene = nullptr;
std::string ProjectManager::projectFolderPath = "";
std::string ProjectManager::assetFolderPath = "";
std::string ProjectManager::engineAssetsFolderPath = "";
bool ProjectManager::projectLoaded;

void SetProjectDirectory(Directory* projectDirectoryBase, ProjectDirectory* realProjectDirectory)
{
	int dirCount = projectDirectoryBase->subdirectories.size();
	for (auto kv : ProjectManager::projectFilesRef)
	{
		if (realProjectDirectory->path == kv.second->file->GetFolderPath())
		{
			realProjectDirectory->files.push_back(kv.second);
		}
	}
	for (int i = 0; i < dirCount; i++)
	{
		ProjectDirectory *newDir = new ProjectDirectory(projectDirectoryBase->subdirectories[i]->GetPath());
		realProjectDirectory->subdirectories.push_back(newDir);
		SetProjectDirectory(projectDirectoryBase->subdirectories[i], newDir);
	}
}

bool ProjectManager::CreateProject(std::string name, std::string folderPath)
{
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\");

	return LoadProject(folderPath + name + "\\");
}

bool ProjectManager::LoadProject(std::string projectPathToLoad)
{
	Debug::Print("Loading project: " + projectPathToLoad);

	for (auto fileRef : projectFilesRef)
	{
		delete fileRef.second;
	}
	projectFilesRef.clear();

	projectLoaded = false;
	projectFolderPath = projectPathToLoad;
	assetFolderPath = projectPathToLoad + "assets\\";
	engineAssetsFolderPath = ".\\engine_assets\\";

	//Get all files of the project
	Directory* projectDirectoryBase = new Directory(assetFolderPath);
	if (!projectDirectoryBase->CheckIfExist())
	{
		return projectLoaded;
	}

	std::vector<File*> projectFiles = projectDirectoryBase->GetAllFiles();

	projectDirectory = new ProjectDirectory(assetFolderPath);
	Engine::currentProjectDirectory = projectDirectory;
	std::vector<File*> allFoundFiles;
	std::unordered_map<File*, FileType> compatibleFiles;

	//Get all files supported by the engine
	int fileCount = (int)projectFiles.size();
	int allFoundFileCount = 0;
	for (int i = 0; i < fileCount; i++)
	{
		File* file = projectFiles[i];

		//Get the extention in lowercase
		std::string ext = file->GetFileExtension();
		int extLen = ext.size();
		for (int i = 1; i < extLen; i++)
		{
			ext[i] = tolower(ext[i]);
		}

		if (ext == ".png" || ext == ".jpg" || ext == ".bmp") //If the file is an image
		{
			compatibleFiles[file] = File_Texture;
		}
		else if (ext == ".wav" || ext == ".mp3") //If the file is a sound/music
		{
			compatibleFiles[file] = File_Audio;
		}
		else if (ext == ".obj") //If the file is a 3D object
		{
			compatibleFiles[file] = File_Mesh;
		}
		else if (ext == ".xen") //If the file is a scene
		{
			compatibleFiles[file] = File_Scene;
		}
		else if (ext == ".h" || ext == ".cpp") //If the file is a scene
		{
			compatibleFiles[file] = File_Code;
		}
		else
		{
			continue;
		}
		allFoundFiles.push_back(file);
		allFoundFileCount++;
	}

	std::vector<File*> fileWithoutMeta;
	int fileWithoutMetaCount = 0;
	uint64_t biggestId = 0;
	for (int i = 0; i < allFoundFileCount; i++)
	{
		File* file = allFoundFiles[i];
		File* metaFile = new File(file->GetPath() + ".meta");
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
		delete metaFile;
	}

	//Set new files ids
	UniqueId::lastFileUniqueId = biggestId;
	for (int i = 0; i < fileWithoutMetaCount; i++)
	{
		uint64_t id = UniqueId::GenerateUniqueId(true);
		fileWithoutMeta[i]->SetUniqueId(id);
	}

	//Create empty textures, meshes...
	for (auto kv : compatibleFiles)
	{
		FileReference* fileRef = nullptr;
		switch (kv.second)
		{
		case File_Audio:
			fileRef = new AudioClip(kv.first->GetPath());
			break;
		case File_Mesh:
			fileRef = new MeshData();
			break;
		case File_Texture:
			fileRef = new Texture();
			break;
		case File_Scene:
			fileRef = new Scene();
			break;
		case File_Code:
			fileRef = new CodeFile(kv.first->GetFileExtension());
			break;
		}

		if (fileRef)
		{
			fileRef->fileId = kv.first->GetUniqueId();
			fileRef->file = kv.first;
			fileRef->fileType = kv.second;
			projectFilesRef[fileRef->fileId] = fileRef;
			LoadMetaFile(fileRef);
#if defined(EDITOR)
			SaveMetaFile(fileRef);
#endif
		}
		else 
		{
			Debug::PrintError("[ProjectManager::LoadProject] Supported file not created");
		}
	}

	SetProjectDirectory(projectDirectoryBase, projectDirectory);

	projectFiles.clear();
	allFoundFiles.clear();
	fileWithoutMeta.clear();
	compatibleFiles.clear();

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

FileReference* ProjectManager::GetFileReferenceById(uint64_t id)
{
	FileReference *fileRef = nullptr;
	if (id != -1)
	{
		auto it = projectFilesRef.find(id);
		if (it != projectFilesRef.end())
		{
			fileRef = it->second;
		}
	}
	return fileRef;
}

void ProjectManager::LoadProjectSettings() 
{
	File *projectFile = new File(projectFolderPath + PROJECT_SETTINGS_FILE_NAME);
	if (projectFile->CheckIfExist())
	{
		std::string jsonString = "";
		projectFile->Open(true);
		jsonString = projectFile->ReadAll();
		projectFile->Close();
		delete projectFile;

		json projectData;
		try
		{
			projectData = json::parse(jsonString);
		}
		catch (const std::exception &)
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

	File *projectFile = new File(projectFolderPath + PROJECT_SETTINGS_FILE_NAME);
	projectFile->Open(true);
	projectFile->Write(projectData.dump(0));
	projectFile->Close();
	delete projectFile;
}

void ProjectManager::SaveMetaFile(FileReference* fileReference)
{
	FileSystem::fileSystem->DeleteFile(fileReference->file->GetPath() + ".meta");
	json metaData;
	metaData["id"] = fileReference->fileId;
	metaData["Values"] = ReflectionUtils::MapToJson(fileReference->GetMetaReflection());

	File* metaFile = new File(fileReference->file->GetPath() + ".meta");
	metaFile->Open(true);
	metaFile->Write(metaData.dump(0));
	metaFile->Close();
	delete metaFile;
}

std::vector<ProjectListItem> ProjectManager::GetProjectsList()
{
	std::vector<ProjectListItem> projects;
	File* file = new File("projects.json");
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
	File* file = new File("projects.json");
	file->Open(true);
	file->Write(j.dump(3));
	file->Close();
}

void ProjectManager::LoadMetaFile(FileReference* fileReference)
{
	File* metaFile = new File(fileReference->file->GetPath() + ".meta");
	if (metaFile->CheckIfExist())
	{
		std::string jsonString = "";
		metaFile->Open(true);
		jsonString = metaFile->ReadAll();
		metaFile->Close();
		delete metaFile;

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

std::string ProjectDirectory::GetFolderName()
{
	if (path.size() == 0)
		return "";
	int textLen = path.size();

	int lastSlashPos = path.find_last_of('\\', textLen - 2);

	std::string fileName = path.substr(lastSlashPos + 1, textLen - lastSlashPos - 2);

	return fileName;
}

std::unordered_map<std::string, Variable> ProjectManager::GetProjetSettingsReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("projectName", projectName);
	reflectedVariables.insert_or_assign("gameName", gameName);
	reflectedVariables.insert_or_assign("startScene", startScene);
	return reflectedVariables;
}