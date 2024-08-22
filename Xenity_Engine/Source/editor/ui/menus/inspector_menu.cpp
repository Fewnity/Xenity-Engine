// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "inspector_menu.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/command/commands/delete.h>
#include <editor/command/commands/create.h>
#include <editor/command/command_manager.h>
#include <editor/ui/editor_ui.h>
#include <editor/ui/utils/menu_builder.h>

#include <engine/file_system/file_reference.h>
#include <engine/file_system/file.h>
#include <engine/asset_management/project_manager.h>
#include <engine/audio/audio_manager.h>
#include <engine/audio/audio_clip.h>
#include <engine/audio/audio_clip_stream.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/inputs/input_system.h>
#include <engine/engine.h>
#include <engine/debug/debug.h>
#include <engine/application.h>

using json = nlohmann::json;

void InspectorMenu::Init()
{
	platformView = Application::PlatformToAssetPlatform(Application::GetPlatform());
}

void InspectorMenu::Draw()
{
	const std::string windowName = "Inspector###Inspector" + std::to_string(id);
	const bool visible = ImGui::Begin(windowName.c_str(), &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		if (Editor::GetSelectedGameObjects().size() == 1)
		{
			std::shared_ptr <GameObject> selectedGameObject = Editor::GetSelectedGameObjects()[0].lock();

			if (selectedGameObject)
			{
				DrawGameObjectInfo(*selectedGameObject);
			}
		}
		else
		{
			std::shared_ptr<FileReference> selectedFileReference = Editor::GetSelectedFileReference();
			if (selectedFileReference)
			{
				DrawFileInfo(*selectedFileReference);
			}
		}


		DrawFilePreview();

		CalculateWindowValues();

		if(isFocused)
			areWindowsFocused = true;
		
		if (!areWindowsFocused)
		{
			StopAudio();
		}

		areWindowsFocused = false;
	}
	else
	{
		ResetWindowValues();
		StopAudio();
	}
	if (!isActive) 
	{
		StopAudio();
	}
	ImGui::End();


	forceItemUpdate = false;
}

int InspectorMenu::CheckOpenRightClickPopupTransform(Transform& transform, const std::string& id)
{
	std::function<void()> copyFunc = [&transform]()
		{
			json copyData;
			copyData["Values"] = ReflectionUtils::ReflectiveDataToJson(transform.GetReflectiveData());
			EditorUI::copiedComponentJson = copyData;
			EditorUI::currentCopyType = CopyType::Transform;
		};

	std::function<void()> pastFunc = [&transform]()
		{
			std::shared_ptr<InspectorSetTransformDataCommand> command = std::make_shared<InspectorSetTransformDataCommand>(transform, EditorUI::copiedComponentJson);
			CommandManager::AddCommandAndExecute(command);
		};

	RightClickMenu inspectorRightClickMenu = RightClickMenu(id);
	RightClickMenuState rightClickState = inspectorRightClickMenu.Check(false);
	if (rightClickState != RightClickMenuState::Closed)
	{
		inspectorRightClickMenu.AddItem("Copy transform values", copyFunc);
		RightClickMenuItem* pastItem = inspectorRightClickMenu.AddItem("Past transform values", pastFunc);
		pastItem->SetIsEnabled(EditorUI::currentCopyType == CopyType::Transform);
	}
	const bool rightClickMenuDrawn = inspectorRightClickMenu.Draw();

	int state = 0;

	if (rightClickState == RightClickMenuState::JustOpened)
		state = 1;
	else if (rightClickMenuDrawn)
		state = 2;

	return state;
}

