#include "inspector_menu.h"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../editor.h"
#include "../../../engine/asset_management/project_manager.h"
#include "../../../engine/audio/audio_manager.h"
#include "../../../engine/audio/audio_clip_stream.h"
#include <imgui/imgui_impl_opengl3.h>
#include "../../../engine/graphics/renderer/renderer.h"

void InspectorMenu::Init()
{
}

void InspectorMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoCollapse);

	auto selectedGameObject = Engine::selectedGameObject.lock();
	std::shared_ptr<FileReference> selectedFileReference = Engine::GetSelectedFileReference();

	if (selectedFileReference)
	{
		std::string fileNameExt = selectedFileReference->file->GetFileName() + selectedFileReference->file->GetFileExtension();
		ImGui::Text(fileNameExt.c_str());
		ImGui::Separator();

		auto reflection = std::dynamic_pointer_cast<Reflection>(selectedFileReference);

		auto metaReflection = selectedFileReference->GetMetaReflection();
		if (reflection)
		{
			auto reflectionList = reflection->GetReflection();
			if (reflectionList.size() != 0)
			{
				bool changed = EditorUI::DrawMap(reflectionList);
				if (changed)
				{
					reflection->OnReflectionUpdated();
				}
			}
		}
		if (metaReflection.size() != 0)
		{
			EditorUI::DrawMap(metaReflection);

			if (ImGui::Button("Apply"))
			{
				ProjectManager::SaveMetaFile(selectedFileReference);
			}
		}
	}
	else if (selectedGameObject)
	{
		std::string str0 = selectedGameObject->name;

		//Active checkbox
		bool active = selectedGameObject->GetActive();
		ImGui::Checkbox("##Active", &active);

		//Name input
		ImGui::SameLine();
		ImGui::InputText("##Name ", &str0);

		//Apply new values if changed
		if (str0 != selectedGameObject->name && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
		{
			selectedGameObject->name = str0;
		}
		if (active != selectedGameObject->GetActive())
		{
			selectedGameObject->SetActive(active);
		}

		ImGui::Spacing();
		ImGui::Separator();

		//Local position input
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			std::shared_ptr<Transform> selectedTransform = selectedGameObject->GetTransform();
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				std::string payloadName = "Transform";
				ImGui::SetDragDropPayload(payloadName.c_str(), selectedTransform.get(), sizeof(Transform));
				ImGui::Text("Transform");
				ImGui::EndDragDropSource();
			}
			Vector3 localPos = selectedTransform->GetLocalPosition();
			bool changed = EditorUI::DrawInput("Local Position", localPos);

			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedTransform->SetLocalPosition(localPos);
			}
			//ImGui::Text("World Position: %f %f %f", selectedTransform->GetPosition().x, selectedTransform->GetPosition().y, selectedTransform->GetPosition().z);

			//Local rotation input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localRot = selectedTransform->GetLocalRotation();
			changed = EditorUI::DrawInput("Local Rotation", localRot);
			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedTransform->SetLocalRotation(localRot);
			}
			//ImGui::Text("World Rotation: %f %f %f", selectedTransform->GetRotation().x, selectedTransform->GetRotation().y, selectedTransform->GetRotation().z);

			//Local scale input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localScale = selectedTransform->GetLocalScale();
			changed = EditorUI::DrawInput("Local Scale", localScale);
			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedTransform->SetLocalScale(localScale);
			}
			//ImGui::Text("World Scale: %f %f %f", selectedTransform->GetScale().x, selectedTransform->GetScale().y, selectedTransform->GetScale().z);
			ImGui::Separator();
		}

		//Component list
		int componentCount = selectedGameObject->GetComponentCount();
		for (int i = 0; i < componentCount; i++)
		{
			auto& comp = selectedGameObject->components[i];
			//Draw component title

			float cursorY = ImGui::GetCursorPosY();

			bool isEnable = comp->GetIsEnabled();
			ImGui::SetCursorPosX(35);
			ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &isEnable);
			if (comp->GetIsEnabled() != isEnable)
			{
				comp->SetIsEnabled(isEnable);
			}
			ImGui::SetCursorPosY(cursorY);

			if (ImGui::CollapsingHeader(EditorUI::GenerateItemId().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
			{
				CheckOpenRightClickPopupFile(comp, componentCount, i, "RightClick" + std::to_string(comp->GetUniqueId()));
				if (!comp->waitingForDestroy)
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						std::string payloadName = "Component";
						ImGui::SetDragDropPayload(payloadName.c_str(), comp.get(), sizeof(Component));
						ImGui::Text("%s", comp->GetComponentName().c_str());
						ImGui::EndDragDropSource();
					}

					//Draw component variables
					if (EditorUI::DrawReflection(*comp))
					{
						comp->OnReflectionUpdated();
					}

					ImGui::Separator();
				}
			}
			float lastCursorY = ImGui::GetCursorPosY();
			ImGui::SetCursorPosX(65);
			ImGui::SetCursorPosY(cursorY + 4);
			ImGui::Text(comp->GetComponentName().c_str());
			ImGui::SetCursorPosY(lastCursorY);
		}
	}

	DrawFilePreview();

	ImGui::End();
}

