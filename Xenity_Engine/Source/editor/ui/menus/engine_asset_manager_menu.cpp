#include "engine_asset_manager_menu.h"

#include <imgui/imgui.h>
#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>
#include <engine/asset_management/project_manager.h>
#include <engine/file_system/file_system.h>
#include <editor/ui/editor_ui.h>
#include <engine/asset_management/asset_manager.h>

#include <json.hpp>
using json = nlohmann::json;

void EngineAssetManagerMenu::Init()
{
	//engineAssetsFiles = ProjectManager::publicEngineAssetsDirectoryBase->GetAllFiles(true);
	/*const int engineAssetsFileCount = (int)engineAssetsFiles.size();
	for (int i = 0; i < engineAssetsFileCount; i++)
	{
		projectFiles.push_back(engineAssetsFiles[i]);
	}
	engineAssetsFiles.clear();*/
}

void EngineAssetManagerMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Engine Assets Manager", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		if (ImGui::Button("Refresh list")) 
		{
			ProjectManager::RefreshProjectDirectory();
			engineAssetsFiles = ProjectManager::publicEngineAssetsDirectoryBase->GetAllFiles(true);

			ids.clear();
			int fileCount = engineAssetsFiles.size();
			for (int i = 0; i < fileCount; i++)
			{
				std::shared_ptr<File> file = engineAssetsFiles[i];
				if (file->GetPath().substr(file->GetPath().size() - 5) == ".meta") 
				{
					engineAssetsFiles.erase(engineAssetsFiles.begin() + i);
					i--;
					fileCount--;
					continue;
				}

				const std::string metaFilePath = file->GetPath() + META_EXTENSION;
				std::shared_ptr<File> metaFile = FileSystem::MakeFile(metaFilePath);

				if (metaFile->Open(FileMode::ReadOnly))
				{
					const std::string jsonString = metaFile->ReadAll();
					metaFile->Close();
					if (!jsonString.empty())
					{
						json data;
						try
						{
							data = json::parse(jsonString);
						}
						catch (const std::exception&)
						{
							ids.push_back(0);
						}
						ids.push_back(data["id"]);
					}
				}
			}
			oldIds = ids;
		}
		const int idCount = ids.size();
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			for (int i = 0; i < idCount; i++)
			{
				std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(oldIds[i]);
				fileRef->fileId = ids[i];
				fileRef->isMetaDirty = true;
				ProjectManager::SaveMetaFile(fileRef);
			}
			oldIds = ids;
		}
		if (ImGui::Button("Fix ids"))
		{
			int fileRefCount = AssetManager::GetFileReferenceCount();
			for (int i = 0; i < fileRefCount; i++)
			{
				AssetManager::GetFileReference(i)->fileId += UniqueId::reservedFileId;
				AssetManager::GetFileReference(i)->isMetaDirty = true;
			}

			for (int i = 0; i < fileRefCount; i++)
			{
				ProjectManager::SaveMetaFile(AssetManager::GetFileReference(i));
			}
		}
		ImGui::Separator();

		ImGui::Text("Engine Assets list");
		ImGui::Text("Only change asset ids when id is >= %lld", UniqueId::reservedFileId);
		if (ImGui::BeginTable("meta_file_table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_Resizable))
		{
			for (int i = 0; i < idCount; i++)
			{
				ImGui::TableNextRow(0, 0);
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", engineAssetsFiles[i]->GetFileName().c_str());
				ImGui::TableSetColumnIndex(1);
				EditorUI::DrawInputTemplate("Id", ids[i]);
			}
			ImGui::EndTable();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
