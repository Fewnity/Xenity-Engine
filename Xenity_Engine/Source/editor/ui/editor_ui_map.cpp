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

bool EditorUI::DrawMap(const std::unordered_map<std::string, ReflectionEntry>& myMap)
{
	bool valueChanged = false;
	for (const auto& kv : myMap)
	{
		std::string variableName = GetPrettyVariableName(kv.first);
		ReflectionEntry reflectionEntry = kv.second;
		if (reflectionEntry.isPublic)
		{
			bool valueChangedTemp = false;
			Variable variableRef = kv.second.variable.value();
			if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))// Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))// Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))// Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get());
			else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef)) // Supported basic type{
			{
				bool newValue;
				valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);

			}
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get(), reflectionEntry.typeId);
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Collider>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get(), reflectionEntry.typeId);
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get(), reflectionEntry.typeId);
			else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef)) // Supported basic type
				valueChangedTemp = DrawInput(variableName, valuePtr->get(), reflectionEntry.typeId);
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
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<SkyBox>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Skybox, "SkyBox", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Shader>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Shader, "Shader", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Material>>>(&variableRef))
			{
				DrawFileReference(FileType::File_Material, "Material", valuePtr, valueChangedTemp, variableName);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
			{
				DrawVector(true, "Texture", valuePtr, valueChangedTemp, variableName, FileType::File_Texture);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<MeshData>>>>(&variableRef))
			{
				DrawVector(true, "MeshData", valuePtr, valueChangedTemp, variableName, FileType::File_Mesh);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<AudioClip>>>>(&variableRef))
			{
				DrawVector(true, "AudioClip", valuePtr, valueChangedTemp, variableName, FileType::File_Audio);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Scene>>>>(&variableRef))
			{
				DrawVector(true, "Scene", valuePtr, valueChangedTemp, variableName, FileType::File_Scene);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<SkyBox>>>>(&variableRef))
			{
				DrawVector(true, "SkyBox", valuePtr, valueChangedTemp, variableName, FileType::File_Skybox);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Font>>>>(&variableRef))
			{
				DrawVector(true, "Font", valuePtr, valueChangedTemp, variableName, FileType::File_Font);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Shader>>>>(&variableRef))
			{
				DrawVector(true, "Shader", valuePtr, valueChangedTemp, variableName, FileType::File_Shader);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Material>>>>(&variableRef))
			{
				DrawVector(true, "Material", valuePtr, valueChangedTemp, variableName, FileType::File_Material);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>>(&variableRef))
			{
				DrawVector("Transform", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>>(&variableRef))
			{
				DrawVector("GameObject", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Component>>>>(&variableRef))
			{
				DrawVector("Component", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<int>>>(&variableRef))
			{
				DrawVectorSimpleType(valuePtr, valueChangedTemp, variableName);
			}

			if (valueChangedTemp)
			{
				valueChanged = true;
			}
		}
	}
	return valueChanged;
}

void EditorUI::DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, float& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat(inputId.c_str(), &value, 0, 0, "%.4f");
}

void EditorUI::DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, int& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputInt(inputId.c_str(), &value, 0, 0);
}

int EditorUI::DrawInputButton(const std::string& inputName, const std::string& text, bool addUnbindButton)
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