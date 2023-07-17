#if defined(_WIN32) || defined(_WIN64)
#pragma once

#include <string>
#include <memory>

class GameObject;
class Engine;
class Vector2;
class Vector2Int;
class Vector3;
class Vector4;
class Color;

class EditorUI
{
public:
	static void Init();
	static void NewFrame();
	static void DrawProfiler();
	static void DrawInspector();
	static void DrawHierarchy();
	static void DrawMainMenuBar();

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

	static void DrawTextCentered(std::string text);

	static void DrawTreeItem(std::weak_ptr<GameObject> child);
	static void Render();

	static Engine* engine;
	static int uiId;
private:
	static float nextFpsUpdate;
	static float lastFps;
};
#endif