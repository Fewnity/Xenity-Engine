#include "sprite_editor_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/texture.h>
#include <engine/inputs/input_system.h>

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
	currentSelectedSpriteIndex = spriteSelections.size() - 1;
}

void SpriteEditorMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Sprite Editor", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		const float oldCursorXPos = ImGui::GetCursorPosX();
		const float oldCursorYPos = ImGui::GetCursorPosY();

		const ImVec2 availSize = ImGui::GetContentRegionAvail();
		if (spriteToEdit)
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const float topX = winPos.x - (spriteToEdit->GetWidth() * zoom) / 2.0f + availSize.x / 2.0f + oldCursorXPos + spriteToEdit->GetWidth() * offset.x * zoom;
			const float topY = winPos.y - (spriteToEdit->GetHeight() * zoom) / 2.0f + availSize.y / 2.0f + oldCursorYPos + spriteToEdit->GetHeight() * offset.y * zoom;
			const float bottomX = winPos.x + (spriteToEdit->GetWidth() * zoom) / 2.0f + availSize.x / 2.0f + oldCursorXPos + spriteToEdit->GetWidth() * offset.x * zoom;
			const float bottomY = winPos.y + (spriteToEdit->GetHeight() * zoom) / 2.0f + availSize.y / 2.0f + oldCursorYPos + spriteToEdit->GetHeight() * offset.y * zoom;

			ImGui::GetWindowDrawList()->AddImage((ImTextureID)spriteToEdit->GetTextureId(), ImVec2(topX, topY), ImVec2(bottomX, bottomY));

			const int spriteSelectionCount = spriteSelections.size();
			for (int selectionIndex = 0; selectionIndex < spriteSelectionCount; selectionIndex++)
			{
				const SpriteSelection& currentSelection = spriteSelections[selectionIndex];

				const float lineRectTopX = topX + (currentSelection.position.x + currentSelection.size.x) * zoom;
				const float lineRectTopY = topY + (spriteToEdit->GetHeight() - currentSelection.position.y - currentSelection.size.y) * zoom;
				const float lineRectBottomX = topX + (currentSelection.position.x) * zoom;
				const float lineRectBottomY = topY + (spriteToEdit->GetHeight() - currentSelection.position.y) * zoom;

				ImU32 color = IM_COL32(0, 255, 0, 255);
				if (currentSelectedSpriteIndex != selectionIndex)
				{
					color = IM_COL32(0, 255, 0, 100);
				}

				ImGui::GetWindowDrawList()->AddRect(ImVec2(lineRectTopX, lineRectTopY),
					ImVec2(lineRectBottomX, lineRectBottomY),
					color);

				if (currentSelectedSpriteIndex == selectionIndex)
				{
					ImGui::GetWindowDrawList()->AddCircle(ImVec2(lineRectTopX + (lineRectBottomX - lineRectTopX) * currentSelection.pivot.x, lineRectTopY + (lineRectBottomY - lineRectTopY) * currentSelection.pivot.y), 6, IM_COL32(0, 255, 0, 255));
				}
			}
		}

		ImGui::SetCursorPosY(oldCursorYPos);

		ImGui::BeginChild("SpriteEditorChild2", ImVec2(startAvailableSize.x, 0), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
		if (spriteToEdit)
		{
			const std::string buttonText = "Save" + EditorUI::GenerateItemId();
			if (ImGui::Button(buttonText.c_str())) 
			{
				spriteToEdit->ClearSpriteSelections();
				const int spriteSelectionCount = spriteSelections.size();
				for (int selectI = 0; selectI < spriteSelectionCount; selectI++)
				{
					SpriteSelection* newSpriteSelection = new SpriteSelection();
					newSpriteSelection->position = spriteSelections[selectI].position;
					newSpriteSelection->size = spriteSelections[selectI].size;
					newSpriteSelection->pivot = spriteSelections[selectI].pivot;
					spriteToEdit->spriteSelections.push_back(newSpriteSelection);
				}
				//spriteToEdit->spriteSelections = std::vector<SpriteSelection>(spriteSelections);
				ProjectManager::SaveMetaFile(spriteToEdit);
			}
		}
		std::shared_ptr<Texture> newValue;
		std::reference_wrapper<std::shared_ptr<Texture>> textureRef = std::ref(spriteToEdit);
		EditorUI::DrawFileReference(&textureRef, "Texture", newValue);
		ImGui::EndChild();

		if (InputSystem::GetKey(KeyCode::MOUSE_RIGHT))
		{
			offset += Vector2(InputSystem::mouseSpeed.x * 2.0f, -InputSystem::mouseSpeed.y * 2.0f);

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

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Sprite Editor Tool", &isActive, ImGuiWindowFlags_NoCollapse);
	if (spriteToEdit)
	{
		//ImVec2 availSize = ImGui::GetContentRegionAvail();
		if (ImGui::Button("Add new sprite selection"))
		{
			AddNewSpriteSelection(Vector2(0, 0), Vector2(spriteToEdit->GetWidth(), spriteToEdit->GetHeight()), Vector2(0.5f, 0.5f));
		}
		ImGui::Spacing();

		int spriteSelectionCount = spriteSelections.size();
		for (int selectionIndex = 0; selectionIndex < spriteSelectionCount; selectionIndex++)
		{
			SpriteSelection& currentSelection = spriteSelections[selectionIndex];

			const float uvTopX = (currentSelection.position.x + currentSelection.size.x) / spriteToEdit->GetWidth();
			const float uvTopY = (spriteToEdit->GetHeight() - currentSelection.position.y - currentSelection.size.y) / spriteToEdit->GetHeight();
			const float uvBottomX = (currentSelection.position.x) / spriteToEdit->GetWidth();
			const float uvBottomY = (spriteToEdit->GetHeight() - currentSelection.position.y) / spriteToEdit->GetHeight();

			ImGui::Image((ImTextureID)spriteToEdit->GetTextureId(), ImVec2(150, 150), ImVec2(uvBottomX, uvTopY), ImVec2(uvTopX, uvBottomY));
			const std::string selectButtonText = "Select" + EditorUI::GenerateItemId();
			if (ImGui::Button(selectButtonText.c_str()))
			{
				currentSelectedSpriteIndex = selectionIndex;
			}
			ImGui::SameLine();
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

			if (currentSelectedSpriteIndex == selectionIndex)
			{
				Vector2 selectionPosition = currentSelection.position;
				const bool positionUpdated = EditorUI::DrawInput("Position", selectionPosition);
				if (positionUpdated)
					currentSelection.position = selectionPosition;

				Vector2 selectionSize = currentSelection.size;
				const bool sizeUpdated = EditorUI::DrawInput("Size", selectionSize);
				if (sizeUpdated)
					currentSelection.size = selectionSize;

				Vector2 selectionPivot = currentSelection.pivot;
				const bool pivotUpdated = EditorUI::DrawInput("Pivot", selectionPivot);
				if (pivotUpdated)
					currentSelection.pivot = selectionPivot;
			}

			ImGui::Spacing();
		}
	}
	ImGui::End();

	ImGui::End();
}