int InspectorMenu::CheckOpenRightClickPopup(Component& component, int& componentCount, int& componentIndex, const std::string& id)
{
	std::function<void()> deleteFunc = [&component, &componentCount, &componentIndex]()
		{
			auto command = std::make_shared<InspectorDeleteComponentCommand<Component>>(component);
			CommandManager::AddCommandAndExecute(command);
			componentCount--;
			componentIndex--;
		};

	std::function<void()> copyFunc = [&component]()
		{
			json copyData;
			copyData["Values"] = ReflectionUtils::ReflectiveDataToJson(component.GetReflectiveData());
			EditorUI::copiedComponentJson = copyData;
			EditorUI::copiedComponentName = component.GetComponentName();
			EditorUI::currentCopyType = CopyType::Component;
		};

	std::function<void()> pastFunc = [&component]()
		{
			std::shared_ptr<InspectorSetComponentDataCommand<Component>> command = std::make_shared<InspectorSetComponentDataCommand<Component>>(component, EditorUI::copiedComponentJson);
			CommandManager::AddCommandAndExecute(command);
		};

	RightClickMenu inspectorRightClickMenu = RightClickMenu(id);
	RightClickMenuState rightClickState = inspectorRightClickMenu.Check(false);
	if (rightClickState != RightClickMenuState::Closed)
	{
		inspectorRightClickMenu.AddItem("Copy component values", copyFunc);
		RightClickMenuItem* pastItem = inspectorRightClickMenu.AddItem("Past component values", pastFunc);
		pastItem->SetIsEnabled(EditorUI::currentCopyType == CopyType::Component && EditorUI::copiedComponentName == component.GetComponentName());
		inspectorRightClickMenu.AddItem("Delete", deleteFunc);
	}
	const bool rightClickMenuDrawn = inspectorRightClickMenu.Draw();

	int state = 0;

	if (rightClickState == RightClickMenuState::JustOpened)
		state = 1;
	else if (rightClickMenuDrawn)
		state = 2;

	return state;
}

