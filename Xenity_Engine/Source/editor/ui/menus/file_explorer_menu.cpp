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

void FileExplorerMenu::DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, int offset, FileExplorerItem item, std::string itemName)
{
	if (currentCol == 0)
		ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(currentCol);

	currentCol++;
	currentCol %= colCount;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.2f, 0.3f, 0.5f));

	ImGui::BeginGroup();
	int cursorPos = ImGui::GetCursorPosX();
	int availWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);

	unsigned int textureId = EditorUI::fileIcon->GetTextureId();
	if (!isFile)
	{
		textureId = EditorUI::folderIcon->GetTextureId();
	}
	else
	{
		int fileType = item.file->fileType;
		if (fileType == File_Texture)
		{
			Texture* tex = (Texture*)item.file;
			//textureId = tex->GetTextureId();
			textureId = EditorUI::imageIcon->GetTextureId();
		}
		else if (fileType == File_Scene)
		{
			textureId = EditorUI::sceneIcon->GetTextureId();
		}
	}
	bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
	ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)textureId, ImVec2(iconSize, iconSize));
	if (ImGui::IsItemHovered() && ((ImGui::IsMouseClicked(0) && doubleClicked) || ImGui::IsMouseReleased(0)))
	{
		if (doubleClicked)
		{
			if (isFile)
			{
				if (item.file->fileType == File_Scene)
				{
					SceneManager::LoadScene((Scene*)item.file);
				}
			}
			else
			{
				Engine::currentProjectDirectory = item.directory;
				Engine::selectedFileReference = nullptr;
			}
		}
		else
		{
			if (isFile)
			{
				Engine::selectedFileReference = item.file;
			}
			
			Engine::selectedGameObject.reset();
		}
	}

	float windowWidth = ImGui::GetContentRegionAvail().x;
	float textWidth = ImGui::CalcTextSize(itemName.c_str()).x;
	if (textWidth <= availWidth)
	{
		ImGui::SetCursorPosX(cursorPos + (windowWidth - textWidth) * 0.5f);
		ImGui::Text(itemName.c_str());
	}
	else
	{
		ImGui::TextWrapped(itemName.c_str());
	}

	ImGui::EndGroup();
	if (isFile)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string payloadName = "Files" + std::to_string(item.file->fileType);
			ImGui::SetDragDropPayload(payloadName.c_str(), item.file, sizeof(FileReference));
			ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)textureId, ImVec2(iconSize, iconSize));
			ImGui::TextWrapped(itemName.c_str());
			ImGui::EndDragDropSource();
		}
	}
	ImGui::PopStyleColor(3);
}

void FileExplorerMenu::Draw()
{
	float iconSize = 64 * EditorUI::GetUiScale();
	ImGui::Begin("File Explorer", 0, ImGuiWindowFlags_NoCollapse);

	float height = ImGui::GetContentRegionAvail().y;

	int offset = ImGui::GetCursorPosX();
	if (ImGui::BeginTable("explorer_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow(0, height);
		ImGui::TableSetColumnIndex(0);
		EditorUI::DrawTreeItem(ProjectManager::projectDirectory);

		ImGui::TableSetColumnIndex(1);
		float width = ImGui::GetContentRegionAvail().x;
		int colCount = width / (100 * EditorUI::GetUiScale());
		if (colCount <= 0)
			colCount = 1;

		int currentCol = 0;
		if (ImGui::BeginTable("filetable", colCount, ImGuiTableFlags_None))
		{
			ProjectDirectory* currentDir = Engine::currentProjectDirectory;
			int folderCount = currentDir->subdirectories.size();
			int fileCount = currentDir->files.size();

			for (int i = 0; i < folderCount; i++)
			{
				FileExplorerItem item;
				item.directory = currentDir->subdirectories[i];
				DrawExplorerItem(iconSize, currentCol, colCount, false, offset, item, item.directory->GetFolderName());
			}

			for (int i = 0; i < fileCount; i++)
			{
				FileExplorerItem item;
				item.file = currentDir->files[i];
				DrawExplorerItem(iconSize, currentCol, colCount, true, offset, item, item.file->file->GetFileName());
			}
			ImGui::EndTable();
		}
		ImGui::EndTable();
	}

	ImGui::End();
}
