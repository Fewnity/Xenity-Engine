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
	Icon_Count,
};

class EditorUI
{
public:
	static void Init();
	static void Draw();
	static void NewFrame();
	static void Render();
	static void SetRoundedCorner(int value);

	static std::string GetPrettyVariableName(std::string variableName);

	static void DrawTextCentered(std::string text);
	static int DrawInputButton(std::string inputName, std::string text, bool addUnbindButton);
	static bool DragDropTarget(std::string name, std::shared_ptr <FileReference>& ref);
	static bool DragDropTarget(std::string name, Component*& ref);
	static bool DragDropTarget(std::string name, GameObject*& ref);
	static bool DragDropTarget(std::string name, Transform*& ref);

	static bool DrawInput(std::string inputName, Color& value);
	static bool DrawInput(std::string inputName, Vector2& value);
	static bool DrawInput(std::string inputName, Vector2Int& value);
	static bool DrawInput(std::string inputName, Vector3 & value);
	static bool DrawInput(std::string inputName, Vector4& value);
	static bool DrawInput(std::string inputName, float& value);
	static bool DrawInput(std::string inputName, double& value);
	static bool DrawInput(std::string inputName, std::string& value);
	static bool DrawInput(std::string inputName, int& value);
	static bool DrawInput(std::string inputName, bool& value);
	static bool DrawInput(std::string inputName, std::weak_ptr<Component>& value);
	static bool DrawInput(std::string inputName, std::weak_ptr<GameObject>& value);
	static bool DrawInput(std::string inputName, std::weak_ptr<Transform>& value);
	static bool DrawTreeItem(std::weak_ptr<GameObject> child);
	static bool DrawTreeItem(ProjectDirectory* projectDir);
	static void DrawInputTitle(std::string title);
	static void DrawTableInput(std::string inputName, std::string inputId, int columnIndex, float& value);
	static void DrawTableInput(std::string inputName, std::string inputId, int columnIndex, int& value);
	static bool DrawReflection(Reflection& reflection);
	static bool DrawMap(std::unordered_map<std::string, Variable> myMap);

	static std::string OpenFolderDialog(std::string title);
	static std::string OpenFileDialog(std::string title);
	static std::string SaveFileDialog(std::string title);

	static std::string GenerateItemId();
	static bool showProfiler;
	static bool showEditor;
	static bool showEngineSettings;
	static bool showProjectsSettings;
	static std::vector<std::shared_ptr<Texture>> icons;

	static float GetUiScale() 
	{
		return uiScale;
	}
private:
	static int uiId;
	static float uiScale;
	static void UpdateUIScale();
	template <typename T>
	static void DrawFileReference(FileType fileType, std::string className, std::reference_wrapper<std::shared_ptr<T>>* valuePtr, bool& valueChangedTemp, std::string& variableName);
};
#endif