#include "file_explorer_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../editor.h"
#include "../editor_ui.h"
#include "../../../engine/scene_management/scene_manager.h"
#include "../../../engine/asset_management/project_manager.h"

void FileExplorerMenu::Init()
{
}

void FileExplorerMenu::Draw()
{
	float iconSize = 64 * EditorUI::GetUiScale();
	ImGui::Begin("File Explorer", 0, ImGuiWindowFlags_NoCollapse);

	float width = ImGui::GetContentRegionAvail().x;
	int colCount = width / (100 * EditorUI::GetUiScale());
	if (colCount <= 0)
		colCount = 1;

	int currentCol = 0;
	if (ImGui::BeginTable("filetable", colCount, ImGuiTableFlags_None))
	{
		int offset = ImGui::GetCursorPosX();
		for (auto kv : ProjectManager::projectFilesRef)
		{
			if (currentCol == 0)
				ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(currentCol);

			currentCol++;
			currentCol %= colCount;

			FileReference* file = kv.second;
			std::string fileName = file->file->GetFileName();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.2f, 0.3f, 0.5f));

			ImGui::BeginGroup();
			int cursorPos = ImGui::GetCursorPosX();
			int availWidth = ImGui::GetContentRegionAvail().x;
			ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);

			unsigned int textureId = EditorUI::fileIcon->GetTextureId();
			int fileType = file->fileType;
			if (fileType == File_Texture)
			{
				Texture* tex = (Texture*)file;
				//textureId = tex->GetTextureId();
				textureId = EditorUI::imageIcon->GetTextureId();
			}
			else if (fileType == File_Scene)
			{
				textureId = EditorUI::sceneIcon->GetTextureId();
			}
			bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
			ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)textureId, ImVec2(iconSize, iconSize));
			if (ImGui::IsItemHovered() && ((ImGui::IsMouseClicked(0) && doubleClicked) || ImGui::IsMouseReleased(0)))
			{
				if (doubleClicked)
				{
					if (file->fileType == File_Scene) 
					{
						SceneManager::LoadScene((Scene*)file);
					}
				}
				else 
				{
					Engine::selectedFileReference = file;
					Engine::selectedGameObject.reset();
				}
			}

			float windowWidth = ImGui::GetContentRegionAvail().x;
			float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
			if (textWidth <= availWidth)
			{
				ImGui::SetCursorPosX(cursorPos + (windowWidth - textWidth) * 0.5f);
				ImGui::Text(fileName.c_str());
			}
			else
			{
				ImGui::TextWrapped(fileName.c_str());
			}

			ImGui::EndGroup();

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				std::string payloadName = "Files" + std::to_string(fileType);
				ImGui::SetDragDropPayload(payloadName.c_str(), file, sizeof(FileReference));
				ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)textureId, ImVec2(iconSize, iconSize));
				ImGui::TextWrapped(fileName.c_str());
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::EndTable();
	}

	ImGui::End();
}
