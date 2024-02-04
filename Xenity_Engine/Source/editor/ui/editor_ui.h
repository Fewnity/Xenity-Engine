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

typedef std::variant <
	std::reference_wrapper<int>,
	std::reference_wrapper<double>,
	std::reference_wrapper<float>, 
	std::reference_wrapper<bool>, 
	std::reference_wrapper<uint64_t>,
	std::reference_wrapper<std::string>,
	std::reference_wrapper<Reflective>,
	std::reference_wrapper<std::shared_ptr<Texture>>,
	std::reference_wrapper<std::shared_ptr<MeshData>>,
	std::reference_wrapper<std::shared_ptr<Scene>>,
	std::reference_wrapper<std::shared_ptr<SkyBox>>,
	std::reference_wrapper<std::shared_ptr<Font>>,
	std::reference_wrapper<std::shared_ptr<Shader>>,
	std::reference_wrapper<std::shared_ptr<Material>>,
	std::reference_wrapper<std::weak_ptr<Component>>,
	std::reference_wrapper<std::weak_ptr<Collider>>,
	std::reference_wrapper<std::weak_ptr<GameObject>>,
	std::reference_wrapper<std::weak_ptr<Transform>>,
	std::reference_wrapper<std::vector<Reflective>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Component>>>> VariableReference2;

//typedef std::variant <
//	std::reference_wrapper<int>,
//	std::reference_wrapper<double>,
//	std::reference_wrapper<float>,
//	std::reference_wrapper<bool>,
//	std::reference_wrapper<uint64_t>,
//	std::reference_wrapper<std::string>> VariableReference2;

