// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sprite_editor_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/asset_modifier/asset_modifier.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/texture.h>
#include <engine/inputs/input_system.h>
#include <engine/file_system/file_system.h>

void SpriteEditorMenu::Init()
{
}

void SpriteEditorMenu::AddNewSpriteSelection(const Vector2& position, const Vector2& size, const Vector2& pivot)
{
	SpriteSelection selection;
	selection.position = position;
	selection.size = size;
	selection.pivot = pivot;
	spriteSelections.push_back(selection);
	currentSelectedSpriteIndex = (int)spriteSelections.size() - 1;
}

void SpriteEditorMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Sprite Editor", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		if (spriteToEdit)
		{
			DrawSpriteSheet();
		}		
		DrawSpriteSheetOverlay();
		MoveSpriteSheet();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	DrawToolWindow();

	ImGui::End();
}

void SpriteEditorMenu::LoadSpriteSelections()
{
	spriteSelections.clear();

	// Create a sprite selection and copy data from the spriteToEdit texture
	const size_t spriteToEditSelectionCount = spriteToEdit->spriteSelections.size();
	for (size_t i = 0; i < spriteToEditSelectionCount; i++)
	{
		SpriteSelection* selectionToCopy = spriteToEdit->spriteSelections[i];

		SpriteSelection newSelection;
		newSelection.position = selectionToCopy->position;
		newSelection.size = selectionToCopy->size;
		newSelection.pivot = selectionToCopy->pivot;
		spriteSelections.push_back(newSelection);
	}
}

void SpriteEditorMenu::SaveSpriteSelections()
{
	const std::string folderPath0 = ProjectManager::GetProjectFolderPath() + "additional_assets\\sprite_sheets\\";

	// Create the folder where sprites will be saved
	FileSystem::fileSystem->CreateFolder(folderPath0);

	const std::string folderPath = ProjectManager::GetProjectFolderPath() + "additional_assets\\sprite_sheets\\" + std::to_string(spriteToEdit->fileId) + "\\";
	// Create the folder where sprites will be saved
	FileSystem::fileSystem->CreateFolder(folderPath);

	spriteToEdit->ClearSpriteSelections();
	const size_t spriteSelectionCount = spriteSelections.size();
	for (size_t selectI = 0; selectI < spriteSelectionCount; selectI++)
	{
		// Add sprite selections to the sprite sheet texture
		SpriteSelection* newSpriteSelection = new SpriteSelection();
		newSpriteSelection->position = spriteSelections[selectI].position;
		newSpriteSelection->size = spriteSelections[selectI].size;
		newSpriteSelection->pivot = spriteSelections[selectI].pivot;
		spriteToEdit->spriteSelections.push_back(newSpriteSelection);

		// Create cropped texture file
		std::shared_ptr<File> newFile = FileSystem::fileSystem->MakeFile(folderPath + spriteToEdit->file->GetFileName() + "_" + std::to_string(selectI) + ".png");
		AssetModifier::CropTexture(spriteToEdit, newSpriteSelection->position.x, newSpriteSelection->position.y, newSpriteSelection->size.x, newSpriteSelection->size.y, newFile);
	}

	// Save sprite sheet texture meta file
	spriteToEdit->isMetaDirty = true;
	ProjectManager::SaveMetaFile(spriteToEdit);
}

void SpriteEditorMenu::DrawSpriteSheetOverlay()
{
	ImGui::BeginChild("SpriteEditorChild", ImVec2(startAvailableSize.x, 0), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
	if (spriteToEdit)
	{
		// Draw save button
		const std::string buttonText = "Save" + EditorUI::GenerateItemId();
		if (ImGui::Button(buttonText.c_str()))
		{
			SaveSpriteSelections();
		}
	}

	// Draw sprite sheet texture variable
	std::shared_ptr<Texture> newValue;
	std::reference_wrapper<std::shared_ptr<Texture>> textureRef = std::ref(spriteToEdit);
	EditorUI::DrawFileReference(&textureRef, "Texture", newValue);
	if (spriteToEdit != oldSpriteToEdit)
	{
		oldSpriteToEdit = spriteToEdit;
		LoadSpriteSelections();
	}

	ImGui::EndChild();
}

void SpriteEditorMenu::DrawSpriteSheet()
{
	const float oldCursorXPos = ImGui::GetCursorPosX();
	const float oldCursorYPos = ImGui::GetCursorPosY();

	const ImVec2 availSize = ImGui::GetContentRegionAvail();

	// Calculate sprite sheet screen position
	const ImVec2 winPos = ImGui::GetWindowPos();
	const float topX = winPos.x - (spriteToEdit->GetWidth() * zoom) / 2.0f + availSize.x / 2.0f + oldCursorXPos + spriteToEdit->GetWidth() * offset.x * zoom;
	const float topY = winPos.y - (spriteToEdit->GetHeight() * zoom) / 2.0f + availSize.y / 2.0f + oldCursorYPos + spriteToEdit->GetHeight() * offset.y * zoom;
	const float bottomX = winPos.x + (spriteToEdit->GetWidth() * zoom) / 2.0f + availSize.x / 2.0f + oldCursorXPos + spriteToEdit->GetWidth() * offset.x * zoom;
	const float bottomY = winPos.y + (spriteToEdit->GetHeight() * zoom) / 2.0f + availSize.y / 2.0f + oldCursorYPos + spriteToEdit->GetHeight() * offset.y * zoom;

	// Draw sprite sheet
	ImGui::GetWindowDrawList()->AddImage((ImTextureID)spriteToEdit->GetTextureId(), ImVec2(topX, topY), ImVec2(bottomX, bottomY));

	// Draw all sprite selection lines
	const size_t spriteSelectionCount = spriteSelections.size();
	for (size_t selectionIndex = 0; selectionIndex < spriteSelectionCount; selectionIndex++)
	{
		const SpriteSelection& currentSelection = spriteSelections[selectionIndex];

		// Get rect corners positions
		const float lineRectTopX = topX + (currentSelection.position.x + currentSelection.size.x) * zoom;
		const float lineRectTopY = topY + (spriteToEdit->GetHeight() - currentSelection.position.y - currentSelection.size.y) * zoom;
		const float lineRectBottomX = topX + (currentSelection.position.x) * zoom;
		const float lineRectBottomY = topY + (spriteToEdit->GetHeight() - currentSelection.position.y) * zoom;

		// Get lines color
		ImU32 color = IM_COL32(0, 255, 0, 255);
		// Use transparent color if not selected
		if (currentSelectedSpriteIndex != selectionIndex)
		{
			color = IM_COL32(0, 255, 0, 70);
		}

		// Draw rect
		ImGui::GetWindowDrawList()->AddRect(ImVec2(lineRectTopX, lineRectTopY),
			ImVec2(lineRectBottomX, lineRectBottomY),
			color);

		// Draw center circle
		if (currentSelectedSpriteIndex == selectionIndex)
		{
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(lineRectTopX + (lineRectBottomX - lineRectTopX) * currentSelection.pivot.x, lineRectTopY + (lineRectBottomY - lineRectTopY) * currentSelection.pivot.y), 6, IM_COL32(0, 255, 0, 255));
		}
	}

	ImGui::SetCursorPosY(oldCursorYPos);
}

