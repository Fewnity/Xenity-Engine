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
#include "menus/select_asset_menu.h"

class Reflective;
class ProjectDirectory;
class Collider;

enum class EditorUIError
{
	EDITOR_UI_ERROR_MISSING_FONT = -1,
};

enum class IconName
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
	Icon_Play,
	Icon_Pause,
	Icon_Stop,
	Icon_Camera_Move,
	Icon_Move,
	Icon_Rotate,
	Icon_Scale,
	Icon_Count,
};

enum class DialogType
{
	Dialog_Type_OK,
	Dialog_Type_OK_CANCEL,
	Dialog_Type_ABORT_RETRY_IGNORE,
	Dialog_Type_YES_NO_CANCEL,
	Dialog_Type_YES_NO,
	Dialog_Type_RETRY_CANCEL,
};

enum class DialogResult
{
	Dialog_YES,
	Dialog_NO,
	Dialog_CANCEL,
};

struct MultiDragData
{
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
	static bool DrawInput(const std::string& inputName, uint64_t value, uint64_t& newValue);
	static bool DrawInput(const std::string& inputName, std::string value, std::string& newValue);
	static bool DrawInput(const std::string& inputName, int value, int&newValue);
	static bool DrawInput(const std::string& inputName, bool value, bool& newValue);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Component> value, std::weak_ptr<Component>& newValue, uint64_t typeId);
	static bool DrawEnum(const std::string& inputName, int value, int& newValue, uint64_t enumType);

	static bool DrawInput(const std::string& inputName, std::weak_ptr<Collider> value, std::weak_ptr<Collider>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<GameObject> value, std::weak_ptr<GameObject>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Transform> value, std::weak_ptr<Transform>& newValue, uint64_t typeId);

	static int DrawTreeItem(const std::shared_ptr<GameObject>& child, std::weak_ptr<GameObject>& rightClickedElement);
	static bool DrawTreeItem(std::shared_ptr <ProjectDirectory> projectDir);
	static void DrawInputTitle(const std::string& title);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, float& value);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, int& value);

	template<typename T2, typename T>
	static bool DrawInputOf(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<T>* valuePtr)
	{
		T newValue;
		const bool valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
		if (valueChangedTemp)
			command = std::make_shared<InspectorChangeValueCommand<T2, T>>(parent, &valuePtr->get(), newValue, valuePtr->get());

		return valueChangedTemp;
	}

	template<typename T2, typename T>
	static bool DrawInputOf(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, T* valuePtr)
	{
		T newValue;
		const bool valueChangedTemp = DrawInput(variableName, *valuePtr, newValue);
		if (valueChangedTemp)
			command = std::make_shared<InspectorChangeValueCommand<T2, T>>(parent, valuePtr, newValue, *valuePtr);

		return valueChangedTemp;
	}

	template<typename T2, typename T>
	static bool DrawInputOf(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper <T>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		T newValue;
		const bool valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue, reflectionEntry.typeId);
		if (valueChangedTemp)
			command = std::make_shared<InspectorChangeValueCommand<T2, T>>(parent, &valuePtr->get(), newValue, valuePtr->get());

		return valueChangedTemp;
	}

	template<class T>
	struct is_shared_ptr : std::false_type {};

	template<class T>
	struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

	template<class T>
	struct is_weak_ptr : std::false_type {};

	template<class T>
	struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {};

	template <typename T>
	struct is_vector : std::false_type {};

	template <typename T>
	struct is_vector<std::vector<T>> : std::true_type {};

	// Template for basic types (int, float, strings...)
	template<typename T2, typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, bool>
	static DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<T>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		const bool valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr);
		return valueChangedTemp;
	}
	
	// Template for vectors of reflective (not implemented)
	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<Reflective>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("[Not implemented] Vector for Reflective!");
		bool valueChangedTemp = false;

		//DrawVector("GameObject", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);

		return valueChangedTemp;
	}
	
	// Template for vector of std::weak_ptr (GameObject, Transform, Component)
	template<typename T2, typename T>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		bool valueChangedTemp = false;

		if (std::is_same <T, GameObject>())
		{
			valueChangedTemp = DrawVector("GameObject", valuePtr, variableName, reflectionEntry.typeId);
		}
		else if (std::is_same <T, Transform>())
		{
			valueChangedTemp = DrawVector("Transform", valuePtr, variableName, reflectionEntry.typeId);
		}
		else if (std::is_same <T, Component>())
		{
			valueChangedTemp = DrawVector("Component", valuePtr, variableName, reflectionEntry.typeId);
		}

		return valueChangedTemp;
	}
	
	// Template for vector of std::shared_ptr (Files: MeshData, AudioClip, Texture...)
	template<typename T2, typename T>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		const bool valueChangedTemp = DrawVector(valuePtr, variableName);
		return valueChangedTemp;
	}

	// Template for std::shared_ptr (Files: MeshData, AudioClip, Texture...)
	template<typename T2, typename T>
	std::enable_if_t<is_shared_ptr<T>::value, bool>
	static DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<T>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		const bool valueChangedTemp = DrawFileReference(valuePtr, variableName);
		return valueChangedTemp;
	}

	// Template for std::weak_ptr (GameObject, Transform, Component)
	template<typename T2, typename T>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::weak_ptr<T>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		const bool valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);
		return valueChangedTemp;
	}

	// Template for reflective (Custom reflective, Vectors, Color)
	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<Reflective>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		bool valueChangedTemp = false;

		if (auto val = dynamic_cast<Vector2*>(&valuePtr->get())) // Specific draw
		{
			valueChangedTemp = DrawInputOf(variableName, command, parent, val);
		}
		else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get())) // Specific draw
		{
			valueChangedTemp = DrawInputOf(variableName, command, parent, val);
		}
		else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get())) // Specific draw
		{
			valueChangedTemp = DrawInputOf(variableName, command, parent, val);
		}
		else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get())) // Specific draw
		{
			valueChangedTemp = DrawInputOf(variableName, command, parent, val);
		}
		else if (auto val = dynamic_cast<Color*>(&valuePtr->get())) // Specific draw
		{
			valueChangedTemp = DrawInputOf(variableName, command, parent, val);
		}
		else //Basic draw
		{
			std::string headerName = variableName + "##ListHeader" + std::to_string((uint64_t)valuePtr);
			if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
			{
				DrawReflectiveData(valuePtr->get().GetReflectiveData(), command, parent);
			}
		}

		return valueChangedTemp;
	}

	template<typename... Types, typename ParentType>
	static bool ProcessVariant(const std::variant<Types...>& var, const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<ParentType> parent, const ReflectiveEntry& reflectionEntry) {
		bool valueChangedTemp = false;
		std::visit([&valueChangedTemp, &variableName, &command, &parent, &reflectionEntry](auto value)
			{
				valueChangedTemp = DrawInputOf2(variableName, command, parent, &value, reflectionEntry);
			}, var);

		return valueChangedTemp;
	}

	template<typename T>
	static bool DrawReflectiveData(const ReflectiveData& myMap, std::shared_ptr<Command>& command, std::shared_ptr<T> parent)
	{
		bool valueChanged = false;
		for (const auto& kv : myMap)
		{
			const std::string variableName = GetPrettyVariableName(kv.first);
			const ReflectiveEntry& reflectionEntry = kv.second;
			if (reflectionEntry.isPublic)
			{
				bool valueChangedTemp = false;
				const VariableReference& variableRef = kv.second.variable.value();
				valueChangedTemp = ProcessVariant(variableRef, variableName, command, parent, reflectionEntry);

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

	static std::vector<std::shared_ptr<Texture>> icons;
	static std::shared_ptr<Menu> currentSelectAssetMenu;

	static float GetUiScale()
	{
		return uiScale;
	}

	template <typename T>
	static bool DrawFileReference(std::reference_wrapper<std::shared_ptr<T>>* valuePtr, const std::string& variableName)
	{
		bool valueChangedTemp = false;
		const ClassRegistry::FileClassInfo& classInfo = ClassRegistry::GetFileClassInfo<T>();

		std::string inputText = "None (" + classInfo.name + ")";
		const auto ptr = valuePtr->get();
		if (ptr != nullptr)
		{
			if (ptr->file != nullptr)
				inputText = ptr->file->GetFileName();
			else
				inputText = "Filled but invalid file reference (" + classInfo.name + ")";

			inputText += " " + std::to_string(ptr->fileId) + " ";
			if (ptr->file)
				inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
		}
		int returnValue = DrawInputButton(variableName, inputText, true);
		if (returnValue == 2)
		{
			valuePtr->get() = nullptr;
		}
		else if (returnValue == 1)
		{
			if (currentSelectAssetMenu)
				Editor::RemoveMenu(currentSelectAssetMenu.get());

			std::shared_ptr<SelectAssetMenu<T>> selectMenu = Editor::AddMenu<SelectAssetMenu<T>>(true);
			selectMenu->SetActive(true);
			selectMenu->valuePtr = *valuePtr;
			selectMenu->SearchFiles(classInfo.fileType);
			currentSelectAssetMenu = selectMenu;
			selectMenu->Focus();
		}

		std::shared_ptr <FileReference> ref = nullptr;
		const std::string payloadName = "Files" + std::to_string((int)classInfo.fileType);
		if (DragDropTarget(payloadName, ref))
		{
			valuePtr->get() = std::dynamic_pointer_cast<T>(ref);
			valueChangedTemp = true;
		}

		return valueChangedTemp;
	}

	template <typename T>
	static bool DrawVectorSimpleType(std::reference_wrapper<std::vector<T>>* valuePtr, const std::string& variableName)
	{
		bool valueChangedTemp = false;
		const size_t vectorSize = valuePtr->get().size();
		ImGui::Text(variableName.c_str());
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			const auto& ptr = valuePtr->get()[vectorI];
			DrawInput("", (int&)ptr);
		}

		const std::string addText = "Add " + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr->get().push_back(0);
		}

		const std::string removeText = "Remove " + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			const size_t textureSize = valuePtr->get().size();
			if (textureSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + textureSize - 1);
			}
		}

		return valueChangedTemp;
	}

	template <typename T>
	static bool DrawVector(std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, const std::string& variableName)
	{
		bool valueChangedTemp = false;
		const ClassRegistry::FileClassInfo& classInfo = ClassRegistry::GetFileClassInfo<T>();

		const size_t vectorSize = valuePtr->get().size();
		const std::string headerName = variableName + "##ListHeader" + std::to_string((uint64_t)valuePtr);
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
			{
				std::string inputText = "None (" + classInfo.name + ")";
				const auto& ptr = valuePtr->get()[vectorI];
				if (ptr != nullptr)
				{
					if (ptr->file != nullptr)
						inputText = ptr->file->GetFileName();
					else
						inputText = "Filled but invalid " + classInfo.name;

					inputText += " " + std::to_string(ptr->fileId) + " ";
					if (ptr->file)
						inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
				}

				int result = DrawInputButton("", inputText, true);
				if (result == 2)
				{
					valuePtr->get()[vectorI] = nullptr;
				}
				else if (result == 1)
				{
					if (currentSelectAssetMenu)
						Editor::RemoveMenu(currentSelectAssetMenu.get());

					std::shared_ptr<SelectAssetMenu<T>> selectMenu = Editor::AddMenu<SelectAssetMenu<T>>(true);
					selectMenu->SetActive(true);
					selectMenu->valuePtr = (valuePtr->get()[vectorI]);
					selectMenu->SearchFiles(classInfo.fileType);
					currentSelectAssetMenu = selectMenu;
					selectMenu->Focus();
				}

				std::shared_ptr <FileReference> ref = nullptr;
				const std::string payloadName = "Files" + std::to_string((int)classInfo.fileType);
				if (DragDropTarget(payloadName, ref))
				{
					valuePtr->get()[vectorI] = std::dynamic_pointer_cast<T>(ref);
					valueChangedTemp = true;
				}
			}

			const std::string addText = "Add " + classInfo.name + GenerateItemId();
			if (ImGui::Button(addText.c_str()))
			{
				valuePtr->get().push_back(nullptr);
			}

			const std::string removeText = "Remove " + classInfo.name + GenerateItemId();
			if (ImGui::Button(removeText.c_str()))
			{
				if (vectorSize != 0)
				{
					valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
				}
			}
		}

		return valueChangedTemp;
	}

	template <typename T>
	static bool DrawVector(const std::string& className, std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr, const std::string& variableName, const uint64_t& dragdropId)
	{
		bool valueChangedTemp = false;
		const size_t vectorSize = valuePtr->get().size();
		ImGui::Text(variableName.c_str());
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			std::string inputText = "None (" + className + ")";
			const auto& ptr = valuePtr->get()[vectorI].lock();
			if (ptr != nullptr)
			{
				if constexpr (std::is_same <T, GameObject>())
					inputText = ptr->name + " " + std::to_string(ptr->GetUniqueId());
				else
					inputText = ptr->GetGameObject()->name;
			}

			if (DrawInputButton("", inputText, true) == 2)
			{
				valuePtr->get()[vectorI] = std::weak_ptr<T>();
			}

			std::shared_ptr <T> ref = nullptr;
			const std::string payloadName = "Type" + std::to_string(dragdropId);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get()[vectorI] = ref;
				valueChangedTemp = true;
			}
		}

		const std::string addText = "Add " + className + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr->get().push_back(std::weak_ptr<T>());
		}

		const std::string removeText = "Remove " + className + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			if (vectorSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
			}
		}

		return valueChangedTemp;
	}

	static void SetButtonColor(bool isSelected);
	static void EndButtonColor();

private:
	static int uiId;
	static float uiScale;
	static void UpdateUIScale();
	static void LoadEditorIcon(IconName iconName, const std::string& path);
};
#endif