//typedef std::variant <
//	std::reference_wrapper<int>,
//	std::reference_wrapper<uint64_t>,
//	std::reference_wrapper<double>,
//	std::reference_wrapper<float>,
//	std::reference_wrapper<bool>> VariableReference2;

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
	//static bool DrawInput(const std::string& inputName, std::shared_ptr<SkyBox> value, std::shared_ptr<SkyBox>& newValue);

	static int DrawTreeItem(const std::shared_ptr<GameObject>& child, std::weak_ptr<GameObject>& rightClickedElement);
	static bool DrawTreeItem(std::shared_ptr <ProjectDirectory> projectDir);
	static void DrawInputTitle(const std::string& title);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, float& value);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, int& value);
	//static bool DrawReflection(Reflective& reflection);
	//static bool DrawMap(const std::unordered_map<std::string, ReflectiveEntry>& myMap);

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

	template<typename T2, typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, bool>
	static DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<T>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Simple!");
		T newValue;
		const bool valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
		if (valueChangedTemp)
			command = std::make_shared<InspectorChangeValueCommand<T2, T>>(parent, &valuePtr->get(), newValue, valuePtr->get());

		return valueChangedTemp;
	}

	/*std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && is_vector<T>::value, bool>
	static DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<T>* valuePtr, const ReflectiveEntry& reflectionEntry)
	*/
	/*std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && is_vector<T>::value, bool>*/
	//std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value&& is_vector<T>::value, bool>
	
	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<Reflective>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("[Not implemented] Vector for Reflective!");
		bool valueChangedTemp = false;

		//DrawVector("GameObject", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);

		return valueChangedTemp;
	}

	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Vector for GameObject!");
		bool valueChangedTemp = false;

		DrawVector("GameObject", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);	

		return valueChangedTemp;
	}

	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Vector for Transform!");
		bool valueChangedTemp = false;
		DrawVector("Transform", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
		return valueChangedTemp;
	}

	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<std::weak_ptr<Component>>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Vector for Component!");
		bool valueChangedTemp = false;
		DrawVector("Component", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
		return valueChangedTemp;
	}
	
	template<typename T2, typename T>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Vector for file!");
		bool valueChangedTemp = false;

		if (std::is_same <T, Texture>())
		{
			DrawVector(FileType::File_Texture, "Texture", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, MeshData>())
		{
			DrawVector(FileType::File_Mesh, "MeshData", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, AudioClip>())
		{
			DrawVector(FileType::File_Audio, "AudioClip", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, Scene>())
		{
			DrawVector(FileType::File_Scene, "Scene", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, SkyBox>())
		{
			DrawVector(FileType::File_Skybox, "SkyBox", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, Font>())
		{
			DrawVector(FileType::File_Font, "Font", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, Shader>())
		{
			DrawVector(FileType::File_Shader, "Shader", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <T, Material>())
		{
			DrawVector(FileType::File_Material, "Material", valuePtr, valueChangedTemp, variableName);
		}

		return valueChangedTemp;
	}

	template<typename T2, typename T>
	std::enable_if_t<is_shared_ptr<T>::value, bool>
	static DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<T>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("File!");

		bool valueChangedTemp = false;
		if (std::is_same <decltype(valuePtr->get().get()), Texture*>())
		{
			DrawFileReference(FileType::File_Texture, "Texture", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), MeshData*>())
		{
			DrawFileReference(FileType::File_Mesh, "MeshData", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), AudioClip*>())
		{
			DrawFileReference(FileType::File_Audio, "AudioClip", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), Scene*>())
		{
			DrawFileReference(FileType::File_Scene, "Scene", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), Font*>())
		{
			DrawFileReference(FileType::File_Font, "Font", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), SkyBox*>())
		{
			DrawFileReference(FileType::File_Skybox, "SkyBox", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), Shader*>())
		{
			DrawFileReference(FileType::File_Shader, "Shader", valuePtr, valueChangedTemp, variableName);
		}
		else if (std::is_same <decltype(valuePtr->get().get()), Material*>())
		{
			DrawFileReference(FileType::File_Material, "Material", valuePtr, valueChangedTemp, variableName);
		}

		return valueChangedTemp;
	}

	template<typename T2>
	//std::enable_if_t<is_weak_ptr<T>::value, bool>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::weak_ptr<Component>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Component!");

		bool valueChangedTemp = false;
		valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);

		return valueChangedTemp;
	}

	template<typename T2>
	//std::enable_if_t<is_weak_ptr<T>::value, bool>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::weak_ptr<Collider>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Collider!");

		bool valueChangedTemp = false;
		valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);

		return valueChangedTemp;
	}

	template<typename T2>
	//std::enable_if_t<is_weak_ptr<T>::value, bool>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::weak_ptr<GameObject>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("GameObject!");

		bool valueChangedTemp = false;
		valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);

		return valueChangedTemp;
	}

	template<typename T2>
	//std::enable_if_t<is_weak_ptr<T>::value, bool>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<std::weak_ptr<Transform>>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Transform!");

		bool valueChangedTemp = false;
		valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);

		return valueChangedTemp;
	}

	template<typename T2>
	static bool DrawInputOf2(const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<T2> parent, std::reference_wrapper<Reflective>* valuePtr, const ReflectiveEntry& reflectionEntry)
	{
		Debug::Print("Reflective!");
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
	static bool processVariant(const std::variant<Types...>& var, const std::string variableName, std::shared_ptr<Command>& command, std::shared_ptr<ParentType> parent, const ReflectiveEntry& reflectionEntry) {
		bool valueChangedTemp = false;
		std::visit([&valueChangedTemp, variableName, &command, &parent, &reflectionEntry](auto value)
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
			const ReflectiveEntry reflectionEntry = kv.second;
			if (reflectionEntry.isPublic)
			{
				bool valueChangedTemp = false;
				VariableReference variableRef = kv.second.variable.value();
				valueChangedTemp = processVariant(variableRef, variableName, command, parent, reflectionEntry);

				/*Color t = Color::CreateFromRGB(14,55,20);
				VariableReference2 variableRef2 = t;
				valueChangedTemp = processVariant(variableRef2, variableName, command, parent, reflectionEntry);

				std::shared_ptr<MeshData> t2 = nullptr;
				VariableReference2 variableRef3 = t2;
				valueChangedTemp = processVariant(variableRef3, variableName, command, parent, reflectionEntry);

				float t3 = 34;
				VariableReference2 variableRef4 = t3;
				valueChangedTemp = processVariant(variableRef4, variableName, command, parent, reflectionEntry);

				std::weak_ptr<Component> t4;
				VariableReference2 variableRef5 = t4;
				valueChangedTemp = processVariant(variableRef5, variableName, command, parent, reflectionEntry);

				std::vector<std::shared_ptr<Texture>> t5;
				VariableReference2 variableRef6 = t5;
				valueChangedTemp = processVariant(variableRef6, variableName, command, parent, reflectionEntry);*/

				//if (auto valuePtr = std::get_if<std::reference_wrapper<int>>(&variableRef)) // Supported basic type
				//{
				//	int newValue;
				//	if(!reflectionEntry.isEnum)
				//		valueChangedTemp = DrawInput(variableName, valuePtr->get(), newValue);
				//	else
				//		valueChangedTemp = DrawEnum(variableName, valuePtr->get(), newValue, reflectionEntry.typeId);
				//	if (valueChangedTemp)
				//		command = std::make_shared<InspectorChangeValueCommand<T, int>>(parent, &valuePtr->get(), newValue, valuePtr->get());
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))// Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<double>>(&variableRef))// Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::string>>(&variableRef))// Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<bool>>(&variableRef)) // Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef)) // Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Collider>>>(&variableRef)) // Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef)) // Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef)) // Supported basic type
				//{
				//	valueChangedTemp = DrawInputOf(variableName, command, parent, valuePtr, reflectionEntry);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflective>>(&variableRef))
				//{
				//	if (auto val = dynamic_cast<Vector2*>(&valuePtr->get())) // Specific draw
				//	{
				//		valueChangedTemp = DrawInputOf(variableName, command, parent, val);
				//	}
				//	else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get())) // Specific draw
				//	{
				//		valueChangedTemp = DrawInputOf(variableName, command, parent, val);
				//	}
				//	else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get())) // Specific draw
				//	{
				//		valueChangedTemp = DrawInputOf(variableName, command, parent, val);
				//	}
				//	else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get())) // Specific draw
				//	{
				//		valueChangedTemp = DrawInputOf(variableName, command, parent, val);
				//	}
				//	else if (auto val = dynamic_cast<Color*>(&valuePtr->get())) // Specific draw
				//	{
				//		valueChangedTemp = DrawInputOf(variableName, command, parent, val);
				//	}
				//	else //Basic draw
				//	{
				//		std::string headerName = variableName + "##ListHeader" + std::to_string((uint64_t)valuePtr);
				//		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
				//		{
				//			DrawReflectiveData(valuePtr->get().GetReflectiveData(), command, parent);
				//		}
				//	}
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<MeshData>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Mesh, "MeshData", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Audio, "AudioClip", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Texture, "Texture", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Scene, "Scene", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Font>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Font, "Font", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<SkyBox>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Skybox, "SkyBox", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Shader>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Shader, "Shader", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Material>>>(&variableRef))
				//{
				//	DrawFileReference(FileType::File_Material, "Material", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Texture, "Texture", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<MeshData>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Mesh, "MeshData", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<AudioClip>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Audio, "AudioClip", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Scene>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Scene, "Scene", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<SkyBox>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Skybox, "SkyBox", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Font>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Font, "Font", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Shader>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Shader, "Shader", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Material>>>>(&variableRef))
				//{
				//	DrawVector(FileType::File_Material, "Material", valuePtr, valueChangedTemp, variableName);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>>(&variableRef))
				//{
				//	DrawVector("Transform", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>>(&variableRef))
				//{
				//	DrawVector("GameObject", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
				//}
				//else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Component>>>>(&variableRef))
				//{
				//	DrawVector("Component", valuePtr, valueChangedTemp, variableName, reflectionEntry.typeId);
				//}
				///*else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<int>>>(&variableRef))
				//{
				//	DrawVectorSimpleType(valuePtr, valueChangedTemp, variableName);
				//}*/

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
	static void DrawFileReference(FileType fileType, const std::string& className, std::reference_wrapper<std::shared_ptr<T>>* valuePtr, bool& valueChangedTemp, const std::string& variableName)
	{
		std::string inputText = "None (" + className + ")";
		const auto ptr = valuePtr->get();
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
		int returnValue = DrawInputButton(variableName, inputText, true);
		if (returnValue == 2)
		{
			valuePtr->get() = nullptr;
		}
		if (returnValue == 1)
		{
			if (currentSelectAssetMenu)
				Editor::RemoveMenu(currentSelectAssetMenu.get());

			std::shared_ptr<SelectAssetMenu<T>> selectMenu = Editor::AddMenu<SelectAssetMenu<T>>(true);
			selectMenu->SetActive(true);
			selectMenu->valuePtr = *valuePtr;
			selectMenu->SearchFiles(fileType);
			currentSelectAssetMenu = selectMenu;
			selectMenu->Focus();
		}

		std::shared_ptr <FileReference> ref = nullptr;
		const std::string payloadName = "Files" + std::to_string((int)fileType);
		if (DragDropTarget(payloadName, ref))
		{
			valuePtr->get() = std::dynamic_pointer_cast<T>(ref);
			valueChangedTemp = true;
		}
	}

	template <typename T>
	static void DrawVectorSimpleType(std::reference_wrapper<std::vector<T>>* valuePtr, bool& valueChangedTemp, const std::string& variableName)
	{
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
	}

	template <typename T>
	static void DrawVector(FileType fileType, const std::string& className, std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, bool& valueChangedTemp, const std::string& variableName)
	{
		const size_t vectorSize = valuePtr->get().size();
		const std::string headerName = variableName + "##ListHeader" + std::to_string((uint64_t)valuePtr);
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
				if (ImGui::IsItemClicked())
				{
					if (currentSelectAssetMenu)
						Editor::RemoveMenu(currentSelectAssetMenu.get());

					std::shared_ptr<SelectAssetMenu<T>> selectMenu = Editor::AddMenu<SelectAssetMenu<T>>(true);
					selectMenu->SetActive(true);
					selectMenu->valuePtr = (valuePtr->get()[vectorI]);
					selectMenu->SearchFiles(fileType);
					currentSelectAssetMenu = selectMenu;
					selectMenu->Focus();
				}

				std::shared_ptr <FileReference> ref = nullptr;
				const std::string payloadName = "Files" + std::to_string((int)fileType);
				if (DragDropTarget(payloadName, ref))
				{
					valuePtr->get()[vectorI] = std::dynamic_pointer_cast<T>(ref);
					valueChangedTemp = true;
				}
			}

			const std::string addText = "Add " + className + GenerateItemId();
			if (ImGui::Button(addText.c_str()))
			{
				valuePtr->get().push_back(nullptr);
			}

			const std::string removeText = "Remove " + className + GenerateItemId();
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
		const size_t vectorSize = valuePtr->get().size();
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
			valuePtr->get().push_back(std::weak_ptr<Transform>());
		}

		const std::string removeText = "Remove " + className + GenerateItemId();
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
		const size_t vectorSize = valuePtr->get().size();
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
			valuePtr->get().push_back(std::weak_ptr<Component>());
		}

		const std::string removeText = "Remove " + className + GenerateItemId();
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
			valuePtr->get().push_back(std::weak_ptr<GameObject>());
		}

		const std::string removeText = "Remove " + className + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			if (vectorSize != 0)
			{
				valuePtr->get().erase(valuePtr->get().begin() + vectorSize - 1);
			}
		}
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