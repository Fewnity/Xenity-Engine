#include "scene_menu.h"
#include <imgui/imgui.h>

#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <engine/graphics/graphics.h>
#include <engine/inputs/input_system.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/transform.h>
#include <editor/editor.h>
#include <engine/time/time.h>
#include <iostream>
#include <engine/debug/debug.h>
#include <engine/tools/shape_spawner.h>

void SceneMenu::Init()
{
	cameraGO = CreateGameObjectEditor("Camera");
	std::shared_ptr<Camera> cameraLock = cameraGO.lock()->AddComponent<Camera>();
	cameraLock->SetNearClippingPlane(0.01f);
	cameraLock->SetFarClippingPlane(500);
	cameraLock->SetProjectionSize(5.0f);
	cameraLock->SetFov(70);
	cameraLock->isEditor = true;
	cameraLock->GetTransform()->SetPosition(Vector3(0, 1, 0));
	weakCamera = cameraLock;
}

ImVec4 normalColor{ 0.5f, 0.5f, 0.5f, 0.5f };
ImVec4 pressedColor{ 0.3f, 0.3f, 0.3f, 0.5f };
ImVec4 hoverColor{ 0.4f, 0.4f, 0.4f, 0.6f };

void SceneMenu::SetButtonColor(bool isSelected)
{
	ImVec4 currentColor = normalColor;
	if (isSelected)
	{
		currentColor = pressedColor;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
}

struct Line2
{
	Vector3 origin;
	Vector3 direction;

	Line2(const Vector3& _origin, const Vector3& _direction)
	{
		origin = _origin;
		direction = _direction;
	}
};

struct Plane
{
	Vector3 normal;

	Plane(const Vector3& _normal)
	{
		normal = _normal;
	}
};

Vector3 intersectionPoint(const Line2& line, const Plane& plane) {
	// Vérifie si la ligne est parallèle au plan
	double dotProduct = line.direction.Dot(plane.normal);
	if (std::abs(dotProduct) < 1e-6) {
		// La ligne est parallèle au plan
		std::cerr << "La ligne est parallèle au plan, aucune intersection." << std::endl;
		return Vector3(0, 0, 0); // ou une autre valeur d'erreur
	}

	// Calcul de la distance le long de la ligne à partir de son origine jusqu'au point d'intersection
	double t = (-line.origin.Dot(plane.normal)) / dotProduct;

	// Calcul du point d'intersection
	Vector3 intersection = line.origin + (line.direction * t);
	return intersection;
}

void SceneMenu::MoveCamera()
{
	// Check user input and camera movement when in the scene menu
	if (InputSystem::GetKeyUp(MOUSE_RIGHT) || InputSystem::GetKeyUp(MOUSE_MIDDLE))
	{
		startRotatingCamera = false;
	}
	if (isFocused)
	{
		// Get camera transform
		std::shared_ptr<Transform> cameraTransform = cameraGO.lock()->GetTransform();
		Vector3 rot = cameraTransform->GetRotation();
		Vector3 pos = cameraTransform->GetPosition();

		if ((ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) && isHovered)
		{
			startRotatingCamera = true;
		}

		// Rotate the camera when dragging the mouse right click
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && startRotatingCamera && !mode2D)
		{
			rot.x += -InputSystem::mouseSpeed.y * 70;
			rot.y += InputSystem::mouseSpeed.x * 70;
		}

		// Move the camera when using keyboard's arrows
		float fwd = 0;
		float up = 0;
		float upWorld = 0;
		float side = 0;

		if (InputSystem::GetKey(UP) || InputSystem::GetKey(Z))
			fwd = -1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(DOWN) || InputSystem::GetKey(S))
			fwd = 1 * Time::GetDeltaTime();

		if (InputSystem::GetKey(A))
			upWorld = 1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(E))
			upWorld = -1 * Time::GetDeltaTime();

		if (InputSystem::GetKey(RIGHT) || InputSystem::GetKey(D))
			side = 1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(LEFT) || InputSystem::GetKey(Q))
			side = -1 * Time::GetDeltaTime();

		if ((InputSystem::GetKey(MOUSE_MIDDLE) || (mode2D && ImGui::IsMouseDown(ImGuiMouseButton_Right))) && startRotatingCamera)
		{
			up += InputSystem::InputSystem::mouseSpeed.y * 1.5f;
			side -= InputSystem::InputSystem::mouseSpeed.x * 1.5f;
		}

		// Move the camera when using the mouse's wheel (Do not use delta time)
		if (isHovered)
		{
			if (!mode2D)
			{
				fwd -= InputSystem::mouseWheel / 15.0f;
			}
			else
			{
				float newSize = weakCamera.lock()->GetProjectionSize() - InputSystem::mouseWheel * weakCamera.lock()->GetProjectionSize() / 10.0f;
				if (newSize < 0.1f)
					newSize = 0.1f;
				weakCamera.lock()->SetProjectionSize(newSize);
			}
		}

		// Apply values
		pos -= cameraTransform->GetForward() * (fwd / 7.0f) * 30;
		pos -= cameraTransform->GetLeft() * (side / 7.0f) * 30;
		pos -= cameraTransform->GetUp() * (up / 7.0f) * 30;
		pos.y -= (upWorld / 7.0f) * 30;

		cameraTransform->SetPosition(pos);
		if (!mode2D)
			cameraTransform->SetRotation(rot);
		else
			cameraTransform->SetRotation(Vector3(0));
	}
}