void SpriteEditorMenu::DrawToolWindow()
{
	ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Sprite Editor Tool", &isActive, ImGuiWindowFlags_NoCollapse);
	if (spriteToEdit)
	{
		// Draw button to add a new selection
		if (ImGui::Button("Add new sprite selection"))
		{
			AddNewSpriteSelection(Vector2(0, 0), Vector2(spriteToEdit->GetWidth(), spriteToEdit->GetHeight()), Vector2(0.5f, 0.5f));
		}
		ImGui::Separator();

		// Draw all sprite selections
		size_t spriteSelectionCount = spriteSelections.size();
		for (size_t selectionIndex = 0; selectionIndex < spriteSelectionCount; selectionIndex++)
		{
			SpriteSelection& currentSelection = spriteSelections[selectionIndex];

			// Generate sprite UV from position and size
			const float uvTopX = (currentSelection.position.x + currentSelection.size.x) / spriteToEdit->GetWidth();
			const float uvTopY = (spriteToEdit->GetHeight() - currentSelection.position.y - currentSelection.size.y) / spriteToEdit->GetHeight();
			const float uvBottomX = (currentSelection.position.x) / spriteToEdit->GetWidth();
			const float uvBottomY = (spriteToEdit->GetHeight() - currentSelection.position.y) / spriteToEdit->GetHeight();

			// Draw sprite preview
			ImGui::Image((ImTextureID)spriteToEdit->GetTextureId(), ImVec2(150, 150), ImVec2(uvBottomX, uvTopY), ImVec2(uvTopX, uvBottomY));

			// Draw button to select the sprite selection
			const std::string selectButtonText = "Select" + EditorUI::GenerateItemId();
			if (ImGui::Button(selectButtonText.c_str()))
			{
				currentSelectedSpriteIndex = selectionIndex;
			}
			ImGui::SameLine();

			// Draw the remove button to delete the sprite selection
			const std::string removeButtonText = "Remove" + EditorUI::GenerateItemId();
			if (ImGui::Button(removeButtonText.c_str()))
			{
				spriteSelections.erase(spriteSelections.begin() + selectionIndex);
				if (currentSelectedSpriteIndex >= selectionIndex)
				{
					currentSelectedSpriteIndex--;
				}
				selectionIndex--;
				spriteSelectionCount--;
			}

			// Draw sprite selection variables
			if (currentSelectedSpriteIndex == selectionIndex)
			{
				// Position variable
				Vector2 selectionPosition = currentSelection.position;
				const bool positionUpdated = EditorUI::DrawInput("Position", selectionPosition);
				if (positionUpdated)
					currentSelection.position = selectionPosition;

				// Size variable
				Vector2 selectionSize = currentSelection.size;
				const bool sizeUpdated = EditorUI::DrawInput("Size", selectionSize);
				if (sizeUpdated)
					currentSelection.size = selectionSize;

				// Pivot variable
				Vector2 selectionPivot = currentSelection.pivot;
				const bool pivotUpdated = EditorUI::DrawInput("Pivot", selectionPivot);
				if (pivotUpdated)
					currentSelection.pivot = selectionPivot;
			}

			ImGui::Separator();
		}
	}
	ImGui::End();
}

void SpriteEditorMenu::MoveSpriteSheet()
{
	if (InputSystem::GetKey(KeyCode::MOUSE_RIGHT))
	{
		offset += Vector2(InputSystem::mouseSpeed.x * 2.0f, -InputSystem::mouseSpeed.y * 2.0f);

		// Keep the sprite sheet in the window
		if (offset.x < minOffset.x)
			offset.x = minOffset.x;
		else if (offset.x > maxOffset.x)
			offset.x = maxOffset.x;

		if (offset.y < minOffset.y)
			offset.y = minOffset.y;
		else if (offset.y > maxOffset.y)
			offset.y = maxOffset.y;
	}

	if (InputSystem::GetKey(KeyCode::LEFT_CONTROL))
	{
		zoom += InputSystem::mouseWheel / 5.0f;
		if (zoom < minZoom)
			zoom = minZoom;
		else if (zoom > maxZoom)
			zoom = maxZoom;
	}
}
