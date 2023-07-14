#if defined(_WIN32) || defined(_WIN64)
#pragma once

#include "../../engine/vectors/vector3.h"
#include <string>
#include <memory>

class GameObject;
class Engine;


class EditorUI
{
public:
	static void Init();
	static void NewFrame();
	static void DrawProfiler();
	static void DrawInspector();
	static void DrawHierarchy();
	static void DrawMainMenuBar();
	static bool DrawVector3Input(std::string inputName, std::string name1, std::string name2, std::string name3, Vector3 &vector3);
	static bool DrawFloatInput(std::string inputName, float& value);
	static bool DrawIntInput(std::string inputName, int& value);
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