void InspectorMenu::DrawFilePreview()
{
	if (Editor::GetSelectedFileReference())
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();

		//Get preview area available size
		ImVec2 availSize = ImGui::GetContentRegionAvail();
		float sizeY = availSize.x;
		if (availSize.x > availSize.y)
			sizeY = availSize.y;

		unsigned int textureId = 0;
		// If the selected file needs to be loaded for preview
		if (loadedPreview != Editor::GetSelectedFileReference())
		{
			loadedPreview = Editor::GetSelectedFileReference();
			previewText.clear();
			// Read text file
			if (loadedPreview->fileType == FileType::File_Code || loadedPreview->fileType == FileType::File_Header || loadedPreview->fileType == FileType::File_Shader)
			{
				std::shared_ptr<File> file = loadedPreview->file;
				if (file->Open(FileMode::ReadOnly))
				{
					previewText = file->ReadAll();
					file->Close();
				}
				else
				{
					Debug::PrintError("[InspectorMenu::DrawFilePreview] Fail to open the preview file", true);
				}
			}
		}
		// If the file is a texture, get the texture id
		if (loadedPreview->fileType == FileType::File_Texture)
		{
			textureId = std::dynamic_pointer_cast<Texture>(loadedPreview)->GetTextureId();
		}

		// If the preview is a text, calculate the text size
		if (!previewText.empty())
		{
			sizeY = ImGui::CalcTextSize(previewText.c_str(), 0, false, availSize.x).y + 10; // + 10 to avoid the hided last line in some text
		}

		ImGui::Text("Preview:");
		ImGui::BeginChild("Preview", ImVec2(0, sizeY), ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (!previewText.empty()) // Draw text preview
		{
			ImGui::TextWrapped("%s", previewText.c_str());
		}
		else if (textureId != 0) // Draw image preview
		{
			const std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(loadedPreview);
			const ImVec2 availArea = ImGui::GetContentRegionAvail();
			Engine::GetRenderer().BindTexture(*texture);
			ImGui::Image((ImTextureID)(size_t)textureId, availArea);

			const std::string text = std::to_string(texture->GetWidth()) + "x" + std::to_string(texture->GetHeight());
			const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
			ImVec2 textPos;
			textPos.x = availArea.x / 2.0f - textSize.x / 2.0f + ImGui::GetCursorPosX();
			textPos.y = availArea.y - textSize.y / 2.0f;

			//Draw text background
			const ImVec2 childWindowPos = ImGui::GetWindowPos();
			const ImVec2 rectTopLeftPos = ImVec2(childWindowPos.x + textPos.x - 4, childWindowPos.y + textPos.y - 1);
			const ImVec2 rectBottomRightPos = ImVec2(childWindowPos.x + textPos.x + textSize.x + 4, childWindowPos.y + textPos.y + textSize.y - 1);
			draw_list->AddRectFilled(rectTopLeftPos, rectBottomRightPos, ImColor(ImVec4(0, 0, 0, 0.35f)));

			// Print texture resolution
			ImGui::SetCursorPos(textPos);
			ImGui::Text("%s", text.c_str());
		}
		else if (loadedPreview->fileType == FileType::File_Mesh) // Draw audio preview
		{
			ImGui::Text("SubMesh count: %d", std::dynamic_pointer_cast<MeshData>(loadedPreview)->subMeshCount);
		}
		else if (loadedPreview->fileType == FileType::File_Audio) // Draw audio preview
		{
			const size_t playedSoundCount = AudioManager::channel->playedSounds.size();
			AudioClipStream* stream = nullptr;
			for (size_t i = 0; i < playedSoundCount; i++)
			{
				if (AudioManager::channel->playedSounds[i]->audioSource.lock() == Editor::audioSource.lock())
				{
					// Get audio stream
					stream = AudioManager::channel->playedSounds[i]->audioClipStream;
					break;
				}
			}

			// Draw Play/Stop button
			if (stream)
			{
				if (Editor::audioSource.lock()->IsPlaying())
				{
					if (ImGui::Button("Pause audio"))
					{
						Editor::audioSource.lock()->Pause();
					}
				}
				else
				{
					if (ImGui::Button("Resume audio"))
					{
						Editor::audioSource.lock()->Resume();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop audio"))
				{
					Editor::audioSource.lock()->Stop();
					isPlayingAudio = false;
				}
			}
			else
			{
				if (ImGui::Button("Play audio"))
				{
					std::shared_ptr<AudioSource> audioSource = Editor::audioSource.lock();
					if (audioSource)
					{
						audioSource->Stop();
						audioSource->SetAudioClip(std::dynamic_pointer_cast<AudioClip>(loadedPreview));
						audioSource->Play();
						isPlayingAudio = true;
					}
				}
			}

			if (stream)
			{
				// Get audio stream info
				const float seekPos = (float)(stream->GetSeekPosition() / (double)stream->GetSampleCount());
				const float totalTime = (float)(stream->GetSampleCount() / (double)stream->GetFrequency());

				// Draw current time
				availSize = ImGui::GetContentRegionAvail();
				ImVec2 cursorPos = ImGui::GetCursorPos();
				const std::string currentTimeText = std::to_string(((int)(totalTime * seekPos * 1000)) / 1000.0f);
				const ImVec2 infoTextSize2 = ImGui::CalcTextSize(currentTimeText.substr(0, currentTimeText.find_last_of('.') + 4).c_str());
				ImGui::SetCursorPosX(availSize.x / 2 - infoTextSize2.x / 2 + cursorPos.x);
				ImGui::Text("%ss", currentTimeText.substr(0, currentTimeText.find_last_of('.') + 4).c_str());

				cursorPos = ImGui::GetCursorPos();
				const ImVec2 mousePos = ImGui::GetMousePos();
				const ImVec2 windowPos = ImGui::GetWindowPos();

				// Move cursor when user is clicking on the timeline
				float normalisedPos = ((mousePos.x - windowPos.x - cursorPos.x) / (float)(availSize.x));
				const float mouseYPos = mousePos.y - windowPos.y;
				const bool isMouseXPosOk = normalisedPos >= 0 && normalisedPos <= 1;
				const bool isMouseYPosOk = mouseYPos >= cursorPos.y && mouseYPos <= cursorPos.y + 50;

				if (ImGui::IsMouseClicked(0) && isMouseXPosOk && isMouseYPosOk)
				{
					if (normalisedPos < 0)
						normalisedPos = 0;
					else if (normalisedPos > 1)
						normalisedPos = 1;
					stream->SetSeek((uint64_t)(stream->GetSampleCount() * normalisedPos));
				}

				// Draw audio cursor
				draw_list->AddLine(ImVec2(cursorPos.x + availSize.x * seekPos + windowPos.x, cursorPos.y + windowPos.y), ImVec2(cursorPos.x + availSize.x * seekPos + windowPos.x, cursorPos.y + 50 + windowPos.y), IM_COL32(255, 255, 255, 255));
				ImGui::SetCursorPosY(cursorPos.y + 50);

				// Draw audio info text
				std::string channelText = "Stereo";
				if (stream->GetChannelCount() == 1)
					channelText = "Mono";

				std::string audioTypeText = "Waveform";
				if (stream->GetAudioType() == AudioType::Mp3)
				{
					audioTypeText = "Mp3";
				}

				const std::string totalTimeText = std::to_string(((int)(totalTime * 1000)) / 1000.0f);
				const std::string infoText = audioTypeText + ", " + std::to_string(stream->GetFrequency()) + " Hz, " + channelText + ", " + totalTimeText.substr(0, totalTimeText.find_last_of('.') + 4) + "s";
				const ImVec2 infoTextSize = ImGui::CalcTextSize(infoText.c_str());
				ImGui::SetCursorPosX(availSize.x / 2 - infoTextSize.x / 2 + cursorPos.x);
				ImGui::Text("%s", infoText.c_str());
			}
		}
		else
			ImGui::Text("No preview available");

			if (ImGui::IsWindowFocused())
				areWindowsFocused = true;

		ImGui::EndChild();
	}
}

void InspectorMenu::DrawFileInfo(FileReference& selectedFileReference)
{
	const std::string fileNameExt = selectedFileReference.file->GetFileName() + selectedFileReference.file->GetFileExtension();
	ImGui::Text("%s", fileNameExt.c_str());
	ImGui::Separator();

	const ReflectiveData reflectionList = selectedFileReference.GetReflectiveData();
	if (reflectionList.size() != 0)
	{
		ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw(selectedFileReference, platformView);
		const ValueInputState valueInputState = EditorUI::DrawReflectiveData(reflectiveDataToDraw, reflectionList, nullptr);
		if (valueInputState != ValueInputState::NO_CHANGE && reflectiveDataToDraw.command)
		{
			if (valueInputState == ValueInputState::APPLIED)
				CommandManager::AddCommandAndExecute(reflectiveDataToDraw.command);
			else
				reflectiveDataToDraw.command->Execute();
		}
		if (forceItemUpdate)
		{
			selectedFileReference.OnReflectionUpdated();
		}
	}

	const ReflectiveData metaReflection = selectedFileReference.GetMetaReflectiveData(platformView);
	bool disableMetaView = false;
	if (loadedPreview && loadedPreview->fileType == FileType::File_Audio) // Draw audio preview
	{
		const size_t playedSoundCount = AudioManager::channel->playedSounds.size();
		AudioClipStream* stream = nullptr;
		for (size_t i = 0; i < playedSoundCount; i++)
		{
			if (AudioManager::channel->playedSounds[i]->audioSource.lock() == Editor::audioSource.lock())
			{
				// Get audio stream
				disableMetaView = true;
				break;
			}
		}
	}

	if (metaReflection.size() != 0 && !disableMetaView)
	{
		EditorUI::SetButtonColor(platformView == AssetPlatform::AP_Standalone);
		if (ImGui::Button("Standalone"))
		{
			platformView = AssetPlatform::AP_Standalone;
		}
		EditorUI::EndButtonColor();

		ImGui::SameLine();
		EditorUI::SetButtonColor(platformView == AssetPlatform::AP_PSP);
		if (ImGui::Button("PSP"))
		{
			platformView = AssetPlatform::AP_PSP;
		}
		EditorUI::EndButtonColor();

		ImGui::SameLine();
		EditorUI::SetButtonColor(platformView == AssetPlatform::AP_PsVita);
		if (ImGui::Button("PSVita"))
		{
			platformView = AssetPlatform::AP_PsVita;
		}
		EditorUI::EndButtonColor();

		ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw(selectedFileReference, platformView);
		reflectiveDataToDraw.isMeta = true;
		const ValueInputState valueInputState = EditorUI::DrawReflectiveData(reflectiveDataToDraw, metaReflection, nullptr);
		if (valueInputState != ValueInputState::NO_CHANGE && reflectiveDataToDraw.command)
		{
			if (valueInputState == ValueInputState::APPLIED)
				CommandManager::AddCommandAndExecute(reflectiveDataToDraw.command);
			else
				reflectiveDataToDraw.command->Execute();
		}

		if (ImGui::Button("Apply"))
		{
			selectedFileReference.isMetaDirty = true;
			ProjectManager::SaveMetaFile(selectedFileReference);
		}
	}
}

void InspectorMenu::DrawGameObjectInfo(GameObject& selectedGameObject)
{
	//Active checkbox
	bool active = selectedGameObject.IsActive();
	ImGui::Checkbox("##Active", &active);

	//Name input
	std::string gameObjectName = selectedGameObject.GetName();
	ImGui::SameLine();
	ImGui::InputText("##Name ", &gameObjectName);

	bool isStatic = selectedGameObject.IsStatic();
	ImGui::Checkbox("##IsStatic", &isStatic);
	ImGui::SameLine();
	ImGui::Text("Is Static");

	//Apply new values if changed
	if (gameObjectName != selectedGameObject.GetName() && (InputSystem::GetKeyDown(KeyCode::RETURN) || InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT)))
	{
		// Improve this
		ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw(selectedGameObject, platformView);
		reflectiveDataToDraw.currentEntry = ReflectionUtils::GetReflectiveEntryByName(selectedGameObject.GetReflectiveData(), "name");
		reflectiveDataToDraw.reflectiveDataStack.push_back(selectedGameObject.GetReflectiveData());
		auto command = std::make_shared<ReflectiveChangeValueCommand<std::string>>(reflectiveDataToDraw, &selectedGameObject.GetName(), selectedGameObject.GetName(), gameObjectName);
		CommandManager::AddCommandAndExecute(command);
	}
	if (active != selectedGameObject.IsActive())
	{
		auto command = std::make_shared<InspectorItemSetActiveCommand<GameObject>>(selectedGameObject, active);
		CommandManager::AddCommandAndExecute(command);
	}

	if (isStatic != selectedGameObject.IsStatic())
	{
		auto command = std::make_shared<InspectorItemSetStaticCommand<GameObject>>(selectedGameObject, isStatic);
		CommandManager::AddCommandAndExecute(command);
	}

	ImGui::Spacing();
	ImGui::Separator();
	DrawTransformHeader(selectedGameObject);

	DrawComponentsHeaders(selectedGameObject);

	const float cursorX = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(startAvailableSize.x / 4.0f + cursorX);
	bool justChanged = false;
	if (ImGui::Button("Add Component", ImVec2(startAvailableSize.x / 2.0f, 0)))
	{
		showAddComponentMenu = true;
		justChanged = true;
	}
	if (showAddComponentMenu)
	{
		ImGui::SetCursorPosX(startAvailableSize.x / 4.0f + cursorX);
		ImGui::BeginChild("inspectorComponentList", ImVec2(startAvailableSize.x / 2.0f, 0), ImGuiChildFlags_FrameStyle);
		std::vector<std::string> componentNames = ClassRegistry::GetComponentNames();
		const size_t componentCount = componentNames.size();
		for (size_t i = 0; i < componentCount; i++)
		{
			float lastCursorX = ImGui::GetCursorPosX();
			float lastCursorY = ImGui::GetCursorPosY();

			ImGui::SetCursorPosX(30);
			if (ImGui::Button(componentNames[i].c_str()))
			{
				auto command = std::make_shared<InspectorAddComponentCommand>(*Editor::GetSelectedGameObjects()[0].lock(), componentNames[i]);
				CommandManager::AddCommandAndExecute(command);

				std::shared_ptr<Component> newComponent = FindComponentById(command->componentId);

				if (std::shared_ptr<Collider> boxCollider = std::dynamic_pointer_cast<Collider>(newComponent))
					boxCollider->SetDefaultSize();

				showAddComponentMenu = false;
			}
			std::shared_ptr<Texture> texture = EditorUI::componentsIcons[componentNames[i]];
			if (!texture)
				texture = EditorUI::componentsIcons["Default"];
			if (texture)
			{
				ImGui::SetCursorPosX(lastCursorX);
				ImGui::SetCursorPosY(lastCursorY);
				Engine::GetRenderer().BindTexture(*texture);
				ImGui::Image((ImTextureID)(size_t)texture->GetTextureId(), ImVec2(23, 23));
			}

		}
		ImGui::EndChild();
		if ((ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1)) && !ImGui::IsItemHovered() && !justChanged)
		{
			showAddComponentMenu = false;
		}
	}
}

