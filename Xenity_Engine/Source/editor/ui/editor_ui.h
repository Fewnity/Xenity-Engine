#if defined(_WIN32) || defined(_WIN64)
#pragma once

#include <string>
#include <memory>
#include "../../engine/reflection/reflection.h"

class GameObject;
class Vector2;
class Vector2Int;
class Vector3;
class Vector4;
class Color;
class Reflection;
class Texture;
class FileReference;

class EditorUI
{
public:
	static void Init();
	static void Draw();
	static void NewFrame();
	static void Render();

	static std::string GetPrettyVariableName(std::string variableName);

	static void DrawTextCentered(std::string text);
	static bool DrawInputButton(std::string inputName, std::string text);
	static bool DragDropTarget(std::string name, FileReference*& ref);
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
	static void DrawTreeItem(std::weak_ptr<GameObject> child);
	static void DrawInputTitle(std::string title);
	static void DrawTableInput(std::string inputName, std::string inputId, int columnIndex, float& value);
	static void DrawTableInput(std::string inputName, std::string inputId, int columnIndex, int& value);
	static void DrawReflection(Reflection& reflection);
	static void DrawMap(std::unordered_map<std::string, Variable> myMap);

	static std::string GenerateItemId();
	static bool showProfiler;
	static bool showEditor;
	static bool showEngineSettings;
	static Texture* folderIcon;
	static Texture* fileIcon;
	static Texture* sceneIcon;
	static Texture* imageIcon;

	static float GetUiScale() 
	{
		return uiScale;
	}
private:
	static int uiId;
	static float uiScale;
	static void UpdateUIScale();
};
#endif