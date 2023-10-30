#if defined(_WIN32) || defined(_WIN64)
#pragma once

#include <string>
#include <memory>
#include "../../engine/reflection/reflection.h"
#include "../../engine/file_system/file_reference.h"

class GameObject;
class Vector2;
class Vector2Int;
class Vector3;
class Vector4;
class Color;
class Reflection;
class Texture;
class FileReference;
class ProjectDirectory;
class Component;
class Transform;
class SkyBox;
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

class MultiDragData {
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
	static void SetRoundedCorner(int value);

	static std::string GetPrettyVariableName(std::string variableName);

	static void DrawTextCentered(const std::string& text);
	static int DrawInputButton(const std::string& inputName, const std::string& text, bool addUnbindButton);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<FileReference>& ref);
	static bool DragDropTarget(const std::string& name, std::shared_ptr<Component>& ref, uint64_t typeId);
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
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Collider>& value);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<GameObject>& value);
	static bool DrawInput(const std::string& inputName, std::weak_ptr<Transform>& value);
	static bool DrawInput(const std::string& inputName, std::shared_ptr<SkyBox>& value);

	static int DrawTreeItem(const std::shared_ptr<GameObject>& child);
	static bool DrawTreeItem(ProjectDirectory* projectDir);
	static void DrawInputTitle(const std::string& title);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, float& value);
	static void DrawTableInput(const std::string& inputName, const std::string& inputId, int columnIndex, int& value);
	static bool DrawReflection(Reflection& reflection);
	static bool DrawMap(const std::unordered_map<std::string, ReflectionEntry>& myMap);

	static std::string OpenFolderDialog(const std::string& title);
	static std::string OpenFileDialog(const std::string& title);
	static std::string SaveFileDialog(const std::string& title);

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

private:
	static int uiId;
	static float uiScale;
	static void UpdateUIScale();
};
#endif