void InspectorMenu::DrawTransformHeader(const GameObject& selectedGameObject)
{
	//Local position input
	ImGui::Spacing();
	float cursorX = ImGui::GetCursorPosX();
	float cursorY = ImGui::GetCursorPosY();

	std::shared_ptr<Texture> texture = EditorUI::componentsIcons["Transform"];

	if (ImGui::CollapsingHeader("##Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		std::shared_ptr<Transform> selectedTransform = selectedGameObject.GetTransform();
		CheckOpenRightClickPopupTransform(*selectedTransform, "RightClick" + std::to_string(selectedTransform->GetGameObject()->GetUniqueId()));

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const std::string typeId = std::to_string(typeid(std::weak_ptr <Transform>).hash_code());
			const std::string payloadName = "Type" + typeId;
			ImGui::SetDragDropPayload(payloadName.c_str(), selectedTransform.get(), sizeof(Transform));

			if (texture)
			{
				Engine::GetRenderer().BindTexture(*texture);
				ImGui::Image((ImTextureID)(size_t)texture->GetTextureId(), ImVec2(23, 23));
				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
			}

			ImGui::Text("Transform");
			ImGui::EndDragDropSource();
		}
		Vector3 localPos = selectedTransform->GetLocalPosition();
		bool changed = EditorUI::DrawInput("Local Position", localPos) != ValueInputState::NO_CHANGE;

		if (changed && (InputSystem::GetKeyDown(KeyCode::RETURN) || InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT)))
		{
			auto command = std::make_shared<InspectorTransformSetPositionCommand>(selectedTransform->GetGameObject()->GetUniqueId(), localPos, selectedTransform->GetLocalPosition(), true);
			CommandManager::AddCommandAndExecute(command);
		}
		//ImGui::Text("World Position: %f %f %f", selectedTransform->GetPosition().x, selectedTransform->GetPosition().y, selectedTransform->GetPosition().z);

		//Local rotation input
		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localRot = selectedTransform->GetLocalEulerAngles();
		changed = EditorUI::DrawInput("Local Rotation", localRot) != ValueInputState::NO_CHANGE;
		if (changed && (InputSystem::GetKeyDown(KeyCode::RETURN) || InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT)))
		{
			auto command = std::make_shared<InspectorTransformSetRotationCommand>(selectedTransform->GetGameObject()->GetUniqueId(), localRot, selectedTransform->GetLocalEulerAngles(), true);
			CommandManager::AddCommandAndExecute(command);
		}
		//ImGui::Text("World Rotation: %f %f %f", selectedTransform->GetRotation().x, selectedTransform->GetRotation().y, selectedTransform->GetRotation().z);

		//Local scale input
		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localScale = selectedTransform->GetLocalScale();
		changed = EditorUI::DrawInput("Local Scale", localScale) != ValueInputState::NO_CHANGE;
		if (changed && (InputSystem::GetKeyDown(KeyCode::RETURN) || InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT)))
		{
			auto command = std::make_shared<InspectorTransformSetLocalScaleCommand>(selectedTransform->GetGameObject()->GetUniqueId(), localScale, selectedTransform->GetLocalScale());
			CommandManager::AddCommandAndExecute(command);
		}
		//ImGui::Text("World Scale: %f %f %f", selectedTransform->GetScale().x, selectedTransform->GetScale().y, selectedTransform->GetScale().z);
		ImGui::Separator();
	}
	float finalCursorX = ImGui::GetCursorPosX();
	float finalCursorY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosX(63);
	ImGui::SetCursorPosY(cursorY + 3);
	ImGui::Text("Transform");

	if (texture)
	{
		ImGui::SetCursorPosX(35);
		ImGui::SetCursorPosY(cursorY + 1);
		Engine::GetRenderer().BindTexture(*texture);
		ImGui::Image((ImTextureID)(size_t)texture->GetTextureId(), ImVec2(23, 23));
	}

	ImGui::SetCursorPosX(finalCursorX);
	ImGui::SetCursorPosY(finalCursorY);
}