void SceneMenu::ProcessTool(std::shared_ptr<Camera>& camera)
{
	std::shared_ptr<Transform> cameraTransform = camera->GetTransform();

	//------------------------------------------------------------------------- Detect user inputs to apply tool's behaviour on the selected gameobject

	// Calculate camera matrix without translate
	Vector3 cameraRotation = cameraTransform->GetRotation();
	glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x * -1), glm::vec3(1.0f, 0.0f, 0.0f));
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));

	// Get screen mouse position (inverted)
	glm::vec3 mousePositionGLM = glm::vec3(startAvailableSize.x - mousePosition.x, startAvailableSize.y - (windowSize.y - mousePosition.y), 0.0f); // Invert Y for OpenGL coordinates

	// Get world mouse position (position at the near clipping plane)
	glm::vec3 worldCoords = glm::unProject(mousePositionGLM, cameraModelMatrix, camera->projection, glm::vec4(0, 0, startAvailableSize.x, startAvailableSize.y));
	// Normalise direction if needed
	Vector3 mouseWorldDir = Vector3(worldCoords.x, worldCoords.y, worldCoords.z);
	if (toolMode != Tool_MoveCamera)
		mouseWorldDir = mouseWorldDir.Normalized();
	mouseWorldDir *= -1;
	if (mode2D)
		mouseWorldDir = Vector3(0, 0, -1);

	// Store world mouse position
	oldWorldMousePosition = worldMousePosition;
	worldMousePosition = mouseWorldDir;

	if (toolMode == Tool_MoveCamera) // If the tool mode is the move camera
	{
		// Move the camera if the mouse left button is held
		if (InputSystem::GetKey(MOUSE_LEFT))
		{
			float tempCameraHandMoveSpeed = cameraHandMoveSpeed;
			if (mode2D)
				tempCameraHandMoveSpeed = 1;

			Vector3 newPos = cameraTransform->GetPosition() + (oldWorldMousePosition - worldMousePosition) * tempCameraHandMoveSpeed;
			cameraTransform->SetPosition(newPos);
		}
	}
	else
	{
		std::shared_ptr<GameObject> selectedGO = Editor::GetSelectedGameObject();
		if (selectedGO)
		{
			std::shared_ptr<Transform> selectedGoTransform = selectedGO->GetTransform();

			Vector3 objectPosition = selectedGoTransform->GetPosition();
			Vector3 camPosition = cameraTransform->GetPosition();
			if (mode2D)
				camPosition = Vector3(-worldCoords.x, -worldCoords.y, -worldCoords.z) + camPosition;

			Vector3 objectRight = Vector3(1, 0, 0);
			Vector3 objectUp = Vector3(0, 1, 0);
			Vector3 objectForward = Vector3(0, 0, 1);
			if (!Editor::isToolLocalMode)
			{
				// Get selected gameObject directions
				objectRight = selectedGoTransform->GetRight();
				objectUp = selectedGoTransform->GetUp();
				objectForward = selectedGoTransform->GetForward();
			}

			float camDistance = Vector3::Distance(objectPosition, camPosition);
			if (InputSystem::GetKeyDown(MOUSE_LEFT))
			{
				// Get start object value
				if (toolMode == Tool_Move)
					startObjectValue = selectedGoTransform->GetPosition();
				else if (toolMode == Tool_Rotate)
					startObjectValue = selectedGoTransform->GetRotation();
				else if (toolMode == Tool_Scale)
					startObjectValue = selectedGoTransform->GetLocalScale();

				// Get nearest lines interaction points on the object's direction axis
				Vector3 rightClosestPoint = GetNearestPoint(objectPosition, objectRight, camPosition, mouseWorldDir);
				Vector3 upClosestPoint = GetNearestPoint(objectPosition, objectUp, camPosition, mouseWorldDir);
				Vector3 forwardClosestPoint = GetNearestPoint(objectPosition, objectForward, camPosition, mouseWorldDir);

				// Get nearest lines interaction points on the camera's direction axis
				Vector3 rightClosestPointCam = GetNearestPoint(camPosition, mouseWorldDir, objectPosition, objectRight);
				Vector3 upClosestPointCam = GetNearestPoint(camPosition, mouseWorldDir, objectPosition, objectUp);
				Vector3 forwardClosestPointCam = GetNearestPoint(camPosition, mouseWorldDir, objectPosition, objectForward);

				if (cube1.lock())
				{
					cube1.lock()->GetTransform()->SetPosition(rightClosestPoint);
					cube2.lock()->GetTransform()->SetPosition(upClosestPoint);
					cube3.lock()->GetTransform()->SetPosition(forwardClosestPoint);
				}

				Vector3 rightDirPoint = (rightClosestPoint - objectPosition).Normalize();
				Vector3 upDirPoint = (upClosestPoint - objectPosition).Normalize();
				Vector3 forwardDirPoint = (forwardClosestPoint - objectPosition).Normalize();
				float rightRightSide = rightDirPoint.Dot(objectRight);
				float upRightSide = upDirPoint.Dot(objectUp);
				float forwardRightSide = forwardDirPoint.Dot(objectForward);

				//----------------------------------- Check if the mouse is on the arrow

				// Check if the side of the arrows is correct

				bool isRightTooFar = abs(objectPosition.x - rightClosestPoint.x) > 1 * camera->GetProjectionSize() / 5.0f;
				bool isUpTooFar = abs(objectPosition.y - upClosestPoint.y) > 1 * camera->GetProjectionSize() / 5.0f;
				bool isForwardTooFar = abs(objectPosition.z - forwardClosestPoint.z) > 1 * camera->GetProjectionSize() / 5.0f;
				if (!mode2D)
				{
					isRightTooFar = abs(objectPosition.x - rightClosestPoint.x) > 1 * camDistance / 8.0f;
					isUpTooFar = abs(objectPosition.y - upClosestPoint.y) > 1 * camDistance / 8.0f;
					isForwardTooFar = abs(objectPosition.z - forwardClosestPoint.z) > 1 * camDistance / 8.0f;
				}

				// Get the distance between the two lines nearest points
				float rightPointsDist = Vector3::Distance(rightClosestPointCam, rightClosestPoint);
				float upPointsDist = Vector3::Distance(upClosestPointCam, upClosestPoint);
				float forwardPointsDist = Vector3::Distance(forwardClosestPointCam, forwardClosestPoint);
				if (isnan(rightPointsDist))
					rightPointsDist = 99999999.0f;
				if (isnan(upPointsDist))
					upPointsDist = 99999999.0f;
				if (isnan(forwardPointsDist))
					forwardPointsDist = 99999999.0f;

				// Detect the arrow
				Side nearSide = Side_None;
				if (rightPointsDist < upPointsDist && rightPointsDist < forwardPointsDist)
				{
					nearSide = Side_Right;
				}
				else if (upPointsDist < rightPointsDist && upPointsDist < forwardPointsDist)
				{
					nearSide = Side_Up;
				}
				else if (forwardPointsDist < upPointsDist && forwardPointsDist < rightPointsDist)
				{
					nearSide = Side_Forward;
				}

				side = Side_None;
				if (mode2D)
				{
					float size = camera->GetProjectionSize() * 5.0f;
					if (rightRightSide >= 0.95f && nearSide == Side_Right && !isRightTooFar && rightPointsDist / size <= 0.11f)
					{
						side = Side_Right;
					}
					else if (upRightSide >= 0.95f && nearSide == Side_Up && !isUpTooFar && upPointsDist / size <= 0.11f)
					{
						side = Side_Up;
					}
					else if (forwardRightSide >= 0.95f && nearSide == Side_Forward && !isForwardTooFar && forwardPointsDist / size <= 0.11f)
					{
						side = Side_Forward;
					}
				}
				else
				{
					if (rightRightSide >= 0.95f && nearSide == Side_Right && !isRightTooFar && rightPointsDist / camDistance <= 0.02f)
					{
						side = Side_Right;
					}
					else if (upRightSide >= 0.95f && nearSide == Side_Up && !isUpTooFar && upPointsDist / camDistance <= 0.02f)
					{
						side = Side_Up;
					}
					else if (forwardRightSide >= 0.95f && nearSide == Side_Forward && !isForwardTooFar && forwardPointsDist / camDistance <= 0.02f)
					{
						side = Side_Forward;
					}
				}
			}

			if (InputSystem::GetKey(MOUSE_LEFT) && (side != Side_None || toolMode == Tool_Rotate))
			{
				Vector3 planeNormal= Vector3(0, 1, 0); // Face UP
				//Vector3 planeNormal(0, 0, 1); // Red arrow
				Plane plane = Plane(planeNormal);
				Line2 line = Line2(cameraTransform->GetPosition() - Editor::GetSelectedGameObject()->GetTransform()->GetPosition(), mouseWorldDir); // Normalisation de la direction
				Vector3 intersection = intersectionPoint(line, plane);

				//std::cout << "Point d'intersection : (" << intersection.x << ", " << intersection.y << ", " << intersection.z << ")" << std::endl;
				Vector3 objectDir = Vector3(0);

				// Select the right value
				if (side == Side_Right)
					objectDir = objectRight;
				else if (side == Side_Up)
					objectDir = objectUp;
				else if (side == Side_Forward)
					objectDir = objectForward;

				// Get the closest point on the object axis
				Vector3 closestPoint = GetNearestPoint(camPosition, mouseWorldDir, objectPosition, objectDir);
				if (InputSystem::GetKeyDown(MOUSE_LEFT))
				{
					startMovePosition = closestPoint;
					startDragPos = intersection;
					finalAngle = 0;
					float dist = Vector3::Distance(Vector3(0), intersection) / camDistance;
					if (dist >= 0.10f && dist <= 0.13f) 
					{
						allowRotation = true;
					}
					else
					{
						allowRotation = false;
					}
				}

				// Calculate the value offset
				Vector3 objectOffset = (closestPoint - startMovePosition);
				if (toolMode == Tool_Scale)
					objectOffset *= 4;

				// Snap values if needed
				if (InputSystem::GetKey(LEFT_CONTROL))
				{
					objectOffset.x = (int)(objectOffset.x / snapAmount) * snapAmount;
					objectOffset.y = (int)(objectOffset.y / snapAmount) * snapAmount;
					objectOffset.z = (int)(objectOffset.z / snapAmount) * snapAmount;
				}

				//Set object position/rotation/scale
				if (toolMode == Tool_Move)
				{
					selectedGoTransform->SetPosition(startObjectValue + objectOffset);
				}
				else if (toolMode == Tool_Rotate && allowRotation)
				{
					if (intersection != startDragPos)
					{
						float angle = startDragPos.Dot(intersection) / (startDragPos.Magnitude() * intersection.Magnitude());
						float crossProduct = startDragPos.x * intersection.z - startDragPos.z * intersection.x;
						startDragPos = intersection;
						if (!isnan(acos(angle) * 180.0 / M_PI))
						{
							if (crossProduct < 0)
								finalAngle += (acos(angle) * 180.0f / M_PI);
							else
								finalAngle -= (acos(angle) * 180.0f / M_PI);
							selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, finalAngle, 0));
						}
						//std::cout << "angle: " << angle << std::endl;
						//if (angle == 1)
						//	std::cout << "salut" << std::endl;
						//std::cout << "acos(angle): " << acosf(angle) << std::endl;
						//std::cout << "(acos(angle) * 180.0f / M_PI): " << (acos(angle) * 180.0f / M_PI) << std::endl;
					}
				}
				else if (toolMode == Tool_Scale) 
				{
					//selectedGoTransform->SetLocalScale(startObjectValue + objectOffset);
				}
			}
		}
	}
}

