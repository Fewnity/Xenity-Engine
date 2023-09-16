#if defined(EDITOR)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>
#include <imgui/imgui_internal.h>
#include "../../engine/asset_management/project_manager.h"
#include <Windows.h>
//#include <Commdlg.h>
#include <ShObjIdl.h>
#include "../../engine/graphics/skybox.h"

bool EditorUI::DrawMap(std::unordered_map<std::string, ReflectionEntry> myMap)
{
	bool valueChanged = false;
	for (const auto& kv : myMap)
	{
		bool valueChangedTemp = false;
		std::string variableName = GetPrettyVariableName(kv.first);
		ReflectionEntry reflectionEntry = kv.second;
		if (reflectionEntry.isPublic)
		{
			Variable variableRef = kv.second.variable.value();
			if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))// Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))// Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))// Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
			{
				if (auto val = dynamic_cast<Vector2*>(&valuePtr->get())) // Specific draw
					valueChangedTemp = DrawInput(variableName, *val);
				else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get())) // Specific draw
					valueChangedTemp = DrawInput(variableName, *val);
				else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get())) // Specific draw
					valueChangedTemp = DrawInput(variableName, *val);
				else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get())) // Specific draw
					valueChangedTemp = DrawInput(variableName, *val);
				else if (auto val = dynamic_cast<Color*>(&valuePtr->get())) // Specific draw
					valueChangedTemp = DrawInput(variableName, *val);
				else //Basic draw
					DrawReflection(valuePtr->get());
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<MeshData>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Mesh, "MeshData", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Audio, "AudioClip", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Texture, "Texture", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Scene, "Scene", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Font>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Font, "Font", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
			{
				int vectorSize = valuePtr->get().size();
				std::string tempVariableName = variableName;
				bool firstDraw = true;
				for (int vectorI = 0; vectorI < vectorSize; vectorI++)
				{
					std::string inputText = "None (Texture)";
					auto& ptr = valuePtr->get()[vectorI];
					if (ptr != nullptr)
					{
						if (ptr->file != nullptr)
							inputText = ptr->file->GetFileName();
						else
							inputText = "Filled but invalid file reference (Texture)";

						inputText += " " + std::to_string(ptr->fileId) + " ";
						if (ptr->file)
							inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
					}

					if (DrawInputButton(tempVariableName, inputText, true) == 2)
					{
						valuePtr->get()[vectorI] = nullptr;
					}

					if (firstDraw) 
					{
						tempVariableName = "";
						firstDraw = false;
					}

					std::shared_ptr <FileReference> ref = nullptr;
					std::string payloadName = "Files" + std::to_string(FileType::File_Texture);
					if (DragDropTarget(payloadName, ref))
					{
						valuePtr->get()[vectorI] = std::dynamic_pointer_cast<Texture>(ref);
						valueChangedTemp = true;
					}
				}
				if (ImGui::Button("Add Texture"))
				{
					valuePtr->get().push_back(nullptr);
				}
				if (ImGui::Button("Remove Texture"))
				{
					int textureSize = valuePtr->get().size();
					if (textureSize != 0)
					{
						valuePtr->get().erase(valuePtr->get().begin() + textureSize - 1);
					}
				}
			}
			if (valueChangedTemp)
			{
				valueChanged = true;
			}
		}
	}
	return valueChanged;
}

void EditorUI::DrawTableInput(std::string inputName, std::string inputId, int columnIndex, float& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat(inputId.c_str(), &value, 0, 0, "%.4f");
}

void EditorUI::DrawTableInput(std::string inputName, std::string inputId, int columnIndex, int& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputInt(inputId.c_str(), &value, 0, 0);
}

int EditorUI::DrawInputButton(std::string inputName, std::string text, bool addUnbindButton)
{
	int returnValue = 0;
	DrawInputTitle(inputName);
	float w = ImGui::GetContentRegionAvail().x;
	if (addUnbindButton)
	{
		w -= 25 * uiScale;
	}
	ImGui::BeginGroup();
	std::string id = text + GenerateItemId();
	if (ImGui::Button(id.c_str(), ImVec2(w, 0)))
	{
		returnValue = 1;
	}
	if (addUnbindButton)
	{
		ImGui::SameLine();
		std::string id2 = "X" + GenerateItemId();
		if (ImGui::Button(id2.c_str()))
		{
			returnValue = 2;
		}
	}
	ImGui::EndGroup();
	return returnValue;
}

#endif // #if defined(EDITOR)