void InspectorMenu::DrawComponentsHeaders(const GameObject& selectedGameObject)
{
	//Component list
	int componentCount = selectedGameObject.GetComponentCount();
	for (int i = 0; i < componentCount; i++)
	{
		std::shared_ptr <Component> comp = selectedGameObject.components[i];

		const float cursorY = ImGui::GetCursorPosY();

		bool isEnable = comp->IsEnabled();

		std::shared_ptr<Texture> texture = EditorUI::componentsIcons[comp->m_componentName];
		if (!texture)
			texture = EditorUI::componentsIcons["Default"];

		const std::string headerName = "##ComponentHeader" + std::to_string(comp->GetUniqueId());
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlap))
		{
			CheckOpenRightClickPopup(*comp, componentCount, i, "RightClick" + std::to_string(comp->GetUniqueId()));
			if (!comp->m_waitingForDestroy)
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					const std::string typeId = std::to_string(typeid(*comp.get()).hash_code());
					const std::string payloadName = "Type" + typeId;
					ImGui::SetDragDropPayload(payloadName.c_str(), comp.get(), sizeof(Component));

					if (texture)
					{
						Engine::GetRenderer().BindTexture(*texture);
						ImGui::Image((ImTextureID)(size_t)texture->GetTextureId(), ImVec2(23, 23));
						ImGui::SameLine();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
					}

					if (!comp->GetComponentName().empty())
						ImGui::Text("%s", comp->GetComponentName().c_str());
					else
						ImGui::Text("Missing component name");
					ImGui::EndDragDropSource();
				}

				//Draw component variables
				ReflectiveDataToDraw reflectiveDataToDraw = EditorUI::CreateReflectiveDataToDraw(*comp, platformView);

				const ValueInputState valueInputState = EditorUI::DrawReflectiveData(reflectiveDataToDraw, comp->GetReflectiveData(), nullptr);
				if (valueInputState != ValueInputState::NO_CHANGE)
				{
					if (reflectiveDataToDraw.command)
					{
						if (valueInputState == ValueInputState::APPLIED)
							CommandManager::AddCommandAndExecute(reflectiveDataToDraw.command);
						else
							reflectiveDataToDraw.command->Execute();
					}
					else
					{
						comp->OnReflectionUpdated();
					}
				}

				if (forceItemUpdate)
				{
					comp->OnReflectionUpdated();
				}
			}
		}

		const float lastCursorX = ImGui::GetCursorPosX();
		const float lastCursorY = ImGui::GetCursorPosY();

		// Draw component enabled checkbox
		ImGui::SetCursorPosX(62);
		ImGui::SetCursorPosY(cursorY);
		const bool isEnabledChanged = ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &isEnable);
		if (isEnabledChanged)
		{
			auto command = std::make_shared<InspectorItemSetActiveCommand<Component>>(*comp, isEnable);
			CommandManager::AddCommandAndExecute(command);
		}

		//Draw component title
		ImGui::SetCursorPosX(92);
		ImGui::SetCursorPosY(cursorY + 3);
		if (!comp->GetComponentName().empty())
			ImGui::Text("%s", comp->GetComponentName().c_str());
		else
			ImGui::Text("Missing component name");

		ImGui::SetCursorPosX(35);
		ImGui::SetCursorPosY(cursorY + 1);

		if (texture)
		{
			Engine::GetRenderer().BindTexture(*texture);
			ImGui::Image((ImTextureID)(size_t)texture->GetTextureId(), ImVec2(23, 23));
		}

		ImGui::SetCursorPosX(lastCursorX);
		ImGui::SetCursorPosY(lastCursorY);
	}
}

void InspectorMenu::StopAudio()
{
	if (isPlayingAudio)
	{
		Editor::audioSource.lock()->Stop();
		isPlayingAudio = false;
	}
}
