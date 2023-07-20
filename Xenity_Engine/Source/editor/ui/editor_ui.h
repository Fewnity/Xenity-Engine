#if defined(_WIN32) || defined(_WIN64)
#pragma once

#include <string>
#include <memory>

class GameObject;
class Vector2;
class Vector2Int;
class Vector3;
class Vector4;
class Color;
class Reflection;

class EditorUI
{
public:
	static void Init();
	static void Draw();


private:
	static void NewFrame();
	static void Render();
	static void DrawProfiler();
	static void DrawInspector();
	static void DrawHierarchy();
	static void DrawMainMenuBar();
	static void DrawEngineSettings();

	static void DrawTextCentered(std::string text);
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
	static int uiId;
	static void DrawInputTitle(std::string title);
	static void DrawTableInput(std::string inputName, std::string inputId, int columnIndex, float& value);
	static void DrawTableInput(std::string inputName, std::string inputId, int columnIndex, int& value);
	static std::string GenerateItemId();
	static void DrawReflection(Reflection& reflection);

	static float nextFpsUpdate;
	static float lastFps;
	static bool showProfiler;
	static bool showEditor;
	static bool showEngineSettings;

};
#endif