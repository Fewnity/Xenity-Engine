#pragma once
#if defined(_WIN32) || defined(_WIN64)

#include <string>
#include <memory>

// ImGui
#include <imgui/imgui.h>

#include <engine/reflection/reflection.h>
#include <engine/file_system/file_reference.h>
#include <engine/component.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <editor/command/command.h>
#include <editor/command/commands/inspector_commands.h>

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/material.h>
#include <engine/audio/audio_clip.h>
#include <engine/file_system/file.h>
#include <engine/scene_management/scene.h>

class Reflective;
class ProjectDirectory;
class Collider;

enum EditorUIError
{
	EDITOR_UI_ERROR_MISSING_FONT = -1,
};

enum IconName
{
	Icon_Folder,
	Icon_File,
	Icon_Scene,
	Icon_Image,
	Icon_Mesh,
	Icon_Code,
	Icon_Header,
	Icon_Audio,
	Icon_Sky,
	Icon_Font,
	Icon_Audio_Source,
	Icon_Light,
	Icon_Camera,
	Icon_Material,
	Icon_Shader,
	Icon_Count,
};

enum DialogType
{
	Dialog_Type_OK,
	Dialog_Type_OK_CANCEL,
	Dialog_Type_ABORT_RETRY_IGNORE,
	Dialog_Type_YES_NO_CANCEL,
	Dialog_Type_YES_NO,
	Dialog_Type_RETRY_CANCEL,
};

enum DialogResult
{
	Dialog_YES,
	Dialog_NO,
	Dialog_CANCEL,
};

class MultiDragData
{
public:
	std::vector<GameObject*> gameObjects;
	std::vector<Transform*> transforms;
	std::vector<Component*> components;
};

class EditorUI
{
public:
	static MultiDragData multiDragData;
	[[nodiscard]] static int Init();
	static void Draw();
	static void NewFrame();
	static void Render();
	static void SetRoundedCorner(float value);

	static std::string GetPrettyVariableName(std::string variableName);

