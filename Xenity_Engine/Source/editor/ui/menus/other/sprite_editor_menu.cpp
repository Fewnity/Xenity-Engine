// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sprite_editor_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/asset_modifier/asset_modifier.h>

#include <engine/graphics/texture/texture.h>
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
	m_spriteSelections.push_back(selection);
	m_currentSelectedSpriteIndex = (int)m_spriteSelections.size() - 1;
}

void SpriteEditorMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Sprite Editor", &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		if (m_spriteToEdit)
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
	m_spriteSelections.clear();

	// Create a sprite selection and copy data from the spriteToEdit texture
	const size_t spriteToEditSelectionCount = m_spriteToEdit->spriteSelections.size();
	for (size_t i = 0; i < spriteToEditSelectionCount; i++)
	{
		SpriteSelection* selectionToCopy = m_spriteToEdit->spriteSelections[i];

		SpriteSelection newSelection;
		newSelection.position = selectionToCopy->position;
		newSelection.size = selectionToCopy->size;
		newSelection.pivot = selectionToCopy->pivot;
		m_spriteSelections.push_back(newSelection);
	}
}

void SpriteEditorMenu::SaveSpriteSelections()
{
	const std::string folderPath0 = ProjectManager::GetProjectFolderPath() + "additional_assets\\sprite_sheets\\";

	// Create the folder where sprites will be saved
	FileSystem::CreateFolder(folderPath0);

	const std::string folderPath = ProjectManager::GetProjectFolderPath() + "additional_assets\\sprite_sheets\\" + std::to_string(m_spriteToEdit->m_fileId) + "\\";
	// Create the folder where sprites will be saved
	FileSystem::CreateFolder(folderPath);

	m_spriteToEdit->ClearSpriteSelections();
	const size_t spriteSelectionCount = m_spriteSelections.size();
	for (size_t selectI = 0; selectI < spriteSelectionCount; selectI++)
	{
		// Add sprite selections to the sprite sheet texture
		SpriteSelection* newSpriteSelection = new SpriteSelection();
		newSpriteSelection->position = m_spriteSelections[selectI].position;
		newSpriteSelection->size = m_spriteSelections[selectI].size;
		newSpriteSelection->pivot = m_spriteSelections[selectI].pivot;
		m_spriteToEdit->spriteSelections.push_back(newSpriteSelection);

		// Create cropped texture file
		std::shared_ptr<File> newFile = FileSystem::MakeFile(folderPath + m_spriteToEdit->m_file->GetFileName() + "_" + std::to_string(selectI) + ".png");
		AssetModifier::CropTexture(m_spriteToEdit, static_cast<int>(newSpriteSelection->position.x), static_cast<int>(newSpriteSelection->position.y), static_cast<int>(newSpriteSelection->size.x), static_cast<int>(newSpriteSelection->size.y), newFile);
	}

	// Save sprite sheet texture meta file
	m_spriteToEdit->m_isMetaDirty = true;
	ProjectManager::SaveMetaFile(*m_spriteToEdit);
}

