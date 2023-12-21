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
	//cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x * -1), glm::vec3(1.0f, 0.0f, 0.0f));
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));

	// Get screen mouse position (inverted)
	glm::vec3 mousePositionGLM = glm::vec3(startAvailableSize.x - mousePosition.x, startAvailableSize.y - (windowSize.y - mousePosition.y), 0.0f); // Invert Y for OpenGL coordinates

	// Get world mouse position (position at the near clipping plane)
	glm::vec3 worldCoords = glm::unProject(mousePositionGLM, cameraModelMatrix, camera->projection, glm::vec4(0, 0, startAvailableSize.x, startAvailableSize.y));
	//std::cout << "Pos " << worldCoords.x << " " << worldCoords.y << " " << worldCoords.z << std::endl;
	// Normalise direction if needed
	Vector3 mouseWorldDir = Vector3(worldCoords.x, worldCoords.y, worldCoords.z);
	if (toolMode != Tool_MoveCamera)
		mouseWorldDir = mouseWorldDir.Normalized();
	mouseWorldDir *= -1;
	if (mode2D)
		mouseWorldDir = Vector3(0, 0, -1);

	//std::cout << "Dir " << mouseWorldDir.x << " " << mouseWorldDir.y << " " << mouseWorldDir.z << std::endl;

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

			Vector3 camPos = cameraTransform->GetPosition();
			if (mode2D)
				camPos = Vector3(-worldCoords.x, -worldCoords.y, -worldCoords.z) + camPos;
			Vector3 objectPos = selectedGoTransform->GetPosition();

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
				Vector3 rightClosestPoint = GetNearestPoint(objectPos, objectRight, camPos, mouseWorldDir);
				Vector3 upClosestPoint = GetNearestPoint(objectPos, objectUp, camPos, mouseWorldDir);
				Vector3 forwardClosestPoint = GetNearestPoint(objectPos, objectForward, camPos, mouseWorldDir);

				// Get nearest lines interaction points on the camera's direction axis
				Vector3 rightClosestPointCam = GetNearestPoint(camPos, mouseWorldDir, objectPos, objectRight);
				Vector3 upClosestPointCam = GetNearestPoint(camPos, mouseWorldDir, objectPos, objectUp);
				Vector3 forwardClosestPointCam = GetNearestPoint(camPos, mouseWorldDir, objectPos, objectForward);

				if (cube1.lock())
				{
					cube1.lock()->GetTransform()->SetPosition(rightClosestPoint);
					cube2.lock()->GetTransform()->SetPosition(upClosestPoint);
					cube3.lock()->GetTransform()->SetPosition(forwardClosestPoint);
				}

				// Get the distance between the gameobject and the closest point of the camera's direction
				float rightDist = Vector3::Distance(rightClosestPointCam, objectPos);
				float upDist = Vector3::Distance(upClosestPointCam, objectPos);
				float forwardDist = Vector3::Distance(forwardClosestPointCam, objectPos);
				/*std::cout << "rightDist: " << rightDist << std::endl;
				std::cout << "upDist: " << upDist << std::endl;
				std::cout << "forwardDist: " << forwardDist << std::endl;*/

				// Get the distance between the two lines nearest points
				float rightDist2 = Vector3::Distance(rightClosestPointCam, rightClosestPoint);
				float upDist2 = Vector3::Distance(upClosestPointCam, upClosestPoint);
				float forwardDist2 = Vector3::Distance(forwardClosestPointCam, forwardClosestPoint);
				/*std::cout << "rightDist2: " << rightDist2 << std::endl;
				std::cout << "upDist2: " << upDist2 << std::endl;
				std::cout << "forwardDist2: " << forwardDist2 << std::endl;*/

				float camDist = Vector3::Distance(objectPos, camPos);

				//----------------------------------- Check if the mouse is on the arrow

				// Check if the side of the arrows is correct
				bool isRightGood = objectPos.x < rightClosestPoint.x;
				if (objectRight.x < 0)
					isRightGood = objectPos.x > rightClosestPoint.x;

				bool isUpGood = objectPos.y < upClosestPoint.y;
				if (objectUp.y < 0)
					isUpGood = objectPos.y > upClosestPoint.y;

				bool isForwardGood = objectPos.z < forwardClosestPoint.z;
				if (objectForward.z < 0)
					isForwardGood = objectPos.z > forwardClosestPoint.z;

				bool isRightTooFar = abs(objectPos.x - rightClosestPoint.x) > 1 * camera->GetProjectionSize() / 5.0f;
				bool isUpTooFar = abs(objectPos.y - upClosestPoint.y) > 1 * camera->GetProjectionSize() / 5.0f;
				bool isForwardTooFar = abs(objectPos.z - forwardClosestPoint.z) > 1 * camera->GetProjectionSize() / 5.0f;
				if (!mode2D)
				{
					isRightTooFar = abs(objectPos.x - rightClosestPoint.x) > 1 * camDist / 8.0f;
					isUpTooFar = abs(objectPos.y - upClosestPoint.y) > 1 * camDist / 8.0f;
					isForwardTooFar = abs(objectPos.z - forwardClosestPoint.z) > 1 * camDist / 8.0f;
				}
				/*std::cout << "isRightGood: " << isRightGood << std::endl;
				std::cout << "isUpGood: " << isUpGood << std::endl;
				std::cout << "isForwardGood: " << isForwardGood << std::endl;*/

				if (isnan(rightDist2))
					rightDist2 = 99999999;
				if (isnan(upDist2))
					upDist2 = 99999999;
				if (isnan(forwardDist2))
					forwardDist2 = 99999999;

				// Detect the arrow
				side = Side_None;
				Side nearSide = Side_None;
				if (rightDist2 < upDist2 && rightDist2 < forwardDist2)
				{
					nearSide = Side_Right;
				}
				if (upDist2 < rightDist2 && upDist2 < forwardDist2)
				{
					nearSide = Side_Up;
				}
				if (forwardDist2 < upDist2 && forwardDist2 < rightDist2)
				{
					nearSide = Side_Forward;
				}

				if (mode2D)
				{
					if (isRightGood && nearSide == Side_Right && !isRightTooFar && rightDist2 / camera->GetProjectionSize() * 5.0f <= 0.11f)
					{
						side = Side_Right;
					}
					else if (isUpGood && nearSide == Side_Up && !isUpTooFar && upDist2 / camera->GetProjectionSize() * 5.0f <= 0.11f)
					{
						side = Side_Up;
					}
					else if (isForwardGood && nearSide == Side_Forward && !isForwardTooFar && forwardDist2 / camera->GetProjectionSize() * 5.0f <= 0.11f)
					{
						side = Side_Forward;
					}
				}
				else
				{
					if (isRightGood && nearSide == Side_Right && !isRightTooFar && rightDist2 / camDist <= 0.02f)
					{
						side = Side_Right;
					}
					else if (isUpGood && nearSide == Side_Up && !isUpTooFar && upDist2 / camDist <= 0.02f)
					{
						side = Side_Up;
					}
					else if (isForwardGood && nearSide == Side_Forward && !isForwardTooFar && forwardDist2 / camDist <= 0.02f)
					{
						side = Side_Forward;
					}
				}
			}

			if (InputSystem::GetKey(MOUSE_LEFT) && side != Side_None)
			{
				Vector3 objectDir = Vector3(0);

				// Select the right value
				if (side == Side_Right)
					objectDir = objectRight;
				else if (side == Side_Up)
					objectDir = objectUp;
				else if (side == Side_Forward)
					objectDir = objectForward;

				// Get the closest point on the object axis
				Vector3 closestPoint = GetNearestPoint(camPos, mouseWorldDir, objectPos, objectDir);
				if (InputSystem::GetKeyDown(MOUSE_LEFT))
				{
					startMovePosition = closestPoint;
				}

				// Calculate the value offset
				Vector3 objectOffset = (closestPoint - startMovePosition);

				if (toolMode == Tool_Scale)
					objectOffset *= 4;
				else if (toolMode == Tool_Rotate)
					objectOffset *= 10;

				// Snap values if needed
				if (InputSystem::GetKey(LEFT_CONTROL))
				{
					objectOffset.x = (int)(objectOffset.x / snapAmount) * snapAmount;
					objectOffset.y = (int)(objectOffset.y / snapAmount) * snapAmount;
					objectOffset.z = (int)(objectOffset.z / snapAmount) * snapAmount;
				}

				//Set object position/rotation/scale
				if (toolMode == Tool_Move)
					selectedGoTransform->SetPosition(startObjectValue + objectOffset);
				else if (toolMode == Tool_Rotate)
				{
					/*if (side == 0)
						//selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(Vector3::Distance(Vector3(0),  objectOffset), 0,0));
						//objectDir = objectRight;
					else if (side == 1)
						//selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, Vector3::Distance(Vector3(0), objectOffset), 0));
						//objectDir = objectUp;
					else if (side == 2)
						//selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, 0, Vector3::Distance(Vector3(0), objectOffset)));
						//objectDir = objectForward;*/

					if (side == Side_Right)
						selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(Vector3::Distance(Vector3(0), objectOffset), 0, 0));
					else if (side == Side_Up)
						selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, Vector3::Distance(Vector3(0), objectOffset), 0));
					else if (side == Side_Forward)
						selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, 0, Vector3::Distance(Vector3(0), objectOffset)));
					//std::cout << side << std::endl;
					//selectedGoTransform->SetLocalRotation(startObjectValue + objectOffset);
				}
				else if (toolMode == Tool_Scale)
					selectedGoTransform->SetLocalScale(startObjectValue + objectOffset);
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