int InspectorMenu::CheckOpenRightClickPopupFile(std::shared_ptr<Component>& component, int& componentCount, int& componentIndex, const std::string& id)
{
	int state = 0;
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
	{
		ImGui::OpenPopup(id.c_str());
		state = 1;
	}

	if (ImGui::BeginPopup(id.c_str()))
	{
		if (state == 0)
			state = 2;
		if (ImGui::MenuItem("Delete"))
		{
			Destroy(component);
			componentCount--;
			componentIndex--;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return state;
}

void InspectorMenu::DrawFilePreview()
{
	if (Engine::GetSelectedFileReference())
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();

		//Get preview area available size
		ImVec2 availSize = ImGui::GetContentRegionAvail();
		float sizeY = availSize.x;
		if (availSize.x > availSize.y)
			sizeY = availSize.y;

		int textureId = 0;
		// If the selected file needs to be loaded for preview
		if (loadedPreview != Engine::GetSelectedFileReference())
		{
			loadedPreview = Engine::GetSelectedFileReference();
			previewText.clear();
			// Read text file
			if (loadedPreview->fileType == File_Code)
			{
				std::shared_ptr<File> file = loadedPreview->file;
				file->Open(false);
				previewText = file->ReadAll();
				file->Close();
			}
		}
		// If the file is a texture, get the texture id
		if (loadedPreview->fileType == File_Texture)
		{
			textureId = std::dynamic_pointer_cast<Texture>(loadedPreview)->GetTextureId();
		}

		// If the preview is a text, calculate the texte size
		if (!previewText.empty())
		{
			sizeY = ImGui::CalcTextSize(previewText.c_str(), 0, false, availSize.x).y + 10; // + 10 to avoid the hided last line in some text
		}

		ImGui::Text("Preview:");
		ImGui::BeginChild("Preview", ImVec2(0, sizeY), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (!previewText.empty()) // Draw text preview
		{
			ImGui::TextWrapped(previewText.c_str());
		}
		else if (textureId != 0) // Draw image preview
		{
			std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(loadedPreview);
			ImVec2 availArea = ImGui::GetContentRegionAvail();
			Engine::renderer->BindTexture(texture);
			ImGui::Image((ImTextureID)textureId, availArea);

			// Print texture resolution
			std::string text = std::to_string(texture->GetWidth()) + "x" + std::to_string(texture->GetHeight());
			ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
			ImVec2 textPos;
			textPos.x = availArea.x / 2.0f - textSize.x / 2.0f + ImGui::GetCursorPosX();
			textPos.y = availArea.y - textSize.y / 2.0f;
			ImGui::SetCursorPos(textPos);
			ImGui::Text(text.c_str());
		}
		else if (loadedPreview->fileType == File_Audio) // Draw audio preview
		{
			int playedSoundCount = AudioManager::channel->playedSounds.size();
			AudioClipStream* stream = nullptr;
			for (int i = 0; i < playedSoundCount; i++)
			{
				if (AudioManager::channel->playedSounds[i]->audioSource == Editor::audioSource.lock())
				{
					// Get audio stream
					stream = AudioManager::channel->playedSounds[i]->audioClipStream;
					break;
				}
			}

			// Draw Play/Stop button
			if (stream)
			{
				if (Editor::audioSource.lock()->GetIsPlaying())
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
						audioSource->audioClip = std::dynamic_pointer_cast<AudioClip>(loadedPreview);
						audioSource->Play();
					}
				}
			}

			if (stream)
			{
				// Get audio stream info
				float seekPos = stream->GetSeekPosition() / (double)stream->GetSampleCount();
				float totalTime = stream->GetSampleCount() / (double)stream->GetFrequency();

				// Draw current time
				availSize = ImGui::GetContentRegionAvail();
				ImVec2 cursorPos = ImGui::GetCursorPos();
				std::string currentTimeText = std::to_string(((int)(totalTime * seekPos * 1000)) / 1000.0f);
				ImVec2 infoTextSize2 = ImGui::CalcTextSize(currentTimeText.substr(0, currentTimeText.find_last_of('.') + 4).c_str());
				ImGui::SetCursorPosX(availSize.x / 2 - infoTextSize2.x / 2 + cursorPos.x);
				ImGui::Text("%ss", currentTimeText.substr(0, currentTimeText.find_last_of('.') + 4).c_str());

				cursorPos = ImGui::GetCursorPos();
				ImVec2 mousePos = ImGui::GetMousePos();
				ImVec2 windowPos = ImGui::GetWindowPos();

				// Move cursor when user is clicking on the timeline
				float normalisedPos = ((mousePos.x - windowPos.x - cursorPos.x) / (float)(availSize.x));
				int mouseYPos = mousePos.y - windowPos.y;
				bool isMouseXPosOk = normalisedPos >= 0 && normalisedPos <= 1;
				bool isMouseYPosOk = mouseYPos >= cursorPos.y && mouseYPos <= cursorPos.y + 50;

				if (ImGui::IsMouseClicked(0) && isMouseXPosOk && isMouseYPosOk)
				{
					if (normalisedPos < 0)
						normalisedPos = 0;
					else if (normalisedPos > 1)
						normalisedPos = 1;
					stream->SetSeek(stream->GetSampleCount() * normalisedPos);
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

				std::string totalTimeText = std::to_string(((int)(totalTime * 1000)) / 1000.0f);
				std::string infoText = audioTypeText + ", " + std::to_string(stream->GetFrequency()) + " Hz, " + channelText + ", " + totalTimeText.substr(0, totalTimeText.find_last_of('.') + 4) + "s";
				ImVec2 infoTextSize = ImGui::CalcTextSize(infoText.c_str());
				ImGui::SetCursorPosX(availSize.x / 2 - infoTextSize.x / 2 + cursorPos.x);
				ImGui::Text("%s", infoText.c_str());
			}
		}
		else
			ImGui::Text("No preview available");

		ImGui::EndChild();
	}
}
