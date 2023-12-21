#pragma once

enum Side
{
	Side_None,
	Side_Right,
	Side_Up,
	Side_Forward,
};

#include "menu.h"
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <editor/tool_mode.h>
#include <memory>

class GameObject;
class Camera;

class SceneMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool startRotatingCamera = false;
	ToolMode toolMode = Tool_Move;

private:
	void DrawToolWindow();
	void SetButtonColor(bool isSelected);
	void MoveCamera();
	void ProcessTool(std::shared_ptr<Camera>& camera);

	float Dot(const Vector3& a, const Vector3& b);
	Vector3 CrossProduct(const Vector3& a, const Vector3& b);
	Vector3 GetNearestPoint(const Vector3& linePos1, const Vector3& lineDir1, const Vector3& linePos2, const Vector3& lineDir2);

	std::weak_ptr<GameObject> cameraGO;
	std::weak_ptr<Camera> weakCamera;
	bool isLastFrameOpened = false;
	Vector3 oldWorldMousePosition;
	Vector3 worldMousePosition;
	Vector3 startMovePosition;
	Vector3 startObjectValue;
	Side side = Side_None;
	float snapAmount = 1.0f;
	int cameraHandMoveSpeed = 200;

	std::weak_ptr<GameObject> cube1;
	std::weak_ptr<GameObject> cube2;
	std::weak_ptr<GameObject> cube3;
	Vector3 startDragPos = Vector3(0);
	bool mode2D = false;
	void Switch2DMode(bool is2D);
};