float SceneMenu::Dot(const Vector3& a, const Vector3& b)
{
	float f;
	f = a.x * b.x + a.y * b.y + a.z * b.z;
	return f;
}

Vector3 SceneMenu::CrossProduct(const Vector3& a, const Vector3& b)
{
	Vector3 c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = -(a.x * b.z - a.z * b.x);
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

Vector3 SceneMenu::GetNearestPoint(const Vector3& linePos1, const Vector3& lineDir1, const Vector3& linePos2, const Vector3& lineDir2)
{
	Vector3 ClosestPoint;
	if (lineDir2 != Vector3(0))
	{
		Vector3 V = linePos2 - linePos1;

		float dotD1D1 = Dot(lineDir1, lineDir1);
		float dotD2D2 = Dot(lineDir2, lineDir2);
		float dotD1D2 = Dot(lineDir1, lineDir2);

		float a = Dot(lineDir1, V);
		float b = Dot(lineDir2, V);
		float c = Dot(lineDir1, lineDir2);
		float d = Dot(lineDir2, lineDir2);

		float s = (a - b * dotD1D2 / dotD2D2) / (dotD1D1 - dotD1D2 * dotD1D2 / dotD2D2);
		float t = (b - c * s) / d;

		ClosestPoint = linePos2 + t * (lineDir2 * -1);
	}
	return ClosestPoint;
}

void SceneMenu::Switch2DMode(bool is2D)
{
	mode2D = is2D;
	if (mode2D)
	{
		weakCamera.lock()->SetProjectionType(ProjectionTypes::Orthographic);
	}
	else
	{
		weakCamera.lock()->SetProjectionType(ProjectionTypes::Perspective);
	}
}

void SceneMenu::Draw()
{
	if (InputSystem::GetKeyDown(R))
	{
		cube1 = ShapeSpawner::SpawnCube();
		cube2 = ShapeSpawner::SpawnCube();
		cube3 = ShapeSpawner::SpawnCube();

		cube1.lock()->GetTransform()->SetLocalScale(Vector3(0.05));
		cube2.lock()->GetTransform()->SetLocalScale(Vector3(0.15));
		cube3.lock()->GetTransform()->SetLocalScale(Vector3(0.3));
	}

	std::shared_ptr<Camera> camera = weakCamera.lock();
	Vector2Int frameBufferSize = Vector2Int(0, 0);
	if (camera)
	{
		frameBufferSize.x = camera->GetWidth();
		frameBufferSize.y = camera->GetHeight();
	}

	// Generate tab name
	std::string windowName = "Scene";
	if (isLastFrameOpened)
		windowName += " " + std::to_string(frameBufferSize.x) + "x" + std::to_string(frameBufferSize.y);
	windowName += "###Scene" + std::to_string(id);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool isOpen = true;
	bool visible = ImGui::Begin(windowName.c_str(), &isOpen, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse);
	isLastFrameOpened = visible;
	if (visible)
	{
		OnStartDrawing();

		ImVec2 startCursorPos = ImGui::GetCursorPos();
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
		{
			ImGui::SetWindowFocus();
			isFocused = true;
		}
		if (camera)
		{
			MoveCamera();

			camera->ChangeFrameBufferSize(startAvailableSize);
			ImGui::Image((ImTextureID)camera->secondFramebufferTexture, ImVec2(startAvailableSize.x, startAvailableSize.y), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::IsItemHovered())
			{
				if (InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(F))
				{
					if (Editor::GetSelectedGameObject() != nullptr)
					{
						Editor::GetSelectedGameObject()->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + camera->GetTransform()->GetForward() * 2);
					}
				}
				else if (InputSystem::GetKeyDown(F))
				{
					if (Editor::GetSelectedGameObject() != nullptr)
					{
						Vector3 dir = (camera->GetTransform()->GetPosition() - Editor::GetSelectedGameObject()->GetTransform()->GetPosition()).Normalized();
						camera->GetTransform()->SetPosition(Editor::GetSelectedGameObject()->GetTransform()->GetPosition() + dir * 2);
						camera->GetTransform()->SetRotation(Vector3::LookAt(camera->GetTransform()->GetPosition(), Editor::GetSelectedGameObject()->GetTransform()->GetPosition()));
					}
				}

				ProcessTool(camera);
			}
		}

		// List tool modes
		ImGui::SetCursorPos(startCursorPos);
		DrawToolWindow();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
	ImGui::PopStyleVar();

	if (!isOpen)
	{
		Editor::RemoveMenu(this);
	}
}

void SceneMenu::DrawToolWindow()
{
	if (ImGui::CollapsingHeader("Tool modes", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		SetButtonColor(toolMode == Tool_MoveCamera);
		if (ImGui::Button("Move camera"))
		{
			toolMode = Tool_MoveCamera;
		}
		ImGui::PopStyleColor(3);
		SetButtonColor(toolMode == Tool_Move);
		if (ImGui::Button("Move"))
		{
			toolMode = Tool_Move;
		}
		ImGui::PopStyleColor(3);
		SetButtonColor(toolMode == Tool_Rotate);
		if (ImGui::Button("Rotate"))
		{
			toolMode = Tool_Rotate;
		}
		ImGui::PopStyleColor(3);
		SetButtonColor(toolMode == Tool_Scale);
		if (ImGui::Button("Scale"))
		{
			toolMode = Tool_Scale;
		}
		if (ImGui::Button("2D"))
		{
			Switch2DMode(!mode2D);
		}
		if (Editor::isToolLocalMode) {
			if (ImGui::Button("Local"))
			{
				Editor::isToolLocalMode = false;
			}
		}
		else {
			if (ImGui::Button("World"))
			{
				Editor::isToolLocalMode = true;
			}
		}
		ImGui::PopStyleColor(3);
	}
}