void SpriteEditorMenu::DrawSpriteSheetOverlay()
{
	ImGui::BeginChild("SpriteEditorChild", ImVec2(m_startAvailableSize.x, 0), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
	if (m_spriteToEdit)
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
	std::reference_wrapper<std::shared_ptr<Texture>> textureRef = std::ref(m_spriteToEdit);
	EditorUI::DrawFileReference(nullptr, textureRef, "Texture", newValue);
	if (m_spriteToEdit != m_oldSpriteToEdit)
	{
		m_oldSpriteToEdit = m_spriteToEdit;
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
	const float topX = winPos.x - (m_spriteToEdit->GetWidth() * m_zoom) / 2.0f + availSize.x / 2.0f + oldCursorXPos + m_spriteToEdit->GetWidth() * m_offset.x * m_zoom;
	const float topY = winPos.y - (m_spriteToEdit->GetHeight() * m_zoom) / 2.0f + availSize.y / 2.0f + oldCursorYPos + m_spriteToEdit->GetHeight() * m_offset.y * m_zoom;
	const float bottomX = winPos.x + (m_spriteToEdit->GetWidth() * m_zoom) / 2.0f + availSize.x / 2.0f + oldCursorXPos + m_spriteToEdit->GetWidth() * m_offset.x * m_zoom;
	const float bottomY = winPos.y + (m_spriteToEdit->GetHeight() * m_zoom) / 2.0f + availSize.y / 2.0f + oldCursorYPos + m_spriteToEdit->GetHeight() * m_offset.y * m_zoom;

	// Draw sprite sheet
	ImGui::GetWindowDrawList()->AddImage((ImTextureID)(size_t)EditorUI::GetTextureId(*m_spriteToEdit), ImVec2(topX, topY), ImVec2(bottomX, bottomY));

	// Draw all sprite selection lines
	const size_t spriteSelectionCount = m_spriteSelections.size();
	for (size_t selectionIndex = 0; selectionIndex < spriteSelectionCount; selectionIndex++)
	{
		const SpriteSelection& currentSelection = m_spriteSelections[selectionIndex];

		// Get rect corners positions
		const float lineRectTopX = topX + (currentSelection.position.x + currentSelection.size.x) * m_zoom;
		const float lineRectTopY = topY + (m_spriteToEdit->GetHeight() - currentSelection.position.y - currentSelection.size.y) * m_zoom;
		const float lineRectBottomX = topX + (currentSelection.position.x) * m_zoom;
		const float lineRectBottomY = topY + (m_spriteToEdit->GetHeight() - currentSelection.position.y) * m_zoom;

		// Get lines color
		ImU32 color = IM_COL32(0, 255, 0, 255);
		// Use transparent color if not selected
		if (m_currentSelectedSpriteIndex != selectionIndex)
		{
			color = IM_COL32(0, 255, 0, 70);
		}

		// Draw rect
		ImGui::GetWindowDrawList()->AddRect(ImVec2(lineRectTopX, lineRectTopY),
			ImVec2(lineRectBottomX, lineRectBottomY),
			color);

		// Draw center circle
		if (m_currentSelectedSpriteIndex == selectionIndex)
		{
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(lineRectTopX + (lineRectBottomX - lineRectTopX) * currentSelection.pivot.x, lineRectTopY + (lineRectBottomY - lineRectTopY) * currentSelection.pivot.y), 6, IM_COL32(0, 255, 0, 255));
		}
	}

	ImGui::SetCursorPosY(oldCursorYPos);
}

void SpriteEditorMenu::DrawToolWindow()
{
	ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Sprite Editor Tool", &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (m_spriteToEdit)
	{
		// Draw button to add a new selection
		if (ImGui::Button("Add new sprite selection"))
		{
			AddNewSpriteSelection(Vector2(0, 0), Vector2(static_cast<float>(m_spriteToEdit->GetWidth()), static_cast<float>(m_spriteToEdit->GetHeight())), Vector2(0.5f, 0.5f));
		}
		ImGui::Separator();

		// Draw all sprite selections
		size_t spriteSelectionCount = m_spriteSelections.size();
		for (size_t selectionIndex = 0; selectionIndex < spriteSelectionCount; selectionIndex++)
		{
			SpriteSelection& currentSelection = m_spriteSelections[selectionIndex];

			// Generate sprite UV from position and size
			const float uvTopX = (currentSelection.position.x + currentSelection.size.x) / m_spriteToEdit->GetWidth();
			const float uvTopY = (m_spriteToEdit->GetHeight() - currentSelection.position.y - currentSelection.size.y) / m_spriteToEdit->GetHeight();
			const float uvBottomX = (currentSelection.position.x) / m_spriteToEdit->GetWidth();
			const float uvBottomY = (m_spriteToEdit->GetHeight() - currentSelection.position.y) / m_spriteToEdit->GetHeight();

			// Draw sprite preview
			ImGui::Image((ImTextureID)(size_t)EditorUI::GetTextureId(*m_spriteToEdit), ImVec2(150, 150), ImVec2(uvBottomX, uvTopY), ImVec2(uvTopX, uvBottomY));

			// Draw button to select the sprite selection
			const std::string selectButtonText = "Select" + EditorUI::GenerateItemId();
			if (ImGui::Button(selectButtonText.c_str()))
			{
				m_currentSelectedSpriteIndex = static_cast<int>(selectionIndex);
			}
			ImGui::SameLine();

			// Draw the remove button to delete the sprite selection
			const std::string removeButtonText = "Remove" + EditorUI::GenerateItemId();
			if (ImGui::Button(removeButtonText.c_str()))
			{
				m_spriteSelections.erase(m_spriteSelections.begin() + selectionIndex);
				if (m_currentSelectedSpriteIndex >= selectionIndex)
				{
					m_currentSelectedSpriteIndex--;
				}
				selectionIndex--;
				spriteSelectionCount--;
			}

			// Draw sprite selection variables
			if (m_currentSelectedSpriteIndex == selectionIndex)
			{
				// Position variable
				Vector2 selectionPosition = currentSelection.position;
				const bool positionUpdated = EditorUI::DrawInput("Position", selectionPosition) != ValueInputState::NO_CHANGE;
				if (positionUpdated)
					currentSelection.position = selectionPosition;

				// Size variable
				Vector2 selectionSize = currentSelection.size;
				const bool sizeUpdated = EditorUI::DrawInput("Size", selectionSize) != ValueInputState::NO_CHANGE;
				if (sizeUpdated)
					currentSelection.size = selectionSize;

				// Pivot variable
				Vector2 selectionPivot = currentSelection.pivot;
				const bool pivotUpdated = EditorUI::DrawInput("Pivot", selectionPivot) != ValueInputState::NO_CHANGE;
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
		m_offset += Vector2(InputSystem::mouseSpeed.x * 2.0f, -InputSystem::mouseSpeed.y * 2.0f);

		// Keep the sprite sheet in the window
		if (m_offset.x < m_minOffset.x)
			m_offset.x = m_minOffset.x;
		else if (m_offset.x > m_maxOffset.x)
			m_offset.x = m_maxOffset.x;

		if (m_offset.y < m_minOffset.y)
			m_offset.y = m_minOffset.y;
		else if (m_offset.y > m_maxOffset.y)
			m_offset.y = m_maxOffset.y;
	}

	if (InputSystem::GetKey(KeyCode::LEFT_CONTROL))
	{
		m_zoom += InputSystem::mouseWheel / 5.0f;
		if (m_zoom < m_minZoom)
			m_zoom = m_minZoom;
		else if (m_zoom > m_maxZoom)
			m_zoom = m_maxZoom;
	}
}
