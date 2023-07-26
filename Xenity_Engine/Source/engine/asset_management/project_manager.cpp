#include "project_manager.h"
#include "../../xenity.h"
#include <json.hpp>

using json = nlohmann::json;

std::unordered_map<int, FileReference*> ProjectManager::projectFilesRef;

void ProjectManager::LoadProject()
{
	//Get all files of the project
	Directory* projectDirectory = new Directory("C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\project");
	std::vector<File*> projectFiles = projectDirectory->GetAllFiles();

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

	std::vector<PairFile> fileWithoutMeta;
	int fileWithoutMetaCount = 0;
	uint64_t biggestId = 0;
	for (int i = 0; i < allFoundFileCount; i++)
	{
		File* metaFile = new File(allFoundFiles[i]->GetPath() + ".meta");
		if (!metaFile->CheckIfExist()) //If there is not meta for this file
		{
			//Create one later
			PairFile pair;
			pair.file = allFoundFiles[i];
			pair.meta = metaFile;
			fileWithoutMeta.push_back(pair);
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
				allFoundFiles[i]->SetUniqueId(id);
			}
			delete metaFile;
		}
	}

	//Create meta
	UniqueId::lastFileUniqueId = biggestId;
	for (int i = 0; i < fileWithoutMetaCount; i++)
	{
		PairFile pair = fileWithoutMeta[i];
		int id = pair.file->GenerateUniqueId(true);
		pair.file->SetUniqueId(id);
		pair.meta->Open(true);

		//Create json
		json metaData;
		metaData["id"] = id;
		pair.meta->Write(metaData.dump(0));
		pair.meta->Close();
		delete pair.meta;
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
			fileRef = new Scene(kv.first->GetPath());
			break;
		}

		if (fileRef)
		{
			fileRef->fileId = kv.first->GetUniqueId();
			fileRef->file = kv.first;
			fileRef->fileType = kv.second;
			projectFilesRef[fileRef->fileId] = fileRef;
		}
	}

	projectFiles.clear();
	allFoundFiles.clear();
	fileWithoutMeta.clear();
	compatibleFiles.clear();
}

FileReference* ProjectManager::GetFileReferenceById(int id)
{
	FileReference* fileRef = nullptr;
	auto it = projectFilesRef.find(id);
	if (it != projectFilesRef.end())
	{
		fileRef = it->second;
	}
	return fileRef;
}