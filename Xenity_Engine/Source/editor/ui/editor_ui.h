// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/command/command.h>
#include <editor/command/command_manager.h>
#include <editor/command/commands/modify.h>

#include <engine/reflection/reflection.h>
#include <engine/file_system/file_reference.h>
#include <engine/component.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

// List of all types drawn by the EditorUI or the editor wont compile
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/texture.h>

#include "menus/select_asset_menu.h"

class Reflective;
class ProjectDirectory;
class Collider;

#include "reflective_data_to_draw.h"

#include <engine/tools/template_utils.h>

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
	Icon_Point_Light,
	Icon_Sun_Light,
	Icon_Spot_Light,
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
	Icon_Grid,
	Icon_Platform_Windows,
	Icon_Platform_Linux,
	Icon_Platform_PSP,
	Icon_Platform_PSVITA,
	Icon_Platform_PS2,
	Icon_Platform_PS3,
	Icon_Platform_PS4,
	Icon_Count, // Last item
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

enum class ValueInputState
{
	NO_CHANGE,
	CHANGED,
	APPLIED,
	ON_OPEN,
};

enum class InputButtonState
{
	Null,
	OpenAssetMenu,
	ResetValue,
};

struct MultiDragData
{
	std::vector<GameObject*> gameObjects;
	std::vector<Transform*> transforms;
	std::vector<Component*> components;
};

enum class CopyType
{
	Null,
	Component,
	Transform,
};

class EditorUI
{
public:
	static MultiDragData multiDragData;

	/**
	* @brief Initialize the editor UI
	* @return 0 if no error
	*/
	[[nodiscard]] static int Init();

	/**
	* @brief Start a new frame
	*/
	static void NewFrame();

	/**
	* @brief Render at screen the ui
	*/
	static void Render();

	/**
	* @brief Set rounded corner value
	* @param value Rounded corner value
	*/
	static void SetRoundedCorner(float value);

	/**
	* @brief Get a pretty name from a variable name (Capitalize first letter and add space betwwen capitalized letters)
	* @param variableName Variable name to convert
	* @return Pretty variable name
	*/
	static std::string GetPrettyVariableName(std::string variableName);

	static unsigned int GetTextureId(const Texture& texture);

	/**
	* @brief Draw a centered text
	* @param text Text to draw
	*/
	static void DrawTextCentered(const std::string& text);

	/**
	* @brief Draw an input button for file assets
	* @param inputName Name of the input
	* @param buttonText Text to display on the button
	* @param addUnbindButton Add an unbind button
	* @return InputButtonState
	*/
	static InputButtonState DrawInputButton(const std::string& inputName, const std::string& buttonText, bool addUnbindButton);