	static void DrawTextCentered(const std::string& text);
	static int DrawInputButton(const std::string& inputName, const std::string& text, bool addUnbindButton);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<FileReference>& ref);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<ProjectDirectory>& ref);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Component>& ref);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Collider>& ref);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<GameObject>& ref);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Transform>& ref);

	static bool DrawInput(const std::string& inputName, Color& value);
	static bool DrawInput(const std::string& inputName, Vector2& value);
	static bool DrawInput(const std::string& inputName, Vector2Int& value);
	static bool DrawInput(const std::string& inputName, Vector3& value);
	static bool DrawInput(const std::string& inputName, Vector4& value);
	static bool DrawInput(const std::string& inputName, float& value);
	static bool DrawInput(const std::string& inputName, double& value);
	static bool DrawInput(const std::string& inputName, std::string& value);
	static bool DrawInput(const std::string& inputName, int& value);
	static bool DrawInput(const std::string& inputName, bool& value);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Component>& value, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Collider>& value, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<GameObject>& value, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Transform>& value, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::shared_ptr<SkyBox>& value);

	static bool DrawInput(const std::string& inputName, Color value, Color &newValue);
	static bool DrawInput(const std::string& inputName, Vector2 value, Vector2 &newValue);
	static bool DrawInput(const std::string& inputName, Vector2Int value, Vector2Int &newValue);
	static bool DrawInput(const std::string& inputName, Vector3 value, Vector3 &newValue);
	static bool DrawInput(const std::string& inputName, Vector4 value, Vector4 &newValue);
	static bool DrawInput(const std::string& inputName, float value, float &newValue);
	static bool DrawInput(const std::string& inputName, double value, double&newValue);
	static bool DrawInput(const std::string& inputName, std::string value, std::string& newValue);
	static bool DrawInput(const std::string& inputName, int value, int&newValue);
	static bool DrawInput(const std::string& inputName, bool value, bool& newValue);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Component> value, std::weak_ptr<Component>& newValue, uint64_t typeId);

	static bool DrawInput(const std::string& inputName, std::weak_ptr<Collider> value, std::weak_ptr<Collider>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<GameObject> value, std::weak_ptr<GameObject>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Transform> value, std::weak_ptr<Transform>& newValue, uint64_t typeId);
	//static bool DrawInput(const std::string& inputName, std::shared_ptr<SkyBox> value, std::shared_ptr<SkyBox>& newValue);

	static int DrawTreeItem(const std::shared_ptr<GameObject>& child, std::shared_ptr<GameObject>& rightClickedElement);
	static bool DrawTreeItem(std::shared_ptr <ProjectDirectory> projectDir);
	static void DrawInputTitle(const std::string& title);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, float& value);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, int& value);
	//static bool DrawReflection(Reflective& reflection);
	//static bool DrawMap(const std::unordered_map<std::string, ReflectiveEntry>& myMap);

	template<typename T>
	static bool DrawReflectiveData(const ReflectiveData& myMap, std::shared_ptr<Command>& command, std::shared_ptr<T> parent)
	{
		bool valueChanged = false;
		for (const auto& kv : myMap)
		{
			std::string variableName = GetPrettyVariableName(kv.first);
			ReflectiveEntry reflectionEntry = kv.second;
			if (reflectionEntry.isPublic)
			{
				bool valueChangedTemp = false;
				VariableReference variableRef = kv.second.variable.value();
				if (auto valuePtr = std::get_if<std::reference_wrapper<int>>(&variableRef)) // Supported basic type
				{
					int newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, int>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))// Supported basic type
				{
					float newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, float>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<double>>(&variableRef))// Supported basic type
				{
					double newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, double>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::string>>(&variableRef))// Supported basic type
				{
					std::string newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, std::string>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<bool>>(&variableRef)) // Supported basic type
				{
					bool newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, bool>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef)) // Supported basic type
				{
					std::weak_ptr<Component> newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue, reflectionEntry.typeId);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, std::weak_ptr<Component>>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Collider>>>(&variableRef)) // Supported basic type
				{
					std::weak_ptr<Collider> newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue, reflectionEntry.typeId);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, std::weak_ptr<Collider>>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef)) // Supported basic type
				{
					std::weak_ptr<GameObject> newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue, reflectionEntry.typeId);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, std::weak_ptr<GameObject>>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef)) // Supported basic type
				{
					std::weak_ptr<Transform> newValue;
					valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue, reflectionEntry.typeId);
					if (valueChangedTemp)
						command = std::make_shared<InspectorChangeValueCommand<T, std::weak_ptr<Transform>>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflective>>(&variableRef))
				{
					if (auto val = dynamic_cast<Vector2*>(&valuePtr->get())) // Specific draw
					{
						Vector2 newValue;
						valueChangedTemp = DrawInput(variableName, *val, newValue);
						if (valueChangedTemp)
							command = std::make_shared<InspectorChangeValueCommand<T, Vector2>>(parent, val, newValue, *val);
					}
					else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get())) // Specific draw
					{
						Vector2Int newValue;
						valueChangedTemp = DrawInput(variableName, *val, newValue);
						if (valueChangedTemp)
							command = std::make_shared<InspectorChangeValueCommand<T, Vector2Int>>(parent, val, newValue, *val);
					}
					else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get())) // Specific draw
					{
						Vector3 newValue;
						valueChangedTemp = DrawInput(variableName, *val, newValue);
						if (valueChangedTemp)
							command = std::make_shared<InspectorChangeValueCommand<T, Vector3>>(parent, val, newValue, *val);
					}
					else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get())) // Specific draw
					{
						Vector4 newValue;
						valueChangedTemp = DrawInput(variableName, *val, newValue);
						if (valueChangedTemp)
							command = std::make_shared<InspectorChangeValueCommand<T, Vector4>>(parent, val, newValue, *val);
					}
					else if (auto val = dynamic_cast<Color*>(&valuePtr->get())) // Specific draw
					{
						Color newValue;
						valueChangedTemp = DrawInput(variableName, *val, newValue);
						if (valueChangedTemp)
							command = std::make_shared<InspectorChangeValueCommand<T, Color>>(parent, val, newValue, *val);
					}
					else //Basic draw
					{
						std::string headerName = variableName + "##ListHeader" + std::to_string((uint64_t)valuePtr);
						if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
						{
							DrawReflectiveData(valuePtr->get().GetReflectiveData(), command, parent);
						}
					}
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

	static std::string OpenFolderDialog(const std::string& title, const std::string& defaultLocation);
	static std::string OpenFileDialog(const std::string& title, const std::string& defaultLocation);
	static std::string SaveFileDialog(const std::string& title, const std::string& defaultLocation);

	static DialogResult OpenDialog(const std::string& title, const std::string& message, DialogType type);

	static std::string GenerateItemId();
	static bool showProfiler;
	static bool showEditor;
	static bool showEngineSettings;
	static bool showProjectsSettings;
	static bool showLightingSettings;
	static bool showCreateClass;
	static std::vector<std::shared_ptr<Texture>> icons;

	static float GetUiScale()
	{
		return uiScale;
	}

	template <typename T>
	static void DrawFileReference(FileType fileType, const std::string& className, std::reference_wrapper<std::shared_ptr<T>>* valuePtr, bool& valueChangedTemp, const std::string& variableName)
	{
		std::string inputText = "None (" + className + ")";
		auto ptr = valuePtr->get();
		if (ptr != nullptr)
		{
			if (ptr->file != nullptr)
				inputText = ptr->file->GetFileName();
			else
				inputText = "Filled but invalid file reference (" + className + ")";

			inputText += " " + std::to_string(ptr->fileId) + " ";
			if (ptr->file)
				inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
		}

		if (DrawInputButton(variableName, inputText, true) == 2)
		{
			valuePtr->get() = nullptr;
		}

		std::shared_ptr <FileReference> ref = nullptr;
		std::string payloadName = "Files" + std::to_string(fileType);
		if (DragDropTarget(payloadName, ref))
		{
			valuePtr->get() = std::dynamic_pointer_cast<T>(ref);
			valueChangedTemp = true;
		}
	}

	template <typename T>
	static void DrawVectorSimpleType(std::reference_wrapper<std::vector<T>>* valuePtr, bool& valueChangedTemp, const std::string& variableName)
	{
		size_t vectorSize = valuePtr->get().size();
		ImGui::Text(variableName.c_str());
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			const auto& ptr = valuePtr->get()[vectorI];
			DrawInput("", (int&)ptr);
			//if()
		}

		std::string addText = "Add " + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr->get().push_back(0);
		}

		std::string removeText = "Remove " + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			size_t textureSize = valuePtr->get().size();
			if (textureSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + textureSize - 1);
			}
		}
	}

	template <typename T>
	static void DrawVector(bool isFileReference, const std::string& className, std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, bool& valueChangedTemp, const std::string& variableName, const uint64_t& dragdropId)
	{
		size_t vectorSize = valuePtr->get().size();
		std::string headerName = variableName + "##ListHeader" + std::to_string((uint64_t)valuePtr);
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
			{
				std::string inputText = "None (" + className + ")";
				const auto& ptr = valuePtr->get()[vectorI];
				if (ptr != nullptr)
				{
					if (ptr->file != nullptr)
						inputText = ptr->file->GetFileName();
					else
						inputText = "Filled but invalid " + className;

					inputText += " " + std::to_string(ptr->fileId) + " ";
					if (ptr->file)
						inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
				}

				if (DrawInputButton("", inputText, true) == 2)
				{
					valuePtr->get()[vectorI] = nullptr;
				}

				std::shared_ptr <FileReference> ref = nullptr;
				std::string payloadName = "Files" + std::to_string(dragdropId);
				if (DragDropTarget(payloadName, ref))
				{
					valuePtr->get()[vectorI] = std::dynamic_pointer_cast<T>(ref);
					valueChangedTemp = true;
				}
			}

			std::string addText = "Add " + className + GenerateItemId();
			if (ImGui::Button(addText.c_str()))
			{
				valuePtr->get().push_back(nullptr);
			}

			std::string removeText = "Remove " + className + GenerateItemId();
			if (ImGui::Button(removeText.c_str()))
			{
				if (vectorSize != 0)
				{
					valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
				}
			}
		}
	}

	static void DrawVector(const std::string& className, std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>* valuePtr, bool& valueChangedTemp, const std::string& variableName, const uint64_t& dragdropId)
	{
		size_t vectorSize = valuePtr->get().size();
		ImGui::Text(variableName.c_str());
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			std::string inputText = "None (" + className + ")";
			const auto& ptr = valuePtr->get()[vectorI].lock();
			if (ptr != nullptr)
			{
				inputText = ptr->GetGameObject()->name;
			}

			if (DrawInputButton("", inputText, true) == 2)
			{
				valuePtr->get()[vectorI] = std::weak_ptr<Transform>();
			}

			std::shared_ptr <Transform> ref = nullptr;
			std::string payloadName = "Type" + std::to_string(dragdropId);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get()[vectorI] = ref;
				valueChangedTemp = true;
			}
		}

		std::string addText = "Add " + className + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr->get().push_back(std::weak_ptr<Transform>());
		}

		std::string removeText = "Remove " + className + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			if (vectorSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
			}
		}
	}

	static void DrawVector(const std::string& className, std::reference_wrapper<std::vector<std::weak_ptr<Component>>>* valuePtr, bool& valueChangedTemp, const std::string& variableName, const uint64_t& dragdropId)
	{
		size_t vectorSize = valuePtr->get().size();
		ImGui::Text(variableName.c_str());
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			std::string inputText = "None (" + className + ")";
			const auto& ptr = valuePtr->get()[vectorI].lock();
			if (ptr != nullptr)
			{
				inputText = ptr->GetGameObject()->name;
			}

			if (DrawInputButton("", inputText, true) == 2)
			{
				valuePtr->get()[vectorI] = std::weak_ptr<Component>();
			}

			std::shared_ptr <Component> ref = nullptr;
			std::string payloadName = "Type" + std::to_string(dragdropId);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get()[vectorI] = ref;
				valueChangedTemp = true;
			}
		}

		std::string addText = "Add " + className + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr->get().push_back(std::weak_ptr<Component>());
		}

		std::string removeText = "Remove " + className + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			if (vectorSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
			}
		}
	}

	static void DrawVector(const std::string& className, std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>* valuePtr, bool& valueChangedTemp, const std::string& variableName, const uint64_t& dragdropId)
	{
		size_t vectorSize = valuePtr->get().size();
		ImGui::Text(variableName.c_str());
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			std::string inputText = "None (" + className + ")";
			const auto& ptr = valuePtr->get()[vectorI].lock();
			if (ptr != nullptr)
			{
				inputText = ptr->name + " " + std::to_string(ptr->GetUniqueId());
			}

			if (DrawInputButton("", inputText, true) == 2)
			{
				valuePtr->get()[vectorI] = std::weak_ptr<GameObject>();
			}

			std::shared_ptr <GameObject> ref = nullptr;
			std::string payloadName = "Type" + std::to_string(dragdropId);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get()[vectorI] = ref;
				valueChangedTemp = true;
			}
		}

		std::string addText = "Add " + className + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr->get().push_back(std::weak_ptr<GameObject>());
		}

		std::string removeText = "Remove " + className + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			if (vectorSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
			}
		}
	}
private:
	static int uiId;
	static float uiScale;
	static void UpdateUIScale();
};
#endif