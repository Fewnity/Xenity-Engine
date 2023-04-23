#pragma once

#include "../../vectors/vector3.h"
#include <string>

class GameObject;
class Engine;

class EditorUI
{
public:
	static void Init();
	static void DrawProfiler();
	static void DrawInspector();
	static void DrawHierarchy();
	static bool DrawVector3Input(std::string inputName, std::string name1, std::string name2, std::string name3, Vector3 &vector3);
	static void DrawTextCentered(std::string text);

	static void DrawTreeItem(GameObject* child);
	static Engine* engine;
	static int uiId;
};