	/**
	* @brief Add a drag drop target to the previous UI element
	* @param name Name of the drag drop target
	* @param ref Reference to the object to set
	* @param getOnMouseRelease Get the object on mouse release or every frame on hover
	*/
	static bool DragDropTarget(const std::string& name, std::shared_ptr<FileReference>& ref, bool getOnMouseRelease = true);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<ProjectDirectory>& ref, bool getOnMouseRelease = true);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Component>& ref, bool getOnMouseRelease = true);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Collider>& ref, bool getOnMouseRelease = true);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<GameObject>& ref, bool getOnMouseRelease = true);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Transform>& ref, bool getOnMouseRelease = true);

	/**
	* @brief Draw input for specific types Color, Vectors
	*/
	static ValueInputState DrawInput(const std::string& inputName, Color& newValue);
	static ValueInputState DrawInput(const std::string& inputName, Vector2& newValue);
	static ValueInputState DrawInput(const std::string& inputName, Vector2Int& newValue);
	static ValueInputState DrawInput(const std::string& inputName, Vector3& newValue);
	static ValueInputState DrawInput(const std::string& inputName, Vector4& newValue);

	/**
	* @brief Draw input for file references
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, ValueInputState>
		static DrawInput(const std::string& inputName, std::shared_ptr<T>& value)
	{
		ValueInputState state = ValueInputState::NO_CHANGE;
		bool changed = false;
		std::reference_wrapper<std::shared_ptr<T>> ref = std::ref(value);
		std::shared_ptr<T> newValue;
		changed = DrawFileReference(nullptr, ref, inputName, newValue);

		if (changed)
		{
			state = ValueInputState::APPLIED;
			std::shared_ptr<Command> command = std::make_shared<InspectorChangeValueCommand<void, std::shared_ptr<T>>>(std::weak_ptr<void>(), &ref.get(), newValue, ref.get());
			CommandManager::AddCommandAndExecute(command);
		}

		return state;
	}

	/**
	* @brief Draw input for specific types (Component, gameobject, transform...)
	* @param inputName Name of the input
	* @param newValue New value
	* @param typeId Type id of the object (hash of the class)
	* @return True if the value has changed
	*/
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Component>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Collider>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<GameObject>& newValue, uint64_t typeId);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Transform>& newValue, uint64_t typeId);

	/**
	* @brief Draw an enum
	* @param inputName Name of the input
	* @param newValue New value
	* @param enumType Type of the enum
	* @return True if the value has changed
	*/
	static ValueInputState DrawEnum(const std::string& inputName, int& newValue, uint64_t enumType);

	/**
	* @brief Draw gameobject tree
	* @param gameObject Gameobject to draw
	* @parma rightClickedElement Element that has been right clicked
	* @return 0 nothing as been clicked, 1 hovered, 2 clicked, 3 click released (TODO: change to enum)
	*/
	static int DrawTreeItem(const std::shared_ptr<GameObject>& gameObject, std::weak_ptr<GameObject>& rightClickedElement);

	/**
	* @brief Draw tree item for a project directory
	* @param projectDir Project directory to draw
	* @return True if the item has been clicked
	*/
	static bool DrawTreeItem(const std::shared_ptr <ProjectDirectory>& projectDir);

	/**
	* @brief Draw input title
	* @param title Title to draw
	*/
	static void DrawInputTitle(const std::string& title);

	/**
	* @brief Draw a table input (used with ImGui::BeginTable before)
	* @param inputName Name of the input
	* @param inputId Id of the input
	* @param columnIndex Column index
	* @param value Value to draw
	*/
	template<typename T>
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, T& value)
	{
		ImGui::TableSetColumnIndex(columnIndex);
		ImGui::Text("%s", inputName.c_str());
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-1);

		if constexpr (std::is_same<T, float>())
			ImGui::InputFloat(inputId.c_str(), &value, 0, 0, "%.4f");
		else if constexpr (std::is_same<T, int>())
			ImGui::InputInt(inputId.c_str(), &value);
		else if constexpr (std::is_same<T, double>())
			ImGui::InputDouble(inputId.c_str(), &value, 0, 0, "%0.8f");
		else if constexpr (std::is_same<T, bool>())
			ImGui::Checkbox(inputId.c_str(), &value);
		else if constexpr (std::is_same<T, std::string>())
			ImGui::InputText(inputId.c_str(), &value);
	}

	/**
	* @brief Draw input template for basic types (int, float, bool, strings...)
	* @param inputName Name of the input
	* @param valueRef Reference to the value
	*/
	template<typename T>
	static ValueInputState DrawInputTemplate(const std::string& inputName, T& valueRef)
	{
		T value = valueRef;
		ValueInputState returnValue = ValueInputState::NO_CHANGE;

		if constexpr (std::is_same<T, nlohmann::json>())
			return returnValue;

		DrawInputTitle(inputName);
		bool hasChanged = false;

		if constexpr (std::is_same<T, float>())
			hasChanged = ImGui::InputFloat(GenerateItemId().c_str(), &value, 0, 0, "%.4f");
		else if constexpr (std::is_same<T, int>())
			hasChanged = ImGui::InputInt(GenerateItemId().c_str(), &value);
		else if constexpr (std::is_same<T, double>())
			hasChanged = ImGui::InputDouble(GenerateItemId().c_str(), &value, 0, 0, "%0.8f");
		else if constexpr (std::is_same<T, bool>())
			hasChanged = ImGui::Checkbox(GenerateItemId().c_str(), &value);
		else if constexpr (std::is_same<T, std::string>())
			hasChanged = ImGui::InputText(GenerateItemId().c_str(), &value);
		else if constexpr (std::is_same<T, uint64_t>())
			hasChanged = ImGui::InputScalar(GenerateItemId().c_str(), ImGuiDataType_S64, &value);
		/*else if constexpr (std::is_same<T, Vector2>() || std::is_same<T, Vector2Int>() || std::is_same<T, Vector3>() || std::is_same<T, Vector4>()) {

		}*/
		//else if constexpr (std::is_same<T, uint64_t>())

		valueRef = value;

		const bool hasApplied = ImGui::IsItemDeactivatedAfterEdit();
		if (hasApplied)
			returnValue = ValueInputState::APPLIED;
		else if (hasChanged)
			returnValue = ValueInputState::CHANGED;

		return returnValue;
	}

	template<typename T>
	static ValueInputState DrawInputSliderTemplate(const std::string& inputName, T& valueRef, double min, double max)
	{
		T value = valueRef;
		ValueInputState returnValue = ValueInputState::NO_CHANGE;

		if constexpr (std::is_same<T, nlohmann::json>())
			return returnValue;

		DrawInputTitle(inputName);
		bool hasChanged = false;

		if constexpr (std::is_same<T, float>())
			hasChanged = ImGui::SliderFloat(GenerateItemId().c_str(), &value, static_cast<float>(min), static_cast<float>(max), "%.4f");
		else if constexpr (std::is_same<T, int>())
			hasChanged = ImGui::SliderInt(GenerateItemId().c_str(), &value, static_cast<int>(min), static_cast<int>(max));
		else if constexpr (std::is_same<T, double>())
			hasChanged = ImGui::SliderScalar(GenerateItemId().c_str(), ImGuiDataType_Double, &value, &min, &max, "%0.8f");

		valueRef = value;

		const bool hasApplied = ImGui::IsItemDeactivatedAfterEdit();
		if (hasApplied)
			returnValue = ValueInputState::APPLIED;
		else if (hasChanged)
			returnValue = ValueInputState::CHANGED;

		return returnValue;
	}

	static ReflectiveDataToDraw CreateReflectiveDataToDraw(AssetPlatform platform);

	template<typename T>
	static ReflectiveDataToDraw CreateReflectiveDataToDraw(T& owner, AssetPlatform platform)
	{
		ReflectiveDataToDraw reflectiveDataToDraw;
		reflectiveDataToDraw.platform = platform;
		if constexpr (std::is_base_of<T, FileReference>())
		{
			reflectiveDataToDraw.ownerType = 0;
			reflectiveDataToDraw.ownerUniqueId = owner.m_fileId;
		}
		else if constexpr (std::is_base_of<T, GameObject>())
		{
			reflectiveDataToDraw.ownerType = 1;
			reflectiveDataToDraw.ownerUniqueId = owner.GetUniqueId();
		}
		else if constexpr (std::is_base_of<T, Component>())
		{
			reflectiveDataToDraw.ownerType = 2;
			reflectiveDataToDraw.ownerUniqueId = owner.GetUniqueId();
		}

		return reflectiveDataToDraw;
	}

	/**
	* @brief Draw reflective data
	* @param myMap Reflective data to draw
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @return True if the value has changed
	*/
	static ValueInputState DrawReflectiveData(ReflectiveDataToDraw& reflectiveDataToDraw, const ReflectiveData& myMap, Event<>* _onValueChangedEvent);

	/**
	* @brief Open folder dialog (Windows only)
	* @param title Title of the dialog
	* @param defaultLocation Default location
	* @return Path of the folder
	*/
	static std::string OpenFolderDialog(const std::string& title, const std::string& defaultLocation);

	/**
	* @brief Open file dialog (Windows only)
	* @param title Title of the dialog
	* @param defaultLocation Default location
	* @return Path of the file
	*/
	static std::string OpenFileDialog(const std::string& title, const std::string& defaultLocation);

	/**
	* @brief Save file dialog (Windows only)
	* @param title Title of the dialog
	* @param defaultLocation Default location
	* @return Path of the file
	*/
	static std::string SaveFileDialog(const std::string& title, const std::string& defaultLocation);

	/**
	* @brief Open a dialog box (Windows only)
	* @param title Title of the dialog
	* @param message Message of the dialog
	* @param type Type of the dialog
	* @return Result of the dialog
	*/
	static DialogResult OpenDialog(const std::string& title, const std::string& message, DialogType type);

	/**
	* @brief Generate an id for the next ui element
	*/
	static std::string GenerateItemId();

	static std::vector<std::shared_ptr<Texture>> icons;
	static std::map<std::string, std::shared_ptr<Texture>> componentsIcons;
	static std::shared_ptr<Menu> currentSelectAssetMenu;

	/**
	* @brief Get the ui scale based on the screen resolution/settings
	*/
	static float GetUiScale()
	{
		return uiScale;
	}

	/**
	* @brief Draw a file reference
	* @param valuePtr Reference to the file reference
	* @param variableName Name of the variable
	* @param newValue New value
	* @return True if the value has changed
	*/
	template <typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
		static DrawFileReference(ReflectiveDataToDraw* reflectiveDataToDraw, const std::reference_wrapper<std::shared_ptr<T>> valuePtr, const std::string& variableName, std::shared_ptr<T>& newValue)
	{
		bool valueChangedTemp = false;
		const ClassRegistry::FileClassInfo* classInfo = ClassRegistry::GetFileClassInfoById(reflectiveDataToDraw->currentEntry.typeId);
		std::string inputText = "None (" + classInfo->name + ")";
		const std::shared_ptr<T> ptr = valuePtr.get();
		if (ptr != nullptr)
		{
			if (ptr->m_file != nullptr)
				inputText = ptr->m_file->GetFileName();
			else
				inputText = "Filled but invalid file reference (" + classInfo->name + ")";

			inputText += " " + std::to_string(ptr->m_fileId) + " ";
			if (ptr->m_file)
				inputText += " " + std::to_string(ptr->m_file->GetUniqueId()) + " ";
		}
		const InputButtonState returnValue = DrawInputButton(variableName, inputText, true);
		if (returnValue == InputButtonState::ResetValue)
		{
			newValue = nullptr;
			valueChangedTemp = true;
		}
		else if (returnValue == InputButtonState::OpenAssetMenu)
		{
			if (currentSelectAssetMenu)
				Editor::RemoveMenu(currentSelectAssetMenu.get());

			std::shared_ptr<SelectAssetMenu<T>> selectMenu = Editor::AddMenu<SelectAssetMenu<T>>(true);
			selectMenu->onValueChangedEvent = onValueChangedEvent;
			selectMenu->valuePtr = valuePtr;
			selectMenu->SearchFiles(classInfo->fileType);
			if (reflectiveDataToDraw)
			{
				selectMenu->reflectiveDataToDraw = *reflectiveDataToDraw;
				selectMenu->hasReflectiveDataToDraw = true;
			}
			currentSelectAssetMenu = selectMenu;
			selectMenu->Focus();
		}

		std::shared_ptr <FileReference> ref = nullptr;
		const std::string payloadName = "Files" + std::to_string((int)classInfo->fileType);
		if (DragDropTarget(payloadName, ref))
		{
			newValue = std::dynamic_pointer_cast<T>(ref);
			valueChangedTemp = true;
		}
		if (!variableName.empty())
			ImGui::Separator();
		else
			ImGui::Spacing();

		return valueChangedTemp;
	}

	/**
	* @brief Draw a vector of file references
	* @param valuePtr Reference to the vector of file references
	* @param variableName Name of the variable
	* @return True if the value has changed
	*/
	template <typename T>
	static bool DrawVector(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::vector<std::shared_ptr<T>>> valuePtr)
	{
		bool listChangedTemp = false;

		const std::string headerName = reflectiveDataToDraw.name + "##ListHeader" + std::to_string((uint64_t)&valuePtr.get());
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			const ClassRegistry::FileClassInfo* classInfo = ClassRegistry::GetFileClassInfoById(reflectiveDataToDraw.currentEntry.typeId);
			const size_t vectorSize = valuePtr.get().size();
			const std::string tempName = reflectiveDataToDraw.name;
			reflectiveDataToDraw.name = "";
			for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
			{
				std::shared_ptr<T> newValue = nullptr;
				ValueInputState variableChangedTemp = DrawVariable(reflectiveDataToDraw, std::ref(valuePtr.get()[vectorI]));
				if (variableChangedTemp != ValueInputState::NO_CHANGE)
				{
					listChangedTemp = true;
					valuePtr.get()[vectorI] = newValue;
				}
			}
			reflectiveDataToDraw.name = tempName;
			const std::string addText = "Add " + classInfo->name + GenerateItemId();
			if (ImGui::Button(addText.c_str()))
			{
				valuePtr.get().push_back(nullptr);
				listChangedTemp = true;
			}

			const std::string removeText = "Remove " + classInfo->name + GenerateItemId();
			if (ImGui::Button(removeText.c_str()))
			{
				if (vectorSize != 0)
				{
					valuePtr.get().erase(valuePtr.get().begin() + vectorSize - 1);
					listChangedTemp = true;
				}
			}
		}
		ImGui::Separator();
		return listChangedTemp;
	}

	/**
	* @brief Draw a vector of std::weak_ptr (GameObject, Transform, Component...)
	* @param className Name of the class
	* @param valuePtr Reference to the vector of std::weak_ptr
	* @param variableName Name of the variable
	* @param dragdropId Id of the dragdrop
	* @return True if the value has changed
	*/
	template <typename T>
	static bool DrawVector(ReflectiveDataToDraw& reflectiveDataToDraw, const std::string& className, const std::reference_wrapper<std::vector<std::weak_ptr<T>>> valuePtr)
	{
		bool valueChangedTemp = false;
		const std::string headerName = reflectiveDataToDraw.name + "##ListHeader" + std::to_string((uint64_t)&valuePtr.get());
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) 
		{
			const size_t vectorSize = valuePtr.get().size();
			const std::string tempName = reflectiveDataToDraw.name;
			reflectiveDataToDraw.name = "";
			for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
			{
				std::string inputText = "None (" + className + ")";
				const auto& ptr = valuePtr.get()[vectorI].lock();
				if (ptr != nullptr)
				{
					if constexpr (std::is_same <T, GameObject>())
						inputText = ptr->GetName() + " " + std::to_string(ptr->GetUniqueId());
					else
						inputText = ptr->GetGameObject()->GetName();
				}

				const InputButtonState result = DrawInputButton("", inputText, true);
				if (result == InputButtonState::ResetValue)
				{
					valuePtr.get()[vectorI] = std::weak_ptr<T>();
					valueChangedTemp = true;
				}

				std::shared_ptr <T> ref = nullptr;
				const std::string payloadName = "Type" + std::to_string(reflectiveDataToDraw.currentEntry.typeId);
				if (DragDropTarget(payloadName, ref))
				{
					valuePtr.get()[vectorI] = ref;
					valueChangedTemp = true;
				}
			}
			reflectiveDataToDraw.name = tempName;

			const std::string addText = "Add " + className + GenerateItemId();
			if (ImGui::Button(addText.c_str()))
			{
				valuePtr.get().push_back(std::weak_ptr<T>());
				valueChangedTemp = true;
			}

			const std::string removeText = "Remove " + className + GenerateItemId();
			if (ImGui::Button(removeText.c_str()))
			{
				if (vectorSize != 0)
				{
					valuePtr.get().erase(valuePtr.get().begin() + vectorSize - 1);
					valueChangedTemp = true;
				}
			}
			ImGui::Separator();
		}
		return valueChangedTemp;
	}

	static bool DrawVector(ReflectiveDataToDraw& reflectiveDataToDraw, const std::string& className, const std::reference_wrapper<std::vector<Reflective*>> valuePtr);

	template <typename T>
	std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value || std::is_same<T, uint64_t>::value
		|| std::is_same<T, double>::value || std::is_same<T, std::string>::value, bool>
		static DrawVector(ReflectiveDataToDraw& reflectiveDataToDraw, const std::string& className, const std::reference_wrapper<std::vector<T>> valuePtr)
	{
		bool valueChangedTemp = false;
		const std::string headerName = reflectiveDataToDraw.name + "##ListHeader" + std::to_string((uint64_t)&valuePtr.get());
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			std::vector<T>& valueRef = valuePtr.get();
			const size_t vectorSize = valueRef.size();

			const std::string tempName = reflectiveDataToDraw.name;
			reflectiveDataToDraw.name = "";

			for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
			{
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Empty name normally here!!!!
				if (DrawVariable(reflectiveDataToDraw, std::ref(valueRef[vectorI])) != ValueInputState::NO_CHANGE)
				{
					valueChangedTemp = true;
				}
			}
			reflectiveDataToDraw.name = tempName;

			const std::string addText = "Add " + GenerateItemId();
			if (ImGui::Button(addText.c_str()))
			{
				valueRef.push_back(T());
				valueChangedTemp = true;
			}

			const std::string removeText = "Remove " + GenerateItemId();
			if (ImGui::Button(removeText.c_str()))
			{
				if (vectorSize != 0)
				{
					valueRef.erase(valueRef.begin() + vectorSize - 1);
					valueChangedTemp = true;
				}
			}
			ImGui::Separator();
		}
		return valueChangedTemp;
	}

	/**
	* @brief Set next buttons style color
	* @param isSelected Is the button selected
	*/
	static void SetButtonColor(bool isSelected);

	/**
	* @brief End next buttons style color
	*/
	static void EndButtonColor();

	/**
	* @brief Get if an element is being edited
	*/
	static bool IsEditingElement()
	{
		return isEditingElement;
	}

	static CopyType currentCopyType;
	static nlohmann::json copiedComponentJson;
	static std::string copiedComponentName;
	static Event<>* onValueChangedEvent;
