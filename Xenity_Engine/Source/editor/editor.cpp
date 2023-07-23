#include "editor.h"
#include "../xenity.h"
#include "../xenity_editor.h"
#include <functional>
#include "../engine/class_registry/class_registry.h"
#include "../engine/file_system/file_reference.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h";

using json = nlohmann::json;

std::weak_ptr<GameObject> Editor::cameraGO;

std::vector<std::shared_ptr<Component>> Editor::allCreatedComponents;
int allCreatedComponentsCount = 0;

void Editor::JsonToReflection(json j, Reflection& component)
{
	for (auto& kv : j["Values"].items())
	{
		auto t = component.GetReflection();
		if (t.contains(kv.key()))
		{
			Variable& variableRef = t.at(kv.key());
			if (kv.value().is_object())
			{
				//std::cout << kv.key() << " is an object!" << "\n";
				if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
				{
					JsonToReflection(kv.value(), valuePtr->get());
				}
			}
			else
			{
				if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if<std::weak_ptr<GameObject>*>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					**valuePtr = go;
				}
				else if (auto valuePtr = std::get_if<std::weak_ptr<Transform>*>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					**valuePtr = go->GetTransform();
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
				{
					int fileId = kv.value();
					int c = Editor::fileRefs.size();
					for (int i = 0; i < c; i++)
					{
						if (Editor::fileRefs[i]->fileId == fileId)
						{
							valuePtr->get() = (Texture*)(Editor::fileRefs[i]);
						}
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
				{
					int fileId = kv.value();
					int c = Editor::fileRefs.size();
					for (int i = 0; i < c; i++)
					{
						if (Editor::fileRefs[i]->fileId == fileId)
						{
							valuePtr->get() = (MeshData*)(Editor::fileRefs[i]);
						}
					}
				}
				/*else if (auto valuePtr = std::get_if<void*>(&variableRef))
				{
					std::weak_ptr<Component>* weakC = (std::weak_ptr<Component>*)(*valuePtr);

					for (int compI = 0; compI < allCreatedComponentsCount; compI++)
					{
						if (allCreatedComponents[compI]->GetUniqueId() == kv.value())
						{
							*weakC = allCreatedComponents[compI];
							break;
						}
					}
				}*/
			}
		}
		// std::cout << kv.key() << " : " << kv.value() << "\n";
	}
}

std::vector<FileReference*> Editor::fileRefs;

void Editor::Start()
{
	Directory* projectDirectory = new Directory("C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\project");
	std::vector<File*> allFoundFiles;
	std::vector<File*> projectFiles = projectDirectory->GetAllFiles();
	std::vector<File*> texturesFiles;
	std::vector<File*> meshFiles;
	std::vector<File*> audioFiles;

	int fileCount = (int)projectFiles.size();
	for (int i = 0; i < fileCount; i++)
	{
		File* file = projectFiles[i];
		std::string ext = projectFiles[i]->GetFileExtention();

		if (ext == ".png" || ext == ".jpg" || ext == ".bmp")
		{
			texturesFiles.push_back(file);
		}
		else if (ext == ".wav" || ext == ".mp3")
		{
			audioFiles.push_back(file);
		}
		else if (ext == ".obj")
		{
			meshFiles.push_back(file);
		}
		else
		{
			continue;
		}
		allFoundFiles.push_back(file);
	}
	int allFoundFileCount = (int)allFoundFiles.size();
	std::vector<File*> fileWithoutMeta;
	int fileWithoutMetaCount = 0;
	uint64_t biggestId = 0;
	for (int i = 0; i < allFoundFileCount; i++)
	{
		File* metaFile = new File(allFoundFiles[i]->GetPath() + ".meta");
		if (!metaFile->CheckIfExist())
		{
			fileWithoutMeta.push_back(metaFile);
			fileWithoutMetaCount++;
		}
		else
		{
			//Read meta
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
	UniqueId::lastFileUniqueId = biggestId;
	for (int i = 0; i < fileWithoutMetaCount; i++)
	{
		File* file = fileWithoutMeta[i];
		file->SetUniqueId(file->GenerateUniqueId());
		file->Open(true);
		json metaData;
		metaData["id"] = file->GetUniqueId();
		file->Write(metaData.dump(0));
		file->Close();
		delete file;
	}

	int textureCount = (int)texturesFiles.size();
	for (int i = 0; i < textureCount; i++)
	{
		FileReference* texture = new Texture(texturesFiles[i]->GetPath(), texturesFiles[i]->GetPath(), true);
		texture->fileId = texturesFiles[i]->GetUniqueId();
		texture->file = texturesFiles[i];
		texture->fileType = File_Texture;
		fileRefs.push_back(texture);
	}
	int meshCount = (int)meshFiles.size();
	for (int i = 0; i < meshCount; i++)
	{
		FileReference* mesh = WavefrontLoader::LoadFromRawData(meshFiles[i]->GetPath());
		mesh->fileId = meshFiles[i]->GetUniqueId();
		mesh->file = meshFiles[i];
		mesh->fileType = File_Mesh;
		fileRefs.push_back(mesh);
	}
	int audioCount = (int)audioFiles.size();
	for (int i = 0; i < audioCount; i++)
	{
		FileReference* audioClip = new AudioClip(audioFiles[i]->GetPath());
		audioClip->fileId = audioFiles[i]->GetUniqueId();
		audioClip->file = audioFiles[i];
		audioClip->fileType = File_Audio;
		fileRefs.push_back(audioClip);
	}

	projectFiles.clear();
	texturesFiles.clear();
	meshFiles.clear();
	audioFiles.clear();
	allFoundFiles.clear();

	cameraGO = CreateGameObjectEditor("Camera");
	auto camera = cameraGO.lock()->AddComponent<Camera>();
	camera->SetNearClippingPlane(0.01f);
	camera->SetFarClippingPlane(500);
	camera->SetProjectionSize(5.0f);
	camera->SetFov(70);

	File *jsonFile = new File("scene.txt");
	bool isOpen = jsonFile->Open(false);
	if (isOpen)
	{
		std::string jsonString = jsonFile->ReadAll();
		jsonFile->Close();

		json data;
		try
		{
			data = json::parse(jsonString);
		}
		catch (const std::exception &)
		{
			Debug::PrintError("Scene file error");
			return;
		}

		// Create all GameObjects and Components
		for (auto &kv : data["GameObjects"].items())
		{
			auto go = CreateGameObject();
			go->SetUniqueId(std::stoull(kv.key()));
			JsonToReflection(kv.value(), *go.get());
			for (auto &kv2 : kv.value()["Components"].items())
			{
				auto comp = ClassRegistry::AddComponentFromName(kv2.value()["Type"], go);
				comp->SetUniqueId(std::stoull(kv2.key()));
				allCreatedComponents.push_back(comp);
			}
		}

		allCreatedComponentsCount = (int)allCreatedComponents.size();

		// Bind Components values and GameObjects childs
		for (auto &kv : data["GameObjects"].items())
		{
			auto go = FindGameObjectById(std::stoull(kv.key()));
			if (go)
			{
				for (auto &kv2 : kv.value()["Childs"].items())
				{
					auto goChild = FindGameObjectById(kv2.value());
					if (goChild)
					{
						goChild->SetParent(go);
					}
				}
			}

			JsonToReflection(kv.value()["Transform"], *go->GetTransform().get());
			go->GetTransform()->isTransformationMatrixDirty = true;
			go->GetTransform()->UpdateWorldValues();

			for (auto &kv2 : kv.value()["Components"].items())
			{
				int componentCount = go->GetComponentCount();
				for (int compI = 0; compI < componentCount; compI++)
				{
					if (go->components[compI]->GetUniqueId() == std::stoull(kv2.key()))
					{
						JsonToReflection(kv2.value(), *go->components[compI].get());
						break;
					}
				}
			}
		}
		allCreatedComponents.clear();
		allCreatedComponentsCount = 0;
	}
	delete jsonFile;
	SaveScene();
}

void Editor::Update()
{
	Vector3 rot = cameraGO.lock()->GetTransform()->GetRotation();
	Vector3 pos = cameraGO.lock()->GetTransform()->GetPosition();

	if (InputSystem::GetKey(MOUSE_RIGHT))
	{
		rot.x += -InputSystem::mouseSpeed.y * Time::GetDeltaTime() * 20000;
		rot.y += InputSystem::mouseSpeed.x * Time::GetDeltaTime() * 20000;
	}

	float fwd = 0;
	float side = 0;
	if (InputSystem::GetKey(UP))
		fwd = -1;
	else if (InputSystem::GetKey(DOWN))
		fwd = 1;

	if (InputSystem::GetKey(RIGHT))
		side = 1;
	else if (InputSystem::GetKey(LEFT))
		side = -1;

	pos -= cameraGO.lock()->GetTransform()->GetForward() * (fwd / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraGO.lock()->GetTransform()->GetLeft() * (side / 7.0f) * Time::GetDeltaTime() * 30;

	cameraGO.lock()->GetTransform()->SetPosition(pos);
	cameraGO.lock()->GetTransform()->SetRotation(rot);
}

void Editor::CreateEmpty()
{
	auto go = CreateGameObject();
	Engine::SetSelectedGameObject(go);
}

void Editor::CreateEmptyChild()
{
	auto go = CreateGameObject();
	go->SetParent(Engine::selectedGameObject);
	Engine::SetSelectedGameObject(go);
}

#pragma region Save

json Editor::ReflectiveToJson(Reflection& relection)
{
	json j2;
	auto t = relection.GetReflection();
	for (const auto& kv :t)
	{
		Variable& variableRef = t.at(kv.first);
		if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::reference_wrapper<double>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::weak_ptr<GameObject> *>(&variableRef))
		{
			if (auto lockValue = (**valuePtr).lock())
				j2[kv.first] = lockValue->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::weak_ptr<Transform> *>(&variableRef))
		{
			if (auto lockValue = (**valuePtr).lock())
				j2[kv.first] = lockValue->GetGameObject()->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			j2[kv.first]["Values"] = ReflectiveToJson(valuePtr->get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
		{
			if (auto lockValue = (valuePtr->get()).lock())
			j2[kv.first]["Values"] = ReflectiveToJson(*lockValue.get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				j2[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<AudioClip*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				j2[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				j2[kv.first] = (valuePtr->get())->fileId;
		}
		/*else if (auto valuePtr = std::get_if<void*>(&variableRef))
		{
			std::weak_ptr<Component>* weakC = (std::weak_ptr<Component>*)(*valuePtr);
			if (auto lockValue = weakC->lock())
			{
				j2[kv.first] = lockValue->GetUniqueId();
			}
		}*/
	}
	return j2;
}

void Editor::SaveScene()
{
	json j;
	int gameObjectCount = Engine::gameObjectCount;
	for (int goI = 0; goI < gameObjectCount; goI++)
	{
		auto go = Engine::gameObjects[goI];
		std::string goName = go->name;
		std::string goId = std::to_string(go->GetUniqueId());

		j["GameObjects"][goId]["Transform"]["Values"] = ReflectiveToJson(*go->GetTransform().get());
		j["GameObjects"][goId]["Values"] = ReflectiveToJson(*go.get());

		std::vector<uint64_t> ids;
		int childCount = go->GetChildrenCount();
		for (int childI = 0; childI < childCount; childI++)
		{
			ids.push_back(go->children[childI].lock()->GetUniqueId());
		}
		j["GameObjects"][goId]["Childs"] = ids;

		int componentCount = go->GetComponentCount();
		for (int componentI = 0; componentI < componentCount; componentI++)
		{
			auto component = go->components[componentI];
			std::string compName = component->GetComponentName();
			std::string compId = std::to_string(component->GetUniqueId());
			j["GameObjects"][goId]["Components"][compId]["Type"] = compName;
			j["GameObjects"][goId]["Components"][compId]["Values"] = ReflectiveToJson((*component.get()));
		}
	}

	std::string s = j.dump(2);
	// std::cout << s << std::endl;
	FileSystem::fileSystem->DeleteFile("scene2.txt");
	File* file = new File("scene2.txt");
	file->Open(true);
	file->Write(s);
	file->Close();
	delete file;
}

#pragma endregion