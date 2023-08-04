#include "project_manager.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"
#include <json.hpp>
#include "../reflection/reflection_utils.h"

using json = nlohmann::json;

std::unordered_map<uint64_t, FileReference*> ProjectManager::projectFilesRef;
ProjectDirectory* ProjectManager::projectDirectory = nullptr;
std::string ProjectManager::projectName = "";
std::string ProjectManager::gameName = "";
Scene *ProjectManager::startScene = nullptr;

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

void ProjectManager::LoadProject()
{
	std::string projectPath = "C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\project\\";

	//Get all files of the project
	Directory* projectDirectoryBase = new Directory(projectPath);
	std::vector<File*> projectFiles = projectDirectoryBase->GetAllFiles();

	projectDirectory = new ProjectDirectory(projectPath);
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
		std::string ext = file->GetFileExtention();
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
			delete metaFile;
		}
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
		}

		if (fileRef)
		{
			fileRef->fileId = kv.first->GetUniqueId();
			fileRef->file = kv.first;
			fileRef->fileType = kv.second;
			projectFilesRef[fileRef->fileId] = fileRef;
			LoadMetaFile(fileRef);
			SaveMetaFile(fileRef);
		}
	}

	SetProjectDirectory(projectDirectoryBase, projectDirectory);

	projectFiles.clear();
	allFoundFiles.clear();
	fileWithoutMeta.clear();
	compatibleFiles.clear();
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
	File *projectFile = new File(projectPath + "project_settings.json");
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

		ReflectionUtils::JsonToMap(GetProjetSettingsReflection(), projectData);
	}
}

void ProjectManager::SaveProjectSettigs() 
{
	FileSystem::fileSystem->DeleteFile(projectPath + "project_settings.json");
	json projectData;

	projectData["Values"] = ReflectionUtils::MapToJson(GetProjetSettingsReflection(), projectData);

	File *projectFile = new File(projectPath + "project_settings.json");
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
	metaData["Values"] = ReflectionUtils::MapToJson(fileReference->GetMetaReflection(), metaData);

	File* metaFile = new File(fileReference->file->GetPath() + ".meta");
	metaFile->Open(true);
	metaFile->Write(metaData.dump(0));
	metaFile->Close();
	delete metaFile;
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

		ReflectionUtils::JsonToMap(fileReference->GetMetaReflection(), metaData);
	}
}

std::string ProjectDirectory::GetFolderName()
{
	if (path.size() == 0)
		return "";
	int textLen = path.size();

	int lastSlashPos = path.find_last_of('\\', textLen - 2);
	if (lastSlashPos == -1)
		lastSlashPos = 0;
	std::string fileName = path.substr(lastSlashPos + 1, textLen - lastSlashPos - 2);

	return fileName;
}
