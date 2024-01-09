#pragma once

enum Side
{
	Side_None,
	Side_Right,
	Side_Up,
	Side_Forward,
};

#include "menu.h"
#include <engine/vectors/vector2.h>
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
	bool DrawImageButton(bool enabled, std::shared_ptr<Texture> texture);
	void SetButtonColor(bool isSelected);
	void MoveCamera();
	void ProcessTool(std::shared_ptr<Camera>& camera);

	Vector3 GetNearestPoint(const Vector3& linePos1, const Vector3& lineDir1, const Vector3& linePos2, const Vector3& lineDir2);
	void GetMouseRay(Vector3& mouseWorldDir, Vector3& mouseWorldDirNormalized, Vector3& worldCoords, std::shared_ptr<Camera>& camera);
	Side GetNearSide(float camDistance,
		const Vector3& rightClosestPointCam, const Vector3& rightClosestPoint,
		const Vector3& upClosestPointCam, const Vector3& upClosestPoint,
		const Vector3& forwardClosestPointCam, const Vector3& forwardClosestPoint);

	Side DetectSide(float camDistance, const Vector3& objectPosition, const Vector3& camPosition, const Vector3& mouseWorldDirNormalized,
		const Vector3& objectRight, const Vector3& objectUp, const Vector3& objectForward);

	void CheckAllowRotation(float dist, bool& allowRotation, bool isIntersectionGood, Side sideToCheck, const Vector3& intersection);

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
	float finalAngle = 0;
	bool allowRotation = false;
	void Switch2DMode(bool is2D);
	int gridAxis = 0;
};

