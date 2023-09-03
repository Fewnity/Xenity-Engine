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

std::unordered_map<uint64_t, std::string> ProjectManager::projectFilesIds;
ProjectDirectory* ProjectManager::projectDirectory = nullptr;
std::string ProjectManager::projectName = "";
std::string ProjectManager::gameName = "";
std::shared_ptr<Scene> ProjectManager::startScene = nullptr;
std::string ProjectManager::projectFolderPath = "";
std::string ProjectManager::assetFolderPath = "";
std::string ProjectManager::engineAssetsFolderPath = "";
bool ProjectManager::projectLoaded;
Directory* ProjectManager::projectDirectoryBase = nullptr;

void ProjectManager::CreateProjectDirectories(Directory* projectDirectoryBase, ProjectDirectory* realProjectDirectory)
{
	int dirCount = projectDirectoryBase->subdirectories.size();
	for (int i = 0; i < dirCount; i++)
	{
		ProjectDirectory *newDir = new ProjectDirectory(projectDirectoryBase->subdirectories[i]->GetPath());
		realProjectDirectory->subdirectories.push_back(newDir);
		CreateProjectDirectories(projectDirectoryBase->subdirectories[i], newDir);
	}
}

void ProjectManager::FillProjectDirectory(ProjectDirectory* realProjectDirectory)
{
	realProjectDirectory->files.clear();

	for (auto& kv : ProjectManager::projectFilesIds)
	{
		File* file = new File(kv.second);

		if (realProjectDirectory->path == file->GetFolderPath())
		{
			realProjectDirectory->files.push_back(ProjectManager::GetFileReferenceById(kv.first));
		}
		delete file;
	}
}

bool ProjectManager::CreateProject(std::string name, std::string folderPath)
{
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\");
	FileSystem::fileSystem->CreateDirectory(folderPath + name + "\\assets\\");

	return LoadProject(folderPath + name + "\\");
}

FileType GetFileType(std::string extension) 
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

	return fileType;
}

bool ProjectManager::LoadProject(std::string projectPathToLoad)
{
	Debug::Print("Loading project: " + projectPathToLoad);

	projectFilesIds.clear();

	projectLoaded = false;
	projectFolderPath = projectPathToLoad;
	assetFolderPath = projectPathToLoad + "assets\\";
	engineAssetsFolderPath = ".\\engine_assets\\";

	//Get all files of the project
	projectDirectoryBase = new Directory(assetFolderPath);
	if (!projectDirectoryBase->CheckIfExist())
	{
		return projectLoaded;
	}

	std::vector<File*> projectFiles = projectDirectoryBase->GetAllFiles(true);

	projectDirectory = new ProjectDirectory(assetFolderPath);

	std::vector<File*> allFoundFiles;
	std::unordered_map<File*, FileType> compatibleFiles;

	//Get all files supported by the engine
	int fileCount = (int)projectFiles.size();
	int allFoundFileCount = 0;
	for (int i = 0; i < fileCount; i++)
	{
		File* file = projectFiles[i];
		FileType fileType = GetFileType(file->GetFileExtension());

		if (fileType != File_Other)
		{
			compatibleFiles[file] = fileType;
			allFoundFiles.push_back(file);
			allFoundFileCount++;
		}
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

	for (auto& kv : compatibleFiles)
	{
		projectFilesIds[kv.first->GetUniqueId()] = kv.first->GetPath();
	}

	CreateProjectDirectories(projectDirectoryBase, projectDirectory);
	FillProjectDirectory(projectDirectory);
	Engine::SetCurrentProjectDirectory(projectDirectory);

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

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceById(uint64_t id)
{
	std::shared_ptr<FileReference> fileRef = nullptr;
	int fileRefCount = AssetManager::GetFileReferenceCount2();
	for (int i = 0; i < fileRefCount; i++)
	{
		if (AssetManager::GetFileReference2(i)->fileId == id) 
		{
			fileRef = AssetManager::GetFileReference2(i);
		}
	}

	if (fileRef == nullptr) 
	{
		File* file = new File(projectFilesIds[id]);
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
	}
	/*if (id != -1)
	{
		auto it = projectFilesRef.find(id);
		if (it != projectFilesRef.end())
		{
			fileRef = it->second;
		}
	}*/
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

void ProjectManager::SaveMetaFile(std::shared_ptr<FileReference> fileReference)
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

void ProjectManager::LoadMetaFile(std::shared_ptr<FileReference> fileReference)
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