private:
	static int uiId;
	static bool isEditingElement;
	static float uiScale;

	/**
	* @brief Update the ui scale based on the screen resolution/settings
	*/
	static void UpdateUIScale();

	/**
	* @brief Load editor icons
	*/
	static void LoadEditorIcon(IconName iconName, const std::string& path);
	static void LoadComponentIcon(std::string iconName, const std::string& path);

	/**
	* @brief Draw input for reflective (Color, Vectors)
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @return True if the value has changed
	*/
	template<typename T>
	static ValueInputState DrawInputReflective(ReflectiveDataToDraw& reflectiveDataToDraw, T* valuePtr)
	{
		T newValue = *valuePtr;
		static T newValue2;
		static size_t canChange = true;
		const ValueInputState valueInputState = DrawInput(reflectiveDataToDraw.name, newValue);

		if constexpr (std::is_same<T, Color>())
		{
			if (valueInputState == ValueInputState::ON_OPEN || canChange == reinterpret_cast<size_t>(valuePtr))
			{
				newValue2 = *valuePtr;
				canChange = 0;
			}

			if (valueInputState == ValueInputState::CHANGED)
			{
				reflectiveDataToDraw.command = std::make_shared<ReflectiveChangeValueCommand<Reflective>>(reflectiveDataToDraw, valuePtr, *valuePtr, newValue);
			}
			if (valueInputState == ValueInputState::APPLIED)
			{
				reflectiveDataToDraw.command = std::make_shared<ReflectiveChangeValueCommand<Reflective>>(reflectiveDataToDraw, valuePtr, newValue2, newValue);
				canChange = reinterpret_cast<size_t>(valuePtr);
			}
		}
		else 
		{
			if (valueInputState == ValueInputState::APPLIED)
			{
				reflectiveDataToDraw.command = std::make_shared<ReflectiveChangeValueCommand<Reflective>>(reflectiveDataToDraw, valuePtr, *valuePtr, newValue);
			}
		}
		return valueInputState;
	}

	/**
	* @brief Draw a variable of basic types (int, float, strings...) and enums
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	*/
	template<typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, ValueInputState>
		static DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<T> valuePtr)
	{
		ValueInputState state = ValueInputState::NO_CHANGE;
		T newValue = valuePtr.get();
		if (!reflectiveDataToDraw.currentEntry.isEnum)
		{
			if(reflectiveDataToDraw.currentEntry.isSlider)
				state = DrawInputSliderTemplate(reflectiveDataToDraw.name, newValue, reflectiveDataToDraw.currentEntry.minSliderValue, reflectiveDataToDraw.currentEntry.maxSliderValue);
			else
				state = DrawInputTemplate(reflectiveDataToDraw.name, newValue);
		}
		else if constexpr (std::is_same<int, T>())
			state = DrawEnum(reflectiveDataToDraw.name, newValue, reflectiveDataToDraw.currentEntry.typeId);

		if (state != ValueInputState::NO_CHANGE)
			reflectiveDataToDraw.command = std::make_shared<ReflectiveChangeValueCommand<T>>(reflectiveDataToDraw, &valuePtr.get(), valuePtr.get(), newValue);

		return state;
	}

	/**
	* @brief Draw a variable of a list of reflectives
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	* @return True if the value has changed
	*/
	static ValueInputState DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::vector<Reflective*>> valuePtr);

	template<typename T>
	std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value || std::is_same<T, uint64_t>::value
		|| std::is_same<T, double>::value || std::is_same<T, bool>::value || std::is_same<T, std::string>::value, ValueInputState>
		static DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::vector<T>> valuePtr)
	{
		ValueInputState valueInputState = ValueInputState::NO_CHANGE;
		bool valueChangedTemp = false;
		valueChangedTemp = DrawVector(reflectiveDataToDraw, "simple value", valuePtr);

		if (valueChangedTemp)
			valueInputState = ValueInputState::APPLIED;
		return valueInputState;
	}

	/**
	* @brief Draw a variable of a list of std::weak_ptr (GameObject, Transform, Component)
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	* @return True if the value has changed
	*/
	template<typename T>
	static ValueInputState DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>& valuePtr)
	{
		ValueInputState valueInputState = ValueInputState::NO_CHANGE;
		bool valueChangedTemp = false;

		if constexpr (std::is_same <T, GameObject>())
		{
			valueChangedTemp = DrawVector(reflectiveDataToDraw, "GameObject", valuePtr);
		}
		else if constexpr (std::is_same <T, Transform>())
		{
			valueChangedTemp = DrawVector(reflectiveDataToDraw, "Transform", valuePtr);
		}
		else if constexpr (std::is_same <T, Component>())
		{
			valueChangedTemp = DrawVector(reflectiveDataToDraw, ClassRegistry::GetClassNameById(reflectiveDataToDraw.currentEntry.typeId), valuePtr);
		}

		if (valueChangedTemp)
			valueInputState = ValueInputState::APPLIED;
		return valueInputState;
	}

	/**
	* @brief Draw a variable of a list of std::shared_ptr (MeshData, AudioClip, Texture...)
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	* @return True if the value has changed
	*/
	template<typename T>
	static ValueInputState DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::vector<std::shared_ptr<T>>> valuePtr)
	{
		ValueInputState valueInputState = ValueInputState::NO_CHANGE;
		const bool valueChangedTemp = DrawVector(reflectiveDataToDraw, valuePtr);

		if (valueChangedTemp)
			valueInputState = ValueInputState::APPLIED;
		return valueInputState;
	}

	/**
	* @brief Draw a variable of std::shared_ptr (MeshData, AudioClip, Texture...)
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	* @return True if the value has changed
	*/
	template<typename T>
	std::enable_if_t<is_shared_ptr<T>::value, ValueInputState>
		static DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<T> valuePtr)
	{
		ValueInputState valueInputState = ValueInputState::NO_CHANGE;

		T newValue = nullptr;
		const bool valueChangedTemp = DrawFileReference(&reflectiveDataToDraw, valuePtr, reflectiveDataToDraw.name, newValue);
		if (valueChangedTemp)
			reflectiveDataToDraw.command = std::make_shared<ReflectiveChangeValueCommand<T>>(reflectiveDataToDraw, &valuePtr.get(), valuePtr.get(), newValue);

		if (valueChangedTemp)
			valueInputState = ValueInputState::APPLIED;
		return valueInputState;
	}

	/**
	* @brief Draw a variable of std::weak_ptr (GameObject, Transform, Component)
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	* @return True if the value has changed
	*/
	template<typename T>
	static ValueInputState DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::weak_ptr<T>> valuePtr)
	{
		ValueInputState valueInputState = ValueInputState::NO_CHANGE;
		std::weak_ptr<T> newValue = valuePtr.get();
		const bool valueChangedTemp = DrawInput(reflectiveDataToDraw.name, newValue, reflectiveDataToDraw.currentEntry.typeId);
		if (valueChangedTemp)
			reflectiveDataToDraw.command = std::make_shared<ReflectiveChangeValueCommand<std::weak_ptr<T>>>(reflectiveDataToDraw, &valuePtr.get(), valuePtr.get(), newValue);

		if (valueChangedTemp)
			valueInputState = ValueInputState::APPLIED;
		return valueInputState;
	}

	/**
	* @brief Draw a variable of reflective (Custom reflective, Vectors, Color)
	* @param variableName Name of the variable
	* @param command Command to apply if the value has changed
	* @param parent Parent of the reflective data
	* @param valuePtr Reference to the value
	* @param reflectionEntry Reflection entry of the variable
	* @return True if the value has changed
	*/
	static ValueInputState DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<Reflective> valuePtr);

	static bool DragDropOrderGameObject(std::shared_ptr <GameObject>& droppedGameObject, const std::shared_ptr <GameObject>& dropAreaOwner, bool isParent, bool isParentOpened);
};